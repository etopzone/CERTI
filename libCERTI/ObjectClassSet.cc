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
#include "Named.hh"
#include "Object.hh"
#include "ObjectClass.hh"
#include "ObjectClassBroadcastList.hh"
#include "ObjectClassSet.hh"
#include "PrettyDebug.hh"
#include "SecurityServer.hh"

// Standard
#include <iosfwd>
#include <sstream>

namespace certi {

static PrettyDebug D("OBJECTCLASSSET", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);

ObjectClassSet::ObjectClassSet(SecurityServer* theSecurityServer, bool isRootClassSet)
    : TreeNamedAndHandledSet<ObjectClass>("Object Classes", isRootClassSet)
{
    // It can be NULL on the RTIA.
    server = theSecurityServer;
}

ObjectClassSet::~ObjectClassSet()
{
}

void ObjectClassSet::addClass(ObjectClass* newClass, ObjectClass* parentClass)
{
    Debug(D, pdInit) << "Adding new object class " << newClass->getHandle() << std::endl;
    /* link to server */
    newClass->server = server;
    add(newClass, parentClass);
}

Responses
ObjectClassSet::deleteObject(FederateHandle federate, Object* object, FederationTime theTime, const std::string& tag)
{
    Responses ret;
    // It may throw ObjectNotKnown
    ObjectClass* oclass = getInstanceClass(object->getHandle());

    Debug(D, pdRegister) << "Federate " << federate << " attempts to delete instance " << object << " in class "
                         << oclass->getHandle() << std::endl;

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList* ocbList;
    std::tie(ocbList, ret) = oclass->deleteInstance(federate, object, theTime, tag);

    // Broadcast RemoveObject message recursively
    ObjectClassHandle current_class = 0;
    if (ocbList != 0) {
        current_class = oclass->getSuperclass();

        while (current_class) {
            Debug(D, pdRegister) << "Broadcasting Remove msg to parent class " << current_class << " for instance "
                                 << object << std::endl;

            // It may throw ObjectClassNotDefined
            oclass = getObjectFromHandle(current_class);
            auto resp = oclass->broadcastClassMessage(ocbList);
            ret.insert(std::end(ret), make_move_iterator(std::begin(resp)), make_move_iterator(std::end(resp)));

            current_class = oclass->getSuperclass();
        }

        delete ocbList;
    }
    return ret;
}

Responses ObjectClassSet::deleteObject(FederateHandle federate, Object* object, const std::string& tag)
{
    Responses ret;
    // It may throw ObjectNotKnown
    ObjectClass* oclass = getInstanceClass(object->getHandle());

    Debug(D, pdRegister) << "Federate " << federate << " attempts to delete instance " << object << " in class "
                         << oclass->getHandle() << std::endl;

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList* ocbList;
    std::tie(ocbList, ret) = oclass->deleteInstance(federate, object, tag);

    // Broadcast RemoveObject message recursively
    ObjectClassHandle current_class = 0;
    if (ocbList != 0) {
        current_class = oclass->getSuperclass();

        while (current_class) {
            Debug(D, pdRegister) << "Broadcasting Remove msg to parent class " << current_class << " for instance "
                                 << object << std::endl;

            // It may throw ObjectClassNotDefined
            oclass = getObjectFromHandle(current_class);
            auto resp = oclass->broadcastClassMessage(ocbList);
            ret.insert(std::end(ret), make_move_iterator(std::begin(resp)), make_move_iterator(std::end(resp)));

            current_class = oclass->getSuperclass();
        }

        delete ocbList;
    }

    Debug(D, pdRegister) << "Instance " << object << " has been deleted" << std::endl;

    return ret;
}

AttributeHandle ObjectClassSet::getAttributeHandle(const std::string& the_name, ObjectClassHandle the_class) const
{
    Debug(G, pdGendoc) << "enter ObjectClassSet::getAttributeHandle" << std::endl;

    ObjectClass* objectClass = NULL;
    AttributeHandle handle = 0;

    if (the_name.empty()) {
        throw RTIinternalError("provided Attribute name is null");
    }

    Debug(D, pdRequest) << "Looking for attribute \"" << the_name << "\" of class " << the_class << std::endl;

    // It may throw ObjectClassNotDefined.
    objectClass = getObjectFromHandle(the_class);

    try {
        handle = objectClass->getAttributeHandle(the_name);
        Debug(G, pdGendoc) << "exit ObjectClassSet::getAttributeHandle" << std::endl;
    }
    catch (NameNotFound&) {
        Debug(G, pdGendoc) << "exit  ObjectClassset::getAttributeHandle on NameNotFound" << std::endl;
        throw NameNotFound(the_name);
    }
    return handle;
}

const std::string& ObjectClassSet::getAttributeName(AttributeHandle the_handle, ObjectClassHandle the_class) const
{
    ObjectClass* objectClass = NULL;

    Debug(D, pdRequest) << "Looking for attribute " << the_handle << " of class " << the_class << std::endl;

    // It may throw ObjectClassNotDefined.
    objectClass = getObjectFromHandle(the_class);

    return objectClass->getAttributeName(the_handle);
}

ObjectClass* ObjectClassSet::getInstanceClass(ObjectHandle theObjectHandle) const
{
    handled_const_iterator i;
    for (i = fromHandle.begin(); i != fromHandle.end(); ++i) {
        if (i->second->isInstanceInClass(theObjectHandle) == true)
            return (i->second);
    }

    throw ObjectNotKnown("ObjectHandle <" + std::to_string(theObjectHandle) + "> not found in any object class.");
}

ObjectClassHandle ObjectClassSet::getObjectClassHandle(const std::string& class_name) const
{
    return getHandleFromName(class_name);
}

const std::string& ObjectClassSet::getObjectClassName(ObjectClassHandle the_handle) const
{
    Debug(D, pdRequest) << "Looking for class " << the_handle << std::endl;
    return getNameFromHandle(the_handle);
}

Responses ObjectClassSet::killFederate(FederateHandle theFederate) noexcept
{
    Responses ret;

    ObjectClassBroadcastList* ocbList = NULL;
    ObjectClassHandle currentClass = 0;

    handled_iterator i;

    for (i = fromHandle.begin(); i != fromHandle.end(); ++i) {
        // Call KillFederate on every class of the current class set
        // until it returns NULL.
        do {
            Debug(D, pdExcept) << "Kill Federate Handle " << theFederate << std::endl;

            Responses resp;
            std::tie(ocbList, resp) = i->second->killFederate(theFederate);
            ret.insert(std::end(ret), make_move_iterator(std::begin(resp)), make_move_iterator(std::end(resp)));

            Debug(D, pdExcept) << "Federate Handle " << theFederate << " Killed" << std::endl;

            // Broadcast RemoveObject message recursively
            // going from current class to its superclass.
            if (ocbList != NULL) {
                currentClass = i->second->getSuperclass();
                Debug(D, pdExcept) << "List not NULL" << std::endl;
                while (currentClass != 0) {
                    Debug(D, pdRegister) << "Broadcasting Remove msg to parent class (Killed) " << currentClass
                                         << std::endl;

                    // It may throw ObjectClassNotDefined
                    try {
                        ObjectClass* currentClassObject = getObjectFromHandle(currentClass);

                        auto resp = currentClassObject->broadcastClassMessage(ocbList);
                        ret.insert(
                            std::end(ret), make_move_iterator(std::begin(resp)), make_move_iterator(std::end(resp)));

                        currentClass = currentClassObject->getSuperclass();
                    }
                    catch (ObjectClassNotDefined& e) {
                        Debug(D, pdExcept) << "getObjectFromHandle threw when killing " << theFederate << std::endl;
                    }
                }

                delete ocbList;
            }
        } while (ocbList != NULL);
    }
    Debug(D, pdExcept) << "End of the KillFederate Procedure." << std::endl;
    return ret;
}

void ObjectClassSet::publish(FederateHandle theFederateHandle,
                             ObjectClassHandle theClassHandle,
                             const std::vector<AttributeHandle>& theAttributeList,
                             bool PubOrUnpub)
{
    // It may throw ObjectClassNotDefined
    ObjectClass* theClass = getObjectFromHandle(theClassHandle);

    Debug(D, pdInit) << "Federate " << theFederateHandle << " attempts to " << (PubOrUnpub ? "publish" : "unpublish")
                     << " Object Class " << theClassHandle << std::endl;

    // It may throw AttributeNotDefined
    theClass->publish(theFederateHandle, theAttributeList, PubOrUnpub);
}

Responses
ObjectClassSet::registerObjectInstance(FederateHandle the_federate, Object* the_object, ObjectClassHandle the_class)
{
    Responses ret;
    ObjectClassHandle currentClass = the_class;

    // It may throw ObjectClassNotDefined
    ObjectClass* theClass = getObjectFromHandle(the_class);

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList* ocbList = NULL;
    std::tie(ocbList, ret) = theClass->registerObjectInstance(the_federate, the_object, the_class);

    // Broadcast DiscoverObject message recursively
    if (ocbList != 0) {
        currentClass = theClass->getSuperclass();

        while (currentClass != 0) {
            Debug(D, pdRegister) << "Broadcasting Discover msg to parent class " << currentClass << " for instance "
                                 << the_object << std::endl;
            // It may throw ObjectClassNotDefined
            theClass = getObjectFromHandle(currentClass);

            auto resp = theClass->broadcastClassMessage(ocbList);
            ret.insert(std::end(ret), make_move_iterator(std::begin(resp)), make_move_iterator(std::end(resp)));

            currentClass = theClass->getSuperclass();
        }

        delete ocbList;
    }

    Debug(D, pdRegister) << "Instance " << the_object << " has been registered." << std::endl;
    return ret;
}

void ObjectClassSet::subscribe(FederateHandle federate,
                               ObjectClassHandle class_handle,
                               const std::vector<AttributeHandle>& attributes,
                               const RTIRegion* region)
{
    ObjectClass* object_class = getObjectFromHandle(class_handle);

    bool need_discover = object_class->subscribe(federate, attributes, region);

    if (need_discover) {
        object_class->recursiveDiscovering(federate, class_handle);
    }
}

Responses ObjectClassSet::updateAttributeValues(FederateHandle federate,
                                                Object* object,
                                                const std::vector<AttributeHandle>& attributes,
                                                const std::vector<AttributeValue_t>& values,
                                                const FederationTime& time,
                                                const std::string& tag)
{
    Responses ret;

    ObjectClass* object_class = getObjectFromHandle(object->getClass());
    ObjectClassHandle current_class = object_class->getHandle();

    Debug(D, pdProtocol) << "Federate " << federate << " Updating object " << object->getHandle() << " from class "
                         << current_class << std::endl;

    // It may throw a bunch of exceptions
    ObjectClassBroadcastList* ocbList = NULL;
    std::tie(ocbList, ret)
        = object_class->updateAttributeValues(federate, object, attributes, values, attributes.size(), time, tag);

    // Broadcast ReflectAttributeValues message recursively
    current_class = object_class->getSuperclass();

    while (0 != current_class) {
        Debug(D, pdProtocol) << "Broadcasting RAV msg to parent class " << current_class << " for instance "
                             << object->getHandle() << std::endl;

        // It may throw ObjectClassNotDefined
        object_class = getObjectFromHandle(current_class);

        auto resp = object_class->broadcastClassMessage(ocbList, object);
        ret.insert(std::end(ret), make_move_iterator(std::begin(resp)), make_move_iterator(std::end(resp)));

        current_class = object_class->getSuperclass();
    }

    delete ocbList;

    return ret;
}

Responses ObjectClassSet::updateAttributeValues(FederateHandle federate,
                                                Object* object,
                                                const std::vector<AttributeHandle>& attributes,
                                                const std::vector<AttributeValue_t>& values,
                                                const std::string& tag)
{
    Responses ret;

    ObjectClass* object_class = getObjectFromHandle(object->getClass());
    ObjectClassHandle current_class = object_class->getHandle();

    Debug(D, pdProtocol) << "Federate " << federate << " Updating object " << object->getHandle() << " from class "
                         << current_class << std::endl;

    // It may throw a bunch of exceptions
    ObjectClassBroadcastList* ocbList = NULL;
    std::tie(ocbList, ret)
        = object_class->updateAttributeValues(federate, object, attributes, values, attributes.size(), tag);

    // Broadcast ReflectAttributeValues message recursively
    current_class = object_class->getSuperclass();

    while (current_class != 0) {
        Debug(D, pdProtocol) << "Broadcasting RAV msg to parent class " << current_class << " for instance "
                             << object->getHandle() << std::endl;

        // It may throw ObjectClassNotDefined
        object_class = getObjectFromHandle(current_class);
        auto resp = object_class->broadcastClassMessage(ocbList, object);
        ret.insert(std::end(ret), make_move_iterator(std::begin(resp)), make_move_iterator(std::end(resp)));

        current_class = object_class->getSuperclass();
    }

    delete ocbList;

    return ret;
}

Responses ObjectClassSet::negotiatedAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                                                  Object* object,
                                                                  const std::vector<AttributeHandle>& theAttributeList,
                                                                  const std::string& theTag)
{
    Responses ret;

    // It may throw ObjectNotKnown
    ObjectClass* objectClass = getInstanceClass(object->getHandle());
    ObjectClassHandle currentClass = objectClass->getHandle();

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList* ocbList = NULL;
    std::tie(ocbList, ret)
        = objectClass->negotiatedAttributeOwnershipDivestiture(theFederateHandle, object, theAttributeList, theTag);

    // Broadcast ReflectAttributeValues message recursively
    currentClass = objectClass->getSuperclass();

    while (currentClass != 0) {
        Debug(D, pdProtocol) << "Broadcasting NAOD msg to parent class " << currentClass << " for instance "
                             << object->getHandle() << std::endl;

        // It may throw ObjectClassNotDefined
        objectClass = getObjectFromHandle(currentClass);
        auto resp = objectClass->broadcastClassMessage(ocbList);
        ret.insert(std::end(ret), make_move_iterator(std::begin(resp)), make_move_iterator(std::end(resp)));

        currentClass = objectClass->getSuperclass();
    }

    delete ocbList;

    return ret;
}

void ObjectClassSet::attributeOwnershipAcquisitionIfAvailable(FederateHandle theFederateHandle,
                                                              Object* object,
                                                              const std::vector<AttributeHandle>& theAttributeList)
{
    // It may throw ObjectNotKnown
    ObjectClass* objectClass = getInstanceClass(object->getHandle());

    // It may throw a bunch of exceptions.
    objectClass->attributeOwnershipAcquisitionIfAvailable(theFederateHandle, object, theAttributeList);
}

Responses ObjectClassSet::unconditionalAttributeOwnershipDivestiture(
    FederateHandle theFederateHandle, Object* object, const std::vector<AttributeHandle>& theAttributeList)
{
    Responses ret;

    ObjectClass* objectClass = getInstanceClass(object->getHandle());
    ObjectClassHandle currentClass = objectClass->getHandle();

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList* ocbList = NULL;
    std::tie(ocbList, ret)
        = objectClass->unconditionalAttributeOwnershipDivestiture(theFederateHandle, object, theAttributeList);

    // Broadcast ReflectAttributeValues message recursively
    currentClass = objectClass->getSuperclass();

    if (ocbList != NULL) {
        while (currentClass != 0) {
            Debug(D, pdProtocol) << "Broadcasting UAOD msg to parent class " << currentClass << " for instance "
                                 << object->getHandle() << std::endl;

            // It may throw ObjectClassNotDefined
            objectClass = getObjectFromHandle(currentClass);
            auto resp = objectClass->broadcastClassMessage(ocbList);
            ret.insert(std::end(ret), make_move_iterator(std::begin(resp)), make_move_iterator(std::end(resp)));

            currentClass = objectClass->getSuperclass();
        }
        delete ocbList;
    }
    return ret;
}

void ObjectClassSet::attributeOwnershipAcquisition(FederateHandle theFederateHandle,
                                                   Object* object,
                                                   const std::vector<AttributeHandle>& theAttributeList,
                                                   const std::string& theTag)
{
    // It may throw ObjectNotKnown
    ObjectClass* objectClass = getInstanceClass(object->getHandle());

    // It may throw a bunch of exceptions.
    objectClass->attributeOwnershipAcquisition(theFederateHandle, object, theAttributeList, theTag);
}

AttributeHandleSet* ObjectClassSet::attributeOwnershipReleaseResponse(
    FederateHandle theFederateHandle, Object* object, const std::vector<AttributeHandle>& theAttributeList)
{
    // It may throw ObjectNotKnown
    ObjectClass* objectClass = getInstanceClass(object->getHandle());

    // It may throw a bunch of exceptions.
    return objectClass->attributeOwnershipReleaseResponse(theFederateHandle, object, theAttributeList);
}

void ObjectClassSet::cancelAttributeOwnershipAcquisition(FederateHandle theFederateHandle,
                                                         Object* object,
                                                         const std::vector<AttributeHandle>& theAttributeList)
{
    // It may throw ObjectNotKnown
    ObjectClass* objectClass = getInstanceClass(object->getHandle());

    // It may throw a bunch of exceptions.
    objectClass->cancelAttributeOwnershipAcquisition(theFederateHandle, object, theAttributeList);
}

} // namespace certi

// $Id: ObjectClassSet.cc,v 3.59 2013/09/24 14:27:57 erk Exp $
