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
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIG.hh"
#include "NM_Classes.hh"

#include <iostream>
#include <cassert>
#include <cstring>
#include <memory>

using std::endl ;
using std::cout ;

namespace certi {
namespace rtig {

static PrettyDebug D("RTIG", __FILE__);
static PrettyDebug G("GENDOC",__FILE__);
static PrettyDebug DNULL("RTIG_NULLMSG","[RTIG NULL MSG]");

// ----------------------------------------------------------------------------
//! Creates a new federation.
void
RTIG::processCreateFederation(Socket *link, NM_Create_Federation_Execution *req)
{
	std::string federation = req->getFederationName();
	std::string FEDid      = req->getFEDid();
	NM_Create_Federation_Execution  rep;   // rep for answer to RTIA

	G.Out(pdGendoc,"enter RTIG::processCreateFederation");
	G.Out(pdGendoc,"BEGIN ** CREATE FEDERATION SERVICE **");

	if (federation.length() == 0)
	{
		G.Out(pdGendoc,"exit  RTIG::processCreateFederation on exception RTIinternalError");
		throw RTIinternalError("Invalid Federation Name.");
	}

	auditServer << "Federation Name : " << federation;
	Handle h = federationHandles.provide();

#ifdef FEDERATION_USES_MULTICAST
	// multicast base address
	unsigned long base_adr_mc = inet_addr(ADRESSE_MULTICAST);
	SocketMC *com_mc = NULL ;

	// creer la communication multicast
	com_mc = new SocketMC();
	if (com_mc == NULL) {
		D.Out(pdExcept, "Unable to allocate Multicast socket.");
		G.Out(pdGendoc,"exit  RTIG::processCreateFederation on exception RTIinternalError")
        								throw RTIinternalError("Unable to allocate Multicast socket.");
	}

	com_mc->CreerSocketMC(base_adr_mc + h, MC_PORT);

	// inserer la nouvelle federation dans la liste des federations
	federations->createFederation(federation, h, com_mc);

	// inserer descripteur fichier pour le prochain appel a un select
	ClientSockets.push_front(com_mc);

#else
	// We catch createFederation because it is useful to send
	// exception reason to RTIA
	try {
		federations.createFederation(federation, h, FEDid);
	}
	catch (CouldNotOpenFED& e)
	{
		rep.setException(e_CouldNotOpenFED,e._reason);
	}
	catch (ErrorReadingFED& e)
	{
		rep.setException(e_ErrorReadingFED,e._reason);
	}
	catch (FederationExecutionAlreadyExists& e)
	{
		rep.setException(e_FederationExecutionAlreadyExists,e._reason);
	}
#endif
	// Prepare answer for RTIA : store NetworkMessage rep
	if ( rep.getException() == e_NO_EXCEPTION )
	{
		rep.setFederation(h);
		rep.setFEDid(FEDid);
		rep.setFederationName(federation);
		auditServer <<" created.";

	}

	G.Out(pdGendoc,"processCreateFederation===>write answer to RTIA");

	rep.send(link,NM_msgBufSend); // Send answer to RTIA

	D.Out(pdInit, "Federation \"%s\" created with Handle %d.",
			federation.c_str(), rep.getFederation());

	G.Out(pdGendoc,"END ** CREATE FEDERATION SERVICE **");
	G.Out(pdGendoc,"exit RTIG::processCreateFederation");
}

// ----------------------------------------------------------------------------
//! Add a new federate to federation.
void
RTIG::processJoinFederation(Socket *link, NM_Join_Federation_Execution *req)
{
	std::string federation = req->getFederationName();
	std::string federate   = req->getFederateName();

	unsigned int peer     = req->getBestEffortPeer();
	unsigned long address = req->getBestEffortAddress();

	FederateHandle num_federe ;

	int nb_regulateurs ;
	int nb_federes ;
	bool pause ;

	G.Out(pdGendoc,"BEGIN ** JOIN FEDERATION SERVICE **");
	G.Out(pdGendoc,"enter RTIG::processJoinFederation");

	if (federation.empty() || federate.empty())
		throw RTIinternalError("Invalid Federation/Federate Name.");

	auditServer << "Federate \"" << federate << "\" joins Federation \""
			<< federation << "\"" ;

	Handle num_federation = federations.getFederationHandle(federation);

	// Need to dump the FOM into that
	NM_Join_Federation_Execution rep ;
	try
	{
		num_federe = federations.addFederate(num_federation,
				federate,
				static_cast<SocketTCP*>(link),
				rep);
	}
	catch (FederateAlreadyExecutionMember &e)
	{
		G.Out(pdGendoc,"exit RTIG::processJoinFederation on Error");
		G.Out(pdGendoc,"END ** JOIN FEDERATION (BAD) SERVICE **");
		// Prepare answer about JoinFederationExecution
		NM_Join_Federation_Execution rep ;
		rep.setException(e_FederateAlreadyExecutionMember,
		certi::stringize() << "Federate with same name <"<< federate << "> has already joined the federation");

		G.Out(pdGendoc,"processJoinFederation==>Answer to RTIA JFE ERROR %s",rep.getExceptionReason().c_str());

		rep.send(link,NM_msgBufSend);
		return ;
	}

#ifdef FEDERATION_USES_MULTICAST
	SocketMC *com_mc = NULL ;

	federations.info(num_federation, nb_federes, nb_regulateurs,
			pause, com_mc);
	assert(com_mc != NULL);
#else
	federations.info(num_federation, nb_federes, nb_regulateurs, pause);
#endif

	// Store Federate <->Socket reference.
	socketServer.setReferences(link->returnSocket(),
			num_federation,
			num_federe,
			address,
			peer);

	auditServer << "(" << num_federation << ")with handle " << num_federe
			<< ". Socket " << int(link->returnSocket());

	// Prepare answer about JoinFederationExecution
	rep.setFederationName(federation);
	rep.setFederate(num_federe);
	rep.setFederation(num_federation);
	rep.setNumberOfRegulators(nb_regulateurs);
	rep.setBestEffortPeer(peer);
	rep.setBestEffortAddress(address);

	// Now we have to answer about JoinFederationExecution
#ifdef FEDERATION_USES_MULTICAST
	rep.AdresseMulticast = com_mc->returnAdress();
#endif

	D.Out(pdInit, "Federate \"%s\" has joined Federation %u under handle %u.",
			federate.c_str(), num_federation, num_federe);

	// Send answer
	rep.send(link,NM_msgBufSend);

	G.Out(pdGendoc,"exit RTIG::processJoinFederation");
	G.Out(pdGendoc,"END ** JOIN FEDERATION SERVICE **");

}

// ----------------------------------------------------------------------------
//! Removes a federate from federation.
void
RTIG::processResignFederation(Socket *link,Handle federation,
		FederateHandle federe)
{
	NM_Resign_Federation_Execution reponse ;

	G.Out(pdGendoc,"BEGIN ** RESIGN FEDERATION SERVICE **");
	G.Out(pdGendoc,"enter RTIG::processResignFederation");

	federations.remove(federation, federe);
	D.Out(pdInit, "Federate %u is resigning from federation %u.",
			federe, federation);

	auditServer << "Federate " << federe << " resign federation("<<federation<<")" ;

	// Send answer to RTIA
	reponse.setFederate(federe);
	reponse.setFederation(federation);
	reponse.send(link,NM_msgBufSend);

	G.Out(pdGendoc,"exit RTIG::processResignFederation");
	G.Out(pdGendoc,"END ** RESIGN FEDERATION SERVICE **");

}

// ----------------------------------------------------------------------------
//! Removes a federation.
void
RTIG::processDestroyFederation(Socket *link, NM_Destroy_Federation_Execution *req)
{
	NM_Destroy_Federation_Execution rep ;

	std::string federation = req->getFederationName();

	G.Out(pdGendoc,"enter RTIG::processDestroyFederation");
	G.Out(pdGendoc,"BEGIN ** DESTROY FEDERATION SERVICE **");

	if (federation.length() == 0) throw RTIinternalError("Invalid Federation Name.");

	Handle num_federation = federations.getFederationHandle(federation);
	// May throw RTIinternalError
	//           FederatesCurrentlyJoined
	//           FederationExecutionDoesNotExist
	try {
		federations.destroyFederation(num_federation);
		// Here delete federation (num_federation) has been done
		federationHandles.free(num_federation);
		D.Out(pdInit, "Federation \"%s\" has been destroyed.", federation.c_str());
	}
	catch (Exception &e)
	{ printf("ERROR : %s  reason : %s\n",e._name,e._reason.c_str());
	if (strcmp(e._name,"RTIinternalError")==0 )
	{
		rep.setException(e_RTIinternalError,e._reason);
	}
	else if (strcmp(e._name,"FederationExecutionDoesNotExist")==0 )
	{
		rep.setException(e_FederationExecutionDoesNotExist,e._reason);
	}
	else if (strcmp(e._name,"FederatesCurrentlyJoined")==0 )
	{
		rep.setException(e_FederatesCurrentlyJoined,"at least one federate joined");
	}
	}

	rep.setFederate(req->getFederate());
	rep.setFederationName(req->getFederationName());
	if ( rep.getException() == e_NO_EXCEPTION )
	{
		auditServer << "Federation Name \"" << federation << "\"("<<num_federation<<") destroyed." ;
	}
	G.Out(pdGendoc,"processDestroyFederation===>write DFE to RTIA");

	rep.send(link,NM_msgBufSend);

	G.Out(pdGendoc,"END ** DESTROY FEDERATION SERVICE **");
	G.Out(pdGendoc,"exit RTIG::processDestroyFederation");
}

// ----------------------------------------------------------------------------
//! Set federate's class relevance advisroy switch
void
RTIG::processSetClassRelevanceAdvisorySwitch(Socket *link,
		NM_Set_Class_Relevance_Advisory_Switch *msg)
{
	NM_Set_Class_Relevance_Advisory_Switch rep;

	G.Out(pdGendoc,"enter RTIG::processSetClassRelevanceAdvisorySwitch");
	G.Out(pdGendoc,"BEGIN ** SET CLASS RELEVANCE ADVISORY SWITCH **");

	if (msg->isClassRelevanceAdvisorySwitchOn()) {
		auditServer << "ON";
		federations.setClassRelevanceAdvisorySwitch(msg->getFederation(), msg->getFederate());
		D.Out(pdTerm, "Federate %u of Federation %u sets AttributeRelevanceAdvisorySwitch.",
				msg->getFederate(), msg->getFederation());
	}
	else {
		auditServer << "OFF";
		federations.unsetClassRelevanceAdvisorySwitch(msg->getFederation(), msg->getFederate());
		D.Out(pdTerm, "Federate %u of Federation %u clears AttributeRelevanceAdvisorySwitch.",
				msg->getFederate(), msg->getFederation());
	}

	rep.send(link,NM_msgBufSend);

	G.Out(pdGendoc,"END ** SET CLASS RELEVANCE ADVISORY SWITCH **");
	G.Out(pdGendoc,"exit RTIG::processSetClassRelevanceAdvisorySwitch");
}

// ----------------------------------------------------------------------------
//! Set federate's interaction relevance advisroy switch
void
RTIG::processSetInteractionRelevanceAdvisorySwitch(Socket *link,
		NM_Set_Interaction_Relevance_Advisory_Switch *msg)
{
	NM_Set_Interaction_Relevance_Advisory_Switch rep;

	G.Out(pdGendoc,"enter RTIG::processSetInteractionRelevanceAdvisorySwitch");
	G.Out(pdGendoc,"BEGIN ** SET INTERACTION RELEVANCE ADVISORY SWITCH **");

	if (msg->isInteractionRelevanceAdvisorySwitchOn()) {
		auditServer << "ON";
		federations.setInteractionRelevanceAdvisorySwitch(msg->getFederation(), msg->getFederate());
		D.Out(pdTerm, "Federate %u of Federation %u sets InteractionRelevanceAdvisorySwitch.",
				msg->getFederate(), msg->getFederation());
	}
	else {
		auditServer << "OFF";
		federations.unsetInteractionRelevanceAdvisorySwitch(msg->getFederation(), msg->getFederate());
		D.Out(pdTerm, "Federate %u of Federation %u clears InteractionRelevanceAdvisorySwitch.",
				msg->getFederate(), msg->getFederation());
	}

	rep.send(link,NM_msgBufSend);

	G.Out(pdGendoc,"END ** SET INTERACTION RELEVANCE ADVISORY SWITCH **");
	G.Out(pdGendoc,"exit RTIG::processSetInteractionRelevanceAdvisorySwitch");
}

// ----------------------------------------------------------------------------
//! Set federate's attribute relevance advisroy switch
void
RTIG::processSetAttributeRelevanceAdvisorySwitch(Socket *link,
		NM_Set_Attribute_Relevance_Advisory_Switch *msg)
{
	NM_Set_Attribute_Relevance_Advisory_Switch rep;

	G.Out(pdGendoc,"enter RTIG::processSetAttributeRelevanceAdvisorySwitch");
	G.Out(pdGendoc,"BEGIN ** SET ATTRIBUTE RELEVANCE ADVISORY SWITCH **");

	if (msg->isAttributeRelevanceAdvisorySwitchOn()) {
		auditServer << "ON";
		federations.setAttributeRelevanceAdvisorySwitch(msg->getFederation(), msg->getFederate());
		D.Out(pdTerm, "Federate %u of Federation %u sets AttributeRelevanceAdvisorySwitch.",
				msg->getFederate(), msg->getFederation());
	}
	else {
		auditServer << "OFF";
		federations.unsetAttributeRelevanceAdvisorySwitch(msg->getFederation(), msg->getFederate());
		D.Out(pdTerm, "Federate %u of Federation %u clears AttributeRelevanceAdvisorySwitch.",
				msg->getFederate(), msg->getFederation());
	}

	rep.send(link,NM_msgBufSend);

	G.Out(pdGendoc,"END ** SET ATTRIBUTE RELEVANCE ADVISORY SWITCH **");
	G.Out(pdGendoc,"exit RTIG::processSetAttributeRelevanceAdvisorySwitch");
}

// ----------------------------------------------------------------------------
//! Set federate's attribute scope advisroy switch
void
RTIG::processSetAttributeScopeAdvisorySwitch(Socket *link,
		NM_Set_Attribute_Scope_Advisory_Switch *msg)
{
	NM_Set_Attribute_Scope_Advisory_Switch rep;

	G.Out(pdGendoc,"enter RTIG::processSetAttributeScopeAdvisorySwitch");
	G.Out(pdGendoc,"BEGIN ** SET ATTRIBUTE SCOPE ADVISORY SWITCH **");

	if (msg->isAttributeScopeAdvisorySwitchOn()) {
		auditServer << "ON";
		federations.setAttributeScopeAdvisorySwitch(msg->getFederation(), msg->getFederate());
		D.Out(pdTerm, "Federate %u of Federation %u sets AttributeScopeAdvisorySwitch.",
				msg->getFederate(), msg->getFederation());
	}
	else {
		auditServer << "OFF";
		federations.unsetAttributeScopeAdvisorySwitch(msg->getFederation(), msg->getFederate());
		D.Out(pdTerm, "Federate %u of Federation %u clears AttributeScopeAdvisorySwitch.",
				msg->getFederate(), msg->getFederation());
	}

	rep.send(link,NM_msgBufSend);

	G.Out(pdGendoc,"END ** SET ATTRIBUTE SCOPE ADVISORY SWITCH **");
	G.Out(pdGendoc,"exit RTIG::processSetAttributeScopeAdvisorySwitch");
}

// ----------------------------------------------------------------------------
//! Put federate as time regulating.
void
RTIG::processSetTimeRegulating(Socket *link, NM_Set_Time_Regulating *msg)
{
	G.Out(pdGendoc,"enter RTIG::processSetTimeRegulating");
	G.Out(pdGendoc,"BEGIN ** SET TIME REGULATING **");

	if (msg->isRegulatorOn()) {
		auditServer << "ON at time " << msg->getDate().getTime();

		federations.createRegulator(msg->getFederation(),
				msg->getFederate(),
				msg->getDate());

		// send timeRegulationEnabled() to federate.
		NM_Time_Regulation_Enabled rep ;
		rep.setFederate(msg->getFederate());
		rep.setFederation(msg->getFederation());
		rep.setDate(msg->getDate());

		G.Out(pdGendoc,"      processSetTimeRegulating====> write TRE to RTIA");

		rep.send(link,NM_msgBufSend);

		D.Out(pdTerm, "Federate %u of Federation %u sets TimeRegulation ON.",
				msg->getFederate(), msg->getFederation());
	}
	else {
		auditServer << "OFF" ;

		federations.removeRegulator(msg->getFederation(),
				msg->getFederate());
		D.Out(pdTerm, "Federate %u of Federation %u sets TimeRegulation OFF.",
				msg->getFederate(), msg->getFederation());
	}

	G.Out(pdGendoc,"END ** SET TIME REGULATING **");
	G.Out(pdGendoc,"exit RTIG::processSetTimeRegulating");
}

// ----------------------------------------------------------------------------
//! Put federate as time constrained
void
RTIG::processSetTimeConstrained(Socket *link, NM_Set_Time_Constrained *msg)
{
	G.Out(pdGendoc,"enter RTIG::processSetTimeConstrained");
	G.Out(pdGendoc,"BEGIN ** SET TIME CONSTRAINED **");

	if (msg->isConstrainedOn()) {
		auditServer << "ON at time " << msg->getDate().getTime();

		federations.addConstrained(msg->getFederation(),
				msg->getFederate());

		// send timeConstrainedEnabled() to federate.
		NM_Time_Constrained_Enabled rep ;
		rep.setFederate(msg->getFederate());
		rep.setFederation(msg->getFederation());
		rep.setDate(msg->getDate());

		G.Out(pdGendoc,"      processSetTimeConstrained====> write TCE to RTIA");

		rep.send(link,NM_msgBufSend);

		D.Out(pdTerm, "Federate %u of Federation %u is now constrained.",
				msg->getFederate(), msg->getFederation());
	}
	else {
		auditServer << "OFF" ;

		federations.removeConstrained(msg->getFederation(),
				msg->getFederate());
		D.Out(pdTerm, "Federate %u of Federation %u is no more constrained.",
				msg->getFederate(), msg->getFederation());
	}

	G.Out(pdGendoc,"END ** SET TIME CONSTRAINED **");
	G.Out(pdGendoc,"exit RTIG::processSetTimeConstrained");
}

void
RTIG::processMessageNull(NetworkMessage *msg, bool anonymous)
{
	auditServer << "Date " << msg->getDate().getTime();

	DNULL.Out(pdDebug, "Rcv NULL MSG (Federate=%d, Time = %f)",
	        msg->getFederate(), msg->getDate().getTime()) ;
	// Catch all exceptions because RTIA does not expect an answer anyway.
	try {
		federations.updateRegulator(msg->getFederation(),
				msg->getFederate(),
				msg->getDate(), anonymous);
	} catch (Exception &e) {}
} /* end of processMessageNull */

void
RTIG::processMessageNullPrime(NM_Message_Null_Prime *msg)
{
    DNULL.Out(pdDebug, "Rcv NULL PRIME MSG (Federate=%d, Time = %f)",
                msg->getFederate(), msg->getDate().getTime()) ;
	/*
	 * Update the NullPrimeDate of the concerned federate.
	 * and check the result in order to decide whether
	 * if the RTIG should send an anonymous NULL message or not
	 */
	if (federations.handleMessageNullPrime(msg->getFederation(), msg->getFederate(), msg->getDate())) {
		NM_Message_Null nmsg;
		nmsg.setDate(federations.getNullPrimeValue(msg->getFederation()));
		nmsg.setFederation(msg->getFederation());
		nmsg.setFederate(0);
		//nmsg.show(std::cout);
		processMessageNull(&nmsg,true);
	}

} /* end of processMessageNullPrime */

// ----------------------------------------------------------------------------
//! processRegisterSynchronization.
void
RTIG::processRegisterSynchronization(Socket *link, NM_Register_Federation_Synchronization_Point *req)
{

  G.Out(pdGendoc,"BEGIN ** REGISTER FEDERATION SYNCHRONIZATION POINT Service **");
  G.Out(pdGendoc,"enter RTIG::processRegisterSynchronization");

  auditServer << "Label \"" << req->getLabel() << "\" registered. Tag is \""
      << req->getTag() << "\"" ;

  /* prepare answer to be sent to RTIA */
  NM_Confirm_Synchronization_Point_Registration rep;
  rep.setFederate(req->getFederate());
  rep.setFederation(req->getFederation());
  rep.setLabel(req->getLabel());

  try {
    // boolean true means a federates set exists
    if ( req->getExists() ) {
      federations.manageSynchronization(req->getFederation(),
          req->getFederate(),
          true,
          req->getLabel(),
          req->getTag(),
          req->getFederatesSize(),
          req->getFederates());
    }
    else {
      federations.manageSynchronization(req->getFederation(),
          req->getFederate(),
          true,
          req->getLabel(),
          req->getTag());
    }
    // send synchronizationPointRegistrationSucceeded() to federate.
    rep.setSuccessIndicator(true);
  } catch (Exception &e) {
    /* the registration did fail */
    rep.setSuccessIndicator(false);
    rep.setFailureReason(std::string(e._name) + ":" + std::string(e._reason));
  }
  D.Out(pdTerm, "Federation %u is now synchronizing.", req->getFederation());

  G.Out(pdGendoc,"      processRegisterSynchronization====> write SPRS to RTIA");
  rep.send(link,NM_msgBufSend);

  // proceed with the broadcast iff registration was successful
  if (rep.getSuccessIndicator()) {
    // boolean true means a federates set exists
    if ( req->getExists() ) {
      federations.broadcastSynchronization(req->getFederation(),
          req->getFederate(),
          req->getLabel(),
          req->getTag(),
          req->getFederatesSize(),
          req->getFederates());
    }
    else {
      federations.broadcastSynchronization(req->getFederation(),
          req->getFederate(),
          req->getLabel(),
          req->getTag());
    }
  }
  G.Out(pdGendoc,"exit  RTIG::processRegisterSynchronization");
  G.Out(pdGendoc,"END   ** REGISTER FEDERATION SYNCHRONIZATION POINT Service **");

}

// ----------------------------------------------------------------------------
//! processSynchronizationAchieved.
void
RTIG::processSynchronizationAchieved(Socket *, NetworkMessage *req)
{
	auditServer << "Label \"" << req->getLabel() << "\" ended." ;

	federations.manageSynchronization(req->getFederation(),
			req->getFederate(),
			false,
			req->getLabel(),
			"");
	D.Out(pdTerm, "Federate %u has synchronized.", req->getFederate());
}

// ----------------------------------------------------------------------------
void
RTIG::processRequestFederationSave(Socket *, NetworkMessage *req)
{
	G.Out(pdGendoc,"BEGIN ** REQUEST FEDERATION SAVE SERVICE **");
	G.Out(pdGendoc,"enter RTIG::processRequestFederationSave");

	auditServer << "Federation save request." ;

	if ( req->isDated() )
		// With time
		federations.requestFederationSave(req->getFederation(), req->getFederate(),
				req->getLabel(), req->getDate());
	else
		// Without time
		federations.requestFederationSave(req->getFederation(), req->getFederate(),
				req->getLabel());

	G.Out(pdGendoc,"exit  RTIG::processRequestFederationSave");
	G.Out(pdGendoc,"END   ** REQUEST FEDERATION SAVE SERVICE **");
}
// ----------------------------------------------------------------------------
void
RTIG::processFederateSaveBegun(Socket *, NetworkMessage *req)
{
	G.Out(pdGendoc,"enter RTIG::processFederateSaveBegun");
	G.Out(pdGendoc,"BEGIN ** FEDERATE SAVE BEGUN SERVICE **");
	G.Out(pdGendoc,"processFederateSaveBegun federation = %d",req->getFederation());

	auditServer << "Federate " << req->getFederate() << " save begun." ;

	federations.federateSaveBegun(req->getFederation(), req->getFederate());

	G.Out(pdGendoc,"END   ** FEDERATE SAVE BEGUN SERVICE **");
	G.Out(pdGendoc,"exit  RTIG::processFederateSaveBegun");
}

// ----------------------------------------------------------------------------
void
RTIG::processFederateSaveStatus(Socket *, NetworkMessage *req)
{
	G.Out(pdGendoc,"enter RTIG::processFederateSaveStatus");
	if (req->getMessageType() == NetworkMessage::FEDERATE_SAVE_COMPLETE)
		G.Out(pdGendoc,"BEGIN ** FEDERATE SAVE COMPLETE SERVICE **");
	else
		G.Out(pdGendoc,"BEGIN ** FEDERATE SAVE NOT COMPLETE SERVICE **");

	auditServer << "Federate " << req->getFederate() << " save ended." ;

	bool status = req->getMessageType() == NetworkMessage::FEDERATE_SAVE_COMPLETE ;
	federations.federateSaveStatus(req->getFederation(), req->getFederate(), status);

	G.Out(pdGendoc,"exit  END   ** FEDERATE SAVE (NOT) COMPLETE SERVICE **");
	G.Out(pdGendoc,"exit  RTIG::processFederateSaveStatus");
}

// ----------------------------------------------------------------------------
void
RTIG::processRequestFederationRestore(Socket *, NetworkMessage *req)
{
	G.Out(pdGendoc,"BEGIN ** REQUEST FEDERATION RESTORE SERVICE **");
	G.Out(pdGendoc,"enter RTIG::processRequestFederationRestore");
	auditServer << "Federate " << req->getFederate() << " request restore." ;

	federations.requestFederationRestore(req->getFederation(), req->getFederate(),
			req->getLabel());
	G.Out(pdGendoc,"exit  RTIG::processRequestFederationRestore");
	G.Out(pdGendoc,"END   ** REQUEST FEDERATION RESTORE SERVICE **");
}

// ----------------------------------------------------------------------------
void
RTIG::processFederateRestoreStatus(Socket *, NetworkMessage *req)
{
	G.Out(pdGendoc,"BEGIN ** FEDERATE RESTORE (NOT)COMPLETE **");
	G.Out(pdGendoc,"enter RTIG::processRequestFederateRestoreStatus");
	auditServer << "Federate " << req->getFederate() << " restore ended." ;

	bool status = req->getMessageType() == NetworkMessage::FEDERATE_RESTORE_COMPLETE ;

	federations.federateRestoreStatus(req->getFederation(), req->getFederate(), status);

	G.Out(pdGendoc,"exit  RTIG::processRequestFederateRestoreStatus");
	G.Out(pdGendoc,"END   ** FEDERATE RESTORE (NOT)COMPLETE **");
}

// ----------------------------------------------------------------------------
// processPublishObjectClass
void
RTIG::processPublishObjectClass(Socket *link, NM_Publish_Object_Class *req)
{
    bool pub = (req->getMessageType() == NetworkMessage::PUBLISH_OBJECT_CLASS);

    auditServer << "Publish Object Class = " << req->getObjectClass() << ", # of att. = "
                << req->getAttributesSize() ;

    federations.publishObject(req->getFederation(),
                              req->getFederate(),
                              req->getObjectClass(),
                              req->getAttributes(),
                              pub);

    D.Out(pdRegister, "Federate %u of Federation %u published object class %d.",
            req->getFederate(), req->getFederation(), req->getObjectClass());

    if (pub) {
        NM_Publish_Object_Class POC;
        POC.setFederate(req->getFederate());
        POC.setObjectClass(req->getObjectClass());
        POC.send(link,NM_msgBufSend); // send answer to RTIA
    } else {
        NM_Unpublish_Object_Class UOC;
        UOC.setFederate(req->getFederate());
        UOC.setObjectClass(req->getObjectClass());
        UOC.send(link,NM_msgBufSend); // send answer to RTIA
    }
}

// ----------------------------------------------------------------------------
// processSubscribeObjectClass
void
RTIG::processSubscribeObjectClass(Socket *link, NM_Subscribe_Object_Class *req)
{
    G.Out(pdGendoc,"enter RTIG::processSubscribeObjectClass");
    G.Out(pdGendoc,"BEGIN **  SUBSCRIBE OBJECT CLASS SERVICE **");

    std::vector <AttributeHandle> emptyAttributeList;
    bool sub = (req->getMessageType() == NetworkMessage::SUBSCRIBE_OBJECT_CLASS);

    auditServer << "Subscribe Object Class = " << req->getObjectClass()
                << ", # of att. = " << req->getAttributesSize() ;

    federations.subscribeObject(req->getFederation(),
                                req->getFederate(),
                                req->getObjectClass(),
                                sub ? req->getAttributes() : emptyAttributeList);

    D.Out(pdRegister,
            "Federate %u of Federation %u subscribed to object class %d.",
            req->getFederate(), req->getFederation(), req->getObjectClass());

    if (sub) {
        NM_Subscribe_Object_Class rep;
        rep.setFederate(req->getFederate());
        rep.setObjectClass(req->getObjectClass());
        rep.send(link,NM_msgBufSend); // send answer to RTIA
    } else {
        NM_Unsubscribe_Object_Class rep;
        rep.setFederate(req->getFederate());
        rep.setObjectClass(req->getObjectClass());
        rep.send(link,NM_msgBufSend); // send answer to RTIA
    }

    G.Out(pdGendoc,"END   **  SUBSCRIBE OBJECT CLASS SERVICE **");
    G.Out(pdGendoc,"exit  RTIG::processSubscribeObjectClass");
}

// ----------------------------------------------------------------------------
// processPublishInteractionClass
void
RTIG::processPublishInteractionClass(Socket *link, NM_Publish_Interaction_Class *req)
{
	assert(link != NULL && req != NULL);

	bool pub = (req->getMessageType() == NetworkMessage::PUBLISH_INTERACTION_CLASS);

	auditServer << "Publish Interaction Class = " << req->getInteractionClass() ;
	federations.publishInteraction(req->getFederation(),
			req->getFederate(),
			req->getInteractionClass(),
			pub);
	D.Out(pdRequest, "Federate %u of Federation %u publishes Interaction %d.",
			req->getFederate(),
			req->getFederation(),
			req->getInteractionClass());

	if (pub) {
		NM_Publish_Interaction_Class rep;
		rep.setFederate(req->getFederate());
		rep.setInteractionClass(req->getInteractionClass());

		rep.send(link,NM_msgBufSend); // send answer to RTIA
	} else {
		NM_Unpublish_Interaction_Class rep;
		rep.setFederate(req->getFederate());
		rep.setInteractionClass(req->getInteractionClass());

		rep.send(link,NM_msgBufSend); // send answer to RTIA
	}
}

// ----------------------------------------------------------------------------
// processSubscribeInteractionClass
void
RTIG::processSubscribeInteractionClass(Socket *link, NM_Subscribe_Interaction_Class *req)
{
	bool sub = (req->getMessageType() == NetworkMessage::SUBSCRIBE_INTERACTION_CLASS);

	auditServer << "Subscribe Interaction Class = " << req->getInteractionClass() ;
	federations.subscribeInteraction(req->getFederation(),
			req->getFederate(),
			req->getInteractionClass(),
			sub);
	D.Out(pdRequest,
			"Federate %u of Federation %u subscribed to Interaction %d.",
			req->getFederate(),
			req->getFederation(),
			req->getInteractionClass());

	if (sub) {
		NM_Subscribe_Interaction_Class rep;
		rep.setFederate(req->getFederate());
		rep.setInteractionClass(req->getInteractionClass());

		rep.send(link,NM_msgBufSend); // send answer to RTIA
	} else {
		NM_Unsubscribe_Interaction_Class rep;
		rep.setFederate(req->getFederate());
		rep.setInteractionClass(req->getInteractionClass());

		rep.send(link,NM_msgBufSend); // send answer to RTIA
	}
}

// ----------------------------------------------------------------------------
// processReserveObjectInstanceName
void 
RTIG::processReserveObjectInstanceName(Socket *link, NM_Reserve_Object_Instance_Name *req)
{
	G.Out(pdGendoc,"enter RTIG::processReserveObjectInstanceName");
	auditServer << "Reserve Object Name = " << req->getObjectName();

	federations.reserveObjectInstanceName(	req->getFederation(), 
											req->getFederate(), 
											req->getObjectName());

	G.Out(pdGendoc,"exit RTIG::processReserveObjectInstanceName");
}

// ----------------------------------------------------------------------------
// processRegisterObject
void
RTIG::processRegisterObject(Socket *link, NM_Register_Object *req)
{
	NM_Register_Object rep;

	auditServer << "Register Object Class = " << req->getObjectClass() ;
	rep.setObject(federations.registerObject(req->getFederation(),
			req->getFederate(),
			req->getObjectClass(),
			req->getLabel()));

	D.Out(pdRegister,
			"Object \"%s\" of Federate %u has been registered under ID %u.",
			req->getLabel().c_str(), req->getFederate(), rep.getObject());

	rep.setFederate(req->getFederate());
	rep.setFederation(req->getFederation());
	rep.setObjectClass(req->getObjectClass());
	// rep.object is set by the call of registerObject
	if (req->hasObjectName()) {
		rep.setObjectName(req->getObjectName());
	}
	if (req->isLabelled()) {
		rep.setLabel(req->getLabel());
	}

	rep.send(link,NM_msgBufSend); // Send answer to RTIA
}

// ----------------------------------------------------------------------------
// processUpdateAttributeValues
void
RTIG::processUpdateAttributeValues(Socket *link, NM_Update_Attribute_Values *req)
{
	G.Out(pdGendoc,"enter RTIG::processUpdateAttributeValues");

	auditServer << "ObjID = " << req->getObject()
									<< ", Date = " << req->getDate().getTime();

	// Forward the call
	if ( req->isDated() )
	{
		// UAV with time
		federations.updateAttribute(req->getFederation(),
				req->getFederate(),
				req->getObject(),
				req->getAttributes(),
				req->getValues(),
				req->getAttributesSize(),
				req->getDate(),
				req->getLabel());
	}
	else
	{
		// UAV without time
		federations.updateAttribute(req->getFederation(),
				req->getFederate(),
				req->getObject(),
				req->getAttributes(),
				req->getValues(),
				req->getAttributesSize(),
				req->getLabel());
	}

	// Building answer (Network Message)
	NM_Update_Attribute_Values rep ;
	rep.setFederate(req->getFederate());
	rep.setObject(req->getObject());
	// Don't forget date, label and tag if provided in the request
	if (req->isDated()) {
		rep.setDate(req->getDate());
	}
	if (req->isLabelled()) {
		rep.setLabel(req->getLabel());
	}
	if (req->isTagged()) {
		rep.setTag(req->getTag());
	}

	rep.send(link,NM_msgBufSend); // send answer to RTIA
	G.Out(pdGendoc,"exit  RTIG::processUpdateAttributeValues");
}

// ----------------------------------------------------------------------------
// processSendInteraction
void
RTIG::processSendInteraction(Socket *link, NM_Send_Interaction *req)
{
    G.Out(pdGendoc,"BEGIN ** SEND INTERACTION SERVICE **");
    G.Out(pdGendoc,"enter RTIG::processSendInteraction");

    // Building Value Array
    auditServer << "IntID = " << req->getInteractionClass()
                << ", date = " << req->getDate().getTime();
    if (req->isDated()) {
        federations.updateParameter(req->getFederation(),
                req->getFederate(),
                req->getInteractionClass(),
                req->getParameters(),
                req->getValues(),
                req->getParametersSize(),
                req->getDate(),
                req->getRegion(),
                req->getLabel());
    }
    else {
        federations.updateParameter(req->getFederation(),
                req->getFederate(),
                req->getInteractionClass(),
                req->getParameters(),
                req->getValues(),
                req->getParametersSize(),
                req->getRegion(),
                req->getLabel());
    }

    D.Out(pdDebug, "Interaction %d parameters update completed",
            req->getInteractionClass());

    NM_Send_Interaction rep ;
    rep.setFederate(req->getFederate());
    rep.setInteractionClass(req->getInteractionClass());
    // Don't forget label and tag
    rep.setLabel(req->getLabel());
    rep.setTag(req->getTag());
    G.Out(pdGendoc,"processSendInteraction===>write");
    rep.send(link,NM_msgBufSend); // send answer to RTIA

    G.Out(pdGendoc,"exit RTIG::processSendInteraction");
    G.Out(pdGendoc,"END ** SEND INTERACTION SERVICE **");

}

// ----------------------------------------------------------------------------
// processDeleteObject
void
RTIG::processDeleteObject(Socket *link, NM_Delete_Object *req)
{
	G.Out(pdGendoc,"BEGIN ** DELETE OBJECT INSTANCE service **");
	G.Out(pdGendoc,"enter RTIG::processDeleteObject");
	auditServer << "Delete ObjID = " << req->getObject() ;

	if ( req->isDated() ) {
		federations.destroyObject(req->getFederation(),
				req->getFederate(),
				req->getObject(),
				req->getDate(),
				req->getLabel());
	}
	else {
		federations.destroyObject(req->getFederation(),
				req->getFederate(),
				req->getObject(),
				req->getLabel());
	}

	D.Out(pdRegister, "Object # %u of Federation %u has been deleted.",
			req->getObject(), req->getFederation());

	NM_Delete_Object rep ;
	rep.setFederate( req->getFederate() );
	rep.setObject(req->getObject());

	rep.send(link,NM_msgBufSend); // send answer to RTIA

	G.Out(pdGendoc,"exit RTIG::processDeleteObject");
	G.Out(pdGendoc,"END ** DELETE OBJECT INSTANCE **");
}

// ----------------------------------------------------------------------------
// processqueryAttributeOwnership
void
RTIG::processQueryAttributeOwnership(Socket *link, NM_Query_Attribute_Ownership *req)
{
	D.Out(pdDebug, "Owner of Attribute %u of Object %u .",
			req->getAttribute(), req->getObject());

	auditServer << "AttributeHandle = " << req->getAttribute() ;

	federations.searchOwner(req->getFederation(),
			req->getFederate(),
			req->getObject(),
			req->getAttribute());

	D.Out(pdDebug, "Owner of Attribute %u of Object %u .",
			req->getAttribute(), req->getObject());

	NM_Query_Attribute_Ownership rep ;
	rep.setFederate(req->getFederate());
	rep.setObject(req->getObject());

	rep.send(link,NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processAttributeOwnedByFederate
void
RTIG::processAttributeOwnedByFederate(Socket *link, NM_Is_Attribute_Owned_By_Federate *req)
{
	NM_Is_Attribute_Owned_By_Federate rep ;

	D.Out(pdDebug, "Owner of Attribute %u of Object %u .",
			req->getAttribute(), req->getObject());

	auditServer << "AttributeHandle = " << req->getAttribute() ;

	if (federations.isOwner(req->getFederation(),
			req->getFederate(),
			req->getObject(),
			req->getAttribute()))
		rep.setLabel("RTI_TRUE");
	else
		rep.setLabel("RTI_FALSE");

	D.Out(pdDebug, "Owner of Attribute %u of Object %u .",
			req->getAttribute(), req->getObject());

	rep.setFederate(req->getFederate());
	rep.setObject(req->getObject());
	rep.setAttribute(req->getAttribute()) ;

	rep.send(link,NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processNegotiatedOwnershipDivestiture
void
RTIG::processNegotiatedOwnershipDivestiture(Socket *link, NM_Negotiated_Attribute_Ownership_Divestiture *req)
{
	auditServer << "Object = " <<  req->getObject()
										<< ", # of att. = " << req->getAttributesSize() ;
	federations.negotiateDivestiture(req->getFederation(),
			req->getFederate(),
			req->getObject(),
			req->getAttributes(),
			req->getAttributesSize(),
			req->getLabel());

	D.Out(pdDebug, "Federate %u of Federation %u negotiate "
			"divestiture of object %u.",
			req->getFederate(), req->getFederation(), req->getObject());

	NM_Negotiated_Attribute_Ownership_Divestiture rep ;
	rep.setFederate(req->getFederate());
	rep.setObject(req->getObject());

	rep.send(link,NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processAcquisitionIfAvailable
void
RTIG::processAcquisitionIfAvailable(Socket *link, NM_Attribute_Ownership_Acquisition_If_Available *req)
{
	auditServer << "Object = " << req->getObject()
										<< ", # of att. = " << req->getAttributesSize() ;

	federations.acquireIfAvailable(req->getFederation(),
			req->getFederate(),
			req->getObject(),
			req->getAttributes(),
			req->getAttributesSize());

	D.Out(pdDebug,
			"Federate %u of Federation %u acquisitionIfAvailable "
			"of object %u.",
			req->getFederate(), req->getFederation(), req->getObject());

	NM_Attribute_Ownership_Acquisition_If_Available rep ;
	rep.setFederate(req->getFederate());
	rep.setObject(req->getObject());

	rep.send(link,NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processUnconditionalDivestiture
void
RTIG::processUnconditionalDivestiture(Socket *link, NM_Unconditional_Attribute_Ownership_Divestiture *req)
{
	auditServer << "Object = " << req->getObject()
										<< ", # of att. = " << req->getAttributesSize() ;

	federations.divest(req->getFederation(),
			req->getFederate(),
			req->getObject(),
			req->getAttributes(),
			req->getAttributesSize());

	D.Out(pdDebug,
			"Federate %u of Federation %u UnconditionalDivestiture "
			"of object %u.",
			req->getFederate(), req->getFederation(), req->getObject());

	NM_Unconditional_Attribute_Ownership_Divestiture rep ;
	rep.setFederate(req->getFederate());
	rep.setObject(req->getObject());

	rep.send(link,NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processOwnershipAcquisition
void
RTIG::processOwnershipAcquisition(Socket *link, NM_Attribute_Ownership_Acquisition *req)
{
	auditServer << "Object = " << req->getObject()
										<< ", # of att. = " << req->getAttributesSize() ;

	federations.acquire(req->getFederation(),
			req->getFederate(),
			req->getObject(),
			req->getAttributes(),
			req->getAttributesSize(),
			req->getLabel());

	D.Out(pdDebug,
			"Federate %u of Federation %u ownership acquisition of object %u.",
			req->getFederate(), req->getFederation(), req->getObject());

	NM_Attribute_Ownership_Acquisition rep ;
	rep.setFederate(req->getFederate());
	rep.setObject(req->getObject());
	rep.setAttributesSize(0);

	rep.send(link,NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processCancelNegotiatedDivestiture
void
RTIG::processCancelNegotiatedDivestiture(Socket *link, NM_Cancel_Negotiated_Attribute_Ownership_Divestiture *req)
{
	auditServer << "Object = " << req->getObject()
										<< ", # of att. = " << req->getAttributesSize() ;

	federations.cancelDivestiture(req->getFederation(),
			req->getFederate(),
			req->getObject(),
			req->getAttributes(),
			req->getAttributesSize());

	D.Out(pdDebug, "Federate %u of Federation %u cancel negotiate "
			"divestiture of object %u.",
			req->getFederate(), req->getFederation(), req->getObject());

	NM_Cancel_Negotiated_Attribute_Ownership_Divestiture rep ;
	rep.setFederate(req->getFederate());
	rep.setObject(req->getObject());

	rep.send(link,NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processReleaseResponse
void
RTIG::processReleaseResponse(Socket *link, NM_Attribute_Ownership_Release_Response *req)
{
	auditServer << "Object = " << req->getObject()
									<< ", # of att. = " << req->getAttributesSize() ;

	AttributeHandleSet *attributes =
			federations.respondRelease(req->getFederation(),
					req->getFederate(),
					req->getObject(),
					req->getAttributes(),
					req->getAttributesSize());

	D.Out(pdDebug, "Federate %u of Federation %u release response "
			"of object %u.",
			req->getFederate(), req->getFederation(), req->getObject());

	NM_Attribute_Ownership_Release_Response rep ;
	rep.setAttributesSize(attributes->size()) ;

	for (uint32_t i = 0 ; i < attributes->size(); ++i) {
		rep.setAttributes(attributes->getHandle(i),i);
	}

	rep.setFederate(req->getFederate());
	rep.setObject(req->getObject());

	rep.send(link,NM_msgBufSend); // Send answer to RTIA
}

// ----------------------------------------------------------------------------
// processCancelAcquisition
void
RTIG::processCancelAcquisition(Socket *link, NM_Cancel_Attribute_Ownership_Acquisition *req)
{
	auditServer << "Object = " << req->getObject()
									<< ", # of att. = " << req->getAttributesSize() ;

	federations.cancelAcquisition(req->getFederation(),
			req->getFederate(),
			req->getObject(),
			req->getAttributes(),
			req->getAttributesSize());

	D.Out(pdDebug,
			"Federate %u of Federation %u release response of object %u.",
			req->getFederate(), req->getFederation(), req->getObject());

	NM_Cancel_Attribute_Ownership_Acquisition rep ;
	rep.setFederate(req->getFederate());
	rep.setObject(req->getObject());

	rep.send(link,NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processCreateRegion
void
RTIG::processCreateRegion(Socket *link, NM_DDM_Create_Region *req)
{
	// TODO: audit...

	NM_DDM_Create_Region rep ;
	rep.setRegion(federations.createRegion(req->getFederation(),
			req->getFederate(),
			req->getSpace(),
			req->getNbExtents()));

	Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation "
			<< req->getFederation() << " creates region " << rep.getRegion()
			<< endl ;

	rep.setFederate(req->getFederate());
	rep.send(link,NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processModifyRegion
void
RTIG::processModifyRegion(Socket *link, NM_DDM_Modify_Region *req)
{
	Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation "
			<< req->getFederation() << " modifies region " << req->getRegion()
			<< endl ;

	federations.modifyRegion(req->getFederation(), req->getFederate(),
			req->getRegion(), req->getExtents());

	NM_DDM_Modify_Region rep ;
	rep.setFederate(req->getFederate());
	rep.send(link,NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processDeleteRegion
void
RTIG::processDeleteRegion(Socket *link, NM_DDM_Delete_Region *req)
{
	// TODO: audit...

	Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation "
			<< req->getFederation() << " deletes region " << req->getRegion()
			<< endl ;

	federations.deleteRegion(req->getFederation(), req->getFederate(), req->getRegion());

	NM_DDM_Delete_Region rep;

	rep.setFederate(req->getFederate());
	rep.setRegion(req->getRegion());
	rep.send(link,NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processAssociateRegion
void
RTIG::processAssociateRegion(Socket *link, NM_DDM_Associate_Region *req)
{
	// TODO: audit...

	Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation "
			<< req->getFederation() << " associates region " << req->getRegion()
			<< " to some attributes of object " << req->getObject() << endl ;

	federations.associateRegion(req->getFederation(), req->getFederate(), req->getObject(),
			req->getRegion(), req->getAttributesSize(),
			req->getAttributes());

	NM_DDM_Associate_Region rep ;
	rep.setFederate(req->getFederate());
	rep.send(link,NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processUnassociateRegion
void
RTIG::processUnassociateRegion(Socket *link, NM_DDM_Unassociate_Region *req)
{
	// TODO: audit...

	federations.unassociateRegion(req->getFederation(), req->getFederate(),
			req->getObject(), req->getRegion());

	Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation "
			<< req->getFederation() << " associates region " << req->getRegion()
			<< " from object " << req->getObject() << endl ;

	NM_DDM_Unassociate_Region rep ;
	rep.setFederate(req->getFederate());
	rep.send(link,NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processSubscribeAttributes
void
RTIG::processSubscribeAttributesWR(Socket *link, NM_DDM_Subscribe_Attributes *req)
{
	// TODO: audit...
	Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation "
			<< req->getFederation() << " subscribes with region " << req->getRegion()
			<< " to some attributes of class " << req->getObjectClass() << endl ;

	federations.subscribeAttributesWR(req->getFederation(), req->getFederate(),
			req->getObjectClass(), req->getRegion(),
			req->getAttributesSize(), req->getAttributes());

	NM_DDM_Subscribe_Attributes rep ;
	rep.setFederate(req->getFederate());
	rep.setObjectClass(req->getObjectClass());
	rep.send(link,NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processUnsubscribeAttributes
void
RTIG::processUnsubscribeAttributesWR(Socket *link, NM_DDM_Unsubscribe_Attributes *req)
{
	// TODO: audit...
	Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation "
			<< req->getFederation() << " unsubscribes with region " << req->getRegion()
			<< " from object class " << req->getObjectClass() << endl ;

	federations.unsubscribeAttributesWR(req->getFederation(), req->getFederate(),
			req->getObjectClass(), req->getRegion());

	NM_DDM_Unsubscribe_Attributes rep ;
	rep.setFederate( req->getFederate());
	rep.send(link,NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processSubscribeInteractions
void
RTIG::processSubscribeInteractionWR(Socket *link, NM_DDM_Subscribe_Interaction *req)
{
	// TODO: audit...

	federations.subscribeInteractionWR(req->getFederation(), req->getFederate(),
			req->getInteractionClass(), req->getRegion());

	Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation "
			<< req->getFederation() << " subscribes with region " << req->getRegion()
			<< " to interaction class " << req->getInteractionClass() << endl ;

	NM_DDM_Subscribe_Interaction rep ;
	rep.setFederate(req->getFederate());
	rep.send(link,NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processUnsubscribeInteractions
void
RTIG::processUnsubscribeInteractionWR(Socket *link, NM_DDM_Unsubscribe_Interaction *req)
{
	// TODO: audit...

	federations.unsubscribeInteractionWR(req->getFederation(), req->getFederate(),
			req->getInteractionClass(), req->getRegion());

	Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation "
			<< req->getFederation() << " unsubscribes with region " << req->getRegion()
			<< " from interaction class " << req->getInteractionClass() << endl ;

	NM_DDM_Unsubscribe_Interaction rep ;
	rep.setFederate(req->getFederate());
	rep.send(link,NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processRegisterObjectWithRegion
void
RTIG::processRegisterObjectWithRegion(Socket *link, NM_DDM_Register_Object *req)
{
	NM_DDM_Register_Object rep;
	// FIXME bug #9869
	// When we were passed a set of region
	// we should register object for each region
	// the trouble comes from the fact that
	// there is both
	//     - req->region  (coming from NetworkMessage::region)
	//     - req->regions (coming from BasicMessage::regions)
	// would be nice to sort those thing out.
	rep.setObject(federations.registerObjectWithRegion(req->getFederation(),
			req->getFederate(),
			req->getObjectClass(),
			req->getLabel(),
			req->getRegion(),
			req->getAttributesSize(),
			req->getAttributes()));

	D.Out(pdRegister,
			"Object \"%s\" of Federate %u has been registered under ID %u.",
			req->getLabel().c_str(), req->getFederate(), rep.getObject());

	rep.setFederate(req->getFederate());
	rep.send(link,NM_msgBufSend); // Send answer to RTIA
}

// ----------------------------------------------------------------------------
// processRequestObjectAttributeValueUpdate
void
RTIG::processRequestObjectAttributeValueUpdate(Socket *link, NM_Request_Object_Attribute_Value_Update *request)
{
    NM_Request_Object_Attribute_Value_Update answer;
    G.Out(pdGendoc,"enter RTIG::processRequestObjectAttributeValueUpdate");
    G.Out(pdGendoc,"BEGIN ** REQUEST OBJECT ATTRIBUTE VALUE UPDATE **");

    auditServer << "ObjID = " << request->getObject() ;

    // We have to do verifications about this object and we need owner
    answer.setException(e_NO_EXCEPTION);
    try	{
        // While searching for the federate owner we will send
        // a NM_Provide_Attribute_Value_Update
        // (see Federation::requestObjectOwner)
        (void) federations.requestObjectOwner(request->getFederation(),
                request->getFederate(),
                request->getObject(),
                request->getAttributes(),
                request->getAttributesSize());
    }
    catch (ObjectNotKnown& e)
    {
        answer.setException(e_ObjectNotKnown,e._reason);
    }
    catch (FederationExecutionDoesNotExist& e)
    {
        answer.setException(e_FederationExecutionDoesNotExist,e._reason);
    }
    catch (RTIinternalError& e)
    {
        answer.setException(e_RTIinternalError,e._reason);
    }

    answer.setFederate(request->getFederate());
    answer.setObject(request->getObject());

    answer.send(link,NM_msgBufSend); // Send answer to RTIA
    G.Out(pdGendoc,"exit  RTIG::processRequestObjectAttributeValueUpdate");
    G.Out(pdGendoc,"END   ** REQUEST OBJECT ATTRIBUTE VALUE UPDATE **");
}

// ----------------------------------------------------------------------------
// processRequestClassAttributeValueUpdate
void
RTIG::processRequestClassAttributeValueUpdate(Socket *link, NM_Request_Class_Attribute_Value_Update *request)
{
	NM_Request_Class_Attribute_Value_Update answer;

	G.Out(pdGendoc,"enter RTIG::processRequestClassAttributeValueUpdate");
	G.Out(pdGendoc,"BEGIN ** REQUEST CLASS ATTRIBUTE VALUE UPDATE **");

	answer.setException(e_NO_EXCEPTION);
	try
	{
		federations.requestClassAttributeValueUpdate(request->getFederation(),
				request->getFederate(),
				request->getObjectClass(),
				request->getAttributes(),
				request->getAttributesSize());
	}
	catch (ObjectClassNotDefined& e)
	{
		answer.setException(e_ObjectClassNotDefined,e._reason);
	}
	catch (FederationExecutionDoesNotExist& e)
	{
		answer.setException(e_FederationExecutionDoesNotExist,e._reason);
	}
	catch (RTIinternalError& e)
	{
		answer.setException(e_RTIinternalError,e._reason);
	}
	
	answer.setFederate(request->getFederate());
	answer.setObjectClass(request->getObjectClass());

	answer.send(link,NM_msgBufSend); // Send answer to RTIA

	G.Out(pdGendoc,"exit  RTIG::processRequestClassAttributeValueUpdate");
	G.Out(pdGendoc,"END   ** REQUEST CLASS  ATTRIBUTE VALUE UPDATE **");
}

}} // namespace certi/rtig

// $Id: RTIG_processing.cc,v 3.116 2013/09/24 14:27:58 erk Exp $
