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
// $Id: ObjectClassBroadcastList.cc,v 3.2 2002/11/30 22:13:32 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "ObjectClassBroadcastList.hh"

namespace certi {

static pdCDebug D("OBJBROADCASTLIST", "(broadcas) - ");

// ------------------
// -- AdaptMessage --
// ------------------

NetworkMessage *ObjectClassBroadcastList::
adaptMessage(ObjectBroadcastLine *Line)
{
  UShort           i;
  UShort           CurrentSize;
  AttributeHandle  CurrentAttrib;
  char             Buffer [MAX_BYTES_PER_VALUE + 1];

  NetworkMessage  *ReducedMessage = new NetworkMessage;
  if((Message->Type != m_REFLECT_ATTRIBUTE_VALUES) 
      &&(Message->Type != m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION)) {
    delete ReducedMessage;
    throw RTIinternalError("Bad Message type in Broadcast's AdaptMsg.");
  }

  // Copy static informations.
  ReducedMessage->Type             = Message->Type;
  ReducedMessage->Exception        = Message->Exception;
  ReducedMessage->NumeroFederation = Message->NumeroFederation;
  ReducedMessage->NumeroFedere     = Message->NumeroFedere;
  ReducedMessage->Objectid         = Message->Objectid;
  ReducedMessage->Date             = Message->Date;

  strcpy(ReducedMessage->Label, Message->Label);

  // Copy attributes that are in the bsWaiting state in Line.
  ReducedMessage->HandleArraySize  = 0;

  for(i = 0; i < Message->HandleArraySize; i++) {

    CurrentAttrib = Message->HandleArray [i];

    if(Line->State [CurrentAttrib] == bsWaiting) {

      // Update number of attributes in ReducedMessage.
      CurrentSize    =  ReducedMessage->HandleArraySize;
      ReducedMessage->HandleArraySize ++;

      // Copy Attribute Handle.
      ReducedMessage->HandleArray [CurrentSize] = CurrentAttrib;

		  if(Message->Type == m_REFLECT_ATTRIBUTE_VALUES){
			// Copy Attribute Value.
      Message      ->getValue(i,           Buffer);
      ReducedMessage->setValue(CurrentSize, Buffer);
			}
			
    }
  }

  return ReducedMessage;
}


// -----------------
// -- addFederate --
// -----------------

void ObjectClassBroadcastList::addFederate(FederateHandle  theFederate,
					  AttributeHandle theAttribute)
{
  ObjectBroadcastLine *Line = NULL;

  if(theAttribute > MaxHandle) {
    D.Out(pdExcept, "Bad attribute handle: %u > %u.", 
	   theAttribute, MaxHandle);
    throw RTIinternalError();
  }

  Line = getLineWithFederate(theFederate);

  if(Line == NULL) {
    Line = new ObjectBroadcastLine(theFederate, bsNotSub);
    lst.Inserer(1, Line);
    D.Out(pdRegister, 
	   "Adding new line in list for Federate %d.", theFederate);
  }
  
  if(Line->State [theAttribute] != bsSent) {
    Line->State [theAttribute] = bsWaiting;
    D.Out(pdRegister, 
	   "List attribute %d for Federate %d is now bsWaiting.",
	   theAttribute, theFederate);
  }
  else
    D.Out(pdTrace,
	   "Message already sent to federate %d about attribute %d.",
	   theFederate, theAttribute);

}


// -----------------------------
// --  ObjectBroadcastLine --
// -----------------------------

ObjectBroadcastLine::
ObjectBroadcastLine(FederateHandle theFederate,
			BroadcastState theInitialState)
{
  AttributeHandle i;

  Federate = theFederate;

  for(i = 0; i <= MAX_ATTRIBUTES_PER_CLASS; i++)
    State [i] = theInitialState;
}


// -----------------------------
// --  ObjectClassBroadcastList --
// -----------------------------

ObjectClassBroadcastList::ObjectClassBroadcastList(NetworkMessage  *theMsg,
						AttributeHandle  MaxAttHandle)
  : MaxHandle(MaxAttHandle), lst()
{
  ObjectBroadcastLine *FirstLine = NULL;

  if(theMsg == NULL)
    throw RTIinternalError("Null Broadcast Message.");

  Message  = theMsg;

  // Add reference of the sender.
  if(Message->NumeroFedere != 0) {
    FirstLine = new ObjectBroadcastLine(Message->NumeroFedere,
					    bsSent);
    lst.Inserer(1, FirstLine);
  }					 
}


// ------------------------------
// --  ~ObjectClassBroadcastList --
// ------------------------------

ObjectClassBroadcastList::~ObjectClassBroadcastList()
{
  clear();
}


// ----------------
// -- EmptyList  --
// ----------------

void ObjectClassBroadcastList::clear(void)
{
  ObjectBroadcastLine *Line = NULL;

  delete Message;

  Message   = NULL;
  MaxHandle = 0;

  while(lst.getLength() > 0) {
    Line = lst.Ieme(1);
    lst.Supprimer(1);
    delete Line;
  }

  D.Out(pdTerm, "List is now empty.");
}


// -------------------------
// -- GetLineWithFederate --
// -------------------------

ObjectBroadcastLine *ObjectClassBroadcastList::
getLineWithFederate(FederateHandle theFederate)
{
  ObjectBroadcastLine *Line = NULL;
  int                     i;

  for(i = 1; i <= lst.getLength(); i++) {
    Line = lst.Ieme(i);
    if(Line->Federate == theFederate)
      return Line;
  }

  return NULL;
}

// ---------------
// -- IsWaiting --
// ---------------

Boolean ObjectClassBroadcastList::isWaiting(ObjectBroadcastLine *Line)
{
  unsigned int AttrIndex;
  
  for(AttrIndex = 1; AttrIndex <= MaxHandle; AttrIndex++)
    if(Line->State [AttrIndex] == bsWaiting)
      return RTI_TRUE;

  return RTI_FALSE;
}


// --------------------------
// -- SendPendingDOMessage --
// --------------------------

void ObjectClassBroadcastList::
sendPendingDOMessage(SecurityServer *Server)
{
  int                     LineIndex;
  ObjectBroadcastLine *Line           = NULL;
  Socket                *socket         = NULL;

  // Pour chaque ligne de la liste
  for(LineIndex = 1; LineIndex <= lst.getLength(); LineIndex++) {
    Line = lst.Ieme(LineIndex);

    // Si le federe attend un message(attribute 0 en attente)
    if(Line->State [0] == bsWaiting) {

      // 1. Envoyer le message au federe
      D.Out(pdProtocol, 
	     "Broadcasting message to Federate %d.", Line->Federate);
      try {
	socket = Server->getSocketLink(Line->Federate);
	Message->write(socket);
      }
      catch(RTIinternalError &e) {
	D.Out(pdExcept, "Reference to a killed Federate while broadcasting.");
      }
      catch(NetworkError &e) {
	D.Out(pdExcept, "Network error while broadcasting, ignoring.");
      }

      // 2. Marquer le federe comme ayant recu le message.
      Line->State [0] = bsSent;
    }
    else
      D.Out(pdProtocol, 
	     "No message sent to Federate %d.", Line->Federate);	  
  }
}


// ------------------------
// -- SendPendingMessage --
// ------------------------

void ObjectClassBroadcastList::
sendPendingMessage(SecurityServer *Server)
{
  switch(Message->Type) {

  case m_REFLECT_ATTRIBUTE_VALUES:
  case m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
    sendPendingRAVMessage(Server);
    break;

  case m_DISCOVER_OBJECT:
  case m_REMOVE_OBJECT:
    sendPendingDOMessage(Server);
    break;
    
  default:
    throw RTIinternalError("Unknown message type to broadcast.");
  }

}

// ---------------------------
// -- SendPendingRAVMessage --
// ---------------------------

void ObjectClassBroadcastList::
sendPendingRAVMessage(SecurityServer *Server)
{
  int                     LineIndex;
  unsigned int            AttrIndex;

  Boolean                 AllWaiting;
  AttributeHandle         CurrentAttrib;

  ObjectBroadcastLine *Line           = NULL;
  Socket                *socket         = NULL;
  NetworkMessage         *CurrentMessage = NULL;
  
	// Pour chacunes des lignes :
  for(LineIndex = 1; LineIndex <= lst.getLength(); LineIndex++) {
    Line = lst.Ieme(LineIndex);
    
    // Si AU MOINS UN des attributs est en bsWaiting
    if(isWaiting(Line) == RTI_TRUE) {
      
      // 1. Est-ce que tous les attributs du message sont en bsWaiting ?
      AllWaiting = RTI_TRUE;
      for(AttrIndex = 0; 
	   AttrIndex < Message->HandleArraySize;
	   AttrIndex ++) {
	CurrentAttrib = Message->HandleArray [AttrIndex];
	if(Line->State [CurrentAttrib] != bsWaiting)
	  AllWaiting = RTI_FALSE;
      }
      
      if(AllWaiting == RTI_FALSE) {
	//   NON: Faire un nouveau message, contenant seulement les attributs
	//        en bsWaiting.
	CurrentMessage = adaptMessage(Line);
	D.Out(pdProtocol, "Broadcasting reduced message to Federate %d.",
	       Line->Federate);
      }
      else {
	// OUI: Rien a faire
	CurrentMessage = Message;
	D.Out(pdProtocol, "Broadcasting complete message to Federate %d.",
	       Line->Federate);
      }

      // 2. Envoyer le message(ou la copie reduite),
      try {
#ifdef HLA_USES_UDP
	socket = Server->getSocketLink(Line->Federate, BEST_EFFORT);
#else
	socket = Server->getSocketLink(Line->Federate);
#endif
	CurrentMessage->write(socket);
      }
      catch(RTIinternalError &e) {
	D.Out(pdExcept, "Reference to a killed Federate while broadcasting.");
      }
      catch(NetworkError &e) {
	D.Out(pdExcept, "Network error while broadcasting, ignoring.");
      }
      
      // 3. marquer les attributs en bsSent.
      for(AttrIndex = 1; AttrIndex <= MaxHandle; AttrIndex ++)
	if(Line->State [AttrIndex] == bsWaiting)
	  Line->State [AttrIndex] = bsSent;
      
      // 4. Eventuellement effacer la copie du message.
      if(CurrentMessage != Message) {
	delete CurrentMessage;
	CurrentMessage = NULL;
      }
      
    } // Si AU MOINS UN des attributs est en bsWaiting
    else
      D.Out(pdProtocol, 
	     "No message sent to Federate %d.", Line->Federate);	  
    
  } // Pour chaque ligne...
  
}

}

// $Id: ObjectClassBroadcastList.cc,v 3.2 2002/11/30 22:13:32 breholee Exp $
