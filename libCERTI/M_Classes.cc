// Generated on 2018 April Fri, 13 at 11:41:23 by the CERTI message generator
#include <string>
#include <vector>
#include "M_Classes.hh"
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
// CERTI Messages description
// ----------------------------------------------------------------------------
// The messages related classes will be placed in the certi package
namespace certi {

void EventRetraction::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Specific serialization code
    msgBuffer.write_uint32(sendingFederate);
    msgBuffer.write_uint64(SN);
}

void EventRetraction::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Specific deserialization code
    sendingFederate = static_cast<FederateHandle>(msgBuffer.read_uint32());
    SN = msgBuffer.read_uint64();
}

const FederateHandle& EventRetraction::getSendingFederate() const
{
    return sendingFederate;
}

void EventRetraction::setSendingFederate(const FederateHandle& newSendingFederate)
{
    sendingFederate = newSendingFederate;
}

const uint64_t& EventRetraction::getSN() const
{
    return SN;
}

void EventRetraction::setSN(const uint64_t& newSN)
{
    SN = newSN;
}

std::ostream& operator<<(std::ostream& os, const EventRetraction& msg)
{
    os << "[EventRetraction - Begin]" << std::endl;
    
    // Specific display
    os << "  sendingFederate = " << msg.sendingFederate << std::endl;
    os << "  SN = " << msg.SN << std::endl;
    
    os << "[EventRetraction - End]" << std::endl;
    return os;
}

M_Open_Connexion::M_Open_Connexion()
{
    this->messageName = "M_Open_Connexion";
    this->type = Message::OPEN_CONNEXION;
}

void M_Open_Connexion::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(versionMajor);
    msgBuffer.write_uint32(versionMinor);
}

void M_Open_Connexion::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    versionMajor = msgBuffer.read_uint32();
    versionMinor = msgBuffer.read_uint32();
}

const uint32_t& M_Open_Connexion::getVersionMajor() const
{
    return versionMajor;
}

void M_Open_Connexion::setVersionMajor(const uint32_t& newVersionMajor)
{
    versionMajor = newVersionMajor;
}

const uint32_t& M_Open_Connexion::getVersionMinor() const
{
    return versionMinor;
}

void M_Open_Connexion::setVersionMinor(const uint32_t& newVersionMinor)
{
    versionMinor = newVersionMinor;
}

std::ostream& operator<<(std::ostream& os, const M_Open_Connexion& msg)
{
    os << "[M_Open_Connexion - Begin]" << std::endl;
    
    os << static_cast<const M_Open_Connexion::Super&>(msg); // show parent class
    
    // Specific display
    os << "  versionMajor = " << msg.versionMajor << std::endl;
    os << "  versionMinor = " << msg.versionMinor << std::endl;
    
    os << "[M_Open_Connexion - End]" << std::endl;
    return os;
}

M_Close_Connexion::M_Close_Connexion()
{
    this->messageName = "M_Close_Connexion";
    this->type = Message::CLOSE_CONNEXION;
}

M_Create_Federation_Execution::M_Create_Federation_Execution()
{
    this->messageName = "M_Create_Federation_Execution";
    this->type = Message::CREATE_FEDERATION_EXECUTION;
}

void M_Create_Federation_Execution::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_string(federationName);
    msgBuffer.write_string(FEDid);
}

void M_Create_Federation_Execution::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    msgBuffer.read_string(federationName);
    msgBuffer.read_string(FEDid);
}

const std::string& M_Create_Federation_Execution::getFederationName() const
{
    return federationName;
}

void M_Create_Federation_Execution::setFederationName(const std::string& newFederationName)
{
    federationName = newFederationName;
}

const std::string& M_Create_Federation_Execution::getFEDid() const
{
    return FEDid;
}

void M_Create_Federation_Execution::setFEDid(const std::string& newFEDid)
{
    FEDid = newFEDid;
}

std::ostream& operator<<(std::ostream& os, const M_Create_Federation_Execution& msg)
{
    os << "[M_Create_Federation_Execution - Begin]" << std::endl;
    
    os << static_cast<const M_Create_Federation_Execution::Super&>(msg); // show parent class
    
    // Specific display
    os << "  federationName = " << msg.federationName << std::endl;
    os << "  FEDid = " << msg.FEDid << std::endl;
    
    os << "[M_Create_Federation_Execution - End]" << std::endl;
    return os;
}

M_Create_Federation_Execution_V4::M_Create_Federation_Execution_V4()
{
    this->messageName = "M_Create_Federation_Execution_V4";
    this->type = Message::CREATE_FEDERATION_EXECUTION_V4;
}

void M_Create_Federation_Execution_V4::serialize(libhla::MessageBuffer& msgBuffer)
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
    msgBuffer.write_bool(_hasLogicalTimeRepresentation);
    if (_hasLogicalTimeRepresentation) {
        msgBuffer.write_string(logicalTimeRepresentation);
    }
}

void M_Create_Federation_Execution_V4::deserialize(libhla::MessageBuffer& msgBuffer)
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
    _hasLogicalTimeRepresentation = msgBuffer.read_bool();
    if (_hasLogicalTimeRepresentation) {
        msgBuffer.read_string(logicalTimeRepresentation);
    }
}

const std::string& M_Create_Federation_Execution_V4::getFederationExecutionName() const
{
    return federationExecutionName;
}

void M_Create_Federation_Execution_V4::setFederationExecutionName(const std::string& newFederationExecutionName)
{
    federationExecutionName = newFederationExecutionName;
}

const RtiVersion& M_Create_Federation_Execution_V4::getRtiVersion() const
{
    return rtiVersion;
}

void M_Create_Federation_Execution_V4::setRtiVersion(const RtiVersion& newRtiVersion)
{
    rtiVersion = newRtiVersion;
}

uint32_t M_Create_Federation_Execution_V4::getFomModuleDesignatorsSize() const
{
    return fomModuleDesignators.size();
}

void M_Create_Federation_Execution_V4::setFomModuleDesignatorsSize(uint32_t num)
{
    fomModuleDesignators.resize(num);
}

const std::vector<std::string>& M_Create_Federation_Execution_V4::getFomModuleDesignators() const
{
    return fomModuleDesignators;
}

const std::string& M_Create_Federation_Execution_V4::getFomModuleDesignators(uint32_t rank) const
{
    return fomModuleDesignators[rank];
}

std::string& M_Create_Federation_Execution_V4::getFomModuleDesignators(uint32_t rank)
{
    return fomModuleDesignators[rank];
}

void M_Create_Federation_Execution_V4::setFomModuleDesignators(const std::string& newFomModuleDesignators, uint32_t rank)
{
    fomModuleDesignators[rank] = newFomModuleDesignators;
}

void M_Create_Federation_Execution_V4::removeFomModuleDesignators(uint32_t rank)
{
    fomModuleDesignators.erase(fomModuleDesignators.begin() + rank);
}

const std::string& M_Create_Federation_Execution_V4::getMimDesignator() const
{
    return mimDesignator;
}

void M_Create_Federation_Execution_V4::setMimDesignator(const std::string& newMimDesignator)
{
    _hasMimDesignator = true;
    mimDesignator = newMimDesignator;
}

bool M_Create_Federation_Execution_V4::hasMimDesignator() const
{
    return _hasMimDesignator;
}

const std::string& M_Create_Federation_Execution_V4::getLogicalTimeRepresentation() const
{
    return logicalTimeRepresentation;
}

void M_Create_Federation_Execution_V4::setLogicalTimeRepresentation(const std::string& newLogicalTimeRepresentation)
{
    _hasLogicalTimeRepresentation = true;
    logicalTimeRepresentation = newLogicalTimeRepresentation;
}

bool M_Create_Federation_Execution_V4::hasLogicalTimeRepresentation() const
{
    return _hasLogicalTimeRepresentation;
}

std::ostream& operator<<(std::ostream& os, const M_Create_Federation_Execution_V4& msg)
{
    os << "[M_Create_Federation_Execution_V4 - Begin]" << std::endl;
    
    os << static_cast<const M_Create_Federation_Execution_V4::Super&>(msg); // show parent class
    
    // Specific display
    os << "  federationExecutionName = " << msg.federationExecutionName << std::endl;
    os << "  rtiVersion = " << msg.rtiVersion << std::endl;
    os << "  fomModuleDesignators [] =" << std::endl;
    for (const auto& element : msg.fomModuleDesignators) {
        os << element;
    }
    os << std::endl;
    os << "  (opt) mimDesignator =" << msg.mimDesignator << std::endl;
    os << "  (opt) logicalTimeRepresentation =" << msg.logicalTimeRepresentation << std::endl;
    
    os << "[M_Create_Federation_Execution_V4 - End]" << std::endl;
    return os;
}

M_Destroy_Federation_Execution::M_Destroy_Federation_Execution()
{
    this->messageName = "M_Destroy_Federation_Execution";
    this->type = Message::DESTROY_FEDERATION_EXECUTION;
}

void M_Destroy_Federation_Execution::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_string(federationName);
}

void M_Destroy_Federation_Execution::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    msgBuffer.read_string(federationName);
}

const std::string& M_Destroy_Federation_Execution::getFederationName() const
{
    return federationName;
}

void M_Destroy_Federation_Execution::setFederationName(const std::string& newFederationName)
{
    federationName = newFederationName;
}

std::ostream& operator<<(std::ostream& os, const M_Destroy_Federation_Execution& msg)
{
    os << "[M_Destroy_Federation_Execution - Begin]" << std::endl;
    
    os << static_cast<const M_Destroy_Federation_Execution::Super&>(msg); // show parent class
    
    // Specific display
    os << "  federationName = " << msg.federationName << std::endl;
    
    os << "[M_Destroy_Federation_Execution - End]" << std::endl;
    return os;
}

M_Join_Federation_Execution::M_Join_Federation_Execution()
{
    this->messageName = "M_Join_Federation_Execution";
    this->type = Message::JOIN_FEDERATION_EXECUTION;
}

void M_Join_Federation_Execution::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(federate);
    msgBuffer.write_string(federationName);
    msgBuffer.write_string(federateName);
}

void M_Join_Federation_Execution::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    federate = static_cast<FederateHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(federationName);
    msgBuffer.read_string(federateName);
}

const FederateHandle& M_Join_Federation_Execution::getFederate() const
{
    return federate;
}

void M_Join_Federation_Execution::setFederate(const FederateHandle& newFederate)
{
    federate = newFederate;
}

const std::string& M_Join_Federation_Execution::getFederationName() const
{
    return federationName;
}

void M_Join_Federation_Execution::setFederationName(const std::string& newFederationName)
{
    federationName = newFederationName;
}

const std::string& M_Join_Federation_Execution::getFederateName() const
{
    return federateName;
}

void M_Join_Federation_Execution::setFederateName(const std::string& newFederateName)
{
    federateName = newFederateName;
}

std::ostream& operator<<(std::ostream& os, const M_Join_Federation_Execution& msg)
{
    os << "[M_Join_Federation_Execution - Begin]" << std::endl;
    
    os << static_cast<const M_Join_Federation_Execution::Super&>(msg); // show parent class
    
    // Specific display
    os << "  federate = " << msg.federate << std::endl;
    os << "  federationName = " << msg.federationName << std::endl;
    os << "  federateName = " << msg.federateName << std::endl;
    
    os << "[M_Join_Federation_Execution - End]" << std::endl;
    return os;
}

M_Join_Federation_Execution_V4::M_Join_Federation_Execution_V4()
{
    this->messageName = "M_Join_Federation_Execution_V4";
    this->type = Message::JOIN_FEDERATION_EXECUTION_V4;
}

void M_Join_Federation_Execution_V4::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(federate);
    msgBuffer.write_bool(_hasFederateName);
    if (_hasFederateName) {
        msgBuffer.write_string(federateName);
    }
    msgBuffer.write_string(federateType);
    msgBuffer.write_uint8(rtiVersion);
    msgBuffer.write_string(federationExecutionName);
    uint32_t additionalFomModulesSize = additionalFomModules.size();
    msgBuffer.write_uint32(additionalFomModulesSize);
    for (uint32_t i = 0; i < additionalFomModulesSize; ++i) {
        msgBuffer.write_string(additionalFomModules[i]);
    }
}

void M_Join_Federation_Execution_V4::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    federate = static_cast<FederateHandle>(msgBuffer.read_uint32());
    _hasFederateName = msgBuffer.read_bool();
    if (_hasFederateName) {
        msgBuffer.read_string(federateName);
    }
    msgBuffer.read_string(federateType);
    rtiVersion = static_cast<RtiVersion>(msgBuffer.read_uint8());
    msgBuffer.read_string(federationExecutionName);
    uint32_t additionalFomModulesSize = msgBuffer.read_uint32();
    additionalFomModules.resize(additionalFomModulesSize);
    for (uint32_t i = 0; i < additionalFomModulesSize; ++i) {
        msgBuffer.read_string(additionalFomModules[i]);
    }
}

const FederateHandle& M_Join_Federation_Execution_V4::getFederate() const
{
    return federate;
}

void M_Join_Federation_Execution_V4::setFederate(const FederateHandle& newFederate)
{
    federate = newFederate;
}

const std::string& M_Join_Federation_Execution_V4::getFederateName() const
{
    return federateName;
}

void M_Join_Federation_Execution_V4::setFederateName(const std::string& newFederateName)
{
    _hasFederateName = true;
    federateName = newFederateName;
}

bool M_Join_Federation_Execution_V4::hasFederateName() const
{
    return _hasFederateName;
}

const std::string& M_Join_Federation_Execution_V4::getFederateType() const
{
    return federateType;
}

void M_Join_Federation_Execution_V4::setFederateType(const std::string& newFederateType)
{
    federateType = newFederateType;
}

const RtiVersion& M_Join_Federation_Execution_V4::getRtiVersion() const
{
    return rtiVersion;
}

void M_Join_Federation_Execution_V4::setRtiVersion(const RtiVersion& newRtiVersion)
{
    rtiVersion = newRtiVersion;
}

const std::string& M_Join_Federation_Execution_V4::getFederationExecutionName() const
{
    return federationExecutionName;
}

void M_Join_Federation_Execution_V4::setFederationExecutionName(const std::string& newFederationExecutionName)
{
    federationExecutionName = newFederationExecutionName;
}

uint32_t M_Join_Federation_Execution_V4::getAdditionalFomModulesSize() const
{
    return additionalFomModules.size();
}

void M_Join_Federation_Execution_V4::setAdditionalFomModulesSize(uint32_t num)
{
    additionalFomModules.resize(num);
}

const std::vector<std::string>& M_Join_Federation_Execution_V4::getAdditionalFomModules() const
{
    return additionalFomModules;
}

const std::string& M_Join_Federation_Execution_V4::getAdditionalFomModules(uint32_t rank) const
{
    return additionalFomModules[rank];
}

std::string& M_Join_Federation_Execution_V4::getAdditionalFomModules(uint32_t rank)
{
    return additionalFomModules[rank];
}

void M_Join_Federation_Execution_V4::setAdditionalFomModules(const std::string& newAdditionalFomModules, uint32_t rank)
{
    additionalFomModules[rank] = newAdditionalFomModules;
}

void M_Join_Federation_Execution_V4::removeAdditionalFomModules(uint32_t rank)
{
    additionalFomModules.erase(additionalFomModules.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Join_Federation_Execution_V4& msg)
{
    os << "[M_Join_Federation_Execution_V4 - Begin]" << std::endl;
    
    os << static_cast<const M_Join_Federation_Execution_V4::Super&>(msg); // show parent class
    
    // Specific display
    os << "  federate = " << msg.federate << std::endl;
    os << "  (opt) federateName =" << msg.federateName << std::endl;
    os << "  federateType = " << msg.federateType << std::endl;
    os << "  rtiVersion = " << msg.rtiVersion << std::endl;
    os << "  federationExecutionName = " << msg.federationExecutionName << std::endl;
    os << "  additionalFomModules [] =" << std::endl;
    for (const auto& element : msg.additionalFomModules) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Join_Federation_Execution_V4 - End]" << std::endl;
    return os;
}

M_Resign_Federation_Execution::M_Resign_Federation_Execution()
{
    this->messageName = "M_Resign_Federation_Execution";
    this->type = Message::RESIGN_FEDERATION_EXECUTION;
}

void M_Resign_Federation_Execution::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint16(resignAction);
}

void M_Resign_Federation_Execution::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    resignAction = static_cast<ResignAction>(msgBuffer.read_uint16());
}

const ResignAction& M_Resign_Federation_Execution::getResignAction() const
{
    return resignAction;
}

void M_Resign_Federation_Execution::setResignAction(const ResignAction& newResignAction)
{
    resignAction = newResignAction;
}

std::ostream& operator<<(std::ostream& os, const M_Resign_Federation_Execution& msg)
{
    os << "[M_Resign_Federation_Execution - Begin]" << std::endl;
    
    os << static_cast<const M_Resign_Federation_Execution::Super&>(msg); // show parent class
    
    // Specific display
    os << "  resignAction = " << msg.resignAction << std::endl;
    
    os << "[M_Resign_Federation_Execution - End]" << std::endl;
    return os;
}

M_Register_Federation_Synchronization_Point::M_Register_Federation_Synchronization_Point()
{
    this->messageName = "M_Register_Federation_Synchronization_Point";
    this->type = Message::REGISTER_FEDERATION_SYNCHRONIZATION_POINT;
}

void M_Register_Federation_Synchronization_Point::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    uint32_t federateSetSize = federateSet.size();
    msgBuffer.write_uint32(federateSetSize);
    for (uint32_t i = 0; i < federateSetSize; ++i) {
        msgBuffer.write_uint32(federateSet[i]);
    }
}

void M_Register_Federation_Synchronization_Point::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    uint32_t federateSetSize = msgBuffer.read_uint32();
    federateSet.resize(federateSetSize);
    for (uint32_t i = 0; i < federateSetSize; ++i) {
        federateSet[i] = static_cast<FederateHandle>(msgBuffer.read_uint32());
    }
}

uint32_t M_Register_Federation_Synchronization_Point::getFederateSetSize() const
{
    return federateSet.size();
}

void M_Register_Federation_Synchronization_Point::setFederateSetSize(uint32_t num)
{
    federateSet.resize(num);
}

const std::vector<FederateHandle>& M_Register_Federation_Synchronization_Point::getFederateSet() const
{
    return federateSet;
}

const FederateHandle& M_Register_Federation_Synchronization_Point::getFederateSet(uint32_t rank) const
{
    return federateSet[rank];
}

FederateHandle& M_Register_Federation_Synchronization_Point::getFederateSet(uint32_t rank)
{
    return federateSet[rank];
}

void M_Register_Federation_Synchronization_Point::setFederateSet(const FederateHandle& newFederateSet, uint32_t rank)
{
    federateSet[rank] = newFederateSet;
}

void M_Register_Federation_Synchronization_Point::removeFederateSet(uint32_t rank)
{
    federateSet.erase(federateSet.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Register_Federation_Synchronization_Point& msg)
{
    os << "[M_Register_Federation_Synchronization_Point - Begin]" << std::endl;
    
    os << static_cast<const M_Register_Federation_Synchronization_Point::Super&>(msg); // show parent class
    
    // Specific display
    os << "  federateSet [] =" << std::endl;
    for (const auto& element : msg.federateSet) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Register_Federation_Synchronization_Point - End]" << std::endl;
    return os;
}

M_Synchronization_Point_Registration_Failed::M_Synchronization_Point_Registration_Failed()
{
    this->messageName = "M_Synchronization_Point_Registration_Failed";
    this->type = Message::SYNCHRONIZATION_POINT_REGISTRATION_FAILED;
}

M_Synchronization_Point_Registration_Succeeded::M_Synchronization_Point_Registration_Succeeded()
{
    this->messageName = "M_Synchronization_Point_Registration_Succeeded";
    this->type = Message::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED;
}

M_Announce_Synchronization_Point::M_Announce_Synchronization_Point()
{
    this->messageName = "M_Announce_Synchronization_Point";
    this->type = Message::ANNOUNCE_SYNCHRONIZATION_POINT;
}

M_Synchronization_Point_Achieved::M_Synchronization_Point_Achieved()
{
    this->messageName = "M_Synchronization_Point_Achieved";
    this->type = Message::SYNCHRONIZATION_POINT_ACHIEVED;
}

M_Federation_Synchronized::M_Federation_Synchronized()
{
    this->messageName = "M_Federation_Synchronized";
    this->type = Message::FEDERATION_SYNCHRONIZED;
}

M_Request_Federation_Save::M_Request_Federation_Save()
{
    this->messageName = "M_Request_Federation_Save";
    this->type = Message::REQUEST_FEDERATION_SAVE;
}

M_Initiate_Federate_Save::M_Initiate_Federate_Save()
{
    this->messageName = "M_Initiate_Federate_Save";
    this->type = Message::INITIATE_FEDERATE_SAVE;
}

M_Federate_Save_Begun::M_Federate_Save_Begun()
{
    this->messageName = "M_Federate_Save_Begun";
    this->type = Message::FEDERATE_SAVE_BEGUN;
}

M_Federate_Save_Complete::M_Federate_Save_Complete()
{
    this->messageName = "M_Federate_Save_Complete";
    this->type = Message::FEDERATE_SAVE_COMPLETE;
}

M_Federate_Save_Not_Complete::M_Federate_Save_Not_Complete()
{
    this->messageName = "M_Federate_Save_Not_Complete";
    this->type = Message::FEDERATE_SAVE_NOT_COMPLETE;
}

M_Federation_Saved::M_Federation_Saved()
{
    this->messageName = "M_Federation_Saved";
    this->type = Message::FEDERATION_SAVED;
}

M_Federation_Not_Saved::M_Federation_Not_Saved()
{
    this->messageName = "M_Federation_Not_Saved";
    this->type = Message::FEDERATION_NOT_SAVED;
}

M_Request_Federation_Restore::M_Request_Federation_Restore()
{
    this->messageName = "M_Request_Federation_Restore";
    this->type = Message::REQUEST_FEDERATION_RESTORE;
}

M_Request_Federation_Restore_Failed::M_Request_Federation_Restore_Failed()
{
    this->messageName = "M_Request_Federation_Restore_Failed";
    this->type = Message::REQUEST_FEDERATION_RESTORE_FAILED;
}

void M_Request_Federation_Restore_Failed::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_string(reason);
}

void M_Request_Federation_Restore_Failed::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    msgBuffer.read_string(reason);
}

const std::string& M_Request_Federation_Restore_Failed::getReason() const
{
    return reason;
}

void M_Request_Federation_Restore_Failed::setReason(const std::string& newReason)
{
    reason = newReason;
}

std::ostream& operator<<(std::ostream& os, const M_Request_Federation_Restore_Failed& msg)
{
    os << "[M_Request_Federation_Restore_Failed - Begin]" << std::endl;
    
    os << static_cast<const M_Request_Federation_Restore_Failed::Super&>(msg); // show parent class
    
    // Specific display
    os << "  reason = " << msg.reason << std::endl;
    
    os << "[M_Request_Federation_Restore_Failed - End]" << std::endl;
    return os;
}

M_Request_Federation_Restore_Succeeded::M_Request_Federation_Restore_Succeeded()
{
    this->messageName = "M_Request_Federation_Restore_Succeeded";
    this->type = Message::REQUEST_FEDERATION_RESTORE_SUCCEEDED;
}

M_Initiate_Federate_Restore::M_Initiate_Federate_Restore()
{
    this->messageName = "M_Initiate_Federate_Restore";
    this->type = Message::INITIATE_FEDERATE_RESTORE;
}

void M_Initiate_Federate_Restore::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(federate);
}

void M_Initiate_Federate_Restore::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    federate = static_cast<FederateHandle>(msgBuffer.read_uint32());
}

const FederateHandle& M_Initiate_Federate_Restore::getFederate() const
{
    return federate;
}

void M_Initiate_Federate_Restore::setFederate(const FederateHandle& newFederate)
{
    federate = newFederate;
}

std::ostream& operator<<(std::ostream& os, const M_Initiate_Federate_Restore& msg)
{
    os << "[M_Initiate_Federate_Restore - Begin]" << std::endl;
    
    os << static_cast<const M_Initiate_Federate_Restore::Super&>(msg); // show parent class
    
    // Specific display
    os << "  federate = " << msg.federate << std::endl;
    
    os << "[M_Initiate_Federate_Restore - End]" << std::endl;
    return os;
}

M_Federate_Restore_Complete::M_Federate_Restore_Complete()
{
    this->messageName = "M_Federate_Restore_Complete";
    this->type = Message::FEDERATE_RESTORE_COMPLETE;
}

M_Federate_Restore_Not_Complete::M_Federate_Restore_Not_Complete()
{
    this->messageName = "M_Federate_Restore_Not_Complete";
    this->type = Message::FEDERATE_RESTORE_NOT_COMPLETE;
}

M_Federation_Restored::M_Federation_Restored()
{
    this->messageName = "M_Federation_Restored";
    this->type = Message::FEDERATION_RESTORED;
}

M_Federation_Not_Restored::M_Federation_Not_Restored()
{
    this->messageName = "M_Federation_Not_Restored";
    this->type = Message::FEDERATION_NOT_RESTORED;
}

M_Federation_Restore_Begun::M_Federation_Restore_Begun()
{
    this->messageName = "M_Federation_Restore_Begun";
    this->type = Message::FEDERATION_RESTORE_BEGUN;
}

M_Publish_Object_Class::M_Publish_Object_Class()
{
    this->messageName = "M_Publish_Object_Class";
    this->type = Message::PUBLISH_OBJECT_CLASS;
}

void M_Publish_Object_Class::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Publish_Object_Class::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectClassHandle& M_Publish_Object_Class::getObjectClass() const
{
    return objectClass;
}

void M_Publish_Object_Class::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

uint32_t M_Publish_Object_Class::getAttributesSize() const
{
    return attributes.size();
}

void M_Publish_Object_Class::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Publish_Object_Class::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Publish_Object_Class::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Publish_Object_Class::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Publish_Object_Class::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Publish_Object_Class::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Publish_Object_Class& msg)
{
    os << "[M_Publish_Object_Class - Begin]" << std::endl;
    
    os << static_cast<const M_Publish_Object_Class::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Publish_Object_Class - End]" << std::endl;
    return os;
}

M_Unpublish_Object_Class::M_Unpublish_Object_Class()
{
    this->messageName = "M_Unpublish_Object_Class";
    this->type = Message::UNPUBLISH_OBJECT_CLASS;
}

void M_Unpublish_Object_Class::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
}

void M_Unpublish_Object_Class::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
}

const ObjectClassHandle& M_Unpublish_Object_Class::getObjectClass() const
{
    return objectClass;
}

void M_Unpublish_Object_Class::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

std::ostream& operator<<(std::ostream& os, const M_Unpublish_Object_Class& msg)
{
    os << "[M_Unpublish_Object_Class - Begin]" << std::endl;
    
    os << static_cast<const M_Unpublish_Object_Class::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    
    os << "[M_Unpublish_Object_Class - End]" << std::endl;
    return os;
}

M_Publish_Interaction_Class::M_Publish_Interaction_Class()
{
    this->messageName = "M_Publish_Interaction_Class";
    this->type = Message::PUBLISH_INTERACTION_CLASS;
}

void M_Publish_Interaction_Class::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
}

void M_Publish_Interaction_Class::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
}

const InteractionClassHandle& M_Publish_Interaction_Class::getInteractionClass() const
{
    return interactionClass;
}

void M_Publish_Interaction_Class::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

std::ostream& operator<<(std::ostream& os, const M_Publish_Interaction_Class& msg)
{
    os << "[M_Publish_Interaction_Class - Begin]" << std::endl;
    
    os << static_cast<const M_Publish_Interaction_Class::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    
    os << "[M_Publish_Interaction_Class - End]" << std::endl;
    return os;
}

M_Unpublish_Interaction_Class::M_Unpublish_Interaction_Class()
{
    this->messageName = "M_Unpublish_Interaction_Class";
    this->type = Message::UNPUBLISH_INTERACTION_CLASS;
}

void M_Unpublish_Interaction_Class::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
}

void M_Unpublish_Interaction_Class::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
}

const InteractionClassHandle& M_Unpublish_Interaction_Class::getInteractionClass() const
{
    return interactionClass;
}

void M_Unpublish_Interaction_Class::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

std::ostream& operator<<(std::ostream& os, const M_Unpublish_Interaction_Class& msg)
{
    os << "[M_Unpublish_Interaction_Class - Begin]" << std::endl;
    
    os << static_cast<const M_Unpublish_Interaction_Class::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    
    os << "[M_Unpublish_Interaction_Class - End]" << std::endl;
    return os;
}

M_Subscribe_Object_Class_Attributes::M_Subscribe_Object_Class_Attributes()
{
    this->messageName = "M_Subscribe_Object_Class_Attributes";
    this->type = Message::SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES;
}

void M_Subscribe_Object_Class_Attributes::serialize(libhla::MessageBuffer& msgBuffer)
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
    msgBuffer.write_bool(active);
}

void M_Subscribe_Object_Class_Attributes::deserialize(libhla::MessageBuffer& msgBuffer)
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
    active = msgBuffer.read_bool();
}

const ObjectClassHandle& M_Subscribe_Object_Class_Attributes::getObjectClass() const
{
    return objectClass;
}

void M_Subscribe_Object_Class_Attributes::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

uint32_t M_Subscribe_Object_Class_Attributes::getAttributesSize() const
{
    return attributes.size();
}

void M_Subscribe_Object_Class_Attributes::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Subscribe_Object_Class_Attributes::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Subscribe_Object_Class_Attributes::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Subscribe_Object_Class_Attributes::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Subscribe_Object_Class_Attributes::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Subscribe_Object_Class_Attributes::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

const bool& M_Subscribe_Object_Class_Attributes::getActive() const
{
    return active;
}

void M_Subscribe_Object_Class_Attributes::setActive(const bool& newActive)
{
    active = newActive;
}

std::ostream& operator<<(std::ostream& os, const M_Subscribe_Object_Class_Attributes& msg)
{
    os << "[M_Subscribe_Object_Class_Attributes - Begin]" << std::endl;
    
    os << static_cast<const M_Subscribe_Object_Class_Attributes::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    os << "  active = " << msg.active << std::endl;
    
    os << "[M_Subscribe_Object_Class_Attributes - End]" << std::endl;
    return os;
}

M_Unsubscribe_Object_Class::M_Unsubscribe_Object_Class()
{
    this->messageName = "M_Unsubscribe_Object_Class";
    this->type = Message::UNSUBSCRIBE_OBJECT_CLASS;
}

void M_Unsubscribe_Object_Class::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
}

void M_Unsubscribe_Object_Class::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
}

const ObjectClassHandle& M_Unsubscribe_Object_Class::getObjectClass() const
{
    return objectClass;
}

void M_Unsubscribe_Object_Class::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

std::ostream& operator<<(std::ostream& os, const M_Unsubscribe_Object_Class& msg)
{
    os << "[M_Unsubscribe_Object_Class - Begin]" << std::endl;
    
    os << static_cast<const M_Unsubscribe_Object_Class::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    
    os << "[M_Unsubscribe_Object_Class - End]" << std::endl;
    return os;
}

M_Subscribe_Interaction_Class::M_Subscribe_Interaction_Class()
{
    this->messageName = "M_Subscribe_Interaction_Class";
    this->type = Message::SUBSCRIBE_INTERACTION_CLASS;
}

void M_Subscribe_Interaction_Class::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
}

void M_Subscribe_Interaction_Class::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
}

const InteractionClassHandle& M_Subscribe_Interaction_Class::getInteractionClass() const
{
    return interactionClass;
}

void M_Subscribe_Interaction_Class::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

std::ostream& operator<<(std::ostream& os, const M_Subscribe_Interaction_Class& msg)
{
    os << "[M_Subscribe_Interaction_Class - Begin]" << std::endl;
    
    os << static_cast<const M_Subscribe_Interaction_Class::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    
    os << "[M_Subscribe_Interaction_Class - End]" << std::endl;
    return os;
}

M_Unsubscribe_Interaction_Class::M_Unsubscribe_Interaction_Class()
{
    this->messageName = "M_Unsubscribe_Interaction_Class";
    this->type = Message::UNSUBSCRIBE_INTERACTION_CLASS;
}

void M_Unsubscribe_Interaction_Class::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
}

void M_Unsubscribe_Interaction_Class::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
}

const InteractionClassHandle& M_Unsubscribe_Interaction_Class::getInteractionClass() const
{
    return interactionClass;
}

void M_Unsubscribe_Interaction_Class::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

std::ostream& operator<<(std::ostream& os, const M_Unsubscribe_Interaction_Class& msg)
{
    os << "[M_Unsubscribe_Interaction_Class - Begin]" << std::endl;
    
    os << static_cast<const M_Unsubscribe_Interaction_Class::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    
    os << "[M_Unsubscribe_Interaction_Class - End]" << std::endl;
    return os;
}

M_Start_Registration_For_Object_Class::M_Start_Registration_For_Object_Class()
{
    this->messageName = "M_Start_Registration_For_Object_Class";
    this->type = Message::START_REGISTRATION_FOR_OBJECT_CLASS;
}

void M_Start_Registration_For_Object_Class::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
}

void M_Start_Registration_For_Object_Class::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
}

const ObjectClassHandle& M_Start_Registration_For_Object_Class::getObjectClass() const
{
    return objectClass;
}

void M_Start_Registration_For_Object_Class::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

std::ostream& operator<<(std::ostream& os, const M_Start_Registration_For_Object_Class& msg)
{
    os << "[M_Start_Registration_For_Object_Class - Begin]" << std::endl;
    
    os << static_cast<const M_Start_Registration_For_Object_Class::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    
    os << "[M_Start_Registration_For_Object_Class - End]" << std::endl;
    return os;
}

M_Stop_Registration_For_Object_Class::M_Stop_Registration_For_Object_Class()
{
    this->messageName = "M_Stop_Registration_For_Object_Class";
    this->type = Message::STOP_REGISTRATION_FOR_OBJECT_CLASS;
}

void M_Stop_Registration_For_Object_Class::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
}

void M_Stop_Registration_For_Object_Class::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
}

const ObjectClassHandle& M_Stop_Registration_For_Object_Class::getObjectClass() const
{
    return objectClass;
}

void M_Stop_Registration_For_Object_Class::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

std::ostream& operator<<(std::ostream& os, const M_Stop_Registration_For_Object_Class& msg)
{
    os << "[M_Stop_Registration_For_Object_Class - Begin]" << std::endl;
    
    os << static_cast<const M_Stop_Registration_For_Object_Class::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    
    os << "[M_Stop_Registration_For_Object_Class - End]" << std::endl;
    return os;
}

M_Turn_Interactions_On::M_Turn_Interactions_On()
{
    this->messageName = "M_Turn_Interactions_On";
    this->type = Message::TURN_INTERACTIONS_ON;
}

void M_Turn_Interactions_On::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
}

void M_Turn_Interactions_On::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
}

const InteractionClassHandle& M_Turn_Interactions_On::getInteractionClass() const
{
    return interactionClass;
}

void M_Turn_Interactions_On::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

std::ostream& operator<<(std::ostream& os, const M_Turn_Interactions_On& msg)
{
    os << "[M_Turn_Interactions_On - Begin]" << std::endl;
    
    os << static_cast<const M_Turn_Interactions_On::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    
    os << "[M_Turn_Interactions_On - End]" << std::endl;
    return os;
}

M_Turn_Interactions_Off::M_Turn_Interactions_Off()
{
    this->messageName = "M_Turn_Interactions_Off";
    this->type = Message::TURN_INTERACTIONS_OFF;
}

void M_Turn_Interactions_Off::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
}

void M_Turn_Interactions_Off::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
}

const InteractionClassHandle& M_Turn_Interactions_Off::getInteractionClass() const
{
    return interactionClass;
}

void M_Turn_Interactions_Off::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

std::ostream& operator<<(std::ostream& os, const M_Turn_Interactions_Off& msg)
{
    os << "[M_Turn_Interactions_Off - Begin]" << std::endl;
    
    os << static_cast<const M_Turn_Interactions_Off::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    
    os << "[M_Turn_Interactions_Off - End]" << std::endl;
    return os;
}

M_Register_Object_Instance::M_Register_Object_Instance()
{
    this->messageName = "M_Register_Object_Instance";
    this->type = Message::REGISTER_OBJECT_INSTANCE;
}

void M_Register_Object_Instance::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_uint32(object);
    msgBuffer.write_bool(_hasObjectName);
    if (_hasObjectName) {
        msgBuffer.write_string(objectName);
    }
}

void M_Register_Object_Instance::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    _hasObjectName = msgBuffer.read_bool();
    if (_hasObjectName) {
        msgBuffer.read_string(objectName);
    }
}

const ObjectClassHandle& M_Register_Object_Instance::getObjectClass() const
{
    return objectClass;
}

void M_Register_Object_Instance::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const ObjectHandle& M_Register_Object_Instance::getObject() const
{
    return object;
}

void M_Register_Object_Instance::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const std::string& M_Register_Object_Instance::getObjectName() const
{
    return objectName;
}

void M_Register_Object_Instance::setObjectName(const std::string& newObjectName)
{
    _hasObjectName = true;
    objectName = newObjectName;
}

bool M_Register_Object_Instance::hasObjectName() const
{
    return _hasObjectName;
}

std::ostream& operator<<(std::ostream& os, const M_Register_Object_Instance& msg)
{
    os << "[M_Register_Object_Instance - Begin]" << std::endl;
    
    os << static_cast<const M_Register_Object_Instance::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  object = " << msg.object << std::endl;
    os << "  (opt) objectName =" << msg.objectName << std::endl;
    
    os << "[M_Register_Object_Instance - End]" << std::endl;
    return os;
}

M_Update_Attribute_Values::M_Update_Attribute_Values()
{
    this->messageName = "M_Update_Attribute_Values";
    this->type = Message::UPDATE_ATTRIBUTE_VALUES;
}

void M_Update_Attribute_Values::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
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
    msgBuffer.write_bool(_hasEventRetraction);
    if (_hasEventRetraction) {
        eventRetraction.serialize(msgBuffer);
    }
}

void M_Update_Attribute_Values::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
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
    _hasEventRetraction = msgBuffer.read_bool();
    if (_hasEventRetraction) {
        eventRetraction.deserialize(msgBuffer);
    }
}

const ObjectClassHandle& M_Update_Attribute_Values::getObjectClass() const
{
    return objectClass;
}

void M_Update_Attribute_Values::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const ObjectHandle& M_Update_Attribute_Values::getObject() const
{
    return object;
}

void M_Update_Attribute_Values::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Update_Attribute_Values::getAttributesSize() const
{
    return attributes.size();
}

void M_Update_Attribute_Values::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Update_Attribute_Values::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Update_Attribute_Values::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Update_Attribute_Values::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Update_Attribute_Values::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Update_Attribute_Values::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

uint32_t M_Update_Attribute_Values::getValuesSize() const
{
    return values.size();
}

void M_Update_Attribute_Values::setValuesSize(uint32_t num)
{
    values.resize(num);
}

const std::vector<AttributeValue_t>& M_Update_Attribute_Values::getValues() const
{
    return values;
}

const AttributeValue_t& M_Update_Attribute_Values::getValues(uint32_t rank) const
{
    return values[rank];
}

AttributeValue_t& M_Update_Attribute_Values::getValues(uint32_t rank)
{
    return values[rank];
}

void M_Update_Attribute_Values::setValues(const AttributeValue_t& newValues, uint32_t rank)
{
    values[rank] = newValues;
}

void M_Update_Attribute_Values::removeValues(uint32_t rank)
{
    values.erase(values.begin() + rank);
}

const EventRetraction& M_Update_Attribute_Values::getEventRetraction() const
{
    return eventRetraction;
}

void M_Update_Attribute_Values::setEventRetraction(const EventRetraction& newEventRetraction)
{
    _hasEventRetraction = true;
    eventRetraction = newEventRetraction;
}

bool M_Update_Attribute_Values::hasEventRetraction() const
{
    return _hasEventRetraction;
}

std::ostream& operator<<(std::ostream& os, const M_Update_Attribute_Values& msg)
{
    os << "[M_Update_Attribute_Values - Begin]" << std::endl;
    
    os << static_cast<const M_Update_Attribute_Values::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
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
    os << "  (opt) eventRetraction =" << msg.eventRetraction << std::endl;
    
    os << "[M_Update_Attribute_Values - End]" << std::endl;
    return os;
}

M_Discover_Object_Instance::M_Discover_Object_Instance()
{
    this->messageName = "M_Discover_Object_Instance";
    this->type = Message::DISCOVER_OBJECT_INSTANCE;
}

void M_Discover_Object_Instance::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_uint32(object);
    msgBuffer.write_string(objectName);
    msgBuffer.write_bool(_hasEventRetraction);
    if (_hasEventRetraction) {
        eventRetraction.serialize(msgBuffer);
    }
}

void M_Discover_Object_Instance::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(objectName);
    _hasEventRetraction = msgBuffer.read_bool();
    if (_hasEventRetraction) {
        eventRetraction.deserialize(msgBuffer);
    }
}

const ObjectClassHandle& M_Discover_Object_Instance::getObjectClass() const
{
    return objectClass;
}

void M_Discover_Object_Instance::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const ObjectHandle& M_Discover_Object_Instance::getObject() const
{
    return object;
}

void M_Discover_Object_Instance::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const std::string& M_Discover_Object_Instance::getObjectName() const
{
    return objectName;
}

void M_Discover_Object_Instance::setObjectName(const std::string& newObjectName)
{
    objectName = newObjectName;
}

const EventRetraction& M_Discover_Object_Instance::getEventRetraction() const
{
    return eventRetraction;
}

void M_Discover_Object_Instance::setEventRetraction(const EventRetraction& newEventRetraction)
{
    _hasEventRetraction = true;
    eventRetraction = newEventRetraction;
}

bool M_Discover_Object_Instance::hasEventRetraction() const
{
    return _hasEventRetraction;
}

std::ostream& operator<<(std::ostream& os, const M_Discover_Object_Instance& msg)
{
    os << "[M_Discover_Object_Instance - Begin]" << std::endl;
    
    os << static_cast<const M_Discover_Object_Instance::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  object = " << msg.object << std::endl;
    os << "  objectName = " << msg.objectName << std::endl;
    os << "  (opt) eventRetraction =" << msg.eventRetraction << std::endl;
    
    os << "[M_Discover_Object_Instance - End]" << std::endl;
    return os;
}

M_Reflect_Attribute_Values::M_Reflect_Attribute_Values()
{
    this->messageName = "M_Reflect_Attribute_Values";
    this->type = Message::REFLECT_ATTRIBUTE_VALUES;
}

void M_Reflect_Attribute_Values::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
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
    msgBuffer.write_bool(_hasEventRetraction);
    if (_hasEventRetraction) {
        eventRetraction.serialize(msgBuffer);
    }
}

void M_Reflect_Attribute_Values::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
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
    _hasEventRetraction = msgBuffer.read_bool();
    if (_hasEventRetraction) {
        eventRetraction.deserialize(msgBuffer);
    }
}

const ObjectClassHandle& M_Reflect_Attribute_Values::getObjectClass() const
{
    return objectClass;
}

void M_Reflect_Attribute_Values::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const ObjectHandle& M_Reflect_Attribute_Values::getObject() const
{
    return object;
}

void M_Reflect_Attribute_Values::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Reflect_Attribute_Values::getAttributesSize() const
{
    return attributes.size();
}

void M_Reflect_Attribute_Values::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Reflect_Attribute_Values::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Reflect_Attribute_Values::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Reflect_Attribute_Values::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Reflect_Attribute_Values::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Reflect_Attribute_Values::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

uint32_t M_Reflect_Attribute_Values::getValuesSize() const
{
    return values.size();
}

void M_Reflect_Attribute_Values::setValuesSize(uint32_t num)
{
    values.resize(num);
}

const std::vector<AttributeValue_t>& M_Reflect_Attribute_Values::getValues() const
{
    return values;
}

const AttributeValue_t& M_Reflect_Attribute_Values::getValues(uint32_t rank) const
{
    return values[rank];
}

AttributeValue_t& M_Reflect_Attribute_Values::getValues(uint32_t rank)
{
    return values[rank];
}

void M_Reflect_Attribute_Values::setValues(const AttributeValue_t& newValues, uint32_t rank)
{
    values[rank] = newValues;
}

void M_Reflect_Attribute_Values::removeValues(uint32_t rank)
{
    values.erase(values.begin() + rank);
}

const EventRetraction& M_Reflect_Attribute_Values::getEventRetraction() const
{
    return eventRetraction;
}

void M_Reflect_Attribute_Values::setEventRetraction(const EventRetraction& newEventRetraction)
{
    _hasEventRetraction = true;
    eventRetraction = newEventRetraction;
}

bool M_Reflect_Attribute_Values::hasEventRetraction() const
{
    return _hasEventRetraction;
}

std::ostream& operator<<(std::ostream& os, const M_Reflect_Attribute_Values& msg)
{
    os << "[M_Reflect_Attribute_Values - Begin]" << std::endl;
    
    os << static_cast<const M_Reflect_Attribute_Values::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
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
    os << "  (opt) eventRetraction =" << msg.eventRetraction << std::endl;
    
    os << "[M_Reflect_Attribute_Values - End]" << std::endl;
    return os;
}

M_Send_Interaction::M_Send_Interaction()
{
    this->messageName = "M_Send_Interaction";
    this->type = Message::SEND_INTERACTION;
}

void M_Send_Interaction::serialize(libhla::MessageBuffer& msgBuffer)
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
    msgBuffer.write_bool(_hasEventRetraction);
    if (_hasEventRetraction) {
        eventRetraction.serialize(msgBuffer);
    }
}

void M_Send_Interaction::deserialize(libhla::MessageBuffer& msgBuffer)
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
    _hasEventRetraction = msgBuffer.read_bool();
    if (_hasEventRetraction) {
        eventRetraction.deserialize(msgBuffer);
    }
}

const InteractionClassHandle& M_Send_Interaction::getInteractionClass() const
{
    return interactionClass;
}

void M_Send_Interaction::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

uint32_t M_Send_Interaction::getParametersSize() const
{
    return parameters.size();
}

void M_Send_Interaction::setParametersSize(uint32_t num)
{
    parameters.resize(num);
}

const std::vector<ParameterHandle>& M_Send_Interaction::getParameters() const
{
    return parameters;
}

const ParameterHandle& M_Send_Interaction::getParameters(uint32_t rank) const
{
    return parameters[rank];
}

ParameterHandle& M_Send_Interaction::getParameters(uint32_t rank)
{
    return parameters[rank];
}

void M_Send_Interaction::setParameters(const ParameterHandle& newParameters, uint32_t rank)
{
    parameters[rank] = newParameters;
}

void M_Send_Interaction::removeParameters(uint32_t rank)
{
    parameters.erase(parameters.begin() + rank);
}

uint32_t M_Send_Interaction::getValuesSize() const
{
    return values.size();
}

void M_Send_Interaction::setValuesSize(uint32_t num)
{
    values.resize(num);
}

const std::vector<ParameterValue_t>& M_Send_Interaction::getValues() const
{
    return values;
}

const ParameterValue_t& M_Send_Interaction::getValues(uint32_t rank) const
{
    return values[rank];
}

ParameterValue_t& M_Send_Interaction::getValues(uint32_t rank)
{
    return values[rank];
}

void M_Send_Interaction::setValues(const ParameterValue_t& newValues, uint32_t rank)
{
    values[rank] = newValues;
}

void M_Send_Interaction::removeValues(uint32_t rank)
{
    values.erase(values.begin() + rank);
}

const RegionHandle& M_Send_Interaction::getRegion() const
{
    return region;
}

void M_Send_Interaction::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

const EventRetraction& M_Send_Interaction::getEventRetraction() const
{
    return eventRetraction;
}

void M_Send_Interaction::setEventRetraction(const EventRetraction& newEventRetraction)
{
    _hasEventRetraction = true;
    eventRetraction = newEventRetraction;
}

bool M_Send_Interaction::hasEventRetraction() const
{
    return _hasEventRetraction;
}

std::ostream& operator<<(std::ostream& os, const M_Send_Interaction& msg)
{
    os << "[M_Send_Interaction - Begin]" << std::endl;
    
    os << static_cast<const M_Send_Interaction::Super&>(msg); // show parent class
    
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
    os << "  (opt) eventRetraction =" << msg.eventRetraction << std::endl;
    
    os << "[M_Send_Interaction - End]" << std::endl;
    return os;
}

M_Receive_Interaction::M_Receive_Interaction()
{
    this->messageName = "M_Receive_Interaction";
    this->type = Message::RECEIVE_INTERACTION;
}

void M_Receive_Interaction::serialize(libhla::MessageBuffer& msgBuffer)
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
    msgBuffer.write_bool(_hasEventRetraction);
    if (_hasEventRetraction) {
        eventRetraction.serialize(msgBuffer);
    }
}

void M_Receive_Interaction::deserialize(libhla::MessageBuffer& msgBuffer)
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
    _hasEventRetraction = msgBuffer.read_bool();
    if (_hasEventRetraction) {
        eventRetraction.deserialize(msgBuffer);
    }
}

const InteractionClassHandle& M_Receive_Interaction::getInteractionClass() const
{
    return interactionClass;
}

void M_Receive_Interaction::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

uint32_t M_Receive_Interaction::getParametersSize() const
{
    return parameters.size();
}

void M_Receive_Interaction::setParametersSize(uint32_t num)
{
    parameters.resize(num);
}

const std::vector<ParameterHandle>& M_Receive_Interaction::getParameters() const
{
    return parameters;
}

const ParameterHandle& M_Receive_Interaction::getParameters(uint32_t rank) const
{
    return parameters[rank];
}

ParameterHandle& M_Receive_Interaction::getParameters(uint32_t rank)
{
    return parameters[rank];
}

void M_Receive_Interaction::setParameters(const ParameterHandle& newParameters, uint32_t rank)
{
    parameters[rank] = newParameters;
}

void M_Receive_Interaction::removeParameters(uint32_t rank)
{
    parameters.erase(parameters.begin() + rank);
}

uint32_t M_Receive_Interaction::getValuesSize() const
{
    return values.size();
}

void M_Receive_Interaction::setValuesSize(uint32_t num)
{
    values.resize(num);
}

const std::vector<ParameterValue_t>& M_Receive_Interaction::getValues() const
{
    return values;
}

const ParameterValue_t& M_Receive_Interaction::getValues(uint32_t rank) const
{
    return values[rank];
}

ParameterValue_t& M_Receive_Interaction::getValues(uint32_t rank)
{
    return values[rank];
}

void M_Receive_Interaction::setValues(const ParameterValue_t& newValues, uint32_t rank)
{
    values[rank] = newValues;
}

void M_Receive_Interaction::removeValues(uint32_t rank)
{
    values.erase(values.begin() + rank);
}

const RegionHandle& M_Receive_Interaction::getRegion() const
{
    return region;
}

void M_Receive_Interaction::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

const EventRetraction& M_Receive_Interaction::getEventRetraction() const
{
    return eventRetraction;
}

void M_Receive_Interaction::setEventRetraction(const EventRetraction& newEventRetraction)
{
    _hasEventRetraction = true;
    eventRetraction = newEventRetraction;
}

bool M_Receive_Interaction::hasEventRetraction() const
{
    return _hasEventRetraction;
}

std::ostream& operator<<(std::ostream& os, const M_Receive_Interaction& msg)
{
    os << "[M_Receive_Interaction - Begin]" << std::endl;
    
    os << static_cast<const M_Receive_Interaction::Super&>(msg); // show parent class
    
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
    os << "  (opt) eventRetraction =" << msg.eventRetraction << std::endl;
    
    os << "[M_Receive_Interaction - End]" << std::endl;
    return os;
}

M_Delete_Object_Instance::M_Delete_Object_Instance()
{
    this->messageName = "M_Delete_Object_Instance";
    this->type = Message::DELETE_OBJECT_INSTANCE;
}

void M_Delete_Object_Instance::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_uint32(object);
    msgBuffer.write_string(objectName);
    msgBuffer.write_bool(_hasEventRetraction);
    if (_hasEventRetraction) {
        eventRetraction.serialize(msgBuffer);
    }
}

void M_Delete_Object_Instance::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(objectName);
    _hasEventRetraction = msgBuffer.read_bool();
    if (_hasEventRetraction) {
        eventRetraction.deserialize(msgBuffer);
    }
}

const ObjectClassHandle& M_Delete_Object_Instance::getObjectClass() const
{
    return objectClass;
}

void M_Delete_Object_Instance::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const ObjectHandle& M_Delete_Object_Instance::getObject() const
{
    return object;
}

void M_Delete_Object_Instance::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const std::string& M_Delete_Object_Instance::getObjectName() const
{
    return objectName;
}

void M_Delete_Object_Instance::setObjectName(const std::string& newObjectName)
{
    objectName = newObjectName;
}

const EventRetraction& M_Delete_Object_Instance::getEventRetraction() const
{
    return eventRetraction;
}

void M_Delete_Object_Instance::setEventRetraction(const EventRetraction& newEventRetraction)
{
    _hasEventRetraction = true;
    eventRetraction = newEventRetraction;
}

bool M_Delete_Object_Instance::hasEventRetraction() const
{
    return _hasEventRetraction;
}

std::ostream& operator<<(std::ostream& os, const M_Delete_Object_Instance& msg)
{
    os << "[M_Delete_Object_Instance - Begin]" << std::endl;
    
    os << static_cast<const M_Delete_Object_Instance::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  object = " << msg.object << std::endl;
    os << "  objectName = " << msg.objectName << std::endl;
    os << "  (opt) eventRetraction =" << msg.eventRetraction << std::endl;
    
    os << "[M_Delete_Object_Instance - End]" << std::endl;
    return os;
}

M_Local_Delete_Object_Instance::M_Local_Delete_Object_Instance()
{
    this->messageName = "M_Local_Delete_Object_Instance";
    this->type = Message::LOCAL_DELETE_OBJECT_INSTANCE;
}

void M_Local_Delete_Object_Instance::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
}

void M_Local_Delete_Object_Instance::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
}

const ObjectHandle& M_Local_Delete_Object_Instance::getObject() const
{
    return object;
}

void M_Local_Delete_Object_Instance::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

std::ostream& operator<<(std::ostream& os, const M_Local_Delete_Object_Instance& msg)
{
    os << "[M_Local_Delete_Object_Instance - Begin]" << std::endl;
    
    os << static_cast<const M_Local_Delete_Object_Instance::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    
    os << "[M_Local_Delete_Object_Instance - End]" << std::endl;
    return os;
}

M_Remove_Object_Instance::M_Remove_Object_Instance()
{
    this->messageName = "M_Remove_Object_Instance";
    this->type = Message::REMOVE_OBJECT_INSTANCE;
}

void M_Remove_Object_Instance::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_uint32(object);
    msgBuffer.write_string(objectName);
    msgBuffer.write_bool(_hasEventRetraction);
    if (_hasEventRetraction) {
        eventRetraction.serialize(msgBuffer);
    }
}

void M_Remove_Object_Instance::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(objectName);
    _hasEventRetraction = msgBuffer.read_bool();
    if (_hasEventRetraction) {
        eventRetraction.deserialize(msgBuffer);
    }
}

const ObjectClassHandle& M_Remove_Object_Instance::getObjectClass() const
{
    return objectClass;
}

void M_Remove_Object_Instance::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const ObjectHandle& M_Remove_Object_Instance::getObject() const
{
    return object;
}

void M_Remove_Object_Instance::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const std::string& M_Remove_Object_Instance::getObjectName() const
{
    return objectName;
}

void M_Remove_Object_Instance::setObjectName(const std::string& newObjectName)
{
    objectName = newObjectName;
}

const EventRetraction& M_Remove_Object_Instance::getEventRetraction() const
{
    return eventRetraction;
}

void M_Remove_Object_Instance::setEventRetraction(const EventRetraction& newEventRetraction)
{
    _hasEventRetraction = true;
    eventRetraction = newEventRetraction;
}

bool M_Remove_Object_Instance::hasEventRetraction() const
{
    return _hasEventRetraction;
}

std::ostream& operator<<(std::ostream& os, const M_Remove_Object_Instance& msg)
{
    os << "[M_Remove_Object_Instance - Begin]" << std::endl;
    
    os << static_cast<const M_Remove_Object_Instance::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  object = " << msg.object << std::endl;
    os << "  objectName = " << msg.objectName << std::endl;
    os << "  (opt) eventRetraction =" << msg.eventRetraction << std::endl;
    
    os << "[M_Remove_Object_Instance - End]" << std::endl;
    return os;
}

M_Change_Attribute_Transportation_Type::M_Change_Attribute_Transportation_Type()
{
    this->messageName = "M_Change_Attribute_Transportation_Type";
    this->type = Message::CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE;
}

void M_Change_Attribute_Transportation_Type::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint8(transportationType);
    msgBuffer.write_uint8(orderType);
    msgBuffer.write_uint32(object);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
}

void M_Change_Attribute_Transportation_Type::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    transportationType = static_cast<TransportType>(msgBuffer.read_uint8());
    orderType = static_cast<OrderType>(msgBuffer.read_uint8());
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
}

const TransportType& M_Change_Attribute_Transportation_Type::getTransportationType() const
{
    return transportationType;
}

void M_Change_Attribute_Transportation_Type::setTransportationType(const TransportType& newTransportationType)
{
    transportationType = newTransportationType;
}

const OrderType& M_Change_Attribute_Transportation_Type::getOrderType() const
{
    return orderType;
}

void M_Change_Attribute_Transportation_Type::setOrderType(const OrderType& newOrderType)
{
    orderType = newOrderType;
}

const ObjectHandle& M_Change_Attribute_Transportation_Type::getObject() const
{
    return object;
}

void M_Change_Attribute_Transportation_Type::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Change_Attribute_Transportation_Type::getAttributesSize() const
{
    return attributes.size();
}

void M_Change_Attribute_Transportation_Type::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Change_Attribute_Transportation_Type::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Change_Attribute_Transportation_Type::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Change_Attribute_Transportation_Type::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Change_Attribute_Transportation_Type::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Change_Attribute_Transportation_Type::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Change_Attribute_Transportation_Type& msg)
{
    os << "[M_Change_Attribute_Transportation_Type - Begin]" << std::endl;
    
    os << static_cast<const M_Change_Attribute_Transportation_Type::Super&>(msg); // show parent class
    
    // Specific display
    os << "  transportationType = " << msg.transportationType << std::endl;
    os << "  orderType = " << msg.orderType << std::endl;
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Change_Attribute_Transportation_Type - End]" << std::endl;
    return os;
}

M_Change_Interaction_Transportation_Type::M_Change_Interaction_Transportation_Type()
{
    this->messageName = "M_Change_Interaction_Transportation_Type";
    this->type = Message::CHANGE_INTERACTION_TRANSPORTATION_TYPE;
}

void M_Change_Interaction_Transportation_Type::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    msgBuffer.write_uint8(transportationType);
    msgBuffer.write_uint8(orderType);
}

void M_Change_Interaction_Transportation_Type::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    transportationType = static_cast<TransportType>(msgBuffer.read_uint8());
    orderType = static_cast<OrderType>(msgBuffer.read_uint8());
}

const InteractionClassHandle& M_Change_Interaction_Transportation_Type::getInteractionClass() const
{
    return interactionClass;
}

void M_Change_Interaction_Transportation_Type::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

const TransportType& M_Change_Interaction_Transportation_Type::getTransportationType() const
{
    return transportationType;
}

void M_Change_Interaction_Transportation_Type::setTransportationType(const TransportType& newTransportationType)
{
    transportationType = newTransportationType;
}

const OrderType& M_Change_Interaction_Transportation_Type::getOrderType() const
{
    return orderType;
}

void M_Change_Interaction_Transportation_Type::setOrderType(const OrderType& newOrderType)
{
    orderType = newOrderType;
}

std::ostream& operator<<(std::ostream& os, const M_Change_Interaction_Transportation_Type& msg)
{
    os << "[M_Change_Interaction_Transportation_Type - Begin]" << std::endl;
    
    os << static_cast<const M_Change_Interaction_Transportation_Type::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  transportationType = " << msg.transportationType << std::endl;
    os << "  orderType = " << msg.orderType << std::endl;
    
    os << "[M_Change_Interaction_Transportation_Type - End]" << std::endl;
    return os;
}

M_Request_Object_Attribute_Value_Update::M_Request_Object_Attribute_Value_Update()
{
    this->messageName = "M_Request_Object_Attribute_Value_Update";
    this->type = Message::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE;
}

void M_Request_Object_Attribute_Value_Update::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Request_Object_Attribute_Value_Update::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Request_Object_Attribute_Value_Update::getObject() const
{
    return object;
}

void M_Request_Object_Attribute_Value_Update::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Request_Object_Attribute_Value_Update::getAttributesSize() const
{
    return attributes.size();
}

void M_Request_Object_Attribute_Value_Update::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Request_Object_Attribute_Value_Update::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Request_Object_Attribute_Value_Update::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Request_Object_Attribute_Value_Update::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Request_Object_Attribute_Value_Update::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Request_Object_Attribute_Value_Update::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Request_Object_Attribute_Value_Update& msg)
{
    os << "[M_Request_Object_Attribute_Value_Update - Begin]" << std::endl;
    
    os << static_cast<const M_Request_Object_Attribute_Value_Update::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Request_Object_Attribute_Value_Update - End]" << std::endl;
    return os;
}

M_Request_Class_Attribute_Value_Update::M_Request_Class_Attribute_Value_Update()
{
    this->messageName = "M_Request_Class_Attribute_Value_Update";
    this->type = Message::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE;
}

void M_Request_Class_Attribute_Value_Update::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Request_Class_Attribute_Value_Update::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectClassHandle& M_Request_Class_Attribute_Value_Update::getObjectClass() const
{
    return objectClass;
}

void M_Request_Class_Attribute_Value_Update::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

uint32_t M_Request_Class_Attribute_Value_Update::getAttributesSize() const
{
    return attributes.size();
}

void M_Request_Class_Attribute_Value_Update::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Request_Class_Attribute_Value_Update::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Request_Class_Attribute_Value_Update::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Request_Class_Attribute_Value_Update::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Request_Class_Attribute_Value_Update::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Request_Class_Attribute_Value_Update::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Request_Class_Attribute_Value_Update& msg)
{
    os << "[M_Request_Class_Attribute_Value_Update - Begin]" << std::endl;
    
    os << static_cast<const M_Request_Class_Attribute_Value_Update::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Request_Class_Attribute_Value_Update - End]" << std::endl;
    return os;
}

M_Provide_Attribute_Value_Update::M_Provide_Attribute_Value_Update()
{
    this->messageName = "M_Provide_Attribute_Value_Update";
    this->type = Message::PROVIDE_ATTRIBUTE_VALUE_UPDATE;
}

void M_Provide_Attribute_Value_Update::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Provide_Attribute_Value_Update::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Provide_Attribute_Value_Update::getObject() const
{
    return object;
}

void M_Provide_Attribute_Value_Update::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Provide_Attribute_Value_Update::getAttributesSize() const
{
    return attributes.size();
}

void M_Provide_Attribute_Value_Update::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Provide_Attribute_Value_Update::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Provide_Attribute_Value_Update::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Provide_Attribute_Value_Update::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Provide_Attribute_Value_Update::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Provide_Attribute_Value_Update::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Provide_Attribute_Value_Update& msg)
{
    os << "[M_Provide_Attribute_Value_Update - Begin]" << std::endl;
    
    os << static_cast<const M_Provide_Attribute_Value_Update::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Provide_Attribute_Value_Update - End]" << std::endl;
    return os;
}

M_Attributes_In_Scope::M_Attributes_In_Scope()
{
    this->messageName = "M_Attributes_In_Scope";
    this->type = Message::ATTRIBUTES_IN_SCOPE;
}

void M_Attributes_In_Scope::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Attributes_In_Scope::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Attributes_In_Scope::getObject() const
{
    return object;
}

void M_Attributes_In_Scope::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Attributes_In_Scope::getAttributesSize() const
{
    return attributes.size();
}

void M_Attributes_In_Scope::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Attributes_In_Scope::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Attributes_In_Scope::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Attributes_In_Scope::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Attributes_In_Scope::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Attributes_In_Scope::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Attributes_In_Scope& msg)
{
    os << "[M_Attributes_In_Scope - Begin]" << std::endl;
    
    os << static_cast<const M_Attributes_In_Scope::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Attributes_In_Scope - End]" << std::endl;
    return os;
}

M_Attributes_Out_Of_Scope::M_Attributes_Out_Of_Scope()
{
    this->messageName = "M_Attributes_Out_Of_Scope";
    this->type = Message::ATTRIBUTES_OUT_OF_SCOPE;
}

void M_Attributes_Out_Of_Scope::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Attributes_Out_Of_Scope::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Attributes_Out_Of_Scope::getObject() const
{
    return object;
}

void M_Attributes_Out_Of_Scope::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Attributes_Out_Of_Scope::getAttributesSize() const
{
    return attributes.size();
}

void M_Attributes_Out_Of_Scope::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Attributes_Out_Of_Scope::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Attributes_Out_Of_Scope::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Attributes_Out_Of_Scope::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Attributes_Out_Of_Scope::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Attributes_Out_Of_Scope::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Attributes_Out_Of_Scope& msg)
{
    os << "[M_Attributes_Out_Of_Scope - Begin]" << std::endl;
    
    os << static_cast<const M_Attributes_Out_Of_Scope::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Attributes_Out_Of_Scope - End]" << std::endl;
    return os;
}

M_Turn_Updates_On_For_Object_Instance::M_Turn_Updates_On_For_Object_Instance()
{
    this->messageName = "M_Turn_Updates_On_For_Object_Instance";
    this->type = Message::TURN_UPDATES_ON_FOR_OBJECT_INSTANCE;
}

void M_Turn_Updates_On_For_Object_Instance::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Turn_Updates_On_For_Object_Instance::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Turn_Updates_On_For_Object_Instance::getObject() const
{
    return object;
}

void M_Turn_Updates_On_For_Object_Instance::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Turn_Updates_On_For_Object_Instance::getAttributesSize() const
{
    return attributes.size();
}

void M_Turn_Updates_On_For_Object_Instance::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Turn_Updates_On_For_Object_Instance::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Turn_Updates_On_For_Object_Instance::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Turn_Updates_On_For_Object_Instance::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Turn_Updates_On_For_Object_Instance::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Turn_Updates_On_For_Object_Instance::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Turn_Updates_On_For_Object_Instance& msg)
{
    os << "[M_Turn_Updates_On_For_Object_Instance - Begin]" << std::endl;
    
    os << static_cast<const M_Turn_Updates_On_For_Object_Instance::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Turn_Updates_On_For_Object_Instance - End]" << std::endl;
    return os;
}

M_Turn_Updates_Off_For_Object_Instance::M_Turn_Updates_Off_For_Object_Instance()
{
    this->messageName = "M_Turn_Updates_Off_For_Object_Instance";
    this->type = Message::TURN_UPDATES_OFF_FOR_OBJECT_INSTANCE;
}

void M_Turn_Updates_Off_For_Object_Instance::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Turn_Updates_Off_For_Object_Instance::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Turn_Updates_Off_For_Object_Instance::getObject() const
{
    return object;
}

void M_Turn_Updates_Off_For_Object_Instance::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Turn_Updates_Off_For_Object_Instance::getAttributesSize() const
{
    return attributes.size();
}

void M_Turn_Updates_Off_For_Object_Instance::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Turn_Updates_Off_For_Object_Instance::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Turn_Updates_Off_For_Object_Instance::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Turn_Updates_Off_For_Object_Instance::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Turn_Updates_Off_For_Object_Instance::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Turn_Updates_Off_For_Object_Instance::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Turn_Updates_Off_For_Object_Instance& msg)
{
    os << "[M_Turn_Updates_Off_For_Object_Instance - Begin]" << std::endl;
    
    os << static_cast<const M_Turn_Updates_Off_For_Object_Instance::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Turn_Updates_Off_For_Object_Instance - End]" << std::endl;
    return os;
}

M_Request_Attribute_Ownership_Divestiture::M_Request_Attribute_Ownership_Divestiture()
{
    this->messageName = "M_Request_Attribute_Ownership_Divestiture";
    this->type = Message::REQUEST_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
}

M_Request_Attribute_Ownership_Assumption::M_Request_Attribute_Ownership_Assumption()
{
    this->messageName = "M_Request_Attribute_Ownership_Assumption";
    this->type = Message::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION;
}

void M_Request_Attribute_Ownership_Assumption::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Request_Attribute_Ownership_Assumption::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Request_Attribute_Ownership_Assumption::getObject() const
{
    return object;
}

void M_Request_Attribute_Ownership_Assumption::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Request_Attribute_Ownership_Assumption::getAttributesSize() const
{
    return attributes.size();
}

void M_Request_Attribute_Ownership_Assumption::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Request_Attribute_Ownership_Assumption::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Request_Attribute_Ownership_Assumption::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Request_Attribute_Ownership_Assumption::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Request_Attribute_Ownership_Assumption::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Request_Attribute_Ownership_Assumption::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Request_Attribute_Ownership_Assumption& msg)
{
    os << "[M_Request_Attribute_Ownership_Assumption - Begin]" << std::endl;
    
    os << static_cast<const M_Request_Attribute_Ownership_Assumption::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Request_Attribute_Ownership_Assumption - End]" << std::endl;
    return os;
}

M_Negotiated_Attribute_Ownership_Divestiture::M_Negotiated_Attribute_Ownership_Divestiture()
{
    this->messageName = "M_Negotiated_Attribute_Ownership_Divestiture";
    this->type = Message::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
}

void M_Negotiated_Attribute_Ownership_Divestiture::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Negotiated_Attribute_Ownership_Divestiture::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Negotiated_Attribute_Ownership_Divestiture::getObject() const
{
    return object;
}

void M_Negotiated_Attribute_Ownership_Divestiture::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Negotiated_Attribute_Ownership_Divestiture::getAttributesSize() const
{
    return attributes.size();
}

void M_Negotiated_Attribute_Ownership_Divestiture::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Negotiated_Attribute_Ownership_Divestiture::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Negotiated_Attribute_Ownership_Divestiture::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Negotiated_Attribute_Ownership_Divestiture::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Negotiated_Attribute_Ownership_Divestiture::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Negotiated_Attribute_Ownership_Divestiture::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Negotiated_Attribute_Ownership_Divestiture& msg)
{
    os << "[M_Negotiated_Attribute_Ownership_Divestiture - Begin]" << std::endl;
    
    os << static_cast<const M_Negotiated_Attribute_Ownership_Divestiture::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Negotiated_Attribute_Ownership_Divestiture - End]" << std::endl;
    return os;
}

M_Attribute_Ownership_Divestiture_Notification::M_Attribute_Ownership_Divestiture_Notification()
{
    this->messageName = "M_Attribute_Ownership_Divestiture_Notification";
    this->type = Message::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION;
}

void M_Attribute_Ownership_Divestiture_Notification::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Attribute_Ownership_Divestiture_Notification::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Attribute_Ownership_Divestiture_Notification::getObject() const
{
    return object;
}

void M_Attribute_Ownership_Divestiture_Notification::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Attribute_Ownership_Divestiture_Notification::getAttributesSize() const
{
    return attributes.size();
}

void M_Attribute_Ownership_Divestiture_Notification::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Attribute_Ownership_Divestiture_Notification::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Attribute_Ownership_Divestiture_Notification::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Attribute_Ownership_Divestiture_Notification::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Attribute_Ownership_Divestiture_Notification::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Attribute_Ownership_Divestiture_Notification::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Attribute_Ownership_Divestiture_Notification& msg)
{
    os << "[M_Attribute_Ownership_Divestiture_Notification - Begin]" << std::endl;
    
    os << static_cast<const M_Attribute_Ownership_Divestiture_Notification::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Attribute_Ownership_Divestiture_Notification - End]" << std::endl;
    return os;
}

M_Attribute_Ownership_Acquisition_Notification::M_Attribute_Ownership_Acquisition_Notification()
{
    this->messageName = "M_Attribute_Ownership_Acquisition_Notification";
    this->type = Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION;
}

void M_Attribute_Ownership_Acquisition_Notification::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Attribute_Ownership_Acquisition_Notification::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Attribute_Ownership_Acquisition_Notification::getObject() const
{
    return object;
}

void M_Attribute_Ownership_Acquisition_Notification::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Attribute_Ownership_Acquisition_Notification::getAttributesSize() const
{
    return attributes.size();
}

void M_Attribute_Ownership_Acquisition_Notification::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Attribute_Ownership_Acquisition_Notification::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Attribute_Ownership_Acquisition_Notification::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Attribute_Ownership_Acquisition_Notification::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Attribute_Ownership_Acquisition_Notification::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Attribute_Ownership_Acquisition_Notification::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Attribute_Ownership_Acquisition_Notification& msg)
{
    os << "[M_Attribute_Ownership_Acquisition_Notification - Begin]" << std::endl;
    
    os << static_cast<const M_Attribute_Ownership_Acquisition_Notification::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Attribute_Ownership_Acquisition_Notification - End]" << std::endl;
    return os;
}

M_Request_Attribute_Ownership_Acquisition::M_Request_Attribute_Ownership_Acquisition()
{
    this->messageName = "M_Request_Attribute_Ownership_Acquisition";
    this->type = Message::REQUEST_ATTRIBUTE_OWNERSHIP_ACQUISITION;
}

void M_Request_Attribute_Ownership_Acquisition::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Request_Attribute_Ownership_Acquisition::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Request_Attribute_Ownership_Acquisition::getObject() const
{
    return object;
}

void M_Request_Attribute_Ownership_Acquisition::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Request_Attribute_Ownership_Acquisition::getAttributesSize() const
{
    return attributes.size();
}

void M_Request_Attribute_Ownership_Acquisition::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Request_Attribute_Ownership_Acquisition::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Request_Attribute_Ownership_Acquisition::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Request_Attribute_Ownership_Acquisition::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Request_Attribute_Ownership_Acquisition::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Request_Attribute_Ownership_Acquisition::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Request_Attribute_Ownership_Acquisition& msg)
{
    os << "[M_Request_Attribute_Ownership_Acquisition - Begin]" << std::endl;
    
    os << static_cast<const M_Request_Attribute_Ownership_Acquisition::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Request_Attribute_Ownership_Acquisition - End]" << std::endl;
    return os;
}

M_Request_Attribute_Ownership_Release::M_Request_Attribute_Ownership_Release()
{
    this->messageName = "M_Request_Attribute_Ownership_Release";
    this->type = Message::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE;
}

void M_Request_Attribute_Ownership_Release::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Request_Attribute_Ownership_Release::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Request_Attribute_Ownership_Release::getObject() const
{
    return object;
}

void M_Request_Attribute_Ownership_Release::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Request_Attribute_Ownership_Release::getAttributesSize() const
{
    return attributes.size();
}

void M_Request_Attribute_Ownership_Release::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Request_Attribute_Ownership_Release::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Request_Attribute_Ownership_Release::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Request_Attribute_Ownership_Release::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Request_Attribute_Ownership_Release::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Request_Attribute_Ownership_Release::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Request_Attribute_Ownership_Release& msg)
{
    os << "[M_Request_Attribute_Ownership_Release - Begin]" << std::endl;
    
    os << static_cast<const M_Request_Attribute_Ownership_Release::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Request_Attribute_Ownership_Release - End]" << std::endl;
    return os;
}

M_Query_Attribute_Ownership::M_Query_Attribute_Ownership()
{
    this->messageName = "M_Query_Attribute_Ownership";
    this->type = Message::QUERY_ATTRIBUTE_OWNERSHIP;
}

void M_Query_Attribute_Ownership::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_uint32(attribute);
}

void M_Query_Attribute_Ownership::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    attribute = static_cast<AttributeHandle>(msgBuffer.read_uint32());
}

const ObjectHandle& M_Query_Attribute_Ownership::getObject() const
{
    return object;
}

void M_Query_Attribute_Ownership::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const AttributeHandle& M_Query_Attribute_Ownership::getAttribute() const
{
    return attribute;
}

void M_Query_Attribute_Ownership::setAttribute(const AttributeHandle& newAttribute)
{
    attribute = newAttribute;
}

std::ostream& operator<<(std::ostream& os, const M_Query_Attribute_Ownership& msg)
{
    os << "[M_Query_Attribute_Ownership - Begin]" << std::endl;
    
    os << static_cast<const M_Query_Attribute_Ownership::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attribute = " << msg.attribute << std::endl;
    
    os << "[M_Query_Attribute_Ownership - End]" << std::endl;
    return os;
}

M_Inform_Attribute_Ownership::M_Inform_Attribute_Ownership()
{
    this->messageName = "M_Inform_Attribute_Ownership";
    this->type = Message::INFORM_ATTRIBUTE_OWNERSHIP;
}

void M_Inform_Attribute_Ownership::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_uint32(attribute);
    msgBuffer.write_uint32(federate);
}

void M_Inform_Attribute_Ownership::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    attribute = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    federate = static_cast<FederateHandle>(msgBuffer.read_uint32());
}

const ObjectHandle& M_Inform_Attribute_Ownership::getObject() const
{
    return object;
}

void M_Inform_Attribute_Ownership::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const AttributeHandle& M_Inform_Attribute_Ownership::getAttribute() const
{
    return attribute;
}

void M_Inform_Attribute_Ownership::setAttribute(const AttributeHandle& newAttribute)
{
    attribute = newAttribute;
}

const FederateHandle& M_Inform_Attribute_Ownership::getFederate() const
{
    return federate;
}

void M_Inform_Attribute_Ownership::setFederate(const FederateHandle& newFederate)
{
    federate = newFederate;
}

std::ostream& operator<<(std::ostream& os, const M_Inform_Attribute_Ownership& msg)
{
    os << "[M_Inform_Attribute_Ownership - Begin]" << std::endl;
    
    os << static_cast<const M_Inform_Attribute_Ownership::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attribute = " << msg.attribute << std::endl;
    os << "  federate = " << msg.federate << std::endl;
    
    os << "[M_Inform_Attribute_Ownership - End]" << std::endl;
    return os;
}

M_Is_Attribute_Owned_By_Federate::M_Is_Attribute_Owned_By_Federate()
{
    this->messageName = "M_Is_Attribute_Owned_By_Federate";
    this->type = Message::IS_ATTRIBUTE_OWNED_BY_FEDERATE;
}

void M_Is_Attribute_Owned_By_Federate::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_uint32(attribute);
}

void M_Is_Attribute_Owned_By_Federate::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    attribute = static_cast<AttributeHandle>(msgBuffer.read_uint32());
}

const ObjectHandle& M_Is_Attribute_Owned_By_Federate::getObject() const
{
    return object;
}

void M_Is_Attribute_Owned_By_Federate::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const AttributeHandle& M_Is_Attribute_Owned_By_Federate::getAttribute() const
{
    return attribute;
}

void M_Is_Attribute_Owned_By_Federate::setAttribute(const AttributeHandle& newAttribute)
{
    attribute = newAttribute;
}

std::ostream& operator<<(std::ostream& os, const M_Is_Attribute_Owned_By_Federate& msg)
{
    os << "[M_Is_Attribute_Owned_By_Federate - Begin]" << std::endl;
    
    os << static_cast<const M_Is_Attribute_Owned_By_Federate::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attribute = " << msg.attribute << std::endl;
    
    os << "[M_Is_Attribute_Owned_By_Federate - End]" << std::endl;
    return os;
}

M_Attribute_Is_Not_Owned::M_Attribute_Is_Not_Owned()
{
    this->messageName = "M_Attribute_Is_Not_Owned";
    this->type = Message::ATTRIBUTE_IS_NOT_OWNED;
}

void M_Attribute_Is_Not_Owned::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_uint32(attribute);
    msgBuffer.write_uint32(federate);
}

void M_Attribute_Is_Not_Owned::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    attribute = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    federate = static_cast<FederateHandle>(msgBuffer.read_uint32());
}

const ObjectHandle& M_Attribute_Is_Not_Owned::getObject() const
{
    return object;
}

void M_Attribute_Is_Not_Owned::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const AttributeHandle& M_Attribute_Is_Not_Owned::getAttribute() const
{
    return attribute;
}

void M_Attribute_Is_Not_Owned::setAttribute(const AttributeHandle& newAttribute)
{
    attribute = newAttribute;
}

const FederateHandle& M_Attribute_Is_Not_Owned::getFederate() const
{
    return federate;
}

void M_Attribute_Is_Not_Owned::setFederate(const FederateHandle& newFederate)
{
    federate = newFederate;
}

std::ostream& operator<<(std::ostream& os, const M_Attribute_Is_Not_Owned& msg)
{
    os << "[M_Attribute_Is_Not_Owned - Begin]" << std::endl;
    
    os << static_cast<const M_Attribute_Is_Not_Owned::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attribute = " << msg.attribute << std::endl;
    os << "  federate = " << msg.federate << std::endl;
    
    os << "[M_Attribute_Is_Not_Owned - End]" << std::endl;
    return os;
}

M_Attribute_Owned_By_Rti::M_Attribute_Owned_By_Rti()
{
    this->messageName = "M_Attribute_Owned_By_Rti";
    this->type = Message::ATTRIBUTE_OWNED_BY_RTI;
}

M_Attribute_Ownership_Acquisition_If_Available::M_Attribute_Ownership_Acquisition_If_Available()
{
    this->messageName = "M_Attribute_Ownership_Acquisition_If_Available";
    this->type = Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE;
}

void M_Attribute_Ownership_Acquisition_If_Available::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Attribute_Ownership_Acquisition_If_Available::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Attribute_Ownership_Acquisition_If_Available::getObject() const
{
    return object;
}

void M_Attribute_Ownership_Acquisition_If_Available::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Attribute_Ownership_Acquisition_If_Available::getAttributesSize() const
{
    return attributes.size();
}

void M_Attribute_Ownership_Acquisition_If_Available::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Attribute_Ownership_Acquisition_If_Available::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Attribute_Ownership_Acquisition_If_Available::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Attribute_Ownership_Acquisition_If_Available::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Attribute_Ownership_Acquisition_If_Available::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Attribute_Ownership_Acquisition_If_Available::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Attribute_Ownership_Acquisition_If_Available& msg)
{
    os << "[M_Attribute_Ownership_Acquisition_If_Available - Begin]" << std::endl;
    
    os << static_cast<const M_Attribute_Ownership_Acquisition_If_Available::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Attribute_Ownership_Acquisition_If_Available - End]" << std::endl;
    return os;
}

M_Attribute_Ownership_Unavailable::M_Attribute_Ownership_Unavailable()
{
    this->messageName = "M_Attribute_Ownership_Unavailable";
    this->type = Message::ATTRIBUTE_OWNERSHIP_UNAVAILABLE;
}

void M_Attribute_Ownership_Unavailable::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Attribute_Ownership_Unavailable::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Attribute_Ownership_Unavailable::getObject() const
{
    return object;
}

void M_Attribute_Ownership_Unavailable::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Attribute_Ownership_Unavailable::getAttributesSize() const
{
    return attributes.size();
}

void M_Attribute_Ownership_Unavailable::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Attribute_Ownership_Unavailable::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Attribute_Ownership_Unavailable::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Attribute_Ownership_Unavailable::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Attribute_Ownership_Unavailable::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Attribute_Ownership_Unavailable::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Attribute_Ownership_Unavailable& msg)
{
    os << "[M_Attribute_Ownership_Unavailable - Begin]" << std::endl;
    
    os << static_cast<const M_Attribute_Ownership_Unavailable::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Attribute_Ownership_Unavailable - End]" << std::endl;
    return os;
}

M_Unconditional_Attribute_Ownership_Divestiture::M_Unconditional_Attribute_Ownership_Divestiture()
{
    this->messageName = "M_Unconditional_Attribute_Ownership_Divestiture";
    this->type = Message::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
}

void M_Unconditional_Attribute_Ownership_Divestiture::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Unconditional_Attribute_Ownership_Divestiture::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Unconditional_Attribute_Ownership_Divestiture::getObject() const
{
    return object;
}

void M_Unconditional_Attribute_Ownership_Divestiture::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Unconditional_Attribute_Ownership_Divestiture::getAttributesSize() const
{
    return attributes.size();
}

void M_Unconditional_Attribute_Ownership_Divestiture::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Unconditional_Attribute_Ownership_Divestiture::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Unconditional_Attribute_Ownership_Divestiture::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Unconditional_Attribute_Ownership_Divestiture::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Unconditional_Attribute_Ownership_Divestiture::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Unconditional_Attribute_Ownership_Divestiture::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Unconditional_Attribute_Ownership_Divestiture& msg)
{
    os << "[M_Unconditional_Attribute_Ownership_Divestiture - Begin]" << std::endl;
    
    os << static_cast<const M_Unconditional_Attribute_Ownership_Divestiture::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Unconditional_Attribute_Ownership_Divestiture - End]" << std::endl;
    return os;
}

M_Attribute_Ownership_Acquisition::M_Attribute_Ownership_Acquisition()
{
    this->messageName = "M_Attribute_Ownership_Acquisition";
    this->type = Message::ATTRIBUTE_OWNERSHIP_ACQUISITION;
}

void M_Attribute_Ownership_Acquisition::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Attribute_Ownership_Acquisition::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Attribute_Ownership_Acquisition::getObject() const
{
    return object;
}

void M_Attribute_Ownership_Acquisition::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Attribute_Ownership_Acquisition::getAttributesSize() const
{
    return attributes.size();
}

void M_Attribute_Ownership_Acquisition::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Attribute_Ownership_Acquisition::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Attribute_Ownership_Acquisition::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Attribute_Ownership_Acquisition::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Attribute_Ownership_Acquisition::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Attribute_Ownership_Acquisition::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Attribute_Ownership_Acquisition& msg)
{
    os << "[M_Attribute_Ownership_Acquisition - Begin]" << std::endl;
    
    os << static_cast<const M_Attribute_Ownership_Acquisition::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Attribute_Ownership_Acquisition - End]" << std::endl;
    return os;
}

M_Cancel_Negotiated_Attribute_Ownership_Divestiture::M_Cancel_Negotiated_Attribute_Ownership_Divestiture()
{
    this->messageName = "M_Cancel_Negotiated_Attribute_Ownership_Divestiture";
    this->type = Message::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
}

void M_Cancel_Negotiated_Attribute_Ownership_Divestiture::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Cancel_Negotiated_Attribute_Ownership_Divestiture::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Cancel_Negotiated_Attribute_Ownership_Divestiture::getObject() const
{
    return object;
}

void M_Cancel_Negotiated_Attribute_Ownership_Divestiture::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Cancel_Negotiated_Attribute_Ownership_Divestiture::getAttributesSize() const
{
    return attributes.size();
}

void M_Cancel_Negotiated_Attribute_Ownership_Divestiture::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Cancel_Negotiated_Attribute_Ownership_Divestiture::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Cancel_Negotiated_Attribute_Ownership_Divestiture::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Cancel_Negotiated_Attribute_Ownership_Divestiture::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Cancel_Negotiated_Attribute_Ownership_Divestiture::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Cancel_Negotiated_Attribute_Ownership_Divestiture::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Cancel_Negotiated_Attribute_Ownership_Divestiture& msg)
{
    os << "[M_Cancel_Negotiated_Attribute_Ownership_Divestiture - Begin]" << std::endl;
    
    os << static_cast<const M_Cancel_Negotiated_Attribute_Ownership_Divestiture::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Cancel_Negotiated_Attribute_Ownership_Divestiture - End]" << std::endl;
    return os;
}

M_Attribute_Ownership_Release_Response::M_Attribute_Ownership_Release_Response()
{
    this->messageName = "M_Attribute_Ownership_Release_Response";
    this->type = Message::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE;
}

void M_Attribute_Ownership_Release_Response::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Attribute_Ownership_Release_Response::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Attribute_Ownership_Release_Response::getObject() const
{
    return object;
}

void M_Attribute_Ownership_Release_Response::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Attribute_Ownership_Release_Response::getAttributesSize() const
{
    return attributes.size();
}

void M_Attribute_Ownership_Release_Response::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Attribute_Ownership_Release_Response::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Attribute_Ownership_Release_Response::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Attribute_Ownership_Release_Response::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Attribute_Ownership_Release_Response::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Attribute_Ownership_Release_Response::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Attribute_Ownership_Release_Response& msg)
{
    os << "[M_Attribute_Ownership_Release_Response - Begin]" << std::endl;
    
    os << static_cast<const M_Attribute_Ownership_Release_Response::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Attribute_Ownership_Release_Response - End]" << std::endl;
    return os;
}

M_Cancel_Attribute_Ownership_Acquisition::M_Cancel_Attribute_Ownership_Acquisition()
{
    this->messageName = "M_Cancel_Attribute_Ownership_Acquisition";
    this->type = Message::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION;
}

void M_Cancel_Attribute_Ownership_Acquisition::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Cancel_Attribute_Ownership_Acquisition::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Cancel_Attribute_Ownership_Acquisition::getObject() const
{
    return object;
}

void M_Cancel_Attribute_Ownership_Acquisition::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Cancel_Attribute_Ownership_Acquisition::getAttributesSize() const
{
    return attributes.size();
}

void M_Cancel_Attribute_Ownership_Acquisition::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Cancel_Attribute_Ownership_Acquisition::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Cancel_Attribute_Ownership_Acquisition::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Cancel_Attribute_Ownership_Acquisition::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Cancel_Attribute_Ownership_Acquisition::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Cancel_Attribute_Ownership_Acquisition::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Cancel_Attribute_Ownership_Acquisition& msg)
{
    os << "[M_Cancel_Attribute_Ownership_Acquisition - Begin]" << std::endl;
    
    os << static_cast<const M_Cancel_Attribute_Ownership_Acquisition::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Cancel_Attribute_Ownership_Acquisition - End]" << std::endl;
    return os;
}

M_Confirm_Attribute_Ownership_Acquisition_Cancellation::M_Confirm_Attribute_Ownership_Acquisition_Cancellation()
{
    this->messageName = "M_Confirm_Attribute_Ownership_Acquisition_Cancellation";
    this->type = Message::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION;
}

void M_Confirm_Attribute_Ownership_Acquisition_Cancellation::serialize(libhla::MessageBuffer& msgBuffer)
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

void M_Confirm_Attribute_Ownership_Acquisition_Cancellation::deserialize(libhla::MessageBuffer& msgBuffer)
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

const ObjectHandle& M_Confirm_Attribute_Ownership_Acquisition_Cancellation::getObject() const
{
    return object;
}

void M_Confirm_Attribute_Ownership_Acquisition_Cancellation::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Confirm_Attribute_Ownership_Acquisition_Cancellation::getAttributesSize() const
{
    return attributes.size();
}

void M_Confirm_Attribute_Ownership_Acquisition_Cancellation::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Confirm_Attribute_Ownership_Acquisition_Cancellation::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Confirm_Attribute_Ownership_Acquisition_Cancellation::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Confirm_Attribute_Ownership_Acquisition_Cancellation::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Confirm_Attribute_Ownership_Acquisition_Cancellation::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Confirm_Attribute_Ownership_Acquisition_Cancellation::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Confirm_Attribute_Ownership_Acquisition_Cancellation& msg)
{
    os << "[M_Confirm_Attribute_Ownership_Acquisition_Cancellation - Begin]" << std::endl;
    
    os << static_cast<const M_Confirm_Attribute_Ownership_Acquisition_Cancellation::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Confirm_Attribute_Ownership_Acquisition_Cancellation - End]" << std::endl;
    return os;
}

M_Change_Attribute_Order_Type::M_Change_Attribute_Order_Type()
{
    this->messageName = "M_Change_Attribute_Order_Type";
    this->type = Message::CHANGE_ATTRIBUTE_ORDER_TYPE;
}

void M_Change_Attribute_Order_Type::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint8(transport);
    msgBuffer.write_uint8(order);
    msgBuffer.write_uint32(object);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
}

void M_Change_Attribute_Order_Type::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    transport = static_cast<TransportType>(msgBuffer.read_uint8());
    order = static_cast<OrderType>(msgBuffer.read_uint8());
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
}

const TransportType& M_Change_Attribute_Order_Type::getTransport() const
{
    return transport;
}

void M_Change_Attribute_Order_Type::setTransport(const TransportType& newTransport)
{
    transport = newTransport;
}

const OrderType& M_Change_Attribute_Order_Type::getOrder() const
{
    return order;
}

void M_Change_Attribute_Order_Type::setOrder(const OrderType& newOrder)
{
    order = newOrder;
}

const ObjectHandle& M_Change_Attribute_Order_Type::getObject() const
{
    return object;
}

void M_Change_Attribute_Order_Type::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

uint32_t M_Change_Attribute_Order_Type::getAttributesSize() const
{
    return attributes.size();
}

void M_Change_Attribute_Order_Type::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Change_Attribute_Order_Type::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Change_Attribute_Order_Type::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Change_Attribute_Order_Type::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Change_Attribute_Order_Type::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Change_Attribute_Order_Type::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Change_Attribute_Order_Type& msg)
{
    os << "[M_Change_Attribute_Order_Type - Begin]" << std::endl;
    
    os << static_cast<const M_Change_Attribute_Order_Type::Super&>(msg); // show parent class
    
    // Specific display
    os << "  transport = " << msg.transport << std::endl;
    os << "  order = " << msg.order << std::endl;
    os << "  object = " << msg.object << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Change_Attribute_Order_Type - End]" << std::endl;
    return os;
}

M_Change_Interaction_Order_Type::M_Change_Interaction_Order_Type()
{
    this->messageName = "M_Change_Interaction_Order_Type";
    this->type = Message::CHANGE_INTERACTION_ORDER_TYPE;
}

void M_Change_Interaction_Order_Type::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    msgBuffer.write_uint8(transport);
    msgBuffer.write_uint8(order);
}

void M_Change_Interaction_Order_Type::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    transport = static_cast<TransportType>(msgBuffer.read_uint8());
    order = static_cast<OrderType>(msgBuffer.read_uint8());
}

const InteractionClassHandle& M_Change_Interaction_Order_Type::getInteractionClass() const
{
    return interactionClass;
}

void M_Change_Interaction_Order_Type::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

const TransportType& M_Change_Interaction_Order_Type::getTransport() const
{
    return transport;
}

void M_Change_Interaction_Order_Type::setTransport(const TransportType& newTransport)
{
    transport = newTransport;
}

const OrderType& M_Change_Interaction_Order_Type::getOrder() const
{
    return order;
}

void M_Change_Interaction_Order_Type::setOrder(const OrderType& newOrder)
{
    order = newOrder;
}

std::ostream& operator<<(std::ostream& os, const M_Change_Interaction_Order_Type& msg)
{
    os << "[M_Change_Interaction_Order_Type - Begin]" << std::endl;
    
    os << static_cast<const M_Change_Interaction_Order_Type::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  transport = " << msg.transport << std::endl;
    os << "  order = " << msg.order << std::endl;
    
    os << "[M_Change_Interaction_Order_Type - End]" << std::endl;
    return os;
}

M_Enable_Time_Regulation::M_Enable_Time_Regulation()
{
    this->messageName = "M_Enable_Time_Regulation";
    this->type = Message::ENABLE_TIME_REGULATION;
}

void M_Enable_Time_Regulation::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_double(lookahead);
}

void M_Enable_Time_Regulation::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    lookahead = msgBuffer.read_double();
}

const double& M_Enable_Time_Regulation::getLookahead() const
{
    return lookahead;
}

void M_Enable_Time_Regulation::setLookahead(const double& newLookahead)
{
    lookahead = newLookahead;
}

std::ostream& operator<<(std::ostream& os, const M_Enable_Time_Regulation& msg)
{
    os << "[M_Enable_Time_Regulation - Begin]" << std::endl;
    
    os << static_cast<const M_Enable_Time_Regulation::Super&>(msg); // show parent class
    
    // Specific display
    os << "  lookahead = " << msg.lookahead << std::endl;
    
    os << "[M_Enable_Time_Regulation - End]" << std::endl;
    return os;
}

M_Disable_Time_Regulation::M_Disable_Time_Regulation()
{
    this->messageName = "M_Disable_Time_Regulation";
    this->type = Message::DISABLE_TIME_REGULATION;
}

void M_Disable_Time_Regulation::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_double(lookahead);
}

void M_Disable_Time_Regulation::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    lookahead = msgBuffer.read_double();
}

const double& M_Disable_Time_Regulation::getLookahead() const
{
    return lookahead;
}

void M_Disable_Time_Regulation::setLookahead(const double& newLookahead)
{
    lookahead = newLookahead;
}

std::ostream& operator<<(std::ostream& os, const M_Disable_Time_Regulation& msg)
{
    os << "[M_Disable_Time_Regulation - Begin]" << std::endl;
    
    os << static_cast<const M_Disable_Time_Regulation::Super&>(msg); // show parent class
    
    // Specific display
    os << "  lookahead = " << msg.lookahead << std::endl;
    
    os << "[M_Disable_Time_Regulation - End]" << std::endl;
    return os;
}

M_Enable_Time_Constrained::M_Enable_Time_Constrained()
{
    this->messageName = "M_Enable_Time_Constrained";
    this->type = Message::ENABLE_TIME_CONSTRAINED;
}

M_Disable_Time_Constrained::M_Disable_Time_Constrained()
{
    this->messageName = "M_Disable_Time_Constrained";
    this->type = Message::DISABLE_TIME_CONSTRAINED;
}

M_Query_Lbts::M_Query_Lbts()
{
    this->messageName = "M_Query_Lbts";
    this->type = Message::QUERY_LBTS;
}

M_Query_Federate_Time::M_Query_Federate_Time()
{
    this->messageName = "M_Query_Federate_Time";
    this->type = Message::QUERY_FEDERATE_TIME;
}

M_Query_Min_Next_Event_Time::M_Query_Min_Next_Event_Time()
{
    this->messageName = "M_Query_Min_Next_Event_Time";
    this->type = Message::QUERY_MIN_NEXT_EVENT_TIME;
}

M_Modify_Lookahead::M_Modify_Lookahead()
{
    this->messageName = "M_Modify_Lookahead";
    this->type = Message::MODIFY_LOOKAHEAD;
}

void M_Modify_Lookahead::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_double(lookahead);
}

void M_Modify_Lookahead::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    lookahead = msgBuffer.read_double();
}

const double& M_Modify_Lookahead::getLookahead() const
{
    return lookahead;
}

void M_Modify_Lookahead::setLookahead(const double& newLookahead)
{
    lookahead = newLookahead;
}

std::ostream& operator<<(std::ostream& os, const M_Modify_Lookahead& msg)
{
    os << "[M_Modify_Lookahead - Begin]" << std::endl;
    
    os << static_cast<const M_Modify_Lookahead::Super&>(msg); // show parent class
    
    // Specific display
    os << "  lookahead = " << msg.lookahead << std::endl;
    
    os << "[M_Modify_Lookahead - End]" << std::endl;
    return os;
}

M_Query_Lookahead::M_Query_Lookahead()
{
    this->messageName = "M_Query_Lookahead";
    this->type = Message::QUERY_LOOKAHEAD;
}

void M_Query_Lookahead::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_double(lookahead);
}

void M_Query_Lookahead::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    lookahead = msgBuffer.read_double();
}

const double& M_Query_Lookahead::getLookahead() const
{
    return lookahead;
}

void M_Query_Lookahead::setLookahead(const double& newLookahead)
{
    lookahead = newLookahead;
}

std::ostream& operator<<(std::ostream& os, const M_Query_Lookahead& msg)
{
    os << "[M_Query_Lookahead - Begin]" << std::endl;
    
    os << static_cast<const M_Query_Lookahead::Super&>(msg); // show parent class
    
    // Specific display
    os << "  lookahead = " << msg.lookahead << std::endl;
    
    os << "[M_Query_Lookahead - End]" << std::endl;
    return os;
}

M_Retract::M_Retract()
{
    this->messageName = "M_Retract";
    this->type = Message::RETRACT;
}

void M_Retract::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    eventRetraction.serialize(msgBuffer);
}

void M_Retract::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    eventRetraction.deserialize(msgBuffer);
}

const EventRetraction& M_Retract::getEventRetraction() const
{
    return eventRetraction;
}

void M_Retract::setEventRetraction(const EventRetraction& newEventRetraction)
{
    eventRetraction = newEventRetraction;
}

std::ostream& operator<<(std::ostream& os, const M_Retract& msg)
{
    os << "[M_Retract - Begin]" << std::endl;
    
    os << static_cast<const M_Retract::Super&>(msg); // show parent class
    
    // Specific display
    os << "  eventRetraction = " << msg.eventRetraction << std::endl;
    
    os << "[M_Retract - End]" << std::endl;
    return os;
}

M_Request_Retraction::M_Request_Retraction()
{
    this->messageName = "M_Request_Retraction";
    this->type = Message::REQUEST_RETRACTION;
}

M_Time_Advance_Request::M_Time_Advance_Request()
{
    this->messageName = "M_Time_Advance_Request";
    this->type = Message::TIME_ADVANCE_REQUEST;
}

M_Time_Advance_Request_Available::M_Time_Advance_Request_Available()
{
    this->messageName = "M_Time_Advance_Request_Available";
    this->type = Message::TIME_ADVANCE_REQUEST_AVAILABLE;
}

M_Next_Event_Request::M_Next_Event_Request()
{
    this->messageName = "M_Next_Event_Request";
    this->type = Message::NEXT_EVENT_REQUEST;
}

M_Next_Event_Request_Available::M_Next_Event_Request_Available()
{
    this->messageName = "M_Next_Event_Request_Available";
    this->type = Message::NEXT_EVENT_REQUEST_AVAILABLE;
}

M_Flush_Queue_Request::M_Flush_Queue_Request()
{
    this->messageName = "M_Flush_Queue_Request";
    this->type = Message::FLUSH_QUEUE_REQUEST;
}

M_Time_Advance_Grant::M_Time_Advance_Grant()
{
    this->messageName = "M_Time_Advance_Grant";
    this->type = Message::TIME_ADVANCE_GRANT;
}

M_Enable_Asynchronous_Delivery::M_Enable_Asynchronous_Delivery()
{
    this->messageName = "M_Enable_Asynchronous_Delivery";
    this->type = Message::ENABLE_ASYNCHRONOUS_DELIVERY;
}

M_Disable_Asynchronous_Delivery::M_Disable_Asynchronous_Delivery()
{
    this->messageName = "M_Disable_Asynchronous_Delivery";
    this->type = Message::DISABLE_ASYNCHRONOUS_DELIVERY;
}

M_Time_Regulation_Enabled::M_Time_Regulation_Enabled()
{
    this->messageName = "M_Time_Regulation_Enabled";
    this->type = Message::TIME_REGULATION_ENABLED;
}

M_Time_Constrained_Enabled::M_Time_Constrained_Enabled()
{
    this->messageName = "M_Time_Constrained_Enabled";
    this->type = Message::TIME_CONSTRAINED_ENABLED;
}

M_Ddm_Create_Region::M_Ddm_Create_Region()
{
    this->messageName = "M_Ddm_Create_Region";
    this->type = Message::DDM_CREATE_REGION;
}

void M_Ddm_Create_Region::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(space);
    uint32_t extentSetSize = extentSet.size();
    msgBuffer.write_uint32(extentSetSize);
    for (uint32_t i = 0; i < extentSetSize; ++i) {
        // FIXME FIXME FIXME
        //don't know how to serialize native field <extentSet> of type <Extent>
        //probably no 'representation' given
    }
    msgBuffer.write_uint32(region);
}

void M_Ddm_Create_Region::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    space = static_cast<SpaceHandle>(msgBuffer.read_uint32());
    uint32_t extentSetSize = msgBuffer.read_uint32();
    extentSet.resize(extentSetSize);
    for (uint32_t i = 0; i < extentSetSize; ++i) {
        // FIXME FIXME FIXME
        //don't know how to deserialize native field <extentSet> of type <Extent>
        //probably no 'representation' given
    }
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
}

const SpaceHandle& M_Ddm_Create_Region::getSpace() const
{
    return space;
}

void M_Ddm_Create_Region::setSpace(const SpaceHandle& newSpace)
{
    space = newSpace;
}

uint32_t M_Ddm_Create_Region::getExtentSetSize() const
{
    return extentSet.size();
}

void M_Ddm_Create_Region::setExtentSetSize(uint32_t num)
{
    extentSet.resize(num);
}

const std::vector<Extent>& M_Ddm_Create_Region::getExtentSet() const
{
    return extentSet;
}

const Extent& M_Ddm_Create_Region::getExtentSet(uint32_t rank) const
{
    return extentSet[rank];
}

Extent& M_Ddm_Create_Region::getExtentSet(uint32_t rank)
{
    return extentSet[rank];
}

void M_Ddm_Create_Region::setExtentSet(const Extent& newExtentSet, uint32_t rank)
{
    extentSet[rank] = newExtentSet;
}

void M_Ddm_Create_Region::removeExtentSet(uint32_t rank)
{
    extentSet.erase(extentSet.begin() + rank);
}

const RegionHandle& M_Ddm_Create_Region::getRegion() const
{
    return region;
}

void M_Ddm_Create_Region::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

std::ostream& operator<<(std::ostream& os, const M_Ddm_Create_Region& msg)
{
    os << "[M_Ddm_Create_Region - Begin]" << std::endl;
    
    os << static_cast<const M_Ddm_Create_Region::Super&>(msg); // show parent class
    
    // Specific display
    os << "  space = " << msg.space << std::endl;
    os << "  extentSet [] =" << std::endl;
    for (const auto& element : msg.extentSet) {
        os << "// TODO field <extentSet> of type <Extent>";
        (void) element;
    }
    os << std::endl;
    os << "  region = " << msg.region << std::endl;
    
    os << "[M_Ddm_Create_Region - End]" << std::endl;
    return os;
}

M_Ddm_Modify_Region::M_Ddm_Modify_Region()
{
    this->messageName = "M_Ddm_Modify_Region";
    this->type = Message::DDM_MODIFY_REGION;
}

void M_Ddm_Modify_Region::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(region);
    uint32_t extentSetSize = extentSet.size();
    msgBuffer.write_uint32(extentSetSize);
    for (uint32_t i = 0; i < extentSetSize; ++i) {
        // FIXME FIXME FIXME
        //don't know how to serialize native field <extentSet> of type <Extent>
        //probably no 'representation' given
    }
}

void M_Ddm_Modify_Region::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
    uint32_t extentSetSize = msgBuffer.read_uint32();
    extentSet.resize(extentSetSize);
    for (uint32_t i = 0; i < extentSetSize; ++i) {
        // FIXME FIXME FIXME
        //don't know how to deserialize native field <extentSet> of type <Extent>
        //probably no 'representation' given
    }
}

const RegionHandle& M_Ddm_Modify_Region::getRegion() const
{
    return region;
}

void M_Ddm_Modify_Region::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

uint32_t M_Ddm_Modify_Region::getExtentSetSize() const
{
    return extentSet.size();
}

void M_Ddm_Modify_Region::setExtentSetSize(uint32_t num)
{
    extentSet.resize(num);
}

const std::vector<Extent>& M_Ddm_Modify_Region::getExtentSet() const
{
    return extentSet;
}

const Extent& M_Ddm_Modify_Region::getExtentSet(uint32_t rank) const
{
    return extentSet[rank];
}

Extent& M_Ddm_Modify_Region::getExtentSet(uint32_t rank)
{
    return extentSet[rank];
}

void M_Ddm_Modify_Region::setExtentSet(const Extent& newExtentSet, uint32_t rank)
{
    extentSet[rank] = newExtentSet;
}

void M_Ddm_Modify_Region::removeExtentSet(uint32_t rank)
{
    extentSet.erase(extentSet.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Ddm_Modify_Region& msg)
{
    os << "[M_Ddm_Modify_Region - Begin]" << std::endl;
    
    os << static_cast<const M_Ddm_Modify_Region::Super&>(msg); // show parent class
    
    // Specific display
    os << "  region = " << msg.region << std::endl;
    os << "  extentSet [] =" << std::endl;
    for (const auto& element : msg.extentSet) {
        os << "// TODO field <extentSet> of type <Extent>";
        (void) element;
    }
    os << std::endl;
    
    os << "[M_Ddm_Modify_Region - End]" << std::endl;
    return os;
}

M_Ddm_Delete_Region::M_Ddm_Delete_Region()
{
    this->messageName = "M_Ddm_Delete_Region";
    this->type = Message::DDM_DELETE_REGION;
}

void M_Ddm_Delete_Region::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(region);
}

void M_Ddm_Delete_Region::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
}

const RegionHandle& M_Ddm_Delete_Region::getRegion() const
{
    return region;
}

void M_Ddm_Delete_Region::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

std::ostream& operator<<(std::ostream& os, const M_Ddm_Delete_Region& msg)
{
    os << "[M_Ddm_Delete_Region - Begin]" << std::endl;
    
    os << static_cast<const M_Ddm_Delete_Region::Super&>(msg); // show parent class
    
    // Specific display
    os << "  region = " << msg.region << std::endl;
    
    os << "[M_Ddm_Delete_Region - End]" << std::endl;
    return os;
}

M_Ddm_Register_Object::M_Ddm_Register_Object()
{
    this->messageName = "M_Ddm_Register_Object";
    this->type = Message::DDM_REGISTER_OBJECT;
}

void M_Ddm_Register_Object::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_uint32(object);
    msgBuffer.write_bool(_hasObjectInstanceName);
    if (_hasObjectInstanceName) {
        msgBuffer.write_string(objectInstanceName);
    }
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
}

void M_Ddm_Register_Object::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    _hasObjectInstanceName = msgBuffer.read_bool();
    if (_hasObjectInstanceName) {
        msgBuffer.read_string(objectInstanceName);
    }
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
}

const ObjectClassHandle& M_Ddm_Register_Object::getObjectClass() const
{
    return objectClass;
}

void M_Ddm_Register_Object::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const ObjectHandle& M_Ddm_Register_Object::getObject() const
{
    return object;
}

void M_Ddm_Register_Object::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const std::string& M_Ddm_Register_Object::getObjectInstanceName() const
{
    return objectInstanceName;
}

void M_Ddm_Register_Object::setObjectInstanceName(const std::string& newObjectInstanceName)
{
    _hasObjectInstanceName = true;
    objectInstanceName = newObjectInstanceName;
}

bool M_Ddm_Register_Object::hasObjectInstanceName() const
{
    return _hasObjectInstanceName;
}

uint32_t M_Ddm_Register_Object::getAttributesSize() const
{
    return attributes.size();
}

void M_Ddm_Register_Object::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Ddm_Register_Object::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Ddm_Register_Object::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Ddm_Register_Object::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Ddm_Register_Object::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Ddm_Register_Object::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Ddm_Register_Object& msg)
{
    os << "[M_Ddm_Register_Object - Begin]" << std::endl;
    
    os << static_cast<const M_Ddm_Register_Object::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  object = " << msg.object << std::endl;
    os << "  (opt) objectInstanceName =" << msg.objectInstanceName << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Ddm_Register_Object - End]" << std::endl;
    return os;
}

M_Ddm_Associate_Region::M_Ddm_Associate_Region()
{
    this->messageName = "M_Ddm_Associate_Region";
    this->type = Message::DDM_ASSOCIATE_REGION;
}

void M_Ddm_Associate_Region::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_uint32(region);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
}

void M_Ddm_Associate_Region::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
}

const ObjectHandle& M_Ddm_Associate_Region::getObject() const
{
    return object;
}

void M_Ddm_Associate_Region::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const RegionHandle& M_Ddm_Associate_Region::getRegion() const
{
    return region;
}

void M_Ddm_Associate_Region::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

uint32_t M_Ddm_Associate_Region::getAttributesSize() const
{
    return attributes.size();
}

void M_Ddm_Associate_Region::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Ddm_Associate_Region::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Ddm_Associate_Region::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Ddm_Associate_Region::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Ddm_Associate_Region::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Ddm_Associate_Region::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Ddm_Associate_Region& msg)
{
    os << "[M_Ddm_Associate_Region - Begin]" << std::endl;
    
    os << static_cast<const M_Ddm_Associate_Region::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Ddm_Associate_Region - End]" << std::endl;
    return os;
}

M_Ddm_Unassociate_Region::M_Ddm_Unassociate_Region()
{
    this->messageName = "M_Ddm_Unassociate_Region";
    this->type = Message::DDM_UNASSOCIATE_REGION;
}

void M_Ddm_Unassociate_Region::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_uint32(region);
}

void M_Ddm_Unassociate_Region::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
}

const ObjectHandle& M_Ddm_Unassociate_Region::getObject() const
{
    return object;
}

void M_Ddm_Unassociate_Region::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const RegionHandle& M_Ddm_Unassociate_Region::getRegion() const
{
    return region;
}

void M_Ddm_Unassociate_Region::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

std::ostream& operator<<(std::ostream& os, const M_Ddm_Unassociate_Region& msg)
{
    os << "[M_Ddm_Unassociate_Region - Begin]" << std::endl;
    
    os << static_cast<const M_Ddm_Unassociate_Region::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  region = " << msg.region << std::endl;
    
    os << "[M_Ddm_Unassociate_Region - End]" << std::endl;
    return os;
}

M_Ddm_Subscribe_Attributes::M_Ddm_Subscribe_Attributes()
{
    this->messageName = "M_Ddm_Subscribe_Attributes";
    this->type = Message::DDM_SUBSCRIBE_ATTRIBUTES;
}

void M_Ddm_Subscribe_Attributes::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_uint32(region);
    msgBuffer.write_bool(passive);
    uint32_t attributesSize = attributes.size();
    msgBuffer.write_uint32(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        msgBuffer.write_uint32(attributes[i]);
    }
}

void M_Ddm_Subscribe_Attributes::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
    passive = msgBuffer.read_bool();
    uint32_t attributesSize = msgBuffer.read_uint32();
    attributes.resize(attributesSize);
    for (uint32_t i = 0; i < attributesSize; ++i) {
        attributes[i] = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    }
}

const ObjectClassHandle& M_Ddm_Subscribe_Attributes::getObjectClass() const
{
    return objectClass;
}

void M_Ddm_Subscribe_Attributes::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const RegionHandle& M_Ddm_Subscribe_Attributes::getRegion() const
{
    return region;
}

void M_Ddm_Subscribe_Attributes::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

void M_Ddm_Subscribe_Attributes::passiveOn()
{
    passive = true;
}

void M_Ddm_Subscribe_Attributes::passiveOff()
{
    passive = false;
}

bool M_Ddm_Subscribe_Attributes::isPassiveOn() const
{
    return passive;
}

uint32_t M_Ddm_Subscribe_Attributes::getAttributesSize() const
{
    return attributes.size();
}

void M_Ddm_Subscribe_Attributes::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Ddm_Subscribe_Attributes::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Ddm_Subscribe_Attributes::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Ddm_Subscribe_Attributes::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Ddm_Subscribe_Attributes::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Ddm_Subscribe_Attributes::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Ddm_Subscribe_Attributes& msg)
{
    os << "[M_Ddm_Subscribe_Attributes - Begin]" << std::endl;
    
    os << static_cast<const M_Ddm_Subscribe_Attributes::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  passive = " << msg.passive << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Ddm_Subscribe_Attributes - End]" << std::endl;
    return os;
}

M_Ddm_Unsubscribe_Attributes::M_Ddm_Unsubscribe_Attributes()
{
    this->messageName = "M_Ddm_Unsubscribe_Attributes";
    this->type = Message::DDM_UNSUBSCRIBE_ATTRIBUTES;
}

void M_Ddm_Unsubscribe_Attributes::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_uint32(region);
}

void M_Ddm_Unsubscribe_Attributes::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
}

const ObjectClassHandle& M_Ddm_Unsubscribe_Attributes::getObjectClass() const
{
    return objectClass;
}

void M_Ddm_Unsubscribe_Attributes::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const RegionHandle& M_Ddm_Unsubscribe_Attributes::getRegion() const
{
    return region;
}

void M_Ddm_Unsubscribe_Attributes::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

std::ostream& operator<<(std::ostream& os, const M_Ddm_Unsubscribe_Attributes& msg)
{
    os << "[M_Ddm_Unsubscribe_Attributes - Begin]" << std::endl;
    
    os << static_cast<const M_Ddm_Unsubscribe_Attributes::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  region = " << msg.region << std::endl;
    
    os << "[M_Ddm_Unsubscribe_Attributes - End]" << std::endl;
    return os;
}

M_Ddm_Subscribe_Interaction::M_Ddm_Subscribe_Interaction()
{
    this->messageName = "M_Ddm_Subscribe_Interaction";
    this->type = Message::DDM_SUBSCRIBE_INTERACTION;
}

void M_Ddm_Subscribe_Interaction::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    msgBuffer.write_uint32(region);
    msgBuffer.write_bool(passive);
}

void M_Ddm_Subscribe_Interaction::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
    passive = msgBuffer.read_bool();
}

const InteractionClassHandle& M_Ddm_Subscribe_Interaction::getInteractionClass() const
{
    return interactionClass;
}

void M_Ddm_Subscribe_Interaction::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

const RegionHandle& M_Ddm_Subscribe_Interaction::getRegion() const
{
    return region;
}

void M_Ddm_Subscribe_Interaction::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

void M_Ddm_Subscribe_Interaction::passiveOn()
{
    passive = true;
}

void M_Ddm_Subscribe_Interaction::passiveOff()
{
    passive = false;
}

bool M_Ddm_Subscribe_Interaction::isPassiveOn() const
{
    return passive;
}

std::ostream& operator<<(std::ostream& os, const M_Ddm_Subscribe_Interaction& msg)
{
    os << "[M_Ddm_Subscribe_Interaction - Begin]" << std::endl;
    
    os << static_cast<const M_Ddm_Subscribe_Interaction::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  passive = " << msg.passive << std::endl;
    
    os << "[M_Ddm_Subscribe_Interaction - End]" << std::endl;
    return os;
}

M_Ddm_Unsubscribe_Interaction::M_Ddm_Unsubscribe_Interaction()
{
    this->messageName = "M_Ddm_Unsubscribe_Interaction";
    this->type = Message::DDM_UNSUBSCRIBE_INTERACTION;
}

void M_Ddm_Unsubscribe_Interaction::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    msgBuffer.write_uint32(region);
    msgBuffer.write_bool(passive);
}

void M_Ddm_Unsubscribe_Interaction::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    region = static_cast<RegionHandle>(msgBuffer.read_uint32());
    passive = msgBuffer.read_bool();
}

const InteractionClassHandle& M_Ddm_Unsubscribe_Interaction::getInteractionClass() const
{
    return interactionClass;
}

void M_Ddm_Unsubscribe_Interaction::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

const RegionHandle& M_Ddm_Unsubscribe_Interaction::getRegion() const
{
    return region;
}

void M_Ddm_Unsubscribe_Interaction::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

void M_Ddm_Unsubscribe_Interaction::passiveOn()
{
    passive = true;
}

void M_Ddm_Unsubscribe_Interaction::passiveOff()
{
    passive = false;
}

bool M_Ddm_Unsubscribe_Interaction::isPassiveOn() const
{
    return passive;
}

std::ostream& operator<<(std::ostream& os, const M_Ddm_Unsubscribe_Interaction& msg)
{
    os << "[M_Ddm_Unsubscribe_Interaction - Begin]" << std::endl;
    
    os << static_cast<const M_Ddm_Unsubscribe_Interaction::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  passive = " << msg.passive << std::endl;
    
    os << "[M_Ddm_Unsubscribe_Interaction - End]" << std::endl;
    return os;
}

M_Ddm_Request_Update::M_Ddm_Request_Update()
{
    this->messageName = "M_Ddm_Request_Update";
    this->type = Message::DDM_REQUEST_UPDATE;
}

void M_Ddm_Request_Update::serialize(libhla::MessageBuffer& msgBuffer)
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
}

void M_Ddm_Request_Update::deserialize(libhla::MessageBuffer& msgBuffer)
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
}

const ObjectClassHandle& M_Ddm_Request_Update::getObjectClass() const
{
    return objectClass;
}

void M_Ddm_Request_Update::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const RegionHandle& M_Ddm_Request_Update::getRegion() const
{
    return region;
}

void M_Ddm_Request_Update::setRegion(const RegionHandle& newRegion)
{
    region = newRegion;
}

uint32_t M_Ddm_Request_Update::getAttributesSize() const
{
    return attributes.size();
}

void M_Ddm_Request_Update::setAttributesSize(uint32_t num)
{
    attributes.resize(num);
}

const std::vector<AttributeHandle>& M_Ddm_Request_Update::getAttributes() const
{
    return attributes;
}

const AttributeHandle& M_Ddm_Request_Update::getAttributes(uint32_t rank) const
{
    return attributes[rank];
}

AttributeHandle& M_Ddm_Request_Update::getAttributes(uint32_t rank)
{
    return attributes[rank];
}

void M_Ddm_Request_Update::setAttributes(const AttributeHandle& newAttributes, uint32_t rank)
{
    attributes[rank] = newAttributes;
}

void M_Ddm_Request_Update::removeAttributes(uint32_t rank)
{
    attributes.erase(attributes.begin() + rank);
}

std::ostream& operator<<(std::ostream& os, const M_Ddm_Request_Update& msg)
{
    os << "[M_Ddm_Request_Update - Begin]" << std::endl;
    
    os << static_cast<const M_Ddm_Request_Update::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  region = " << msg.region << std::endl;
    os << "  attributes [] =" << std::endl;
    for (const auto& element : msg.attributes) {
        os << element;
    }
    os << std::endl;
    
    os << "[M_Ddm_Request_Update - End]" << std::endl;
    return os;
}

M_Get_Object_Class_Handle::M_Get_Object_Class_Handle()
{
    this->messageName = "M_Get_Object_Class_Handle";
    this->type = Message::GET_OBJECT_CLASS_HANDLE;
}

void M_Get_Object_Class_Handle::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_string(className);
}

void M_Get_Object_Class_Handle::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(className);
}

const ObjectClassHandle& M_Get_Object_Class_Handle::getObjectClass() const
{
    return objectClass;
}

void M_Get_Object_Class_Handle::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const std::string& M_Get_Object_Class_Handle::getClassName() const
{
    return className;
}

void M_Get_Object_Class_Handle::setClassName(const std::string& newClassName)
{
    className = newClassName;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Object_Class_Handle& msg)
{
    os << "[M_Get_Object_Class_Handle - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Object_Class_Handle::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  className = " << msg.className << std::endl;
    
    os << "[M_Get_Object_Class_Handle - End]" << std::endl;
    return os;
}

M_Get_Object_Class_Name::M_Get_Object_Class_Name()
{
    this->messageName = "M_Get_Object_Class_Name";
    this->type = Message::GET_OBJECT_CLASS_NAME;
}

void M_Get_Object_Class_Name::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_string(className);
}

void M_Get_Object_Class_Name::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(className);
}

const ObjectClassHandle& M_Get_Object_Class_Name::getObjectClass() const
{
    return objectClass;
}

void M_Get_Object_Class_Name::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const std::string& M_Get_Object_Class_Name::getClassName() const
{
    return className;
}

void M_Get_Object_Class_Name::setClassName(const std::string& newClassName)
{
    className = newClassName;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Object_Class_Name& msg)
{
    os << "[M_Get_Object_Class_Name - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Object_Class_Name::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  className = " << msg.className << std::endl;
    
    os << "[M_Get_Object_Class_Name - End]" << std::endl;
    return os;
}

M_Get_Attribute_Handle::M_Get_Attribute_Handle()
{
    this->messageName = "M_Get_Attribute_Handle";
    this->type = Message::GET_ATTRIBUTE_HANDLE;
}

void M_Get_Attribute_Handle::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_string(attributeName);
    msgBuffer.write_uint32(attribute);
}

void M_Get_Attribute_Handle::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(attributeName);
    attribute = static_cast<AttributeHandle>(msgBuffer.read_uint32());
}

const ObjectClassHandle& M_Get_Attribute_Handle::getObjectClass() const
{
    return objectClass;
}

void M_Get_Attribute_Handle::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const std::string& M_Get_Attribute_Handle::getAttributeName() const
{
    return attributeName;
}

void M_Get_Attribute_Handle::setAttributeName(const std::string& newAttributeName)
{
    attributeName = newAttributeName;
}

const AttributeHandle& M_Get_Attribute_Handle::getAttribute() const
{
    return attribute;
}

void M_Get_Attribute_Handle::setAttribute(const AttributeHandle& newAttribute)
{
    attribute = newAttribute;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Attribute_Handle& msg)
{
    os << "[M_Get_Attribute_Handle - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Attribute_Handle::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  attributeName = " << msg.attributeName << std::endl;
    os << "  attribute = " << msg.attribute << std::endl;
    
    os << "[M_Get_Attribute_Handle - End]" << std::endl;
    return os;
}

M_Get_Attribute_Name::M_Get_Attribute_Name()
{
    this->messageName = "M_Get_Attribute_Name";
    this->type = Message::GET_ATTRIBUTE_NAME;
}

void M_Get_Attribute_Name::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_string(attributeName);
    msgBuffer.write_uint32(attribute);
}

void M_Get_Attribute_Name::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(attributeName);
    attribute = static_cast<AttributeHandle>(msgBuffer.read_uint32());
}

const ObjectClassHandle& M_Get_Attribute_Name::getObjectClass() const
{
    return objectClass;
}

void M_Get_Attribute_Name::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const std::string& M_Get_Attribute_Name::getAttributeName() const
{
    return attributeName;
}

void M_Get_Attribute_Name::setAttributeName(const std::string& newAttributeName)
{
    attributeName = newAttributeName;
}

const AttributeHandle& M_Get_Attribute_Name::getAttribute() const
{
    return attribute;
}

void M_Get_Attribute_Name::setAttribute(const AttributeHandle& newAttribute)
{
    attribute = newAttribute;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Attribute_Name& msg)
{
    os << "[M_Get_Attribute_Name - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Attribute_Name::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  attributeName = " << msg.attributeName << std::endl;
    os << "  attribute = " << msg.attribute << std::endl;
    
    os << "[M_Get_Attribute_Name - End]" << std::endl;
    return os;
}

M_Get_Interaction_Class_Handle::M_Get_Interaction_Class_Handle()
{
    this->messageName = "M_Get_Interaction_Class_Handle";
    this->type = Message::GET_INTERACTION_CLASS_HANDLE;
}

void M_Get_Interaction_Class_Handle::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    msgBuffer.write_string(className);
}

void M_Get_Interaction_Class_Handle::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(className);
}

const InteractionClassHandle& M_Get_Interaction_Class_Handle::getInteractionClass() const
{
    return interactionClass;
}

void M_Get_Interaction_Class_Handle::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

const std::string& M_Get_Interaction_Class_Handle::getClassName() const
{
    return className;
}

void M_Get_Interaction_Class_Handle::setClassName(const std::string& newClassName)
{
    className = newClassName;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Interaction_Class_Handle& msg)
{
    os << "[M_Get_Interaction_Class_Handle - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Interaction_Class_Handle::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  className = " << msg.className << std::endl;
    
    os << "[M_Get_Interaction_Class_Handle - End]" << std::endl;
    return os;
}

M_Get_Interaction_Class_Name::M_Get_Interaction_Class_Name()
{
    this->messageName = "M_Get_Interaction_Class_Name";
    this->type = Message::GET_INTERACTION_CLASS_NAME;
}

void M_Get_Interaction_Class_Name::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    msgBuffer.write_string(className);
}

void M_Get_Interaction_Class_Name::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(className);
}

const InteractionClassHandle& M_Get_Interaction_Class_Name::getInteractionClass() const
{
    return interactionClass;
}

void M_Get_Interaction_Class_Name::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

const std::string& M_Get_Interaction_Class_Name::getClassName() const
{
    return className;
}

void M_Get_Interaction_Class_Name::setClassName(const std::string& newClassName)
{
    className = newClassName;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Interaction_Class_Name& msg)
{
    os << "[M_Get_Interaction_Class_Name - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Interaction_Class_Name::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  className = " << msg.className << std::endl;
    
    os << "[M_Get_Interaction_Class_Name - End]" << std::endl;
    return os;
}

M_Get_Parameter_Handle::M_Get_Parameter_Handle()
{
    this->messageName = "M_Get_Parameter_Handle";
    this->type = Message::GET_PARAMETER_HANDLE;
}

void M_Get_Parameter_Handle::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    msgBuffer.write_string(parameterName);
    msgBuffer.write_uint32(parameter);
}

void M_Get_Parameter_Handle::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(parameterName);
    parameter = static_cast<ParameterHandle>(msgBuffer.read_uint32());
}

const InteractionClassHandle& M_Get_Parameter_Handle::getInteractionClass() const
{
    return interactionClass;
}

void M_Get_Parameter_Handle::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

const std::string& M_Get_Parameter_Handle::getParameterName() const
{
    return parameterName;
}

void M_Get_Parameter_Handle::setParameterName(const std::string& newParameterName)
{
    parameterName = newParameterName;
}

const ParameterHandle& M_Get_Parameter_Handle::getParameter() const
{
    return parameter;
}

void M_Get_Parameter_Handle::setParameter(const ParameterHandle& newParameter)
{
    parameter = newParameter;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Parameter_Handle& msg)
{
    os << "[M_Get_Parameter_Handle - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Parameter_Handle::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  parameterName = " << msg.parameterName << std::endl;
    os << "  parameter = " << msg.parameter << std::endl;
    
    os << "[M_Get_Parameter_Handle - End]" << std::endl;
    return os;
}

M_Get_Parameter_Name::M_Get_Parameter_Name()
{
    this->messageName = "M_Get_Parameter_Name";
    this->type = Message::GET_PARAMETER_NAME;
}

void M_Get_Parameter_Name::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    msgBuffer.write_string(parameterName);
    msgBuffer.write_uint32(parameter);
}

void M_Get_Parameter_Name::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(parameterName);
    parameter = static_cast<ParameterHandle>(msgBuffer.read_uint32());
}

const InteractionClassHandle& M_Get_Parameter_Name::getInteractionClass() const
{
    return interactionClass;
}

void M_Get_Parameter_Name::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

const std::string& M_Get_Parameter_Name::getParameterName() const
{
    return parameterName;
}

void M_Get_Parameter_Name::setParameterName(const std::string& newParameterName)
{
    parameterName = newParameterName;
}

const ParameterHandle& M_Get_Parameter_Name::getParameter() const
{
    return parameter;
}

void M_Get_Parameter_Name::setParameter(const ParameterHandle& newParameter)
{
    parameter = newParameter;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Parameter_Name& msg)
{
    os << "[M_Get_Parameter_Name - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Parameter_Name::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  parameterName = " << msg.parameterName << std::endl;
    os << "  parameter = " << msg.parameter << std::endl;
    
    os << "[M_Get_Parameter_Name - End]" << std::endl;
    return os;
}

M_Get_Object_Instance_Handle::M_Get_Object_Instance_Handle()
{
    this->messageName = "M_Get_Object_Instance_Handle";
    this->type = Message::GET_OBJECT_INSTANCE_HANDLE;
}

void M_Get_Object_Instance_Handle::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_string(objectInstanceName);
}

void M_Get_Object_Instance_Handle::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(objectInstanceName);
}

const ObjectHandle& M_Get_Object_Instance_Handle::getObject() const
{
    return object;
}

void M_Get_Object_Instance_Handle::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const std::string& M_Get_Object_Instance_Handle::getObjectInstanceName() const
{
    return objectInstanceName;
}

void M_Get_Object_Instance_Handle::setObjectInstanceName(const std::string& newObjectInstanceName)
{
    objectInstanceName = newObjectInstanceName;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Object_Instance_Handle& msg)
{
    os << "[M_Get_Object_Instance_Handle - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Object_Instance_Handle::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  objectInstanceName = " << msg.objectInstanceName << std::endl;
    
    os << "[M_Get_Object_Instance_Handle - End]" << std::endl;
    return os;
}

M_Get_Object_Instance_Name::M_Get_Object_Instance_Name()
{
    this->messageName = "M_Get_Object_Instance_Name";
    this->type = Message::GET_OBJECT_INSTANCE_NAME;
}

void M_Get_Object_Instance_Name::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(object);
    msgBuffer.write_string(objectInstanceName);
}

void M_Get_Object_Instance_Name::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
    msgBuffer.read_string(objectInstanceName);
}

const ObjectHandle& M_Get_Object_Instance_Name::getObject() const
{
    return object;
}

void M_Get_Object_Instance_Name::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

const std::string& M_Get_Object_Instance_Name::getObjectInstanceName() const
{
    return objectInstanceName;
}

void M_Get_Object_Instance_Name::setObjectInstanceName(const std::string& newObjectInstanceName)
{
    objectInstanceName = newObjectInstanceName;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Object_Instance_Name& msg)
{
    os << "[M_Get_Object_Instance_Name - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Object_Instance_Name::Super&>(msg); // show parent class
    
    // Specific display
    os << "  object = " << msg.object << std::endl;
    os << "  objectInstanceName = " << msg.objectInstanceName << std::endl;
    
    os << "[M_Get_Object_Instance_Name - End]" << std::endl;
    return os;
}

M_Get_Space_Handle::M_Get_Space_Handle()
{
    this->messageName = "M_Get_Space_Handle";
    this->type = Message::GET_SPACE_HANDLE;
}

void M_Get_Space_Handle::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_string(spaceName);
    msgBuffer.write_uint32(space);
}

void M_Get_Space_Handle::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    msgBuffer.read_string(spaceName);
    space = static_cast<SpaceHandle>(msgBuffer.read_uint32());
}

const std::string& M_Get_Space_Handle::getSpaceName() const
{
    return spaceName;
}

void M_Get_Space_Handle::setSpaceName(const std::string& newSpaceName)
{
    spaceName = newSpaceName;
}

const SpaceHandle& M_Get_Space_Handle::getSpace() const
{
    return space;
}

void M_Get_Space_Handle::setSpace(const SpaceHandle& newSpace)
{
    space = newSpace;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Space_Handle& msg)
{
    os << "[M_Get_Space_Handle - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Space_Handle::Super&>(msg); // show parent class
    
    // Specific display
    os << "  spaceName = " << msg.spaceName << std::endl;
    os << "  space = " << msg.space << std::endl;
    
    os << "[M_Get_Space_Handle - End]" << std::endl;
    return os;
}

M_Get_Space_Name::M_Get_Space_Name()
{
    this->messageName = "M_Get_Space_Name";
    this->type = Message::GET_SPACE_NAME;
}

void M_Get_Space_Name::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_string(spaceName);
    msgBuffer.write_uint32(space);
}

void M_Get_Space_Name::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    msgBuffer.read_string(spaceName);
    space = static_cast<SpaceHandle>(msgBuffer.read_uint32());
}

const std::string& M_Get_Space_Name::getSpaceName() const
{
    return spaceName;
}

void M_Get_Space_Name::setSpaceName(const std::string& newSpaceName)
{
    spaceName = newSpaceName;
}

const SpaceHandle& M_Get_Space_Name::getSpace() const
{
    return space;
}

void M_Get_Space_Name::setSpace(const SpaceHandle& newSpace)
{
    space = newSpace;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Space_Name& msg)
{
    os << "[M_Get_Space_Name - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Space_Name::Super&>(msg); // show parent class
    
    // Specific display
    os << "  spaceName = " << msg.spaceName << std::endl;
    os << "  space = " << msg.space << std::endl;
    
    os << "[M_Get_Space_Name - End]" << std::endl;
    return os;
}

M_Get_Dimension_Handle::M_Get_Dimension_Handle()
{
    this->messageName = "M_Get_Dimension_Handle";
    this->type = Message::GET_DIMENSION_HANDLE;
}

void M_Get_Dimension_Handle::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_string(dimensionName);
    msgBuffer.write_uint32(space);
    msgBuffer.write_uint32(dimension);
}

void M_Get_Dimension_Handle::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    msgBuffer.read_string(dimensionName);
    space = static_cast<SpaceHandle>(msgBuffer.read_uint32());
    dimension = static_cast<DimensionHandle>(msgBuffer.read_uint32());
}

const std::string& M_Get_Dimension_Handle::getDimensionName() const
{
    return dimensionName;
}

void M_Get_Dimension_Handle::setDimensionName(const std::string& newDimensionName)
{
    dimensionName = newDimensionName;
}

const SpaceHandle& M_Get_Dimension_Handle::getSpace() const
{
    return space;
}

void M_Get_Dimension_Handle::setSpace(const SpaceHandle& newSpace)
{
    space = newSpace;
}

const DimensionHandle& M_Get_Dimension_Handle::getDimension() const
{
    return dimension;
}

void M_Get_Dimension_Handle::setDimension(const DimensionHandle& newDimension)
{
    dimension = newDimension;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Dimension_Handle& msg)
{
    os << "[M_Get_Dimension_Handle - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Dimension_Handle::Super&>(msg); // show parent class
    
    // Specific display
    os << "  dimensionName = " << msg.dimensionName << std::endl;
    os << "  space = " << msg.space << std::endl;
    os << "  dimension = " << msg.dimension << std::endl;
    
    os << "[M_Get_Dimension_Handle - End]" << std::endl;
    return os;
}

M_Get_Dimension_Name::M_Get_Dimension_Name()
{
    this->messageName = "M_Get_Dimension_Name";
    this->type = Message::GET_DIMENSION_NAME;
}

void M_Get_Dimension_Name::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_string(dimensionName);
    msgBuffer.write_uint32(space);
    msgBuffer.write_uint32(dimension);
}

void M_Get_Dimension_Name::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    msgBuffer.read_string(dimensionName);
    space = static_cast<SpaceHandle>(msgBuffer.read_uint32());
    dimension = static_cast<DimensionHandle>(msgBuffer.read_uint32());
}

const std::string& M_Get_Dimension_Name::getDimensionName() const
{
    return dimensionName;
}

void M_Get_Dimension_Name::setDimensionName(const std::string& newDimensionName)
{
    dimensionName = newDimensionName;
}

const SpaceHandle& M_Get_Dimension_Name::getSpace() const
{
    return space;
}

void M_Get_Dimension_Name::setSpace(const SpaceHandle& newSpace)
{
    space = newSpace;
}

const DimensionHandle& M_Get_Dimension_Name::getDimension() const
{
    return dimension;
}

void M_Get_Dimension_Name::setDimension(const DimensionHandle& newDimension)
{
    dimension = newDimension;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Dimension_Name& msg)
{
    os << "[M_Get_Dimension_Name - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Dimension_Name::Super&>(msg); // show parent class
    
    // Specific display
    os << "  dimensionName = " << msg.dimensionName << std::endl;
    os << "  space = " << msg.space << std::endl;
    os << "  dimension = " << msg.dimension << std::endl;
    
    os << "[M_Get_Dimension_Name - End]" << std::endl;
    return os;
}

M_Get_Attribute_Space_Handle::M_Get_Attribute_Space_Handle()
{
    this->messageName = "M_Get_Attribute_Space_Handle";
    this->type = Message::GET_ATTRIBUTE_SPACE_HANDLE;
}

void M_Get_Attribute_Space_Handle::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_uint32(attribute);
    msgBuffer.write_uint32(space);
}

void M_Get_Attribute_Space_Handle::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    attribute = static_cast<AttributeHandle>(msgBuffer.read_uint32());
    space = static_cast<SpaceHandle>(msgBuffer.read_uint32());
}

const ObjectClassHandle& M_Get_Attribute_Space_Handle::getObjectClass() const
{
    return objectClass;
}

void M_Get_Attribute_Space_Handle::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const AttributeHandle& M_Get_Attribute_Space_Handle::getAttribute() const
{
    return attribute;
}

void M_Get_Attribute_Space_Handle::setAttribute(const AttributeHandle& newAttribute)
{
    attribute = newAttribute;
}

const SpaceHandle& M_Get_Attribute_Space_Handle::getSpace() const
{
    return space;
}

void M_Get_Attribute_Space_Handle::setSpace(const SpaceHandle& newSpace)
{
    space = newSpace;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Attribute_Space_Handle& msg)
{
    os << "[M_Get_Attribute_Space_Handle - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Attribute_Space_Handle::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  attribute = " << msg.attribute << std::endl;
    os << "  space = " << msg.space << std::endl;
    
    os << "[M_Get_Attribute_Space_Handle - End]" << std::endl;
    return os;
}

M_Get_Object_Class::M_Get_Object_Class()
{
    this->messageName = "M_Get_Object_Class";
    this->type = Message::GET_OBJECT_CLASS;
}

void M_Get_Object_Class::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(objectClass);
    msgBuffer.write_uint32(object);
}

void M_Get_Object_Class::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    objectClass = static_cast<ObjectClassHandle>(msgBuffer.read_uint32());
    object = static_cast<ObjectHandle>(msgBuffer.read_uint32());
}

const ObjectClassHandle& M_Get_Object_Class::getObjectClass() const
{
    return objectClass;
}

void M_Get_Object_Class::setObjectClass(const ObjectClassHandle& newObjectClass)
{
    objectClass = newObjectClass;
}

const ObjectHandle& M_Get_Object_Class::getObject() const
{
    return object;
}

void M_Get_Object_Class::setObject(const ObjectHandle& newObject)
{
    object = newObject;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Object_Class& msg)
{
    os << "[M_Get_Object_Class - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Object_Class::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectClass = " << msg.objectClass << std::endl;
    os << "  object = " << msg.object << std::endl;
    
    os << "[M_Get_Object_Class - End]" << std::endl;
    return os;
}

M_Get_Interaction_Space_Handle::M_Get_Interaction_Space_Handle()
{
    this->messageName = "M_Get_Interaction_Space_Handle";
    this->type = Message::GET_INTERACTION_SPACE_HANDLE;
}

void M_Get_Interaction_Space_Handle::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_uint32(interactionClass);
    msgBuffer.write_uint32(space);
}

void M_Get_Interaction_Space_Handle::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    interactionClass = static_cast<InteractionClassHandle>(msgBuffer.read_uint32());
    space = static_cast<SpaceHandle>(msgBuffer.read_uint32());
}

const InteractionClassHandle& M_Get_Interaction_Space_Handle::getInteractionClass() const
{
    return interactionClass;
}

void M_Get_Interaction_Space_Handle::setInteractionClass(const InteractionClassHandle& newInteractionClass)
{
    interactionClass = newInteractionClass;
}

const SpaceHandle& M_Get_Interaction_Space_Handle::getSpace() const
{
    return space;
}

void M_Get_Interaction_Space_Handle::setSpace(const SpaceHandle& newSpace)
{
    space = newSpace;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Interaction_Space_Handle& msg)
{
    os << "[M_Get_Interaction_Space_Handle - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Interaction_Space_Handle::Super&>(msg); // show parent class
    
    // Specific display
    os << "  interactionClass = " << msg.interactionClass << std::endl;
    os << "  space = " << msg.space << std::endl;
    
    os << "[M_Get_Interaction_Space_Handle - End]" << std::endl;
    return os;
}

M_Get_Transportation_Handle::M_Get_Transportation_Handle()
{
    this->messageName = "M_Get_Transportation_Handle";
    this->type = Message::GET_TRANSPORTATION_HANDLE;
}

void M_Get_Transportation_Handle::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_string(transportationName);
    msgBuffer.write_uint8(transportation);
}

void M_Get_Transportation_Handle::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    msgBuffer.read_string(transportationName);
    transportation = static_cast<TransportType>(msgBuffer.read_uint8());
}

const std::string& M_Get_Transportation_Handle::getTransportationName() const
{
    return transportationName;
}

void M_Get_Transportation_Handle::setTransportationName(const std::string& newTransportationName)
{
    transportationName = newTransportationName;
}

const TransportType& M_Get_Transportation_Handle::getTransportation() const
{
    return transportation;
}

void M_Get_Transportation_Handle::setTransportation(const TransportType& newTransportation)
{
    transportation = newTransportation;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Transportation_Handle& msg)
{
    os << "[M_Get_Transportation_Handle - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Transportation_Handle::Super&>(msg); // show parent class
    
    // Specific display
    os << "  transportationName = " << msg.transportationName << std::endl;
    os << "  transportation = " << msg.transportation << std::endl;
    
    os << "[M_Get_Transportation_Handle - End]" << std::endl;
    return os;
}

M_Get_Transportation_Name::M_Get_Transportation_Name()
{
    this->messageName = "M_Get_Transportation_Name";
    this->type = Message::GET_TRANSPORTATION_NAME;
}

void M_Get_Transportation_Name::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_string(transportationName);
    msgBuffer.write_uint8(transportation);
}

void M_Get_Transportation_Name::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    msgBuffer.read_string(transportationName);
    transportation = static_cast<TransportType>(msgBuffer.read_uint8());
}

const std::string& M_Get_Transportation_Name::getTransportationName() const
{
    return transportationName;
}

void M_Get_Transportation_Name::setTransportationName(const std::string& newTransportationName)
{
    transportationName = newTransportationName;
}

const TransportType& M_Get_Transportation_Name::getTransportation() const
{
    return transportation;
}

void M_Get_Transportation_Name::setTransportation(const TransportType& newTransportation)
{
    transportation = newTransportation;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Transportation_Name& msg)
{
    os << "[M_Get_Transportation_Name - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Transportation_Name::Super&>(msg); // show parent class
    
    // Specific display
    os << "  transportationName = " << msg.transportationName << std::endl;
    os << "  transportation = " << msg.transportation << std::endl;
    
    os << "[M_Get_Transportation_Name - End]" << std::endl;
    return os;
}

M_Get_Ordering_Handle::M_Get_Ordering_Handle()
{
    this->messageName = "M_Get_Ordering_Handle";
    this->type = Message::GET_ORDERING_HANDLE;
}

void M_Get_Ordering_Handle::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_string(orderingName);
    msgBuffer.write_uint8(ordering);
}

void M_Get_Ordering_Handle::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    msgBuffer.read_string(orderingName);
    ordering = static_cast<OrderType>(msgBuffer.read_uint8());
}

const std::string& M_Get_Ordering_Handle::getOrderingName() const
{
    return orderingName;
}

void M_Get_Ordering_Handle::setOrderingName(const std::string& newOrderingName)
{
    orderingName = newOrderingName;
}

const OrderType& M_Get_Ordering_Handle::getOrdering() const
{
    return ordering;
}

void M_Get_Ordering_Handle::setOrdering(const OrderType& newOrdering)
{
    ordering = newOrdering;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Ordering_Handle& msg)
{
    os << "[M_Get_Ordering_Handle - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Ordering_Handle::Super&>(msg); // show parent class
    
    // Specific display
    os << "  orderingName = " << msg.orderingName << std::endl;
    os << "  ordering = " << msg.ordering << std::endl;
    
    os << "[M_Get_Ordering_Handle - End]" << std::endl;
    return os;
}

M_Get_Ordering_Name::M_Get_Ordering_Name()
{
    this->messageName = "M_Get_Ordering_Name";
    this->type = Message::GET_ORDERING_NAME;
}

void M_Get_Ordering_Name::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_string(orderingName);
    msgBuffer.write_uint8(ordering);
}

void M_Get_Ordering_Name::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    msgBuffer.read_string(orderingName);
    ordering = static_cast<OrderType>(msgBuffer.read_uint8());
}

const std::string& M_Get_Ordering_Name::getOrderingName() const
{
    return orderingName;
}

void M_Get_Ordering_Name::setOrderingName(const std::string& newOrderingName)
{
    orderingName = newOrderingName;
}

const OrderType& M_Get_Ordering_Name::getOrdering() const
{
    return ordering;
}

void M_Get_Ordering_Name::setOrdering(const OrderType& newOrdering)
{
    ordering = newOrdering;
}

std::ostream& operator<<(std::ostream& os, const M_Get_Ordering_Name& msg)
{
    os << "[M_Get_Ordering_Name - Begin]" << std::endl;
    
    os << static_cast<const M_Get_Ordering_Name::Super&>(msg); // show parent class
    
    // Specific display
    os << "  orderingName = " << msg.orderingName << std::endl;
    os << "  ordering = " << msg.ordering << std::endl;
    
    os << "[M_Get_Ordering_Name - End]" << std::endl;
    return os;
}

M_Enable_Class_Relevance_Advisory_Switch::M_Enable_Class_Relevance_Advisory_Switch()
{
    this->messageName = "M_Enable_Class_Relevance_Advisory_Switch";
    this->type = Message::ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH;
}

M_Disable_Class_Relevance_Advisory_Switch::M_Disable_Class_Relevance_Advisory_Switch()
{
    this->messageName = "M_Disable_Class_Relevance_Advisory_Switch";
    this->type = Message::DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH;
}

M_Enable_Attribute_Relevance_Advisory_Switch::M_Enable_Attribute_Relevance_Advisory_Switch()
{
    this->messageName = "M_Enable_Attribute_Relevance_Advisory_Switch";
    this->type = Message::ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH;
}

M_Disable_Attribute_Relevance_Advisory_Switch::M_Disable_Attribute_Relevance_Advisory_Switch()
{
    this->messageName = "M_Disable_Attribute_Relevance_Advisory_Switch";
    this->type = Message::DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH;
}

M_Enable_Attribute_Scope_Advisory_Switch::M_Enable_Attribute_Scope_Advisory_Switch()
{
    this->messageName = "M_Enable_Attribute_Scope_Advisory_Switch";
    this->type = Message::ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH;
}

M_Disable_Attribute_Scope_Advisory_Switch::M_Disable_Attribute_Scope_Advisory_Switch()
{
    this->messageName = "M_Disable_Attribute_Scope_Advisory_Switch";
    this->type = Message::DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH;
}

M_Enable_Interaction_Relevance_Advisory_Switch::M_Enable_Interaction_Relevance_Advisory_Switch()
{
    this->messageName = "M_Enable_Interaction_Relevance_Advisory_Switch";
    this->type = Message::ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH;
}

M_Disable_Interaction_Relevance_Advisory_Switch::M_Disable_Interaction_Relevance_Advisory_Switch()
{
    this->messageName = "M_Disable_Interaction_Relevance_Advisory_Switch";
    this->type = Message::DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH;
}

M_Tick_Request::M_Tick_Request()
{
    this->messageName = "M_Tick_Request";
    this->type = Message::TICK_REQUEST;
}

void M_Tick_Request::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_bool(multiple);
    msgBuffer.write_double(minTickTime);
    msgBuffer.write_double(maxTickTime);
}

void M_Tick_Request::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    multiple = msgBuffer.read_bool();
    minTickTime = msgBuffer.read_double();
    maxTickTime = msgBuffer.read_double();
}

const bool& M_Tick_Request::getMultiple() const
{
    return multiple;
}

void M_Tick_Request::setMultiple(const bool& newMultiple)
{
    multiple = newMultiple;
}

const double& M_Tick_Request::getMinTickTime() const
{
    return minTickTime;
}

void M_Tick_Request::setMinTickTime(const double& newMinTickTime)
{
    minTickTime = newMinTickTime;
}

const double& M_Tick_Request::getMaxTickTime() const
{
    return maxTickTime;
}

void M_Tick_Request::setMaxTickTime(const double& newMaxTickTime)
{
    maxTickTime = newMaxTickTime;
}

std::ostream& operator<<(std::ostream& os, const M_Tick_Request& msg)
{
    os << "[M_Tick_Request - Begin]" << std::endl;
    
    os << static_cast<const M_Tick_Request::Super&>(msg); // show parent class
    
    // Specific display
    os << "  multiple = " << msg.multiple << std::endl;
    os << "  minTickTime = " << msg.minTickTime << std::endl;
    os << "  maxTickTime = " << msg.maxTickTime << std::endl;
    
    os << "[M_Tick_Request - End]" << std::endl;
    return os;
}

M_Tick_Request_Next::M_Tick_Request_Next()
{
    this->messageName = "M_Tick_Request_Next";
    this->type = Message::TICK_REQUEST_NEXT;
}

M_Tick_Request_Stop::M_Tick_Request_Stop()
{
    this->messageName = "M_Tick_Request_Stop";
    this->type = Message::TICK_REQUEST_STOP;
}

M_Reserve_Object_Instance_Name::M_Reserve_Object_Instance_Name()
{
    this->messageName = "M_Reserve_Object_Instance_Name";
    this->type = Message::RESERVE_OBJECT_INSTANCE_NAME;
}

void M_Reserve_Object_Instance_Name::serialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::serialize(msgBuffer);
    // Specific serialization code
    msgBuffer.write_string(objectName);
}

void M_Reserve_Object_Instance_Name::deserialize(libhla::MessageBuffer& msgBuffer)
{
    // Call parent class
    Super::deserialize(msgBuffer);
    // Specific deserialization code
    msgBuffer.read_string(objectName);
}

const std::string& M_Reserve_Object_Instance_Name::getObjectName() const
{
    return objectName;
}

void M_Reserve_Object_Instance_Name::setObjectName(const std::string& newObjectName)
{
    objectName = newObjectName;
}

std::ostream& operator<<(std::ostream& os, const M_Reserve_Object_Instance_Name& msg)
{
    os << "[M_Reserve_Object_Instance_Name - Begin]" << std::endl;
    
    os << static_cast<const M_Reserve_Object_Instance_Name::Super&>(msg); // show parent class
    
    // Specific display
    os << "  objectName = " << msg.objectName << std::endl;
    
    os << "[M_Reserve_Object_Instance_Name - End]" << std::endl;
    return os;
}

M_Reserve_Object_Instance_Name_Succeeded::M_Reserve_Object_Instance_Name_Succeeded()
{
    this->messageName = "M_Reserve_Object_Instance_Name_Succeeded";
    this->type = Message::RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED;
}

M_Reserve_Object_Instance_Name_Failed::M_Reserve_Object_Instance_Name_Failed()
{
    this->messageName = "M_Reserve_Object_Instance_Name_Failed";
    this->type = Message::RESERVE_OBJECT_INSTANCE_NAME_FAILED;
}

Message* M_Factory::create(M_Type type) throw (NetworkError ,NetworkSignal) { 
    Message* msg = NULL;

    switch (type) {
        case Message::Type::NOT_USED:
            throw NetworkError("NOT_USED message type should not be used!!");
            break;
        case Message::Type::OPEN_CONNEXION:
            msg = new M_Open_Connexion();
            break;
        case Message::Type::CLOSE_CONNEXION:
            msg = new M_Close_Connexion();
            break;
        case Message::Type::CREATE_FEDERATION_EXECUTION:
            msg = new M_Create_Federation_Execution();
            break;
        case Message::Type::CREATE_FEDERATION_EXECUTION_V4:
            msg = new M_Create_Federation_Execution_V4();
            break;
        case Message::Type::DESTROY_FEDERATION_EXECUTION:
            msg = new M_Destroy_Federation_Execution();
            break;
        case Message::Type::JOIN_FEDERATION_EXECUTION:
            msg = new M_Join_Federation_Execution();
            break;
        case Message::Type::JOIN_FEDERATION_EXECUTION_V4:
            msg = new M_Join_Federation_Execution_V4();
            break;
        case Message::Type::RESIGN_FEDERATION_EXECUTION:
            msg = new M_Resign_Federation_Execution();
            break;
        case Message::Type::REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
            msg = new M_Register_Federation_Synchronization_Point();
            break;
        case Message::Type::SYNCHRONIZATION_POINT_REGISTRATION_FAILED:
            msg = new M_Synchronization_Point_Registration_Failed();
            break;
        case Message::Type::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
            msg = new M_Synchronization_Point_Registration_Succeeded();
            break;
        case Message::Type::ANNOUNCE_SYNCHRONIZATION_POINT:
            msg = new M_Announce_Synchronization_Point();
            break;
        case Message::Type::SYNCHRONIZATION_POINT_ACHIEVED:
            msg = new M_Synchronization_Point_Achieved();
            break;
        case Message::Type::FEDERATION_SYNCHRONIZED:
            msg = new M_Federation_Synchronized();
            break;
        case Message::Type::REQUEST_FEDERATION_SAVE:
            msg = new M_Request_Federation_Save();
            break;
        case Message::Type::INITIATE_FEDERATE_SAVE:
            msg = new M_Initiate_Federate_Save();
            break;
        case Message::Type::FEDERATE_SAVE_BEGUN:
            msg = new M_Federate_Save_Begun();
            break;
        case Message::Type::FEDERATE_SAVE_COMPLETE:
            msg = new M_Federate_Save_Complete();
            break;
        case Message::Type::FEDERATE_SAVE_NOT_COMPLETE:
            msg = new M_Federate_Save_Not_Complete();
            break;
        case Message::Type::FEDERATION_SAVED:
            msg = new M_Federation_Saved();
            break;
        case Message::Type::FEDERATION_NOT_SAVED:
            msg = new M_Federation_Not_Saved();
            break;
        case Message::Type::REQUEST_FEDERATION_RESTORE:
            msg = new M_Request_Federation_Restore();
            break;
        case Message::Type::REQUEST_FEDERATION_RESTORE_FAILED:
            msg = new M_Request_Federation_Restore_Failed();
            break;
        case Message::Type::REQUEST_FEDERATION_RESTORE_SUCCEEDED:
            msg = new M_Request_Federation_Restore_Succeeded();
            break;
        case Message::Type::INITIATE_FEDERATE_RESTORE:
            msg = new M_Initiate_Federate_Restore();
            break;
        case Message::Type::FEDERATE_RESTORE_COMPLETE:
            msg = new M_Federate_Restore_Complete();
            break;
        case Message::Type::FEDERATE_RESTORE_NOT_COMPLETE:
            msg = new M_Federate_Restore_Not_Complete();
            break;
        case Message::Type::FEDERATION_RESTORED:
            msg = new M_Federation_Restored();
            break;
        case Message::Type::FEDERATION_NOT_RESTORED:
            msg = new M_Federation_Not_Restored();
            break;
        case Message::Type::FEDERATION_RESTORE_BEGUN:
            msg = new M_Federation_Restore_Begun();
            break;
        case Message::Type::PUBLISH_OBJECT_CLASS:
            msg = new M_Publish_Object_Class();
            break;
        case Message::Type::UNPUBLISH_OBJECT_CLASS:
            msg = new M_Unpublish_Object_Class();
            break;
        case Message::Type::PUBLISH_INTERACTION_CLASS:
            msg = new M_Publish_Interaction_Class();
            break;
        case Message::Type::UNPUBLISH_INTERACTION_CLASS:
            msg = new M_Unpublish_Interaction_Class();
            break;
        case Message::Type::SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES:
            msg = new M_Subscribe_Object_Class_Attributes();
            break;
        case Message::Type::UNSUBSCRIBE_OBJECT_CLASS:
            msg = new M_Unsubscribe_Object_Class();
            break;
        case Message::Type::SUBSCRIBE_INTERACTION_CLASS:
            msg = new M_Subscribe_Interaction_Class();
            break;
        case Message::Type::UNSUBSCRIBE_INTERACTION_CLASS:
            msg = new M_Unsubscribe_Interaction_Class();
            break;
        case Message::Type::START_REGISTRATION_FOR_OBJECT_CLASS:
            msg = new M_Start_Registration_For_Object_Class();
            break;
        case Message::Type::STOP_REGISTRATION_FOR_OBJECT_CLASS:
            msg = new M_Stop_Registration_For_Object_Class();
            break;
        case Message::Type::TURN_INTERACTIONS_ON:
            msg = new M_Turn_Interactions_On();
            break;
        case Message::Type::TURN_INTERACTIONS_OFF:
            msg = new M_Turn_Interactions_Off();
            break;
        case Message::Type::REGISTER_OBJECT_INSTANCE:
            msg = new M_Register_Object_Instance();
            break;
        case Message::Type::UPDATE_ATTRIBUTE_VALUES:
            msg = new M_Update_Attribute_Values();
            break;
        case Message::Type::DISCOVER_OBJECT_INSTANCE:
            msg = new M_Discover_Object_Instance();
            break;
        case Message::Type::REFLECT_ATTRIBUTE_VALUES:
            msg = new M_Reflect_Attribute_Values();
            break;
        case Message::Type::SEND_INTERACTION:
            msg = new M_Send_Interaction();
            break;
        case Message::Type::RECEIVE_INTERACTION:
            msg = new M_Receive_Interaction();
            break;
        case Message::Type::DELETE_OBJECT_INSTANCE:
            msg = new M_Delete_Object_Instance();
            break;
        case Message::Type::LOCAL_DELETE_OBJECT_INSTANCE:
            msg = new M_Local_Delete_Object_Instance();
            break;
        case Message::Type::REMOVE_OBJECT_INSTANCE:
            msg = new M_Remove_Object_Instance();
            break;
        case Message::Type::CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE:
            msg = new M_Change_Attribute_Transportation_Type();
            break;
        case Message::Type::CHANGE_INTERACTION_TRANSPORTATION_TYPE:
            msg = new M_Change_Interaction_Transportation_Type();
            break;
        case Message::Type::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
            msg = new M_Request_Object_Attribute_Value_Update();
            break;
        case Message::Type::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE:
            msg = new M_Request_Class_Attribute_Value_Update();
            break;
        case Message::Type::PROVIDE_ATTRIBUTE_VALUE_UPDATE:
            msg = new M_Provide_Attribute_Value_Update();
            break;
        case Message::Type::ATTRIBUTES_IN_SCOPE:
            msg = new M_Attributes_In_Scope();
            break;
        case Message::Type::ATTRIBUTES_OUT_OF_SCOPE:
            msg = new M_Attributes_Out_Of_Scope();
            break;
        case Message::Type::TURN_UPDATES_ON_FOR_OBJECT_INSTANCE:
            msg = new M_Turn_Updates_On_For_Object_Instance();
            break;
        case Message::Type::TURN_UPDATES_OFF_FOR_OBJECT_INSTANCE:
            msg = new M_Turn_Updates_Off_For_Object_Instance();
            break;
        case Message::Type::REQUEST_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
            msg = new M_Request_Attribute_Ownership_Divestiture();
            break;
        case Message::Type::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
            msg = new M_Request_Attribute_Ownership_Assumption();
            break;
        case Message::Type::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
            msg = new M_Negotiated_Attribute_Ownership_Divestiture();
            break;
        case Message::Type::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
            msg = new M_Attribute_Ownership_Divestiture_Notification();
            break;
        case Message::Type::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
            msg = new M_Attribute_Ownership_Acquisition_Notification();
            break;
        case Message::Type::REQUEST_ATTRIBUTE_OWNERSHIP_ACQUISITION:
            msg = new M_Request_Attribute_Ownership_Acquisition();
            break;
        case Message::Type::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
            msg = new M_Request_Attribute_Ownership_Release();
            break;
        case Message::Type::QUERY_ATTRIBUTE_OWNERSHIP:
            msg = new M_Query_Attribute_Ownership();
            break;
        case Message::Type::INFORM_ATTRIBUTE_OWNERSHIP:
            msg = new M_Inform_Attribute_Ownership();
            break;
        case Message::Type::IS_ATTRIBUTE_OWNED_BY_FEDERATE:
            msg = new M_Is_Attribute_Owned_By_Federate();
            break;
        case Message::Type::ATTRIBUTE_IS_NOT_OWNED:
            msg = new M_Attribute_Is_Not_Owned();
            break;
        case Message::Type::ATTRIBUTE_OWNED_BY_RTI:
            msg = new M_Attribute_Owned_By_Rti();
            break;
        case Message::Type::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
            msg = new M_Attribute_Ownership_Acquisition_If_Available();
            break;
        case Message::Type::ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
            msg = new M_Attribute_Ownership_Unavailable();
            break;
        case Message::Type::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
            msg = new M_Unconditional_Attribute_Ownership_Divestiture();
            break;
        case Message::Type::ATTRIBUTE_OWNERSHIP_ACQUISITION:
            msg = new M_Attribute_Ownership_Acquisition();
            break;
        case Message::Type::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
            msg = new M_Cancel_Negotiated_Attribute_Ownership_Divestiture();
            break;
        case Message::Type::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
            msg = new M_Attribute_Ownership_Release_Response();
            break;
        case Message::Type::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
            msg = new M_Cancel_Attribute_Ownership_Acquisition();
            break;
        case Message::Type::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
            msg = new M_Confirm_Attribute_Ownership_Acquisition_Cancellation();
            break;
        case Message::Type::CHANGE_ATTRIBUTE_ORDER_TYPE:
            msg = new M_Change_Attribute_Order_Type();
            break;
        case Message::Type::CHANGE_INTERACTION_ORDER_TYPE:
            msg = new M_Change_Interaction_Order_Type();
            break;
        case Message::Type::ENABLE_TIME_REGULATION:
            msg = new M_Enable_Time_Regulation();
            break;
        case Message::Type::DISABLE_TIME_REGULATION:
            msg = new M_Disable_Time_Regulation();
            break;
        case Message::Type::ENABLE_TIME_CONSTRAINED:
            msg = new M_Enable_Time_Constrained();
            break;
        case Message::Type::DISABLE_TIME_CONSTRAINED:
            msg = new M_Disable_Time_Constrained();
            break;
        case Message::Type::QUERY_LBTS:
            msg = new M_Query_Lbts();
            break;
        case Message::Type::QUERY_FEDERATE_TIME:
            msg = new M_Query_Federate_Time();
            break;
        case Message::Type::QUERY_MIN_NEXT_EVENT_TIME:
            msg = new M_Query_Min_Next_Event_Time();
            break;
        case Message::Type::MODIFY_LOOKAHEAD:
            msg = new M_Modify_Lookahead();
            break;
        case Message::Type::QUERY_LOOKAHEAD:
            msg = new M_Query_Lookahead();
            break;
        case Message::Type::RETRACT:
            msg = new M_Retract();
            break;
        case Message::Type::REQUEST_RETRACTION:
            msg = new M_Request_Retraction();
            break;
        case Message::Type::TIME_ADVANCE_REQUEST:
            msg = new M_Time_Advance_Request();
            break;
        case Message::Type::TIME_ADVANCE_REQUEST_AVAILABLE:
            msg = new M_Time_Advance_Request_Available();
            break;
        case Message::Type::NEXT_EVENT_REQUEST:
            msg = new M_Next_Event_Request();
            break;
        case Message::Type::NEXT_EVENT_REQUEST_AVAILABLE:
            msg = new M_Next_Event_Request_Available();
            break;
        case Message::Type::FLUSH_QUEUE_REQUEST:
            msg = new M_Flush_Queue_Request();
            break;
        case Message::Type::TIME_ADVANCE_GRANT:
            msg = new M_Time_Advance_Grant();
            break;
        case Message::Type::ENABLE_ASYNCHRONOUS_DELIVERY:
            msg = new M_Enable_Asynchronous_Delivery();
            break;
        case Message::Type::DISABLE_ASYNCHRONOUS_DELIVERY:
            msg = new M_Disable_Asynchronous_Delivery();
            break;
        case Message::Type::TIME_REGULATION_ENABLED:
            msg = new M_Time_Regulation_Enabled();
            break;
        case Message::Type::TIME_CONSTRAINED_ENABLED:
            msg = new M_Time_Constrained_Enabled();
            break;
        case Message::Type::DDM_CREATE_REGION:
            msg = new M_Ddm_Create_Region();
            break;
        case Message::Type::DDM_MODIFY_REGION:
            msg = new M_Ddm_Modify_Region();
            break;
        case Message::Type::DDM_DELETE_REGION:
            msg = new M_Ddm_Delete_Region();
            break;
        case Message::Type::DDM_REGISTER_OBJECT:
            msg = new M_Ddm_Register_Object();
            break;
        case Message::Type::DDM_ASSOCIATE_REGION:
            msg = new M_Ddm_Associate_Region();
            break;
        case Message::Type::DDM_UNASSOCIATE_REGION:
            msg = new M_Ddm_Unassociate_Region();
            break;
        case Message::Type::DDM_SUBSCRIBE_ATTRIBUTES:
            msg = new M_Ddm_Subscribe_Attributes();
            break;
        case Message::Type::DDM_UNSUBSCRIBE_ATTRIBUTES:
            msg = new M_Ddm_Unsubscribe_Attributes();
            break;
        case Message::Type::DDM_SUBSCRIBE_INTERACTION:
            msg = new M_Ddm_Subscribe_Interaction();
            break;
        case Message::Type::DDM_UNSUBSCRIBE_INTERACTION:
            msg = new M_Ddm_Unsubscribe_Interaction();
            break;
        case Message::Type::DDM_REQUEST_UPDATE:
            msg = new M_Ddm_Request_Update();
            break;
        case Message::Type::GET_OBJECT_CLASS_HANDLE:
            msg = new M_Get_Object_Class_Handle();
            break;
        case Message::Type::GET_OBJECT_CLASS_NAME:
            msg = new M_Get_Object_Class_Name();
            break;
        case Message::Type::GET_ATTRIBUTE_HANDLE:
            msg = new M_Get_Attribute_Handle();
            break;
        case Message::Type::GET_ATTRIBUTE_NAME:
            msg = new M_Get_Attribute_Name();
            break;
        case Message::Type::GET_INTERACTION_CLASS_HANDLE:
            msg = new M_Get_Interaction_Class_Handle();
            break;
        case Message::Type::GET_INTERACTION_CLASS_NAME:
            msg = new M_Get_Interaction_Class_Name();
            break;
        case Message::Type::GET_PARAMETER_HANDLE:
            msg = new M_Get_Parameter_Handle();
            break;
        case Message::Type::GET_PARAMETER_NAME:
            msg = new M_Get_Parameter_Name();
            break;
        case Message::Type::GET_OBJECT_INSTANCE_HANDLE:
            msg = new M_Get_Object_Instance_Handle();
            break;
        case Message::Type::GET_OBJECT_INSTANCE_NAME:
            msg = new M_Get_Object_Instance_Name();
            break;
        case Message::Type::GET_SPACE_HANDLE:
            msg = new M_Get_Space_Handle();
            break;
        case Message::Type::GET_SPACE_NAME:
            msg = new M_Get_Space_Name();
            break;
        case Message::Type::GET_DIMENSION_HANDLE:
            msg = new M_Get_Dimension_Handle();
            break;
        case Message::Type::GET_DIMENSION_NAME:
            msg = new M_Get_Dimension_Name();
            break;
        case Message::Type::GET_ATTRIBUTE_SPACE_HANDLE:
            msg = new M_Get_Attribute_Space_Handle();
            break;
        case Message::Type::GET_OBJECT_CLASS:
            msg = new M_Get_Object_Class();
            break;
        case Message::Type::GET_INTERACTION_SPACE_HANDLE:
            msg = new M_Get_Interaction_Space_Handle();
            break;
        case Message::Type::GET_TRANSPORTATION_HANDLE:
            msg = new M_Get_Transportation_Handle();
            break;
        case Message::Type::GET_TRANSPORTATION_NAME:
            msg = new M_Get_Transportation_Name();
            break;
        case Message::Type::GET_ORDERING_HANDLE:
            msg = new M_Get_Ordering_Handle();
            break;
        case Message::Type::GET_ORDERING_NAME:
            msg = new M_Get_Ordering_Name();
            break;
        case Message::Type::ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH:
            msg = new M_Enable_Class_Relevance_Advisory_Switch();
            break;
        case Message::Type::DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH:
            msg = new M_Disable_Class_Relevance_Advisory_Switch();
            break;
        case Message::Type::ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH:
            msg = new M_Enable_Attribute_Relevance_Advisory_Switch();
            break;
        case Message::Type::DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH:
            msg = new M_Disable_Attribute_Relevance_Advisory_Switch();
            break;
        case Message::Type::ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH:
            msg = new M_Enable_Attribute_Scope_Advisory_Switch();
            break;
        case Message::Type::DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH:
            msg = new M_Disable_Attribute_Scope_Advisory_Switch();
            break;
        case Message::Type::ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
            msg = new M_Enable_Interaction_Relevance_Advisory_Switch();
            break;
        case Message::Type::DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
            msg = new M_Disable_Interaction_Relevance_Advisory_Switch();
            break;
        case Message::Type::TICK_REQUEST:
            msg = new M_Tick_Request();
            break;
        case Message::Type::TICK_REQUEST_NEXT:
            msg = new M_Tick_Request_Next();
            break;
        case Message::Type::TICK_REQUEST_STOP:
            msg = new M_Tick_Request_Stop();
            break;
        case Message::Type::RESERVE_OBJECT_INSTANCE_NAME:
            msg = new M_Reserve_Object_Instance_Name();
            break;
        case Message::Type::RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED:
            msg = new M_Reserve_Object_Instance_Name_Succeeded();
            break;
        case Message::Type::RESERVE_OBJECT_INSTANCE_NAME_FAILED:
            msg = new M_Reserve_Object_Instance_Name_Failed();
            break;
        case Message::Type::LAST:
            throw NetworkError("LAST message type should not be used!!");
            break;
    } // end if switch (type)
    return msg;
} /* end of M_Factory::create */

Message* M_Factory::receive(MStreamType stream) throw (NetworkError ,NetworkSignal) { 
    // FIXME This is not thread safe
    static libhla::MessageBuffer msgBuffer;
    Message  msgGen;
    Message* msg;

    // receive generic message 
    msgGen.receive(stream,msgBuffer);
    // create specific message from type 
    msg = M_Factory::create(msgGen.getMessageType());
    msgBuffer.assumeSizeFromReservedBytes();
    msg->deserialize(msgBuffer);
    return msg;
} /* end of M_Factory::receive */ 

} // end of namespace certi 
