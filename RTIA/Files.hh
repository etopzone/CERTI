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
// $Id: Files.hh,v 3.6 2003/10/20 12:05:50 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIA_FILES_HH
#define _CERTI_RTIA_FILES_HH

#include "FederationManagement.hh"
#include "ObjectManagement.hh"
#include "NetworkMessage.hh"

#include <list>
#include <stdlib.h>

namespace certi {
namespace rtia {

class FederationManagement ;
class ObjectManagement ;

class Queues
{
public:
    Queues();
    ~Queues();

    FederationManagement *fm ;
    ObjectManagement *om ;

    // File FIFO(First In First Out, or Receive Order)
    void insertFifoMessage(NetworkMessage *msg);
    NetworkMessage *giveFifoMessage(Boolean &msg_donne, Boolean &msg_restant);

    // File TSO(Time Stamp Order)
    void insertTsoMessage(NetworkMessage *msg);
    NetworkMessage *giveTsoMessage(FederationTime heure_logique,
                                   Boolean &msg_donne,
                                   Boolean &msg_restant);
    void nextTsoDate(Boolean &trouve, FederationTime &heure_logique);

    // File Commandes(ex: requestPause)
    void insertBeginCommand(NetworkMessage *msg);
    void insertLastCommand(NetworkMessage *msg);
    NetworkMessage *giveCommandMessage(Boolean &msg_donne,
                                       Boolean &msg_restant);

private:
    // Attributes
    std::list<NetworkMessage *> fifos ; //!< FIFO list.
    std::list<NetworkMessage *> tsos ; //!< TSO list.
    std::list<NetworkMessage *> commands ; //!< commands list.

    // Call a service on the federate.
    void executeFederateService(NetworkMessage *);
};

}} // namespace certi/rtia

#endif // _CERTI_RTIA_FILES_HH

// $Id: Files.hh,v 3.6 2003/10/20 12:05:50 breholee Exp $
