// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libRTI
//
// CERTI-libRTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libRTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: RTIambassador.cc,v 3.35 2003/10/20 12:39:04 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIambassador.hh"

#include "FederateAmbassador.hh"
#include "Message.hh"
#include "PrettyDebug.hh"

#include <signal.h>
#include <iostream>
#include <unistd.h>

using std::cout ;
using std::cerr ;
using std::endl ;

namespace certi {

static pdCDebug D("LIBRTI", __FILE__);

// ----------------------------------------------------------------------------
void
RTIambassador::leave(const char *msg)
{
    cout << "libRTI: " << msg << endl ;
    exit(EXIT_FAILURE);
}

// ----------------------------------------------------------------------------
void
RTIambassador::executeService(Message *req, Message *rep)
{
    // raise exception if reentrant call.
    if (is_reentrant)
        throw ConcurrentAccessAttempted();

    D.Out(pdDebug, "sending request to RTIA.");

    is_reentrant = true ;
    try {
        req->write((SocketUN *) this);
    }
    catch (NetworkError) {
        cerr << "libRTI: exception: NetworkError (write)" << endl ;
        throw RTIinternalError("libRTI: Network Write Error");
    }

    D.Out(pdDebug, "waiting RTIA reply.");

    // waiting RTI reply.
    try {
        rep->read((SocketUN *) this);
    }
    catch (NetworkError) {
        cerr << "libRTI: exception: NetworkError (read)" << endl ;
        throw RTIinternalError("libRTI: Network Read Error");
    }

    D.Out(pdDebug, "RTIA reply received.");

    if (rep->type != req->type) {
        cout << "LibRTI: Assertion failed: rep->type != req->type" << endl ;
        throw RTIinternalError("RTIambassador::executeService: "
                               "rep->type != req->type");
    }

    is_reentrant = false ;
    D.Out(pdDebug, "processing returned exception (from reply).");
    processException(rep);
    D.Out(pdDebug, "exception processed.");
}

// ----------------------------------------------------------------------------
//! Start RTIambassador processes for communication with RTIG.
/*! When a new RTIambassador is created in the application, a new process rtia
  is launched. This process is used for data exchange with rtig server.
  This process connects to rtia after one second delay (UNIX socket).
*/
RTIambassador::RTIambassador()
    throw (MemoryExhausted, RTIinternalError) : SocketUN(stIgnoreSignal)
{
    is_reentrant = false ;
    const char *rtiaexec = "rtia" ;
    const char *rtiaenv = getenv("CERTI_RTIA");
    const char *rtiacall ;
    if (rtiaenv) rtiacall = rtiaenv ;
    else rtiacall = rtiaexec ;

    // creating RTIA process.
    switch((pid_RTIA = fork())) {
      case -1: // fork failed.
        perror("fork");
        throw RTIinternalError("fork failed in RTIambassador constructor");
        break ;

      case 0: // child process (RTIA).
        execlp(rtiacall, NULL);
        perror("execlp");

        cerr << "Could not launch RTIA process." << endl
             << "Maybe RTIA is not in search PATH environment." << endl ;

        exit(-1);

      default: // father process (Federe).
        sleep(1);
        connectUN(pid_RTIA);
        break ;
    }
}

// ----------------------------------------------------------------------------
//! Closes processes.
/*! When destructor is called, kill rtia process.
 */
RTIambassador::~RTIambassador()
{
    kill(pid_RTIA, SIGINT);
}

// ----------------------------------------------------------------------------
// Tick
Boolean
RTIambassador::tick()
    throw (SpecifiedSaveLabelDoesNotExist,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message vers_RTI, vers_Fed ;

    // Throw exception if reentrant call.
    if (is_reentrant)
        throw ConcurrentAccessAttempted();

    is_reentrant = true ;

    // Prevenir le RTI
    vers_RTI.type = Message::TICK_REQUEST ;

    try {
        vers_RTI.write((SocketUN *) this);
    }
    catch (NetworkError) {
        cout << "tick 1." << endl ;
        cout << "LibRTI: Catched NetworkError, throw RTIinternalError."
             << endl ;
        throw RTIinternalError();
    }

    for (;;) {

        // Lire la reponse du RTIA local
        try {
            vers_Fed.read((SocketUN *) this);
        }
        catch (NetworkError) {
            cout << "tick 2." << endl ;
            cout << "LibRTI: Catched NetworkError, throw RTIinternalError."
                 << endl ;
            throw RTIinternalError();
        }

        // Si c'est de type TICK_REQUEST, il n'y a qu'a traiter l'exception.
        if (vers_Fed.type == Message::TICK_REQUEST) {
            is_reentrant = false ;
            processException(&vers_Fed);
            return vers_Fed.getBoolean();
        }

        // Sinon, le RTI nous demande un service, donc on appele une methode
        // du FederateAmbassador.

        vers_RTI.setException(e_NO_EXCEPTION);
        try {
            switch (vers_Fed.type) {

              case Message::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
                fed_amb->synchronizationPointRegistrationSucceeded(
		    vers_Fed.getLabel());
                break ;

              case Message::ANNOUNCE_SYNCHRONIZATION_POINT:
                fed_amb->announceSynchronizationPoint(vers_Fed.getLabel(),
                                                      vers_Fed.getTag());
                break ;

              case Message::FEDERATION_SYNCHRONIZED:
                fed_amb->federationSynchronized(vers_Fed.getLabel());
                break ;

              case Message::INITIATE_FEDERATE_SAVE:
                fed_amb->initiateFederateSave(vers_Fed.getLabel());
                break ;

              case Message::FEDERATION_SAVED:
                fed_amb->federationSaved();
                break ;

              case Message::REQUEST_FEDERATION_RESTORE_SUCCEEDED:
                fed_amb->requestFederationRestoreSucceeded(
                    vers_Fed.getLabel());
                break ;

              case Message::REQUEST_FEDERATION_RESTORE_FAILED:
                fed_amb->requestFederationRestoreFailed(vers_Fed.getLabel(),
                                                        vers_Fed.getTag());
                break ;

              case Message::FEDERATION_RESTORE_BEGUN:
                fed_amb->federationRestoreBegun();
                break ;

              case Message::INITIATE_FEDERATE_RESTORE:
                fed_amb->initiateFederateRestore(vers_Fed.getLabel(),
                                                 vers_Fed.getFederate());
                break ;

              case Message::FEDERATION_RESTORED:
                fed_amb->federationRestored();
                break ;

              case Message::FEDERATION_NOT_RESTORED:
                fed_amb->federationNotRestored();
                break ;

              case Message::START_REGISTRATION_FOR_OBJECT_CLASS: {
                  fed_amb->startRegistrationForObjectClass(
		      vers_Fed.getObjectClass());
              } break ;

              case Message::STOP_REGISTRATION_FOR_OBJECT_CLASS: {
                  fed_amb->
                      stopRegistrationForObjectClass(vers_Fed.getObjectClass());
              } break ;

              case Message::TURN_INTERACTIONS_ON: {
                  fed_amb->turnInteractionsOn(vers_Fed.getInteractionClass());
              } break ;

              case Message::TURN_INTERACTIONS_OFF: {
                  fed_amb->turnInteractionsOff(vers_Fed.getInteractionClass());
              } break ;

              case Message::DISCOVER_OBJECT_INSTANCE: {
                  fed_amb->
                      discoverObjectInstance(vers_Fed.getObject(),
                                             vers_Fed.getObjectClass(),
                                             vers_Fed.getName());
              } break ;

              case Message::REFLECT_ATTRIBUTE_VALUES: {
                  AttributeHandleValuePairSet *attributes = vers_Fed.getAHVPS();
                  fed_amb->
                      reflectAttributeValues(vers_Fed.getObject(),
                                             *attributes,
                                             vers_Fed.getFedTime(),
                                             vers_Fed.getTag(),
                                             vers_Fed.getEventRetraction());

                  delete attributes ;
              } break ;

              case Message::RECEIVE_INTERACTION: {
                  ParameterHandleValuePairSet *parameters = vers_Fed.getPHVPS();

                  fed_amb->receiveInteraction(vers_Fed.getInteractionClass(),
                                              *parameters,
                                              vers_Fed.getFedTime(),
                                              vers_Fed.getTag(),
                                              vers_Fed.getEventRetraction());

                  delete parameters ;
              } break ;

              case Message::REMOVE_OBJECT_INSTANCE: {
                  fed_amb->removeObjectInstance(vers_Fed.getObject(),
                                                vers_Fed.getFedTime(),
                                                vers_Fed.getTag(),
                                                vers_Fed.getEventRetraction());
              } break ;

              case Message::PROVIDE_ATTRIBUTE_VALUE_UPDATE: {



                  // fed_amb->provideAttributeValueUpdate();
              } break ;

              case Message::REQUEST_RETRACTION: {

              } break ;

              case Message::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION: {
                  AttributeHandleSet *attributeSet = vers_Fed.getAHS();

                  fed_amb->
                      requestAttributeOwnershipAssumption(vers_Fed.getObject(),
                                                          *attributeSet,
                                                          vers_Fed.getTag());
                  delete attributeSet ;
              } break ;

              case Message::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE: {
                  AttributeHandleSet *attributeSet = vers_Fed.getAHS();

                  fed_amb->requestAttributeOwnershipRelease(
		      vers_Fed.getObject(),
		      *attributeSet,
		      vers_Fed.getTag());

                  delete attributeSet ;
              } break ;

              case Message::ATTRIBUTE_OWNERSHIP_UNAVAILABLE: {
                  AttributeHandleSet *attributeSet = vers_Fed.getAHS();

                  fed_amb->attributeOwnershipUnavailable(vers_Fed.getObject(),
                                                         *attributeSet);

                  delete attributeSet ;
              } break ;

              case Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION: {
                  AttributeHandleSet *attributeSet = vers_Fed.getAHS();

                  fed_amb->attributeOwnershipAcquisitionNotification(
		      vers_Fed.getObject(),
		      *attributeSet);

                  delete attributeSet ;
              } break ;

              case Message::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION: {
                  AttributeHandleSet *attributeSet = vers_Fed.getAHS();

                  fed_amb->attributeOwnershipDivestitureNotification(
		      vers_Fed.getObject(),
		      *attributeSet);

                  delete attributeSet ;
              } break ;

              case Message::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION: {
                  AttributeHandleSet *attributeSet = vers_Fed.getAHS();

                  fed_amb->
                      confirmAttributeOwnershipAcquisitionCancellation
                      (vers_Fed.getObject(), *attributeSet);

                  delete attributeSet ;
              } break ;

              case Message::INFORM_ATTRIBUTE_OWNERSHIP: {
                  fed_amb->
                      informAttributeOwnership(vers_Fed.getObject(),
                                               vers_Fed.getAttribute(),
                                               vers_Fed.getFederate());
              } break ;

              case Message::ATTRIBUTE_IS_NOT_OWNED: {
                  fed_amb->attributeIsNotOwned(vers_Fed.getObject(),
                                               vers_Fed.getAttribute());
              } break ;

              case Message::TIME_ADVANCE_GRANT: {
                  fed_amb->timeAdvanceGrant(vers_Fed.getFedTime());
              } break ;

              default: {
                  leave("RTI service requested by RTI is unknown.");
              }
            }
        }
        catch (InvalidFederationTime &e) {
            vers_RTI.setException(e_InvalidFederationTime, e._reason);
        }
        catch (TimeAdvanceWasNotInProgress &e) {
            vers_RTI.setException(e_TimeAdvanceWasNotInProgress, e._reason);
        }
        catch (FederationTimeAlreadyPassed &e) {
            vers_RTI.setException(e_FederationTimeAlreadyPassed, e._reason);
        }
        catch (FederateInternalError &e) {
            vers_RTI.setException(e_FederateInternalError, e._reason);
        }
        catch (Exception &e) {
            vers_RTI.setException(e_RTIinternalError, e._reason);
        }

        // retourner au RTI la reponse du service demande
        vers_RTI.type = vers_Fed.type ;

        try {
            vers_RTI.write((SocketUN *) this);
        }
        catch (NetworkError) {
            cout << "tick 3." << endl ;
            cout << "LibRTI: Catched NetworkError, throw RTIinternalError."
                 << endl ;
            throw RTIinternalError();
        }
    }
}

// ----------------------------------------------------------------------------
Boolean
RTIambassador::tick(TickTime minimum, TickTime maximum)
    throw (SpecifiedSaveLabelDoesNotExist, ConcurrentAccessAttempted,
           RTIinternalError)
{
    return tick();
}

// ----------------------------------------------------------------------------
//! Process exception from received message.
/*! When a message is received from RTIA, it can contains an exception.
  This exception is processed by this module and a new exception is thrown.
*/
void
RTIambassador::processException(Message *msg)
{
    D.Out(pdExcept, "n° de l'exception : %d .", msg->getExceptionType());
    switch(msg->getExceptionType()) {
      case e_NO_EXCEPTION: {
      } break ;

      case e_ArrayIndexOutOfBounds: {
          D.Out(pdExcept, "Throwing e_ArrayIndexOutOfBounds exception.");
          throw ArrayIndexOutOfBounds(msg->getExceptionReason());
      } break ;

      case e_AttributeAlreadyOwned: {
          D.Out(pdExcept, "Throwing e_AttributeAlreadyOwned exception.");
          throw AttributeAlreadyOwned(msg->getExceptionReason());
      } break ;

      case e_AttributeAlreadyBeingAcquired: {
          D.Out(pdExcept,
		"Throwing e_AttributeAlreadyBeingAcquired exception.");
          throw AttributeAlreadyBeingAcquired(msg->getExceptionReason());
      } break ;

      case e_AttributeAlreadyBeingDivested: {
          D.Out(pdExcept,
		"Throwing e_AttributeAlreadyBeingDivested exception.");
          throw AttributeAlreadyBeingDivested(msg->getExceptionReason());
      } break ;

      case e_AttributeDivestitureWasNotRequested: {
          D.Out(pdExcept,
                "Throwing e_AttributeDivestitureWasNotRequested exception.");
          throw AttributeDivestitureWasNotRequested(msg->getExceptionReason());
      } break ;

      case e_AttributeAcquisitionWasNotRequested: {
          D.Out(pdExcept,
                "Throwing e_AttributeAcquisitionWasNotRequested exception.");
          throw AttributeAcquisitionWasNotRequested(msg->getExceptionReason());
      } break ;

      case e_AttributeNotDefined: {
          D.Out(pdExcept, "Throwing e_AttributeNotDefined exception.");
          throw AttributeNotDefined(msg->getExceptionReason());
      } break ;

      case e_AttributeNotKnown: {
          D.Out(pdExcept, "Throwing e_AttributeNotKnown exception.");
          throw AttributeNotKnown(msg->getExceptionReason());
      } break ;

      case e_AttributeNotOwned: {
          D.Out(pdExcept, "Throwing e_AttributeNotOwned exception.");
          throw AttributeNotOwned(msg->getExceptionReason());
      } break ;

      case e_AttributeNotPublished: {
          D.Out(pdExcept, "Throwing e_AttributeNotPublished exception.");
          throw AttributeNotPublished(msg->getExceptionReason());
      } break ;

      case e_AttributeNotSubscribed: {
          D.Out(pdExcept, "Throwing e_AttributeNotSubscribed exception.");
          throw AttributeNotSubscribed(msg->getExceptionReason());
      } break ;

      case e_ConcurrentAccessAttempted: {
          D.Out(pdExcept, "Throwing e_ConcurrentAccessAttempted exception.");
          throw ConcurrentAccessAttempted(msg->getExceptionReason());
      } break ;

      case e_CouldNotDiscover: {
          D.Out(pdExcept, "Throwing e_CouldNotDiscover exception.");
          throw CouldNotDiscover(msg->getExceptionReason());
      } break ;

      case e_CouldNotOpenRID: {
          D.Out(pdExcept, "Throwing e_CouldNotOpenRID exception.");
          throw CouldNotOpenRID(msg->getExceptionReason());
      } break ;

      case e_CouldNotRestore: {
          D.Out(pdExcept, "Throwing e_CouldNotRestore exception.");
          throw CouldNotRestore(msg->getExceptionReason());
      } break ;

      case e_DeletePrivilegeNotHeld: {
          D.Out(pdExcept, "Throwing e_DeletePrivilegeNotHeld exception.");
          throw DeletePrivilegeNotHeld(msg->getExceptionReason());
      } break ;

      case e_ErrorReadingRID: {
          D.Out(pdExcept, "Throwing e_ErrorReadingRID exception.");
          throw ErrorReadingRID(msg->getExceptionReason());
      } break ;

      case e_EventNotKnown: {
          D.Out(pdExcept, "Throwing e_EventNotKnown exception.");
          throw EventNotKnown(msg->getExceptionReason());
      } break ;

      case e_FederateAlreadyPaused: {
          D.Out(pdExcept, "Throwing e_FederateAlreadyPaused exception.");
          throw FederateAlreadyPaused(msg->getExceptionReason());
      } break ;

      case e_FederateAlreadyExecutionMember: {
          D.Out(pdExcept,
		"Throwing e_FederateAlreadyExecutionMember exception.");
          throw FederateAlreadyExecutionMember(msg->getExceptionReason());
      } break ;

      case e_FederateDoesNotExist: {
          D.Out(pdExcept, "Throwing e_FederateDoesNotExist exception.");
          throw FederateDoesNotExist(msg->getExceptionReason());
      } break ;

      case e_FederateInternalError: {
          D.Out(pdExcept, "Throwing e_FederateInternalError exception.");
          throw FederateInternalError(msg->getExceptionReason());
      } break ;

      case e_FederateNameAlreadyInUse: {
          D.Out(pdExcept, "Throwing e_FederateNameAlreadyInUse exception.");
          throw FederateNameAlreadyInUse(msg->getExceptionReason());
      } break ;

      case e_FederateNotExecutionMember: {
          D.Out(pdExcept, "Throwing e_FederateNotExecutionMember exception.");
          throw FederateNotExecutionMember(msg->getExceptionReason());
      } break ;

      case e_FederateNotPaused: {
          D.Out(pdExcept, "Throwing e_FederateNotPaused exception.");
          throw FederateNotPaused(msg->getExceptionReason());
      } break ;

      case e_FederateOwnsAttributes: {
          D.Out(pdExcept, "Throwing e_FederateOwnsAttributes exception.");
          throw FederateOwnsAttributes(msg->getExceptionReason());
      } break ;

      case e_FederatesCurrentlyJoined: {
          D.Out(pdExcept, "Throwing e_FederatesCurrentlyJoined exception.");
          throw FederatesCurrentlyJoined(msg->getExceptionReason());
      } break ;

      case e_FederateWasNotAskedToReleaseAttribute: {
          D.Out(pdExcept,
                "Throwing e_FederateWasNotAskedToReleaseAttribute exception.");
          D.Out(pdDebug,
                "Throwing e_FederateWasNotAskedToReleaseAttribute exception.");
          throw
	      FederateWasNotAskedToReleaseAttribute(msg->getExceptionReason());
      } break ;

      case e_FederationAlreadyPaused: {
          D.Out(pdExcept, "Throwing e_FederationAlreadyPaused exception.");
          throw FederationAlreadyPaused(msg->getExceptionReason());
      } break ;

      case e_FederationExecutionAlreadyExists: {
          D.Out(pdExcept, "Throwing e_FederationExecutionAlreadyExists excep.");
          throw FederationExecutionAlreadyExists(msg->getExceptionReason());
      } break ;

      case e_FederationExecutionDoesNotExist: {
          D.Out(pdExcept, "Throwing e_FederationExecutionDoesNotExist except.");
          throw FederationExecutionDoesNotExist(msg->getExceptionReason());
      } break ;

      case e_FederationNotPaused: {
          D.Out(pdExcept, "Throwing e_FederationNotPaused exception.");
          throw FederationNotPaused(msg->getExceptionReason());
      } break ;

      case e_FederationTimeAlreadyPassed: {
          D.Out(pdExcept, "Throwing e_FederationTimeAlreadyPassed exception.");
          throw FederationTimeAlreadyPassed(msg->getExceptionReason());
      } break ;

      case e_FederateNotPublishing: {
          D.Out(pdExcept, "Throwing e_FederateNotPublishing exception.");
          throw FederateNotPublishing(msg->getExceptionReason());
      } break ;

      case e_FederateNotSubscribing: {
          D.Out(pdExcept, "Throwing e_FederateNotSubscribing exception.");
          throw FederateNotSubscribing(msg->getExceptionReason());
      } break ;

      case e_RegionNotKnown: {
          D.Out(pdExcept, "Throwing e_RegionNotKnown exception.");
          throw RegionNotKnown(msg->getExceptionReason());
      } break ;

      case e_IDsupplyExhausted: {
          D.Out(pdExcept, "Throwing e_IDsupplyExhausted exception.");
          throw IDsupplyExhausted(msg->getExceptionReason());
      } break ;

      case e_InteractionClassNotDefined: {
          D.Out(pdExcept, "Throwing e_InteractionClassNotDefined exception.");
          throw InteractionClassNotDefined(msg->getExceptionReason());
      } break ;

      case e_InteractionClassNotKnown: {
          D.Out(pdExcept, "Throwing e_InteractionClassNotKnown exception.");
          throw InteractionClassNotKnown(msg->getExceptionReason());
      } break ;

      case e_InteractionClassNotPublished: {
          D.Out(pdExcept, "Throwing e_InteractionClassNotPublished exception.");
          throw InteractionClassNotPublished(msg->getExceptionReason());
      } break ;

      case e_InteractionParameterNotDefined: {
          D.Out(pdExcept,
		"Throwing e_InteractionParameterNotDefined exception.");
          throw InteractionParameterNotDefined(msg->getExceptionReason());
      } break ;

      case e_InteractionParameterNotKnown: {
          D.Out(pdExcept, "Throwing e_InteractionParameterNotKnown exception.");
          throw InteractionParameterNotKnown(msg->getExceptionReason());
      } break ;

      case e_InvalidDivestitureCondition: {
          D.Out(pdExcept, "Throwing e_InvalidDivestitureCondition exception.");
          throw InvalidDivestitureCondition(msg->getExceptionReason());
      } break ;

      case e_InvalidExtents: {
          D.Out(pdExcept, "Throwing e_InvalidExtents exception.");
          throw InvalidExtents(msg->getExceptionReason());
      } break ;

      case e_InvalidFederationTime: {
          D.Out(pdExcept, "Throwing e_InvalidFederationTime exception.");
          throw InvalidFederationTime(msg->getExceptionReason());
      } break ;

      case e_InvalidFederationTimeDelta: {
          D.Out(pdExcept, "Throwing e_InvalidFederationTimeDelta exception.");
          throw InvalidFederationTimeDelta(msg->getExceptionReason());
      } break ;

      case e_InvalidObjectHandle: {
          D.Out(pdExcept, "Throwing e_InvalidObjectHandle exception.");
          throw InvalidObjectHandle(msg->getExceptionReason());
      } break ;

      case e_InvalidOrderType: {
          D.Out(pdExcept, "Throwing e_InvalidOrderType exception.");
          throw InvalidOrderType(msg->getExceptionReason());
      } break ;

      case e_InvalidResignAction: {
          D.Out(pdExcept, "Throwing e_InvalidResignAction exception.");
          throw InvalidResignAction(msg->getExceptionReason());
      } break ;

      case e_InvalidRetractionHandle: {
          D.Out(pdExcept, "Throwing e_InvalidRetractionHandle exception.");
          throw InvalidRetractionHandle(msg->getExceptionReason());
      } break ;

      case e_InvalidRoutingSpace: {
          D.Out(pdExcept, "Throwing e_InvalidRoutingSpace exception.");
          throw InvalidRoutingSpace(msg->getExceptionReason());
      } break ;

      case e_InvalidTransportType: {
          D.Out(pdExcept, "Throwing e_InvalidTransportType exception.");
          throw InvalidTransportType(msg->getExceptionReason());
      } break ;

      case e_MemoryExhausted: {
          D.Out(pdExcept, "Throwing e_MemoryExhausted exception.");
          throw MemoryExhausted(msg->getExceptionReason());
      } break ;

      case e_NameNotFound: {
          D.Out(pdExcept, "Throwing e_NameNotFound exception.");
          throw NameNotFound(msg->getExceptionReason());
      } break ;

      case e_NoPauseRequested: {
          D.Out(pdExcept, "Throwing e_NoPauseRequested exception.");
          throw NoPauseRequested(msg->getExceptionReason());
      } break ;

      case e_NoResumeRequested: {
          D.Out(pdExcept, "Throwing e_NoResumeRequested exception.");
          throw NoResumeRequested(msg->getExceptionReason());
      } break ;

      case e_ObjectClassNotDefined: {
          D.Out(pdExcept, "Throwing e_ObjectClassNotDefined exception.");
          throw ObjectClassNotDefined(msg->getExceptionReason());
      } break ;

      case e_ObjectClassNotKnown: {
          D.Out(pdExcept, "Throwing e_ObjectClassNotKnown exception.");
          throw ObjectClassNotKnown(msg->getExceptionReason());
      } break ;

      case e_ObjectClassNotPublished: {
          D.Out(pdExcept, "Throwing e_ObjectClassNotPublished exception.");
          throw ObjectClassNotPublished(msg->getExceptionReason());
      } break ;

      case e_ObjectClassNotSubscribed: {
          D.Out(pdExcept, "Throwing e_ObjectClassNotSubscribed exception.");
          throw ObjectClassNotSubscribed(msg->getExceptionReason());
      } break ;

      case e_ObjectNotKnown: {
          D.Out(pdExcept, "Throwing e_ObjectNotKnown exception.");
          throw ObjectNotKnown(msg->getExceptionReason());
      } break ;

      case e_ObjectAlreadyRegistered: {
          D.Out(pdExcept, "Throwing e_ObjectAlreadyRegistered exception.");
          throw ObjectAlreadyRegistered(msg->getExceptionReason());
      } break ;

      case e_RestoreInProgress: {
          D.Out(pdExcept, "Throwing e_RestoreInProgress exception.");
          throw RestoreInProgress(msg->getExceptionReason());
      } break ;

      case e_RestoreNotRequested: {
          D.Out(pdExcept, "Throwing e_RestoreNotRequested exception.");
          throw RestoreNotRequested(msg->getExceptionReason());
      } break ;

      case e_RTIinternalError: {
          D.Out(pdExcept, "Throwing e_RTIinternalError exception.");
          throw RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_SpaceNotDefined: {
          D.Out(pdExcept, "Throwing e_SpaceNotDefined exception.");
          throw SpaceNotDefined(msg->getExceptionReason());
      } break ;

      case e_SaveInProgress: {
          D.Out(pdExcept, "Throwing e_SaveInProgress exception.");
          throw SaveInProgress(msg->getExceptionReason());
      } break ;

      case e_SaveNotInitiated: {
          D.Out(pdExcept, "Throwing e_SaveNotInitiated exception.");
          throw SaveNotInitiated(msg->getExceptionReason());
      } break ;

      case e_SecurityError: {
          D.Out(pdExcept, "Throwing e_SecurityError exception.");
          throw SecurityError(msg->getExceptionReason());
      } break ;

      case e_SpecifiedSaveLabelDoesNotExist: {
          D.Out(pdExcept,
		"Throwing e_SpecifiedSaveLabelDoesNotExist exception.");
          throw SpecifiedSaveLabelDoesNotExist(msg->getExceptionReason());
      } break ;

      case e_TimeAdvanceAlreadyInProgress: {
          D.Out(pdExcept, "Throwing e_TimeAdvanceAlreadyInProgress exception.");
          throw TimeAdvanceAlreadyInProgress(msg->getExceptionReason());
      } break ;

      case e_TimeAdvanceWasNotInProgress: {
          D.Out(pdExcept, "Throwing e_TimeAdvanceWasNotInProgress exception.");
          throw TimeAdvanceWasNotInProgress(msg->getExceptionReason());
      } break ;

      case e_TooManyIDsRequested: {
          D.Out(pdExcept, "Throwing e_TooManyIDsRequested exception.");
          throw TooManyIDsRequested(msg->getExceptionReason());
      } break ;

      case e_UnableToPerformSave: {
          D.Out(pdExcept, "Throwing e_UnableToPerformSave exception.");
          throw UnableToPerformSave(msg->getExceptionReason());
      } break ;

      case e_UnimplementedService: {
          D.Out(pdExcept, "Throwing e_UnimplementedService exception.");
          throw UnimplementedService(msg->getExceptionReason());
      } break ;

      case e_UnknownLabel: {
          D.Out(pdExcept, "Throwing e_UnknownLabel exception.");
          throw UnknownLabel(msg->getExceptionReason());
      } break ;

      case e_ValueCountExceeded: {
          D.Out(pdExcept, "Throwing e_ValueCountExceeded exception.");
          throw ValueCountExceeded(msg->getExceptionReason());
      } break ;

      case e_ValueLengthExceeded: {
          D.Out(pdExcept, "Throwing e_ValueLengthExceeded exception.");
          throw ValueLengthExceeded(msg->getExceptionReason());
      } break ;

      default: {
          D.Out(pdExcept, "Throwing unknown exception !");
          cout << "LibRTI: Receiving unknown exception." << endl ;
          throw RTIinternalError(msg->getExceptionReason());
      } break ;
    }
}

} // namespace certi

// $Id: RTIambassador.cc,v 3.35 2003/10/20 12:39:04 breholee Exp $
