// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: RTIA.hh,v 3.21 2010/03/21 17:10:11 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIA_HH
#define _CERTI_RTIA_HH

#include <libHLA/Clock.hh>

#include "Communications.hh"
#include "DataDistribution.hh"
#include "DeclarationManagement.hh"
#include "FederationManagement.hh"
#include "ObjectManagement.hh"
#include "OwnershipManagement.hh"
#include "Statistics.hh"
#include "TimeManagement.hh"

namespace certi {

namespace rtia {

/** The RTI Ambassador class.
 * This class is the Federate to RTI proxy class which is used
 * to communication to/from the RTI.
 * In current CERTI implementation RTIA is a seperate process
 * which is created (forked) when the RTIambassador's federate
 * constructor is called.
 * RTIA is a reactive process which process Message from federate
 * and NetworkMessage from RTIG.
 */
class RTIA {
public:
    /** RTIA constructor.
     * @param[in] RTIA_port the TCP port used
     * @param[in] RTIA_fd the file descriptor
     */
    RTIA(int RTIA_port, int RTIA_fd);

    ~RTIA();

    /** The RTIA reactive [endless] main loop.
     * Messages allocated for reading data exchange between RTIA and federate/RTIG
     * are freed by 'processFederateRequest' or 'processNetworkMessage'.
     */
    void execute();
    void displayStatistics();

private:
    void saveAndRestoreStatus(Message::Type type) throw(SaveInProgress, RestoreInProgress);

    /// Process one message from RTIG (i.e. a NetworkMessage).
    void processNetworkMessage(NetworkMessage* request);

    /** Process a service request coming from the Federate (i.e. a Message).
     * An answer in sent inside the call.
     * @param[in,out] request the message request coming from the federate
     *                the message is destroyed before return
     */
    void processFederateRequest(Message* request);

    /** The method called by processFederateRequest in order to
     * process requests in the FederationManagement::CONNECTION_PRELUDE state.
     * @param[in] request the federate request message
     * @param[out] answer answer message to be sent back to the federate
     */
    void initFederateProcessing(Message* request, Message* answer);

    /** The method called by processFederateRequest in order to
     * effectively process the request and build the appropriate
     * answer.
     * @param[in] request the federate request message
     * @param[out] answer answer message to be sent back to the federate
     * @param[out] e exception raised (if any).
     */
    void chooseFederateProcessing(Message* request, Message* answer, Exception::Type& e);

    /// RTIA processes the TICK_REQUEST.
    void processOngoingTick();

    RootObject my_root_object{};
    libhla::clock::Clock* my_clock{libhla::clock::Clock::getBestClock()};
    Statistics stat;
    Queues queues{};

    Communications comm;
    FederationManagement fm;
    ObjectManagement om;
    OwnershipManagement owm;
    DeclarationManagement dm;
    TimeManagement tm;
    DataDistribution ddm;
    
    std::string rti_version;
};
}
} // namespace certi

#endif // _CERTI_RTIA_HH

// $Id: RTIA.hh,v 3.21 2010/03/21 17:10:11 erk Exp $
