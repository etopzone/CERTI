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

#include "InteractionSet.hh"

using std::cout;
using std::endl;

namespace certi {
namespace rtig {

static PrettyDebug D("MOM", __FILE__);

void Mom::processInteraction(/*const FederateHandle federate_handle,*/
                             const InteractionClassHandle interaction_class_handle,
                             const std::vector<ParameterHandle>& parameter_handles,
                             const std::vector<ParameterValue_t>& parameter_values,
                             const RegionHandle region_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processInteraction" << endl;

    Debug(D, pdGendoc) << interaction_class_handle << endl;
    Debug(D, pdGendoc) << parameter_handles.size() << endl;
    Debug(D, pdGendoc) << region_handle << endl;

    std::map<ParameterHandle, ParameterValue_t> parameters;
    for (auto i(0u); i < parameter_handles.size(); ++i) {
        parameters[parameter_handles[i]] = parameter_values[i];
#if 0
        std::cout << my_root.Interactions->getParameterName(parameter_handles[i], interaction_class_handle)
                  << " == " << parameter_values[i].size() << "{ ";
        for (const auto& byte : parameter_values[i]) {
            std::cout << byte << "(" << static_cast<int>(byte) << ") ";
        }
        std::cout << "}" << std::endl;
#endif
    }

    if (interaction_class_handle == my_interaction_class_cache["HLAmanager.HLAfederate.HLAadjust.HLAsetTiming"]) {
        processFederateSetTiming(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAreportPeriod")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAadjust.HLAmodifyAttributeState"]) {
        processFederateModifyAttributeState(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectInstance")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAattribute")]),
            decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAattributeState")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAadjust.HLAsetSwitches"]) {
        processFederateSetSwitches(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAconveyRegionDesignatorSets")]),
            decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAconveyProducingFederate")]),
            decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAserviceReporting")]),
            decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAexceptionReporting")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestPublications"]) {
        processFederateRequestPublications(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestSubscriptions"]) {
        processFederateRequestSubscriptions(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache
                    ["HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesThatCanBeDeleted"]) {
        processFederateRequestObjectInstancesThatCanBeDeleted(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesUpdated"]) {
        processFederateRequestObjectInstancesUpdated(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesReflected"]) {
        processFederateRequestObjectInstancesReflected(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestUpdatesSent"]) {
        processFederateRequestUpdatesSent(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestInteractionsSent"]) {
        processFederateRequestInteractionsSent(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestReflectionsReceived"]) {
        processFederateRequestReflectionsReceived(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestInteractionsReceived"]) {
        processFederateRequestInteractionsReceived(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstanceInformation"]) {
        processFederateRequestObjectInstanceInformation(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]), decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectInstance")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestFOMmoduleData"]) {
        processFederateRequestFOMmoduleData(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]), decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAFOMmoduleIndicator")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAresignFederationExecution"]) {
        processFederateResignFederationExecution(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]), 
                                                 decodeResignAction(parameters[getParameterHandle(interaction_class_handle, "HLAresignAction")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAsynchronizationPointAchieved"]) {
        processFederateSynchronizationPointAchieved(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]), decodeString(parameters[getParameterHandle(interaction_class_handle, "HLAlabel")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAfederateSaveBegun"]) {
        processFederateFederateSaveBegun(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAfederateSaveComplete"]) {
        processFederateFederateSaveComplete(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]), decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAsuccessIndicator")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAfederateRestoreComplete"]) {
        processFederateFederateRestoreComplete(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]), decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAsuccessIndicator")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLApublishObjectClassAttributes"]) {
        processFederatePublishObjectClassAttributes(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectClass")]),
            decodeVectorAttributeHandle(parameters[getParameterHandle(interaction_class_handle, "HLAattributeList")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAunpublishObjectClassAttributes"]) {
        processFederateUnpublishObjectClassAttributes(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectClass")]),
            decodeVectorAttributeHandle(parameters[getParameterHandle(interaction_class_handle, "HLAattributeList")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLApublishInteractionClass"]) {
        processFederatePublishInteractionClass(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]), decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAinteractionClass")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAunpublishInteractionClass"]) {
        processFederateUnpublishInteractionClass(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]), decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAinteractionClass")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAsubscribeObjectClassAttributes"]) {
        processFederateSubscribeObjectClassAttributes(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectClass")]),
            decodeVectorAttributeHandle(parameters[getParameterHandle(interaction_class_handle, "HLAattributeList")]),
            decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAactive")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAunsubscribeObjectClassAttributes"]) {
        processFederateUnsubscribeObjectClassAttributes(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectClass")]),
            decodeVectorAttributeHandle(parameters[getParameterHandle(interaction_class_handle, "HLAattributeList")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAsubscribeInteractionClass"]) {
        processFederateSubscribeInteractionClass(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAinteractionClass")]),
            decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAactive")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAunsubscribeInteractionClass"]) {
        processFederateUnsubscribeInteractionClass(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]), decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAinteractionClass")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAdeleteObjectInstance"]) {
        processFederateDeleteObjectInstance(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectInstance")]),
            decodeString(parameters[getParameterHandle(interaction_class_handle, "HLAtag")]),
            decodeFederationTime(parameters[getParameterHandle(interaction_class_handle, "HLAtimeStamp")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAlocalDeleteObjectInstance"]) {
        processFederateLocalDeleteObjectInstance(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]), decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectInstance")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache
                    ["HLAmanager.HLAfederate.HLAservice.HLArequestAttributeTransportationTypeChange"]) {
        processFederateRequestAttributeTransportationTypeChange(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectInstance")]),
            decodeVectorAttributeHandle(parameters[getParameterHandle(interaction_class_handle, "HLAattributeList")]),
            decodeString(parameters[getParameterHandle(interaction_class_handle, "HLAtransportation")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache
                    ["HLAmanager.HLAfederate.HLAservice.HLArequestInteractionTransportationTypeChange"]) {
        processFederateRequestInteractionTransportationTypeChange(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAinteractionClass")]),
            decodeString(parameters[getParameterHandle(interaction_class_handle, "HLAtransportation")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache
                    ["HLAmanager.HLAfederate.HLAservice.HLAunconditionalAttributeOwnershipDivestiture"]) {
        processFederateUnconditionalAttributeOwnershipDivestiture(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectInstance")]),
            decodeVectorAttributeHandle(parameters[getParameterHandle(interaction_class_handle, "HLAattributeList")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAenableTimeRegulation"]) {
        processFederateEnableTimeRegulation(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]), decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAlookahead")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAdisableTimeRegulation"]) {
        processFederateDisableTimeRegulation(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAenableTimeConstrained"]) {
        processFederateEnableTimeConstrained(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAdisableTimeConstrained"]) {
        processFederateDisableTimeConstrained(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAtimeAdvanceRequest"]) {
        processFederateTimeAdvanceRequest(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]), 
                                          decodeFederationTime(parameters[getParameterHandle(interaction_class_handle, "HLAtimeStamp")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAtimeAdvanceRequestAvailable"]) {
        processFederateTimeAdvanceRequestAvailable(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]), decodeFederationTime(parameters[getParameterHandle(interaction_class_handle, "HLAtimeStamp")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAnextMessageRequest"]) {
        processFederateNextMessageRequest(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]), decodeFederationTime(parameters[getParameterHandle(interaction_class_handle, "HLAtimeStamp")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAnextMessageRequestAvailable"]) {
        processFederateNextMessageRequestAvailable(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]), decodeFederationTime(parameters[getParameterHandle(interaction_class_handle, "HLAtimeStamp")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAflushQueueRequest"]) {
        processFederateFlushQueueRequest(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]), decodeFederationTime(parameters[getParameterHandle(interaction_class_handle, "HLAtimeStamp")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAenableAsynchronousDelivery"]) {
        processFederateEnableAsynchronousDelivery(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAdisableAsynchronousDelivery"]) {
        processFederateDisableAsynchronousDelivery(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAmodifyLookahead"]) {
        processFederateModifyLookahead(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]), decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAlookahead")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAchangeAttributeOrderType"]) {
        processFederateChangeAttributeOrderType(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectInstance")]),
            decodeVectorAttributeHandle(parameters[getParameterHandle(interaction_class_handle, "HLAattributeList")]),
            decodeOrderType(parameters[getParameterHandle(interaction_class_handle, "HLAsendOrder")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAchangeInteractionOrderType"]) {
        processFederateChangeInteractionOrderType(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAinteractionClass")]),
            decodeOrderType(parameters[getParameterHandle(interaction_class_handle, "HLAsendOrder")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederation.HLAadjust.HLAsetSwitches"]) {
        processFederationSetSwitches(decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAautoProvide")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederation.HLArequest.HLArequestSynchronizationPoints"]) {
        processFederationRequestSynchronizationPoints();
    }
    else if (interaction_class_handle
             == my_interaction_class_cache
                    ["HLAmanager.HLAfederation.HLArequest.HLArequestSynchronizationPointStatus"]) {
        processFederationRequestSynchronizationPointStatus(decodeString(parameters[getParameterHandle(interaction_class_handle, "HLAsyncPointName")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederation.HLArequest.HLArequestFOMmoduleData"]) {
        processFederationRequestFOMmoduleData(decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAFOMmoduleIndicator")]));
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
    std::cout << federate_handle << endl;
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

ParameterHandle Mom::getParameterHandle(const InteractionClassHandle interaction, const std::string& name)
{
    try {
        return my_parameter_cache.at(name);
    }
    catch (std::out_of_range& e) {
        auto handle = my_root.Interactions->getParameterHandle(name, interaction);
        my_parameter_cache.insert(std::make_pair(name, handle));
        return handle;
    }
}
}
}
