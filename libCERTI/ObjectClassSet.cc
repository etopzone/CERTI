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
// ----------------------------------------------------------------------------

// Project
#include "Object.hh"
#include "ObjectClass.hh"
#include "ObjectClassSet.hh"
#include "ObjectClassBroadcastList.hh"
#include "SecurityServer.hh"
#include "PrettyDebug.hh"
#include "Named.hh"

// Standard
#include <iosfwd>
#include <sstream>

namespace certi {

static PrettyDebug D("OBJECTCLASSSET", __FILE__);
static PrettyDebug G("GENDOC",__FILE__) ;

ObjectClassSet::ObjectClassSet(SecurityServer *theSecurityServer, bool isRootClassSet)
 : TreeNamedAndHandledSet<ObjectClass>("Object Classes",isRootClassSet)
{
    // It can be NULL on the RTIA.
    server = theSecurityServer ;
}

// ----------------------------------------------------------------------------
//! Destructor.
ObjectClassSet::~ObjectClassSet()
{

} /* end of ~ObjectClassSet */

void
ObjectClassSet::addClass(ObjectClass *newClass,ObjectClass *parentClass) throw (RTIinternalError)
{
	D.Out(pdInit, "Adding new object class %d.", newClass->getHandle());
	/* link to server */
    newClass->server = server ;
	add(newClass,parentClass);

} /* end of addClass */

// ----------------------------------------------------------------------------
//! deleteObject with time.
void
ObjectClassSet::deleteObject(FederateHandle federate,
                             Object* object,
			     FederationTime theTime,
                             const std::string& tag)
    throw (DeletePrivilegeNotHeld, ObjectNotKnown, RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *oclass = getInstanceClass(object->getHandle());

    D.Out(pdRegister,
          "Federate %d attempts to delete instance %d in class %d.",
          federate, object, oclass->getHandle());

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList *ocbList = oclass->deleteInstance(federate,
                                                               object,
							       theTime,
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
            oclass = getObjectFromHandle(current_class);
            oclass->broadcastClassMessage(ocbList);

            current_class = oclass->getSuperclass();
        }

        delete ocbList ;
    }
}
// ----------------------------------------------------------------------------
//! deleteObject without time.
void
ObjectClassSet::deleteObject(FederateHandle federate,
                             Object* object,
                             const std::string& tag)
    throw (DeletePrivilegeNotHeld, ObjectNotKnown, RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *oclass = getInstanceClass(object->getHandle());

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
            oclass = getObjectFromHandle(current_class);
            oclass->broadcastClassMessage(ocbList);

            current_class = oclass->getSuperclass();
        }

        delete ocbList ;
    }

    D.Out(pdRegister, "Instance %d has been deleted.", object);
}

// ----------------------------------------------------------------------------
//! getAttributeHandle.
AttributeHandle
ObjectClassSet::getAttributeHandle(const std::string& the_name,
                                   ObjectClassHandle the_class) const
    throw (NameNotFound, ObjectClassNotDefined, RTIinternalError)
{
    G.Out(pdGendoc,"enter ObjectClassSet::getAttributeHandle");

    ObjectClass    *objectClass = NULL;
    AttributeHandle      handle = 0;

    if (the_name.empty()) {
        throw RTIinternalError("provided Attribute name is null");
    }

    D.Out(pdRequest, "Looking for attribute \"%s\" of class %u...",
          the_name.c_str(), the_class);

    // It may throw ObjectClassNotDefined.
    objectClass = getObjectFromHandle(the_class);

    try
        {
         handle = objectClass->getAttributeHandle(the_name);
         G.Out(pdGendoc,"exit ObjectClassSet::getAttributeHandle");
         }
    catch (NameNotFound& )
         {
         G.Out(pdGendoc,"exit  ObjectClassset::getAttributeHandle on NameNotFound");
         throw NameNotFound (the_name) ;
         }
    return handle;
}

// ----------------------------------------------------------------------------
//! getAttributeName.
const std::string&
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
    objectClass = getObjectFromHandle(the_class);

    return objectClass->getAttributeName(the_handle);
}

// ----------------------------------------------------------------------------
//! getInstanceClass.
ObjectClass *
ObjectClassSet::getInstanceClass(ObjectHandle theObjectHandle) const
    throw (ObjectNotKnown)
{
    handled_const_iterator i ;
    for (i = fromHandle.begin(); i != fromHandle.end(); ++i) {
        if (i->second->isInstanceInClass(theObjectHandle) == true)
            return (i->second);
    }

    std::stringstream msg;
    msg << "ObjectHandle <" << theObjectHandle <<"> not found in any object class.";
    D.Out(pdExcept, msg.str().c_str());
    throw ObjectNotKnown(msg.str());
}

// ----------------------------------------------------------------------------
//! getObjectClassHandle.
ObjectClassHandle
ObjectClassSet::getObjectClassHandle(const std::string& class_name) const
throw (NameNotFound){
	return getHandleFromName(class_name);
} /* end of getObjectClassHandle */

// ----------------------------------------------------------------------------
//! getObjectClassName.
const std::string&
ObjectClassSet::getObjectClassName(ObjectClassHandle the_handle) const
    throw (ObjectClassNotDefined)
{
    D.Out(pdRequest, "Looking for class %u...", the_handle);
    return getNameFromHandle(the_handle);
}

// ----------------------------------------------------------------------------
//! killFederate.
void ObjectClassSet::killFederate(FederateHandle theFederate)
    throw ()
{
    ObjectClassBroadcastList *ocbList      = NULL ;
    ObjectClassHandle         currentClass = 0 ;

    handled_iterator i;

    for (i = fromHandle.begin(); i != fromHandle.end(); ++i) {
        // Call KillFederate on every class of the current class set
        // until it returns NULL.
        do {
            D.Out(pdExcept, "Kill Federate Handle %d .", theFederate);
            ocbList = i->second->killFederate(theFederate);

            D.Out(pdExcept, "Federate Handle %d Killed.", theFederate);

            // Broadcast RemoveObject message recursively
            // going from current class to its superclass.
            if (ocbList != NULL) {
                currentClass = i->second->getSuperclass();
                D.Out(pdExcept, "List not NULL");
                while (currentClass != 0) {
                    D.Out(pdRegister,
                          "Broadcasting Remove msg to parent class %d(Killed).",
                          currentClass);

                    // It may throw ObjectClassNotDefined
                    ObjectClass* currentClassObject = getObjectFromHandle(currentClass);

                    currentClassObject->broadcastClassMessage(ocbList);

                    currentClass = currentClassObject->getSuperclass();
                }

                delete ocbList ;
            }
        } while (ocbList != NULL);
    }
    D.Out(pdExcept, "End of the KillFederate Procedure.");
} /* end of killFederate */

// ----------------------------------------------------------------------------
void
ObjectClassSet::publish(FederateHandle theFederateHandle,
                        ObjectClassHandle theClassHandle,
                        const std::vector <AttributeHandle> &theAttributeList,
                        bool PubOrUnpub)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           RTIinternalError,
           SecurityError)
{
    // It may throw ObjectClassNotDefined
    ObjectClass *theClass = getObjectFromHandle(theClassHandle);

    if (PubOrUnpub)
        D.Out(pdInit, "Federate %d attempts to publish Object Class %d.",
              theFederateHandle, theClassHandle);
    else
        D.Out(pdTerm, "Federate %d attempts to unpublish Object Class %d.",
              theFederateHandle, theClassHandle);

    // It may throw AttributeNotDefined
    theClass->publish(theFederateHandle,
                      theAttributeList,
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
    ObjectClass *theClass = getObjectFromHandle(the_class);

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
            theClass = getObjectFromHandle(currentClass);

            theClass->broadcastClassMessage(ocbList);

            currentClass = theClass->getSuperclass();
        }

        delete ocbList ;
    }

    Debug(D, pdRegister) << "Instance " << the_object << " has been registered."
                         << std::endl ;
}

// ----------------------------------------------------------------------------
void
ObjectClassSet::subscribe(FederateHandle federate,
                          ObjectClassHandle class_handle,
                          const std::vector <AttributeHandle> &attributes,
                          const RTIRegion *region)
    throw (ObjectClassNotDefined, AttributeNotDefined, RTIinternalError,
           SecurityError)
{
    ObjectClass *object_class = getObjectFromHandle(class_handle);

    bool need_discover = object_class->subscribe(federate, attributes, region);

    if (need_discover) {
        object_class->recursiveDiscovering(federate, class_handle);
    }
} /* end of subscribe */

// ----------------------------------------------------------------------------
//! updateAttributeValues with time
void
ObjectClassSet::updateAttributeValues(FederateHandle federate,
                                      Object* object,
                                      const std::vector <AttributeHandle> &attributes,
                                      const std::vector <AttributeValue_t> &values,
                                      const FederationTime& time,
                                      const std::string& tag)
    throw (AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError,
           InvalidObjectHandle)
{
    ObjectClass *object_class = getObjectFromHandle(object->getClass());
    ObjectClassHandle current_class = object_class->getHandle();

    D.Out(pdProtocol, "Federate %d Updating object %d from class %d.",
          federate, object->getHandle(), current_class);

    // It may throw a bunch of exceptions
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList = object_class->updateAttributeValues(
                                                  federate, object, attributes, values, attributes.size(), time, tag);

    // Broadcast ReflectAttributeValues message recursively
    current_class = object_class->getSuperclass();

    while (0!=current_class) {
        D.Out(pdProtocol,
              "Broadcasting RAV msg to parent class %d for instance %d.",
              current_class, object->getHandle());

        // It may throw ObjectClassNotDefined
        object_class = getObjectFromHandle(current_class);
        object_class->broadcastClassMessage(ocbList, object);

        current_class = object_class->getSuperclass();
    }

    delete ocbList ;
}

// ----------------------------------------------------------------------------
//! updateAttributeValues without time
void
ObjectClassSet::updateAttributeValues(FederateHandle federate,
                                      Object* object,
                                      const std::vector <AttributeHandle> &attributes,
                                      const std::vector <AttributeValue_t> &values,
                                      const std::string& tag)
    throw (AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError,
           InvalidObjectHandle)
{
    ObjectClass *object_class = getObjectFromHandle(object->getClass());
    ObjectClassHandle current_class = object_class->getHandle();

    D.Out(pdProtocol, "Federate %d Updating object %d from class %d.",
          federate, object->getHandle(), current_class);

    // It may throw a bunch of exceptions
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList = object_class->updateAttributeValues(
                                                  federate, object, attributes, values, attributes.size(), tag);

    // Broadcast ReflectAttributeValues message recursively
    current_class = object_class->getSuperclass();

    while (current_class != 0) {
        D.Out(pdProtocol,
              "Broadcasting RAV msg to parent class %d for instance %d.",
              current_class, object->getHandle());

        // It may throw ObjectClassNotDefined
        object_class = getObjectFromHandle(current_class);
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
                                        Object* object,
                                        const std::vector <AttributeHandle> &theAttributeList,
                                        const std::string& theTag)
    throw (AttributeNotDefined,
           AttributeNotOwned,
           AttributeAlreadyBeingDivested,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *objectClass = getInstanceClass(object->getHandle());
    ObjectClassHandle currentClass = objectClass->getHandle();

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList =
        objectClass->negotiatedAttributeOwnershipDivestiture(theFederateHandle,
                                                             object,
                                                             theAttributeList,
                                                             theTag);

    // Broadcast ReflectAttributeValues message recursively
    currentClass = objectClass->getSuperclass();

    while (currentClass != 0) {
        D.Out(pdProtocol,
              "Broadcasting NAOD msg to parent class %d for instance %d.",
              currentClass, object->getHandle());

        // It may throw ObjectClassNotDefined
        objectClass = getObjectFromHandle(currentClass);
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
                                         Object* object,
                                         const std::vector <AttributeHandle> &theAttributeList)
    throw (ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           AttributeAlreadyBeingAcquired,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass * objectClass = getInstanceClass(object->getHandle());

    // It may throw a bunch of exceptions.
    objectClass->attributeOwnershipAcquisitionIfAvailable(theFederateHandle,
                                                          object,
                                                          theAttributeList);
}

// ----------------------------------------------------------------------------
//! unconditionalAttributeOwnershipDivestiture
void
ObjectClassSet::
unconditionalAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                           Object* object,
                                           const std::vector <AttributeHandle> &theAttributeList)
    throw (AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError)
{
    ObjectClass *objectClass = getInstanceClass(object->getHandle());
    ObjectClassHandle currentClass = objectClass->getHandle();

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList = objectClass->
        unconditionalAttributeOwnershipDivestiture(theFederateHandle, object, theAttributeList);

    // Broadcast ReflectAttributeValues message recursively
    currentClass = objectClass->getSuperclass();

    if (ocbList != NULL) {
      while (currentClass != 0) {
          D.Out(pdProtocol,
                "Broadcasting UAOD msg to parent class %d for instance %d.",
                currentClass, object->getHandle());

          // It may throw ObjectClassNotDefined
          objectClass = getObjectFromHandle(currentClass);
          objectClass->broadcastClassMessage(ocbList);
          currentClass = objectClass->getSuperclass();
      }
      delete ocbList ;
    }
} /* end of unconditionalAttributeOwnershipDivestiture */

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisition.
void
ObjectClassSet::
attributeOwnershipAcquisition(FederateHandle theFederateHandle,
                              Object* object,
                              const std::vector <AttributeHandle> &theAttributeList,
                              const std::string& theTag)
    throw (ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass * objectClass = getInstanceClass(object->getHandle());
    
    // It may throw a bunch of exceptions.
    objectClass->attributeOwnershipAcquisition(theFederateHandle, object, theAttributeList, theTag);
}

// ----------------------------------------------------------------------------
//! attributeOwnershipReleaseResponse.
AttributeHandleSet * ObjectClassSet::
attributeOwnershipReleaseResponse(FederateHandle theFederateHandle,
                                  Object* object,
                                  const std::vector <AttributeHandle> &theAttributeList)
    throw (AttributeNotDefined,
           AttributeNotOwned,
           FederateWasNotAskedToReleaseAttribute,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *objectClass = getInstanceClass(object->getHandle());

    // It may throw a bunch of exceptions.
    return objectClass->attributeOwnershipReleaseResponse(theFederateHandle, object, theAttributeList);
}

// ----------------------------------------------------------------------------
//! cancelAttributeOwnershipAcquisition.
void ObjectClassSet::
cancelAttributeOwnershipAcquisition(FederateHandle theFederateHandle,
                                    Object* object,
                                    const std::vector <AttributeHandle> &theAttributeList)
    throw (AttributeNotDefined,
           AttributeAlreadyOwned,
           AttributeAcquisitionWasNotRequested,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *objectClass = getInstanceClass(object->getHandle());

    // It may throw a bunch of exceptions.
    objectClass->cancelAttributeOwnershipAcquisition(theFederateHandle, object, theAttributeList);
}

} // namespace certi

// $Id: ObjectClassSet.cc,v 3.59 2013/09/24 14:27:57 erk Exp $
