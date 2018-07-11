// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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
// ----------------------------------------------------------------------------

#include "TimeManagement.hh"
#include <config.h>

#include <float.h>

#include <libCERTI/M_Classes.hh>
#include <libCERTI/NM_Classes.hh>

namespace certi {
namespace rtia {

namespace {

PrettyDebug D("RTIA_TM", "[RTIA TM] ");
static PrettyDebug G("GENDOC", __FILE__);
static PrettyDebug DNULL("RTIA_NULLMSG", "[RTIA NULL MSG] ");
static PrettyDebug DTUS("RTIA_TIME_UP", "[RTIA NULL MSG] ");

static constexpr double epsilon2 = 1.0e-4;
}

TimeManagement::TimeManagement(Communications* GC,
                               Queues* GQueues,
                               FederationManagement* GF,
                               DeclarationManagement* GD,
                               ObjectManagement* GO,
                               OwnershipManagement* GP)
    : LBTS()
    , _tick_state(NO_TICK)
    , comm(GC)
    , queues(GQueues)
    , fm(GF)
    , dm(GD)
    , om(GO)
    , owm(GP)
{
    sendTimeStateUpdate();
}

void TimeManagement::timeAdvanceRequest(FederationTime logical_time, Exception::Type& e)
{
    e = Exception::Type::NO_EXCEPTION;

    // Verifications

    if (_avancee_en_cours != PAS_D_AVANCEE) {
        e = Exception::Type::TimeAdvanceAlreadyInProgress;
    }

    if (logical_time < _heure_courante) {
        e = Exception::Type::FederationTimeAlreadyPassed;
    }

    //    This is check may be overkill because
    //    if we consider that advancing in time is NOT a timestamped event
    //    see bug #25497 : https://savannah.nongnu.org/bugs/?25497
    //    if (logical_time < _heure_courante + _lookahead_courant) {
    //
    //    D.Out(pdDebug,"InvalidFederation time lkahead=%f, current=%f, requested=%f",
    //    			_lookahead_courant.getTime(),_heure_courante.getTime(),logical_time.getTime());
    //       e = Exception::Type::InvalidFederationTime ;
    //    }

    if (e != Exception::Type::NO_EXCEPTION) {
        Debug(D, pdExcept) << "timeAdvanceRequest refused, exception = " << static_cast<int>(e) << std::endl;
        return;
    }
    
    _type_granted_state = AFTER_TAR_OR_NER; // will be

    if (_lookahead_courant == 0.0) {
        _lookahead_courant = epsilon2;
        _type_granted_state = AFTER_TAR_OR_NER_WITH_ZERO_LK;
    }

    if (_is_regulating) {
        sendNullMessage(logical_time);
    }

    _avancee_en_cours = TAR;
    date_avancee = logical_time;
    
    sendTimeStateUpdate();

    Debug(D, pdTrace) << "timeAdvanceRequest accepted, asked time=" << date_avancee.getTime() << std::endl;
}

void TimeManagement::timeAdvanceRequestAvailable(FederationTime logical_time, Exception::Type& e)
{
    e = Exception::Type::NO_EXCEPTION;

    // Verifications

    if (_avancee_en_cours != PAS_D_AVANCEE) {
        e = Exception::Type::TimeAdvanceAlreadyInProgress;
    }

    if (logical_time < _heure_courante) {
        e = Exception::Type::FederationTimeAlreadyPassed;
    }

    //    This is check may be overkill because
    //    if we consider that advancing in time is NOT a timestamped event
    //    see bug #25497 : https://savannah.nongnu.org/bugs/?25497
    //    if (logical_time < _heure_courante + _lookahead_courant)
    //       e = Exception::Type::InvalidFederationTime ;

    if (e != Exception::Type::NO_EXCEPTION) {
        Debug(D, pdExcept) << "timeAdvanceRequestAvailable refused, exception = " << static_cast<int>(e) << std::endl;
        return;
    }
    
    _type_granted_state = AFTER_TARA_OR_NERA; // will be

    if (_is_regulating) {
        sendNullMessage(logical_time);
    }

    _avancee_en_cours = TARA;
    date_avancee = logical_time;
    
    sendTimeStateUpdate();

    Debug(D, pdTrace) << "timeAdvanceRequestAvailable accepted, asked time=" << date_avancee.getTime() << std::endl;
}

void TimeManagement::nextEventRequest(FederationTime logical_time, Exception::Type& e)
{
    e = Exception::Type::NO_EXCEPTION;

    // Verifications

    if (_avancee_en_cours != PAS_D_AVANCEE) {
        e = Exception::Type::TimeAdvanceAlreadyInProgress;
    }

    if (logical_time < _heure_courante) {
        e = Exception::Type::FederationTimeAlreadyPassed;
    }

    //    This is check may be overkill because
    //    if we consider that advancing in time is NOT a timestamped event
    //    see bug #25497 : https://savannah.nongnu.org/bugs/?25497
    //    if (heure_logique < _heure_courante + _lookahead_courant)
    //       e = Exception::Type::InvalidFederationTime ;

    if (e != Exception::Type::NO_EXCEPTION) {
        Debug(D, pdExcept) << "NextEventRequest refused, exception = " << static_cast<int>(e) << std::endl;
        return;
    }
    
    _type_granted_state = AFTER_TAR_OR_NER; // will be

    if (_lookahead_courant == 0.0) {
        _lookahead_courant = epsilon2;
        _type_granted_state = AFTER_TAR_OR_NER_WITH_ZERO_LK;
        Debug(D, pdDebug) << "NER: with ZERO LK, lk=" << _lookahead_courant.getTime() << std::endl;
    }

    _avancee_en_cours = NER;
    date_avancee = logical_time;
    _nerTimeRequested = logical_time;
    sendNullPrimeMessage(logical_time);
    Debug(D, pdTrace) << "NextEventRequest accepted, lk=" << _lookahead_courant.getTime()
                        << ", date_avance=" << date_avancee.getTime() << std::endl;

    sendTimeStateUpdate();
}

void TimeManagement::nextEventRequestAvailable(FederationTime heure_logique, Exception::Type& e)
{
    e = Exception::Type::NO_EXCEPTION;

    // Verifications

    if (_avancee_en_cours != PAS_D_AVANCEE) {
        e = Exception::Type::TimeAdvanceAlreadyInProgress;
    }

    if (heure_logique < _heure_courante) {
        e = Exception::Type::FederationTimeAlreadyPassed;
    }

    //    This is check may be overkill because
    //    if we consider that advancing in time is NOT a timestamped event
    //    see bug #25497 : https://savannah.nongnu.org/bugs/?25497
    //    if (heure_logique < _heure_courante + _lookahead_courant)
    //       e = Exception::Type::InvalidFederationTime ;

    if (e == Exception::Type::NO_EXCEPTION) {
        Debug(D, pdExcept) << "NextEventRequestAvailable refused, exception = " << static_cast<int>(e) << std::endl;
        return;
    }
    
    _type_granted_state = AFTER_TARA_OR_NERA; // will be
    _avancee_en_cours = NERA;
    date_avancee = heure_logique;
    sendNullPrimeMessage(heure_logique);
    Debug(D, pdTrace) << "NextEventRequestAvailable accepted." << std::endl;

    sendTimeStateUpdate();
}

bool TimeManagement::tick(Exception::Type& e)
{
    // Becomes true if there is message to give back to the federate
    bool oneMsgToHandle = false;
    // When oneMsgToHandle==true, then msg_restant becomes true if there is more message to handle
    bool moreMsgToHandle = false;

    Debug(G, pdGendoc) << " enter TimeManagement::tick" << std::endl;
    // Note: While msg_donne = false, msg_restant doesn't matter.

    // 1st try, give a command message. (requestPause, etc.)
    NetworkMessage* msg = queues->giveCommandMessage(oneMsgToHandle, moreMsgToHandle);

    // 2nd try, give a FIFO message. (discoverObject, etc.)
    if (!oneMsgToHandle) {
        if (_asynchronous_delivery || (_avancee_en_cours != PAS_D_AVANCEE) || (!_is_constrained)) {
            Debug(D, pdDebug) << "FIFO message to be delivered async_deliver=" << _asynchronous_delivery
                              << ", _avancee=" << _avancee_en_cours << ", constrained=" << _is_constrained << std::endl;
            // to exclude the case not asynchronous_delivery and
            // not time advancing for a constrained federate
            msg = queues->giveFifoMessage(oneMsgToHandle, moreMsgToHandle);
        }
        else {
            Debug(D, pdDebug) << "FIFO message skipped async_deliver=" << _asynchronous_delivery
                              << ", _avancee=" << _avancee_en_cours << ", constrained=" << _is_constrained << std::endl;
        }
    }

    // If message exists, send it to the federate.
    if (oneMsgToHandle) {
        Debug(D, pdDebug) << "TickRequest being processed, Message to send." << std::endl;
        try {
            executeFederateService(*msg);
        }
        catch (RTIinternalError& e) {
            Debug(D, pdError) << "RTIA:RTIinternalError thrown in tick (execute)." << std::endl;
            throw;
        }
    }
    // No message: we try to send TSO messages.
    // Messages to be sent depends on asked advance type.
    else {
        Debug(D, pdDebug) << "TickRequest being processed, advance called." << std::endl;
        try {
            advance(moreMsgToHandle, e);
        }
        catch (RTIinternalError& e) {
            Debug(D, pdError) << "RTIA:RTIinternalError thrown in tick (Advance)." << std::endl;
            throw;
        }
    }

    delete msg;
    
    sendTimeStateUpdate();

    Debug(G, pdGendoc) << " exit  TimeManagement::tick" << std::endl;
    return moreMsgToHandle;
}

bool TimeManagement::testValidTime(FederationTime logical_time)
{
    if (_avancee_en_cours == PAS_D_AVANCEE) {
        if (_type_granted_state == AFTER_TAR_OR_NER_WITH_ZERO_LK) {
            if (logical_time <= _heure_courante) {
                return false;
            }
        }
        else { // AFTER_TAR_OR_NER or AFTER_TARA_OR_NARA
            if (logical_time < _heure_courante + _lookahead_courant) {
                return false;
            }
        }
    }
    else {
        if (_type_granted_state == AFTER_TAR_OR_NER_WITH_ZERO_LK) {
            if (logical_time <= date_avancee) {
                return false;
            }
        }
        else { // AFTER_TAR_OR_NER or AFTER_TARA_OR_NARA
            if (logical_time < date_avancee + _lookahead_courant) {
                return false;
            }
        }
    }
    return true;
}

void TimeManagement::setLookahead(FederationTimeDelta lookahead, Exception::Type& e)
{
    e = Exception::Type::NO_EXCEPTION;

    // Verifications

    if (lookahead < 0.0) {
        e = Exception::Type::InvalidLookahead;
    }

    if (lookahead == epsilon2) {
        Debug(D, pdError) << "Bad value of lookahead due to a zero lookahead implementation trick" << std::endl;
        e = Exception::Type::RTIinternalError;
    }

    if (e != Exception::Type::NO_EXCEPTION) {
        Debug(D, pdExcept) << "setLookahead refused, exception = " << static_cast<int>(e) << std::endl;
        return;
    }
    
    _lookahead_courant = lookahead;

    // On previent les autres en leur envoyant un message NULL qui contient
    // notre temps local + le Lookahead.
    if (_is_regulating) {
        sendNullMessage(_heure_courante);
    }
    
    sendTimeStateUpdate();

    Debug(D, pdRegister) << "New Lookahead : " << _lookahead_courant.getTime() << std::endl;
}

void TimeManagement::setTimeConstrained(bool etat, Exception::Type& e)
{
    NM_Set_Time_Constrained msg;

    e = Exception::Type::NO_EXCEPTION;

    // Verifications

    if (_is_constrained == etat) {
        e = Exception::Type::RTIinternalError;
    }

    if (_avancee_en_cours != PAS_D_AVANCEE) {
        e = Exception::Type::RTIinternalError;
    }

    if (e != Exception::Type::NO_EXCEPTION) {
        Debug(D, pdExcept) << "SetTimeConstrained refuse, exception = " << static_cast<int>(e) << std::endl;
        return;
    }
    
    _is_constrained = etat;

    msg.setFederation(fm->getFederationHandle().get());
    msg.setFederate(fm->getFederateHandle());
    if (etat) {
        msg.constrainedOn();
    }
    else {
        msg.constrainedOff();
    }

    comm->sendMessage(&msg);

    Debug(D, pdRegister) << "Demande de modif de TimeConstrained envoyee, etat=" << etat << std::endl;
}

// Rajouter le time et le lookahead
// Modifier le lookahead courant
void TimeManagement::setTimeRegulating(bool etat,
                                       FederationTime /*logical_time*/,
                                       FederationTimeDelta the_lookahead,
                                       Exception::Type& e)
{
    NM_Set_Time_Regulating msg;

    e = Exception::Type::NO_EXCEPTION;

    // Verifications

    if (_is_regulating == etat) {
        e = Exception::Type::RTIinternalError;
        Debug(D, pdRegister) << "erreur e_RTIinternalError : federe deja regulateur" << std::endl;
    }

    if (_avancee_en_cours != PAS_D_AVANCEE) {
        e = Exception::Type::RTIinternalError;
        Debug(D, pdRegister) << "erreur e_RTIinternalError avancee_en_cours" << std::endl;
    }

    // Si aucune erreur, prevenir le RTIG et devenir regulateur.
    if (e != Exception::Type::NO_EXCEPTION) {
        Debug(D, pdExcept) << "SetTimeRegulating refuse, exception = " << static_cast<int>(e) << std::endl;
        return;
    }
    
    _is_regulating = etat;

    msg.setFederation(fm->getFederationHandle().get());
    msg.setFederate(fm->getFederateHandle());
    if (etat) {
        msg.regulatorOn();
        Debug(D, pdDebug) << "REGULATOR ON" << std::endl;
    }
    else {
        msg.regulatorOff();
        Debug(D, pdDebug) << "REGULATOR OFF" << std::endl;
    }
    // Modifier lookahead courant
    _lookahead_courant = the_lookahead;
    Debug(D, pdDebug) << "New lookahead = " << _lookahead_courant.getTime() << std::endl;
    // faudrait peut etre remplacer heure courante par le temps en parametre
    msg.setDate(_heure_courante + _lookahead_courant);

    comm->sendMessage(&msg);
    
    sendTimeStateUpdate();

    Debug(D, pdRegister) << "Demande de modif de TimeRegulating emise, etat=" << etat << std::endl;
}

void TimeManagement::StopperAvanceTemps()
{
    _avancee_en_cours = PAS_D_AVANCEE;
    
    sendTimeStateUpdate();
};

FederationTime TimeManagement::requestFederationTime()
{
    return _LBTS;
}

FederationTime TimeManagement::requestFederateTime()
{
    return (_heure_courante);
}

FederationTimeDelta TimeManagement::requestLookahead()
{
    return _lookahead_courant;
}

FederationTime TimeManagement::requestMinNextEventTime()
{
    FederationTime dateTSO;
    FederationTime dateMNET;
    bool found;

    queues->nextTsoDate(found, dateTSO);

    if (!found) {
        dateMNET = _LBTS;
    }
    else {
        dateMNET = (_LBTS <= dateTSO ? _LBTS : dateTSO);
    }

    Debug(D, pdRegister) << "Minimum Next Event Time : " << dateMNET.getTime() << std::endl;

    return dateMNET;
}

void TimeManagement::updateMinTxMessageDate(FederationTime TxMessageDate)
{ 
    if (TxMessageDate > _heure_courante){
		_MyEventTimestampList.push_back(TxMessageDate.getTime());
		std::sort(_MyEventTimestampList.begin(), _MyEventTimestampList.end(), std::less<double>());
	}
}
    
FederationTime TimeManagement::requestLBTS()
{
    return _LBTS;
}

bool TimeManagement::requestContraintState()
{
    return _is_constrained;
}

bool TimeManagement::requestRegulateurState()
{
    return _is_regulating;
}

void TimeManagement::setMomUpdateRate(const std::chrono::seconds updateRate)
{
    my_updateRate = updateRate;
}

void TimeManagement::advance(bool& msg_restant, Exception::Type& e)
{
    switch (_avancee_en_cours) {
    case TAR:
    case TARA:
        Debug(D, pdTrace) << "Call to TimeAdvance." << std::endl;
        timeAdvance(msg_restant, e);
        break;
    case NER:
    case NERA:
        Debug(D, pdTrace) << "Call to NextEventAdvance." << std::endl;
        nextEventAdvance(msg_restant, e);
        break;
    default:
        Debug(D, pdTrace) << "Unexpected case in advance: " << _avancee_en_cours << std::endl;
        // No exception is raised, ca peut etre un cas ou on a rien a faire,
        // par exemple en cas d'attente active pendant une pause.
    }
}

void TimeManagement::timeAdvance(bool& msg_restant, Exception::Type& e)
{
    bool msg_donne;
    FederationTime min;
    NetworkMessage* msg;
    Debug(G, pdGendoc) << " enter TimeManagement::timeAdvance" << std::endl;
    msg_restant = false;

    if (_is_constrained) {
        // give a TSO message.
        if (_LBTS.isPositiveInfinity()) {
            Debug(D, pdDebug) << "Logical time : " << date_avancee.getTime() << ", LBTS : infini." << std::endl;
        }
        else {
            Debug(D, pdDebug) << "Logical time : " << date_avancee.getTime() << ", LBTS : " << _LBTS.getTime()
                              << std::endl;
        }
        min = (_LBTS < date_avancee) ? (_LBTS) : (date_avancee);
        msg = queues->giveTsoMessage(min, msg_donne, msg_restant);

        // otherwise
        if (!msg_donne) {
            // if LBTS allows to give a timeAdvanceGrant.
            if (_LBTS.isPositiveInfinity()) {
                Debug(D, pdDebug) << "Logical time : " << date_avancee.getTime()
                                  << ", LBTS : infini, lookahead : " << _lookahead_courant.getTime() << std::endl;
            }
            else {
                Debug(D, pdDebug) << "Logical time : " << date_avancee.getTime() << ", LBTS : " << _LBTS.getTime()
                                  << ", lookahead : " << _lookahead_courant.getTime() << std::endl;
            }
#ifdef CERTI_USE_NULL_PRIME_MESSAGE_PROTOCOL
		// Using NMP protocol the NER advance must be agreed when date min = _LBTS from Anonymous update
        //if ((date_min < _LBTS) || ((date_min <= _LBTS) && (_avancee_en_cours == NER))) {
			if (date_avancee <= _LBTS) {
#else
			if ((date_avancee < _LBTS) || ((date_avancee <= _LBTS) && (_avancee_en_cours == TARA))) {
#endif
                // send a timeAdvanceGrant to federate.
                timeAdvanceGrant(date_avancee, e);

                if (e != Exception::Type::NO_EXCEPTION) {
                    return;
                }

                _avancee_en_cours = PAS_D_AVANCEE;
            }
            // otherwise nothing has to be sent to federate (empty tick).
        }
        else {
            executeFederateService(*msg);
            delete msg;
        }
    }
    else {
        // if federate is not constrained, send a timeAdvanceGrant to federate.
        timeAdvanceGrant(date_avancee, e);
        if (e != Exception::Type::NO_EXCEPTION)
            return;
        _avancee_en_cours = PAS_D_AVANCEE;
    }
        
    sendTimeStateUpdate();
    
    Debug(G, pdGendoc) << " exit  TimeManagement::timeAdvance" << std::endl;
}

void TimeManagement::nextEventAdvance(bool& msg_restant, Exception::Type& e)
{
    FederationTime dateTSO;
    FederationTime date_min = 0.0;
    bool TSOPresent;
    bool msg_donne;
    NetworkMessage* msg;

    Debug(G, pdGendoc) << " enter TimeManagement::nextEventAdvance" << std::endl;
    msg_restant = false;

    if (_is_constrained) {
        // Select lower value between expected time and first TSO message time.
        queues->nextTsoDate(TSOPresent, dateTSO);

        if ((TSOPresent) && (dateTSO < date_avancee)) {
            date_min = dateTSO;
        }
        else {
            date_min = date_avancee;
        }

        Debug(D, pdDebug) << "TM::nextEventAdvance - date avancee=" << date_avancee.getTime()
                          << " date min=" << date_min.getTime() << " LBTS = " << _LBTS.getTime() << std::endl;

#ifdef CERTI_USE_NULL_PRIME_MESSAGE_PROTOCOL
		// Using NMP protocol the NER advance must be agreed when date min = _LBTS from Anonymous update
        //if ((date_min < _LBTS) || ((date_min <= _LBTS) && (_avancee_en_cours == NER))) {
        if (date_min <= _LBTS) {
#else
         if ((date_min < _LBTS) || ((date_min <= _LBTS) && (_avancee_en_cours == NERA))) {
#endif
            // nextEventRequest is done because either a TSO message
            // can be delivered or no message with lower value than
            // expected time is avail.

            // New expected time is keep (can be first TSO message to deliver).
            // This value must not be changed.
            date_avancee = date_min;

            // If federate is regulating, inform other federate we advanced.
            if (_is_regulating) {
				#ifdef CERTI_USE_NULL_PRIME_MESSAGE_PROTOCOL
				//sendNullPrimeMessage(date_min);
				#else
                sendNullMessage(date_min);
                #endif
            }

            // Deliver to federate every TSO messages with time
            // 'date_min' (1 by 1).
            msg = queues->giveTsoMessage(date_min, msg_donne, msg_restant);
            if (msg_donne) {
                Debug(D, pdDebug) << "TM::nextEventAdvance - MSG :" << msg->getMessageName() << std::endl;
                // Send message back to federate.
                executeFederateService(*msg);
                delete msg;
            }
            else {
                // Advance current time up to 'date_min'.
                Debug(D, pdDebug) << "TM::nextEventAdvance - TAG to" << date_min.getTime() << std::endl;
                timeAdvanceGrant(date_min, e);
                _avancee_en_cours = PAS_D_AVANCEE;
            }
        }
        else { // date_min < _LBTS
            // Federate can't advance up to expected time but up to LBTS. Other
            // federates are informed and no TSO message are sent.
            if (_is_regulating) {
				#ifdef CERTI_USE_NULL_PRIME_MESSAGE_PROTOCOL
                /**
                 * The following NULL PRIME message is part of SN NULL MESSAGE PRIME algorithm
                 * This message is sent because we did receive some anonymous NULL message
                 * in the past and we are still NERing so RTIG may be interested (again)
                 * in our wanted advance time.
                 */
                if (hasReceivedAnonymousUpdate()){
					std::vector<double> MyEventTimestampTmp;
					for (std::vector<double>::iterator it = _MyEventTimestampList.begin() ; it != _MyEventTimestampList.end(); ++it){
						if (*it > getLastAnonymousUpdate().getTime() && (fabs(*it - getLastAnonymousUpdate().getTime()) > epsilon2) ){
							MyEventTimestampTmp.push_back(*it);
						}
					}
					FederationTime EchoBackNMP;
					_MyEventTimestampList = MyEventTimestampTmp;
					EchoBackNMP = sendNullPrimeMessage(_nerTimeRequested);
					Debug(D, pdDebug) << "TM::Anonymous NMP received from RTIG with ts = " << getLastAnonymousUpdate().getTime() << "echo back with ts = " << EchoBackNMP.getTime() << std::endl;
					resetAnonymousUpdate();
                }
                #else
                /* The following NULL message is part of the classical CMB NULL MESSAGE algorithm */
                sendNullMessage(_LBTS);
                #endif
            }
        }
    }

    else { // if federate isn't constrained.

        // In this case, federate can advance freely. Moreover, there must be no
        // message in TSO list.
        if (_is_regulating) {
            sendNullMessage(date_avancee);
        }

        timeAdvanceGrant(date_avancee, e);

        _avancee_en_cours = PAS_D_AVANCEE;
    }
    
    sendTimeStateUpdate();
    
    Debug(G, pdGendoc) << " exit  TimeManagement::nextEventAdvance" << std::endl;
}

void TimeManagement::timeAdvanceGrant(FederationTime logical_time, Exception::Type& /*e*/)
{
    M_Time_Advance_Grant req;

    req.setDate(logical_time);

    Debug(D, pdRegister) << "timeAdvanceGrant sent to federate, time = " << req.getDate().getTime() << std::endl;

    if (_lookahead_courant == epsilon2) {
        _lookahead_courant = 0.0;
    }

    _tick_state = TICK_NEXT; // indicate the callback was processed

    comm->requestFederateService(&req);

    _heure_courante = logical_time;
    sendTimeStateUpdate();
}

void TimeManagement::flushQueueRequest(FederationTime heure_logique, Exception::Type& e)
{
    e = Exception::Type::NO_EXCEPTION;

    // Verifications
    if (_avancee_en_cours != PAS_D_AVANCEE) {
        e = Exception::Type::TimeAdvanceAlreadyInProgress;
    }

    if (heure_logique <= _heure_courante) {
        e = Exception::Type::FederationTimeAlreadyPassed;
    }

    if (e == Exception::Type::NO_EXCEPTION) {
        // BUG: Not implemented.
        Debug(D, pdExcept) << "flushQueueRequest not implemented." << std::endl;
        throw RTIinternalError("flushQueueRequest not implemented.");
    }
}

bool TimeManagement::executeFederateService(NetworkMessage& msg)
{
    Debug(G, pdGendoc) << "enter TimeManagement::executeFederateService for type "
                       << static_cast<int>(msg.getMessageType()) << std::endl;
    Debug(D, pdRequest) << "Execute federate service: Type " << static_cast<int>(msg.getMessageType()) << std::endl;

    _tick_state = TICK_NEXT; // indicate the callback was processed

    switch (msg.getMessageType()) {
    case NetworkMessage::Type::FEDERATION_SYNCHRONIZED:
        try {
            fm->federationSynchronized(msg.getLabel());
        }
        catch (RTIinternalError& e) {
            Debug(D, pdError) << "RTIA:RTIinternalError in federationSynchronized." << std::endl;
            throw;
        }
        break;

    case NetworkMessage::Type::CONFIRM_SYNCHRONIZATION_POINT_REGISTRATION:
        try {
            NM_Confirm_Synchronization_Point_Registration& CSPR
                = static_cast<NM_Confirm_Synchronization_Point_Registration&>(msg);
            if (CSPR.getSuccessIndicator()) {
                fm->synchronizationPointRegistrationSucceeded(CSPR.getLabel());
            }
            else {
                fm->synchronizationPointRegistrationFailed(CSPR.getLabel());
            }
        }
        catch (RTIinternalError& e) {
            Debug(D, pdError) << "RTIA:RTIinternalError in synchronizationPointRegistrationSucceeded." << std::endl;
            throw;
        }
        break;

    case NetworkMessage::Type::ANNOUNCE_SYNCHRONIZATION_POINT:
        try {
            fm->announceSynchronizationPoint(msg.getLabel(), msg.getTag());
        }
        catch (RTIinternalError& e) {
            Debug(D, pdError) << "RTIA:RTIinternalError in announceSynchronizationPoint." << std::endl;
            throw;
        }
        break;

    case NetworkMessage::Type::DISCOVER_OBJECT:
        try {
            NM_Discover_Object& DO = static_cast<NM_Discover_Object&>(msg);
            om->discoverObject(DO.getObject(),
                               DO.getObjectClass(),
                               msg.getLabel(),
                               msg.getDate(),
                               msg.eventRetraction,
                               msg.getRefException());
        }
        catch (RTIinternalError& e) {
            Debug(D, pdError) << "RTIA:RTIinternalError in discoverObject." << std::endl;
            throw;
        }
        break;

    case NetworkMessage::Type::REFLECT_ATTRIBUTE_VALUES: {
        NM_Reflect_Attribute_Values& RAV = static_cast<NM_Reflect_Attribute_Values&>(msg);
        if (msg.isDated())
            om->reflectAttributeValues(RAV.getObject(),
                                       RAV.getAttributes(),
                                       RAV.getValues(),
                                       RAV.getAttributesSize(),
                                       msg.getDate(),
                                       msg.getLabel(),
                                       msg.eventRetraction,
                                       msg.getRefException());
        else
            om->reflectAttributeValues(RAV.getObject(),
                                       RAV.getAttributes(),
                                       RAV.getValues(),
                                       RAV.getAttributesSize(),
                                       msg.getLabel(),
                                       msg.getRefException());
        break;
    }

    case NetworkMessage::Type::PROVIDE_ATTRIBUTE_VALUE_UPDATE: {
        NM_Provide_Attribute_Value_Update& PAVU = static_cast<NM_Provide_Attribute_Value_Update&>(msg);
        om->provideAttributeValueUpdate(
            PAVU.getObject(), PAVU.getAttributes(), PAVU.getAttributesSize(), msg.getRefException());
        break;
    }

    case NetworkMessage::Type::RECEIVE_INTERACTION: {
        NM_Receive_Interaction& RI = static_cast<NM_Receive_Interaction&>(msg);
        if (msg.isDated())
            om->receiveInteraction(RI.getInteractionClass(),
                                   RI.getParameters(),
                                   RI.getValues(),
                                   RI.getParametersSize(),
                                   msg.getDate(),
                                   msg.getLabel(),
                                   msg.eventRetraction,
                                   msg.getRefException());
        else
            om->receiveInteraction(RI.getInteractionClass(),
                                   RI.getParameters(),
                                   RI.getValues(),
                                   RI.getParametersSize(),
                                   msg.getLabel(),
                                   msg.getRefException());

        break;
    }

    case NetworkMessage::Type::REMOVE_OBJECT: {
        NM_Remove_Object& RO = static_cast<NM_Remove_Object&>(msg);
        if (msg.isDated()) {
            om->removeObject(RO.getObject(),
                             msg.getFederate(),
                             msg.getDate(),
                             msg.getLabel(),
                             msg.eventRetraction,
                             msg.getRefException());
        }
        else {
            om->removeObject(RO.getObject(), msg.getFederate(), msg.getLabel(), msg.getRefException());
        }
    } break;

    case NetworkMessage::Type::INFORM_ATTRIBUTE_OWNERSHIP: {
        NM_Inform_Attribute_Ownership& IAO = static_cast<NM_Inform_Attribute_Ownership&>(msg);
        Debug(D, pdInit) << "m_REFLECT_ATTRIBUTE_VALUES Owner " << msg.getFederate() << std::endl;

        owm->informAttributeOwnership(IAO.getObject(), IAO.getAttribute(), msg.getFederate(), msg.getRefException());
    } break;

    case NetworkMessage::Type::ATTRIBUTE_IS_NOT_OWNED: {
        NM_Attribute_Is_Not_Owned& AINO = static_cast<NM_Attribute_Is_Not_Owned&>(msg);
        owm->attributeIsNotOwned(AINO.getObject(), AINO.getAttribute(), msg.getFederate(), msg.getRefException());
    } break;

    case NetworkMessage::Type::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION: {
        NM_Request_Attribute_Ownership_Assumption& RAOA = static_cast<NM_Request_Attribute_Ownership_Assumption&>(msg);
        owm->requestAttributeOwnershipAssumption(RAOA.getObject(),
                                                 RAOA.getAttributes(),
                                                 RAOA.getAttributesSize(),
                                                 msg.getFederate(),
                                                 msg.getLabel(),
                                                 msg.getRefException());
        break;
    }

    case NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_UNAVAILABLE: {
        NM_Attribute_Ownership_Unavailable& AOU = static_cast<NM_Attribute_Ownership_Unavailable&>(msg);
        owm->attributeOwnershipUnavailable(
            AOU.getObject(), AOU.getAttributes(), AOU.getAttributesSize(), msg.getFederate(), msg.getRefException());
        break;
    }

    case NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION: {
        NM_Attribute_Ownership_Acquisition_Notification& AOAN
            = static_cast<NM_Attribute_Ownership_Acquisition_Notification&>(msg);
        owm->attributeOwnershipAcquisitionNotification(
            AOAN.getObject(), AOAN.getAttributes(), AOAN.getAttributesSize(), msg.getFederate(), msg.getRefException());
        break;
    }

    case NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION: {
        NM_Attribute_Ownership_Divestiture_Notification& AODN
            = static_cast<NM_Attribute_Ownership_Divestiture_Notification&>(msg);
        owm->attributeOwnershipDivestitureNotification(
            AODN.getObject(), AODN.getAttributes(), AODN.getAttributesSize(), msg.getRefException());
        break;
    }

    case NetworkMessage::Type::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE: {
        NM_Request_Attribute_Ownership_Release& RAOR = static_cast<NM_Request_Attribute_Ownership_Release&>(msg);
        owm->requestAttributeOwnershipRelease(
            RAOR.getObject(), RAOR.getAttributes(), RAOR.getAttributesSize(), msg.getLabel(), msg.getRefException());
        break;
    }

    case NetworkMessage::Type::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION: {
        NM_Confirm_Attribute_Ownership_Acquisition_Cancellation& CAOAC
            = static_cast<NM_Confirm_Attribute_Ownership_Acquisition_Cancellation&>(msg);
        owm->confirmAttributeOwnershipAcquisitionCancellation(
            CAOAC.getObject(), CAOAC.getAttributes(), CAOAC.getAttributesSize(), msg.getRefException());
        break;
    }

    case NetworkMessage::Type::INITIATE_FEDERATE_SAVE:
        fm->initiateFederateSave(msg.getLabel());
        break;

    case NetworkMessage::Type::FEDERATION_SAVED:
    case NetworkMessage::Type::FEDERATION_NOT_SAVED: {
        bool status = (msg.getMessageType() == NetworkMessage::Type::FEDERATION_SAVED) ? true : false;
        fm->federationSavedStatus(status);
    } break;

    case NetworkMessage::Type::REQUEST_FEDERATION_RESTORE_SUCCEEDED:
    case NetworkMessage::Type::REQUEST_FEDERATION_RESTORE_FAILED: {
        bool status
            = (msg.getMessageType() == NetworkMessage::Type::REQUEST_FEDERATION_RESTORE_SUCCEEDED) ? true : false;
        fm->requestFederationRestoreStatus(status, msg.getLabel(), msg.getTag());
    } break;

    case NetworkMessage::Type::FEDERATION_RESTORE_BEGUN:
        fm->federationRestoreBegun();
        break;

    case NetworkMessage::Type::INITIATE_FEDERATE_RESTORE:
        fm->initiateFederateRestore(msg.getLabel(), msg.getFederate());
        break;

    case NetworkMessage::Type::FEDERATION_RESTORED:
    case NetworkMessage::Type::FEDERATION_NOT_RESTORED: {
        bool status = (msg.getMessageType() == NetworkMessage::Type::FEDERATION_RESTORED) ? true : false;
        fm->federationRestoredStatus(status);
    } break;
    case NetworkMessage::Type::TIME_REGULATION_ENABLED:
        this->timeRegulationEnabled(msg.getDate(), msg.getRefException());
        break;
    case NetworkMessage::Type::TIME_CONSTRAINED_ENABLED:
        this->timeConstrainedEnabled(msg.getDate(), msg.getRefException());
        break;
    case NetworkMessage::Type::START_REGISTRATION_FOR_OBJECT_CLASS: {
        NM_Start_Registration_For_Object_Class& SRFOC = static_cast<NM_Start_Registration_For_Object_Class&>(msg);
        dm->startRegistrationForObjectClass(SRFOC.getObjectClass(), msg.getRefException());
    } break;

    case NetworkMessage::Type::RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED: {
        NM_Reserve_Object_Instance_Name_Succeeded ROINS = static_cast<NM_Reserve_Object_Instance_Name_Succeeded&>(msg);
        om->nameReservationSucceeded(ROINS.getObjectName());
    } break;
    case NetworkMessage::Type::RESERVE_OBJECT_INSTANCE_NAME_FAILED: {
        NM_Reserve_Object_Instance_Name_Failed ROINF = static_cast<NM_Reserve_Object_Instance_Name_Failed&>(msg);
        om->nameReservationFailed(ROINF.getObjectName());
    } break;

    default:
        Debug(D, pdExcept) << "Unknown message type in executeFederateService." << std::endl;
        throw RTIinternalError("Unknown message <" + std::string(msg.getMessageName())
                               + "> in executeFederateService.");
    }
    Debug(G, pdGendoc) << "exit  TimeManagement::executeFederateService" << std::endl;
    return true;
}

void TimeManagement::sendNullMessage(FederationTime logical_time)
{
    NM_Message_Null msg;

    // Chandy-Misra NMA indicates that NULL message timestamp
    // must be logical time + lookahead
    logical_time += _lookahead_courant;

    if (logical_time > _lastNullMessageDate) {
        msg.setFederation(fm->getFederationHandle().get());
        msg.setFederate(fm->getFederateHandle());
        msg.setDate(logical_time);

        comm->sendMessage(&msg);
        _lastNullMessageDate = logical_time;
        Debug(DNULL, pdDebug) << "NULL message sent, Time = " << logical_time.getTime() << std::endl;
    }
    else {
        Debug(DNULL, pdExcept) << "NULL message not sent, Time = " << logical_time.getTime()
                               << ", Last = " << _lastNullMessageDate.getTime() << std::endl;
    }
}

FederationTime TimeManagement::sendNullPrimeMessage(FederationTime logical_time)
{
    NM_Message_Null_Prime msg;
#ifdef CERTI_USE_NULL_PRIME_MESSAGE_PROTOCOL
    /*
     * If the time stamp of the last Tx event is bigger than current time and
     * smaller than the requested advance time then it has to be the next
     * time advance timestamp
     */
     if (_MyEventTimestampList.size() > 0){
		FederationTime smallestEventTimeStamp(_MyEventTimestampList[0]);
		if ((smallestEventTimeStamp > _heure_courante) && (smallestEventTimeStamp < logical_time)) {
			logical_time = smallestEventTimeStamp;
			}
	}
    
    /*
     * We cannot send null prime in the past of
     *  - the last NULL message
     *  - the last NULL PRIME message
     */
    if ((logical_time > _lastNullMessageDate) || (logical_time > _lastNullPrimeMessageDate)) {
        msg.setFederation(fm->getFederationHandle().get());
        msg.setFederate(fm->getFederateHandle());
        msg.setDate(logical_time);
        comm->sendMessage(&msg);
        _lastNullPrimeMessageDate = logical_time;
        Debug(DNULL, pdDebug) << "NULL PRIME message sent, Time = " << logical_time.getTime() << std::endl;
    }
    else {
        Debug(DNULL, pdExcept) << "NULL PRIME message not sent, Time = " << logical_time.getTime()
                               << ", Last NULL= " << _lastNullMessageDate.getTime()
                               << ", Last NULL PRIME = " << _lastNullPrimeMessageDate.getTime() << std::endl;
    }
   return logical_time;
#else
    return logical_time;
#endif
}

void TimeManagement::timeRegulationEnabled(FederationTime logical_time, Exception::Type& /*e*/)
{
    M_Time_Regulation_Enabled req;

    Debug(D, pdDebug) << "Sending TIME_REGULATION_ENABLED to Federate" << std::endl;
    req.setDate(logical_time);
    comm->requestFederateService(&req);
}

void TimeManagement::timeConstrainedEnabled(FederationTime logical_time, Exception::Type& /*e*/)
{
    M_Time_Constrained_Enabled req;

    Debug(D, pdDebug) << "Sending TIME_CONSTRAINED_ENABLED to Federate" << std::endl;
    req.setDate(logical_time);
    comm->requestFederateService(&req);
}

void TimeManagement::sendTimeStateUpdate()
{
    if(my_updateRate == std::chrono::seconds(0)) {
        // mom disabled or no periodic update, do nothing
        return;
    }
    
    static std::chrono::time_point<std::chrono::system_clock> my_lastUpdate{};
    static NM_Time_State_Update last_msg{};
    NM_Time_State_Update msg;

    msg.setFederation(fm->getFederationHandle().get());
    msg.setFederate(fm->getFederateHandle());
    msg.setDate(_heure_courante);

    msg.setLookahead(_lookahead_courant.getTime());
    msg.setState(_avancee_en_cours == TAR);
    msg.setGalt(_LBTS.getTime());
    msg.setLits(requestMinNextEventTime().getTime());
    
    if(msg.getDate() == last_msg.getDate()
        && msg.getLookahead() == last_msg.getLookahead()
        && msg.getState() == last_msg.getState()
        && msg.getGalt() == last_msg.getGalt()
        && msg.getLits() == last_msg.getLits()) {
        // nothing changed, do nothing
        return;
    }

    auto now = std::chrono::system_clock::now();
    
    if(now - my_lastUpdate >= my_updateRate) {
        comm->sendMessage(&msg);
        last_msg = msg;
        my_lastUpdate = now;
        
        Debug(DTUS, pdDebug) << "Time State Update sent" << std::endl;
    }
}
}
} // namespaces
