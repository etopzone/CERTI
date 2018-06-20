// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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
#include <set>
#include <string>
#include <vector>

#include <memory>
#include <unordered_map>

#include <include/certi.hh>

#include <libCERTI/BaseRegion.hh>
#include <libCERTI/BasicMessage.hh>
#include <libCERTI/Exception.hh>
#include <libCERTI/FedTimeD.hh>
#include <libCERTI/HandleManager.hh>
#include <libCERTI/LBTS.hh>
#include <libCERTI/MessageEvent.hh>
#include <libHLA/MessageBuffer.hh>

#include "Federate.hh"
#include "Mom.hh"

#ifdef FEDERATION_USES_MULTICAST
#include "SocketMC.hh"
#endif

namespace certi {
class AttributeHandleSet;
class AuditFile;
class Extent;
class NM_Join_Federation_Execution;
class NM_Resign_Federation_Execution;
class NetworkMessage;
class RootObject;
class SecurityServer;
class SocketServer;
class SocketTCP;

namespace rtig {
class Mom;

class Federation {
public:
#ifdef FEDERATION_USES_MULTICAST
    /**
	 * Allocates memory the Name's storage, and read its FED file to store the
	 * result in RootObj.
	 *   (with FEDERATION_USES_MULTICAST defined).
	 */
    Federation(const std::string& federation_name,
               const FederationHandle federation_handle,
               SocketServer& socket_server,
               AuditFile& audit_server,
               const std::vector<std::string> fom_modules,
               const std::string& mim_module,
               const RtiVersion rti_version,
               SocketMC* mc_link,
               const int verboseLevel);
#else
    /**
     * Allocates memory the Name's storage, and read its FED file to store the
     * result in RootObj.
     * with FEDERATION_USES_MULTICAST not defined
     * @param federation_name Name of the federation
     * @param federation_handle Handle of the federation
     * @param socket_server Socket server from RTIG
     * @param audit_server Audit server from RTIG
     * @param fom_modules list of paths to fom modules (may be a .fed or a .xml file)
     * @param mim_module path to mim module, or empty for default mim module
     */
    Federation(const std::string& federation_name,
               const FederationHandle federation_handle,
               SocketServer& socket_server,
               AuditFile& audit_server,
               const std::vector<std::string> fom_modules,
               const std::string& mim_module,
               const RtiVersion rti_version,
               const int verboseLevel);
#endif

    ~Federation();

    /// Returns the federation handle.
    FederationHandle getHandle() const;

    /// Returns the federation name given in 'Create Federation Execution'.
    std::string getName() const;

    /// Returns the current list of fom module used in this federation
    std::vector<std::string> getFomModules() const;

    /// Return the current mim module used
    std::string getMimModule() const;

    RtiVersion getRtiVersion() const;

    /// Returns the number of federates in federation.
    int getNbFederates() const;

    /// Returns the number of federates regulator.
    int getNbRegulators() const;

    /// Return true if federation is being synchronized.
    bool isSynchronizing() const;

    bool isMomEnabled() const;

    bool isAutoProvideActive() const;

    Responses enableMomIfAvailable();

    // -------------------------
    // -- Federate Management --
    // -------------------------

    /** Return true if there are no Federates left in the Federation
     * 
     * else throw FederatesCurrentlyJoined.
     */
    bool empty() const;

    /** Check whether if the federate is part of the federation.
     * 
     * @param federate_handle federate to check
     * @return Throw an exception if the federate is not part of the federation.
     *          Else return true if the federate is a "real" federate or false if it is the rti mom federate handle
     */
    bool check(FederateHandle federate_handle) const;

    /** Add the Federate to the Federation, and return its new federate handle.
     * 
     * Also send Null messages from all others federates to initialize its LBTS, and
     * finally a RequestPause message if the Federation is already paused.
     */
    std::pair<FederateHandle, Responses> add(const std::string& federate_name,
                                             const std::string& federate_type,
                                             std::vector<std::string> additional_fom_modules,
                                             const RtiVersion rti_version,
                                             SocketTCP* tcp_link,
                                             const uint32_t peer,
                                             const uint32_t address);

    /** Remove a federate.
     * 
     * NOTE: Is it normal it does not check if federate is used, e.g is regulator ?
     * 
     * @param federate_handle Handle of the federate to remove.
     * @bug Currently does not check if Federate owns attributes. The
     * Federate Object is deleted.
     */
    Responses remove(FederateHandle federate_handle);

    /** Make a federate resign the federation.
     *
     * This function tries to remove all references to this federate in the federation.
     * To be used when a Federate is supposed to have crashed.
     * 
     * NOTE: Connection to the federate is already closed.
     *
     * @param federate_handle Handle of the federate to kill
     */
    Responses kill(FederateHandle federate_handle) noexcept;

    // ---------------------
    // -- Time Management --
    // ---------------------

    /** Add the Federate to the Regulators List.
     * 
     * Check if it's already present, but not if the Time 'time' is allowed or not.
     */
    Responses addRegulator(FederateHandle federate_handle,
                           FederationTime time); // includes Time Regulation already enabled.

    /// Update the current time of a regulator federate.
    Responses updateRegulator(FederateHandle federate_handle,
                              FederationTime time,
                              bool anonymous);

    /// includes Time Regulation already disabled.
    Responses removeRegulator(FederateHandle federate_handle);

    Responses setConstrained(FederateHandle federate_handle, bool constrained, FederationTime time);

    /// Update the current time of a regulator federate.
    Responses updateTimeState(FederateHandle federate_handle,
                              FederationTime time,
                              FederationTime lookahead,
                              bool time_manager_state,
                              FederationTime galt,
                              FederationTime lits);

    // Synchronization Management.

    /// Add a new synchronization point to federation.
    void registerSynchronization(FederateHandle federate_handle, const std::string& label, const std::string& tag);

    /// Add a new synchronization point (with federates set) to federation.
    void registerSynchronization(FederateHandle federate_handle,
                                 const std::string& label,
                                 const std::string& tag,
                                 const std::vector<FederateHandle>& federates);

    Responses unregisterSynchronization(FederateHandle federate_handle, const std::string& label);

    /// Broadcast an 'Announce Synchronization Point' when registering a new synchronization point.
    Responses
    broadcastSynchronization(FederateHandle federate_handle, const std::string& label, const std::string& tag);

    /// Broadcast an 'Announce Synchronization Point' when registering a new synchronization point onto a set of federates
    Responses broadcastSynchronization(FederateHandle federate_handle,
                                       const std::string& label,
                                       const std::string& tag,
                                       const std::vector<FederateHandle>& federates);

    // Save Management.

    /** Request a federation save with time.
     * 
     * This service puts each federate from federation in saving state.
     * time is not managed yet.
     */
    Responses requestFederationSave(FederateHandle federate_handle, const std::string& label, FederationTime time);

    /** Request a federation save without time.
     * 
     * This service puts each federate from federation in saving state.
     * time is not managed yet.
     */
    Responses requestFederationSave(FederateHandle federate_handle, const std::string& label);

    /** Received from a federate to inform a save has been received and is being processed.
     * 
     * Maybe, a timeout should be set to determine if federate still alive.
     */
    void federateSaveBegun(FederateHandle federate_handle);

    /// Informs that a federate returns a save end message (with success or not!).
    Responses federateSaveStatus(FederateHandle federate_handle, bool status);

    /// Informs that a federate is requesting a save.
    Responses requestFederationRestore(FederateHandle federate_handle, const std::string& label);

    /** Informs that a federate has ended a restore.
     * 
     * If each federate in federation has ended restoring, this service send a federation restore status.
     */
    Responses federateRestoreStatus(FederateHandle federate_handle, bool status);

    // -----------------------
    // -- Object Management --
    // -----------------------

    Responses publishObject(FederateHandle federate_handle,
                            ObjectClassHandle object_handle,
                            const std::vector<AttributeHandle>& attributes,
                            bool publish_or_unpublish);

    /**
     * Subscribes attributes to an object class. After subscription, attribute 
     * publishers are notified by the federate service 
     * startRegistrationForObjectClass.
     * @param[in] federate_handle federate handle of the subscriber
     * @param[in] object_handle subscripted object class handle 
     * @param[in] attributes subscripted vector of attributes 
     */
    Responses subscribeObject(FederateHandle federate_handle,
                              ObjectClassHandle object_handle,
                              const std::vector<AttributeHandle>& attributes,
                              const bool subscribe_or_unsubscribe);

    void reserveObjectInstanceName(FederateHandle federate_handle, std::string object_name);

    std::pair<ObjectHandle, Responses>
    registerObject(FederateHandle federate_handle, ObjectClassHandle class_handle, const std::string& name);

    /** Removes an object instance from federation.
     * 
     *  @param federate_handle Federate requesting removal
     *  @param object_handle Object handle
     *  @param time Federation Time
     *  @param user_tag Label for this operation
     */
    Responses deleteObject(FederateHandle federate_handle,
                           ObjectHandle object_handle,
                           FederationTime time,
                           const std::string& user_tag);

    /** Removes an object instance from federation.
     * 
     *  @param federate_handle Federate requesting removal
     *  @param object_handle Object handle
     *  @param user_tag Label for this operation
     */
    Responses deleteObject(FederateHandle federate_handle, ObjectHandle object_handle, const std::string& user_tag);

    std::pair<FederateHandle, Responses> requestObjectOwner(FederateHandle federate_handle,
                                                            ObjectHandle object_handle,
                                                            const std::vector<AttributeHandle>& attributes);

    Responses requestClassAttributeValueUpdate(FederateHandle federate_handle,
                                               ObjectClassHandle class_handle,
                                               const std::vector<AttributeHandle>& attributes);

    Responses updateAttributeValues(FederateHandle federate_handle,
                                    ObjectHandle object_handle,
                                    const std::vector<AttributeHandle>& attributes,
                                    const std::vector<AttributeValue_t>& attribute_values,
                                    FederationTime time,
                                    const std::string& tag);

    Responses updateAttributeValues(FederateHandle federate_handle,
                                    ObjectHandle object_handle,
                                    const std::vector<AttributeHandle>& attributes,
                                    const std::vector<AttributeValue_t>& attribute_values,
                                    const std::string& tag);

    // ----------------------------
    // -- Interaction Management --
    // ----------------------------

    Responses publishInteraction(FederateHandle federate_handle,
                                 InteractionClassHandle interaction_class_handle,
                                 bool publish_or_unpublish);

    Responses subscribeInteraction(FederateHandle federate_handle,
                                   InteractionClassHandle interaction_class_handle,
                                   bool subscribe_or_unsubscribe);

    /// broadcastInteraction with time
    Responses broadcastInteraction(FederateHandle federate_handle,
                                   InteractionClassHandle interaction_class_handle,
                                   const std::vector<ParameterHandle>& theParameterList,
                                   const std::vector<ParameterValue_t>& attribute_values,
                                   FederationTime time,
                                   RegionHandle region,
                                   const std::string& tag);

    /// broadcastInteraction without time
    Responses broadcastInteraction(FederateHandle federate_handle,
                                   InteractionClassHandle interaction_class_handle,
                                   const std::vector<ParameterHandle>& parameters,
                                   const std::vector<ParameterValue_t>& parameters_values,
                                   RegionHandle region,
                                   const std::string& tag);

    // --------------------------
    // -- Ownership Management --
    // --------------------------

    bool isOwner(FederateHandle federate_handle, ObjectHandle object_handle, AttributeHandle attribute_handle);

    void queryAttributeOwnership(FederateHandle federate_handle,
                                 ObjectHandle object_handle,
                                 AttributeHandle attribute_handle);

    Responses negotiateDivestiture(FederateHandle federate_handle,
                                   ObjectHandle object_handle,
                                   const std::vector<AttributeHandle>& attributes,
                                   const std::string& tag);

    void acquireIfAvailable(FederateHandle federate_handle,
                            ObjectHandle object_handle,
                            const std::vector<AttributeHandle>& attributes);

    Responses
    divest(FederateHandle federate_handle, ObjectHandle object_handle, const std::vector<AttributeHandle>& attributes);

    void acquire(FederateHandle federate_handle,
                 ObjectHandle object_handle,
                 const std::vector<AttributeHandle>& attributes,
                 const std::string& tag);

    void cancelDivestiture(FederateHandle federate_handle,
                           ObjectHandle object_handle,
                           const std::vector<AttributeHandle>& attributes);

    AttributeHandleSet* respondRelease(FederateHandle federate_handle,
                                       ObjectHandle object_handle,
                                       const std::vector<AttributeHandle>& attributes);

    void cancelAcquisition(FederateHandle federate_handle,
                           ObjectHandle object_handle,
                           const std::vector<AttributeHandle>& attributes);

    // Data Distribution Management

    long createRegion(FederateHandle federate_handle, SpaceHandle space_handle, long extents_count);

    void modifyRegion(FederateHandle federate_handle, RegionHandle region_handle, const std::vector<Extent>& extents);

    // FIXME second arguments should be region_handle ?
    void deleteRegion(FederateHandle federate_handle, long region);

    void associateRegion(FederateHandle federate_handle,
                         ObjectHandle object_handle,
                         RegionHandle region_handle,
                         const std::vector<AttributeHandle>& attributes);

    void unassociateRegion(FederateHandle federate_handle, ObjectHandle, RegionHandle);

    void subscribeAttributesWR(FederateHandle federate_handle,
                               ObjectClassHandle object_class_handle,
                               RegionHandle region_handle,
                               const std::vector<AttributeHandle>& attributes);

    void unsubscribeAttributesWR(FederateHandle federate_handle,
                                 ObjectClassHandle object_class_handle,
                                 RegionHandle region_handle);

    void subscribeInteractionWR(FederateHandle federate_handle,
                                InteractionClassHandle interaction_class_handle,
                                RegionHandle region_handle);

    void unsubscribeInteractionWR(FederateHandle federate_handle,
                                  InteractionClassHandle interaction_class_handle,
                                  RegionHandle region_handle);

    std::pair<ObjectHandle, Responses> registerObjectWithRegion(FederateHandle federate_handle,
                                                                ObjectClassHandle object_class_handle,
                                                                const std::string& name,
                                                                RegionHandle region_handle,
                                                                const std::vector<AttributeHandle>& attributes);

    Responses updateAsynchronousDelivery(FederateHandle federate_handle, bool status);

    void getFOM(NM_Join_Federation_Execution& object_model_data);
    void getFOM(NM_Additional_Fom_Module& object_model_data);

    /**
     * Update the last NERx message date for the concerned federate.
     * @param[in] federate_handle the handle of the federate for which we want to update NERx time.
     * @param[in] date the new NERx date for the specified federate.
     */
    bool updateLastNERxForFederate(FederateHandle federate_handle, FederationTime date);

    /**
     * Compute the minimum of all NERx messsage date
     * for all Federates using NERx messages.
     * @return the minimum if at least one federate is using NERx, 0 otherwise
     */
    FederationTime computeMinNERx();

    /**
     * Get the (precomputed) minimum of all NERx messsage date
     * for all Federates using NERx messages.
     * @return the minimum if at least one federate is using NERx, 0 otherwise
     */
    FederationTime getMinNERx() const;

    /// Return the Federate whose Name is name, if found.
    Federate& getFederate(const std::string& name);

    /// Return the Federate whose Handle is theHandle, if found.
    Federate& getFederate(FederateHandle federate_handle);

    RootObject& getRootObject();
    
private:
    friend class Mom;

    void openFomModules(std::vector<std::string> modules, const bool is_mim = false);

    bool saveXmlData();
    bool restoreXmlData(std::string docFilename);

    Responses respondToAll(std::unique_ptr<NetworkMessage> message, const FederateHandle except = 0);
    Responses respondToSome(std::unique_ptr<NetworkMessage> message, const std::vector<FederateHandle>& recipients);

    Responses setAutoProvide(const bool value);

    FederationHandle my_handle;
    std::string my_name;

    std::vector<std::string> my_fom_modules;
    std::string my_mim_module;

    bool my_auto_provide{false};

#ifdef FEDERATION_USES_MULTICAST
    SocketMC* MCLink;
#endif

    /** This object is initialized when the Federation is created
     * 
     *  with the reference of the RTIG managed Socket Server. The reference of
     *  this object is passed down the Classes Tree with the help of RootObj.
     */
    std::unique_ptr<SecurityServer> my_server;
    std::unique_ptr<RootObject> my_root_object;

    /// The minimum NERx timestamp for this federation
    FederationTime my_min_NERx{};

    std::unique_ptr<Mom> my_mom;

    std::unordered_map<FederateHandle, std::unique_ptr<Federate>> my_federates{};

    LBTS my_regulators{};

    /// Labels and Tags not on synchronization.
    std::map<std::string, std::string> my_synchronization_labels{};

    /// The message buffer used to send Network messages
    MessageBuffer my_nm_buffer{};

    HandleManager<FederateHandle> my_federate_handle_generator{1};
    HandleManager<ObjectHandle> my_objects_handle_generator{1};

    bool my_is_save_in_progress{false};
    bool my_is_restore_in_progress{false};
    bool my_save_status{true}; /// True if saving was correctly done, false otherwise.
    bool my_restore_status{true}; /// True if restoring was correctly done.
    std::string my_save_label{""}; /// The label associated with the save request.

    RtiVersion my_rti_version;
};
}
} // namespace certi/rtig

#endif // _CERTI_RTIG_FEDERATION_HH
