// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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
// ----------------------------------------------------------------------------

#ifndef _CERTI_COMMUNICATIONS_HH
#define _CERTI_COMMUNICATIONS_HH

#include <list>

#include <include/certi.hh>

#include <libCERTI/Message.hh>
#include <libCERTI/NetworkMessage.hh>
#include <libCERTI/SocketTCP.hh>
#include <libCERTI/SocketUDP.hh>
#include <libCERTI/SocketUN.hh>
#ifdef FEDERATION_USES_MULTICAST
#include <libCERTI/SocketMC.hh>
#endif

namespace certi {
namespace rtia {

/** The communication class is an abstraction
 * to be used by RTIA and RTIG in order to send/receive
 * CERTI internal messages.
 */
class Communications {
public:
    
    enum class ReadResult { Invalid, FromNetwork, FromFederate, Timeout };
    
    Communications(int RTIA_port, int RTIA_fd);
    ~Communications();

    /**
     * Send a message to RTIG.
     * @param[in] Msg the message to be sent
     */
    void sendMessage(NetworkMessage* Msg);

    /** Send a message to RTIA.
     * FIXME Historically those messages were sent to Unix Socket thus the 'UN'.
     * @param[in] Msg the message to be sent
     */
    void sendUN(Message* Msg);

    /**
     * Receive a message from RTIA.
     * FIXME Historically those messages were sent to Unix Socket thus the 'UN'.
     * The message is allocated during receive using a virtual constructor.
     * It should be destroyed by the caller.
     */
    Message* receiveUN();

    /**
     * Read some message from either network (RTIG/RTIA) or federate (RTIA/Federate).
     * Returns the actual source in the 1st parameter (RTIG=>1 federate=>2)
     * @param[out] n result of the operation
     * @param[out] msg_reseau pointer to pointer to network message
     * @param[out] msg pointer to pointer to message
     * @param[out] timeout time to wait
     */
    void readMessage(Communications::ReadResult& n, NetworkMessage** msg_reseau, Message** msg, struct timeval* timeout);

    void requestFederateService(Message* req);
    unsigned long getAddress();
    unsigned int getPort();

    /**
     * Wait for a message coming from RTIG and return when received.
     * @param[in] type_msg expected message type,
     * @param[in] numeroFedere federate which sent the message, 0 if indifferent.
     * @return the pointer to new awaited message
     */
    NetworkMessage* waitMessage(NetworkMessage::Type type_msg, FederateHandle numeroFedere);

protected:
    MessageBuffer NM_msgBufSend;
    MessageBuffer msgBufSend;

    SocketUN* socketUN;
#ifdef FEDERATION_USES_MULTICAST
    SocketMC* socketMC;
#endif
    SocketTCP* socketTCP;
    SocketUDP* socketUDP;

private:
    /** This is the wait list of message already received from RTIG
     * but not yet dispatched. We need a wait list because we may
     * receive messages while waiting for some particular [other] messages.
     */
    std::list<NetworkMessage*> waitingList;

    /** Returns true if a 'type_msg' message coming from federate
     * 'numeroFedere' (or any other federate if numeroFedere == 0) was in
     * the queue and was copied in 'msg'. If no such message is found,
     * returns RTI_FALSE.
     */
    bool searchMessage(NetworkMessage::Type type_msg, FederateHandle numeroFedere, NetworkMessage** msg);
};
}
} // namespace certi/rtia

#endif // _CERTI_COMMUNICATIONS_HH

// $Id: Communications.hh,v 3.19 2010/02/27 16:53:36 erk Exp $
