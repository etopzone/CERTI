// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: ObjectManagement.cc,v 3.3 2003/01/17 18:19:47 breholee Exp $
// ---------------------------------------------------------------------------

#include "ObjectManagement.hh"

namespace certi {
namespace rtia {

static pdCDebug D("RTIA_GO", "(RTIA GO ) - ");
// ------------------
// -- Constructeur --
// ------------------

ObjectManagement::ObjectManagement(Communications *GC,
				   Queues *GQueues,
				   FederationManagement *GF,
				   RootObject *theRootObj)
{
  _GC = GC;
  _GQueues = GQueues;
  _GF = GF;
  _theRootObj = theRootObj;
}

// -----------------
// -- Destructeur --
// -----------------

ObjectManagement::~ObjectManagement()
{
}


// -------------------
// -- 4.1 requestID --
// -------------------

void 
ObjectManagement::requestID(ObjectHandlecount idCount, 
			    ObjectHandle &firstID, 
			    ObjectHandle &lastID,
			    TypeException &e)
{
  NetworkMessage req,rep;
 
  req.Type = m_REQUEST_ID;
  req.IDcount = idCount;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;

  _GC->sendMessage(&req);

  _GC->waitMessage(&rep, m_REQUEST_ID, req.NumeroFedere);
 
  e = rep.Exception;
  firstID = rep.FirstID;
  lastID = rep.LastID;
}


// ------------------------
// -- 4.2 registerObject --
// ------------------------

ObjectHandle
ObjectManagement::registerObject(ObjectClassHandle theClassHandle,
				 const char *theObjectName, 
				 FederationTime,
				 FederationTime,
				 TypeException & e)
{
  NetworkMessage req,rep;

  req.Type = m_REGISTER_OBJECT;
  req.NumeroFedere = _GF->federate;
  req.NumeroFederation = _GF->_numero_federation;
  req.objectClassHandle = theClassHandle;
  strcpy(req.Label,(char*)theObjectName);
 
  // BUG: A quoi servent Date et Heure ?

  _GC->sendMessage(&req);

  _GC->waitMessage(&rep, m_REGISTER_OBJECT, req.NumeroFedere);

  e = rep.Exception;

  if(e == e_NO_EXCEPTION) {
    _theRootObj->ObjectClasses->registerInstance(_GF->federate,
						 theClassHandle,
						 rep.Objectid,
						 rep.Label);

    // La reponse contient le numero d'objet(Objectid)
    return rep.Objectid; 
  } 
  else
    return 0;
}
 

// -------------------------------
// -- 4.3 updateAttributeValues --
// -------------------------------

EventRetractionHandle
ObjectManagement::updateAttributeValues(ObjectHandle theObjectHandle, 
					AttributeHandle *attribArray,
					AttributeValue *valueArray,
					UShort attribArraySize,
					FederationTime theTime, 
					const char*  theTag,
					TypeException &e)
{
  NetworkMessage req, rep;
  int i;

  // Mise en place de la requete
  req.Type = m_UPDATE_ATTRIBUTE_VALUES;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
  req.Objectid = theObjectHandle;
  req.Date = theTime;

  req.HandleArraySize = attribArraySize;

  for(i = 0;i < attribArraySize;i++) {
    req.HandleArray [i] = attribArray [i];
    req.setValue(i, valueArray [i]);
  }

  strcpy(req.Label, theTag);

  // Emission et reception de la reponse

  _GC->sendMessage(&req);

  _GC->waitMessage(&rep, m_UPDATE_ATTRIBUTE_VALUES, req.NumeroFedere);

  e = rep.Exception;

  return rep.Retract;
}


// ------------------------
// -- 4.4 discoverObject --
// ------------------------

void 
ObjectManagement::discoverObject(ObjectHandle theObjectHandle, 
				 ObjectClassHandle theObjectClassHandle,
				 const char *theObjectName,
				 FederationTime theTime,
				 EventRetractionHandle theHandle,
				 TypeException &)
{
  Message req, rep;
 
  req.Type = DISCOVER_OBJECT;
  req.Objectid = theObjectHandle;
  req.objectClassHandle = theObjectClassHandle;
  req.Date = theTime;
  req.Retract = theHandle;
  req.setName((char*)theObjectName);

  // BUG: Et on fait quoi de la reponse ?
  _GC->requestFederateService(&req, &rep);

  // Insertion de l'objet decouvert dans la liste interne du federe 
  _theRootObj->ObjectClasses->registerInstance(_GF->federate,
					       theObjectClassHandle,
					       req.Objectid,
					       req.getName());

}
 

// --------------------------------
// -- 4.5 reflectAttributeValues --
// --------------------------------

void 
ObjectManagement::reflectAttributeValues(ObjectHandle theObjectHandle, 
					 AttributeHandle *attribArray,
					 AttributeValue *valueArray,
					 UShort attribArraySize,
					 FederationTime theTime, 
					 const char*  theTag, 
					 EventRetractionHandle theHandle,
					 TypeException &)
{
  Message req, rep;
  int i;

  req.Type = REFLECT_ATTRIBUTE_VALUES;
  req.Objectid = theObjectHandle;
  req.Date = theTime;
  req.Retract = theHandle;
  req.setTag(theTag);

  req.HandleArraySize = attribArraySize;

  for(i = 0; i < attribArraySize; i++) {
    req.HandleArray [i] = attribArray[i];
    req.setValue(i, valueArray [i]);
  }

  // BUG: Et on fait quoi de la reponse ?
  _GC->requestFederateService(&req, &rep);
}
 

// -------------------------
// -- 4.6 sendInteraction --
// -------------------------

EventRetractionHandle 
ObjectManagement::sendInteraction(InteractionClassHandle theInteraction,
				  ParameterHandle *paramArray,
				  ParameterValue *valueArray,
				  UShort paramArraySize,
				  FederationTime theTime, 
				  const char*  theTag,
				  TypeException &e)
{
  NetworkMessage req,rep;

  // Test local pour savoir si l'interaction est correcte.

  _theRootObj->Interactions->isReady(_GF->federate,
				     theInteraction,
				     paramArray,
				     paramArraySize);

  // Preparation du message au RTI.

  req.Type = m_SEND_INTERACTION;
  req.InteractionHandle = theInteraction;
  req.Date = theTime;

  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
 
  req.HandleArraySize = paramArraySize;

  for(int i=0; i<paramArraySize; i++)
    {
      req.HandleArray [i] = paramArray [i];
      req.setValue(i, valueArray [i]);
    }

  strcpy(req.Label, theTag);

  // Emission et attente de la reponse.

  _GC->sendMessage(&req);

  _GC->waitMessage(&rep, m_SEND_INTERACTION, req.NumeroFedere);
 
  e = rep.Exception;

  return rep.Retract;
}


// ----------------------------
// -- 4.7 receiveInteraction --
// ----------------------------

void 
ObjectManagement::receiveInteraction(InteractionClassHandle theInteraction, 
				     ParameterHandle *paramArray,
				     ParameterValue *valueArray,
				     UShort paramArraySize,
				     FederationTime theTime, 
				     const char*  theTag, 
				     EventRetractionHandle theHandle,
				     TypeException &)
{
  int i;
  Message req, rep;
 
  req.Type = RECEIVE_INTERACTION;
  req.InteractionHandle = theInteraction;
  req.Date = theTime;
  req.Retract = theHandle;
  req.setTag(theTag);

  req.HandleArraySize = paramArraySize;
  for(i = 0; i < paramArraySize; i++) {
    req.HandleArray[i] = paramArray[i];
    req.setValue(i, valueArray[i]);
  }

  // BUG: On fait quoi de la reponse ?
  _GC->requestFederateService(&req, &rep);
}
 

// ----------------------
// -- 4.8 deleteObject --
// ----------------------

EventRetractionHandle 
ObjectManagement::deleteObject(ObjectHandle theObjectHandle,
			       const char*  theTag,
			       TypeException &e)
{
  NetworkMessage req,rep;
 
  req.Type = m_DELETE_OBJECT;
  req.Objectid = theObjectHandle;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
 
  strcpy(req.Label, theTag);

  _GC->sendMessage(&req);

  _GC->waitMessage(&rep, m_DELETE_OBJECT, req.NumeroFedere);
 
  e = rep.Exception;

  if(e == e_NO_EXCEPTION)
    _theRootObj->ObjectClasses->deleteObject(_GF->federate,
					     theObjectHandle,
					     theTag);

  return rep.Retract;
}


// --------------------------
// -- 4.9(1) removeObject --
// --------------------------

void 
ObjectManagement::removeObject(ObjectHandle theObjectHandle,
			       FederateHandle theFederateHandle,
			       const char*  theTag, 
			       EventRetractionHandle theHandle,
			       TypeException &)
{
  Message req,rep;

  req.Type = REMOVE_OBJECT;
  req.Objectid = theObjectHandle;
  req.Retract = theHandle;
  req.setTag(theTag);

  // BUG: On fait quoi de la reponse ?
  _GC->requestFederateService(&req, &rep);
 
  _theRootObj->ObjectClasses->deleteObject(theFederateHandle,
					   theObjectHandle,
					   theTag);
}
 
// --------------------------
// -- 4.9(2) removeObject --
// --------------------------

void 
ObjectManagement::removeObject(ObjectHandle, 
			       ObjectRemovalReason,
			       TypeException &)
{
  printf("ObjectManagement.cc: RemoveObject(2) not implemented.\n");
  throw RTIinternalError();
}


// ---------------------------------------
// -- 4.10 changeAttributeTransportType --
// ---------------------------------------

EventRetractionHandle
ObjectManagement::changeAttributeTransportType(ObjectHandle theObjectHandle, 
					       AttributeHandle *attribArray,
					       UShort attribArraySize,
					       TransportType theType,
					       TypeException &e)
{
  NetworkMessage req,rep;
  int i;

  req.Type = m_CHANGE_ATTRIBUTE_TRANSPORT_TYPE;
  req.Objectid = theObjectHandle;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
  req.Transport = theType;

  for(i = 0;i < attribArraySize;i++)
    req.HandleArray[i] = attribArray[i];
 
  _GC->sendMessage(&req);

  _GC->waitMessage(&rep, 
		   m_CHANGE_ATTRIBUTE_TRANSPORT_TYPE,
		   req.NumeroFedere);

  e = rep.Exception;

  return rep.Retract;
}
 

// -----------------------------------
// -- 4.11 changeAttributeOrderType --
// -----------------------------------

EventRetractionHandle
ObjectManagement::changeAttributeOrderType(ObjectHandle theObjectHandle, 
					   AttributeHandle *attribArray,
					   UShort attribArraySize,
					   OrderType theType,
					   TypeException &e)
{
  NetworkMessage req,rep;
  int i;

  req.Type = m_CHANGE_ATTRIBUTE_ORDER_TYPE;
  req.Objectid = theObjectHandle;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
  req.Order = theType;

  for(i = 0;i < attribArraySize;i++)
    req.HandleArray[i] = attribArray[i];
 

  _GC->sendMessage(&req);

  _GC->waitMessage(&rep, m_CHANGE_ATTRIBUTE_ORDER_TYPE, req.NumeroFedere);

  e = rep.Exception;

  return rep.Retract;
}
 

// -----------------------------------------
// -- 4.12 changeInteractionTransportType --
// -----------------------------------------

EventRetractionHandle ObjectManagement::
changeInteractionTransportType(InteractionClassHandle theClassID, 
			       TransportType theType,
			       TypeException &e)
{
  NetworkMessage req,rep;

  req.Type = m_CHANGE_INTERACTION_TRANSPORT_TYPE;
  req.InteractionHandle = theClassID;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
  req.Transport = theType;
 
  _GC->sendMessage(&req);

  _GC->waitMessage(&rep,
		   m_CHANGE_INTERACTION_TRANSPORT_TYPE,
		   req.NumeroFedere);

  e = rep.Exception;

  return rep.Retract;
}
 

// -------------------------------------
// -- 4.13 changeInteractionOrderType --
// -------------------------------------

EventRetractionHandle ObjectManagement::
changeInteractionOrderType(InteractionClassHandle theClassID, 
			   OrderType theType,
			   TypeException &e)
{
  NetworkMessage req,rep;

  req.Type = m_CHANGE_INTERACTION_ORDER_TYPE;
  req.InteractionHandle = theClassID;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
  req.Order = theType;

  _GC->sendMessage(&req);

  _GC->waitMessage(&rep, m_CHANGE_INTERACTION_ORDER_TYPE, req.NumeroFedere);

  e = rep.Exception;

  return rep.Retract;
}


// ------------------------------------------------
// -- 4.14(1) requestObjectAttributeValueUpdate --
// ------------------------------------------------

EventRetractionHandle ObjectManagement::
requestObjectAttributeValueUpdate(ObjectHandle theObjectHandle,
				  AttributeHandle *attribArray,
				  UShort attribArraySize,
				  TypeException &e)
{ 
  NetworkMessage req,rep;
  int i;

  req.Type = m_REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE;
  req.Objectid = theObjectHandle;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
 
  for(i = 0; i < attribArraySize; i++)
    req.HandleArray[i] = attribArray[i];

  req.HandleArraySize = attribArraySize;

  _GC->sendMessage(&req);

  _GC->waitMessage(&rep, 
		   m_REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE,
		   req.NumeroFedere);

  e = rep.Exception;

  return rep.Retract;
}


// ------------------------------------------------
// -- 4.14(2) requestObjectAttributeValueUpdate --
// ------------------------------------------------

// EventRetractionHandle ObjectManagement::
// requestObjectAttributeValueUpdate(ObjectClassHandle theObjectClassID,
// 				  AttributeHandle *attribArray,
// 				  UShort attribArraySize,
// 				  TypeException &e)
// { 
//   NetworkMessage req,rep;
//   int i;

//   req.Type = m_REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE;
//   req.objectClassHandle = theObjectClassID;
//   req.NumeroFederation = _GF->_numero_federation;
//   req.NumeroFedere = _GF->federate;
 
//   for(i = 0; i < attribArraySize; i++)
//     req.HandleArray[i] = attribArray[i];

//   req.HandleArraySize = attribArraySize;

//   _GC->sendMessage(&req);

//   _GC->waitMessage(&rep,
// 		   m_REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE,
// 		   req.NumeroFedere);

//   e = rep.Exception;

//   return rep.Retract;
// }

 
// --------------------------------------
// -- 4.15 provideAttributeValueUpdate --
// --------------------------------------

void 
ObjectManagement::provideAttributeValueUpdate(ObjectHandle,
					      AttributeValue &,
					      TypeException &)
{
  printf("ObjectManagement.cc: provideAttributeValueUpdate not implemented\n");
  throw RTIinternalError();
}


// ------------------
// -- 4.16 retract --
// ------------------

void ObjectManagement::retract(EventRetractionHandle /*theHandle*/,
			       TypeException & /*e*/)
{
  printf("GO.cc: retract not implemented.\n");
  throw RTIinternalError();
}


// ----------------------------
// -- 4.17 reflectRetraction --
// ----------------------------

void 
ObjectManagement::reflectRetraction(EventRetractionHandle,
				    TypeException &)
{
  printf("GO.cc: reflectRetraction not implemented.\n");
  throw RTIinternalError();
}

// ---------------------------------------------------------------------------
//! 8.1 getObjectClassHandle.
ObjectClassHandle 
ObjectManagement::getObjectClassHandle(const char* theName)
{
  return _theRootObj->ObjectClasses->getObjectClassHandle(theName);
}


// ---------------------------------------------------------------------------
//! 8.2 getObjectClassName.
const char* 
ObjectManagement::getObjectClassName(ObjectClassHandle theHandle)
{
  return _theRootObj->ObjectClasses->getObjectClassName(theHandle);
}
 
// ---------------------------------------------------------------------------
//! 8.3 getAttributeHandle.
AttributeHandle 
ObjectManagement::getAttributeHandle(const char* theName, 
                                     ObjectClassHandle theClassHandle)
{
  return _theRootObj->ObjectClasses->getAttributeHandle(theName,
							theClassHandle);
}
 
// ---------------------------------------------------------------------------
//! 8.4 getAttributeName.
const char* 
ObjectManagement::getAttributeName(AttributeHandle theHandle, 
                                   ObjectClassHandle theClassHandle)
{
  return _theRootObj->ObjectClasses->getAttributeName(theHandle,
						      theClassHandle);
}

// ---------------------------------------------------------------------------
//! 8.5 getInteractionClassHandle.
InteractionClassHandle 
ObjectManagement::getInteractionClassHandle(const char* theName)
{
  return _theRootObj->Interactions->getInteractionClassHandle(theName);
}
 
// ---------------------------------------------------------------------------
//! 8.6 getInteractionClassName.
const char* 
ObjectManagement::
getInteractionClassName(InteractionClassHandle theClassHandle)
{
  return 
    _theRootObj->Interactions->getInteractionClassName(theClassHandle);
}
 
// ---------------------------------------------------------------------------
//! 8.7 getParameterHandle.
ParameterHandle 
ObjectManagement::getParameterHandle(const char* theParameterName, 
                                     InteractionClassHandle theClassHandle)
{
  return _theRootObj->Interactions->getParameterHandle(theParameterName,
						       theClassHandle);
}
 
// ---------------------------------------------------------------------------
//! 8.8 getParameterName.
const char* 
ObjectManagement::getParameterName(ParameterHandle theParameterHandle, 
                                   InteractionClassHandle theClassHandle)
{
  return _theRootObj->Interactions->getParameterName(theParameterHandle,
						     theClassHandle);
}
}
}

// $Id: ObjectManagement.cc,v 3.3 2003/01/17 18:19:47 breholee Exp $
