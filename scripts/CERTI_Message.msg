
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
version 1.0

// Message is the base class for
// message exchanged between RTIA and Federate (libRTI) AKA CERTI Message. 
// Every message which is a merge from Message will first include the content 
// of a Message
native Message {
    language CXX    [#include "Message.hh"] 
    language Java   [import certi.communication.CertiMessage]
}

native AttributeHandleSet {    
    representation combine
    language CXX  [#include "certi.hh"]
    language Java [import hla.rti.AttributeHandleSet;]
}

native LogicalTimeInterval {
    representation combine
    language Java [import hla.rti.LogicalTimeInterval;]
}

native FederateHandleSet {
    representation combine
    language CXX  [#include "certi.hh"]   
    language Java [import hla.rti.FederateHandleSet;]
}

native Region {
    language CXX  [#include "certi.hh"]    
    language Java [import hla.rti.Region;]
}

native Extents {    
    language Java [import hla.rti.Extents;]
}

native ResignAction  {
    representation uint16
    language CXX [#include "certi.hh"]	    
}

native Handle {
    representation uint32
    language CXX [#include "certi.hh"]
}

native RegionHandle {
    representation uint32
    language CXX [#include "certi.hh"]
}

native AttributeHandle {
    representation uint32
    language CXX [#include "certi.hh"]
    language CXX [#define ATTRIBUTE_HANDLE_UNDEFINED_VALUE 0]
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
    language CXX [#include "certi.hh"]
}

native ParameterHandle {
    representation uint32
    language CXX [#include "certi.hh"]
}

native SpaceHandle {
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

native AttributeHandleValuePairSet {
    representation combine
    language CXX   [combine AttributeHandleValuePairSet]
    language Java  [import hla.rti.SuppliedAttributes] 
}

native AttributeValue_t {
	representation repeated byte
	language CXX [#include "certi.hh"]
}

native ParameterValue_t {
	representation repeated byte
	language CXX [#include "certi.hh"]
}

native ParameterHandleValuePairSet {
    representation combine
    language CXX   [combine ParameterHandleValuePairSet]
    language Java  [import hla.rti.SuppliedParameters]
}

native EventRetractionHandle {
    representation combine    
    language CXX [combine EventRetractionHandle]      
}

// The EventRetraction is not inheriting from base "Message"
// this is a plain message which may be used as field
// in messages merging from "Message". As such
// it won't appear in the generated factory method.
// It's a convenient way to describe a structured native
// message using "combine". 
message EventRetraction {
    required FederateHandle sendingFederate {default = 0}
    required uint64         SN              {default = 0}
}

// Connexion initialization message
message M_Open_Connexion : merge Message {
    required uint32 versionMajor
    required uint32 versionMinor
}

// Closing connexion message
message M_Close_Connexion : merge Message {
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

message M_Request_Federation_Restore_Failed : merge Message {
     required string reason
}

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
    repeated AttributeHandle   attributes
}

message M_Unpublish_Object_Class : merge Message {
	required ObjectClassHandle objectClass
}

message M_Publish_Interaction_Class : merge Message {
	required InteractionClassHandle  interactionClass
}

message M_Unpublish_Interaction_Class : merge Message {
	required InteractionClassHandle  interactionClass
}

message M_Subscribe_Object_Class_Attributes : merge Message {
	required ObjectClassHandle objectClass
    repeated AttributeHandle   attributes
    required bool              active {default=true}
}

message M_Unsubscribe_Object_Class : merge Message {
	required ObjectClassHandle objectClass
}

message M_Subscribe_Interaction_Class : merge Message {
	required InteractionClassHandle  interactionClass
}

message M_Unsubscribe_Interaction_Class : merge Message {
	required InteractionClassHandle  interactionClass
}

message M_Start_Registration_For_Object_Class : merge Message {
	required ObjectClassHandle objectClass
}

message M_Stop_Registration_For_Object_Class : merge Message {
	required ObjectClassHandle objectClass
}

message M_Turn_Interactions_On : merge Message {
	required InteractionClassHandle  interactionClass
}

message M_Turn_Interactions_Off : merge Message {
	required InteractionClassHandle  interactionClass
}

// Object
message M_Register_Object_Instance : merge Message {
	required ObjectClassHandle objectClass
    required ObjectHandle      object
    optional string            objectName
}

message M_Update_Attribute_Values : merge Message {
        required ObjectClassHandle     objectClass
        required ObjectHandle          object
        combine AttributeHandleValuePairSet {
           repeated AttributeHandle       attributes
           repeated AttributeValue_t      values
        }        
        combine EventRetractionHandle {
           optional EventRetraction eventRetraction                    
        } 
}

message M_Discover_Object_Instance : merge Message {
	    required ObjectClassHandle     objectClass
        required ObjectHandle          object
        required string                objectName        
        combine EventRetractionHandle {
           optional EventRetraction eventRetraction                    
        }
}

message M_Reflect_Attribute_Values : merge Message {
 	    required ObjectClassHandle  objectClass
	    required ObjectHandle       object
	    combine AttributeHandleValuePairSet {
           repeated AttributeHandle    attributes
           repeated AttributeValue_t   values
        }
        combine EventRetractionHandle {
           optional EventRetraction eventRetraction                    
        }
}

message M_Send_Interaction : merge Message {
	required InteractionClassHandle interactionClass
    repeated ParameterHandle        parameters
	repeated ParameterValue_t       values
	required RegionHandle           region
	combine EventRetractionHandle {
           optional EventRetraction eventRetraction                    
        }          
}

message M_Receive_Interaction : merge Message {
	required InteractionClassHandle interactionClass
    repeated ParameterHandle        parameters
	repeated ParameterValue_t       values
	required RegionHandle           region	
	combine EventRetractionHandle {
           optional EventRetraction eventRetraction                    
        }
}

message M_Delete_Object_Instance : merge Message {
	    required ObjectClassHandle objectClass
        required ObjectHandle      object
        required string            objectName        
        combine EventRetractionHandle {
           optional EventRetraction eventRetraction                    
        }
}

message M_Local_Delete_Object_Instance : merge Message {
	    required ObjectHandle      object
}

message M_Remove_Object_Instance : merge Message {
	    required ObjectClassHandle objectClass
        required ObjectHandle      object
        required string            objectName        
        combine EventRetractionHandle {
           optional EventRetraction eventRetraction                    
        }
}

message M_Change_Attribute_Transportation_Type : merge Message {
        required TransportType   transportationType
        required OrderType       orderType
        required ObjectHandle    object
	    repeated AttributeHandle attributes
}

message M_Change_Interaction_Transportation_Type : merge Message {
	    required InteractionClassHandle interactionClass
        required TransportType          transportationType
        required OrderType              orderType
}

message M_Request_Object_Attribute_Value_Update : merge Message {
	required ObjectHandle       object
    repeated AttributeHandle    attributes
}

message M_Request_Class_Attribute_Value_Update : merge Message {
	required ObjectClassHandle objectClass
    repeated AttributeHandle   attributes
}

message M_Provide_Attribute_Value_Update : merge Message {
	required ObjectHandle      object
    repeated AttributeHandle   attributes	
}

message M_Attributes_In_Scope : merge Message {
   required ObjectHandle object
   repeated AttributeHandle attributes
}

message M_Attributes_Out_Of_Scope : merge Message {
   required ObjectHandle object
   repeated AttributeHandle attributes
}

message M_Turn_Updates_On_For_Object_Instance : merge Message {
   required ObjectHandle object
   repeated AttributeHandle attributes
}

message M_Turn_Updates_Off_For_Object_Instance : merge Message {
   required ObjectHandle object
   repeated AttributeHandle attributes
}

// Ownership management
message M_Request_Attribute_Ownership_Divestiture : merge Message {
}

message M_Request_Attribute_Ownership_Assumption : merge Message {
	required ObjectHandle object
	repeated AttributeHandle attributes
}

message M_Negotiated_Attribute_Ownership_Divestiture : merge Message {
	required ObjectHandle object
	repeated AttributeHandle attributes
}

message M_Attribute_Ownership_Divestiture_Notification : merge Message {
    required ObjectHandle object
	repeated AttributeHandle attributes
}

message M_Attribute_Ownership_Acquisition_Notification : merge Message {
    required ObjectHandle object
	repeated AttributeHandle attributes
}

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

message M_Attribute_Ownership_Acquisition_If_Available : merge Message {
    required ObjectHandle object
	repeated AttributeHandle attributes
}

message M_Attribute_Ownership_Unavailable : merge Message {
    required ObjectHandle object
	repeated AttributeHandle attributes
}

message M_Unconditional_Attribute_Ownership_Divestiture : merge Message {
	required ObjectHandle object
	repeated AttributeHandle attributes
}

message M_Attribute_Ownership_Acquisition : merge Message {
	required ObjectHandle object
	repeated AttributeHandle attributes
}

message M_Cancel_Negotiated_Attribute_Ownership_Divestiture : merge Message {
	required ObjectHandle object
	repeated AttributeHandle attributes
}

message M_Attribute_Ownership_Release_Response : merge Message {
	required ObjectHandle object
	repeated AttributeHandle attributes
}

message M_Cancel_Attribute_Ownership_Acquisition : merge Message {
	required ObjectHandle object
	repeated AttributeHandle attributes
}

message M_Confirm_Attribute_Ownership_Acquisition_Cancellation : merge Message {
    required ObjectHandle object
	repeated AttributeHandle attributes
}

        // Time
message M_Change_Attribute_Order_Type : merge Message {
        required TransportType      transport
        required OrderType          order
        required ObjectHandle       object
	    repeated AttributeHandle    attributes
}

message M_Change_Interaction_Order_Type : merge Message {
	    required InteractionClassHandle interactionClass
        required TransportType          transport
        required OrderType              order
}

// HLA 1.3 - §8.2
message M_Enable_Time_Regulation : merge Message {    
    combine LogicalTimeInterval {
        required double lookahead {default=0}
    }
}

// HLA 1.3 - §8.4
message M_Disable_Time_Regulation : merge Message {        
    combine LogicalTimeInterval {
        required double lookahead {default=0}
    }
}

// HLA 1.3 - §8.5
message M_Enable_Time_Constrained : merge Message {   
}

// HLA 1.3 - §8.7
message M_Disable_Time_Constrained : merge Message {    
}

message M_Query_Lbts : merge Message {}
message M_Query_Federate_Time : merge Message {}
message M_Query_Min_Next_Event_Time : merge Message {}

message M_Modify_Lookahead : merge Message {
	required double lookahead {default=0}
}

message M_Query_Lookahead : merge Message {
	required double lookahead {default=0}
}

message M_Retract : merge Message {
    combine EventRetractionHandle {
          required EventRetraction eventRetraction                    
    }	
}

message M_Request_Retraction : merge Message {}

// HLA 1.3 - §8.8
message M_Time_Advance_Request : merge Message {}

message M_Time_Advance_Request_Available : merge Message {}

message M_Next_Event_Request : merge Message {}

message M_Next_Event_Request_Available : merge Message {}

message M_Flush_Queue_Request : merge Message {}

message M_Time_Advance_Grant : merge Message {}

message M_Enable_Asynchronous_Delivery : merge Message {}

message M_Disable_Asynchronous_Delivery : merge Message {}

// HLA 1.3 - §8.3
message M_Time_Regulation_Enabled : merge Message {}

// HLA 1.3 - §8.6
message M_Time_Constrained_Enabled : merge Message {}

native Extent {
   language CXX [#include "Extent.hh"]
}

// Data Distribution Management
message M_Ddm_Create_Region : merge Message {
	required SpaceHandle  space      // the routing space designator
	repeated Extent       extentSet  // the extent set    
    required RegionHandle region     // the region handle obtained upon successful creation
}

message M_Ddm_Modify_Region : merge Message {
	required RegionHandle region
	repeated Extent       extentSet  // the extent set
}

message M_Ddm_Delete_Region : merge Message {
	required RegionHandle region
}

message M_Ddm_Register_Object : merge Message {
	required ObjectClassHandle objectClass
	required ObjectHandle      object
	optional string            objectInstanceName
	repeated AttributeHandle   attributes
	// repeated RegionHandle      regions 
}

message M_Ddm_Associate_Region : merge Message {
	required ObjectHandle      object
	required RegionHandle      region	
	repeated AttributeHandle   attributes
}

message M_Ddm_Unassociate_Region : merge Message {
	required ObjectHandle object
	required RegionHandle region
}

message M_Ddm_Subscribe_Attributes : merge Message {
	required ObjectClassHandle objectClass
	required RegionHandle      region
	required onoff             passive
	repeated AttributeHandle   attributes
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

message M_Ddm_Request_Update : merge Message {
   required ObjectClassHandle objectClass
   required RegionHandle      region
   repeated AttributeHandle   attributes
}

// Support Services
message M_Get_Object_Class_Handle : merge Message {
	required ObjectClassHandle objectClass
    required string            className    
}

message M_Get_Object_Class_Name : merge Message {
	required ObjectClassHandle objectClass
    required string            className    
}

message M_Get_Attribute_Handle : merge Message {
	required ObjectClassHandle objectClass
    required string            attributeName
    required AttributeHandle   attribute {default=0}
}

message M_Get_Attribute_Name : merge Message {
	required ObjectClassHandle objectClass
    required string            attributeName
    required AttributeHandle   attribute {default=0}
}

message M_Get_Interaction_Class_Handle : merge Message {
	required InteractionClassHandle interactionClass {default=0}
    required string                 className
}

message M_Get_Interaction_Class_Name : merge Message {
	required InteractionClassHandle interactionClass    
	required string                 className	
}

message M_Get_Parameter_Handle : merge Message {
	required InteractionClassHandle interactionClass
    required string                 parameterName
    required ParameterHandle        parameter {default=0}
}

message M_Get_Parameter_Name : merge Message {
	required InteractionClassHandle interactionClass
    required string                 parameterName
    required ParameterHandle        parameter {default=0}
}

message M_Get_Object_Instance_Handle : merge Message {
	required ObjectHandle object {default=0}
	required string       objectInstanceName
}

message M_Get_Object_Instance_Name : merge Message {
	required ObjectHandle object
	required string       objectInstanceName
}

message M_Get_Space_Handle : merge Message {
	required string      spaceName
    required SpaceHandle space {default=0}
}

message M_Get_Space_Name : merge Message {
	required string      spaceName
	required SpaceHandle space
}

message M_Get_Dimension_Handle : merge Message {
	required string          dimensionName
 	required SpaceHandle     space
	required DimensionHandle dimension {default=0}
}

message M_Get_Dimension_Name : merge Message {
	required string          dimensionName
    required SpaceHandle     space
	required DimensionHandle dimension
}

message M_Get_Attribute_Space_Handle : merge Message {
	required ObjectClassHandle objectClass
    required AttributeHandle   attribute
    required SpaceHandle       space {default=0}
}

message M_Get_Object_Class : merge Message {
     required ObjectClassHandle objectClass
     required ObjectHandle      object 
}

message M_Get_Interaction_Space_Handle : merge Message {
	required InteractionClassHandle interactionClass
    required SpaceHandle            space {default=0}
}

message M_Get_Transportation_Handle : merge Message {
	required string         transportationName
    required TransportType  transportation {default=0}
}

message M_Get_Transportation_Name : merge Message {
	required string         transportationName
    required TransportType  transportation
}

message M_Get_Ordering_Handle : merge Message {
	required string     orderingName
    required OrderType  ordering {default=0}
}

message M_Get_Ordering_Name : merge Message {
	required string     orderingName
    required OrderType  ordering
}
message M_Enable_Class_Relevance_Advisory_Switch : merge Message {}
message M_Disable_Class_Relevance_Advisory_Switch : merge Message {}
message M_Enable_Attribute_Relevance_Advisory_Switch : merge Message {}
message M_Disable_Attribute_Relevance_Advisory_Switch : merge Message {}
message M_Enable_Attribute_Scope_Advisory_Switch : merge Message {}
message M_Disable_Attribute_Scope_Advisory_Switch : merge Message {}
message M_Enable_Interaction_Relevance_Advisory_Switch : merge Message {}
message M_Disable_Interaction_Relevance_Advisory_Switch : merge Message {}

message M_Tick_Request : merge Message {
	required bool    multiple
    required double  minTickTime
    required double  maxTickTime
}

message M_Tick_Request_Next : merge Message {}
message M_Tick_Request_Stop : merge Message {}

// HLA 1516 - §6.2
message M_Reserve_Object_Instance_Name : merge Message {
    required string objectName
}

// HLA 1516 - §6.3
message M_Reserve_Object_Instance_Name_Succeeded : merge M_Reserve_Object_Instance_Name {
}

message M_Reserve_Object_Instance_Name_Failed : merge M_Reserve_Object_Instance_Name {
}

native SocketUN {
    language CXX [#include "SocketUN.hh"]
}

native MStreamType {
    language CXX [typedef SocketUN* MStreamType;]
}

native M_Type {
    language CXX [typedef Message::Type M_Type;]
}

factory M_Factory {
	factoryCreator  Message create(M_Type)
	factoryReceiver Message receive(MStreamType)
}
