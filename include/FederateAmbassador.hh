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
// $Id: FederateAmbassador.hh,v 3.3 2003/04/23 17:24:09 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_FEDERATE_AMBASSADOR_HH
#define _CERTI_FEDERATE_AMBASSADOR_HH

#include "baseTypes.hh"
#include "RTItypes.hh"
#include "fedtime.hh"
#include "RTIambassador.hh"

using namespace certi ;

namespace certi {

class FederateAmbassador
{
public:

    // Federation Management -------------------------------------------------

    virtual void
    synchronizationPointRegistrationSucceeded(const char *label)
        throw (FederateInternalError) = 0 ;

    virtual void
    synchronizationPointRegistrationFailed(const char *label)
        throw (FederateInternalError) = 0 ;

    virtual void
    announceSynchronizationPoint(const char *label, const char *tag)
        throw (FederateInternalError) = 0 ;

    virtual void
    federationSynchronized(const char *label)
        throw (FederateInternalError) = 0 ;

    virtual void
    initiateFederateSave(const char *)
        throw (FederateInternalError) = 0 ;

    virtual void
    federationSaved(void)
        throw (FederateInternalError) = 0 ;

    // Declaration Management ------------------------------------------------

    virtual void
    startRegistrationForObjectClass(ObjectClassHandle theClass)
        throw (ObjectClassNotPublished, FederateInternalError) = 0 ;

    virtual void
    stopRegistrationForObjectClass(ObjectClassHandle theClass)
        throw (ObjectClassNotPublished, FederateInternalError) = 0 ;

    virtual void
    turnInteractionsOn(InteractionClassHandle theHandle)
        throw (InteractionClassNotPublished, FederateInternalError) = 0 ;

    virtual void
    turnInteractionsOff(InteractionClassHandle theHandle)
        throw (InteractionClassNotPublished, FederateInternalError) = 0 ;

    // Object Management -----------------------------------------------------

    virtual void
    discoverObjectInstance(ObjectHandle theObject,
                           ObjectClassHandle theObjectClass,
                           const char *theObjectName)
        throw (CouldNotDiscover,
               ObjectClassNotKnown,
               InvalidFederationTime,
               FederateInternalError) = 0 ;

    virtual void
    reflectAttributeValues(ObjectHandle theObject,
                           const AttributeHandleValuePairSet& theAttributes,
                           const FedTime& theTime,
                           const char *theTag,
                           EventRetractionHandle theHandle)
        throw (ObjectNotKnown,
               AttributeNotKnown,
               FederateOwnsAttributes,
               InvalidFederationTime,
               FederateInternalError) = 0 ;

    virtual void
    receiveInteraction(InteractionClassHandle theInteraction,
                       const ParameterHandleValuePairSet& theParameters,
                       const FedTime& theTime,
                       const char *theTag,
                       EventRetractionHandle theHandle)
        throw (InteractionClassNotKnown,
               InteractionParameterNotKnown,
               InvalidFederationTime,
               FederateInternalError) = 0 ;

    virtual void
    removeObjectInstance(ObjectHandle theObject,
                         const FedTime& theTime,
                         const char *theTag,
                         EventRetractionHandle theHandle)
        throw (ObjectNotKnown, InvalidFederationTime, FederateInternalError) = 0 ;

    // Ownership Management --------------------------------------------------

    virtual void
    requestAttributeOwnershipAssumption(ObjectHandle theObject,
                                        const AttributeHandleSet& offeredAttributes,
                                        const char *theTag)
        throw (ObjectNotKnown,
               AttributeNotKnown,
               AttributeAlreadyOwned,
               AttributeNotPublished,
               FederateInternalError) = 0 ;

    virtual void
    attributeOwnershipUnavailable(ObjectHandle theObject,
                                  const AttributeHandleSet& offeredAttributes)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeAlreadyOwned,
               AttributeAcquisitionWasNotRequested,
               FederateInternalError) = 0 ;

    virtual void
    attributeOwnershipDivestitureNotification(ObjectHandle theObject,
                                              const AttributeHandleSet& releasedAttributes)
        throw (ObjectNotKnown,
               AttributeNotKnown,
               AttributeNotOwned,
               AttributeDivestitureWasNotRequested,
               FederateInternalError) = 0 ;

    virtual void
    attributeOwnershipAcquisitionNotification(ObjectHandle theObject,
                                              const AttributeHandleSet& securedAttributes)
        throw (ObjectNotKnown,
               AttributeNotKnown,
               AttributeAcquisitionWasNotRequested,
               AttributeAlreadyOwned,
               AttributeNotPublished,
               FederateInternalError) = 0 ;

    virtual void
    requestAttributeOwnershipRelease(ObjectHandle theObject,
                                     const AttributeHandleSet& candidateAttributes,
                                     const char *theTag)
        throw (ObjectNotKnown,
               AttributeNotKnown,
               AttributeNotOwned,
               FederateInternalError) = 0 ;

    virtual void
    informAttributeOwnership(ObjectHandle theObject,
                             AttributeHandle theAttribute,
                             FederateHandle theOwner)
        throw (ObjectNotKnown, AttributeNotKnown, FederateInternalError) = 0 ;

    virtual void
    attributeIsNotOwned(ObjectHandle theObject, AttributeHandle theAttribute)
        throw (ObjectNotKnown, AttributeNotKnown, FederateInternalError) = 0 ;

    virtual void
    confirmAttributeOwnershipAcquisitionCancellation(ObjectHandle theObject,
                                                     const AttributeHandleSet& candidateAttributes)
        throw (ObjectNotKnown,
               AttributeNotKnown,
               AttributeNotDefined,
               AttributeAlreadyOwned,
               AttributeAcquisitionWasNotCanceled,
               FederateInternalError) = 0 ;

    // Time Management -------------------------------------------------------

    virtual void
    timeRegulationEnabled(const FedTime& theTime)
        throw (InvalidFederationTime,
               EnableTimeRegulationWasNotPending,
               FederateInternalError) = 0 ;

    virtual void
    timeAdvanceGrant(const FedTime& theTime)
        throw (InvalidFederationTime,
               TimeAdvanceWasNotInProgress,
               FederationTimeAlreadyPassed,
               FederateInternalError) = 0 ;
};
}

#endif // _CERTI_FEDERATE_AMBASSADOR_HH

// $Id: FederateAmbassador.hh,v 3.3 2003/04/23 17:24:09 breholee Exp $
