// HLA 1.3 Header "RTIambServices.hh"

typedef FederateAmbassador *FederateAmbassadorPtr ;


/**
 * @defgroup HLA13_FederationManagement Federation Management
 * @ingroup  HLA13_RTI_Ambassador
 * @{
 */

/**
 * Create the federation execution.
 * @param[in] executionName, the name of the federation to be created
 * @param[in] FEDid, the file path to the Federation Execution Data file, a.k.a. FED file.
 *            this file can be an XML file or a "fed" file.
 * @exception FederationExecutionAlreadyExists the RTI has already registered a federation with the same name
 * @exception CouldNotOpenFED the FED (Federation Execution Data) file cannot be found or read properly (check RTIG log output)
 * @exception ErrorReadingFED
 * @exception ConcurrentAccessAttempted
 * @exception RTIinternalError
 */
void createFederationExecution(const char *executionName, const char *FEDid)
    throw (FederationExecutionAlreadyExists, CouldNotOpenFED, ErrorReadingFED,
	   ConcurrentAccessAttempted, RTIinternalError);

/**
 * Destroys the federation execution.
 * @param[in] executionName the name of the federation
 */
void destroyFederationExecution(const char *executionName)
    throw (FederatesCurrentlyJoined, FederationExecutionDoesNotExist,
	   ConcurrentAccessAttempted, RTIinternalError);

/**
 * Joins the federation execution.
 * 
 */
FederateHandle joinFederationExecution(const char *, const char *, FederateAmbassadorPtr)
    throw (FederateAlreadyExecutionMember, FederationExecutionDoesNotExist,
	   CouldNotOpenFED, ErrorReadingFED, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void resignFederationExecution(ResignAction)
    throw (FederateOwnsAttributes, FederateNotExecutionMember, InvalidResignAction,
	   ConcurrentAccessAttempted, RTIinternalError);

/**
 * Register Federation Synchronization Point.
 * All federates who have joined the federation are informed.
 *
 * @param[in] label Synchronization point label
 * @param[in] tag User-supplied tag
 */
void registerFederationSynchronizationPoint(const char *, const char *)
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

/**
 * Register Federation Synchronization Point with set of federates.
 * Only federates in the set are informed.
 *
 * @param[in] label Synchronization point label
 * @param[in] tag User-supplied tag
 * @param[in] federates set of federate designators
 */
void registerFederationSynchronizationPoint(const char *label, const char *tag, const FederateHandleSet &federates)
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress,
	   RestoreInProgress, RTIinternalError);

void synchronizationPointAchieved(const char *)
    throw (SynchronizationPointLabelWasNotAnnounced, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void requestFederationSave(const char *, const FedTime &)
    throw (FederationTimeAlreadyPassed, InvalidFederationTime, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void requestFederationSave(const char *)
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress,
	   RestoreInProgress, RTIinternalError);

void federateSaveBegun()
    throw (SaveNotInitiated, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   RestoreInProgress, RTIinternalError);

void federateSaveComplete()
    throw (SaveNotInitiated, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   RestoreInProgress, RTIinternalError);

void federateSaveNotComplete()
    throw (SaveNotInitiated, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   RestoreInProgress, RTIinternalError);

void requestFederationRestore(const char *)
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress,
	   RestoreInProgress, RTIinternalError);

void federateRestoreComplete()
    throw (RestoreNotRequested, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RTIinternalError);

void federateRestoreNotComplete()
    throw (RestoreNotRequested, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RTIinternalError);

/** @} end group HLA13_FederationManagement */

/**
 * @defgroup HLA13_DeclarationManagement Declaration Management
 * @ingroup  HLA13_RTI_Ambassador
 * @{
 */

void publishObjectClass(ObjectClassHandle, const AttributeHandleSet &)
    throw (ObjectClassNotDefined, AttributeNotDefined, OwnershipAcquisitionPending,
	   FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress,
	   RestoreInProgress, RTIinternalError);

void unpublishObjectClass(ObjectClassHandle)
    throw (ObjectClassNotDefined, ObjectClassNotPublished, OwnershipAcquisitionPending,
	   FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress,
	   RestoreInProgress, RTIinternalError);

void publishInteractionClass(InteractionClassHandle)
    throw (InteractionClassNotDefined, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void unpublishInteractionClass(InteractionClassHandle)
    throw (InteractionClassNotDefined, InteractionClassNotPublished, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void subscribeObjectClassAttributes(ObjectClassHandle, const AttributeHandleSet &, Boolean = RTI_TRUE)
    throw (ObjectClassNotDefined, AttributeNotDefined, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void unsubscribeObjectClass(ObjectClassHandle)
    throw (ObjectClassNotDefined, ObjectClassNotSubscribed, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void subscribeInteractionClass(InteractionClassHandle, Boolean = RTI_TRUE)
    throw (InteractionClassNotDefined, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   FederateLoggingServiceCalls, SaveInProgress, RestoreInProgress, RTIinternalError);

void unsubscribeInteractionClass(InteractionClassHandle)
    throw (InteractionClassNotDefined, InteractionClassNotSubscribed, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

/** @} end group HLA13_DeclarationManagement */

/**
 * @defgroup HLA13_ObjectManagement Object Management
 * @ingroup  HLA13_RTI_Ambassador
 * @{
 */


ObjectHandle registerObjectInstance(ObjectClassHandle, const char *)
    throw (ObjectClassNotDefined, ObjectClassNotPublished, ObjectAlreadyRegistered,
	   FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress,
	   RestoreInProgress, RTIinternalError);

ObjectHandle registerObjectInstance(ObjectClassHandle)
    throw (ObjectClassNotDefined, ObjectClassNotPublished, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

/**
 * Update Attribute Values (with time) service (HLA 1.3, §6.4).
 * Provide current values to the federation for instance attributes owned by
 * the federate.
 * @param[in] object     Object instance designator
 * @param[in] attributes Set of attribute designator and value pairs
 * @param[in] tag        User supplied tag
 */
EventRetractionHandle updateAttributeValues(ObjectHandle, const AttributeHandleValuePairSet &,
					    const FedTime &, const char *)
    throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned, InvalidFederationTime,
	   FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress,
	   RestoreInProgress, RTIinternalError);

/**
 * Update Attribute Values (without time) service (HLA 1.3, §6.4).
 * Provide current values to the federation for instance attributes owned by
 * the federate.
 * @param[in] object     Object instance designator
 * @param[in] attributes Set of attribute designator and value pairs
 * @param[in] tag        User supplied tag
 */
void updateAttributeValues(ObjectHandle object, const AttributeHandleValuePairSet& attributes, const char * tag)
    throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

/**
 * Send Interaction with time
 * This service (HLA 1.3) send an interaction into the federation.
 * As the federation time argument is supplied, an event retraction designator is returned.
 * @param interactionClassHandle Interaction class designator
 * @param parameters Set of interaction parameters designator and value pairs
 * @param time Federation time
 * @param tag User-supplied tag
 */
EventRetractionHandle sendInteraction(InteractionClassHandle interactionClassHandle, const ParameterHandleValuePairSet& parameters,
				      const FedTime& time, const char * tag)
    throw (InteractionClassNotDefined, InteractionClassNotPublished, InteractionParameterNotDefined,
	   InvalidFederationTime, FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress,
	   RestoreInProgress, RTIinternalError);

void sendInteraction(InteractionClassHandle, const ParameterHandleValuePairSet &, const char *)
    throw (InteractionClassNotDefined, InteractionClassNotPublished, InteractionParameterNotDefined,
	   FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress,
	   RTIinternalError);

/**
 * Delete Object with time
 * This service (HLA 1.3, §6.8) deletes an object instance.
 * As the federation time argument is supplied, an event retraction designator is returned.
 * @param theObject Object designator
 * @param theTime Federation time
 * @param theTag user supplied tag
 */
EventRetractionHandle deleteObjectInstance(ObjectHandle, const FedTime &, const char *)
    throw (ObjectNotKnown, DeletePrivilegeNotHeld, InvalidFederationTime, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

/**
 * Delete Object without time
 * This service (HLA 1.3) deletes an object instance.
 * @param theObject Object designator
 * @param theTag user supplied tag
 */
void deleteObjectInstance(ObjectHandle, const char *)
    throw (ObjectNotKnown, DeletePrivilegeNotHeld, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void localDeleteObjectInstance(ObjectHandle)
    throw (ObjectNotKnown, FederateOwnsAttributes, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void changeAttributeTransportationType(ObjectHandle, const AttributeHandleSet &, TransportationHandle)
    throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned, InvalidTransportationHandle,
	   FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress,
	   RTIinternalError);

void changeInteractionTransportationType(InteractionClassHandle, TransportationHandle)
    throw (InteractionClassNotDefined, InteractionClassNotPublished, InvalidTransportationHandle,
	   FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress,
	   RTIinternalError);

void requestObjectAttributeValueUpdate(ObjectHandle, const AttributeHandleSet &)
    throw (ObjectNotKnown, AttributeNotDefined, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void requestClassAttributeValueUpdate(ObjectClassHandle, const AttributeHandleSet &)
    throw (ObjectClassNotDefined, AttributeNotDefined, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

/** @} end group HLA13_ObjectManagement */

/**
 * @defgroup HLA13_OwnershipManagement Ownership Management
 * @ingroup  HLA13_RTI_Ambassador
 * @{
 */

void unconditionalAttributeOwnershipDivestiture(ObjectHandle, const AttributeHandleSet &)
    throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void negotiatedAttributeOwnershipDivestiture(ObjectHandle, const AttributeHandleSet &, const char *)
    throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned, AttributeAlreadyBeingDivested,
	   FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress,
	   RTIinternalError);

void attributeOwnershipAcquisition(ObjectHandle, const AttributeHandleSet &desiredAttributes, const char *)
    throw (ObjectNotKnown, ObjectClassNotPublished, AttributeNotDefined, AttributeNotPublished,
	   FederateOwnsAttributes, FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress,
	   RestoreInProgress, RTIinternalError);

void attributeOwnershipAcquisitionIfAvailable(ObjectHandle, const AttributeHandleSet &)
    throw (ObjectNotKnown, ObjectClassNotPublished, AttributeNotDefined, AttributeNotPublished,
	   FederateOwnsAttributes, AttributeAlreadyBeingAcquired, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

AttributeHandleSet *attributeOwnershipReleaseResponse(ObjectHandle, const AttributeHandleSet &)
    throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned, FederateWasNotAskedToReleaseAttribute,
	   FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void cancelNegotiatedAttributeOwnershipDivestiture(ObjectHandle, const AttributeHandleSet &)
    throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned, AttributeDivestitureWasNotRequested,
	   FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress,
	   RTIinternalError);

void cancelAttributeOwnershipAcquisition(ObjectHandle, const AttributeHandleSet &)
    throw (ObjectNotKnown, AttributeNotDefined, AttributeAlreadyOwned,
	   AttributeAcquisitionWasNotRequested, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void queryAttributeOwnership(ObjectHandle, AttributeHandle)
    throw (ObjectNotKnown, AttributeNotDefined, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

Boolean isAttributeOwnedByFederate(ObjectHandle, AttributeHandle)
    throw (ObjectNotKnown, AttributeNotDefined, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

/** @} end group HLA13_OwnershipManagement */

/**
 * @defgroup HLA13_TimeManagement Time Management
 * @ingroup  HLA13_RTI_Ambassador
 * @{
 */


void enableTimeRegulation(const FedTime &, const FedTime &)
    throw (TimeRegulationAlreadyEnabled, EnableTimeRegulationPending, TimeAdvanceAlreadyInProgress,
	   InvalidFederationTime, InvalidLookahead, ConcurrentAccessAttempted,
	   FederateNotExecutionMember, SaveInProgress, RestoreInProgress, RTIinternalError);

void disableTimeRegulation()
    throw (TimeRegulationWasNotEnabled, ConcurrentAccessAttempted, FederateNotExecutionMember,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void enableTimeConstrained()
    throw (TimeConstrainedAlreadyEnabled, EnableTimeConstrainedPending, TimeAdvanceAlreadyInProgress,
	   FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress,
	   RTIinternalError);

void disableTimeConstrained()
    throw (TimeConstrainedWasNotEnabled, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void timeAdvanceRequest(const FedTime &)
    throw (InvalidFederationTime, FederationTimeAlreadyPassed, TimeAdvanceAlreadyInProgress,
	   EnableTimeRegulationPending, EnableTimeConstrainedPending, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void timeAdvanceRequestAvailable(const FedTime &)
    throw (InvalidFederationTime, FederationTimeAlreadyPassed, TimeAdvanceAlreadyInProgress,
	   EnableTimeRegulationPending, EnableTimeConstrainedPending, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void nextEventRequest(const FedTime &)
    throw (InvalidFederationTime, FederationTimeAlreadyPassed, TimeAdvanceAlreadyInProgress,
	   EnableTimeRegulationPending, EnableTimeConstrainedPending, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void nextEventRequestAvailable(const FedTime &)
    throw (InvalidFederationTime, FederationTimeAlreadyPassed, TimeAdvanceAlreadyInProgress,
	   EnableTimeRegulationPending, EnableTimeConstrainedPending, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void flushQueueRequest(const FedTime &)
    throw (InvalidFederationTime, FederationTimeAlreadyPassed, TimeAdvanceAlreadyInProgress,
	   EnableTimeRegulationPending, EnableTimeConstrainedPending, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void enableAsynchronousDelivery()
    throw (AsynchronousDeliveryAlreadyEnabled, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void disableAsynchronousDelivery()
    throw (AsynchronousDeliveryAlreadyDisabled, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void queryLBTS(FedTime &)
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void queryFederateTime(FedTime &)
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void queryMinNextEventTime(FedTime &)
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void modifyLookahead(const FedTime &)
    throw (InvalidLookahead, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void queryLookahead(FedTime &)
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress,
	   RestoreInProgress, RTIinternalError);

void retract(EventRetractionHandle theHandle)
    throw (InvalidRetractionHandle, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void changeAttributeOrderType(ObjectHandle, const AttributeHandleSet &, OrderingHandle)
    throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned, InvalidOrderingHandle,
	   FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void changeInteractionOrderType(InteractionClassHandle, OrderingHandle)
    throw (InteractionClassNotDefined, InteractionClassNotPublished, InvalidOrderingHandle,
	   FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

/** @} end group HLA13_TimeManagement */

/**
 * @defgroup HLA13_DataDistributionManagement Data Distribution Management
 * @ingroup  HLA13_RTI_Ambassador
 * @{
 */


Region *createRegion(SpaceHandle, ULong)
    throw (SpaceNotDefined, InvalidExtents, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void notifyAboutRegionModification(Region &theRegion)
    throw (RegionNotKnown, InvalidExtents, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void deleteRegion(Region *)
    throw (RegionNotKnown, RegionInUse, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

RTI::ObjectHandle registerObjectInstanceWithRegion(ObjectClassHandle, const char *, AttributeHandle [],
					      Region *theRegions[], ULong)
    throw (ObjectClassNotDefined, ObjectClassNotPublished, AttributeNotDefined, AttributeNotPublished,
	   RegionNotKnown, InvalidRegionContext, ObjectAlreadyRegistered, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

RTI::ObjectHandle registerObjectInstanceWithRegion(ObjectClassHandle, AttributeHandle [], Region *regions[], ULong)
    throw (ObjectClassNotDefined, ObjectClassNotPublished, AttributeNotDefined, AttributeNotPublished,
	   RegionNotKnown, InvalidRegionContext, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void associateRegionForUpdates(Region &, ObjectHandle, const AttributeHandleSet &)
    throw (ObjectNotKnown, AttributeNotDefined, InvalidRegionContext, RegionNotKnown,
	   FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress,
	   RestoreInProgress, RTIinternalError);

void unassociateRegionForUpdates(Region &, ObjectHandle)
    throw (ObjectNotKnown, InvalidRegionContext, RegionNotKnown, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void subscribeObjectClassAttributesWithRegion(ObjectClassHandle, Region &, const AttributeHandleSet &,
					      Boolean = RTI_TRUE)
    throw (ObjectClassNotDefined, AttributeNotDefined, RegionNotKnown, InvalidRegionContext,
	   FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void unsubscribeObjectClassWithRegion(ObjectClassHandle, Region &)
    throw (ObjectClassNotDefined, RegionNotKnown, ObjectClassNotSubscribed, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void subscribeInteractionClassWithRegion(InteractionClassHandle, Region &, Boolean = RTI_TRUE)
    throw (InteractionClassNotDefined, RegionNotKnown, InvalidRegionContext, FederateLoggingServiceCalls,
	   FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void unsubscribeInteractionClassWithRegion(InteractionClassHandle, Region &)
    throw (InteractionClassNotDefined, InteractionClassNotSubscribed, RegionNotKnown,
	   FederateNotExecutionMember, ConcurrentAccessAttempted, SaveInProgress,
	   RestoreInProgress, RTIinternalError);

EventRetractionHandle sendInteractionWithRegion(InteractionClassHandle, const ParameterHandleValuePairSet &,
						const FedTime &, const char *, const Region &)
    throw (InteractionClassNotDefined, InteractionClassNotPublished, InteractionParameterNotDefined,
	   InvalidFederationTime, RegionNotKnown, InvalidRegionContext, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

void sendInteractionWithRegion(InteractionClassHandle, const ParameterHandleValuePairSet &,
			       const char *, const Region &)
    throw (InteractionClassNotDefined, InteractionClassNotPublished, InteractionParameterNotDefined,
	   RegionNotKnown, InvalidRegionContext, FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

void requestClassAttributeValueUpdateWithRegion(ObjectClassHandle, const AttributeHandleSet &, const Region &)
    throw (ObjectClassNotDefined, AttributeNotDefined, RegionNotKnown, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, SaveInProgress, RestoreInProgress, RTIinternalError);

/** @} end group HLA13_DataDistributionManagement */

/**
 * @defgroup HLA13_SupportService Support Service
 * @ingroup  HLA13_RTI_Ambassador
 * @{
 */

ObjectClassHandle getObjectClassHandle(const char *)
    throw (NameNotFound, FederateNotExecutionMember, ConcurrentAccessAttempted, RTIinternalError);

char *getObjectClassName(ObjectClassHandle)
    throw (ObjectClassNotDefined, FederateNotExecutionMember, ConcurrentAccessAttempted, RTIinternalError);

AttributeHandle getAttributeHandle(const char *, ObjectClassHandle)
    throw (ObjectClassNotDefined, NameNotFound, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, RTIinternalError);

char *getAttributeName(AttributeHandle, ObjectClassHandle)
    throw (ObjectClassNotDefined, AttributeNotDefined, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, RTIinternalError);

InteractionClassHandle getInteractionClassHandle(const char *)
    throw (NameNotFound, FederateNotExecutionMember, ConcurrentAccessAttempted, RTIinternalError);

char *getInteractionClassName(InteractionClassHandle)
    throw (InteractionClassNotDefined, FederateNotExecutionMember, ConcurrentAccessAttempted, RTIinternalError);

ParameterHandle getParameterHandle(const char *, InteractionClassHandle)
    throw (InteractionClassNotDefined, NameNotFound, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, RTIinternalError);

char *getParameterName(ParameterHandle, InteractionClassHandle)
    throw (InteractionClassNotDefined, InteractionParameterNotDefined, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, RTIinternalError);

ObjectHandle getObjectInstanceHandle(const char *)
    throw (ObjectNotKnown, FederateNotExecutionMember, ConcurrentAccessAttempted, RTIinternalError);

char *getObjectInstanceName(ObjectHandle)
    throw (ObjectNotKnown, FederateNotExecutionMember, ConcurrentAccessAttempted, RTIinternalError);

SpaceHandle getRoutingSpaceHandle(const char *)
    throw (NameNotFound, FederateNotExecutionMember, ConcurrentAccessAttempted, RTIinternalError);

char *getRoutingSpaceName(SpaceHandle)
    throw (SpaceNotDefined, FederateNotExecutionMember, ConcurrentAccessAttempted, RTIinternalError);

RTI::DimensionHandle getDimensionHandle(const char *, SpaceHandle)
    throw (SpaceNotDefined, NameNotFound, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, RTIinternalError);

char *getDimensionName(DimensionHandle, SpaceHandle)
    throw (SpaceNotDefined, DimensionNotDefined, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, RTIinternalError);

RTI::SpaceHandle getAttributeRoutingSpaceHandle(AttributeHandle, ObjectClassHandle)
    throw (ObjectClassNotDefined, AttributeNotDefined, FederateNotExecutionMember,
	   ConcurrentAccessAttempted, RTIinternalError);

RTI::ObjectClassHandle getObjectClass(ObjectHandle)
    throw (ObjectNotKnown, FederateNotExecutionMember, ConcurrentAccessAttempted, RTIinternalError);

RTI::SpaceHandle getInteractionRoutingSpaceHandle(InteractionClassHandle)
    throw (InteractionClassNotDefined, FederateNotExecutionMember, ConcurrentAccessAttempted, RTIinternalError);

TransportationHandle getTransportationHandle(const char *)
    throw (NameNotFound, FederateNotExecutionMember, ConcurrentAccessAttempted, RTIinternalError);

char *getTransportationName(TransportationHandle)
    throw (InvalidTransportationHandle, FederateNotExecutionMember, ConcurrentAccessAttempted, RTIinternalError);

OrderingHandle getOrderingHandle(const char *)
    throw (NameNotFound, FederateNotExecutionMember, ConcurrentAccessAttempted, RTIinternalError);

char *getOrderingName(OrderingHandle)
    throw (InvalidOrderingHandle, FederateNotExecutionMember, ConcurrentAccessAttempted, RTIinternalError);

/**
 * Sets the ClassRelevanceAdvisory (CRA) switch to true. The switch
 * state is hold on the RTIG side. That's why the message
 * ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the CRA switch is true. This causes a delivery of the
 * federate service startRegistrationForObjectClass to a publisher
 * if there are any new subscribers for the federates published object
 * classes. If there are no more subscribers a publisher gets the
 * federate service stopRegistrationForObjectClass.
 *
 * By disabling the CRA switch the federate is no longer informed by
 * subscriptions to its published object classes, i.e. the federate
 * services startRegistrationForObjectClass and
 * stopRegistrationForObjectClass respectively are not invoked.
 * @see disableClassRelevanceAdvisorySwitch()
 */
void enableClassRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

/**
 * Sets the ClassRelevanceAdvisory (CRA) switch to false. The switch
 * state is hold on the RTIG side. That's why the message
 * DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the CRA switch is true. This causes a delivery of the
 * federate service startRegistrationForObjectClass to a publisher
 * if there are any new subscribers for the federates published object
 * classes. If there are no more subscribers a publisher gets the
 * federate service stopRegistrationForObjectClass.
 * @see enableClassRelevanceAdvisorySwitch()
 *
 * By disabling the CRA switch the federate is no longer informed by
 * subscriptions to its published object classes, i.e. the federate
 * services startRegistrationForObjectClass and
 * stopRegistrationForObjectClass respectively are not invoked.
 */
void disableClassRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

/**
 * Sets the AttributeRelevanceAdvisory (ARA) switch to true. The switch
 * state is hold on the RTIG side. That's why the message
 * ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the ARA switch is false. When enabling the ARA switch
 * the federate is informed by the federate service
 * turnUpdatesOnForObjectInstance of new object instances within remote
 * federates actively subscribed to its published attributes. If there
 * are no active subscribers for a set of instance-attributes the federate
 * receives the federate service turnUpdatesOffForObjectInstance.
 *
 * By disabling the ARA switch the federate is no longer informed by
 * subscriptions to its published attributes, i.e. the federate
 * services turnUpdatesOnForObjectInstance and
 * turnUpdatesOffForObjectInstance respectively are not invoked.
 * @see disableAttributeRelevanceAdvisorySwitch()
 */
void enableAttributeRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

/**
 * Sets the AttributeRelevanceAdvisory (ARA) switch to false. The switch
 * state is hold on the RTIG side. That's why the message
 * DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the ARA switch is false. When enabling the ARA switch
 * the federate is informed by the federate service
 * turnUpdatesOnForObjectInstance of new object instances within remote
 * federates actively subscribed to its published attributes. If there
 * are no active subscribers for a set of instance-attributes the federate
 * receives the federate service turnUpdatesOffForObjectInstance.
 * @see enableAttributeRelevanceAdvisorySwitch()
 *
 * By disabling the ARA switch the federate is no longer informed by
 * subscriptions to its published attributes, i.e. the federate
 * services turnUpdatesOnForObjectInstance and
 * turnUpdatesOffForObjectInstance respectively are not invoked.
 */
void disableAttributeRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

/**
 * Sets the AttributeScopeAdvisory (ASA) switch to true. The switch state
 * is hold on the RTIG side. That's why the message
 * ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the ASA switch is false. When enabling the ASA switch
 * the federate is informed by the federate services
 * attributesInScope and attributesOutScope respectively of discovered
 * or registrated but not owned attribute-instances intersecting or
 * leaving its subscription regions.
 *
 * By disabling the ASA switch the federate is no longer informed of
 * changes in attribute-instance scope, i.e. the federate
 * services attributesInScope and attributesOutScope respectively are
 * not invoked.
 * @see disableAttributeScopeAdvisorySwitch()
 */
void enableAttributeScopeAdvisorySwitch()
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

/**
 * Sets the AttributeScopeAdvisory (ASA) switch to false. The switch state
 * is hold on the RTIG side. That's why the message
 * DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the ASA switch is false. When enabling the ASA switch
 * the federate is informed by the federate services
 * attributesInScope and attributesOutScope respectively of discovered
 * or registrated but not owned attribute-instances intersecting or
 * leaving its subscription regions.
 * @see enableAttributeScopeAdvisorySwitch()
 *
 * By disabling the ASA switch the federate is no longer informed of
 * changes in attribute-instance scope, i.e. the federate
 * services attributesInScope and attributesOutScope respectively are
 * not invoked.
 */
void disableAttributeScopeAdvisorySwitch()
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

/**
 * Sets the InteractionRelevanceAdvisory (IRA) switch to true. The switch
 * state is hold on the RTIG side. That's why the message
 * ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the IRA switch is true. This causes a delivery of the
 * federate service turnInteractionsOn to a publisher if there are
 * any new subscribers for the federates published interaction
 * classes. If there are no more subscribers a publisher gets the
 * federate service turnInteractionsOff().
 *
 * By disabling the IRA switch the federate is no longer informed by
 * subscriptions to its published interaction classes, i.e. the federate
 * services turnInteractionsOn and turnInteractionsOff respectively are
 * not invoked.
 * @see disableInteractionRelevanceAdvisorySwitch()
 */
void enableInteractionRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

/**
 * Sets the InteractionRelevanceAdvisory (IRA) switch to false. The switch
 * state is hold on the RTIG side. That's why the message
 * DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the IRA switch is true. This causes a delivery of the
 * federate service turnInteractionsOn to a publisher if there are
 * any new subscribers for the federates published interaction
 * classes. If there are no more subscribers a publisher gets the
 * federate service turnInteractionsOff().
 * @see enableInteractionRelevanceAdvisorySwitch()
 *
 * By disabling the IRA switch the federate is no longer informed by
 * subscriptions to its published interaction classes, i.e. the federate
 * services turnInteractionsOn and turnInteractionsOff respectively are
 * not invoked.
 */
void disableInteractionRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted,
	   SaveInProgress, RestoreInProgress, RTIinternalError);

/**
 * Evoke callback (HLA1.3).
 * Non-blocking callback evocation, returns immediately with or without 1 callback.
 * If CERTI has been compiled in HLA13NG_LIBRTI mode then tick
 * may evoke 0 or multiple callbacks (may even be infinite).
 * If CERTI has been compiled in LEGACY_LIBRTI mode then tick
 * may evoke 0 or at most 1 callback.
 * @return Always returns false.
 */
Boolean tick()
    throw (SpecifiedSaveLabelDoesNotExist, ConcurrentAccessAttempted, RTIinternalError);

/**
 * Blocking callback evocation (CERTI extension).
 * Blocks until a callback delivery and then evokes a single callback.
 * Waits until a callback delivery, be careful.
 * @return true if additional callbacks pending, false otherwise
 * @warning This is a non-standard extension of the HLA 1.3/IEEE-1516 API.
 */
Boolean tick2()
    throw (SpecifiedSaveLabelDoesNotExist, ConcurrentAccessAttempted, RTIinternalError);

/**
 * Generic callback evocation (CERTI extension).
 * Blocks up to "minimum" seconds until a callback delivery
 * and then evokes either single (multiple==false) or
 * multiple (multiple==true) callback(s).
 * @param[in] multiple if false will only evoke a single callback
 *                     if true may evoke multiple
 * @param[in] minimum the minimum amount of time spent in the tick call.
 * @param[in] maximum the maximum amount of time spent in the tick call.
 * @return true if additional callbacks pending, false otherwise
 * @warning This is a non-standard extension of the HLA 1.3/IEEE-1516 API.
 */
Boolean __tick_kernel(Boolean multiple, TickTime minimum, TickTime maximum)
    throw (SpecifiedSaveLabelDoesNotExist, ConcurrentAccessAttempted, RTIinternalError);

/**
 * Evoke multiple callbacks (HLA1.3).
 * Blocks up to "minimum" seconds until a callback delivery, then evokes
 * multiple callbacks until no callback is pending, or until "maximum"
 * duration is reached.
 * @param[in] minimum the minimum amount of time spent in the tick call.
 * @param[in] maximum the maximum amount of time spent in the tick call.
 * @return true if additional callbacks pending, false otherwise
 */
Boolean tick(TickTime minimum, TickTime maximum)
    throw (SpecifiedSaveLabelDoesNotExist, ConcurrentAccessAttempted, RTIinternalError);

/** @} end group HLA13_SupportService */

#ifdef CERTI_REALTIME_EXTENSIONS
/**
 * @defgroup CERTI_RealtimeManagement Real-time Management
 * @ingroup  HLA13_RTI_Ambassador
 * @{
 */

/** 
 * Set Priority for RTIA Process (CERTI Real-time extension).
 * @param priority the priority needed for RTIA process (from 0 to 99 on linux system)
 * @param schedPolicy the scheduling policy needed for RTIA process
 *           - SCHED_FIFO or SCHED_RR on Linux system
 * @warning This is a non-standard extension of the HLA 1.3 API.
 */
void setPriorityforRTIAProcess(int priority, unsigned int schedPolicy)
    throw(RTIinternalError) ;

/** 
 * Set Processor Mask affinity for RTIA Process (CERTI Real-time extension).
 * @param mask the mask to be applied for RTIA process (depend on available number of processors)
 * @warning This is a non-standard extension of the HLA 1.3 API.
 */
void setAffinityforRTIAProcess(cpu_set_t mask)
    throw(RTIinternalError) ;

/** @} end group CERTI_RealtimeManagement */
#endif

RTIambassador()
    throw (MemoryExhausted, RTIinternalError);

~RTIambassador()
    throw (RTIinternalError);

RegionToken getRegionToken(Region *)
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted, RegionNotKnown, RTIinternalError);

Region *getRegion(RegionToken)
    throw (FederateNotExecutionMember, ConcurrentAccessAttempted, RegionNotKnown, RTIinternalError);
