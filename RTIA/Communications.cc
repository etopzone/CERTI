// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI
//
// CERTI is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// $Id: Communications.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#include "Communications.hh"

namespace certi {
namespace rtia {

static pdCDebug D("RTIA_GCOM", "(RTIA GC ) - ");

// -----------------
// -- AttendreMsg --
// -----------------

void Communications::waitMessage(NetworkMessage *msg, 
				 TypeNetworkMessage type_msg, 
				 FederateHandle numeroFedere)
{
  NetworkMessage *tampon;
  Boolean test;

  assert(type_msg > 0 && type_msg < 100);

  D.Out(pdProtocol, "Waiting for Message of Type %d.", type_msg);

  // Est-ce que le message est deja arrive ? 
  // test = searchMessage(type_msg, numeroFedere, msg);
  // if(test) return;
 
  if(searchMessage(type_msg, numeroFedere, msg) == RTI_TRUE)
    return;

  // Sinon on l'attend tant que le type de message recu est different,
  // ou que le numero de federe ne correspond pas.

  tampon = new NetworkMessage;
  tampon->read((SecureTCPSocket *)this);

  D.Out(pdProtocol, "TCP Message of Type %d has arrived.", type_msg);

  while((tampon->Type != type_msg) || 
	((numeroFedere != 0) &&(tampon->NumeroFedere != numeroFedere))) {
    _fileAttente.Inserer(_fileAttente.lg+1, tampon);
    tampon = new NetworkMessage;
    tampon->read((SecureTCPSocket *) this);
    
    D.Out(pdProtocol, "Message of Type %d has arrived.", type_msg);    
  }
 
  // BUG: Devrait utiliser l'operateur de copie.
  memcpy((void *) msg,(void *) tampon, sizeof(NetworkMessage));
  delete tampon;
 
  assert(msg != NULL);
  assert(msg->Type == type_msg);
}

// ----------------------------
// -- Communications --
// ----------------------------

Communications::Communications() : SocketUN(), SecureTCPSocket(), SocketUDP()
{
  FILE *fp = NULL;
  char nom_serveur_RTIG[200]; 

  // creation communication Federe/RTIA
  acceptUN();

  // creation liaison TCP avec le RTIG
  char *certihost = getenv("CERTI_HOST");
  if(certihost==NULL) {
    fp = fopen("RTIA.dat", "r");
    if(fp == NULL) {
      printf("RTIA ERROR: Unable to find RTIG host.\n");
      printf("No RTIA.dat file found, no CERTI_HOST variable set\n");
      exit(-1);
    }
    
    fscanf(fp, "%s", nom_serveur_RTIG); // FIXME: buffer overflow
    fclose(fp);
    certihost = nom_serveur_RTIG ;
  }

  const char* tcp_port = getenv("CERTI_TCP_PORT");
  const char* udp_port = getenv("CERTI_UDP_PORT");
  if(tcp_port==NULL) tcp_port = PORT_TCP_RTIG ;
  if(udp_port==NULL) udp_port = PORT_UDP_RTIG ;

  createTCPClient(atoi(tcp_port), certihost);
  createUDPClient(atoi(udp_port), certihost);
}


// -----------------------------
// -- ~Communications --
// -----------------------------

Communications::~Communications()
{
  NetworkMessage msg;

  // Prevenir le RTIG de la fermeture de la liaison TCP
  msg.Type = m_CLOSE_CONNEXION;
  msg.write((SecureTCPSocket *) this);

  SecureTCPSocket::close();
}


// ---------------------------
// -- DemanderServiceFedere --
// ---------------------------

void 
Communications::requestFederateService(Message *req, 
				       Message *rep)
{
  Boolean erreur;
  assert(req != NULL);
  D.Out(pdRequest, "Sending Request to Federate, Type %d.", req->Type);
  sendUN(req);
  receiveUN(rep);
  D.Out(pdAnswer, "Received Answer from Federate.");
  assert(req->Type == rep->Type);
}

unsigned long 
Communications::getAddress()
{
  return((SocketUDP *) this)->getAddr();
}

unsigned int 
Communications::getPort()
{
  return((SocketUDP *) this)->getPort();
}

// -------------
// -- LireMsg --
// -------------

void 
Communications::readMessage(int &n, NetworkMessage *msg_reseau, Message *msg)
{
  fd_set fdset;
  NetworkMessage *msg2;
  int i;
 

  // initialiser fdset pour pouvoir utiliser un select
  FD_ZERO(&fdset);
  FD_SET(_socket_un, &fdset);
  FD_SET(SecureTCPSocket::returnSocket(), &fdset);
  FD_SET(SocketUDP::returnSocket(), &fdset);

#ifdef FEDERATION_USES_MULTICAST
  // si communication multicast initialisee(lors du join federation)
  if(_est_init_mc)
    FD_SET(_socket_mc, &fdset);
#endif

  if(_fileAttente.lg > 0) {
    // Il y a un message dans la file d'attente
    msg2 = _fileAttente.Ieme(1);
    _fileAttente.Supprimer(1);
    memcpy(msg_reseau, msg2, TAILLE_MSG_RESEAU);
    delete msg2;
    n = 1;

  }
  else if(SecureTCPSocket::isDataReady() == RTI_TRUE) {
    // Il y a des donnees en attente dans le buffer du socket TCP
    // Lire un message venant de la liaison TCP avec le RTIG
    msg_reseau->read((SecureTCPSocket *) this);
    n = 1;
  }
  else if(SocketUDP::isDataReady() == RTI_TRUE) {
    // Il y a des donnees en attente dans le buffer du socket UDP
    // Lire un message venant de la liaison UDP avec le RTIG
    msg_reseau->read((SocketUDP *) this);
    n = 1;
  }
  else if(SocketUN::isDataReady() == RTI_TRUE) {
    // Il y a des donnees en attente dans le buffer du socket UNIX
    // Lire un message venant de la liaison UNIX avec le federe
    msg->read((SocketUN *) this);
    n = 2;
  }
  else {
 
    // _fileAttente.lg == 0 et pas de data dans le buffer TCP
    // Attendre un message(venant du federe ou du reseau)
 
    if(select(ulimit(4,0), &fdset, NULL, NULL, NULL) < 0 ) {
      if(errno == EINTR)
	throw NetworkSignal();
      else
	throw NetworkError();
    }
 
    // Un message au moins est recu, lire ce message
 
#ifdef FEDERATION_USES_MULTICAST
    // Priorite aux messages venant du multicast(pour essayer d'eviter
    // un depassement de la file et donc la perte de messages)

    if(_est_init_mc && FD_ISSET(_socket_mc, &fdset)) {
      // Lire un message venant du multicast
      receiveMC(msg_reseau);
      n = 1;
    }
#endif

    if(FD_ISSET(SecureTCPSocket::returnSocket(), &fdset)) {
      // Lire un message venant de la liaison TCP avec le RTIG
      msg_reseau->read((SecureTCPSocket *) this);
      n = 1;
    }
    else {
      if(FD_ISSET(SocketUDP::returnSocket(), &fdset)) {
	// Lire un message venant de la liaison UDP avec le RTIG
	msg_reseau->read((SocketUDP *) this);
	n = 1;
      }
      else {
	// Lire un message venant du federe
	assert(FD_ISSET(_socket_un, &fdset));
	receiveUN(msg);
	n = 2;
      }   
    }
  }
}


// -------------------
// -- RechercherMsg --
// -------------------

Boolean 
Communications::searchMessage(TypeNetworkMessage type_msg,
			      FederateHandle numeroFedere,
			      NetworkMessage *msg)
{
  int i;
  NetworkMessage *Message;

  for(i = 1; i <= _fileAttente.lg; i++) {
    Message = _fileAttente.Ieme(i);
 
    D.Out(pdProtocol, "Rechercher message de type %d .", type_msg);
 
    if(numeroFedere == 0) {
      // Comparaison sans numero de federe
      if(Message->Type == type_msg) {
	memcpy(msg, Message, TAILLE_MSG_RESEAU);
	_fileAttente.Supprimer(i);
	delete Message;
	D.Out(pdProtocol, "Message of Type %d was already here.", 
	      type_msg);
	return RTI_TRUE;
      }
    }
    else {
      // Comparaison avec numero de federe
      if((Message->Type==type_msg) && (Message->NumeroFedere==numeroFedere)) {
	memcpy(msg, Message, TAILLE_MSG_RESEAU);
	_fileAttente.Supprimer(i);
	delete Message;
	D.Out(pdProtocol, "Message of Type %d was already here.", 
	      type_msg);
	return RTI_TRUE;
      }
    }
  }
  return RTI_FALSE;
}

}
}

// $Id: Communications.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
