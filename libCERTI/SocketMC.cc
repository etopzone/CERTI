// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
//
// This program is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
// ----------------------------------------------------------------------------

#include "SocketMC.hh"
#include "config.h"

#ifdef _WIN32
#include "SocketTCP.hh"
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace certi {

// ----------------------------------------------------------------------------
SocketMC::SocketMC()
{
    _est_init_mc = false;
    _num_msg = 1;
#ifdef _WIN32
    SocketTCP::winsockStartup();
#endif
}

// ----------------------------------------------------------------------------
SocketMC::~SocketMC()
{
    if (_est_init_mc) {
#ifdef _WIN32
        closesocket(_socket_mc);
        closesocket(_socket_emetteur);
#else
        ::close(_socket_mc);
        ::close(_socket_emetteur);
#endif
        _est_init_mc = false;
    }

#ifdef _WIN32
    SocketTCP::winsockShutdown();
#endif
}

// ----------------------------------------------------------------------------
void SocketMC::CreerSocketMC(char* addr, unsigned long port)
{
    assert(addr != NULL);
    CreerSocketMC(inet_addr(addr), port);
}

// ----------------------------------------------------------------------------
void SocketMC::CreerSocketMC(unsigned long addr, unsigned long port)
{
    struct ip_mreq _mreq;
    unsigned long _mreqlen;

    assert(!_est_init_mc);
    assert(addr > 0);

#ifdef _WIN32
    assert(SocketTCP::winsockInitialized());
#endif

    // create receiving socket
    _socket_mc = socket(AF_INET, SOCK_DGRAM, 0);
    if (_socket_mc < 0) {
        perror("socket1");
        throw NetworkError("cannot create socket");
    }

    memset(&_sin, sizeof(_sin), sizeof(_sin));
    _sin.sin_family = AF_INET;
    _sin.sin_addr.s_addr = htonl(INADDR_ANY);
    _sin.sin_port = htons(port);
    _sinlen = sizeof(_sin);

    if (bind(_socket_mc, (struct sockaddr*) &_sin, _sinlen) < 0) {
        perror("SocketMC: bind");
        throw NetworkError("cannot bind");
    }

    // joindre le groupe multiCast
    _mreq.imr_multiaddr.s_addr = addr;
    _mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    _mreqlen = sizeof(_mreq);
    if (setsockopt(_socket_mc, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &_mreq, _mreqlen) < 0) {
        perror("setsockopt");
        throw NetworkError("cannot setsockopt");
    }

    // creation du socket emetteur
    _socket_emetteur = socket(AF_INET, SOCK_DGRAM, 0);
    if (_socket_emetteur < 0) {
        perror("socket2");
        throw NetworkError("cannot create socket");
    }

    memset(&_sin_e, sizeof(_sin_e), sizeof(_sin_e));
    _sin_e.sin_family = AF_INET;
    _sin_e.sin_port = htons(port);
    _sin_e.sin_addr.s_addr = addr;
    _sinlen_e = sizeof(_sin_e);

    // l'initialisation de la communication multicast est reussie
    _est_init_mc = true;
}

// ----------------------------------------------------------------------------
void SocketMC::send(const unsigned char* Buffer, size_t)
{
    // BUG:Revoir ca en faisant le contraire(EmettreMC appelle Emettre)
    sendMC((NetworkMessage*) Buffer);
}

// ----------------------------------------------------------------------------
void SocketMC::receive(void* Buffer, unsigned long)
{
    // BUG: Revoir ca en faisant le contraire(RecevoirMC appelle Recevoir)
    receiveMC((NetworkMessage*) Buffer);
}

// ----------------------------------------------------------------------------
#ifdef _WIN32
SOCKET SocketMC::returnSocket()
{
    return _socket_mc;
}
#else
int SocketMC::returnSocket()
{
    return _socket_mc;
}
#endif

// ----------------------------------------------------------------------------
unsigned long SocketMC::returnAdress() const
{
    return _sin_e.sin_addr.s_addr;
}

// ----------------------------------------------------------------------------
void SocketMC::close()
{
    if (_est_init_mc) {
#ifdef _WIN32
        closesocket(_socket_mc);
        closesocket(_socket_emetteur);
#else
        ::close(_socket_mc);
        ::close(_socket_emetteur);
#endif
        _est_init_mc = false;
    }
}

// ----------------------------------------------------------------------------
void SocketMC::sendMC(NetworkMessage* message)
{
    int cnt;

    //message->number=_num_msg ;
    // FIXME EN: SocketMC is now broken for several release
    _num_msg++;
    assert(_est_init_mc);
    assert(message != NULL);

    message->send(this, MCMessageBuffer); // FIXME to be checked
    cnt = sendto(_socket_emetteur, (char*) message, TAILLE_MSG_RESEAU, 0, (struct sockaddr*) &_sin_e, _sinlen_e);

    if (cnt < 0) {
        perror("Send");
        throw NetworkError("cannot sendto");
    }
}

// ----------------------------------------------------------------------------
char* SocketMC::receiveMC(NetworkMessage* message)
{
    int cnt;

    assert(_est_init_mc);
    // FIXME receive NetworkMessage with virtual constructor
    cnt = recvfrom(_socket_mc, (char*) message, TAILLE_MSG_RESEAU, 0, (struct sockaddr*) &_sin, &_sinlen);
    if (cnt < 0) {
        perror("Recv");
        throw NetworkError("cannot recvfrom");
    }

    return (inet_ntoa(_sin.sin_addr));
}

// ----------------------------------------------------------------------------
/** Wait for a socket event, until a time-out. The time-out is given
    with seconds and microseconds in parameter.
    @param sec seconds
    @param usec microseconds
    @return number of waiting events, 0 if time-out. May be negative
    in case of error
 */
int SocketMC::timeoutMC(int sec, int usec)
{
    assert(_est_init_mc);

    struct timeval timeout;
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(_socket_mc, &fdset);

    int nb = 0;
    bool check;
    do {
        nb = select(_socket_mc + 1, SELECT_TYPE_ARG234 & fdset, NULL, NULL, &timeout);

#ifdef _WIN32
        check = (WSAGetLastError() == WSAEINTR);
#else
        check = (errno == EINTR);
#endif
    } while ((nb < 0) && check);

    return nb;
}

} // namespace certi
