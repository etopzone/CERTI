// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
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
// $Id: SocketUDP.hh,v 3.4 2003/02/19 18:07:30 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_SOCKET_UDP_HH
#define _CERTI_SOCKET_UDP_HH

#include <config.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <cstring>
#include <ulimit.h>
#include <assert.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/param.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <strings.h>

#include "Socket.hh"
#include "RTItypes.hh"
#include "PrettyDebug.hh"

namespace certi {

class SocketUDP : public Socket
{
public :
    // ---------------------------------------------
    // -- Fonctions heritee de la classe Socket --
    // ---------------------------------------------

    void send(void * Message, unsigned long Size)
        throw (NetworkError,
               NetworkSignal);

    void receive(void * Message, unsigned long Size)
        throw (NetworkError,
               NetworkSignal);

    Boolean isDataReady(void) const ;

    int getClass(void) const { return SOCKET_TYPE_UDP ; };
    int returnSocket(void) const ;

    unsigned long returnAdress(void) const ;

    void close(void);

    // --------------------------
    // -- UDP Specific Methods --
    // --------------------------

    SocketUDP(void);
    ~SocketUDP(void);

    void createUDPClient(unsigned int port, char *nom_serveur)
        throw (NetworkError,
               NetworkSignal);
    void createUDPServer(unsigned int port)
        throw (NetworkError,
               NetworkSignal);

    void attach(int socket_ouvert, unsigned long Adresse,
                unsigned int port)
        throw (NetworkError,
               NetworkSignal);

    unsigned int getPort(void) const ;
    unsigned long getAddr(void) const ;

private:

#define BUFFER_MAXSIZE 2000
    // ------------------------
    // -- Private Attributes --
    // ------------------------

    //! TAG indiquant si c'est une liaison logique ou physique
    Boolean PhysicalLink ;

    long _socket_udp ;
    struct sockaddr_in sock_local ;

    struct sockaddr_in sock_source ;
    char *Addr_Source ;
    unsigned int Port_Source ;
    struct sockaddr_in sock_distant ;
    struct hostent * hp_distant ;

    int _sock_local_length ;
    Boolean _est_init_udp ;

    unsigned long long SentBytesCount ;
    unsigned long long RcvdBytesCount ;
    unsigned long BufferSize ;
    char Buffer[4096] ;

    struct hostent * hp_local ;

    // ---------------------
    // -- Private Methods --
    // ---------------------

    void setPort(unsigned int port);

    int bind();
    int open();
};
}

#endif // _CERTI_SOCKET_UDP_HH

// $Id: SocketUDP.hh,v 3.4 2003/02/19 18:07:30 breholee Exp $

