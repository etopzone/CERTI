// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI
//
// CERTI is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// $Id: Files.hh,v 3.2 2002/12/11 12:34:13 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_FILES_HH
#define _CERTI_FILES_HH

#include <config.h>

#include <list>
using std::list;

#include <stdlib.h>

#include "FederationManagement.hh"
#include "ObjectManagement.hh"
#include "NetworkMessage.hh"

namespace certi {
namespace rtia {

class FederationManagement;
class ObjectManagement;

class Queues
{
public:
    FederationManagement *_GF;
    ObjectManagement *_GO;

    Queues();
    ~Queues();

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
    list<NetworkMessage *> fifos ;      //!< FIFO list.
    list<NetworkMessage *> tsos ;       //!< TSO list.
    list<NetworkMessage *> commands ;   //!< commands list.

    // Call a service on the federate.
    void executeFederateService(NetworkMessage *);
};

}}

#endif // _CERTI_FILES_HH

// $Id: Files.hh,v 3.2 2002/12/11 12:34:13 breholee Exp $
