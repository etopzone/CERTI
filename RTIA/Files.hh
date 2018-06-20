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

#ifndef CERTI_RTIA_FILES_HH
#define CERTI_RTIA_FILES_HH

#include <list>
#include <stdlib.h>

#include "DeclarationManagement.hh"
#include "FederationManagement.hh"
#include "ObjectManagement.hh"
#include <libCERTI/NetworkMessage.hh>

namespace certi {
namespace rtia {

class FederationManagement;
class DeclarationManagement;
class ObjectManagement;

class Queues {
public:
    // File FIFO(First In First Out, or Receive Order)
    /// Insert a message to end FIFO list.
    void insertFifoMessage(NetworkMessage* msg);

    /// Give a FIFO message to federate.
    NetworkMessage* giveFifoMessage(bool& gave_msg, bool& has_remaining_msg);

    // File TSO(Time Stamp Order)
    /// TSO list is sorted by message logical time.
    void insertTsoMessage(NetworkMessage* msg);

    /** 'heure_logique' is the minimum value between current LBTS and current
     * time
     */
    NetworkMessage* giveTsoMessage(FederationTime logical_time, bool& gave_msg, bool& has_remaining_msg);

    /// Returns logical time from first message in TSO list.
    void nextTsoDate(bool& found, FederationTime& logical_time);

    // File Commandes(ex: requestPause)
    /** Insert a message with a command (ex: requestPause) to the beginning of
     *  command list.
     */
    void insertBeginCommand(NetworkMessage* msg);
    /// Insert a message with a command at the end of command list.
    void insertLastCommand(NetworkMessage* msg);

    /** Give all the commands to the federate (en invoquant les services
     *  "RTI Initiated" du federe).
     */
    NetworkMessage* giveCommandMessage(bool& gave_msg, bool& has_remaining_msg);

    FederationManagement* fm;
    DeclarationManagement* dm;
    ObjectManagement* om;

private:
    // Attributes
    std::list<NetworkMessage*> fifos; /// FIFO list.
    std::list<NetworkMessage*> tsos; /// TSO list.
    std::list<NetworkMessage*> commands; /// commands list.

    /// Call a service on the federate.
    void executeFederateService(NetworkMessage*);
};
}
} // namespace certi/rtia

#endif // CERTI_RTIA_FILES_HH
