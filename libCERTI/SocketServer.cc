// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: SocketServer.cc,v 3.21 2014/04/16 12:24:01 erk Exp $
// ----------------------------------------------------------------------------


#include "SocketServer.hh"
#include "PrettyDebug.hh"

using std::list ;

namespace certi {
static PrettyDebug G("GENDOC",__FILE__);
// ----------------------------------------------------------------------------
/** This method is called when the RTIG wants to initialize its
    FD_SET before doing a select. It will add all open socket to the set.
    \return the highest file descriptor in the FD_SET
*/
int
SocketServer::addToFDSet(fd_set *select_fdset)
{
    int fd_max = 0 ;

    list<SocketTuple *>::iterator i ;
    for (i = begin(); i != end(); i++) {
        if ((*i)->ReliableLink != NULL) {
	    int fd = (*i)->ReliableLink->returnSocket();
            FD_SET(fd, select_fdset);
	    fd_max = fd > fd_max ? fd : fd_max ;
	}
    }
    return fd_max ;
}

// ----------------------------------------------------------------------------
/*! Check if 'message' coming from socket link 'Socket' has a valid
  Federate field, that is, the Federate number linked to the socket is
  the same as the Federate Number specified in the message.
  If not, throw SecurityError.
*/
void
SocketServer::checkMessage(long socket_number, NetworkMessage *message) const
    throw (SecurityError)
{
    // G.Out(pdGendoc,"enter SocketServer::checkMessage");

    if ((message->getFederation() == 0) && (message->getFederate() == 0))
        {
        // G.Out(pdGendoc,"exit  SocketServer::checkMessage on return");
        return ;
        }

    Socket *socket ;
    try {
        socket = getSocketLink(message->getFederation(),
                               message->getFederate());
    }
    catch (Exception &e) {
        // BUG: Should put a line in the Audit.
        throw SecurityError(stringize() << "Message for federation <" <<  message->getFederation() << "> has a unknown origin federate <"<<message->getFederate() <<">.");
    }

    if (socket->returnSocket() != socket_number) {
        // BUG: Should put a line in the Audit.
        throw SecurityError("Message has a forged origin.");
    }
    // G.Out(pdGendoc,"exit  SocketServer::checkMessage");
}

// ----------------------------------------------------------------------------
/*! Close and delete the Socket object whose socket is "Socket",
  and return the former references associated with this socket in
  the last two parameters. Those returned references can be
  used for example to force the Federate out of the Federation.
  Further call to GetSocket with those references will return a
  NULL Socket object.
  Throw RTIinternalError if the socket is not found.
*/
void
SocketServer::close(long socket,
                    Handle &federation_referenced,
                    FederateHandle &federate_referenced)
    throw (RTIinternalError)
{
    federation_referenced = 0 ;
    federate_referenced = 0 ;

    // It may throw RTIinternalError.
    SocketTuple *tuple = getWithSocket(socket);

    federation_referenced = tuple->Federation ;
    federate_referenced = tuple->Federate ;

    // If the Tuple had no references, remove it, else just delete the socket.
    // Also, if no federate (no Join)
    if (tuple->Federation == 0 && tuple->Federate != 0) {
        list<SocketTuple *>::iterator i ;
        list<SocketTuple *>::iterator tmp ;
        for (i = begin(); i != end(); i++) {
            if (((*i)->ReliableLink != NULL) &&
                ((*i)->ReliableLink->returnSocket() == socket)) {
                delete (*i);
                i= erase(i); // i is dereferenced.
                //tmp = erase(i); // i is dereferenced.
                //i = tmp-- ; // loop will increment i ;
            }
        }
    }
    else {
        tuple->ReliableLink->close();
        tuple->BestEffortLink->close();

        delete tuple->ReliableLink ;
        delete tuple->BestEffortLink ;

        tuple->ReliableLink = NULL ;
        tuple->BestEffortLink = NULL ;
    }
}

// ----------------------------------------------------------------------------
//! socketServer.
SocketServer::SocketServer(SocketTCP *tcp_socket,
                           SocketUDP *udp_socket)
    : list<SocketTuple *>()
{
    if (tcp_socket == NULL)
        throw RTIinternalError("");

    ServerSocketTCP = tcp_socket ;
    ServerSocketUDP = udp_socket ;
}

// ----------------------------------------------------------------------------
//! Destructor (frees tuple list).
SocketServer::~SocketServer()
{
    // Deleting remaining tuples.
    while (!list<SocketTuple *>::empty()) {
        delete front();
        pop_front();
    }
}

// ----------------------------------------------------------------------------
//! SocketTuple constructor.
SocketTuple::SocketTuple(Socket *tcp_link)
    : Federation(0), Federate(0)
{
    if (tcp_link != NULL)
        ReliableLink = (SocketTCP *)tcp_link ;
    else
        throw RTIinternalError("Null socket");

    BestEffortLink = new SocketUDP();
}

// ----------------------------------------------------------------------------
//! SocketTuple destructor.
SocketTuple::~SocketTuple()
{
    if (ReliableLink != NULL) {
        ReliableLink->close();
        delete ReliableLink ;
        ReliableLink = NULL ;
    }
    if (BestEffortLink != NULL) {
        BestEffortLink->close();
        delete BestEffortLink ;
        BestEffortLink = NULL ;
    }
}

// ----------------------------------------------------------------------------
/*! This method return the first socket object who has been declared active
  in the fd_set. It can be called several times to get all active sockets.
*/
Socket *
SocketServer::getActiveSocket(fd_set *select_fdset) const
{
    list<SocketTuple *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        if (((*i)->ReliableLink != NULL) &&
            (FD_ISSET((*i)->ReliableLink->returnSocket(), select_fdset)))
            return (*i)->ReliableLink ;
    }

    return NULL ;
}

// ----------------------------------------------------------------------------
/*! Return the Socket object associated with(theFederation, theFederate). If
  the couple(Federation, Federate) is not found, a
  FederateNotExecutionMember exception is thrown.

  If the Federate has crashed, it should return a NULL socket object, but
  this should not happen. In fact, when a Client(Federate) crashes, the
  RTIG is supposed be remove all references to this federate. That's the
  reason why a RTIinternalError is thrown in that case.

  JYR : sorry but we return NULL (avoid rtig crash) because development needed
*/
Socket*
SocketServer::getSocketLink(Handle the_federation,
                            FederateHandle the_federate,
                            TransportType the_type) const
    throw (FederateNotExecutionMember, RTIinternalError)
{
    // G.Out(pdGendoc,"enter SocketServer::getSocketLink");
    // It may throw FederateNotExecutionMember
    SocketTuple *tuple = getWithReferences(the_federation, the_federate);

    if (the_type == RELIABLE) {
        if (tuple->ReliableLink == 0)
            {
            return NULL ;
            }
            //throw RTIinternalError("Reference to a killed Federate.");
        return tuple->ReliableLink ;
    }
    else {
        if (tuple->BestEffortLink == 0)
            {
            return NULL ;
            }
            //throw RTIinternalError("Reference to a killed Federate.");
        return tuple->BestEffortLink ;
    }
    // G.Out(pdGendoc,"exit  SocketServer::getSocketLink without return");
}

// ----------------------------------------------------------------------------
//! getWithReferences.
SocketTuple *
SocketServer::getWithReferences(Handle the_federation,
                                FederateHandle the_federate) const
    throw (FederateNotExecutionMember)
{
    list<SocketTuple *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        if (((*i)->Federation == the_federation) &&
            ((*i)->Federate == the_federate))
            return (*i);
    }

    throw FederateNotExecutionMember(certi::stringize() << "Federate handle" << the_federate << "is not a member of Federation" << the_federation);
}

// ----------------------------------------------------------------------------
//! getWithSocket (private).
SocketTuple *
SocketServer::getWithSocket(long socket_descriptor) const
    throw (RTIinternalError)
{
    list<SocketTuple *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        if (((*i)->ReliableLink != NULL) &&
            ((*i)->ReliableLink->returnSocket() == socket_descriptor))
            return (*i);
        if (((*i)->BestEffortLink != NULL) &&
            ((*i)->BestEffortLink->returnSocket() == socket_descriptor))
            return (*i);
    }

    throw RTIinternalError("Socket not found.");
}

// ----------------------------------------------------------------------------
/*! Allocate a new SocketTuple by Accepting on the ServerSocket.
  The SocketTuple references are empty.
  Throw RTIinternalError in case of a memory allocation problem.
*/
void
SocketServer::open()
    throw (RTIinternalError)
{
#ifdef WITH_GSSAPI
    SecureTCPSocket *newLink = new SecureTCPSocket();
#else
    SocketTCP *newLink = new SocketTCP();
#endif

    if (newLink == NULL)
        throw RTIinternalError("Could not allocate new socket.");

    newLink->accept(ServerSocketTCP);

    SocketTuple *newTuple = new SocketTuple(newLink);

    if (newTuple == NULL)
        throw RTIinternalError("Could not allocate new tuple.");

    push_front(newTuple);
}

// ----------------------------------------------------------------------------
/*! Change the FederationHandle and the FederateHandle associated with
  "socket". Once the references have been set for a Socket, they can't
  be changed. References can be zeros(but should not).
  Throw RTIinternalError if the References have already been set, or
  if the Socket is not found.
*/
void
SocketServer::setReferences(long socket,
                            Handle federation_reference,
                            FederateHandle federate_reference,
                            unsigned long address,
                            unsigned int port)
    throw (RTIinternalError)
{
    // It may throw RTIinternalError if not found.
    SocketTuple *tuple = getWithSocket(socket);

    if ((tuple->Federation != 0) || (tuple->Federate != 0))
        // References have already been set once.
        throw RTIinternalError("Socket References have already been set.");

    tuple->Federation = federation_reference ;
    tuple->Federate = federate_reference ;
    tuple->BestEffortLink->attach(ServerSocketUDP->returnSocket(), address,
                                  port);
}

}

// $Id: SocketServer.cc,v 3.21 2014/04/16 12:24:01 erk Exp $
