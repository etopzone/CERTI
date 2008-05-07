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
// $Id: Files.cc,v 3.10 2008/05/07 15:37:38 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Files.hh"

using std::list ;

namespace certi {
namespace rtia {

// ----------------------------------------------------------------------------
//! Returns logical time from first message in TSO list.
void
Queues::nextTsoDate(bool &found, FederationTime &time)
{
    NetworkMessage *msg_buffer ;

    if (tsos.empty()) {
        found = false ;
        time = -1.0 ;
    }
    else {
        msg_buffer = tsos.front();
        found = true ;
        time = msg_buffer->getDate();
    }
}

// ----------------------------------------------------------------------------
/*! Give all the commands to the federate (en invoquant les services
  "RTI Initiated" du federe).
*/
NetworkMessage *
Queues::giveCommandMessage(bool &msg_donne, bool &msg_restant)
{
    NetworkMessage *msg ;

    msg_donne = false ;
    msg_restant = false ;

    if (!commands.empty()) {
        // remove from list but keep pointer to execute ExecuterServiceFedere.
        msg = commands.front();
        commands.pop_front();
        msg_donne = true ;

        if (!commands.empty())
            msg_restant = true ;

        return msg ;
    }
    else return 0 ;
}

// ----------------------------------------------------------------------------
//! Give a FIFO message to federate.
NetworkMessage *
Queues::giveFifoMessage(bool &msg_donne, bool &msg_restant)
{
    NetworkMessage *msg_tampon ;

    msg_donne = false ;
    msg_restant = false ;

    if (!fifos.empty()) {
        // remove from list but keep pointer to execute ExecuterServiceFedere.
        msg_tampon = fifos.front();
        fifos.pop_front();
        msg_donne = true ;

        if (!fifos.empty())
            msg_restant = true ;

        return msg_tampon ;
    }
    else return 0 ;
}

// ----------------------------------------------------------------------------
/*! 'heure_logique' is the minimum value between current LBTS and current
  time
*/
NetworkMessage *
Queues::giveTsoMessage(FederationTime heure_logique,
                       bool &msg_donne,
                       bool &msg_restant)
{
    NetworkMessage *buffer_msg = NULL ;

    msg_donne = false ;
    msg_restant = false ;

    if (!tsos.empty()) {
        buffer_msg = tsos.front();
        if (buffer_msg->getDate() <= heure_logique) {
            // remove from list but keep pointer to execute
            // ExecuterServiceFedere.
            tsos.pop_front();
            msg_donne = true ;

            // Test if next TSO message can be sent.
            if (!tsos.empty()) {
                NetworkMessage *buffer_msg2 ;
                buffer_msg2 = tsos.front();

                if (buffer_msg2->getDate() <= heure_logique)
                    msg_restant = true ;
            }
            return buffer_msg ;
        }
        else return NULL;
    }
    else return NULL;
}

// ----------------------------------------------------------------------------
/*! Insert a message with a command (ex: requestPause) to the beginning of
  command list.
*/
void
Queues::insertBeginCommand(NetworkMessage *msg)
{
    commands.push_front(msg);
}

// ----------------------------------------------------------------------------
//! Insert a message with a command at the end of command list.
void
Queues::insertLastCommand(NetworkMessage *msg)
{
    commands.push_back(msg);
}

// ----------------------------------------------------------------------------
//! Insert a message to end FIFO list.
void
Queues::insertFifoMessage(NetworkMessage *msg)
{
    fifos.push_back(msg);
}

// ----------------------------------------------------------------------------
//! TSO list is sorted by message logical time.
void
Queues::insertTsoMessage(NetworkMessage *msg)
{
    if (tsos.empty()) {
        tsos.push_front(msg);
        return ;
    }
    else {
        list<NetworkMessage *>::iterator i ;
        for (i = tsos.begin(); i != tsos.end(); i++) {
            // stricly greater because we want to place new message behind
            // older ones with same logical time and thus keep receive order
            // in list.
            if ((*i)->getDate() > msg->getDate()) {
                tsos.insert(i, msg);
                return ;
            }
        }
    }
    tsos.push_back(msg);
}

}} // namespaces

// $Id: Files.cc,v 3.10 2008/05/07 15:37:38 erk Exp $
