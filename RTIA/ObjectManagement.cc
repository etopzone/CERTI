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
// $Id: ObjectManagement.cc,v 3.4 2003/02/17 09:17:03 breholee Exp $
// ----------------------------------------------------------------------------

#include "ObjectManagement.hh"

namespace certi {
namespace rtia {

static pdCDebug D("RTIA_OM", "(RTIA OM) ");

ObjectManagement::ObjectManagement(Communications *GC,
                                   FederationManagement *GF,
                                   RootObject *theRootObj)
{
    comm = GC ;
    fm = GF ;
    rootObject = theRootObj ;
}

ObjectManagement::~ObjectManagement()
{
}

// -------------------
// -- 4.1 requestID --
// -------------------

void
ObjectManagement::requestID(ObjectHandlecount idCount,
                            ObjectHandle &firstID,
                            ObjectHandle &lastID,
                            TypeException &e)
{
    NetworkMessage req, rep ;

    req.type = m_REQUEST_ID ;
    req.idCount = idCount ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;

    comm->sendMessage(&req);

    comm->waitMessage(&rep, m_REQUEST_ID, req.federate);

    e = rep.exception ;
    firstID = rep.firstId ;
    lastID = rep.lastId ;
}


// ------------------------
// -- 4.2 registerObject --
// ------------------------

ObjectHandle
ObjectManagement::registerObject(ObjectClassHandle theClassHandle,
                                 const char *theObjectName,
                                 FederationTime,
                                 FederationTime,
                                 TypeException & e)
{
    NetworkMessage req, rep ;

    req.type = m_REGISTER_OBJECT ;
    req.federate = fm->federate ;
    req.federation = fm->_numero_federation ;
    req.objectClass = theClassHandle ;
    strcpy(req.label, (char *)theObjectName);

    // BUG: A quoi servent Date et Heure ?

    comm->sendMessage(&req);

    comm->waitMessage(&rep, m_REGISTER_OBJECT, req.federate);

    e = rep.exception ;

    if (e == e_NO_EXCEPTION) {
        rootObject->ObjectClasses->registerInstance(fm->federate,
                                                    theClassHandle,
                                                    rep.object,
                                                    rep.label);

        // La reponse contient le numero d'objet(object)
        return rep.object ;
    }
    else
        return 0 ;
}


// -------------------------------
// -- 4.3 updateAttributeValues --
// -------------------------------

EventRetractionHandle
ObjectManagement::updateAttributeValues(ObjectHandle theObjectHandle,
                                        AttributeHandle *attribArray,
                                        AttributeValue *valueArray,
                                        UShort attribArraySize,
                                        FederationTime theTime,
                                        const char *theTag,
                                        TypeException &e)
{
    NetworkMessage req, rep ;
    int i ;

    // Mise en place de la requete
    req.type = m_UPDATE_ATTRIBUTE_VALUES ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObjectHandle ;
    req.date = theTime ;

    req.handleArraySize = attribArraySize ;

    for (i = 0 ; i < attribArraySize ; i++) {
        req.handleArray [i] = attribArray [i] ;
        req.setValue(i, valueArray [i]);
    }

    strcpy(req.label, theTag);

    // Emission et reception de la reponse

    comm->sendMessage(&req);

    comm->waitMessage(&rep, m_UPDATE_ATTRIBUTE_VALUES, req.federate);

    e = rep.exception ;

    return rep.eventRetraction ;
}


// ------------------------
// -- 4.4 discoverObject --
// ------------------------

void
ObjectManagement::discoverObject(ObjectHandle theObjectHandle,
                                 ObjectClassHandle theObjectClassHandle,
                                 const char *theObjectName,
                                 FederationTime theTime,
                                 EventRetractionHandle theHandle,
                                 TypeException &)
{
    Message req, rep ;

    req.type = DISCOVER_OBJECT ;
    req.object = theObjectHandle ;
    req.objectClass = theObjectClassHandle ;
    req.date = theTime ;
    req.eventRetraction = theHandle ;
    req.setName((char *)theObjectName);

    // BUG: Et on fait quoi de la reponse ?
    comm->requestFederateService(&req, &rep);

    // Insertion de l'objet decouvert dans la liste interne du federe
    rootObject->ObjectClasses->registerInstance(fm->federate,
                                                theObjectClassHandle,
                                                req.object,
                                                req.getName());

}


// --------------------------------
// -- 4.5 reflectAttributeValues --
// --------------------------------

void
ObjectManagement::reflectAttributeValues(ObjectHandle theObjectHandle,
                                         AttributeHandle *attribArray,
                                         AttributeValue *valueArray,
                                         UShort attribArraySize,
                                         FederationTime theTime,
                                         const char *theTag,
                                         EventRetractionHandle theHandle,
                                         TypeException &)
{
    Message req, rep ;
    int i ;

    req.type = REFLECT_ATTRIBUTE_VALUES ;
    req.object = theObjectHandle ;
    req.date = theTime ;
    req.eventRetraction = theHandle ;
    req.setTag(theTag);

    req.handleArraySize = attribArraySize ;

    for (i = 0 ; i < attribArraySize ; i++) {
        req.handleArray [i] = attribArray[i] ;
        req.setValue(i, valueArray [i]);
    }

    // BUG: Et on fait quoi de la reponse ?
    comm->requestFederateService(&req, &rep);
}


// -------------------------
// -- 4.6 sendInteraction --
// -------------------------

EventRetractionHandle
ObjectManagement::sendInteraction(InteractionClassHandle theInteraction,
                                  ParameterHandle *paramArray,
                                  ParameterValue *valueArray,
                                  UShort paramArraySize,
                                  FederationTime theTime,
                                  const char *theTag,
                                  TypeException &e)
{
    NetworkMessage req, rep ;

    // Test local pour savoir si l'interaction est correcte.

    rootObject->Interactions->isReady(fm->federate,
                                      theInteraction,
                                      paramArray,
                                      paramArraySize);

    // Preparation du message au RTI.

    req.type = m_SEND_INTERACTION ;
    req.interactionClass = theInteraction ;
    req.date = theTime ;

    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;

    req.handleArraySize = paramArraySize ;

    for (int i=0 ; i<paramArraySize ; i++)
        {
            req.handleArray [i] = paramArray [i] ;
            req.setValue(i, valueArray [i]);
        }

    strcpy(req.label, theTag);

    // Emission et attente de la reponse.

    comm->sendMessage(&req);

    comm->waitMessage(&rep, m_SEND_INTERACTION, req.federate);

    e = rep.exception ;

    return rep.eventRetraction ;
}


// ----------------------------
// -- 4.7 receiveInteraction --
// ----------------------------

void
ObjectManagement::receiveInteraction(InteractionClassHandle theInteraction,
                                     ParameterHandle *paramArray,
                                     ParameterValue *valueArray,
                                     UShort paramArraySize,
                                     FederationTime theTime,
                                     const char *theTag,
                                     EventRetractionHandle theHandle,
                                     TypeException &)
{
    int i ;
    Message req, rep ;

    req.type = RECEIVE_INTERACTION ;
    req.interactionClass = theInteraction ;
    req.date = theTime ;
    req.eventRetraction = theHandle ;
    req.setTag(theTag);

    req.handleArraySize = paramArraySize ;
    for (i = 0 ; i < paramArraySize ; i++) {
        req.handleArray[i] = paramArray[i] ;
        req.setValue(i, valueArray[i]);
    }

    // BUG: On fait quoi de la reponse ?
    comm->requestFederateService(&req, &rep);
}


// ----------------------
// -- 4.8 deleteObject --
// ----------------------

EventRetractionHandle
ObjectManagement::deleteObject(ObjectHandle theObjectHandle,
                               const char *theTag,
                               TypeException &e)
{
    NetworkMessage req, rep ;

    req.type = m_DELETE_OBJECT ;
    req.object = theObjectHandle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;

    strcpy(req.label, theTag);

    comm->sendMessage(&req);

    comm->waitMessage(&rep, m_DELETE_OBJECT, req.federate);

    e = rep.exception ;

    if (e == e_NO_EXCEPTION)
        rootObject->ObjectClasses->deleteObject(fm->federate,
                                                theObjectHandle,
                                                theTag);

    return rep.eventRetraction ;
}


// --------------------------
// -- 4.9(1) removeObject --
// --------------------------

void
ObjectManagement::removeObject(ObjectHandle theObjectHandle,
                               FederateHandle theFederateHandle,
                               const char *theTag,
                               EventRetractionHandle theHandle,
                               TypeException &)
{
    Message req, rep ;

    req.type = REMOVE_OBJECT ;
    req.object = theObjectHandle ;
    req.eventRetraction = theHandle ;
    req.setTag(theTag);

    // BUG: On fait quoi de la reponse ?
    comm->requestFederateService(&req, &rep);

    rootObject->ObjectClasses->deleteObject(theFederateHandle,
                                            theObjectHandle,
                                            theTag);
}

// --------------------------
// -- 4.9(2) removeObject --
// --------------------------

void
ObjectManagement::removeObject(ObjectHandle,
                               ObjectRemovalReason,
                               TypeException &)
{
    printf("ObjectManagement.cc: RemoveObject(2) not implemented.\n");
    throw RTIinternalError();
}


// ---------------------------------------
// -- 4.10 changeAttributeTransportType --
// ---------------------------------------

EventRetractionHandle
ObjectManagement::changeAttributeTransportType(ObjectHandle theObjectHandle,
                                               AttributeHandle *attribArray,
                                               UShort attribArraySize,
                                               TransportType theType,
                                               TypeException &e)
{
    NetworkMessage req, rep ;
    int i ;

    req.type = m_CHANGE_ATTRIBUTE_TRANSPORT_TYPE ;
    req.object = theObjectHandle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.transport = theType ;

    for (i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    comm->sendMessage(&req);

    comm->waitMessage(&rep,
                      m_CHANGE_ATTRIBUTE_TRANSPORT_TYPE,
                      req.federate);

    e = rep.exception ;

    return rep.eventRetraction ;
}


// -----------------------------------
// -- 4.11 changeAttributeOrderType --
// -----------------------------------

EventRetractionHandle
ObjectManagement::changeAttributeOrderType(ObjectHandle theObjectHandle,
                                           AttributeHandle *attribArray,
                                           UShort attribArraySize,
                                           OrderType theType,
                                           TypeException &e)
{
    NetworkMessage req, rep ;
    int i ;

    req.type = m_CHANGE_ATTRIBUTE_ORDER_TYPE ;
    req.object = theObjectHandle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.order = theType ;

    for (i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;


    comm->sendMessage(&req);

    comm->waitMessage(&rep, m_CHANGE_ATTRIBUTE_ORDER_TYPE, req.federate);

    e = rep.exception ;

    return rep.eventRetraction ;
}


// -----------------------------------------
// -- 4.12 changeInteractionTransportType --
// -----------------------------------------

EventRetractionHandle ObjectManagement::
changeInteractionTransportType(InteractionClassHandle theClassID,
                               TransportType theType,
                               TypeException &e)
{
    NetworkMessage req, rep ;

    req.type = m_CHANGE_INTERACTION_TRANSPORT_TYPE ;
    req.interactionClass = theClassID ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.transport = theType ;

    comm->sendMessage(&req);

    comm->waitMessage(&rep,
                      m_CHANGE_INTERACTION_TRANSPORT_TYPE,
                      req.federate);

    e = rep.exception ;

    return rep.eventRetraction ;
}


// -------------------------------------
// -- 4.13 changeInteractionOrderType --
// -------------------------------------

EventRetractionHandle
ObjectManagement::changeInteractionOrderType(InteractionClassHandle theClassID,
                                             OrderType theType,
                                             TypeException &e)
{
    NetworkMessage req, rep ;

    req.type = m_CHANGE_INTERACTION_ORDER_TYPE ;
    req.interactionClass = theClassID ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.order = theType ;

    comm->sendMessage(&req);

    comm->waitMessage(&rep, m_CHANGE_INTERACTION_ORDER_TYPE, req.federate);

    e = rep.exception ;

    return rep.eventRetraction ;
}


// ------------------------------------------------
// -- 4.14(1) requestObjectAttributeValueUpdate --
// ------------------------------------------------

EventRetractionHandle ObjectManagement::
requestObjectAttributeValueUpdate(ObjectHandle theObjectHandle,
                                  AttributeHandle *attribArray,
                                  UShort attribArraySize,
                                  TypeException &e)
{
    NetworkMessage req, rep ;
    int i ;

    req.type = m_REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE ;
    req.object = theObjectHandle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;

    for (i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    req.handleArraySize = attribArraySize ;

    comm->sendMessage(&req);

    comm->waitMessage(&rep,
                      m_REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE,
                      req.federate);

    e = rep.exception ;

    return rep.eventRetraction ;
}


// --------------------------------------
// -- 4.15 provideAttributeValueUpdate --
// --------------------------------------

void
ObjectManagement::provideAttributeValueUpdate(ObjectHandle,
                                              AttributeValue &,
                                              TypeException &)
{
    printf("ObjectManagement.cc: "
           "provideAttributeValueUpdate not implemented\n");
    throw RTIinternalError();
}


// ------------------
// -- 4.16 retract --
// ------------------

void ObjectManagement::retract(EventRetractionHandle /*theHandle*/,
                               TypeException & /*e*/)
{
    printf("GO.cc: retract not implemented.\n");
    throw RTIinternalError();
}


// ----------------------------
// -- 4.17 reflectRetraction --
// ----------------------------

void
ObjectManagement::reflectRetraction(EventRetractionHandle,
                                    TypeException &)
{
    printf("GO.cc: reflectRetraction not implemented.\n");
    throw RTIinternalError();
}

// ----------------------------------------------------------------------------
//! getObjectClassHandle.
ObjectClassHandle
ObjectManagement::getObjectClassHandle(const char *theName)
{
    return rootObject->ObjectClasses->getObjectClassHandle(theName);
}


// ----------------------------------------------------------------------------
//! getObjectClassName.
const char *
ObjectManagement::getObjectClassName(ObjectClassHandle theHandle)
{
    return rootObject->ObjectClasses->getObjectClassName(theHandle);
}

// ----------------------------------------------------------------------------
//! getAttributeHandle.
AttributeHandle
ObjectManagement::getAttributeHandle(const char *theName,
                                     ObjectClassHandle theClassHandle)
{
    return rootObject->ObjectClasses->getAttributeHandle(theName,
                                                         theClassHandle);
}

// ----------------------------------------------------------------------------
//! getAttributeName.
const char *
ObjectManagement::getAttributeName(AttributeHandle theHandle,
                                   ObjectClassHandle theClassHandle)
{
    return rootObject->ObjectClasses->getAttributeName(theHandle,
                                                       theClassHandle);
}

// ----------------------------------------------------------------------------
//! getInteractionClassHandle.
InteractionClassHandle
ObjectManagement::getInteractionClassHandle(const char *theName)
{
    return rootObject->Interactions->getInteractionClassHandle(theName);
}

// ----------------------------------------------------------------------------
//! getInteractionClassName.
const char *
ObjectManagement::
getInteractionClassName(InteractionClassHandle theClassHandle)
{
    return
        rootObject->Interactions->getInteractionClassName(theClassHandle);
}

// ----------------------------------------------------------------------------
//! getParameterHandle.
ParameterHandle
ObjectManagement::getParameterHandle(const char *theParameterName,
                                     InteractionClassHandle theClassHandle)
{
    return rootObject->Interactions->getParameterHandle(theParameterName,
                                                        theClassHandle);
}

// ----------------------------------------------------------------------------
//! getParameterName.
const char *
ObjectManagement::getParameterName(ParameterHandle theParameterHandle,
                                   InteractionClassHandle theClassHandle)
{
    return rootObject->Interactions->getParameterName(theParameterHandle,
                                                      theClassHandle);
}

}} // namespace certi/rtia

// $Id: ObjectManagement.cc,v 3.4 2003/02/17 09:17:03 breholee Exp $
