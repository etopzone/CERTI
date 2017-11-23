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

#include <include/certi.hh>
#include <string>

namespace certi {

class CERTI_EXPORT Exception {
public:
    enum class Type : unsigned char {
        NO_EXCEPTION = 0,
        ArrayIndexOutOfBounds,
        AsynchronousDeliveryAlreadyEnabled,
        AsynchronousDeliveryAlreadyDisabled,
        AttributeAlreadyOwned,
        AttributeAlreadyBeingAcquired,
        AttributeAlreadyBeingDivested,
        AttributeAcquisitionWasNotRequested,
        AttributeDivestitureWasNotRequested,
        AttributeNotDefined,
        AttributeNotKnown,
        AttributeNotOwned,
        AttributeNotPublished,
        AttributeNotSubscribed,
        ConcurrentAccessAttempted,
        CouldNotDiscover,
        CouldNotOpenRID,
        CouldNotOpenFED,
        CouldNotRestore,
        DeletePrivilegeNotHeld,
        ErrorReadingRID,
        ErrorReadingFED,
        EventNotKnown,
        FederateAlreadyPaused,
        FederateAlreadyExecutionMember,
        FederateDoesNotExist,
        FederateInternalError,
        FederateNameAlreadyInUse,
        FederateNotExecutionMember,
        FederateNotPaused,
        FederateNotPublishing,
        FederateNotSubscribing,
        FederateOwnsAttributes,
        FederatesCurrentlyJoined,
        FederateWasNotAskedToReleaseAttribute,
        FederationAlreadyPaused,
        FederationExecutionAlreadyExists,
        FederationExecutionDoesNotExist,
        FederationNotPaused,
        FederationTimeAlreadyPassed,
        RegionNotKnown,
        IDsupplyExhausted,
        InteractionClassNotDefined,
        InteractionClassNotKnown,
        InteractionClassNotPublished,
        InteractionParameterNotDefined,
        InteractionParameterNotKnown,
        InvalidDivestitureCondition,
        InvalidExtents,
        InvalidFederationTime,
        InvalidFederationTimeDelta,
        InvalidObjectHandle,
        InvalidResignAction,
        InvalidRetractionHandle,
        InvalidRoutingSpace,
        MemoryExhausted,
        NameNotFound,
        NoPauseRequested,
        NoResumeRequested,
        ObjectClassNotDefined,
        ObjectClassNotKnown,
        ObjectClassNotPublished,
        ObjectClassNotSubscribed,
        ObjectNotKnown,
        ObjectAlreadyRegistered,
        RestoreInProgress,
        RestoreNotRequested,
        RTICannotRestore,
        RTIinternalError,
        SpaceNotDefined,
        SaveInProgress,
        SaveNotInitiated,
        SecurityError,
        SocketNotConnected,
        MessageNotSent,
        MessageNotReceived,
        SocketNotClosed,
        RingBufferNotCreated,
        RingBufferNotClosed,
        RingBufferNotDeleted,
        RingBufferNotAttached,
        MessageTooLong,
        BufferFull,
        BufferEmpty,
        SocketSHMNotCreated,
        SocketSHMNotOpen,
        SocketSHMNotDeleted,
        SpecifiedSaveLabelDoesNotExist,
        TimeAdvanceAlreadyInProgress,
        TimeAdvanceWasNotInProgress,
        TooManyIDsRequested,
        UnableToPerformSave,
        UnimplementedService,
        UnknownLabel,
        ValueCountExceeded,
        ValueLengthExceeded,
        AttributeAcquisitionWasNotCanceled,
        DimensionNotDefined,
        EnableTimeConstrainedPending,
        EnableTimeConstrainedWasNotPending,
        EnableTimeRegulationPending,
        EnableTimeRegulationWasNotPending,
        FederateLoggingServiceCalls,
        HandleValuePairMaximumExceeded,
        InteractionClassNotSubscribed,
        InvalidHandleValuePairSetContext,
        InvalidLookahead,
        InvalidOrderingHandle,
        InvalidRegionContext,
        InvalidTransportationHandle,
        OwnershipAcquisitionPending,
        RegionInUse,
        SynchronizationPointLabelWasNotAnnounced,
        TimeConstrainedAlreadyEnabled,
        TimeConstrainedWasNotEnabled,
        TimeRegulationAlreadyEnabled,
        TimeRegulationWasNotEnabled,
        NetworkError,
        NetworkSignal,
        IllegalName, //1516 only
        CustomException
    };

    virtual ~Exception() = default;

    virtual std::string name() const
    {
        return _name;
    }

    std::string reason() const
    {
        return _reason;
    }

    const std::wstring wreason() const
    {
        return {begin(_reason), end(_reason)};
    }

    virtual Type type() const = 0;

    const std::string displayMe() const;

protected:
    /// Only for subclasses
    Exception(const std::string& name, const std::string& reason) : _name{name}, _reason{reason}
    {
        displayMe();
    }

private:
    const std::string _name;
    const std::string _reason;
};

#define CERTI_EXCEPTION(A)                                                                                             \
    class CERTI_EXPORT A : public Exception {                                                                          \
    public:                                                                                                            \
        explicit A(const std::string& reason) : Exception{#A, reason}                                                  \
        {                                                                                                              \
        }                                                                                                              \
                                                                                                                       \
        virtual Type type() const override                                                                             \
        {                                                                                                              \
            return Type::A;                                                                                            \
        }                                                                                                              \
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

// Additional CERTI exceptions
CERTI_EXCEPTION(FederateNotPublishing)
CERTI_EXCEPTION(FederateNotSubscribing)
CERTI_EXCEPTION(InvalidObjectHandle)
CERTI_EXCEPTION(CouldNotOpenRID)
CERTI_EXCEPTION(ErrorReadingRID)
CERTI_EXCEPTION(AttributeNotSubscribed)
CERTI_EXCEPTION(FederationAlreadyPaused)
CERTI_EXCEPTION(FederationNotPaused)
CERTI_EXCEPTION(SecurityError)
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
CERTI_EXCEPTION(NetworkError)
CERTI_EXCEPTION(NetworkSignal)
CERTI_EXCEPTION(SocketNotConnected)
CERTI_EXCEPTION(MessageNotSent)
CERTI_EXCEPTION(MessageNotReceived)
CERTI_EXCEPTION(SocketNotClosed)
CERTI_EXCEPTION(RingBufferNotCreated)
CERTI_EXCEPTION(RingBufferNotClosed)
CERTI_EXCEPTION(RingBufferNotDeleted)
CERTI_EXCEPTION(RingBufferNotAttached)
CERTI_EXCEPTION(MessageTooLong)
CERTI_EXCEPTION(BufferFull)
CERTI_EXCEPTION(BufferEmpty)
CERTI_EXCEPTION(SocketSHMNotCreated)
CERTI_EXCEPTION(SocketSHMNotOpen)
CERTI_EXCEPTION(SocketSHMNotDeleted)

// RTI1516 Exceptions for use inside libCERTI
CERTI_EXCEPTION(IllegalName)

} // namespace certi

#endif // _CERTI_EXCEPTION_HH
