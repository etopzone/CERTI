// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2006  ONERA
//
// This program is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: RTItypes.cc,v 1.1 2014/03/03 15:18:23 erk Exp $
// ----------------------------------------------------------------------------

#include "Exception.hh"
#include "PrettyDebug.hh"
#include <assert.h>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include "RTI.hh"

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

// TypeException managing (how to obtain TypeException from Exception name ?)
long RTI::ArrayIndexOutOfBounds::type = certi::e_ArrayIndexOutOfBounds ;
long RTI::AsynchronousDeliveryAlreadyDisabled::type = certi::e_AsynchronousDeliveryAlreadyDisabled ;
long RTI::AsynchronousDeliveryAlreadyEnabled::type = certi::e_AsynchronousDeliveryAlreadyEnabled ;
long RTI::AttributeAcquisitionWasNotRequested::type = certi::e_AttributeAcquisitionWasNotRequested ;
long RTI::AttributeAcquisitionWasNotCanceled::type = certi::e_AttributeAcquisitionWasNotCanceled ;
long RTI::AttributeAlreadyBeingAcquired::type = certi::e_AttributeAlreadyBeingAcquired ;
long RTI::AttributeAlreadyBeingDivested::type = certi::e_AttributeAlreadyBeingDivested ;
long RTI::AttributeAlreadyOwned::type = certi::e_AttributeAlreadyOwned ;
long RTI::AttributeDivestitureWasNotRequested::type = certi::e_AttributeDivestitureWasNotRequested ;
long RTI::AttributeNotDefined::type = certi::e_AttributeNotDefined ;
long RTI::AttributeNotKnown::type = certi::e_AttributeNotKnown ;
long RTI::AttributeNotOwned::type = certi::e_AttributeNotOwned ;
long RTI::AttributeNotPublished::type = certi::e_AttributeNotPublished ;
long RTI::ConcurrentAccessAttempted::type = certi::e_ConcurrentAccessAttempted ;
long RTI::CouldNotDiscover::type = certi::e_CouldNotDiscover ;
long RTI::CouldNotOpenFED::type = certi::e_CouldNotOpenFED ;
long RTI::CouldNotRestore::type = certi::e_CouldNotRestore ;
long RTI::DeletePrivilegeNotHeld::type = certi::e_DeletePrivilegeNotHeld ;
long RTI::DimensionNotDefined::type = certi::e_DimensionNotDefined ;
long RTI::EnableTimeConstrainedPending::type = certi::e_EnableTimeConstrainedPending ;
long RTI::EnableTimeConstrainedWasNotPending::type = certi::e_EnableTimeConstrainedWasNotPending ;
long RTI::EnableTimeRegulationPending::type = certi::e_EnableTimeRegulationPending ;
long RTI::EnableTimeRegulationWasNotPending::type = certi::e_EnableTimeRegulationWasNotPending ;
long RTI::ErrorReadingFED::type = certi::e_ErrorReadingFED ;
long RTI::EventNotKnown::type = certi::e_EventNotKnown ;
long RTI::FederateAlreadyExecutionMember::type = certi::e_FederateAlreadyExecutionMember ;
long RTI::FederateInternalError::type = certi::e_FederateInternalError ;
long RTI::FederateLoggingServiceCalls::type = certi::e_FederateLoggingServiceCalls ;
long RTI::FederateNotExecutionMember::type = certi::e_FederateNotExecutionMember ;
long RTI::FederateOwnsAttributes::type = certi::e_FederateOwnsAttributes ;
long RTI::FederateWasNotAskedToReleaseAttribute::type = certi::e_FederateWasNotAskedToReleaseAttribute ;
long RTI::FederatesCurrentlyJoined::type = certi::e_FederatesCurrentlyJoined ;
long RTI::FederationExecutionAlreadyExists::type = certi::e_FederationExecutionAlreadyExists ;
long RTI::FederationExecutionDoesNotExist::type = certi::e_FederationExecutionDoesNotExist ;
long RTI::FederationTimeAlreadyPassed::type = certi::e_FederationTimeAlreadyPassed ;
long RTI::HandleValuePairMaximumExceeded::type = certi::e_HandleValuePairMaximumExceeded ;
long RTI::InteractionClassNotDefined::type = certi::e_InteractionClassNotDefined ;
long RTI::InteractionClassNotKnown::type = certi::e_InteractionClassNotKnown ;
long RTI::InteractionClassNotPublished::type = certi::e_InteractionClassNotPublished ;
long RTI::InteractionClassNotSubscribed::type = certi::e_InteractionClassNotSubscribed ;
long RTI::InteractionParameterNotDefined::type = certi::e_InteractionParameterNotDefined ;
long RTI::InteractionParameterNotKnown::type = certi::e_InteractionParameterNotKnown ;
long RTI::InvalidExtents::type = certi::e_InvalidExtents ;
long RTI::InvalidFederationTime::type = certi::e_InvalidFederationTime ;
long RTI::InvalidHandleValuePairSetContext::type = certi::e_InvalidHandleValuePairSetContext ;
long RTI::InvalidLookahead::type = certi::e_InvalidLookahead ;
long RTI::InvalidOrderingHandle::type = certi::e_InvalidOrderingHandle ;
long RTI::InvalidRegionContext::type = certi::e_InvalidRegionContext ;
long RTI::InvalidResignAction::type = certi::e_InvalidResignAction ;
long RTI::InvalidRetractionHandle::type = certi::e_InvalidRetractionHandle ;
long RTI::InvalidTransportationHandle::type = certi::e_InvalidTransportationHandle ;
long RTI::MemoryExhausted::type = certi::e_MemoryExhausted ;
long RTI::NameNotFound::type = certi::e_NameNotFound ;
long RTI::ObjectClassNotDefined::type = certi::e_ObjectClassNotDefined ;
long RTI::ObjectClassNotKnown::type = certi::e_ObjectClassNotKnown ;
long RTI::ObjectClassNotPublished::type = certi::e_ObjectClassNotPublished ;
long RTI::ObjectClassNotSubscribed::type = certi::e_ObjectClassNotSubscribed ;
long RTI::ObjectNotKnown::type = certi::e_ObjectNotKnown ;
long RTI::ObjectAlreadyRegistered::type = certi::e_ObjectAlreadyRegistered ;
long RTI::OwnershipAcquisitionPending::type = certi::e_OwnershipAcquisitionPending ;
long RTI::RegionInUse::type = certi::e_RegionInUse ;
long RTI::RegionNotKnown::type = certi::e_RegionNotKnown ;
long RTI::RestoreInProgress::type = certi::e_RestoreInProgress ;
long RTI::RestoreNotRequested::type = certi::e_RestoreNotRequested ;
long RTI::RTIinternalError::type = certi::e_RTIinternalError ;
long RTI::SpaceNotDefined::type = certi::e_SpaceNotDefined ;
long RTI::SaveInProgress::type = certi::e_SaveInProgress ;
long RTI::SaveNotInitiated::type = certi::e_SaveNotInitiated ;
long RTI::SpecifiedSaveLabelDoesNotExist::type = certi::e_SpecifiedSaveLabelDoesNotExist ;
long RTI::SynchronizationPointLabelWasNotAnnounced::type = certi::e_SynchronizationPointLabelWasNotAnnounced ;
long RTI::TimeAdvanceAlreadyInProgress::type = certi::e_TimeAdvanceAlreadyInProgress ;
long RTI::TimeAdvanceWasNotInProgress::type = certi::e_TimeAdvanceWasNotInProgress ;
long RTI::TimeConstrainedAlreadyEnabled::type = certi::e_TimeConstrainedAlreadyEnabled ;
long RTI::TimeConstrainedWasNotEnabled::type = certi::e_TimeConstrainedWasNotEnabled ;
long RTI::TimeRegulationAlreadyEnabled::type = certi::e_TimeRegulationAlreadyEnabled ;
long RTI::TimeRegulationWasNotEnabled::type = certi::e_TimeRegulationWasNotEnabled ;
long RTI::UnableToPerformSave::type = certi::e_UnableToPerformSave ;
long RTI::ValueCountExceeded::type = certi::e_ValueCountExceeded ;
long RTI::ValueLengthExceeded::type = certi::e_ValueLengthExceeded ;

static PrettyDebug PD_RTIException("RTI_EXCEPTION",__FILE__);

RTI::Exception::~Exception()
{
    if (NULL!=_reason) {
        free(_reason);
    }
}

RTI::Exception::Exception(const char *reason)
{
    _reason = (reason == NULL)?NULL:strdup(reason);
    _serial = 0 ;
}

RTI::Exception::Exception(ULong serial, const char *reason)
{
    _serial = serial ;
    _reason = (reason == NULL)?NULL:strdup(reason);
}


RTI::Exception::Exception(const Exception &toCopy)
{
    _serial = toCopy._serial ;
    _reason = ( toCopy._reason== NULL)?NULL:strdup(toCopy._reason) ;
    _name = toCopy._name ;
}

RTI::Exception& RTI::Exception::operator=(const Exception &toCopy)
{
    _serial = toCopy._serial ;
    _reason = (toCopy._reason == NULL)?NULL:strdup(toCopy._reason) ;
    _name = toCopy._name ;
    return *this ;
}

const std::string RTI::Exception::displayMe() const
{
    std::stringstream msg;

    msg << "RTI::Exception [";
    if (NULL!=_name) {
        msg <<_name;
    } else {
        msg<<"<noname>";
    }
    msg <<",0x"<<std::hex<<_serial<<"]";
    msg << " - reason=";
    if (NULL !=_reason) {
        msg << _reason;
    } else {
        msg << "<noreason>";
    }
    msg << std::endl;
    msg << std::flush;

    Debug(PD_RTIException, pdExcept) << msg.str();
    return msg.str();
}

RTI_STD::ostream &
operator<<(RTI_STD::ostream &os, RTI::Exception const &ex)
{
    // FIXME
    return os << ex.displayMe();
}

RTI_STD::ostream &
operator<<(RTI_STD::ostream &os, RTI::Exception *ex)
{
    assert(ex);
    return os<<(*ex);
}

// $Id: RTItypes.cc,v 1.1 2014/03/03 15:18:23 erk Exp $
