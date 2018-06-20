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

#include "PrettyDebug.hh"
#include "SocketUDP.hh"
#include "certi.hh"
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#include "SocketTCP.hh"
#else
#include <unistd.h>
#endif

#include <assert.h>
#include <errno.h>
#include <iostream>

using std::cout;
using std::endl;

namespace certi {

static PrettyDebug D("SOCKUDP", "(SocketUDP) - ");

// ----------------------------------------------------------------------------
void SocketUDP::attach(int socket_ouvert, unsigned long Adresse, unsigned int port)
{
    assert(!_est_init_udp);

    PhysicalLink = false;

    _socket_udp = socket_ouvert;
    // Building Distant Address
    memset((struct sockaddr_in*) &sock_distant, 0, sizeof(struct sockaddr_in));

    sock_distant.sin_addr.s_addr = htonl(Adresse);
    sock_distant.sin_family = AF_INET;
    sock_distant.sin_port = port;

    _est_init_udp = true;

    Debug(D, pdDebug) << "Attaching the federate to his address and to his peer..." << std::endl;
}

// ----------------------------------------------------------------------------
int SocketUDP::bind()
{
    assert(!_est_init_udp);

    long result = ::bind(_socket_udp, (sockaddr*) &sock_local, sizeof(struct sockaddr_in));

    return ((result == 0) ? 1 : 0);
}

// ----------------------------------------------------------------------------
//! Create an UDP client.
void SocketUDP::createConnection(const char* /*server_name*/, unsigned int /*port*/)
{
    struct sockaddr_in sock_temp;
    char localhost[4096];
    struct hostent* hp_local;

#ifdef _WIN32 //netDot
    int taille = sizeof(struct sockaddr_in);
#else
    socklen_t taille = sizeof(struct sockaddr_in);
#endif

    assert(!_est_init_udp);

    // Building Local Address
    memset(&sock_local, 0, sizeof(struct sockaddr_in));
    hp_local = NULL;

    gethostname(localhost, 4096);

    if ((hp_local = gethostbyname(localhost)) == NULL) {
        throw NetworkError("gethostbyname gave NULL answer for hostname <" + std::string(localhost) + "> with error <"
                           + strerror(errno)
                           + ">");
    }

    memcpy((char*) &sock_local.sin_addr, (char*) hp_local->h_addr, hp_local->h_length);

    // lors du BIND, le systeme alloue un port libre au socket _sock_udp
    sock_local.sin_family = AF_INET;
    sock_local.sin_port = 0;

    if (!open()) {
        throw NetworkError("Cannot Open Socket open gave error < " + std::string(strerror(errno)) + ">");
    }

    if (!bind()) {
        throw NetworkError("Cannot Bind Socket bind gave error < " + std::string(strerror(errno)) + ">");
    }

    // recuperation du port lie au socket _socket_udp
    getsockname(_socket_udp, (sockaddr*) &sock_temp, &taille);
    //HPUX: getsockname(_socket_udp, (sockaddr *)&sock_temp, (int*)&taille);
    sock_local.sin_port = sock_temp.sin_port;
    _est_init_udp = true;
}

// ----------------------------------------------------------------------------
//! create an UDP server.
void SocketUDP::createServer(unsigned int port, in_addr_t addr)
{
    assert(!_est_init_udp);

    // Building Local Address
    memset((struct sockaddr_in*) &sock_local, 0, sizeof(struct sockaddr_in));

    sock_local.sin_addr.s_addr = addr;
    sock_local.sin_family = AF_INET;
    sock_local.sin_port = htons((u_short) port);

    if (!open()) {
        throw NetworkError("Cannot open UDP Socket" + std::string(strerror(errno)));
    }

    if (!bind()) {
        throw NetworkError("Cannot bind UDP Socket" + std::string(strerror(errno)));
    }

    _est_init_udp = true;
}

// ----------------------------------------------------------------------------
SocketUDP::SocketUDP()
{
    PhysicalLink = true;

    _socket_udp = 0;

    memset(&sock_local, 0, sizeof(struct sockaddr_in));
    memset(&sock_source, 0, sizeof(struct sockaddr_in));
    Addr_Source = NULL;
    Port_Source = 0;

    memset(&sock_distant, 0, sizeof(struct sockaddr_in));
    hp_distant = NULL;

    _sock_local_length = 0;
    _est_init_udp = false;

    SentBytesCount = 0;
    RcvdBytesCount = 0;
    BufferSize = 0;

#ifdef _WIN32 //netDot
    SocketTCP::winsockStartup();
#endif
}

// ----------------------------------------------------------------------------
SocketUDP::~SocketUDP()
{
    // Closing the socket
    if (_est_init_udp)
        close();

#ifdef _WIN32 //netDot
    SocketTCP::winsockShutdown();
#endif

#ifdef RTI_PRINTS_STATISTICS
    cout << " UDP Socket ";
    cout.width(2);
    cout << _socket_udp << " : total = ";
    cout.width(9);
    cout << SentBytesCount << " Bytes sent " << endl;
    cout << " UDP Socket ";
    cout.width(2);
    cout << _socket_udp << " : total = ";
    cout.width(9);
    cout << RcvdBytesCount << " Bytes received" << endl;
#endif
}

// ----------------------------------------------------------------------------
void SocketUDP::send(const unsigned char* Message, size_t Size)
{
    Debug(D, pdDebug) << "Beginning to send UDP message... Size = " << Size << std::endl;
    assert(_est_init_udp);

    int sent = sendto(_socket_udp, (char*) Message, Size, 0, (struct sockaddr*) &sock_distant, sizeof(sock_distant));
    if (sent < 0) {
        perror("Sendto");
        throw NetworkError("cannot sendto");
    };
    Debug(D, pdDebug) << "Sent UDP message." << std::endl;
    SentBytesCount += sent;
}

// ----------------------------------------------------------------------------
void SocketUDP::close()
{
    if (_est_init_udp) {
        Debug(D, pdDebug) << "Closing UDP object..." << std::endl;
        _est_init_udp = false;
        if (PhysicalLink) {
            Debug(D, pdDebug) << "Closing physical UDP link..." << std::endl;
#ifdef _WIN32 //netDot
            ::closesocket(_socket_udp);
#else
            ::close(_socket_udp);
#endif
        }
    }
}

// ----------------------------------------------------------------------------
unsigned long SocketUDP::getAddr() const
{
    Debug(D, pdDebug) << "Hostname is " << sock_local.sin_addr.s_addr << std::endl;
    return (sock_local.sin_addr.s_addr);
}

// ----------------------------------------------------------------------------
unsigned int SocketUDP::getPort() const
{
    Debug(D, pdDebug) << "UDP port is " << sock_local.sin_port << std::endl;
    return sock_local.sin_port;
}

// ----------------------------------------------------------------------------
/*! Return whether any data as already been read from the system socket
  and is waiting in the internal buffer.
*/
bool SocketUDP::isDataReady() const
{
    return BufferSize > 0;
}

// ----------------------------------------------------------------------------
int SocketUDP::open()
{
#ifdef _WIN32 //netDot
    assert(SocketTCP::winsockInitialized());
#endif

    _socket_udp = socket(AF_INET, SOCK_DGRAM, 0);
    return _socket_udp >= 0;
}

// ----------------------------------------------------------------------------
void SocketUDP::receive(void* Message, unsigned long Size)
{
#ifdef _WIN32 //netDot
    int taille = sizeof(struct sockaddr_in);
#else
    socklen_t taille = sizeof(struct sockaddr_in);
#endif

    int CR;

    assert(_est_init_udp);

    Debug(D, pdDebug) << "Beginning to receive UDP message..." << std::endl;
    if (BufferSize == 0) {
        CR = recvfrom(_socket_udp, Buffer, BUFFER_MAXSIZE, 0, (struct sockaddr*) &sock_source, &taille);
        //HPUX:(struct sockaddr *)&sock_source, (int*) &taille);
        if (CR <= 0) {
            perror("Recvfrom");
            throw NetworkError("cannot recvfrom");
        }
        else {
            RcvdBytesCount += CR;
            BufferSize += CR;
        }
    }

    if (BufferSize < Size) {
        perror("Taille du Buffer inferieure a celle demandee");
        throw NetworkError("");
    }
    else {
        BufferSize -= Size;
        memcpy(Message, (void*) Buffer, Size);
        memmove((void*) Buffer, (void*) (Buffer + Size), BufferSize);
    }
}

// ----------------------------------------------------------------------------
unsigned long SocketUDP::returnAdress() const
{
    Debug(D, pdDebug) << "Retourner Adresse Machine locale..." << std::endl;
    return getAddr();
}

// ----------------------------------------------------------------------------
#ifdef _WIN32
SOCKET SocketUDP::returnSocket()
#else
int SocketUDP::returnSocket()
#endif
{
    Debug(D, pdDebug) << "Retourner Socket UDP..." << std::endl;
    return _socket_udp;
}

// ----------------------------------------------------------------------------
void SocketUDP::setPort(unsigned int port)
{
    Debug(D, pdDebug) << "Affectation du Port UDP..." << std::endl;
    sock_local.sin_port = port;
}

} // namespace certi
