// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: Communications.cc,v 3.40 2014/03/03 14:26:37 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Communications.hh"
#include <assert.h>
#include "PrettyDebug.hh"
#include "NM_Classes.hh"
#include "M_Classes.hh"

#include "SocketHTTPProxy.hh"
#include "SecureTCPSocket.hh"

#include <cerrno>
#include <fstream>
#include <iostream>
#include <cstdlib>

using std::ifstream ;
using std::ios ;
using std::cout ;
using std::endl ;
using std::list ;

namespace certi {
namespace rtia {

static PrettyDebug D("RTIA_COMM", "(RTIA Comm) ");
static PrettyDebug G("GENDOC",__FILE__);

// ----------------------------------------------------------------------------

NetworkMessage* Communications::waitMessage(
                                 NetworkMessage::Type type_msg,
                                 FederateHandle numeroFedere)
{    
    assert(type_msg > 0 && type_msg < NetworkMessage::LAST);
    NetworkMessage *msg = NULL;
    
    D.Out(pdProtocol, "Waiting for Message of Type %d.", type_msg);

    // Does a new message of the expected type has arrived ?
    if (searchMessage(type_msg, numeroFedere, &msg))
        return msg;

    // Otherwise, wait for a message with same type than expected and with
    // same federate number.
    msg = NM_Factory::receive(socketTCP);

    D.Out(pdProtocol, "TCP Message of Type %d has arrived.", type_msg);

    while ((msg->getMessageType() != type_msg) ||
           ((numeroFedere != 0) && (msg->getFederate() != numeroFedere))) {
        waitingList.push_back(msg);
        msg = NM_Factory::receive(socketTCP);
        D.Out(pdProtocol, "Message of Type %d has arrived.", type_msg);
    }
    
    assert(msg != NULL);
    assert(msg->getMessageType() == type_msg);
    return msg;
} /* end of waitMessage */

// ----------------------------------------------------------------------------
//! Communications.
Communications::Communications(int RTIA_port, int RTIA_fd)
{
    char nom_serveur_RTIG[200] ;
    const char *default_host = "localhost" ;

		
			// For int to string conversion
#if defined(RTIA_USE_SHM)
	pid_t pid_RTIA;
	std::stringstream pid_RTIA_str_tmp;
	std::string pid_RTIA_str;
#endif
		
		

    socketUN = new SocketUN();
	
#ifdef FEDERATION_USES_MULTICAST
    socketMC = new SocketMC();
#endif
    if(getenv("CERTI_HTTP_PROXY") != NULL || getenv("http_proxy") != NULL)
        socketTCP = new SocketHTTPProxy();
    else
#ifdef WITH_GSSAPI
        socketTCP = new SecureTCPSocket();
#else
        socketTCP = new SocketTCP();
#endif
    socketUDP = new SocketUDP();

    // Federate/RTIA link creation.
    if (0 <= RTIA_fd) {
      socketUN->setSocketFD(RTIA_fd);
    } else if (0 <= RTIA_port) {
      if (socketUN->connectUN(RTIA_port) == -1)
        exit(EXIT_FAILURE);
    } else {
      exit(EXIT_FAILURE);
    }
#if defined(RTIA_USE_SHM)
	pid_RTIA = getpid();
	pid_RTIA_str_tmp << (int) pid_RTIA;
	pid_RTIA_str = pid_RTIA_str_tmp.str();
	// std::cout << "PID RTIA (in Communications.cc) is: " << (int) pid_RTIA << std::endl ;
	RingBufferSHM = new RingBuffer(pid_RTIA_str,
											  RingBuffer::BUFFER_CS,
											  50000,
											  "Posix"
											   ) ;
	try {
		RingBufferSHM->Attach() ;
	}
	catch (certi::RingBufferNotAttached& e)
	{
		std::cout << "Catch Exception RingBufferNotAttached" << std::endl ;
		std::cout << "RingBuffer::Attach() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
	}
#endif

    // RTIG TCP link creation.
    const char *certihost = NULL ;
    ifstream file("RTIA.dat", ios::in);

    if (!file.is_open()) {
        certihost = getenv("CERTI_HOST");
        if (NULL == certihost) {
            certihost = default_host ;
        }
    }
    else {
        file.get(nom_serveur_RTIG, 200);
        file.close();
        certihost = nom_serveur_RTIG ;
    }

    const char *tcp_port = getenv("CERTI_TCP_PORT");
    const char *udp_port = getenv("CERTI_UDP_PORT");
    if (tcp_port==NULL) tcp_port = PORT_TCP_RTIG ;
    if (udp_port==NULL) udp_port = PORT_UDP_RTIG ;

    socketTCP->createConnection(certihost, atoi(tcp_port));
    socketUDP->createConnection(certihost, atoi(udp_port));
}

// ----------------------------------------------------------------------------
//! ~Communications.
Communications::~Communications()
{
    // Advertise RTIG that TCP link is being closed.
    G.Out(pdGendoc,"enter Communications::~Communications");

    NM_Close_Connexion closeMsg ;    
    closeMsg.send(socketTCP, NM_msgBufSend);
    socketTCP->close();

    delete socketUN;
#ifdef FEDERATION_USES_MULTICAST
    delete socketMC;
#endif
    delete socketTCP;
    delete socketUDP;
	
	#if defined(RTIA_USE_SHM)	
	delete RingBufferSHM ;
	#endif

    G.Out(pdGendoc,"exit  Communications::~Communications");
}

// ----------------------------------------------------------------------------
//! Request a service to federate.
void
Communications::requestFederateService(Message *req)
{
    // G.Out(pdGendoc,"enter Communications::requestFederateService for message "
    //               "type %d",req->type);
    assert(req != NULL);
    D.Out(pdRequest, "Sending Request to Federate, Name %s, Type %d.", req->getMessageName(),req->getMessageType());
	#if defined(RTIA_USE_SHM)
	req->send(RingBufferSHM, msgBufSend);	
	#else
    req->send(socketUN, msgBufSend);
	#endif
    // G.Out(pdGendoc,"exit  Communications::requestFederateService");
}

// ----------------------------------------------------------------------------
unsigned long
Communications::getAddress()
{
    return socketUDP->getAddr();
}

// ----------------------------------------------------------------------------
unsigned int
Communications::getPort()
{
    return socketUDP->getPort();
}

// ----------------------------------------------------------------------------
//! read message.
/*! Reads a message either from the network or from the federate
  Returns the actual source in the 1st parameter (RTIG=>1 federate=>2)
*/
void
Communications::readMessage(int &n, NetworkMessage **msg_reseau, Message **msg,
                            struct timeval *timeout)
{
    const int tcp_fd(socketTCP->returnSocket());
    const int udp_fd(socketUDP->returnSocket());

    int max_fd = 0; // not used for _WIN32
    fd_set fdset ;
    FD_ZERO(&fdset);

    if (msg_reseau) {
        FD_SET(tcp_fd, &fdset);
        FD_SET(udp_fd, &fdset);
#ifndef _WIN32
	max_fd = std::max(max_fd, std::max(tcp_fd, udp_fd));
#endif
    }
    if (msg) {
        FD_SET(socketUN->returnSocket(), &fdset);
#ifndef _WIN32
	max_fd = std::max(max_fd, socketUN->returnSocket());
#endif
    }

#ifdef FEDERATION_USES_MULTICAST
    // if multicast link is initialized (during join federation).
    if (_est_init_mc) {
        FD_SET(_socket_mc, &fdset);
#ifndef _WIN32
        max_fd = std::max(max_fd, _socket_mc);
#endif
   }
#endif

    if (msg_reseau && !waitingList.empty()) {
        // One message is in waiting buffer.        
        *msg_reseau = waitingList.front();
        waitingList.pop_front();                
        n = 1 ;
    }
    else if (msg_reseau && socketTCP->isDataReady()) {
        // Datas are in TCP waiting buffer.
        // Read a message from RTIG TCP link.
    	*msg_reseau = NM_Factory::receive(socketTCP);
        n = 1 ;
    }
    else if (msg_reseau && socketUDP->isDataReady()) {
        // Datas are in UDP waiting buffer.
        // Read a message from RTIG UDP link.
    	*msg_reseau = NM_Factory::receive(socketUDP);
        n = 1 ;
    }
    else if (msg && socketUN->isDataReady()) {
        // Datas are in UNIX waiting buffer.
        // Read a message from federate UNIX link.
			#if defined(RTIA_USE_SHM)
			*msg = M_Factory::receive(RingBufferSHM);	
			#else
			*msg = M_Factory::receive(socketUN);
			#endif
        n = 2 ;
    }
    else {
        // waitingList is empty and no data in TCP buffer.
        // Wait a message (coming from federate or network).
#ifdef _WIN32
        if (select(max_fd, &fdset, NULL, NULL, timeout) < 0) {
            if (WSAGetLastError() == WSAEINTR)
#else
        if (select(max_fd+1, &fdset, NULL, NULL, timeout) < 0) {
            if (errno == EINTR)
#endif 
            {
                throw NetworkSignal("EINTR on select");
                }
				else {
                 throw NetworkError("Unexpected errno on select");
				}
        }

        // At least one message has been received, read this message.

#ifdef FEDERATION_USES_MULTICAST
        // Priorite aux messages venant du multicast(pour essayer d'eviter
        // un depassement de la file et donc la perte de messages)

        if (_est_init_mc && FD_ISSET(_socket_mc, &fdset)) {
            // Read a message coming from the multicast link.
            receiveMC(*msg_reseau);
            n = 1 ;
        }
#endif

        if (FD_ISSET(socketTCP->returnSocket(), &fdset)) {
            // Read a message coming from the TCP link with RTIG.
        	(*msg_reseau) = NM_Factory::receive(socketTCP);
            n = 1 ;
        }
        else if (FD_ISSET(socketUDP->returnSocket(), &fdset)) {
            // Read a message coming from the UDP link with RTIG.
        	(*msg_reseau) = NM_Factory::receive(socketUDP);
            n = 1 ;
        }
        else if (FD_ISSET(socketUN->returnSocket(), &fdset)) {
            // Read a message coming from the federate.
			#if defined(RTIA_USE_SHM)
			*msg = M_Factory::receive(RingBufferSHM);	
			#else
			*msg = M_Factory::receive(socketUN);
			#endif
            n = 2 ;
        }
        else
        {
            // select() timeout occured
            n = 3;
        }
    }
} /* end of readMessage */

// ----------------------------------------------------------------------------
/*! Returns true if a 'type_msg' message coming from federate
  'numeroFedere' (or any other federate if numeroFedere == 0) was in
  the queue and was copied in 'msg'. If no such message is found,
  returns RTI_FALSE.
*/
bool
Communications::searchMessage(NetworkMessage::Type type_msg,
                              FederateHandle numeroFedere,
                              NetworkMessage **msg)
{
    list<NetworkMessage *>::iterator i ;
    for (i = waitingList.begin(); i != waitingList.end(); i++) {

        D.Out(pdProtocol, "Rechercher message de type %d .", type_msg);

        if ((*i)->getMessageType() == type_msg) {
            // if numeroFedere != 0, verify that federateNumbers are similar
            if (((*i)->getFederate() == numeroFedere) || (numeroFedere == 0)) {
            	*msg = *i;                                
                waitingList.erase(i);
                D.Out(pdProtocol,
                      "Message of Type %d was already here.",
                      type_msg);
                return true ;
            }
        }
    }
    return false ;
}

// ----------------------------------------------------------------------------
void
Communications::sendMessage(NetworkMessage *Msg)
{
    Msg->send(socketTCP, NM_msgBufSend);
}

// ----------------------------------------------------------------------------
void
Communications::sendUN(Message *Msg)
{
	#if defined(RTIA_USE_SHM)
	Msg->send(RingBufferSHM, msgBufSend);
	#else
    Msg->send(socketUN, msgBufSend);
	#endif
}

// ----------------------------------------------------------------------------
Message*
Communications::receiveUN()
{
	#if defined(RTIA_USE_SHM)
	Message* msg = M_Factory::receive(RingBufferSHM);	
	#else
	Message* msg = M_Factory::receive(socketUN);
	#endif
	return msg;
}

}} // namespace certi/rtia

// $Id: Communications.cc,v 3.40 2014/03/03 14:26:37 erk Exp $
