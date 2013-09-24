// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This program is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// ----------------------------------------------------------------------------

#ifndef CERTI_INTERACTION_BROADCAST_LIST_HH
#define CERTI_INTERACTION_BROADCAST_LIST_HH

#include "certi.hh"
#include "NetworkMessage.hh"
#include "SecurityServer.hh"
#include "NM_Classes.hh"

#include <list>

namespace certi {

struct InteractionBroadcastLine
{
public:
    enum State {
        sent, waiting, notSub
    };

    InteractionBroadcastLine(FederateHandle fed, State init = notSub)
        : federate(fed), state(init) { };

    FederateHandle federate ;
    State state ;
};

class InteractionBroadcastList
{
public:
    InteractionBroadcastList(NM_Receive_Interaction *theMsg);
    ~InteractionBroadcastList();

    void clear();
    void addFederate(FederateHandle theFederate);
    void sendPendingMessage(SecurityServer *Server);

    /** 
     * The Message to be broadcasted. This message must be allocated before
     * calling the constructor of the class, be is deleted by the destructor.
     */
    NM_Receive_Interaction *message ;

private:
    InteractionBroadcastLine *getLineWithFederate(FederateHandle theFederate);
    /* The message buffer used to send Network messages */
    MessageBuffer NM_msgBufSend;
    std::list<InteractionBroadcastLine *> lines ;
};

}

#endif // CERTI_INTERACTION_BROADCAST_LIST_HH
