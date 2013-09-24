// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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

#include "Exception.hh"
#include "certi.hh"
#include "PrettyDebug.hh"
#include <sstream>

// static members for HLA Exceptions
long certi::ArrayIndexOutOfBounds::_type = certi::e_ArrayIndexOutOfBounds ;
long certi::AsynchronousDeliveryAlreadyDisabled::_type = certi::e_AsynchronousDeliveryAlreadyDisabled ;
long certi::AsynchronousDeliveryAlreadyEnabled::_type = certi::e_AsynchronousDeliveryAlreadyEnabled ;
long certi::AttributeAcquisitionWasNotRequested::_type = certi::e_AttributeAcquisitionWasNotRequested ;
long certi::AttributeAcquisitionWasNotCanceled::_type = certi::e_AttributeAcquisitionWasNotCanceled ;
long certi::AttributeAlreadyBeingAcquired::_type = certi::e_AttributeAlreadyBeingAcquired ;
long certi::AttributeAlreadyBeingDivested::_type = certi::e_AttributeAlreadyBeingDivested ;
long certi::AttributeAlreadyOwned::_type = certi::e_AttributeAlreadyOwned ;
long certi::AttributeDivestitureWasNotRequested::_type = certi::e_AttributeDivestitureWasNotRequested ;
long certi::AttributeNotDefined::_type = certi::e_AttributeNotDefined ;
long certi::AttributeNotKnown::_type = certi::e_AttributeNotKnown ;
long certi::AttributeNotOwned::_type = certi::e_AttributeNotOwned ;
long certi::AttributeNotPublished::_type = certi::e_AttributeNotPublished ;
long certi::ConcurrentAccessAttempted::_type = certi::e_ConcurrentAccessAttempted ;
long certi::CouldNotDiscover::_type = certi::e_CouldNotDiscover ;
long certi::CouldNotOpenFED::_type = certi::e_CouldNotOpenFED ;
long certi::CouldNotRestore::_type = certi::e_CouldNotRestore ;
long certi::DeletePrivilegeNotHeld::_type = certi::e_DeletePrivilegeNotHeld ;
long certi::DimensionNotDefined::_type = certi::e_DimensionNotDefined ;
long certi::EnableTimeConstrainedPending::_type = certi::e_EnableTimeConstrainedPending ;
long certi::EnableTimeConstrainedWasNotPending::_type = certi::e_EnableTimeConstrainedWasNotPending ;
long certi::EnableTimeRegulationPending::_type = certi::e_EnableTimeRegulationPending ;
long certi::EnableTimeRegulationWasNotPending::_type = certi::e_EnableTimeRegulationWasNotPending ;
long certi::ErrorReadingFED::_type = certi::e_ErrorReadingFED ;
long certi::EventNotKnown::_type = certi::e_EventNotKnown ;
long certi::FederateAlreadyExecutionMember::_type = certi::e_FederateAlreadyExecutionMember ;
long certi::FederateInternalError::_type = certi::e_FederateInternalError ;
long certi::FederateLoggingServiceCalls::_type = certi::e_FederateLoggingServiceCalls ;
long certi::FederateNotExecutionMember::_type = certi::e_FederateNotExecutionMember ;
long certi::FederateOwnsAttributes::_type = certi::e_FederateOwnsAttributes ;
long certi::FederateWasNotAskedToReleaseAttribute::_type = certi::e_FederateWasNotAskedToReleaseAttribute ;
long certi::FederatesCurrentlyJoined::_type = certi::e_FederatesCurrentlyJoined ;
long certi::FederationExecutionAlreadyExists::_type = certi::e_FederationExecutionAlreadyExists ;
long certi::FederationExecutionDoesNotExist::_type = certi::e_FederationExecutionDoesNotExist ;
long certi::FederationTimeAlreadyPassed::_type = certi::e_FederationTimeAlreadyPassed ;
long certi::HandleValuePairMaximumExceeded::_type = certi::e_HandleValuePairMaximumExceeded ;
long certi::InteractionClassNotDefined::_type = certi::e_InteractionClassNotDefined ;
long certi::InteractionClassNotKnown::_type = certi::e_InteractionClassNotKnown ;
long certi::InteractionClassNotPublished::_type = certi::e_InteractionClassNotPublished ;
long certi::InteractionClassNotSubscribed::_type = certi::e_InteractionClassNotSubscribed ;
long certi::InteractionParameterNotDefined::_type = certi::e_InteractionParameterNotDefined ;
long certi::InteractionParameterNotKnown::_type = certi::e_InteractionParameterNotKnown ;
long certi::InvalidExtents::_type = certi::e_InvalidExtents ;
long certi::InvalidFederationTime::_type = certi::e_InvalidFederationTime ;
long certi::InvalidHandleValuePairSetContext::_type = certi::e_InvalidHandleValuePairSetContext ;
long certi::InvalidLookahead::_type = certi::e_InvalidLookahead ;
long certi::InvalidOrderingHandle::_type = certi::e_InvalidOrderingHandle ;
long certi::InvalidRegionContext::_type = certi::e_InvalidRegionContext ;
long certi::InvalidResignAction::_type = certi::e_InvalidResignAction ;
long certi::InvalidRetractionHandle::_type = certi::e_InvalidRetractionHandle ;
long certi::InvalidTransportationHandle::_type = certi::e_InvalidTransportationHandle ;
long certi::MemoryExhausted::_type = certi::e_MemoryExhausted ;
long certi::NameNotFound::_type = certi::e_NameNotFound ;
long certi::ObjectClassNotDefined::_type = certi::e_ObjectClassNotDefined ;
long certi::ObjectClassNotKnown::_type = certi::e_ObjectClassNotKnown ;
long certi::ObjectClassNotPublished::_type = certi::e_ObjectClassNotPublished ;
long certi::ObjectClassNotSubscribed::_type = certi::e_ObjectClassNotSubscribed ;
long certi::ObjectNotKnown::_type = certi::e_ObjectNotKnown ;
long certi::ObjectAlreadyRegistered::_type = certi::e_ObjectAlreadyRegistered ;
long certi::OwnershipAcquisitionPending::_type = certi::e_OwnershipAcquisitionPending ;
long certi::RegionInUse::_type = certi::e_RegionInUse ;
long certi::RegionNotKnown::_type = certi::e_RegionNotKnown ;
long certi::RestoreInProgress::_type = certi::e_RestoreInProgress ;
long certi::RestoreNotRequested::_type = certi::e_RestoreNotRequested ;
long certi::RTIinternalError::_type = certi::e_RTIinternalError ;
long certi::SpaceNotDefined::_type = certi::e_SpaceNotDefined ;
long certi::SaveInProgress::_type = certi::e_SaveInProgress ;
long certi::SaveNotInitiated::_type = certi::e_SaveNotInitiated ;
long certi::SpecifiedSaveLabelDoesNotExist::_type = certi::e_SpecifiedSaveLabelDoesNotExist ;
long certi::SynchronizationPointLabelWasNotAnnounced::_type = certi::e_SynchronizationPointLabelWasNotAnnounced ;
long certi::TimeAdvanceAlreadyInProgress::_type = certi::e_TimeAdvanceAlreadyInProgress ;
long certi::TimeAdvanceWasNotInProgress::_type = certi::e_TimeAdvanceWasNotInProgress ;
long certi::TimeConstrainedAlreadyEnabled::_type = certi::e_TimeConstrainedAlreadyEnabled ;
long certi::TimeConstrainedWasNotEnabled::_type = certi::e_TimeConstrainedWasNotEnabled ;
long certi::TimeRegulationAlreadyEnabled::_type = certi::e_TimeRegulationAlreadyEnabled ;
long certi::TimeRegulationWasNotEnabled::_type = certi::e_TimeRegulationWasNotEnabled ;
long certi::UnableToPerformSave::_type = certi::e_UnableToPerformSave ;
long certi::ValueCountExceeded::_type = certi::e_ValueCountExceeded ;
long certi::ValueLengthExceeded::_type = certi::e_ValueLengthExceeded ;

//  TypeException managing (how to obtain TypeException from Exception name ?)
long certi::FederateNotPublishing::_type = certi::e_FederateNotPublishing ;
long certi::FederateNotSubscribing::_type = certi::e_FederateNotSubscribing ;
long certi::InvalidObjectHandle::_type = certi::e_InvalidObjectHandle ;
long certi::CouldNotOpenRID::_type = certi::e_CouldNotOpenRID ;
long certi::ErrorReadingRID::_type = certi::e_ErrorReadingRID ;
long certi::AttributeNotSubscribed::_type = certi::e_AttributeNotSubscribed ;
long certi::FederationAlreadyPaused::_type = certi::e_FederationAlreadyPaused ;
long certi::FederationNotPaused::_type = certi::e_FederationNotPaused ;
long certi::SecurityError::_type = certi::e_SecurityError ;
long certi::FederateAlreadyPaused::_type = certi::e_FederateAlreadyPaused ;
long certi::FederateDoesNotExist::_type = certi::e_FederateDoesNotExist ;
long certi::FederateNameAlreadyInUse::_type = certi::e_FederateNameAlreadyInUse ;
long certi::FederateNotPaused::_type = certi::e_FederateNotPaused ;
long certi::IDsupplyExhausted::_type = certi::e_IDsupplyExhausted ;
long certi::InvalidDivestitureCondition::_type = certi::e_InvalidDivestitureCondition ;
long certi::InvalidFederationTimeDelta::_type = certi::e_InvalidFederationTimeDelta ;
long certi::InvalidRoutingSpace::_type = certi::e_InvalidRoutingSpace ;
long certi::NoPauseRequested::_type = certi::e_NoPauseRequested ;
long certi::NoResumeRequested::_type = certi::e_NoResumeRequested ;
long certi::TooManyIDsRequested::_type = certi::e_TooManyIDsRequested ;
long certi::UnimplementedService::_type = certi::e_UnimplementedService ;
long certi::UnknownLabel::_type = certi::e_UnknownLabel ;
long certi::NetworkError::_type = certi::e_NetworkError ;
long certi::NetworkSignal::_type = certi::e_NetworkSignal ;
long certi::SocketNotConnected::_type = certi::e_SocketNotConnected ;
long certi::MessageNotSent::_type = certi::e_MessageNotSent ;
long certi::MessageNotReceived::_type = certi::e_MessageNotReceived ;
long certi::SocketNotClosed::_type = certi::e_SocketNotClosed ;
long certi::RingBufferNotCreated::_type = certi::e_RingBufferNotCreated ;
long certi::RingBufferNotClosed::_type = certi::e_RingBufferNotClosed ;
long certi::RingBufferNotDeleted::_type = certi::e_RingBufferNotDeleted ;
long certi::RingBufferNotAttached::_type = certi::e_RingBufferNotAttached ;
long certi::MessageTooLong::_type = certi::e_MessageTooLong ;
long certi::BufferFull::_type = certi::e_BufferFull ;
long certi::BufferEmpty::_type = certi::e_BufferEmpty ;
long certi::SocketSHMNotCreated::_type = certi::e_SocketSHMNotCreated ;
long certi::SocketSHMNotOpen::_type = certi::e_SocketSHMNotOpen ;
long certi::SocketSHMNotDeleted::_type = certi::e_SocketSHMNotDeleted ;

long certi::IllegalName::_type = certi::e_IllegalName ;

static PrettyDebug PD_Exception("CERTI_EXCEPTION",__FILE__);

const std::string certi::Exception::displayMe() const
{
    std::stringstream msg;

    msg << "CERTI::Exception [";
    if (NULL!=_name) {
        msg <<_name;
    } else {
        msg<<"<noname>";
    }
    msg << " - reason=";
    if (!_reason.empty()) {
        msg << _reason;
    } else {
        msg << "<noreason>";
    }
    msg << std::endl;
    msg << std::flush;

    Debug(PD_Exception, pdExcept) << msg.str();
    return msg.str();
}
