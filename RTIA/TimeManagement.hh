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
// $Id: TimeManagement.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_RTIA_TIME_MANAGEMENT_HH
#define _CERTI_RTIA_TIME_MANAGEMENT_HH

#include <config.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#include "baseTypes.hh"
#include "RTItypes.hh"
#include "Communications.hh"
#include "FederationManagement.hh"
#include "ObjectManagement.hh"
#include "OwnershipManagement.hh"
#include "Files.hh"
#include "LBTS.hh"
#include "Message.hh"
#include "Communications.hh"

#include "PrettyDebug.hh"

namespace certi {
namespace rtia {

class OwnershipManagement ;

typedef enum {
  PAS_D_AVANCEE=1,
  TAR, // TimeAdvanceRequest
  NER // NextEventRequest
} TypeAvancee;

//class Communications;;

class TimeManagement : public LBTS
{
public:
 
  // -----------------
  // -- Constructor --
  // -----------------

  TimeManagement(Communications *,
		 Queues*,
		 FederationManagement *,
		 ObjectManagement *,
		 OwnershipManagement *);

  // --------------------------
  // -- Advance Time Methods --
  // --------------------------

  // Le federe appelle soit nextEventR. soit timeAdvanceR. pour fixer un
  // objectif de date a atteindre, puis appelle tick jusqu'a ce qu'il
  // recoive un timeAdvanceGrant.

  void nextEventRequest(FederationTime heure_logique,
			TypeException &e);

  Boolean tick(TypeException &e);

  void timeAdvanceRequest(FederationTime heure_logique,
			  TypeException &e);

  // --------------------------------
  // -- Change Federate Time State --
  // --------------------------------

  void setLookahead(FederationTimeDelta lookahead, TypeException &e);
  void setTimeConstrained(Boolean etat, TypeException &e);
  void setTimeRegulating(Boolean etat, TypeException &e);
  void StopperAvanceTemps() { 
    _avancee_en_cours = PAS_D_AVANCEE; 
  };

  // -------------------------------
  // -- Request Attribute Methods --
  // -------------------------------

  FederationTime requestFederationTime();
  FederationTime requestFederateTime() { return(_heure_courante); };
  FederationTimeDelta requestLookahead();
  FederationTime requestLBTS() { return _LBTS; };
  Boolean requestContraintState() { return _est_contraint; };
  Boolean requestRegulateurState() { return _est_regulateur; };

private:

  // ------------------------
  // -- Private Attributes --
  // ------------------------

  // Other RTIA Objects

  Communications *_GC;
  Queues *_GQueues;
  FederationManagement *_GF;
  ObjectManagement *_GO;
  OwnershipManagement *_GP;

  // Federate State

  FederationTime lastNullMessageDate;

  // Type/date de la derniere requete(timeAdvance, nextEvent, flushQueue)
  TypeAvancee _avancee_en_cours;
  FederationTime date_avancee;

  // Federate Data
  FederationTime _heure_courante;
  FederationTimeDelta _lookahead_courant;
  Boolean _est_regulateur;
  Boolean _est_contraint;

  // ---------------------
  // -- Private Methods --
  // ---------------------

  // Dispatche les appels entre TimeAdvanceEnCours et NextEventEnCours.
  // Appelee par tick.

  void advance(Boolean &msg_restant, TypeException &e);

  // Les deux methodes suivantes sont appelees par tick suivant le
  // type de request qui a ete fait avant. Elles delivrent les
  // messages TSO au federe, ou s'il n'y a plus de messages,
  // accorde un TimeAdvanceGrant.

  void timeAdvance(Boolean &msg_restant, TypeException &e);
  void nextEventAdvance(Boolean &msg_restant, TypeException &e);

  // Une fois que tous les messages ont ete donnes au federe, on peut
  // avancer le temps et donner un TimeAdvanceGrant au federe.

  void timeAdvanceGrant(FederationTime, TypeException &);

  // Pas implemente.
  void flushQueueRequest(FederationTime, TypeException &);

  // Delivre les messages TSO au federe(UAV, ReceiveInteraction, etc...)
  Boolean executeFederateService(NetworkMessage &msg);

  // Emet un message nul vers le RTIG contenant Heure Locale + Lookahead.
  void sendNullMessage(FederationTime heure_logique);
};
}
}

#endif // _CERTI_RTIA_TIME_MANAGEMENT_HH

// $Id: TimeManagement.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
