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
               FederationHandle federation_handle,
               SocketServer& socket_server,
               AuditFile& audit_server,
               SocketMC* mc_link,
               int theVerboseLevel)
#else
    /**
     * Allocates memory the Name's storage, and read its FED file to store the
     * result in RootObj.
     * with FEDERATION_USES_MULTICAST not defined
     * @param federation_name
     * @param federation_handle
     * @param socket_server
     * @param audit_server
     * @param FEDid_name i.e. FED file name (may be a .fed or a .xml file)
     */
    Federation(const std::string& federation_name,
               Handle federation_handle,
               SocketServer& socket_server,
               AuditFile& audit_server,
               const std::string& FEDid_name,
               int theVerboseLevel)
#endif
        throw(CouldNotOpenFED, ErrorReadingFED, MemoryExhausted, SecurityError, RTIinternalError);

    ~Federation();

    /// Returns the federation handle.
    Handle getHandle() const;

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
    FederateHandle add(const std::string& theName,
                       SocketTCP* theTCPLink) throw(FederateAlreadyExecutionMember, MemoryExhausted, RTIinternalError);

    /** Return true if there are no Federates left in the Federation
     * 
     * else throw FederatesCurrentlyJoined.
     * 
     * FIXME This is very strange
     */
    bool empty() const throw(FederatesCurrentlyJoined);

    /** Check whether if the federate is part of the federation.
     * 
     * @param[in] theHandle
     * @return true if the federate is part of the Federation,
     *              else throw an exception.
     * @throws FederateNotExecutionMember
     */
    bool check(FederateHandle theHandle) const throw(FederateNotExecutionMember);

    /** Make a federate resign the federation.
     *
     * This function tries to remove all references to this federate in the federation.
     * To be used when a Federate is supposed to have crashed.
     * @param federate Handle of the federate to kill
     */
    void kill(FederateHandle theFederate) throw();

    /** Remove a federate.
     * 
     * @param federate_handle Handle of the federate to remove.
     * @bug Currently does not check if Federate owns attributes. The
     * Federate Object is deleted.
     */
    void remove(FederateHandle theHandle) throw(FederateOwnsAttributes, FederateNotExecutionMember);

    // ---------------------
    // -- Time Management --
    // ---------------------

    /** Add the Federate to the Regulators List.
     * 
     * Check if it's already present, but not if the Time 'theTime' is allowed or not.
     */
    void addRegulator(FederateHandle theHandle,
                      FederationTime theTime) throw(FederateNotExecutionMember,
                                                    SaveInProgress,
                                                    RestoreInProgress,
                                                    RTIinternalError); // includes Time Regulation already enabled.

    /// Update the current time of a regulator federate.
    void updateRegulator(FederateHandle theHandle,
                         FederationTime theTime,
                         bool anonymous) throw(FederateNotExecutionMember, RTIinternalError);

    void
    removeRegulator(FederateHandle theHandle) throw(FederateNotExecutionMember,
                                                    SaveInProgress,
                                                    RestoreInProgress,
                                                    RTIinternalError); // includes Time Regulation already disabled.

    /** Set Federate's attribute IsConstrained to true.
     * 
     *  FIXME: name/ merge with removeConstrained.
     */
    void addConstrained(FederateHandle theHandle) throw(FederateNotExecutionMember,
                                                        SaveInProgress,
                                                        RestoreInProgress,
                                                        RTIinternalError); // includes Time constrained already enabled.

    void
    removeConstrained(FederateHandle theHandle) throw(FederateNotExecutionMember,
                                                      SaveInProgress,
                                                      RestoreInProgress,
                                                      RTIinternalError); // includes Time constrained already disabled.

    // Synchronization Management.

    /// Add a new synchronization point to federation.
    void registerSynchronization(FederateHandle the_federate,
                                 const std::string& the_label,
                                 const std::string& the_tag) throw(FederateNotExecutionMember,
                                                                   FederationAlreadyPaused,
                                                                   SaveInProgress,
                                                                   RestoreInProgress,
                                                                   RTIinternalError);

    /// Add a new synchronization point (with federates set) to federation.
    void registerSynchronization(FederateHandle the_federate,
                                 const std::string& the_label,
                                 const std::string& the_tag,
                                 unsigned short federate_setSize,
                                 const std::vector<FederateHandle>& federate_set) throw(FederateNotExecutionMember,
                                                                                        FederationAlreadyPaused,
                                                                                        SaveInProgress,
                                                                                        RestoreInProgress,
                                                                                        RTIinternalError);

    void unregisterSynchronization(FederateHandle theFederate, const std::string& theLabel) throw(
        FederateNotExecutionMember, FederationNotPaused, SaveInProgress, RestoreInProgress, RTIinternalError);

    /// Broadcast an 'Announce Synchronization Point' when registering a new synchronization point.
    void broadcastSynchronization(FederateHandle federate,
                                  const std::string& label,
                                  const std::string& tag) throw(RTIinternalError);

    /// Broadcast an 'Announce Synchronization Point' when registering a new synchronization point onto a set of federates
    void broadcastSynchronization(FederateHandle federate,
                                  const std::string& label,
                                  const std::string& tag,
                                  unsigned short federate_setSize,
                                  const std::vector<FederateHandle>& federate_set) throw(RTIinternalError);

    // Save Management.

    /** Request a federation save with time.
     * 
     * This service puts each federate from federation in saving state.
     * the_time is not managed yet.
     */
    void requestFederationSave(FederateHandle, const std::string&, FederationTime) throw(FederateNotExecutionMember,
                                                                                         SaveInProgress);

    /** Request a federation save without time.
     * 
     * This service puts each federate from federation in saving state.
     * the_time is not managed yet.
     */
    void requestFederationSave(FederateHandle, const std::string&) throw(FederateNotExecutionMember, SaveInProgress);

    /** Received from a federate to inform a save has been received and is being processed.
     * 
     * Maybe, a timeout should be set to determine if federate still alive.
     */
    void federateSaveBegun(FederateHandle) throw(FederateNotExecutionMember);

    /// Informs that a federate returns a save end message (with success or not!).
    void federateSaveStatus(FederateHandle, bool) throw(FederateNotExecutionMember);

    /// Informs that a federate is requesting a save.
    void requestFederationRestore(FederateHandle the_federate,
                                  const std::string& the_label) throw(FederateNotExecutionMember);

    /** Informs that a federate has ended a restore.
     * 
     * If each federate in federation has ended restoring, this service send a federation restore status.
     */
    void federateRestoreStatus(FederateHandle the_federate, bool the_status) throw(FederateNotExecutionMember);

    // -----------------------
    // -- Object Management --
    // -----------------------

    /** Removes an object instance from federation.
     * 
     *  @param federate Federate requesting removal
     *  @param id Object handle
     *  @param theTime Federation Time
     *  @param tag Label for this operation
     */
    void deleteObject(FederateHandle theFederateHandle,
                      ObjectHandle theObjectHandle,
                      FederationTime theTime,
                      const std::string& theUserTag) throw(FederateNotExecutionMember,
                                                           DeletePrivilegeNotHeld,
                                                           ObjectNotKnown,
                                                           SaveInProgress,
                                                           RestoreInProgress,
                                                           InvalidFederationTime,
                                                           RTIinternalError);

    /** Removes an object instance from federation.
     * 
     *  @param federate Federate requesting removal
     *  @param id Object handle
     *  @param tag Label for this operation
     */
    void deleteObject(FederateHandle theFederateHandle,
                      ObjectHandle theObjectHandle,
                      const std::string& theUserTag) throw(FederateNotExecutionMember,
                                                           DeletePrivilegeNotHeld,
                                                           ObjectNotKnown,
                                                           SaveInProgress,
                                                           RestoreInProgress,
                                                           RTIinternalError);

    void publishObject(FederateHandle theFederateHandle,
                       ObjectClassHandle theObjectHandle,
                       const std::vector<AttributeHandle>& theAttributeList,
                       bool PubOrUnpub) throw(ObjectClassNotDefined,
                                              AttributeNotDefined,
                                              FederateNotExecutionMember,
                                              SaveInProgress,
                                              SecurityError,
                                              RestoreInProgress,
                                              RTIinternalError);

    void reserveObjectInstanceName(FederateHandle theFederateHandle, std::string newObjName) throw(IllegalName,
                                                                                                   SaveInProgress,
                                                                                                   RestoreInProgress,
                                                                                                   RTIinternalError);

    ObjectHandle registerObject(FederateHandle theFederateHandle,
                                ObjectClassHandle theClass,
                                const std::string& theName) throw(FederateNotExecutionMember,
                                                                  FederateNotPublishing,
                                                                  ObjectAlreadyRegistered,
                                                                  ObjectClassNotDefined,
                                                                  ObjectClassNotPublished,
                                                                  SaveInProgress,
                                                                  RestoreInProgress,
                                                                  RTIinternalError);

    FederateHandle requestObjectOwner(FederateHandle theFederateHandle,
                                      ObjectHandle theObject,
                                      const std::vector<AttributeHandle>& theAttributeList,
                                      uint32_t theListSize) throw(ObjectNotKnown);

    void requestClassAttributeValueUpdate(FederateHandle theFederateHandle,
                                          ObjectClassHandle theClassHandle,
                                          const std::vector<AttributeHandle>& theAttributeList,
                                          uint32_t theListSize) throw(ObjectClassNotDefined, RTIinternalError);

    /**
     * Subscribes attributes to an object class. After subscription, attribute 
     * publishers are notified by the federate service 
     * startRegistrationForObjectClass.
     * @param[in] federate federate handle of the subscriber
     * @param[in] object subscripted object class handle 
     * @param[in] attributes subscripted vector of attributes 
     */
    void subscribeObject(FederateHandle theFederateHandle,
                         ObjectClassHandle theObjectHandle,
                         const std::vector<AttributeHandle>& theAttributeList) throw(ObjectClassNotDefined,
                                                                                     AttributeNotDefined,
                                                                                     FederateNotExecutionMember,
                                                                                     SaveInProgress,
                                                                                     SecurityError,
                                                                                     RestoreInProgress,
                                                                                     RTIinternalError);

    void updateAttributeValues(FederateHandle theFederateHandle,
                               ObjectHandle theObjectHandle,
                               const std::vector<AttributeHandle>& theAttributeList,
                               const std::vector<AttributeValue_t>& theValueList,
                               uint16_t theListSize,
                               FederationTime theTime,
                               const std::string& theTag) throw(FederateNotExecutionMember,
                                                                ObjectNotKnown,
                                                                AttributeNotDefined,
                                                                AttributeNotOwned,
                                                                SaveInProgress,
                                                                RestoreInProgress,
                                                                RTIinternalError);

    void updateAttributeValues(FederateHandle theFederateHandle,
                               ObjectHandle theObjectHandle,
                               const std::vector<AttributeHandle>& theAttributeList,
                               const std::vector<AttributeValue_t>& theValueList,
                               uint16_t theListSize,
                               const std::string& theTag) throw(FederateNotExecutionMember,
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
    void broadcastInteraction(FederateHandle theFederateHandle,
                              InteractionClassHandle theInteractionHandle,
                              const std::vector<ParameterHandle>& theParameterList,
                              const std::vector<ParameterValue_t>& theValueList,
                              uint16_t theListSize,
                              FederationTime theTime,
                              RegionHandle region,
                              const std::string& theTag) throw(FederateNotExecutionMember,
                                                               FederateNotPublishing,
                                                               InteractionClassNotDefined,
                                                               InteractionParameterNotDefined,
                                                               SaveInProgress,
                                                               RestoreInProgress,
                                                               RTIinternalError);

    /// broadcastInteraction without time
    void broadcastInteraction(FederateHandle theFederateHandle,
                              InteractionClassHandle theInteractionHandle,
                              const std::vector<ParameterHandle>& theParameterList,
                              const std::vector<ParameterValue_t>& theValueList,
                              uint16_t theListSize,
                              RegionHandle region,
                              const std::string& theTag) throw(FederateNotExecutionMember,
                                                               FederateNotPublishing,
                                                               InteractionClassNotDefined,
                                                               InteractionParameterNotDefined,
                                                               SaveInProgress,
                                                               RestoreInProgress,
                                                               RTIinternalError);

    void publishInteraction(FederateHandle theFederateHandle,
                            InteractionClassHandle theInteractionHandle,
                            bool PubOrUnpub) throw(InteractionClassNotDefined,
                                                   FederateNotExecutionMember,
                                                   SaveInProgress,
                                                   SecurityError,
                                                   RestoreInProgress,
                                                   RTIinternalError);

    void subscribeInteraction(FederateHandle theFederateHandle,
                              InteractionClassHandle theInteractionHandle,
                              bool SubOrUnsub) throw(InteractionClassNotDefined,
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
                 AttributeHandle theAttribute) throw(FederateNotExecutionMember,
                                                     ObjectNotKnown,
                                                     AttributeNotDefined,
                                                     SaveInProgress,
                                                     RestoreInProgress,
                                                     RTIinternalError);

    void queryAttributeOwnership(FederateHandle theFederateHandle,
                                 ObjectHandle theObjectHandle,
                                 AttributeHandle theAttribute) throw(FederateNotExecutionMember,
                                                                     ObjectNotKnown,
                                                                     AttributeNotDefined,
                                                                     SaveInProgress,
                                                                     RestoreInProgress,
                                                                     RTIinternalError);

    void negotiateDivestiture(FederateHandle theFederateHandle,
                              ObjectHandle theObjectHandle,
                              const std::vector<AttributeHandle>& theAttributeList,
                              uint16_t theListSize,
                              const std::string& theTag) throw(FederateNotExecutionMember,
                                                               ObjectNotKnown,
                                                               AttributeNotDefined,
                                                               AttributeNotOwned,
                                                               AttributeAlreadyBeingDivested,
                                                               SaveInProgress,
                                                               RestoreInProgress,
                                                               RTIinternalError);

    void acquireIfAvailable(FederateHandle theFederateHandle,
                            ObjectHandle theObjectHandle,
                            const std::vector<AttributeHandle>& theAttributeList,
                            uint16_t theListSize) throw(ObjectNotKnown,
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
                const std::vector<AttributeHandle>& theAttributeList,
                uint16_t theListSize) throw(ObjectNotKnown,
                                            AttributeNotDefined,
                                            AttributeNotOwned,
                                            FederateNotExecutionMember,
                                            SaveInProgress,
                                            RestoreInProgress,
                                            RTIinternalError);

    void acquire(FederateHandle theFederateHandle,
                 ObjectHandle theObjectHandle,
                 const std::vector<AttributeHandle>& theAttributeList,
                 uint16_t theListSize,
                 const std::string& theTag) throw(ObjectNotKnown,
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
                           const std::vector<AttributeHandle>& theAttributeList,
                           uint16_t theListSize) throw(ObjectNotKnown,
                                                       AttributeNotDefined,
                                                       AttributeNotOwned,
                                                       AttributeDivestitureWasNotRequested,
                                                       FederateNotExecutionMember,
                                                       SaveInProgress,
                                                       RestoreInProgress,
                                                       RTIinternalError);

    AttributeHandleSet* respondRelease(FederateHandle theFederateHandle,
                                       ObjectHandle theObjectHandle,
                                       const std::vector<AttributeHandle>& theAttributeList,
                                       uint16_t theListSize) throw(ObjectNotKnown,
                                                                   AttributeNotDefined,
                                                                   AttributeNotOwned,
                                                                   FederateWasNotAskedToReleaseAttribute,
                                                                   FederateNotExecutionMember,
                                                                   SaveInProgress,
                                                                   RestoreInProgress,
                                                                   RTIinternalError);

    void cancelAcquisition(FederateHandle theFederateHandle,
                           ObjectHandle theObjectHandle,
                           const std::vector<AttributeHandle>& theAttributeList,
                           uint16_t theListSize) throw(ObjectNotKnown,
                                                       AttributeNotDefined,
                                                       AttributeAlreadyOwned,
                                                       AttributeAcquisitionWasNotRequested,
                                                       FederateNotExecutionMember,
                                                       SaveInProgress,
                                                       RestoreInProgress,
                                                       RTIinternalError);

    // Data Distribution Management

    long createRegion(FederateHandle,
                      SpaceHandle,
                      long) throw(SpaceNotDefined, InvalidExtents, SaveInProgress, RestoreInProgress, RTIinternalError);

    void modifyRegion(FederateHandle, RegionHandle, const std::vector<Extent>&) throw(
        RegionNotKnown, InvalidExtents, SaveInProgress, RestoreInProgress, RTIinternalError);

    void deleteRegion(FederateHandle,
                      long) throw(RegionNotKnown, RegionInUse, SaveInProgress, RestoreInProgress, RTIinternalError);

    void associateRegion(FederateHandle,
                         ObjectHandle,
                         RegionHandle,
                         unsigned short,
                         const std::vector<AttributeHandle>&) throw(RegionNotKnown,
                                                                    SaveInProgress,
                                                                    RestoreInProgress,
                                                                    RTIinternalError);

    void unassociateRegion(FederateHandle,
                           ObjectHandle,
                           RegionHandle) throw(RegionNotKnown, SaveInProgress, RestoreInProgress, RTIinternalError);

    void subscribeAttributesWR(FederateHandle,
                               ObjectClassHandle,
                               RegionHandle,
                               unsigned short,
                               const std::vector<AttributeHandle>&) throw(RegionNotKnown,
                                                                          SaveInProgress,
                                                                          RestoreInProgress,
                                                                          RTIinternalError);

    void unsubscribeAttributesWR(FederateHandle, ObjectClassHandle, RegionHandle) throw(RegionNotKnown,
                                                                                        SaveInProgress,
                                                                                        RestoreInProgress,
                                                                                        RTIinternalError);

    void subscribeInteractionWR(FederateHandle, InteractionClassHandle, RegionHandle) throw(RegionNotKnown,
                                                                                            SaveInProgress,
                                                                                            RestoreInProgress,
                                                                                            RTIinternalError);

    void unsubscribeInteractionWR(FederateHandle, InteractionClassHandle, RegionHandle) throw(RegionNotKnown,
                                                                                              SaveInProgress,
                                                                                              RestoreInProgress,
                                                                                              RTIinternalError);

    ObjectHandle registerObjectWithRegion(FederateHandle,
                                          ObjectClassHandle,
                                          const std::string&,
                                          RegionHandle,
                                          int,
                                          const std::vector<AttributeHandle>&) throw(ObjectClassNotDefined,
                                                                                     ObjectClassNotPublished,
                                                                                     AttributeNotDefined,
                                                                                     AttributeNotPublished,
                                                                                     RegionNotKnown,
                                                                                     InvalidRegionContext,
                                                                                     ObjectAlreadyRegistered,
                                                                                     SaveInProgress,
                                                                                     RestoreInProgress,
                                                                                     RTIinternalError);

    // switches

    void setClassRelevanceAdvisorySwitch(FederateHandle theHandle) throw(FederateNotExecutionMember,
                                                                         SaveInProgress,
                                                                         RestoreInProgress,
                                                                         RTIinternalError);

    void unsetClassRelevanceAdvisorySwitch(FederateHandle theHandle) throw(FederateNotExecutionMember,
                                                                           SaveInProgress,
                                                                           RestoreInProgress,
                                                                           RTIinternalError);

    void setInteractionRelevanceAdvisorySwitch(FederateHandle theHandle) throw(FederateNotExecutionMember,
                                                                               SaveInProgress,
                                                                               RestoreInProgress,
                                                                               RTIinternalError);

    void unsetInteractionRelevanceAdvisorySwitch(FederateHandle theHandle) throw(FederateNotExecutionMember,
                                                                                 SaveInProgress,
                                                                                 RestoreInProgress,
                                                                                 RTIinternalError);

    void setAttributeRelevanceAdvisorySwitch(FederateHandle theHandle) throw(FederateNotExecutionMember,
                                                                             SaveInProgress,
                                                                             RestoreInProgress,
                                                                             RTIinternalError);

    void unsetAttributeRelevanceAdvisorySwitch(FederateHandle theHandle) throw(FederateNotExecutionMember,
                                                                               SaveInProgress,
                                                                               RestoreInProgress,
                                                                               RTIinternalError);

    void setAttributeScopeAdvisorySwitch(FederateHandle theHandle) throw(FederateNotExecutionMember,
                                                                         SaveInProgress,
                                                                         RestoreInProgress,
                                                                         RTIinternalError);

    void unsetAttributeScopeAdvisorySwitch(FederateHandle theHandle) throw(FederateNotExecutionMember,
                                                                           SaveInProgress,
                                                                           RestoreInProgress,
                                                                           RTIinternalError);

    void getFOM(NM_Join_Federation_Execution& objectModelData);

    /**
     * Update the last NERx message date for the concerned federate.
     * @param[in] federate the handle of the federate for which we want to update NERx time.
     * @param[in] date the new NERx date for the specified federate.
     */
    bool updateLastNERxForFederate(FederateHandle federate, FederationTime date) throw(FederateNotExecutionMember);

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

private:
    Handle handle;
    std::string name;
    std::string FEDid;

    /// Labels and Tags not on synchronization.
    std::map<std::string, std::string> synchronizationLabels;

    HandleManager<FederateHandle> federateHandles;
    HandleManager<ObjectHandle> objectHandles;

    /** This object is initialized when the Federation is created
     * 
     *  with the reference of the RTIG managed Socket Server. The reference of
     *  this object is passed down the Classes Tree with the help of RootObj.
     */
    SecurityServer* server;
    RootObject* root;

    LBTS regulators;

#ifdef FEDERATION_USES_MULTICAST
    SocketMC* MCLink;
#endif

    bool saveXmlData();
    bool restoreXmlData(std::string docFilename);

    /// Broadcast 'msg' to all Federate except the specified one (unless this is an anonymous update)
    void broadcastAnyMessage(NetworkMessage* msg, FederateHandle Except, bool anonymous);

    /// Broadcast 'msg' to some Federates except the specified one
    void broadcastSomeMessage(NetworkMessage* msg,
                              FederateHandle Except,
                              const std::vector<FederateHandle>& fede_array,
                              uint32_t nbfed);

    /// Return the Federate whose Name is theName, if found.
    Federate& getFederate(const std::string& theName) throw(FederateNotExecutionMember);

    /// Return the Federate whose Handle is theHandle, if found.
    Federate& getFederate(FederateHandle theHandle) throw(FederateNotExecutionMember);

    // Private attributes
    typedef std::map<FederateHandle, Federate> HandleFederateMap;
    HandleFederateMap _handleFederateMap;
    bool saveInProgress;
    bool restoreInProgress;
    bool saveStatus; /// True if saving was correctly done, false otherwise.
    bool restoreStatus; /// True if restoring was correctly done.
    int verboseLevel;
    std::string saveLabel; /// The label associated with the save request.

    /// The minimum NERx timestamp for this federation
    FederationTime minNERx;

    /// The message buffer used to send Network messages
    MessageBuffer NM_msgBufSend;
};
}
} // namespace certi/rtig

#endif // _CERTI_RTIG_FEDERATION_HH
