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
// $Id: RTIambPrivateRefs.cpp,v 1.2 2014/03/07 18:00:49 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIambPrivateRefs.h"
#include "HandleImplementation.h"

#include <RTI/Exception.h>
#include <RTI/certiLogicalTime.h>
#include <RTI/certiLogicalTimeInterval.h>
#include <RTI/certiLogicalTimeFactory.h>
#include "RTIHandleFactory.h"
#include "RTI1516fedTime.h"

#include "PrettyDebug.hh"
#include "M_Classes.hh"
#include <sstream>
#include <iostream>

namespace {
static PrettyDebug D("LIBRTI", __FILE__);
static PrettyDebug G("GENDOC",__FILE__);

template<typename T>
std::vector<std::pair<AttributeHandle, AttributeValue_t> >
getAHVPSFromRequest(T* request)
{
	uint32_t size = request->getAttributesSize();
	std::vector<std::pair<AttributeHandle, AttributeValue_t> > result;
	result.resize(size);

	for (uint32_t i = 0 ; i < size ; ++i) {
		result[i].first = request->getAttributes(i);
		result[i].second.assign(request->getValues(i).data(), request->getValues(i).length());
	}

	return result;
}

template<typename T>
std::vector<std::pair<rti1516e::ParameterHandle, ParameterValue_t> >
getPHVPSFromRequest(T* request)
{
	uint32_t size = request->getParametersSize();

	std::vector<std::pair<rti1516e::ParameterHandle, ParameterValue_t> > result;
	result.resize(size);

	for (uint32_t i = 0 ; i < size ; ++i) {
		result[i].first = request->getParameters(i);
		result[i].second.assign(request->getValues(i).data(), request->getValues(i).length());
	}

	return result;
}

template<typename T>
rti1516e::AttributeHandleValueMap *
getAHVMFromRequest(T* request)
{
	uint32_t size = request->getAttributesSize();
	rti1516e::AttributeHandleValueMap *result = new rti1516e::AttributeHandleValueMap();
	
	for (uint32_t i = 0 ; i < size ; ++i) {
		rti1516e::AttributeHandle attribute = rti1516e::AttributeHandleFriend::createRTI1516Handle(request->getAttributes(i));
		rti1516e::VariableLengthData varData(&(request->getValues(i)[0]), request->getValues(i).size());
		result->insert(std::pair<rti1516e::AttributeHandle,rti1516e::VariableLengthData>(attribute, varData));
	}

	return result;
}

template<typename T>
rti1516e::ParameterHandleValueMap *
getPHVMFromRequest(T* request)
{
	uint32_t size = request->getParametersSize();
	rti1516e::ParameterHandleValueMap *result = new rti1516e::ParameterHandleValueMap();

	for (uint32_t i = 0 ; i < size ; ++i) {
		rti1516e::ParameterHandle parameter = rti1516e::ParameterHandleFriend::createRTI1516Handle(request->getParameters(i));
		rti1516e::VariableLengthData varData(&(request->getValues(i)[0]), request->getValues(i).size());
		result->insert(std::pair<rti1516e::ParameterHandle,rti1516e::VariableLengthData>(parameter, varData));
	}

	return result;
}

template<typename T>
rti1516e::AttributeHandleSet *
getAHSFromRequest(T* request)
{
	uint32_t size = request->getAttributesSize();
	rti1516e::AttributeHandleSet *result = new rti1516e::AttributeHandleSet();
	
	for (uint32_t i = 0 ; i < size ; ++i) {
		rti1516e::AttributeHandle attribute = rti1516e::AttributeHandleFriend::createRTI1516Handle(request->getAttributes(i));
		result->insert(attribute);
	}

	return result;
}

std::auto_ptr< rti1516e::LogicalTime >
getLogicalTime()
throw ( rti1516e::RTIinternalError )
{
	std::auto_ptr< rti1516e::LogicalTimeFactory > timeFactory = rti1516e::LogicalTimeFactoryFactory::makeLogicalTimeFactory(L"");
	if (timeFactory.get() != 0)
	{
		return timeFactory->makeLogicalTime();
	} else
	{
		throw rti1516e::RTIinternalError(L"Could not create LogicalTimeFactory");
	}
}

} // End anonymous namespace

RTI1516ambPrivateRefs::RTI1516ambPrivateRefs()
{
	fed_amb      = NULL;
#ifdef _WIN32
	handle_RTIA  = (HANDLE)-1;
#else
	pid_RTIA     = (pid_t)-1;
#endif
	is_reentrant = false;
	_theRootObj  = NULL;
	socketUn     = NULL;
}

RTI1516ambPrivateRefs::~RTI1516ambPrivateRefs()
{
	delete socketUn ;
}

// ----------------------------------------------------------------------------
void
RTI1516ambPrivateRefs::leave(const char *msg) throw (rti1516e::RTIinternalError)
{
	std::wstringstream smsg;
	smsg << "RTI called leave because <" <<msg<<">";
	throw rti1516e::RTIinternalError(smsg.str());
}

// ----------------------------------------------------------------------------
void
RTI1516ambPrivateRefs::executeService(Message *req, Message *rep)
{
	G.Out(pdGendoc,"enter RTI1516ambPrivateRefs::executeService(%s,%s)",req->getMessageName(),rep->getMessageName());

	D.Out(pdDebug, "sending request to RTIA.");

	try {
		req->send(socketUn,msgBufSend);
	}
	catch (certi::NetworkError) {
		std::cerr << "libRTI: exception: NetworkError (write)" << std::endl ;
		if ( req->getMessageType() == certi::Message::CLOSE_CONNEXION)
		{
			std::cerr << "libRTI: Could not execute 'Close connexion' service (Network error). Service request ignored." << std::endl;
			return;
		} else
		{
			throw rti1516e::RTIinternalError(L"libRTI: Network Write Error");
		}
	}

	D.Out(pdDebug, "waiting RTIA reply.");

	// waiting RTI reply.
	try {
		rep->receive(socketUn,msgBufReceive);
	}
	catch (certi::NetworkError) {
		std::cerr << "libRTI: exception: NetworkError (read)" << std::endl ;
		throw rti1516e::RTIinternalError(L"libRTI: Network Read Error waiting RTI reply");
	}

	D.Out(pdDebug, "RTIA reply received.");


	if (rep->getMessageType() != req->getMessageType()) {
		std::cout << "LibRTI: Assertion failed: rep->type != req->type" << std::endl ;
		throw rti1516e::RTIinternalError(L"RTI1516ambPrivateRefs::executeService: "
				L"rep->type != req->type");
	}

	D.Out(pdDebug, "processing returned exception (from reply).");
	// the services may only throw exceptions defined in the HLA standard
	// the RTIA is responsible for sending 'allowed' exceptions only
	processException(rep);

	D.Out(pdDebug, "exception processed.");
	G.Out(pdGendoc,"exit RTI1516ambPrivateRefs::executeService");
}

void
RTI1516ambPrivateRefs::sendTickRequestStop()
{
	G.Out(pdGendoc,"enter RTI1516ambPrivateRefs::sendTickRequestStop");

	M_Tick_Request_Stop req, rep ;

	try {
		req.send(socketUn, msgBufSend);
	}
	catch (certi::NetworkError) {
		std::cerr << "libRTI: exception: NetworkError (write)" << std::endl ;
		throw rti1516e::RTIinternalError(L"libRTI: Network Write Error");
	}

	try {
		rep.receive(socketUn, msgBufReceive);
	}
	catch (certi::NetworkError) {
		std::cerr << "libRTI: exception: NetworkError (read)" << std::endl ;
		throw rti1516e::RTIinternalError(L"libRTI: Network Read Error waiting RTI reply");
	}

	// ignore the response, ignore exceptions
	// rep->type == Message::TICK_REQUEST;

	G.Out(pdGendoc,"exit RTI1516ambPrivateRefs::sendTickRequestStop");
}

// ----------------------------------------------------------------------------
//! Process exception from received message.
/*! When a message is received from RTIA, it can contains an exception.
  This exception is processed by this module and a new exception is thrown.
 */
void
RTI1516ambPrivateRefs::processException(Message *msg)
{
	D.Out(pdExcept, "num de l'exception : %d .", msg->getExceptionType());
	switch(msg->getExceptionType()) {
	case e_NO_EXCEPTION: {
	} break ;

	case e_ArrayIndexOutOfBounds: {
		D.Out(pdExcept, "Throwing e_ArrayIndexOutOfBounds exception.");
		// JvY: Changed name of exception
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_AsynchronousDeliveryAlreadyEnabled: {
		D.Out(pdExcept, "Throwing e_AsynchronousDeliveryAlreadyEnabled exception.");
		throw rti1516e::AsynchronousDeliveryAlreadyEnabled(msg->getExceptionReasonW());
	} break ;

	case e_AsynchronousDeliveryAlreadyDisabled: {
		D.Out(pdExcept, "Throwing e_AsynchronousDeliveryAlreadyDisabled exception.");
		throw rti1516e::AsynchronousDeliveryAlreadyDisabled(msg->getExceptionReasonW());
	} break ;

	case e_AttributeAlreadyOwned: {
		D.Out(pdExcept, "Throwing e_AttributeAlreadyOwned exception.");
		throw rti1516e::AttributeAlreadyOwned(msg->getExceptionReasonW());
	} break ;

	case e_AttributeAlreadyBeingAcquired: {
		D.Out(pdExcept,
				"Throwing e_AttributeAlreadyBeingAcquired exception.");
		throw rti1516e::AttributeAlreadyBeingAcquired(msg->getExceptionReasonW());
	} break ;

	case e_AttributeAlreadyBeingDivested: {
		D.Out(pdExcept,
				"Throwing e_AttributeAlreadyBeingDivested exception.");
		throw rti1516e::AttributeAlreadyBeingDivested(msg->getExceptionReasonW());
	} break ;

	case e_AttributeDivestitureWasNotRequested: {
		D.Out(pdExcept,
				"Throwing e_AttributeDivestitureWasNotRequested exception.");
		throw rti1516e::AttributeDivestitureWasNotRequested(msg->getExceptionReasonW());
	} break ;

	case e_AttributeAcquisitionWasNotRequested: {
		D.Out(pdExcept,
				"Throwing e_AttributeAcquisitionWasNotRequested exception.");
		throw rti1516e::AttributeAcquisitionWasNotRequested(msg->getExceptionReasonW());
	} break ;

	case e_AttributeNotDefined: {
		D.Out(pdExcept, "Throwing e_AttributeNotDefined exception.");
		throw rti1516e::AttributeNotDefined(msg->getExceptionReasonW());
	} break ;

	case e_AttributeNotKnown: {
		D.Out(pdExcept, "Throwing e_AttributeNotKnown exception.");
		// JvY: Changed name of exception
		throw rti1516e::AttributeNotRecognized(msg->getExceptionReasonW());
	} break ;

	case e_AttributeNotOwned: {
		D.Out(pdExcept, "Throwing e_AttributeNotOwned exception.");
		throw rti1516e::AttributeNotOwned(msg->getExceptionReasonW());
	} break ;

	case e_AttributeNotPublished: {
		D.Out(pdExcept, "Throwing e_AttributeNotPublished exception.");
		throw rti1516e::AttributeNotPublished(msg->getExceptionReasonW());
	} break ;

	case e_AttributeNotSubscribed: {
		D.Out(pdExcept, "Throwing e_AttributeNotSubscribed exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_ConcurrentAccessAttempted: {
		D.Out(pdExcept, "Throwing e_ConcurrentAccessAttempted exception.");
		// JvY: Changed name of exception
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_CouldNotDiscover: {
		D.Out(pdExcept, "Throwing e_CouldNotDiscover exception.");
		throw rti1516e::CouldNotDiscover(msg->getExceptionReasonW());
	} break ;

	case e_CouldNotOpenFED: {
		D.Out(pdExcept, "Throwing e_CouldNotOpenFED exception.");
		// JvY: Changed name of exception
		throw rti1516e::CouldNotOpenFDD(msg->getExceptionReasonW());
	} break ;

	case e_CouldNotOpenRID: {
		D.Out(pdExcept, "Throwing e_CouldNotOpenRID exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_CouldNotRestore: {
		D.Out(pdExcept, "Throwing e_CouldNotRestore exception.");
		// JvY: Changed name of exception
		throw rti1516e::CouldNotInitiateRestore(msg->getExceptionReasonW());
	} break ;

	case e_DeletePrivilegeNotHeld: {
		D.Out(pdExcept, "Throwing e_DeletePrivilegeNotHeld exception.");
		throw rti1516e::DeletePrivilegeNotHeld(msg->getExceptionReasonW());
	} break ;

	case e_ErrorReadingRID: {
		D.Out(pdExcept, "Throwing e_ErrorReadingRID exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_ErrorReadingFED: {
		D.Out(pdExcept, "Throwing e_ErrorReadingFED exception.");
		// JvY: Changed name of exception
		throw rti1516e::ErrorReadingFDD(msg->getExceptionReasonW());
	} break ;

	case e_EventNotKnown: {
		D.Out(pdExcept, "Throwing e_EventNotKnown exception.");
		// JvY: Changed name of exception
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_FederateAlreadyPaused: {
		D.Out(pdExcept, "Throwing e_FederateAlreadyPaused exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_FederateAlreadyExecutionMember: {
		D.Out(pdExcept,
				"Throwing e_FederateAlreadyExecutionMember exception.");
		throw rti1516e::FederateAlreadyExecutionMember(msg->getExceptionReasonW());
	} break ;

	case e_FederateDoesNotExist: {
		D.Out(pdExcept, "Throwing e_FederateDoesNotExist exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_FederateInternalError: {
		D.Out(pdExcept, "Throwing e_FederateInternalError exception.");
		throw rti1516e::FederateInternalError(msg->getExceptionReasonW());
	} break ;

	case e_FederateNameAlreadyInUse: {
		D.Out(pdExcept, "Throwing e_FederateNameAlreadyInUse exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_FederateNotExecutionMember: {
		D.Out(pdExcept, "Throwing e_FederateNotExecutionMember exception.");
		throw rti1516e::FederateNotExecutionMember(msg->getExceptionReasonW());
	} break ;

	case e_FederateNotPaused: {
		D.Out(pdExcept, "Throwing e_FederateNotPaused exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_FederateOwnsAttributes: {
		D.Out(pdExcept, "Throwing e_FederateOwnsAttributes exception.");
		throw rti1516e::FederateOwnsAttributes(msg->getExceptionReasonW());
	} break ;

	case e_FederatesCurrentlyJoined: {
		D.Out(pdExcept, "Throwing e_FederatesCurrentlyJoined exception.");
		throw rti1516e::FederatesCurrentlyJoined(msg->getExceptionReasonW());
	} break ;

	case e_FederateWasNotAskedToReleaseAttribute: {
		D.Out(pdExcept,
				"Throwing e_FederateWasNotAskedToReleaseAttribute exception.");
		D.Out(pdDebug,
				"Throwing e_FederateWasNotAskedToReleaseAttribute exception.");
		// JvY: Changed name of exception
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_FederationAlreadyPaused: {
		D.Out(pdExcept, "Throwing e_FederationAlreadyPaused exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_FederationExecutionAlreadyExists: {
		D.Out(pdExcept, "Throwing e_FederationExecutionAlreadyExists excep.");
		throw rti1516e::FederationExecutionAlreadyExists(msg->getExceptionReasonW());
	} break ;

	case e_FederationExecutionDoesNotExist: {
		D.Out(pdExcept, "Throwing e_FederationExecutionDoesNotExist except.");
		throw rti1516e::FederationExecutionDoesNotExist(msg->getExceptionReasonW());
	} break ;

	case e_FederationNotPaused: {
		D.Out(pdExcept, "Throwing e_FederationNotPaused exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_FederationTimeAlreadyPassed: {
		D.Out(pdExcept, "Throwing e_FederationTimeAlreadyPassed exception.");
		// JvY: Changed name of exception
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_FederateNotPublishing: {
		D.Out(pdExcept, "Throwing e_FederateNotPublishing exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_FederateNotSubscribing: {
		D.Out(pdExcept, "Throwing e_FederateNotSubscribing exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_RegionNotKnown: {
		D.Out(pdExcept, "Throwing e_RegionNotKnown exception.");
		// JvY: Changed name of exception
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_IDsupplyExhausted: {
		D.Out(pdExcept, "Throwing e_IDsupplyExhausted exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_InteractionClassNotDefined: {
		D.Out(pdExcept, "Throwing e_InteractionClassNotDefined exception.");
		throw rti1516e::InteractionClassNotDefined(msg->getExceptionReasonW());
	} break ;

	case e_InteractionClassNotKnown: {
		D.Out(pdExcept, "Throwing e_InteractionClassNotKnown exception.");
		// JvY: Changed name of exception
		throw rti1516e::InteractionClassNotRecognized(msg->getExceptionReasonW());
	} break ;

	case e_InteractionClassNotPublished: {
		D.Out(pdExcept, "Throwing e_InteractionClassNotPublished exception.");
		throw rti1516e::InteractionClassNotPublished(msg->getExceptionReasonW());
	} break ;

	case e_InteractionParameterNotDefined: {
		D.Out(pdExcept,
				"Throwing e_InteractionParameterNotDefined exception.");
		throw rti1516e::InteractionParameterNotDefined(msg->getExceptionReasonW());
	} break ;

	case e_InteractionParameterNotKnown: {
		D.Out(pdExcept, "Throwing e_InteractionParameterNotKnown exception.");
		// JvY: Changed name of exception
		throw rti1516e::InteractionParameterNotRecognized(msg->getExceptionReasonW());
	} break ;

	case e_InvalidDivestitureCondition: {
		D.Out(pdExcept, "Throwing e_InvalidDivestitureCondition exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_InvalidExtents: {
		D.Out(pdExcept, "Throwing e_InvalidExtents exception.");
		// JvY: Changed name of exception
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_InvalidFederationTime: {
		D.Out(pdExcept, "Throwing e_InvalidFederationTime exception.");
		// JvY: Changed name of exception
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_InvalidFederationTimeDelta: {
		D.Out(pdExcept, "Throwing e_InvalidFederationTimeDelta exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_InvalidLookahead: {
		D.Out(pdExcept, "Throwing e_InvalidLookahead.");
		throw rti1516e::InvalidLookahead(msg->getExceptionReasonW());
	} break ;

	case e_InvalidObjectHandle: {
		D.Out(pdExcept, "Throwing e_InvalidObjectHandle exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_InvalidOrderingHandle: {
		D.Out(pdExcept, "Throwing e_InvalidOrderingHandle exception.");
		// JvY: Changed name of exception
		throw rti1516e::InvalidOrderType(msg->getExceptionReasonW());
	} break ;

	case e_InvalidResignAction: {
		D.Out(pdExcept, "Throwing e_InvalidResignAction exception.");
		// JvY: Changed name of exception
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_InvalidRetractionHandle: {
		D.Out(pdExcept, "Throwing e_InvalidRetractionHandle exception.");
		throw rti1516e::InvalidMessageRetractionHandle(msg->getExceptionReasonW());
	} break ;

	case e_InvalidRoutingSpace: {
		D.Out(pdExcept, "Throwing e_InvalidRoutingSpace exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_InvalidTransportationHandle: {
		D.Out(pdExcept, "Throwing e_InvalidTransportationHandle exception.");
		// JvY: Changed name of exception
		throw rti1516e::InvalidTransportationType(msg->getExceptionReasonW());
	} break ;

	case e_MemoryExhausted: {
		D.Out(pdExcept, "Throwing e_MemoryExhausted exception.");
		// JvY: Changed name of exception
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_NameNotFound: {
		D.Out(pdExcept, "Throwing e_NameNotFound exception.");
		throw rti1516e::NameNotFound(msg->getExceptionReasonW());
	} break ;

	case e_NoPauseRequested: {
		D.Out(pdExcept, "Throwing e_NoPauseRequested exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_NoResumeRequested: {
		D.Out(pdExcept, "Throwing e_NoResumeRequested exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_ObjectClassNotDefined: {
		D.Out(pdExcept, "Throwing e_ObjectClassNotDefined exception.");
		throw rti1516e::ObjectClassNotDefined(msg->getExceptionReasonW());
	} break ;

	case e_ObjectClassNotKnown: {
		D.Out(pdExcept, "Throwing e_ObjectClassNotKnown exception.");
		throw rti1516e::ObjectClassNotKnown(msg->getExceptionReasonW());
	} break ;

	case e_ObjectClassNotPublished: {
		D.Out(pdExcept, "Throwing e_ObjectClassNotPublished exception.");
		throw rti1516e::ObjectClassNotPublished(msg->getExceptionReasonW());
	} break ;

	case e_ObjectClassNotSubscribed: {
		D.Out(pdExcept, "Throwing e_ObjectClassNotSubscribed exception.");
		// JvY: Changed name of exception
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_ObjectNotKnown: {
		D.Out(pdExcept, "Throwing e_ObjectNotKnown exception.");
		// JvY: Changed name of exception
		throw rti1516e::ObjectInstanceNotKnown(msg->getExceptionReasonW());
	} break ;

	case e_ObjectAlreadyRegistered: {
		D.Out(pdExcept, "Throwing e_ObjectAlreadyRegistered exception.");
		// JvY: Changed name of exception
		throw rti1516e::ObjectInstanceNameInUse(msg->getExceptionReasonW());
	} break ;

	case e_RestoreInProgress: {
		D.Out(pdExcept, "Throwing e_RestoreInProgress exception.");
		throw rti1516e::RestoreInProgress(msg->getExceptionReasonW());
	} break ;

	case e_RestoreNotRequested: {
		D.Out(pdExcept, "Throwing e_RestoreNotRequested exception.");
		throw rti1516e::RestoreNotRequested(msg->getExceptionReasonW());
	} break ;

	case e_RTIinternalError: {
		D.Out(pdExcept, "Throwing e_RTIinternalError exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_SpaceNotDefined: {
		D.Out(pdExcept, "Throwing e_SpaceNotDefined exception.");
		// JvY: Changed name of exception
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_SaveInProgress: {
		D.Out(pdExcept, "Throwing e_SaveInProgress exception.");
		throw rti1516e::SaveInProgress(msg->getExceptionReasonW());
	} break ;

	case e_SaveNotInitiated: {
		D.Out(pdExcept, "Throwing e_SaveNotInitiated exception.");
		throw rti1516e::SaveNotInitiated(msg->getExceptionReasonW());
	} break ;

	case e_SecurityError: {
		D.Out(pdExcept, "Throwing e_SecurityError exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_SpecifiedSaveLabelDoesNotExist: {
		D.Out(pdExcept,
				"Throwing e_SpecifiedSaveLabelDoesNotExist exception.");
		throw rti1516e::SpecifiedSaveLabelDoesNotExist(msg->getExceptionReasonW());
	} break ;

	case e_TimeAdvanceAlreadyInProgress: {
		D.Out(pdExcept, "Throwing e_TimeAdvanceAlreadyInProgress exception.");
		// JvY: Changed name of exception
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_TimeAdvanceWasNotInProgress: {
		D.Out(pdExcept, "Throwing e_TimeAdvanceWasNotInProgress exception.");
		// JvY: Changed name of exception
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_TooManyIDsRequested: {
		D.Out(pdExcept, "Throwing e_TooManyIDsRequested exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_UnableToPerformSave: {
		D.Out(pdExcept, "Throwing e_UnableToPerformSave exception.");
		throw rti1516e::UnableToPerformSave(msg->getExceptionReasonW());
	} break ;

	case e_UnimplementedService: {
		D.Out(pdExcept, "Throwing e_UnimplementedService exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_UnknownLabel: {
		D.Out(pdExcept, "Throwing e_UnknownLabel exception.");
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_ValueCountExceeded: {
		D.Out(pdExcept, "Throwing e_ValueCountExceeded exception.");
		// JvY: Changed name of exception
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;

	case e_ValueLengthExceeded: {
		D.Out(pdExcept, "Throwing e_ValueLengthExceeded exception.");
		// JvY: Changed name of exception
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;
	
	case e_IllegalName: {
		D.Out(pdExcept, "Throwing e_IllegalName exception.");
		throw rti1516e::IllegalName(msg->getExceptionReasonW());
	} break;


	default: {
		D.Out(pdExcept, "Throwing unknown exception !");
		std::cout << "LibRTI: Receiving unknown RTI exception." << std::endl;
		throw rti1516e::RTIinternalError(msg->getExceptionReasonW());
	} break ;
	}
}

#define CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(service) \
		catch (rti1516e::Exception &e) { \
			std::wstringstream msg; \
			msg << "Error in " << service << ": " << e.what(); \
			throw rti1516e::RTIinternalError(msg.str()); \
		}

void
RTI1516ambPrivateRefs::callFederateAmbassador(Message *msg)
throw (rti1516e::RTIinternalError)
{
	switch (msg->getMessageType()) {

	case Message::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
		try {
			fed_amb->synchronizationPointRegistrationSucceeded(msg->getLabelW());
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"synchronizationPointRegistrationSucceeded")
		break ;

	case Message::SYNCHRONIZATION_POINT_REGISTRATION_FAILED:
		try {
			fed_amb->synchronizationPointRegistrationFailed(msg->getLabelW(), rti1516e::SYNCHRONIZATION_SET_MEMBER_NOT_JOINED);
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"synchronizationPointRegistrationFailed")
		break ;

	case Message::ANNOUNCE_SYNCHRONIZATION_POINT:
		try {
			std::string tagString(msg->getTag());
			rti1516e::VariableLengthData tagVarData(tagString.c_str(), tagString.length());
			fed_amb->announceSynchronizationPoint(msg->getLabelW(),tagVarData);
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"announceSynchronizationPoint")
		break ;

	case Message::FEDERATION_SYNCHRONIZED:
		try {
		    /* FIXME: how to setup the FHS content */
		    rti1516e::FederateHandleSet fhs;
			fed_amb->federationSynchronized(msg->getLabelW(),fhs);
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"federationSynchronized")
		break ;

	case Message::INITIATE_FEDERATE_SAVE:
		try {
			fed_amb->initiateFederateSave((msg->getLabelW()));
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"initiateFederateSave")
		break ;

	case Message::FEDERATION_SAVED:
		try {
			fed_amb->federationSaved();
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"federationSaved")
		break ;

	case Message::REQUEST_FEDERATION_RESTORE_SUCCEEDED:
		try {
			fed_amb->requestFederationRestoreSucceeded(
					(msg->getLabelW()));
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"requestFederationRestoreSucceeded")
		break ;

	case Message::REQUEST_FEDERATION_RESTORE_FAILED:
		try {
			fed_amb->requestFederationRestoreFailed(msg->getLabelW());
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"requestFederationRestoreFailed")
		break ;

	case Message::FEDERATION_RESTORE_BEGUN:
		try {
			fed_amb->federationRestoreBegun();
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"federationRestoreBegun")
		break ;

	case Message::INITIATE_FEDERATE_RESTORE:
		try {
			M_Initiate_Federate_Restore *IFR = static_cast<M_Initiate_Federate_Restore *>(msg);
			rti1516e::FederateHandle rti1516Handle = rti1516e::FederateHandleFriend::createRTI1516Handle(IFR->getFederate());
			/* FIXME how to setup federateName (2nd) argument) */
			fed_amb->initiateFederateRestore((IFR->getLabelW()), L"NoFederateName",rti1516Handle);
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"initiateFederateRestore")
		break ;

	case Message::FEDERATION_RESTORED:
		try {
			fed_amb->federationRestored();
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"federationRestored")
		break ;

	case Message::FEDERATION_NOT_RESTORED:
		try {
			// TODO Need to know reason
			fed_amb->federationNotRestored(rti1516e::RTI_UNABLE_TO_RESTORE);
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"federationNotRestored")
		break ;

	case Message::START_REGISTRATION_FOR_OBJECT_CLASS:
		try {
			rti1516e::ObjectClassHandle rti1516Handle = rti1516e::ObjectClassHandleFriend::createRTI1516Handle(static_cast<M_Start_Registration_For_Object_Class *>(msg)->getObjectClass());
			fed_amb->startRegistrationForObjectClass(rti1516Handle);
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"startRegistrationForObjectClass")
		break ;

	case Message::STOP_REGISTRATION_FOR_OBJECT_CLASS:
		try {
			rti1516e::ObjectClassHandle rti1516Handle = rti1516e::ObjectClassHandleFriend::createRTI1516Handle(static_cast<M_Stop_Registration_For_Object_Class *>(msg)->getObjectClass());
			fed_amb->stopRegistrationForObjectClass(rti1516Handle);
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"stopRegistrationForObjectClass")
		break ;

	case Message::TURN_INTERACTIONS_ON:
		try {
			rti1516e::InteractionClassHandle rti1516Handle = rti1516e::InteractionClassHandleFriend::createRTI1516Handle(static_cast<M_Turn_Interactions_On *>(msg)->getInteractionClass());
			fed_amb->turnInteractionsOn(rti1516Handle);
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"turnInteractionsOn")
		break ;

	case Message::TURN_INTERACTIONS_OFF:
		try {
			rti1516e::InteractionClassHandle rti1516Handle = rti1516e::InteractionClassHandleFriend::createRTI1516Handle(static_cast<M_Turn_Interactions_Off *>(msg)->getInteractionClass());
			fed_amb->turnInteractionsOff(rti1516Handle);
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"turnInteractionsOff")
		break ;

	case Message::DISCOVER_OBJECT_INSTANCE:
		try {
			M_Discover_Object_Instance* DOI = static_cast<M_Discover_Object_Instance *>(msg);
			rti1516e::ObjectInstanceHandle objectInstance = rti1516e::ObjectInstanceHandleFriend::createRTI1516Handle(DOI->getObject());
			rti1516e::ObjectClassHandle objectClass = rti1516e::ObjectClassHandleFriend::createRTI1516Handle(DOI->getObjectClass());
			std::string str = DOI->getObjectName();
			std::wstring str2(str.length(), L' '); // Make room for characters
			// Copy string to wstring.
			std::copy(str.begin(), str.end(), str2.begin());

			fed_amb->discoverObjectInstance(objectInstance,
					objectClass,
					str2);
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"discoverObjectInstance")
		break ;

	case Message::REFLECT_ATTRIBUTE_VALUES:
		try {
			M_Reflect_Attribute_Values* RAV = static_cast<M_Reflect_Attribute_Values *>(msg);
			G.Out(pdGendoc,"          tick_kernel call to reflectAttributeValues");

			rti1516e::ObjectInstanceHandle instance = rti1516e::ObjectInstanceHandleFriend::createRTI1516Handle(RAV->getObject());

			rti1516e::AttributeHandleValueMap *attributes = getAHVMFromRequest(RAV);

			std::string tagString(msg->getTag());
			rti1516e::VariableLengthData tagVarData(tagString.c_str(), tagString.length());
			/* FIXME 1516-2010: Howto setup SRI properly ?? */
			rti1516e::SupplementalReflectInfo sri;

			if (msg->isDated()) {
				uint64_t sn = RAV->getEventRetraction().getSN();
				certi::FederateHandle certiHandle = RAV->getEventRetraction().getSendingFederate();
				rti1516e::MessageRetractionHandle event = rti1516e::MessageRetractionHandleFriend::createRTI1516Handle(certiHandle, sn);

				std::auto_ptr < rti1516e::LogicalTime > fedTime (new RTI1516fedTime(msg->getDate().getTime()));


				fed_amb->reflectAttributeValues(instance, //ObjectInstanceHandle
						*attributes,							  //AttributeHandleValueMap &
						tagVarData,								  //VariableLengthData &
						rti1516e::TIMESTAMP,						  //OrderType (send)
						rti1516e::RELIABLE,						  //TransportationType
						*fedTime,								  //LogicalTime &
						rti1516e::RECEIVE,						  //OrderType (receive)
						event,									  //MessageRetractionHandle
						sri);
			}
			else {
				fed_amb->reflectAttributeValues(instance,
						*attributes,
						tagVarData,
						rti1516e::RECEIVE,
						rti1516e::RELIABLE,
						sri);
			}
			delete attributes ;
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"reflectAttributeValues")
		break ;

	case Message::RECEIVE_INTERACTION:
		try {
			M_Receive_Interaction* RI = static_cast<M_Receive_Interaction *>(msg);
			rti1516e::ParameterHandleValueMap *parameters = getPHVMFromRequest(RI);

			rti1516e::InteractionClassHandle interactionHandle = rti1516e::InteractionClassHandleFriend::createRTI1516Handle(RI->getInteractionClass());

			std::string tagString(msg->getTag());
			rti1516e::VariableLengthData tagVarData(tagString.c_str(), tagString.length());
			/* FIXME 1516-2010: Howto setup SRI properly ?? */
			rti1516e::SupplementalReceiveInfo sri;

			if (msg->isDated()) {
				uint64_t sn = RI->getEventRetraction().getSN();
				certi::FederateHandle certiHandle = RI->getEventRetraction().getSendingFederate();
				rti1516e::MessageRetractionHandle event = rti1516e::MessageRetractionHandleFriend::createRTI1516Handle(certiHandle, sn);

				std::auto_ptr < rti1516e::LogicalTime > fedTime (new RTI1516fedTime(msg->getDate().getTime()));

				fed_amb->receiveInteraction(
						interactionHandle,						// InteractionClassHandle
						*parameters,							// ParameterHandleValueMap &
						tagVarData,								// VariableLengthData &
						rti1516e::TIMESTAMP,						  //OrderType (send)
						rti1516e::RELIABLE,						  //TransportationType
						*fedTime,								  //LogicalTime &
						rti1516e::RECEIVE,						  //OrderType (receive)
						event,									  //MessageRetractionHandle
						sri);
			}
			else {
				fed_amb->receiveInteraction(
						interactionHandle,
						*parameters,
						tagVarData,
						rti1516e::RECEIVE,
						rti1516e::RELIABLE,
						sri);
			}
			delete parameters ;
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"receiveInteraction")
		break ;

	case Message::REMOVE_OBJECT_INSTANCE:
		try {
			M_Remove_Object_Instance* ROI = static_cast<M_Remove_Object_Instance *>(msg);

			rti1516e::ObjectInstanceHandle instance = rti1516e::ObjectInstanceHandleFriend::createRTI1516Handle(ROI->getObject());

			std::string tagString(msg->getTag());
			rti1516e::VariableLengthData tagVarData(tagString.c_str(), tagString.length());
			/* FIXME 1516-2010: Howto setup SRI properly ?? */
			rti1516e::SupplementalRemoveInfo sri;

			if (msg->isDated()) {
				uint64_t sn = ROI->getEventRetraction().getSN();
				certi::FederateHandle certiHandle = ROI->getEventRetraction().getSendingFederate();
				rti1516e::MessageRetractionHandle event = rti1516e::MessageRetractionHandleFriend::createRTI1516Handle(certiHandle, sn);

				std::auto_ptr < rti1516e::LogicalTime > fedTime (new RTI1516fedTime(msg->getDate().getTime()));

				fed_amb->removeObjectInstance(
						instance,
						tagVarData,
						rti1516e::TIMESTAMP,
						*fedTime,
						rti1516e::RECEIVE,
						event,
						sri);
			}
			else {
				fed_amb->removeObjectInstance(
						instance,
						tagVarData,
						rti1516e::RECEIVE,
						sri);
			}
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"removeObjectInstance")
		break ;

	case Message::PROVIDE_ATTRIBUTE_VALUE_UPDATE:
		try {
			M_Provide_Attribute_Value_Update* PAVU = static_cast<M_Provide_Attribute_Value_Update *>(msg);

			rti1516e::AttributeHandleSet *attributeSet = getAHSFromRequest(PAVU);

			rti1516e::ObjectInstanceHandle instance = rti1516e::ObjectInstanceHandleFriend::createRTI1516Handle(PAVU->getObject());

			std::string tag = PAVU->getTag();
			rti1516e::VariableLengthData tagVarData(tag.c_str(), tag.size());

			fed_amb->provideAttributeValueUpdate(instance,*attributeSet, tagVarData);
			delete attributeSet ;
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"provideAttributeValueUpdate")
		break ;

	case Message::REQUEST_RETRACTION: {

	} break ;

	case Message::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
		try {
			M_Request_Attribute_Ownership_Assumption* RAOA = static_cast<M_Request_Attribute_Ownership_Assumption *>(msg);
			rti1516e::AttributeHandleSet *attributeSet = getAHSFromRequest(RAOA);

			rti1516e::ObjectInstanceHandle instance = rti1516e::ObjectInstanceHandleFriend::createRTI1516Handle(RAOA->getObject());

			std::string tag = RAOA->getTag();
			rti1516e::VariableLengthData tagVarData(tag.c_str(), tag.size());

			fed_amb->
			requestAttributeOwnershipAssumption(instance,
					*attributeSet,
					tagVarData);
			delete attributeSet ;
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"requestAttributeOwnershipAssumption")
		break ;

	case Message::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
		try {
			M_Request_Attribute_Ownership_Release* RAOR = static_cast<M_Request_Attribute_Ownership_Release *>(msg);			
			rti1516e::AttributeHandleSet *attributeSet = getAHSFromRequest(RAOR);

			rti1516e::ObjectInstanceHandle instance = rti1516e::ObjectInstanceHandleFriend::createRTI1516Handle(RAOR->getObject());

			std::string tag = RAOR->getTag();
			rti1516e::VariableLengthData tagVarData(tag.c_str(), tag.size());

			fed_amb->requestAttributeOwnershipRelease(
					instance,
					*attributeSet,
					tagVarData);

			delete attributeSet ;
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"requestAttributeOwnershipRelease")
		break ;

	case Message::ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
		try {
			M_Attribute_Ownership_Unavailable* AOU = static_cast<M_Attribute_Ownership_Unavailable *>(msg);
			
			rti1516e::AttributeHandleSet *attributeSet = getAHSFromRequest(AOU);

			rti1516e::ObjectInstanceHandle instance = rti1516e::ObjectInstanceHandleFriend::createRTI1516Handle(AOU->getObject());

			fed_amb->attributeOwnershipUnavailable(instance,
					*attributeSet);

			delete attributeSet ;
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"attributeOwnershipUnavailable")
		break ;

	case Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
		try {
			M_Attribute_Ownership_Acquisition_Notification* AOAN = static_cast<M_Attribute_Ownership_Acquisition_Notification *>(msg);
			
			rti1516e::AttributeHandleSet *attributeSet = getAHSFromRequest(AOAN);

			rti1516e::ObjectInstanceHandle instance = rti1516e::ObjectInstanceHandleFriend::createRTI1516Handle(AOAN->getObject());

			std::string tag = AOAN->getTag();
			rti1516e::VariableLengthData tagVarData(tag.c_str(), tag.size());
			
			fed_amb->attributeOwnershipAcquisitionNotification(
					instance,
					*attributeSet,
					tagVarData);

			delete attributeSet ;
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"attributeOwnershipAcquisitionNotification")
		break ;

	case Message::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
		try {
			M_Attribute_Ownership_Divestiture_Notification* AODN = static_cast<M_Attribute_Ownership_Divestiture_Notification *>(msg);
			
			rti1516e::AttributeHandleSet *attributeSet = getAHSFromRequest(AODN);

			rti1516e::ObjectInstanceHandle instance = rti1516e::ObjectInstanceHandleFriend::createRTI1516Handle(AODN->getObject());
			
			// requestDivestitureConfirmation = 7.5, en zou ontbreken in 1.3. JRE: klopt dit wel?
			// Changed function name! Originally: attributeOwnershipDivestitureNotification
			fed_amb->requestDivestitureConfirmation(
					instance,
					*attributeSet);

			delete attributeSet ;
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"attributeOwnershipDivestitureNotification")
		break ;

	case Message::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
		try {
			M_Confirm_Attribute_Ownership_Acquisition_Cancellation* CAOAC = static_cast<M_Confirm_Attribute_Ownership_Acquisition_Cancellation *>(msg);
			
			rti1516e::AttributeHandleSet *attributeSet = getAHSFromRequest(CAOAC);

			rti1516e::ObjectInstanceHandle instance = rti1516e::ObjectInstanceHandleFriend::createRTI1516Handle(CAOAC->getObject());

			fed_amb->confirmAttributeOwnershipAcquisitionCancellation(
					instance,
					*attributeSet);

			delete attributeSet ;
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"confirmAttributeOwnershipAcquisitionCancellation")
		break ;

	case Message::INFORM_ATTRIBUTE_OWNERSHIP:
		try {
			M_Inform_Attribute_Ownership* IAO = static_cast<M_Inform_Attribute_Ownership *>(msg);
			rti1516e::ObjectInstanceHandle instance = rti1516e::ObjectInstanceHandleFriend::createRTI1516Handle(IAO->getObject());
			rti1516e::AttributeHandle attribute = rti1516e::AttributeHandleFriend::createRTI1516Handle(IAO->getAttribute());
			rti1516e::FederateHandle federate = rti1516e::FederateHandleFriend::createRTI1516Handle(IAO->getFederate());

			fed_amb->
			informAttributeOwnership(
					instance,
					attribute,
					federate);
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"informAttributeOwnership")
		break ;

	case Message::ATTRIBUTE_IS_NOT_OWNED:
		try {
			M_Attribute_Is_Not_Owned* AINO = static_cast<M_Attribute_Is_Not_Owned *>(msg);
			rti1516e::ObjectInstanceHandle instance = rti1516e::ObjectInstanceHandleFriend::createRTI1516Handle(AINO->getObject());
			rti1516e::AttributeHandle attribute = rti1516e::AttributeHandleFriend::createRTI1516Handle(AINO->getAttribute());
			
			fed_amb->attributeIsNotOwned(
					instance,
					attribute);
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"attributeIsNotOwned")
		break ;

	case Message::TIME_ADVANCE_GRANT:
		try {
			fed_amb->timeAdvanceGrant(RTI1516fedTime(msg->getDate().getTime()));
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"timeAdvanceGrant")
		break ;

	case Message::TIME_REGULATION_ENABLED:
		try {
			std::auto_ptr < rti1516e::LogicalTime > fedTime (new RTI1516fedTime(msg->getDate().getTime()));
			fed_amb->timeRegulationEnabled(*fedTime);
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"timeRegulationEnabled")
		break ;

	case Message::TIME_CONSTRAINED_ENABLED:
		try {
			std::auto_ptr < rti1516e::LogicalTime > fedTime (new RTI1516fedTime(msg->getDate().getTime()));
			fed_amb->timeConstrainedEnabled(*fedTime);
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"timeConstrainedEnabled")
		break ;

	case Message::RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED:
		try {
			M_Reserve_Object_Instance_Name_Succeeded* ROINS = static_cast<M_Reserve_Object_Instance_Name_Succeeded *>(msg);
			std::string objName_s(ROINS->getObjectName());
			std::wstring objName_ws(objName_s.begin(), objName_s.end());

			fed_amb->objectInstanceNameReservationSucceeded(objName_ws);
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"reserveObjectInstanceNameSucceeded")
		break;
		
	case Message::RESERVE_OBJECT_INSTANCE_NAME_FAILED:
		try {
			M_Reserve_Object_Instance_Name_Failed* ROINS = static_cast<M_Reserve_Object_Instance_Name_Failed *>(msg);
			std::string objName_s(ROINS->getObjectName());
			std::wstring objName_ws(objName_s.begin(), objName_s.end());

			fed_amb->objectInstanceNameReservationFailed(objName_ws);
		}
		CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"reserveObjectInstanceNameFailed")
		break;

	default:
		leave("RTI service requested by RTI is unknown.");
	}
}

// $Id: RTIambPrivateRefs.cpp,v 1.2 2014/03/07 18:00:49 erk Exp $
