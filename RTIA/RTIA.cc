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
// $Id: RTIA.cc,v 3.36 2011/07/11 11:17:24 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIA.hh"
#include <assert.h>
#include <math.h>
#include <limits.h>

namespace certi {
namespace rtia {

static PrettyDebug D("RTIA", "(RTIA) ");

RTIA::RTIA(int RTIA_port, int RTIA_fd) {

    clock = libhla::clock::Clock::getBestClock();

    // No SocketServer is passed to the RootObject (RTIA use case)
	// socket server are passed to RootObject iff we are in RTIG.
    rootObject = new RootObject(NULL);

    comm   = new Communications(RTIA_port, RTIA_fd);
    queues = new Queues ;
    fm     = new FederationManagement(comm,&stat);
    om     = new ObjectManagement(comm, fm, rootObject);
    owm    = new OwnershipManagement(comm, fm);
    dm     = new DeclarationManagement(comm, fm, rootObject);
    tm     = new TimeManagement(comm, queues, fm, dm, om, owm);
    ddm    = new DataDistribution(rootObject, fm, comm);

    fm->tm     = tm ;
    queues->fm = fm ;
    queues->dm = dm ;
    om->tm     = tm ;
} /* end of RTIA(int RTIA_port, int RTIA_fd) */


RTIA::~RTIA() {
    /* 
     * FIXME Erk
     * this is may be a design issue
     * see  https://savannah.nongnu.org/patch/?6937
     */
    fm->resignFederationExecutionForTermination();
     
     /* delete objects in reverse order just like generated destructor would have done */
    delete ddm ;
    delete tm ;
    delete dm ;
    delete owm ;
    delete om ;    
    delete fm ;
    delete queues ;
    delete comm ;    
    delete rootObject ;
    delete clock ;
} /* end of ~RTIA() */

// ----------------------------------------------------------------------------
// displayStatistics
void
RTIA::displayStatistics()
{
    if (stat.display()) {
        std::cout << stat ;
    }
}

// ----------------------------------------------------------------------------
void
RTIA::execute() {
    Message        *msgFromFederate;
    NetworkMessage *msgFromRTIG;
    int n ;

    while (fm->_connection_state != FederationManagement::CONNECTION_FIN) {
       
        /* 
         * readMessage call will allocate EITHER a Network Message or a Message 
         *   Network Message will come from a virtual constructor call
         *   Message will come from a "simple" constructor call
         */
    	msgFromFederate      = NULL;
    	msgFromRTIG = NULL;
        try {
            switch (tm->_tick_state) {
              case TimeManagement::NO_TICK:
                /* tick() is not active:
                 *   block until RTIA or federate message comes
                 */
                comm->readMessage(n, &msgFromRTIG, &msgFromFederate, NULL);
                break;

              case TimeManagement::TICK_BLOCKING:
                /* blocking tick() waits for an event to come:
                 *   block until RTIA or federate message comes, or timeout expires
                 */
                if (tm->_tick_timeout != std::numeric_limits<double>::infinity() &&
                    tm->_tick_timeout < LONG_MAX) {

                    struct timeval timev;
                    timev.tv_sec = int(tm->_tick_timeout);
                    timev.tv_usec = int((tm->_tick_timeout-timev.tv_sec)*1000000.0);

                    comm->readMessage(n, &msgFromRTIG, &msgFromFederate, &timev);
                }
                else
                    comm->readMessage(n, &msgFromRTIG, &msgFromFederate, NULL);
                break;

              case TimeManagement::TICK_CALLBACK:
              case TimeManagement::TICK_RETURN:
                /* tick() waits until a federate callback finishes:
                 *   block until federate message comes
                 *   RTIA messages are queued in a system queue
                 */
                comm->readMessage(n, NULL, &msgFromFederate, NULL);
                break;

              default:
                assert(false);
            }

            /* timev is undefined after select() */
        }
        catch (NetworkSignal&) {
            fm->_connection_state = FederationManagement::CONNECTION_FIN;
            n = 0 ;
            delete msgFromFederate ;
            delete msgFromRTIG ;
        }

        switch (n) {
          case 0:
            break ;
          case 1:
            processNetworkMessage(msgFromRTIG);
            if (tm->_tick_state == TimeManagement::TICK_BLOCKING) {
                processOngoingTick();
            }
            break ;
          case 2:
            processFederateRequest(msgFromFederate);
            break ;
          case 3: // timeout
            if (tm->_tick_state == TimeManagement::TICK_BLOCKING) {
                // stop the ongoing tick() operation
                tm->_tick_state = TimeManagement::TICK_RETURN;
                processOngoingTick();
            }
            break ;
          default:
            assert(false);
        }
    }   
} /* end of execute() */

}} // namespace certi/rtia

// $Id: RTIA.cc,v 3.36 2011/07/11 11:17:24 erk Exp $
