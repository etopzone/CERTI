// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libcerti
//
// CERTI-libcerti is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libcerti is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: SocketTCP.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

// $Id: SocketTCP.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_SOCKET_TCP_HH
#define _CERTI_SOCKET_TCP_HH

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <cstring>
#include <ulimit.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/tcp.h>

#include "Socket.hh"
#include "RTItypes.hh"
#include "PrettyDebug.hh"

// This is the read buffer of TCP sockets. It must be at least as long
// as the longest data ever received by a socket.
// If the next line is commented out, no buffer will be used at all.
#define SOCKTCP_BUFFER_LENGTH 4096

// IMPORTANT NOTE: This TCP socket implementation uses a Read Buffer to
// improve global read performances(by reducing Recv system calls). 
// An important drawback of this improvement is that a socket can be
// marked as empty for the system, but in fact there is data waiting 
// in the read buffer. This is especially a problem for processes using
// the 'select' system call: the socket won't be marked as ready for reading,
// because all data has already been read, and is waiting in the internal
// buffer. Therefore, before returning to a select loop, be sure to call
// the IsDataReady method to check whether any data is waiting for processing.

namespace certi {

class SocketTCP : public Socket
{
public :
  // ---------------------------------------------
  // -- Fonctions heritee de la classe Socket --
  // ---------------------------------------------

  void send(void *Buffer, unsigned long Size)
    throw(NetworkError, NetworkSignal);

  void receive(void *Buffer, unsigned long Size)
    throw(NetworkError, NetworkSignal);

  // Return RTI_TRUE if any data as already been read from the system
  // socket and is waiting in the internal buffer, else RTI_FALSE.
  Boolean isDataReady();

  int getClass() { return SOCKET_TYPE_TCP; };
  int returnSocket();

  unsigned long returnAdress();

  void close();
 
  // --------------------------
  // -- TCP Specific Methods --
  // --------------------------

  SocketTCP();
  ~SocketTCP();

  int accept(SocketTCP *serveur);

  int createTCPClient(unsigned int port, char *nom_serveur);
  int createTCPClient(unsigned int port, unsigned long addr);
  int 
  createTCPServer(unsigned int port = 0, unsigned long addr = INADDR_ANY);

  SocketTCP & operator=(SocketTCP &theSocket);

private:

  // ------------------------
  // -- Private Attributes --
  // ------------------------

  long _socket_tcp;

  struct sockaddr_in _sockIn;
  Boolean _est_init_tcp;

  unsigned long long SentBytesCount;
  unsigned long long RcvdBytesCount;

#ifdef SOCKTCP_BUFFER_LENGTH
  // This class can use a buffer to reduce the number of systems calls
  // when reading a lot of small amouts of data. Each time a Receive
  // is made, it will try to read SOCKTCP_BUFFER_LENGTH 

  char ReadBuffer [SOCKTCP_BUFFER_LENGTH];
  unsigned long RBLength;
#endif

  // ---------------------
  // -- Private Methods --
  // ---------------------

  unsigned int getPort();
  unsigned long getAddr();
  void setPort(unsigned int port);
 
  int bind(unsigned int port=0, unsigned long addr=INADDR_ANY);
  void changeReuseOption();
  int connect(unsigned int port, unsigned long addr);
  int listen(unsigned long howMuch=5);
  int open();
  int timeoutTCP(int, int);
 
};
}

#endif // _CERTI_SOCKET_TCP_HH

// $Id: SocketTCP.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
