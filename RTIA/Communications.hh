// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: Communications.hh,v 3.13 2008/05/29 12:20:32 rousse Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_COMMUNICATIONS_HH
#define _CERTI_COMMUNICATIONS_HH

#include "certi.hh"
#include "NetworkMessage.hh"
#include "Message.hh"
#include "SocketUN.hh"
#include "SecureTCPSocket.hh"
#include "SocketUDP.hh"
#ifdef FEDERATION_USES_MULTICAST
#include "SocketMC.hh"
#endif

#include <list>

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

    //! Send a message to RTIG.
    void sendMessage(NetworkMessage *Msg);
    void sendUN(Message *Msg);
    void receiveUN(Message *Msg);
    void readMessage(int&, NetworkMessage **, Message **, struct timeval *);
    void requestFederateService(Message *req);
    unsigned long getAddress();
    unsigned int getPort();
    /**
     * Wait for a message coming from RTIG and return
     * when received.
     * @param[in] type_msg, expected message type,
     * @param[in] numeroFedere, federate which sent the message, 0 if indifferent.
     * @return the pointer to new awaited message
     */
    NetworkMessage* waitMessage(NetworkMessage::Type type_msg,
                                FederateHandle numeroFedere);
    MessageBuffer msgBufSend, msgBufReceive ;

private:
	/**
	 * this is the wait list of message 
	 * already received from RTIG
	 * but not dispatched message.
	 */
    std::list<NetworkMessage *> waitingList ;

    bool searchMessage(NetworkMessage::Type type_msg,
		       FederateHandle numeroFedere,
		       NetworkMessage **msg);
};

}} // namespace certi/rtia

#endif // _CERTI_COMMUNICATIONS_HH

// $Id: Communications.hh,v 3.13 2008/05/29 12:20:32 rousse Exp $
