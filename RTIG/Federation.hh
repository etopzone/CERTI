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
// $Id: Federation.hh,v 3.0.2.1 2002/11/22 18:44:00 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_RTIG_FEDERATION_HH
#define _CERTI_RTIG_FEDERATION_HH

#include <config.h>

#include <time.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "List.hh"
#include "Federate.hh"
#include "RootObject.hh"
#include "LBTS.hh"
#include "SecurityServer.hh"
#include "RTItypes.hh"
#include "FedParser.hh" // CRead : FED file reader.
#include "PrettyDebug.hh"

#ifdef FEDERATION_USES_MULTICAST
#include "SocketMC.hh"
#endif

using namespace certi ;

namespace certi {
namespace rtig {

class Federation : private List <Federate *>
{
  // ATTRIBUTES --------------------------------------------------------------
private:
  FederationHandle handle;
  FederationExecutionName name; 
  bool paused ;
  char pauseLabel[MAX_USER_TAG_LENGTH + 1];
  ObjectHandle nextObjectId ;
  FederateHandle nextFederateHandle ;

  // This object is initialized when the Federation is created, with
  // the reference of the RTIG managed Socket Server. The reference of
  // this object is passed down the Classes Tree with the help of RootObj.
  SecurityServer *server;
  RootObject *root;

  LBTS regulators;

#ifdef FEDERATION_USES_MULTICAST
  SocketMC *MCLink;
#endif

  // METHODS -----------------------------------------------------------------
public:
  // Allocates memory the Name's storage,
  // and read its FED file to store the result in RootObj.
#ifdef FEDERATION_USES_MULTICAST
  Federation(FederationExecutionName, 
	     FederationHandle, 
	     SocketServer*, 
	     AuditFile*,
	     SocketMC*)
#else
  Federation(FederationExecutionName,
	     FederationHandle,
	     SocketServer*,
	     AuditFile*)
#endif
    throw(CouldNotOpenRID, ErrorReadingRID, MemoryExhausted, SecurityError,
	  RTIinternalError);

  // Please check empty() before deleting a Federation.
  //(it can be bypassed in case of emergency, but then Federates will be 
  // deleted without asking any questions!)
  ~Federation();

  // Return 'IDCount' unique IDs, ranging from FirstID to LastID.
  void requestId(ObjectHandlecount IDCount,
		 ObjectHandle &FirstID,
		 ObjectHandle &LastID)
    throw(TooManyIDsRequested);
 
  int getNbFederates(void);
  int getNbRegulators(void);
  bool isPaused(void);
  FederationHandle getHandle(void);
  FederationExecutionName getName(void);

  // -------------------------
  // -- Federate Management --
  // -------------------------

  // Add the Federate to the Federation, and return its new federate
  // handle.  MAX_FEDERATE is the maximum number of federates per
  // federation.  Also send Null messages from all others federates to
  // initialize its LBTS, and finally a RequestPause message if the
  // Federation is already paused.
  FederateHandle add(FederateName theName, SocketTCP *theTCPLink)
    throw(FederateAlreadyExecutionMember, MemoryExhausted, RTIinternalError);

  // Return true if there are no Federates left in the Federation,
  // else throw FederatesCurrentlyJoined.
  bool empty() throw(FederatesCurrentlyJoined);

  // Return true if the Federate is part of the Federation, else throw
  // an exception.
  bool check(FederateHandle theHandle)
    throw(FederateNotExecutionMember);

  // This Method tries to remove all references to this Federate in
  // the Federation. To be used when a Federate is supposed to have
  // crashed.
  void kill(FederateHandle theFederate) throw();
 
  // BUG: Currently does not check if Federate owns attributes.  The
  // Federate Object is deleted.
  void remove(FederateHandle theHandle)
    throw(FederateOwnsAttributes, FederateNotExecutionMember);

  // ---------------------
  // -- Time Management --
  // ---------------------

  // Add the Federate to the Regulators List. Check if it's already
  // present, but not if the Time 'theTime' is allowed or not.
  void addRegulator(FederateHandle theHandle, FederationTime theTime)
    throw(FederateNotExecutionMember,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError); // includes Time Regulation already enabled.

  // Update the current time of a regulator federate.
  void updateRegulator(FederateHandle theHandle, FederationTime theTime)
    throw(FederateNotExecutionMember,
	  RTIinternalError);
 
  // Remove the Federate from the Regulators' list.
  void removeRegulator(FederateHandle theHandle)
    throw(FederateNotExecutionMember,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError); // includes Time Regulation already disabled.

  // Set Federate's IsConstrained to true.
  void addConstrained(FederateHandle theHandle)  // FIXME: name/ merge with
    throw(FederateNotExecutionMember,            // removeConstrained  
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError); // includes Time constrained already enabled.

  // Set Federate's IsConstrained to false.
  void removeConstrained(FederateHandle theHandle)
    throw(FederateNotExecutionMember,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError); // includes Time constrained already disabled.

  // ----------------------
  // -- Pause Management --
  // ----------------------

  void enterPause(FederateHandle theFederate, PauseLabel theLabel)
    throw(FederateNotExecutionMember,
	  FederationAlreadyPaused,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);
 
  void resumePause(FederateHandle theFederate, PauseLabel theLabel)
    throw(FederateNotExecutionMember,
	  FederationNotPaused,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);

  // -----------------------
  // -- Object Management --
  // -----------------------

  void deleteObject(FederateHandle theFederateHandle,
		    ObjectHandle theObjectHandle,
		    const char*  theUserTag)
    throw(FederateNotExecutionMember,
	  DeletePrivilegeNotHeld,
	  ObjectNotKnown,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);

  void publishObject(FederateHandle theFederateHandle,
		     ObjectClassHandle theObjectHandle,
		     AttributeHandle *theAttributeList,
		     UShort theListSize,
		     bool PubOrUnpub)
    throw(ObjectClassNotDefined,
	  AttributeNotDefined,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  SecurityError,
	  RestoreInProgress,
	  RTIinternalError);

  ObjectHandle registerObject(FederateHandle theFederateHandle,
			  ObjectClassHandle theClass,
			  ObjectName theName)
    throw(FederateNotExecutionMember,
	  FederateNotPublishing,
	  ObjectAlreadyRegistered,
	  ObjectClassNotDefined,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);

  void subscribeObject(FederateHandle theFederateHandle,
		       ObjectClassHandle theObjectHandle,
		       AttributeHandle *theAttributeList,
		       UShort theListSize,
		       bool SubOrUnSub)
    throw(ObjectClassNotDefined,
	  AttributeNotDefined,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  SecurityError,
	  RestoreInProgress,
	  RTIinternalError);

  void updateAttributeValues(FederateHandle theFederateHandle,
			     ObjectHandle theObjectHandle,
			     AttributeHandle *theAttributeList,
			     AttributeValue *theValueList,
			     UShort theListSize,
			     FederationTime theTime,
			     const char*  theTag)
    throw(FederateNotExecutionMember,
	  ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeNotOwned,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);
 
  // ----------------------------
  // -- Interaction Management --
  // ----------------------------

  void broadcastInteraction(FederateHandle theFederateHandle,
			    InteractionClassHandle theInteractionHandle,
			    ParameterHandle *theParameterList,
			    ParameterValue *theValueList,
			    UShort theListSize,
			    FederationTime theTime,
			    const char*  theTag)
    throw(FederateNotExecutionMember,
	  FederateNotPublishing,
	  InteractionClassNotDefined,
	  InteractionParameterNotDefined,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);

  void publishInteraction(FederateHandle theFederateHandle,
			  InteractionClassHandle theInteractionHandle,
			  bool PubOrUnpub)
    throw(InteractionClassNotDefined,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  SecurityError,
	  RestoreInProgress,
	  RTIinternalError);

  void subscribeInteraction(FederateHandle theFederateHandle,
			    InteractionClassHandle theInteractionHandle,
			    bool SubOrUnsub)
    throw(InteractionClassNotDefined,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  SecurityError,
	  RestoreInProgress,
	  RTIinternalError);
 
 
  // --------------------------
  // -- Ownership Management --
  // --------------------------
 
  bool isOwner(FederateHandle theFederateHandle,
				  ObjectHandle theObjectHandle,
				  AttributeHandle theAttribute)
    throw(FederateNotExecutionMember,
	  ObjectNotKnown,
	  AttributeNotDefined,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);
 
  void queryAttributeOwnership(FederateHandle theFederateHandle,
			       ObjectHandle theObjectHandle,
			       AttributeHandle theAttribute)
    throw(FederateNotExecutionMember,
	  ObjectNotKnown,
	  AttributeNotDefined,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError);
 
  void negotiateDivestiture(FederateHandle theFederateHandle,
			    ObjectHandle theObjectHandle,
			    AttributeHandle *theAttributeList,
			    UShort theListSize,
			    const char *theTag)
    throw(FederateNotExecutionMember,
	  ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeNotOwned,
	  AttributeAlreadyBeingDivested, 
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError); 

  void acquireIfAvailable(FederateHandle theFederateHandle,
			  ObjectHandle theObjectHandle,
			  AttributeHandle *theAttributeList,
			  UShort theListSize) 
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

  void divest(FederateHandle theFederateHandle,
	      ObjectHandle theObjectHandle,
	      AttributeHandle *theAttributeList,
	      UShort theListSize) 
    throw(ObjectNotKnown,
	  AttributeNotDefined,
	  AttributeNotOwned,
	  FederateNotExecutionMember,
	  SaveInProgress,
	  RestoreInProgress,
	  RTIinternalError); 

  void acquire(FederateHandle theFederateHandle,
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

  void cancelDivestiture(FederateHandle theFederateHandle,
			 ObjectHandle theObjectHandle,
			 AttributeHandle *theAttributeList,
			 UShort theListSize)
    throw( ObjectNotKnown,
	   AttributeNotDefined,
	   AttributeNotOwned,
	   AttributeDivestitureWasNotRequested,
	   FederateNotExecutionMember,
	   SaveInProgress,
	   RestoreInProgress,
	   RTIinternalError); 

  AttributeHandleSet* respondRelease(FederateHandle theFederateHandle,
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
 
  void cancelAcquisition(FederateHandle theFederateHandle,
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
 
private:
  // ---------------------
  // -- Private Methods --
  // ---------------------
  
  // Broadcast 'msg' to all Federate except the one whose Handle is 'Except'.
  void broadcastAnyMessage(NetworkMessage *msg, FederateHandle Except); 

  // Return a pointer of the Federate whose Name is theName, if found.
  Federate *getByName(FederateName theName)
    throw(FederateNotExecutionMember);
 
  // Return a pointer of the Federate whose Handle is theHandle, if found.
  Federate *getByHandle(FederateHandle theHandle)
    throw(FederateNotExecutionMember);

  // Return a brand new Federate Handle.  Throw an exception if there
  // are no more handles left(MAX_FEDERATEHANDLE)
  FederateHandle getNewHandle(void)
    throw(RTIinternalError);
};

}}

#endif // _CERTI_RTIG_FEDERATION_HH

// $Id: Federation.hh,v 3.0.2.1 2002/11/22 18:44:00 breholee Exp $
