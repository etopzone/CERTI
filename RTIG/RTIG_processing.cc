// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: RTIG_processing.cc,v 3.4 2003/01/29 18:11:10 breholee Exp $
// ---------------------------------------------------------------------------

#include "RTIG.hh"

namespace certi {
namespace rtig {

static pdCDebug D("RTIG", "(RTIG)- ");

// ---------------------------------------------------------------------------
//! Creates a new federation.
void 
RTIG::processCreateFederation(Socket *link, NetworkMessage *req)
{
    char *federation = req->NomFederation;

    if (federation == NULL) throw RTIinternalError("Invalid Federation Name.");

    auditServer->addToLinef("Federation Name : %s", federation);

#ifdef FEDERATION_USES_MULTICAST
    // multicast base address
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
  ClientSockets.push_front(com_mc);

#else
  federations->createFederation(federation, nextFederationHandle); 
#endif

  nextFederationHandle++;

  // Prepare answer for RTIA
  NetworkMessage rep;
  rep.Type = m_CREATE_FEDERATION_EXECUTION;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFederation = nextFederationHandle;

  rep.write(link); // Send answer to RTIA

  D.Out(pdInit, "Federation \"%s\" created with Handle %d.",
	federation, rep.NumeroFederation);
}

// ---------------------------------------------------------------------------
//! Add a new federate to federation.
void 
RTIG::processJoinFederation(Socket *link, NetworkMessage *req)
{
    char *federation = req->NomFederation ;
    char *federate = req->NomFedere ;

    unsigned int peer = req->BestEffortPeer ;
    unsigned long address = req->BestEffortAddress ;

    FederationHandle num_federation ;
    FederateHandle num_federe ;

    int nb_regulateurs ;
    int nb_federes ;
    bool pause ;

    if ((federation == NULL) || (federate == NULL))
        throw RTIinternalError("Invalid Federation/Federate Name.");

    auditServer->addToLinef("Federate \"%s\" joins Federation \"%s\"",
                            federate, federation);

    federations->exists(federation, num_federation);

    num_federe = federations->addFederate(num_federation,
                                          federate,
                                          (SecureTCPSocket *) link);

#ifdef FEDERATION_USES_MULTICAST
    SocketMC *com_mc = NULL;

    federations->info(num_federation, nb_federes, nb_regulateurs, 
                      pause, com_mc);
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
 
    auditServer->addToLinef("(%d)with handle %d. Socket %d",
                            num_federation, num_federe,
                            link->returnSocket());

    // Prepare answer
    NetworkMessage rep;
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

    D.Out(pdInit, "Federate \"%s\" has joined Federation %u under handle %u.",
          federate, num_federation, num_federe);

    // Send answer
    rep.write(link);
}
 
// ---------------------------------------------------------------------------
//! Removes a federate from federation.
void 
RTIG::processResignFederation(FederationHandle federation, 
			      FederateHandle federe)
{
  federations->remove(federation, federe);
  D.Out(pdInit, "Federate %u is resigning from federation %u.",
	federe, federation);
}

// ---------------------------------------------------------------------------
//! Removes a federation.
void 
RTIG::processDestroyFederation(Socket *link, NetworkMessage *req)
{
  FederationHandle num_federation;
  char *federation = req->NomFederation;

  if(federation == NULL) throw RTIinternalError("Invalid Federation Name.");

  auditServer->addToLinef("Name \"%s\"", federation);
  federations->exists(federation, num_federation);
  federations->destroyFederation(num_federation);
  D.Out(pdInit, "Federation \"%s\" has been destroyed.", federation);

  NetworkMessage rep;
  rep.Type = m_DESTROY_FEDERATION_EXECUTION;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;

  rep.write(link);
}

// ---------------------------------------------------------------------------
//! Put federate as time regulating.
void 
RTIG::processSetTimeRegulating(NetworkMessage *msg)
{
  if(msg->EstRegulateur){
    auditServer->addToLinef("ON at time %f", msg->Date);

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
//! Put federate as time constrained
void 
RTIG::processSetTimeConstrained(NetworkMessage *msg)
{
  if(msg->EstContraint){
    auditServer->addToLinef("ON at time %f", msg->Date);

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
//! processMessageNull.
void 
RTIG::processMessageNull(NetworkMessage *msg)
{
  auditServer->addToLinef("Date %f", msg->Date);

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
  auditServer->addToLinef("Label \"%s\"", req->Label);
  federations->setPause(req->NumeroFederation,
			req->NumeroFedere,
			true,
			req->Label);
  D.Out(pdTerm, "Federation %u is now paused.", req->NumeroFederation);

  NetworkMessage rep;
  rep.Type = m_REQUEST_PAUSE;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;

  rep.write(link);
}

// ---------------------------------------------------------------------------
// processRequestResume
void 
RTIG::processRequestResume(Socket*, NetworkMessage *msg)
{
  auditServer->addToLinef("Label \"%s\"", msg->Label);
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
  bool pub = (req->Type == m_PUBLISH_OBJECT_CLASS) ;

  auditServer->addToLinef("Class = %u, # of att. = %u",
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

  NetworkMessage rep;
  rep.Type = req->Type;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.objectClassHandle = req->objectClassHandle;
  rep.HandleArraySize = 0;

  rep.write(link); // send answer to RTIA
}

// ---------------------------------------------------------------------------
// processSubscribeObjectClass
void 
RTIG::processSubscribeObjectClass(Socket *link, NetworkMessage *req)
{
  bool sub = (req->Type == m_SUBSCRIBE_OBJECT_CLASS) ;
 
  auditServer->addToLinef("Class = %u, # of att. = %u",
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

  NetworkMessage rep;
  rep.Type = req->Type;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.objectClassHandle = req->objectClassHandle;
  rep.HandleArraySize = 0;

  rep.write(link); // send answer to RTIA
}

// ---------------------------------------------------------------------------
// processPublishInteractionClass
void 
RTIG::processPublishInteractionClass(Socket *link, NetworkMessage *req)
{
  assert(link != NULL && req != NULL);

  bool pub = (req->Type == m_PUBLISH_INTERACTION_CLASS) ;
 
  auditServer->addToLinef("Class = %u", req->InteractionHandle);
  federations->publishInteraction(req->NumeroFederation, 
				  req->NumeroFedere, 
				  req->InteractionHandle, 
				  pub);
  D.Out(pdRequest, "Federate %u of Federation %u publishes Interaction %d.",
        req->NumeroFedere, 
        req->NumeroFederation, 
        req->InteractionHandle);

  NetworkMessage rep;
  rep.Type = req->Type;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.InteractionHandle = req->InteractionHandle;

  rep.write(link); // send answer to RTIA
}

// ---------------------------------------------------------------------------
// processSubscribeInteractionClass
void 
RTIG::processSubscribeInteractionClass(Socket *link, NetworkMessage *req)
{
  bool sub = (req->Type == m_SUBSCRIBE_INTERACTION_CLASS) ;

  auditServer->addToLinef("Class = %u", req->InteractionHandle);
  federations->subscribeInteraction(req->NumeroFederation, 
				    req->NumeroFedere,
				    req->InteractionHandle,
				    sub);
  D.Out(pdRequest, 
	"Federate %u of Federation %u subscribed to Interaction %d.",
	req->NumeroFedere, 
	req->NumeroFederation, 
	req->InteractionHandle);

  NetworkMessage rep;
  rep.Type = req->Type;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.InteractionHandle = req->InteractionHandle;

  rep.write(link); // send answer to RTIA
}

// ---------------------------------------------------------------------------
// processRequestId
void 
RTIG::processRequestId(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;

  auditServer->addToLinef("Asked %u", req->IDcount);
  federations->requestId(req->NumeroFederation, 
                         req->IDcount, 
                         rep.FirstID, 
                         rep.LastID);
  auditServer->addToLinef(", given range from %u to %u",
			 rep.FirstID, rep.LastID);

  D.Out(pdInit, "%d IDs have been sent for Federation %u.",
	req->IDcount, req->NumeroFederation);

  rep.Type = req->Type;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;

  rep.write(link); // send answer to RTIA
}

// ---------------------------------------------------------------------------
// processRegisterObject
void 
RTIG::processRegisterObject(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;

  auditServer->addToLinef("Class = %u", req->objectClassHandle);
  rep.Objectid = federations->registerObject(req->NumeroFederation, 
                                             req->NumeroFedere, 
                                             req->objectClassHandle,
                                             req->Label);
  auditServer->addToLinef(", Handle = %u", rep.Objectid);

  D.Out(pdRegister, 
	"Object \"%s\" of Federate %u has been registered under ID %u.",
	req->Label, req->NumeroFedere, rep.Objectid);

  rep.Type = req->Type;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  // rep.Objectid is set by the call of registerObject

  rep.write(link); // Send answer to RTIA
}

// ---------------------------------------------------------------------------
// processUpdateAttributeValues
void 
RTIG::processUpdateAttributeValues(Socket *link, NetworkMessage *req)
{
  AttributeValue *ValueArray = NULL;

  auditServer->addToLinef("ObjID = %u, Date = %f", req->Objectid, req->Date);

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
  NetworkMessage rep;
  rep.Type = m_UPDATE_ATTRIBUTE_VALUES;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;
  rep.Date = req->Date;
  rep.HandleArraySize = 0;

  rep.write(link); // send answer to RTIA
}

// ---------------------------------------------------------------------------
// processSendInteraction
void 
RTIG::processSendInteraction(Socket *link, NetworkMessage *req)
{
  AttributeValue *values = NULL;

  // Prepare le Value Array
  auditServer->addToLinef("IntID = %u, Date = %f",
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

  NetworkMessage rep;
  rep.Type = m_SEND_INTERACTION;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.InteractionHandle = req->InteractionHandle;
  rep.HandleArraySize = 0;

  rep.write(link); // send answer to RTIA
}

// ---------------------------------------------------------------------------
// processdeleteObject
void 
RTIG::processDeleteObject(Socket *link, NetworkMessage *req)
{
  auditServer->addToLinef("ObjID = %u", req->Objectid);

  federations->destroyObject(req->NumeroFederation,
			     req->NumeroFedere,
			     req->Objectid,
			     req->Label);

  D.Out(pdRegister, "Object # %u of Federation %u has been deleted.",
	req->Objectid, req->NumeroFederation);

  NetworkMessage rep;
  rep.Type = m_DELETE_OBJECT;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;

  rep.write(link); // send answer to RTIA
}

// ---------------------------------------------------------------------------
// processqueryAttributeOwnership
void 
RTIG::processQueryAttributeOwnership(Socket *link, NetworkMessage *req)
{
  D.Out(pdDebug, "Owner of Attribute %u of Object %u .",
        req->HandleArray[0], req->Objectid); 

  auditServer->addToLinef("AttributeHandle = %u", req->HandleArray[0]);

  federations->searchOwner(req->NumeroFederation,
			   req->NumeroFedere,
			   req->Objectid,
			   req->HandleArray[0]);

  D.Out(pdDebug, "Owner of Attribute %u of Object %u .",
        req->HandleArray[0], req->Objectid);

  NetworkMessage rep;
  rep.Type = m_QUERY_ATTRIBUTE_OWNERSHIP;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;

  rep.write(link); // send answer to RTIA
}

// ---------------------------------------------------------------------------
// processattributeOwnedByFederate
void 
RTIG::processAttributeOwnedByFederate(Socket *link, NetworkMessage *req)
{
  NetworkMessage rep;

  D.Out(pdDebug, "Owner of Attribute %u of Object %u .",
        req->HandleArray[0], req->Objectid); 

  auditServer->addToLinef("AttributeHandle = %u", req->HandleArray[0]);

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

  rep.write(link); // send answer to RTIA
}

// ---------------------------------------------------------------------------
// processNegotiatedOwnershipDivestiture
void 
RTIG::processNegotiatedOwnershipDivestiture(Socket *link, NetworkMessage *req)
{
  auditServer->addToLinef("Object = %u, # of att. = %u",
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

  NetworkMessage rep;
  rep.Type = m_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;
  rep.HandleArraySize = 0;

  rep.write(link); // send answer to RTIA
}

// ---------------------------------------------------------------------------
// processAcquisitionIfAvailable
void 
RTIG::processAcquisitionIfAvailable(Socket *link, NetworkMessage *req)
{
  auditServer->addToLinef("Object = %u, # of att. = %u",
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

  NetworkMessage rep;
  rep.Type = m_ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;
  rep.HandleArraySize = 0;

  rep.write(link); // send answer to RTIA
}

// ---------------------------------------------------------------------------
// processUnconditionalDivestiture
void 
RTIG::processUnconditionalDivestiture(Socket *link, NetworkMessage *req)
{
  auditServer->addToLinef("Object = %u, # of att. = %u",
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

  NetworkMessage rep;
  rep.Type = m_UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;
  rep.HandleArraySize = 0;

  rep.write(link); // send answer to RTIA
}

// ---------------------------------------------------------------------------
// processOwnershipAcquisition
void 
RTIG::processOwnershipAcquisition(Socket *link, NetworkMessage *req)
{
  auditServer->addToLinef("Object = %u, # of att. = %u",
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

  NetworkMessage rep;
  rep.Type = m_ATTRIBUTE_OWNERSHIP_ACQUISITION;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;
  rep.HandleArraySize = 0;

  rep.write(link); // send answer to RTIA
}

// ---------------------------------------------------------------------------
// processCancelNegotiatedDivestiture
void 
RTIG::processCancelNegotiatedDivestiture(Socket *link, NetworkMessage *req)
{
  auditServer->addToLinef("Object = %u, # of att. = %u",
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

  NetworkMessage rep;
  rep.Type = m_CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;
  rep.HandleArraySize = 0;

  rep.write(link); // send answer to RTIA
}

// ---------------------------------------------------------------------------
// processReleaseResponse
void 
RTIG::processReleaseResponse(Socket *link, NetworkMessage *req)
{
  auditServer->addToLinef("Object = %u, # of att. = %u",
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

  NetworkMessage rep;
  rep.HandleArraySize = attributes->size();
 
  for(unsigned int i = 0; i < attributes->size(); i++) {
    rep.HandleArray[i] = attributes->getHandle(i);
  }
 
  rep.Type = m_ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;

  rep.write(link); // Send answer to RTIA
}

// ---------------------------------------------------------------------------
// processCancelAcquisition
void 
RTIG::processCancelAcquisition(Socket *link, NetworkMessage *req)
{
  auditServer->addToLinef("Object = %u, # of att. = %u",
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

  NetworkMessage rep; 
  rep.Type = m_CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION;
  rep.Exception = e_NO_EXCEPTION;
  rep.NumeroFedere = req->NumeroFedere;
  rep.Objectid = req->Objectid;
  rep.HandleArraySize = 0; 

  rep.write(link); // send answer to RTIA
}

}}

// $Id: RTIG_processing.cc,v 3.4 2003/01/29 18:11:10 breholee Exp $
