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
// $Id: FederationsList.hh,v 3.1 2002/11/26 15:48:01 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_RTIG_FEDERATIONS_LIST_HH
#define _CERTI_RTIG_FEDERATIONS_LIST_HH

#include <config.h>

#include "List.hh"
#include "Federation.hh"
#include "SocketMC.hh"
#include "SocketServer.hh"
#include "AuditFile.hh"
#include "PrettyDebug.hh"

namespace certi {
namespace rtig {

class FederationsList : private List <Federation*>
{
  // ATTRIBUTES --------------------------------------------------------------
private:
  SocketServer *socketServer;
  AuditFile *auditFile;

  // METHODS -----------------------------------------------------------------
public:
  // constructor/destructor
  FederationsList(SocketServer*, AuditFile*);
  ~FederationsList();
 
  // ---------------------------
  // -- Federation Management --
  // ---------------------------

  // MAX_FEDERATION is the maximum number of federations.
#ifdef FEDERATION_USES_MULTICAST
  void createFederation(FederationExecutionName, FederationHandle, SocketMC*)
#else
  void createFederation(FederationExecutionName, FederationHandle)
#endif
    throw(FederationExecutionAlreadyExists,
	  CouldNotOpenRID,
	  ErrorReadingRID,
	  MemoryExhausted,
	  SecurityError,
	  RTIinternalError);

  // Return the Handle of the Federation named "theName" if it is found
  // in the FederationList, else throw FederationExecutionDoesNotExist.
  void exists(FederationExecutionName theName, FederationHandle &theHandle)
    throw(FederationExecutionDoesNotExist, RTIinternalError);
 
  void destroyFederation(FederationHandle)
    throw(FederatesCurrentlyJoined,
	  FederationExecutionDoesNotExist,
	  RTIinternalError);
 
#ifdef FEDERATION_USES_MULTICAST
  void info(FederationHandle theHandle,
	    int &nb_federes,
	    int &nb_regulateurs,
	    bool &est_pause,
	    SocketMC* &comm_mc)
#else
  void info(FederationHandle theHandle,
	    int &nb_federes,
	    int &nb_regulateurs,
	    bool &est_pause)
#endif
    throw(FederationExecutionDoesNotExist, RTIinternalError);

  void requestId(FederationHandle theHandle, 
		 ObjectHandlecount IDCount,
		 ObjectHandle &FirstID,
		 ObjectHandle &LastID)
    throw(TooManyIDsRequested);

  // ----------------------
  // -- Pause Management --
  // ----------------------

  void setPause(FederationHandle theHandle,
		FederateHandle theFederate,
		bool state,
		PauseLabel theLabel)
    throw(FederationAlreadyPaused,
	  FederationNotPaused,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);
 
 
  // -------------------------
  // -- Federate Management --
  // -------------------------

  // Create a new Federate if its name is unique, and return its new
  // Federate Handle. Also send Null messages from all others federates
  // to initialize its LBTS, and finally a RequestPause message if the
  // Federation is already paused.
  FederateHandle addFederate(FederationHandle theHandle,
			     FederateName theFederateName,
			     SocketTCP *theTCPLink)
    throw(FederationExecutionDoesNotExist,
	  FederateAlreadyExecutionMember,
	  MemoryExhausted,
	  RTIinternalError);
 
  void remove(FederationHandle theHandle,
	      FederateHandle num_federe)
    throw(FederationExecutionDoesNotExist,
	  FederateOwnsAttributes,
	  FederateNotExecutionMember,
	  RTIinternalError);
 
  // This Method tries to remove all references to this Federate
  // in the Federation. To be used when a Federate is supposed to
  // have crashed.
  void killFederate(FederationHandle theHandle,
		    FederateHandle theFederateHandle)
    throw();

  // ---------------------
  // -- Time Management --
  // ---------------------

  void createRegulator(FederationHandle theHandle,
		       FederateHandle theFederateHandle,
		       FederationTime theTime)
    throw(FederationExecutionDoesNotExist,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);
 
  void updateRegulator(FederationHandle theHandle,
		       FederateHandle theFederateHandle,
		       FederationTime heure_logique)
    throw(FederationExecutionDoesNotExist,
	  FederateNotExecutionMember,
	  RTIinternalError);
 
  void removeRegulator(FederationHandle theHandle,
		       FederateHandle theFederateHandle)
    throw(FederationExecutionDoesNotExist,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);
 
  void addConstrained(FederationHandle theHandle,
		      FederateHandle theFederateHandle)
    throw(FederationExecutionDoesNotExist,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);
 
  void removeConstrained(FederationHandle theHandle,
			 FederateHandle theFederateHandle)
    throw(FederationExecutionDoesNotExist,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);

  // -------------------------------------------
  // -- Object Classes & Instances Management --
  // -------------------------------------------

  void publishObject(FederationHandle theHandle,
		     FederateHandle theFederateHandle,
		     ObjectClassHandle theObjectHandle,
		     AttributeHandle *theAttributeList,
		     UShort theListSize,
		     bool pub)
    throw(ObjectClassNotDefined,
	  AttributeNotDefined,
	  FederationExecutionDoesNotExist,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  RestoreInProgress,
	  SecurityError,
	  RTIinternalError);
 
  void subscribeObject(FederationHandle theHandle,
		       FederateHandle theFederateHandle,
		       ObjectClassHandle theObjectHandle,
		       AttributeHandle *theAttributeList,
		       UShort theListSize,
		       bool subscribe_or_unsubscribe)
    throw(ObjectClassNotDefined,
	  AttributeNotDefined,
	  FederationExecutionDoesNotExist,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  RestoreInProgress,
	  SecurityError,
	  RTIinternalError);

  ObjectHandle registerObject(FederationHandle theHandle,
			 FederateHandle theFederateHandle,
			 ObjectClassHandle theClass,
			 ObjectName theName)
    throw(FederateNotExecutionMember,
	  FederateNotPublishing,
	  ObjectAlreadyRegistered,
	  ObjectClassNotDefined,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);
 
  void destroyObject(FederationHandle theHandle,
		     FederateHandle theFederateHandle,
		     ObjectHandle theObjectHandle,
		     const char*  theUserTag)
    throw(FederateNotExecutionMember,
	  FederationExecutionDoesNotExist,
	  DeletePrivilegeNotHeld,
	  ObjectNotKnown,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);
 
  void updateAttribute(FederationHandle theHandle, 
		       FederateHandle theFederateHandle,
		       ObjectHandle theObjectHandle,
		       AttributeHandle *theAttributeList,
		       AttributeValue *theValueList,
		       UShort theListSize,
		       FederationTime theTime,
		       const char*  theTag)
    throw(FederateNotExecutionMember,
	  FederationExecutionDoesNotExist,
	  ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeNotOwned,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);

  // -------------------------------------
  // -- Interactions Classes Management --
  // -------------------------------------

  void publishInteraction(FederationHandle theHandle,
			  FederateHandle theFederateHandle, 
			  InteractionClassHandle theInteractionHandle,
			  bool publish_or_unpublish)
    throw(InteractionClassNotDefined,
	  FederationExecutionDoesNotExist,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  SecurityError,
	  RestoreInProgress,
	  RTIinternalError);
 
  void subscribeInteraction(FederationHandle theHandle,
			    FederateHandle theFederateHandle, 
			    InteractionClassHandle theInteractionHandle,
			    bool subscribe_or_unsubscribe)
    throw(InteractionClassNotDefined,
	  FederationExecutionDoesNotExist,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  SecurityError,
	  RestoreInProgress,
	  RTIinternalError);
 
  void updateParameter(FederationHandle theFederation,
		       FederateHandle theFederateHandle,
		       InteractionClassHandle theInteractionHandle,
		       ParameterHandle *theParameterList,
		       ParameterValue *theValueList,
		       UShort theListSize,
		       FederationTime theTime,
		       const char*  theTag)
    throw(FederateNotExecutionMember,
	  FederateNotPublishing,
	  FederationExecutionDoesNotExist,
	  InteractionClassNotDefined,
	  InteractionParameterNotDefined,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);
 
 
  // --------------------------
  // -- Ownership Management --
  // -------------------------- 
  bool isOwner(FederationHandle theHandle,
	       FederateHandle theFederateHandle,
	       ObjectHandle theObjectHandle,
	       AttributeHandle theAttribute) 
    throw(FederateNotExecutionMember,
	  ObjectNotKnown,
	  AttributeNotDefined,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);
 
  void searchOwner(FederationHandle theHandle,
		   FederateHandle theFederateHandle,
		   ObjectHandle theObjectHandle,
		   AttributeHandle theAttribute) 
    throw(FederateNotExecutionMember,
	  ObjectNotKnown,
	  AttributeNotDefined,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);
 
  void negotiateDivestiture(FederationHandle,
			    FederateHandle,
			    ObjectHandle,
			    AttributeHandle*,
			    UShort,
			    const char *) 
    throw(FederateNotExecutionMember,
	  ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeNotOwned,
	  AttributeAlreadyBeingDivested, 
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError); 
 
  void acquireIfAvailable(FederationHandle,
			  FederateHandle,
			  ObjectHandle,
			  AttributeHandle*,
			  UShort) 
    throw(ObjectNotKnown,
	  ObjectClassNotPublished,
	  AttributeNotDefined,
	  AttributeNotPublished,
	  FederateOwnsAttributes,
	  AttributeAlreadyBeingAcquired,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);
 
  void divest(FederationHandle, 
	      FederateHandle, 
	      ObjectHandle, 
	      AttributeHandle*, 
	      UShort)
    throw(ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeNotOwned,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);

  void acquire(FederationHandle theHandle,
	       FederateHandle theFederateHandle,
	       ObjectHandle theObjectHandle,
	       AttributeHandle *theAttributeList,
	       UShort theListSize,
	       const char *theTag) 
    throw(ObjectNotKnown,
	  ObjectClassNotPublished,
	  AttributeNotDefined,
	  AttributeNotPublished,
	  FederateOwnsAttributes,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);
 
  void cancelDivestiture(FederationHandle theHandle,
			 FederateHandle theFederateHandle,
			 ObjectHandle theObjectHandle,
			 AttributeHandle *theAttributeList,
			 UShort theListSize) 
    throw(ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeNotOwned,
	  AttributeDivestitureWasNotRequested,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError); 
 
  AttributeHandleSet* respondRelease(FederationHandle theHandle,
				     FederateHandle theFederateHandle,
				     ObjectHandle theObjectHandle,
				     AttributeHandle *theAttributeList,
				     UShort theListSize)
    throw(ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeNotOwned,
	  FederateWasNotAskedToReleaseAttribute,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);
 
  void cancelAcquisition(FederationHandle theHandle,
			 FederateHandle theFederateHandle,
			 ObjectHandle theObjectHandle,
			 AttributeHandle *theAttributeList,
			 UShort theListSize)
     throw(ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeAlreadyOwned,
	  AttributeAcquisitionWasNotRequested,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError); 
 
  // PRIVATE METHODS -----
private:
  void checkHandle(FederationHandle theHandle) throw(RTIinternalError);
  // Throw an exception if Handle is not correct(too low or too big)

  void searchFederation(FederationHandle theHandle,
			Federation* &Federation,
			int &Rank)
    throw(FederationExecutionDoesNotExist, RTIinternalError);
};

}}

#endif // _CERTI_RTIG_FEDERATIONS_LIST_HH

// $Id: FederationsList.hh,v 3.1 2002/11/26 15:48:01 breholee Exp $
