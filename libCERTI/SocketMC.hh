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

#ifndef CERTI_SOCKET_MC_HH
#define CERTI_SOCKET_MC_HH

#include "Socket.hh"
#include "NetworkMessage.hh"

#ifdef _WIN32					//dotNet
	#ifndef _WINSOCK2API_
	#ifndef _WINSOCKAPI_
	#include <winsock.h>
	#endif
	#endif
#if __MINGW32__
 	#define IP_ADD_MEMBERSHIP	5
	struct ip_mreq {
		struct in_addr imr_multiaddr;
		struct in_addr imr_interface;
	};
#endif
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
#endif
namespace certi {

class SocketMC : public Socket
{
public:
	int _socket_mc ;
	bool _est_init_mc ;

	SocketMC();
	virtual ~SocketMC();

	void CreerSocketMC(char *addr, unsigned long port);
	void CreerSocketMC(unsigned long addr, unsigned long port);

	virtual void send(const unsigned char *, size_t)
	  throw (NetworkError, NetworkSignal);
	virtual void receive(void *Buffer, unsigned long Size)
	  throw (NetworkError, NetworkSignal);

	#ifdef _WIN32
                SOCKET returnSocket();
	#else
		virtual int returnSocket();
	#endif

	virtual unsigned long returnAdress() const ;
	virtual int getClass() const { return SOCKET_TYPE_MC ; };
	virtual void close();

	void sendMC(NetworkMessage *msg);
	char *receiveMC(NetworkMessage *msg);
	
private:
	struct sockaddr_in _sin ;
	struct sockaddr_in _sin_e ;
	#ifdef _WIN32					//dotNet
		int _sinlen;
		SOCKET _socket_emetteur;
	#else
		socklen_t _sinlen;
		int _socket_emetteur;
	#endif    
	int _sinlen_e ;

	int timeoutMC(int, int);

	int _num_msg ;
};

} // namespace certi

#endif // CERTI_SOCKET_MC_HH
