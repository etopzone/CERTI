
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
package certi

// Message is the base class
// for message exchanged between RTIA and Federate (libRTI)
// AKA CERTI Message
native Message {
    language CXX    [#include "Message.hh"]
    language Python [import certi.Message]
    language Java   [import certi.Message]
}

message M_Create_Federation_Execution : merge Message {

}

message M_Destroy_Federation_Execution : merge Message {

}

message M_Join_Federation_Execution : merge Message {

}

message M_Resign_Federation_Execution : merge Message {

}

message M_Register_Federation_Synchronization_Point : merge Message {

}
message M_Synchronization_Point_Registration_Failed : merge Message {}
message M_Synchronization_Point_Registration_Succeeded : merge Message {}
message M_Announce_Synchronization_Point : merge Message {}
message M_Synchronization_Point_Achieved : merge Message {}
message M_Federation_Synchronized : merge Message {}
message M_Request_Federation_Save : merge Message {}
message M_Initiate_Federate_Save : merge Message {}
message M_Federate_Save_Begun : merge Message {}
message M_Federate_Save_Complete : merge Message {}
message M_Federate_Save_Not_Complete : merge Message {}
message M_Federation_Saved : merge Message {}
message M_Federation_Not_Saved : merge Message {}
message M_Request_Federation_Restore : merge Message {}
message M_Request_Federation_Restore_Failed : merge Message {}
message M_Request_Federation_Restore_Succeeded : merge Message {}
message M_Initiate_Federate_Restore : merge Message {}
message M_Federate_Restore_Complete : merge Message {}
message M_Federate_Restore_Not_Complete : merge Message {}
message M_Federation_Restored : merge Message {}
message M_Federation_Not_Restored : merge Message {}
message M_Federation_Restore_Begun : merge Message {}

 // Declaration
message M_Publish_Object_Class : merge Message {}
message M_Unpublish_Object_Class : merge Message {}
message M_Publish_Interaction_Class : merge Message {}
message M_Unpublish_Interaction_Class : merge Message {}
message M_Subscribe_Object_Class_Attributes : merge Message {}
message M_Unsubscribe_Object_Class : merge Message {}
message M_Subscribe_Interaction_Class : merge Message {}
message M_Unsubscribe_Interaction_Class : merge Message {}
message M_Start_Registration_For_Object_Class : merge Message {}
message M_Stop_Registration_For_Object_Class : merge Message {}
message M_Turn_Interactions_On : merge Message {}
message M_Turn_Interactions_Off : merge Message {}

        // Object
message M_Register_Object_Instance : merge Message {}
message M_Update_Attribute_Values : merge Message {}
message M_Discover_Object_Instance : merge Message {}
message M_Reflect_Attribute_Values : merge Message {}
message M_Send_Interaction : merge Message {}
message M_Receive_Interaction : merge Message {}
message M_Delete_Object_Instance : merge Message {}
message M_Local_Delete_Object_Instance : merge Message {}
message M_Remove_Object_Instance : merge Message {}
message M_Change_Attribute_Transportation_Type : merge Message {}
message M_Change_Interaction_Transportation_Type : merge Message {}
message M_Request_Object_Attribute_Value_Update : merge Message {}
message M_Request_Class_Attribute_Value_Update : merge Message {}
message M_Provide_Attribute_Value_Update : merge Message {}
message M_Attributes_In_Scope : merge Message {}
message M_Attributes_Out_Of_Scope : merge Message {}
message M_Turn_Updates_On_For_Object_Instance : merge Message {}
message M_Turn_Updates_Off_For_Object_Instance : merge Message {}

        // Ownership
message M_Request_Attribute_Ownership_Divestiture : merge Message {}
message M_Request_Attribute_Ownership_Assumption : merge Message {}
message M_Negotiated_Attribute_Ownership_Divestiture : merge Message {}
message M_Attribute_Ownership_Divestiture_Notification : merge Message {}
message M_Attribute_Ownership_Acquisition_Notification : merge Message {}
message M_Request_Attribute_Ownership_Acquisition : merge Message {}
message M_Request_Attribute_Ownership_Release : merge Message {}
message M_Query_Attribute_Ownership : merge Message {}
message M_Inform_Attribute_Ownership : merge Message {}
message M_Is_Attribute_Owned_By_Federate : merge Message {}
message M_Attribute_Is_Not_Owned : merge Message {}
message M_Attribute_Owned_By_Rti : merge Message {}
message M_Attribute_Ownership_Acquisition_If_Available : merge Message {}
message M_Attribute_Ownership_Unavailable : merge Message {}
message M_Unconditional_Attribute_Ownership_Divestiture : merge Message {}
message M_Attribute_Ownership_Acquisition : merge Message {}
message M_Cancel_Negotiated_Attribute_Ownership_Divestiture : merge Message {}
message M_Attribute_Ownership_Release_Response : merge Message {}
message M_Cancel_Attribute_Ownership_Acquisition : merge Message {}
message M_Confirm_Attribute_Ownership_Acquisition_Cancellation : merge Message {}

        // Time
message M_Change_Attribute_Order_Type : merge Message {}
message M_Change_Interaction_Order_Type : merge Message {}
message M_Enable_Time_Regulation : merge Message {
    required double fedTime
    required double lookahead
}
message M_Disable_Time_Regulation : merge Message {}
message M_Enable_Time_Constrained : merge Message {}
message M_Disable_Time_Constrained : merge Message {}
message M_Query_Lbts : merge Message {}
message M_Query_Federate_Time : merge Message {}
message M_Query_Min_Next_Event_Time : merge Message {}
message M_Modify_Lookahead : merge Message {}
message M_Query_Lookahead : merge Message {}
message M_Retract : merge Message {}
message M_Request_Retraction : merge Message {}
message M_Time_Advance_Request : merge Message {}
message M_Time_Advance_Request_Available : merge Message {}
message M_Next_Event_Request : merge Message {}
message M_Next_Event_Request_Available : merge Message {}
message M_Flush_Queue_Request : merge Message {}
message M_Time_Advance_Grant : merge Message {}
message M_Enable_Asynchronous_Delivery : merge Message {}
message M_Disable_Asynchronous_Delivery : merge Message {}
message M_Time_Regulation_Enabled : merge Message {}
message M_Time_Constrained_Enabled : merge Message {}

        // Data Distribution Management
message M_Ddm_Create_Region : merge Message {}
message M_Ddm_Modify_Region : merge Message {}
message M_Ddm_Delete_Region : merge Message {}
message M_Ddm_Register_Object : merge Message {}
message M_Ddm_Associate_Region : merge Message {}
message M_Ddm_Unassociate_Region : merge Message {}
message M_Ddm_Subscribe_Attributes : merge Message {}
message M_Ddm_Unsubscribe_Attributes : merge Message {}
message M_Ddm_Subscribe_Interaction : merge Message {}
message M_Ddm_Unsubscribe_Interaction : merge Message {}
message M_Ddm_Request_Update : merge Message {}

        // Support Services
message M_Get_Object_Class_Handle : merge Message {}
message M_Get_Object_Class_Name : merge Message {}
message M_Get_Attribute_Handle : merge Message {}
message M_Get_Attribute_Name : merge Message {}
message M_Get_Interaction_Class_Handle : merge Message {}
message M_Get_Interaction_Class_Name : merge Message {}
message M_Get_Parameter_Handle : merge Message {}
message M_Get_Parameter_Name : merge Message {}
message M_Get_Object_Instance_Handle : merge Message {}
message M_Get_Object_Instance_Name : merge Message {}
message M_Get_Space_Handle : merge Message {}
message M_Get_Space_Name : merge Message {}
message M_Get_Dimension_Handle : merge Message {}
message M_Get_Dimension_Name : merge Message {}
message M_Get_Attribute_Space_Handle : merge Message {}
message M_Get_Object_Class : merge Message {}
message M_Get_Interaction_Space_Handle : merge Message {}
message M_Get_Transportation_Handle : merge Message {}
message M_Get_Transportation_Name : merge Message {}
message M_Get_Ordering_Handle : merge Message {}
message M_Get_Ordering_Name : merge Message {}
message M_Enable_Class_Relevance_Advisory_Switch : merge Message {}
message M_Disable_Class_Relevance_Advisory_Switch : merge Message {}
message M_Enable_Attribute_Relevance_Advisory_Switch : merge Message {}
message M_Disable_Attribute_Relevance_Advisory_Switch : merge Message {}
message M_Enable_Attribute_Scope_Advisory_Switch : merge Message {}
message M_Disable_Attribute_Scope_Advisory_Switch : merge Message {}
message M_Enable_Interaction_Relevance_Advisory_Switch : merge Message {}
message M_Disable_Interaction_Relevance_Advisory_Switch : merge Message {}
message M_Tick_Request : merge Message {}
message M_Tick_Request_Next : merge Message {}
message M_Tick_Request_Stop : merge Message {}

native SocketUN {
    language CXX [#include "SocketUN.hh"]
}

native StreamType {
    language CXX [typedef SocketUN* StreamType;]
}

native M_Type {
    language CXX [typedef Message::Type M_Type;]
}

factory M_Factory {

	factoryCreator  Message create(M_Type)
	factoryReceiver Message receive(StreamType)
}
