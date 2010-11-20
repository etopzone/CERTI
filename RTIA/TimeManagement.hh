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
// $Id: TimeManagement.hh,v 3.28 2010/11/20 16:52:44 erk Exp $
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

namespace certi {
namespace rtia {

class OwnershipManagement ;

typedef enum {
    PAS_D_AVANCEE=1, /*!< No time advancing request pending */
    TAR,             /*!< TimeAdvanceRequest pending        */
    NER,             /*!< NextEventRequest                  */
    TARA,            /*!< TimeAdvanceRequestAvailable       */
    NERA             /*!< NextEventRequestAvailable         */
} TypeAvancee ;

typedef enum {
    AFTER_TAR_OR_NER=1,
    AFTER_TAR_OR_NER_WITH_ZERO_LK,
    AFTER_TARA_OR_NERA
} TypeGrantedState ;

/**
 * The [distrbuted] time management class.
 * This class is instantiated in CERTI LRC (RTIA) and maintains
 * local view of LBTS and other time management related data and services.
 */
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

    /**
     * Try to deliver some message to the federate (command or FIFO)
     * and then try to advance time and deliver TSO messages.
     * The method will call either @ref advance which in turn
     * will call @ref nextEventAdvance or @ref timeAdvance to determine
     * which time to attain. It then calls tick() until a timeAdvanceGrant is
     * made.
     * @param[out] e exception which may have occurred during tick
     * @return true if there is more message for the federate to handle
     */
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
    bool requestContraintState() { return _is_constrained ; };
    bool requestRegulateurState() { return _is_regulating ; };

    /**
     * The different tick state values.
     * The @tick method is the method that will be called
     * inside LRC (RTIA) whenever the federate is calling
     * one the tick/evoke method in libRTI:
     *    - tick()
     *    - tick2()
     *    - tick(minTime,maxTime)
     */
    enum {
        NO_TICK,        /*!< No tick request received from federate */
        TICK_BLOCKING,  /*!< A 'blocking' tick request has been received */
        TICK_NEXT,      /*!< This is the continuation state of a previous
                             (blocking with timeout) tick request */
        TICK_CALLBACK,  /*!< Time management needs to treat a Callback */
        TICK_RETURN     /*!< Time management will terminate the tick call
                             because of an error in a callback or because this is the end
                             (timeout)
                         */
    } _tick_state;

    /**
     * This is true if the federate requested the LRC to
     * try to handle multiple callback in the allowed
     * amount of time tick(minTime,maxTime)
     */
    bool _tick_multiple;  // process multiple callbacks
    bool _tick_result;  // tick() return value

    TickTime _tick_timeout;
    TickTime _tick_max_tick;
    uint64_t _tick_clock_start;
    /**
     * Is asynchronous delivery enabled/disabled.
     */
    bool _asynchronous_delivery ;

private:

    /**
     * Main time advancing method.
     * This method is called by @ref tick().
     * Calls are dispatched between timeAdvance (TAR/TARA) and nextEventAdvance (NER/NERA)
     * depending on current time advancing method.
     * @param[out] msg_restant
     * @param[out] e exception will be updated to
     */
    void advance(bool &msg_restant, TypeException &e);

    /**
     * This method is called by @ref advance which is called by tick. This call is
     * done only if request type does correspond. It delivers TSO messages to
     * federate and if no messages are available, delivers a TimeAdvanceGrant.
     */
    void timeAdvance(bool &msg_restant, TypeException &e);

    /**
     * This method is called by @ref advance which is called by tick. This call
     * is done only if request type does correspond. It delivers TSO messages to
     * federate and if no messages are available, delivers a TimeAdvanceGrant.
     */
    void nextEventAdvance(bool &msg_restant, TypeException &e);

    /**
     * Once every messages has been delivered to federate, logical time can be
     * advanced and send a timeAdvanceGrant to federate.
     */
    void timeAdvanceGrant(FederationTime, TypeException& e);

    /**
     * Not implemented.
     */
    void flushQueueRequest(FederationTime, TypeException& e);

    /**
     * Deliver TSO messages to federate (UAV, ReceiveInteraction, etc...).
     * @param[in] msg the message to be handled.
     */
    bool executeFederateService(NetworkMessage &msg);
    /**
     * Send a null message to RTIG containing Logicaal Time + Lookahead.
     * This is the Null Message Algorithm from Chandy-Misra-Bryant
     * see e.g.
     *  http://www.cs.utexas.edu/users/misra/scannedPdf.dir/DistrSimulation.pdf
     *  @param[in] logicalTime the logical time of the NULL message to be sent
     */
    void sendNullMessage(FederationTime logicalTime);
    void sendNullPrimeMessage(FederationTime logicalTime);
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
    FederationTime lastNullPrimeMessageDate;

    /// Type/date from last request (timeAdvance, nextEvent, flushQueue)
    TypeAvancee _avancee_en_cours ;
    FederationTime date_avancee ;
    TypeGrantedState _type_granted_state ; 

    // Federate Data
    FederationTime      _heure_courante ;
    FederationTimeDelta _lookahead_courant ;
    bool _is_regulating;
    bool _is_constrained;

};

}} // namespace certi/rtia

#endif // CERTI_RTIA_TIME_MANAGEMENT_HH

// $Id: TimeManagement.hh,v 3.28 2010/11/20 16:52:44 erk Exp $
