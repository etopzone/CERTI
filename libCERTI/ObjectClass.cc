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
// $Id: ObjectClass.cc,v 3.2 2002/11/30 22:13:32 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "ObjectClass.hh"

namespace certi {

  static pdCDebug D("OBJECTCLASS", "(ObjClass) - ");

  // ------------------
  // -- AddAttribute --
  // ------------------

  AttributeHandle ObjectClass::
  addAttribute(ObjectClassAttribute *theAttribute,
		Boolean                Inherited)
  {
    if(theAttribute == NULL)
      throw RTIinternalError("Tried to add NULL attribute.");

    theAttribute->Handle         = AttributeSet.getLength() + 1;
    theAttribute->server = server;
  
    // If the attribute is inherited, it keeps its security level.
    // If not, it takes the default security level of the class.
    if(Inherited != RTI_TRUE)
      theAttribute->LevelID = LevelID;

    AttributeSet.Inserer(1, theAttribute);

    D.Out(pdProtocol, "ObjectClass %u has a new attribute %u.",
	   Handle, theAttribute->Handle);

    return theAttribute->Handle;
  }


  // --------------------------
  // -- AddAttributesToChild --
  // --------------------------

  void ObjectClass::addAttributesToChild(ObjectClass *Child)
  {
    int AttribIndex;
    ObjectClassAttribute *Attribute      = NULL;
    ObjectClassAttribute *ChildAttribute = NULL;

    // The Attribute List is read backwards to respect the same attribute
    // order for the child(Attributes are inserted at the beginning of the list)

    for(AttribIndex = AttributeSet.getLength(); AttribIndex > 0; AttribIndex--) {
      Attribute = AttributeSet.Ieme(AttribIndex);
      assert(Attribute != NULL);

      ChildAttribute = new ObjectClassAttribute(Attribute);
      assert(ChildAttribute != NULL);
    
      D.Out(pdProtocol, 
	     "ObjectClass %u adding new attribute %d to child class %u.",
	     Handle, Attribute->Handle, Child->Handle);

      Child->addAttribute(ChildAttribute);

      if(ChildAttribute->Handle != Attribute->Handle)
	throw RTIinternalError("Error while copying child's attributes.");

    }

  }

  // ---------------------------
  // -- BroadcastClassMessage --
  // ---------------------------

  void ObjectClass::broadcastClassMessage(ObjectClassBroadcastList *List)
  {
    int                    AttrIndex = 0;
    FederateHandle         Federate  = 0;
    ObjectClassAttribute *Attribute = NULL;


	
    // 1. Set ObjectHandle to local class Handle.

    List->Message->objectClassHandle = Handle;

    // 2. Update message attribute list by removing child's attributes.

    if((List->Message->Type == m_REFLECT_ATTRIBUTE_VALUES)
	||( List->Message->Type == m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION)) {
      AttrIndex = 0;
      while(AttrIndex < List->Message->HandleArraySize) {
	// If the Attribute is not in that class, remove it from the message.
	try {
	  getAttributeWithHandle(List->Message->HandleArray [AttrIndex]);
	  AttrIndex ++;
	} catch(AttributeNotDefined &e) {
	  List->Message->removeAttribute(AttrIndex);
	}
      }

    }

    // 3. Add class/attributes subscribers to the list.

    switch(List->Message->Type) {

    case m_DISCOVER_OBJECT:
    case m_REMOVE_OBJECT:
      // Pour chaque federe, l'ajouter a la liste s'il a souscrit a au moins
      // un attribut de la classe.
      for(Federate = 1; Federate <= MaxSubscriberHandle; Federate++) {
	if(isFederateSubscriber(Federate) == RTI_TRUE)
	  List->addFederate(Federate);
      }
      break;

    case m_REFLECT_ATTRIBUTE_VALUES:
    case m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
      // Pour chaque attribut de la classe, mettre a jour la liste en ajoutant
      // les federes qui ont souscrit a l'attribut.
      for(AttrIndex = 1; AttrIndex <= AttributeSet.getLength(); AttrIndex++) {
	Attribute = AttributeSet.Ieme(AttrIndex);
	Attribute->updateBroadcastList(List);
      }
      break;
		

    default:
      throw RTIinternalError("BroadcastClassMsg: Unknown type.");
    }

    // 4. Send pending messages.
    List->sendPendingMessage(server        );
  }




  // --------------------
  // -- SendToFederate --
  // --------------------

  void ObjectClass::sendToFederate(NetworkMessage *msg,
				    FederateHandle  theFederate)
  {
    Socket    *socket   = NULL;

    // Send the message 'msg' to the Federate which Handle is theFederate

    try {
#ifdef HLA_USES_UDP
      socket = server->getSocketLink(theFederate, BEST_EFFORT);
#else
      socket = server->getSocketLink(theFederate);
#endif	
      msg->write(socket);
    }
    catch(RTIinternalError &e) {
      D.Out(pdExcept, 
	     "Reference to a killed Federate while broadcasting.");
    }
    catch(NetworkError &e) {
      D.Out(pdExcept, "Network error while broadcasting, ignoring.");
    }
 
    // BUG: If except = 0, could use Multicast. 
  }




  // ------------------
  // -- SendToOwners --
  // ------------------

  void ObjectClass::sendToOwners(CDiffusion *diffusionList,
				  ObjectHandle theObjectHandle,
				  FederateHandle  theFederate,
				  const char   *theTag,
				  TypeNetworkMessage type)
        
  {
    FederateHandle Destinataire;
    int nbAttributs = diffusionList->size;
    NetworkMessage        *Answer = NULL; 
   
    for( int i = 0 ; i <  nbAttributs ; i++)
      {
	Destinataire = diffusionList->DiffArray[i].NumeroFedere;
	if( Destinataire != 0)
	  {
	    int index = 0;
      
	    Answer = new NetworkMessage;
     
	    Answer->Type             = type;
	    Answer->NumeroFederation = server->federation();
	    Answer->NumeroFedere     = theFederate;
	    Answer->Exception        = e_NO_EXCEPTION;
	    Answer->Objectid         = theObjectHandle;
    
	    strcpy(Answer->Label, theTag);    
      
	    for(int j = i ; j < nbAttributs ; j++)
	      {
		if(diffusionList->DiffArray[j].NumeroFedere == Destinataire )
		  {
		    D.Out(pdDebug,"handle : %u",diffusionList->DiffArray[j].AttribHandle); 
		    diffusionList->DiffArray[j].NumeroFedere = 0;
		    Answer->HandleArray [index] = diffusionList->DiffArray[j].AttribHandle;
		    index++;
		  }
	      }
	    Answer->HandleArraySize  = index;    
	    D.Out(pdDebug,"Envoi message type %u ", type);
	    sendToFederate(Answer,Destinataire);      
      
	  }
      }

  }


  // -------------------------
  // -- CheckFederateAccess --
  // -------------------------

  void ObjectClass::checkFederateAccess(FederateHandle  theFederate,
					 const char *Reason)
    throw(SecurityError)
  {
    Boolean Result;

    D.Out(pdInit, "Beginning of CheckFederateAccess for the federate %d",
	   theFederate);

    // BUG: Should at least but a line in Audit
    if(server == NULL)
      return;

    Result = server->canFederateAccessData(theFederate, LevelID);

    // BUG: Should use Audit.
    if(Result != RTI_TRUE) {
      printf("Object Class %ld : SecurityError for federate %ld(%s).\n",
	      Handle, theFederate, Reason);
      throw SecurityError("Federate should not access Object Class.");
    }
  }


  // ------------------
  // -- ObjectClass --
  // ------------------

  ObjectClass::ObjectClass()
    : SonSet(), AttributeSet(), ObjectSet()
  {
    Handle   = 0;
    Name     = NULL;
    Father   = 0;

    LevelID  = PublicLevelID;

    Depth    = 0;

    server = NULL;

    MaxSubscriberHandle = 0;
  }

  // ------------------
  // -- ~ObjectClass --
  // ------------------

  ObjectClass::~ObjectClass()
  {
    ObjectClassChild     *Son       = NULL;
    Object               *Instance  = NULL;
    ObjectClassAttribute *Attribute = NULL;
  
    if(Name != NULL) {
      free(Name);
      Name = NULL;
    }

    // Deleting instances
    if(ObjectSet.getLength() > 0) 
      D.Out(pdError, 
	     "ObjectClass %d : Instances remaining while exiting...", Handle);

    while(ObjectSet.getLength() > 0) {
      Instance = ObjectSet.Ieme(1);
      ObjectSet.Supprimer(1);
      delete Instance;
    }
    
    // Deleting Class Attributes
    while(AttributeSet.getLength() > 0) {
      Attribute = AttributeSet.Ieme(1);
      AttributeSet.Supprimer(1);
      delete Attribute;
    }

    // Deleting Sons
    while(SonSet.getLength() > 0) {
      Son = SonSet.Ieme(1);
      SonSet.Supprimer(1);
      delete Son;
    }

  }


  // --------------------
  // -- DeleteInstance --
  // --------------------

  ObjectClassBroadcastList *
  ObjectClass::deleteInstance(FederateHandle   theFederateHandle,
			       ObjectHandle         theObjectHandle,
			       const char*   theUserTag)
    throw(DeletePrivilegeNotHeld,
	   ObjectNotKnown,
	   RTIinternalError)
  {
    int                     ObjIndex = 0;
    Object                *Object   = NULL;
    NetworkMessage         *Answer   = NULL;
    ObjectClassBroadcastList *List     = NULL;


    // 1. Pre-conditions checking(may throw ObjectNotKnown)
    Object = getInstanceWithID(theObjectHandle);

    // Is the Federate really the Object Owner?(Checked only on RTIG)
    if((server != NULL) &&(Object->Owner != theFederateHandle)) {
      D.Out(pdExcept, "Delete Object %d: Federate %d not owner.",
	     theObjectHandle, theFederateHandle);
      throw DeletePrivilegeNotHeld();
    }

    // 2. Remove Instance
    for(ObjIndex = 1; ObjIndex <= ObjectSet.getLength(); ObjIndex ++) {
      Object = ObjectSet.Ieme(ObjIndex);
      if(Object->ID == theObjectHandle) {
	ObjectSet.Supprimer(ObjIndex);
	delete Object;
      }
    }

    // 3. Prepare and broadcast message.

    if(server != NULL) {

      D.Out(pdRegister, 
	     "Object %u deleted in class %u, now broadcasting...", 
	     theObjectHandle, Handle);

      Answer                     = new NetworkMessage;
      Answer->Type             = m_REMOVE_OBJECT;
      Answer->NumeroFederation = server->federation();
      Answer->NumeroFedere     = theFederateHandle;
      Answer->Exception        = e_NO_EXCEPTION;
      Answer->objectClassHandle     = Handle; // Class Handle
      Answer->Objectid         = theObjectHandle;
      strcpy(Answer->Label, theUserTag);
    
      List = new ObjectClassBroadcastList(Answer, 0);
      broadcastClassMessage(List);
    }
    else {
      D.Out(pdRegister, 
	     "Object %u deleted in class %u, no broadcast to do.", 
	     theObjectHandle, Handle);
    }

    // Return the BroadcastList in case it had to be passed to the parent class.
    return List;
  }


  // -------------
  // -- Display --
  // -------------
    
  void ObjectClass::display(void)
  {
    int i;
    ObjectClassChild     *Son       = NULL;
    Object               *Object    = NULL;
    ObjectClassAttribute *Attribute = NULL;

    printf("      ObjectClass #%ld \"%s\":\n", Handle, Name);

    // Display inheritance

    printf("         Parent Class Handle: %ld\n", Father);
    printf("         Security Level: %d\n",      LevelID);
    printf("         %d Child(s):\n",            SonSet.getLength());

    for(i = 1; i <= SonSet.getLength(); i++) {
      Son = SonSet.Ieme(i);
      printf("            Son %d Handle: %ld\n", i, Son->Handle);
    }

    // Display Attributes

    printf("         %d Attribute(s):\n", AttributeSet.getLength());

    for(i = 1; i <= AttributeSet.getLength(); i++) {
      Attribute = AttributeSet.Ieme(i);
      Attribute->display();
    }

    // Display Instances

    printf("         %d Instances(s):\n", ObjectSet.getLength());

    for(i = 1; i <= ObjectSet.getLength(); i++) {
      Object = ObjectSet.Ieme(i);
      Object->display();
    }

  }


  // ------------------------
  // -- GetAttributeHandle --
  // ------------------------

  AttributeHandle ObjectClass::getAttributeHandle(const AttributeName theName)
    throw(AttributeNotDefined,
	   RTIinternalError)
  {
    int i;
    ObjectClassAttribute *Attribute = NULL;

    for(i = 1; i <= AttributeSet.getLength(); i++) {
      Attribute = AttributeSet.Ieme(i);
      if(strcmp(Attribute->getName(), theName) == 0)
	return Attribute->Handle;
    }

    D.Out(pdExcept, "ObjectClass %u: Attribute \"%s\" not defined.",
	   Handle, theName);

    throw AttributeNotDefined();
  }

  
  // ----------------------
  // -- GetAttributeName --
  // ----------------------

  const AttributeName ObjectClass::
  getAttributeName(AttributeHandle     theHandle)
    throw(AttributeNotDefined,
	   RTIinternalError)
  {
    return getAttributeWithHandle(theHandle)->getName();
  }



  // ----------------------------
  // -- GetAttributeWithHandle --(private)
  // ----------------------------

  ObjectClassAttribute *ObjectClass::
  getAttributeWithHandle(AttributeHandle theHandle)
    throw(AttributeNotDefined)
  {
    int i;
    ObjectClassAttribute *Attribute = NULL;

    for(i = 1; i <= AttributeSet.getLength(); i++) {
      Attribute = AttributeSet.Ieme(i);
      if(Attribute->Handle == theHandle)
	return Attribute;
    }

    D.Out(pdDebug, "ObjectClass %d: Attribute %d not defined.",
	   Handle, theHandle);

    throw AttributeNotDefined();
  }


  // -----------------------
  // -- GetInstanceWithId --(private)
  // -----------------------

  Object *ObjectClass::getInstanceWithID(ObjectHandle theID)
    throw(ObjectNotKnown)
  {
    int i;
    Object *Object;

    for(i = 1; i <= ObjectSet.getLength(); i++) {
      Object = ObjectSet.Ieme(i);
      if(Object->ID == theID)
	return Object;
    }

    throw ObjectNotKnown();
  }



  // -------------------------
  // -- IsFederatePublisher --
  // -------------------------

  Boolean ObjectClass::isFederatePublisher(FederateHandle theFederate)
  {
    int                    i         = 0;
    ObjectClassAttribute *Attribute = NULL;

    D.Out(pdRegister, "AttributeSet.getLength() = %d.",AttributeSet.getLength());
    for(i = 1; i < AttributeSet.getLength(); i++) {/*MODIF <=*/
      Attribute = AttributeSet.Ieme(i);
      if(Attribute->IsPublishing(theFederate) == RTI_TRUE)
	return RTI_TRUE;
    }

    return RTI_FALSE;
  }


  // --------------------------
  // -- IsFederateSubscriber --
  // --------------------------

  Boolean ObjectClass::isFederateSubscriber(FederateHandle theFederate)
  {
    int                    i         = 0;
    ObjectClassAttribute *Attribute = NULL;

    for(i = 1; i < AttributeSet.getLength(); i++) {/*MODIF <=*/
      Attribute = AttributeSet.Ieme(i);
      if(Attribute->hasSubscribed(theFederate) == RTI_TRUE)
	return RTI_TRUE;
    }

    return RTI_FALSE;
  }


  // -----------------------
  // -- IsInstanceInClass --
  // -----------------------

  Boolean ObjectClass::isInstanceInClass(ObjectHandle theID)
  {
    try {
      getInstanceWithID(theID);
    }
    catch(ObjectNotKnown &e) {
      return RTI_FALSE;
    }

    return RTI_TRUE;
  }  


  // ------------------
  // -- KillFederate --
  // ------------------

  ObjectClassBroadcastList *ObjectClass::killFederate(FederateHandle theFederate)
    throw()
  {
    int      i;
    Object *Object;

    D.Out(pdRegister, "Object Class %d: Killing Federate %d.",
	   Handle, theFederate);

    try {

      // Est-ce que le federe publie des trucs ?(pas important)
      if(isFederatePublisher(theFederate) == RTI_TRUE)
	publish(theFederate, NULL, 0, RTI_FALSE);

      // Est-ce que le federe a souscrit a quelquechose ?
      // Probleme, il ne faut pas lui envoyer de RemoveObject ou autre Discover.
      if(isFederateSubscriber(theFederate) == RTI_TRUE)
	subscribe(theFederate, NULL, 0, RTI_FALSE);

    } catch(SecurityError &e) {}

    // Est-ce que le federe possede des instances ?
    for(i = 1; i <= ObjectSet.getLength(); i++) {
      Object = ObjectSet.Ieme(i);
      if(Object->Owner == theFederate)
	// Return non-NULL to indicate that :
	//   1- A RemoveObject message should be broadcasted through parent
	//      class
	//   2- The federate may own another instance, and this function must
	//      be called again.
	return deleteInstance(theFederate, Object->ID, "Killed"); //BUG: String
    }
  
    D.Out(pdRegister, "Object Class %d:Federate %d killed.",
	   Handle, theFederate);
  
    // Return NULL if the Federate did not own any instance.
    return NULL;
  }


  // -------------
  // -- Publish --
  // -------------

  void ObjectClass::publish(FederateHandle   theFederateHandle,
			     AttributeHandle *theAttributeList,
			     UShort           theListSize,
			     bool          PubOrUnpub)
    throw(AttributeNotDefined,
	   RTIinternalError,
	   SecurityError)
  {
    UShort                 Index;
    ObjectClassAttribute *Attribute = NULL;
  
    D.Out(pdInit, "Beginning of Publish for the federate %d",theFederateHandle);
  
    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for(Index = 0; Index < theListSize; Index ++)
      getAttributeWithHandle(theAttributeList [Index]);

    // Check Security Levels
    checkFederateAccess(theFederateHandle, "Publish");

    // Reset all previous publish information.
    D.Out(pdInit, "ObjectClass %d: Reset publish info of Federate %d.",
	   Handle, theFederateHandle);

    for(Index = 1; Index < AttributeSet.getLength(); Index ++) {/*MODIF <=*/
      Attribute = AttributeSet.Ieme(Index);

      if(Attribute->IsPublishing(theFederateHandle) == RTI_TRUE)
	Attribute->publish(theFederateHandle, RTI_FALSE);
    }

    // Publish attributes one by one.
    for(Index = 0; Index < theListSize; Index ++) {
      D.Out(pdInit, "ObjectClass %d: Federate %d publishes attribute %d.",
	     Handle, theFederateHandle, theAttributeList [Index]);
      Attribute = getAttributeWithHandle(theAttributeList [Index]);
      Attribute->publish(theFederateHandle, PubOrUnpub);
    }
  }


  // ----------------------
  // -- RegisterInstance --
  // ----------------------

  ObjectClassBroadcastList *ObjectClass::
  registerInstance(FederateHandle theFederateHandle,
		    ObjectHandle       theObjectHandle,
		    ObjectName     theObjectName)
    throw(ObjectClassNotPublished,
	   ObjectAlreadyRegistered,
	   RTIinternalError)
  {
    Object                *object = NULL;
    NetworkMessage         *Answer = NULL;
    ObjectClassBroadcastList *List   = NULL;

    D.Out(pdInit, "ObjectClass.");

    // Pre-conditions checking
    if(isInstanceInClass(theObjectHandle) == RTI_TRUE) {
      D.Out(pdExcept, "exception : ObjectAlreadyRegistered.");
      throw ObjectAlreadyRegistered();
    }
 
    // This condition is only to be checked on the RTIG 
    if((server != NULL) &&
	(isFederatePublisher(theFederateHandle) != RTI_TRUE)) {
      D.Out(pdExcept, "exception : ObjectClassNotPublished.");
      throw ObjectClassNotPublished();
    }

    D.Out(pdInit, "ObjectClass1.");

    // Register new Object.
    object = new Object(theFederateHandle);

    object->ID = theObjectHandle;
    if(theObjectName != NULL)
      object->setName(theObjectName);
  
    //Gestion de la propriété :
    //Recopie des attributs au niveau de l'instance

    ObjectClassAttribute *Attribute = NULL;

    //Le fédéré ne possède que les attributs qu'il publie

    for(int k=AttributeSet.getLength();k>0;k--)
      {
	Attribute = AttributeSet.Ieme(k);
	if(Attribute->IsPublishing(theFederateHandle))
	  object->AttributeState.Inserer(1,new ObjectAttribute(Attribute->Handle,
							       theFederateHandle));
	else
	  object->AttributeState.Inserer(1,new ObjectAttribute(Attribute->Handle,0));
  
	//Le privilegeToDelete appartient au fédéré même s'il n'est pas publié 
	if( strcmp(AttributeSet.Ieme(k)->getName(),"privilegeToDelete") == 0)
	  object->AttributeState.Ieme(1)->setOwner(theFederateHandle);  
      }

    ObjectSet.Inserer(1, object);


    // Prepare and Broadcast message for this class
    if(server != NULL) {

      D.Out(pdRegister, 
	     "Object %u registered in class %u, now broadcasting...", 
	     theObjectHandle, Handle);

      Answer                     = new NetworkMessage;
      Answer->Type             = m_DISCOVER_OBJECT;
      Answer->NumeroFederation = server->federation();
      Answer->NumeroFedere     = theFederateHandle;
      Answer->Exception        = e_NO_EXCEPTION;
      Answer->objectClassHandle     = Handle; // Class Handle
      Answer->Objectid         = theObjectHandle;
      strcpy(Answer->Label, theObjectName);

      List = new ObjectClassBroadcastList(Answer, 0);
      broadcastClassMessage(List);
    }
    else {
      D.Out(pdRegister, 
	     "Object %u registered in class %u, no broadcast to do.", 
	     theObjectHandle, Handle);
    }

    // Return the BroadcastList in case it had to be passed to the parent class.
    return List;
  }


  // --------------------------
  // -- SendDiscoverMessages --
  // --------------------------

  Boolean ObjectClass::
  sendDiscoverMessages(FederateHandle    theFederate,
			 ObjectClassHandle theOriginalClass)
  {
    NetworkMessage *Message = NULL;
    Object        *Object  = NULL;
    Socket        *socket  = NULL;
    int             Index;

    // 1- If this class is not the original class, and the Federate is a 
    //    subscriber of the class, the Recursive process must be stopped, 
    //    because the Federate must have received all previous DiscoverObject
    //    Message for this class and its sub-classes.
  
    if((Handle != theOriginalClass) &&
	(isFederateSubscriber(theFederate) == RTI_TRUE))
      return RTI_FALSE;

    // 2- If there is no instance of the class, return.(the recursive process
    //    must continue).

    if(ObjectSet.getLength() == 0)
      return RTI_TRUE;

    // 3- Else prepare the common part of the Message.
    //    Messages are sent on behalf of the original class.

    Message                     = new NetworkMessage;
    Message->Type             = m_DISCOVER_OBJECT;
    Message->NumeroFederation = server->federation();
    Message->NumeroFedere     = theFederate;
    Message->Exception        = e_NO_EXCEPTION;
    Message->objectClassHandle = theOriginalClass;

    // 4- For each Object instance in the class, send a Discover message.

    for(Index = 1; Index <= ObjectSet.getLength(); Index ++) {

      Object = ObjectSet.Ieme(Index);
    
      D.Out(pdInit, 
	     "Sending DiscoverObj to Federate %d for Object %u in class %u ", 
	     theFederate, Object->ID, Handle, Message->Label);
    
      Message->Objectid         = Object->ID;
      Object->getName(Message->Label);

      // Send Message to Federate
      try {
	socket = server->getSocketLink(theFederate);
	Message->write(socket);
      }
      catch(RTIinternalError &e) {
	D.Out(pdExcept, "Reference to a killed Federate while sending DO msg.");
      }
      catch(NetworkError &e) {
	D.Out(pdExcept, "Network error while sending DO msg, ignoring.");
      }
    
    } // for each object instance

    delete Message;

    // 5- The same method must be called on my sub-classes.

    return RTI_TRUE;
  }


  // -------------
  // -- SetName --
  // -------------

  void ObjectClass::setName(char *NewName)
    throw(ValueLengthExceeded, RTIinternalError)
  {
    // Check Length
    if((NewName == NULL) ||(strlen(NewName) > MAX_USER_TAG_LENGTH)) {
      D.Out(pdExcept, "Object class Name %s too long.", NewName);
      throw ValueLengthExceeded("Object class name too long.");
    }

    // Free previous name
    if(Name != NULL)
      free(Name);

    // Store new name
    Name = strdup(NewName);
    if(Name == NULL)
      throw RTIinternalError("Memory Exhausted.");
  }


  // ----------------
  // -- SetLevelID --
  // ----------------

  void ObjectClass::setLevelId(SecurityLevelID NewLevelID)
  {
    if(server->dominates(NewLevelID, LevelID) == RTI_FALSE)
      throw SecurityError("Attempt to lower object class level.");

    LevelID = NewLevelID;
  }


  // ---------------
  // -- Subscribe --
  // ---------------

  Boolean ObjectClass::subscribe(FederateHandle   theFederate,
				  AttributeHandle *theAttributeList,
				  UShort           theListSize,
				  bool         SubOrUnsub)
    throw(AttributeNotDefined,
	   RTIinternalError,
	   SecurityError)
  {
    Boolean                WasPreviousSubscriber = RTI_FALSE;

    UShort                 Index;
    ObjectClassAttribute *Attribute = NULL;

    // Check Security Levels
    checkFederateAccess(theFederate, "Subscribe");

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for(Index = 0; Index < theListSize; Index ++)
      getAttributeWithHandle(theAttributeList [Index]);

    // Save the Federate number.
    if(theFederate > MaxSubscriberHandle)
      MaxSubscriberHandle = theFederate;

    // A new subscribtion invalidates all previous subscription to the same
    // object class, so we first remove all previous subscription information.
    D.Out(pdInit, "ObjectClass %d: Resetting previous Sub info of Federate %d.",
	   Handle, theFederate);

    for(Index = 1; Index < AttributeSet.getLength(); Index ++) { /*MODIF <=*/
      Attribute = AttributeSet.Ieme(Index);

      if(Attribute->hasSubscribed(theFederate) == RTI_TRUE) {
	Attribute->subscribe(theFederate, RTI_FALSE);
	WasPreviousSubscriber = RTI_TRUE;
      }
    }

    // Subscribe to attributes one by one.
    for(Index = 0; Index < theListSize; Index ++) {
      D.Out(pdInit, "ObjectClass %d: Federate %d subscribes to attribute %d.",
	     Handle, theFederate, theAttributeList [Index]);
      Attribute = getAttributeWithHandle(theAttributeList [Index]);
      Attribute->subscribe(theFederate, SubOrUnsub);
    }

    // If the Federate was not a subscriber before, and has now subscribed
    // to at least one attribute, it must discover class' current instances.

    // BUG: If the Federate unsubscribe, he should receive RemoveObject msgs?

    if((WasPreviousSubscriber == RTI_FALSE) && // Not sub. before
	(SubOrUnsub == RTI_TRUE) &&             // subscribe(and not Unsub.)
	(theListSize > 0))                      // at least to 1 attribute.
      return RTI_TRUE;
    else
      return RTI_FALSE;
  }


  // ---------------------------
  // -- UpdateAttributeValues --
  // ---------------------------

  ObjectClassBroadcastList *ObjectClass::
  updateAttributeValues(FederateHandle     theFederateHandle,
			 ObjectHandle           theObjectHandle,
			 AttributeHandle   *theAttributeArray,
			 AttributeValue    *theValueArray,
			 UShort             theArraySize,
			 FederationTime     theTime,
			 const char*     theUserTag)
    throw(ObjectNotKnown,
	   AttributeNotDefined,
	   AttributeNotOwned,
	   RTIinternalError,
	   InvalidObjectHandle)
  {
    int                     i;
    Object                *Object = NULL;
    NetworkMessage         *Answer = NULL;
    ObjectClassBroadcastList *List   = NULL;


    // Pre-conditions checking
    Object = getInstanceWithID(theObjectHandle);


    //Gestion de la propriété
    //Test de propriété sur chaque attribut avant mise à jour

    for(i = 0; i < theArraySize; i++) 
      for(int k=1;k<=Object->AttributeState.getLength();k++)
	{
	  if(theAttributeArray [i] == Object->AttributeState.Ieme(k)->Handle )
	    {
	      if( Object->AttributeState.Ieme(k)->getOwner() != theFederateHandle )
		throw AttributeNotOwned(); 
	    }
	}      


    // Federate must be Owner of all attributes(not Owner of the instance).
    //   if(Object->Owner != theFederateHandle)
    //     throw AttributeNotOwned();

    // Prepare and Broadcast message for this class
    if(server != NULL) {

      Answer                     = new NetworkMessage;

      Answer->Type             = m_REFLECT_ATTRIBUTE_VALUES;
      Answer->NumeroFederation = server->federation();
      Answer->NumeroFedere     = theFederateHandle;
      Answer->Exception        = e_NO_EXCEPTION;
      Answer->Objectid         = theObjectHandle;
      Answer->Date             = theTime;

      strcpy(Answer->Label, theUserTag);

      Answer->HandleArraySize  = theArraySize;

      for(i = 0; i < theArraySize; i++) {
	Answer->HandleArray [i] = theAttributeArray [i];
	Answer->setValue(i,   theValueArray     [i]);
      }

      List = new ObjectClassBroadcastList(Answer, AttributeSet.getLength());

      D.Out(pdProtocol, 
	     "Object %u updated in class %u, now broadcasting...", 
	     theObjectHandle, Handle);

      broadcastClassMessage(List);
    }
    else {
      D.Out(pdExcept, 
	     "UpdateAttributeValues should not be called on the RTIA.");
      throw RTIinternalError("UpdateAttributeValues called on the RTIA.");
    }

    // Return the BroadcastList in case it had to be passed to the parent class.
    return List;

  }




  // ------------------------------------
  // -- . isAttributeOwnedByFederate --
  // ------------------------------------

  Boolean ObjectClass::isAttributeOwnedByFederate(ObjectHandle theObject,
						   AttributeHandle   theAttribute,
						   FederateHandle     theFederateHandle)
    throw(
	   ObjectNotKnown,
	   AttributeNotDefined,
	   RTIinternalError)
  {
  
    Object* Object;
    ObjectAttribute   *attribute = NULL; 
  
    // Pre-conditions checking
 
    // may throw ObjectNotKnown  
    Object = getInstanceWithID(theObject);
 
    // It may throw AttributeNotDefined     
    getAttributeWithHandle(theAttribute); 
 

    D.Out(pdDebug, "isAttributeOwnedByFederate sur l'attribut %u de l'objet %u",
	   theAttribute, theObject );
  
    if(server != NULL) {
 
      for(int k = 1 ; k <= Object->AttributeState.getLength() ; k++)
	{
	  attribute = Object->AttributeState.Ieme(k);     
	  if(theAttribute == attribute->Handle )
	    {
	      if( attribute->getOwner() == theFederateHandle)
		return(RTI_TRUE);
	      else
		return(RTI_FALSE);
	    }
	}
    }
    else {
      D.Out(pdDebug,"Only called on RTIG");
    }

    return RTI_FALSE ; 
  }


  // ------------------------------------
  // -- . queryAttributeOwnership --
  // ------------------------------------

  void ObjectClass::queryAttributeOwnership( ObjectHandle theObject,
					     AttributeHandle   theAttribute,
					     FederateHandle theFederateHandle)
    throw(
	   ObjectNotKnown,
	   AttributeNotDefined,
	   RTIinternalError)
  {
    Object* Object;
    ObjectAttribute   *attribute = NULL; 
    NetworkMessage         *Answer = NULL;
 
    // Pre-conditions checking
 
    // may throw ObjectNotKnown  
    Object = getInstanceWithID(theObject);
 
    // It may throw AttributeNotDefined     
    getAttributeWithHandle(theAttribute); 

    D.Out(pdDebug, "queryAttributeOwnership sur l'attribut %u de l'objet %u",
	   theAttribute, theObject );
  
    if(server != NULL) {

      Answer                     = new NetworkMessage;
    
      Answer->NumeroFederation = server->federation();
      Answer->Exception        = e_NO_EXCEPTION;
      Answer->Objectid         = theObject;
      Answer->HandleArray[0]   = theAttribute;
  
      for(int k = 1 ; k <= Object->AttributeState.getLength() ; k++)
	{
	  attribute = Object->AttributeState.Ieme(k);     
	  if(theAttribute == attribute->Handle )
	    {
	      Answer->NumeroFedere = attribute->getOwner();
	    }
	}
     
      if(Answer->NumeroFedere != 0)
	Answer->Type             = m_INFORM_ATTRIBUTE_OWNERSHIP;
      else
	Answer->Type             = m_ATTRIBUTE_IS_NOT_OWNED;       
     
      sendToFederate(Answer,theFederateHandle); 
 
    }
    else {
      D.Out(pdDebug,"Only called on RTIG");
    }


  }

  // -----------------------------------------------
  // -- . NegotiatedAttributeOwnershipDivestiture --
  // -----------------------------------------------

  ObjectClassBroadcastList *
  ObjectClass::negotiatedAttributeOwnershipDivestiture(
						       FederateHandle    theFederateHandle,
						       ObjectHandle          theObjectHandle,
						       AttributeHandle  *theAttributeList,
						       UShort            theListSize,
						       const char    *theTag)
    throw(
	   ObjectNotKnown,
	   AttributeNotDefined,
	   AttributeNotOwned,
	   AttributeAlreadyBeingDivested, 
	   RTIinternalError)
  {
    Object                *Object = NULL;
    int                    compteur_acquisition = 0;
    int                    compteur_assumption = 0;
    int                    compteur_divestiture = 0; 
    NetworkMessage         *AnswerAssumption = NULL;
    //  NetworkMessage         *AnswerNotification = NULL;
    NetworkMessage         *AnswerDivestiture = NULL;   
    ObjectClassBroadcastList *List   = NULL;
    FederateHandle     NewOwner;
    ObjectClassAttribute *classAttribute = NULL; 
    ObjectAttribute   *attribute = NULL; 

 
    // Pre-conditions checking
 
    // may throw ObjectNotKnown
    Object = getInstanceWithID(theObjectHandle);

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for(int Index = 0; Index < theListSize; Index ++)
      getAttributeWithHandle(theAttributeList [Index]); 

    //Le fédéré est-il propriétaire de tous les attributs
    //Le fédéré n'a t-il pas déja fait appel à 
    //NegotiatedAttributeOwnershipDivestiture
  
    D.Out(pdDebug, "NegotiatedDivestiture Demandeur : %u",theFederateHandle);
       
    for(int i = 0; i < theListSize; i++) 
      for(int k=1;k<=Object->AttributeState.getLength();k++)
	{
	  classAttribute = AttributeSet.Ieme(k);
	  attribute = Object->AttributeState.Ieme(k);
	  if(theAttributeList [i] == attribute->Handle )
	    {
	      D.Out(pdDebug, "Attribute Name : %s",classAttribute->getName());
	      D.Out(pdDebug, "Attribute Handle : %u",attribute->Handle);
	      D.Out(pdDebug, "Attribute Owner : %u",attribute->getOwner()); 
	      if( attribute->getOwner() != theFederateHandle )
		throw AttributeNotOwned();
	      if( attribute->beingDivested())
		throw AttributeAlreadyBeingDivested();
	    }
	}
   
     
    if(server != NULL) {
 
      AnswerDivestiture = new NetworkMessage;
 
      AnswerAssumption = new NetworkMessage;
 
      AnswerAssumption->HandleArraySize  = theListSize;
  
      CDiffusion *diffusionAcquisition = new CDiffusion(); 
     
      for(int i = 0; i < theListSize; i++) 
	for(int k=1;k<=Object->AttributeState.getLength();k++)
	  {
	    classAttribute = AttributeSet.Ieme(k);
	    attribute = Object->AttributeState.Ieme(k);     
	    if(theAttributeList [i] == attribute->Handle )
	      {
		if( attribute->OwnerCandidate.getLength() != 0 )
		  {//Un attributeOwnershipAcquisition est en cours sur cet
		    //attribut
            
		    //Le demandeur le plus récent devient propriétaire       
		    NewOwner = attribute->getCandidate(1);

		    attribute->setOwner(NewOwner);
     
		    //On le supprime de la liste des demandeurs
		    attribute->removeCandidate(NewOwner);
     
		    //On réinitialise divesting
		    attribute->setDivesting(RTI_FALSE);

		    diffusionAcquisition->DiffArray[compteur_acquisition].NumeroFedere 
		      = NewOwner;
		    diffusionAcquisition->DiffArray[compteur_acquisition].AttribHandle 
		      = attribute->Handle;
		    compteur_acquisition++;
            
		    AnswerDivestiture->HandleArray [compteur_divestiture] 
		      = theAttributeList [i];
		    compteur_divestiture++;
     
		    if( strcmp(classAttribute->getName(),"privilegeToDelete") == 0)
		      Object->Owner = NewOwner;            
            
		  }
		else
		  {         
		    AnswerAssumption->HandleArray [compteur_assumption] 
		      = theAttributeList [i];
		    Object->AttributeState.Ieme(k)->setDivesting(RTI_TRUE);
		    compteur_assumption++;

		  }           
	      }
	  }
   
      if(compteur_acquisition != 0)
	{
	  diffusionAcquisition->size =compteur_acquisition;
	  sendToOwners(diffusionAcquisition,theObjectHandle,theFederateHandle,theTag,
			m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION);
	}
      delete diffusionAcquisition; 

      if(compteur_divestiture !=0)
	{       

	  AnswerDivestiture->Type=m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION;
	  AnswerDivestiture->NumeroFederation = server->federation();
	  AnswerDivestiture->NumeroFedere     = theFederateHandle;
	  AnswerDivestiture->Exception        = e_NO_EXCEPTION;

	  AnswerDivestiture->Objectid         = theObjectHandle;
	  strcpy(AnswerDivestiture->Label, "\0");
	  AnswerDivestiture->HandleArraySize  = compteur_divestiture;

	  sendToFederate(AnswerDivestiture,theFederateHandle);
   
	}
      else
	delete AnswerDivestiture;
        
        
      if(compteur_assumption !=0)
	{       

	  AnswerAssumption->Type = m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION;
	  AnswerAssumption->NumeroFederation = server->federation();
	  AnswerAssumption->NumeroFedere     = theFederateHandle;
	  AnswerAssumption->Exception        = e_NO_EXCEPTION;

	  AnswerAssumption->Objectid         = theObjectHandle;
	  strcpy(AnswerAssumption->Label, theTag);
	  AnswerAssumption->HandleArraySize  = compteur_assumption;

	  List = new ObjectClassBroadcastList(AnswerAssumption, AttributeSet.getLength());

	  D.Out(pdProtocol, 
		 "Object %u divestiture in class %u, now broadcasting...", 
		 theObjectHandle, Handle);
	  broadcastClassMessage(List);
   
	}
      else
	delete AnswerAssumption;        
    }
    else {
      D.Out(pdExcept, 
	     "NegotiatedAttributeOwnershipDivestiture should not "
	     "be called on the RTIA.");
      throw RTIinternalError("NegotiatedAttributeOwnershipDivestiture "
			      "called on the RTIA.");
    }

    // Return the BroadcastList in case it had to be passed to the parent class.
    return List;

  }


  // ------------------------------------------------
  // -- . AttributeOwnershipAcquisitionIfAvailable --
  // ------------------------------------------------

  void
  ObjectClass::attributeOwnershipAcquisitionIfAvailable(
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

    Object                *Object = NULL;
    NetworkMessage         *Answer_unavailable = NULL;
    NetworkMessage         *Answer_notification = NULL; 
    int          compteur_unavailable = 0;
    int          compteur_notification = 0;
    int          compteur_divestiture = 0;
    FederateHandle    OldOwner;    
    ObjectClassAttribute *classAttribute = NULL; 
    ObjectAttribute   *attribute = NULL;
  
    // Pre-conditions checking
 
    //It may throw ObjectNotKnown.
    Object = getInstanceWithID(theObjectHandle);

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for(int Index = 0; Index < theListSize; Index ++)
      getAttributeWithHandle(theAttributeList [Index]);


    if(server != NULL) {
 

      //Le fédéré doit publier la classe
      if(!isFederatePublisher(theFederateHandle)) {
	D.Out(pdExcept, "exception : ObjectClassNotPublished.");
	throw ObjectClassNotPublished();
      }
     
      //rem AttributeSet.getLength()=AttributeState.getLength()
      int i;
      for(i = 0 ; i < theListSize ; i++)
	{
	  for(int k = 1 ; k <= AttributeSet.getLength() ; k++)
	    {
	      classAttribute = AttributeSet.Ieme(k);
	      attribute = Object->AttributeState.Ieme(k);     
	      if(theAttributeList [i] == AttributeSet.Ieme(k)->Handle )
		{
		  //Le fédéré doit publier les attributs qu'il souhaite acquérir       
		  if(!classAttribute->IsPublishing(theFederateHandle)
		      &&(strcmp(classAttribute->getName(),"privilegeToDelete") != 0))
		    throw AttributeNotPublished(); 
		  //Le fédéré n'est-il pas déjà propriétaire de certains attributs     
		  if( attribute->getOwner() == theFederateHandle )
		    throw FederateOwnsAttributes();
		  //Le fédéré n'a t-il pas déja une procédure d'acquisition en cours     
		  if( attribute->isCandidate(theFederateHandle) != 0)
		    throw AttributeAlreadyBeingAcquired();
		}
	    }
	}      


      Answer_notification = new NetworkMessage;
 
      Answer_notification->Type = m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION;
      Answer_notification->NumeroFederation = server->federation();
      Answer_notification->NumeroFedere     = theFederateHandle;
      Answer_notification->Exception        = e_NO_EXCEPTION;
      Answer_notification->Objectid         = theObjectHandle;

      Answer_unavailable = new NetworkMessage;     
      Answer_unavailable->Type             = m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE;
      Answer_unavailable->NumeroFederation = server->federation();
      Answer_unavailable->NumeroFedere     = theFederateHandle;
      Answer_unavailable->Exception        = e_NO_EXCEPTION;
      Answer_unavailable->Objectid         = theObjectHandle; 

      CDiffusion *diffusionDivestiture = new CDiffusion();


      //
      //Ce service ne doit pas ajouter le fédéré demandeur à la liste 
      //des candidats!!!
      //
  
      for(i = 0; i < theListSize; i++)
	{
	  for(int k=1;k<=Object->AttributeState.getLength();k++)
	    {
	      classAttribute = AttributeSet.Ieme(k);     
	      attribute = Object->AttributeState.Ieme(k);       
	      if(theAttributeList [i] == attribute->Handle )
		{


		  OldOwner = attribute->getOwner();
		  if(( OldOwner == 0 )
		       ||( attribute->beingDivested()) )
		    {//Cet attribut est libre ou offert par son propriétaire
		      //S'il est offert      
		      if(attribute->beingDivested())
			{
			  diffusionDivestiture->DiffArray[compteur_divestiture].NumeroFedere 
			    = OldOwner;
			  diffusionDivestiture->DiffArray[compteur_divestiture].AttribHandle 
			    = attribute->Handle;
			  compteur_divestiture++;
			}           
		      //Qu'il soit offert ou libre       
		      Answer_notification->HandleArray [compteur_notification]  
			= theAttributeList [i];
		      attribute->setOwner(theFederateHandle);
		      attribute->setDivesting(RTI_FALSE);       
		      compteur_notification++;
		      //Object->Owner reste le champ de référence pour le privilegeToDelete
		      if( strcmp(classAttribute->getName(),"privilegeToDelete") == 0)
			Object->Owner = theFederateHandle;       
		    }
		  else
		    {//Cet attribut n'est pas disponible!!!
		      Answer_unavailable->HandleArray [compteur_unavailable]  
			= theAttributeList [i];
		      attribute->addCandidate(theFederateHandle);     
		      compteur_unavailable++;           
		    }                        
		}
	    }
	}   

      if(compteur_notification != 0)
	{
	  Answer_notification->HandleArraySize  = compteur_notification;    
	  sendToFederate(Answer_notification,theFederateHandle);    
	}
      else
	delete Answer_notification;
    
      D.Out(pdDebug,
	     "Debut traitement : envoi message divestiture notification");

      if(compteur_divestiture != 0)
	{
	  diffusionDivestiture->size =compteur_divestiture;
	  sendToOwners(diffusionDivestiture,theObjectHandle,theFederateHandle,"\0",
			m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION);
	}
      delete diffusionDivestiture;
       
        
      if(compteur_unavailable != 0)
	{
	  Answer_unavailable->HandleArraySize  = compteur_unavailable;    
	  sendToFederate(Answer_unavailable,theFederateHandle);    
	}
      else
	delete Answer_unavailable;
    
    }
    else {
      D.Out(pdExcept, "AttributeOwnershipAcquisitionIfAvailable should not "
	     "be called on the RTIA.");
      throw RTIinternalError("AttributeOwnershipAcquisitionIfAvailable "
			      "called on the RTIA.");
    }


  }


  // --------------------------------------------------
  // -- . UnconditionalAttributeOwnershipDivestiture --
  // --------------------------------------------------

  ObjectClassBroadcastList *
  ObjectClass::unconditionalAttributeOwnershipDivestiture(
							  FederateHandle    theFederateHandle,
							  ObjectHandle          theObjectHandle,
							  AttributeHandle  *theAttributeList,
							  UShort            theListSize)
    throw(ObjectNotKnown,
	   AttributeNotDefined,
	   AttributeNotOwned,
	   RTIinternalError)
  {
    Object                *Object = NULL;
    int                    compteur_assumption = 0;
    int                    compteur_acquisition = 0;
    NetworkMessage         *AnswerAssumption = NULL; 
    ObjectClassBroadcastList *List   = NULL;
    FederateHandle     NewOwner;
    ObjectClassAttribute *classAttribute = NULL; 
    ObjectAttribute   *attribute = NULL;

    // Pre-conditions checking
 
    // may throw ObjectNotKnown
    Object = getInstanceWithID(theObjectHandle);

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for(int Index = 0; Index < theListSize; Index ++)
      getAttributeWithHandle(theAttributeList [Index]); 

    //Le fédéré est-il propriétaire de tous les attributs

    for(int i = 0; i < theListSize; i++) 
      for(int k = 1 ; k <= Object->AttributeState.getLength() ; k++)
	{
	  attribute = Object->AttributeState.Ieme(k);      
	  if(theAttributeList [i] == attribute->Handle )
	    {
	      if( attribute->getOwner() != theFederateHandle )
		throw AttributeNotOwned();
       
	    }
	}
   
     
    if(server != NULL) {
 
      AnswerAssumption                     = new NetworkMessage;
  
      AnswerAssumption->HandleArraySize  = theListSize;  

      CDiffusion *diffusionAcquisition = new CDiffusion(); 
 
      for(int i = 0; i < theListSize; i++) 
	for(int k = 1 ; k <= Object->AttributeState.getLength() ; k++)
	  {
	    classAttribute = AttributeSet.Ieme(k);     
	    attribute = Object->AttributeState.Ieme(k);      
	    if(theAttributeList [i] == attribute->Handle )
	      {
		if( attribute->OwnerCandidate.getLength() != 0 )
		  {
		    //Un attributeOwnershipAcquisition est en cours sur cet
		    //attribut
     
		    //Le demandeur le plus récent devient propriétaire       
		    NewOwner = attribute->getCandidate(1);

		    attribute->setOwner(NewOwner);
     
		    //On le supprime de la liste des demandeurs
		    attribute->removeCandidate(NewOwner);
     
		    //On réinitialise divesting
		    attribute->setDivesting(RTI_FALSE);

		    diffusionAcquisition->DiffArray[compteur_acquisition].NumeroFedere 
		      = NewOwner;
		    diffusionAcquisition->DiffArray[compteur_acquisition].AttribHandle 
		      = attribute->Handle;
		    compteur_acquisition++;
            
		    if( strcmp(classAttribute->getName(),"privilegeToDelete") == 0)
		      Object->Owner = NewOwner;
                      
         
		  }
		else
		  {
		    AnswerAssumption->HandleArray [compteur_assumption] 
		      = theAttributeList [i];
		    attribute->setOwner(0);
		    attribute->setDivesting(RTI_FALSE);
		    compteur_assumption++;

		  }           
	      }
	  }
      
      if(compteur_assumption != 0)
	{ 

	  AnswerAssumption->Type = m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION;
	  AnswerAssumption->NumeroFederation = server->federation();
	  AnswerAssumption->NumeroFedere     = theFederateHandle;
	  AnswerAssumption->Exception        = e_NO_EXCEPTION;

	  AnswerAssumption->Objectid         = theObjectHandle;
	  strcpy(AnswerAssumption->Label, "\0");
	  AnswerAssumption->HandleArraySize  = compteur_assumption;

	  List = new ObjectClassBroadcastList(AnswerAssumption, AttributeSet.getLength());

	  D.Out(pdProtocol, 
		 "Object %u updated in class %u, now broadcasting...", 
		 theObjectHandle, Handle);

	  broadcastClassMessage(List);
   
	}
      else
	delete AnswerAssumption;
     
      if(compteur_acquisition != 0)
	{
	  diffusionAcquisition->size =compteur_acquisition;
	  sendToOwners(diffusionAcquisition,theObjectHandle,theFederateHandle,"\0",
			m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION);
	}
      delete diffusionAcquisition; 
                
    }
    else {
      D.Out(pdExcept, "UnconditionalAttributeOwnershipDivestiture should not "
	     "be called on the RTIA.");
      throw RTIinternalError("UnconditionalAttributeOwnershipDivestiture "
			      "called on the RTIA.");
    }

    // Return the BroadcastList in case it had to be passed to the parent class.
    return List;

  }


  // -------------------------------------
  // -- . AttributeOwnershipAcquisition --
  // -------------------------------------

  void
  ObjectClass::attributeOwnershipAcquisition(
					     FederateHandle    theFederateHandle,
					     ObjectHandle          theObjectHandle,
					     AttributeHandle  *theAttributeList,
					     UShort            theListSize,
					     const char    *theTag)
    throw(
	   ObjectNotKnown,
	   ObjectClassNotPublished,
	   AttributeNotDefined,
	   AttributeNotPublished,
	   FederateOwnsAttributes,
	   RTIinternalError)
  {

    Object               *Object = NULL;
    NetworkMessage        *AnswerNotification = NULL;
    int          compteur_notification = 0;
    int          compteur_divestiture = 0;
    int          compteur_release = 0; 
    FederateHandle    OldOwner; 
    ObjectClassAttribute *classAttribute = NULL; 
    ObjectAttribute   *attribute = NULL;
 
    //Les vérifications des conditions doivent être faites avant 
    //toute modification!!!
 
    // Pre-conditions checking
 
    // may throw ObjectNotKnown
    Object = getInstanceWithID(theObjectHandle);

    for(int i = 0 ; i < theListSize ; i++)
      {
	// Do all attribute handles exist ? It may throw AttributeNotDefined.   
	getAttributeWithHandle(theAttributeList [i]);
      
	for(int k = 1 ; k <= Object->AttributeState.getLength() ; k++)
	  {
	    classAttribute = AttributeSet.Ieme(k);
	    attribute = Object->AttributeState.Ieme(k);
	    if(theAttributeList [i] == attribute->Handle )
	      {
		//Le fédéré est-il déjà propriétaire de certains attributs 
		if( attribute->getOwner() == theFederateHandle )
		  throw FederateOwnsAttributes();
		//Le fédéré publie-t-il les attributs
		if(!classAttribute->IsPublishing(theFederateHandle)
		    &&(strcmp(classAttribute->getName(),"privilegeToDelete") != 0))
		  throw AttributeNotPublished();            
	      }
	  }
      }
 
    if(server != NULL) {
 
      //Le fédéré doit publier la classe
      if(!isFederatePublisher(theFederateHandle)) {
	D.Out(pdExcept, "exception : ObjectClassNotPublished.");
	throw ObjectClassNotPublished();
      } 
 
      AnswerNotification = new NetworkMessage;
 
      AnswerNotification->Type = m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION;
      AnswerNotification->NumeroFederation = server->federation();
      AnswerNotification->NumeroFedere     = theFederateHandle;
      AnswerNotification->Exception        = e_NO_EXCEPTION;
      AnswerNotification->Objectid         = theObjectHandle;
  
      CDiffusion *diffusionDivestiture = new CDiffusion();

      CDiffusion *diffusionRelease = new CDiffusion();  
  
      for(int i = 0; i < theListSize; i++)
	{
	  for(int k=1;k<=Object->AttributeState.getLength();k++)
	    {
	      classAttribute = AttributeSet.Ieme(k);
	      attribute = Object->AttributeState.Ieme(k);     
	      if(theAttributeList [i] == attribute->Handle )
		{
		  OldOwner = attribute->getOwner();
		  if(( OldOwner == 0 )
		       ||( attribute->beingDivested()) )
		    {//Cet attribut est libre ou offert par son propriétaire
		      //S'il est offert      
		      if(attribute->beingDivested())
			{
			  diffusionDivestiture->DiffArray[compteur_divestiture].NumeroFedere 
			    = OldOwner;
			  diffusionDivestiture->DiffArray[compteur_divestiture].AttribHandle 
			    = attribute->Handle;
			  compteur_divestiture++;
			}           
		      //Qu'il soit offert ou libre
		      if( attribute->isCandidate(theFederateHandle) != 0)            
			attribute->removeCandidate(theFederateHandle);     
		      AnswerNotification->HandleArray [compteur_notification]  
			= theAttributeList [i];
		      attribute->setOwner(theFederateHandle);
		      attribute->setDivesting(RTI_FALSE);       
		      compteur_notification++;

		      //Object->Owner reste le champ de référence pour le privilegeToDelete
		      if( strcmp(classAttribute->getName(),"privilegeToDelete") == 0)
			Object->Owner = theFederateHandle;       
     
		    }
		  else
		    {
		      diffusionRelease->DiffArray[compteur_release].NumeroFedere = OldOwner;
		      diffusionRelease->DiffArray[compteur_release].AttribHandle 
			= attribute->Handle;
		      compteur_release++;
                
		      //On l'enlève de la liste des demandeurs s'il y était
		      attribute->removeCandidate(theFederateHandle);
     
		      //pour le rajouter en 1ere position
		      attribute->addCandidate(theFederateHandle);           
		    }            
		}
	    }
	}   
   
      if(compteur_notification != 0)
	{
	  AnswerNotification->HandleArraySize  = compteur_notification;    
	  sendToFederate(AnswerNotification,theFederateHandle);    
	}
      else
	delete AnswerNotification;  

      if(compteur_divestiture != 0)
	{
	  diffusionDivestiture->size =compteur_divestiture;
	  sendToOwners(diffusionDivestiture,theObjectHandle,theFederateHandle,"\0",
			m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION);
	}
      delete diffusionDivestiture;

      if(compteur_release != 0)
	{
	  diffusionRelease->size =compteur_release;
	  sendToOwners(diffusionRelease,theObjectHandle,theFederateHandle,theTag,
			m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE);
	}
      delete diffusionRelease;   
  
    }
    else {
      D.Out(pdExcept, 
	     "AttributeOwnershipAcquisition should not be called on the RTIA.");
      throw RTIinternalError("AttributeOwnershipAcquisition called on the RTIA");
    }



  }

  // -----------------------------------------------------
  // -- . CancelNegotiatedAttributeOwnershipDivestiture --
  // -----------------------------------------------------

  void
  ObjectClass::cancelNegotiatedAttributeOwnershipDivestiture(
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

    Object                *Object = NULL;
    ObjectAttribute   *attribute = NULL;
  
    // Pre-conditions checking
 
    // may throw ObjectNotKnown
    Object = getInstanceWithID(theObjectHandle);

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for(int Index = 0; Index < theListSize; Index ++)
      getAttributeWithHandle(theAttributeList [Index]); 

    for(int i = 0; i < theListSize; i++) 
      for(int k = 1 ; k <= Object->AttributeState.getLength() ; k++)
	{
	  attribute = Object->AttributeState.Ieme(k);       
	  if(theAttributeList [i] == attribute->Handle )
	    {
	      //Le fédéré est-il propriétaire de tous les attributs
	      if( attribute->getOwner() != theFederateHandle )
		throw AttributeNotOwned();
	      //Le fédéré a t-il fait appel à NegotiatedAttributeOwnershipDivestiture
	      if( !attribute->beingDivested())
		throw AttributeDivestitureWasNotRequested();
	    }
	}
   
     
    if(server != NULL) {
 
      for(int i = 0; i < theListSize; i++) 
	for(int k = 1 ; k <= Object->AttributeState.getLength() ; k++)
	  {
	    if(theAttributeList [i] == attribute->Handle )
	      attribute->setDivesting(RTI_FALSE);
	  }
    }
    else {
      D.Out(pdExcept, "CancelNegotiatedAttributeOwnershipDivestiture should "
	     "not be called on the RTIA.");
      throw RTIinternalError("CancelNegotiatedAttributeOwnershipDivestiture "
			      "called on the RTIA.");
    }
  }

  // ------------------------------------------
  // -- . AttributeOwnershipRealeaseResponse --
  // ------------------------------------------

  AttributeHandleSet *
  ObjectClass::attributeOwnershipRealeaseResponse(
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
    Object                *Object = NULL;
    int                    compteur_acquisition = 0;
    FederateHandle     NewOwner;
    ObjectClassAttribute *classAttribute = NULL; 
    ObjectAttribute   *attribute = NULL; 
    AttributeHandleSet   *theAttribute;

 
    // Pre-conditions checking
 
    // may throw ObjectNotKnown
    Object = getInstanceWithID(theObjectHandle);

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for(int Index = 0; Index < theListSize; Index ++)
      getAttributeWithHandle(theAttributeList [Index]); 

    //Le fédéré est-il propriétaire de tous les attributs
    //Y a-t-il des acquéreurs pour les attributs

    for(int i = 0; i < theListSize; i++) 
      for(int k=1;k<=Object->AttributeState.getLength();k++)
	{
	  classAttribute = AttributeSet.Ieme(k);
	  attribute = Object->AttributeState.Ieme(k);
	  if(theAttributeList [i] == attribute->Handle )
	    {
	      if( attribute->getOwner() != theFederateHandle )
		throw AttributeNotOwned();
	      if( attribute->OwnerCandidate.getLength() == 0 )           
		throw FederateWasNotAskedToReleaseAttribute();
 
	    }
	}
   
     
    if(server != NULL) {
 
      CDiffusion *diffusionAcquisition = new CDiffusion();
  
      theAttribute = AttributeHandleSetFactory::create(theListSize); 
     
      for(int i = 0; i < theListSize; i++) 
	for(int k=1;k<=Object->AttributeState.getLength();k++)
	  {
	    classAttribute = AttributeSet.Ieme(k);
	    attribute = Object->AttributeState.Ieme(k);     
	    if(theAttributeList [i] == attribute->Handle )
	      {
            
		//Le demandeur le plus récent devient propriétaire       
		NewOwner = attribute->getCandidate(1);

		attribute->setOwner(NewOwner);
     
		//On le supprime de la liste des demandeurs
		attribute->removeCandidate(NewOwner);
     
		//On réinitialise divesting
		attribute->setDivesting(RTI_FALSE);

		diffusionAcquisition->DiffArray[compteur_acquisition].NumeroFedere 
		  = NewOwner;
		diffusionAcquisition->DiffArray[compteur_acquisition].AttribHandle 
		  = attribute->Handle;
		compteur_acquisition++;
		theAttribute->add(attribute->Handle);
   
		D.Out(pdDebug, "Acquisition handle %u compteur %u",
		       theAttributeList [i],compteur_acquisition);
            
		if( strcmp(classAttribute->getName(),"privilegeToDelete") == 0)
		  Object->Owner = NewOwner;            
         
	      }
	  }
   
      if(compteur_acquisition != 0)
	{
	  diffusionAcquisition->size =compteur_acquisition;
	  sendToOwners(diffusionAcquisition,theObjectHandle,theFederateHandle,"\0",
			m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION);
	}
      delete diffusionAcquisition; 

    }
    else {
      D.Out(pdExcept, "NegotiatedAttributeOwnershipDivestiture should not be "
	     "called on the RTIA.");
      throw RTIinternalError("NegotiatedAttributeOwnershipDivestiture called "
			      "on the RTIA.");
    }
 
    return(theAttribute);

  }

  // -------------------------------------------
  // -- . CancelAttributeOwnershipAcquisition --
  // -------------------------------------------

  void
  ObjectClass::
  cancelAttributeOwnershipAcquisition(FederateHandle theFederateHandle,
				      ObjectHandle theObjectHandle,
				      AttributeHandle *theAttributeList,
				      UShort theListSize)
    throw(ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeAlreadyOwned,
	  AttributeAcquisitionWasNotRequested,
	  RTIinternalError)
  {
    
    Object *Object = NULL;
    NetworkMessage         *Answer_confirmation = NULL;
    int          compteur_confirmation = 0;
    ObjectClassAttribute *classAttribute = NULL; 
    ObjectAttribute   *attribute = NULL;
  
    // Pre-conditions checking
 
    //It may throw ObjectNotKnown.
    Object = getInstanceWithID(theObjectHandle);

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for(int Index = 0; Index < theListSize; Index ++)
      getAttributeWithHandle(theAttributeList [Index]);

    for(int i = 0; i < theListSize; i++) 
      D.Out(pdDebug, "CancelAcquisition Object %u Attribut %u ",
	     theObjectHandle,theAttributeList [i]);   

    if(server != NULL) {
 
      //rem AttributeSet.getLength()=AttributeState.getLength()
      int i;
      for(i = 0 ; i < theListSize ; i++)
	{
	  for(int k = 1 ; k <= AttributeSet.getLength() ; k++)
	    {
	      classAttribute = AttributeSet.Ieme(k);
	      attribute = Object->AttributeState.Ieme(k);     
	      if(theAttributeList [i] == AttributeSet.Ieme(k)->Handle )
		{
		  D.Out(pdDebug, "Attribut %u Owner %u",theAttributeList [i],
			 attribute->getOwner());      
		  //Le fédéré n'est-il pas déjà propriétaire de certains attributs     
		  if( attribute->getOwner() == theFederateHandle )
		    throw AttributeAlreadyOwned();
		  //Le fédéré n'a t-il pas déja une procédure d'acquisition en cours     
		  if( attribute->isCandidate(theFederateHandle) == 0)
		    throw AttributeAcquisitionWasNotRequested();
		}
	    }
	}      


      Answer_confirmation = new NetworkMessage;     
      Answer_confirmation->Type 
	= m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION;
      Answer_confirmation->NumeroFederation = server->federation();
      Answer_confirmation->NumeroFedere     = theFederateHandle;
      Answer_confirmation->Exception        = e_NO_EXCEPTION;
      Answer_confirmation->Objectid         = theObjectHandle; 


      for(i = 0; i < theListSize; i++)
	{
	  for(int k=1;k<=Object->AttributeState.getLength();k++)
	    {
	      classAttribute = AttributeSet.Ieme(k);     
	      attribute = Object->AttributeState.Ieme(k);       
	      if(theAttributeList [i] == attribute->Handle )
		{
     
		  Answer_confirmation->HandleArray [compteur_confirmation]  
		    = theAttributeList [i];
     
		  //On l'enlève de la liste des demandeurs 
		  attribute->removeCandidate(theFederateHandle);
		  D.Out(pdDebug, "ajout de %u  à attribut %u objet %u",
			 theFederateHandle,theAttributeList[i],theObjectHandle);             
		  compteur_confirmation++;           
		}                        
	    }
	}
     
      if(compteur_confirmation != 0)
	{
	  Answer_confirmation->HandleArraySize  = compteur_confirmation;    
	  sendToFederate(Answer_confirmation,theFederateHandle);    
	}
      else
	delete Answer_confirmation;
   
    }
    else {
      D.Out(pdExcept, 
	     "CancelAttributeOwnershipAcquisition should not be called on the RTIA.");
      throw RTIinternalError("CancelAttributeOwnershipAcquisition called on the RTIA.");
    }
  }

}

// $Id: ObjectClass.cc,v 3.2 2002/11/30 22:13:32 breholee Exp $
