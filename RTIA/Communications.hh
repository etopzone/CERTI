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
// $Id: Communications.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef GCOM_HH
#define GCOM_HH

#include <config.h>

#include "PrettyDebug.hh"
#include "baseTypes.hh"
#include "RTItypes.hh"
#include "NetworkMessage.hh" // NetworkMessage class
#include "Message.hh" // Message class
#include "SocketUN.hh" // SocketUN class
#include "SecureTCPSocket.hh" // SecureTCPSocket class
#include "List.hh"
#include "SocketUDP.hh"

#include <stdio.h>
#include <unistd.h>
#include <string>

#ifdef FEDERATION_USES_MULTICAST
#include "SocketMC.hh"
#endif

namespace certi {
namespace rtia {

class Communications : public SocketUN, 
#ifdef FEDERATION_USES_MULTICAST
		       public SocketMC, 
#endif
		       public SecureTCPSocket,
		       public SocketUDP 
{
public:
      
  Communications();
  ~Communications();

  // ---------------------
  // -- Network Methods --
  // ---------------------

  // Emet un message vers le RTIG.
  void sendMessage(NetworkMessage *Msg)
  { Msg->write((SecureTCPSocket *) this); };

  void sendUN(Message *Msg)
  { Msg->write((SocketUN *) this); };

  void receiveUN(Message *Msg)
  { Msg->read((SocketUN *) this); };

  // Lit un message venant soit du reseau soit du federe.
  // Renvoie la source dans le premier parametre(1 ou 2).
  void readMessage(int&, NetworkMessage *, Message *);

  // Attend un message venant du RTIG. Les parametres sont :
  // 1- Le message retourne,
  // 2- Le type de message attendu,
  // 3- Le federe qui a envoye le message, ou 0 si indifferent.
  void waitMessage(NetworkMessage *msg, 
		   TypeNetworkMessage type_msg, 
		   FederateHandle numeroFedere);
 
  void requestFederateService(Message *req, Message *rep);
  unsigned long getAddress(); 
  unsigned int getPort();

private:
  List <NetworkMessage*> _fileAttente;

  // Renvoie RTI_TRUE si un message du type "type_msg" venant du federe
  // "numeroFedere"(ou de n'importe qui si numerofedere vaut 0) etait
  // present dans la file d'attente et a ete copie dans "msg".
  // Si aucun message n'a ete trouve, renvoie RTI_FALSE.
  Boolean searchMessage(TypeNetworkMessage type_msg,
			FederateHandle numeroFedere,
			NetworkMessage *msg);
};

}
}

#endif

// $Id: Communications.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
