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
// $Id: Communications.hh,v 3.3 2003/01/16 17:55:33 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_COMMUNICATIONS_HH
#define _CERTI_COMMUNICATIONS_HH

#include <config.h>

#include <fstream>
using std::ifstream;
using std::ios;
#include <iostream>
using std::cout;
using std::endl;
#include <list>
using std::list;

#include "PrettyDebug.hh"
#include "baseTypes.hh"
#include "RTItypes.hh"
#include "NetworkMessage.hh"
#include "Message.hh"
#include "SocketUN.hh"
#include "SecureTCPSocket.hh"
#include "SocketUDP.hh"

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
      
  Communications(void);
  ~Communications(void);

  // ---------------------
  // -- Network Methods --
  // ---------------------

  //! Send a message to RTIG.
  void sendMessage(NetworkMessage *Msg)
  { Msg->write((SecureTCPSocket *) this); };

  void sendUN(Message *Msg)
  { Msg->write((SocketUN *) this); };

  void receiveUN(Message *Msg)
  { Msg->read((SocketUN *) this); };

  void readMessage(int&, NetworkMessage *, Message *);

  void waitMessage(NetworkMessage *msg, 
		   TypeNetworkMessage type_msg, 
		   FederateHandle numeroFedere);
 
  void requestFederateService(Message *req, Message *rep);
  unsigned long getAddress(void);
  unsigned int getPort(void);

private:
  list<NetworkMessage *> waitingList;

  Boolean searchMessage(TypeNetworkMessage type_msg,
			FederateHandle numeroFedere,
			NetworkMessage *msg);
};

}}

#endif // _CERTI_COMMUNICATIONS_HH

// $Id: Communications.hh,v 3.3 2003/01/16 17:55:33 breholee Exp $
