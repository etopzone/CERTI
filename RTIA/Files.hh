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
// $Id: Files.hh,v 3.8 2009/04/08 10:47:17 approx Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_RTIA_FILES_HH
#define CERTI_RTIA_FILES_HH

#include "FederationManagement.hh"
#include "DeclarationManagement.hh"
#include "ObjectManagement.hh"
#include "NetworkMessage.hh"

#include <list>
#include <stdlib.h>

namespace certi {
namespace rtia {

class FederationManagement ;
class DeclarationManagement ;
class ObjectManagement ;

class Queues
{
public:
    // File FIFO(First In First Out, or Receive Order)
    void insertFifoMessage(NetworkMessage *msg);
    NetworkMessage *giveFifoMessage(bool &, bool &);

    // File TSO(Time Stamp Order)
    void insertTsoMessage(NetworkMessage *msg);
    NetworkMessage *giveTsoMessage(FederationTime heure_logique,
                                   bool &msg_donne,
                                   bool &msg_restant);
    void nextTsoDate(bool &trouve, FederationTime &heure_logique);

    // File Commandes(ex: requestPause)
    void insertBeginCommand(NetworkMessage *msg);
    void insertLastCommand(NetworkMessage *msg);
    NetworkMessage *giveCommandMessage(bool &msg_donne, bool &msg_restant);

    FederationManagement *fm ;
    DeclarationManagement *dm ;
    ObjectManagement *om ;

private:
    // Attributes
    std::list<NetworkMessage *> fifos ; //!< FIFO list.
    std::list<NetworkMessage *> tsos ; //!< TSO list.
    std::list<NetworkMessage *> commands ; //!< commands list.

    // Call a service on the federate.
    void executeFederateService(NetworkMessage *);
};

}} // namespace certi/rtia

#endif // CERTI_RTIA_FILES_HH

// $Id: Files.hh,v 3.8 2009/04/08 10:47:17 approx Exp $
