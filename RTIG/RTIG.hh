// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: RTIG.hh,v 3.3 2003/01/14 17:17:33 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_RTIG_HH
#define _CERTI_RTIG_HH

#include <config.h>

#include <iostream>
using std::cout;
using std::endl;

#include <csignal>

#include "baseTypes.hh"
#include "RTItypes.hh"
#include "NetworkMessage.hh"
#include "SecureTCPSocket.hh"
#include "SocketMC.hh"
#include "List.hh"

#include "SocketServer.hh"
#include "FederationsList.hh"
#include "AuditFile.hh"

#include "PrettyDebug.hh"

#include "cmdline.h"

namespace certi {
namespace rtig {

/*! This class manages the rtig server. Each time a message is received, it is
    processed by a RTI_processing.cc module.

    These modules called "processXXX" are divided into two parts :
      - one part for treating the received message.
      - another part generating and sending back a response.
*/
class RTIG
{
  // ATTRIBUTES ----------------------------------------------------------
private:
  int tcpPort ;
  int udpPort ;
  bool terminate ;
  FederationHandle nextFederationHandle;
  SocketTCP tcpSocketServer ;
  SocketUDP udpSocketServer ;
  SocketServer* socketServer ;
  AuditFile* auditServer;
  FederationsList* federations; //!< Manages federations.
 
  // METHODS -------------------------------------------------------------
public:
  RTIG();
  ~RTIG();
 
  void signalHandler(int sig); 
  void execute();
 
private:
  // Both methods return the socket, because it may have been closed & deleted.
  Socket* processIncomingMessage(Socket*);  
  Socket* chooseProcessingMethod(Socket*, NetworkMessage *);

  void openConnection(void);
  void closeConnection(Socket*, bool emergency);

  // Event handlers
  void processCreateFederation(Socket*, NetworkMessage*); 
  void processJoinFederation(Socket*, NetworkMessage*);
  void processResignFederation(FederationHandle, FederateHandle);
  void processDestroyFederation(Socket*, NetworkMessage*);
  void processSetTimeRegulating(NetworkMessage *msg);
  void processSetTimeConstrained(NetworkMessage *msg);
  void processMessageNull(NetworkMessage*);
  void processRequestPause(Socket*, NetworkMessage*);
  void processRequestResume(Socket*, NetworkMessage*);
  void processPublishObjectClass(Socket*, NetworkMessage*);
  void processSubscribeObjectClass(Socket*, NetworkMessage*);
  void processUnpublishObjectClass(Socket*, NetworkMessage*);
  void processUnsubscribeObjectClass(Socket*, NetworkMessage*);
  void processPublishInteractionClass(Socket*, NetworkMessage*);
  void processSubscribeInteractionClass(Socket*, NetworkMessage*);
  void processUnpublishInteractionClass(Socket*, NetworkMessage*);
  void processUnsubscribeInteractionClass(Socket*, NetworkMessage*msg);
  void processRequestId(Socket*, NetworkMessage*);
  void processRegisterObject(Socket*, NetworkMessage*);
  void processUpdateAttributeValues(Socket*, NetworkMessage*);
  void processSendInteraction(Socket*, NetworkMessage*);
  void processDeleteObject(Socket*, NetworkMessage*);
  void processQueryAttributeOwnership(Socket*, NetworkMessage*);
  void processNegotiatedOwnershipDivestiture(Socket*, NetworkMessage*); 
  void processAcquisitionIfAvailable(Socket*, NetworkMessage*);  
  void processUnconditionalDivestiture(Socket*, NetworkMessage*); 
  void processOwnershipAcquisition(Socket*, NetworkMessage*); 
  void processCancelNegotiatedDivestiture(Socket*, NetworkMessage*);
  void processAttributeOwnedByFederate(Socket*, NetworkMessage*);
  void processReleaseResponse(Socket*, NetworkMessage*);
  void processCancelAcquisition(Socket*, NetworkMessage*);       
};

}}

#endif // _CERTI_RTIG_HH

// ---------------------------------------------------------------------------
// $Id: RTIG.hh,v 3.3 2003/01/14 17:17:33 breholee Exp $
