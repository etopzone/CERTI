// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libcerti
//
// CERTI-libcerti is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libcerti is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: ObjectClassSet.cc,v 3.0.2.1 2002/11/22 00:52:57 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "ObjectClassSet.hh"

namespace certi {

static pdCDebug D("OBJECTCLASSSET", "(ObjClSet) - ");

// --------------
// -- AddClass --
// --------------

void ObjectClassSet::addClass(ObjectClass *Class)
{
  D.Out(pdInit, "Adding new object class %d.", Class->Handle);

  Class->server = server;

  Inserer(1, Class);
}


// -------------------------
// -- BuildParentRelation --
// -------------------------

void ObjectClassSet::buildParentRelation(ObjectClass *Child,
					   ObjectClass *Parent)
{
  ObjectClassChild *ObjSon = NULL;

  // Register Parent to Son
  Child->Father   = Parent->Handle;
 
  // Transfer Security Level
  Child->setLevelId(Parent->getLevelId());
  
  // Register Son to Parent
  ObjSon = new ObjectClassChild(Child->Handle);
  Parent->SonSet.Inserer(1, ObjSon);

  // Copy Parent Attribute into Child class.
  Parent->addAttributesToChild(Child);

}


// ---------------------
// -- ObjectClassSet --
// ---------------------

ObjectClassSet::ObjectClassSet(SecurityServer *theSecurityServer)
{
  // It can be NULL on the RTIA.
  server = theSecurityServer;

  List <ObjectClass *>();
}


// ----------------------
// -- ~ObjectClassSet --
// ----------------------

ObjectClassSet::~ObjectClassSet()
{
  ObjectClass *Class = NULL;

  while(lg > 0) {
    Class = Ieme(1);
    Supprimer(1);
    delete Class;
  }
}


// ------------------
// -- DeleteObject --
// ------------------

void ObjectClassSet::deleteObject(FederateHandle    theFederateHandle,
				   ObjectHandle          theObjectHandle,
				   UserSuppliedTag   theTag)
  throw(DeletePrivilegeNotHeld,
	ObjectNotKnown,
	RTIinternalError)
{
  ObjectClass           *theClass     = NULL;
  ObjectClassBroadcastList *List         = NULL;
  ObjectClassHandle       CurrentClass = 0;

  // It may throw ObjectNotKnown
  theClass  = getInstanceClass(theObjectHandle);

  D.Out(pdRegister, 
	 "Federate %d attempts to delete instance %d in class %d.",
	 theFederateHandle, theObjectHandle, theClass->Handle);
  
  // It may throw a bunch of exceptions.
  List = theClass->deleteInstance(theFederateHandle,
				     theObjectHandle,
				     theTag);
  
  // Broadcast RemoveObject message recursively
  if(List != NULL) {

    CurrentClass = theClass->Father;
    
    while(CurrentClass != 0) {
      D.Out(pdRegister, 
	     "Broadcasting Remove msg to parent class %d for instance %d.",
	     CurrentClass, theObjectHandle);

      // It may throw ObjectClassNotDefined
      theClass = getWithHandle(CurrentClass);
      
      theClass->broadcastClassMessage(List);
      
      CurrentClass = theClass->Father;
    }

    delete List;
  }

  D.Out(pdRegister, "Instance %d has been deleted.", theObjectHandle);

}


// -------------
// -- Display --
// -------------
    
void ObjectClassSet::display(void)
{
  int i;
  ObjectClass *Class = NULL;

  printf("   ObjectClasses :\n");

  for(i = 1; i <= lg; i++) {
    Class = Ieme(i);
    Class->display();
  }

}


// ------------------------
// -- GetAttributeHandle --
// ------------------------

AttributeHandle ObjectClassSet::
getAttributeHandle(const AttributeName theName,
		    ObjectClassHandle   theClass)
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

const AttributeName ObjectClassSet::
getAttributeName(AttributeHandle   theHandle,
		  ObjectClassHandle theClass)
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
ObjectClassSet::getInstanceClass(ObjectHandle theObjectHandle)
  throw(ObjectNotKnown)
{
  int i;
  ObjectClass *object_class;
  bool found ;

  object_class = NULL;
  i = 1;
  found = false ;

  // Try to find Instance's Class
  while((i <= lg) && (!found)) {
    object_class = Ieme(i);
    found = object_class->isInstanceInClass(theObjectHandle);
    i++;
  }

  if(!found) {
    D.Out(pdExcept, 
	   "Object instance %d not found in any object class.", 
	  theObjectHandle);
    throw ObjectNotKnown();
  }
  else return object_class;
}


// --------------------------
// -- GetObjectClassHandle --
// --------------------------

ObjectClassHandle ObjectClassSet::
getObjectClassHandle(const ObjectClassName theName)
  throw(ObjectClassNotDefined,
	 RTIinternalError)
{
  int           i;
  ObjectClass *ObjectClass = NULL;

  if(theName == NULL)
    throw RTIinternalError();

  D.Out(pdRequest, "Looking for class \"%s\"...", theName);

  for(i = 1; i <= lg; i++) {
    ObjectClass = Ieme(i);
    if(strcmp(ObjectClass->getName(), theName) == 0)
      return ObjectClass->Handle;
  }

  throw ObjectClassNotDefined();
}


// ------------------------
// -- GetObjectClassName --
// ------------------------

const ObjectClassName ObjectClassSet::
getObjectClassName(ObjectClassHandle theHandle)
  throw(ObjectClassNotDefined,
	 RTIinternalError)
{
  D.Out(pdRequest, "Looking for class %u...", theHandle);

  return getWithHandle(theHandle)->getName();
}
    

// -------------------
// -- GetWithHandle --(privee)
// -------------------

ObjectClass *ObjectClassSet::getWithHandle(ObjectClassHandle theHandle)
  throw(ObjectClassNotDefined)
{
  int           i;
  ObjectClass *ObjectClass;

  for(i = 1; i <= lg; i++) {
    ObjectClass = Ieme(i);
    if(ObjectClass->Handle == theHandle)
      return ObjectClass;
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
  int                     i;
  ObjectClass           *ObjectClass;
  ObjectClassBroadcastList *List         = NULL;
  ObjectClassHandle       CurrentClass = 0;

  for(i = 1; i <= lg; i++) {
    ObjectClass = Ieme(i);
    
    // Call KillFederate on that class until it returns NULL.
    do {
      D.Out(pdExcept, "Kill Federate Handle %d .", theFederate);
      List = ObjectClass->killFederate(theFederate);

      D.Out(pdExcept, "Federate Handle %d Killed.", theFederate);

      // Broadcast RemoveObject message recursively
      if(List != NULL) {
      
	CurrentClass = ObjectClass->Father;
	D.Out(pdExcept, "List not NULL");
	while(CurrentClass != 0) {
	  D.Out(pdRegister, 
		 "Broadcasting Remove msg to parent class %d(Killed).",
		 CurrentClass);
	  
	  // It may throw ObjectClassNotDefined
	  ObjectClass = getWithHandle(CurrentClass);
	  
	  ObjectClass->broadcastClassMessage(List);
	  
	  CurrentClass = ObjectClass->Father;
	}
      
	delete List;
      }
    } while(List != NULL);
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
  ObjectClass *theClass = NULL;

  // It may throw ObjectClassNotDefined
  theClass = getWithHandle(theClassHandle);

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
  ObjectClass *theClass = NULL;
  Boolean       Result;
  int           i;

  // It may throw ObjectClassNotDefined
  theClass = getWithHandle(theClassHandle);
  
  D.Out(pdInit, "Recursive Discovering on class %d for Federate %d.",
	 theClassHandle, theFederate);
  
  Result = theClass->sendDiscoverMessages(theFederate, theOriginalClass);

  if(Result == RTI_TRUE) {

    for(i = 1; i <= theClass->SonSet.lg; i ++)
      recursiveDiscovering(theClass->SonSet.Ieme(i)->Handle,
			    theFederate,
			    theOriginalClass);
  }

}


// ----------------------
// -- RegisterInstance --
// ----------------------

void ObjectClassSet::registerInstance(FederateHandle    theFederateHandle,
					ObjectClassHandle theClassHandle,
					ObjectHandle          theObjectHandle,
					ObjectName        theObjectName)
  throw(InvalidObjectHandle,
	 ObjectClassNotDefined,
	 ObjectClassNotPublished,
	 ObjectAlreadyRegistered,
	 RTIinternalError)
{
  ObjectClass           *theClass     = NULL;
  ObjectClassBroadcastList *List         = NULL;
  ObjectClassHandle       CurrentClass = theClassHandle;
 
  D.Out(pdRegister, 
	 "Federate %d attempts to register instance %d in class %d.",
	 theFederateHandle, theObjectHandle, theClassHandle);
  
  // It may throw ObjectClassNotDefined
  theClass = getWithHandle(theClassHandle);
  
  // It may throw a bunch of exceptions.
  List = theClass->registerInstance(theFederateHandle,
				       theObjectHandle,
				       theObjectName);
  
  // Broadcast DiscoverObject message recursively
  if(List != NULL) {

    CurrentClass = theClass->Father;
    
    while(CurrentClass != 0) {
      D.Out(pdRegister, 
	     "Broadcasting Discover msg to parent class %d for instance %d.",
	     CurrentClass, theObjectHandle);
      // It may throw ObjectClassNotDefined
      theClass = getWithHandle(CurrentClass);
      
      theClass->broadcastClassMessage(List);
      
      CurrentClass = theClass->Father;
    }

    delete List;
  }

  D.Out(pdRegister, "Instance %d has been registered.", theObjectHandle);

}


// ---------------
// -- Subscribe --
// ---------------

void ObjectClassSet::subscribe(FederateHandle     theFederateHandle,
				 ObjectClassHandle  theClassHandle,
				 AttributeHandle   *theAttributeList,
				 UShort             theListSize,
				 bool            SubOrUnsub)
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

void ObjectClassSet::updateAttributeValues(FederateHandle   theFederateHandle,
					    ObjectHandle         theObjectHandle,
					    AttributeHandle *theAttribArray,
					    AttributeValue  *theValueArray,
					    UShort           theArraySize,
					    FederationTime   theTime,
					    UserSuppliedTag  theUserTag)
  throw(ObjectNotKnown,
	 AttributeNotDefined,
	 AttributeNotOwned,
	 RTIinternalError,
	 InvalidObjectHandle)
{
  ObjectClassBroadcastList *List         = NULL;
  ObjectClass           *ObjectClass;
  ObjectClassHandle       CurrentClass = 0;

  // It may throw ObjectNotKnown
  ObjectClass  = getInstanceClass(theObjectHandle);
  CurrentClass = ObjectClass->Handle;

  D.Out(pdProtocol, "Federate %d Updating object %d from class %d.",
	 theFederateHandle, theObjectHandle, CurrentClass);

  // It may throw a bunch of exceptions
  List = ObjectClass->updateAttributeValues(theFederateHandle,
					       theObjectHandle,
					       theAttribArray,
					       theValueArray,
					       theArraySize,
					       theTime,
					       theUserTag);

  // Broadcast ReflectAttributeValues message recursively
  CurrentClass = ObjectClass->Father;
  
  while(CurrentClass != 0) {
    D.Out(pdProtocol, 
	   "Broadcasting RAV msg to parent class %d for instance %d.",
	   CurrentClass, theObjectHandle);
    
    // It may throw ObjectClassNotDefined
    ObjectClass  = getWithHandle(CurrentClass);
    ObjectClass->broadcastClassMessage(List);
    
    CurrentClass = ObjectClass->Father;
  }
  
  delete List;
				  
}



// ------------------------------------
// -- . isAttributeOwnedByFederate --
// ------------------------------------

Boolean ObjectClassSet::isAttributeOwnedByFederate(ObjectHandle theObject,
                     AttributeHandle   theAttribute,
                     FederateHandle     theFederateHandle)
    throw(
  ObjectNotKnown,
  AttributeNotDefined,
  RTIinternalError)
{

  ObjectClass           *ObjectClass;


  // It may throw ObjectNotKnown
  ObjectClass  = getInstanceClass(theObject);


  // It may throw a bunch of exceptions.
  return(ObjectClass->isAttributeOwnedByFederate( theObject,
                          theAttribute,
                           theFederateHandle));
}


// -------------------------------
// -- . queryAttributeOwnership --
// -------------------------------

 void ObjectClassSet::queryAttributeOwnership(ObjectHandle theObject,
                     AttributeHandle   theAttribute,
                     FederateHandle     theFederateHandle)
    throw(
  ObjectNotKnown,
  AttributeNotDefined,
  RTIinternalError)
{
  ObjectClass           *ObjectClass;

  D.Out(pdDebug, "queryAttributeOwnership sur l'attribut %u de l'objet %u",
 theAttribute, theObject );
 
  // It may throw ObjectNotKnown
  ObjectClass  = getInstanceClass(theObject);
 
  // It may throw a bunch of exceptions.
  ObjectClass->queryAttributeOwnership( theObject, theAttribute, theFederateHandle);


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
        
  ObjectClassBroadcastList *List         = NULL;
  ObjectClass           *ObjectClass;
  ObjectClassHandle       CurrentClass = 0;

  // It may throw ObjectNotKnown
  ObjectClass  = getInstanceClass(theObjectHandle);
  CurrentClass = ObjectClass->Handle;
 
  // It may throw a bunch of exceptions.
  List =   ObjectClass->negotiatedAttributeOwnershipDivestiture(
        theFederateHandle,
           theObjectHandle,
            theAttributeList,
           theListSize,
           theTag);
 
  // Broadcast ReflectAttributeValues message recursively
  CurrentClass = ObjectClass->Father;
  
  while(CurrentClass != 0) {
    D.Out(pdProtocol, 
    "Broadcasting NAOD msg to parent class %d for instance %d.",
    CurrentClass, theObjectHandle);
    
    // It may throw ObjectClassNotDefined
    ObjectClass  = getWithHandle(CurrentClass);
    ObjectClass->broadcastClassMessage(List);
    
    CurrentClass = ObjectClass->Father;
  }
  
  delete List;        
        
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
  ObjectClass           *ObjectClass;
 
  // It may throw ObjectNotKnown
  ObjectClass  = getInstanceClass(theObjectHandle);
 
  // It may throw a bunch of exceptions.
 ObjectClass->attributeOwnershipAcquisitionIfAvailable(
        theFederateHandle,
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
        
  ObjectClassBroadcastList *List         = NULL;
  ObjectClass           *ObjectClass;
  ObjectClassHandle       CurrentClass = 0;

  // It may throw ObjectNotKnown
  ObjectClass  = getInstanceClass(theObjectHandle);
  CurrentClass = ObjectClass->Handle;
  
  // It may throw a bunch of exceptions.
  List =   ObjectClass->unconditionalAttributeOwnershipDivestiture(
        theFederateHandle,
           theObjectHandle,
            theAttributeList,
           theListSize);

 
  // Broadcast ReflectAttributeValues message recursively
  CurrentClass = ObjectClass->Father;
  
  while(CurrentClass != 0) {
    D.Out(pdProtocol, 
    "Broadcasting UAOD msg to parent class %d for instance %d.",
    CurrentClass, theObjectHandle);
    
    // It may throw ObjectClassNotDefined
    ObjectClass  = getWithHandle(CurrentClass);
    ObjectClass->broadcastClassMessage(List);
    
    CurrentClass = ObjectClass->Father;
  }
  
  delete List;        
        
}

// -------------------------------------
// -- . AttributeOwnershipAcquisition --
// -------------------------------------

void ObjectClassSet::attributeOwnershipAcquisition(
   FederateHandle    theFederateHandle,
    ObjectHandle          theObjectHandle,
    AttributeHandle  *theAttributeList,
    UShort            theListSize,
   const char    *theTag)
   
  throw(ObjectNotKnown,
  ObjectClassNotPublished,
  AttributeNotDefined,
  AttributeNotPublished,
  FederateOwnsAttributes,
  RTIinternalError)
{
        
  ObjectClass           *ObjectClass;
 
  // It may throw ObjectNotKnown
  ObjectClass  = getInstanceClass(theObjectHandle);
 
  // It may throw a bunch of exceptions.
 ObjectClass->attributeOwnershipAcquisition(
        theFederateHandle,
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

       
  ObjectClass           *ObjectClass;
 
  // It may throw ObjectNotKnown
  ObjectClass  = getInstanceClass(theObjectHandle);
 
  // It may throw a bunch of exceptions.
 ObjectClass->cancelNegotiatedAttributeOwnershipDivestiture(
        theFederateHandle,
           theObjectHandle,
            theAttributeList,
           theListSize);
        
}



// -----------------------------------------------------
// -- . AttributeOwnershipRealeaseResponse --
// -----------------------------------------------------

AttributeHandleSet *ObjectClassSet::
attributeOwnershipRealeaseResponse(
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

       
  ObjectClass           *ObjectClass;
 
  // It may throw ObjectNotKnown
  ObjectClass  = getInstanceClass(theObjectHandle);
 
  // It may throw a bunch of exceptions.
 return(ObjectClass->attributeOwnershipRealeaseResponse(
        theFederateHandle,
           theObjectHandle,
            theAttributeList,
           theListSize));
        
}


// -----------------------------------------------------
// -- . CancelAttributeOwnershipAcquisition --
// -----------------------------------------------------

void  ObjectClassSet::cancelAttributeOwnershipAcquisition(
    FederateHandle    theFederateHandle,
    ObjectHandle          theObjectHandle,
    AttributeHandle  *theAttributeList,
    UShort            theListSize)
   
throw(
  ObjectNotKnown,
  AttributeNotDefined,
  AttributeAlreadyOwned,
 AttributeAcquisitionWasNotRequested,
  RTIinternalError)
{

       
  ObjectClass           *ObjectClass;
 
  // It may throw ObjectNotKnown
  ObjectClass  = getInstanceClass(theObjectHandle);
 
  // It may throw a bunch of exceptions.
 ObjectClass->cancelAttributeOwnershipAcquisition(
        theFederateHandle,
           theObjectHandle,
            theAttributeList,
           theListSize);
        
}

}

// $Id: ObjectClassSet.cc,v 3.0.2.1 2002/11/22 00:52:57 breholee Exp $

