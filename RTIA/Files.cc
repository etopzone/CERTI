// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
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
// $Id: Files.cc,v 3.3 2002/12/11 12:34:13 breholee Exp $
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
Queues::nextTsoDate(Boolean &found, FederationTime &time)
{
    NetworkMessage *msg_buffer;

    if(tsos.empty()) {
        found = RTI_FALSE;
        time = -1.0;
    }
    else {
        msg_buffer = tsos.front();
        found = RTI_TRUE;
        time = msg_buffer->Date;
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

    if (!commands.empty()) {
        msg = commands.front();

        // Supprimer de la file, mais garder le pointeur en memoire
        // quand meme pour l'execution de ExecuterServiceFedere
        commands.pop_front();
        msg_donne = RTI_TRUE;

        if (!commands.empty()) 
            msg_restant = RTI_TRUE;

        return msg ;
    }
    else return 0 ;
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
    
    if (!fifos.empty()) {
        msg_tampon = fifos.front();
        
        // supprimer de la file, mais garder le pointeur en memoire
        // quand meme pour l'execution de ExecuterServiceFedere
        fifos.pop_front();
        msg_donne = RTI_TRUE;
        
        if (!fifos.empty()) 
            msg_restant = RTI_TRUE ;

        return msg_tampon ;
    }
    else return 0 ;
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
    NetworkMessage *buffer_msg = NULL;
    
    msg_donne = RTI_FALSE;
    msg_restant = RTI_FALSE;
    
    if (!tsos.empty()) {
        buffer_msg = tsos.front();
        if (buffer_msg->Date <= heure_logique) {
            // supprimer de la file, mais garder le pointeur en memoire
            // quand meme pour l'execution de ExecuterServiceFedere
            tsos.pop_front();
            msg_donne = RTI_TRUE;

            // regarder si le message TSO suivant peut etre donne
            if (!tsos.empty()) {
                NetworkMessage *buffer_msg2;
                buffer_msg2 = tsos.front();

                if (buffer_msg2->Date <= heure_logique) 
                    msg_restant = RTI_TRUE ;
            }
            return buffer_msg ;
        }
        else return 0 ;
    }
    else return 0 ;
}

// ---------------------------------------------------------------------------
// insertBeginCommand

// Inserer un message contenant une commande(ex: requestPause)
// en debut de la file des commandes

void
Queues::insertBeginCommand(NetworkMessage *msg)
{
    commands.push_front(msg);
}

// ---------------------------------------------------------------------------
// insertLastCommand

// Inserer un message contenant une commande en fin de la file des
// commandes.

void
Queues::insertLastCommand(NetworkMessage *msg)
{
    commands.push_back(msg);
}

// ---------------------------------------------------------------------------
// insertFifoMessage

// Inserer un message en fin de file FIFO.

void
Queues::insertFifoMessage(NetworkMessage *msg)
{
    fifos.push_back(msg);
}

// ---------------------------------------------------------------------------
// insertTsoMessage

// La file TSO est triee selon l'Horloge Logique des messages.

void
Queues::insertTsoMessage(NetworkMessage *msg)
{
    if (tsos.empty()) {
        tsos.push_front(msg);
        return;
    }
    else {
        list<NetworkMessage *>::iterator i = tsos.begin();
        for (; i != tsos.end() ; i++) {
            // strictement superieur pour placer le nouveau message
            // a la suite des anciens ayant la meme heure logique
            //(pour garder l'ordre de reception dans la file)
            if( (*i)->Date > msg->Date ) {
                tsos.insert(i, msg);
                return;
            }
        }
    }

    // inserer en fin de file
    tsos.push_back(msg);
}

}}

// $Id: Files.cc,v 3.3 2002/12/11 12:34:13 breholee Exp $
