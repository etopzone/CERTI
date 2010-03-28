// Generated on 2010 March Sun, 28 at 18:01:32 by the CERTI message generator
#ifndef NM_CLASSES_HH
#define NM_CLASSES_HH
// ****-**** Global System includes ****-****
#include <vector>
#include <string>
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

      typedef enum CERTI_NetworkMessage_MessageType {
         NOT_USED = 0, 
         NM_CLOSE_CONNEXION, 
         NM_MESSAGE_NULL, 
         NM_CREATE_FEDERATION_EXECUTION, 
         NM_DESTROY_FEDERATION_EXECUTION, 
         NM_JOIN_FEDERATION_EXECUTION, 
         NM_RESIGN_FEDERATION_EXECUTION, 
         NM_SET_TIME_REGULATING, 
         NM_SET_TIME_CONSTRAINED, 
         NM_TIME_REGULATION_ENABLED, 
         NM_TIME_CONSTRAINED_ENABLED, 
         NM_SET_CLASS_RELEVANCE_ADVISORY_SWITCH, 
         NM_SET_INTERACTION_RELEVANCE_ADVISORY_SWITCH, 
         NM_SET_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH, 
         NM_SET_ATTRIBUTE_SCOPE_ADVISORY_SWITCH, 
         NM_REGISTER_FEDERATION_SYNCHRONIZATION_POINT, 
         NM_SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED, 
         NM_ANNOUNCE_SYNCHRONIZATION_POINT, 
         NM_SYNCHRONIZATION_POINT_ACHIEVED, 
         NM_FEDERATION_SYNCHRONIZED, 
         NM_REQUEST_FEDERATION_SAVE, 
         NM_FEDERATE_SAVE_BEGUN, 
         NM_FEDERATE_SAVE_COMPLETE, 
         NM_FEDERATE_SAVE_NOT_COMPLETE, 
         NM_INITIATE_FEDERATE_SAVE, 
         NM_FEDERATION_SAVED, 
         NM_FEDERATION_NOT_SAVED, 
         NM_REQUEST_FEDERATION_RESTORE, 
         NM_FEDERATE_RESTORE_COMPLETE, 
         NM_FEDERATE_RESTORE_NOT_COMPLETE, 
         NM_REQUEST_FEDERATION_RESTORE_SUCCEEDED, 
         NM_REQUEST_FEDERATION_RESTORE_FAILED, 
         NM_FEDERATION_RESTORE_BEGUN, 
         NM_INITIATE_FEDERATE_RESTORE, 
         NM_FEDERATION_RESTORED, 
         NM_FEDERATION_NOT_RESTORED, 
         NM_PUBLISH_OBJECT_CLASS, 
         NM_UNPUBLISH_OBJECT_CLASS, 
         NM_PUBLISH_INTERACTION_CLASS, 
         NM_UNPUBLISH_INTERACTION_CLASS, 
         NM_SUBSCRIBE_OBJECT_CLASS, 
         NM_UNSUBSCRIBE_OBJECT_CLASS, 
         NM_SUBSCRIBE_INTERACTION_CLASS, 
         NM_UNSUBSCRIBE_INTERACTION_CLASS, 
         NM_TURN_INTERACTIONS_ON, 
         NM_TURN_INTERACTIONS_OFF, 
         NM_REGISTER_OBJECT, 
         NM_DISCOVER_OBJECT, 
         NM_UPDATE_ATTRIBUTE_VALUES, 
         NM_REFLECT_ATTRIBUTE_VALUES, 
         NM_SEND_INTERACTION, 
         NM_RECEIVE_INTERACTION, 
         NM_DELETE_OBJECT, 
         NM_REMOVE_OBJECT, 
         NM_CHANGE_ATTRIBUTE_TRANSPORT_TYPE, 
         NM_CHANGE_ATTRIBUTE_ORDER_TYPE, 
         NM_CHANGE_INTERACTION_TRANSPORT_TYPE, 
         NM_CHANGE_INTERACTION_ORDER_TYPE, 
         NM_REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE, 
         NM_REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE, 
         NM_IS_ATTRIBUTE_OWNED_BY_FEDERATE, 
         NM_QUERY_ATTRIBUTE_OWNERSHIP, 
         NM_ATTRIBUTE_IS_NOT_OWNED, 
         NM_INFORM_ATTRIBUTE_OWNERSHIP, 
         NM_ATTRIBUTE_OWNERSHIP_BASE, 
         NM_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE, 
         NM_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION, 
         NM_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION, 
         NM_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION, 
         NM_ATTRIBUTE_OWNERSHIP_UNAVAILABLE, 
         NM_ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE, 
         NM_UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE, 
         NM_ATTRIBUTE_OWNERSHIP_ACQUISITION, 
         NM_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE, 
         NM_CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE, 
         NM_ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE, 
         NM_CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION, 
         NM_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION, 
         NM_DDM_CREATE_REGION, 
         NM_DDM_MODIFY_REGION, 
         NM_DDM_DELETE_REGION, 
         NM_DDM_ASSOCIATE_REGION, 
         NM_DDM_UNASSOCIATE_REGION, 
         NM_DDM_REGISTER_OBJECT, 
         NM_DDM_SUBSCRIBE_ATTRIBUTES, 
         NM_DDM_UNSUBSCRIBE_ATTRIBUTES, 
         NM_DDM_SUBSCRIBE_INTERACTION, 
         NM_DDM_UNSUBSCRIBE_INTERACTION, 
         NM_PROVIDE_ATTRIBUTE_VALUE_UPDATE, 
         NM_START_REGISTRATION_FOR_OBJECT_CLASS, 
         NM_STOP_REGISTRATION_FOR_OBJECT_CLASS, 
         LAST 
      } CERTI_NetworkMessage_MessageType_t; //end of enum CERTI_NetworkMessage_MessageType 
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
         NM_FOM_Dimension();
         ~NM_FOM_Dimension();
         void serialize(MessageBuffer& msgBuffer);
         void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const DimensionHandle& getHandle() const {return handle;}
         void setHandle(const DimensionHandle& newHandle) {handle=newHandle;}
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         // the show method
         void show(std::ostream& out);
      protected:
         DimensionHandle handle;
         std::string name;
      private:
   };

   class CERTI_EXPORT NM_FOM_Routing_Space {
      public:
         NM_FOM_Routing_Space();
         ~NM_FOM_Routing_Space();
         void serialize(MessageBuffer& msgBuffer);
         void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const SpaceHandle& getSpace() const {return space;}
         void setSpace(const SpaceHandle& newSpace) {space=newSpace;}
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         uint32_t getDimensionsSize() const {return dimensions.size();}
         void setDimensionsSize(uint32_t num) {dimensions.resize(num);}
         const std::vector<NM_FOM_Dimension>& getDimensions() const {return dimensions;}
         const NM_FOM_Dimension& getDimensions(uint32_t rank) const {return dimensions[rank];}
         NM_FOM_Dimension& getDimensions(uint32_t rank) {return dimensions[rank];}
         void setDimensions(const NM_FOM_Dimension& newDimensions, uint32_t rank) {dimensions[rank]=newDimensions;}
         void removeDimensions(uint32_t rank) {dimensions.erase(dimensions.begin() + rank);}
         // the show method
         void show(std::ostream& out);
      protected:
         SpaceHandle space;
         std::string name;
         std::vector<NM_FOM_Dimension> dimensions;
      private:
   };

   class CERTI_EXPORT NM_FOM_Attribute {
      public:
         NM_FOM_Attribute();
         ~NM_FOM_Attribute();
         void serialize(MessageBuffer& msgBuffer);
         void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const AttributeHandle& getHandle() const {return handle;}
         void setHandle(const AttributeHandle& newHandle) {handle=newHandle;}
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const SpaceHandle& getSpaceHandle() const {return spaceHandle;}
         void setSpaceHandle(const SpaceHandle& newSpaceHandle) {spaceHandle=newSpaceHandle;}
         const OrderType& getOrder() const {return order;}
         void setOrder(const OrderType& newOrder) {order=newOrder;}
         const TransportType& getTransport() const {return transport;}
         void setTransport(const TransportType& newTransport) {transport=newTransport;}
         // the show method
         void show(std::ostream& out);
      protected:
         AttributeHandle handle;
         std::string name;
         SpaceHandle spaceHandle;
         OrderType order;
         TransportType transport;
      private:
   };

   class CERTI_EXPORT NM_FOM_Object_Class {
      public:
         NM_FOM_Object_Class();
         ~NM_FOM_Object_Class();
         void serialize(MessageBuffer& msgBuffer);
         void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getHandle() const {return handle;}
         void setHandle(const ObjectClassHandle& newHandle) {handle=newHandle;}
         const ObjectClassHandle& getSuperClass() const {return superClass;}
         void setSuperClass(const ObjectClassHandle& newSuperClass) {superClass=newSuperClass;}
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<NM_FOM_Attribute>& getAttributes() const {return attributes;}
         const NM_FOM_Attribute& getAttributes(uint32_t rank) const {return attributes[rank];}
         NM_FOM_Attribute& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const NM_FOM_Attribute& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         // the show method
         void show(std::ostream& out);
      protected:
         ObjectClassHandle handle;
         ObjectClassHandle superClass;
         std::string name;
         std::vector<NM_FOM_Attribute> attributes;
      private:
   };

   class CERTI_EXPORT NM_FOM_Parameter {
      public:
         NM_FOM_Parameter();
         ~NM_FOM_Parameter();
         void serialize(MessageBuffer& msgBuffer);
         void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ParameterHandle& getHandle() const {return handle;}
         void setHandle(const ParameterHandle& newHandle) {handle=newHandle;}
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         // the show method
         void show(std::ostream& out);
      protected:
         ParameterHandle handle;
         std::string name;
      private:
   };

   class CERTI_EXPORT NM_FOM_Interaction_Class {
      public:
         NM_FOM_Interaction_Class();
         ~NM_FOM_Interaction_Class();
         void serialize(MessageBuffer& msgBuffer);
         void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const InteractionClassHandle& getSuperClass() const {return superClass;}
         void setSuperClass(const InteractionClassHandle& newSuperClass) {superClass=newSuperClass;}
         const SpaceHandle& getSpace() const {return space;}
         void setSpace(const SpaceHandle& newSpace) {space=newSpace;}
         const OrderType& getOrder() const {return order;}
         void setOrder(const OrderType& newOrder) {order=newOrder;}
         const TransportType& getTransport() const {return transport;}
         void setTransport(const TransportType& newTransport) {transport=newTransport;}
         uint32_t getParametersSize() const {return parameters.size();}
         void setParametersSize(uint32_t num) {parameters.resize(num);}
         const std::vector<NM_FOM_Parameter>& getParameters() const {return parameters;}
         const NM_FOM_Parameter& getParameters(uint32_t rank) const {return parameters[rank];}
         NM_FOM_Parameter& getParameters(uint32_t rank) {return parameters[rank];}
         void setParameters(const NM_FOM_Parameter& newParameters, uint32_t rank) {parameters[rank]=newParameters;}
         void removeParameters(uint32_t rank) {parameters.erase(parameters.begin() + rank);}
         // the show method
         void show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
         std::string name;
         InteractionClassHandle superClass;
         SpaceHandle space;
         OrderType order;
         TransportType transport;
         std::vector<NM_FOM_Parameter> parameters;
      private:
   };

   class CERTI_EXPORT NM_Close_Connexion : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Close_Connexion();
         virtual ~NM_Close_Connexion();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Message_Null : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Message_Null();
         virtual ~NM_Message_Null();
      protected:
      private:
   };
   // Create the federation execution
   class CERTI_EXPORT NM_Create_Federation_Execution : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Create_Federation_Execution();
         virtual ~NM_Create_Federation_Execution();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getFederationName() const {return federationName;}
         void setFederationName(const std::string& newFederationName) {federationName=newFederationName;}
         const std::string& getFEDid() const {return FEDid;}
         void setFEDid(const std::string& newFEDid) {FEDid=newFEDid;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         std::string federationName;// the federation name
         std::string FEDid;// the Federation ID which is in fact a filename
      private:
   };
   // Destroy the federation execution
   class CERTI_EXPORT NM_Destroy_Federation_Execution : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Destroy_Federation_Execution();
         virtual ~NM_Destroy_Federation_Execution();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getFederationName() const {return federationName;}
         void setFederationName(const std::string& newFederationName) {federationName=newFederationName;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         std::string federationName;
      private:
   };
   // Join the federation
   class CERTI_EXPORT NM_Join_Federation_Execution : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Join_Federation_Execution();
         virtual ~NM_Join_Federation_Execution();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const int32_t& getNumberOfRegulators() const {return numberOfRegulators;}
         void setNumberOfRegulators(const int32_t& newNumberOfRegulators) {numberOfRegulators=newNumberOfRegulators;}
         const uint32_t& getMulticastAddress() const {return multicastAddress;}
         void setMulticastAddress(const uint32_t& newMulticastAddress) {multicastAddress=newMulticastAddress;}
         const uint32_t& getBestEffortAddress() const {return bestEffortAddress;}
         void setBestEffortAddress(const uint32_t& newBestEffortAddress) {bestEffortAddress=newBestEffortAddress;}
         const uint32_t& getBestEffortPeer() const {return bestEffortPeer;}
         void setBestEffortPeer(const uint32_t& newBestEffortPeer) {bestEffortPeer=newBestEffortPeer;}
         const std::string& getFederationName() const {return federationName;}
         void setFederationName(const std::string& newFederationName) {federationName=newFederationName;}
         const std::string& getFederateName() const {return federateName;}
         void setFederateName(const std::string& newFederateName) {federateName=newFederateName;}
         uint32_t getRoutingSpacesSize() const {return routingSpaces.size();}
         void setRoutingSpacesSize(uint32_t num) {routingSpaces.resize(num);}
         const std::vector<NM_FOM_Routing_Space>& getRoutingSpaces() const {return routingSpaces;}
         const NM_FOM_Routing_Space& getRoutingSpaces(uint32_t rank) const {return routingSpaces[rank];}
         NM_FOM_Routing_Space& getRoutingSpaces(uint32_t rank) {return routingSpaces[rank];}
         void setRoutingSpaces(const NM_FOM_Routing_Space& newRoutingSpaces, uint32_t rank) {routingSpaces[rank]=newRoutingSpaces;}
         void removeRoutingSpaces(uint32_t rank) {routingSpaces.erase(routingSpaces.begin() + rank);}
         uint32_t getObjectClassesSize() const {return objectClasses.size();}
         void setObjectClassesSize(uint32_t num) {objectClasses.resize(num);}
         const std::vector<NM_FOM_Object_Class>& getObjectClasses() const {return objectClasses;}
         const NM_FOM_Object_Class& getObjectClasses(uint32_t rank) const {return objectClasses[rank];}
         NM_FOM_Object_Class& getObjectClasses(uint32_t rank) {return objectClasses[rank];}
         void setObjectClasses(const NM_FOM_Object_Class& newObjectClasses, uint32_t rank) {objectClasses[rank]=newObjectClasses;}
         void removeObjectClasses(uint32_t rank) {objectClasses.erase(objectClasses.begin() + rank);}
         uint32_t getInteractionClassesSize() const {return interactionClasses.size();}
         void setInteractionClassesSize(uint32_t num) {interactionClasses.resize(num);}
         const std::vector<NM_FOM_Interaction_Class>& getInteractionClasses() const {return interactionClasses;}
         const NM_FOM_Interaction_Class& getInteractionClasses(uint32_t rank) const {return interactionClasses[rank];}
         NM_FOM_Interaction_Class& getInteractionClasses(uint32_t rank) {return interactionClasses[rank];}
         void setInteractionClasses(const NM_FOM_Interaction_Class& newInteractionClasses, uint32_t rank) {interactionClasses[rank]=newInteractionClasses;}
         void removeInteractionClasses(uint32_t rank) {interactionClasses.erase(interactionClasses.begin() + rank);}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         int32_t numberOfRegulators;
         uint32_t multicastAddress;
         uint32_t bestEffortAddress;
         uint32_t bestEffortPeer;
         std::string federationName;// the federation name
         std::string federateName;// the federate name (should be unique within a federation)
         std::vector<NM_FOM_Routing_Space> routingSpaces;
         std::vector<NM_FOM_Object_Class> objectClasses;
         std::vector<NM_FOM_Interaction_Class> interactionClasses;
      private:
   };
   // Resign from federation
   class CERTI_EXPORT NM_Resign_Federation_Execution : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Resign_Federation_Execution();
         virtual ~NM_Resign_Federation_Execution();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Set_Time_Regulating : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Set_Time_Regulating();
         virtual ~NM_Set_Time_Regulating();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         void regulatorOn() {regulator = true;}
         void regulatorOff() {regulator = false;}
         bool isRegulatorOn() const {return regulator;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         bool regulator;
      private:
   };

   class CERTI_EXPORT NM_Set_Time_Constrained : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Set_Time_Constrained();
         virtual ~NM_Set_Time_Constrained();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         void constrainedOn() {constrained = true;}
         void constrainedOff() {constrained = false;}
         bool isConstrainedOn() const {return constrained;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         bool constrained;
      private:
   };

   class CERTI_EXPORT NM_Time_Regulation_Enabled : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Time_Regulation_Enabled();
         virtual ~NM_Time_Regulation_Enabled();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Time_Constrained_Enabled : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Time_Constrained_Enabled();
         virtual ~NM_Time_Constrained_Enabled();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Set_Class_Relevance_Advisory_Switch : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Set_Class_Relevance_Advisory_Switch();
         virtual ~NM_Set_Class_Relevance_Advisory_Switch();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         void classRelevanceAdvisorySwitchOn() {classRelevanceAdvisorySwitch = true;}
         void classRelevanceAdvisorySwitchOff() {classRelevanceAdvisorySwitch = false;}
         bool isClassRelevanceAdvisorySwitchOn() const {return classRelevanceAdvisorySwitch;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         bool classRelevanceAdvisorySwitch;// value of CRA switch 	
      private:
   };

   class CERTI_EXPORT NM_Set_Interaction_Relevance_Advisory_Switch : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Set_Interaction_Relevance_Advisory_Switch();
         virtual ~NM_Set_Interaction_Relevance_Advisory_Switch();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         void interactionRelevanceAdvisorySwitchOn() {interactionRelevanceAdvisorySwitch = true;}
         void interactionRelevanceAdvisorySwitchOff() {interactionRelevanceAdvisorySwitch = false;}
         bool isInteractionRelevanceAdvisorySwitchOn() const {return interactionRelevanceAdvisorySwitch;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         bool interactionRelevanceAdvisorySwitch;// value of IRA switch 	
      private:
   };

   class CERTI_EXPORT NM_Set_Attribute_Relevance_Advisory_Switch : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Set_Attribute_Relevance_Advisory_Switch();
         virtual ~NM_Set_Attribute_Relevance_Advisory_Switch();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         void attributeRelevanceAdvisorySwitchOn() {attributeRelevanceAdvisorySwitch = true;}
         void attributeRelevanceAdvisorySwitchOff() {attributeRelevanceAdvisorySwitch = false;}
         bool isAttributeRelevanceAdvisorySwitchOn() const {return attributeRelevanceAdvisorySwitch;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         bool attributeRelevanceAdvisorySwitch;// value of ARA switch 	
      private:
   };

   class CERTI_EXPORT NM_Set_Attribute_Scope_Advisory_Switch : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Set_Attribute_Scope_Advisory_Switch();
         virtual ~NM_Set_Attribute_Scope_Advisory_Switch();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         void attributeScopeAdvisorySwitchOn() {attributeScopeAdvisorySwitch = true;}
         void attributeScopeAdvisorySwitchOff() {attributeScopeAdvisorySwitch = false;}
         bool isAttributeScopeAdvisorySwitchOn() const {return attributeScopeAdvisorySwitch;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         bool attributeScopeAdvisorySwitch;// value of ASA switch 	
      private:
   };

   class CERTI_EXPORT NM_Register_Federation_Synchronization_Point : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Register_Federation_Synchronization_Point();
         virtual ~NM_Register_Federation_Synchronization_Point();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const bool& getExists() const {return exists;}
         void setExists(const bool& newExists) {exists=newExists;}
         uint32_t getFederatesSize() const {return federates.size();}
         void setFederatesSize(uint32_t num) {federates.resize(num);}
         const std::vector<FederateHandle>& getFederates() const {return federates;}
         const FederateHandle& getFederates(uint32_t rank) const {return federates[rank];}
         FederateHandle& getFederates(uint32_t rank) {return federates[rank];}
         void setFederates(const FederateHandle& newFederates, uint32_t rank) {federates[rank]=newFederates;}
         void removeFederates(uint32_t rank) {federates.erase(federates.begin() + rank);}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         bool exists;// tell whether if the synchronization set is specified or not
         std::vector<FederateHandle> federates;// the federate set
      private:
   };

   class CERTI_EXPORT NM_Synchronization_Point_Registration_Succeeded : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Synchronization_Point_Registration_Succeeded();
         virtual ~NM_Synchronization_Point_Registration_Succeeded();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Announce_Synchronization_Point : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Announce_Synchronization_Point();
         virtual ~NM_Announce_Synchronization_Point();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Synchronization_Point_Achieved : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Synchronization_Point_Achieved();
         virtual ~NM_Synchronization_Point_Achieved();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Federation_Synchronized : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Federation_Synchronized();
         virtual ~NM_Federation_Synchronized();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Request_Federation_Save : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Request_Federation_Save();
         virtual ~NM_Request_Federation_Save();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Federate_Save_Begun : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Federate_Save_Begun();
         virtual ~NM_Federate_Save_Begun();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Federate_Save_Complete : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Federate_Save_Complete();
         virtual ~NM_Federate_Save_Complete();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Federate_Save_Not_Complete : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Federate_Save_Not_Complete();
         virtual ~NM_Federate_Save_Not_Complete();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Initiate_Federate_Save : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Initiate_Federate_Save();
         virtual ~NM_Initiate_Federate_Save();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Federation_Saved : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Federation_Saved();
         virtual ~NM_Federation_Saved();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Federation_Not_Saved : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Federation_Not_Saved();
         virtual ~NM_Federation_Not_Saved();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Request_Federation_Restore : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Request_Federation_Restore();
         virtual ~NM_Request_Federation_Restore();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Federate_Restore_Complete : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Federate_Restore_Complete();
         virtual ~NM_Federate_Restore_Complete();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Federate_Restore_Not_Complete : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Federate_Restore_Not_Complete();
         virtual ~NM_Federate_Restore_Not_Complete();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Request_Federation_Restore_Succeeded : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Request_Federation_Restore_Succeeded();
         virtual ~NM_Request_Federation_Restore_Succeeded();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Request_Federation_Restore_Failed : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Request_Federation_Restore_Failed();
         virtual ~NM_Request_Federation_Restore_Failed();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Federation_Restore_Begun : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Federation_Restore_Begun();
         virtual ~NM_Federation_Restore_Begun();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Initiate_Federate_Restore : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Initiate_Federate_Restore();
         virtual ~NM_Initiate_Federate_Restore();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Federation_Restored : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Federation_Restored();
         virtual ~NM_Federation_Restored();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Federation_Not_Restored : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Federation_Not_Restored();
         virtual ~NM_Federation_Not_Restored();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Publish_Object_Class : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Publish_Object_Class();
         virtual ~NM_Publish_Object_Class();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT NM_Unpublish_Object_Class : public NM_Publish_Object_Class {
      public:
         typedef NM_Publish_Object_Class Super;
         NM_Unpublish_Object_Class();
         virtual ~NM_Unpublish_Object_Class();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Publish_Interaction_Class : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Publish_Interaction_Class();
         virtual ~NM_Publish_Interaction_Class();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;// the interaction class
      private:
   };

   class CERTI_EXPORT NM_Unpublish_Interaction_Class : public NM_Publish_Interaction_Class {
      public:
         typedef NM_Publish_Interaction_Class Super;
         NM_Unpublish_Interaction_Class();
         virtual ~NM_Unpublish_Interaction_Class();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Subscribe_Object_Class : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Subscribe_Object_Class();
         virtual ~NM_Subscribe_Object_Class();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT NM_Unsubscribe_Object_Class : public NM_Subscribe_Object_Class {
      public:
         typedef NM_Subscribe_Object_Class Super;
         NM_Unsubscribe_Object_Class();
         virtual ~NM_Unsubscribe_Object_Class();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Subscribe_Interaction_Class : public NM_Publish_Interaction_Class {
      public:
         typedef NM_Publish_Interaction_Class Super;
         NM_Subscribe_Interaction_Class();
         virtual ~NM_Subscribe_Interaction_Class();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Unsubscribe_Interaction_Class : public NM_Publish_Interaction_Class {
      public:
         typedef NM_Publish_Interaction_Class Super;
         NM_Unsubscribe_Interaction_Class();
         virtual ~NM_Unsubscribe_Interaction_Class();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Turn_Interactions_On : public NM_Publish_Interaction_Class {
      public:
         typedef NM_Publish_Interaction_Class Super;
         NM_Turn_Interactions_On();
         virtual ~NM_Turn_Interactions_On();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Turn_Interactions_Off : public NM_Publish_Interaction_Class {
      public:
         typedef NM_Publish_Interaction_Class Super;
         NM_Turn_Interactions_Off();
         virtual ~NM_Turn_Interactions_Off();
      protected:
      private:
   };
   // HLA 1.3 §6.2
   class CERTI_EXPORT NM_Register_Object : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Register_Object();
         virtual ~NM_Register_Object();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const std::string& getObjectName() const {return objectName;}
         void setObjectName(const std::string& newObjectName) {
            _hasObjectName=true;
            objectName=newObjectName;
         }
         bool hasObjectName() {return _hasObjectName;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         std::string objectName;
         bool _hasObjectName;
         ObjectHandle object;
      private:
   };
   // HLA 1.3 §6.3
   class CERTI_EXPORT NM_Discover_Object : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Discover_Object();
         virtual ~NM_Discover_Object();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         ObjectHandle object;
      private:
   };
   // HLA 1.3 §6.4
   class CERTI_EXPORT NM_Update_Attribute_Values : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Update_Attribute_Values();
         virtual ~NM_Update_Attribute_Values();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         uint32_t getValuesSize() const {return values.size();}
         void setValuesSize(uint32_t num) {values.resize(num);}
         const std::vector<AttributeValue_t>& getValues() const {return values;}
         const AttributeValue_t& getValues(uint32_t rank) const {return values[rank];}
         AttributeValue_t& getValues(uint32_t rank) {return values[rank];}
         void setValues(const AttributeValue_t& newValues, uint32_t rank) {values[rank]=newValues;}
         void removeValues(uint32_t rank) {values.erase(values.begin() + rank);}
         const EventRetractionHandle& getEvent() const {return event;}
         void setEvent(const EventRetractionHandle& newEvent) {
            _hasEvent=true;
            event=newEvent;
         }
         bool hasEvent() {return _hasEvent;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
         std::vector<AttributeValue_t> values;
         EventRetractionHandle event;
         bool _hasEvent;
      private:
   };
   // HLA 1.3 §6.5
   class CERTI_EXPORT NM_Reflect_Attribute_Values : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Reflect_Attribute_Values();
         virtual ~NM_Reflect_Attribute_Values();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         uint32_t getValuesSize() const {return values.size();}
         void setValuesSize(uint32_t num) {values.resize(num);}
         const std::vector<AttributeValue_t>& getValues() const {return values;}
         const AttributeValue_t& getValues(uint32_t rank) const {return values[rank];}
         AttributeValue_t& getValues(uint32_t rank) {return values[rank];}
         void setValues(const AttributeValue_t& newValues, uint32_t rank) {values[rank]=newValues;}
         void removeValues(uint32_t rank) {values.erase(values.begin() + rank);}
         const EventRetractionHandle& getEvent() const {return event;}
         void setEvent(const EventRetractionHandle& newEvent) {
            _hasEvent=true;
            event=newEvent;
         }
         bool hasEvent() {return _hasEvent;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
         std::vector<AttributeValue_t> values;
         EventRetractionHandle event;
         bool _hasEvent;
      private:
   };
   // HLA 1.3 §6.6
   class CERTI_EXPORT NM_Send_Interaction : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Send_Interaction();
         virtual ~NM_Send_Interaction();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         uint32_t getParametersSize() const {return parameters.size();}
         void setParametersSize(uint32_t num) {parameters.resize(num);}
         const std::vector<ParameterHandle>& getParameters() const {return parameters;}
         const ParameterHandle& getParameters(uint32_t rank) const {return parameters[rank];}
         ParameterHandle& getParameters(uint32_t rank) {return parameters[rank];}
         void setParameters(const ParameterHandle& newParameters, uint32_t rank) {parameters[rank]=newParameters;}
         void removeParameters(uint32_t rank) {parameters.erase(parameters.begin() + rank);}
         uint32_t getValuesSize() const {return values.size();}
         void setValuesSize(uint32_t num) {values.resize(num);}
         const std::vector<ParameterValue_t>& getValues() const {return values;}
         const ParameterValue_t& getValues(uint32_t rank) const {return values[rank];}
         ParameterValue_t& getValues(uint32_t rank) {return values[rank];}
         void setValues(const ParameterValue_t& newValues, uint32_t rank) {values[rank]=newValues;}
         void removeValues(uint32_t rank) {values.erase(values.begin() + rank);}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
         std::vector<ParameterHandle> parameters;
         std::vector<ParameterValue_t> values;
         RegionHandle region;// FIXME check this....
      private:
   };
   // HLA 1.3 §6.7
   class CERTI_EXPORT NM_Receive_Interaction : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Receive_Interaction();
         virtual ~NM_Receive_Interaction();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         uint32_t getParametersSize() const {return parameters.size();}
         void setParametersSize(uint32_t num) {parameters.resize(num);}
         const std::vector<ParameterHandle>& getParameters() const {return parameters;}
         const ParameterHandle& getParameters(uint32_t rank) const {return parameters[rank];}
         ParameterHandle& getParameters(uint32_t rank) {return parameters[rank];}
         void setParameters(const ParameterHandle& newParameters, uint32_t rank) {parameters[rank]=newParameters;}
         void removeParameters(uint32_t rank) {parameters.erase(parameters.begin() + rank);}
         uint32_t getValuesSize() const {return values.size();}
         void setValuesSize(uint32_t num) {values.resize(num);}
         const std::vector<ParameterValue_t>& getValues() const {return values;}
         const ParameterValue_t& getValues(uint32_t rank) const {return values[rank];}
         ParameterValue_t& getValues(uint32_t rank) {return values[rank];}
         void setValues(const ParameterValue_t& newValues, uint32_t rank) {values[rank]=newValues;}
         void removeValues(uint32_t rank) {values.erase(values.begin() + rank);}
         const EventRetractionHandle& getEvent() const {return event;}
         void setEvent(const EventRetractionHandle& newEvent) {
            _hasEvent=true;
            event=newEvent;
         }
         bool hasEvent() {return _hasEvent;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
         std::vector<ParameterHandle> parameters;
         std::vector<ParameterValue_t> values;
         EventRetractionHandle event;
         bool _hasEvent;
      private:
   };
   // HLA 1.3 §6.8 Delete Object Instance
   class CERTI_EXPORT NM_Delete_Object : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Delete_Object();
         virtual ~NM_Delete_Object();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const EventRetractionHandle& getEvent() const {return event;}
         void setEvent(const EventRetractionHandle& newEvent) {
            _hasEvent=true;
            event=newEvent;
         }
         bool hasEvent() {return _hasEvent;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectHandle object;
         EventRetractionHandle event;
         bool _hasEvent;
      private:
   };
   // HLA 1.3 §6.9 Remove Object Instance
   class CERTI_EXPORT NM_Remove_Object : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Remove_Object();
         virtual ~NM_Remove_Object();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const EventRetractionHandle& getEvent() const {return event;}
         void setEvent(const EventRetractionHandle& newEvent) {
            _hasEvent=true;
            event=newEvent;
         }
         bool hasEvent() {return _hasEvent;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectHandle object;
         ObjectClassHandle objectClass;
         EventRetractionHandle event;
         bool _hasEvent;
      private:
   };

   class CERTI_EXPORT NM_Change_Attribute_Transport_Type : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Change_Attribute_Transport_Type();
         virtual ~NM_Change_Attribute_Transport_Type();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         const TransportType& getTransport() const {return transport;}
         void setTransport(const TransportType& newTransport) {transport=newTransport;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
         TransportType transport;
      private:
   };

   class CERTI_EXPORT NM_Change_Attribute_Order_Type : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Change_Attribute_Order_Type();
         virtual ~NM_Change_Attribute_Order_Type();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         const OrderType& getOrder() const {return order;}
         void setOrder(const OrderType& newOrder) {order=newOrder;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
         OrderType order;
      private:
   };

   class CERTI_EXPORT NM_Change_Interaction_Transport_Type : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Change_Interaction_Transport_Type();
         virtual ~NM_Change_Interaction_Transport_Type();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const TransportType& getTransport() const {return transport;}
         void setTransport(const TransportType& newTransport) {transport=newTransport;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
         TransportType transport;
      private:
   };

   class CERTI_EXPORT NM_Change_Interaction_Order_Type : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Change_Interaction_Order_Type();
         virtual ~NM_Change_Interaction_Order_Type();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const OrderType& getOrder() const {return order;}
         void setOrder(const OrderType& newOrder) {order=newOrder;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
         OrderType order;
      private:
   };

   class CERTI_EXPORT NM_Request_Class_Attribute_Value_Update : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Request_Class_Attribute_Value_Update();
         virtual ~NM_Request_Class_Attribute_Value_Update();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT NM_Request_Object_Attribute_Value_Update : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Request_Object_Attribute_Value_Update();
         virtual ~NM_Request_Object_Attribute_Value_Update();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT NM_Is_Attribute_Owned_By_Federate : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Is_Attribute_Owned_By_Federate();
         virtual ~NM_Is_Attribute_Owned_By_Federate();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectHandle object;
         AttributeHandle attribute;
      private:
   };

   class CERTI_EXPORT NM_Query_Attribute_Ownership : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Query_Attribute_Ownership();
         virtual ~NM_Query_Attribute_Ownership();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectHandle object;
         AttributeHandle attribute;
      private:
   };

   class CERTI_EXPORT NM_Attribute_Is_Not_Owned : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Attribute_Is_Not_Owned();
         virtual ~NM_Attribute_Is_Not_Owned();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectHandle object;
         AttributeHandle attribute;
      private:
   };

   class CERTI_EXPORT NM_Inform_Attribute_Ownership : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Inform_Attribute_Ownership();
         virtual ~NM_Inform_Attribute_Ownership();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectHandle object;
         AttributeHandle attribute;
      private:
   };

   class CERTI_EXPORT NM_Attribute_Ownership_Base : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Attribute_Ownership_Base();
         virtual ~NM_Attribute_Ownership_Base();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT NM_Negotiated_Attribute_Ownership_Divestiture : public NM_Attribute_Ownership_Base {
      public:
         typedef NM_Attribute_Ownership_Base Super;
         NM_Negotiated_Attribute_Ownership_Divestiture();
         virtual ~NM_Negotiated_Attribute_Ownership_Divestiture();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Attribute_Ownership_Acquisition_Notification : public NM_Attribute_Ownership_Base {
      public:
         typedef NM_Attribute_Ownership_Base Super;
         NM_Attribute_Ownership_Acquisition_Notification();
         virtual ~NM_Attribute_Ownership_Acquisition_Notification();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Attribute_Ownership_Divestiture_Notification : public NM_Attribute_Ownership_Base {
      public:
         typedef NM_Attribute_Ownership_Base Super;
         NM_Attribute_Ownership_Divestiture_Notification();
         virtual ~NM_Attribute_Ownership_Divestiture_Notification();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Request_Attribute_Ownership_Assumption : public NM_Attribute_Ownership_Base {
      public:
         typedef NM_Attribute_Ownership_Base Super;
         NM_Request_Attribute_Ownership_Assumption();
         virtual ~NM_Request_Attribute_Ownership_Assumption();
      protected:
      private:
   };
   // This is a base message which is used to 
   // define several Attribute_Ownership_XXX
   // since all these message contains the same fields
   class CERTI_EXPORT NM_Attribute_Ownership_Unavailable : public NM_Attribute_Ownership_Base {
      public:
         typedef NM_Attribute_Ownership_Base Super;
         NM_Attribute_Ownership_Unavailable();
         virtual ~NM_Attribute_Ownership_Unavailable();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Attribute_Ownership_Acquisition_If_Available : public NM_Attribute_Ownership_Base {
      public:
         typedef NM_Attribute_Ownership_Base Super;
         NM_Attribute_Ownership_Acquisition_If_Available();
         virtual ~NM_Attribute_Ownership_Acquisition_If_Available();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Unconditional_Attribute_Ownership_Divestiture : public NM_Attribute_Ownership_Base {
      public:
         typedef NM_Attribute_Ownership_Base Super;
         NM_Unconditional_Attribute_Ownership_Divestiture();
         virtual ~NM_Unconditional_Attribute_Ownership_Divestiture();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Attribute_Ownership_Acquisition : public NM_Attribute_Ownership_Base {
      public:
         typedef NM_Attribute_Ownership_Base Super;
         NM_Attribute_Ownership_Acquisition();
         virtual ~NM_Attribute_Ownership_Acquisition();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Request_Attribute_Ownership_Release : public NM_Attribute_Ownership_Base {
      public:
         typedef NM_Attribute_Ownership_Base Super;
         NM_Request_Attribute_Ownership_Release();
         virtual ~NM_Request_Attribute_Ownership_Release();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Cancel_Negotiated_Attribute_Ownership_Divestiture : public NM_Attribute_Ownership_Base {
      public:
         typedef NM_Attribute_Ownership_Base Super;
         NM_Cancel_Negotiated_Attribute_Ownership_Divestiture();
         virtual ~NM_Cancel_Negotiated_Attribute_Ownership_Divestiture();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Attribute_Ownership_Release_Response : public NM_Attribute_Ownership_Base {
      public:
         typedef NM_Attribute_Ownership_Base Super;
         NM_Attribute_Ownership_Release_Response();
         virtual ~NM_Attribute_Ownership_Release_Response();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Cancel_Attribute_Ownership_Acquisition : public NM_Attribute_Ownership_Base {
      public:
         typedef NM_Attribute_Ownership_Base Super;
         NM_Cancel_Attribute_Ownership_Acquisition();
         virtual ~NM_Cancel_Attribute_Ownership_Acquisition();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Confirm_Attribute_Ownership_Acquisition_Cancellation : public NM_Attribute_Ownership_Base {
      public:
         typedef NM_Attribute_Ownership_Base Super;
         NM_Confirm_Attribute_Ownership_Acquisition_Cancellation();
         virtual ~NM_Confirm_Attribute_Ownership_Acquisition_Cancellation();
      protected:
      private:
   };

   class CERTI_EXPORT NM_DDM_Create_Region : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_DDM_Create_Region();
         virtual ~NM_DDM_Create_Region();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         const bool& getDDM_bool() const {return DDM_bool;}
         void setDDM_bool(const bool& newDDM_bool) {DDM_bool=newDDM_bool;}
         const SpaceHandle& getSpace() const {return space;}
         void setSpace(const SpaceHandle& newSpace) {space=newSpace;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         const uint32_t& getNbExtents() const {return nbExtents;}
         void setNbExtents(const uint32_t& newNbExtents) {nbExtents=newNbExtents;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         std::vector<AttributeHandle> attributes;
         bool DDM_bool;
         SpaceHandle space;
         RegionHandle region;
         uint32_t nbExtents;// FIXME not sure it must be here
      private:
   };

   class CERTI_EXPORT NM_DDM_Modify_Region : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_DDM_Modify_Region();
         virtual ~NM_DDM_Modify_Region();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         const bool& getDDM_bool() const {return DDM_bool;}
         void setDDM_bool(const bool& newDDM_bool) {DDM_bool=newDDM_bool;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         std::vector<AttributeHandle> attributes;
         bool DDM_bool;
         RegionHandle region;
      private:
   };

   class CERTI_EXPORT NM_DDM_Delete_Region : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_DDM_Delete_Region();
         virtual ~NM_DDM_Delete_Region();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         RegionHandle region;
      private:
   };

   class CERTI_EXPORT NM_DDM_Associate_Region : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_DDM_Associate_Region();
         virtual ~NM_DDM_Associate_Region();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         const bool& getDDM_bool() const {return DDM_bool;}
         void setDDM_bool(const bool& newDDM_bool) {DDM_bool=newDDM_bool;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         std::vector<AttributeHandle> attributes;
         bool DDM_bool;
         RegionHandle region;
         ObjectHandle object;
      private:
   };

   class CERTI_EXPORT NM_DDM_Unassociate_Region : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_DDM_Unassociate_Region();
         virtual ~NM_DDM_Unassociate_Region();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const bool& getDDM_bool() const {return DDM_bool;}
         void setDDM_bool(const bool& newDDM_bool) {DDM_bool=newDDM_bool;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         std::vector<AttributeHandle> attributes;
         RegionHandle region;
         ObjectHandle object;
         bool DDM_bool;
      private:
   };

   class CERTI_EXPORT NM_DDM_Register_Object : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_DDM_Register_Object();
         virtual ~NM_DDM_Register_Object();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         const std::string& getObjectInstanceName() const {return objectInstanceName;}
         void setObjectInstanceName(const std::string& newObjectInstanceName) {
            _hasObjectInstanceName=true;
            objectInstanceName=newObjectInstanceName;
         }
         bool hasObjectInstanceName() {return _hasObjectInstanceName;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const bool& getDDM_bool() const {return DDM_bool;}
         void setDDM_bool(const bool& newDDM_bool) {DDM_bool=newDDM_bool;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         std::vector<AttributeHandle> attributes;
         RegionHandle region;
         std::string objectInstanceName;
         bool _hasObjectInstanceName;
         ObjectHandle object;
         bool DDM_bool;
      private:
   };

   class CERTI_EXPORT NM_DDM_Subscribe_Attributes : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_DDM_Subscribe_Attributes();
         virtual ~NM_DDM_Subscribe_Attributes();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         const bool& getDDM_bool() const {return DDM_bool;}
         void setDDM_bool(const bool& newDDM_bool) {DDM_bool=newDDM_bool;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         RegionHandle region;
         std::vector<AttributeHandle> attributes;
         bool DDM_bool;
      private:
   };

   class CERTI_EXPORT NM_DDM_Unsubscribe_Attributes : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_DDM_Unsubscribe_Attributes();
         virtual ~NM_DDM_Unsubscribe_Attributes();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         const bool& getDDM_bool() const {return DDM_bool;}
         void setDDM_bool(const bool& newDDM_bool) {DDM_bool=newDDM_bool;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         RegionHandle region;
         std::vector<AttributeHandle> attributes;
         bool DDM_bool;
      private:
   };

   class CERTI_EXPORT NM_DDM_Subscribe_Interaction : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_DDM_Subscribe_Interaction();
         virtual ~NM_DDM_Subscribe_Interaction();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         const bool& getDDM_bool() const {return DDM_bool;}
         void setDDM_bool(const bool& newDDM_bool) {DDM_bool=newDDM_bool;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
         RegionHandle region;
         bool DDM_bool;
      private:
   };

   class CERTI_EXPORT NM_DDM_Unsubscribe_Interaction : public NM_DDM_Subscribe_Interaction {
      public:
         typedef NM_DDM_Subscribe_Interaction Super;
         NM_DDM_Unsubscribe_Interaction();
         virtual ~NM_DDM_Unsubscribe_Interaction();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Provide_Attribute_Value_Update : public NM_Request_Object_Attribute_Value_Update {
      public:
         typedef NM_Request_Object_Attribute_Value_Update Super;
         NM_Provide_Attribute_Value_Update();
         virtual ~NM_Provide_Attribute_Value_Update();
      protected:
      private:
   };

   class CERTI_EXPORT NM_Start_Registration_For_Object_Class : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Start_Registration_For_Object_Class();
         virtual ~NM_Start_Registration_For_Object_Class();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
      private:
   };

   class CERTI_EXPORT NM_Stop_Registration_For_Object_Class : public NetworkMessage {
      public:
         typedef NetworkMessage Super;
         NM_Stop_Registration_For_Object_Class();
         virtual ~NM_Stop_Registration_For_Object_Class();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         // the show method
         virtual void show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
      private:
   };

   class CERTI_EXPORT New_NetworkMessage {
      public:
         New_NetworkMessage();
         ~New_NetworkMessage();
         void serialize(MessageBuffer& msgBuffer);
         void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const uint32_t& getType() const {return type;}
         void setType(const uint32_t& newType) {type=newType;}
         const FederationTime& getDate() const {return date;}
         void setDate(const FederationTime& newDate) {
            _hasDate=true;
            date=newDate;
         }
         bool hasDate() {return _hasDate;}
         const std::string& getLabel() const {return label;}
         void setLabel(const std::string& newLabel) {
            _hasLabel=true;
            label=newLabel;
         }
         bool hasLabel() {return _hasLabel;}
         const std::string& getTag() const {return tag;}
         void setTag(const std::string& newTag) {
            _hasTag=true;
            tag=newTag;
         }
         bool hasTag() {return _hasTag;}
         // the show method
         void show(std::ostream& out);
      protected:
         uint32_t type;//required string          name  {default="MessageBaseClass"}
         FederationTime date;
         bool _hasDate;
         std::string label;
         bool _hasLabel;
         std::string tag;
         bool _hasTag;
      private:
   };

   class CERTI_EXPORT NM_Factory {
      public:
         static NetworkMessage* create(NM_Type type) throw (NetworkError, NetworkSignal);
         static NetworkMessage* receive(NMStreamType stream) throw (NetworkError, NetworkSignal);
      protected:
      private:
   };

} // end of namespace certi 
// NM_CLASSES_HH
#endif
