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

#ifndef CERTI_NM_CLASSES_HH
#define CERTI_NM_CLASSES_HH
#include "NetworkMessage.hh"

namespace certi {
/**
 * Network Message factory
 */
class NM_Factory {
public:
	static NetworkMessage* create(NetworkMessage::Message_T type);
};

/*<BEGIN>---------- Not_Used ------------<BEGIN>*/
class CERTI_EXPORT NM_Not_Used : public NetworkMessage {
  public:
       NM_Not_Used();
       virtual ~NM_Not_Used();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Not_Used ------------<END>*/

/*<BEGIN>---------- Close_Connexion ------------<BEGIN>*/
class CERTI_EXPORT NM_Close_Connexion : public NetworkMessage {
  public:
       NM_Close_Connexion();
       virtual ~NM_Close_Connexion();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Close_Connexion ------------<END>*/

/*<BEGIN>---------- Message_Null ------------<BEGIN>*/
class CERTI_EXPORT NM_Message_Null : public NetworkMessage {
  public:
       NM_Message_Null();
       virtual ~NM_Message_Null();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Message_Null ------------<END>*/

/*<BEGIN>---------- Create_Federation_Execution ------------<BEGIN>*/
class CERTI_EXPORT NM_Create_Federation_Execution : public NetworkMessage {
  public:
       NM_Create_Federation_Execution();
       virtual ~NM_Create_Federation_Execution();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */	  
	  std::string federationName ;
	  std::string FEDid ;
  private:
};

/*<END>---------- Create_Federation_Execution ------------<END>*/

/*<BEGIN>---------- Destroy_Federation_Execution ------------<BEGIN>*/
class CERTI_EXPORT NM_Destroy_Federation_Execution : public NetworkMessage {
  public:
       NM_Destroy_Federation_Execution();
       virtual ~NM_Destroy_Federation_Execution();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
      /* specific field */	 
	  std::string federationName;
  private:
};

/*<END>---------- Destroy_Federation_Execution ------------<END>*/

/*<BEGIN>---------- Join_Federation_Execution ------------<BEGIN>*/
class CERTI_EXPORT NM_Join_Federation_Execution : public NetworkMessage {
  public:
       NM_Join_Federation_Execution();
       virtual ~NM_Join_Federation_Execution();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
	  int32_t  numberOfRegulators;
	  uint32_t multicastAddress;
	  uint32_t bestEffortAddress;
	  uint32_t bestEffortPeer;
	  std::string federationName;
	  std::string federateName;
  private:
};

/*<END>---------- Join_Federation_Execution ------------<END>*/

/*<BEGIN>---------- Resign_Federation_Execution ------------<BEGIN>*/
class CERTI_EXPORT NM_Resign_Federation_Execution : public NetworkMessage {
  public:
       NM_Resign_Federation_Execution();
       virtual ~NM_Resign_Federation_Execution();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Resign_Federation_Execution ------------<END>*/

/*<BEGIN>---------- Set_Time_Regulating ------------<BEGIN>*/
class CERTI_EXPORT NM_Set_Time_Regulating : public NetworkMessage {
  public:
       NM_Set_Time_Regulating();
       virtual ~NM_Set_Time_Regulating();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
	  bool   regulating;
  private:
};

/*<END>---------- Set_Time_Regulating ------------<END>*/

/*<BEGIN>---------- Set_Time_Constrained ------------<BEGIN>*/
class CERTI_EXPORT NM_Set_Time_Constrained : public NetworkMessage {
  public:
       NM_Set_Time_Constrained();
       virtual ~NM_Set_Time_Constrained();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */	
	  bool   constrained;
  private:
};

/*<END>---------- Set_Time_Constrained ------------<END>*/

/*<BEGIN>---------- Register_Federation_Synchronization_Point ------------<BEGIN>*/
class CERTI_EXPORT NM_Register_Federation_Synchronization_Point : public NetworkMessage {
  public:
       NM_Register_Federation_Synchronization_Point();
       virtual ~NM_Register_Federation_Synchronization_Point();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
      /* specific field */	  
	  	
	  std::string tag;
	  bool hasHandleArray;
	  UShort handleArraySize ;
	  AttributeHandle handleArray[MAX_ATTRIBUTES_PER_CLASS];	  
	  
  private:
};

/*<END>---------- Register_Federation_Synchronization_Point ------------<END>*/

/*<BEGIN>---------- Synchronization_Point_Registration_Succeeded ------------<BEGIN>*/
class CERTI_EXPORT NM_Synchronization_Point_Registration_Succeeded : public NetworkMessage {
  public:
       NM_Synchronization_Point_Registration_Succeeded();
       virtual ~NM_Synchronization_Point_Registration_Succeeded();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
      /* specific field */	 
	  
  private:
};

/*<END>---------- Synchronization_Point_Registration_Succeeded ------------<END>*/

/*<BEGIN>---------- Announce_Synchronization_Point ------------<BEGIN>*/
class CERTI_EXPORT NM_Announce_Synchronization_Point : public NetworkMessage {
  public:
       NM_Announce_Synchronization_Point();
       virtual ~NM_Announce_Synchronization_Point();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
      /* specific field */	  
	  	
	  std::string tag;
  private:
};

/*<END>---------- Announce_Synchronization_Point ------------<END>*/

/*<BEGIN>---------- Synchronization_Point_Achieved ------------<BEGIN>*/
class CERTI_EXPORT NM_Synchronization_Point_Achieved : public NetworkMessage {
  public:
       NM_Synchronization_Point_Achieved();
       virtual ~NM_Synchronization_Point_Achieved();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */	  
	  
  private:
};

/*<END>---------- Synchronization_Point_Achieved ------------<END>*/

/*<BEGIN>---------- Federation_Synchronized ------------<BEGIN>*/
class CERTI_EXPORT NM_Federation_Synchronized : public NetworkMessage {
  public:
       NM_Federation_Synchronized();
       virtual ~NM_Federation_Synchronized();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */	 
	  
  private:
};

/*<END>---------- Federation_Synchronized ------------<END>*/

/*<BEGIN>---------- Request_Federation_Save ------------<BEGIN>*/
class CERTI_EXPORT NM_Request_Federation_Save : public NetworkMessage {
  public:
       NM_Request_Federation_Save();
       virtual ~NM_Request_Federation_Save();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
	   
  private:
};

/*<END>---------- Request_Federation_Save ------------<END>*/

/*<BEGIN>---------- Federate_Save_Begun ------------<BEGIN>*/
class CERTI_EXPORT NM_Federate_Save_Begun : public NetworkMessage {
  public:
       NM_Federate_Save_Begun();
       virtual ~NM_Federate_Save_Begun();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
	  
  private:
};

/*<END>---------- Federate_Save_Begun ------------<END>*/

/*<BEGIN>---------- Federate_Save_Complete ------------<BEGIN>*/
class CERTI_EXPORT NM_Federate_Save_Complete : public NetworkMessage {
  public:
       NM_Federate_Save_Complete();
       virtual ~NM_Federate_Save_Complete();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Federate_Save_Complete ------------<END>*/

/*<BEGIN>---------- Federate_Save_Not_Complete ------------<BEGIN>*/
class CERTI_EXPORT NM_Federate_Save_Not_Complete : public NetworkMessage {
  public:
       NM_Federate_Save_Not_Complete();
       virtual ~NM_Federate_Save_Not_Complete();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Federate_Save_Not_Complete ------------<END>*/

/*<BEGIN>---------- Initiate_Federate_Save ------------<BEGIN>*/
class CERTI_EXPORT NM_Initiate_Federate_Save : public NetworkMessage {
  public:
       NM_Initiate_Federate_Save();
       virtual ~NM_Initiate_Federate_Save();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
	  
  private:
};

/*<END>---------- Initiate_Federate_Save ------------<END>*/

/*<BEGIN>---------- Federation_Saved ------------<BEGIN>*/
class CERTI_EXPORT NM_Federation_Saved : public NetworkMessage {
  public:
       NM_Federation_Saved();
       virtual ~NM_Federation_Saved();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Federation_Saved ------------<END>*/

/*<BEGIN>---------- Federation_Not_Saved ------------<BEGIN>*/
class CERTI_EXPORT NM_Federation_Not_Saved : public NetworkMessage {
  public:
       NM_Federation_Not_Saved();
       virtual ~NM_Federation_Not_Saved();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Federation_Not_Saved ------------<END>*/

/*<BEGIN>---------- Request_Federation_Restore ------------<BEGIN>*/
class CERTI_EXPORT NM_Request_Federation_Restore : public NetworkMessage {
  public:
       NM_Request_Federation_Restore();
       virtual ~NM_Request_Federation_Restore();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
	  
  private:
};

/*<END>---------- Request_Federation_Restore ------------<END>*/

/*<BEGIN>---------- Federate_Restore_Complete ------------<BEGIN>*/
class CERTI_EXPORT NM_Federate_Restore_Complete : public NetworkMessage {
  public:
       NM_Federate_Restore_Complete();
       virtual ~NM_Federate_Restore_Complete();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Federate_Restore_Complete ------------<END>*/

/*<BEGIN>---------- Federate_Restore_Not_Complete ------------<BEGIN>*/
class CERTI_EXPORT NM_Federate_Restore_Not_Complete : public NetworkMessage {
  public:
       NM_Federate_Restore_Not_Complete();
       virtual ~NM_Federate_Restore_Not_Complete();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Federate_Restore_Not_Complete ------------<END>*/

/*<BEGIN>---------- Request_Federation_Restore_Succeeded ------------<BEGIN>*/
class CERTI_EXPORT NM_Request_Federation_Restore_Succeeded : public NetworkMessage {
  public:
       NM_Request_Federation_Restore_Succeeded();
       virtual ~NM_Request_Federation_Restore_Succeeded();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Request_Federation_Restore_Succeeded ------------<END>*/

/*<BEGIN>---------- Request_Federation_Restore_Failed ------------<BEGIN>*/
class CERTI_EXPORT NM_Request_Federation_Restore_Failed : public NetworkMessage {
  public:
       NM_Request_Federation_Restore_Failed();
       virtual ~NM_Request_Federation_Restore_Failed();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
	  std::string tag;
  private:
};

/*<END>---------- Request_Federation_Restore_Failed ------------<END>*/

/*<BEGIN>---------- Federation_Restore_Begun ------------<BEGIN>*/
class CERTI_EXPORT NM_Federation_Restore_Begun : public NetworkMessage {
  public:
       NM_Federation_Restore_Begun();
       virtual ~NM_Federation_Restore_Begun();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Federation_Restore_Begun ------------<END>*/

/*<BEGIN>---------- Initiate_Federate_Restore ------------<BEGIN>*/
class CERTI_EXPORT NM_Initiate_Federate_Restore : public NetworkMessage {
  public:
       NM_Initiate_Federate_Restore();
       virtual ~NM_Initiate_Federate_Restore();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Initiate_Federate_Restore ------------<END>*/

/*<BEGIN>---------- Federation_Restored ------------<BEGIN>*/
class CERTI_EXPORT NM_Federation_Restored : public NetworkMessage {
  public:
       NM_Federation_Restored();
       virtual ~NM_Federation_Restored();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Federation_Restored ------------<END>*/

/*<BEGIN>---------- Federation_Not_Restored ------------<BEGIN>*/
class CERTI_EXPORT NM_Federation_Not_Restored : public NetworkMessage {
  public:
       NM_Federation_Not_Restored();
       virtual ~NM_Federation_Not_Restored();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Federation_Not_Restored ------------<END>*/

/*<BEGIN>---------- Publish_Object_Class ------------<BEGIN>*/
class CERTI_EXPORT NM_Publish_Object_Class : public NetworkMessage {
  public:
       NM_Publish_Object_Class();
       virtual ~NM_Publish_Object_Class();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
	  bool hasHandleArray;
	  UShort handleArraySize ;
	  AttributeHandle handleArray[MAX_ATTRIBUTES_PER_CLASS];	
  private:
};

/*<END>---------- Publish_Object_Class ------------<END>*/

/*<BEGIN>---------- Unpublish_Object_Class ------------<BEGIN>*/
class CERTI_EXPORT NM_Unpublish_Object_Class : public NetworkMessage {
  public:
       NM_Unpublish_Object_Class();
       virtual ~NM_Unpublish_Object_Class();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
	  int32_t objectClass;
  private:
};

/*<END>---------- Unpublish_Object_Class ------------<END>*/

/*<BEGIN>---------- Publish_Interaction_Class ------------<BEGIN>*/
class CERTI_EXPORT NM_Publish_Interaction_Class : public NetworkMessage {
  public:
       NM_Publish_Interaction_Class();
       virtual ~NM_Publish_Interaction_Class();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
	  int32_t interactionClass;
  private:
};

/*<END>---------- Publish_Interaction_Class ------------<END>*/

/*<BEGIN>---------- Unpublish_Interaction_Class ------------<BEGIN>*/
class CERTI_EXPORT NM_Unpublish_Interaction_Class : public NetworkMessage {
  public:
       NM_Unpublish_Interaction_Class();
       virtual ~NM_Unpublish_Interaction_Class();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
	  int32_t interactionClass;
  private:
};

/*<END>---------- Unpublish_Interaction_Class ------------<END>*/

/*<BEGIN>---------- Subscribe_Object_Class ------------<BEGIN>*/
class CERTI_EXPORT NM_Subscribe_Object_Class : public NetworkMessage {
  public:
       NM_Subscribe_Object_Class();
       virtual ~NM_Subscribe_Object_Class();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
      /* specific field */
	  bool hasHandleArray;
	  UShort handleArraySize ;
	  AttributeHandle handleArray[MAX_ATTRIBUTES_PER_CLASS];
  private:
};

/*<END>---------- Subscribe_Object_Class ------------<END>*/

/*<BEGIN>---------- Unsubscribe_Object_Class ------------<BEGIN>*/
class CERTI_EXPORT NM_Unsubscribe_Object_Class : public NetworkMessage {
  public:
       NM_Unsubscribe_Object_Class();
       virtual ~NM_Unsubscribe_Object_Class();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
	  int32_t objectClass;
  private:
};

/*<END>---------- Unsubscribe_Object_Class ------------<END>*/

/*<BEGIN>---------- Subscribe_Interaction_Class ------------<BEGIN>*/
class CERTI_EXPORT NM_Subscribe_Interaction_Class : public NetworkMessage {
  public:
       NM_Subscribe_Interaction_Class();
       virtual ~NM_Subscribe_Interaction_Class();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
	  int32_t interactionClass;
  private:
};

/*<END>---------- Subscribe_Interaction_Class ------------<END>*/

/*<BEGIN>---------- Unsubscribe_Interaction_Class ------------<BEGIN>*/
class CERTI_EXPORT NM_Unsubscribe_Interaction_Class : public NetworkMessage {
  public:
       NM_Unsubscribe_Interaction_Class();
       virtual ~NM_Unsubscribe_Interaction_Class();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
	  int32_t interactionClass;
  private:
};

/*<END>---------- Unsubscribe_Interaction_Class ------------<END>*/

/*<BEGIN>---------- Turn_Interactions_On ------------<BEGIN>*/
class CERTI_EXPORT NM_Turn_Interactions_On : public NetworkMessage {
  public:
       NM_Turn_Interactions_On();
       virtual ~NM_Turn_Interactions_On();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
	  int32_t interactionClass;
  private:
};

/*<END>---------- Turn_Interactions_On ------------<END>*/

/*<BEGIN>---------- Turn_Interactions_Off ------------<BEGIN>*/
class CERTI_EXPORT NM_Turn_Interactions_Off : public NetworkMessage {
  public:
       NM_Turn_Interactions_Off();
       virtual ~NM_Turn_Interactions_Off();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
	  int32_t interactionClass;
  private:
};

/*<END>---------- Turn_Interactions_Off ------------<END>*/

/*<BEGIN>---------- Register_Object ------------<BEGIN>*/
class CERTI_EXPORT NM_Register_Object : public NetworkMessage {
  public:
       NM_Register_Object();
       virtual ~NM_Register_Object();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Register_Object ------------<END>*/

/*<BEGIN>---------- Discover_Object ------------<BEGIN>*/
class CERTI_EXPORT NM_Discover_Object : public NetworkMessage {
  public:
       NM_Discover_Object();
       virtual ~NM_Discover_Object();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Discover_Object ------------<END>*/

/*<BEGIN>---------- Update_Attribute_Values ------------<BEGIN>*/
class CERTI_EXPORT NM_Update_Attribute_Values : public NetworkMessage {
  public:
       NM_Update_Attribute_Values();
       virtual ~NM_Update_Attribute_Values();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Update_Attribute_Values ------------<END>*/

/*<BEGIN>---------- Reflect_Attribute_Values ------------<BEGIN>*/
class CERTI_EXPORT NM_Reflect_Attribute_Values : public NetworkMessage {
  public:
       NM_Reflect_Attribute_Values();
       virtual ~NM_Reflect_Attribute_Values();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Reflect_Attribute_Values ------------<END>*/

/*<BEGIN>---------- Send_Interaction ------------<BEGIN>*/
class CERTI_EXPORT NM_Send_Interaction : public NetworkMessage {
  public:
       NM_Send_Interaction();
       virtual ~NM_Send_Interaction();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Send_Interaction ------------<END>*/

/*<BEGIN>---------- Receive_Interaction ------------<BEGIN>*/
class CERTI_EXPORT NM_Receive_Interaction : public NetworkMessage {
  public:
       NM_Receive_Interaction();
       virtual ~NM_Receive_Interaction();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Receive_Interaction ------------<END>*/

/*<BEGIN>---------- Delete_Object ------------<BEGIN>*/
class CERTI_EXPORT NM_Delete_Object : public NetworkMessage {
  public:
       NM_Delete_Object();
       virtual ~NM_Delete_Object();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Delete_Object ------------<END>*/

/*<BEGIN>---------- Remove_Object ------------<BEGIN>*/
class CERTI_EXPORT NM_Remove_Object : public NetworkMessage {
  public:
       NM_Remove_Object();
       virtual ~NM_Remove_Object();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Remove_Object ------------<END>*/

/*<BEGIN>---------- Change_Attribute_Transport_Type ------------<BEGIN>*/
class CERTI_EXPORT NM_Change_Attribute_Transport_Type : public NetworkMessage {
  public:
       NM_Change_Attribute_Transport_Type();
       virtual ~NM_Change_Attribute_Transport_Type();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Change_Attribute_Transport_Type ------------<END>*/

/*<BEGIN>---------- Change_Attribute_Order_Type ------------<BEGIN>*/
class CERTI_EXPORT NM_Change_Attribute_Order_Type : public NetworkMessage {
  public:
       NM_Change_Attribute_Order_Type();
       virtual ~NM_Change_Attribute_Order_Type();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Change_Attribute_Order_Type ------------<END>*/

/*<BEGIN>---------- Change_Interaction_Transport_Type ------------<BEGIN>*/
class CERTI_EXPORT NM_Change_Interaction_Transport_Type : public NetworkMessage {
  public:
       NM_Change_Interaction_Transport_Type();
       virtual ~NM_Change_Interaction_Transport_Type();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Change_Interaction_Transport_Type ------------<END>*/

/*<BEGIN>---------- Change_Interaction_Order_Type ------------<BEGIN>*/
class CERTI_EXPORT NM_Change_Interaction_Order_Type : public NetworkMessage {
  public:
       NM_Change_Interaction_Order_Type();
       virtual ~NM_Change_Interaction_Order_Type();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Change_Interaction_Order_Type ------------<END>*/

/*<BEGIN>---------- Request_Class_Attribute_Value_Update ------------<BEGIN>*/
class CERTI_EXPORT NM_Request_Class_Attribute_Value_Update : public NetworkMessage {
  public:
       NM_Request_Class_Attribute_Value_Update();
       virtual ~NM_Request_Class_Attribute_Value_Update();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Request_Class_Attribute_Value_Update ------------<END>*/

/*<BEGIN>---------- Request_Object_Attribute_Value_Update ------------<BEGIN>*/
class CERTI_EXPORT NM_Request_Object_Attribute_Value_Update : public NetworkMessage {
  public:
       NM_Request_Object_Attribute_Value_Update();
       virtual ~NM_Request_Object_Attribute_Value_Update();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Request_Object_Attribute_Value_Update ------------<END>*/

/*<BEGIN>---------- Is_Attribute_Owned_By_Federate ------------<BEGIN>*/
class CERTI_EXPORT NM_Is_Attribute_Owned_By_Federate : public NetworkMessage {
  public:
       NM_Is_Attribute_Owned_By_Federate();
       virtual ~NM_Is_Attribute_Owned_By_Federate();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Is_Attribute_Owned_By_Federate ------------<END>*/

/*<BEGIN>---------- Query_Attribute_Ownership ------------<BEGIN>*/
class CERTI_EXPORT NM_Query_Attribute_Ownership : public NetworkMessage {
  public:
       NM_Query_Attribute_Ownership();
       virtual ~NM_Query_Attribute_Ownership();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Query_Attribute_Ownership ------------<END>*/

/*<BEGIN>---------- Attribute_Is_Not_Owned ------------<BEGIN>*/
class CERTI_EXPORT NM_Attribute_Is_Not_Owned : public NetworkMessage {
  public:
       NM_Attribute_Is_Not_Owned();
       virtual ~NM_Attribute_Is_Not_Owned();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Attribute_Is_Not_Owned ------------<END>*/

/*<BEGIN>---------- Inform_Attribute_Ownership ------------<BEGIN>*/
class CERTI_EXPORT NM_Inform_Attribute_Ownership : public NetworkMessage {
  public:
       NM_Inform_Attribute_Ownership();
       virtual ~NM_Inform_Attribute_Ownership();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Inform_Attribute_Ownership ------------<END>*/

/*<BEGIN>---------- Negotiated_Attribute_Ownership_Divestiture ------------<BEGIN>*/
class CERTI_EXPORT NM_Negotiated_Attribute_Ownership_Divestiture : public NetworkMessage {
  public:
       NM_Negotiated_Attribute_Ownership_Divestiture();
       virtual ~NM_Negotiated_Attribute_Ownership_Divestiture();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Negotiated_Attribute_Ownership_Divestiture ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Acquisition_Notification ------------<BEGIN>*/
class CERTI_EXPORT NM_Attribute_Ownership_Acquisition_Notification : public NetworkMessage {
  public:
       NM_Attribute_Ownership_Acquisition_Notification();
       virtual ~NM_Attribute_Ownership_Acquisition_Notification();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Attribute_Ownership_Acquisition_Notification ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Divestiture_Notification ------------<BEGIN>*/
class CERTI_EXPORT NM_Attribute_Ownership_Divestiture_Notification : public NetworkMessage {
  public:
       NM_Attribute_Ownership_Divestiture_Notification();
       virtual ~NM_Attribute_Ownership_Divestiture_Notification();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Attribute_Ownership_Divestiture_Notification ------------<END>*/

/*<BEGIN>---------- Request_Attribute_Ownership_Assumption ------------<BEGIN>*/
class CERTI_EXPORT NM_Request_Attribute_Ownership_Assumption : public NetworkMessage {
  public:
       NM_Request_Attribute_Ownership_Assumption();
       virtual ~NM_Request_Attribute_Ownership_Assumption();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Request_Attribute_Ownership_Assumption ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Unavailable ------------<BEGIN>*/
class CERTI_EXPORT NM_Attribute_Ownership_Unavailable : public NetworkMessage {
  public:
       NM_Attribute_Ownership_Unavailable();
       virtual ~NM_Attribute_Ownership_Unavailable();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Attribute_Ownership_Unavailable ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Acquisition_If_Available ------------<BEGIN>*/
class CERTI_EXPORT NM_Attribute_Ownership_Acquisition_If_Available : public NetworkMessage {
  public:
       NM_Attribute_Ownership_Acquisition_If_Available();
       virtual ~NM_Attribute_Ownership_Acquisition_If_Available();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Attribute_Ownership_Acquisition_If_Available ------------<END>*/

/*<BEGIN>---------- Unconditional_Attribute_Ownership_Divestiture ------------<BEGIN>*/
class CERTI_EXPORT NM_Unconditional_Attribute_Ownership_Divestiture : public NetworkMessage {
  public:
       NM_Unconditional_Attribute_Ownership_Divestiture();
       virtual ~NM_Unconditional_Attribute_Ownership_Divestiture();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Unconditional_Attribute_Ownership_Divestiture ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Acquisition ------------<BEGIN>*/
class CERTI_EXPORT NM_Attribute_Ownership_Acquisition : public NetworkMessage {
  public:
       NM_Attribute_Ownership_Acquisition();
       virtual ~NM_Attribute_Ownership_Acquisition();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Attribute_Ownership_Acquisition ------------<END>*/

/*<BEGIN>---------- Request_Attribute_Ownership_Release ------------<BEGIN>*/
class CERTI_EXPORT NM_Request_Attribute_Ownership_Release : public NetworkMessage {
  public:
       NM_Request_Attribute_Ownership_Release();
       virtual ~NM_Request_Attribute_Ownership_Release();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Request_Attribute_Ownership_Release ------------<END>*/

/*<BEGIN>---------- Cancel_Negotiated_Attribute_Ownership_Divestiture ------------<BEGIN>*/
class CERTI_EXPORT NM_Cancel_Negotiated_Attribute_Ownership_Divestiture : public NetworkMessage {
  public:
       NM_Cancel_Negotiated_Attribute_Ownership_Divestiture();
       virtual ~NM_Cancel_Negotiated_Attribute_Ownership_Divestiture();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Cancel_Negotiated_Attribute_Ownership_Divestiture ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Release_Response ------------<BEGIN>*/
class CERTI_EXPORT NM_Attribute_Ownership_Release_Response : public NetworkMessage {
  public:
       NM_Attribute_Ownership_Release_Response();
       virtual ~NM_Attribute_Ownership_Release_Response();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Attribute_Ownership_Release_Response ------------<END>*/

/*<BEGIN>---------- Cancel_Attribute_Ownership_Acquisition ------------<BEGIN>*/
class CERTI_EXPORT NM_Cancel_Attribute_Ownership_Acquisition : public NetworkMessage {
  public:
       NM_Cancel_Attribute_Ownership_Acquisition();
       virtual ~NM_Cancel_Attribute_Ownership_Acquisition();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Cancel_Attribute_Ownership_Acquisition ------------<END>*/

/*<BEGIN>---------- Confirm_Attribute_Ownership_Acquisition_Cancellation ------------<BEGIN>*/
class CERTI_EXPORT NM_Confirm_Attribute_Ownership_Acquisition_Cancellation : public NetworkMessage {
  public:
       NM_Confirm_Attribute_Ownership_Acquisition_Cancellation();
       virtual ~NM_Confirm_Attribute_Ownership_Acquisition_Cancellation();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Confirm_Attribute_Ownership_Acquisition_Cancellation ------------<END>*/

/*<BEGIN>---------- Ddm_Create_Region ------------<BEGIN>*/
class CERTI_EXPORT NM_Ddm_Create_Region : public NetworkMessage {
  public:
       NM_Ddm_Create_Region();
       virtual ~NM_Ddm_Create_Region();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Ddm_Create_Region ------------<END>*/

/*<BEGIN>---------- Ddm_Modify_Region ------------<BEGIN>*/
class CERTI_EXPORT NM_Ddm_Modify_Region : public NetworkMessage {
  public:
       NM_Ddm_Modify_Region();
       virtual ~NM_Ddm_Modify_Region();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Ddm_Modify_Region ------------<END>*/

/*<BEGIN>---------- Ddm_Delete_Region ------------<BEGIN>*/
class CERTI_EXPORT NM_Ddm_Delete_Region : public NetworkMessage {
  public:
       NM_Ddm_Delete_Region();
       virtual ~NM_Ddm_Delete_Region();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Ddm_Delete_Region ------------<END>*/

/*<BEGIN>---------- Ddm_Associate_Region ------------<BEGIN>*/
class CERTI_EXPORT NM_Ddm_Associate_Region : public NetworkMessage {
  public:
       NM_Ddm_Associate_Region();
       virtual ~NM_Ddm_Associate_Region();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Ddm_Associate_Region ------------<END>*/

/*<BEGIN>---------- Ddm_Register_Object ------------<BEGIN>*/
class CERTI_EXPORT NM_Ddm_Register_Object : public NetworkMessage {
  public:
       NM_Ddm_Register_Object();
       virtual ~NM_Ddm_Register_Object();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Ddm_Register_Object ------------<END>*/

/*<BEGIN>---------- Ddm_Unassociate_Region ------------<BEGIN>*/
class CERTI_EXPORT NM_Ddm_Unassociate_Region : public NetworkMessage {
  public:
       NM_Ddm_Unassociate_Region();
       virtual ~NM_Ddm_Unassociate_Region();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Ddm_Unassociate_Region ------------<END>*/

/*<BEGIN>---------- Ddm_Subscribe_Attributes ------------<BEGIN>*/
class CERTI_EXPORT NM_Ddm_Subscribe_Attributes : public NetworkMessage {
  public:
       NM_Ddm_Subscribe_Attributes();
       virtual ~NM_Ddm_Subscribe_Attributes();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Ddm_Subscribe_Attributes ------------<END>*/

/*<BEGIN>---------- Ddm_Unsubscribe_Attributes ------------<BEGIN>*/
class CERTI_EXPORT NM_Ddm_Unsubscribe_Attributes : public NetworkMessage {
  public:
       NM_Ddm_Unsubscribe_Attributes();
       virtual ~NM_Ddm_Unsubscribe_Attributes();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Ddm_Unsubscribe_Attributes ------------<END>*/

/*<BEGIN>---------- Ddm_Subscribe_Interaction ------------<BEGIN>*/
class CERTI_EXPORT NM_Ddm_Subscribe_Interaction : public NetworkMessage {
  public:
       NM_Ddm_Subscribe_Interaction();
       virtual ~NM_Ddm_Subscribe_Interaction();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Ddm_Subscribe_Interaction ------------<END>*/

/*<BEGIN>---------- Ddm_Unsubscribe_Interaction ------------<BEGIN>*/
class CERTI_EXPORT NM_Ddm_Unsubscribe_Interaction : public NetworkMessage {
  public:
       NM_Ddm_Unsubscribe_Interaction();
       virtual ~NM_Ddm_Unsubscribe_Interaction();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Ddm_Unsubscribe_Interaction ------------<END>*/

/*<BEGIN>---------- Provide_Attribute_Value_Update ------------<BEGIN>*/
class CERTI_EXPORT NM_Provide_Attribute_Value_Update : public NetworkMessage {
  public:
       NM_Provide_Attribute_Value_Update();
       virtual ~NM_Provide_Attribute_Value_Update();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Provide_Attribute_Value_Update ------------<END>*/

/*<BEGIN>---------- Get_Fed_File ------------<BEGIN>*/
class CERTI_EXPORT NM_Get_Fed_File : public NetworkMessage {
  public:
       NM_Get_Fed_File();
       virtual ~NM_Get_Fed_File();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Get_Fed_File ------------<END>*/

/*<BEGIN>---------- Last ------------<BEGIN>*/
class CERTI_EXPORT NM_Last : public NetworkMessage {
  public:
       NM_Last();
       virtual ~NM_Last();
       virtual void serialize();
       virtual void deserialize();
       /* specific Getter/Setter */
  protected:
       /* specific field */
  private:
};

/*<END>---------- Last ------------<END>*/

} /* end of namespace certi */

#endif
