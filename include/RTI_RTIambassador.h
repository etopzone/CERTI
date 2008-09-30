/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_RTIambassador.h
***********************************************************************/
// This interface is used to access the services of the RTI.
#ifndef RTI_RTIambassador_h
#define RTI_RTIambassador_h
//
// These forward declarations significantly decrease compilation time when only
// including this file
//
class    RTI_AsynchronousDeliveryAlreadyDisabled;
class    RTI_AsynchronousDeliveryAlreadyEnabled;
class    RTI_AttributeAcquisitionWasNotRequested;
class    RTI_AttributeAlreadyBeingAcquired;
class    RTI_AttributeAlreadyBeingDivested;
class    RTI_AttributeAlreadyOwned;
class    RTI_AttributeDivestitureWasNotRequested;
class    RTI_AttributeNotDefined;
class    RTI_AttributeNotOwned;
class    RTI_AttributeNotPublished;
class    RTI_AttributeRelevanceAdvisorySwitchIsOff;
class    RTI_AttributeRelevanceAdvisorySwitchIsOn;
class    RTI_AttributeScopeAdvisorySwitchIsOff;
class    RTI_AttributeScopeAdvisorySwitchIsOn;
class    RTI_CouldNotOpenFDD;
class    RTI_DeletePrivilegeNotHeld;
class    RTI_ErrorReadingFDD;
class    RTI_FederateAlreadyExecutionMember;
class    RTI_FederateAmbassador;
class    RTI_FederateHasNotBegunSave;
class    RTI_FederateNotExecutionMember;
class    RTI_FederateOwnsAttributes;
class    RTI_FederateServiceInvocationsAreBeingReportedViaMOM;
class    RTI_FederateUnableToUseTime;
class    RTI_FederatesCurrentlyJoined;
class    RTI_FederationExecutionAlreadyExists;
class    RTI_FederationExecutionDoesNotExist ;
class    RTI_FederationExecutionDoesNotExist;
class    RTI_IllegalName;
class    RTI_InTimeAdvancingState;
class    RTI_InteractionClassNotDefined;
class    RTI_InteractionClassNotPublished;
class    RTI_InteractionParameterNotDefined;
class    RTI_InteractionRelevanceAdvisorySwitchIsOff;
class    RTI_InteractionRelevanceAdvisorySwitchIsOn;
 class   RTI_InvalidAttributeHandle;
 class   RTI_InvalidDimensionHandle;
 class   RTI_InvalidInteractionClassHandle;
 class   RTI_InvalidLogicalTime;
 class   RTI_InvalidLookahead;
 class   RTI_InvalidObjectClassHandle;
 class   RTI_InvalidOrderName;
 class   RTI_InvalidOrderType;
 class   RTI_InvalidParameterHandle;
 class   RTI_InvalidRangeBounds;
 class   RTI_InvalidRegion;
 class   RTI_InvalidRegionContext;
 class   RTI_InvalidRetractionHandle;
 class   RTI_InvalidTransportationName;
 class   RTI_InvalidTransportationType;
 class   RTI_LogicalTime;
 class   RTI_LogicalTimeAlreadyPassed ;
 class   RTI_LogicalTimeAlreadyPassed;
 class   RTI_LogicalTimeFactory;
 class   RTI_LogicalTimeInterval;
 class   RTI_LogicalTimeIntervalFactory;
 class   RTI_MessageCanNoLongerBeRetracted;
 class   RTI_NameNotFound;
 class   RTI_ObjectClassNotDefined ;
 class   RTI_ObjectClassNotDefined;
 class   RTI_ObjectClassNotPublished;
 class   RTI_ObjectClassRelevanceAdvisorySwitchIsOff;
 class   RTI_ObjectClassRelevanceAdvisorySwitchIsOn;
 class   RTI_ObjectInstanceNameInUse;
 class   RTI_ObjectInstanceNameNotReserved;
 class   RTI_ObjectInstanceNotKnown ;
 class   RTI_ObjectInstanceNotKnown;
 class   RTI_OrderType;
 class   RTI_OwnershipAcquisitionPending;
 class   RTI_RTIinternalError;
 class   RTI_RangeBounds;
 class   RTI_RegionDoesNotContainSpecifiedDimension;
 class   RTI_RegionInUseForUpdateOrSubscription;
 class   RTI_RegionNotCreatedByThisFederate;
 class   RTI_RequestForTimeConstrainedPending;
 class   RTI_RequestForTimeConstrainedPending;
 class   RTI_RequestForTimeRegulationPending;
 class   RTI_ResignAction;
 class   RTI_RestoreInProgress;
 class   RTI_RestoreNotInProgress;
 class   RTI_RestoreNotRequested;
 class   RTI_RestoreStatus;
 class   RTI_SaveInProgress;
 class   RTI_SaveNotInProgress;
 class   RTI_SaveNotInitiated;
 class   RTI_SaveStatus;
 class   RTI_ServiceGroupIndicator;
 class   RTI_SynchronizationPointLabelNotAnnounced;
 class   RTI_TimeConstrainedAlreadyEnabled;
 class   RTI_TimeConstrainedIsNotEnabled;
 class   RTI_TimeRegulationAlreadyEnabled;
 class   RTI_TimeRegulationIsNotEnabled;
 class   RTI_TransportationType;
 #include    <RTI_SpecificConfig.h>
 #include    <RTI_memory>
 #include    <RTI_string>
 #include    <RTI_bool.h>
 #include    <RTI_Typedefs.h>
 class RTI_RTIambassador
 {
 public:
   // 10.37
   virtual ~RTI_RTIambassador ()
    throw ();
 // 4.2
  virtual void createFederationExecution
  (RTI_wstring const & federationExecutionName,
   RTI_wstring const & fullPathNameToTheFDDfile)
    throw (RTI_FederationExecutionAlreadyExists,
              RTI_CouldNotOpenFDD,
              RTI_ErrorReadingFDD,
              RTI_RTIinternalError) = 0;
  // 4.3
  virtual void destroyFederationExecution
  (RTI_wstring const & federationExecutionName)
    throw (RTI_FederatesCurrentlyJoined,
              RTI_FederationExecutionDoesNotExist,
              RTI_RTIinternalError) = 0;
  // 4.4
  virtualRTI_FederateHandle const & joinFederationExecution
  (RTI_wstring const & federateType,
   RTI_wstring const & federationExecutionName,
   RTI_auto_ptr< RTI_FederateAmbassador > federateAmbassador,
   RTI_auto_ptr< RTI_LogicalTimeFactory > logicalTimeFactory,
   RTI_auto_ptr< RTI_LogicalTimeIntervalFactory > logicalTimeIntervalFactory)
    throw (RTI_FederateAlreadyExecutionMember,
              RTI_FederationExecutionDoesNotExist,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 4.5
  virtual void resignFederationExecution
  (RTI_ResignAction resignAction)
    throw (RTI_OwnershipAcquisitionPending,
              RTI_FederateOwnsAttributes,
              RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 4.6
  virtual void registerFederationSynchronizationPoint
  (RTI_wstringconst & label,
   RTI_UserSuppliedTag const & theUserSuppliedTag)
    throw (RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  virtual void registerFederationSynchronizationPoint
  (RTI_wstring const & label,
   RTI_UserSuppliedTag const & theUserSuppliedTag,
   RTI_FederateHandleSet const & syncSet)
    throw (RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 4.9
  virtual void synchronizationPointAchieved
  (RTI_wstring const & label)
    throw (RTI_SynchronizationPointLabelNotAnnounced,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 4.11
  virtual void requestFederationSave
  (RTI_wstring const & label)
    throw (RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  virtual void requestFederationSave
  (RTI_wstring const & label,
    RTI_LogicalTime const & theTime)
     throw (RTI_LogicalTimeAlreadyPassed,
              RTI_InvalidLogicalTime,
       RTI_FederateUnableToUseTime,
            RTI_FederateNotExecutionMember,
            RTI_SaveInProgress,
            RTI_RestoreInProgress,
            RTI_RTIinternalError) = 0;
  // 4.13
  virtual void federateSaveBegun ()
     throw (RTI_SaveNotInitiated,
              RTI_FederateNotExecutionMember,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 4.14
  virtual void federateSaveComplete ()
     throw (RTI_FederateHasNotBegunSave,
              RTI_FederateNotExecutionMember,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  virtual void federateSaveNotComplete()
     throw (RTI_FederateHasNotBegunSave,
              RTI_FederateNotExecutionMember,
              RTI_RestoreInProgress,
              RTsI_RTIinternalError) = 0;
  // 4.16
  virtual void queryFederationSaveStatus ()
     throw (RTI_FederateNotExecutionMember,
              RTI_SaveNotInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 4.18
  virtual void requestFederationRestore
  (RTI_wstring const & label)
     throw (RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 4.22
  virtual void federateRestoreComplete ()
     throw (RTI_RestoreNotRequested,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RTIinternalError) = 0;
  virtual void federateRestoreNotComplete ()
     throw (RTI_RestoreNotRequested,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RTIinternalError) = 0;
  // 4.24
  virtual void queryFederationRestoreStatus ()
     throw (RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreNotInProgress,
              RTI_RTIinternalError) = 0;
  /////////////////////////////////////
  // Declaration Management Services //
  /////////////////////////////////////
  // 5.2
  virtual void publishObjectClassAttributes
  (RTI_ObjectClassHandleconst & theClass,
   RTI_AttributeHandleSet const & attributeList)
    throw (RTI_ObjectClassNotDefined,
              RTI_AttributeNotDefined,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 5.3
  virtual void unpublishObjectClass
  (RTI_ObjectClassHandle const & theClass)
    throw (RTI_ObjectClassNotDefined,
              RTI_OwnershipAcquisitionPending,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  virtual void unpublishObjectClassAttributes
  (RTI_ObjectClassHandleconst & theClass,
   RTI_AttributeHandleSet const & attributeList)
    throw (RTI_ObjectClassNotDefined,
              RTI_AttributeNotDefined,
              RTI_OwnershipAcquisitionPending,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 5.4
  virtual void publishInteractionClass
  (RTI_InteractionClassHandle const & theInteraction)
    throw (RTI_InteractionClassNotDefined,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 5.5
  virtual void unpublishInteractionClass
  (RTI_InteractionClassHandle const & theInteraction)
              throw (RTI_InteractionClassNotDefined,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 5.6
  virtual void subscribeObjectClassAttributes
  (RTI_ObjectClassHandleconst & theClass,
   RTI_AttributeHandleSet const & attributeList,
   RTI_bool active = RTI_true)
    throw (RTI_ObjectClassNotDefined,
              RTI_AttributeNotDefined,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 5.7
  virtual void unsubscribeObjectClass
  (RTI_ObjectClassHandle const & theClass)
    throw (RTI_ObjectClassNotDefined,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  virtual void unsubscribeObjectClassAttributes
  (RTI_ObjectClassHandleconst & theClass,
   RTI_AttributeHandleSet const & attributeList)
     throw (RTI_ObjectClassNotDefined,
              RTI_AttributeNotDefined,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 5.8
  virtual void subscribeInteractionClass
  (RTI_InteractionClassHandle const & theClass,
    RTI_bool active = RTI_true)
     throw (RTI_InteractionClassNotDefined,
              RTI_FederateServiceInvocationsAreBeingReportedViaMOM,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 5.9
  virtual void unsubscribeInteractionClass
  (RTI_InteractionClassHandle const & theClass)
     throw (RTI_InteractionClassNotDefined,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  ////////////////////////////////
  // Object Management Services //
  ////////////////////////////////
  // 6.2
  virtual void reserveObjectInstanceName
  (RTI_wstring const & theObjectInstanceName)
     throw (RTI_IllegalName,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 6.4
  virtual RTI_ObjectInstanceHandle const & registerObjectInstance
  (RTI_ObjectClassHandle const & theClass)
     throw (RTI_ObjectClassNotDefined,
              RTI_ObjectClassNotPublished,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  virtual RTI_ObjectInstanceHandle const & registerObjectInstance
  (RTI_ObjectClassHandle const & theClass,
    RTI_wstring const & theObjectInstanceName)
     throw (RTI_ObjectClassNotDefined,
              RTI_ObjectClassNotPublished,
              RTI_ObjectInstanceNameNotReserved,
              RTI_ObjectInstanceNameInUse,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 6.6
  virtual void updateAttributeValues
  (RTI_ObjectInstanceHandle const & theObject,
    RTI_AttributeHandleValueMap const & theAttributeValues,
    RTI_UserSuppliedTag const & theUserSuppliedTag)
     throw (RTI_ObjectInstanceNotKnown,
              RTI_AttributeNotDefined,
              RTI_AttributeNotOwned,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 virtual RTI_auto_ptr< RTI_MessageRetractionHandle > updateAttributeValues
 (RTI_ObjectInstanceHandle const & theObject,
   RTI_AttributeHandleValueMap const & theAttributeValues,
   RTI_UserSuppliedTag const & theUserSuppliedTag,
   RTI_LogicalTime const & theTime)
    throw (RTI_ObjectInstanceNotKnown,
              RTI_AttributeNotDefined,
              RTI_AttributeNotOwned,
              RTI_InvalidLogicalTime,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 6.8
 virtual void sendInteraction
 (RTI_InteractionClassHandleconst & theInteraction,
   RTI_ParameterHandleValueMap const & theParameterValues,
   RTI_UserSuppliedTag const & theUserSuppliedTag)
    throw (RTI_InteractionClassNotPublished,
              RTI_InteractionClassNotDefined,
              RTI_InteractionParameterNotDefined,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 virtual RTI_auto_ptr< RTI_MessageRetractionHandle > sendInteraction
 (RTI_InteractionClassHandleconst & theInteraction,
   RTI_ParameterHandleValueMap const & theParameterValues,
   RTI_UserSuppliedTag const & theUserSuppliedTag,
   RTI_LogicalTime const & theTime)
    throw (RTI_InteractionClassNotPublished,
              RTI_InteractionClassNotDefined,
              RTI_InteractionParameterNotDefined,
              RTI_InvalidLogicalTime,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 6.10
 virtual void deleteObjectInstance
 (RTI_ObjectInstanceHandle const & theObject,
   RTI_UserSuppliedTag const & theUserSuppliedTag)
    throw (RTI_DeletePrivilegeNotHeld,
              RTI_ObjectInstanceNotKnown,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 virtual RTI_auto_ptr< RTI_MessageRetractionHandle > deleteObjectInstance
 (RTI_ObjectInstanceHandle const & theObject,
   RTI_UserSuppliedTag const & theUserSuppliedTag,
   RTI_LogicalTimeconst & theTime)
    throw (RTI_DeletePrivilegeNotHeld,
              RTI_ObjectInstanceNotKnown,
              RTI_InvalidLogicalTime,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 6.12
 virtual void localDeleteObjectInstance
 (RTI_ObjectInstanceHandle const & theObject)
    throw (RTI_ObjectInstanceNotKnown,
              RTI_FederateOwnsAttributes,
              RTI_OwnershipAcquisitionPending,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 6.13
  virtual void changeAttributeTransportationType
  (RTI_ObjectInstanceHandle const & theObject,
    RTI_AttributeHandleSet const & theAttributes,
    RTI_TransportationType const & theType)
     throw (RTI_ObjectInstanceNotKnown,
              RTI_AttributeNotDefined,
              RTI_AttributeNotOwned,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 6.14
  virtual void changeInteractionTransportationType
  (RTI_InteractionClassHandle const & theClass,
    RTI_TransportationType const & theType)
     throw (RTI_InteractionClassNotDefined,
              RTI_InteractionClassNotPublished,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 6.17
  virtual void requestAttributeValueUpdate
  (RTI_ObjectInstanceHandle const & theObject,
    RTI_AttributeHandleSet const & theAttributes,
    RTI_UserSuppliedTag const & theUserSuppliedTag)
     throw (RTI_ObjectInstanceNotKnown,
              RTI_AttributeNotDefined,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  virtual void requestAttributeValueUpdate
  (RTI_ObjectClassHandle const & theClass,
    RTI_AttributeHandleSet const & theAttributes,
    RTI_UserSuppliedTag const & theUserSuppliedTag)
     throw (RTI_ObjectClassNotDefined,
              RTI_AttributeNotDefined,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  ///////////////////////////////////
  // Ownership Management Services //
  ///////////////////////////////////
  // 7.2
  virtual void unconditionalAttributeOwnershipDivestiture
  (RTI_ObjectInstanceHandle const & theObject,
    RTI_AttributeHandleSet const & theAttributes)
     throw (RTI_ObjectInstanceNotKnown,
              RTI_AttributeNotDefined,
              RTI_AttributeNotOwned,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 7.3
  virtual void negotiatedAttributeOwnershipDivestiture
  (RTI_ObjectInstanceHandle const & theObject,
   RTI_AttributeHandleSet const & theAttributes,
   RTI_UserSuppliedTag const & theUserSuppliedTag)
    throw (RTI_ObjectInstanceNotKnown,
              RTI_AttributeNotDefined,
              RTI_AttributeNotOwned,
              RTI_AttributeAlreadyBeingDivested,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 7.6
  virtual
  void
  confirmDivestiture
  (RTI_ObjectInstanceHandle const & theObject,
   RTI_auto_ptr< RTI_AttributeHandleSet >securedAttributes,
   RTI_UserSuppliedTagconst & theUserSuppliedTag)
    throw (RTI_ObjectInstanceNotKnown,
              RTI_AttributeNotDefined,
              RTI_AttributeNotOwned,
              RTI_AttributeDivestitureWasNotRequested,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 7.8
  virtual void attributeOwnershipAcquisition
  (RTI_ObjectInstanceHandle const & theObject,
   RTI_AttributeHandleSet const & desiredAttributes,
   RTI_UserSuppliedTag const & theUserSuppliedTag)
    throw (RTI_ObjectInstanceNotKnown,
              RTI_ObjectClassNotPublished,
              RTI_AttributeNotDefined,
              RTI_AttributeNotPublished,
              RTI_FederateOwnsAttributes,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 7.9
  virtual void attributeOwnershipAcquisitionIfAvailable
  (RTI_ObjectInstanceHandle const & theObject,
   RTI_AttributeHandleSet const & desiredAttributes)
    throw (RTI_ObjectInstanceNotKnown,
              RTI_ObjectClassNotPublished,
              RTI_AttributeNotDefined,
              RTI_AttributeNotPublished,
              RTI_FederateOwnsAttributes,
              RTI_AttributeAlreadyBeingAcquired,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 7.12
  virtual RTI_auto_ptr< RTI_AttributeHandleSet >
   attributeOwnershipDivestitureIfWanted
  (RTI_ObjectInstanceHandle const & theObject,
   RTI_AttributeHandleSet const & theAttributes)
    throw (RTI_ObjectInstanceNotKnown,
              RTI_AttributeNotDefined,
              RTI_AttributeNotOwned,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 7.13
  virtual void cancelNegotiatedAttributeOwnershipDivestiture
  (RTI_ObjectInstanceHandle const & theObject,
    RTI_AttributeHandleSet const & theAttributes)
     throw (RTI_ObjectInstanceNotKnown,
              RTI_AttributeNotDefined,
              RTI_AttributeNotOwned,
              RTI_AttributeDivestitureWasNotRequested,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 7.14
  virtual void cancelAttributeOwnershipAcquisition
  (RTI_ObjectInstanceHandle const & theObject,
    RTI_AttributeHandleSet const & theAttributes)
     throw (RTI_ObjectInstanceNotKnown,
              RTI_AttributeNotDefined,
              RTI_AttributeAlreadyOwned,
              RTI_AttributeAcquisitionWasNotRequested,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 7.16
  virtual void queryAttributeOwnership
  (RTI_ObjectInstanceHandle const & theObject,
    RTI_AttributeHandle const & theAttribute)
     throw (RTI_ObjectInstanceNotKnown,
              RTI_AttributeNotDefined,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 7.18
  virtual RTI_bool isAttributeOwnedByFederate
  (RTI_ObjectInstanceHandle const & theObject,
    RTI_AttributeHandle const & theAttribute)
     throw (RTI_ObjectInstanceNotKnown,
              RTI_AttributeNotDefined,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  //////////////////////////////
  // Time Management Services //
  //////////////////////////////
  // 8.2
  virtual void enableTimeRegulation
  (RTI_LogicalTimeInterval const & theLookahead)
     throw (RTI_TimeRegulationAlreadyEnabled,
              RTI_InvalidLookahead,
              RTI_InTimeAdvancingState,
              RTI_RequestForTimeRegulationPending,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 8.4
  virtual void disableTimeRegulation ()
     throw (RTI_TimeRegulationIsNotEnabled,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 8.5
  virtual void enableTimeConstrained ()
    throw (RTI_TimeConstrainedAlreadyEnabled,
              RTI_InTimeAdvancingState,
              RTI_RequestForTimeConstrainedPending,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 8.7
 virtual void disableTimeConstrained ()
    throw (RTI_TimeConstrainedIsNotEnabled,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 8.8
 virtual void timeAdvanceRequest
 (RTI_LogicalTime const & theTime)
    throw (RTI_InvalidLogicalTime,
              RTI_LogicalTimeAlreadyPassed,
              RTI_InTimeAdvancingState,
              RTI_RequestForTimeRegulationPending,
              RTI_RequestForTimeConstrainedPending,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 8.9
 virtual void timeAdvanceRequestAvailable
 (RTI_LogicalTime const & theTime)
    throw (RTI_InvalidLogicalTime,
              RTI_LogicalTimeAlreadyPassed,
              RTI_InTimeAdvancingState,
              RTI_RequestForTimeRegulationPending,
              RTI_RequestForTimeConstrainedPending,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 8.10
 virtual void nextMessageRequest
 (RTI_LogicalTime const & theTime)
    throw (RTI_InvalidLogicalTime,
              RTI_LogicalTimeAlreadyPassed,
              RTI_InTimeAdvancingState,
              RTI_RequestForTimeRegulationPending,
              RTI_RequestForTimeConstrainedPending,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 8.11
 virtual void nextMessageRequestAvailable
 (RTI_LogicalTime const & theTime)
    throw (RTI_InvalidLogicalTime,
              RTI_LogicalTimeAlreadyPassed,
              RTI_InTimeAdvancingState,
              RTI_RequestForTimeRegulationPending,
              RTI_RequestForTimeConstrainedPending,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 8.12
 virtual void flushQueueRequest
 (RTI_LogicalTime const & theTime)
    throw (RTI_InvalidLogicalTime,
            RTI_LogicalTimeAlreadyPassed,
              RTI_InTimeAdvancingState,
              RTI_RequestForTimeRegulationPending,
              RTI_RequestForTimeConstrainedPending,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 8.14
  virtual void enableAsynchronousDelivery ()
     throw (RTI_AsynchronousDeliveryAlreadyEnabled,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 8.15
     virtual void disableAsynchronousDelivery ()
     throw (RTI_AsynchronousDeliveryAlreadyDisabled,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 8.16
  virtual RTI_auto_ptr< RTI_LogicalTime > queryGALT ()
     throw (RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 8.17
 virtual RTI_LogicalTime const & queryLogicalTime ()
     throw (RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 8.18
  virtual RTI_auto_ptr< RTI_LogicalTime > const & queryLITS ()
     throw (RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 8.19
  virtual void modifyLookahead
  (RTI_LogicalTimeInterval const & theLookahead)
     throw (RTI_TimeRegulationIsNotEnabled,
              RTI_InvalidLookahead,
              RTI_InTimeAdvancingState,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 8.20
 virtual RTI_LogicalTimeInterval const & queryLookahead ()
     throw (RTI_TimeRegulationIsNotEnabled,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 8.21
  virtual void retract
  (RTI_MessageRetractionHandle const & theHandle)
     throw (RTI_InvalidRetractionHandle,
              RTI_TimeRegulationIsNotEnabled,
              RTI_MessageCanNoLongerBeRetracted,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 8.23
 virtual void changeAttributeOrderType
 (RTI_ObjectInstanceHandle const & theObject,
   RTI_AttributeHandleSet const & theAttributes,
   RTI_OrderType const & theType)
    throw (RTI_ObjectInstanceNotKnown,
              RTI_AttributeNotDefined,
              RTI_AttributeNotOwned,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 8.24
 virtual void changeInteractionOrderType
 (RTI_InteractionClassHandle const & theClass,
   RTI_OrderType const & theType)
    throw (RTI_InteractionClassNotDefined,
              RTI_InteractionClassNotPublished,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 //////////////////////////////////
 // Data Distribution Management //
 //////////////////////////////////
 // 9.2
 virtual RTI_auto_ptr< RTI_RegionHandle > createRegion
 (RTI_DimensionHandleSet const & theDimensions)
    throw (RTI_InvalidDimensionHandle,
       RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 9.3
 virtual void commitRegionModifications
 (RTI_RegionHandleSet const & theRegionHandleSet)
    throw (RTI_InvalidRegion,
       RTI_RegionNotCreatedByThisFederate,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 9.4
 virtual void deleteRegion
 (RTI_auto_ptr< RTI_RegionHandle > theRegion)
    throw (RTI_InvalidRegion,
       RTI_RegionNotCreatedByThisFederate,
              RTI_RegionInUseForUpdateOrSubscription,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 9.5
 virtual RTI_ObjectInstanceHandle const & registerObjectInstanceWithRegions
 (RTI_ObjectClassHandle const & theClass,
   RTI_AttributeHandleSetRegionHandleSetPairVector const &
   theAttributeHandleSetRegionHandleSetPairVector)
    throw (RTI_ObjectClassNotDefined,
              RTI_ObjectClassNotPublished,
              RTI_AttributeNotDefined,
              RTI_AttributeNotPublished,
              RTI_InvalidRegion,
              RTI_RegionNotCreatedByThisFederate,
              RTI_InvalidRegionContext,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  virtual RTI_ObjectInstanceHandle const & registerObjectInstanceWithRegions
  (RTI_ObjectClassHandle const & theClass,
    RTI_AttributeHandleSetRegionHandleSetPairVector const &
 theAttributeHandleSetRegionHandleSetPairVector,
    RTI_wstring const & theObjectInstanceName)
     throw (RTI_ObjectClassNotDefined,
              RTI_ObjectClassNotPublished,
              RTI_AttributeNotDefined,
              RTI_AttributeNotPublished,
        RTI_InvalidRegion,
              RTI_RegionNotCreatedByThisFederate,
              RTI_InvalidRegionContext,
              RTI_ObjectInstanceNameNotReserved,
              RTI_ObjectInstanceNameInUse,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 9.6
  virtual void associateRegionsForUpdates
  (RTI_ObjectInstanceHandle const & theObject,
    RTI_AttributeHandleSetRegionHandleSetPairVector const &
 theAttributeHandleSetRegionHandleSetPairVector)
     throw (RTI_ObjectInstanceNotKnown,
              RTI_AttributeNotDefined,
        RTI_InvalidRegion,
              RTI_RegionNotCreatedByThisFederate,
              RTI_InvalidRegionContext,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 9.7
  virtual void unassociateRegionsForUpdates
  (RTI_ObjectInstanceHandle const & theObject,
    RTI_AttributeHandleSetRegionHandleSetPairVector const &
 theAttributeHandleSetRegionHandleSetPairVector)
     throw (RTI_ObjectInstanceNotKnown,
              RTI_AttributeNotDefined,
        RTI_InvalidRegion,
              RTI_RegionNotCreatedByThisFederate,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 9.8
  virtual void subscribeObjectClassAttributesWithRegions
  (RTI_ObjectClassHandle const & theClass,
    RTI_AttributeHandleSetRegionHandleSetPairVector const &
 theAttributeHandleSetRegionHandleSetPairVector,
    RTI_bool active = RTI_true)
     throw (RTI_ObjectClassNotDefined,
              RTI_AttributeNotDefined,
        RTI_InvalidRegion,
              RTI_RegionNotCreatedByThisFederate,
              RTI_InvalidRegionContext,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 9.9
  virtual void unsubscribeObjectClassAttributesWithRegions
  (RTI_ObjectClassHandle const & theClass,
   RTI_AttributeHandleSetRegionHandleSetPairVector const
& theAttributeHandleSetRegionHandleSetPairVector)
    throw (RTI_ObjectClassNotDefined,
              RTI_AttributeNotDefined,
       RTI_InvalidRegion,
              RTI_RegionNotCreatedByThisFederate,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 9.10
 virtual void subscribeInteractionClassWithRegions
 (RTI_InteractionClassHandle const & theClass,
   RTI_RegionHandleSet const & theRegionHandleSet,
   RTI_bool active = RTI_true)
    throw (RTI_InteractionClassNotDefined,RTI_InvalidRegion,
              RTI_RegionNotCreatedByThisFederate,
       RTI_InvalidRegionContext,
              RTI_FederateServiceInvocationsAreBeingReportedViaMOM,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
// 9.11
  virtual void unsubscribeInteractionClassWithRegions
  (RTI_InteractionClassHandle const & theClass,
   RTI_RegionHandleSet const & theRegionHandleSet)
    throw (RTI_InteractionClassNotDefined,
       RTI_InvalidRegion,
              RTI_RegionNotCreatedByThisFederate,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 9.12
  virtual void sendInteractionWithRegions
  (RTI_InteractionClassHandleconst & theInteraction,
   RTI_ParameterHandleValueMap const & theParameterValues,
   RTI_RegionHandleSet const & theRegionHandleSet,
   RTI_UserSuppliedTag const & theUserSuppliedTag)
    throw (RTI_InteractionClassNotDefined,
              RTI_InteractionClassNotPublished,
              RTI_InteractionParameterNotDefined,
       RTI_InvalidRegion,
              RTI_RegionNotCreatedByThisFederate,
              RTI_InvalidRegionContext,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  virtual RTI_auto_ptr< RTI_MessageRetractionHandle > sendInteractionWithRegions
  (RTI_InteractionClassHandleconst & theInteraction,
   RTI_ParameterHandleValueMap const & theParameterValues,
   RTI_RegionHandleSet const & theRegionHandleSet,
   RTI_UserSuppliedTag const & theUserSuppliedTag,
   RTI_LogicalTime const & theTime)
    throw (RTI_InteractionClassNotDefined,
              RTI_InteractionClassNotPublished,
              RTI_InteractionParameterNotDefined,
       RTI_InvalidRegion,
              RTI_RegionNotCreatedByThisFederate,
              RTI_InvalidRegionContext,
              RTI_InvalidLogicalTime,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 9.13
  virtual void requestAttributeValueUpdateWithRegions
  (RTI_ObjectClassHandle const & theClass,
    RTI_AttributeHandleSetRegionHandleSetPairVector const & theSet,
    RTI_UserSuppliedTag const & theUserSuppliedTag)
     throw (RTI_ObjectClassNotDefined,
              RTI_AttributeNotDefined,
        RTI_InvalidRegion,
              RTI_RegionNotCreatedByThisFederate,
              RTI_InvalidRegionContext,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  //////////////////////////
  // RTI Support Services //
  //////////////////////////
  // 10.2
  virtual RTI_ObjectClassHandle const & getObjectClassHandle
  (RTI_wstring const & theName)
     throw (RTI_NameNotFound,
              RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.3
  virtual RTI_wstring const & getObjectClassName
  (RTI_ObjectClassHandle const & theHandle)
     throw (RTI_InvalidObjectClassHandle,
        RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.4
  virtual RTI_AttributeHandle const & getAttributeHandle
  (RTI_ObjectClassHandle const & whichClass,
    RTI_wstring const & theAttributeName)
     throw (RTI_InvalidObjectClassHandle,
        RTI_NameNotFound,
              RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.5
 virtual RTI_wstring const & getAttributeName
 (RTI_ObjectClassHandle const & whichClass,
 RTI_AttributeHandle const & theHandle)
  throw (RTI_InvalidObjectClassHandle,
     RTI_InvalidAttributeHandle,
     RTI_AttributeNotDefined,
           RTI_FederateNotExecutionMember,
           RTI_RTIinternalError) = 0;
  // 10.6
  virtual RTI_InteractionClassHandle const & getInteractionClassHandle
  (RTI_wstring const & theName)
     throw (RTI_NameNotFound,
              RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.7
  virtual RTI_wstring const & getInteractionClassName
  (RTI_InteractionClassHandle const & theHandle)
     throw (RTI_InvalidInteractionClassHandle,
        RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.8
  virtual RTI_ParameterHandle const & getParameterHandle
  (RTI_InteractionClassHandle const & whichClass,
   RTI_wstring const & theName)
      throw (RTI_InvalidInteractionClassHandle,
       RTI_NameNotFound,
              RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.9
  virtual RTI_wstring const & getParameterName
  (RTI_InteractionClassHandle const & whichClass,
   RTI_ParameterHandle const & theHandle)
    throw (RTI_InvalidInteractionClassHandle,
       RTI_InvalidParameterHandle,
       RTI_InteractionParameterNotDefined,
              RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.10
  virtual RTI_ObjectInstanceHandle const & getObjectInstanceHandle
  (RTI_wstring const & theName)
    throw (RTI_ObjectInstanceNotKnown,
              RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.11
  virtual RTI_wstring const & getObjectInstanceName
  (RTI_ObjectInstanceHandle const & theHandle)
    throw (RTI_ObjectInstanceNotKnown,
              RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.12
  virtual RTI_DimensionHandle const & getDimensionHandle
  (RTI_wstring const & theName)
    throw (RTI_NameNotFound,
              RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.13
  virtual RTI_wstring const & getDimensionName
  (RTI_DimensionHandle const & theHandle)
    throw (RTI_InvalidDimensionHandle,
       RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.14
  virtual unsigned long getDimensionUpperBound
  (RTI_DimensionHandle const & theHandle)
    throw (RTI_InvalidDimensionHandle,
              RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.15
  virtual RTI_DimensionHandleSet const & getAvailableDimensionsForClassAttribute
  (RTI_ObjectClassHandle const & theClass,
   RTI_AttributeHandle const & theHandle)
    throw (RTI_InvalidObjectClassHandle,
       RTI_InvalidAttributeHandle,
       RTI_AttributeNotDefined,
              RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.16
  virtual RTI_ObjectClassHandle const & getKnownObjectClassHandle
  (RTI_ObjectInstanceHandle const & theObject)
    throw (RTI_ObjectInstanceNotKnown,
              RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.17
  virtual RTI_DimensionHandleSet const & getAvailableDimensionsForInteractionClass
  (RTI_InteractionClassHandle const & theClass)
     throw (RTI_InvalidInteractionClassHandle,
        RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.18
  virtual RTI_TransportationType const & getTransportationType
  (RTI_wstring const & transportationName)
     throw (RTI_InvalidTransportationName,
              RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.19
  virtual RTI_wstring const & getTransportationName
  (RTI_TransportationType const & transportationType)
     throw (RTI_InvalidTransportationType,
              RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.20
  virtual RTI_OrderType const & getOrderType
  (RTI_wstring const & orderName)
     throw (RTI_InvalidOrderName,
              RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.21
  virtual RTI_wstring const & getOrderName
  (RTI_OrderType const & orderType)
     throw (RTI_InvalidOrderType,
              RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.22
  virtual void enableObjectClassRelevanceAdvisorySwitch ()
     throw (RTI_ObjectClassRelevanceAdvisorySwitchIsOn,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 10.23
  virtual void disableObjectClassRelevanceAdvisorySwitch ()
     throw (RTI_ObjectClassRelevanceAdvisorySwitchIsOff,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 10.24
  virtual void enableAttributeRelevanceAdvisorySwitch ()
     throw (RTI_AttributeRelevanceAdvisorySwitchIsOn,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 10.25
  virtual void disableAttributeRelevanceAdvisorySwitch ()
     throw (RTI_AttributeRelevanceAdvisorySwitchIsOff,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 10.26
  virtual void enableAttributeScopeAdvisorySwitch ()
     throw (RTI_AttributeScopeAdvisorySwitchIsOn,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 10.27
 virtual void disableAttributeScopeAdvisorySwitch ()
    throw (RTI_AttributeScopeAdvisorySwitchIsOff,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 10.28
 virtual void enableInteractionRelevanceAdvisorySwitch ()
    throw (RTI_InteractionRelevanceAdvisorySwitchIsOn,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 10.29
 virtual void disableInteractionRelevanceAdvisorySwitch ()
    throw (RTI_InteractionRelevanceAdvisorySwitchIsOff,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 10.30
 virtual
 RTI_DimensionHandleSet const & getDimensionHandleSet
 (RTI_RegionHandle const & theRegionHandle)
    throw (RTI_InvalidRegion,
       RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 10.31
 virtual
 RTI_RangeBounds const & getRangeBounds
 (RTI_RegionHandle const & theRegionHandle,
   RTI_DimensionHandle const & theDimensionHandle)
    throw (RTI_InvalidRegion,
       RTI_RegionDoesNotContainSpecifiedDimension,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 10.32
 virtual void setRangeBounds
 (RTI_RegionHandle const & theRegionHandle,
   RTI_DimensionHandle const & theDimensionHandle,
   RTI_RangeBounds const & theRangeBounds)
    throw (RTI_InvalidRegion,
       RTI_RegionNotCreatedByThisFederate,
              RTI_RegionDoesNotContainSpecifiedDimension,
              RTI_InvalidRangeBounds,
              RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 // 10.33
 virtual unsigned long normalizeFederateHandle
 (RTI_FederateHandle const & theFederateHandle)
    throw (RTI_FederateHandle,
       RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
 // 10.34
 virtual unsigned long normalizeServiceGroup
 (RTI_ServiceGroupIndicator const & theServiceGroup)
     throw (RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.37
  virtual RTI_bool evokeCallback(double approximateMinimumTimeInSeconds)
     throw (RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.38
  virtual RTI_bool evokeMultipleCallbacks(double approximateMinimumTimeInSeconds,
 double approximateMaximumTimeInSeconds)
     throw (RTI_FederateNotExecutionMember,
              RTI_RTIinternalError) = 0;
  // 10.39
    virtual void enableCallbacks ()
     throw (RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
  // 10.40
  virtual void disableCallbacks ()
     throw (RTI_FederateNotExecutionMember,
              RTI_SaveInProgress,
              RTI_RestoreInProgress,
              RTI_RTIinternalError) = 0;
 };
 #endif // RTI_RTIambassador_h
