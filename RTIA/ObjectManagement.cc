// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: ObjectManagement.cc,v 3.17 2005/04/30 16:38:39 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "ObjectManagement.hh"

#include "FederationManagement.hh"
#include "PrettyDebug.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

using std::cout ;
using std::endl ;
using std::string ;

namespace certi {
namespace rtia {

static pdCDebug D("RTIA_OM", __FILE__);

ObjectManagement::ObjectManagement(Communications *GC,
                                   FederationManagement *GF,
                                   RootObject *theRootObj)
    : comm(GC), 
      fm(GF),
      rootObject(theRootObj) { }

ObjectManagement::~ObjectManagement() { }

// ----------------------------------------------------------------------------
//! registerObject
ObjectHandle
ObjectManagement::registerObject(ObjectClassHandle the_class,
                                 const char *theObjectName,
                                 FederationTime,
                                 FederationTime,
                                 TypeException & e)
{
    NetworkMessage req, rep ;

    req.type = NetworkMessage::REGISTER_OBJECT ;
    req.federate = fm->federate ;
    req.federation = fm->_numero_federation ;
    req.objectClass = the_class ;
    strcpy(req.label, (char *) theObjectName);

    comm->sendMessage(&req);

    comm->waitMessage(&rep, NetworkMessage::REGISTER_OBJECT, req.federate);

    e = rep.exception ;

    if (e == e_NO_EXCEPTION) {
        rootObject->registerObjectInstance(fm->federate, the_class, rep.object,
                                           rep.label);
        return rep.object ;
    }
    else
        return 0 ;
}

// ----------------------------------------------------------------------------
//! updateAttributeValues
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
    req.type = NetworkMessage::UPDATE_ATTRIBUTE_VALUES ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObjectHandle ;
    req.date = theTime ;

    req.handleArraySize = attribArraySize ;

    for (i = 0 ; i < attribArraySize ; i++) {
        req.handleArray[i] = attribArray[i] ;
        req.setValue(i, valueArray[i]);
    }

    strcpy(req.label, theTag);

    comm->sendMessage(&req);
    comm->waitMessage(&rep, NetworkMessage::UPDATE_ATTRIBUTE_VALUES, req.federate);

    e = rep.exception ;

    return rep.eventRetraction ;
}

// ----------------------------------------------------------------------------
//! discoverObject.
void
ObjectManagement::discoverObject(ObjectHandle the_object,
                                 ObjectClassHandle the_class,
                                 const char *the_name,
                                 FederationTime the_time,
                                 EventRetractionHandle the_event,
                                 TypeException &)
{
    Message req, rep ;

    req.type = Message::DISCOVER_OBJECT_INSTANCE ;
    req.setObject(the_object);
    req.setObjectClass(the_class);
    req.setFederationTime(the_time);
    req.setEventRetraction(the_event);
    req.setName(the_name);

    comm->requestFederateService(&req, &rep);

    // Adding discovered object in federate internal object list.
    rootObject->registerObjectInstance(fm->federate, the_class, the_object,
                                       req.getName());
}

// ----------------------------------------------------------------------------
//! reflectAttributeValues.
void
ObjectManagement::reflectAttributeValues(ObjectHandle the_object,
                                         AttributeHandle *the_attributes,
                                         AttributeValue *the_values,
                                         UShort the_size,
                                         FederationTime the_time,
                                         const char *the_tag,
                                         EventRetractionHandle the_event,
                                         TypeException &)
{
    Message req, rep ;

    req.type = Message::REFLECT_ATTRIBUTE_VALUES ;
    req.setObject(the_object);
    req.setFederationTime(the_time);
    req.setEventRetraction(the_event);
    req.setTag(the_tag);
    req.setAttributes(the_attributes, the_values, the_size);

    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! sendInteraction
EventRetractionHandle
ObjectManagement::sendInteraction(InteractionClassHandle theInteraction,
                                  ParameterHandle *paramArray,
                                  ParameterValue *valueArray,
                                  UShort paramArraySize,
                                  FederationTime theTime,
                                  const char *theTag,
				  RegionHandle region,
                                  TypeException &e)
{
    NetworkMessage req, rep ;

    // Test local pour savoir si l'interaction est correcte.
    rootObject->Interactions->isReady(fm->federate,
                                      theInteraction,
                                      paramArray,
                                      paramArraySize);

    // Preparation du message au RTI.
    req.type = NetworkMessage::SEND_INTERACTION ;
    req.interactionClass = theInteraction ;
    req.date = theTime ;
    req.region = region ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;

    req.handleArraySize = paramArraySize ;

    for (int i=0 ; i<paramArraySize ; i++) {
	req.handleArray[i] = paramArray[i] ;
	req.setValue(i, valueArray[i]);
    }

    strcpy(req.label, theTag);

    // Emission et attente de la reponse.
    comm->sendMessage(&req);
    comm->waitMessage(&rep, NetworkMessage::SEND_INTERACTION, req.federate);

    e = rep.exception ;

    return rep.eventRetraction ;
}

// ----------------------------------------------------------------------------
//! receiveInteraction
void
ObjectManagement::receiveInteraction(InteractionClassHandle the_interaction,
                                     ParameterHandle *the_parameters,
                                     ParameterValue *the_values,
                                     UShort the_size,
                                     FederationTime the_time,
                                     const char *the_tag,
                                     EventRetractionHandle the_event,
                                     TypeException &)
{
    Message req, rep ;

    req.type = Message::RECEIVE_INTERACTION ;
    req.setInteractionClass(the_interaction);
    req.setFederationTime(the_time);
    req.setEventRetraction(the_event);
    req.setTag(the_tag);
    req.setParameters(the_parameters, the_values, the_size);

    // BUG: On fait quoi de la reponse ?
    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! deleteObject
EventRetractionHandle
ObjectManagement::deleteObject(ObjectHandle theObjectHandle,
                               const char *theTag,
                               TypeException &e)
{
    NetworkMessage req, rep ;

    req.type = NetworkMessage::DELETE_OBJECT ;
    req.object = theObjectHandle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;

    strcpy(req.label, theTag);
    comm->sendMessage(&req);
    comm->waitMessage(&rep, NetworkMessage::DELETE_OBJECT, req.federate);

    e = rep.exception ;

    if (e == e_NO_EXCEPTION)
        rootObject->ObjectClasses->deleteObject(fm->federate,
                                                theObjectHandle,
                                                theTag);

    return rep.eventRetraction ;
}

// ----------------------------------------------------------------------------
//! removeObject
void
ObjectManagement::removeObject(ObjectHandle the_object,
                               FederateHandle the_federate,
                               const char *the_tag,
                               EventRetractionHandle the_event,
                               TypeException &)
{
    Message req, rep ;

    req.type = Message::REMOVE_OBJECT_INSTANCE ;
    req.setObject(the_object);
    req.setEventRetraction(the_event);
    req.setTag(the_tag);

    // BUG: On fait quoi de la reponse ?
    comm->requestFederateService(&req, &rep);

    rootObject->ObjectClasses->deleteObject(the_federate, the_object, the_tag);
}

// ----------------------------------------------------------------------------
//! removeObject
void
ObjectManagement::removeObject(ObjectHandle,
                               ObjectRemovalReason,
                               TypeException &)
{
    printf("ObjectManagement.cc: RemoveObject(2) not implemented.\n");
    throw RTIinternalError("");
}

// ----------------------------------------------------------------------------
//! changeAttributeTransportType
EventRetractionHandle
ObjectManagement::changeAttributeTransportType(ObjectHandle theObjectHandle,
                                               AttributeHandle *attribArray,
                                               UShort attribArraySize,
                                               TransportType theType,
                                               TypeException &e)
{
    NetworkMessage req, rep ;
    int i ;

    req.type = NetworkMessage::CHANGE_ATTRIBUTE_TRANSPORT_TYPE ;
    req.object = theObjectHandle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.transport = theType ;

    for (i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    comm->sendMessage(&req);

    comm->waitMessage(&rep,
                      NetworkMessage::CHANGE_ATTRIBUTE_TRANSPORT_TYPE,
                      req.federate);

    e = rep.exception ;

    return rep.eventRetraction ;
}

// ----------------------------------------------------------------------------
//! changeAttributeOrderType
EventRetractionHandle
ObjectManagement::changeAttributeOrderType(ObjectHandle theObjectHandle,
                                           AttributeHandle *attribArray,
                                           UShort attribArraySize,
                                           OrderType theType,
                                           TypeException &e)
{
    NetworkMessage req, rep ;
    int i ;

    req.type = NetworkMessage::CHANGE_ATTRIBUTE_ORDER_TYPE ;
    req.object = theObjectHandle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.order = theType ;

    for (i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;


    comm->sendMessage(&req);

    comm->waitMessage(&rep, NetworkMessage::CHANGE_ATTRIBUTE_ORDER_TYPE, req.federate);

    e = rep.exception ;

    return rep.eventRetraction ;
}

// ----------------------------------------------------------------------------
//! changeInteractionTransportType
EventRetractionHandle
ObjectManagement::changeInteractionTransportType(InteractionClassHandle id,
                                                 TransportType theType,
                                                 TypeException &e)
{
    NetworkMessage req, rep ;

    req.type = NetworkMessage::CHANGE_INTERACTION_TRANSPORT_TYPE ;
    req.interactionClass = id ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.transport = theType ;

    comm->sendMessage(&req);
    comm->waitMessage(&rep, NetworkMessage::CHANGE_INTERACTION_TRANSPORT_TYPE, req.federate);
    e = rep.exception ;

    return rep.eventRetraction ;
}

// ----------------------------------------------------------------------------
//! changeInteractionOrderType
EventRetractionHandle
ObjectManagement::changeInteractionOrderType(InteractionClassHandle id,
                                             OrderType theType,
                                             TypeException &e)
{
    NetworkMessage req, rep ;

    req.type = NetworkMessage::CHANGE_INTERACTION_ORDER_TYPE ;
    req.interactionClass = id ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.order = theType ;

    comm->sendMessage(&req);

    comm->waitMessage(&rep, NetworkMessage::CHANGE_INTERACTION_ORDER_TYPE, req.federate);

    e = rep.exception ;

    return rep.eventRetraction ;
}

// ----------------------------------------------------------------------------
//! requestObjectAttributeValueUpdate
EventRetractionHandle
ObjectManagement::requestObjectAttributeValueUpdate(ObjectHandle handle,
                                                    AttributeHandle *attribs,
                                                    UShort attribArraySize,
                                                    TypeException &e)
{
    NetworkMessage req, rep ;
    req.type = NetworkMessage::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE ;
    req.object = handle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;

    for (int i = 0 ; i < attribArraySize ; i++) {
        req.handleArray[i] = attribs[i] ;
    }

    req.handleArraySize = attribArraySize ;
    comm->sendMessage(&req);
    comm->waitMessage(&rep, NetworkMessage::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE,
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
    throw RTIinternalError("");
}


// ------------------
// -- 4.16 retract --
// ------------------

void ObjectManagement::retract(EventRetractionHandle /*theHandle*/,
                               TypeException & /*e*/)
{
    printf("GO.cc: retract not implemented.\n");
    throw RTIinternalError("");
}


// ----------------------------
// -- 4.17 reflectRetraction --
// ----------------------------

void
ObjectManagement::reflectRetraction(EventRetractionHandle,
                                    TypeException &)
{
    printf("GO.cc: reflectRetraction not implemented.\n");
    throw RTIinternalError("");
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
string
ObjectManagement::getObjectClassName(ObjectClassHandle theHandle)
{
    return rootObject->ObjectClasses->getObjectClassName(theHandle);
}

// ----------------------------------------------------------------------------
//! Returns the object instance handle.
ObjectHandle
ObjectManagement::getObjectInstanceHandle(const char *the_name)
{
    return rootObject->objects->getObjectInstanceHandle(the_name);
}

// ----------------------------------------------------------------------------
//! Returns the object instance name.
const char *
ObjectManagement::getObjectInstanceName(ObjectHandle the_object)
{
    return rootObject->objects->getObjectInstanceName(the_object);
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

// ----------------------------------------------------------------------------
//! getObjectClass
ObjectClassHandle
ObjectManagement::getObjectClass(ObjectHandle object)
{
    return rootObject->objects->getObjectClass(object);
}

}} // namespace certi/rtia

// $Id: ObjectManagement.cc,v 3.17 2005/04/30 16:38:39 breholee Exp $
