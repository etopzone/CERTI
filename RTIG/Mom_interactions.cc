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

#include "Mom.hh"

#include "PrettyDebug.hh"

using std::cout;
using std::endl;

namespace certi {
namespace rtig {

static PrettyDebug D("MOM", __FILE__);

void Mom::processInteraction(const FederateHandle federate_handle,
                             const InteractionClassHandle interaction_class_handle,
                             const std::vector<ParameterHandle>& parameter_handles,
                             const std::vector<ParameterValue_t>& parameter_values,
                             const RegionHandle region_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processInteraction" << endl;

    Debug(D, pdGendoc) << federate_handle << endl;
    Debug(D, pdGendoc) << interaction_class_handle << endl;
    Debug(D, pdGendoc) << parameter_handles.size() << endl;
    Debug(D, pdGendoc) << region_handle << endl;

    if (interaction_class_handle == my_interaction_class_cache["HLAmanager.HLAfederate.HLAadjust.HLAsetTiming"]) {
        int reportPeriod;
        processFederateSetTiming(federate_handle, reportPeriod);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAadjust.HLAmodifyAttributeState"]) {
        ObjectHandle objectInstance;
        AttributeHandle attribute;
        bool HLAattributeState;
        processFederateModifyAttributeState(federate_handle, objectInstance, attribute, HLAattributeState);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAadjust.HLAsetSwitches"]) {
        bool conveyRegionDesignatorSets;
        bool conveyProducingFederate;
        bool serviceReporting;
        bool exceptionReporting;
        processFederateSetSwitches(
            federate_handle, conveyRegionDesignatorSets, conveyProducingFederate, serviceReporting, exceptionReporting);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestPublications"]) {
        processFederateRequestPublications(federate_handle);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestSubscriptions"]) {
        processFederateRequestSubscriptions(federate_handle);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache
                    ["HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesThatCanBeDeleted"]) {
        processFederateRequestObjectInstancesThatCanBeDeleted(federate_handle);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesUpdated"]) {
        processFederateRequestObjectInstancesUpdated(federate_handle);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesReflected"]) {
        processFederateRequestObjectInstancesReflected(federate_handle);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestUpdatesSent"]) {
        processFederateRequestUpdatesSent(federate_handle);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestInteractionsSent"]) {
        processFederateRequestInteractionsSent(federate_handle);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestReflectionsReceived"]) {
        processFederateRequestReflectionsReceived(federate_handle);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestInteractionsReceived"]) {
        processFederateRequestInteractionsReceived(federate_handle);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstanceInformation"]) {
        ObjectHandle objectInstance;
        processFederateRequestObjectInstanceInformation(federate_handle,
                                                        objectInstance);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestFOMmoduleData"]) {
        int FOMmoduleIndicator;
        processFederateRequestFOMmoduleData(federate_handle, FOMmoduleIndicator);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAresignFederationExecution"]) {
        ResignAction resignAction;
        processFederateResignFederationExecution(federate_handle, resignAction);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAsynchronizationPointAchieved"]) {
        std::string label;
        processFederateSynchronizationPointAchieved(federate_handle, label);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAfederateSaveBegun"]) {
        processFederateFederateSaveBegun(federate_handle);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAfederateSaveComplete"]) {
        bool successIndicator;
        processFederateFederateSaveComplete(federate_handle, successIndicator);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAfederateRestoreComplete"]) {
        bool successIndicator;
        processFederateFederateRestoreComplete(federate_handle, successIndicator);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLApublishObjectClassAttributes"]) {
        ObjectClassHandle objectClass;
        std::vector<AttributeHandle> attributeList;
        processFederatePublishObjectClassAttributes(federate_handle, objectClass, attributeList);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAunpublishObjectClassAttributes"]) {
        ObjectClassHandle objectClass;
        std::vector<AttributeHandle> attributeList;
        processFederateUnpublishObjectClassAttributes(federate_handle, objectClass, attributeList);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLApublishInteractionClass"]) {
        InteractionClassHandle interactionClass;
        processFederatePublishInteractionClass(federate_handle, interactionClass);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAunpublishInteractionClass"]) {
        InteractionClassHandle interactionClass;
        processFederateUnpublishInteractionClass(federate_handle, interactionClass);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAsubscribeObjectClassAttributes"]) {
        ObjectClassHandle objectClass;
        std::vector<AttributeHandle> attributeList;
        bool active;
        processFederateSubscribeObjectClassAttributes(federate_handle, objectClass, attributeList, active);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAunsubscribeObjectClassAttributes"]) {
        ObjectClassHandle objectClass;
        std::vector<AttributeHandle> attributeList;
        processFederateUnsubscribeObjectClassAttributes(federate_handle, objectClass, attributeList);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAsubscribeInteractionClass"]) {
        InteractionClassHandle interactionClass;
        bool active;
        processFederateSubscribeInteractionClass(federate_handle, interactionClass, active);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAunsubscribeInteractionClass"]) {
        InteractionClassHandle interactionClass;
        processFederateUnsubscribeInteractionClass(federate_handle, interactionClass);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAdeleteObjectInstance"]) {
        ObjectHandle objectInstance;
        std::string tag;
        FederationTime timeStamp;
        processFederateDeleteObjectInstance(federate_handle, objectInstance, tag, timeStamp);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAlocalDeleteObjectInstance"]) {
        ObjectHandle objectInstance;
        processFederateLocalDeleteObjectInstance(federate_handle, objectInstance);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache
                    ["HLAmanager.HLAfederate.HLAservice.HLArequestAttributeTransportationTypeChange"]) {
        ObjectHandle objectInstance;
        std::vector<AttributeHandle> attributeList;
        std::string transportation;
        processFederateRequestAttributeTransportationTypeChange(
            federate_handle, objectInstance, attributeList, transportation);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache
                    ["HLAmanager.HLAfederate.HLAservice.HLArequestInteractionTransportationTypeChange"]) {
        InteractionClassHandle interactionClass;
        std::string transportation;
        processFederateRequestInteractionTransportationTypeChange(federate_handle, interactionClass, transportation);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache
                    ["HLAmanager.HLAfederate.HLAservice.HLAunconditionalAttributeOwnershipDivestiture"]) {
        ObjectHandle objectInstance;
        std::vector<AttributeHandle> attributeList;
        processFederateUnconditionalAttributeOwnershipDivestiture(federate_handle, objectInstance, attributeList);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAenableTimeRegulation"]) {
        int lookahead;
        processFederateEnableTimeRegulation(federate_handle, lookahead);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAdisableTimeRegulation"]) {
        processFederateDisableTimeRegulation(federate_handle);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAenableTimeConstrained"]) {
        processFederateEnableTimeConstrained(federate_handle);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAdisableTimeConstrained"]) {
        processFederateDisableTimeConstrained(federate_handle);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAtimeAdvanceRequest"]) {
        FederationTime timeStamp;
        processFederateTimeAdvanceRequest(federate_handle, timeStamp);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAtimeAdvanceRequestAvailable"]) {
        FederationTime timeStamp;
        processFederateTimeAdvanceRequestAvailable(federate_handle, timeStamp);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAnextMessageRequest"]) {
        FederationTime timeStamp;
        processFederateNextMessageRequest(federate_handle, timeStamp);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAnextMessageRequestAvailable"]) {
        FederationTime timeStamp;
        processFederateNextMessageRequestAvailable(federate_handle, timeStamp);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAflushQueueRequest"]) {
        FederationTime timeStamp;
        processFederateFlushQueueRequest(federate_handle, timeStamp);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAenableAsynchronousDelivery"]) {
        processFederateEnableAsynchronousDelivery(federate_handle);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAdisableAsynchronousDelivery"]) {
        processFederateDisableAsynchronousDelivery(federate_handle);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAmodifyLookahead"]) {
        int lookahead;
        processFederateModifyLookahead(federate_handle, lookahead);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAchangeAttributeOrderType"]) {
        ObjectHandle objectInstance;
        std::vector<AttributeHandle> attributeList;
        OrderType sendOrder;
        processFederateChangeAttributeOrderType(federate_handle, objectInstance, attributeList, sendOrder);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAchangeInteractionOrderType"]) {
        InteractionClassHandle interactionClass;
        OrderType sendOrder;
        processFederateChangeInteractionOrderType(federate_handle, interactionClass, sendOrder);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederation.HLAadjust.HLAsetSwitches"]) {
        bool autoProvide;
        processFederationSetSwitches(autoProvide);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederation.HLArequest.HLArequestSynchronizationPoints"]) {
        processFederationRequestSynchronizationPoints();
    }
    else if (interaction_class_handle
             == my_interaction_class_cache
                    ["HLAmanager.HLAfederation.HLArequest.HLArequestSynchronizationPointStatus"]) {
        std::string syncPointName;
        processFederationRequestSynchronizationPointStatus(syncPointName);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederation.HLArequest.HLArequestFOMmoduleData"]) {
        int FOMmoduleIndicator;
        processFederationRequestFOMmoduleData(FOMmoduleIndicator);
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederation.HLArequest.HLArequestMIMData"]) {
        processFederationRequestMIMData();
    }
    else {
        Debug(D, pdGendoc) << "Unknown interaction" << endl;
    }
    Debug(D, pdGendoc) << "exit  Mom::processInteraction" << endl;
}

void Mom::processFederateSetTiming(const FederateHandle& federate_handle, const int reportPeriod)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateSetTiming" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateSetTiming" << endl;
}

void Mom::processFederateModifyAttributeState(const FederateHandle& federate_handle,
                                              const ObjectHandle objectInstance,
                                              const AttributeHandle attribute,
                                              const bool HLAattributeState)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateModifyAttributeState" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateModifyAttributeState" << endl;
}

void Mom::processFederateSetSwitches(const FederateHandle& federate_handle,
                                     const bool conveyRegionDesignatorSets,
                                     const bool conveyProducingFederate,
                                     const bool serviceReporting,
                                     const bool exceptionReporting)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateSetSwitches" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateSetSwitches" << endl;
}

void Mom::processFederateRequestPublications(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestPublications" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestPublications" << endl;
}

void Mom::processFederateRequestSubscriptions(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestSubscriptions" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestSubscriptions" << endl;
}

void Mom::processFederateRequestObjectInstancesThatCanBeDeleted(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestObjectInstancesThatCanBeDeleted" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestObjectInstancesThatCanBeDeleted" << endl;
}

void Mom::processFederateRequestObjectInstancesUpdated(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestObjectInstancesUpdated" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestObjectInstancesUpdated" << endl;
}

void Mom::processFederateRequestObjectInstancesReflected(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestObjectInstancesReflected" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestObjectInstancesReflected" << endl;
}

void Mom::processFederateRequestUpdatesSent(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestUpdatesSent" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestUpdatesSent" << endl;
}

void Mom::processFederateRequestInteractionsSent(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestInteractionsSent" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestInteractionsSent" << endl;
}

void Mom::processFederateRequestReflectionsReceived(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestReflectionsReceived" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestReflectionsReceived" << endl;
}

void Mom::processFederateRequestInteractionsReceived(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestInteractionsReceived" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestInteractionsReceived" << endl;
}

void Mom::processFederateRequestObjectInstanceInformation(const FederateHandle& federate_handle,
                                                          const ObjectHandle& objectInstance)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestObjectInstanceInformation" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestObjectInstanceInformation" << endl;
}

void Mom::processFederateRequestFOMmoduleData(const FederateHandle& federate_handle, const int FOMmoduleIndicator)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestFOMmoduleData" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestFOMmoduleData" << endl;
}

void Mom::processFederateResignFederationExecution(const FederateHandle& federate_handle,
                                                   const ResignAction resignAction)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateResignFederationExecution" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateResignFederationExecution" << endl;
}

void Mom::processFederateSynchronizationPointAchieved(const FederateHandle& federate_handle, const std::string& label)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateSynchronizationPointAchieved" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateSynchronizationPointAchieved" << endl;
}

void Mom::processFederateFederateSaveBegun(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateFederateSaveBegun" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateFederateSaveBegun" << endl;
}

void Mom::processFederateFederateSaveComplete(const FederateHandle& federate_handle, const bool successIndicator)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateFederateSaveComplete" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateFederateSaveComplete" << endl;
}

void Mom::processFederateFederateRestoreComplete(const FederateHandle& federate_handle, const bool successIndicator)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateFederateRestoreComplete" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateFederateRestoreComplete" << endl;
}

void Mom::processFederatePublishObjectClassAttributes(const FederateHandle& federate_handle,
                                                      const ObjectClassHandle& objectClass,
                                                      const std::vector<AttributeHandle>& attributeList)
{
    Debug(D, pdGendoc) << "enter Mom::processFederatePublishObjectClassAttributes" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederatePublishObjectClassAttributes" << endl;
}

void Mom::processFederateUnpublishObjectClassAttributes(const FederateHandle& federate_handle,
                                                        const ObjectClassHandle& objectClass,
                                                        const std::vector<AttributeHandle>& attributeList)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateUnpublishObjectClassAttributes" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateUnpublishObjectClassAttributes" << endl;
}

void Mom::processFederatePublishInteractionClass(const FederateHandle& federate_handle,
                                                 const InteractionClassHandle& interactionClass)
{
    Debug(D, pdGendoc) << "enter Mom::processFederatePublishInteractionClass" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederatePublishInteractionClass" << endl;
}

void Mom::processFederateUnpublishInteractionClass(const FederateHandle& federate_handle,
                                                   const InteractionClassHandle& interactionClass)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateUnpublishInteractionClass" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateUnpublishInteractionClass" << endl;
}

void Mom::processFederateSubscribeObjectClassAttributes(const FederateHandle& federate_handle,
                                                        const ObjectClassHandle& objectClass,
                                                        const std::vector<AttributeHandle>& attributeList,
                                                        const bool active)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateSubscribeObjectClassAttributes" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateSubscribeObjectClassAttributes" << endl;
}

void Mom::processFederateUnsubscribeObjectClassAttributes(const FederateHandle& federate_handle,
                                                          const ObjectClassHandle& objectClass,
                                                          const std::vector<AttributeHandle>& attributeList)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateUnsubscribeObjectClassAttributes" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateUnsubscribeObjectClassAttributes" << endl;
}

void Mom::processFederateSubscribeInteractionClass(const FederateHandle& federate_handle,
                                                   const InteractionClassHandle& interactionClass,
                                                   const bool active)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateSubscribeInteractionClass" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateSubscribeInteractionClass" << endl;
}

void Mom::processFederateUnsubscribeInteractionClass(const FederateHandle& federate_handle,
                                                     const InteractionClassHandle& interactionClass)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateUnsubscribeInteractionClass" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateUnsubscribeInteractionClass" << endl;
}

void Mom::processFederateDeleteObjectInstance(const FederateHandle& federate_handle,
                                              const ObjectHandle& objectInstance,
                                              const std::string& tag,
                                              const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateDeleteObjectInstance" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateDeleteObjectInstance" << endl;
}

void Mom::processFederateLocalDeleteObjectInstance(const FederateHandle& federate_handle,
                                                   const ObjectHandle& objectInstance)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateLocalDeleteObjectInstance" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateLocalDeleteObjectInstance" << endl;
}

void Mom::processFederateRequestAttributeTransportationTypeChange(const FederateHandle& federate_handle,
                                                                  const ObjectHandle& objectInstance,
                                                                  const std::vector<AttributeHandle>& attributeList,
                                                                  const std::string& transportation)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestAttributeTransportationTypeChange" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestAttributeTransportationTypeChange" << endl;
}

void Mom::processFederateRequestInteractionTransportationTypeChange(const FederateHandle& federate_handle,
                                                                    const InteractionClassHandle& interactionClass,
                                                                    const std::string& transportation)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestInteractionTransportationTypeChange" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestInteractionTransportationTypeChange" << endl;
}

void Mom::processFederateUnconditionalAttributeOwnershipDivestiture(const FederateHandle& federate_handle,
                                                                    const ObjectHandle& objectInstance,
                                                                    const std::vector<AttributeHandle>& attributeList)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateUnconditionalAttributeOwnershipDivestiture" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateUnconditionalAttributeOwnershipDivestiture" << endl;
}

void Mom::processFederateEnableTimeRegulation(const FederateHandle& federate_handle, const int lookahead)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateEnableTimeRegulation" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateEnableTimeRegulation" << endl;
}

void Mom::processFederateDisableTimeRegulation(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateDisableTimeRegulation" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateDisableTimeRegulation" << endl;
}

void Mom::processFederateEnableTimeConstrained(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateEnableTimeConstrained" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateEnableTimeConstrained" << endl;
}

void Mom::processFederateDisableTimeConstrained(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateDisableTimeConstrained" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateDisableTimeConstrained" << endl;
}

void Mom::processFederateTimeAdvanceRequest(const FederateHandle& federate_handle, const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateTimeAdvanceRequest" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateTimeAdvanceRequest" << endl;
}

void Mom::processFederateTimeAdvanceRequestAvailable(const FederateHandle& federate_handle,
                                                     const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateTimeAdvanceRequestAvailable" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateTimeAdvanceRequestAvailable" << endl;
}

void Mom::processFederateNextMessageRequest(const FederateHandle& federate_handle, const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateNextMessageRequest" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateNextMessageRequest" << endl;
}

void Mom::processFederateNextMessageRequestAvailable(const FederateHandle& federate_handle,
                                                     const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateNextMessageRequestAvailable" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateNextMessageRequestAvailable" << endl;
}

void Mom::processFederateFlushQueueRequest(const FederateHandle& federate_handle, const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateFlushQueueRequest" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateFlushQueueRequest" << endl;
}

void Mom::processFederateEnableAsynchronousDelivery(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateEnableAsynchronousDelivery" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateEnableAsynchronousDelivery" << endl;
}

void Mom::processFederateDisableAsynchronousDelivery(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateDisableAsynchronousDelivery" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateDisableAsynchronousDelivery" << endl;
}

void Mom::processFederateModifyLookahead(const FederateHandle& federate_handle, const int lookahead)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateModifyLookahead" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateModifyLookahead" << endl;
}

void Mom::processFederateChangeAttributeOrderType(const FederateHandle& federate_handle,
                                                  const ObjectHandle& objectInstance,
                                                  const std::vector<AttributeHandle>& attributeList,
                                                  const OrderType& sendOrder)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateChangeAttributeOrderType" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateChangeAttributeOrderType" << endl;
}

void Mom::processFederateChangeInteractionOrderType(const FederateHandle& federate_handle,
                                                    const InteractionClassHandle& interactionClass,
                                                    const OrderType& sendOrder)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateChangeInteractionOrderType" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederateChangeInteractionOrderType" << endl;
}

void Mom::processFederationSetSwitches(const bool autoProvide)
{
    Debug(D, pdGendoc) << "enter Mom::processFederationSetSwitches" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederationSetSwitches" << endl;
}

void Mom::processFederationRequestSynchronizationPoints()
{
    Debug(D, pdGendoc) << "enter Mom::processFederationRequestSynchronizationPoints" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederationRequestSynchronizationPoints" << endl;
}

void Mom::processFederationRequestSynchronizationPointStatus(const std::string& syncPointName)
{
    Debug(D, pdGendoc) << "enter Mom::processFederationRequestSynchronizationPointStatus" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederationRequestSynchronizationPointStatus" << endl;
}

void Mom::processFederationRequestFOMmoduleData(const int FOMmoduleIndicator)
{
    Debug(D, pdGendoc) << "enter Mom::processFederationRequestFOMmoduleData" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederationRequestFOMmoduleData" << endl;
}

void Mom::processFederationRequestMIMData()
{
    Debug(D, pdGendoc) << "enter Mom::processFederationRequestMIMData" << endl;
    Debug(D, pdGendoc) << "exit  Mom::processFederationRequestMIMData" << endl;
}
}
}
