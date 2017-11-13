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
#include "MessageEvent.hh"
#include "Object.hh"
#include "ObjectClassAttribute.hh"
#include "ObjectClassSet.hh"
#include "ObjectSet.hh"

using std::cout;
using std::endl;

namespace certi {
namespace rtig {

#if 0
    
    // We never send those reports as of today :
    
    "HLAmanager.HLAfederate.HLAreport.HLAreportException"
        "HLAservice"
        "HLAexception"
    
    "HLAmanager.HLAfederate.HLAreport.HLAreportServiceInvocation"
        "HLAservice"
        "HLAsuccessIndicator"
        "HLAsuppliedArguments"
        "HLAreturnedArguments"
        "HLAexception"
        "HLAserialNumber"
    
    "HLAmanager.HLAfederate.HLAreport.HLAreportMOMexception"
        "HLAservice"
        "HLAexception"
        "HLAparameterError"
    
    "HLAmanager.HLAfederate.HLAreport.HLAreportFederateLost"
        "HLAfederateName"
        "HLAtimestamp"
        "HLAfaultDescription"

#endif

static PrettyDebug D("MOM", __FILE__);

Responses Mom::processInteraction(/*const FederateHandle federate_handle,*/
                                  const InteractionClassHandle interaction_class_handle,
                                  const std::vector<ParameterHandle>& parameter_handles,
                                  const std::vector<ParameterValue_t>& parameter_values,
                                  const RegionHandle region_handle)
{
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
        return processFederateSetTiming(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAreportPeriod")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAadjust.HLAmodifyAttributeState"]) {
        return processFederateModifyAttributeState(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectInstance")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAattribute")]),
            decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAattributeState")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAadjust.HLAsetSwitches"]) {
        return processFederateSetSwitches(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAconveyRegionDesignatorSets")]),
            decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAconveyProducingFederate")]),
            decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAserviceReporting")]),
            decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAexceptionReporting")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestPublications"]) {
        return processFederateRequestPublications(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestSubscriptions"]) {
        return processFederateRequestSubscriptions(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache
                    ["HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesThatCanBeDeleted"]) {
        return processFederateRequestObjectInstancesThatCanBeDeleted(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesUpdated"]) {
        return processFederateRequestObjectInstancesUpdated(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesReflected"]) {
        return processFederateRequestObjectInstancesReflected(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestUpdatesSent"]) {
        return processFederateRequestUpdatesSent(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestInteractionsSent"]) {
        return processFederateRequestInteractionsSent(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestReflectionsReceived"]) {
        return processFederateRequestReflectionsReceived(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestInteractionsReceived"]) {
        return processFederateRequestInteractionsReceived(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstanceInformation"]) {
        return processFederateRequestObjectInstanceInformation(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectInstance")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLArequest.HLArequestFOMmoduleData"]) {
        return processFederateRequestFOMmoduleData(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAFOMmoduleIndicator")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAresignFederationExecution"]) {
        return processFederateResignFederationExecution(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeResignAction(parameters[getParameterHandle(interaction_class_handle, "HLAresignAction")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAsynchronizationPointAchieved"]) {
        return processFederateSynchronizationPointAchieved(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeString(parameters[getParameterHandle(interaction_class_handle, "HLAlabel")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAfederateSaveBegun"]) {
        return processFederateFederateSaveBegun(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAfederateSaveComplete"]) {
        return processFederateFederateSaveComplete(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAsuccessIndicator")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAfederateRestoreComplete"]) {
        return processFederateFederateRestoreComplete(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAsuccessIndicator")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLApublishObjectClassAttributes"]) {
        return processFederatePublishObjectClassAttributes(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectClass")]),
            decodeVectorAttributeHandle(parameters[getParameterHandle(interaction_class_handle, "HLAattributeList")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAunpublishObjectClassAttributes"]) {
        return processFederateUnpublishObjectClassAttributes(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectClass")]),
            decodeVectorAttributeHandle(parameters[getParameterHandle(interaction_class_handle, "HLAattributeList")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLApublishInteractionClass"]) {
        return processFederatePublishInteractionClass(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAinteractionClass")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAunpublishInteractionClass"]) {
        return processFederateUnpublishInteractionClass(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAinteractionClass")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAsubscribeObjectClassAttributes"]) {
        return processFederateSubscribeObjectClassAttributes(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectClass")]),
            decodeVectorAttributeHandle(parameters[getParameterHandle(interaction_class_handle, "HLAattributeList")]),
            decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAactive")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAunsubscribeObjectClassAttributes"]) {
        return processFederateUnsubscribeObjectClassAttributes(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectClass")]),
            decodeVectorAttributeHandle(parameters[getParameterHandle(interaction_class_handle, "HLAattributeList")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAsubscribeInteractionClass"]) {
        return processFederateSubscribeInteractionClass(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAinteractionClass")]),
            decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAactive")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAunsubscribeInteractionClass"]) {
        return processFederateUnsubscribeInteractionClass(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAinteractionClass")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAdeleteObjectInstance"]) {
        return processFederateDeleteObjectInstance(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectInstance")]),
            decodeString(parameters[getParameterHandle(interaction_class_handle, "HLAtag")]),
            decodeFederationTime(parameters[getParameterHandle(interaction_class_handle, "HLAtimeStamp")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAlocalDeleteObjectInstance"]) {
        return processFederateLocalDeleteObjectInstance(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectInstance")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache
                    ["HLAmanager.HLAfederate.HLAservice.HLArequestAttributeTransportationTypeChange"]) {
        return processFederateRequestAttributeTransportationTypeChange(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectInstance")]),
            decodeVectorAttributeHandle(parameters[getParameterHandle(interaction_class_handle, "HLAattributeList")]),
            decodeString(parameters[getParameterHandle(interaction_class_handle, "HLAtransportation")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache
                    ["HLAmanager.HLAfederate.HLAservice.HLArequestInteractionTransportationTypeChange"]) {
        return processFederateRequestInteractionTransportationTypeChange(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAinteractionClass")]),
            decodeString(parameters[getParameterHandle(interaction_class_handle, "HLAtransportation")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache
                    ["HLAmanager.HLAfederate.HLAservice.HLAunconditionalAttributeOwnershipDivestiture"]) {
        return processFederateUnconditionalAttributeOwnershipDivestiture(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectInstance")]),
            decodeVectorAttributeHandle(parameters[getParameterHandle(interaction_class_handle, "HLAattributeList")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAenableTimeRegulation"]) {
        return processFederateEnableTimeRegulation(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAlookahead")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAdisableTimeRegulation"]) {
        return processFederateDisableTimeRegulation(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAenableTimeConstrained"]) {
        return processFederateEnableTimeConstrained(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAdisableTimeConstrained"]) {
        return processFederateDisableTimeConstrained(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAtimeAdvanceRequest"]) {
        return processFederateTimeAdvanceRequest(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeFederationTime(parameters[getParameterHandle(interaction_class_handle, "HLAtimeStamp")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAtimeAdvanceRequestAvailable"]) {
        return processFederateTimeAdvanceRequestAvailable(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeFederationTime(parameters[getParameterHandle(interaction_class_handle, "HLAtimeStamp")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAnextMessageRequest"]) {
        return processFederateNextMessageRequest(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeFederationTime(parameters[getParameterHandle(interaction_class_handle, "HLAtimeStamp")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAnextMessageRequestAvailable"]) {
        return processFederateNextMessageRequestAvailable(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeFederationTime(parameters[getParameterHandle(interaction_class_handle, "HLAtimeStamp")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAflushQueueRequest"]) {
        return processFederateFlushQueueRequest(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeFederationTime(parameters[getParameterHandle(interaction_class_handle, "HLAtimeStamp")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAenableAsynchronousDelivery"]) {
        return processFederateEnableAsynchronousDelivery(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAdisableAsynchronousDelivery"]) {
        return processFederateDisableAsynchronousDelivery(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAmodifyLookahead"]) {
        return processFederateModifyLookahead(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAlookahead")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAchangeAttributeOrderType"]) {
        return processFederateChangeAttributeOrderType(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAobjectInstance")]),
            decodeVectorAttributeHandle(parameters[getParameterHandle(interaction_class_handle, "HLAattributeList")]),
            decodeOrderType(parameters[getParameterHandle(interaction_class_handle, "HLAsendOrder")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederate.HLAservice.HLAchangeInteractionOrderType"]) {
        return processFederateChangeInteractionOrderType(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAfederate")]),
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAinteractionClass")]),
            decodeOrderType(parameters[getParameterHandle(interaction_class_handle, "HLAsendOrder")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederation.HLAadjust.HLAsetSwitches"]) {
        return processFederationSetSwitches(
            decodeBoolean(parameters[getParameterHandle(interaction_class_handle, "HLAautoProvide")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederation.HLArequest.HLArequestSynchronizationPoints"]) {
        return processFederationRequestSynchronizationPoints();
    }
    else if (interaction_class_handle
             == my_interaction_class_cache
                    ["HLAmanager.HLAfederation.HLArequest.HLArequestSynchronizationPointStatus"]) {
        return processFederationRequestSynchronizationPointStatus(
            decodeString(parameters[getParameterHandle(interaction_class_handle, "HLAsyncPointName")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederation.HLArequest.HLArequestFOMmoduleData"]) {
        return processFederationRequestFOMmoduleData(
            decodeUInt32(parameters[getParameterHandle(interaction_class_handle, "HLAFOMmoduleIndicator")]));
    }
    else if (interaction_class_handle
             == my_interaction_class_cache["HLAmanager.HLAfederation.HLArequest.HLArequestMIMData"]) {
        return processFederationRequestMIMData();
    }
    else {
        Debug(D, pdGendoc) << "Unknown interaction" << endl;
        return {};
    }
}

Responses Mom::processFederateSetTiming(const FederateHandle& federate_handle, const int reportPeriod)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateSetTiming " << federate_handle << ", " << reportPeriod << endl;

    // TODO

    return {};

    Debug(D, pdGendoc) << "exit  Mom::processFederateSetTiming" << endl;
}

Responses Mom::processFederateModifyAttributeState(const FederateHandle& federate_handle,
                                                   const ObjectHandle objectInstance,
                                                   const AttributeHandle attribute,
                                                   const bool attributeState)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateModifyAttributeState " << federate_handle << ", " << objectInstance
                       << ", " << attribute << ", " << attributeState << endl;

    // TODO

    return {};

    Debug(D, pdGendoc) << "exit  Mom::processFederateModifyAttributeState" << endl;
}

Responses Mom::processFederateSetSwitches(const FederateHandle& federate_handle,
                                          const bool conveyRegionDesignatorSets,
                                          const bool conveyProducingFederate,
                                          const bool serviceReporting,
                                          const bool exceptionReporting)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateSetSwitches " << federate_handle << ", "
                       << conveyRegionDesignatorSets << ", " << conveyProducingFederate << ", " << serviceReporting
                       << ", " << exceptionReporting << endl;

    // TODO

    return {};

    Debug(D, pdGendoc) << "exit  Mom::processFederateSetSwitches" << endl;
}

/** Request that the RTI send report interactions that contain the publication data of a joined federate.
 *
 * It shall result in one interaction of class HLAmanager.HLAfederate.HLAreport.HLAreportInteractionPublication
 * and one interaction of class HLAmanager.HLAfederate.HLAreport.HLAreportObjectClassPublication for
 * each object class published. If the joined federate is published to no object classes, then one of class
 * HLAmanager.HLAfederate.HLAreport.HLAreportObjectClassPublication shall be sent as a NULL
 * response with the HLAobjectClassCount parameter having a value of 0.
 */
Responses Mom::processFederateRequestPublications(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestPublications " << federate_handle << endl;

    Responses responses;

    /* The interaction shall be sent by the RTI in response to an interaction of class
     * HLAmanager.HLAfederate.HLArequest.HLArequestPublications. It shall report the attributes of one
     * object class published by the joined federate. One of these interactions shall be sent for each object class
     * containing attributes that are published by the joined federate. If the joined federate is published to no
     * object classes, then a single interaction shall be sent as a NULL response with the HLAobjectClassCount
     * parameter having a value of 0.
     */
    std::map<ObjectClassHandle, std::vector<AttributeHandle>> publishingAttributes;

    for (const auto& pair : *my_root.ObjectClasses) {
        if (pair.second->isFederatePublisher(federate_handle)) {
            for (const auto& attributePair : pair.second->getHandleClassAttributeMap()) {
                if (attributePair.second->isPublishing(federate_handle)) {
                    publishingAttributes[pair.second->getHandle()].push_back(attributePair.first);
                }
            }
        }
    }

    if (publishingAttributes.size() == 0) {
        auto interaction_handle = my_root.Interactions->getHandleFromName(
            "HLAmanager.HLAfederate.HLAreport.HLAreportObjectClassPublication");

        std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                                getParameterHandle(interaction_handle, "HLAnumberOfClasses")};

        std::vector<AttributeValue_t> values{encodeUInt32(federate_handle), encodeUInt32(0)};

        auto resp = my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
        responses.insert(end(responses), make_move_iterator(begin(resp)), make_move_iterator(end(resp)));
    }
    else {
        for (const auto& pair : publishingAttributes) {
            auto interaction_handle = my_root.Interactions->getHandleFromName(
                "HLAmanager.HLAfederate.HLAreport.HLAreportObjectClassPublication");

            std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                                    getParameterHandle(interaction_handle, "HLAnumberOfClasses"),
                                                    getParameterHandle(interaction_handle, "HLAobjectClass"),
                                                    getParameterHandle(interaction_handle, "HLAattributeList")};

            std::vector<AttributeValue_t> values{encodeUInt32(federate_handle),
                                                 encodeUInt32(publishingAttributes.size()),
                                                 encodeUInt32(pair.first),
                                                 encodeVectorHandle(pair.second)};

            auto resp = my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
            responses.insert(end(responses), make_move_iterator(begin(resp)), make_move_iterator(end(resp)));
        }
    }

    /** The interaction shall be sent by the RTI in response to an interaction of class
     * HLAmanager.HLAfederate.HLAfequest.HLArequestPublications. It shall report the interaction classes
     * published by the joined federate. If the joined federate is published to no interaction classes, then a single
     * interaction shall be sent as a NULL response with the HLAinteractionClassList parameter having an
     * undefined value (i.e., 0 length array).
     */
    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederate.HLAreport.HLAreportInteractionPublication");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAinteractionClassList")};

    std::vector<InteractionClassHandle> interactionClassList;
    for (const auto& pair : *my_root.Interactions) {
        if (pair.second->isPublishing(federate_handle)) {
            interactionClassList.push_back(pair.second->getHandle());
        }
    }
    std::vector<AttributeValue_t> values{encodeUInt32(federate_handle), encodeVectorHandle(interactionClassList)};

    auto resp = my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
    responses.insert(end(responses), make_move_iterator(begin(resp)), make_move_iterator(end(resp)));

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestPublications " << federate_handle << endl;

    return responses;
}

/** Request that the RTI send report interactions that contain the subscription data of a joined federate.
 *
 * It shall result in one interaction of class
 * HLAmanager.HLAfederate.HLAreport.HLAreportInteractionSubscription and one interaction of class
 * HLAmanager.HLAfederate.HLAreport.HLAreportObjectClassSubscription for each different
 * combination of (object class, passive subscription indicator) values that are subscribed. If the joined
 * federate is subscribed to no object classes, then one interaction of class
 * HLAmanager.HLAfederate.HLAreport.HLAreportObjectClassSubscription shall be sent as a NULL
 * response with the HLAobjectClassCount parameter having a value of 0.
 */
Responses Mom::processFederateRequestSubscriptions(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestSubscriptions " << federate_handle << endl;
    
    Responses responses;

    /** The interaction shall be sent by the RTI in response to an interaction of class HLAmanager.HLAfederate.HLArequest.HLArequestSubscriptions.
     * It shall report the attributes of one object class subscribed to
     * by the joined federate. One of these interactions shall be sent for each combination of object class and
     * passive subscription indicator subscribed to by the joined federate. This information shall reflect related
     * DDM usage. If the joined federate has no subscribed object classes, then a single interaction shall be sent
     * as a NULL response with the HLAnumberOfClasses parameter having a value of 0.
     */
    std::map<ObjectClassHandle, std::vector<AttributeHandle>> publishingAttributes;

    for (const auto& pair : *my_root.ObjectClasses) {
        if (pair.second->isSubscribed(federate_handle)) {
            for (const auto& attributePair : pair.second->getHandleClassAttributeMap()) {
                if (attributePair.second->isPublishing(federate_handle)) {
                    publishingAttributes[pair.second->getHandle()].push_back(attributePair.first);
                }
            }
        }
    }

    if (publishingAttributes.size() == 0) {
        auto interaction_handle = my_root.Interactions->getHandleFromName(
            "HLAmanager.HLAfederate.HLAreport.HLAreportObjectClassSubscription");

        std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                                getParameterHandle(interaction_handle, "HLAnumberOfClasses")};

        std::vector<AttributeValue_t> values{encodeUInt32(federate_handle), encodeUInt32(0)};

        auto resp = my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
        responses.insert(end(responses), make_move_iterator(begin(resp)), make_move_iterator(end(resp)));
    }
    else {
        for (const auto& pair : publishingAttributes) {
            auto interaction_handle = my_root.Interactions->getHandleFromName(
                "HLAmanager.HLAfederate.HLAreport.HLAreportObjectClassSubscription");

            std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                                    getParameterHandle(interaction_handle, "HLAnumberOfClasses"),
                                                    getParameterHandle(interaction_handle, "HLAobjectClass"),
                                                    getParameterHandle(interaction_handle, "HLAactive"),
                                                    getParameterHandle(interaction_handle, "HLAmaxUpdateRate"),
                                                    getParameterHandle(interaction_handle, "HLAattributeList")};

            std::vector<AttributeValue_t> values{encodeUInt32(federate_handle),
                                                 encodeUInt32(publishingAttributes.size()),
                                                 encodeUInt32(pair.first),
                                                 encodeBoolean(true), // not implemented
                                                 encodeString(""), // not implemented
                                                 encodeVectorHandle(pair.second)};

            auto resp = my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
            responses.insert(end(responses), make_move_iterator(begin(resp)), make_move_iterator(end(resp)));
        }
    }

    /** The interaction shall be sent by the RTI in response to an interaction of class
     * HLAmanager.HLAfederate.HLArequest.HLArequestSubscriptions.
     * It shall report the interaction classes subscribed to by the joined federate. This information shall reflect
     * related DDM usage. If the joined federate has no subscribed interaction classes, then a single interaction
     * shall be sent as a NULL response with the HLAinteractionClassList parameter having an undefined value.
     */

    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederate.HLAreport.HLAreportInteractionSubscription");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAinteractionClassList")};

    std::vector<InteractionClassHandle> interactionClassList;
    for (const auto& pair : *my_root.Interactions) {
        if (pair.second->isSubscribed(federate_handle)) {
            interactionClassList.push_back(pair.second->getHandle());
        }
    }

    std::vector<AttributeValue_t> values{encodeUInt32(federate_handle), encodeVectorHandle(interactionClassList)};

    auto resp = my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
    responses.insert(end(responses), make_move_iterator(begin(resp)), make_move_iterator(end(resp)));

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestSubscriptions" << endl;

    return responses;
}

/** Request that the RTI send a report interaction that contains the object instances that can be deleted at a
 * joined federate. It shall result in one interaction of class
 * HLAmanager.HLAfederate.HLAreport.HLAreportObjectInstancesThatCanBeDeleted.
 */
Responses Mom::processFederateRequestObjectInstancesThatCanBeDeleted(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestObjectInstancesThatCanBeDeleted " << federate_handle
                       << endl;

    /** The interaction shall be sent by the RTI in response to an interaction of class
     * HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesThatCanBeDeleted. It shall report
     * the number of object instances (by registered class of the object instances) whose
     * HLAprivilegeToDeleteObject attributes are owned by the joined federate. If the joined federate has no
     * objects that can be deleted, then a single interaction shall be sent with the HLAobjectInstanceCounts
     * parameter having an undefined value.
     */
    auto interaction_handle = my_root.Interactions->getHandleFromName(
        "HLAmanager.HLAfederate.HLAreport.HLAreportObjectInstancesThatCanBeDeleted");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAobjectInstanceCounts")};
    AttributeHandle privilegeToDelete = my_root.ObjectClasses->getAttributeHandle(
        "HLAprivilegeToDeleteObject", my_root.ObjectClasses->getHandleFromName("HLAobjectRoot"));

    std::map<ObjectClassHandle, int> objectInstancesCounts;

    std::vector<ObjectHandle> ownedObjectInstances;
    my_root.objects->getAllObjectInstancesFromFederate(federate_handle, ownedObjectInstances);
    for (const auto& object : ownedObjectInstances) {
        ++objectInstancesCounts[object];
    }

    std::vector<AttributeValue_t> values{encodeUInt32(federate_handle),
                                         encodeObjectClassBasedCounts(objectInstancesCounts)};

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestObjectInstancesThatCanBeDeleted" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

/** Request that the RTI send a report interaction that contains the object instance updating responsibility of a
 * joined federate. It shall result in one interaction of class HLAmanager.HLAfederate.HLAreport.HLAreportObjectInstancesUpdated.
 */
Responses Mom::processFederateRequestObjectInstancesUpdated(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestObjectInstancesUpdated " << federate_handle << endl;
    
    /** The interaction shall be sent by the RTI in response to an interaction of class
     * HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesUpdated. It shall report the number
     * of object instances (by registered class of the object instances) for which the joined federate has
     * successfully invoked the Update Attribute Values service. If the joined federate has no object instances
     * that are updated for a given transport type, then a single interaction shall be sent as a NULL response with
     * the HLAobjectInstanceCounts parameter having an undefined value.
     */
    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederate.HLAreport.HLAreportObjectInstancesUpdated");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAobjectInstanceCounts")};

    // TODO

    std::vector<AttributeValue_t> values;

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestObjectInstancesUpdated" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

Responses Mom::processFederateRequestObjectInstancesReflected(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestObjectInstancesReflected " << federate_handle << endl;
    
    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederate.HLAreport.HLAreportObjectInstancesReflected");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAobjectInstanceCounts")};

    // TODO

    std::vector<AttributeValue_t> values;

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestObjectInstancesReflected" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

Responses Mom::processFederateRequestUpdatesSent(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestUpdatesSent " << federate_handle << endl;

    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederate.HLAreport.HLAreportUpdatesSent");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAtransportation"),
                                            getParameterHandle(interaction_handle, "HLAupdateCounts")};

    // TODO

    std::vector<AttributeValue_t> values;

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestUpdatesSent" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

Responses Mom::processFederateRequestInteractionsSent(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestInteractionsSent " << federate_handle << endl;

    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederate.HLAreport.HLAreportInteractionsSent");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAtransportation"),
                                            getParameterHandle(interaction_handle, "HLAinteractionCounts")};

    // TODO

    std::vector<AttributeValue_t> values;

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestInteractionsSent" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

Responses Mom::processFederateRequestReflectionsReceived(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestReflectionsReceived " << federate_handle << endl;

    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederate.HLAreport.HLAreportReflectionsReceived");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAtransportation"),
                                            getParameterHandle(interaction_handle, "HLAreflectCounts")};

    // TODO

    std::vector<AttributeValue_t> values;

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestReflectionsReceived" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

Responses Mom::processFederateRequestInteractionsReceived(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestInteractionsReceived " << federate_handle << endl;

    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederate.HLAreport.HLAreportInteractionsReceived");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAtransportation"),
                                            getParameterHandle(interaction_handle, "HLAinteractionCounts")};

    // TODO

    std::vector<AttributeValue_t> values;

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestInteractionsReceived" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

Responses Mom::processFederateRequestObjectInstanceInformation(const FederateHandle& federate_handle,
                                                               const ObjectHandle& objectInstance)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestObjectInstanceInformation " << federate_handle << ", "
                       << objectInstance << endl;

    auto interaction_handle = my_root.Interactions->getHandleFromName(
        "HLAmanager.HLAfederate.HLAreport.HLAreportObjectInstanceInformation");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAobjectInstance"),
                                            getParameterHandle(interaction_handle, "HLAownedInstanceAttributeList"),
                                            getParameterHandle(interaction_handle, "HLAregisteredClass"),
                                            getParameterHandle(interaction_handle, "HLAknownClass")};

    // TODO

    std::vector<AttributeValue_t> values;

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestObjectInstanceInformation" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

Responses Mom::processFederateRequestFOMmoduleData(const FederateHandle& federate_handle, const int FOMmoduleIndicator)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestFOMmoduleData " << federate_handle << ", "
                       << FOMmoduleIndicator << endl;

    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederate.HLAreport.HLAreportFOMmoduleData");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAFOMmoduleIndicator"),
                                            getParameterHandle(interaction_handle, "HLAFOMmoduleData")};

    // TODO

    std::vector<AttributeValue_t> values;

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestFOMmoduleData" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

Responses Mom::processFederateResignFederationExecution(const FederateHandle& federate_handle,
                                                        const ResignAction resignAction)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateResignFederationExecution " << federate_handle << ", "
                       << static_cast<int>(resignAction) << endl;
    std::cout << federate_handle << endl;

    Responses responses;

    auto rep = my_federation.remove(federate_handle);

    responses.emplace_back(getSocketForFederate(federate_handle), std::move(rep));

    Debug(D, pdGendoc) << "exit  Mom::processFederateResignFederationExecution" << endl;

    return responses;
}

Responses Mom::processFederateSynchronizationPointAchieved(const FederateHandle& federate_handle,
                                                           const std::string& label)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateSynchronizationPointAchieved " << federate_handle << ", " << label
                       << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateSynchronizationPointAchieved" << endl;

    return responses;
}

Responses Mom::processFederateFederateSaveBegun(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateFederateSaveBegun " << federate_handle << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateFederateSaveBegun" << endl;

    return responses;
}

Responses Mom::processFederateFederateSaveComplete(const FederateHandle& federate_handle, const bool successIndicator)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateFederateSaveComplete " << federate_handle << ", "
                       << successIndicator << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateFederateSaveComplete" << endl;

    return responses;
}

Responses Mom::processFederateFederateRestoreComplete(const FederateHandle& federate_handle,
                                                      const bool successIndicator)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateFederateRestoreComplete " << federate_handle << ", "
                       << successIndicator << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateFederateRestoreComplete" << endl;

    return responses;
}

Responses Mom::processFederatePublishObjectClassAttributes(const FederateHandle& federate_handle,
                                                           const ObjectClassHandle& objectClass,
                                                           const std::vector<AttributeHandle>& attributeList)
{
    Debug(D, pdGendoc) << "enter Mom::processFederatePublishObjectClassAttributes " << federate_handle << ", "
                       << objectClass << ", " << attributeList.size() << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederatePublishObjectClassAttributes" << endl;

    return responses;
}

Responses Mom::processFederateUnpublishObjectClassAttributes(const FederateHandle& federate_handle,
                                                             const ObjectClassHandle& objectClass,
                                                             const std::vector<AttributeHandle>& attributeList)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateUnpublishObjectClassAttributes " << federate_handle << ", "
                       << objectClass << ", " << attributeList.size() << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateUnpublishObjectClassAttributes" << endl;

    return responses;
}

Responses Mom::processFederatePublishInteractionClass(const FederateHandle& federate_handle,
                                                      const InteractionClassHandle& interactionClass)
{
    Debug(D, pdGendoc) << "enter Mom::processFederatePublishInteractionClass " << federate_handle << ", "
                       << interactionClass << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederatePublishInteractionClass" << endl;

    return responses;
}

Responses Mom::processFederateUnpublishInteractionClass(const FederateHandle& federate_handle,
                                                        const InteractionClassHandle& interactionClass)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateUnpublishInteractionClass " << federate_handle << ", "
                       << interactionClass << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateUnpublishInteractionClass" << endl;

    return responses;
}

Responses Mom::processFederateSubscribeObjectClassAttributes(const FederateHandle& federate_handle,
                                                             const ObjectClassHandle& objectClass,
                                                             const std::vector<AttributeHandle>& attributeList,
                                                             const bool active)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateSubscribeObjectClassAttributes " << federate_handle << ", "
                       << objectClass << ", " << attributeList.size() << ", " << active << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateSubscribeObjectClassAttributes" << endl;

    return responses;
}

Responses Mom::processFederateUnsubscribeObjectClassAttributes(const FederateHandle& federate_handle,
                                                               const ObjectClassHandle& objectClass,
                                                               const std::vector<AttributeHandle>& attributeList)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateUnsubscribeObjectClassAttributes " << federate_handle << ", "
                       << objectClass << ", " << attributeList.size() << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateUnsubscribeObjectClassAttributes" << endl;

    return responses;
}

Responses Mom::processFederateSubscribeInteractionClass(const FederateHandle& federate_handle,
                                                        const InteractionClassHandle& interactionClass,
                                                        const bool active)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateSubscribeInteractionClass " << federate_handle << ", "
                       << interactionClass << ", " << active << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateSubscribeInteractionClass" << endl;

    return responses;
}

Responses Mom::processFederateUnsubscribeInteractionClass(const FederateHandle& federate_handle,
                                                          const InteractionClassHandle& interactionClass)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateUnsubscribeInteractionClass " << federate_handle << ", "
                       << interactionClass << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateUnsubscribeInteractionClass" << endl;

    return responses;
}

Responses Mom::processFederateDeleteObjectInstance(const FederateHandle& federate_handle,
                                                   const ObjectHandle& objectInstance,
                                                   const std::string& tag,
                                                   const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateDeleteObjectInstance " << federate_handle << ", " << objectInstance
                       << ", " << tag << ", " << timeStamp << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateDeleteObjectInstance" << endl;

    return responses;
}

Responses Mom::processFederateLocalDeleteObjectInstance(const FederateHandle& federate_handle,
                                                        const ObjectHandle& objectInstance)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateLocalDeleteObjectInstance " << federate_handle << ", "
                       << objectInstance << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateLocalDeleteObjectInstance" << endl;

    return responses;
}

Responses
Mom::processFederateRequestAttributeTransportationTypeChange(const FederateHandle& federate_handle,
                                                             const ObjectHandle& objectInstance,
                                                             const std::vector<AttributeHandle>& attributeList,
                                                             const std::string& transportation)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestAttributeTransportationTypeChange " << federate_handle
                       << ", " << objectInstance << ", " << attributeList.size() << ", " << transportation << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestAttributeTransportationTypeChange" << endl;

    return responses;
}

Responses Mom::processFederateRequestInteractionTransportationTypeChange(const FederateHandle& federate_handle,
                                                                         const InteractionClassHandle& interactionClass,
                                                                         const std::string& transportation)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestInteractionTransportationTypeChange " << federate_handle
                       << ", " << interactionClass << ", " << transportation << endl;

    Responses responses;

    // TODO
    
    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestInteractionTransportationTypeChange" << endl;

    return responses;
}

Responses
Mom::processFederateUnconditionalAttributeOwnershipDivestiture(const FederateHandle& federate_handle,
                                                               const ObjectHandle& objectInstance,
                                                               const std::vector<AttributeHandle>& attributeList)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateUnconditionalAttributeOwnershipDivestiture " << federate_handle
                       << ", " << objectInstance << ", " << attributeList.size() << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateUnconditionalAttributeOwnershipDivestiture" << endl;

    return responses;
}

Responses Mom::processFederateEnableTimeRegulation(const FederateHandle& federate_handle, const int lookahead)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateEnableTimeRegulation " << federate_handle << ", " << lookahead
                       << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateEnableTimeRegulation" << endl;

    return responses;
}

Responses Mom::processFederateDisableTimeRegulation(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateDisableTimeRegulation " << federate_handle << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateDisableTimeRegulation" << endl;

    return responses;
}

Responses Mom::processFederateEnableTimeConstrained(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateEnableTimeConstrained " << federate_handle << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateEnableTimeConstrained" << endl;

    return responses;
}

Responses Mom::processFederateDisableTimeConstrained(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateDisableTimeConstrained " << federate_handle << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateDisableTimeConstrained" << endl;

    return responses;
}

Responses Mom::processFederateTimeAdvanceRequest(const FederateHandle& federate_handle, const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateTimeAdvanceRequest " << federate_handle << ", " << timeStamp
                       << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateTimeAdvanceRequest" << endl;

    return responses;
}

Responses Mom::processFederateTimeAdvanceRequestAvailable(const FederateHandle& federate_handle,
                                                          const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateTimeAdvanceRequestAvailable " << federate_handle << ", "
                       << timeStamp << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateTimeAdvanceRequestAvailable" << endl;

    return responses;
}

Responses Mom::processFederateNextMessageRequest(const FederateHandle& federate_handle, const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateNextMessageRequest " << federate_handle << ", " << timeStamp
                       << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateNextMessageRequest" << endl;

    return responses;
}

Responses Mom::processFederateNextMessageRequestAvailable(const FederateHandle& federate_handle,
                                                          const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateNextMessageRequestAvailable " << federate_handle << ", "
                       << timeStamp << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateNextMessageRequestAvailable" << endl;

    return responses;
}

Responses Mom::processFederateFlushQueueRequest(const FederateHandle& federate_handle, const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateFlushQueueRequest " << federate_handle << ", " << timeStamp
                       << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateFlushQueueRequest" << endl;

    return responses;
}

Responses Mom::processFederateEnableAsynchronousDelivery(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateEnableAsynchronousDelivery " << federate_handle << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateEnableAsynchronousDelivery" << endl;

    return responses;
}

Responses Mom::processFederateDisableAsynchronousDelivery(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateDisableAsynchronousDelivery " << federate_handle << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateDisableAsynchronousDelivery" << endl;

    return responses;
}

Responses Mom::processFederateModifyLookahead(const FederateHandle& federate_handle, const int lookahead)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateModifyLookahead " << federate_handle << ", " << lookahead << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateModifyLookahead" << endl;

    return responses;
}

Responses Mom::processFederateChangeAttributeOrderType(const FederateHandle& federate_handle,
                                                       const ObjectHandle& objectInstance,
                                                       const std::vector<AttributeHandle>& attributeList,
                                                       const OrderType& sendOrder)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateChangeAttributeOrderType " << federate_handle << ", "
                       << objectInstance << ", " << attributeList.size() << ", " << static_cast<int>(sendOrder) << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateChangeAttributeOrderType" << endl;

    return responses;
}

Responses Mom::processFederateChangeInteractionOrderType(const FederateHandle& federate_handle,
                                                         const InteractionClassHandle& interactionClass,
                                                         const OrderType& sendOrder)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateChangeInteractionOrderType " << federate_handle << ", "
                       << interactionClass << ", " << static_cast<int>(sendOrder) << endl;

    Responses responses;

    // TODO

    Debug(D, pdGendoc) << "exit  Mom::processFederateChangeInteractionOrderType" << endl;

    return responses;
}

Responses Mom::processFederationSetSwitches(const bool autoProvide)
{
    Debug(D, pdGendoc) << "enter Mom::processFederationSetSwitches " << autoProvide << endl;

    my_federation.setAutoProvide(autoProvide);

    Debug(D, pdGendoc) << "exit  Mom::processFederationSetSwitches" << endl;
    
    return {};
}

Responses Mom::processFederationRequestSynchronizationPoints()
{
    Debug(D, pdGendoc) << "enter Mom::processFederationRequestSynchronizationPoints" << endl;

    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederation.HLAreport.HLAreportSynchronizationPoints");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAsyncPoints")};

    // TODO

    std::vector<AttributeValue_t> values;

    Debug(D, pdGendoc) << "exit  Mom::processFederationRequestSynchronizationPoints" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

Responses Mom::processFederationRequestSynchronizationPointStatus(const std::string& syncPointName)
{
    Debug(D, pdGendoc) << "enter Mom::processFederationRequestSynchronizationPointStatus " << syncPointName << endl;

    auto interaction_handle = my_root.Interactions->getHandleFromName(
        "HLAmanager.HLAfederation.HLAreport.HLAreportSynchronizationPointStatus");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAsyncPointName"),
                                            getParameterHandle(interaction_handle, "HLAsyncPointFederates")};

    // TODO

    std::vector<AttributeValue_t> values;

    Debug(D, pdGendoc) << "exit  Mom::processFederationRequestSynchronizationPointStatus" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

Responses Mom::processFederationRequestFOMmoduleData(const int FOMmoduleIndicator)
{
    Debug(D, pdGendoc) << "enter Mom::processFederationRequestFOMmoduleData " << FOMmoduleIndicator << endl;

    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederation.HLAreport.HLAreportFOMmoduleData");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAFOMmoduleIndicator"),
                                            getParameterHandle(interaction_handle, "HLAFOMmoduleData")};

    // TODO

    std::vector<AttributeValue_t> values;

    Debug(D, pdGendoc) << "exit  Mom::processFederationRequestFOMmoduleData" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

Responses Mom::processFederationRequestMIMData()
{
    Debug(D, pdGendoc) << "enter Mom::processFederationRequestMIMData" << endl;

    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederation.HLAreport.HLAreportMIMData");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAMIMData")};

    // TODO

    std::vector<AttributeValue_t> values;

    Debug(D, pdGendoc) << "exit  Mom::processFederationRequestMIMData" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
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

Socket* Mom::getSocketForFederate(const FederateHandle& federate_handle)
{
    return my_federation.my_server->getSocketLink(federate_handle);
}
}
}
