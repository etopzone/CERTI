// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// ----------------------------------------------------------------------------

#ifndef _CERTI_EXCEPTION_HH
#define _CERTI_EXCEPTION_HH

#include "certi.hh"
#include <string>

namespace certi {

typedef enum {
    e_NO_EXCEPTION = 0,
    e_ArrayIndexOutOfBounds,
    e_AsynchronousDeliveryAlreadyEnabled,
    e_AsynchronousDeliveryAlreadyDisabled,
    e_AttributeAlreadyOwned,
    e_AttributeAlreadyBeingAcquired,
    e_AttributeAlreadyBeingDivested,
    e_AttributeAcquisitionWasNotRequested,
    e_AttributeDivestitureWasNotRequested,
    e_AttributeNotDefined,
    e_AttributeNotKnown,
    e_AttributeNotOwned,
    e_AttributeNotPublished,
    e_AttributeNotSubscribed,
    e_ConcurrentAccessAttempted,
    e_CouldNotDiscover,
    e_CouldNotOpenRID,
    e_CouldNotOpenFED,
    e_CouldNotRestore,
    e_DeletePrivilegeNotHeld,
    e_ErrorReadingRID,
    e_ErrorReadingFED,
    e_EventNotKnown,
    e_FederateAlreadyPaused,
    e_FederateAlreadyExecutionMember,
    e_FederateDoesNotExist,
    e_FederateInternalError,
    e_FederateNameAlreadyInUse,
    e_FederateNotExecutionMember,
    e_FederateNotPaused,
    e_FederateNotPublishing,
    e_FederateNotSubscribing,
    e_FederateOwnsAttributes,
    e_FederatesCurrentlyJoined,
    e_FederateWasNotAskedToReleaseAttribute,
    e_FederationAlreadyPaused,
    e_FederationExecutionAlreadyExists,
    e_FederationExecutionDoesNotExist,
    e_FederationNotPaused,
    e_FederationTimeAlreadyPassed,
    e_RegionNotKnown,
    e_IDsupplyExhausted,
    e_InteractionClassNotDefined,
    e_InteractionClassNotKnown,
    e_InteractionClassNotPublished,
    e_InteractionParameterNotDefined,
    e_InteractionParameterNotKnown,
    e_InvalidDivestitureCondition,
    e_InvalidExtents,
    e_InvalidFederationTime,
    e_InvalidFederationTimeDelta,
    e_InvalidObjectHandle,
    e_InvalidResignAction,
    e_InvalidRetractionHandle,
    e_InvalidRoutingSpace,
    e_MemoryExhausted,
    e_NameNotFound,
    e_NoPauseRequested,
    e_NoResumeRequested,
    e_ObjectClassNotDefined,
    e_ObjectClassNotKnown,
    e_ObjectClassNotPublished,
    e_ObjectClassNotSubscribed,
    e_ObjectNotKnown,
    e_ObjectAlreadyRegistered,
    e_RestoreInProgress,
    e_RestoreNotRequested,
    e_RTICannotRestore,
    e_RTIinternalError,
    e_SpaceNotDefined,
    e_SaveInProgress,
    e_SaveNotInitiated,
    e_SecurityError,
    e_SocketNotConnected,
    e_MessageNotSent,
    e_MessageNotReceived,
    e_SocketNotClosed,
    e_RingBufferNotCreated,
    e_RingBufferNotClosed,
    e_RingBufferNotDeleted,
    e_RingBufferNotAttached,
    e_MessageTooLong,
    e_BufferFull,
    e_BufferEmpty,
    e_SocketSHMNotCreated,
    e_SocketSHMNotOpen,
    e_SocketSHMNotDeleted,
    e_SpecifiedSaveLabelDoesNotExist,
    e_TimeAdvanceAlreadyInProgress,
    e_TimeAdvanceWasNotInProgress,
    e_TooManyIDsRequested,
    e_UnableToPerformSave,
    e_UnimplementedService,
    e_UnknownLabel,
    e_ValueCountExceeded,
    e_ValueLengthExceeded,
    e_AttributeAcquisitionWasNotCanceled,
    e_DimensionNotDefined,
    e_EnableTimeConstrainedPending,
    e_EnableTimeConstrainedWasNotPending,
    e_EnableTimeRegulationPending,
    e_EnableTimeRegulationWasNotPending,
    e_FederateLoggingServiceCalls,
    e_HandleValuePairMaximumExceeded,
    e_InteractionClassNotSubscribed,
    e_InvalidHandleValuePairSetContext,
    e_InvalidLookahead,
    e_InvalidOrderingHandle,
    e_InvalidRegionContext,
    e_InvalidTransportationHandle,
    e_OwnershipAcquisitionPending,
    e_RegionInUse,
    e_SynchronizationPointLabelWasNotAnnounced,
    e_TimeConstrainedAlreadyEnabled,
    e_TimeConstrainedWasNotEnabled,
    e_TimeRegulationAlreadyEnabled,
    e_TimeRegulationWasNotEnabled,
    e_NetworkError,
    e_NetworkSignal,
	e_IllegalName //1516 only
} TypeException ;

class CERTI_EXPORT Exception
{
public:
    const std::string _reason;
    const char *_name;

    Exception(const std::string &reason)
        : _reason(reason), _name("Exception") { }

    const std::wstring wreason() const {
    	std::wstring wval(_reason.begin(),_reason.end());
    	return wval;
    }
    virtual ~Exception() { }
    const std::string displayMe() const;
    virtual long getType() const = 0;
};

#define CERTI_EXCEPTION(A) \
    class CERTI_EXPORT A : public Exception { \
    public: \
        static long _type; \
        A(const std::string &reason) : Exception(reason) { _name = #A; this->displayMe();} \
        long getType() const { return _type; } \
};

// RTI Exceptions for use inside libCERTI
CERTI_EXCEPTION(ArrayIndexOutOfBounds)
CERTI_EXCEPTION(AsynchronousDeliveryAlreadyDisabled)
CERTI_EXCEPTION(AsynchronousDeliveryAlreadyEnabled)
CERTI_EXCEPTION(AttributeAcquisitionWasNotRequested)
CERTI_EXCEPTION(AttributeAcquisitionWasNotCanceled)
CERTI_EXCEPTION(AttributeAlreadyBeingAcquired)
CERTI_EXCEPTION(AttributeAlreadyBeingDivested)
CERTI_EXCEPTION(AttributeAlreadyOwned)
CERTI_EXCEPTION(AttributeDivestitureWasNotRequested)
CERTI_EXCEPTION(AttributeNotDefined)
CERTI_EXCEPTION(AttributeNotKnown)
CERTI_EXCEPTION(AttributeNotOwned)
CERTI_EXCEPTION(AttributeNotPublished)
CERTI_EXCEPTION(ConcurrentAccessAttempted)
CERTI_EXCEPTION(CouldNotDiscover)
CERTI_EXCEPTION(CouldNotOpenFED)
CERTI_EXCEPTION(CouldNotRestore)
CERTI_EXCEPTION(DeletePrivilegeNotHeld)
CERTI_EXCEPTION(DimensionNotDefined)
CERTI_EXCEPTION(EnableTimeConstrainedPending)
CERTI_EXCEPTION(EnableTimeConstrainedWasNotPending)
CERTI_EXCEPTION(EnableTimeRegulationPending)
CERTI_EXCEPTION(EnableTimeRegulationWasNotPending)
CERTI_EXCEPTION(ErrorReadingFED)
CERTI_EXCEPTION(EventNotKnown)
CERTI_EXCEPTION(FederateAlreadyExecutionMember)
CERTI_EXCEPTION(FederateInternalError)
CERTI_EXCEPTION(FederateLoggingServiceCalls)
CERTI_EXCEPTION(FederateNotExecutionMember)
CERTI_EXCEPTION(FederateOwnsAttributes)
CERTI_EXCEPTION(FederateWasNotAskedToReleaseAttribute)
CERTI_EXCEPTION(FederatesCurrentlyJoined)
CERTI_EXCEPTION(FederationExecutionAlreadyExists)
CERTI_EXCEPTION(FederationExecutionDoesNotExist)
CERTI_EXCEPTION(FederationTimeAlreadyPassed)
CERTI_EXCEPTION(HandleValuePairMaximumExceeded)
CERTI_EXCEPTION(InteractionClassNotDefined)
CERTI_EXCEPTION(InteractionClassNotKnown)
CERTI_EXCEPTION(InteractionClassNotPublished)
CERTI_EXCEPTION(InteractionClassNotSubscribed)
CERTI_EXCEPTION(InteractionParameterNotDefined)
CERTI_EXCEPTION(InteractionParameterNotKnown)
CERTI_EXCEPTION(InvalidExtents)
CERTI_EXCEPTION(InvalidFederationTime)
CERTI_EXCEPTION(InvalidHandleValuePairSetContext)
CERTI_EXCEPTION(InvalidLookahead)
CERTI_EXCEPTION(InvalidOrderingHandle)
CERTI_EXCEPTION(InvalidRegionContext)
CERTI_EXCEPTION(InvalidResignAction)
CERTI_EXCEPTION(InvalidRetractionHandle)
CERTI_EXCEPTION(InvalidTransportationHandle)
CERTI_EXCEPTION(MemoryExhausted)
CERTI_EXCEPTION(NameNotFound)
CERTI_EXCEPTION(ObjectClassNotDefined)
CERTI_EXCEPTION(ObjectClassNotKnown)
CERTI_EXCEPTION(ObjectClassNotPublished)
CERTI_EXCEPTION(ObjectClassNotSubscribed)
CERTI_EXCEPTION(ObjectNotKnown)
CERTI_EXCEPTION(ObjectAlreadyRegistered)
CERTI_EXCEPTION(OwnershipAcquisitionPending)
CERTI_EXCEPTION(RegionInUse)
CERTI_EXCEPTION(RegionNotKnown)
CERTI_EXCEPTION(RestoreInProgress)
CERTI_EXCEPTION(RestoreNotRequested)
CERTI_EXCEPTION(RTIinternalError)
CERTI_EXCEPTION(SpaceNotDefined)
CERTI_EXCEPTION(SaveInProgress)
CERTI_EXCEPTION(SaveNotInitiated)
CERTI_EXCEPTION(SpecifiedSaveLabelDoesNotExist)
CERTI_EXCEPTION(SynchronizationPointLabelWasNotAnnounced)
CERTI_EXCEPTION(TimeAdvanceAlreadyInProgress)
CERTI_EXCEPTION(TimeAdvanceWasNotInProgress)
CERTI_EXCEPTION(TimeConstrainedAlreadyEnabled)
CERTI_EXCEPTION(TimeConstrainedWasNotEnabled)
CERTI_EXCEPTION(TimeRegulationAlreadyEnabled)
CERTI_EXCEPTION(TimeRegulationWasNotEnabled)
CERTI_EXCEPTION(UnableToPerformSave)
CERTI_EXCEPTION(ValueCountExceeded)
CERTI_EXCEPTION(ValueLengthExceeded)

// RTI1516 Exceptions for use inside libCERTI
CERTI_EXCEPTION(IllegalName)

// Additional CERTI exceptions
CERTI_EXCEPTION(FederateNotPublishing)
CERTI_EXCEPTION(FederateNotSubscribing)
CERTI_EXCEPTION(InvalidObjectHandle)
CERTI_EXCEPTION(SecurityError)
CERTI_EXCEPTION(CouldNotOpenRID)
CERTI_EXCEPTION(ErrorReadingRID)
CERTI_EXCEPTION(FederationAlreadyPaused)
CERTI_EXCEPTION(FederationNotPaused)
CERTI_EXCEPTION(AttributeNotSubscribed)
CERTI_EXCEPTION(FederateAlreadyPaused)
CERTI_EXCEPTION(FederateDoesNotExist)
CERTI_EXCEPTION(FederateNameAlreadyInUse)
CERTI_EXCEPTION(FederateNotPaused)
CERTI_EXCEPTION(IDsupplyExhausted)
CERTI_EXCEPTION(InvalidDivestitureCondition)
CERTI_EXCEPTION(InvalidFederationTimeDelta)
CERTI_EXCEPTION(InvalidRoutingSpace)
CERTI_EXCEPTION(NoPauseRequested)
CERTI_EXCEPTION(NoResumeRequested)
CERTI_EXCEPTION(TooManyIDsRequested)
CERTI_EXCEPTION(UnimplementedService)
CERTI_EXCEPTION(UnknownLabel)
CERTI_EXCEPTION(NetworkSignal)
CERTI_EXCEPTION(NetworkError)
CERTI_EXCEPTION(SocketNotConnected)
CERTI_EXCEPTION(MessageNotSent)
CERTI_EXCEPTION(MessageNotReceived)
CERTI_EXCEPTION(SocketNotClosed)
CERTI_EXCEPTION(RingBufferNotCreated)
CERTI_EXCEPTION(RingBufferNotClosed)
CERTI_EXCEPTION(RingBufferNotDeleted)
CERTI_EXCEPTION(RingBufferNotAttached)
CERTI_EXCEPTION(MessageTooLong)
CERTI_EXCEPTION(BufferEmpty)
CERTI_EXCEPTION(BufferFull)
CERTI_EXCEPTION(SocketSHMNotCreated)
CERTI_EXCEPTION(SocketSHMNotOpen)
CERTI_EXCEPTION(SocketSHMNotDeleted)

} // namespace certi

#endif // _CERTI_EXCEPTION_HH

// $Id: Exception.hh,v 3.13 2013/09/24 14:27:57 erk Exp $
