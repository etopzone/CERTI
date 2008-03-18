// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This program is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// ----------------------------------------------------------------------------

#include "NM_Classes.hh"

namespace certi {

NetworkMessage* NM_Factory::create(NetworkMessage::Message_T type) {
	NetworkMessage* msg;
	
	switch (type) {
	case NetworkMessage::NOT_USED:
	   msg = new NM_Not_Used(); 
	   break;
	case NetworkMessage::CLOSE_CONNEXION:
	   msg = new NM_Close_Connexion(); 
	   break;
	case NetworkMessage::MESSAGE_NULL:
	   msg = new NM_Message_Null(); 
	   break;
	case NetworkMessage::CREATE_FEDERATION_EXECUTION:
	   msg = new NM_Create_Federation_Execution(); 
	   break;
	case NetworkMessage::DESTROY_FEDERATION_EXECUTION:
	   msg = new NM_Destroy_Federation_Execution(); 
	   break;
	case NetworkMessage::JOIN_FEDERATION_EXECUTION:
	   msg = new NM_Join_Federation_Execution(); 
	   break;
	case NetworkMessage::RESIGN_FEDERATION_EXECUTION:
	   msg = new NM_Resign_Federation_Execution(); 
	   break;
	case NetworkMessage::SET_TIME_REGULATING:
	   msg = new NM_Set_Time_Regulating(); 
	   break;
	case NetworkMessage::SET_TIME_CONSTRAINED:
	   msg = new NM_Set_Time_Constrained(); 
	   break;
	case NetworkMessage::REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
	   msg = new NM_Register_Federation_Synchronization_Point(); 
	   break;
	case NetworkMessage::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
	   msg = new NM_Synchronization_Point_Registration_Succeeded(); 
	   break;
	case NetworkMessage::ANNOUNCE_SYNCHRONIZATION_POINT:
	   msg = new NM_Announce_Synchronization_Point(); 
	   break;
	case NetworkMessage::SYNCHRONIZATION_POINT_ACHIEVED:
	   msg = new NM_Synchronization_Point_Achieved(); 
	   break;
	case NetworkMessage::FEDERATION_SYNCHRONIZED:
	   msg = new NM_Federation_Synchronized(); 
	   break;
	case NetworkMessage::REQUEST_FEDERATION_SAVE:
	   msg = new NM_Request_Federation_Save(); 
	   break;
	case NetworkMessage::FEDERATE_SAVE_BEGUN:
	   msg = new NM_Federate_Save_Begun(); 
	   break;
	case NetworkMessage::FEDERATE_SAVE_COMPLETE:
	   msg = new NM_Federate_Save_Complete(); 
	   break;
	case NetworkMessage::FEDERATE_SAVE_NOT_COMPLETE:
	   msg = new NM_Federate_Save_Not_Complete(); 
	   break;
	case NetworkMessage::INITIATE_FEDERATE_SAVE:
	   msg = new NM_Initiate_Federate_Save(); 
	   break;
	case NetworkMessage::FEDERATION_SAVED:
	   msg = new NM_Federation_Saved(); 
	   break;
	case NetworkMessage::FEDERATION_NOT_SAVED:
	   msg = new NM_Federation_Not_Saved(); 
	   break;
	case NetworkMessage::REQUEST_FEDERATION_RESTORE:
	   msg = new NM_Request_Federation_Restore(); 
	   break;
	case NetworkMessage::FEDERATE_RESTORE_COMPLETE:
	   msg = new NM_Federate_Restore_Complete(); 
	   break;
	case NetworkMessage::FEDERATE_RESTORE_NOT_COMPLETE:
	   msg = new NM_Federate_Restore_Not_Complete(); 
	   break;
	case NetworkMessage::REQUEST_FEDERATION_RESTORE_SUCCEEDED:
	   msg = new NM_Request_Federation_Restore_Succeeded(); 
	   break;
	case NetworkMessage::REQUEST_FEDERATION_RESTORE_FAILED:
	   msg = new NM_Request_Federation_Restore_Failed(); 
	   break;
	case NetworkMessage::FEDERATION_RESTORE_BEGUN:
	   msg = new NM_Federation_Restore_Begun(); 
	   break;
	case NetworkMessage::INITIATE_FEDERATE_RESTORE:
	   msg = new NM_Initiate_Federate_Restore(); 
	   break;
	case NetworkMessage::FEDERATION_RESTORED:
	   msg = new NM_Federation_Restored(); 
	   break;
	case NetworkMessage::FEDERATION_NOT_RESTORED:
	   msg = new NM_Federation_Not_Restored(); 
	   break;
	case NetworkMessage::PUBLISH_OBJECT_CLASS:
	   msg = new NM_Publish_Object_Class(); 
	   break;
	case NetworkMessage::UNPUBLISH_OBJECT_CLASS:
	   msg = new NM_Unpublish_Object_Class(); 
	   break;
	case NetworkMessage::PUBLISH_INTERACTION_CLASS:
	   msg = new NM_Publish_Interaction_Class(); 
	   break;
	case NetworkMessage::UNPUBLISH_INTERACTION_CLASS:
	   msg = new NM_Unpublish_Interaction_Class(); 
	   break;
	case NetworkMessage::SUBSCRIBE_OBJECT_CLASS:
	   msg = new NM_Subscribe_Object_Class(); 
	   break;
	case NetworkMessage::UNSUBSCRIBE_OBJECT_CLASS:
	   msg = new NM_Unsubscribe_Object_Class(); 
	   break;
	case NetworkMessage::SUBSCRIBE_INTERACTION_CLASS:
	   msg = new NM_Subscribe_Interaction_Class(); 
	   break;
	case NetworkMessage::UNSUBSCRIBE_INTERACTION_CLASS:
	   msg = new NM_Unsubscribe_Interaction_Class(); 
	   break;
	case NetworkMessage::TURN_INTERACTIONS_ON:
	   msg = new NM_Turn_Interactions_On(); 
	   break;
	case NetworkMessage::TURN_INTERACTIONS_OFF:
	   msg = new NM_Turn_Interactions_Off(); 
	   break;
	case NetworkMessage::REGISTER_OBJECT:
	   msg = new NM_Register_Object(); 
	   break;
	case NetworkMessage::DISCOVER_OBJECT:
	   msg = new NM_Discover_Object(); 
	   break;
	case NetworkMessage::UPDATE_ATTRIBUTE_VALUES:
	   msg = new NM_Update_Attribute_Values(); 
	   break;
	case NetworkMessage::REFLECT_ATTRIBUTE_VALUES:
	   msg = new NM_Reflect_Attribute_Values(); 
	   break;
	case NetworkMessage::SEND_INTERACTION:
	   msg = new NM_Send_Interaction(); 
	   break;
	case NetworkMessage::RECEIVE_INTERACTION:
	   msg = new NM_Receive_Interaction(); 
	   break;
	case NetworkMessage::DELETE_OBJECT:
	   msg = new NM_Delete_Object(); 
	   break;
	case NetworkMessage::REMOVE_OBJECT:
	   msg = new NM_Remove_Object(); 
	   break;
	case NetworkMessage::CHANGE_ATTRIBUTE_TRANSPORT_TYPE:
	   msg = new NM_Change_Attribute_Transport_Type(); 
	   break;
	case NetworkMessage::CHANGE_ATTRIBUTE_ORDER_TYPE:
	   msg = new NM_Change_Attribute_Order_Type(); 
	   break;
	case NetworkMessage::CHANGE_INTERACTION_TRANSPORT_TYPE:
	   msg = new NM_Change_Interaction_Transport_Type(); 
	   break;
	case NetworkMessage::CHANGE_INTERACTION_ORDER_TYPE:
	   msg = new NM_Change_Interaction_Order_Type(); 
	   break;
	case NetworkMessage::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE:
	   msg = new NM_Request_Class_Attribute_Value_Update(); 
	   break;
	case NetworkMessage::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
	   msg = new NM_Request_Object_Attribute_Value_Update(); 
	   break;
	case NetworkMessage::IS_ATTRIBUTE_OWNED_BY_FEDERATE:
	   msg = new NM_Is_Attribute_Owned_By_Federate(); 
	   break;
	case NetworkMessage::QUERY_ATTRIBUTE_OWNERSHIP:
	   msg = new NM_Query_Attribute_Ownership(); 
	   break;
	case NetworkMessage::ATTRIBUTE_IS_NOT_OWNED:
	   msg = new NM_Attribute_Is_Not_Owned(); 
	   break;
	case NetworkMessage::INFORM_ATTRIBUTE_OWNERSHIP:
	   msg = new NM_Inform_Attribute_Ownership(); 
	   break;
	case NetworkMessage::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
	   msg = new NM_Negotiated_Attribute_Ownership_Divestiture(); 
	   break;
	case NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
	   msg = new NM_Attribute_Ownership_Acquisition_Notification(); 
	   break;
	case NetworkMessage::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
	   msg = new NM_Attribute_Ownership_Divestiture_Notification(); 
	   break;
	case NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
	   msg = new NM_Request_Attribute_Ownership_Assumption(); 
	   break;
	case NetworkMessage::ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
	   msg = new NM_Attribute_Ownership_Unavailable(); 
	   break;
	case NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
	   msg = new NM_Attribute_Ownership_Acquisition_If_Available(); 
	   break;
	case NetworkMessage::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
	   msg = new NM_Unconditional_Attribute_Ownership_Divestiture(); 
	   break;
	case NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION:
	   msg = new NM_Attribute_Ownership_Acquisition(); 
	   break;
	case NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
	   msg = new NM_Request_Attribute_Ownership_Release(); 
	   break;
	case NetworkMessage::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
	   msg = new NM_Cancel_Negotiated_Attribute_Ownership_Divestiture(); 
	   break;
	case NetworkMessage::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
	   msg = new NM_Attribute_Ownership_Release_Response(); 
	   break;
	case NetworkMessage::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
	   msg = new NM_Cancel_Attribute_Ownership_Acquisition(); 
	   break;
	case NetworkMessage::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
	   msg = new NM_Confirm_Attribute_Ownership_Acquisition_Cancellation(); 
	   break;
	case NetworkMessage::DDM_CREATE_REGION:
	   msg = new NM_Ddm_Create_Region(); 
	   break;
	case NetworkMessage::DDM_MODIFY_REGION:
	   msg = new NM_Ddm_Modify_Region(); 
	   break;
	case NetworkMessage::DDM_DELETE_REGION:
	   msg = new NM_Ddm_Delete_Region(); 
	   break;
	case NetworkMessage::DDM_ASSOCIATE_REGION:
	   msg = new NM_Ddm_Associate_Region(); 
	   break;
	case NetworkMessage::DDM_REGISTER_OBJECT:
	   msg = new NM_Ddm_Register_Object(); 
	   break;
	case NetworkMessage::DDM_UNASSOCIATE_REGION:
	   msg = new NM_Ddm_Unassociate_Region(); 
	   break;
	case NetworkMessage::DDM_SUBSCRIBE_ATTRIBUTES:
	   msg = new NM_Ddm_Subscribe_Attributes(); 
	   break;
	case NetworkMessage::DDM_UNSUBSCRIBE_ATTRIBUTES:
	   msg = new NM_Ddm_Unsubscribe_Attributes(); 
	   break;
	case NetworkMessage::DDM_SUBSCRIBE_INTERACTION:
	   msg = new NM_Ddm_Subscribe_Interaction(); 
	   break;
	case NetworkMessage::DDM_UNSUBSCRIBE_INTERACTION:
	   msg = new NM_Ddm_Unsubscribe_Interaction(); 
	   break;
	case NetworkMessage::PROVIDE_ATTRIBUTE_VALUE_UPDATE:
	   msg = new NM_Provide_Attribute_Value_Update(); 
	   break;
	case NetworkMessage::GET_FED_FILE:
	   msg = new NM_Get_Fed_File(); 
	   break;
	case NetworkMessage::LAST:
	   msg = new NM_Last(); 
	   break;

	default:
		throw RTIinternalError("Unknown/Unimplemented message Type");
	}
} /* end of NM_Factory::create */

/*<BEGIN>---------- Not_Used ------------<BEGIN>*/
NM_Not_Used::NM_Not_Used() {
    this->type = NetworkMessage::NOT_USED;
    this->name = "NOT_USED";
}
NM_Not_Used::~NM_Not_Used() {
}
void NM_Not_Used::serialize() {
}
void NM_Not_Used::deserialize() {
}
/*<END>---------- Not_Used ------------<END>*/

/*<BEGIN>---------- Close_Connexion ------------<BEGIN>*/
NM_Close_Connexion::NM_Close_Connexion() {
    this->type = NetworkMessage::CLOSE_CONNEXION;
    this->name = "CLOSE_CONNEXION";
}
NM_Close_Connexion::~NM_Close_Connexion() {
}
void NM_Close_Connexion::serialize() {
}
void NM_Close_Connexion::deserialize() {
}
/*<END>---------- Close_Connexion ------------<END>*/

/*<BEGIN>---------- Message_Null ------------<BEGIN>*/
NM_Message_Null::NM_Message_Null() {
    this->type = NetworkMessage::MESSAGE_NULL;
    this->name = "MESSAGE_NULL";
}
NM_Message_Null::~NM_Message_Null() {
}
void NM_Message_Null::serialize() {
}
void NM_Message_Null::deserialize() {
}
/*<END>---------- Message_Null ------------<END>*/

/*<BEGIN>---------- Create_Federation_Execution ------------<BEGIN>*/
NM_Create_Federation_Execution::NM_Create_Federation_Execution() {
    this->type = NetworkMessage::CREATE_FEDERATION_EXECUTION;
    this->name = "CREATE_FEDERATION_EXECUTION";
}
NM_Create_Federation_Execution::~NM_Create_Federation_Execution() {
}
void NM_Create_Federation_Execution::serialize() {
}
void NM_Create_Federation_Execution::deserialize() {
}
/*<END>---------- Create_Federation_Execution ------------<END>*/

/*<BEGIN>---------- Destroy_Federation_Execution ------------<BEGIN>*/
NM_Destroy_Federation_Execution::NM_Destroy_Federation_Execution() {
    this->type = NetworkMessage::DESTROY_FEDERATION_EXECUTION;
    this->name = "DESTROY_FEDERATION_EXECUTION";
}
NM_Destroy_Federation_Execution::~NM_Destroy_Federation_Execution() {
}
void NM_Destroy_Federation_Execution::serialize() {
}
void NM_Destroy_Federation_Execution::deserialize() {
}
/*<END>---------- Destroy_Federation_Execution ------------<END>*/

/*<BEGIN>---------- Join_Federation_Execution ------------<BEGIN>*/
NM_Join_Federation_Execution::NM_Join_Federation_Execution() {
    this->type = NetworkMessage::JOIN_FEDERATION_EXECUTION;
    this->name = "JOIN_FEDERATION_EXECUTION";
}
NM_Join_Federation_Execution::~NM_Join_Federation_Execution() {
}
void NM_Join_Federation_Execution::serialize() {
}
void NM_Join_Federation_Execution::deserialize() {
}
/*<END>---------- Join_Federation_Execution ------------<END>*/

/*<BEGIN>---------- Resign_Federation_Execution ------------<BEGIN>*/
NM_Resign_Federation_Execution::NM_Resign_Federation_Execution() {
    this->type = NetworkMessage::RESIGN_FEDERATION_EXECUTION;
    this->name = "RESIGN_FEDERATION_EXECUTION";
}
NM_Resign_Federation_Execution::~NM_Resign_Federation_Execution() {
}
void NM_Resign_Federation_Execution::serialize() {
}
void NM_Resign_Federation_Execution::deserialize() {
}
/*<END>---------- Resign_Federation_Execution ------------<END>*/

/*<BEGIN>---------- Set_Time_Regulating ------------<BEGIN>*/
NM_Set_Time_Regulating::NM_Set_Time_Regulating() {
    this->type = NetworkMessage::SET_TIME_REGULATING;
    this->name = "SET_TIME_REGULATING";
}
NM_Set_Time_Regulating::~NM_Set_Time_Regulating() {
}
void NM_Set_Time_Regulating::serialize() {
}
void NM_Set_Time_Regulating::deserialize() {
}
/*<END>---------- Set_Time_Regulating ------------<END>*/

/*<BEGIN>---------- Set_Time_Constrained ------------<BEGIN>*/
NM_Set_Time_Constrained::NM_Set_Time_Constrained() {
    this->type = NetworkMessage::SET_TIME_CONSTRAINED;
    this->name = "SET_TIME_CONSTRAINED";
}
NM_Set_Time_Constrained::~NM_Set_Time_Constrained() {
}
void NM_Set_Time_Constrained::serialize() {
}
void NM_Set_Time_Constrained::deserialize() {
}
/*<END>---------- Set_Time_Constrained ------------<END>*/

/*<BEGIN>---------- Register_Federation_Synchronization_Point ------------<BEGIN>*/
NM_Register_Federation_Synchronization_Point::NM_Register_Federation_Synchronization_Point() {
    this->type = NetworkMessage::REGISTER_FEDERATION_SYNCHRONIZATION_POINT;
    this->name = "REGISTER_FEDERATION_SYNCHRONIZATION_POINT";
}
NM_Register_Federation_Synchronization_Point::~NM_Register_Federation_Synchronization_Point() {
}
void NM_Register_Federation_Synchronization_Point::serialize() {
}
void NM_Register_Federation_Synchronization_Point::deserialize() {
}
/*<END>---------- Register_Federation_Synchronization_Point ------------<END>*/

/*<BEGIN>---------- Synchronization_Point_Registration_Succeeded ------------<BEGIN>*/
NM_Synchronization_Point_Registration_Succeeded::NM_Synchronization_Point_Registration_Succeeded() {
    this->type = NetworkMessage::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED;
    this->name = "SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED";
}
NM_Synchronization_Point_Registration_Succeeded::~NM_Synchronization_Point_Registration_Succeeded() {
}
void NM_Synchronization_Point_Registration_Succeeded::serialize() {
}
void NM_Synchronization_Point_Registration_Succeeded::deserialize() {
}
/*<END>---------- Synchronization_Point_Registration_Succeeded ------------<END>*/

/*<BEGIN>---------- Announce_Synchronization_Point ------------<BEGIN>*/
NM_Announce_Synchronization_Point::NM_Announce_Synchronization_Point() {
    this->type = NetworkMessage::ANNOUNCE_SYNCHRONIZATION_POINT;
    this->name = "ANNOUNCE_SYNCHRONIZATION_POINT";
}
NM_Announce_Synchronization_Point::~NM_Announce_Synchronization_Point() {
}
void NM_Announce_Synchronization_Point::serialize() {
}
void NM_Announce_Synchronization_Point::deserialize() {
}
/*<END>---------- Announce_Synchronization_Point ------------<END>*/

/*<BEGIN>---------- Synchronization_Point_Achieved ------------<BEGIN>*/
NM_Synchronization_Point_Achieved::NM_Synchronization_Point_Achieved() {
    this->type = NetworkMessage::SYNCHRONIZATION_POINT_ACHIEVED;
    this->name = "SYNCHRONIZATION_POINT_ACHIEVED";
}
NM_Synchronization_Point_Achieved::~NM_Synchronization_Point_Achieved() {
}
void NM_Synchronization_Point_Achieved::serialize() {
}
void NM_Synchronization_Point_Achieved::deserialize() {
}
/*<END>---------- Synchronization_Point_Achieved ------------<END>*/

/*<BEGIN>---------- Federation_Synchronized ------------<BEGIN>*/
NM_Federation_Synchronized::NM_Federation_Synchronized() {
    this->type = NetworkMessage::FEDERATION_SYNCHRONIZED;
    this->name = "FEDERATION_SYNCHRONIZED";
}
NM_Federation_Synchronized::~NM_Federation_Synchronized() {
}
void NM_Federation_Synchronized::serialize() {
}
void NM_Federation_Synchronized::deserialize() {
}
/*<END>---------- Federation_Synchronized ------------<END>*/

/*<BEGIN>---------- Request_Federation_Save ------------<BEGIN>*/
NM_Request_Federation_Save::NM_Request_Federation_Save() {
    this->type = NetworkMessage::REQUEST_FEDERATION_SAVE;
    this->name = "REQUEST_FEDERATION_SAVE";
}
NM_Request_Federation_Save::~NM_Request_Federation_Save() {
}
void NM_Request_Federation_Save::serialize() {
}
void NM_Request_Federation_Save::deserialize() {
}
/*<END>---------- Request_Federation_Save ------------<END>*/

/*<BEGIN>---------- Federate_Save_Begun ------------<BEGIN>*/
NM_Federate_Save_Begun::NM_Federate_Save_Begun() {
    this->type = NetworkMessage::FEDERATE_SAVE_BEGUN;
    this->name = "FEDERATE_SAVE_BEGUN";
}
NM_Federate_Save_Begun::~NM_Federate_Save_Begun() {
}
void NM_Federate_Save_Begun::serialize() {
}
void NM_Federate_Save_Begun::deserialize() {
}
/*<END>---------- Federate_Save_Begun ------------<END>*/

/*<BEGIN>---------- Federate_Save_Complete ------------<BEGIN>*/
NM_Federate_Save_Complete::NM_Federate_Save_Complete() {
    this->type = NetworkMessage::FEDERATE_SAVE_COMPLETE;
    this->name = "FEDERATE_SAVE_COMPLETE";
}
NM_Federate_Save_Complete::~NM_Federate_Save_Complete() {
}
void NM_Federate_Save_Complete::serialize() {
}
void NM_Federate_Save_Complete::deserialize() {
}
/*<END>---------- Federate_Save_Complete ------------<END>*/

/*<BEGIN>---------- Federate_Save_Not_Complete ------------<BEGIN>*/
NM_Federate_Save_Not_Complete::NM_Federate_Save_Not_Complete() {
    this->type = NetworkMessage::FEDERATE_SAVE_NOT_COMPLETE;
    this->name = "FEDERATE_SAVE_NOT_COMPLETE";
}
NM_Federate_Save_Not_Complete::~NM_Federate_Save_Not_Complete() {
}
void NM_Federate_Save_Not_Complete::serialize() {
}
void NM_Federate_Save_Not_Complete::deserialize() {
}
/*<END>---------- Federate_Save_Not_Complete ------------<END>*/

/*<BEGIN>---------- Initiate_Federate_Save ------------<BEGIN>*/
NM_Initiate_Federate_Save::NM_Initiate_Federate_Save() {
    this->type = NetworkMessage::INITIATE_FEDERATE_SAVE;
    this->name = "INITIATE_FEDERATE_SAVE";
}
NM_Initiate_Federate_Save::~NM_Initiate_Federate_Save() {
}
void NM_Initiate_Federate_Save::serialize() {
}
void NM_Initiate_Federate_Save::deserialize() {
}
/*<END>---------- Initiate_Federate_Save ------------<END>*/

/*<BEGIN>---------- Federation_Saved ------------<BEGIN>*/
NM_Federation_Saved::NM_Federation_Saved() {
    this->type = NetworkMessage::FEDERATION_SAVED;
    this->name = "FEDERATION_SAVED";
}
NM_Federation_Saved::~NM_Federation_Saved() {
}
void NM_Federation_Saved::serialize() {
}
void NM_Federation_Saved::deserialize() {
}
/*<END>---------- Federation_Saved ------------<END>*/

/*<BEGIN>---------- Federation_Not_Saved ------------<BEGIN>*/
NM_Federation_Not_Saved::NM_Federation_Not_Saved() {
    this->type = NetworkMessage::FEDERATION_NOT_SAVED;
    this->name = "FEDERATION_NOT_SAVED";
}
NM_Federation_Not_Saved::~NM_Federation_Not_Saved() {
}
void NM_Federation_Not_Saved::serialize() {
}
void NM_Federation_Not_Saved::deserialize() {
}
/*<END>---------- Federation_Not_Saved ------------<END>*/

/*<BEGIN>---------- Request_Federation_Restore ------------<BEGIN>*/
NM_Request_Federation_Restore::NM_Request_Federation_Restore() {
    this->type = NetworkMessage::REQUEST_FEDERATION_RESTORE;
    this->name = "REQUEST_FEDERATION_RESTORE";
}
NM_Request_Federation_Restore::~NM_Request_Federation_Restore() {
}
void NM_Request_Federation_Restore::serialize() {
}
void NM_Request_Federation_Restore::deserialize() {
}
/*<END>---------- Request_Federation_Restore ------------<END>*/

/*<BEGIN>---------- Federate_Restore_Complete ------------<BEGIN>*/
NM_Federate_Restore_Complete::NM_Federate_Restore_Complete() {
    this->type = NetworkMessage::FEDERATE_RESTORE_COMPLETE;
    this->name = "FEDERATE_RESTORE_COMPLETE";
}
NM_Federate_Restore_Complete::~NM_Federate_Restore_Complete() {
}
void NM_Federate_Restore_Complete::serialize() {
}
void NM_Federate_Restore_Complete::deserialize() {
}
/*<END>---------- Federate_Restore_Complete ------------<END>*/

/*<BEGIN>---------- Federate_Restore_Not_Complete ------------<BEGIN>*/
NM_Federate_Restore_Not_Complete::NM_Federate_Restore_Not_Complete() {
    this->type = NetworkMessage::FEDERATE_RESTORE_NOT_COMPLETE;
    this->name = "FEDERATE_RESTORE_NOT_COMPLETE";
}
NM_Federate_Restore_Not_Complete::~NM_Federate_Restore_Not_Complete() {
}
void NM_Federate_Restore_Not_Complete::serialize() {
}
void NM_Federate_Restore_Not_Complete::deserialize() {
}
/*<END>---------- Federate_Restore_Not_Complete ------------<END>*/

/*<BEGIN>---------- Request_Federation_Restore_Succeeded ------------<BEGIN>*/
NM_Request_Federation_Restore_Succeeded::NM_Request_Federation_Restore_Succeeded() {
    this->type = NetworkMessage::REQUEST_FEDERATION_RESTORE_SUCCEEDED;
    this->name = "REQUEST_FEDERATION_RESTORE_SUCCEEDED";
}
NM_Request_Federation_Restore_Succeeded::~NM_Request_Federation_Restore_Succeeded() {
}
void NM_Request_Federation_Restore_Succeeded::serialize() {
}
void NM_Request_Federation_Restore_Succeeded::deserialize() {
}
/*<END>---------- Request_Federation_Restore_Succeeded ------------<END>*/

/*<BEGIN>---------- Request_Federation_Restore_Failed ------------<BEGIN>*/
NM_Request_Federation_Restore_Failed::NM_Request_Federation_Restore_Failed() {
    this->type = NetworkMessage::REQUEST_FEDERATION_RESTORE_FAILED;
    this->name = "REQUEST_FEDERATION_RESTORE_FAILED";
}
NM_Request_Federation_Restore_Failed::~NM_Request_Federation_Restore_Failed() {
}
void NM_Request_Federation_Restore_Failed::serialize() {
}
void NM_Request_Federation_Restore_Failed::deserialize() {
}
/*<END>---------- Request_Federation_Restore_Failed ------------<END>*/

/*<BEGIN>---------- Federation_Restore_Begun ------------<BEGIN>*/
NM_Federation_Restore_Begun::NM_Federation_Restore_Begun() {
    this->type = NetworkMessage::FEDERATION_RESTORE_BEGUN;
    this->name = "FEDERATION_RESTORE_BEGUN";
}
NM_Federation_Restore_Begun::~NM_Federation_Restore_Begun() {
}
void NM_Federation_Restore_Begun::serialize() {
}
void NM_Federation_Restore_Begun::deserialize() {
}
/*<END>---------- Federation_Restore_Begun ------------<END>*/

/*<BEGIN>---------- Initiate_Federate_Restore ------------<BEGIN>*/
NM_Initiate_Federate_Restore::NM_Initiate_Federate_Restore() {
    this->type = NetworkMessage::INITIATE_FEDERATE_RESTORE;
    this->name = "INITIATE_FEDERATE_RESTORE";
}
NM_Initiate_Federate_Restore::~NM_Initiate_Federate_Restore() {
}
void NM_Initiate_Federate_Restore::serialize() {
}
void NM_Initiate_Federate_Restore::deserialize() {
}
/*<END>---------- Initiate_Federate_Restore ------------<END>*/

/*<BEGIN>---------- Federation_Restored ------------<BEGIN>*/
NM_Federation_Restored::NM_Federation_Restored() {
    this->type = NetworkMessage::FEDERATION_RESTORED;
    this->name = "FEDERATION_RESTORED";
}
NM_Federation_Restored::~NM_Federation_Restored() {
}
void NM_Federation_Restored::serialize() {
}
void NM_Federation_Restored::deserialize() {
}
/*<END>---------- Federation_Restored ------------<END>*/

/*<BEGIN>---------- Federation_Not_Restored ------------<BEGIN>*/
NM_Federation_Not_Restored::NM_Federation_Not_Restored() {
    this->type = NetworkMessage::FEDERATION_NOT_RESTORED;
    this->name = "FEDERATION_NOT_RESTORED";
}
NM_Federation_Not_Restored::~NM_Federation_Not_Restored() {
}
void NM_Federation_Not_Restored::serialize() {
}
void NM_Federation_Not_Restored::deserialize() {
}
/*<END>---------- Federation_Not_Restored ------------<END>*/

/*<BEGIN>---------- Publish_Object_Class ------------<BEGIN>*/
NM_Publish_Object_Class::NM_Publish_Object_Class() {
    this->type = NetworkMessage::PUBLISH_OBJECT_CLASS;
    this->name = "PUBLISH_OBJECT_CLASS";
}
NM_Publish_Object_Class::~NM_Publish_Object_Class() {
}
void NM_Publish_Object_Class::serialize() {
}
void NM_Publish_Object_Class::deserialize() {
}
/*<END>---------- Publish_Object_Class ------------<END>*/

/*<BEGIN>---------- Unpublish_Object_Class ------------<BEGIN>*/
NM_Unpublish_Object_Class::NM_Unpublish_Object_Class() {
    this->type = NetworkMessage::UNPUBLISH_OBJECT_CLASS;
    this->name = "UNPUBLISH_OBJECT_CLASS";
}
NM_Unpublish_Object_Class::~NM_Unpublish_Object_Class() {
}
void NM_Unpublish_Object_Class::serialize() {
}
void NM_Unpublish_Object_Class::deserialize() {
}
/*<END>---------- Unpublish_Object_Class ------------<END>*/

/*<BEGIN>---------- Publish_Interaction_Class ------------<BEGIN>*/
NM_Publish_Interaction_Class::NM_Publish_Interaction_Class() {
    this->type = NetworkMessage::PUBLISH_INTERACTION_CLASS;
    this->name = "PUBLISH_INTERACTION_CLASS";
}
NM_Publish_Interaction_Class::~NM_Publish_Interaction_Class() {
}
void NM_Publish_Interaction_Class::serialize() {
}
void NM_Publish_Interaction_Class::deserialize() {
}
/*<END>---------- Publish_Interaction_Class ------------<END>*/

/*<BEGIN>---------- Unpublish_Interaction_Class ------------<BEGIN>*/
NM_Unpublish_Interaction_Class::NM_Unpublish_Interaction_Class() {
    this->type = NetworkMessage::UNPUBLISH_INTERACTION_CLASS;
    this->name = "UNPUBLISH_INTERACTION_CLASS";
}
NM_Unpublish_Interaction_Class::~NM_Unpublish_Interaction_Class() {
}
void NM_Unpublish_Interaction_Class::serialize() {
}
void NM_Unpublish_Interaction_Class::deserialize() {
}
/*<END>---------- Unpublish_Interaction_Class ------------<END>*/

/*<BEGIN>---------- Subscribe_Object_Class ------------<BEGIN>*/
NM_Subscribe_Object_Class::NM_Subscribe_Object_Class() {
    this->type = NetworkMessage::SUBSCRIBE_OBJECT_CLASS;
    this->name = "SUBSCRIBE_OBJECT_CLASS";
}
NM_Subscribe_Object_Class::~NM_Subscribe_Object_Class() {
}
void NM_Subscribe_Object_Class::serialize() {
}
void NM_Subscribe_Object_Class::deserialize() {
}
/*<END>---------- Subscribe_Object_Class ------------<END>*/

/*<BEGIN>---------- Unsubscribe_Object_Class ------------<BEGIN>*/
NM_Unsubscribe_Object_Class::NM_Unsubscribe_Object_Class() {
    this->type = NetworkMessage::UNSUBSCRIBE_OBJECT_CLASS;
    this->name = "UNSUBSCRIBE_OBJECT_CLASS";
}
NM_Unsubscribe_Object_Class::~NM_Unsubscribe_Object_Class() {
}
void NM_Unsubscribe_Object_Class::serialize() {
}
void NM_Unsubscribe_Object_Class::deserialize() {
}
/*<END>---------- Unsubscribe_Object_Class ------------<END>*/

/*<BEGIN>---------- Subscribe_Interaction_Class ------------<BEGIN>*/
NM_Subscribe_Interaction_Class::NM_Subscribe_Interaction_Class() {
    this->type = NetworkMessage::SUBSCRIBE_INTERACTION_CLASS;
    this->name = "SUBSCRIBE_INTERACTION_CLASS";
}
NM_Subscribe_Interaction_Class::~NM_Subscribe_Interaction_Class() {
}
void NM_Subscribe_Interaction_Class::serialize() {
}
void NM_Subscribe_Interaction_Class::deserialize() {
}
/*<END>---------- Subscribe_Interaction_Class ------------<END>*/

/*<BEGIN>---------- Unsubscribe_Interaction_Class ------------<BEGIN>*/
NM_Unsubscribe_Interaction_Class::NM_Unsubscribe_Interaction_Class() {
    this->type = NetworkMessage::UNSUBSCRIBE_INTERACTION_CLASS;
    this->name = "UNSUBSCRIBE_INTERACTION_CLASS";
}
NM_Unsubscribe_Interaction_Class::~NM_Unsubscribe_Interaction_Class() {
}
void NM_Unsubscribe_Interaction_Class::serialize() {
}
void NM_Unsubscribe_Interaction_Class::deserialize() {
}
/*<END>---------- Unsubscribe_Interaction_Class ------------<END>*/

/*<BEGIN>---------- Turn_Interactions_On ------------<BEGIN>*/
NM_Turn_Interactions_On::NM_Turn_Interactions_On() {
    this->type = NetworkMessage::TURN_INTERACTIONS_ON;
    this->name = "TURN_INTERACTIONS_ON";
}
NM_Turn_Interactions_On::~NM_Turn_Interactions_On() {
}
void NM_Turn_Interactions_On::serialize() {
}
void NM_Turn_Interactions_On::deserialize() {
}
/*<END>---------- Turn_Interactions_On ------------<END>*/

/*<BEGIN>---------- Turn_Interactions_Off ------------<BEGIN>*/
NM_Turn_Interactions_Off::NM_Turn_Interactions_Off() {
    this->type = NetworkMessage::TURN_INTERACTIONS_OFF;
    this->name = "TURN_INTERACTIONS_OFF";
}
NM_Turn_Interactions_Off::~NM_Turn_Interactions_Off() {
}
void NM_Turn_Interactions_Off::serialize() {
}
void NM_Turn_Interactions_Off::deserialize() {
}
/*<END>---------- Turn_Interactions_Off ------------<END>*/

/*<BEGIN>---------- Register_Object ------------<BEGIN>*/
NM_Register_Object::NM_Register_Object() {
    this->type = NetworkMessage::REGISTER_OBJECT;
    this->name = "REGISTER_OBJECT";
}
NM_Register_Object::~NM_Register_Object() {
}
void NM_Register_Object::serialize() {
}
void NM_Register_Object::deserialize() {
}
/*<END>---------- Register_Object ------------<END>*/

/*<BEGIN>---------- Discover_Object ------------<BEGIN>*/
NM_Discover_Object::NM_Discover_Object() {
    this->type = NetworkMessage::DISCOVER_OBJECT;
    this->name = "DISCOVER_OBJECT";
}
NM_Discover_Object::~NM_Discover_Object() {
}
void NM_Discover_Object::serialize() {
}
void NM_Discover_Object::deserialize() {
}
/*<END>---------- Discover_Object ------------<END>*/

/*<BEGIN>---------- Update_Attribute_Values ------------<BEGIN>*/
NM_Update_Attribute_Values::NM_Update_Attribute_Values() {
    this->type = NetworkMessage::UPDATE_ATTRIBUTE_VALUES;
    this->name = "UPDATE_ATTRIBUTE_VALUES";
}
NM_Update_Attribute_Values::~NM_Update_Attribute_Values() {
}
void NM_Update_Attribute_Values::serialize() {
}
void NM_Update_Attribute_Values::deserialize() {
}
/*<END>---------- Update_Attribute_Values ------------<END>*/

/*<BEGIN>---------- Reflect_Attribute_Values ------------<BEGIN>*/
NM_Reflect_Attribute_Values::NM_Reflect_Attribute_Values() {
    this->type = NetworkMessage::REFLECT_ATTRIBUTE_VALUES;
    this->name = "REFLECT_ATTRIBUTE_VALUES";
}
NM_Reflect_Attribute_Values::~NM_Reflect_Attribute_Values() {
}
void NM_Reflect_Attribute_Values::serialize() {
}
void NM_Reflect_Attribute_Values::deserialize() {
}
/*<END>---------- Reflect_Attribute_Values ------------<END>*/

/*<BEGIN>---------- Send_Interaction ------------<BEGIN>*/
NM_Send_Interaction::NM_Send_Interaction() {
    this->type = NetworkMessage::SEND_INTERACTION;
    this->name = "SEND_INTERACTION";
}
NM_Send_Interaction::~NM_Send_Interaction() {
}
void NM_Send_Interaction::serialize() {
}
void NM_Send_Interaction::deserialize() {
}
/*<END>---------- Send_Interaction ------------<END>*/

/*<BEGIN>---------- Receive_Interaction ------------<BEGIN>*/
NM_Receive_Interaction::NM_Receive_Interaction() {
    this->type = NetworkMessage::RECEIVE_INTERACTION;
    this->name = "RECEIVE_INTERACTION";
}
NM_Receive_Interaction::~NM_Receive_Interaction() {
}
void NM_Receive_Interaction::serialize() {
}
void NM_Receive_Interaction::deserialize() {
}
/*<END>---------- Receive_Interaction ------------<END>*/

/*<BEGIN>---------- Delete_Object ------------<BEGIN>*/
NM_Delete_Object::NM_Delete_Object() {
    this->type = NetworkMessage::DELETE_OBJECT;
    this->name = "DELETE_OBJECT";
}
NM_Delete_Object::~NM_Delete_Object() {
}
void NM_Delete_Object::serialize() {
}
void NM_Delete_Object::deserialize() {
}
/*<END>---------- Delete_Object ------------<END>*/

/*<BEGIN>---------- Remove_Object ------------<BEGIN>*/
NM_Remove_Object::NM_Remove_Object() {
    this->type = NetworkMessage::REMOVE_OBJECT;
    this->name = "REMOVE_OBJECT";
}
NM_Remove_Object::~NM_Remove_Object() {
}
void NM_Remove_Object::serialize() {
}
void NM_Remove_Object::deserialize() {
}
/*<END>---------- Remove_Object ------------<END>*/

/*<BEGIN>---------- Change_Attribute_Transport_Type ------------<BEGIN>*/
NM_Change_Attribute_Transport_Type::NM_Change_Attribute_Transport_Type() {
    this->type = NetworkMessage::CHANGE_ATTRIBUTE_TRANSPORT_TYPE;
    this->name = "CHANGE_ATTRIBUTE_TRANSPORT_TYPE";
}
NM_Change_Attribute_Transport_Type::~NM_Change_Attribute_Transport_Type() {
}
void NM_Change_Attribute_Transport_Type::serialize() {
}
void NM_Change_Attribute_Transport_Type::deserialize() {
}
/*<END>---------- Change_Attribute_Transport_Type ------------<END>*/

/*<BEGIN>---------- Change_Attribute_Order_Type ------------<BEGIN>*/
NM_Change_Attribute_Order_Type::NM_Change_Attribute_Order_Type() {
    this->type = NetworkMessage::CHANGE_ATTRIBUTE_ORDER_TYPE;
    this->name = "CHANGE_ATTRIBUTE_ORDER_TYPE";
}
NM_Change_Attribute_Order_Type::~NM_Change_Attribute_Order_Type() {
}
void NM_Change_Attribute_Order_Type::serialize() {
}
void NM_Change_Attribute_Order_Type::deserialize() {
}
/*<END>---------- Change_Attribute_Order_Type ------------<END>*/

/*<BEGIN>---------- Change_Interaction_Transport_Type ------------<BEGIN>*/
NM_Change_Interaction_Transport_Type::NM_Change_Interaction_Transport_Type() {
    this->type = NetworkMessage::CHANGE_INTERACTION_TRANSPORT_TYPE;
    this->name = "CHANGE_INTERACTION_TRANSPORT_TYPE";
}
NM_Change_Interaction_Transport_Type::~NM_Change_Interaction_Transport_Type() {
}
void NM_Change_Interaction_Transport_Type::serialize() {
}
void NM_Change_Interaction_Transport_Type::deserialize() {
}
/*<END>---------- Change_Interaction_Transport_Type ------------<END>*/

/*<BEGIN>---------- Change_Interaction_Order_Type ------------<BEGIN>*/
NM_Change_Interaction_Order_Type::NM_Change_Interaction_Order_Type() {
    this->type = NetworkMessage::CHANGE_INTERACTION_ORDER_TYPE;
    this->name = "CHANGE_INTERACTION_ORDER_TYPE";
}
NM_Change_Interaction_Order_Type::~NM_Change_Interaction_Order_Type() {
}
void NM_Change_Interaction_Order_Type::serialize() {
}
void NM_Change_Interaction_Order_Type::deserialize() {
}
/*<END>---------- Change_Interaction_Order_Type ------------<END>*/

/*<BEGIN>---------- Request_Class_Attribute_Value_Update ------------<BEGIN>*/
NM_Request_Class_Attribute_Value_Update::NM_Request_Class_Attribute_Value_Update() {
    this->type = NetworkMessage::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE;
    this->name = "REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE";
}
NM_Request_Class_Attribute_Value_Update::~NM_Request_Class_Attribute_Value_Update() {
}
void NM_Request_Class_Attribute_Value_Update::serialize() {
}
void NM_Request_Class_Attribute_Value_Update::deserialize() {
}
/*<END>---------- Request_Class_Attribute_Value_Update ------------<END>*/

/*<BEGIN>---------- Request_Object_Attribute_Value_Update ------------<BEGIN>*/
NM_Request_Object_Attribute_Value_Update::NM_Request_Object_Attribute_Value_Update() {
    this->type = NetworkMessage::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE;
    this->name = "REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE";
}
NM_Request_Object_Attribute_Value_Update::~NM_Request_Object_Attribute_Value_Update() {
}
void NM_Request_Object_Attribute_Value_Update::serialize() {
}
void NM_Request_Object_Attribute_Value_Update::deserialize() {
}
/*<END>---------- Request_Object_Attribute_Value_Update ------------<END>*/

/*<BEGIN>---------- Is_Attribute_Owned_By_Federate ------------<BEGIN>*/
NM_Is_Attribute_Owned_By_Federate::NM_Is_Attribute_Owned_By_Federate() {
    this->type = NetworkMessage::IS_ATTRIBUTE_OWNED_BY_FEDERATE;
    this->name = "IS_ATTRIBUTE_OWNED_BY_FEDERATE";
}
NM_Is_Attribute_Owned_By_Federate::~NM_Is_Attribute_Owned_By_Federate() {
}
void NM_Is_Attribute_Owned_By_Federate::serialize() {
}
void NM_Is_Attribute_Owned_By_Federate::deserialize() {
}
/*<END>---------- Is_Attribute_Owned_By_Federate ------------<END>*/

/*<BEGIN>---------- Query_Attribute_Ownership ------------<BEGIN>*/
NM_Query_Attribute_Ownership::NM_Query_Attribute_Ownership() {
    this->type = NetworkMessage::QUERY_ATTRIBUTE_OWNERSHIP;
    this->name = "QUERY_ATTRIBUTE_OWNERSHIP";
}
NM_Query_Attribute_Ownership::~NM_Query_Attribute_Ownership() {
}
void NM_Query_Attribute_Ownership::serialize() {
}
void NM_Query_Attribute_Ownership::deserialize() {
}
/*<END>---------- Query_Attribute_Ownership ------------<END>*/

/*<BEGIN>---------- Attribute_Is_Not_Owned ------------<BEGIN>*/
NM_Attribute_Is_Not_Owned::NM_Attribute_Is_Not_Owned() {
    this->type = NetworkMessage::ATTRIBUTE_IS_NOT_OWNED;
    this->name = "ATTRIBUTE_IS_NOT_OWNED";
}
NM_Attribute_Is_Not_Owned::~NM_Attribute_Is_Not_Owned() {
}
void NM_Attribute_Is_Not_Owned::serialize() {
}
void NM_Attribute_Is_Not_Owned::deserialize() {
}
/*<END>---------- Attribute_Is_Not_Owned ------------<END>*/

/*<BEGIN>---------- Inform_Attribute_Ownership ------------<BEGIN>*/
NM_Inform_Attribute_Ownership::NM_Inform_Attribute_Ownership() {
    this->type = NetworkMessage::INFORM_ATTRIBUTE_OWNERSHIP;
    this->name = "INFORM_ATTRIBUTE_OWNERSHIP";
}
NM_Inform_Attribute_Ownership::~NM_Inform_Attribute_Ownership() {
}
void NM_Inform_Attribute_Ownership::serialize() {
}
void NM_Inform_Attribute_Ownership::deserialize() {
}
/*<END>---------- Inform_Attribute_Ownership ------------<END>*/

/*<BEGIN>---------- Negotiated_Attribute_Ownership_Divestiture ------------<BEGIN>*/
NM_Negotiated_Attribute_Ownership_Divestiture::NM_Negotiated_Attribute_Ownership_Divestiture() {
    this->type = NetworkMessage::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
    this->name = "NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE";
}
NM_Negotiated_Attribute_Ownership_Divestiture::~NM_Negotiated_Attribute_Ownership_Divestiture() {
}
void NM_Negotiated_Attribute_Ownership_Divestiture::serialize() {
}
void NM_Negotiated_Attribute_Ownership_Divestiture::deserialize() {
}
/*<END>---------- Negotiated_Attribute_Ownership_Divestiture ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Acquisition_Notification ------------<BEGIN>*/
NM_Attribute_Ownership_Acquisition_Notification::NM_Attribute_Ownership_Acquisition_Notification() {
    this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION;
    this->name = "ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION";
}
NM_Attribute_Ownership_Acquisition_Notification::~NM_Attribute_Ownership_Acquisition_Notification() {
}
void NM_Attribute_Ownership_Acquisition_Notification::serialize() {
}
void NM_Attribute_Ownership_Acquisition_Notification::deserialize() {
}
/*<END>---------- Attribute_Ownership_Acquisition_Notification ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Divestiture_Notification ------------<BEGIN>*/
NM_Attribute_Ownership_Divestiture_Notification::NM_Attribute_Ownership_Divestiture_Notification() {
    this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION;
    this->name = "ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION";
}
NM_Attribute_Ownership_Divestiture_Notification::~NM_Attribute_Ownership_Divestiture_Notification() {
}
void NM_Attribute_Ownership_Divestiture_Notification::serialize() {
}
void NM_Attribute_Ownership_Divestiture_Notification::deserialize() {
}
/*<END>---------- Attribute_Ownership_Divestiture_Notification ------------<END>*/

/*<BEGIN>---------- Request_Attribute_Ownership_Assumption ------------<BEGIN>*/
NM_Request_Attribute_Ownership_Assumption::NM_Request_Attribute_Ownership_Assumption() {
    this->type = NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION;
    this->name = "REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION";
}
NM_Request_Attribute_Ownership_Assumption::~NM_Request_Attribute_Ownership_Assumption() {
}
void NM_Request_Attribute_Ownership_Assumption::serialize() {
}
void NM_Request_Attribute_Ownership_Assumption::deserialize() {
}
/*<END>---------- Request_Attribute_Ownership_Assumption ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Unavailable ------------<BEGIN>*/
NM_Attribute_Ownership_Unavailable::NM_Attribute_Ownership_Unavailable() {
    this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_UNAVAILABLE;
    this->name = "ATTRIBUTE_OWNERSHIP_UNAVAILABLE";
}
NM_Attribute_Ownership_Unavailable::~NM_Attribute_Ownership_Unavailable() {
}
void NM_Attribute_Ownership_Unavailable::serialize() {
}
void NM_Attribute_Ownership_Unavailable::deserialize() {
}
/*<END>---------- Attribute_Ownership_Unavailable ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Acquisition_If_Available ------------<BEGIN>*/
NM_Attribute_Ownership_Acquisition_If_Available::NM_Attribute_Ownership_Acquisition_If_Available() {
    this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE;
    this->name = "ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE";
}
NM_Attribute_Ownership_Acquisition_If_Available::~NM_Attribute_Ownership_Acquisition_If_Available() {
}
void NM_Attribute_Ownership_Acquisition_If_Available::serialize() {
}
void NM_Attribute_Ownership_Acquisition_If_Available::deserialize() {
}
/*<END>---------- Attribute_Ownership_Acquisition_If_Available ------------<END>*/

/*<BEGIN>---------- Unconditional_Attribute_Ownership_Divestiture ------------<BEGIN>*/
NM_Unconditional_Attribute_Ownership_Divestiture::NM_Unconditional_Attribute_Ownership_Divestiture() {
    this->type = NetworkMessage::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
    this->name = "UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE";
}
NM_Unconditional_Attribute_Ownership_Divestiture::~NM_Unconditional_Attribute_Ownership_Divestiture() {
}
void NM_Unconditional_Attribute_Ownership_Divestiture::serialize() {
}
void NM_Unconditional_Attribute_Ownership_Divestiture::deserialize() {
}
/*<END>---------- Unconditional_Attribute_Ownership_Divestiture ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Acquisition ------------<BEGIN>*/
NM_Attribute_Ownership_Acquisition::NM_Attribute_Ownership_Acquisition() {
    this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION;
    this->name = "ATTRIBUTE_OWNERSHIP_ACQUISITION";
}
NM_Attribute_Ownership_Acquisition::~NM_Attribute_Ownership_Acquisition() {
}
void NM_Attribute_Ownership_Acquisition::serialize() {
}
void NM_Attribute_Ownership_Acquisition::deserialize() {
}
/*<END>---------- Attribute_Ownership_Acquisition ------------<END>*/

/*<BEGIN>---------- Request_Attribute_Ownership_Release ------------<BEGIN>*/
NM_Request_Attribute_Ownership_Release::NM_Request_Attribute_Ownership_Release() {
    this->type = NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE;
    this->name = "REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE";
}
NM_Request_Attribute_Ownership_Release::~NM_Request_Attribute_Ownership_Release() {
}
void NM_Request_Attribute_Ownership_Release::serialize() {
}
void NM_Request_Attribute_Ownership_Release::deserialize() {
}
/*<END>---------- Request_Attribute_Ownership_Release ------------<END>*/

/*<BEGIN>---------- Cancel_Negotiated_Attribute_Ownership_Divestiture ------------<BEGIN>*/
NM_Cancel_Negotiated_Attribute_Ownership_Divestiture::NM_Cancel_Negotiated_Attribute_Ownership_Divestiture() {
    this->type = NetworkMessage::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
    this->name = "CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE";
}
NM_Cancel_Negotiated_Attribute_Ownership_Divestiture::~NM_Cancel_Negotiated_Attribute_Ownership_Divestiture() {
}
void NM_Cancel_Negotiated_Attribute_Ownership_Divestiture::serialize() {
}
void NM_Cancel_Negotiated_Attribute_Ownership_Divestiture::deserialize() {
}
/*<END>---------- Cancel_Negotiated_Attribute_Ownership_Divestiture ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Release_Response ------------<BEGIN>*/
NM_Attribute_Ownership_Release_Response::NM_Attribute_Ownership_Release_Response() {
    this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE;
    this->name = "ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE";
}
NM_Attribute_Ownership_Release_Response::~NM_Attribute_Ownership_Release_Response() {
}
void NM_Attribute_Ownership_Release_Response::serialize() {
}
void NM_Attribute_Ownership_Release_Response::deserialize() {
}
/*<END>---------- Attribute_Ownership_Release_Response ------------<END>*/

/*<BEGIN>---------- Cancel_Attribute_Ownership_Acquisition ------------<BEGIN>*/
NM_Cancel_Attribute_Ownership_Acquisition::NM_Cancel_Attribute_Ownership_Acquisition() {
    this->type = NetworkMessage::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION;
    this->name = "CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION";
}
NM_Cancel_Attribute_Ownership_Acquisition::~NM_Cancel_Attribute_Ownership_Acquisition() {
}
void NM_Cancel_Attribute_Ownership_Acquisition::serialize() {
}
void NM_Cancel_Attribute_Ownership_Acquisition::deserialize() {
}
/*<END>---------- Cancel_Attribute_Ownership_Acquisition ------------<END>*/

/*<BEGIN>---------- Confirm_Attribute_Ownership_Acquisition_Cancellation ------------<BEGIN>*/
NM_Confirm_Attribute_Ownership_Acquisition_Cancellation::NM_Confirm_Attribute_Ownership_Acquisition_Cancellation() {
    this->type = NetworkMessage::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION;
    this->name = "CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION";
}
NM_Confirm_Attribute_Ownership_Acquisition_Cancellation::~NM_Confirm_Attribute_Ownership_Acquisition_Cancellation() {
}
void NM_Confirm_Attribute_Ownership_Acquisition_Cancellation::serialize() {
}
void NM_Confirm_Attribute_Ownership_Acquisition_Cancellation::deserialize() {
}
/*<END>---------- Confirm_Attribute_Ownership_Acquisition_Cancellation ------------<END>*/

/*<BEGIN>---------- Ddm_Create_Region ------------<BEGIN>*/
NM_Ddm_Create_Region::NM_Ddm_Create_Region() {
    this->type = NetworkMessage::DDM_CREATE_REGION;
    this->name = "DDM_CREATE_REGION";
}
NM_Ddm_Create_Region::~NM_Ddm_Create_Region() {
}
void NM_Ddm_Create_Region::serialize() {
}
void NM_Ddm_Create_Region::deserialize() {
}
/*<END>---------- Ddm_Create_Region ------------<END>*/

/*<BEGIN>---------- Ddm_Modify_Region ------------<BEGIN>*/
NM_Ddm_Modify_Region::NM_Ddm_Modify_Region() {
    this->type = NetworkMessage::DDM_MODIFY_REGION;
    this->name = "DDM_MODIFY_REGION";
}
NM_Ddm_Modify_Region::~NM_Ddm_Modify_Region() {
}
void NM_Ddm_Modify_Region::serialize() {
}
void NM_Ddm_Modify_Region::deserialize() {
}
/*<END>---------- Ddm_Modify_Region ------------<END>*/

/*<BEGIN>---------- Ddm_Delete_Region ------------<BEGIN>*/
NM_Ddm_Delete_Region::NM_Ddm_Delete_Region() {
    this->type = NetworkMessage::DDM_DELETE_REGION;
    this->name = "DDM_DELETE_REGION";
}
NM_Ddm_Delete_Region::~NM_Ddm_Delete_Region() {
}
void NM_Ddm_Delete_Region::serialize() {
}
void NM_Ddm_Delete_Region::deserialize() {
}
/*<END>---------- Ddm_Delete_Region ------------<END>*/

/*<BEGIN>---------- Ddm_Associate_Region ------------<BEGIN>*/
NM_Ddm_Associate_Region::NM_Ddm_Associate_Region() {
    this->type = NetworkMessage::DDM_ASSOCIATE_REGION;
    this->name = "DDM_ASSOCIATE_REGION";
}
NM_Ddm_Associate_Region::~NM_Ddm_Associate_Region() {
}
void NM_Ddm_Associate_Region::serialize() {
}
void NM_Ddm_Associate_Region::deserialize() {
}
/*<END>---------- Ddm_Associate_Region ------------<END>*/

/*<BEGIN>---------- Ddm_Register_Object ------------<BEGIN>*/
NM_Ddm_Register_Object::NM_Ddm_Register_Object() {
    this->type = NetworkMessage::DDM_REGISTER_OBJECT;
    this->name = "DDM_REGISTER_OBJECT";
}
NM_Ddm_Register_Object::~NM_Ddm_Register_Object() {
}
void NM_Ddm_Register_Object::serialize() {
}
void NM_Ddm_Register_Object::deserialize() {
}
/*<END>---------- Ddm_Register_Object ------------<END>*/

/*<BEGIN>---------- Ddm_Unassociate_Region ------------<BEGIN>*/
NM_Ddm_Unassociate_Region::NM_Ddm_Unassociate_Region() {
    this->type = NetworkMessage::DDM_UNASSOCIATE_REGION;
    this->name = "DDM_UNASSOCIATE_REGION";
}
NM_Ddm_Unassociate_Region::~NM_Ddm_Unassociate_Region() {
}
void NM_Ddm_Unassociate_Region::serialize() {
}
void NM_Ddm_Unassociate_Region::deserialize() {
}
/*<END>---------- Ddm_Unassociate_Region ------------<END>*/

/*<BEGIN>---------- Ddm_Subscribe_Attributes ------------<BEGIN>*/
NM_Ddm_Subscribe_Attributes::NM_Ddm_Subscribe_Attributes() {
    this->type = NetworkMessage::DDM_SUBSCRIBE_ATTRIBUTES;
    this->name = "DDM_SUBSCRIBE_ATTRIBUTES";
}
NM_Ddm_Subscribe_Attributes::~NM_Ddm_Subscribe_Attributes() {
}
void NM_Ddm_Subscribe_Attributes::serialize() {
}
void NM_Ddm_Subscribe_Attributes::deserialize() {
}
/*<END>---------- Ddm_Subscribe_Attributes ------------<END>*/

/*<BEGIN>---------- Ddm_Unsubscribe_Attributes ------------<BEGIN>*/
NM_Ddm_Unsubscribe_Attributes::NM_Ddm_Unsubscribe_Attributes() {
    this->type = NetworkMessage::DDM_UNSUBSCRIBE_ATTRIBUTES;
    this->name = "DDM_UNSUBSCRIBE_ATTRIBUTES";
}
NM_Ddm_Unsubscribe_Attributes::~NM_Ddm_Unsubscribe_Attributes() {
}
void NM_Ddm_Unsubscribe_Attributes::serialize() {
}
void NM_Ddm_Unsubscribe_Attributes::deserialize() {
}
/*<END>---------- Ddm_Unsubscribe_Attributes ------------<END>*/

/*<BEGIN>---------- Ddm_Subscribe_Interaction ------------<BEGIN>*/
NM_Ddm_Subscribe_Interaction::NM_Ddm_Subscribe_Interaction() {
    this->type = NetworkMessage::DDM_SUBSCRIBE_INTERACTION;
    this->name = "DDM_SUBSCRIBE_INTERACTION";
}
NM_Ddm_Subscribe_Interaction::~NM_Ddm_Subscribe_Interaction() {
}
void NM_Ddm_Subscribe_Interaction::serialize() {
}
void NM_Ddm_Subscribe_Interaction::deserialize() {
}
/*<END>---------- Ddm_Subscribe_Interaction ------------<END>*/

/*<BEGIN>---------- Ddm_Unsubscribe_Interaction ------------<BEGIN>*/
NM_Ddm_Unsubscribe_Interaction::NM_Ddm_Unsubscribe_Interaction() {
    this->type = NetworkMessage::DDM_UNSUBSCRIBE_INTERACTION;
    this->name = "DDM_UNSUBSCRIBE_INTERACTION";
}
NM_Ddm_Unsubscribe_Interaction::~NM_Ddm_Unsubscribe_Interaction() {
}
void NM_Ddm_Unsubscribe_Interaction::serialize() {
}
void NM_Ddm_Unsubscribe_Interaction::deserialize() {
}
/*<END>---------- Ddm_Unsubscribe_Interaction ------------<END>*/

/*<BEGIN>---------- Provide_Attribute_Value_Update ------------<BEGIN>*/
NM_Provide_Attribute_Value_Update::NM_Provide_Attribute_Value_Update() {
    this->type = NetworkMessage::PROVIDE_ATTRIBUTE_VALUE_UPDATE;
    this->name = "PROVIDE_ATTRIBUTE_VALUE_UPDATE";
}
NM_Provide_Attribute_Value_Update::~NM_Provide_Attribute_Value_Update() {
}
void NM_Provide_Attribute_Value_Update::serialize() {
}
void NM_Provide_Attribute_Value_Update::deserialize() {
}
/*<END>---------- Provide_Attribute_Value_Update ------------<END>*/

/*<BEGIN>---------- Get_Fed_File ------------<BEGIN>*/
NM_Get_Fed_File::NM_Get_Fed_File() {
    this->type = NetworkMessage::GET_FED_FILE;
    this->name = "GET_FED_FILE";
}
NM_Get_Fed_File::~NM_Get_Fed_File() {
}
void NM_Get_Fed_File::serialize() {
}
void NM_Get_Fed_File::deserialize() {
}
/*<END>---------- Get_Fed_File ------------<END>*/

/*<BEGIN>---------- Last ------------<BEGIN>*/
NM_Last::NM_Last() {
    this->type = NetworkMessage::LAST;
    this->name = "LAST";
}
NM_Last::~NM_Last() {
}
void NM_Last::serialize() {
}
void NM_Last::deserialize() {
}
/*<END>---------- Last ------------<END>*/





} /* end of certi namespace */

