// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003, 2004  ONERA
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
// $Id: FederationsList.hh,v 3.25 2005/02/09 15:43:07 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIG_FEDERATIONS_LIST_HH
#define _CERTI_RTIG_FEDERATIONS_LIST_HH

#include "Federation.hh"
#include "SocketMC.hh"
#include "SocketServer.hh"
#include "AuditFile.hh"
#include "PrettyDebug.hh"

#include <list>

namespace certi {
namespace rtig {

/*! FederationsList keep track on federation operations.
 */
class FederationsList : private std::list<Federation *>
{
private:
    SocketServer &socketServer ;
    AuditFile &auditFile ;

public:
    // constructor/destructor
    FederationsList(SocketServer &, AuditFile &);
    ~FederationsList();

    // ---------------------------
    // -- Federation Management --
    // ---------------------------

    // MAX_FEDERATION is the maximum number of federations.
#ifdef FEDERATION_USES_MULTICAST
    void createFederation(const char *, FederationHandle, SocketMC*)
#else
        void createFederation(const char *, Handle)
#endif
        throw (FederationExecutionAlreadyExists,
               CouldNotOpenRID,
               ErrorReadingRID,
               MemoryExhausted,
               SecurityError,
               RTIinternalError);

    void exists(const char *name, Handle &handle)
        throw (FederationExecutionDoesNotExist, RTIinternalError);

    void destroyFederation(Handle)
        throw (FederatesCurrentlyJoined, FederationExecutionDoesNotExist,
               RTIinternalError);

#ifdef FEDERATION_USES_MULTICAST
    void info(FederationHandle theHandle,
              int &nb_federes,
              int &nb_regulateurs,
              bool &is_syncing,
              SocketMC* &comm_mc)
#else
        void info(Handle theHandle,
                  int &nb_federes,
                  int &nb_regulateurs,
                  bool &is_syncing)
#endif
        throw (FederationExecutionDoesNotExist, RTIinternalError);

    // Synchronization Management
    void manageSynchronization(Handle theHandle,
                               FederateHandle theFederate,
                               bool state,
                               const char *the_label,
                               const char *the_tag)
        throw (FederationAlreadyPaused,
               FederationNotPaused,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void broadcastSynchronization(Handle handle,
                                  FederateHandle federate,
                                  const char *label,
                                  const char *tag)
        throw (FederationExecutionDoesNotExist,
               RTIinternalError);

    void requestFederationSave(Handle, FederateHandle, const char *,
                               FederationTime);

    void federateSaveBegun(Handle, FederateHandle);
    void federateSaveStatus(Handle, FederateHandle, bool);

    void requestFederationRestore(Handle the_federation,
                                  FederateHandle the_federate,
                                  const char *the_label);

    void federateRestoreStatus(Handle the_federation,
                               FederateHandle the_federate,
                               bool the_status);

    // -------------------------
    // -- Federate Management --
    // -------------------------
    FederateHandle addFederate(Handle theHandle,
                               const char *theFederateName,
                               SocketTCP *theTCPLink)
        throw (FederationExecutionDoesNotExist,
               FederateAlreadyExecutionMember,
               MemoryExhausted,
               RTIinternalError);

    void remove(Handle, FederateHandle)
        throw (FederationExecutionDoesNotExist,
               FederateOwnsAttributes,
               FederateNotExecutionMember,
               RTIinternalError);

    void killFederate(Handle, FederateHandle)
        throw ();

    // ---------------------
    // -- Time Management --
    // ---------------------

    void createRegulator(Handle theHandle,
                         FederateHandle theFederateHandle,
                         FederationTime theTime)
        throw (FederationExecutionDoesNotExist,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void updateRegulator(Handle theHandle,
                         FederateHandle theFederateHandle,
                         FederationTime heure_logique)
        throw (FederationExecutionDoesNotExist,
               FederateNotExecutionMember,
               RTIinternalError);

    void removeRegulator(Handle theHandle,
                         FederateHandle theFederateHandle)
        throw (FederationExecutionDoesNotExist,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void addConstrained(Handle theHandle,
                        FederateHandle theFederateHandle)
        throw (FederationExecutionDoesNotExist,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void removeConstrained(Handle theHandle,
                           FederateHandle theFederateHandle)
        throw (FederationExecutionDoesNotExist,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    // -------------------------------------------
    // -- Object Classes & Instances Management --
    // -------------------------------------------

    void publishObject(Handle theHandle,
                       FederateHandle theFederateHandle,
                       ObjectClassHandle theObjectHandle,
                       AttributeHandle *theAttributeList,
                       UShort theListSize,
                       bool pub)
        throw (ObjectClassNotDefined,
               AttributeNotDefined,
               FederationExecutionDoesNotExist,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               SecurityError,
               RTIinternalError);

    void subscribeObject(Handle theHandle,
                         FederateHandle theFederateHandle,
                         ObjectClassHandle theObjectHandle,
                         AttributeHandle *theAttributeList,
                         UShort theListSize,
                         bool subscribe_or_unsubscribe)
        throw (ObjectClassNotDefined,
               AttributeNotDefined,
               FederationExecutionDoesNotExist,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               SecurityError,
               RTIinternalError);

    ObjectHandle registerObject(Handle theHandle,
                                FederateHandle theFederateHandle,
                                ObjectClassHandle theClass,
                                ObjectName theName)
        throw (FederateNotExecutionMember,
               FederateNotPublishing,
               ObjectAlreadyRegistered,
               ObjectClassNotDefined,
               ObjectClassNotPublished,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void destroyObject(Handle theHandle,
                       FederateHandle theFederateHandle,
                       ObjectHandle theObjectHandle,
                       const char *theUserTag)
        throw (FederateNotExecutionMember,
               FederationExecutionDoesNotExist,
               DeletePrivilegeNotHeld,
               ObjectNotKnown,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void updateAttribute(Handle theHandle,
                         FederateHandle theFederateHandle,
                         ObjectHandle theObjectHandle,
                         AttributeHandle *theAttributeList,
                         AttributeValue *theValueList,
                         UShort theListSize,
                         FederationTime theTime,
                         const char *theTag)
        throw (FederateNotExecutionMember,
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

    void publishInteraction(Handle theHandle,
                            FederateHandle theFederateHandle,
                            InteractionClassHandle theInteractionHandle,
                            bool publish_or_unpublish)
        throw (InteractionClassNotDefined,
               FederationExecutionDoesNotExist,
               FederateNotExecutionMember,
               SaveInProgress,
               SecurityError,
               RestoreInProgress,
               RTIinternalError);

    void subscribeInteraction(Handle theHandle,
                              FederateHandle theFederateHandle,
                              InteractionClassHandle theInteractionHandle,
                              bool subscribe_or_unsubscribe)
        throw (InteractionClassNotDefined,
               FederationExecutionDoesNotExist,
               FederateNotExecutionMember,
               SaveInProgress,
               SecurityError,
               RestoreInProgress,
               RTIinternalError);

    void updateParameter(Handle theFederation,
                         FederateHandle theFederateHandle,
                         InteractionClassHandle theInteractionHandle,
                         ParameterHandle *theParameterList,
                         ParameterValue *theValueList,
                         UShort theListSize,
                         FederationTime theTime,
                         const char *theTag)
        throw (FederateNotExecutionMember,
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
    bool isOwner(Handle theHandle,
                 FederateHandle theFederateHandle,
                 ObjectHandle theObjectHandle,
                 AttributeHandle theAttribute)
        throw (FederateNotExecutionMember,
               ObjectNotKnown,
               AttributeNotDefined,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void searchOwner(Handle theHandle,
                     FederateHandle theFederateHandle,
                     ObjectHandle theObjectHandle,
                     AttributeHandle theAttribute)
        throw (FederateNotExecutionMember,
               ObjectNotKnown,
               AttributeNotDefined,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void negotiateDivestiture(Handle,
                              FederateHandle,
                              ObjectHandle,
                              AttributeHandle*,
                              UShort,
                              const char *)
        throw (FederateNotExecutionMember,
               ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               AttributeAlreadyBeingDivested,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void acquireIfAvailable(Handle,
                            FederateHandle,
                            ObjectHandle,
                            AttributeHandle*,
                            UShort)
        throw (ObjectNotKnown,
               ObjectClassNotPublished,
               AttributeNotDefined,
               AttributeNotPublished,
               FederateOwnsAttributes,
               AttributeAlreadyBeingAcquired,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void divest(Handle,
                FederateHandle,
                ObjectHandle,
                AttributeHandle*,
                UShort)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void acquire(Handle theHandle,
                 FederateHandle theFederateHandle,
                 ObjectHandle theObjectHandle,
                 AttributeHandle *theAttributeList,
                 UShort theListSize,
                 const char *theTag)
        throw (ObjectNotKnown,
               ObjectClassNotPublished,
               AttributeNotDefined,
               AttributeNotPublished,
               FederateOwnsAttributes,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void cancelDivestiture(Handle theHandle,
                           FederateHandle theFederateHandle,
                           ObjectHandle theObjectHandle,
                           AttributeHandle *theAttributeList,
                           UShort theListSize)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               AttributeDivestitureWasNotRequested,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    AttributeHandleSet* respondRelease(Handle theHandle,
                                       FederateHandle theFederateHandle,
                                       ObjectHandle theObjectHandle,
                                       AttributeHandle *theAttributeList,
                                       UShort theListSize)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               FederateWasNotAskedToReleaseAttribute,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void cancelAcquisition(Handle theHandle,
                           FederateHandle theFederateHandle,
                           ObjectHandle theObjectHandle,
                           AttributeHandle *theAttributeList,
                           UShort theListSize)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeAlreadyOwned,
               AttributeAcquisitionWasNotRequested,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    // Data Distribution Management
    long createRegion(Handle, FederateHandle, SpaceHandle, long)
        throw (SpaceNotDefined, InvalidExtents, FederateNotExecutionMember,
               SaveInProgress, RestoreInProgress, RTIinternalError);

    void modifyRegion(Handle, FederateHandle, RegionHandle,
		      const std::vector<Extent> &)
	throw (InvalidExtents, SaveInProgress, RestoreInProgress,
	       RTIinternalError);
    
    void deleteRegion(Handle, FederateHandle, long)
        throw (RegionNotKnown, RegionInUse, FederateNotExecutionMember,
               SaveInProgress, RestoreInProgress, RTIinternalError);

    void associateRegion(Handle, FederateHandle, ObjectHandle, RegionHandle,
			 unsigned short, AttributeHandle *)
	throw (RegionInUse, FederateNotExecutionMember, SaveInProgress,
	       RestoreInProgress, RTIinternalError);

    void unassociateRegion(Handle, FederateHandle, ObjectHandle, RegionHandle)
	throw (RegionInUse, FederateNotExecutionMember, SaveInProgress,
	       RestoreInProgress, RTIinternalError);

    void subscribeAttributesWR(Handle, FederateHandle, ObjectClassHandle,
			       RegionHandle, unsigned short, AttributeHandle *)
	throw (FederateNotExecutionMember, SaveInProgress, RestoreInProgress,
	       RTIinternalError);

    void unsubscribeAttributesWR(Handle, FederateHandle, ObjectClassHandle,
				 RegionHandle)
	throw (FederateNotExecutionMember, SaveInProgress, RestoreInProgress,
	       RTIinternalError);

    void subscribeInteractionWR(Handle, FederateHandle, InteractionClassHandle,
				RegionHandle)
	throw (FederateNotExecutionMember, SaveInProgress, RestoreInProgress,
	       RTIinternalError);
    
    void unsubscribeInteractionWR(Handle, FederateHandle,
				  InteractionClassHandle, RegionHandle)
	throw (FederateNotExecutionMember, SaveInProgress, RestoreInProgress,
	       RTIinternalError);

    ObjectHandle registerObjectWithRegion(Handle, FederateHandle, 
					  ObjectClassHandle, ObjectName,
					  RegionHandle, int, AttributeHandle *)
	throw (ObjectClassNotDefined, ObjectClassNotPublished,
	       AttributeNotDefined, AttributeNotPublished, RegionNotKnown,
	       InvalidRegionContext, ObjectAlreadyRegistered,
	       FederateNotExecutionMember, SaveInProgress, RestoreInProgress,
	       RTIinternalError);               

    void setVerbose(bool flag) { verbose = flag ; }
    
private:
    // Private methods
    void checkHandle(Handle theHandle) throw (RTIinternalError);
    int searchFederation(Handle the_handle, Federation* &federation)
        throw (FederationExecutionDoesNotExist, RTIinternalError);

    bool verbose ;
};

}} // namespace certi/rtig

#endif // _CERTI_RTIG_FEDERATIONS_LIST_HH

// $Id: FederationsList.hh,v 3.25 2005/02/09 15:43:07 breholee Exp $
