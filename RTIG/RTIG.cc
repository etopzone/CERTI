// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: RTIG.cc,v 3.13 2003/06/25 16:23:48 breholee Exp $
// ----------------------------------------------------------------------------

#include "RTIG.hh"

namespace certi {
namespace rtig {

static pdCDebug D("RTIG", "(RTIG)- ");

// ----------------------------------------------------------------------------
// Constructor

RTIG::RTIG()
{
    terminate = false ;

    nextFederationHandle = 1 ;

    // Start RTIG services
    const char *tcp_port_s = getenv("CERTI_TCP_PORT");
    const char *udp_port_s = getenv("CERTI_UDP_PORT");
    if (tcp_port_s==NULL) tcp_port_s = PORT_TCP_RTIG ;
    if (udp_port_s==NULL) udp_port_s = PORT_UDP_RTIG ;
    tcpPort = atoi(tcp_port_s);
    udpPort = atoi(udp_port_s);

    socketServer = new SocketServer(&tcpSocketServer,
                                    &udpSocketServer,
                                    udpPort);

    if (socketServer == NULL)
        throw RTIinternalError("No memory to create socketServer.");

    auditServer = new AuditFile(RTIG_AUDIT_FILENAME);

    if (auditServer == NULL)
        throw RTIinternalError("No memory to create auditServer.");

    federations = new FederationsList(socketServer, auditServer);
    if (federations == NULL)
        throw RTIinternalError("No memory to create federations.");
}

// ----------------------------------------------------------------------------
// Destructor

RTIG::~RTIG()
{
    tcpSocketServer.close();
    udpSocketServer.close();

    delete socketServer ;
    delete federations ;
    delete auditServer ;

    cout << endl << "Stopping RTIG" << endl ;
}

// ----------------------------------------------------------------------------
//! Choose the right processing module to call.
/*! This module chooses the right processing module to call. This process is
  done by examinating the message type.
*/
Socket*
RTIG::chooseProcessingMethod(Socket *link, NetworkMessage *msg)
{

    // This may throw a security error.
    socketServer->checkMessage(link->returnSocket(), msg);

    switch(msg->type) {
      case NetworkMessage::MESSAGE_NULL:
        D.Out(pdDebug, "Message Null.");
        auditServer->setLevel(0);
        processMessageNull(msg);
        break ;

      case NetworkMessage::UPDATE_ATTRIBUTE_VALUES:
        D.Out(pdDebug, "UpdateAttributeValue.");
        auditServer->setLevel(1);
        processUpdateAttributeValues(link, msg);
        break ;

      case NetworkMessage::SEND_INTERACTION:
        D.Out(pdTrace, "send interaction.");
        auditServer->setLevel(2);
        processSendInteraction(link, msg);
        break ;

      case NetworkMessage::CLOSE_CONNEXION:
        D.Out(pdTrace, "Close connection %ld.", link->returnSocket());
        auditServer->setLevel(9);
        auditServer->addToLinef("Socket %ld", link->returnSocket());
        closeConnection(link, false);
        link = NULL ;
        break ;

      case NetworkMessage::CREATE_FEDERATION_EXECUTION:
        D.Out(pdTrace, "Create federation \"%s\".", msg->federationName);
        auditServer->setLevel(9);
        processCreateFederation(link, msg);
        break ;

      case NetworkMessage::DESTROY_FEDERATION_EXECUTION:
        D.Out(pdTrace, "Destroy federation \"%s\".", msg->federationName);
        auditServer->setLevel(9);
        processDestroyFederation(link, msg);
        break ;

      case NetworkMessage::JOIN_FEDERATION_EXECUTION:
        D.Out(pdTrace, "federate \"%s\" joins federation \"%s\".",
              msg->federateName, msg->federationName);
        auditServer->setLevel(9);
        processJoinFederation(link, msg);
        break ;

      case NetworkMessage::RESIGN_FEDERATION_EXECUTION:
        D.Out(pdTrace, "Federate no %u leaves federation no %u .",
              msg->federate, msg->federation);
        auditServer->setLevel(9);
        processResignFederation(msg->federation, msg->federate);
        break ;

      case NetworkMessage::REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
        D.Out(pdTrace,
              "Federation %u: registerFedSyncPoint from federate %u.",
              msg->federation, msg->federate);
        auditServer->setLevel(8);
        processRegisterSynchronization(link, msg);
        break ;

      case NetworkMessage::SYNCHRONIZATION_POINT_ACHIEVED:
        D.Out(pdTrace,
              "Federation %u: synchronizationPointAchieved from federate %u.",
              msg->federation, msg->federate);
        auditServer->setLevel(8);
        processSynchronizationAchieved(link, msg);
        break ;

      case NetworkMessage::REQUEST_FEDERATION_SAVE:
        D.Out(pdTrace, "Request federation save from federate %u.",
              msg->federate);
        auditServer->setLevel(8);
        processRequestFederationSave(link, msg);
        break ;

      case NetworkMessage::FEDERATE_SAVE_BEGUN:
        D.Out(pdTrace, "Federate %u begun save.", msg->federate);
        auditServer->setLevel(8);
        processFederateSaveBegun(link, msg);
        break ;

      case NetworkMessage::FEDERATE_SAVE_COMPLETE:
      case NetworkMessage::FEDERATE_SAVE_NOT_COMPLETE:
        D.Out(pdTrace, "Federate %u save complete/not complete.",
              msg->federate);
        auditServer->setLevel(8);
        processFederateSaveStatus(link, msg);
        break ;

      case NetworkMessage::REQUEST_FEDERATION_RESTORE:
        D.Out(pdTrace, "Federate %u request a restoration.", msg->federate);
        auditServer->setLevel(8);
        processRequestFederationRestore(link, msg);
        break ;

      case NetworkMessage::FEDERATE_RESTORE_COMPLETE:
      case NetworkMessage::FEDERATE_RESTORE_NOT_COMPLETE:
        D.Out(pdTrace, "Federate %u restore complete/not complete.",
              msg->federate);
        auditServer->setLevel(8);
        processFederateRestoreStatus(link, msg);
        break ;

      case NetworkMessage::SET_TIME_REGULATING:
        D.Out(pdTrace, "SetTimeRegulating du federe %u(date=%f).",
              msg->federate, msg->date);
        auditServer->setLevel(8);
        processSetTimeRegulating(msg);
        break ;

      case NetworkMessage::SET_TIME_CONSTRAINED:
        D.Out(pdTrace, "SetTimeConstrained du federe %u.", msg->federate);
        auditServer->setLevel(8);
        processSetTimeConstrained(msg);
        break ;

      case NetworkMessage::PUBLISH_OBJECT_CLASS:
      case NetworkMessage::UNPUBLISH_OBJECT_CLASS:
        D.Out(pdTrace, "un/publishObjectClass.");
        auditServer->setLevel(7);
        processPublishObjectClass(link, msg);
        break ;

      case NetworkMessage::PUBLISH_INTERACTION_CLASS:
      case NetworkMessage::UNPUBLISH_INTERACTION_CLASS:
        D.Out(pdTrace, "un/publishInteractionClass.");
        auditServer->setLevel(7);
        processPublishInteractionClass(link, msg);
        break ;

      case NetworkMessage::SUBSCRIBE_OBJECT_CLASS:
      case NetworkMessage::UNSUBSCRIBE_OBJECT_CLASS:
        D.Out(pdTrace, "un/subscribeObjectClass.");
        auditServer->setLevel(7);
        processSubscribeObjectClass(link, msg);
        break ;

      case NetworkMessage::SUBSCRIBE_INTERACTION_CLASS:
      case NetworkMessage::UNSUBSCRIBE_INTERACTION_CLASS:
        D.Out(pdTrace, "un/subscribeInteractionClass.");
        auditServer->setLevel(7);
        processSubscribeInteractionClass(link, msg);
        break ;

      case NetworkMessage::REQUEST_ID:
        D.Out(pdTrace, "requestID.");
        auditServer->setLevel(6);
        processRequestId(link, msg);
        break ;

      case NetworkMessage::REGISTER_OBJECT:
        D.Out(pdTrace, "registerObject.");
        auditServer->setLevel(6);
        processRegisterObject(link, msg);
        break ;

      case NetworkMessage::DELETE_OBJECT:
        D.Out(pdTrace, "DeleteObject..");
        auditServer->setLevel(6);
        processDeleteObject(link, msg);
        break ;

      case NetworkMessage::IS_ATTRIBUTE_OWNED_BY_FEDERATE:
        D.Out(pdTrace, "isAttributeOwnedByFederate..");
        auditServer->setLevel(2);
        processAttributeOwnedByFederate(link, msg);
        break ;

      case NetworkMessage::QUERY_ATTRIBUTE_OWNERSHIP:
        D.Out(pdTrace, "queryAttributeOwnership..");
        auditServer->setLevel(2);
        processQueryAttributeOwnership(link, msg);
        break ;

      case NetworkMessage::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        D.Out(pdTrace, "negotiatedAttributeOwnershipDivestiture..");
        auditServer->setLevel(6);
        processNegotiatedOwnershipDivestiture(link, msg);
        break ;

      case NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
        D.Out(pdTrace, "attributeOwnershipAcquisitionIfAvailable..");
        auditServer->setLevel(6);
        processAcquisitionIfAvailable(link, msg);
        break ;

      case NetworkMessage::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        D.Out(pdTrace, "unconditionalAttributeOwnershipDivestiture..");
        auditServer->setLevel(6);
        processUnconditionalDivestiture(link, msg);
        break ;

      case NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION:
        D.Out(pdTrace, "attributeOwnershipAcquisition..");
        auditServer->setLevel(6);
        processOwnershipAcquisition(link, msg);
        break ;

      case NetworkMessage::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        D.Out(pdTrace, "cancelNegociatedAttributeOwnershipDivestiture..");
        auditServer->setLevel(6);
        processCancelNegotiatedDivestiture(link, msg);
        break ;

      case NetworkMessage::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
        D.Out(pdTrace, "attributeOwnershipReleaseResponse..");
        auditServer->setLevel(6);
        processReleaseResponse(link, msg);
        break ;

      case NetworkMessage::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
        D.Out(pdTrace, "cancelAttributeOwnershipAcquisition..");
        auditServer->setLevel(6);
        processCancelAcquisition(link, msg);
        break ;

      case NetworkMessage::CREATE_REGION:
        D[pdTrace] << "createRegion" << endl ;
        auditServer->setLevel(6);
        processCreateRegion(link, msg);
        break ;

      case NetworkMessage::DELETE_REGION:
        D[pdTrace] << "deleteRegion" << endl ;
        auditServer->setLevel(6);
        processDeleteRegion(link, msg);
        break ;

      default:
        // FIXME: Sould treat other cases CHANGE_*_ORDER/TRANSPORT_TYPE
        D.Out(pdError, "processMessageRecu: unknown type %u.", msg->type);
        throw RTIinternalError("Unknown Message Type");
    }

    return link ; // It may have been set to NULL by closeConnection.
}

// ----------------------------------------------------------------------------
//! closeConnection
/*! If a connection is closed in emergency, KillFederate will be called on
  federations attribute to remove all references to this federate.
*/
void
RTIG::closeConnection(Socket *link, bool emergency)
{
    Handle federation ;
    FederateHandle federate ;

    try {
        socketServer->close(link->returnSocket(), federation, federate);
    }
    catch (RTIinternalError &e) {
        D.Out(pdError, "Connection not found while trying to close it.");
    }

    if (emergency) {
        D.Out(pdExcept, "Killing Federate(%u, %u)...", federation, federate);
        federations->killFederate(federation, federate);
        D.Out(pdExcept, "Federate(%u, %u)Killed... ", federation, federate);
    }
}

// ----------------------------------------------------------------------------
// execute
void
RTIG::execute()
{
    int result ;
    fd_set fd ;
    Socket *link ;

    // create TCP and UDP connections for the RTIG server
    udpSocketServer.createUDPServer(udpPort);
    tcpSocketServer.createTCPServer(tcpPort);
    // udpSocketServer.createUDPServer(PORT_UDP_RTIG);

    cout << "RTIG up and running." << endl ;

    terminate = false ;

    while (!terminate) {
        // Initialize fd_set structure with all opened sockets.
        FD_ZERO(&fd);
        FD_SET(tcpSocketServer.returnSocket(), &fd);
        socketServer->addToFDSet(&fd);

        // Wait for an incoming message.
        result = 0 ;
        result = select(ulimit(4, 0), &fd, NULL, NULL, NULL);
        if ((result == -1)&& (errno == EINTR)) break ;

        // Is it a message from an already opened connection?
        link = socketServer->getActiveSocket(&fd);
        if (link != NULL) {
            D.Out(pdCom, "Incoming message on socket %ld.", link->returnSocket());
            try {
                do {
                    link = processIncomingMessage((SecureTCPSocket *)link);
                    if (link == NULL)break ;
                } while (link->isDataReady()== RTI_TRUE);
            }
            catch (NetworkError &e) {
                if (e._reason != NULL)
                    D.Out(pdExcept, "Catching Network Error, reason : %s", e._reason);
                else
                    D.Out(pdExcept, "Catching Network Error, no reason string.");
                cout << "RTIG dropping client connection " << link->returnSocket()
                     << '.' << endl ;
                closeConnection((SecureTCPSocket *)link, true);
                link = NULL ;
            }
        }

        // Or on the server socket ?
        if (FD_ISSET(tcpSocketServer.returnSocket(), &fd)) {
            D.Out(pdCom, "Demande de connexion.");
            openConnection();
        }
    }
}

// ----------------------------------------------------------------------------
// openConnection

void
RTIG::openConnection()
{
    try {
        socketServer->open();
    }
    catch (RTIinternalError &e) {
        D.Out(pdExcept, "Error while accepting new connection : %s.", e._reason);
    }

    D.Out(pdInit, "Accepting new connection.");
}

// ----------------------------------------------------------------------------
//! process incoming messages.
/*! This module works as follows:

Each processXXX module processes its own answer and any broadcast needed.
processXXX module calling is decided by the ChooseProcessingMethod module.
But if an exception occurs while processing a message, the exception is
caught by this module. Then a message, similar to the received one is sent
on the link. This message only holds the exception.
*/
Socket*
RTIG::processIncomingMessage(Socket *link)
{
    NetworkMessage msg ;
    NetworkMessage rep ; // Server Answer(only if an exception is raised)
    char buffer[256] ; // To store the exception reason

    if (link == NULL) {
        D.Out(pdError, "NULL socket in processMessageRecu.");
        return NULL ;
    }

    msg.read(link);

    rep.type = msg.type ;
    rep.exception = e_NO_EXCEPTION ;
    rep.federate = msg.federate ;

    auditServer->startLine(msg.federation, msg.federate, msg.type);

    // This macro is used to copy any non null exception reason
    // string into our buffer(used for Audit purpose).

#define CPY_NOT_NULL(A) { if (A._reason != NULL)strcpy(buffer, A._reason); }

    buffer[0] = 0 ;

    try {
        link = chooseProcessingMethod(link, &msg);
    }
    catch (ArrayIndexOutOfBounds &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_ArrayIndexOutOfBounds ;
    }
    catch (AttributeAlreadyOwned &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_AttributeAlreadyOwned ;
    }
    catch (AttributeAlreadyBeingAcquired &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_AttributeAlreadyBeingAcquired ;
    }
    catch (AttributeAlreadyBeingDivested &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_AttributeAlreadyBeingDivested ;
    }
    catch (AttributeDivestitureWasNotRequested &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_AttributeDivestitureWasNotRequested ;
    }
    catch (AttributeAcquisitionWasNotRequested &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_AttributeAcquisitionWasNotRequested ;
    }
    catch (AttributeNotDefined &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_AttributeNotDefined ;
    }
    catch (AttributeNotKnown &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_AttributeNotKnown ;
    }
    catch (AttributeNotOwned &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_AttributeNotOwned ;
    }
    catch (AttributeNotPublished &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_AttributeNotPublished ;
    }
    catch (AttributeNotSubscribed &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_AttributeNotSubscribed ;
    }
    catch (ConcurrentAccessAttempted &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_ConcurrentAccessAttempted ;
    }
    catch (CouldNotDiscover &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_CouldNotDiscover ;
    }
    catch (CouldNotOpenRID &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_CouldNotOpenRID ;
    }
    catch (CouldNotRestore &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_CouldNotRestore ;
    }
    catch (DeletePrivilegeNotHeld &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_DeletePrivilegeNotHeld ;
    }
    catch (ErrorReadingRID &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_ErrorReadingRID ;
    }
    catch (EventNotKnown &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_EventNotKnown ;
    }
    catch (FederateAlreadyPaused &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_FederateAlreadyPaused ;
    }
    catch (FederateAlreadyExecutionMember &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_FederateAlreadyExecutionMember ;
    }
    catch (FederateDoesNotExist &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_FederateDoesNotExist ;
    }
    catch (FederateInternalError &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_FederateInternalError ;
    }
    catch (FederateNameAlreadyInUse &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_FederateNameAlreadyInUse ;
    }
    catch (FederateNotExecutionMember &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_FederateNotExecutionMember ;
    }
    catch (FederateNotPaused &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_FederateNotPaused ;
    }
    catch (FederateNotPublishing &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_FederateNotPublishing ;
    }
    catch (FederateNotSubscribing &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_FederateNotSubscribing ;
    }
    catch (FederateOwnsAttributes &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_FederateOwnsAttributes ;
    }
    catch (FederatesCurrentlyJoined &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_FederatesCurrentlyJoined ;
    }
    catch (FederateWasNotAskedToReleaseAttribute &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_FederateWasNotAskedToReleaseAttribute ;
    }
    catch (FederationAlreadyPaused &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_FederationAlreadyPaused ;
    }
    catch (FederationExecutionAlreadyExists &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_FederationExecutionAlreadyExists ;
    }
    catch (FederationExecutionDoesNotExist &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_FederationExecutionDoesNotExist ;
    }
    catch (FederationNotPaused &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_FederationNotPaused ;
    }
    catch (FederationTimeAlreadyPassed &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_FederationTimeAlreadyPassed ;
    }
    catch (IDsupplyExhausted &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_IDsupplyExhausted ;
    }
    catch (InteractionClassNotDefined &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_InteractionClassNotDefined ;
    }
    catch (InteractionClassNotKnown &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_InteractionClassNotKnown ;
    }
    catch (InteractionClassNotPublished &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_InteractionClassNotPublished ;
    }
    catch (InteractionParameterNotDefined &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_InteractionParameterNotDefined ;
    }
    catch (InteractionParameterNotKnown &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_InteractionParameterNotKnown ;
    }
    catch (InvalidDivestitureCondition &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_InvalidDivestitureCondition ;
    }
    catch (InvalidExtents &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_InvalidExtents ;
    }
    catch (InvalidFederationTime &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_InvalidFederationTime ;
    }
    catch (InvalidFederationTimeDelta &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_InvalidFederationTimeDelta ;
    }
    catch (InvalidObjectHandle &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_InvalidObjectHandle ;
    }
    catch (InvalidOrderType &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_InvalidOrderType ;
    }
    catch (InvalidResignAction &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_InvalidResignAction ;
    }
    catch (InvalidRetractionHandle &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_InvalidRetractionHandle ;
    }
    catch (InvalidRoutingSpace &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_InvalidRoutingSpace ;
    }
    catch (InvalidTransportType &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_InvalidTransportType ;
    }
    catch (MemoryExhausted &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_MemoryExhausted ;
    }
    catch (NameNotFound &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_NameNotFound ;
    }
    catch (NoPauseRequested &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_NoPauseRequested ;
    }
    catch (NoResumeRequested &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_NoResumeRequested ;
    }
    catch (ObjectClassNotDefined &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_ObjectClassNotDefined ;
    }
    catch (ObjectClassNotKnown &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_ObjectClassNotKnown ;
    }
    catch (ObjectClassNotPublished &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_ObjectClassNotPublished ;
    }
    catch (ObjectClassNotSubscribed &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_ObjectClassNotSubscribed ;
    }
    catch (ObjectNotKnown &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_ObjectNotKnown ;
    }
    catch (ObjectAlreadyRegistered &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_ObjectAlreadyRegistered ;
    }
    catch (RegionNotKnown &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_RegionNotKnown ;
    }
    catch (RestoreInProgress &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_RestoreInProgress ;
    }
    catch (RestoreNotRequested &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_RestoreNotRequested ;
    }
    catch (RTIinternalError &e) {
        if (e._reason == NULL)
            D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        else
            D.Out(pdExcept, "Catching \"%s\" exception: %s.", e._name, e._reason);
        CPY_NOT_NULL(e);
        rep.exception = e_RTIinternalError ;
    }
    catch (SaveInProgress &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_SaveInProgress ;
    }
    catch (SaveNotInitiated &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_SaveNotInitiated ;
    }
    catch (SecurityError &e) {
        cout << endl << "Security Error : " << e._reason << endl ;
        CPY_NOT_NULL(e);
        rep.exception = e_SecurityError ;
    }
    catch (SpaceNotDefined &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_SpaceNotDefined ;
    }
    catch (SpecifiedSaveLabelDoesNotExist &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_SpecifiedSaveLabelDoesNotExist ;
    }
    catch (TimeAdvanceAlreadyInProgress &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_TimeAdvanceAlreadyInProgress ;
    }
    catch (TimeAdvanceWasNotInProgress &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_TimeAdvanceWasNotInProgress ;
    }
    catch (TooManyIDsRequested &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_TooManyIDsRequested ;
    }
    catch (UnableToPerformSave &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_UnableToPerformSave ;
    }
    catch (UnimplementedService &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_UnimplementedService ;
    }
    catch (UnknownLabel &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_UnknownLabel ;
    }
    catch (ValueCountExceeded &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_ValueCountExceeded ;
    }
    catch (ValueLengthExceeded &e) {
        D.Out(pdExcept, "Catching \"%s\" exception.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_ValueLengthExceeded ;
    }

    // Non RTI specific exception, Client connection problem(internal)
    catch (NetworkError &e) {
        strcpy(buffer, " - NetworkError");
        auditServer->endLine(rep.exception, buffer);

        throw e ;
    }

    // Default Handler
    catch (Exception &e) {
        D.Out(pdExcept, "Unknown Exception : %s.", e._name);
        CPY_NOT_NULL(e);
        rep.exception = e_RTIinternalError ;
    }

    // buffer may contain an exception reason. If not, set it to OK
    // or Exception
    if (strlen(buffer)== 0) {
        if (rep.exception == e_NO_EXCEPTION)
            strcpy(buffer, " - OK");
        else
            strcpy(buffer, " - Exception");
    }

    auditServer->endLine(rep.exception, buffer);

    if (link == NULL) return link ;

    if (rep.exception != e_NO_EXCEPTION) {
        rep.write(link);
        D.Out(pdExcept,
              "RTIG catched exception %d and sent it back to federate %d.",
              rep.exception, rep.federate);
    }

    return link ;
}

// ----------------------------------------------------------------------------
//! process received signals.
void
RTIG::signalHandler(int sig)
{
    D.Out(pdError, "Received Signal %d.", sig);

    if (sig == SIGINT) terminate = true ;
    if (sig == SIGPIPE) cout << "Ignoring 'Broken pipe' signal." << endl ;
}

}} // namespace certi/rtig

// $Id: RTIG.cc,v 3.13 2003/06/25 16:23:48 breholee Exp $
