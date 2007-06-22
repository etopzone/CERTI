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

#ifndef CERTI_SOCKET_HH
#define CERTI_SOCKET_HH

#include "certi.hh"
	#ifdef _WIN32
	#ifndef _WINSOCK2API_
		#ifndef _WINSOCKAPI_
			#include <winsock2.h>
		#endif
	#endif
#else
	#include <sys/time.h>
	#include <sys/types.h>
	#include <netinet/in.h>
#endif

// Those values are returned by the GetClass method.
#define SOCKET_TYPE_TCP 0 // TCP Socket class ID
#define SOCKET_TYPE_MC 1 // Multicast Socket class ID
#define SOCKET_TYPE_UDP 2
#define SOCKET_TYPE_S_TCP 3 // Secure TCP Socket class ID

namespace certi {

typedef unsigned long ByteCount ;

class Socket
{
public:
	virtual ~Socket() {};

	virtual void send(const unsigned char *, size_t) = 0 ;
	virtual void receive(void *Buffer, unsigned long Size) = 0 ;
	virtual void close() = 0 ;

	// This method may be used for implementation using Read Buffers,
	// because in that case 'select' system calls are not trustworthy.
	// See Important Note in SocketTCP.hh
	virtual bool isDataReady() const = 0 ;

	virtual int getClass() const = 0 ;
	virtual unsigned long returnAdress() const = 0 ;

	#ifdef _WIN32
		virtual SOCKET returnSocket() = 0;
	#else
		virtual int returnSocket() = 0;
	#endif
};

} // namespace certi

#endif // CERTI_SOCKET_HH
