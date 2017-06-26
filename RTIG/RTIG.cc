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
#include "NetworkMessage.hh"
#include "PrettyDebug.hh"
#include "Socket.hh"

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
            D.Out(pdCom, "Incoming message on socket %ld.", link->returnSocket());
            try {
                do {
                    link = processIncomingMessage(link);
                    if (!link) {
                        break;
                    }
                } while (link->isDataReady());
            }
            catch (NetworkError& e) {
                if (!e._reason.empty()) {
                    D.Out(pdExcept, "Catching Network Error, reason : %s", e._reason.c_str());
                }
                else {
                    D.Out(pdExcept, "Catching Network Error, no reason string.");
                }
                std::cout << "RTIG dropping client connection " << link->returnSocket() << '.' << std::endl;
                closeConnection(link, true);
                link = nullptr;
            }
        }

        // Or on the server socket ?
        if (FD_ISSET(my_tcpSocketServer.returnSocket(), &fd)) {
            D.Out(pdCom, "Demande de connexion.");
            openConnection();
        }
    }
}

void RTIG::signalHandler(int sig)
{
    D.Out(pdError, "Received Signal %d.", sig);

    if (sig == SIGINT) {
        terminate = true;
    }
#ifndef _WIN32
    if (sig == SIGPIPE) {
        std::cout << "Ignoring 'Broken pipe' signal." << std::endl;
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
    G.Out(pdGendoc, "enter RTIG::chooseProcessingMethod type (%s)", msg->getMessageName());

    // This may throw a security error.
    if (msg->getMessageType() != NetworkMessage::DESTROY_FEDERATION_EXECUTION) {
        my_socketServer.checkMessage(link->returnSocket(), msg);
    }

    switch (msg->getMessageType()) {
    case NetworkMessage::MESSAGE_NULL:
        D.Out(pdDebug, "Message Null.");
        my_auditServer.setLevel(0);
        processMessageNull(msg, false);
        break;

    case NetworkMessage::MESSAGE_NULL_PRIME:
        D.Out(pdDebug, "Message Null.");
        my_auditServer.setLevel(0);
        processMessageNullPrime(static_cast<NM_Message_Null_Prime*>(msg));
        break;

    case NetworkMessage::UPDATE_ATTRIBUTE_VALUES:
        D.Out(pdDebug, "UpdateAttributeValue.");
        my_auditServer.setLevel(1);
        processUpdateAttributeValues(link, static_cast<NM_Update_Attribute_Values*>(msg));
        break;

    case NetworkMessage::SEND_INTERACTION:
        D.Out(pdTrace, "send interaction.");
        my_auditServer.setLevel(2);
        processSendInteraction(link, static_cast<NM_Send_Interaction*>(msg));
        break;

    case NetworkMessage::CLOSE_CONNEXION:
        D.Out(pdTrace, "Close connection %ld.", link->returnSocket());
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
        D.Out(pdTrace, "Federate no %u leaves federation no %u .", msg->getFederate(), msg->getFederation());
        my_auditServer.setLevel(9);
        processResignFederation(link, msg->getFederation(), msg->getFederate());
        break;

    case NetworkMessage::REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
        D.Out(
            pdTrace, "Federation %u: registerFedSyncPoint from federate %u.", msg->getFederation(), msg->getFederate());
        my_auditServer.setLevel(8);
        processRegisterSynchronization(link, static_cast<NM_Register_Federation_Synchronization_Point*>(msg));
        break;

    case NetworkMessage::SYNCHRONIZATION_POINT_ACHIEVED:
        D.Out(pdTrace,
              "Federation %u: synchronizationPointAchieved from federate %u.",
              msg->getFederation(),
              msg->getFederate());
        my_auditServer.setLevel(8);
        processSynchronizationAchieved(link, msg);
        break;

    case NetworkMessage::REQUEST_FEDERATION_SAVE:
        D.Out(pdTrace, "Request federation save from federate %u.", msg->getFederate());
        my_auditServer.setLevel(8);
        processRequestFederationSave(link, msg);
        break;

    case NetworkMessage::FEDERATE_SAVE_BEGUN:
        D.Out(pdTrace, "Federate %u begun save.", msg->getFederate());
        my_auditServer.setLevel(8);
        processFederateSaveBegun(link, msg);
        break;

    case NetworkMessage::FEDERATE_SAVE_COMPLETE:
    case NetworkMessage::FEDERATE_SAVE_NOT_COMPLETE:
        D.Out(pdTrace, "Federate %u save complete/not complete.", msg->getFederate());
        my_auditServer.setLevel(8);
        processFederateSaveStatus(link, msg);
        break;

    case NetworkMessage::REQUEST_FEDERATION_RESTORE:
        D.Out(pdTrace, "Federate %u request a restoration.", msg->getFederate());
        my_auditServer.setLevel(8);
        processRequestFederationRestore(link, msg);
        break;

    case NetworkMessage::FEDERATE_RESTORE_COMPLETE:
    case NetworkMessage::FEDERATE_RESTORE_NOT_COMPLETE:
        D.Out(pdTrace, "Federate %u restore complete/not complete.", msg->getFederate());
        my_auditServer.setLevel(8);
        processFederateRestoreStatus(link, msg);
        break;

    case NetworkMessage::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
        D.Out(pdTrace, "RequestAttributeValueUpdate.)");
        my_auditServer.setLevel(6);
        processRequestObjectAttributeValueUpdate(link, static_cast<NM_Request_Object_Attribute_Value_Update*>(msg));
        break;

    case NetworkMessage::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE:
        D.Out(pdTrace, "RequestClassAttributeValueUpdate.)");
        my_auditServer.setLevel(6);
        processRequestClassAttributeValueUpdate(link, static_cast<NM_Request_Class_Attribute_Value_Update*>(msg));
        break;

    case NetworkMessage::SET_TIME_REGULATING:
        D.Out(pdTrace, "SetTimeRegulating du federe %u(date=%f).", msg->getFederate(), msg->getDate().getTime());
        my_auditServer.setLevel(8);
        processSetTimeRegulating(link, static_cast<NM_Set_Time_Regulating*>(msg));
        break;

    case NetworkMessage::SET_TIME_CONSTRAINED:
        D.Out(pdTrace, "SetTimeConstrained du federe %u.", msg->getFederate());
        my_auditServer.setLevel(8);

        processSetTimeConstrained(link, static_cast<NM_Set_Time_Constrained*>(msg));
        break;

    case NetworkMessage::PUBLISH_OBJECT_CLASS:
    case NetworkMessage::UNPUBLISH_OBJECT_CLASS:
        D.Out(pdTrace, "un/publishObjectClass.");
        my_auditServer.setLevel(7);
        /* we cast to Publish because Unpublish inherits from Publish */
        processPublishObjectClass(link, static_cast<NM_Publish_Object_Class*>(msg));
        break;

    case NetworkMessage::PUBLISH_INTERACTION_CLASS:
    case NetworkMessage::UNPUBLISH_INTERACTION_CLASS:
        D.Out(pdTrace, "un/publishInteractionClass.");
        my_auditServer.setLevel(7);
        processPublishInteractionClass(link, static_cast<NM_Publish_Interaction_Class*>(msg));
        break;

    case NetworkMessage::SUBSCRIBE_OBJECT_CLASS:
    case NetworkMessage::UNSUBSCRIBE_OBJECT_CLASS:
        D.Out(pdTrace, "un/subscribeObjectClass.");
        my_auditServer.setLevel(7);
        processSubscribeObjectClass(link, static_cast<NM_Subscribe_Object_Class*>(msg));
        break;

    case NetworkMessage::SUBSCRIBE_INTERACTION_CLASS:
    case NetworkMessage::UNSUBSCRIBE_INTERACTION_CLASS:
        D.Out(pdTrace, "un/subscribeInteractionClass.");
        my_auditServer.setLevel(7);
        processSubscribeInteractionClass(link, static_cast<NM_Subscribe_Interaction_Class*>(msg));
        break;

    case NetworkMessage::SET_CLASS_RELEVANCE_ADVISORY_SWITCH:
        D.Out(pdTrace, "setClassRelevanceAdvisorySwitch.");
        my_auditServer.setLevel(6);
        processSetClassRelevanceAdvisorySwitch(link, static_cast<NM_Set_Class_Relevance_Advisory_Switch*>(msg));
        break;

    case NetworkMessage::SET_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
        D.Out(pdTrace, "setInteractionRelevanceAdvisorySwitch.");
        my_auditServer.setLevel(6);
        processSetInteractionRelevanceAdvisorySwitch(link,
                                                     static_cast<NM_Set_Interaction_Relevance_Advisory_Switch*>(msg));
        break;

    case NetworkMessage::SET_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH:
        D.Out(pdTrace, "setAttributeRelevanceAdvisorySwitch.");
        my_auditServer.setLevel(6);
        processSetAttributeRelevanceAdvisorySwitch(link, static_cast<NM_Set_Attribute_Relevance_Advisory_Switch*>(msg));
        break;

    case NetworkMessage::SET_ATTRIBUTE_SCOPE_ADVISORY_SWITCH:
        D.Out(pdTrace, "setAttributeScopeAdvisorySwitch.");
        my_auditServer.setLevel(6);
        processSetAttributeScopeAdvisorySwitch(link, static_cast<NM_Set_Attribute_Scope_Advisory_Switch*>(msg));
        break;

    case NetworkMessage::RESERVE_OBJECT_INSTANCE_NAME:
        Debug(D, pdTrace) << "reserveObjectInstanceName" << std::endl;
        my_auditServer.setLevel(6);
        processReserveObjectInstanceName(link, static_cast<NM_Reserve_Object_Instance_Name*>(msg));
        break;

    case NetworkMessage::REGISTER_OBJECT:
        D.Out(pdTrace, "registerObject.");
        my_auditServer.setLevel(6);
        processRegisterObject(link, static_cast<NM_Register_Object*>(msg));
        break;

    case NetworkMessage::DELETE_OBJECT:
        D.Out(pdTrace, "DeleteObject..");
        my_auditServer.setLevel(6);
        processDeleteObject(link, static_cast<NM_Delete_Object*>(msg));
        break;

    case NetworkMessage::IS_ATTRIBUTE_OWNED_BY_FEDERATE:
        D.Out(pdTrace, "isAttributeOwnedByFederate..");
        my_auditServer.setLevel(2);
        processAttributeOwnedByFederate(link, static_cast<NM_Is_Attribute_Owned_By_Federate*>(msg));
        break;

    case NetworkMessage::QUERY_ATTRIBUTE_OWNERSHIP:
        D.Out(pdTrace, "queryAttributeOwnership..");
        my_auditServer.setLevel(2);
        processQueryAttributeOwnership(link, static_cast<NM_Query_Attribute_Ownership*>(msg));
        break;

    case NetworkMessage::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        D.Out(pdTrace, "negotiatedAttributeOwnershipDivestiture..");
        my_auditServer.setLevel(6);
        processNegotiatedOwnershipDivestiture(link, static_cast<NM_Negotiated_Attribute_Ownership_Divestiture*>(msg));
        break;

    case NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
        D.Out(pdTrace, "attributeOwnershipAcquisitionIfAvailable..");
        my_auditServer.setLevel(6);
        processAcquisitionIfAvailable(link, static_cast<NM_Attribute_Ownership_Acquisition_If_Available*>(msg));
        break;

    case NetworkMessage::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        D.Out(pdTrace, "unconditionalAttributeOwnershipDivestiture..");
        my_auditServer.setLevel(6);
        processUnconditionalDivestiture(link, static_cast<NM_Unconditional_Attribute_Ownership_Divestiture*>(msg));
        break;

    case NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION:
        D.Out(pdTrace, "attributeOwnershipAcquisition..");
        my_auditServer.setLevel(6);
        processOwnershipAcquisition(link, static_cast<NM_Attribute_Ownership_Acquisition*>(msg));
        break;

    case NetworkMessage::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        D.Out(pdTrace, "cancelNegociatedAttributeOwnershipDivestiture..");
        my_auditServer.setLevel(6);
        processCancelNegotiatedDivestiture(link,
                                           static_cast<NM_Cancel_Negotiated_Attribute_Ownership_Divestiture*>(msg));
        break;

    case NetworkMessage::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
        D.Out(pdTrace, "attributeOwnershipReleaseResponse..");
        my_auditServer.setLevel(6);
        processReleaseResponse(link, static_cast<NM_Attribute_Ownership_Release_Response*>(msg));
        break;

    case NetworkMessage::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
        D.Out(pdTrace, "cancelAttributeOwnershipAcquisition..");
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
        D.Out(pdError, "processMessageRecu: unknown type %u.", msg->getMessageType());
        throw RTIinternalError("Unknown Message Type");
    }
    G.Out(pdGendoc, "exit  RTIG::chooseProcessingMethod");
    return link; // It may have been set to NULL by closeConnection.
}

Socket* RTIG::processIncomingMessage(Socket* link) throw(NetworkError)
{
    G.Out(pdGendoc, "enter RTIG::processIncomingMessage");

    if (!link) {
        D.Out(pdError, "No socket in processIncomingMessage.");
        return nullptr;
    }

    /* virtual constructor call */
    NetworkMessage* msg = NM_Factory::receive(link);

    // Server Answer(only if an exception is raised)
    //         std::auto_ptr<NetworkMessage> rep();
    auto response = std::unique_ptr<NetworkMessage>(NM_Factory::create(msg->getMessageType()));
    response->setFederate(msg->getFederate());

    my_auditServer.startLine(msg->getFederation(), msg->getFederate(), msg->getMessageType());

// This macro is used to copy any non null exception reason
// string into our buffer(used for Audit purpose).

#define CPY_NOT_NULL(A)                                                                                                \
    {                                                                                                                  \
        if (!A._reason.empty())                                                                                        \
            strcpy(buffer, A._reason.c_str());                                                                         \
    }

    char buffer[BUFFER_EXCEPTION_REASON_SIZE]; // To store the exception reason
    buffer[0] = 0;

    try {
        link = chooseProcessingMethod(link, msg);
    }
    catch (ArrayIndexOutOfBounds& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_ArrayIndexOutOfBounds);
    }
    catch (AttributeAlreadyOwned& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_AttributeAlreadyOwned);
    }
    catch (AttributeAlreadyBeingAcquired& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_AttributeAlreadyBeingAcquired);
    }
    catch (AttributeAlreadyBeingDivested& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_AttributeAlreadyBeingDivested);
    }
    catch (AttributeDivestitureWasNotRequested& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_AttributeDivestitureWasNotRequested);
    }
    catch (AttributeAcquisitionWasNotRequested& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_AttributeAcquisitionWasNotRequested);
    }
    catch (AttributeNotDefined& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_AttributeNotDefined);
    }
    catch (AttributeNotKnown& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_AttributeNotKnown);
    }
    catch (AttributeNotOwned& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_AttributeNotOwned);
    }
    catch (AttributeNotPublished& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_AttributeNotPublished);
    }
    catch (AttributeNotSubscribed& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_AttributeNotSubscribed);
    }
    catch (ConcurrentAccessAttempted& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_ConcurrentAccessAttempted);
    }
    catch (CouldNotDiscover& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_CouldNotDiscover);
    }
    catch (CouldNotOpenRID& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_CouldNotOpenRID);
    }
    catch (CouldNotOpenFED& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_CouldNotOpenFED);
    }
    catch (CouldNotRestore& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_CouldNotRestore);
    }
    catch (DeletePrivilegeNotHeld& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_DeletePrivilegeNotHeld);
    }
    catch (ErrorReadingRID& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_ErrorReadingRID);
    }
    catch (EventNotKnown& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_EventNotKnown);
    }
    catch (FederateAlreadyPaused& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_FederateAlreadyPaused);
    }
    catch (FederateAlreadyExecutionMember& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_FederateAlreadyExecutionMember);
    }
    catch (FederateDoesNotExist& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_FederateDoesNotExist);
    }
    catch (FederateInternalError& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_FederateInternalError);
    }
    catch (FederateNameAlreadyInUse& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_FederateNameAlreadyInUse);
    }
    catch (FederateNotExecutionMember& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_FederateNotExecutionMember);
    }
    catch (FederateNotPaused& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_FederateNotPaused);
    }
    catch (FederateNotPublishing& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_FederateNotPublishing);
    }
    catch (FederateNotSubscribing& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_FederateNotSubscribing);
    }
    catch (FederateOwnsAttributes& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_FederateOwnsAttributes);
    }
    catch (FederatesCurrentlyJoined& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_FederatesCurrentlyJoined);
    }
    catch (FederateWasNotAskedToReleaseAttribute& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_FederateWasNotAskedToReleaseAttribute);
    }
    catch (FederationAlreadyPaused& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_FederationAlreadyPaused);
    }
    catch (FederationExecutionAlreadyExists& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_FederationExecutionAlreadyExists);
    }
    catch (FederationExecutionDoesNotExist& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_FederationExecutionDoesNotExist);
    }
    catch (FederationNotPaused& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_FederationNotPaused);
    }
    catch (FederationTimeAlreadyPassed& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_FederationTimeAlreadyPassed);
    }
    catch (IDsupplyExhausted& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_IDsupplyExhausted);
    }
    catch (InteractionClassNotDefined& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_InteractionClassNotDefined);
    }
    catch (InteractionClassNotKnown& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_InteractionClassNotKnown);
    }
    catch (InteractionClassNotPublished& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_InteractionClassNotPublished);
    }
    catch (InteractionParameterNotDefined& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_InteractionParameterNotDefined);
    }
    catch (InteractionParameterNotKnown& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_InteractionParameterNotKnown);
    }
    catch (InvalidDivestitureCondition& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_InvalidDivestitureCondition);
    }
    catch (InvalidExtents& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_InvalidExtents);
    }
    catch (InvalidFederationTime& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_InvalidFederationTime);
    }
    catch (InvalidFederationTimeDelta& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_InvalidFederationTimeDelta);
    }
    catch (InvalidObjectHandle& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_InvalidObjectHandle);
    }
    catch (InvalidOrderingHandle& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_InvalidOrderingHandle);
    }
    catch (InvalidResignAction& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_InvalidResignAction);
    }
    catch (InvalidRetractionHandle& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_InvalidRetractionHandle);
    }
    catch (InvalidRoutingSpace& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_InvalidRoutingSpace);
    }
    catch (InvalidTransportationHandle& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_InvalidTransportationHandle);
    }
    catch (MemoryExhausted& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_MemoryExhausted);
    }
    catch (NameNotFound& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_NameNotFound);
    }
    catch (NoPauseRequested& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_NoPauseRequested);
    }
    catch (NoResumeRequested& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_NoResumeRequested);
    }
    catch (ObjectClassNotDefined& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_ObjectClassNotDefined);
    }
    catch (ObjectClassNotKnown& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_ObjectClassNotKnown);
    }
    catch (ObjectClassNotPublished& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_ObjectClassNotPublished);
    }
    catch (ObjectClassNotSubscribed& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_ObjectClassNotSubscribed);
    }
    catch (ObjectNotKnown& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_ObjectNotKnown);
    }
    catch (ObjectAlreadyRegistered& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_ObjectAlreadyRegistered);
    }
    catch (RegionNotKnown& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_RegionNotKnown);
    }
    catch (RestoreInProgress& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_RestoreInProgress);
    }
    catch (RestoreNotRequested& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_RestoreNotRequested);
    }
    catch (RTIinternalError& e) {
        if (e._reason.empty())
            D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        else
            D.Out(pdExcept, "Catching \"%s\" exception: %s.", e._name, e._reason.c_str());
        CPY_NOT_NULL(e);
        response->setException(e_RTIinternalError);
    }
    catch (SaveInProgress& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_SaveInProgress);
    }
    catch (SaveNotInitiated& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_SaveNotInitiated);
    }
    catch (SecurityError& e) {
        std::cout << std::endl << "Security Error : " << e._reason << std::endl;
        CPY_NOT_NULL(e);
        response->setException(e_SecurityError);
    }
    catch (SpaceNotDefined& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_SpaceNotDefined);
    }
    catch (SpecifiedSaveLabelDoesNotExist& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_SpecifiedSaveLabelDoesNotExist);
    }
    catch (TimeAdvanceAlreadyInProgress& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_TimeAdvanceAlreadyInProgress);
    }
    catch (TimeAdvanceWasNotInProgress& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_TimeAdvanceWasNotInProgress);
    }
    catch (TooManyIDsRequested& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_TooManyIDsRequested);
    }
    catch (UnableToPerformSave& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_UnableToPerformSave);
    }
    catch (UnimplementedService& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_UnimplementedService);
    }
    catch (UnknownLabel& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_UnknownLabel);
    }
    catch (ValueCountExceeded& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_ValueCountExceeded);
    }
    catch (ValueLengthExceeded& e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_ValueLengthExceeded);
    }

    // Non RTI specific exception, Client connection problem(internal)
    catch (NetworkError& e) {
        strcpy(buffer, " - NetworkError");
        my_auditServer.endLine(response->getException(), buffer);
        delete msg;
        throw e;
    }
    // Default Handler
    catch (Exception& e) {
        D.Out(pdExcept, "Unknown Exception : %s.", e._name);
        CPY_NOT_NULL(e);
        response->setException(e_RTIinternalError);
    }

    // buffer may contain an exception reason. If not, set it to OK
    // or Exception
    if (strlen(buffer) == 0) {
        if (response->getException() == e_NO_EXCEPTION)
            strcpy(buffer, " - OK");
        else
            strcpy(buffer, " - Exception");
    }

    my_auditServer.endLine(response->getException(), buffer);
    delete msg;
    if (link == nullptr) {
        return link;
    }

    /* FIXME ***/
    if (response->getException() != e_NO_EXCEPTION) {
        G.Out(pdGendoc, "            processIncomingMessage ===> write on exception to RTIA");
        response->send(link, my_NM_msgBufSend);
        D.Out(pdExcept,
              "RTIG catched exception %d and sent it back to federate %d.",
              response->getException(),
              response->getFederate());
    }
    G.Out(pdGendoc, "exit  RTIG::processIncomingMessage");
    return link;
}

void RTIG::openConnection()
{
    try {
        my_socketServer.open();
    }
    catch (RTIinternalError& e) {
        D.Out(pdExcept, "Error while accepting new connection : %s.", e._reason.c_str());
    }

    D.Out(pdInit, "Accepting new connection.");
}

void RTIG::closeConnection(Socket* link, bool emergency)
{
    Handle federation;
    FederateHandle federate;

    G.Out(pdGendoc, "enter RTIG::closeConnection");
    try {
        my_socketServer.close(link->returnSocket(), federation, federate);
    }
    catch (RTIinternalError& e) {
        D.Out(pdError, "Connection not found while trying to close it.");
    }

    if (emergency) {
        D.Out(pdExcept, "Killing Federate(%u, %u)...", federation, federate);
        my_federations.killFederate(federation, federate);
        D.Out(pdExcept, "Federate(%u, %u)Killed... ", federation, federate);
    }

    G.Out(pdGendoc, "exit  RTIG::closeConnection");
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
