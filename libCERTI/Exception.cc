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
// $Id: Exception.cc,v 3.3 2005/04/30 16:50:43 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Exception.hh"

#include "certi.hh"

// static members for HLA Exceptions
const char *RTI::ArrayIndexOutOfBounds::_ex = 0 ;
const char *RTI::AsynchronousDeliveryAlreadyDisabled::_ex = 0 ;
const char *RTI::AsynchronousDeliveryAlreadyEnabled::_ex = 0 ;
const char *RTI::AttributeAcquisitionWasNotRequested::_ex = 0 ;
const char *RTI::AttributeAcquisitionWasNotCanceled::_ex = 0 ;
const char *RTI::AttributeAlreadyBeingAcquired::_ex = 0 ;
const char *RTI::AttributeAlreadyBeingDivested::_ex = 0 ;
const char *RTI::AttributeAlreadyOwned::_ex = 0 ;
const char *RTI::AttributeDivestitureWasNotRequested::_ex = 0 ;
const char *RTI::AttributeNotDefined::_ex = 0 ;
const char *RTI::AttributeNotKnown::_ex = 0 ;
const char *RTI::AttributeNotOwned::_ex = 0 ;
const char *RTI::AttributeNotPublished::_ex = 0 ;
const char *RTI::ConcurrentAccessAttempted::_ex = 0 ;
const char *RTI::CouldNotDiscover::_ex = 0 ;
const char *RTI::CouldNotOpenFED::_ex = 0 ;
const char *RTI::CouldNotRestore::_ex = 0 ;
const char *RTI::DeletePrivilegeNotHeld::_ex = 0 ;
const char *RTI::DimensionNotDefined::_ex = 0 ;
const char *RTI::EnableTimeConstrainedPending::_ex = 0 ;
const char *RTI::EnableTimeConstrainedWasNotPending::_ex = 0 ;
const char *RTI::EnableTimeRegulationPending::_ex = 0 ;
const char *RTI::EnableTimeRegulationWasNotPending::_ex = 0 ;
const char *RTI::ErrorReadingFED::_ex = 0 ;
const char *RTI::EventNotKnown::_ex = 0 ;
const char *RTI::FederateAlreadyExecutionMember::_ex = 0 ;
const char *RTI::FederateInternalError::_ex = 0 ;
const char *RTI::FederateLoggingServiceCalls::_ex = 0 ;
const char *RTI::FederateNotExecutionMember::_ex = 0 ;
const char *RTI::FederateOwnsAttributes::_ex = 0 ;
const char *RTI::FederateWasNotAskedToReleaseAttribute::_ex = 0 ;
const char *RTI::FederatesCurrentlyJoined::_ex = 0 ;
const char *RTI::FederationExecutionAlreadyExists::_ex = 0 ;
const char *RTI::FederationExecutionDoesNotExist::_ex = 0 ;
const char *RTI::FederationTimeAlreadyPassed::_ex = 0 ;
const char *RTI::HandleValuePairMaximumExceeded::_ex = 0 ;
const char *RTI::InteractionClassNotDefined::_ex = 0 ;
const char *RTI::InteractionClassNotKnown::_ex = 0 ;
const char *RTI::InteractionClassNotPublished::_ex = 0 ;
const char *RTI::InteractionClassNotSubscribed::_ex = 0 ;
const char *RTI::InteractionParameterNotDefined::_ex = 0 ;
const char *RTI::InteractionParameterNotKnown::_ex = 0 ;
const char *RTI::InvalidExtents::_ex = 0 ;
const char *RTI::InvalidFederationTime::_ex = 0 ;
const char *RTI::InvalidHandleValuePairSetContext::_ex = 0 ;
const char *RTI::InvalidLookahead::_ex = 0 ;
const char *RTI::InvalidOrderingHandle::_ex = 0 ;
const char *RTI::InvalidRegionContext::_ex = 0 ;
const char *RTI::InvalidResignAction::_ex = 0 ;
const char *RTI::InvalidRetractionHandle::_ex = 0 ;
const char *RTI::InvalidTransportationHandle::_ex = 0 ;
const char *RTI::MemoryExhausted::_ex = 0 ;
const char *RTI::NameNotFound::_ex = 0 ;
const char *RTI::ObjectClassNotDefined::_ex = 0 ;
const char *RTI::ObjectClassNotKnown::_ex = 0 ;
const char *RTI::ObjectClassNotPublished::_ex = 0 ;
const char *RTI::ObjectClassNotSubscribed::_ex = 0 ;
const char *RTI::ObjectNotKnown::_ex = 0 ;
const char *RTI::ObjectAlreadyRegistered::_ex = 0 ;
const char *RTI::OwnershipAcquisitionPending::_ex = 0 ;
const char *RTI::RegionInUse::_ex = 0 ;
const char *RTI::RegionNotKnown::_ex = 0 ;
const char *RTI::RestoreInProgress::_ex = 0 ;
const char *RTI::RestoreNotRequested::_ex = 0 ;
const char *RTI::RTIinternalError::_ex = 0 ;
const char *RTI::SpaceNotDefined::_ex = 0 ;
const char *RTI::SaveInProgress::_ex = 0 ;
const char *RTI::SaveNotInitiated::_ex = 0 ;
const char *RTI::SpecifiedSaveLabelDoesNotExist::_ex = 0 ;
const char *RTI::SynchronizationPointLabelWasNotAnnounced::_ex = 0 ;
const char *RTI::TimeAdvanceAlreadyInProgress::_ex = 0 ;
const char *RTI::TimeAdvanceWasNotInProgress::_ex = 0 ;
const char *RTI::TimeConstrainedAlreadyEnabled::_ex = 0 ;
const char *RTI::TimeConstrainedWasNotEnabled::_ex = 0 ;
const char *RTI::TimeRegulationAlreadyEnabled::_ex = 0 ;
const char *RTI::TimeRegulationWasNotEnabled::_ex = 0 ;
const char *RTI::UnableToPerformSave::_ex = 0 ;
const char *RTI::ValueCountExceeded::_ex = 0 ;
const char *RTI::ValueLengthExceeded::_ex = 0 ;

// From certi.hh
const char *certi::FederateNotPublishing::_ex = 0 ;
const char *certi::FederateNotSubscribing::_ex = 0 ;
const char *certi::InvalidObjectHandle::_ex = 0 ;
const char *certi::InvalidOrderType::_ex = 0 ;
const char *certi::InvalidTransportType::_ex = 0 ;
const char *certi::SecurityError::_ex = 0 ;
const char *certi::CouldNotOpenRID::_ex = 0 ;
const char *certi::ErrorReadingRID::_ex = 0 ;
const char *certi::FederationAlreadyPaused::_ex = 0 ;
const char *certi::FederationNotPaused::_ex = 0 ;
const char *certi::AttributeNotSubscribed::_ex = 0 ;
const char *certi::FederateAlreadyPaused::_ex = 0 ;
const char *certi::FederateDoesNotExist::_ex = 0 ;
const char *certi::FederateNameAlreadyInUse::_ex = 0 ;
const char *certi::FederateNotPaused::_ex = 0 ;
const char *certi::IDsupplyExhausted::_ex = 0 ;
const char *certi::InvalidDivestitureCondition::_ex = 0 ;
const char *certi::InvalidFederationTimeDelta::_ex = 0 ;
const char *certi::InvalidRoutingSpace::_ex = 0 ;
const char *certi::NoPauseRequested::_ex = 0 ;
const char *certi::NoResumeRequested::_ex = 0 ;
const char *certi::TooManyIDsRequested::_ex = 0 ;
const char *certi::UnimplementedService::_ex = 0 ;
const char *certi::UnknownLabel::_ex = 0 ;
const char *certi::NetworkError::_ex = 0 ;
const char *certi::NetworkSignal::_ex = 0 ;

RTI::Exception::~Exception()
{
}

RTI::Exception::Exception(const char *reason)
{
    _reason = const_cast<char *>(reason);
    _serial = 0 ;
}

RTI::Exception::Exception(ULong serial, const char *reason)
{
    _serial = serial ;
    _reason = const_cast<char *>(reason);
}


RTI::Exception::Exception(const Exception &toCopy)
{
    _serial = toCopy._serial ;
    _reason = toCopy._reason ;
    _name = toCopy._name ;
}

RTI::Exception& RTI::Exception::operator=(const Exception &toCopy)
{
    _serial = toCopy._serial ;
    _reason = toCopy._reason ;
    _name = toCopy._name ;

    return *this ;
}

// $Id: Exception.cc,v 3.3 2005/04/30 16:50:43 breholee Exp $$
