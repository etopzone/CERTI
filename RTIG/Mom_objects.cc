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

#include <chrono>
#include <ctime>
#include <iomanip>
#include <libCERTI/PrettyDebug.hh>
#include <libCERTI/NM_Classes.hh>

#include "make_unique.hh"

using std::cout;
using std::endl;

namespace certi {
namespace rtig {

static PrettyDebug D("MOM", __FILE__);

Responses Mom::registerFederation()
{
    Responses responses;

    auto objectName = "Federation_" + my_federation.getName();

    my_federation.reserveObjectInstanceName(my_handle, objectName);

    std::tie(my_federation_object, responses) = my_federation.registerObject(
        my_handle, my_object_class_cache["HLAmanager.HLAfederation"], objectName); // discard responses

    std::vector<AttributeHandle> attributes;

    AttributeHandle attribute;

    // Static
    attribute = my_attribute_cache["HLAmanager.HLAfederation.HLAfederationName"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeString(my_federation.getName());

    // Conditional
    attribute = my_attribute_cache["HLAmanager.HLAfederation.HLAfederatesInFederation"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeFederateHandleList();

    // Static
    attribute = my_attribute_cache["HLAmanager.HLAfederation.HLARTIversion"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeVersion(my_federation.getRtiVersion());

    // Static
    attribute = my_attribute_cache["HLAmanager.HLAfederation.HLAMIMDesignator"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeString(my_federation.getMimModule());

    // Conditional
    attribute = my_attribute_cache["HLAmanager.HLAfederation.HLAFOMmoduleDesignatorList"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeStringList(my_federation.getFomModules());

    // Conditional
    attribute = my_attribute_cache["HLAmanager.HLAfederation.HLAcurrentFDD"];
    attributes.push_back(attribute);
    std::string currentFDD {""};
    for(const auto& module: my_federation.getFomModules()) {
        currentFDD += '"' + module + "\" && ";
    }
    currentFDD = currentFDD.substr(0, currentFDD.size() - 4); // remove last " && "
    my_attribute_values_cache[my_federation_object][attribute] = encodeString(currentFDD);

    // Static
    attribute = my_attribute_cache["HLAmanager.HLAfederation.HLAtimeImplementationName"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeString("HLAfloat64Time");

    // Conditional
    attribute = my_attribute_cache["HLAmanager.HLAfederation.HLAlastSaveName"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeString("");

    // Conditional
    attribute = my_attribute_cache["HLAmanager.HLAfederation.HLAlastSaveTime"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeUInt32(0);

    // Conditional
    attribute = my_attribute_cache["HLAmanager.HLAfederation.HLAnextSaveName"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeUInt32(0);

    // Conditional
    attribute = my_attribute_cache["HLAmanager.HLAfederation.HLAnextSaveTime"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeString("");

    // Conditional
    attribute = my_attribute_cache["HLAmanager.HLAfederation.HLAautoProvide"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeBoolean(my_federation.isAutoProvideActive());

    auto resp = provideAttributeValueUpdate(my_federation_object, attributes);
    responses.insert(end(responses), make_move_iterator(begin(resp)), make_move_iterator(end(resp)));

    return responses;
}

Responses Mom::registerFederate(const Federate& federate, SocketTCP* tcp_link, const std::vector<std::string>& additional_fom_modules)
{
    Responses responses;
    auto objectName = "Federate_" + federate.getName();

    my_federation.reserveObjectInstanceName(my_handle, objectName);

    ObjectHandle federate_object;
    std::tie(federate_object, responses)
        = my_federation.registerObject(my_handle, my_object_class_cache["HLAmanager.HLAfederate"], objectName);
    my_federate_objects.insert(std::make_pair(federate.getHandle(), federate_object));

    std::vector<AttributeHandle> attributes;

    AttributeHandle attribute;

    // Static
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAfederateHandle"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(federate.getHandle());

    // Static
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAfederateName"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeString(federate.getName());

    // Static
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAfederateType"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeString(federate.getType());

    // Static
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAfederateHost"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute]
        = tcp_link ? encodeIp(tcp_link->returnAdress()) : encodeString("");

    // Static
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLARTIversion"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeVersion(federate.getRtiVersion());

    // Static
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAFOMmoduleDesignatorList"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeStringList(additional_fom_modules);

    // Conditional
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAtimeConstrained"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeBoolean(federate.isConstrained());

    // Conditional
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAtimeRegulating"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeBoolean(federate.isRegulator());

    // Conditional
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAasynchronousDelivery"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeBoolean(false);

    // Conditional
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAfederateState"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeFederateState(federate);

    // Conditional
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAtimeManagerState"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeBoolean(false);

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAlogicalTime"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeTime({0.0});

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAlookahead"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeTime({0.0});

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAGALT"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeTime({0.0});

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLALITS"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeTime({0.0});

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAROlength"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLATSOlength"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAreflectionsReceived"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAupdatesSent"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAinteractionsReceived"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAinteractionsSent"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAobjectInstancesThatCanBeDeleted"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAobjectInstancesUpdated"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAobjectInstancesReflected"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAobjectInstancesDeleted"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAobjectInstancesRemoved"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAobjectInstancesRegistered"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAobjectInstancesDiscovered"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAtimeGrantedTime"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeTime({0.0});

    // Periodic
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAtimeAdvancingTime"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeTime({0.0});

    // Conditional
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAconveyRegionDesignatorSets"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute]
        = encodeBoolean(federate.isConveyRegionDesignatorSetsSwitch());

    // Conditional
    attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAconveyProducingFederate"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeBoolean(federate.isConveyProducingFederateSwitch());

    auto resp = provideAttributeValueUpdate(federate_object, attributes);
    responses.insert(end(responses), make_move_iterator(begin(resp)), make_move_iterator(end(resp)));
    
    auto mom_msg = make_unique<NM_Mom_Status>();
    mom_msg->setFederation(my_federation.getHandle().get());
    mom_msg->setFederate(federate.getHandle());
    mom_msg->setMomState(true);
    mom_msg->setUpdatePeriod(my_federates_update_settings[federate.getHandle()].updateRate.count());
    
    responses.emplace_back(tcp_link, std::move(mom_msg));

    Debug(D, pdGendoc) << "exit  Mom::registerFederate" << endl;
    return responses;
}

void Mom::deleteFederate(const FederateHandle federate_handle)
{
    auto federate_object = my_federate_objects[federate_handle];
    my_federation.deleteObject(my_federation_object, federate_object, "");

    my_federate_objects.erase(federate_handle);
    my_attribute_values_cache.erase(federate_handle);
}

Responses Mom::updateFederatesInFederation()
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederation.HLAfederatesInFederation"];

    my_attribute_values_cache[my_federation_object][attribute] = encodeFederateHandleList();

    return provideAttributeValueUpdate(my_federation_object, {attribute});
}

Responses Mom::updateFomModuleDesignatorList()
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederation.HLAFOMmoduleDesignatorList"];
    
    const auto encoded_value = encodeStringList(my_federation.getFomModules());

    if(my_attribute_values_cache[my_federation_object][attribute] != encoded_value) {
        my_attribute_values_cache[my_federation_object][attribute] = encoded_value;
        
        return provideAttributeValueUpdate(my_federation_object, {attribute});
    }
    return {};
}

Responses Mom::updateCurrentFDD()
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederation.HLAcurrentFDD"];
    
    std::string currentFDD {""};
    for(const auto& module: my_federation.getFomModules()) {
        currentFDD += '"' + module + "\" && ";
    }
    currentFDD = currentFDD.substr(0, currentFDD.size() - 4); // remove last " && "
    
    const auto encoded_value = encodeString(currentFDD);

    if(my_attribute_values_cache[my_federation_object][attribute] != encoded_value) {
        my_attribute_values_cache[my_federation_object][attribute] = encoded_value;
        
        return provideAttributeValueUpdate(my_federation_object, {attribute});
    }
    return {};
}

Responses Mom::updateLastSave()
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederation.HLAlastSaveName"];

    my_attribute_values_cache[my_federation_object][attribute]
        = my_attribute_values_cache[my_federation_object][my_attribute_cache["HLAmanager.HLAfederation.HLAnextSaveName"]];

    auto attribute2 = my_attribute_cache["HLAmanager.HLAfederation.HLAlastSaveTime"];

    my_attribute_values_cache[my_federation_object][attribute]
        = my_attribute_values_cache[my_federation_object][my_attribute_cache["HLAmanager.HLAfederation.HLAnextSaveTime"]];

    return provideAttributeValueUpdate(my_federation_object, {attribute, attribute2});
}

Responses Mom::updateNextSave(const std::string& label, const FederationTime& time)
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederation.HLAnextSaveName"];

    my_attribute_values_cache[my_federation_object][attribute] = encodeString(label);

    auto attribute2 = my_attribute_cache["HLAmanager.HLAfederation.HLAnextSaveTime"];

    my_attribute_values_cache[my_federation_object][attribute] = encodeTime(time);
    
    return provideAttributeValueUpdate(my_federation_object, {attribute, attribute2});
}

Responses Mom::updateAutoProvide(const bool value)
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederation.HLAautoProvide"];
    
    const auto encoded_value = encodeBoolean(value);

    if(my_attribute_values_cache[my_federation_object][attribute] != encoded_value) {
        my_attribute_values_cache[my_federation_object][attribute] = encoded_value;
        
        return provideAttributeValueUpdate(my_federation_object, {attribute});
    }
    return {};
}

Responses Mom::updateTimeConstrained(const Federate& federate)
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAtimeConstrained"];

    auto object = my_federate_objects[federate.getHandle()];
    
    const auto encoded_value = encodeBoolean(federate.isConstrained());

    if(my_attribute_values_cache[object][attribute] != encoded_value) {
        my_attribute_values_cache[object][attribute] = encoded_value;
        
        return provideAttributeValueUpdate(object, {attribute});
    }
    return {};
}

Responses Mom::updateTimeRegulating(const Federate& federate)
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAtimeRegulating"];

    auto object = my_federate_objects[federate.getHandle()];
    
    const auto encoded_value = encodeBoolean(federate.isRegulator());

    if(my_attribute_values_cache[object][attribute] != encoded_value) {
        my_attribute_values_cache[object][attribute] = encoded_value;
        
        return provideAttributeValueUpdate(object, {attribute});
    }
    return {};
}

Responses Mom::updateAsynchronousDelivery(const FederateHandle federate_handle, const bool value)
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAasynchronousDelivery"];

    auto object = my_federate_objects[federate_handle];
    
    const auto encoded_value = encodeBoolean(value);

    if(my_attribute_values_cache[object][attribute] != encoded_value) {
        my_attribute_values_cache[object][attribute] = encoded_value;
        
        return provideAttributeValueUpdate(object, {attribute});
    }
    return {};
}

Responses Mom::updateFederateState(const Federate& federate)
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAfederateState"];

    auto object = my_federate_objects[federate.getHandle()];
    
    const auto encoded_value = encodeFederateState(federate);

    if(my_attribute_values_cache[object][attribute] != encoded_value) {
        my_attribute_values_cache[object][attribute] = encoded_value;
        
        return provideAttributeValueUpdate(object, {attribute});
    }
    return {};
}

Responses Mom::updateTimeManagerState(const FederateHandle federate_handle, const TimeState value)
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAtimeManagerState"];

    auto object = my_federate_objects[federate_handle];
    
    const auto encoded_value = encodeBoolean(value == TimeState::TimeAdvancing);

    if(my_attribute_values_cache[object][attribute] != encoded_value) {
        my_attribute_values_cache[object][attribute] = encoded_value;
        
        return provideAttributeValueUpdate(object, {attribute});
    }
    return {};
}

Responses Mom::updateConveyRegionDesignatorSets(const FederateHandle federate_handle, const bool value)
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAconveyRegionDesignatorSets"];

    auto object = my_federate_objects[federate_handle];
    
    const auto encoded_value = encodeBoolean(value);

    if(my_attribute_values_cache[object][attribute] != encoded_value) {
        my_attribute_values_cache[object][attribute] = encoded_value;
        
        return provideAttributeValueUpdate(object, {attribute});
    }
    return {};
}

Responses Mom::updateConveyProducingFederate(const FederateHandle federate_handle, const bool value)
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAconveyProducingFederate"];

    auto object = my_federate_objects[federate_handle];
    
    const auto encoded_value = encodeBoolean(value);

    if(my_attribute_values_cache[object][attribute] != encoded_value) {
        my_attribute_values_cache[object][attribute] = encoded_value;
        
        return provideAttributeValueUpdate(object, {attribute});
    }
    return {};
}

Responses Mom::updateLogicalTime(const FederateHandle federate_handle, const FederationTime& value)
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAlogicalTime"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeTime(value);

    return preparePeriodicAttributeValueUpdate(federate_handle, {attribute});
}

Responses Mom::updateLookahead(const FederateHandle federate_handle, const FederationTime& value)
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAlookahead"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeTime(value);

    return preparePeriodicAttributeValueUpdate(federate_handle, {attribute});
}

Responses Mom::updateGALT(const FederateHandle federate_handle, const FederationTime& value)
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAGALT"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeTime(value);

    return preparePeriodicAttributeValueUpdate(federate_handle, {attribute});
}

Responses Mom::updateLITS(const FederateHandle federate_handle, const FederationTime& value)
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederate.HLALITS"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeTime(value);

    return preparePeriodicAttributeValueUpdate(federate_handle, {attribute});
}

Responses Mom::updateRoLenght(const FederateHandle federate_handle, const int delta)
{
    return updateCountAttribute(federate_handle, "HLAmanager.HLAfederate.HLAROlength", delta);
}

Responses Mom::updateTsoLenght(const FederateHandle federate_handle, const int delta)
{
    return updateCountAttribute(federate_handle, "HLAmanager.HLAfederate.HLATSOlength", delta);
}

Responses Mom::updateReflectionsReceived(const FederateHandle federate_handle, const int delta)
{
    return updateCountAttribute(federate_handle, "HLAmanager.HLAfederate.HLAreflectionsReceived", delta);
}

Responses Mom::updateUpdatesSent(const FederateHandle federate_handle, const int delta)
{
    return updateCountAttribute(federate_handle, "HLAmanager.HLAfederate.HLAupdatesSent", delta);
}

Responses Mom::updateInteractionsReceived(const FederateHandle federate_handle, const int delta)
{
    return updateCountAttribute(federate_handle, "HLAmanager.HLAfederate.HLAinteractionsReceived", delta);
}

Responses Mom::updateInteractionsSent(const FederateHandle federate_handle, const int delta)
{
    return updateCountAttribute(federate_handle, "HLAmanager.HLAfederate.HLAinteractionsSent", delta);
}

Responses Mom::updateObjectInstancesThatCanBeDeleted(const FederateHandle federate_handle, const int delta)
{
    return updateCountAttribute(federate_handle, "HLAmanager.HLAfederate.HLAobjectInstancesThatCanBeDeleted", delta);
}

Responses Mom::updateObjectInstancesUpdated(const FederateHandle federate_handle)
{
    int count = 0;

    for (const auto& pair : my_object_instances_updated[federate_handle]) {
        count += pair.second.size();
    }

    auto attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAobjectInstancesUpdated"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeUInt32(count);

    return preparePeriodicAttributeValueUpdate(federate_handle, {attribute});
}

Responses Mom::updateObjectInstancesReflected(const FederateHandle federate_handle)
{
    int count = 0;

    for (const auto& pair : my_object_instances_reflected[federate_handle]) {
        count += pair.second.size();
    }

    auto attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAobjectInstancesReflected"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeUInt32(count);

    return preparePeriodicAttributeValueUpdate(federate_handle, {attribute});
}

Responses Mom::updateObjectInstancesDeleted(const FederateHandle federate_handle, const int delta)
{
    return updateCountAttribute(federate_handle, "HLAmanager.HLAfederate.HLAobjectInstancesDeleted", delta);
}

Responses Mom::updateObjectInstancesRemoved(const FederateHandle federate_handle, const int delta)
{
    return updateCountAttribute(federate_handle, "HLAmanager.HLAfederate.HLAobjectInstancesRemoved", delta);
}

Responses Mom::updateObjectInstancesRegistered(const FederateHandle federate_handle, const int delta)
{
    return updateCountAttribute(federate_handle, "HLAmanager.HLAfederate.HLAobjectInstancesRegistered", delta);
}

Responses Mom::updateObjectInstancesDiscovered(const FederateHandle federate_handle, const int delta)
{
    return updateCountAttribute(federate_handle, "HLAmanager.HLAfederate.HLAobjectInstancesDiscovered", delta);
}

Responses Mom::updateTimeGrantedTime(const FederateHandle federate_handle, const int value)
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAtimeGrantedTime"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeUInt32(value);

    return preparePeriodicAttributeValueUpdate(federate_handle, {attribute});
}

Responses Mom::updateTimeAdvancingTime(const FederateHandle federate_handle, const int value)
{
    auto attribute = my_attribute_cache["HLAmanager.HLAfederate.HLAtimeAdvancingTime"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeUInt32(value);

    return preparePeriodicAttributeValueUpdate(federate_handle, {attribute});
}

Responses
Mom::updateCountAttribute(const FederateHandle& federate_handle, const std::string& attribute_name, const int delta)
{
    auto attribute = my_attribute_cache[attribute_name];

    my_count_attributes_cache[federate_handle][attribute] += delta;

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute]
        = encodeUInt32(my_count_attributes_cache[federate_handle][attribute]);

    return preparePeriodicAttributeValueUpdate(federate_handle, {attribute});
}

Responses Mom::provideAttributeValueUpdate(const ObjectHandle& object, const std::vector<AttributeHandle>& attributes)
{
    Debug(D, pdGendoc) << "enter Mom::provideAttributeValueUpdate" << endl;

    std::vector<AttributeValue_t> values;

    for (const auto& attribute : attributes) {
        values.push_back(my_attribute_values_cache[object][attribute]);
    }

    auto responses = my_federation.updateAttributeValues(my_handle, object, attributes, values, "");

    Debug(D, pdGendoc) << "exit  Mom::provideAttributeValueUpdate" << endl;

    return responses;
}

Responses Mom::preparePeriodicAttributeValueUpdate(const FederateHandle& federate,
                                                   const std::vector<AttributeHandle>& attributes)
{
    Debug(D, pdGendoc) << "enter Mom::preparePeriodicAttributeValueUpdate" << endl;

    if (federate == my_handle) {
        return Responses();
    }

    for (const auto& attribute : attributes) {
        my_attributes_to_update_periodically[federate].insert(attribute);
    }

    auto responses = providePeriodicAttributeValueUpdatesIfApplicable();

    Debug(D, pdGendoc) << "exit  Mom::preparePeriodicAttributeValueUpdate" << endl;

    return responses;
}

Responses Mom::providePeriodicAttributeValueUpdatesIfApplicable()
{
    Debug(D, pdGendoc) << "enter Mom::providePeriodicAttributeValueUpdatesIfApplicable" << endl;

    Responses responses;

    auto now = std::chrono::system_clock::now();

    for (auto& kv : my_attributes_to_update_periodically) {
        auto& settings = my_federates_update_settings[kv.first];

        if (settings.updateRate != std::chrono::seconds(0)) {
            if (now - settings.lastUpdate >= settings.updateRate) {
                auto resp
                    = provideAttributeValueUpdate(my_federate_objects[kv.first], {begin(kv.second), end(kv.second)});
                responses.insert(end(responses), make_move_iterator(begin(resp)), make_move_iterator(end(resp)));

                kv.second.clear();
                settings.lastUpdate = now;
            }
        }
    }

    Debug(D, pdGendoc) << "exit  Mom::providePeriodicAttributeValueUpdatesIfApplicable" << endl;

    return responses;
}
}
}
