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
// $Id: DeclarationManagement.cc,v 3.1 2002/11/26 15:48:01 breholee Exp $
// ---------------------------------------------------------------------------

#include "DeclarationManagement.hh"

namespace certi {
namespace rtia {

pdCDebug D("RTIA_GD", "(RTIA GD ) - ");

// -------------------------
// -- DeclarationManagement --
// -------------------------

DeclarationManagement::DeclarationManagement(Communications *GC,
					     Queues *GQueues,
					     FederationManagement *GF,
					     RootObject *theRootObj)
{
  _GC = GC;
  _GQueues = GQueues;
  _GF = GF;
  _theRootObj = theRootObj;
}


// --------------------------
// -- ~DeclarationManagement --
// --------------------------

DeclarationManagement::~DeclarationManagement()
{
}

// --------------------------------
// -- 3.1(1) publishObjectClass --
// --------------------------------
void DeclarationManagement::
publishObjectClass(ObjectClassHandle theClassHandle, 
		   AttributeHandle *attribArray,
		   UShort attribArraySize,
		   TypeException &e)
{
  NetworkMessage req,rep;

  e = e_NO_EXCEPTION;

  // Partie Locale

  try {
    _theRootObj->ObjectClasses->publish(_GF->federate,
					theClassHandle,
					attribArray,
					attribArraySize,
					RTI_TRUE);
  }
  catch(Exception *e) {
    D.Out(pdExcept, "Exception catched in PublishObjectClass.");
    throw e;
  }
 
  // Partie RTIG

  req.Type = m_PUBLISH_OBJECT_CLASS;
  req.objectClassHandle = theClassHandle; 
  req.HandleArraySize = attribArraySize;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
 
  for(int i=0; i<attribArraySize; i++)
    req.HandleArray[i] = attribArray[i];
 
  // Emission
  _GC->sendMessage(&req);
 
  // Reception
  _GC->waitMessage(&rep,
		   m_PUBLISH_OBJECT_CLASS,
		   req.NumeroFedere);
 
  e = rep.Exception;
}

// ----------------------------------
// -- 3.1(2) unpublishObjectClass --
// ----------------------------------

void DeclarationManagement::
unpublishObjectClass(ObjectClassHandle theClassHandle, 
		     TypeException &e)
{
  NetworkMessage req,rep;

  // Variables leurres
  AttributeHandle *attribArray = NULL;
  UShort attribArraySize = 0;

  e = e_NO_EXCEPTION;

  // Partie Locale

  try {
    _theRootObj->ObjectClasses->publish(_GF->federate,
					theClassHandle,
					attribArray,
					attribArraySize,
					RTI_FALSE);
  } catch(Exception *e) {
    D.Out(pdExcept, "Exception catched in UnpublishObjectClass.");
    throw e;
  }

  // Partie RTIG

  req.Type = m_UNPUBLISH_OBJECT_CLASS;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
  req.objectClassHandle = theClassHandle; 
 
  // Emission de la requete vers le RTIG
  _GC->sendMessage(&req);
 
  // On attend une reponse
  _GC->waitMessage(&rep,
		   m_UNPUBLISH_OBJECT_CLASS,
		   req.NumeroFedere);

  e = rep.Exception;
}


// -------------------------------------
// -- 3.2(1) publishInteractionClass --
// -------------------------------------

void DeclarationManagement::
publishInteractionClass(InteractionClassHandle theInteractionHandle,
			TypeException &e)
{
  NetworkMessage req,rep;

  e = e_NO_EXCEPTION;

  // Partie Locale

  try {
    _theRootObj->Interactions->publish(_GF->federate,
				       theInteractionHandle,
				       RTI_TRUE);
  } catch(Exception *e) {
    D.Out(pdExcept, "Exception catched in publishInteractionClass.");
    throw e;
  }

  // Partie RTIG

  req.Type = m_PUBLISH_INTERACTION_CLASS;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
  req.InteractionHandle = theInteractionHandle; 
 
  _GC->sendMessage(&req);
 
  _GC->waitMessage(&rep,
		   m_PUBLISH_INTERACTION_CLASS,
		   req.NumeroFedere);

  e = rep.Exception;
}


// ---------------------------------------
// -- 3.2(2) unpublishInteractionClass --
// ---------------------------------------

void DeclarationManagement::
unpublishInteractionClass(InteractionClassHandle theInteractionHandle, 
			  TypeException &e)
{
  NetworkMessage req,rep;

  e = e_NO_EXCEPTION;

  // Partie Locale

  try {
    _theRootObj->Interactions->publish(_GF->federate,
				       theInteractionHandle,
				       RTI_FALSE);
  } catch(Exception *e) {
    D.Out(pdExcept, "Exception catched in UnpublishInteractionClass.");
    throw e;
  }

  // Partie RTIG

  req.Type = m_UNPUBLISH_INTERACTION_CLASS;
  req.InteractionHandle = theInteractionHandle; 
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
 
  _GC->sendMessage(&req);
 
  _GC->waitMessage(&rep,
		   m_UNPUBLISH_INTERACTION_CLASS,
		   req.NumeroFedere);

  e = rep.Exception;
}


// -------------------------------------------
// -- 3.3(1) subscribeObjectClassAttribute --
// -------------------------------------------

void DeclarationManagement::
subscribeObjectClassAttribute(ObjectClassHandle theClassHandle, 
			      AttributeHandle *attribArray,
			      UShort attribArraySize,
			      TypeException &e)
{
  NetworkMessage req,rep;

  // Pas de partie locale pour les abonnements

  // Partie RTIG

  req.Type = m_SUBSCRIBE_OBJECT_CLASS;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
  req.objectClassHandle = theClassHandle; 
  req.HandleArraySize = attribArraySize;
 
  for(int i=0; i<attribArraySize; i++)
    req.HandleArray[i] = attribArray[i];
 
  // Emission
  _GC->sendMessage(&req);
 
  // Reception
  _GC->waitMessage(&rep,
		   m_SUBSCRIBE_OBJECT_CLASS,
		   req.NumeroFedere);

  e = rep.Exception;
 
}


// -------------------------------------------
// -- 3.3(2) subscribeObjectClassAttribute --(with Region)
// -------------------------------------------
void 
DeclarationManagement::subscribeObjectClassAttribute(ObjectClassHandle,
						     AttributeHandle,
						     HLA_Region,
						     TypeException &e)
{
  // BUG: Non implemente en F.0
  e = e_UnimplementedService;
}


// ---------------------------------------------
// -- 3.3(3) unsubscribeObjectClassAttribute --
// ---------------------------------------------

void 
DeclarationManagement::
unsubscribeObjectClassAttribute(ObjectClassHandle theClassHandle, 
				TypeException &e)
{
  NetworkMessage req,rep;
 
  e = e_NO_EXCEPTION;

  // Pas de Partie Locale pour les abonnements

  // Partie RTIG
  req.Type = m_UNSUBSCRIBE_OBJECT_CLASS;
  req.objectClassHandle = theClassHandle;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
 
  _GC->sendMessage(&req);
 
  _GC->waitMessage(&rep,
		   m_UNSUBSCRIBE_OBJECT_CLASS,
		   req.NumeroFedere);
 
  e = rep.Exception;
}


// ---------------------------------------------
// -- 3.3(4) unsubscribeObjectClassAttribute --(with Region)
// ---------------------------------------------

void DeclarationManagement::
unsubscribeObjectClassAttribute(ObjectClassHandle,
				HLA_Region,
				TypeException &e)
{
  e = e_UnimplementedService;
}


// ---------------------------------------
// -- 3.4(1) subscribeInteractionClass --
// ---------------------------------------

void DeclarationManagement::
subscribeInteractionClass(InteractionClassHandle theClassHandle, 
			  TypeException &e)
{
  NetworkMessage req,rep;

  e = e_NO_EXCEPTION;

  // Partie Locale

  // BUG: Pourquoi il y a-t-il une partie locale pour un abonnement ?
  // Ca ne va pas marcher avec les niveaux de securite !!!!

  try {
    _theRootObj->Interactions->subscribe(_GF->federate,
					 theClassHandle,
					 RTI_TRUE);
  } catch(Exception *e) {
    D.Out(pdExcept, "Exception catched in subscribeInteractionClass.");
    throw e;
  }

  // Partie RTIG

  req.Type = m_SUBSCRIBE_INTERACTION_CLASS;
  req.InteractionHandle = theClassHandle; 
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
 
  _GC->sendMessage(&req);

  _GC->waitMessage(&rep,
		   m_SUBSCRIBE_INTERACTION_CLASS,
		   req.NumeroFedere);

  e = rep.Exception;
}


// ---------------------------------------
// -- 3.4(2) subscribeInteractionClass --(with Region)
// ---------------------------------------

void 
DeclarationManagement::subscribeInteractionClass(InteractionClassHandle,
						 HLA_Region, 
						 TypeException &e)
{
  e = e_UnimplementedService;
}


// -----------------------------------------
// -- 3.4(3) unsubscribeInteractionClass --
// -----------------------------------------

void DeclarationManagement::
unsubscribeInteractionClass(InteractionClassHandle theClassHandle, 
			    TypeException &e)
{
  NetworkMessage req,rep;
 
  e = e_NO_EXCEPTION;
 
  // Partie Locale

  // BUG: Pourquoi il y a-t-il une partie locale pour un abonnement ?
  // Ca ne va pas marcher avec les niveaux de securite !!!!

  try {
    _theRootObj->Interactions->subscribe(_GF->federate,
					 theClassHandle,
					 RTI_FALSE);
  } catch(Exception *e) {
    D.Out(pdExcept, "Exception catched in subscribeInteractionClass.");
    throw e;
  }

  // Partie RTIG

  req.Type = m_UNSUBSCRIBE_INTERACTION_CLASS;
  req.InteractionHandle = theClassHandle;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
 
  _GC->sendMessage(&req);
 
  _GC->waitMessage(&rep,
		   m_UNSUBSCRIBE_INTERACTION_CLASS,
		   req.NumeroFedere);
 
  e = rep.Exception;
}


// -----------------------------------------
// -- 3.4(4) unsubscribeInteractionClass --(with Region)
// -----------------------------------------

void DeclarationManagement::
unsubscribeInteractionClass(InteractionClassHandle,
			    HLA_Region,
			    TypeException &e)
{
  e = e_UnimplementedService;
}


// ---------------------------------------------
// -- 3.5(1) startRegistrationForObjectClass --
// ---------------------------------------------

void DeclarationManagement::
startRegistrationForObjectClass(ObjectClassHandle theClass, 
				// CAttributeHandleValuePairSet &theAttributes, 
				TypeException &e)
{
  Message req, rep;

  // Pas de partie locale

  // Partie Federe

  req.Type = START_REGISTRATION_FOR_OBJECT_CLASS;
  req.objectClassHandle = theClass;

  _GC->sendUN(&req);

  _GC->receiveUN(&rep);

  if(rep.Type != req.Type) {
    D.Out(pdExcept, "Unknown response type when waiting for "
	  "START_REGISTRATION_FOR_OBJECT_CLASS.");
    throw RTIinternalError();
  }
 
  e = rep.Exception;
}


// --------------------------------------------
// -- 3.5(2) stopRegistrationForObjectClass --
// --------------------------------------------

void DeclarationManagement::
stopRegistrationForObjectClass(ObjectClassHandle theClass, 		
			       TypeException &e)
{
  Message req, rep;

  // Pas de partie Locale

  // Partie Federe

  req.Type = STOP_REGISTRATION_FOR_OBJECT_CLASS;
  req.objectClassHandle = theClass;

  _GC->sendUN(&req);

  _GC->receiveUN(&rep);

  if(rep.Type != req.Type) {
    D.Out(pdExcept, "Unknown response type when waiting for "
	  "START_REGISTRATION_FOR_OBJECT_CLASS.");
    throw RTIinternalError();
  }
 
  e = rep.Exception;
}


// ----------------------------------------
// -- 3.6(1) turnInteractionsOn --
// ----------------------------------------

void DeclarationManagement::
turnInteractionsOn(InteractionClassHandle theHandle, 
		   TypeException &e)
{
  Message req, rep;

  // Pas de partie Locale

  // Partie Federe

  req.Type = TURN_INTERACTIONS_ON;
  req.InteractionHandle = theHandle;

  _GC->sendUN(&req);

  _GC->receiveUN(&rep);

  if(rep.Type != req.Type) {
    D.Out(pdExcept, 
	  "Unknown response type, expecting TURN_INTERACTIONS_ON.");
    throw RTIinternalError();
  }
 
  e = rep.Exception;
}


// ---------------------------------------
// -- 3.6(2) turnInteractionsOff --
// ---------------------------------------

void DeclarationManagement::
turnInteractionsOff(InteractionClassHandle theHandle, 
		    TypeException &e)
{
  Message req, rep;

  // Pas de partie Locale

  // Partie Federe


  req.Type = TURN_INTERACTIONS_OFF;
  req.InteractionHandle = theHandle;

  _GC->sendUN(&req);

  _GC->receiveUN(&rep);

  if(rep.Type != req.Type) {
    D.Out(pdExcept, 
	  "Unknown response type, expecting TURN_INTERACTIONS_OFF.");
    throw RTIinternalError();
  }

  e = rep.Exception;
}

}
}

// $Id: DeclarationManagement.cc,v 3.1 2002/11/26 15:48:01 breholee Exp $
