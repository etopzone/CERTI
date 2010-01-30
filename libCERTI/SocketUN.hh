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

#ifndef CERTI_SOCKET_UN_HH
#define CERTI_SOCKET_UN_HH

#include "SocketTCP.hh"
#include "Socket.hh"
#include "PrettyDebug.hh"

namespace certi {
// Signal Handler Types for a UNIX socket : - stSignalInterrupt :
// return when read/write operation is interrupted by a signal. The
// RW operation may not be complete. - stSignalIgnore : Ignore
// signal interruption and continue to perform the RW operation till
// its done. LibRTI will use the stSignalIgnore, RTIA will use
// stSignalInterrupt.
typedef enum { stSignalInterrupt, stIgnoreSignal } SignalHandlerType ;

// This is the read buffer of UNIX sockets. It must be at least as
// long as the longest data ever received by a socket. If the next
// line is commented out, no buffer will be used at all.
//#define SOCKUN_BUFFER_LENGTH 4096

/**
 * Socket Unix handling class.
 * On windows platform this socket class is implemented using
 * a TCP socket since Unix socket are not available.
 * IMPORTANT NOTE: This UNIX socket implementation uses a Read Buffer to
 * improve global read performances(by reducing Read system calls). An
 * important drawback of this improvement is that a socket can be marked as
 * empty for the system, but in fact there is data waiting in the read
 * buffer. This is especially a problem for processes using the 'select'
 * system call: the socket won't be marked as ready for reading, because all
 * data has already been read, and is waiting in the internal buffer.
 * Therefore, before returning to a select loop, be sure to call the
 * IsDataReady method to check whether any data is waiting for processing.
 */
class CERTI_EXPORT SocketUN
{
public:
	SocketUN(SignalHandlerType theType = stSignalInterrupt);
	~SocketUN();

	int listenUN();
	int connectUN(int port);
	int acceptUN(unsigned msec);

	SOCKET socketpair();
	void setSocketFD(SOCKET fd) { _socket_un = fd; }

	bool isDataReady();

	SOCKET returnSocket();

	void send(const unsigned char *, size_t)		throw (NetworkError, NetworkSignal);
	void receive(const unsigned char *, size_t)	throw (NetworkError, NetworkSignal);

protected:
	void error(const char *) throw (NetworkError);

	SOCKET _socket_un;

	SignalHandlerType HandlerType ;

	std::string name ;

private:

	Socket::ByteCount_t SentBytesCount ;
	Socket::ByteCount_t RcvdBytesCount ;

	PrettyDebug *pD ;

#ifdef SOCKUN_BUFFER_LENGTH
// This class can use a buffer to reduce the number of systems
	// calls when reading a lot of small amouts of data. Each time a
	// Receive is made, it will try to read SOCKUN_BUFFER_LENGTH

	char ReadBuffer[SOCKUN_BUFFER_LENGTH] ;
	unsigned long RBLength ;
#endif
};

} // namespace certi

#endif // CERTI_SOCKET_UN_HH
