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
// $Id: SocketServer.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

// $Id
// ---------------------------------------------------------------------------
// Liste dynamique mettant en relation un couple(Federation, Federe)
// avec une Socket TCP. Sont decritent les classes : SocketTuple
//(l'element de la liste) SocketServer(la liste au niveau du RTIG)
// CFederationSocketServer(l'interface de la liste precedente au
// niveau de la federation et de ses objets, qui contient en plus des
// fonctionnalites de securite)

#ifndef _CERTI_SOCKET_SERVER_HH
#define _CERTI_SOCKET_SERVER_HH

#include "RTItypes.hh"
#include "List.hh"
#include "Socket.hh"
#include "SocketTCP.hh" 
#include "SocketUDP.hh"

#include "NetworkMessage.hh"

#include "SecurityLevel.hh"
#include "SecureTCPSocket.hh"

namespace certi {

class Socket ;
class SocketTCP ;
class SocketUDP ; 
class NetworkMessage ;

// ------------------------
// -- SocketTuple Class --
// ------------------------

// Element of the SocketServer internal list.

class SocketTuple 
{
public:
  FederationHandle Federation;
  FederateHandle Federate;

  SocketTCP *ReliableLink;
  SocketUDP *BestEffortLink;
 
  SocketTuple(Socket *theTCPLink);
  ~SocketTuple();
};


// -------------------------
// -- SocketServer Class --
// -------------------------

class SocketServer : private List <SocketTuple *>
{
public:

  // --------------------
  // -- Public Methods --
  // --------------------

  SocketServer(SocketTCP *theServerTCPSocket,
	       SocketUDP *theServerUDPSocket, int Port);

  ~SocketServer();

  // --------------------------------
  // -- Connection related methods --
  // --------------------------------

  // Allocate a new SocketTuple by Accepting on the ServerSocket.
  // The SocketTuple references are empty.
  // Throw RTIinternalError in case of a memory allocation problem.
  void open(void)
    throw(RTIinternalError);

  // Close and delete the Socket object whose socket is "Socket",
  // and return the former references associated with this socket in
  // the last two parameters. Those returned references can be
  // used for example to force the Federate out of the Federation.
  // Further call to GetSocket with those references will return a
  // NULL Socket object.
  // Throw RTIinternalError if the socket is not found.
  void close(long Socket, // Provided
	     FederationHandle &FederationReferenced, // Returned
	     FederateHandle &FederateReferenced) // Returned
    throw(RTIinternalError);

  // Change the FederationHandle and the FederateHandle associated with 
  // "Socket". Once the references have been set for a Socket, they can't
  // be changed. References can be zeros(but should not).
  // Throw RTIinternalError if the References have already been set, or
  // if the Socket is not found.
  void setReferences(long Socket,
		     FederationHandle FederationReference,
		     FederateHandle FederateReference,
		     unsigned long Adresse,
		     unsigned int Port)
    throw(RTIinternalError);


  // -----------------------------
  // -- Message related methods --
  // -----------------------------

  // Check if 'message' coming from socket link 'Socket' has a valid
  // Federate field, that is, the Federate number linked to the socket is
  // the same as the Federate Number specified in the message.
  // If not, throw SecurityError.
  void checkMessage(long socket, NetworkMessage *message)
    throw(SecurityError);

  // --------------------------
  // -- RTIG related methods --
  // --------------------------

  // This method is called when the RTIG wants to initialize its
  // FD_SET before doing a select. It will add all open socket to the set.
  void addToFDSet(fd_set *SelectFDSet);


  // This method return the first socket object who has been declared
  // active in the fd_set. It can be called several times to get all
  // active sockets.
  Socket *getActiveSocket(fd_set *SelectFDSet);

  // ------------------------------------------
  // -- Message Broadcasting related Methods --
  // ------------------------------------------
 
  // Return the Socket object associated with(theFederation, theFederate).
  // If the couple(Federation, Federate) is not found, a 
  // FederateNotExecutionMember exception is thrown.
  //
  // If the Federate has crashed, it should return a NULL socket object,
  // but this should not happen. In fact, when a Client(Federate) crashes,
  // the RTIG is supposed be remove all references to this federate. That's
  // the reason why a RTIinternalError is thrown in that case.
  Socket *getSocketLink(FederationHandle theFederation,
			FederateHandle theFederate,
			TransportType theType = RELIABLE)
    throw(FederateNotExecutionMember,
	  RTIinternalError);

  SocketTuple *getWithReferences(FederationHandle theFederation,
				 FederateHandle theFederate)
    throw(FederateNotExecutionMember);


private:

  // -----------------------
  // -- Private Attribute --
  // -----------------------

  // The Server socket object(used for Accepts)
  SocketTCP *ServerSocketTCP;
  SocketUDP *ServerSocketUDP;
  int port;


  // ---------------------
  // -- Private Methods --
  // ---------------------

  // SocketTuple *getWithReferences(FederationHandle theFederation,
  // 				 FederateHandle theFederate)
  // throw(FederateNotExecutionMember);

  SocketTuple *getWithSocket(long SocketDescriptor)
    throw(RTIinternalError);

};

}

#endif // _CERTI_SOCKET_SERVER_HH

// $Id: SocketServer.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
