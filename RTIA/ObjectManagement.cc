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
#include <iostream>

#include "InteractionSet.hh"
#include "ObjectSet.hh"
#include "ObjectClassSet.hh"
#include "ObjectManagement.hh"
#include "FederationManagement.hh"
#include "PrettyDebug.hh"
#include "TimeManagement.hh"
#include "NM_Classes.hh"
#include "M_Classes.hh"

using std::cout ;
using std::endl ;

namespace certi {
namespace rtia {

static PrettyDebug D("RTIA_OM", __FILE__);
static PrettyDebug G("GENDOC",__FILE__);

const ObjectManagement::TransportTypeList
ObjectManagement::transportTypeList[] = {
		{ "HLAreliable", RELIABLE },
		{ "HLAbestEffort", BEST_EFFORT }
};

const ObjectManagement::OrderTypeList
ObjectManagement::orderTypeList[] = {
		{ "Receive", RECEIVE },
		{ "Timestamp", TIMESTAMP }
};

ObjectManagement::ObjectManagement(Communications *GC,
		FederationManagement *GF,
		RootObject *theRootObj)
: comm(GC),
  fm(GF),
  rootObject(theRootObj) { }

ObjectManagement::~ObjectManagement() { }

// ----------------------------------------------------------------------------
void
ObjectManagement::reserveObjectName(const std::string &newObjName, TypeException &e)
{
    NM_Reserve_Object_Instance_Name req;

    //  Empty strings not allowed
    if (newObjName.size() <= 0 ||
        // According to spec, the HLA prefix is reserved for RTI-internal objects.
        newObjName.compare(0, 3, "HLA") == 0 )
    {
        e = e_IllegalName;
    } else {
        req.setFederation(fm->_numero_federation);
        req.setFederate(fm->federate);

        req.setObjectName(newObjName);

        comm->sendMessage(&req);
    }
    // JvY TODO: Finish handling on other side (and return path)
}


// ----------------------------------------------------------------------------
//! registerObject
ObjectHandle
ObjectManagement::registerObject(ObjectClassHandle the_class,
		const std::string& theObjectName,
		FederationTime,
		FederationTime,
		TypeException & e)
{
	NM_Register_Object req;

	req.setFederate(fm->federate) ;
	req.setFederation(fm->_numero_federation);
	req.setObjectClass(the_class);
	req.setLabel(theObjectName);

	comm->sendMessage(&req);

	std::auto_ptr<NM_Register_Object> rep(static_cast<NM_Register_Object*>(comm->waitMessage(NetworkMessage::REGISTER_OBJECT, req.getFederate())));

	e = rep->getException() ;

	if (e == e_NO_EXCEPTION) {
		rootObject->registerObjectInstance(fm->federate, the_class, rep->getObject(),
				rep->getLabel());
		return rep->getObject() ;
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
		const std::vector<AttributeHandle> &attribArray,
		const std::vector<AttributeValue_t> &valueArray,
		uint32_t attribArraySize,
		FederationTime theTime,
		const std::string& theTag,
		TypeException &e)
{
	NM_Update_Attribute_Values req;
	bool validCall ;
	EventRetractionHandle evtrHandle;

	G.Out(pdGendoc,"enter ObjectManagement::updateAttributeValues with time");
	validCall = tm->testValidTime(theTime) ;
	if (validCall) {

		// Building request (req NetworkMessage)
		req.setFederation(fm->_numero_federation);
		req.setFederate(fm->federate);
		req.setObject(theObjectHandle);
		// set Date for UAV with time
		req.setDate(theTime);
		req.setAttributesSize(attribArraySize);
		req.setValuesSize(attribArraySize);
		for (uint32_t i = 0 ; i < attribArraySize ; ++i) {
			req.setAttributes(attribArray[i],i) ;
			req.setValues(valueArray[i],i) ;
		}

		req.setLabel(theTag);

		comm->sendMessage(&req);
		std::auto_ptr<NM_Update_Attribute_Values> rep(static_cast<NM_Update_Attribute_Values*>(comm->waitMessage(req.getMessageType(), req.getFederate())));
		e = rep->getException() ;
		evtrHandle = rep->getEvent();
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
		const std::vector<AttributeHandle> &attribArray,
		const std::vector<AttributeValue_t> &valueArray,
		uint32_t attribArraySize,
		const std::string& theTag,
		TypeException &e)
{
	NM_Update_Attribute_Values req;

	G.Out(pdGendoc,"enter ObjectManagement::updateAttributeValues without time");
	// Building request (req NetworkMessage)
	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);
	req.setObject(theObjectHandle);
	// Do no set Date if without time
	req.setAttributesSize(attribArraySize) ;
	req.setValuesSize(attribArraySize);

	for (uint32_t i = 0 ; i < attribArraySize ; ++i) {
		req.setAttributes(attribArray[i],i) ;
		req.setValues(valueArray[i],i);
	}

	req.setLabel(theTag);

	comm->sendMessage(&req);
	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(req.getMessageType(), req.getFederate()));

	e = rep->getException() ;
	G.Out(pdGendoc,"exit  ObjectManagement::updateAttributeValues without time");
}

// ----------------------------------------------------------------------------
//! discoverObject.
void
ObjectManagement::discoverObject(ObjectHandle the_object,
		ObjectClassHandle the_class,
		const std::string& the_name,
		FederationTime the_time,
		EventRetractionHandle the_event,
		TypeException &)
{
	M_Discover_Object_Instance req;
	EventRetraction  event;

	req.setObject(the_object);
	req.setObjectClass(the_class);
	req.setDate(the_time);
	event.setSN(the_event);
	req.setEventRetraction(event);
	req.setObjectName(the_name);

	comm->requestFederateService(&req);

	/* The registration of the discovered object in RootObject
	 * MUST have been done just before putting DO message in FIFO.
	 * see RTIA_network.cc
	 */
}

// ----------------------------------------------------------------------------
//! reflectAttributeValues with time
void
ObjectManagement::reflectAttributeValues(ObjectHandle the_object,
		const std::vector <AttributeHandle> &the_attributes,
		const std::vector <AttributeValue_t> &the_values,
		uint16_t the_size,
		FederationTime the_time,
		const std::string& the_tag,
		EventRetractionHandle the_event,
		TypeException &)
{
	M_Reflect_Attribute_Values req;
	EventRetraction event;

	G.Out(pdGendoc,"enter ObjectManagement::reflectAttributeValues with time");
	req.setObject(the_object);
	req.setDate(the_time);
	event.setSN(the_event);
	req.setEventRetraction(event);
	req.setTag(the_tag);

	req.setValuesSize(the_size);
	req.setAttributesSize(the_size);
	for (int i=0; i<the_size;++i) {
		req.setValues(the_values[i],i);
		req.setAttributes(the_attributes[i],i);
	}

	comm->requestFederateService(&req);
	G.Out(pdGendoc,"exit  ObjectManagement::reflectAttributeValues with time");
}

// ----------------------------------------------------------------------------
//! reflectAttributeValues without time
void
ObjectManagement::reflectAttributeValues(ObjectHandle the_object,
		const std::vector <AttributeHandle> &the_attributes,
		const std::vector <AttributeValue_t> &the_values,
		uint16_t the_size,
		const std::string& the_tag,
		TypeException &)
{
	M_Reflect_Attribute_Values req;

	G.Out(pdGendoc,"enter ObjectManagement::reflectAttributeValues without time");
	req.setObject(the_object);
	req.setTag(the_tag);

	req.setValuesSize(the_size);
	req.setAttributesSize(the_size);
	for (int i=0; i<the_size;++i) {
		req.setValues(the_values[i],i);
		req.setAttributes(the_attributes[i],i);
	}


	comm->requestFederateService(&req);
	G.Out(pdGendoc,"exit  ObjectManagement::reflectAttributeValues without time");
}

// ----------------------------------------------------------------------------
//! sendInteraction with time
EventRetractionHandle
ObjectManagement::sendInteraction(InteractionClassHandle theInteraction,
		const std::vector <ParameterHandle> &paramArray,
		const std::vector <ParameterValue_t> &valueArray,
		uint32_t paramArraySize,
		FederationTime theTime,
		const std::string& theTag,
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
		req.setInteractionClass(theInteraction);
		// true for UAV with time
		req.setDate(theTime);
		req.setRegion(region);
		req.setFederation(fm->_numero_federation);
		req.setFederate(fm->federate);
		req.setParametersSize(paramArraySize);
		req.setValuesSize(paramArraySize);

		for (uint32_t i=0 ; i<paramArraySize ; i++) {
			req.setParameters(paramArray[i],i) ;
			req.setValues(valueArray[i],i);
		}

		req.setLabel(theTag);

		// Send network message and then wait for answer.
		comm->sendMessage(&req);
		std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::SEND_INTERACTION, req.getFederate()));
		e = rep->getException() ;
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
		const std::vector <ParameterHandle> &paramArray,
		const std::vector <ParameterValue_t> &valueArray,
		uint32_t paramArraySize,
		const std::string& theTag,
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
	req.setInteractionClass(theInteraction);
	req.setRegion(region);
	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);
	req.setParametersSize(paramArraySize) ;
	req.setValuesSize(paramArraySize) ;

	for (uint32_t i=0 ; i<paramArraySize ; i++) {
		req.setParameters(paramArray[i],i) ;
		req.setValues(valueArray[i],i);
	}

	req.setLabel(theTag);

	// Send network message and then wait for answer.
	comm->sendMessage(&req);
	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::SEND_INTERACTION, req.getFederate()));

	e = rep->getException() ;

} /* end of sendInteraction */

// ----------------------------------------------------------------------------
//! receiveInteraction with time
void
ObjectManagement::receiveInteraction(InteractionClassHandle the_interaction,
		const std::vector <ParameterHandle> &the_parameters,
		const std::vector <ParameterValue_t> &the_values,
		uint16_t the_size,
		FederationTime the_time,
		const std::string& the_tag,
		EventRetractionHandle the_event,
		TypeException &)
{
	M_Receive_Interaction req;
	EventRetraction  event;

	req.setInteractionClass(the_interaction);
	req.setDate(the_time);
	event.setSN(the_event);
	req.setEventRetraction(event);
	req.setTag(the_tag);
	req.setParametersSize(the_size);
	req.setValuesSize(the_size);
	for (uint32_t i=0;i<the_size;++i) {
		req.setParameters(the_parameters[i],i);
		req.setValues(the_values[i],i);
	}
	comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
//! receiveInteraction without time
void
ObjectManagement::receiveInteraction(InteractionClassHandle the_interaction,
		const std::vector <ParameterHandle> &the_parameters,
		const std::vector <ParameterValue_t> &the_values,
		uint16_t the_size,
		const std::string& the_tag,
		TypeException &)
{
	M_Receive_Interaction req;

	req.setInteractionClass(the_interaction);
	req.setTag(the_tag);
	req.setParametersSize(the_size);
	req.setValuesSize(the_size);
	for (uint32_t i=0;i<the_size;++i) {
		req.setParameters(the_parameters[i],i);
		req.setValues(the_values[i],i);
	}
	comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
//! deleteObject with time
EventRetractionHandle
ObjectManagement::deleteObject(ObjectHandle theObjectHandle,
		FederationTime theTime,
		const std::string& theTag,
		TypeException &e)
{
	NM_Delete_Object req;

	req.setObject(theObjectHandle);
	req.setDate(theTime);
	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);

	req.setLabel(theTag);
	comm->sendMessage(&req);
	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DELETE_OBJECT, req.getFederate()));

	e = rep->getException() ;

	if (e == e_NO_EXCEPTION) {
		rootObject->deleteObjectInstance(fm->federate, theObjectHandle, theTag);
	}

	return rep->eventRetraction ;
} /* end deleteObject */

// ----------------------------------------------------------------------------
//! deleteObject without time
void
ObjectManagement::deleteObject(ObjectHandle theObjectHandle,
		const std::string& theTag,
		TypeException &e)
{
	NM_Delete_Object req;

	req.setObject(theObjectHandle);
	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);

	req.setLabel(theTag);
	comm->sendMessage(&req);
	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DELETE_OBJECT, req.getFederate()));

	e = rep->getException() ;

	if (e == e_NO_EXCEPTION) {
		rootObject->deleteObjectInstance(fm->federate, theObjectHandle, theTag);
	}
} /* end of deleteObject */

// ----------------------------------------------------------------------------

void 
ObjectManagement::deleteAllObjects(TypeException &e)
{
	std::vector<ObjectHandle> ownedObjectInstances;

	rootObject->getAllObjectInstancesFromFederate(fm->federate, ownedObjectInstances);
		
	for(std::vector<ObjectHandle>::iterator it  = ownedObjectInstances.begin();
	    it != ownedObjectInstances.end();
	    ++it)
	{
		this->deleteObject((*it), "", e);
	}
} /* end of deleteAllObjects */

// ----------------------------------------------------------------------------
//! removeObject with time
void
ObjectManagement::removeObject(ObjectHandle the_object,
		FederateHandle the_federate,
		FederationTime theTime,
		const std::string& the_tag,
		EventRetractionHandle the_event,
		TypeException &)

{
	M_Remove_Object_Instance req;
	EventRetraction    event;

	req.setObject(the_object);
	event.setSN(the_event);
	req.setEventRetraction(event);
	req.setTag(the_tag);
	req.setDate(theTime);

	comm->requestFederateService(&req);

	rootObject->deleteObjectInstance(the_federate, the_object, theTime, the_tag);
}

// ----------------------------------------------------------------------------
//! removeObject without time
void
ObjectManagement::removeObject(ObjectHandle the_object,
		FederateHandle the_federate,
		const std::string& the_tag,
		TypeException &)
{
	M_Remove_Object_Instance req;

	req.setObject(the_object);
	req.setTag(the_tag);

	comm->requestFederateService(&req);

	rootObject->deleteObjectInstance(the_federate, the_object, the_tag);
}

// ----------------------------------------------------------------------------
//! changeAttributeTransportType
EventRetractionHandle
ObjectManagement::changeAttributeTransportType(ObjectHandle theObjectHandle,
		const std::vector <AttributeHandle> &attribArray,
		uint32_t attribArraySize,
		TransportType theType,
		TypeException &e)
{
	NM_Change_Attribute_Transport_Type req;
	uint32_t i ;

	req.setObject(theObjectHandle);
	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);
	req.setTransport(theType);
	req.setAttributesSize(attribArraySize);

	for (i = 0 ; i < attribArraySize ; i++)
		req.setAttributes(attribArray[i],i) ;

	comm->sendMessage(&req);

	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(
			NetworkMessage::CHANGE_ATTRIBUTE_TRANSPORT_TYPE,
			req.getFederate()));

	e = rep->getException() ;

	return rep->eventRetraction ;
}

// ----------------------------------------------------------------------------
//! changeAttributeOrderType
EventRetractionHandle
ObjectManagement::changeAttributeOrderType(ObjectHandle theObjectHandle,
		const std::vector <AttributeHandle> &attribArray,
		uint32_t attribArraySize,
		OrderType theType,
		TypeException &e)
{
	NM_Change_Attribute_Order_Type req ;
	uint32_t i ;

	req.setObject(theObjectHandle);
	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);
	req.setOrder(theType);
	req.setAttributesSize(attribArraySize);

	for (i = 0 ; i < attribArraySize ; i++)
		req.setAttributes(attribArray[i],i) ;

	comm->sendMessage(&req);

	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::CHANGE_ATTRIBUTE_ORDER_TYPE, req.getFederate()));

	e = rep->getException() ;

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

	req.setInteractionClass(id);
	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);
	req.setTransport(theType);

	comm->sendMessage(&req);
	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::CHANGE_INTERACTION_TRANSPORT_TYPE, req.getFederate()));
	e = rep->getException() ;

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

	req.setInteractionClass(id);
	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);
	req.setOrder(theType);

	comm->sendMessage(&req);

	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::CHANGE_INTERACTION_ORDER_TYPE, req.getFederate()));

	e = rep->getException() ;

	return rep->eventRetraction ;
} /* end of changeInteractionOrderType */

// ----------------------------------------------------------------------------
//! requestObjectAttributeValueUpdate
void
ObjectManagement::requestObjectAttributeValueUpdate(ObjectHandle handle,
		const std::vector <AttributeHandle> &attribs,
		uint32_t attribArraySize,
		TypeException &e)
{
	NM_Request_Object_Attribute_Value_Update req;

	G.Out(pdGendoc,"enter ObjectManagement::requestObjectAttributeValueUpdate");

	req.setObject(handle);
	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);
	req.setAttributesSize(attribArraySize);

	for (uint32_t i = 0 ; i < attribArraySize ; i++) {
		req.setAttributes(attribs[i],i) ;
	}

	comm->sendMessage(&req);
	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE,
			req.getFederate()));
	e = rep->getException() ;
	G.Out(pdGendoc,"exit  ObjectManagement::requestObjectAttributeValueUpdate");

} /* end of requestObjectAttributeValueUpdate */

// ----------------------------------------------------------------------------
//! requestClassAttributeValueUpdate
void 
ObjectManagement::requestClassAttributeValueUpdate(ObjectClassHandle theClass,
		const std::vector <AttributeHandle> &attribs,
		uint32_t attribArraySize,
		TypeException &e)
{
	NM_Request_Class_Attribute_Value_Update req;
	
	G.Out(pdGendoc,"enter ObjectManagement::requestClassAttributeValueUpdate");
	
	req.setObjectClass(theClass);
	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);
	req.setAttributesSize(attribArraySize);
	
	for (uint32_t i = 0 ; i < attribArraySize ; ++i) {
		req.setAttributes(attribs[i],i) ;
	}

	comm->sendMessage(&req);
	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE,
			req.getFederate()));
	e = rep->getException() ;

	G.Out(pdGendoc,"exit  ObjectManagement::requestClassAttributeValueUpdate");

} /* end of requestClassAttributeValueUpdate */

// --------------------------------------
// -- 4.15 provideAttributeValueUpdate --
// --------------------------------------

void
ObjectManagement::provideAttributeValueUpdate(ObjectHandle the_object,
		const std::vector <AttributeHandle> &the_attributes,
		uint32_t attribArraySize,
		TypeException &)
{
	M_Provide_Attribute_Value_Update req;

	G.Out(pdGendoc,"enter ObjectManagement::provideAttributeValueUpdate");
	req.setObject(the_object);
	req.setAttributesSize(attribArraySize);
	for (uint32_t i = 0 ; i < attribArraySize ; ++i) {
		req.setAttributes(the_attributes[i],i);
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
ObjectManagement::getObjectClassHandle(const std::string& theName)
{
    ObjectClassHandle handle ;
    G.Out(pdGendoc,"enter ObjectManagement::getObjectClassHandle");
    try {
        handle = rootObject->ObjectClasses->getObjectClassHandle(theName);
        G.Out(pdGendoc,"exit  ObjectManagement::getObjectClassHandle");

    }
    catch (NameNotFound&) {
        G.Out(pdGendoc,"exit  ObjectManagement::getObjectClassHandle on NameNotFound");
        throw NameNotFound(theName);
    }
    return handle;
}


// ----------------------------------------------------------------------------
//! getObjectClassName.
const std::string&
ObjectManagement::getObjectClassName(ObjectClassHandle theHandle)
{
	return rootObject->ObjectClasses->getObjectClassName(theHandle);
}

// ----------------------------------------------------------------------------
//! Returns the object instance handle.
ObjectHandle
ObjectManagement::getObjectInstanceHandle(const std::string& the_name)
{
	return rootObject->objects->getObjectInstanceHandle(the_name);
}

// ----------------------------------------------------------------------------
//! Returns the object instance name.
const std::string&
ObjectManagement::getObjectInstanceName(ObjectHandle the_object)
{
	return rootObject->objects->getObjectInstanceName(the_object);
}

// ----------------------------------------------------------------------------
//! getAttributeHandle.
AttributeHandle
ObjectManagement::getAttributeHandle(const std::string& theName,
        ObjectClassHandle theClassHandle)
{
    AttributeHandle handle ;
    G.Out(pdGendoc,"enter ObjectManagement::getAttributeHandle");
    try {
        handle = rootObject->ObjectClasses->getAttributeHandle(theName,
                theClassHandle);
        G.Out(pdGendoc,"exit  ObjectManagement::getAttributeHandle");

    }
    catch (NameNotFound&) {
        G.Out(pdGendoc,"exit  ObjectManagement::getAttributeHandle on NameNotFound");
        throw NameNotFound(theName);
    }
    return handle;
}

// ----------------------------------------------------------------------------
//! getAttributeName.
const std::string&
ObjectManagement::getAttributeName(AttributeHandle theHandle,
		ObjectClassHandle theClassHandle)
{
	return rootObject->ObjectClasses->getAttributeName(theHandle,
			theClassHandle);
}

// ----------------------------------------------------------------------------
//! getInteractionClassHandle.
InteractionClassHandle
ObjectManagement::getInteractionClassHandle(const std::string& theName)
{
	return rootObject->Interactions->getInteractionClassHandle(theName);
}

// ----------------------------------------------------------------------------
//! getInteractionClassName.
const std::string&
ObjectManagement::
getInteractionClassName(InteractionClassHandle theClassHandle)
{
	return
	rootObject->Interactions->getInteractionClassName(theClassHandle);
}

// ----------------------------------------------------------------------------
//! getParameterHandle.
ParameterHandle
ObjectManagement::getParameterHandle(const std::string& theParameterName,
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
ObjectManagement::getTransportationHandle(const std::string& theName)
{
	for(unsigned i = 0; i < sizeof(transportTypeList)/sizeof(transportTypeList[0]); ++i) {
		if(theName == transportTypeList[i].name)
			return transportTypeList[i].type;
	}

	throw NameNotFound(theName);
}

// ----------------------------------------------------------------------------
//! getTransportationName
const std::string&
ObjectManagement::getTransportationName(TransportType theType)
{
	for(unsigned i = 0; i < sizeof(transportTypeList)/sizeof(transportTypeList[0]); ++i) {
		if(theType == transportTypeList[i].type)
			return transportTypeList[i].name;
	}

	throw InvalidTransportationHandle("");
}

// ----------------------------------------------------------------------------
//! getOrderingHandle
OrderType
ObjectManagement::getOrderingHandle(const std::string& theName)
{
	for(unsigned i = 0; i < sizeof(orderTypeList)/sizeof(orderTypeList[0]); ++i) {
		if(theName == orderTypeList[i].name)
			return orderTypeList[i].type;
	}

	throw NameNotFound(theName);
}

// ----------------------------------------------------------------------------
//! getOrderingName
const std::string&
ObjectManagement::getOrderingName(OrderType theType)
{
	for(unsigned i = 0; i < sizeof(orderTypeList)/sizeof(orderTypeList[0]); ++i) {
		if(theType == orderTypeList[i].type)
			return orderTypeList[i].name;
	}

	throw InvalidOrderingHandle("");
}

void
ObjectManagement::
setAttributeScopeAdvisorySwitch(bool state, TypeException &e) {
	G.Out(pdGendoc,"enter ObjectManagement::setAttributeScopeAdvisorySwitch");

	NM_Set_Attribute_Scope_Advisory_Switch msg ;

	e = e_NO_EXCEPTION ;

	msg.setFederation(fm->_numero_federation);
	msg.setFederate(fm->federate);

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
		const uint16_t attribArraySize,
		TypeException &e) {

	M_Attributes_In_Scope req;

	G.Out(pdGendoc,"enter ObjectManagement::attributesInScope");

	req.setObject(theObject);
	req.setAttributesSize(attribArraySize);

	for (int i = 0 ; i < attribArraySize ; i++)
		req.setAttributes(attribArray[i],i);

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
		const uint16_t attribArraySize,
		TypeException &e) {

	M_Attributes_Out_Of_Scope req;

	G.Out(pdGendoc,"enter ObjectManagement::attributesOutScope");

	req.setObject(theObject);
	req.setAttributesSize(attribArraySize);

	for (int i = 0 ; i < attribArraySize ; i++)
		req.setAttributes(attribArray[i],i);

	comm->requestFederateService(&req);

	G.Out(pdGendoc,"exit  ObjectManagement::attributesOutScope");
}

void
ObjectManagement::
setAttributeRelevanceAdvisorySwitch(bool state, TypeException &e) {
	G.Out(pdGendoc,"enter ObjectManagement::setAttributeRelevanceAdvisorySwitch");

	NM_Set_Attribute_Relevance_Advisory_Switch msg ;

	e = e_NO_EXCEPTION ;

	msg.setFederation(fm->_numero_federation);
	msg.setFederate(fm->federate);

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
		const uint16_t attribArraySize,
		TypeException &e) {

	M_Turn_Updates_On_For_Object_Instance req;

	G.Out(pdGendoc,"enter ObjectManagement::turnUpdatesOnForObjectInstance");

	req.setObject(theObject);
	req.setAttributesSize(attribArraySize);

	for (int i = 0 ; i < attribArraySize ; i++)
		req.setAttributes(attribArray[i],i);

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
		const uint16_t attribArraySize,
		TypeException &e) {

	M_Turn_Updates_Off_For_Object_Instance req;

	G.Out(pdGendoc,"enter ObjectManagement::turnUpdatesOffForObjectInstance");

	req.setObject(theObject);
	req.setAttributesSize(attribArraySize);

	for (int i = 0 ; i < attribArraySize ; i++)
		req.setAttributes(attribArray[i],i);

	comm->requestFederateService(&req);

	G.Out(pdGendoc,"exit  ObjectManagement::turnUpdatesOffForObjectInstance");
}

// 1516 - 6.3
void 
ObjectManagement::nameReservationSucceeded(const std::string &reservedName)
{
	M_Reserve_Object_Instance_Name_Succeeded req;

	req.setObjectName(reservedName);

	comm->requestFederateService(&req);
}

void 
ObjectManagement::nameReservationFailed(const std::string &reservedName)
{
	M_Reserve_Object_Instance_Name_Failed req;

	req.setObjectName(reservedName);

	comm->requestFederateService(&req);
}


}} // namespace certi/rtia

