// Generated on 2018 April Fri, 13 at 11:41:23 by the CERTI message generator
#ifndef NM_CLASSES_HH
#define NM_CLASSES_HH
// ****-**** Global System includes ****-****
#include <string>
#include <vector>
// ****-**** Includes coming from native types ****-****
// Network message is the base class
// for message exchanged between RTIA and RTIG
// AKA CERTI Network Message
#include "NetworkMessage.hh"

#include "FedTimeD.hh"

#include "certi.hh"
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

// Native types has been defined:
//     - by included headers (see above)
//     - with typedef (see below [if any])

typedef Socket* NMStreamType;

typedef NetworkMessage::Type NM_Type;
namespace CERTI_NetworkMessage {
    static const uint32_t versionMajor = 1;
    static const uint32_t versionMinor = 0;

}
// The EventRetraction is not inheriting from base "Message"
// this is a plain message which may be used as field
// in messages merging from "Message". As such
// it won't appear in the generated factory method.
// It's a convenient way to describe a structured native
// message using "combine". 
//message EventRetraction {
//    required FederateHandle sendingFederate {default = 0}
//    required uint64         SN              {default = 0}
//}
class CERTI_EXPORT NM_FOM_Dimension {
public:
    NM_FOM_Dimension() = default;
    ~NM_FOM_Dimension() = default;
    
    void serialize(libhla::MessageBuffer& msgBuffer);
    void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const DimensionHandle& getHandle() const;
    void setHandle(const DimensionHandle& newHandle);
    
    const std::string& getName() const;
    void setName(const std::string& newName);
    
    friend std::ostream& operator<<(std::ostream& os, const NM_FOM_Dimension& msg);

protected:
    DimensionHandle handle {0};
    std::string name;
};

std::ostream& operator<<(std::ostream& os, const NM_FOM_Dimension& msg);


class CERTI_EXPORT NM_FOM_Routing_Space {
public:
    NM_FOM_Routing_Space() = default;
    ~NM_FOM_Routing_Space() = default;
    
    void serialize(libhla::MessageBuffer& msgBuffer);
    void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const SpaceHandle& getSpace() const;
    void setSpace(const SpaceHandle& newSpace);
    
    const std::string& getName() const;
    void setName(const std::string& newName);
    
    uint32_t getDimensionsSize() const;
    void setDimensionsSize(uint32_t num);
    const std::vector<NM_FOM_Dimension>& getDimensions() const;
    const NM_FOM_Dimension& getDimensions(uint32_t rank) const;
    NM_FOM_Dimension& getDimensions(uint32_t rank);
    void setDimensions(const NM_FOM_Dimension& newDimensions, uint32_t rank);
    void removeDimensions(uint32_t rank);
    
    friend std::ostream& operator<<(std::ostream& os, const NM_FOM_Routing_Space& msg);

protected:
    SpaceHandle space {0};
    std::string name;
    std::vector<NM_FOM_Dimension> dimensions;
};

std::ostream& operator<<(std::ostream& os, const NM_FOM_Routing_Space& msg);


class CERTI_EXPORT NM_FOM_Attribute {
public:
    NM_FOM_Attribute() = default;
    ~NM_FOM_Attribute() = default;
    
    void serialize(libhla::MessageBuffer& msgBuffer);
    void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const AttributeHandle& getHandle() const;
    void setHandle(const AttributeHandle& newHandle);
    
    const std::string& getName() const;
    void setName(const std::string& newName);
    
    const SpaceHandle& getSpaceHandle() const;
    void setSpaceHandle(const SpaceHandle& newSpaceHandle);
    
    const OrderType& getOrder() const;
    void setOrder(const OrderType& newOrder);
    
    const TransportType& getTransport() const;
    void setTransport(const TransportType& newTransport);
    
    friend std::ostream& operator<<(std::ostream& os, const NM_FOM_Attribute& msg);

protected:
    AttributeHandle handle {0};
    std::string name;
    SpaceHandle spaceHandle {0};
    OrderType order;
    TransportType transport;
};

std::ostream& operator<<(std::ostream& os, const NM_FOM_Attribute& msg);


class CERTI_EXPORT NM_FOM_Object_Class {
public:
    NM_FOM_Object_Class() = default;
    ~NM_FOM_Object_Class() = default;
    
    void serialize(libhla::MessageBuffer& msgBuffer);
    void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectClassHandle& getHandle() const;
    void setHandle(const ObjectClassHandle& newHandle);
    
    const ObjectClassHandle& getSuperClass() const;
    void setSuperClass(const ObjectClassHandle& newSuperClass);
    
    const std::string& getName() const;
    void setName(const std::string& newName);
    
    uint32_t getAttributesSize() const;
    void setAttributesSize(uint32_t num);
    const std::vector<NM_FOM_Attribute>& getAttributes() const;
    const NM_FOM_Attribute& getAttributes(uint32_t rank) const;
    NM_FOM_Attribute& getAttributes(uint32_t rank);
    void setAttributes(const NM_FOM_Attribute& newAttributes, uint32_t rank);
    void removeAttributes(uint32_t rank);
    
    friend std::ostream& operator<<(std::ostream& os, const NM_FOM_Object_Class& msg);

protected:
    ObjectClassHandle handle {0};
    ObjectClassHandle superClass {0};
    std::string name;
    std::vector<NM_FOM_Attribute> attributes;
};

std::ostream& operator<<(std::ostream& os, const NM_FOM_Object_Class& msg);


class CERTI_EXPORT NM_FOM_Parameter {
public:
    NM_FOM_Parameter() = default;
    ~NM_FOM_Parameter() = default;
    
    void serialize(libhla::MessageBuffer& msgBuffer);
    void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ParameterHandle& getHandle() const;
    void setHandle(const ParameterHandle& newHandle);
    
    const std::string& getName() const;
    void setName(const std::string& newName);
    
    friend std::ostream& operator<<(std::ostream& os, const NM_FOM_Parameter& msg);

protected:
    ParameterHandle handle {0};
    std::string name;
};

std::ostream& operator<<(std::ostream& os, const NM_FOM_Parameter& msg);


class CERTI_EXPORT NM_FOM_Interaction_Class {
public:
    NM_FOM_Interaction_Class() = default;
    ~NM_FOM_Interaction_Class() = default;
    
    void serialize(libhla::MessageBuffer& msgBuffer);
    void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const InteractionClassHandle& getInteractionClass() const;
    void setInteractionClass(const InteractionClassHandle& newInteractionClass);
    
    const std::string& getName() const;
    void setName(const std::string& newName);
    
    const InteractionClassHandle& getSuperClass() const;
    void setSuperClass(const InteractionClassHandle& newSuperClass);
    
    const SpaceHandle& getSpace() const;
    void setSpace(const SpaceHandle& newSpace);
    
    const OrderType& getOrder() const;
    void setOrder(const OrderType& newOrder);
    
    const TransportType& getTransport() const;
    void setTransport(const TransportType& newTransport);
    
    uint32_t getParametersSize() const;
    void setParametersSize(uint32_t num);
    const std::vector<NM_FOM_Parameter>& getParameters() const;
    const NM_FOM_Parameter& getParameters(uint32_t rank) const;
    NM_FOM_Parameter& getParameters(uint32_t rank);
    void setParameters(const NM_FOM_Parameter& newParameters, uint32_t rank);
    void removeParameters(uint32_t rank);
    
    friend std::ostream& operator<<(std::ostream& os, const NM_FOM_Interaction_Class& msg);

protected:
    InteractionClassHandle interactionClass {0};
    std::string name;
    InteractionClassHandle superClass {0};
    SpaceHandle space {0};
    OrderType order;
    TransportType transport;
    std::vector<NM_FOM_Parameter> parameters;
};

std::ostream& operator<<(std::ostream& os, const NM_FOM_Interaction_Class& msg);


class CERTI_EXPORT NM_Close_Connexion : public NetworkMessage {
public:
    NM_Close_Connexion();
    virtual ~NM_Close_Connexion() = default;
    
};


class CERTI_EXPORT NM_Message_Null : public NetworkMessage {
public:
    NM_Message_Null();
    virtual ~NM_Message_Null() = default;
    
};

// Create the federation execution
class CERTI_EXPORT NM_Create_Federation_Execution : public NetworkMessage {
public:
    NM_Create_Federation_Execution();
    virtual ~NM_Create_Federation_Execution() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const std::string& getFederationExecutionName() const;
    void setFederationExecutionName(const std::string& newFederationExecutionName);
    
    const RtiVersion& getRtiVersion() const;
    void setRtiVersion(const RtiVersion& newRtiVersion);
    
    uint32_t getFomModuleDesignatorsSize() const;
    void setFomModuleDesignatorsSize(uint32_t num);
    const std::vector<std::string>& getFomModuleDesignators() const;
    const std::string& getFomModuleDesignators(uint32_t rank) const;
    std::string& getFomModuleDesignators(uint32_t rank);
    void setFomModuleDesignators(const std::string& newFomModuleDesignators, uint32_t rank);
    void removeFomModuleDesignators(uint32_t rank);
    
    const std::string& getMimDesignator() const;
    void setMimDesignator(const std::string& newMimDesignator);
    bool hasMimDesignator() const;
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Create_Federation_Execution& msg);

protected:
    std::string federationExecutionName;// the federation name
    RtiVersion rtiVersion;// the rti version
    std::vector<std::string> fomModuleDesignators;// the Federation ID which is in fact a filename
    std::string mimDesignator;
    bool _hasMimDesignator {false};
};

std::ostream& operator<<(std::ostream& os, const NM_Create_Federation_Execution& msg);

// Destroy the federation execution
class CERTI_EXPORT NM_Destroy_Federation_Execution : public NetworkMessage {
public:
    NM_Destroy_Federation_Execution();
    virtual ~NM_Destroy_Federation_Execution() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const std::string& getFederationName() const;
    void setFederationName(const std::string& newFederationName);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Destroy_Federation_Execution& msg);

protected:
    std::string federationName;
};

std::ostream& operator<<(std::ostream& os, const NM_Destroy_Federation_Execution& msg);

// Join the federation
class CERTI_EXPORT NM_Join_Federation_Execution : public NetworkMessage {
public:
    NM_Join_Federation_Execution();
    virtual ~NM_Join_Federation_Execution() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const int32_t& getNumberOfRegulators() const;
    void setNumberOfRegulators(const int32_t& newNumberOfRegulators);
    
    const uint32_t& getMulticastAddress() const;
    void setMulticastAddress(const uint32_t& newMulticastAddress);
    
    const uint32_t& getBestEffortAddress() const;
    void setBestEffortAddress(const uint32_t& newBestEffortAddress);
    
    const uint32_t& getBestEffortPeer() const;
    void setBestEffortPeer(const uint32_t& newBestEffortPeer);
    
    const std::string& getFederationExecutionName() const;
    void setFederationExecutionName(const std::string& newFederationExecutionName);
    
    const std::string& getFederateName() const;
    void setFederateName(const std::string& newFederateName);
    bool hasFederateName() const;
    
    const RtiVersion& getRtiVersion() const;
    void setRtiVersion(const RtiVersion& newRtiVersion);
    
    const std::string& getFederateType() const;
    void setFederateType(const std::string& newFederateType);
    
    uint32_t getAdditionalFomModulesSize() const;
    void setAdditionalFomModulesSize(uint32_t num);
    const std::vector<std::string>& getAdditionalFomModules() const;
    const std::string& getAdditionalFomModules(uint32_t rank) const;
    std::string& getAdditionalFomModules(uint32_t rank);
    void setAdditionalFomModules(const std::string& newAdditionalFomModules, uint32_t rank);
    void removeAdditionalFomModules(uint32_t rank);
    
    uint32_t getRoutingSpacesSize() const;
    void setRoutingSpacesSize(uint32_t num);
    const std::vector<NM_FOM_Routing_Space>& getRoutingSpaces() const;
    const NM_FOM_Routing_Space& getRoutingSpaces(uint32_t rank) const;
    NM_FOM_Routing_Space& getRoutingSpaces(uint32_t rank);
    void setRoutingSpaces(const NM_FOM_Routing_Space& newRoutingSpaces, uint32_t rank);
    void removeRoutingSpaces(uint32_t rank);
    
    uint32_t getObjectClassesSize() const;
    void setObjectClassesSize(uint32_t num);
    const std::vector<NM_FOM_Object_Class>& getObjectClasses() const;
    const NM_FOM_Object_Class& getObjectClasses(uint32_t rank) const;
    NM_FOM_Object_Class& getObjectClasses(uint32_t rank);
    void setObjectClasses(const NM_FOM_Object_Class& newObjectClasses, uint32_t rank);
    void removeObjectClasses(uint32_t rank);
    
    uint32_t getInteractionClassesSize() const;
    void setInteractionClassesSize(uint32_t num);
    const std::vector<NM_FOM_Interaction_Class>& getInteractionClasses() const;
    const NM_FOM_Interaction_Class& getInteractionClasses(uint32_t rank) const;
    NM_FOM_Interaction_Class& getInteractionClasses(uint32_t rank);
    void setInteractionClasses(const NM_FOM_Interaction_Class& newInteractionClasses, uint32_t rank);
    void removeInteractionClasses(uint32_t rank);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Join_Federation_Execution& msg);

protected:
    int32_t numberOfRegulators;
    uint32_t multicastAddress;
    uint32_t bestEffortAddress;
    uint32_t bestEffortPeer;
    std::string federationExecutionName;// the federation name
    std::string federateName;// the federate name (should be unique within a federation)
    bool _hasFederateName {false};
    RtiVersion rtiVersion;// the rti version
    std::string federateType;
    std::vector<std::string> additionalFomModules;
    std::vector<NM_FOM_Routing_Space> routingSpaces;
    std::vector<NM_FOM_Object_Class> objectClasses;
    std::vector<NM_FOM_Interaction_Class> interactionClasses;
};

std::ostream& operator<<(std::ostream& os, const NM_Join_Federation_Execution& msg);


class CERTI_EXPORT NM_Additional_Fom_Module : public NetworkMessage {
public:
    NM_Additional_Fom_Module();
    virtual ~NM_Additional_Fom_Module() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    uint32_t getRoutingSpacesSize() const;
    void setRoutingSpacesSize(uint32_t num);
    const std::vector<NM_FOM_Routing_Space>& getRoutingSpaces() const;
    const NM_FOM_Routing_Space& getRoutingSpaces(uint32_t rank) const;
    NM_FOM_Routing_Space& getRoutingSpaces(uint32_t rank);
    void setRoutingSpaces(const NM_FOM_Routing_Space& newRoutingSpaces, uint32_t rank);
    void removeRoutingSpaces(uint32_t rank);
    
    uint32_t getObjectClassesSize() const;
    void setObjectClassesSize(uint32_t num);
    const std::vector<NM_FOM_Object_Class>& getObjectClasses() const;
    const NM_FOM_Object_Class& getObjectClasses(uint32_t rank) const;
    NM_FOM_Object_Class& getObjectClasses(uint32_t rank);
    void setObjectClasses(const NM_FOM_Object_Class& newObjectClasses, uint32_t rank);
    void removeObjectClasses(uint32_t rank);
    
    uint32_t getInteractionClassesSize() const;
    void setInteractionClassesSize(uint32_t num);
    const std::vector<NM_FOM_Interaction_Class>& getInteractionClasses() const;
    const NM_FOM_Interaction_Class& getInteractionClasses(uint32_t rank) const;
    NM_FOM_Interaction_Class& getInteractionClasses(uint32_t rank);
    void setInteractionClasses(const NM_FOM_Interaction_Class& newInteractionClasses, uint32_t rank);
    void removeInteractionClasses(uint32_t rank);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Additional_Fom_Module& msg);

protected:
    std::vector<NM_FOM_Routing_Space> routingSpaces;
    std::vector<NM_FOM_Object_Class> objectClasses;
    std::vector<NM_FOM_Interaction_Class> interactionClasses;
};

std::ostream& operator<<(std::ostream& os, const NM_Additional_Fom_Module& msg);

// Resign from federation
class CERTI_EXPORT NM_Resign_Federation_Execution : public NetworkMessage {
public:
    NM_Resign_Federation_Execution();
    virtual ~NM_Resign_Federation_Execution() = default;
    
};


class CERTI_EXPORT NM_Set_Time_Regulating : public NetworkMessage {
public:
    NM_Set_Time_Regulating();
    virtual ~NM_Set_Time_Regulating() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    void regulatorOn();
    void regulatorOff();
    bool isRegulatorOn() const;
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Set_Time_Regulating& msg);

protected:
    bool regulator;
};

std::ostream& operator<<(std::ostream& os, const NM_Set_Time_Regulating& msg);


class CERTI_EXPORT NM_Set_Time_Constrained : public NetworkMessage {
public:
    NM_Set_Time_Constrained();
    virtual ~NM_Set_Time_Constrained() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    void constrainedOn();
    void constrainedOff();
    bool isConstrainedOn() const;
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Set_Time_Constrained& msg);

protected:
    bool constrained;
};

std::ostream& operator<<(std::ostream& os, const NM_Set_Time_Constrained& msg);


class CERTI_EXPORT NM_Time_Regulation_Enabled : public NetworkMessage {
public:
    NM_Time_Regulation_Enabled();
    virtual ~NM_Time_Regulation_Enabled() = default;
    
};


class CERTI_EXPORT NM_Time_Constrained_Enabled : public NetworkMessage {
public:
    NM_Time_Constrained_Enabled();
    virtual ~NM_Time_Constrained_Enabled() = default;
    
};


class CERTI_EXPORT NM_Set_Class_Relevance_Advisory_Switch : public NetworkMessage {
public:
    NM_Set_Class_Relevance_Advisory_Switch();
    virtual ~NM_Set_Class_Relevance_Advisory_Switch() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    void classRelevanceAdvisorySwitchOn();
    void classRelevanceAdvisorySwitchOff();
    bool isClassRelevanceAdvisorySwitchOn() const;
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Set_Class_Relevance_Advisory_Switch& msg);

protected:
    bool classRelevanceAdvisorySwitch;// value of CRA switch     
};

std::ostream& operator<<(std::ostream& os, const NM_Set_Class_Relevance_Advisory_Switch& msg);


class CERTI_EXPORT NM_Set_Interaction_Relevance_Advisory_Switch : public NetworkMessage {
public:
    NM_Set_Interaction_Relevance_Advisory_Switch();
    virtual ~NM_Set_Interaction_Relevance_Advisory_Switch() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    void interactionRelevanceAdvisorySwitchOn();
    void interactionRelevanceAdvisorySwitchOff();
    bool isInteractionRelevanceAdvisorySwitchOn() const;
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Set_Interaction_Relevance_Advisory_Switch& msg);

protected:
    bool interactionRelevanceAdvisorySwitch;// value of IRA switch     
};

std::ostream& operator<<(std::ostream& os, const NM_Set_Interaction_Relevance_Advisory_Switch& msg);


class CERTI_EXPORT NM_Set_Attribute_Relevance_Advisory_Switch : public NetworkMessage {
public:
    NM_Set_Attribute_Relevance_Advisory_Switch();
    virtual ~NM_Set_Attribute_Relevance_Advisory_Switch() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    void attributeRelevanceAdvisorySwitchOn();
    void attributeRelevanceAdvisorySwitchOff();
    bool isAttributeRelevanceAdvisorySwitchOn() const;
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Set_Attribute_Relevance_Advisory_Switch& msg);

protected:
    bool attributeRelevanceAdvisorySwitch;// value of ARA switch     
};

std::ostream& operator<<(std::ostream& os, const NM_Set_Attribute_Relevance_Advisory_Switch& msg);


class CERTI_EXPORT NM_Set_Attribute_Scope_Advisory_Switch : public NetworkMessage {
public:
    NM_Set_Attribute_Scope_Advisory_Switch();
    virtual ~NM_Set_Attribute_Scope_Advisory_Switch() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    void attributeScopeAdvisorySwitchOn();
    void attributeScopeAdvisorySwitchOff();
    bool isAttributeScopeAdvisorySwitchOn() const;
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Set_Attribute_Scope_Advisory_Switch& msg);

protected:
    bool attributeScopeAdvisorySwitch;// value of ASA switch     
};

std::ostream& operator<<(std::ostream& os, const NM_Set_Attribute_Scope_Advisory_Switch& msg);


class CERTI_EXPORT NM_Register_Federation_Synchronization_Point : public NetworkMessage {
public:
    NM_Register_Federation_Synchronization_Point();
    virtual ~NM_Register_Federation_Synchronization_Point() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const bool& getExists() const;
    void setExists(const bool& newExists);
    
    uint32_t getFederatesSize() const;
    void setFederatesSize(uint32_t num);
    const std::vector<FederateHandle>& getFederates() const;
    const FederateHandle& getFederates(uint32_t rank) const;
    FederateHandle& getFederates(uint32_t rank);
    void setFederates(const FederateHandle& newFederates, uint32_t rank);
    void removeFederates(uint32_t rank);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Register_Federation_Synchronization_Point& msg);

protected:
    bool exists;// tell whether if the synchronization set is specified or not
    std::vector<FederateHandle> federates;// the federate set
};

std::ostream& operator<<(std::ostream& os, const NM_Register_Federation_Synchronization_Point& msg);

// HLA 1.3    - §4.7
// IEEE-1516  - §4.7
class CERTI_EXPORT NM_Confirm_Synchronization_Point_Registration : public NetworkMessage {
public:
    NM_Confirm_Synchronization_Point_Registration();
    virtual ~NM_Confirm_Synchronization_Point_Registration() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const bool& getSuccessIndicator() const;
    void setSuccessIndicator(const bool& newSuccessIndicator);
    
    const std::string& getFailureReason() const;
    void setFailureReason(const std::string& newFailureReason);
    bool hasFailureReason() const;
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Confirm_Synchronization_Point_Registration& msg);

protected:
    bool successIndicator;// the success indicator true=successful
    std::string failureReason;// [IEEE-1516] the failure reason.
    bool _hasFailureReason {false};
};

std::ostream& operator<<(std::ostream& os, const NM_Confirm_Synchronization_Point_Registration& msg);


class CERTI_EXPORT NM_Announce_Synchronization_Point : public NetworkMessage {
public:
    NM_Announce_Synchronization_Point();
    virtual ~NM_Announce_Synchronization_Point() = default;
    
};


class CERTI_EXPORT NM_Synchronization_Point_Achieved : public NetworkMessage {
public:
    NM_Synchronization_Point_Achieved();
    virtual ~NM_Synchronization_Point_Achieved() = default;
    
};


class CERTI_EXPORT NM_Federation_Synchronized : public NetworkMessage {
public:
    NM_Federation_Synchronized();
    virtual ~NM_Federation_Synchronized() = default;
    
};


class CERTI_EXPORT NM_Request_Federation_Save : public NetworkMessage {
public:
    NM_Request_Federation_Save();
    virtual ~NM_Request_Federation_Save() = default;
    
};


class CERTI_EXPORT NM_Federate_Save_Begun : public NetworkMessage {
public:
    NM_Federate_Save_Begun();
    virtual ~NM_Federate_Save_Begun() = default;
    
};


class CERTI_EXPORT NM_Federate_Save_Complete : public NetworkMessage {
public:
    NM_Federate_Save_Complete();
    virtual ~NM_Federate_Save_Complete() = default;
    
};


class CERTI_EXPORT NM_Federate_Save_Not_Complete : public NetworkMessage {
public:
    NM_Federate_Save_Not_Complete();
    virtual ~NM_Federate_Save_Not_Complete() = default;
    
};


class CERTI_EXPORT NM_Initiate_Federate_Save : public NetworkMessage {
public:
    NM_Initiate_Federate_Save();
    virtual ~NM_Initiate_Federate_Save() = default;
    
};


class CERTI_EXPORT NM_Federation_Saved : public NetworkMessage {
public:
    NM_Federation_Saved();
    virtual ~NM_Federation_Saved() = default;
    
};


class CERTI_EXPORT NM_Federation_Not_Saved : public NetworkMessage {
public:
    NM_Federation_Not_Saved();
    virtual ~NM_Federation_Not_Saved() = default;
    
};


class CERTI_EXPORT NM_Request_Federation_Restore : public NetworkMessage {
public:
    NM_Request_Federation_Restore();
    virtual ~NM_Request_Federation_Restore() = default;
    
};


class CERTI_EXPORT NM_Federate_Restore_Complete : public NetworkMessage {
public:
    NM_Federate_Restore_Complete();
    virtual ~NM_Federate_Restore_Complete() = default;
    
};


class CERTI_EXPORT NM_Federate_Restore_Not_Complete : public NetworkMessage {
public:
    NM_Federate_Restore_Not_Complete();
    virtual ~NM_Federate_Restore_Not_Complete() = default;
    
};


class CERTI_EXPORT NM_Request_Federation_Restore_Succeeded : public NetworkMessage {
public:
    NM_Request_Federation_Restore_Succeeded();
    virtual ~NM_Request_Federation_Restore_Succeeded() = default;
    
};


class CERTI_EXPORT NM_Request_Federation_Restore_Failed : public NetworkMessage {
public:
    NM_Request_Federation_Restore_Failed();
    virtual ~NM_Request_Federation_Restore_Failed() = default;
    
};


class CERTI_EXPORT NM_Federation_Restore_Begun : public NetworkMessage {
public:
    NM_Federation_Restore_Begun();
    virtual ~NM_Federation_Restore_Begun() = default;
    
};


class CERTI_EXPORT NM_Initiate_Federate_Restore : public NetworkMessage {
public:
    NM_Initiate_Federate_Restore();
    virtual ~NM_Initiate_Federate_Restore() = default;
    
};


class CERTI_EXPORT NM_Federation_Restored : public NetworkMessage {
public:
    NM_Federation_Restored();
    virtual ~NM_Federation_Restored() = default;
    
};


class CERTI_EXPORT NM_Federation_Not_Restored : public NetworkMessage {
public:
    NM_Federation_Not_Restored();
    virtual ~NM_Federation_Not_Restored() = default;
    
};


class CERTI_EXPORT NM_Publish_Object_Class : public NetworkMessage {
public:
    NM_Publish_Object_Class();
    virtual ~NM_Publish_Object_Class() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectClassHandle& getObjectClass() const;
    void setObjectClass(const ObjectClassHandle& newObjectClass);
    
    uint32_t getAttributesSize() const;
    void setAttributesSize(uint32_t num);
    const std::vector<AttributeHandle>& getAttributes() const;
    const AttributeHandle& getAttributes(uint32_t rank) const;
    AttributeHandle& getAttributes(uint32_t rank);
    void setAttributes(const AttributeHandle& newAttributes, uint32_t rank);
    void removeAttributes(uint32_t rank);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Publish_Object_Class& msg);

protected:
    ObjectClassHandle objectClass;
    std::vector<AttributeHandle> attributes;
};

std::ostream& operator<<(std::ostream& os, const NM_Publish_Object_Class& msg);


class CERTI_EXPORT NM_Unpublish_Object_Class : public NM_Publish_Object_Class {
public:
    NM_Unpublish_Object_Class();
    virtual ~NM_Unpublish_Object_Class() = default;
    
};


class CERTI_EXPORT NM_Publish_Interaction_Class : public NetworkMessage {
public:
    NM_Publish_Interaction_Class();
    virtual ~NM_Publish_Interaction_Class() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const InteractionClassHandle& getInteractionClass() const;
    void setInteractionClass(const InteractionClassHandle& newInteractionClass);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Publish_Interaction_Class& msg);

protected:
    InteractionClassHandle interactionClass;// the interaction class
};

std::ostream& operator<<(std::ostream& os, const NM_Publish_Interaction_Class& msg);


class CERTI_EXPORT NM_Unpublish_Interaction_Class : public NM_Publish_Interaction_Class {
public:
    NM_Unpublish_Interaction_Class();
    virtual ~NM_Unpublish_Interaction_Class() = default;
    
};


class CERTI_EXPORT NM_Subscribe_Object_Class : public NetworkMessage {
public:
    NM_Subscribe_Object_Class();
    virtual ~NM_Subscribe_Object_Class() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectClassHandle& getObjectClass() const;
    void setObjectClass(const ObjectClassHandle& newObjectClass);
    
    uint32_t getAttributesSize() const;
    void setAttributesSize(uint32_t num);
    const std::vector<AttributeHandle>& getAttributes() const;
    const AttributeHandle& getAttributes(uint32_t rank) const;
    AttributeHandle& getAttributes(uint32_t rank);
    void setAttributes(const AttributeHandle& newAttributes, uint32_t rank);
    void removeAttributes(uint32_t rank);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Subscribe_Object_Class& msg);

protected:
    ObjectClassHandle objectClass;
    std::vector<AttributeHandle> attributes;
};

std::ostream& operator<<(std::ostream& os, const NM_Subscribe_Object_Class& msg);


class CERTI_EXPORT NM_Unsubscribe_Object_Class : public NM_Subscribe_Object_Class {
public:
    NM_Unsubscribe_Object_Class();
    virtual ~NM_Unsubscribe_Object_Class() = default;
    
};


class CERTI_EXPORT NM_Subscribe_Interaction_Class : public NM_Publish_Interaction_Class {
public:
    NM_Subscribe_Interaction_Class();
    virtual ~NM_Subscribe_Interaction_Class() = default;
    
};


class CERTI_EXPORT NM_Unsubscribe_Interaction_Class : public NM_Publish_Interaction_Class {
public:
    NM_Unsubscribe_Interaction_Class();
    virtual ~NM_Unsubscribe_Interaction_Class() = default;
    
};


class CERTI_EXPORT NM_Turn_Interactions_On : public NM_Publish_Interaction_Class {
public:
    NM_Turn_Interactions_On();
    virtual ~NM_Turn_Interactions_On() = default;
    
};


class CERTI_EXPORT NM_Turn_Interactions_Off : public NM_Publish_Interaction_Class {
public:
    NM_Turn_Interactions_Off();
    virtual ~NM_Turn_Interactions_Off() = default;
    
};

// HLA 1.3 §6.2
class CERTI_EXPORT NM_Register_Object : public NetworkMessage {
public:
    NM_Register_Object();
    virtual ~NM_Register_Object() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectClassHandle& getObjectClass() const;
    void setObjectClass(const ObjectClassHandle& newObjectClass);
    
    const std::string& getObjectName() const;
    void setObjectName(const std::string& newObjectName);
    bool hasObjectName() const;
    
    const ObjectHandle& getObject() const;
    void setObject(const ObjectHandle& newObject);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Register_Object& msg);

protected:
    ObjectClassHandle objectClass;
    std::string objectName;
    bool _hasObjectName {false};
    ObjectHandle object;
};

std::ostream& operator<<(std::ostream& os, const NM_Register_Object& msg);

// HLA 1.3 §6.3
class CERTI_EXPORT NM_Discover_Object : public NetworkMessage {
public:
    NM_Discover_Object();
    virtual ~NM_Discover_Object() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectClassHandle& getObjectClass() const;
    void setObjectClass(const ObjectClassHandle& newObjectClass);
    
    const ObjectHandle& getObject() const;
    void setObject(const ObjectHandle& newObject);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Discover_Object& msg);

protected:
    ObjectClassHandle objectClass;
    ObjectHandle object;
};

std::ostream& operator<<(std::ostream& os, const NM_Discover_Object& msg);

// HLA 1.3 §6.4
class CERTI_EXPORT NM_Update_Attribute_Values : public NetworkMessage {
public:
    NM_Update_Attribute_Values();
    virtual ~NM_Update_Attribute_Values() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectHandle& getObject() const;
    void setObject(const ObjectHandle& newObject);
    
    uint32_t getAttributesSize() const;
    void setAttributesSize(uint32_t num);
    const std::vector<AttributeHandle>& getAttributes() const;
    const AttributeHandle& getAttributes(uint32_t rank) const;
    AttributeHandle& getAttributes(uint32_t rank);
    void setAttributes(const AttributeHandle& newAttributes, uint32_t rank);
    void removeAttributes(uint32_t rank);
    
    uint32_t getValuesSize() const;
    void setValuesSize(uint32_t num);
    const std::vector<AttributeValue_t>& getValues() const;
    const AttributeValue_t& getValues(uint32_t rank) const;
    AttributeValue_t& getValues(uint32_t rank);
    void setValues(const AttributeValue_t& newValues, uint32_t rank);
    void removeValues(uint32_t rank);
    
    const EventRetractionHandle& getEvent() const;
    void setEvent(const EventRetractionHandle& newEvent);
    bool hasEvent() const;
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Update_Attribute_Values& msg);

protected:
    ObjectHandle object;
    std::vector<AttributeHandle> attributes;
    std::vector<AttributeValue_t> values;
    EventRetractionHandle event;
    bool _hasEvent {false};
};

std::ostream& operator<<(std::ostream& os, const NM_Update_Attribute_Values& msg);

// HLA 1.3 §6.5
class CERTI_EXPORT NM_Reflect_Attribute_Values : public NetworkMessage {
public:
    NM_Reflect_Attribute_Values();
    virtual ~NM_Reflect_Attribute_Values() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectHandle& getObject() const;
    void setObject(const ObjectHandle& newObject);
    
    uint32_t getAttributesSize() const;
    void setAttributesSize(uint32_t num);
    const std::vector<AttributeHandle>& getAttributes() const;
    const AttributeHandle& getAttributes(uint32_t rank) const;
    AttributeHandle& getAttributes(uint32_t rank);
    void setAttributes(const AttributeHandle& newAttributes, uint32_t rank);
    void removeAttributes(uint32_t rank);
    
    uint32_t getValuesSize() const;
    void setValuesSize(uint32_t num);
    const std::vector<AttributeValue_t>& getValues() const;
    const AttributeValue_t& getValues(uint32_t rank) const;
    AttributeValue_t& getValues(uint32_t rank);
    void setValues(const AttributeValue_t& newValues, uint32_t rank);
    void removeValues(uint32_t rank);
    
    const EventRetractionHandle& getEvent() const;
    void setEvent(const EventRetractionHandle& newEvent);
    bool hasEvent() const;
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Reflect_Attribute_Values& msg);

protected:
    ObjectHandle object;
    std::vector<AttributeHandle> attributes;
    std::vector<AttributeValue_t> values;
    EventRetractionHandle event;
    bool _hasEvent {false};
};

std::ostream& operator<<(std::ostream& os, const NM_Reflect_Attribute_Values& msg);

// HLA 1.3 §6.6
class CERTI_EXPORT NM_Send_Interaction : public NetworkMessage {
public:
    NM_Send_Interaction();
    virtual ~NM_Send_Interaction() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const InteractionClassHandle& getInteractionClass() const;
    void setInteractionClass(const InteractionClassHandle& newInteractionClass);
    
    uint32_t getParametersSize() const;
    void setParametersSize(uint32_t num);
    const std::vector<ParameterHandle>& getParameters() const;
    const ParameterHandle& getParameters(uint32_t rank) const;
    ParameterHandle& getParameters(uint32_t rank);
    void setParameters(const ParameterHandle& newParameters, uint32_t rank);
    void removeParameters(uint32_t rank);
    
    uint32_t getValuesSize() const;
    void setValuesSize(uint32_t num);
    const std::vector<ParameterValue_t>& getValues() const;
    const ParameterValue_t& getValues(uint32_t rank) const;
    ParameterValue_t& getValues(uint32_t rank);
    void setValues(const ParameterValue_t& newValues, uint32_t rank);
    void removeValues(uint32_t rank);
    
    const RegionHandle& getRegion() const;
    void setRegion(const RegionHandle& newRegion);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Send_Interaction& msg);

protected:
    InteractionClassHandle interactionClass;
    std::vector<ParameterHandle> parameters;
    std::vector<ParameterValue_t> values;
    RegionHandle region;// FIXME check this....
};

std::ostream& operator<<(std::ostream& os, const NM_Send_Interaction& msg);

// HLA 1.3 §6.7
class CERTI_EXPORT NM_Receive_Interaction : public NetworkMessage {
public:
    NM_Receive_Interaction();
    virtual ~NM_Receive_Interaction() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const InteractionClassHandle& getInteractionClass() const;
    void setInteractionClass(const InteractionClassHandle& newInteractionClass);
    
    uint32_t getParametersSize() const;
    void setParametersSize(uint32_t num);
    const std::vector<ParameterHandle>& getParameters() const;
    const ParameterHandle& getParameters(uint32_t rank) const;
    ParameterHandle& getParameters(uint32_t rank);
    void setParameters(const ParameterHandle& newParameters, uint32_t rank);
    void removeParameters(uint32_t rank);
    
    uint32_t getValuesSize() const;
    void setValuesSize(uint32_t num);
    const std::vector<ParameterValue_t>& getValues() const;
    const ParameterValue_t& getValues(uint32_t rank) const;
    ParameterValue_t& getValues(uint32_t rank);
    void setValues(const ParameterValue_t& newValues, uint32_t rank);
    void removeValues(uint32_t rank);
    
    const EventRetractionHandle& getEvent() const;
    void setEvent(const EventRetractionHandle& newEvent);
    bool hasEvent() const;
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Receive_Interaction& msg);

protected:
    InteractionClassHandle interactionClass;
    std::vector<ParameterHandle> parameters;
    std::vector<ParameterValue_t> values;
    EventRetractionHandle event;
    bool _hasEvent {false};
};

std::ostream& operator<<(std::ostream& os, const NM_Receive_Interaction& msg);

// HLA 1.3 §6.8 Delete Object Instance
class CERTI_EXPORT NM_Delete_Object : public NetworkMessage {
public:
    NM_Delete_Object();
    virtual ~NM_Delete_Object() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectHandle& getObject() const;
    void setObject(const ObjectHandle& newObject);
    
    const EventRetractionHandle& getEvent() const;
    void setEvent(const EventRetractionHandle& newEvent);
    bool hasEvent() const;
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Delete_Object& msg);

protected:
    ObjectHandle object;
    EventRetractionHandle event;
    bool _hasEvent {false};
};

std::ostream& operator<<(std::ostream& os, const NM_Delete_Object& msg);

// HLA 1.3 §6.9 Remove Object Instance
class CERTI_EXPORT NM_Remove_Object : public NetworkMessage {
public:
    NM_Remove_Object();
    virtual ~NM_Remove_Object() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectHandle& getObject() const;
    void setObject(const ObjectHandle& newObject);
    
    const ObjectClassHandle& getObjectClass() const;
    void setObjectClass(const ObjectClassHandle& newObjectClass);
    
    const EventRetractionHandle& getEvent() const;
    void setEvent(const EventRetractionHandle& newEvent);
    bool hasEvent() const;
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Remove_Object& msg);

protected:
    ObjectHandle object;
    ObjectClassHandle objectClass;
    EventRetractionHandle event;
    bool _hasEvent {false};
};

std::ostream& operator<<(std::ostream& os, const NM_Remove_Object& msg);


class CERTI_EXPORT NM_Change_Attribute_Transport_Type : public NetworkMessage {
public:
    NM_Change_Attribute_Transport_Type();
    virtual ~NM_Change_Attribute_Transport_Type() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectHandle& getObject() const;
    void setObject(const ObjectHandle& newObject);
    
    uint32_t getAttributesSize() const;
    void setAttributesSize(uint32_t num);
    const std::vector<AttributeHandle>& getAttributes() const;
    const AttributeHandle& getAttributes(uint32_t rank) const;
    AttributeHandle& getAttributes(uint32_t rank);
    void setAttributes(const AttributeHandle& newAttributes, uint32_t rank);
    void removeAttributes(uint32_t rank);
    
    const TransportType& getTransport() const;
    void setTransport(const TransportType& newTransport);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Change_Attribute_Transport_Type& msg);

protected:
    ObjectHandle object;
    std::vector<AttributeHandle> attributes;
    TransportType transport;
};

std::ostream& operator<<(std::ostream& os, const NM_Change_Attribute_Transport_Type& msg);


class CERTI_EXPORT NM_Change_Attribute_Order_Type : public NetworkMessage {
public:
    NM_Change_Attribute_Order_Type();
    virtual ~NM_Change_Attribute_Order_Type() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectHandle& getObject() const;
    void setObject(const ObjectHandle& newObject);
    
    uint32_t getAttributesSize() const;
    void setAttributesSize(uint32_t num);
    const std::vector<AttributeHandle>& getAttributes() const;
    const AttributeHandle& getAttributes(uint32_t rank) const;
    AttributeHandle& getAttributes(uint32_t rank);
    void setAttributes(const AttributeHandle& newAttributes, uint32_t rank);
    void removeAttributes(uint32_t rank);
    
    const OrderType& getOrder() const;
    void setOrder(const OrderType& newOrder);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Change_Attribute_Order_Type& msg);

protected:
    ObjectHandle object;
    std::vector<AttributeHandle> attributes;
    OrderType order;
};

std::ostream& operator<<(std::ostream& os, const NM_Change_Attribute_Order_Type& msg);


class CERTI_EXPORT NM_Change_Interaction_Transport_Type : public NetworkMessage {
public:
    NM_Change_Interaction_Transport_Type();
    virtual ~NM_Change_Interaction_Transport_Type() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const InteractionClassHandle& getInteractionClass() const;
    void setInteractionClass(const InteractionClassHandle& newInteractionClass);
    
    const TransportType& getTransport() const;
    void setTransport(const TransportType& newTransport);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Change_Interaction_Transport_Type& msg);

protected:
    InteractionClassHandle interactionClass;
    TransportType transport;
};

std::ostream& operator<<(std::ostream& os, const NM_Change_Interaction_Transport_Type& msg);


class CERTI_EXPORT NM_Change_Interaction_Order_Type : public NetworkMessage {
public:
    NM_Change_Interaction_Order_Type();
    virtual ~NM_Change_Interaction_Order_Type() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const InteractionClassHandle& getInteractionClass() const;
    void setInteractionClass(const InteractionClassHandle& newInteractionClass);
    
    const OrderType& getOrder() const;
    void setOrder(const OrderType& newOrder);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Change_Interaction_Order_Type& msg);

protected:
    InteractionClassHandle interactionClass;
    OrderType order;
};

std::ostream& operator<<(std::ostream& os, const NM_Change_Interaction_Order_Type& msg);


class CERTI_EXPORT NM_Request_Class_Attribute_Value_Update : public NetworkMessage {
public:
    NM_Request_Class_Attribute_Value_Update();
    virtual ~NM_Request_Class_Attribute_Value_Update() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectClassHandle& getObjectClass() const;
    void setObjectClass(const ObjectClassHandle& newObjectClass);
    
    uint32_t getAttributesSize() const;
    void setAttributesSize(uint32_t num);
    const std::vector<AttributeHandle>& getAttributes() const;
    const AttributeHandle& getAttributes(uint32_t rank) const;
    AttributeHandle& getAttributes(uint32_t rank);
    void setAttributes(const AttributeHandle& newAttributes, uint32_t rank);
    void removeAttributes(uint32_t rank);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Request_Class_Attribute_Value_Update& msg);

protected:
    ObjectClassHandle objectClass;
    std::vector<AttributeHandle> attributes;
};

std::ostream& operator<<(std::ostream& os, const NM_Request_Class_Attribute_Value_Update& msg);


class CERTI_EXPORT NM_Request_Object_Attribute_Value_Update : public NetworkMessage {
public:
    NM_Request_Object_Attribute_Value_Update();
    virtual ~NM_Request_Object_Attribute_Value_Update() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectHandle& getObject() const;
    void setObject(const ObjectHandle& newObject);
    
    uint32_t getAttributesSize() const;
    void setAttributesSize(uint32_t num);
    const std::vector<AttributeHandle>& getAttributes() const;
    const AttributeHandle& getAttributes(uint32_t rank) const;
    AttributeHandle& getAttributes(uint32_t rank);
    void setAttributes(const AttributeHandle& newAttributes, uint32_t rank);
    void removeAttributes(uint32_t rank);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Request_Object_Attribute_Value_Update& msg);

protected:
    ObjectHandle object;
    std::vector<AttributeHandle> attributes;
};

std::ostream& operator<<(std::ostream& os, const NM_Request_Object_Attribute_Value_Update& msg);


class CERTI_EXPORT NM_Is_Attribute_Owned_By_Federate : public NetworkMessage {
public:
    NM_Is_Attribute_Owned_By_Federate();
    virtual ~NM_Is_Attribute_Owned_By_Federate() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectHandle& getObject() const;
    void setObject(const ObjectHandle& newObject);
    
    const AttributeHandle& getAttribute() const;
    void setAttribute(const AttributeHandle& newAttribute);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Is_Attribute_Owned_By_Federate& msg);

protected:
    ObjectHandle object;
    AttributeHandle attribute;
};

std::ostream& operator<<(std::ostream& os, const NM_Is_Attribute_Owned_By_Federate& msg);


class CERTI_EXPORT NM_Query_Attribute_Ownership : public NetworkMessage {
public:
    NM_Query_Attribute_Ownership();
    virtual ~NM_Query_Attribute_Ownership() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectHandle& getObject() const;
    void setObject(const ObjectHandle& newObject);
    
    const AttributeHandle& getAttribute() const;
    void setAttribute(const AttributeHandle& newAttribute);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Query_Attribute_Ownership& msg);

protected:
    ObjectHandle object;
    AttributeHandle attribute;
};

std::ostream& operator<<(std::ostream& os, const NM_Query_Attribute_Ownership& msg);


class CERTI_EXPORT NM_Attribute_Is_Not_Owned : public NetworkMessage {
public:
    NM_Attribute_Is_Not_Owned();
    virtual ~NM_Attribute_Is_Not_Owned() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectHandle& getObject() const;
    void setObject(const ObjectHandle& newObject);
    
    const AttributeHandle& getAttribute() const;
    void setAttribute(const AttributeHandle& newAttribute);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Attribute_Is_Not_Owned& msg);

protected:
    ObjectHandle object;
    AttributeHandle attribute;
};

std::ostream& operator<<(std::ostream& os, const NM_Attribute_Is_Not_Owned& msg);


class CERTI_EXPORT NM_Inform_Attribute_Ownership : public NetworkMessage {
public:
    NM_Inform_Attribute_Ownership();
    virtual ~NM_Inform_Attribute_Ownership() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectHandle& getObject() const;
    void setObject(const ObjectHandle& newObject);
    
    const AttributeHandle& getAttribute() const;
    void setAttribute(const AttributeHandle& newAttribute);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Inform_Attribute_Ownership& msg);

protected:
    ObjectHandle object;
    AttributeHandle attribute;
};

std::ostream& operator<<(std::ostream& os, const NM_Inform_Attribute_Ownership& msg);


class CERTI_EXPORT NM_Attribute_Ownership_Base : public NetworkMessage {
public:
    NM_Attribute_Ownership_Base();
    virtual ~NM_Attribute_Ownership_Base() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectHandle& getObject() const;
    void setObject(const ObjectHandle& newObject);
    
    uint32_t getAttributesSize() const;
    void setAttributesSize(uint32_t num);
    const std::vector<AttributeHandle>& getAttributes() const;
    const AttributeHandle& getAttributes(uint32_t rank) const;
    AttributeHandle& getAttributes(uint32_t rank);
    void setAttributes(const AttributeHandle& newAttributes, uint32_t rank);
    void removeAttributes(uint32_t rank);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Attribute_Ownership_Base& msg);

protected:
    ObjectHandle object;
    std::vector<AttributeHandle> attributes;
};

std::ostream& operator<<(std::ostream& os, const NM_Attribute_Ownership_Base& msg);


class CERTI_EXPORT NM_Negotiated_Attribute_Ownership_Divestiture : public NM_Attribute_Ownership_Base {
public:
    NM_Negotiated_Attribute_Ownership_Divestiture();
    virtual ~NM_Negotiated_Attribute_Ownership_Divestiture() = default;
    
};


class CERTI_EXPORT NM_Attribute_Ownership_Acquisition_Notification : public NM_Attribute_Ownership_Base {
public:
    NM_Attribute_Ownership_Acquisition_Notification();
    virtual ~NM_Attribute_Ownership_Acquisition_Notification() = default;
    
};


class CERTI_EXPORT NM_Attribute_Ownership_Divestiture_Notification : public NM_Attribute_Ownership_Base {
public:
    NM_Attribute_Ownership_Divestiture_Notification();
    virtual ~NM_Attribute_Ownership_Divestiture_Notification() = default;
    
};


class CERTI_EXPORT NM_Request_Attribute_Ownership_Assumption : public NM_Attribute_Ownership_Base {
public:
    NM_Request_Attribute_Ownership_Assumption();
    virtual ~NM_Request_Attribute_Ownership_Assumption() = default;
    
};

// This is a base message which is used to 
// define several Attribute_Ownership_XXX
// since all these message contains the same fields
class CERTI_EXPORT NM_Attribute_Ownership_Unavailable : public NM_Attribute_Ownership_Base {
public:
    NM_Attribute_Ownership_Unavailable();
    virtual ~NM_Attribute_Ownership_Unavailable() = default;
    
};


class CERTI_EXPORT NM_Attribute_Ownership_Acquisition_If_Available : public NM_Attribute_Ownership_Base {
public:
    NM_Attribute_Ownership_Acquisition_If_Available();
    virtual ~NM_Attribute_Ownership_Acquisition_If_Available() = default;
    
};


class CERTI_EXPORT NM_Unconditional_Attribute_Ownership_Divestiture : public NM_Attribute_Ownership_Base {
public:
    NM_Unconditional_Attribute_Ownership_Divestiture();
    virtual ~NM_Unconditional_Attribute_Ownership_Divestiture() = default;
    
};


class CERTI_EXPORT NM_Attribute_Ownership_Acquisition : public NM_Attribute_Ownership_Base {
public:
    NM_Attribute_Ownership_Acquisition();
    virtual ~NM_Attribute_Ownership_Acquisition() = default;
    
};


class CERTI_EXPORT NM_Request_Attribute_Ownership_Release : public NM_Attribute_Ownership_Base {
public:
    NM_Request_Attribute_Ownership_Release();
    virtual ~NM_Request_Attribute_Ownership_Release() = default;
    
};


class CERTI_EXPORT NM_Cancel_Negotiated_Attribute_Ownership_Divestiture : public NM_Attribute_Ownership_Base {
public:
    NM_Cancel_Negotiated_Attribute_Ownership_Divestiture();
    virtual ~NM_Cancel_Negotiated_Attribute_Ownership_Divestiture() = default;
    
};


class CERTI_EXPORT NM_Attribute_Ownership_Release_Response : public NM_Attribute_Ownership_Base {
public:
    NM_Attribute_Ownership_Release_Response();
    virtual ~NM_Attribute_Ownership_Release_Response() = default;
    
};


class CERTI_EXPORT NM_Cancel_Attribute_Ownership_Acquisition : public NM_Attribute_Ownership_Base {
public:
    NM_Cancel_Attribute_Ownership_Acquisition();
    virtual ~NM_Cancel_Attribute_Ownership_Acquisition() = default;
    
};


class CERTI_EXPORT NM_Confirm_Attribute_Ownership_Acquisition_Cancellation : public NM_Attribute_Ownership_Base {
public:
    NM_Confirm_Attribute_Ownership_Acquisition_Cancellation();
    virtual ~NM_Confirm_Attribute_Ownership_Acquisition_Cancellation() = default;
    
};


class CERTI_EXPORT NM_DDM_Create_Region : public NetworkMessage {
public:
    NM_DDM_Create_Region();
    virtual ~NM_DDM_Create_Region() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    uint32_t getAttributesSize() const;
    void setAttributesSize(uint32_t num);
    const std::vector<AttributeHandle>& getAttributes() const;
    const AttributeHandle& getAttributes(uint32_t rank) const;
    AttributeHandle& getAttributes(uint32_t rank);
    void setAttributes(const AttributeHandle& newAttributes, uint32_t rank);
    void removeAttributes(uint32_t rank);
    
    const bool& getDDM_bool() const;
    void setDDM_bool(const bool& newDDM_bool);
    
    const SpaceHandle& getSpace() const;
    void setSpace(const SpaceHandle& newSpace);
    
    const RegionHandle& getRegion() const;
    void setRegion(const RegionHandle& newRegion);
    
    const uint32_t& getNbExtents() const;
    void setNbExtents(const uint32_t& newNbExtents);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_DDM_Create_Region& msg);

protected:
    std::vector<AttributeHandle> attributes;
    bool DDM_bool {false};
    SpaceHandle space;
    RegionHandle region;
    uint32_t nbExtents;// FIXME not sure it must be here
};

std::ostream& operator<<(std::ostream& os, const NM_DDM_Create_Region& msg);


class CERTI_EXPORT NM_DDM_Modify_Region : public NetworkMessage {
public:
    NM_DDM_Modify_Region();
    virtual ~NM_DDM_Modify_Region() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    uint32_t getAttributesSize() const;
    void setAttributesSize(uint32_t num);
    const std::vector<AttributeHandle>& getAttributes() const;
    const AttributeHandle& getAttributes(uint32_t rank) const;
    AttributeHandle& getAttributes(uint32_t rank);
    void setAttributes(const AttributeHandle& newAttributes, uint32_t rank);
    void removeAttributes(uint32_t rank);
    
    const bool& getDDM_bool() const;
    void setDDM_bool(const bool& newDDM_bool);
    
    const RegionHandle& getRegion() const;
    void setRegion(const RegionHandle& newRegion);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_DDM_Modify_Region& msg);

protected:
    std::vector<AttributeHandle> attributes;
    bool DDM_bool {false};
    RegionHandle region;
};

std::ostream& operator<<(std::ostream& os, const NM_DDM_Modify_Region& msg);


class CERTI_EXPORT NM_DDM_Delete_Region : public NetworkMessage {
public:
    NM_DDM_Delete_Region();
    virtual ~NM_DDM_Delete_Region() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const RegionHandle& getRegion() const;
    void setRegion(const RegionHandle& newRegion);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_DDM_Delete_Region& msg);

protected:
    RegionHandle region;
};

std::ostream& operator<<(std::ostream& os, const NM_DDM_Delete_Region& msg);


class CERTI_EXPORT NM_DDM_Associate_Region : public NetworkMessage {
public:
    NM_DDM_Associate_Region();
    virtual ~NM_DDM_Associate_Region() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    uint32_t getAttributesSize() const;
    void setAttributesSize(uint32_t num);
    const std::vector<AttributeHandle>& getAttributes() const;
    const AttributeHandle& getAttributes(uint32_t rank) const;
    AttributeHandle& getAttributes(uint32_t rank);
    void setAttributes(const AttributeHandle& newAttributes, uint32_t rank);
    void removeAttributes(uint32_t rank);
    
    const bool& getDDM_bool() const;
    void setDDM_bool(const bool& newDDM_bool);
    
    const RegionHandle& getRegion() const;
    void setRegion(const RegionHandle& newRegion);
    
    const ObjectHandle& getObject() const;
    void setObject(const ObjectHandle& newObject);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_DDM_Associate_Region& msg);

protected:
    std::vector<AttributeHandle> attributes;
    bool DDM_bool {false};
    RegionHandle region;
    ObjectHandle object;
};

std::ostream& operator<<(std::ostream& os, const NM_DDM_Associate_Region& msg);


class CERTI_EXPORT NM_DDM_Unassociate_Region : public NetworkMessage {
public:
    NM_DDM_Unassociate_Region();
    virtual ~NM_DDM_Unassociate_Region() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    uint32_t getAttributesSize() const;
    void setAttributesSize(uint32_t num);
    const std::vector<AttributeHandle>& getAttributes() const;
    const AttributeHandle& getAttributes(uint32_t rank) const;
    AttributeHandle& getAttributes(uint32_t rank);
    void setAttributes(const AttributeHandle& newAttributes, uint32_t rank);
    void removeAttributes(uint32_t rank);
    
    const RegionHandle& getRegion() const;
    void setRegion(const RegionHandle& newRegion);
    
    const ObjectHandle& getObject() const;
    void setObject(const ObjectHandle& newObject);
    
    const bool& getDDM_bool() const;
    void setDDM_bool(const bool& newDDM_bool);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_DDM_Unassociate_Region& msg);

protected:
    std::vector<AttributeHandle> attributes;
    RegionHandle region;
    ObjectHandle object;
    bool DDM_bool {false};
};

std::ostream& operator<<(std::ostream& os, const NM_DDM_Unassociate_Region& msg);


class CERTI_EXPORT NM_DDM_Register_Object : public NetworkMessage {
public:
    NM_DDM_Register_Object();
    virtual ~NM_DDM_Register_Object() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectClassHandle& getObjectClass() const;
    void setObjectClass(const ObjectClassHandle& newObjectClass);
    
    uint32_t getAttributesSize() const;
    void setAttributesSize(uint32_t num);
    const std::vector<AttributeHandle>& getAttributes() const;
    const AttributeHandle& getAttributes(uint32_t rank) const;
    AttributeHandle& getAttributes(uint32_t rank);
    void setAttributes(const AttributeHandle& newAttributes, uint32_t rank);
    void removeAttributes(uint32_t rank);
    
    const RegionHandle& getRegion() const;
    void setRegion(const RegionHandle& newRegion);
    
    const std::string& getObjectInstanceName() const;
    void setObjectInstanceName(const std::string& newObjectInstanceName);
    bool hasObjectInstanceName() const;
    
    const ObjectHandle& getObject() const;
    void setObject(const ObjectHandle& newObject);
    
    const bool& getDDM_bool() const;
    void setDDM_bool(const bool& newDDM_bool);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_DDM_Register_Object& msg);

protected:
    ObjectClassHandle objectClass;
    std::vector<AttributeHandle> attributes;
    RegionHandle region;
    std::string objectInstanceName;
    bool _hasObjectInstanceName {false};
    ObjectHandle object;
    bool DDM_bool {false};
};

std::ostream& operator<<(std::ostream& os, const NM_DDM_Register_Object& msg);


class CERTI_EXPORT NM_DDM_Subscribe_Attributes : public NetworkMessage {
public:
    NM_DDM_Subscribe_Attributes();
    virtual ~NM_DDM_Subscribe_Attributes() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectClassHandle& getObjectClass() const;
    void setObjectClass(const ObjectClassHandle& newObjectClass);
    
    const RegionHandle& getRegion() const;
    void setRegion(const RegionHandle& newRegion);
    
    uint32_t getAttributesSize() const;
    void setAttributesSize(uint32_t num);
    const std::vector<AttributeHandle>& getAttributes() const;
    const AttributeHandle& getAttributes(uint32_t rank) const;
    AttributeHandle& getAttributes(uint32_t rank);
    void setAttributes(const AttributeHandle& newAttributes, uint32_t rank);
    void removeAttributes(uint32_t rank);
    
    const bool& getDDM_bool() const;
    void setDDM_bool(const bool& newDDM_bool);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_DDM_Subscribe_Attributes& msg);

protected:
    ObjectClassHandle objectClass;
    RegionHandle region;
    std::vector<AttributeHandle> attributes;
    bool DDM_bool {false};
};

std::ostream& operator<<(std::ostream& os, const NM_DDM_Subscribe_Attributes& msg);


class CERTI_EXPORT NM_DDM_Unsubscribe_Attributes : public NetworkMessage {
public:
    NM_DDM_Unsubscribe_Attributes();
    virtual ~NM_DDM_Unsubscribe_Attributes() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectClassHandle& getObjectClass() const;
    void setObjectClass(const ObjectClassHandle& newObjectClass);
    
    const RegionHandle& getRegion() const;
    void setRegion(const RegionHandle& newRegion);
    
    uint32_t getAttributesSize() const;
    void setAttributesSize(uint32_t num);
    const std::vector<AttributeHandle>& getAttributes() const;
    const AttributeHandle& getAttributes(uint32_t rank) const;
    AttributeHandle& getAttributes(uint32_t rank);
    void setAttributes(const AttributeHandle& newAttributes, uint32_t rank);
    void removeAttributes(uint32_t rank);
    
    const bool& getDDM_bool() const;
    void setDDM_bool(const bool& newDDM_bool);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_DDM_Unsubscribe_Attributes& msg);

protected:
    ObjectClassHandle objectClass;
    RegionHandle region;
    std::vector<AttributeHandle> attributes;
    bool DDM_bool {false};
};

std::ostream& operator<<(std::ostream& os, const NM_DDM_Unsubscribe_Attributes& msg);


class CERTI_EXPORT NM_DDM_Subscribe_Interaction : public NetworkMessage {
public:
    NM_DDM_Subscribe_Interaction();
    virtual ~NM_DDM_Subscribe_Interaction() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const InteractionClassHandle& getInteractionClass() const;
    void setInteractionClass(const InteractionClassHandle& newInteractionClass);
    
    const RegionHandle& getRegion() const;
    void setRegion(const RegionHandle& newRegion);
    
    const bool& getDDM_bool() const;
    void setDDM_bool(const bool& newDDM_bool);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_DDM_Subscribe_Interaction& msg);

protected:
    InteractionClassHandle interactionClass;
    RegionHandle region;
    bool DDM_bool {false};
};

std::ostream& operator<<(std::ostream& os, const NM_DDM_Subscribe_Interaction& msg);


class CERTI_EXPORT NM_DDM_Unsubscribe_Interaction : public NM_DDM_Subscribe_Interaction {
public:
    NM_DDM_Unsubscribe_Interaction();
    virtual ~NM_DDM_Unsubscribe_Interaction() = default;
    
};


class CERTI_EXPORT NM_Provide_Attribute_Value_Update : public NM_Request_Object_Attribute_Value_Update {
public:
    NM_Provide_Attribute_Value_Update();
    virtual ~NM_Provide_Attribute_Value_Update() = default;
    
};


class CERTI_EXPORT NM_Start_Registration_For_Object_Class : public NetworkMessage {
public:
    NM_Start_Registration_For_Object_Class();
    virtual ~NM_Start_Registration_For_Object_Class() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectClassHandle& getObjectClass() const;
    void setObjectClass(const ObjectClassHandle& newObjectClass);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Start_Registration_For_Object_Class& msg);

protected:
    ObjectClassHandle objectClass;
};

std::ostream& operator<<(std::ostream& os, const NM_Start_Registration_For_Object_Class& msg);


class CERTI_EXPORT NM_Stop_Registration_For_Object_Class : public NetworkMessage {
public:
    NM_Stop_Registration_For_Object_Class();
    virtual ~NM_Stop_Registration_For_Object_Class() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const ObjectClassHandle& getObjectClass() const;
    void setObjectClass(const ObjectClassHandle& newObjectClass);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Stop_Registration_For_Object_Class& msg);

protected:
    ObjectClassHandle objectClass;
};

std::ostream& operator<<(std::ostream& os, const NM_Stop_Registration_For_Object_Class& msg);

// HLA 1516 - §6.2
class CERTI_EXPORT NM_Reserve_Object_Instance_Name : public NetworkMessage {
public:
    NM_Reserve_Object_Instance_Name();
    virtual ~NM_Reserve_Object_Instance_Name() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const std::string& getObjectName() const;
    void setObjectName(const std::string& newObjectName);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Reserve_Object_Instance_Name& msg);

protected:
    std::string objectName;
};

std::ostream& operator<<(std::ostream& os, const NM_Reserve_Object_Instance_Name& msg);

// HLA 1516 - §6.3
class CERTI_EXPORT NM_Reserve_Object_Instance_Name_Succeeded : public NM_Reserve_Object_Instance_Name {
public:
    NM_Reserve_Object_Instance_Name_Succeeded();
    virtual ~NM_Reserve_Object_Instance_Name_Succeeded() = default;
    
};


class CERTI_EXPORT NM_Reserve_Object_Instance_Name_Failed : public NM_Reserve_Object_Instance_Name {
public:
    NM_Reserve_Object_Instance_Name_Failed();
    virtual ~NM_Reserve_Object_Instance_Name_Failed() = default;
    
};


class CERTI_EXPORT NM_Message_Null_Prime : public NetworkMessage {
public:
    NM_Message_Null_Prime();
    virtual ~NM_Message_Null_Prime() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const FederationTime& getTimestamp() const;
    void setTimestamp(const FederationTime& newTimestamp);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Message_Null_Prime& msg);

protected:
    FederationTime timestamp;
};

std::ostream& operator<<(std::ostream& os, const NM_Message_Null_Prime& msg);


class CERTI_EXPORT NM_Enable_Asynchronous_Delivery : public NetworkMessage {
public:
    NM_Enable_Asynchronous_Delivery();
    virtual ~NM_Enable_Asynchronous_Delivery() = default;
    
};


class CERTI_EXPORT NM_Disable_Asynchronous_Delivery : public NetworkMessage {
public:
    NM_Disable_Asynchronous_Delivery();
    virtual ~NM_Disable_Asynchronous_Delivery() = default;
    
};


class CERTI_EXPORT NM_Time_Advance_Request : public NetworkMessage {
public:
    NM_Time_Advance_Request();
    virtual ~NM_Time_Advance_Request() = default;
    
};


class CERTI_EXPORT NM_Time_Advance_Request_Available : public NetworkMessage {
public:
    NM_Time_Advance_Request_Available();
    virtual ~NM_Time_Advance_Request_Available() = default;
    
};


class CERTI_EXPORT NM_Next_Message_Request : public NetworkMessage {
public:
    NM_Next_Message_Request();
    virtual ~NM_Next_Message_Request() = default;
    
};


class CERTI_EXPORT NM_Next_Message_Request_Available : public NetworkMessage {
public:
    NM_Next_Message_Request_Available();
    virtual ~NM_Next_Message_Request_Available() = default;
    
};


class CERTI_EXPORT NM_Mom_Status : public NetworkMessage {
public:
    NM_Mom_Status();
    virtual ~NM_Mom_Status() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const bool& getMomState() const;
    void setMomState(const bool& newMomState);
    
    const uint32_t& getUpdatePeriod() const;
    void setUpdatePeriod(const uint32_t& newUpdatePeriod);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Mom_Status& msg);

protected:
    bool momState;
    uint32_t updatePeriod;
};

std::ostream& operator<<(std::ostream& os, const NM_Mom_Status& msg);


class CERTI_EXPORT NM_Time_State_Update : public NetworkMessage {
public:
    NM_Time_State_Update();
    virtual ~NM_Time_State_Update() = default;
    
    virtual void serialize(libhla::MessageBuffer& msgBuffer);
    virtual void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const double& getLookahead() const;
    void setLookahead(const double& newLookahead);
    
    const bool& getState() const;
    void setState(const bool& newState);
    
    const double& getGalt() const;
    void setGalt(const double& newGalt);
    
    const double& getLits() const;
    void setLits(const double& newLits);
    
    using Super = NetworkMessage;
    friend std::ostream& operator<<(std::ostream& os, const NM_Time_State_Update& msg);

protected:
    double lookahead;
    bool state;
    double galt;
    double lits;
};

std::ostream& operator<<(std::ostream& os, const NM_Time_State_Update& msg);


class CERTI_EXPORT New_NetworkMessage {
public:
    New_NetworkMessage() = default;
    ~New_NetworkMessage() = default;
    
    void serialize(libhla::MessageBuffer& msgBuffer);
    void deserialize(libhla::MessageBuffer& msgBuffer);

    // Attributes accessors and mutators
    const uint32_t& getType() const;
    void setType(const uint32_t& newType);
    
    const FederationTime& getDate() const;
    void setDate(const FederationTime& newDate);
    bool hasDate() const;
    
    const std::string& getLabel() const;
    void setLabel(const std::string& newLabel);
    bool hasLabel() const;
    
    const std::string& getTag() const;
    void setTag(const std::string& newTag);
    bool hasTag() const;
    
    friend std::ostream& operator<<(std::ostream& os, const New_NetworkMessage& msg);

protected:
    uint32_t type {0};//required string          name  {default="MessageBaseClass"}
    FederationTime date;
    bool _hasDate {false};
    std::string label;
    bool _hasLabel {false};
    std::string tag;
    bool _hasTag {false};
};

std::ostream& operator<<(std::ostream& os, const New_NetworkMessage& msg);


class CERTI_EXPORT NM_Factory {
    public:
        static NetworkMessage* create(NM_Type type) throw (NetworkError ,NetworkSignal); 
        static NetworkMessage* receive(NMStreamType stream) throw (NetworkError ,NetworkSignal); 
    protected:
    private:
};

} // end of namespace certi 
// NM_CLASSES_HH
#endif
