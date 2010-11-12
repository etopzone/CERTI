// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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

#include "certi.hh"
#include "SocketUN.hh"
#include "SocketTCP.hh"

#include <cstring>
#include <cerrno>
#include <cstdio>
#include <cassert>

#include <string>
#include <iostream>
#ifndef _WIN32
#include <unistd.h>
#endif

using std::string ;
using std::cout ;
using std::endl ;

#ifdef _WIN32
extern "C" int socketpair_win32(SOCKET socks[2], int make_overlapped);
#endif

namespace certi {
static PrettyDebug G("GENDOC",__FILE__);

#ifndef _WIN32
static void closesocket(int fd)
{
  close(fd);
}
#endif

int
SocketUN::listenUN()
{
  struct sockaddr_in addr;
#ifdef _WIN32
  int  addrlen = sizeof(addr);
#else
  socklen_t addrlen = sizeof(addr);
#endif

  if ((_socket_un = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    return -1;
  pD->Out(pdInit, "Server: Created IPV4 socket.");

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;

  if (bind(_socket_un, (const struct sockaddr*) &addr, sizeof(addr)) == -1) {
    closesocket(_socket_un);
    _socket_un = -1;
    return -1;
  }
  pD->Out(pdInit, "Server: Bound to anonymous IPV4 address.");

  if (getsockname(_socket_un, (struct sockaddr*) &addr, &addrlen) == -1) {
    closesocket(_socket_un);
    _socket_un = -1;
    return -1;
  }
  pD->Out(pdInit, "Server: Got server port address %d.", ntohs(addr.sin_port));

  if (listen(_socket_un, 1) == -1) {
    closesocket(_socket_un);
    _socket_un = -1;
    return -1;
  }
  pD->Out(pdInit, "Server: Now listening.");

  return ntohs(addr.sin_port);
}

// ----------------------------------------------------------------------------
//! Called by client to connect.
int
SocketUN::connectUN(int port)
{
  if ((_socket_un = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    return -1;
  pD->Out(pdInit, "Client: Created IPV4 socket.");

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = htons(port);
  if (connect(_socket_un, (const struct sockaddr*) &addr, sizeof(addr)) == -1)
    return -1;
  pD->Out(pdInit, "Client: Connected to IPV4 address at port %d.", port);

  return 0;
}

// ----------------------------------------------------------------------------
//! Called by server to open the socket and wait for the connection.
int
SocketUN::acceptUN(unsigned msec)
{
  fd_set fdset ;
  FD_ZERO(&fdset);
  FD_SET(_socket_un, &fdset);

  struct timeval timeout;
  timeout.tv_sec = msec/1000;
  timeout.tv_usec = 1000*(msec%1000);

  pD->Out(pdInit, "Server: Waiting for a connection to accept.");
  if (select(_socket_un + 1, &fdset, NULL, NULL, &timeout) <= 0) {
    closesocket(_socket_un);
    _socket_un = -1;
    return -1;
  }

  pD->Out(pdInit, "Server: Accepting connection.");
  SOCKET accepted;
  if ((accepted = accept(_socket_un, NULL, NULL)) == -1) {
    closesocket(_socket_un);
    _socket_un = -1;
    return -1;
  }
  pD->Out(pdInit, "Server: Accepted IPV4 connection.");

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
int
SocketUN::socketpair()
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
    : _socket_un(-1),
      HandlerType(theType), SentBytesCount(0), RcvdBytesCount(0)
{
#ifdef _WIN32
	SocketTCP::winsockStartup();
#endif

#ifdef SOCKUN_BUFFER_LENGTH
	RBLength = 0 ;
#endif

 pD = new PrettyDebug("SOCKUN", "SocketUN");
 pD->Out(pdInit, "UNIX Socket created.");
}

// ----------------------------------------------------------------------------
//! Close the socket.
SocketUN::~SocketUN()
{
  if (0 <= _socket_un)
    closesocket(_socket_un);

  pD->Out(pdTerm, "SocketUN: Closed all sockets.");

#ifdef _WIN32
  SocketTCP::winsockShutdown();
#endif

pD->Out(pdCom, "Unix Socket %2d : total = %9db sent", _socket_un, SentBytesCount ) ;
pD->Out(pdCom, "Unix Socket %2d : total = %9db received", _socket_un, RcvdBytesCount ) ;

delete pD ;
}

// ----------------------------------------------------------------------------
//! send Message using Socket UNIX.
/**    @param buffer buffer to send
       @param size   buffer size
*/
void
SocketUN::send(const unsigned char *buffer, size_t size)
    throw (NetworkError, NetworkSignal)
{
long sent = 0 ;
unsigned long total_sent = 0 ;

// G.Out(pdGendoc,"enter SocketUN::send");
assert(0 <= _socket_un);

pD->Out(pdTrace, "Beginning to send UN message...");

while (total_sent < size)
	{
	#ifdef _WIN32
		sent = ::send(_socket_un, (char *) buffer + total_sent, size - total_sent, 0);
	#else
		sent = write(_socket_un, (char *) buffer + total_sent, size - total_sent);
	#endif

	if (sent > 0)
		{
		total_sent += sent ;
		pD->Out(pdTrace, "Sent %ld bytes out of %ld.", total_sent, size);
		}
	else
		{
		if (sent < 0)
			{
			pD->Out(pdExcept, "Error while sending on UN socket.");

			#ifdef _WIN32
				if(WSAGetLastError() == WSAEINTR)
			#else
				if(errno == EINTR)
			#endif
				{// Incoming Signal
				if (HandlerType == stSignalInterrupt) throw NetworkSignal("");
				else pD->Out(pdExcept, "EmettreUN ignoring signal interruption.");
				}
			else // Other errors
				{
				perror("UN Socket(EmettreUN) : ");
					throw NetworkError("Error while sending UN message.");
			}	}

		if (sent == 0)
			{
			pD->Out(pdExcept, "No data could be sent, connection closed?.");
			throw NetworkError("Could not send any data on UN socket.");
			}
		}
	}
SentBytesCount += total_sent ;
// G.Out(pdGendoc,"exit  SocketUN::send");
}

// ----------------------------------------------------------------------------
//! error.
void SocketUN::error(const char *msg) throw (NetworkError)
{
    throw NetworkError(stringize() <<
        "SocketUN::error <" << strerror(errno) << "> msg = <" << msg <<">");
}

// ----------------------------------------------------------------------------
/*! Indicates whether any data as already been read from the system socket
  and is waiting in the internal buffer
*/
bool
SocketUN::isDataReady()
{
#ifdef SOCKUN_BUFFER_LENGTH
    return RBLength > 0 ;
#else
    return false ;
#endif
}

// ----------------------------------------------------------------------------
SOCKET
SocketUN::returnSocket()
{
    return _socket_un;
}

// ----------------------------------------------------------------------------
void
SocketUN::receive(const unsigned char *buffer, size_t Size)
    throw (NetworkError, NetworkSignal)
{
// G.Out(pdGendoc,"enter SocketUN::receive");

assert(0 <= _socket_un);

long nReceived = 0 ;

#ifndef SOCKUN_BUFFER_LENGTH
	unsigned long RBLength = 0 ;
#endif

pD->Out(pdTrace, "Beginning to receive U/W message...(Size  %ld)",Size);

while (RBLength < Size)
	{
	#ifdef _WIN32
		#ifdef SOCKUN_BUFFER_LENGTH
				nReceived = recv(_socket_un,
						 ReadBuffer + RBLength,
						 SOCKUN_BUFFER_LENGTH - RBLength,
						 0);
		#else
				nReceived = recv(_socket_un,
						 (char *) buffer + RBLength,
						 Size - RBLength,
						 0);
		#endif
	#else
		#ifdef SOCKUN_BUFFER_LENGTH
		nReceived = read(_socket_un, ReadBuffer + RBLength, SOCKUN_BUFFER_LENGTH - RBLength);
		#else
		nReceived = read(_socket_un, (char *) buffer + RBLength, Size - RBLength);
		#endif
	#endif

	if (nReceived < 0)
		{
		pD->Out(pdExcept, "Error while receiving on UN socket.");

		#ifdef _WIN32
			if(WSAGetLastError() == WSAEINTR)
		#else
			if(errno == EINTR)
		#endif
			{// Incoming Signal
			if (HandlerType == stSignalInterrupt)
				throw NetworkSignal("");
			else
				pD->Out(pdExcept, "RecevoirUN ignoring signal interruption.");
			}
		else
			{// Other errors
			perror("UN Socket(RecevoirUN) : ");
			throw NetworkError("Error while receiving UN message.");
			}
		}

	if (nReceived == 0)
		{
		pD->Out(pdExcept, "UN connection has been closed by peer.");
		throw NetworkError("Connection closed by client.");
		}
	else if (nReceived > 0)
		{
		RBLength += nReceived ;
		RcvdBytesCount += nReceived ;
		}
	}
pD->Out(pdTrace, "Received %ld bytes out of %ld.", RBLength, Size);

#ifdef SOCKUN_BUFFER_LENGTH
memcpy(const_cast<unsigned char *>(buffer), ReadBuffer, Size);
memmove((void *) ReadBuffer,
(void *)(ReadBuffer + Size),
RBLength - Size);
RBLength -= Size ;
#endif
// G.Out(pdGendoc,"exit  SocketUN::receive");
}

} // namespace certi
