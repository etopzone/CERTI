// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: RTIambassador.hh,v 3.8 2003/02/19 17:20:28 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTI_AMBASSADOR_HH
#define _CERTI_RTI_AMBASSADOR_HH

#include <csignal>
#include <iostream>
using std::cout ;
using std::cerr ;
using std::endl ;

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>

#include <assert.h>

#include "fedtime.hh"

#include "SocketUN.hh"

namespace certi {

class FederateAmbassador ;
class RootObject ;
class Message ;
class CAttributeHandleValuePairSet ;
class AttributeHandleSet ;

/*! This class is used by applications for calls to RTI.
  RTIambassador is separated into two parts. This part uses an UNIX socket
  for sending and receiving data to/from rtia.
*/
class RTIambassador : private SocketUN
{
public:
    RTIambassador(void) throw (MemoryExhausted, RTIinternalError);
    ~RTIambassador(void);

    // Data Integrity

    void objectToString(const char *char_initial,
                        ULong taille,
                        char *char_final);

    void getObjectToStringLength(char *char_initial,
                                 ULong taille_initiale,
                                 ULong& taille);

    // Federation Management -------------------------------------------------
    void
    createFederationExecution(const char *executionName,
                              const char *FED)
        throw (FederationExecutionAlreadyExists,
               ConcurrentAccessAttempted,
               CouldNotOpenFED,
               ErrorReadingFED,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void
    destroyFederationExecution(const char *executionName)
        throw (FederatesCurrentlyJoined,
               FederationExecutionDoesNotExist,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    FederateHandle
    joinFederationExecution(const char *yourName,
                            const char *executionName,
                            FederateAmbassadorPtr federateAmbassadorReference)
        throw (FederateAlreadyExecutionMember,
               FederationExecutionDoesNotExist,
               CouldNotOpenRID, //CERTI
               ErrorReadingRID, //CERTI
               CouldNotOpenFED, //not implemented
               ErrorReadingFED, //not implemented
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    resignFederationExecution(ResignAction theAction)
        throw (FederateOwnsAttributes,
               FederateNotExecutionMember,
               InvalidResignAction,
               ConcurrentAccessAttempted,
               RTIinternalError);

    void
    registerFederationSynchronizationPoint(const char *label,
                                           const char *theTag)
        throw (
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void
    registerFederationSynchronizationPoint(const char *label,
                                           const char *theTag,
                                           const FederateHandleSet& syncSet)
        throw (FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService); //CERTI

    void
    synchronizationPointAchieved(const char *label)
        throw (SynchronizationPointLabelWasNotAnnounced,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void
    requestFederationSave(const char *label,
                          const FedTime& theTime)
        throw (FederationTimeAlreadyPassed,
               InvalidFederationTime,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError,
               UnimplementedService); //CERTI

    void
    requestFederationSave(const char *label)
        throw (FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError,
               UnimplementedService); //CERTI

    void
    federateSaveBegun()
        throw (SaveNotInitiated,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RestoreInProgress, //not implemented
               RTIinternalError,
               UnimplementedService); //CERTI

    void
    federateSaveComplete()
        throw (SaveNotInitiated,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RestoreInProgress, //not implemented
               RTIinternalError,
               UnimplementedService); //CERTI

    void
    federateSaveNotComplete()
        throw (SaveNotInitiated,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RestoreInProgress, //not implemented
               RTIinternalError,
               UnimplementedService); //CERTI

    void
    requestFederationRestore(const char *label)
        throw (SpecifiedSaveLabelDoesNotExist, //CERTI
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError,
               UnimplementedService); //CERTI

    void
    federateRestoreComplete()
        throw (UnknownLabel,
               RestoreNotRequested,
               RTICannotRestore,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RTIinternalError,
               UnimplementedService); //CERTI

    void
    federateRestoreNotComplete()
        throw (UnknownLabel,
               RestoreNotRequested,
               FederateNotExecutionMember,
               RTICannotRestore,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RTIinternalError,
               UnimplementedService); //CERTI

    // Declaration Management ------------------------------------------------

    void
    publishObjectClass(ObjectClassHandle theClass,
                       const AttributeHandleSet& attributeList)
        throw (ObjectClassNotDefined,
               AttributeNotDefined,
               OwnershipAcquisitionPending, //not implemented
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemeted
               RestoreInProgress, //not implemented
               SecurityError, //CERTI
               RTIinternalError);

    void
    unpublishObjectClass(ObjectClassHandle theClass)
        throw (ObjectClassNotDefined,
               ObjectClassNotPublished, //not implemented
               OwnershipAcquisitionPending, //not implemented
               FederateOwnsAttributes, //CERTI
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemeted
               RestoreInProgress, //not implemented
               SecurityError, //CERTI
               RTIinternalError);

    void
    publishInteractionClass(InteractionClassHandle theInteraction)
        throw (InteractionClassNotDefined,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               SecurityError, //RTI
               RTIinternalError);

    void
    unpublishInteractionClass(InteractionClassHandle theInteraction)
        throw (InteractionClassNotDefined,
               InteractionClassNotPublished, //not implemented
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               SecurityError, //RTI
               RTIinternalError);

    void
    subscribeObjectClassAttributes(ObjectClassHandle theClass,
                                   const AttributeHandleSet& attributeList,
                                   Boolean active = RTI_TRUE)
        throw (ObjectClassNotDefined,
               AttributeNotDefined,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               SecurityError, //RTI
               RTIinternalError);

    void
    unsubscribeObjectClass(ObjectClassHandle theClass)
        throw (ObjectClassNotDefined,
               ObjectClassNotSubscribed, //not implemented
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               SecurityError, //RTI
               RTIinternalError);

    void
    subscribeInteractionClass(InteractionClassHandle theClass,
                              Boolean active = RTI_TRUE)
        throw (InteractionClassNotDefined,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               FederateLoggingServiceCalls, //not implemented
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               SecurityError, //RTI
               RTIinternalError);

    void
    unsubscribeInteractionClass(InteractionClassHandle theClass)
        throw (InteractionClassNotDefined,
               InteractionClassNotSubscribed, //not implemented
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               SecurityError, //RTI
               RTIinternalError);

    // Object Management -----------------------------------------------------

    ObjectHandle
    registerObjectInstance(ObjectClassHandle theClass,
                           const char *theObjectName)
        throw (ObjectClassNotDefined,
               ObjectClassNotPublished,
               InvalidObjectHandle, //RTI
               ObjectAlreadyRegistered,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    ObjectHandle
    registerObjectInstance(ObjectClassHandle theClass)
        throw (ObjectClassNotDefined,
               ObjectClassNotPublished,
               InvalidObjectHandle, //RTI
               ObjectAlreadyRegistered,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    EventRetractionHandle
    updateAttributeValues(ObjectHandle theObject,
                          const AttributeHandleValuePairSet& theAttributes,
                          const FedTime& theTime,
                          const char *theTag)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               InvalidFederationTime,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    updateAttributeValues(ObjectHandle theObject,
                          const AttributeHandleValuePairSet& theAttributes,
                          const char *theTag)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError,
               UnimplementedService); //CERTI

    EventRetractionHandle
    sendInteraction(InteractionClassHandle theInteraction,
                    const ParameterHandleValuePairSet& theParameters,
                    const FedTime& theTime,
                    const char *theTag)
        throw (InteractionClassNotDefined,
               InteractionClassNotPublished,
               InteractionParameterNotDefined,
               InvalidFederationTime,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    sendInteraction(InteractionClassHandle theInteraction,
                    const ParameterHandleValuePairSet& theParameters,
                    const char *theTag)
        throw (InteractionClassNotDefined,
               InteractionClassNotPublished,
               InteractionParameterNotDefined,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError,
               UnimplementedService); //CERTI

    EventRetractionHandle
    deleteObjectInstance(ObjectHandle theObject,
                         const FedTime& theTime,
                         const char *theTag)
        throw (ObjectNotKnown,
               DeletePrivilegeNotHeld,
               InvalidFederationTime,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    deleteObjectInstance(ObjectHandle theObject,
                         const char *theTag)
        throw (ObjectNotKnown,
               DeletePrivilegeNotHeld,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError,
               UnimplementedService); //CERTI

    void
    localDeleteObjectInstance(ObjectHandle theObject)
        throw (ObjectNotKnown,
               FederateOwnsAttributes, //not implemented
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError,
               UnimplementedService); //CERTI

    void
    changeAttributeTransportationType(ObjectHandle theObject,
                                      const AttributeHandleSet& theAttributes,
                                      TransportationHandle theType)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               InvalidTransportType, //CERTI
               InvalidTransportationHandle, //not implemented
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    changeInteractionTransportationType(InteractionClassHandle theClass,
                                        TransportationHandle theType)
        throw (InteractionClassNotDefined,
               InteractionClassNotPublished,
               InvalidTransportType, //CERTI
               InvalidTransportationHandle, //not implemented
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    requestObjectAttributeValueUpdate(ObjectHandle theObject,
                                      const AttributeHandleSet& theAttributes)
        throw (ObjectNotKnown, // not implemented
               AttributeNotDefined,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError,
               UnimplementedService); //CERTI

    void
    requestClassAttributeValueUpdate(ObjectClassHandle theClass,
                                     const AttributeHandleSet& theAttributes)
        throw (ObjectClassNotDefined,
               AttributeNotDefined,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError,
               UnimplementedService); //CERTI

    // Ownership Management

    void
    unconditionalAttributeOwnershipDivestiture(ObjectHandle theObject,
                                               const AttributeHandleSet& theAttributes)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);


    void
    negotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                            const AttributeHandleSet& theAttributes,
                                            const char *theTag)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               AttributeAlreadyBeingDivested,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void
    attributeOwnershipAcquisition(ObjectHandle theObject,
                                  const AttributeHandleSet& desiredAttributes,
                                  const char *theTag)
        throw (ObjectNotKnown,
               ObjectClassNotPublished,
               AttributeNotDefined,
               AttributeNotPublished,
               FederateOwnsAttributes,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    AttributeHandleSet*
    attributeOwnershipReleaseResponse(ObjectHandle theObject,
                                      const AttributeHandleSet& theAttributes)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               FederateWasNotAskedToReleaseAttribute,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void
    cancelNegotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                                  const AttributeHandleSet& theAttributes)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               AttributeDivestitureWasNotRequested,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void
    cancelAttributeOwnershipAcquisition(ObjectHandle theObject,
                                        const AttributeHandleSet& theAttributes)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeAlreadyOwned,
               AttributeAcquisitionWasNotRequested,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void
    attributeOwnershipAcquisitionIfAvailable(ObjectHandle theObject,
                                             const AttributeHandleSet& desiredAttributes)
        throw (ObjectNotKnown,
               ObjectClassNotPublished,
               AttributeNotDefined,
               AttributeNotPublished,
               FederateOwnsAttributes,
               AttributeAlreadyBeingAcquired,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void
    queryAttributeOwnership(ObjectHandle theObject,
                            AttributeHandle theAttribute)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotKnown,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    Boolean
    isAttributeOwnedByFederate(ObjectHandle theObject,
                               AttributeHandle theAttribute)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    // Time Management -------------------------------------------------------

    void
    enableTimeRegulation(const FedTime& theFederateTime,
                         const FedTime& theLookahead)
        throw (FederationTimeAlreadyPassed,
               TimeRegulationAlreadyEnabled, //not implemented
               EnableTimeRegulationPending, //not implemented
               TimeAdvanceAlreadyInProgress, //not implemented
               InvalidFederationTime, //not implemented
               InvalidLookahead, //not implemented
               ConcurrentAccessAttempted,
               FederateNotExecutionMember,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    disableTimeRegulation()
        throw (FederationTimeAlreadyPassed,
               TimeRegulationWasNotEnabled, //not implemented
               ConcurrentAccessAttempted,
               FederateNotExecutionMember,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    enableTimeConstrained()
        throw (TimeConstrainedAlreadyEnabled, //not implemented
               EnableTimeConstrainedPending, //not implemented
               TimeAdvanceAlreadyInProgress, //not implemented
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    disableTimeConstrained()
        throw (TimeConstrainedWasNotEnabled, //not implemented
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    timeAdvanceRequest(
                       FedTime& theTime)
        throw (TimeAdvanceAlreadyInProgress,
               FederationTimeAlreadyPassed,
               InvalidFederationTime,
               EnableTimeRegulationPending, //not implemented
               EnableTimeConstrainedPending, //not implemented
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    timeAdvanceRequestAvailable(const FedTime& theTime)
        throw (InvalidFederationTime,
               FederationTimeAlreadyPassed,
               TimeAdvanceAlreadyInProgress,
               EnableTimeRegulationPending,
               EnableTimeConstrainedPending,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    nextEventRequest(const FedTime& theTime)
        throw (TimeAdvanceAlreadyInProgress,
               FederationTimeAlreadyPassed,
               InvalidFederationTime,
               EnableTimeRegulationPending, //not implemented
               EnableTimeConstrainedPending, //not implemented
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    nextEventRequestAvailable(const FedTime& theTime)
        throw (InvalidFederationTime,
               FederationTimeAlreadyPassed,
               TimeAdvanceAlreadyInProgress,
               EnableTimeRegulationPending,
               EnableTimeConstrainedPending,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    flushQueueRequest(const FedTime& theTime)
        throw (TimeAdvanceAlreadyInProgress,
               FederationTimeAlreadyPassed,
               InvalidFederationTime,
               EnableTimeRegulationPending, //not implemented
               EnableTimeConstrainedPending, //not implemented
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    enableAsynchronousDelivery()
        throw (AsynchronousDeliveryAlreadyEnabled,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    disableAsynchronousDelivery()
        throw (AsynchronousDeliveryAlreadyDisabled,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    queryLBTS(FedTime& theTime)
        throw (FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    queryFederateTime(FedTime& theTime)
        throw (FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    queryMinNextEventTime(FedTime& theTime)
        throw (FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    modifyLookahead(const FedTime& theLookahead)
        throw (InvalidFederationTimeDelta, //CERTI
               InvalidLookahead, //not implemented
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    queryLookahead(FedTime& theTime)
        throw (FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    retract(EventRetractionHandle theHandle)
        throw (InvalidRetractionHandle,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError,
               UnimplementedService); //CERTI

    void
    changeAttributeOrderType(ObjectHandle theObject,
                             const AttributeHandleSet& theAttributes,
                             OrderingHandle theType)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               InvalidOrderType, //CERTI
               InvalidOrderingHandle, //not implemented
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    void
    changeInteractionOrderType(InteractionClassHandle theClass,
                               OrderingHandle theType)
        throw (InteractionClassNotDefined,
               InteractionClassNotPublished,
               InvalidOrderType, //CERTI
               InvalidOrderingHandle, //not implemented
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress, //not implemented
               RestoreInProgress, //not implemented
               RTIinternalError);

    // Data Distribution Management ------------------------------------------

    Region*
    createRegion(SpaceHandle theSpace,
                 ULong numberOfExtents)
        throw (SpaceNotDefined,
               InvalidExtents,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    notifyAboutRegionModification(Region &theRegion)
        throw (RegionNotKnown,
               InvalidExtents,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    deleteRegion(Region *theRegion)
        throw (RegionNotKnown,
               RegionInUse,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    ObjectHandle
    registerObjectInstanceWithRegion(ObjectClassHandle theClass,
                                     const char *theObject,
                                     AttributeHandle theAttributes[],
                                     Region *theRegions[],
                                     ULong theNumberOfHandles)
        throw (ObjectClassNotDefined,
               ObjectClassNotPublished,
               AttributeNotDefined,
               AttributeNotPublished,
               RegionNotKnown,
               InvalidRegionContext,
               ObjectAlreadyRegistered,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    ObjectHandle
    registerObjectInstanceWithRegion(ObjectClassHandle theClass,
                                     AttributeHandle theAttributes[],
                                     Region *theRegions[],
                                     ULong theNumberOfHandles)
        throw (ObjectClassNotDefined,
               ObjectClassNotPublished,
               AttributeNotDefined,
               AttributeNotPublished,
               RegionNotKnown,
               InvalidRegionContext,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    associateRegionForUpdates(Region &theRegion,
                              ObjectHandle theObject,
                              const AttributeHandleSet &theAttributes)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               InvalidRegionContext,
               RegionNotKnown,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    unassociateRegionForUpdates(Region &theRegion,
                                ObjectHandle theObject)
        throw (ObjectNotKnown,
               InvalidRegionContext,
               RegionNotKnown,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    subscribeObjectClassAttributesWithRegion(ObjectClassHandle theClass,
                                             Region &theRegion,
                                             const AttributeHandleSet &attributeList,
                                             Boolean active = RTI_TRUE)
        throw (ObjectClassNotDefined,
               AttributeNotDefined,
               RegionNotKnown,
               InvalidRegionContext,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    unsubscribeObjectClassWithRegion(ObjectClassHandle theClass,
                                     Region &theRegion)
        throw (ObjectClassNotDefined,
               RegionNotKnown,
               ObjectClassNotSubscribed,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    subscribeInteractionClassWithRegion(InteractionClassHandle theClass,
                                        Region &theRegion,
                                        Boolean active = RTI_TRUE)
        throw (InteractionClassNotDefined,
               RegionNotKnown,
               InvalidRegionContext,
               FederateLoggingServiceCalls,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    unsubscribeInteractionClassWithRegion(InteractionClassHandle theClass,
                                          Region &theRegion)
        throw (InteractionClassNotDefined,
               InteractionClassNotSubscribed,
               RegionNotKnown,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    EventRetractionHandle
    sendInteractionWithRegion(InteractionClassHandle theInteraction,
                              const ParameterHandleValuePairSet &theParameters,
                              const FedTime& theTime,
                              const char *theTag,
                              const Region &theRegion)
        throw (InteractionClassNotDefined,
               InteractionClassNotPublished,
               InteractionParameterNotDefined,
               InvalidFederationTime,
               RegionNotKnown,
               InvalidRegionContext,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    sendInteractionWithRegion(InteractionClassHandle theInteraction,
                              const ParameterHandleValuePairSet &theParameters,
                              const char *theTag,
                              const Region &theRegion)
        throw (InteractionClassNotDefined,
               InteractionClassNotPublished,
               InteractionParameterNotDefined,
               RegionNotKnown,
               InvalidRegionContext,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    requestClassAttributeValueUpdateWithRegion(ObjectClassHandle theClass,
                                               const AttributeHandleSet &theAttributes,
                                               const Region &theRegion)
        throw (ObjectClassNotDefined,
               AttributeNotDefined,
               RegionNotKnown,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    ObjectClassHandle
    getObjectClassHandle(const char *theName)
        throw (NameNotFound,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError);

    // Support Services ------------------------------------------------------

    char *
    getObjectClassName(ObjectClassHandle theHandle)
        throw (ObjectClassNotDefined,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError);

    AttributeHandle
    getAttributeHandle(const char *theName,
                       ObjectClassHandle whichClass)
        throw (ObjectClassNotDefined,
               NameNotFound,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError);

    char *
    getAttributeName(AttributeHandle theHandle,
                     ObjectClassHandle whichClass)
        throw (ObjectClassNotDefined,
               AttributeNotDefined,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError);

    InteractionClassHandle
    getInteractionClassHandle(const char *theName)
        throw (NameNotFound,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError);

    char *
    getInteractionClassName(InteractionClassHandle theHandle)
        throw (InteractionClassNotDefined,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError);

    ParameterHandle
    getParameterHandle(const char *theName,
                       InteractionClassHandle whichClass)
        throw (InteractionClassNotDefined,
               NameNotFound,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError);

    char *
    getParameterName(ParameterHandle theHandle,
                     InteractionClassHandle whichClass)
        throw (InteractionClassNotDefined,
               InteractionParameterNotDefined,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError);

    ObjectHandle
    getObjectInstanceHandle(const char *theName)
        throw (ObjectNotKnown,
               NameNotFound,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError,
               UnimplementedService);

    char *
    getObjectInstanceName(ObjectHandle theHandle)
        throw (ObjectNotKnown,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError,
               UnimplementedService);

    SpaceHandle
    getRoutingSpaceHandle(const char *)
        throw (NameNotFound,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError);

    char *
    getRoutingSpaceName(const SpaceHandle theHandle)
        throw (SpaceNotDefined,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError,
               UnimplementedService);

    DimensionHandle
    getDimensionHandle(const char *theName,
                       SpaceHandle whichSpace)
        throw (SpaceNotDefined,
               NameNotFound,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError,
               UnimplementedService);

    char *
    getDimensionName(DimensionHandle theHandle,
                     SpaceHandle whichSpace)
        throw (SpaceNotDefined,
               DimensionNotDefined,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError,
               UnimplementedService);

    SpaceHandle
    getAttributeRoutingSpaceHandle(AttributeHandle theHandle,
                                   ObjectClassHandle whichClass)
        throw (ObjectClassNotDefined,
               AttributeNotDefined,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError,
               UnimplementedService);

    ObjectClassHandle
    getObjectClass(ObjectHandle theObject)
        throw (ObjectNotKnown,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError,
               UnimplementedService);

    SpaceHandle
    getInteractionRoutingSpaceHandle(InteractionClassHandle theHandle)
        throw (InteractionClassNotDefined,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError,
               UnimplementedService);

    TransportationHandle
    getTransportationHandle(const char *theName)
        throw (NameNotFound,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError,
               UnimplementedService);

    char *
    getTransportationName(TransportationHandle theHandle)
        throw (InvalidTransportationHandle,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError,
               UnimplementedService);

    OrderingHandle
    getOrderingHandle(const char *theName)
        throw (NameNotFound,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError,
               UnimplementedService);

    char *
    getOrderingName(OrderingHandle theHandle)
        throw (InvalidOrderingHandle,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               RTIinternalError,
               UnimplementedService);

    void
    enableClassRelevanceAdvisorySwitch()
        throw (FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    disableClassRelevanceAdvisorySwitch()
        throw (FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    enableAttributeRelevanceAdvisorySwitch()
        throw (FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    disableAttributeRelevanceAdvisorySwitch()
        throw (FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    enableAttributeScopeAdvisorySwitch()
        throw (FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    disableAttributeScopeAdvisorySwitch()
        throw (FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    enableInteractionRelevanceAdvisorySwitch()
        throw (FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    void
    disableInteractionRelevanceAdvisorySwitch()
        throw (FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError,
               UnimplementedService);

    Boolean
    tick()
        throw (SpecifiedSaveLabelDoesNotExist,
               ConcurrentAccessAttempted,
               RTIinternalError);

    Boolean
    tick(TickTime minimum,
         TickTime maximum)
        throw (SpecifiedSaveLabelDoesNotExist,
               ConcurrentAccessAttempted,
               RTIinternalError);

//     RegionToken
//     getRegionToken(Region *)
//         throw (FederateNotExecutionMember,
//                ConcurrentAccessAttempted,
//                RegionNotKnown,
//                RTIinternalError,
//                UnimplementedService);

//     Region *
//     getRegion(RegionToken)
//         throw (FederateNotExecutionMember,
//                ConcurrentAccessAttempted,
//                RegionNotKnown,
//                RTIinternalError,
//                UnimplementedService);

private:
    pid_t pid_RTIA ; //!< pid associated with rtia fork (private).

    //! Federate Ambassador reference for module calls.
    FederateAmbassador *fed_amb ;

    //! used to prevent reentrant calls (see tick() and executeService()).
    bool is_reentrant ;

    RootObject *_theRootObj ;

    void processException(Message *);
    void executeService(Message *requete, Message *reponse);
    void leave(const char *msg);
};

}

#endif // _CERTI_RTI_AMBASSADOR_HH

// EOF $Id: RTIambassador.hh,v 3.8 2003/02/19 17:20:28 breholee Exp $
