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
// $Id: Exception.cc,v 3.6 2007/06/22 08:51:36 erk Exp $
// ----------------------------------------------------------------------------

#include "Certi_Win.h"
#include "Exception.hh"

#include "certi.hh"

/*! \cond
  Doxygen will ignore because it doesn't like this and I don't know why
*/
// static members for HLA Exceptions
const char *RTI::ArrayIndexOutOfBounds::_ex = "ArrayIndexOutOfBounds" ;
const char *RTI::AsynchronousDeliveryAlreadyDisabled::_ex = "AsynchronousDeliveryAlreadyDisabled" ;
const char *RTI::AsynchronousDeliveryAlreadyEnabled::_ex = "AsynchronousDeliveryAlreadyEnabled" ;
const char *RTI::AttributeAcquisitionWasNotRequested::_ex = "AttributeAcquisitionWasNotRequested" ;
const char *RTI::AttributeAcquisitionWasNotCanceled::_ex = "AttributeAcquisitionWasNotCanceled" ;
const char *RTI::AttributeAlreadyBeingAcquired::_ex = "AttributeAlreadyBeingAcquired" ;
const char *RTI::AttributeAlreadyBeingDivested::_ex = "AttributeAlreadyBeingDivested" ;
const char *RTI::AttributeAlreadyOwned::_ex = "AttributeAlreadyOwned" ;
const char *RTI::AttributeDivestitureWasNotRequested::_ex = "AttributeDivestitureWasNotRequested" ;
const char *RTI::AttributeNotDefined::_ex = "AttributeNotDefined" ;
const char *RTI::AttributeNotKnown::_ex = "AttributeNotKnown" ;
const char *RTI::AttributeNotOwned::_ex = "AttributeNotOwned" ;
const char *RTI::AttributeNotPublished::_ex = "AttributeNotPublished" ;
const char *RTI::ConcurrentAccessAttempted::_ex = "ConcurrentAccessAttempted" ;
const char *RTI::CouldNotDiscover::_ex = "CouldNotDiscover" ;
const char *RTI::CouldNotOpenFED::_ex = "CouldNotOpenFED" ;
const char *RTI::CouldNotRestore::_ex = "CouldNotRestore" ;
const char *RTI::DeletePrivilegeNotHeld::_ex = "DeletePrivilegeNotHeld" ;
const char *RTI::DimensionNotDefined::_ex = "DimensionNotDefined" ;
const char *RTI::EnableTimeConstrainedPending::_ex = "EnableTimeConstrainedPending" ;
const char *RTI::EnableTimeConstrainedWasNotPending::_ex = "EnableTimeConstrainedWasNotPending" ;
const char *RTI::EnableTimeRegulationPending::_ex = "EnableTimeRegulationPending" ;
const char *RTI::EnableTimeRegulationWasNotPending::_ex = "EnableTimeRegulationWasNotPending" ;
const char *RTI::ErrorReadingFED::_ex = "ErrorReadingFED" ;
const char *RTI::EventNotKnown::_ex = "EventNotKnown" ;
const char *RTI::FederateAlreadyExecutionMember::_ex = "FederateAlreadyExecutionMember" ;
const char *RTI::FederateInternalError::_ex = "FederateInternalError" ;
const char *RTI::FederateLoggingServiceCalls::_ex = "FederateLoggingServiceCalls" ;
const char *RTI::FederateNotExecutionMember::_ex = "FederateNotExecutionMember" ;
const char *RTI::FederateOwnsAttributes::_ex = "FederateOwnsAttributes" ;
const char *RTI::FederateWasNotAskedToReleaseAttribute::_ex = "FederateWasNotAskedToReleaseAttribute" ;
const char *RTI::FederatesCurrentlyJoined::_ex = "FederatesCurrentlyJoined" ;
const char *RTI::FederationExecutionAlreadyExists::_ex = "FederationExecutionAlreadyExists" ;
const char *RTI::FederationExecutionDoesNotExist::_ex = "FederationExecutionDoesNotExist" ;
const char *RTI::FederationTimeAlreadyPassed::_ex = "FederationTimeAlreadyPassed" ;
const char *RTI::HandleValuePairMaximumExceeded::_ex = "HandleValuePairMaximumExceeded" ;
const char *RTI::InteractionClassNotDefined::_ex = "InteractionClassNotDefined" ;
const char *RTI::InteractionClassNotKnown::_ex = "InteractionClassNotKnown" ;
const char *RTI::InteractionClassNotPublished::_ex = "InteractionClassNotPublished" ;
const char *RTI::InteractionClassNotSubscribed::_ex = "InteractionClassNotSubscribed" ;
const char *RTI::InteractionParameterNotDefined::_ex = "InteractionParameterNotDefined" ;
const char *RTI::InteractionParameterNotKnown::_ex = "InteractionParameterNotKnown" ;
const char *RTI::InvalidExtents::_ex = "InvalidExtents" ;
const char *RTI::InvalidFederationTime::_ex = "InvalidFederationTime" ;
const char *RTI::InvalidHandleValuePairSetContext::_ex = "InvalidHandleValuePairSetContext" ;
const char *RTI::InvalidLookahead::_ex = "InvalidLookahead" ;
const char *RTI::InvalidOrderingHandle::_ex = "InvalidOrderingHandle" ;
const char *RTI::InvalidRegionContext::_ex = "InvalidRegionContext" ;
const char *RTI::InvalidResignAction::_ex = "InvalidResignAction" ;
const char *RTI::InvalidRetractionHandle::_ex = "InvalidRetractionHandle" ;
const char *RTI::InvalidTransportationHandle::_ex = "InvalidTransportationHandle" ;
const char *RTI::MemoryExhausted::_ex = "MemoryExhausted" ;
const char *RTI::NameNotFound::_ex = "NameNotFound" ;
const char *RTI::ObjectClassNotDefined::_ex = "ObjectClassNotDefined" ;
const char *RTI::ObjectClassNotKnown::_ex = "ObjectClassNotKnown" ;
const char *RTI::ObjectClassNotPublished::_ex = "ObjectClassNotPublished" ;
const char *RTI::ObjectClassNotSubscribed::_ex = "ObjectClassNotSubscribed" ;
const char *RTI::ObjectNotKnown::_ex = "ObjectNotKnown" ;
const char *RTI::ObjectAlreadyRegistered::_ex = "ObjectAlreadyRegistered" ;
const char *RTI::OwnershipAcquisitionPending::_ex = "OwnershipAcquisitionPending" ;
const char *RTI::RegionInUse::_ex = "RegionInUse" ;
const char *RTI::RegionNotKnown::_ex = "RegionNotKnown" ;
const char *RTI::RestoreInProgress::_ex = "RestoreInProgress" ;
const char *RTI::RestoreNotRequested::_ex = "RestoreNotRequested" ;
const char *RTI::RTIinternalError::_ex = "RTIinternalError" ;
const char *RTI::SpaceNotDefined::_ex = "SpaceNotDefined" ;
const char *RTI::SaveInProgress::_ex = "SaveInProgress" ;
const char *RTI::SaveNotInitiated::_ex = "SaveNotInitiated" ;
const char *RTI::SpecifiedSaveLabelDoesNotExist::_ex = "SpecifiedSaveLabelDoesNotExist" ;
const char *RTI::SynchronizationPointLabelWasNotAnnounced::_ex = "SynchronizationPointLabelWasNotAnnounced" ;
const char *RTI::TimeAdvanceAlreadyInProgress::_ex = "TimeAdvanceAlreadyInProgress" ;
const char *RTI::TimeAdvanceWasNotInProgress::_ex = "TimeAdvanceWasNotInProgress" ;
const char *RTI::TimeConstrainedAlreadyEnabled::_ex = "TimeConstrainedAlreadyEnabled" ;
const char *RTI::TimeConstrainedWasNotEnabled::_ex = "TimeConstrainedWasNotEnabled" ;
const char *RTI::TimeRegulationAlreadyEnabled::_ex = "TimeRegulationAlreadyEnabled" ;
const char *RTI::TimeRegulationWasNotEnabled::_ex = "TimeRegulationWasNotEnabled" ;
const char *RTI::UnableToPerformSave::_ex = "UnableToPerformSave" ;
const char *RTI::ValueCountExceeded::_ex = "ValueCountExceeded" ;
const char *RTI::ValueLengthExceeded::_ex = "ValueLengthExceeded" ;

// From certi.hh
const char *certi::FederateNotPublishing::_ex = "FederateNotPublishing" ;
const char *certi::FederateNotSubscribing::_ex = "FederateNotSubscribing" ;
const char *certi::InvalidObjectHandle::_ex = "InvalidObjectHandle" ;
const char *certi::InvalidOrderType::_ex = "InvalidOrderType" ;
const char *certi::InvalidTransportType::_ex = "InvalidTransportType" ;
const char *certi::SecurityError::_ex = "SecurityError" ;
const char *certi::CouldNotOpenRID::_ex = "CouldNotOpenRID" ;
const char *certi::ErrorReadingRID::_ex = "ErrorReadingRID" ;
const char *certi::FederationAlreadyPaused::_ex = "FederationAlreadyPaused" ;
const char *certi::FederationNotPaused::_ex = "FederationNotPaused" ;
const char *certi::AttributeNotSubscribed::_ex = "AttributeNotSubscribed" ;
const char *certi::FederateAlreadyPaused::_ex = "FederateAlreadyPaused" ;
const char *certi::FederateDoesNotExist::_ex = "FederateDoesNotExist" ;
const char *certi::FederateNameAlreadyInUse::_ex = "FederateNameAlreadyInUse" ;
const char *certi::FederateNotPaused::_ex = "FederateNotPaused" ;
const char *certi::IDsupplyExhausted::_ex = "IDsupplyExhausted" ;
const char *certi::InvalidDivestitureCondition::_ex = "InvalidDivestitureCondition" ;
const char *certi::InvalidFederationTimeDelta::_ex = "InvalidFederationTimeDelta" ;
const char *certi::InvalidRoutingSpace::_ex = "InvalidRoutingSpace" ;
const char *certi::NoPauseRequested::_ex = "NoPauseRequested" ;
const char *certi::NoResumeRequested::_ex = "NoResumeRequested" ;
const char *certi::TooManyIDsRequested::_ex = "TooManyIDsRequested" ;
const char *certi::UnimplementedService::_ex = "UnimplementedService" ;
const char *certi::UnknownLabel::_ex = "UnknownLabel" ;
const char *certi::NetworkError::_ex = "NetworkError" ;
const char *certi::NetworkSignal::_ex = "NetworkSignal" ;
/*! \endcond
*/
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

// $Id: Exception.cc,v 3.6 2007/06/22 08:51:36 erk Exp $$
