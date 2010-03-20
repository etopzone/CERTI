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
// $Id: RTIG_processing.cc,v 3.97 2010/03/20 16:34:13 erk Exp $
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
	catch (CouldNotOpenFED e)
	{
		rep.setException(e_CouldNotOpenFED,e._reason);
	}
	catch (ErrorReadingFED e)
	{
		rep.setException(e_ErrorReadingFED,e._reason);
	}
	catch (FederationExecutionAlreadyExists e)
	{
		rep.setException(e_FederationExecutionAlreadyExists,e._reason);
	}
#endif
	// Prepare answer for RTIA : store NetworkMessage rep
	if ( rep.getException() == e_NO_EXCEPTION )
	{
		rep.federation = h ;
		rep.setFEDid(FEDid);
		rep.setFederationName(federation);
		auditServer <<" created.";

	}

	G.Out(pdGendoc,"processCreateFederation===>write answer to RTIA");

	rep.send(link,NM_msgBufSend); // Send answer to RTIA

	D.Out(pdInit, "Federation \"%s\" created with Handle %d.",
			federation.c_str(), rep.federation);

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

	// Need to dump the fom into that
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
				"Federate with same name has yet joined the federation");

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
	rep.federate = num_federe ;
	rep.federation = num_federation ;
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
	reponse.federate = federe ;
	reponse.federation = federation ;
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

	rep.federate = req->federate ;
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
		federations.setClassRelevanceAdvisorySwitch(msg->federation, msg->federate);
		D.Out(pdTerm, "Federate %u of Federation %u sets AttributeRelevanceAdvisorySwitch.",
				msg->federate, msg->federation);
	}
	else {
		auditServer << "OFF";
		federations.unsetClassRelevanceAdvisorySwitch(msg->federation, msg->federate);
		D.Out(pdTerm, "Federate %u of Federation %u clears AttributeRelevanceAdvisorySwitch.",
				msg->federate, msg->federation);
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
		federations.setInteractionRelevanceAdvisorySwitch(msg->federation, msg->federate);
		D.Out(pdTerm, "Federate %u of Federation %u sets InteractionRelevanceAdvisorySwitch.",
				msg->federate, msg->federation);
	}
	else {
		auditServer << "OFF";
		federations.unsetInteractionRelevanceAdvisorySwitch(msg->federation, msg->federate);
		D.Out(pdTerm, "Federate %u of Federation %u clears InteractionRelevanceAdvisorySwitch.",
				msg->federate, msg->federation);
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
		federations.setAttributeRelevanceAdvisorySwitch(msg->federation, msg->federate);
		D.Out(pdTerm, "Federate %u of Federation %u sets AttributeRelevanceAdvisorySwitch.",
				msg->federate, msg->federation);
	}
	else {
		auditServer << "OFF";
		federations.unsetAttributeRelevanceAdvisorySwitch(msg->federation, msg->federate);
		D.Out(pdTerm, "Federate %u of Federation %u clears AttributeRelevanceAdvisorySwitch.",
				msg->federate, msg->federation);
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
		federations.setAttributeScopeAdvisorySwitch(msg->federation, msg->federate);
		D.Out(pdTerm, "Federate %u of Federation %u sets AttributeScopeAdvisorySwitch.",
				msg->federate, msg->federation);
	}
	else {
		auditServer << "OFF";
		federations.unsetAttributeScopeAdvisorySwitch(msg->federation, msg->federate);
		D.Out(pdTerm, "Federate %u of Federation %u clears AttributeScopeAdvisorySwitch.",
				msg->federate, msg->federation);
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

		federations.createRegulator(msg->federation,
				msg->federate,
				msg->getDate());

		// send timeRegulationEnabled() to federate.
		NM_Time_Regulation_Enabled rep ;
		rep.federate = msg->federate ;
		rep.federation = msg->federation ;
		rep.setDate(msg->getDate());

		G.Out(pdGendoc,"      processSetTimeRegulating====> write TRE to RTIA");

		rep.send(link,NM_msgBufSend);

		D.Out(pdTerm, "Federate %u of Federation %u sets TimeRegulation ON.",
				msg->federate, msg->federation);
	}
	else {
		auditServer << "OFF" ;

		federations.removeRegulator(msg->federation,
				msg->federate);
		D.Out(pdTerm, "Federate %u of Federation %u sets TimeRegulation OFF.",
				msg->federate, msg->federation);
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

		federations.addConstrained(msg->federation,
				msg->federate);

		// send timeConstrainedEnabled() to federate.
		NM_Time_Constrained_Enabled rep ;
		rep.federate = msg->federate ;
		rep.federation = msg->federation ;
		rep.setDate(msg->getDate());

		G.Out(pdGendoc,"      processSetTimeConstrained====> write TCE to RTIA");

		rep.send(link,NM_msgBufSend);

		D.Out(pdTerm, "Federate %u of Federation %u is now constrained.",
				msg->federate, msg->federation);
	}
	else {
		auditServer << "OFF" ;

		federations.removeConstrained(msg->federation,
				msg->federate);
		D.Out(pdTerm, "Federate %u of Federation %u is no more constrained.",
				msg->federate, msg->federation);
	}

	G.Out(pdGendoc,"END ** SET TIME CONSTRAINED **");
	G.Out(pdGendoc,"exit RTIG::processSetTimeConstrained");
}

// ----------------------------------------------------------------------------
//! processMessageNull.
void
RTIG::processMessageNull(NetworkMessage *msg)
{
	auditServer << "Date " << msg->getDate().getTime();

	// Catch all exceptions because RTIA does not expect an answer anyway.
	try {
		federations.updateRegulator(msg->federation,
				msg->federate,
				msg->getDate());
	} catch (Exception &e) {}
}

// ----------------------------------------------------------------------------
//! processRegisterSynchronization.
void
RTIG::processRegisterSynchronization(Socket *link, NM_Register_Federation_Synchronization_Point *req)
{

	G.Out(pdGendoc,"BEGIN ** REGISTER FEDERATION SYNCHRONIZATION POINT Service **");
	G.Out(pdGendoc,"enter RTIG::processRegisterSynchronization");

	auditServer << "Label \"" << req->getLabel() << "\" registered. Tag is \""
			<< req->getTag() << "\"" ;

	// boolean true means a federates set exists
	if ( req->getExists() ) {
		federations.manageSynchronization(req->federation,
				req->federate,
				true,
				req->getLabel(),
				req->getTag(),
				req->getFederatesSize(),
				req->getFederates());
	}
	else {
		federations.manageSynchronization(req->federation,
				req->federate,
				true,
				req->getLabel(),
				req->getTag());
	}
	D.Out(pdTerm, "Federation %u is now synchronizing.", req->federation);

	// send synchronizationPointRegistrationSucceeded() to federate.
	NM_Synchronization_Point_Registration_Succeeded rep ;
	rep.federate = req->federate ;
	rep.federation = req->federation ;
	rep.setLabel(req->getLabel());

	G.Out(pdGendoc,"      processRegisterSynchronization====> write SPRS to RTIA");

	rep.send(link,NM_msgBufSend);

	// boolean true means a federates set exists
	if ( req->getExists() )
		federations.broadcastSynchronization(req->federation,
				req->federate,
				req->getLabel(),
				req->getTag(),
				req->getFederatesSize(),
				req->getFederates());
	else
		federations.broadcastSynchronization(req->federation,
				req->federate,
				req->getLabel(),
				req->getTag());

	G.Out(pdGendoc,"exit  RTIG::processRegisterSynchronization");
	G.Out(pdGendoc,"END   ** REGISTER FEDERATION SYNCHRONIZATION POINT Service **");

}

// ----------------------------------------------------------------------------
//! processSynchronizationAchieved.
void
RTIG::processSynchronizationAchieved(Socket *, NetworkMessage *req)
{
	auditServer << "Label \"" << req->getLabel() << "\" ended." ;

	federations.manageSynchronization(req->federation,
			req->federate,
			false,
			req->getLabel(),
			"");
	D.Out(pdTerm, "Federate %u has synchronized.", req->federate);
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
		federations.requestFederationSave(req->federation, req->federate,
				req->getLabel(), req->getDate());
	else
		// Without time
		federations.requestFederationSave(req->federation, req->federate,
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
	G.Out(pdGendoc,"processFederateSaveBegun federation = %d",req->federation);

	auditServer << "Federate " << req->federate << " save begun." ;

	federations.federateSaveBegun(req->federation, req->federate);

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

	auditServer << "Federate " << req->federate << " save ended." ;

	bool status = req->getMessageType() == NetworkMessage::FEDERATE_SAVE_COMPLETE ;
	federations.federateSaveStatus(req->federation, req->federate, status);

	G.Out(pdGendoc,"exit  END   ** FEDERATE SAVE (NOT) COMPLETE SERVICE **");
	G.Out(pdGendoc,"exit  RTIG::processFederateSaveStatus");
}

// ----------------------------------------------------------------------------
void
RTIG::processRequestFederationRestore(Socket *, NetworkMessage *req)
{
	G.Out(pdGendoc,"BEGIN ** REQUEST FEDERATION RESTORE SERVICE **");
	G.Out(pdGendoc,"enter RTIG::processRequestFederationRestore");
	auditServer << "Federate " << req->federate << " request restore." ;

	federations.requestFederationRestore(req->federation, req->federate,
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
	auditServer << "Federate " << req->federate << " restore ended." ;

	bool status = req->getMessageType() == NetworkMessage::FEDERATE_RESTORE_COMPLETE ;

	federations.federateRestoreStatus(req->federation, req->federate, status);

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

	federations.publishObject(req->federation,
			req->federate,
			req->getObjectClass(),
			req->getAttributes(),
			req->getAttributesSize(),
			pub);

	D.Out(pdRegister, "Federate %u of Federation %u published object class %d.",
			req->federate, req->federation, req->getObjectClass());


	if (pub) {
		NM_Publish_Object_Class POC;
		POC.federate = req->federate;
		POC.setObjectClass(req->getObjectClass());
		POC.send(link,NM_msgBufSend); // send answer to RTIA
	} else {
		NM_Unpublish_Object_Class UOC;
		UOC.federate = req->federate;
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

	std::vector <AttributeHandle> arrayVide ;
	bool sub = (req->getMessageType() == NetworkMessage::SUBSCRIBE_OBJECT_CLASS);

	auditServer << "Subscribe Object Class = " << req->getObjectClass()
						<< ", # of att. = " << req->getAttributesSize() ;

	federations.subscribeObject(req->federation,
			req->federate,
			req->getObjectClass(),
			sub ? req->getAttributes() : arrayVide,
					sub ? req->getAttributesSize() : 0);

	D.Out(pdRegister,
			"Federate %u of Federation %u subscribed to object class %d.",
			req->federate, req->federation, req->getObjectClass());

	if (sub) {
		NM_Subscribe_Object_Class rep;
		rep.federate = req->federate ;
		rep.setObjectClass(req->getObjectClass());

		rep.send(link,NM_msgBufSend); // send answer to RTIA
	} else {
		NM_Unsubscribe_Object_Class rep;
		rep.federate = req->federate ;
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
	federations.publishInteraction(req->federation,
			req->federate,
			req->getInteractionClass(),
			pub);
	D.Out(pdRequest, "Federate %u of Federation %u publishes Interaction %d.",
			req->federate,
			req->federation,
			req->getInteractionClass());

	NM_Publish_Interaction_Class rep;

	rep.federate = req->federate ;
	rep.setInteractionClass(req->getInteractionClass());

	rep.send(link,NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processSubscribeInteractionClass
void
RTIG::processSubscribeInteractionClass(Socket *link, NM_Subscribe_Interaction_Class *req)
{
	bool sub = (req->getMessageType() == NetworkMessage::SUBSCRIBE_INTERACTION_CLASS);

	auditServer << "Subscribe Interaction Class = " << req->getInteractionClass() ;
	federations.subscribeInteraction(req->federation,
			req->federate,
			req->getInteractionClass(),
			sub);
	D.Out(pdRequest,
			"Federate %u of Federation %u subscribed to Interaction %d.",
			req->federate,
			req->federation,
			req->getInteractionClass());

	NM_Subscribe_Interaction_Class rep;
	rep.federate = req->federate ;
	rep.setInteractionClass(req->getInteractionClass());

	rep.send(link,NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processRegisterObject
void
RTIG::processRegisterObject(Socket *link, NM_Register_Object *req)
{
	NM_Register_Object rep;

	auditServer << "Register Object Class = " << req->getObjectClass() ;
	rep.setObject(federations.registerObject(req->federation,
			req->federate,
			req->getObjectClass(),
			req->getLabel()));

	D.Out(pdRegister,
			"Object \"%s\" of Federate %u has been registered under ID %u.",
			req->getLabel().c_str(), req->federate, rep.getObject());

	rep.federate    = req->federate ;
	rep.federation  = req->federation;
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
		federations.updateAttribute(req->federation,
				req->federate,
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
		federations.updateAttribute(req->federation,
				req->federate,
				req->getObject(),
				req->getAttributes(),
				req->getValues(),
				req->getAttributesSize(),
				req->getLabel());
	}

	// Building answer (Network Message)
	NM_Update_Attribute_Values rep ;
	rep.federate = req->federate ;
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
	if ( req->isDated() )
	{
		federations.updateParameter(req->federation,
				req->federate,
				req->getInteractionClass(),
				req->getParameters(),
				req->getValues(),
				req->getParametersSize(),
				req->getDate(),
				req->getRegion(),
				req->getLabel());
	}
	else
	{
		federations.updateParameter(req->federation,
				req->federate,
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
	rep.federate = req->federate ;
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
		federations.destroyObject(req->federation,
				req->federate,
				req->getObject(),
				req->getDate(),
				req->getLabel());
	}
	else {
		federations.destroyObject(req->federation,
				req->federate,
				req->getObject(),
				req->getLabel());
	}

	D.Out(pdRegister, "Object # %u of Federation %u has been deleted.",
			req->getObject(), req->federation);

	NM_Delete_Object rep ;
	rep.federate = req->federate ;
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

	federations.searchOwner(req->federation,
			req->federate,
			req->getObject(),
			req->getAttribute());

	D.Out(pdDebug, "Owner of Attribute %u of Object %u .",
			req->getAttribute(), req->getObject());

	NM_Query_Attribute_Ownership rep ;
	rep.federate = req->federate ;
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

	if (federations.isOwner(req->federation,
			req->federate,
			req->getObject(),
			req->getAttribute()))
		rep.setLabel("RTI_TRUE");
	else
		rep.setLabel("RTI_FALSE");

	D.Out(pdDebug, "Owner of Attribute %u of Object %u .",
			req->getAttribute(), req->getObject());

	rep.federate = req->federate ;
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
	federations.negotiateDivestiture(req->federation,
			req->federate,
			req->getObject(),
			req->getAttributes(),
			req->getAttributesSize(),
			req->getLabel());

	D.Out(pdDebug, "Federate %u of Federation %u negotiate "
			"divestiture of object %u.",
			req->federate, req->federation, req->getObject());

	NM_Negotiated_Attribute_Ownership_Divestiture rep ;
	rep.federate = req->federate ;
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

	federations.acquireIfAvailable(req->federation,
			req->federate,
			req->getObject(),
			req->getAttributes(),
			req->getAttributesSize());

	D.Out(pdDebug,
			"Federate %u of Federation %u acquisitionIfAvailable "
			"of object %u.",
			req->federate, req->federation, req->getObject());

	NM_Attribute_Ownership_Acquisition_If_Available rep ;
	rep.federate = req->federate ;
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

	federations.divest(req->federation,
			req->federate,
			req->getObject(),
			req->getAttributes(),
			req->getAttributesSize());

	D.Out(pdDebug,
			"Federate %u of Federation %u UnconditionalDivestiture "
			"of object %u.",
			req->federate, req->federation, req->getObject());

	NM_Unconditional_Attribute_Ownership_Divestiture rep ;
	rep.federate = req->federate ;
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

	federations.acquire(req->federation,
			req->federate,
			req->getObject(),
			req->getAttributes(),
			req->getAttributesSize(),
			req->getLabel());

	D.Out(pdDebug,
			"Federate %u of Federation %u ownership acquisition of object %u.",
			req->federate, req->federation, req->getObject());

	NM_Attribute_Ownership_Acquisition rep ;
	rep.federate = req->federate ;
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

	federations.cancelDivestiture(req->federation,
			req->federate,
			req->getObject(),
			req->getAttributes(),
			req->getAttributesSize());

	D.Out(pdDebug, "Federate %u of Federation %u cancel negotiate "
			"divestiture of object %u.",
			req->federate, req->federation, req->getObject());

	NM_Cancel_Negotiated_Attribute_Ownership_Divestiture rep ;
	rep.federate = req->federate ;
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
			federations.respondRelease(req->federation,
					req->federate,
					req->getObject(),
					req->getAttributes(),
					req->getAttributesSize());

	D.Out(pdDebug, "Federate %u of Federation %u release response "
			"of object %u.",
			req->federate, req->federation, req->getObject());

	NM_Attribute_Ownership_Release_Response rep ;
	rep.setAttributesSize(attributes->size()) ;

	for (uint32_t i = 0 ; i < attributes->size(); ++i) {
		rep.setAttributes(attributes->getHandle(i),i);
	}

	rep.federate = req->federate ;
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

	federations.cancelAcquisition(req->federation,
			req->federate,
			req->getObject(),
			req->getAttributes(),
			req->getAttributesSize());

	D.Out(pdDebug,
			"Federate %u of Federation %u release response of object %u.",
			req->federate, req->federation, req->getObject());

	NM_Cancel_Attribute_Ownership_Acquisition rep ;
	rep.federate = req->federate ;
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
	rep.setRegion(federations.createRegion(req->federation,
			req->federate,
			req->getSpace(),
			req->getNbExtents()));

	Debug(D, pdDebug) << "Federate " << req->federate << " of Federation "
			<< req->federation << " creates region " << rep.getRegion()
			<< endl ;

	rep.federate = req->federate ;
	rep.send(link,NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processModifyRegion
void
RTIG::processModifyRegion(Socket *link, NM_DDM_Modify_Region *req)
{
	Debug(D, pdDebug) << "Federate " << req->federate << " of Federation "
			<< req->federation << " modifies region " << req->getRegion()
			<< endl ;

	federations.modifyRegion(req->federation, req->federate,
			req->getRegion(), req->getExtents());

	NM_DDM_Modify_Region rep ;
	rep.federate = req->federate ;
	rep.send(link,NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processDeleteRegion
void
RTIG::processDeleteRegion(Socket *link, NM_DDM_Delete_Region *req)
{
	// TODO: audit...

	Debug(D, pdDebug) << "Federate " << req->federate << " of Federation "
			<< req->federation << " deletes region " << req->getRegion()
			<< endl ;

	federations.deleteRegion(req->federation, req->federate, req->getRegion());

	NM_DDM_Delete_Region rep;

	rep.federate = req->federate ;
	rep.setRegion(req->getRegion());
	rep.send(link,NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processAssociateRegion
void
RTIG::processAssociateRegion(Socket *link, NM_DDM_Associate_Region *req)
{
	// TODO: audit...

	Debug(D, pdDebug) << "Federate " << req->federate << " of Federation "
			<< req->federation << " associates region " << req->getRegion()
			<< " to some attributes of object " << req->getObject() << endl ;

	federations.associateRegion(req->federation, req->federate, req->getObject(),
			req->getRegion(), req->getAttributesSize(),
			req->getAttributes());

	NM_DDM_Associate_Region rep ;
	rep.federate = req->federate ;
	rep.send(link,NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processUnassociateRegion
void
RTIG::processUnassociateRegion(Socket *link, NM_DDM_Unassociate_Region *req)
{
	// TODO: audit...

	federations.unassociateRegion(req->federation, req->federate,
			req->getObject(), req->getRegion());

	Debug(D, pdDebug) << "Federate " << req->federate << " of Federation "
			<< req->federation << " associates region " << req->getRegion()
			<< " from object " << req->getObject() << endl ;

	NM_DDM_Unassociate_Region rep ;
	rep.federate = req->federate ;
	rep.send(link,NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processSubscribeAttributes
void
RTIG::processSubscribeAttributesWR(Socket *link, NM_DDM_Subscribe_Attributes *req)
{
	// TODO: audit...
	Debug(D, pdDebug) << "Federate " << req->federate << " of Federation "
			<< req->federation << " subscribes with region " << req->getRegion()
			<< " to some attributes of class " << req->getObjectClass() << endl ;

	federations.subscribeAttributesWR(req->federation, req->federate,
			req->getObjectClass(), req->getRegion(),
			req->getAttributesSize(), req->getAttributes());

	NM_DDM_Subscribe_Attributes rep ;
	rep.federate = req->federate ;
	rep.setObjectClass(req->getObjectClass());
	rep.send(link,NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processUnsubscribeAttributes
void
RTIG::processUnsubscribeAttributesWR(Socket *link, NM_DDM_Unsubscribe_Attributes *req)
{
	// TODO: audit...
	Debug(D, pdDebug) << "Federate " << req->federate << " of Federation "
			<< req->federation << " unsubscribes with region " << req->getRegion()
			<< " from object class " << req->getObjectClass() << endl ;

	federations.unsubscribeAttributesWR(req->federation, req->federate,
			req->getObjectClass(), req->getRegion());

	NM_DDM_Unsubscribe_Attributes rep ;
	rep.federate = req->federate ;
	rep.send(link,NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processSubscribeInteractions
void
RTIG::processSubscribeInteractionWR(Socket *link, NM_DDM_Subscribe_Interaction *req)
{
	// TODO: audit...

	federations.subscribeInteractionWR(req->federation, req->federate,
			req->getInteractionClass(), req->getRegion());

	Debug(D, pdDebug) << "Federate " << req->federate << " of Federation "
			<< req->federation << " subscribes with region " << req->getRegion()
			<< " to interaction class " << req->getInteractionClass() << endl ;

	NM_DDM_Subscribe_Interaction rep ;
	rep.federate = req->federate ;
	rep.send(link,NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processUnsubscribeInteractions
void
RTIG::processUnsubscribeInteractionWR(Socket *link, NM_DDM_Unsubscribe_Interaction *req)
{
	// TODO: audit...

	federations.unsubscribeInteractionWR(req->federation, req->federate,
			req->getInteractionClass(), req->getRegion());

	Debug(D, pdDebug) << "Federate " << req->federate << " of Federation "
			<< req->federation << " unsubscribes with region " << req->getRegion()
			<< " from interaction class " << req->getInteractionClass() << endl ;

	NM_DDM_Unsubscribe_Interaction rep ;
	rep.federate = req->federate ;
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
	rep.setObject(federations.registerObjectWithRegion(req->federation,
			req->federate,
			req->getObjectClass(),
			req->getLabel(),
			req->getRegion(),
			req->getAttributesSize(),
			req->getAttributes()));

	D.Out(pdRegister,
			"Object \"%s\" of Federate %u has been registered under ID %u.",
			req->getLabel().c_str(), req->federate, rep.getObject());

	rep.federate = req->federate ;
	rep.send(link,NM_msgBufSend); // Send answer to RTIA
}

// ----------------------------------------------------------------------------
// processRequestObjectAttributeValueUpdate
void
RTIG::processRequestObjectAttributeValueUpdate(Socket *link, NM_Request_Object_Attribute_Value_Update *request)
{
	NM_Request_Object_Attribute_Value_Update answer ;
	Handle federateOwner ;  // federate owner of the object
	G.Out(pdGendoc,"enter RTIG::processRequestObjectAttributeValueUpdate");
	G.Out(pdGendoc,"BEGIN ** REQUEST OBJECT ATTRIBUTE VALUE UPDATE **");

	auditServer << "ObjID = " << request->getObject() ;

	// We have to do verifications about this object and we need owner
	answer.setException(e_NO_EXCEPTION);
	try
	{
		federateOwner = federations.requestObjectOwner(request->federation,
				request->federate,
				request->getObject(),
				request->getAttributes(),
				request->getAttributesSize());
	}
	catch (ObjectNotKnown e)
	{
		answer.setException(e_ObjectNotKnown,e._reason);
	}
	catch (FederationExecutionDoesNotExist e)
	{
		answer.setException(e_FederationExecutionDoesNotExist,e._reason);
	}
	catch (RTIinternalError e)
	{
		answer.setException(e_RTIinternalError,e._reason);
	}

	answer.federate = request->federate ;
	answer.setObject(request->getObject());

	answer.send(link,NM_msgBufSend); // Send answer to RTIA
	G.Out(pdGendoc,"exit  RTIG::processRequestObjectAttributeValueUpdate");
	G.Out(pdGendoc,"END   ** REQUEST OBJECT ATTRIBUTE VALUE UPDATE **");
}

}} // namespace certi/rtig

// $Id: RTIG_processing.cc,v 3.97 2010/03/20 16:34:13 erk Exp $
