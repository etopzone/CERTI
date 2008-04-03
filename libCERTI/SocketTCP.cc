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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: SocketTCP.cc,v 3.22 2008/04/03 15:21:52 rousse Exp $
// ----------------------------------------------------------------------------

#ifdef _WIN32
	#define	EADDRINUSE		WSAEADDRINUSE
	#include <winsock2.h>
#else
	#include <netdb.h>
	#include <unistd.h>
	#include <netinet/tcp.h>
	#include <iostream>
#endif

#include "SocketTCP.hh"
#include "PrettyDebug.hh"
#include "baseTypes.hh"

#include <assert.h>
#include <errno.h>

using std::cout ;
using std::endl ;

namespace certi {

static pdCDebug D("SOCKTCP", "(SocketTCP) - ");
static PrettyDebug G("GENDOC",__FILE__);

#ifdef _WIN32
int SocketTCP::winsockInits = 0;

bool SocketTCP::winsockStartup()
{
WORD wVersionRequested;
WSADATA wsaData;
int lError;

if (winsockInits > 0)
	{
	winsockInits++;
	return RTI_TRUE;
	} 
else if (winsockInits <0)
	{
	return RTI_FALSE;
	}

wVersionRequested = MAKEWORD( 2, 0 );
lError = WSAStartup( wVersionRequested, &wsaData );
if (lError != 0)
	{
	winsockInits = -1;
	return RTI_FALSE;
	}

if (LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0)
	{// Tell the user that we couldn't find a usable WinSock DLL.    
	WSACleanup( );
	winsockInits = -1;
	return RTI_FALSE; 
	}

winsockInits = 1;
return RTI_TRUE;
}

void SocketTCP::winsockShutdown()
{
winsockInits--;
if (winsockInits==0)
	WSACleanup( );
}
#endif

// ----------------------------------------------------------------------------
SocketTCP::SocketTCP()
{
_est_init_tcp = false ;

SentBytesCount = 0 ;
RcvdBytesCount = 0 ;

#ifdef _WIN32
	winsockStartup();
#endif

#ifdef SOCKTCP_BUFFER_LENGTH
    RBLength = 0 ;
#endif
}

// ----------------------------------------------------------------------------
SocketTCP::~SocketTCP()
{// Fermeture
if (_est_init_tcp)
	close();

#ifdef _WIN32
	winsockShutdown();
#endif

#ifdef RTI_PRINTS_STATISTICS
    cout << " TCP Socket " ;
    cout.width(2);
    cout << _socket_tcp << " : total = " ;
    cout.width(9);
    cout << SentBytesCount << "b sent " << endl ;
    cout << " TCP Socket " ;
    cout.width(2);
    cout << _socket_tcp << " : total = " ;
    cout.width(9);
    cout << RcvdBytesCount << "b received" << endl ;
#endif
}

// ----------------------------------------------------------------------------
int SocketTCP::connect(in_port_t port, in_addr_t addr)
{
int Result ;
struct protoent *TCPent ;
int optval = 1 ;

assert(!_est_init_tcp);

_sockIn.sin_family=AF_INET ;
_sockIn.sin_port=htons(port);
_sockIn.sin_addr.s_addr=addr ;


Result = ::connect(_socket_tcp, (sockaddr*)&_sockIn, sizeof(_sockIn));

if (Result < 0)
	return 0 ;

// Set the TCP_NODELAY option(Client Side)

TCPent = getprotobyname("tcp");
if (TCPent == NULL) 
	{
	cout << "Unable to retrieve TCP protocol number." << endl ;
	return 0 ;
	}

if (setsockopt(_socket_tcp,
				TCPent->p_proto,
				TCP_NODELAY,
				(char *) &optval,
				sizeof(optval))) 
	{
	cout << "Error while calling setsockopt." << endl ;
	return 0 ;
	}

if (Result < 0)
	return 0 ;
else
	return 1 ;
}

// ----------------------------------------------------------------------------
int SocketTCP::accept(SocketTCP *serveur) throw (NetworkError)
{
	std::stringstream msg;
struct protoent *TCPent ;
int optval = 1 ;

#ifdef _WIN32
	int			l;
#else
	socklen_t	l;
#endif

assert(!_est_init_tcp);
assert(serveur != NULL);

l = sizeof(_sockIn);

_socket_tcp = ::accept(serveur->_socket_tcp, (sockaddr*)&_sockIn, &l);
if (_socket_tcp < 0) 
	{
	  msg << "SocketTCP: Accept Failed"
	      << "<" << strerror(errno) <<">"; 
	  throw NetworkError(msg.str().c_str());	
	}

// Set the TCP_NODELAY option(Server Side)
TCPent = getprotobyname("tcp");
if (TCPent == NULL) 
	{
	cout << "Unable to retrieve TCP protocol number." << endl ;
	return 0 ;
	}

if (setsockopt(_socket_tcp,
					TCPent->p_proto,
					TCP_NODELAY,
					(char *) &optval,
					sizeof(optval))) 
	{
	cout << "Error while calling setsockopt." << endl ;
	return 0 ;
	}

_est_init_tcp = true ;

return 1 ;
}

// ----------------------------------------------------------------------------
int
SocketTCP::bind(in_port_t port, in_addr_t addr)
{
long Length, Result ;

assert(!_est_init_tcp);

memset(&_sockIn, 0, sizeof(_sockIn));

_sockIn.sin_family = AF_INET ;
_sockIn.sin_addr.s_addr = addr ;
_sockIn.sin_port = htons(port);

Length = sizeof(_sockIn);

Result = ::bind(_socket_tcp, (sockaddr *)&_sockIn, Length);

#ifdef _WIN32
	if((Result <0) &&(WSAGetLastError() == WSAEADDRINUSE))
#else
	if((Result <0) &&(errno == EADDRINUSE))
#endif
	{// Error on Bind. If the error is "Address already in use", allow
	// the user to choose to "reuse address" and then try again.
	changeReuseOption();
	Result = ::bind(_socket_tcp, (sockaddr *)&_sockIn, Length);
	}

if (Result == 0)
	return 1 ;
else
	return 0 ;
}

// ----------------------------------------------------------------------------
void
SocketTCP::changeReuseOption()
{
int on = 1 ;

cout << endl << "TCP Server, Bind : Address already in use." << endl ;
cout << "If you are sure no other RTIG is running, we can try to set" << endl ;
cout << "the \"Reuse Address\" option and try again." << endl ;
cout << "\tPress ENTER to try again or CTRL-C to abort." << endl ;

fflush(stdin); getchar();

// Set the SO_REUSEADDR option(Server Side)
if (setsockopt(_socket_tcp,
					SOL_SOCKET,
					SO_REUSEADDR,
					(char *)&on,
					sizeof(on))) 
	{
	perror("Setsockopt");
	}
}

// ----------------------------------------------------------------------------
void
SocketTCP::createTCPClient(in_port_t port, const char *nom_serveur)
throw (NetworkError)
{
	std::stringstream msg;
	// get host information from server name
	// this may perform DNS query
	struct hostent *hptr = gethostbyname(nom_serveur);
	if (NULL == hptr)
	{
		msg << "gethostbyname gave NULL answer for hostname <"
		<< nom_serveur
		<< "> with error <"
		<< strerror(errno)
		<< ">";
		throw NetworkError(msg.str().c_str());
	}

	in_addr_t addr = 0;
	memcpy((void *) &addr, (void *) hptr->h_addr, hptr->h_length);

	createTCPClient(port, addr);
}

// ----------------------------------------------------------------------------
void
SocketTCP::createTCPClient(in_port_t port, in_addr_t addr) 
throw (NetworkError)
{
assert(!_est_init_tcp);
std::stringstream msg;
if (!open()) 
	{
		msg << "Cannot open port <" << port 
			<< "> on addr <" << addr2string(addr)
			<< "> : error =" << strerror(errno);
		throw NetworkError(msg.str().c_str());
	}

if (!connect(port, addr)) 
	{
		msg << "Cannot connect port <" << port 
		    << "> on addr <" << addr2string(addr)
			<< "> : error =" << strerror(errno);
		throw NetworkError(msg.str().c_str());	
	}

_est_init_tcp = true ;
}

// ----------------------------------------------------------------------------
void
SocketTCP::createTCPServer(in_port_t port, in_addr_t addr)
throw (NetworkError)
{
	std::stringstream msg;
	assert(!_est_init_tcp);


if (!open()) 
	{
	msg << "Cannot open port <" << port 
		<< "> on addr <" << addr2string(addr)
		<< "> : error =" << strerror(errno);
	throw NetworkError(msg.str().c_str());
	}

if (!bind(port, addr)) 
	{
	msg << "Cannot bind port <" << port 
		<< "> on addr <" << addr2string(addr)
		<< "> : error =" << strerror(errno);
	throw NetworkError(msg.str().c_str());
	}

if (!listen(MAX_BACKLOG)) 
	{
	msg << "Cannot listen port <" << port 
		<< "> on addr <" << addr2string(addr)
		<< "> : error =" << strerror(errno);
	throw NetworkError(msg.str().c_str());
	}

_est_init_tcp = true ;
}


// ----------------------------------------------------------------------------
void
SocketTCP::send(const unsigned char *buffer, size_t size)
					 throw (NetworkError, NetworkSignal)
{
long total_sent = 0 ;
long expected_size = size ;

assert(_est_init_tcp);

D.Out(pdDebug, "Beginning to send TCP message...");

while (total_sent < expected_size) 
	{
	#ifdef _WIN32
		int sent = ::send(_socket_tcp, (char*) buffer + total_sent, expected_size - total_sent, 0);
	#else
		int sent = ::send(_socket_tcp, buffer + total_sent, expected_size - total_sent, 0);
	#endif

	if (sent < 0) 
		{
		D.Out(pdExcept, "Error while sending on TCP socket.");

		#ifdef _WIN32
			if(WSAGetLastError() == WSAEINTR)
		#else
			if(errno == EINTR)
		#endif
			throw NetworkSignal("");
		else 
			{
			perror("TCP Socket(EmettreTCP) ");
			throw NetworkError("Error while sending TCP message.");
			}
		}

	if (sent == 0) 
		{
		D.Out(pdExcept, "No data could be sent, connection closed?.");
		throw NetworkError("Could not send any data on TCP socket.");
		}

	total_sent += sent ;
	D.Out(pdTrace, "Sent %ld bytes out of %ld.", total_sent, expected_size);
	}

SentBytesCount += total_sent ;
}

// ----------------------------------------------------------------------------
void
SocketTCP::close()
{
if (_est_init_tcp) 
	{
	#ifdef _WIN32
		::closesocket(_socket_tcp);
	#else
		::close(_socket_tcp);
	#endif
	_est_init_tcp = false ;
	}
}

// ----------------------------------------------------------------------------
int
SocketTCP::listen(unsigned long howMuch)
{
assert(!_est_init_tcp);

return ::listen(_socket_tcp, howMuch) >= 0 ;
}

// ----------------------------------------------------------------------------
in_addr_t
SocketTCP::getAddr() const
{
return(_sockIn.sin_addr.s_addr);
}

// ----------------------------------------------------------------------------
in_port_t
SocketTCP::getPort() const
{
return _sockIn.sin_port ;
}

// ----------------------------------------------------------------------------
/*! Return RTI_TRUE if any data as already been read from the system socket
  and is waiting in the internal buffer, else RTI_FALSE.
*/
bool
SocketTCP::isDataReady() const
{
#ifdef SOCKTCP_BUFFER_LENGTH
	return RBLength > 0 ;
#else
	return false ;
#endif
}

// ----------------------------------------------------------------------------
int
SocketTCP::open()
{
#ifdef _WIN32
	assert(winsockInits>0);
#endif
	return(((_socket_tcp=socket(AF_INET,SOCK_STREAM,0))<0)?0:1);
}

// ----------------------------------------------------------------------------
SocketTCP &
SocketTCP::operator=(SocketTCP &theSocket)
{
_sockIn.sin_addr.s_addr=theSocket.getAddr();
_sockIn.sin_port =theSocket.getPort();
_socket_tcp =theSocket.returnSocket();

return(*this);
}

// ----------------------------------------------------------------------------
void
SocketTCP::receive(void *buffer, unsigned long size)
						throw (NetworkError, NetworkSignal)
{
// G.Out(pdGendoc,"enter SocketTCP::receive");
assert(_est_init_tcp);

long nReceived = 0 ;
#ifndef SOCKTCP_BUFFER_LENGTH
long RBLength = 0 ;
#endif

D.Out(pdDebug, "Beginning to receive TCP message...(Size  %ld)",size);

while (RBLength < size)
	{

	#ifdef SOCKTCP_BUFFER_LENGTH
	nReceived = recv(_socket_tcp,
							ReadBuffer + RBLength,
							SOCKTCP_BUFFER_LENGTH - RBLength,
							0);
	#else
	nReceived = recv(_socket_tcp,
							(char *) buffer + RBLength,
							size - RBLength,
							0);
	#endif

	if (nReceived < 0) 
		{
		D.Out(pdExcept, "Error while receiving on TCP socket.");
		#ifdef _WIN32
			if(WSAGetLastError() == WSAEINTR)
		#else
			if(errno == EINTR)
		#endif
			throw NetworkSignal("");
		else 
			{
			perror("TCP Socket(RecevoirTCP) ");
			throw NetworkError("Error while receiving TCP message.");
			}
		}

	if (nReceived == 0) 
		{
		D.Out(pdExcept, "TCP connection has been closed by peer.");
		throw NetworkError("Connection closed by client.");
		}

	RBLength += nReceived ;
	RcvdBytesCount += nReceived ;
	}
D.Out(pdTrace, "Received %ld bytes out of %ld.", RBLength, size);

#ifdef SOCKTCP_BUFFER_LENGTH
	memcpy(buffer, (void *) ReadBuffer, size);
	memmove((void *) ReadBuffer, (void *)(ReadBuffer + size), RBLength - size);
	RBLength -= size ;
#endif
// G.Out(pdGendoc,"exit  SocketTCP::receive");
}

// ----------------------------------------------------------------------------
unsigned long
SocketTCP::returnAdress() const
{
return getAddr();
}

// ----------------------------------------------------------------------------
#ifdef _WIN32
SOCKET SocketTCP::returnSocket()	{ return _socket_tcp ;}
#else
int SocketTCP::returnSocket()		{ return _socket_tcp ;}
#endif

// ----------------------------------------------------------------------------
//! Change the port value.
void SocketTCP::setPort(in_port_t port)
{
_sockIn.sin_port=port ;
}

// ----------------------------------------------------------------------------
/** Wait for a socket event, until a time-out. The time-out is given
    with seconds and microseconds in parameter.
    @param sec seconds
    @param usec microseconds
    @return 1 if there are waiting events, 0 if the wait timed out
    @exception NetworkSignal and NetworkError
 */
int
SocketTCP::timeoutTCP(int sec, int usec)
{
assert(_est_init_tcp);

struct timeval time_out ;    
time_out.tv_sec = sec ;
time_out.tv_usec = usec ;

fd_set fdset ;
FD_ZERO(&fdset);
FD_SET(_socket_tcp, &fdset);

int nb = select(_socket_tcp+1, &fdset, NULL, NULL, &time_out);

if (nb < 0) 
	{
	#ifdef _WIN32
		 if(WSAGetLastError() == WSAEINTR)
	#else
		 if(errno == EINTR)
	#endif
		throw NetworkSignal("TCP::TimeOut signal interrupt.");
	else
		throw NetworkError("Select gave negative return value");
	}
else 
	return nb > 0 ;
}

} // namespace

// $Id: SocketTCP.cc,v 3.22 2008/04/03 15:21:52 rousse Exp $
