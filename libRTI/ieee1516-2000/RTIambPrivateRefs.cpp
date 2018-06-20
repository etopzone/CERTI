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
// $Id: RTIambPrivateRefs.cpp,v 1.1 2014/03/03 16:41:51 erk Exp $
// ----------------------------------------------------------------------------

#include "HandleImplementation.h"
#include "RTIambPrivateRefs.h"
#include <config.h>

#include "RTI1516fedTime.h"
#include "RTIHandleFactory.h"
#include <RTI/Exception.h>
#include <RTI/certiLogicalTime.h>
#include <RTI/certiLogicalTimeFactory.h>
#include <RTI/certiLogicalTimeInterval.h>

#include "M_Classes.hh"
#include "PrettyDebug.hh"
#include <iostream>
#include <sstream>

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
        result[i].second.assign(request->getValues(i).data(), request->getValues(i).length());
    }

    return result;
}

template <typename T>
std::vector<std::pair<rti1516::ParameterHandle, ParameterValue_t>> getPHVPSFromRequest(T* request)
{
    uint32_t size = request->getParametersSize();

    std::vector<std::pair<rti1516::ParameterHandle, ParameterValue_t>> result;
    result.resize(size);

    for (uint32_t i = 0; i < size; ++i) {
        result[i].first = request->getParameters(i);
        result[i].second.assign(request->getValues(i).data(), request->getValues(i).length());
    }

    return result;
}

template <typename T>
rti1516::AttributeHandleValueMap* getAHVMFromRequest(T* request)
{
    uint32_t size = request->getAttributesSize();
    rti1516::AttributeHandleValueMap* result = new rti1516::AttributeHandleValueMap();

    for (uint32_t i = 0; i < size; ++i) {
        rti1516::AttributeHandle attribute
            = rti1516::AttributeHandleFriend::createRTI1516Handle(request->getAttributes(i));
        rti1516::VariableLengthData varData(&(request->getValues(i)[0]), request->getValues(i).size());
        result->insert(std::pair<rti1516::AttributeHandle, rti1516::VariableLengthData>(attribute, varData));
    }

    return result;
}

template <typename T>
rti1516::ParameterHandleValueMap* getPHVMFromRequest(T* request)
{
    uint32_t size = request->getParametersSize();
    rti1516::ParameterHandleValueMap* result = new rti1516::ParameterHandleValueMap();

    for (uint32_t i = 0; i < size; ++i) {
        rti1516::ParameterHandle parameter
            = rti1516::ParameterHandleFriend::createRTI1516Handle(request->getParameters(i));
        rti1516::VariableLengthData varData(&(request->getValues(i)[0]), request->getValues(i).size());
        result->insert(std::pair<rti1516::ParameterHandle, rti1516::VariableLengthData>(parameter, varData));
    }

    return result;
}

template <typename T>
rti1516::AttributeHandleSet* getAHSFromRequest(T* request)
{
    uint32_t size = request->getAttributesSize();
    rti1516::AttributeHandleSet* result = new rti1516::AttributeHandleSet();

    for (uint32_t i = 0; i < size; ++i) {
        rti1516::AttributeHandle attribute
            = rti1516::AttributeHandleFriend::createRTI1516Handle(request->getAttributes(i));
        result->insert(attribute);
    }

    return result;
}

std::unique_ptr<rti1516::LogicalTime> getLogicalTime() throw(rti1516::RTIinternalError)
{
    std::unique_ptr<rti1516::LogicalTimeFactory> timeFactory
        = rti1516::LogicalTimeFactoryFactory::makeLogicalTimeFactory(L"");
    if (timeFactory.get() != 0) {
        return timeFactory->makeLogicalTime();
    }
    else {
        throw rti1516::RTIinternalError(L"Could not create LogicalTimeFactory");
    }
}

} // End anonymous namespace

RTI1516ambPrivateRefs::RTI1516ambPrivateRefs()
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

RTI1516ambPrivateRefs::~RTI1516ambPrivateRefs()
{
    delete socketUn;
}

// ----------------------------------------------------------------------------
void RTI1516ambPrivateRefs::leave(const char* msg) throw(rti1516::RTIinternalError)
{
    std::wstringstream smsg;
    smsg << "RTI called leave because <" << msg << ">";
    throw rti1516::RTIinternalError(smsg.str());
}

// ----------------------------------------------------------------------------
void RTI1516ambPrivateRefs::executeService(Message* req, Message* rep)
{
    Debug(G, pdGendoc) << "enter RTI1516ambPrivateRefs::executeService(" << req->getMessageName() << ", "
                       << rep->getMessageName() << ")" << std::endl;

    Debug(D, pdDebug) << "sending request to RTIA." << std::endl;

    try {
        req->send(socketUn, msgBufSend);
    }
    catch (certi::NetworkError) {
        std::cerr << "libRTI: exception: NetworkError (write)" << std::endl;
        if (req->getMessageType() == certi::Message::CLOSE_CONNEXION) {
            std::cerr << "libRTI: Could not execute 'Close connexion' service (Network error). Service request ignored."
                      << std::endl;
            return;
        }
        else {
            throw rti1516::RTIinternalError(L"libRTI: Network Write Error");
        }
    }

    Debug(D, pdDebug) << "waiting RTIA reply." << std::endl;

    // waiting RTI reply.
    try {
        rep->receive(socketUn, msgBufReceive);
    }
    catch (certi::NetworkError) {
        std::cerr << "libRTI: exception: NetworkError (read)" << std::endl;
        throw rti1516::RTIinternalError(L"libRTI: Network Read Error waiting RTI reply");
    }

    Debug(D, pdDebug) << "RTIA reply received." << std::endl;

    if (rep->getMessageType() != req->getMessageType()) {
        std::cout << "LibRTI: Assertion failed: rep->type != req->type" << std::endl;
        throw rti1516::RTIinternalError(L"RTI1516ambPrivateRefs::executeService: "
                                        L"rep->type != req->type");
    }

    Debug(D, pdDebug) << "processing returned exception (from reply)." << std::endl;
    // the services may only throw exceptions defined in the HLA standard
    // the RTIA is responsible for sending 'allowed' exceptions only
    processException(rep);

    Debug(D, pdDebug) << "exception processed." << std::endl;
    Debug(G, pdGendoc) << "exit RTI1516ambPrivateRefs::executeService" << std::endl;
}

void RTI1516ambPrivateRefs::sendTickRequestStop()
{
    Debug(G, pdGendoc) << "enter RTI1516ambPrivateRefs::sendTickRequestStop" << std::endl;

    M_Tick_Request_Stop req, rep;

    try {
        req.send(socketUn, msgBufSend);
    }
    catch (certi::NetworkError) {
        std::cerr << "libRTI: exception: NetworkError (write)" << std::endl;
        throw rti1516::RTIinternalError(L"libRTI: Network Write Error");
    }

    try {
        rep.receive(socketUn, msgBufReceive);
    }
    catch (certi::NetworkError) {
        std::cerr << "libRTI: exception: NetworkError (read)" << std::endl;
        throw rti1516::RTIinternalError(L"libRTI: Network Read Error waiting RTI reply");
    }

    // ignore the response, ignore exceptions
    // rep->type == Message::TICK_REQUEST;

    Debug(G, pdGendoc) << "exit RTI1516ambPrivateRefs::sendTickRequestStop" << std::endl;
}

// ----------------------------------------------------------------------------
//! Process exception from received message.
/*! When a message is received from RTIA, it can contains an exception.
  This exception is processed by this module and a new exception is thrown.
 */
void RTI1516ambPrivateRefs::processException(Message* msg)
{
    Debug(D, pdExcept) << "exception : " << static_cast<int>(msg->getExceptionType()) << std::endl;
    switch (msg->getExceptionType()) {
    case Exception::Type::NO_EXCEPTION: {
    } break;

    case Exception::Type::ArrayIndexOutOfBounds: {
        Debug(D, pdExcept) << "Throwing e_ArrayIndexOutOfBounds exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::AsynchronousDeliveryAlreadyEnabled: {
        Debug(D, pdExcept) << "Throwing e_AsynchronousDeliveryAlreadyEnabled exception." << std::endl;
        throw rti1516::AsynchronousDeliveryAlreadyEnabled(msg->getExceptionReasonW());
    } break;

    case Exception::Type::AsynchronousDeliveryAlreadyDisabled: {
        Debug(D, pdExcept) << "Throwing e_AsynchronousDeliveryAlreadyDisabled exception." << std::endl;
        throw rti1516::AsynchronousDeliveryAlreadyDisabled(msg->getExceptionReasonW());
    } break;

    case Exception::Type::AttributeAlreadyOwned: {
        Debug(D, pdExcept) << "Throwing e_AttributeAlreadyOwned exception." << std::endl;
        throw rti1516::AttributeAlreadyOwned(msg->getExceptionReasonW());
    } break;

    case Exception::Type::AttributeAlreadyBeingAcquired: {
        Debug(D, pdExcept) << "Throwing e_AttributeAlreadyBeingAcquired exception." << std::endl;
        throw rti1516::AttributeAlreadyBeingAcquired(msg->getExceptionReasonW());
    } break;

    case Exception::Type::AttributeAlreadyBeingDivested: {
        Debug(D, pdExcept) << "Throwing e_AttributeAlreadyBeingDivested exception." << std::endl;
        throw rti1516::AttributeAlreadyBeingDivested(msg->getExceptionReasonW());
    } break;

    case Exception::Type::AttributeDivestitureWasNotRequested: {
        Debug(D, pdExcept) << "Throwing e_AttributeDivestitureWasNotRequested exception." << std::endl;
        throw rti1516::AttributeDivestitureWasNotRequested(msg->getExceptionReasonW());
    } break;

    case Exception::Type::AttributeAcquisitionWasNotRequested: {
        Debug(D, pdExcept) << "Throwing e_AttributeAcquisitionWasNotRequested exception." << std::endl;
        throw rti1516::AttributeAcquisitionWasNotRequested(msg->getExceptionReasonW());
    } break;

    case Exception::Type::AttributeNotDefined: {
        Debug(D, pdExcept) << "Throwing e_AttributeNotDefined exception." << std::endl;
        throw rti1516::AttributeNotDefined(msg->getExceptionReasonW());
    } break;

    case Exception::Type::AttributeNotKnown: {
        Debug(D, pdExcept) << "Throwing e_AttributeNotKnown exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::AttributeNotRecognized(msg->getExceptionReasonW());
    } break;

    case Exception::Type::AttributeNotOwned: {
        Debug(D, pdExcept) << "Throwing e_AttributeNotOwned exception." << std::endl;
        throw rti1516::AttributeNotOwned(msg->getExceptionReasonW());
    } break;

    case Exception::Type::AttributeNotPublished: {
        Debug(D, pdExcept) << "Throwing e_AttributeNotPublished exception." << std::endl;
        throw rti1516::AttributeNotPublished(msg->getExceptionReasonW());
    } break;

    case Exception::Type::AttributeNotSubscribed: {
        Debug(D, pdExcept) << "Throwing e_AttributeNotSubscribed exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::ConcurrentAccessAttempted: {
        Debug(D, pdExcept) << "Throwing e_ConcurrentAccessAttempted exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::CouldNotDiscover: {
        Debug(D, pdExcept) << "Throwing e_CouldNotDiscover exception." << std::endl;
        throw rti1516::CouldNotDiscover(msg->getExceptionReasonW());
    } break;

    case Exception::Type::CouldNotOpenFED: {
        Debug(D, pdExcept) << "Throwing e_CouldNotOpenFED exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::CouldNotOpenFDD(msg->getExceptionReasonW());
    } break;

    case Exception::Type::CouldNotOpenRID: {
        Debug(D, pdExcept) << "Throwing e_CouldNotOpenRID exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::CouldNotRestore: {
        Debug(D, pdExcept) << "Throwing e_CouldNotRestore exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::CouldNotInitiateRestore(msg->getExceptionReasonW());
    } break;

    case Exception::Type::DeletePrivilegeNotHeld: {
        Debug(D, pdExcept) << "Throwing e_DeletePrivilegeNotHeld exception." << std::endl;
        throw rti1516::DeletePrivilegeNotHeld(msg->getExceptionReasonW());
    } break;

    case Exception::Type::ErrorReadingRID: {
        Debug(D, pdExcept) << "Throwing e_ErrorReadingRID exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::ErrorReadingFED: {
        Debug(D, pdExcept) << "Throwing e_ErrorReadingFED exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::ErrorReadingFDD(msg->getExceptionReasonW());
    } break;

    case Exception::Type::EventNotKnown: {
        Debug(D, pdExcept) << "Throwing e_EventNotKnown exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::FederateAlreadyPaused: {
        Debug(D, pdExcept) << "Throwing e_FederateAlreadyPaused exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::FederateAlreadyExecutionMember: {
        Debug(D, pdExcept) << "Throwing e_FederateAlreadyExecutionMember exception." << std::endl;
        throw rti1516::FederateAlreadyExecutionMember(msg->getExceptionReasonW());
    } break;

    case Exception::Type::FederateDoesNotExist: {
        Debug(D, pdExcept) << "Throwing e_FederateDoesNotExist exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::FederateInternalError: {
        Debug(D, pdExcept) << "Throwing e_FederateInternalError exception." << std::endl;
        throw rti1516::FederateInternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::FederateNameAlreadyInUse: {
        Debug(D, pdExcept) << "Throwing e_FederateNameAlreadyInUse exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::FederateNotExecutionMember: {
        Debug(D, pdExcept) << "Throwing e_FederateNotExecutionMember exception." << std::endl;
        throw rti1516::FederateNotExecutionMember(msg->getExceptionReasonW());
    } break;

    case Exception::Type::FederateNotPaused: {
        Debug(D, pdExcept) << "Throwing e_FederateNotPaused exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::FederateOwnsAttributes: {
        Debug(D, pdExcept) << "Throwing e_FederateOwnsAttributes exception." << std::endl;
        throw rti1516::FederateOwnsAttributes(msg->getExceptionReasonW());
    } break;

    case Exception::Type::FederatesCurrentlyJoined: {
        Debug(D, pdExcept) << "Throwing e_FederatesCurrentlyJoined exception." << std::endl;
        throw rti1516::FederatesCurrentlyJoined(msg->getExceptionReasonW());
    } break;

    case Exception::Type::FederateWasNotAskedToReleaseAttribute: {
        Debug(D, pdExcept) << "Throwing e_FederateWasNotAskedToReleaseAttribute exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::FederationAlreadyPaused: {
        Debug(D, pdExcept) << "Throwing e_FederationAlreadyPaused exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::FederationExecutionAlreadyExists: {
        Debug(D, pdExcept) << "Throwing e_FederationExecutionAlreadyExists excep." << std::endl;
        throw rti1516::FederationExecutionAlreadyExists(msg->getExceptionReasonW());
    } break;

    case Exception::Type::FederationExecutionDoesNotExist: {
        Debug(D, pdExcept) << "Throwing e_FederationExecutionDoesNotExist except." << std::endl;
        throw rti1516::FederationExecutionDoesNotExist(msg->getExceptionReasonW());
    } break;

    case Exception::Type::FederationNotPaused: {
        Debug(D, pdExcept) << "Throwing e_FederationNotPaused exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::FederationTimeAlreadyPassed: {
        Debug(D, pdExcept) << "Throwing e_FederationTimeAlreadyPassed exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::FederateNotPublishing: {
        Debug(D, pdExcept) << "Throwing e_FederateNotPublishing exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::FederateNotSubscribing: {
        Debug(D, pdExcept) << "Throwing e_FederateNotSubscribing exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::RegionNotKnown: {
        Debug(D, pdExcept) << "Throwing e_RegionNotKnown exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::IDsupplyExhausted: {
        Debug(D, pdExcept) << "Throwing e_IDsupplyExhausted exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::InteractionClassNotDefined: {
        Debug(D, pdExcept) << "Throwing e_InteractionClassNotDefined exception." << std::endl;
        throw rti1516::InteractionClassNotDefined(msg->getExceptionReasonW());
    } break;

    case Exception::Type::InteractionClassNotKnown: {
        Debug(D, pdExcept) << "Throwing e_InteractionClassNotKnown exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::InteractionClassNotRecognized(msg->getExceptionReasonW());
    } break;

    case Exception::Type::InteractionClassNotPublished: {
        Debug(D, pdExcept) << "Throwing e_InteractionClassNotPublished exception." << std::endl;
        throw rti1516::InteractionClassNotPublished(msg->getExceptionReasonW());
    } break;

    case Exception::Type::InteractionParameterNotDefined: {
        Debug(D, pdExcept) << "Throwing e_InteractionParameterNotDefined exception." << std::endl;
        throw rti1516::InteractionParameterNotDefined(msg->getExceptionReasonW());
    } break;

    case Exception::Type::InteractionParameterNotKnown: {
        Debug(D, pdExcept) << "Throwing e_InteractionParameterNotKnown exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::InteractionParameterNotRecognized(msg->getExceptionReasonW());
    } break;

    case Exception::Type::InvalidDivestitureCondition: {
        Debug(D, pdExcept) << "Throwing e_InvalidDivestitureCondition exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::InvalidExtents: {
        Debug(D, pdExcept) << "Throwing e_InvalidExtents exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::InvalidFederationTime: {
        Debug(D, pdExcept) << "Throwing e_InvalidFederationTime exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::InvalidFederationTimeDelta: {
        Debug(D, pdExcept) << "Throwing e_InvalidFederationTimeDelta exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::InvalidLookahead: {
        Debug(D, pdExcept) << "Throwing e_InvalidLookahead." << std::endl;
        throw rti1516::InvalidLookahead(msg->getExceptionReasonW());
    } break;

    case Exception::Type::InvalidObjectHandle: {
        Debug(D, pdExcept) << "Throwing e_InvalidObjectHandle exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::InvalidOrderingHandle: {
        Debug(D, pdExcept) << "Throwing e_InvalidOrderingHandle exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::InvalidOrderType(msg->getExceptionReasonW());
    } break;

    case Exception::Type::InvalidResignAction: {
        Debug(D, pdExcept) << "Throwing e_InvalidResignAction exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::InvalidRetractionHandle: {
        Debug(D, pdExcept) << "Throwing e_InvalidRetractionHandle exception." << std::endl;
        throw rti1516::InvalidRetractionHandle(msg->getExceptionReasonW());
    } break;

    case Exception::Type::InvalidRoutingSpace: {
        Debug(D, pdExcept) << "Throwing e_InvalidRoutingSpace exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::InvalidTransportationHandle: {
        Debug(D, pdExcept) << "Throwing e_InvalidTransportationHandle exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::InvalidTransportationType(msg->getExceptionReasonW());
    } break;

    case Exception::Type::MemoryExhausted: {
        Debug(D, pdExcept) << "Throwing e_MemoryExhausted exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::NameNotFound: {
        Debug(D, pdExcept) << "Throwing e_NameNotFound exception." << std::endl;
        throw rti1516::NameNotFound(msg->getExceptionReasonW());
    } break;

    case Exception::Type::NoPauseRequested: {
        Debug(D, pdExcept) << "Throwing e_NoPauseRequested exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::NoResumeRequested: {
        Debug(D, pdExcept) << "Throwing e_NoResumeRequested exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::ObjectClassNotDefined: {
        Debug(D, pdExcept) << "Throwing e_ObjectClassNotDefined exception." << std::endl;
        throw rti1516::ObjectClassNotDefined(msg->getExceptionReasonW());
    } break;

    case Exception::Type::ObjectClassNotKnown: {
        Debug(D, pdExcept) << "Throwing e_ObjectClassNotKnown exception." << std::endl;
        throw rti1516::ObjectClassNotKnown(msg->getExceptionReasonW());
    } break;

    case Exception::Type::ObjectClassNotPublished: {
        Debug(D, pdExcept) << "Throwing e_ObjectClassNotPublished exception." << std::endl;
        throw rti1516::ObjectClassNotPublished(msg->getExceptionReasonW());
    } break;

    case Exception::Type::ObjectClassNotSubscribed: {
        Debug(D, pdExcept) << "Throwing e_ObjectClassNotSubscribed exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::ObjectNotKnown: {
        Debug(D, pdExcept) << "Throwing e_ObjectNotKnown exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::ObjectInstanceNotKnown(msg->getExceptionReasonW());
    } break;

    case Exception::Type::ObjectAlreadyRegistered: {
        Debug(D, pdExcept) << "Throwing e_ObjectAlreadyRegistered exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::ObjectInstanceNameInUse(msg->getExceptionReasonW());
    } break;

    case Exception::Type::RestoreInProgress: {
        Debug(D, pdExcept) << "Throwing e_RestoreInProgress exception." << std::endl;
        throw rti1516::RestoreInProgress(msg->getExceptionReasonW());
    } break;

    case Exception::Type::RestoreNotRequested: {
        Debug(D, pdExcept) << "Throwing e_RestoreNotRequested exception." << std::endl;
        throw rti1516::RestoreNotRequested(msg->getExceptionReasonW());
    } break;

    case Exception::Type::RTIinternalError: {
        Debug(D, pdExcept) << "Throwing e_RTIinternalError exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::SpaceNotDefined: {
        Debug(D, pdExcept) << "Throwing e_SpaceNotDefined exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::SaveInProgress: {
        Debug(D, pdExcept) << "Throwing e_SaveInProgress exception." << std::endl;
        throw rti1516::SaveInProgress(msg->getExceptionReasonW());
    } break;

    case Exception::Type::SaveNotInitiated: {
        Debug(D, pdExcept) << "Throwing e_SaveNotInitiated exception." << std::endl;
        throw rti1516::SaveNotInitiated(msg->getExceptionReasonW());
    } break;

    case Exception::Type::SecurityError: {
        Debug(D, pdExcept) << "Throwing e_SecurityError exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::SpecifiedSaveLabelDoesNotExist: {
        Debug(D, pdExcept) << "Throwing e_SpecifiedSaveLabelDoesNotExist exception." << std::endl;
        throw rti1516::SpecifiedSaveLabelDoesNotExist(msg->getExceptionReasonW());
    } break;

    case Exception::Type::TimeAdvanceAlreadyInProgress: {
        Debug(D, pdExcept) << "Throwing e_TimeAdvanceAlreadyInProgress exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::TimeAdvanceWasNotInProgress: {
        Debug(D, pdExcept) << "Throwing e_TimeAdvanceWasNotInProgress exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::TooManyIDsRequested: {
        Debug(D, pdExcept) << "Throwing e_TooManyIDsRequested exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::UnableToPerformSave: {
        Debug(D, pdExcept) << "Throwing e_UnableToPerformSave exception." << std::endl;
        throw rti1516::UnableToPerformSave(msg->getExceptionReasonW());
    } break;

    case Exception::Type::UnimplementedService: {
        Debug(D, pdExcept) << "Throwing e_UnimplementedService exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::UnknownLabel: {
        Debug(D, pdExcept) << "Throwing e_UnknownLabel exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::ValueCountExceeded: {
        Debug(D, pdExcept) << "Throwing e_ValueCountExceeded exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::ValueLengthExceeded: {
        Debug(D, pdExcept) << "Throwing e_ValueLengthExceeded exception." << std::endl;
        // JvY: Changed name of exception
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;

    case Exception::Type::IllegalName: {
        Debug(D, pdExcept) << "Throwing e_IllegalName exception." << std::endl;
        throw rti1516::IllegalName(msg->getExceptionReasonW());
    } break;

    default: {
        Debug(D, pdExcept) << "Throwing unknown exception !" << std::endl;
        std::cout << "LibRTI: Receiving unknown RTI exception." << std::endl;
        throw rti1516::RTIinternalError(msg->getExceptionReasonW());
    } break;
    }
}

#define CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(service)                                                                  \
    catch (rti1516::Exception & e)                                                                                     \
    {                                                                                                                  \
        std::wstringstream msg;                                                                                        \
        msg << "Error in " << service << ": " << e.what();                                                             \
        throw rti1516::RTIinternalError(msg.str());                                                                    \
    }

void RTI1516ambPrivateRefs::callFederateAmbassador(Message* msg) throw(rti1516::RTIinternalError)
{
    switch (msg->getMessageType()) {
    case Message::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
        try {
            fed_amb->synchronizationPointRegistrationSucceeded(msg->getLabelW());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"synchronizationPointRegistrationSucceeded")
        break;

    case Message::SYNCHRONIZATION_POINT_REGISTRATION_FAILED:
        try {
            fed_amb->synchronizationPointRegistrationFailed(msg->getLabelW(),
                                                            rti1516::RTI_DETECTED_FAILURE_DURING_SYNCHRONIZATION);
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"synchronizationPointRegistrationFailed")
        break;

    case Message::ANNOUNCE_SYNCHRONIZATION_POINT:
        try {
            std::string tagString(msg->getTag());
            rti1516::VariableLengthData tagVarData(tagString.c_str(), tagString.length());
            fed_amb->announceSynchronizationPoint(msg->getLabelW(), tagVarData);
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"announceSynchronizationPoint")
        break;

    case Message::FEDERATION_SYNCHRONIZED:
        try {
            fed_amb->federationSynchronized(msg->getLabelW());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"federationSynchronized")
        break;

    case Message::INITIATE_FEDERATE_SAVE:
        try {
            fed_amb->initiateFederateSave((msg->getLabelW()));
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"initiateFederateSave")
        break;

    case Message::FEDERATION_SAVED:
        try {
            fed_amb->federationSaved();
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"federationSaved")
        break;

    case Message::REQUEST_FEDERATION_RESTORE_SUCCEEDED:
        try {
            fed_amb->requestFederationRestoreSucceeded((msg->getLabelW()));
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"requestFederationRestoreSucceeded")
        break;

    case Message::REQUEST_FEDERATION_RESTORE_FAILED:
        try {
            fed_amb->requestFederationRestoreFailed(msg->getLabelW());
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"requestFederationRestoreFailed")
        break;

    case Message::FEDERATION_RESTORE_BEGUN:
        try {
            fed_amb->federationRestoreBegun();
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"federationRestoreBegun")
        break;

    case Message::INITIATE_FEDERATE_RESTORE:
        try {
            M_Initiate_Federate_Restore* IFR = static_cast<M_Initiate_Federate_Restore*>(msg);
            rti1516::FederateHandle rti1516Handle
                = rti1516::FederateHandleFriend::createRTI1516Handle(IFR->getFederate());
            fed_amb->initiateFederateRestore((IFR->getLabelW()), rti1516Handle);
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"initiateFederateRestore")
        break;

    case Message::FEDERATION_RESTORED:
        try {
            fed_amb->federationRestored();
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"federationRestored")
        break;

    case Message::FEDERATION_NOT_RESTORED:
        try {
            // TODO Need to know reason
            fed_amb->federationNotRestored(rti1516::RTI_UNABLE_TO_RESTORE);
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"federationNotRestored")
        break;

    case Message::START_REGISTRATION_FOR_OBJECT_CLASS:
        try {
            rti1516::ObjectClassHandle rti1516Handle = rti1516::ObjectClassHandleFriend::createRTI1516Handle(
                static_cast<M_Start_Registration_For_Object_Class*>(msg)->getObjectClass());
            fed_amb->startRegistrationForObjectClass(rti1516Handle);
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"startRegistrationForObjectClass")
        break;

    case Message::STOP_REGISTRATION_FOR_OBJECT_CLASS:
        try {
            rti1516::ObjectClassHandle rti1516Handle = rti1516::ObjectClassHandleFriend::createRTI1516Handle(
                static_cast<M_Stop_Registration_For_Object_Class*>(msg)->getObjectClass());
            fed_amb->stopRegistrationForObjectClass(rti1516Handle);
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"stopRegistrationForObjectClass")
        break;

    case Message::TURN_INTERACTIONS_ON:
        try {
            rti1516::InteractionClassHandle rti1516Handle = rti1516::InteractionClassHandleFriend::createRTI1516Handle(
                static_cast<M_Turn_Interactions_On*>(msg)->getInteractionClass());
            fed_amb->turnInteractionsOn(rti1516Handle);
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"turnInteractionsOn")
        break;

    case Message::TURN_INTERACTIONS_OFF:
        try {
            rti1516::InteractionClassHandle rti1516Handle = rti1516::InteractionClassHandleFriend::createRTI1516Handle(
                static_cast<M_Turn_Interactions_Off*>(msg)->getInteractionClass());
            fed_amb->turnInteractionsOff(rti1516Handle);
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"turnInteractionsOff")
        break;

    case Message::DISCOVER_OBJECT_INSTANCE:
        try {
            M_Discover_Object_Instance* DOI = static_cast<M_Discover_Object_Instance*>(msg);
            rti1516::ObjectInstanceHandle objectInstance
                = rti1516::ObjectInstanceHandleFriend::createRTI1516Handle(DOI->getObject());
            rti1516::ObjectClassHandle objectClass
                = rti1516::ObjectClassHandleFriend::createRTI1516Handle(DOI->getObjectClass());
            std::string str = DOI->getObjectName();
            std::wstring str2(str.length(), L' '); // Make room for characters
            // Copy string to wstring.
            std::copy(str.begin(), str.end(), str2.begin());

            fed_amb->discoverObjectInstance(objectInstance, objectClass, str2);
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"discoverObjectInstance")
        break;

    case Message::REFLECT_ATTRIBUTE_VALUES:
        try {
            M_Reflect_Attribute_Values* RAV = static_cast<M_Reflect_Attribute_Values*>(msg);
            Debug(G, pdGendoc) << "          tick_kernel call to reflectAttributeValues" << std::endl;

            rti1516::ObjectInstanceHandle instance
                = rti1516::ObjectInstanceHandleFriend::createRTI1516Handle(RAV->getObject());

            rti1516::AttributeHandleValueMap* attributes = getAHVMFromRequest(RAV);

            std::string tagString(msg->getTag());
            rti1516::VariableLengthData tagVarData(tagString.c_str(), tagString.length());

            if (msg->isDated()) {
                uint64_t sn = RAV->getEventRetraction().getSN();
                certi::FederateHandle certiHandle = RAV->getEventRetraction().getSendingFederate();
                rti1516::MessageRetractionHandle event
                    = rti1516::MessageRetractionHandleFriend::createRTI1516Handle(certiHandle, sn);

                std::unique_ptr<rti1516::LogicalTime> fedTime(new RTI1516fedTime(msg->getDate().getTime()));

                fed_amb->reflectAttributeValues(instance, //ObjectInstanceHandle
                                                *attributes, //AttributeHandleValueMap &
                                                tagVarData, //VariableLengthData &
                                                rti1516::TIMESTAMP, //OrderType (send)
                                                rti1516::RELIABLE, //TransportationType
                                                *fedTime, //LogicalTime &
                                                rti1516::RECEIVE, //OrderType (receive)
                                                event //MessageRetractionHandle
                                                );
            }
            else {
                fed_amb->reflectAttributeValues(instance, *attributes, tagVarData, rti1516::RECEIVE, rti1516::RELIABLE);
            }
            delete attributes;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"reflectAttributeValues")
        break;

    case Message::RECEIVE_INTERACTION:
        try {
            M_Receive_Interaction* RI = static_cast<M_Receive_Interaction*>(msg);
            rti1516::ParameterHandleValueMap* parameters = getPHVMFromRequest(RI);

            rti1516::InteractionClassHandle interactionHandle
                = rti1516::InteractionClassHandleFriend::createRTI1516Handle(RI->getInteractionClass());

            std::string tagString(msg->getTag());
            rti1516::VariableLengthData tagVarData(tagString.c_str(), tagString.length());

            if (msg->isDated()) {
                uint64_t sn = RI->getEventRetraction().getSN();
                certi::FederateHandle certiHandle = RI->getEventRetraction().getSendingFederate();
                rti1516::MessageRetractionHandle event
                    = rti1516::MessageRetractionHandleFriend::createRTI1516Handle(certiHandle, sn);

                std::unique_ptr<rti1516::LogicalTime> fedTime(new RTI1516fedTime(msg->getDate().getTime()));

                fed_amb->receiveInteraction(interactionHandle, // InteractionClassHandle
                                            *parameters, // ParameterHandleValueMap &
                                            tagVarData, // VariableLengthData &
                                            rti1516::TIMESTAMP, //OrderType (send)
                                            rti1516::RELIABLE, //TransportationType
                                            *fedTime, //LogicalTime &
                                            rti1516::RECEIVE, //OrderType (receive)
                                            event //MessageRetractionHandle
                                            );
            }
            else {
                fed_amb->receiveInteraction(
                    interactionHandle, *parameters, tagVarData, rti1516::RECEIVE, rti1516::RELIABLE);
            }
            delete parameters;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"receiveInteraction")
        break;

    case Message::REMOVE_OBJECT_INSTANCE:
        try {
            M_Remove_Object_Instance* ROI = static_cast<M_Remove_Object_Instance*>(msg);

            rti1516::ObjectInstanceHandle instance
                = rti1516::ObjectInstanceHandleFriend::createRTI1516Handle(ROI->getObject());

            std::string tagString(msg->getTag());
            rti1516::VariableLengthData tagVarData(tagString.c_str(), tagString.length());

            if (msg->isDated()) {
                uint64_t sn = ROI->getEventRetraction().getSN();
                certi::FederateHandle certiHandle = ROI->getEventRetraction().getSendingFederate();
                rti1516::MessageRetractionHandle event
                    = rti1516::MessageRetractionHandleFriend::createRTI1516Handle(certiHandle, sn);

                std::unique_ptr<rti1516::LogicalTime> fedTime(new RTI1516fedTime(msg->getDate().getTime()));

                fed_amb->removeObjectInstance(
                    instance, tagVarData, rti1516::TIMESTAMP, *fedTime, rti1516::RECEIVE, event);
            }
            else {
                fed_amb->removeObjectInstance(instance, tagVarData, rti1516::RECEIVE);
            }
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"removeObjectInstance")
        break;

    case Message::PROVIDE_ATTRIBUTE_VALUE_UPDATE:
        try {
            M_Provide_Attribute_Value_Update* PAVU = static_cast<M_Provide_Attribute_Value_Update*>(msg);

            rti1516::AttributeHandleSet* attributeSet = getAHSFromRequest(PAVU);

            rti1516::ObjectInstanceHandle instance
                = rti1516::ObjectInstanceHandleFriend::createRTI1516Handle(PAVU->getObject());

            std::string tag = PAVU->getTag();
            rti1516::VariableLengthData tagVarData(tag.c_str(), tag.size());

            fed_amb->provideAttributeValueUpdate(instance, *attributeSet, tagVarData);
            delete attributeSet;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"provideAttributeValueUpdate")
        break;

    case Message::REQUEST_RETRACTION: {
    } break;

    case Message::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
        try {
            M_Request_Attribute_Ownership_Assumption* RAOA
                = static_cast<M_Request_Attribute_Ownership_Assumption*>(msg);
            rti1516::AttributeHandleSet* attributeSet = getAHSFromRequest(RAOA);

            rti1516::ObjectInstanceHandle instance
                = rti1516::ObjectInstanceHandleFriend::createRTI1516Handle(RAOA->getObject());

            std::string tag = RAOA->getTag();
            rti1516::VariableLengthData tagVarData(tag.c_str(), tag.size());

            fed_amb->requestAttributeOwnershipAssumption(instance, *attributeSet, tagVarData);
            delete attributeSet;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"requestAttributeOwnershipAssumption")
        break;

    case Message::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
        try {
            M_Request_Attribute_Ownership_Release* RAOR = static_cast<M_Request_Attribute_Ownership_Release*>(msg);
            rti1516::AttributeHandleSet* attributeSet = getAHSFromRequest(RAOR);

            rti1516::ObjectInstanceHandle instance
                = rti1516::ObjectInstanceHandleFriend::createRTI1516Handle(RAOR->getObject());

            std::string tag = RAOR->getTag();
            rti1516::VariableLengthData tagVarData(tag.c_str(), tag.size());

            fed_amb->requestAttributeOwnershipRelease(instance, *attributeSet, tagVarData);

            delete attributeSet;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"requestAttributeOwnershipRelease")
        break;

    case Message::ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
        try {
            M_Attribute_Ownership_Unavailable* AOU = static_cast<M_Attribute_Ownership_Unavailable*>(msg);

            rti1516::AttributeHandleSet* attributeSet = getAHSFromRequest(AOU);

            rti1516::ObjectInstanceHandle instance
                = rti1516::ObjectInstanceHandleFriend::createRTI1516Handle(AOU->getObject());

            fed_amb->attributeOwnershipUnavailable(instance, *attributeSet);

            delete attributeSet;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"attributeOwnershipUnavailable")
        break;

    case Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
        try {
            M_Attribute_Ownership_Acquisition_Notification* AOAN
                = static_cast<M_Attribute_Ownership_Acquisition_Notification*>(msg);

            rti1516::AttributeHandleSet* attributeSet = getAHSFromRequest(AOAN);

            rti1516::ObjectInstanceHandle instance
                = rti1516::ObjectInstanceHandleFriend::createRTI1516Handle(AOAN->getObject());

            std::string tag = AOAN->getTag();
            rti1516::VariableLengthData tagVarData(tag.c_str(), tag.size());

            fed_amb->attributeOwnershipAcquisitionNotification(instance, *attributeSet, tagVarData);

            delete attributeSet;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"attributeOwnershipAcquisitionNotification")
        break;

    case Message::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
        try {
            M_Attribute_Ownership_Divestiture_Notification* AODN
                = static_cast<M_Attribute_Ownership_Divestiture_Notification*>(msg);

            rti1516::AttributeHandleSet* attributeSet = getAHSFromRequest(AODN);

            rti1516::ObjectInstanceHandle instance
                = rti1516::ObjectInstanceHandleFriend::createRTI1516Handle(AODN->getObject());

            // requestDivestitureConfirmation = 7.5, en zou ontbreken in 1.3. JRE: klopt dit wel?
            // Changed function name! Originally: attributeOwnershipDivestitureNotification
            fed_amb->requestDivestitureConfirmation(instance, *attributeSet);

            delete attributeSet;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"attributeOwnershipDivestitureNotification")
        break;

    case Message::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
        try {
            M_Confirm_Attribute_Ownership_Acquisition_Cancellation* CAOAC
                = static_cast<M_Confirm_Attribute_Ownership_Acquisition_Cancellation*>(msg);

            rti1516::AttributeHandleSet* attributeSet = getAHSFromRequest(CAOAC);

            rti1516::ObjectInstanceHandle instance
                = rti1516::ObjectInstanceHandleFriend::createRTI1516Handle(CAOAC->getObject());

            fed_amb->confirmAttributeOwnershipAcquisitionCancellation(instance, *attributeSet);

            delete attributeSet;
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"confirmAttributeOwnershipAcquisitionCancellation")
        break;

    case Message::INFORM_ATTRIBUTE_OWNERSHIP:
        try {
            M_Inform_Attribute_Ownership* IAO = static_cast<M_Inform_Attribute_Ownership*>(msg);
            rti1516::ObjectInstanceHandle instance
                = rti1516::ObjectInstanceHandleFriend::createRTI1516Handle(IAO->getObject());
            rti1516::AttributeHandle attribute
                = rti1516::AttributeHandleFriend::createRTI1516Handle(IAO->getAttribute());
            rti1516::FederateHandle federate = rti1516::FederateHandleFriend::createRTI1516Handle(IAO->getFederate());

            fed_amb->informAttributeOwnership(instance, attribute, federate);
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"informAttributeOwnership")
        break;

    case Message::ATTRIBUTE_IS_NOT_OWNED:
        try {
            M_Attribute_Is_Not_Owned* AINO = static_cast<M_Attribute_Is_Not_Owned*>(msg);
            rti1516::ObjectInstanceHandle instance
                = rti1516::ObjectInstanceHandleFriend::createRTI1516Handle(AINO->getObject());
            rti1516::AttributeHandle attribute
                = rti1516::AttributeHandleFriend::createRTI1516Handle(AINO->getAttribute());

            fed_amb->attributeIsNotOwned(instance, attribute);
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"attributeIsNotOwned")
        break;

    case Message::TIME_ADVANCE_GRANT:
        try {
            fed_amb->timeAdvanceGrant(RTI1516fedTime(msg->getDate().getTime()));
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"timeAdvanceGrant")
        break;

    case Message::TIME_REGULATION_ENABLED:
        try {
            std::unique_ptr<rti1516::LogicalTime> fedTime(new RTI1516fedTime(msg->getDate().getTime()));
            fed_amb->timeRegulationEnabled(*fedTime);
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"timeRegulationEnabled")
        break;

    case Message::TIME_CONSTRAINED_ENABLED:
        try {
            std::unique_ptr<rti1516::LogicalTime> fedTime(new RTI1516fedTime(msg->getDate().getTime()));
            fed_amb->timeConstrainedEnabled(*fedTime);
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"timeConstrainedEnabled")
        break;

    case Message::RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED:
        try {
            M_Reserve_Object_Instance_Name_Succeeded* ROINS
                = static_cast<M_Reserve_Object_Instance_Name_Succeeded*>(msg);
            std::string objName_s(ROINS->getObjectName());
            std::wstring objName_ws(objName_s.begin(), objName_s.end());

            fed_amb->objectInstanceNameReservationSucceeded(objName_ws);
        }
        CATCH_FEDERATE_AMBASSADOR_EXCEPTIONS(L"reserveObjectInstanceNameSucceeded")
        break;

    case Message::RESERVE_OBJECT_INSTANCE_NAME_FAILED:
        try {
            M_Reserve_Object_Instance_Name_Failed* ROINS = static_cast<M_Reserve_Object_Instance_Name_Failed*>(msg);
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

// $Id: RTIambPrivateRefs.cpp,v 1.1 2014/03/03 16:41:51 erk Exp $
