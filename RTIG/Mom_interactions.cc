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

#include "Mom.hh"

#include <libCERTI/PrettyDebug.hh>

#include <libCERTI/InteractionSet.hh>
#include <libCERTI/MessageEvent.hh>
#include <libCERTI/NM_Classes.hh>
#include <libCERTI/Object.hh>
#include <libCERTI/ObjectAttribute.hh>
#include <libCERTI/ObjectClassAttribute.hh>
#include <libCERTI/ObjectClassSet.hh>
#include <libCERTI/ObjectSet.hh>

#include <include/make_unique.hh>

using std::cout;
using std::endl;

namespace certi {
namespace rtig {

#if 0
    
    // FIXME We never send those reports as of today :
    
    /** The interaction shall be sent by the RTI when an exception occurs as the result of a service invocation at
     * the indicated joined federate. This interaction shall be sent only if the
     * HLAmanager.HLAfederate.HLAadjust.HLAexceptionReporting switch changing the HLAreportingState
     * parameter sets the parameter to HLAtrue for the indicated joined federate.
     */
    "HLAmanager.HLAfederate.HLAreport.HLAreportException"
        /** In the case in which the HLAreportMOMexception interaction is
        * sent by the RTI because a service interaction (an interaction that
        * imitates a federate’s invocation of an HLA service) was sent and
        * not all of the service’s preconditions are met, the value of this
        * parameter shall be the name of the
        * HLAinteractionRoot.HLA.Manager.HLAfederate.HLAservice
        * interaction that was sent.
        * In the case in which the HLAreportMOMexception interaction is
        * sent by the RTI because a MOM interaction without all of the
        * necessary parameters was sent, the value of this parameter shall
        * be the name of the class of the interaction that was sent.
        * The name of the interaction class provided shall always be fully
        * qualified, as defined in the OMT, so as to avoid potential
        * ambiguities.
        */
        "HLAservice"
        "HLAexception" /// Textual depiction of the exception.
    
    /** This interaction shall be sent by the RTI whenever an HLA service is invoked, either by the indicated
    * joined federate or by the RTI at the indicated joined federate, and the service reporting switch is enabled
    * for the indicated joined federate.
    * This interaction shall always contain the arguments supplied by the service invoker. If the service
    * invocation was successful, the interaction also shall contain the value returned to the invoker (if the
    * service returns a value); otherwise, the interaction also shall contain an indication of the exception that
    * was raised to the invoker.
    */
    "HLAmanager.HLAfederate.HLAreport.HLAreportServiceInvocation"
        "HLAservice"
        "HLAsuccessIndicator"
        "HLAsuppliedArguments"
        "HLAreturnedArguments"
        "HLAexception"
        "HLAserialNumber"
    
    /** The interaction shall be sent by the RTI when one the following occurs: a MOM interaction without all the
     * necessary parameters is sent or an interaction that imitates a federate’s invocation of an HLA service is
     * sent and not all of the service’s preconditions are met.
     */
    "HLAmanager.HLAfederate.HLAreport.HLAreportMOMexception"
        "HLAservice"
        "HLAexception"
        "HLAparameterError"
    
    /** The interaction shall be sent when a federate has been lost from the federation due to a fault. */
    "HLAmanager.HLAfederate.HLAreport.HLAreportFederateLost"
        "HLAfederateName"
        "HLAtimestamp"
        "HLAfaultDescription"

#endif

static PrettyDebug D("MOM", __FILE__);

Responses Mom::processInteraction(const InteractionClassHandle interaction_class_handle,
                                  const std::vector<ParameterHandle>& parameter_handles,
                                  const std::vector<ParameterValue_t>& parameter_values,
                                  const RegionHandle /*region_handle*/)
{
    std::map<ParameterHandle, ParameterValue_t> parameters;
    for (auto i(0u); i < parameter_handles.size(); ++i) {
        parameters[parameter_handles[i]] = parameter_values[i];
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
    Responses ret;
    
    Debug(D, pdGendoc) << "enter Mom::processFederateSetTiming " << federate_handle << ", " << reportPeriod << endl;

    my_federates_update_settings[federate_handle].updateRate = std::chrono::seconds(reportPeriod);
    
    auto mom_msg = make_unique<NM_Mom_Status>();
    mom_msg->setFederation(my_federation.getHandle().get());
    mom_msg->setFederate(federate_handle);
    mom_msg->setMomState(true);
    mom_msg->setUpdatePeriod(my_federates_update_settings[federate_handle].updateRate.count());
    
    ret.emplace_back(my_federation.my_server->getSocketLink(federate_handle), std::move(mom_msg));
    
    Debug(D, pdGendoc) << "exit  Mom::processFederateSetTiming" << endl;

    return ret;
}

/** Modify the ownership state of an attribute of an object instance for the specified joined federate. If the
 * interaction is used to give ownership of the instance attribute to the specified joined federate and another
 * joined federate currently owns the instance attribute, the owning joined federate shall be divested of
 * ownership of the instance attribute before ownership is granted to the specified joined federate. No
 * notification of change of ownership of the instance attribute shall be provided to either joined federate.
 * In order for ownership of the instance attribute to be granted to the specified joined federate, the following
 * conditions shall be true:
 * – The specified joined federate knows about the object instance.
 * – The specified joined federate is publishing the corresponding class attribute at the known class of the
 * specified object instance at that joined federate.
 * – The specified instance attribute is not owned by the RTI (i.e., it is not a predefined attribute of a MOM
 * object class).
 * If one or more of the above conditions are not met, the interaction shall have no effect, and an error shall
 * be reported via an interaction of class HLAmanager.HLAfederate.HLAreport.HLAreportMOMexception.
 * 
 * @param federate_handle Handle of the federate concerned
 * @param objectInstance Handle of the object instance whose attribute state is being changed.
 * @param attribute Handle of the instance attribute whose state is being changed.
 * @param attributeState New state for the attribute of the object instance. (false is Unowned, true is Owned)
 */
Responses Mom::processFederateModifyAttributeState(const FederateHandle& federate_handle,
                                                   const ObjectHandle objectInstance,
                                                   const AttributeHandle attribute,
                                                   const bool attributeState)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateModifyAttributeState " << federate_handle << ", " << objectInstance
                       << ", " << attribute << ", " << attributeState << endl;

    auto object = my_root.objects->getObject(objectInstance);

    // TODO catch everything and package it in a MOMexception
    if (attributeState) {
        // we want to grand federate_handle the ownership of objectInstance.attribute
        // preconditions already checked by the call
        my_federation.divest(object->getAttribute(attribute)->getOwner(), objectInstance, {attribute});
        my_federation.acquire(federate_handle, objectInstance, {attribute}, "");
    }
    else {
        // we want to divest federate_handle the ownership of objectInstance.attribute
        if (object->isAttributeOwnedByFederate(federate_handle, attribute)) {
            my_federation.divest(federate_handle, objectInstance, {attribute});
        }
        else {
            // FIXME do nothing ?
        }
    }

    Debug(D, pdGendoc) << "exit  Mom::processFederateModifyAttributeState" << endl;

    return {};
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

    Responses responses;

    auto& federate = my_federation.getFederate(federate_handle);

    federate.setConveyRegionDesignatorSetsSwitch(conveyRegionDesignatorSets);
    federate.setConveyProducingFederateSwitch(conveyProducingFederate);
    federate.setServiceReportingSwitch(serviceReporting);
    federate.setExceptionReportingSwitch(exceptionReporting);

    // FIXME should be done on Federate instead of here
    auto resp = updateConveyRegionDesignatorSets(federate_handle, conveyRegionDesignatorSets);
    responses.insert(end(responses), make_move_iterator(begin(resp)), make_move_iterator(end(resp)));

    auto resp2 = updateConveyProducingFederate(federate_handle, conveyProducingFederate);
    responses.insert(end(responses), make_move_iterator(begin(resp2)), make_move_iterator(end(resp2)));

    Debug(D, pdGendoc) << "exit  Mom::processFederateSetSwitches" << endl;

    return responses;
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
    // FIXME as of now we only check for object ownership instead of looking at privilegeToDelete ownership
    (void) privilegeToDelete;

    std::map<ObjectClassHandle, int> objectInstancesCounts;

    std::vector<ObjectHandle> ownedObjectInstances;
    my_root.objects->getAllObjectInstancesFromFederate(federate_handle, ownedObjectInstances);
    for (const auto& object : ownedObjectInstances) {
        ++objectInstancesCounts[object];
    }

    std::vector<AttributeValue_t> values{encodeUInt32(federate_handle), encodeHandleBasedCounts(objectInstancesCounts)};

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

    std::map<ObjectClassHandle, int> objectInstancesCounts;

    for (const auto& pair : my_object_instances_updated[federate_handle]) {
        if (not pair.second.empty()) {
            objectInstancesCounts[pair.first] = pair.second.size();
        }
    }

    std::vector<AttributeValue_t> values{encodeUInt32(federate_handle), encodeHandleBasedCounts(objectInstancesCounts)};

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestObjectInstancesUpdated" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

/** Request that the RTI send a report interaction that contains the object instances for which a joined federate
 * has had a Reflect Attribute Values † service invocation. It shall result in one interaction of class
 * HLAmanager.HLAfederate.HLAreport.HLAreportObjectInstancesReflected.
 */
Responses Mom::processFederateRequestObjectInstancesReflected(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestObjectInstancesReflected " << federate_handle << endl;

    /** The interaction shall be sent by the RTI in response to an interaction of class
     * HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesReflected. It shall report the number
     * of object instances (by registered class of the object instances) for which the joined federate has had a
     * Reflect Attribute Values † service invocation. If the joined federate has no object instances that are
     * reflected for a given transport type, then a single interaction shall be sent as a NULL response with the
     * HLAobjectInstanceCounts parameter having an undefined value.
     */
    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederate.HLAreport.HLAreportObjectInstancesReflected");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAobjectInstanceCounts")};

    std::map<ObjectClassHandle, int> objectInstancesCounts;

    for (const auto& pair : my_object_instances_reflected[federate_handle]) {
        if (not pair.second.empty()) {
            objectInstancesCounts[pair.first] = pair.second.size();
        }
    }

    std::vector<AttributeValue_t> values{encodeUInt32(federate_handle), encodeHandleBasedCounts(objectInstancesCounts)};

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestObjectInstancesReflected" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

/** Request that the RTI send a report interaction that contains the number of updates that a joined federate
 * has generated. It shall result in one interaction of class
 * HLAmanager.HLAfederate.HLAreport.HLAreportUpdatesSent for each transportation type.
 */
Responses Mom::processFederateRequestUpdatesSent(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestUpdatesSent " << federate_handle << endl;

    Responses responses;

    /** The interaction shall be sent by the RTI in response to an interaction of class
     * HLAmanager.HLAfederate.HLArequest.HLArequestUpdatesSent. It shall report the number of updates
     * sent (by registered class of the object instances of the updates) by the joined federate since the beginning
     * of the federation execution. One interaction of this class shall be sent by the RTI for each transportation
     * type used. If the joined federate has no updates sent for a given transportation type, then a single
     * interaction shall be sent as a NULL response with the HLAupdateCounts parameter having an undefined
     * value.
     */
    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederate.HLAreport.HLAreportUpdatesSent");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAtransportation"),
                                            getParameterHandle(interaction_handle, "HLAupdateCounts")};

    std::vector<AttributeValue_t> values{encodeUInt32(federate_handle),
                                         encodeString("HLAreliable"),
                                         encodeHandleBasedCounts(my_updates_sent[federate_handle])};

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestUpdatesSent" << endl;

    auto resp = my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
    responses.insert(end(responses), make_move_iterator(begin(resp)), make_move_iterator(end(resp)));

    std::map<ObjectClassHandle, int> bestEffortUpdates; // FIXME we do not track best effort for now
    std::vector<AttributeValue_t> bestEffortValues{
        encodeUInt32(federate_handle), encodeString("HLAbestEffort"), encodeHandleBasedCounts(bestEffortUpdates)};
    auto bestEffortResp
        = my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, bestEffortValues, 0, "");
    responses.insert(end(responses), make_move_iterator(begin(resp)), make_move_iterator(end(resp)));

    return responses;
}

/** Request that the RTI send a report interaction that contains the number of interactions that a joined
 * federate has generated. This count shall include interactions sent with region. It shall result in one
 * interaction of class HLAmanager.HLAfederate.HLAreport.HLAreportInteractionsSent for each
 * transportation type.
 */
Responses Mom::processFederateRequestInteractionsSent(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestInteractionsSent " << federate_handle << endl;

    Responses responses;

    /** The interaction shall be sent by the RTI in response to an interaction of class
     * HLAmanager.HLAfederate.HLArequest.HLArequestInteractionsSent. It shall report the number of
     * interactions sent (by sent class of the interactions) by the joined federate since the beginning of the
     * federation execution. This count shall include interactions sent with region. One interaction of this class
     * shall be sent by the RTI for each transportation type used. If the joined federate has no interactions sent for
     * a given transportation type, then a single interaction shall be sent as a NULL response with the
     * HLAinteractionCounts parameter having an undefined value.
     */
    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederate.HLAreport.HLAreportInteractionsSent");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAtransportation"),
                                            getParameterHandle(interaction_handle, "HLAinteractionCounts")};

    std::vector<AttributeValue_t> values{encodeUInt32(federate_handle),
                                         encodeString("HLAreliable"),
                                         encodeHandleBasedCounts(my_interactions_sent[federate_handle])};

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestInteractionsSent" << endl;

    auto resp = my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
    responses.insert(end(responses), make_move_iterator(begin(resp)), make_move_iterator(end(resp)));

    std::map<ObjectClassHandle, int> bestEffortUpdates; // FIXME we do not track best effort for now
    std::vector<AttributeValue_t> bestEffortValues{
        encodeUInt32(federate_handle), encodeString("HLAbestEffort"), encodeHandleBasedCounts(bestEffortUpdates)};
    auto bestEffortResp
        = my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, bestEffortValues, 0, "");
    responses.insert(
        end(responses), make_move_iterator(begin(bestEffortResp)), make_move_iterator(end(bestEffortResp)));

    return responses;
}

/** Request that the RTI send a report interaction that contains the number of reflections that a joined federate
 * has received. It shall result in one interaction of class
 * HLAmanager.HLAfederate.HLAreport.HLAreportReflectionsReceived for each transportation type.
 */
Responses Mom::processFederateRequestReflectionsReceived(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestReflectionsReceived " << federate_handle << endl;

    Responses responses;

    /** The interaction shall be sent by the RTI in response to an interaction of class
     * HLAmanager.HLAfederate.HLArequest.HLArequestReflectionsReceived. It shall report the number of
     * reflections received (by registered class of the object instances of the reflects) by the joined federate since
     * the beginning of the federation execution. One interaction of this class shall be sent by the RTI for each
     * transportation type used. If the joined federate has no reflections received for a given transportation type,
     * then a single interaction shall be sent as a NULL response with the HLAreflectCounts parameter having
     * an undefined value.
     */
    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederate.HLAreport.HLAreportReflectionsReceived");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAtransportation"),
                                            getParameterHandle(interaction_handle, "HLAreflectCounts")};

    std::vector<AttributeValue_t> values{encodeUInt32(federate_handle),
                                         encodeString("HLAreliable"),
                                         encodeHandleBasedCounts(my_reflections_received[federate_handle])};

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestReflectionsReceived" << endl;

    auto resp = my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
    responses.insert(end(responses), make_move_iterator(begin(resp)), make_move_iterator(end(resp)));

    std::map<ObjectClassHandle, int> bestEffortUpdates; // FIXME we do not track best effort for now
    std::vector<AttributeValue_t> bestEffortValues{
        encodeUInt32(federate_handle), encodeString("HLAbestEffort"), encodeHandleBasedCounts(bestEffortUpdates)};
    auto bestEffortResp
        = my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, bestEffortValues, 0, "");
    responses.insert(
        end(responses), make_move_iterator(begin(bestEffortResp)), make_move_iterator(end(bestEffortResp)));

    return responses;
}

/** Request that the RTI send a report interaction that contains the number of interactions that a joined
 * federate has received. It shall result in one interaction of class
 * HLAmanager.HLAfederate.HLAreport.HLAreportInteractionsReceived for each transportation type.
 */
Responses Mom::processFederateRequestInteractionsReceived(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestInteractionsReceived " << federate_handle << endl;

    Responses responses;

    /** The interaction shall be sent by the RTI in response to an interaction of class
     * HLAmanager.HLAfederate.HLArequest.HLArequestInteractionsReceived. It shall report the number of
     * interactions received (by sent class of the interactions) by the joined federate since the beginning of the
     * federation execution. One interaction of this class shall be sent by the RTI for each transportation type
     * used. If the joined federate has no interactions received for a given transportation type, then a single
     * interaction shall be sent as a NULL response with the HLAinteractionCounts parameter having an
     * undefined value.
     */
    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederate.HLAreport.HLAreportInteractionsReceived");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAtransportation"),
                                            getParameterHandle(interaction_handle, "HLAinteractionCounts")};

    std::vector<AttributeValue_t> values{encodeUInt32(federate_handle),
                                         encodeString("HLAreliable"),
                                         encodeHandleBasedCounts(my_interactions_received[federate_handle])};

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestInteractionsReceived" << endl;

    auto resp = my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
    responses.insert(end(responses), make_move_iterator(begin(resp)), make_move_iterator(end(resp)));

    std::map<ObjectClassHandle, int> bestEffortUpdates; // FIXME we do not track best effort for now
    std::vector<AttributeValue_t> bestEffortValues{
        encodeUInt32(federate_handle), encodeString("HLAbestEffort"), encodeHandleBasedCounts(bestEffortUpdates)};
    auto bestEffortResp
        = my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, bestEffortValues, 0, "");
    responses.insert(
        end(responses), make_move_iterator(begin(bestEffortResp)), make_move_iterator(end(bestEffortResp)));

    return responses;
}

/** Request that the RTI send a report interaction that contains the information that a joined federate maintains
 * on a single object instance. It shall result in one interaction of class
 * HLAmanager.HLAfederate.HLAreport.HLAreportObjectInstanceInformation.
 */
Responses Mom::processFederateRequestObjectInstanceInformation(const FederateHandle& federate_handle,
                                                               const ObjectHandle& objectInstance)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestObjectInstanceInformation " << federate_handle << ", "
                       << objectInstance << endl;

    /** The interaction shall be sent by the RTI in response to an interaction of class
     * HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstanceInformation. It shall report on a
     * single object instance and portray the instance attributes of that object instance that are owned by the
     * joined federate, the registered class of the object instance, and the known class of the object instance at
     * that joined federate. If the joined federate does not know the object instance, a single interaction shall be
     * sent as a NULL response with the HLAownedInstanceAttributeList parameter having an undefined value.
     */
    auto interaction_handle = my_root.Interactions->getHandleFromName(
        "HLAmanager.HLAfederate.HLAreport.HLAreportObjectInstanceInformation");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAobjectInstance"),
                                            getParameterHandle(interaction_handle, "HLAownedInstanceAttributeList"),
                                            getParameterHandle(interaction_handle, "HLAregisteredClass"),
                                            getParameterHandle(interaction_handle, "HLAknownClass")};

    try {
        auto object = my_root.objects->getObject(objectInstance);

        ObjectClassHandle registeredClass = object->getClass();
        ObjectClassHandle knownClass = registeredClass; // FIXME we do not differenciate known and registered class

        std::vector<AttributeHandle> ownedInstanceAttributeList;
        for (const auto& pair :
             my_root.ObjectClasses->getObjectFromHandle(registeredClass)->getHandleClassAttributeMap()) {
            if (object->isAttributeOwnedByFederate(pair.first, federate_handle)) {
                ownedInstanceAttributeList.push_back(pair.first);
            }
        }

        std::vector<AttributeValue_t> values{encodeUInt32(federate_handle),
                                             encodeUInt32(objectInstance),
                                             encodeVectorHandle(ownedInstanceAttributeList),
                                             encodeUInt32(registeredClass),
                                             encodeUInt32(knownClass)};

        Debug(D, pdGendoc) << "exit  Mom::processFederateRequestObjectInstanceInformation" << endl;

        return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
    }
    catch (ObjectNotKnown& e) {
        std::vector<AttributeValue_t> values{encodeUInt32(federate_handle),
                                             encodeUInt32(objectInstance),
                                             encodeVectorHandle({}),
                                             encodeUInt32(0),
                                             encodeUInt32(0)};

        Debug(D, pdGendoc) << "exit  Mom::processFederateRequestObjectInstanceInformation" << endl;

        return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
    }
}

/** Request that the RTI shall send a report interaction with the content of the specified FOM module that was
 * specified by the federate. The FOM module is indicated by the order number in the federate’s
 * HLAFOMmoduleDesignatorList attribute.
 */
Responses Mom::processFederateRequestFOMmoduleData(const FederateHandle& federate_handle, const int FOMmoduleIndicator)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestFOMmoduleData " << federate_handle << ", "
                       << FOMmoduleIndicator << endl;

    /** The interaction shall be sent by the RTI in response to an interaction of class
     * HLAmanager.HLAfederate.HLArequest.HLArequestFOMmoduleData. It shall report the content of the
     * specified FOM module for the federate.
     */
    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederate.HLAreport.HLAreportFOMmoduleData");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAfederate"),
                                            getParameterHandle(interaction_handle, "HLAFOMmoduleIndicator"),
                                            getParameterHandle(interaction_handle, "HLAFOMmoduleData")};

    std::string fomModuleData = "TODO"; // TODO

    std::vector<AttributeValue_t> values{
        encodeUInt32(federate_handle), encodeUInt32(FOMmoduleIndicator), encodeString(fomModuleData)};

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestFOMmoduleData" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

/** Cause the RTI to react as if the Resign Federation Execution service has been invoked by the specified
 * joined federate using the specified arguments (see 4.10).
 * 
 * A joined federate shall be able to send this interaction anytime.
 */
Responses Mom::processFederateResignFederationExecution(const FederateHandle& federate_handle,
                                                        const ResignAction resignAction)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateResignFederationExecution " << federate_handle << ", "
                       << static_cast<int>(resignAction) << endl;

    Responses responses;

    responses = my_federation.remove(federate_handle);

    Debug(D, pdGendoc) << "exit  Mom::processFederateResignFederationExecution" << endl;

    return responses;
}

/** Cause the RTI to react as if the Synchronization Point Achieved service has been invoked by the specified
 * joined federate using the specified arguments (see 4.14).
 */
Responses Mom::processFederateSynchronizationPointAchieved(const FederateHandle& federate_handle,
                                                           const std::string& label)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateSynchronizationPointAchieved " << federate_handle << ", " << label
                       << endl;

    Responses responses = my_federation.unregisterSynchronization(federate_handle, label);

    Debug(D, pdGendoc) << "exit  Mom::processFederateSynchronizationPointAchieved" << endl;

    return responses;
}

/** Cause the RTI to react as if the Federate Save Begun service has been invoked by the specified joined
 * federate using the specified arguments (see 4.18).
 */
Responses Mom::processFederateFederateSaveBegun(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateFederateSaveBegun " << federate_handle << endl;

    my_federation.federateSaveBegun(federate_handle);

    Debug(D, pdGendoc) << "exit  Mom::processFederateFederateSaveBegun" << endl;

    return {};
}

/** Cause the RTI to react as if the Federate Save Complete service has been invoked by the specified joined
 * federate using the specified arguments (see 4.19).
 */
Responses Mom::processFederateFederateSaveComplete(const FederateHandle& federate_handle, const bool successIndicator)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateFederateSaveComplete " << federate_handle << ", "
                       << successIndicator << endl;

    return my_federation.federateSaveStatus(federate_handle, successIndicator);
}

/** Cause the RTI to react as if the Federate Restore Complete service has been invoked by the specified
 * joined federate using the specified arguments (see 4.28).
 */
Responses Mom::processFederateFederateRestoreComplete(const FederateHandle& federate_handle,
                                                      const bool successIndicator)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateFederateRestoreComplete " << federate_handle << ", "
                       << successIndicator << endl;

    return my_federation.federateRestoreStatus(federate_handle, successIndicator);
}

/** Cause the RTI to react as if the Publish Object Class Attributes service has been invoked by the specified
 * joined federate using the specified arguments (see 5.2).
 */
Responses Mom::processFederatePublishObjectClassAttributes(const FederateHandle& federate_handle,
                                                           const ObjectClassHandle& objectClass,
                                                           const std::vector<AttributeHandle>& attributeList)
{
    Debug(D, pdGendoc) << "enter Mom::processFederatePublishObjectClassAttributes " << federate_handle << ", "
                       << objectClass << ", " << attributeList.size() << endl;

    Responses responses = my_federation.publishObject(federate_handle, objectClass, attributeList, true);

    Debug(D, pdGendoc) << "exit  Mom::processFederatePublishObjectClassAttributes" << endl;

    return responses;
}

/** Cause the RTI to react as if the Unpublish Object Class Attributes service has been invoked by the
 * specified joined federate using the specified arguments (see 5.3).
 */
Responses Mom::processFederateUnpublishObjectClassAttributes(const FederateHandle& federate_handle,
                                                             const ObjectClassHandle& objectClass,
                                                             const std::vector<AttributeHandle>& attributeList)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateUnpublishObjectClassAttributes " << federate_handle << ", "
                       << objectClass << ", " << attributeList.size() << endl;

    Responses responses = my_federation.publishObject(federate_handle, objectClass, attributeList, false);

    Debug(D, pdGendoc) << "exit  Mom::processFederateUnpublishObjectClassAttributes" << endl;

    return responses;
}

/** Cause the RTI to react as if the Publish Interaction Class service has been invoked by the specified joined
 * federate using the specified arguments (see 5.4).
 */
Responses Mom::processFederatePublishInteractionClass(const FederateHandle& federate_handle,
                                                      const InteractionClassHandle& interactionClass)
{
    Debug(D, pdGendoc) << "enter Mom::processFederatePublishInteractionClass " << federate_handle << ", "
                       << interactionClass << endl;

    Responses responses = my_federation.publishInteraction(federate_handle, interactionClass, true);

    Debug(D, pdGendoc) << "exit  Mom::processFederatePublishInteractionClass" << endl;

    return responses;
}

/** Cause the RTI to react as if the Unpublish Interaction Class service has been invoked by the specified
 * joined federate using the specified arguments (see 5.5).
 */
Responses Mom::processFederateUnpublishInteractionClass(const FederateHandle& federate_handle,
                                                        const InteractionClassHandle& interactionClass)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateUnpublishInteractionClass " << federate_handle << ", "
                       << interactionClass << endl;

    Responses responses = my_federation.publishInteraction(federate_handle, interactionClass, false);

    Debug(D, pdGendoc) << "exit  Mom::processFederateUnpublishInteractionClass" << endl;

    return responses;
}

/** Cause the RTI to react as if the Subscribe Object Class Attributes service has been invoked by the
 * specified joined federate using the specified arguments (see 5.6).
 */
Responses Mom::processFederateSubscribeObjectClassAttributes(const FederateHandle& federate_handle,
                                                             const ObjectClassHandle& objectClass,
                                                             const std::vector<AttributeHandle>& attributeList,
                                                             const bool active)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateSubscribeObjectClassAttributes " << federate_handle << ", "
                       << objectClass << ", " << attributeList.size() << ", " << active << endl;

    Responses responses = my_federation.subscribeObject(federate_handle, objectClass, attributeList, true);

    Debug(D, pdGendoc) << "exit  Mom::processFederateSubscribeObjectClassAttributes" << endl;

    return responses;
}

/** Cause the RTI to react as if the Unsubscribe Object Class Attributes service has been invoked by the
 * specified joined federate using the specified arguments (see 5.7).
 */
Responses Mom::processFederateUnsubscribeObjectClassAttributes(const FederateHandle& federate_handle,
                                                               const ObjectClassHandle& objectClass,
                                                               const std::vector<AttributeHandle>& attributeList)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateUnsubscribeObjectClassAttributes " << federate_handle << ", "
                       << objectClass << ", " << attributeList.size() << endl;

    Responses responses = my_federation.subscribeObject(federate_handle, objectClass, {}, false);

    Debug(D, pdGendoc) << "exit  Mom::processFederateUnsubscribeObjectClassAttributes" << endl;

    return responses;
}

/** Cause the RTI to react as if the Subscribe Interaction Class service has been invoked by the specified
 * joined federate using the specified arguments (see 5.8).
 */
Responses Mom::processFederateSubscribeInteractionClass(const FederateHandle& federate_handle,
                                                        const InteractionClassHandle& interactionClass,
                                                        const bool active)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateSubscribeInteractionClass " << federate_handle << ", "
                       << interactionClass << ", " << active << endl;

    Responses responses = my_federation.subscribeInteraction(federate_handle, interactionClass, true);

    Debug(D, pdGendoc) << "exit  Mom::processFederateSubscribeInteractionClass" << endl;

    return responses;
}

/** Cause the RTI to react as if the Unsubscribe Interaction Class service has been invoked by the specified
 * joined federate using the specified arguments (see 5.9).
 */
Responses Mom::processFederateUnsubscribeInteractionClass(const FederateHandle& federate_handle,
                                                          const InteractionClassHandle& interactionClass)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateUnsubscribeInteractionClass " << federate_handle << ", "
                       << interactionClass << endl;

    Responses responses = my_federation.subscribeInteraction(federate_handle, interactionClass, false);

    Debug(D, pdGendoc) << "exit  Mom::processFederateUnsubscribeInteractionClass" << endl;

    return responses;
}

/** Cause the RTI to react as if the Delete Object Instance service has been invoked by the specified joined
 * federate using the specified arguments (see 6.14).
 * If applicable, the retraction handle returned by invoking this service is discarded, and the scheduled action cannot be retracted.
 */
Responses Mom::processFederateDeleteObjectInstance(const FederateHandle& federate_handle,
                                                   const ObjectHandle& objectInstance,
                                                   const std::string& tag,
                                                   const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateDeleteObjectInstance " << federate_handle << ", " << objectInstance
                       << ", " << tag << ", " << timeStamp << endl;

    Responses responses;

    if (timeStamp.isZero()) {
        responses = my_federation.deleteObject(federate_handle, objectInstance, tag);
    }
    else {
        responses = my_federation.deleteObject(federate_handle, objectInstance, timeStamp, tag);
    }

    Debug(D, pdGendoc) << "exit  Mom::processFederateDeleteObjectInstance" << endl;

    return responses;
}

/** Cause the RTI to react as if the Local Delete Object Instance service has been invoked by the specified
 * joined federate using the specified arguments (see 6.16).
 */
Responses Mom::processFederateLocalDeleteObjectInstance(const FederateHandle& federate_handle,
                                                        const ObjectHandle& objectInstance)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateLocalDeleteObjectInstance " << federate_handle << ", "
                       << objectInstance << endl;

    Responses responses;

    // FIXME not handled by certi

    Debug(D, pdGendoc) << "exit  Mom::processFederateLocalDeleteObjectInstance" << endl;

    return responses;
}

/** Cause the RTI to react as if the Request Attribute Transportation Type Change service has been invoked
 * by the specified joined federate using the specified arguments (see 6.23).
 */
Responses
Mom::processFederateRequestAttributeTransportationTypeChange(const FederateHandle& federate_handle,
                                                             const ObjectHandle& objectInstance,
                                                             const std::vector<AttributeHandle>& attributeList,
                                                             const std::string& transportation)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestAttributeTransportationTypeChange " << federate_handle
                       << ", " << objectInstance << ", " << attributeList.size() << ", " << transportation << endl;

    Responses responses;

    // FIXME not handled by certi

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestAttributeTransportationTypeChange" << endl;

    return responses;
}

/** Cause the RTI to react as if the Request Interaction Transportation Type Change service has been invoked
 * by the specified joined federate using the specified arguments (see 6.27)
 */
Responses Mom::processFederateRequestInteractionTransportationTypeChange(const FederateHandle& federate_handle,
                                                                         const InteractionClassHandle& interactionClass,
                                                                         const std::string& transportation)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateRequestInteractionTransportationTypeChange " << federate_handle
                       << ", " << interactionClass << ", " << transportation << endl;

    Responses responses;

    // FIXME not handled by certi

    Debug(D, pdGendoc) << "exit  Mom::processFederateRequestInteractionTransportationTypeChange" << endl;

    return responses;
}

/** Cause the RTI to react as if the Unconditional Attribute Ownership Divestiture service has been invoked
 * by the specified joined federate using the specified arguments (see 7.2).
 */
Responses
Mom::processFederateUnconditionalAttributeOwnershipDivestiture(const FederateHandle& federate_handle,
                                                               const ObjectHandle& objectInstance,
                                                               const std::vector<AttributeHandle>& attributeList)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateUnconditionalAttributeOwnershipDivestiture " << federate_handle
                       << ", " << objectInstance << ", " << attributeList.size() << endl;

    Responses responses = my_federation.divest(federate_handle, objectInstance, attributeList);

    Debug(D, pdGendoc) << "exit  Mom::processFederateUnconditionalAttributeOwnershipDivestiture" << endl;

    return responses;
}

/** Cause the RTI to react as if the Enable Time Regulation service has been invoked by the specified joined
 * federate using the specified arguments (see 8.2).
 */
Responses Mom::processFederateEnableTimeRegulation(const FederateHandle& federate_handle, const int lookahead)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateEnableTimeRegulation " << federate_handle << ", " << lookahead
                       << endl;

    Responses responses = my_federation.addRegulator(federate_handle, 0); // FIXME what time should we put here ?

    Debug(D, pdGendoc) << "exit  Mom::processFederateEnableTimeRegulation" << endl;

    return responses;
}

/** Cause the RTI to react as if the Disable Time Regulation service has been invoked by the specified joined
 * federate using the specified arguments (see 8.4).
 */
Responses Mom::processFederateDisableTimeRegulation(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateDisableTimeRegulation " << federate_handle << endl;

    Responses responses = my_federation.removeRegulator(federate_handle);

    Debug(D, pdGendoc) << "exit  Mom::processFederateDisableTimeRegulation" << endl;

    return responses;
}

/** Cause the RTI to react as if the Enable Time Constrained service has been invoked by the specified joined
 * federate using the specified arguments (see 8.5).
 */
Responses Mom::processFederateEnableTimeConstrained(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateEnableTimeConstrained " << federate_handle << endl;

    Responses responses
        = my_federation.setConstrained(federate_handle, true, 0); // FIXME what time should we put here ?

    Debug(D, pdGendoc) << "exit  Mom::processFederateEnableTimeConstrained" << endl;

    return responses;
}

/** Cause the RTI to react as if the Disable Time Constrained service has been invoked by the specified joined
 * federate using the specified arguments (see 8.7).
 */
Responses Mom::processFederateDisableTimeConstrained(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateDisableTimeConstrained " << federate_handle << endl;

    Responses responses
        = my_federation.setConstrained(federate_handle, false, 0); // FIXME what time should we put here ?

    Debug(D, pdGendoc) << "exit  Mom::processFederateDisableTimeConstrained" << endl;

    return responses;
}

/** Cause the RTI to react as if the Time Advance Request service has been invoked by the specified joined
 * federate using the specified arguments (see 8.8).
 */
Responses Mom::processFederateTimeAdvanceRequest(const FederateHandle& federate_handle, const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateTimeAdvanceRequest " << federate_handle << ", " << timeStamp
                       << endl;

    Responses responses;

    auto message = make_unique<NM_Time_Advance_Request>();

    message->setFederation(my_federation.getHandle().get());
    message->setFederate(federate_handle);
    message->setDate(timeStamp);

    responses.emplace_back(getSocketForFederate(federate_handle), std::move(message));

    Debug(D, pdGendoc) << "exit  Mom::processFederateTimeAdvanceRequest" << endl;

    return responses;
}

/** Cause the RTI to react as if the Time Advance Request Available service has been invoked by the specified
 * joined federate using the specified arguments (see 8.9)
 */
Responses Mom::processFederateTimeAdvanceRequestAvailable(const FederateHandle& federate_handle,
                                                          const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateTimeAdvanceRequestAvailable " << federate_handle << ", "
                       << timeStamp << endl;

    Responses responses;

    auto message = make_unique<NM_Time_Advance_Request_Available>();

    message->setFederation(my_federation.getHandle().get());
    message->setFederate(federate_handle);
    message->setDate(timeStamp);

    responses.emplace_back(getSocketForFederate(federate_handle), std::move(message));

    Debug(D, pdGendoc) << "exit  Mom::processFederateTimeAdvanceRequestAvailable" << endl;

    return responses;
}

/** Cause the RTI to react as if the Next Message Request service has been invoked by the specified joined
 * federate using the specified arguments (see 8.10)
 */
Responses Mom::processFederateNextMessageRequest(const FederateHandle& federate_handle, const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateNextMessageRequest " << federate_handle << ", " << timeStamp
                       << endl;

    Responses responses;

    auto message = make_unique<NM_Next_Message_Request>();

    message->setFederation(my_federation.getHandle().get());
    message->setFederate(federate_handle);
    message->setDate(timeStamp);

    responses.emplace_back(getSocketForFederate(federate_handle), std::move(message));

    Debug(D, pdGendoc) << "exit  Mom::processFederateNextMessageRequest" << endl;

    return responses;
}

/** Cause the RTI to react as if the Next Message Request Available service has been invoked by the specified
 * joined federate using the specified arguments (see 8.11)
 */
Responses Mom::processFederateNextMessageRequestAvailable(const FederateHandle& federate_handle,
                                                          const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateNextMessageRequestAvailable " << federate_handle << ", "
                       << timeStamp << endl;

    Responses responses;

    auto message = make_unique<NM_Next_Message_Request_Available>();

    message->setFederation(my_federation.getHandle().get());
    message->setFederate(federate_handle);
    message->setDate(timeStamp);

    responses.emplace_back(getSocketForFederate(federate_handle), std::move(message));

    Debug(D, pdGendoc) << "exit  Mom::processFederateNextMessageRequestAvailable" << endl;

    return responses;
}

/** Cause the RTI to react as if the Flush Queue Request service has been invoked by the specified joined
 * federate using the specified arguments (see 8.12)
 */
Responses Mom::processFederateFlushQueueRequest(const FederateHandle& federate_handle, const FederationTime& timeStamp)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateFlushQueueRequest " << federate_handle << ", " << timeStamp
                       << endl;

    Responses responses;

    // TODO not handled by certi

    Debug(D, pdGendoc) << "exit  Mom::processFederateFlushQueueRequest" << endl;

    return responses;
}

/** Cause the RTI to react as if the Enable Asynchronous Delivery service has been invoked by the specified
 * joined federate using the specified arguments (see 8.14)
 */
Responses Mom::processFederateEnableAsynchronousDelivery(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateEnableAsynchronousDelivery " << federate_handle << endl;

    Responses responses;

    // FIXME not handled by certi

    Debug(D, pdGendoc) << "exit  Mom::processFederateEnableAsynchronousDelivery" << endl;

    return responses;
}

/** Cause the RTI to react as if the Disable Asynchronous Delivery service has been invoked by the specified
 * joined federate using the specified arguments (see 8.15)
 */
Responses Mom::processFederateDisableAsynchronousDelivery(const FederateHandle& federate_handle)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateDisableAsynchronousDelivery " << federate_handle << endl;

    Responses responses;

    // FIXME not handled by certi

    Debug(D, pdGendoc) << "exit  Mom::processFederateDisableAsynchronousDelivery" << endl;

    return responses;
}

/** Cause the RTI to react as if the Modify Lookahead service has been invoked by the specified joined
 * federate using the specified arguments (see 8.19)
 */
Responses Mom::processFederateModifyLookahead(const FederateHandle& federate_handle, const int lookahead)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateModifyLookahead " << federate_handle << ", " << lookahead << endl;

    Responses responses;

    // FIXME not handled by certi

    Debug(D, pdGendoc) << "exit  Mom::processFederateModifyLookahead" << endl;

    return responses;
}

/** Cause the RTI to react as if the Change Attribute Order Type service has been invoked by the specified
 * joined federate using the specified arguments (see 8.23)
 */
Responses Mom::processFederateChangeAttributeOrderType(const FederateHandle& federate_handle,
                                                       const ObjectHandle& objectInstance,
                                                       const std::vector<AttributeHandle>& attributeList,
                                                       const OrderType& sendOrder)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateChangeAttributeOrderType " << federate_handle << ", "
                       << objectInstance << ", " << attributeList.size() << ", " << static_cast<int>(sendOrder) << endl;

    Responses responses;

    // FIXME not handled by certi

    Debug(D, pdGendoc) << "exit  Mom::processFederateChangeAttributeOrderType" << endl;

    return responses;
}

/** Cause the RTI to react as if the Change Interaction Order Type service has been invoked by the specified
 * joined federate using the specified arguments (see 8.24)
 */
Responses Mom::processFederateChangeInteractionOrderType(const FederateHandle& federate_handle,
                                                         const InteractionClassHandle& interactionClass,
                                                         const OrderType& sendOrder)
{
    Debug(D, pdGendoc) << "enter Mom::processFederateChangeInteractionOrderType " << federate_handle << ", "
                       << interactionClass << ", " << static_cast<int>(sendOrder) << endl;

    Responses responses;

    // FIXME not handled by certi

    Debug(D, pdGendoc) << "exit  Mom::processFederateChangeInteractionOrderType" << endl;

    return responses;
}

/** Set the values of federation execution-wide switches. A joined federate may send individual declared
 * parameters of this subclass
 */
Responses Mom::processFederationSetSwitches(const bool autoProvide)
{
    Debug(D, pdGendoc) << "enter Mom::processFederationSetSwitches " << autoProvide << endl;

    auto ret = my_federation.setAutoProvide(autoProvide);

    Debug(D, pdGendoc) << "exit  Mom::processFederationSetSwitches" << endl;

    return ret;
}

/** Request that the RTI send a report interaction that contains a list of all in-progress federation
 * synchronization points. It shall result in one interaction of class
 * HLAmanager.HLAfederate.HLAreport.HLAreportSynchronizationPoints.
 */
Responses Mom::processFederationRequestSynchronizationPoints()
{
    Debug(D, pdGendoc) << "enter Mom::processFederationRequestSynchronizationPoints" << endl;

    /** The interaction shall be sent by the RTI in response to an interaction of class
     * HLAmanager.HLAfederation.HLArequest.HLArequestSynchronizationPoints. It shall report the list of
     * active synchronization points in the federation execution. If there are no active synchronization points in
     * the federation execution, a single interaction shall be sent as a NULL response with the HLAsyncPoints
     * parameter having an undefined value.
     */
    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederation.HLAreport.HLAreportSynchronizationPoints");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAsyncPoints")};

    std::vector<std::string> syncPoints;
    for (const auto& pair : my_federation.my_synchronization_labels) {
        syncPoints.push_back(pair.first);
    }

    std::vector<AttributeValue_t> values{encodeVectorString(syncPoints)};

    Debug(D, pdGendoc) << "exit  Mom::processFederationRequestSynchronizationPoints" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

/** Request that the RTI send a report interaction that contains a list that includes each federate (and its
 * synchronization status) that is associated with a particular synchronization point. It shall result in one
 * interaction of class HLAmanager.HLAfederate.HLAreport.HLAreportSynchronizationPointStatus.
 */
Responses Mom::processFederationRequestSynchronizationPointStatus(const std::string& syncPointName)
{
    Debug(D, pdGendoc) << "enter Mom::processFederationRequestSynchronizationPointStatus " << syncPointName << endl;

    /** The interaction shall be sent by the RTI in response to an interaction of class
     * HLAmanager.HLAfederation.HLArequest.HLArequestSynchronizationPointStatus. It shall report the
     * status of a particular synchronization point. This report shall be a list that includes each federate (and its
     * synchronization status) that is associated with the particular synchronization point.
     * 
     * One interaction of class HLAmanager.HLAfederation.HLAreport.HLAreportSynchronizationPointStatus
     * shall be sent by the RTI for each active synchronization point in the federation execution. If there is no
     * active synchronization point with the given name from the request, then an interaction shall be sent as a
     * NULL response with the HLAsyncPointFederates parameter having an undefined value.
     */
    auto interaction_handle = my_root.Interactions->getHandleFromName(
        "HLAmanager.HLAfederation.HLAreport.HLAreportSynchronizationPointStatus");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAsyncPointName"),
                                            getParameterHandle(interaction_handle, "HLAsyncPointFederates")};

    std::map<FederateHandle, int> syncPointFederates;

    if (my_federation.my_synchronization_labels.find(syncPointName) != end(my_federation.my_synchronization_labels)) {
        for (const auto& pair : my_federation.my_federates) {
            if (pair.second->hasSynchronizationLabel(syncPointName)) {
                // we should trace if the federate is attempting to register or moving to sync point
                // FIXME for now as the response is sent immediately we only trace the second one
                syncPointFederates[pair.first] = static_cast<int>(SyncPointStatus::MovingToSyncPoint);
            }
            else {
                // we should trace if the federate is waiting for the rest of the federation or does not care about the sync
                // FIXME for now as the information is not available, we answer waiting
                syncPointFederates[pair.first] = static_cast<int>(SyncPointStatus::WaitingForRestOfFederation);
            }
        }
    }

    std::vector<AttributeValue_t> values{encodeString(syncPointName), encodeHandleBasedCounts(syncPointFederates)};

    Debug(D, pdGendoc) << "exit  Mom::processFederationRequestSynchronizationPointStatus" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

/** Requests that the RTI shall send a report interaction with the content of the specified FOM module for the
 * federation. The FOM module is indicated by the order number in the federation’s
 * HLAFOMmoduleDesignatorList attribute.
 */
Responses Mom::processFederationRequestFOMmoduleData(const int FOMmoduleIndicator)
{
    Debug(D, pdGendoc) << "enter Mom::processFederationRequestFOMmoduleData " << FOMmoduleIndicator << endl;

    /** The interaction shall be sent by the RTI in response to an interaction of class
     * HLAmanager.HLAfederation.HLArequest.HLArequestFOMmoduleData. It shall report the content of the
     * specified FOM module for the federation.
     */
    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederation.HLAreport.HLAreportFOMmoduleData");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAFOMmoduleIndicator"),
                                            getParameterHandle(interaction_handle, "HLAFOMmoduleData")};

    std::string fomModuleData = "TODO"; // TODO

    std::vector<AttributeValue_t> values{encodeUInt32(FOMmoduleIndicator), encodeString(fomModuleData)};

    Debug(D, pdGendoc) << "exit  Mom::processFederationRequestFOMmoduleData" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

/** Requests that the RTI shall send a report interaction with the content of the MIM for the federation.
 */
Responses Mom::processFederationRequestMIMData()
{
    Debug(D, pdGendoc) << "enter Mom::processFederationRequestMIMData" << endl;

    /** The interaction shall be sent by the RTI in response to an interaction of class
     * HLAmanager.HLAfederation.HLArequest.HLArequest MIMData. It shall report the content of the MIM
     * for the federation.
     */
    auto interaction_handle
        = my_root.Interactions->getHandleFromName("HLAmanager.HLAfederation.HLAreport.HLAreportMIMData");

    std::vector<AttributeHandle> parameters{getParameterHandle(interaction_handle, "HLAMIMData")};

    std::string mimData = "TODO"; // TODO

    std::vector<AttributeValue_t> values{encodeString(mimData)};

    Debug(D, pdGendoc) << "exit  Mom::processFederationRequestMIMData" << endl;

    return my_federation.broadcastInteraction(my_handle, interaction_handle, parameters, values, 0, "");
}

void Mom::registerObjectInstanceUpdated(const FederateHandle federate,
                                        const ObjectClassHandle object,
                                        const ObjectHandle instance)
{
    my_object_instances_updated[federate][object].insert(instance);
}

void Mom::registerObjectInstanceReflected(const FederateHandle federate,
                                          const ObjectClassHandle object,
                                          const ObjectHandle instance)
{
    my_object_instances_reflected[federate][object].insert(instance);
}

void Mom::registerUpdate(const FederateHandle federate, const ObjectClassHandle object)
{
    ++my_updates_sent[federate][object];
}

void Mom::registerReflection(const FederateHandle federate, const ObjectClassHandle object)
{
    ++my_reflections_received[federate][object];
}

void Mom::registerInteractionSent(const FederateHandle federate, const InteractionClassHandle interaction)
{
    ++my_interactions_sent[federate][interaction];
}

void Mom::registerInteractionReceived(const FederateHandle federate, const InteractionClassHandle interaction)
{
    ++my_interactions_received[federate][interaction];
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
