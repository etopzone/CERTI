// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libcerti
//
// CERTI-libcerti is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libcerti is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: SecurityServer.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "SecurityServer.hh"

namespace certi {

// -----------------
// -- ADominatesB --
// -----------------

Boolean  SecurityServer::dominates(SecurityLevelID A,
				       SecurityLevelID B)
{
  if((A == B) ||(B == PublicLevelID))
    return RTI_TRUE;
  else
    return RTI_FALSE;
}



// ---------------------------
// -- CanFederateAccessData --
// ---------------------------

Boolean  SecurityServer::
canFederateAccessData(FederateHandle    theFederate,
		       SecurityLevelID   theDataLevelID)
{
  Socket          *FederateSocket;
  SecureTCPSocket *SecureSocket;

  const char       *FederateName;
  SecurityLevelID   FederateLevel;

  // 1- Get the socket of this federate
  //(Killed federate/link will throw an RTIinternalError)

  try {
    FederateSocket = getSocketLink(theFederate);
  } catch(RTIinternalError &e) {
    return dominates(PublicLevelID, theDataLevelID);
  }

  // 2- Check if it is a secure socket

  if(FederateSocket->getClass() != SOCKET_TYPE_S_TCP)
    // If not, all federate are at Public Level.
    return dominates(PublicLevelID, theDataLevelID);

  // 3- If yes, retrieve federate principal name.

  SecureSocket  =(SecureTCPSocket *) FederateSocket;

  FederateName  = SecureSocket->getPeerName();

  // 4- Retrieve Federate level

  FederateLevel = getLevel(FederateName);

  return dominates(FederateLevel, theDataLevelID);
}


// ---------------------
// -- SecurityServer --
// ---------------------

SecurityServer::
SecurityServer(SocketServer    *theRTIGServer,
		 AuditFile       *theAuditServer,
		 FederationHandle  theFederation)
  : List <SecurityLevel *>()
{
  MyFederation      = theFederation;
  RTIG_SocketServer = theRTIGServer;
  Audit             = theAuditServer;

  if((MyFederation == 0) ||
(RTIG_SocketServer == NULL) ||
(Audit == NULL))
    throw RTIinternalError();
}


// ---------------------
// -- SecurityServer --
// ---------------------

SecurityServer::~SecurityServer()
{
  SecurityLevel *Level;

  while(lg > 0) {
    Level = Ieme(1);
    Supprimer(1);
    delete Level;
  }
}

// ----------------------
// -- getLevel --
// ----------------------

SecurityLevelID  SecurityServer::getLevel(const char *theFederate)
{
  return FedLevelList.getLevel(theFederate);
}


// ------------------------
// -- GetLevelIDWithName --
// ------------------------

SecurityLevelID  SecurityServer::
getLevelIDWithName(SecurityLevelName theName)
{
  int               i;
  SecurityLevel   *StoredLevel = NULL;

  if(lg == 0)
    insertPublicLevel();

  if((theName == NULL) ||(strlen(theName) > MAX_USER_TAG_LENGTH))
    throw RTIinternalError("Security Level Name null or too long.");

  for(i = 1; i <= lg; i++) {
    StoredLevel = Ieme(i);
    if(strcmp(StoredLevel->Name, theName) == 0)
      return StoredLevel->LevelID;
  }

  // Level not Found
  LastLevelID ++;
  StoredLevel = new SecurityLevel(theName, LastLevelID);
  Inserer(lg + 1, StoredLevel);

  return LastLevelID;
}



// -----------------------
// -- InsertPublicLevel --
// -----------------------

void SecurityServer::insertPublicLevel(void)
{
  SecurityLevel  *PublicLevel;

  PublicLevel = new SecurityLevel(PublicLevelName, PublicLevelID);
  assert(PublicLevel != NULL);

  Inserer(1, PublicLevel);

  LastLevelID = PublicLevelID;
}


// ----------------------
// -- RegisterFederate --
// ----------------------

void SecurityServer::registerFederate(FederateName      theFederate,
					SecurityLevelID   theLevelID)
{
  FedLevelList.addFederate(theFederate, theLevelID);
}

}

// $Id: SecurityServer.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $

