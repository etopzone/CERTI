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

#include "SocketTCP.hh"
#include "SocketUN.hh"
#include "certi.hh"

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>

#include <iostream>
#include <string>
#ifndef _WIN32
#include <unistd.h>
#endif

using std::string;
using std::cout;
using std::endl;

#ifdef _WIN32
extern "C" int socketpair_win32(SOCKET socks[2], int make_overlapped);
#endif

namespace certi {
static PrettyDebug D("SOCKUN", "SocketUN");
static PrettyDebug G("GENDOC", __FILE__);

#ifndef _WIN32
static void closesocket(int fd)
{
    close(fd);
}
#endif

int SocketUN::listenUN()
{
    struct sockaddr_in addr;
#ifdef _WIN32
    int addrlen = sizeof(addr);
#else
    socklen_t addrlen = sizeof(addr);
#endif

    if ((_socket_un = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;
    Debug(D, pdInit) << "Server: Created IPV4 socket." << std::endl;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;

    if (bind(_socket_un, (const struct sockaddr*) &addr, sizeof(addr)) == -1) {
        closesocket(_socket_un);
        _socket_un = -1;
        return -1;
    }
    Debug(D, pdInit) << "Server: Bound to anonymous IPV4 address." << std::endl;

    if (getsockname(_socket_un, (struct sockaddr*) &addr, &addrlen) == -1) {
        closesocket(_socket_un);
        _socket_un = -1;
        return -1;
    }
    Debug(D, pdInit) << "Server: Got server port address " << ntohs(addr.sin_port) << std::endl;

    if (listen(_socket_un, 1) == -1) {
        closesocket(_socket_un);
        _socket_un = -1;
        return -1;
    }
    Debug(D, pdInit) << "Server: Now listening." << std::endl;

    return ntohs(addr.sin_port);
}

// ----------------------------------------------------------------------------
//! Called by client to connect.
int SocketUN::connectUN(int port)
{
    if ((_socket_un = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return -1;
    }
    Debug(D, pdInit) << "Client: Created IPV4 socket." << std::endl;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(port);
    if (connect(_socket_un, (const struct sockaddr*) &addr, sizeof(addr)) == -1) {
        return -1;
    }

    Debug(D, pdInit) << "Client: Connected to IPV4 address at port " << port << std::endl;

    return 0;
}

// ----------------------------------------------------------------------------
//! Called by server to open the socket and wait for the connection.
int SocketUN::acceptUN(unsigned msec)
{
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(_socket_un, &fdset);

    struct timeval timeout;
    timeout.tv_sec = msec / 1000;
    timeout.tv_usec = 1000 * (msec % 1000);

    Debug(D, pdInit) << "Server: Waiting for a connection to accept." << std::endl;
    if (select(_socket_un + 1, &fdset, NULL, NULL, &timeout) <= 0) {
        closesocket(_socket_un);
        _socket_un = -1;
        return -1;
    }

    Debug(D, pdInit) << "Server: Accepting connection." << std::endl;
    SOCKET accepted;
    if ((accepted = accept(_socket_un, NULL, NULL)) == -1) {
        closesocket(_socket_un);
        _socket_un = -1;
        return -1;
    }
    Debug(D, pdInit) << "Server: Accepted IPV4 connection." << std::endl;

    closesocket(_socket_un);
    _socket_un = accepted;
    return 0;
}

#ifdef _WIN32
SOCKET
SocketUN::socketpair()
{
    SOCKET fd[2];
    if (::socketpair_win32(fd, 0) == -1) {
        perror("socketpair");
        return -1;
    }
    _socket_un = fd[0];
    return fd[1];
}
#else
int SocketUN::socketpair()
{
    SOCKET fd[2];
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, fd) == -1) {
        perror("socketpair");
        return -1;
    }
    _socket_un = fd[0];
    return fd[1];
}
#endif

// ----------------------------------------------------------------------------
//! Does not open the socket, see Init methods.
SocketUN::SocketUN(SignalHandlerType theType)
    : _socket_un(-1), HandlerType(theType), SentBytesCount(0), RcvdBytesCount(0)
{
#ifdef _WIN32
    SocketTCP::winsockStartup();
#endif

#ifdef SOCKUN_BUFFER_LENGTH
    RBLength = 0;
#endif

    Debug(D, pdInit) << "UNIX Socket created." << std::endl;
}

// ----------------------------------------------------------------------------
//! Close the socket.
SocketUN::~SocketUN()
{
    if (0 <= _socket_un)
        closesocket(_socket_un);

    Debug(D, pdTerm) << "SocketUN: Closed all sockets." << std::endl;

#ifdef _WIN32
    SocketTCP::winsockShutdown();
#endif

    Debug(D, pdCom) << "Unix Socket " << _socket_un << ": total = " << SentBytesCount << "b sent" << std::endl;
    Debug(D, pdCom) << "Unix Socket " << _socket_un << ": total = " << RcvdBytesCount << "b received" << std::endl;
}

// ----------------------------------------------------------------------------
//! send Message using Socket UNIX.
/**    @param buffer buffer to send
       @param size   buffer size
*/
void SocketUN::send(const unsigned char* buffer, size_t size)
{
    long sent = 0;
    unsigned long total_sent = 0;

    // G.Out(pdGendoc,"enter SocketUN::send");
    assert(0 <= _socket_un);

    Debug(D, pdTrace) << "Beginning to send UN message..." << std::endl;

    while (total_sent < size) {
#ifdef _WIN32
        sent = ::send(_socket_un, (char*) buffer + total_sent, size - total_sent, 0);
#else
        sent = write(_socket_un, (char*) buffer + total_sent, size - total_sent);
#endif

        if (sent > 0) {
            total_sent += sent;
            Debug(D, pdTrace) << "Sent " << total_sent << " bytes out of " << size << std::endl;
        }
        else {
            if (sent < 0) {
                Debug(D, pdExcept) << "Error while sending on UN socket." << std::endl;

#ifdef _WIN32
                if (WSAGetLastError() == WSAEINTR)
#else
                if (errno == EINTR)
#endif
                { // Incoming Signal
                    if (HandlerType == stSignalInterrupt)
                        throw NetworkSignal("");
                    else
                        Debug(D, pdExcept) << "EmettreUN ignoring signal interruption." << std::endl;
                }
                else // Other errors
                {
                    perror("UN Socket(EmettreUN) : ");
                    throw NetworkError("Error while sending UN message.");
                }
            }

            if (sent == 0) {
                Debug(D, pdExcept) << "No data could be sent, connection closed?." << std::endl;
                throw NetworkError("Could not send any data on UN socket.");
            }
        }
    }
    SentBytesCount += total_sent;
    // G.Out(pdGendoc,"exit  SocketUN::send");
}

// ----------------------------------------------------------------------------
//! error.
void SocketUN::error(const char* msg)
{
    throw NetworkError("SocketUN::error <" + std::string(strerror(errno)) + "> msg = <" + std::string(msg) + ">");
}

// ----------------------------------------------------------------------------
/*! Indicates whether any data as already been read from the system socket
  and is waiting in the internal buffer
*/
bool SocketUN::isDataReady()
{
#ifdef SOCKUN_BUFFER_LENGTH
    return RBLength > 0;
#else
    return false;
#endif
}

// ----------------------------------------------------------------------------
SOCKET
SocketUN::returnSocket()
{
    return _socket_un;
}

// ----------------------------------------------------------------------------
void SocketUN::receive(const unsigned char* buffer, size_t Size)
{
    // G.Out(pdGendoc,"enter SocketUN::receive");

    assert(0 <= _socket_un);

    long nReceived = 0;

#ifndef SOCKUN_BUFFER_LENGTH
    unsigned long RBLength = 0;
#endif

    Debug(D, pdTrace) << "Beginning to receive U/W message, size " << Size << std::endl;

    while (RBLength < Size) {
#ifdef _WIN32
#ifdef SOCKUN_BUFFER_LENGTH
        nReceived = recv(_socket_un, ReadBuffer + RBLength, SOCKUN_BUFFER_LENGTH - RBLength, 0);
#else
        nReceived = recv(_socket_un, (char*) buffer + RBLength, Size - RBLength, 0);
#endif
#else
#ifdef SOCKUN_BUFFER_LENGTH
        nReceived = read(_socket_un, ReadBuffer + RBLength, SOCKUN_BUFFER_LENGTH - RBLength);
#else
        nReceived = read(_socket_un, (char*) buffer + RBLength, Size - RBLength);
#endif
#endif

        if (nReceived < 0) {
            Debug(D, pdExcept) << "Error while receiving on UN socket." << std::endl;

#ifdef _WIN32
            if (WSAGetLastError() == WSAEINTR)
#else
            if (errno == EINTR)
#endif
            { // Incoming Signal
                if (HandlerType == stSignalInterrupt)
                    throw NetworkSignal("");
                else
                    Debug(D, pdExcept) << "RecevoirUN ignoring signal interruption." << std::endl;
            }
            else { // Other errors
                perror("UN Socket(RecevoirUN) : ");
                throw NetworkError("Error while receiving UN message.");
            }
        }

        if (nReceived == 0) {
            Debug(D, pdExcept) << "UN connection has been closed by peer." << std::endl;
            throw NetworkError("Connection closed by client.");
        }
        else if (nReceived > 0) {
            RBLength += nReceived;
            RcvdBytesCount += nReceived;
        }
    }
    Debug(D, pdTrace) << "Received " << RBLength << " bytes out of " << Size << std::endl;

#ifdef SOCKUN_BUFFER_LENGTH
    memcpy(const_cast<unsigned char*>(buffer), ReadBuffer, Size);
    memmove((void*) ReadBuffer, (void*) (ReadBuffer + Size), RBLength - Size);
    RBLength -= Size;
#endif
    // G.Out(pdGendoc,"exit  SocketUN::receive");
}

} // namespace certi
