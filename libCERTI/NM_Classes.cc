// Generated on 2017 August Tue, 01 at 16:48:15 by the CERTI message generator
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

NM_FOM_Dimension::NM_FOM_Dimension() {
    handle=0;
    //name= <no default value in message spec using builtin>
}

void NM_FOM_Dimension::serialize(libhla::MessageBuffer& msgBuffer) {
    //Specific serialization code
    msgBuffer.write_uint32(handle);
    msgBuffer.write_string(name);
}

void NM_FOM_Dimension::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Specific deserialization code
    handle = static_cast<DimensionHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(name);
}

std::ostream& operator<<(std::ostream& os, const NM_FOM_Dimension& msg) {
    os << "[NM_FOM_Dimension - Begin]" << std::endl;
    
    // === Specific show code {
    os << "  handle = " << msg.handle << std::endl;
    os << "  name = " << msg.name << std::endl;
    // } Specific show code ===
    
    os << "[NM_FOM_Dimension - End]" << std::endl;
    return os;
}

NM_FOM_Routing_Space::NM_FOM_Routing_Space() {
    space=0;
    //name= <no default value in message spec using builtin>
    //dimensions= <no default value in message spec using builtin>
}

void NM_FOM_Routing_Space::serialize(libhla::MessageBuffer& msgBuffer) {
    //Specific serialization code
    msgBuffer.write_uint32(space);
    msgBuffer.write_string(name);
    uint32_t dimensionsSize = dimensions.size();
    msgBuffer.write_uint32(dimensionsSize);
    for (uint32_t i = 0; i < dimensionsSize; ++i) {
        dimensions[i].serialize(msgBuffer);
    }
}

void NM_FOM_Routing_Space::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Specific deserialization code
    space = static_cast<SpaceHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(name);
    uint32_t dimensionsSize = msgBuffer.read_uint32();
    dimensions.resize(dimensionsSize);
    for (uint32_t i = 0; i < dimensionsSize; ++i) {
        dimensions[i].deserialize(msgBuffer);
    }
}

std::ostream& operator<<(std::ostream& os, const NM_FOM_Routing_Space& msg) {
    os << "[NM_FOM_Routing_Space - Begin]" << std::endl;
    
    // === Specific show code {
    os << "  space = " << msg.space << std::endl;
    os << "  name = " << msg.name << std::endl;
    os << "  dimensions [] =" << std::endl;
    for (const auto& element: msg.dimensions) {
        os << element;
    }
    os << std::endl;
    // } Specific show code ===
    
    os << "[NM_FOM_Routing_Space - End]" << std::endl;
    return os;
}

NM_FOM_Attribute::NM_FOM_Attribute() {
    handle=0;
    //name= <no default value in message spec using builtin>
    spaceHandle=0;
    //order= <no default value in message spec using builtin>
    //transport= <no default value in message spec using builtin>
}

void NM_FOM_Attribute::serialize(libhla::MessageBuffer& msgBuffer) {
    //Specific serialization code
    msgBuffer.write_uint32(handle);
    msgBuffer.write_string(name);
    msgBuffer.write_uint32(spaceHandle);
    msgBuffer.write_uint8(order);
    msgBuffer.write_uint8(transport);
}

void NM_FOM_Attribute::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Specific deserialization code
    handle = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(name);
    spaceHandle = static_cast<SpaceHandle>(msgBuffer.read_uint32());
    order = static_cast<OrderType>(msgBuffer.read_uint8());
    transport = static_cast<TransportType>(msgBuffer.read_uint8());
}

std::ostream& operator<<(std::ostream& os, const NM_FOM_Attribute& msg) {
    os << "[NM_FOM_Attribute - Begin]" << std::endl;
    
    // === Specific show code {
    os << "  handle = " << msg.handle << std::endl;
    os << "  name = " << msg.name << std::endl;
    os << "  spaceHandle = " << msg.spaceHandle << std::endl;
    os << "  order = " << msg.order << std::endl;
    os << "  transport = " << msg.transport << std::endl;
    // } Specific show code ===
    
    os << "[NM_FOM_Attribute - End]" << std::endl;
    return os;
}

NM_FOM_Object_Class::NM_FOM_Object_Class() {
    handle=0;
    superClass=0;
    //name= <no default value in message spec using builtin>
    //attributes= <no default value in message spec using builtin>
}

void NM_FOM_Object_Class::serialize(libhla::MessageBuffer& msgBuffer) {
    //Specific serialization code
    msgBuffer.write_uint32(handle);
    msgBuffer.write_uint32(superClass);
    msgBuffer.write_string(name);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i].serialize(msgBuffer);
    }
}

void NM_FOM_Object_Class::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Specific deserialization code
    handle = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    superClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(name);
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i].deserialize(msgBuffer);
    }
}

std::ostream& operator<<(std::ostream& os, const NM_FOM_Object_Class& msg) {
    os << "[NM_FOM_Object_Class - Begin]" << std::endl;
    
    // === Specific show code {
    os << "  handle = " << msg.handle << std::endl;
    os << "  superClass = " << msg.superClass << std::endl;
    os << "  name = " << msg.name << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element: msg.attributes) {
        os << element;
    }
    os << std::endl;
    // } Specific show code ===
    
    os << "[NM_FOM_Object_Class - End]" << std::endl;
    return os;
}

NM_FOM_Parameter::NM_FOM_Parameter() {
    handle=0;
    //name= <no default value in message spec using builtin>
}

void NM_FOM_Parameter::serialize(libhla::MessageBuffer& msgBuffer) {
    //Specific serialization code
    msgBuffer.write_uint32(handle);
    msgBuffer.write_string(name);
}

void NM_FOM_Parameter::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Specific deserialization code
    handle = static_cast<ParameterHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(name);
}

std::ostream& operator<<(std::ostream& os, const NM_FOM_Parameter& msg) {
    os << "[NM_FOM_Parameter - Begin]" << std::endl;
    
    // === Specific show code {
    os << "  handle = " << msg.handle << std::endl;
    os << "  name = " << msg.name << std::endl;
    // } Specific show code ===
    
    os << "[NM_FOM_Parameter - End]" << std::endl;
    return os;
}

NM_FOM_Interaction_Class::NM_FOM_Interaction_Class() {
    interactionClass=0;
    //name= <no default value in message spec using builtin>
    superClass=0;
    space=0;
    //order= <no default value in message spec using builtin>
    //transport= <no default value in message spec using builtin>
    //parameters= <no default value in message spec using builtin>
}

void NM_FOM_Interaction_Class::serialize(libhla::MessageBuffer& msgBuffer) {
    //Specific serialization code
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

void NM_FOM_Interaction_Class::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Specific deserialization code
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

std::ostream& operator<<(std::ostream& os, const NM_FOM_Interaction_Class& msg) {
    os << "[NM_FOM_Interaction_Class - Begin]" << std::endl;
    
    // === Specific show code {
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  name = " << msg.name << std::endl;
    os << "  superClass = " << msg.superClass << std::endl;
    os << "  space = " << msg.space << std::endl;
    os << "  order = " << msg.order << std::endl;
    os << "  transport = " << msg.transport << std::endl;
    os << "  parameters [] =" << std::endl;
    for (const auto& element: msg.parameters) {
        os << element;
    }
    os << std::endl;
    // } Specific show code ===
    
    os << "[NM_FOM_Interaction_Class - End]" << std::endl;
    return os;
}

NM_Close_Connexion::NM_Close_Connexion() {
    this->messageName = "NM_Close_Connexion";
    this->type = NetworkMessage::CLOSE_CONNEXION;
}

NM_Message_Null::NM_Message_Null() {
    this->messageName = "NM_Message_Null";
    this->type = NetworkMessage::MESSAGE_NULL;
}

NM_Create_Federation_Execution::NM_Create_Federation_Execution() {
    this->messageName = "NM_Create_Federation_Execution";
    this->type = NetworkMessage::CREATE_FEDERATION_EXECUTION;
    //federationName= <no default value in message spec using builtin>
    //FEDid= <no default value in message spec using builtin>
}

void NM_Create_Federation_Execution::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_string(federationName);
    msgBuffer.write_string(FEDid);
}

void NM_Create_Federation_Execution::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    msgBuffer.read_string(federationName);
    msgBuffer.read_string(FEDid);
}

std::ostream& operator<<(std::ostream& os, const NM_Create_Federation_Execution& msg) {
    os << "[NM_Create_Federation_Execution - Begin]" << std::endl;
    
    os << static_cast<const NM_Create_Federation_Execution::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  federationName = " << msg.federationName << std::endl;
    os << "  FEDid = " << msg.FEDid << std::endl;
    // } Specific show code ===
    
    os << "[NM_Create_Federation_Execution - End]" << std::endl;
    return os;
}

NM_Destroy_Federation_Execution::NM_Destroy_Federation_Execution() {
    this->messageName = "NM_Destroy_Federation_Execution";
    this->type = NetworkMessage::DESTROY_FEDERATION_EXECUTION;
    //federationName= <no default value in message spec using builtin>
}

void NM_Destroy_Federation_Execution::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_string(federationName);
}

void NM_Destroy_Federation_Execution::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    msgBuffer.read_string(federationName);
}

std::ostream& operator<<(std::ostream& os, const NM_Destroy_Federation_Execution& msg) {
    os << "[NM_Destroy_Federation_Execution - Begin]" << std::endl;
    
    os << static_cast<const NM_Destroy_Federation_Execution::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  federationName = " << msg.federationName << std::endl;
    // } Specific show code ===
    
    os << "[NM_Destroy_Federation_Execution - End]" << std::endl;
    return os;
}

NM_Join_Federation_Execution::NM_Join_Federation_Execution() {
    this->messageName = "NM_Join_Federation_Execution";
    this->type = NetworkMessage::JOIN_FEDERATION_EXECUTION;
    //numberOfRegulators= <no default value in message spec using builtin>
    //multicastAddress= <no default value in message spec using builtin>
    //bestEffortAddress= <no default value in message spec using builtin>
    //bestEffortPeer= <no default value in message spec using builtin>
    //federationName= <no default value in message spec using builtin>
    //federateName= <no default value in message spec using builtin>
    //routingSpaces= <no default value in message spec using builtin>
    //objectClasses= <no default value in message spec using builtin>
    //interactionClasses= <no default value in message spec using builtin>
}

void NM_Join_Federation_Execution::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_int32(numberOfRegulators);
    msgBuffer.write_uint32(multicastAddress);
    msgBuffer.write_uint32(bestEffortAddress);
    msgBuffer.write_uint32(bestEffortPeer);
    msgBuffer.write_string(federationName);
    msgBuffer.write_string(federateName);
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

void NM_Join_Federation_Execution::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    numberOfRegulators = msgBuffer.read_int32();
    multicastAddress = msgBuffer.read_uint32();
    bestEffortAddress = msgBuffer.read_uint32();
    bestEffortPeer = msgBuffer.read_uint32();
    msgBuffer.read_string(federationName);
    msgBuffer.read_string(federateName);
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

std::ostream& operator<<(std::ostream& os, const NM_Join_Federation_Execution& msg) {
    os << "[NM_Join_Federation_Execution - Begin]" << std::endl;
    
    os << static_cast<const NM_Join_Federation_Execution::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  numberOfRegulators = " << msg.numberOfRegulators << std::endl;
    os << "  multicastAddress = " << msg.multicastAddress << std::endl;
    os << "  bestEffortAddress = " << msg.bestEffortAddress << std::endl;
    os << "  bestEffortPeer = " << msg.bestEffortPeer << std::endl;
    os << "  federationName = " << msg.federationName << std::endl;
    os << "  federateName = " << msg.federateName << std::endl;
    os << "  routingSpaces [] =" << std::endl;
    for (const auto& element: msg.routingSpaces) {
        os << element;
    }
    os << std::endl;
    os << "  objectClasses [] =" << std::endl;
    for (const auto& element: msg.objectClasses) {
        os << element;
    }
    os << std::endl;
    os << "  interactionClasses [] =" << std::endl;
    for (const auto& element: msg.interactionClasses) {
        os << element;
    }
    os << std::endl;
    // } Specific show code ===
    
    os << "[NM_Join_Federation_Execution - End]" << std::endl;
    return os;
}

NM_Resign_Federation_Execution::NM_Resign_Federation_Execution() {
    this->messageName = "NM_Resign_Federation_Execution";
    this->type = NetworkMessage::RESIGN_FEDERATION_EXECUTION;
}

NM_Set_Time_Regulating::NM_Set_Time_Regulating() {
    this->messageName = "NM_Set_Time_Regulating";
    this->type = NetworkMessage::SET_TIME_REGULATING;
    //regulator= <no default value in message spec using builtin>
}

void NM_Set_Time_Regulating::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_bool(regulator);
}

void NM_Set_Time_Regulating::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    regulator = msgBuffer.read_bool();
}

std::ostream& operator<<(std::ostream& os, const NM_Set_Time_Regulating& msg) {
    os << "[NM_Set_Time_Regulating - Begin]" << std::endl;
    
    os << static_cast<const NM_Set_Time_Regulating::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  regulator = " << msg.regulator << std::endl;
    // } Specific show code ===
    
    os << "[NM_Set_Time_Regulating - End]" << std::endl;
    return os;
}

NM_Set_Time_Constrained::NM_Set_Time_Constrained() {
    this->messageName = "NM_Set_Time_Constrained";
    this->type = NetworkMessage::SET_TIME_CONSTRAINED;
    //constrained= <no default value in message spec using builtin>
}

void NM_Set_Time_Constrained::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_bool(constrained);
}

void NM_Set_Time_Constrained::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    constrained = msgBuffer.read_bool();
}

std::ostream& operator<<(std::ostream& os, const NM_Set_Time_Constrained& msg) {
    os << "[NM_Set_Time_Constrained - Begin]" << std::endl;
    
    os << static_cast<const NM_Set_Time_Constrained::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  constrained = " << msg.constrained << std::endl;
    // } Specific show code ===
    
    os << "[NM_Set_Time_Constrained - End]" << std::endl;
    return os;
}

NM_Time_Regulation_Enabled::NM_Time_Regulation_Enabled() {
    this->messageName = "NM_Time_Regulation_Enabled";
    this->type = NetworkMessage::TIME_REGULATION_ENABLED;
}

NM_Time_Constrained_Enabled::NM_Time_Constrained_Enabled() {
    this->messageName = "NM_Time_Constrained_Enabled";
    this->type = NetworkMessage::TIME_CONSTRAINED_ENABLED;
}

NM_Set_Class_Relevance_Advisory_Switch::NM_Set_Class_Relevance_Advisory_Switch() {
    this->messageName = "NM_Set_Class_Relevance_Advisory_Switch";
    this->type = NetworkMessage::SET_CLASS_RELEVANCE_ADVISORY_SWITCH;
    //classRelevanceAdvisorySwitch= <no default value in message spec using builtin>
}

void NM_Set_Class_Relevance_Advisory_Switch::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_bool(classRelevanceAdvisorySwitch);
}

void NM_Set_Class_Relevance_Advisory_Switch::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    classRelevanceAdvisorySwitch = msgBuffer.read_bool();
}

std::ostream& operator<<(std::ostream& os, const NM_Set_Class_Relevance_Advisory_Switch& msg) {
    os << "[NM_Set_Class_Relevance_Advisory_Switch - Begin]" << std::endl;
    
    os << static_cast<const NM_Set_Class_Relevance_Advisory_Switch::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  classRelevanceAdvisorySwitch = " << msg.classRelevanceAdvisorySwitch << std::endl;
    // } Specific show code ===
    
    os << "[NM_Set_Class_Relevance_Advisory_Switch - End]" << std::endl;
    return os;
}

NM_Set_Interaction_Relevance_Advisory_Switch::NM_Set_Interaction_Relevance_Advisory_Switch() {
    this->messageName = "NM_Set_Interaction_Relevance_Advisory_Switch";
    this->type = NetworkMessage::SET_INTERACTION_RELEVANCE_ADVISORY_SWITCH;
    //interactionRelevanceAdvisorySwitch= <no default value in message spec using builtin>
}

void NM_Set_Interaction_Relevance_Advisory_Switch::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_bool(interactionRelevanceAdvisorySwitch);
}

void NM_Set_Interaction_Relevance_Advisory_Switch::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    interactionRelevanceAdvisorySwitch = msgBuffer.read_bool();
}

std::ostream& operator<<(std::ostream& os, const NM_Set_Interaction_Relevance_Advisory_Switch& msg) {
    os << "[NM_Set_Interaction_Relevance_Advisory_Switch - Begin]" << std::endl;
    
    os << static_cast<const NM_Set_Interaction_Relevance_Advisory_Switch::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  interactionRelevanceAdvisorySwitch = " << msg.interactionRelevanceAdvisorySwitch << std::endl;
    // } Specific show code ===
    
    os << "[NM_Set_Interaction_Relevance_Advisory_Switch - End]" << std::endl;
    return os;
}

NM_Set_Attribute_Relevance_Advisory_Switch::NM_Set_Attribute_Relevance_Advisory_Switch() {
    this->messageName = "NM_Set_Attribute_Relevance_Advisory_Switch";
    this->type = NetworkMessage::SET_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH;
    //attributeRelevanceAdvisorySwitch= <no default value in message spec using builtin>
}

void NM_Set_Attribute_Relevance_Advisory_Switch::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_bool(attributeRelevanceAdvisorySwitch);
}

void NM_Set_Attribute_Relevance_Advisory_Switch::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    attributeRelevanceAdvisorySwitch = msgBuffer.read_bool();
}

std::ostream& operator<<(std::ostream& os, const NM_Set_Attribute_Relevance_Advisory_Switch& msg) {
    os << "[NM_Set_Attribute_Relevance_Advisory_Switch - Begin]" << std::endl;
    
    os << static_cast<const NM_Set_Attribute_Relevance_Advisory_Switch::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  attributeRelevanceAdvisorySwitch = " << msg.attributeRelevanceAdvisorySwitch << std::endl;
    // } Specific show code ===
    
    os << "[NM_Set_Attribute_Relevance_Advisory_Switch - End]" << std::endl;
    return os;
}

NM_Set_Attribute_Scope_Advisory_Switch::NM_Set_Attribute_Scope_Advisory_Switch() {
    this->messageName = "NM_Set_Attribute_Scope_Advisory_Switch";
    this->type = NetworkMessage::SET_ATTRIBUTE_SCOPE_ADVISORY_SWITCH;
    //attributeScopeAdvisorySwitch= <no default value in message spec using builtin>
}

void NM_Set_Attribute_Scope_Advisory_Switch::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_bool(attributeScopeAdvisorySwitch);
}

void NM_Set_Attribute_Scope_Advisory_Switch::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    attributeScopeAdvisorySwitch = msgBuffer.read_bool();
}

std::ostream& operator<<(std::ostream& os, const NM_Set_Attribute_Scope_Advisory_Switch& msg) {
    os << "[NM_Set_Attribute_Scope_Advisory_Switch - Begin]" << std::endl;
    
    os << static_cast<const NM_Set_Attribute_Scope_Advisory_Switch::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  attributeScopeAdvisorySwitch = " << msg.attributeScopeAdvisorySwitch << std::endl;
    // } Specific show code ===
    
    os << "[NM_Set_Attribute_Scope_Advisory_Switch - End]" << std::endl;
    return os;
}

NM_Register_Federation_Synchronization_Point::NM_Register_Federation_Synchronization_Point() {
    this->messageName = "NM_Register_Federation_Synchronization_Point";
    this->type = NetworkMessage::REGISTER_FEDERATION_SYNCHRONIZATION_POINT;
    //exists= <no default value in message spec using builtin>
    //federates= <no default value in message spec using builtin>
}

void NM_Register_Federation_Synchronization_Point::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_bool(exists);
    uint32_t federatesSize = federates.size();
    msgBuffer.write_uint32(federatesSize);
    for (uint32_t i = 0; i < federatesSize; ++i) {
        msgBuffer.write_uint32(federates[i]);
    }
}

void NM_Register_Federation_Synchronization_Point::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    exists = msgBuffer.read_bool();
    uint32_t federatesSize = msgBuffer.read_uint32();
    federates.resize(federatesSize);
    for (uint32_t i = 0; i < federatesSize; ++i) {
        federates[i] = static_cast<FederateHandle>(msgBuffer.read_uint32());
    }
}

std::ostream& operator<<(std::ostream& os, const NM_Register_Federation_Synchronization_Point& msg) {
    os << "[NM_Register_Federation_Synchronization_Point - Begin]" << std::endl;
    
    os << static_cast<const NM_Register_Federation_Synchronization_Point::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  exists = " << msg.exists << std::endl;
    os << "  federates [] =" << std::endl;
    for (const auto& element: msg.federates) {
        os << element;
    }
    os << std::endl;
    // } Specific show code ===
    
    os << "[NM_Register_Federation_Synchronization_Point - End]" << std::endl;
    return os;
}

NM_Confirm_Synchronization_Point_Registration::NM_Confirm_Synchronization_Point_Registration() {
    this->messageName = "NM_Confirm_Synchronization_Point_Registration";
    this->type = NetworkMessage::CONFIRM_SYNCHRONIZATION_POINT_REGISTRATION;
    //successIndicator= <no default value in message spec using builtin>
    _hasFailureReason=false;
    //failureReason= <no default value in message spec using builtin>
}

void NM_Confirm_Synchronization_Point_Registration::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_bool(successIndicator);
    msgBuffer.write_bool(_hasFailureReason);
    if (_hasFailureReason) {
        msgBuffer.write_string(failureReason);
    }
}

void NM_Confirm_Synchronization_Point_Registration::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    successIndicator = msgBuffer.read_bool();
    _hasFailureReason = msgBuffer.read_bool();
    if (_hasFailureReason) {
        msgBuffer.read_string(failureReason);
    }
}

std::ostream& operator<<(std::ostream& os, const NM_Confirm_Synchronization_Point_Registration& msg) {
    os << "[NM_Confirm_Synchronization_Point_Registration - Begin]" << std::endl;
    
    os << static_cast<const NM_Confirm_Synchronization_Point_Registration::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  successIndicator = " << msg.successIndicator << std::endl;
    os << "  (opt) failureReason =" << msg.failureReason << std::endl;
    // } Specific show code ===
    
    os << "[NM_Confirm_Synchronization_Point_Registration - End]" << std::endl;
    return os;
}

NM_Announce_Synchronization_Point::NM_Announce_Synchronization_Point() {
    this->messageName = "NM_Announce_Synchronization_Point";
    this->type = NetworkMessage::ANNOUNCE_SYNCHRONIZATION_POINT;
}

NM_Synchronization_Point_Achieved::NM_Synchronization_Point_Achieved() {
    this->messageName = "NM_Synchronization_Point_Achieved";
    this->type = NetworkMessage::SYNCHRONIZATION_POINT_ACHIEVED;
}

NM_Federation_Synchronized::NM_Federation_Synchronized() {
    this->messageName = "NM_Federation_Synchronized";
    this->type = NetworkMessage::FEDERATION_SYNCHRONIZED;
}

NM_Request_Federation_Save::NM_Request_Federation_Save() {
    this->messageName = "NM_Request_Federation_Save";
    this->type = NetworkMessage::REQUEST_FEDERATION_SAVE;
}

NM_Federate_Save_Begun::NM_Federate_Save_Begun() {
    this->messageName = "NM_Federate_Save_Begun";
    this->type = NetworkMessage::FEDERATE_SAVE_BEGUN;
}

NM_Federate_Save_Complete::NM_Federate_Save_Complete() {
    this->messageName = "NM_Federate_Save_Complete";
    this->type = NetworkMessage::FEDERATE_SAVE_COMPLETE;
}

NM_Federate_Save_Not_Complete::NM_Federate_Save_Not_Complete() {
    this->messageName = "NM_Federate_Save_Not_Complete";
    this->type = NetworkMessage::FEDERATE_SAVE_NOT_COMPLETE;
}

NM_Initiate_Federate_Save::NM_Initiate_Federate_Save() {
    this->messageName = "NM_Initiate_Federate_Save";
    this->type = NetworkMessage::INITIATE_FEDERATE_SAVE;
}

NM_Federation_Saved::NM_Federation_Saved() {
    this->messageName = "NM_Federation_Saved";
    this->type = NetworkMessage::FEDERATION_SAVED;
}

NM_Federation_Not_Saved::NM_Federation_Not_Saved() {
    this->messageName = "NM_Federation_Not_Saved";
    this->type = NetworkMessage::FEDERATION_NOT_SAVED;
}

NM_Request_Federation_Restore::NM_Request_Federation_Restore() {
    this->messageName = "NM_Request_Federation_Restore";
    this->type = NetworkMessage::REQUEST_FEDERATION_RESTORE;
}

NM_Federate_Restore_Complete::NM_Federate_Restore_Complete() {
    this->messageName = "NM_Federate_Restore_Complete";
    this->type = NetworkMessage::FEDERATE_RESTORE_COMPLETE;
}

NM_Federate_Restore_Not_Complete::NM_Federate_Restore_Not_Complete() {
    this->messageName = "NM_Federate_Restore_Not_Complete";
    this->type = NetworkMessage::FEDERATE_RESTORE_NOT_COMPLETE;
}

NM_Request_Federation_Restore_Succeeded::NM_Request_Federation_Restore_Succeeded() {
    this->messageName = "NM_Request_Federation_Restore_Succeeded";
    this->type = NetworkMessage::REQUEST_FEDERATION_RESTORE_SUCCEEDED;
}

NM_Request_Federation_Restore_Failed::NM_Request_Federation_Restore_Failed() {
    this->messageName = "NM_Request_Federation_Restore_Failed";
    this->type = NetworkMessage::REQUEST_FEDERATION_RESTORE_FAILED;
}

NM_Federation_Restore_Begun::NM_Federation_Restore_Begun() {
    this->messageName = "NM_Federation_Restore_Begun";
    this->type = NetworkMessage::FEDERATION_RESTORE_BEGUN;
}

NM_Initiate_Federate_Restore::NM_Initiate_Federate_Restore() {
    this->messageName = "NM_Initiate_Federate_Restore";
    this->type = NetworkMessage::INITIATE_FEDERATE_RESTORE;
}

NM_Federation_Restored::NM_Federation_Restored() {
    this->messageName = "NM_Federation_Restored";
    this->type = NetworkMessage::FEDERATION_RESTORED;
}

NM_Federation_Not_Restored::NM_Federation_Not_Restored() {
    this->messageName = "NM_Federation_Not_Restored";
    this->type = NetworkMessage::FEDERATION_NOT_RESTORED;
}

NM_Publish_Object_Class::NM_Publish_Object_Class() {
    this->messageName = "NM_Publish_Object_Class";
    this->type = NetworkMessage::PUBLISH_OBJECT_CLASS;
    //objectClass= <no default value in message spec using builtin>
    //attributes= <no default value in message spec using builtin>
}

void NM_Publish_Object_Class::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(objectClass);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
}

void NM_Publish_Object_Class::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
}

std::ostream& operator<<(std::ostream& os, const NM_Publish_Object_Class& msg) {
    os << "[NM_Publish_Object_Class - Begin]" << std::endl;
    
    os << static_cast<const NM_Publish_Object_Class::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element: msg.attributes) {
        os << element;
    }
    os << std::endl;
    // } Specific show code ===
    
    os << "[NM_Publish_Object_Class - End]" << std::endl;
    return os;
}

NM_Unpublish_Object_Class::NM_Unpublish_Object_Class() {
    this->messageName = "NM_Unpublish_Object_Class";
    this->type = NetworkMessage::UNPUBLISH_OBJECT_CLASS;
}

NM_Publish_Interaction_Class::NM_Publish_Interaction_Class() {
    this->messageName = "NM_Publish_Interaction_Class";
    this->type = NetworkMessage::PUBLISH_INTERACTION_CLASS;
    //interactionClass= <no default value in message spec using builtin>
}

void NM_Publish_Interaction_Class::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(interactionClass);
}

void NM_Publish_Interaction_Class::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
}

std::ostream& operator<<(std::ostream& os, const NM_Publish_Interaction_Class& msg) {
    os << "[NM_Publish_Interaction_Class - Begin]" << std::endl;
    
    os << static_cast<const NM_Publish_Interaction_Class::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    // } Specific show code ===
    
    os << "[NM_Publish_Interaction_Class - End]" << std::endl;
    return os;
}

NM_Unpublish_Interaction_Class::NM_Unpublish_Interaction_Class() {
    this->messageName = "NM_Unpublish_Interaction_Class";
    this->type = NetworkMessage::UNPUBLISH_INTERACTION_CLASS;
}

NM_Subscribe_Object_Class::NM_Subscribe_Object_Class() {
    this->messageName = "NM_Subscribe_Object_Class";
    this->type = NetworkMessage::SUBSCRIBE_OBJECT_CLASS;
    //objectClass= <no default value in message spec using builtin>
    //attributes= <no default value in message spec using builtin>
}

void NM_Subscribe_Object_Class::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(objectClass);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
}

void NM_Subscribe_Object_Class::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
}

std::ostream& operator<<(std::ostream& os, const NM_Subscribe_Object_Class& msg) {
    os << "[NM_Subscribe_Object_Class - Begin]" << std::endl;
    
    os << static_cast<const NM_Subscribe_Object_Class::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element: msg.attributes) {
        os << element;
    }
    os << std::endl;
    // } Specific show code ===
    
    os << "[NM_Subscribe_Object_Class - End]" << std::endl;
    return os;
}

NM_Unsubscribe_Object_Class::NM_Unsubscribe_Object_Class() {
    this->messageName = "NM_Unsubscribe_Object_Class";
    this->type = NetworkMessage::UNSUBSCRIBE_OBJECT_CLASS;
}

NM_Subscribe_Interaction_Class::NM_Subscribe_Interaction_Class() {
    this->messageName = "NM_Subscribe_Interaction_Class";
    this->type = NetworkMessage::SUBSCRIBE_INTERACTION_CLASS;
}

NM_Unsubscribe_Interaction_Class::NM_Unsubscribe_Interaction_Class() {
    this->messageName = "NM_Unsubscribe_Interaction_Class";
    this->type = NetworkMessage::UNSUBSCRIBE_INTERACTION_CLASS;
}

NM_Turn_Interactions_On::NM_Turn_Interactions_On() {
    this->messageName = "NM_Turn_Interactions_On";
    this->type = NetworkMessage::TURN_INTERACTIONS_ON;
}

NM_Turn_Interactions_Off::NM_Turn_Interactions_Off() {
    this->messageName = "NM_Turn_Interactions_Off";
    this->type = NetworkMessage::TURN_INTERACTIONS_OFF;
}

NM_Register_Object::NM_Register_Object() {
    this->messageName = "NM_Register_Object";
    this->type = NetworkMessage::REGISTER_OBJECT;
    //objectClass= <no default value in message spec using builtin>
    _hasObjectName=false;
    //objectName= <no default value in message spec using builtin>
    //object= <no default value in message spec using builtin>
}

void NM_Register_Object::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_bool(_hasObjectName);
    if (_hasObjectName) {
        msgBuffer.write_string(objectName);
    }
    msgBuffer.write_uint32(object);
}

void NM_Register_Object::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    _hasObjectName = msgBuffer.read_bool();
    if (_hasObjectName) {
        msgBuffer.read_string(objectName);
    }
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
}

std::ostream& operator<<(std::ostream& os, const NM_Register_Object& msg) {
    os << "[NM_Register_Object - Begin]" << std::endl;
    
    os << static_cast<const NM_Register_Object::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  (opt) objectName =" << msg.objectName << std::endl;
    os << "  object = " << msg.object << std::endl;
    // } Specific show code ===
    
    os << "[NM_Register_Object - End]" << std::endl;
    return os;
}

NM_Discover_Object::NM_Discover_Object() {
    this->messageName = "NM_Discover_Object";
    this->type = NetworkMessage::DISCOVER_OBJECT;
    //objectClass= <no default value in message spec using builtin>
    //object= <no default value in message spec using builtin>
}

void NM_Discover_Object::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_uint32(object);
}

void NM_Discover_Object::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
}

std::ostream& operator<<(std::ostream& os, const NM_Discover_Object& msg) {
    os << "[NM_Discover_Object - Begin]" << std::endl;
    
    os << static_cast<const NM_Discover_Object::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  object = " << msg.object << std::endl;
    // } Specific show code ===
    
    os << "[NM_Discover_Object - End]" << std::endl;
    return os;
}

NM_Update_Attribute_Values::NM_Update_Attribute_Values() {
    this->messageName = "NM_Update_Attribute_Values";
    this->type = NetworkMessage::UPDATE_ATTRIBUTE_VALUES;
    //object= <no default value in message spec using builtin>
    //attributes= <no default value in message spec using builtin>
    //values= <no default value in message spec using builtin>
    _hasEvent=false;
    //event= <no default value in message spec using builtin>
}

void NM_Update_Attribute_Values::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
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

void NM_Update_Attribute_Values::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
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

std::ostream& operator<<(std::ostream& os, const NM_Update_Attribute_Values& msg) {
    os << "[NM_Update_Attribute_Values - Begin]" << std::endl;
    
    os << static_cast<const NM_Update_Attribute_Values::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element: msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  values [] =" << std::endl;
    for (const auto& element: msg.values) {
        os << "// TODO field <values> of type <AttributeValue_t>";
    }
    os << std::endl;
    os << "  (opt) event =" << "// TODO field <event> of type <EventRetractionHandle>" << std::endl;
    // } Specific show code ===
    
    os << "[NM_Update_Attribute_Values - End]" << std::endl;
    return os;
}

NM_Reflect_Attribute_Values::NM_Reflect_Attribute_Values() {
    this->messageName = "NM_Reflect_Attribute_Values";
    this->type = NetworkMessage::REFLECT_ATTRIBUTE_VALUES;
    //object= <no default value in message spec using builtin>
    //attributes= <no default value in message spec using builtin>
    //values= <no default value in message spec using builtin>
    _hasEvent=false;
    //event= <no default value in message spec using builtin>
}

void NM_Reflect_Attribute_Values::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
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

void NM_Reflect_Attribute_Values::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
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

std::ostream& operator<<(std::ostream& os, const NM_Reflect_Attribute_Values& msg) {
    os << "[NM_Reflect_Attribute_Values - Begin]" << std::endl;
    
    os << static_cast<const NM_Reflect_Attribute_Values::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element: msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  values [] =" << std::endl;
    for (const auto& element: msg.values) {
        os << "// TODO field <values> of type <AttributeValue_t>";
    }
    os << std::endl;
    os << "  (opt) event =" << "// TODO field <event> of type <EventRetractionHandle>" << std::endl;
    // } Specific show code ===
    
    os << "[NM_Reflect_Attribute_Values - End]" << std::endl;
    return os;
}

NM_Send_Interaction::NM_Send_Interaction() {
    this->messageName = "NM_Send_Interaction";
    this->type = NetworkMessage::SEND_INTERACTION;
    //interactionClass= <no default value in message spec using builtin>
    //parameters= <no default value in message spec using builtin>
    //values= <no default value in message spec using builtin>
    //region= <no default value in message spec using builtin>
}

void NM_Send_Interaction::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
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

void NM_Send_Interaction::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
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

std::ostream& operator<<(std::ostream& os, const NM_Send_Interaction& msg) {
    os << "[NM_Send_Interaction - Begin]" << std::endl;
    
    os << static_cast<const NM_Send_Interaction::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  parameters [] =" << std::endl;
    for (const auto& element: msg.parameters) {
        os << element;
    }
    os << std::endl;
    os << "  values [] =" << std::endl;
    for (const auto& element: msg.values) {
        os << "// TODO field <values> of type <ParameterValue_t>";
    }
    os << std::endl;
    os << "  region = " << msg.region << std::endl;
    // } Specific show code ===
    
    os << "[NM_Send_Interaction - End]" << std::endl;
    return os;
}

NM_Receive_Interaction::NM_Receive_Interaction() {
    this->messageName = "NM_Receive_Interaction";
    this->type = NetworkMessage::RECEIVE_INTERACTION;
    //interactionClass= <no default value in message spec using builtin>
    //parameters= <no default value in message spec using builtin>
    //values= <no default value in message spec using builtin>
    _hasEvent=false;
    //event= <no default value in message spec using builtin>
}

void NM_Receive_Interaction::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
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

void NM_Receive_Interaction::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
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

std::ostream& operator<<(std::ostream& os, const NM_Receive_Interaction& msg) {
    os << "[NM_Receive_Interaction - Begin]" << std::endl;
    
    os << static_cast<const NM_Receive_Interaction::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  parameters [] =" << std::endl;
    for (const auto& element: msg.parameters) {
        os << element;
    }
    os << std::endl;
    os << "  values [] =" << std::endl;
    for (const auto& element: msg.values) {
        os << "// TODO field <values> of type <ParameterValue_t>";
    }
    os << std::endl;
    os << "  (opt) event =" << "// TODO field <event> of type <EventRetractionHandle>" << std::endl;
    // } Specific show code ===
    
    os << "[NM_Receive_Interaction - End]" << std::endl;
    return os;
}

NM_Delete_Object::NM_Delete_Object() {
    this->messageName = "NM_Delete_Object";
    this->type = NetworkMessage::DELETE_OBJECT;
    //object= <no default value in message spec using builtin>
    _hasEvent=false;
    //event= <no default value in message spec using builtin>
}

void NM_Delete_Object::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_bool(_hasEvent);
    if (_hasEvent) {
            }
}

void NM_Delete_Object::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    _hasEvent = msgBuffer.read_bool();
    if (_hasEvent) {
            }
}

std::ostream& operator<<(std::ostream& os, const NM_Delete_Object& msg) {
    os << "[NM_Delete_Object - Begin]" << std::endl;
    
    os << static_cast<const NM_Delete_Object::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  object = " << msg.object << std::endl;
    os << "  (opt) event =" << "// TODO field <event> of type <EventRetractionHandle>" << std::endl;
    // } Specific show code ===
    
    os << "[NM_Delete_Object - End]" << std::endl;
    return os;
}

NM_Remove_Object::NM_Remove_Object() {
    this->messageName = "NM_Remove_Object";
    this->type = NetworkMessage::REMOVE_OBJECT;
    //object= <no default value in message spec using builtin>
    //objectClass= <no default value in message spec using builtin>
    _hasEvent=false;
    //event= <no default value in message spec using builtin>
}

void NM_Remove_Object::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_bool(_hasEvent);
    if (_hasEvent) {
            }
}

void NM_Remove_Object::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    _hasEvent = msgBuffer.read_bool();
    if (_hasEvent) {
            }
}

std::ostream& operator<<(std::ostream& os, const NM_Remove_Object& msg) {
    os << "[NM_Remove_Object - Begin]" << std::endl;
    
    os << static_cast<const NM_Remove_Object::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  object = " << msg.object << std::endl;
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  (opt) event =" << "// TODO field <event> of type <EventRetractionHandle>" << std::endl;
    // } Specific show code ===
    
    os << "[NM_Remove_Object - End]" << std::endl;
    return os;
}

NM_Change_Attribute_Transport_Type::NM_Change_Attribute_Transport_Type() {
    this->messageName = "NM_Change_Attribute_Transport_Type";
    this->type = NetworkMessage::CHANGE_ATTRIBUTE_TRANSPORT_TYPE;
    //object= <no default value in message spec using builtin>
    //attributes= <no default value in message spec using builtin>
    //transport= <no default value in message spec using builtin>
}

void NM_Change_Attribute_Transport_Type::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(object);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    msgBuffer.write_uint8(transport);
}

void NM_Change_Attribute_Transport_Type::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    transport = static_cast<TransportType>(msgBuffer.read_uint8());
}

std::ostream& operator<<(std::ostream& os, const NM_Change_Attribute_Transport_Type& msg) {
    os << "[NM_Change_Attribute_Transport_Type - Begin]" << std::endl;
    
    os << static_cast<const NM_Change_Attribute_Transport_Type::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element: msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  transport = " << msg.transport << std::endl;
    // } Specific show code ===
    
    os << "[NM_Change_Attribute_Transport_Type - End]" << std::endl;
    return os;
}

NM_Change_Attribute_Order_Type::NM_Change_Attribute_Order_Type() {
    this->messageName = "NM_Change_Attribute_Order_Type";
    this->type = NetworkMessage::CHANGE_ATTRIBUTE_ORDER_TYPE;
    //object= <no default value in message spec using builtin>
    //attributes= <no default value in message spec using builtin>
    //order= <no default value in message spec using builtin>
}

void NM_Change_Attribute_Order_Type::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(object);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    msgBuffer.write_uint8(order);
}

void NM_Change_Attribute_Order_Type::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    order = static_cast<OrderType>(msgBuffer.read_uint8());
}

std::ostream& operator<<(std::ostream& os, const NM_Change_Attribute_Order_Type& msg) {
    os << "[NM_Change_Attribute_Order_Type - Begin]" << std::endl;
    
    os << static_cast<const NM_Change_Attribute_Order_Type::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element: msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  order = " << msg.order << std::endl;
    // } Specific show code ===
    
    os << "[NM_Change_Attribute_Order_Type - End]" << std::endl;
    return os;
}

NM_Change_Interaction_Transport_Type::NM_Change_Interaction_Transport_Type() {
    this->messageName = "NM_Change_Interaction_Transport_Type";
    this->type = NetworkMessage::CHANGE_INTERACTION_TRANSPORT_TYPE;
    //interactionClass= <no default value in message spec using builtin>
    //transport= <no default value in message spec using builtin>
}

void NM_Change_Interaction_Transport_Type::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    msgBuffer.write_uint8(transport);
}

void NM_Change_Interaction_Transport_Type::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    transport = static_cast<TransportType>(msgBuffer.read_uint8());
}

std::ostream& operator<<(std::ostream& os, const NM_Change_Interaction_Transport_Type& msg) {
    os << "[NM_Change_Interaction_Transport_Type - Begin]" << std::endl;
    
    os << static_cast<const NM_Change_Interaction_Transport_Type::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  transport = " << msg.transport << std::endl;
    // } Specific show code ===
    
    os << "[NM_Change_Interaction_Transport_Type - End]" << std::endl;
    return os;
}

NM_Change_Interaction_Order_Type::NM_Change_Interaction_Order_Type() {
    this->messageName = "NM_Change_Interaction_Order_Type";
    this->type = NetworkMessage::CHANGE_INTERACTION_ORDER_TYPE;
    //interactionClass= <no default value in message spec using builtin>
    //order= <no default value in message spec using builtin>
}

void NM_Change_Interaction_Order_Type::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    msgBuffer.write_uint8(order);
}

void NM_Change_Interaction_Order_Type::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    order = static_cast<OrderType>(msgBuffer.read_uint8());
}

std::ostream& operator<<(std::ostream& os, const NM_Change_Interaction_Order_Type& msg) {
    os << "[NM_Change_Interaction_Order_Type - Begin]" << std::endl;
    
    os << static_cast<const NM_Change_Interaction_Order_Type::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  order = " << msg.order << std::endl;
    // } Specific show code ===
    
    os << "[NM_Change_Interaction_Order_Type - End]" << std::endl;
    return os;
}

NM_Request_Class_Attribute_Value_Update::NM_Request_Class_Attribute_Value_Update() {
    this->messageName = "NM_Request_Class_Attribute_Value_Update";
    this->type = NetworkMessage::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE;
    //objectClass= <no default value in message spec using builtin>
    //attributes= <no default value in message spec using builtin>
}

void NM_Request_Class_Attribute_Value_Update::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(objectClass);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
}

void NM_Request_Class_Attribute_Value_Update::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
}

std::ostream& operator<<(std::ostream& os, const NM_Request_Class_Attribute_Value_Update& msg) {
    os << "[NM_Request_Class_Attribute_Value_Update - Begin]" << std::endl;
    
    os << static_cast<const NM_Request_Class_Attribute_Value_Update::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element: msg.attributes) {
        os << element;
    }
    os << std::endl;
    // } Specific show code ===
    
    os << "[NM_Request_Class_Attribute_Value_Update - End]" << std::endl;
    return os;
}

NM_Request_Object_Attribute_Value_Update::NM_Request_Object_Attribute_Value_Update() {
    this->messageName = "NM_Request_Object_Attribute_Value_Update";
    this->type = NetworkMessage::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE;
    //object= <no default value in message spec using builtin>
    //attributes= <no default value in message spec using builtin>
}

void NM_Request_Object_Attribute_Value_Update::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(object);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
}

void NM_Request_Object_Attribute_Value_Update::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
}

std::ostream& operator<<(std::ostream& os, const NM_Request_Object_Attribute_Value_Update& msg) {
    os << "[NM_Request_Object_Attribute_Value_Update - Begin]" << std::endl;
    
    os << static_cast<const NM_Request_Object_Attribute_Value_Update::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element: msg.attributes) {
        os << element;
    }
    os << std::endl;
    // } Specific show code ===
    
    os << "[NM_Request_Object_Attribute_Value_Update - End]" << std::endl;
    return os;
}

NM_Is_Attribute_Owned_By_Federate::NM_Is_Attribute_Owned_By_Federate() {
    this->messageName = "NM_Is_Attribute_Owned_By_Federate";
    this->type = NetworkMessage::IS_ATTRIBUTE_OWNED_BY_FEDERATE;
    //object= <no default value in message spec using builtin>
    //attribute= <no default value in message spec using builtin>
}

void NM_Is_Attribute_Owned_By_Federate::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_uint32(attribute);
}

void NM_Is_Attribute_Owned_By_Federate::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    attribute = static_cast<AttributeHandle>(msgBuffer.read_uint32());
}

std::ostream& operator<<(std::ostream& os, const NM_Is_Attribute_Owned_By_Federate& msg) {
    os << "[NM_Is_Attribute_Owned_By_Federate - Begin]" << std::endl;
    
    os << static_cast<const NM_Is_Attribute_Owned_By_Federate::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  object = " << msg.object << std::endl;
    os << "  attribute = " << msg.attribute << std::endl;
    // } Specific show code ===
    
    os << "[NM_Is_Attribute_Owned_By_Federate - End]" << std::endl;
    return os;
}

NM_Query_Attribute_Ownership::NM_Query_Attribute_Ownership() {
    this->messageName = "NM_Query_Attribute_Ownership";
    this->type = NetworkMessage::QUERY_ATTRIBUTE_OWNERSHIP;
    //object= <no default value in message spec using builtin>
    //attribute= <no default value in message spec using builtin>
}

void NM_Query_Attribute_Ownership::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_uint32(attribute);
}

void NM_Query_Attribute_Ownership::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    attribute = static_cast<AttributeHandle>(msgBuffer.read_uint32());
}

std::ostream& operator<<(std::ostream& os, const NM_Query_Attribute_Ownership& msg) {
    os << "[NM_Query_Attribute_Ownership - Begin]" << std::endl;
    
    os << static_cast<const NM_Query_Attribute_Ownership::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  object = " << msg.object << std::endl;
    os << "  attribute = " << msg.attribute << std::endl;
    // } Specific show code ===
    
    os << "[NM_Query_Attribute_Ownership - End]" << std::endl;
    return os;
}

NM_Attribute_Is_Not_Owned::NM_Attribute_Is_Not_Owned() {
    this->messageName = "NM_Attribute_Is_Not_Owned";
    this->type = NetworkMessage::ATTRIBUTE_IS_NOT_OWNED;
    //object= <no default value in message spec using builtin>
    //attribute= <no default value in message spec using builtin>
}

void NM_Attribute_Is_Not_Owned::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_uint32(attribute);
}

void NM_Attribute_Is_Not_Owned::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    attribute = static_cast<AttributeHandle>(msgBuffer.read_uint32());
}

std::ostream& operator<<(std::ostream& os, const NM_Attribute_Is_Not_Owned& msg) {
    os << "[NM_Attribute_Is_Not_Owned - Begin]" << std::endl;
    
    os << static_cast<const NM_Attribute_Is_Not_Owned::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  object = " << msg.object << std::endl;
    os << "  attribute = " << msg.attribute << std::endl;
    // } Specific show code ===
    
    os << "[NM_Attribute_Is_Not_Owned - End]" << std::endl;
    return os;
}

NM_Inform_Attribute_Ownership::NM_Inform_Attribute_Ownership() {
    this->messageName = "NM_Inform_Attribute_Ownership";
    this->type = NetworkMessage::INFORM_ATTRIBUTE_OWNERSHIP;
    //object= <no default value in message spec using builtin>
    //attribute= <no default value in message spec using builtin>
}

void NM_Inform_Attribute_Ownership::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_uint32(attribute);
}

void NM_Inform_Attribute_Ownership::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    attribute = static_cast<AttributeHandle>(msgBuffer.read_uint32());
}

std::ostream& operator<<(std::ostream& os, const NM_Inform_Attribute_Ownership& msg) {
    os << "[NM_Inform_Attribute_Ownership - Begin]" << std::endl;
    
    os << static_cast<const NM_Inform_Attribute_Ownership::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  object = " << msg.object << std::endl;
    os << "  attribute = " << msg.attribute << std::endl;
    // } Specific show code ===
    
    os << "[NM_Inform_Attribute_Ownership - End]" << std::endl;
    return os;
}

NM_Attribute_Ownership_Base::NM_Attribute_Ownership_Base() {
    this->messageName = "NM_Attribute_Ownership_Base";
    this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_BASE;
    //object= <no default value in message spec using builtin>
    //attributes= <no default value in message spec using builtin>
}

void NM_Attribute_Ownership_Base::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(object);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
}

void NM_Attribute_Ownership_Base::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
}

std::ostream& operator<<(std::ostream& os, const NM_Attribute_Ownership_Base& msg) {
    os << "[NM_Attribute_Ownership_Base - Begin]" << std::endl;
    
    os << static_cast<const NM_Attribute_Ownership_Base::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element: msg.attributes) {
        os << element;
    }
    os << std::endl;
    // } Specific show code ===
    
    os << "[NM_Attribute_Ownership_Base - End]" << std::endl;
    return os;
}

NM_Negotiated_Attribute_Ownership_Divestiture::NM_Negotiated_Attribute_Ownership_Divestiture() {
    this->messageName = "NM_Negotiated_Attribute_Ownership_Divestiture";
    this->type = NetworkMessage::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
}

NM_Attribute_Ownership_Acquisition_Notification::NM_Attribute_Ownership_Acquisition_Notification() {
    this->messageName = "NM_Attribute_Ownership_Acquisition_Notification";
    this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION;
}

NM_Attribute_Ownership_Divestiture_Notification::NM_Attribute_Ownership_Divestiture_Notification() {
    this->messageName = "NM_Attribute_Ownership_Divestiture_Notification";
    this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION;
}

NM_Request_Attribute_Ownership_Assumption::NM_Request_Attribute_Ownership_Assumption() {
    this->messageName = "NM_Request_Attribute_Ownership_Assumption";
    this->type = NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION;
}

NM_Attribute_Ownership_Unavailable::NM_Attribute_Ownership_Unavailable() {
    this->messageName = "NM_Attribute_Ownership_Unavailable";
    this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_UNAVAILABLE;
}

NM_Attribute_Ownership_Acquisition_If_Available::NM_Attribute_Ownership_Acquisition_If_Available() {
    this->messageName = "NM_Attribute_Ownership_Acquisition_If_Available";
    this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE;
}

NM_Unconditional_Attribute_Ownership_Divestiture::NM_Unconditional_Attribute_Ownership_Divestiture() {
    this->messageName = "NM_Unconditional_Attribute_Ownership_Divestiture";
    this->type = NetworkMessage::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
}

NM_Attribute_Ownership_Acquisition::NM_Attribute_Ownership_Acquisition() {
    this->messageName = "NM_Attribute_Ownership_Acquisition";
    this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION;
}

NM_Request_Attribute_Ownership_Release::NM_Request_Attribute_Ownership_Release() {
    this->messageName = "NM_Request_Attribute_Ownership_Release";
    this->type = NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE;
}

NM_Cancel_Negotiated_Attribute_Ownership_Divestiture::NM_Cancel_Negotiated_Attribute_Ownership_Divestiture() {
    this->messageName = "NM_Cancel_Negotiated_Attribute_Ownership_Divestiture";
    this->type = NetworkMessage::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
}

NM_Attribute_Ownership_Release_Response::NM_Attribute_Ownership_Release_Response() {
    this->messageName = "NM_Attribute_Ownership_Release_Response";
    this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE;
}

NM_Cancel_Attribute_Ownership_Acquisition::NM_Cancel_Attribute_Ownership_Acquisition() {
    this->messageName = "NM_Cancel_Attribute_Ownership_Acquisition";
    this->type = NetworkMessage::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION;
}

NM_Confirm_Attribute_Ownership_Acquisition_Cancellation::NM_Confirm_Attribute_Ownership_Acquisition_Cancellation() {
    this->messageName = "NM_Confirm_Attribute_Ownership_Acquisition_Cancellation";
    this->type = NetworkMessage::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION;
}

NM_DDM_Create_Region::NM_DDM_Create_Region() {
    this->messageName = "NM_DDM_Create_Region";
    this->type = NetworkMessage::DDM_CREATE_REGION;
    //attributes= <no default value in message spec using builtin>
    DDM_bool=false;
    //space= <no default value in message spec using builtin>
    //region= <no default value in message spec using builtin>
    //nbExtents= <no default value in message spec using builtin>
}

void NM_DDM_Create_Region::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
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

void NM_DDM_Create_Region::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
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

std::ostream& operator<<(std::ostream& os, const NM_DDM_Create_Region& msg) {
    os << "[NM_DDM_Create_Region - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Create_Region::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  attributes [] =" << std::endl;
    for (const auto& element: msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  DDM_bool = " << msg.DDM_bool << std::endl;
    os << "  space = " << msg.space << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  nbExtents = " << msg.nbExtents << std::endl;
    // } Specific show code ===
    
    os << "[NM_DDM_Create_Region - End]" << std::endl;
    return os;
}

NM_DDM_Modify_Region::NM_DDM_Modify_Region() {
    this->messageName = "NM_DDM_Modify_Region";
    this->type = NetworkMessage::DDM_MODIFY_REGION;
    //attributes= <no default value in message spec using builtin>
    DDM_bool=false;
    //region= <no default value in message spec using builtin>
}

void NM_DDM_Modify_Region::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    msgBuffer.write_bool(DDM_bool);
    msgBuffer.write_uint32(region);
}

void NM_DDM_Modify_Region::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    DDM_bool = msgBuffer.read_bool();
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
}

std::ostream& operator<<(std::ostream& os, const NM_DDM_Modify_Region& msg) {
    os << "[NM_DDM_Modify_Region - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Modify_Region::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  attributes [] =" << std::endl;
    for (const auto& element: msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  DDM_bool = " << msg.DDM_bool << std::endl;
    os << "  region = " << msg.region << std::endl;
    // } Specific show code ===
    
    os << "[NM_DDM_Modify_Region - End]" << std::endl;
    return os;
}

NM_DDM_Delete_Region::NM_DDM_Delete_Region() {
    this->messageName = "NM_DDM_Delete_Region";
    this->type = NetworkMessage::DDM_DELETE_REGION;
    //region= <no default value in message spec using builtin>
}

void NM_DDM_Delete_Region::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(region);
}

void NM_DDM_Delete_Region::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
}

std::ostream& operator<<(std::ostream& os, const NM_DDM_Delete_Region& msg) {
    os << "[NM_DDM_Delete_Region - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Delete_Region::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  region = " << msg.region << std::endl;
    // } Specific show code ===
    
    os << "[NM_DDM_Delete_Region - End]" << std::endl;
    return os;
}

NM_DDM_Associate_Region::NM_DDM_Associate_Region() {
    this->messageName = "NM_DDM_Associate_Region";
    this->type = NetworkMessage::DDM_ASSOCIATE_REGION;
    //attributes= <no default value in message spec using builtin>
    DDM_bool=false;
    //region= <no default value in message spec using builtin>
    //object= <no default value in message spec using builtin>
}

void NM_DDM_Associate_Region::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    msgBuffer.write_bool(DDM_bool);
    msgBuffer.write_uint32(region);
    msgBuffer.write_uint32(object);
}

void NM_DDM_Associate_Region::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    DDM_bool = msgBuffer.read_bool();
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
}

std::ostream& operator<<(std::ostream& os, const NM_DDM_Associate_Region& msg) {
    os << "[NM_DDM_Associate_Region - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Associate_Region::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  attributes [] =" << std::endl;
    for (const auto& element: msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  DDM_bool = " << msg.DDM_bool << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  object = " << msg.object << std::endl;
    // } Specific show code ===
    
    os << "[NM_DDM_Associate_Region - End]" << std::endl;
    return os;
}

NM_DDM_Unassociate_Region::NM_DDM_Unassociate_Region() {
    this->messageName = "NM_DDM_Unassociate_Region";
    this->type = NetworkMessage::DDM_UNASSOCIATE_REGION;
    //attributes= <no default value in message spec using builtin>
    //region= <no default value in message spec using builtin>
    //object= <no default value in message spec using builtin>
    DDM_bool=false;
}

void NM_DDM_Unassociate_Region::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    msgBuffer.write_uint32(region);
    msgBuffer.write_uint32(object);
    msgBuffer.write_bool(DDM_bool);
}

void NM_DDM_Unassociate_Region::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    DDM_bool = msgBuffer.read_bool();
}

std::ostream& operator<<(std::ostream& os, const NM_DDM_Unassociate_Region& msg) {
    os << "[NM_DDM_Unassociate_Region - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Unassociate_Region::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  attributes [] =" << std::endl;
    for (const auto& element: msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  object = " << msg.object << std::endl;
    os << "  DDM_bool = " << msg.DDM_bool << std::endl;
    // } Specific show code ===
    
    os << "[NM_DDM_Unassociate_Region - End]" << std::endl;
    return os;
}

NM_DDM_Register_Object::NM_DDM_Register_Object() {
    this->messageName = "NM_DDM_Register_Object";
    this->type = NetworkMessage::DDM_REGISTER_OBJECT;
    //objectClass= <no default value in message spec using builtin>
    //attributes= <no default value in message spec using builtin>
    //region= <no default value in message spec using builtin>
    _hasObjectInstanceName=false;
    //objectInstanceName= <no default value in message spec using builtin>
    //object= <no default value in message spec using builtin>
    DDM_bool=false;
}

void NM_DDM_Register_Object::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
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

void NM_DDM_Register_Object::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
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

std::ostream& operator<<(std::ostream& os, const NM_DDM_Register_Object& msg) {
    os << "[NM_DDM_Register_Object - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Register_Object::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element: msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  (opt) objectInstanceName =" << msg.objectInstanceName << std::endl;
    os << "  object = " << msg.object << std::endl;
    os << "  DDM_bool = " << msg.DDM_bool << std::endl;
    // } Specific show code ===
    
    os << "[NM_DDM_Register_Object - End]" << std::endl;
    return os;
}

NM_DDM_Subscribe_Attributes::NM_DDM_Subscribe_Attributes() {
    this->messageName = "NM_DDM_Subscribe_Attributes";
    this->type = NetworkMessage::DDM_SUBSCRIBE_ATTRIBUTES;
    //objectClass= <no default value in message spec using builtin>
    //region= <no default value in message spec using builtin>
    //attributes= <no default value in message spec using builtin>
    DDM_bool=false;
}

void NM_DDM_Subscribe_Attributes::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_uint32(region);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    msgBuffer.write_bool(DDM_bool);
}

void NM_DDM_Subscribe_Attributes::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    DDM_bool = msgBuffer.read_bool();
}

std::ostream& operator<<(std::ostream& os, const NM_DDM_Subscribe_Attributes& msg) {
    os << "[NM_DDM_Subscribe_Attributes - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Subscribe_Attributes::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element: msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  DDM_bool = " << msg.DDM_bool << std::endl;
    // } Specific show code ===
    
    os << "[NM_DDM_Subscribe_Attributes - End]" << std::endl;
    return os;
}

NM_DDM_Unsubscribe_Attributes::NM_DDM_Unsubscribe_Attributes() {
    this->messageName = "NM_DDM_Unsubscribe_Attributes";
    this->type = NetworkMessage::DDM_UNSUBSCRIBE_ATTRIBUTES;
    //objectClass= <no default value in message spec using builtin>
    //region= <no default value in message spec using builtin>
    //attributes= <no default value in message spec using builtin>
    DDM_bool=false;
}

void NM_DDM_Unsubscribe_Attributes::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_uint32(region);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
    msgBuffer.write_bool(DDM_bool);
}

void NM_DDM_Unsubscribe_Attributes::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
    DDM_bool = msgBuffer.read_bool();
}

std::ostream& operator<<(std::ostream& os, const NM_DDM_Unsubscribe_Attributes& msg) {
    os << "[NM_DDM_Unsubscribe_Attributes - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Unsubscribe_Attributes::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element: msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  DDM_bool = " << msg.DDM_bool << std::endl;
    // } Specific show code ===
    
    os << "[NM_DDM_Unsubscribe_Attributes - End]" << std::endl;
    return os;
}

NM_DDM_Subscribe_Interaction::NM_DDM_Subscribe_Interaction() {
    this->messageName = "NM_DDM_Subscribe_Interaction";
    this->type = NetworkMessage::DDM_SUBSCRIBE_INTERACTION;
    //interactionClass= <no default value in message spec using builtin>
    //region= <no default value in message spec using builtin>
    DDM_bool=false;
}

void NM_DDM_Subscribe_Interaction::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    msgBuffer.write_uint32(region);
    msgBuffer.write_bool(DDM_bool);
}

void NM_DDM_Subscribe_Interaction::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
    DDM_bool = msgBuffer.read_bool();
}

std::ostream& operator<<(std::ostream& os, const NM_DDM_Subscribe_Interaction& msg) {
    os << "[NM_DDM_Subscribe_Interaction - Begin]" << std::endl;
    
    os << static_cast<const NM_DDM_Subscribe_Interaction::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  DDM_bool = " << msg.DDM_bool << std::endl;
    // } Specific show code ===
    
    os << "[NM_DDM_Subscribe_Interaction - End]" << std::endl;
    return os;
}

NM_DDM_Unsubscribe_Interaction::NM_DDM_Unsubscribe_Interaction() {
    this->messageName = "NM_DDM_Unsubscribe_Interaction";
    this->type = NetworkMessage::DDM_UNSUBSCRIBE_INTERACTION;
}

NM_Provide_Attribute_Value_Update::NM_Provide_Attribute_Value_Update() {
    this->messageName = "NM_Provide_Attribute_Value_Update";
    this->type = NetworkMessage::PROVIDE_ATTRIBUTE_VALUE_UPDATE;
}

NM_Start_Registration_For_Object_Class::NM_Start_Registration_For_Object_Class() {
    this->messageName = "NM_Start_Registration_For_Object_Class";
    this->type = NetworkMessage::START_REGISTRATION_FOR_OBJECT_CLASS;
    //objectClass= <no default value in message spec using builtin>
}

void NM_Start_Registration_For_Object_Class::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(objectClass);
}

void NM_Start_Registration_For_Object_Class::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
}

std::ostream& operator<<(std::ostream& os, const NM_Start_Registration_For_Object_Class& msg) {
    os << "[NM_Start_Registration_For_Object_Class - Begin]" << std::endl;
    
    os << static_cast<const NM_Start_Registration_For_Object_Class::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  objectClass = " << msg.objectClass << std::endl;
    // } Specific show code ===
    
    os << "[NM_Start_Registration_For_Object_Class - End]" << std::endl;
    return os;
}

NM_Stop_Registration_For_Object_Class::NM_Stop_Registration_For_Object_Class() {
    this->messageName = "NM_Stop_Registration_For_Object_Class";
    this->type = NetworkMessage::STOP_REGISTRATION_FOR_OBJECT_CLASS;
    //objectClass= <no default value in message spec using builtin>
}

void NM_Stop_Registration_For_Object_Class::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_uint32(objectClass);
}

void NM_Stop_Registration_For_Object_Class::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
}

std::ostream& operator<<(std::ostream& os, const NM_Stop_Registration_For_Object_Class& msg) {
    os << "[NM_Stop_Registration_For_Object_Class - Begin]" << std::endl;
    
    os << static_cast<const NM_Stop_Registration_For_Object_Class::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  objectClass = " << msg.objectClass << std::endl;
    // } Specific show code ===
    
    os << "[NM_Stop_Registration_For_Object_Class - End]" << std::endl;
    return os;
}

NM_Reserve_Object_Instance_Name::NM_Reserve_Object_Instance_Name() {
    this->messageName = "NM_Reserve_Object_Instance_Name";
    this->type = NetworkMessage::RESERVE_OBJECT_INSTANCE_NAME;
    //objectName= <no default value in message spec using builtin>
}

void NM_Reserve_Object_Instance_Name::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    msgBuffer.write_string(objectName);
}

void NM_Reserve_Object_Instance_Name::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    msgBuffer.read_string(objectName);
}

std::ostream& operator<<(std::ostream& os, const NM_Reserve_Object_Instance_Name& msg) {
    os << "[NM_Reserve_Object_Instance_Name - Begin]" << std::endl;
    
    os << static_cast<const NM_Reserve_Object_Instance_Name::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  objectName = " << msg.objectName << std::endl;
    // } Specific show code ===
    
    os << "[NM_Reserve_Object_Instance_Name - End]" << std::endl;
    return os;
}

NM_Reserve_Object_Instance_Name_Succeeded::NM_Reserve_Object_Instance_Name_Succeeded() {
    this->messageName = "NM_Reserve_Object_Instance_Name_Succeeded";
    this->type = NetworkMessage::RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED;
}

NM_Reserve_Object_Instance_Name_Failed::NM_Reserve_Object_Instance_Name_Failed() {
    this->messageName = "NM_Reserve_Object_Instance_Name_Failed";
    this->type = NetworkMessage::RESERVE_OBJECT_INSTANCE_NAME_FAILED;
}

NM_Message_Null_Prime::NM_Message_Null_Prime() {
    this->messageName = "NM_Message_Null_Prime";
    this->type = NetworkMessage::MESSAGE_NULL_PRIME;
    //timestamp= <no default value in message spec using builtin>
}

void NM_Message_Null_Prime::serialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::serialize(msgBuffer);
    //Specific serialization code
    // FIXME FIXME FIXME
    //don't know how to serialize native field <timestamp> of type <FederationTime>
    //probably no 'representation' given
}

void NM_Message_Null_Prime::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Call parent class
    Super::deserialize(msgBuffer);
    //Specific deserialization code
    // FIXME FIXME FIXME
    //don't know how to deserialize native field <timestamp> of type <FederationTime>
    //probably no 'representation' given
}

std::ostream& operator<<(std::ostream& os, const NM_Message_Null_Prime& msg) {
    os << "[NM_Message_Null_Prime - Begin]" << std::endl;
    
    os << static_cast<const NM_Message_Null_Prime::Super&>(msg); // show parent class
    
    // === Specific show code {
    os << "  timestamp = " << "// TODO field <timestamp> of type <FederationTime>" << std::endl;
    // } Specific show code ===
    
    os << "[NM_Message_Null_Prime - End]" << std::endl;
    return os;
}

New_NetworkMessage::New_NetworkMessage() {
    type=0;
    _hasDate=false;
    //date= <no default value in message spec using builtin>
    _hasLabel=false;
    //label= <no default value in message spec using builtin>
    _hasTag=false;
    //tag= <no default value in message spec using builtin>
}

void New_NetworkMessage::serialize(libhla::MessageBuffer& msgBuffer) {
    //Specific serialization code
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

void New_NetworkMessage::deserialize(libhla::MessageBuffer& msgBuffer) {
    //Specific deserialization code
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

std::ostream& operator<<(std::ostream& os, const New_NetworkMessage& msg) {
    os << "[New_NetworkMessage - Begin]" << std::endl;
    
    // === Specific show code {
    os << "  type = " << msg.type << std::endl;
    os << "  (opt) date =" << "// TODO field <date> of type <FederationTime>" << std::endl;
    os << "  (opt) label =" << msg.label << std::endl;
    os << "  (opt) tag =" << msg.tag << std::endl;
    // } Specific show code ===
    
    os << "[New_NetworkMessage - End]" << std::endl;
    return os;
}

NetworkMessage* NM_Factory::create(NM_Type type) throw (NetworkError ,NetworkSignal) { 
    NetworkMessage* msg = NULL;

    switch (type) {
        case NetworkMessage::NOT_USED:
            throw NetworkError("NOT_USED message type should not be used!!");
            break;
        case NetworkMessage::CLOSE_CONNEXION:
            msg = new NM_Close_Connexion();
            break;
        case NetworkMessage::MESSAGE_NULL:
            msg = new NM_Message_Null();
            break;
        case NetworkMessage::CREATE_FEDERATION_EXECUTION:
            msg = new NM_Create_Federation_Execution();
            break;
        case NetworkMessage::DESTROY_FEDERATION_EXECUTION:
            msg = new NM_Destroy_Federation_Execution();
            break;
        case NetworkMessage::JOIN_FEDERATION_EXECUTION:
            msg = new NM_Join_Federation_Execution();
            break;
        case NetworkMessage::RESIGN_FEDERATION_EXECUTION:
            msg = new NM_Resign_Federation_Execution();
            break;
        case NetworkMessage::SET_TIME_REGULATING:
            msg = new NM_Set_Time_Regulating();
            break;
        case NetworkMessage::SET_TIME_CONSTRAINED:
            msg = new NM_Set_Time_Constrained();
            break;
        case NetworkMessage::TIME_REGULATION_ENABLED:
            msg = new NM_Time_Regulation_Enabled();
            break;
        case NetworkMessage::TIME_CONSTRAINED_ENABLED:
            msg = new NM_Time_Constrained_Enabled();
            break;
        case NetworkMessage::SET_CLASS_RELEVANCE_ADVISORY_SWITCH:
            msg = new NM_Set_Class_Relevance_Advisory_Switch();
            break;
        case NetworkMessage::SET_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
            msg = new NM_Set_Interaction_Relevance_Advisory_Switch();
            break;
        case NetworkMessage::SET_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH:
            msg = new NM_Set_Attribute_Relevance_Advisory_Switch();
            break;
        case NetworkMessage::SET_ATTRIBUTE_SCOPE_ADVISORY_SWITCH:
            msg = new NM_Set_Attribute_Scope_Advisory_Switch();
            break;
        case NetworkMessage::REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
            msg = new NM_Register_Federation_Synchronization_Point();
            break;
        case NetworkMessage::CONFIRM_SYNCHRONIZATION_POINT_REGISTRATION:
            msg = new NM_Confirm_Synchronization_Point_Registration();
            break;
        case NetworkMessage::ANNOUNCE_SYNCHRONIZATION_POINT:
            msg = new NM_Announce_Synchronization_Point();
            break;
        case NetworkMessage::SYNCHRONIZATION_POINT_ACHIEVED:
            msg = new NM_Synchronization_Point_Achieved();
            break;
        case NetworkMessage::FEDERATION_SYNCHRONIZED:
            msg = new NM_Federation_Synchronized();
            break;
        case NetworkMessage::REQUEST_FEDERATION_SAVE:
            msg = new NM_Request_Federation_Save();
            break;
        case NetworkMessage::FEDERATE_SAVE_BEGUN:
            msg = new NM_Federate_Save_Begun();
            break;
        case NetworkMessage::FEDERATE_SAVE_COMPLETE:
            msg = new NM_Federate_Save_Complete();
            break;
        case NetworkMessage::FEDERATE_SAVE_NOT_COMPLETE:
            msg = new NM_Federate_Save_Not_Complete();
            break;
        case NetworkMessage::INITIATE_FEDERATE_SAVE:
            msg = new NM_Initiate_Federate_Save();
            break;
        case NetworkMessage::FEDERATION_SAVED:
            msg = new NM_Federation_Saved();
            break;
        case NetworkMessage::FEDERATION_NOT_SAVED:
            msg = new NM_Federation_Not_Saved();
            break;
        case NetworkMessage::REQUEST_FEDERATION_RESTORE:
            msg = new NM_Request_Federation_Restore();
            break;
        case NetworkMessage::FEDERATE_RESTORE_COMPLETE:
            msg = new NM_Federate_Restore_Complete();
            break;
        case NetworkMessage::FEDERATE_RESTORE_NOT_COMPLETE:
            msg = new NM_Federate_Restore_Not_Complete();
            break;
        case NetworkMessage::REQUEST_FEDERATION_RESTORE_SUCCEEDED:
            msg = new NM_Request_Federation_Restore_Succeeded();
            break;
        case NetworkMessage::REQUEST_FEDERATION_RESTORE_FAILED:
            msg = new NM_Request_Federation_Restore_Failed();
            break;
        case NetworkMessage::FEDERATION_RESTORE_BEGUN:
            msg = new NM_Federation_Restore_Begun();
            break;
        case NetworkMessage::INITIATE_FEDERATE_RESTORE:
            msg = new NM_Initiate_Federate_Restore();
            break;
        case NetworkMessage::FEDERATION_RESTORED:
            msg = new NM_Federation_Restored();
            break;
        case NetworkMessage::FEDERATION_NOT_RESTORED:
            msg = new NM_Federation_Not_Restored();
            break;
        case NetworkMessage::PUBLISH_OBJECT_CLASS:
            msg = new NM_Publish_Object_Class();
            break;
        case NetworkMessage::UNPUBLISH_OBJECT_CLASS:
            msg = new NM_Unpublish_Object_Class();
            break;
        case NetworkMessage::PUBLISH_INTERACTION_CLASS:
            msg = new NM_Publish_Interaction_Class();
            break;
        case NetworkMessage::UNPUBLISH_INTERACTION_CLASS:
            msg = new NM_Unpublish_Interaction_Class();
            break;
        case NetworkMessage::SUBSCRIBE_OBJECT_CLASS:
            msg = new NM_Subscribe_Object_Class();
            break;
        case NetworkMessage::UNSUBSCRIBE_OBJECT_CLASS:
            msg = new NM_Unsubscribe_Object_Class();
            break;
        case NetworkMessage::SUBSCRIBE_INTERACTION_CLASS:
            msg = new NM_Subscribe_Interaction_Class();
            break;
        case NetworkMessage::UNSUBSCRIBE_INTERACTION_CLASS:
            msg = new NM_Unsubscribe_Interaction_Class();
            break;
        case NetworkMessage::TURN_INTERACTIONS_ON:
            msg = new NM_Turn_Interactions_On();
            break;
        case NetworkMessage::TURN_INTERACTIONS_OFF:
            msg = new NM_Turn_Interactions_Off();
            break;
        case NetworkMessage::REGISTER_OBJECT:
            msg = new NM_Register_Object();
            break;
        case NetworkMessage::DISCOVER_OBJECT:
            msg = new NM_Discover_Object();
            break;
        case NetworkMessage::UPDATE_ATTRIBUTE_VALUES:
            msg = new NM_Update_Attribute_Values();
            break;
        case NetworkMessage::REFLECT_ATTRIBUTE_VALUES:
            msg = new NM_Reflect_Attribute_Values();
            break;
        case NetworkMessage::SEND_INTERACTION:
            msg = new NM_Send_Interaction();
            break;
        case NetworkMessage::RECEIVE_INTERACTION:
            msg = new NM_Receive_Interaction();
            break;
        case NetworkMessage::DELETE_OBJECT:
            msg = new NM_Delete_Object();
            break;
        case NetworkMessage::REMOVE_OBJECT:
            msg = new NM_Remove_Object();
            break;
        case NetworkMessage::CHANGE_ATTRIBUTE_TRANSPORT_TYPE:
            msg = new NM_Change_Attribute_Transport_Type();
            break;
        case NetworkMessage::CHANGE_ATTRIBUTE_ORDER_TYPE:
            msg = new NM_Change_Attribute_Order_Type();
            break;
        case NetworkMessage::CHANGE_INTERACTION_TRANSPORT_TYPE:
            msg = new NM_Change_Interaction_Transport_Type();
            break;
        case NetworkMessage::CHANGE_INTERACTION_ORDER_TYPE:
            msg = new NM_Change_Interaction_Order_Type();
            break;
        case NetworkMessage::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE:
            msg = new NM_Request_Class_Attribute_Value_Update();
            break;
        case NetworkMessage::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
            msg = new NM_Request_Object_Attribute_Value_Update();
            break;
        case NetworkMessage::IS_ATTRIBUTE_OWNED_BY_FEDERATE:
            msg = new NM_Is_Attribute_Owned_By_Federate();
            break;
        case NetworkMessage::QUERY_ATTRIBUTE_OWNERSHIP:
            msg = new NM_Query_Attribute_Ownership();
            break;
        case NetworkMessage::ATTRIBUTE_IS_NOT_OWNED:
            msg = new NM_Attribute_Is_Not_Owned();
            break;
        case NetworkMessage::INFORM_ATTRIBUTE_OWNERSHIP:
            msg = new NM_Inform_Attribute_Ownership();
            break;
        case NetworkMessage::ATTRIBUTE_OWNERSHIP_BASE:
            msg = new NM_Attribute_Ownership_Base();
            break;
        case NetworkMessage::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
            msg = new NM_Negotiated_Attribute_Ownership_Divestiture();
            break;
        case NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
            msg = new NM_Attribute_Ownership_Acquisition_Notification();
            break;
        case NetworkMessage::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
            msg = new NM_Attribute_Ownership_Divestiture_Notification();
            break;
        case NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
            msg = new NM_Request_Attribute_Ownership_Assumption();
            break;
        case NetworkMessage::ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
            msg = new NM_Attribute_Ownership_Unavailable();
            break;
        case NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
            msg = new NM_Attribute_Ownership_Acquisition_If_Available();
            break;
        case NetworkMessage::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
            msg = new NM_Unconditional_Attribute_Ownership_Divestiture();
            break;
        case NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION:
            msg = new NM_Attribute_Ownership_Acquisition();
            break;
        case NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
            msg = new NM_Request_Attribute_Ownership_Release();
            break;
        case NetworkMessage::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
            msg = new NM_Cancel_Negotiated_Attribute_Ownership_Divestiture();
            break;
        case NetworkMessage::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
            msg = new NM_Attribute_Ownership_Release_Response();
            break;
        case NetworkMessage::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
            msg = new NM_Cancel_Attribute_Ownership_Acquisition();
            break;
        case NetworkMessage::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
            msg = new NM_Confirm_Attribute_Ownership_Acquisition_Cancellation();
            break;
        case NetworkMessage::DDM_CREATE_REGION:
            msg = new NM_DDM_Create_Region();
            break;
        case NetworkMessage::DDM_MODIFY_REGION:
            msg = new NM_DDM_Modify_Region();
            break;
        case NetworkMessage::DDM_DELETE_REGION:
            msg = new NM_DDM_Delete_Region();
            break;
        case NetworkMessage::DDM_ASSOCIATE_REGION:
            msg = new NM_DDM_Associate_Region();
            break;
        case NetworkMessage::DDM_UNASSOCIATE_REGION:
            msg = new NM_DDM_Unassociate_Region();
            break;
        case NetworkMessage::DDM_REGISTER_OBJECT:
            msg = new NM_DDM_Register_Object();
            break;
        case NetworkMessage::DDM_SUBSCRIBE_ATTRIBUTES:
            msg = new NM_DDM_Subscribe_Attributes();
            break;
        case NetworkMessage::DDM_UNSUBSCRIBE_ATTRIBUTES:
            msg = new NM_DDM_Unsubscribe_Attributes();
            break;
        case NetworkMessage::DDM_SUBSCRIBE_INTERACTION:
            msg = new NM_DDM_Subscribe_Interaction();
            break;
        case NetworkMessage::DDM_UNSUBSCRIBE_INTERACTION:
            msg = new NM_DDM_Unsubscribe_Interaction();
            break;
        case NetworkMessage::PROVIDE_ATTRIBUTE_VALUE_UPDATE:
            msg = new NM_Provide_Attribute_Value_Update();
            break;
        case NetworkMessage::START_REGISTRATION_FOR_OBJECT_CLASS:
            msg = new NM_Start_Registration_For_Object_Class();
            break;
        case NetworkMessage::STOP_REGISTRATION_FOR_OBJECT_CLASS:
            msg = new NM_Stop_Registration_For_Object_Class();
            break;
        case NetworkMessage::RESERVE_OBJECT_INSTANCE_NAME:
            msg = new NM_Reserve_Object_Instance_Name();
            break;
        case NetworkMessage::RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED:
            msg = new NM_Reserve_Object_Instance_Name_Succeeded();
            break;
        case NetworkMessage::RESERVE_OBJECT_INSTANCE_NAME_FAILED:
            msg = new NM_Reserve_Object_Instance_Name_Failed();
            break;
        case NetworkMessage::MESSAGE_NULL_PRIME:
            msg = new NM_Message_Null_Prime();
            break;
        case NetworkMessage::LAST:
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
