// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: ObjectClassSet.cc,v 3.4 2003/01/16 16:05:48 breholee Exp $
// ---------------------------------------------------------------------------

#include "ObjectClassSet.hh"

namespace certi {

static pdCDebug D("OBJECTCLASSSET", "(ObjClSet) - ");

// --------------
// -- AddClass --
// --------------

void
ObjectClassSet::addClass(ObjectClass *newClass)
{
  D.Out(pdInit, "Adding new object class %d.", newClass->Handle);

  newClass->server = server;

  push_front(newClass);
}


// -------------------------
// -- BuildParentRelation --
// -------------------------

void
ObjectClassSet::buildParentRelation(ObjectClass *child, ObjectClass *parent)
{
  // Register Parent to Son
  child->Father = parent->Handle;
 
  // Transfer Security Level
  child->setLevelId(parent->getLevelId());
  
  // Register Son to Parent
  ObjectClassChild *objSon = new ObjectClassChild(child->Handle);
  parent->SonSet.Inserer(1, objSon);

  // Copy Parent Attribute into Child class.
  parent->addAttributesToChild(child);
}


// ---------------------
// -- ObjectClassSet --
// ---------------------
ObjectClassSet::ObjectClassSet(SecurityServer *theSecurityServer)
    : list<ObjectClass *>()
{
  // It can be NULL on the RTIA.
  server = theSecurityServer;
}


// ----------------------
// -- ~ObjectClassSet --
// ----------------------
ObjectClassSet::~ObjectClassSet(void)
{
    while (!empty()) {
        delete front();
        pop_front();
  }
}


// ------------------
// -- DeleteObject --
// ------------------
void
ObjectClassSet::deleteObject(FederateHandle theFederateHandle,
                             ObjectHandle   theObjectHandle,
                             const char*    theTag)
  throw(DeletePrivilegeNotHeld,
        ObjectNotKnown,
        RTIinternalError)
{
  // It may throw ObjectNotKnown
  ObjectClass *theClass = getInstanceClass(theObjectHandle);

  D.Out(pdRegister, 
	 "Federate %d attempts to delete instance %d in class %d.",
	 theFederateHandle, theObjectHandle, theClass->Handle);
  
  // It may throw a bunch of exceptions.
  ObjectClassBroadcastList *ocbList = NULL;
  ocbList = theClass->deleteInstance(theFederateHandle,
                                     theObjectHandle,
                                     theTag);
  
  // Broadcast RemoveObject message recursively
  ObjectClassHandle currentClass = 0;
  if (ocbList != 0) {

    currentClass = theClass->Father;

    while (currentClass != 0) {
        D.Out(pdRegister, 
              "Broadcasting Remove msg to parent class %d for instance %d.",
              currentClass, theObjectHandle);

      // It may throw ObjectClassNotDefined
      theClass = getWithHandle(currentClass);
      theClass->broadcastClassMessage(ocbList);

      currentClass = theClass->Father;
    }

    delete ocbList;
  }

  D.Out(pdRegister, "Instance %d has been deleted.", theObjectHandle);
}


// -------------
// -- Display --
// -------------
void
ObjectClassSet::display(void) const
{
    cout << "   ObjectClasses :" << endl;
    
    list<ObjectClass *>::const_iterator i ;
    for (i = begin(); i != end() ; i++) {
        (*i)->display();
    }
}


// ------------------------
// -- GetAttributeHandle --
// ------------------------

AttributeHandle
ObjectClassSet::getAttributeHandle(const AttributeName theName,
                                   ObjectClassHandle   theClass) const
  throw(AttributeNotDefined,
	 ObjectClassNotDefined,
	 RTIinternalError)
{
  ObjectClass *ObjectClass = NULL;

  if(theName == NULL)
    throw RTIinternalError();

  D.Out(pdRequest, "Looking for attribute \"%s\" of class %u...",
	 theName, theClass);

  // It may throw ObjectClassNotDefined.
  ObjectClass = getWithHandle(theClass);

  return ObjectClass->getAttributeHandle(theName);
}


// ----------------------
// -- GetAttributeName --
// ----------------------

const AttributeName
ObjectClassSet::getAttributeName(AttributeHandle   theHandle,
                                 ObjectClassHandle theClass) const
  throw(AttributeNotDefined,
	 ObjectClassNotDefined,
	 RTIinternalError)
{
  ObjectClass *ObjectClass = NULL;

  D.Out(pdRequest, "Looking for attribute %u of class %u...",
	 theHandle, theClass);

  // It may throw ObjectClassNotDefined.
  ObjectClass = getWithHandle(theClass);

  return ObjectClass->getAttributeName(theHandle);
}


// ----------------------
// -- GetInstanceClass --
// ----------------------

ObjectClass *
ObjectClassSet::getInstanceClass(ObjectHandle theObjectHandle) const
  throw(ObjectNotKnown)
{
  // Try to find Instance's Class
    list<ObjectClass *>::const_iterator i ;
    for (i = begin(); i != end() ; i++) {
        if ((*i)->isInstanceInClass(theObjectHandle) == true)
            return (*i);
  }

  D.Out(pdExcept, "Object instance %d not found in any object class.", 
        theObjectHandle);
  throw ObjectNotKnown();
}


// --------------------------
// -- GetObjectClassHandle --
// --------------------------

ObjectClassHandle
ObjectClassSet::getObjectClassHandle(const ObjectClassName theName) const
  throw(ObjectClassNotDefined,
	 RTIinternalError)
{
    if(theName == NULL)
        throw RTIinternalError();

  D.Out(pdRequest, "Looking for class \"%s\"...", theName);

  list<ObjectClass *>::const_iterator i ;
  for (i = begin(); i != end() ; i++) {
      if (strcmp((*i)->getName(), theName) == 0)
          return (*i)->Handle;
  }

  throw ObjectClassNotDefined();
}


// ------------------------
// -- GetObjectClassName --
// ------------------------

const ObjectClassName
ObjectClassSet::getObjectClassName(ObjectClassHandle theHandle) const
  throw(ObjectClassNotDefined,
	 RTIinternalError)
{
  D.Out(pdRequest, "Looking for class %u...", theHandle);

  return getWithHandle(theHandle)->getName();
}
    

// -------------------
// -- GetWithHandle --(privee)
// -------------------
ObjectClass *
ObjectClassSet::getWithHandle(ObjectClassHandle theHandle) const
  throw(ObjectClassNotDefined)
{
  list<ObjectClass *>::const_iterator i ;
  for (i = begin(); i != end() ; i++) {
      if ((*i)->Handle == theHandle)
          return (*i);
  }

  D.Out(pdExcept, "Unknown Object Class Handle %d .", theHandle);

  throw ObjectClassNotDefined("Unknow class handle.");
}

// ------------------
// -- KillFederate --
// ------------------

void ObjectClassSet::killFederate(FederateHandle theFederate)
  throw()
{
  ObjectClassBroadcastList *ocbList      = NULL;
  ObjectClassHandle         currentClass = 0;

  list<ObjectClass *>::iterator i ;
  for (i = begin(); i != end() ; i++) {
      // Call KillFederate on that class until it returns NULL.
      do {
          D.Out(pdExcept, "Kill Federate Handle %d .", theFederate);
          ocbList = (*i)->killFederate(theFederate);

          D.Out(pdExcept, "Federate Handle %d Killed.", theFederate);

          // Broadcast RemoveObject message recursively
          if (ocbList != 0) {
              currentClass = (*i)->Father;
              D.Out(pdExcept, "List not NULL");
              while(currentClass != 0) {
                  D.Out(pdRegister, 
                        "Broadcasting Remove msg to parent class %d(Killed).",
                        currentClass);

                  // It may throw ObjectClassNotDefined
                  (*i) = getWithHandle(currentClass);

                  (*i)->broadcastClassMessage(ocbList);

                  currentClass = (*i)->Father;
              }

              delete ocbList;
          }
      } while(ocbList != NULL);
  }
  D.Out(pdExcept, "End of the KillFederate Procedure.");
}


// -------------
// -- Publish --
// -------------

void ObjectClassSet::publish(FederateHandle     theFederateHandle,
			       ObjectClassHandle  theClassHandle,
			       AttributeHandle   *theAttributeList,
			       UShort             theListSize,
			       bool           PubOrUnpub)
  throw(ObjectClassNotDefined,
	 AttributeNotDefined,
	 RTIinternalError,
	 SecurityError)
{
  // It may throw ObjectClassNotDefined
    ObjectClass *theClass = getWithHandle(theClassHandle);

  if(PubOrUnpub == RTI_TRUE)
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

// --------------------------
// -- RecursiveDiscovering --
// --------------------------

void ObjectClassSet::recursiveDiscovering(ObjectClassHandle theClassHandle,
					    FederateHandle    theFederate,
					    ObjectClassHandle theOriginalClass)
{
  // It may throw ObjectClassNotDefined
  ObjectClass *theClass = getWithHandle(theClassHandle);
  
  D.Out(pdInit, "Recursive Discovering on class %d for Federate %d.",
	 theClassHandle, theFederate);

  Boolean  Result;  
  Result = theClass->sendDiscoverMessages(theFederate, theOriginalClass);

  if(Result == RTI_TRUE) {

    for (int i = 1; i <= theClass->SonSet.getLength(); i ++)
        recursiveDiscovering(theClass->SonSet.Ieme(i)->Handle,
                             theFederate,
                             theOriginalClass);
  }
}


// ----------------------
// -- RegisterInstance --
// ----------------------

void
ObjectClassSet::registerInstance(FederateHandle    theFederateHandle,
                                 ObjectClassHandle theClassHandle,
                                 ObjectHandle      theObjectHandle,
                                 ObjectName        theObjectName)
  throw(InvalidObjectHandle,
	 ObjectClassNotDefined,
	 ObjectClassNotPublished,
	 ObjectAlreadyRegistered,
	 RTIinternalError)
{
  ObjectClassHandle currentClass = theClassHandle;
 
  D.Out(pdRegister, 
        "Federate %d attempts to register instance %d in class %d.",
        theFederateHandle, theObjectHandle, theClassHandle);
  
  // It may throw ObjectClassNotDefined
  ObjectClass *theClass = getWithHandle(theClassHandle);
  
  // It may throw a bunch of exceptions.
  ObjectClassBroadcastList *ocbList = NULL;
  ocbList = theClass->registerInstance(theFederateHandle,
                                       theObjectHandle,
                                       theObjectName);
  
  // Broadcast DiscoverObject message recursively
  if (ocbList != 0) {

    currentClass = theClass->Father;
    
    while (currentClass != 0) {
      D.Out(pdRegister, 
            "Broadcasting Discover msg to parent class %d for instance %d.",
            currentClass, theObjectHandle);
      // It may throw ObjectClassNotDefined
      theClass = getWithHandle(currentClass);
      
      theClass->broadcastClassMessage(ocbList);
      
      currentClass = theClass->Father;
    }

    delete ocbList;
  }

  D.Out(pdRegister, "Instance %d has been registered.", theObjectHandle);
}


// ---------------
// -- Subscribe --
// ---------------
void
ObjectClassSet::subscribe(FederateHandle     theFederateHandle,
                          ObjectClassHandle  theClassHandle,
                          AttributeHandle   *theAttributeList,
                          UShort             theListSize,
                          bool               SubOrUnsub)
  throw(ObjectClassNotDefined,
	 AttributeNotDefined,
	 RTIinternalError,
	 SecurityError)
{
  ObjectClass *theClass = NULL;
  Boolean       Result;

  // It may throw ObjectClassNotDefined
  theClass = getWithHandle(theClassHandle);

  if(SubOrUnsub == RTI_TRUE)
    D.Out(pdInit, "Federate %d attempts to subscribe to Object Class %d.",
	   theFederateHandle, theClassHandle);
  else
    D.Out(pdTerm, "Federate %d attempts to unsubscribe from Object Class %d.",
	   theFederateHandle, theClassHandle);
    
  // It may throw AttributeNotDefined
  Result = theClass->subscribe(theFederateHandle,
				  theAttributeList,
				  theListSize,
				  SubOrUnsub);

  // If Result is true, the Federate has never been a subscriber of this
  // class, so it my have missed some DiscoverObject messages, for this
  // class or one of its sub-class. Therefore, we must start a recursive
  // process to check this class and its subclass for possible instances
  // to discover, until we have reached an already subscribed class in
  // each branch.

  if(Result == RTI_TRUE)
    recursiveDiscovering(theClass->Handle,  // Start process with this class
			  theFederateHandle,   // Send msgs to this Federate
			  theClass->Handle); 
  // Discovered objects belong to the same class, the original one.
}


// ---------------------------
// -- UpdateAttributeValues --
// ---------------------------
void
ObjectClassSet::updateAttributeValues(FederateHandle   theFederateHandle,
                                      ObjectHandle     theObjectHandle,
                                      AttributeHandle *theAttribArray,
                                      AttributeValue  *theValueArray,
                                      UShort           theArraySize,
                                      FederationTime   theTime,
                                      const char*      theUserTag)
  throw(ObjectNotKnown,
	 AttributeNotDefined,
	 AttributeNotOwned,
	 RTIinternalError,
	 InvalidObjectHandle)
{
  // It may throw ObjectNotKnown
  ObjectClass * objectClass = getInstanceClass(theObjectHandle);
  ObjectClassHandle currentClass = objectClass->Handle;

  D.Out(pdProtocol, "Federate %d Updating object %d from class %d.",
        theFederateHandle, theObjectHandle, currentClass);

  // It may throw a bunch of exceptions
  ObjectClassBroadcastList *ocbList = NULL;
  ocbList = objectClass->updateAttributeValues(theFederateHandle,
                                               theObjectHandle,
                                               theAttribArray,
                                               theValueArray,
                                               theArraySize,
                                               theTime,
                                               theUserTag);

  // Broadcast ReflectAttributeValues message recursively
  currentClass = objectClass->Father;
  
  while (currentClass != 0) {
    D.Out(pdProtocol, 
          "Broadcasting RAV msg to parent class %d for instance %d.",
          currentClass, theObjectHandle);
    
    // It may throw ObjectClassNotDefined
    objectClass  = getWithHandle(currentClass);
    objectClass->broadcastClassMessage(ocbList);
    
    currentClass = objectClass->Father;
  }

  delete ocbList;
}

// ------------------------------------
// -- . isAttributeOwnedByFederate --
// ------------------------------------
Boolean
ObjectClassSet::isAttributeOwnedByFederate(ObjectHandle    theObject,
                                           AttributeHandle theAttribute,
                                           FederateHandle  theFederateHandle)
    throw(ObjectNotKnown,
          AttributeNotDefined,
          RTIinternalError)
{
  // It may throw ObjectNotKnown
  ObjectClass * objectClass  = getInstanceClass(theObject);

  // It may throw a bunch of exceptions.
  return objectClass->isAttributeOwnedByFederate(theObject,
                                                 theAttribute,
                                                 theFederateHandle);
}


// -------------------------------
// -- . queryAttributeOwnership --
// -------------------------------
 void
 ObjectClassSet::queryAttributeOwnership(ObjectHandle    theObject,
                                         AttributeHandle theAttribute,
                                         FederateHandle  theFederateHandle)
     throw(ObjectNotKnown,
           AttributeNotDefined,
           RTIinternalError)
{
    D.Out(pdDebug, "queryAttributeOwnership sur l'attribut %u de l'objet %u",
          theAttribute, theObject);
 
    // It may throw ObjectNotKnown
    ObjectClass * objectClass = getInstanceClass(theObject);
 
    // It may throw a bunch of exceptions.
    objectClass->queryAttributeOwnership(theObject,
                                         theAttribute,
                                         theFederateHandle);
}



// -----------------------------------------------
// -- . NegotiatedAttributeOwnershipDivestiture --
// -----------------------------------------------
void ObjectClassSet::negotiatedAttributeOwnershipDivestiture(
   FederateHandle    theFederateHandle,
    ObjectHandle          theObjectHandle,
    AttributeHandle  *theAttributeList,
    UShort            theListSize,
   const char    *theTag)
   
  throw(ObjectNotKnown,
  AttributeNotDefined,
  AttributeNotOwned,
  AttributeAlreadyBeingDivested,
  RTIinternalError)
{
  ObjectClassBroadcastList *ocbList      = NULL;

  // It may throw ObjectNotKnown
  ObjectClass *objectClass = getInstanceClass(theObjectHandle);
  ObjectClassHandle currentClass = objectClass->Handle;
 
  // It may throw a bunch of exceptions.
  ocbList = objectClass->negotiatedAttributeOwnershipDivestiture(
               theFederateHandle,
               theObjectHandle,
               theAttributeList,
               theListSize,
               theTag);
 
  // Broadcast ReflectAttributeValues message recursively
  currentClass = objectClass->Father;
  
  while(currentClass != 0) {
    D.Out(pdProtocol, 
          "Broadcasting NAOD msg to parent class %d for instance %d.",
          currentClass, theObjectHandle);
    
    // It may throw ObjectClassNotDefined
    objectClass  = getWithHandle(currentClass);
    objectClass->broadcastClassMessage(ocbList);

    currentClass = objectClass->Father;
  }

  delete ocbList;
}

// ------------------------------------------------
// -- . AttributeOwnershipAcquisitionIfAvailable --
// ------------------------------------------------
void ObjectClassSet::attributeOwnershipAcquisitionIfAvailable(
   FederateHandle    theFederateHandle,
    ObjectHandle          theObjectHandle,
    AttributeHandle  *theAttributeList,
    UShort            theListSize)
throw(
  ObjectNotKnown,
  ObjectClassNotPublished,
  AttributeNotDefined,
  AttributeNotPublished,
  FederateOwnsAttributes,
 AttributeAlreadyBeingAcquired,
  RTIinternalError)
{
  // It may throw ObjectNotKnown
  ObjectClass * objectClass  = getInstanceClass(theObjectHandle);
 
  // It may throw a bunch of exceptions.
  objectClass->attributeOwnershipAcquisitionIfAvailable(theFederateHandle,
                                                        theObjectHandle,
                                                        theAttributeList,
                                                        theListSize);
}
  

// -----------------------------------------------
// -- . UnconditionalAttributeOwnershipDivestiture --
// -----------------------------------------------

void ObjectClassSet::unconditionalAttributeOwnershipDivestiture(
   FederateHandle    theFederateHandle,
    ObjectHandle          theObjectHandle,
    AttributeHandle  *theAttributeList,
    UShort            theListSize)
   
  throw(ObjectNotKnown,
  AttributeNotDefined,
  AttributeNotOwned,
  RTIinternalError)
{
  // It may throw ObjectNotKnown
  ObjectClass *objectClass = getInstanceClass(theObjectHandle);
  ObjectClassHandle currentClass = objectClass->Handle;

  // It may throw a bunch of exceptions.
  ObjectClassBroadcastList *ocbList = NULL;
  ocbList = objectClass->unconditionalAttributeOwnershipDivestiture(
                             theFederateHandle,
                             theObjectHandle,
                             theAttributeList,
                             theListSize);

 
  // Broadcast ReflectAttributeValues message recursively
  currentClass = objectClass->Father;
  
  while(currentClass != 0) {
    D.Out(pdProtocol, 
          "Broadcasting UAOD msg to parent class %d for instance %d.",
          currentClass, theObjectHandle);
    
    // It may throw ObjectClassNotDefined
    objectClass = getWithHandle(currentClass);
    objectClass->broadcastClassMessage(ocbList);
    
    currentClass = objectClass->Father;
  }

  delete ocbList;
}

// -------------------------------------
// -- . AttributeOwnershipAcquisition --
// -------------------------------------

void
ObjectClassSet::attributeOwnershipAcquisition(
                                           FederateHandle   theFederateHandle,
                                           ObjectHandle     theObjectHandle,
                                           AttributeHandle *theAttributeList,
                                           UShort           theListSize,
                                           const char      *theTag)
  throw(ObjectNotKnown,
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


// -----------------------------------------------------
// -- . CancelNegotiatedAttributeOwnershipDivestiture --
// -----------------------------------------------------
void ObjectClassSet::cancelNegotiatedAttributeOwnershipDivestiture(
   FederateHandle    theFederateHandle,
    ObjectHandle          theObjectHandle,
    AttributeHandle  *theAttributeList,
    UShort            theListSize)
   
  throw(  ObjectNotKnown,
  AttributeNotDefined,
  AttributeNotOwned,
 AttributeDivestitureWasNotRequested,
  RTIinternalError)
{
  // It may throw ObjectNotKnown
  ObjectClass *objectClass = getInstanceClass(theObjectHandle);
 
  // It may throw a bunch of exceptions.
  objectClass->cancelNegotiatedAttributeOwnershipDivestiture(
                                                             theFederateHandle,
                                                             theObjectHandle,
                                                             theAttributeList,
                                                             theListSize);
}


// -----------------------------------------------------
// -- . AttributeOwnershipRealeaseResponse --
// -----------------------------------------------------
AttributeHandleSet *
ObjectClassSet::attributeOwnershipRealeaseResponse(
   FederateHandle    theFederateHandle,
    ObjectHandle          theObjectHandle,
    AttributeHandle  *theAttributeList,
    UShort            theListSize)
   
  throw(
 ObjectNotKnown,
  AttributeNotDefined,
  AttributeNotOwned,
 FederateWasNotAskedToReleaseAttribute,
  RTIinternalError)
{ 
  // It may throw ObjectNotKnown
    ObjectClass *objectClass = getInstanceClass(theObjectHandle);
 
  // It may throw a bunch of exceptions.
    return(objectClass->attributeOwnershipRealeaseResponse(theFederateHandle,
                                                           theObjectHandle,
                                                           theAttributeList,
                                                           theListSize));
}


// -----------------------------------------------------
// -- . CancelAttributeOwnershipAcquisition --
// -----------------------------------------------------
void
ObjectClassSet::cancelAttributeOwnershipAcquisition(
    FederateHandle    theFederateHandle,
    ObjectHandle      theObjectHandle,
    AttributeHandle  *theAttributeList,
    UShort            theListSize)
throw(
  ObjectNotKnown,
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

}

// $Id: ObjectClassSet.cc,v 3.4 2003/01/16 16:05:48 breholee Exp $
