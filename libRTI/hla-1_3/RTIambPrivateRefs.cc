// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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
// $Id: RTIambPrivateRefs.cc,v 1.1 2014/03/03 15:18:24 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIambPrivateRefs.hh"
#include "RTItypesImp.hh"
#include "PrettyDebug.hh"
#include "M_Classes.hh"
#include <sstream>
#include <iostream>
#include <memory>

namespace {
static PrettyDebug D("LIBRTI", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);

template <typename T>
std::vector<std::pair<AttributeHandle, AttributeValue_t>> getAHVPSFromRequest(T* request)
{
    uint32_t size = request->getAttributesSize();
    std::vector<std::pair<AttributeHandle, AttributeValue_t>> result;
    result.resize(size);

    for (uint32_t i = 0; i < size; ++i) {
        result[i].first = request->getAttributes(i);
        result[i].second.assign(request->getValues(i).begin(), request->getValues(i).end());
    }

    return result;
}

template <typename T>
std::vector<std::pair<RTI::ParameterHandle, ParameterValue_t>> getPHVPSFromRequest(T* request)
{
    uint32_t size = request->getParametersSize();

    std::vector<std::pair<RTI::ParameterHandle, ParameterValue_t>> result;
    result.resize(size);

    for (uint32_t i = 0; i < size; ++i) {
        result[i].first = request->getParameters(i);
        result[i].second.assign(request->getValues(i).begin(), request->getValues(i).end());
    }

    return result;
}
}

RTIambPrivateRefs::RTIambPrivateRefs()
{
    fed_amb = NULL;
#ifdef _WIN32
    handle_RTIA = (HANDLE) -1;
#else
    pid_RTIA = (pid_t) -1;
#endif
    is_reentrant = false;
    _theRootObj = NULL;
    socketUn = NULL;
}

RTIambPrivateRefs::~RTIambPrivateRefs()
{
    delete socketUn;
}

// ----------------------------------------------------------------------------
void RTIambPrivateRefs::leave(const char* msg) throw(RTI::RTIinternalError)
{
    std::stringstream smsg;
    smsg << "RTI called leave because <" << msg << ">";
    throw RTI::RTIinternalError(smsg.str().c_str());
}

// ----------------------------------------------------------------------------
void RTIambPrivateRefs::executeService(Message* req, Message* rep)
{
    Debug(G, pdGendoc) << "enter RTIambPrivateRefs::executeService(" << req->getMessageName() << ", "
                       << rep->getMessageName() << ")" << std::endl;

    Debug(D, pdDebug) << "sending request to RTIA." << std::endl;

    try {
        req->send(socketUn, msgBufSend);
    }
    catch (NetworkError) {
        std::cerr << "libRTI: exception: NetworkError (write)" << std::endl;
        throw RTI::RTIinternalError("libRTI: Network Write Error");
    }

    Debug(D, pdDebug) << "waiting RTIA reply." << std::endl;

    // waiting RTI reply.
    try {
        rep->receive(socketUn, msgBufReceive);
    }
    catch (NetworkError) {
        std::cerr << "libRTI: exception: NetworkError (read)" << std::endl;
        throw RTI::RTIinternalError("libRTI: Network Read Error waiting RTI reply");
    }

    Debug(D, pdDebug) << "RTIA reply received." << std::endl;

    if (rep->getMessageType() != req->getMessageType()) {
        std::cout << "LibRTI: Assertion failed: rep->type != req->type" << std::endl;
        throw RTI::RTIinternalError("RTIambPrivateRefs::executeService: "
                                    "rep->type != req->type");
    }

    Debug(D, pdDebug) << "processing returned exception (from reply)." << std::endl;
    // the services may only throw exceptions defined in the HLA standard
    // the RTIA is responsible for sending 'allowed' exceptions only
    processException(rep);

    Debug(D, pdDebug) << "exception processed." << std::endl;
    Debug(G, pdGendoc) << "exit RTIambPrivateRefs::executeService" << std::endl;
}

void RTIambPrivateRefs::sendTickRequestStop()
{
    Debug(G, pdGendoc) << "enter RTIambPrivateRefs::sendTickRequestStop" << std::endl;

    M_Tick_Request_Stop req, rep;

    try {
        req.send(socketUn, msgBufSend);
    }
    catch (NetworkError) {
        std::cerr << "libRTI: exception: NetworkError (write)" << std::endl;
        throw RTI::RTIinternalError("libRTI: Network Write Error");
    }

    try {
        rep.receive(socketUn, msgBufReceive);
    }
    catch (NetworkError) {
        std::cerr << "libRTI: exception: NetworkError (read)" << std::endl;
        throw RTI::RTIinternalError("libRTI: Network Read Error waiting RTI reply");
    }

    // ignore the response, ignore exceptions
    // rep->type == Message::TICK_REQUEST;

    Debug(G, pdGendoc) << "exit RTIambPrivateRefs::sendTickRequestStop" << std::endl;
}

// ----------------------------------------------------------------------------
//! Process exception from received message.
/*! When a message is received from RTIA, it can contains an exception.
  This exception is processed by this module and a new exception is thrown.
 */
void RTIambPrivateRefs::processException(Message* msg)
{
    Debug(D, pdExcept) << "num de l'exception : " << static_cast<int>(msg->getExceptionType()) << std::endl;
    switch (msg->getExceptionType()) {
    case Exception::Type::NO_EXCEPTION: {
    } break;

    case Exception::Type::ArrayIndexOutOfBounds: {
        Debug(D, pdExcept) << "Throwing e_ArrayIndexOutOfBounds exception." << std::endl;
        throw RTI::ArrayIndexOutOfBounds(msg->getExceptionReason());
    } break;

    case Exception::Type::AsynchronousDeliveryAlreadyEnabled: {
        Debug(D, pdExcept) << "Throwing e_AsynchronousDeliveryAlreadyEnabled exception." << std::endl;
        throw RTI::AsynchronousDeliveryAlreadyEnabled(msg->getExceptionReason());
    } break;

    case Exception::Type::AsynchronousDeliveryAlreadyDisabled: {
        Debug(D, pdExcept) << "Throwing e_AsynchronousDeliveryAlreadyDisabled exception." << std::endl;
        throw RTI::AsynchronousDeliveryAlreadyDisabled(msg->getExceptionReason());
    } break;

    case Exception::Type::AttributeAlreadyOwned: {
        Debug(D, pdExcept) << "Throwing e_AttributeAlreadyOwned exception." << std::endl;
        throw RTI::AttributeAlreadyOwned(msg->getExceptionReason());
    } break;

    case Exception::Type::AttributeAlreadyBeingAcquired: {
        Debug(D, pdExcept) << "Throwing e_AttributeAlreadyBeingAcquired exception." << std::endl;
        throw RTI::AttributeAlreadyBeingAcquired(msg->getExceptionReason());
    } break;

    case Exception::Type::AttributeAlreadyBeingDivested: {
        Debug(D, pdExcept) << "Throwing e_AttributeAlreadyBeingDivested exception." << std::endl;
        throw RTI::AttributeAlreadyBeingDivested(msg->getExceptionReason());
    } break;

    case Exception::Type::AttributeDivestitureWasNotRequested: {
        Debug(D, pdExcept) << "Throwing e_AttributeDivestitureWasNotRequested exception." << std::endl;
        throw RTI::AttributeDivestitureWasNotRequested(msg->getExceptionReason());
    } break;

    case Exception::Type::AttributeAcquisitionWasNotRequested: {
        Debug(D, pdExcept) << "Throwing e_AttributeAcquisitionWasNotRequested exception." << std::endl;
        throw RTI::AttributeAcquisitionWasNotRequested(msg->getExceptionReason());
    } break;

    case Exception::Type::AttributeNotDefined: {
        Debug(D, pdExcept) << "Throwing e_AttributeNotDefined exception." << std::endl;
        throw RTI::AttributeNotDefined(msg->getExceptionReason());
    } break;

    case Exception::Type::AttributeNotKnown: {
        Debug(D, pdExcept) << "Throwing e_AttributeNotKnown exception." << std::endl;
        throw RTI::AttributeNotKnown(msg->getExceptionReason());
    } break;

    case Exception::Type::AttributeNotOwned: {
        Debug(D, pdExcept) << "Throwing e_AttributeNotOwned exception." << std::endl;
        throw RTI::AttributeNotOwned(msg->getExceptionReason());
    } break;

    case Exception::Type::AttributeNotPublished: {
        Debug(D, pdExcept) << "Throwing e_AttributeNotPublished exception." << std::endl;
        throw RTI::AttributeNotPublished(msg->getExceptionReason());
    } break;

    case Exception::Type::AttributeNotSubscribed: {
        Debug(D, pdExcept) << "Throwing e_AttributeNotSubscribed exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::ConcurrentAccessAttempted: {
        Debug(D, pdExcept) << "Throwing e_ConcurrentAccessAttempted exception." << std::endl;
        throw RTI::ConcurrentAccessAttempted(msg->getExceptionReason());
    } break;

    case Exception::Type::CouldNotDiscover: {
        Debug(D, pdExcept) << "Throwing e_CouldNotDiscover exception." << std::endl;
        throw RTI::CouldNotDiscover(msg->getExceptionReason());
    } break;

    case Exception::Type::CouldNotOpenFED: {
        Debug(D, pdExcept) << "Throwing e_CouldNotOpenFED exception." << std::endl;
        throw RTI::CouldNotOpenFED(msg->getExceptionReason());
    } break;

    case Exception::Type::CouldNotOpenRID: {
        Debug(D, pdExcept) << "Throwing e_CouldNotOpenRID exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::CouldNotRestore: {
        Debug(D, pdExcept) << "Throwing e_CouldNotRestore exception." << std::endl;
        throw RTI::CouldNotRestore(msg->getExceptionReason());
    } break;

    case Exception::Type::DeletePrivilegeNotHeld: {
        Debug(D, pdExcept) << "Throwing e_DeletePrivilegeNotHeld exception." << std::endl;
        throw RTI::DeletePrivilegeNotHeld(msg->getExceptionReason());
    } break;

    case Exception::Type::ErrorReadingRID: {
        Debug(D, pdExcept) << "Throwing e_ErrorReadingRID exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::ErrorReadingFED: {
        Debug(D, pdExcept) << "Throwing e_ErrorReadingFED exception." << std::endl;
        throw RTI::ErrorReadingFED(msg->getExceptionReason());
    } break;

    case Exception::Type::EventNotKnown: {
        Debug(D, pdExcept) << "Throwing e_EventNotKnown exception." << std::endl;
        throw RTI::EventNotKnown(msg->getExceptionReason());
    } break;

    case Exception::Type::FederateAlreadyPaused: {
        Debug(D, pdExcept) << "Throwing e_FederateAlreadyPaused exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::FederateAlreadyExecutionMember: {
        Debug(D, pdExcept) << "Throwing e_FederateAlreadyExecutionMember exception." << std::endl;
        throw RTI::FederateAlreadyExecutionMember(msg->getExceptionReason());
    } break;

    case Exception::Type::FederateDoesNotExist: {
        Debug(D, pdExcept) << "Throwing e_FederateDoesNotExist exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::FederateInternalError: {
        Debug(D, pdExcept) << "Throwing e_FederateInternalError exception." << std::endl;
        throw RTI::FederateInternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::FederateNameAlreadyInUse: {
        Debug(D, pdExcept) << "Throwing e_FederateNameAlreadyInUse exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::FederateNotExecutionMember: {
        Debug(D, pdExcept) << "Throwing e_FederateNotExecutionMember exception." << std::endl;
        throw RTI::FederateNotExecutionMember(msg->getExceptionReason());
    } break;

    case Exception::Type::FederateNotPaused: {
        Debug(D, pdExcept) << "Throwing e_FederateNotPaused exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::FederateOwnsAttributes: {
        Debug(D, pdExcept) << "Throwing e_FederateOwnsAttributes exception." << std::endl;
        throw RTI::FederateOwnsAttributes(msg->getExceptionReason());
    } break;

    case Exception::Type::FederatesCurrentlyJoined: {
        Debug(D, pdExcept) << "Throwing e_FederatesCurrentlyJoined exception." << std::endl;
        throw RTI::FederatesCurrentlyJoined(msg->getExceptionReason());
    } break;

    case Exception::Type::FederateWasNotAskedToReleaseAttribute: {
        Debug(D, pdExcept) << "Throwing e_FederateWasNotAskedToReleaseAttribute exception." << std::endl;
        throw RTI::FederateWasNotAskedToReleaseAttribute(msg->getExceptionReason());
    } break;

    case Exception::Type::FederationAlreadyPaused: {
        Debug(D, pdExcept) << "Throwing e_FederationAlreadyPaused exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::FederationExecutionAlreadyExists: {
        Debug(D, pdExcept) << "Throwing e_FederationExecutionAlreadyExists excep." << std::endl;
        throw RTI::FederationExecutionAlreadyExists(msg->getExceptionReason());
    } break;

    case Exception::Type::FederationExecutionDoesNotExist: {
        Debug(D, pdExcept) << "Throwing e_FederationExecutionDoesNotExist except." << std::endl;
        throw RTI::FederationExecutionDoesNotExist(msg->getExceptionReason());
    } break;

    case Exception::Type::FederationNotPaused: {
        Debug(D, pdExcept) << "Throwing e_FederationNotPaused exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::FederationTimeAlreadyPassed: {
        Debug(D, pdExcept) << "Throwing e_FederationTimeAlreadyPassed exception." << std::endl;
        throw RTI::FederationTimeAlreadyPassed(msg->getExceptionReason());
    } break;

    case Exception::Type::FederateNotPublishing: {
        Debug(D, pdExcept) << "Throwing e_FederateNotPublishing exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::FederateNotSubscribing: {
        Debug(D, pdExcept) << "Throwing e_FederateNotSubscribing exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::RegionNotKnown: {
        Debug(D, pdExcept) << "Throwing e_RegionNotKnown exception." << std::endl;
        throw RTI::RegionNotKnown(msg->getExceptionReason());
    } break;

    case Exception::Type::IDsupplyExhausted: {
        Debug(D, pdExcept) << "Throwing e_IDsupplyExhausted exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::InteractionClassNotDefined: {
        Debug(D, pdExcept) << "Throwing e_InteractionClassNotDefined exception." << std::endl;
        throw RTI::InteractionClassNotDefined(msg->getExceptionReason());
    } break;

    case Exception::Type::InteractionClassNotKnown: {
        Debug(D, pdExcept) << "Throwing e_InteractionClassNotKnown exception." << std::endl;
        throw RTI::InteractionClassNotKnown(msg->getExceptionReason());
    } break;

    case Exception::Type::InteractionClassNotPublished: {
        Debug(D, pdExcept) << "Throwing e_InteractionClassNotPublished exception." << std::endl;
        throw RTI::InteractionClassNotPublished(msg->getExceptionReason());
    } break;

    case Exception::Type::InteractionParameterNotDefined: {
        Debug(D, pdExcept) << "Throwing e_InteractionParameterNotDefined exception." << std::endl;
        throw RTI::InteractionParameterNotDefined(msg->getExceptionReason());
    } break;

    case Exception::Type::InteractionParameterNotKnown: {
        Debug(D, pdExcept) << "Throwing e_InteractionParameterNotKnown exception." << std::endl;
        throw RTI::InteractionParameterNotKnown(msg->getExceptionReason());
    } break;

    case Exception::Type::InvalidDivestitureCondition: {
        Debug(D, pdExcept) << "Throwing e_InvalidDivestitureCondition exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::InvalidExtents: {
        Debug(D, pdExcept) << "Throwing e_InvalidExtents exception." << std::endl;
        throw RTI::InvalidExtents(msg->getExceptionReason());
    } break;

    case Exception::Type::InvalidFederationTime: {
        Debug(D, pdExcept) << "Throwing e_InvalidFederationTime exception." << std::endl;
        throw RTI::InvalidFederationTime(msg->getExceptionReason());
    } break;

    case Exception::Type::InvalidFederationTimeDelta: {
        Debug(D, pdExcept) << "Throwing e_InvalidFederationTimeDelta exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::InvalidLookahead: {
        Debug(D, pdExcept) << "Throwing e_InvalidLookahead." << std::endl;
        throw RTI::InvalidLookahead(msg->getExceptionReason());
    } break;

    case Exception::Type::InvalidObjectHandle: {
        Debug(D, pdExcept) << "Throwing e_InvalidObjectHandle exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::InvalidOrderingHandle: {
        Debug(D, pdExcept) << "Throwing e_InvalidOrderingHandle exception." << std::endl;
        throw RTI::InvalidOrderingHandle(msg->getExceptionReason());
    } break;

    case Exception::Type::InvalidResignAction: {
        Debug(D, pdExcept) << "Throwing e_InvalidResignAction exception." << std::endl;
        throw RTI::InvalidResignAction(msg->getExceptionReason());
    } break;

    case Exception::Type::InvalidRetractionHandle: {
        Debug(D, pdExcept) << "Throwing e_InvalidRetractionHandle exception." << std::endl;
        throw RTI::InvalidRetractionHandle(msg->getExceptionReason());
    } break;

    case Exception::Type::InvalidRoutingSpace: {
        Debug(D, pdExcept) << "Throwing e_InvalidRoutingSpace exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::InvalidTransportationHandle: {
        Debug(D, pdExcept) << "Throwing e_InvalidTransportationHandle exception." << std::endl;
        throw RTI::InvalidTransportationHandle(msg->getExceptionReason());
    } break;

    case Exception::Type::MemoryExhausted: {
        Debug(D, pdExcept) << "Throwing e_MemoryExhausted exception." << std::endl;
        throw RTI::MemoryExhausted(msg->getExceptionReason());
    } break;

    case Exception::Type::NameNotFound: {
        Debug(D, pdExcept) << "Throwing e_NameNotFound exception." << std::endl;
        throw RTI::NameNotFound(msg->getExceptionReason());
    } break;

    case Exception::Type::NoPauseRequested: {
        Debug(D, pdExcept) << "Throwing e_NoPauseRequested exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::NoResumeRequested: {
        Debug(D, pdExcept) << "Throwing e_NoResumeRequested exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::ObjectClassNotDefined: {
        Debug(D, pdExcept) << "Throwing e_ObjectClassNotDefined exception." << std::endl;
        throw RTI::ObjectClassNotDefined(msg->getExceptionReason());
    } break;

    case Exception::Type::ObjectClassNotKnown: {
        Debug(D, pdExcept) << "Throwing e_ObjectClassNotKnown exception." << std::endl;
        throw RTI::ObjectClassNotKnown(msg->getExceptionReason());
    } break;

    case Exception::Type::ObjectClassNotPublished: {
        Debug(D, pdExcept) << "Throwing e_ObjectClassNotPublished exception." << std::endl;
        throw RTI::ObjectClassNotPublished(msg->getExceptionReason());
    } break;

    case Exception::Type::ObjectClassNotSubscribed: {
        Debug(D, pdExcept) << "Throwing e_ObjectClassNotSubscribed exception." << std::endl;
        throw RTI::ObjectClassNotSubscribed(msg->getExceptionReason());
    } break;

    case Exception::Type::ObjectNotKnown: {
        Debug(D, pdExcept) << "Throwing e_ObjectNotKnown exception." << std::endl;
        throw RTI::ObjectNotKnown(msg->getExceptionReason());
    } break;

    case Exception::Type::ObjectAlreadyRegistered: {
        Debug(D, pdExcept) << "Throwing e_ObjectAlreadyRegistered exception." << std::endl;
        throw RTI::ObjectAlreadyRegistered(msg->getExceptionReason());
    } break;

    case Exception::Type::RestoreInProgress: {
        Debug(D, pdExcept) << "Throwing e_RestoreInProgress exception." << std::endl;
        throw RTI::RestoreInProgress(msg->getExceptionReason());
    } break;

    case Exception::Type::RestoreNotRequested: {
        Debug(D, pdExcept) << "Throwing e_RestoreNotRequested exception." << std::endl;
        throw RTI::RestoreNotRequested(msg->getExceptionReason());
    } break;

    case Exception::Type::RTIinternalError: {
        Debug(D, pdExcept) << "Throwing e_RTIinternalError exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::SpaceNotDefined: {
        Debug(D, pdExcept) << "Throwing e_SpaceNotDefined exception." << std::endl;
        throw RTI::SpaceNotDefined(msg->getExceptionReason());
    } break;

    case Exception::Type::SaveInProgress: {
        Debug(D, pdExcept) << "Throwing e_SaveInProgress exception." << std::endl;
        throw RTI::SaveInProgress(msg->getExceptionReason());
    } break;

    case Exception::Type::SaveNotInitiated: {
        Debug(D, pdExcept) << "Throwing e_SaveNotInitiated exception." << std::endl;
        throw RTI::SaveNotInitiated(msg->getExceptionReason());
    } break;

    case Exception::Type::SecurityError: {
        Debug(D, pdExcept) << "Throwing e_SecurityError exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::SpecifiedSaveLabelDoesNotExist: {
        Debug(D, pdExcept) << "Throwing e_SpecifiedSaveLabelDoesNotExist exception." << std::endl;
        throw RTI::SpecifiedSaveLabelDoesNotExist(msg->getExceptionReason());
    } break;

    case Exception::Type::TimeAdvanceAlreadyInProgress: {
        Debug(D, pdExcept) << "Throwing e_TimeAdvanceAlreadyInProgress exception." << std::endl;
        throw RTI::TimeAdvanceAlreadyInProgress(msg->getExceptionReason());
    } break;

    case Exception::Type::TimeAdvanceWasNotInProgress: {
        Debug(D, pdExcept) << "Throwing e_TimeAdvanceWasNotInProgress exception." << std::endl;
        throw RTI::TimeAdvanceWasNotInProgress(msg->getExceptionReason());
    } break;

    case Exception::Type::TooManyIDsRequested: {
        Debug(D, pdExcept) << "Throwing e_TooManyIDsRequested exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::UnableToPerformSave: {
        Debug(D, pdExcept) << "Throwing e_UnableToPerformSave exception." << std::endl;
        throw RTI::UnableToPerformSave(msg->getExceptionReason());
    } break;

    case Exception::Type::UnimplementedService: {
        Debug(D, pdExcept) << "Throwing e_UnimplementedService exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::UnknownLabel: {
        Debug(D, pdExcept) << "Throwing e_UnknownLabel exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;

    case Exception::Type::ValueCountExceeded: {
        Debug(D, pdExcept) << "Throwing e_ValueCountExceeded exception." << std::endl;
        throw RTI::ValueCountExceeded(msg->getExceptionReason());
    } break;

    case Exception::Type::ValueLengthExceeded: {
        Debug(D, pdExcept) << "Throwing e_ValueLengthExceeded exception." << std::endl;
        throw RTI::ValueLengthExceeded(msg->getExceptionReason());
    } break;

    default: {
        Debug(D, pdExcept) << "Throwing unknown exception !" << std::endl;
        std::cout << "LibRTI: Receiving unknown RTI exception." << std::endl;
        throw RTI::RTIinternalError(msg->getExceptionReason());
    } break;
    }
}

#define CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(service)                                                                  \
    catch (RTI::Exception & e)                                                                                         \
    {                                                                                                                  \
        std::stringstream msg;                                                                                         \
        msg << "Error in " << service << ": " << e._name << "[" << e._reason << "]";                                   \
        throw RTI::RTIinternalError(msg.str().c_str());                                                                \
    }

void RTIambPrivateRefs::callFederateAmbassador(Message* msg) throw(RTI::RTIinternalError)
{
    switch (msg->getMessageType()) {
    case Message::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
        try {
            fed_amb->synchronizationPointRegistrationSucceeded((msg->getLabel()).c_str());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("synchronizationPointRegistrationSucceeded")
        break;

    case Message::SYNCHRONIZATION_POINT_REGISTRATION_FAILED:
        try {
            fed_amb->synchronizationPointRegistrationFailed((msg->getLabel()).c_str());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("synchronizationPointRegistrationFailed")
        break;

    case Message::ANNOUNCE_SYNCHRONIZATION_POINT:
        try {
            fed_amb->announceSynchronizationPoint((msg->getLabel()).c_str(), (msg->getTag()).c_str());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("announceSynchronizationPoint")
        break;

    case Message::FEDERATION_SYNCHRONIZED:
        try {
            fed_amb->federationSynchronized((msg->getLabel()).c_str());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("federationSynchronized")
        break;

    case Message::INITIATE_FEDERATE_SAVE:
        try {
            fed_amb->initiateFederateSave((msg->getLabel()).c_str());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("initiateFederateSave")
        break;

    case Message::FEDERATION_SAVED:
        try {
            fed_amb->federationSaved();
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("federationSaved")
        break;

    case Message::REQUEST_FEDERATION_RESTORE_SUCCEEDED:
        try {
            fed_amb->requestFederationRestoreSucceeded((msg->getLabel()).c_str());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("requestFederationRestoreSucceeded")
        break;

    case Message::REQUEST_FEDERATION_RESTORE_FAILED:
        try {
            fed_amb->requestFederationRestoreFailed((msg->getLabel()).c_str(), (msg->getTag()).c_str());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("requestFederationRestoreFailed")
        break;

    case Message::FEDERATION_RESTORE_BEGUN:
        try {
            fed_amb->federationRestoreBegun();
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("federationRestoreBegun")
        break;

    case Message::INITIATE_FEDERATE_RESTORE:
        try {
            M_Initiate_Federate_Restore* IFR = static_cast<M_Initiate_Federate_Restore*>(msg);
            fed_amb->initiateFederateRestore((IFR->getLabel()).c_str(), IFR->getFederate());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("initiateFederateRestore")
        break;

    case Message::FEDERATION_RESTORED:
        try {
            fed_amb->federationRestored();
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("federationRestored")
        break;

    case Message::FEDERATION_NOT_RESTORED:
        try {
            fed_amb->federationNotRestored();
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("federationNotRestored")
        break;

    case Message::START_REGISTRATION_FOR_OBJECT_CLASS:
        try {
            fed_amb->startRegistrationForObjectClass(
                static_cast<M_Start_Registration_For_Object_Class*>(msg)->getObjectClass());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("startRegistrationForObjectClass")
        break;

    case Message::STOP_REGISTRATION_FOR_OBJECT_CLASS:
        try {
            fed_amb->stopRegistrationForObjectClass(
                static_cast<M_Stop_Registration_For_Object_Class*>(msg)->getObjectClass());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("stopRegistrationForObjectClass")
        break;

    case Message::TURN_INTERACTIONS_ON:
        try {
            fed_amb->turnInteractionsOn(static_cast<M_Turn_Interactions_On*>(msg)->getInteractionClass());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("turnInteractionsOn")
        break;

    case Message::TURN_INTERACTIONS_OFF:
        try {
            fed_amb->turnInteractionsOff(static_cast<M_Turn_Interactions_Off*>(msg)->getInteractionClass());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("turnInteractionsOff")
        break;

    case Message::DISCOVER_OBJECT_INSTANCE:
        try {
            M_Discover_Object_Instance* DOI = static_cast<M_Discover_Object_Instance*>(msg);
            fed_amb->discoverObjectInstance(DOI->getObject(), DOI->getObjectClass(), DOI->getObjectName().c_str());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("discoverObjectInstance")
        break;

    case Message::REFLECT_ATTRIBUTE_VALUES:
        try {
            M_Reflect_Attribute_Values* RAV = static_cast<M_Reflect_Attribute_Values*>(msg);
            Debug(G, pdGendoc) << "          tick_kernel call to reflectAttributeValues" << std::endl;
            RTI::AttributeHandleValuePairSet* attributes = new AttributeHandleValuePairSetImp(getAHVPSFromRequest(RAV));

            if (msg->isDated()) {
                RTI::EventRetractionHandle event;
                event.theSerialNumber = RAV->getEventRetraction().getSN();
                event.sendingFederate = RAV->getEventRetraction().getSendingFederate();
                fed_amb->reflectAttributeValues(RAV->getObject(),
                                                *attributes,
                                                RTIfedTime(msg->getDate().getTime()),
                                                (msg->getTag()).c_str(),
                                                event);
            }
            else {
                fed_amb->reflectAttributeValues(RAV->getObject(), *attributes, (msg->getTag()).c_str());
            }
            delete attributes;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("reflectAttributeValues")
        break;

    case Message::RECEIVE_INTERACTION:
        try {
            M_Receive_Interaction* RI = static_cast<M_Receive_Interaction*>(msg);
            RTI::ParameterHandleValuePairSet* parameters = new ParameterHandleValuePairSetImp(getPHVPSFromRequest(RI));

            if (msg->isDated()) {
                RTI::EventRetractionHandle event;
                event.theSerialNumber = RI->getEventRetraction().getSN();
                event.sendingFederate = RI->getEventRetraction().getSendingFederate();
                fed_amb->receiveInteraction(RI->getInteractionClass(),
                                            *parameters,
                                            RTIfedTime(msg->getDate().getTime()),
                                            (msg->getTag()).c_str(),
                                            event);
            }
            else {
                fed_amb->receiveInteraction(RI->getInteractionClass(), *parameters, (msg->getTag()).c_str());
            }
            delete parameters;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("receiveInteraction")
        break;

    case Message::REMOVE_OBJECT_INSTANCE:
        try {
            M_Remove_Object_Instance* ROI = static_cast<M_Remove_Object_Instance*>(msg);
            RTI::EventRetractionHandle event;
            event.theSerialNumber = ROI->getEventRetraction().getSN();
            event.sendingFederate = ROI->getEventRetraction().getSendingFederate();
            if (msg->isDated()) {
                fed_amb->removeObjectInstance(
                    ROI->getObject(), RTIfedTime(msg->getDate().getTime()), (msg->getTag()).c_str(), event);
            }
            else {
                fed_amb->removeObjectInstance(ROI->getObject(), (msg->getTag()).c_str());
            }
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("removeObjectInstance")
        break;

    case Message::PROVIDE_ATTRIBUTE_VALUE_UPDATE:
        try {
            M_Provide_Attribute_Value_Update* PAVU = static_cast<M_Provide_Attribute_Value_Update*>(msg);
            std::unique_ptr<RTI::AttributeHandleSet> attributeSet(new AttributeHandleSetImp(PAVU->getAttributes()));

            fed_amb->provideAttributeValueUpdate(PAVU->getObject(), *attributeSet);
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("provideAttributeValueUpdate")
        break;

    case Message::REQUEST_RETRACTION: {
    } break;

    case Message::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
        try {
            M_Request_Attribute_Ownership_Assumption* RAOA
                = static_cast<M_Request_Attribute_Ownership_Assumption*>(msg);
            RTI::AttributeHandleSet* attributeSet = new AttributeHandleSetImp(RAOA->getAttributes());

            fed_amb->requestAttributeOwnershipAssumption(RAOA->getObject(), *attributeSet, (msg->getTag()).c_str());
            delete attributeSet;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("requestAttributeOwnershipAssumption")
        break;

    case Message::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
        try {
            M_Request_Attribute_Ownership_Release* RAOR = static_cast<M_Request_Attribute_Ownership_Release*>(msg);
            RTI::AttributeHandleSet* attributeSet = new AttributeHandleSetImp(RAOR->getAttributes());

            fed_amb->requestAttributeOwnershipRelease(RAOR->getObject(), *attributeSet, (msg->getTag()).c_str());

            delete attributeSet;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("requestAttributeOwnershipRelease")
        break;

    case Message::ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
        try {
            M_Attribute_Ownership_Unavailable* AOU = static_cast<M_Attribute_Ownership_Unavailable*>(msg);
            RTI::AttributeHandleSet* attributeSet = new AttributeHandleSetImp(AOU->getAttributes());

            fed_amb->attributeOwnershipUnavailable(AOU->getObject(), *attributeSet);

            delete attributeSet;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("attributeOwnershipUnavailable")
        break;

    case Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
        try {
            M_Attribute_Ownership_Acquisition_Notification* AOAN
                = static_cast<M_Attribute_Ownership_Acquisition_Notification*>(msg);
            RTI::AttributeHandleSet* attributeSet = new AttributeHandleSetImp(AOAN->getAttributes());

            fed_amb->attributeOwnershipAcquisitionNotification(AOAN->getObject(), *attributeSet);

            delete attributeSet;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("attributeOwnershipAcquisitionNotification")
        break;

    case Message::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
        try {
            M_Attribute_Ownership_Divestiture_Notification* AODN
                = static_cast<M_Attribute_Ownership_Divestiture_Notification*>(msg);
            RTI::AttributeHandleSet* attributeSet = new AttributeHandleSetImp(AODN->getAttributes());

            fed_amb->attributeOwnershipDivestitureNotification(AODN->getObject(), *attributeSet);

            delete attributeSet;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("attributeOwnershipDivestitureNotification")
        break;

    case Message::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
        try {
            M_Confirm_Attribute_Ownership_Acquisition_Cancellation* CAOAC
                = static_cast<M_Confirm_Attribute_Ownership_Acquisition_Cancellation*>(msg);
            RTI::AttributeHandleSet* attributeSet = new AttributeHandleSetImp(CAOAC->getAttributes());

            fed_amb->confirmAttributeOwnershipAcquisitionCancellation(CAOAC->getObject(), *attributeSet);

            delete attributeSet;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("confirmAttributeOwnershipAcquisitionCancellation")
        break;

    case Message::INFORM_ATTRIBUTE_OWNERSHIP:
        try {
            M_Inform_Attribute_Ownership* IAO = static_cast<M_Inform_Attribute_Ownership*>(msg);
            fed_amb->informAttributeOwnership(IAO->getObject(), IAO->getAttribute(), IAO->getFederate());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("informAttributeOwnership")
        break;

    case Message::ATTRIBUTE_IS_NOT_OWNED:
        try {
            M_Attribute_Is_Not_Owned* AINO = static_cast<M_Attribute_Is_Not_Owned*>(msg);
            fed_amb->attributeIsNotOwned(AINO->getObject(), AINO->getAttribute());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("attributeIsNotOwned")
        break;

    case Message::TIME_ADVANCE_GRANT:
        try {
            fed_amb->timeAdvanceGrant(RTIfedTime(msg->getDate().getTime()));
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("timeAdvanceGrant")
        break;

    case Message::TIME_REGULATION_ENABLED:
        try {
            fed_amb->timeRegulationEnabled(RTIfedTime(msg->getDate().getTime()));
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("timeRegulationEnabled")
        break;

    case Message::TIME_CONSTRAINED_ENABLED:
        try {
            fed_amb->timeConstrainedEnabled(RTIfedTime(msg->getDate().getTime()));
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS("timeConstrainedEnabled")
        break;

    default:
        leave("RTI service requested by RTI is unknown.");
    }
}

// $Id: RTIambPrivateRefs.cc,v 1.1 2014/03/03 15:18:24 erk Exp $
