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
// $Id: FederationsList.cc,v 3.1 2002/11/26 15:48:01 breholee Exp $
// ---------------------------------------------------------------------------

#include "FederationsList.hh"

namespace certi {
namespace rtig {

static pdCDebug D("LISTEFEDERATIONS", "(ListFede) - ");

// ---------------------------------------------------------------------------
// Constructor

FederationsList::FederationsList(SocketServer *server, AuditFile *audit)
  : List <Federation*>()
{
  if(server == NULL)
    throw RTIinternalError("FederationsList: No SocketServer was given.");

  if(audit == NULL)
    throw RTIinternalError("FederationsList: No AuditServer was given.");

  socketServer = server ;
  auditFile = audit ;
}

// ---------------------------------------------------------------------------
// Destructor

FederationsList::~FederationsList()
{
  Federation *federation = NULL;

  if(lg > 0)
    D.Out(pdError, "ListeFederation not empty at destruction time.");

  while(lg > 0) {
    federation = Ieme(1);
    Supprimer(1);
    delete federation;
  }
}

// ---------------------------------------------------------------------------
// addConstrained

void 
FederationsList::addConstrained(FederationHandle handle, 
				FederateHandle federate)
  throw(FederationExecutionDoesNotExist,
	FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank; // Unused

  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);

  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
 
  // It may throw a bunch of exceptions.
  federation->addConstrained(federate);
}

// ---------------------------------------------------------------------------
// addFederate

FederateHandle 
FederationsList::addFederate(FederationHandle handle,
			     FederateName name,
			     SocketTCP *tcp_link)
  throw(FederationExecutionDoesNotExist,
	FederateAlreadyExecutionMember,
	MemoryExhausted,
	RTIinternalError)
{
  Federation *federation = NULL;
  FederateHandle federate;
  int rank; // Unused
 
  // It may throw RTIinternalError
  checkHandle(handle);

  // It may throw FederationExecutionDoesNotExist
  searchFederation(handle, federation, rank);

  // It may raise a bunch of exceptions
  federate = federation->add(name, tcp_link);

  return federate;
}

// ---------------------------------------------------------------------------
// searchFederation

void 
FederationsList::searchFederation(FederationHandle handle,
				  Federation* &federation,
				  int &rank)
  throw(FederationExecutionDoesNotExist,
	RTIinternalError)
{
  int i;
  federation = NULL;
  rank = 0;

  // It may raise RTIinternalError
  checkHandle(handle);

  i = 1;
  while(i<=lg) {
    federation = Ieme(i);
    if(federation->getHandle() == handle) {
      rank = i;
      return;
    }
    i++;
  }
  D.Out(pdExcept, "Unknown Federation Handle %d.", handle);
  throw FederationExecutionDoesNotExist("Bad Federation Handle.");
}

// ---------------------------------------------------------------------------
// createFederation

#ifdef FEDERATION_USES_MULTICAST
void FederationsList::createFederation(FederationExecutionName name,
				       FederationHandle handle,
				       SocketMC *mc_link)
#else
void FederationsList::createFederation(FederationExecutionName name,
				       FederationHandle handle)
#endif
  throw(FederationExecutionAlreadyExists,
	CouldNotOpenRID,
	ErrorReadingRID,
	MemoryExhausted, 
	SecurityError,
	RTIinternalError)
{
  Federation *federation = NULL;
  FederationHandle unused;

  // It may throw RTIinternalError
  checkHandle(handle);
  auditFile->addToLinef(", Handle : %hu", handle);
  if(name == NULL) throw RTIinternalError("Invalid Federation Name.");

  // It should throw FederationExecutionDoesNotExist.
  try {
    exists(name, unused);
    D.Out(pdExcept, "Federation %s already present.", name);
    throw FederationExecutionAlreadyExists();
  }
  catch(FederationExecutionDoesNotExist &e) {
    D.Out(pdDebug, 
	  "CreerFederation catches FederationExecutionDoesNotExist.");
  }

  if(lg >= MAX_FEDERATION)
    throw RTIinternalError("Too many federation executions.");


#ifdef FEDERATION_USES_MULTICAST
  federation = new Federation(name, handle, socketServer, auditFile, mc_link);
#else
  federation = new Federation(name, handle, socketServer, auditFile);
#endif 

  if(federation == NULL)
    throw MemoryExhausted("No memory left for new Federation.");

  Inserer(1, federation);
  D.Out(pdInit, "New Federation created with Handle %d.", handle);
}

// ---------------------------------------------------------------------------
// createRegulator

void 
FederationsList::createRegulator(FederationHandle handle,
				 FederateHandle federate,
				 FederationTime time)
  throw(FederationExecutionDoesNotExist,
	FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank; // unused

  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);

  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);

  // It may throw a bunch of exceptions.
  federation->addRegulator(federate, time);
}

// ---------------------------------------------------------------------------
// requestId

void 
FederationsList::requestId(FederationHandle handle, 
			   ObjectHandlecount id_count,
			   ObjectHandle &first_id,
			   ObjectHandle &last_id)
  throw(TooManyIDsRequested)
{
  Federation *federation = NULL;
  int rank;
 
  // It may throw RTIinternalError.
  checkHandle(handle);

  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
 
  federation->requestId(id_count, first_id, last_id);
}

// ---------------------------------------------------------------------------
// destroyObject

void 
FederationsList::destroyObject(FederationHandle handle,
			       FederateHandle federate,
			       ObjectHandle id,
			       const char*  tag)
  throw(FederateNotExecutionMember,
	FederationExecutionDoesNotExist,
	DeletePrivilegeNotHeld,
	ObjectNotKnown,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank;
 
  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);

  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);

  federation->deleteObject(federate, id, tag);
}

// ---------------------------------------------------------------------------
// exists

void FederationsList::exists(FederationExecutionName name,
			     FederationHandle &handle)
  throw(FederationExecutionDoesNotExist,
	RTIinternalError)
{
  Federation *federation;
  int i;

  if(name == NULL) throw RTIinternalError("Null Federation Name.");

  i = 1; 
  while(i <= lg) {
    federation = Ieme(i);
    if(!strcmp(federation->getName(), name)) {
      handle = federation->getHandle();
      return;
    }
    i++;
  }

  D.Out(pdDebug, "EstPresent throws FederationExecutionDoesNotExist.");
  throw FederationExecutionDoesNotExist();
}

// ---------------------------------------------------------------------------
// info

#ifdef FEDERATION_USES_MULTICAST
void FederationsList::info(FederationHandle handle,
			   int &nb_federates,
			   int &nb_regulators,
			   bool &is_paused,
			   SocketMC* &comm_mc)
#else
void FederationsList::info(FederationHandle handle,
			   int &nb_federates,
			   int &nb_regulators,
			   bool &is_paused)
#endif
  throw(FederationExecutionDoesNotExist, RTIinternalError)
{
  Federation *federation;
  int rank;

  // It may raise RTIinternalError
  checkHandle(handle);

  // It may throw FederationExecutionNotFound
  searchFederation(handle, federation, rank);

  nb_federates = federation->getNbFederates();
  nb_regulators = federation->getNbRegulators();
  is_paused = federation->isPaused();

#ifdef FEDERATION_USES_MULTICAST
  comm_mc = federation->MCLink;
#endif
}

// ---------------------------------------------------------------------------
// registerObject

ObjectHandle 
FederationsList::registerObject(FederationHandle handle,
				FederateHandle federate,
				ObjectClassHandle object_class,
				ObjectName name)
  throw(FederateNotExecutionMember,
	FederateNotPublishing,
	ObjectAlreadyRegistered,
	ObjectClassNotDefined,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank;
 
  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);
  D.Out(pdTrace,"handle = %d, federate = %d.",
	handle,federate);
  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
 
  D.Out(pdTrace,"theObjectClass = %d, name = %s.",object_class,name);
  return(federation->registerObject(federate,object_class,name));
}

// ---------------------------------------------------------------------------
// checkHandle
// handle can be any type of handle, not only a Federation handle.

void 
FederationsList::checkHandle(FederationHandle handle)
  throw(RTIinternalError)
{
  if(handle <= 0) {
    D.Out(pdExcept, "Illegal Handle %d.", handle);
    throw RTIinternalError("Negative or null FederationHandle.");
  }
}

// ---------------------------------------------------------------------------
// updateRegulator

void 
FederationsList::updateRegulator(FederationHandle handle,
				 FederateHandle federate,
				 FederationTime time)
  throw(FederationExecutionDoesNotExist,
	FederateNotExecutionMember,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank; // unused

  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);

  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
 
  federation->updateRegulator(federate, time);
}

// ---------------------------------------------------------------------------
// updateAttribute

void 
FederationsList::updateAttribute(FederationHandle handle,
				 FederateHandle federate,
				 ObjectHandle id,
				 AttributeHandle *attributes,
				 AttributeValue *values,
				 UShort list_size,
				 FederationTime time,
				 const char*  tag)
  throw(FederateNotExecutionMember,
	FederationExecutionDoesNotExist,
	ObjectNotKnown,
	AttributeNotDefined,
	AttributeNotOwned,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank;
 
  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);

  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
 
  federation->updateAttributeValues(federate, id, attributes, values,
				    list_size, time, tag);
}

// ---------------------------------------------------------------------------
// updateParameter

void 
FederationsList::updateParameter(FederationHandle handle,
				 FederateHandle federate,
				 InteractionClassHandle interaction,
				 ParameterHandle *parameters,
				 ParameterValue *values,
				 UShort list_size,
				 FederationTime time,
				 const char*  tag)
  throw(FederateNotExecutionMember,
	FederateNotPublishing,
	FederationExecutionDoesNotExist,
	InteractionClassNotDefined,
	InteractionParameterNotDefined,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank;
 
  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);

  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
 
  federation->broadcastInteraction(federate, interaction, parameters, values, 
				   list_size, time, tag);
}

// ---------------------------------------------------------------------------
// setPause

void 
FederationsList::setPause(FederationHandle handle,
			  FederateHandle federate,
			  bool state,
			  PauseLabel label)
  throw(FederationAlreadyPaused, 
	FederationNotPaused,
	FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
 
{
  Federation *federation = NULL;
  int rank;

  // It may throw RTIinternalError
  checkHandle(handle);

  // It may throw FederationExecutionDoesNotExist
  searchFederation(handle, federation, rank);

  // It may throw a bunch of exceptions.
  if(state) federation->enterPause(federate, label);
  else federation->resumePause(federate, label);
}

// ---------------------------------------------------------------------------
// publishInteraction

void 
FederationsList::publishInteraction(FederationHandle handle,
				    FederateHandle federate, 
				    InteractionClassHandle interaction,
				    bool pub)
  throw(InteractionClassNotDefined,
	FederationExecutionDoesNotExist,
	FederateNotExecutionMember,
	SaveInProgress,
	SecurityError,
	RestoreInProgress,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank;
 
  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);

  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
 
  federation->publishInteraction(federate, interaction, pub);
}

// ---------------------------------------------------------------------------
// publishObject

void 
FederationsList::publishObject(FederationHandle handle,
			       FederateHandle federate,
			       ObjectClassHandle object_class,
			       AttributeHandle *attributes,
			       UShort list_size,
			       bool pub) 
  throw(ObjectClassNotDefined,
	AttributeNotDefined,
	FederationExecutionDoesNotExist,
	FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	SecurityError,
	RTIinternalError)
 
{
  Federation *federation = NULL;
  int rank;

  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);

  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
 
  federation->publishObject(federate, object_class, attributes, list_size, 
			    pub);
}

// ---------------------------------------------------------------------------
// subscribeInteraction

void 
FederationsList::subscribeInteraction(FederationHandle handle,
				      FederateHandle federate, 
				      InteractionClassHandle interaction,
				      bool sub)
  throw(InteractionClassNotDefined,
	FederationExecutionDoesNotExist,
	FederateNotExecutionMember,
	SaveInProgress,
	SecurityError,
	RestoreInProgress,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank;
 
  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);

  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
 
  federation->subscribeInteraction(federate, interaction, sub);
}

// ---------------------------------------------------------------------------
// subscribeObject

void 
FederationsList::subscribeObject(FederationHandle handle,
				 FederateHandle federate,
				 ObjectClassHandle object_class,
				 AttributeHandle *attributes,
				 UShort list_size,
				 bool sub)
  throw(ObjectClassNotDefined,
	AttributeNotDefined,
	FederationExecutionDoesNotExist,
	FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	SecurityError,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank;
 
  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);

  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);

  federation->subscribeObject(federate, object_class, attributes, list_size, 
			      sub);
}

// ---------------------------------------------------------------------------
// removeConstrained

void 
FederationsList::removeConstrained(FederationHandle handle,
				   FederateHandle federate)
  throw(FederationExecutionDoesNotExist,
	FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank; // unused

  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);

  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
 
  // It may throw a bunch of exceptions.
  federation->removeConstrained(federate);
}

// ---------------------------------------------------------------------------
// destroyFederation

void 
FederationsList::destroyFederation(FederationHandle handle)
  throw(FederatesCurrentlyJoined,
	FederationExecutionDoesNotExist,
	RTIinternalError)
{
  Federation *federation;
  int rank;

  // It may throw RTIinternalError
  checkHandle(handle);

  // It may throw FederationExecutionDoesNotExist
  searchFederation(handle, federation, rank);

  // It may throw FederatesCurrentlyJoined
  if(federation->empty()) {
    Supprimer(rank);
    delete federation;
  }
}

// ---------------------------------------------------------------------------
// remove

void 
FederationsList::remove(FederationHandle handle, FederateHandle federate)
  throw(FederationExecutionDoesNotExist,
	FederateOwnsAttributes,
	FederateNotExecutionMember,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank; // unused

  // It may throw RTIinternalError.
  checkHandle(handle);

  // It may throw FederationExecutionDoesNotExist
  searchFederation(handle, federation, rank);

  // It may throw FederateOwnsAttributes or FederateNotExecutionMember
  federation->remove(federate);
}

// ---------------------------------------------------------------------------
// removeRegulator

void 
FederationsList::removeRegulator(FederationHandle handle,
				 FederateHandle federate)
  throw(FederationExecutionDoesNotExist,
	FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank;

  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);

  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
 
  federation->removeRegulator(federate);
}

// ---------------------------------------------------------------------------
// killFederate

void 
FederationsList::killFederate(FederationHandle handle, FederateHandle federate)
  throw()
{
  Federation *federation = NULL;
  int rank;

  try {
    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);
 
    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation, rank);
    federation->kill(federate);
  }
  catch(Exception &e) {
    return;
  }
}

// ---------------------------------------------------------------------------
// isOwner

bool 
FederationsList::isOwner(FederationHandle handle,
			 FederateHandle federate,
			 ObjectHandle id,
			 AttributeHandle attribute)
  throw(FederateNotExecutionMember,
	ObjectNotKnown,
	AttributeNotDefined,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  Federation *f = NULL;
  int rank;
 
  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, f, rank);
 
  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);
 
  D.Out(pdDebug, "Owner of Attribute %u of Object %u .", attribute, id); 
 
  return(f->isOwner(federate, id, attribute));
}

// ---------------------------------------------------------------------------
// searchOwner

void 
FederationsList::searchOwner(FederationHandle handle,
			     FederateHandle federate,
			     ObjectHandle id,
			     AttributeHandle attribute)
  throw(FederateNotExecutionMember,
	ObjectNotKnown,
	AttributeNotDefined,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank;
 
  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
 
 
  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);
 
  D.Out(pdDebug, "Owner of Attribute %u of Object %u .", attribute, id); 
 
  federation->queryAttributeOwnership(federate, id, attribute);
}

// ---------------------------------------------------------------------------
// negotiateDivestiture

void 
FederationsList::negotiateDivestiture(FederationHandle handle,
				      FederateHandle federate,
				      ObjectHandle id,
				      AttributeHandle *attributes,
				      UShort list_size,
				      const char *tag)
  throw(FederateNotExecutionMember,
	ObjectNotKnown,
	AttributeNotDefined,
	AttributeNotOwned,
	AttributeAlreadyBeingDivested, 
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank;
 
  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
  
  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);
 
  D.Out(pdDebug, "NegotiatedAttributeOwnershipDivestiture of Object %u.", id); 
 
  federation->negotiateDivestiture(federate, id, attributes, list_size, tag);
}

// ---------------------------------------------------------------------------
// acquireIfAvailable

void 
FederationsList::acquireIfAvailable(FederationHandle handle,
				    FederateHandle federate,
				    ObjectHandle id,
				    AttributeHandle *attributes,
				    UShort list_size)  
  throw(ObjectNotKnown,
	ObjectClassNotPublished,
	AttributeNotDefined,
	AttributeNotPublished,
	FederateOwnsAttributes,
	AttributeAlreadyBeingAcquired,
	FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank;
 
  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
  
  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);
 
  federation->acquireIfAvailable(federate, id, attributes, list_size);
}

// ---------------------------------------------------------------------------
// divest

void 
FederationsList::divest(FederationHandle handle,
			FederateHandle federate,
			ObjectHandle id,
			AttributeHandle *attributes,
			UShort list_size) 
  throw(ObjectNotKnown,
	AttributeNotDefined,
	AttributeNotOwned,
	FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank;
 
  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
 
  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);
 
  federation->divest(federate, id, attributes, list_size);
}

// ---------------------------------------------------------------------------
// acquire

void 
FederationsList::acquire(FederationHandle handle,
			 FederateHandle federate,
			 ObjectHandle id,
			 AttributeHandle *attributes,
			 UShort list_size,
			 const char *tag)
  throw(ObjectNotKnown,
	ObjectClassNotPublished,
	AttributeNotDefined,
	AttributeNotPublished,
	FederateOwnsAttributes,
	FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank;

  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
 
  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);
 
  D.Out(pdDebug, "attributeOwnershipAcquisition of Object %u .", id); 
 
  federation->acquire(federate, id, attributes, list_size, tag);
}

// ---------------------------------------------------------------------------
// cancelNegotiatedAttributeOwnershipDivestiture

void 
FederationsList::cancelDivestiture(FederationHandle handle,
				   FederateHandle federate,
				   ObjectHandle id,
				   AttributeHandle *attributes,
				   UShort list_size) 
  throw(ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
	AttributeDivestitureWasNotRequested, FederateNotExecutionMember,
	SaveInProgress, RestoreInProgress, RTIinternalError)
{
  Federation *federation = NULL;
  int rank;
 
  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
  
  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);
 
  D.Out(pdDebug, 
	"cancelNegotiatedAttributeOwnershipDivestiture of Object %u .",	id); 
 
  federation->cancelDivestiture(federate, id, attributes, list_size);
}

// ---------------------------------------------------------------------------
// respondRelease

AttributeHandleSet*
FederationsList::respondRelease(FederationHandle handle,
				FederateHandle federate,
				ObjectHandle id,
				AttributeHandle *attributes,
				UShort list_size) 
  throw(ObjectNotKnown,
	AttributeNotDefined,
	AttributeNotOwned,
	FederateWasNotAskedToReleaseAttribute,
	FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  Federation *federation = NULL;
  int rank;
 
  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
  
  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);
 
  D.Out(pdDebug, "AttributeOwnershipRealeaseResponse of Object %u .", id); 
 
  return federation->respondRelease(federate, id, attributes, list_size);
}

// ---------------------------------------------------------------------------
// cancelAcquisition

void 
FederationsList::cancelAcquisition(FederationHandle handle,
				   FederateHandle federate,
				   ObjectHandle id,
				   AttributeHandle *attributes,
				   UShort list_size)
  throw(ObjectNotKnown,
	AttributeNotDefined,
	AttributeAlreadyOwned,
	AttributeAcquisitionWasNotRequested,
	FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError) 
{
  Federation *federation = NULL;
  int rank;
 
  // It may throw FederationExecutionDoesNotExist.
  searchFederation(handle, federation, rank);
 
  // It may throw RTIinternalError.
  checkHandle(handle);
  checkHandle(federate);
 
  D.Out(pdDebug, "cancelAttributeOwnershipAcquisition of Object %u .", id); 
 
  federation->cancelAcquisition(federate, id, attributes, list_size);
}

}}

// EOF $Id: FederationsList.cc,v 3.1 2002/11/26 15:48:01 breholee Exp $

