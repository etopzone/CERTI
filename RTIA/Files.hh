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
// $Id: Files.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef GFILES_HH
#define GFILES_HH

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include "FederationManagement.hh"
#include "ObjectManagement.hh"
#include "NetworkMessage.hh"
#include "Message.hh"
#include "List.hh"

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

      NetworkMessage *giveFifoMessage(Boolean &msg_donne,
				      Boolean &msg_restant);

      // File TSO(Time Stamp Order)

      void insertTsoMessage(NetworkMessage *msg);

      NetworkMessage *giveTsoMessage(FederationTime heure_logique,
				     Boolean &msg_donne,
				     Boolean &msg_restant);

      void nextTsoDate(Boolean &trouve,
		       FederationTime &heure_logique);

      // File Commandes(ex: requestPause)

      void insertBeginCommand(NetworkMessage *msg);

      void insertLastCommand(NetworkMessage *msg);

      NetworkMessage *giveCommandMessage(Boolean &msg_donne,
					 Boolean &msg_restant);

    private:

      // ------------------------
      // -- Private Attributes --
      // ------------------------

      // File FIFO
      List <NetworkMessage*> _fileFIFO;

      // File TSO
      List <NetworkMessage*> _fileTSO;

      // File de commandes
      List <NetworkMessage*> _file_commandes;

      // ---------------------
      // -- Private Methods --
      // ---------------------

      // Call a service on the federate.
      void executeFederateService(NetworkMessage *);

    };
  }
}

#endif

// $Id: Files.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
