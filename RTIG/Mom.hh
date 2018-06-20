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

#ifndef _CERTI_RTIG_MOM_HH
#define _CERTI_RTIG_MOM_HH

#include <chrono>
#include <unordered_map>

#include <libCERTI/MessageEvent.hh>
#include <libCERTI/RootObject.hh>

#include "Federation.hh"

namespace certi {
class RootObject;

namespace rtig {
class Federation;

class Mom {
public:
    enum class FederateState { Active, SaveInProgress, RestoreInProgress };

    enum class TimeState { TimeGranted, TimeAdvancing };

    enum class ResignAction {
        DivestOwnership,
        DeleteObjectInstances,
        CancelPendingAcquisitions,
        DeleteObjectInstancesThenDivestOwnership,
        CancelPendingAcquisitionsThenDeleteObjectInstancesThenDivestOwnership,
        NoActions
    };

    enum class OrderType { Receive, TimeStamp };

    enum class SyncPointStatus {
        NoActivity,
        AttemptingToRegisterSyncPoint,
        MovingToSyncPoint,
        WaitingForRestOfFederation
    };

    static bool isAvailableInRootObjectAndCompliant(const RootObject& root);

    Mom(const FederateHandle handle, Federation& federation, RootObject& root);

    FederateHandle getHandle() const;

    // Initialization
    void publishObjects();
    void publishAndSubscribeInteractions();

    // Object management
    Responses registerFederation();
    Responses registerFederate(const Federate& federate, SocketTCP* tcp_link, const std::vector<std::string>& additional_fom_modules);
    void deleteFederate(const FederateHandle federate_handle);

    // Federation
    // Conditional Attributes
    Responses updateFederatesInFederation();
    Responses updateFomModuleDesignatorList(); // FIXME never called
    Responses updateCurrentFDD(); // FIXME never called
    Responses updateLastSave();
    Responses updateNextSave(const std::string& label, const FederationTime& time);
    Responses updateAutoProvide(const bool value);

    // Federate
    // Conditional Attributes
    Responses updateTimeConstrained(const Federate& federate);
    Responses updateTimeRegulating(const Federate& federate);
    Responses updateAsynchronousDelivery(const FederateHandle federate_handle, const bool value);
    Responses updateFederateState(const Federate& federate);
    Responses updateTimeManagerState(const FederateHandle federate_handle, const TimeState value);
    Responses updateConveyRegionDesignatorSets(const FederateHandle federate_handle, const bool value);
    Responses updateConveyProducingFederate(const FederateHandle federate_handle, const bool value);
    // Periodic Attributes
    Responses updateLogicalTime(const FederateHandle federate_handle, const FederationTime& value);
    Responses updateLookahead(const FederateHandle federate_handle, const FederationTime& value);
    Responses
    updateGALT(const FederateHandle federate_handle,
               const FederationTime& value); // TODO gathered from RTIA but check if we cannot get them in RTIG
    Responses updateLITS(const FederateHandle federate_handle, const FederationTime& value); // idem
    Responses updateRoLenght(const FederateHandle federate_handle,
                             const int delta = 1); // TODO check if available from queues or not compliant
    Responses updateTsoLenght(const FederateHandle federate_handle, const int delta = 1); // idem
    Responses updateReflectionsReceived(const FederateHandle federate_handle, const int delta = 1);
    Responses updateUpdatesSent(const FederateHandle federate_handle, const int delta = 1);
    Responses updateInteractionsReceived(const FederateHandle federate_handle, const int delta = 1);
    Responses updateInteractionsSent(const FederateHandle federate_handle, const int delta = 1);
    Responses updateObjectInstancesThatCanBeDeleted(const FederateHandle federate_handle, const int delta = 1);
    Responses updateObjectInstancesUpdated(const FederateHandle federate_handle);
    Responses updateObjectInstancesReflected(const FederateHandle federate_handle);
    Responses updateObjectInstancesDeleted(const FederateHandle federate_handle, const int delta = 1);
    Responses updateObjectInstancesRemoved(const FederateHandle federate_handle, const int delta = 1);
    Responses updateObjectInstancesRegistered(const FederateHandle federate_handle, const int delta = 1);
    Responses updateObjectInstancesDiscovered(const FederateHandle federate_handle, const int delta = 1);
    Responses
    updateTimeGrantedTime(const FederateHandle federate_handle,
                          const int value); // TODO this will be resource intensive, check if we want to support it
    Responses updateTimeAdvancingTime(const FederateHandle federate_handle, const int value); // idem

    Responses
    updateCountAttribute(const FederateHandle& federate_handle, const std::string& attribute_name, const int delta);

    Responses provideAttributeValueUpdate(const ObjectHandle& object, const std::vector<AttributeHandle>& attributes);

    // Interactions
    Responses processInteraction(const InteractionClassHandle interaction_class_handle,
                                 const std::vector<ParameterHandle>& parameter_handles,
                                 const std::vector<ParameterValue_t>& parameter_values,
                                 const RegionHandle region_handle);

    Responses processFederateSetTiming(const FederateHandle& federate_handle, const int reportPeriod);
    Responses processFederateModifyAttributeState(const FederateHandle& federate_handle,
                                                  const ObjectHandle objectInstance,
                                                  const AttributeHandle attribute,
                                                  const bool attributeState);
    Responses processFederateSetSwitches(const FederateHandle& federate_handle,
                                         const bool conveyRegionDesignatorSets,
                                         const bool conveyProducingFederate,
                                         const bool serviceReporting,
                                         const bool exceptionReporting);
    Responses processFederateRequestPublications(const FederateHandle& federate_handle);
    Responses processFederateRequestSubscriptions(const FederateHandle& federate_handle);
    Responses processFederateRequestObjectInstancesThatCanBeDeleted(const FederateHandle& federate_handle);
    Responses processFederateRequestObjectInstancesUpdated(const FederateHandle& federate_handle);
    Responses processFederateRequestObjectInstancesReflected(const FederateHandle& federate_handle);
    Responses processFederateRequestUpdatesSent(const FederateHandle& federate_handle);
    Responses processFederateRequestInteractionsSent(const FederateHandle& federate_handle);
    Responses processFederateRequestReflectionsReceived(const FederateHandle& federate_handle);
    Responses processFederateRequestInteractionsReceived(const FederateHandle& federate_handle);
    Responses processFederateRequestObjectInstanceInformation(const FederateHandle& federate_handle,
                                                              const ObjectHandle& objectInstance);
    Responses processFederateRequestFOMmoduleData(const FederateHandle& federate_handle, const int FOMmoduleIndicator);
    Responses processFederateResignFederationExecution(const FederateHandle& federate_handle,
                                                       const ResignAction resignAction);
    Responses processFederateSynchronizationPointAchieved(const FederateHandle& federate_handle,
                                                          const std::string& label);
    Responses processFederateFederateSaveBegun(const FederateHandle& federate_handle);
    Responses processFederateFederateSaveComplete(const FederateHandle& federate_handle, const bool successIndicator);
    Responses processFederateFederateRestoreComplete(const FederateHandle& federate_handle,
                                                     const bool successIndicator);
    Responses processFederatePublishObjectClassAttributes(const FederateHandle& federate_handle,
                                                          const ObjectClassHandle& objectClass,
                                                          const std::vector<AttributeHandle>& attributeList);
    Responses processFederateUnpublishObjectClassAttributes(const FederateHandle& federate_handle,
                                                            const ObjectClassHandle& objectClass,
                                                            const std::vector<AttributeHandle>& attributeList);
    Responses processFederatePublishInteractionClass(const FederateHandle& federate_handle,
                                                     const InteractionClassHandle& interactionClass);
    Responses processFederateUnpublishInteractionClass(const FederateHandle& federate_handle,
                                                       const InteractionClassHandle& interactionClass);
    Responses processFederateSubscribeObjectClassAttributes(const FederateHandle& federate_handle,
                                                            const ObjectClassHandle& objectClass,
                                                            const std::vector<AttributeHandle>& attributeList,
                                                            const bool active);
    Responses processFederateUnsubscribeObjectClassAttributes(const FederateHandle& federate_handle,
                                                              const ObjectClassHandle& objectClass,
                                                              const std::vector<AttributeHandle>& attributeList);
    Responses processFederateSubscribeInteractionClass(const FederateHandle& federate_handle,
                                                       const InteractionClassHandle& interactionClass,
                                                       const bool active);
    Responses processFederateUnsubscribeInteractionClass(const FederateHandle& federate_handle,
                                                         const InteractionClassHandle& interactionClass);
    Responses processFederateDeleteObjectInstance(const FederateHandle& federate_handle,
                                                  const ObjectHandle& objectInstance,
                                                  const std::string& tag,
                                                  const FederationTime& timeStamp);
    Responses processFederateLocalDeleteObjectInstance(const FederateHandle& federate_handle,
                                                       const ObjectHandle& objectInstance);
    Responses processFederateRequestAttributeTransportationTypeChange(const FederateHandle& federate_handle,
                                                                      const ObjectHandle& objectInstance,
                                                                      const std::vector<AttributeHandle>& attributeList,
                                                                      const std::string& transportation);
    Responses processFederateRequestInteractionTransportationTypeChange(const FederateHandle& federate_handle,
                                                                        const InteractionClassHandle& interactionClass,
                                                                        const std::string& transportation);
    Responses
    processFederateUnconditionalAttributeOwnershipDivestiture(const FederateHandle& federate_handle,
                                                              const ObjectHandle& objectInstance,
                                                              const std::vector<AttributeHandle>& attributeList);
    Responses processFederateEnableTimeRegulation(const FederateHandle& federate_handle, const int lookahead);
    Responses processFederateDisableTimeRegulation(const FederateHandle& federate_handle);
    Responses processFederateEnableTimeConstrained(const FederateHandle& federate_handle);
    Responses processFederateDisableTimeConstrained(const FederateHandle& federate_handle);
    Responses processFederateTimeAdvanceRequest(const FederateHandle& federate_handle, const FederationTime& timeStamp);
    Responses processFederateTimeAdvanceRequestAvailable(const FederateHandle& federate_handle,
                                                         const FederationTime& timeStamp);
    Responses processFederateNextMessageRequest(const FederateHandle& federate_handle, const FederationTime& timeStamp);
    Responses processFederateNextMessageRequestAvailable(const FederateHandle& federate_handle,
                                                         const FederationTime& timeStamp);
    Responses processFederateFlushQueueRequest(const FederateHandle& federate_handle, const FederationTime& timeStamp);
    Responses processFederateEnableAsynchronousDelivery(const FederateHandle& federate_handle);
    Responses processFederateDisableAsynchronousDelivery(const FederateHandle& federate_handle);
    Responses processFederateModifyLookahead(const FederateHandle& federate_handle, const int lookahead);
    Responses processFederateChangeAttributeOrderType(const FederateHandle& federate_handle,
                                                      const ObjectHandle& objectInstance,
                                                      const std::vector<AttributeHandle>& attributeList,
                                                      const OrderType& sendOrder);
    Responses processFederateChangeInteractionOrderType(const FederateHandle& federate_handle,
                                                        const InteractionClassHandle& interactionClass,
                                                        const OrderType& sendOrder);
    Responses processFederationSetSwitches(const bool autoProvide);
    Responses processFederationRequestSynchronizationPoints();
    Responses processFederationRequestSynchronizationPointStatus(const std::string& syncPointName);
    Responses processFederationRequestFOMmoduleData(const int FOMmoduleIndicator);
    Responses processFederationRequestMIMData();

    void registerObjectInstanceUpdated(const FederateHandle federate,
                                       const ObjectClassHandle object,
                                       const ObjectHandle instance);
    void registerObjectInstanceReflected(const FederateHandle federate,
                                         const ObjectClassHandle object,
                                         const ObjectHandle instance);
    void registerUpdate(const FederateHandle federate, const ObjectClassHandle object);
    void registerReflection(const FederateHandle federate, const ObjectClassHandle object);
    void registerInteractionSent(const FederateHandle federate, const InteractionClassHandle interaction);
    void registerInteractionReceived(const FederateHandle federate, const InteractionClassHandle interaction);

private:
    struct FederateUpdateSettings {
        std::chrono::seconds updateRate{0};
        std::chrono::time_point<std::chrono::system_clock> lastUpdate{};
    };

    // Support
    Responses preparePeriodicAttributeValueUpdate(const ObjectHandle& object,
                                                  const std::vector<AttributeHandle>& attributes);
    Responses providePeriodicAttributeValueUpdatesIfApplicable();

    void display() const;

    ParameterHandle getParameterHandle(const InteractionClassHandle interaction, const std::string& name);

    Socket* getSocketForFederate(const FederateHandle& federate_handle);

    /** This handle is used to detect MOM interactions.
     * It does not really belong to the federation.
     */
    const FederateHandle my_handle;

    Federation& my_federation;
    RootObject& my_root;

    ObjectHandle my_federation_object;
    std::unordered_map<FederateHandle, ObjectHandle> my_federate_objects;

    std::unordered_map<std::string, ObjectClassHandle> my_object_class_cache;
    std::unordered_map<std::string, AttributeHandle> my_attribute_cache;
    std::unordered_map<std::string, InteractionClassHandle> my_interaction_class_cache;
    std::unordered_map<std::string, ParameterHandle> my_parameter_cache;

    std::map<ObjectHandle, std::map<AttributeHandle, AttributeValue_t>> my_attribute_values_cache;
    std::map<FederateHandle, std::map<AttributeHandle, int>> my_count_attributes_cache;

    std::map<FederateHandle, FederateUpdateSettings> my_federates_update_settings;
    std::map<FederateHandle, std::set<AttributeHandle>> my_attributes_to_update_periodically;

    AttributeValue_t encodeString(const std::string& str);
    AttributeValue_t encodeStringList(const std::vector<std::string>& strs);
    AttributeValue_t encodeBoolean(const bool data);
    AttributeValue_t encodeUInt32(const uint32_t data);
    AttributeValue_t encodeVersion(const RtiVersion version);
    AttributeValue_t encodeTime(const FederationTime& time);
    AttributeValue_t encodeIp(const unsigned long ip);
    AttributeValue_t encodeFederateHandleList();
    AttributeValue_t encodeFederateState(const Federate& federate);
    AttributeValue_t encodeVectorHandle(const std::vector<Handle>& data);
    AttributeValue_t encodeHandleBasedCounts(std::map<Handle, int> data);
    AttributeValue_t encodeVectorString(const std::vector<std::string>& data);

    std::string decodeString(const ParameterValue_t& data);
    bool decodeBoolean(const ParameterValue_t& data);
    uint32_t decodeUInt32(const ParameterValue_t& data);
    ResignAction decodeResignAction(const ParameterValue_t& data);
    std::vector<AttributeHandle> decodeVectorAttributeHandle(const ParameterValue_t& data);
    FederationTime decodeFederationTime(const ParameterValue_t& data);
    OrderType decodeOrderType(const ParameterValue_t& data);

    AttributeValue_t encodeMB();

    MessageBuffer mb;

    std::map<FederateHandle, std::map<ObjectClassHandle, std::unordered_set<ObjectHandle>>> my_object_instances_updated;
    std::map<FederateHandle, std::map<ObjectClassHandle, std::unordered_set<ObjectHandle>>>
        my_object_instances_reflected;
    std::map<FederateHandle, std::map<ObjectClassHandle, int>> my_updates_sent;
    std::map<FederateHandle, std::map<ObjectClassHandle, int>> my_reflections_received;
    std::map<FederateHandle, std::map<InteractionClassHandle, int>> my_interactions_sent;
    std::map<FederateHandle, std::map<InteractionClassHandle, int>> my_interactions_received;
};
}
} // namespace certi/rtig

#endif // _CERTI_RTIG_MOM_HH
