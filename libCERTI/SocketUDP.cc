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
// $Id: SocketUDP.cc,v 3.8 2003/08/20 18:40:02 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "SocketUDP.hh"

#include "RTItypes.hh"
#include "PrettyDebug.hh"

#include <iostream>
#include <unistd.h>
#include <strings.h>
#include <assert.h>

using std::cout ;
using std::endl ;

namespace certi {

static pdCDebug D("SOCKUDP", "(SocketUDP) - ");

// ----------------------------------------------------------------------------
//! attach.
void
SocketUDP::attach(int socket_ouvert, unsigned long Adresse, unsigned int port)
    throw (NetworkError,
           NetworkSignal)
{
    assert(!_est_init_udp);

    PhysicalLink = RTI_FALSE ;

    _socket_udp = socket_ouvert ;
    // Building Distant Address
    memset((struct sockaddr_in *) &sock_distant, 0, sizeof(struct sockaddr_in));

    sock_distant.sin_addr.s_addr = htonl(Adresse);
    sock_distant.sin_family = AF_INET ;
    sock_distant.sin_port = port ;

    _est_init_udp = RTI_TRUE ;

    D.Out(pdDebug, "Attaching the federate to his address and to his peer...");
}

// ----------------------------------------------------------------------------
//! bind.
int SocketUDP::bind()
{
    assert(!_est_init_udp);

    long result = ::bind(_socket_udp, (sockaddr *)&sock_local,
                         sizeof(struct sockaddr_in));

    return ((result == 0) ? 1 : 0);
}

// ----------------------------------------------------------------------------
//! Create an UDP client.
void
SocketUDP::createUDPClient(unsigned int, // port
                           char *) // nom serveur
    throw (NetworkError, NetworkSignal)
{
    // unsigned long addr = 0 ;
    struct sockaddr_in sock_temp ;
    socklen_t taille = sizeof(struct sockaddr_in);
    char localhost[4096] ;

    assert(!_est_init_udp);

    // Building Local Address
    memset((struct sockaddr_in *) &sock_local, 0, sizeof(struct sockaddr_in));
    hp_local = (struct hostent *)malloc(sizeof(struct hostent));

    gethostname(localhost, 4096);

    if ((hp_local = (struct hostent *)gethostbyname(localhost)) == NULL)
        {
            perror("SocketUDP: gethostbyname");
            throw NetworkError();
        }

    bcopy((char *)hp_local->h_addr, (char *) &sock_local.sin_addr,
          hp_local->h_length);
    sock_local.sin_family = AF_INET ;

    // lors du BIND, le systeme alloue un port libre au socket _sock_udp
    sock_local.sin_port = 0 ;


    if (!open())
        {
            perror("SocketUDP: Open");
            throw NetworkError();
        }


    if (!bind())
        {
            perror("SocketUDP: Bind");
            throw NetworkError();
        }

    // recuperation du port lie au socket _socket_udp
    getsockname(_socket_udp, (sockaddr *)&sock_temp, &taille);
    //HPUX: getsockname(_socket_udp, (sockaddr *)&sock_temp, (int*)&taille);
    sock_local.sin_port = sock_temp.sin_port ;
    _est_init_udp = RTI_TRUE ;
}

// ----------------------------------------------------------------------------
//! create an UDP server.
void
SocketUDP::createUDPServer(unsigned int port)
    throw (NetworkError,
           NetworkSignal)
{
    assert(!_est_init_udp);

    char localhost[4096] ;

    // Building Local Address
    memset((struct sockaddr_in *) &sock_local, 0, sizeof(struct sockaddr_in));
    hp_local = (struct hostent *)malloc(sizeof(struct hostent));

    gethostname(localhost, 4096);

    hp_local = (struct hostent *) gethostbyname(localhost);
    if (hp_local == 0) {
        perror("SocketUDP: gethostbyname");
        throw NetworkError();
    }

    bcopy((char *)hp_local->h_addr, (char *) &sock_local.sin_addr,
          hp_local->h_length);
    sock_local.sin_family = hp_local->h_addrtype ;
    sock_local.sin_port = htons((u_short)port);


    if (!open())
        {
            perror("SocketUDP: Open");
            throw NetworkError();
        }

    if (!bind())
        {
            perror("SocketUDP: Bind");
            throw NetworkError();
        }

    _est_init_udp = RTI_TRUE ;
}

// ----------------------------------------------------------------------------
//! Constructor.
SocketUDP::SocketUDP()
{
    _est_init_udp = RTI_FALSE ;

    hp_distant = NULL ;
    hp_local = NULL ;
    PhysicalLink = RTI_TRUE ;

    BufferSize = 0 ;
    SentBytesCount = 0 ;
    RcvdBytesCount = 0 ;
}

// ----------------------------------------------------------------------------
//! Destructor.
SocketUDP::~SocketUDP()
{
    // Fermeture
    if (_est_init_udp)
        close();

#ifdef RTI_PRINTS_STATISTICS
    cout << " UDP Socket " ;
    cout.width(2);
    cout << _socket_udp << " : total = " ;
    cout.width(9);
    cout << SentBytesCount << "b sent " << endl ;
    cout << " UDP Socket " ;
    cout.width(2);
    cout << _socket_udp << " : total = " ;
    cout.width(9);
    cout << RcvdBytesCount << "b received" << endl ;
#endif
}

// ----------------------------------------------------------------------------
//! send.
void SocketUDP::send(void * Message, unsigned long Size)
    throw (NetworkError,
           NetworkSignal)
{
    long nSent = 0 ;
    // long expected_size = Size ;

    D.Out(pdDebug, "Beginning to send UDP message... Size = %ld", Size);

    assert(_est_init_udp);

    nSent = sendto(_socket_udp, (char *)Message, Size, 0,
                   (struct sockaddr *)&sock_distant, sizeof(sock_distant));
    if (nSent < 0) {
        perror("Sendto");
        throw NetworkError();
    };
    D.Out(pdDebug, "Sent UDP message.");
    SentBytesCount += nSent ;
}

// ----------------------------------------------------------------------------
//! close.
void SocketUDP::close()
{
    if (_est_init_udp) {
        D.Out(pdDebug, "Closing UDP object...");
        _est_init_udp = RTI_FALSE ;
        if (PhysicalLink == RTI_TRUE) {
            D.Out(pdDebug, "Closing physical UDP link...");
            ::close(_socket_udp);
        }
    }
}

// ----------------------------------------------------------------------------
//! getAddr.
unsigned long
SocketUDP::getAddr() const
{
    D.Out(pdDebug, "Hostname is %ul...", sock_local.sin_addr.s_addr);
    return(sock_local.sin_addr.s_addr);
}

// ----------------------------------------------------------------------------
//! getPort.
unsigned int
SocketUDP::getPort() const
{
    D.Out(pdDebug, "UDP port is %ud...", sock_local.sin_port);
    return sock_local.sin_port ;
}

// ----------------------------------------------------------------------------
/*! Return RTI_TRUE if any data as already been read from the system socket
  and is waiting in the internal buffer, else RTI_FALSE.
*/
Boolean
SocketUDP::isDataReady() const
{
    return ((BufferSize > 0) ? RTI_TRUE : RTI_FALSE);
}

// ----------------------------------------------------------------------------
//! open.
int SocketUDP::open()
{
    return(((_socket_udp=socket(AF_INET, SOCK_DGRAM, 0))<0)?0:1);
}

// ----------------------------------------------------------------------------
//! receive.
void
SocketUDP::receive(void * Message, unsigned long Size)
    throw (NetworkError, NetworkSignal)
{
    socklen_t taille = sizeof(struct sockaddr_in);
    int CR ;

    assert(_est_init_udp);

    D.Out(pdDebug, "Beginning to receive UDP message...");
    if (BufferSize == 0) {
        CR = recvfrom(_socket_udp,
                      Buffer, BUFFER_MAXSIZE, 0,
                      (struct sockaddr *)&sock_source, &taille);
        //HPUX:(struct sockaddr *)&sock_source, (int*) &taille);
        if (CR <= 0) {
            perror("Recvfrom");
            throw NetworkError();
        }
        else {
            RcvdBytesCount += CR ;
            BufferSize += CR ;
        }
    }

    if (BufferSize < Size) {
        perror("Taille du Buffer inferieure a celle demandee");
        throw NetworkError();
    }
    else {
        BufferSize -= Size ;
        memcpy(Message, (void *)Buffer, Size);
        memmove((void *) Buffer, (void *)(Buffer + Size), BufferSize);
    }
}

// ----------------------------------------------------------------------------
//! returnAdress.
unsigned long
SocketUDP::returnAdress() const
{
    D.Out(pdDebug, "Retourner Adresse Machine locale...");
    return getAddr();
}

// ----------------------------------------------------------------------------
//! returnSocket.
int
SocketUDP::returnSocket() const
{
    D.Out(pdDebug, "Retourner Socket UDP...");
    return _socket_udp ;
}

// ----------------------------------------------------------------------------
//! setPort.
void SocketUDP::setPort(unsigned int port)
{
    D.Out(pdDebug, "Affectation du Port UDP...");
    sock_local.sin_port=port ;
}

}

// $Id: SocketUDP.cc,v 3.8 2003/08/20 18:40:02 breholee Exp $
