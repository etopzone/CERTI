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

#ifndef _CERTI_RTIG_FEDERATION_HH
#define _CERTI_RTIG_FEDERATION_HH

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <memory>
#include <unordered_map>

#include "certi.hh"

#include "BaseRegion.hh"
#include "BasicMessage.hh"
#include "Exception.hh"
#include "FedTimeD.hh"
#include "Federate.hh"
#include "HandleManager.hh"
#include "LBTS.hh"
#include "MessageBuffer.hh"

#ifdef FEDERATION_USES_MULTICAST
#include "SocketMC.hh"
#endif

namespace certi {
class AttributeHandleSet;
class AuditFile;
class Extent;
class NM_Join_Federation_Execution;
class NetworkMessage;
class RootObject;
class SecurityServer;
class SocketServer;
class SocketTCP;

namespace rtig {

class Federation {
public:
#ifdef FEDERATION_USES_MULTICAST
    /**
	 * Allocates memory the Name's storage, and read its FED file to store the
	 * result in RootObj.
	 *   (with FEDERATION_USES_MULTICAST defined).
	 *  @param federation_name
	 *  @param federation_handle
	 *  @param socket_server
	 *  @param audit_server
	 *  @param mc_link
	 */
    Federation(const std::string& federation_name,
               const FederationHandle federation_handle,
               SocketServer& socket_server,
               AuditFile& audit_server,
               SocketMC* mc_link,
               const int verboseLevel)
#else
    /**
     * Allocates memory the Name's storage, and read its FED file to store the
     * result in RootObj.
     * with FEDERATION_USES_MULTICAST not defined
     * @param federation_name Name of the federation
     * @param federation_handle Handle of the federation
     * @param socket_server Socket server from RTIG
     * @param audit_server Audit server from RTIG
     * @param FEDid_name i.e. FED file name (may be a .fed or a .xml file)
     */
    Federation(const std::string& federation_name,
               const FederationHandle federation_handle,
               SocketServer& socket_server,
               AuditFile& audit_server,
               const std::string& FEDid_name,
               int theVerboseLevel)
#endif
        throw(CouldNotOpenFED, ErrorReadingFED, MemoryExhausted, SecurityError, RTIinternalError);

    ~Federation();

    /// Returns the federation handle.
    FederationHandle getHandle() const;

    /// Returns the federation name given in 'Create Federation Execution'.
    std::string getName() const;

    /// Returns the FEDid name given in 'Create Federation Execution'.
    std::string getFEDid() const;

    /// Returns the number of federates in federation.
    int getNbFederates() const;

    /// Returns the number of federates regulator.
    int getNbRegulators() const;

    /// Return true if federation is being synchronized.
    bool isSynchronizing() const;

    // -------------------------
    // -- Federate Management --
    // -------------------------

    /** Add the Federate to the Federation, and return its new federate handle.
     * 
     * Also send Null messages from all others federates to initialize its LBTS, and
     * finally a RequestPause message if the Federation is already paused.
     */
    FederateHandle add(const std::string& federate_name,
                       SocketTCP* tcp_link) throw(FederateAlreadyExecutionMember, MemoryExhausted, RTIinternalError);

    /** Return true if there are no Federates left in the Federation
     * 
     * else throw FederatesCurrentlyJoined.
     */
    bool empty() const;

    /** Check whether if the federate is part of the federation.
     * 
     * @param federate_handle federate to check
     * @return true if the federate is part of the Federation,
     *              else throw an exception.
     */
    bool check(FederateHandle federate_handle) const;

    /** Make a federate resign the federation.
     *
     * This function tries to remove all references to this federate in the federation.
     * To be used when a Federate is supposed to have crashed.
     * 
     * NOTE: Connection to the federate is already closed.
     *
     * @param federate_handle Handle of the federate to kill
     */
    void kill(FederateHandle federate_handle) noexcept;

    /** Remove a federate.
     * 
     * NOTE: Is it normal it does not check if federate is used, e.g is regulator ?
     * 
     * @param federate_handle Handle of the federate to remove.
     * @bug Currently does not check if Federate owns attributes. The
     * Federate Object is deleted.
     */
    void remove(FederateHandle federate_handle) throw(FederateOwnsAttributes, FederateNotExecutionMember);

    // ---------------------
    // -- Time Management --
    // ---------------------

    /** Add the Federate to the Regulators List.
     * 
     * Check if it's already present, but not if the Time 'time' is allowed or not.
     */
    void addRegulator(FederateHandle federate_handle,
                      FederationTime time) throw(FederateNotExecutionMember,
                                                 SaveInProgress,
                                                 RestoreInProgress,
                                                 RTIinternalError); // includes Time Regulation already enabled.

    /// Update the current time of a regulator federate.
    void updateRegulator(FederateHandle federate_handle,
                         FederationTime time,
                         bool anonymous) throw(FederateNotExecutionMember, RTIinternalError);

    void removeRegulator(FederateHandle federate_handle) throw(
        FederateNotExecutionMember,
        SaveInProgress,
        RestoreInProgress,
        RTIinternalError); // includes Time Regulation already disabled.

    // Synchronization Management.

    /// Add a new synchronization point to federation.
    void
    registerSynchronization(FederateHandle federate_handle, const std::string& label, const std::string& tag) throw(
        FederateNotExecutionMember, FederationAlreadyPaused, SaveInProgress, RestoreInProgress, RTIinternalError);

    /// Add a new synchronization point (with federates set) to federation.
    void registerSynchronization(FederateHandle federate_handle,
                                 const std::string& label,
                                 const std::string& tag,
                                 const std::vector<FederateHandle>& federates) throw(FederateNotExecutionMember,
                                                                                     FederationAlreadyPaused,
                                                                                     SaveInProgress,
                                                                                     RestoreInProgress,
                                                                                     RTIinternalError);

    void unregisterSynchronization(FederateHandle federate_handle, const std::string& label) throw(
        FederateNotExecutionMember, FederationNotPaused, SaveInProgress, RestoreInProgress, RTIinternalError);

    /// Broadcast an 'Announce Synchronization Point' when registering a new synchronization point.
    void broadcastSynchronization(FederateHandle federate_handle,
                                  const std::string& label,
                                  const std::string& tag) throw(FederateNotExecutionMember, RTIinternalError);

    /// Broadcast an 'Announce Synchronization Point' when registering a new synchronization point onto a set of federates
    void broadcastSynchronization(FederateHandle federate_handle,
                                  const std::string& label,
                                  const std::string& tag,
                                  const std::vector<FederateHandle>& federates) throw(FederateNotExecutionMember,
                                                                                      RTIinternalError);

    // Save Management.

    /** Request a federation save with time.
     * 
     * This service puts each federate from federation in saving state.
     * time is not managed yet.
     */
    void requestFederationSave(FederateHandle federate_handle,
                               const std::string& label,
                               FederationTime time) throw(FederateNotExecutionMember, SaveInProgress);

    /** Request a federation save without time.
     * 
     * This service puts each federate from federation in saving state.
     * time is not managed yet.
     */
    void requestFederationSave(FederateHandle federate_handle,
                               const std::string& label) throw(FederateNotExecutionMember, SaveInProgress);

    /** Received from a federate to inform a save has been received and is being processed.
     * 
     * Maybe, a timeout should be set to determine if federate still alive.
     */
    void federateSaveBegun(FederateHandle federate_handle) throw(FederateNotExecutionMember);

    /// Informs that a federate returns a save end message (with success or not!).
    void federateSaveStatus(FederateHandle federate_handle, bool status) throw(FederateNotExecutionMember);

    /// Informs that a federate is requesting a save.
    void requestFederationRestore(FederateHandle federate_handle,
                                  const std::string& label) throw(FederateNotExecutionMember);

    /** Informs that a federate has ended a restore.
     * 
     * If each federate in federation has ended restoring, this service send a federation restore status.
     */
    void federateRestoreStatus(FederateHandle federate_handle, bool status) throw(FederateNotExecutionMember);

    // -----------------------
    // -- Object Management --
    // -----------------------

    /** Removes an object instance from federation.
     * 
     *  @param federate_handle Federate requesting removal
     *  @param object_handle Object handle
     *  @param time Federation Time
     *  @param user_tag Label for this operation
     */
    void deleteObject(FederateHandle federate_handle,
                      ObjectHandle object_handle,
                      FederationTime time,
                      const std::string& user_tag) throw(FederateNotExecutionMember,
                                                         DeletePrivilegeNotHeld,
                                                         ObjectNotKnown,
                                                         SaveInProgress,
                                                         RestoreInProgress,
                                                         InvalidFederationTime,
                                                         RTIinternalError);

    /** Removes an object instance from federation.
     * 
     *  @param federate_handle Federate requesting removal
     *  @param object_handle Object handle
     *  @param user_tag Label for this operation
     */
    void deleteObject(FederateHandle federate_handle,
                      ObjectHandle object_handle,
                      const std::string& user_tag) throw(FederateNotExecutionMember,
                                                         DeletePrivilegeNotHeld,
                                                         ObjectNotKnown,
                                                         SaveInProgress,
                                                         RestoreInProgress,
                                                         RTIinternalError);

    void publishObject(FederateHandle federate_handle,
                       ObjectClassHandle object_handle,
                       const std::vector<AttributeHandle>& attributes,
                       bool publish_or_unpublish) throw(ObjectClassNotDefined,
                                                        AttributeNotDefined,
                                                        FederateNotExecutionMember,
                                                        SaveInProgress,
                                                        SecurityError,
                                                        RestoreInProgress,
                                                        RTIinternalError);

    void reserveObjectInstanceName(FederateHandle federate_handle, std::string new_object_name) throw(IllegalName,
                                                                                                      SaveInProgress,
                                                                                                      RestoreInProgress,
                                                                                                      RTIinternalError);

    ObjectHandle registerObject(FederateHandle federate_handle,
                                ObjectClassHandle class_handle,
                                const std::string& name) throw(FederateNotExecutionMember,
                                                               FederateNotPublishing,
                                                               ObjectAlreadyRegistered,
                                                               ObjectClassNotDefined,
                                                               ObjectClassNotPublished,
                                                               SaveInProgress,
                                                               RestoreInProgress,
                                                               RTIinternalError);

    FederateHandle requestObjectOwner(FederateHandle federate_handle,
                                      ObjectHandle object_handle,
                                      const std::vector<AttributeHandle>& attributes) throw(ObjectNotKnown);

    void requestClassAttributeValueUpdate(FederateHandle federate_handle,
                                          ObjectClassHandle class_handle,
                                          const std::vector<AttributeHandle>& attributes) throw(ObjectClassNotDefined,
                                                                                                RTIinternalError);

    /**
     * Subscribes attributes to an object class. After subscription, attribute 
     * publishers are notified by the federate service 
     * startRegistrationForObjectClass.
     * @param[in] federate_handle federate handle of the subscriber
     * @param[in] object_handle subscripted object class handle 
     * @param[in] attributes subscripted vector of attributes 
     */
    void subscribeObject(FederateHandle federate_handle,
                         ObjectClassHandle object_handle,
                         const std::vector<AttributeHandle>& attributes) throw(ObjectClassNotDefined,
                                                                               AttributeNotDefined,
                                                                               FederateNotExecutionMember,
                                                                               SaveInProgress,
                                                                               SecurityError,
                                                                               RestoreInProgress,
                                                                               RTIinternalError);

    void updateAttributeValues(FederateHandle federate_handle,
                               ObjectHandle object_handle,
                               const std::vector<AttributeHandle>& attributes,
                               const std::vector<AttributeValue_t>& attribute_values,
                               FederationTime time,
                               const std::string& tag) throw(FederateNotExecutionMember,
                                                             ObjectNotKnown,
                                                             AttributeNotDefined,
                                                             AttributeNotOwned,
                                                             SaveInProgress,
                                                             RestoreInProgress,
                                                             RTIinternalError);

    void updateAttributeValues(FederateHandle federate_handle,
                               ObjectHandle object_handle,
                               const std::vector<AttributeHandle>& attributes,
                               const std::vector<AttributeValue_t>& attribute_values,
                               const std::string& tag) throw(FederateNotExecutionMember,
                                                             ObjectNotKnown,
                                                             AttributeNotDefined,
                                                             AttributeNotOwned,
                                                             SaveInProgress,
                                                             RestoreInProgress,
                                                             RTIinternalError);

    // ----------------------------
    // -- Interaction Management --
    // ----------------------------

    /// broadcastInteraction with time
    void broadcastInteraction(FederateHandle federate_handle,
                              InteractionClassHandle interaction_class_handle,
                              const std::vector<ParameterHandle>& theParameterList,
                              const std::vector<ParameterValue_t>& attribute_values,
                              FederationTime time,
                              RegionHandle region,
                              const std::string& tag) throw(FederateNotExecutionMember,
                                                            FederateNotPublishing,
                                                            InteractionClassNotDefined,
                                                            InteractionParameterNotDefined,
                                                            SaveInProgress,
                                                            RestoreInProgress,
                                                            RTIinternalError);

    /// broadcastInteraction without time
    void broadcastInteraction(FederateHandle federate_handle,
                              InteractionClassHandle interaction_class_handle,
                              const std::vector<ParameterHandle>& parameters,
                              const std::vector<ParameterValue_t>& parameters_values,
                              RegionHandle region,
                              const std::string& tag) throw(FederateNotExecutionMember,
                                                            FederateNotPublishing,
                                                            InteractionClassNotDefined,
                                                            InteractionParameterNotDefined,
                                                            SaveInProgress,
                                                            RestoreInProgress,
                                                            RTIinternalError);

    void publishInteraction(FederateHandle federate_handle,
                            InteractionClassHandle interaction_class_handle,
                            bool publish_or_unpublish) throw(InteractionClassNotDefined,
                                                             FederateNotExecutionMember,
                                                             SaveInProgress,
                                                             SecurityError,
                                                             RestoreInProgress,
                                                             RTIinternalError);

    void subscribeInteraction(FederateHandle federate_handle,
                              InteractionClassHandle interaction_class_handle,
                              bool subscribe_or_unsubscribe) throw(InteractionClassNotDefined,
                                                                   FederateNotExecutionMember,
                                                                   SaveInProgress,
                                                                   SecurityError,
                                                                   RestoreInProgress,
                                                                   RTIinternalError);

    // --------------------------
    // -- Ownership Management --
    // --------------------------

    bool isOwner(FederateHandle federate_handle,
                 ObjectHandle object_handle,
                 AttributeHandle attribute_handle) throw(FederateNotExecutionMember,
                                                         ObjectNotKnown,
                                                         AttributeNotDefined,
                                                         SaveInProgress,
                                                         RestoreInProgress,
                                                         RTIinternalError);

    void queryAttributeOwnership(FederateHandle federate_handle,
                                 ObjectHandle object_handle,
                                 AttributeHandle attribute_handle) throw(FederateNotExecutionMember,
                                                                         ObjectNotKnown,
                                                                         AttributeNotDefined,
                                                                         SaveInProgress,
                                                                         RestoreInProgress,
                                                                         RTIinternalError);

    void negotiateDivestiture(FederateHandle federate_handle,
                              ObjectHandle object_handle,
                              const std::vector<AttributeHandle>& attributes,
                              const std::string& tag) throw(FederateNotExecutionMember,
                                                            ObjectNotKnown,
                                                            AttributeNotDefined,
                                                            AttributeNotOwned,
                                                            AttributeAlreadyBeingDivested,
                                                            SaveInProgress,
                                                            RestoreInProgress,
                                                            RTIinternalError);

    void acquireIfAvailable(FederateHandle federate_handle,
                            ObjectHandle object_handle,
                            const std::vector<AttributeHandle>& attributes) throw(ObjectNotKnown,
                                                                                  ObjectClassNotPublished,
                                                                                  AttributeNotDefined,
                                                                                  AttributeNotPublished,
                                                                                  FederateOwnsAttributes,
                                                                                  AttributeAlreadyBeingAcquired,
                                                                                  FederateNotExecutionMember,
                                                                                  SaveInProgress,
                                                                                  RestoreInProgress,
                                                                                  RTIinternalError);

    void divest(FederateHandle federate_handle,
                ObjectHandle object_handle,
                const std::vector<AttributeHandle>& attributes) throw(ObjectNotKnown,
                                                                      AttributeNotDefined,
                                                                      AttributeNotOwned,
                                                                      FederateNotExecutionMember,
                                                                      SaveInProgress,
                                                                      RestoreInProgress,
                                                                      RTIinternalError);

    void acquire(FederateHandle federate_handle,
                 ObjectHandle object_handle,
                 const std::vector<AttributeHandle>& attributes,
                 const std::string& tag) throw(ObjectNotKnown,
                                               ObjectClassNotPublished,
                                               AttributeNotDefined,
                                               AttributeNotPublished,
                                               FederateOwnsAttributes,
                                               FederateNotExecutionMember,
                                               SaveInProgress,
                                               RestoreInProgress,
                                               RTIinternalError);

    void cancelDivestiture(FederateHandle federate_handle,
                           ObjectHandle object_handle,
                           const std::vector<AttributeHandle>& attributes) throw(ObjectNotKnown,
                                                                                 AttributeNotDefined,
                                                                                 AttributeNotOwned,
                                                                                 AttributeDivestitureWasNotRequested,
                                                                                 FederateNotExecutionMember,
                                                                                 SaveInProgress,
                                                                                 RestoreInProgress,
                                                                                 RTIinternalError);

    AttributeHandleSet*
    respondRelease(FederateHandle federate_handle,
                   ObjectHandle object_handle,
                   const std::vector<AttributeHandle>& attributes) throw(ObjectNotKnown,
                                                                         AttributeNotDefined,
                                                                         AttributeNotOwned,
                                                                         FederateWasNotAskedToReleaseAttribute,
                                                                         FederateNotExecutionMember,
                                                                         SaveInProgress,
                                                                         RestoreInProgress,
                                                                         RTIinternalError);

    void cancelAcquisition(FederateHandle federate_handle,
                           ObjectHandle object_handle,
                           const std::vector<AttributeHandle>& attributes) throw(ObjectNotKnown,
                                                                                 AttributeNotDefined,
                                                                                 AttributeAlreadyOwned,
                                                                                 AttributeAcquisitionWasNotRequested,
                                                                                 FederateNotExecutionMember,
                                                                                 SaveInProgress,
                                                                                 RestoreInProgress,
                                                                                 RTIinternalError);

    // Data Distribution Management

    long createRegion(FederateHandle federate_handle,
                      SpaceHandle space_handle,
                      long extents_count) throw(FederateNotExecutionMember,
                                                SpaceNotDefined,
                                                InvalidExtents,
                                                SaveInProgress,
                                                RestoreInProgress,
                                                RTIinternalError);

    void modifyRegion(FederateHandle federate_handle,
                      RegionHandle region_handle,
                      const std::vector<Extent>& extents) throw(FederateNotExecutionMember,
                                                                RegionNotKnown,
                                                                InvalidExtents,
                                                                SaveInProgress,
                                                                RestoreInProgress,
                                                                RTIinternalError);

    // FIXME second arguments should be region_handle ?
    void deleteRegion(FederateHandle federate_handle, long region) throw(
        FederateNotExecutionMember, RegionNotKnown, RegionInUse, SaveInProgress, RestoreInProgress, RTIinternalError);

    void associateRegion(FederateHandle federate_handle,
                         ObjectHandle object_handle,
                         RegionHandle region_handle,
                         const std::vector<AttributeHandle>& attributes) throw(FederateNotExecutionMember,
                                                                               RegionNotKnown,
                                                                               SaveInProgress,
                                                                               RestoreInProgress,
                                                                               RTIinternalError);

    void unassociateRegion(FederateHandle federate_handle, ObjectHandle, RegionHandle) throw(
        FederateNotExecutionMember, RegionNotKnown, SaveInProgress, RestoreInProgress, RTIinternalError);

    void subscribeAttributesWR(FederateHandle federate_handle,
                               ObjectClassHandle object_class_handle,
                               RegionHandle region_handle,
                               const std::vector<AttributeHandle>& attributes) throw(FederateNotExecutionMember,
                                                                                     RegionNotKnown,
                                                                                     SaveInProgress,
                                                                                     RestoreInProgress,
                                                                                     RTIinternalError);

    void unsubscribeAttributesWR(FederateHandle federate_handle,
                                 ObjectClassHandle object_class_handle,
                                 RegionHandle region_handle) throw(FederateNotExecutionMember,
                                                                   RegionNotKnown,
                                                                   SaveInProgress,
                                                                   RestoreInProgress,
                                                                   RTIinternalError);

    void subscribeInteractionWR(FederateHandle federate_handle,
                                InteractionClassHandle interaction_class_handle,
                                RegionHandle region_handle) throw(FederateNotExecutionMember,
                                                                  RegionNotKnown,
                                                                  SaveInProgress,
                                                                  RestoreInProgress,
                                                                  RTIinternalError);

    void unsubscribeInteractionWR(FederateHandle federate_handle,
                                  InteractionClassHandle interaction_class_handle,
                                  RegionHandle region_handle) throw(FederateNotExecutionMember,
                                                                    RegionNotKnown,
                                                                    SaveInProgress,
                                                                    RestoreInProgress,
                                                                    RTIinternalError);

    ObjectHandle
    registerObjectWithRegion(FederateHandle federate_handle,
                             ObjectClassHandle object_class_handle,
                             const std::string& name,
                             RegionHandle region_handle,
                             const std::vector<AttributeHandle>& attributes) throw(FederateNotExecutionMember,
                                                                                   ObjectClassNotDefined,
                                                                                   ObjectClassNotPublished,
                                                                                   AttributeNotDefined,
                                                                                   AttributeNotPublished,
                                                                                   RegionNotKnown,
                                                                                   InvalidRegionContext,
                                                                                   ObjectAlreadyRegistered,
                                                                                   SaveInProgress,
                                                                                   RestoreInProgress,
                                                                                   RTIinternalError);

    void getFOM(NM_Join_Federation_Execution& object_model_data);

    /**
     * Update the last NERx message date for the concerned federate.
     * @param[in] federate_handle the handle of the federate for which we want to update NERx time.
     * @param[in] date the new NERx date for the specified federate.
     */
    bool updateLastNERxForFederate(FederateHandle federate_handle,
                                   FederationTime date) throw(FederateNotExecutionMember);

    /**
     * Compute the minimum of all NERx messsage date
     * for all Federates using NERx messages.
     * @return the minimum if at least one federate is using NERx, 0 otherwise
     */
    FederationTime computeMinNERx() throw(FederateNotExecutionMember);

    /**
     * Get the (precomputed) minimum of all NERx messsage date
     * for all Federates using NERx messages.
     * @return the minimum if at least one federate is using NERx, 0 otherwise
     */
    FederationTime getMinNERx() const;

    /// Return the Federate whose Name is name, if found.
    Federate& getFederate(const std::string& name) throw(FederateNotExecutionMember);

    /// Return the Federate whose Handle is theHandle, if found.
    Federate& getFederate(FederateHandle federate_handle) throw(FederateNotExecutionMember);

private:
    bool saveXmlData();
    bool restoreXmlData(std::string docFilename);

    /// Broadcast 'msg' to all Federate except the specified one (unless this is an anonymous update)
    void broadcastAnyMessage(NetworkMessage* msg, FederateHandle except_federate, bool anonymous);

    /// Broadcast 'msg' to some Federates except the specified one
    void broadcastSomeMessage(NetworkMessage* msg,
                              FederateHandle except_federate,
                              const std::vector<FederateHandle>& fede_array,
                              uint32_t nbfed);

    FederationHandle my_handle;
    std::string my_name;
    std::string my_FED_id;

    /// Labels and Tags not on synchronization.
    std::map<std::string, std::string> my_synchronization_labels;

    HandleManager<FederateHandle> my_federate_handle_generator;
    HandleManager<ObjectHandle> my_objects_handle_generator;

    /** This object is initialized when the Federation is created
     * 
     *  with the reference of the RTIG managed Socket Server. The reference of
     *  this object is passed down the Classes Tree with the help of RootObj.
     */
    std::unique_ptr<SecurityServer> my_server;
    std::unique_ptr<RootObject> my_root_object;

    LBTS my_regulators;

#ifdef FEDERATION_USES_MULTICAST
    SocketMC* MCLink;
#endif

    // Private attributes
    std::unordered_map<FederateHandle, std::unique_ptr<Federate>> my_federates;

    bool my_is_save_in_progress;
    bool my_is_restore_in_progress;
    bool my_save_status; /// True if saving was correctly done, false otherwise.
    bool my_restore_status; /// True if restoring was correctly done.
    std::string my_save_label; /// The label associated with the save request.

    /// The minimum NERx timestamp for this federation
    FederationTime my_min_NERx;

    /// The message buffer used to send Network messages
    MessageBuffer my_nm_buffer;
};
}
} // namespace certi/rtig

#endif // _CERTI_RTIG_FEDERATION_HH
