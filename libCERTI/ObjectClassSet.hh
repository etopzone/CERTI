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
// $Id: ObjectClassSet.hh,v 3.1 2002/11/26 15:48:01 breholee Exp $
// ---------------------------------------------------------------------------

// $Id: ObjectClassSet.hh,v 3.1 2002/11/26 15:48:01 breholee Exp $
// ---------------------------------------------------------------------------
// Class ObjectClassSet, qui est la racine de l'arborescence des
// classes d'objets.

#ifndef _CERTI_OBJECT_CLASS_SET_HH
#define _CERTI_OBJECT_CLASS_SET_HH

#include <stdio.h>

#include "List.hh"
#include "ObjectClass.hh"
#include "SecurityServer.hh"
#include "ObjectClassBroadcastList.hh"
#include "PrettyDebug.hh"

namespace certi {

class ObjectClassSet : private List <ObjectClass *> 
{

public:

  // --------------------
  // -- Public Methods --
  // --------------------

  ObjectClassSet(SecurityServer *theSecurityServer);

  ~ObjectClassSet();


  // The class is not allocated, only the pointer is memorized.
  void addClass(ObjectClass *theClass);

  // Build a Parent-Child relation between two object class, by setting
  // the Child's Parent handle, and registering the Child in the Parent's
  // SonSet. 
  // Also copy all Parent's Attributes in the Child Class.
  void buildParentRelation(ObjectClass *Child, ObjectClass *Parent);

  // Print the ObjectClasses tree to the standard output.
  void display(void);

  // --------------------------
  // -- RTI Support Services --
  // --------------------------

  AttributeHandle getAttributeHandle(const AttributeName theName,
				     ObjectClassHandle theClass)
    throw(AttributeNotDefined,
	  ObjectClassNotDefined,
	  RTIinternalError);

  const AttributeName getAttributeName(AttributeHandle theHandle,
				       ObjectClassHandle theClass)
    throw(AttributeNotDefined,
	  ObjectClassNotDefined,
	  RTIinternalError);

  ObjectClassHandle getObjectClassHandle(const ObjectClassName theName)
    throw(ObjectClassNotDefined,
	  RTIinternalError);

  const ObjectClassName getObjectClassName(ObjectClassHandle theHandle)
    throw(ObjectClassNotDefined,
	  RTIinternalError);
 
  void killFederate(FederateHandle theFederate)
    throw();


  // -----------------------------
  // -- Object Class Management --
  // -----------------------------

  void publish(FederateHandle theFederateHandle,
	       ObjectClassHandle theClassHandle,
	       AttributeHandle *theAttributeList,
	       UShort theListSize,
	       bool PubOrUnpub)
    throw(ObjectClassNotDefined,
	  AttributeNotDefined,
	  RTIinternalError,
	  SecurityError);

  void subscribe(FederateHandle theFederateHandle,
		 ObjectClassHandle theClassHandle,
		 AttributeHandle *theAttributeList,
		 UShort theListSize,
		 bool SubOrUnsub)
    throw(ObjectClassNotDefined,
	  AttributeNotDefined,
	  RTIinternalError,
	  SecurityError);

  // See Subscribe
  void recursiveDiscovering(ObjectClassHandle theClassHandle,
			    FederateHandle theFederate,
			    ObjectClassHandle theOriginalClass);

  void changeAttributeTransportType(ObjectHandle theObjectHandle,
				    AttributeHandle *attribArray,
				    UShort attribArraySize,
				    TransportType theType)
    throw(ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeNotOwned,
	  RTIinternalError,
	  InvalidObjectHandle);

  void changeAttributeOrdertType(ObjectHandle theObjectHandle,
				 AttributeHandle *attribArray,
				 UShort attribArraySize,
				 TransportType theType)
    throw(ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeNotOwned,
	  RTIinternalError,
	  InvalidObjectHandle);

  // --------------------------------
  // -- Object Instance Management --
  // --------------------------------

  void deleteObject(FederateHandle theFederateHandle,
		    ObjectHandle theObjectHandle,
		    const char*  theTag)
    throw(DeletePrivilegeNotHeld,
	  ObjectNotKnown,
	  RTIinternalError);
 
  void registerInstance(FederateHandle theFederateHandle,
			ObjectClassHandle theClassHandle,
			ObjectHandle theObjectHandle,
			ObjectName theObjectName)
    throw(InvalidObjectHandle,
	  ObjectClassNotDefined,
	  ObjectClassNotPublished,
	  ObjectAlreadyRegistered,
	  RTIinternalError);

  void updateAttributeValues(FederateHandle theFederateHandle,
			     ObjectHandle theObjectHandle,
			     AttributeHandle *theAttribArray,
			     AttributeValue *theValueArray,
			     UShort theArraySize,
			     FederationTime theTime,
			     const char*  theUserTag)
    throw(ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeNotOwned,
	  RTIinternalError,
	  InvalidObjectHandle);
 
 
  // --------------------------
  // -- Ownership Management --
  // --------------------------
 
 

  Boolean isAttributeOwnedByFederate(ObjectHandle theObject,
				     AttributeHandle theAttribute,
				     FederateHandle theFederateHandle)
    throw(
	  ObjectNotKnown,
	  AttributeNotDefined,
	  RTIinternalError);
 
  void queryAttributeOwnership(ObjectHandle theObject,
			       AttributeHandle theAttribute,
			       FederateHandle theFederateHandle)
    throw(
	  ObjectNotKnown,
	  AttributeNotDefined,
	  RTIinternalError);
 
  void 
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
 
 
  void attributeOwnershipAcquisitionIfAvailable(FederateHandle,
						ObjectHandle theObjectHandle,
						AttributeHandle*,
						UShort theListSize)
    throw(ObjectNotKnown,
	  ObjectClassNotPublished,
	  AttributeNotDefined,
	  AttributeNotPublished,
	  FederateOwnsAttributes,
	  AttributeAlreadyBeingAcquired,
	  RTIinternalError); 
 
 
  void 
  unconditionalAttributeOwnershipDivestiture(FederateHandle,
					     ObjectHandle theObjectHandle,
					     AttributeHandle*,
					     UShort theListSize)
    throw(
	  ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeNotOwned,
	  RTIinternalError); 
 
 
  void attributeOwnershipAcquisition(FederateHandle theFederateHandle,
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
 
  AttributeHandleSet 
  *attributeOwnershipRealeaseResponse( FederateHandle theFederateHandle,
				       ObjectHandle theObjectHandle,
				       AttributeHandle *theAttributeList,
				       UShort theListSize)
 
    throw( ObjectNotKnown,
	   AttributeNotDefined,
	   AttributeNotOwned,
	   FederateWasNotAskedToReleaseAttribute,
	   RTIinternalError); 
 
  void cancelAttributeOwnershipAcquisition( FederateHandle theFederateHandle,
					    ObjectHandle theObjectHandle,
					    AttributeHandle *theAttributeList,
					    UShort theListSize)
 
    throw(ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeAlreadyOwned,
	  AttributeAcquisitionWasNotRequested,
	  RTIinternalError); 
 
private:

  // ------------------------
  // -- Private Attributes --
  // ------------------------

  // This object will help to find the TCPLink associated with a Federate.
  // This reference is passed to all new ObjectClass.
  SecurityServer *server;

  // ---------------------
  // -- Private Methods --
  // ---------------------

  ObjectClass *getWithHandle(ObjectClassHandle theHandle)
    throw(ObjectClassNotDefined);

  ObjectClass *getInstanceClass(ObjectHandle theObjectHandle)
    throw(ObjectNotKnown);

};

}

#endif // _CERTI_OBJECT_CLASS_SET_HH

// $Id: ObjectClassSet.hh,v 3.1 2002/11/26 15:48:01 breholee Exp $
