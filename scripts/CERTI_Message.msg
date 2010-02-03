
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

// Message is the base class for
// message exchanged between RTIA and Federate (libRTI) AKA CERTI Message. 
// Every message which is a merge from Message will first include the content 
// of a Message
native Message {
    language CXX    [#include "Message.hh"]
    language Python [import certi.Message]
    language Java   [import certi.Message]
}

native ResignAction  {
    representation uint16
    language CXX [#include "RTItypes.hh"]	
}

native Handle {
    representation uint64
    language CXX [#include "certi.hh"]
}

native RegionHandle {
    representation uint32
    language CXX [#include "certi.hh"]
}

native AttributeHandle {
    representation uint32
    language CXX [#include "certi.hh"]
}

native DimensionHandle {
    representation uint32
    language CXX [#include "certi.hh"]
}

native InteractionClassHandle {
    representation uint32
    language CXX [#include "certi.hh"]
}

native ObjectClassHandle {
    representation uint32
    language CXX [#include "certi.hh"]
}

native ObjectHandle {
    representation uint32
    language CXX [#include "certi.hh"]
}

native FederateHandle {
    representation uint32
    language CXX [#include "RTItypes.hh"]
}

native ParameterHandle {
    representation uint32
    language CXX [#include "certi.hh"]
}

native SpaceHandle {
    representation uint32
    language CXX [#include "certi.hh"]
}

native InteractionHandle {
    representation uint32
    language CXX [#include "certi.hh"]
}

native TransportType {
    representation uint8
    language CXX [#include "certi.hh"]
}

native OrderType {
    representation uint8
    language CXX [#include "certi.hh"]
}

message M_Create_Federation_Execution : merge Message {
	required string federationName // the federation name
	required string FEDid          // the Federation ID which is in fact a filename
}

message M_Destroy_Federation_Execution : merge Message {
	required string federationName
}

message M_Join_Federation_Execution : merge Message {
	required FederateHandle federate
	required string federationName
	required string federateName
}

message M_Resign_Federation_Execution : merge Message {
	required ResignAction resignAction
}

message M_Register_Federation_Synchronization_Point : merge Message {        
	repeated FederateHandle federateSet // the set of Federate which will participate to this sync point.
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

message M_Initiate_Federate_Restore : merge Message {
	required FederateHandle federate
}
message M_Federate_Restore_Complete : merge Message {}
message M_Federate_Restore_Not_Complete : merge Message {}
message M_Federation_Restored : merge Message {}
message M_Federation_Not_Restored : merge Message {}
message M_Federation_Restore_Begun : merge Message {}

// Declaration
message M_Publish_Object_Class : merge Message {
	required ObjectClassHandle objectClass
        repeated Handle handles
}

message M_Unpublish_Object_Class : merge Message {
	required ObjectClassHandle objectClass
}
message M_Publish_Interaction_Class : merge Message {}
message M_Unpublish_Interaction_Class : merge Message {}
message M_Subscribe_Object_Class_Attributes : merge Message {
	required ObjectClassHandle objectClass
        repeated Handle handles
}

message M_Unsubscribe_Object_Class : merge Message {
	required ObjectClassHandle objectClass
}

message M_Subscribe_Interaction_Class : merge Message {}
message M_Unsubscribe_Interaction_Class : merge Message {}

message M_Start_Registration_For_Object_Class : merge Message {
	required ObjectClassHandle objectClass
}

message M_Stop_Registration_For_Object_Class : merge Message {
	required ObjectClassHandle objectClass
}

message M_Turn_Interactions_On : merge Message {}
message M_Turn_Interactions_Off : merge Message {}

// Object
message M_Register_Object_Instance : merge Message {
	required ObjectClassHandle objectClass
        required ObjectHandle      object
        required string            objectName
}

message M_Update_Attribute_Values : merge Message {
        required ObjectClassHandle  objectClass
	required ObjectHandle       object
        repeated Handle             handle
        repeated string             attributeValues // FIXME check this one
        required ResignAction       resignAction
}

message M_Discover_Object_Instance : merge Message {
	required ObjectClassHandle objectClass
        required ObjectHandle      object
        required string            objectName
        required ResignAction      resignAction
}

message M_Reflect_Attribute_Values : merge Message {
 	required ObjectClassHandle  objectClass
	required ObjectHandle       object
        repeated Handle             handle
        repeated string             attributeValues // FIXME check this one
        required ResignAction       resignAction
}

message M_Send_Interaction : merge Message {
	required InteractionClassHandle interactionClass
        repeated Handle                 handles
	repeated string                 values
	required RegionHandle           region
	required ResignAction           resignAction          
}

message M_Receive_Interaction : merge Message {
	required InteractionClassHandle interactionClass
        repeated Handle                 handles
	repeated string                 values
	required RegionHandle           region
	required ResignAction           resignAction
}

message M_Delete_Object_Instance : merge Message {
	required ObjectClassHandle objectClass
        required ObjectHandle      object
        required string            objectName
        required ResignAction      resignAction
}

message M_Local_Delete_Object_Instance : merge Message {}

message M_Remove_Object_Instance : merge Message {
	required ObjectClassHandle objectClass
        required ObjectHandle      object
        required string            objectName
        required ResignAction      resignAction
}

message M_Change_Attribute_Transportation_Type : merge Message {
        required TransportType transport
        required OrderType     order
        required ObjectHandle  object
	repeated Handle        handles
}

message M_Change_Interaction_Transportation_Type : merge Message {
	required InteractionHandle interactionClass
        required TransportType     transport
        required OrderType         order
}

message M_Request_Object_Attribute_Value_Update : merge Message {}
message M_Request_Class_Attribute_Value_Update : merge Message {}
message M_Provide_Attribute_Value_Update : merge Message {}
message M_Attributes_In_Scope : merge Message {}
message M_Attributes_Out_Of_Scope : merge Message {}
message M_Turn_Updates_On_For_Object_Instance : merge Message {}
message M_Turn_Updates_Off_For_Object_Instance : merge Message {}

// Ownership management
message M_Request_Attribute_Ownership_Divestiture : merge Message {}
message M_Request_Attribute_Ownership_Assumption : merge Message {
	required ObjectHandle object
	repeated AttributeHandle attributes
}

message M_Negotiated_Attribute_Ownership_Divestiture : merge Message {
	required ObjectHandle object
	repeated AttributeHandle attributes
}

message M_Attribute_Ownership_Divestiture_Notification : merge Message {}
message M_Attribute_Ownership_Acquisition_Notification : merge Message {}
message M_Request_Attribute_Ownership_Acquisition : merge Message {
	required ObjectHandle object
	repeated AttributeHandle attributes
}
message M_Request_Attribute_Ownership_Release : merge Message {
	required ObjectHandle object
	repeated AttributeHandle attributes
}

message M_Query_Attribute_Ownership : merge Message {
	required ObjectHandle object
	required AttributeHandle attribute	
}

message M_Inform_Attribute_Ownership : merge Message {
	required ObjectHandle object
	required AttributeHandle attribute
	required FederateHandle federate
}

message M_Is_Attribute_Owned_By_Federate : merge Message {
	required ObjectHandle object
	required AttributeHandle attribute
}

message M_Attribute_Is_Not_Owned : merge Message {
	required ObjectHandle object
	required AttributeHandle attribute
	required FederateHandle federate
}

message M_Attribute_Owned_By_Rti : merge Message {}
message M_Attribute_Ownership_Acquisition_If_Available : merge Message {}
message M_Attribute_Ownership_Unavailable : merge Message {}
message M_Unconditional_Attribute_Ownership_Divestiture : merge Message {}
message M_Attribute_Ownership_Acquisition : merge Message {}
message M_Cancel_Negotiated_Attribute_Ownership_Divestiture : merge Message {
	required ObjectHandle object
	repeated AttributeHandle attributes
}
message M_Attribute_Ownership_Release_Response : merge Message {}
message M_Cancel_Attribute_Ownership_Acquisition : merge Message {}
message M_Confirm_Attribute_Ownership_Acquisition_Cancellation : merge Message {}

        // Time
message M_Change_Attribute_Order_Type : merge Message {
        required TransportType transport
        required OrderType     order
        required ObjectHandle  object
	repeated Handle        handles
}

message M_Change_Interaction_Order_Type : merge Message {
	required InteractionHandle interactionClass
        required TransportType     transport
        required OrderType         order
}

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
message M_Ddm_Create_Region : merge Message {
	required SpaceHandle  space
        required uint32       nbExtent
        required RegionHandle region
}

message M_Ddm_Modify_Region : merge Message {}
message M_Ddm_Delete_Region : merge Message {}

message M_Ddm_Register_Object : merge Message {
	required ObjectClassHandle objectClass
	required ObjectHandle      object
	repeated AttributeHandle   handles
	// repeated M_Regions   regions 
}

message M_Ddm_Associate_Region : merge Message {
	required ObjectHandle object
	required RegionHandle region	
}

message M_Ddm_Unassociate_Region : merge Message {
	required ObjectHandle object
	required RegionHandle region
}

message M_Ddm_Subscribe_Attributes : merge Message {
	required ObjectClassHandle objectClass
	required RegionHandle      region
	required onoff             passive
	repeated AttributeHandle   handles
}

message M_Ddm_Unsubscribe_Attributes : merge Message {
	required ObjectClassHandle objectClass
	required RegionHandle      region
}

message M_Ddm_Subscribe_Interaction : merge Message {
	required InteractionClassHandle interactionClass
	required RegionHandle           region
	required onoff                  passive
}

message M_Ddm_Unsubscribe_Interaction : merge Message {
	required InteractionClassHandle interactionClass
	required RegionHandle           region
	required onoff                  passive
}

message M_Ddm_Request_Update : merge Message {}

        // Support Services
message M_Get_Object_Class_Handle : merge Message {
	required ObjectClassHandle objectClass
        required string            name
        required AttributeHandle   attribute
}

message M_Get_Object_Class_Name : merge Message {
	required ObjectClassHandle objectClass
        required string            name
        required AttributeHandle   attribute
}

message M_Get_Attribute_Handle : merge Message {
	required ObjectClassHandle objectClass
        required string            name
        required AttributeHandle   attribute
}

message M_Get_Attribute_Name : merge Message {
	required ObjectClassHandle objectClass
        required string            name
        required AttributeHandle   attribute
}

message M_Get_Interaction_Class_Handle : merge Message {
	required InteractionClassHandle interactionClass
        required string                 name
        required ParameterHandle        parameter
}

message M_Get_Interaction_Class_Name : merge Message {
	required InteractionClassHandle interactionClass
        required string                 name
        required ParameterHandle        parameter	
}

message M_Get_Parameter_Handle : merge Message {
	required InteractionClassHandle interactionClass
        required string                 name
        required ParameterHandle        parameter
}

message M_Get_Parameter_Name : merge Message {
	required InteractionClassHandle interactionClass
        required string                 name
        required ParameterHandle        parameter
}

message M_Get_Object_Instance_Handle : merge Message {}
message M_Get_Object_Instance_Name : merge Message {}

message M_Get_Space_Handle : merge Message {
	required string      name
        required SpaceHandle space
}

message M_Get_Space_Name : merge Message {
	required string      name
        required SpaceHandle space
}

message M_Get_Dimension_Handle : merge Message {
	required string          name
        required SpaceHandle     space
	required DimensionHandle dimension
}

message M_Get_Dimension_Name : merge Message {
	required string          name
        required SpaceHandle     space
	required DimensionHandle dimension
}

message M_Get_Attribute_Space_Handle : merge Message {
	required ObjectClassHandle objectClass
        required AttributeHandle   attribute
        required SpaceHandle       space
}

message M_Get_Object_Class : merge Message {
        required ObjectClassHandle objectClass
        required ObjectHandle      object 
}

message M_Get_Interaction_Space_Handle : merge Message {
	required InteractionClassHandle interactionClass
        required SpaceHandle            space
}

message M_Get_Transportation_Handle : merge Message {
	required string         name
        required TransportType  transport
}

message M_Get_Transportation_Name : merge Message {
	required string         name
        required TransportType  transport
}

message M_Get_Ordering_Handle : merge Message {
	required string     name
        required OrderType  order
}

message M_Get_Ordering_Name : merge Message {
	required string     name
        required OrderType  order
}
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
