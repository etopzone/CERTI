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
// $Id: RTIA_federate.cc,v 3.1 2002/11/26 15:48:01 breholee Exp $
// ---------------------------------------------------------------------------

#include "RTIA.hh"

using namespace certi ;
using namespace fedparser ;

namespace certi {
namespace rtia {

static pdCDebug D("RTIA", "(RTIA ) - ");

// -----------------------------
// -- ChoisirTraitementFedere --
// -----------------------------

void RTIA::chooseFederateProcessing(Message *req,
				    Message &rep,
				    TypeException &e)
{
  e = e_NO_EXCEPTION;

  switch(req->Type) {
 
    // 2.1
  case CREATE_FEDERATION_EXECUTION:
    nb_requetes [0] ++;
    D.Out(pdTrace, "Receving Message from Federate, type CreateFederation.");
 
    _GF->createFederationExecution(req->getFederationName(), e);
    D.Out(pdTrace, "Receving Message from Federate, "
	  "type CreateFederation done.");
 
    break;
 
    // 2.2
  case DESTROY_FEDERATION_EXECUTION:
    nb_requetes [1] ++;
    D.Out(pdTrace, "Receving Message from Federate, type DestroyFederation.");

    _GF->destroyFederationExecution(req->getFederationName(), e);
    break;
 
    // 2.3
  case JOIN_FEDERATION_EXECUTION:
    char *nom_fed;
    FedParser *lecture;

    nb_requetes [2] ++;
    D.Out(pdTrace, "Receving Message from Federate, type JoinFederation.");
 
    rep.NumeroFedere = _GF->joinFederationExecution(req->getFederateName(),
						  req->getFederationName(),
						  e);
 
    nom_fed = new char [MAX_FEDERATION_NAME_LENGTH+4];
    strcpy(nom_fed, req->getFederationName());
    strcat(nom_fed, ".fed");
 
    lecture = new FedParser(_theRootObj);
    lecture->readFile(nom_fed);
 
    delete nom_fed;
    delete lecture;
    break;
 
    // 2.4
  case RESIGN_FEDERATION_EXECUTION:
    nb_requetes [3] ++;
    D.Out(pdTrace, "Receving Message from Federate, type ResignFederation.");
 
    _GF->resignFederationExecution(req->RAction, e);
    break;
 
    // 2.5
  case REQUEST_PAUSE:
    D.Out(pdTrace, "Receving Message from Federate, type RequestPause.");
    nb_requetes [4] ++;

    _GF->requestPause(req->getLabel(), e);
    break;
 
    // 2.7
  case PAUSE_ACHIEVED:
    D.Out(pdTrace, "Receving Message from Federate, type PauseAchieved.");
    nb_requetes [5] ++;

    _GF->pauseAchieved(req->getLabel(), e);
    break;
 
    // 2.8
  case REQUEST_RESUME:
    D.Out(pdTrace, "Receving Message from Federate, type RequestResume.");
    nb_requetes [6] ++;

    _GF->requestResume(req->getLabel(), e);
    break;
 
    // 2.10
  case RESUME_ACHIEVED:
    D.Out(pdTrace, "Receving Message from Federate, type ResumeAchieved.");
    nb_requetes [7] ++;

    _GF->resumeAchieved(e);
    break;
 
    // 2.11
  case REQUEST_FEDERATION_SAVE:
    // unimplemented
    break;
 
    // 2.13
  case FEDERATE_SAVE_BEGUN:
    // unimplemented
    break;
 
    // 2.14(1)
  case FEDERATE_SAVE_ACHIEVED:
    // unimplemented
    break;
 
    // 2.14(2)
  case FEDERATE_SAVE_NOT_ACHIEVED:
    // unimplemented
    break;
 
    // 2.15
  case REQUEST_RESTORE:
    // unimplemented
    break;
 
    // 2.17(1)
  case RESTORE_ACHIEVED:
    // unimplemented
    break;
 
    // 2.17(2)
  case RESTORE_NOT_ACHIEVED:
    // unimplemented
    break;
 
    // 3.1(1)
  case PUBLISH_OBJECT_CLASS:
    D.Out(pdTrace,"Receving Message from Federate, type PublishObjectClass.");
    nb_requetes [8] ++;

    _GD->publishObjectClass(req->objectClassHandle, 
			    req->HandleArray, 
			    req->HandleArraySize, 
			    e);
    break;
 
    // 3.1(2)
  case UNPUBLISH_OBJECT_CLASS:
    nb_requetes [9] ++;
    D.Out(pdTrace, 
	  "Receving Message from Federate, type UnpublishObjectClass.");

    _GD->unpublishObjectClass(req->objectClassHandle, e);
    break;
 
    // 3.2(1)
  case PUBLISH_INTERACTION_CLASS:
    nb_requetes [10] ++;
    D.Out(pdTrace, 
	  "Receving Message from Federate, type PublishInteractionClass.");

    _GD->publishInteractionClass(req->InteractionHandle, e);
    break;
 
    // 3.2(2)
  case UNPUBLISH_INTERACTION_CLASS:
    nb_requetes [11] ++;
    D.Out(pdTrace, 
	  "Receving Message from Federate, type UnpublishInteractionClass.");

    _GD->unpublishInteractionClass(req->InteractionHandle, e);
    break;
 
    // 3.3.(1)
  case SUBSCRIBE_OBJECT_CLASS_ATTRIBUTE:
    nb_requetes [12] ++;
    D.Out(pdTrace, 
	  "Receving Message from Federate, type SubscribeObjectClass.");
    _GD->subscribeObjectClassAttribute(req->objectClassHandle, 
				       req->HandleArray, 
				       req->HandleArraySize, 
				       e);
    break;
 
    // 3.3(2)
  case UNSUBSCRIBE_OBJECT_CLASS_ATTRIBUTE:
    nb_requetes [13] ++;
    D.Out(pdTrace, 
	  "Receving Message from Federate, type UnsubscribeObjectClass.");
 
    // BUG: Pourquoi on ne lui passe pas les attributs ?
    _GD->unsubscribeObjectClassAttribute(req->objectClassHandle, e);
    break;
 
    // 3.4(1)
  case SUBSCRIBE_INTERACTION_CLASS:
    nb_requetes [14] ++;
    D.Out(pdTrace, 
	  "Receving Message from Federate, type SubscribeInteraction.");

    _GD->subscribeInteractionClass(req->InteractionHandle, e);
    break;
 
    // 3.4(2)
  case UNSUBSCRIBE_INTERACTION_CLASS:
    nb_requetes [15] ++;
    D.Out(pdTrace, 
	  "Receving Message from Federate, type UnsubscribeInteraction.");

    _GD->unsubscribeInteractionClass(req->InteractionHandle, e);
    break;
 
    // 4.1
  case REQUEST_ID:
    ObjectHandle first;
    ObjectHandle last;

    D.Out(pdTrace, "Receving Message from Federate, type RequestIDs.");
    nb_requetes [16] ++;

    _GO->requestID(req->IDcount, first, last, e);

    rep.FirstID = first;
    rep.LastID = last;
    break;
 
    // 4.2
  case REGISTER_OBJECT: {
    FederationTime date = _GT->requestFederateTime();
    FederationTime heure = date + _GT->requestLookahead();

    D.Out(pdTrace, "Receving Message from Federate, type RegisterObject.");
    nb_requetes [17] ++;

    rep.Objectid = _GO->registerObject(req->objectClassHandle, req->getName(), 
				       date, heure, e);
    break;/*FAYET 25.07.01*/
  }

    // 4.3
  case UPDATE_ATTRIBUTE_VALUES: {
    D.Out(pdTrace,"Receving Message from Federate, type UpdateAttribValues.");
    nb_requetes [18] ++;

    AttributeValue *ValueArray = req->getValueArray();

    try {
      rep.Retract = _GO->updateAttributeValues(req->Objectid, 
					       req->HandleArray, 
					       ValueArray, 
					       req->HandleArraySize, 
					       req->Date, 
					       req->getTag(), 
					       e);
      free(ValueArray);
    } catch(Exception *e) {
      free(ValueArray);
      throw e;
    }
  }
    break;
 
    // 4.6
  case SEND_INTERACTION: {
    D.Out(pdTrace, "Receving Message from Federate, type SendInteraction.");
    nb_requetes [19] ++;

    ParameterValue *ValueArray =(ParameterValue *) req->getValueArray();

    try {
      rep.Retract = _GO->sendInteraction(req->InteractionHandle, 
					 req->HandleArray, 
					 ValueArray, 
					 req->HandleArraySize, 
					 req->Date, 
					 req->getTag(),
					 e);
      free(ValueArray);
    } catch(Exception *e) {
      free(ValueArray);
      throw e;
    }
  }
    break;

    // 4.8
  case DELETE_OBJECT:
    D.Out(pdTrace, "Receving Message from Federate, type DeleteObject.");
    nb_requetes [20] ++;
    rep.Retract = _GO->deleteObject(req->Objectid, req->getTag(), e);
    break;
 
    // 4.10
  case CHANGE_ATTRIBUTE_TRANSPORT_TYPE:
    nb_requetes [21] ++;
    D.Out(pdTrace, 
	  "Receving Message from Federate, type ChangeAttribTransport.");

    _GO->changeAttributeTransportType(req->Objectid, 
				      req->HandleArray,
				      req->HandleArraySize,
				      req->Transport,
				      e);
    break;
 
    // 4.11
  case CHANGE_ATTRIBUTE_ORDER_TYPE:
    D.Out(pdTrace, "Receving Message from Federate, type ChangeAttribOrder.");
    nb_requetes [22] ++;

    _GO->changeAttributeOrderType(req->Objectid,
				  req->HandleArray,
				  req->HandleArraySize,
				  req->Order,
				  e);
    break;
 
    // 4.12
  case CHANGE_INTERACTION_TRANSPORT_TYPE:
    D.Out(pdTrace, 
	  "Receving Message from Federate, type ChangeInterTransport.");
    nb_requetes [23] ++;

    _GO->changeInteractionTransportType(req->InteractionHandle,
					req->Transport,
					e);
    break;
 
    // 4.13
  case CHANGE_INTERACTION_ORDER_TYPE:
    D.Out(pdTrace, "Receving Message from Federate, type ChangeInterOrder.");
    nb_requetes [24] ++;

    _GO->changeInteractionOrderType(req->InteractionHandle, req->Order, e);
    break;
 
    // 4.14(1)
  case REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
    // unimplemented
    break;
 
    // 4.14(2)
  case REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE:
    // unimplemented
    break;
 
    // 4.16
  case RETRACT:
    // unimplemented
    break;
 
    // 5.1
  case UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
    D.Out(pdTrace, "Receving Message from Federate, "
	  "type unconditionalAttributeOwnershipDivestiture.");
    nb_requetes [47] ++;
    D.Out(pdTrace, "Objet %u nb d'Attribut %u ",
	  req->Objectid,req->HandleArraySize);
    _GP->unconditionalAttributeOwnershipDivestiture(req->Objectid, 
						    req->HandleArray, 
						    req->HandleArraySize,
						    e); 
    break;
 

    // 5.2
  case NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
    D.Out(pdTrace, "Receving Message from Federate, "
	  "type negotiatedAttributeOwnershipDivestiture.");
    nb_requetes [45] ++;
    D.Out(pdTrace, "Objet %u ; %u nb d'Attribut ",req->Objectid,
	  req->HandleArraySize);
    _GP->negotiatedAttributeOwnershipDivestiture(req->Objectid,
						 req->HandleArray, 
						 req->HandleArraySize,
						 req->getTag(), 
						 e);
    break; 
 
 
    // 5.6
  case ATTRIBUTE_OWNERSHIP_ACQUISITION:
    D.Out(pdTrace, "Receving Message from Federate, "
	  "type attributeOwnershipAcquisition.");
    nb_requetes [48] ++;
    D.Out(pdTrace, "Objet %u ; Nb d'Attributs ",req->Objectid,
	  req->HandleArraySize);
    _GP->attributeOwnershipAcquisition(req->Objectid, 
				       req->HandleArray, 
				       req->HandleArraySize,
				       req->getTag(), e); 
    break;
 
    // 5.8
  case ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
    {
      D.Out(pdTrace, "Receving Message from Federate, "
	    "type attributeOwnershipRealeaseResponse.");
      nb_requetes [50] ++;
      D.Out(pdTrace, "Objet %u nb d'Attribut %u ",
	    req->Objectid,req->HandleArraySize);

      AttributeHandleSet* theAttributes=_GP
	->attributeOwnershipRealeaseResponse(req->Objectid,
					     req->HandleArray, 
					     req->HandleArraySize,
					     e);
      if(e == e_NO_EXCEPTION) { 
	rep.HandleArraySize = theAttributes->size();
 
	for(unsigned int i=0; i<theAttributes->size(); i++) {
	  rep.HandleArray[i] = theAttributes->getHandle(i);
	}
      }
    } 
    break; 
 
    // 5.9
  case CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
    D.Out(pdTrace, "Receving Message from Federate, "
	  "type cancelNegociatedAttributeOwnershipDivestiture.");
    nb_requetes [49] ++;
    D.Out(pdTrace, "Objet %u nb d'Attribut %u ",
	  req->Objectid, req->HandleArraySize);
    _GP->cancelnegotiatedAttributeOwnershipDivestiture(req->Objectid,
						       req->HandleArray, 
						       req->HandleArraySize,
						       e);
    break;
 
 
    // 5.10
  case CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
    D.Out(pdTrace, "Receving Message from Federate, "
	  "type cancelAttributeOwnershipAcquisition.");
    nb_requetes [51] ++;
    D.Out(pdTrace, "Objet %u nb d'Attribut %u ",
	  req->Objectid,req->HandleArraySize);
    _GP->cancelattributeOwnershipAcquisition(req->Objectid,
					     req->HandleArray, 
					     req->HandleArraySize,
					     e);
    break; 
    // 5.12
  case ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
    D.Out(pdTrace, "Receving Message from Federate, "
	  "type attributeOwnershipAcquisitionIfAvailable.");
    nb_requetes [46] ++;
    D.Out(pdTrace, "Objet %u nb d'Attribut %u ",
	  req->Objectid,req->HandleArraySize);
    _GP->attributeOwnershipAcquisitionIfAvailable(req->Objectid,
						  req->HandleArray, 
						  req->HandleArraySize,
						  e);
    break;
 
 
    // 5.14
  case QUERY_ATTRIBUTE_OWNERSHIP:
    D.Out(pdTrace, "Receving Message from Federate, "
	  "type queryAttributeOwnership.");
    nb_requetes [44] ++;
    D.Out(pdTrace, "Objet %u Attribut %u ",req->Objectid,req->AttribHandle);
    _GP->queryAttributeOwnership(req->Objectid, req->AttribHandle, e);

    break;
 
 
    // 5.16
  case IS_ATTRIBUTE_OWNED_BY_FEDERATE:
    D.Out(pdTrace, "Receving Message from Federate, "
	  "type isAttributeOwnedByFederate.");
    nb_requetes [43] ++;
    D.Out(pdTrace, "Objet %u Attribut %u ",req->Objectid,req->AttribHandle);
    rep.setTag(_GP->attributeOwnedByFederate(req->Objectid,
					     req->AttribHandle, e));
    break;
 
 
    // 6.1
  case REQUEST_FEDERATION_TIME:
    nb_requetes [25] ++;
    D.Out(pdTrace, 
	  "Receving Message from Federate, type RequestFederationTime.");

    rep.Date = _GT->requestFederationTime();
    break;

    // 6.2
  case REQUEST_LBTS:
    D.Out(pdTrace, "Receving Message from Federate, type RequestLBTS.");
    nb_requetes [26] ++;

    rep.Date = _GT->requestLBTS();
    break;
 
    // 6.3
  case REQUEST_FEDERATE_TIME:
    nb_requetes [27] ++;
    D.Out(pdTrace, 
	  "Receving Message from Federate, type RequestFederateTime.");

    rep.Date = _GT->requestFederateTime();
    break;
 
    // 6.4
  case REQUEST_MIN_NEXT_EVENT_TIME:
    // unimplemented
    break;
 
    // 6.5
  case SET_LOOKAHEAD:
    D.Out(pdTrace, "Receving Message from Federate, type SetLookAhead.");
    nb_requetes [28] ++;

    _GT->setLookahead(req->Lookahead, e);
    break;
 
    // 6.6
  case REQUEST_LOOKAHEAD:
    D.Out(pdTrace, "Receving Message from Federate, type RequestLookAhead.");
    nb_requetes [29] ++;

    rep.Lookahead = _GT->requestLookahead(); 
    break;
 
    // 6.7
  case TIME_ADVANCE_REQUEST:
    D.Out(pdTrace, 
	  "Receving Message from Federate, type TimeAdvanceRequest.");
    nb_requetes [30] ++;

    _GT->timeAdvanceRequest(req->Date, e);
    break;
 
    // 6.8
  case NEXT_EVENT_REQUEST:
    D.Out(pdTrace, 
	  "Receving Message from Federate, type NestEventRequest.");
    nb_requetes [31] ++;

    _GT->nextEventRequest(req->Date, e); 
    break;
 
    // 6.9
  case FLUSH_QUEUE_REQUEST:
    // unimplemented
    break;
 
    // 7.1
  case CREATE_UPDATE_REGION:
    // unimplemented
    break;
 
    // 7.2
  case CREATE_SUBSCRIPTION_REGION:
    // unimplemented
    break;
 
    // 7.3(1)
  case ASSOCIATE_UPDATE_REGION:
    // unimplemented
    break;
 
    // 7.3(2)
  case DISASSOCIATE_UPDATE_REGION:
    // unimplemented
    break;
 
    // 7.5
  case MODIFY_REGION:
    // unimplemented
    break;
 
    // 7.6
  case DELETE_REGION:
    // unimplemented
    break;
 
    // 8.1
  case GET_OBJECT_CLASS_HANDLE:
    nb_requetes [32] ++;
    D.Out(pdTrace, 
	  "Receving Message from Federate, type GetObjectClassHandle.");

    rep.objectClassHandle = _GO->getObjectClassHandle(req->getName());
    break;
 
    // 8.2
  case GET_OBJECT_CLASS_NAME:
    D.Out(pdTrace,
	  "Receving Message from Federate, type GetObjectClassName.");
    nb_requetes [33] ++;

    rep.setName(_GO->getObjectClassName(req->objectClassHandle));
    break;
 
    // 8.3
  case GET_ATTRIBUTE_HANDLE:
    D.Out(pdTrace,
	  "Receving Message from Federate, type GetAttributeHandle.");
    nb_requetes [34] ++;

    rep.AttribHandle = _GO->getAttributeHandle(req->getName(),
					       req->objectClassHandle);
    break;
 
    // 8.4
  case GET_ATTRIBUTE_NAME:
    D.Out(pdTrace, 
	  "Receving Message from Federate, type GetAttributeName.");
    nb_requetes [35] ++;

    rep.setName(_GO->getAttributeName(req->AttribHandle,
				      req->objectClassHandle));
    break;
 
    // 8.5
  case GET_INTERACTION_CLASS_HANDLE:
    nb_requetes [36] ++;
    D.Out(pdTrace, 
	  "Receving Message from Federate, type GetInteractionHandle.");

    rep.InteractionHandle = _GO->getInteractionClassHandle(req->getName());
    break;
 
    // 8.6
  case GET_INTERACTION_CLASS_NAME:
    nb_requetes [37] ++;
    D.Out(pdTrace, 
	  "Receving Message from Federate, type GetInteractionName.");

    rep.setName(_GO->getInteractionClassName(req->InteractionHandle));
    break;
 
    // 8.7
  case GET_PARAMETER_HANDLE:
    D.Out(pdTrace,
	  "Receving Message from Federate, type GetParameterHandle.");
    nb_requetes [38] ++;

    rep.ParamHandle = _GO->getParameterHandle(req->getName(),
					      req->InteractionHandle);
    break;
 
    // 8.8
  case GET_PARAMETER_NAME:
    D.Out(pdTrace, 
	  "Receving Message from Federate, type GetParameterName.");
    nb_requetes [39] ++;

    rep.setName(_GO->getParameterName(req->ParamHandle,
				      req->InteractionHandle));
    break;
 
    // 8.9
  case GET_SPACE_HANDLE:
    // unimplemented
    break;
 
    // 8.10
  case GET_SPACE_NAME:
    // unimplemented
    break;
 
    // 8.11
  case GET_FEDERATE_HANDLE:
    // unimplemented
    break;
 
    // 8.12
  case GET_FEDERATE_NAME:
    // unimplemented
    break;
 
    // 8.13
  case SET_TIME_REGULATING:
    D.Out(pdTrace, 
	  "Receving Message from Federate, type SetTimeRegulating.");
    nb_requetes [40] ++;

    _GT->setTimeRegulating(req->Bool,e);
    break;
 
    // 8.14
  case SET_TIME_CONSTRAINED:
    D.Out(pdTrace,
	  "Receving Message from Federate, type SetTimeConstrained.");
    nb_requetes [41] ++;

    _GT->setTimeConstrained(req->Bool,e);
    break;
 
    // 8.15
  case TICK_REQUEST:
    D.Out(pdDebug, "Receving Message from Federate, type TickRequest.");
    nb_requetes [42] ++;

    rep.Bool = _GT->tick(e);
    break;
 
  default:
    D.Out(pdExcept, 
	  "Receving Message from Federate, Unknown Type %d.", req->Type);
    throw RTIinternalError();
  }
}


// ----------------------------
// -- TraiterRequeteDuFedere --
// ----------------------------

// le RTIA traite une demande de service du federe

void RTIA::processFederateRequest(Message *req)
{
  Message rep;

  rep.Exception = e_NO_EXCEPTION;
  rep.Type = req->Type;

  try {
    chooseFederateProcessing(req, rep, rep.Exception);
  }
  catch(ArrayIndexOutOfBounds &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_ArrayIndexOutOfBounds;
  }
  catch(AttributeAlreadyOwned &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_AttributeAlreadyOwned;
  }
  catch(AttributeAlreadyBeingDivested &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_AttributeAlreadyBeingDivested;
  }
  catch(AttributeAlreadyBeingAcquired &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_AttributeAlreadyBeingAcquired;
  }
  catch(AttributeAcquisitionWasNotRequested &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_AttributeAcquisitionWasNotRequested;
  } 
  catch(AttributeDivestitureWasNotRequested &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_AttributeDivestitureWasNotRequested;
  } 
  catch(AttributeNotDefined &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_AttributeNotDefined;
  }
  catch(AttributeNotKnown &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_AttributeNotKnown;
  }
  catch(AttributeNotOwned &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_AttributeNotOwned;
  }
  catch(AttributeNotPublished &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_AttributeNotPublished;
  }
  catch(AttributeNotSubscribed &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_AttributeNotSubscribed;
  }
  catch(ConcurrentAccessAttempted &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_ConcurrentAccessAttempted;
  }
  catch(CouldNotDiscover &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_CouldNotDiscover;
  }
  catch(CouldNotOpenRID &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_CouldNotOpenRID;
  }
  catch(CouldNotRestore &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_CouldNotRestore;
  }
  catch(DeletePrivilegeNotHeld &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_DeletePrivilegeNotHeld;
  }
  catch(ErrorReadingRID &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_ErrorReadingRID;
  }
  catch(EventNotKnown &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_EventNotKnown;
  }
  catch(FederateAlreadyPaused &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_FederateAlreadyPaused;
  }
  catch(FederateAlreadyExecutionMember &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_FederateAlreadyExecutionMember;
  }
  catch(FederateDoesNotExist &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_FederateDoesNotExist;
  }
  catch(FederateInternalError &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_FederateInternalError;
  }
  catch(FederateNameAlreadyInUse &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_FederateNameAlreadyInUse;
  }
  catch(FederateNotExecutionMember &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_FederateNotExecutionMember;
  }
  catch(FederateNotPaused &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_FederateNotPaused;
  }
  catch(FederateNotPublishing &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_FederateNotPublishing;
  }
  catch(FederateNotSubscribing &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_FederateNotSubscribing;
  }
  catch(FederateOwnsAttributes &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_FederateOwnsAttributes;
  }
  catch(FederatesCurrentlyJoined &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_FederatesCurrentlyJoined;
  }
  catch(FederateWasNotAskedToReleaseAttribute &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_FederateWasNotAskedToReleaseAttribute;
  }
  catch(FederationAlreadyPaused &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_FederationAlreadyPaused;
  }
  catch(FederationExecutionAlreadyExists &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_FederationExecutionAlreadyExists;
  }
  catch(FederationExecutionDoesNotExist &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_FederationExecutionDoesNotExist;
  }
  catch(FederationNotPaused &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_FederationNotPaused;
  }
  catch(FederationTimeAlreadyPassed &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_FederationTimeAlreadyPassed;
  }
  catch(IDsupplyExhausted &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_IDsupplyExhausted;
  }
  catch(InteractionClassNotDefined &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_InteractionClassNotDefined;
  }
  catch(InteractionClassNotKnown &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_InteractionClassNotKnown;
  }
  catch(InteractionClassNotPublished &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_InteractionClassNotPublished;
  }
  catch(InteractionParameterNotDefined &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_InteractionParameterNotDefined;
  }
  catch(InteractionParameterNotKnown &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_InteractionParameterNotKnown;
  }
  catch(InvalidDivestitureCondition &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_InvalidDivestitureCondition;
  }
  catch(InvalidExtents &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_InvalidExtents;
  }
  catch(InvalidFederationTime &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_InvalidFederationTime;
  }
  catch(InvalidFederationTimeDelta &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_InvalidFederationTimeDelta;
  }
  catch(InvalidObjectHandle &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_InvalidObjectHandle;
  }
  catch(InvalidOrderType &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_InvalidOrderType;
  }
  catch(InvalidResignAction &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_InvalidResignAction;
  }
  catch(InvalidRetractionHandle &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_InvalidRetractionHandle;
  }
  catch(InvalidRoutingSpace &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_InvalidRoutingSpace;
  }
  catch(InvalidTransportType &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_InvalidTransportType;
  }
  catch(MemoryExhausted &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_MemoryExhausted;
  }
  catch(NameNotFound &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_NameNotFound;
  }
  catch(NoPauseRequested &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_NoPauseRequested;
  }
  catch(NoResumeRequested &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_NoResumeRequested;
  }
  catch(ObjectClassNotDefined &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_ObjectClassNotDefined;
  }
  catch(ObjectClassNotKnown &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_ObjectClassNotKnown;
  }
  catch(ObjectClassNotPublished &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_ObjectClassNotPublished;
  }
  catch(ObjectClassNotSubscribed &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_ObjectClassNotSubscribed;
  }
  catch(ObjectNotKnown &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_ObjectNotKnown;
  }
  catch(ObjectAlreadyRegistered &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_ObjectAlreadyRegistered;
  }
  catch(RegionNotKnown &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_RegionNotKnown;
  }
  catch(RestoreInProgress &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_RestoreInProgress;
  }
  catch(RestoreNotRequested &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_RestoreNotRequested;
  }
  catch(RTIinternalError &e) {
    if(e._reason != NULL)
      printf("RTIA sends InternalError to Fed., reason : %s\n", e._reason);
    else
      printf("RTIA sends InternalError to Fed., no reason given.\n");
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_RTIinternalError;
  }
  catch(SpaceNotDefined &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_SpaceNotDefined;
  }
  catch(SaveInProgress &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_SaveInProgress;
  }
  catch(SaveNotInitiated &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_SaveNotInitiated;
  }
  catch(SpecifiedSaveLabelDoesNotExist &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_SpecifiedSaveLabelDoesNotExist;
  }
  catch(TimeAdvanceAlreadyInProgress &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_TimeAdvanceAlreadyInProgress;
  }
  catch(TimeAdvanceWasNotInProgress &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_TimeAdvanceWasNotInProgress;
  }
  catch(TooManyIDsRequested &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_TooManyIDsRequested;
  }
  catch(UnableToPerformSave &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_UnableToPerformSave;
  }
  catch(UnimplementedService &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_UnimplementedService;
  }
  catch(UnknownLabel &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_UnknownLabel;
  }
  catch(ValueCountExceeded &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_ValueCountExceeded;
  }
  catch(ValueLengthExceeded &e) {
    D.Out(pdExcept, "Catched %s Exception.", e._name);
    rep.Exception = e_ValueLengthExceeded;
  }

  // Default Handler
  catch(Exception &e) {
    D.Out(pdExcept, "Unknown Exception : %s.", e._name);
    rep.Exception = e_RTIinternalError;
  }

  delete req;

  _GC->sendUN(&rep);
  D.Out(pdDebug, "Reponse emise sur le socket Unix.");

}

}}

// $Id: RTIA_federate.cc,v 3.1 2002/11/26 15:48:01 breholee Exp $

