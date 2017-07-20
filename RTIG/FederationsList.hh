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

#ifndef _CERTI_RTIG_FEDERATIONS_LIST_HH
#define _CERTI_RTIG_FEDERATIONS_LIST_HH

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "BaseRegion.hh"
#include "Exception.hh"
#include "FedTimeD.hh"
#include "certi.hh"

namespace certi {

class AttributeHandleSet;
class AuditFile;
class Extent;
class NM_Join_Federation_Execution;
class SocketServer;
class SocketTCP;

namespace rtig {

class Federation;

/**
 * FederationsList keeps track on federation operations.
 * There is one federation list in each RTIG instance.
 * You usually have only one RTIG instance.
 */
class FederationsList {
public:
    // constructor/destructor
    FederationsList(SocketServer& server, AuditFile& audit);
    ~FederationsList();

// ---------------------------
// -- Federation Management --
// ---------------------------

#ifdef FEDERATION_USES_MULTICAST
    void createFederation(const std::string&, FederationHandle, SocketMC*)
#else
    void createFederation(const std::string&, Handle, const std::string& FEDid)
#endif
        throw(FederationExecutionAlreadyExists,
              CouldNotOpenFED,
              ErrorReadingFED,
              MemoryExhausted,
              SecurityError,
              RTIinternalError);

    /** Return the Handle of the Federation named "name" if it is found in the
     * FederationList, else throw FederationExecutionDoesNotExist.
     */
    Handle getFederationHandle(const std::string& name) throw(FederationExecutionDoesNotExist);

    void destroyFederation(Handle) throw(FederatesCurrentlyJoined, FederationExecutionDoesNotExist, RTIinternalError);

#ifdef FEDERATION_USES_MULTICAST
    void info(FederationHandle theHandle, int& nb_federes, int& nb_regulateurs, bool& is_syncing, SocketMC*& comm_mc)
#else
    void info(Handle theHandle, int& nb_federes, int& nb_regulateurs, bool& is_syncing)
#endif
        throw(FederationExecutionDoesNotExist, RTIinternalError);

    // Synchronization Management

    /** Called by processRegisterSynchronization and
     * processSynchronizationAchieved.
     */
    void manageSynchronization(Handle theHandle,
                               FederateHandle theFederate,
                               bool state,
                               const std::string& the_label,
                               const std::string& the_tag) throw(FederationExecutionDoesNotExist,
                                                                 FederationAlreadyPaused,
                                                                 FederationNotPaused,
                                                                 FederateNotExecutionMember,
                                                                 SaveInProgress,
                                                                 RestoreInProgress,
                                                                 RTIinternalError);

    void manageSynchronization(Handle theHandle,
                               FederateHandle theFederate,
                               bool state,
                               const std::string& the_label,
                               const std::string& the_tag,
                               unsigned short federate_setSize,
                               const std::vector<FederateHandle>& federate_set) throw(FederationExecutionDoesNotExist,
                                                                                      FederationAlreadyPaused,
                                                                                      FederationNotPaused,
                                                                                      FederateNotExecutionMember,
                                                                                      SaveInProgress,
                                                                                      RestoreInProgress,
                                                                                      RTIinternalError);

    /// Called by processRegisterSynchronization.
    void broadcastSynchronization(Handle handle,
                                  FederateHandle federate,
                                  const std::string& label,
                                  const std::string& tag) throw(FederationExecutionDoesNotExist, RTIinternalError);

    /** Called by processRegisterSynchronization.
     * 
     * Broadcast only on the federates into a set
     */
    void
    broadcastSynchronization(Handle handle,
                             FederateHandle federate,
                             const std::string& label,
                             const std::string& tag,
                             unsigned short federate_setSize,
                             const std::vector<FederateHandle>& federate_set) throw(FederationExecutionDoesNotExist,
                                                                                    RTIinternalError);

    void requestFederationSave(Handle the_federation,
                               FederateHandle the_federate,
                               const std::string& the_label,
                               FederationTime the_time);

    void requestFederationSave(Handle the_federation, FederateHandle the_federate, const std::string& the_label);

    void federateSaveBegun(Handle, FederateHandle);
    void federateSaveStatus(Handle, FederateHandle, bool);

    void requestFederationRestore(Handle the_federation, FederateHandle the_federate, const std::string& the_label);

    void federateRestoreStatus(Handle the_federation, FederateHandle the_federate, bool the_status);

    // -------------------------
    // -- Federate Management --
    // -------------------------

    /** Creates a new Federate if its name is unique, and returns its new
     * Federate Handle. Also sends Null messages from all other federates
     * to initialize its LBTS, and finally a RequestPause message if the
     * Federation is already paused.
     */
    FederateHandle addFederate(Handle theHandle,
                               const std::string& theFederateName,
                               SocketTCP* theTCPLink,
                               NM_Join_Federation_Execution& objectModelData) throw(FederationExecutionDoesNotExist,
                                                                                    FederateAlreadyExecutionMember,
                                                                                    MemoryExhausted,
                                                                                    RTIinternalError);

    void remove(Handle, FederateHandle) throw(FederationExecutionDoesNotExist,
                                              FederateOwnsAttributes,
                                              FederateNotExecutionMember,
                                              RTIinternalError);

    void setClassRelevanceAdvisorySwitch(Handle theHandle,
                                         FederateHandle theFederateHandle) throw(FederationExecutionDoesNotExist,
                                                                                 FederateNotExecutionMember,
                                                                                 SaveInProgress,
                                                                                 RestoreInProgress,
                                                                                 RTIinternalError);

    void unsetClassRelevanceAdvisorySwitch(Handle theHandle,
                                           FederateHandle theFederateHandle) throw(FederationExecutionDoesNotExist,
                                                                                   FederateNotExecutionMember,
                                                                                   SaveInProgress,
                                                                                   RestoreInProgress,
                                                                                   RTIinternalError);

    void setInteractionRelevanceAdvisorySwitch(Handle theHandle,
                                               FederateHandle theFederateHandle) throw(FederationExecutionDoesNotExist,
                                                                                       FederateNotExecutionMember,
                                                                                       SaveInProgress,
                                                                                       RestoreInProgress,
                                                                                       RTIinternalError);

    void
    unsetInteractionRelevanceAdvisorySwitch(Handle theHandle,
                                            FederateHandle theFederateHandle) throw(FederationExecutionDoesNotExist,
                                                                                    FederateNotExecutionMember,
                                                                                    SaveInProgress,
                                                                                    RestoreInProgress,
                                                                                    RTIinternalError);

    void setAttributeRelevanceAdvisorySwitch(Handle theHandle,
                                             FederateHandle theFederateHandle) throw(FederationExecutionDoesNotExist,
                                                                                     FederateNotExecutionMember,
                                                                                     SaveInProgress,
                                                                                     RestoreInProgress,
                                                                                     RTIinternalError);

    void unsetAttributeRelevanceAdvisorySwitch(Handle theHandle,
                                               FederateHandle theFederateHandle) throw(FederationExecutionDoesNotExist,
                                                                                       FederateNotExecutionMember,
                                                                                       SaveInProgress,
                                                                                       RestoreInProgress,
                                                                                       RTIinternalError);

    void setAttributeScopeAdvisorySwitch(Handle theHandle,
                                         FederateHandle theFederateHandle) throw(FederationExecutionDoesNotExist,
                                                                                 FederateNotExecutionMember,
                                                                                 SaveInProgress,
                                                                                 RestoreInProgress,
                                                                                 RTIinternalError);

    void unsetAttributeScopeAdvisorySwitch(Handle theHandle,
                                           FederateHandle theFederateHandle) throw(FederationExecutionDoesNotExist,
                                                                                   FederateNotExecutionMember,
                                                                                   SaveInProgress,
                                                                                   RestoreInProgress,
                                                                                   RTIinternalError);

    /** This Method tries to remove all references to this Federate in the
     * Federation. To be used when a Federate is supposed to have crashed.
     */
    void killFederate(Handle, FederateHandle) throw();

    // ---------------------
    // -- Time Management --
    // ---------------------

    void createRegulator(Handle theHandle,
                         FederateHandle theFederateHandle,
                         FederationTime theTime) throw(FederationExecutionDoesNotExist,
                                                       FederateNotExecutionMember,
                                                       SaveInProgress,
                                                       RestoreInProgress,
                                                       RTIinternalError);

    void updateRegulator(FederationHandle theHandle,
                         FederateHandle theFederateHandle,
                         FederationTime heure_logique,
                         bool anonymous) throw(FederationExecutionDoesNotExist,
                                               FederateNotExecutionMember,
                                               RTIinternalError);

    void removeRegulator(Handle theHandle, FederateHandle theFederateHandle) throw(FederationExecutionDoesNotExist,
                                                                                   FederateNotExecutionMember,
                                                                                   SaveInProgress,
                                                                                   RestoreInProgress,
                                                                                   RTIinternalError);

    /// Adds a constrained federate to federation.
    void addConstrained(Handle theHandle, FederateHandle theFederateHandle) throw(FederationExecutionDoesNotExist,
                                                                                  FederateNotExecutionMember,
                                                                                  SaveInProgress,
                                                                                  RestoreInProgress,
                                                                                  RTIinternalError);

    void removeConstrained(Handle theHandle, FederateHandle theFederateHandle) throw(FederationExecutionDoesNotExist,
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
                       const std::vector<AttributeHandle>& theAttributeList,
                       bool pub) throw(ObjectClassNotDefined,
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
                         const std::vector<AttributeHandle>& theAttributeList) throw(ObjectClassNotDefined,
                                                                                     AttributeNotDefined,
                                                                                     FederationExecutionDoesNotExist,
                                                                                     FederateNotExecutionMember,
                                                                                     SaveInProgress,
                                                                                     RestoreInProgress,
                                                                                     SecurityError,
                                                                                     RTIinternalError);

    void reserveObjectInstanceName(Handle federationHandle,
                                   FederateHandle the_federate,
                                   std::string newObjName) throw(FederationExecutionDoesNotExist,
                                                                 IllegalName,
                                                                 FederateNotExecutionMember,
                                                                 SaveInProgress,
                                                                 RestoreInProgress,
                                                                 RTIinternalError);

    ObjectHandle registerObject(Handle theHandle,
                                FederateHandle theFederateHandle,
                                ObjectClassHandle theClass,
                                ObjectName_t theName) throw(FederationExecutionDoesNotExist,
                                                            FederateNotExecutionMember,
                                                            FederateNotPublishing,
                                                            ObjectAlreadyRegistered,
                                                            ObjectClassNotDefined,
                                                            ObjectClassNotPublished,
                                                            SaveInProgress,
                                                            RestoreInProgress,
                                                            RTIinternalError);

    FederateHandle
    requestObjectOwner(Handle handle,
                       FederateHandle federate,
                       ObjectHandle id,
                       const std::vector<AttributeHandle>& attributes,
                       uint32_t list_size) throw(ObjectNotKnown, FederationExecutionDoesNotExist, RTIinternalError);

    void requestClassAttributeValueUpdate(Handle theHandle,
                                          FederateHandle federate,
                                          ObjectClassHandle classHandle,
                                          const std::vector<AttributeHandle>& attributes,
                                          uint32_t list_size) throw(ObjectClassNotDefined,
                                                                    FederationExecutionDoesNotExist,
                                                                    RTIinternalError);

    void destroyObject(Handle theHandle,
                       FederateHandle theFederateHandle,
                       ObjectHandle theObjectHandle,
                       FederationTime theTime,
                       const std::string& theUserTag) throw(FederateNotExecutionMember,
                                                            FederationExecutionDoesNotExist,
                                                            DeletePrivilegeNotHeld,
                                                            ObjectNotKnown,
                                                            SaveInProgress,
                                                            RestoreInProgress,
                                                            InvalidFederationTime,
                                                            RTIinternalError);

    void destroyObject(Handle theHandle,
                       FederateHandle theFederateHandle,
                       ObjectHandle theObjectHandle,
                       const std::string& theUserTag) throw(FederateNotExecutionMember,
                                                            FederationExecutionDoesNotExist,
                                                            DeletePrivilegeNotHeld,
                                                            ObjectNotKnown,
                                                            SaveInProgress,
                                                            RestoreInProgress,
                                                            RTIinternalError);

    void updateAttribute(Handle theHandle,
                         FederateHandle theFederateHandle,
                         ObjectHandle theObjectHandle,
                         const std::vector<AttributeHandle>& theAttributeList,
                         const std::vector<AttributeValue_t>& theValueList,
                         uint16_t theListSize,
                         FederationTime theTime,
                         const std::string& theTag) throw(FederateNotExecutionMember,
                                                          FederationExecutionDoesNotExist,
                                                          ObjectNotKnown,
                                                          AttributeNotDefined,
                                                          AttributeNotOwned,
                                                          SaveInProgress,
                                                          RestoreInProgress,
                                                          RTIinternalError);

    void updateAttribute(Handle theHandle,
                         FederateHandle theFederateHandle,
                         ObjectHandle theObjectHandle,
                         const std::vector<AttributeHandle>& theAttributeList,
                         const std::vector<AttributeValue_t>& theValueList,
                         uint16_t theListSize,
                         const std::string& theTag) throw(FederateNotExecutionMember,
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
                            bool publish_or_unpublish) throw(InteractionClassNotDefined,
                                                             FederationExecutionDoesNotExist,
                                                             FederateNotExecutionMember,
                                                             SaveInProgress,
                                                             SecurityError,
                                                             RestoreInProgress,
                                                             RTIinternalError);

    void subscribeInteraction(Handle theHandle,
                              FederateHandle theFederateHandle,
                              InteractionClassHandle theInteractionHandle,
                              bool subscribe_or_unsubscribe) throw(InteractionClassNotDefined,
                                                                   FederationExecutionDoesNotExist,
                                                                   FederateNotExecutionMember,
                                                                   SaveInProgress,
                                                                   SecurityError,
                                                                   RestoreInProgress,
                                                                   RTIinternalError);

    void updateParameter(Handle theFederation,
                         FederateHandle theFederateHandle,
                         InteractionClassHandle theInteractionHandle,
                         const std::vector<ParameterHandle>& theParameterList,
                         const std::vector<ParameterValue_t>& theValueList,
                         uint16_t theListSize,
                         FederationTime theTime,
                         RegionHandle,
                         const std::string& theTag) throw(FederateNotExecutionMember,
                                                          FederateNotPublishing,
                                                          FederationExecutionDoesNotExist,
                                                          InteractionClassNotDefined,
                                                          InteractionParameterNotDefined,
                                                          SaveInProgress,
                                                          RestoreInProgress,
                                                          RTIinternalError);

    void updateParameter(Handle theFederation,
                         FederateHandle theFederateHandle,
                         InteractionClassHandle theInteractionHandle,
                         const std::vector<ParameterHandle>& theParameterList,
                         const std::vector<ParameterValue_t>& theValueList,
                         uint16_t theListSize,
                         RegionHandle,
                         const std::string& theTag) throw(FederateNotExecutionMember,
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
                 AttributeHandle theAttribute) throw(FederationExecutionDoesNotExist,
                                                     FederateNotExecutionMember,
                                                     ObjectNotKnown,
                                                     AttributeNotDefined,
                                                     SaveInProgress,
                                                     RestoreInProgress,
                                                     RTIinternalError);

    void searchOwner(Handle theHandle,
                     FederateHandle theFederateHandle,
                     ObjectHandle theObjectHandle,
                     AttributeHandle theAttribute) throw(FederationExecutionDoesNotExist,
                                                         FederateNotExecutionMember,
                                                         ObjectNotKnown,
                                                         AttributeNotDefined,
                                                         SaveInProgress,
                                                         RestoreInProgress,
                                                         RTIinternalError);

    void negotiateDivestiture(Handle,
                              FederateHandle,
                              ObjectHandle,
                              const std::vector<AttributeHandle>&,
                              uint16_t,
                              const std::string&) throw(FederationExecutionDoesNotExist,
                                                        FederateNotExecutionMember,
                                                        ObjectNotKnown,
                                                        AttributeNotDefined,
                                                        AttributeNotOwned,
                                                        AttributeAlreadyBeingDivested,
                                                        SaveInProgress,
                                                        RestoreInProgress,
                                                        RTIinternalError);

    void acquireIfAvailable(Handle, FederateHandle, ObjectHandle, const std::vector<AttributeHandle>&, uint16_t) throw(
        FederationExecutionDoesNotExist,
        ObjectNotKnown,
        ObjectClassNotPublished,
        AttributeNotDefined,
        AttributeNotPublished,
        FederateOwnsAttributes,
        AttributeAlreadyBeingAcquired,
        FederateNotExecutionMember,
        SaveInProgress,
        RestoreInProgress,
        RTIinternalError);

    void divest(Handle, FederateHandle, ObjectHandle, const std::vector<AttributeHandle>&, uint16_t) throw(
        FederationExecutionDoesNotExist,
        ObjectNotKnown,
        AttributeNotDefined,
        AttributeNotOwned,
        FederateNotExecutionMember,
        SaveInProgress,
        RestoreInProgress,
        RTIinternalError);

    void acquire(Handle theHandle,
                 FederateHandle theFederateHandle,
                 ObjectHandle theObjectHandle,
                 const std::vector<AttributeHandle>& theAttributeList,
                 uint16_t theListSize,
                 const std::string& theTag) throw(FederationExecutionDoesNotExist,
                                                  ObjectNotKnown,
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
                           const std::vector<AttributeHandle>& theAttributeList,
                           uint16_t theListSize) throw(FederationExecutionDoesNotExist,
                                                       ObjectNotKnown,
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
                                       const std::vector<AttributeHandle>& theAttributeList,
                                       uint16_t theListSize) throw(FederationExecutionDoesNotExist,
                                                                   ObjectNotKnown,
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
                           const std::vector<AttributeHandle>& theAttributeList,
                           uint16_t theListSize) throw(FederationExecutionDoesNotExist,
                                                       ObjectNotKnown,
                                                       AttributeNotDefined,
                                                       AttributeAlreadyOwned,
                                                       AttributeAcquisitionWasNotRequested,
                                                       FederateNotExecutionMember,
                                                       SaveInProgress,
                                                       RestoreInProgress,
                                                       RTIinternalError);

    // Data Distribution Management
    long createRegion(Handle, FederateHandle, SpaceHandle, long) throw(FederationExecutionDoesNotExist,
                                                                       SpaceNotDefined,
                                                                       InvalidExtents,
                                                                       FederateNotExecutionMember,
                                                                       SaveInProgress,
                                                                       RestoreInProgress,
                                                                       RTIinternalError);

    void modifyRegion(Handle, FederateHandle, RegionHandle, const std::vector<Extent>&) throw(
        FederationExecutionDoesNotExist, InvalidExtents, SaveInProgress, RestoreInProgress, RTIinternalError);

    void deleteRegion(Handle, FederateHandle, long) throw(FederationExecutionDoesNotExist,
                                                          RegionNotKnown,
                                                          RegionInUse,
                                                          FederateNotExecutionMember,
                                                          SaveInProgress,
                                                          RestoreInProgress,
                                                          RTIinternalError);

    void associateRegion(Handle,
                         FederateHandle,
                         ObjectHandle,
                         RegionHandle,
                         unsigned short,
                         const std::vector<AttributeHandle>&) throw(FederationExecutionDoesNotExist,
                                                                    RegionInUse,
                                                                    FederateNotExecutionMember,
                                                                    SaveInProgress,
                                                                    RestoreInProgress,
                                                                    RTIinternalError);

    void unassociateRegion(Handle, FederateHandle, ObjectHandle, RegionHandle) throw(FederationExecutionDoesNotExist,
                                                                                     RegionInUse,
                                                                                     FederateNotExecutionMember,
                                                                                     SaveInProgress,
                                                                                     RestoreInProgress,
                                                                                     RTIinternalError);

    void subscribeAttributesWR(Handle,
                               FederateHandle,
                               ObjectClassHandle,
                               RegionHandle,
                               unsigned short,
                               const std::vector<AttributeHandle>&) throw(FederationExecutionDoesNotExist,
                                                                          FederateNotExecutionMember,
                                                                          SaveInProgress,
                                                                          RestoreInProgress,
                                                                          RTIinternalError);

    void unsubscribeAttributesWR(Handle,
                                 FederateHandle,
                                 ObjectClassHandle,
                                 RegionHandle) throw(FederationExecutionDoesNotExist,
                                                     FederateNotExecutionMember,
                                                     SaveInProgress,
                                                     RestoreInProgress,
                                                     RTIinternalError);

    void subscribeInteractionWR(Handle,
                                FederateHandle,
                                InteractionClassHandle,
                                RegionHandle) throw(FederationExecutionDoesNotExist,
                                                    FederateNotExecutionMember,
                                                    SaveInProgress,
                                                    RestoreInProgress,
                                                    RTIinternalError);

    void unsubscribeInteractionWR(Handle,
                                  FederateHandle,
                                  InteractionClassHandle,
                                  RegionHandle) throw(FederationExecutionDoesNotExist,
                                                      FederateNotExecutionMember,
                                                      SaveInProgress,
                                                      RestoreInProgress,
                                                      RTIinternalError);

    ObjectHandle registerObjectWithRegion(Handle,
                                          FederateHandle,
                                          ObjectClassHandle,
                                          ObjectName_t,
                                          RegionHandle,
                                          int,
                                          const std::vector<AttributeHandle>&) throw(FederationExecutionDoesNotExist,
                                                                                     ObjectClassNotDefined,
                                                                                     ObjectClassNotPublished,
                                                                                     AttributeNotDefined,
                                                                                     AttributeNotPublished,
                                                                                     RegionNotKnown,
                                                                                     InvalidRegionContext,
                                                                                     ObjectAlreadyRegistered,
                                                                                     FederateNotExecutionMember,
                                                                                     SaveInProgress,
                                                                                     RestoreInProgress,
                                                                                     RTIinternalError);

    void setVerboseLevel(int theVerboseLevel);

    /** Handle a Null Prime message.
     * 
     * @param[in] federation the federation handle
     * @param[in] federate the handle of the federate which is sending a NULL PRIME message.
     * @param[in] date the date of the NULL PRIME message
     * @return True is the NULL PRIME message changed the minNERx of the federation
     *         False otherwise.
     */
    bool handleMessageNullPrime(FederationHandle federation, FederateHandle federate, FederationTime date);

    /** Get the Null Prime value of the specified federation
     * 
     * @param[in] federation specific federation
     * @return the Null Prime value of the federation
     */
    FederationTime getNullPrimeValue(FederationHandle federation);

private:
    /** Search federation from handle.
     * 
     * @param[in] federationHandle the handle of the search federation
     * @throw FederationExecutionDoesNotExist if the provided federation handle
     *        does not match any created federation.
     */
    Federation* searchFederation(Handle federationHandle) throw(FederationExecutionDoesNotExist);

    SocketServer& socketServer;
    AuditFile& auditFile;

    int verboseLevel;

    typedef std::map<Handle, Federation*> HandleFederationMap;
    HandleFederationMap _handleFederationMap;
};
}
} // namespace certi/rtig

#endif // _CERTI_RTIG_FEDERATIONS_LIST_HH
