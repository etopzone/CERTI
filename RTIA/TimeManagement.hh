// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: TimeManagement.hh,v 3.11 2007/10/25 08:07:16 rousse Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_RTIA_TIME_MANAGEMENT_HH
#define CERTI_RTIA_TIME_MANAGEMENT_HH

// Project
#include "certi.hh"
#include "Communications.hh"
#include "FederationManagement.hh"
#include "ObjectManagement.hh"
#include "OwnershipManagement.hh"
#include "Files.hh"
#include "LBTS.hh"
#include "Message.hh"
#include "Communications.hh"
#include "PrettyDebug.hh"

// Standard libraries
#ifdef _WIN32
#include <windows.h>
#else
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#endif

using std::cout ;
using std::endl ;

namespace certi {
namespace rtia {

class OwnershipManagement ;

typedef enum {
    PAS_D_AVANCEE=1,
    TAR, // TimeAdvanceRequest
    NER // NextEventRequest
} TypeAvancee ;

class TimeManagement : public LBTS
{
public:
    TimeManagement(Communications *,
                   Queues*,
                   FederationManagement *,
                   ObjectManagement *,
                   OwnershipManagement *);


    // Advance Time Methods
    void nextEventRequest(FederationTime heure_logique, TypeException &e);
    bool tick(TypeException &e);
    void timeAdvanceRequest(FederationTime heure_logique, TypeException &e);

    // Change Federate Time State
    void setLookahead(FederationTimeDelta lookahead, TypeException &e);
    void setTimeConstrained(bool etat, TypeException &e);
    void setTimeRegulating(bool etat, TypeException &e);
    void StopperAvanceTemps() {
        _avancee_en_cours = PAS_D_AVANCEE ;
    };

    // Request Attribute Methods
    FederationTime requestFederationTime();
    FederationTime requestFederateTime() { return(_heure_courante); };
    FederationTimeDelta requestLookahead();
    FederationTime requestLBTS() { return _LBTS ; };
    bool requestContraintState() { return _est_contraint ; };
    bool requestRegulateurState() { return _est_regulateur ; };

    // Attribute
    bool _ongoing_tick;  // tick2 processing
    bool _asynchronous_delivery ;

private:
    // Methods
    void advance(bool &msg_restant, TypeException &e);
    void timeAdvance(bool &msg_restant, TypeException &e);
    void nextEventAdvance(bool &msg_restant, TypeException &e);
    void timeAdvanceGrant(FederationTime, TypeException &);
    void flushQueueRequest(FederationTime, TypeException &);
    bool executeFederateService(NetworkMessage &msg);
    void sendNullMessage(FederationTime heure_logique);

    // Other RTIA Objects
    Communications *comm ;
    Queues *queues ;
    FederationManagement *fm ;
    ObjectManagement *om ;
    OwnershipManagement *owm ;

    /// Federate State
    FederationTime lastNullMessageDate ;

    /// Type/date from last request (timeAdvance, nextEvent, flushQueue)
    TypeAvancee _avancee_en_cours ;
    FederationTime date_avancee ;

    // Federate Data
    FederationTime _heure_courante ;
    FederationTimeDelta _lookahead_courant ;
    bool _est_regulateur ;
    bool _est_contraint ;
};

}} // namespace certi/rtia

#endif // CERTI_RTIA_TIME_MANAGEMENT_HH

// $Id: TimeManagement.hh,v 3.11 2007/10/25 08:07:16 rousse Exp $
