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
// $Id: RTIA.cc,v 3.18 2008/04/23 12:55:05 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIA.hh"

#include <assert.h>
#include <math.h>

namespace certi {
namespace rtia {

static pdCDebug D("RTIA", "(RTIA) ");

// Tableau des messages pouvant etre recus du RTIG

#define MSG_RTIG_MAX 18

// ----------------------------------------------------------------------------
//! RTIA constructor.
RTIA::RTIA()
{
    // No SocketServer is passed to the RootObject.
    rootObject = new RootObject(NULL);

    comm = new Communications();
    queues = new Queues ;
    fm = new FederationManagement(comm,&stat);
    om = new ObjectManagement(comm, fm, rootObject);
    owm = new OwnershipManagement(comm, fm);
    dm = new DeclarationManagement(comm, fm, rootObject);
    tm = new TimeManagement(comm, queues, fm, om, owm);
    ddm = new DataDistribution(rootObject, fm, comm);

    fm->tm = tm ;
    queues->fm = fm ;
    om->tm = tm ;
}

// ----------------------------------------------------------------------------
// RTIA Destructor
RTIA::~RTIA()
{
    // BUG: TCP link destroyed ?

     // Remove temporary file (if not yet done)
     if ( fm->_FEDid != NULL)
        {
        if ( fm->_FEDid[0] != '\0' )
           {
           // If RTIA end (abort ?) before join don't remove file if not temporary
           // temporary file name begins with _RT ( yes, but...)
           if ( fm->_FEDid[0] != '_' || fm->_FEDid[1] != 'R' || fm->_FEDid[2] != 'T')
              {
               std::cout<<"** W ** I don't remove file "<<fm->_FEDid<<std::endl;
              }
           else
              {
              std::cout<<"*** W ** Removing temporary file "<<fm->_FEDid<<" on RTIA stop."<<std::endl;
              std::remove(fm->_FEDid);
              }
           fm->_FEDid[0] = '\0' ;
           }
        }
    delete tm ;
    delete dm ;
    delete om ;
    delete fm ;
    delete queues ;
    delete comm ;
    delete ddm ;
    delete rootObject ;
}

// ----------------------------------------------------------------------------
// displayStatistics
void
RTIA::displayStatistics()
{
    if (stat.display()) {
        cout << stat ;
    }
}

// ----------------------------------------------------------------------------
//! RTIA mainloop.
/*! Messages allocated for reading data exchange between RTIA and federate/RTIG
  are freed by 'processFederateRequest' or 'processNetworkMessage'.
*/
void
RTIA::execute()
{
    Message *msg_un ;
    NetworkMessage *msg_tcp_udp ;
    int n ;

    while (!fm->_fin_execution) {

        msg_tcp_udp = new NetworkMessage ;
        msg_un = new Message ;

        try {
            comm->readMessage(n, msg_tcp_udp, msg_un, NULL);
        }
        catch (NetworkSignal) {
            fm->_fin_execution = true ;
            n = 0 ;
            delete msg_un ;
            delete msg_tcp_udp ;
        }

        switch (n) {
          case 0:
            delete msg_un ;
            delete msg_tcp_udp ;
            break ;
          case 1:
            processNetworkMessage(msg_tcp_udp);
            delete msg_un ;
            break ;
          case 2:
            processFederateRequest(msg_un);
            delete msg_tcp_udp ;
            break ;
          case 3: // timeout
            delete msg_un ;
            delete msg_tcp_udp ;
            break ;
          default:
            assert(false);
        }

        // special case, blocking tick
        while (!fm->_fin_execution && tm->_blocking_tick) {
	    // read a message from the rtig
            // same code is reused, but only the case 1 should match
            msg_tcp_udp = new NetworkMessage ;
            msg_un = new Message ;

            try {
                if (tm->_tick_timeout != std::numeric_limits<double>::infinity() &&
                   tm->_tick_timeout < LONG_MAX)
                {
                    struct timeval timev;
                    timev.tv_sec = int(tm->_tick_timeout);
                    timev.tv_usec = int((tm->_tick_timeout-timev.tv_sec)*1000000.0);

                    comm->readMessage(n, msg_tcp_udp, msg_un, &timev);
                }
                else
                    comm->readMessage(n, msg_tcp_udp, msg_un, NULL);

                /* timev is undefined after select() */
            }
            catch (NetworkSignal) {
                fm->_fin_execution = true ;
                n = 0 ;
                delete msg_un ;
                delete msg_tcp_udp ;
            }

            switch (n) {
              case 0:
                delete msg_un;
                delete msg_tcp_udp;
                break ;
              case 1:
                processNetworkMessage(msg_tcp_udp) ;  // could authorize a callbak
                delete msg_un ;
                // may have reset tm->_blocking_tick
                processOngoingTick();
                break ;
              case 2:
                assert(false);
              case 3: // timeout
                delete msg_un;
                delete msg_tcp_udp;
                // stop the ongoing tick() operation
                tm->_blocking_tick = false;
                processOngoingTick();
                break ;
              default:
                assert(false);
            }
        }   
    }   
}

}} // namespace certi/rtia

// $Id: RTIA.cc,v 3.18 2008/04/23 12:55:05 erk Exp $
