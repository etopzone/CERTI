// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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
// ----------------------------------------------------------------------------

#include "PrettyDebug.hh"
#include "SocketServer.hh"

using std::list;

namespace certi {
static PrettyDebug G("GENDOC", __FILE__);

// ----------------------------------------------------------------------------
/** This method is called when the RTIG wants to initialize its
 *  FD_SET before doing a select. It will add all open socket to the set.
 *  \return the highest file descriptor in the FD_SET
 */
int SocketServer::addToFDSet(fd_set* select_fdset)
{
    int fd_max = 0;

    list<SocketTuple*>::iterator i;
    for (i = begin(); i != end(); ++i) {
        if ((*i)->ReliableLink != NULL) {
            int fd = (*i)->ReliableLink->returnSocket();
            FD_SET(fd, select_fdset);
            fd_max = fd > fd_max ? fd : fd_max;
        }
    }
    return fd_max;
}

void SocketServer::checkMessage(long socket_number, NetworkMessage* message) const
{
    // G.Out(pdGendoc,"enter SocketServer::checkMessage");

    if ((message->getFederation() == 0) && (message->getFederate() == 0)) {
        // G.Out(pdGendoc,"exit  SocketServer::checkMessage on return");
        return;
    }

    Socket* socket;
    try {
        socket = getSocketLink(FederationHandle(message->getFederation()), message->getFederate());
    }
    catch (Exception& e) {
        // BUG: Should put a line in the Audit.
        throw SecurityError("Message for federation <" + std::to_string(message->getFederation())
                            + "> has a unknown origin federate <"
                            + std::to_string(message->getFederate())
                            + ">.");
    }

    if (socket->returnSocket() != socket_number) {
        // BUG: Should put a line in the Audit.
        throw SecurityError("Message has a forged origin.");
    }
    // G.Out(pdGendoc,"exit  SocketServer::checkMessage");
}

void SocketServer::close(long socket, FederationHandle& federation_referenced, FederateHandle& federate_referenced)
{
    federation_referenced = FederationHandle(0);
    federate_referenced = 0;

    // It may throw RTIinternalError.
    SocketTuple* tuple = getWithSocket(socket);

    federation_referenced = tuple->Federation;
    federate_referenced = tuple->Federate;

    // If the Tuple had no references, remove it, else just delete the socket.
    // Also, if no federate (no Join)
    if ((!tuple->Federation.isValid()) && tuple->Federate != 0) {
        list<SocketTuple*>::iterator i;
        list<SocketTuple*>::iterator tmp;
        for (i = begin(); i != end(); ++i) {
            if (((*i)->ReliableLink != NULL) && ((*i)->ReliableLink->returnSocket() == socket)) {
                delete (*i);
                i = erase(i); // i is dereferenced.
                //tmp = erase(i); // i is dereferenced.
                //i = tmp-- ; // loop will increment i ;
            }
        }
    }
    else {
        tuple->ReliableLink->close();
        tuple->BestEffortLink->close();

        delete tuple->ReliableLink;
        delete tuple->BestEffortLink;

        tuple->ReliableLink = NULL;
        tuple->BestEffortLink = NULL;
    }
}

SocketServer::SocketServer(SocketTCP* tcp_socket, SocketUDP* udp_socket) : list<SocketTuple*>()
{
    if (tcp_socket == NULL)
        throw RTIinternalError("");

    ServerSocketTCP = tcp_socket;
    ServerSocketUDP = udp_socket;
}

SocketServer::~SocketServer()
{
    // Deleting remaining tuples.
    while (!list<SocketTuple*>::empty()) {
        delete front();
        pop_front();
    }
}

SocketTuple::SocketTuple(Socket* tcp_link) : Federation(0), Federate(0)
{
    if (tcp_link != NULL)
        ReliableLink = (SocketTCP*) tcp_link;
    else
        throw RTIinternalError("Null socket");

    BestEffortLink = new SocketUDP();
}

SocketTuple::~SocketTuple()
{
    if (ReliableLink != NULL) {
        ReliableLink->close();
        delete ReliableLink;
        ReliableLink = NULL;
    }
    if (BestEffortLink != NULL) {
        BestEffortLink->close();
        delete BestEffortLink;
        BestEffortLink = NULL;
    }
}

Socket* SocketServer::getActiveSocket(fd_set* select_fdset) const
{
    list<SocketTuple*>::const_iterator i;
    for (i = begin(); i != end(); ++i) {
        if (((*i)->ReliableLink != NULL) && (FD_ISSET((*i)->ReliableLink->returnSocket(), select_fdset)))
            return (*i)->ReliableLink;
    }

    return NULL;
}

Socket*
SocketServer::getSocketLink(FederationHandle the_federation, FederateHandle the_federate, TransportType the_type) const
{
    // G.Out(pdGendoc,"enter SocketServer::getSocketLink");
    // It may throw FederateNotExecutionMember
    SocketTuple* tuple = getWithReferences(the_federation, the_federate);

    if (the_type == RELIABLE) {
        if (tuple->ReliableLink == 0) {
            return NULL;
        }
        //throw RTIinternalError("Reference to a killed Federate.");
        return tuple->ReliableLink;
    }
    else {
        if (tuple->BestEffortLink == 0) {
            return NULL;
        }
        //throw RTIinternalError("Reference to a killed Federate.");
        return tuple->BestEffortLink;
    }
    // G.Out(pdGendoc,"exit  SocketServer::getSocketLink without return");
}

SocketTuple* SocketServer::getWithReferences(FederationHandle the_federation, FederateHandle the_federate) const
{
    list<SocketTuple*>::const_iterator i;
    for (i = begin(); i != end(); ++i) {
        if (((*i)->Federation == the_federation) && ((*i)->Federate == the_federate))
            return (*i);
    }

    throw FederateNotExecutionMember("Federate handle " + std::to_string(the_federate)
                                     + " is not a member of Federation "
                                     + std::to_string(the_federation.get()));
}

FederateHandle SocketServer::getFederateFromSocket(FederationHandle the_federation, Socket* socket) const
{
    for (const auto& tuple : *this) {
        if ((tuple->Federation == the_federation)
            && (tuple->BestEffortLink == socket || tuple->ReliableLink == socket)) {
            return tuple->Federate;
        }
    }

    throw RTIinternalError("Federate not found.");
}

SocketTuple* SocketServer::getWithSocket(long socket_descriptor) const
{
    list<SocketTuple*>::const_iterator i;
    for (i = begin(); i != end(); ++i) {
        if (((*i)->ReliableLink != NULL) && ((*i)->ReliableLink->returnSocket() == socket_descriptor))
            return (*i);
        if (((*i)->BestEffortLink != NULL) && ((*i)->BestEffortLink->returnSocket() == socket_descriptor))
            return (*i);
    }

    throw RTIinternalError("Socket not found.");
}

void SocketServer::open()
{
#ifdef WITH_GSSAPI
    SecureTCPSocket* newLink = new SecureTCPSocket();
#else
    SocketTCP* newLink = new SocketTCP();
#endif

    if (newLink == NULL)
        throw RTIinternalError("Could not allocate new socket.");

    newLink->accept(ServerSocketTCP);

    SocketTuple* newTuple = new SocketTuple(newLink);

#ifdef CERTI_RTIG_USE_EPOLL    
    addElementEpoll(newTuple->ReliableLink->returnSocket());
#endif
    if (newTuple == NULL)
        throw RTIinternalError("Could not allocate new tuple.");

    push_front(newTuple);
}

void SocketServer::setReferences(long socket,
                                 FederationHandle federation_reference,
                                 FederateHandle federate_reference,
                                 unsigned long address,
                                 unsigned int port)
{
    // It may throw RTIinternalError if not found.
    SocketTuple* tuple = getWithSocket(socket);

    if ((tuple->Federation.isValid()) || (tuple->Federate != 0))
        // References have already been set once.
        throw RTIinternalError("Socket References have already been set.");

    tuple->Federation = federation_reference;
    tuple->Federate = federate_reference;
    tuple->BestEffortLink->attach(ServerSocketUDP->returnSocket(), address, port);
}

#ifdef CERTI_RTIG_USE_POLL
void SocketServer::constructPollList()
{
	struct pollfd pfd;
	std::memset(&pfd, 0, sizeof(pfd));
    list<SocketTuple*>::iterator i;
    for (i = begin(); i != end(); ++i) {
        if ((*i)->ReliableLink != NULL) {
            int fd = (*i)->ReliableLink->returnSocket();
            pfd.fd = fd;
            pfd.events = POLLIN;
            _SocketVector.push_back(pfd);
            //std::cout << "constructPollList called !!! socket " << pfd.fd << std::endl;
        }
    }
}

Socket* SocketServer::getSocketFromFileDescriptor(int fd)
{
    list<SocketTuple*>::const_iterator i;
    for (i = begin(); i != end(); ++i) {
        if (((*i)->ReliableLink != NULL) && ((*i)->ReliableLink->returnSocket()== fd))
            return (*i)->ReliableLink;
    }

    return NULL;
}
#endif

#ifdef CERTI_RTIG_USE_EPOLL
void SocketServer::constructEpollList()
{
	struct epoll_event ev;
    list<SocketTuple*>::iterator i;
    for (i = begin(); i != end(); ++i) {
        if ((*i)->ReliableLink != NULL) {
            int fd = (*i)->ReliableLink->returnSocket();
            ev.data.fd = fd;
            ev.events = EPOLLIN;
            epoll_ctl(_Epollfd, EPOLL_CTL_ADD, fd, &ev);
			// We should handle error here ...
			// errExit("epoll_ctl");
        }
    }
}
Socket* SocketServer::getSocketFromFileDescriptor(int fd)
{
    list<SocketTuple*>::const_iterator i;
    for (i = begin(); i != end(); ++i) {
        if (((*i)->ReliableLink != NULL) && ((*i)->ReliableLink->returnSocket()== fd))
            return (*i)->ReliableLink;
    }

    return NULL;
}
#endif
}
