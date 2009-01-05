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
// $Id: Exception.cc,v 3.17.4.1 2009/01/05 13:34:50 gotthardp Exp $
// ----------------------------------------------------------------------------

#include "Exception.hh"
#include "certi.hh"
#include "PrettyDebug.hh"
#include <sstream>

using namespace certi;

// static members for HLA Exceptions
long ArrayIndexOutOfBounds::_type = e_ArrayIndexOutOfBounds ;
long AsynchronousDeliveryAlreadyDisabled::_type = e_AsynchronousDeliveryAlreadyDisabled ;
long AsynchronousDeliveryAlreadyEnabled::_type = e_AsynchronousDeliveryAlreadyEnabled ;
long AttributeAcquisitionWasNotRequested::_type = e_AttributeAcquisitionWasNotRequested ;
long AttributeAcquisitionWasNotCanceled::_type = e_AttributeAcquisitionWasNotCanceled ;
long AttributeAlreadyBeingAcquired::_type = e_AttributeAlreadyBeingAcquired ;
long AttributeAlreadyBeingDivested::_type = e_AttributeAlreadyBeingDivested ;
long AttributeAlreadyOwned::_type = e_AttributeAlreadyOwned ;
long AttributeDivestitureWasNotRequested::_type = e_AttributeDivestitureWasNotRequested ;
long AttributeNotDefined::_type = e_AttributeNotDefined ;
long AttributeNotKnown::_type = e_AttributeNotKnown ;
long AttributeNotOwned::_type = e_AttributeNotOwned ;
long AttributeNotPublished::_type = e_AttributeNotPublished ;
long ConcurrentAccessAttempted::_type = e_ConcurrentAccessAttempted ;
long CouldNotDiscover::_type = e_CouldNotDiscover ;
long CouldNotOpenFED::_type = e_CouldNotOpenFED ;
long CouldNotRestore::_type = e_CouldNotRestore ;
long DeletePrivilegeNotHeld::_type = e_DeletePrivilegeNotHeld ;
long DimensionNotDefined::_type = e_DimensionNotDefined ;
long EnableTimeConstrainedPending::_type = e_EnableTimeConstrainedPending ;
long EnableTimeConstrainedWasNotPending::_type = e_EnableTimeConstrainedWasNotPending ;
long EnableTimeRegulationPending::_type = e_EnableTimeRegulationPending ;
long EnableTimeRegulationWasNotPending::_type = e_EnableTimeRegulationWasNotPending ;
long ErrorReadingFED::_type = e_ErrorReadingFED ;
long EventNotKnown::_type = e_EventNotKnown ;
long FederateAlreadyExecutionMember::_type = e_FederateAlreadyExecutionMember ;
long FederateInternalError::_type = e_FederateInternalError ;
long FederateLoggingServiceCalls::_type = e_FederateLoggingServiceCalls ;
long FederateNotExecutionMember::_type = e_FederateNotExecutionMember ;
long FederateOwnsAttributes::_type = e_FederateOwnsAttributes ;
long FederateWasNotAskedToReleaseAttribute::_type = e_FederateWasNotAskedToReleaseAttribute ;
long FederatesCurrentlyJoined::_type = e_FederatesCurrentlyJoined ;
long FederationExecutionAlreadyExists::_type = e_FederationExecutionAlreadyExists ;
long FederationExecutionDoesNotExist::_type = e_FederationExecutionDoesNotExist ;
long FederationTimeAlreadyPassed::_type = e_FederationTimeAlreadyPassed ;
long HandleValuePairMaximumExceeded::_type = e_HandleValuePairMaximumExceeded ;
long InteractionClassNotDefined::_type = e_InteractionClassNotDefined ;
long InteractionClassNotKnown::_type = e_InteractionClassNotKnown ;
long InteractionClassNotPublished::_type = e_InteractionClassNotPublished ;
long InteractionClassNotSubscribed::_type = e_InteractionClassNotSubscribed ;
long InteractionParameterNotDefined::_type = e_InteractionParameterNotDefined ;
long InteractionParameterNotKnown::_type = e_InteractionParameterNotKnown ;
long InvalidExtents::_type = e_InvalidExtents ;
long InvalidFederationTime::_type = e_InvalidFederationTime ;
long InvalidHandleValuePairSetContext::_type = e_InvalidHandleValuePairSetContext ;
long InvalidLookahead::_type = e_InvalidLookahead ;
long InvalidOrderingHandle::_type = e_InvalidOrderingHandle ;
long InvalidRegionContext::_type = e_InvalidRegionContext ;
long InvalidResignAction::_type = e_InvalidResignAction ;
long InvalidRetractionHandle::_type = e_InvalidRetractionHandle ;
long InvalidTransportationHandle::_type = e_InvalidTransportationHandle ;
long MemoryExhausted::_type = e_MemoryExhausted ;
long NameNotFound::_type = e_NameNotFound ;
long ObjectClassNotDefined::_type = e_ObjectClassNotDefined ;
long ObjectClassNotKnown::_type = e_ObjectClassNotKnown ;
long ObjectClassNotPublished::_type = e_ObjectClassNotPublished ;
long ObjectClassNotSubscribed::_type = e_ObjectClassNotSubscribed ;
long ObjectNotKnown::_type = e_ObjectNotKnown ;
long ObjectAlreadyRegistered::_type = e_ObjectAlreadyRegistered ;
long OwnershipAcquisitionPending::_type = e_OwnershipAcquisitionPending ;
long RegionInUse::_type = e_RegionInUse ;
long RegionNotKnown::_type = e_RegionNotKnown ;
long RestoreInProgress::_type = e_RestoreInProgress ;
long RestoreNotRequested::_type = e_RestoreNotRequested ;
long RTIinternalError::_type = e_RTIinternalError ;
long SpaceNotDefined::_type = e_SpaceNotDefined ;
long SaveInProgress::_type = e_SaveInProgress ;
long SaveNotInitiated::_type = e_SaveNotInitiated ;
long SpecifiedSaveLabelDoesNotExist::_type = e_SpecifiedSaveLabelDoesNotExist ;
long SynchronizationPointLabelWasNotAnnounced::_type = e_SynchronizationPointLabelWasNotAnnounced ;
long TimeAdvanceAlreadyInProgress::_type = e_TimeAdvanceAlreadyInProgress ;
long TimeAdvanceWasNotInProgress::_type = e_TimeAdvanceWasNotInProgress ;
long TimeConstrainedAlreadyEnabled::_type = e_TimeConstrainedAlreadyEnabled ;
long TimeConstrainedWasNotEnabled::_type = e_TimeConstrainedWasNotEnabled ;
long TimeRegulationAlreadyEnabled::_type = e_TimeRegulationAlreadyEnabled ;
long TimeRegulationWasNotEnabled::_type = e_TimeRegulationWasNotEnabled ;
long UnableToPerformSave::_type = e_UnableToPerformSave ;
long ValueCountExceeded::_type = e_ValueCountExceeded ;
long ValueLengthExceeded::_type = e_ValueLengthExceeded ;

//  TypeException managing (how to obtain TypeException from Exception name ?)
long FederateNotPublishing::_type = e_FederateNotPublishing ;
long FederateNotSubscribing::_type = e_FederateNotSubscribing ;
long InvalidObjectHandle::_type = e_InvalidObjectHandle ;
long CouldNotOpenRID::_type = e_CouldNotOpenRID ;
long ErrorReadingRID::_type = e_ErrorReadingRID ;
long AttributeNotSubscribed::_type = e_AttributeNotSubscribed ;
long FederationAlreadyPaused::_type = e_FederationAlreadyPaused ;
long FederationNotPaused::_type = e_FederationNotPaused ;
long SecurityError::_type = e_SecurityError ;
long FederateAlreadyPaused::_type = e_FederateAlreadyPaused ;
long FederateDoesNotExist::_type = e_FederateDoesNotExist ;
long FederateNameAlreadyInUse::_type = e_FederateNameAlreadyInUse ;
long FederateNotPaused::_type = e_FederateNotPaused ;
long IDsupplyExhausted::_type = e_IDsupplyExhausted ;
long InvalidDivestitureCondition::_type = e_InvalidDivestitureCondition ;
long InvalidFederationTimeDelta::_type = e_InvalidFederationTimeDelta ;
long InvalidRoutingSpace::_type = e_InvalidRoutingSpace ;
long NoPauseRequested::_type = e_NoPauseRequested ;
long NoResumeRequested::_type = e_NoResumeRequested ;
long TooManyIDsRequested::_type = e_TooManyIDsRequested ;
long UnimplementedService::_type = e_UnimplementedService ;
long UnknownLabel::_type = e_UnknownLabel ;
long NetworkError::_type = e_NetworkError ;
long NetworkSignal::_type = e_NetworkSignal ;

static PrettyDebug PD_Exception("CERTI_EXCEPTION",__FILE__);

const std::string Exception::displayMe() const
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

    PD_Exception[pdExcept] << msg.str().c_str();
    return msg.str();
}
// $Id: Exception.cc,v 3.17.4.1 2009/01/05 13:34:50 gotthardp Exp $
