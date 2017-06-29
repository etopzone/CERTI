// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// ----------------------------------------------------------------------------

#include "RTIG.hh"
// #include <config.h>

#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <sys/select.h>

// #include "NM_Classes.hh"
#include "FedTimeD.hh"
#include "PrettyDebug.hh"
#include "Socket.hh"
#include <NetworkMessage.hh>

#ifdef _WIN32
#include <signal.h>
// #else
// #include <unistd.h>
#endif

// using std::cout;
// using std::endl;
// using std::cerr;

namespace {
static constexpr auto defaultTcpPort = PORT_TCP_RTIG;
static constexpr auto tcpPortEnvironmentVariable = "CERTI_TCP_PORT";

static constexpr auto defaultUdpPort = PORT_UDP_RTIG;
static constexpr auto udpPortEnvironmentVariable = "CERTI_UDP_PORT";
}

namespace certi {
namespace rtig {

static PrettyDebug D("RTIG", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);

bool RTIG::terminate = false;

RTIG::RTIG()
    : my_tcpPort(inferTcpPort())
    , my_udpPort(inferUdpPort())
    , my_verboseLevel(0)
    , my_listeningIPAddress(0)
    , my_federationHandles(1)
    , my_socketServer(&my_tcpSocketServer, &my_udpSocketServer)
    , my_auditServer(RTIG_AUDIT_FILENAME)
    , my_federations(my_socketServer, my_auditServer)
{
    my_federations.setVerboseLevel(my_verboseLevel);

    for (int i = pdAnswer; i < pdLast; ++i) {
        D.enableDebugLevel(static_cast<pdDebugLevel>(i));
        G.enableDebugLevel(static_cast<pdDebugLevel>(i));
    }

    my_NM_msgBufSend.reset();
    my_NM_msgBufReceive.reset();
}

RTIG::~RTIG()
{
    my_tcpSocketServer.close();
    my_udpSocketServer.close();

    std::cout << std::endl << "Stopping RTIG" << std::endl;
}

void RTIG::execute() throw(NetworkError)
{
    // create TCP and UDP connections for the RTIG server
    createSocketServers();

    if (my_verboseLevel > 0) {
        std::cout << "CERTI RTIG up and running ..." << std::endl;
    }
    terminate = false;

    int result;
    fd_set fd;
    Socket* link;
    while (!terminate) {
// Initialize fd_set structure with all opened sockets.
#if _WIN32

        result = 0; // Wait for an incoming message.
        while (!result) {
            int test;

            FD_ZERO(&fd);
            FD_SET(tcpSocketServer.returnSocket(), &fd);

            int highest_fd = socketServer.addToFDSet(&fd);
            int server_socket = tcpSocketServer.returnSocket();

            // typedef struct timeval {  long tv_sec;  long tv_usec; }
            timeval watchDog;
            watchDog.tv_sec = 0;
            watchDog.tv_usec = 50000L;

            highest_fd = server_socket > highest_fd ? server_socket : highest_fd;

            result = select(highest_fd + 1, &fd, NULL, NULL, &watchDog);
            if (result < 0) {
                test = WSAGetLastError();
            }

            if (terminate) {
                break;
            }
        }
        if (terminate) {
            break;
        }

        if ((result == -1) && (WSAGetLastError() == WSAEINTR)) {
            break;
        }
#else
        FD_ZERO(&fd);
        FD_SET(my_tcpSocketServer.returnSocket(), &fd);

        int fd_max = my_socketServer.addToFDSet(&fd);
        fd_max = std::max(my_tcpSocketServer.returnSocket(), fd_max);

        result = 0; // Wait for an incoming message.
        result = select(fd_max + 1, &fd, nullptr, nullptr, nullptr);

        if ((result == -1) && (errno == EINTR)) {
            break;
        }
#endif

        // Is it a message from an already opened connection?
        link = my_socketServer.getActiveSocket(&fd);
        if (link) {
            Debug(D, pdCom) << "Incoming message on socket " << link->returnSocket() << std::endl;

            try {
                do {
                    link = processIncomingMessage(link);
                    if (!link) {
                        break;
                    }
                } while (link->isDataReady());
            }
            catch (NetworkError& e) {
                if (!e.reason().empty()) {
                    Debug(D, pdExcept) << "Catching Network Error, reason: " << e.reason() << std::endl;
                }
                else {
                    Debug(D, pdExcept) << "Catching Network Error, no reason string" << std::endl;
                }
                std::cout << "RTIG dropping client connection " << link->returnSocket() << '.' << std::endl;
                closeConnection(link, true);
                link = nullptr;
            }
        }

        // Or on the server socket ?
        if (FD_ISSET(my_tcpSocketServer.returnSocket(), &fd)) {
            Debug(D, pdCom) << "New client" << std::endl;
            openConnection();
        }
    }
}

void RTIG::signalHandler(int sig)
{
    Debug(D, pdError) << "Received Signal: " << sig << std::endl;

    if (sig == SIGINT) {
        terminate = true;
    }
#ifndef _WIN32
    if (sig == SIGPIPE) {
        std::cout << "Ignoring 'Broken pipe' signal" << std::endl;
    }
#endif
}

void RTIG::setVerboseLevel(const int level)
{
    my_verboseLevel = level;
    my_federations.setVerboseLevel(level);
}

void RTIG::setListeningIPAddress(const std::string& hostName) throw(NetworkError)
{
    Socket::host2addr(hostName, my_listeningIPAddress);
}

void RTIG::createSocketServers()
{
    if (my_listeningIPAddress == 0) {
        // default case on all network interfaces
        my_udpSocketServer.createServer(my_udpPort);
        my_tcpSocketServer.createServer(my_tcpPort);
    }
    else {
        //  1) listen on interface specified on the command line
        my_udpSocketServer.createServer(my_udpPort, my_listeningIPAddress);
        my_tcpSocketServer.createServer(my_tcpPort, my_listeningIPAddress);
    }
}

Socket* RTIG::chooseProcessingMethod(Socket* link, NetworkMessage* msg)
{
    Debug(G, pdGendoc) << "enter RTIG::chooseProcessingMethod type (" << msg->getMessageName() << ")" << std::endl;

    // This may throw a security error.
    if (msg->getMessageType() != NetworkMessage::DESTROY_FEDERATION_EXECUTION) {
        my_socketServer.checkMessage(link->returnSocket(), msg);
    }

    switch (msg->getMessageType()) {
    case NetworkMessage::MESSAGE_NULL:
        Debug(D, pdDebug) << "Message Null" << std::endl;
        my_auditServer.setLevel(0);
        processMessageNull(msg, false);
        break;

    case NetworkMessage::MESSAGE_NULL_PRIME:
        Debug(D, pdDebug) << "Message Null" << std::endl;
        my_auditServer.setLevel(0);
        processMessageNullPrime(static_cast<NM_Message_Null_Prime*>(msg));
        break;

    case NetworkMessage::UPDATE_ATTRIBUTE_VALUES:
        Debug(D, pdDebug) << "UpdateAttributeValue" << std::endl;
        my_auditServer.setLevel(1);
        processUpdateAttributeValues(link, static_cast<NM_Update_Attribute_Values*>(msg));
        break;

    case NetworkMessage::SEND_INTERACTION:
        Debug(D, pdTrace) << "send interaction" << std::endl;
        my_auditServer.setLevel(2);
        processSendInteraction(link, static_cast<NM_Send_Interaction*>(msg));
        break;

    case NetworkMessage::CLOSE_CONNEXION:
        Debug(D, pdTrace) << "Close connection: " << link->returnSocket() << std::endl;
        my_auditServer.setLevel(9);
        my_auditServer << "Socket " << int(link->returnSocket());
        closeConnection(link, false);
        link = NULL;
        break;

    case NetworkMessage::CREATE_FEDERATION_EXECUTION:
        my_auditServer.setLevel(9);
        processCreateFederation(link, static_cast<NM_Create_Federation_Execution*>(msg));
        break;

    case NetworkMessage::DESTROY_FEDERATION_EXECUTION:
        my_auditServer.setLevel(9);
        processDestroyFederation(link, static_cast<NM_Destroy_Federation_Execution*>(msg));
        break;

    case NetworkMessage::JOIN_FEDERATION_EXECUTION:
        my_auditServer.setLevel(9);
        processJoinFederation(link, static_cast<NM_Join_Federation_Execution*>(msg));
        break;

    case NetworkMessage::RESIGN_FEDERATION_EXECUTION:
        Debug(D, pdTrace) << "Federate (" << msg->getFederate() << ") leaves federation (" << msg->getFederation()
                          << ")" << std::endl;
        my_auditServer.setLevel(9);
        processResignFederation(link, msg->getFederation(), msg->getFederate());
        break;

    case NetworkMessage::REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
        Debug(D, pdTrace) << "Federation " << msg->getFederation() << ": registerFedSyncPoint from federate "
                          << msg->getFederate() << std::endl;
        my_auditServer.setLevel(8);
        processRegisterSynchronization(link, static_cast<NM_Register_Federation_Synchronization_Point*>(msg));
        break;

    case NetworkMessage::SYNCHRONIZATION_POINT_ACHIEVED:
        Debug(D, pdTrace) << "Federation " << msg->getFederation() << ": synchronizationPointAchieved from federate "
                          << msg->getFederate() << std::endl;
        my_auditServer.setLevel(8);
        processSynchronizationAchieved(link, msg);
        break;

    case NetworkMessage::REQUEST_FEDERATION_SAVE:
        Debug(D, pdTrace) << "Request federation save from federate " << msg->getFederate() << std::endl;
        my_auditServer.setLevel(8);
        processRequestFederationSave(link, msg);
        break;

    case NetworkMessage::FEDERATE_SAVE_BEGUN:
        Debug(D, pdTrace) << "Federate " << msg->getFederate() << " begun save" << std::endl;
        my_auditServer.setLevel(8);
        processFederateSaveBegun(link, msg);
        break;

    case NetworkMessage::FEDERATE_SAVE_COMPLETE:
    case NetworkMessage::FEDERATE_SAVE_NOT_COMPLETE:
        Debug(D, pdTrace) << "Federate " << msg->getFederate() << " save complete/not complete" << std::endl;
        my_auditServer.setLevel(8);
        processFederateSaveStatus(link, msg);
        break;

    case NetworkMessage::REQUEST_FEDERATION_RESTORE:
        Debug(D, pdTrace) << "Federate " << msg->getFederate() << " request a restoration" << std::endl;
        my_auditServer.setLevel(8);
        processRequestFederationRestore(link, msg);
        break;

    case NetworkMessage::FEDERATE_RESTORE_COMPLETE:
    case NetworkMessage::FEDERATE_RESTORE_NOT_COMPLETE:
        Debug(D, pdTrace) << "Federate " << msg->getFederate() << " restore complete/not complete" << std::endl;
        my_auditServer.setLevel(8);
        processFederateRestoreStatus(link, msg);
        break;

    case NetworkMessage::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
        Debug(D, pdTrace) << "RequestAttributeValueUpdate" << std::endl;
        my_auditServer.setLevel(6);
        processRequestObjectAttributeValueUpdate(link, static_cast<NM_Request_Object_Attribute_Value_Update*>(msg));
        break;

    case NetworkMessage::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE:
        Debug(D, pdTrace) << "RequestClassAttributeValueUpdate" << std::endl;
        my_auditServer.setLevel(6);
        processRequestClassAttributeValueUpdate(link, static_cast<NM_Request_Class_Attribute_Value_Update*>(msg));
        break;

    case NetworkMessage::SET_TIME_REGULATING:
        Debug(D, pdTrace) << "SetTimeRegulating for federate " << msg->getFederate()
                          << ", date:" << msg->getDate().getTime() << std::endl;
        my_auditServer.setLevel(8);
        processSetTimeRegulating(link, static_cast<NM_Set_Time_Regulating*>(msg));
        break;

    case NetworkMessage::SET_TIME_CONSTRAINED:
        Debug(D, pdTrace) << "SetTimeConstrained for federate " << msg->getFederate() << std::endl;
        my_auditServer.setLevel(8);

        processSetTimeConstrained(link, static_cast<NM_Set_Time_Constrained*>(msg));
        break;

    case NetworkMessage::PUBLISH_OBJECT_CLASS:
    case NetworkMessage::UNPUBLISH_OBJECT_CLASS:
        Debug(D, pdTrace) << "un/publishObjectClass" << std::endl;
        my_auditServer.setLevel(7);
        /* we cast to Publish because Unpublish inherits from Publish */
        processPublishObjectClass(link, static_cast<NM_Publish_Object_Class*>(msg));
        break;

    case NetworkMessage::PUBLISH_INTERACTION_CLASS:
    case NetworkMessage::UNPUBLISH_INTERACTION_CLASS:
        Debug(D, pdTrace) << "un/publishInteractionClass" << std::endl;
        my_auditServer.setLevel(7);
        processPublishInteractionClass(link, static_cast<NM_Publish_Interaction_Class*>(msg));
        break;

    case NetworkMessage::SUBSCRIBE_OBJECT_CLASS:
    case NetworkMessage::UNSUBSCRIBE_OBJECT_CLASS:
        Debug(D, pdTrace) << "un/subscribeObjectClass" << std::endl;
        my_auditServer.setLevel(7);
        processSubscribeObjectClass(link, static_cast<NM_Subscribe_Object_Class*>(msg));
        break;

    case NetworkMessage::SUBSCRIBE_INTERACTION_CLASS:
    case NetworkMessage::UNSUBSCRIBE_INTERACTION_CLASS:
        Debug(D, pdTrace) << "un/subscribeInteractionClass" << std::endl;
        my_auditServer.setLevel(7);
        processSubscribeInteractionClass(link, static_cast<NM_Subscribe_Interaction_Class*>(msg));
        break;

    case NetworkMessage::SET_CLASS_RELEVANCE_ADVISORY_SWITCH:
        Debug(D, pdTrace) << "setClassRelevanceAdvisorySwitch" << std::endl;
        my_auditServer.setLevel(6);
        processSetClassRelevanceAdvisorySwitch(link, static_cast<NM_Set_Class_Relevance_Advisory_Switch*>(msg));
        break;

    case NetworkMessage::SET_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
        Debug(D, pdTrace) << "setInteractionRelevanceAdvisorySwitch" << std::endl;
        my_auditServer.setLevel(6);
        processSetInteractionRelevanceAdvisorySwitch(link,
                                                     static_cast<NM_Set_Interaction_Relevance_Advisory_Switch*>(msg));
        break;

    case NetworkMessage::SET_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH:
        Debug(D, pdTrace) << "setAttributeRelevanceAdvisorySwitch" << std::endl;
        my_auditServer.setLevel(6);
        processSetAttributeRelevanceAdvisorySwitch(link, static_cast<NM_Set_Attribute_Relevance_Advisory_Switch*>(msg));
        break;

    case NetworkMessage::SET_ATTRIBUTE_SCOPE_ADVISORY_SWITCH:
        Debug(D, pdTrace) << "setAttributeScopeAdvisorySwitch" << std::endl;
        my_auditServer.setLevel(6);
        processSetAttributeScopeAdvisorySwitch(link, static_cast<NM_Set_Attribute_Scope_Advisory_Switch*>(msg));
        break;

    case NetworkMessage::RESERVE_OBJECT_INSTANCE_NAME:
        Debug(D, pdTrace) << "reserveObjectInstanceName" << std::endl;
        my_auditServer.setLevel(6);
        processReserveObjectInstanceName(link, static_cast<NM_Reserve_Object_Instance_Name*>(msg));
        break;

    case NetworkMessage::REGISTER_OBJECT:
        Debug(D, pdTrace) << "registerObject" << std::endl;
        my_auditServer.setLevel(6);
        processRegisterObject(link, static_cast<NM_Register_Object*>(msg));
        break;

    case NetworkMessage::DELETE_OBJECT:
        Debug(D, pdTrace) << "DeleteObject" << std::endl;
        my_auditServer.setLevel(6);
        processDeleteObject(link, static_cast<NM_Delete_Object*>(msg));
        break;

    case NetworkMessage::IS_ATTRIBUTE_OWNED_BY_FEDERATE:
        Debug(D, pdTrace) << "isAttributeOwnedByFederate" << std::endl;
        my_auditServer.setLevel(2);
        processAttributeOwnedByFederate(link, static_cast<NM_Is_Attribute_Owned_By_Federate*>(msg));
        break;

    case NetworkMessage::QUERY_ATTRIBUTE_OWNERSHIP:
        Debug(D, pdTrace) << "queryAttributeOwnership" << std::endl;
        my_auditServer.setLevel(2);
        processQueryAttributeOwnership(link, static_cast<NM_Query_Attribute_Ownership*>(msg));
        break;

    case NetworkMessage::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        Debug(D, pdTrace) << "negotiatedAttributeOwnershipDivestiture" << std::endl;
        my_auditServer.setLevel(6);
        processNegotiatedOwnershipDivestiture(link, static_cast<NM_Negotiated_Attribute_Ownership_Divestiture*>(msg));
        break;

    case NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
        Debug(D, pdTrace) << "attributeOwnershipAcquisitionIfAvailable" << std::endl;
        my_auditServer.setLevel(6);
        processAcquisitionIfAvailable(link, static_cast<NM_Attribute_Ownership_Acquisition_If_Available*>(msg));
        break;

    case NetworkMessage::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        Debug(D, pdTrace) << "unconditionalAttributeOwnershipDivestiture" << std::endl;
        my_auditServer.setLevel(6);
        processUnconditionalDivestiture(link, static_cast<NM_Unconditional_Attribute_Ownership_Divestiture*>(msg));
        break;

    case NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION:
        Debug(D, pdTrace) << "attributeOwnershipAcquisition" << std::endl;
        my_auditServer.setLevel(6);
        processOwnershipAcquisition(link, static_cast<NM_Attribute_Ownership_Acquisition*>(msg));
        break;

    case NetworkMessage::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        Debug(D, pdTrace) << "cancelNegociatedAttributeOwnershipDivestiture" << std::endl;
        my_auditServer.setLevel(6);
        processCancelNegotiatedDivestiture(link,
                                           static_cast<NM_Cancel_Negotiated_Attribute_Ownership_Divestiture*>(msg));
        break;

    case NetworkMessage::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
        Debug(D, pdTrace) << "attributeOwnershipReleaseResponse" << std::endl;
        my_auditServer.setLevel(6);
        processReleaseResponse(link, static_cast<NM_Attribute_Ownership_Release_Response*>(msg));
        break;

    case NetworkMessage::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
        Debug(D, pdTrace) << "cancelAttributeOwnershipAcquisition" << std::endl;
        my_auditServer.setLevel(6);
        processCancelAcquisition(link, static_cast<NM_Cancel_Attribute_Ownership_Acquisition*>(msg));
        break;

    case NetworkMessage::DDM_CREATE_REGION:
        Debug(D, pdTrace) << "createRegion" << std::endl;
        my_auditServer.setLevel(6);
        processCreateRegion(link, static_cast<NM_DDM_Create_Region*>(msg));
        break;

    case NetworkMessage::DDM_MODIFY_REGION:
        Debug(D, pdTrace) << "modifyRegion" << std::endl;
        my_auditServer.setLevel(6);
        processModifyRegion(link, static_cast<NM_DDM_Modify_Region*>(msg));
        break;

    case NetworkMessage::DDM_DELETE_REGION:
        Debug(D, pdTrace) << "deleteRegion" << std::endl;
        my_auditServer.setLevel(6);
        processDeleteRegion(link, static_cast<NM_DDM_Delete_Region*>(msg));
        break;

    case NetworkMessage::DDM_REGISTER_OBJECT:
        Debug(D, pdTrace) << "registerObjectWithRegion" << std::endl;
        my_auditServer.setLevel(6);
        processRegisterObjectWithRegion(link, static_cast<NM_DDM_Register_Object*>(msg));
        break;

    case NetworkMessage::DDM_ASSOCIATE_REGION:
        Debug(D, pdTrace) << "associateRegionForUpdates" << std::endl;
        my_auditServer.setLevel(6);
        processAssociateRegion(link, static_cast<NM_DDM_Associate_Region*>(msg));
        break;

    case NetworkMessage::DDM_UNASSOCIATE_REGION:
        Debug(D, pdTrace) << "unassociateRegionForUpdates" << std::endl;
        my_auditServer.setLevel(6);
        processUnassociateRegion(link, static_cast<NM_DDM_Unassociate_Region*>(msg));
        break;

    case NetworkMessage::DDM_SUBSCRIBE_ATTRIBUTES:
        Debug(D, pdTrace) << "subscribeObjectClassAttributes (DDM)" << std::endl;
        my_auditServer.setLevel(6);
        processSubscribeAttributesWR(link, static_cast<NM_DDM_Subscribe_Attributes*>(msg));
        break;

    case NetworkMessage::DDM_UNSUBSCRIBE_ATTRIBUTES:
        Debug(D, pdTrace) << "unsubscribeObjectClassAttributes (DDM)" << std::endl;
        my_auditServer.setLevel(6);
        processUnsubscribeAttributesWR(link, static_cast<NM_DDM_Unsubscribe_Attributes*>(msg));
        break;

    case NetworkMessage::DDM_SUBSCRIBE_INTERACTION:
        Debug(D, pdTrace) << "subscribeInteraction (DDM)" << std::endl;
        my_auditServer.setLevel(6);
        processSubscribeInteractionWR(link, static_cast<NM_DDM_Subscribe_Interaction*>(msg));
        break;

    case NetworkMessage::DDM_UNSUBSCRIBE_INTERACTION:
        Debug(D, pdTrace) << "unsubscribeInteraction (DDM)" << std::endl;
        my_auditServer.setLevel(6);
        processUnsubscribeInteractionWR(link, static_cast<NM_DDM_Unsubscribe_Interaction*>(msg));
        break;

    default:
        // FIXME: Should treat other cases CHANGE_*_ORDER/TRANSPORT_TYPE
        Debug(D, pdError) << "processMessageRecu: unknown type " << msg->getMessageType() << std::endl;
        throw RTIinternalError("Unknown Message Type");
    }
    Debug(G, pdGendoc) << "exit  RTIG::chooseProcessingMethod" << std::endl;
    return link; // It may have been set to NULL by closeConnection.
}

Socket* RTIG::processIncomingMessage(Socket* link) throw(NetworkError)
{
    Debug(G, pdGendoc) << "enter RTIG::processIncomingMessage" << std::endl;

    if (!link) {
        Debug(D, pdError) << "No socket in processIncomingMessage" << std::endl;
        return nullptr;
    }

    /* virtual constructor call */
    auto msg = std::unique_ptr<NetworkMessage>(NM_Factory::receive(link));

    // Server Answer(only if an exception is raised)
    //         std::auto_ptr<NetworkMessage> rep();
    auto response = std::unique_ptr<NetworkMessage>(NM_Factory::create(msg->getMessageType()));
    response->setFederate(msg->getFederate());

    my_auditServer.startLine(msg->getFederation(), msg->getFederate(), msg->getMessageType());

// This macro is used to copy any non null exception reason
// string into our buffer(used for Audit purpose).

#define CPY_NOT_NULL(A)                                                                                                \
    {                                                                                                                  \
        if (!A.reason().empty()) {                                                                                     \
            exceptionReason = A.reason();                                                                              \
        }                                                                                                              \
    }

#define PRINT_DEBUG_MESSAGE(A) Debug(D, pdError) << "Caught exception " << A.name() << std::endl

#define BASIC_CATCH(ExceptionType)                                                                                     \
    catch (ExceptionType & e)                                                                                          \
    {                                                                                                                  \
        PRINT_DEBUG_MESSAGE(e);                                                                                        \
        CPY_NOT_NULL(e);                                                                                               \
        response->setException(e.type());                                                                              \
    }

    std::string exceptionReason;

    try {
        link = chooseProcessingMethod(link, msg.get());
    }
    BASIC_CATCH(ArrayIndexOutOfBounds)
    BASIC_CATCH(AttributeAlreadyOwned)
    BASIC_CATCH(AttributeAlreadyBeingAcquired)
    BASIC_CATCH(AttributeAlreadyBeingDivested)
    BASIC_CATCH(AttributeDivestitureWasNotRequested)
    BASIC_CATCH(AttributeAcquisitionWasNotRequested)
    BASIC_CATCH(AttributeNotDefined)
    BASIC_CATCH(AttributeNotKnown)
    BASIC_CATCH(AttributeNotOwned)
    BASIC_CATCH(AttributeNotPublished)
    BASIC_CATCH(AttributeNotSubscribed)
    BASIC_CATCH(ConcurrentAccessAttempted)
    BASIC_CATCH(CouldNotDiscover)
    BASIC_CATCH(CouldNotOpenRID)
    BASIC_CATCH(CouldNotOpenFED)
    BASIC_CATCH(CouldNotRestore)
    BASIC_CATCH(DeletePrivilegeNotHeld)
    BASIC_CATCH(ErrorReadingRID)
    BASIC_CATCH(EventNotKnown)
    BASIC_CATCH(FederateAlreadyPaused)
    BASIC_CATCH(FederateAlreadyExecutionMember)
    BASIC_CATCH(FederateDoesNotExist)
    BASIC_CATCH(FederateInternalError)
    BASIC_CATCH(FederateNameAlreadyInUse)
    BASIC_CATCH(FederateNotExecutionMember)
    BASIC_CATCH(FederateNotPaused)
    BASIC_CATCH(FederateNotPublishing)
    BASIC_CATCH(FederateNotSubscribing)
    BASIC_CATCH(FederateOwnsAttributes)
    BASIC_CATCH(FederatesCurrentlyJoined)
    BASIC_CATCH(FederateWasNotAskedToReleaseAttribute)
    BASIC_CATCH(FederationAlreadyPaused)
    BASIC_CATCH(FederationExecutionAlreadyExists)
    BASIC_CATCH(FederationExecutionDoesNotExist)
    BASIC_CATCH(FederationNotPaused)
    BASIC_CATCH(FederationTimeAlreadyPassed)
    BASIC_CATCH(IDsupplyExhausted)
    BASIC_CATCH(InteractionClassNotDefined)
    BASIC_CATCH(InteractionClassNotKnown)
    BASIC_CATCH(InteractionClassNotPublished)
    BASIC_CATCH(InteractionParameterNotDefined)
    BASIC_CATCH(InteractionParameterNotKnown)
    BASIC_CATCH(InvalidDivestitureCondition)
    BASIC_CATCH(InvalidExtents)
    BASIC_CATCH(InvalidFederationTime)
    BASIC_CATCH(InvalidFederationTimeDelta)
    BASIC_CATCH(InvalidObjectHandle)
    BASIC_CATCH(InvalidOrderingHandle)
    BASIC_CATCH(InvalidResignAction)
    BASIC_CATCH(InvalidRetractionHandle)
    BASIC_CATCH(InvalidRoutingSpace)
    BASIC_CATCH(InvalidTransportationHandle)
    BASIC_CATCH(MemoryExhausted)
    BASIC_CATCH(NameNotFound)
    BASIC_CATCH(NoPauseRequested)
    BASIC_CATCH(NoResumeRequested)
    BASIC_CATCH(ObjectClassNotDefined)
    BASIC_CATCH(ObjectClassNotKnown)
    BASIC_CATCH(ObjectClassNotPublished)
    BASIC_CATCH(ObjectClassNotSubscribed)
    BASIC_CATCH(ObjectNotKnown)
    BASIC_CATCH(ObjectAlreadyRegistered)
    BASIC_CATCH(RegionNotKnown)
    BASIC_CATCH(RestoreInProgress)
    BASIC_CATCH(RestoreNotRequested)
    BASIC_CATCH(SaveInProgress)
    BASIC_CATCH(SaveNotInitiated)
    BASIC_CATCH(SpaceNotDefined)
    BASIC_CATCH(SpecifiedSaveLabelDoesNotExist)
    BASIC_CATCH(TimeAdvanceAlreadyInProgress)
    BASIC_CATCH(TimeAdvanceWasNotInProgress)
    BASIC_CATCH(TooManyIDsRequested)
    BASIC_CATCH(UnableToPerformSave)
    BASIC_CATCH(UnimplementedService)
    BASIC_CATCH(UnknownLabel)
    BASIC_CATCH(ValueCountExceeded)
    BASIC_CATCH(ValueLengthExceeded)

    catch (RTIinternalError& e)
    {
        if (e.reason().empty()) {
            PRINT_DEBUG_MESSAGE(e);
        }
        else {
            Debug(D, pdExcept) << "Caught Exception: " << e.name() << ", " << e.reason() << std::endl;
        }
        CPY_NOT_NULL(e);
        response->setException(Exception::Type::RTIinternalError);
    }

    catch (SecurityError& e)
    {
        std::cout << std::endl << "Security Error : " << e.reason() << std::endl;
        CPY_NOT_NULL(e);
        response->setException(Exception::Type::SecurityError);
    }

    // Non RTI specific exception, Client connection problem(internal)
    catch (NetworkError& e)
    {
        exceptionReason = " - NetworkError";
        my_auditServer.endLine(static_cast<unsigned short>(response->getException()), exceptionReason);
        throw e;
    }

    // Default Handler
    catch (Exception& e)
    {
        Debug(D, pdExcept) << "Unknown Exception: " << e.name() << std::endl;
        CPY_NOT_NULL(e);
        response->setException(Exception::Type::RTIinternalError);
    }

    // buffer may contain an exception reason. If not, set it to OK
    // or Exception
    if (exceptionReason.empty()) {
        if (response->getException() == Exception::Type::NO_EXCEPTION) {
            exceptionReason = " - OK";
        }
        else {
            exceptionReason = " - Exception";
        }
    }

    my_auditServer.endLine(static_cast<unsigned short>(response->getException()), exceptionReason);
    if (link == nullptr) {
        return link;
    }

    /* FIXME ***/
    if (response->getException() != Exception::Type::NO_EXCEPTION) {
        Debug(G, pdGendoc) << "            processIncomingMessage ===> write on exception to RTIA" << std::endl;
        response->send(link, my_NM_msgBufSend);
        Debug(D, pdExcept) << "RTIG catched exception " << static_cast<long>(response->getException()) << " and sent it back to federate "
                           << response->getFederate() << std::endl;
    }
    Debug(G, pdGendoc) << "exit  RTIG::processIncomingMessage" << std::endl;
    return link;
}

void RTIG::openConnection()
{
    try {
        my_socketServer.open();
    }
    catch (RTIinternalError& e) {
        Debug(D, pdExcept) << "Error while accepting new connection: " << e.reason() << std::endl;
    }

    Debug(D, pdInit) << "Accepting new connection" << std::endl;
}

void RTIG::closeConnection(Socket* link, bool emergency)
{
    Handle federation;
    FederateHandle federate;

    Debug(G, pdGendoc) << "enter RTIG::closeConnection" << std::endl;
    try {
        my_socketServer.close(link->returnSocket(), federation, federate);
    }
    catch (RTIinternalError& e) {
        Debug(D, pdError) << "Connection not found while trying to close it" << std::endl;
    }

    if (emergency) {
        Debug(D, pdExcept) << "Killing Federate(" << federation << ", " << federate << ")..." << std::endl;
        my_federations.killFederate(federation, federate);
        Debug(D, pdExcept) << "Federate(" << federation << ", " << federate << ") killed" << std::endl;
    }

    Debug(G, pdGendoc) << "exit  RTIG::closeConnection" << std::endl;
}

int RTIG::inferTcpPort() const
{
    auto tcp_port_s = getenv(tcpPortEnvironmentVariable);
    if (tcp_port_s) {
        return std::stoi(tcp_port_s);
    }
    else {
        return std::stoi(defaultTcpPort);
    }
}

int RTIG::inferUdpPort() const
{
    auto udp_port_s = getenv(udpPortEnvironmentVariable);
    if (udp_port_s) {
        return std::stoi(udp_port_s);
    }
    else {
        return std::stoi(defaultUdpPort);
    }
}
}
} // namespace certi/rtig

// $Id: RTIG.cc,v 3.73 2014/04/16 12:24:01 erk Exp $
