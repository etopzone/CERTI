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
// $Id: SocketUDP.hh,v 3.5 2003/06/26 15:13:38 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_SOCKET_UDP_HH
#define _CERTI_SOCKET_UDP_HH

#include "Socket.hh"

#include <sys/socket.h>
#include <netdb.h>

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

    Boolean isDataReady() const ;

    int getClass() const { return SOCKET_TYPE_UDP ; };
    int returnSocket() const ;

    unsigned long returnAdress() const ;

    void close();

    // --------------------------
    // -- UDP Specific Methods --
    // --------------------------

    SocketUDP();
    ~SocketUDP();

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

    unsigned int getPort() const ;
    unsigned long getAddr() const ;

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

    ByteCount SentBytesCount ;
    ByteCount RcvdBytesCount ;
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

// $Id: SocketUDP.hh,v 3.5 2003/06/26 15:13:38 breholee Exp $

