// Generated on 2011 September Thu, 08 at 22:08:47 by the CERTI message generator
#ifndef M_CLASSES_HH
#define M_CLASSES_HH
// ****-**** Global System includes ****-****
#include <vector>
#include <string>
// ****-**** Includes coming from native types ****-****
// Message is the base class for
// message exchanged between RTIA and Federate (libRTI) AKA CERTI Message. 
// Every message which is a merge from Message will first include the content 
// of a Message
#include "Message.hh"

#include "certi.hh"

#include "Extent.hh"

#include "SocketUN.hh"
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

   // Native types has been defined:
   //     - by included headers (see above)
   //     - with typedef (see below [if any])

   typedef SocketUN* MStreamType;

   typedef Message::Type M_Type;
   namespace CERTI_Message {
      static const uint32_t versionMajor = 1;
      static const uint32_t versionMinor = 0;

   }
   // The EventRetraction is not inheriting from base "Message"   
// this is a plain message which may be used as field   
// in messages merging from "Message". As such   
// it won't appear in the generated factory method.   
// It's a convenient way to describe a structured native   
// message using "combine". 
   class CERTI_EXPORT EventRetraction {
      public:
         EventRetraction();
         ~EventRetraction();
         void serialize(libhla::MessageBuffer& msgBuffer);
         void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const FederateHandle& getSendingFederate() const {return sendingFederate;}
         void setSendingFederate(const FederateHandle& newSendingFederate) {sendingFederate=newSendingFederate;}
         const uint64_t& getSN() const {return SN;}
         void setSN(const uint64_t& newSN) {SN=newSN;}
         // the show method
         std::ostream& show(std::ostream& out);
      protected:
         FederateHandle sendingFederate;
         uint64_t SN;
      private:
   };
   // Connexion initialization message
   class CERTI_EXPORT M_Open_Connexion : public Message {
      public:
         typedef Message Super;
         M_Open_Connexion();
         virtual ~M_Open_Connexion();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const uint32_t& getVersionMajor() const {return versionMajor;}
         void setVersionMajor(const uint32_t& newVersionMajor) {versionMajor=newVersionMajor;}
         const uint32_t& getVersionMinor() const {return versionMinor;}
         void setVersionMinor(const uint32_t& newVersionMinor) {versionMinor=newVersionMinor;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         uint32_t versionMajor;
         uint32_t versionMinor;
      private:
   };
   // Closing connexion message
   class CERTI_EXPORT M_Close_Connexion : public Message {
      public:
         typedef Message Super;
         M_Close_Connexion();
         virtual ~M_Close_Connexion();
      protected:
      private:
   };

   class CERTI_EXPORT M_Create_Federation_Execution : public Message {
      public:
         typedef Message Super;
         M_Create_Federation_Execution();
         virtual ~M_Create_Federation_Execution();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getFederationName() const {return federationName;}
         void setFederationName(const std::string& newFederationName) {federationName=newFederationName;}
         const std::string& getFEDid() const {return FEDid;}
         void setFEDid(const std::string& newFEDid) {FEDid=newFEDid;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         std::string federationName;// the federation name
         std::string FEDid;// the Federation ID which is in fact a filename
      private:
   };

   class CERTI_EXPORT M_Destroy_Federation_Execution : public Message {
      public:
         typedef Message Super;
         M_Destroy_Federation_Execution();
         virtual ~M_Destroy_Federation_Execution();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getFederationName() const {return federationName;}
         void setFederationName(const std::string& newFederationName) {federationName=newFederationName;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         std::string federationName;
      private:
   };

   class CERTI_EXPORT M_Join_Federation_Execution : public Message {
      public:
         typedef Message Super;
         M_Join_Federation_Execution();
         virtual ~M_Join_Federation_Execution();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const FederateHandle& getFederate() const {return federate;}
         void setFederate(const FederateHandle& newFederate) {federate=newFederate;}
         const std::string& getFederationName() const {return federationName;}
         void setFederationName(const std::string& newFederationName) {federationName=newFederationName;}
         const std::string& getFederateName() const {return federateName;}
         void setFederateName(const std::string& newFederateName) {federateName=newFederateName;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         FederateHandle federate;
         std::string federationName;
         std::string federateName;
      private:
   };

   class CERTI_EXPORT M_Resign_Federation_Execution : public Message {
      public:
         typedef Message Super;
         M_Resign_Federation_Execution();
         virtual ~M_Resign_Federation_Execution();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ResignAction& getResignAction() const {return resignAction;}
         void setResignAction(const ResignAction& newResignAction) {resignAction=newResignAction;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ResignAction resignAction;
      private:
   };

   class CERTI_EXPORT M_Register_Federation_Synchronization_Point : public Message {
      public:
         typedef Message Super;
         M_Register_Federation_Synchronization_Point();
         virtual ~M_Register_Federation_Synchronization_Point();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         uint32_t getFederateSetSize() const {return federateSet.size();}
         void setFederateSetSize(uint32_t num) {federateSet.resize(num);}
         const std::vector<FederateHandle>& getFederateSet() const {return federateSet;}
         const FederateHandle& getFederateSet(uint32_t rank) const {return federateSet[rank];}
         FederateHandle& getFederateSet(uint32_t rank) {return federateSet[rank];}
         void setFederateSet(const FederateHandle& newFederateSet, uint32_t rank) {federateSet[rank]=newFederateSet;}
         void removeFederateSet(uint32_t rank) {federateSet.erase(federateSet.begin() + rank);}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         std::vector<FederateHandle> federateSet;// the set of Federate which will participate to this sync point.
      private:
   };

   class CERTI_EXPORT M_Synchronization_Point_Registration_Failed : public Message {
      public:
         typedef Message Super;
         M_Synchronization_Point_Registration_Failed();
         virtual ~M_Synchronization_Point_Registration_Failed();
      protected:
      private:
   };

   class CERTI_EXPORT M_Synchronization_Point_Registration_Succeeded : public Message {
      public:
         typedef Message Super;
         M_Synchronization_Point_Registration_Succeeded();
         virtual ~M_Synchronization_Point_Registration_Succeeded();
      protected:
      private:
   };

   class CERTI_EXPORT M_Announce_Synchronization_Point : public Message {
      public:
         typedef Message Super;
         M_Announce_Synchronization_Point();
         virtual ~M_Announce_Synchronization_Point();
      protected:
      private:
   };

   class CERTI_EXPORT M_Synchronization_Point_Achieved : public Message {
      public:
         typedef Message Super;
         M_Synchronization_Point_Achieved();
         virtual ~M_Synchronization_Point_Achieved();
      protected:
      private:
   };

   class CERTI_EXPORT M_Federation_Synchronized : public Message {
      public:
         typedef Message Super;
         M_Federation_Synchronized();
         virtual ~M_Federation_Synchronized();
      protected:
      private:
   };

   class CERTI_EXPORT M_Request_Federation_Save : public Message {
      public:
         typedef Message Super;
         M_Request_Federation_Save();
         virtual ~M_Request_Federation_Save();
      protected:
      private:
   };

   class CERTI_EXPORT M_Initiate_Federate_Save : public Message {
      public:
         typedef Message Super;
         M_Initiate_Federate_Save();
         virtual ~M_Initiate_Federate_Save();
      protected:
      private:
   };

   class CERTI_EXPORT M_Federate_Save_Begun : public Message {
      public:
         typedef Message Super;
         M_Federate_Save_Begun();
         virtual ~M_Federate_Save_Begun();
      protected:
      private:
   };

   class CERTI_EXPORT M_Federate_Save_Complete : public Message {
      public:
         typedef Message Super;
         M_Federate_Save_Complete();
         virtual ~M_Federate_Save_Complete();
      protected:
      private:
   };

   class CERTI_EXPORT M_Federate_Save_Not_Complete : public Message {
      public:
         typedef Message Super;
         M_Federate_Save_Not_Complete();
         virtual ~M_Federate_Save_Not_Complete();
      protected:
      private:
   };

   class CERTI_EXPORT M_Federation_Saved : public Message {
      public:
         typedef Message Super;
         M_Federation_Saved();
         virtual ~M_Federation_Saved();
      protected:
      private:
   };

   class CERTI_EXPORT M_Federation_Not_Saved : public Message {
      public:
         typedef Message Super;
         M_Federation_Not_Saved();
         virtual ~M_Federation_Not_Saved();
      protected:
      private:
   };

   class CERTI_EXPORT M_Request_Federation_Restore : public Message {
      public:
         typedef Message Super;
         M_Request_Federation_Restore();
         virtual ~M_Request_Federation_Restore();
      protected:
      private:
   };

   class CERTI_EXPORT M_Request_Federation_Restore_Failed : public Message {
      public:
         typedef Message Super;
         M_Request_Federation_Restore_Failed();
         virtual ~M_Request_Federation_Restore_Failed();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getReason() const {return reason;}
         void setReason(const std::string& newReason) {reason=newReason;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         std::string reason;
      private:
   };

   class CERTI_EXPORT M_Request_Federation_Restore_Succeeded : public Message {
      public:
         typedef Message Super;
         M_Request_Federation_Restore_Succeeded();
         virtual ~M_Request_Federation_Restore_Succeeded();
      protected:
      private:
   };

   class CERTI_EXPORT M_Initiate_Federate_Restore : public Message {
      public:
         typedef Message Super;
         M_Initiate_Federate_Restore();
         virtual ~M_Initiate_Federate_Restore();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const FederateHandle& getFederate() const {return federate;}
         void setFederate(const FederateHandle& newFederate) {federate=newFederate;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         FederateHandle federate;
      private:
   };

   class CERTI_EXPORT M_Federate_Restore_Complete : public Message {
      public:
         typedef Message Super;
         M_Federate_Restore_Complete();
         virtual ~M_Federate_Restore_Complete();
      protected:
      private:
   };

   class CERTI_EXPORT M_Federate_Restore_Not_Complete : public Message {
      public:
         typedef Message Super;
         M_Federate_Restore_Not_Complete();
         virtual ~M_Federate_Restore_Not_Complete();
      protected:
      private:
   };

   class CERTI_EXPORT M_Federation_Restored : public Message {
      public:
         typedef Message Super;
         M_Federation_Restored();
         virtual ~M_Federation_Restored();
      protected:
      private:
   };

   class CERTI_EXPORT M_Federation_Not_Restored : public Message {
      public:
         typedef Message Super;
         M_Federation_Not_Restored();
         virtual ~M_Federation_Not_Restored();
      protected:
      private:
   };

   class CERTI_EXPORT M_Federation_Restore_Begun : public Message {
      public:
         typedef Message Super;
         M_Federation_Restore_Begun();
         virtual ~M_Federation_Restore_Begun();
      protected:
      private:
   };
   // Declaration
   class CERTI_EXPORT M_Publish_Object_Class : public Message {
      public:
         typedef Message Super;
         M_Publish_Object_Class();
         virtual ~M_Publish_Object_Class();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Unpublish_Object_Class : public Message {
      public:
         typedef Message Super;
         M_Unpublish_Object_Class();
         virtual ~M_Unpublish_Object_Class();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
      private:
   };

   class CERTI_EXPORT M_Publish_Interaction_Class : public Message {
      public:
         typedef Message Super;
         M_Publish_Interaction_Class();
         virtual ~M_Publish_Interaction_Class();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
      private:
   };

   class CERTI_EXPORT M_Unpublish_Interaction_Class : public Message {
      public:
         typedef Message Super;
         M_Unpublish_Interaction_Class();
         virtual ~M_Unpublish_Interaction_Class();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
      private:
   };

   class CERTI_EXPORT M_Subscribe_Object_Class_Attributes : public Message {
      public:
         typedef Message Super;
         M_Subscribe_Object_Class_Attributes();
         virtual ~M_Subscribe_Object_Class_Attributes();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         const bool& getActive() const {return active;}
         void setActive(const bool& newActive) {active=newActive;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         std::vector<AttributeHandle> attributes;
         bool active;
      private:
   };

   class CERTI_EXPORT M_Unsubscribe_Object_Class : public Message {
      public:
         typedef Message Super;
         M_Unsubscribe_Object_Class();
         virtual ~M_Unsubscribe_Object_Class();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
      private:
   };

   class CERTI_EXPORT M_Subscribe_Interaction_Class : public Message {
      public:
         typedef Message Super;
         M_Subscribe_Interaction_Class();
         virtual ~M_Subscribe_Interaction_Class();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
      private:
   };

   class CERTI_EXPORT M_Unsubscribe_Interaction_Class : public Message {
      public:
         typedef Message Super;
         M_Unsubscribe_Interaction_Class();
         virtual ~M_Unsubscribe_Interaction_Class();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
      private:
   };

   class CERTI_EXPORT M_Start_Registration_For_Object_Class : public Message {
      public:
         typedef Message Super;
         M_Start_Registration_For_Object_Class();
         virtual ~M_Start_Registration_For_Object_Class();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
      private:
   };

   class CERTI_EXPORT M_Stop_Registration_For_Object_Class : public Message {
      public:
         typedef Message Super;
         M_Stop_Registration_For_Object_Class();
         virtual ~M_Stop_Registration_For_Object_Class();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
      private:
   };

   class CERTI_EXPORT M_Turn_Interactions_On : public Message {
      public:
         typedef Message Super;
         M_Turn_Interactions_On();
         virtual ~M_Turn_Interactions_On();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
      private:
   };

   class CERTI_EXPORT M_Turn_Interactions_Off : public Message {
      public:
         typedef Message Super;
         M_Turn_Interactions_Off();
         virtual ~M_Turn_Interactions_Off();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
      private:
   };
   // Object
   class CERTI_EXPORT M_Register_Object_Instance : public Message {
      public:
         typedef Message Super;
         M_Register_Object_Instance();
         virtual ~M_Register_Object_Instance();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const std::string& getObjectName() const {return objectName;}
         void setObjectName(const std::string& newObjectName) {
            _hasObjectName=true;
            objectName=newObjectName;
         }
         bool hasObjectName() {return _hasObjectName;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         ObjectHandle object;
         std::string objectName;
         bool _hasObjectName;
      private:
   };

   class CERTI_EXPORT M_Update_Attribute_Values : public Message {
      public:
         typedef Message Super;
         M_Update_Attribute_Values();
         virtual ~M_Update_Attribute_Values();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
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
         const EventRetraction& getEventRetraction() const {return eventRetraction;}
         void setEventRetraction(const EventRetraction& newEventRetraction) {
            _hasEventRetraction=true;
            eventRetraction=newEventRetraction;
         }
         bool hasEventRetraction() {return _hasEventRetraction;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
         std::vector<AttributeValue_t> values;
         EventRetraction eventRetraction;
         bool _hasEventRetraction;
      private:
   };

   class CERTI_EXPORT M_Discover_Object_Instance : public Message {
      public:
         typedef Message Super;
         M_Discover_Object_Instance();
         virtual ~M_Discover_Object_Instance();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const std::string& getObjectName() const {return objectName;}
         void setObjectName(const std::string& newObjectName) {objectName=newObjectName;}
         const EventRetraction& getEventRetraction() const {return eventRetraction;}
         void setEventRetraction(const EventRetraction& newEventRetraction) {
            _hasEventRetraction=true;
            eventRetraction=newEventRetraction;
         }
         bool hasEventRetraction() {return _hasEventRetraction;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         ObjectHandle object;
         std::string objectName;
         EventRetraction eventRetraction;
         bool _hasEventRetraction;
      private:
   };

   class CERTI_EXPORT M_Reflect_Attribute_Values : public Message {
      public:
         typedef Message Super;
         M_Reflect_Attribute_Values();
         virtual ~M_Reflect_Attribute_Values();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
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
         const EventRetraction& getEventRetraction() const {return eventRetraction;}
         void setEventRetraction(const EventRetraction& newEventRetraction) {
            _hasEventRetraction=true;
            eventRetraction=newEventRetraction;
         }
         bool hasEventRetraction() {return _hasEventRetraction;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
         std::vector<AttributeValue_t> values;
         EventRetraction eventRetraction;
         bool _hasEventRetraction;
      private:
   };

   class CERTI_EXPORT M_Send_Interaction : public Message {
      public:
         typedef Message Super;
         M_Send_Interaction();
         virtual ~M_Send_Interaction();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         const EventRetraction& getEventRetraction() const {return eventRetraction;}
         void setEventRetraction(const EventRetraction& newEventRetraction) {
            _hasEventRetraction=true;
            eventRetraction=newEventRetraction;
         }
         bool hasEventRetraction() {return _hasEventRetraction;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
         std::vector<ParameterHandle> parameters;
         std::vector<ParameterValue_t> values;
         RegionHandle region;
         EventRetraction eventRetraction;
         bool _hasEventRetraction;
      private:
   };

   class CERTI_EXPORT M_Receive_Interaction : public Message {
      public:
         typedef Message Super;
         M_Receive_Interaction();
         virtual ~M_Receive_Interaction();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         const EventRetraction& getEventRetraction() const {return eventRetraction;}
         void setEventRetraction(const EventRetraction& newEventRetraction) {
            _hasEventRetraction=true;
            eventRetraction=newEventRetraction;
         }
         bool hasEventRetraction() {return _hasEventRetraction;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
         std::vector<ParameterHandle> parameters;
         std::vector<ParameterValue_t> values;
         RegionHandle region;
         EventRetraction eventRetraction;
         bool _hasEventRetraction;
      private:
   };

   class CERTI_EXPORT M_Delete_Object_Instance : public Message {
      public:
         typedef Message Super;
         M_Delete_Object_Instance();
         virtual ~M_Delete_Object_Instance();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const std::string& getObjectName() const {return objectName;}
         void setObjectName(const std::string& newObjectName) {objectName=newObjectName;}
         const EventRetraction& getEventRetraction() const {return eventRetraction;}
         void setEventRetraction(const EventRetraction& newEventRetraction) {
            _hasEventRetraction=true;
            eventRetraction=newEventRetraction;
         }
         bool hasEventRetraction() {return _hasEventRetraction;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         ObjectHandle object;
         std::string objectName;
         EventRetraction eventRetraction;
         bool _hasEventRetraction;
      private:
   };

   class CERTI_EXPORT M_Local_Delete_Object_Instance : public Message {
      public:
         typedef Message Super;
         M_Local_Delete_Object_Instance();
         virtual ~M_Local_Delete_Object_Instance();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
      private:
   };

   class CERTI_EXPORT M_Remove_Object_Instance : public Message {
      public:
         typedef Message Super;
         M_Remove_Object_Instance();
         virtual ~M_Remove_Object_Instance();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const std::string& getObjectName() const {return objectName;}
         void setObjectName(const std::string& newObjectName) {objectName=newObjectName;}
         const EventRetraction& getEventRetraction() const {return eventRetraction;}
         void setEventRetraction(const EventRetraction& newEventRetraction) {
            _hasEventRetraction=true;
            eventRetraction=newEventRetraction;
         }
         bool hasEventRetraction() {return _hasEventRetraction;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         ObjectHandle object;
         std::string objectName;
         EventRetraction eventRetraction;
         bool _hasEventRetraction;
      private:
   };

   class CERTI_EXPORT M_Change_Attribute_Transportation_Type : public Message {
      public:
         typedef Message Super;
         M_Change_Attribute_Transportation_Type();
         virtual ~M_Change_Attribute_Transportation_Type();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const TransportType& getTransportationType() const {return transportationType;}
         void setTransportationType(const TransportType& newTransportationType) {transportationType=newTransportationType;}
         const OrderType& getOrderType() const {return orderType;}
         void setOrderType(const OrderType& newOrderType) {orderType=newOrderType;}
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         TransportType transportationType;
         OrderType orderType;
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Change_Interaction_Transportation_Type : public Message {
      public:
         typedef Message Super;
         M_Change_Interaction_Transportation_Type();
         virtual ~M_Change_Interaction_Transportation_Type();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const TransportType& getTransportationType() const {return transportationType;}
         void setTransportationType(const TransportType& newTransportationType) {transportationType=newTransportationType;}
         const OrderType& getOrderType() const {return orderType;}
         void setOrderType(const OrderType& newOrderType) {orderType=newOrderType;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
         TransportType transportationType;
         OrderType orderType;
      private:
   };

   class CERTI_EXPORT M_Request_Object_Attribute_Value_Update : public Message {
      public:
         typedef Message Super;
         M_Request_Object_Attribute_Value_Update();
         virtual ~M_Request_Object_Attribute_Value_Update();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Request_Class_Attribute_Value_Update : public Message {
      public:
         typedef Message Super;
         M_Request_Class_Attribute_Value_Update();
         virtual ~M_Request_Class_Attribute_Value_Update();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Provide_Attribute_Value_Update : public Message {
      public:
         typedef Message Super;
         M_Provide_Attribute_Value_Update();
         virtual ~M_Provide_Attribute_Value_Update();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Attributes_In_Scope : public Message {
      public:
         typedef Message Super;
         M_Attributes_In_Scope();
         virtual ~M_Attributes_In_Scope();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Attributes_Out_Of_Scope : public Message {
      public:
         typedef Message Super;
         M_Attributes_Out_Of_Scope();
         virtual ~M_Attributes_Out_Of_Scope();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Turn_Updates_On_For_Object_Instance : public Message {
      public:
         typedef Message Super;
         M_Turn_Updates_On_For_Object_Instance();
         virtual ~M_Turn_Updates_On_For_Object_Instance();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Turn_Updates_Off_For_Object_Instance : public Message {
      public:
         typedef Message Super;
         M_Turn_Updates_Off_For_Object_Instance();
         virtual ~M_Turn_Updates_Off_For_Object_Instance();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };
   // Ownership management
   class CERTI_EXPORT M_Request_Attribute_Ownership_Divestiture : public Message {
      public:
         typedef Message Super;
         M_Request_Attribute_Ownership_Divestiture();
         virtual ~M_Request_Attribute_Ownership_Divestiture();
      protected:
      private:
   };

   class CERTI_EXPORT M_Request_Attribute_Ownership_Assumption : public Message {
      public:
         typedef Message Super;
         M_Request_Attribute_Ownership_Assumption();
         virtual ~M_Request_Attribute_Ownership_Assumption();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Negotiated_Attribute_Ownership_Divestiture : public Message {
      public:
         typedef Message Super;
         M_Negotiated_Attribute_Ownership_Divestiture();
         virtual ~M_Negotiated_Attribute_Ownership_Divestiture();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Attribute_Ownership_Divestiture_Notification : public Message {
      public:
         typedef Message Super;
         M_Attribute_Ownership_Divestiture_Notification();
         virtual ~M_Attribute_Ownership_Divestiture_Notification();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Attribute_Ownership_Acquisition_Notification : public Message {
      public:
         typedef Message Super;
         M_Attribute_Ownership_Acquisition_Notification();
         virtual ~M_Attribute_Ownership_Acquisition_Notification();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Request_Attribute_Ownership_Acquisition : public Message {
      public:
         typedef Message Super;
         M_Request_Attribute_Ownership_Acquisition();
         virtual ~M_Request_Attribute_Ownership_Acquisition();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Request_Attribute_Ownership_Release : public Message {
      public:
         typedef Message Super;
         M_Request_Attribute_Ownership_Release();
         virtual ~M_Request_Attribute_Ownership_Release();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Query_Attribute_Ownership : public Message {
      public:
         typedef Message Super;
         M_Query_Attribute_Ownership();
         virtual ~M_Query_Attribute_Ownership();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         AttributeHandle attribute;
      private:
   };

   class CERTI_EXPORT M_Inform_Attribute_Ownership : public Message {
      public:
         typedef Message Super;
         M_Inform_Attribute_Ownership();
         virtual ~M_Inform_Attribute_Ownership();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
         const FederateHandle& getFederate() const {return federate;}
         void setFederate(const FederateHandle& newFederate) {federate=newFederate;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         AttributeHandle attribute;
         FederateHandle federate;
      private:
   };

   class CERTI_EXPORT M_Is_Attribute_Owned_By_Federate : public Message {
      public:
         typedef Message Super;
         M_Is_Attribute_Owned_By_Federate();
         virtual ~M_Is_Attribute_Owned_By_Federate();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         AttributeHandle attribute;
      private:
   };

   class CERTI_EXPORT M_Attribute_Is_Not_Owned : public Message {
      public:
         typedef Message Super;
         M_Attribute_Is_Not_Owned();
         virtual ~M_Attribute_Is_Not_Owned();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
         const FederateHandle& getFederate() const {return federate;}
         void setFederate(const FederateHandle& newFederate) {federate=newFederate;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         AttributeHandle attribute;
         FederateHandle federate;
      private:
   };

   class CERTI_EXPORT M_Attribute_Owned_By_Rti : public Message {
      public:
         typedef Message Super;
         M_Attribute_Owned_By_Rti();
         virtual ~M_Attribute_Owned_By_Rti();
      protected:
      private:
   };

   class CERTI_EXPORT M_Attribute_Ownership_Acquisition_If_Available : public Message {
      public:
         typedef Message Super;
         M_Attribute_Ownership_Acquisition_If_Available();
         virtual ~M_Attribute_Ownership_Acquisition_If_Available();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Attribute_Ownership_Unavailable : public Message {
      public:
         typedef Message Super;
         M_Attribute_Ownership_Unavailable();
         virtual ~M_Attribute_Ownership_Unavailable();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Unconditional_Attribute_Ownership_Divestiture : public Message {
      public:
         typedef Message Super;
         M_Unconditional_Attribute_Ownership_Divestiture();
         virtual ~M_Unconditional_Attribute_Ownership_Divestiture();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Attribute_Ownership_Acquisition : public Message {
      public:
         typedef Message Super;
         M_Attribute_Ownership_Acquisition();
         virtual ~M_Attribute_Ownership_Acquisition();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Cancel_Negotiated_Attribute_Ownership_Divestiture : public Message {
      public:
         typedef Message Super;
         M_Cancel_Negotiated_Attribute_Ownership_Divestiture();
         virtual ~M_Cancel_Negotiated_Attribute_Ownership_Divestiture();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Attribute_Ownership_Release_Response : public Message {
      public:
         typedef Message Super;
         M_Attribute_Ownership_Release_Response();
         virtual ~M_Attribute_Ownership_Release_Response();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Cancel_Attribute_Ownership_Acquisition : public Message {
      public:
         typedef Message Super;
         M_Cancel_Attribute_Ownership_Acquisition();
         virtual ~M_Cancel_Attribute_Ownership_Acquisition();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Confirm_Attribute_Ownership_Acquisition_Cancellation : public Message {
      public:
         typedef Message Super;
         M_Confirm_Attribute_Ownership_Acquisition_Cancellation();
         virtual ~M_Confirm_Attribute_Ownership_Acquisition_Cancellation();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };
   // Time
   class CERTI_EXPORT M_Change_Attribute_Order_Type : public Message {
      public:
         typedef Message Super;
         M_Change_Attribute_Order_Type();
         virtual ~M_Change_Attribute_Order_Type();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const TransportType& getTransport() const {return transport;}
         void setTransport(const TransportType& newTransport) {transport=newTransport;}
         const OrderType& getOrder() const {return order;}
         void setOrder(const OrderType& newOrder) {order=newOrder;}
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
         virtual std::ostream& show(std::ostream& out);
      protected:
         TransportType transport;
         OrderType order;
         ObjectHandle object;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Change_Interaction_Order_Type : public Message {
      public:
         typedef Message Super;
         M_Change_Interaction_Order_Type();
         virtual ~M_Change_Interaction_Order_Type();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const TransportType& getTransport() const {return transport;}
         void setTransport(const TransportType& newTransport) {transport=newTransport;}
         const OrderType& getOrder() const {return order;}
         void setOrder(const OrderType& newOrder) {order=newOrder;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
         TransportType transport;
         OrderType order;
      private:
   };
   // HLA 1.3 - §8.2
   class CERTI_EXPORT M_Enable_Time_Regulation : public Message {
      public:
         typedef Message Super;
         M_Enable_Time_Regulation();
         virtual ~M_Enable_Time_Regulation();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const double& getLookahead() const {return lookahead;}
         void setLookahead(const double& newLookahead) {lookahead=newLookahead;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         double lookahead;
      private:
   };
   // HLA 1.3 - §8.4
   class CERTI_EXPORT M_Disable_Time_Regulation : public Message {
      public:
         typedef Message Super;
         M_Disable_Time_Regulation();
         virtual ~M_Disable_Time_Regulation();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const double& getLookahead() const {return lookahead;}
         void setLookahead(const double& newLookahead) {lookahead=newLookahead;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         double lookahead;
      private:
   };
   // HLA 1.3 - §8.5
   class CERTI_EXPORT M_Enable_Time_Constrained : public Message {
      public:
         typedef Message Super;
         M_Enable_Time_Constrained();
         virtual ~M_Enable_Time_Constrained();
      protected:
      private:
   };
   // HLA 1.3 - §8.7
   class CERTI_EXPORT M_Disable_Time_Constrained : public Message {
      public:
         typedef Message Super;
         M_Disable_Time_Constrained();
         virtual ~M_Disable_Time_Constrained();
      protected:
      private:
   };

   class CERTI_EXPORT M_Query_Lbts : public Message {
      public:
         typedef Message Super;
         M_Query_Lbts();
         virtual ~M_Query_Lbts();
      protected:
      private:
   };

   class CERTI_EXPORT M_Query_Federate_Time : public Message {
      public:
         typedef Message Super;
         M_Query_Federate_Time();
         virtual ~M_Query_Federate_Time();
      protected:
      private:
   };

   class CERTI_EXPORT M_Query_Min_Next_Event_Time : public Message {
      public:
         typedef Message Super;
         M_Query_Min_Next_Event_Time();
         virtual ~M_Query_Min_Next_Event_Time();
      protected:
      private:
   };

   class CERTI_EXPORT M_Modify_Lookahead : public Message {
      public:
         typedef Message Super;
         M_Modify_Lookahead();
         virtual ~M_Modify_Lookahead();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const double& getLookahead() const {return lookahead;}
         void setLookahead(const double& newLookahead) {lookahead=newLookahead;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         double lookahead;
      private:
   };

   class CERTI_EXPORT M_Query_Lookahead : public Message {
      public:
         typedef Message Super;
         M_Query_Lookahead();
         virtual ~M_Query_Lookahead();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const double& getLookahead() const {return lookahead;}
         void setLookahead(const double& newLookahead) {lookahead=newLookahead;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         double lookahead;
      private:
   };

   class CERTI_EXPORT M_Retract : public Message {
      public:
         typedef Message Super;
         M_Retract();
         virtual ~M_Retract();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const EventRetraction& getEventRetraction() const {return eventRetraction;}
         void setEventRetraction(const EventRetraction& newEventRetraction) {eventRetraction=newEventRetraction;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         EventRetraction eventRetraction;
      private:
   };

   class CERTI_EXPORT M_Request_Retraction : public Message {
      public:
         typedef Message Super;
         M_Request_Retraction();
         virtual ~M_Request_Retraction();
      protected:
      private:
   };
   // HLA 1.3 - §8.8
   class CERTI_EXPORT M_Time_Advance_Request : public Message {
      public:
         typedef Message Super;
         M_Time_Advance_Request();
         virtual ~M_Time_Advance_Request();
      protected:
      private:
   };

   class CERTI_EXPORT M_Time_Advance_Request_Available : public Message {
      public:
         typedef Message Super;
         M_Time_Advance_Request_Available();
         virtual ~M_Time_Advance_Request_Available();
      protected:
      private:
   };

   class CERTI_EXPORT M_Next_Event_Request : public Message {
      public:
         typedef Message Super;
         M_Next_Event_Request();
         virtual ~M_Next_Event_Request();
      protected:
      private:
   };

   class CERTI_EXPORT M_Next_Event_Request_Available : public Message {
      public:
         typedef Message Super;
         M_Next_Event_Request_Available();
         virtual ~M_Next_Event_Request_Available();
      protected:
      private:
   };

   class CERTI_EXPORT M_Flush_Queue_Request : public Message {
      public:
         typedef Message Super;
         M_Flush_Queue_Request();
         virtual ~M_Flush_Queue_Request();
      protected:
      private:
   };

   class CERTI_EXPORT M_Time_Advance_Grant : public Message {
      public:
         typedef Message Super;
         M_Time_Advance_Grant();
         virtual ~M_Time_Advance_Grant();
      protected:
      private:
   };

   class CERTI_EXPORT M_Enable_Asynchronous_Delivery : public Message {
      public:
         typedef Message Super;
         M_Enable_Asynchronous_Delivery();
         virtual ~M_Enable_Asynchronous_Delivery();
      protected:
      private:
   };

   class CERTI_EXPORT M_Disable_Asynchronous_Delivery : public Message {
      public:
         typedef Message Super;
         M_Disable_Asynchronous_Delivery();
         virtual ~M_Disable_Asynchronous_Delivery();
      protected:
      private:
   };
   // HLA 1.3 - §8.3
   class CERTI_EXPORT M_Time_Regulation_Enabled : public Message {
      public:
         typedef Message Super;
         M_Time_Regulation_Enabled();
         virtual ~M_Time_Regulation_Enabled();
      protected:
      private:
   };
   // HLA 1.3 - §8.6
   class CERTI_EXPORT M_Time_Constrained_Enabled : public Message {
      public:
         typedef Message Super;
         M_Time_Constrained_Enabled();
         virtual ~M_Time_Constrained_Enabled();
      protected:
      private:
   };
   // Data Distribution Management
   class CERTI_EXPORT M_Ddm_Create_Region : public Message {
      public:
         typedef Message Super;
         M_Ddm_Create_Region();
         virtual ~M_Ddm_Create_Region();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const SpaceHandle& getSpace() const {return space;}
         void setSpace(const SpaceHandle& newSpace) {space=newSpace;}
         uint32_t getExtentSetSize() const {return extentSet.size();}
         void setExtentSetSize(uint32_t num) {extentSet.resize(num);}
         const std::vector<Extent>& getExtentSet() const {return extentSet;}
         const Extent& getExtentSet(uint32_t rank) const {return extentSet[rank];}
         Extent& getExtentSet(uint32_t rank) {return extentSet[rank];}
         void setExtentSet(const Extent& newExtentSet, uint32_t rank) {extentSet[rank]=newExtentSet;}
         void removeExtentSet(uint32_t rank) {extentSet.erase(extentSet.begin() + rank);}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         SpaceHandle space;// the routing space designator
         std::vector<Extent> extentSet;// the extent set    
         RegionHandle region;// the region handle obtained upon successful creation
      private:
   };

   class CERTI_EXPORT M_Ddm_Modify_Region : public Message {
      public:
         typedef Message Super;
         M_Ddm_Modify_Region();
         virtual ~M_Ddm_Modify_Region();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         uint32_t getExtentSetSize() const {return extentSet.size();}
         void setExtentSetSize(uint32_t num) {extentSet.resize(num);}
         const std::vector<Extent>& getExtentSet() const {return extentSet;}
         const Extent& getExtentSet(uint32_t rank) const {return extentSet[rank];}
         Extent& getExtentSet(uint32_t rank) {return extentSet[rank];}
         void setExtentSet(const Extent& newExtentSet, uint32_t rank) {extentSet[rank]=newExtentSet;}
         void removeExtentSet(uint32_t rank) {extentSet.erase(extentSet.begin() + rank);}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         RegionHandle region;
         std::vector<Extent> extentSet;// the extent set
      private:
   };

   class CERTI_EXPORT M_Ddm_Delete_Region : public Message {
      public:
         typedef Message Super;
         M_Ddm_Delete_Region();
         virtual ~M_Ddm_Delete_Region();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         RegionHandle region;
      private:
   };

   class CERTI_EXPORT M_Ddm_Register_Object : public Message {
      public:
         typedef Message Super;
         M_Ddm_Register_Object();
         virtual ~M_Ddm_Register_Object();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const std::string& getObjectInstanceName() const {return objectInstanceName;}
         void setObjectInstanceName(const std::string& newObjectInstanceName) {
            _hasObjectInstanceName=true;
            objectInstanceName=newObjectInstanceName;
         }
         bool hasObjectInstanceName() {return _hasObjectInstanceName;}
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         ObjectHandle object;
         std::string objectInstanceName;
         bool _hasObjectInstanceName;
         std::vector<AttributeHandle> attributes;// repeated RegionHandle      regions 
      private:
   };

   class CERTI_EXPORT M_Ddm_Associate_Region : public Message {
      public:
         typedef Message Super;
         M_Ddm_Associate_Region();
         virtual ~M_Ddm_Associate_Region();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         RegionHandle region;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Ddm_Unassociate_Region : public Message {
      public:
         typedef Message Super;
         M_Ddm_Unassociate_Region();
         virtual ~M_Ddm_Unassociate_Region();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         RegionHandle region;
      private:
   };

   class CERTI_EXPORT M_Ddm_Subscribe_Attributes : public Message {
      public:
         typedef Message Super;
         M_Ddm_Subscribe_Attributes();
         virtual ~M_Ddm_Subscribe_Attributes();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         void passiveOn() {passive = true;}
         void passiveOff() {passive = false;}
         bool isPassiveOn() const {return passive;}
         uint32_t getAttributesSize() const {return attributes.size();}
         void setAttributesSize(uint32_t num) {attributes.resize(num);}
         const std::vector<AttributeHandle>& getAttributes() const {return attributes;}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
         void removeAttributes(uint32_t rank) {attributes.erase(attributes.begin() + rank);}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         RegionHandle region;
         bool passive;
         std::vector<AttributeHandle> attributes;
      private:
   };

   class CERTI_EXPORT M_Ddm_Unsubscribe_Attributes : public Message {
      public:
         typedef Message Super;
         M_Ddm_Unsubscribe_Attributes();
         virtual ~M_Ddm_Unsubscribe_Attributes();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         RegionHandle region;
      private:
   };

   class CERTI_EXPORT M_Ddm_Subscribe_Interaction : public Message {
      public:
         typedef Message Super;
         M_Ddm_Subscribe_Interaction();
         virtual ~M_Ddm_Subscribe_Interaction();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         void passiveOn() {passive = true;}
         void passiveOff() {passive = false;}
         bool isPassiveOn() const {return passive;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
         RegionHandle region;
         bool passive;
      private:
   };

   class CERTI_EXPORT M_Ddm_Unsubscribe_Interaction : public Message {
      public:
         typedef Message Super;
         M_Ddm_Unsubscribe_Interaction();
         virtual ~M_Ddm_Unsubscribe_Interaction();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         void passiveOn() {passive = true;}
         void passiveOff() {passive = false;}
         bool isPassiveOn() const {return passive;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
         RegionHandle region;
         bool passive;
      private:
   };

   class CERTI_EXPORT M_Ddm_Request_Update : public Message {
      public:
         typedef Message Super;
         M_Ddm_Request_Update();
         virtual ~M_Ddm_Request_Update();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
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
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         RegionHandle region;
         std::vector<AttributeHandle> attributes;
      private:
   };
   // Support Services
   class CERTI_EXPORT M_Get_Object_Class_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Object_Class_Handle();
         virtual ~M_Get_Object_Class_Handle();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const std::string& getClassName() const {return className;}
         void setClassName(const std::string& newClassName) {className=newClassName;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         std::string className;
      private:
   };

   class CERTI_EXPORT M_Get_Object_Class_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Object_Class_Name();
         virtual ~M_Get_Object_Class_Name();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const std::string& getClassName() const {return className;}
         void setClassName(const std::string& newClassName) {className=newClassName;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         std::string className;
      private:
   };

   class CERTI_EXPORT M_Get_Attribute_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Attribute_Handle();
         virtual ~M_Get_Attribute_Handle();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const std::string& getAttributeName() const {return attributeName;}
         void setAttributeName(const std::string& newAttributeName) {attributeName=newAttributeName;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         std::string attributeName;
         AttributeHandle attribute;
      private:
   };

   class CERTI_EXPORT M_Get_Attribute_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Attribute_Name();
         virtual ~M_Get_Attribute_Name();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const std::string& getAttributeName() const {return attributeName;}
         void setAttributeName(const std::string& newAttributeName) {attributeName=newAttributeName;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         std::string attributeName;
         AttributeHandle attribute;
      private:
   };

   class CERTI_EXPORT M_Get_Interaction_Class_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Interaction_Class_Handle();
         virtual ~M_Get_Interaction_Class_Handle();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const std::string& getClassName() const {return className;}
         void setClassName(const std::string& newClassName) {className=newClassName;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
         std::string className;
      private:
   };

   class CERTI_EXPORT M_Get_Interaction_Class_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Interaction_Class_Name();
         virtual ~M_Get_Interaction_Class_Name();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const std::string& getClassName() const {return className;}
         void setClassName(const std::string& newClassName) {className=newClassName;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
         std::string className;
      private:
   };

   class CERTI_EXPORT M_Get_Parameter_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Parameter_Handle();
         virtual ~M_Get_Parameter_Handle();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const std::string& getParameterName() const {return parameterName;}
         void setParameterName(const std::string& newParameterName) {parameterName=newParameterName;}
         const ParameterHandle& getParameter() const {return parameter;}
         void setParameter(const ParameterHandle& newParameter) {parameter=newParameter;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
         std::string parameterName;
         ParameterHandle parameter;
      private:
   };

   class CERTI_EXPORT M_Get_Parameter_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Parameter_Name();
         virtual ~M_Get_Parameter_Name();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const std::string& getParameterName() const {return parameterName;}
         void setParameterName(const std::string& newParameterName) {parameterName=newParameterName;}
         const ParameterHandle& getParameter() const {return parameter;}
         void setParameter(const ParameterHandle& newParameter) {parameter=newParameter;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
         std::string parameterName;
         ParameterHandle parameter;
      private:
   };

   class CERTI_EXPORT M_Get_Object_Instance_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Object_Instance_Handle();
         virtual ~M_Get_Object_Instance_Handle();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const std::string& getObjectInstanceName() const {return objectInstanceName;}
         void setObjectInstanceName(const std::string& newObjectInstanceName) {objectInstanceName=newObjectInstanceName;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::string objectInstanceName;
      private:
   };

   class CERTI_EXPORT M_Get_Object_Instance_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Object_Instance_Name();
         virtual ~M_Get_Object_Instance_Name();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const std::string& getObjectInstanceName() const {return objectInstanceName;}
         void setObjectInstanceName(const std::string& newObjectInstanceName) {objectInstanceName=newObjectInstanceName;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectHandle object;
         std::string objectInstanceName;
      private:
   };

   class CERTI_EXPORT M_Get_Space_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Space_Handle();
         virtual ~M_Get_Space_Handle();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getSpaceName() const {return spaceName;}
         void setSpaceName(const std::string& newSpaceName) {spaceName=newSpaceName;}
         const SpaceHandle& getSpace() const {return space;}
         void setSpace(const SpaceHandle& newSpace) {space=newSpace;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         std::string spaceName;
         SpaceHandle space;
      private:
   };

   class CERTI_EXPORT M_Get_Space_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Space_Name();
         virtual ~M_Get_Space_Name();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getSpaceName() const {return spaceName;}
         void setSpaceName(const std::string& newSpaceName) {spaceName=newSpaceName;}
         const SpaceHandle& getSpace() const {return space;}
         void setSpace(const SpaceHandle& newSpace) {space=newSpace;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         std::string spaceName;
         SpaceHandle space;
      private:
   };

   class CERTI_EXPORT M_Get_Dimension_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Dimension_Handle();
         virtual ~M_Get_Dimension_Handle();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getDimensionName() const {return dimensionName;}
         void setDimensionName(const std::string& newDimensionName) {dimensionName=newDimensionName;}
         const SpaceHandle& getSpace() const {return space;}
         void setSpace(const SpaceHandle& newSpace) {space=newSpace;}
         const DimensionHandle& getDimension() const {return dimension;}
         void setDimension(const DimensionHandle& newDimension) {dimension=newDimension;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         std::string dimensionName;
         SpaceHandle space;
         DimensionHandle dimension;
      private:
   };

   class CERTI_EXPORT M_Get_Dimension_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Dimension_Name();
         virtual ~M_Get_Dimension_Name();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getDimensionName() const {return dimensionName;}
         void setDimensionName(const std::string& newDimensionName) {dimensionName=newDimensionName;}
         const SpaceHandle& getSpace() const {return space;}
         void setSpace(const SpaceHandle& newSpace) {space=newSpace;}
         const DimensionHandle& getDimension() const {return dimension;}
         void setDimension(const DimensionHandle& newDimension) {dimension=newDimension;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         std::string dimensionName;
         SpaceHandle space;
         DimensionHandle dimension;
      private:
   };

   class CERTI_EXPORT M_Get_Attribute_Space_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Attribute_Space_Handle();
         virtual ~M_Get_Attribute_Space_Handle();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
         const SpaceHandle& getSpace() const {return space;}
         void setSpace(const SpaceHandle& newSpace) {space=newSpace;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         AttributeHandle attribute;
         SpaceHandle space;
      private:
   };

   class CERTI_EXPORT M_Get_Object_Class : public Message {
      public:
         typedef Message Super;
         M_Get_Object_Class();
         virtual ~M_Get_Object_Class();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         ObjectClassHandle objectClass;
         ObjectHandle object;
      private:
   };

   class CERTI_EXPORT M_Get_Interaction_Space_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Interaction_Space_Handle();
         virtual ~M_Get_Interaction_Space_Handle();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const SpaceHandle& getSpace() const {return space;}
         void setSpace(const SpaceHandle& newSpace) {space=newSpace;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         InteractionClassHandle interactionClass;
         SpaceHandle space;
      private:
   };

   class CERTI_EXPORT M_Get_Transportation_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Transportation_Handle();
         virtual ~M_Get_Transportation_Handle();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getTransportationName() const {return transportationName;}
         void setTransportationName(const std::string& newTransportationName) {transportationName=newTransportationName;}
         const TransportType& getTransportation() const {return transportation;}
         void setTransportation(const TransportType& newTransportation) {transportation=newTransportation;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         std::string transportationName;
         TransportType transportation;
      private:
   };

   class CERTI_EXPORT M_Get_Transportation_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Transportation_Name();
         virtual ~M_Get_Transportation_Name();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getTransportationName() const {return transportationName;}
         void setTransportationName(const std::string& newTransportationName) {transportationName=newTransportationName;}
         const TransportType& getTransportation() const {return transportation;}
         void setTransportation(const TransportType& newTransportation) {transportation=newTransportation;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         std::string transportationName;
         TransportType transportation;
      private:
   };

   class CERTI_EXPORT M_Get_Ordering_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Ordering_Handle();
         virtual ~M_Get_Ordering_Handle();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getOrderingName() const {return orderingName;}
         void setOrderingName(const std::string& newOrderingName) {orderingName=newOrderingName;}
         const OrderType& getOrdering() const {return ordering;}
         void setOrdering(const OrderType& newOrdering) {ordering=newOrdering;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         std::string orderingName;
         OrderType ordering;
      private:
   };

   class CERTI_EXPORT M_Get_Ordering_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Ordering_Name();
         virtual ~M_Get_Ordering_Name();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getOrderingName() const {return orderingName;}
         void setOrderingName(const std::string& newOrderingName) {orderingName=newOrderingName;}
         const OrderType& getOrdering() const {return ordering;}
         void setOrdering(const OrderType& newOrdering) {ordering=newOrdering;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         std::string orderingName;
         OrderType ordering;
      private:
   };

   class CERTI_EXPORT M_Enable_Class_Relevance_Advisory_Switch : public Message {
      public:
         typedef Message Super;
         M_Enable_Class_Relevance_Advisory_Switch();
         virtual ~M_Enable_Class_Relevance_Advisory_Switch();
      protected:
      private:
   };

   class CERTI_EXPORT M_Disable_Class_Relevance_Advisory_Switch : public Message {
      public:
         typedef Message Super;
         M_Disable_Class_Relevance_Advisory_Switch();
         virtual ~M_Disable_Class_Relevance_Advisory_Switch();
      protected:
      private:
   };

   class CERTI_EXPORT M_Enable_Attribute_Relevance_Advisory_Switch : public Message {
      public:
         typedef Message Super;
         M_Enable_Attribute_Relevance_Advisory_Switch();
         virtual ~M_Enable_Attribute_Relevance_Advisory_Switch();
      protected:
      private:
   };

   class CERTI_EXPORT M_Disable_Attribute_Relevance_Advisory_Switch : public Message {
      public:
         typedef Message Super;
         M_Disable_Attribute_Relevance_Advisory_Switch();
         virtual ~M_Disable_Attribute_Relevance_Advisory_Switch();
      protected:
      private:
   };

   class CERTI_EXPORT M_Enable_Attribute_Scope_Advisory_Switch : public Message {
      public:
         typedef Message Super;
         M_Enable_Attribute_Scope_Advisory_Switch();
         virtual ~M_Enable_Attribute_Scope_Advisory_Switch();
      protected:
      private:
   };

   class CERTI_EXPORT M_Disable_Attribute_Scope_Advisory_Switch : public Message {
      public:
         typedef Message Super;
         M_Disable_Attribute_Scope_Advisory_Switch();
         virtual ~M_Disable_Attribute_Scope_Advisory_Switch();
      protected:
      private:
   };

   class CERTI_EXPORT M_Enable_Interaction_Relevance_Advisory_Switch : public Message {
      public:
         typedef Message Super;
         M_Enable_Interaction_Relevance_Advisory_Switch();
         virtual ~M_Enable_Interaction_Relevance_Advisory_Switch();
      protected:
      private:
   };

   class CERTI_EXPORT M_Disable_Interaction_Relevance_Advisory_Switch : public Message {
      public:
         typedef Message Super;
         M_Disable_Interaction_Relevance_Advisory_Switch();
         virtual ~M_Disable_Interaction_Relevance_Advisory_Switch();
      protected:
      private:
   };

   class CERTI_EXPORT M_Tick_Request : public Message {
      public:
         typedef Message Super;
         M_Tick_Request();
         virtual ~M_Tick_Request();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const bool& getMultiple() const {return multiple;}
         void setMultiple(const bool& newMultiple) {multiple=newMultiple;}
         const double& getMinTickTime() const {return minTickTime;}
         void setMinTickTime(const double& newMinTickTime) {minTickTime=newMinTickTime;}
         const double& getMaxTickTime() const {return maxTickTime;}
         void setMaxTickTime(const double& newMaxTickTime) {maxTickTime=newMaxTickTime;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         bool multiple;
         double minTickTime;
         double maxTickTime;
      private:
   };

   class CERTI_EXPORT M_Tick_Request_Next : public Message {
      public:
         typedef Message Super;
         M_Tick_Request_Next();
         virtual ~M_Tick_Request_Next();
      protected:
      private:
   };

   class CERTI_EXPORT M_Tick_Request_Stop : public Message {
      public:
         typedef Message Super;
         M_Tick_Request_Stop();
         virtual ~M_Tick_Request_Stop();
      protected:
      private:
   };
   // HLA 1516 - §6.2
   class CERTI_EXPORT M_Reserve_Object_Instance_Name : public Message {
      public:
         typedef Message Super;
         M_Reserve_Object_Instance_Name();
         virtual ~M_Reserve_Object_Instance_Name();
         virtual void serialize(libhla::MessageBuffer& msgBuffer);
         virtual void deserialize(libhla::MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getObjectName() const {return objectName;}
         void setObjectName(const std::string& newObjectName) {objectName=newObjectName;}
         // the show method
         virtual std::ostream& show(std::ostream& out);
      protected:
         std::string objectName;
      private:
   };
   // HLA 1516 - §6.3
   class CERTI_EXPORT M_Reserve_Object_Instance_Name_Succeeded : public M_Reserve_Object_Instance_Name {
      public:
         typedef M_Reserve_Object_Instance_Name Super;
         M_Reserve_Object_Instance_Name_Succeeded();
         virtual ~M_Reserve_Object_Instance_Name_Succeeded();
      protected:
      private:
   };

   class CERTI_EXPORT M_Reserve_Object_Instance_Name_Failed : public M_Reserve_Object_Instance_Name {
      public:
         typedef M_Reserve_Object_Instance_Name Super;
         M_Reserve_Object_Instance_Name_Failed();
         virtual ~M_Reserve_Object_Instance_Name_Failed();
      protected:
      private:
   };

   class CERTI_EXPORT M_Factory {
      public:
         static Message* create(M_Type type) throw (NetworkError ,NetworkSignal); 
         static Message* receive(MStreamType stream) throw (NetworkError ,NetworkSignal); 
      protected:
      private:
   };

} // end of namespace certi 
// M_CLASSES_HH
#endif
