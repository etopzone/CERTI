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
// $Id: RTIambPrivateRefs.cc,v 3.9 2007/12/11 16:44:21 rousse Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIambPrivateRefs.hh"
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
RTIambPrivateRefs::leave(const char *msg) throw (RTIinternalError)
{
	std::stringstream smsg;
	smsg << "RTI called leave because <" <<msg<<">"; 
	throw RTIinternalError(smsg.str().c_str());   
}

// ----------------------------------------------------------------------------
void
RTIambPrivateRefs::executeService(Message *req, Message *rep)
{
    G.Out(pdGendoc,"enter RTIambPrivateRefs::executeService");
    // raise exception if reentrant call.
	// FIXME EN: On SMP machine may we really 
	//           guarantee that the following protection
	//           against re-entrance is an efficient one?
    if (is_reentrant) {
        throw ConcurrentAccessAttempted("is_reentrant was true in RTIambPrivateRefs::executeService");
    }

    D.Out(pdDebug, "sending request to RTIA.");
    
    is_reentrant = true ;
    try {
        req->write(socketUn);
    }
    catch (NetworkError) {
        std::cerr << "libRTI: exception: NetworkError (write)" << std::endl ;
        throw RTIinternalError("libRTI: Network Write Error");
    }

    D.Out(pdDebug, "waiting RTIA reply.");

    // waiting RTI reply.
    try {
        rep->read(socketUn);
    }
    catch (NetworkError) {
        std::cerr << "libRTI: exception: NetworkError (read)" << std::endl ;
        throw RTIinternalError("libRTI: Network Read Error waiting RTI reply");
    }

    D.Out(pdDebug, "RTIA reply received.");

    if (rep->type != req->type) {
        std::cout << "LibRTI: Assertion failed: rep->type != req->type" << std::endl ;
        throw RTIinternalError("RTIambPrivateRefs::executeService: "
                               "rep->type != req->type");
    }

    is_reentrant = false ;
    D.Out(pdDebug, "processing returned exception (from reply).");
    processException(rep);
    D.Out(pdDebug, "exception processed.");
    G.Out(pdGendoc,"exit RTIambPrivateRefs::executeService");
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
          throw ArrayIndexOutOfBounds(msg->getExceptionReason());
      } break ;

      case e_AsynchronousDeliveryAlreadyEnabled: {
          D.Out(pdExcept, "Throwing e_AsynchronousDeliveryAlreadyEnabled exception.");
          throw AsynchronousDeliveryAlreadyEnabled(msg->getExceptionReason());
      } break ;

      case e_AsynchronousDeliveryAlreadyDisabled: {
          D.Out(pdExcept, "Throwing e_AsynchronousDeliveryAlreadyDisabled exception.");
          throw AsynchronousDeliveryAlreadyDisabled(msg->getExceptionReason());
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

      case e_CouldNotOpenFED: {
          D.Out(pdExcept, "Throwing e_CouldNotOpenFED exception.");
          throw CouldNotOpenFED(msg->getExceptionReason());
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

      case e_ErrorReadingFED: {
          D.Out(pdExcept, "Throwing e_ErrorReadingFED exception.");
          throw ErrorReadingFED(msg->getExceptionReason());
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
          std::cout << "LibRTI: Receiving unknown exception." << std::endl ;
          throw RTIinternalError(msg->getExceptionReason());
      } break ;
    }
}

// $Id: RTIambPrivateRefs.cc,v 3.9 2007/12/11 16:44:21 rousse Exp $
