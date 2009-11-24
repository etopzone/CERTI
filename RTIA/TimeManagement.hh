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
// $Id: TimeManagement.hh,v 3.22 2009/11/24 21:44:48 erk Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_RTIA_TIME_MANAGEMENT_HH
#define CERTI_RTIA_TIME_MANAGEMENT_HH

// Project
#include "certi.hh"
#include "Communications.hh"
#include "FederationManagement.hh"
#include "DeclarationManagement.hh"
#include "ObjectManagement.hh"
#include "OwnershipManagement.hh"
#include "Files.hh"
#include "LBTS.hh"
#include "Message.hh"
#include "Communications.hh"
#include "PrettyDebug.hh"

// Standard libraries
#include <iostream>

using std::cout ;
using std::endl ;

namespace certi {
namespace rtia {

class OwnershipManagement ;

typedef enum {
    PAS_D_AVANCEE=1,
    TAR, // TimeAdvanceRequest
    NER, // NextEventRequest
    TARA, // TimeAdvanceRequestAvailable
    NERA // NextEventRequestAvailable
} TypeAvancee ;

typedef enum {
    AFTER_TAR_OR_NER=1,
    AFTER_TAR_OR_NER_WITH_ZERO_LK,
    AFTER_TARA_OR_NERA
} TypeGrantedState ;

class TimeManagement : public LBTS
{
public:
    TimeManagement(Communications *,
                   Queues*,
                   FederationManagement *,
		   DeclarationManagement *,
                   ObjectManagement *,
                   OwnershipManagement *);


    // Advance Time Methods
    void nextEventRequest(FederationTime heure_logique, TypeException &e);
    void nextEventRequestAvailable(FederationTime heure_logique, TypeException &e);
    bool tick(TypeException &e);
    void timeAdvanceRequest(FederationTime heure_logique, TypeException &e);
    void timeAdvanceRequestAvailable(FederationTime heure_logique, TypeException &e);
    bool testValidTime(FederationTime theTime);

    // Change Federate Time State
    void setLookahead(FederationTimeDelta lookahead, TypeException &e);
    void setTimeConstrained(bool etat, TypeException &e);
    void setTimeRegulating(bool etat,FederationTime heure_logique,
                           FederationTimeDelta lookahead, TypeException &e);
    void StopperAvanceTemps() {
        _avancee_en_cours = PAS_D_AVANCEE ;
    };

    // Request Attribute Methods
    FederationTime requestFederationTime();
    FederationTime requestFederateTime() { return(_heure_courante); };
    FederationTimeDelta requestLookahead();
    FederationTime requestMinNextEventTime();
    FederationTime requestLBTS() { return _LBTS ; };
    bool requestContraintState() { return _est_contraint ; };
    bool requestRegulateurState() { return _est_regulateur ; };

    // Attribute
    enum {
        NO_TICK,
        TICK_BLOCKING,
        TICK_NEXT,
        TICK_CALLBACK,
        TICK_RETURN
    } _tick_state;
    bool _tick_multiple;  // process multiple callbacks
    bool _tick_result;  // tick() return value
    RTI::TickTime _tick_timeout;
    RTI::TickTime _tick_max_tick;
    uint64_t _tick_clock_start;
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
    void timeRegulationEnabled(FederationTime, TypeException &e);
    void timeConstrainedEnabled(FederationTime, TypeException &e);

    // Other RTIA Objects
    Communications *comm ;
    Queues *queues ;
    FederationManagement *fm ;
    DeclarationManagement *dm ;
    ObjectManagement *om ;
    OwnershipManagement *owm ;

    /// Federate State
    FederationTime lastNullMessageDate ;

    /// Type/date from last request (timeAdvance, nextEvent, flushQueue)
    TypeAvancee _avancee_en_cours ;
    FederationTime date_avancee ;
    TypeGrantedState _type_granted_state ; 

    // Federate Data
    FederationTime _heure_courante ;
    FederationTimeDelta _lookahead_courant ;
    bool _est_regulateur ;
    bool _est_contraint ;

};

}} // namespace certi/rtia

#endif // CERTI_RTIA_TIME_MANAGEMENT_HH

// $Id: TimeManagement.hh,v 3.22 2009/11/24 21:44:48 erk Exp $
