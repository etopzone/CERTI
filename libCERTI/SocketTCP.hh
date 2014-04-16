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

#ifndef CERTI_SOCKET_TCP_HH
#define CERTI_SOCKET_TCP_HH

#include "certi.hh"
#include "Socket.hh"

// This is the read buffer of TCP sockets. It must be at least as long
// as the longest data ever received by a socket.
// If the next line is commented out, no buffer will be used at all.
//#define SOCKTCP_BUFFER_LENGTH 4096

namespace certi {

/** This TCP socket implementation uses a Read Buffer to
  improve global read performances(by reducing Recv system calls). An
  important drawback of this improvement is that a socket can be marked as
  empty for the system, but in fact there is data waiting in the read
  buffer. This is especially a problem for processes using the 'select'
  system call: the socket won't be marked as ready for reading, because all
  data has already been read, and is waiting in the internal buffer.
  Therefore, before returning to a select loop, be sure to call the
  IsDataReady method to check whether any data is waiting for processing.
*/
class CERTI_EXPORT SocketTCP : public Socket
{
public :
	SocketTCP();
	virtual ~SocketTCP();
	virtual void close();

    virtual void createConnection(const char *server_name, unsigned int port)
        throw (NetworkError);
	void createTCPClient(in_port_t port, in_addr_t addr) throw (NetworkError);
	void createServer(in_port_t port = 0, in_addr_t addr = INADDR_ANY) throw (NetworkError);

	int accept(SocketTCP *serveur) throw (NetworkError);
	virtual void send(const unsigned char *, size_t)		throw (NetworkError, NetworkSignal);
	virtual void receive(void *Buffer, unsigned long Size)	throw (NetworkError, NetworkSignal);

	virtual bool isDataReady() const ;

	virtual unsigned long returnAdress() const ;
	
	SocketTCP &operator=(SocketTCP &theSocket);

	virtual SOCKET returnSocket();

	#ifdef _WIN32
		static bool winsockStartup();
		static void winsockShutdown();
		static bool winsockInitialized()	{ return (winsockInits > 0);}
	#endif

protected:
    int timeoutTCP(int, int);

    ByteCount_t SentBytesCount;
    ByteCount_t RcvdBytesCount;

private:
    int open();
    int connect(in_port_t port, in_addr_t addr);
    int listen(unsigned long howMuch=5);
    int bind(in_port_t port=0, in_addr_t addr=INADDR_ANY);
    void setPort(in_port_t port);
    in_port_t getPort() const ;
    in_addr_t getAddr() const ;

    SOCKET _socket_tcp;
    #ifdef _WIN32
    static int winsockInits;
    #endif
    bool _est_init_tcp;
    struct sockaddr_in _sockIn;

#ifdef SOCKTCP_BUFFER_LENGTH
    // This class can use a buffer to reduce the number of systems calls
    // when reading a lot of small amouts of data. Each time a Receive
    // is made, it will try to read SOCKTCP_BUFFER_LENGTH
    char ReadBuffer[SOCKTCP_BUFFER_LENGTH] ;
    unsigned long RBLength ;
#endif
};

} // namespace certi

#endif // CERTI_SOCKET_TCP_HH
