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
// $Id: Files.cc,v 3.1 2002/11/30 22:13:32 breholee Exp $
// ---------------------------------------------------------------------------

#include "Files.hh"

namespace certi {
namespace rtia {

// ---------------------------------------------------------------------------
Queues::Queues() { }
Queues::~Queues() { }

// ---------------------------------------------------------------------------
// nextTsoDate

// Retourner l'heure logique du premier message de la file TSO.

void 
Queues::nextTsoDate(Boolean &trouve, FederationTime &heure_logique)
{
  NetworkMessage *msg_tampon;

  if(_fileTSO.getLength() == 0) {
    trouve = RTI_FALSE;
    heure_logique = -1.0;
  }
  else {
    msg_tampon = _fileTSO.Ieme(1);
    trouve = RTI_TRUE;
    heure_logique = msg_tampon->Date;
  }
}

// ---------------------------------------------------------------------------
// giveCommandMessage

// Donner toutes les commandes au federe(en invoquant les services
// "RTI Initiated" du federe).

NetworkMessage* 
Queues::giveCommandMessage(Boolean &msg_donne, Boolean &msg_restant)
{
  NetworkMessage *msg;

  msg_donne = RTI_FALSE;
  msg_restant = RTI_FALSE;

  if(_file_commandes.getLength() > 0) {
    msg = _file_commandes.Ieme(1);
    
    // Supprimer de la file, mais garder le pointeur en memoire
    // quand meme pour l'execution de ExecuterServiceFedere
    _file_commandes.Supprimer(1);
    msg_donne = RTI_TRUE;
    
    if(_file_commandes.getLength() > 0)
      msg_restant = RTI_TRUE;
    
    return msg;
  }
  return NULL;
}

// ---------------------------------------------------------------------------
// giveFifoMessage

// Donner un message FIFO au federe.

NetworkMessage*
Queues::giveFifoMessage(Boolean &msg_donne, Boolean &msg_restant)
{
  NetworkMessage *msg_tampon;

  msg_donne = RTI_FALSE;
  msg_restant = RTI_FALSE;

  if(_fileFIFO.getLength() > 0) {
    msg_tampon = _fileFIFO.Ieme(1);
    
    // supprimer de la file, mais garder le pointeur en memoire
    // quand meme pour l'execution de ExecuterServiceFedere
    _fileFIFO.Supprimer(1);
    msg_donne = RTI_TRUE;
    
    if(_fileFIFO.getLength() > 0)
      msg_restant = RTI_TRUE;
    
    return(msg_tampon);
  }
  return NULL;
}

// ---------------------------------------------------------------------------
// giveTsoMessage

// 'heure_logique' est le minimum entre le LBTS courant et l'heure
// courante.

NetworkMessage *
Queues::giveTsoMessage(FederationTime heure_logique,
		       Boolean &msg_donne,
		       Boolean &msg_restant)
{
  NetworkMessage *tampon_msg = NULL;

  msg_donne = RTI_FALSE;
  msg_restant = RTI_FALSE;

  if(_fileTSO.getLength() != 0) {
    tampon_msg = _fileTSO.Ieme(1);
    if(tampon_msg->Date <= heure_logique) {
      // supprimer de la file, mais garder le pointeur en memoire
      // quand meme pour l'execution de ExecuterServiceFedere
      _fileTSO.Supprimer(1);
      msg_donne = RTI_TRUE;
      
      // regarder si le message TSO suivant peut etre donne
      if(_fileTSO.getLength() != 0) {
	NetworkMessage *tampon_msg2;
	tampon_msg2 = _fileTSO.Ieme(1);
	if(tampon_msg2->Date <= heure_logique)
	  msg_restant = RTI_TRUE;
      }
      return tampon_msg;
    }
    else {
      return NULL;
    }
  }
  else // lg == 0
    return NULL;
}

// ---------------------------------------------------------------------------
// insertBeginCommand

// Inserer un message contenant une commande(ex: requestPause)
// en debut de la file des commandes

void 
Queues::insertBeginCommand(NetworkMessage *msg)
{
  _file_commandes.Inserer(1, msg);
}

// ---------------------------------------------------------------------------
// insertLastCommand

// Inserer un message contenant une commande en fin de la file des
// commandes.

void 
Queues::insertLastCommand(NetworkMessage *msg)
{
  _file_commandes.Inserer(_file_commandes.getLength()+1, msg);
}

// ---------------------------------------------------------------------------
// insertFifoMessage

// Inserer un message en fin de file FIFO.

void 
Queues::insertFifoMessage(NetworkMessage *msg)
{
  _fileFIFO.Inserer(_fileFIFO.getLength()+1, msg);
}

// ---------------------------------------------------------------------------
// insertTsoMessage

// La file TSO est triee selon l'Horloge Logique des messages.

void 
Queues::insertTsoMessage(NetworkMessage *msg)
{
  NetworkMessage *msg_tampon = NULL;
  Boolean fin = RTI_FALSE;

  if(_fileTSO.getLength() == 0) _fileTSO.Inserer(1, msg);    
  else { // _fileTSO.getLength() >= 1
    for(int i=1; i<=_fileTSO.getLength() && !fin; i++) {
      msg_tampon = _fileTSO.Ieme(i);
      // strictement superieur pour placer le nouveau message
      // a la suite des anciens ayant la meme heure logique
      //(pour garder l'ordre de reception dans la file)
      if(msg_tampon->Date > msg->Date) {
	_fileTSO.Inserer(i, msg);
	fin = RTI_TRUE;
      }
    }
    if(fin == RTI_FALSE) {
      // inserer en fin de file
      _fileTSO.Inserer(_fileTSO.getLength()+1, msg);
    }
  }
}

}
}

// $Id: Files.cc,v 3.1 2002/11/30 22:13:32 breholee Exp $
