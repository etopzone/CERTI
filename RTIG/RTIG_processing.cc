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
// $Id: RTIG_processing.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#include "RTIG.hh"

namespace certi {
namespace rtig {

static pdCDebug D("RTIG", "(RTIG)- ");

// ---------------------------------------------------------------------------
// processCreatefederation

void 
RTIG::processCreatefederation(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;
  FederationExecutionName federation = req->NomFederation;

  if(federation == NULL) throw RTIinternalError("Invalid Federation Name.");

  auditServer->addToLine("Federation Name : %s", federation);

#ifdef FEDERATION_USES_MULTICAST
  // adresse de base pour le multicast
  unsigned long base_adr_mc = inet_addr(ADRESSE_MULTICAST);
  SocketMC *com_mc = NULL;

  // creer la communication multicast 
  com_mc = new SocketMC();
  if(com_mc == NULL){
    D.Out(pdExcept, "Unable to allocate Multicast socket.");
    throw RTIinternalError("Unable to allocate Multicast socket.");
  }

  com_mc->CreerSocketMC(base_adr_mc + nextFederationHandle, MC_PORT);

  // inserer la nouvelle federation dans la liste des federations
  federations->createFederation(federation, nextFederationHandle, com_mc);

  // inserer descripteur fichier pour le prochain appel a un select
  ClientSockets.Inserer(1, com_mc);

#else
  federations->createFederation(federation, nextFederationHandle); 
#endif

  nextFederationHandle++;

  // Prepare answer
  rep.Type = m_CREATE_FEDERATION_EXECUTION;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFederation = nextFederationHandle;

  // Send Answer
  rep.write(link);

  D.Out(pdInit, "Federation \"%s\" created with Handle %d.",
	federation, rep.NumeroFederation);

}

// ---------------------------------------------------------------------------
// processJoinfederation

void 
RTIG::processJoinfederation(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;
  FederationExecutionName federation = req->NomFederation;
  FederateName federate = req->NomFedere;

  unsigned int peer = req->BestEffortPeer;
  unsigned long address = req->BestEffortAddress;

  FederationHandle num_federation;
  FederateHandle num_federe;

  int nb_regulateurs;
  int nb_federes;
  bool pause;

  if((federation == NULL)||(federate == NULL))
    throw RTIinternalError("Invalid Federation/Federate Name.");

  auditServer->addToLine("Federate \"%s\" joins Federation \"%s\"",
			 federate, federation);

  federations->exists(federation, num_federation);

  num_federe = federations->addFederate(num_federation,
					federate,
					(SecureTCPSocket *)link);

#ifdef FEDERATION_USES_MULTICAST
  SocketMC *com_mc = NULL;

  federations->info(num_federation, nb_federes, nb_regulateurs, pause, com_mc);
  assert(com_mc != NULL);
#else
  federations->info(num_federation, nb_federes, nb_regulateurs, pause);
#endif

  // Store Federate <->Socket reference.
  socketServer->setReferences(link->returnSocket(),
			      num_federation,
			      num_federe,
			      address,
			      peer);
 
  auditServer->addToLine("(%d)with handle %d. Socket %d",
			 num_federation, num_federe,
			 link->returnSocket());

  // Prepare answer
  rep.Type = m_JOIN_FEDERATION_EXECUTION;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = num_federe;
  rep.NumeroFederation = num_federation;
  rep.NombreRegulateurs = nb_regulateurs;
  rep.BestEffortPeer = peer;
  rep.BestEffortAddress = address;


#ifdef FEDERATION_USES_MULTICAST
  rep.AdresseMulticast = com_mc->returnAdress();
#endif

  D.Out(pdInit, 
	"Federate \"%s\" has joined Federation %u under handle %u.",
	federate, num_federation, num_federe);

  // Send answer
  rep.write(link);
}
 
// ---------------------------------------------------------------------------
// processResignfederation

void 
RTIG::processResignfederation(FederationHandle federation, 
			      FederateHandle federe)
{
  federations->remove(federation, federe);
  D.Out(pdInit, "Federate %u is resigning from federation %u.",
	federe, federation);
}

// ---------------------------------------------------------------------------
// processDestroyfederation

void 
RTIG::processDestroyfederation(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;
  FederationHandle num_federation;
  FederationExecutionName federation = req->NomFederation;

  if(federation == NULL) throw RTIinternalError("Invalid Federation Name.");
  auditServer->addToLine("Name \"%s\"", federation);
  federations->exists(federation, num_federation);
  federations->destroyFederation(num_federation);
  D.Out(pdInit, "Federation \"%s\" has been destroyed.", federation);

  rep.Type = m_DESTROY_FEDERATION_EXECUTION;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;

  rep.write(link);
}

// ---------------------------------------------------------------------------
// processSetTimeRegulating

void 
RTIG::processSetTimeRegulating(NetworkMessage *msg)
{
  if(msg->EstRegulateur){
    auditServer->addToLine("ON at time %f", msg->Date);

    federations->createRegulator(msg->NumeroFederation,
				 msg->NumeroFedere,
				 msg->Date);
    D.Out(pdTerm, "Federate %u of Federation %u sets TimeRegulation ON.",
	  msg->NumeroFedere, msg->NumeroFederation);
  }
  else {
    auditServer->addToLine("OFF");

    federations->removeRegulator(msg->NumeroFederation,
				 msg->NumeroFedere);
    D.Out(pdTerm, "Federate %u of Federation %u sets TimeRegulation OFF.",
	  msg->NumeroFedere, msg->NumeroFederation);
  }
}

// ---------------------------------------------------------------------------
// processSetTimeConstrained

void 
RTIG::processSetTimeConstrained(NetworkMessage *msg)
{
  if(msg->EstContraint){
    auditServer->addToLine("ON at time %f", msg->Date);

    federations->addConstrained(msg->NumeroFederation,
				msg->NumeroFedere);
    D.Out(pdTerm, "Federate %u of Federation %u is now constrained.",
	  msg->NumeroFedere, msg->NumeroFederation);
  }
  else {
    auditServer->addToLine("OFF");

    federations->removeConstrained(msg->NumeroFederation,
				   msg->NumeroFedere);
    D.Out(pdTerm, "Federate %u of Federation %u is no more constrained.",
	  msg->NumeroFedere, msg->NumeroFederation);
  }
}

// ---------------------------------------------------------------------------
// processMessageNull

void 
RTIG::processMessageNull(NetworkMessage *msg)
{
  auditServer->addToLine("Date %f", msg->Date);

  // Catch all exceptions because RTIA does not expect an answer anyway.
  try {
    federations->updateRegulator(msg->NumeroFederation,
				 msg->NumeroFedere,
				 msg->Date);
  } catch(Exception &e){}
}

// ---------------------------------------------------------------------------
// processRequestPause

void 
RTIG::processRequestPause(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;

  auditServer->addToLine("Label \"%s\"", req->Label);
  federations->setPause(req->NumeroFederation,
			req->NumeroFedere,
			true,
			req->Label);
  D.Out(pdTerm, "Federation %u is now paused.", req->NumeroFederation);

  rep.Type = m_REQUEST_PAUSE;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;

  rep.write(link);
}

// ---------------------------------------------------------------------------
// processRequestResume

void 
RTIG::processRequestResume(Socket *link, NetworkMessage *msg)
{
  auditServer->addToLine("Label \"%s\"", msg->Label);
  federations->setPause(msg->NumeroFederation,
			msg->NumeroFedere,
			false,
			msg->Label);
  D.Out(pdTerm, "Federation %u has been resumed.", msg->NumeroFederation);
}

// ---------------------------------------------------------------------------
// processPublishObjectClass

void 
RTIG::processPublishObjectClass(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;
  bool pub ;

  pub = (req->Type == m_PUBLISH_OBJECT_CLASS) ;

  auditServer->addToLine("Class = %u, # of att. = %u",
			 req->objectClassHandle, 
			 req->HandleArraySize);

  federations->publishObject(req->NumeroFederation, 
			     req->NumeroFedere, 
			     req->objectClassHandle, 
			     req->HandleArray, 
			     req->HandleArraySize, 
			     pub);

  D.Out(pdRegister, "Federate %u of Federation %u published object class %d.",
	req->NumeroFedere, req->NumeroFederation, req->objectClassHandle);

  rep.Type = req->Type;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.objectClassHandle = req->objectClassHandle;
  rep.HandleArraySize = 0;

  // emettre la reponse au RTIA
  rep.write(link);
}

// ---------------------------------------------------------------------------
// processSubscribeObjectClass

void 
RTIG::processSubscribeObjectClass(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;
  bool sub ;

  sub = (req->Type == m_SUBSCRIBE_OBJECT_CLASS) ;
 
  auditServer->addToLine("Class = %u, # of att. = %u",
			 req->objectClassHandle, 
			 req->HandleArraySize);

  federations->subscribeObject(req->NumeroFederation, 
			       req->NumeroFedere, 
			       req->objectClassHandle, 
			       req->HandleArray, 
			       req->HandleArraySize, 
			       sub);

  D.Out(pdRegister, 
	"Federate %u of Federation %u subscribed to object class %d.",
	req->NumeroFedere, req->NumeroFederation, req->objectClassHandle);

  rep.Type = req->Type;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.objectClassHandle = req->objectClassHandle;
  rep.HandleArraySize = 0;

  // emettre la reponse au RTIA
  rep.write(link);
}

// ---------------------------------------------------------------------------
// processPublishInteractionClass

void 
RTIG::processPublishInteractionClass(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;
  bool pub ;

  assert(link != NULL && req != NULL);
  pub = (req->Type == m_PUBLISH_INTERACTION_CLASS) ;
 
  auditServer->addToLine("Class = %u", req->InteractionHandle);
  federations->publishInteraction(req->NumeroFederation, 
				  req->NumeroFedere, 
				  req->InteractionHandle, 
				  pub);
  D.Out(pdRequest, 
	"Federate %u of Federation %u publishes Interaction %d.",
	req->NumeroFedere, 
	req->NumeroFederation, 
	req->InteractionHandle);

  rep.Type = req->Type;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.InteractionHandle = req->InteractionHandle;

  // emettre la reponse au RTIA
  rep.write(link);
}

// ---------------------------------------------------------------------------
// processSubscribeInteractionClass

void 
RTIG::processSubscribeInteractionClass(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;
  bool sub ;

  sub = (req->Type == m_SUBSCRIBE_INTERACTION_CLASS) ;

  auditServer->addToLine("Class = %u", req->InteractionHandle);
  federations->subscribeInteraction(req->NumeroFederation, 
				    req->NumeroFedere,
				    req->InteractionHandle,
				    sub);
  D.Out(pdRequest, 
	"Federate %u of Federation %u subscribed to Interaction %d.",
	req->NumeroFedere, 
	req->NumeroFederation, 
	req->InteractionHandle);

  rep.Type = req->Type;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.InteractionHandle = req->InteractionHandle;

  // emettre la reponse au RTIA
  rep.write(link);
}

// ---------------------------------------------------------------------------
// processRequestId
// FIXME: change case (processrequestId -> processRequestId)

void 
RTIG::processrequestId(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;

  auditServer->addToLine("Asked %u", req->IDcount);
  federations->requestId(req->NumeroFederation, 
			 req->IDcount, 
			 rep.FirstID, 
			 rep.LastID);
  auditServer->addToLine(", given range from %u to %u",
			 rep.FirstID, rep.LastID);

  D.Out(pdInit, "%d IDs have been sent for Federation %u.",
	req->IDcount, req->NumeroFederation);

  rep.Type = req->Type;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;

  // emettre la reponse au RTIA
  rep.write(link);
}

// ---------------------------------------------------------------------------
// processRegisterObject

void 
RTIG::processRegisterObject(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;

  auditServer->addToLine("Class = %u", req->objectClassHandle);
  rep.Objectid = federations->registerObject(req->NumeroFederation, 
					     req->NumeroFedere, 
					     req->objectClassHandle,
					     req->Label);
  auditServer->addToLine(", Handle = %u", rep.Objectid);

  D.Out(pdRegister, 
	"Object \"%s\" of Federate %u has been registered under ID %u.",
	req->Label, req->NumeroFedere, rep.Objectid);

  rep.Type = req->Type;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  // rep.Objectid is set by the call of registerObject

  // Emettre la reponse au RTIA
  rep.write(link);
}

// ---------------------------------------------------------------------------
// processUpdateAttributeValues
// FIXME: change case (-> processUpdateAttributeValues)

void 
RTIG::processupdateAttributeValues(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;
  AttributeValue *ValueArray = NULL;
  int i;

  auditServer->addToLine("ObjID = %u, Date = %f", req->Objectid, req->Date);

  // Prepare le Value Array
  ValueArray = req->getAttribValueArray();

  // Propage l'appel
  federations->updateAttribute(req->NumeroFederation,
			       req->NumeroFedere,
			       req->Objectid,
			       req->HandleArray,
			       ValueArray,
			       req->HandleArraySize,
			       req->Date,
			       req->Label); 
  free(ValueArray);

  // Prepare la reponse
  rep.Type = m_UPDATE_ATTRIBUTE_VALUES;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;
  rep.Date = req->Date;
  rep.HandleArraySize = 0;

  // emettre la reponse au RTIA
  rep.write(link);
}

// ---------------------------------------------------------------------------
// processsendInteraction
// FIXME: change case

void 
RTIG::processsendInteraction(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;
  AttributeValue *values = NULL;
  int i;

  // Prepare le Value Array
  auditServer->addToLine("IntID = %u, Date = %f",
			 req->InteractionHandle, req->Date);
  values = req->getParamValueArray();
 
  federations->updateParameter(req->NumeroFederation,
			       req->NumeroFedere,
			       req->InteractionHandle,
			       req->HandleArray,
			       values,
			       req->HandleArraySize,
			       req->Date,
			       req->Label);
  free(values);

  D.Out(pdDebug,"Mise A Jour des parametres de l'interaction %d terminee",
	req->InteractionHandle);

  rep.Type = m_SEND_INTERACTION;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.InteractionHandle = req->InteractionHandle;
  rep.HandleArraySize = 0;

  // emettre la reponse au RTIA
  rep.write(link);
}

// ---------------------------------------------------------------------------
// processdeleteObject
// FIXME: change case

void 
RTIG::processdeleteObject(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;

  auditServer->addToLine("ObjID = %u", req->Objectid);

  federations->destroyObject(req->NumeroFederation,
			     req->NumeroFedere,
			     req->Objectid,
			     req->Label);

  D.Out(pdRegister, "Object # %u of Federation %u has been deleted.",
	req->Objectid, req->NumeroFederation);

  rep.Type = m_DELETE_OBJECT;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;

  // emettre la reponse au RTIA
  rep.write(link);
}

// ---------------------------------------------------------------------------
// processqueryAttributeOwnership
// FIXME: change case

void 
RTIG::processqueryAttributeOwnership(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;
 
  D.Out(pdDebug, "Owner of Attribute %u of Object %u .",
	req->HandleArray[0], req->Objectid); 

  auditServer->addToLine("AttributeHandle = %u", req->HandleArray[0]);

  federations->searchOwner(req->NumeroFederation,
			   req->NumeroFedere,
			   req->Objectid,
			   req->HandleArray[0]);

  D.Out(pdDebug, "Owner of Attribute %u of Object %u .",
	req->HandleArray[0], req->Objectid);

  rep.Type = m_QUERY_ATTRIBUTE_OWNERSHIP;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;

  // emettre la reponse au RTIA
  rep.write(link);
}

// ---------------------------------------------------------------------------
// processattributeOwnedByFederate

void 
RTIG::processattributeOwnedByFederate(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;
 
  D.Out(pdDebug, "Owner of Attribute %u of Object %u .",
	req->HandleArray[0], req->Objectid); 

  auditServer->addToLine("AttributeHandle = %u", req->HandleArray[0]);

  if(federations->isOwner(req->NumeroFederation,
			  req->NumeroFedere,
			  req->Objectid,
			  req->HandleArray[0]))
    strcpy(rep.Label, "RTI_TRUE");
  else
    strcpy(rep.Label, "RTI_FALSE"); 

  D.Out(pdDebug, "Owner of Attribute %u of Object %u .",
	req->HandleArray[0], req->Objectid);

  rep.Type = m_IS_ATTRIBUTE_OWNED_BY_FEDERATE;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;

  // emettre la reponse au RTIA
  rep.write(link);
}

// ---------------------------------------------------------------------------
// processNegotiatedOwnershipDivestiture

void 
RTIG::processNegotiatedOwnershipDivestiture(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;

  auditServer->addToLine("Object = %u, # of att. = %u",
			 req->Objectid, 
			 req->HandleArraySize);
  federations->negotiateDivestiture(req->NumeroFederation, 
				    req->NumeroFedere, 
				    req->Objectid, 
				    req->HandleArray, 
				    req->HandleArraySize,
				    req->Label);

  D.Out(pdDebug, "Federate %u of Federation %u negotiate "
	"divestiture of object %u.",
	req->NumeroFedere, req->NumeroFederation, req->Objectid);

  rep.Type = m_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;
  rep.HandleArraySize = 0;

  // emettre la reponse au RTIA
  rep.write(link);
}

// ---------------------------------------------------------------------------
// processAcquisitionIfAvailable

void 
RTIG::processAcquisitionIfAvailable(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;

  auditServer->addToLine("Object = %u, # of att. = %u",
			 req->Objectid, 
			 req->HandleArraySize);

  federations->acquireIfAvailable(req->NumeroFederation, 
				  req->NumeroFedere, 
				  req->Objectid, 
				  req->HandleArray, 
				  req->HandleArraySize);

  D.Out(pdDebug, 
	"Federate %u of Federation %u acquisitionIfAvailable "
	"of object %u.",
	req->NumeroFedere, req->NumeroFederation, req->Objectid);

  rep.Type = m_ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;
  rep.HandleArraySize = 0;

  // emettre la reponse au RTIA
  rep.write(link);
}

// ---------------------------------------------------------------------------
// processUnconditionalDivestiture

void 
RTIG::processUnconditionalDivestiture(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;

  auditServer->addToLine("Object = %u, # of att. = %u",
			 req->Objectid, 
			 req->HandleArraySize);

  federations->divest(req->NumeroFederation, 
		      req->NumeroFedere, 
		      req->Objectid, 
		      req->HandleArray, 
		      req->HandleArraySize);

  D.Out(pdDebug, 
	"Federate %u of Federation %u UnconditionalDivestiture "
	"of object %u.",
	req->NumeroFedere, req->NumeroFederation, req->Objectid);

  rep.Type = m_UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;
  rep.HandleArraySize = 0;

  // emettre la reponse au RTIA
  rep.write(link);
}

// ---------------------------------------------------------------------------
// processOwnershipAcquisition

void 
RTIG::processOwnershipAcquisition(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;

  auditServer->addToLine("Object = %u, # of att. = %u",
			 req->Objectid, 
			 req->HandleArraySize);

  federations->acquire(req->NumeroFederation, 
		       req->NumeroFedere, 
		       req->Objectid, 
		       req->HandleArray, 
		       req->HandleArraySize,
		       req->Label);

  D.Out(pdDebug, 
	"Federate %u of Federation %u ownership acquisition of object %u.",
	req->NumeroFedere, req->NumeroFederation, req->Objectid);

  rep.Type = m_ATTRIBUTE_OWNERSHIP_ACQUISITION;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;
  rep.HandleArraySize = 0;

  // emettre la reponse au RTIA
  rep.write(link);
}

// ---------------------------------------------------------------------------
// processAnnulerNegotiatedDivestiture
// FIXME: fix name

void 
RTIG::processAnnulerNegotiatedDivestiture(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;

  auditServer->addToLine("Object = %u, # of att. = %u",
			 req->Objectid, 
			 req->HandleArraySize);

  federations->cancelDivestiture(req->NumeroFederation, 
				 req->NumeroFedere, 
				 req->Objectid, 
				 req->HandleArray, 
				 req->HandleArraySize);
  
  D.Out(pdDebug, "Federate %u of Federation %u cancel negotiate "
	"divestiture of object %u.",
	req->NumeroFedere, req->NumeroFederation, req->Objectid);

  rep.Type = m_CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;
  rep.HandleArraySize = 0;

  // emettre la reponse au RTIA
  rep.write(link);
}

// ---------------------------------------------------------------------------
// processRealeaseResponse
// FIXME: fix name

void 
RTIG::processRealeaseResponse(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;

  auditServer->addToLine("Object = %u, # of att. = %u",
			 req->Objectid, 
			 req->HandleArraySize);

  AttributeHandleSet *attributes = 
    federations->respondRelease(req->NumeroFederation, 
				req->NumeroFedere, 
				req->Objectid, 
				req->HandleArray, 
				req->HandleArraySize);

  D.Out(pdDebug, "Federate %u of Federation %u release response "
	"of object %u.",
	req->NumeroFedere, req->NumeroFederation, req->Objectid);

  rep.HandleArraySize = attributes->size();
 
  for(int i = 0; i < attributes->size(); i++) {
    rep.HandleArray[i] = attributes->getHandle(i);
  }
 
  rep.Type = m_ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;

  // emettre la reponse au RTIA
  rep.write(link);
}

// ---------------------------------------------------------------------------
// processCancelAcquisition

void 
RTIG::processCancelAcquisition(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;

  auditServer->addToLine("Object = %u, # of att. = %u",
			 req->Objectid, 
			 req->HandleArraySize);

  federations->cancelAcquisition(req->NumeroFederation, 
				 req->NumeroFedere, 
				 req->Objectid, 
				 req->HandleArray, 
				 req->HandleArraySize);

  D.Out(pdDebug, 
	"Federate %u of Federation %u release response of object %u.",
	req->NumeroFedere, req->NumeroFederation, req->Objectid);
 
  rep.Type = m_CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;
  rep.HandleArraySize = 0; 

  // emettre la reponse au RTIA
  rep.write(link); 
}

}}

// $Id: RTIG_processing.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $

