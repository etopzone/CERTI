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
long RTI::ArrayIndexOutOfBounds::type = static_cast<long>(certi::Exception::Type::ArrayIndexOutOfBounds) ;
long RTI::AsynchronousDeliveryAlreadyDisabled::type = static_cast<long>(certi::Exception::Type::AsynchronousDeliveryAlreadyDisabled );
long RTI::AsynchronousDeliveryAlreadyEnabled::type = static_cast<long>(certi::Exception::Type::AsynchronousDeliveryAlreadyEnabled );
long RTI::AttributeAcquisitionWasNotRequested::type = static_cast<long>(certi::Exception::Type::AttributeAcquisitionWasNotRequested );
long RTI::AttributeAcquisitionWasNotCanceled::type = static_cast<long>(certi::Exception::Type::AttributeAcquisitionWasNotCanceled );
long RTI::AttributeAlreadyBeingAcquired::type = static_cast<long>(certi::Exception::Type::AttributeAlreadyBeingAcquired );
long RTI::AttributeAlreadyBeingDivested::type = static_cast<long>(certi::Exception::Type::AttributeAlreadyBeingDivested );
long RTI::AttributeAlreadyOwned::type = static_cast<long>(certi::Exception::Type::AttributeAlreadyOwned );
long RTI::AttributeDivestitureWasNotRequested::type = static_cast<long>(certi::Exception::Type::AttributeDivestitureWasNotRequested );
long RTI::AttributeNotDefined::type = static_cast<long>(certi::Exception::Type::AttributeNotDefined );
long RTI::AttributeNotKnown::type = static_cast<long>(certi::Exception::Type::AttributeNotKnown );
long RTI::AttributeNotOwned::type = static_cast<long>(certi::Exception::Type::AttributeNotOwned );
long RTI::AttributeNotPublished::type = static_cast<long>(certi::Exception::Type::AttributeNotPublished );
long RTI::ConcurrentAccessAttempted::type = static_cast<long>(certi::Exception::Type::ConcurrentAccessAttempted );
long RTI::CouldNotDiscover::type = static_cast<long>(certi::Exception::Type::CouldNotDiscover );
long RTI::CouldNotOpenFED::type = static_cast<long>(certi::Exception::Type::CouldNotOpenFED );
long RTI::CouldNotRestore::type = static_cast<long>(certi::Exception::Type::CouldNotRestore );
long RTI::DeletePrivilegeNotHeld::type = static_cast<long>(certi::Exception::Type::DeletePrivilegeNotHeld );
long RTI::DimensionNotDefined::type = static_cast<long>(certi::Exception::Type::DimensionNotDefined );
long RTI::EnableTimeConstrainedPending::type = static_cast<long>(certi::Exception::Type::EnableTimeConstrainedPending );
long RTI::EnableTimeConstrainedWasNotPending::type = static_cast<long>(certi::Exception::Type::EnableTimeConstrainedWasNotPending );
long RTI::EnableTimeRegulationPending::type = static_cast<long>(certi::Exception::Type::EnableTimeRegulationPending );
long RTI::EnableTimeRegulationWasNotPending::type = static_cast<long>(certi::Exception::Type::EnableTimeRegulationWasNotPending );
long RTI::ErrorReadingFED::type = static_cast<long>(certi::Exception::Type::ErrorReadingFED );
long RTI::EventNotKnown::type = static_cast<long>(certi::Exception::Type::EventNotKnown );
long RTI::FederateAlreadyExecutionMember::type = static_cast<long>(certi::Exception::Type::FederateAlreadyExecutionMember );
long RTI::FederateInternalError::type = static_cast<long>(certi::Exception::Type::FederateInternalError );
long RTI::FederateLoggingServiceCalls::type = static_cast<long>(certi::Exception::Type::FederateLoggingServiceCalls );
long RTI::FederateNotExecutionMember::type = static_cast<long>(certi::Exception::Type::FederateNotExecutionMember );
long RTI::FederateOwnsAttributes::type = static_cast<long>(certi::Exception::Type::FederateOwnsAttributes );
long RTI::FederateWasNotAskedToReleaseAttribute::type = static_cast<long>(certi::Exception::Type::FederateWasNotAskedToReleaseAttribute );
long RTI::FederatesCurrentlyJoined::type = static_cast<long>(certi::Exception::Type::FederatesCurrentlyJoined );
long RTI::FederationExecutionAlreadyExists::type = static_cast<long>(certi::Exception::Type::FederationExecutionAlreadyExists );
long RTI::FederationExecutionDoesNotExist::type = static_cast<long>(certi::Exception::Type::FederationExecutionDoesNotExist );
long RTI::FederationTimeAlreadyPassed::type = static_cast<long>(certi::Exception::Type::FederationTimeAlreadyPassed );
long RTI::HandleValuePairMaximumExceeded::type = static_cast<long>(certi::Exception::Type::HandleValuePairMaximumExceeded );
long RTI::InteractionClassNotDefined::type = static_cast<long>(certi::Exception::Type::InteractionClassNotDefined );
long RTI::InteractionClassNotKnown::type = static_cast<long>(certi::Exception::Type::InteractionClassNotKnown );
long RTI::InteractionClassNotPublished::type = static_cast<long>(certi::Exception::Type::InteractionClassNotPublished );
long RTI::InteractionClassNotSubscribed::type = static_cast<long>(certi::Exception::Type::InteractionClassNotSubscribed );
long RTI::InteractionParameterNotDefined::type = static_cast<long>(certi::Exception::Type::InteractionParameterNotDefined );
long RTI::InteractionParameterNotKnown::type = static_cast<long>(certi::Exception::Type::InteractionParameterNotKnown );
long RTI::InvalidExtents::type = static_cast<long>(certi::Exception::Type::InvalidExtents );
long RTI::InvalidFederationTime::type = static_cast<long>(certi::Exception::Type::InvalidFederationTime );
long RTI::InvalidHandleValuePairSetContext::type = static_cast<long>(certi::Exception::Type::InvalidHandleValuePairSetContext );
long RTI::InvalidLookahead::type = static_cast<long>(certi::Exception::Type::InvalidLookahead );
long RTI::InvalidOrderingHandle::type = static_cast<long>(certi::Exception::Type::InvalidOrderingHandle );
long RTI::InvalidRegionContext::type = static_cast<long>(certi::Exception::Type::InvalidRegionContext );
long RTI::InvalidResignAction::type = static_cast<long>(certi::Exception::Type::InvalidResignAction );
long RTI::InvalidRetractionHandle::type = static_cast<long>(certi::Exception::Type::InvalidRetractionHandle );
long RTI::InvalidTransportationHandle::type = static_cast<long>(certi::Exception::Type::InvalidTransportationHandle );
long RTI::MemoryExhausted::type = static_cast<long>(certi::Exception::Type::MemoryExhausted );
long RTI::NameNotFound::type = static_cast<long>(certi::Exception::Type::NameNotFound );
long RTI::ObjectClassNotDefined::type = static_cast<long>(certi::Exception::Type::ObjectClassNotDefined );
long RTI::ObjectClassNotKnown::type = static_cast<long>(certi::Exception::Type::ObjectClassNotKnown );
long RTI::ObjectClassNotPublished::type = static_cast<long>(certi::Exception::Type::ObjectClassNotPublished );
long RTI::ObjectClassNotSubscribed::type = static_cast<long>(certi::Exception::Type::ObjectClassNotSubscribed );
long RTI::ObjectNotKnown::type = static_cast<long>(certi::Exception::Type::ObjectNotKnown );
long RTI::ObjectAlreadyRegistered::type = static_cast<long>(certi::Exception::Type::ObjectAlreadyRegistered );
long RTI::OwnershipAcquisitionPending::type = static_cast<long>(certi::Exception::Type::OwnershipAcquisitionPending );
long RTI::RegionInUse::type = static_cast<long>(certi::Exception::Type::RegionInUse );
long RTI::RegionNotKnown::type = static_cast<long>(certi::Exception::Type::RegionNotKnown );
long RTI::RestoreInProgress::type = static_cast<long>(certi::Exception::Type::RestoreInProgress );
long RTI::RestoreNotRequested::type = static_cast<long>(certi::Exception::Type::RestoreNotRequested );
long RTI::RTIinternalError::type = static_cast<long>(certi::Exception::Type::RTIinternalError );
long RTI::SpaceNotDefined::type = static_cast<long>(certi::Exception::Type::SpaceNotDefined );
long RTI::SaveInProgress::type = static_cast<long>(certi::Exception::Type::SaveInProgress );
long RTI::SaveNotInitiated::type = static_cast<long>(certi::Exception::Type::SaveNotInitiated );
long RTI::SpecifiedSaveLabelDoesNotExist::type = static_cast<long>(certi::Exception::Type::SpecifiedSaveLabelDoesNotExist );
long RTI::SynchronizationPointLabelWasNotAnnounced::type = static_cast<long>(certi::Exception::Type::SynchronizationPointLabelWasNotAnnounced );
long RTI::TimeAdvanceAlreadyInProgress::type = static_cast<long>(certi::Exception::Type::TimeAdvanceAlreadyInProgress );
long RTI::TimeAdvanceWasNotInProgress::type = static_cast<long>(certi::Exception::Type::TimeAdvanceWasNotInProgress );
long RTI::TimeConstrainedAlreadyEnabled::type = static_cast<long>(certi::Exception::Type::TimeConstrainedAlreadyEnabled );
long RTI::TimeConstrainedWasNotEnabled::type = static_cast<long>(certi::Exception::Type::TimeConstrainedWasNotEnabled );
long RTI::TimeRegulationAlreadyEnabled::type = static_cast<long>(certi::Exception::Type::TimeRegulationAlreadyEnabled );
long RTI::TimeRegulationWasNotEnabled::type = static_cast<long>(certi::Exception::Type::TimeRegulationWasNotEnabled );
long RTI::UnableToPerformSave::type = static_cast<long>(certi::Exception::Type::UnableToPerformSave );
long RTI::ValueCountExceeded::type = static_cast<long>(certi::Exception::Type::ValueCountExceeded );
long RTI::ValueLengthExceeded::type = static_cast<long>(certi::Exception::Type::ValueLengthExceeded);

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
