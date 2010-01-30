// Generated on 2010 January Sat, 30 at 19:13:24 by the CERTI message generator
#include <vector>
#include <string>
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

   M_Create_Federation_Execution::M_Create_Federation_Execution() {
      this->name = "M_Create_Federation_Execution";
      this->type = CREATE_FEDERATION_EXECUTION;
   }

   M_Create_Federation_Execution::~M_Create_Federation_Execution() {
   }

   M_Destroy_Federation_Execution::M_Destroy_Federation_Execution() {
      this->name = "M_Destroy_Federation_Execution";
      this->type = DESTROY_FEDERATION_EXECUTION;
   }

   M_Destroy_Federation_Execution::~M_Destroy_Federation_Execution() {
   }

   M_Join_Federation_Execution::M_Join_Federation_Execution() {
      this->name = "M_Join_Federation_Execution";
      this->type = JOIN_FEDERATION_EXECUTION;
   }

   M_Join_Federation_Execution::~M_Join_Federation_Execution() {
   }

   M_Resign_Federation_Execution::M_Resign_Federation_Execution() {
      this->name = "M_Resign_Federation_Execution";
      this->type = RESIGN_FEDERATION_EXECUTION;
   }

   M_Resign_Federation_Execution::~M_Resign_Federation_Execution() {
   }

   M_Register_Federation_Synchronization_Point::M_Register_Federation_Synchronization_Point() {
      this->name = "M_Register_Federation_Synchronization_Point";
      this->type = REGISTER_FEDERATION_SYNCHRONIZATION_POINT;
   }

   M_Register_Federation_Synchronization_Point::~M_Register_Federation_Synchronization_Point() {
   }

   M_Synchronization_Point_Registration_Failed::M_Synchronization_Point_Registration_Failed() {
      this->name = "M_Synchronization_Point_Registration_Failed";
      this->type = SYNCHRONIZATION_POINT_REGISTRATION_FAILED;
   }

   M_Synchronization_Point_Registration_Failed::~M_Synchronization_Point_Registration_Failed() {
   }

   M_Synchronization_Point_Registration_Succeeded::M_Synchronization_Point_Registration_Succeeded() {
      this->name = "M_Synchronization_Point_Registration_Succeeded";
      this->type = SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED;
   }

   M_Synchronization_Point_Registration_Succeeded::~M_Synchronization_Point_Registration_Succeeded() {
   }

   M_Announce_Synchronization_Point::M_Announce_Synchronization_Point() {
      this->name = "M_Announce_Synchronization_Point";
      this->type = ANNOUNCE_SYNCHRONIZATION_POINT;
   }

   M_Announce_Synchronization_Point::~M_Announce_Synchronization_Point() {
   }

   M_Synchronization_Point_Achieved::M_Synchronization_Point_Achieved() {
      this->name = "M_Synchronization_Point_Achieved";
      this->type = SYNCHRONIZATION_POINT_ACHIEVED;
   }

   M_Synchronization_Point_Achieved::~M_Synchronization_Point_Achieved() {
   }

   M_Federation_Synchronized::M_Federation_Synchronized() {
      this->name = "M_Federation_Synchronized";
      this->type = FEDERATION_SYNCHRONIZED;
   }

   M_Federation_Synchronized::~M_Federation_Synchronized() {
   }

   M_Request_Federation_Save::M_Request_Federation_Save() {
      this->name = "M_Request_Federation_Save";
      this->type = REQUEST_FEDERATION_SAVE;
   }

   M_Request_Federation_Save::~M_Request_Federation_Save() {
   }

   M_Initiate_Federate_Save::M_Initiate_Federate_Save() {
      this->name = "M_Initiate_Federate_Save";
      this->type = INITIATE_FEDERATE_SAVE;
   }

   M_Initiate_Federate_Save::~M_Initiate_Federate_Save() {
   }

   M_Federate_Save_Begun::M_Federate_Save_Begun() {
      this->name = "M_Federate_Save_Begun";
      this->type = FEDERATE_SAVE_BEGUN;
   }

   M_Federate_Save_Begun::~M_Federate_Save_Begun() {
   }

   M_Federate_Save_Complete::M_Federate_Save_Complete() {
      this->name = "M_Federate_Save_Complete";
      this->type = FEDERATE_SAVE_COMPLETE;
   }

   M_Federate_Save_Complete::~M_Federate_Save_Complete() {
   }

   M_Federate_Save_Not_Complete::M_Federate_Save_Not_Complete() {
      this->name = "M_Federate_Save_Not_Complete";
      this->type = FEDERATE_SAVE_NOT_COMPLETE;
   }

   M_Federate_Save_Not_Complete::~M_Federate_Save_Not_Complete() {
   }

   M_Federation_Saved::M_Federation_Saved() {
      this->name = "M_Federation_Saved";
      this->type = FEDERATION_SAVED;
   }

   M_Federation_Saved::~M_Federation_Saved() {
   }

   M_Federation_Not_Saved::M_Federation_Not_Saved() {
      this->name = "M_Federation_Not_Saved";
      this->type = FEDERATION_NOT_SAVED;
   }

   M_Federation_Not_Saved::~M_Federation_Not_Saved() {
   }

   M_Request_Federation_Restore::M_Request_Federation_Restore() {
      this->name = "M_Request_Federation_Restore";
      this->type = REQUEST_FEDERATION_RESTORE;
   }

   M_Request_Federation_Restore::~M_Request_Federation_Restore() {
   }

   M_Request_Federation_Restore_Failed::M_Request_Federation_Restore_Failed() {
      this->name = "M_Request_Federation_Restore_Failed";
      this->type = REQUEST_FEDERATION_RESTORE_FAILED;
   }

   M_Request_Federation_Restore_Failed::~M_Request_Federation_Restore_Failed() {
   }

   M_Request_Federation_Restore_Succeeded::M_Request_Federation_Restore_Succeeded() {
      this->name = "M_Request_Federation_Restore_Succeeded";
      this->type = REQUEST_FEDERATION_RESTORE_SUCCEEDED;
   }

   M_Request_Federation_Restore_Succeeded::~M_Request_Federation_Restore_Succeeded() {
   }

   M_Initiate_Federate_Restore::M_Initiate_Federate_Restore() {
      this->name = "M_Initiate_Federate_Restore";
      this->type = INITIATE_FEDERATE_RESTORE;
   }

   M_Initiate_Federate_Restore::~M_Initiate_Federate_Restore() {
   }

   M_Federate_Restore_Complete::M_Federate_Restore_Complete() {
      this->name = "M_Federate_Restore_Complete";
      this->type = FEDERATE_RESTORE_COMPLETE;
   }

   M_Federate_Restore_Complete::~M_Federate_Restore_Complete() {
   }

   M_Federate_Restore_Not_Complete::M_Federate_Restore_Not_Complete() {
      this->name = "M_Federate_Restore_Not_Complete";
      this->type = FEDERATE_RESTORE_NOT_COMPLETE;
   }

   M_Federate_Restore_Not_Complete::~M_Federate_Restore_Not_Complete() {
   }

   M_Federation_Restored::M_Federation_Restored() {
      this->name = "M_Federation_Restored";
      this->type = FEDERATION_RESTORED;
   }

   M_Federation_Restored::~M_Federation_Restored() {
   }

   M_Federation_Not_Restored::M_Federation_Not_Restored() {
      this->name = "M_Federation_Not_Restored";
      this->type = FEDERATION_NOT_RESTORED;
   }

   M_Federation_Not_Restored::~M_Federation_Not_Restored() {
   }

   M_Federation_Restore_Begun::M_Federation_Restore_Begun() {
      this->name = "M_Federation_Restore_Begun";
      this->type = FEDERATION_RESTORE_BEGUN;
   }

   M_Federation_Restore_Begun::~M_Federation_Restore_Begun() {
   }

   M_Publish_Object_Class::M_Publish_Object_Class() {
      this->name = "M_Publish_Object_Class";
      this->type = PUBLISH_OBJECT_CLASS;
   }

   M_Publish_Object_Class::~M_Publish_Object_Class() {
   }

   M_Unpublish_Object_Class::M_Unpublish_Object_Class() {
      this->name = "M_Unpublish_Object_Class";
      this->type = UNPUBLISH_OBJECT_CLASS;
   }

   M_Unpublish_Object_Class::~M_Unpublish_Object_Class() {
   }

   M_Publish_Interaction_Class::M_Publish_Interaction_Class() {
      this->name = "M_Publish_Interaction_Class";
      this->type = PUBLISH_INTERACTION_CLASS;
   }

   M_Publish_Interaction_Class::~M_Publish_Interaction_Class() {
   }

   M_Unpublish_Interaction_Class::M_Unpublish_Interaction_Class() {
      this->name = "M_Unpublish_Interaction_Class";
      this->type = UNPUBLISH_INTERACTION_CLASS;
   }

   M_Unpublish_Interaction_Class::~M_Unpublish_Interaction_Class() {
   }

   M_Subscribe_Object_Class_Attributes::M_Subscribe_Object_Class_Attributes() {
      this->name = "M_Subscribe_Object_Class_Attributes";
      this->type = SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES;
   }

   M_Subscribe_Object_Class_Attributes::~M_Subscribe_Object_Class_Attributes() {
   }

   M_Unsubscribe_Object_Class::M_Unsubscribe_Object_Class() {
      this->name = "M_Unsubscribe_Object_Class";
      this->type = UNSUBSCRIBE_OBJECT_CLASS;
   }

   M_Unsubscribe_Object_Class::~M_Unsubscribe_Object_Class() {
   }

   M_Subscribe_Interaction_Class::M_Subscribe_Interaction_Class() {
      this->name = "M_Subscribe_Interaction_Class";
      this->type = SUBSCRIBE_INTERACTION_CLASS;
   }

   M_Subscribe_Interaction_Class::~M_Subscribe_Interaction_Class() {
   }

   M_Unsubscribe_Interaction_Class::M_Unsubscribe_Interaction_Class() {
      this->name = "M_Unsubscribe_Interaction_Class";
      this->type = UNSUBSCRIBE_INTERACTION_CLASS;
   }

   M_Unsubscribe_Interaction_Class::~M_Unsubscribe_Interaction_Class() {
   }

   M_Start_Registration_For_Object_Class::M_Start_Registration_For_Object_Class() {
      this->name = "M_Start_Registration_For_Object_Class";
      this->type = START_REGISTRATION_FOR_OBJECT_CLASS;
   }

   M_Start_Registration_For_Object_Class::~M_Start_Registration_For_Object_Class() {
   }

   M_Stop_Registration_For_Object_Class::M_Stop_Registration_For_Object_Class() {
      this->name = "M_Stop_Registration_For_Object_Class";
      this->type = STOP_REGISTRATION_FOR_OBJECT_CLASS;
   }

   M_Stop_Registration_For_Object_Class::~M_Stop_Registration_For_Object_Class() {
   }

   M_Turn_Interactions_On::M_Turn_Interactions_On() {
      this->name = "M_Turn_Interactions_On";
      this->type = TURN_INTERACTIONS_ON;
   }

   M_Turn_Interactions_On::~M_Turn_Interactions_On() {
   }

   M_Turn_Interactions_Off::M_Turn_Interactions_Off() {
      this->name = "M_Turn_Interactions_Off";
      this->type = TURN_INTERACTIONS_OFF;
   }

   M_Turn_Interactions_Off::~M_Turn_Interactions_Off() {
   }

   M_Register_Object_Instance::M_Register_Object_Instance() {
      this->name = "M_Register_Object_Instance";
      this->type = REGISTER_OBJECT_INSTANCE;
   }

   M_Register_Object_Instance::~M_Register_Object_Instance() {
   }

   M_Update_Attribute_Values::M_Update_Attribute_Values() {
      this->name = "M_Update_Attribute_Values";
      this->type = UPDATE_ATTRIBUTE_VALUES;
   }

   M_Update_Attribute_Values::~M_Update_Attribute_Values() {
   }

   M_Discover_Object_Instance::M_Discover_Object_Instance() {
      this->name = "M_Discover_Object_Instance";
      this->type = DISCOVER_OBJECT_INSTANCE;
   }

   M_Discover_Object_Instance::~M_Discover_Object_Instance() {
   }

   M_Reflect_Attribute_Values::M_Reflect_Attribute_Values() {
      this->name = "M_Reflect_Attribute_Values";
      this->type = REFLECT_ATTRIBUTE_VALUES;
   }

   M_Reflect_Attribute_Values::~M_Reflect_Attribute_Values() {
   }

   M_Send_Interaction::M_Send_Interaction() {
      this->name = "M_Send_Interaction";
      this->type = SEND_INTERACTION;
   }

   M_Send_Interaction::~M_Send_Interaction() {
   }

   M_Receive_Interaction::M_Receive_Interaction() {
      this->name = "M_Receive_Interaction";
      this->type = RECEIVE_INTERACTION;
   }

   M_Receive_Interaction::~M_Receive_Interaction() {
   }

   M_Delete_Object_Instance::M_Delete_Object_Instance() {
      this->name = "M_Delete_Object_Instance";
      this->type = DELETE_OBJECT_INSTANCE;
   }

   M_Delete_Object_Instance::~M_Delete_Object_Instance() {
   }

   M_Local_Delete_Object_Instance::M_Local_Delete_Object_Instance() {
      this->name = "M_Local_Delete_Object_Instance";
      this->type = LOCAL_DELETE_OBJECT_INSTANCE;
   }

   M_Local_Delete_Object_Instance::~M_Local_Delete_Object_Instance() {
   }

   M_Remove_Object_Instance::M_Remove_Object_Instance() {
      this->name = "M_Remove_Object_Instance";
      this->type = REMOVE_OBJECT_INSTANCE;
   }

   M_Remove_Object_Instance::~M_Remove_Object_Instance() {
   }

   M_Change_Attribute_Transportation_Type::M_Change_Attribute_Transportation_Type() {
      this->name = "M_Change_Attribute_Transportation_Type";
      this->type = CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE;
   }

   M_Change_Attribute_Transportation_Type::~M_Change_Attribute_Transportation_Type() {
   }

   M_Change_Interaction_Transportation_Type::M_Change_Interaction_Transportation_Type() {
      this->name = "M_Change_Interaction_Transportation_Type";
      this->type = CHANGE_INTERACTION_TRANSPORTATION_TYPE;
   }

   M_Change_Interaction_Transportation_Type::~M_Change_Interaction_Transportation_Type() {
   }

   M_Request_Object_Attribute_Value_Update::M_Request_Object_Attribute_Value_Update() {
      this->name = "M_Request_Object_Attribute_Value_Update";
      this->type = REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE;
   }

   M_Request_Object_Attribute_Value_Update::~M_Request_Object_Attribute_Value_Update() {
   }

   M_Request_Class_Attribute_Value_Update::M_Request_Class_Attribute_Value_Update() {
      this->name = "M_Request_Class_Attribute_Value_Update";
      this->type = REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE;
   }

   M_Request_Class_Attribute_Value_Update::~M_Request_Class_Attribute_Value_Update() {
   }

   M_Provide_Attribute_Value_Update::M_Provide_Attribute_Value_Update() {
      this->name = "M_Provide_Attribute_Value_Update";
      this->type = PROVIDE_ATTRIBUTE_VALUE_UPDATE;
   }

   M_Provide_Attribute_Value_Update::~M_Provide_Attribute_Value_Update() {
   }

   M_Attributes_In_Scope::M_Attributes_In_Scope() {
      this->name = "M_Attributes_In_Scope";
      this->type = ATTRIBUTES_IN_SCOPE;
   }

   M_Attributes_In_Scope::~M_Attributes_In_Scope() {
   }

   M_Attributes_Out_Of_Scope::M_Attributes_Out_Of_Scope() {
      this->name = "M_Attributes_Out_Of_Scope";
      this->type = ATTRIBUTES_OUT_OF_SCOPE;
   }

   M_Attributes_Out_Of_Scope::~M_Attributes_Out_Of_Scope() {
   }

   M_Turn_Updates_On_For_Object_Instance::M_Turn_Updates_On_For_Object_Instance() {
      this->name = "M_Turn_Updates_On_For_Object_Instance";
      this->type = TURN_UPDATES_ON_FOR_OBJECT_INSTANCE;
   }

   M_Turn_Updates_On_For_Object_Instance::~M_Turn_Updates_On_For_Object_Instance() {
   }

   M_Turn_Updates_Off_For_Object_Instance::M_Turn_Updates_Off_For_Object_Instance() {
      this->name = "M_Turn_Updates_Off_For_Object_Instance";
      this->type = TURN_UPDATES_OFF_FOR_OBJECT_INSTANCE;
   }

   M_Turn_Updates_Off_For_Object_Instance::~M_Turn_Updates_Off_For_Object_Instance() {
   }

   M_Request_Attribute_Ownership_Divestiture::M_Request_Attribute_Ownership_Divestiture() {
      this->name = "M_Request_Attribute_Ownership_Divestiture";
      this->type = REQUEST_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
   }

   M_Request_Attribute_Ownership_Divestiture::~M_Request_Attribute_Ownership_Divestiture() {
   }

   M_Request_Attribute_Ownership_Assumption::M_Request_Attribute_Ownership_Assumption() {
      this->name = "M_Request_Attribute_Ownership_Assumption";
      this->type = REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION;
   }

   M_Request_Attribute_Ownership_Assumption::~M_Request_Attribute_Ownership_Assumption() {
   }

   M_Negotiated_Attribute_Ownership_Divestiture::M_Negotiated_Attribute_Ownership_Divestiture() {
      this->name = "M_Negotiated_Attribute_Ownership_Divestiture";
      this->type = NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
   }

   M_Negotiated_Attribute_Ownership_Divestiture::~M_Negotiated_Attribute_Ownership_Divestiture() {
   }

   M_Attribute_Ownership_Divestiture_Notification::M_Attribute_Ownership_Divestiture_Notification() {
      this->name = "M_Attribute_Ownership_Divestiture_Notification";
      this->type = ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION;
   }

   M_Attribute_Ownership_Divestiture_Notification::~M_Attribute_Ownership_Divestiture_Notification() {
   }

   M_Attribute_Ownership_Acquisition_Notification::M_Attribute_Ownership_Acquisition_Notification() {
      this->name = "M_Attribute_Ownership_Acquisition_Notification";
      this->type = ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION;
   }

   M_Attribute_Ownership_Acquisition_Notification::~M_Attribute_Ownership_Acquisition_Notification() {
   }

   M_Request_Attribute_Ownership_Acquisition::M_Request_Attribute_Ownership_Acquisition() {
      this->name = "M_Request_Attribute_Ownership_Acquisition";
      this->type = REQUEST_ATTRIBUTE_OWNERSHIP_ACQUISITION;
   }

   M_Request_Attribute_Ownership_Acquisition::~M_Request_Attribute_Ownership_Acquisition() {
   }

   M_Request_Attribute_Ownership_Release::M_Request_Attribute_Ownership_Release() {
      this->name = "M_Request_Attribute_Ownership_Release";
      this->type = REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE;
   }

   M_Request_Attribute_Ownership_Release::~M_Request_Attribute_Ownership_Release() {
   }

   M_Query_Attribute_Ownership::M_Query_Attribute_Ownership() {
      this->name = "M_Query_Attribute_Ownership";
      this->type = QUERY_ATTRIBUTE_OWNERSHIP;
   }

   M_Query_Attribute_Ownership::~M_Query_Attribute_Ownership() {
   }

   M_Inform_Attribute_Ownership::M_Inform_Attribute_Ownership() {
      this->name = "M_Inform_Attribute_Ownership";
      this->type = INFORM_ATTRIBUTE_OWNERSHIP;
   }

   M_Inform_Attribute_Ownership::~M_Inform_Attribute_Ownership() {
   }

   M_Is_Attribute_Owned_By_Federate::M_Is_Attribute_Owned_By_Federate() {
      this->name = "M_Is_Attribute_Owned_By_Federate";
      this->type = IS_ATTRIBUTE_OWNED_BY_FEDERATE;
   }

   M_Is_Attribute_Owned_By_Federate::~M_Is_Attribute_Owned_By_Federate() {
   }

   M_Attribute_Is_Not_Owned::M_Attribute_Is_Not_Owned() {
      this->name = "M_Attribute_Is_Not_Owned";
      this->type = ATTRIBUTE_IS_NOT_OWNED;
   }

   M_Attribute_Is_Not_Owned::~M_Attribute_Is_Not_Owned() {
   }

   M_Attribute_Owned_By_Rti::M_Attribute_Owned_By_Rti() {
      this->name = "M_Attribute_Owned_By_Rti";
      this->type = ATTRIBUTE_OWNED_BY_RTI;
   }

   M_Attribute_Owned_By_Rti::~M_Attribute_Owned_By_Rti() {
   }

   M_Attribute_Ownership_Acquisition_If_Available::M_Attribute_Ownership_Acquisition_If_Available() {
      this->name = "M_Attribute_Ownership_Acquisition_If_Available";
      this->type = ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE;
   }

   M_Attribute_Ownership_Acquisition_If_Available::~M_Attribute_Ownership_Acquisition_If_Available() {
   }

   M_Attribute_Ownership_Unavailable::M_Attribute_Ownership_Unavailable() {
      this->name = "M_Attribute_Ownership_Unavailable";
      this->type = ATTRIBUTE_OWNERSHIP_UNAVAILABLE;
   }

   M_Attribute_Ownership_Unavailable::~M_Attribute_Ownership_Unavailable() {
   }

   M_Unconditional_Attribute_Ownership_Divestiture::M_Unconditional_Attribute_Ownership_Divestiture() {
      this->name = "M_Unconditional_Attribute_Ownership_Divestiture";
      this->type = UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
   }

   M_Unconditional_Attribute_Ownership_Divestiture::~M_Unconditional_Attribute_Ownership_Divestiture() {
   }

   M_Attribute_Ownership_Acquisition::M_Attribute_Ownership_Acquisition() {
      this->name = "M_Attribute_Ownership_Acquisition";
      this->type = ATTRIBUTE_OWNERSHIP_ACQUISITION;
   }

   M_Attribute_Ownership_Acquisition::~M_Attribute_Ownership_Acquisition() {
   }

   M_Cancel_Negotiated_Attribute_Ownership_Divestiture::M_Cancel_Negotiated_Attribute_Ownership_Divestiture() {
      this->name = "M_Cancel_Negotiated_Attribute_Ownership_Divestiture";
      this->type = CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
   }

   M_Cancel_Negotiated_Attribute_Ownership_Divestiture::~M_Cancel_Negotiated_Attribute_Ownership_Divestiture() {
   }

   M_Attribute_Ownership_Release_Response::M_Attribute_Ownership_Release_Response() {
      this->name = "M_Attribute_Ownership_Release_Response";
      this->type = ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE;
   }

   M_Attribute_Ownership_Release_Response::~M_Attribute_Ownership_Release_Response() {
   }

   M_Cancel_Attribute_Ownership_Acquisition::M_Cancel_Attribute_Ownership_Acquisition() {
      this->name = "M_Cancel_Attribute_Ownership_Acquisition";
      this->type = CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION;
   }

   M_Cancel_Attribute_Ownership_Acquisition::~M_Cancel_Attribute_Ownership_Acquisition() {
   }

   M_Confirm_Attribute_Ownership_Acquisition_Cancellation::M_Confirm_Attribute_Ownership_Acquisition_Cancellation() {
      this->name = "M_Confirm_Attribute_Ownership_Acquisition_Cancellation";
      this->type = CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION;
   }

   M_Confirm_Attribute_Ownership_Acquisition_Cancellation::~M_Confirm_Attribute_Ownership_Acquisition_Cancellation() {
   }

   M_Change_Attribute_Order_Type::M_Change_Attribute_Order_Type() {
      this->name = "M_Change_Attribute_Order_Type";
      this->type = CHANGE_ATTRIBUTE_ORDER_TYPE;
   }

   M_Change_Attribute_Order_Type::~M_Change_Attribute_Order_Type() {
   }

   M_Change_Interaction_Order_Type::M_Change_Interaction_Order_Type() {
      this->name = "M_Change_Interaction_Order_Type";
      this->type = CHANGE_INTERACTION_ORDER_TYPE;
   }

   M_Change_Interaction_Order_Type::~M_Change_Interaction_Order_Type() {
   }

   M_Enable_Time_Regulation::M_Enable_Time_Regulation() {
      this->name = "M_Enable_Time_Regulation";
      this->type = ENABLE_TIME_REGULATION;
      //fedTime= <no default value in message spec using builtin>
      //lookahead= <no default value in message spec using builtin>
   }

   M_Enable_Time_Regulation::~M_Enable_Time_Regulation() {
   }

   void M_Enable_Time_Regulation::serialize(MessageBuffer& msgBuffer) {
      //Call mother class
      Super::serialize(msgBuffer);
      //Specific serialization code
      msgBuffer.write_double(fedTime);
      msgBuffer.write_double(lookahead);
   }

   void M_Enable_Time_Regulation::deserialize(MessageBuffer& msgBuffer) {
      //Call mother class
      Super::deserialize(msgBuffer);
      //Specific deserialization code
      fedTime = msgBuffer.read_double();
      lookahead = msgBuffer.read_double();
   }

   M_Disable_Time_Regulation::M_Disable_Time_Regulation() {
      this->name = "M_Disable_Time_Regulation";
      this->type = DISABLE_TIME_REGULATION;
   }

   M_Disable_Time_Regulation::~M_Disable_Time_Regulation() {
   }

   M_Enable_Time_Constrained::M_Enable_Time_Constrained() {
      this->name = "M_Enable_Time_Constrained";
      this->type = ENABLE_TIME_CONSTRAINED;
   }

   M_Enable_Time_Constrained::~M_Enable_Time_Constrained() {
   }

   M_Disable_Time_Constrained::M_Disable_Time_Constrained() {
      this->name = "M_Disable_Time_Constrained";
      this->type = DISABLE_TIME_CONSTRAINED;
   }

   M_Disable_Time_Constrained::~M_Disable_Time_Constrained() {
   }

   M_Query_Lbts::M_Query_Lbts() {
      this->name = "M_Query_Lbts";
      this->type = QUERY_LBTS;
   }

   M_Query_Lbts::~M_Query_Lbts() {
   }

   M_Query_Federate_Time::M_Query_Federate_Time() {
      this->name = "M_Query_Federate_Time";
      this->type = QUERY_FEDERATE_TIME;
   }

   M_Query_Federate_Time::~M_Query_Federate_Time() {
   }

   M_Query_Min_Next_Event_Time::M_Query_Min_Next_Event_Time() {
      this->name = "M_Query_Min_Next_Event_Time";
      this->type = QUERY_MIN_NEXT_EVENT_TIME;
   }

   M_Query_Min_Next_Event_Time::~M_Query_Min_Next_Event_Time() {
   }

   M_Modify_Lookahead::M_Modify_Lookahead() {
      this->name = "M_Modify_Lookahead";
      this->type = MODIFY_LOOKAHEAD;
   }

   M_Modify_Lookahead::~M_Modify_Lookahead() {
   }

   M_Query_Lookahead::M_Query_Lookahead() {
      this->name = "M_Query_Lookahead";
      this->type = QUERY_LOOKAHEAD;
   }

   M_Query_Lookahead::~M_Query_Lookahead() {
   }

   M_Retract::M_Retract() {
      this->name = "M_Retract";
      this->type = RETRACT;
   }

   M_Retract::~M_Retract() {
   }

   M_Request_Retraction::M_Request_Retraction() {
      this->name = "M_Request_Retraction";
      this->type = REQUEST_RETRACTION;
   }

   M_Request_Retraction::~M_Request_Retraction() {
   }

   M_Time_Advance_Request::M_Time_Advance_Request() {
      this->name = "M_Time_Advance_Request";
      this->type = TIME_ADVANCE_REQUEST;
   }

   M_Time_Advance_Request::~M_Time_Advance_Request() {
   }

   M_Time_Advance_Request_Available::M_Time_Advance_Request_Available() {
      this->name = "M_Time_Advance_Request_Available";
      this->type = TIME_ADVANCE_REQUEST_AVAILABLE;
   }

   M_Time_Advance_Request_Available::~M_Time_Advance_Request_Available() {
   }

   M_Next_Event_Request::M_Next_Event_Request() {
      this->name = "M_Next_Event_Request";
      this->type = NEXT_EVENT_REQUEST;
   }

   M_Next_Event_Request::~M_Next_Event_Request() {
   }

   M_Next_Event_Request_Available::M_Next_Event_Request_Available() {
      this->name = "M_Next_Event_Request_Available";
      this->type = NEXT_EVENT_REQUEST_AVAILABLE;
   }

   M_Next_Event_Request_Available::~M_Next_Event_Request_Available() {
   }

   M_Flush_Queue_Request::M_Flush_Queue_Request() {
      this->name = "M_Flush_Queue_Request";
      this->type = FLUSH_QUEUE_REQUEST;
   }

   M_Flush_Queue_Request::~M_Flush_Queue_Request() {
   }

   M_Time_Advance_Grant::M_Time_Advance_Grant() {
      this->name = "M_Time_Advance_Grant";
      this->type = TIME_ADVANCE_GRANT;
   }

   M_Time_Advance_Grant::~M_Time_Advance_Grant() {
   }

   M_Enable_Asynchronous_Delivery::M_Enable_Asynchronous_Delivery() {
      this->name = "M_Enable_Asynchronous_Delivery";
      this->type = ENABLE_ASYNCHRONOUS_DELIVERY;
   }

   M_Enable_Asynchronous_Delivery::~M_Enable_Asynchronous_Delivery() {
   }

   M_Disable_Asynchronous_Delivery::M_Disable_Asynchronous_Delivery() {
      this->name = "M_Disable_Asynchronous_Delivery";
      this->type = DISABLE_ASYNCHRONOUS_DELIVERY;
   }

   M_Disable_Asynchronous_Delivery::~M_Disable_Asynchronous_Delivery() {
   }

   M_Time_Regulation_Enabled::M_Time_Regulation_Enabled() {
      this->name = "M_Time_Regulation_Enabled";
      this->type = TIME_REGULATION_ENABLED;
   }

   M_Time_Regulation_Enabled::~M_Time_Regulation_Enabled() {
   }

   M_Time_Constrained_Enabled::M_Time_Constrained_Enabled() {
      this->name = "M_Time_Constrained_Enabled";
      this->type = TIME_CONSTRAINED_ENABLED;
   }

   M_Time_Constrained_Enabled::~M_Time_Constrained_Enabled() {
   }

   M_Ddm_Create_Region::M_Ddm_Create_Region() {
      this->name = "M_Ddm_Create_Region";
      this->type = DDM_CREATE_REGION;
   }

   M_Ddm_Create_Region::~M_Ddm_Create_Region() {
   }

   M_Ddm_Modify_Region::M_Ddm_Modify_Region() {
      this->name = "M_Ddm_Modify_Region";
      this->type = DDM_MODIFY_REGION;
   }

   M_Ddm_Modify_Region::~M_Ddm_Modify_Region() {
   }

   M_Ddm_Delete_Region::M_Ddm_Delete_Region() {
      this->name = "M_Ddm_Delete_Region";
      this->type = DDM_DELETE_REGION;
   }

   M_Ddm_Delete_Region::~M_Ddm_Delete_Region() {
   }

   M_Ddm_Register_Object::M_Ddm_Register_Object() {
      this->name = "M_Ddm_Register_Object";
      this->type = DDM_REGISTER_OBJECT;
   }

   M_Ddm_Register_Object::~M_Ddm_Register_Object() {
   }

   M_Ddm_Associate_Region::M_Ddm_Associate_Region() {
      this->name = "M_Ddm_Associate_Region";
      this->type = DDM_ASSOCIATE_REGION;
   }

   M_Ddm_Associate_Region::~M_Ddm_Associate_Region() {
   }

   M_Ddm_Unassociate_Region::M_Ddm_Unassociate_Region() {
      this->name = "M_Ddm_Unassociate_Region";
      this->type = DDM_UNASSOCIATE_REGION;
   }

   M_Ddm_Unassociate_Region::~M_Ddm_Unassociate_Region() {
   }

   M_Ddm_Subscribe_Attributes::M_Ddm_Subscribe_Attributes() {
      this->name = "M_Ddm_Subscribe_Attributes";
      this->type = DDM_SUBSCRIBE_ATTRIBUTES;
   }

   M_Ddm_Subscribe_Attributes::~M_Ddm_Subscribe_Attributes() {
   }

   M_Ddm_Unsubscribe_Attributes::M_Ddm_Unsubscribe_Attributes() {
      this->name = "M_Ddm_Unsubscribe_Attributes";
      this->type = DDM_UNSUBSCRIBE_ATTRIBUTES;
   }

   M_Ddm_Unsubscribe_Attributes::~M_Ddm_Unsubscribe_Attributes() {
   }

   M_Ddm_Subscribe_Interaction::M_Ddm_Subscribe_Interaction() {
      this->name = "M_Ddm_Subscribe_Interaction";
      this->type = DDM_SUBSCRIBE_INTERACTION;
   }

   M_Ddm_Subscribe_Interaction::~M_Ddm_Subscribe_Interaction() {
   }

   M_Ddm_Unsubscribe_Interaction::M_Ddm_Unsubscribe_Interaction() {
      this->name = "M_Ddm_Unsubscribe_Interaction";
      this->type = DDM_UNSUBSCRIBE_INTERACTION;
   }

   M_Ddm_Unsubscribe_Interaction::~M_Ddm_Unsubscribe_Interaction() {
   }

   M_Ddm_Request_Update::M_Ddm_Request_Update() {
      this->name = "M_Ddm_Request_Update";
      this->type = DDM_REQUEST_UPDATE;
   }

   M_Ddm_Request_Update::~M_Ddm_Request_Update() {
   }

   M_Get_Object_Class_Handle::M_Get_Object_Class_Handle() {
      this->name = "M_Get_Object_Class_Handle";
      this->type = GET_OBJECT_CLASS_HANDLE;
   }

   M_Get_Object_Class_Handle::~M_Get_Object_Class_Handle() {
   }

   M_Get_Object_Class_Name::M_Get_Object_Class_Name() {
      this->name = "M_Get_Object_Class_Name";
      this->type = GET_OBJECT_CLASS_NAME;
   }

   M_Get_Object_Class_Name::~M_Get_Object_Class_Name() {
   }

   M_Get_Attribute_Handle::M_Get_Attribute_Handle() {
      this->name = "M_Get_Attribute_Handle";
      this->type = GET_ATTRIBUTE_HANDLE;
   }

   M_Get_Attribute_Handle::~M_Get_Attribute_Handle() {
   }

   M_Get_Attribute_Name::M_Get_Attribute_Name() {
      this->name = "M_Get_Attribute_Name";
      this->type = GET_ATTRIBUTE_NAME;
   }

   M_Get_Attribute_Name::~M_Get_Attribute_Name() {
   }

   M_Get_Interaction_Class_Handle::M_Get_Interaction_Class_Handle() {
      this->name = "M_Get_Interaction_Class_Handle";
      this->type = GET_INTERACTION_CLASS_HANDLE;
   }

   M_Get_Interaction_Class_Handle::~M_Get_Interaction_Class_Handle() {
   }

   M_Get_Interaction_Class_Name::M_Get_Interaction_Class_Name() {
      this->name = "M_Get_Interaction_Class_Name";
      this->type = GET_INTERACTION_CLASS_NAME;
   }

   M_Get_Interaction_Class_Name::~M_Get_Interaction_Class_Name() {
   }

   M_Get_Parameter_Handle::M_Get_Parameter_Handle() {
      this->name = "M_Get_Parameter_Handle";
      this->type = GET_PARAMETER_HANDLE;
   }

   M_Get_Parameter_Handle::~M_Get_Parameter_Handle() {
   }

   M_Get_Parameter_Name::M_Get_Parameter_Name() {
      this->name = "M_Get_Parameter_Name";
      this->type = GET_PARAMETER_NAME;
   }

   M_Get_Parameter_Name::~M_Get_Parameter_Name() {
   }

   M_Get_Object_Instance_Handle::M_Get_Object_Instance_Handle() {
      this->name = "M_Get_Object_Instance_Handle";
      this->type = GET_OBJECT_INSTANCE_HANDLE;
   }

   M_Get_Object_Instance_Handle::~M_Get_Object_Instance_Handle() {
   }

   M_Get_Object_Instance_Name::M_Get_Object_Instance_Name() {
      this->name = "M_Get_Object_Instance_Name";
      this->type = GET_OBJECT_INSTANCE_NAME;
   }

   M_Get_Object_Instance_Name::~M_Get_Object_Instance_Name() {
   }

   M_Get_Space_Handle::M_Get_Space_Handle() {
      this->name = "M_Get_Space_Handle";
      this->type = GET_SPACE_HANDLE;
   }

   M_Get_Space_Handle::~M_Get_Space_Handle() {
   }

   M_Get_Space_Name::M_Get_Space_Name() {
      this->name = "M_Get_Space_Name";
      this->type = GET_SPACE_NAME;
   }

   M_Get_Space_Name::~M_Get_Space_Name() {
   }

   M_Get_Dimension_Handle::M_Get_Dimension_Handle() {
      this->name = "M_Get_Dimension_Handle";
      this->type = GET_DIMENSION_HANDLE;
   }

   M_Get_Dimension_Handle::~M_Get_Dimension_Handle() {
   }

   M_Get_Dimension_Name::M_Get_Dimension_Name() {
      this->name = "M_Get_Dimension_Name";
      this->type = GET_DIMENSION_NAME;
   }

   M_Get_Dimension_Name::~M_Get_Dimension_Name() {
   }

   M_Get_Attribute_Space_Handle::M_Get_Attribute_Space_Handle() {
      this->name = "M_Get_Attribute_Space_Handle";
      this->type = GET_ATTRIBUTE_SPACE_HANDLE;
   }

   M_Get_Attribute_Space_Handle::~M_Get_Attribute_Space_Handle() {
   }

   M_Get_Object_Class::M_Get_Object_Class() {
      this->name = "M_Get_Object_Class";
      this->type = GET_OBJECT_CLASS;
   }

   M_Get_Object_Class::~M_Get_Object_Class() {
   }

   M_Get_Interaction_Space_Handle::M_Get_Interaction_Space_Handle() {
      this->name = "M_Get_Interaction_Space_Handle";
      this->type = GET_INTERACTION_SPACE_HANDLE;
   }

   M_Get_Interaction_Space_Handle::~M_Get_Interaction_Space_Handle() {
   }

   M_Get_Transportation_Handle::M_Get_Transportation_Handle() {
      this->name = "M_Get_Transportation_Handle";
      this->type = GET_TRANSPORTATION_HANDLE;
   }

   M_Get_Transportation_Handle::~M_Get_Transportation_Handle() {
   }

   M_Get_Transportation_Name::M_Get_Transportation_Name() {
      this->name = "M_Get_Transportation_Name";
      this->type = GET_TRANSPORTATION_NAME;
   }

   M_Get_Transportation_Name::~M_Get_Transportation_Name() {
   }

   M_Get_Ordering_Handle::M_Get_Ordering_Handle() {
      this->name = "M_Get_Ordering_Handle";
      this->type = GET_ORDERING_HANDLE;
   }

   M_Get_Ordering_Handle::~M_Get_Ordering_Handle() {
   }

   M_Get_Ordering_Name::M_Get_Ordering_Name() {
      this->name = "M_Get_Ordering_Name";
      this->type = GET_ORDERING_NAME;
   }

   M_Get_Ordering_Name::~M_Get_Ordering_Name() {
   }

   M_Enable_Class_Relevance_Advisory_Switch::M_Enable_Class_Relevance_Advisory_Switch() {
      this->name = "M_Enable_Class_Relevance_Advisory_Switch";
      this->type = ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH;
   }

   M_Enable_Class_Relevance_Advisory_Switch::~M_Enable_Class_Relevance_Advisory_Switch() {
   }

   M_Disable_Class_Relevance_Advisory_Switch::M_Disable_Class_Relevance_Advisory_Switch() {
      this->name = "M_Disable_Class_Relevance_Advisory_Switch";
      this->type = DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH;
   }

   M_Disable_Class_Relevance_Advisory_Switch::~M_Disable_Class_Relevance_Advisory_Switch() {
   }

   M_Enable_Attribute_Relevance_Advisory_Switch::M_Enable_Attribute_Relevance_Advisory_Switch() {
      this->name = "M_Enable_Attribute_Relevance_Advisory_Switch";
      this->type = ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH;
   }

   M_Enable_Attribute_Relevance_Advisory_Switch::~M_Enable_Attribute_Relevance_Advisory_Switch() {
   }

   M_Disable_Attribute_Relevance_Advisory_Switch::M_Disable_Attribute_Relevance_Advisory_Switch() {
      this->name = "M_Disable_Attribute_Relevance_Advisory_Switch";
      this->type = DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH;
   }

   M_Disable_Attribute_Relevance_Advisory_Switch::~M_Disable_Attribute_Relevance_Advisory_Switch() {
   }

   M_Enable_Attribute_Scope_Advisory_Switch::M_Enable_Attribute_Scope_Advisory_Switch() {
      this->name = "M_Enable_Attribute_Scope_Advisory_Switch";
      this->type = ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH;
   }

   M_Enable_Attribute_Scope_Advisory_Switch::~M_Enable_Attribute_Scope_Advisory_Switch() {
   }

   M_Disable_Attribute_Scope_Advisory_Switch::M_Disable_Attribute_Scope_Advisory_Switch() {
      this->name = "M_Disable_Attribute_Scope_Advisory_Switch";
      this->type = DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH;
   }

   M_Disable_Attribute_Scope_Advisory_Switch::~M_Disable_Attribute_Scope_Advisory_Switch() {
   }

   M_Enable_Interaction_Relevance_Advisory_Switch::M_Enable_Interaction_Relevance_Advisory_Switch() {
      this->name = "M_Enable_Interaction_Relevance_Advisory_Switch";
      this->type = ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH;
   }

   M_Enable_Interaction_Relevance_Advisory_Switch::~M_Enable_Interaction_Relevance_Advisory_Switch() {
   }

   M_Disable_Interaction_Relevance_Advisory_Switch::M_Disable_Interaction_Relevance_Advisory_Switch() {
      this->name = "M_Disable_Interaction_Relevance_Advisory_Switch";
      this->type = DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH;
   }

   M_Disable_Interaction_Relevance_Advisory_Switch::~M_Disable_Interaction_Relevance_Advisory_Switch() {
   }

   M_Tick_Request::M_Tick_Request() {
      this->name = "M_Tick_Request";
      this->type = TICK_REQUEST;
   }

   M_Tick_Request::~M_Tick_Request() {
   }

   M_Tick_Request_Next::M_Tick_Request_Next() {
      this->name = "M_Tick_Request_Next";
      this->type = TICK_REQUEST_NEXT;
   }

   M_Tick_Request_Next::~M_Tick_Request_Next() {
   }

   M_Tick_Request_Stop::M_Tick_Request_Stop() {
      this->name = "M_Tick_Request_Stop";
      this->type = TICK_REQUEST_STOP;
   }

   M_Tick_Request_Stop::~M_Tick_Request_Stop() {
   }

   Message* M_Factory::create(M_Type type) throw (RTIinternalError) {
      Message* msg;

      switch (type) {
         case Message::NOT_USED:
            throw RTIinternalError("NOT_USED message type should not be used!!");
            break;
         case Message::CREATE_FEDERATION_EXECUTION:
            msg = new M_Create_Federation_Execution();
            break;
         case Message::DESTROY_FEDERATION_EXECUTION:
            msg = new M_Destroy_Federation_Execution();
            break;
         case Message::JOIN_FEDERATION_EXECUTION:
            msg = new M_Join_Federation_Execution();
            break;
         case Message::RESIGN_FEDERATION_EXECUTION:
            msg = new M_Resign_Federation_Execution();
            break;
         case Message::REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
            msg = new M_Register_Federation_Synchronization_Point();
            break;
         case Message::SYNCHRONIZATION_POINT_REGISTRATION_FAILED:
            msg = new M_Synchronization_Point_Registration_Failed();
            break;
         case Message::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
            msg = new M_Synchronization_Point_Registration_Succeeded();
            break;
         case Message::ANNOUNCE_SYNCHRONIZATION_POINT:
            msg = new M_Announce_Synchronization_Point();
            break;
         case Message::SYNCHRONIZATION_POINT_ACHIEVED:
            msg = new M_Synchronization_Point_Achieved();
            break;
         case Message::FEDERATION_SYNCHRONIZED:
            msg = new M_Federation_Synchronized();
            break;
         case Message::REQUEST_FEDERATION_SAVE:
            msg = new M_Request_Federation_Save();
            break;
         case Message::INITIATE_FEDERATE_SAVE:
            msg = new M_Initiate_Federate_Save();
            break;
         case Message::FEDERATE_SAVE_BEGUN:
            msg = new M_Federate_Save_Begun();
            break;
         case Message::FEDERATE_SAVE_COMPLETE:
            msg = new M_Federate_Save_Complete();
            break;
         case Message::FEDERATE_SAVE_NOT_COMPLETE:
            msg = new M_Federate_Save_Not_Complete();
            break;
         case Message::FEDERATION_SAVED:
            msg = new M_Federation_Saved();
            break;
         case Message::FEDERATION_NOT_SAVED:
            msg = new M_Federation_Not_Saved();
            break;
         case Message::REQUEST_FEDERATION_RESTORE:
            msg = new M_Request_Federation_Restore();
            break;
         case Message::REQUEST_FEDERATION_RESTORE_FAILED:
            msg = new M_Request_Federation_Restore_Failed();
            break;
         case Message::REQUEST_FEDERATION_RESTORE_SUCCEEDED:
            msg = new M_Request_Federation_Restore_Succeeded();
            break;
         case Message::INITIATE_FEDERATE_RESTORE:
            msg = new M_Initiate_Federate_Restore();
            break;
         case Message::FEDERATE_RESTORE_COMPLETE:
            msg = new M_Federate_Restore_Complete();
            break;
         case Message::FEDERATE_RESTORE_NOT_COMPLETE:
            msg = new M_Federate_Restore_Not_Complete();
            break;
         case Message::FEDERATION_RESTORED:
            msg = new M_Federation_Restored();
            break;
         case Message::FEDERATION_NOT_RESTORED:
            msg = new M_Federation_Not_Restored();
            break;
         case Message::FEDERATION_RESTORE_BEGUN:
            msg = new M_Federation_Restore_Begun();
            break;
         case Message::PUBLISH_OBJECT_CLASS:
            msg = new M_Publish_Object_Class();
            break;
         case Message::UNPUBLISH_OBJECT_CLASS:
            msg = new M_Unpublish_Object_Class();
            break;
         case Message::PUBLISH_INTERACTION_CLASS:
            msg = new M_Publish_Interaction_Class();
            break;
         case Message::UNPUBLISH_INTERACTION_CLASS:
            msg = new M_Unpublish_Interaction_Class();
            break;
         case Message::SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES:
            msg = new M_Subscribe_Object_Class_Attributes();
            break;
         case Message::UNSUBSCRIBE_OBJECT_CLASS:
            msg = new M_Unsubscribe_Object_Class();
            break;
         case Message::SUBSCRIBE_INTERACTION_CLASS:
            msg = new M_Subscribe_Interaction_Class();
            break;
         case Message::UNSUBSCRIBE_INTERACTION_CLASS:
            msg = new M_Unsubscribe_Interaction_Class();
            break;
         case Message::START_REGISTRATION_FOR_OBJECT_CLASS:
            msg = new M_Start_Registration_For_Object_Class();
            break;
         case Message::STOP_REGISTRATION_FOR_OBJECT_CLASS:
            msg = new M_Stop_Registration_For_Object_Class();
            break;
         case Message::TURN_INTERACTIONS_ON:
            msg = new M_Turn_Interactions_On();
            break;
         case Message::TURN_INTERACTIONS_OFF:
            msg = new M_Turn_Interactions_Off();
            break;
         case Message::REGISTER_OBJECT_INSTANCE:
            msg = new M_Register_Object_Instance();
            break;
         case Message::UPDATE_ATTRIBUTE_VALUES:
            msg = new M_Update_Attribute_Values();
            break;
         case Message::DISCOVER_OBJECT_INSTANCE:
            msg = new M_Discover_Object_Instance();
            break;
         case Message::REFLECT_ATTRIBUTE_VALUES:
            msg = new M_Reflect_Attribute_Values();
            break;
         case Message::SEND_INTERACTION:
            msg = new M_Send_Interaction();
            break;
         case Message::RECEIVE_INTERACTION:
            msg = new M_Receive_Interaction();
            break;
         case Message::DELETE_OBJECT_INSTANCE:
            msg = new M_Delete_Object_Instance();
            break;
         case Message::LOCAL_DELETE_OBJECT_INSTANCE:
            msg = new M_Local_Delete_Object_Instance();
            break;
         case Message::REMOVE_OBJECT_INSTANCE:
            msg = new M_Remove_Object_Instance();
            break;
         case Message::CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE:
            msg = new M_Change_Attribute_Transportation_Type();
            break;
         case Message::CHANGE_INTERACTION_TRANSPORTATION_TYPE:
            msg = new M_Change_Interaction_Transportation_Type();
            break;
         case Message::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
            msg = new M_Request_Object_Attribute_Value_Update();
            break;
         case Message::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE:
            msg = new M_Request_Class_Attribute_Value_Update();
            break;
         case Message::PROVIDE_ATTRIBUTE_VALUE_UPDATE:
            msg = new M_Provide_Attribute_Value_Update();
            break;
         case Message::ATTRIBUTES_IN_SCOPE:
            msg = new M_Attributes_In_Scope();
            break;
         case Message::ATTRIBUTES_OUT_OF_SCOPE:
            msg = new M_Attributes_Out_Of_Scope();
            break;
         case Message::TURN_UPDATES_ON_FOR_OBJECT_INSTANCE:
            msg = new M_Turn_Updates_On_For_Object_Instance();
            break;
         case Message::TURN_UPDATES_OFF_FOR_OBJECT_INSTANCE:
            msg = new M_Turn_Updates_Off_For_Object_Instance();
            break;
         case Message::REQUEST_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
            msg = new M_Request_Attribute_Ownership_Divestiture();
            break;
         case Message::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
            msg = new M_Request_Attribute_Ownership_Assumption();
            break;
         case Message::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
            msg = new M_Negotiated_Attribute_Ownership_Divestiture();
            break;
         case Message::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
            msg = new M_Attribute_Ownership_Divestiture_Notification();
            break;
         case Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
            msg = new M_Attribute_Ownership_Acquisition_Notification();
            break;
         case Message::REQUEST_ATTRIBUTE_OWNERSHIP_ACQUISITION:
            msg = new M_Request_Attribute_Ownership_Acquisition();
            break;
         case Message::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
            msg = new M_Request_Attribute_Ownership_Release();
            break;
         case Message::QUERY_ATTRIBUTE_OWNERSHIP:
            msg = new M_Query_Attribute_Ownership();
            break;
         case Message::INFORM_ATTRIBUTE_OWNERSHIP:
            msg = new M_Inform_Attribute_Ownership();
            break;
         case Message::IS_ATTRIBUTE_OWNED_BY_FEDERATE:
            msg = new M_Is_Attribute_Owned_By_Federate();
            break;
         case Message::ATTRIBUTE_IS_NOT_OWNED:
            msg = new M_Attribute_Is_Not_Owned();
            break;
         case Message::ATTRIBUTE_OWNED_BY_RTI:
            msg = new M_Attribute_Owned_By_Rti();
            break;
         case Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
            msg = new M_Attribute_Ownership_Acquisition_If_Available();
            break;
         case Message::ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
            msg = new M_Attribute_Ownership_Unavailable();
            break;
         case Message::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
            msg = new M_Unconditional_Attribute_Ownership_Divestiture();
            break;
         case Message::ATTRIBUTE_OWNERSHIP_ACQUISITION:
            msg = new M_Attribute_Ownership_Acquisition();
            break;
         case Message::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
            msg = new M_Cancel_Negotiated_Attribute_Ownership_Divestiture();
            break;
         case Message::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
            msg = new M_Attribute_Ownership_Release_Response();
            break;
         case Message::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
            msg = new M_Cancel_Attribute_Ownership_Acquisition();
            break;
         case Message::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
            msg = new M_Confirm_Attribute_Ownership_Acquisition_Cancellation();
            break;
         case Message::CHANGE_ATTRIBUTE_ORDER_TYPE:
            msg = new M_Change_Attribute_Order_Type();
            break;
         case Message::CHANGE_INTERACTION_ORDER_TYPE:
            msg = new M_Change_Interaction_Order_Type();
            break;
         case Message::ENABLE_TIME_REGULATION:
            msg = new M_Enable_Time_Regulation();
            break;
         case Message::DISABLE_TIME_REGULATION:
            msg = new M_Disable_Time_Regulation();
            break;
         case Message::ENABLE_TIME_CONSTRAINED:
            msg = new M_Enable_Time_Constrained();
            break;
         case Message::DISABLE_TIME_CONSTRAINED:
            msg = new M_Disable_Time_Constrained();
            break;
         case Message::QUERY_LBTS:
            msg = new M_Query_Lbts();
            break;
         case Message::QUERY_FEDERATE_TIME:
            msg = new M_Query_Federate_Time();
            break;
         case Message::QUERY_MIN_NEXT_EVENT_TIME:
            msg = new M_Query_Min_Next_Event_Time();
            break;
         case Message::MODIFY_LOOKAHEAD:
            msg = new M_Modify_Lookahead();
            break;
         case Message::QUERY_LOOKAHEAD:
            msg = new M_Query_Lookahead();
            break;
         case Message::RETRACT:
            msg = new M_Retract();
            break;
         case Message::REQUEST_RETRACTION:
            msg = new M_Request_Retraction();
            break;
         case Message::TIME_ADVANCE_REQUEST:
            msg = new M_Time_Advance_Request();
            break;
         case Message::TIME_ADVANCE_REQUEST_AVAILABLE:
            msg = new M_Time_Advance_Request_Available();
            break;
         case Message::NEXT_EVENT_REQUEST:
            msg = new M_Next_Event_Request();
            break;
         case Message::NEXT_EVENT_REQUEST_AVAILABLE:
            msg = new M_Next_Event_Request_Available();
            break;
         case Message::FLUSH_QUEUE_REQUEST:
            msg = new M_Flush_Queue_Request();
            break;
         case Message::TIME_ADVANCE_GRANT:
            msg = new M_Time_Advance_Grant();
            break;
         case Message::ENABLE_ASYNCHRONOUS_DELIVERY:
            msg = new M_Enable_Asynchronous_Delivery();
            break;
         case Message::DISABLE_ASYNCHRONOUS_DELIVERY:
            msg = new M_Disable_Asynchronous_Delivery();
            break;
         case Message::TIME_REGULATION_ENABLED:
            msg = new M_Time_Regulation_Enabled();
            break;
         case Message::TIME_CONSTRAINED_ENABLED:
            msg = new M_Time_Constrained_Enabled();
            break;
         case Message::DDM_CREATE_REGION:
            msg = new M_Ddm_Create_Region();
            break;
         case Message::DDM_MODIFY_REGION:
            msg = new M_Ddm_Modify_Region();
            break;
         case Message::DDM_DELETE_REGION:
            msg = new M_Ddm_Delete_Region();
            break;
         case Message::DDM_REGISTER_OBJECT:
            msg = new M_Ddm_Register_Object();
            break;
         case Message::DDM_ASSOCIATE_REGION:
            msg = new M_Ddm_Associate_Region();
            break;
         case Message::DDM_UNASSOCIATE_REGION:
            msg = new M_Ddm_Unassociate_Region();
            break;
         case Message::DDM_SUBSCRIBE_ATTRIBUTES:
            msg = new M_Ddm_Subscribe_Attributes();
            break;
         case Message::DDM_UNSUBSCRIBE_ATTRIBUTES:
            msg = new M_Ddm_Unsubscribe_Attributes();
            break;
         case Message::DDM_SUBSCRIBE_INTERACTION:
            msg = new M_Ddm_Subscribe_Interaction();
            break;
         case Message::DDM_UNSUBSCRIBE_INTERACTION:
            msg = new M_Ddm_Unsubscribe_Interaction();
            break;
         case Message::DDM_REQUEST_UPDATE:
            msg = new M_Ddm_Request_Update();
            break;
         case Message::GET_OBJECT_CLASS_HANDLE:
            msg = new M_Get_Object_Class_Handle();
            break;
         case Message::GET_OBJECT_CLASS_NAME:
            msg = new M_Get_Object_Class_Name();
            break;
         case Message::GET_ATTRIBUTE_HANDLE:
            msg = new M_Get_Attribute_Handle();
            break;
         case Message::GET_ATTRIBUTE_NAME:
            msg = new M_Get_Attribute_Name();
            break;
         case Message::GET_INTERACTION_CLASS_HANDLE:
            msg = new M_Get_Interaction_Class_Handle();
            break;
         case Message::GET_INTERACTION_CLASS_NAME:
            msg = new M_Get_Interaction_Class_Name();
            break;
         case Message::GET_PARAMETER_HANDLE:
            msg = new M_Get_Parameter_Handle();
            break;
         case Message::GET_PARAMETER_NAME:
            msg = new M_Get_Parameter_Name();
            break;
         case Message::GET_OBJECT_INSTANCE_HANDLE:
            msg = new M_Get_Object_Instance_Handle();
            break;
         case Message::GET_OBJECT_INSTANCE_NAME:
            msg = new M_Get_Object_Instance_Name();
            break;
         case Message::GET_SPACE_HANDLE:
            msg = new M_Get_Space_Handle();
            break;
         case Message::GET_SPACE_NAME:
            msg = new M_Get_Space_Name();
            break;
         case Message::GET_DIMENSION_HANDLE:
            msg = new M_Get_Dimension_Handle();
            break;
         case Message::GET_DIMENSION_NAME:
            msg = new M_Get_Dimension_Name();
            break;
         case Message::GET_ATTRIBUTE_SPACE_HANDLE:
            msg = new M_Get_Attribute_Space_Handle();
            break;
         case Message::GET_OBJECT_CLASS:
            msg = new M_Get_Object_Class();
            break;
         case Message::GET_INTERACTION_SPACE_HANDLE:
            msg = new M_Get_Interaction_Space_Handle();
            break;
         case Message::GET_TRANSPORTATION_HANDLE:
            msg = new M_Get_Transportation_Handle();
            break;
         case Message::GET_TRANSPORTATION_NAME:
            msg = new M_Get_Transportation_Name();
            break;
         case Message::GET_ORDERING_HANDLE:
            msg = new M_Get_Ordering_Handle();
            break;
         case Message::GET_ORDERING_NAME:
            msg = new M_Get_Ordering_Name();
            break;
         case Message::ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH:
            msg = new M_Enable_Class_Relevance_Advisory_Switch();
            break;
         case Message::DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH:
            msg = new M_Disable_Class_Relevance_Advisory_Switch();
            break;
         case Message::ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH:
            msg = new M_Enable_Attribute_Relevance_Advisory_Switch();
            break;
         case Message::DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH:
            msg = new M_Disable_Attribute_Relevance_Advisory_Switch();
            break;
         case Message::ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH:
            msg = new M_Enable_Attribute_Scope_Advisory_Switch();
            break;
         case Message::DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH:
            msg = new M_Disable_Attribute_Scope_Advisory_Switch();
            break;
         case Message::ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
            msg = new M_Enable_Interaction_Relevance_Advisory_Switch();
            break;
         case Message::DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
            msg = new M_Disable_Interaction_Relevance_Advisory_Switch();
            break;
         case Message::TICK_REQUEST:
            msg = new M_Tick_Request();
            break;
         case Message::TICK_REQUEST_NEXT:
            msg = new M_Tick_Request_Next();
            break;
         case Message::TICK_REQUEST_STOP:
            msg = new M_Tick_Request_Stop();
            break;
         case Message::LAST:
            throw RTIinternalError("LAST message type should not be used!!");
            break;
      } // end if switch (type)
   }

   Message* M_Factory::receive(StreamType stream) throw (RTIinternalError) {
      // FIXME This is not thread safe
      static MessageBuffer msgBuffer;
      Message  msgGen;
      Message* msg;

      // receive generic message 
      msgGen.receive(stream,msgBuffer);
      // create specific message from type 
      msg = M_Factory::create(msgGen.getType());
      msgBuffer.assumeSizeFromReservedBytes();
      msg->deserialize(msgBuffer);
      return msg;
   }

} // end of namespace certi 
