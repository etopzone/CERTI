// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: RTIambPrivateRefs.cc,v 3.16.4.1 2009/01/05 13:34:52 gotthardp Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIambPrivateRefs.hh"
#include "RTItypesImp.hh"
#include "PrettyDebug.hh"
#include <sstream>

namespace {
PrettyDebug D("LIBRTI", __FILE__);
static PrettyDebug G("GENDOC",__FILE__);
}

RTIambPrivateRefs::RTIambPrivateRefs()
{
	fed_amb      = NULL;
	pid_RTIA     = (pid_t)-1;
	is_reentrant = false;
	_theRootObj  = NULL;
	socketUn     = NULL;
}

RTIambPrivateRefs::~RTIambPrivateRefs()
{
    delete socketUn ;
}

// ----------------------------------------------------------------------------
void
RTIambPrivateRefs::leave(const char *msg) throw (RTI::RTIinternalError)
{
	std::stringstream smsg;
	smsg << "RTI called leave because <" <<msg<<">"; 
    throw RTI::RTIinternalError(smsg.str().c_str());
}

// ----------------------------------------------------------------------------
void
RTIambPrivateRefs::executeService(Message *req, Message *rep)
{
    G.Out(pdGendoc,"enter RTIambPrivateRefs::executeService");

    D.Out(pdDebug, "sending request to RTIA.");
    
    try {
        req->send(socketUn,msgBufSend);
    }
    catch (NetworkError) {
        std::cerr << "libRTI: exception: NetworkError (write)" << std::endl ;
        throw RTI::RTIinternalError("libRTI: Network Write Error");
    }

    D.Out(pdDebug, "waiting RTIA reply.");

    // waiting RTI reply.
    try {
        rep->receive(socketUn,msgBufReceive);
    }
    catch (NetworkError) {
        std::cerr << "libRTI: exception: NetworkError (read)" << std::endl ;
        throw RTI::RTIinternalError("libRTI: Network Read Error waiting RTI reply");
    }

    D.Out(pdDebug, "RTIA reply received.");


    if (rep->type != req->type) {
        std::cout << "LibRTI: Assertion failed: rep->type != req->type" << std::endl ;
        throw RTI::RTIinternalError("RTIambPrivateRefs::executeService: "
                               "rep->type != req->type");
    }

    D.Out(pdDebug, "processing returned exception (from reply).");
    // the services may only throw exceptions defined in the HLA standard
    // the RTIA is responsible for sending 'allowed' exceptions only
    processException(rep);

    D.Out(pdDebug, "exception processed.");
    G.Out(pdGendoc,"exit RTIambPrivateRefs::executeService");
}

void
RTIambPrivateRefs::sendTickRequestStop()
{
    G.Out(pdGendoc,"enter RTIambPrivateRefs::sendTickRequestStop");

    Message req, rep ;
    req.type = Message::TICK_REQUEST_STOP;

    try {
        req.send(socketUn, msgBufSend);
    }
    catch (NetworkError) {
        std::cerr << "libRTI: exception: NetworkError (write)" << std::endl ;
        throw RTI::RTIinternalError("libRTI: Network Write Error");
    }

    try {
        rep.receive(socketUn, msgBufReceive);
    }
    catch (NetworkError) {
        std::cerr << "libRTI: exception: NetworkError (read)" << std::endl ;
        throw RTI::RTIinternalError("libRTI: Network Read Error waiting RTI reply");
    }

    // ignore the response, ignore exceptions
    // rep->type == Message::TICK_REQUEST;

    G.Out(pdGendoc,"exit RTIambPrivateRefs::sendTickRequestStop");
}

// ----------------------------------------------------------------------------
//! Process exception from received message.
/*! When a message is received from RTIA, it can contains an exception.
  This exception is processed by this module and a new exception is thrown.
*/
void
RTIambPrivateRefs::processException(Message *msg)
{
    D.Out(pdExcept, "num de l'exception : %d .", msg->getExceptionType());
    switch(msg->getExceptionType()) {
      case e_NO_EXCEPTION: {
      } break ;

      case e_ArrayIndexOutOfBounds: {
          D.Out(pdExcept, "Throwing e_ArrayIndexOutOfBounds exception.");
          throw RTI::ArrayIndexOutOfBounds(msg->getExceptionReason());
      } break ;

      case e_AsynchronousDeliveryAlreadyEnabled: {
          D.Out(pdExcept, "Throwing e_AsynchronousDeliveryAlreadyEnabled exception.");
          throw RTI::AsynchronousDeliveryAlreadyEnabled(msg->getExceptionReason());
      } break ;

      case e_AsynchronousDeliveryAlreadyDisabled: {
          D.Out(pdExcept, "Throwing e_AsynchronousDeliveryAlreadyDisabled exception.");
          throw RTI::AsynchronousDeliveryAlreadyDisabled(msg->getExceptionReason());
      } break ;

      case e_AttributeAlreadyOwned: {
          D.Out(pdExcept, "Throwing e_AttributeAlreadyOwned exception.");
          throw RTI::AttributeAlreadyOwned(msg->getExceptionReason());
      } break ;

      case e_AttributeAlreadyBeingAcquired: {
          D.Out(pdExcept,
		"Throwing e_AttributeAlreadyBeingAcquired exception.");
          throw RTI::AttributeAlreadyBeingAcquired(msg->getExceptionReason());
      } break ;

      case e_AttributeAlreadyBeingDivested: {
          D.Out(pdExcept,
		"Throwing e_AttributeAlreadyBeingDivested exception.");
          throw RTI::AttributeAlreadyBeingDivested(msg->getExceptionReason());
      } break ;

      case e_AttributeDivestitureWasNotRequested: {
          D.Out(pdExcept,
                "Throwing e_AttributeDivestitureWasNotRequested exception.");
          throw RTI::AttributeDivestitureWasNotRequested(msg->getExceptionReason());
      } break ;

      case e_AttributeAcquisitionWasNotRequested: {
          D.Out(pdExcept,
                "Throwing e_AttributeAcquisitionWasNotRequested exception.");
          throw RTI::AttributeAcquisitionWasNotRequested(msg->getExceptionReason());
      } break ;

      case e_AttributeNotDefined: {
          D.Out(pdExcept, "Throwing e_AttributeNotDefined exception.");
          throw RTI::AttributeNotDefined(msg->getExceptionReason());
      } break ;

      case e_AttributeNotKnown: {
          D.Out(pdExcept, "Throwing e_AttributeNotKnown exception.");
          throw RTI::AttributeNotKnown(msg->getExceptionReason());
      } break ;

      case e_AttributeNotOwned: {
          D.Out(pdExcept, "Throwing e_AttributeNotOwned exception.");
          throw RTI::AttributeNotOwned(msg->getExceptionReason());
      } break ;

      case e_AttributeNotPublished: {
          D.Out(pdExcept, "Throwing e_AttributeNotPublished exception.");
          throw RTI::AttributeNotPublished(msg->getExceptionReason());
      } break ;

      case e_AttributeNotSubscribed: {
          D.Out(pdExcept, "Throwing e_AttributeNotSubscribed exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_ConcurrentAccessAttempted: {
          D.Out(pdExcept, "Throwing e_ConcurrentAccessAttempted exception.");
          throw RTI::ConcurrentAccessAttempted(msg->getExceptionReason());
      } break ;

      case e_CouldNotDiscover: {
          D.Out(pdExcept, "Throwing e_CouldNotDiscover exception.");
          throw RTI::CouldNotDiscover(msg->getExceptionReason());
      } break ;

      case e_CouldNotOpenFED: {
          D.Out(pdExcept, "Throwing e_CouldNotOpenFED exception.");
          throw RTI::CouldNotOpenFED(msg->getExceptionReason());
      } break ;

      case e_CouldNotOpenRID: {
          D.Out(pdExcept, "Throwing e_CouldNotOpenRID exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_CouldNotRestore: {
          D.Out(pdExcept, "Throwing e_CouldNotRestore exception.");
          throw RTI::CouldNotRestore(msg->getExceptionReason());
      } break ;

      case e_DeletePrivilegeNotHeld: {
          D.Out(pdExcept, "Throwing e_DeletePrivilegeNotHeld exception.");
          throw RTI::DeletePrivilegeNotHeld(msg->getExceptionReason());
      } break ;

      case e_ErrorReadingRID: {
          D.Out(pdExcept, "Throwing e_ErrorReadingRID exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_ErrorReadingFED: {
          D.Out(pdExcept, "Throwing e_ErrorReadingFED exception.");
          throw RTI::ErrorReadingFED(msg->getExceptionReason());
      } break ;

      case e_EventNotKnown: {
          D.Out(pdExcept, "Throwing e_EventNotKnown exception.");
          throw RTI::EventNotKnown(msg->getExceptionReason());
      } break ;

      case e_FederateAlreadyPaused: {
          D.Out(pdExcept, "Throwing e_FederateAlreadyPaused exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_FederateAlreadyExecutionMember: {
          D.Out(pdExcept,
		"Throwing e_FederateAlreadyExecutionMember exception.");
          throw RTI::FederateAlreadyExecutionMember(msg->getExceptionReason());
      } break ;

      case e_FederateDoesNotExist: {
          D.Out(pdExcept, "Throwing e_FederateDoesNotExist exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_FederateInternalError: {
          D.Out(pdExcept, "Throwing e_FederateInternalError exception.");
          throw RTI::FederateInternalError(msg->getExceptionReason());
      } break ;

      case e_FederateNameAlreadyInUse: {
          D.Out(pdExcept, "Throwing e_FederateNameAlreadyInUse exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_FederateNotExecutionMember: {
          D.Out(pdExcept, "Throwing e_FederateNotExecutionMember exception.");
          throw RTI::FederateNotExecutionMember(msg->getExceptionReason());
      } break ;

      case e_FederateNotPaused: {
          D.Out(pdExcept, "Throwing e_FederateNotPaused exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_FederateOwnsAttributes: {
          D.Out(pdExcept, "Throwing e_FederateOwnsAttributes exception.");
          throw RTI::FederateOwnsAttributes(msg->getExceptionReason());
      } break ;

      case e_FederatesCurrentlyJoined: {
          D.Out(pdExcept, "Throwing e_FederatesCurrentlyJoined exception.");
          throw RTI::FederatesCurrentlyJoined(msg->getExceptionReason());
      } break ;

      case e_FederateWasNotAskedToReleaseAttribute: {
          D.Out(pdExcept,
                "Throwing e_FederateWasNotAskedToReleaseAttribute exception.");
          D.Out(pdDebug,
                "Throwing e_FederateWasNotAskedToReleaseAttribute exception.");
          throw RTI::FederateWasNotAskedToReleaseAttribute(msg->getExceptionReason());
      } break ;

      case e_FederationAlreadyPaused: {
          D.Out(pdExcept, "Throwing e_FederationAlreadyPaused exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_FederationExecutionAlreadyExists: {
          D.Out(pdExcept, "Throwing e_FederationExecutionAlreadyExists excep.");
          throw RTI::FederationExecutionAlreadyExists(msg->getExceptionReason());
      } break ;

      case e_FederationExecutionDoesNotExist: {
          D.Out(pdExcept, "Throwing e_FederationExecutionDoesNotExist except.");
          throw RTI::FederationExecutionDoesNotExist(msg->getExceptionReason());
      } break ;

      case e_FederationNotPaused: {
          D.Out(pdExcept, "Throwing e_FederationNotPaused exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_FederationTimeAlreadyPassed: {
          D.Out(pdExcept, "Throwing e_FederationTimeAlreadyPassed exception.");
          throw RTI::FederationTimeAlreadyPassed(msg->getExceptionReason());
      } break ;

      case e_FederateNotPublishing: {
          D.Out(pdExcept, "Throwing e_FederateNotPublishing exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_FederateNotSubscribing: {
          D.Out(pdExcept, "Throwing e_FederateNotSubscribing exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_RegionNotKnown: {
          D.Out(pdExcept, "Throwing e_RegionNotKnown exception.");
          throw RTI::RegionNotKnown(msg->getExceptionReason());
      } break ;

      case e_IDsupplyExhausted: {
          D.Out(pdExcept, "Throwing e_IDsupplyExhausted exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_InteractionClassNotDefined: {
          D.Out(pdExcept, "Throwing e_InteractionClassNotDefined exception.");
          throw RTI::InteractionClassNotDefined(msg->getExceptionReason());
      } break ;

      case e_InteractionClassNotKnown: {
          D.Out(pdExcept, "Throwing e_InteractionClassNotKnown exception.");
          throw RTI::InteractionClassNotKnown(msg->getExceptionReason());
      } break ;

      case e_InteractionClassNotPublished: {
          D.Out(pdExcept, "Throwing e_InteractionClassNotPublished exception.");
          throw RTI::InteractionClassNotPublished(msg->getExceptionReason());
      } break ;

      case e_InteractionParameterNotDefined: {
          D.Out(pdExcept,
		"Throwing e_InteractionParameterNotDefined exception.");
          throw RTI::InteractionParameterNotDefined(msg->getExceptionReason());
      } break ;

      case e_InteractionParameterNotKnown: {
          D.Out(pdExcept, "Throwing e_InteractionParameterNotKnown exception.");
          throw RTI::InteractionParameterNotKnown(msg->getExceptionReason());
      } break ;

      case e_InvalidDivestitureCondition: {
          D.Out(pdExcept, "Throwing e_InvalidDivestitureCondition exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_InvalidExtents: {
          D.Out(pdExcept, "Throwing e_InvalidExtents exception.");
          throw RTI::InvalidExtents(msg->getExceptionReason());
      } break ;

      case e_InvalidFederationTime: {
          D.Out(pdExcept, "Throwing e_InvalidFederationTime exception.");
          throw RTI::InvalidFederationTime(msg->getExceptionReason());
      } break ;

      case e_InvalidFederationTimeDelta: {
          D.Out(pdExcept, "Throwing e_InvalidFederationTimeDelta exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_InvalidObjectHandle: {
          D.Out(pdExcept, "Throwing e_InvalidObjectHandle exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_InvalidOrderingHandle: {
          D.Out(pdExcept, "Throwing e_InvalidOrderingHandle exception.");
          throw RTI::InvalidOrderingHandle(msg->getExceptionReason());
      } break ;

      case e_InvalidResignAction: {
          D.Out(pdExcept, "Throwing e_InvalidResignAction exception.");
          throw RTI::InvalidResignAction(msg->getExceptionReason());
      } break ;

      case e_InvalidRetractionHandle: {
          D.Out(pdExcept, "Throwing e_InvalidRetractionHandle exception.");
          throw RTI::InvalidRetractionHandle(msg->getExceptionReason());
      } break ;

      case e_InvalidRoutingSpace: {
          D.Out(pdExcept, "Throwing e_InvalidRoutingSpace exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_InvalidTransportationHandle: {
          D.Out(pdExcept, "Throwing e_InvalidTransportationHandle exception.");
          throw RTI::InvalidTransportationHandle(msg->getExceptionReason());
      } break ;

      case e_MemoryExhausted: {
          D.Out(pdExcept, "Throwing e_MemoryExhausted exception.");
          throw RTI::MemoryExhausted(msg->getExceptionReason());
      } break ;

      case e_NameNotFound: {
          D.Out(pdExcept, "Throwing e_NameNotFound exception.");
          throw RTI::NameNotFound(msg->getExceptionReason());
      } break ;

      case e_NoPauseRequested: {
          D.Out(pdExcept, "Throwing e_NoPauseRequested exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_NoResumeRequested: {
          D.Out(pdExcept, "Throwing e_NoResumeRequested exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_ObjectClassNotDefined: {
          D.Out(pdExcept, "Throwing e_ObjectClassNotDefined exception.");
          throw RTI::ObjectClassNotDefined(msg->getExceptionReason());
      } break ;

      case e_ObjectClassNotKnown: {
          D.Out(pdExcept, "Throwing e_ObjectClassNotKnown exception.");
          throw RTI::ObjectClassNotKnown(msg->getExceptionReason());
      } break ;

      case e_ObjectClassNotPublished: {
          D.Out(pdExcept, "Throwing e_ObjectClassNotPublished exception.");
          throw RTI::ObjectClassNotPublished(msg->getExceptionReason());
      } break ;

      case e_ObjectClassNotSubscribed: {
          D.Out(pdExcept, "Throwing e_ObjectClassNotSubscribed exception.");
          throw RTI::ObjectClassNotSubscribed(msg->getExceptionReason());
      } break ;

      case e_ObjectNotKnown: {
          D.Out(pdExcept, "Throwing e_ObjectNotKnown exception.");
          throw RTI::ObjectNotKnown(msg->getExceptionReason());
      } break ;

      case e_ObjectAlreadyRegistered: {
          D.Out(pdExcept, "Throwing e_ObjectAlreadyRegistered exception.");
          throw RTI::ObjectAlreadyRegistered(msg->getExceptionReason());
      } break ;

      case e_RestoreInProgress: {
          D.Out(pdExcept, "Throwing e_RestoreInProgress exception.");
          throw RTI::RestoreInProgress(msg->getExceptionReason());
      } break ;

      case e_RestoreNotRequested: {
          D.Out(pdExcept, "Throwing e_RestoreNotRequested exception.");
          throw RTI::RestoreNotRequested(msg->getExceptionReason());
      } break ;

      case e_RTIinternalError: {
          D.Out(pdExcept, "Throwing e_RTIinternalError exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_SpaceNotDefined: {
          D.Out(pdExcept, "Throwing e_SpaceNotDefined exception.");
          throw RTI::SpaceNotDefined(msg->getExceptionReason());
      } break ;

      case e_SaveInProgress: {
          D.Out(pdExcept, "Throwing e_SaveInProgress exception.");
          throw RTI::SaveInProgress(msg->getExceptionReason());
      } break ;

      case e_SaveNotInitiated: {
          D.Out(pdExcept, "Throwing e_SaveNotInitiated exception.");
          throw RTI::SaveNotInitiated(msg->getExceptionReason());
      } break ;

      case e_SecurityError: {
          D.Out(pdExcept, "Throwing e_SecurityError exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_SpecifiedSaveLabelDoesNotExist: {
          D.Out(pdExcept,
		"Throwing e_SpecifiedSaveLabelDoesNotExist exception.");
          throw RTI::SpecifiedSaveLabelDoesNotExist(msg->getExceptionReason());
      } break ;

      case e_TimeAdvanceAlreadyInProgress: {
          D.Out(pdExcept, "Throwing e_TimeAdvanceAlreadyInProgress exception.");
          throw RTI::TimeAdvanceAlreadyInProgress(msg->getExceptionReason());
      } break ;

      case e_TimeAdvanceWasNotInProgress: {
          D.Out(pdExcept, "Throwing e_TimeAdvanceWasNotInProgress exception.");
          throw RTI::TimeAdvanceWasNotInProgress(msg->getExceptionReason());
      } break ;

      case e_TooManyIDsRequested: {
          D.Out(pdExcept, "Throwing e_TooManyIDsRequested exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_UnableToPerformSave: {
          D.Out(pdExcept, "Throwing e_UnableToPerformSave exception.");
          throw RTI::UnableToPerformSave(msg->getExceptionReason());
      } break ;

      case e_UnimplementedService: {
          D.Out(pdExcept, "Throwing e_UnimplementedService exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_UnknownLabel: {
          D.Out(pdExcept, "Throwing e_UnknownLabel exception.");
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_ValueCountExceeded: {
          D.Out(pdExcept, "Throwing e_ValueCountExceeded exception.");
          throw RTI::ValueCountExceeded(msg->getExceptionReason());
      } break ;

      case e_ValueLengthExceeded: {
          D.Out(pdExcept, "Throwing e_ValueLengthExceeded exception.");
          throw RTI::ValueLengthExceeded(msg->getExceptionReason());
      } break ;

      default: {
          D.Out(pdExcept, "Throwing unknown exception !");
          std::cout << "LibRTI: Receiving unknown RTI exception." << std::endl;
          throw RTI::RTIinternalError(msg->getExceptionReason());
      } break ;
    }
}

#define CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(service) \
    catch (Exception &e) { \
        std::stringstream msg; \
        msg << "Error in " << service << ": " << e._name << "[" << e._reason << "]"; \
        throw RTI::RTIinternalError(msg.str().c_str()); \
    }

void
RTIambPrivateRefs::callFederateAmbassador(Message *msg)
    throw (RTI::RTIinternalError)
{
    switch (msg->type) {

      case Message::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
        try {
            fed_amb->synchronizationPointRegistrationSucceeded((msg->getLabel()).c_str());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("synchronizationPointRegistrationSucceeded")
        break ;

      case Message::ANNOUNCE_SYNCHRONIZATION_POINT:
        try {
            fed_amb->announceSynchronizationPoint((msg->getLabel()).c_str(),(msg->getTag()).c_str());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("announceSynchronizationPoint")
        break ;

      case Message::FEDERATION_SYNCHRONIZED:
        try {
            fed_amb->federationSynchronized((msg->getLabel()).c_str());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("federationSynchronized")
        break ;

      case Message::INITIATE_FEDERATE_SAVE:
        try {
            fed_amb->initiateFederateSave((msg->getLabel()).c_str());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("initiateFederateSave")
        break ;

      case Message::FEDERATION_SAVED:
        try {
            fed_amb->federationSaved();
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("federationSaved")
        break ;

      case Message::REQUEST_FEDERATION_RESTORE_SUCCEEDED:
        try {
            fed_amb->requestFederationRestoreSucceeded(
                (msg->getLabel()).c_str());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("requestFederationRestoreSucceeded")
        break ;

      case Message::REQUEST_FEDERATION_RESTORE_FAILED:
        try {
            fed_amb->requestFederationRestoreFailed((msg->getLabel()).c_str(),
                                                (msg->getTag()).c_str());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("requestFederationRestoreFailed")
        break ;

      case Message::FEDERATION_RESTORE_BEGUN:
        try {
            fed_amb->federationRestoreBegun();
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("federationRestoreBegun")
        break ;

      case Message::INITIATE_FEDERATE_RESTORE:
        try {
            fed_amb->initiateFederateRestore((msg->getLabel()).c_str(),
                                         msg->getFederate());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("initiateFederateRestore")
        break ;

      case Message::FEDERATION_RESTORED:
        try {
            fed_amb->federationRestored();
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("federationRestored")
        break ;

      case Message::FEDERATION_NOT_RESTORED:
        try {
            fed_amb->federationNotRestored();
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("federationNotRestored")
        break ;

      case Message::START_REGISTRATION_FOR_OBJECT_CLASS:
        try {
            fed_amb->startRegistrationForObjectClass(
                msg->getObjectClass());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("startRegistrationForObjectClass")
        break ;

      case Message::STOP_REGISTRATION_FOR_OBJECT_CLASS:
        try {
            fed_amb->stopRegistrationForObjectClass(msg->getObjectClass());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("stopRegistrationForObjectClass")
        break ;

      case Message::TURN_INTERACTIONS_ON:
        try {
            fed_amb->turnInteractionsOn(msg->getInteractionClass());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("turnInteractionsOn")
        break ;

      case Message::TURN_INTERACTIONS_OFF:
        try {
            fed_amb->turnInteractionsOff(msg->getInteractionClass());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("turnInteractionsOff")
        break ;

      case Message::DISCOVER_OBJECT_INSTANCE:
        try {
            fed_amb->discoverObjectInstance(msg->getObject(),
                                            msg->getObjectClass(),
                                            msg->getName().c_str());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("discoverObjectInstance")
        break ;

      case Message::REFLECT_ATTRIBUTE_VALUES:
        try {
            G.Out(pdGendoc,"          tick_kernel call to reflectAttributeValues");
            RTI::AttributeHandleValuePairSet *attributes =
                new AttributeHandleValuePairSetImp(msg->getAHVPS());

            if (msg->getBoolean())
               fed_amb->reflectAttributeValues(msg->getObject(),
                                               *attributes,
                                               RTIfedTime(msg->getFedTime()),
                                               (msg->getTag()).c_str(),
                                               msg->getEventRetraction());
            else
               fed_amb->reflectAttributeValues(msg->getObject(),
                                               *attributes,
                                               (msg->getTag()).c_str());
            delete attributes ;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("reflectAttributeValues")
        break ;

      case Message::RECEIVE_INTERACTION:
        try {
            RTI::ParameterHandleValuePairSet *parameters =
                new ParameterHandleValuePairSetImp(msg->getPHVPS());

            if (msg->getBoolean())
                fed_amb->receiveInteraction(
                                        msg->getInteractionClass(),
                                        *parameters,
                                        RTIfedTime(msg->getFedTime()),
                                        (msg->getTag()).c_str(),
                                        msg->getEventRetraction());
            else
                fed_amb->receiveInteraction(
                                        msg->getInteractionClass(),
                                        *parameters,
                                        (msg->getTag()).c_str());

            delete parameters ;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("receiveInteraction")
        break ;

      case Message::REMOVE_OBJECT_INSTANCE:
        try {
            if (msg->getBoolean()) {
                fed_amb->removeObjectInstance(
                                          msg->getObject(),
                                          RTIfedTime(msg->getFedTime()),
                                          (msg->getTag()).c_str(),
                                          msg->getEventRetraction());
            }
            else {
                fed_amb->removeObjectInstance(
                                          msg->getObject(),
                                          (msg->getTag()).c_str());
            }
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("removeObjectInstance")
        break ;

      case Message::PROVIDE_ATTRIBUTE_VALUE_UPDATE:
        try {
            RTI::AttributeHandleSet *attributeSet =
                new AttributeHandleSetImp(msg->getAHS());

            fed_amb->provideAttributeValueUpdate(msg->getObject(),*attributeSet);
            delete attributeSet ;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("provideAttributeValueUpdate")
        break ;

      case Message::REQUEST_RETRACTION: {

      } break ;

      case Message::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
        try {
            RTI::AttributeHandleSet *attributeSet =
                new AttributeHandleSetImp(msg->getAHS());

            fed_amb->
                requestAttributeOwnershipAssumption(msg->getObject(),
                                                    *attributeSet,
                                                    (msg->getTag()).c_str());
            delete attributeSet ;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("requestAttributeOwnershipAssumption")
        break ;

      case Message::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
        try {
            RTI::AttributeHandleSet *attributeSet =
                new AttributeHandleSetImp(msg->getAHS());

            fed_amb->requestAttributeOwnershipRelease(
                msg->getObject(),
                *attributeSet,
                (msg->getTag()).c_str());

            delete attributeSet ;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("requestAttributeOwnershipRelease")
        break ;

      case Message::ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
        try {
            RTI::AttributeHandleSet *attributeSet =
                new AttributeHandleSetImp(msg->getAHS());

            fed_amb->attributeOwnershipUnavailable(msg->getObject(),
                                                       *attributeSet);

            delete attributeSet ;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("attributeOwnershipUnavailable")
        break ;

      case Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
        try {
            RTI::AttributeHandleSet *attributeSet =
                new AttributeHandleSetImp(msg->getAHS());

            fed_amb->attributeOwnershipAcquisitionNotification(
                msg->getObject(),
                *attributeSet);

            delete attributeSet ;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("attributeOwnershipAcquisitionNotification")
        break ;

      case Message::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
        try {
            RTI::AttributeHandleSet *attributeSet =
                new AttributeHandleSetImp(msg->getAHS());

            fed_amb->attributeOwnershipDivestitureNotification(
                msg->getObject(),
                *attributeSet);

            delete attributeSet ;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("attributeOwnershipDivestitureNotification")
        break ;

      case Message::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
        try {
            RTI::AttributeHandleSet *attributeSet =
                new AttributeHandleSetImp(msg->getAHS());

            fed_amb->confirmAttributeOwnershipAcquisitionCancellation(
                msg->getObject(),
                *attributeSet);

            delete attributeSet ;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("confirmAttributeOwnershipAcquisitionCancellation")
        break ;

      case Message::INFORM_ATTRIBUTE_OWNERSHIP: 
        try {
            fed_amb->
                informAttributeOwnership(msg->getObject(),
                                         msg->getAttribute(),
                                         msg->getFederate());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("informAttributeOwnership")
        break ;

      case Message::ATTRIBUTE_IS_NOT_OWNED:
        try {
            fed_amb->attributeIsNotOwned(msg->getObject(),
                                                      msg->getAttribute());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("attributeIsNotOwned")
        break ;

      case Message::TIME_ADVANCE_GRANT:
        try {
            fed_amb->timeAdvanceGrant(RTIfedTime(msg->getFedTime()));
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("timeAdvanceGrant")
        break ;

      case Message::TIME_REGULATION_ENABLED:
        try {
            fed_amb->timeRegulationEnabled(RTIfedTime(msg->getFedTime()));
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("timeRegulationEnabled")
        break ;

      case Message::TIME_CONSTRAINED_ENABLED:
        try {
            fed_amb->timeConstrainedEnabled(RTIfedTime(msg->getFedTime()));
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("timeConstrainedEnabled")
        break ;

      default:
        leave("RTI service requested by RTI is unknown.");
    }
}

// $Id: RTIambPrivateRefs.cc,v 3.16.4.1 2009/01/05 13:34:52 gotthardp Exp $
