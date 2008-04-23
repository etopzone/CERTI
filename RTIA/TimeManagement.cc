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
// $Id: TimeManagement.cc,v 3.29.2.5 2008/04/23 06:23:23 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "TimeManagement.hh"
#include "NM_Classes.hh"
#include <float.h>

namespace certi {
namespace rtia {

namespace {

PrettyDebug D("RTIA_TM", __FILE__);
static PrettyDebug G("GENDOC",__FILE__) ;
const double epsilon = 1.0e-9 ;

}

// ----------------------------------------------------------------------------
/*! This method is called by tick(). Calls are dispatched between timeAdvance
  and nextEventAdvance.
*/
void
TimeManagement::advance(bool &msg_restant, TypeException &e)
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
    _ongoing_tick          = false ;
    _asynchronous_delivery = false ;

    _heure_courante = 0.0 ;
    _lookahead_courant = epsilon ;
    _est_regulateur = false ;
    _est_contraint = false ;
}

// ----------------------------------------------------------------------------
//! Send a null message to RTIG containing Local Time + Lookahead.
void TimeManagement::sendNullMessage(FederationTime heure_logique)
{
    NM_Message_Null msg ;

    msg.setDate(heure_logique);
    heure_logique += _lookahead_courant ;

    if (heure_logique > lastNullMessageDate) {        
        msg.federation = fm->_numero_federation ;
        msg.federate = fm->federate ;
        msg.setDate(heure_logique) ; // ? See 6 lines upper !

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
bool
TimeManagement::executeFederateService(NetworkMessage &msg)
{
  G.Out(pdGendoc,"enter TimeManagement::executeFederateService");
  D.Out(pdRequest, "Execute federate service: Type %d (%s).", msg.getType(),msg.getName().c_str());

  _ongoing_tick = false ;  // end of the blocking tick, a message is delivered
  _tick_request_ack = false ;  // the callback message serves as the ack

    switch (msg.getType()) {

      case NetworkMessage::FEDERATION_SYNCHRONIZED:
        try {
            fm->federationSynchronized(msg.getLabel().c_str());
        }
        catch (RTIinternalError &e) {
            cout << "RTIA:RTIinternalError in federationSynchronized." << endl ;
            throw e ;
        }
        break ;

      case NetworkMessage::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
        try {
            fm->synchronizationPointRegistrationSucceeded(msg.getLabel().c_str());
        }
        catch (RTIinternalError &e) {
            cout << "RTIA:RTIinternalError in synchronizationPointRegistration"
                "Succeeded." << endl ;
            throw e ;
        }
        break ;

      case NetworkMessage::ANNOUNCE_SYNCHRONIZATION_POINT:
        try {
            fm->announceSynchronizationPoint(msg.getLabel().c_str(), msg.getTag().c_str());
        }
        catch (RTIinternalError &e) {
            cout << "RTIA:RTIinternalError in announceSynchronizationPoint." << endl ;
            throw e ;
        }
        break ;

      case NetworkMessage::DISCOVER_OBJECT:
        try {
            om->discoverObject(msg.object,
                               msg.objectClass,
                               msg.getLabel().c_str(),
                               msg.getDate(),
                               msg.eventRetraction,
                               msg.exception);

        }
        catch (RTIinternalError &e) {
            cout << "RTIA:RTIinternalError in discoverObject." << endl ;
            throw e ;
        }
        break ;

      case NetworkMessage::REFLECT_ATTRIBUTE_VALUES:
      {
          ValueLengthPair *ValueArray = msg.getAttribValueArray();

          if (msg.isDated)
             om->reflectAttributeValues(msg.object,
                                        msg.handleArray,
                                        ValueArray,
                                        msg.handleArraySize,
                                        msg.getDate(),
                                        msg.getLabel().c_str(),
                                        msg.eventRetraction,
                                        msg.exception);
          else
            om->reflectAttributeValues(msg.object,
                                        msg.handleArray,
                                        ValueArray,
                                        msg.handleArraySize,
                                        msg.getLabel().c_str(),
                                        msg.exception);
          free(ValueArray);
          break ;
      }

      case NetworkMessage::PROVIDE_ATTRIBUTE_VALUE_UPDATE:
      { 
      om->provideAttributeValueUpdate(msg.object,
                                      msg.handleArray,
                                      msg.handleArraySize,
                                      msg.exception);
      break;
      }


      case NetworkMessage::RECEIVE_INTERACTION:
      {
          ParameterLengthPair *ValueArray = msg.getParamValueArray();

          if (msg.isDated)
              om->receiveInteraction(msg.interactionClass,
                                     msg.handleArray,
                                     ValueArray,
                                     msg.handleArraySize,
                                     msg.getDate(),
                                     msg.getLabel().c_str(),
                                     msg.eventRetraction,
                                     msg.exception);
          else
              om->receiveInteraction(msg.interactionClass,
                                     msg.handleArray,
                                     ValueArray,
                                     msg.handleArraySize,
                                     msg.getLabel().c_str(),
                                     msg.exception);
          free(ValueArray);

          break ;
      }

      case NetworkMessage::REMOVE_OBJECT:
          if (msg.isDated) {
        	om->removeObject(msg.object,
                		 msg.federate,
				 msg.getDate(),
                         	 msg.getLabel().c_str(),
                         	 msg.eventRetraction,
                         	 msg.exception);
	  }
	  else {
        	om->removeObject(msg.object,
                		 msg.federate,
                         	 msg.getLabel().c_str(),
                         	 msg.exception);
	  }
        break ;

      case NetworkMessage::INFORM_ATTRIBUTE_OWNERSHIP:

        D.Out(pdInit, "m_REFLECT_ATTRIBUTE_VALUES Owner %u", msg.federate);

        owm->informAttributeOwnership(msg.object,
                                      msg.handleArray[0],
                                      msg.federate,
                                      msg.exception);
        break ;

      case NetworkMessage::ATTRIBUTE_IS_NOT_OWNED:
        owm->attributeIsNotOwned(msg.object,
                                 msg.handleArray[0],
                                 msg.federate,
                                 msg.exception);
        break ;

      case NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
        owm->requestAttributeOwnershipAssumption(msg.object,
                                                 msg.handleArray,
                                                 msg.handleArraySize,
                                                 msg.federate,
                                                 const_cast<char*>(msg.getLabel().c_str()),
                                                 msg.exception);
        break ;

      case NetworkMessage::ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
        owm->attributeOwnershipUnavailable(msg.object,
                                           msg.handleArray,
                                           msg.handleArraySize,
                                           msg.federate,
                                           msg.exception);
        break ;

      case NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
        owm->attributeOwnershipAcquisitionNotification(msg.object,
                                                       msg.handleArray,
                                                       msg.handleArraySize,
                                                       msg.federate,
                                                       msg.exception);
        break ;

      case NetworkMessage::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
        owm->attributeOwnershipDivestitureNotification(msg.object,
                                                       msg.handleArray,
                                                       msg.handleArraySize,
                                                       msg.exception);
        break ;

      case NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
        owm->requestAttributeOwnershipRelease(msg.object,
                                              msg.handleArray,
                                              msg.handleArraySize,
                                              const_cast<char*>(msg.getLabel().c_str()),
                                              msg.exception);
        break ;

      case NetworkMessage::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
        owm->confirmAttributeOwnershipAcquisitionCancellation(msg.object,
                                                              msg.handleArray,
                                                              msg.handleArraySize,
                                                              msg.exception);
        break ;

      case NetworkMessage::INITIATE_FEDERATE_SAVE:
        fm->initiateFederateSave(msg.getLabel().c_str());
        break ;

      case NetworkMessage::FEDERATION_SAVED:
      case NetworkMessage::FEDERATION_NOT_SAVED: {
          bool status = (msg.getType() == NetworkMessage::FEDERATION_SAVED) ? true : false ;
          fm->federationSavedStatus(status);
      }
        break ;

      case NetworkMessage::REQUEST_FEDERATION_RESTORE_SUCCEEDED:
      case NetworkMessage::REQUEST_FEDERATION_RESTORE_FAILED: {
          bool status = (msg.getType() == NetworkMessage::REQUEST_FEDERATION_RESTORE_SUCCEEDED)
              ? true : false ;
          fm->requestFederationRestoreStatus(status, msg.getLabel().c_str(), msg.getTag().c_str());
      }
        break ;

      case NetworkMessage::FEDERATION_RESTORE_BEGUN:
        fm->federationRestoreBegun();
        break ;

      case NetworkMessage::INITIATE_FEDERATE_RESTORE:
        fm->initiateFederateRestore(msg.getLabel().c_str(), msg.federate);
        break ;

      case NetworkMessage::FEDERATION_RESTORED:
      case NetworkMessage::FEDERATION_NOT_RESTORED: {
          bool status = (msg.getType() == NetworkMessage::FEDERATION_RESTORED) ? true : false ;
          fm->federationRestoredStatus(status);
      }
        break ;

      default:
	std::stringstream errorMsg;
        D.Out(pdExcept, "Unknown message type in executeFederateService.");        
	errorMsg << "Unknown message <" <<  msg.getName() << " in executeFederateService.";
        throw RTIinternalError(errorMsg.str().c_str());
    }
    G.Out(pdGendoc,"exit  TimeManagement::executeFederateService");
    return true ;
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
TimeManagement::nextEventAdvance(bool &msg_restant, TypeException &e)
{
    FederationTime dateTSO ;
    FederationTime date_min = 0.0 ;
    bool TSOPresent ;
    bool msg_donne ;
    NetworkMessage *msg ;

    G.Out(pdGendoc," enter TimeManagement::nextEventAdvance");
    msg_restant = false ;

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
G.Out(pdGendoc," exit  TimeManagement::nextEventAdvance");
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
TimeManagement::requestFederationTime()
{
    if (_heure_courante < _LBTS)
        return _heure_courante ;
    else
        return _LBTS ;
}

// ----------------------------------------------------------------------------
FederationTimeDelta TimeManagement::requestLookahead()
{
    // BUG: C'est quoi cette salade ?

    if (_heure_courante + _lookahead_courant < lastNullMessageDate)
        return(lastNullMessageDate - _heure_courante);
    else
        return _lookahead_courant ;
}

// ----------------------------------------------------------------------------
//!Return the min of LBTS and the time stamp of the next TSO message
FederationTime
TimeManagement::requestMinNextEventTime()
{
    FederationTime dateTSO ;
    FederationTime dateMNET ;
    bool found ;
 
    queues->nextTsoDate(found, dateTSO) ;

    if (!found)
        dateMNET = _LBTS ;
    else
        dateMNET = (_LBTS <= dateTSO ? _LBTS : dateTSO) ;

   D.Out(pdRegister, "Minimum Next Event Time : %f.", dateMNET);

   return dateMNET ;
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
void
TimeManagement::setTimeConstrained(bool etat, TypeException &e)
{
    NM_Set_Time_Constrained msg ;

    e = e_NO_EXCEPTION ;

    // Verifications

    if (_est_contraint == etat)
        e = e_RTIinternalError ;

    if (_avancee_en_cours != PAS_D_AVANCEE)
        e = e_RTIinternalError ;

    if (e == e_NO_EXCEPTION) {
        _est_contraint = etat ;

        msg.federation = fm->_numero_federation ;
        msg.federate = fm->federate ;
        if (etat) {
        	msg.constrainedOn();
        } else {
        	msg.constrainedOff();
        }        

        comm->sendMessage(&msg);

        D.Out(pdRegister,
              "Demande de modif de TimeConstrained envoyee(etat=%d, ", etat);
    }
    else {
        D.Out(pdExcept, "SetTimeConstrained refuse(exception = %d).", e);
    }
}

// ----------------------------------------------------------------------------
void
TimeManagement::setTimeRegulating(bool etat, TypeException &e)
{
    NM_Set_Time_Regulating msg ;

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

    // Si aucune erreur, prevenir le RTIG et devenir regulateur.
    if (e == e_NO_EXCEPTION) {
        _est_regulateur = etat ;

        msg.federation = fm->_numero_federation ;
        msg.federate = fm->federate ;
        if (etat) {
        	msg.regulatorOn();
        } else {
        	msg.regulatorOff();
        }        
        msg.setDate(_heure_courante + _lookahead_courant);

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
bool
TimeManagement::tick(TypeException &e)
{
    bool msg_donne = false ;
    bool msg_restant = false ;
    NetworkMessage *msg = NULL ;

    G.Out(pdGendoc," enter TimeManagement::tick");
    // Note: While msg_donne = RTI::RTI_FALSE, msg_restant doesn't matter.

    // 1st try, give a command message. (requestPause, etc.)
    msg = queues->giveCommandMessage(msg_donne, msg_restant);


    // 2nd try, give a FIFO message. (discoverObject, etc.)
    if (!msg_donne)
        if ( _asynchronous_delivery || (_avancee_en_cours != PAS_D_AVANCEE) || (! _est_contraint))
          // to exclude the case not asynchronous_delivery and
          // not time advancing for a constrained federate
          msg = queues->giveFifoMessage(msg_donne, msg_restant);

    // If message exists, send it to federate.
    if (msg_donne) {
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

    G.Out(pdGendoc," exit  TimeManagement::tick");
    return msg_restant ;
}

// ----------------------------------------------------------------------------
/*! timeAdvance is called by advance which is called by tick. This call is
  done only if request type does correspond. It delivers TSO messages to
  federate and if no messages are available, delivers a TimeAdvanceGrant.
*/
void
TimeManagement::timeAdvance(bool &msg_restant, TypeException &e)
{
    bool msg_donne ;
    FederationTime min ;
    NetworkMessage *msg ;
    G.Out(pdGendoc," enter TimeManagement::timeAdvance");
    msg_restant = false ;

    if (_est_contraint) {
        // give a TSO message.
        if (_LBTS == DBL_MAX)
           D.Out(pdDebug, "Logical time : %f, LBTS : infini.", date_avancee);
        else
           D.Out(pdDebug, "Logical time : %f, LBTS : %lf.", date_avancee, _LBTS);
        min = (_LBTS<date_avancee)?(_LBTS):(date_avancee);
        msg = queues->giveTsoMessage(min, msg_donne, msg_restant);

        // otherwise
        if (!msg_donne) {
            // if LBTS allows to give a timeAdvanceGrant.
            if (_LBTS == DBL_MAX)
               D.Out(pdDebug, "Logical time : %f, LBTS : infini, lookahead : %f.",
                     date_avancee, _lookahead_courant);
            else
               D.Out(pdDebug, "Logical time : %f, LBTS : %lf, lookahead : %f.",
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
G.Out(pdGendoc," exit  TimeManagement::timeAdvance");
}

// ----------------------------------------------------------------------------
/*! Once every messages has been delivered to federate, logical time can be
  advanced and send a timeAdvanceGrant to federate.
*/
void
TimeManagement::timeAdvanceGrant(FederationTime logical_time,
                                 TypeException &e)
{
    Message req;

    req.type = Message::TIME_ADVANCE_GRANT ;
    req.setFederationTime(logical_time);

    D.Out(pdRegister, "timeAdvanceGrant sent to federate (time = %f).",
          req.getFederationTime());

    _ongoing_tick = false ;  // end of the blocking tick, a message is delivered
    _tick_request_ack = false ;

    comm->requestFederateService(&req);

    _heure_courante = logical_time ;
}

// ----------------------------------------------------------------------------
/*! Either nextEventRequest or timeAdvanceRequest is called by federate to
  determine time to reach. It then calls tick() until a timeAdvanceGrant is
  received.
*/
void
TimeManagement::timeAdvanceRequest(FederationTime logical_time,
                                   TypeException &e)
{
    e = e_NO_EXCEPTION ;

    // Verifications

    if (_avancee_en_cours != PAS_D_AVANCEE)
        e = e_TimeAdvanceAlreadyInProgress ;

    if (logical_time <= _heure_courante)
        e = e_FederationTimeAlreadyPassed ;


    if (e == e_NO_EXCEPTION) {
        if (_est_regulateur)
            sendNullMessage(logical_time);

        _avancee_en_cours = TAR ;
        date_avancee = logical_time ;

        D.Out(pdTrace, "timeAdvanceRequest accepted (asked time=%f).",
              date_avancee);
    }
    else {
        D.Out(pdExcept, "timeAdvanceRequest refused (exception = %d).", e);
    }
}

}} // namespaces

// $Id: TimeManagement.cc,v 3.29.2.5 2008/04/23 06:23:23 erk Exp $
