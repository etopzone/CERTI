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
// $Id: RTIA.cc,v 3.13.2.2 2008/04/21 11:29:34 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIA.hh"

#include <assert.h>

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
}

// ----------------------------------------------------------------------------
// RTIA Destructor
RTIA::~RTIA()
{
    // BUG: TCP link destroyed ?

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
    Message        *msg_un;
    NetworkMessage *msg_tcp_udp;
    int n ;

    while (!fm->_fin_execution) {
       
        /* 
         * readMessage call will allocate EITHER a Network Message or a Message 
         *   Network Message will come from a virtual constructor call
         *   Message will come from a "simple" constructor call
         */
    	msg_un      = NULL;
    	msg_tcp_udp = NULL;
        try {        	
            comm->readMessage(n, &msg_tcp_udp, &msg_un);
            assert((msg_un!=NULL) || (msg_tcp_udp!=NULL));
        }
        catch (NetworkSignal) {
            fm->_fin_execution = true ;
            n = 0 ;
            delete msg_un ;
            delete msg_tcp_udp ;
        }

        switch (n) {
          case 0:
            break ;
          case 1:
            processNetworkMessage(msg_tcp_udp);
            break ;
          case 2:
            processFederateRequest(msg_un);            
            break ;
          default:
            assert(false);
        }

        // special case, blocking tick (tick2)
        while (!fm->_fin_execution && tm->_ongoing_tick) {
	        // read a message from the rtig
            // same code is reused, but only the case 1 should match

        	/* NetworkMessage will be allocated by the readMessage call
        	 * We may not get a Message in this call see previous comment
        	 */
            msg_un      = NULL;
            msg_tcp_udp = NULL;
                        
            try {
                comm->readMessage(n, &msg_tcp_udp, &msg_un);
            }
            catch (NetworkSignal) {
                fm->_fin_execution = true ;
                n = 0 ;                
                delete msg_tcp_udp ;
            }

            switch (n) {
              case 0:
                break ;
              case 1:
                processNetworkMessage(msg_tcp_udp) ;  // could authorize a callback
                msg_un = new Message();
                msg_un->type = Message::TICK_REQUEST ;
                msg_un->setBoolean(true) ;
                processFederateRequest(msg_un);  //could reset _ongoing_tick                
                break ;
              case 2:
              default:
                assert(false);
            }
        }   
    }   
}

}} // namespace certi/rtia

// $Id: RTIA.cc,v 3.13.2.2 2008/04/21 11:29:34 erk Exp $
