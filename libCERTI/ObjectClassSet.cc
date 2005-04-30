// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: ObjectClassSet.cc,v 3.20 2005/04/30 17:16:08 breholee Exp $
// ----------------------------------------------------------------------------

#include "ObjectClassSet.hh"

// Project
#include <config.h>
#include "ObjectClassBroadcastList.hh"
#include "PrettyDebug.hh"

// Standard
#include <iostream>

using std::list ;
using std::cout ;
using std::endl ;
using std::string ;

namespace certi {

static pdCDebug D("OBJECTCLASSSET", __FILE__);

// ----------------------------------------------------------------------------
//! The class is not allocated, only the pointer is memorized.
void
ObjectClassSet::addClass(ObjectClass *newClass)
{
    D.Out(pdInit, "Adding new object class %d.", newClass->getHandle());

    newClass->server = server ;

    push_front(newClass);
}

// ----------------------------------------------------------------------------
/** Build an inheritance relation between two object classes.
 */
void
ObjectClassSet::buildParentRelation(ObjectClass *subclass,
				    ObjectClass *superclass)
{
    subclass->setSuperclass(superclass->getHandle());
    subclass->setLevelId(superclass->getLevelId());
    superclass->addSubclass(subclass);
    superclass->addAttributesToChild(subclass);
}

// ----------------------------------------------------------------------------
//! Constructor.
ObjectClassSet::ObjectClassSet(SecurityServer *theSecurityServer)
    : list<ObjectClass *>()
{
    // It can be NULL on the RTIA.
    server = theSecurityServer ;
}

// ----------------------------------------------------------------------------
//! Destructor.
ObjectClassSet::~ObjectClassSet()
{
    while (!empty()) {
        delete front();
        pop_front();
    }
}

// ----------------------------------------------------------------------------
//! deleteObject.
void
ObjectClassSet::deleteObject(FederateHandle federate,
                             ObjectHandle object,
                             const char *tag)
    throw (DeletePrivilegeNotHeld, ObjectNotKnown, RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *oclass = getInstanceClass(object);

    D.Out(pdRegister,
          "Federate %d attempts to delete instance %d in class %d.",
          federate, object, oclass->getHandle());

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList *ocbList = oclass->deleteInstance(federate,
                                                               object,
                                                               tag);

    // Broadcast RemoveObject message recursively
    ObjectClassHandle current_class = 0 ;
    if (ocbList != 0) {

        current_class = oclass->getSuperclass();

        while (current_class) {
            D.Out(pdRegister,
                  "Broadcasting Remove msg to parent class %d for instance %d.",
                  current_class, object);

            // It may throw ObjectClassNotDefined
            oclass = getWithHandle(current_class);
            oclass->broadcastClassMessage(ocbList);

            current_class = oclass->getSuperclass();
        }

        delete ocbList ;
    }

    D.Out(pdRegister, "Instance %d has been deleted.", object);
}

// ----------------------------------------------------------------------------
//! Print the ObjectClasses tree to the standard output.
void
ObjectClassSet::display() const
{
    cout << " ObjectClasses :" << endl ;

    list<ObjectClass *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        (*i)->display();
    }
}

// ----------------------------------------------------------------------------
//! getAttributeHandle.
AttributeHandle
ObjectClassSet::getAttributeHandle(const char *the_name,
                                   ObjectClassHandle the_class) const
    throw (NameNotFound, ObjectClassNotDefined, RTIinternalError)
{
    ObjectClass *objectClass = 0 ;

    if (the_name == 0)
        throw RTIinternalError("");

    D.Out(pdRequest, "Looking for attribute \"%s\" of class %u...",
          the_name, the_class);

    // It may throw ObjectClassNotDefined.
    objectClass = getWithHandle(the_class);

    return objectClass->getAttributeHandle(the_name);
}

// ----------------------------------------------------------------------------
//! getAttributeName.
const char *
ObjectClassSet::getAttributeName(AttributeHandle the_handle,
                                 ObjectClassHandle the_class) const
    throw (AttributeNotDefined,
           ObjectClassNotDefined,
           RTIinternalError)
{
    ObjectClass *objectClass = NULL ;

    D.Out(pdRequest, "Looking for attribute %u of class %u...",
          the_handle, the_class);

    // It may throw ObjectClassNotDefined.
    objectClass = getWithHandle(the_class);

    return objectClass->getAttributeName(the_handle);
}

// ----------------------------------------------------------------------------
//! getInstanceClass.
ObjectClass *
ObjectClassSet::getInstanceClass(ObjectHandle theObjectHandle) const
    throw (ObjectNotKnown)
{
    list<ObjectClass *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        if ((*i)->isInstanceInClass(theObjectHandle) == true)
            return (*i);
    }

    D.Out(pdExcept, "Object instance %d not found in any object class.",
          theObjectHandle);
    throw ObjectNotKnown("");
}

// ----------------------------------------------------------------------------
/** Get object
 */
Object *
ObjectClassSet::getObject(ObjectHandle h) const
    throw (ObjectNotKnown)
{
    list<ObjectClass *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
	try {
	    Object *object = (*i)->getInstanceWithID(h);
	    return object ;
	}
	catch (ObjectNotKnown &e) {
	}
    }
    throw ObjectNotKnown("");
}

// ----------------------------------------------------------------------------
//! getObjectClassHandle.
ObjectClassHandle
ObjectClassSet::getObjectClassHandle(string class_name) const
    throw (NameNotFound)
{
    list<ObjectClass *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        if ((*i)->getName() == class_name)
            return (*i)->getHandle();
    }
    throw NameNotFound("");
}

// ----------------------------------------------------------------------------
//! getObjectClassName.
string
ObjectClassSet::getObjectClassName(ObjectClassHandle the_handle) const
    throw (ObjectClassNotDefined,
           RTIinternalError)
{
    D.Out(pdRequest, "Looking for class %u...", the_handle);

    return getWithHandle(the_handle)->getName();
}

// ----------------------------------------------------------------------------
//! getWithHandle (private method).
ObjectClass *
ObjectClassSet::getWithHandle(ObjectClassHandle theHandle) const
    throw (ObjectClassNotDefined)
{
    list<ObjectClass *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        if ((*i)->getHandle() == theHandle)
            return (*i);
    }

    D.Out(pdExcept, "Unknown Object Class Handle %d .", theHandle);

    throw ObjectClassNotDefined("Unknow class handle.");
}

// ----------------------------------------------------------------------------
//! killFederate.
void ObjectClassSet::killFederate(FederateHandle theFederate)
    throw ()
{
    ObjectClassBroadcastList *ocbList = NULL ;
    ObjectClassHandle currentClass = 0 ;

    list<ObjectClass *>::iterator i ;
    for (i = begin(); i != end(); i++) {
        // Call KillFederate on that class until it returns NULL.
        do {
            D.Out(pdExcept, "Kill Federate Handle %d .", theFederate);
            ocbList = (*i)->killFederate(theFederate);

            D.Out(pdExcept, "Federate Handle %d Killed.", theFederate);

            // Broadcast RemoveObject message recursively
            if (ocbList != 0) {
                currentClass = (*i)->getSuperclass();
                D.Out(pdExcept, "List not NULL");
                while (currentClass != 0) {
                    D.Out(pdRegister,
                          "Broadcasting Remove msg to parent class %d(Killed).",
                          currentClass);

                    // It may throw ObjectClassNotDefined
                    (*i) = getWithHandle(currentClass);

                    (*i)->broadcastClassMessage(ocbList);

                    currentClass = (*i)->getSuperclass();
                }

                delete ocbList ;
            }
        } while (ocbList != NULL);
    }
    D.Out(pdExcept, "End of the KillFederate Procedure.");
}

// ----------------------------------------------------------------------------
//! publish
void
ObjectClassSet::publish(FederateHandle theFederateHandle,
                        ObjectClassHandle theClassHandle,
                        AttributeHandle *theAttributeList,
                        UShort theListSize,
                        bool PubOrUnpub)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           RTIinternalError,
           SecurityError)
{
    // It may throw ObjectClassNotDefined
    ObjectClass *theClass = getWithHandle(theClassHandle);

    if (PubOrUnpub)
        D.Out(pdInit, "Federate %d attempts to publish Object Class %d.",
              theFederateHandle, theClassHandle);
    else
        D.Out(pdTerm, "Federate %d attempts to unpublish Object Class %d.",
              theFederateHandle, theClassHandle);

    // It may throw AttributeNotDefined
    theClass->publish(theFederateHandle,
                      theAttributeList,
                      theListSize,
                      PubOrUnpub);
}

// ----------------------------------------------------------------------------
//! registerInstance.
void
ObjectClassSet::registerObjectInstance(FederateHandle the_federate,
                                       Object *the_object,
                                       ObjectClassHandle the_class)
    throw (InvalidObjectHandle,
           ObjectClassNotDefined,
           ObjectClassNotPublished,
           ObjectAlreadyRegistered,
           RTIinternalError)
{
    ObjectClassHandle currentClass = the_class ;

    // It may throw ObjectClassNotDefined
    ObjectClass *theClass = getWithHandle(the_class);

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList = theClass->registerObjectInstance(the_federate, the_object,
                                               the_class);

    // Broadcast DiscoverObject message recursively
    if (ocbList != 0) {

        currentClass = theClass->getSuperclass();

        while (currentClass != 0) {
            D.Out(pdRegister,
                  "Broadcasting Discover msg to parent class "
                  "%d for instance %d.",
                  currentClass, the_object);
            // It may throw ObjectClassNotDefined
            theClass = getWithHandle(currentClass);

            theClass->broadcastClassMessage(ocbList);

            currentClass = theClass->getSuperclass();
        }

        delete ocbList ;
    }

    D[pdRegister] << "Instance " << the_object << " has been registered."
                  << endl ;
}

// ----------------------------------------------------------------------------
/** Subscribes a federate to a set of attributes with a region. Sends the
    discovery messages if necessary.
    @param federate Federate to subscribe
    @param class_handle Class to be subscribed
    @param attributes List of attributes to be subscribed
    @param nb Number of attributes
    @param region Subscription region (0 for default)
 */
void
ObjectClassSet::subscribe(FederateHandle federate,
                          ObjectClassHandle class_handle,
                          AttributeHandle *attributes,
                          int nb,
			  const RTIRegion *region)
    throw (ObjectClassNotDefined, AttributeNotDefined, RTIinternalError,
           SecurityError)
{
    ObjectClass *object_class = getWithHandle(class_handle);

    bool need_discover = object_class->subscribe(federate, attributes, nb, region);

    if (need_discover)
	object_class->recursiveDiscovering(federate, class_handle);
}

// ----------------------------------------------------------------------------
//! updateAttributeValues.
void
ObjectClassSet::updateAttributeValues(FederateHandle federate,
                                      ObjectHandle object_handle,
                                      AttributeHandle *attributes,
                                      AttributeValue *values,
                                      UShort nb,
                                      FederationTime time,
                                      const char *tag)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError,
           InvalidObjectHandle)
{
    Object *object = getObject(object_handle);
    ObjectClass *object_class = getWithHandle(object->getClass());
    ObjectClassHandle current_class = object_class->getHandle();

    D.Out(pdProtocol, "Federate %d Updating object %d from class %d.",
          federate, object_handle, current_class);

    // It may throw a bunch of exceptions
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList = object_class->updateAttributeValues(
	federate, object, attributes, values, nb, time, tag);

    // Broadcast ReflectAttributeValues message recursively
    current_class = object_class->getSuperclass();

    while (current_class != 0) {
        D.Out(pdProtocol,
              "Broadcasting RAV msg to parent class %d for instance %d.",
              current_class, object_handle);

        // It may throw ObjectClassNotDefined
        object_class = getWithHandle(current_class);
        object_class->broadcastClassMessage(ocbList, object);

        current_class = object_class->getSuperclass();
    }

    delete ocbList ;
}

// ----------------------------------------------------------------------------
//! negotiatedAttributeOwnershipDivestiture.
void
ObjectClassSet::
negotiatedAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                        ObjectHandle theObjectHandle,
                                        AttributeHandle *theAttributeList,
                                        UShort theListSize,
                                        const char *theTag)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           AttributeAlreadyBeingDivested,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *objectClass = getInstanceClass(theObjectHandle);
    ObjectClassHandle currentClass = objectClass->getHandle();

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList =
        objectClass->negotiatedAttributeOwnershipDivestiture(theFederateHandle,
                                                             theObjectHandle,
                                                             theAttributeList,
                                                             theListSize,
                                                             theTag);

    // Broadcast ReflectAttributeValues message recursively
    currentClass = objectClass->getSuperclass();

    while (currentClass != 0) {
        D.Out(pdProtocol,
              "Broadcasting NAOD msg to parent class %d for instance %d.",
              currentClass, theObjectHandle);

        // It may throw ObjectClassNotDefined
        objectClass = getWithHandle(currentClass);
        objectClass->broadcastClassMessage(ocbList);

        currentClass = objectClass->getSuperclass();
    }

    delete ocbList ;
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisitionIfAvailable.
void
ObjectClassSet::
attributeOwnershipAcquisitionIfAvailable(FederateHandle theFederateHandle,
                                         ObjectHandle theObjectHandle,
                                         AttributeHandle *theAttributeList,
                                         UShort theListSize)
    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           AttributeAlreadyBeingAcquired,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass * objectClass = getInstanceClass(theObjectHandle);

    // It may throw a bunch of exceptions.
    objectClass->attributeOwnershipAcquisitionIfAvailable(theFederateHandle,
                                                          theObjectHandle,
                                                          theAttributeList,
                                                          theListSize);
}

// ----------------------------------------------------------------------------
//! unconditionalAttributeOwnershipDivestiture
void
ObjectClassSet::
unconditionalAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                           ObjectHandle theObjectHandle,
                                           AttributeHandle *theAttributeList,
                                           UShort theListSize)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *objectClass = getInstanceClass(theObjectHandle);
    ObjectClassHandle currentClass = objectClass->getHandle();

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList = objectClass->
        unconditionalAttributeOwnershipDivestiture(theFederateHandle,
                                                   theObjectHandle,
                                                   theAttributeList,
                                                   theListSize);

    // Broadcast ReflectAttributeValues message recursively
    currentClass = objectClass->getSuperclass();

    while (currentClass != 0) {
        D.Out(pdProtocol,
              "Broadcasting UAOD msg to parent class %d for instance %d.",
              currentClass, theObjectHandle);

        // It may throw ObjectClassNotDefined
        objectClass = getWithHandle(currentClass);
        objectClass->broadcastClassMessage(ocbList);

        currentClass = objectClass->getSuperclass();
    }

    delete ocbList ;
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisition.
void
ObjectClassSet::
attributeOwnershipAcquisition(FederateHandle theFederateHandle,
                              ObjectHandle theObjectHandle,
                              AttributeHandle *theAttributeList,
                              UShort theListSize,
                              const char *theTag)
    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass * objectClass = getInstanceClass(theObjectHandle);

    // It may throw a bunch of exceptions.
    objectClass->attributeOwnershipAcquisition(theFederateHandle,
                                               theObjectHandle,
                                               theAttributeList,
                                               theListSize,
                                               theTag);
}

// ----------------------------------------------------------------------------
//! attributeOwnershipReleaseResponse.
AttributeHandleSet * ObjectClassSet::
attributeOwnershipReleaseResponse(FederateHandle theFederateHandle,
                                  ObjectHandle theObjectHandle,
                                  AttributeHandle *theAttributeList,
                                  UShort theListSize)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           FederateWasNotAskedToReleaseAttribute,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *objectClass = getInstanceClass(theObjectHandle);

    // It may throw a bunch of exceptions.
    return(objectClass->attributeOwnershipReleaseResponse(theFederateHandle,
                                                          theObjectHandle,
                                                          theAttributeList,
                                                          theListSize));
}

// ----------------------------------------------------------------------------
//! cancelAttributeOwnershipAcquisition.
void ObjectClassSet::
cancelAttributeOwnershipAcquisition(FederateHandle theFederateHandle,
                                    ObjectHandle theObjectHandle,
                                    AttributeHandle *theAttributeList,
                                    UShort theListSize)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeAlreadyOwned,
           AttributeAcquisitionWasNotRequested,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *objectClass = getInstanceClass(theObjectHandle);

    // It may throw a bunch of exceptions.
    objectClass->cancelAttributeOwnershipAcquisition(theFederateHandle,
                                                     theObjectHandle,
                                                     theAttributeList,
                                                     theListSize);
}

} // namespace certi

// $Id: ObjectClassSet.cc,v 3.20 2005/04/30 17:16:08 breholee Exp $
