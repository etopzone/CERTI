// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: TimeManagement.cc,v 3.6 2003/03/21 15:06:46 breholee Exp $
// ----------------------------------------------------------------------------

#include "TimeManagement.hh"

namespace certi {
namespace rtia {

static pdCDebug D("RTIA_TM", "(RTIA TM) ");

// ----------------------------------------------------------------------------
/*! This method is called by tick(). Calls are dispatched between timeAdvance
  and nextEventAdvance.
*/
void
TimeManagement::advance(Boolean &msg_restant, TypeException &e)
{
    switch(_avancee_en_cours) {
    case TAR:
        D.Out(pdTrace, "Call to TimeAdvance.");
        timeAdvance(msg_restant, e);
        break ;
    case NER:
        D.Out(pdTrace, "Call to NextEventAdvance.");
        nextEventAdvance(msg_restant, e);
        break ;
    default:
        D.Out(pdTrace, "Unexpected case in advance: %d.", _avancee_en_cours);
                                       // No exception is raised, ca
                                       // peut etre un cas ou on a
                                       // rien a faire, par exemple en
                                       // cas d'attente active pendant
                                       // une pause.
    }
}

// ----------------------------------------------------------------------------
//! Constructor.
TimeManagement::TimeManagement(Communications *GC,
                               Queues *GQueues,
                               FederationManagement *GF,
                               ObjectManagement *GO,
                               OwnershipManagement *GP)
    : LBTS()
{
    comm = GC ;
    queues = GQueues ;
    fm = GF ;
    om = GO ;
    owm = GP ;

    lastNullMessageDate = 0.0 ;

    _avancee_en_cours = PAS_D_AVANCEE ;

    _heure_courante = 0.0 ;
    _lookahead_courant = EPSILON ;
    _est_regulateur = RTI_FALSE ;
    _est_contraint = RTI_FALSE ;

}

// ----------------------------------------------------------------------------
//! Send a null message to RTIG containing Local Time + Lookahead.
void TimeManagement::sendNullMessage(FederationTime heure_logique)
{
    NetworkMessage msg ;

    msg.date = heure_logique ;
    heure_logique += _lookahead_courant ;

    if (heure_logique > lastNullMessageDate) {
        msg.type = m_MESSAGE_NULL ;
        msg.federation = fm->_numero_federation ;
        msg.federate = fm->federate ;
        msg.date = heure_logique ; // ? See 6 lines upper !

        comm->sendMessage(&msg);
        lastNullMessageDate = heure_logique ;
        D.Out(pdDebug, "NULL message sent.");
    }
    else {
        D.Out(pdExcept, "NULL message not sent (Time = %f, Last = %f).",
              heure_logique, lastNullMessageDate);
    }
}

// ----------------------------------------------------------------------------
//! Deliver TSO messages to federate (UAV, ReceiveInteraction, etc...).
Boolean TimeManagement::executeFederateService(NetworkMessage &msg)
{
    D.Out(pdRequest, "Execute federate service: Type %d.", msg.type);

    switch (msg.type) {

    case m_FEDERATION_SYNCHRONIZED:
        try {
            fm->federationSynchronized(msg.label);
        }
        catch(RTIinternalError &e) {
            cout << "RTIA:RTIinternalError in federationSynchronized." << endl;
            throw e;
        }
        break;

    case m_SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
        try {
            fm->synchronizationPointRegistrationSucceeded(msg.label);
        }
        catch(RTIinternalError &e) {
            cout << "RTIA:RTIinternalError in synchronizationPointRegistration"
                "Succeeded." << endl;
            throw e;
        }
        break;

    case m_ANNOUNCE_SYNCHRONIZATION_POINT:
        try {
            fm->announceSynchronizationPoint(msg.label, msg.tag);
        }
        catch(RTIinternalError &e) {
            cout << "RTIA:RTIinternalError in announceSynchronizationPoint." << endl;
            throw e;
        }
        break;

    case m_DISCOVER_OBJECT:
        try {
            om->discoverObject(msg.object,
                               msg.objectClass,
                               msg.label,
                               msg.date,
                               msg.eventRetraction,
                               msg.exception);

        }
        catch (RTIinternalError &e) {
            cout << "RTIA:RTIinternalError in discoverObject." << endl ;
            throw e ;
        }
        break ;

    case m_REFLECT_ATTRIBUTE_VALUES:
        {
            AttributeValue *ValueArray = msg.getAttribValueArray();

            om->reflectAttributeValues(msg.object,
                                       msg.handleArray,
                                       ValueArray,
                                       msg.handleArraySize,
                                       msg.date,
                                       msg.label,
                                       msg.eventRetraction,
                                       msg.exception);
            free(ValueArray);
            break ;
        }

    case m_RECEIVE_INTERACTION:
        {
            ParameterValue *ValueArray = msg.getParamValueArray();

            om->receiveInteraction(msg.interactionClass,
                                   msg.handleArray,
                                   ValueArray,
                                   msg.handleArraySize,
                                   msg.date,
                                   msg.label,
                                   msg.eventRetraction,
                                   msg.exception);
            free(ValueArray);

            break ;
        }

    case m_REMOVE_OBJECT:
        om->removeObject(msg.object,
                         msg.federate,
                         msg.label,
                         msg.eventRetraction,
                         msg.exception);
        break ;

    case m_INFORM_ATTRIBUTE_OWNERSHIP:

        D.Out(pdInit, "m_REFLECT_ATTRIBUTE_VALUES Owner %u", msg.federate);

        owm->informAttributeOwnership(msg.object,
                                      msg.handleArray[0],
                                      msg.federate,
                                      msg.exception);
        break ;

    case m_ATTRIBUTE_IS_NOT_OWNED:
        owm->attributeIsNotOwned(msg.object,
                                 msg.handleArray[0],
                                 msg.federate,
                                 msg.exception);
        break ;

    case m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
        owm->requestAttributeOwnershipAssumption(msg.object,
                                                 msg.handleArray,
                                                 msg.handleArraySize,
                                                 msg.federate,
                                                 msg.label,
                                                 msg.exception);
        break ;

    case m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
        owm->attributeOwnershipUnavailable(msg.object,
                                           msg.handleArray,
                                           msg.handleArraySize,
                                           msg.federate,
                                           msg.exception);
        break ;

    case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
        owm->attributeOwnershipAcquisitionNotification(msg.object,
                                                       msg.handleArray,
                                                       msg.handleArraySize,
                                                       msg.federate,
                                                       msg.exception);
        break ;

    case m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
        owm->attributeOwnershipDivestitureNotification(msg.object,
                                                       msg.handleArray,
                                                       msg.handleArraySize,
                                                       msg.exception);
        break ;

    case m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
        owm->requestAttributeOwnershipRelease(msg.object,
                                              msg.handleArray,
                                              msg.handleArraySize,
                                              msg.label,
                                              msg.exception);
        break ;

    case m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
        owm->confirmAttributeOwnershipAcquisitionCancellation(msg.object,
                                                              msg.handleArray,
                                                              msg.handleArraySize,
                                                              msg.exception);
        break ;

    default:
        D.Out(pdExcept, "Unknown message type in executeFederateService.");
        msg.display("ERROR");
        throw RTIinternalError("Unknown message in executeFederateService.");
    }

    return RTI_TRUE ;
}

// ----------------------------------------------------------------------------
//! Not implemented.
void
TimeManagement::flushQueueRequest(FederationTime heure_logique,
                                  TypeException &e)
{
    e = e_NO_EXCEPTION ;

    // Verifications
    if (_avancee_en_cours != PAS_D_AVANCEE)
        e = e_TimeAdvanceAlreadyInProgress ;

    if (heure_logique <= _heure_courante)
        e = e_FederationTimeAlreadyPassed ;

    if (e == e_NO_EXCEPTION) {
        // BUG: Not implemented.
        D.Out(pdExcept, "flushQueueRequest not implemented.");
        throw RTIinternalError("flushQueueRequest not implemented.");
    }
}

// ----------------------------------------------------------------------------
/*! nextEventAdvance is called by advance which is called by tick. This call
  is done only if request type does correspond. It delivers TSO messages to
  federate and if no messages are available, delivers a TimeAdvanceGrant.
*/
void
TimeManagement::nextEventAdvance(Boolean &msg_restant, TypeException &e)
{
    FederationTime dateTSO ;
    FederationTime date_min = 0.0 ;
    Boolean TSOPresent ;
    Boolean msg_donne ;
    NetworkMessage *msg ;

    msg_restant = RTI_FALSE ;

    if (_est_contraint) {

        // Select lower value between expected time and first TSO message time.
        queues->nextTsoDate(TSOPresent, dateTSO);

        if ((TSOPresent) && (dateTSO < date_avancee))
            date_min = dateTSO ;
        else
            date_min = date_avancee ;

        if (date_min < _LBTS) {
            // nextEventRequest is done because either a TSO message
            // can be delivered or no message with lower value than
            // expected time is avail.

            // New expected time is keep (can be first TSO message to deliver).
            // This value must not be changed.
            date_avancee = date_min ;

            // If federate is regulating, inform other federate we advanced.
            if (_est_regulateur)
                sendNullMessage(date_min);

            // Deliver to federate every TSO messages with time
            // 'date_min' (1 by 1).
            msg = queues->giveTsoMessage(date_min, msg_donne, msg_restant);
            if (msg_donne) {
                // Send message back to federate.
                executeFederateService(*msg);
                delete msg ;
            }
            else {
                // Advance current time up to 'date_min'.
                timeAdvanceGrant(date_min, e);
                _avancee_en_cours = PAS_D_AVANCEE ;
            }
        }
        else { // date_min < _LBTS

            // Federate can't advance up to expected time but up to LBTS. Other
            // federates are informed and no TSO message are sent.
            if (_est_regulateur)
                sendNullMessage(_LBTS);
        }
    }

    else { // if federate isn't constrained.

        // In this case, federate can advance freely. Moreover, there must be no
        // message in TSO list.
        if (_est_regulateur)
            sendNullMessage(date_avancee);

        timeAdvanceGrant(date_avancee, e);

        _avancee_en_cours = PAS_D_AVANCEE ;
    }
}

// ----------------------------------------------------------------------------
void
TimeManagement::nextEventRequest(FederationTime heure_logique,
                                 TypeException &e)
{
    e = e_NO_EXCEPTION ;

    // Verifications

    if (_avancee_en_cours != PAS_D_AVANCEE)
        e = e_TimeAdvanceAlreadyInProgress ;

    if (heure_logique <= _heure_courante)
        e = e_FederationTimeAlreadyPassed ;


    if (e == e_NO_EXCEPTION) {
        _avancee_en_cours = NER ;
        date_avancee = heure_logique ;
        D.Out(pdTrace, "NextEventRequest accepted.");
    }
    else {
        D.Out(pdExcept, "NextEventRequest refused (exception = %d).", e);
    }
}

// ----------------------------------------------------------------------------
FederationTime
TimeManagement::requestFederationTime(void)
{
    if (_heure_courante < _LBTS)
        return _heure_courante ;
    else
        return _LBTS ;
}

// ----------------------------------------------------------------------------
FederationTimeDelta TimeManagement::requestLookahead(void)
{
    // BUG: C'est quoi cette salade ?

    if (_heure_courante + _lookahead_courant < lastNullMessageDate)
        return(lastNullMessageDate - _heure_courante);
    else
        return _lookahead_courant ;
}

// ----------------------------------------------------------------------------
void
TimeManagement::setLookahead(FederationTimeDelta lookahead, TypeException &e)
{
    e = e_NO_EXCEPTION ;

    // Verifications

    if (lookahead <= 0.0)
        e = e_InvalidFederationTimeDelta ;


    if (e == e_NO_EXCEPTION) {
        _lookahead_courant = lookahead ;

        // On previent les autres en leur envoyant un message nul qui contient
        // notre temps local + le Lookahead.
        if (_est_regulateur)
            sendNullMessage(_heure_courante);

        D.Out(pdRegister, "New Lookahead : %f.", _lookahead_courant);
    }
}

// ----------------------------------------------------------------------------
void TimeManagement::setTimeConstrained(Boolean etat, TypeException &e)
{
    NetworkMessage msg ;

    e = e_NO_EXCEPTION ;

    // Verifications

    if (_est_contraint == etat)
        e = e_RTIinternalError ;

    if (_avancee_en_cours != PAS_D_AVANCEE)
        e = e_RTIinternalError ;


    if (e == e_NO_EXCEPTION) {
        _est_contraint = etat ;

        msg.type = m_SET_TIME_CONSTRAINED ;
        msg.federation = fm->_numero_federation ;
        msg.federate = fm->federate ;
        msg.constrained = etat ;

        comm->sendMessage(&msg);

        D.Out(pdRegister,
              "Demande de modif de TimeConstrained envoyee(etat=%d, ", etat);
    }
    else {
        D.Out(pdExcept, "SetTimeConstrained refuse(exception = %d).", e);
    }
}

// ----------------------------------------------------------------------------
void TimeManagement::setTimeRegulating(Boolean etat, TypeException &e)
{
    NetworkMessage msg ;

    e = e_NO_EXCEPTION ;

    // Verifications

    if (_est_regulateur == etat) {
        e = e_RTIinternalError ;
        D.Out(pdRegister,
              "erreur e_RTIinternalError : federe deja regulateur");
    }

    if (_avancee_en_cours != PAS_D_AVANCEE) {
        e = e_RTIinternalError ;
        D.Out(pdRegister, "erreur e_RTIinternalError avancee_en_cours");
    }

    // Si il existe d'autres federes regulateurs plus avances
    // if ((etat == RTI_TRUE) &&
    //(lg > 0) &&
    //(_heure_courante +_lookahead_courant < _LBTS)) {
    // e = e_FederationTimeAlreadyPassed ;
    // D.Out(pdRegister, "erreur e_FederationTimeAlreadyPassed ");
    // }

    // Si aucune erreur, prevenir le RTIG et devenir regulateur.
    if (e == e_NO_EXCEPTION) {
        _est_regulateur = etat ;

        msg.type = m_SET_TIME_REGULATING ;
        msg.federation = fm->_numero_federation ;
        msg.federate = fm->federate ;
        msg.regulator = etat ;
        msg.date = _heure_courante + _lookahead_courant ;

        comm->sendMessage(&msg);

        D.Out(pdRegister,
              "Demande de modif de TimeRegulating emise(etat=%d).", etat);
    }
    else {
        D.Out(pdExcept, "SetTimeRegulating refuse(exception = %d).", e);
    }
}

// ----------------------------------------------------------------------------
/*! Federate calls either nextEventRequest or timeAdvanceRequest to determine
  which time to attain. It then calls tick() until a timeAdvanceGrant is
  made.
*/
Boolean TimeManagement::tick(TypeException &e)
{
    Boolean msg_donne = RTI_FALSE ;
    Boolean msg_restant = RTI_FALSE ;
    NetworkMessage *msg = NULL ;

    // Note: While msg_donne = RTI_FALSE, msg_restant doesn't matter.

    // 1st try, give a command message. (requestPause, etc.)
    msg = queues->giveCommandMessage(msg_donne, msg_restant);


    // 2nd try, give a FIFO message. (discoverObject, etc.)
    if (msg_donne == RTI_FALSE)
        msg = queues->giveFifoMessage(msg_donne, msg_restant);

    // If message exists, send it to federate.
    if (msg_donne == RTI_TRUE) {
        D.Out(pdDebug, "TickRequest being processed, Message to send.");
        try {
            executeFederateService(*msg);
        }
        catch (RTIinternalError &e) {
            cout << "RTIA:RTIinternalError thrown in tick (execute)." << endl ;
            throw e ;
        }
    }

    // No message: we try to send TSO messages.
    // Messages to be sent depends on asked advance type.
    else {
        D.Out(pdDebug, "TickRequest being processed, advance called.");
        try {
            advance(msg_restant, e);
        }
        catch (RTIinternalError &e) {
            cout << "RTIA:RTIinternalError thrown in tick (Advance)." << endl ;
            throw e ;
        }
    }

    delete msg ;

    return msg_restant ;
}

// ----------------------------------------------------------------------------
/*! timeAdvance is called by advance which is called by tick. This call is
  done only if request type does correspond. It delivers TSO messages to
  federate and if no messages are available, delivers a TimeAdvanceGrant.
*/
void
TimeManagement::timeAdvance(Boolean &msg_restant, TypeException &e)
{
    Boolean msg_donne ;
    FederationTime min ;
    NetworkMessage *msg ;

    msg_restant = RTI_FALSE ;

    if (_est_contraint) {
        // give a TSO message.
        D.Out(pdDebug, "Logical time : %f, LBTS : %f.", date_avancee, _LBTS);
        min = (_LBTS<date_avancee)?(_LBTS):(date_avancee);
        msg = queues->giveTsoMessage(min, msg_donne, msg_restant);

        // otherwise
        if (!msg_donne) {
            // if LBTS allows to give a timeAdvanceGrant.
            D.Out(pdDebug, "Logical time : %f, LBTS : %f, lookahead : %f.",
                  date_avancee, _LBTS, _lookahead_courant);
            if (date_avancee < _LBTS) {
                // send a timeAdvanceGrant to federate.
                timeAdvanceGrant(date_avancee, e);

                if (e != e_NO_EXCEPTION)
                    return ;

                _avancee_en_cours = PAS_D_AVANCEE ;
            }
            // otherwise nothing has to be sent to federate (empty tick).
        }
        else {
            executeFederateService(*msg);
            delete msg ;
        }
    }
    else {
        // if federate is not constrained, sent a timeAdvanceGrant to federate.
        timeAdvanceGrant(date_avancee, e);
        if (e != e_NO_EXCEPTION)
            return ;
        _avancee_en_cours = PAS_D_AVANCEE ;
    }
}

// ----------------------------------------------------------------------------
/*! Once every messages has been delivered to federate, logical time can be
  advanced and send a timeAdvanceGrant to federate.
*/
void
TimeManagement::timeAdvanceGrant(FederationTime heure_logique,
                                 TypeException &e)
{
    Message req, rep ;

    req.type = TIME_ADVANCE_GRANT ;
    req.date = heure_logique ;

    D.Out(pdRegister, "timeAdvanceGrant sent to federate (time = %f).",
          req.date);

    comm->requestFederateService(&req, &rep);

    e = rep.exception ;

    if (e == e_NO_EXCEPTION)
        _heure_courante = heure_logique ;
}

// ----------------------------------------------------------------------------
/*! Either nextEventRequest or timeAdvanceRequest is called by federate to
  to determine time to reach. It then calls tick() until a timeAdvanceGrant
  is received.
*/
void
TimeManagement::timeAdvanceRequest(FederationTime heure_logique,
                                   TypeException &e)
{
    e = e_NO_EXCEPTION ;

    // Verifications

    if (_avancee_en_cours != PAS_D_AVANCEE)
        e = e_TimeAdvanceAlreadyInProgress ;

    if (heure_logique <= _heure_courante)
        e = e_FederationTimeAlreadyPassed ;


    if (e == e_NO_EXCEPTION) {
        if (_est_regulateur)
            sendNullMessage(heure_logique);

        _avancee_en_cours = TAR ;
        date_avancee = heure_logique ;

        D.Out(pdTrace, "timeAdvanceRequest accepted (asked time=%f).",
              date_avancee);
    }
    else {
        D.Out(pdExcept, "timeAdvanceRequest refused (exception = %d).", e);
    }
}

}} // namespaces

// $Id: TimeManagement.cc,v 3.6 2003/03/21 15:06:46 breholee Exp $
