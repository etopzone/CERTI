// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
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
// $Id: InteractionBroadcastList.hh,v 3.1 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_INTERACTION_BROADCAST_LIST_HH
#define _CERTI_INTERACTION_BROADCAST_LIST_HH

#include "RTItypes.hh"
#include "NetworkMessage.hh"
#include "SecurityServer.hh"
#include "PrettyDebug.hh"

// Describe the BroadcastList's elements, and the possible states.
#include "InteractionBroadcastLine.hh"

namespace certi {

class InteractionBroadcastList {

public:

  // -----------------------
  // -- Public Attributes --
  // -----------------------

  // First, the Message to be broadcast. This message must be
  // allocated before calling the constructor of the class, be is
  // deleted by the destructor.
  NetworkMessage *Message;

  // --------------------
  // -- Public Methods --
  // --------------------

  // theMsg must have been allocated, and will be destroyed by the
  // destructor.  theMsg->NumeroFedere is added to the list, and
  // its state is set as "Sent".
  InteractionBroadcastList(NetworkMessage  *theMsg);

  // Free all structures, including Message.
  ~InteractionBroadcastList();
  
  // Empty the list so it can reused(like the destructor).
  void clear(void);

  // Add a federate to the list. If it was not present in the list,
  // a new line is added and its state is set as bsNotSub.  Then if
  // the Federate has not been sent a message for this interaction,
  // the line's state is set to bsWaiting.
  void addFederate(FederateHandle  theFederate);

  // IMPORTANT: Before calling this method, be sure to set the 
  //      Message->NumeroFederation handle.
  //
  // Broadcast the message to all the Federate in the bsWaiting state,
  // and then set their state to bsSent.
  void sendPendingMessage(SecurityServer *Server);

private:

  // ------------------------
  // -- Private Attributes --
  // ------------------------

  List <InteractionBroadcastLine *> lst ;

  // ---------------------
  // -- Private Methods --
  // ---------------------

  // Return the line of the list describing federate 'theFederate', or NULL.
  InteractionBroadcastLine *getLineWithFederate(FederateHandle theFederate);
};

}

#endif // _CERTI_INTERACTION_BROADCAST_LIST_HH

// $Id: InteractionBroadcastList.hh,v 3.1 2002/12/11 00:47:33 breholee Exp $
