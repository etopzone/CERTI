// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: my_fed.hh,v 3.7 2003/04/23 17:24:09 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef TH_MYFED_HH
#define TH_MYFED_HH

#include <config.h>

#include <iostream>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>

#include "RTI.hh"
#include "fedtime.hh"

#include "bille.hh"

#include <fstream>
#include <iostream>

using std::ofstream ;
using std::ios ;
using std::cout ;
using std::endl ;

class Fed : public RTI::FederateAmbassador
{
public:
    // ATTRIBUTS
    bool granted ;
    bool paused ; //!< Pause synchronization before starting application.

    // Object Instances management
    CBoule Local ; // La bille locale.
    CBille Remote[20] ; // tableau des billes des autres federes
    int RemoteCount ; // indice du tableau des billes

    // METHODES

    // Constructeur et Destructeur
    Fed(RTI::RTIambassador*);
    virtual ~Fed();

    void enableLog(const char *);
    void disableLog(void);

    void DeleteObjects(const FedTime& DeletionTime);

    void PublishAndsubscribe(void);

    void RegisterObjects(const char *);

    void sendInteraction(const FedTime& InteractionTime,
                         ObjectHandle Id);

    void SendUpdate(const FedTime& UpdateTime);

    // redefinition des services du RTI
    //(qui sont abstraits dans la classe FederateAmbassador)

    void announceSynchronizationPoint(const char *label, const char *tag)
        throw (FederateInternalError);

    void synchronizationPointRegistrationSucceeded(const char *label)
        throw (FederateInternalError);

    void synchronizationPointRegistrationFailed(const char *label)
        throw (FederateInternalError);

    void federationSynchronized(const char *label)
        throw (FederateInternalError);

    void initiateFederateSave(const char *label)
        throw (FederateInternalError) {}

    void federationSaved(void)
        throw (FederateInternalError) {}

    void removeObjectInstance(ObjectHandle theObject, const FedTime& theTime,
                              const char *theTag,
                              EventRetractionHandle theHandle)
        throw (ObjectNotKnown, InvalidFederationTime, FederateInternalError);

    void timeAdvanceGrant(const FedTime& theTime)
        throw (InvalidFederationTime, TimeAdvanceWasNotInProgress,
               FederationTimeAlreadyPassed, FederateInternalError);

    void startRegistrationForObjectClass(ObjectClassHandle theClass)
        throw (ObjectClassNotPublished,
               FederateInternalError);

    void stopRegistrationForObjectClass(ObjectClassHandle theClass)
        throw (ObjectClassNotPublished,
               FederateInternalError);

    void turnInteractionsOn(InteractionClassHandle theHandle)
        throw (InteractionClassNotPublished, FederateInternalError);

    void turnInteractionsOff(InteractionClassHandle theHandle)
        throw (InteractionClassNotPublished, FederateInternalError);

    void discoverObjectInstance(ObjectHandle theObject,
                                ObjectClassHandle theObjectClass,
                                const char *theObjectName)
        throw (CouldNotDiscover, ObjectClassNotKnown, InvalidFederationTime,
               FederateInternalError);

    void reflectAttributeValues(ObjectHandle theObject,
                                const AttributeHandleValuePairSet& theAttributes,
                                const FedTime& theTime, const char *theTag,
                                EventRetractionHandle theHandle)
        throw (ObjectNotKnown, AttributeNotKnown, InvalidFederationTime,
               FederateInternalError);

    void receiveInteraction(InteractionClassHandle theInteraction,
                            const ParameterHandleValuePairSet& theParameters,
                            const FedTime& theTime, const char *theTag,
                            EventRetractionHandle theHandle)
        throw (InteractionClassNotKnown, InteractionParameterNotKnown,
               InvalidFederationTime, FederateInternalError);

    // void removeObjectInstance(ObjectHandle theObject, const FedTime& theTime,
    // const char *theTag,
    // EventRetractionHandle theHandle)
    // throw (ObjectNotKnown, InvalidFederationTime, FederateInternalError);

    void timeRegulationEnabled(const FedTime& theTime)
        throw (InvalidFederationTime, EnableTimeRegulationWasNotPending,
               FederateInternalError);

    Boolean isAttributeOwnedByFederate(
                                       ObjectHandle theObject,
                                       AttributeHandle theAttribute)
        throw (
               ObjectNotKnown,
               AttributeNotDefined,
               FederateNotExecutionMember,
               ConcurrentAccessAttempted,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);


    void attributeIsNotOwned(
                             ObjectHandle theObject, // supplied C1
                             AttributeHandle theAttribute) // supplied C1
        throw (
               ObjectNotKnown,
               AttributeNotKnown,
               FederateInternalError);

    void informAttributeOwnership(
                                  ObjectHandle theObject, // supplied C1
                                  AttributeHandle theAttribute, // supplied C1
                                  FederateHandle theOwner) // supplied C1
        throw (
               ObjectNotKnown,
               AttributeNotKnown,
               FederateInternalError);

    void attributeOwnershipUnavailable(
                                       ObjectHandle theObject,
                                       const AttributeHandleSet& offeredAttributes)
        throw (
               ObjectNotKnown,
               AttributeNotDefined,
               AttributeAlreadyOwned,
               AttributeAcquisitionWasNotRequested,
               FederateInternalError);

    void attributeOwnershipAcquisitionNotification(
                                                   ObjectHandle theObject,
                                                   const AttributeHandleSet& securedAttributes)
        throw (
               ObjectNotKnown,
               AttributeNotKnown,
               AttributeAcquisitionWasNotRequested,
               AttributeAlreadyOwned,
               AttributeNotPublished,
               FederateInternalError);



    void requestAttributeOwnershipAssumption(
                                             ObjectHandle theObject,
                                             const AttributeHandleSet& offeredAttributes,
                                             const char *theTag)
        throw (
               ObjectNotKnown,
               AttributeNotKnown,
               AttributeAlreadyOwned,
               AttributeNotPublished,
               FederateInternalError);



    void attributeOwnershipDivestitureNotification(
                                                   ObjectHandle theObject,
                                                   const AttributeHandleSet& releasedAttributes)
        throw (
               ObjectNotKnown,
               AttributeNotKnown,
               AttributeNotOwned,
               AttributeDivestitureWasNotRequested,
               FederateInternalError);


    void requestAttributeOwnershipRelease(
                                          ObjectHandle theObject, // supplied C1
                                          const AttributeHandleSet& candidateAttributes, // supplied C4
                                          const char *theTag) // supplied C4
        throw (
               ObjectNotKnown,
               AttributeNotKnown,
               AttributeNotOwned,
               FederateInternalError);

    void confirmAttributeOwnershipAcquisitionCancellation(
                                                          ObjectHandle theObject, // supplied C1
                                                          const AttributeHandleSet& theAttributes) // supplied C4
        throw (
               ObjectNotKnown,
               AttributeNotKnown,
               AttributeAlreadyOwned,
               AttributeAcquisitionWasNotCanceled,
               FederateInternalError);


    // ------------------
    // -- Private Part --
    // ------------------

private:
    ofstream *logfile ;
    bool log ;

    // Federation State management
    RTI::RTIambassador *RTIA ;

    // Object Class management
    ObjectClassHandle BilleClassID ;
    ObjectClassHandle BouleClassID ;

    // On suppose que les numeros d'attributs sont les memes dans Bille et
    // Boule(pour PositionX et PositionY).
    AttributeHandle AttrXID ;
    AttributeHandle AttrYID ;
    AttributeHandle AttrColorID ;

    // Interaction Class management
    InteractionClassHandle BingClassID ;
    ParameterHandle ParamDXID ;
    ParameterHandle ParamDYID ;
    ParameterHandle ParamBoulID ;

    // Private Methods
    void GetHandles(void);
};

#endif // TH_MYFED_HH
