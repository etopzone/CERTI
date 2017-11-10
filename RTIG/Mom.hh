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

#ifndef _CERTI_RTIG_MOM_HH
#define _CERTI_RTIG_MOM_HH

#include <unordered_map>

#include "Federation.hh"
#include "RootObject.hh"

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

    enum class OrderType {
        Receive,
        TimeStamp
    };

    static bool isAvailableInRootObjectAndCompliant(const RootObject& root);

    Mom(const FederateHandle handle, Federation& federation, RootObject& root);

    FederateHandle getHandle() const;

    // Initialization
    void publishObjects();
    void publishAndSubscribeInteractions();

    // Object management
    void registerFederation();
    void registerFederate(const Federate& federate);
    void deleteFederate(const FederateHandle federate_handle);

    // Conditional Attributes
    // Federation
    void updateFederatesInFederation();
    void updateFomModuleDesignatorList(const std::vector<std::string>& value);
    void updateCurrentFDD(const std::string& value);
    void updateLastSave();
    void updateNextSave(const std::string& label, const FederationTime& time);
    void updateAutoProvide(const bool value);

    // Federate
    void updateTimeConstrained(const Federate& federate);
    void updateTimeRegulating(const Federate& federate);
    void updateAsynchronousDelivery(const FederateHandle federate_handle, const bool value);
    void updateFederateState(const Federate& federate);
    void updateTimeManagerState(const FederateHandle federate_handle, const TimeState value);
    void updateLogicalTime(const FederateHandle federate_handle, const FederationTime& value);
    void updateLookahead(const FederateHandle federate_handle, const FederationTime& value);
    void updateGALT(const FederateHandle federate_handle, const FederationTime& value);
    void updateLITS(const FederateHandle federate_handle, const FederationTime& value);
    void updateRoLenght(const FederateHandle federate_handle, const int delta);
    void updateTsoLenght(const FederateHandle federate_handle, const int delta);
    void updateReflectionsReceived(const FederateHandle federate_handle, const int delta);
    void updateUpdatesSent(const FederateHandle federate_handle, const int delta);
    void updateInteractionsReceived(const FederateHandle federate_handle, const int delta);
    void updateInteractionsSent(const FederateHandle federate_handle, const int delta);
    void updateObjectInstancesThatCanBeDeleted(const FederateHandle federate_handle, const int delta);
    void updateObjectInstancesUpdated(const FederateHandle federate_handle, const int delta);
    void updateObjectInstancesReflected(const FederateHandle federate_handle, const int delta);
    void updateObjectInstancesDeleted(const FederateHandle federate_handle, const int delta);
    void updateObjectInstancesRemoved(const FederateHandle federate_handle, const int delta);
    void updateObjectInstancesRegistered(const FederateHandle federate_handle, const int delta);
    void updateObjectInstancesDiscovered(const FederateHandle federate_handle, const int delta);
    void updateTimeGrantedTime(const FederateHandle federate_handle, const int value);
    void updateTimeAdvancingTime(const FederateHandle federate_handle, const int value);
    void updateConveyRegionDesignatorSets(const FederateHandle federate_handle, const bool value);
    void updateConveyProducingFederate(const FederateHandle federate_handle, const bool value);

    // Interactions
    void processInteraction(/*const FederateHandle federate_handle,*/
                            const InteractionClassHandle interaction_class_handle,
                            const std::vector<ParameterHandle>& parameter_handles,
                            const std::vector<ParameterValue_t>& parameter_values,
                            const RegionHandle region_handle);

    void processFederateSetTiming(const FederateHandle& federate_handle, const int reportPeriod);
    void processFederateModifyAttributeState(const FederateHandle& federate_handle,
                                             const ObjectHandle objectInstance,
                                             const AttributeHandle attribute,
                                             const bool attributeState);
    void processFederateSetSwitches(const FederateHandle& federate_handle,
                                    const bool conveyRegionDesignatorSets,
                                    const bool conveyProducingFederate,
                                    const bool serviceReporting,
                                    const bool exceptionReporting);
    void processFederateRequestPublications(const FederateHandle& federate_handle);
    void processFederateRequestSubscriptions(const FederateHandle& federate_handle);
    void processFederateRequestObjectInstancesThatCanBeDeleted(const FederateHandle& federate_handle);
    void processFederateRequestObjectInstancesUpdated(const FederateHandle& federate_handle);
    void processFederateRequestObjectInstancesReflected(const FederateHandle& federate_handle);
    void processFederateRequestUpdatesSent(const FederateHandle& federate_handle);
    void processFederateRequestInteractionsSent(const FederateHandle& federate_handle);
    void processFederateRequestReflectionsReceived(const FederateHandle& federate_handle);
    void processFederateRequestInteractionsReceived(const FederateHandle& federate_handle);
    void processFederateRequestObjectInstanceInformation(const FederateHandle& federate_handle,
                                                         const ObjectHandle& objectInstance);
    void processFederateRequestFOMmoduleData(const FederateHandle& federate_handle, const int FOMmoduleIndicator);
    void processFederateResignFederationExecution(const FederateHandle& federate_handle,
                                                  const ResignAction resignAction);
    void processFederateSynchronizationPointAchieved(const FederateHandle& federate_handle, const std::string& label);
    void processFederateFederateSaveBegun(const FederateHandle& federate_handle);
    void processFederateFederateSaveComplete(const FederateHandle& federate_handle, const bool successIndicator);
    void processFederateFederateRestoreComplete(const FederateHandle& federate_handle, const bool successIndicator);
    void processFederatePublishObjectClassAttributes(const FederateHandle& federate_handle,
                                                     const ObjectClassHandle& objectClass,
                                                     const std::vector<AttributeHandle>& attributeList);
    void processFederateUnpublishObjectClassAttributes(const FederateHandle& federate_handle,
                                                       const ObjectClassHandle& objectClass,
                                                       const std::vector<AttributeHandle>& attributeList);
    void processFederatePublishInteractionClass(const FederateHandle& federate_handle,
                                                const InteractionClassHandle& interactionClass);
    void processFederateUnpublishInteractionClass(const FederateHandle& federate_handle,
                                                  const InteractionClassHandle& interactionClass);
    void processFederateSubscribeObjectClassAttributes(const FederateHandle& federate_handle,
                                                       const ObjectClassHandle& objectClass,
                                                       const std::vector<AttributeHandle>& attributeList,
                                                       const bool active);
    void processFederateUnsubscribeObjectClassAttributes(const FederateHandle& federate_handle,
                                                         const ObjectClassHandle& objectClass,
                                                         const std::vector<AttributeHandle>& attributeList);
    void processFederateSubscribeInteractionClass(const FederateHandle& federate_handle,
                                                  const InteractionClassHandle& interactionClass,
                                                  const bool active);
    void processFederateUnsubscribeInteractionClass(const FederateHandle& federate_handle,
                                                    const InteractionClassHandle& interactionClass);
    void processFederateDeleteObjectInstance(const FederateHandle& federate_handle,
                                             const ObjectHandle& objectInstance,
                                             const std::string& tag,
                                             const FederationTime& timeStamp);
    void processFederateLocalDeleteObjectInstance(const FederateHandle& federate_handle,
                                                  const ObjectHandle& objectInstance);
    void processFederateRequestAttributeTransportationTypeChange(const FederateHandle& federate_handle,
                                                                 const ObjectHandle& objectInstance,
                                                                 const std::vector<AttributeHandle>& attributeList,
                                                                 const std::string& transportation);
    void processFederateRequestInteractionTransportationTypeChange(const FederateHandle& federate_handle,
                                                                   const InteractionClassHandle& interactionClass,
                                                                   const std::string& transportation);
    void processFederateUnconditionalAttributeOwnershipDivestiture(const FederateHandle& federate_handle,
                                                                   const ObjectHandle& objectInstance,
                                                                   const std::vector<AttributeHandle>& attributeList);
    void processFederateEnableTimeRegulation(const FederateHandle& federate_handle, const int lookahead);
    void processFederateDisableTimeRegulation(const FederateHandle& federate_handle);
    void processFederateEnableTimeConstrained(const FederateHandle& federate_handle);
    void processFederateDisableTimeConstrained(const FederateHandle& federate_handle);
    void processFederateTimeAdvanceRequest(const FederateHandle& federate_handle, const FederationTime& timeStamp);
    void processFederateTimeAdvanceRequestAvailable(const FederateHandle& federate_handle, const FederationTime& timeStamp);
    void processFederateNextMessageRequest(const FederateHandle& federate_handle, const FederationTime& timeStamp);
    void processFederateNextMessageRequestAvailable(const FederateHandle& federate_handle,
                                                    const FederationTime& timeStamp);
    void processFederateFlushQueueRequest(const FederateHandle& federate_handle, const FederationTime& timeStamp);
    void processFederateEnableAsynchronousDelivery(const FederateHandle& federate_handle);
    void processFederateDisableAsynchronousDelivery(const FederateHandle& federate_handle);
    void processFederateModifyLookahead(const FederateHandle& federate_handle, const int lookahead);
    void processFederateChangeAttributeOrderType(const FederateHandle& federate_handle,
                                                 const ObjectHandle& objectInstance,
                                                 const std::vector<AttributeHandle>& attributeList,
                                                 const OrderType& sendOrder);
    void processFederateChangeInteractionOrderType(const FederateHandle& federate_handle,
                                                   const InteractionClassHandle& interactionClass,
                                                   const OrderType& sendOrder);
    void processFederationSetSwitches(const bool autoProvide);
    void processFederationRequestSynchronizationPoints();
    void processFederationRequestSynchronizationPointStatus(const std::string& syncPointName);
    void processFederationRequestFOMmoduleData(const int FOMmoduleIndicator);
    void processFederationRequestMIMData();

    // Support
    void provideAttributeValueUpdate(const ObjectHandle& object, const std::vector<AttributeHandle>& attributes);
    void preparePeriodicAttributeValueUpdate(const ObjectHandle& object, const std::vector<AttributeHandle>& attributes);

private:
    void display() const;
    
    ParameterHandle getParameterHandle(const InteractionClassHandle interaction, const std::string& name);
    
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

    std::map<ObjectHandle, std::set<AttributeHandle>> my_attributes_to_update_periodically;

    AttributeValue_t encodeString(const std::string& str);
    AttributeValue_t encodeBoolean(const bool data);
    AttributeValue_t encodeUInt32(const uint32_t data);
    AttributeValue_t encodeFederateHandleList();
    AttributeValue_t encodeFederateState(const Federate& federate);
    AttributeValue_t encodeVectorHandle(const std::vector<Handle>& data);
    AttributeValue_t encodeObjectClassBasedCounts(std::map<Handle, int> data);

    std::string decodeString(const ParameterValue_t& data);
    bool decodeBoolean(const ParameterValue_t& data);
    uint32_t decodeUInt32(const ParameterValue_t& data);
    ResignAction decodeResignAction(const ParameterValue_t& data);
    std::vector<AttributeHandle> decodeVectorAttributeHandle(const ParameterValue_t& data);
    FedTime decodeFederationTime(const ParameterValue_t& data);
    OrderType decodeOrderType(const ParameterValue_t& data);
    

    AttributeValue_t encodeMB();

    MessageBuffer mb;
};
}
} // namespace certi/rtig

#endif // _CERTI_RTIG_MOM_HH
