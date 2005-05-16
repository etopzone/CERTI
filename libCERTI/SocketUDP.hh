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

#include <sys/socket.h>
#include <netdb.h>

#define BUFFER_MAXSIZE 2000

namespace certi {

class SocketUDP : public Socket
{
public :
    SocketUDP();
    virtual ~SocketUDP();

    // Socket
    virtual void send(const unsigned char *, size_t)
        throw (NetworkError, NetworkSignal);

    virtual void receive(void * Message, unsigned long Size)
        throw (NetworkError,
               NetworkSignal);

    virtual bool isDataReady() const ;

    virtual int getClass() const { return SOCKET_TYPE_UDP ; };
    virtual int returnSocket() const ;

    virtual unsigned long returnAdress() const ;

    virtual void close();

    // SocketUDP
    void createUDPClient(unsigned int port, char *nom_serveur)
        throw (NetworkError, NetworkSignal);

    void createUDPServer(unsigned int port)
        throw (NetworkError, NetworkSignal);

    void attach(int socket_ouvert, unsigned long Adresse, unsigned int port)
        throw (NetworkError, NetworkSignal);

    unsigned int getPort() const ;
    unsigned long getAddr() const ;

private:
    void setPort(unsigned int port);

    int bind();
    int open();
    
    bool PhysicalLink ; ///< tak indicating physical or logical link

    long _socket_udp ;
    struct sockaddr_in sock_local ;

    struct sockaddr_in sock_source ;
    char *Addr_Source ;
    unsigned int Port_Source ;
    struct sockaddr_in sock_distant ;
    struct hostent * hp_distant ;

    int _sock_local_length ;
    bool _est_init_udp ;

    ByteCount SentBytesCount ;
    ByteCount RcvdBytesCount ;
    unsigned long BufferSize ;
    char Buffer[4096] ;

    struct hostent * hp_local ;
};

} // namespace certi

#endif // CERTI_SOCKET_UDP_HH
