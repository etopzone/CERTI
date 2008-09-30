/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_exception.h
***********************************************************************/
#ifndefRTI_exception_h
#defineRTI_exception_h
#include <RTI_SpecificConfig.h>
// The RTI_exception class follows the interface of the C++ standard exception
// class.The key method, what, returns a null terminated character string that
// describes details of the exception that has occured.
class RTI_exception
{
public:
   RTI_exception();
   RTI_exception(RTI_exception const & rhs);
   RTI_exception &
   operator=(RTI_exception const & rhs);
   virtual
   ~RTI_exception()
     throw();
   virtual
   char const *
   what() const
     throw() = 0;
};
#include <RTI_string>
#define RTI_EXCEPTION(A) class A : public RTI_exception { public: A(RTI_string
const & message) throw(); A(A const & rhs) throw(); A & operator=(A const &)
throw(); virtual ~A() throw(); virtual char const * what() const throw(); pri-
vate: RTI_string _msg; };
RTI_EXCEPTION(RTI_ArrayIndexOutOfBounds)
RTI_EXCEPTION(RTI_AsynchronousDeliveryAlreadyDisabled)
RTI_EXCEPTION(RTI_AsynchronousDeliveryAlreadyEnabled)
RTI_EXCEPTION(RTI_AttributeAcquisitionWasNotCanceled)
RTI_EXCEPTION(RTI_AttributeAcquisitionWasNotRequested)
RTI_EXCEPTION(RTI_AttributeAlreadyBeingAcquired)
RTI_EXCEPTION(RTI_AttributeAlreadyBeingDivested)
RTI_EXCEPTION(RTI_AttributeAlreadyOwned)
RTI_EXCEPTION(RTI_AttributeDivestitureWasNotRequested)
RTI_EXCEPTION(RTI_AttributeNotDefined)
RTI_EXCEPTION(RTI_AttributeNotOwned)
RTI_EXCEPTION(RTI_AttributeNotPublished)
RTI_EXCEPTION(RTI_AttributeNotRecognized)
RTI_EXCEPTION(RTI_AttributeNotSubscribed)
RTI_EXCEPTION(RTI_AttributeRelevanceAdvisorySwitchIsOff)
RTI_EXCEPTION(RTI_AttributeRelevanceAdvisorySwitchIsOn)
RTI_EXCEPTION(RTI_AttributeScopeAdvisorySwitchIsOff)
RTI_EXCEPTION(RTI_AttributeScopeAdvisorySwitchIsOn)
RTI_EXCEPTION(RTI_CouldNotDecode)
RTI_EXCEPTION(RTI_CouldNotDiscover)
RTI_EXCEPTION(RTI_CouldNotOpenFDD)
RTI_EXCEPTION(RTI_CouldNotInitiateRestore)
RTI_EXCEPTION(RTI_DeletePrivilegeNotHeld)
RTI_EXCEPTION(RTI_RequestForTimeConstrainedPending)
RTI_EXCEPTION(RTI_NoRequestToEnableTimeConstrainedWasPending)
RTI_EXCEPTION(RTI_RequestForTimeRegulationPending)
RTI_EXCEPTION(RTI_NoRequestToEnableTimeRegulationWasPending)
RTI_EXCEPTION(RTI_ErrorReadingFDD)
RTI_EXCEPTION(RTI_FederateAlreadyExecutionMember)
RTI_EXCEPTION(RTI_FederateHasNotBegunSave)
RTI_EXCEPTION(RTI_FederateInternalError)
RTI_EXCEPTION(RTI_FederateNotExecutionMember)
RTI_EXCEPTION(RTI_FederateOwnsAttributes)
RTI_EXCEPTION(RTI_FederateServiceInvocationsAreBeingReportedViaMOM)
RTI_EXCEPTION(RTI_FederateUnableToUseTime)
RTI_EXCEPTION(RTI_FederatesCurrentlyJoined)
RTI_EXCEPTION(RTI_FederationExecutionAlreadyExists)
RTI_EXCEPTION(RTI_FederationExecutionDoesNotExist)
RTI_EXCEPTION(RTI_IllegalName)
RTI_EXCEPTION(RTI_IllegalTimeArithmetic)
RTI_EXCEPTION(RTI_InteractionClassNotDefined)
RTI_EXCEPTION(RTI_InteractionClassNotPublished)
RTI_EXCEPTION(RTI_InteractionClassNotRecognized)
RTI_EXCEPTION(RTI_InteractionClassNotSubscribed)
RTI_EXCEPTION(RTI_InteractionParameterNotDefined)
RTI_EXCEPTION(RTI_InteractionParameterNotRecognized)
RTI_EXCEPTION(RTI_InteractionRelevanceAdvisorySwitchIsOff)
RTI_EXCEPTION(RTI_InteractionRelevanceAdvisorySwitchIsOn)
RTI_EXCEPTION(RTI_InTimeAdvancingState)
RTI_EXCEPTION(RTI_InvalidAttributeHandle)
RTI_EXCEPTION(RTI_InvalidDimensionHandle)
RTI_EXCEPTION(RTI_InvalidInteractionClassHandle)
RTI_EXCEPTION(RTI_InvalidLogicalTime)
RTI_EXCEPTION(RTI_InvalidLogicalTimeInterval)
RTI_EXCEPTION(RTI_InvalidLookahead)
RTI_EXCEPTION(RTI_InvalidObjectClassHandle)
RTI_EXCEPTION(RTI_InvalidOrderName)
RTI_EXCEPTION(RTI_InvalidOrderType)
RTI_EXCEPTION(RTI_InvalidParameterHandle)
RTI_EXCEPTION(RTI_InvalidRangeBounds)
RTI_EXCEPTION(RTI_InvalidRegion)
RTI_EXCEPTION(RTI_InvalidRegionContext)
RTI_EXCEPTION(RTI_InvalidRetractionHandle)
RTI_EXCEPTION(RTI_InvalidString)
RTI_EXCEPTION(RTI_InvalidTransportationName)
RTI_EXCEPTION(RTI_InvalidTransportationType)
RTI_EXCEPTION(RTI_JoinedFederateIsNotInTimeAdvancingState)
RTI_EXCEPTION(RTI_LogicalTimeAlreadyPassed)
RTI_EXCEPTION(RTI_LowerBoundOutOfRange)
RTI_EXCEPTION(RTI_MessageCanNoLongerBeRetracted)
RTI_EXCEPTION(RTI_NameNotFound)
RTI_EXCEPTION(RTI_ObjectClassNotDefined)
RTI_EXCEPTION(RTI_ObjectClassNotKnown)
RTI_EXCEPTION(RTI_ObjectClassNotPublished)
RTI_EXCEPTION(RTI_ObjectClassNotSubscribed)
RTI_EXCEPTION(RTI_ObjectClassRelevanceAdvisorySwitchIsOff)
RTI_EXCEPTION(RTI_ObjectClassRelevanceAdvisorySwitchIsOn)
RTI_EXCEPTION(RTI_ObjectInstanceNameInUse)
RTI_EXCEPTION(RTI_ObjectInstanceNameNotReserved)
RTI_EXCEPTION(RTI_ObjectInstanceNotKnown)
RTI_EXCEPTION(RTI_OwnershipAcquisitionPending)
RTI_EXCEPTION(RTI_RTIinternalError)
RTI_EXCEPTION(RTI_RegionDoesNotContainSpecifiedDimension)
RTI_EXCEPTION(RTI_RegionInUseForUpdateOrSubscription)
RTI_EXCEPTION(RTI_RegionNotCreatedByThisFederate)
RTI_EXCEPTION(RTI_RestoreInProgress)
RTI_EXCEPTION(RTI_RestoreNotInProgress)
RTI_EXCEPTION(RTI_RestoreNotRequested)
RTI_EXCEPTION(RTI_SaveInProgress)
RTI_EXCEPTION(RTI_SaveNotInitiated)
RTI_EXCEPTION(RTI_SaveNotInProgress)
RTI_EXCEPTION(RTI_SpecifiedSaveLabelDoesNotExist)
RTI_EXCEPTION(RTI_SynchronizationPointLabelNotAnnounced)
RTI_EXCEPTION(RTI_SynchronizationSetMemberNotJoined)
RTI_EXCEPTION(RTI_TimeConstrainedAlreadyEnabled)
RTI_EXCEPTION(RTI_TimeConstrainedIsNotEnabled)
RTI_EXCEPTION(RTI_TimeRegulationAlreadyEnabled)
RTI_EXCEPTION(RTI_TimeRegulationIsNotEnabled)
RTI_EXCEPTION(RTI_UnableToPerformSave)
RTI_EXCEPTION(RTI_UnknownName)
RTI_EXCEPTION(RTI_UpperBoundOutOfRange)
RTI_EXCEPTION(RTI_ValueCountExceeded)
RTI_EXCEPTION(RTI_ValueLengthExceeded)
#undef RTI_EXCEPTION
#ifdef RTI_USE_INLINE
#include "RTI_exception.i"
#endif // RTI_USE_INLINE
#endif // RTI_exception_h
