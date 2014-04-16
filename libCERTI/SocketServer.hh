// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: SocketServer.hh,v 3.11 2014/04/16 12:24:01 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_SOCKET_SERVER_HH
#define _CERTI_SOCKET_SERVER_HH

#include "certi.hh"
#include "Socket.hh"
#include "SocketTCP.hh"
#include "SocketUDP.hh"
#include "NetworkMessage.hh"
#include "SecurityLevel.hh"
#include "SecureTCPSocket.hh"

#include <list>

namespace certi {

// ----------------------------------------------------------------------------
//! Element of the SocketServer internal list.
class SocketTuple
{
public:
    Handle Federation ;
    FederateHandle Federate ;

    SocketTCP *ReliableLink ;
    SocketUDP *BestEffortLink ;

    SocketTuple(Socket *theTCPLink);
    ~SocketTuple();
};

// ----------------------------------------------------------------------------
/*! Liste dynamique mettant en relation un couple(Federation, Federe)
  avec une Socket TCP. Sont decritent les classes : SocketTuple
  (l'element de la liste) SocketServer(la liste au niveau du RTIG)
  CFederationSocketServer (l'interface de la liste precedente au
  niveau de la federation et de ses objets, qui contient en plus des
  fonctionnalites de securite)
*/
class CERTI_EXPORT SocketServer : private std::list<SocketTuple *>
{
public:
    SocketServer(SocketTCP *tcp_socket,
                 SocketUDP *udp_socket);

    ~SocketServer();

    // --------------------------------
    // -- Connection related methods --
    // --------------------------------
    void open()
        throw (RTIinternalError);

    void close(long socket, // Provided
               Handle &federation_referenced, // Returned
               FederateHandle &federate_referenced) // Returned
        throw (RTIinternalError);

    void setReferences(long the_socket,
                       Handle federation_reference,
                       FederateHandle federate_reference,
                       unsigned long the_address,
                       unsigned int the_port)
        throw (RTIinternalError);

    // -----------------------------
    // -- Message related methods --
    // -----------------------------
    void checkMessage(long socket, NetworkMessage *message) const
        throw (SecurityError);

    // --------------------------
    // -- RTIG related methods --
    // --------------------------
    int addToFDSet(fd_set *select_fdset);
    Socket *getActiveSocket(fd_set *select_fdset) const ;

    // ------------------------------------------
    // -- Message Broadcasting related Methods --
    // ------------------------------------------
    Socket *getSocketLink(Handle the_federation,
                          FederateHandle the_federate,
                          TransportType the_type = RELIABLE) const
        throw (FederateNotExecutionMember, RTIinternalError);

    SocketTuple *getWithReferences(Handle the_federation,
                                   FederateHandle the_federate) const
        throw (FederateNotExecutionMember);

private:
    // The Server socket object(used for Accepts)
    SocketTCP *ServerSocketTCP ;
    SocketUDP *ServerSocketUDP ;

    // ---------------------
    // -- Private Methods --
    // ---------------------
    SocketTuple *getWithSocket(long socket_descriptor) const
        throw (RTIinternalError);
};

} // namespace certi

#endif // _CERTI_SOCKET_SERVER_HH

// $Id: SocketServer.hh,v 3.11 2014/04/16 12:24:01 erk Exp $
