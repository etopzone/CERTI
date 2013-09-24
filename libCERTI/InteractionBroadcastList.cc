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

using std::list ;

namespace certi {

static PrettyDebug D("INTBROADCASTLIST", "(broadcas) - ");
static PrettyDebug G("GENDOC",__FILE__) ;

// ----------------------------------------------------------------------------
/*! Add a federate to the list. If it was not present in the list, a new line
  is added and its state is set as bsNotSub. Then if the Federate has not
  been sent a message for this interaction, the line's state is set to
  bsWaiting.
*/
void
InteractionBroadcastList::addFederate(FederateHandle federate)
{
    // 1. Is there already a line in the list for this Federate?
    InteractionBroadcastLine * line = getLineWithFederate(federate);

    // If NO, add a new one, in the bsWaiting State.
    if (line == 0) {
        line = new InteractionBroadcastLine(federate,
                                            InteractionBroadcastLine::waiting);
        lines.push_front(line);
        D.Out(pdRegister, "Adding new line in list for Federate %d.", federate);
    }
    else
        D.Out(pdTrace, "Message already sent to federate %d.", federate);
}

// ----------------------------------------------------------------------------
/*! theMsg must have been allocated, and will be destroyed by the destructor.
  theMsg->NumeroFedere is added to the list, and its state is set as "Sent".
*/
InteractionBroadcastList::InteractionBroadcastList(NM_Receive_Interaction *theMsg)
{

    G.Out(pdGendoc,"enter InteractionBroadcastList::InteractionBroadcastList");

    if (theMsg == 0)
        throw RTIinternalError("Null Broadcast Message.");

    message = theMsg ;

    // Add reference of the sender(so it does not receive its own message).
    if (message->getFederate() != 0) {
        InteractionBroadcastLine *firstLine ;
        firstLine =
            new InteractionBroadcastLine(message->getFederate(),
                                         InteractionBroadcastLine::sent);
        lines.push_front(firstLine);
    }

    G.Out(pdGendoc,"exit InteractionBroadcastList::InteractionBroadcastList");

}

// ----------------------------------------------------------------------------
//! Free all structures, including Message.
InteractionBroadcastList::~InteractionBroadcastList()
{
    clear();
}

// ----------------------------------------------------------------------------
//! Empty the list so it can reused(like the destructor).
void
InteractionBroadcastList::clear()
{
    delete message ;
    message = NULL ;

    while (!lines.empty()) {
        delete lines.front();
        lines.pop_front();
    }

    D.Out(pdTerm, "List is now empty.");
}

// ----------------------------------------------------------------------------
//! Return the line of the list describing federate 'federate', or 0.
InteractionBroadcastLine*
InteractionBroadcastList::getLineWithFederate(FederateHandle federate)
{
    list<InteractionBroadcastLine *>::iterator i ;
    for (i = lines.begin(); i != lines.end(); i++) {
        if ((*i)->federate == federate)
            return (*i);
    }

    return 0 ;
}

// ----------------------------------------------------------------------------
/*! IMPORTANT: Before calling this method, be sure to set the
  Message->NumeroFederation handle.

  Broadcast the message to all the Federate in the bsWaiting state, and then
  set their state to bsSent.
*/
void
InteractionBroadcastList::sendPendingMessage(SecurityServer *server)
{

    G.Out(pdGendoc,"enter InteractionBroadcastList::sendPendingMessage");

    list<InteractionBroadcastLine *>::iterator i ;
    for (i = lines.begin(); i != lines.end(); i++) {
        // If federate is waiting for a message.
        if ((*i)->state == InteractionBroadcastLine::waiting) {

            // 1. Send message to federate.
            D.Out(pdProtocol, "Broadcasting message to Federate %d.",
                  (*i)->federate);

            Socket *socket = 0 ;
            try {
#ifdef HLA_USES_UDP
                socket = server->getSocketLink((*i)->federate, BEST_EFFORT);
#else
                socket = server->getSocketLink((*i)->federate);
#endif

                G.Out(pdGendoc,"sendPendingMessage===>write");

                message->send(socket,NM_msgBufSend);
            }
            catch (RTIinternalError &e) {
                D.Out(pdExcept,
                      "Reference to a killed Federate while broadcasting.");
            }
            catch (NetworkError &e) {
                D.Out(pdExcept, "Network error while broadcasting, ignoring.");
            }

            // 2. Mark federate as having received the message.
            (*i)->state = InteractionBroadcastLine::sent ;
        }
        else
            D.Out(pdProtocol, "No message sent to Federate %d.",
                  (*i)->federate);
    }

    G.Out(pdGendoc,"exit InteractionBroadcastList::sendPendingMessage");

}

} // namespace certi
