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
// $Id: TimeManagement.cc,v 3.70 2011/01/11 12:29:40 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "TimeManagement.hh"
#include "NM_Classes.hh"
#include "M_Classes.hh"

#include <float.h>

namespace certi {
namespace rtia {

namespace {

PrettyDebug D("RTIA_TM", "[RTIA TM] ");
static PrettyDebug G("GENDOC",__FILE__) ;
static PrettyDebug DNULL("RTIA_NULLMSG","[RTIA NULL MSG] ");

const double epsilon2 = 1.0e-4 ;

}

// ----------------------------------------------------------------------------
void
TimeManagement::advance(bool &msg_restant, TypeException &e)
{
    switch(_avancee_en_cours) {
      case TAR:
      case TARA:
        D.Out(pdTrace, "Call to TimeAdvance.");
        timeAdvance(msg_restant, e);
        break ;
      case NER:
      case NERA:
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
} /* advance */

// ----------------------------------------------------------------------------
//! Constructor.
TimeManagement::TimeManagement(Communications *GC,
                               Queues *GQueues,
                               FederationManagement *GF,
                               DeclarationManagement *GD,
                               ObjectManagement *GO,
                               OwnershipManagement *GP)
    : LBTS()
{
    comm = GC ;
    queues = GQueues ;
    fm = GF ;
    dm = GD ;
    om = GO ;
    owm = GP ;

    lastNullMessageDate = 0.0 ;

    _avancee_en_cours = PAS_D_AVANCEE ;
    _tick_state = NO_TICK;
    _asynchronous_delivery = false ;

    _heure_courante = 0.0 ;
    _lookahead_courant = 0.0 ;
    _is_regulating = false ;
    _is_constrained = false ;
} /* end of TimeManagement */

// ----------------------------------------------------------------------------
void
TimeManagement::sendNullMessage(FederationTime logicalTime) {
    NM_Message_Null msg ;

    // Chandy-Misra NMA indicates that NULL message timestamp
    // must be logical time + lookahead
    logicalTime += _lookahead_courant ;

    if (logicalTime > lastNullMessageDate) {
        msg.setFederation(fm->_numero_federation);
        msg.setFederate(fm->federate);
        msg.setDate(logicalTime);

        comm->sendMessage(&msg);
        lastNullMessageDate = logicalTime ;
        DNULL.Out(pdDebug, "NULL message sent (Time = %f).", logicalTime.getTime()) ;
    }
    else {
        DNULL.Out(pdExcept, "NULL message not sent (Time = %f, Last = %f).",
              logicalTime.getTime(), lastNullMessageDate.getTime());
    }
} /* end of sendNullMessage */

void
TimeManagement::sendNullPrimeMessage(FederationTime logicalTime) {
    NM_Message_Null_Prime msg ;
#ifdef CERTI_USE_NULL_PRIME_MESSAGE_PROTOCOL
    /*
     * We cannot send null prime in the past of
     *  - the last NULL message
     *  - the last NULL PRIME message
     */
    if ((logicalTime > lastNullMessageDate) || (logicalTime > lastNullPrimeMessageDate)) {
        msg.setFederation(fm->_numero_federation);
        msg.setFederate(fm->federate);
        msg.setDate(logicalTime) ;
        comm->sendMessage(&msg);
        lastNullPrimeMessageDate = logicalTime ;
        DNULL.Out(pdDebug, "NULL PRIME message sent (Time = %f).", logicalTime.getTime()) ;
    }
    else {
        DNULL.Out(pdExcept, "NULL PRIME message not sent (Time = %f, Last NULL= %f, Last NULL PRIME = %f).",
              logicalTime.getTime(), lastNullMessageDate.getTime(), lastNullPrimeMessageDate.getTime());
    }
#endif
} /* end of sendNullPrimeMessage */

// ----------------------------------------------------------------------------
bool
TimeManagement::executeFederateService(NetworkMessage &msg) {

  G.Out(pdGendoc,"enter TimeManagement::executeFederateService for type %d",msg.getMessageType());
  D.Out(pdRequest, "Execute federate service: Type %d.", msg.getMessageType());

    _tick_state = TICK_NEXT;  // indicate the callback was processed

    switch (msg.getMessageType()) {

      case NetworkMessage::FEDERATION_SYNCHRONIZED:
        try {
            fm->federationSynchronized(msg.getLabel());
        }
        catch (RTIinternalError &e) {
            Debug(D,pdError) << "RTIA:RTIinternalError in federationSynchronized." << std::endl ;
            throw e;
        }
        break ;

      case NetworkMessage::CONFIRM_SYNCHRONIZATION_POINT_REGISTRATION:
        try {
          NM_Confirm_Synchronization_Point_Registration& CSPR=static_cast<NM_Confirm_Synchronization_Point_Registration&>(msg);
          if (CSPR.getSuccessIndicator()) {
            fm->synchronizationPointRegistrationSucceeded(CSPR.getLabel());
          } else {
            fm->synchronizationPointRegistrationFailed(CSPR.getLabel());
          }
        }
        catch (RTIinternalError &e) {
        	Debug(D,pdError) << "RTIA:RTIinternalError in synchronizationPointRegistration"
                "Succeeded." << std::endl ;
            throw e ;
        }
        break ;

      case NetworkMessage::ANNOUNCE_SYNCHRONIZATION_POINT:
        try {
            fm->announceSynchronizationPoint(msg.getLabel(), msg.getTag());
        }
        catch (RTIinternalError &e) {
        	Debug(D,pdError) << "RTIA:RTIinternalError in announceSynchronizationPoint." << std::endl ;
            throw e ;
        }
        break ;

      case NetworkMessage::DISCOVER_OBJECT:
        try {
        	NM_Discover_Object& DO=static_cast<NM_Discover_Object&>(msg);
            om->discoverObject(DO.getObject(),
                               DO.getObjectClass(),
                               msg.getLabel(),
                               msg.getDate(),
                               msg.eventRetraction,
                               msg.getRefException());

        }
        catch (RTIinternalError &e) {
        	Debug(D,pdError) << "RTIA:RTIinternalError in discoverObject." << std::endl ;
            throw e ;
        }
        break ;

      case NetworkMessage::REFLECT_ATTRIBUTE_VALUES:
      {
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
          break ;
      }

      case NetworkMessage::PROVIDE_ATTRIBUTE_VALUE_UPDATE:
      {
      NM_Provide_Attribute_Value_Update& PAVU = static_cast<NM_Provide_Attribute_Value_Update&>	(msg);
      om->provideAttributeValueUpdate(PAVU.getObject(),
                                      PAVU.getAttributes(),
                                      PAVU.getAttributesSize(),
                                      msg.getRefException());
      break;
      }


      case NetworkMessage::RECEIVE_INTERACTION:
      {
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

          break ;
      }

      case NetworkMessage::REMOVE_OBJECT: {
    	  NM_Remove_Object& RO=static_cast<NM_Remove_Object&>(msg);
          if (msg.isDated()) {
        	om->removeObject(RO.getObject(),
                		     msg.getFederate(),
				             msg.getDate(),
                         	 msg.getLabel(),
                         	 msg.eventRetraction,
                         	 msg.getRefException());
	  }
	  else {
        	om->removeObject(RO.getObject(),
                		     msg.getFederate(),
                         	 msg.getLabel(),
                         	 msg.getRefException());
	  }
      }
        break ;

      case NetworkMessage::INFORM_ATTRIBUTE_OWNERSHIP: {
    	 NM_Inform_Attribute_Ownership& IAO = static_cast<NM_Inform_Attribute_Ownership&>(msg);
        D.Out(pdInit, "m_REFLECT_ATTRIBUTE_VALUES Owner %u", msg.getFederate());

        owm->informAttributeOwnership(IAO.getObject(),
                                      IAO.getAttribute(),
                                      msg.getFederate(),
                                      msg.getRefException());
      }
        break ;

      case NetworkMessage::ATTRIBUTE_IS_NOT_OWNED: {
    	  NM_Attribute_Is_Not_Owned& AINO = static_cast<NM_Attribute_Is_Not_Owned&>(msg);
        owm->attributeIsNotOwned(AINO.getObject(),
                                 AINO.getAttribute(),
                                 msg.getFederate(),
                                 msg.getRefException());
      }
        break ;

      case NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
          {
        NM_Request_Attribute_Ownership_Assumption& RAOA=static_cast<NM_Request_Attribute_Ownership_Assumption&>(msg);
        owm->requestAttributeOwnershipAssumption(RAOA.getObject(),
                                                 RAOA.getAttributes(),
                                                 RAOA.getAttributesSize(),
                                                 msg.getFederate(),
                                                 msg.getLabel(),
                                                 msg.getRefException());
        break ;
           }

      case NetworkMessage::ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
        {
        NM_Attribute_Ownership_Unavailable& AOU = static_cast<NM_Attribute_Ownership_Unavailable&>(msg);
        owm->attributeOwnershipUnavailable(AOU.getObject(),
                                           AOU.getAttributes(),
                                           AOU.getAttributesSize(),
                                           msg.getFederate(),
                                           msg.getRefException());
        break ;
        }

      case NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
        {
        NM_Attribute_Ownership_Acquisition_Notification& AOAN = static_cast<NM_Attribute_Ownership_Acquisition_Notification&>(msg);
        owm->attributeOwnershipAcquisitionNotification(AOAN.getObject(),
                                                       AOAN.getAttributes(),
                                                       AOAN.getAttributesSize(),
                                                       msg.getFederate(),
                                                       msg.getRefException());
        break ;
        }

      case NetworkMessage::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
        {
        	NM_Attribute_Ownership_Divestiture_Notification& AODN = static_cast<NM_Attribute_Ownership_Divestiture_Notification&>(msg);
        owm->attributeOwnershipDivestitureNotification(AODN.getObject(),
                                                       AODN.getAttributes(),
                                                       AODN.getAttributesSize(),
                                                       msg.getRefException());
        break ;
        }

      case NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
        {
        	NM_Request_Attribute_Ownership_Release& RAOR = static_cast<NM_Request_Attribute_Ownership_Release&>(msg);
        owm->requestAttributeOwnershipRelease(RAOR.getObject(),
                                              RAOR.getAttributes(),
                                              RAOR.getAttributesSize(),
                                              msg.getLabel(),
                                              msg.getRefException());
        break ;
        }

      case NetworkMessage::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
        {
        NM_Confirm_Attribute_Ownership_Acquisition_Cancellation& CAOAC= static_cast<NM_Confirm_Attribute_Ownership_Acquisition_Cancellation& >(msg);
        owm->confirmAttributeOwnershipAcquisitionCancellation(CAOAC.getObject(),
                                                              CAOAC.getAttributes(),
                                                              CAOAC.getAttributesSize(),
                                                              msg.getRefException());
        break ;
        }

      case NetworkMessage::INITIATE_FEDERATE_SAVE:
        fm->initiateFederateSave(msg.getLabel());
        break ;

      case NetworkMessage::FEDERATION_SAVED:
      case NetworkMessage::FEDERATION_NOT_SAVED: {
          bool status = (msg.getMessageType() == NetworkMessage::FEDERATION_SAVED) ? true : false ;
          fm->federationSavedStatus(status);
      }
        break ;

      case NetworkMessage::REQUEST_FEDERATION_RESTORE_SUCCEEDED:
      case NetworkMessage::REQUEST_FEDERATION_RESTORE_FAILED: {
          bool status = (msg.getMessageType() == NetworkMessage::REQUEST_FEDERATION_RESTORE_SUCCEEDED)
              ? true : false ;
          fm->requestFederationRestoreStatus(status, msg.getLabel(), msg.getTag());
      }
        break ;

      case NetworkMessage::FEDERATION_RESTORE_BEGUN:
        fm->federationRestoreBegun();
        break ;

      case NetworkMessage::INITIATE_FEDERATE_RESTORE:
        fm->initiateFederateRestore(msg.getLabel(), msg.getFederate());
        break ;

      case NetworkMessage::FEDERATION_RESTORED:
      case NetworkMessage::FEDERATION_NOT_RESTORED: {
          bool status = (msg.getMessageType() == NetworkMessage::FEDERATION_RESTORED) ? true : false ;
          fm->federationRestoredStatus(status);
      }
        break ;
      case NetworkMessage::TIME_REGULATION_ENABLED:
        this->timeRegulationEnabled(msg.getDate(), msg.getRefException());
        break;
      case NetworkMessage::TIME_CONSTRAINED_ENABLED:
        this->timeConstrainedEnabled(msg.getDate(), msg.getRefException());
        break;
      case NetworkMessage::START_REGISTRATION_FOR_OBJECT_CLASS: {
    	  NM_Start_Registration_For_Object_Class& SRFOC = static_cast<NM_Start_Registration_For_Object_Class&>(msg);
	dm->startRegistrationForObjectClass(SRFOC.getObjectClass(), msg.getRefException());
      }
        break;

	  case NetworkMessage::RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED: 
		  {
			  NM_Reserve_Object_Instance_Name_Succeeded ROINS = static_cast<NM_Reserve_Object_Instance_Name_Succeeded&>(msg);
			  om->nameReservationSucceeded(ROINS.getObjectName());
		  }
		  break;
	  case NetworkMessage::RESERVE_OBJECT_INSTANCE_NAME_FAILED: 
		  {
			  NM_Reserve_Object_Instance_Name_Failed ROINF = static_cast<NM_Reserve_Object_Instance_Name_Failed&>(msg);
			  om->nameReservationFailed(ROINF.getObjectName());
		  }
		  break;

      default:
        D.Out(pdExcept, "Unknown message type in executeFederateService.");
        throw RTIinternalError(stringize() <<
            "Unknown message <" <<  msg.getMessageName() << "> in executeFederateService.");
    }
    G.Out(pdGendoc,"exit  TimeManagement::executeFederateService");
    return true ;
} /* executeFederateService */

// ----------------------------------------------------------------------------
void
TimeManagement::flushQueueRequest(FederationTime heure_logique,
                                  TypeException &e) {
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

    if (_is_constrained) {

        // Select lower value between expected time and first TSO message time.
        queues->nextTsoDate(TSOPresent, dateTSO);

        if ((TSOPresent) && (dateTSO < date_avancee))
            date_min = dateTSO ;
        else
            date_min = date_avancee ;

        Debug(D,pdDebug) << "TM::nextEventAdvance - date avancee="<< date_avancee.getTime()
        		<< " date min=" << date_min.getTime() << " LBTS = " << _LBTS.getTime() << std::endl;

        if ((date_min < _LBTS)||
        	((date_min <= _LBTS) && (_avancee_en_cours == NERA))
            ) {
            // nextEventRequest is done because either a TSO message
            // can be delivered or no message with lower value than
            // expected time is avail.

            // New expected time is keep (can be first TSO message to deliver).
            // This value must not be changed.
            date_avancee = date_min ;

            // If federate is regulating, inform other federate we advanced.
            if (_is_regulating)
                sendNullMessage(date_min);

            // Deliver to federate every TSO messages with time
            // 'date_min' (1 by 1).
            msg = queues->giveTsoMessage(date_min, msg_donne, msg_restant);
            if (msg_donne) {
            	Debug(D,pdDebug) << "TM::nextEventAdvance - MSG :" << msg->getMessageName() << std::endl;
                // Send message back to federate.
                executeFederateService(*msg);
                delete msg ;
            }
            else {
                // Advance current time up to 'date_min'.
            	Debug(D,pdDebug) << "TM::nextEventAdvance - TAG to" << date_min.getTime() << std::endl;
                timeAdvanceGrant(date_min, e);
                _avancee_en_cours = PAS_D_AVANCEE ;
            }
        }
        else { // date_min < _LBTS
            // Federate can't advance up to expected time but up to LBTS. Other
            // federates are informed and no TSO message are sent.
            if (_is_regulating) {
            	/* The following NULL message is part of the classical CMB NULL MESSAGE algorithm */
                sendNullMessage(_LBTS);
                /**
                 * The following NULL PRIME message is part of SN NULL MESSAGE PRIME algorithm
                 * This message is sent because we did receive some anonymous NULL message
                 * in the past and we are still NERing so RTIG may be interested (again)
                 * in our wanted advance time.
                 */
                if (hasReceivedAnonymousUpdate()) {
                	Debug(D,pdDebug) << "TM::nextEventAdvance - (re)sending NULL PRIME" << std::endl;
                	sendNullPrimeMessage(date_avancee);
                }
            }
        }
    }

    else { // if federate isn't constrained.

        // In this case, federate can advance freely. Moreover, there must be no
        // message in TSO list.
        if (_is_regulating)
            sendNullMessage(date_avancee);

        timeAdvanceGrant(date_avancee, e);

        _avancee_en_cours = PAS_D_AVANCEE ;
    }
G.Out(pdGendoc," exit  TimeManagement::nextEventAdvance");
} /* nextEventAdvance */

// ----------------------------------------------------------------------------
void
TimeManagement::nextEventRequest(FederationTime logicalTime,
                                 TypeException &e)
{
    e = e_NO_EXCEPTION ;

    // Verifications

    if (_avancee_en_cours != PAS_D_AVANCEE)
        e = e_TimeAdvanceAlreadyInProgress ;

    if (logicalTime < _heure_courante)
        e = e_FederationTimeAlreadyPassed ;

//    This is check may be overkill because
//    if we consider that advancing in time is NOT a timestamped event
//    see bug #25497 : https://savannah.nongnu.org/bugs/?25497
//    if (heure_logique < _heure_courante + _lookahead_courant)
//       e = e_InvalidFederationTime ;

    if (e == e_NO_EXCEPTION) {

        _type_granted_state = AFTER_TAR_OR_NER ;  // will be

        if (_lookahead_courant == 0.0) {
           _lookahead_courant = epsilon2 ;
           _type_granted_state = AFTER_TAR_OR_NER_WITH_ZERO_LK ;
           D.Out(pdDebug,"NER: with ZERO LK, lk=%f",_lookahead_courant.getTime());
        }

        _avancee_en_cours = NER ;
        date_avancee = logicalTime ;
        sendNullPrimeMessage(logicalTime);
        D.Out(pdTrace, "NextEventRequest accepted (lk=%f,date_avance=%f.)",_lookahead_courant.getTime(),date_avancee.getTime());
    }
    else {
        D.Out(pdExcept, "NextEventRequest refused (exception = %d).", e);
    }
} /* end of nextEventRequest */

// ----------------------------------------------------------------------------
void
TimeManagement::nextEventRequestAvailable(FederationTime heure_logique,
                                 TypeException &e)
{
    e = e_NO_EXCEPTION ;

    // Verifications

    if (_avancee_en_cours != PAS_D_AVANCEE)
        e = e_TimeAdvanceAlreadyInProgress ;

    if (heure_logique < _heure_courante)
        e = e_FederationTimeAlreadyPassed ;

//    This is check may be overkill because
//    if we consider that advancing in time is NOT a timestamped event
//    see bug #25497 : https://savannah.nongnu.org/bugs/?25497
//    if (heure_logique < _heure_courante + _lookahead_courant)
//       e = e_InvalidFederationTime ;

    if (e == e_NO_EXCEPTION) {
        _type_granted_state = AFTER_TARA_OR_NERA ;  // will be
        _avancee_en_cours = NERA ;
        date_avancee = heure_logique;
        sendNullPrimeMessage(heure_logique);
        D.Out(pdTrace, "NextEventRequestAvailable accepted.");
    }
    else {
        D.Out(pdExcept, "NextEventRequestAvailable refused (exception = %d).", e);
    }
} /* end of nextEventRequestAvailable */

// ----------------------------------------------------------------------------
FederationTime
TimeManagement::requestFederationTime()
{
    return _LBTS ;
}

// ----------------------------------------------------------------------------
FederationTimeDelta TimeManagement::requestLookahead()
{
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

   D.Out(pdRegister, "Minimum Next Event Time : %f.", dateMNET.getTime());

   return dateMNET ;
}

// ----------------------------------------------------------------------------
void
TimeManagement::setLookahead(FederationTimeDelta lookahead, TypeException &e)
{
    e = e_NO_EXCEPTION ;

    // Verifications

    if (lookahead < 0.0)
        e = e_InvalidLookahead ;

    if (lookahead == epsilon2) {
    	Debug(D,pdError) << "Bad value of lookahead due to a zero lookahead implementation trick" << std::endl;
    	e = e_RTIinternalError ;
    }

    if (e == e_NO_EXCEPTION) {
        _lookahead_courant = lookahead ;

        // On previent les autres en leur envoyant un message NULL qui contient
        // notre temps local + le Lookahead.
        if (_is_regulating)
            sendNullMessage(_heure_courante);

        D.Out(pdRegister, "New Lookahead : %f.", _lookahead_courant.getTime());
    }
} /* end of setLookahead */

// ----------------------------------------------------------------------------
void
TimeManagement::setTimeConstrained(bool etat, TypeException &e)
{
    NM_Set_Time_Constrained msg ;

    e = e_NO_EXCEPTION ;

    // Verifications

    if (_is_constrained == etat)
        e = e_RTIinternalError ;

    if (_avancee_en_cours != PAS_D_AVANCEE)
        e = e_RTIinternalError ;

    if (e == e_NO_EXCEPTION) {
        _is_constrained = etat ;

        msg.setFederation(fm->_numero_federation);
        msg.setFederate(fm->federate);
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
// Rajouter le time et le lookahead
// Modifier le lookahead courant
void
TimeManagement::setTimeRegulating(bool etat,FederationTime heure_logique,
               FederationTimeDelta the_lookahead, TypeException &e)
{
    NM_Set_Time_Regulating msg ;

    e = e_NO_EXCEPTION ;

    // Verifications

    if (_is_regulating == etat) {
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
        _is_regulating = etat ;

        msg.setFederation(fm->_numero_federation);
        msg.setFederate(fm->federate);
        if (etat) {
        	msg.regulatorOn();
        	D.Out(pdDebug,
        		  "REGULATOR ON");
        } else {
        	msg.regulatorOff();
        	D.Out(pdDebug,
        	      "REGULATOR OFF");
        }
    // Modifier lookahead courant
    _lookahead_courant = the_lookahead;
          D.Out(pdDebug,
    	        "New lookahead = %f",_lookahead_courant.getTime());
    // faudrait peut etre remplacer heure courante par le temps en parametre
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
void
TimeManagement::timeRegulationEnabled(FederationTime theTime, TypeException &e) {
    M_Time_Regulation_Enabled req;

    D.Out(pdDebug,"Sending TIME_REGULATION_ENABLED to Federate");
    req.setDate(theTime);
    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
void
TimeManagement::timeConstrainedEnabled(FederationTime theTime, TypeException &e) {
    M_Time_Constrained_Enabled req;

    D.Out(pdDebug,"Sending TIME_CONSTRAINED_ENABLED to Federate");
    req.setDate(theTime);
    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
/*! Is the time stamp of a time advance request correct ?
*/
bool
TimeManagement::testValidTime(FederationTime theTime)
{
   if (_avancee_en_cours == PAS_D_AVANCEE) {
      if (_type_granted_state == AFTER_TAR_OR_NER_WITH_ZERO_LK) {
         if (theTime <= _heure_courante)
            return false;
      }
      else {  // AFTER_TAR_OR_NER or AFTER_TARA_OR_NARA
         if (theTime  < _heure_courante + _lookahead_courant)
            return false;
      }
   }
   else {
      if (_type_granted_state == AFTER_TAR_OR_NER_WITH_ZERO_LK) {
         if (theTime <= date_avancee)
            return false;
      }
      else {  // AFTER_TAR_OR_NER or AFTER_TARA_OR_NARA
         if (theTime  < date_avancee + _lookahead_courant)
            return false;
      }
   }
   return true;
}

// ----------------------------------------------------------------------------
bool
TimeManagement::tick(TypeException &e)
{
	// Becomes true if there is message to give back to the federate
    bool oneMsgToHandle   = false;
    // When oneMsgToHandle==true, then msg_restant becomes true if there is more message to handle
    bool moreMsgToHandle = false;

    NetworkMessage *msg = NULL ;

    G.Out(pdGendoc," enter TimeManagement::tick");
    // Note: While msg_donne = false, msg_restant doesn't matter.

    // 1st try, give a command message. (requestPause, etc.)
    msg = queues->giveCommandMessage(oneMsgToHandle, moreMsgToHandle);

    // 2nd try, give a FIFO message. (discoverObject, etc.)
    if (!oneMsgToHandle) {
        if ( _asynchronous_delivery              ||
        	(_avancee_en_cours != PAS_D_AVANCEE) ||
        	(! _is_constrained)) {
        	D.Out(pdDebug,"FIFO message to be delivered async_deliver=%d, _avancee=%d, constrained=%d",
        	        		  _asynchronous_delivery,_avancee_en_cours,_is_constrained);
          // to exclude the case not asynchronous_delivery and
          // not time advancing for a constrained federate
          msg = queues->giveFifoMessage(oneMsgToHandle, moreMsgToHandle);
        }
        else {
          D.Out(pdDebug,"FIFO message skipped async_deliver=%d, _avancee=%d, constrained=%d",
        		  _asynchronous_delivery,_avancee_en_cours,_is_constrained);
        }
    }

    // If message exists, send it to the federate.
    if (oneMsgToHandle) {
        D.Out(pdDebug, "TickRequest being processed, Message to send.");
        try {
            executeFederateService(*msg);
        }
        catch (RTIinternalError &e) {
        	Debug(D,pdError) << "RTIA:RTIinternalError thrown in tick (execute)." << std::endl ;
            throw e ;
        }
    }
    // No message: we try to send TSO messages.
    // Messages to be sent depends on asked advance type.
    else {
        D.Out(pdDebug, "TickRequest being processed, advance called.");
        try {
            advance(moreMsgToHandle, e);
        }
        catch (RTIinternalError &e) {
        	Debug(D,pdError) << "RTIA:RTIinternalError thrown in tick (Advance)." << std::endl ;
            throw e ;
        }
    }

    delete msg ;

    G.Out(pdGendoc," exit  TimeManagement::tick");
    return moreMsgToHandle;
} /* end of tick */

// ----------------------------------------------------------------------------
void
TimeManagement::timeAdvance(bool &msg_restant, TypeException &e)
{
    bool msg_donne ;
    FederationTime min ;
    NetworkMessage *msg ;
    G.Out(pdGendoc," enter TimeManagement::timeAdvance");
    msg_restant = false ;

    if (_is_constrained) {
        // give a TSO message.
        if (_LBTS.isPositiveInfinity())
           D.Out(pdDebug, "Logical time : %f, LBTS : infini.", date_avancee.getTime());
        else
           D.Out(pdDebug, "Logical time : %f, LBTS : %lf.", date_avancee.getTime(), _LBTS.getTime());
        min = (_LBTS<date_avancee)?(_LBTS):(date_avancee);
        msg = queues->giveTsoMessage(min, msg_donne, msg_restant);

        // otherwise
        if (!msg_donne) {
            // if LBTS allows to give a timeAdvanceGrant.
            if (_LBTS.isPositiveInfinity())
               D.Out(pdDebug, "Logical time : %f, LBTS : infini, lookahead : %f.",
                     date_avancee.getTime(), _lookahead_courant.getTime());
            else
               D.Out(pdDebug, "Logical time : %15.12f, LBTS : %15.12f, lookahead : %f.",
                     date_avancee.getTime(), _LBTS.getTime(), _lookahead_courant.getTime());

            if ((date_avancee < _LBTS) ||
               ((date_avancee == _LBTS) && (_avancee_en_cours == TARA))) {
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
        // if federate is not constrained, send a timeAdvanceGrant to federate.
        timeAdvanceGrant(date_avancee, e);
        if (e != e_NO_EXCEPTION)
            return ;
        _avancee_en_cours = PAS_D_AVANCEE ;
    }
G.Out(pdGendoc," exit  TimeManagement::timeAdvance");
}

// ----------------------------------------------------------------------------
void
TimeManagement::timeAdvanceGrant(FederationTime logical_time,
                                 TypeException &e)
{
    M_Time_Advance_Grant req;

    req.setDate(logical_time);

    D.Out(pdRegister, "timeAdvanceGrant sent to federate (time = %f).",
          req.getDate().getTime());

    if (_lookahead_courant == epsilon2)
        _lookahead_courant = 0.0 ;

    _tick_state = TICK_NEXT;  // indicate the callback was processed

    comm->requestFederateService(&req);

    _heure_courante = logical_time ;
    /* reset the sending of NULL PRIME message whenever we get TAG */
    resetAnonymousUpdate();
} /* timeAdvanceGrant */

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

    if (logical_time < _heure_courante)
        e = e_FederationTimeAlreadyPassed ;

    //    This is check may be overkill because
    //    if we consider that advancing in time is NOT a timestamped event
    //    see bug #25497 : https://savannah.nongnu.org/bugs/?25497
//    if (logical_time < _heure_courante + _lookahead_courant) {
//
//    D.Out(pdDebug,"InvalidFederation time lkahead=%f, current=%f, requested=%f",
//    			_lookahead_courant.getTime(),_heure_courante.getTime(),logical_time.getTime());
//       e = e_InvalidFederationTime ;
//    }

    if (e == e_NO_EXCEPTION) {

        _type_granted_state = AFTER_TAR_OR_NER ;  // will be

        if (_lookahead_courant == 0.0) {
           _lookahead_courant = epsilon2 ;
           _type_granted_state = AFTER_TAR_OR_NER_WITH_ZERO_LK ;
        }

        if (_is_regulating) {
            sendNullMessage(logical_time);
        }

        _avancee_en_cours = TAR ;
        date_avancee = logical_time ;

        D.Out(pdTrace, "timeAdvanceRequest accepted (asked time=%f).",
              date_avancee.getTime());
    }
    else {
        D.Out(pdExcept, "timeAdvanceRequest refused (exception = %d).", e);
    }
}


void
TimeManagement::timeAdvanceRequestAvailable(FederationTime logical_time,
                                   TypeException &e)
{
    e = e_NO_EXCEPTION ;

    // Verifications

    if (_avancee_en_cours != PAS_D_AVANCEE)
        e = e_TimeAdvanceAlreadyInProgress ;

    if (logical_time < _heure_courante)
        e = e_FederationTimeAlreadyPassed ;

    //    This is check may be overkill because
    //    if we consider that advancing in time is NOT a timestamped event
    //    see bug #25497 : https://savannah.nongnu.org/bugs/?25497
//    if (logical_time < _heure_courante + _lookahead_courant)
//       e = e_InvalidFederationTime ;

    if (e == e_NO_EXCEPTION) {

        _type_granted_state = AFTER_TARA_OR_NERA ;  // will be

        if (_is_regulating) {
            sendNullMessage(logical_time);
        }

        _avancee_en_cours = TARA ;
        date_avancee = logical_time ;

        D.Out(pdTrace, "timeAdvanceRequestAvailable accepted (asked time=%f).",
              date_avancee.getTime());
    }
    else {
        D.Out(pdExcept, "timeAdvanceRequestAvailable refused (exception = %d).", e);
    }
}


}} // namespaces

// $Id: TimeManagement.cc,v 3.70 2011/01/11 12:29:40 erk Exp $
