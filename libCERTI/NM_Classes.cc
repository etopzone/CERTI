// Generated on 2018 April Fri, 13 at 11:41:23 by the CERTI message generator
#include <string>
#include <vector>
#include "NM_Classes.hh"
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2008  ONERA
//
// This program is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) Any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// ----------------------------------------------------------------------------
// CERTI Network Messages description
// ----------------------------------------------------------------------------
// The messages related classes will be placed in the certi package
namespace certi {

void NM_FOM_Dimension::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Specific serialization code
    msgBuffer.write_uint32(handle);
    msgBuffer.write_string(name);
}

void NM_FOM_Dimension::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Specific deserialization code
    handle = static_cast<DimensionHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(name);
}

const DimensionHandle& NM_FOM_Dimension::getHandle() const
{
    return handle;
}

void NM_FOM_Dimension::setHandle(const DimensionHandle& newHandle)
{
    handle = newHandle;
}

const std::string& NM_FOM_Dimension::getName() const
{
    return name;
}

void NM_FOM_Dimension::setName(const std::string& newName)
{
    name = newName;
}

std::ostream& operator<<(std::ostream& os, const NM_FOM_Dimension& msg)
{
    os << "[NM_FOM_Dimension - Begin]" << std::endl;
    
    // Specific display
    os << "  handle = " << msg.handle << std::endl;
    os << "  name = " << msg.name << std::endl;
    
    os << "[NM_FOM_Dimension - End]" << std::endl;
    return os;
}

void NM_FOM_Routing_Space::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Specific serialization code
    msgBuffer.write_uint32(space);
    msgBuffer.write_string(name);
    uint32_t dimensionsSize = dimensions.size();
    msgBuffer.write_uint32(dimensionsSize);
    for (uint32_t i = 0; i < dimensionsSize; ++i) {
        dimensions[i].serialize(msgBuffer);
    }
}

void NM_FOM_Routing_Space::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Specific deserialization code
    space = static_cast<SpaceHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(name);
    uint32_t dimensionsSize = msgBuffer.read_uint32();
    dimensions.resize(dimensionsSize);
    for (uint32_t i = 0; i < dimensionsSize; ++i) {
        dimensions[i].deserialize(msgBuffer);
    }
}

const SpaceHandle& NM_FOM_Routing_Space::getSpace() const
{
    return space;
}

void NM_FOM_Routing_Space::setSpace(const SpaceHandle& newSpace)
{
    space = newSpace;
}

const std::string& NM_FOM_Routing_Space::getName() const
{
    return name;
}

void NM_FOM_Routing_Space::setName(const std::string& newName)
{
    name = newName;
}

uint32_t NM_FOM_Routing_Space::getDimensionsSize() const
{
    return dimensions.size();
}

void NM_FOM_Routing_Space::setDimensionsSize(uint32_t num)
{
    dimensions.resize(num);
}

const std::vector<NM_FOM_Dimension>& NM_FOM_Routing_Space::getDimensions() const
{
    return dimensions;
}

const NM_FOM_Dimension& NM_FOM_Routing_Space::getDimensions(uint32_t rank) const
{
    return dimensions[rank];
}

NM_FOM_Dimension& NM_FOM_Routing_Space::getDimensions(uint32_t rank)
{
    return dimensions[rank];
}

void NM_FOM_Routing_Space::setDimensions(const NM_FOM_Dimension& newDimensions, uint32_t rank)
{
    dimensions[rank] = newDimensions;
}

void NM_FOM_Routing_Space::removeDimensions(uint32_t rank)
{
    dimensions.erase(dimensions.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const NM_FOM_Routing_Space& msg)
{
    os << "[NM_FOM_Routing_Space - Begin]" << std::endl;
    
    // Specific display
    os << "  space = " << msg.space << std::endl;
    os << "  name = " << msg.name << std::endl;
    os << "  dimensions [] =" << std::endl;
    for (const auto& element : msg.dimensions) {
        os << element;
    }
    os << std::endl;
    
    os << "[NM_FOM_Routing_Space - End]" << std::endl;
    return os;
}

void NM_FOM_Attribute::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Specific serialization code
    msgBuffer.write_uint32(handle);
    msgBuffer.write_string(name);
    msgBuffer.write_uint32(spaceHandle);
    msgBuffer.write_uint8(order);
    msgBuffer.write_uint8(transport);
}

void NM_FOM_Attribute::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Specific deserialization code
    handle = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(name);
    spaceHandle = static_cast<SpaceHandle>(msgBuffer.read_uint32());
    order = static_cast<OrderType>(msgBuffer.read_uint8());
    transport = static_cast<TransportType>(msgBuffer.read_uint8());
}

const AttributeHandle& NM_FOM_Attribute::getHandle() const
{
    return handle;
}

void NM_FOM_Attribute::setHandle(const AttributeHandle& newHandle)
{
    handle = newHandle;
}

const std::string& NM_FOM_Attribute::getName() const
{
    return name;
}

void NM_FOM_Attribute::setName(const std::string& newName)
{
    name = newName;
}

const SpaceHandle& NM_FOM_Attribute::getSpaceHandle() const
{
    return spaceHandle;
}

void NM_FOM_Attribute::setSpaceHandle(const SpaceHandle& newSpaceHandle)
{
    spaceHandle = newSpaceHandle;
}

const OrderType& NM_FOM_Attribute::getOrder() const
{
    return order;
}

void NM_FOM_Attribute::setOrder(const OrderType& newOrder)
{
    order = newOrder;
}

const TransportType& NM_FOM_Attribute::getTransport() const
{
    return transport;
}

void NM_FOM_Attribute::setTransport(const TransportType& newTransport)
{
    transport = newTransport;
}

std::ostream& operator<<(std::ostream& os, const NM_FOM_Attribute& msg)
{
    os << "[NM_FOM_Attribute - Begin]" << std::endl;
    
    // Specific display
    os << "  handle = " << msg.handle << std::endl;
    os << "  name = " << msg.name << std::endl;
    os << "  spaceHandle = " << msg.spaceHandle << std::endl;
    os << "  order = " << msg.order << std::endl;
    os << "  transport = " << msg.transport << std::endl;
    
    os << "[NM_FOM_Attribute - End]" << std::endl;
    return os;
}

void NM_FOM_Object_Class::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Specific serialization code
    msgBuffer.write_uint32(handle);
    msgBuffer.write_uint32(superClass);
    msgBuffer.write_string(name);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i].serialize(msgBuffer);
    }
}

void NM_FOM_Object_Class::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Specific deserialization code
    handle = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    superClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(name);
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i].deserialize(msgBuffer);
    }
}

const ObjectClassHandle& NM_FOM_Object_Class::getHandle() const
{
    return handle;
}

void NM_FOM_Object_Class::setHandle(const ObjectClassHandle& newHandle)
{
    handle = newHandle;
}

const ObjectClassHandle& NM_FOM_Object_Class::getSuperClass() const
{
    return superClass;
}

void NM_FOM_Object_Class::setSuperClass(const ObjectClassHandle& newSuperClass)
{
    superClass = newSuperClass;
}

const std::string& NM_FOM_Object_Class::getName() const
{
    return name;
}

void NM_FOM_Object_Class::setName(const std::string& newName)
{
    name = newName;
}

uint32_t NM_FOM_Object_Class::getAttributesSize() const
{
    return attributes.size();
}

void NM_FOM_Object_Class::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<NM_FOM_Attribute>& NM_FOM_Object_Class::getAttributes() const
{
    return attributes;
}

const NM_FOM_Attribute& NM_FOM_Object_Class::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

NM_FOM_Attribute& NM_FOM_Object_Class::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void NM_FOM_Object_Class::setAttributes(const NM_FOM_Attribute& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void NM_FOM_Object_Class::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const NM_FOM_Object_Class& msg)
{
    os << "[NM_FOM_Object_Class - Begin]" << std::endl;
    
    // Specific display
    os << "  handle = " << msg.handle << std::endl;
    os << "  superClass = " << msg.superClass << std::endl;
    os << "  name = " << msg.name << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[NM_FOM_Object_Class - End]" << std::endl;
    return os;
}

void NM_FOM_Parameter::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Specific serialization code
    msgBuffer.write_uint32(handle);
    msgBuffer.write_string(name);
}

void NM_FOM_Parameter::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Specific deserialization code
    handle = static_cast<ParameterHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(name);
}

const ParameterHandle& NM_FOM_Parameter::getHandle() const
{
    return handle;
}

void NM_FOM_Parameter::setHandle(const ParameterHandle& newHandle)
{
    handle = newHandle;
}

const std::string& NM_FOM_Parameter::getName() const
{
    return name;
}

void NM_FOM_Parameter::setName(const std::string& newName)
{
    name = newName;
}

std::ostream& operator<<(std::ostream& os, const NM_FOM_Parameter& msg)
{
    os << "[NM_FOM_Parameter - Begin]" << std::endl;
    
    // Specific display
    os << "  handle = " << msg.handle << std::endl;
    os << "  name = " << msg.name << std::endl;
    
    os << "[NM_FOM_Parameter - End]" << std::endl;
    return os;
}

void NM_FOM_Interaction_Class::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    msgBuffer.write_string(name);
    msgBuffer.write_uint32(superClass);
    msgBuffer.write_uint32(space);
    msgBuffer.write_uint8(order);
    msgBuffer.write_uint8(transport);
    uint32_t parametersSize = parameters.size();
    msgBuffer.write_uint32(parametersSize);
    for (uint32_t i = 0; i < parametersSize; ++i) {
        parameters[i].serialize(msgBuffer);
    }
}

void NM_FOM_Interaction_Class::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(name);
    superClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    space = static_cast<SpaceHandle>(msgBuffer.read_uint32());
    order = static_cast<OrderType>(msgBuffer.read_uint8());
    transport = static_cast<TransportType>(msgBuffer.read_uint8());
    uint32_t parametersSize = msgBuffer.read_uint32();
    parameters.resize(parametersSize);
    for (uint32_t i = 0; i < parametersSize; ++i) {
        parameters[i].deserialize(msgBuffer);
    }
}

const InteractionClassHandle& NM_FOM_Interaction_Class::getInteractionClass() const
{
    return interactionClass;
}

void NM_FOM_Interaction_Class::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

const std::string& NM_FOM_Interaction_Class::getName() const
{
    return name;
}

void NM_FOM_Interaction_Class::setName(const std::string& newName)
{
    name = newName;
}

const InteractionClassHandle& NM_FOM_Interaction_Class::getSuperClass() const
{
    return superClass;
}

void NM_FOM_Interaction_Class::setSuperClass(const InteractionClassHandle& newSuperClass)
{
    superClass = newSuperClass;
}

const SpaceHandle& NM_FOM_Interaction_Class::getSpace() const
{
    return space;
}

void NM_FOM_Interaction_Class::setSpace(const SpaceHandle& newSpace)
{
    space = newSpace;
}

const OrderType& NM_FOM_Interaction_Class::getOrder() const
{
    return order;
}

void NM_FOM_Interaction_Class::setOrder(const OrderType& newOrder)
{
    order = newOrder;
}

const TransportType& NM_FOM_Interaction_Class::getTransport() const
{
    return transport;
}

void NM_FOM_Interaction_Class::setTransport(const TransportType& newTransport)
{
    transport = newTransport;
}

uint32_t NM_FOM_Interaction_Class::getParametersSize() const
{
    return parameters.size();
}

void NM_FOM_Interaction_Class::setParametersSize(uint32_t num)
{
    parameters.resize(num);
}

const std::vector<NM_FOM_Parameter>& NM_FOM_Interaction_Class::getParameters() const
{
    return parameters;
}

const NM_FOM_Parameter& NM_FOM_Interaction_Class::getParameters(uint32_t rank) const
{
    return parameters[rank];
}

NM_FOM_Parameter& NM_FOM_Interaction_Class::getParameters(uint32_t rank)
{
    return parameters[rank];
}

void NM_FOM_Interaction_Class::setParameters(const NM_FOM_Parameter& newParameters, uint32_t rank)
{
    parameters[rank] = newParameters;
}

void NM_FOM_Interaction_Class::removeParameters(uint32_t rank)
{
    parameters.erase(parameters.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const NM_FOM_Interaction_Class& msg)
{
    os << "[NM_FOM_Interaction_Class - Begin]" << std::endl;
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  name = " << msg.name << std::endl;
    os << "  superClass = " << msg.superClass << std::endl;
    os << "  space = " << msg.space << std::endl;
    os << "  order = " << msg.order << std::endl;
    os << "  transport = " << msg.transport << std::endl;
    os << "  parameters [] =" << std::endl;
    for (const auto& element : msg.parameters) {
        os << element;
    }
    os << std::endl;
    
    os << "[NM_FOM_Interaction_Class - End]" << std::endl;
    return os;
}

NM_Close_Connexion::NM_Close_Connexion()
{
    this->messageName = "NM_Close_Connexion";
    this->type = NetworkMessage::Type::CLOSE_CONNEXION;
}

NM_Message_Null::NM_Message_Null()
{
    this->messageName = "NM_Message_Null";
    this->type = NetworkMessage::Type::MESSAGE_NULL;
}

NM_Create_Federation_Execution::NM_Create_Federation_Execution()
{
    this->messageName = "NM_Create_Federation_Execution";
    this->type = NetworkMessage::Type::CREATE_FEDERATION_EXECUTION;
}

void NM_Create_Federation_Execution::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_string(federationExecutionName);
    msgBuffer.write_uint8(rtiVersion);
    uint32_t fomModuleDesignatorsSize = fomModuleDesignators.size();
    msgBuffer.write_uint32(fomModuleDesignatorsSize);
    for (uint32_t i = 0; i < fomModuleDesignatorsSize; ++i) {
        msgBuffer.write_string(fomModuleDesignators[i]);
    }
    msgBuffer.write_bool(_hasMimDesignator);
    if (_hasMimDesignator) {
        msgBuffer.write_string(mimDesignator);
    }
}

void NM_Create_Federation_Execution::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    msgBuffer.read_string(federationExecutionName);
    rtiVersion = static_cast<RtiVersion>(msgBuffer.read_uint8());
    uint32_t fomModuleDesignatorsSize = msgBuffer.read_uint32();
    fomModuleDesignators.resize(fomModuleDesignatorsSize);
    for (uint32_t i = 0; i < fomModuleDesignatorsSize; ++i) {
        msgBuffer.read_string(fomModuleDesignators[i]);
    }
    _hasMimDesignator = msgBuffer.read_bool();
    if (_hasMimDesignator) {
        msgBuffer.read_string(mimDesignator);
    }
}

const std::string& NM_Create_Federation_Execution::getFederationExecutionName() const
{
    return federationExecutionName;
}

void NM_Create_Federation_Execution::setFederationExecutionName(const std::string& newFederationExecutionName)
{
    federationExecutionName = newFederationExecutionName;
}

const RtiVersion& NM_Create_Federation_Execution::getRtiVersion() const
{
    return rtiVersion;
}

void NM_Create_Federation_Execution::setRtiVersion(const RtiVersion& newRtiVersion)
{
    rtiVersion = newRtiVersion;
}

uint32_t NM_Create_Federation_Execution::getFomModuleDesignatorsSize() const
{
    return fomModuleDesignators.size();
}

void NM_Create_Federation_Execution::setFomModuleDesignatorsSize(uint32_t num)
{
    fomModuleDesignators.resize(num);
}

const std::vector<std::string>& NM_Create_Federation_Execution::getFomModuleDesignators() const
{
    return fomModuleDesignators;
}

const std::string& NM_Create_Federation_Execution::getFomModuleDesignators(uint32_t rank) const
{
    return fomModuleDesignators[rank];
}

std::string& NM_Create_Federation_Execution::getFomModuleDesignators(uint32_t rank)
{
    return fomModuleDesignators[rank];
}

void NM_Create_Federation_Execution::setFomModuleDesignators(const std::string& newFomModuleDesignators, uint32_t rank)
{
    fomModuleDesignators[rank] = newFomModuleDesignators;
}

void NM_Create_Federation_Execution::removeFomModuleDesignators(uint32_t rank)
{
    fomModuleDesignators.erase(fomModuleDesignators.begin() + rank);
}

const std::string& NM_Create_Federation_Execution::getMimDesignator() const
{
    return mimDesignator;
}

void NM_Create_Federation_Execution::setMimDesignator(const std::string& newMimDesignator)
{
    _hasMimDesignator = true;
    mimDesignator = newMimDesignator;
}

bool NM_Create_Federation_Execution::hasMimDesignator() const
{
    return _hasMimDesignator;
}

std::ostream& operator<<(std::ostream& os, const NM_Create_Federation_Execution& msg)
{
    os << "[NM_Create_Federation_Execution - Begin]" << std::endl;
    
    os << static_cast<const NM_Create_Federation_Execution::Super&>(msg); // show parent class
    
    // Specific display
    os << "  federationExecutionName = " << msg.federationExecutionName << std::endl;
    os << "  rtiVersion = " << msg.rtiVersion << std::endl;
    os << "  fomModuleDesignators [] =" << std::endl;
    for (const auto& element : msg.fomModuleDesignators) {
        os << element;
    }
    os << std::endl;
    os << "  (opt) mimDesignator =" << msg.mimDesignator << std::endl;
    
    os << "[NM_Create_Federation_Execution - End]" << std::endl;
    return os;
}

NM_Destroy_Federation_Execution::NM_Destroy_Federation_Execution()
{
    this->messageName = "NM_Destroy_Federation_Execution";
    this->type = NetworkMessage::Type::DESTROY_FEDERATION_EXECUTION;
}

void NM_Destroy_Federation_Execution::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_string(federationName);
}

void NM_Destroy_Federation_Execution::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    msgBuffer.read_string(federationName);
}

const std::string& NM_Destroy_Federation_Execution::getFederationName() const
{
    return federationName;
}

void NM_Destroy_Federation_Execution::setFederationName(const std::string& newFederationName)
{
    federationName = newFederationName;
}

std::ostream& operator<<(std::ostream& os, const NM_Destroy_Federation_Execution& msg)
{
    os << "[NM_Destroy_Federation_Execution - Begin]" << std::endl;
    
    os << static_cast<const NM_Destroy_Federation_Execution::Super&>(msg); // show parent class
    
    // Specific display
    os << "  federationName = " << msg.federationName << std::endl;
    
    os << "[NM_Destroy_Federation_Execution - End]" << std::endl;
    return os;
}

NM_Join_Federation_Execution::NM_Join_Federation_Execution()
{
    this->messageName = "NM_Join_Federation_Execution";
    this->type = NetworkMessage::Type::JOIN_FEDERATION_EXECUTION;
}

void NM_Join_Federation_Execution::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_int32(numberOfRegulators);
    msgBuffer.write_uint32(multicastAddress);
    msgBuffer.write_uint32(bestEffortAddress);
    msgBuffer.write_uint32(bestEffortPeer);
    msgBuffer.write_string(federationExecutionName);
    msgBuffer.write_bool(_hasFederateName);
    if (_hasFederateName) {
        msgBuffer.write_string(federateName);
    }
    msgBuffer.write_uint8(rtiVersion);
    msgBuffer.write_string(federateType);
    uint32_t additionalFomModulesSize = additionalFomModules.size();
    msgBuffer.write_uint32(additionalFomModulesSize);
    for (uint32_t i = 0; i < additionalFomModulesSize; ++i) {
        msgBuffer.write_string(additionalFomModules[i]);
    }
    uint32_t routingSpacesSize = routingSpaces.size();
    msgBuffer.write_uint32(routingSpacesSize);
    for (uint32_t i = 0; i < routingSpacesSize; ++i) {
        routingSpaces[i].serialize(msgBuffer);
    }
    uint32_t objectClassesSize = objectClasses.size();
    msgBuffer.write_uint32(objectClassesSize);
    for (uint32_t i = 0; i < objectClassesSize; ++i) {
        objectClasses[i].serialize(msgBuffer);
    }
    uint32_t interactionClassesSize = interactionClasses.size();
    msgBuffer.write_uint32(interactionClassesSize);
    for (uint32_t i = 0; i < interactionClassesSize; ++i) {
        interactionClasses[i].serialize(msgBuffer);
    }
}

void NM_Join_Federation_Execution::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    numberOfRegulators = msgBuffer.read_int32();
    multicastAddress = msgBuffer.read_uint32();
    bestEffortAddress = msgBuffer.read_uint32();
    bestEffortPeer = msgBuffer.read_uint32();
    msgBuffer.read_string(federationExecutionName);
    _hasFederateName = msgBuffer.read_bool();
    if (_hasFederateName) {
        msgBuffer.read_string(federateName);
    }
    rtiVersion = static_cast<RtiVersion>(msgBuffer.read_uint8());
    msgBuffer.read_string(federateType);
    uint32_t additionalFomModulesSize = msgBuffer.read_uint32();
    additionalFomModules.resize(additionalFomModulesSize);
    for (uint32_t i = 0; i < additionalFomModulesSize; ++i) {
        msgBuffer.read_string(additionalFomModules[i]);
    }
    uint32_t routingSpacesSize = msgBuffer.read_uint32();
    routingSpaces.resize(routingSpacesSize);
    for (uint32_t i = 0; i < routingSpacesSize; ++i) {
        routingSpaces[i].deserialize(msgBuffer);
    }
    uint32_t objectClassesSize = msgBuffer.read_uint32();
    objectClasses.resize(objectClassesSize);
    for (uint32_t i = 0; i < objectClassesSize; ++i) {
        objectClasses[i].deserialize(msgBuffer);
    }
    uint32_t interactionClassesSize = msgBuffer.read_uint32();
    interactionClasses.resize(interactionClassesSize);
    for (uint32_t i = 0; i < interactionClassesSize; ++i) {
        interactionClasses[i].deserialize(msgBuffer);
    }
}

const int32_t& NM_Join_Federation_Execution::getNumberOfRegulators() const
{
    return numberOfRegulators;
}

void NM_Join_Federation_Execution::setNumberOfRegulators(const int32_t& newNumberOfRegulators)
{
    numberOfRegulators = newNumberOfRegulators;
}

const uint32_t& NM_Join_Federation_Execution::getMulticastAddress() const
{
    return multicastAddress;
}

void NM_Join_Federation_Execution::setMulticastAddress(const uint32_t& newMulticastAddress)
{
    multicastAddress = newMulticastAddress;
}

const uint32_t& NM_Join_Federation_Execution::getBestEffortAddress() const
{
    return bestEffortAddress;
}

void NM_Join_Federation_Execution::setBestEffortAddress(const uint32_t& newBestEffortAddress)
{
    bestEffortAddress = newBestEffortAddress;
}

const uint32_t& NM_Join_Federation_Execution::getBestEffortPeer() const
{
    return bestEffortPeer;
}

void NM_Join_Federation_Execution::setBestEffortPeer(const uint32_t& newBestEffortPeer)
{
    bestEffortPeer = newBestEffortPeer;
}

const std::string& NM_Join_Federation_Execution::getFederationExecutionName() const
{
    return federationExecutionName;
}

void NM_Join_Federation_Execution::setFederationExecutionName(const std::string& newFederationExecutionName)
{
    federationExecutionName = newFederationExecutionName;
}

const std::string& NM_Join_Federation_Execution::getFederateName() const
{
    return federateName;
}

void NM_Join_Federation_Execution::setFederateName(const std::string& newFederateName)
{
    _hasFederateName = true;
    federateName = newFederateName;
}

bool NM_Join_Federation_Execution::hasFederateName() const
{
    return _hasFederateName;
}

const RtiVersion& NM_Join_Federation_Execution::getRtiVersion() const
{
    return rtiVersion;
}

void NM_Join_Federation_Execution::setRtiVersion(const RtiVersion& newRtiVersion)
{
    rtiVersion = newRtiVersion;
}

const std::string& NM_Join_Federation_Execution::getFederateType() const
{
    return federateType;
}

void NM_Join_Federation_Execution::setFederateType(const std::string& newFederateType)
{
    federateType = newFederateType;
}

uint32_t NM_Join_Federation_Execution::getAdditionalFomModulesSize() const
{
    return additionalFomModules.size();
}

void NM_Join_Federation_Execution::setAdditionalFomModulesSize(uint32_t num)
{
    additionalFomModules.resize(num);
}

const std::vector<std::string>& NM_Join_Federation_Execution::getAdditionalFomModules() const
{
    return additionalFomModules;
}

const std::string& NM_Join_Federation_Execution::getAdditionalFomModules(uint32_t rank) const
{
    return additionalFomModules[rank];
}

std::string& NM_Join_Federation_Execution::getAdditionalFomModules(uint32_t rank)
{
    return additionalFomModules[rank];
}

void NM_Join_Federation_Execution::setAdditionalFomModules(const std::string& newAdditionalFomModules, uint32_t rank)
{
    additionalFomModules[rank] = newAdditionalFomModules;
}

void NM_Join_Federation_Execution::removeAdditionalFomModules(uint32_t rank)
{
    additionalFomModules.erase(additionalFomModules.begin() + rank);
}

uint32_t NM_Join_Federation_Execution::getRoutingSpacesSize() const
{
    return routingSpaces.size();
}

void NM_Join_Federation_Execution::setRoutingSpacesSize(uint32_t num)
{
    routingSpaces.resize(num);
}

const std::vector<NM_FOM_Routing_Space>& NM_Join_Federation_Execution::getRoutingSpaces() const
{
    return routingSpaces;
}

const NM_FOM_Routing_Space& NM_Join_Federation_Execution::getRoutingSpaces(uint32_t rank) const
{
    return routingSpaces[rank];
}

NM_FOM_Routing_Space& NM_Join_Federation_Execution::getRoutingSpaces(uint32_t rank)
{
    return routingSpaces[rank];
}

void NM_Join_Federation_Execution::setRoutingSpaces(const NM_FOM_Routing_Space& newRoutingSpaces, uint32_t rank)
{
    routingSpaces[rank] = newRoutingSpaces;
}

void NM_Join_Federation_Execution::removeRoutingSpaces(uint32_t rank)
{
    routingSpaces.erase(routingSpaces.begin() + rank);
}

uint32_t NM_Join_Federation_Execution::getObjectClassesSize() const
{
    return objectClasses.size();
}

void NM_Join_Federation_Execution::setObjectClassesSize(uint32_t num)
{
    objectClasses.resize(num);
}

const std::vector<NM_FOM_Object_Class>& NM_Join_Federation_Execution::getObjectClasses() const
{
    return objectClasses;
}

const NM_FOM_Object_Class& NM_Join_Federation_Execution::getObjectClasses(uint32_t rank) const
{
    return objectClasses[rank];
}

NM_FOM_Object_Class& NM_Join_Federation_Execution::getObjectClasses(uint32_t rank)
{
    return objectClasses[rank];
}

void NM_Join_Federation_Execution::setObjectClasses(const NM_FOM_Object_Class& newObjectClasses, uint32_t rank)
{
    objectClasses[rank] = newObjectClasses;
}

void NM_Join_Federation_Execution::removeObjectClasses(uint32_t rank)
{
    objectClasses.erase(objectClasses.begin() + rank);
}

uint32_t NM_Join_Federation_Execution::getInteractionClassesSize() const
{
    return interactionClasses.size();
}

void NM_Join_Federation_Execution::setInteractionClassesSize(uint32_t num)
{
    interactionClasses.resize(num);
}

const std::vector<NM_FOM_Interaction_Class>& NM_Join_Federation_Execution::getInteractionClasses() const
{
    return interactionClasses;
}

const NM_FOM_Interaction_Class& NM_Join_Federation_Execution::getInteractionClasses(uint32_t rank) const
{
    return interactionClasses[rank];
}

NM_FOM_Interaction_Class& NM_Join_Federation_Execution::getInteractionClasses(uint32_t rank)
{
    return interactionClasses[rank];
}

void NM_Join_Federation_Execution::setInteractionClasses(const NM_FOM_Interaction_Class& newInteractionClasses, uint32_t rank)
{
    interactionClasses[rank] = newInteractionClasses;
}

void NM_Join_Federation_Execution::removeInteractionClasses(uint32_t rank)
{
    interactionClasses.erase(interactionClasses.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const NM_Join_Federation_Execution& msg)
{
    os << "[NM_Join_Federation_Execution - Begin]" << std::endl;
    
    os << static_cast<const NM_Join_Federation_Execution::Super&>(msg); // show parent class
    
    // Specific display
    os << "  numberOfRegulators = " << msg.numberOfRegulators << std::endl;
    os << "  multicastAddress = " << msg.multicastAddress << std::endl;
    os << "  bestEffortAddress = " << msg.bestEffortAddress << std::endl;
    os << "  bestEffortPeer = " << msg.bestEffortPeer << std::endl;
    os << "  federationExecutionName = " << msg.federationExecutionName << std::endl;
    os << "  (opt) federateName =" << msg.federateName << std::endl;
    os << "  rtiVersion = " << msg.rtiVersion << std::endl;
    os << "  federateType = " << msg.federateType << std::endl;
    os << "  additionalFomModules [] =" << std::endl;
    for (const auto& element : msg.additionalFomModules) {
        os << element;
    }
    os << std::endl;
    os << "  routingSpaces [] =" << std::endl;
    for (const auto& element : msg.routingSpaces) {
        os << element;
    }
    os << std::endl;
    os << "  objectClasses [] =" << std::endl;
    for (const auto& element : msg.objectClasses) {
        os << element;
    }
    os << std::endl;
    os << "  interactionClasses [] =" << std::endl;
    for (const auto& element : msg.interactionClasses) {
        os << element;
    }
    os << std::endl;
    
    os << "[NM_Join_Federation_Execution - End]" << std::endl;
    return os;
}

NM_Additional_Fom_Module::NM_Additional_Fom_Module()
{
    this->messageName = "NM_Additional_Fom_Module";
    this->type = NetworkMessage::Type::ADDITIONAL_FOM_MODULE;
}

void NM_Additional_Fom_Module::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    uint32_t routingSpacesSize = routingSpaces.size();
    msgBuffer.write_uint32(routingSpacesSize);
    for (uint32_t i = 0; i < routingSpacesSize; ++i) {
        routingSpaces[i].serialize(msgBuffer);
    }
    uint32_t objectClassesSize = objectClasses.size();
    msgBuffer.write_uint32(objectClassesSize);
    for (uint32_t i = 0; i < objectClassesSize; ++i) {
        objectClasses[i].serialize(msgBuffer);
    }
    uint32_t interactionClassesSize = interactionClasses.size();
    msgBuffer.write_uint32(interactionClassesSize);
    for (uint32_t i = 0; i < interactionClassesSize; ++i) {
        interactionClasses[i].serialize(msgBuffer);
    }
}

void NM_Additional_Fom_Module::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    uint32_t routingSpacesSize = msgBuffer.read_uint32();
    routingSpaces.resize(routingSpacesSize);
    for (uint32_t i = 0; i < routingSpacesSize; ++i) {
        routingSpaces[i].deserialize(msgBuffer);
    }
    uint32_t objectClassesSize = msgBuffer.read_uint32();
    objectClasses.resize(objectClassesSize);
    for (uint32_t i = 0; i < objectClassesSize; ++i) {
        objectClasses[i].deserialize(msgBuffer);
    }
    uint32_t interactionClassesSize = msgBuffer.read_uint32();
    interactionClasses.resize(interactionClassesSize);
    for (uint32_t i = 0; i < interactionClassesSize; ++i) {
        interactionClasses[i].deserialize(msgBuffer);
    }
}

uint32_t NM_Additional_Fom_Module::getRoutingSpacesSize() const
{
    return routingSpaces.size();
}

void NM_Additional_Fom_Module::setRoutingSpacesSize(uint32_t num)
{
    routingSpaces.resize(num);
}

const std::vector<NM_FOM_Routing_Space>& NM_Additional_Fom_Module::getRoutingSpaces() const
{
    return routingSpaces;
}

const NM_FOM_Routing_Space& NM_Additional_Fom_Module::getRoutingSpaces(uint32_t rank) const
{
    return routingSpaces[rank];
}

NM_FOM_Routing_Space& NM_Additional_Fom_Module::getRoutingSpaces(uint32_t rank)
{
    return routingSpaces[rank];
}

void NM_Additional_Fom_Module::setRoutingSpaces(const NM_FOM_Routing_Space& newRoutingSpaces, uint32_t rank)
{
    routingSpaces[rank] = newRoutingSpaces;
}

void NM_Additional_Fom_Module::removeRoutingSpaces(uint32_t rank)
{
    routingSpaces.erase(routingSpaces.begin() + rank);
}

uint32_t NM_Additional_Fom_Module::getObjectClassesSize() const
{
    return objectClasses.size();
}

void NM_Additional_Fom_Module::setObjectClassesSize(uint32_t num)
{
    objectClasses.resize(num);
}

const std::vector<NM_FOM_Object_Class>& NM_Additional_Fom_Module::getObjectClasses() const
{
    return objectClasses;
}

const NM_FOM_Object_Class& NM_Additional_Fom_Module::getObjectClasses(uint32_t rank) const
{
    return objectClasses[rank];
}

NM_FOM_Object_Class& NM_Additional_Fom_Module::getObjectClasses(uint32_t rank)
{
    return objectClasses[rank];
}

void NM_Additional_Fom_Module::setObjectClasses(const NM_FOM_Object_Class& newObjectClasses, uint32_t rank)
{
    objectClasses[rank] = newObjectClasses;
}

void NM_Additional_Fom_Module::removeObjectClasses(uint32_t rank)
{
    objectClasses.erase(objectClasses.begin() + rank);
}

uint32_t NM_Additional_Fom_Module::getInteractionClassesSize() const
{
    return interactionClasses.size();
}

void NM_Additional_Fom_Module::setInteractionClassesSize(uint32_t num)
{
    interactionClasses.resize(num);
}

const std::vector<NM_FOM_Interaction_Class>& NM_Additional_Fom_Module::getInteractionClasses() const
{
    return interactionClasses;
}

const NM_FOM_Interaction_Class& NM_Additional_Fom_Module::getInteractionClasses(uint32_t rank) const
{
    return interactionClasses[rank];
}

NM_FOM_Interaction_Class& NM_Additional_Fom_Module::getInteractionClasses(uint32_t rank)
{
    return interactionClasses[rank];
}

void NM_Additional_Fom_Module::setInteractionClasses(const NM_FOM_Interaction_Class& newInteractionClasses, uint32_t rank)
{
    interactionClasses[rank] = newInteractionClasses;
}

void NM_Additional_Fom_Module::removeInteractionClasses(uint32_t rank)
{
    interactionClasses.erase(interactionClasses.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const NM_Additional_Fom_Module& msg)
{
    os << "[NM_Additional_Fom_Module - Begin]" << std::endl;
    
    os << static_cast<const NM_Additional_Fom_Module::Super&>(msg); // show parent class
    
    // Specific display
    os << "  routingSpaces [] =" << std::endl;
    for (const auto& element : msg.routingSpaces) {
        os << element;
    }
    os << std::endl;
    os << "  objectClasses [] =" << std::endl;
    for (const auto& element : msg.objectClasses) {
        os << element;
    }
    os << std::endl;
    os << "  interactionClasses [] =" << std::endl;
    for (const auto& element : msg.interactionClasses) {
        os << element;
    }
    os << std::endl;
    
    os << "[NM_Additional_Fom_Module - End]" << std::endl;
    return os;
}

NM_Resign_Federation_Execution::NM_Resign_Federation_Execution()
{
    this->messageName = "NM_Resign_Federation_Execution";
    this->type = NetworkMessage::Type::RESIGN_FEDERATION_EXECUTION;
}

NM_Set_Time_Regulating::NM_Set_Time_Regulating()
{
    this->messageName = "NM_Set_Time_Regulating";
    this->type = NetworkMessage::Type::SET_TIME_REGULATING;
}

void NM_Set_Time_Regulating::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_bool(regulator);
}

void NM_Set_Time_Regulating::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    regulator = msgBuffer.read_bool();
}

void NM_Set_Time_Regulating::regulatorOn()
{
    regulator = true;
}

void NM_Set_Time_Regulating::regulatorOff()
{
    regulator = false;
}

bool NM_Set_Time_Regulating::isRegulatorOn() const
{
    return regulator;
}

std::ostream& operator<<(std::ostream& os, const NM_Set_Time_Regulating& msg)
{
    os << "[NM_Set_Time_Regulating - Begin]" << std::endl;
    
    os << static_cast<const NM_Set_Time_Regulating::Super&>(msg); // show parent class
    
    // Specific display
    os << "  regulator = " << msg.regulator << std::endl;
    
    os << "[NM_Set_Time_Regulating - End]" << std::endl;
    return os;
}

NM_Set_Time_Constrained::NM_Set_Time_Constrained()
{
    this->messageName = "NM_Set_Time_Constrained";
    this->type = NetworkMessage::Type::SET_TIME_CONSTRAINED;
}

void NM_Set_Time_Constrained::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_bool(constrained);
}

void NM_Set_Time_Constrained::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    constrained = msgBuffer.read_bool();
}

void NM_Set_Time_Constrained::constrainedOn()
{
    constrained = true;
}

void NM_Set_Time_Constrained::constrainedOff()
{
    constrained = false;
}

bool NM_Set_Time_Constrained::isConstrainedOn() const
{
    return constrained;
}

std::ostream& operator<<(std::ostream& os, const NM_Set_Time_Constrained& msg)
{
    os << "[NM_Set_Time_Constrained - Begin]" << std::endl;
    
    os << static_cast<const NM_Set_Time_Constrained::Super&>(msg); // show parent class
    
    // Specific display
    os << "  constrained = " << msg.constrained << std::endl;
    
    os << "[NM_Set_Time_Constrained - End]" << std::endl;
    return os;
}

NM_Time_Regulation_Enabled::NM_Time_Regulation_Enabled()
{
    this->messageName = "NM_Time_Regulation_Enabled";
    this->type = NetworkMessage::Type::TIME_REGULATION_ENABLED;
}

NM_Time_Constrained_Enabled::NM_Time_Constrained_Enabled()
{
    this->messageName = "NM_Time_Constrained_Enabled";
    this->type = NetworkMessage::Type::TIME_CONSTRAINED_ENABLED;
}

NM_Set_Class_Relevance_Advisory_Switch::NM_Set_Class_Relevance_Advisory_Switch()
{
    this->messageName = "NM_Set_Class_Relevance_Advisory_Switch";
    this->type = NetworkMessage::Type::SET_CLASS_RELEVANCE_ADVISORY_SWITCH;
}

void NM_Set_Class_Relevance_Advisory_Switch::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_bool(classRelevanceAdvisorySwitch);
}

void NM_Set_Class_Relevance_Advisory_Switch::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    classRelevanceAdvisorySwitch = msgBuffer.read_bool();
}

void NM_Set_Class_Relevance_Advisory_Switch::classRelevanceAdvisorySwitchOn()
{
    classRelevanceAdvisorySwitch = true;
}

void NM_Set_Class_Relevance_Advisory_Switch::classRelevanceAdvisorySwitchOff()
{
    classRelevanceAdvisorySwitch = false;
}

bool NM_Set_Class_Relevance_Advisory_Switch::isClassRelevanceAdvisorySwitchOn() const
{
    return classRelevanceAdvisorySwitch;
}

std::ostream& operator<<(std::ostream& os, const NM_Set_Class_Relevance_Advisory_Switch& msg)
{
    os << "[NM_Set_Class_Relevance_Advisory_Switch - Begin]" << std::endl;
    
    os << static_cast<const NM_Set_Class_Relevance_Advisory_Switch::Super&>(msg); // show parent class
    
    // Specific display
    os << "  classRelevanceAdvisorySwitch = " << msg.classRelevanceAdvisorySwitch << std::endl;
    
    os << "[NM_Set_Class_Relevance_Advisory_Switch - End]" << std::endl;
    return os;
}

NM_Set_Interaction_Relevance_Advisory_Switch::NM_Set_Interaction_Relevance_Advisory_Switch()
{
    this->messageName = "NM_Set_Interaction_Relevance_Advisory_Switch";
    this->type = NetworkMessage::Type::SET_INTERACTION_RELEVANCE_ADVISORY_SWITCH;
}

void NM_Set_Interaction_Relevance_Advisory_Switch::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_bool(interactionRelevanceAdvisorySwitch);
}

void NM_Set_Interaction_Relevance_Advisory_Switch::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionRelevanceAdvisorySwitch = msgBuffer.read_bool();
}

void NM_Set_Interaction_Relevance_Advisory_Switch::interactionRelevanceAdvisorySwitchOn()
{
    interactionRelevanceAdvisorySwitch = true;
}

void NM_Set_Interaction_Relevance_Advisory_Switch::interactionRelevanceAdvisorySwitchOff()
{
    interactionRelevanceAdvisorySwitch = false;
}

bool NM_Set_Interaction_Relevance_Advisory_Switch::isInteractionRelevanceAdvisorySwitchOn() const
{
    return interactionRelevanceAdvisorySwitch;
}

std::ostream& operator<<(std::ostream& os, const NM_Set_Interaction_Relevance_Advisory_Switch& msg)
{
    os << "[NM_Set_Interaction_Relevance_Advisory_Switch - Begin]" << std::endl;
    
    os << static_cast<const NM_Set_Interaction_Relevance_Advisory_Switch::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionRelevanceAdvisorySwitch = " << msg.interactionRelevanceAdvisorySwitch << std::endl;
    
    os << "[NM_Set_Interaction_Relevance_Advisory_Switch - End]" << std::endl;
    return os;
}

NM_Set_Attribute_Relevance_Advisory_Switch::NM_Set_Attribute_Relevance_Advisory_Switch()
{
    this->messageName = "NM_Set_Attribute_Relevance_Advisory_Switch";
    this->type = NetworkMessage::Type::SET_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH;
}

void NM_Set_Attribute_Relevance_Advisory_Switch::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_bool(attributeRelevanceAdvisorySwitch);
}

void NM_Set_Attribute_Relevance_Advisory_Switch::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    attributeRelevanceAdvisorySwitch = msgBuffer.read_bool();
}

void NM_Set_Attribute_Relevance_Advisory_Switch::attributeRelevanceAdvisorySwitchOn()
{
    attributeRelevanceAdvisorySwitch = true;
}

void NM_Set_Attribute_Relevance_Advisory_Switch::attributeRelevanceAdvisorySwitchOff()
{
    attributeRelevanceAdvisorySwitch = false;
}

bool NM_Set_Attribute_Relevance_Advisory_Switch::isAttributeRelevanceAdvisorySwitchOn() const
{
    return attributeRelevanceAdvisorySwitch;
}

std::ostream& operator<<(std::ostream& os, const NM_Set_Attribute_Relevance_Advisory_Switch& msg)
{
    os << "[NM_Set_Attribute_Relevance_Advisory_Switch - Begin]" << std::endl;
    
    os << static_cast<const NM_Set_Attribute_Relevance_Advisory_Switch::Super&>(msg); // show parent class
    
    // Specific display
    os << "  attributeRelevanceAdvisorySwitch = " << msg.attributeRelevanceAdvisorySwitch << std::endl;
    
    os << "[NM_Set_Attribute_Relevance_Advisory_Switch - End]" << std::endl;
    return os;
}

NM_Set_Attribute_Scope_Advisory_Switch::NM_Set_Attribute_Scope_Advisory_Switch()
{
    this->messageName = "NM_Set_Attribute_Scope_Advisory_Switch";
    this->type = NetworkMessage::Type::SET_ATTRIBUTE_SCOPE_ADVISORY_SWITCH;
}

void NM_Set_Attribute_Scope_Advisory_Switch::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_bool(attributeScopeAdvisorySwitch);
}

void NM_Set_Attribute_Scope_Advisory_Switch::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    attributeScopeAdvisorySwitch = msgBuffer.read_bool();
}

void NM_Set_Attribute_Scope_Advisory_Switch::attributeScopeAdvisorySwitchOn()
{
    attributeScopeAdvisorySwitch = true;
}

void NM_Set_Attribute_Scope_Advisory_Switch::attributeScopeAdvisorySwitchOff()
{
    attributeScopeAdvisorySwitch = false;
}

bool NM_Set_Attribute_Scope_Advisory_Switch::isAttributeScopeAdvisorySwitchOn() const
{
    return attributeScopeAdvisorySwitch;
}

std::ostream& operator<<(std::ostream& os, const NM_Set_Attribute_Scope_Advisory_Switch& msg)
{
    os << "[NM_Set_Attribute_Scope_Advisory_Switch - Begin]" << std::endl;
    
    os << static_cast<const NM_Set_Attribute_Scope_Advisory_Switch::Super&>(msg); // show parent class
    
    // Specific display
    os << "  attributeScopeAdvisorySwitch = " << msg.attributeScopeAdvisorySwitch << std::endl;
    
    os << "[NM_Set_Attribute_Scope_Advisory_Switch - End]" << std::endl;
    return os;
}

NM_Register_Federation_Synchronization_Point::NM_Register_Federation_Synchronization_Point()
{
    this->messageName = "NM_Register_Federation_Synchronization_Point";
    this->type = NetworkMessage::Type::REGISTER_FEDERATION_SYNCHRONIZATION_POINT;
}

void NM_Register_Federation_Synchronization_Point::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_bool(exists);
    uint32_t federatesSize = federates.size();
    msgBuffer.write_uint32(federatesSize);
    for (uint32_t i = 0; i < federatesSize; ++i) {
        msgBuffer.write_uint32(federates[i]);
    }
}

void NM_Register_Federation_Synchronization_Point::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    exists = msgBuffer.read_bool();
    uint32_t federatesSize = msgBuffer.read_uint32();
    federates.resize(federatesSize);
    for (uint32_t i = 0; i < federatesSize; ++i) {
        federates[i] = static_cast<FederateHandle>(msgBuffer.read_uint32());
    }
}

const bool& NM_Register_Federation_Synchronization_Point::getExists() const
{
    return exists;
}

void NM_Register_Federation_Synchronization_Point::setExists(const bool& newExists)
{
    exists = newExists;
}

uint32_t NM_Register_Federation_Synchronization_Point::getFederatesSize() const
{
    return federates.size();
}

void NM_Register_Federation_Synchronization_Point::setFederatesSize(uint32_t num)
{
    federates.resize(num);
}

const std::vector<FederateHandle>& NM_Register_Federation_Synchronization_Point::getFederates() const
{
    return federates;
}

const FederateHandle& NM_Register_Federation_Synchronization_Point::getFederates(uint32_t rank) const
{
    return federates[rank];
}

FederateHandle& NM_Register_Federation_Synchronization_Point::getFederates(uint32_t rank)
{
    return federates[rank];
}

void NM_Register_Federation_Synchronization_Point::setFederates(const FederateHandle& newFederates, uint32_t rank)
{
    federates[rank] = newFederates;
}

void NM_Register_Federation_Synchronization_Point::removeFederates(uint32_t rank)
{
    federates.erase(federates.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const NM_Register_Federation_Synchronization_Point& msg)
{
    os << "[NM_Register_Federation_Synchronization_Point - Begin]" << std::endl;
    
    os << static_cast<const NM_Register_Federation_Synchronization_Point::Super&>(msg); // show parent class
    
    // Specific display
    os << "  exists = " << msg.exists << std::endl;
    os << "  federates [] =" << std::endl;
    for (const auto& element : msg.federates) {
        os << element;
    }
    os << std::endl;
    
    os << "[NM_Register_Federation_Synchronization_Point - End]" << std::endl;
    return os;
}

NM_Confirm_Synchronization_Point_Registration::NM_Confirm_Synchronization_Point_Registration()
{
    this->messageName = "NM_Confirm_Synchronization_Point_Registration";
    this->type = NetworkMessage::Type::CONFIRM_SYNCHRONIZATION_POINT_REGISTRATION;
}

void NM_Confirm_Synchronization_Point_Registration::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_bool(successIndicator);
    msgBuffer.write_bool(_hasFailureReason);
    if (_hasFailureReason) {
        msgBuffer.write_string(failureReason);
    }
}

void NM_Confirm_Synchronization_Point_Registration::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    successIndicator = msgBuffer.read_bool();
    _hasFailureReason = msgBuffer.read_bool();
    if (_hasFailureReason) {
        msgBuffer.read_string(failureReason);
    }
}

const bool& NM_Confirm_Synchronization_Point_Registration::getSuccessIndicator() const
{
    return successIndicator;
}

void NM_Confirm_Synchronization_Point_Registration::setSuccessIndicator(const bool& newSuccessIndicator)
{
    successIndicator = newSuccessIndicator;
}

const std::string& NM_Confirm_Synchronization_Point_Registration::getFailureReason() const
{
    return failureReason;
}

void NM_Confirm_Synchronization_Point_Registration::setFailureReason(const std::string& newFailureReason)
{
    _hasFailureReason = true;
    failureReason = newFailureReason;
}

bool NM_Confirm_Synchronization_Point_Registration::hasFailureReason() const
{
    return _hasFailureReason;
}

std::ostream& operator<<(std::ostream& os, const NM_Confirm_Synchronization_Point_Registration& msg)
{
    os << "[NM_Confirm_Synchronization_Point_Registration - Begin]" << std::endl;
    
    os << static_cast<const NM_Confirm_Synchronization_Point_Registration::Super&>(msg); // show parent class
    
    // Specific display
    os << "  successIndicator = " << msg.successIndicator << std::endl;
    os << "  (opt) failureReason =" << msg.failureReason << std::endl;
    
    os << "[NM_Confirm_Synchronization_Point_Registration - End]" << std::endl;
    return os;
}

NM_Announce_Synchronization_Point::NM_Announce_Synchronization_Point()
{
    this->messageName = "NM_Announce_Synchronization_Point";
    this->type = NetworkMessage::Type::ANNOUNCE_SYNCHRONIZATION_POINT;
}

NM_Synchronization_Point_Achieved::NM_Synchronization_Point_Achieved()
{
    this->messageName = "NM_Synchronization_Point_Achieved";
    this->type = NetworkMessage::Type::SYNCHRONIZATION_POINT_ACHIEVED;
}

NM_Federation_Synchronized::NM_Federation_Synchronized()
{
    this->messageName = "NM_Federation_Synchronized";
    this->type = NetworkMessage::Type::FEDERATION_SYNCHRONIZED;
}

NM_Request_Federation_Save::NM_Request_Federation_Save()
{
    this->messageName = "NM_Request_Federation_Save";
    this->type = NetworkMessage::Type::REQUEST_FEDERATION_SAVE;
}

NM_Federate_Save_Begun::NM_Federate_Save_Begun()
{
    this->messageName = "NM_Federate_Save_Begun";
    this->type = NetworkMessage::Type::FEDERATE_SAVE_BEGUN;
}

NM_Federate_Save_Complete::NM_Federate_Save_Complete()
{
    this->messageName = "NM_Federate_Save_Complete";
    this->type = NetworkMessage::Type::FEDERATE_SAVE_COMPLETE;
}

NM_Federate_Save_Not_Complete::NM_Federate_Save_Not_Complete()
{
    this->messageName = "NM_Federate_Save_Not_Complete";
    this->type = NetworkMessage::Type::FEDERATE_SAVE_NOT_COMPLETE;
}

NM_Initiate_Federate_Save::NM_Initiate_Federate_Save()
{
    this->messageName = "NM_Initiate_Federate_Save";
    this->type = NetworkMessage::Type::INITIATE_FEDERATE_SAVE;
}

NM_Federation_Saved::NM_Federation_Saved()
{
    this->messageName = "NM_Federation_Saved";
    this->type = NetworkMessage::Type::FEDERATION_SAVED;
}

NM_Federation_Not_Saved::NM_Federation_Not_Saved()
{
    this->messageName = "NM_Federation_Not_Saved";
    this->type = NetworkMessage::Type::FEDERATION_NOT_SAVED;
}

NM_Request_Federation_Restore::NM_Request_Federation_Restore()
{
    this->messageName = "NM_Request_Federation_Restore";
    this->type = NetworkMessage::Type::REQUEST_FEDERATION_RESTORE;
}

NM_Federate_Restore_Complete::NM_Federate_Restore_Complete()
{
    this->messageName = "NM_Federate_Restore_Complete";
    this->type = NetworkMessage::Type::FEDERATE_RESTORE_COMPLETE;
}

NM_Federate_Restore_Not_Complete::NM_Federate_Restore_Not_Complete()
{
    this->messageName = "NM_Federate_Restore_Not_Complete";
    this->type = NetworkMessage::Type::FEDERATE_RESTORE_NOT_COMPLETE;
}

NM_Request_Federation_Restore_Succeeded::NM_Request_Federation_Restore_Succeeded()
{
    this->messageName = "NM_Request_Federation_Restore_Succeeded";
    this->type = NetworkMessage::Type::REQUEST_FEDERATION_RESTORE_SUCCEEDED;
}

NM_Request_Federation_Restore_Failed::NM_Request_Federation_Restore_Failed()
{
    this->messageName = "NM_Request_Federation_Restore_Failed";
    this->type = NetworkMessage::Type::REQUEST_FEDERATION_RESTORE_FAILED;
}

NM_Federation_Restore_Begun::NM_Federation_Restore_Begun()
{
    this->messageName = "NM_Federation_Restore_Begun";
    this->type = NetworkMessage::Type::FEDERATION_RESTORE_BEGUN;
}

NM_Initiate_Federate_Restore::NM_Initiate_Federate_Restore()
{
    this->messageName = "NM_Initiate_Federate_Restore";
    this->type = NetworkMessage::Type::INITIATE_FEDERATE_RESTORE;
}

NM_Federation_Restored::NM_Federation_Restored()
{
    this->messageName = "NM_Federation_Restored";
    this->type = NetworkMessage::Type::FEDERATION_RESTORED;
}

NM_Federation_Not_Restored::NM_Federation_Not_Restored()
{
    this->messageName = "NM_Federation_Not_Restored";
    this->type = NetworkMessage::Type::FEDERATION_NOT_RESTORED;
}

NM_Publish_Object_Class::NM_Publish_Object_Class()
{
    this->messageName = "NM_Publish_Object_Class";
    this->type = NetworkMessage::Type::PUBLISH_OBJECT_CLASS;
}

void NM_Publish_Object_Class::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
}

void NM_Publish_Object_Class::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
}

const ObjectClassHandle& NM_Publish_Object_Class::getObjectClass() const
{
    return objectClass;
}

void NM_Publish_Object_Class::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

uint32_t NM_Publish_Object_Class::getAttributesSize() const
{
    return attributes.size();
}

void NM_Publish_Object_Class::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& NM_Publish_Object_Class::getAttributes() const
{
    return attributes;
}

const AttributeHandle& NM_Publish_Object_Class::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& NM_Publish_Object_Class::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void NM_Publish_Object_Class::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void NM_Publish_Object_Class::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const NM_Publish_Object_Class& msg)
{
    os << "[NM_Publish_Object_Class - Begin]" << std::endl;
    
    os << static_cast<const NM_Publish_Object_Class::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[NM_Publish_Object_Class - End]" << std::endl;
    return os;
}

NM_Unpublish_Object_Class::NM_Unpublish_Object_Class()
{
    this->messageName = "NM_Unpublish_Object_Class";
    this->type = NetworkMessage::Type::UNPUBLISH_OBJECT_CLASS;
}

NM_Publish_Interaction_Class::NM_Publish_Interaction_Class()
{
    this->messageName = "NM_Publish_Interaction_Class";
    this->type = NetworkMessage::Type::PUBLISH_INTERACTION_CLASS;
}

void NM_Publish_Interaction_Class::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
}

void NM_Publish_Interaction_Class::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
}

const InteractionClassHandle& NM_Publish_Interaction_Class::getInteractionClass() const
{
    return interactionClass;
}

void NM_Publish_Interaction_Class::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

std::ostream& operator<<(std::ostream& os, const NM_Publish_Interaction_Class& msg)
{
    os << "[NM_Publish_Interaction_Class - Begin]" << std::endl;
    
    os << static_cast<const NM_Publish_Interaction_Class::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    
    os << "[NM_Publish_Interaction_Class - End]" << std::endl;
    return os;
}

NM_Unpublish_Interaction_Class::NM_Unpublish_Interaction_Class()
{
    this->messageName = "NM_Unpublish_Interaction_Class";
    this->type = NetworkMessage::Type::UNPUBLISH_INTERACTION_CLASS;
}

NM_Subscribe_Object_Class::NM_Subscribe_Object_Class()
{
    this->messageName = "NM_Subscribe_Object_Class";
    this->type = NetworkMessage::Type::SUBSCRIBE_OBJECT_CLASS;
}

void NM_Subscribe_Object_Class::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
}

void NM_Subscribe_Object_Class::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
}

const ObjectClassHandle& NM_Subscribe_Object_Class::getObjectClass() const
{
    return objectClass;
}

void NM_Subscribe_Object_Class::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

uint32_t NM_Subscribe_Object_Class::getAttributesSize() const
{
    return attributes.size();
}

void NM_Subscribe_Object_Class::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& NM_Subscribe_Object_Class::getAttributes() const
{
    return attributes;
}

const AttributeHandle& NM_Subscribe_Object_Class::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& NM_Subscribe_Object_Class::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void NM_Subscribe_Object_Class::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void NM_Subscribe_Object_Class::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const NM_Subscribe_Object_Class& msg)
{
    os << "[NM_Subscribe_Object_Class - Begin]" << std::endl;
    
    os << static_cast<const NM_Subscribe_Object_Class::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[NM_Subscribe_Object_Class - End]" << std::endl;
    return os;
}

NM_Unsubscribe_Object_Class::NM_Unsubscribe_Object_Class()
{
    this->messageName = "NM_Unsubscribe_Object_Class";
    this->type = NetworkMessage::Type::UNSUBSCRIBE_OBJECT_CLASS;
}

NM_Subscribe_Interaction_Class::NM_Subscribe_Interaction_Class()
{
    this->messageName = "NM_Subscribe_Interaction_Class";
    this->type = NetworkMessage::Type::SUBSCRIBE_INTERACTION_CLASS;
}

NM_Unsubscribe_Interaction_Class::NM_Unsubscribe_Interaction_Class()
{
    this->messageName = "NM_Unsubscribe_Interaction_Class";
    this->type = NetworkMessage::Type::UNSUBSCRIBE_INTERACTION_CLASS;
}

NM_Turn_Interactions_On::NM_Turn_Interactions_On()
{
    this->messageName = "NM_Turn_Interactions_On";
    this->type = NetworkMessage::Type::TURN_INTERACTIONS_ON;
}

NM_Turn_Interactions_Off::NM_Turn_Interactions_Off()
{
    this->messageName = "NM_Turn_Interactions_Off";
    this->type = NetworkMessage::Type::TURN_INTERACTIONS_OFF;
}

NM_Register_Object::NM_Register_Object()
{
    this->messageName = "NM_Register_Object";
    this->type = NetworkMessage::Type::REGISTER_OBJECT;
}

void NM_Register_Object::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_bool(_hasObjectName);
    if (_hasObjectName) {
        msgBuffer.write_string(objectName);
    }
    msgBuffer.write_uint32(object);
}

void NM_Register_Object::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    _hasObjectName = msgBuffer.read_bool();
    if (_hasObjectName) {
        msgBuffer.read_string(objectName);
    }
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
}

const ObjectClassHandle& NM_Register_Object::getObjectClass() const
{
    return objectClass;
}

void NM_Register_Object::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const std::string& NM_Register_Object::getObjectName() const
{
    return objectName;
}

void NM_Register_Object::setObjectName(const std::string& newObjectName)
{
    _hasObjectName = true;
    objectName = newObjectName;
}

bool NM_Register_Object::hasObjectName() const
{
    return _hasObjectName;
}

const ObjectHandle& NM_Register_Object::getObject() const
{
    return object;
}

void NM_Register_Object::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

std::ostream& operator<<(std::ostream& os, const NM_Register_Object& msg)
{
    os << "[NM_Register_Object - Begin]" << std::endl;
    
    os << static_cast<const NM_Register_Object::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  (opt) objectName =" << msg.objectName << std::endl;
    os << "  object = " << msg.object << std::endl;
    
    os << "[NM_Register_Object - End]" << std::endl;
    return os;
}

NM_Discover_Object::NM_Discover_Object()
{
    this->messageName = "NM_Discover_Object";
    this->type = NetworkMessage::Type::DISCOVER_OBJECT;
}

void NM_Discover_Object::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_uint32(object);
}

void NM_Discover_Object::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
}

const ObjectClassHandle& NM_Discover_Object::getObjectClass() const
{
    return objectClass;
}

void NM_Discover_Object::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const ObjectHandle& NM_Discover_Object::getObject() const
{
    return object;
}

void NM_Discover_Object::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

std::ostream& operator<<(std::ostream& os, const NM_Discover_Object& msg)
{
    os << "[NM_Discover_Object - Begin]" << std::endl;
    
    os << static_cast<const NM_Discover_Object::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  object = " << msg.object << std::endl;
    
    os << "[NM_Discover_Object - End]" << std::endl;
    return os;
}

NM_Update_Attribute_Values::NM_Update_Attribute_Values()
{
    this->messageName = "NM_Update_Attribute_Values";
    this->type = NetworkMessage::Type::UPDATE_ATTRIBUTE_VALUES;
}

void NM_Update_Attribute_Values::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    uint32_t valuesSize = values.size();
    msgBuffer.write_uint32(valuesSize);
    for (uint32_t i = 0; i < valuesSize; ++i) {
        //serialize native whose representation is 'repeated' byte 
        msgBuffer.write_uint32(values[i].size());
        msgBuffer.write_bytes(&(values[i][0]),values[i].size());
    }
    msgBuffer.write_bool(_hasEvent);
    if (_hasEvent) {
            }
}

void NM_Update_Attribute_Values::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    uint32_t valuesSize = msgBuffer.read_uint32();
    values.resize(valuesSize);
    for (uint32_t i = 0; i < valuesSize; ++i) {
        //deserialize native whose representation is 'repeated' byte 
        values[i].resize(msgBuffer.read_uint32());
        msgBuffer.read_bytes(&(values[i][0]),values[i].size());
    }
    _hasEvent = msgBuffer.read_bool();
    if (_hasEvent) {
            }
}

const ObjectHandle& NM_Update_Attribute_Values::getObject() const
{
    return object;
}

void NM_Update_Attribute_Values::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t NM_Update_Attribute_Values::getAttributesSize() const
{
    return attributes.size();
}

void NM_Update_Attribute_Values::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& NM_Update_Attribute_Values::getAttributes() const
{
    return attributes;
}

const AttributeHandle& NM_Update_Attribute_Values::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& NM_Update_Attribute_Values::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void NM_Update_Attribute_Values::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void NM_Update_Attribute_Values::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

uint32_t NM_Update_Attribute_Values::getValuesSize() const
{
    return values.size();
}

void NM_Update_Attribute_Values::setValuesSize(uint32_t num)
{
    values.resize(num);
}

const std::vector<AttributeValue_t>& NM_Update_Attribute_Values::getValues() const
{
    return values;
}

const AttributeValue_t& NM_Update_Attribute_Values::getValues(uint32_t rank) const
{
    return values[rank];
}

AttributeValue_t& NM_Update_Attribute_Values::getValues(uint32_t rank)
{
    return values[rank];
}

void NM_Update_Attribute_Values::setValues(const AttributeValue_t& newValues, uint32_t rank)
{
    values[rank] = newValues;
}

void NM_Update_Attribute_Values::removeValues(uint32_t rank)
{
    values.erase(values.begin() + rank);
}

const EventRetractionHandle& NM_Update_Attribute_Values::getEvent() const
{
    return event;
}

void NM_Update_Attribute_Values::setEvent(const EventRetractionHandle& newEvent)
{
    _hasEvent = true;
    event = newEvent;
}

bool NM_Update_Attribute_Values::hasEvent() const
{
    return _hasEvent;
}

std::ostream& operator<<(std::ostream& os, const NM_Update_Attribute_Values& msg)
{
    os << "[NM_Update_Attribute_Values - Begin]" << std::endl;
    
    os << static_cast<const NM_Update_Attribute_Values::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  values [] =" << std::endl;
    for (const auto& element : msg.values) {
        os << "// TODO field <values> of type <AttributeValue_t>";
        (void) element;
    }
    os << std::endl;
    os << "  (opt) event =" << "// TODO field <event> of type <EventRetractionHandle>" << std::endl;
    
    os << "[NM_Update_Attribute_Values - End]" << std::endl;
    return os;
}

NM_Reflect_Attribute_Values::NM_Reflect_Attribute_Values()
{
    this->messageName = "NM_Reflect_Attribute_Values";
    this->type = NetworkMessage::Type::REFLECT_ATTRIBUTE_VALUES;
}

void NM_Reflect_Attribute_Values::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    uint32_t valuesSize = values.size();
    msgBuffer.write_uint32(valuesSize);
    for (uint32_t i = 0; i < valuesSize; ++i) {
        //serialize native whose representation is 'repeated' byte 
        msgBuffer.write_uint32(values[i].size());
        msgBuffer.write_bytes(&(values[i][0]),values[i].size());
    }
    msgBuffer.write_bool(_hasEvent);
    if (_hasEvent) {
            }
}

void NM_Reflect_Attribute_Values::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    uint32_t valuesSize = msgBuffer.read_uint32();
    values.resize(valuesSize);
    for (uint32_t i = 0; i < valuesSize; ++i) {
        //deserialize native whose representation is 'repeated' byte 
        values[i].resize(msgBuffer.read_uint32());
        msgBuffer.read_bytes(&(values[i][0]),values[i].size());
    }
    _hasEvent = msgBuffer.read_bool();
    if (_hasEvent) {
            }
}

const ObjectHandle& NM_Reflect_Attribute_Values::getObject() const
{
    return object;
}

void NM_Reflect_Attribute_Values::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t NM_Reflect_Attribute_Values::getAttributesSize() const
{
    return attributes.size();
}

void NM_Reflect_Attribute_Values::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& NM_Reflect_Attribute_Values::getAttributes() const
{
    return attributes;
}

const AttributeHandle& NM_Reflect_Attribute_Values::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& NM_Reflect_Attribute_Values::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void NM_Reflect_Attribute_Values::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void NM_Reflect_Attribute_Values::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

uint32_t NM_Reflect_Attribute_Values::getValuesSize() const
{
    return values.size();
}

void NM_Reflect_Attribute_Values::setValuesSize(uint32_t num)
{
    values.resize(num);
}

const std::vector<AttributeValue_t>& NM_Reflect_Attribute_Values::getValues() const
{
    return values;
}

const AttributeValue_t& NM_Reflect_Attribute_Values::getValues(uint32_t rank) const
{
    return values[rank];
}

AttributeValue_t& NM_Reflect_Attribute_Values::getValues(uint32_t rank)
{
    return values[rank];
}

void NM_Reflect_Attribute_Values::setValues(const AttributeValue_t& newValues, uint32_t rank)
{
    values[rank] = newValues;
}

void NM_Reflect_Attribute_Values::removeValues(uint32_t rank)
{
    values.erase(values.begin() + rank);
}

const EventRetractionHandle& NM_Reflect_Attribute_Values::getEvent() const
{
    return event;
}

void NM_Reflect_Attribute_Values::setEvent(const EventRetractionHandle& newEvent)
{
    _hasEvent = true;
    event = newEvent;
}

bool NM_Reflect_Attribute_Values::hasEvent() const
{
    return _hasEvent;
}

std::ostream& operator<<(std::ostream& os, const NM_Reflect_Attribute_Values& msg)
{
    os << "[NM_Reflect_Attribute_Values - Begin]" << std::endl;
    
    os << static_cast<const NM_Reflect_Attribute_Values::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  values [] =" << std::endl;
    for (const auto& element : msg.values) {
        os << "// TODO field <values> of type <AttributeValue_t>";
        (void) element;
    }
    os << std::endl;
    os << "  (opt) event =" << "// TODO field <event> of type <EventRetractionHandle>" << std::endl;
    
    os << "[NM_Reflect_Attribute_Values - End]" << std::endl;
    return os;
}

NM_Send_Interaction::NM_Send_Interaction()
{
    this->messageName = "NM_Send_Interaction";
    this->type = NetworkMessage::Type::SEND_INTERACTION;
}

void NM_Send_Interaction::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    uint32_t parametersSize = parameters.size();
    msgBuffer.write_uint32(parametersSize);
    for (uint32_t i = 0; i < parametersSize; ++i) {
        msgBuffer.write_uint32(parameters[i]);
    }
    uint32_t valuesSize = values.size();
    msgBuffer.write_uint32(valuesSize);
    for (uint32_t i = 0; i < valuesSize; ++i) {
        //serialize native whose representation is 'repeated' byte 
        msgBuffer.write_uint32(values[i].size());
        msgBuffer.write_bytes(&(values[i][0]),values[i].size());
    }
    msgBuffer.write_uint32(region);
}

void NM_Send_Interaction::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    uint32_t parametersSize = msgBuffer.read_uint32();
    parameters.resize(parametersSize);
    for (uint32_t i = 0; i < parametersSize; ++i) {
        parameters[i] = static_cast<ParameterHandle>(msgBuffer.read_uint32());
    }
    uint32_t valuesSize = msgBuffer.read_uint32();
    values.resize(valuesSize);
    for (uint32_t i = 0; i < valuesSize; ++i) {
        //deserialize native whose representation is 'repeated' byte 
        values[i].resize(msgBuffer.read_uint32());
        msgBuffer.read_bytes(&(values[i][0]),values[i].size());
    }
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
}

const InteractionClassHandle& NM_Send_Interaction::getInteractionClass() const
{
    return interactionClass;
}

void NM_Send_Interaction::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

uint32_t NM_Send_Interaction::getParametersSize() const
{
    return parameters.size();
}

void NM_Send_Interaction::setParametersSize(uint32_t num)
{
    parameters.resize(num);
}

const std::vector<ParameterHandle>& NM_Send_Interaction::getParameters() const
{
    return parameters;
}

const ParameterHandle& NM_Send_Interaction::getParameters(uint32_t rank) const
{
    return parameters[rank];
}

ParameterHandle& NM_Send_Interaction::getParameters(uint32_t rank)
{
    return parameters[rank];
}

void NM_Send_Interaction::setParameters(const ParameterHandle& newParameters, uint32_t rank)
{
    parameters[rank] = newParameters;
}

void NM_Send_Interaction::removeParameters(uint32_t rank)
{
    parameters.erase(parameters.begin() + rank);
}

uint32_t NM_Send_Interaction::getValuesSize() const
{
    return values.size();
}

void NM_Send_Interaction::setValuesSize(uint32_t num)
{
    values.resize(num);
}

const std::vector<ParameterValue_t>& NM_Send_Interaction::getValues() const
{
    return values;
}

const ParameterValue_t& NM_Send_Interaction::getValues(uint32_t rank) const
{
    return values[rank];
}

ParameterValue_t& NM_Send_Interaction::getValues(uint32_t rank)
{
    return values[rank];
}

void NM_Send_Interaction::setValues(const ParameterValue_t& newValues, uint32_t rank)
{
    values[rank] = newValues;
}

void NM_Send_Interaction::removeValues(uint32_t rank)
{
    values.erase(values.begin() + rank);
}

const RegionHandle& NM_Send_Interaction::getRegion() const
{
    return region;
}

void NM_Send_Interaction::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

std::ostream& operator<<(std::ostream& os, const NM_Send_Interaction& msg)
{
    os << "[NM_Send_Interaction - Begin]" << std::endl;
    
    os << static_cast<const NM_Send_Interaction::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  parameters [] =" << std::endl;
    for (const auto& element : msg.parameters) {
        os << element;
    }
    os << std::endl;
    os << "  values [] =" << std::endl;
    for (const auto& element : msg.values) {
        os << "// TODO field <values> of type <ParameterValue_t>";
        (void) element;
    }
    os << std::endl;
    os << "  region = " << msg.region << std::endl;
    
    os << "[NM_Send_Interaction - End]" << std::endl;
    return os;
}

NM_Receive_Interaction::NM_Receive_Interaction()
{
    this->messageName = "NM_Receive_Interaction";
    this->type = NetworkMessage::Type::RECEIVE_INTERACTION;
}

void NM_Receive_Interaction::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    uint32_t parametersSize = parameters.size();
    msgBuffer.write_uint32(parametersSize);
    for (uint32_t i = 0; i < parametersSize; ++i) {
        msgBuffer.write_uint32(parameters[i]);
    }
    uint32_t valuesSize = values.size();
    msgBuffer.write_uint32(valuesSize);
    for (uint32_t i = 0; i < valuesSize; ++i) {
        //serialize native whose representation is 'repeated' byte 
        msgBuffer.write_uint32(values[i].size());
        msgBuffer.write_bytes(&(values[i][0]),values[i].size());
    }
    msgBuffer.write_bool(_hasEvent);
    if (_hasEvent) {
            }
}

void NM_Receive_Interaction::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    uint32_t parametersSize = msgBuffer.read_uint32();
    parameters.resize(parametersSize);
    for (uint32_t i = 0; i < parametersSize; ++i) {
        parameters[i] = static_cast<ParameterHandle>(msgBuffer.read_uint32());
    }
    uint32_t valuesSize = msgBuffer.read_uint32();
    values.resize(valuesSize);
    for (uint32_t i = 0; i < valuesSize; ++i) {
        //deserialize native whose representation is 'repeated' byte 
        values[i].resize(msgBuffer.read_uint32());
        msgBuffer.read_bytes(&(values[i][0]),values[i].size());
    }
    _hasEvent = msgBuffer.read_bool();
    if (_hasEvent) {
            }
}

const InteractionClassHandle& NM_Receive_Interaction::getInteractionClass() const
{
    return interactionClass;
}

void NM_Receive_Interaction::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

uint32_t NM_Receive_Interaction::getParametersSize() const
{
    return parameters.size();
}

void NM_Receive_Interaction::setParametersSize(uint32_t num)
{
    parameters.resize(num);
}

const std::vector<ParameterHandle>& NM_Receive_Interaction::getParameters() const
{
    return parameters;
}

const ParameterHandle& NM_Receive_Interaction::getParameters(uint32_t rank) const
{
    return parameters[rank];
}

ParameterHandle& NM_Receive_Interaction::getParameters(uint32_t rank)
{
    return parameters[rank];
}

void NM_Receive_Interaction::setParameters(const ParameterHandle& newParameters, uint32_t rank)
{
    parameters[rank] = newParameters;
}

void NM_Receive_Interaction::removeParameters(uint32_t rank)
{
    parameters.erase(parameters.begin() + rank);
}

uint32_t NM_Receive_Interaction::getValuesSize() const
{
    return values.size();
}

void NM_Receive_Interaction::setValuesSize(uint32_t num)
{
    values.resize(num);
}

const std::vector<ParameterValue_t>& NM_Receive_Interaction::getValues() const
{
    return values;
}

const ParameterValue_t& NM_Receive_Interaction::getValues(uint32_t rank) const
{
    return values[rank];
}

ParameterValue_t& NM_Receive_Interaction::getValues(uint32_t rank)
{
    return values[rank];
}

void NM_Receive_Interaction::setValues(const ParameterValue_t& newValues, uint32_t rank)
{
    values[rank] = newValues;
}

void NM_Receive_Interaction::removeValues(uint32_t rank)
{
    values.erase(values.begin() + rank);
}

const EventRetractionHandle& NM_Receive_Interaction::getEvent() const
{
    return event;
}

void NM_Receive_Interaction::setEvent(const EventRetractionHandle& newEvent)
{
    _hasEvent = true;
    event = newEvent;
}

bool NM_Receive_Interaction::hasEvent() const
{
    return _hasEvent;
}

std::ostream& operator<<(std::ostream& os, const NM_Receive_Interaction& msg)
{
    os << "[NM_Receive_Interaction - Begin]" << std::endl;
    
    os << static_cast<const NM_Receive_Interaction::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  parameters [] =" << std::endl;
    for (const auto& element : msg.parameters) {
        os << element;
    }
    os << std::endl;
    os << "  values [] =" << std::endl;
    for (const auto& element : msg.values) {
        os << "// TODO field <values> of type <ParameterValue_t>";
        (void) element;
    }
    os << std::endl;
    os << "  (opt) event =" << "// TODO field <event> of type <EventRetractionHandle>" << std::endl;
    
    os << "[NM_Receive_Interaction - End]" << std::endl;
    return os;
}

NM_Delete_Object::NM_Delete_Object()
{
    this->messageName = "NM_Delete_Object";
    this->type = NetworkMessage::Type::DELETE_OBJECT;
}

void NM_Delete_Object::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_bool(_hasEvent);
    if (_hasEvent) {
            }
}

void NM_Delete_Object::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    _hasEvent = msgBuffer.read_bool();
    if (_hasEvent) {
            }
}

const ObjectHandle& NM_Delete_Object::getObject() const
{
    return object;
}

void NM_Delete_Object::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const EventRetractionHandle& NM_Delete_Object::getEvent() const
{
    return event;
}

void NM_Delete_Object::setEvent(const EventRetractionHandle& newEvent)
{
    _hasEvent = true;
    event = newEvent;
}

bool NM_Delete_Object::hasEvent() const
{
    return _hasEvent;
}

std::ostream& operator<<(std::ostream& os, const NM_Delete_Object& msg)
{
    os << "[NM_Delete_Object - Begin]" << std::endl;
    
    os << static_cast<const NM_Delete_Object::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  (opt) event =" << "// TODO field <event> of type <EventRetractionHandle>" << std::endl;
    
    os << "[NM_Delete_Object - End]" << std::endl;
    return os;
}

NM_Remove_Object::NM_Remove_Object()
{
    this->messageName = "NM_Remove_Object";
    this->type = NetworkMessage::Type::REMOVE_OBJECT;
}

void NM_Remove_Object::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_bool(_hasEvent);
    if (_hasEvent) {
            }
}

void NM_Remove_Object::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    _hasEvent = msgBuffer.read_bool();
    if (_hasEvent) {
            }
}

const ObjectHandle& NM_Remove_Object::getObject() const
{
    return object;
}

void NM_Remove_Object::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const ObjectClassHandle& NM_Remove_Object::getObjectClass() const
{
    return objectClass;
}

void NM_Remove_Object::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const EventRetractionHandle& NM_Remove_Object::getEvent() const
{
    return event;
}

void NM_Remove_Object::setEvent(const EventRetractionHandle& newEvent)
{
    _hasEvent = true;
    event = newEvent;
}

bool NM_Remove_Object::hasEvent() const
{
    return _hasEvent;
}

std::ostream& operator<<(std::ostream& os, const NM_Remove_Object& msg)
{
    os << "[NM_Remove_Object - Begin]" << std::endl;
    
    os << static_cast<const NM_Remove_Object::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  (opt) event =" << "// TODO field <event> of type <EventRetractionHandle>" << std::endl;
    
    os << "[NM_Remove_Object - End]" << std::endl;
    return os;
}

NM_Change_Attribute_Transport_Type::NM_Change_Attribute_Transport_Type()
{
    this->messageName = "NM_Change_Attribute_Transport_Type";
    this->type = NetworkMessage::Type::CHANGE_ATTRIBUTE_TRANSPORT_TYPE;
}

void NM_Change_Attribute_Transport_Type::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    msgBuffer.write_uint8(transport);
}

void NM_Change_Attribute_Transport_Type::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    transport = static_cast<TransportType>(msgBuffer.read_uint8());
}

const ObjectHandle& NM_Change_Attribute_Transport_Type::getObject() const
{
    return object;
}

void NM_Change_Attribute_Transport_Type::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t NM_Change_Attribute_Transport_Type::getAttributesSize() const
{
    return attributes.size();
}

void NM_Change_Attribute_Transport_Type::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& NM_Change_Attribute_Transport_Type::getAttributes() const
{
    return attributes;
}

const AttributeHandle& NM_Change_Attribute_Transport_Type::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& NM_Change_Attribute_Transport_Type::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void NM_Change_Attribute_Transport_Type::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void NM_Change_Attribute_Transport_Type::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

const TransportType& NM_Change_Attribute_Transport_Type::getTransport() const
{
    return transport;
}

void NM_Change_Attribute_Transport_Type::setTransport(const TransportType& newTransport)
{
    transport = newTransport;
}

std::ostream& operator<<(std::ostream& os, const NM_Change_Attribute_Transport_Type& msg)
{
    os << "[NM_Change_Attribute_Transport_Type - Begin]" << std::endl;
    
    os << static_cast<const NM_Change_Attribute_Transport_Type::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  transport = " << msg.transport << std::endl;
    
    os << "[NM_Change_Attribute_Transport_Type - End]" << std::endl;
    return os;
}

NM_Change_Attribute_Order_Type::NM_Change_Attribute_Order_Type()
{
    this->messageName = "NM_Change_Attribute_Order_Type";
    this->type = NetworkMessage::Type::CHANGE_ATTRIBUTE_ORDER_TYPE;
}

void NM_Change_Attribute_Order_Type::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    msgBuffer.write_uint8(order);
}

void NM_Change_Attribute_Order_Type::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    order = static_cast<OrderType>(msgBuffer.read_uint8());
}

const ObjectHandle& NM_Change_Attribute_Order_Type::getObject() const
{
    return object;
}

void NM_Change_Attribute_Order_Type::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t NM_Change_Attribute_Order_Type::getAttributesSize() const
{
    return attributes.size();
}

void NM_Change_Attribute_Order_Type::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& NM_Change_Attribute_Order_Type::getAttributes() const
{
    return attributes;
}

const AttributeHandle& NM_Change_Attribute_Order_Type::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& NM_Change_Attribute_Order_Type::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void NM_Change_Attribute_Order_Type::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void NM_Change_Attribute_Order_Type::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

const OrderType& NM_Change_Attribute_Order_Type::getOrder() const
{
    return order;
}

void NM_Change_Attribute_Order_Type::setOrder(const OrderType& newOrder)
{
    order = newOrder;
}

std::ostream& operator<<(std::ostream& os, const NM_Change_Attribute_Order_Type& msg)
{
    os << "[NM_Change_Attribute_Order_Type - Begin]" << std::endl;
    
    os << static_cast<const NM_Change_Attribute_Order_Type::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  order = " << msg.order << std::endl;
    
    os << "[NM_Change_Attribute_Order_Type - End]" << std::endl;
    return os;
}

NM_Change_Interaction_Transport_Type::NM_Change_Interaction_Transport_Type()
{
    this->messageName = "NM_Change_Interaction_Transport_Type";
    this->type = NetworkMessage::Type::CHANGE_INTERACTION_TRANSPORT_TYPE;
}

void NM_Change_Interaction_Transport_Type::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    msgBuffer.write_uint8(transport);
}

void NM_Change_Interaction_Transport_Type::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    transport = static_cast<TransportType>(msgBuffer.read_uint8());
}

const InteractionClassHandle& NM_Change_Interaction_Transport_Type::getInteractionClass() const
{
    return interactionClass;
}

void NM_Change_Interaction_Transport_Type::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

const TransportType& NM_Change_Interaction_Transport_Type::getTransport() const
{
    return transport;
}

void NM_Change_Interaction_Transport_Type::setTransport(const TransportType& newTransport)
{
    transport = newTransport;
}

std::ostream& operator<<(std::ostream& os, const NM_Change_Interaction_Transport_Type& msg)
{
    os << "[NM_Change_Interaction_Transport_Type - Begin]" << std::endl;
    
    os << static_cast<const NM_Change_Interaction_Transport_Type::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  transport = " << msg.transport << std::endl;
    
    os << "[NM_Change_Interaction_Transport_Type - End]" << std::endl;
    return os;
}

NM_Change_Interaction_Order_Type::NM_Change_Interaction_Order_Type()
{
    this->messageName = "NM_Change_Interaction_Order_Type";
    this->type = NetworkMessage::Type::CHANGE_INTERACTION_ORDER_TYPE;
}

void NM_Change_Interaction_Order_Type::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    msgBuffer.write_uint8(order);
}

void NM_Change_Interaction_Order_Type::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    order = static_cast<OrderType>(msgBuffer.read_uint8());
}

const InteractionClassHandle& NM_Change_Interaction_Order_Type::getInteractionClass() const
{
    return interactionClass;
}

void NM_Change_Interaction_Order_Type::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

const OrderType& NM_Change_Interaction_Order_Type::getOrder() const
{
    return order;
}

void NM_Change_Interaction_Order_Type::setOrder(const OrderType& newOrder)
{
    order = newOrder;
}

std::ostream& operator<<(std::ostream& os, const NM_Change_Interaction_Order_Type& msg)
{
    os << "[NM_Change_Interaction_Order_Type - Begin]" << std::endl;
    
    os << static_cast<const NM_Change_Interaction_Order_Type::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  order = " << msg.order << std::endl;
    
    os << "[NM_Change_Interaction_Order_Type - End]" << std::endl;
    return os;
}

NM_Request_Class_Attribute_Value_Update::NM_Request_Class_Attribute_Value_Update()
{
    this->messageName = "NM_Request_Class_Attribute_Value_Update";
    this->type = NetworkMessage::Type::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE;
}

void NM_Request_Class_Attribute_Value_Update::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
}

void NM_Request_Class_Attribute_Value_Update::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
}

const ObjectClassHandle& NM_Request_Class_Attribute_Value_Update::getObjectClass() const
{
    return objectClass;
}

void NM_Request_Class_Attribute_Value_Update::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

uint32_t NM_Request_Class_Attribute_Value_Update::getAttributesSize() const
{
    return attributes.size();
}

void NM_Request_Class_Attribute_Value_Update::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& NM_Request_Class_Attribute_Value_Update::getAttributes() const
{
    return attributes;
}

const AttributeHandle& NM_Request_Class_Attribute_Value_Update::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& NM_Request_Class_Attribute_Value_Update::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void NM_Request_Class_Attribute_Value_Update::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void NM_Request_Class_Attribute_Value_Update::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const NM_Request_Class_Attribute_Value_Update& msg)
{
    os << "[NM_Request_Class_Attribute_Value_Update - Begin]" << std::endl;
    
    os << static_cast<const NM_Request_Class_Attribute_Value_Update::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[NM_Request_Class_Attribute_Value_Update - End]" << std::endl;
    return os;
}

NM_Request_Object_Attribute_Value_Update::NM_Request_Object_Attribute_Value_Update()
{
    this->messageName = "NM_Request_Object_Attribute_Value_Update";
    this->type = NetworkMessage::Type::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE;
}

void NM_Request_Object_Attribute_Value_Update::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
}

void NM_Request_Object_Attribute_Value_Update::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
}

const ObjectHandle& NM_Request_Object_Attribute_Value_Update::getObject() const
{
    return object;
}

void NM_Request_Object_Attribute_Value_Update::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t NM_Request_Object_Attribute_Value_Update::getAttributesSize() const
{
    return attributes.size();
}

void NM_Request_Object_Attribute_Value_Update::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& NM_Request_Object_Attribute_Value_Update::getAttributes() const
{
    return attributes;
}

const AttributeHandle& NM_Request_Object_Attribute_Value_Update::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& NM_Request_Object_Attribute_Value_Update::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void NM_Request_Object_Attribute_Value_Update::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void NM_Request_Object_Attribute_Value_Update::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const NM_Request_Object_Attribute_Value_Update& msg)
{
    os << "[NM_Request_Object_Attribute_Value_Update - Begin]" << std::endl;
    
    os << static_cast<const NM_Request_Object_Attribute_Value_Update::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[NM_Request_Object_Attribute_Value_Update - End]" << std::endl;
    return os;
}

NM_Is_Attribute_Owned_By_Federate::NM_Is_Attribute_Owned_By_Federate()
{
    this->messageName = "NM_Is_Attribute_Owned_By_Federate";
    this->type = NetworkMessage::Type::IS_ATTRIBUTE_OWNED_BY_FEDERATE;
}

void NM_Is_Attribute_Owned_By_Federate::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_uint32(attribute);
}

void NM_Is_Attribute_Owned_By_Federate::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    attribute = static_cast<AttributeHandle>(msgBuffer.read_uint32());
}

const ObjectHandle& NM_Is_Attribute_Owned_By_Federate::getObject() const
{
    return object;
}

void NM_Is_Attribute_Owned_By_Federate::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const AttributeHandle& NM_Is_Attribute_Owned_By_Federate::getAttribute() const
{
    return attribute;
}

void NM_Is_Attribute_Owned_By_Federate::setAttribute(const AttributeHandle& newAttribute)
{
    attribute = newAttribute;
}

std::ostream& operator<<(std::ostream& os, const NM_Is_Attribute_Owned_By_Federate& msg)
{
    os << "[NM_Is_Attribute_Owned_By_Federate - Begin]" << std::endl;
    
    os << static_cast<const NM_Is_Attribute_Owned_By_Federate::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attribute = " << msg.attribute << std::endl;
    
    os << "[NM_Is_Attribute_Owned_By_Federate - End]" << std::endl;
    return os;
}

NM_Query_Attribute_Ownership::NM_Query_Attribute_Ownership()
{
    this->messageName = "NM_Query_Attribute_Ownership";
    this->type = NetworkMessage::Type::QUERY_ATTRIBUTE_OWNERSHIP;
}

void NM_Query_Attribute_Ownership::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_uint32(attribute);
}

void NM_Query_Attribute_Ownership::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    attribute = static_cast<AttributeHandle>(msgBuffer.read_uint32());
}

const ObjectHandle& NM_Query_Attribute_Ownership::getObject() const
{
    return object;
}

void NM_Query_Attribute_Ownership::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const AttributeHandle& NM_Query_Attribute_Ownership::getAttribute() const
{
    return attribute;
}

void NM_Query_Attribute_Ownership::setAttribute(const AttributeHandle& newAttribute)
{
    attribute = newAttribute;
}

std::ostream& operator<<(std::ostream& os, const NM_Query_Attribute_Ownership& msg)
{
    os << "[NM_Query_Attribute_Ownership - Begin]" << std::endl;
    
    os << static_cast<const NM_Query_Attribute_Ownership::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attribute = " << msg.attribute << std::endl;
    
    os << "[NM_Query_Attribute_Ownership - End]" << std::endl;
    return os;
}

NM_Attribute_Is_Not_Owned::NM_Attribute_Is_Not_Owned()
{
    this->messageName = "NM_Attribute_Is_Not_Owned";
    this->type = NetworkMessage::Type::ATTRIBUTE_IS_NOT_OWNED;
}

void NM_Attribute_Is_Not_Owned::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_uint32(attribute);
}

void NM_Attribute_Is_Not_Owned::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    attribute = static_cast<AttributeHandle>(msgBuffer.read_uint32());
}

const ObjectHandle& NM_Attribute_Is_Not_Owned::getObject() const
{
    return object;
}

void NM_Attribute_Is_Not_Owned::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const AttributeHandle& NM_Attribute_Is_Not_Owned::getAttribute() const
{
    return attribute;
}

void NM_Attribute_Is_Not_Owned::setAttribute(const AttributeHandle& newAttribute)
{
    attribute = newAttribute;
}

std::ostream& operator<<(std::ostream& os, const NM_Attribute_Is_Not_Owned& msg)
{
    os << "[NM_Attribute_Is_Not_Owned - Begin]" << std::endl;
    
    os << static_cast<const NM_Attribute_Is_Not_Owned::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attribute = " << msg.attribute << std::endl;
    
    os << "[NM_Attribute_Is_Not_Owned - End]" << std::endl;
    return os;
}

NM_Inform_Attribute_Ownership::NM_Inform_Attribute_Ownership()
{
    this->messageName = "NM_Inform_Attribute_Ownership";
    this->type = NetworkMessage::Type::INFORM_ATTRIBUTE_OWNERSHIP;
}

void NM_Inform_Attribute_Ownership::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_uint32(attribute);
}

void NM_Inform_Attribute_Ownership::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    attribute = static_cast<AttributeHandle>(msgBuffer.read_uint32());
}

const ObjectHandle& NM_Inform_Attribute_Ownership::getObject() const
{
    return object;
}

void NM_Inform_Attribute_Ownership::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const AttributeHandle& NM_Inform_Attribute_Ownership::getAttribute() const
{
    return attribute;
}

void NM_Inform_Attribute_Ownership::setAttribute(const AttributeHandle& newAttribute)
{
    attribute = newAttribute;
}

std::ostream& operator<<(std::ostream& os, const NM_Inform_Attribute_Ownership& msg)
{
    os << "[NM_Inform_Attribute_Ownership - Begin]" << std::endl;
    
    os << static_cast<const NM_Inform_Attribute_Ownership::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attribute = " << msg.attribute << std::endl;
    
    os << "[NM_Inform_Attribute_Ownership - End]" << std::endl;
    return os;
}

NM_Attribute_Ownership_Base::NM_Attribute_Ownership_Base()
{
    this->messageName = "NM_Attribute_Ownership_Base";
    this->type = NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_BASE;
}

void NM_Attribute_Ownership_Base::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
}

void NM_Attribute_Ownership_Base::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
}

const ObjectHandle& NM_Attribute_Ownership_Base::getObject() const
{
    return object;
}

void NM_Attribute_Ownership_Base::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t NM_Attribute_Ownership_Base::getAttributesSize() const
{
    return attributes.size();
}

void NM_Attribute_Ownership_Base::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& NM_Attribute_Ownership_Base::getAttributes() const
{
    return attributes;
}

const AttributeHandle& NM_Attribute_Ownership_Base::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& NM_Attribute_Ownership_Base::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void NM_Attribute_Ownership_Base::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void NM_Attribute_Ownership_Base::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const NM_Attribute_Ownership_Base& msg)
{
    os << "[NM_Attribute_Ownership_Base - Begin]" << std::endl;
    
    os << static_cast<const NM_Attribute_Ownership_Base::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[NM_Attribute_Ownership_Base - End]" << std::endl;
    return os;
}

NM_Negotiated_Attribute_Ownership_Divestiture::NM_Negotiated_Attribute_Ownership_Divestiture()
{
    this->messageName = "NM_Negotiated_Attribute_Ownership_Divestiture";
    this->type = NetworkMessage::Type::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
}

NM_Attribute_Ownership_Acquisition_Notification::NM_Attribute_Ownership_Acquisition_Notification()
{
    this->messageName = "NM_Attribute_Ownership_Acquisition_Notification";
    this->type = NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION;
}

NM_Attribute_Ownership_Divestiture_Notification::NM_Attribute_Ownership_Divestiture_Notification()
{
    this->messageName = "NM_Attribute_Ownership_Divestiture_Notification";
    this->type = NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION;
}

NM_Request_Attribute_Ownership_Assumption::NM_Request_Attribute_Ownership_Assumption()
{
    this->messageName = "NM_Request_Attribute_Ownership_Assumption";
    this->type = NetworkMessage::Type::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION;
}

NM_Attribute_Ownership_Unavailable::NM_Attribute_Ownership_Unavailable()
{
    this->messageName = "NM_Attribute_Ownership_Unavailable";
    this->type = NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_UNAVAILABLE;
}

NM_Attribute_Ownership_Acquisition_If_Available::NM_Attribute_Ownership_Acquisition_If_Available()
{
    this->messageName = "NM_Attribute_Ownership_Acquisition_If_Available";
    this->type = NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE;
}

NM_Unconditional_Attribute_Ownership_Divestiture::NM_Unconditional_Attribute_Ownership_Divestiture()
{
    this->messageName = "NM_Unconditional_Attribute_Ownership_Divestiture";
    this->type = NetworkMessage::Type::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
}

NM_Attribute_Ownership_Acquisition::NM_Attribute_Ownership_Acquisition()
{
    this->messageName = "NM_Attribute_Ownership_Acquisition";
    this->type = NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_ACQUISITION;
}

NM_Request_Attribute_Ownership_Release::NM_Request_Attribute_Ownership_Release()
{
    this->messageName = "NM_Request_Attribute_Ownership_Release";
    this->type = NetworkMessage::Type::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE;
}

NM_Cancel_Negotiated_Attribute_Ownership_Divestiture::NM_Cancel_Negotiated_Attribute_Ownership_Divestiture()
{
    this->messageName = "NM_Cancel_Negotiated_Attribute_Ownership_Divestiture";
    this->type = NetworkMessage::Type::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
}

NM_Attribute_Ownership_Release_Response::NM_Attribute_Ownership_Release_Response()
{
    this->messageName = "NM_Attribute_Ownership_Release_Response";
    this->type = NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE;
}

NM_Cancel_Attribute_Ownership_Acquisition::NM_Cancel_Attribute_Ownership_Acquisition()
{
    this->messageName = "NM_Cancel_Attribute_Ownership_Acquisition";
    this->type = NetworkMessage::Type::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION;
}

NM_Confirm_Attribute_Ownership_Acquisition_Cancellation::NM_Confirm_Attribute_Ownership_Acquisition_Cancellation()
{
    this->messageName = "NM_Confirm_Attribute_Ownership_Acquisition_Cancellation";
    this->type = NetworkMessage::Type::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION;
}

NM_DDM_Create_Region::NM_DDM_Create_Region()
{
    this->messageName = "NM_DDM_Create_Region";
    this->type = NetworkMessage::Type::DDM_CREATE_REGION;
}

void NM_DDM_Create_Region::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    msgBuffer.write_bool(DDM_bool);
    msgBuffer.write_uint32(space);
    msgBuffer.write_uint32(region);
    msgBuffer.write_uint32(nbExtents);
}

void NM_DDM_Create_Region::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    DDM_bool = msgBuffer.read_bool();
    space = static_cast<SpaceHandle>(msgBuffer.read_uint32());
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
    nbExtents = msgBuffer.read_uint32();
}

uint32_t NM_DDM_Create_Region::getAttributesSize() const
{
    return attributes.size();
}

void NM_DDM_Create_Region::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& NM_DDM_Create_Region::getAttributes() const
{
    return attributes;
}

const AttributeHandle& NM_DDM_Create_Region::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& NM_DDM_Create_Region::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void NM_DDM_Create_Region::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void NM_DDM_Create_Region::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

const bool& NM_DDM_Create_Region::getDDM_bool() const
{
    return DDM_bool;
}

void NM_DDM_Create_Region::setDDM_bool(const bool& newDDM_bool)
{
    DDM_bool = newDDM_bool;
}

const SpaceHandle& NM_DDM_Create_Region::getSpace() const
{
    return space;
}

void NM_DDM_Create_Region::setSpace(const SpaceHandle& newSpace)
{
    space = newSpace;
}

const RegionHandle& NM_DDM_Create_Region::getRegion() const
{
    return region;
}

void NM_DDM_Create_Region::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

const uint32_t& NM_DDM_Create_Region::getNbExtents() const
{
    return nbExtents;
}

void NM_DDM_Create_Region::setNbExtents(const uint32_t& newNbExtents)
{
    nbExtents = newNbExtents;
}

std::ostream& operator<<(std::ostream& os, const NM_DDM_Create_Region& msg)
{
    os << "[NM_DDM_Create_Region - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Create_Region::Super&>(msg); // show parent class
    
    // Specific display
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  DDM_bool = " << msg.DDM_bool << std::endl;
    os << "  space = " << msg.space << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  nbExtents = " << msg.nbExtents << std::endl;
    
    os << "[NM_DDM_Create_Region - End]" << std::endl;
    return os;
}

NM_DDM_Modify_Region::NM_DDM_Modify_Region()
{
    this->messageName = "NM_DDM_Modify_Region";
    this->type = NetworkMessage::Type::DDM_MODIFY_REGION;
}

void NM_DDM_Modify_Region::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    msgBuffer.write_bool(DDM_bool);
    msgBuffer.write_uint32(region);
}

void NM_DDM_Modify_Region::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    DDM_bool = msgBuffer.read_bool();
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
}

uint32_t NM_DDM_Modify_Region::getAttributesSize() const
{
    return attributes.size();
}

void NM_DDM_Modify_Region::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& NM_DDM_Modify_Region::getAttributes() const
{
    return attributes;
}

const AttributeHandle& NM_DDM_Modify_Region::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& NM_DDM_Modify_Region::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void NM_DDM_Modify_Region::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void NM_DDM_Modify_Region::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

const bool& NM_DDM_Modify_Region::getDDM_bool() const
{
    return DDM_bool;
}

void NM_DDM_Modify_Region::setDDM_bool(const bool& newDDM_bool)
{
    DDM_bool = newDDM_bool;
}

const RegionHandle& NM_DDM_Modify_Region::getRegion() const
{
    return region;
}

void NM_DDM_Modify_Region::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

std::ostream& operator<<(std::ostream& os, const NM_DDM_Modify_Region& msg)
{
    os << "[NM_DDM_Modify_Region - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Modify_Region::Super&>(msg); // show parent class
    
    // Specific display
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  DDM_bool = " << msg.DDM_bool << std::endl;
    os << "  region = " << msg.region << std::endl;
    
    os << "[NM_DDM_Modify_Region - End]" << std::endl;
    return os;
}

NM_DDM_Delete_Region::NM_DDM_Delete_Region()
{
    this->messageName = "NM_DDM_Delete_Region";
    this->type = NetworkMessage::Type::DDM_DELETE_REGION;
}

void NM_DDM_Delete_Region::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(region);
}

void NM_DDM_Delete_Region::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
}

const RegionHandle& NM_DDM_Delete_Region::getRegion() const
{
    return region;
}

void NM_DDM_Delete_Region::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

std::ostream& operator<<(std::ostream& os, const NM_DDM_Delete_Region& msg)
{
    os << "[NM_DDM_Delete_Region - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Delete_Region::Super&>(msg); // show parent class
    
    // Specific display
    os << "  region = " << msg.region << std::endl;
    
    os << "[NM_DDM_Delete_Region - End]" << std::endl;
    return os;
}

NM_DDM_Associate_Region::NM_DDM_Associate_Region()
{
    this->messageName = "NM_DDM_Associate_Region";
    this->type = NetworkMessage::Type::DDM_ASSOCIATE_REGION;
}

void NM_DDM_Associate_Region::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    msgBuffer.write_bool(DDM_bool);
    msgBuffer.write_uint32(region);
    msgBuffer.write_uint32(object);
}

void NM_DDM_Associate_Region::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    DDM_bool = msgBuffer.read_bool();
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
}

uint32_t NM_DDM_Associate_Region::getAttributesSize() const
{
    return attributes.size();
}

void NM_DDM_Associate_Region::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& NM_DDM_Associate_Region::getAttributes() const
{
    return attributes;
}

const AttributeHandle& NM_DDM_Associate_Region::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& NM_DDM_Associate_Region::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void NM_DDM_Associate_Region::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void NM_DDM_Associate_Region::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

const bool& NM_DDM_Associate_Region::getDDM_bool() const
{
    return DDM_bool;
}

void NM_DDM_Associate_Region::setDDM_bool(const bool& newDDM_bool)
{
    DDM_bool = newDDM_bool;
}

const RegionHandle& NM_DDM_Associate_Region::getRegion() const
{
    return region;
}

void NM_DDM_Associate_Region::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

const ObjectHandle& NM_DDM_Associate_Region::getObject() const
{
    return object;
}

void NM_DDM_Associate_Region::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

std::ostream& operator<<(std::ostream& os, const NM_DDM_Associate_Region& msg)
{
    os << "[NM_DDM_Associate_Region - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Associate_Region::Super&>(msg); // show parent class
    
    // Specific display
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  DDM_bool = " << msg.DDM_bool << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  object = " << msg.object << std::endl;
    
    os << "[NM_DDM_Associate_Region - End]" << std::endl;
    return os;
}

NM_DDM_Unassociate_Region::NM_DDM_Unassociate_Region()
{
    this->messageName = "NM_DDM_Unassociate_Region";
    this->type = NetworkMessage::Type::DDM_UNASSOCIATE_REGION;
}

void NM_DDM_Unassociate_Region::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    msgBuffer.write_uint32(region);
    msgBuffer.write_uint32(object);
    msgBuffer.write_bool(DDM_bool);
}

void NM_DDM_Unassociate_Region::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    DDM_bool = msgBuffer.read_bool();
}

uint32_t NM_DDM_Unassociate_Region::getAttributesSize() const
{
    return attributes.size();
}

void NM_DDM_Unassociate_Region::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& NM_DDM_Unassociate_Region::getAttributes() const
{
    return attributes;
}

const AttributeHandle& NM_DDM_Unassociate_Region::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& NM_DDM_Unassociate_Region::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void NM_DDM_Unassociate_Region::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void NM_DDM_Unassociate_Region::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

const RegionHandle& NM_DDM_Unassociate_Region::getRegion() const
{
    return region;
}

void NM_DDM_Unassociate_Region::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

const ObjectHandle& NM_DDM_Unassociate_Region::getObject() const
{
    return object;
}

void NM_DDM_Unassociate_Region::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const bool& NM_DDM_Unassociate_Region::getDDM_bool() const
{
    return DDM_bool;
}

void NM_DDM_Unassociate_Region::setDDM_bool(const bool& newDDM_bool)
{
    DDM_bool = newDDM_bool;
}

std::ostream& operator<<(std::ostream& os, const NM_DDM_Unassociate_Region& msg)
{
    os << "[NM_DDM_Unassociate_Region - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Unassociate_Region::Super&>(msg); // show parent class
    
    // Specific display
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  object = " << msg.object << std::endl;
    os << "  DDM_bool = " << msg.DDM_bool << std::endl;
    
    os << "[NM_DDM_Unassociate_Region - End]" << std::endl;
    return os;
}

NM_DDM_Register_Object::NM_DDM_Register_Object()
{
    this->messageName = "NM_DDM_Register_Object";
    this->type = NetworkMessage::Type::DDM_REGISTER_OBJECT;
}

void NM_DDM_Register_Object::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    msgBuffer.write_uint32(region);
    msgBuffer.write_bool(_hasObjectInstanceName);
    if (_hasObjectInstanceName) {
        msgBuffer.write_string(objectInstanceName);
    }
    msgBuffer.write_uint32(object);
    msgBuffer.write_bool(DDM_bool);
}

void NM_DDM_Register_Object::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
    _hasObjectInstanceName = msgBuffer.read_bool();
    if (_hasObjectInstanceName) {
        msgBuffer.read_string(objectInstanceName);
    }
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    DDM_bool = msgBuffer.read_bool();
}

const ObjectClassHandle& NM_DDM_Register_Object::getObjectClass() const
{
    return objectClass;
}

void NM_DDM_Register_Object::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

uint32_t NM_DDM_Register_Object::getAttributesSize() const
{
    return attributes.size();
}

void NM_DDM_Register_Object::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& NM_DDM_Register_Object::getAttributes() const
{
    return attributes;
}

const AttributeHandle& NM_DDM_Register_Object::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& NM_DDM_Register_Object::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void NM_DDM_Register_Object::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void NM_DDM_Register_Object::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

const RegionHandle& NM_DDM_Register_Object::getRegion() const
{
    return region;
}

void NM_DDM_Register_Object::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

const std::string& NM_DDM_Register_Object::getObjectInstanceName() const
{
    return objectInstanceName;
}

void NM_DDM_Register_Object::setObjectInstanceName(const std::string& newObjectInstanceName)
{
    _hasObjectInstanceName = true;
    objectInstanceName = newObjectInstanceName;
}

bool NM_DDM_Register_Object::hasObjectInstanceName() const
{
    return _hasObjectInstanceName;
}

const ObjectHandle& NM_DDM_Register_Object::getObject() const
{
    return object;
}

void NM_DDM_Register_Object::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const bool& NM_DDM_Register_Object::getDDM_bool() const
{
    return DDM_bool;
}

void NM_DDM_Register_Object::setDDM_bool(const bool& newDDM_bool)
{
    DDM_bool = newDDM_bool;
}

std::ostream& operator<<(std::ostream& os, const NM_DDM_Register_Object& msg)
{
    os << "[NM_DDM_Register_Object - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Register_Object::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  (opt) objectInstanceName =" << msg.objectInstanceName << std::endl;
    os << "  object = " << msg.object << std::endl;
    os << "  DDM_bool = " << msg.DDM_bool << std::endl;
    
    os << "[NM_DDM_Register_Object - End]" << std::endl;
    return os;
}

NM_DDM_Subscribe_Attributes::NM_DDM_Subscribe_Attributes()
{
    this->messageName = "NM_DDM_Subscribe_Attributes";
    this->type = NetworkMessage::Type::DDM_SUBSCRIBE_ATTRIBUTES;
}

void NM_DDM_Subscribe_Attributes::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_uint32(region);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    msgBuffer.write_bool(DDM_bool);
}

void NM_DDM_Subscribe_Attributes::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    DDM_bool = msgBuffer.read_bool();
}

const ObjectClassHandle& NM_DDM_Subscribe_Attributes::getObjectClass() const
{
    return objectClass;
}

void NM_DDM_Subscribe_Attributes::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const RegionHandle& NM_DDM_Subscribe_Attributes::getRegion() const
{
    return region;
}

void NM_DDM_Subscribe_Attributes::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

uint32_t NM_DDM_Subscribe_Attributes::getAttributesSize() const
{
    return attributes.size();
}

void NM_DDM_Subscribe_Attributes::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& NM_DDM_Subscribe_Attributes::getAttributes() const
{
    return attributes;
}

const AttributeHandle& NM_DDM_Subscribe_Attributes::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& NM_DDM_Subscribe_Attributes::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void NM_DDM_Subscribe_Attributes::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void NM_DDM_Subscribe_Attributes::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

const bool& NM_DDM_Subscribe_Attributes::getDDM_bool() const
{
    return DDM_bool;
}

void NM_DDM_Subscribe_Attributes::setDDM_bool(const bool& newDDM_bool)
{
    DDM_bool = newDDM_bool;
}

std::ostream& operator<<(std::ostream& os, const NM_DDM_Subscribe_Attributes& msg)
{
    os << "[NM_DDM_Subscribe_Attributes - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Subscribe_Attributes::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  DDM_bool = " << msg.DDM_bool << std::endl;
    
    os << "[NM_DDM_Subscribe_Attributes - End]" << std::endl;
    return os;
}

NM_DDM_Unsubscribe_Attributes::NM_DDM_Unsubscribe_Attributes()
{
    this->messageName = "NM_DDM_Unsubscribe_Attributes";
    this->type = NetworkMessage::Type::DDM_UNSUBSCRIBE_ATTRIBUTES;
}

void NM_DDM_Unsubscribe_Attributes::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_uint32(region);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    msgBuffer.write_bool(DDM_bool);
}

void NM_DDM_Unsubscribe_Attributes::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    DDM_bool = msgBuffer.read_bool();
}

const ObjectClassHandle& NM_DDM_Unsubscribe_Attributes::getObjectClass() const
{
    return objectClass;
}

void NM_DDM_Unsubscribe_Attributes::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const RegionHandle& NM_DDM_Unsubscribe_Attributes::getRegion() const
{
    return region;
}

void NM_DDM_Unsubscribe_Attributes::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

uint32_t NM_DDM_Unsubscribe_Attributes::getAttributesSize() const
{
    return attributes.size();
}

void NM_DDM_Unsubscribe_Attributes::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& NM_DDM_Unsubscribe_Attributes::getAttributes() const
{
    return attributes;
}

const AttributeHandle& NM_DDM_Unsubscribe_Attributes::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& NM_DDM_Unsubscribe_Attributes::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void NM_DDM_Unsubscribe_Attributes::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void NM_DDM_Unsubscribe_Attributes::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

const bool& NM_DDM_Unsubscribe_Attributes::getDDM_bool() const
{
    return DDM_bool;
}

void NM_DDM_Unsubscribe_Attributes::setDDM_bool(const bool& newDDM_bool)
{
    DDM_bool = newDDM_bool;
}

std::ostream& operator<<(std::ostream& os, const NM_DDM_Unsubscribe_Attributes& msg)
{
    os << "[NM_DDM_Unsubscribe_Attributes - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Unsubscribe_Attributes::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  DDM_bool = " << msg.DDM_bool << std::endl;
    
    os << "[NM_DDM_Unsubscribe_Attributes - End]" << std::endl;
    return os;
}

NM_DDM_Subscribe_Interaction::NM_DDM_Subscribe_Interaction()
{
    this->messageName = "NM_DDM_Subscribe_Interaction";
    this->type = NetworkMessage::Type::DDM_SUBSCRIBE_INTERACTION;
}

void NM_DDM_Subscribe_Interaction::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    msgBuffer.write_uint32(region);
    msgBuffer.write_bool(DDM_bool);
}

void NM_DDM_Subscribe_Interaction::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
    DDM_bool = msgBuffer.read_bool();
}

const InteractionClassHandle& NM_DDM_Subscribe_Interaction::getInteractionClass() const
{
    return interactionClass;
}

void NM_DDM_Subscribe_Interaction::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

const RegionHandle& NM_DDM_Subscribe_Interaction::getRegion() const
{
    return region;
}

void NM_DDM_Subscribe_Interaction::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

const bool& NM_DDM_Subscribe_Interaction::getDDM_bool() const
{
    return DDM_bool;
}

void NM_DDM_Subscribe_Interaction::setDDM_bool(const bool& newDDM_bool)
{
    DDM_bool = newDDM_bool;
}

std::ostream& operator<<(std::ostream& os, const NM_DDM_Subscribe_Interaction& msg)
{
    os << "[NM_DDM_Subscribe_Interaction - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Subscribe_Interaction::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  DDM_bool = " << msg.DDM_bool << std::endl;
    
    os << "[NM_DDM_Subscribe_Interaction - End]" << std::endl;
    return os;
}

NM_DDM_Unsubscribe_Interaction::NM_DDM_Unsubscribe_Interaction()
{
    this->messageName = "NM_DDM_Unsubscribe_Interaction";
    this->type = NetworkMessage::Type::DDM_UNSUBSCRIBE_INTERACTION;
}

NM_Provide_Attribute_Value_Update::NM_Provide_Attribute_Value_Update()
{
    this->messageName = "NM_Provide_Attribute_Value_Update";
    this->type = NetworkMessage::Type::PROVIDE_ATTRIBUTE_VALUE_UPDATE;
}

NM_Start_Registration_For_Object_Class::NM_Start_Registration_For_Object_Class()
{
    this->messageName = "NM_Start_Registration_For_Object_Class";
    this->type = NetworkMessage::Type::START_REGISTRATION_FOR_OBJECT_CLASS;
}

void NM_Start_Registration_For_Object_Class::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
}

void NM_Start_Registration_For_Object_Class::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
}

const ObjectClassHandle& NM_Start_Registration_For_Object_Class::getObjectClass() const
{
    return objectClass;
}

void NM_Start_Registration_For_Object_Class::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

std::ostream& operator<<(std::ostream& os, const NM_Start_Registration_For_Object_Class& msg)
{
    os << "[NM_Start_Registration_For_Object_Class - Begin]" << std::endl;
    
    os << static_cast<const NM_Start_Registration_For_Object_Class::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    
    os << "[NM_Start_Registration_For_Object_Class - End]" << std::endl;
    return os;
}

NM_Stop_Registration_For_Object_Class::NM_Stop_Registration_For_Object_Class()
{
    this->messageName = "NM_Stop_Registration_For_Object_Class";
    this->type = NetworkMessage::Type::STOP_REGISTRATION_FOR_OBJECT_CLASS;
}

void NM_Stop_Registration_For_Object_Class::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
}

void NM_Stop_Registration_For_Object_Class::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
}

const ObjectClassHandle& NM_Stop_Registration_For_Object_Class::getObjectClass() const
{
    return objectClass;
}

void NM_Stop_Registration_For_Object_Class::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

std::ostream& operator<<(std::ostream& os, const NM_Stop_Registration_For_Object_Class& msg)
{
    os << "[NM_Stop_Registration_For_Object_Class - Begin]" << std::endl;
    
    os << static_cast<const NM_Stop_Registration_For_Object_Class::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    
    os << "[NM_Stop_Registration_For_Object_Class - End]" << std::endl;
    return os;
}

NM_Reserve_Object_Instance_Name::NM_Reserve_Object_Instance_Name()
{
    this->messageName = "NM_Reserve_Object_Instance_Name";
    this->type = NetworkMessage::Type::RESERVE_OBJECT_INSTANCE_NAME;
}

void NM_Reserve_Object_Instance_Name::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_string(objectName);
}

void NM_Reserve_Object_Instance_Name::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    msgBuffer.read_string(objectName);
}

const std::string& NM_Reserve_Object_Instance_Name::getObjectName() const
{
    return objectName;
}

void NM_Reserve_Object_Instance_Name::setObjectName(const std::string& newObjectName)
{
    objectName = newObjectName;
}

std::ostream& operator<<(std::ostream& os, const NM_Reserve_Object_Instance_Name& msg)
{
    os << "[NM_Reserve_Object_Instance_Name - Begin]" << std::endl;
    
    os << static_cast<const NM_Reserve_Object_Instance_Name::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectName = " << msg.objectName << std::endl;
    
    os << "[NM_Reserve_Object_Instance_Name - End]" << std::endl;
    return os;
}

NM_Reserve_Object_Instance_Name_Succeeded::NM_Reserve_Object_Instance_Name_Succeeded()
{
    this->messageName = "NM_Reserve_Object_Instance_Name_Succeeded";
    this->type = NetworkMessage::Type::RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED;
}

NM_Reserve_Object_Instance_Name_Failed::NM_Reserve_Object_Instance_Name_Failed()
{
    this->messageName = "NM_Reserve_Object_Instance_Name_Failed";
    this->type = NetworkMessage::Type::RESERVE_OBJECT_INSTANCE_NAME_FAILED;
}

NM_Message_Null_Prime::NM_Message_Null_Prime()
{
    this->messageName = "NM_Message_Null_Prime";
    this->type = NetworkMessage::Type::MESSAGE_NULL_PRIME;
}

void NM_Message_Null_Prime::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    // FIXME FIXME FIXME
    //don't know how to serialize native field <timestamp> of type <FederationTime>
    //probably no 'representation' given
}

void NM_Message_Null_Prime::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    // FIXME FIXME FIXME
    //don't know how to deserialize native field <timestamp> of type <FederationTime>
    //probably no 'representation' given
}

const FederationTime& NM_Message_Null_Prime::getTimestamp() const
{
    return timestamp;
}

void NM_Message_Null_Prime::setTimestamp(const FederationTime& newTimestamp)
{
    timestamp = newTimestamp;
}

std::ostream& operator<<(std::ostream& os, const NM_Message_Null_Prime& msg)
{
    os << "[NM_Message_Null_Prime - Begin]" << std::endl;
    
    os << static_cast<const NM_Message_Null_Prime::Super&>(msg); // show parent class
    
    // Specific display
    os << "  timestamp = " << "// TODO field <timestamp> of type <FederationTime>" << std::endl;
    
    os << "[NM_Message_Null_Prime - End]" << std::endl;
    return os;
}

NM_Enable_Asynchronous_Delivery::NM_Enable_Asynchronous_Delivery()
{
    this->messageName = "NM_Enable_Asynchronous_Delivery";
    this->type = NetworkMessage::Type::ENABLE_ASYNCHRONOUS_DELIVERY;
}

NM_Disable_Asynchronous_Delivery::NM_Disable_Asynchronous_Delivery()
{
    this->messageName = "NM_Disable_Asynchronous_Delivery";
    this->type = NetworkMessage::Type::DISABLE_ASYNCHRONOUS_DELIVERY;
}

NM_Time_Advance_Request::NM_Time_Advance_Request()
{
    this->messageName = "NM_Time_Advance_Request";
    this->type = NetworkMessage::Type::TIME_ADVANCE_REQUEST;
}

NM_Time_Advance_Request_Available::NM_Time_Advance_Request_Available()
{
    this->messageName = "NM_Time_Advance_Request_Available";
    this->type = NetworkMessage::Type::TIME_ADVANCE_REQUEST_AVAILABLE;
}

NM_Next_Message_Request::NM_Next_Message_Request()
{
    this->messageName = "NM_Next_Message_Request";
    this->type = NetworkMessage::Type::NEXT_MESSAGE_REQUEST;
}

NM_Next_Message_Request_Available::NM_Next_Message_Request_Available()
{
    this->messageName = "NM_Next_Message_Request_Available";
    this->type = NetworkMessage::Type::NEXT_MESSAGE_REQUEST_AVAILABLE;
}

NM_Mom_Status::NM_Mom_Status()
{
    this->messageName = "NM_Mom_Status";
    this->type = NetworkMessage::Type::MOM_STATUS;
}

void NM_Mom_Status::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_bool(momState);
    msgBuffer.write_uint32(updatePeriod);
}

void NM_Mom_Status::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    momState = msgBuffer.read_bool();
    updatePeriod = msgBuffer.read_uint32();
}

const bool& NM_Mom_Status::getMomState() const
{
    return momState;
}

void NM_Mom_Status::setMomState(const bool& newMomState)
{
    momState = newMomState;
}

const uint32_t& NM_Mom_Status::getUpdatePeriod() const
{
    return updatePeriod;
}

void NM_Mom_Status::setUpdatePeriod(const uint32_t& newUpdatePeriod)
{
    updatePeriod = newUpdatePeriod;
}

std::ostream& operator<<(std::ostream& os, const NM_Mom_Status& msg)
{
    os << "[NM_Mom_Status - Begin]" << std::endl;
    
    os << static_cast<const NM_Mom_Status::Super&>(msg); // show parent class
    
    // Specific display
    os << "  momState = " << msg.momState << std::endl;
    os << "  updatePeriod = " << msg.updatePeriod << std::endl;
    
    os << "[NM_Mom_Status - End]" << std::endl;
    return os;
}

NM_Time_State_Update::NM_Time_State_Update()
{
    this->messageName = "NM_Time_State_Update";
    this->type = NetworkMessage::Type::TIME_STATE_UPDATE;
}

void NM_Time_State_Update::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_double(lookahead);
    msgBuffer.write_bool(state);
    msgBuffer.write_double(galt);
    msgBuffer.write_double(lits);
}

void NM_Time_State_Update::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    lookahead = msgBuffer.read_double();
    state = msgBuffer.read_bool();
    galt = msgBuffer.read_double();
    lits = msgBuffer.read_double();
}

const double& NM_Time_State_Update::getLookahead() const
{
    return lookahead;
}

void NM_Time_State_Update::setLookahead(const double& newLookahead)
{
    lookahead = newLookahead;
}

const bool& NM_Time_State_Update::getState() const
{
    return state;
}

void NM_Time_State_Update::setState(const bool& newState)
{
    state = newState;
}

const double& NM_Time_State_Update::getGalt() const
{
    return galt;
}

void NM_Time_State_Update::setGalt(const double& newGalt)
{
    galt = newGalt;
}

const double& NM_Time_State_Update::getLits() const
{
    return lits;
}

void NM_Time_State_Update::setLits(const double& newLits)
{
    lits = newLits;
}

std::ostream& operator<<(std::ostream& os, const NM_Time_State_Update& msg)
{
    os << "[NM_Time_State_Update - Begin]" << std::endl;
    
    os << static_cast<const NM_Time_State_Update::Super&>(msg); // show parent class
    
    // Specific display
    os << "  lookahead = " << msg.lookahead << std::endl;
    os << "  state = " << msg.state << std::endl;
    os << "  galt = " << msg.galt << std::endl;
    os << "  lits = " << msg.lits << std::endl;
    
    os << "[NM_Time_State_Update - End]" << std::endl;
    return os;
}

void New_NetworkMessage::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Specific serialization code
    msgBuffer.write_uint32(type);
    msgBuffer.write_bool(_hasDate);
    if (_hasDate) {
        // FIXME FIXME FIXME
        //don't know how to serialize native field <date> of type <FederationTime>
        //probably no 'representation' given
    }
    msgBuffer.write_bool(_hasLabel);
    if (_hasLabel) {
        msgBuffer.write_string(label);
    }
    msgBuffer.write_bool(_hasTag);
    if (_hasTag) {
        msgBuffer.write_string(tag);
    }
}

void New_NetworkMessage::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Specific deserialization code
    type = msgBuffer.read_uint32();
    _hasDate = msgBuffer.read_bool();
    if (_hasDate) {
        // FIXME FIXME FIXME
        //don't know how to deserialize native field <date> of type <FederationTime>
        //probably no 'representation' given
    }
    _hasLabel = msgBuffer.read_bool();
    if (_hasLabel) {
        msgBuffer.read_string(label);
    }
    _hasTag = msgBuffer.read_bool();
    if (_hasTag) {
        msgBuffer.read_string(tag);
    }
}

const uint32_t& New_NetworkMessage::getType() const
{
    return type;
}

void New_NetworkMessage::setType(const uint32_t& newType)
{
    type = newType;
}

const FederationTime& New_NetworkMessage::getDate() const
{
    return date;
}

void New_NetworkMessage::setDate(const FederationTime& newDate)
{
    _hasDate = true;
    date = newDate;
}

bool New_NetworkMessage::hasDate() const
{
    return _hasDate;
}

const std::string& New_NetworkMessage::getLabel() const
{
    return label;
}

void New_NetworkMessage::setLabel(const std::string& newLabel)
{
    _hasLabel = true;
    label = newLabel;
}

bool New_NetworkMessage::hasLabel() const
{
    return _hasLabel;
}

const std::string& New_NetworkMessage::getTag() const
{
    return tag;
}

void New_NetworkMessage::setTag(const std::string& newTag)
{
    _hasTag = true;
    tag = newTag;
}

bool New_NetworkMessage::hasTag() const
{
    return _hasTag;
}

std::ostream& operator<<(std::ostream& os, const New_NetworkMessage& msg)
{
    os << "[New_NetworkMessage - Begin]" << std::endl;
    
    // Specific display
    os << "  type = " << msg.type << std::endl;
    os << "  (opt) date =" << "// TODO field <date> of type <FederationTime>" << std::endl;
    os << "  (opt) label =" << msg.label << std::endl;
    os << "  (opt) tag =" << msg.tag << std::endl;
    
    os << "[New_NetworkMessage - End]" << std::endl;
    return os;
}

NetworkMessage* NM_Factory::create(NM_Type type) throw (NetworkError ,NetworkSignal) { 
    NetworkMessage* msg = NULL;

    switch (type) {
        case NetworkMessage::Type::NOT_USED:
            throw NetworkError("NOT_USED message type should not be used!!");
            break;
        case NetworkMessage::Type::CLOSE_CONNEXION:
            msg = new NM_Close_Connexion();
            break;
        case NetworkMessage::Type::MESSAGE_NULL:
            msg = new NM_Message_Null();
            break;
        case NetworkMessage::Type::CREATE_FEDERATION_EXECUTION:
            msg = new NM_Create_Federation_Execution();
            break;
        case NetworkMessage::Type::DESTROY_FEDERATION_EXECUTION:
            msg = new NM_Destroy_Federation_Execution();
            break;
        case NetworkMessage::Type::JOIN_FEDERATION_EXECUTION:
            msg = new NM_Join_Federation_Execution();
            break;
        case NetworkMessage::Type::ADDITIONAL_FOM_MODULE:
            msg = new NM_Additional_Fom_Module();
            break;
        case NetworkMessage::Type::RESIGN_FEDERATION_EXECUTION:
            msg = new NM_Resign_Federation_Execution();
            break;
        case NetworkMessage::Type::SET_TIME_REGULATING:
            msg = new NM_Set_Time_Regulating();
            break;
        case NetworkMessage::Type::SET_TIME_CONSTRAINED:
            msg = new NM_Set_Time_Constrained();
            break;
        case NetworkMessage::Type::TIME_REGULATION_ENABLED:
            msg = new NM_Time_Regulation_Enabled();
            break;
        case NetworkMessage::Type::TIME_CONSTRAINED_ENABLED:
            msg = new NM_Time_Constrained_Enabled();
            break;
        case NetworkMessage::Type::SET_CLASS_RELEVANCE_ADVISORY_SWITCH:
            msg = new NM_Set_Class_Relevance_Advisory_Switch();
            break;
        case NetworkMessage::Type::SET_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
            msg = new NM_Set_Interaction_Relevance_Advisory_Switch();
            break;
        case NetworkMessage::Type::SET_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH:
            msg = new NM_Set_Attribute_Relevance_Advisory_Switch();
            break;
        case NetworkMessage::Type::SET_ATTRIBUTE_SCOPE_ADVISORY_SWITCH:
            msg = new NM_Set_Attribute_Scope_Advisory_Switch();
            break;
        case NetworkMessage::Type::REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
            msg = new NM_Register_Federation_Synchronization_Point();
            break;
        case NetworkMessage::Type::CONFIRM_SYNCHRONIZATION_POINT_REGISTRATION:
            msg = new NM_Confirm_Synchronization_Point_Registration();
            break;
        case NetworkMessage::Type::ANNOUNCE_SYNCHRONIZATION_POINT:
            msg = new NM_Announce_Synchronization_Point();
            break;
        case NetworkMessage::Type::SYNCHRONIZATION_POINT_ACHIEVED:
            msg = new NM_Synchronization_Point_Achieved();
            break;
        case NetworkMessage::Type::FEDERATION_SYNCHRONIZED:
            msg = new NM_Federation_Synchronized();
            break;
        case NetworkMessage::Type::REQUEST_FEDERATION_SAVE:
            msg = new NM_Request_Federation_Save();
            break;
        case NetworkMessage::Type::FEDERATE_SAVE_BEGUN:
            msg = new NM_Federate_Save_Begun();
            break;
        case NetworkMessage::Type::FEDERATE_SAVE_COMPLETE:
            msg = new NM_Federate_Save_Complete();
            break;
        case NetworkMessage::Type::FEDERATE_SAVE_NOT_COMPLETE:
            msg = new NM_Federate_Save_Not_Complete();
            break;
        case NetworkMessage::Type::INITIATE_FEDERATE_SAVE:
            msg = new NM_Initiate_Federate_Save();
            break;
        case NetworkMessage::Type::FEDERATION_SAVED:
            msg = new NM_Federation_Saved();
            break;
        case NetworkMessage::Type::FEDERATION_NOT_SAVED:
            msg = new NM_Federation_Not_Saved();
            break;
        case NetworkMessage::Type::REQUEST_FEDERATION_RESTORE:
            msg = new NM_Request_Federation_Restore();
            break;
        case NetworkMessage::Type::FEDERATE_RESTORE_COMPLETE:
            msg = new NM_Federate_Restore_Complete();
            break;
        case NetworkMessage::Type::FEDERATE_RESTORE_NOT_COMPLETE:
            msg = new NM_Federate_Restore_Not_Complete();
            break;
        case NetworkMessage::Type::REQUEST_FEDERATION_RESTORE_SUCCEEDED:
            msg = new NM_Request_Federation_Restore_Succeeded();
            break;
        case NetworkMessage::Type::REQUEST_FEDERATION_RESTORE_FAILED:
            msg = new NM_Request_Federation_Restore_Failed();
            break;
        case NetworkMessage::Type::FEDERATION_RESTORE_BEGUN:
            msg = new NM_Federation_Restore_Begun();
            break;
        case NetworkMessage::Type::INITIATE_FEDERATE_RESTORE:
            msg = new NM_Initiate_Federate_Restore();
            break;
        case NetworkMessage::Type::FEDERATION_RESTORED:
            msg = new NM_Federation_Restored();
            break;
        case NetworkMessage::Type::FEDERATION_NOT_RESTORED:
            msg = new NM_Federation_Not_Restored();
            break;
        case NetworkMessage::Type::PUBLISH_OBJECT_CLASS:
            msg = new NM_Publish_Object_Class();
            break;
        case NetworkMessage::Type::UNPUBLISH_OBJECT_CLASS:
            msg = new NM_Unpublish_Object_Class();
            break;
        case NetworkMessage::Type::PUBLISH_INTERACTION_CLASS:
            msg = new NM_Publish_Interaction_Class();
            break;
        case NetworkMessage::Type::UNPUBLISH_INTERACTION_CLASS:
            msg = new NM_Unpublish_Interaction_Class();
            break;
        case NetworkMessage::Type::SUBSCRIBE_OBJECT_CLASS:
            msg = new NM_Subscribe_Object_Class();
            break;
        case NetworkMessage::Type::UNSUBSCRIBE_OBJECT_CLASS:
            msg = new NM_Unsubscribe_Object_Class();
            break;
        case NetworkMessage::Type::SUBSCRIBE_INTERACTION_CLASS:
            msg = new NM_Subscribe_Interaction_Class();
            break;
        case NetworkMessage::Type::UNSUBSCRIBE_INTERACTION_CLASS:
            msg = new NM_Unsubscribe_Interaction_Class();
            break;
        case NetworkMessage::Type::TURN_INTERACTIONS_ON:
            msg = new NM_Turn_Interactions_On();
            break;
        case NetworkMessage::Type::TURN_INTERACTIONS_OFF:
            msg = new NM_Turn_Interactions_Off();
            break;
        case NetworkMessage::Type::REGISTER_OBJECT:
            msg = new NM_Register_Object();
            break;
        case NetworkMessage::Type::DISCOVER_OBJECT:
            msg = new NM_Discover_Object();
            break;
        case NetworkMessage::Type::UPDATE_ATTRIBUTE_VALUES:
            msg = new NM_Update_Attribute_Values();
            break;
        case NetworkMessage::Type::REFLECT_ATTRIBUTE_VALUES:
            msg = new NM_Reflect_Attribute_Values();
            break;
        case NetworkMessage::Type::SEND_INTERACTION:
            msg = new NM_Send_Interaction();
            break;
        case NetworkMessage::Type::RECEIVE_INTERACTION:
            msg = new NM_Receive_Interaction();
            break;
        case NetworkMessage::Type::DELETE_OBJECT:
            msg = new NM_Delete_Object();
            break;
        case NetworkMessage::Type::REMOVE_OBJECT:
            msg = new NM_Remove_Object();
            break;
        case NetworkMessage::Type::CHANGE_ATTRIBUTE_TRANSPORT_TYPE:
            msg = new NM_Change_Attribute_Transport_Type();
            break;
        case NetworkMessage::Type::CHANGE_ATTRIBUTE_ORDER_TYPE:
            msg = new NM_Change_Attribute_Order_Type();
            break;
        case NetworkMessage::Type::CHANGE_INTERACTION_TRANSPORT_TYPE:
            msg = new NM_Change_Interaction_Transport_Type();
            break;
        case NetworkMessage::Type::CHANGE_INTERACTION_ORDER_TYPE:
            msg = new NM_Change_Interaction_Order_Type();
            break;
        case NetworkMessage::Type::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE:
            msg = new NM_Request_Class_Attribute_Value_Update();
            break;
        case NetworkMessage::Type::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
            msg = new NM_Request_Object_Attribute_Value_Update();
            break;
        case NetworkMessage::Type::IS_ATTRIBUTE_OWNED_BY_FEDERATE:
            msg = new NM_Is_Attribute_Owned_By_Federate();
            break;
        case NetworkMessage::Type::QUERY_ATTRIBUTE_OWNERSHIP:
            msg = new NM_Query_Attribute_Ownership();
            break;
        case NetworkMessage::Type::ATTRIBUTE_IS_NOT_OWNED:
            msg = new NM_Attribute_Is_Not_Owned();
            break;
        case NetworkMessage::Type::INFORM_ATTRIBUTE_OWNERSHIP:
            msg = new NM_Inform_Attribute_Ownership();
            break;
        case NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_BASE:
            msg = new NM_Attribute_Ownership_Base();
            break;
        case NetworkMessage::Type::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
            msg = new NM_Negotiated_Attribute_Ownership_Divestiture();
            break;
        case NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
            msg = new NM_Attribute_Ownership_Acquisition_Notification();
            break;
        case NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
            msg = new NM_Attribute_Ownership_Divestiture_Notification();
            break;
        case NetworkMessage::Type::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
            msg = new NM_Request_Attribute_Ownership_Assumption();
            break;
        case NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
            msg = new NM_Attribute_Ownership_Unavailable();
            break;
        case NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
            msg = new NM_Attribute_Ownership_Acquisition_If_Available();
            break;
        case NetworkMessage::Type::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
            msg = new NM_Unconditional_Attribute_Ownership_Divestiture();
            break;
        case NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_ACQUISITION:
            msg = new NM_Attribute_Ownership_Acquisition();
            break;
        case NetworkMessage::Type::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
            msg = new NM_Request_Attribute_Ownership_Release();
            break;
        case NetworkMessage::Type::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
            msg = new NM_Cancel_Negotiated_Attribute_Ownership_Divestiture();
            break;
        case NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
            msg = new NM_Attribute_Ownership_Release_Response();
            break;
        case NetworkMessage::Type::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
            msg = new NM_Cancel_Attribute_Ownership_Acquisition();
            break;
        case NetworkMessage::Type::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
            msg = new NM_Confirm_Attribute_Ownership_Acquisition_Cancellation();
            break;
        case NetworkMessage::Type::DDM_CREATE_REGION:
            msg = new NM_DDM_Create_Region();
            break;
        case NetworkMessage::Type::DDM_MODIFY_REGION:
            msg = new NM_DDM_Modify_Region();
            break;
        case NetworkMessage::Type::DDM_DELETE_REGION:
            msg = new NM_DDM_Delete_Region();
            break;
        case NetworkMessage::Type::DDM_ASSOCIATE_REGION:
            msg = new NM_DDM_Associate_Region();
            break;
        case NetworkMessage::Type::DDM_UNASSOCIATE_REGION:
            msg = new NM_DDM_Unassociate_Region();
            break;
        case NetworkMessage::Type::DDM_REGISTER_OBJECT:
            msg = new NM_DDM_Register_Object();
            break;
        case NetworkMessage::Type::DDM_SUBSCRIBE_ATTRIBUTES:
            msg = new NM_DDM_Subscribe_Attributes();
            break;
        case NetworkMessage::Type::DDM_UNSUBSCRIBE_ATTRIBUTES:
            msg = new NM_DDM_Unsubscribe_Attributes();
            break;
        case NetworkMessage::Type::DDM_SUBSCRIBE_INTERACTION:
            msg = new NM_DDM_Subscribe_Interaction();
            break;
        case NetworkMessage::Type::DDM_UNSUBSCRIBE_INTERACTION:
            msg = new NM_DDM_Unsubscribe_Interaction();
            break;
        case NetworkMessage::Type::PROVIDE_ATTRIBUTE_VALUE_UPDATE:
            msg = new NM_Provide_Attribute_Value_Update();
            break;
        case NetworkMessage::Type::START_REGISTRATION_FOR_OBJECT_CLASS:
            msg = new NM_Start_Registration_For_Object_Class();
            break;
        case NetworkMessage::Type::STOP_REGISTRATION_FOR_OBJECT_CLASS:
            msg = new NM_Stop_Registration_For_Object_Class();
            break;
        case NetworkMessage::Type::RESERVE_OBJECT_INSTANCE_NAME:
            msg = new NM_Reserve_Object_Instance_Name();
            break;
        case NetworkMessage::Type::RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED:
            msg = new NM_Reserve_Object_Instance_Name_Succeeded();
            break;
        case NetworkMessage::Type::RESERVE_OBJECT_INSTANCE_NAME_FAILED:
            msg = new NM_Reserve_Object_Instance_Name_Failed();
            break;
        case NetworkMessage::Type::MESSAGE_NULL_PRIME:
            msg = new NM_Message_Null_Prime();
            break;
        case NetworkMessage::Type::ENABLE_ASYNCHRONOUS_DELIVERY:
            msg = new NM_Enable_Asynchronous_Delivery();
            break;
        case NetworkMessage::Type::DISABLE_ASYNCHRONOUS_DELIVERY:
            msg = new NM_Disable_Asynchronous_Delivery();
            break;
        case NetworkMessage::Type::TIME_ADVANCE_REQUEST:
            msg = new NM_Time_Advance_Request();
            break;
        case NetworkMessage::Type::TIME_ADVANCE_REQUEST_AVAILABLE:
            msg = new NM_Time_Advance_Request_Available();
            break;
        case NetworkMessage::Type::NEXT_MESSAGE_REQUEST:
            msg = new NM_Next_Message_Request();
            break;
        case NetworkMessage::Type::NEXT_MESSAGE_REQUEST_AVAILABLE:
            msg = new NM_Next_Message_Request_Available();
            break;
        case NetworkMessage::Type::MOM_STATUS:
            msg = new NM_Mom_Status();
            break;
        case NetworkMessage::Type::TIME_STATE_UPDATE:
            msg = new NM_Time_State_Update();
            break;
        case NetworkMessage::Type::LAST:
            throw NetworkError("LAST message type should not be used!!");
            break;
    } // end if switch (type)
    return msg;
} /* end of NM_Factory::create */

NetworkMessage* NM_Factory::receive(NMStreamType stream) throw (NetworkError ,NetworkSignal) { 
    // FIXME This is not thread safe
    static libhla::MessageBuffer msgBuffer;
    NetworkMessage  msgGen;
    NetworkMessage* msg;

    // receive generic message 
    msgGen.receive(stream,msgBuffer);
    // create specific message from type 
    msg = NM_Factory::create(msgGen.getMessageType());
    msgBuffer.assumeSizeFromReservedBytes();
    msg->deserialize(msgBuffer);
    return msg;
} /* end of NM_Factory::receive */ 

} // end of namespace certi 
