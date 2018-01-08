// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
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

#include "InteractionBroadcastList.hh"
#include "PrettyDebug.hh"

#include <include/make_unique.hh>

using std::list;

namespace certi {

static PrettyDebug D("INTBROADCASTLIST", "(broadcas) - ");
static PrettyDebug G("GENDOC", __FILE__);

void InteractionBroadcastList::addFederate(FederateHandle federate)
{
    auto it = my_lines.find(federate);

    if (it == end(my_lines)) {
        my_lines[federate] = State::Waiting;
        Debug(D, pdRegister) << "Adding new line in list for Federate" << federate << std::endl;
    }
    else {
        Debug(D, pdTrace) << "Message already sent to federate " << federate << std::endl;
    }
}

InteractionBroadcastList::InteractionBroadcastList(NM_Receive_Interaction message) : my_message{message}
{
    Debug(G, pdGendoc) << "enter InteractionBroadcastList::InteractionBroadcastList" << std::endl;

    // Add reference of the sender(so it does not receive its own message).
    if (my_message.getFederate() != 0) {
        my_lines[my_message.getFederate()] = State::Sent;
    }

    Debug(G, pdGendoc) << "exit InteractionBroadcastList::InteractionBroadcastList" << std::endl;
}

Responses InteractionBroadcastList::preparePendingMessage(SecurityServer& server)
{
    Debug(G, pdGendoc) << "enter InteractionBroadcastList::preparePendingMessage" << std::endl;

    Responses ret;

    std::vector<Socket*> sockets;
    for (auto& pair : my_lines) {
        if (pair.second == State::Waiting) {
            // 1. Prepare message for federate.
            Debug(D, pdProtocol) << "Broadcasting message to Federate " << pair.first << std::endl;

            try {
#ifdef HLA_USES_UDP
                sockets.push_back(server.getSocketLink(pair.first, BEST_EFFORT));
#else
                sockets.push_back(server.getSocketLink(pair.first));
#endif
            }
            catch (Exception& e) {
                Debug(D, pdExcept) << "Reference to a killed Federate while broadcasting." << std::endl;
            }

            // 2. Mark federate as having received the message.
            pair.second = State::Sent;
        }
        else {
            Debug(D, pdProtocol) << "No message sent to Federate " << pair.first << std::endl;
        }
    }

    ret.emplace_back(sockets, make_unique<NM_Receive_Interaction>(my_message));

    Debug(G, pdGendoc) << "exit  InteractionBroadcastList::preparePendingMessage" << std::endl;

    return ret;
}

NM_Receive_Interaction& InteractionBroadcastList::getMessage()
{
    return my_message;
}

} // namespace certi
