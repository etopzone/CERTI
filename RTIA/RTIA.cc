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

#include <config.h>

#include <assert.h>
#include <limits.h>
#include <math.h>

#include "RTIA.hh"

namespace certi {
namespace rtia {

RTIA::RTIA(int RTIA_port, int RTIA_fd)
    : comm{RTIA_port, RTIA_fd}
    , fm{&comm}
    , om{&comm, &fm, &my_root_object}
    , owm{&comm, &fm}
    , dm{&comm, &fm, &my_root_object}
    , tm{&comm, &queues, &fm, &dm, &om, &owm}
    , ddm{&my_root_object, &fm, &comm}
{
    fm.setTm(&tm);
    queues.fm = &fm;
    queues.dm = &dm;
    om.tm = &tm;
}

RTIA::~RTIA()
{
    /* 
     * FIXME Erk
     * this is may be a design issue
     * see  https://savannah.nongnu.org/patch/?6937
     */
    fm.resignFederationExecutionForTermination();

    /* delete objects in reverse order just like generated destructor would have done */
    delete my_clock;
} /* end of ~RTIA() */

void RTIA::displayStatistics()
{
    if (stat.display()) {
        std::cout << stat;
    }
}

void RTIA::execute()
{
    enum class RespType { Invalid, FromNetwork, FromFederate, Timeout };

    while (fm.getConnectionState() != FederationManagement::ConnectionState::Ended) {
        /* readMessage call will allocate EITHER a Network Message or a Message
         * Network Message will come from a virtual constructor call
         * Message will come from a "simple" constructor call
         */
        Message* msgFromFederate{nullptr};
        NetworkMessage* msgFromRTIG{nullptr};
        Communications::ReadResult result{Communications::ReadResult::Invalid};

        try {
            switch (tm._tick_state) {
            case TimeManagement::NO_TICK:
                // tick() is not active: block until RTIA or federate message comes
                comm.readMessage(result, &msgFromRTIG, &msgFromFederate, NULL);
                break;

            case TimeManagement::TICK_BLOCKING:
                // blocking tick() waits for an event to come: block until RTIA or federate message comes, or timeout expires
                if (tm._tick_timeout != std::numeric_limits<double>::infinity() && tm._tick_timeout < LONG_MAX) {
                    struct timeval timev;
                    timev.tv_sec = int(tm._tick_timeout);
                    timev.tv_usec = int((tm._tick_timeout - timev.tv_sec) * 1000000.0);

                    comm.readMessage(result, &msgFromRTIG, &msgFromFederate, &timev);
                }
                else {
                    comm.readMessage(result, &msgFromRTIG, &msgFromFederate, NULL);
                }
                break;

            case TimeManagement::TICK_CALLBACK:
            case TimeManagement::TICK_RETURN:
                // tick() waits until a federate callback finishes: block until federate message comes RTIA messages are queued in a system queue
                comm.readMessage(result, NULL, &msgFromFederate, NULL);
                break;

            default:
                assert(false);
            }

            // timev is undefined after select()
        }
        catch (NetworkSignal& e) {
            fm.setConnectionState(FederationManagement::ConnectionState::Ended);
            result = Communications::ReadResult::Invalid;
            delete msgFromFederate;
            delete msgFromRTIG;
        }

        switch (result) {
        case Communications::ReadResult::Invalid:
            break;
        case Communications::ReadResult::FromNetwork:
            processNetworkMessage(msgFromRTIG);
            if (tm._tick_state == TimeManagement::TICK_BLOCKING) {
                processOngoingTick();
            }
            break;
        case Communications::ReadResult::FromFederate:
            processFederateRequest(msgFromFederate);
            break;
        case Communications::ReadResult::Timeout:
            if (tm._tick_state == TimeManagement::TICK_BLOCKING) {
                // stop the ongoing tick() operation
                tm._tick_state = TimeManagement::TICK_RETURN;
                processOngoingTick();
            }
            break;
        default:
            assert(false);
        }
    }
}
}
}
