// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2014  ONERA
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
// ----------------------------------------------------------------------------

#include <RTI/Exception.h>
#include <iostream>
#include <sstream>

namespace rti1516e
{
Exception::Exception()
{
}

Exception::Exception(Exception const & rhs)
{
    // Nothing to copy...?
}

Exception::~Exception()
{
}

Exception &
Exception::operator=(Exception const & rhs)
{
    // Nothing to copy...?
    return *this;
}

std::wostream &
operator << (std::wostream &stream, Exception const &e)
{
    return stream << e.what() << std::endl;
}


#define RTI_EXCEPTION_IMPL(A)                     \
        A::A(std::wstring const & message) throw()    \
        : Exception()                                 \
          , _msg(message)                               \
          { }                                           \
          std::wstring A::what() const throw()          \
          {                                             \
              std::wstringstream ss;                    \
              ss << "rti1516e::" #A ": " << _msg;        \
              return ss.str();                          \
          }

RTI_EXCEPTION_IMPL(AlreadyConnected)
RTI_EXCEPTION_IMPL(AsynchronousDeliveryAlreadyDisabled)
RTI_EXCEPTION_IMPL(AsynchronousDeliveryAlreadyEnabled)
RTI_EXCEPTION_IMPL(AttributeAcquisitionWasNotCanceled)
RTI_EXCEPTION_IMPL(AttributeAcquisitionWasNotRequested)
RTI_EXCEPTION_IMPL(AttributeAlreadyBeingAcquired)
RTI_EXCEPTION_IMPL(AttributeAlreadyBeingChanged)
RTI_EXCEPTION_IMPL(AttributeAlreadyBeingDivested)
RTI_EXCEPTION_IMPL(AttributeAlreadyOwned)
RTI_EXCEPTION_IMPL(AttributeDivestitureWasNotRequested)
RTI_EXCEPTION_IMPL(AttributeNotDefined)
RTI_EXCEPTION_IMPL(AttributeNotOwned)
RTI_EXCEPTION_IMPL(AttributeNotPublished)
RTI_EXCEPTION_IMPL(AttributeNotRecognized)
RTI_EXCEPTION_IMPL(AttributeNotSubscribed)
RTI_EXCEPTION_IMPL(AttributeRelevanceAdvisorySwitchIsOff)
RTI_EXCEPTION_IMPL(AttributeRelevanceAdvisorySwitchIsOn)
RTI_EXCEPTION_IMPL(AttributeScopeAdvisorySwitchIsOff)
RTI_EXCEPTION_IMPL(AttributeScopeAdvisorySwitchIsOn)
RTI_EXCEPTION_IMPL(BadInitializationParameter)
RTI_EXCEPTION_IMPL(CallNotAllowedFromWithinCallback)
RTI_EXCEPTION_IMPL(ConnectionFailed)
RTI_EXCEPTION_IMPL(CouldNotCreateLogicalTimeFactory)
RTI_EXCEPTION_IMPL(CouldNotDecode)
RTI_EXCEPTION_IMPL(CouldNotDiscover)
RTI_EXCEPTION_IMPL(CouldNotEncode)
RTI_EXCEPTION_IMPL(CouldNotOpenFDD)
RTI_EXCEPTION_IMPL(CouldNotOpenMIM)
RTI_EXCEPTION_IMPL(CouldNotInitiateRestore)
RTI_EXCEPTION_IMPL(DeletePrivilegeNotHeld)
RTI_EXCEPTION_IMPL(DesignatorIsHLAstandardMIM)
RTI_EXCEPTION_IMPL(RequestForTimeConstrainedPending)
RTI_EXCEPTION_IMPL(NoRequestToEnableTimeConstrainedWasPending)
RTI_EXCEPTION_IMPL(RequestForTimeRegulationPending)
RTI_EXCEPTION_IMPL(NoRequestToEnableTimeRegulationWasPending)
RTI_EXCEPTION_IMPL(NoFederateWillingToAcquireAttribute)
RTI_EXCEPTION_IMPL(ErrorReadingFDD)
RTI_EXCEPTION_IMPL(ErrorReadingMIM)
RTI_EXCEPTION_IMPL(FederateAlreadyExecutionMember)
RTI_EXCEPTION_IMPL(FederateHandleNotKnown)
RTI_EXCEPTION_IMPL(FederateHasNotBegunSave)
RTI_EXCEPTION_IMPL(FederateInternalError)
RTI_EXCEPTION_IMPL(FederateIsExecutionMember)
RTI_EXCEPTION_IMPL(FederateNameAlreadyInUse)
RTI_EXCEPTION_IMPL(FederateNotExecutionMember)
RTI_EXCEPTION_IMPL(FederateOwnsAttributes)
RTI_EXCEPTION_IMPL(FederateServiceInvocationsAreBeingReportedViaMOM)
RTI_EXCEPTION_IMPL(FederateUnableToUseTime)
RTI_EXCEPTION_IMPL(FederatesCurrentlyJoined)
RTI_EXCEPTION_IMPL(FederationExecutionAlreadyExists)
RTI_EXCEPTION_IMPL(FederationExecutionDoesNotExist)
RTI_EXCEPTION_IMPL(IllegalName)
RTI_EXCEPTION_IMPL(IllegalTimeArithmetic)
RTI_EXCEPTION_IMPL(InconsistentFDD)
RTI_EXCEPTION_IMPL(InteractionClassAlreadyBeingChanged)
RTI_EXCEPTION_IMPL(InteractionClassNotDefined)
RTI_EXCEPTION_IMPL(InteractionClassNotPublished)
RTI_EXCEPTION_IMPL(InteractionClassNotRecognized)
RTI_EXCEPTION_IMPL(InteractionClassNotSubscribed)
RTI_EXCEPTION_IMPL(InteractionParameterNotDefined)
RTI_EXCEPTION_IMPL(InteractionParameterNotRecognized)
RTI_EXCEPTION_IMPL(InteractionRelevanceAdvisorySwitchIsOff)
RTI_EXCEPTION_IMPL(InteractionRelevanceAdvisorySwitchIsOn)
RTI_EXCEPTION_IMPL(InTimeAdvancingState)
RTI_EXCEPTION_IMPL(InvalidAttributeHandle)
RTI_EXCEPTION_IMPL(InvalidDimensionHandle)
RTI_EXCEPTION_IMPL(InvalidFederateHandle)
RTI_EXCEPTION_IMPL(InvalidInteractionClassHandle)
RTI_EXCEPTION_IMPL(InvalidLocalSettingsDesignator)
RTI_EXCEPTION_IMPL(InvalidLogicalTime)
RTI_EXCEPTION_IMPL(InvalidLogicalTimeInterval)
RTI_EXCEPTION_IMPL(InvalidLookahead)
RTI_EXCEPTION_IMPL(InvalidObjectClassHandle)
RTI_EXCEPTION_IMPL(InvalidOrderName)
RTI_EXCEPTION_IMPL(InvalidOrderType)
RTI_EXCEPTION_IMPL(InvalidParameterHandle)
RTI_EXCEPTION_IMPL(InvalidRangeBound)
RTI_EXCEPTION_IMPL(InvalidRegion)
RTI_EXCEPTION_IMPL(InvalidResignAction)
RTI_EXCEPTION_IMPL(InvalidRegionContext)
RTI_EXCEPTION_IMPL(InvalidMessageRetractionHandle)
RTI_EXCEPTION_IMPL(InvalidServiceGroup)
RTI_EXCEPTION_IMPL(InvalidTransportationName)
RTI_EXCEPTION_IMPL(InvalidTransportationType)
RTI_EXCEPTION_IMPL(InvalidUpdateRateDesignator)
RTI_EXCEPTION_IMPL(JoinedFederateIsNotInTimeAdvancingState)
RTI_EXCEPTION_IMPL(LogicalTimeAlreadyPassed)
RTI_EXCEPTION_IMPL(MessageCanNoLongerBeRetracted)
RTI_EXCEPTION_IMPL(NameNotFound)
RTI_EXCEPTION_IMPL(NameSetWasEmpty)
RTI_EXCEPTION_IMPL(NoAcquisitionPending)
RTI_EXCEPTION_IMPL(NotConnected)
RTI_EXCEPTION_IMPL(ObjectClassNotDefined)
RTI_EXCEPTION_IMPL(ObjectClassNotKnown)
RTI_EXCEPTION_IMPL(ObjectClassNotPublished)
RTI_EXCEPTION_IMPL(ObjectClassRelevanceAdvisorySwitchIsOff)
RTI_EXCEPTION_IMPL(ObjectClassRelevanceAdvisorySwitchIsOn)
RTI_EXCEPTION_IMPL(ObjectInstanceNameInUse)
RTI_EXCEPTION_IMPL(ObjectInstanceNameNotReserved)
RTI_EXCEPTION_IMPL(ObjectInstanceNotKnown)
RTI_EXCEPTION_IMPL(OwnershipAcquisitionPending)
RTI_EXCEPTION_IMPL(RTIinternalError)
RTI_EXCEPTION_IMPL(RegionDoesNotContainSpecifiedDimension)
RTI_EXCEPTION_IMPL(RegionInUseForUpdateOrSubscription)
RTI_EXCEPTION_IMPL(RegionNotCreatedByThisFederate)
RTI_EXCEPTION_IMPL(RestoreInProgress)
RTI_EXCEPTION_IMPL(RestoreNotInProgress)
RTI_EXCEPTION_IMPL(RestoreNotRequested)
RTI_EXCEPTION_IMPL(SaveInProgress)
RTI_EXCEPTION_IMPL(SaveNotInProgress)
RTI_EXCEPTION_IMPL(SaveNotInitiated)
RTI_EXCEPTION_IMPL(SpecifiedSaveLabelDoesNotExist)
RTI_EXCEPTION_IMPL(SynchronizationPointLabelNotAnnounced)
RTI_EXCEPTION_IMPL(TimeConstrainedAlreadyEnabled)
RTI_EXCEPTION_IMPL(TimeConstrainedIsNotEnabled)
RTI_EXCEPTION_IMPL(TimeRegulationAlreadyEnabled)
RTI_EXCEPTION_IMPL(TimeRegulationIsNotEnabled)
RTI_EXCEPTION_IMPL(UnableToPerformSave)
RTI_EXCEPTION_IMPL(UnknownName)
RTI_EXCEPTION_IMPL(UnsupportedCallbackModel)
RTI_EXCEPTION_IMPL(InternalError)

#undef RTI_EXCEPTION_IMPL

} // end namespace rti1516e
