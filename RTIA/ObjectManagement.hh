// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI
//
// CERTI is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// $Id: ObjectManagement.hh,v 3.1 2002/11/26 15:48:01 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef GO_HH
#define GO_HH

#include <config.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#include "FederationManagement.hh"
#include "Communications.hh"
#include "Files.hh"
#include "baseTypes.hh"
#include "RTItypes.hh"
#include "RootObject.hh"

#include "PrettyDebug.hh"

#define IDRESERVED 4294000000 
#define NOTLINKED -1

namespace certi {
namespace rtia {

// Prototypes de classes existantes
class Communications;
class Queues;
class FederationManagement;


// Definition de la classe principale de gestion des objets.
class ObjectManagement
{

public:

  // ------------------------------
  // -- Constructor & Destructor --
  // ------------------------------

  ObjectManagement(Communications *GC,
		   Queues *,
		   FederationManagement *GF,
		   RootObject *theRootObj);
 
  ~ObjectManagement();
 

  // -----------------------
  // -- Object Management --
  // -----------------------

  // 4.1
  void requestID(ObjectHandlecount idCount, 
		 ObjectHandle &firstID, 
		 ObjectHandle &lastID,
		 TypeException &e);
 
  // 4.2
  ObjectHandle registerObject(ObjectClassHandle theClassHandle,
			  const char *theObjectName,
			  FederationTime date,
			  FederationTime heure,
			  TypeException &e);
 
  // 4.3
  EventRetractionHandle 
  updateAttributeValues(ObjectHandle theObjectHandle, 
			AttributeHandle *attribArray,
			AttributeValue *valueArray,
			UShort attribArraySize,
			FederationTime theTime, 
			const char*  theTag,
			TypeException &e);
 
  // 4.4
  void discoverObject(ObjectHandle theObjectHandle, 
		      ObjectClassHandle theObjectClassHandle,
		      const char *theObjectName,
		      FederationTime theTime,
		      EventRetractionHandle theHandle,
		      TypeException &e);
 
  // 4.5
  void reflectAttributeValues(ObjectHandle theObjectHandle, 
			      AttributeHandle *attribArray,
			      AttributeValue *valueArray,
			      UShort attribArraySize,
			      FederationTime theTime, 
			      const char*  theTag, 
			      EventRetractionHandle theHandle,
			      TypeException &e);
 
  // 4.6
  EventRetractionHandle 
  sendInteraction(InteractionClassHandle theInteraction,
		  ParameterHandle *paramArray,
		  ParameterValue *valueArray,
		  UShort paramArraySize,
		  FederationTime theTime, 
		  const char*  theTag,
		  TypeException &e);

  // 4.7
  void receiveInteraction(InteractionClassHandle theInteraction, 
			  ParameterHandle *paramArray,
			  ParameterValue *valueArray,
			  UShort paramArraySize,
			  FederationTime theTime, 
			  const char*  theTag, 
			  EventRetractionHandle theHandle,
			  TypeException &e);
 
  // 4.8
  EventRetractionHandle deleteObject(ObjectHandle theObjectHandle,
				     const char*  theTag,
				     TypeException &e);

  // 4.9(1)
  void removeObject(ObjectHandle theObjectHandle,
		    FederateHandle theFederateHandle,
		    const char*  theTag, 
		    EventRetractionHandle theHandle,
		    TypeException &e);

  // 4.9(2)
  void removeObject(ObjectHandle theObject, 
		    ObjectRemovalReason theReason, 
		    TypeException &e);
 
  // 4.10 Change Attribute Transport
  EventRetractionHandle 
  changeAttributeTransportType(ObjectHandle theObjectHandle, 
			       AttributeHandle *attribArray,
			       UShort attribArraySize,
			       TransportType theType,
			       TypeException &e);
 
  // 4.11 Change Attribute Order
  EventRetractionHandle 
  changeAttributeOrderType(ObjectHandle theObjectHandle, 
			   AttributeHandle *attribArray,
			   UShort attribArraySize,
			   OrderType theType,
			   TypeException &e);
 
  // 4.12 Change Parameter Transport
  EventRetractionHandle 
  changeInteractionTransportType(InteractionClassHandle theClassID, 
				 TransportType theType,
				 TypeException &e);
 
  // 4.13 Change Parameter Order
  EventRetractionHandle
  changeInteractionOrderType(InteractionClassHandle theClassID, 
			     OrderType theType,
			     TypeException &e);

  // 4.14(1) Instance
  EventRetractionHandle 
  requestObjectAttributeValueUpdate(ObjectHandle theObjectHandle,
				    AttributeHandle *attribArray,
				    UShort attribArraySize,
				    TypeException &e);

//   // 4.14(2) Class
//   EventRetractionHandle 
//   requestObjectAttributeValueUpdate(ObjectClassHandle theObjectClassID,
// 				    AttributeHandle *attribArray,
// 				    UShort attribArraySize,
// 				    TypeException &e);

  // 4.15
  void provideAttributeValueUpdate(ObjectHandle theObject,
				   AttributeValue &theAttributes,
				   TypeException &e);
 
  // 4.16
  void retract(EventRetractionHandle theHandle,
	       TypeException &e);
 
  // 4.17
  void reflectRetraction(EventRetractionHandle theHandle,
			 TypeException &e);

  // --------------------------
  // -- RTI Support Services --
  // --------------------------

  // 8.1 
  ObjectClassHandle getObjectClassHandle(const ObjectClassName theName);
 
  // 8.2 
  ObjectClassName getObjectClassName(ObjectClassHandle theHandle);
 
  // 8.3 
  AttributeHandle getAttributeHandle(const AttributeName theName, 
				     ObjectClassHandle theClassHandle);
 
  // 8.4 
  AttributeName getAttributeName(AttributeHandle theHandle, 
				 ObjectClassHandle theClassHandle);
 
  // 8.5 
  InteractionClassHandle 
  getInteractionClassHandle(const InteractionClassName theName);
 
  // 8.6 
  InteractionClassName 
  getInteractionClassName(InteractionClassHandle theClassHandle);
 
  // 8.7 
  ParameterHandle 
  getParameterHandle(const ParameterName theParameterName, 
		     InteractionClassHandle theClassHandle);
 
  // 8.8 
  ParameterName 
  getParameterName(ParameterHandle theParameterHandle, 
		   InteractionClassHandle theClassHandle);

protected:

  // ------------------------
  // -- Private Attributes --
  // ------------------------

  Communications *_GC;
  Queues *_GQueues;
  FederationManagement *_GF;
  RootObject *_theRootObj;
};

}
}

#endif

// $Id: ObjectManagement.hh,v 3.1 2002/11/26 15:48:01 breholee Exp $
