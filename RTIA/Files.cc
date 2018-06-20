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

#include "Files.hh"
#include <config.h>

using std::list;

namespace certi {
namespace rtia {

void Queues::insertFifoMessage(NetworkMessage* msg)
{
    fifos.push_back(msg);
}

NetworkMessage* Queues::giveFifoMessage(bool& gave_msg, bool& has_remaining_msg)
{
    gave_msg = false;
    has_remaining_msg = false;

    if (!fifos.empty()) {
        // remove from list but keep pointer to execute ExecuterServiceFedere.
        auto msg = fifos.front();
        fifos.pop_front();
        gave_msg = true;

        has_remaining_msg = !fifos.empty();

        return msg;
    }

    return nullptr;
}

void Queues::insertTsoMessage(NetworkMessage* msg)
{
    if (tsos.empty()) {
        tsos.push_front(msg);
        return;
    }
    else {
        for (auto i = tsos.begin(); i != tsos.end(); ++i) {
            // stricly greater because we want to place new message behind
            // older ones with same logical time and thus keep receive order
            // in list.
            if ((*i)->getDate() > msg->getDate()) {
                tsos.insert(i, msg);
                return;
            }
        }
    }
    tsos.push_back(msg);
}

NetworkMessage* Queues::giveTsoMessage(FederationTime logical_time, bool& gave_msg, bool& has_remaining_msg)
{
    gave_msg = false;
    has_remaining_msg = false;

    if (!tsos.empty()) {
        auto msg = tsos.front();
        if (msg->getDate() <= logical_time) {
            // remove from list but keep pointer to execute ExecuterServiceFedere.
            tsos.pop_front();
            gave_msg = true;

            // Test if next TSO message can be sent.
            if (!tsos.empty()) {
                auto next_msg = tsos.front();

                has_remaining_msg = (next_msg->getDate() <= logical_time);
            }
            return msg;
        }
    }

    return nullptr;
}

void Queues::nextTsoDate(bool& found, FederationTime& logical_time)
{
    NetworkMessage* msg_buffer;

    if (tsos.empty()) {
        found = false;
        logical_time = -1.0;
    }
    else {
        msg_buffer = tsos.front();
        found = true;
        logical_time = msg_buffer->getDate();
    }
}

void Queues::insertBeginCommand(NetworkMessage* msg)
{
    commands.push_front(msg);
}

void Queues::insertLastCommand(NetworkMessage* msg)
{
    commands.push_back(msg);
}

NetworkMessage* Queues::giveCommandMessage(bool& gave_msg, bool& has_remaining_msg)
{
    gave_msg = false;
    has_remaining_msg = false;

    if (!commands.empty()) {
        // remove from list but keep pointer to execute ExecuterServiceFedere.
        auto msg = commands.front();
        commands.pop_front();
        gave_msg = true;

        has_remaining_msg = !commands.empty();

        return msg;
    }

    return nullptr;
}
}
} // namespaces
