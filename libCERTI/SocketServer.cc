// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: SocketServer.cc,v 3.2 2003/01/15 10:12:29 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "SocketServer.hh"

namespace certi {

// ----------------
// -- AddToFDSet --
// ----------------

void SocketServer::addToFDSet(fd_set *SelectFDSet)
{
  SocketTuple *Tuple;

  for(int i = 1; i <= lg; i++) {
    Tuple = Ieme(i);
    if(Tuple->ReliableLink != NULL)
      FD_SET(Tuple->ReliableLink->returnSocket(), SelectFDSet);
  }
}   
  

// ------------------
// -- CheckMessage --
// ------------------

void
SocketServer::checkMessage(long SocketNumber, NetworkMessage *Message)
  throw(SecurityError)
{
  Socket *socket;

  if ((Message->NumeroFederation == 0) && (Message->NumeroFedere == 0))
    return;

  try {
    socket = getSocketLink(Message->NumeroFederation, Message->NumeroFedere);
  }
  catch(Exception &e) {
    // BUG: Should put a line in the Audit.
    throw SecurityError("Message has a unknown origin.");
  }
  
  if (socket->returnSocket() != SocketNumber) {
    // BUG: Should put a line in the Audit.
    throw SecurityError("Message has a forged origin.");
  }
}


// ---------------------
// -- CloseConnection --
// ---------------------

void
SocketServer::close(long Socket,
                    FederationHandle &FederationReferenced,
                    FederateHandle &FederateReferenced)
  throw(RTIinternalError)
{
  SocketTuple *Tuple = NULL;

  FederationReferenced = 0;
  FederateReferenced   = 0;

  // It may throw RTIinternalError.
  Tuple = getWithSocket(Socket);

  FederationReferenced = Tuple->Federation;
  FederateReferenced   = Tuple->Federate;

  // If the Tuple had no references, remove it, else just delete the socket.
  if(Tuple->Federation == 0)
    for(int i = 1; i <= lg; i++) {
      Tuple = Ieme(i);

      if((Tuple->ReliableLink != NULL) && 
	(Tuple->ReliableLink->returnSocket() == Socket)) {
	Supprimer(i);
	delete Tuple;

      }
    }

  else {
    Tuple->ReliableLink->close();
    Tuple->BestEffortLink->close();
    
    delete   Tuple->ReliableLink;
    delete   Tuple->BestEffortLink;
    Tuple->ReliableLink = NULL;
    Tuple->BestEffortLink = NULL;
  }
}


// -------------------
// -- SocketServer --
// -------------------


SocketServer::SocketServer(SocketTCP *theServerTCPSocket,
			   SocketUDP *theServerUDPSocket, int Port)
{
  if(theServerTCPSocket == NULL)
    throw RTIinternalError();

  ServerSocketTCP = theServerTCPSocket;
  ServerSocketUDP = theServerUDPSocket;
  port = Port;
  
  List <SocketTuple *>();
}



// --------------------
// -- ~SocketServer --
// --------------------


SocketServer::~SocketServer()
{
  SocketTuple *Tuple = NULL;

  // Deleting remaining tuples.
  while(lg > 0) {
    Tuple = Ieme(1);
    Supprimer(1);
    delete Tuple;
  }

}


// ------------------
// -- SocketTuple --
// ------------------

SocketTuple::SocketTuple(Socket *theTCPLink)
{
  Federation = 0;
  Federate   = 0;
  
  if(theTCPLink != NULL)
    ReliableLink =(SocketTCP *)theTCPLink;
  else
    throw RTIinternalError("Null socket");

  BestEffortLink = new SocketUDP();
}


// -------------------
// -- ~SocketTuple --
// -------------------

SocketTuple::~SocketTuple()
{
  if(ReliableLink != NULL) {
    ReliableLink->close();
    delete ReliableLink;
    ReliableLink = NULL;
  }
  if(BestEffortLink != NULL) {
    BestEffortLink->close();
    delete BestEffortLink;
    BestEffortLink = NULL;
  }
}


// ---------------------
// -- GetActiveSocket --
// ---------------------

Socket *SocketServer::getActiveSocket(fd_set *SelectFDSet)
{
  SocketTuple *Tuple = NULL;

  for(int i = 1; i <= lg; i++) {
    Tuple = Ieme(i);
    if((Tuple->ReliableLink != NULL) &&
	(FD_ISSET(Tuple->ReliableLink->returnSocket(), SelectFDSet)))
      return Tuple->ReliableLink;
  }

  return NULL;
}


// -------------------
// -- GetSocketLink --
// -------------------

Socket*
SocketServer::getSocketLink(FederationHandle theFederation,
                            FederateHandle theFederate,
                            TransportType theType)
  throw(FederateNotExecutionMember, RTIinternalError)
{
  SocketTuple *Tuple = NULL;

  // It may throw FederateNotExecutionMember
  Tuple = getWithReferences(theFederation, theFederate);
  if (theType == RELIABLE) {
      if (Tuple->ReliableLink == 0)
          throw RTIinternalError("Reference to a killed Federate.");
      return Tuple->ReliableLink;
  }
  else {
      if (Tuple->BestEffortLink == 0)
          throw RTIinternalError("Reference to a killed Federate.");
      return Tuple->BestEffortLink;
  }
}


// -----------------------
// -- GetWithReferences --(private)
// -----------------------

SocketTuple *SocketServer::getWithReferences(FederationHandle theFederation,
						FederateHandle   theFederate)
  throw(FederateNotExecutionMember)
{
  SocketTuple *Tuple = NULL;
  
  for(int i = 1; i <= lg; i++) {
    Tuple = Ieme(i);
    if((Tuple->Federation == theFederation) &&
	(Tuple->Federate   == theFederate))
      return Tuple;
  }

  throw FederateNotExecutionMember();
}


// -------------------
// -- GetWithSocket --(private)
// -------------------

SocketTuple *SocketServer::getWithSocket(long SocketDescriptor)
  throw(RTIinternalError)
{
  SocketTuple *Tuple = NULL;
  
  for(int i = 1; i <= lg; i++) {
    Tuple = Ieme(i);
    if((Tuple->ReliableLink != NULL) && 
       (Tuple->ReliableLink->returnSocket() == SocketDescriptor))
      return Tuple;
    if((Tuple->BestEffortLink != NULL) && 
       (Tuple->BestEffortLink->returnSocket() == SocketDescriptor))
      return Tuple;
  }

  throw RTIinternalError("Socket not found.");
}


// --------------------
// -- OpenConnection --
// --------------------

void SocketServer::open()
  throw(RTIinternalError)
{
  SecureTCPSocket *NewLink  = NULL;
  SocketTuple     *NewTuple = NULL;

  NewLink = new SecureTCPSocket();

  if(NewLink == NULL)
      throw RTIinternalError("Could not allocate new socket.");

  NewLink->accept(ServerSocketTCP);
  NewTuple = new SocketTuple(NewLink);

  if(NewTuple == NULL)
    throw RTIinternalError("Could not allocate new tuple.");

  Inserer(1, NewTuple);
}


// -------------------
// -- SetReferences --
// -------------------

void
SocketServer::setReferences(long Socket,
                            FederationHandle FederationReference,
                            FederateHandle FederateReference,
                            unsigned long Adresse,
                            unsigned int Port)
    throw(RTIinternalError)
{
  SocketTuple *Tuple = NULL;

  // It may throw RTIinternalError if not found.
  Tuple = getWithSocket(Socket);

  if((Tuple->Federation != 0) ||(Tuple->Federate != 0))
    // References have already been set once.
    throw RTIinternalError("Socket References have already been set.");

  Tuple->Federation = FederationReference;
  Tuple->Federate   = FederateReference;
  Tuple->BestEffortLink->attach(ServerSocketUDP->returnSocket(),
				Adresse, Port);
}

}

// $Id: SocketServer.cc,v 3.2 2003/01/15 10:12:29 breholee Exp $
