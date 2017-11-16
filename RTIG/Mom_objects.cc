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

#include <chrono>
#include <ctime>
#include <iomanip>
#include <libCERTI/PrettyDebug.hh>

using std::cout;
using std::endl;

namespace certi {
namespace rtig {

static PrettyDebug D("MOM", __FILE__);

void Mom::registerFederation()
{
    Debug(D, pdGendoc) << "enter Mom::registerFederation" << endl;

    auto objectName = "Federation_" + my_federation.getName();

    my_federation.reserveObjectInstanceName(my_handle, objectName);

    my_federation_object
        = my_federation.registerObject(my_handle, my_object_class_cache["HLAmanager.HLAfederation"], objectName);

    std::vector<AttributeHandle> attributes;

    AttributeHandle attribute;

    // Static
    attribute = my_attribute_cache["HLAfederationName"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeString(my_federation.getName());

    // Conditional
    attribute = my_attribute_cache["HLAfederatesInFederation"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeFederateHandleList();

    // Static
    attribute = my_attribute_cache["HLARTIversion"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeString("1516-2010");

    // Static
    attribute = my_attribute_cache["HLAMIMDesignator"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeString("HLAstandardMIM");

    // Conditional
    attribute = my_attribute_cache["HLAFOMmoduleDesignatorList"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeString(my_federation.getFEDid());

    // Conditional
    attribute = my_attribute_cache["HLAcurrentFDD"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeString(my_federation.getFEDid());

    // Static
    attribute = my_attribute_cache["HLAtimeImplementationName"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = encodeString("HLAfloat64Time");

    // Conditional
    attribute = my_attribute_cache["HLAlastSaveName"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = {};

    // Conditional
    attribute = my_attribute_cache["HLAlastSaveTime"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = {};

    // Conditional
    attribute = my_attribute_cache["HLAnextSaveName"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = {};

    // Conditional
    attribute = my_attribute_cache["HLAnextSaveTime"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = {};

    // Conditional
    attribute = my_attribute_cache["HLAautoProvide"];
    attributes.push_back(attribute);
    my_attribute_values_cache[my_federation_object][attribute] = {};

    provideAttributeValueUpdate(my_federation_object, attributes);

    Debug(D, pdGendoc) << "exit  Mom::registerFederation" << endl;
}

void Mom::registerFederate(const Federate& federate)
{
    Debug(D, pdGendoc) << "enter Mom::registerFederate" << endl;

    auto objectName = "Federate_" + federate.getName();

    my_federation.reserveObjectInstanceName(my_handle, objectName);

    auto federate_object
        = my_federation.registerObject(my_handle, my_object_class_cache["HLAmanager.HLAfederate"], objectName);
    my_federate_objects.insert(std::make_pair(federate.getHandle(), federate_object));

    std::vector<AttributeHandle> attributes;

    AttributeHandle attribute;

    // Static
    attribute = my_attribute_cache["HLAfederateHandle"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(federate.getHandle());

    // Static
    attribute = my_attribute_cache["HLAfederateName"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeString(federate.getName());

    // Static
    attribute = my_attribute_cache["HLAfederateType"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeString("NOT-COMPLIANT");

    // Static
    attribute = my_attribute_cache["HLAfederateHost"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeString("TODO");

    // Static
    attribute = my_attribute_cache["HLARTIversion"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeString("TODO");

    // Static
    attribute = my_attribute_cache["HLAFOMmoduleDesignatorList"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeString("TODO");

    // Conditional
    attribute = my_attribute_cache["HLAtimeConstrained"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeBoolean(federate.isConstrained());

    // Conditional
    attribute = my_attribute_cache["HLAtimeRegulating"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeBoolean(federate.isRegulator());

    // Conditional
    attribute = my_attribute_cache["HLAasynchronousDelivery"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeString("TODO");

    // Conditional
    attribute = my_attribute_cache["HLAfederateState"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeFederateState(federate);

    // Conditional
    attribute = my_attribute_cache["HLAtimeManagerState"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeString("TODO");

    // Periodic
    attribute = my_attribute_cache["HLAlogicalTime"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeString("TODO");

    // Periodic
    attribute = my_attribute_cache["HLAlookahead"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeString("TODO");

    // Periodic
    attribute = my_attribute_cache["HLAGALT"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeString("TODO");

    // Periodic
    attribute = my_attribute_cache["HLALITS"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeString("TODO");

    // Periodic
    attribute = my_attribute_cache["HLAROlength"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLATSOlength"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAreflectionsReceived"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAupdatesSent"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAinteractionsReceived"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAinteractionsSent"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAobjectInstancesThatCanBeDeleted"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAobjectInstancesUpdated"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAobjectInstancesReflected"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAobjectInstancesDeleted"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAobjectInstancesRemoved"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAobjectInstancesRegistered"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAobjectInstancesDiscovered"];
    attributes.push_back(attribute);
    my_count_attributes_cache[federate.getHandle()][attribute] = 0;
    my_attribute_values_cache[federate_object][attribute] = encodeUInt32(0);

    // Periodic
    attribute = my_attribute_cache["HLAtimeGrantedTime"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeString("TODO");

    // Periodic
    attribute = my_attribute_cache["HLAtimeAdvancingTime"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeString("TODO");

    // Conditional
    attribute = my_attribute_cache["HLAconveyRegionDesignatorSets"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeString("TODO");

    // Conditional
    attribute = my_attribute_cache["HLAconveyProducingFederate"];
    attributes.push_back(attribute);
    my_attribute_values_cache[federate_object][attribute] = encodeString("TODO");

    provideAttributeValueUpdate(federate_object, attributes);

    Debug(D, pdGendoc) << "exit  Mom::registerFederate" << endl;
}

void Mom::deleteFederate(const FederateHandle federate_handle)
{
    auto federate_object = my_federate_objects[federate_handle];
    my_federation.deleteObject(my_federation_object, federate_object, "");

    my_federate_objects.erase(federate_handle);
    my_attribute_values_cache.erase(federate_handle);
}

void Mom::updateFederatesInFederation()
{
    Debug(D, pdGendoc) << "enter Mom::updateFederatesInFederation" << endl;

    auto attribute = my_attribute_cache["HLAfederatesInFederation"];

    my_attribute_values_cache[my_federation_object][attribute] = encodeFederateHandleList();

    provideAttributeValueUpdate(my_federation_object, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateFederatesInFederation" << endl;
}

void Mom::updateFomModuleDesignatorList(const std::vector<std::string>& value)
{
    Debug(D, pdGendoc) << "enter Mom::updateFomModuleDesignatorList" << endl;

    auto attribute = my_attribute_cache["HLAFOMmoduleDesignatorList"];

    my_attribute_values_cache[my_federation_object][attribute] = encodeString("TODO"); // TODO

    provideAttributeValueUpdate(my_federation_object, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateFomModuleDesignatorList" << endl;
}

void Mom::updateCurrentFDD(const std::string& value)
{
    Debug(D, pdGendoc) << "enter Mom::updateCurrentFDD" << endl;

    auto attribute = my_attribute_cache["HLAcurrentFDD"];

    my_attribute_values_cache[my_federation_object][attribute] = encodeString("TODO"); // TODO

    provideAttributeValueUpdate(my_federation_object, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateCurrentFDD" << endl;
}

void Mom::updateLastSave()
{
    Debug(D, pdGendoc) << "enter Mom::updateLastSave" << endl;

    auto attribute = my_attribute_cache["HLAlastSaveName"];

    my_attribute_values_cache[my_federation_object][attribute]
        = my_attribute_values_cache[my_federation_object][my_attribute_cache["HLAnextSaveName"]]; // TODO

    auto attribute2 = my_attribute_cache["HLAlastSaveTime"];

    my_attribute_values_cache[my_federation_object][attribute]
        = my_attribute_values_cache[my_federation_object][my_attribute_cache["HLAnextSaveTime"]]; // TODO

    provideAttributeValueUpdate(my_federation_object, {attribute, attribute2});

    Debug(D, pdGendoc) << "exit  Mom::updateLastSave" << endl;
}

void Mom::updateNextSave(const std::string& label, const FederationTime& /*time*/)
{
    Debug(D, pdGendoc) << "enter Mom::updateNextSave" << endl;

    auto attribute = my_attribute_cache["HLAnextSaveName"];

    my_attribute_values_cache[my_federation_object][attribute] = encodeString(label); // TODO

    auto attribute2 = my_attribute_cache["HLAnextSaveTime"];

    my_attribute_values_cache[my_federation_object][attribute] = encodeString("TODO"); // TODO

    provideAttributeValueUpdate(my_federation_object, {attribute, attribute2});

    Debug(D, pdGendoc) << "enter Mom::updateNextSave" << endl;
}

void Mom::updateAutoProvide(const bool value)
{
    Debug(D, pdGendoc) << "enter Mom::updateAutoProvide" << endl;

    auto attribute = my_attribute_cache["HLAautoProvide"];

    my_attribute_values_cache[my_federation_object][attribute] = encodeString("TODO"); // TODO

    provideAttributeValueUpdate(my_federation_object, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateAutoProvide" << endl;
}

void Mom::updateTimeConstrained(const Federate& federate)
{
    Debug(D, pdGendoc) << "enter Mom::updateTimeConstrained" << endl;

    auto attribute = my_attribute_cache["HLAtimeConstrained"];

    my_attribute_values_cache[my_federate_objects[federate.getHandle()]][attribute]
        = encodeBoolean(federate.isConstrained());

    provideAttributeValueUpdate(my_federation_object, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateTimeConstrained" << endl;
}

void Mom::updateTimeRegulating(const Federate& federate)
{
    Debug(D, pdGendoc) << "enter Mom::updateTimeRegulating" << endl;

    auto attribute = my_attribute_cache["HLAtimeRegulating"];

    my_attribute_values_cache[my_federate_objects[federate.getHandle()]][attribute]
        = encodeBoolean(federate.isRegulator());

    provideAttributeValueUpdate(my_federation_object, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateTimeRegulating" << endl;
}

void Mom::updateAsynchronousDelivery(const FederateHandle federate_handle, const bool value)
{
    Debug(D, pdGendoc) << "enter Mom::updateAsynchronousDelivery" << endl;

    auto attribute = my_attribute_cache["HLAasynchronousDelivery"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeString("TODO"); // TODO

    provideAttributeValueUpdate(my_federation_object, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateAsynchronousDelivery" << endl;
}

void Mom::updateFederateState(const Federate& federate)
{
    Debug(D, pdGendoc) << "enter Mom::updateFederateState" << endl;

    auto attribute = my_attribute_cache["HLAfederateState"];

    my_attribute_values_cache[my_federate_objects[federate.getHandle()]][attribute] = encodeFederateState(federate);

    provideAttributeValueUpdate(my_federation_object, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateFederateState" << endl;
}

void Mom::updateTimeManagerState(const FederateHandle federate_handle, const TimeState value)
{
    Debug(D, pdGendoc) << "enter Mom::updateTimeManagerState" << endl;

    auto attribute = my_attribute_cache["HLAtimeManagerState"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeString("TODO"); // TODO

    provideAttributeValueUpdate(my_federation_object, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateTimeManagerState" << endl;
}

void Mom::updateLogicalTime(const FederateHandle federate_handle, const FederationTime& value)
{
    Debug(D, pdGendoc) << "enter Mom::updateLogicalTime" << endl;

    auto attribute = my_attribute_cache["HLAlogicalTime"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeString("TODO"); // TODO

    preparePeriodicAttributeValueUpdate(federate_handle, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateLogicalTime" << endl;
}

void Mom::updateLookahead(const FederateHandle federate_handle, const FederationTime& value)
{
    Debug(D, pdGendoc) << "enter Mom::updateLookahead" << endl;

    auto attribute = my_attribute_cache["HLAlookahead"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeString("TODO"); // TODO

    preparePeriodicAttributeValueUpdate(federate_handle, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateLookahead" << endl;
}

void Mom::updateGALT(const FederateHandle federate_handle, const FederationTime& value)
{
    Debug(D, pdGendoc) << "enter Mom::updateGALT" << endl;

    auto attribute = my_attribute_cache["HLAGALT"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeString("TODO"); // TODO

    preparePeriodicAttributeValueUpdate(federate_handle, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateGALT" << endl;
}

void Mom::updateLITS(const FederateHandle federate_handle, const FederationTime& value)
{
    Debug(D, pdGendoc) << "enter Mom::updateLITS" << endl;

    auto attribute = my_attribute_cache["HLALITS"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeString("TODO"); // TODO

    preparePeriodicAttributeValueUpdate(federate_handle, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateLITS" << endl;
}

void Mom::updateRoLenght(const FederateHandle federate_handle, const int delta)
{
    Debug(D, pdGendoc) << "enter Mom::updateRoLenght" << endl;

    updateCountAttribute(federate_handle, "HLAROlength", delta);

    Debug(D, pdGendoc) << "exit  Mom::updateRoLenght" << endl;
}

void Mom::updateTsoLenght(const FederateHandle federate_handle, const int delta)
{
    Debug(D, pdGendoc) << "enter Mom::updateTsoLenght" << endl;

    updateCountAttribute(federate_handle, "HLATSOlength", delta);

    Debug(D, pdGendoc) << "exit  Mom::updateTsoLenght" << endl;
}

void Mom::updateReflectionsReceived(const FederateHandle federate_handle, const int delta)
{
    Debug(D, pdGendoc) << "enter Mom::updateReflectionsReceived" << endl;

    updateCountAttribute(federate_handle, "HLAreflectionsReceived", delta);

    Debug(D, pdGendoc) << "exit  Mom::updateReflectionsReceived" << endl;
}

void Mom::updateUpdatesSent(const FederateHandle federate_handle, const int delta)
{
    Debug(D, pdGendoc) << "enter Mom::updateUpdatesSent" << endl;

    updateCountAttribute(federate_handle, "HLAupdatesSent", delta);

    Debug(D, pdGendoc) << "exit  Mom::updateUpdatesSent" << endl;
}

void Mom::updateInteractionsReceived(const FederateHandle federate_handle, const int delta)
{
    Debug(D, pdGendoc) << "enter Mom::updateInteractionsReceived" << endl;

    updateCountAttribute(federate_handle, "HLAinteractionsReceived", delta);

    Debug(D, pdGendoc) << "exit  Mom::updateInteractionsReceived" << endl;
}

void Mom::updateInteractionsSent(const FederateHandle federate_handle, const int delta)
{
    Debug(D, pdGendoc) << "enter Mom::updateInteractionsSent" << endl;

    updateCountAttribute(federate_handle, "HLAinteractionsSent", delta);

    Debug(D, pdGendoc) << "exit  Mom::updateInteractionsSent" << endl;
}

void Mom::updateObjectInstancesThatCanBeDeleted(const FederateHandle federate_handle, const int delta)
{
    Debug(D, pdGendoc) << "enter Mom::updateObjectInstancesThatCanBeDeleted" << endl;

    updateCountAttribute(federate_handle, "HLAobjectInstancesThatCanBeDeleted", delta);

    Debug(D, pdGendoc) << "exit  Mom::updateObjectInstancesThatCanBeDeleted" << endl;
}

void Mom::updateObjectInstancesUpdated(const FederateHandle federate_handle, const int delta)
{
    Debug(D, pdGendoc) << "enter Mom::updateObjectInstancesUpdated" << endl;

    updateCountAttribute(federate_handle, "HLAobjectInstancesUpdated", delta);

    Debug(D, pdGendoc) << "exit  Mom::updateObjectInstancesUpdated" << endl;
}

void Mom::updateObjectInstancesReflected(const FederateHandle federate_handle, const int delta)
{
    Debug(D, pdGendoc) << "enter Mom::updateObjectInstancesReflected" << endl;

    updateCountAttribute(federate_handle, "HLAobjectInstancesReflected", delta);

    Debug(D, pdGendoc) << "exit  Mom::updateObjectInstancesReflected" << endl;
}

void Mom::updateObjectInstancesDeleted(const FederateHandle federate_handle, const int delta)
{
    Debug(D, pdGendoc) << "enter Mom::updateObjectInstancesDeleted" << endl;

    updateCountAttribute(federate_handle, "HLAobjectInstancesDeleted", delta);

    Debug(D, pdGendoc) << "exit  Mom::updateObjectInstancesDeleted" << endl;
}

void Mom::updateObjectInstancesRemoved(const FederateHandle federate_handle, const int delta)
{
    Debug(D, pdGendoc) << "enter Mom::updateObjectInstancesRemoved" << endl;

    updateCountAttribute(federate_handle, "HLAobjectInstancesRemoved", delta);

    Debug(D, pdGendoc) << "exit  Mom::updateObjectInstancesRemoved" << endl;
}

void Mom::updateObjectInstancesRegistered(const FederateHandle federate_handle, const int delta)
{
    Debug(D, pdGendoc) << "enter Mom::updateObjectInstancesRegistered" << endl;

    updateCountAttribute(federate_handle, "HLAobjectInstancesRegistered", delta);

    Debug(D, pdGendoc) << "exit  Mom::updateObjectInstancesRegistered" << endl;
}

void Mom::updateObjectInstancesDiscovered(const FederateHandle federate_handle, const int delta)
{
    Debug(D, pdGendoc) << "enter Mom::updateObjectInstancesDiscovered" << endl;

    updateCountAttribute(federate_handle, "HLAobjectInstancesDiscovered", delta);

    Debug(D, pdGendoc) << "exit  Mom::updateObjectInstancesDiscovered" << endl;
}

void Mom::updateTimeGrantedTime(const FederateHandle federate_handle, const int value)
{
    Debug(D, pdGendoc) << "enter Mom::updateTimeGrantedTime" << endl;

    auto attribute = my_attribute_cache["HLAtimeGrantedTime"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeString("TODO"); // TODO

    preparePeriodicAttributeValueUpdate(federate_handle, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateTimeGrantedTime" << endl;
}

void Mom::updateTimeAdvancingTime(const FederateHandle federate_handle, const int value)
{
    Debug(D, pdGendoc) << "enter Mom::updateTimeAdvancingTime" << endl;

    auto attribute = my_attribute_cache["HLAtimeAdvancingTime"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeString("TODO"); // TODO

    preparePeriodicAttributeValueUpdate(federate_handle, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateTimeAdvancingTime" << endl;
}

void Mom::updateConveyRegionDesignatorSets(const FederateHandle federate_handle, const bool value)
{
    Debug(D, pdGendoc) << "enter Mom::updateConveyRegionDesignatorSets" << endl;

    auto attribute = my_attribute_cache["HLAconveyRegionDesignatorSets"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeString("TODO"); // TODO

    provideAttributeValueUpdate(my_federation_object, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateConveyRegionDesignatorSets" << endl;
}

void Mom::updateConveyProducingFederate(const FederateHandle federate_handle, const bool value)
{
    Debug(D, pdGendoc) << "enter Mom::updateConveyProducingFederate" << endl;

    auto attribute = my_attribute_cache["HLAconveyProducingFederate"];

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute] = encodeString("TODO"); // TODO

    provideAttributeValueUpdate(my_federation_object, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateConveyProducingFederate" << endl;
}

void Mom::updateCountAttribute(const FederateHandle& federate_handle,
                               const std::string& attribute_name,
                               const int delta)
{
    Debug(D, pdGendoc) << "enter Mom::updateCountAttribute" << endl;

    auto attribute = my_attribute_cache[attribute_name];

    my_count_attributes_cache[federate_handle][attribute] += delta;

    my_attribute_values_cache[my_federate_objects[federate_handle]][attribute]
        = encodeUInt32(my_count_attributes_cache[federate_handle][attribute]);

    preparePeriodicAttributeValueUpdate(federate_handle, {attribute});

    Debug(D, pdGendoc) << "exit  Mom::updateCountAttribute" << endl;
}

void Mom::provideAttributeValueUpdate(const ObjectHandle& object, const std::vector<AttributeHandle>& attributes)
{
    Debug(D, pdGendoc) << "enter Mom::provideAttributeValueUpdate" << endl;

    std::vector<AttributeValue_t> values;

    for (const auto& attribute : attributes) {
        values.push_back(my_attribute_values_cache[object][attribute]);
    }

    my_federation.updateAttributeValues(my_handle, object, attributes, values, "");

    Debug(D, pdGendoc) << "exit  Mom::provideAttributeValueUpdate" << endl;
}

void Mom::preparePeriodicAttributeValueUpdate(const FederateHandle& federate,
                                              const std::vector<AttributeHandle>& attributes)
{
    Debug(D, pdGendoc) << "enter Mom::preparePeriodicAttributeValueUpdate" << endl;

    if (federate == my_handle) {
        return;
    }

    for (const auto& attribute : attributes) {
        my_attributes_to_update_periodically[federate].insert(attribute);
    }

    providePeriodicAttributeValueUpdatesIfApplicable();

    Debug(D, pdGendoc) << "exit  Mom::preparePeriodicAttributeValueUpdate" << endl;
}

void Mom::providePeriodicAttributeValueUpdatesIfApplicable()
{
    Debug(D, pdGendoc) << "enter Mom::providePeriodicAttributeValueUpdatesIfApplicable" << endl;
    std::cout << "=1=========================================================" << std::endl;
    //     std::map<FederateHandle, std::set<AttributeHandle>> my_attributes_to_update_periodically;
    for (const auto& kv : my_attributes_to_update_periodically) {
        std::cout << "Federate: " << kv.first << endl;
        for (const auto& attr : kv.second) {
            std::cout << "\t\tAttribute: " << attr << std::endl;
        }
    }
    std::cout << "=2=========================================================" << std::endl;

    auto now = std::chrono::system_clock::now();
    //*/
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::cout << "Current time: " << std::put_time(std::localtime(&now_c), "%F %T") << std::endl;
    //*/

    for (auto& kv : my_attributes_to_update_periodically) {
        auto& settings = my_federates_update_settings[kv.first];
        //*/
        std::time_t last_c = std::chrono::system_clock::to_time_t(settings.lastUpdate);
        std::cout << "Settings: " << settings.updateRate.count() << ", "
                  << std::put_time(std::localtime(&last_c), "%F %T") << std::endl;
        //*/
        if (settings.updateRate == std::chrono::seconds(0)) {
            std::cout << "Update disabled for federate " << kv.first << ", skipping." << std::endl;
        }
        else {
            auto delta = now - settings.lastUpdate;
            std::cout << "Delta: " << delta.count() << std::endl;
            if (delta < settings.updateRate) {
                std::cout << "Last update was less than updateRate, skipping." << std::endl;
            }
            else {
                std::cout << "Updating values" << std::endl;
                
                provideAttributeValueUpdate(my_federate_objects[kv.first], {begin(kv.second), end(kv.second)});
                
                /*
                for (const auto& kv2 : kv.second) {
                    std::vector<AttributeHandle> attributes(begin(kv2.second), end(kv2.second));
                    for (const auto& attr : attributes) {
                        std::cout << "  " << attr << std::endl;
                    }
                    provideAttributeValueUpdate(kv2.first, attributes);
                    std::cout << "PAVU" << kv2.first << " " << kv2.second.size() << std::endl;
                }*/
                std::cout << "clear" << std::endl;
                kv.second.clear();
                std::cout << "update last update" << std::endl;
                settings.lastUpdate = now;
                std::cout << "done" << std::endl;
            }
        }
    }
    std::cout << "=3=========================================================" << std::endl;
    //     std::map<FederateHandle, std::set<AttributeHandle>> my_attributes_to_update_periodically;
    for (const auto& kv : my_attributes_to_update_periodically) {
        std::cout << "Federate: " << kv.first << endl;
        for (const auto& attr : kv.second) {
            std::cout << "\t\tAttribute: " << attr << std::endl;
        }
    }
    std::cout << "=4=========================================================" << std::endl;
    Debug(D, pdGendoc) << "exit  Mom::providePeriodicAttributeValueUpdatesIfApplicable" << endl;
}
}
}
