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

#ifndef CERTI_SOCKET_UDP_HH
#define CERTI_SOCKET_UDP_HH

#include "Socket.hh"

#define BUFFER_MAXSIZE 2000

namespace certi {

class CERTI_EXPORT SocketUDP : public Socket
{
public :
	SocketUDP();
	virtual ~SocketUDP();

	// Socket
	virtual void send(const unsigned char *, size_t)
	throw (NetworkError, NetworkSignal);

	virtual void receive(void * Message, unsigned long Size)
		throw (NetworkError,	NetworkSignal);

	virtual bool isDataReady() const ;

		SOCKET returnSocket();
	virtual unsigned long returnAdress() const ;

	virtual void close();

	// SocketUDP
    virtual void createConnection(const char *server_name, unsigned int port)
        throw (NetworkError);

	void createServer(unsigned int port, in_addr_t addr = INADDR_ANY)
	throw (NetworkError, NetworkSignal);

	void attach(int socket_ouvert, unsigned long Adresse, unsigned int port)
	throw (NetworkError, NetworkSignal);

	unsigned int getPort() const ;
	unsigned long getAddr() const ;

private:
	void setPort(unsigned int port);

	int bind();
	int open();

	bool PhysicalLink ; ///< tag indicating physical or logical link

	SOCKET _socket_udp;
	struct sockaddr_in sock_local ;

	struct sockaddr_in sock_source ;
	char *Addr_Source ;
	unsigned int Port_Source ;
	struct sockaddr_in sock_distant ;
	struct hostent * hp_distant ;

	int _sock_local_length ;
	bool _est_init_udp ;

	ByteCount_t SentBytesCount ;
	ByteCount_t RcvdBytesCount ;

	unsigned long BufferSize ;
	char Buffer[4096] ;
};

} // namespace certi

#endif // CERTI_SOCKET_UDP_HH
