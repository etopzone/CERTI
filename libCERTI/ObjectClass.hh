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
// $Id: ObjectClass.hh,v 3.3 2003/01/17 18:00:21 breholee Exp $
// ---------------------------------------------------------------------------

// Class ObjectClass, qui decrit une classe d'objet. Elle contient
// aussi la liste des instances de cette classe.

#ifndef _CERTI_OBJECT_CLASS_HH
#define _CERTI_OBJECT_CLASS_HH

#include <stdio.h>
#include "RTItypes.hh"
#include "ObjectClassAttribute.hh"
#include "Object.hh"
#include "ObjectAttribute.hh"
#include "SocketTCP.hh"
#include "List.hh"
#include "SecurityServer.hh"
#include "ObjectClassBroadcastList.hh"
#include "PrettyDebug.hh"

// We need to define a class because List only allows pointers, and
// not integer for example.

namespace certi {

class ObjectClassChild {
public:
  ObjectClassHandle Handle;

  ObjectClassChild(ObjectClassHandle theHandle) { Handle = theHandle; };
};

class CDiffusion {

public:

  typedef struct {
    FederateHandle NumeroFedere;
    AttributeHandle AttribHandle;

  } DiffStruct;

  DiffStruct DiffArray [MAX_ATTRIBUTES_PER_CLASS];
 
  int size;

  CDiffusion() { size = 0; };
};


class ObjectClass {

public:

  // -----------------------
  // -- Public Attributes --(cf. Note)
  // -----------------------

  // Note: Most of the folling Attributes should be private. In fact,
  // they can not be reached from the RTIG and RTIA components, because
  // they are hidden from them by ObjectClassSet. 

  // They are kept public for convenience in the CRead class that build
  // the Object Classes tree and set all Father/Son dependences and
  // attributes.

  ObjectClassHandle Handle;
  // numero de la classe de l'objet 

  // This Object help to find a TCPLink from a Federate Handle.
  SecurityServer *server;

  ObjectClassHandle Father;
  // numero du pere de l'objet

  UShort Depth;
  // Profondeur dans l'arborescence des classes. Utilise uniquement
  // par CRead.

  List <ObjectClassChild *> SonSet;
  // ensemble des classes filles de cette classe d'objet

  // ----------------------------
  // -- Initialization Methods --
  // ----------------------------
 
  // Constructor & Destructor
  ObjectClass();
  ~ObjectClass();

  // Print the ObjectClasses tree to the standard output.
  void display(void);

  // Name attribute access(GetName reference must be considered READ-ONLY).
  // NewName lenght must be lower or equal to MAX_USER_TAG_LENGTH.
  char *getName() {return Name; };

  void setName(char *NewName)
    throw(ValueLengthExceeded, RTIinternalError);

  // ----------------------
  // -- Security Methods --
  // ----------------------

  // Throw SecurityError if the Federate is not allowed to access the 
  // Object Class, and print an Audit message containing Reason.
  void checkFederateAccess(FederateHandle theFederate,
			   const char *Reason)
    throw(SecurityError);

  SecurityLevelID getLevelId(void)
  { return LevelID; };

  // A class' LevelID can only be increased.
  void setLevelId(SecurityLevelID NewLevelID);

  // -------------------------------------------------------------
  // -- CRead Methods(used when building the Root Object Tree) --
  // -------------------------------------------------------------

  // To be used only by CRead. It returns the new Attribute's Handle.
  AttributeHandle addAttribute(ObjectClassAttribute *theAttribute,
			       Boolean Inherited = RTI_FALSE);

  // Add the class' attributes to the 'Child' Class.
  void addAttributesToChild(ObjectClass *Child);

  // ----------------------------------
  // -- Publication and Subscription --
  // ----------------------------------

  void publish(FederateHandle theFederateHandle,
	       AttributeHandle *theAttributeList,
	       UShort theListSize,
	       bool PubOrUnpub)
    throw(AttributeNotDefined,
	  RTIinternalError,
	  SecurityError);

  // Return RTI_TRUE if theFederate had never subscribed to this class
  // before. In that case, ObjectClassSet will call SendDiscoverMessages
  // on this class and on all child classes to allow them to send
  // Discover Messages for already registered instances.

  Boolean subscribe(FederateHandle theFederate,
		    AttributeHandle *theAttributeList,
		    UShort theListSize,
		    bool SubOrUnsub)
    throw(AttributeNotDefined,
	  RTIinternalError,
	  SecurityError);
 
  // The second parameter is the Class of whose behalf the message are sent.
  // If SDM is called on the original class, the Federate may be a subscriber
  // of the class without stopping the process(because he has just subscribed)
  //
  // Return RTI_TRUE if the same SendDiscoverMessages method must be called
  // on the child classes of this class.
  // Return RTI_FALSE if no messages were sent because the Federate had
  // already receive DO messages for this class(and all child classes).

  Boolean sendDiscoverMessages(FederateHandle theFederate,
			       ObjectClassHandle theOriginalClass);
 
 
 
  // --------------------------
  // -- Ownership Management --
  // --------------------------
 
  Boolean 
  isAttributeOwnedByFederate(ObjectHandle theObject,
			     AttributeHandle theAttribute,
			     FederateHandle theFederateHandle)
    throw(ObjectNotKnown,
	  AttributeNotDefined,
	  RTIinternalError);

 
  void 
  queryAttributeOwnership(ObjectHandle theObject,
			  AttributeHandle theAttribute,
			  FederateHandle theFederateHandle)
    throw(ObjectNotKnown,
	  AttributeNotDefined,
	  RTIinternalError);
 
 
  ObjectClassBroadcastList * 
  negotiatedAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
					  ObjectHandle theObjectHandle,
					  AttributeHandle *theAttributeList,
					  UShort theListSize,
					  const char *theTag)
    throw(ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeNotOwned,
	  AttributeAlreadyBeingDivested,
	  RTIinternalError);

 
  void 
  attributeOwnershipAcquisitionIfAvailable(FederateHandle theFederateHandle,
					   ObjectHandle theObjectHandle,
					   AttributeHandle *theAttributeList,
					   UShort theListSize)
    throw(ObjectNotKnown,
	  ObjectClassNotPublished,
	  AttributeNotDefined,
	  AttributeNotPublished,
	  FederateOwnsAttributes,
	  AttributeAlreadyBeingAcquired,
	  RTIinternalError);
 
  ObjectClassBroadcastList * 
  unconditionalAttributeOwnershipDivestiture(FederateHandle,
					     ObjectHandle theObjectHandle,
					     AttributeHandle*,
					     UShort theListSize)
    throw(ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeNotOwned,
	  RTIinternalError);
 
 
  void 
  attributeOwnershipAcquisition(FederateHandle theFederateHandle,
				ObjectHandle theObjectHandle,
				AttributeHandle *theAttributeList,
				UShort theListSize,
				const char *theTag)
    throw( ObjectNotKnown,
	   ObjectClassNotPublished,
	   AttributeNotDefined,
	   AttributeNotPublished,
	   FederateOwnsAttributes,
	   RTIinternalError);
 
 
  void 
  cancelNegotiatedAttributeOwnershipDivestiture(FederateHandle,
						ObjectHandle theObjectHandle,
						AttributeHandle*,
						UShort theListSize)
    throw( ObjectNotKnown,
	   AttributeNotDefined,
	   AttributeNotOwned,
	   AttributeDivestitureWasNotRequested,
	   RTIinternalError);
 
  AttributeHandleSet *
  attributeOwnershipRealeaseResponse(FederateHandle theFederateHandle,
				     ObjectHandle theObjectHandle,
				     AttributeHandle *theAttributeList,
				     UShort theListSize)
    throw( ObjectNotKnown,
	   AttributeNotDefined,
	   AttributeNotOwned,
	   FederateWasNotAskedToReleaseAttribute,
	   RTIinternalError); 
 
  void cancelAttributeOwnershipAcquisition(FederateHandle theFederateHandle,
					   ObjectHandle theObjectHandle,
					   AttributeHandle *theAttributeList,
					   UShort theListSize)
 
    throw(
	  ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeAlreadyOwned,
	  AttributeAcquisitionWasNotRequested,
	  RTIinternalError);
 
 
  // --------------------------
  // -- RTI Support Services --
  // --------------------------

 
  AttributeHandle getAttributeHandle(const char* theName)
      throw(AttributeNotDefined,
            RTIinternalError);
 
  const char* getAttributeName(AttributeHandle theHandle)
      throw(AttributeNotDefined,
            RTIinternalError);

  ObjectClassBroadcastList *killFederate(FederateHandle theFederate)
    throw();

  // -------------------------
  // -- Instance Management --
  // -------------------------

  // Delete the object instance 'theObjectHandle', and starts to
  // broadcast the RemoveObject message. Return a BroadcastList of
  // Federates, in order to allow our ObjectClassSet to go on with
  // the message broadcasting, by giving the list to our parent
  // class.
  ObjectClassBroadcastList *deleteInstance(FederateHandle theFederateHandle,
					   ObjectHandle theObjectHandle,
					   const char*  theUserTag)
    throw(DeletePrivilegeNotHeld,
	  ObjectNotKnown,
	  RTIinternalError);

  // Return RTI_TRUE if the object instance designated by 'theID' is
  // present in that class, else return RTI_FALSE.
  Boolean isInstanceInClass(ObjectHandle theID);
 
 ObjectClassBroadcastList *registerInstance(FederateHandle,
                                             ObjectHandle theObjectHandle,
                                             const char*  theObjectName)
      throw(ObjectClassNotPublished,
            ObjectAlreadyRegistered,
            RTIinternalError);

  // Take a Broadcast List and continue to send messages. You should
  // take a look at ObjectClassSet::RegisterObject to understand
  // what is going on...
  void broadcastClassMessage(ObjectClassBroadcastList *List);

  ObjectClassBroadcastList *
  updateAttributeValues(FederateHandle theFederateHandle,
			ObjectHandle theObjectHandle,
			AttributeHandle *theAttributeArray,
			AttributeValue *theValueArray,
			UShort theArraySize,
			FederationTime theTime,
			const char*  theUserTag)
    throw(ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeNotOwned,
	  RTIinternalError,
	  InvalidObjectHandle);

private:
 
  // ------------------------
  // -- Private Attributes --
  // ------------------------

  // Should be allocated and deleted locally.
  // nom de la classe de l'objet
  ObjectClassName Name;

  // All non-inherited attributes have this default level.
  SecurityLevelID LevelID;

  // This Handle is the greatest handle of the class' subscribers.
  FederateHandle MaxSubscriberHandle;

  List <ObjectClassAttribute *> AttributeSet;
  // liste des attributs de la classe d'objet

  List <Object *> ObjectSet;
  // ensemble des intances des objets de cette classe

  // ---------------------
  // -- Private Methods --
  // ---------------------

  void sendToFederate(NetworkMessage *msg,
		      FederateHandle theFederate);
 
  void sendToOwners(CDiffusion *diffusionList,
		    ObjectHandle theObjectHandle,
		    FederateHandle theFederate,
		    const char *theTag, 
		    TypeNetworkMessage type); 

  void sendMessage(NetworkMessage *msg,
		   FederateHandle theDest);

  Object *getInstanceWithID(ObjectHandle theID)
    throw(ObjectNotKnown);

  ObjectClassAttribute *getAttributeWithHandle(AttributeHandle theHandle)
    throw(AttributeNotDefined);

  // --------------------------------
  // -- Publication / Subscription --
  // --------------------------------

  // Return RTI_TRUE if the Federate is publishing any attribute of
  // this class.
  Boolean isFederatePublisher(FederateHandle theFederate);

  // Return RTI_TRUE if the Federate has subscribe to any attribute.
  Boolean isFederateSubscriber(FederateHandle theFederate);
};
}

#endif // _CERTI_OBJECT_CLASS_HH

// $Id: ObjectClass.hh,v 3.3 2003/01/17 18:00:21 breholee Exp $
