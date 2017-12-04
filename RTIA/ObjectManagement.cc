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

#include "ObjectManagement.hh"

#include <cassert>
#include <config.h>
#include <iostream>
#include <memory>

#include <libCERTI/InteractionSet.hh>
#include <libCERTI/M_Classes.hh>
#include <libCERTI/NM_Classes.hh>
#include <libCERTI/ObjectClassSet.hh>
#include <libCERTI/ObjectSet.hh>
#include <libCERTI/PrettyDebug.hh>

#include "FederationManagement.hh"
#include "TimeManagement.hh"

using std::cout;
using std::endl;

namespace certi {
namespace rtia {

static PrettyDebug D("RTIA_OM", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);

const ObjectManagement::TransportTypeList ObjectManagement::transportTypeList[]
    = {{"HLAreliable", RELIABLE}, {"HLAbestEffort", BEST_EFFORT}};

const ObjectManagement::OrderTypeList ObjectManagement::orderTypeList[]
    = {{"Receive", RECEIVE}, {"Timestamp", TIMESTAMP}};

ObjectManagement::ObjectManagement(Communications* GC, FederationManagement* GF, RootObject* theRootObj)
    : comm(GC), fm(GF), rootObject(theRootObj)
{
}

ObjectManagement::~ObjectManagement()
{
}

void ObjectManagement::reserveObjectName(const std::string& newObjName, Exception::Type& e)
{
    NM_Reserve_Object_Instance_Name req;

    //  Empty strings not allowed
    if (newObjName.empty() ||
        // According to spec, the HLA prefix is reserved for RTI-internal objects.
        newObjName.compare(0, 3, "HLA") == 0) {
        e = Exception::Type::IllegalName;
    }
    else {
        req.setFederation(fm->_numero_federation);
        req.setFederate(fm->federate);

        req.setObjectName(newObjName);

        comm->sendMessage(&req);
    }
    // JvY TODO: Finish handling on other side (and return path)
}

ObjectHandle ObjectManagement::registerObject(
    ObjectClassHandle the_class, const std::string& theObjectName, FederationTime, FederationTime, Exception::Type& e)
{
    NM_Register_Object req;

    req.setFederate(fm->federate);
    req.setFederation(fm->_numero_federation);
    req.setObjectClass(the_class);
    req.setLabel(theObjectName);

    comm->sendMessage(&req);

    std::unique_ptr<NM_Register_Object> rep(
        static_cast<NM_Register_Object*>(comm->waitMessage(NetworkMessage::Type::REGISTER_OBJECT, req.getFederate())));

    e = rep->getException();

    if (e == Exception::Type::NO_EXCEPTION) {
        auto responses = rootObject->registerObjectInstance(fm->federate, the_class, rep->getObject(), rep->getLabel());
        std::cout << "==========================" << std::endl;
        std::cout << "RESPONSES FROM ROOT OBJECT" << std::endl;
        for (auto& rep : responses) {
            std::cout << (int) rep.message()->getMessageType() << std::endl;
        }
        std::cout << "==========================" << std::endl;
        return rep->getObject();
    }
    else {
        return 0;
    }
}

EventRetractionHandle ObjectManagement::updateAttributeValues(ObjectHandle theObjectHandle,
                                                              const std::vector<AttributeHandle>& attribArray,
                                                              const std::vector<AttributeValue_t>& valueArray,
                                                              uint32_t attribArraySize,
                                                              FederationTime theTime,
                                                              const std::string& theTag,
                                                              Exception::Type& e)
{
    EventRetractionHandle evtrHandle;

    G.Out(pdGendoc, "enter ObjectManagement::updateAttributeValues with time");
    if (tm->testValidTime(theTime)) {
        // Building request (req NetworkMessage)
        NM_Update_Attribute_Values req;
        req.setFederation(fm->_numero_federation);
        req.setFederate(fm->federate);
        req.setObject(theObjectHandle);
        // set Date for UAV with time
        req.setDate(theTime);
        req.setAttributesSize(attribArraySize);
        req.setValuesSize(attribArraySize);
        for (uint32_t i = 0; i < attribArraySize; ++i) {
            req.setAttributes(attribArray[i], i);
            req.setValues(valueArray[i], i);
        }

        req.setLabel(theTag);

        comm->sendMessage(&req);
        std::unique_ptr<NM_Update_Attribute_Values> rep(
            static_cast<NM_Update_Attribute_Values*>(comm->waitMessage(req.getMessageType(), req.getFederate())));
        e = rep->getException();
        evtrHandle = rep->getEvent();
    }
    else {
        std::stringstream errorMsg;
        errorMsg << "UAV InvalidFederationTime: " << std::endl;
        errorMsg << " providedTime =" << theTime << std::endl;
        errorMsg << " currentTime  =" << tm->requestFederateTime() << std::endl;
        errorMsg << " lookahead    =" << tm->requestLookahead() << std::endl;

        D.Out(pdDebug, errorMsg.str().c_str());
        e = Exception::Type::InvalidFederationTime;
        evtrHandle = 0;
    }

    G.Out(pdGendoc, "exit ObjectManagement::updateAttributeValues with time");
    return evtrHandle;
}

void ObjectManagement::updateAttributeValues(ObjectHandle theObjectHandle,
                                             const std::vector<AttributeHandle>& attribArray,
                                             const std::vector<AttributeValue_t>& valueArray,
                                             uint32_t attribArraySize,
                                             const std::string& theTag,
                                             Exception::Type& e)
{
    NM_Update_Attribute_Values req;

    G.Out(pdGendoc, "enter ObjectManagement::updateAttributeValues without time");
    // Building request (req NetworkMessage)
    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setObject(theObjectHandle);
    // Do no set Date if without time
    req.setAttributesSize(attribArraySize);
    req.setValuesSize(attribArraySize);

    for (uint32_t i = 0; i < attribArraySize; ++i) {
        req.setAttributes(attribArray[i], i);
        req.setValues(valueArray[i], i);
    }

    req.setLabel(theTag);

    comm->sendMessage(&req);
    std::unique_ptr<NetworkMessage> rep(comm->waitMessage(req.getMessageType(), req.getFederate()));

    e = rep->getException();
    G.Out(pdGendoc, "exit  ObjectManagement::updateAttributeValues without time");
}

void ObjectManagement::discoverObject(ObjectHandle the_object,
                                      ObjectClassHandle the_class,
                                      const std::string& the_name,
                                      FederationTime the_time,
                                      EventRetractionHandle the_event,
                                      Exception::Type& /*e*/)
{
    M_Discover_Object_Instance req;
    EventRetraction event;

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

void ObjectManagement::reflectAttributeValues(ObjectHandle the_object,
                                              const std::vector<AttributeHandle>& the_attributes,
                                              const std::vector<AttributeValue_t>& the_values,
                                              uint16_t the_size,
                                              FederationTime the_time,
                                              const std::string& the_tag,
                                              EventRetractionHandle the_event,
                                              Exception::Type& /*e*/)
{
    M_Reflect_Attribute_Values req;
    EventRetraction event;

    G.Out(pdGendoc, "enter ObjectManagement::reflectAttributeValues with time");
    req.setObject(the_object);
    req.setDate(the_time);
    event.setSN(the_event);
    req.setEventRetraction(event);
    req.setTag(the_tag);

    req.setValuesSize(the_size);
    req.setAttributesSize(the_size);
    for (int i = 0; i < the_size; ++i) {
        req.setValues(the_values[i], i);
        req.setAttributes(the_attributes[i], i);
    }

    comm->requestFederateService(&req);
    G.Out(pdGendoc, "exit  ObjectManagement::reflectAttributeValues with time");
}

void ObjectManagement::reflectAttributeValues(ObjectHandle the_object,
                                              const std::vector<AttributeHandle>& the_attributes,
                                              const std::vector<AttributeValue_t>& the_values,
                                              uint16_t the_size,
                                              const std::string& the_tag,
                                              Exception::Type& /*e*/)
{
    M_Reflect_Attribute_Values req;

    G.Out(pdGendoc, "enter ObjectManagement::reflectAttributeValues without time");
    req.setObject(the_object);
    req.setTag(the_tag);

    req.setValuesSize(the_size);
    req.setAttributesSize(the_size);
    for (int i = 0; i < the_size; ++i) {
        req.setValues(the_values[i], i);
        req.setAttributes(the_attributes[i], i);
    }

    comm->requestFederateService(&req);
    G.Out(pdGendoc, "exit  ObjectManagement::reflectAttributeValues without time");
}

EventRetractionHandle ObjectManagement::sendInteraction(InteractionClassHandle theInteraction,
                                                        const std::vector<ParameterHandle>& paramArray,
                                                        const std::vector<ParameterValue_t>& valueArray,
                                                        uint32_t paramArraySize,
                                                        FederationTime theTime,
                                                        const std::string& theTag,
                                                        RegionHandle region,
                                                        Exception::Type& e)
{
    EventRetractionHandle evtrHandle;

    if (tm->testValidTime(theTime)) {
        G.Out(pdGendoc, "ObjectManagement::sendInteraction with time");
        // Local test to know if interaction is correct.
        rootObject->Interactions->isReady(fm->federate, theInteraction, paramArray, paramArraySize);

        // Building network message (req) to RTIG.
        NM_Send_Interaction req;
        req.setInteractionClass(theInteraction);
        // true for UAV with time
        req.setDate(theTime);
        req.setRegion(region);
        req.setFederation(fm->_numero_federation);
        req.setFederate(fm->federate);
        req.setParametersSize(paramArraySize);
        req.setValuesSize(paramArraySize);

        for (uint32_t i = 0; i < paramArraySize; i++) {
            req.setParameters(paramArray[i], i);
            req.setValues(valueArray[i], i);
        }

        req.setLabel(theTag);

        // Send network message and then wait for answer.
        comm->sendMessage(&req);
        std::unique_ptr<NetworkMessage> rep(
            comm->waitMessage(NetworkMessage::Type::SEND_INTERACTION, req.getFederate()));
        e = rep->getException();
        evtrHandle = rep->eventRetraction;
    }
    else {
        e = Exception::Type::InvalidFederationTime;
        evtrHandle = 0;
    }

    return evtrHandle;
}

void ObjectManagement::sendInteraction(InteractionClassHandle theInteraction,
                                       const std::vector<ParameterHandle>& paramArray,
                                       const std::vector<ParameterValue_t>& valueArray,
                                       uint32_t paramArraySize,
                                       const std::string& theTag,
                                       RegionHandle region,
                                       Exception::Type& e)
{
    NM_Send_Interaction req;
    G.Out(pdGendoc, "ObjectManagement::sendInteraction without time");
    // Local test to know if interaction is correct.
    rootObject->Interactions->isReady(fm->federate, theInteraction, paramArray, paramArraySize);

    // Building network message (req) to RTIG.
    req.setInteractionClass(theInteraction);
    req.setRegion(region);
    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setParametersSize(paramArraySize);
    req.setValuesSize(paramArraySize);

    for (uint32_t i = 0; i < paramArraySize; i++) {
        req.setParameters(paramArray[i], i);
        req.setValues(valueArray[i], i);
    }

    req.setLabel(theTag);

    // Send network message and then wait for answer.
    comm->sendMessage(&req);
    std::unique_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::Type::SEND_INTERACTION, req.getFederate()));

    e = rep->getException();

}

void ObjectManagement::receiveInteraction(InteractionClassHandle the_interaction,
                                          const std::vector<ParameterHandle>& the_parameters,
                                          const std::vector<ParameterValue_t>& the_values,
                                          uint16_t the_size,
                                          FederationTime the_time,
                                          const std::string& the_tag,
                                          EventRetractionHandle the_event,
                                          Exception::Type& /*e*/)
{
    M_Receive_Interaction req;
    EventRetraction event;

    req.setInteractionClass(the_interaction);
    req.setDate(the_time);
    event.setSN(the_event);
    req.setEventRetraction(event);
    req.setTag(the_tag);
    req.setParametersSize(the_size);
    req.setValuesSize(the_size);
    for (uint32_t i = 0; i < the_size; ++i) {
        req.setParameters(the_parameters[i], i);
        req.setValues(the_values[i], i);
    }
    comm->requestFederateService(&req);
}

void ObjectManagement::receiveInteraction(InteractionClassHandle the_interaction,
                                          const std::vector<ParameterHandle>& the_parameters,
                                          const std::vector<ParameterValue_t>& the_values,
                                          uint16_t the_size,
                                          const std::string& the_tag,
                                          Exception::Type& /*e*/)
{
    M_Receive_Interaction req;

    req.setInteractionClass(the_interaction);
    req.setTag(the_tag);
    req.setParametersSize(the_size);
    req.setValuesSize(the_size);
    for (uint32_t i = 0; i < the_size; ++i) {
        req.setParameters(the_parameters[i], i);
        req.setValues(the_values[i], i);
    }
    comm->requestFederateService(&req);
}

EventRetractionHandle ObjectManagement::deleteObject(ObjectHandle theObjectHandle,
                                                     FederationTime theTime,
                                                     const std::string& theTag,
                                                     Exception::Type& e)
{
    NM_Delete_Object req;

    req.setObject(theObjectHandle);
    req.setDate(theTime);
    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);

    req.setLabel(theTag);
    comm->sendMessage(&req);
    std::unique_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::Type::DELETE_OBJECT, req.getFederate()));

    e = rep->getException();

    if (e == Exception::Type::NO_EXCEPTION) {
        auto responses = rootObject->deleteObjectInstance(fm->federate, theObjectHandle, theTag);
        std::cout << "==========================" << std::endl;
        std::cout << "RESPONSES FROM ROOT OBJECT" << std::endl;
        for (auto& rep : responses) {
            std::cout << (int) rep.message()->getMessageType() << std::endl;
        }
        std::cout << "==========================" << std::endl;
    }

    return rep->eventRetraction;
}

void ObjectManagement::deleteObject(ObjectHandle theObjectHandle, const std::string& theTag, Exception::Type& e)
{
    NM_Delete_Object req;

    req.setObject(theObjectHandle);
    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);

    req.setLabel(theTag);
    comm->sendMessage(&req);
    std::unique_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::Type::DELETE_OBJECT, req.getFederate()));

    e = rep->getException();

    if (e == Exception::Type::NO_EXCEPTION) {
        auto responses = rootObject->deleteObjectInstance(fm->federate, theObjectHandle, theTag);
        std::cout << "==========================" << std::endl;
        std::cout << "RESPONSES FROM ROOT OBJECT" << std::endl;
        for (auto& rep : responses) {
            std::cout << (int) rep.message()->getMessageType() << std::endl;
        }
        std::cout << "==========================" << std::endl;
    }
}

void ObjectManagement::deleteAllObjects(Exception::Type& e)
{
    std::vector<ObjectHandle> ownedObjectInstances;

    rootObject->getAllObjectInstancesFromFederate(fm->federate, ownedObjectInstances);

    for (std::vector<ObjectHandle>::iterator it = ownedObjectInstances.begin(); it != ownedObjectInstances.end();
         ++it) {
        this->deleteObject((*it), "", e);
    }
}

void ObjectManagement::removeObject(ObjectHandle the_object,
                                    FederateHandle the_federate,
                                    FederationTime theTime,
                                    const std::string& the_tag,
                                    EventRetractionHandle the_event,
                                    Exception::Type& /*e*/)

{
    M_Remove_Object_Instance req;
    EventRetraction event;

    req.setObject(the_object);
    event.setSN(the_event);
    req.setEventRetraction(event);
    req.setTag(the_tag);
    req.setDate(theTime);

    comm->requestFederateService(&req);

    auto responses = rootObject->deleteObjectInstance(the_federate, the_object, theTime, the_tag);
    std::cout << "==========================" << std::endl;
    std::cout << "RESPONSES FROM ROOT OBJECT" << std::endl;
    for (auto& rep : responses) {
        std::cout << (int) rep.message()->getMessageType() << std::endl;
    }
    std::cout << "==========================" << std::endl;
}

void ObjectManagement::removeObject(ObjectHandle the_object,
                                    FederateHandle the_federate,
                                    const std::string& the_tag,
                                    Exception::Type& /*e*/)
{
    M_Remove_Object_Instance req;

    req.setObject(the_object);
    req.setTag(the_tag);

    comm->requestFederateService(&req);

    auto responses = rootObject->deleteObjectInstance(the_federate, the_object, the_tag);
    std::cout << "==========================" << std::endl;
    std::cout << "RESPONSES FROM ROOT OBJECT" << std::endl;
    for (auto& rep : responses) {
        std::cout << (int) rep.message()->getMessageType() << std::endl;
    }
    std::cout << "==========================" << std::endl;
}

EventRetractionHandle ObjectManagement::changeAttributeTransportType(ObjectHandle theObjectHandle,
                                                                     const std::vector<AttributeHandle>& attribArray,
                                                                     uint32_t attribArraySize,
                                                                     TransportType theType,
                                                                     Exception::Type& e)
{
    NM_Change_Attribute_Transport_Type req;
    uint32_t i;

    req.setObject(theObjectHandle);
    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setTransport(theType);
    req.setAttributesSize(attribArraySize);

    for (i = 0; i < attribArraySize; i++)
        req.setAttributes(attribArray[i], i);

    comm->sendMessage(&req);

    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::CHANGE_ATTRIBUTE_TRANSPORT_TYPE, req.getFederate()));

    e = rep->getException();

    return rep->eventRetraction;
}

EventRetractionHandle ObjectManagement::changeAttributeOrderType(ObjectHandle theObjectHandle,
                                                                 const std::vector<AttributeHandle>& attribArray,
                                                                 uint32_t attribArraySize,
                                                                 OrderType theType,
                                                                 Exception::Type& e)
{
    NM_Change_Attribute_Order_Type req;
    uint32_t i;

    req.setObject(theObjectHandle);
    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setOrder(theType);
    req.setAttributesSize(attribArraySize);

    for (i = 0; i < attribArraySize; i++)
        req.setAttributes(attribArray[i], i);

    comm->sendMessage(&req);

    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::CHANGE_ATTRIBUTE_ORDER_TYPE, req.getFederate()));

    e = rep->getException();

    return rep->eventRetraction;
}

EventRetractionHandle
ObjectManagement::changeInteractionTransportType(InteractionClassHandle id, TransportType theType, Exception::Type& e)
{
    NM_Change_Interaction_Transport_Type req;

    req.setInteractionClass(id);
    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setTransport(theType);

    comm->sendMessage(&req);
    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::CHANGE_INTERACTION_TRANSPORT_TYPE, req.getFederate()));
    e = rep->getException();

    return rep->eventRetraction;
}

EventRetractionHandle
ObjectManagement::changeInteractionOrderType(InteractionClassHandle id, OrderType theType, Exception::Type& e)
{
    NM_Change_Interaction_Order_Type req;

    req.setInteractionClass(id);
    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setOrder(theType);

    comm->sendMessage(&req);

    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::CHANGE_INTERACTION_ORDER_TYPE, req.getFederate()));

    e = rep->getException();

    return rep->eventRetraction;
}

void ObjectManagement::requestObjectAttributeValueUpdate(ObjectHandle handle,
                                                         const std::vector<AttributeHandle>& attribs,
                                                         uint32_t attribArraySize,
                                                         Exception::Type& e)
{
    NM_Request_Object_Attribute_Value_Update req;

    G.Out(pdGendoc, "enter ObjectManagement::requestObjectAttributeValueUpdate");

    req.setObject(handle);
    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setAttributesSize(attribArraySize);

    for (uint32_t i = 0; i < attribArraySize; i++) {
        req.setAttributes(attribs[i], i);
    }

    comm->sendMessage(&req);
    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE, req.getFederate()));
    e = rep->getException();
    G.Out(pdGendoc, "exit  ObjectManagement::requestObjectAttributeValueUpdate");

}

void ObjectManagement::requestClassAttributeValueUpdate(ObjectClassHandle theClass,
                                                        const std::vector<AttributeHandle>& attribs,
                                                        uint32_t attribArraySize,
                                                        Exception::Type& e)
{
    NM_Request_Class_Attribute_Value_Update req;

    G.Out(pdGendoc, "enter ObjectManagement::requestClassAttributeValueUpdate");

    req.setObjectClass(theClass);
    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setAttributesSize(attribArraySize);

    for (uint32_t i = 0; i < attribArraySize; ++i) {
        req.setAttributes(attribs[i], i);
    }

    comm->sendMessage(&req);
    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE, req.getFederate()));
    e = rep->getException();

    G.Out(pdGendoc, "exit  ObjectManagement::requestClassAttributeValueUpdate");

}

void ObjectManagement::provideAttributeValueUpdate(ObjectHandle the_object,
                                                   const std::vector<AttributeHandle>& the_attributes,
                                                   uint32_t attribArraySize,
                                                   Exception::Type&)
{
    M_Provide_Attribute_Value_Update req;

    G.Out(pdGendoc, "enter ObjectManagement::provideAttributeValueUpdate");
    req.setObject(the_object);
    req.setAttributesSize(attribArraySize);
    for (uint32_t i = 0; i < attribArraySize; ++i) {
        req.setAttributes(the_attributes[i], i);
    }

    comm->requestFederateService(&req);
    G.Out(pdGendoc, "exit  ObjectManagement::provideAttributeValueUpdate");
}

void ObjectManagement::retract(EventRetractionHandle /*theHandle*/, Exception::Type& /*e*/)
{
    throw RTIinternalError("ObjectManagement::retract not implemented.");
}

void ObjectManagement::reflectRetraction(EventRetractionHandle, Exception::Type&)
{
    throw RTIinternalError("ObjectManagement::reflectRetraction not implemented.");
}

ObjectClassHandle ObjectManagement::getObjectClassHandle(const std::string& theName)
{
    ObjectClassHandle handle;
    G.Out(pdGendoc, "enter ObjectManagement::getObjectClassHandle");
    try {
        handle = rootObject->ObjectClasses->getObjectClassHandle(theName);
        G.Out(pdGendoc, "exit  ObjectManagement::getObjectClassHandle");
    }
    catch (NameNotFound&) {
        G.Out(pdGendoc, "exit  ObjectManagement::getObjectClassHandle on NameNotFound");
        throw NameNotFound(theName);
    }
    return handle;
}

const std::string& ObjectManagement::getObjectClassName(ObjectClassHandle theHandle)
{
    return rootObject->ObjectClasses->getObjectClassName(theHandle);
}

ObjectHandle ObjectManagement::getObjectInstanceHandle(const std::string& the_name)
{
    return rootObject->objects->getObjectInstanceHandle(the_name);
}

const std::string& ObjectManagement::getObjectInstanceName(ObjectHandle the_object)
{
    return rootObject->objects->getObjectInstanceName(the_object);
}

AttributeHandle ObjectManagement::getAttributeHandle(const std::string& theName, ObjectClassHandle theClassHandle)
{
    AttributeHandle handle;
    G.Out(pdGendoc, "enter ObjectManagement::getAttributeHandle");
    try {
        handle = rootObject->ObjectClasses->getAttributeHandle(theName, theClassHandle);
        G.Out(pdGendoc, "exit  ObjectManagement::getAttributeHandle");
    }
    catch (NameNotFound&) {
        G.Out(pdGendoc, "exit  ObjectManagement::getAttributeHandle on NameNotFound");
        throw NameNotFound(theName);
    }
    return handle;
}

const std::string& ObjectManagement::getAttributeName(AttributeHandle theHandle, ObjectClassHandle theClassHandle)
{
    return rootObject->ObjectClasses->getAttributeName(theHandle, theClassHandle);
}

InteractionClassHandle ObjectManagement::getInteractionClassHandle(const std::string& theName)
{
    return rootObject->Interactions->getInteractionClassHandle(theName);
}

const std::string& ObjectManagement::getInteractionClassName(InteractionClassHandle theClassHandle)
{
    return rootObject->Interactions->getInteractionClassName(theClassHandle);
}

ParameterHandle ObjectManagement::getParameterHandle(const std::string& theParameterName,
                                                     InteractionClassHandle theClassHandle)
{
    return rootObject->Interactions->getParameterHandle(theParameterName, theClassHandle);
}

const std::string& ObjectManagement::getParameterName(ParameterHandle theParameterHandle,
                                                      InteractionClassHandle theClassHandle)
{
    return rootObject->Interactions->getParameterName(theParameterHandle, theClassHandle);
}

ObjectClassHandle ObjectManagement::getObjectClass(ObjectHandle object)
{
    return rootObject->objects->getObjectClass(object);
}

TransportType ObjectManagement::getTransportationHandle(const std::string& theName)
{
    for (unsigned i = 0; i < sizeof(transportTypeList) / sizeof(transportTypeList[0]); ++i) {
        if (theName == transportTypeList[i].name)
            return transportTypeList[i].type;
    }

    throw NameNotFound(theName);
}

const std::string& ObjectManagement::getTransportationName(TransportType theType)
{
    for (unsigned i = 0; i < sizeof(transportTypeList) / sizeof(transportTypeList[0]); ++i) {
        if (theType == transportTypeList[i].type)
            return transportTypeList[i].name;
    }

    throw InvalidTransportationHandle("");
}

OrderType ObjectManagement::getOrderingHandle(const std::string& theName)
{
    for (unsigned i = 0; i < sizeof(orderTypeList) / sizeof(orderTypeList[0]); ++i) {
        if (theName == orderTypeList[i].name)
            return orderTypeList[i].type;
    }

    throw NameNotFound(theName);
}

const std::string& ObjectManagement::getOrderingName(OrderType theType)
{
    for (unsigned i = 0; i < sizeof(orderTypeList) / sizeof(orderTypeList[0]); ++i) {
        if (theType == orderTypeList[i].type)
            return orderTypeList[i].name;
    }

    throw InvalidOrderingHandle("");
}

void ObjectManagement::setAttributeScopeAdvisorySwitch(bool state, Exception::Type& e)
{
    G.Out(pdGendoc, "enter ObjectManagement::setAttributeScopeAdvisorySwitch");

    NM_Set_Attribute_Scope_Advisory_Switch msg;

    e = Exception::Type::NO_EXCEPTION;

    msg.setFederation(fm->_numero_federation);
    msg.setFederate(fm->federate);

    if (state) {
        msg.attributeScopeAdvisorySwitchOn();
    }
    else {
        msg.attributeScopeAdvisorySwitchOff();
    }

    comm->sendMessage(&msg);

    G.Out(pdGendoc, "exit ObjectManagement::setAttributeScopeAdvisorySwitch");
}

void ObjectManagement::attributesInScope(ObjectHandle theObject,
                                         const std::vector<AttributeHandle>& attribArray,
                                         const uint16_t attribArraySize,
                                         Exception::Type& /*e*/)
{
    M_Attributes_In_Scope req;

    G.Out(pdGendoc, "enter ObjectManagement::attributesInScope");

    req.setObject(theObject);
    req.setAttributesSize(attribArraySize);

    for (int i = 0; i < attribArraySize; i++)
        req.setAttributes(attribArray[i], i);

    comm->requestFederateService(&req);

    G.Out(pdGendoc, "exit  ObjectManagement::attributesInScope");
}

void ObjectManagement::attributesOutOfScope(ObjectHandle theObject,
                                            const std::vector<AttributeHandle>& attribArray,
                                            const uint16_t attribArraySize,
                                            Exception::Type& /*e*/)
{
    M_Attributes_Out_Of_Scope req;

    G.Out(pdGendoc, "enter ObjectManagement::attributesOutScope");

    req.setObject(theObject);
    req.setAttributesSize(attribArraySize);

    for (int i = 0; i < attribArraySize; i++)
        req.setAttributes(attribArray[i], i);

    comm->requestFederateService(&req);

    G.Out(pdGendoc, "exit  ObjectManagement::attributesOutScope");
}

void ObjectManagement::setAttributeRelevanceAdvisorySwitch(bool state, Exception::Type& e)
{
    G.Out(pdGendoc, "enter ObjectManagement::setAttributeRelevanceAdvisorySwitch");

    NM_Set_Attribute_Relevance_Advisory_Switch msg;

    e = Exception::Type::NO_EXCEPTION;

    msg.setFederation(fm->_numero_federation);
    msg.setFederate(fm->federate);

    if (state) {
        msg.attributeRelevanceAdvisorySwitchOn();
    }
    else {
        msg.attributeRelevanceAdvisorySwitchOff();
    }

    comm->sendMessage(&msg);

    G.Out(pdGendoc, "exit ObjectManagement::setAttributeRelevanceAdvisorySwitch");
}

void ObjectManagement::turnUpdatesOnForObjectInstance(ObjectHandle theObject,
                                                      const std::vector<AttributeHandle>& attribArray,
                                                      const uint16_t attribArraySize,
                                                      Exception::Type& /*e*/)
{
    M_Turn_Updates_On_For_Object_Instance req;

    G.Out(pdGendoc, "enter ObjectManagement::turnUpdatesOnForObjectInstance");

    req.setObject(theObject);
    req.setAttributesSize(attribArraySize);

    for (int i = 0; i < attribArraySize; i++)
        req.setAttributes(attribArray[i], i);

    comm->requestFederateService(&req);

    G.Out(pdGendoc, "exit  ObjectManagement::turnUpdatesOnForObjectInstance");
}

void ObjectManagement::turnUpdatesOffForObjectInstance(ObjectHandle theObject,
                                                       const std::vector<AttributeHandle>& attribArray,
                                                       const uint16_t attribArraySize,
                                                       Exception::Type& /*e*/)
{
    M_Turn_Updates_Off_For_Object_Instance req;

    G.Out(pdGendoc, "enter ObjectManagement::turnUpdatesOffForObjectInstance");

    req.setObject(theObject);
    req.setAttributesSize(attribArraySize);

    for (int i = 0; i < attribArraySize; i++)
        req.setAttributes(attribArray[i], i);

    comm->requestFederateService(&req);

    G.Out(pdGendoc, "exit  ObjectManagement::turnUpdatesOffForObjectInstance");
}

void ObjectManagement::nameReservationSucceeded(const std::string& reservedName)
{
    M_Reserve_Object_Instance_Name_Succeeded req;

    req.setObjectName(reservedName);

    comm->requestFederateService(&req);
}

void ObjectManagement::nameReservationFailed(const std::string& reservedName)
{
    M_Reserve_Object_Instance_Name_Failed req;

    req.setObjectName(reservedName);

    comm->requestFederateService(&req);
}
}
} // namespace certi/rtia
