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
// $Id: RTIG.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_RTIG_HH
#define _CERTI_RTIG_HH

#include <config.h>

#include <csignal>

#include "baseTypes.hh"
#include "RTItypes.hh"
#include "NetworkMessage.hh"
#include "SecureTCPSocket.hh" // SecureTCPSocket class
#include "SocketMC.hh"
#include "List.hh"

#include "SocketServer.hh"
#include "FederationsList.hh"
#include "AuditFile.hh"

#include "PrettyDebug.hh"

namespace certi {
namespace rtig {

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
  FederationsList* federations;
 
  // METHODS -------------------------------------------------------------
public:
  RTIG();
  ~RTIG();
 
  void signalHandler(int sig); 
  void execute();
 
private:
  // Both methods return the socket, because it may have been
  // closed & deleted.
  Socket* processIncomingMessage(Socket*);  
  Socket* chooseProcessingMethod(Socket*, NetworkMessage *);

  void openConnection(void);
 
  // If a connection is closed in emergency, KillFederate will be
  // called on ListeFederation to remove all references to this
  // Federate.
  void closeConnection(Socket*, bool emergency);

  // Event handlers
  // FIXME: check for french/bad case names
  void processCreatefederation(Socket*, NetworkMessage*); 
  void processJoinfederation(Socket*, NetworkMessage*);
  void processResignfederation(FederationHandle, FederateHandle);
  void processDestroyfederation(Socket*, NetworkMessage*);
  void processSetTimeRegulating(NetworkMessage*msg);
  void processSetTimeConstrained(NetworkMessage*msg);
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
  void processrequestId(Socket*, NetworkMessage*);
  void processRegisterObject(Socket*, NetworkMessage*);
  void processupdateAttributeValues(Socket*, NetworkMessage*);
  void processsendInteraction(Socket*, NetworkMessage*);
  void processdeleteObject(Socket*, NetworkMessage*);
  void processqueryAttributeOwnership(Socket*, NetworkMessage*);
  void processNegotiatedOwnershipDivestiture(Socket*, NetworkMessage*); 
  void processAcquisitionIfAvailable(Socket*, NetworkMessage*);  
  void processUnconditionalDivestiture(Socket*, NetworkMessage*); 
  void processOwnershipAcquisition(Socket*, NetworkMessage*); 
  void processAnnulerNegotiatedDivestiture(Socket*, NetworkMessage*);
  void processattributeOwnedByFederate(Socket*, NetworkMessage*);
  void processRealeaseResponse(Socket*, NetworkMessage*);
  void processCancelAcquisition(Socket*, NetworkMessage*);       
};

}}

#endif // _CERTI_RTIG_HH

// ---------------------------------------------------------------------------
// $Id: RTIG.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
