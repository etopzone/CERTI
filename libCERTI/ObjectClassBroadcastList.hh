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
// $Id: ObjectClassBroadcastList.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

// $Id: ObjectClassBroadcastList.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------
// Classes Liste de Diffusion. Ces objets(un pour les classes
// d'objet, l'autre pour les interactions) servent a transporter les
// informations necessaires a la diffusion d'un RegisterObject ou d'un
// UpdateAttributeValue pour les classes d'objets, ou d'un
// SendInteraction pour les classes d'interaction.

#ifndef _CERTI_OBJECT_CLASS_BROADCAST_LIST_HH
#define _CERTI_OBJECT_CLASS_BROADCAST_LIST_HH

#include "RTItypes.hh"
#include "NetworkMessage.hh"
#include "SecurityServer.hh"
#include "ObjectBroadcastLine.hh"
#include "PrettyDebug.hh"

namespace certi {

class ObjectClassBroadcastList {

public:

  // -----------------------
  // -- Public Attributes --
  // -----------------------

  // First, the Message to be broadcast. This message must be allocated 
  // before calling the constructor of the class, be is deleted by the
  // destructor.
  NetworkMessage *Message;

  // --------------------
  // -- Public Methods --
  // --------------------

  // theMsg must have been allocated, and will be destroyed by the
  // destructor.  theMsg->NumeroFedere is added to the list, and
  // its state is set as "Sent" for all attributes.  For RAVs
  // messages, MaxAttHandle is the greatest attribute handle of the
  // class. For Discover_Object message, it can be 0 to mean "any
  // attribute".
  ObjectClassBroadcastList(NetworkMessage *theMsg,
			   AttributeHandle MaxAttHandle = 0);

  // Free all structures, including Message.
  ~ObjectClassBroadcastList();
 
  // Empty the list so it can reused(like the destructor).
  void clear(void);

  // Add a federate to the list. If it was not present in the list,
  // a new line is added and all attributes are marked as
  // bsNotSubscriber.  Then if the Federate has not been sent a
  // message for this attribute, the attribute(for the federate) is
  // marked has bsWaiting.  theAttribute can be not specified in the
  // case of a DiscoverObject message.
  void addFederate(FederateHandle theFederate,
		   AttributeHandle theAttribute = 0);

  // IMPORTANT: Before calling this method, be sure to set the 
  // Message->NumeroFederation handle.
  //
  // Broadcast the message to all the Federate in the bsWaiting
  // state.  If it is a DiscoverObject message, the message is sent
  // as is, and the Federate is marked as bsSent for the ANY
  // attribute.  If it is a RAV message, the message is first
  // copied, without the Attribute list, and then all pending
  // attributes(in the bsWainting state) are added to the copy. The
  // copy is sent, and attributes are marked as bsSent.
  void sendPendingMessage(SecurityServer *Server);

private:

  // ------------------------
  // -- Private Attributes --
  // ------------------------

  AttributeHandle MaxHandle;

  List <ObjectBroadcastLine *> lst ;

  // ---------------------
  // -- Private Methods --
  // ---------------------

  // Return a copy of the REFLECT_ATTRIBUTE_VALUES message 'Message'
  // containing references omly to the attributes marked as
  // bsWaiting in the line 'Line'.  The returned message should be
  // deleted later.
  NetworkMessage *adaptMessage(ObjectBroadcastLine *Line);

  // Return the line of the list describing federate 'theFederate', or NULL.
  ObjectBroadcastLine *getLineWithFederate(FederateHandle theFederate);

  // Return RTI_TRUE is there is any Attribute in the bsWaiting state in
  // the line 'Line', else RTI_FALSE.
  Boolean isWaiting(ObjectBroadcastLine *Line);

  // The two next methods are called by the public
  // SendPendingMessage methods.  They respectively handle
  // DiscoverObject and ReflectAttributeValues messages.
  void sendPendingDOMessage(SecurityServer *Server);
  void sendPendingRAVMessage(SecurityServer *Server);
};
}

#endif // _CERTI_OBJECT_CLASS_BROADCAST_LIST_HH

// $Id: ObjectClassBroadcastList.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
