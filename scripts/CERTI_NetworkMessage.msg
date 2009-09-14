
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

// Network message is the base class
// for message exchanged between RTIA and RTIG
// AKA CERTI Network Message
native NetworkMessage {
    language CXX [#include "NetworkMessage.hh"]
    language Python [import certi.NetworkMessage]
    language Java   [import certi.NetworkMessage]
}

native FederationTime {
    language CXX    [#include "FedTimeD.hh"]
    language Python [import certi.FedTimeD]
    language Java   [import certi.FedTimeD]
}

message NewNetworkMessage {
    required uint32          type
    required string          name
    optional FederationTime  date
    optional string          label
    optional string          tag
}

native AttributeHandle {
    language CXX [#include "certi.hh"]
}

message NM_Close_Connexion : merge NetworkMessage {}

message NM_Message_Null : merge NetworkMessage {}

// Create the federation execution
message NM_Create_Federation_Execution : merge NetworkMessage {
	required string federationName // the federation name
	required string FEDid          // the Federation ID which is in fact a filename
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
	required string  federationName // the federation name
	required string  federateName   // the federate name (should be unique within a federation)
}

// Resign from federation
message NM_Resign_Federation_Execution : merge NetworkMessage {}

message NM_Set_Time_Regulating : merge NetworkMessage {
	required onoff regulator
}

message NM_Set_Time_Constrained : merge NetworkMessage {
	required onoff constrained
}

message NM_Time_Regulation_Enabled : merge NetworkMessage {}

message NM_Time_Constrained_Enabled : merge NetworkMessage {}

native ObjectClassHandle {
	language CXX [#include "certi.hh"]
}


message NM_Set_Class_Relevance_Advisory_Switch : merge NetworkMessage {    
	required onoff classRelevanceAdvisorySwitch // value of CRA switch 	
}

message NM_Set_Interaction_Relevance_Advisory_Switch : merge NetworkMessage {    
	required onoff interactionRelevanceAdvisorySwitch // value of IRA switch 	
}

message NM_Set_Attribute_Relevance_Advisory_Switch : merge NetworkMessage {    
	required bool attributeRelevanceAdvisorySwitch // value of ARA switch 	
}

message NM_Set_Attribute_Scope_Advisory_Switch : merge NetworkMessage {    
	required bool attributeScopeAdvisorySwitch // value of ASA switch 	
}

message NM_Register_Federation_Synchronization_Point : merge NetworkMessage {    
	required bool setExists // tell whether if the synchronization set is specified or not    
}

message NM_Synchronization_Point_Registration_Succeeded : merge NetworkMessage {}

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

message NM_Publish_Object_Class : merge NM_WithHandleArray {}

message NM_Unpublish_Object_Class : merge NetworkMessage {}

native InteractionClassHandle {
	language CXX [#include "certi.hh"]
}

message NM_Publish_Interaction_Class : merge NetworkMessage {
	required InteractionClassHandle interactionClass // the interaction class
}

message NM_Unpublish_Interaction_Class : merge NM_Publish_Interaction_Class {}

message NM_Subscribe_Object_Class : merge NM_WithHandleArray {}

message NM_Unsubscribe_Object_Class : merge NM_Unpublish_Object_Class {}

message NM_Subscribe_Interaction_Class : merge NM_Publish_Interaction_Class {}

message NM_Unsubscribe_Interaction_Class : merge NM_Publish_Interaction_Class {}

message NM_Turn_Interactions_On : merge NM_Publish_Interaction_Class {}

message NM_Turn_Interactions_Off : merge NM_Publish_Interaction_Class {}

message NM_Register_Object : merge NetworkMessage {}

message NM_Discover_Object : merge NM_Register_Object {}

message NM_Update_Attribute_Values : merge NM_WithHandleArray {}

message NM_Reflect_Attribute_Values : merge NM_Update_Attribute_Values {}

message NM_Send_Interaction : merge NM_WithHandleArray {}

message NM_Receive_Interaction : merge NM_Send_Interaction {}

message NM_Delete_Object : merge NM_Register_Object {}

message NM_Remove_Object : merge NM_Delete_Object {}

message NM_Change_Attribute_Transport_Type : merge NetworkMessage {}

message NM_Change_Attribute_Order_Type : merge NetworkMessage {}

message NM_Change_Interaction_Transport_Type : merge NetworkMessage {}

message NM_Change_Interaction_Order_Type : merge NetworkMessage {}

message NM_Request_Class_Attribute_Value_Update : merge NetworkMessage {}

native ObjectHandle {
	language CXX [#include "certi.hh"]
}

message NM_Request_Object_Attribute_Value_Update : merge NM_WithHandleArray {
	required ObjectHandle object
}

message NM_Is_Attribute_Owned_By_Federate : merge NetworkMessage {
      required AttributeHandle attribute
}

message NM_Query_Attribute_Ownership : merge NM_Is_Attribute_Owned_By_Federate {}

message NM_Attribute_Is_Not_Owned : merge NM_Is_Attribute_Owned_By_Federate {}

message NM_Inform_Attribute_Ownership : merge NM_Is_Attribute_Owned_By_Federate {}

message NM_Negotiated_Attribute_Ownership_Divestiture : merge NM_Request_Object_Attribute_Value_Update {}

message NM_Attribute_Ownership_Acquisition_Notification : merge NM_Request_Object_Attribute_Value_Update {}

message NM_Attribute_Ownership_Divestiture_Notification : merge NM_Request_Object_Attribute_Value_Update {}

message NM_Request_Attribute_Ownership_Assumption : merge NM_Negotiated_Attribute_Ownership_Divestiture {}

message NM_Attribute_Ownership_Unavailable : merge NM_Request_Object_Attribute_Value_Update {}

message NM_Attribute_Ownership_Acquisition_If_Available : merge NM_Request_Object_Attribute_Value_Update {}

message NM_Unconditional_Attribute_Ownership_Divestiture : merge NM_Request_Object_Attribute_Value_Update {}

message NM_Attribute_Ownership_Acquisition : merge NM_Negotiated_Attribute_Ownership_Divestiture {}

message NM_Request_Attribute_Ownership_Release : merge NM_Negotiated_Attribute_Ownership_Divestiture {}

message NM_Cancel_Negotiated_Attribute_Ownership_Divestiture : merge NM_Request_Object_Attribute_Value_Update {}

message NM_Attribute_Ownership_Release_Response : merge NM_Request_Object_Attribute_Value_Update {}

message NM_Cancel_Attribute_Ownership_Acquisition : merge NM_Request_Object_Attribute_Value_Update {}

message NM_Confirm_Attribute_Ownership_Acquisition_Cancellation : merge NM_Request_Object_Attribute_Value_Update {}

native SpaceHandle {
	language CXX [#include "certi.hh"]
}

message NM_DDM_Create_Region : merge NM_DDM_Base {
	required SpaceHandle space
	required int32 region
}

message NM_DDM_Modify_Region : merge NM_DDM_Base {
	required int32 region
}

message NM_DDM_Delete_Region : merge NM_DDM_Base {
	required int32 region
}

message NM_DDM_Associate_Region : merge NM_DDM_Base {
	required int32 region
	required ObjectHandle object
}
message NM_DDM_Unassociate_Region : merge NM_DDM_Base {}

message NM_DDM_Register_Object : merge NM_DDM_Base {}

message NM_DDM_Subscribe_Attributes : merge NM_DDM_Base {}
message NM_DDM_Unsubscribe_Attributes : merge NM_DDM_Base {}

message NM_DDM_Subscribe_Interaction : merge NM_DDM_Base {}
message NM_DDM_Unsubscribe_Interaction : merge NM_DDM_Subscribe_Interaction {}

message NM_Provide_Attribute_Value_Update : merge NM_Request_Object_Attribute_Value_Update {}

message NM_Get_FED_File : merge NetworkMessage {
        required string federateName // the Federate name
	required string FEDid        // the FED identifier
        required uint32 lineno       // the line number, 0 means open the file
	required string line         // one line of the FED file
}

message NM_Start_Registration_For_Object_Class : merge NetworkMessage {
	required ObjectClassHandle objectClass
}

message NM_Stop_Registration_For_Object_Class : merge NetworkMessage {
	required ObjectClassHandle objectClass
}

message NM_WithHandleArray : merge  NetworkMessage {
	repeated AttributeHandle handle 
}

message NM_DDM_Base : merge  NM_WithHandleArray {
	required bool DDM_bool { default=false }
}


native StreamType {
    language CXX [typedef Socket* StreamType;]
}

native NM_Type {
    language CXX [typedef NetworkMessage::Type NM_Type;]
}

factory NM_Factory {

	factoryCreator  NetworkMessage create(NM_Type)
	factoryReceiver NetworkMessage receive(StreamType)
}

