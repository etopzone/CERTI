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
// $Id: TimeManagement.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#include "TimeManagement.hh"

namespace certi {
namespace rtia {

static pdCDebug D("RTIA_GT", "(RTIA GT ) - ");

// -------------------------
// -- AvanceeTempsEnCours --
// -------------------------

void TimeManagement::advance(Boolean &msg_restant,
			     TypeException &e)
{
  switch(_avancee_en_cours)
    {
    case TAR:
      D.Out(pdTrace, "Appel de TimeAdvanceEnCours.");
      timeAdvance(msg_restant, e);
      break;
 
    case NER:
      D.Out(pdTrace, "Appel de NextEventEnCours.");
      nextEventAdvance(msg_restant, e);
      break;
 
    default:
      D.Out(pdTrace, 
	    "Cas non prevu dans AvanceeTempsEnCours: %d.",
	    _avancee_en_cours);
      // On ne leve pas d'exception, ca peut etre un cas ou on a
      // rien a faire, par exemple en cas d'attente active pendant
      // une pause.
    }
}


// -------------------
// -- TimeManagement --
// -------------------

TimeManagement::TimeManagement(Communications *GC,
			       Queues *GQueues,
			       FederationManagement *GF,
			       ObjectManagement *GO,
			       OwnershipManagement *GP )
  : LBTS()
{
  _GC = GC;
  _GQueues = GQueues;
  _GF = GF;
  _GO = GO;
  _GP = GP;

  lastNullMessageDate = 0.0;

  _avancee_en_cours = PAS_D_AVANCEE;

  _heure_courante = 0.0;
  _lookahead_courant = EPSILON;
  _est_regulateur = RTI_FALSE;
  _est_contraint = RTI_FALSE;

}



// -----------------------
// -- EmettreMessageNul --
// -----------------------

void TimeManagement::sendNullMessage(FederationTime heure_logique)
{
  NetworkMessage msg;

  msg.Date = heure_logique;
  heure_logique += _lookahead_courant;

  if(heure_logique > lastNullMessageDate)
    {
      msg.Type = m_MESSAGE_NULL;
      msg.NumeroFederation = _GF->_numero_federation;
      msg.NumeroFedere = _GF->federate;
      msg.Date = heure_logique;

      _GC->sendMessage(&msg);
      lastNullMessageDate = heure_logique;
      D.Out(pdDebug, "Message NULL emis.");
    }
  else {
    D.Out(pdExcept, "Message nul non emis(Heure = %f, Dernier = %f).",
	  heure_logique, lastNullMessageDate);
  }
}


// ---------------------------
// -- ExecuterServiceFedere --
// ---------------------------

Boolean TimeManagement::executeFederateService(NetworkMessage &msg)
{

  D.Out(pdRequest, "Executer Service Federe : Type %d.", msg.Type);

  switch(msg.Type) {
 
  case m_REQUEST_PAUSE:
    _GF->initiatePause(msg.Label);
    break;
 
  case m_REQUEST_RESUME:
    _GF->initiateResume(msg.Label);
    break;
 
  case m_DISCOVER_OBJECT:
    try {
      _GO->discoverObject(msg.Objectid, 
			  msg.objectClassHandle,
			  msg.Label,
			  msg.Date,
			  msg.Retract, 
			  msg.Exception);
 
    }
    catch(RTIinternalError &e) {
      printf("RTIA:RTIinternalError in discoverObject.\n");
      throw e;
    }
    break;
 
  case m_REFLECT_ATTRIBUTE_VALUES:
    {
      AttributeValue *ValueArray = msg.getAttribValueArray();
 
      _GO->reflectAttributeValues(msg.Objectid, 
				  msg.HandleArray,
				  ValueArray,
				  msg.HandleArraySize, 
				  msg.Date,
				  msg.Label,
				  msg.Retract,
				  msg.Exception);
      free(ValueArray);
      break;
    }
 
  case m_RECEIVE_INTERACTION: 
    {
      ParameterValue *ValueArray = msg.getParamValueArray();

      _GO->receiveInteraction(msg.InteractionHandle,
			      msg.HandleArray,
			      ValueArray,
			      msg.HandleArraySize,
			      msg.Date,
			      msg.Label,
			      msg.Retract,
			      msg.Exception);
      free(ValueArray);

      break;
    }
 
  case m_REMOVE_OBJECT:
    _GO->removeObject(msg.Objectid,
		      msg.NumeroFedere,
		      msg.Label,
		      msg.Retract,
		      msg.Exception);
    break;
 
  case m_INFORM_ATTRIBUTE_OWNERSHIP:
 
    D.Out(pdInit,"m_REFLECT_ATTRIBUTE_VALUES Owner %u",msg.NumeroFedere);
 
    _GP->informAttributeOwnership(msg.Objectid,
				  msg.HandleArray[0], 
				  msg.NumeroFedere,
				  msg.Exception);
    break;
 
  case m_ATTRIBUTE_IS_NOT_OWNED:
    _GP->attributeIsNotOwned(msg.Objectid,
			     msg.HandleArray[0], 
			     msg.NumeroFedere,
			     msg.Exception);
    break;
 
  case m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
    _GP->requestAttributeOwnershipAssumption(msg.Objectid,
					     msg.HandleArray,
					     msg.HandleArraySize, 
					     msg.NumeroFedere,
					     msg.Label, 
					     msg.Exception);
    break; 
 
  case m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
    _GP->attributeOwnershipUnavailable(msg.Objectid,
				       msg.HandleArray,
				       msg.HandleArraySize, 
				       msg.NumeroFedere,
				       msg.Exception);
    break;
 
  case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
    _GP->attributeOwnershipAcquisitionNotification(msg.Objectid,
						   msg.HandleArray,
						   msg.HandleArraySize, 
						   msg.NumeroFedere,
						   msg.Exception);
    break;
 
  case m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
    _GP->attributeOwnershipDivestitureNotification(msg.Objectid,
						   msg.HandleArray,
						   msg.HandleArraySize, 
						   msg.Exception);
    break;
 
  case m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
    _GP->requestAttributeOwnershipRelease(msg.Objectid,
					  msg.HandleArray,
					  msg.HandleArraySize, 
					  msg.Label, 
					  msg.Exception);
    break; 
 
  case m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
    _GP->confirmAttributeOwnershipAcquisitionCancellation(msg.Objectid,
							  msg.HandleArray,
							  msg.HandleArraySize, 
							  msg.Exception);
    break; 
 
  default:
    D.Out(pdExcept, 
	  "Type de Message inconnu dans ExecuterServiceFedere.");
    msg.display("ERREUR");
    throw RTIinternalError("Message inconnu dans ExecuterServiceFed.");

  }
 
  return RTI_TRUE;
 
}


// -----------------------
// -- flushQueueRequest --
// -----------------------

void TimeManagement::
flushQueueRequest(FederationTime heure_logique,
		  TypeException &e)
{
  e = e_NO_EXCEPTION;

  // Verifications

  if(_avancee_en_cours != PAS_D_AVANCEE)
    e = e_TimeAdvanceAlreadyInProgress;

  if(heure_logique <= _heure_courante)
    e = e_FederationTimeAlreadyPassed;

  if(e == e_NO_EXCEPTION)
    {
      // BUG: Non implementee.
      D.Out(pdExcept, "flushQueueRequest non implementee.");
      throw RTIinternalError("flushQueueRequest non implementee.");
    }
}


// ----------------------
// -- NextEventEnCours --
// ----------------------

void TimeManagement::nextEventAdvance(Boolean &msg_restant,
				      TypeException &e)
{
  FederationTime dateTSO;
  FederationTime date_min = 0.0;
  Boolean TSOPresent;
  Boolean msg_donne;
  NetworkMessage *msg;

  msg_restant = RTI_FALSE;

  // Si le federe est contraint
  if(_est_contraint) {
 
    // Calculer le minimum entre la date demandee par le federe
    // et la date du premier message TSO

    _GQueues->nextTsoDate(TSOPresent, dateTSO);
 
    if((TSOPresent) &&(dateTSO < date_avancee))
      date_min = dateTSO;
    else
      date_min = date_avancee;
 
    if(date_min < _LBTS) {

      // Le nextEventRequest est termine car soit on peut donner un message
      // TSO, soit on n'en a aucun de date inferieure a la date demandee.

      // On memorise la nouvelle date objectif(qui peut etre celle du
      // premier message TSO a donner, mais qui ne doit plus changer).
      date_avancee = date_min;

      // Si on est regulateur, prevenir les autres de notre avancee.
      if(_est_regulateur)
	sendNullMessage(date_min);

      // Donner tous les messages TSO de date 'date_min' au
      // federe, 1 par 1.

      msg = _GQueues->giveTsoMessage(date_min, msg_donne, msg_restant);
      if(msg_donne)
	// Renvoie le message au federe.
	executeFederateService(*msg);
      else {
	// Avance l'heure courante jusqu'a 'date_min'.
	timeAdvanceGrant(date_min, e);
	_avancee_en_cours = PAS_D_AVANCEE;
      }
    }
    else { // date_min < _LBTS
 
      // On ne peut pas avancer jusqu'a la date requise, mais au
      // moins jusqu'au LBTS. On previent les autres, sans
      // distribuer de msg. TSO.
      if(_est_regulateur)
	sendNullMessage(_LBTS);
    }
  }

  else { // si federe non contraint

    // N'etant pas contraint, le federe peut toujours avancer a sa guise.
    // De plus il ne doit pas stocker de messages dans la file TSO.
    if(_est_regulateur)
      sendNullMessage(date_avancee);

    timeAdvanceGrant(date_avancee, e);

    _avancee_en_cours = PAS_D_AVANCEE;
  }

}


// ----------------------
// -- nextEventRequest --
// ----------------------

void TimeManagement::nextEventRequest(FederationTime heure_logique,
				      TypeException &e)
{
  e = e_NO_EXCEPTION;

  // Verifications

  if(_avancee_en_cours != PAS_D_AVANCEE)
    e = e_TimeAdvanceAlreadyInProgress;

  if(heure_logique <= _heure_courante)
    e = e_FederationTimeAlreadyPassed;


  if(e == e_NO_EXCEPTION) {
    _avancee_en_cours = NER;
    date_avancee = heure_logique;
    D.Out(pdTrace, "NextEventRequest acceptee.");
  }
  else {
    D.Out(pdExcept, "NextEventRequest refusee(exception = %d).", e);
  }

}


// ---------------------------
// -- requestFederationTime --
// ---------------------------

FederationTime TimeManagement::requestFederationTime()
{
  if(_heure_courante < _LBTS)
    return _heure_courante;
  else
    return _LBTS;
}


// ----------------------
// -- requestLookahead --
// ----------------------

FederationTimeDelta TimeManagement::requestLookahead()
{
  // BUG: C'est quoi cette salade ?

  if(_heure_courante + _lookahead_courant < lastNullMessageDate)
    return(lastNullMessageDate - _heure_courante);
  else
    return _lookahead_courant;
}


// ------------------
// -- setLookahead --
// ------------------

void TimeManagement::setLookahead(FederationTimeDelta lookahead,
				  TypeException &e)
{
  e = e_NO_EXCEPTION;

  // Verifications

  if(lookahead <= 0.0)
    e = e_InvalidFederationTimeDelta;


  if(e == e_NO_EXCEPTION) {
    _lookahead_courant = lookahead;

    // On previent les autres en leur envoyant un message nul qui contient
    // notre temps local + le Lookahead.
    if(_est_regulateur)
      sendNullMessage(_heure_courante);

    D.Out(pdRegister, "Nouveau Lookahead : %f.", _lookahead_courant);
  }
 
}


// ------------------------
// -- setTimeConstrained --
// ------------------------

void TimeManagement::setTimeConstrained(Boolean etat, TypeException &e)
{ 
  NetworkMessage msg;

  e = e_NO_EXCEPTION;

  // Verifications

  if(_est_contraint == etat)
    e = e_RTIinternalError;

  if(_avancee_en_cours != PAS_D_AVANCEE)
    e = e_RTIinternalError;


  if(e == e_NO_EXCEPTION) {
    _est_contraint = etat;
 
    msg.Type = m_SET_TIME_CONSTRAINED;
    msg.NumeroFederation = _GF->_numero_federation;
    msg.NumeroFedere = _GF->federate;
    msg.EstContraint = etat;

    _GC->sendMessage(&msg);

    D.Out(pdRegister, 
	  "Demande de modif de TimeConstrained envoyee(etat=%d,", etat);
  }
  else {
    D.Out(pdExcept, "SetTimeConstrained refuse(exception = %d).", e);
  }
}


// -----------------------
// -- setTimeRegulating --
// -----------------------

void TimeManagement::setTimeRegulating(Boolean etat, TypeException &e)
{
  NetworkMessage msg;
 
  e = e_NO_EXCEPTION;

  // Verifications

  if(_est_regulateur == etat) {
    e = e_RTIinternalError;
    D.Out(pdRegister, 
	  "erreur e_RTIinternalError : federe deja regulateur");
  }

  if(_avancee_en_cours != PAS_D_AVANCEE) {
    e = e_RTIinternalError;
    D.Out(pdRegister,"erreur e_RTIinternalError avancee_en_cours");
  }
 
  // Si il existe d'autres federes regulateurs plus avances
  // if((etat == RTI_TRUE) &&
  //(lg > 0) && 
  //(_heure_courante +_lookahead_courant < _LBTS)) {
  // e = e_FederationTimeAlreadyPassed;
  // D.Out(pdRegister,"erreur e_FederationTimeAlreadyPassed ");
  // }

  // Si aucune erreur, prevenir le RTIG et devenir regulateur.
  if(e == e_NO_EXCEPTION)
    {
      _est_regulateur = etat;

      msg.Type = m_SET_TIME_REGULATING;
      msg.NumeroFederation = _GF->_numero_federation;
      msg.NumeroFedere = _GF->federate;
      msg.EstRegulateur = etat;
      msg.Date = _heure_courante + _lookahead_courant;

      _GC->sendMessage(&msg);

      D.Out(pdRegister, 
	    "Demande de modif de TimeRegulating emise(etat=%d).", etat);
    }
  else {
    D.Out(pdExcept, "SetTimeRegulating refuse(exception = %d).", e);
  }
}

// ----------
// -- tick --
// ----------

Boolean TimeManagement::tick(TypeException &e)
{
  Boolean msg_donne = RTI_FALSE;
  Boolean msg_restant = RTI_FALSE;
  NetworkMessage *msg = NULL;
 
  // Note: Tant que msg_donne = RTI_FALSE, msg_restant n'est pas
  // significatif.

  // 1er essai, donner un message de commande.(requestPause, etc.)
  msg = _GQueues->giveCommandMessage(msg_donne, msg_restant);


  // 2eme essai, donner un message FIFO.(discoverObject, etc.)
  if(msg_donne == RTI_FALSE)
    msg = _GQueues->giveFifoMessage(msg_donne, msg_restant);
 
  // Si on a finalement recupere quelquechose, l'envoyer au federe.
  if(msg_donne == RTI_TRUE) {
    D.Out(pdDebug, "TickRequest en traitement, Message a envoyer.");
    try {
      executeFederateService(*msg);
    }
    catch(RTIinternalError &e) {
      printf("RTIA:RTIinternalError thrown in tick(Executer).\n");
      throw e;
    }
  }
 
  // Si ca ne donne toujours rien, on essaye d'envoyer les messages TSO.
  // Les messages a envoyer dependent du type d'avancee demandee.
  else {
    D.Out(pdDebug, "TickRequest en traitement, AvanceeTemps appele.");
    try {
      advance(msg_restant, e);
    }
    catch(RTIinternalError &e) {
      printf("RTIA:RTIinternalError thrown in tick(Avance).\n");
      throw e;
    }
  }
 
  delete msg;

  return msg_restant;
}


// ------------------------
// -- TimeAdvanceEnCours --
// ------------------------

void TimeManagement::timeAdvance(Boolean &msg_restant,
				 TypeException &e)
{
  Boolean msg_donne;
  FederationTime min;
  NetworkMessage *msg;

  msg_restant = RTI_FALSE;

  // si federe contraint
  if(_est_contraint)
    {
      // donner un message TSO si possible
      D.Out(pdDebug, "Temps local : %f, LBTS : %f.", date_avancee, _LBTS);
      min =(_LBTS<date_avancee)?(_LBTS):(date_avancee) ;
      msg = _GQueues->giveTsoMessage(min, msg_donne, msg_restant);

      // si pas de message TSO a donner
      if(!msg_donne)
	{
	  // si le LBTS autorise de donner un timeAdvanceGrant
	  D.Out(pdDebug, "Temps local : %f, LBTS : %f, lookahead : %f.", 
		date_avancee, _LBTS,_lookahead_courant);
	  if(date_avancee < _LBTS)
	    {
	      // donner un timeAdvanceGrant au federe
	      timeAdvanceGrant(date_avancee, e);

	      if(e != e_NO_EXCEPTION)
		return;

	      _avancee_en_cours = PAS_D_AVANCEE;
	    }
	  // sinon ne rien donner au federe(tick vide)
	}
      else
	{
	  executeFederateService(*msg);
	}
    }
  else // si federe non contraint
    {
      // donner au federe un timeAdvanceGrant
      timeAdvanceGrant(date_avancee, e);
      if(e != e_NO_EXCEPTION)
	return;
      _avancee_en_cours = PAS_D_AVANCEE;
    }
}


// ----------------------
// -- timeAdvanceGrant --
// ----------------------

void TimeManagement::timeAdvanceGrant(FederationTime heure_logique,
				      TypeException &e)
{
  Message req, rep;

  req.Type = TIME_ADVANCE_GRANT;
  req.Date = heure_logique;

  D.Out(pdRegister, 
	"TimeAdvanceGrant envoye au federe(heure = %f).", req.Date);

  _GC->requestFederateService(&req, &rep);

  e = rep.Exception;

  if(e == e_NO_EXCEPTION)
    _heure_courante = heure_logique;
}


// ------------------------
// -- timeAdvanceRequest --
// ------------------------

void TimeManagement::timeAdvanceRequest(FederationTime heure_logique,
					TypeException &e)
{
  e = e_NO_EXCEPTION;

  // Verifications

  if(_avancee_en_cours != PAS_D_AVANCEE)
    e = e_TimeAdvanceAlreadyInProgress;

  if(heure_logique <= _heure_courante)
    e = e_FederationTimeAlreadyPassed;


  if(e == e_NO_EXCEPTION) {
    if(_est_regulateur)
      sendNullMessage(heure_logique);
 
    _avancee_en_cours = TAR;
    date_avancee = heure_logique;

    D.Out(pdTrace, 
	  "TimeAdvanceRequest(heure demandee=%f) acceptee.", date_avancee);
  }
  else {
    D.Out(pdExcept, "TimeAdvanceRequest refusee(exception = %d).", e);
  }
}
}
}

// $Id: TimeManagement.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
