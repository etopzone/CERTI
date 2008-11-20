// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2006  ONERA
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
// ----------------------------------------------------------------------------

#include <config.h>
#include <cassert>
#include <memory>

#include "InteractionSet.hh"
#include "ObjectSet.hh"
#include "ObjectClassSet.hh"
#include "ObjectManagement.hh"
#include "FederationManagement.hh"
#include "PrettyDebug.hh"
#include "TimeManagement.hh"
#include "NM_Classes.hh"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#endif
using std::cout ;
using std::endl ;
using std::string ;

namespace certi {
namespace rtia {

static PrettyDebug D("RTIA_OM", __FILE__);
static PrettyDebug G("GENDOC",__FILE__);

const ObjectManagement::TransportTypeList
ObjectManagement::transportTypeList[] = {
    { "HLAreliable", RELIABLE },
    { "HLAbestEffort", BEST_EFFORT },
    { NULL }
};

const ObjectManagement::OrderTypeList
ObjectManagement::orderTypeList[] = {
    { "Receive", RECEIVE },
    { "Timestamp", TIMESTAMP },
    { NULL }
};

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
    NM_Register_Object req;

    req.federate    = fm->federate ;
    req.federation  = fm->_numero_federation ;
    req.objectClass = the_class ;
    req.setLabel(theObjectName);

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::REGISTER_OBJECT, req.federate));

    e = rep->exception ;

    if (e == e_NO_EXCEPTION) {
        rootObject->registerObjectInstance(fm->federate, the_class, rep->object,
                                           rep->getLabel().c_str());
        return rep->object ;
    }
    else {
        return 0;
    }
}

// ----------------------------------------------------------------------------
//! updateAttributeValues
/** with time
    @param theObjectHandle Object handle
    @param attribArray attribute handles array (pointer)
    @param valueArray value array (pointer)
    @param attribArraySize attribute and value array size
    @param theTime time of the federation
    @param theTag user tag (pointer)
    @param e exception address (may be modified)
*/
EventRetractionHandle
ObjectManagement::updateAttributeValues(ObjectHandle theObjectHandle,
                                        std::vector<AttributeHandle> &attribArray,
                                        std::vector<AttributeValue_t> &valueArray,
                                        UShort attribArraySize,
                                        FederationTime theTime,
                                        std::string theTag,
                                        TypeException &e)
{
    NM_Update_Attribute_Values req;
    int i ;
    bool validCall ;
    EventRetractionHandle evtrHandle;

    G.Out(pdGendoc,"enter ObjectManagement::updateAttributeValues with time");
    validCall = tm->testValidTime(theTime) ;
    if (validCall) {

		// Building request (req NetworkMessage)
    	req.federation = fm->_numero_federation ;
    	req.federate = fm->federate ;
    	req.object = theObjectHandle ;
    	// set Date for UAV with time
    	req.setDate(theTime);
        req.handleArray.resize(attribArraySize) ;
    	req.handleArraySize = attribArraySize ;
        req.sizeValueArray(attribArraySize) ;

        for (i = 0 ; i < attribArraySize ; i++) {
            req.handleArray[i] = attribArray[i] ;
            req.valueArray[i] = valueArray[i] ;
        }

    	req.setLabel(theTag);

       	comm->sendMessage(&req);
    	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(req.getType(), req.federate));
    	e = rep->exception ;
    	evtrHandle = rep->eventRetraction;
    }
    else {
       std::stringstream errorMsg;
       errorMsg << "UAV InvalidFederationTime: "<<std::endl;
       errorMsg << " providedTime =" << theTime<<std::endl;
       errorMsg << " currentTime  =" << tm->requestFederateTime()<<std::endl;
       errorMsg << " lookahead    =" << tm->requestLookahead()<<std::endl;

       D.Out(pdDebug,errorMsg.str().c_str());
       e = e_InvalidFederationTime;
    }

    // FIXME returned evtrHandle carry uninitialized value
    G.Out(pdGendoc,"exit ObjectManagement::updateAttributeValues with time");
    return evtrHandle ;
}

// ----------------------------------------------------------------------------
//! updateAttributeValues
/** without time
    @param theObjectHandle Object handle
    @param attribArray attribute handles array (pointer)
    @param valueArray value array (pointer)
    @param attribArraySize attribute and value array size
    @param theTag user tag (pointer)
    @param e exception address (may be modified)
*/
void
ObjectManagement::updateAttributeValues(ObjectHandle theObjectHandle,
                                        std::vector<AttributeHandle> &attribArray,
                                        std::vector<AttributeValue_t> &valueArray,
                                        UShort attribArraySize,
                                        std::string theTag,
                                        TypeException &e)
{
    NM_Update_Attribute_Values req;
    int i ;

    G.Out(pdGendoc,"enter ObjectManagement::updateAttributeValues without time");
    // Building request (req NetworkMessage)
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObjectHandle ;
    // Do no set Date if without time
    req.handleArray.resize(attribArraySize) ;
    req.handleArraySize = attribArraySize ;
    req.sizeValueArray(attribArraySize) ;

    for (i = 0 ; i < attribArraySize ; i++) {
        req.handleArray[i] = attribArray[i] ;
        req.valueArray[i] = valueArray[i];
    }

    req.setLabel(theTag);

    comm->sendMessage(&req);
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(req.getType(), req.federate));

    e = rep->exception ;
    G.Out(pdGendoc,"exit  ObjectManagement::updateAttributeValues without time");
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
    Message req;

    req.type = Message::DISCOVER_OBJECT_INSTANCE ;
    req.setObject(the_object);
    req.setObjectClass(the_class);
    req.setFederationTime(the_time);
    req.setEventRetraction(the_event);
    req.setName(the_name);

    comm->requestFederateService(&req);

    // Adding discovered object in federate internal object list.
    rootObject->registerObjectInstance(fm->federate, the_class, the_object,
                                       req.getName().c_str());
}

// ----------------------------------------------------------------------------
//! reflectAttributeValues with time
void
ObjectManagement::reflectAttributeValues(ObjectHandle the_object,
                                         std::vector <AttributeHandle> &the_attributes,
                                         std::vector <AttributeValue_t> &the_values,
                                         UShort the_size,
                                         FederationTime the_time,
                                         const char *the_tag,
                                         EventRetractionHandle the_event,
                                         TypeException &)
{
    Message req;

    G.Out(pdGendoc,"enter ObjectManagement::reflectAttributeValues with time");
    req.type = Message::REFLECT_ATTRIBUTE_VALUES ;
    req.setObject(the_object);
    req.setFederationTime(the_time);
    req.setEventRetraction(the_event);
    req.setTag(the_tag);
    req.setAttributes(the_attributes, the_values, the_size);
    // true for RAV without time
    req.setBoolean(true);

    comm->requestFederateService(&req);
    G.Out(pdGendoc,"exit  ObjectManagement::reflectAttributeValues with time");
}

// ----------------------------------------------------------------------------
//! reflectAttributeValues without time
void
ObjectManagement::reflectAttributeValues(ObjectHandle the_object,
                                         std::vector <AttributeHandle> &the_attributes,
                                         std::vector <AttributeValue_t> &the_values,
                                         UShort the_size,
                                         const char *the_tag,
                                         TypeException &)
{
    Message req;

    G.Out(pdGendoc,"enter ObjectManagement::reflectAttributeValues without time");
    req.type = Message::REFLECT_ATTRIBUTE_VALUES ;
    req.setObject(the_object);
    req.setTag(the_tag);
    req.setAttributes(the_attributes, the_values, the_size);
    // false for RAV without time
    req.setBoolean(false);

    comm->requestFederateService(&req);
    G.Out(pdGendoc,"exit  ObjectManagement::reflectAttributeValues without time");
}

// ----------------------------------------------------------------------------
//! sendInteraction with time
EventRetractionHandle
ObjectManagement::sendInteraction(InteractionClassHandle theInteraction,
                                  std::vector <ParameterHandle> &paramArray,
                                  std::vector <ParameterValue_t> &valueArray,
                                  UShort paramArraySize,
                                  FederationTime theTime,
                                  std::string theTag,
				  RegionHandle region,
                                  TypeException &e)
{
    NM_Send_Interaction req;
    bool validCall ;
    EventRetractionHandle evtrHandle;

    validCall = tm->testValidTime(theTime) ;
    if (validCall) {

       G.Out(pdGendoc,"ObjectManagement::sendInteraction with time") ;
       // Local test to know if interaction is correct.
       rootObject->Interactions->isReady(fm->federate,
                                         theInteraction,
                                         paramArray,
                                         paramArraySize);

       // Building network message (req) to RTIG.
       req.interactionClass = theInteraction ;
       // true for UAV with time
       req.setDate(theTime);
       req.region = region ;
       req.federation = fm->_numero_federation ;
       req.federate = fm->federate ;
       req.handleArray.resize(paramArraySize) ;
       req.handleArraySize = paramArraySize ;
       req.sizeValueArray(paramArraySize) ;

       for (int i=0 ; i<paramArraySize ; i++) {
        	req.handleArray[i] = paramArray[i] ;
                req.valueArray[i] = valueArray[i];
       }

       req.setLabel(theTag);

       // Send network message and then wait for answer.
       comm->sendMessage(&req);
       std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::SEND_INTERACTION, req.federate));
       e = rep->exception ;
       evtrHandle = rep->eventRetraction;
    }
    else {
       e = e_InvalidFederationTime ;
    }

    // FIXME returned evtrHandle carry uninitialized value
    return evtrHandle ;
}

// ----------------------------------------------------------------------------
//! sendInteraction without time
void
ObjectManagement::sendInteraction(InteractionClassHandle theInteraction,
                                  std::vector <ParameterHandle> &paramArray,
                                  std::vector <ParameterValue_t> &valueArray,
                                  UShort paramArraySize,
                                  std::string theTag,
				  RegionHandle region,
                                  TypeException &e)
{
    NM_Send_Interaction req;
    G.Out(pdGendoc,"ObjectManagement::sendInteraction without time");
    // Local test to know if interaction is correct.
    rootObject->Interactions->isReady(fm->federate,
                                      theInteraction,
                                      paramArray,
                                      paramArraySize);

    // Building network message (req) to RTIG.
    req.interactionClass = theInteraction ;
    req.region     = region ;
    req.federation = fm->_numero_federation ;
    req.federate   = fm->federate ;
    req.handleArray.resize(paramArraySize) ;
    req.handleArraySize = paramArraySize ;
    req.sizeValueArray(paramArraySize) ;

    for (int i=0 ; i<paramArraySize ; i++) {
        	req.handleArray[i] = paramArray[i] ;
                req.valueArray[i] = valueArray[i];
    }

    req.setLabel(theTag);

    // Send network message and then wait for answer.
    comm->sendMessage(&req);
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::SEND_INTERACTION, req.federate));

    e = rep->exception ;

} /* end of sendInteraction */

// ----------------------------------------------------------------------------
//! receiveInteraction with time
void
ObjectManagement::receiveInteraction(InteractionClassHandle the_interaction,
                                     std::vector <ParameterHandle> &the_parameters,
                                     std::vector <ParameterValue_t> &the_values,
                                     UShort the_size,
                                     FederationTime the_time,
                                     const char *the_tag,
                                     EventRetractionHandle the_event,
                                     TypeException &)
{
    Message req;

    req.type = Message::RECEIVE_INTERACTION ;
    req.setInteractionClass(the_interaction);
    req.setFederationTime(the_time);
    req.setEventRetraction(the_event);
    req.setTag(the_tag);
    req.setParameters(the_parameters, the_values, the_size);
    req.setBoolean(true);
    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
//! receiveInteraction without time
void
ObjectManagement::receiveInteraction(InteractionClassHandle the_interaction,
                                     std::vector <ParameterHandle> &the_parameters,
                                     std::vector <ParameterValue_t> &the_values,
                                     UShort the_size,
                                     const char *the_tag,
                                     TypeException &)
{
    Message req;

    req.type = Message::RECEIVE_INTERACTION ;
    req.setInteractionClass(the_interaction);
    req.setTag(the_tag);
    req.setParameters(the_parameters, the_values, the_size);
    req.setBoolean(false);
    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
//! deleteObject with time
EventRetractionHandle
ObjectManagement::deleteObject(ObjectHandle theObjectHandle,
			       FederationTime theTime,
                               std::string theTag,
                               TypeException &e)
{
    NM_Delete_Object req;

    req.object     = theObjectHandle ;
    req.setDate(theTime);
    req.federation = fm->_numero_federation ;
    req.federate   = fm->federate ;

    req.setLabel(theTag);
    comm->sendMessage(&req);
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DELETE_OBJECT, req.federate));

    e = rep->exception ;

    if (e == e_NO_EXCEPTION) {
        rootObject->deleteObjectInstance(fm->federate, theObjectHandle, theTag);
    }

    return rep->eventRetraction ;
} /* end deleteObject */

// ----------------------------------------------------------------------------
//! deleteObject without time
void
ObjectManagement::deleteObject(ObjectHandle theObjectHandle,
                               std::string theTag,
                               TypeException &e)
{
    NM_Delete_Object req;

    req.object     = theObjectHandle ;
    req.federation = fm->_numero_federation ;
    req.federate   = fm->federate ;

    req.setLabel(theTag);
    comm->sendMessage(&req);
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DELETE_OBJECT, req.federate));

    e = rep->exception ;

    if (e == e_NO_EXCEPTION) {
        rootObject->deleteObjectInstance(fm->federate, theObjectHandle, theTag);
    }
} /* end of deleteObject */

// ----------------------------------------------------------------------------
//! removeObject with time
void
ObjectManagement::removeObject(ObjectHandle the_object,
                               FederateHandle the_federate,
			                   FederationTime theTime,
                               const char *the_tag,
                               EventRetractionHandle the_event,
                               TypeException &)

{
    Message req;

    req.type = Message::REMOVE_OBJECT_INSTANCE ;
    req.setObject(the_object);
    req.setEventRetraction(the_event);
    req.setTag(the_tag);
    req.setFederationTime(theTime);
    req.setBoolean(true);

    comm->requestFederateService(&req);

    rootObject->deleteObjectInstance(the_federate, the_object, theTime, the_tag);
}

// ----------------------------------------------------------------------------
//! removeObject without time
void
ObjectManagement::removeObject(ObjectHandle the_object,
                               FederateHandle the_federate,
                               const char *the_tag,
                               TypeException &)
{
    Message req;

    req.type = Message::REMOVE_OBJECT_INSTANCE ;
    req.setObject(the_object);
    req.setTag(the_tag);
    req.setBoolean(false);

    comm->requestFederateService(&req);

    rootObject->deleteObjectInstance(the_federate, the_object, the_tag);
}

// ----------------------------------------------------------------------------
//! changeAttributeTransportType
EventRetractionHandle
ObjectManagement::changeAttributeTransportType(ObjectHandle theObjectHandle,
                                               std::vector <AttributeHandle> &attribArray,
                                               UShort attribArraySize,
                                               TransportType theType,
                                               TypeException &e)
{
    NM_Change_Attribute_Transport_Type req;
    int i ;

    req.object = theObjectHandle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.transport = theType ;
    req.handleArray.resize(attribArraySize) ;

    for (i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(
                      NetworkMessage::CHANGE_ATTRIBUTE_TRANSPORT_TYPE,
                      req.federate));

    e = rep->exception ;

    return rep->eventRetraction ;
}

// ----------------------------------------------------------------------------
//! changeAttributeOrderType
EventRetractionHandle
ObjectManagement::changeAttributeOrderType(ObjectHandle theObjectHandle,
                                           std::vector <AttributeHandle> &attribArray,
                                           UShort attribArraySize,
                                           OrderType theType,
                                           TypeException &e)
{
    NM_Change_Attribute_Order_Type req ;
    int i ;

    req.object = theObjectHandle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.order = theType ;
    req.handleArraySize = attribArraySize ;
    req.handleArray.resize(attribArraySize) ;

    for (i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;


    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::CHANGE_ATTRIBUTE_ORDER_TYPE, req.federate));

    e = rep->exception ;

    return rep->eventRetraction ;
}

// ----------------------------------------------------------------------------
//! changeInteractionTransportType
EventRetractionHandle
ObjectManagement::changeInteractionTransportType(InteractionClassHandle id,
                                                 TransportType theType,
                                                 TypeException &e)
{
    NM_Change_Interaction_Transport_Type req;

    req.interactionClass = id ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.transport = theType ;

    comm->sendMessage(&req);
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::CHANGE_INTERACTION_TRANSPORT_TYPE, req.federate));
    e = rep->exception ;

    return rep->eventRetraction ;
}

// ----------------------------------------------------------------------------
//! changeInteractionOrderType
EventRetractionHandle
ObjectManagement::changeInteractionOrderType(InteractionClassHandle id,
                                             OrderType theType,
                                             TypeException &e)
{
    NM_Change_Interaction_Order_Type req;

    req.interactionClass = id ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.order = theType ;

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::CHANGE_INTERACTION_ORDER_TYPE, req.federate));

    e = rep->exception ;

    return rep->eventRetraction ;
} /* end of changeInteractionOrderType */

// ----------------------------------------------------------------------------
//! requestObjectAttributeValueUpdate
void
ObjectManagement::requestObjectAttributeValueUpdate(ObjectHandle handle,
                                                    std::vector <AttributeHandle> &attribs,
                                                    UShort attribArraySize,
                                                    TypeException &e)
{
    NM_Request_Object_Attribute_Value_Update req;

    G.Out(pdGendoc,"enter ObjectManagement::requestObjectAttributeValueUpdate");

    req.object = handle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.handleArray.resize(attribArraySize) ;

    for (int i = 0 ; i < attribArraySize ; i++) {
        req.handleArray[i] = attribs[i] ;
    }

    req.handleArraySize = attribArraySize ;
    comm->sendMessage(&req);
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE,
                      req.federate));
    e = rep->exception ;
    G.Out(pdGendoc,"exit  ObjectManagement::requestObjectAttributeValueUpdate");

} /* end of requestObjectAttributeValueUpdate */


// --------------------------------------
// -- 4.15 provideAttributeValueUpdate --
// --------------------------------------

void
ObjectManagement::provideAttributeValueUpdate(ObjectHandle the_object,
                                              std::vector <AttributeHandle> &the_attributes,
                                              UShort attribArraySize,
                                              TypeException &)
{
    Message req;

    G.Out(pdGendoc,"enter ObjectManagement::provideAttributeValueUpdate");
    req.type = Message::PROVIDE_ATTRIBUTE_VALUE_UPDATE ;
    req.setObject(the_object);
    req.handleArraySize = attribArraySize ;
    req.handleArray.resize(req.handleArraySize) ;
    for (int i = 0 ; i < attribArraySize ; i++) {
        req.handleArray[i] = the_attributes[i] ;
    }

    comm->requestFederateService(&req);
    G.Out(pdGendoc,"exit  ObjectManagement::provideAttributeValueUpdate");
}

// ------------------
// -- 4.16 retract --
// ------------------

void ObjectManagement::retract(EventRetractionHandle /*theHandle*/,
                               TypeException & /*e*/)
{
    throw RTIinternalError("ObjectManagement::retract not implemented.");
}


// ----------------------------
// -- 4.17 reflectRetraction --
// ----------------------------

void
ObjectManagement::reflectRetraction(EventRetractionHandle,
                                    TypeException &)
{
    throw RTIinternalError("ObjectManagement::reflectRetraction not implemented.");
}

// ----------------------------------------------------------------------------
//! getObjectClassHandle.
ObjectClassHandle
ObjectManagement::getObjectClassHandle(const char *theName)
{
    ObjectClassHandle handle ;
    G.Out(pdGendoc,"enter ObjectManagement::getObjectClassHandle");
    try {
         handle = rootObject->ObjectClasses->getObjectClassHandle(theName);
         G.Out(pdGendoc,"exit  ObjectManagement::getObjectClassHandle");
         return handle;
         }
    catch (NameNotFound) {
         G.Out(pdGendoc,"exit  ObjectManagement::getObjectClassHandle on NameNotFound");
         throw NameNotFound(theName);
         }
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
    AttributeHandle handle ;
    G.Out(pdGendoc,"enter ObjectManagement::getAttributeHandle");
    try {
        handle = rootObject->ObjectClasses->getAttributeHandle(theName,
                                                         theClassHandle);
        G.Out(pdGendoc,"exit  ObjectManagement::getAttributeHandle");
        return handle;
        }
     catch (NameNotFound) {
         G.Out(pdGendoc,"exit  ObjectManagement::getAttributeHandle on NameNotFound");
         throw NameNotFound(theName);
        }
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
const std::string
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
const std::string&
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

// ----------------------------------------------------------------------------
//! getTransportationHandle
TransportType
ObjectManagement::getTransportationHandle(const char *theName)
{
    for(const TransportTypeList *item = transportTypeList; item->name != NULL; item++) {
        if(strcmp(theName, item->name) == 0)
            return item->type;
    }

    throw NameNotFound(theName);
}

// ----------------------------------------------------------------------------
//! getTransportationName
const char *
ObjectManagement::getTransportationName(TransportType theType)
{
    for(const TransportTypeList *item = transportTypeList; item->name != NULL; item++) {
        if(theType == item->type)
            return item->name;
    }

    throw InvalidTransportationHandle("");
}

// ----------------------------------------------------------------------------
//! getOrderingHandle
OrderType
ObjectManagement::getOrderingHandle(const char *theName)
{
    for(const OrderTypeList *item = orderTypeList; item->name != NULL; item++) {
        if(strcmp(theName, item->name) == 0)
            return item->type;
    }

    throw NameNotFound(theName);
}

// ----------------------------------------------------------------------------
//! getOrderingName
const char *
ObjectManagement::getOrderingName(OrderType theType)
{
    for(const OrderTypeList *item = orderTypeList; item->name != NULL; item++) {
        if(theType == item->type)
            return item->name;
    }

    throw InvalidOrderingHandle("");
}

// --------------------------------------
// setAttributeScopeAdvisorySwitch
// --------------------------------------
void
ObjectManagement::
setAttributeScopeAdvisorySwitch(bool state, TypeException &e) {
    G.Out(pdGendoc,"enter ObjectManagement::setAttributeScopeAdvisorySwitch");

    NM_Set_Attribute_Scope_Advisory_Switch msg ;

    e = e_NO_EXCEPTION ;

    msg.federation = fm->_numero_federation ;
    msg.federate = fm->federate ;

    if (state) {
        msg.attributeScopeAdvisorySwitchOn();
    }
    else {
        msg.attributeScopeAdvisorySwitchOff();
    }

    comm->sendMessage(&msg);

    G.Out(pdGendoc,"exit ObjectManagement::setAttributeScopeAdvisorySwitch");
}

// --------------------------------------
// -- 6.13 attributesInScope
// --------------------------------------
void
ObjectManagement::
attributesInScope(ObjectHandle theObject,
                  const std::vector <AttributeHandle> &attribArray,
                  const UShort attribArraySize,
                  TypeException &e) {
    Message req;

    G.Out(pdGendoc,"enter ObjectManagement::attributesInScope");

    req.type = Message::ATTRIBUTES_IN_SCOPE;
    req.setObject(theObject);
    req.handleArraySize = attribArraySize;
    req.handleArray.resize(attribArraySize);

    for (int i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i];

    comm->requestFederateService(&req);

    G.Out(pdGendoc,"exit  ObjectManagement::attributesInScope");
}

// --------------------------------------
// -- 6.14 attributesOutOfScope
// --------------------------------------

void
ObjectManagement::
attributesOutOfScope(ObjectHandle theObject,
                     const std::vector <AttributeHandle> &attribArray,
                     const UShort attribArraySize,
                     TypeException &e) {
    Message req;

    G.Out(pdGendoc,"enter ObjectManagement::attributesOutScope");

    req.type = Message::ATTRIBUTES_OUT_OF_SCOPE;
    req.setObject(theObject);
    req.handleArraySize = attribArraySize;
    req.handleArray.resize(attribArraySize);

    for (int i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i];

    comm->requestFederateService(&req);

    G.Out(pdGendoc,"exit  ObjectManagement::attributesOutScope");
}

// --------------------------------------
// setAttributeRelevanceAdvisorySwitch
// --------------------------------------
void
ObjectManagement::
setAttributeRelevanceAdvisorySwitch(bool state, TypeException &e) {
    G.Out(pdGendoc,"enter ObjectManagement::setAttributeRelevanceAdvisorySwitch");

    NM_Set_Attribute_Relevance_Advisory_Switch msg ;

    e = e_NO_EXCEPTION ;

    msg.federation = fm->_numero_federation ;
    msg.federate = fm->federate ;

    if (state) {
        msg.attributeRelevanceAdvisorySwitchOn();
    }
    else {
        msg.attributeRelevanceAdvisorySwitchOff();
    }

    comm->sendMessage(&msg);

    G.Out(pdGendoc,"exit ObjectManagement::setAttributeRelevanceAdvisorySwitch");
}

// --------------------------------------
// -- 6.17 turnUpdatesOnForObjectInstance
// --------------------------------------

void
ObjectManagement::
turnUpdatesOnForObjectInstance(ObjectHandle theObject,
                           const std::vector <AttributeHandle> &attribArray,
                           const UShort attribArraySize,
                           TypeException &e) {
    Message req;

    G.Out(pdGendoc,"enter ObjectManagement::turnUpdatesOnForObjectInstance");

    req.type = Message::TURN_UPDATES_ON_FOR_OBJECT_INSTANCE;
    req.setObject(theObject);
    req.handleArraySize = attribArraySize;
    req.handleArray.resize(attribArraySize);

    for (int i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i];

    comm->requestFederateService(&req);

    G.Out(pdGendoc,"exit  ObjectManagement::turnUpdatesOnForObjectInstance");
}

// --------------------------------------
// -- 6.18 turnUpdatesOffForObjectInstance
// --------------------------------------

void
ObjectManagement::
turnUpdatesOffForObjectInstance(ObjectHandle theObject,
                         const std::vector <AttributeHandle> &attribArray,
                         const UShort attribArraySize,
                         TypeException &e) {
    Message req;

    G.Out(pdGendoc,"enter ObjectManagement::turnUpdatesOffForObjectInstance");

    req.type = Message::TURN_UPDATES_OFF_FOR_OBJECT_INSTANCE;
    req.setObject(theObject);
    req.handleArraySize = attribArraySize;
    req.handleArray.resize(attribArraySize);

    for (int i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i];

    comm->requestFederateService(&req);

    G.Out(pdGendoc,"exit  ObjectManagement::turnUpdatesOffForObjectInstance");
}

}} // namespace certi/rtia

