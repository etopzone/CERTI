// Generated on 2010 February Sat, 06 at 19:51:15 by the CERTI message generator
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

#include "RTItypes.hh"

#include "certi.hh"

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

   typedef RTI::ResignAction ResignAction;

   typedef SocketUN* StreamType;

   typedef Message::Type M_Type;

   typedef enum CERTI_Message_MessageType {
      NOT_USED = 0, 
      M_CLOSE_CONNEXION, 
      M_CREATE_FEDERATION_EXECUTION, 
      M_DESTROY_FEDERATION_EXECUTION, 
      M_JOIN_FEDERATION_EXECUTION, 
      M_RESIGN_FEDERATION_EXECUTION, 
      M_REGISTER_FEDERATION_SYNCHRONIZATION_POINT, 
      M_SYNCHRONIZATION_POINT_REGISTRATION_FAILED, 
      M_SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED, 
      M_ANNOUNCE_SYNCHRONIZATION_POINT, 
      M_SYNCHRONIZATION_POINT_ACHIEVED, 
      M_FEDERATION_SYNCHRONIZED, 
      M_REQUEST_FEDERATION_SAVE, 
      M_INITIATE_FEDERATE_SAVE, 
      M_FEDERATE_SAVE_BEGUN, 
      M_FEDERATE_SAVE_COMPLETE, 
      M_FEDERATE_SAVE_NOT_COMPLETE, 
      M_FEDERATION_SAVED, 
      M_FEDERATION_NOT_SAVED, 
      M_REQUEST_FEDERATION_RESTORE, 
      M_REQUEST_FEDERATION_RESTORE_FAILED, 
      M_REQUEST_FEDERATION_RESTORE_SUCCEEDED, 
      M_INITIATE_FEDERATE_RESTORE, 
      M_FEDERATE_RESTORE_COMPLETE, 
      M_FEDERATE_RESTORE_NOT_COMPLETE, 
      M_FEDERATION_RESTORED, 
      M_FEDERATION_NOT_RESTORED, 
      M_FEDERATION_RESTORE_BEGUN, 
      M_PUBLISH_OBJECT_CLASS, 
      M_UNPUBLISH_OBJECT_CLASS, 
      M_PUBLISH_INTERACTION_CLASS, 
      M_UNPUBLISH_INTERACTION_CLASS, 
      M_SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES, 
      M_UNSUBSCRIBE_OBJECT_CLASS, 
      M_SUBSCRIBE_INTERACTION_CLASS, 
      M_UNSUBSCRIBE_INTERACTION_CLASS, 
      M_START_REGISTRATION_FOR_OBJECT_CLASS, 
      M_STOP_REGISTRATION_FOR_OBJECT_CLASS, 
      M_TURN_INTERACTIONS_ON, 
      M_TURN_INTERACTIONS_OFF, 
      M_REGISTER_OBJECT_INSTANCE, 
      M_UPDATE_ATTRIBUTE_VALUES, 
      M_DISCOVER_OBJECT_INSTANCE, 
      M_REFLECT_ATTRIBUTE_VALUES, 
      M_SEND_INTERACTION, 
      M_RECEIVE_INTERACTION, 
      M_DELETE_OBJECT_INSTANCE, 
      M_LOCAL_DELETE_OBJECT_INSTANCE, 
      M_REMOVE_OBJECT_INSTANCE, 
      M_CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE, 
      M_CHANGE_INTERACTION_TRANSPORTATION_TYPE, 
      M_REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE, 
      M_REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE, 
      M_PROVIDE_ATTRIBUTE_VALUE_UPDATE, 
      M_ATTRIBUTES_IN_SCOPE, 
      M_ATTRIBUTES_OUT_OF_SCOPE, 
      M_TURN_UPDATES_ON_FOR_OBJECT_INSTANCE, 
      M_TURN_UPDATES_OFF_FOR_OBJECT_INSTANCE, 
      M_REQUEST_ATTRIBUTE_OWNERSHIP_DIVESTITURE, 
      M_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION, 
      M_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE, 
      M_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION, 
      M_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION, 
      M_REQUEST_ATTRIBUTE_OWNERSHIP_ACQUISITION, 
      M_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE, 
      M_QUERY_ATTRIBUTE_OWNERSHIP, 
      M_INFORM_ATTRIBUTE_OWNERSHIP, 
      M_IS_ATTRIBUTE_OWNED_BY_FEDERATE, 
      M_ATTRIBUTE_IS_NOT_OWNED, 
      M_ATTRIBUTE_OWNED_BY_RTI, 
      M_ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE, 
      M_ATTRIBUTE_OWNERSHIP_UNAVAILABLE, 
      M_UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE, 
      M_ATTRIBUTE_OWNERSHIP_ACQUISITION, 
      M_CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE, 
      M_ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE, 
      M_CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION, 
      M_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION, 
      M_CHANGE_ATTRIBUTE_ORDER_TYPE, 
      M_CHANGE_INTERACTION_ORDER_TYPE, 
      M_ENABLE_TIME_REGULATION, 
      M_DISABLE_TIME_REGULATION, 
      M_ENABLE_TIME_CONSTRAINED, 
      M_DISABLE_TIME_CONSTRAINED, 
      M_QUERY_LBTS, 
      M_QUERY_FEDERATE_TIME, 
      M_QUERY_MIN_NEXT_EVENT_TIME, 
      M_MODIFY_LOOKAHEAD, 
      M_QUERY_LOOKAHEAD, 
      M_RETRACT, 
      M_REQUEST_RETRACTION, 
      M_TIME_ADVANCE_REQUEST, 
      M_TIME_ADVANCE_REQUEST_AVAILABLE, 
      M_NEXT_EVENT_REQUEST, 
      M_NEXT_EVENT_REQUEST_AVAILABLE, 
      M_FLUSH_QUEUE_REQUEST, 
      M_TIME_ADVANCE_GRANT, 
      M_ENABLE_ASYNCHRONOUS_DELIVERY, 
      M_DISABLE_ASYNCHRONOUS_DELIVERY, 
      M_TIME_REGULATION_ENABLED, 
      M_TIME_CONSTRAINED_ENABLED, 
      M_DDM_CREATE_REGION, 
      M_DDM_MODIFY_REGION, 
      M_DDM_DELETE_REGION, 
      M_DDM_REGISTER_OBJECT, 
      M_DDM_ASSOCIATE_REGION, 
      M_DDM_UNASSOCIATE_REGION, 
      M_DDM_SUBSCRIBE_ATTRIBUTES, 
      M_DDM_UNSUBSCRIBE_ATTRIBUTES, 
      M_DDM_SUBSCRIBE_INTERACTION, 
      M_DDM_UNSUBSCRIBE_INTERACTION, 
      M_DDM_REQUEST_UPDATE, 
      M_GET_OBJECT_CLASS_HANDLE, 
      M_GET_OBJECT_CLASS_NAME, 
      M_GET_ATTRIBUTE_HANDLE, 
      M_GET_ATTRIBUTE_NAME, 
      M_GET_INTERACTION_CLASS_HANDLE, 
      M_GET_INTERACTION_CLASS_NAME, 
      M_GET_PARAMETER_HANDLE, 
      M_GET_PARAMETER_NAME, 
      M_GET_OBJECT_INSTANCE_HANDLE, 
      M_GET_OBJECT_INSTANCE_NAME, 
      M_GET_SPACE_HANDLE, 
      M_GET_SPACE_NAME, 
      M_GET_DIMENSION_HANDLE, 
      M_GET_DIMENSION_NAME, 
      M_GET_ATTRIBUTE_SPACE_HANDLE, 
      M_GET_OBJECT_CLASS, 
      M_GET_INTERACTION_SPACE_HANDLE, 
      M_GET_TRANSPORTATION_HANDLE, 
      M_GET_TRANSPORTATION_NAME, 
      M_GET_ORDERING_HANDLE, 
      M_GET_ORDERING_NAME, 
      M_ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH, 
      M_DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH, 
      M_ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH, 
      M_DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH, 
      M_ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH, 
      M_DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH, 
      M_ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH, 
      M_DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH, 
      M_TICK_REQUEST, 
      M_TICK_REQUEST_NEXT, 
      M_TICK_REQUEST_STOP, 
      LAST 
   } CERTI_Message_MessageType_t; //end of enum CERTI_Message_MessageType 
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getFederationName() const {return federationName;}
         void setFederationName(const std::string& newFederationName) {federationName=newFederationName;}
         const std::string& getFEDid() const {return FEDid;}
         void setFEDid(const std::string& newFEDid) {FEDid=newFEDid;}
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getFederationName() const {return federationName;}
         void setFederationName(const std::string& newFederationName) {federationName=newFederationName;}
      protected:
         std::string federationName;
      private:
   };

   class CERTI_EXPORT M_Join_Federation_Execution : public Message {
      public:
         typedef Message Super;
         M_Join_Federation_Execution();
         virtual ~M_Join_Federation_Execution();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const FederateHandle& getFederate() const {return federate;}
         void setFederate(const FederateHandle& newFederate) {federate=newFederate;}
         const std::string& getFederationName() const {return federationName;}
         void setFederationName(const std::string& newFederationName) {federationName=newFederationName;}
         const std::string& getFederateName() const {return federateName;}
         void setFederateName(const std::string& newFederateName) {federateName=newFederateName;}
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ResignAction& getResignAction() const {return resignAction;}
         void setResignAction(const ResignAction& newResignAction) {resignAction=newResignAction;}
      protected:
         ResignAction resignAction;
      private:
   };

   class CERTI_EXPORT M_Register_Federation_Synchronization_Point : public Message {
      public:
         typedef Message Super;
         M_Register_Federation_Synchronization_Point();
         virtual ~M_Register_Federation_Synchronization_Point();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         uint32_t getNumFederateSet() const {return federateSet.size();}
         void setNumFederateSet(uint32_t num) {federateSet.resize(num);}
         const FederateHandle& getFederateSet(uint32_t rank) const {return federateSet[rank];}
         FederateHandle& getFederateSet(uint32_t rank) {return federateSet[rank];}
         void setFederateSet(const FederateHandle& newFederateSet, uint32_t rank) {federateSet[rank]=newFederateSet;}
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
      protected:
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const FederateHandle& getFederate() const {return federate;}
         void setFederate(const FederateHandle& newFederate) {federate=newFederate;}
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         uint32_t getNumHandles() const {return handles.size();}
         void setNumHandles(uint32_t num) {handles.resize(num);}
         const Handle& getHandles(uint32_t rank) const {return handles[rank];}
         Handle& getHandles(uint32_t rank) {return handles[rank];}
         void setHandles(const Handle& newHandles, uint32_t rank) {handles[rank]=newHandles;}
      protected:
         ObjectClassHandle objectClass;
         std::vector<Handle> handles;
      private:
   };

   class CERTI_EXPORT M_Unpublish_Object_Class : public Message {
      public:
         typedef Message Super;
         M_Unpublish_Object_Class();
         virtual ~M_Unpublish_Object_Class();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
      protected:
         ObjectClassHandle objectClass;
      private:
   };

   class CERTI_EXPORT M_Publish_Interaction_Class : public Message {
      public:
         typedef Message Super;
         M_Publish_Interaction_Class();
         virtual ~M_Publish_Interaction_Class();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
      protected:
         InteractionClassHandle interactionClass;
      private:
   };

   class CERTI_EXPORT M_Unpublish_Interaction_Class : public Message {
      public:
         typedef Message Super;
         M_Unpublish_Interaction_Class();
         virtual ~M_Unpublish_Interaction_Class();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
      protected:
         InteractionClassHandle interactionClass;
      private:
   };

   class CERTI_EXPORT M_Subscribe_Object_Class_Attributes : public Message {
      public:
         typedef Message Super;
         M_Subscribe_Object_Class_Attributes();
         virtual ~M_Subscribe_Object_Class_Attributes();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         uint32_t getNumHandles() const {return handles.size();}
         void setNumHandles(uint32_t num) {handles.resize(num);}
         const Handle& getHandles(uint32_t rank) const {return handles[rank];}
         Handle& getHandles(uint32_t rank) {return handles[rank];}
         void setHandles(const Handle& newHandles, uint32_t rank) {handles[rank]=newHandles;}
      protected:
         ObjectClassHandle objectClass;
         std::vector<Handle> handles;
      private:
   };

   class CERTI_EXPORT M_Unsubscribe_Object_Class : public Message {
      public:
         typedef Message Super;
         M_Unsubscribe_Object_Class();
         virtual ~M_Unsubscribe_Object_Class();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
      protected:
         ObjectClassHandle objectClass;
      private:
   };

   class CERTI_EXPORT M_Subscribe_Interaction_Class : public Message {
      public:
         typedef Message Super;
         M_Subscribe_Interaction_Class();
         virtual ~M_Subscribe_Interaction_Class();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
      protected:
         InteractionClassHandle interactionClass;
      private:
   };

   class CERTI_EXPORT M_Unsubscribe_Interaction_Class : public Message {
      public:
         typedef Message Super;
         M_Unsubscribe_Interaction_Class();
         virtual ~M_Unsubscribe_Interaction_Class();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
      protected:
         InteractionClassHandle interactionClass;
      private:
   };

   class CERTI_EXPORT M_Start_Registration_For_Object_Class : public Message {
      public:
         typedef Message Super;
         M_Start_Registration_For_Object_Class();
         virtual ~M_Start_Registration_For_Object_Class();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
      protected:
         ObjectClassHandle objectClass;
      private:
   };

   class CERTI_EXPORT M_Stop_Registration_For_Object_Class : public Message {
      public:
         typedef Message Super;
         M_Stop_Registration_For_Object_Class();
         virtual ~M_Stop_Registration_For_Object_Class();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
      protected:
         ObjectClassHandle objectClass;
      private:
   };

   class CERTI_EXPORT M_Turn_Interactions_On : public Message {
      public:
         typedef Message Super;
         M_Turn_Interactions_On();
         virtual ~M_Turn_Interactions_On();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
      protected:
         InteractionClassHandle interactionClass;
      private:
   };

   class CERTI_EXPORT M_Turn_Interactions_Off : public Message {
      public:
         typedef Message Super;
         M_Turn_Interactions_Off();
         virtual ~M_Turn_Interactions_Off();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const std::string& getObjectName() const {return objectName;}
         void setObjectName(const std::string& newObjectName) {objectName=newObjectName;}
      protected:
         ObjectClassHandle objectClass;
         ObjectHandle object;
         std::string objectName;
      private:
   };

   class CERTI_EXPORT M_Update_Attribute_Values : public Message {
      public:
         typedef Message Super;
         M_Update_Attribute_Values();
         virtual ~M_Update_Attribute_Values();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getNumHandle() const {return handle.size();}
         void setNumHandle(uint32_t num) {handle.resize(num);}
         const Handle& getHandle(uint32_t rank) const {return handle[rank];}
         Handle& getHandle(uint32_t rank) {return handle[rank];}
         void setHandle(const Handle& newHandle, uint32_t rank) {handle[rank]=newHandle;}
         uint32_t getNumAttributeValues() const {return attributeValues.size();}
         void setNumAttributeValues(uint32_t num) {attributeValues.resize(num);}
         const std::string& getAttributeValues(uint32_t rank) const {return attributeValues[rank];}
         std::string& getAttributeValues(uint32_t rank) {return attributeValues[rank];}
         void setAttributeValues(const std::string& newAttributeValues, uint32_t rank) {attributeValues[rank]=newAttributeValues;}
         const ResignAction& getResignAction() const {return resignAction;}
         void setResignAction(const ResignAction& newResignAction) {resignAction=newResignAction;}
      protected:
         ObjectClassHandle objectClass;
         ObjectHandle object;
         std::vector<Handle> handle;
         std::vector<std::string> attributeValues;// FIXME check this one
         ResignAction resignAction;
      private:
   };

   class CERTI_EXPORT M_Discover_Object_Instance : public Message {
      public:
         typedef Message Super;
         M_Discover_Object_Instance();
         virtual ~M_Discover_Object_Instance();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const std::string& getObjectName() const {return objectName;}
         void setObjectName(const std::string& newObjectName) {objectName=newObjectName;}
         const ResignAction& getResignAction() const {return resignAction;}
         void setResignAction(const ResignAction& newResignAction) {resignAction=newResignAction;}
      protected:
         ObjectClassHandle objectClass;
         ObjectHandle object;
         std::string objectName;
         ResignAction resignAction;
      private:
   };

   class CERTI_EXPORT M_Reflect_Attribute_Values : public Message {
      public:
         typedef Message Super;
         M_Reflect_Attribute_Values();
         virtual ~M_Reflect_Attribute_Values();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getNumHandle() const {return handle.size();}
         void setNumHandle(uint32_t num) {handle.resize(num);}
         const Handle& getHandle(uint32_t rank) const {return handle[rank];}
         Handle& getHandle(uint32_t rank) {return handle[rank];}
         void setHandle(const Handle& newHandle, uint32_t rank) {handle[rank]=newHandle;}
         uint32_t getNumAttributeValues() const {return attributeValues.size();}
         void setNumAttributeValues(uint32_t num) {attributeValues.resize(num);}
         const std::string& getAttributeValues(uint32_t rank) const {return attributeValues[rank];}
         std::string& getAttributeValues(uint32_t rank) {return attributeValues[rank];}
         void setAttributeValues(const std::string& newAttributeValues, uint32_t rank) {attributeValues[rank]=newAttributeValues;}
         const ResignAction& getResignAction() const {return resignAction;}
         void setResignAction(const ResignAction& newResignAction) {resignAction=newResignAction;}
      protected:
         ObjectClassHandle objectClass;
         ObjectHandle object;
         std::vector<Handle> handle;
         std::vector<std::string> attributeValues;// FIXME check this one
         ResignAction resignAction;
      private:
   };

   class CERTI_EXPORT M_Send_Interaction : public Message {
      public:
         typedef Message Super;
         M_Send_Interaction();
         virtual ~M_Send_Interaction();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         uint32_t getNumHandles() const {return handles.size();}
         void setNumHandles(uint32_t num) {handles.resize(num);}
         const Handle& getHandles(uint32_t rank) const {return handles[rank];}
         Handle& getHandles(uint32_t rank) {return handles[rank];}
         void setHandles(const Handle& newHandles, uint32_t rank) {handles[rank]=newHandles;}
         uint32_t getNumValues() const {return values.size();}
         void setNumValues(uint32_t num) {values.resize(num);}
         const std::string& getValues(uint32_t rank) const {return values[rank];}
         std::string& getValues(uint32_t rank) {return values[rank];}
         void setValues(const std::string& newValues, uint32_t rank) {values[rank]=newValues;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         const ResignAction& getResignAction() const {return resignAction;}
         void setResignAction(const ResignAction& newResignAction) {resignAction=newResignAction;}
      protected:
         InteractionClassHandle interactionClass;
         std::vector<Handle> handles;
         std::vector<std::string> values;
         RegionHandle region;
         ResignAction resignAction;
      private:
   };

   class CERTI_EXPORT M_Receive_Interaction : public Message {
      public:
         typedef Message Super;
         M_Receive_Interaction();
         virtual ~M_Receive_Interaction();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         uint32_t getNumHandles() const {return handles.size();}
         void setNumHandles(uint32_t num) {handles.resize(num);}
         const Handle& getHandles(uint32_t rank) const {return handles[rank];}
         Handle& getHandles(uint32_t rank) {return handles[rank];}
         void setHandles(const Handle& newHandles, uint32_t rank) {handles[rank]=newHandles;}
         uint32_t getNumValues() const {return values.size();}
         void setNumValues(uint32_t num) {values.resize(num);}
         const std::string& getValues(uint32_t rank) const {return values[rank];}
         std::string& getValues(uint32_t rank) {return values[rank];}
         void setValues(const std::string& newValues, uint32_t rank) {values[rank]=newValues;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         const ResignAction& getResignAction() const {return resignAction;}
         void setResignAction(const ResignAction& newResignAction) {resignAction=newResignAction;}
      protected:
         InteractionClassHandle interactionClass;
         std::vector<Handle> handles;
         std::vector<std::string> values;
         RegionHandle region;
         ResignAction resignAction;
      private:
   };

   class CERTI_EXPORT M_Delete_Object_Instance : public Message {
      public:
         typedef Message Super;
         M_Delete_Object_Instance();
         virtual ~M_Delete_Object_Instance();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const std::string& getObjectName() const {return objectName;}
         void setObjectName(const std::string& newObjectName) {objectName=newObjectName;}
         const ResignAction& getResignAction() const {return resignAction;}
         void setResignAction(const ResignAction& newResignAction) {resignAction=newResignAction;}
      protected:
         ObjectClassHandle objectClass;
         ObjectHandle object;
         std::string objectName;
         ResignAction resignAction;
      private:
   };

   class CERTI_EXPORT M_Local_Delete_Object_Instance : public Message {
      public:
         typedef Message Super;
         M_Local_Delete_Object_Instance();
         virtual ~M_Local_Delete_Object_Instance();
      protected:
      private:
   };

   class CERTI_EXPORT M_Remove_Object_Instance : public Message {
      public:
         typedef Message Super;
         M_Remove_Object_Instance();
         virtual ~M_Remove_Object_Instance();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const std::string& getObjectName() const {return objectName;}
         void setObjectName(const std::string& newObjectName) {objectName=newObjectName;}
         const ResignAction& getResignAction() const {return resignAction;}
         void setResignAction(const ResignAction& newResignAction) {resignAction=newResignAction;}
      protected:
         ObjectClassHandle objectClass;
         ObjectHandle object;
         std::string objectName;
         ResignAction resignAction;
      private:
   };

   class CERTI_EXPORT M_Change_Attribute_Transportation_Type : public Message {
      public:
         typedef Message Super;
         M_Change_Attribute_Transportation_Type();
         virtual ~M_Change_Attribute_Transportation_Type();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const TransportType& getTransport() const {return transport;}
         void setTransport(const TransportType& newTransport) {transport=newTransport;}
         const OrderType& getOrder() const {return order;}
         void setOrder(const OrderType& newOrder) {order=newOrder;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getNumHandles() const {return handles.size();}
         void setNumHandles(uint32_t num) {handles.resize(num);}
         const Handle& getHandles(uint32_t rank) const {return handles[rank];}
         Handle& getHandles(uint32_t rank) {return handles[rank];}
         void setHandles(const Handle& newHandles, uint32_t rank) {handles[rank]=newHandles;}
      protected:
         TransportType transport;
         OrderType order;
         ObjectHandle object;
         std::vector<Handle> handles;
      private:
   };

   class CERTI_EXPORT M_Change_Interaction_Transportation_Type : public Message {
      public:
         typedef Message Super;
         M_Change_Interaction_Transportation_Type();
         virtual ~M_Change_Interaction_Transportation_Type();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const TransportType& getTransport() const {return transport;}
         void setTransport(const TransportType& newTransport) {transport=newTransport;}
         const OrderType& getOrder() const {return order;}
         void setOrder(const OrderType& newOrder) {order=newOrder;}
      protected:
         InteractionClassHandle interactionClass;
         TransportType transport;
         OrderType order;
      private:
   };

   class CERTI_EXPORT M_Request_Object_Attribute_Value_Update : public Message {
      public:
         typedef Message Super;
         M_Request_Object_Attribute_Value_Update();
         virtual ~M_Request_Object_Attribute_Value_Update();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getNumHandles() const {return handles.size();}
         void setNumHandles(uint32_t num) {handles.resize(num);}
         const Handle& getHandles(uint32_t rank) const {return handles[rank];}
         Handle& getHandles(uint32_t rank) {return handles[rank];}
         void setHandles(const Handle& newHandles, uint32_t rank) {handles[rank]=newHandles;}
      protected:
         ObjectHandle object;
         std::vector<Handle> handles;
      private:
   };

   class CERTI_EXPORT M_Request_Class_Attribute_Value_Update : public Message {
      public:
         typedef Message Super;
         M_Request_Class_Attribute_Value_Update();
         virtual ~M_Request_Class_Attribute_Value_Update();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         uint32_t getNumHandles() const {return handles.size();}
         void setNumHandles(uint32_t num) {handles.resize(num);}
         const Handle& getHandles(uint32_t rank) const {return handles[rank];}
         Handle& getHandles(uint32_t rank) {return handles[rank];}
         void setHandles(const Handle& newHandles, uint32_t rank) {handles[rank]=newHandles;}
      protected:
         ObjectClassHandle objectClass;
         std::vector<Handle> handles;
      private:
   };

   class CERTI_EXPORT M_Provide_Attribute_Value_Update : public Message {
      public:
         typedef Message Super;
         M_Provide_Attribute_Value_Update();
         virtual ~M_Provide_Attribute_Value_Update();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getNumHandles() const {return handles.size();}
         void setNumHandles(uint32_t num) {handles.resize(num);}
         const Handle& getHandles(uint32_t rank) const {return handles[rank];}
         Handle& getHandles(uint32_t rank) {return handles[rank];}
         void setHandles(const Handle& newHandles, uint32_t rank) {handles[rank]=newHandles;}
      protected:
         ObjectHandle object;
         std::vector<Handle> handles;
      private:
   };

   class CERTI_EXPORT M_Attributes_In_Scope : public Message {
      public:
         typedef Message Super;
         M_Attributes_In_Scope();
         virtual ~M_Attributes_In_Scope();
      protected:
      private:
   };

   class CERTI_EXPORT M_Attributes_Out_Of_Scope : public Message {
      public:
         typedef Message Super;
         M_Attributes_Out_Of_Scope();
         virtual ~M_Attributes_Out_Of_Scope();
      protected:
      private:
   };

   class CERTI_EXPORT M_Turn_Updates_On_For_Object_Instance : public Message {
      public:
         typedef Message Super;
         M_Turn_Updates_On_For_Object_Instance();
         virtual ~M_Turn_Updates_On_For_Object_Instance();
      protected:
      private:
   };

   class CERTI_EXPORT M_Turn_Updates_Off_For_Object_Instance : public Message {
      public:
         typedef Message Super;
         M_Turn_Updates_Off_For_Object_Instance();
         virtual ~M_Turn_Updates_Off_For_Object_Instance();
      protected:
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getNumAttributes() const {return attributes.size();}
         void setNumAttributes(uint32_t num) {attributes.resize(num);}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getNumAttributes() const {return attributes.size();}
         void setNumAttributes(uint32_t num) {attributes.resize(num);}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
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
      protected:
      private:
   };

   class CERTI_EXPORT M_Attribute_Ownership_Acquisition_Notification : public Message {
      public:
         typedef Message Super;
         M_Attribute_Ownership_Acquisition_Notification();
         virtual ~M_Attribute_Ownership_Acquisition_Notification();
      protected:
      private:
   };

   class CERTI_EXPORT M_Request_Attribute_Ownership_Acquisition : public Message {
      public:
         typedef Message Super;
         M_Request_Attribute_Ownership_Acquisition();
         virtual ~M_Request_Attribute_Ownership_Acquisition();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getNumAttributes() const {return attributes.size();}
         void setNumAttributes(uint32_t num) {attributes.resize(num);}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getNumAttributes() const {return attributes.size();}
         void setNumAttributes(uint32_t num) {attributes.resize(num);}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
         const FederateHandle& getFederate() const {return federate;}
         void setFederate(const FederateHandle& newFederate) {federate=newFederate;}
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
         const FederateHandle& getFederate() const {return federate;}
         void setFederate(const FederateHandle& newFederate) {federate=newFederate;}
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
      protected:
      private:
   };

   class CERTI_EXPORT M_Attribute_Ownership_Unavailable : public Message {
      public:
         typedef Message Super;
         M_Attribute_Ownership_Unavailable();
         virtual ~M_Attribute_Ownership_Unavailable();
      protected:
      private:
   };

   class CERTI_EXPORT M_Unconditional_Attribute_Ownership_Divestiture : public Message {
      public:
         typedef Message Super;
         M_Unconditional_Attribute_Ownership_Divestiture();
         virtual ~M_Unconditional_Attribute_Ownership_Divestiture();
      protected:
      private:
   };

   class CERTI_EXPORT M_Attribute_Ownership_Acquisition : public Message {
      public:
         typedef Message Super;
         M_Attribute_Ownership_Acquisition();
         virtual ~M_Attribute_Ownership_Acquisition();
      protected:
      private:
   };

   class CERTI_EXPORT M_Cancel_Negotiated_Attribute_Ownership_Divestiture : public Message {
      public:
         typedef Message Super;
         M_Cancel_Negotiated_Attribute_Ownership_Divestiture();
         virtual ~M_Cancel_Negotiated_Attribute_Ownership_Divestiture();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getNumAttributes() const {return attributes.size();}
         void setNumAttributes(uint32_t num) {attributes.resize(num);}
         const AttributeHandle& getAttributes(uint32_t rank) const {return attributes[rank];}
         AttributeHandle& getAttributes(uint32_t rank) {return attributes[rank];}
         void setAttributes(const AttributeHandle& newAttributes, uint32_t rank) {attributes[rank]=newAttributes;}
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
      protected:
      private:
   };

   class CERTI_EXPORT M_Cancel_Attribute_Ownership_Acquisition : public Message {
      public:
         typedef Message Super;
         M_Cancel_Attribute_Ownership_Acquisition();
         virtual ~M_Cancel_Attribute_Ownership_Acquisition();
      protected:
      private:
   };

   class CERTI_EXPORT M_Confirm_Attribute_Ownership_Acquisition_Cancellation : public Message {
      public:
         typedef Message Super;
         M_Confirm_Attribute_Ownership_Acquisition_Cancellation();
         virtual ~M_Confirm_Attribute_Ownership_Acquisition_Cancellation();
      protected:
      private:
   };
   // Time
   class CERTI_EXPORT M_Change_Attribute_Order_Type : public Message {
      public:
         typedef Message Super;
         M_Change_Attribute_Order_Type();
         virtual ~M_Change_Attribute_Order_Type();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const TransportType& getTransport() const {return transport;}
         void setTransport(const TransportType& newTransport) {transport=newTransport;}
         const OrderType& getOrder() const {return order;}
         void setOrder(const OrderType& newOrder) {order=newOrder;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getNumHandles() const {return handles.size();}
         void setNumHandles(uint32_t num) {handles.resize(num);}
         const Handle& getHandles(uint32_t rank) const {return handles[rank];}
         Handle& getHandles(uint32_t rank) {return handles[rank];}
         void setHandles(const Handle& newHandles, uint32_t rank) {handles[rank]=newHandles;}
      protected:
         TransportType transport;
         OrderType order;
         ObjectHandle object;
         std::vector<Handle> handles;
      private:
   };

   class CERTI_EXPORT M_Change_Interaction_Order_Type : public Message {
      public:
         typedef Message Super;
         M_Change_Interaction_Order_Type();
         virtual ~M_Change_Interaction_Order_Type();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const TransportType& getTransport() const {return transport;}
         void setTransport(const TransportType& newTransport) {transport=newTransport;}
         const OrderType& getOrder() const {return order;}
         void setOrder(const OrderType& newOrder) {order=newOrder;}
      protected:
         InteractionClassHandle interactionClass;
         TransportType transport;
         OrderType order;
      private:
   };

   class CERTI_EXPORT M_Enable_Time_Regulation : public Message {
      public:
         typedef Message Super;
         M_Enable_Time_Regulation();
         virtual ~M_Enable_Time_Regulation();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         void enableOn() {enable = true;}
         void enableOff() {enable = false;}
         bool isEnableOn() const {return enable;}
         const double_t& getLookahead() const {return lookahead;}
         void setLookahead(const double_t& newLookahead) {lookahead=newLookahead;}
      protected:
         bool enable;
         double_t lookahead;
      private:
   };

   class CERTI_EXPORT M_Disable_Time_Regulation : public Message {
      public:
         typedef Message Super;
         M_Disable_Time_Regulation();
         virtual ~M_Disable_Time_Regulation();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         void enableOn() {enable = true;}
         void enableOff() {enable = false;}
         bool isEnableOn() const {return enable;}
         const double_t& getLookahead() const {return lookahead;}
         void setLookahead(const double_t& newLookahead) {lookahead=newLookahead;}
      protected:
         bool enable;
         double_t lookahead;
      private:
   };

   class CERTI_EXPORT M_Enable_Time_Constrained : public Message {
      public:
         typedef Message Super;
         M_Enable_Time_Constrained();
         virtual ~M_Enable_Time_Constrained();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         void enableOn() {enable = true;}
         void enableOff() {enable = false;}
         bool isEnableOn() const {return enable;}
         const double_t& getLookahead() const {return lookahead;}
         void setLookahead(const double_t& newLookahead) {lookahead=newLookahead;}
      protected:
         bool enable;
         double_t lookahead;
      private:
   };

   class CERTI_EXPORT M_Disable_Time_Constrained : public Message {
      public:
         typedef Message Super;
         M_Disable_Time_Constrained();
         virtual ~M_Disable_Time_Constrained();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         void enableOn() {enable = true;}
         void enableOff() {enable = false;}
         bool isEnableOn() const {return enable;}
         const double_t& getLookahead() const {return lookahead;}
         void setLookahead(const double_t& newLookahead) {lookahead=newLookahead;}
      protected:
         bool enable;
         double_t lookahead;
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const double_t& getLookahead() const {return lookahead;}
         void setLookahead(const double_t& newLookahead) {lookahead=newLookahead;}
      protected:
         double_t lookahead;
      private:
   };

   class CERTI_EXPORT M_Query_Lookahead : public Message {
      public:
         typedef Message Super;
         M_Query_Lookahead();
         virtual ~M_Query_Lookahead();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const double_t& getLookahead() const {return lookahead;}
         void setLookahead(const double_t& newLookahead) {lookahead=newLookahead;}
      protected:
         double_t lookahead;
      private:
   };

   class CERTI_EXPORT M_Retract : public Message {
      public:
         typedef Message Super;
         M_Retract();
         virtual ~M_Retract();
      protected:
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

   class CERTI_EXPORT M_Time_Regulation_Enabled : public Message {
      public:
         typedef Message Super;
         M_Time_Regulation_Enabled();
         virtual ~M_Time_Regulation_Enabled();
      protected:
      private:
   };

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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const SpaceHandle& getSpace() const {return space;}
         void setSpace(const SpaceHandle& newSpace) {space=newSpace;}
         const uint32_t& getNbExtent() const {return nbExtent;}
         void setNbExtent(const uint32_t& newNbExtent) {nbExtent=newNbExtent;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
      protected:
         SpaceHandle space;
         uint32_t nbExtent;
         RegionHandle region;
      private:
   };

   class CERTI_EXPORT M_Ddm_Modify_Region : public Message {
      public:
         typedef Message Super;
         M_Ddm_Modify_Region();
         virtual ~M_Ddm_Modify_Region();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
      protected:
         RegionHandle region;//repeated M_Extents extent    readExtents(msgBuffer);
      private:
   };

   class CERTI_EXPORT M_Ddm_Delete_Region : public Message {
      public:
         typedef Message Super;
         M_Ddm_Delete_Region();
         virtual ~M_Ddm_Delete_Region();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
      protected:
         RegionHandle region;
      private:
   };

   class CERTI_EXPORT M_Ddm_Register_Object : public Message {
      public:
         typedef Message Super;
         M_Ddm_Register_Object();
         virtual ~M_Ddm_Register_Object();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         uint32_t getNumHandles() const {return handles.size();}
         void setNumHandles(uint32_t num) {handles.resize(num);}
         const AttributeHandle& getHandles(uint32_t rank) const {return handles[rank];}
         AttributeHandle& getHandles(uint32_t rank) {return handles[rank];}
         void setHandles(const AttributeHandle& newHandles, uint32_t rank) {handles[rank]=newHandles;}
      protected:
         ObjectClassHandle objectClass;
         ObjectHandle object;
         std::vector<AttributeHandle> handles;// repeated M_Regions   regions 
      private:
   };

   class CERTI_EXPORT M_Ddm_Associate_Region : public Message {
      public:
         typedef Message Super;
         M_Ddm_Associate_Region();
         virtual ~M_Ddm_Associate_Region();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
      protected:
         ObjectHandle object;
         RegionHandle region;
      private:
   };

   class CERTI_EXPORT M_Ddm_Unassociate_Region : public Message {
      public:
         typedef Message Super;
         M_Ddm_Unassociate_Region();
         virtual ~M_Ddm_Unassociate_Region();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         void passiveOn() {passive = true;}
         void passiveOff() {passive = false;}
         bool isPassiveOn() const {return passive;}
         uint32_t getNumHandles() const {return handles.size();}
         void setNumHandles(uint32_t num) {handles.resize(num);}
         const AttributeHandle& getHandles(uint32_t rank) const {return handles[rank];}
         AttributeHandle& getHandles(uint32_t rank) {return handles[rank];}
         void setHandles(const AttributeHandle& newHandles, uint32_t rank) {handles[rank]=newHandles;}
      protected:
         ObjectClassHandle objectClass;
         RegionHandle region;
         bool passive;
         std::vector<AttributeHandle> handles;
      private:
   };

   class CERTI_EXPORT M_Ddm_Unsubscribe_Attributes : public Message {
      public:
         typedef Message Super;
         M_Ddm_Unsubscribe_Attributes();
         virtual ~M_Ddm_Unsubscribe_Attributes();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         void passiveOn() {passive = true;}
         void passiveOff() {passive = false;}
         bool isPassiveOn() const {return passive;}
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const RegionHandle& getRegion() const {return region;}
         void setRegion(const RegionHandle& newRegion) {region=newRegion;}
         void passiveOn() {passive = true;}
         void passiveOff() {passive = false;}
         bool isPassiveOn() const {return passive;}
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
      protected:
      private:
   };
   // Support Services
   class CERTI_EXPORT M_Get_Object_Class_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Object_Class_Handle();
         virtual ~M_Get_Object_Class_Handle();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
      protected:
         ObjectClassHandle objectClass;
         std::string name;
         AttributeHandle attribute;
      private:
   };

   class CERTI_EXPORT M_Get_Object_Class_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Object_Class_Name();
         virtual ~M_Get_Object_Class_Name();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
      protected:
         ObjectClassHandle objectClass;
         std::string name;
         AttributeHandle attribute;
      private:
   };

   class CERTI_EXPORT M_Get_Attribute_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Attribute_Handle();
         virtual ~M_Get_Attribute_Handle();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
      protected:
         ObjectClassHandle objectClass;
         std::string name;
         AttributeHandle attribute;
      private:
   };

   class CERTI_EXPORT M_Get_Attribute_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Attribute_Name();
         virtual ~M_Get_Attribute_Name();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
      protected:
         ObjectClassHandle objectClass;
         std::string name;
         AttributeHandle attribute;
      private:
   };

   class CERTI_EXPORT M_Get_Interaction_Class_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Interaction_Class_Handle();
         virtual ~M_Get_Interaction_Class_Handle();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const ParameterHandle& getParameter() const {return parameter;}
         void setParameter(const ParameterHandle& newParameter) {parameter=newParameter;}
      protected:
         InteractionClassHandle interactionClass;
         std::string name;
         ParameterHandle parameter;
      private:
   };

   class CERTI_EXPORT M_Get_Interaction_Class_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Interaction_Class_Name();
         virtual ~M_Get_Interaction_Class_Name();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const ParameterHandle& getParameter() const {return parameter;}
         void setParameter(const ParameterHandle& newParameter) {parameter=newParameter;}
      protected:
         InteractionClassHandle interactionClass;
         std::string name;
         ParameterHandle parameter;
      private:
   };

   class CERTI_EXPORT M_Get_Parameter_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Parameter_Handle();
         virtual ~M_Get_Parameter_Handle();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const ParameterHandle& getParameter() const {return parameter;}
         void setParameter(const ParameterHandle& newParameter) {parameter=newParameter;}
      protected:
         InteractionClassHandle interactionClass;
         std::string name;
         ParameterHandle parameter;
      private:
   };

   class CERTI_EXPORT M_Get_Parameter_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Parameter_Name();
         virtual ~M_Get_Parameter_Name();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const ParameterHandle& getParameter() const {return parameter;}
         void setParameter(const ParameterHandle& newParameter) {parameter=newParameter;}
      protected:
         InteractionClassHandle interactionClass;
         std::string name;
         ParameterHandle parameter;
      private:
   };

   class CERTI_EXPORT M_Get_Object_Instance_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Object_Instance_Handle();
         virtual ~M_Get_Object_Instance_Handle();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
      protected:
         ObjectHandle object;
         std::string name;
      private:
   };

   class CERTI_EXPORT M_Get_Object_Instance_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Object_Instance_Name();
         virtual ~M_Get_Object_Instance_Name();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
      protected:
         ObjectHandle object;
         std::string name;
      private:
   };

   class CERTI_EXPORT M_Get_Space_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Space_Handle();
         virtual ~M_Get_Space_Handle();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const SpaceHandle& getSpace() const {return space;}
         void setSpace(const SpaceHandle& newSpace) {space=newSpace;}
      protected:
         std::string name;
         SpaceHandle space;
      private:
   };

   class CERTI_EXPORT M_Get_Space_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Space_Name();
         virtual ~M_Get_Space_Name();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const SpaceHandle& getSpace() const {return space;}
         void setSpace(const SpaceHandle& newSpace) {space=newSpace;}
      protected:
         std::string name;
         SpaceHandle space;
      private:
   };

   class CERTI_EXPORT M_Get_Dimension_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Dimension_Handle();
         virtual ~M_Get_Dimension_Handle();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const SpaceHandle& getSpace() const {return space;}
         void setSpace(const SpaceHandle& newSpace) {space=newSpace;}
         const DimensionHandle& getDimension() const {return dimension;}
         void setDimension(const DimensionHandle& newDimension) {dimension=newDimension;}
      protected:
         std::string name;
         SpaceHandle space;
         DimensionHandle dimension;
      private:
   };

   class CERTI_EXPORT M_Get_Dimension_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Dimension_Name();
         virtual ~M_Get_Dimension_Name();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const SpaceHandle& getSpace() const {return space;}
         void setSpace(const SpaceHandle& newSpace) {space=newSpace;}
         const DimensionHandle& getDimension() const {return dimension;}
         void setDimension(const DimensionHandle& newDimension) {dimension=newDimension;}
      protected:
         std::string name;
         SpaceHandle space;
         DimensionHandle dimension;
      private:
   };

   class CERTI_EXPORT M_Get_Attribute_Space_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Attribute_Space_Handle();
         virtual ~M_Get_Attribute_Space_Handle();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const AttributeHandle& getAttribute() const {return attribute;}
         void setAttribute(const AttributeHandle& newAttribute) {attribute=newAttribute;}
         const SpaceHandle& getSpace() const {return space;}
         void setSpace(const SpaceHandle& newSpace) {space=newSpace;}
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const ObjectClassHandle& getObjectClass() const {return objectClass;}
         void setObjectClass(const ObjectClassHandle& newObjectClass) {objectClass=newObjectClass;}
         const ObjectHandle& getObject() const {return object;}
         void setObject(const ObjectHandle& newObject) {object=newObject;}
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const InteractionClassHandle& getInteractionClass() const {return interactionClass;}
         void setInteractionClass(const InteractionClassHandle& newInteractionClass) {interactionClass=newInteractionClass;}
         const SpaceHandle& getSpace() const {return space;}
         void setSpace(const SpaceHandle& newSpace) {space=newSpace;}
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const TransportType& getTransport() const {return transport;}
         void setTransport(const TransportType& newTransport) {transport=newTransport;}
      protected:
         std::string name;
         TransportType transport;
      private:
   };

   class CERTI_EXPORT M_Get_Transportation_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Transportation_Name();
         virtual ~M_Get_Transportation_Name();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const TransportType& getTransport() const {return transport;}
         void setTransport(const TransportType& newTransport) {transport=newTransport;}
      protected:
         std::string name;
         TransportType transport;
      private:
   };

   class CERTI_EXPORT M_Get_Ordering_Handle : public Message {
      public:
         typedef Message Super;
         M_Get_Ordering_Handle();
         virtual ~M_Get_Ordering_Handle();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const OrderType& getOrder() const {return order;}
         void setOrder(const OrderType& newOrder) {order=newOrder;}
      protected:
         std::string name;
         OrderType order;
      private:
   };

   class CERTI_EXPORT M_Get_Ordering_Name : public Message {
      public:
         typedef Message Super;
         M_Get_Ordering_Name();
         virtual ~M_Get_Ordering_Name();
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const std::string& getName() const {return name;}
         void setName(const std::string& newName) {name=newName;}
         const OrderType& getOrder() const {return order;}
         void setOrder(const OrderType& newOrder) {order=newOrder;}
      protected:
         std::string name;
         OrderType order;
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
         virtual void serialize(MessageBuffer& msgBuffer);
         virtual void deserialize(MessageBuffer& msgBuffer);
         // specific Getter(s)/Setter(s)
         const bool& getMultiple() const {return multiple;}
         void setMultiple(const bool& newMultiple) {multiple=newMultiple;}
         const double_t& getMinTickTime() const {return minTickTime;}
         void setMinTickTime(const double_t& newMinTickTime) {minTickTime=newMinTickTime;}
         const double_t& getMaxTickTime() const {return maxTickTime;}
         void setMaxTickTime(const double_t& newMaxTickTime) {maxTickTime=newMaxTickTime;}
      protected:
         bool multiple;
         double_t minTickTime;
         double_t maxTickTime;
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

   class CERTI_EXPORT M_Factory {
      public:
         static Message* create(M_Type type) throw (RTIinternalError);
         static Message* receive(StreamType stream) throw (RTIinternalError);
      protected:
      private:
   };

} // end of namespace certi 
// M_CLASSES_HH
#endif
