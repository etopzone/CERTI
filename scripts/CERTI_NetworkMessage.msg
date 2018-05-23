
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
package certi
version 1.0

// Network message is the base class
// for message exchanged between RTIA and RTIG
// AKA CERTI Network Message
native NetworkMessage {
    language CXX    [#include "NetworkMessage.hh"]
    language Java   [import certi.communication.NetworkMessage]
}

native FederationTime {
    language CXX    [#include "FedTimeD.hh"]    
    language Java   [import certi.FedTimeD]
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

native ParameterHandle {
    representation uint32
    language CXX [#include "certi.hh"]
}

native SpaceHandle {
    representation uint32
    language CXX [#include "certi.hh"]
}

native RegionHandle {
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

native RtiVersion {
    representation uint8
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

native FederateHandle {
    representation uint32
    language CXX [#include "certi.hh"]
}

native AttributeValue_t {
    representation repeated byte
    language CXX [#include "certi.hh"]
}

native ParameterValue_t {
    representation repeated byte
    language CXX [#include "certi.hh"]
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
//message EventRetraction {
//    required FederateHandle sendingFederate {default = 0}
//    required uint64         SN              {default = 0}
//}

message NM_FOM_Dimension {
    required DimensionHandle handle {default=0}
    required string name
}

message NM_FOM_Routing_Space {
    required SpaceHandle space {default=0}
    required string name
    repeated NM_FOM_Dimension dimensions
}

message NM_FOM_Attribute {
    required AttributeHandle handle {default=0}
    required string name
    required SpaceHandle spaceHandle {default=0}
    required OrderType order
    required TransportType transport
}

message NM_FOM_Object_Class {
    required ObjectClassHandle handle     {default=0}
    required ObjectClassHandle superClass {default=0}
    required string name
    repeated NM_FOM_Attribute attributes
}

message NM_FOM_Parameter {
    required ParameterHandle handle {default=0}
    required string name
}

message NM_FOM_Interaction_Class {
    required InteractionClassHandle interactionClass {default=0}
    required string name
        required InteractionClassHandle superClass {default=0}
    required SpaceHandle space {default=0}
    required OrderType order
    required TransportType transport
        repeated NM_FOM_Parameter parameters
}

message NM_Close_Connexion : merge NetworkMessage {}

message NM_Message_Null : merge NetworkMessage {
}

// Create the federation execution
message NM_Create_Federation_Execution : merge NetworkMessage {
    required string     federationExecutionName // the federation name
    required RtiVersion rtiVersion              // the rti version
    repeated string     fomModuleDesignators    // the Federation ID which is in fact a filename
    optional string     mimDesignator
}

// Destroy the federation execution
message NM_Destroy_Federation_Execution : merge NetworkMessage {
    required string federationName
}

// Join the federation
message NM_Join_Federation_Execution : merge NetworkMessage {
    required  int32  numberOfRegulators
    required uint32  multicastAddress
    required uint32  bestEffortAddress
    required uint32  bestEffortPeer
    required string  federationExecutionName // the federation name
    optional string  federateName            // the federate name (should be unique within a federation)
    required RtiVersion  rtiVersion              // the rti version
    required string  federateType
    repeated string  additionalFomModules
    repeated NM_FOM_Routing_Space routingSpaces
    repeated NM_FOM_Object_Class objectClasses
    repeated NM_FOM_Interaction_Class interactionClasses
}

message NM_Additional_Fom_Module : merge NetworkMessage {
    repeated NM_FOM_Routing_Space routingSpaces
    repeated NM_FOM_Object_Class objectClasses
    repeated NM_FOM_Interaction_Class interactionClasses
}

// Resign from federation
message NM_Resign_Federation_Execution : merge NetworkMessage {
}

message NM_Set_Time_Regulating : merge NetworkMessage {
    required onoff regulator
}

message NM_Set_Time_Constrained : merge NetworkMessage {
    required onoff constrained
}

message NM_Time_Regulation_Enabled : merge NetworkMessage {}

message NM_Time_Constrained_Enabled : merge NetworkMessage {}

message NM_Set_Class_Relevance_Advisory_Switch : merge NetworkMessage {    
    required onoff classRelevanceAdvisorySwitch // value of CRA switch     
}

message NM_Set_Interaction_Relevance_Advisory_Switch : merge NetworkMessage {    
    required onoff interactionRelevanceAdvisorySwitch // value of IRA switch     
}

message NM_Set_Attribute_Relevance_Advisory_Switch : merge NetworkMessage {    
    required onoff attributeRelevanceAdvisorySwitch // value of ARA switch     
}

message NM_Set_Attribute_Scope_Advisory_Switch : merge NetworkMessage {    
    required onoff attributeScopeAdvisorySwitch // value of ASA switch     
}

message NM_Register_Federation_Synchronization_Point : merge NetworkMessage {    
    required bool exists // tell whether if the synchronization set is specified or not
    repeated FederateHandle federates // the federate set
}

// HLA 1.3    - §4.7
// IEEE-1516  - §4.7
message NM_Confirm_Synchronization_Point_Registration : merge NetworkMessage {
   required bool   successIndicator // the success indicator true=successful
   optional string failureReason    // [IEEE-1516] the failure reason.
}

message NM_Announce_Synchronization_Point : merge NetworkMessage {}

message NM_Synchronization_Point_Achieved : merge NetworkMessage {}

message NM_Federation_Synchronized : merge NetworkMessage {}

message NM_Request_Federation_Save : merge NetworkMessage {}

message NM_Federate_Save_Begun : merge NetworkMessage {}

message NM_Federate_Save_Complete : merge NetworkMessage {}

message NM_Federate_Save_Not_Complete : merge NetworkMessage {}

message NM_Initiate_Federate_Save : merge NetworkMessage {}

message NM_Federation_Saved : merge NetworkMessage {}

message NM_Federation_Not_Saved : merge NetworkMessage {}

message NM_Request_Federation_Restore : merge NetworkMessage {}

message NM_Federate_Restore_Complete : merge NetworkMessage {}

message NM_Federate_Restore_Not_Complete : merge NetworkMessage {}

message NM_Request_Federation_Restore_Succeeded : merge NetworkMessage {}

message NM_Request_Federation_Restore_Failed : merge NetworkMessage {}

message NM_Federation_Restore_Begun : merge NetworkMessage {}

message NM_Initiate_Federate_Restore : merge NetworkMessage {}

message NM_Federation_Restored : merge NetworkMessage {}

message NM_Federation_Not_Restored : merge NetworkMessage {}

message NM_Publish_Object_Class : merge NetworkMessage {
    required ObjectClassHandle  objectClass
    repeated AttributeHandle    attributes
}

message NM_Unpublish_Object_Class : merge NM_Publish_Object_Class {
}

message NM_Publish_Interaction_Class : merge NetworkMessage {
    required InteractionClassHandle interactionClass // the interaction class
}

message NM_Unpublish_Interaction_Class : merge NM_Publish_Interaction_Class {}

message NM_Subscribe_Object_Class : merge NetworkMessage {
    required ObjectClassHandle  objectClass
    repeated AttributeHandle    attributes    
}

message NM_Unsubscribe_Object_Class : merge NM_Subscribe_Object_Class {
}

message NM_Subscribe_Interaction_Class : merge NM_Publish_Interaction_Class {}

message NM_Unsubscribe_Interaction_Class : merge NM_Publish_Interaction_Class {}

message NM_Turn_Interactions_On : merge NM_Publish_Interaction_Class {}

message NM_Turn_Interactions_Off : merge NM_Publish_Interaction_Class {}

// HLA 1.3 §6.2
message NM_Register_Object : merge NetworkMessage {
    required ObjectClassHandle objectClass
    optional string            objectName
    required ObjectHandle      object        
}

// HLA 1.3 §6.3
message NM_Discover_Object : merge NetworkMessage {
    required ObjectClassHandle objectClass
    required ObjectHandle      object
}

// HLA 1.3 §6.4
message NM_Update_Attribute_Values : merge NetworkMessage {
    required ObjectHandle             object
    repeated AttributeHandle          attributes
    repeated AttributeValue_t         values
    optional EventRetractionHandle    event    
}

// HLA 1.3 §6.5
message NM_Reflect_Attribute_Values : merge NetworkMessage {
    required ObjectHandle             object
    repeated AttributeHandle          attributes
    repeated AttributeValue_t         values
    optional EventRetractionHandle    event
}

// HLA 1.3 §6.6
message NM_Send_Interaction : merge NetworkMessage {
    required InteractionClassHandle   interactionClass
    repeated ParameterHandle          parameters
    repeated ParameterValue_t         values
    required RegionHandle             region // FIXME check this....
}

// HLA 1.3 §6.7
message NM_Receive_Interaction : merge NetworkMessage {
    required InteractionClassHandle   interactionClass
    repeated ParameterHandle          parameters
    repeated ParameterValue_t         values
    optional EventRetractionHandle    event    
}

// HLA 1.3 §6.8 Delete Object Instance
message NM_Delete_Object : merge NetworkMessage {
    required ObjectHandle             object
    optional EventRetractionHandle    event
}

// HLA 1.3 §6.9 Remove Object Instance
message NM_Remove_Object : merge NetworkMessage {
    required ObjectHandle           object
    required ObjectClassHandle      objectClass
    optional EventRetractionHandle  event
}

message NM_Change_Attribute_Transport_Type : merge NetworkMessage {
    required ObjectHandle       object
    repeated AttributeHandle    attributes
    required TransportType      transport
}

message NM_Change_Attribute_Order_Type : merge NetworkMessage {
    required ObjectHandle       object
    repeated AttributeHandle    attributes
    required OrderType          order
}

message NM_Change_Interaction_Transport_Type : merge NetworkMessage {
    required InteractionClassHandle interactionClass
    required TransportType          transport
}

message NM_Change_Interaction_Order_Type : merge NetworkMessage {
    required InteractionClassHandle interactionClass
    required OrderType              order
}

message NM_Request_Class_Attribute_Value_Update : merge NetworkMessage {
    required ObjectClassHandle objectClass
    repeated AttributeHandle   attributes
}

message NM_Request_Object_Attribute_Value_Update : merge NetworkMessage {
    required ObjectHandle    object
    repeated AttributeHandle attributes
}

message NM_Is_Attribute_Owned_By_Federate : merge NetworkMessage {
    required ObjectHandle    object
    required AttributeHandle attribute
}

message NM_Query_Attribute_Ownership : merge NetworkMessage {
    required ObjectHandle    object
    required AttributeHandle attribute
}

message NM_Attribute_Is_Not_Owned : merge NetworkMessage {
    required ObjectHandle    object
    required AttributeHandle attribute
}

message NM_Inform_Attribute_Ownership : merge NetworkMessage {
    required ObjectHandle    object
    required AttributeHandle attribute
}

message NM_Attribute_Ownership_Base : merge NetworkMessage {
    required ObjectHandle    object
    repeated AttributeHandle attributes
}

message NM_Negotiated_Attribute_Ownership_Divestiture : merge NM_Attribute_Ownership_Base {
    
}

message NM_Attribute_Ownership_Acquisition_Notification : merge NM_Attribute_Ownership_Base {
}

message NM_Attribute_Ownership_Divestiture_Notification : merge NM_Attribute_Ownership_Base {
}

message NM_Request_Attribute_Ownership_Assumption : merge NM_Attribute_Ownership_Base {
}

// This is a base message which is used to 
// define several Attribute_Ownership_XXX
// since all these message contains the same fields
message NM_Attribute_Ownership_Unavailable : merge NM_Attribute_Ownership_Base {
}

message NM_Attribute_Ownership_Acquisition_If_Available : merge NM_Attribute_Ownership_Base {
}

message NM_Unconditional_Attribute_Ownership_Divestiture : merge NM_Attribute_Ownership_Base {
}

message NM_Attribute_Ownership_Acquisition : merge NM_Attribute_Ownership_Base {
}

message NM_Request_Attribute_Ownership_Release : merge NM_Attribute_Ownership_Base {
}

message NM_Cancel_Negotiated_Attribute_Ownership_Divestiture : merge NM_Attribute_Ownership_Base {
}

message NM_Attribute_Ownership_Release_Response : merge NM_Attribute_Ownership_Base {
}

message NM_Cancel_Attribute_Ownership_Acquisition : merge NM_Attribute_Ownership_Base {
}

message NM_Confirm_Attribute_Ownership_Acquisition_Cancellation : merge NM_Attribute_Ownership_Base {
}

message NM_DDM_Create_Region : merge NetworkMessage {        
    repeated AttributeHandle attributes
    required bool DDM_bool { default=false }
    required SpaceHandle  space
    required RegionHandle region
    required uint32       nbExtents // FIXME not sure it must be here
}

message NM_DDM_Modify_Region : merge NetworkMessage {
    repeated AttributeHandle attributes
    required bool DDM_bool { default=false }
    required RegionHandle region
}

message NM_DDM_Delete_Region : merge NetworkMessage {
    required RegionHandle region
}

message NM_DDM_Associate_Region : merge NetworkMessage {
    repeated AttributeHandle attributes
    required bool DDM_bool   { default=false }
    required RegionHandle    region
    required ObjectHandle    object
}

message NM_DDM_Unassociate_Region : merge NetworkMessage {
    repeated AttributeHandle attributes
    required RegionHandle    region
    required ObjectHandle    object
    required bool DDM_bool { default=false }
}

message NM_DDM_Register_Object : merge NetworkMessage {
    required ObjectClassHandle objectClass
    repeated AttributeHandle   attributes
    required RegionHandle      region
    optional string            objectInstanceName
    required ObjectHandle      object
    required bool DDM_bool { default=false }
}

message NM_DDM_Subscribe_Attributes : merge NetworkMessage {
    required ObjectClassHandle objectClass
    required RegionHandle      region
    repeated AttributeHandle   attributes
    required bool DDM_bool { default=false }
}

message NM_DDM_Unsubscribe_Attributes : merge NetworkMessage {
    required ObjectClassHandle objectClass
    required RegionHandle      region
    repeated AttributeHandle attributes
    required bool DDM_bool { default=false }
}

message NM_DDM_Subscribe_Interaction : merge NetworkMessage {
    required InteractionClassHandle interactionClass 
    required RegionHandle           region
    required bool DDM_bool { default=false }
}

message NM_DDM_Unsubscribe_Interaction : merge NM_DDM_Subscribe_Interaction {}

message NM_Provide_Attribute_Value_Update : merge NM_Request_Object_Attribute_Value_Update {
}

message NM_Start_Registration_For_Object_Class : merge NetworkMessage {
    required ObjectClassHandle objectClass
}

message NM_Stop_Registration_For_Object_Class : merge NetworkMessage {
    required ObjectClassHandle objectClass
}

// HLA 1516 - §6.2
message NM_Reserve_Object_Instance_Name : merge NetworkMessage {
    required string objectName
}

// HLA 1516 - §6.3
message NM_Reserve_Object_Instance_Name_Succeeded : merge NM_Reserve_Object_Instance_Name {
}

message NM_Reserve_Object_Instance_Name_Failed : merge NM_Reserve_Object_Instance_Name {
}

message NM_Message_Null_Prime : merge NetworkMessage {
   required FederationTime timestamp
}

message NM_Enable_Asynchronous_Delivery : merge NetworkMessage {
}

message NM_Disable_Asynchronous_Delivery : merge NetworkMessage {
}

message NM_Time_Advance_Request : merge NetworkMessage {
}

message NM_Time_Advance_Request_Available : merge NetworkMessage {
}

message NM_Next_Message_Request : merge NetworkMessage {
}

message NM_Next_Message_Request_Available : merge NetworkMessage {
}

message NM_Mom_Status : merge NetworkMessage {
    required bool momState
    required uint32 updatePeriod
}

message NM_Time_State_Update : merge NetworkMessage {
    required double lookahead
    required bool state
    required double galt
    required double lits
}

message New_NetworkMessage {
    required uint32          type  {default=0}
    //required string          name  {default="MessageBaseClass"}
    optional FederationTime  date  
    optional string          label 
    optional string          tag
}

native NMStreamType {
    language CXX [typedef Socket* NMStreamType;]
}

native NM_Type {
    language CXX [typedef NetworkMessage::Type NM_Type;]
}

factory NM_Factory {

    factoryCreator  NetworkMessage create(NM_Type)
    factoryReceiver NetworkMessage receive(NMStreamType)
}

