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
// $Id: Fed.cc,v 3.2 2003/08/20 18:42:24 breholee Exp $
// ----------------------------------------------------------------------------

#include "Fed.hh"

#include <config.h>
#include "PrettyDebug.hh"
#include "Objects.hh"

#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <memory.h>
#include <fstream>
#include <iostream>

using std::ofstream ;
using std::ios ;
using std::cout ;
using std::endl ;

// Classes
#define CLA_BILLE "Bille"
#define CLA_BOULE "Boule"

// Attributes
#define ATT_POSITION_X "PositionX"
#define ATT_POSITION_Y "PositionY"
#define ATT_COLOR "Color"

// Interactions
#define INT_BING "Bing"

// Parameters
#define PAR_DX "DX"
#define PAR_DY "DY"
#define PAR_BOUL "BoulNum"

// Routing spaces
#define RS_GEO "geo"

// Types
#define TYP_FLOAT "float"
#define TYP_INT "int"

static pdCDebug D("FEDAMB", __FILE__ "> ");

// ----------------------------------------------------------------------------
//! Constructor.
Fed::Fed(RTI::RTIambassador *rtia)
{
    RTIA = rtia ;
    granted = false ;
    paused = false ;
    D.Out(pdInit, "Federate Ambassador created.");
}

// ----------------------------------------------------------------------------
// time advance granted ? resets if yes
bool
Fed::timeAdvanceGranted()
{
    bool ret = granted ;
    granted = false ;
    return ret ;
}

// ----------------------------------------------------------------------------
// init
void
Fed::init(Objects *obj)
{
    objects = obj ;
}

// ----------------------------------------------------------------------------
//! Destructor.
Fed::~Fed()
{
    D.Out(pdTerm, "Federate Ambassador destroyed.");
}

void
Fed::enableLog(const char *file)
{
    logfile = new ofstream(file, ios::out);
    log = logfile->is_open();
}

void
Fed::disableLog()
{
    logfile->close();
    log = false ;
}

// ----------------------------------------------------------------------------
// Publish and subscribe
void
Fed::publishAndSubscribe()
{
    // Get all class and attributes handles
    getHandles();

    // Add PositionX et PositionY to the attribute set
    AttributeHandleSet *AttributeSet = AttributeHandleSetFactory::create(3);
    AttributeSet->add(AttrXID);
    AttributeSet->add(AttrYID);

    // Subscribe to Bille objects.
    RTIA->subscribeObjectClassAttributes(BilleClassID, *AttributeSet, RTI_TRUE);

    // Publish Boule Objects.
    AttributeSet->add(AttrColorID);
    RTIA->publishObjectClass(BouleClassID, *AttributeSet);

    // Publish and subscribe to Bing interactions
    RTIA->subscribeInteractionClass(BingClassID, RTI_TRUE);
    RTIA->publishInteractionClass(BingClassID);

    AttributeSet->empty();

    D.Out(pdInit, "Local Objects and Interactions published and subscribed.");
}

// ----------------------------------------------------------------------------
//! getHandles
/*! get handles of objet/interaction classes and routing spaces
 */
void
Fed::getHandles()
{
    BilleClassID = RTIA->getObjectClassHandle(CLA_BILLE);
    BouleClassID = RTIA->getObjectClassHandle(CLA_BOULE);
    D.Out(pdInit, "BilleClassID = %d, BouleClassID = %d.",
          BilleClassID, BouleClassID);

    // Attributs des classes d'Objets
    AttrXID = RTIA->getAttributeHandle(ATT_POSITION_X, BilleClassID);
    AttrYID = RTIA->getAttributeHandle(ATT_POSITION_Y, BilleClassID);
    AttrColorID = RTIA->getAttributeHandle(ATT_COLOR, BouleClassID);
    D.Out(pdInit, "AttrXID = %d, AttrYID = %d, AttrColorID = %d.",
          AttrXID, AttrYID, AttrColorID);

    // Interactions
    BingClassID = RTIA->getInteractionClassHandle(INT_BING);
    ParamBoulID = RTIA->getParameterHandle(PAR_BOUL, BingClassID);
    ParamDXID = RTIA->getParameterHandle(PAR_DX, BingClassID);
    ParamDYID = RTIA->getParameterHandle(PAR_DY, BingClassID);
    D.Out(pdInit, "BingClassID = %d, DX_ID = %d, DY_ID = %d",
          BingClassID, ParamDXID, ParamDYID);

    // Routing spaces
    GeoID = RTIA->getRoutingSpaceHandle(RS_GEO);
    D[pdInit] << "GeoID = " << GeoID << endl ;
}

// ----------------------------------------------------------------------------
/*! Envoie une interaction, dont les parametres DX et DY ont pour valeur les
  dx et dy de la bille Local, et dont l'etiquette temporelle vaut
  InteractionTime.
*/
void
Fed::sendInteraction(double dx, double dy, const FedTime& InteractionTime,
                     ObjectHandle id)
{
    char buf[512] ;
    ParameterHandleValuePairSet *parameterSet=NULL ;

    parameterSet = ParameterSetFactory::create(3);

    sprintf(buf, "%ld", id);
    parameterSet->add(ParamBoulID, buf, strlen(buf)+1);

    D.Out(pdDebug, "SendInteraction");
    D.Out(pdDebug, "SendInteraction - ParamBoulID= %u", ParamBoulID);
    D.Out(pdDebug, "SendInteraction - x= %d", id);
    D.Out(pdDebug, "SendInteraction - buf= %s", buf);

    // D.Out(pdDebug, "SendInteraction - ParamBoulID= %u, x= %f, buf= %s",
    // ParamBoulID, Id, buf);

    sprintf(buf, "%f", dx);
    parameterSet->add(ParamDXID, buf, strlen(buf)+1);
    D.Out(pdDebug, "SendInteraction - ParamDXID= %u, x= %f, buf= %s",
          ParamDXID, dx, buf);

    sprintf(buf, "%f", dy);
    parameterSet->add(ParamDYID, buf, strlen(buf)+1);
    D.Out(pdDebug, "SendInteraction - ParamDYID= %u, x= %f, buf= %s",
          ParamDYID, dy, buf);

    D.Out(pdRegister, "Sending interaction(DX= %f, DY= %f).", dx, dy);

    try {
        RTIA->sendInteraction(BingClassID, *parameterSet, InteractionTime, "");
    }
    catch (Exception& e) {
        D.Out(pdExcept, "**** Exception sending interaction : %d", &e);
    }

    delete parameterSet ;
}

// ----------------------------------------------------------------------------
//! SendUpdate updates a ball by sending entity position and color.
/*! Envoie un updateAttributeValue pour l'objet Local avec une etiquette
  temporelle de UpdateTime.
*/
void
Fed::sendUpdate(double x, double y, int color, const FedTime& UpdateTime,
                ObjectHandle id)
{
    char buf[512] ;
    AttributeHandleValuePairSet *attributeSet ;

    attributeSet = AttributeSetFactory::create(3);

    D.Out(pdTrace, "SendUpdate.");

    sprintf(buf, "%f", x);
    attributeSet->add(AttrXID, buf, strlen(buf)+1);
    D.Out(pdDebug, "SendUpdate - AttrXID= %u, x= %f, size= %u",
          AttrXID, x, attributeSet->size());

    sprintf(buf, "%f", y);
    attributeSet->add(AttrYID, buf, strlen(buf)+1);
    D.Out(pdDebug, "SendUpdate - AttrYID= %u, y= %f, size= %u",
          AttrYID, y, attributeSet->size());

    sprintf(buf, "%d", color);
    attributeSet->add(AttrColorID, buf, strlen(buf)+1);
    D.Out(pdDebug, "SendUpdate - AttrColorID= %u, color= %f, size= %u",
          AttrColorID, color, attributeSet->size());

    try {
        RTIA->updateAttributeValues(id, *attributeSet, UpdateTime, "");
        // if (log)
        // logfile << string(((RTIfedTime) UpdateTime).getTime()) << " : UAV "
        // << string(Local.x) << " " << string(Local.y) << endl ;
    }
    catch (Exception& e) {
        D.Out(pdExcept, "**** Exception updating attribute values: %d", &e);
    }

    delete attributeSet ;
}

// ============================================================================
// CALLBACKS
// ============================================================================

// ----------------------------------------------------------------------------
//! discoverObjectInstance.
void
Fed::discoverObjectInstance(ObjectHandle theObject,
                            ObjectClassHandle theObjectClass,
                            const char */*theObjectName*/)
    throw (CouldNotDiscover,
           ObjectClassNotKnown,
           InvalidFederationTime,
           FederateInternalError)
{
    if (theObjectClass != BilleClassID) {
        D.Out(pdError, "Object of Unknown Class discovered.");
        throw RTIinternalError();
    }
    objects->discover(theObject);
}

// ----------------------------------------------------------------------------
//! announceSynchronizationPoint.
void
Fed::announceSynchronizationPoint(const char *label, const char *tag)
    throw (FederateInternalError)
{
    if (strcmp(label, "Init") == 0) {
        paused = RTI_TRUE ;
        D.Out(pdProtocol, "announceSynchronizationPoint.");
    }
    else {
        cout << "Unexpected synchronization label" << endl ;
        exit(1);
    }
}

// ----------------------------------------------------------------------------
//! synchronisationPointRegistrationSucceeded.
void
Fed::synchronizationPointRegistrationSucceeded(const char *label)
    throw (FederateInternalError)
{
    D.Out(pdProtocol,
          "CALLBACK : synchronizationPointRegistrationSucceeded with label %s",
          label);
}

// ----------------------------------------------------------------------------
//! synchronisationPointRegistrationFailed.
void
Fed::synchronizationPointRegistrationFailed(const char *label)
    throw (FederateInternalError)
{

    D.Out(pdProtocol,
          "CALLBACK : synchronizationPointRegistrationFailed with label %s",
          label);
}

// ----------------------------------------------------------------------------
//! federationSynchronized.
void
Fed::federationSynchronized(const char *label)
    throw (FederateInternalError)
{
    if (strcmp(label, "Init") == 0) {
        paused = false ;
        D.Out(pdProtocol,
              "CALLBACK : federationSynchronized with label %s", label);
    }
}

// ----------------------------------------------------------------------------
//! receiveInteraction.
void
Fed::receiveInteraction(InteractionClassHandle theInteraction,
                        const ParameterHandleValuePairSet& theParameters,
                        const FedTime& /*theTime*/,
                        const char */*theTag*/,
                        EventRetractionHandle /*theHandle*/)
    throw (InteractionClassNotKnown,
           InteractionParameterNotKnown,
           InvalidFederationTime,
           FederateInternalError)
{
    char *parmValue ;
    ULong valueLength ;
    int dx1 = 0 ;
    int dy1 = 0 ;
    ObjectHandle h1 = 0 ;
    bool bille = false ;

    D.Out(pdTrace, "Fed : receiveInteraction");
    if (theInteraction != BingClassID) {
        D.Out(pdError,
              "CALLBACK receiveInteraction : Unknown Interaction received");
        exit(-1);
    }

    D.Out(pdDebug, "receiveInteraction - nb attributs= %d",
          theParameters.size());

    for (unsigned int j = 0 ; j < theParameters.size(); ++j) {
        ParameterHandle handle = theParameters.getHandle(j);

        valueLength = theParameters.getValueLength(j);
        parmValue = new char[valueLength] ;
        theParameters.getValue(j, parmValue, valueLength);

        if (handle == ParamDXID) {
            if (parmValue != NULL) {
                dx1 = atoi(parmValue);
                // Local.dx = atof(parmValue);
                D.Out(pdDebug, "receiveInteraction(*) - dx= %s", parmValue);
                delete[] parmValue ;
            }
            else
                D.Out(pdError, "Missing Attribute in RAV.");
        }
        else
            if (handle == ParamDYID) {
                if (parmValue != NULL) {
                    dy1 = atoi(parmValue);
                    // Local.dy = atof(parmValue);
                    D.Out(pdDebug, "receiveInteraction(*) - dy= %s", parmValue);
                    delete[] parmValue ;
                }
                else
                    D.Out(pdError, "Missing Attribute in RAV.");
            }
            else
                if (handle == ParamBoulID) {
                    if (parmValue != NULL) {
                        h1 = atoi(parmValue);
                        bille = true ;
                    }
                    else
                        D.Out(pdError, "Unrecognized parameter handle");
                }
    }
    if (bille) {
        objects->receive(h1, dx1, dy1);
    }
}

// ----------------------------------------------------------------------------
//! reflectAttributeValues.
void
Fed::reflectAttributeValues(ObjectHandle theObject,
                            const AttributeHandleValuePairSet& theAttributes,
                            const FedTime& theTime,
                            const char */*theTag*/,
                            EventRetractionHandle /*theHandle*/)
    throw (ObjectNotKnown,
           AttributeNotKnown,
           InvalidFederationTime,
           FederateInternalError)
{
    D.Out(pdTrace, "reflectAttributeValues");

    int i=0 ;
    float oldx, oldy ;

    float x1 = 0 ;
    float y1 = 0 ;

    ULong valueLength ;
    char *attrValue ;


    D.Out(pdDebug, "reflectAttributeValues - nb attributs= %d",
          theAttributes.size());


    for (unsigned int j=0 ; j<theAttributes.size(); j++) {

        AttributeHandle handle = theAttributes.getHandle(j);
        valueLength = theAttributes.getValueLength(j);
        attrValue = new char[valueLength] ;
        theAttributes.getValue(j, attrValue, valueLength);

        if (handle == AttrXID) {
            if (attrValue != NULL) {
                x1 = atof(attrValue);
                delete[] attrValue ;
            }
            else
                D.Out(pdError, "Fed: ERREUR: missing Attribute.");
        }
        else if (handle == AttrYID) {
            if (attrValue != NULL) {
                y1 = atof(attrValue);
                delete[] attrValue ;
            }
            else
                D.Out(pdError, "Fed: ERREUR: missing Attribute.");
        }
        else
            D.Out(pdError, "Fed: ERREUR: handle inconnu.");
    }

    objects->reflect(theObject, x1, y1);
}

// ----------------------------------------------------------------------------
//! removeObjectInstance.
void
Fed::removeObjectInstance(ObjectHandle theObject,
                          //ObjectRemovalReason theReason,
                          const FedTime& /*theTime*/,
                          const char */*theTag*/,
                          EventRetractionHandle /*theHandle*/)
    throw (ObjectNotKnown,
           InvalidFederationTime,
           FederateInternalError)
{
    objects->remove(theObject);
}

// ----------------------------------------------------------------------------
void
Fed::turnInteractionsOn(InteractionClassHandle /*theHandle*/)
    throw (InteractionClassNotPublished, FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
Fed::turnInteractionsOff(InteractionClassHandle /*theHandle*/)
    throw (InteractionClassNotPublished, FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
Fed::startRegistrationForObjectClass(ObjectClassHandle /*theClass*/)
    throw (ObjectClassNotPublished, FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
Fed::stopRegistrationForObjectClass(ObjectClassHandle /*theClass*/)
    throw (ObjectClassNotPublished, FederateInternalError)
{
}

// ----------------------------------------------------------------------------
//! timeAdvanceGrant.
void
Fed::timeAdvanceGrant(const FedTime& /*theTime*/)
    throw (InvalidFederationTime, TimeAdvanceWasNotInProgress,
           FederationTimeAlreadyPassed, FederateInternalError)
{

    granted = true ;
}

// ----------------------------------------------------------------------------
//! timeRegulationEnabled.
void
Fed::timeRegulationEnabled(const FedTime& /*theTime*/)
    throw (InvalidFederationTime, EnableTimeRegulationWasNotPending,
           FederateInternalError)
{
}

// <OWM>

// ----------------------------------------------------------------------------
//! requestAttributeOwnershipRelease.
void
Fed::requestAttributeOwnershipRelease(ObjectHandle,
                                      const AttributeHandleSet&,
                                      const char *)
    throw (ObjectNotKnown, AttributeNotKnown, AttributeNotOwned,
           FederateInternalError)
{
    cout << "requestAttributeOwnershipRelease..." << endl ;
}

// ----------------------------------------------------------------------------
void
Fed::informAttributeOwnership(ObjectHandle theObject, // supplied C1
                              AttributeHandle theAttribute, // supplied C1
                              FederateHandle theOwner) // supplied C1
    throw (ObjectNotKnown,
           AttributeNotKnown,
           FederateInternalError)
{
    cout << "CALLBACK informAttributeOwnership : Attribute " << theAttribute
         << " de l'objet " << theObject << " propriété de " << theOwner
         << endl ;
}

// ----------------------------------------------------------------------------
void
Fed::attributeIsNotOwned(ObjectHandle theObject, // supplied C1
                         AttributeHandle theAttribute) // supplied C1
    throw (ObjectNotKnown,
           AttributeNotKnown,
           FederateInternalError)
{
    cout << "CALLBACK attributeIsNotOwned : Attribute " << theAttribute
         << " de l'objet " << theObject << endl ;
}

// ----------------------------------------------------------------------------
void
Fed::attributeOwnershipUnavailable(ObjectHandle theObject,
                                   const AttributeHandleSet& offeredAttributes)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeAlreadyOwned,
           AttributeAcquisitionWasNotRequested,
           FederateInternalError)
{
    cout << "CALLBACK attributeOwnershipUnavailable Object "<< theObject
         << endl ;


    //Attributs indisponibles
    cout << "CALLBACK Nb d'attributs dans l'AttributeHandleSet : "
         << offeredAttributes.size() << endl ;

    for (unsigned int j=0 ; j < offeredAttributes.size(); j++) {
        AttributeHandle handle = offeredAttributes.getHandle(j);

        if (handle == AttrXID) {
            cout << "CALLBACK AttrXID unavailable" << endl ;
        }
        else if (handle == AttrYID) {
            cout << "CALLBACK AttrYID unavailable" << endl ;
        }
        // else if (handle == AAttrTestID)
        // {
        // cout << "CALLBACK TEST indisponible" << endl ;
        // }
        else
            cout << endl << "CALLBACK Unrecognized handle" << endl ;
    }
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisitionNotification.
void
Fed::attributeOwnershipAcquisitionNotification(
    ObjectHandle theObject,
    const AttributeHandleSet& attrs)
    throw (ObjectNotKnown,
           AttributeNotKnown,
           AttributeAcquisitionWasNotRequested,
           AttributeAlreadyOwned,
           AttributeNotPublished,
           FederateInternalError)
{
    // cout << "attributeOwnershipAcquisitionNotification de : " <<
    // theObject << endl ;

    // //Attributs dont l'acquisition a été obtenue

    // // cout << "Nb d'attributs dans l'AttributeHandleSet : " <<
    // attrs.size() << endl ;

    // for (int j=0 ; j < attrs.size(); j++)
    // {

    // AttributeHandle handle = attrs.getHandle(j);

    // if (handle == AttrXID)
    // {
    // cout << "Acquisition de AttrXID" << endl ;
    // }
    // else if (handle == AttrYID)
    // {
    // cout << "Acquisition de AttrYID" << endl ;
    // }
    // else if (handle == Privilege_To_Delete)
    // {
    // cout << "Acquisition de Privilege_To_Delete" << endl ;
    // }
    // else
    // cout << endl << "Unrecognized handle" << endl ;
    // }

    // int i =0 ;

    // if (theObject != Local .ID)
    // {
    // for (i = 0 ; i <= (RemoteCount-1); i++)
    // {
    // //cout << "Remote[" << i << "]=" << Remote[i].ID << endl ;
    // if (theObject == Remote[i].ID)
    // {
    // cout << "Acquisition effectuée " << endl ;
    // Je_Gere_Boule = RTI_TRUE ;
    // Local.ID=theObject ;
    // Local.dx=Remote[i].dx ;
    // Local.dy=Remote[i].dy ;
    // Local.x=Remote[i].x ;
    // Local.y=Remote[i].y ;
    // // Remote[i].ID = 0 ; //
    // break ;
    // }
    // }
    // //Tassement du tableau
    // for (int j = i ; j <= (RemoteCount-2); j++)
    // Remote[j] = Remote[j+1] ;
    // RemoteCount-- ;
    // }
    // else
    // cout << "Acquisition de " << theObject << " déjà éffectué" << endl ;

}

// ----------------------------------------------------------------------------
//! attributeOwnershipDivestitureNotification.
void
Fed::attributeOwnershipDivestitureNotification(ObjectHandle theObject,
                                               const AttributeHandleSet& attrs)
    throw (ObjectNotKnown,
           AttributeNotKnown,
           AttributeNotOwned,
           AttributeDivestitureWasNotRequested,
           FederateInternalError)
{
    // cout << "attributeOwnershipDivestitureNotification..." << endl ;
    // if (RemoteCount < 20)
    // {
    // cout << "Cession effectuée !" << endl ;
    // Je_Gere_Boule = RTI_FALSE ;
    // Remote[RemoteCount]= (CBille)Local ;
    // Local.ID=0 ;
    // RemoteCount++ ;
    // }
    // else
    // {
    // cout << "Tableau plein " << endl ;
    // }
    // Don_En_Cours= RTI_FALSE ;
}


// ----------------------------------------------------------------------------
//! requestAttributeOwnershipAssumption.
void
Fed::requestAttributeOwnershipAssumption(ObjectHandle theObject,
                                         const AttributeHandleSet& attrs,
                                         const char *theTag)
    throw (ObjectNotKnown,
           AttributeNotKnown,
           AttributeAlreadyOwned,
           AttributeNotPublished,
           FederateInternalError)
{
    // cout << "requestAttributeOwnershipAssumption..." << endl ;

    // //Attributs offerts

    // // cout << "Nb d'attributs dans l'AttributeHandleSet : " <<
    // attrs.size() << endl ;

    // for (int j=0 ; j < attrs.size(); j++)
    // {

    // AttributeHandle handle = attrs.getHandle(j);

    // if (handle == AttrXID)
    // {
    // cout << "AttrXID disponible" << endl ;
    // }
    // else if (handle == AttrYID)
    // {
    // cout << "AttrYID disponible" << endl ;
    // }
    // else if (handle == Privilege_To_Delete)
    // {
    // cout << "Privilege_To_Delete disponible" << endl ;
    // }
    // else
    // cout << endl << "Unrecognized handle" << endl ;
    // }
    // if (!Je_Gere_Boule)
    // {
    // Boule_Disponible = RTI_TRUE ;
    // ObjectOffert = theObject ;
    // }
}

// ----------------------------------------------------------------------------
// timeAdvanceGrant
// ----------------------------------------------------------------------------
// void Fed::timeAdvanceGrant(const FedTime& theTime)
// throw (InvalidFederationTime, TimeAdvanceWasNotInProgress,
// FederateInternalError)
// {
// granted = true ;
// cout << endl << "CALLBACK : timeAdvanceGrant, time = "
// << ((RTIfedTime&)theTime).getTime() << endl ;
// }

// ----------------------------------------------------------------------------
//! confirmAttributeOwnershipAcquisitionCancellation.
void
Fed::confirmAttributeOwnershipAcquisitionCancellation(ObjectHandle theObject,
                                                      const AttributeHandleSet&
                                                      theAttributes)
    throw (ObjectNotKnown,
           AttributeNotKnown,
           AttributeAlreadyOwned,
           AttributeAcquisitionWasNotCanceled,
           FederateInternalError)
{
    // cout << " confirmAttributeOwnershipAcquisitionCancellation de:"
    // << theObject << endl ;

    // int i ;

    // //Attributs dont l'acquisition a été obtenue

    // for (int j=0 ; j < theAttributes.size(); j++)
    // {

    // AttributeHandle handle = theAttributes.getHandle(j);

    // if (handle == AttrXID)
    // {
    // cout << "Annulation de AttrXID" << endl ;
    // }
    // else if (handle == AttrYID)
    // {
    // cout << "Annulation de AttrYID" << endl ;
    // }
    // else if (handle == Privilege_To_Delete)
    // {
    // cout << "Annulation de Privilege_To_Delete" << endl ;
    // }
    // else
    // cout << endl << "Unrecognized handle" << endl ;
    // }
}

// ----------------------------------------------------------------------------
// registerBallInstance
ObjectHandle
Fed::registerBallInstance(const char *s)
{
    return RTIA->registerObjectInstance(BouleClassID, s);
}

// EOF $Id: Fed.cc,v 3.2 2003/08/20 18:42:24 breholee Exp $
