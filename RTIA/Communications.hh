// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: Communications.hh,v 3.4 2003/02/17 09:17:03 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_COMMUNICATIONS_HH
#define _CERTI_COMMUNICATIONS_HH

// Project
#include <config.h>
#include "baseTypes.hh"
#include "RTItypes.hh"
#include "NetworkMessage.hh"
#include "Message.hh"
#include "SocketUN.hh"
#include "SecureTCPSocket.hh"
#include "SocketUDP.hh"
#include "PrettyDebug.hh"

// Standard libraries
#include <fstream>
#include <iostream>
#include <list>
using std::ifstream ;
using std::ios ;
using std::cout ;
using std::endl ;
using std::list ;

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

    //! Send a message to RTIG.
    void sendMessage(NetworkMessage *Msg);
    void sendUN(Message *Msg);
    void receiveUN(Message *Msg);
    void readMessage(int&, NetworkMessage *, Message *);
    void requestFederateService(Message *req, Message *rep);
    unsigned long getAddress(void);
    unsigned int getPort(void);
    void waitMessage(NetworkMessage *msg,
                     TypeNetworkMessage type_msg,
                     FederateHandle numeroFedere);

private:
    list<NetworkMessage *> waitingList ;

    Boolean searchMessage(TypeNetworkMessage type_msg,
                          FederateHandle numeroFedere,
                          NetworkMessage *msg);
};

}} // namespace certi/rtia

#endif // _CERTI_COMMUNICATIONS_HH

// $Id: Communications.hh,v 3.4 2003/02/17 09:17:03 breholee Exp $
