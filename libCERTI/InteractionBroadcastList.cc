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
// $Id: InteractionBroadcastList.cc,v 3.1 2002/11/30 22:13:32 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "InteractionBroadcastList.hh"

namespace certi {

static pdCDebug D("INTBROADCASTLIST", "(broadcas) - ");

// -----------------
// -- addFederate --
// -----------------

void InteractionBroadcastList::addFederate(FederateHandle  theFederate)
{
  InteractionBroadcastLine *Line = NULL;

  // 1. Is there already a line in the list for this Federate?
  Line = getLineWithFederate(theFederate);

  // If NO, add a new one, in the bsWaiting State.
  if(Line == NULL) {
    Line = new InteractionBroadcastLine(theFederate, bsWaiting);
    lst.Inserer(1, Line);
    D.Out(pdRegister, 
	   "Adding new line in list for Federate %d.", theFederate);
  }
  else
    D.Out(pdTrace,"Message already sent to federate %d.", theFederate);
}


// --------------------------------
// --  InteractionBroadcastList --
// --------------------------------

InteractionBroadcastList::InteractionBroadcastList(NetworkMessage *theMsg)
  : lst()
{
  InteractionBroadcastLine *FirstLine = NULL;

  if(theMsg == NULL)
    throw RTIinternalError("Null Broadcast Message.");

  Message  = theMsg;

  // Add reference of the sender(so it does not receive its own message).
  if(Message->NumeroFedere != 0) {
    FirstLine = new InteractionBroadcastLine(Message->NumeroFedere,
					       bsSent);
    lst.Inserer(1, FirstLine);
  }
}


// ---------------------------------
// --  ~InteractionBroadcastList --
// ---------------------------------

InteractionBroadcastList::~InteractionBroadcastList()
{
  clear();
}


// ----------------
// -- EmptyList  --
// ----------------

void InteractionBroadcastList::clear(void)
{
  InteractionBroadcastLine *Line = NULL;

  delete Message;

  Message   = NULL;

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

InteractionBroadcastLine *InteractionBroadcastList::
getLineWithFederate(FederateHandle theFederate)
{
  InteractionBroadcastLine *Line = NULL;
  int                        i;

  for(i = 1; i <= lst.getLength(); i++) {
    Line = lst.Ieme(i);
    if(Line->federate == theFederate)
      return Line;
  }

  return NULL;
}


// ------------------------
// -- SendPendingMessage --
// ------------------------

void InteractionBroadcastList::
sendPendingMessage(SecurityServer *Server)
{
  int LineIndex;
  InteractionBroadcastLine *Line = NULL;
  Socket *socket = NULL;

  // Pour chaque ligne de la liste

  for(LineIndex = 1; LineIndex <= lst.getLength(); LineIndex++) {
    Line = lst.Ieme(LineIndex);
      
    // Si le federe attend un message
    if(Line->state == bsWaiting) {

      // 1. Envoyer le message au federe
      D.Out(pdProtocol, 
	     "Broadcasting message to Federate %d.", Line->federate);
      try {
#ifdef HLA_USES_UDP
	socket = Server->getSocketLink(Line->federate, BEST_EFFORT);
#else
	socket = Server->getSocketLink(Line->federate);
#endif
	Message->write(socket);
      } 
      catch(RTIinternalError &e) {
	D.Out(pdExcept, "Reference to a killed Federate while broadcasting.");
      }
      catch(NetworkError &e) {
	D.Out(pdExcept, "Network error while broadcasting, ignoring.");
      }
      
      // 2. Marquer le federe comme ayant recu le message.
      Line->state = bsSent;
    }
    else
      D.Out(pdProtocol, 
	     "No message sent to Federate %d.", Line->federate);	  
  }

}

}

// $Id: InteractionBroadcastList.cc,v 3.1 2002/11/30 22:13:32 breholee Exp $
