// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: SocketMC.cc,v 3.3 2003/01/15 10:12:29 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "SocketMC.hh"

namespace certi {

//-------------------------------------------------------------------
SocketMC::SocketMC()
{
  _est_init_mc = RTI_FALSE;
  _num_msg = 1;
}


//-------------------------------------------------------------------
SocketMC::~SocketMC()
{
  if(_est_init_mc)
  {
    ::close(_socket_mc);
    ::close(_socket_emetteur);
    _est_init_mc = RTI_FALSE;
  }
}


//-------------------------------------------------------------------
void SocketMC::CreerSocketMC(char *addr, unsigned long port)
{
  assert(addr!=NULL);

  CreerSocketMC(inet_addr(addr), port);
}


//-------------------------------------------------------------------
void SocketMC::
CreerSocketMC(unsigned long addr, unsigned long port)
{
  struct ip_mreq _mreq;
  unsigned long _mreqlen;

  assert(!_est_init_mc);
  assert(addr>0);

  // creation du socket recepteur
  _socket_mc = socket(AF_INET, SOCK_DGRAM, 0);
  if(_socket_mc < 0)
  {
    perror("socket1");
    exit(-1);
  }

  memset(&_sin, sizeof(_sin), sizeof(_sin));
  _sin.sin_family = AF_INET; 
  _sin.sin_addr.s_addr = htonl(INADDR_ANY); 
  _sin.sin_port = htons(port); 
  _sinlen = sizeof(_sin);

  if(bind(_socket_mc,(struct sockaddr *)&_sin, _sinlen) < 0)
  {
    perror("SocketMC: bind");
    exit(-1);
  }

  // joindre le groupe multiCast

  _mreq.imr_multiaddr.s_addr = addr;
  _mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  _mreqlen = sizeof(_mreq);
  if(setsockopt(_socket_mc,
                IPPROTO_IP,
                IP_ADD_MEMBERSHIP,
(char *)&_mreq, _mreqlen) < 0)
  {
    perror("setsockopt"); 
    exit(-1);
  }

  // creation du socket emetteur

  _socket_emetteur = socket(AF_INET, SOCK_DGRAM, 0);
  if(_socket_emetteur < 0)
  {
    perror("socket2");
    exit(-1);
  }

  memset(&_sin_e, sizeof(_sin_e), sizeof(_sin_e));
  _sin_e.sin_family = AF_INET; 
  _sin_e.sin_port = htons(port); 
  _sin_e.sin_addr.s_addr = addr; 
  _sinlen_e = sizeof(_sin_e);

  // l'initialisation de la communication multicast est reussie
  _est_init_mc = RTI_TRUE;
}



//-------------------------------------------------------------------
void 
SocketMC::send(void *Buffer, unsigned long)
  throw(NetworkError, NetworkSignal)
{
  // BUG:Revoir ca en faisant le contraire(EmettreMC appelle Emettre)
  sendMC((NetworkMessage*) Buffer);
}


//-------------------------------------------------------------------
void 
SocketMC::receive(void *Buffer, unsigned long)
  throw(NetworkError, NetworkSignal)
{
  // BUG: Revoir ca en faisant le contraire(RecevoirMC appelle Recevoir)
  receiveMC((NetworkMessage*) Buffer);
}

//-------------------------------------------------------------------
int
SocketMC::returnSocket(void) const
{
  return _socket_mc;
}


//-------------------------------------------------------------------
unsigned long
SocketMC::returnAdress(void) const
{
  return _sin_e.sin_addr.s_addr;
}

//-------------------------------------------------------------------
void SocketMC::close()
{
  if(_est_init_mc)
  {
    ::close(_socket_mc);
    ::close(_socket_emetteur);
    _est_init_mc = RTI_FALSE;
  }
}


//-------------------------------------------------------------------
void SocketMC::sendMC(NetworkMessage *message)
{
  int cnt;

  message->NumeroMessage=_num_msg;
  _num_msg++;
  assert(_est_init_mc);
  assert(message != NULL);

  cnt = sendto(_socket_emetteur,(char*)message, TAILLE_MSG_RESEAU, 0,
(struct sockaddr *)&_sin_e, _sinlen_e); 


  if(cnt < 0)
  {  
    perror("Send"); 
    exit(-1); 
  }
}



//-------------------------------------------------------------------
char *SocketMC::receiveMC(NetworkMessage *message)
{
  int cnt;

  assert(_est_init_mc);

  cnt = recvfrom(_socket_mc,(char*)message, TAILLE_MSG_RESEAU, 0,
(struct sockaddr *)&_sin, &_sinlen);
  if(cnt < 0)
  { 
    perror("Recv");
    exit(1);
  }

  return(inet_ntoa(_sin.sin_addr));
}



//-------------------------------------------------------------------
int SocketMC::timeoutMC(int sec, int usec)
{
  fd_set fdset;
  struct timeval timeout;
  int i;
  assert(_est_init_mc);

  timeout.tv_sec = sec;
  timeout.tv_usec = usec;

  FD_ZERO(&fdset);
  FD_SET(_socket_mc, &fdset);
  while(((i=select(ulimit(4,0), 
		    SELECT_TYPE_ARG234 &fdset, 
		    NULL, NULL, 
		    &timeout)) < 0)
	 &&
	(errno == EINTR));
  return(i);
}

}

// EOF $Id: SocketMC.cc,v 3.3 2003/01/15 10:12:29 breholee Exp $

