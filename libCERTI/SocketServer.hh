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
// ----------------------------------------------------------------------------

#ifndef _CERTI_SOCKET_SERVER_HH
#define _CERTI_SOCKET_SERVER_HH

#include "NetworkMessage.hh"
#include "SecureTCPSocket.hh"
#include "SecurityLevel.hh"
#include "Socket.hh"
#include "SocketTCP.hh"
#include "SocketUDP.hh"
#include <include/certi.hh>

#include <list>
#ifdef CERTI_RTIG_USE_POLL
#include <poll.h>
#endif

#ifdef CERTI_RTIG_USE_EPOLL
#include <sys/epoll.h>
#include <fcntl.h>
// it might be equal to Number of federate (??)
#define MAX_CONNECTIONS     5
#endif



namespace certi {

/// Element of the SocketServer internal list.
class SocketTuple {
public:
    FederationHandle Federation;
    FederateHandle Federate;

    SocketTCP* ReliableLink;
    SocketUDP* BestEffortLink;

    SocketTuple(Socket* theTCPLink);
    ~SocketTuple();
};

/** Liste dynamique mettant en relation un couple(Federation, Federe)
 * avec une Socket TCP. Sont decritent les classes : SocketTuple
 * (l'element de la liste) SocketServer(la liste au niveau du RTIG)
 * CFederationSocketServer (l'interface de la liste precedente au
 * niveau de la federation et de ses objets, qui contient en plus des
 * fonctionnalites de securite)
 */
class CERTI_EXPORT SocketServer : private std::list<SocketTuple*> {
public:
    SocketServer(SocketTCP* tcp_socket, SocketUDP* udp_socket);

    /// Destructor frees tuple list.
    ~SocketServer();

    // --------------------------------
    // -- Connection related methods --
    // --------------------------------

    /** Allocate a new SocketTuple by Accepting on the ServerSocket.
     * 
     * The SocketTuple references are empty.
     * Throw RTIinternalError in case of a memory allocation problem.
     */
    void open();

    /** Close and delete the Socket object whose socket is "Socket",
     * and return the former references associated with this socket in
     * the last two parameters. Those returned references can be
     * used for example to force the Federate out of the Federation.
     * Further call to GetSocket with those references will return a
     * NULL Socket object.
     * 
     * Throw RTIinternalError if the socket is not found.
     */
    void close(long socket, // Provided
               FederationHandle& federation_referenced, // Returned
               FederateHandle& federate_referenced); // Returned

    /** Change the FederationHandle and the FederateHandle associated with
     * "socket". Once the references have been set for a Socket, they can't
     * be changed. References can be zeros(but should not).
     * Throw RTIinternalError if the References have already been set, or
     * if the Socket is not found.
     */
    virtual void setReferences(long the_socket,
                               FederationHandle federation_reference,
                               FederateHandle federate_reference,
                               unsigned long the_address,
                               unsigned int the_port);

    // -----------------------------
    // -- Message related methods --
    // -----------------------------

    /** Check if 'message' coming from socket link 'Socket' has a valid
     * Federate field, that is, the Federate number linked to the socket is
     * the same as the Federate Number specified in the message.
     * If not, throw SecurityError.
     */
    void checkMessage(long socket, NetworkMessage* message) const;

    // --------------------------
    // -- RTIG related methods --
    // --------------------------
    int addToFDSet(fd_set* select_fdset);

    /** This method return the first socket object who has been declared active
     * in the fd_set. It can be called several times to get all active sockets.
     */
    Socket* getActiveSocket(fd_set* select_fdset) const;
    
#ifdef CERTI_RTIG_USE_POLL    
    void constructPollList();
    void addElementPollList(struct pollfd pfd)
    {
		_SocketVector.push_back(pfd);
	}
    std::vector<struct pollfd> getSocketVector()
    {
		return _SocketVector;
	}
	void resetSocketVector()
    {
		_SocketVector.resize(0);
	}
	Socket* getSocketFromFileDescriptor(int fd);
#endif

#ifdef CERTI_RTIG_USE_EPOLL   
	void constructEpollList();	
	void createEpollFd()
    {
		_Epollfd = epoll_create( 0xCAFE );
	}
	void addElementEpoll(int fd)
    {
		struct epoll_event ev;
		ev.data.fd = fd;
		ev.events = EPOLLIN;
		epoll_ctl(_Epollfd, EPOLL_CTL_ADD, fd, &ev);
	}
	int getEpollDescriptor()
	{
		return _Epollfd;
	}
	Socket* getSocketFromFileDescriptor(int fd);
#endif
    // ------------------------------------------
    // -- Message Broadcasting related Methods --
    // ------------------------------------------

    /** Return the Socket object associated with(theFederation, theFederate). If
     * the couple(Federation, Federate) is not found, a
     * FederateNotExecutionMember exception is thrown.
     * 
     * If the Federate has crashed, it should return a NULL socket object, but
     * this should not happen. In fact, when a Client(Federate) crashes, the
     * RTIG is supposed be remove all references to this federate. That's the
     * reason why a RTIinternalError is thrown in that case.
     * 
     * JYR : sorry but we return NULL (avoid rtig crash) because development needed
     */
    virtual Socket* getSocketLink(FederationHandle the_federation,
                                  FederateHandle the_federate,
                                  TransportType the_type = RELIABLE) const;

    SocketTuple* getWithReferences(FederationHandle the_federation, FederateHandle the_federate) const;

    FederateHandle getFederateFromSocket(FederationHandle the_federation, Socket* socket) const;

private:
    // The Server socket object(used for Accepts)
    SocketTCP* ServerSocketTCP;
    SocketUDP* ServerSocketUDP;

    // ---------------------
    // -- Private Methods --
    // ---------------------
    SocketTuple* getWithSocket(long socket_descriptor) const;
    
    #ifdef CERTI_RTIG_USE_POLL
    // use with poll
	std::vector<struct pollfd> _SocketVector;
	#endif
	
	#ifdef CERTI_RTIG_USE_EPOLL
    // use with epoll
	int _Epollfd;
	#endif
};

} // namespace certi

#endif // _CERTI_SOCKET_SERVER_HH
