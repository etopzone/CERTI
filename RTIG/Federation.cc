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
// $Id: Federation.cc,v 3.1 2002/11/26 15:48:01 breholee Exp $
// ---------------------------------------------------------------------------

#include "Federation.hh"

namespace certi {
namespace rtig {

static pdCDebug D("FEDERATION", "(Fed.tion) - ");

// ---------------------------------------------------------------------------
// Constructor

#ifdef FEDERATION_USES_MULTICAST

Federation::Federation(FederationExecutionName federation_name,
		       FederationHandle federation_handle,
		       SocketServer *socket_server,
		       AuditFile *audit_server,
		       SocketMC *mc_link)
#else

  Federation::Federation(FederationExecutionName federation_name,
			 FederationHandle federation_handle,
			 SocketServer *socket_server,
			 AuditFile *audit_server)

#endif
  throw(CouldNotOpenRID,
	ErrorReadingRID,
	MemoryExhausted, 
	SecurityError,
	RTIinternalError)
  : List <Federate *>()
{
  fedparser::FedParser *fed_reader;
  char file_name [MAX_FEDERATION_NAME_LENGTH + 5];

#ifdef FEDERATION_USES_MULTICAST // -----------------
  // Initialize Multicast
  if(mc_link == NULL) {
    D.Out(pdExcept, "Null Multicast socket for new Federation.");
    throw RTIinternalError("NULL Multicast socket for new Federation.");
  }

  D.Out(pdInit, "New Federation %d will use Multicast.", federation_handle);
  MCLink = mc_link;
#endif // FEDERATION_USES_MULTICAST // --------------

  // Allocates Name
  if((federation_name == NULL) ||
     (federation_handle == 0))
    throw RTIinternalError("Null init parameter in Federation creation.");

  if(strlen(federation_name) > MAX_FEDERATION_NAME_LENGTH)
    throw RTIinternalError("Federation name too long.");

  name = strdup(federation_name);
  if(name == NULL)
    throw MemoryExhausted("No memory left for Federation Name.");

  // Default Attribute values

  handle = federation_handle;

  this->paused = false ;
  pauseLabel [0] = '\0';

  nextObjectId = (ObjectHandle) 1 ;
  nextFederateHandle = (FederateHandle) 1 ;

  D.Out(pdInit, 
	"New Federation created with Handle %d, now reading FED.", handle);

  // Initialize the Security Server.

  server = new SecurityServer(socket_server,
			      audit_server,
			      handle);

  if(server == NULL) throw RTIinternalError();

  // Read FED File to initialize Root Object.

  root = new RootObject(server);

  if(root == NULL) 
    throw MemoryExhausted("No memory left for Federation Root Object.");

  fed_reader = new fedparser::FedParser(root);
  if(fed_reader == NULL)
    throw MemoryExhausted("No memory left to read FED file.");

  strcpy(file_name, name);
  strcat(file_name, ".fed");

  server->Audit->addToLinef(", Fed File : %s", file_name);

  try {
    fed_reader->readFile(file_name);
  }
  catch(Exception *e) {
    delete fed_reader;
    delete server;
    server = NULL;
    delete root;
    root = NULL;
    throw e;
  }

  delete fed_reader;

  // Retrieve the FED file last modification time(for Audit)
  struct stat StatBuffer;
  char *MTimeBuffer;

  if(stat(file_name, &StatBuffer) == 0) {
    MTimeBuffer = ctime(&StatBuffer.st_mtime);
    MTimeBuffer [strlen(MTimeBuffer) - 1] = 0; // Remove trailing \n
    server->Audit->addToLinef("(Last modified %s)", MTimeBuffer);
  }
  else
    server->
      Audit->addToLinef("(could not retrieve last modif time, errno %d).",
		       errno);

}

// ---------------------------------------------------------------------------
// Destructor

Federation::~Federation()
{
  // If there are Federates, delete them all!
  Federate *tmp_federate = NULL;

  D.Out(pdInit, "Destroying Federation %d...", handle);

  while(lg > 0) {
    tmp_federate = Ieme(1);
    delete tmp_federate;
    Supprimer(1);
  }

  // Free local allocations
  free(name);
  delete root;

  delete server;

#ifdef FEDERATION_USES_MULTICAST
  FermerConnexion(mc_link);
  delete mc_link;
#endif

}

// ---------------------------------------------------------------------------
// Get attributes

int 
Federation::getNbFederates(void) 
{ 
  return lg ; 
}

bool 
Federation::isPaused(void) 
{ 
  return paused ; 
}

FederationHandle 
Federation::getHandle(void) 
{ 
  return handle ; 
} 
  
FederationExecutionName 
Federation::getName(void) 
{ 
  return name ; 
} 

int
Federation::getNbRegulators(void)
{
  return regulators.lg ;
}

// ---------------------------------------------------------------------------
// add

FederateHandle 
Federation::add(FederateName federate_name, SocketTCP *tcp_link)
  throw(FederateAlreadyExecutionMember,	MemoryExhausted, RTIinternalError)
{
  Federate *federate;
  FederateHandle federate_handle;
  NetworkMessage message;
  int i; // Loop variable

  if(federate_name == NULL) {
    D.Out(pdExcept, "Tried to add a NULL named federate.");
    throw RTIinternalError("Tried to add NULL federate to federation.");
  }

  if(lg >= MAX_FEDERATE) {
    D.Out(pdExcept, "Federation %d has too many Federates.", handle);
    throw RTIinternalError("Too many federates in federation.");
  }

  try {
    this->getByName(federate_name);
    throw FederateAlreadyExecutionMember();
  }
  catch(FederateNotExecutionMember &e) {
    // Nothing to do.
  }

  federate_handle = getNewHandle();

  federate = new Federate(federate_name, federate_handle);

  Inserer(1, federate);

  D.Out(pdInit, "Federate %d joined Federation %d.", federate_handle, handle);

  // Donner au nouveau federe un dernier message nul
  // de chaque federe(i) contenant son heure logique h(i)
  // pour que le nouveau puisse calculer son LBTS.

  try {
    for(i = 1; i <= regulators.lg; i++) {
      message.Type = m_MESSAGE_NULL;
      message.NumeroFederation = handle;

      regulators.get(i, message.NumeroFedere, message.Date);

      D.Out(pdTerm, "Sending NULL message(type %d) from %d to new federate.",
	    message.Type, message.NumeroFedere);

      message.write(tcp_link);
    }

    // Si la federation est en pause, mettre le federe en pause

    if(this->paused) {
      message.Type = m_REQUEST_PAUSE;
      message.NumeroFedere = federate_handle;
      message.NumeroFederation = handle;

      D.Out(pdTerm, "Sending Pause message(type %d) to the new Federate.",
	    message.Type);
      strcpy(message.Label, pauseLabel);

      message.write(tcp_link);
    }
  } 
  catch(NetworkError) {
    throw RTIinternalError("Network Error while initializing federate.");
  }

  return federate_handle ;
}

// ---------------------------------------------------------------------------
// addConstrained

void 
Federation::addConstrained(FederateHandle federate_handle)
  throw(FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  Federate *federate = NULL;

  // It may throw FederateNotExecutionMember
  federate = getByHandle(federate_handle);

  if(federate->isConstrained()) {
    D.Out(pdExcept, "Federate %d already constrained.", federate_handle);
    throw RTIinternalError("Time Regulating already enabled.");
  }

  federate->setConstrained(true);
  D.Out(pdTerm, "Federation %d: Federate %d is now constrained.",
	handle, federate_handle);
}

// ---------------------------------------------------------------------------
// addRegulator

void 
Federation::addRegulator(FederateHandle federate_handle, FederationTime time)
  throw(FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  NetworkMessage msg;
  Federate *federate = NULL;

  // It may throw FederateNotExecutionMember
  federate = getByHandle(federate_handle);

  // It may throw RTIinternalError if Federate was not regulators.
  regulators.insert(federate_handle, time);
  federate->setRegulator(true);
 
  D.Out(pdTerm, "Federation %d: Federate %d is now a regulator(Time=%f).",
	handle, federate_handle, time);
 
  msg.Type = m_SET_TIME_REGULATING;
  msg.Exception = e_NO_EXCEPTION;
  msg.NumeroFederation = handle;
  msg.NumeroFedere = federate_handle;
  msg.EstRegulateur = RTI_TRUE;
  msg.Date = time;

  this->broadcastAnyMessage(&msg, 0);
}

// ---------------------------------------------------------------------------
// broadcastAnyMessage

void Federation::broadcastAnyMessage(NetworkMessage *msg,
				     FederateHandle except_federate)
{
  int i;
  Federate *federate = NULL;
  Socket *socket = NULL;

  // Broadcast the message 'msg' to all Federates in the Federation
  // except to Federate whose Handle is 'Except_Federate'.

  for(i = 1; i <= lg; i++) {
    federate = Ieme(i);
    if(federate->getHandle() != except_federate) {
      try {
#ifdef HLA_USES_UDP
	socket = server->getSocketLink(federate->Handle, BEST_EFFORT);
#else
	socket = server->getSocketLink(federate->getHandle());
#endif 
	msg->write(socket);
      }
      catch(RTIinternalError &e) {
	D.Out(pdExcept, 
	      "Reference to a killed Federate while broadcasting.");
      }
      catch(NetworkError &e) {
	D.Out(pdExcept, "Network error while broadcasting, ignoring.");
      }
    }
  }

  // BUG: If except = 0, could use Multicast. 
}

// ---------------------------------------------------------------------------
// broadcastInteraction

void 
Federation::
broadcastInteraction(FederateHandle federate_handle,
		     InteractionClassHandle interaction,
		     ParameterHandle *parameter_handles,
		     ParameterValue *parameter_values,
		     UShort list_size,
		     FederationTime time,
		     const char*  tag)
  throw(FederateNotExecutionMember,
	FederateNotPublishing,
	InteractionClassNotDefined,
	InteractionParameterNotDefined,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  // It may throw FederateNotExecutionMember.
  this->check(federate_handle);

  root->Interactions->broadcastInteraction(federate_handle,
					   interaction,
					   parameter_handles,
					   parameter_values,
					   list_size,
					   time,
					   tag);
  D.Out(pdRequest, "Federation %d: Broadcasted Interaction %d from Federate "
	"%d nb params %d.", handle, interaction, federate_handle, list_size);
  for(int i=0 ; i < list_size ; i++) 
    D.Out(pdRequest, 
	  " Param %d Value %s",
	  parameter_handles[i], parameter_values[i]);
}

// ---------------------------------------------------------------------------
// deleteObject

void 
Federation::deleteObject(FederateHandle federate,
			 ObjectHandle id,
			 const char*  tag)
  throw(FederateNotExecutionMember,
	DeletePrivilegeNotHeld,
	ObjectNotKnown,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  // It may throw FederateNotExecutionMember.
  this->check(federate);

  D.Out(pdRegister, "Federation %d: Federate %d destroys object %d.",
	this->handle, federate, id);

  root->ObjectClasses->deleteObject(federate, id, tag);
}

// ---------------------------------------------------------------------------
// enterPause

void 
Federation::enterPause(FederateHandle federate, PauseLabel label)
  throw(FederateNotExecutionMember,
	FederationAlreadyPaused,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  NetworkMessage msg;

  // It may throw FederateNotExecutionMember.
  this->check(federate);

  if(this->paused == true)
    throw FederationAlreadyPaused();

  if((label == NULL) ||
     (strlen(label) > MAX_USER_TAG_LENGTH))
    throw RTIinternalError("Bad pause label(null or too long).");
 
  strcpy(pauseLabel, label);
  this->paused = true ;

  D.Out(pdTerm, "Federation %d is now Paused.", handle);

  msg.Type = m_REQUEST_PAUSE;
  msg.Exception = e_NO_EXCEPTION;
  msg.NumeroFedere = federate;
  msg.NumeroFederation = handle;
  strcpy(msg.Label, pauseLabel);
 
  broadcastAnyMessage(&msg, federate);
}

// ---------------------------------------------------------------------------
// getByHandle

Federate* 
Federation::getByHandle(FederateHandle federate_handle)
  throw(FederateNotExecutionMember)
{
  int i = 0;
  Federate *federate = NULL;

  for(i = 1; i <= lg; i++) {
    federate = Ieme(i);
    if(federate->getHandle() == federate_handle)
      return federate;
  }

  throw FederateNotExecutionMember("Federate Handle not found.");
}

// ---------------------------------------------------------------------------
// getByName

Federate* 
Federation::getByName(FederateName federate_name)
  throw(FederateNotExecutionMember)
{
  int i = 0;
  Federate *federate = NULL;

  for(i = 1; i <= lg; i++) {
    federate = Ieme(i);
    if(strcmp(federate->getName(), federate_name) == 0)
      return federate;
  }

  throw FederateNotExecutionMember("Federate Name not found.");
}

// ---------------------------------------------------------------------------
// getNewHandle

FederateHandle 
Federation::getNewHandle(void)
  throw(RTIinternalError)
{
  if(nextFederateHandle > MAX_FEDERATEHANDLE) {
    D.Out(pdExcept, 
	  "Federation %d: Max Federate Handle count reached.", handle);
    throw RTIinternalError("Max Federate Handle reached.");
  }

  FederateHandle new_handle = nextFederateHandle ;
  nextFederateHandle++ ;
  return new_handle ;
}

// ---------------------------------------------------------------------------
// empty

bool 
Federation::empty()
  throw(FederatesCurrentlyJoined)
{
  if(lg == 0) return true ;
  else throw FederatesCurrentlyJoined();
}

// ---------------------------------------------------------------------------
// check

bool 
Federation::check(FederateHandle federate_handle)
  throw(FederateNotExecutionMember)
{
  int i = 0;
  Federate *federate = NULL;

  for(i = 1; i <= lg; i++) {
    federate = Ieme(i);
    if(federate->getHandle() == federate_handle)
      return true ;
  }
  throw FederateNotExecutionMember();
}

// ---------------------------------------------------------------------------
// killFederate

void 
Federation::kill(FederateHandle federate)
  throw()
{
  // NOTE: Connection to the federate is already closed.

  D.Out(pdInit, "Killing Federate %d.", federate);

  // is regulator ?
  try {
    removeRegulator(federate);
    D.Out(pdInit, "Regulator Federate %d removed...", federate);
  } catch(Exception &e) {}

  // is constrained ?
  try {
    removeConstrained(federate);
    D.Out(pdInit, "Constrained Federate %d removed...", federate);
  } catch(Exception &e) {}
 
  // Supprime les references au federe pour les classes d'objets
  D.Out(pdInit, "Federate %d removed from the Root Object tree...",
	federate);
  root->ObjectClasses->killFederate(federate);
 
  // Supprime les references au federe pour les classes d'interactions.
  D.Out(pdInit, "Federate %d removed from the Root Inteaction tree...",
	federate);
  root->Interactions->killFederate(federate);

  // delete from federations list
  try {
    remove(federate);
    D.Out(pdInit, "Federate %d removed...", federate);
  } catch(Exception &e) {}
}

// ---------------------------------------------------------------------------
// publishInteraction

void 
Federation::publishInteraction(FederateHandle federate,
			       InteractionClassHandle interaction,
			       bool pub)
  throw(InteractionClassNotDefined,
	FederateNotExecutionMember,
	SaveInProgress,
	SecurityError,
	RestoreInProgress,
	RTIinternalError)
{
  // It may throw FederateNotExecutionMember.
  this->check(federate);

  // It may throw InteractionClassNotDefined
  root->Interactions->publish(federate,
			      interaction,
			      pub);
  D.Out(pdRequest, 
	"Federation %d: Federate %d has(un)published Interaction %d.",
	handle, federate, interaction);
}

// ---------------------------------------------------------------------------
// publishObject

void 
Federation::publishObject(FederateHandle federate,
			  ObjectClassHandle object,
			  AttributeHandle *attributes,
			  UShort list_size,
			  bool pub)
  throw(ObjectClassNotDefined,
	AttributeNotDefined,
	FederateNotExecutionMember,
	SaveInProgress,
	SecurityError,
	RestoreInProgress,
	RTIinternalError)
{
  // It may throw FederateNotExecutionMember.
  this->check(federate);

  // It may throw *NotDefined
  root->ObjectClasses->publish(federate,
				  object,
				  attributes,
				  list_size,
				  pub);

  D.Out(pdRegister,
	"Federation %d: Federate %d(un)publishes %d attrib. of ObjClass %d.",
	handle, federate, list_size, object);
}

// ---------------------------------------------------------------------------
// registerObject

ObjectHandle 
Federation::registerObject(FederateHandle federate,
			   ObjectClassHandle class_handle,
			   ObjectName object_name)
  throw(FederateNotExecutionMember,
	FederateNotPublishing,
	ObjectAlreadyRegistered,
	ObjectClassNotDefined,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{ 
  ObjectHandle new_id = 0;

  // Allocate new ID.
  if(nextObjectId > MAX_OBJECTID){
    D.Out(pdError, 
	  "RTIinternalError : nextObjectId = %d, MAX = %d.",
	  nextObjectId, MAX_OBJECTID);
    throw RTIinternalError("Too many IDs requested.");
  }

  new_id = nextObjectId ;
  nextObjectId++;
 
  D.Out(pdRegister, 
	"Federation %d: Federate %d registering Object %d of Class %d.",
	handle, federate, new_id, class_handle);

  // Register Object.
  root->ObjectClasses->registerInstance(federate,
					   class_handle,
					   new_id,
					   object_name);
  D.Out(pdDebug,"suite");
  return new_id;
}

// ---------------------------------------------------------------------------
// removeFederate

void 
Federation::remove(FederateHandle federate_handle)
  throw(FederateOwnsAttributes,	FederateNotExecutionMember)
{
  int i = 0;
  Federate *federate = NULL;

  for(i = 1; i <= lg; i++) {
    federate = Ieme(i);
    if(federate->getHandle() == federate_handle) {
      // BUG: RemoveFederate: Should see if Federate owns attributes
      Supprimer(i);
      delete federate;

      D.Out(pdInit, "Federation %d: Removed Federate %d.", handle, federate_handle);
      return;
    }
  }

  D.Out(pdExcept, "Federation %d could not remove unknown federate %d.",
	handle, federate_handle);
  throw FederateNotExecutionMember();
} 

// ---------------------------------------------------------------------------
// removeConstrained

void 
Federation::removeConstrained(FederateHandle federate_handle)
  throw(FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  Federate *federate = NULL;

  // It may throw FederateNotExecutionMember
  federate = getByHandle(federate_handle);

  if(federate->isConstrained() == false) {
    D.Out(pdExcept, "Federate %d was not constrained.", federate_handle);
    throw RTIinternalError("Time constrained not enabled.");
  }

  federate->setConstrained(false);
  D.Out(pdTerm, "Federation %d: Federate %d is not constrained anymore.",
	handle, federate_handle);
}

// -------------------------------------------------------------------------- 
// removeRegulator

void 
Federation::removeRegulator(FederateHandle federate_handle)
  throw(FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  NetworkMessage msg;
  Federate *federate = NULL;

  // It may throw FederateNotExecutionMember
  federate = getByHandle(federate_handle);

  // It may throw RTIinternalError if Federate was not regulators.
  regulators.remove(federate_handle);
 
  federate->setRegulator(false);

  D.Out(pdTerm, "Federation %d: Federate %d is not a regulator anymore.",
	handle, federate_handle);

  msg.Type = m_SET_TIME_REGULATING;
  msg.Exception = e_NO_EXCEPTION;
  msg.NumeroFederation = handle;
  msg.NumeroFedere = federate_handle;
  msg.EstRegulateur = RTI_FALSE;
  msg.Date = 0;

  broadcastAnyMessage(&msg, 0);
}

// --------------------------------------------------------------------------- 
// requestId

void 
Federation::requestId(ObjectHandlecount id_count,
		      ObjectHandle &first_id,
		      ObjectHandle &last_id)
  throw(TooManyIDsRequested)
{
  // BUG: Should note a security threat for this service.

  if(nextObjectId + id_count - 1 > MAX_OBJECTID)
    throw TooManyIDsRequested();

  first_id = nextObjectId ;
  last_id = nextObjectId + id_count - 1 ;
 
  D.Out(pdInit, "Federation %d returns %d new IDs starting at %d.",
	handle, id_count, first_id);

  nextObjectId = last_id + 1 ;
}

// ---------------------------------------------------------------------------
// resumePause

void 
Federation::resumePause(FederateHandle federate,
			PauseLabel label)
  throw(FederateNotExecutionMember,
	FederationNotPaused,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  NetworkMessage msg;

  // It may throw FederateNotExecutionMember.
  this->check(federate);

  if(!this->paused) throw FederationNotPaused();

  if((label == NULL) || (strlen(label) > MAX_USER_TAG_LENGTH))
    throw RTIinternalError("Bad pause label(null or too long).");

  // pauseLabel [0] = '\0';
  strcpy(pauseLabel, label);
  this->paused = false ;

  D.Out(pdTerm, "Federation %d is not Paused anymore.", handle);

  msg.Type = m_REQUEST_RESUME;
  msg.Exception = e_NO_EXCEPTION;
  msg.NumeroFederation = handle;
  msg.NumeroFedere = federate;
  strcpy(msg.Label,pauseLabel);

  broadcastAnyMessage(&msg, federate);
}

// ---------------------------------------------------------------------------
// subscribeInteraction

void 
Federation::subscribeInteraction(FederateHandle federate,
				 InteractionClassHandle interaction,
				 bool sub)
  throw(InteractionClassNotDefined,
	FederateNotExecutionMember,
	SaveInProgress,
	SecurityError,
	RestoreInProgress,
	RTIinternalError)
{
  // It may throw FederateNotExecutionMember.
  this->check(federate);

  // It may throw *NotDefined
  root->Interactions->subscribe(federate, interaction, sub);
  D.Out(pdRegister,
	"Federation %d: Federate %d(un)subscribes to Interaction %d.",
	handle, federate, interaction);
}

// ---------------------------------------------------------------------------
// subscribeObject

void 
Federation::subscribeObject(FederateHandle federate,
			    ObjectClassHandle object,
			    AttributeHandle *attributes,
			    UShort list_size,
			    bool sub)
  throw(ObjectClassNotDefined,
	AttributeNotDefined,
	FederateNotExecutionMember,
	SaveInProgress,
	SecurityError,
	RestoreInProgress,
	RTIinternalError)
{
  // It may throw FederateNotExecutionMember.
  this->check(federate);

  // It may throw *NotDefined
  root->ObjectClasses->subscribe(federate, object, attributes, list_size, sub);
  D.Out(pdRegister,
	"Federation %d: Federate %d(un)sub. to %d attrib. of ObjClass %d.",
	handle, federate, list_size, object);
 
}

// ---------------------------------------------------------------------------
// updateAttributeValues

void 
Federation::updateAttributeValues(FederateHandle federate,
				  ObjectHandle id,
				  AttributeHandle *attributes,
				  AttributeValue *values,
				  UShort list_size,
				  FederationTime time,
				  const char*  tag)
  throw(FederateNotExecutionMember,
	ObjectNotKnown,
	AttributeNotDefined,
	AttributeNotOwned,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  // It may throw FederateNotExecutionMember.
  this->check(federate);

  // It may throw *NotDefined
  root->ObjectClasses->updateAttributeValues(federate,
					     id,
					     attributes,
					     values,
					     list_size,
					     time,
					     tag);

  D.Out(pdRegister,
	"Federation %d: Federate %d updated attributes of Object %d.",
	handle, federate, id);
 
}

// ---------------------------------------------------------------------------
// updateRegulator

void Federation::updateRegulator(FederateHandle federate_handle,
				 FederationTime time)
  throw(FederateNotExecutionMember,
	RTIinternalError)
{
  NetworkMessage msg;
  Federate *federate = NULL;

  // It may throw FederateNotExecutionMember
  federate = getByHandle(federate_handle);

  if(federate->isRegulator() == false) {
    D.Out(pdExcept, "Federate %d is not a regulator.", federate_handle);
    throw RTIinternalError("Time regulation not enabled.");
  }

  D.Out(pdTerm, "Federation %d: Federate %d's new time is %f.",
	handle, federate_handle, time);

  regulators.update(federate_handle, time);
 
  msg.Type = m_MESSAGE_NULL;
  msg.Exception = e_NO_EXCEPTION;
  msg.NumeroFederation = handle;
  msg.NumeroFedere = federate_handle;
  msg.Date = time;

  broadcastAnyMessage(&msg, federate_handle);
}

// ---------------------------------------------------------------------------
// isOwner (isAttributeOwnedByFederate)

bool
Federation::isOwner(FederateHandle federate,
		    ObjectHandle id,
		    AttributeHandle attribute)
  throw(FederateNotExecutionMember,
	ObjectNotKnown,
	AttributeNotDefined,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{ 
  // It may throw FederateNotExecutionMember.
  this->check(federate);

  D.Out(pdDebug,"Owner of Object %u Atrribute %u", id, attribute);
 
  // It may throw *NotDefined
  return(root->ObjectClasses->isAttributeOwnedByFederate(id, 
							 attribute, 
							 federate));
}

// ---------------------------------------------------------------------------
// queryAttributeOwnership

void 
Federation::queryAttributeOwnership(FederateHandle federate,
				    ObjectHandle id,
				    AttributeHandle attribute)
  throw(FederateNotExecutionMember,
	ObjectNotKnown,
	AttributeNotDefined,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError)
{
  // It may throw FederateNotExecutionMember.
  this->check(federate);

  D.Out(pdDebug,"Owner of Object %u Atrribute %u", id,attribute);
 
  // It may throw *NotDefined
  root->ObjectClasses->queryAttributeOwnership(id, attribute, federate);
}

// ---------------------------------------------------------------------------
// negotiateDivestiture (negotiatedAttributeOwnershipDivestiture)

void 
Federation::negotiateDivestiture(FederateHandle federate,
				 ObjectHandle id,
				 AttributeHandle *attribs,
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
  // It may throw FederateNotExecutionMember.
  this->check(federate);

  // It may throw *NotDefined
  root->ObjectClasses->negotiatedAttributeOwnershipDivestiture(federate,
							       id,
							       attribs,
							       list_size,
							       tag);
}

// ---------------------------------------------------------------------------
// acquireIfAvailable (attributeOwnershipAcquisitionIfAvailable)

void 
Federation::acquireIfAvailable(FederateHandle federate,
			       ObjectHandle id,
			       AttributeHandle *attribs,
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
  // It may throw FederateNotExecutionMember.
  this->check(federate);

  // It may throw *NotDefined
  root->ObjectClasses->attributeOwnershipAcquisitionIfAvailable(federate,
								id,
								attribs,
								list_size); 
}

// ---------------------------------------------------------------------------
// divest (unconditionalAttributeOwnershipDivestiture)

void 
Federation::divest(FederateHandle federate,
		   ObjectHandle id,
		   AttributeHandle *attrs,
		   UShort list_size) 
  throw(ObjectNotKnown,
	AttributeNotDefined,
	AttributeNotOwned,
	FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError) 
{ 
  // It may throw FederateNotExecutionMember.
  this->check(federate);

  // It may throw *NotDefined
  root->ObjectClasses->unconditionalAttributeOwnershipDivestiture(federate,
								  id,
								  attrs,
								  list_size);
}

// ---------------------------------------------------------------------------
// attributeOwnershipAcquisition

void 
Federation::acquire(FederateHandle federate,
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
  // It may throw FederateNotExecutionMember.
  this->check(federate);

  // It may throw *NotDefined
  root->ObjectClasses->attributeOwnershipAcquisition(federate,
						     id,
						     attributes,
						     list_size,
						     tag);
 
  D.Out(pdDebug,"Acquisition on Object %u ", id);
 
}

// ---------------------------------------------------------------------------
// cancelDivestiture (cancelNegotiatedAttributeOwnershipDivestiture)

void 
Federation::
cancelDivestiture(FederateHandle federate,
		  ObjectHandle id,
		  AttributeHandle *attributes,
		  UShort list_size)  
  throw(ObjectNotKnown,
	AttributeNotDefined,
	AttributeNotOwned,
	AttributeDivestitureWasNotRequested,
	FederateNotExecutionMember,
	SaveInProgress,
	RestoreInProgress,
	RTIinternalError) 
{
  // It may throw FederateNotExecutionMember.
  this->check(federate);

  // It may throw *NotDefined
  root->ObjectClasses->
    cancelNegotiatedAttributeOwnershipDivestiture(federate, 
						  id, 
						  attributes,
						  list_size);
 
  D.Out(pdDebug,"CancelDivestiture sur Objet %u ", id); 
}

// ---------------------------------------------------------------------------
// respondRelease (attributeOwnershipRealeaseResponse)

AttributeHandleSet*
Federation::respondRelease(FederateHandle federate,
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
  // It may throw FederateNotExecutionMember.
  this->check(federate);

  D.Out(pdDebug,"Response sur Objet %u ", id);

  // It may throw *NotDefined
  return(root->ObjectClasses->attributeOwnershipRealeaseResponse(federate,
								 id,
								 attributes,
								 list_size));
}

// ---------------------------------------------------------------------------
// cancelAttributeOwnershipAcquisition

void 
Federation::cancelAcquisition(FederateHandle federate,
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
  // It may throw FederateNotExecutionMember.
  this->check(federate);

  D.Out(pdDebug,"CancelAcquisition sur Objet %u ", id);

  // It may throw *NotDefined
  root->ObjectClasses->cancelAttributeOwnershipAcquisition(federate,
							   id,
							   attributes,
							   list_size);
} 

}}

// $Id: Federation.cc,v 3.1 2002/11/26 15:48:01 breholee Exp $

