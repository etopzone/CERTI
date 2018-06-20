// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// ----------------------------------------------------------------------------

// Project
#include "NM_Classes.hh"
#include "Object.hh"
#include "ObjectAttribute.hh"
#include "ObjectSet.hh"
#include "PrettyDebug.hh"

// Standard
#include <iostream>

namespace certi {

static PrettyDebug D("OBJECTSET", "(ObjectSet) - ");
static PrettyDebug G("GENDOC", __FILE__);

ObjectSet::ObjectSet(SecurityServer* the_server) : server(the_server)
{
}

ObjectSet::~ObjectSet()
{
    for (auto i = begin(my_objects_per_handle); i != end(my_objects_per_handle); ++i) {
        delete i->second;
    }
}

void ObjectSet::display() const
{
    std::cout << "Object set: " << my_objects_per_handle.size() << std::endl;
    for (const auto& pair : my_objects_per_handle) {
        std::cout << "****" << std::endl;
        std::cout << "Object #" << pair.first << std::endl;
        pair.second->display();
        std::cout << "****" << std::endl;
    }
}

ObjectHandle ObjectSet::getObjectInstanceHandle(const std::string& the_name) const
{
    auto it = my_objects_per_name.find(the_name);

    if (it != my_objects_per_name.end()) {
        return it->second->getHandle();
    }

    throw ObjectNotKnown("No object instance with name <" + the_name + ">");
}

const std::string& ObjectSet::getObjectInstanceName(ObjectHandle the_object) const
{
    return getObject(the_object)->getName();
}

ObjectClassHandle ObjectSet::getObjectClass(ObjectHandle the_object) const
{
    return getObject(the_object)->getClass();
}

void ObjectSet::changeAttributeTransportationType(ObjectHandle, AttributeHandle*, uint16_t, TransportType)
{
    // Object *object = getObject(the_object);
}

void ObjectSet::changeAttributeOrderType(ObjectHandle, AttributeHandle*, uint16_t, TransportType)
{
    // Object *object = getObject(the_object);
}

Object* ObjectSet::registerObjectInstance(FederateHandle the_federate,
                                          ObjectClassHandle the_class,
                                          ObjectHandle the_object,
                                          const std::string& the_name)
{
    if (my_objects_per_handle.find(the_object) != end(my_objects_per_handle)) {
        throw ObjectAlreadyRegistered("Object already in ObjectSet map.");
    }

    if (my_objects_per_name.find(the_name) != end(my_objects_per_name)) {
        throw ObjectAlreadyRegistered("Object name already defined.");
    }

    auto object = new Object(the_federate);
    object->setHandle(the_object);
    object->setClass(the_class);

    if (the_name.size() > 0) {
        object->setName(the_name);
    }
    else {
        object->setName("HLAobject_" + std::to_string(the_object));
    }

    my_objects_per_handle[the_object] = object;
    my_objects_per_name[the_name] = object;

    return object;
}

void ObjectSet::deleteObjectInstance(FederateHandle /*the_federate*/,
                                     ObjectHandle the_object,
                                     const std::string& /*the_tag*/)
{
    auto object = getObject(the_object);
    my_objects_per_handle.erase(object->getHandle());
    my_objects_per_name.erase(object->getName());

    delete object; // Remove the Object instance.
}

FederateHandle ObjectSet::requestObjectOwner(FederateHandle /*the_federate*/, ObjectHandle the_object) const
{
    Debug(G, pdGendoc) << "enter ObjectSet::requestObjectOwner" << std::endl;
    auto it = my_objects_per_handle.find(the_object);

    if (it == end(my_objects_per_handle)) {
        throw ObjectNotKnown("Object <" + std::to_string(the_object) + "> not found in ObjectSet map.");
    }

    // Object found, return the owner
    Debug(G, pdGendoc) << "exit  ObjectSet::requestObjectOwner" << std::endl;
    return it->second->getOwner();
}

void ObjectSet::killFederate(FederateHandle the_federate)
{
    auto i = begin(my_objects_per_handle);
    while (i != end(my_objects_per_handle)) {
        if (i->second->getOwner() == the_federate) {
            deleteObjectInstance(the_federate, i->first, "");
            i = begin(my_objects_per_handle);
        }
        else {
            // It is safe to run this multiple times
            i->second->killFederate(the_federate);
            ++i;
        }
    }
}

bool ObjectSet::isAttributeOwnedByFederate(FederateHandle the_federate,
                                           ObjectHandle the_object,
                                           AttributeHandle the_attribute) const
{
    if (!server) {
        throw RTIinternalError("isAttributeOwnedByFederate not called by RTIG");
    }

    Debug(D, pdDebug) << "isAttributeOwnedByFederate called for attribute " << the_attribute << ", objet " << the_object
                      << std::endl;

    return getObject(the_object)->isAttributeOwnedByFederate(the_federate, the_attribute);
}

void ObjectSet::queryAttributeOwnership(FederateHandle the_federate,
                                        ObjectHandle the_object,
                                        AttributeHandle the_attribute) const
{
    if (!server) {
        Debug(D, pdDebug) << "Should only be called by RTIG" << std::endl;
        return;
    }

    Debug(D, pdDebug) << "query attribute ownership for attribute " << the_attribute << " and object " << the_object
                      << std::endl;

    auto object = getObject(the_object);

    ObjectAttribute* oa = object->getAttribute(the_attribute);

    NetworkMessage* answer;
    if (oa->getOwner()) {
        NM_Inform_Attribute_Ownership* IAO = new NM_Inform_Attribute_Ownership();
        IAO->setObject(the_object);
        IAO->setAttribute(the_attribute);
        answer = IAO;
    }
    else {
        NM_Attribute_Is_Not_Owned* AINO = new NM_Attribute_Is_Not_Owned();
        AINO->setObject(the_object);
        AINO->setAttribute(the_attribute);
        answer = AINO;
    }

    answer->setFederation(server->federation().get());
    answer->setException(Exception::Type::NO_EXCEPTION);
    answer->setFederate(oa->getOwner());

    sendToFederate(answer, the_federate);
}

void ObjectSet::negotiatedAttributeOwnershipDivestiture(
    FederateHandle, ObjectHandle, AttributeHandle*, uint16_t, const std::string&)
{
    // Object *object = getObject(the_object);
}

void ObjectSet::attributeOwnershipAcquisitionIfAvailable(FederateHandle, ObjectHandle, AttributeHandle*, uint16_t)
{
    // Object *object = getObject(the_object);
}

void ObjectSet::unconditionalAttributeOwnershipDivestiture(FederateHandle, ObjectHandle, AttributeHandle*, uint16_t)
{
    // Object *object = getObject(the_object);
}

void ObjectSet::attributeOwnershipAcquisition(
    FederateHandle, ObjectHandle, AttributeHandle*, uint16_t, const std::string&)
{
    // Object *object = getObject(the_object);
}

void ObjectSet::cancelNegotiatedAttributeOwnershipDivestiture(FederateHandle the_federate,
                                                              ObjectHandle the_object,
                                                              const std::vector<AttributeHandle>& the_attributes)
{
    if (!server) {
        Debug(D, pdExcept) << "CancelNegotiatedAttributeOwnershipDivestiture should not be called on the RTIA."
                           << std::endl;
        throw RTIinternalError("CancelNegotiatedAttributeOwnershipDivestiture called on the RTIA.");
    }

    auto object = getObject(the_object);

    for (const auto attribute_handle : the_attributes) {
        auto oa = object->getAttribute(attribute_handle);

        // Does federate owns every attributes.
        if (oa->getOwner() != the_federate) {
            throw AttributeNotOwned("");
        }
        // Does federate called NegotiatedAttributeOwnershipDivestiture
        if (!oa->beingDivested()) {
            throw AttributeDivestitureWasNotRequested("");
        }
    }

    for (const auto attribute_handle : the_attributes) {
        object->getAttribute(attribute_handle)->setDivesting(false);
    }
}

AttributeHandleSet* ObjectSet::attributeOwnershipReleaseResponse(FederateHandle /*the_federate*/,
                                                                 ObjectHandle /*the_object*/,
                                                                 std::vector<AttributeHandle>& /*the_attributes*/,
                                                                 uint16_t /*the_size*/)
{
    // Object *object = getObject(the_object);

    return 0;
}

// ----------------------------------------------------------------------------
void ObjectSet::cancelAttributeOwnershipAcquisition(FederateHandle /*the_federate*/,
                                                    ObjectHandle /*the_object*/,
                                                    std::vector<AttributeHandle>& /*the_attributes*/,
                                                    uint16_t /*the_size*/)
{
    // Object *object = getObject(the_object);
}

Object* ObjectSet::getObject(ObjectHandle the_object) const
{
    auto it = my_objects_per_handle.find(the_object);

    if (it != end(my_objects_per_handle)) {
        return it->second;
    }

    throw ObjectNotKnown("Object <" + std::to_string(the_object) + "> not found in map set.");
}

Object* ObjectSet::getObjectByName(const std::string& the_object_name) const
{
    auto it = my_objects_per_name.find(the_object_name);

    if (it != end(my_objects_per_name)) {
        return it->second;
    }

    return nullptr;
}

void ObjectSet::getAllObjectInstancesFromFederate(FederateHandle the_federate,
                                                  std::vector<ObjectHandle>& ownedObjectInstances) const
{
    ownedObjectInstances.clear();
    for (const auto& kv : my_objects_per_handle) {
        if (kv.second && kv.second->getOwner() == the_federate) {
            ownedObjectInstances.push_back(kv.first);
        }
    }
}

void ObjectSet::sendToFederate(NetworkMessage* msg, FederateHandle the_federate) const
{
    // Send the message 'msg' to the Federate which Handle is theFederate.
    Socket* socket = nullptr;
    try {
#ifdef HLA_USES_UDP
        socket = server->getSocketLink(the_federate, BEST_EFFORT);
#else
        socket = server->getSocketLink(the_federate);
#endif
        msg->send(socket, const_cast<MessageBuffer&>(NM_msgBufSend));
    }
    catch (RTIinternalError& e) {
        Debug(D, pdExcept) << "Reference to a killed Federate while broadcasting." << std::endl;
    }
    catch (NetworkError& e) {
        Debug(D, pdExcept) << "Network error while broadcasting, ignoring." << std::endl;
    }
    // BUG: If except = 0, could use Multicast.
}
}
