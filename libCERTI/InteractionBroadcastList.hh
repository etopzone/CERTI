// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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

#include "MessageEvent.hh"
#include "NM_Classes.hh"
#include "NetworkMessage.hh"
#include "SecurityServer.hh"
#include <include/certi.hh>

#include <unordered_map>

namespace certi {

class InteractionBroadcastList {
public:
    InteractionBroadcastList(NM_Receive_Interaction message);

    ~InteractionBroadcastList() = default;

    void addFederate(FederateHandle theFederate);

    Responses preparePendingMessage(SecurityServer& server);

    NM_Receive_Interaction& getMessage();

private:
    enum class State { Sent, Waiting, NotSub };

    NM_Receive_Interaction my_message;

    std::unordered_map<FederateHandle, State> my_lines;
};
}

#endif // CERTI_INTERACTION_BROADCAST_LIST_HH
