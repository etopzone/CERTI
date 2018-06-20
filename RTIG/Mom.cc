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

#include <numeric>

#include <libCERTI/PrettyDebug.hh>

#include <libCERTI/InteractionSet.hh>
#include <libCERTI/ObjectClassSet.hh>

using std::cout;
using std::endl;

namespace {

enum class MomAction { Publish, Subsribe, Nothing };

static const std::map<std::string, std::pair<MomAction, std::set<std::string>>> the_required_object_classes{
    {"HLAobjectRoot", {MomAction::Nothing, std::set<std::string>{}}},
    {"HLAmanager", {MomAction::Nothing, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate",
     {MomAction::Publish,
      {"HLAfederateHandle",
       "HLAfederateName",
       "HLAfederateType",
       "HLAfederateHost",
       "HLARTIversion",
       "HLAFOMmoduleDesignatorList",
       "HLAtimeConstrained",
       "HLAtimeRegulating",
       "HLAasynchronousDelivery",
       "HLAfederateState",
       "HLAtimeManagerState",
       "HLAlogicalTime",
       "HLAlookahead",
       "HLAGALT",
       "HLALITS",
       "HLAROlength",
       "HLATSOlength",
       "HLAreflectionsReceived",
       "HLAupdatesSent",
       "HLAinteractionsReceived",
       "HLAinteractionsSent",
       "HLAobjectInstancesThatCanBeDeleted",
       "HLAobjectInstancesUpdated",
       "HLAobjectInstancesReflected",
       "HLAobjectInstancesDeleted",
       "HLAobjectInstancesRemoved",
       "HLAobjectInstancesRegistered",
       "HLAobjectInstancesDiscovered",
       "HLAtimeGrantedTime",
       "HLAtimeAdvancingTime",
       "HLAconveyRegionDesignatorSets",
       "HLAconveyProducingFederate"}}},
    {"HLAmanager.HLAfederation",
     {MomAction::Publish,
      {"HLAfederationName",
       "HLAfederatesInFederation",
       "HLARTIversion",
       "HLAMIMDesignator",
       "HLAFOMmoduleDesignatorList",
       "HLAcurrentFDD",
       "HLAtimeImplementationName",
       "HLAlastSaveName",
       "HLAlastSaveTime",
       "HLAnextSaveName",
       "HLAnextSaveTime",
       "HLAautoProvide"}}}};

static const std::map<std::string, std::pair<MomAction, std::set<std::string>>> the_required_interactions{
    {"HLAinteractionRoot", {MomAction::Nothing, std::set<std::string>{}}},
    {"HLAmanager", {MomAction::Nothing, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate", {MomAction::Nothing, {"HLAfederate"}}},
    {"HLAmanager.HLAfederate.HLAadjust", {MomAction::Nothing, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLAadjust.HLAsetTiming", {MomAction::Subsribe, {"HLAreportPeriod"}}},
    {"HLAmanager.HLAfederate.HLAadjust.HLAmodifyAttributeState",
     {MomAction::Subsribe, {"HLAobjectInstance", "HLAattribute", "HLAattributeState"}}},
    {"HLAmanager.HLAfederate.HLAadjust.HLAsetSwitches",
     {MomAction::Subsribe,
      {"HLAconveyRegionDesignatorSets", "HLAconveyProducingFederate", "HLAserviceReporting", "HLAexceptionReporting"}}},
    {"HLAmanager.HLAfederate.HLArequest", {MomAction::Nothing, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLArequest.HLArequestPublications", {MomAction::Subsribe, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLArequest.HLArequestSubscriptions", {MomAction::Subsribe, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesThatCanBeDeleted",
     {MomAction::Subsribe, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesUpdated",
     {MomAction::Subsribe, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesReflected",
     {MomAction::Subsribe, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLArequest.HLArequestUpdatesSent", {MomAction::Subsribe, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLArequest.HLArequestInteractionsSent", {MomAction::Subsribe, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLArequest.HLArequestReflectionsReceived", {MomAction::Subsribe, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLArequest.HLArequestInteractionsReceived",
     {MomAction::Subsribe, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstanceInformation",
     {MomAction::Subsribe, {"HLAobjectInstance"}}},
    {"HLAmanager.HLAfederate.HLArequest.HLArequestFOMmoduleData", {MomAction::Subsribe, {"HLAFOMmoduleIndicator"}}},
    {"HLAmanager.HLAfederate.HLAreport", {MomAction::Nothing, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLAreport.HLAreportObjectClassPublication",
     {MomAction::Publish, {"HLAnumberOfClasses", "HLAobjectClass", "HLAattributeList"}}},
    {"HLAmanager.HLAfederate.HLAreport.HLAreportObjectClassSubscription",
     {MomAction::Publish,
      {"HLAnumberOfClasses", "HLAobjectClass", "HLAactive", "HLAmaxUpdateRate", "HLAattributeList"}}},
    {"HLAmanager.HLAfederate.HLAreport.HLAreportInteractionPublication",
     {MomAction::Publish, {"HLAinteractionClassList"}}},
    {"HLAmanager.HLAfederate.HLAreport.HLAreportInteractionSubscription",
     {MomAction::Publish, {"HLAinteractionClassList"}}},
    {"HLAmanager.HLAfederate.HLAreport.HLAreportObjectInstancesThatCanBeDeleted",
     {MomAction::Publish, {"HLAobjectInstanceCounts"}}},
    {"HLAmanager.HLAfederate.HLAreport.HLAreportObjectInstancesUpdated",
     {MomAction::Publish, {"HLAobjectInstanceCounts"}}},
    {"HLAmanager.HLAfederate.HLAreport.HLAreportObjectInstancesReflected",
     {MomAction::Publish, {"HLAobjectInstanceCounts"}}},
    {"HLAmanager.HLAfederate.HLAreport.HLAreportUpdatesSent",
     {MomAction::Publish, {"HLAtransportation", "HLAupdateCounts"}}},
    {"HLAmanager.HLAfederate.HLAreport.HLAreportReflectionsReceived",
     {MomAction::Publish, {"HLAtransportation", "HLAreflectCounts"}}},
    {"HLAmanager.HLAfederate.HLAreport.HLAreportInteractionsSent",
     {MomAction::Publish, {"HLAtransportation", "HLAinteractionCounts"}}},
    {"HLAmanager.HLAfederate.HLAreport.HLAreportInteractionsReceived",
     {MomAction::Publish, {"HLAtransportation", "HLAinteractionCounts"}}},
    {"HLAmanager.HLAfederate.HLAreport.HLAreportObjectInstanceInformation",
     {MomAction::Publish,
      {"HLAobjectInstance", "HLAownedInstanceAttributeList", "HLAregisteredClass", "HLAknownClass"}}},
    {"HLAmanager.HLAfederate.HLAreport.HLAreportException", {MomAction::Publish, {"HLAservice", "HLAexception"}}},
    {"HLAmanager.HLAfederate.HLAreport.HLAreportServiceInvocation",
     {MomAction::Publish,
      {"HLAservice",
       "HLAsuccessIndicator",
       "HLAsuppliedArguments",
       "HLAreturnedArguments",
       "HLAexception",
       "HLAserialNumber"}}},
    {"HLAmanager.HLAfederate.HLAreport.HLAreportMOMexception",
     {MomAction::Publish, {"HLAservice", "HLAexception", "HLAparameterError"}}},
    {"HLAmanager.HLAfederate.HLAreport.HLAreportFederateLost",
     {MomAction::Publish, {"HLAfederateName", "HLAtimestamp", "HLAfaultDescription"}}},
    {"HLAmanager.HLAfederate.HLAreport.HLAreportFOMmoduleData",
     {MomAction::Publish, {"HLAFOMmoduleIndicator", "HLAFOMmoduleData"}}},
    {"HLAmanager.HLAfederate.HLAservice", {MomAction::Nothing, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAresignFederationExecution", {MomAction::Subsribe, {"HLAresignAction"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAsynchronizationPointAchieved", {MomAction::Subsribe, {"HLAlabel"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAfederateSaveBegun", {MomAction::Subsribe, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAfederateSaveComplete", {MomAction::Subsribe, {"HLAsuccessIndicator"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAfederateRestoreComplete", {MomAction::Subsribe, {"HLAsuccessIndicator"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLApublishObjectClassAttributes",
     {MomAction::Subsribe, {"HLAobjectClass", "HLAattributeList"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAunpublishObjectClassAttributes",
     {MomAction::Subsribe, {"HLAobjectClass", "HLAattributeList"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLApublishInteractionClass", {MomAction::Subsribe, {"HLAinteractionClass"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAunpublishInteractionClass", {MomAction::Subsribe, {"HLAinteractionClass"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAsubscribeObjectClassAttributes",
     {MomAction::Subsribe, {"HLAobjectClass", "HLAattributeList", "HLAactive"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAunsubscribeObjectClassAttributes",
     {MomAction::Subsribe, {"HLAobjectClass", "HLAattributeList"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAsubscribeInteractionClass",
     {MomAction::Subsribe, {"HLAinteractionClass", "HLAactive"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAunsubscribeInteractionClass",
     {MomAction::Subsribe, {"HLAinteractionClass"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAdeleteObjectInstance",
     {MomAction::Subsribe, {"HLAobjectInstance", "HLAtag", "HLAtimeStamp"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAlocalDeleteObjectInstance", {MomAction::Subsribe, {"HLAobjectInstance"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLArequestAttributeTransportationTypeChange",
     {MomAction::Subsribe, {"HLAobjectInstance", "HLAattributeList", "HLAtransportation"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLArequestInteractionTransportationTypeChange",
     {MomAction::Subsribe, {"HLAinteractionClass", "HLAtransportation"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAunconditionalAttributeOwnershipDivestiture",
     {MomAction::Subsribe, {"HLAobjectInstance", "HLAattributeList"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAenableTimeRegulation", {MomAction::Subsribe, {"HLAlookahead"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAdisableTimeRegulation", {MomAction::Subsribe, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAenableTimeConstrained", {MomAction::Subsribe, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAdisableTimeConstrained", {MomAction::Subsribe, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAtimeAdvanceRequest", {MomAction::Subsribe, {"HLAtimeStamp"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAtimeAdvanceRequestAvailable", {MomAction::Subsribe, {"HLAtimeStamp"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAnextMessageRequest", {MomAction::Subsribe, {"HLAtimeStamp"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAnextMessageRequestAvailable", {MomAction::Subsribe, {"HLAtimeStamp"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAflushQueueRequest", {MomAction::Subsribe, {"HLAtimeStamp"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAenableAsynchronousDelivery", {MomAction::Subsribe, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAdisableAsynchronousDelivery",
     {MomAction::Subsribe, std::set<std::string>{}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAmodifyLookahead", {MomAction::Subsribe, {"HLAlookahead"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAchangeAttributeOrderType",
     {MomAction::Subsribe, {"HLAobjectInstance", "HLAattributeList", "HLAsendOrder"}}},
    {"HLAmanager.HLAfederate.HLAservice.HLAchangeInteractionOrderType",
     {MomAction::Subsribe, {"HLAinteractionClass", "HLAsendOrder"}}},
    {"HLAmanager.HLAfederation", {MomAction::Nothing, std::set<std::string>{}}},
    {"HLAmanager.HLAfederation.HLAadjust", {MomAction::Nothing, std::set<std::string>{}}},
    {"HLAmanager.HLAfederation.HLAadjust.HLAsetSwitches", {MomAction::Subsribe, {"HLAautoProvide"}}},
    {"HLAmanager.HLAfederation.HLArequest", {MomAction::Nothing, std::set<std::string>{}}},
    {"HLAmanager.HLAfederation.HLArequest.HLArequestSynchronizationPoints",
     {MomAction::Subsribe, std::set<std::string>{}}},
    {"HLAmanager.HLAfederation.HLArequest.HLArequestSynchronizationPointStatus",
     {MomAction::Subsribe, {"HLAsyncPointName"}}},
    {"HLAmanager.HLAfederation.HLArequest.HLArequestFOMmoduleData", {MomAction::Subsribe, {"HLAFOMmoduleIndicator"}}},
    {"HLAmanager.HLAfederation.HLArequest.HLArequestMIMData", {MomAction::Subsribe, std::set<std::string>{}}},
    {"HLAmanager.HLAfederation.HLAreport", {MomAction::Nothing, std::set<std::string>{}}},
    {"HLAmanager.HLAfederation.HLAreport.HLAreportSynchronizationPoints", {MomAction::Publish, {"HLAsyncPoints"}}},
    {"HLAmanager.HLAfederation.HLAreport.HLAreportSynchronizationPointStatus",
     {MomAction::Publish, {"HLAsyncPointName", "HLAsyncPointFederates"}}},
    {"HLAmanager.HLAfederation.HLAreport.HLAreportFOMmoduleData",
     {MomAction::Publish, {"HLAFOMmoduleIndicator", "HLAFOMmoduleData"}}},
    {"HLAmanager.HLAfederation.HLAreport.HLAreportMIMData", {MomAction::Publish, {"HLAMIMData"}}}};
}

namespace certi {
namespace rtig {

static PrettyDebug D("MOM", __FILE__);

bool Mom::isAvailableInRootObjectAndCompliant(const RootObject& root)
{
    // Pre check with sizes
    if (root.ObjectClasses->size() < the_required_object_classes.size()
        or root.Interactions->size() < the_required_interactions.size()) {
        Debug(D, pdDebug)
            << "the root object does not contain enough object classes or interactions to have a mom definition."
            << endl;
        return false;
    }

    // Chech for every classes and each attributes
    for (auto& pair : the_required_object_classes) {
        try {
            auto objectHandle = root.ObjectClasses->getHandleFromName(pair.first);

            for (const auto& attributeName : pair.second.second) {
                try {
                    root.ObjectClasses->getAttributeHandle(attributeName, objectHandle);
                }
                catch (NameNotFound& e) {
                    Debug(D, pdDebug) << "the object class \"" << pair.first << "\" does not contain the attribute "
                                      << attributeName << endl;
                    return false;
                }
            }
        }
        catch (NameNotFound& e) {
            Debug(D, pdDebug) << "the root object does not contain the object class " << pair.first << endl;
            return false;
        }
    }

    // Chech for every interactions and each parameters
    for (auto& pair : the_required_interactions) {
        try {
            auto objectHandle = root.Interactions->getInteractionClassHandle(pair.first);

            for (const auto& parameterName : pair.second.second) {
                try {
                    root.Interactions->getParameterHandle(parameterName, objectHandle);
                }
                catch (NameNotFound& e) {
                    Debug(D, pdDebug) << "the interaction class \"" << pair.first
                                      << "\" does not contain the parameter " << parameterName << endl;
                    return false;
                }
            }
        }
        catch (NameNotFound& e) {
            Debug(D, pdDebug) << "the root object does not contain the interaction class " << pair.first << endl;
            return false;
        }
    }

    return true;
}

Mom::Mom(const FederateHandle handle, Federation& federation, RootObject& root)
    : my_handle(handle), my_federation(federation), my_root(root)
{
}

FederateHandle Mom::getHandle() const
{
    return my_handle;
}

void Mom::publishObjects()
{
    Debug(D, pdGendoc) << "enter Mom::publishObjects" << endl;

    for (const auto& pair : the_required_object_classes) {
        if (pair.second.first != MomAction::Publish) {
            continue;
        }

        auto object_handle = my_root.ObjectClasses->getHandleFromName(pair.first);
        my_object_class_cache.insert(std::make_pair(pair.first, object_handle));

        // we gather only the attributes specified in the standard
        std::vector<AttributeHandle> attributes;
        for (const std::string& attributeName : pair.second.second) {
            auto attribute_handle = my_root.ObjectClasses->getAttributeHandle(attributeName, object_handle);
            my_attribute_cache.insert(std::make_pair(pair.first + "." + attributeName, attribute_handle));

            attributes.push_back(attribute_handle);
        }

        my_federation.publishObject(my_handle, object_handle, attributes, true);
    }

    Debug(D, pdGendoc) << "exit  Mom::publishObjects" << endl;
}

void Mom::publishAndSubscribeInteractions()
{
    Debug(D, pdGendoc) << "enter Mom::publishAndSubscribeInteractions" << endl;

    for (const auto& pair : the_required_interactions) {
        if (pair.second.first == MomAction::Nothing) {
            continue;
        }

        auto interaction_handle = my_root.Interactions->getInteractionClassHandle(pair.first);
        my_interaction_class_cache.insert(std::make_pair(pair.first, interaction_handle));

        if (pair.second.first == MomAction::Publish) {
            my_federation.publishInteraction(my_handle, interaction_handle, true);
        }
        else { // we already discarded MomAction::Nothing so there is only MomAction::subscribe left
            my_federation.subscribeInteraction(my_handle, interaction_handle, true);
        }
    }

    Debug(D, pdGendoc) << "exit  Mom::publishAndSubscribeInteractions" << endl;
}

void Mom::display() const
{
    std::cout << "my_attribute_values_cache; " << std::endl;
    for (const auto& pair : my_attribute_values_cache) {
        for (const auto& attribute : pair.second) {
            std::cout << "\t<" << pair.first << "." << attribute.first << "> " << attribute.second.size() << "{ ";
            for (const auto& byte : attribute.second) {
                std::cout << byte << "(" << static_cast<int>(byte) << ") ";
            }
            std::cout << "}" << std::endl;
        }
    }
}

AttributeValue_t Mom::encodeString(const std::string& str)
{
    mb.reset();

    // uint32 : message lenght
    mb.write_uint32(sizeof(uint32_t) + str.size());
    // uint32 : cardinality
    // char*size : data
    mb.write_string(str);
    return encodeMB();
}

AttributeValue_t Mom::encodeStringList(const std::vector<std::string>& strs)
{
    mb.reset();
    
    // write overall size
    int overall_size = sizeof(uint32_t);
    for(const auto& str: strs) {
        overall_size += sizeof(uint32_t);
        const auto remainder = (str.size() % 4);
        const auto padding_size = ((remainder == 0) ? (0) : (4 - remainder));
        overall_size += str.size() + padding_size; // size and padding
    }
    mb.write_uint32(overall_size);
    
    // write collection size
    mb.write_uint32(strs.size());
    
    for(const auto& str: strs) {
        mb.write_string(str);
        const auto remainder = (str.size() % 4);
        if(remainder != 0) {
            for(auto i(0u); i < 4 - remainder; ++i) {
                mb.write_char('\0');
            }
        }
    }
    
    return encodeMB();
}

AttributeValue_t Mom::encodeBoolean(const bool data)
{
    return encodeUInt32(data ? 1 : 0);
}

AttributeValue_t Mom::encodeUInt32(const uint32_t data)
{
    mb.reset();

    // uint32 : message lenght
    mb.write_uint32(sizeof(uint32_t));

    // uint32 : data
    mb.write_uint32(data);

    return encodeMB();
}

AttributeValue_t Mom::encodeVersion(const RtiVersion version)
{
    switch(version) {
        case IEEE_1516_2000:
            return encodeString("ieee 1516-2000");
        case IEEE_1516_2010:
            return encodeString("ieee 1516-2010");
        case HLA_1_3:
        default:
            return encodeString("hla 1.3");
    }
}

AttributeValue_t Mom::encodeTime(const FederationTime& time)
{
    mb.reset();
    
    // uint32 : message lenght
    mb.write_uint32(sizeof(double));
    
    // uint32 : data
    mb.write_double(time.getTime());
    
    return encodeMB();
}

AttributeValue_t Mom::encodeIp(const unsigned long ip)
{
    mb.reset();
    mb.write_uint32(ip);

    std::string str;

    for (auto i = 0; i < 4; ++i) {
        if (i != 0) {
            str += ".";
        }
        str += std::to_string(mb.read_char());
    }

    return encodeString(str);
}

AttributeValue_t Mom::encodeFederateHandleList()
{
    mb.reset();

    // uint32 : message lenght
    mb.write_uint32(sizeof(uint32_t) * (1 + my_federation.getNbFederates()));

    // uint32 : cardinality
    mb.write_uint32(my_federation.getNbFederates());

    // uint32=size : data
    for (const auto& federate : my_federation.my_federates) {
        mb.write_uint32(federate.first);
    }

    return encodeMB();
}

AttributeValue_t Mom::encodeFederateState(const Federate& federate)
{
    if (federate.isSaving()) {
        return encodeUInt32(3);
    }
    else if (federate.isRestoring()) {
        return encodeUInt32(5);
    }
    else {
        return encodeUInt32(1);
    }
}

AttributeValue_t Mom::encodeVectorHandle(const std::vector<Handle>& data)
{
    mb.reset();

    // uint32 : message lenght
    mb.write_uint32(sizeof(uint32_t) * (1 + data.size()));

    // uint32 : cardinality
    mb.write_uint32(data.size());

    // uint32=size : data
    for (const auto& handle : data) {
        mb.write_uint32(handle);
    }

    return encodeMB();
}

AttributeValue_t Mom::encodeHandleBasedCounts(std::map<Handle, int> data)
{
    mb.reset();

    // uint32 : message lenght
    mb.write_uint32(sizeof(uint32_t) * (1 + (data.size() * 2)));

    // uint32 : cardinality
    mb.write_uint32(data.size());

    // uint32=size : data
    for (const auto& pair : data) {
        mb.write_uint32(pair.first);
        mb.write_uint32(pair.second);
    }

    return encodeMB();
}

AttributeValue_t Mom::encodeVectorString(const std::vector<std::string>& data)
{
    mb.reset();

    // uint32 : message lenght
    // = 1 uint for list cardinality
    // + data.size() uint per string for its size
    // + accumulate(data).size char per string char
    mb.write_uint32(sizeof(uint32_t) * (1 + data.size())
                    + sizeof(char)
                        * std::accumulate(begin(data), end(data), 0, [](const int totalSize, const std::string& str) {
                              return totalSize + str.size();
                          }));

    // uint32 : cardinality
    mb.write_uint32(data.size());

    // uint32=size : data
    for (const std::string& str : data) {
        mb.write_string(str);
    }

    return encodeMB();
}

std::string Mom::decodeString(const ParameterValue_t& data)
{
    mb.reset();
    mb.write_bytes(&(data[0]), data.size());

    return mb.read_string();
}

bool Mom::decodeBoolean(const ParameterValue_t& data)
{
    mb.reset();
    mb.write_bytes(&(data[0]), data.size());

    return mb.read_bool();
}

uint32_t Mom::decodeUInt32(const ParameterValue_t& data)
{
    mb.reset();
    mb.write_bytes(&(data[0]), data.size());

    return mb.read_uint32();
}

Mom::ResignAction Mom::decodeResignAction(const ParameterValue_t& data)
{
    switch (decodeUInt32(data)) {
    case 1:
        return ResignAction::DivestOwnership;
    case 2:
        return ResignAction::DeleteObjectInstances;
    case 3:
        return ResignAction::CancelPendingAcquisitions;
    case 4:
        return ResignAction::DeleteObjectInstancesThenDivestOwnership;
    case 5:
        return ResignAction::CancelPendingAcquisitionsThenDeleteObjectInstancesThenDivestOwnership;
    default:
        return ResignAction::NoActions;
    }
}

std::vector<AttributeHandle> Mom::decodeVectorAttributeHandle(const ParameterValue_t& data)
{
    mb.reset();
    mb.write_bytes(&(data[0]), data.size());

    std::vector<AttributeHandle> handles;
    for (uint32_t i(0u); i < mb.read_uint32(); ++i) {
        handles.push_back(mb.read_uint32());
    }

    return handles;
}

FederationTime Mom::decodeFederationTime(const ParameterValue_t& data)
{
    mb.reset();
    mb.write_bytes(&(data[0]), data.size());
    
    return FederationTime(mb.read_double());
}

Mom::OrderType Mom::decodeOrderType(const ParameterValue_t& data)
{
    switch (decodeUInt32(data)) {
    case 1:
        return OrderType::TimeStamp;
    case 0:
    default:
        return OrderType::Receive;
    }
}

AttributeValue_t Mom::encodeMB()
{
    AttributeValue_t value;
    value.resize(mb.read_uint32());
    mb.read_bytes(&(value[0]), value.size());
    return value;
}
}
}
