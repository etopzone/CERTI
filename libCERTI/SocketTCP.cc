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
// $Id: SocketTCP.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "SocketTCP.hh"

namespace certi {

static pdCDebug D("SOCKTCP", "(SocketTCP ) - ");

// ------------
// -- Accept --
// -------------

int SocketTCP::accept(SocketTCP *serveur)
{
  struct protoent *TCPent;
  int              optval = 1;
  socklen_t        l;

  assert(!_est_init_tcp);
  assert(serveur != NULL);

  l = sizeof(_sockIn);

  _socket_tcp = ::accept(serveur->_socket_tcp,(sockaddr*)&_sockIn, &l);
  //HPUX: _socket_tcp = accept(serveur->_socket_tcp,(sockaddr*)&_sockIn, 
  //(int*) &l);
  if(_socket_tcp < 0)
  {
    perror("SocketTCP: Accept");
    exit(-1);
  }

  // Set the TCP_NODELAY option(Server Side)
  TCPent = getprotobyname("tcp");
  if(TCPent == NULL) {
    printf("Unable to retrieve TCP protocol number.\n");
    return 0;
  }

  if(setsockopt(_socket_tcp, 
		  TCPent->p_proto, 
		  TCP_NODELAY,
		(char *) &optval, 
		  sizeof(optval))) {
    printf("Error while calling setsockopt.\n");
    return 0;
  }

  _est_init_tcp = RTI_TRUE;

  return 1;
}


// ---------
// --Bind --
// ---------

int SocketTCP::bind(unsigned int port,unsigned long addr)
{
  long Length, Result;

  assert(!_est_init_tcp);

  memset(&_sockIn, 0, sizeof(_sockIn));

  _sockIn.sin_family      = AF_INET;
  _sockIn.sin_addr.s_addr = addr;
  _sockIn.sin_port        = htons(port);

  Length = sizeof(_sockIn);

  Result = ::bind(_socket_tcp,(sockaddr *)&_sockIn, Length);

  if((Result <0) &&(errno == EADDRINUSE)){
    // Error on Bind. If the error is "Address already in use", allow
    // the user to choose to "reuse address" and then try again.
    changeReuseOption();
    Result = ::bind(_socket_tcp,(sockaddr *)&_sockIn, Length);
  }
  
  if(Result == 0)
    return 1;
  else
    return 0;
}


// -----------------------
// -- ChangeReuseOption --
// -----------------------

void SocketTCP::changeReuseOption()
{
  int on = 1;

  printf("\nTCP Server, Bind : Address already in use.\n");
  printf("If you are sure no other RTIG is running, we can try to set\n");
  printf("the \"Reuse Address\" option and try again.\n");
  printf("\tPress ENTER to try again or CTRL-C to abort.\n");

  fflush(stdin);
  getchar();
  
  // Set the SO_REUSEADDR option(Server Side)
  if(setsockopt(_socket_tcp,
		  SOL_SOCKET,
		  SO_REUSEADDR,
		(char *)&on,
		  sizeof(on))) {
    perror("Setsockopt");
  }
}


// -------------
// -- Connect --
// -------------

int SocketTCP::connect(unsigned int port,unsigned long addr)
{
  int              Result;
  struct protoent *TCPent;
  int              optval = 1;

  assert(!_est_init_tcp);

  _sockIn.sin_family=AF_INET;
  _sockIn.sin_port=htons(port);
  _sockIn.sin_addr.s_addr=addr;

  Result = ::connect(_socket_tcp,(sockaddr*)&_sockIn,sizeof(_sockIn));

  if(Result < 0)
    return 0;

  // Set the TCP_NODELAY option(Client Side)

  TCPent = getprotobyname("tcp");
  if(TCPent == NULL) {
    printf("Unable to retrieve TCP protocol number.\n");
    return 0;
  }

  if(setsockopt(_socket_tcp, 
		  TCPent->p_proto, 
		  TCP_NODELAY,
		(char *) &optval, 
		  sizeof(optval))) {
    printf("Error while calling setsockopt.\n");
    return 0;
  }

  if(Result < 0)
    return 0;
  else
    return 1;
}	


// --------------------
// -- CreerClientTCP --(with hostname)
// --------------------

int SocketTCP::createTCPClient(unsigned int  port,
			       char         *nom_serveur)
{
  struct hostent *hptr = NULL;
  unsigned long addr = 0;


  // recuperer les infos sur le serveur a partir du nom
  if((hptr = gethostbyname(nom_serveur)) == NULL)
  {
    perror("SocketTCP: gethostbyname");
    exit(-1);
  }

  memcpy((void *) &addr,(void *) hptr->h_addr, hptr->h_length);

  createTCPClient(port, addr);

  return 1;
}


// --------------------
// -- CreerClientTCP --(with address)
// --------------------

int SocketTCP::createTCPClient(unsigned int  port,
			       unsigned long addr)
{
  assert(!_est_init_tcp);

  if(!open())
  {
    perror("SocketTCP: Open");
    exit(-1);
  }

  if(!connect(port, addr))
  {
    perror("SocketTCP: Connect");
    exit(-1);
  }

  _est_init_tcp = RTI_TRUE;

  return 1;
}


// ---------------------
// -- CreerServeurTCP --
// ---------------------

int SocketTCP::createTCPServer(unsigned int port,
				unsigned long addr)
{
  assert(!_est_init_tcp);

  if(!open())
  {
    perror("SocketTCP: Open");
    exit(-1);
  }

  if(!bind(port, addr))
  { 
    perror("SocketTCP: Bind");
    exit(-1);
  }

  // BUG: On pourrait pas reduire ?
  if(!listen(MAX_FEDERATION*MAX_FEDERATE))
  {
    perror("SocketTCP: Listen"); 
    exit(-1);
  }

  _est_init_tcp = RTI_TRUE;

  return 1;
}


// ----------------
// -- SocketTCP --
// ----------------

SocketTCP::SocketTCP()
{
  _est_init_tcp = RTI_FALSE;

  SentBytesCount = 0;
  RcvdBytesCount = 0;

#ifdef SOCKTCP_BUFFER_LENGTH
  RBLength = 0;
#endif
}


// -----------------
// -- ~SocketTCP --
// -----------------

SocketTCP::~SocketTCP()
{ 
  // Fermeture
  if(_est_init_tcp)
    close();

#ifdef RTI_PRINTS_STATISTICS
  printf("\n");
  printf("TCP Socket(%d): Total Sent Bytes      : %lld.\n",
	  _socket_tcp, SentBytesCount);
  printf("TCP Socket(%d): Total Received Bytes  : %lld.\n",
	  _socket_tcp, RcvdBytesCount);
  printf("\n");
#endif
}


// -------------
// -- Emettre --
// -------------

void SocketTCP::send(void *Buffer, unsigned long Size)
  throw(NetworkError,
	 NetworkSignal)
{
  long nSent         = 0;
  long total_sent    = 0;
  long expected_size = Size;

  assert(_est_init_tcp);

  D.Out(pdDebug, "Beginning to send TCP message...");

  while(total_sent < expected_size) {
    nSent = ::send(_socket_tcp,
		(char *) Buffer + total_sent,
		 expected_size - total_sent,
		 0);

    if(nSent < 0) {
      D.Out(pdExcept, "Error while sending on TCP socket.");
      if(errno == EINTR)
	throw NetworkSignal();
      else {
	perror("TCP Socket(EmettreTCP) ");
	throw NetworkError("Error while sending TCP message.");
      }
    }

    if(nSent == 0) {
      D.Out(pdExcept, "No data could be sent, connection closed?.");
      throw NetworkError("Could not send any data on TCP socket.");
    }

    total_sent += nSent;
    D.Out(pdTrace, "Sent %ld bytes out of %ld.", total_sent, expected_size);
  }

  SentBytesCount += total_sent;

}


// ------------
// -- Fermer --
// ------------

void SocketTCP::close()
{
  if(_est_init_tcp)
  {
    ::close(_socket_tcp);
    _est_init_tcp = RTI_FALSE;
  }
}


// ------------
// -- Listen --
// ------------

int SocketTCP::listen(unsigned long howMuch)
{
  assert(!_est_init_tcp);

  return((::listen(_socket_tcp,howMuch)<0)?0:1);
}


// -------------
// -- GetAddr --
// -------------

unsigned long SocketTCP::getAddr()
{
  return(_sockIn.sin_addr.s_addr);
}


// -------------
// -- GetPort --
// -------------

unsigned int SocketTCP::getPort()
{
  return _sockIn.sin_port;
}


// -----------------
// -- IsDataReady --
// -----------------

Boolean SocketTCP::isDataReady()
{
#ifdef SOCKTCP_BUFFER_LENGTH
  if(RBLength > 0)
    return RTI_TRUE;
  else
    return RTI_FALSE;
#else
  return RTI_FALSE;
#endif
}


// ----------
// -- Open --
// ----------

int SocketTCP::open()
{
  return(((_socket_tcp=socket(AF_INET,SOCK_STREAM,0))<0)?0:1);
}


// ---------------
// -- Operator= --
// ---------------

SocketTCP & SocketTCP::operator=(SocketTCP &theSocket)
{
  _sockIn.sin_addr.s_addr=theSocket.getAddr();
  _sockIn.sin_port       =theSocket.getPort();
  _socket_tcp            =theSocket.returnSocket();

  return(*this);
}


// --------------
// -- Recevoir --
// --------------

void SocketTCP::receive(void *Buffer, unsigned long Size)
  throw(NetworkError,
	 NetworkSignal)
{
  assert(_est_init_tcp);

  long nReceived  = 0;
#ifndef SOCKTCP_BUFFER_LENGTH
  long RBLength   = 0;
#endif

  D.Out(pdDebug, "Beginning to receive TCP message...");

  while(RBLength < Size)
    {

#ifdef SOCKTCP_BUFFER_LENGTH
      nReceived = recv(_socket_tcp,
		       ReadBuffer + RBLength,
		       SOCKTCP_BUFFER_LENGTH - RBLength,
		       0);
#else
      nReceived = recv(_socket_tcp,
		(char *) Buffer + RBLength,
		       Size - RBLength,
		       0);
#endif

      if(nReceived < 0) {
	D.Out(pdExcept, "Error while receiving on TCP socket.");
	if(errno == EINTR)
	  throw NetworkSignal();
	else {
	  perror("TCP Socket(RecevoirTCP) ");
	  throw NetworkError("Error while receiving TCP message.");
	}
      }

      if(nReceived == 0) {
	D.Out(pdExcept, "TCP connection has been closed by peer.");
	throw NetworkError("Connection closed by client.");
      }

      RBLength       += nReceived;
      RcvdBytesCount += nReceived;

      D.Out(pdTrace, "Received %ld bytes out of %ld.", RBLength, Size);
    }

#ifdef SOCKTCP_BUFFER_LENGTH
  memcpy(Buffer,(void *) ReadBuffer, Size);
  memmove((void *) ReadBuffer, 
	(void *)(ReadBuffer + Size), 
	   RBLength - Size);
  RBLength -= Size;
#endif
}


// ----------------------
// -- RetournerAdresse --
// ----------------------

unsigned long SocketTCP::returnAdress()
{
  return getAddr();
}


// ---------------------
// -- RetournerSocket --
// ---------------------

int SocketTCP::returnSocket()
{
  return _socket_tcp;
}


// -------------
// -- SetPort --
// -------------

void SocketTCP::setPort(unsigned int port)
{
  _sockIn.sin_port=port;
}


// ----------------
// -- TimeOutTCP --
// ----------------

int SocketTCP::timeoutTCP(int sec, int usec)
{ int i;
  fd_set fdset;
  struct timeval timeOut;

  assert(_est_init_tcp);

  timeOut.tv_sec  = sec;
  timeOut.tv_usec = usec;
  
  FD_ZERO(&fdset);
  FD_SET(_socket_tcp,&fdset);

  while(i = select(ulimit(4,0), &fdset, NULL, NULL, &timeOut) < 0) {

    // Si c'est un signal on leve NetWorkSignal
    if(errno == EINTR)
      throw NetworkSignal("TCP::TimeOut Interrompu par un signal.");
    else
      throw NetworkError();
  }
  return((i>0)?1:0);
}

}

// $Id: SocketTCP.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
