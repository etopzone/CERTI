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
#include <assert.h>
namespace certi {

NetworkMessage* NM_Factory::create(NetworkMessage::Message_T type) throw (RTIinternalError) {
	NetworkMessage* msg;

	switch (type) {
	case NetworkMessage::NOT_USED:
		throw RTIinternalError("NOT_USED message type should not be used!!");
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
	case NetworkMessage::TIME_REGULATION_ENABLED:
		msg = new NM_Time_Regulation_Enabled(); 
		break;
	case NetworkMessage::TIME_CONSTRAINED_ENABLED:
		msg = new NM_Time_Constrained_Enabled(); 
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
	case NetworkMessage::SET_CLASS_RELEVANCE_ADVISORY_SWITCH:
	        msg = new NM_Set_Class_Relevance_Advisory_Switch();
		break;
	case NetworkMessage::SET_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
	        msg = new NM_Set_Interaction_Relevance_Advisory_Switch();
		break;
	case NetworkMessage::SET_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH:
	        msg = new NM_Set_Attribute_Relevance_Advisory_Switch();
		break;
	case NetworkMessage::SET_ATTRIBUTE_SCOPE_ADVISORY_SWITCH:
	        msg = new NM_Set_Attribute_Scope_Advisory_Switch();
		break;
	case NetworkMessage::START_REGISTRATION_FOR_OBJECT_CLASS:
	        msg = new NM_Start_Registration_For_Object_Class();
		break;
	case NetworkMessage::STOP_REGISTRATION_FOR_OBJECT_CLASS:
	        msg = new NM_Stop_Registration_For_Object_Class();
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
		throw RTIinternalError("CHANGE_ATTRIBUTE_TRANSPORT_TYPE: Unimplemented");
		msg = new NM_Change_Attribute_Transport_Type(); 
		break;
	case NetworkMessage::CHANGE_ATTRIBUTE_ORDER_TYPE:
		throw RTIinternalError("CHANGE_ATTRIBUTE_ORDER_TYPE: Unimplemented");
		msg = new NM_Change_Attribute_Order_Type(); 
		break;	   
	case NetworkMessage::CHANGE_INTERACTION_TRANSPORT_TYPE:
		throw RTIinternalError("CHANGE_INTERACTION_TRANSPORT_TYPE: Unimplemented");
		msg = new NM_Change_Interaction_Transport_Type(); 
		break;	   
	case NetworkMessage::CHANGE_INTERACTION_ORDER_TYPE:
		throw RTIinternalError("CHANGE_INTERACTION_ORDER_TYPE: Unimplemented");
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
		msg = new NM_DDM_Create_Region(); 
		break;
	case NetworkMessage::DDM_MODIFY_REGION:
		msg = new NM_DDM_Modify_Region(); 
		break;
	case NetworkMessage::DDM_DELETE_REGION:
		msg = new NM_DDM_Delete_Region(); 
		break;
	case NetworkMessage::DDM_ASSOCIATE_REGION:
		msg = new NM_DDM_Associate_Region(); 
		break;
	case NetworkMessage::DDM_REGISTER_OBJECT:
		msg = new NM_DDM_Register_Object(); 
		break;
	case NetworkMessage::DDM_UNASSOCIATE_REGION:
		msg = new NM_DDM_Unassociate_Region(); 
		break;
	case NetworkMessage::DDM_SUBSCRIBE_ATTRIBUTES:
		msg = new NM_DDM_Subscribe_Attributes(); 
		break;
	case NetworkMessage::DDM_UNSUBSCRIBE_ATTRIBUTES:
		msg = new NM_DDM_Unsubscribe_Attributes(); 
		break;
	case NetworkMessage::DDM_SUBSCRIBE_INTERACTION:
		msg = new NM_DDM_Subscribe_Interaction(); 
		break;
	case NetworkMessage::DDM_UNSUBSCRIBE_INTERACTION:
		msg = new NM_DDM_Unsubscribe_Interaction(); 
		break;
	case NetworkMessage::PROVIDE_ATTRIBUTE_VALUE_UPDATE:
		msg = new NM_Provide_Attribute_Value_Update(); 
		break;
	case NetworkMessage::LAST:
		throw RTIinternalError("LAST message type should not be used!!");
		msg = new NM_Last(); 
		break;
	default:
		throw RTIinternalError("Unknown/Unimplemented message Type");
	}
	
	return msg;
} /* end of NM_Factory::create */

NetworkMessage* 
NM_Factory::receive(Socket* socket) throw (NetworkError, RTIinternalError) {
	// FIXME this is not thread-safe.
	static MessageBuffer msgBuffer;
	NetworkMessage  msgGen;
	NetworkMessage* msg;
	
	/* receive generic message */
	msgGen.receive(socket,msgBuffer);
	/* create specific message from type */
	msg = NM_Factory::create(msgGen.getType());
	msgBuffer.assumeSizeFromReservedBytes();	
	msg->deserialize(msgBuffer);
	return msg;	
} /* end of NM_Factory::receive */

NM_WithHandleArray::NM_WithHandleArray() {
	this->name = "NM_WithHandleArray";
	this->type = NOT_USED;
	/* specific field init */
	this->handleArraySize = 0;
}

NM_WithHandleArray::~NM_WithHandleArray() {
}

void NM_WithHandleArray::serialize(MessageBuffer& msgBuffer) {
	int i;
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */	
	msgBuffer.write_uint16(handleArraySize);
	/* 
	 * Note that if handleArraySize is 0 
	 * the loop is void which is done on purpose.
	 * (this is a feature not a bug :-) 
	 */		
	for (i = 0 ; i < handleArraySize ; ++i) {
		msgBuffer.write_uint16(handleArray[i]);
	}

} /* end of serialize */ 
void NM_WithHandleArray::deserialize(MessageBuffer& msgBuffer) {
	int i;
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */	
	handleArraySize = msgBuffer.read_int16();
	/* 
	 * Note that if handleArraySize is 0 
	 * the loop is void which is done on purpose.
	 * (this is a feature not a bug :-) 
	 */
        handleArray.resize(handleArraySize);	
	for (i = 0 ; i < handleArraySize ; i ++) {
		handleArray[i] = msgBuffer.read_int16();
	}
} /* end of deserialize */

NM_DDM_Base::NM_DDM_Base() {
	this->name = "NM_DDM_Base";
	this->type = NetworkMessage::NOT_USED;
	/* specific field init */
	this->space       = 0;
	this->nbExtents   = 0;
	this->region      = 0;
	this->object      = 0;
	this->objectClass = 0;
	this->DDM_bool    = false; /* FIXME don't know the purpose of this */
}

NM_DDM_Base::~NM_DDM_Base() {
}

/*<BEGIN>---------- Not_Used ------------<BEGIN>*/
NM_Not_Used::NM_Not_Used() {
	this->name = "NOT_USED";
	this->type = NetworkMessage::NOT_USED;
	/* specific field init */
}
NM_Not_Used::~NM_Not_Used() {
}
/*<END>---------- Not_Used ------------<END>*/

/*<BEGIN>---------- Close_Connexion ------------<BEGIN>*/
NM_Close_Connexion::NM_Close_Connexion() {
	this->name = "CLOSE_CONNEXION";
	this->type = NetworkMessage::CLOSE_CONNEXION;
	/* specific field init */
}
NM_Close_Connexion::~NM_Close_Connexion() {
}
/*<END>---------- Close_Connexion ------------<END>*/

/*<BEGIN>---------- Message_Null ------------<BEGIN>*/
NM_Message_Null::NM_Message_Null() {
	this->name = "MESSAGE_NULL";
	this->type = MESSAGE_NULL;
	/* specific field init */	
}
NM_Message_Null::~NM_Message_Null() {	
}
/*<END>---------- Message_Null ------------<END>*/

/*<BEGIN>---------- FOM_Dimension ------------<BEGIN>*/
void NM_FOM_Dimension::serialize(MessageBuffer& msgBuffer) {
	msgBuffer.write_uint32(_handle);
	msgBuffer.write_string(_name);
}
void NM_FOM_Dimension::deserialize(MessageBuffer& msgBuffer) {
	_handle = msgBuffer.read_uint32();
	msgBuffer.read_string(_name);
}
/*<END>---------- FOM_Dimension ------------<END>*/

/*<BEGIN>---------- FOM_Routing_Space ------------<BEGIN>*/
void NM_FOM_Routing_Space::serialize(MessageBuffer& msgBuffer) {
	msgBuffer.write_uint32(_handle);
	msgBuffer.write_string(_name);
        uint32_t size = _dimensions.size();
	msgBuffer.write_uint32(size);
        for (uint32_t i = 0; i < size; ++i)
          _dimensions[i].serialize(msgBuffer);
} /* end of serialize */ 
void NM_FOM_Routing_Space::deserialize(MessageBuffer& msgBuffer) {
	_handle = msgBuffer.read_uint32();
	msgBuffer.read_string(_name);
	uint32_t size = msgBuffer.read_uint32();
        _dimensions.resize(size);
        for (uint32_t i = 0; i < size; ++i)
          _dimensions[i].deserialize(msgBuffer);
} /* end of deserialize */
/*<END>---------- FOM_Routing_Space ------------<END>*/

/*<BEGIN>---------- FOM_Attribute ------------<BEGIN>*/
void NM_FOM_Attribute::serialize(MessageBuffer& msgBuffer) {
	msgBuffer.write_uint32(_handle);
	msgBuffer.write_string(_name);
	msgBuffer.write_uint32(_spaceHandle);
	msgBuffer.write_uint8(_order);
	msgBuffer.write_uint8(_transport);
}
void NM_FOM_Attribute::deserialize(MessageBuffer& msgBuffer) {
	_handle = msgBuffer.read_uint32();
	msgBuffer.read_string(_name);
        _spaceHandle = msgBuffer.read_uint32();
	_order = msgBuffer.read_uint8();
	_transport = msgBuffer.read_uint8();
}
/*<END>---------- FOM_Attribute ------------<END>*/

/*<BEGIN>---------- FOM_Object_Class ------------<BEGIN>*/
void NM_FOM_Object_Class::serialize(MessageBuffer& msgBuffer) {
	msgBuffer.write_uint32(_handle);
	msgBuffer.write_string(_name);
	msgBuffer.write_uint32(_superclassHandle);
        uint32_t size = _attributes.size();
	msgBuffer.write_uint32(size);
        for (uint32_t i = 0; i < size; ++i)
          _attributes[i].serialize(msgBuffer);
} /* end of serialize */ 
void NM_FOM_Object_Class::deserialize(MessageBuffer& msgBuffer) {
	_handle = msgBuffer.read_uint32();
	msgBuffer.read_string(_name);
        _superclassHandle = msgBuffer.read_uint32();
	uint32_t size = msgBuffer.read_uint32();
        _attributes.resize(size);
        for (uint32_t i = 0; i < size; ++i)
          _attributes[i].deserialize(msgBuffer);
} /* end of deserialize */
/*<END>---------- FOM_Object_Class ------------<END>*/

/*<BEGIN>---------- FOM_FOM_Parameter ------------<BEGIN>*/
void NM_FOM_Parameter::serialize(MessageBuffer& msgBuffer) {
	msgBuffer.write_uint32(_handle);
	msgBuffer.write_string(_name);
}
void NM_FOM_Parameter::deserialize(MessageBuffer& msgBuffer) {
	_handle = msgBuffer.read_uint32();
	msgBuffer.read_string(_name);
}
/*<END>---------- FOM_FOM_Parameter ------------<END>*/

/*<BEGIN>---------- FOM_Interaction_Class ------------<BEGIN>*/
void NM_FOM_Interaction_Class::serialize(MessageBuffer& msgBuffer) {
	msgBuffer.write_uint32(_handle);
	msgBuffer.write_string(_name);
	msgBuffer.write_uint32(_superclassHandle);
	msgBuffer.write_uint32(_spaceHandle);
	msgBuffer.write_uint8(_order);
	msgBuffer.write_uint8(_transport);
        uint32_t size = _parameters.size();
	msgBuffer.write_uint32(size);
        for (uint32_t i = 0; i < size; ++i)
          _parameters[i].serialize(msgBuffer);
} /* end of serialize */ 
void NM_FOM_Interaction_Class::deserialize(MessageBuffer& msgBuffer) {
	_handle = msgBuffer.read_uint32();
	msgBuffer.read_string(_name);
        _superclassHandle = msgBuffer.read_uint32();
        _spaceHandle = msgBuffer.read_uint32();
	_order = msgBuffer.read_uint8();
	_transport = msgBuffer.read_uint8();
	uint32_t size = msgBuffer.read_uint32();
        _parameters.resize(size);
        for (uint32_t i = 0; i < size; ++i)
          _parameters[i].deserialize(msgBuffer);
} /* end of deserialize */
/*<END>---------- FOM_Interaction_Class ------------<END>*/

/*<BEGIN>---------- Create_Federation_Execution ------------<BEGIN>*/
NM_Create_Federation_Execution::NM_Create_Federation_Execution() {
	this->name = "CREATE_FEDERATION_EXECUTION";
	this->type = NetworkMessage::CREATE_FEDERATION_EXECUTION;
	/* specific field init */
}
NM_Create_Federation_Execution::~NM_Create_Federation_Execution() {	
}
void NM_Create_Federation_Execution::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_string(federationName);
	msgBuffer.write_string(FEDid);
} /* end of serialize */ 
void NM_Create_Federation_Execution::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.read_string(federationName);
	msgBuffer.read_string(FEDid);
} /* end of deserialize */
/*<END>---------- Create_Federation_Execution ------------<END>*/

/*<BEGIN>---------- Destroy_Federation_Execution ------------<BEGIN>*/
NM_Destroy_Federation_Execution::NM_Destroy_Federation_Execution() {
	this->name = "DESTROY_FEDERATION_EXECUTION";
	this->type = NetworkMessage::DESTROY_FEDERATION_EXECUTION;    
	/* specific field init */    
}
NM_Destroy_Federation_Execution::~NM_Destroy_Federation_Execution() {
}
void NM_Destroy_Federation_Execution::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_string(federationName);
} /* end of serialize */ 
void NM_Destroy_Federation_Execution::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.read_string(federationName);
} /* end of deserialize */
/*<END>---------- Destroy_Federation_Execution ------------<END>*/

/*<BEGIN>---------- Join_Federation_Execution ------------<BEGIN>*/
NM_Join_Federation_Execution::NM_Join_Federation_Execution() {
	this->name = "JOIN_FEDERATION_EXECUTION";
	this->type = NetworkMessage::JOIN_FEDERATION_EXECUTION;
	/* specific field init */
}
NM_Join_Federation_Execution::~NM_Join_Federation_Execution() {
}
void NM_Join_Federation_Execution::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_int32(numberOfRegulators);
	msgBuffer.write_uint32(multicastAddress);
	msgBuffer.write_uint32(bestEffortAddress);
	msgBuffer.write_uint32(bestEffortPeer);
	msgBuffer.write_string(federationName);
	msgBuffer.write_string(federateName);
        uint32_t size = _routingSpaces.size();
	msgBuffer.write_uint32(size);
        for (uint32_t i = 0; i < size; ++i)
          _routingSpaces[i].serialize(msgBuffer);
        size = _objectClasses.size();
	msgBuffer.write_uint32(size);
        for (uint32_t i = 0; i < size; ++i)
          _objectClasses[i].serialize(msgBuffer);
        size = _interactionClasses.size();
	msgBuffer.write_uint32(size);
        for (uint32_t i = 0; i < size; ++i)
          _interactionClasses[i].serialize(msgBuffer);
} /* end of serialize */ 
void NM_Join_Federation_Execution::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	numberOfRegulators = msgBuffer.read_int32();
	multicastAddress   = msgBuffer.read_uint32();
	bestEffortAddress  = msgBuffer.read_uint32();
	bestEffortPeer     = msgBuffer.read_uint32();
	msgBuffer.read_string(federationName);
	msgBuffer.read_string(federateName);
	uint32_t size = msgBuffer.read_uint32();
        _routingSpaces.resize(size);
        for (uint32_t i = 0; i < size; ++i)
          _routingSpaces[i].deserialize(msgBuffer);
	size = msgBuffer.read_uint32();
        _objectClasses.resize(size);
        for (uint32_t i = 0; i < size; ++i)
          _objectClasses[i].deserialize(msgBuffer);
	size = msgBuffer.read_uint32();
        _interactionClasses.resize(size);
        for (uint32_t i = 0; i < size; ++i)
          _interactionClasses[i].deserialize(msgBuffer);
} /* end of deserialize */
/*<END>---------- Join_Federation_Execution ------------<END>*/

/*<BEGIN>---------- Resign_Federation_Execution ------------<BEGIN>*/
NM_Resign_Federation_Execution::NM_Resign_Federation_Execution() {
	this->name = "RESIGN_FEDERATION_EXECUTION";
	this->type = NetworkMessage::RESIGN_FEDERATION_EXECUTION;
	/* specific field init */
}
NM_Resign_Federation_Execution::~NM_Resign_Federation_Execution() {
}
/*<END>---------- Resign_Federation_Execution ------------<END>*/

/*<BEGIN>---------- Set_Time_Regulating ------------<BEGIN>*/
NM_Set_Time_Regulating::NM_Set_Time_Regulating() {
	this->name = "SET_TIME_REGULATING";
	this->type = NetworkMessage::SET_TIME_REGULATING;
	/* specific field init */	
	regulator = false;
}
NM_Set_Time_Regulating::~NM_Set_Time_Regulating() {
}
void NM_Set_Time_Regulating::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_bool(regulator);  	  
} /* end of serialize */ 
void NM_Set_Time_Regulating::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	regulator = msgBuffer.read_bool();
} /* end of deserialize */
/*<END>---------- Set_Time_Regulating ------------<END>*/

/*<BEGIN>---------- Set_Time_Constrained ------------<BEGIN>*/
NM_Set_Time_Constrained::NM_Set_Time_Constrained() {
	this->name = "SET_TIME_CONSTRAINED";
	this->type = NetworkMessage::SET_TIME_CONSTRAINED;
	/* specific field init */
	constrained=false;
}
NM_Set_Time_Constrained::~NM_Set_Time_Constrained() {
}
void NM_Set_Time_Constrained::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_bool(constrained);  
} /* end of serialize */ 
void NM_Set_Time_Constrained::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	constrained = msgBuffer.read_bool();
} /* end of deserialize */
/*<END>---------- Set_Time_Constrained ------------<END>*/

/*<BEGIN>---------- Time_Regulation_Enabled  ------------<BEGIN>*/
NM_Time_Regulation_Enabled::NM_Time_Regulation_Enabled() {
       this->name = "TIME_REGULATION_ENABLED";
       this->type = NetworkMessage::TIME_REGULATION_ENABLED;
}

NM_Time_Regulation_Enabled::~NM_Time_Regulation_Enabled() {
}
/*<END>---------- Time_Regulation_Enabled ------------<END>*/

/*<BEGIN>---------- Time_Constrained_Enabled  ------------<BEGIN>*/
NM_Time_Constrained_Enabled::NM_Time_Constrained_Enabled() {
       this->name = "TIME_CONSTRAINED_ENABLED";
       this->type = NetworkMessage::TIME_CONSTRAINED_ENABLED;
}
NM_Time_Constrained_Enabled::~NM_Time_Constrained_Enabled() {
}
/*<END>---------- Time_Constrained_Enabled ------------<END>*/

/*<BEGIN>---------- Start_Registration_For_Object_Class ------------<BEGIN>*/
NM_Start_Registration_For_Object_Class::NM_Start_Registration_For_Object_Class() {
	this->name = "START_REGISTRATION_FOR_OBJECT_CLASS";
	this->type = NetworkMessage::START_REGISTRATION_FOR_OBJECT_CLASS;
	/* specific field init */
}
NM_Start_Registration_For_Object_Class::~NM_Start_Registration_For_Object_Class() {
}
void NM_Start_Registration_For_Object_Class::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_int32(objectClass);
} /* end of serialize */ 
void NM_Start_Registration_For_Object_Class::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	objectClass = msgBuffer.read_int32();
} /* end of deserialize */

/*<END>---------- Start_Registration_For_Object_Class ------------<END>*/

/*<BEGIN>---------- Stop_Registration_For_Object_Class ------------<BEGIN>*/
NM_Stop_Registration_For_Object_Class::NM_Stop_Registration_For_Object_Class() {
	this->name = "STOP_REGISTRATION_FOR_OBJECT_CLASS";
	this->type = NetworkMessage::STOP_REGISTRATION_FOR_OBJECT_CLASS;
	/* specific field init */
}
NM_Stop_Registration_For_Object_Class::~NM_Stop_Registration_For_Object_Class() {
}
void NM_Stop_Registration_For_Object_Class::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_int32(objectClass);
} /* end of serialize */ 
void NM_Stop_Registration_For_Object_Class::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	objectClass = msgBuffer.read_int32();
} /* end of deserialize */

/*<END>---------- Stop_Registration_For_Object_Class ------------<END>*/

/*<BEGIN>---------- Set_Class_Relevance_Advisory_Switch ------------<BEGIN>*/
NM_Set_Class_Relevance_Advisory_Switch::NM_Set_Class_Relevance_Advisory_Switch() {
	this->name = "SET_CLASS_RELEVANCE_ADVISORY_SWITCH";
	this->type = NetworkMessage::SET_CLASS_RELEVANCE_ADVISORY_SWITCH;
	/* specific field init */
	classRelevanceAdvisorySwitch=true;
}
NM_Set_Class_Relevance_Advisory_Switch::~NM_Set_Class_Relevance_Advisory_Switch() {
}
void NM_Set_Class_Relevance_Advisory_Switch::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_bool(classRelevanceAdvisorySwitch);  
} /* end of serialize */ 
void NM_Set_Class_Relevance_Advisory_Switch::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	classRelevanceAdvisorySwitch = msgBuffer.read_bool();
} /* end of deserialize */
/*<END>---------- Set_Class_Relevance_Advisory_Switch ------------<END>*/

/*<BEGIN>---------- Set_Interaction_Relevance_Advisory_Switch ------------<BEGIN>*/
NM_Set_Interaction_Relevance_Advisory_Switch::NM_Set_Interaction_Relevance_Advisory_Switch() {
	this->name = "SET_INTERACTION_RELEVANCE_ADVISORY_SWITCH";
	this->type = NetworkMessage::SET_INTERACTION_RELEVANCE_ADVISORY_SWITCH;
	/* specific field init */
	interactionRelevanceAdvisorySwitch=true;
}
NM_Set_Interaction_Relevance_Advisory_Switch::~NM_Set_Interaction_Relevance_Advisory_Switch() {
}
void NM_Set_Interaction_Relevance_Advisory_Switch::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_bool(interactionRelevanceAdvisorySwitch);  
} /* end of serialize */ 
void NM_Set_Interaction_Relevance_Advisory_Switch::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	interactionRelevanceAdvisorySwitch = msgBuffer.read_bool();
} /* end of deserialize */
/*<END>---------- Set_Interaction_Relevance_Advisory_Switch ------------<END>*/

/*<BEGIN>---------- Set_Attribute_Relevance_Advisory_Switch ------------<BEGIN>*/
NM_Set_Attribute_Relevance_Advisory_Switch::NM_Set_Attribute_Relevance_Advisory_Switch() {
	this->name = "SET_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH";
	this->type = NetworkMessage::SET_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH;
	/* specific field init */
	attributeRelevanceAdvisorySwitch=false;
}
NM_Set_Attribute_Relevance_Advisory_Switch::~NM_Set_Attribute_Relevance_Advisory_Switch() {
}
void NM_Set_Attribute_Relevance_Advisory_Switch::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_bool(attributeRelevanceAdvisorySwitch);  
} /* end of serialize */ 
void NM_Set_Attribute_Relevance_Advisory_Switch::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	attributeRelevanceAdvisorySwitch = msgBuffer.read_bool();
} /* end of deserialize */
/*<END>---------- Set_Attribute_Relevance_Advisory_Switch ------------<END>*/

/*<BEGIN>---------- Set_Attribute_Scope_Advisory_Switch ------------<BEGIN>*/
NM_Set_Attribute_Scope_Advisory_Switch::NM_Set_Attribute_Scope_Advisory_Switch() {
	this->name = "SET_ATTRIBUTE_SCOPE_ADVISORY_SWITCH";
	this->type = NetworkMessage::SET_ATTRIBUTE_SCOPE_ADVISORY_SWITCH;
	/* specific field init */
	attributeScopeAdvisorySwitch=false;
}
NM_Set_Attribute_Scope_Advisory_Switch::~NM_Set_Attribute_Scope_Advisory_Switch() {
}
void NM_Set_Attribute_Scope_Advisory_Switch::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_bool(attributeScopeAdvisorySwitch);  
} /* end of serialize */ 
void NM_Set_Attribute_Scope_Advisory_Switch::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	attributeScopeAdvisorySwitch = msgBuffer.read_bool();
} /* end of deserialize */
/*<END>---------- Set_Attribute_Scope_Advisory_Switch ------------<END>*/

/*<BEGIN>---------- Register_Federation_Synchronization_Point ------------<BEGIN>*/
NM_Register_Federation_Synchronization_Point::NM_Register_Federation_Synchronization_Point() {
	this->name = "REGISTER_FEDERATION_SYNCHRONIZATION_POINT";
	this->type = NetworkMessage::REGISTER_FEDERATION_SYNCHRONIZATION_POINT;
	/* specific field init */
	this->_setExist = false;
}
NM_Register_Federation_Synchronization_Point::~NM_Register_Federation_Synchronization_Point() {
}
void 
NM_Register_Federation_Synchronization_Point::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_bool(_setExist);  
} /* end of serialize */ 
void 
NM_Register_Federation_Synchronization_Point::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	_setExist = msgBuffer.read_bool();
} /* end of deserialize */
/*<END>---------- Register_Federation_Synchronization_Point ------------<END>*/

/*<BEGIN>---------- Synchronization_Point_Registration_Succeeded ------------<BEGIN>*/
NM_Synchronization_Point_Registration_Succeeded::NM_Synchronization_Point_Registration_Succeeded() {
	this->name = "SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED";
	this->type = NetworkMessage::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED;
	/* specific field init */
}
NM_Synchronization_Point_Registration_Succeeded::~NM_Synchronization_Point_Registration_Succeeded() {
}
/*<END>---------- Synchronization_Point_Registration_Succeeded ------------<END>*/

/*<BEGIN>---------- Announce_Synchronization_Point ------------<BEGIN>*/
NM_Announce_Synchronization_Point::NM_Announce_Synchronization_Point() {
	this->name = "ANNOUNCE_SYNCHRONIZATION_POINT";
	this->type = NetworkMessage::ANNOUNCE_SYNCHRONIZATION_POINT;
	/* specific field init */
}
NM_Announce_Synchronization_Point::~NM_Announce_Synchronization_Point() {
}
/*<END>---------- Announce_Synchronization_Point ------------<END>*/

/*<BEGIN>---------- Synchronization_Point_Achieved ------------<BEGIN>*/
NM_Synchronization_Point_Achieved::NM_Synchronization_Point_Achieved() {
	this->name = "SYNCHRONIZATION_POINT_ACHIEVED";
	this->type = NetworkMessage::SYNCHRONIZATION_POINT_ACHIEVED;
	/* specific field init */
}
NM_Synchronization_Point_Achieved::~NM_Synchronization_Point_Achieved() {
}
/*<END>---------- Synchronization_Point_Achieved ------------<END>*/

/*<BEGIN>---------- Federation_Synchronized ------------<BEGIN>*/
NM_Federation_Synchronized::NM_Federation_Synchronized() {
	this->name = "FEDERATION_SYNCHRONIZED";
	this->type = NetworkMessage::FEDERATION_SYNCHRONIZED;
	/* specific field init */
}
NM_Federation_Synchronized::~NM_Federation_Synchronized() {
}
/*<END>---------- Federation_Synchronized ------------<END>*/

/*<BEGIN>---------- Request_Federation_Save ------------<BEGIN>*/
NM_Request_Federation_Save::NM_Request_Federation_Save() {
	this->name = "REQUEST_FEDERATION_SAVE";
	this->type = NetworkMessage::REQUEST_FEDERATION_SAVE;
	/* specific field init */
}
NM_Request_Federation_Save::~NM_Request_Federation_Save() {
}
/*<END>---------- Request_Federation_Save ------------<END>*/

/*<BEGIN>---------- Federate_Save_Begun ------------<BEGIN>*/
NM_Federate_Save_Begun::NM_Federate_Save_Begun() {
	this->name = "FEDERATE_SAVE_BEGUN";
	this->type = NetworkMessage::FEDERATE_SAVE_BEGUN;
	/* specific field init */
}
NM_Federate_Save_Begun::~NM_Federate_Save_Begun() {
}
/*<END>---------- Federate_Save_Begun ------------<END>*/

/*<BEGIN>---------- Federate_Save_Complete ------------<BEGIN>*/
NM_Federate_Save_Complete::NM_Federate_Save_Complete() {
	this->name = "FEDERATE_SAVE_COMPLETE";
	this->type = NetworkMessage::FEDERATE_SAVE_COMPLETE;
	/* specific field init */
}
NM_Federate_Save_Complete::~NM_Federate_Save_Complete() {
}
/*<END>---------- Federate_Save_Complete ------------<END>*/

/*<BEGIN>---------- Federate_Save_Not_Complete ------------<BEGIN>*/
NM_Federate_Save_Not_Complete::NM_Federate_Save_Not_Complete() {
	this->name = "FEDERATE_SAVE_NOT_COMPLETE";
	this->type = NetworkMessage::FEDERATE_SAVE_NOT_COMPLETE;
	/* specific field init */
}
NM_Federate_Save_Not_Complete::~NM_Federate_Save_Not_Complete() {
}
/*<END>---------- Federate_Save_Not_Complete ------------<END>*/

/*<BEGIN>---------- Initiate_Federate_Save ------------<BEGIN>*/
NM_Initiate_Federate_Save::NM_Initiate_Federate_Save() {
	this->name = "INITIATE_FEDERATE_SAVE";
	this->type = NetworkMessage::INITIATE_FEDERATE_SAVE;
	/* specific field init */
}
NM_Initiate_Federate_Save::~NM_Initiate_Federate_Save() {
}
/*<END>---------- Initiate_Federate_Save ------------<END>*/

/*<BEGIN>---------- Federation_Saved ------------<BEGIN>*/
NM_Federation_Saved::NM_Federation_Saved() {
	this->name = "FEDERATION_SAVED";
	this->type = NetworkMessage::FEDERATION_SAVED;
	/* specific field init */
}
NM_Federation_Saved::~NM_Federation_Saved() {
}
/*<END>---------- Federation_Saved ------------<END>*/

/*<BEGIN>---------- Federation_Not_Saved ------------<BEGIN>*/
NM_Federation_Not_Saved::NM_Federation_Not_Saved() {
	this->name = "FEDERATION_NOT_SAVED";
	this->type = NetworkMessage::FEDERATION_NOT_SAVED;
	/* specific field init */
}
NM_Federation_Not_Saved::~NM_Federation_Not_Saved() {
}
/*<END>---------- Federation_Not_Saved ------------<END>*/

/*<BEGIN>---------- Request_Federation_Restore ------------<BEGIN>*/
NM_Request_Federation_Restore::NM_Request_Federation_Restore() {
	this->name = "REQUEST_FEDERATION_RESTORE";
	this->type = NetworkMessage::REQUEST_FEDERATION_RESTORE;
	/* specific field init */
}
NM_Request_Federation_Restore::~NM_Request_Federation_Restore() {
}
/*<END>---------- Request_Federation_Restore ------------<END>*/

/*<BEGIN>---------- Federate_Restore_Complete ------------<BEGIN>*/
NM_Federate_Restore_Complete::NM_Federate_Restore_Complete() {
	this->name = "FEDERATE_RESTORE_COMPLETE";
	this->type = NetworkMessage::FEDERATE_RESTORE_COMPLETE;
	/* specific field init */
}
NM_Federate_Restore_Complete::~NM_Federate_Restore_Complete() {
}
/*<END>---------- Federate_Restore_Complete ------------<END>*/

/*<BEGIN>---------- Federate_Restore_Not_Complete ------------<BEGIN>*/
NM_Federate_Restore_Not_Complete::NM_Federate_Restore_Not_Complete() {
	this->name = "FEDERATE_RESTORE_NOT_COMPLETE";
	this->type = NetworkMessage::FEDERATE_RESTORE_NOT_COMPLETE;
	/* specific field init */
}
NM_Federate_Restore_Not_Complete::~NM_Federate_Restore_Not_Complete() {
}
/*<END>---------- Federate_Restore_Not_Complete ------------<END>*/

/*<BEGIN>---------- Request_Federation_Restore_Succeeded ------------<BEGIN>*/
NM_Request_Federation_Restore_Succeeded::NM_Request_Federation_Restore_Succeeded() {
	this->name = "REQUEST_FEDERATION_RESTORE_SUCCEEDED";
	this->type = NetworkMessage::REQUEST_FEDERATION_RESTORE_SUCCEEDED;
	/* specific field init */
}
NM_Request_Federation_Restore_Succeeded::~NM_Request_Federation_Restore_Succeeded() {
}
/*<END>---------- Request_Federation_Restore_Succeeded ------------<END>*/

/*<BEGIN>---------- Request_Federation_Restore_Failed ------------<BEGIN>*/
NM_Request_Federation_Restore_Failed::NM_Request_Federation_Restore_Failed() {
	this->name = "REQUEST_FEDERATION_RESTORE_FAILED";
	this->type = NetworkMessage::REQUEST_FEDERATION_RESTORE_FAILED;
	/* specific field init */
}
NM_Request_Federation_Restore_Failed::~NM_Request_Federation_Restore_Failed() {
}
/*<END>---------- Request_Federation_Restore_Failed ------------<END>*/

/*<BEGIN>---------- Federation_Restore_Begun ------------<BEGIN>*/
NM_Federation_Restore_Begun::NM_Federation_Restore_Begun() {
	this->name = "FEDERATION_RESTORE_BEGUN";
	this->type = NetworkMessage::FEDERATION_RESTORE_BEGUN;
	/* specific field init */
}
NM_Federation_Restore_Begun::~NM_Federation_Restore_Begun() {
}
/*<END>---------- Federation_Restore_Begun ------------<END>*/

/*<BEGIN>---------- Initiate_Federate_Restore ------------<BEGIN>*/
NM_Initiate_Federate_Restore::NM_Initiate_Federate_Restore() {
	this->name = "INITIATE_FEDERATE_RESTORE";
	this->type = NetworkMessage::INITIATE_FEDERATE_RESTORE;
	/* specific field init */
}
NM_Initiate_Federate_Restore::~NM_Initiate_Federate_Restore() {
}
/*<END>---------- Initiate_Federate_Restore ------------<END>*/

/*<BEGIN>---------- Federation_Restored ------------<BEGIN>*/
NM_Federation_Restored::NM_Federation_Restored() {
	this->name = "FEDERATION_RESTORED";
	this->type = NetworkMessage::FEDERATION_RESTORED;
	/* specific field init */
}
NM_Federation_Restored::~NM_Federation_Restored() {
}
/*<END>---------- Federation_Restored ------------<END>*/

/*<BEGIN>---------- Federation_Not_Restored ------------<BEGIN>*/
NM_Federation_Not_Restored::NM_Federation_Not_Restored() {
	this->name = "FEDERATION_NOT_RESTORED";
	this->type = NetworkMessage::FEDERATION_NOT_RESTORED;
	/* specific field init */
}
NM_Federation_Not_Restored::~NM_Federation_Not_Restored() {
}
/*<END>---------- Federation_Not_Restored ------------<END>*/

/*<BEGIN>---------- Publish_Object_Class ------------<BEGIN>*/
NM_Publish_Object_Class::NM_Publish_Object_Class() {
	this->name = "PUBLISH_OBJECT_CLASS";
	this->type = NetworkMessage::PUBLISH_OBJECT_CLASS;
	/* specific field init */
}
NM_Publish_Object_Class::~NM_Publish_Object_Class() {
}
void
NM_Publish_Object_Class::serialize(MessageBuffer& msgBuffer) {
	Super::serialize(msgBuffer);
	msgBuffer.write_int32(objectClass);
}
void
NM_Publish_Object_Class::deserialize(MessageBuffer& msgBuffer) {
	Super::deserialize(msgBuffer);
	objectClass = msgBuffer.read_int32();
}
/*<END>---------- Publish_Object_Class ------------<END>*/

/*<BEGIN>---------- Unpublish_Object_Class ------------<BEGIN>*/
NM_Unpublish_Object_Class::NM_Unpublish_Object_Class() {
	this->name = "UNPUBLISH_OBJECT_CLASS";
	this->type = NetworkMessage::UNPUBLISH_OBJECT_CLASS;
	/* specific field init */
}
NM_Unpublish_Object_Class::~NM_Unpublish_Object_Class() {
}
void NM_Unpublish_Object_Class::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_int32(objectClass);
} /* end of serialize */ 
void NM_Unpublish_Object_Class::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	objectClass = msgBuffer.read_int32();
} /* end of deserialize */
/*<END>---------- Unpublish_Object_Class ------------<END>*/

/*<BEGIN>---------- Publish_Interaction_Class ------------<BEGIN>*/
NM_Publish_Interaction_Class::NM_Publish_Interaction_Class() {
	this->name = "PUBLISH_INTERACTION_CLASS";
	this->type = NetworkMessage::PUBLISH_INTERACTION_CLASS;
	/* specific field init */
}
NM_Publish_Interaction_Class::~NM_Publish_Interaction_Class() {
}
void NM_Publish_Interaction_Class::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_int32(interactionClass);
} /* end of serialize */ 
void NM_Publish_Interaction_Class::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	interactionClass = msgBuffer.read_int32();
} /* end of deserialize */
/*<END>---------- Publish_Interaction_Class ------------<END>*/

/*<BEGIN>---------- Unpublish_Interaction_Class ------------<BEGIN>*/
NM_Unpublish_Interaction_Class::NM_Unpublish_Interaction_Class() {
	this->name = "UNPUBLISH_INTERACTION_CLASS";
	this->type = NetworkMessage::UNPUBLISH_INTERACTION_CLASS;
	/* specific field init */
}
NM_Unpublish_Interaction_Class::~NM_Unpublish_Interaction_Class() {
}
/*<END>---------- Unpublish_Interaction_Class ------------<END>*/

/*<BEGIN>---------- Subscribe_Object_Class ------------<BEGIN>*/
NM_Subscribe_Object_Class::NM_Subscribe_Object_Class() {
	this->name = "SUBSCRIBE_OBJECT_CLASS";
	this->type = NetworkMessage::SUBSCRIBE_OBJECT_CLASS;
	/* specific field init */
}
NM_Subscribe_Object_Class::~NM_Subscribe_Object_Class() {
}
void NM_Subscribe_Object_Class::serialize(MessageBuffer& msgBuffer) {
	Super::serialize(msgBuffer);
	msgBuffer.write_int32(objectClass);
}
void NM_Subscribe_Object_Class::deserialize(MessageBuffer& msgBuffer) {
	Super::deserialize(msgBuffer);
	objectClass=msgBuffer.read_int32();
}
/*<END>---------- Subscribe_Object_Class ------------<END>*/

/*<BEGIN>---------- Unsubscribe_Object_Class ------------<BEGIN>*/
NM_Unsubscribe_Object_Class::NM_Unsubscribe_Object_Class() {
	this->name = "UNSUBSCRIBE_OBJECT_CLASS";
	this->type = NetworkMessage::UNSUBSCRIBE_OBJECT_CLASS;
	/* specific field init */
}
NM_Unsubscribe_Object_Class::~NM_Unsubscribe_Object_Class() {
}
/*<END>---------- Unsubscribe_Object_Class ------------<END>*/

/*<BEGIN>---------- Subscribe_Interaction_Class ------------<BEGIN>*/
NM_Subscribe_Interaction_Class::NM_Subscribe_Interaction_Class() {
	this->name = "SUBSCRIBE_INTERACTION_CLASS";
	this->type = NetworkMessage::SUBSCRIBE_INTERACTION_CLASS;
	/* specific field init */
}
NM_Subscribe_Interaction_Class::~NM_Subscribe_Interaction_Class() {
}
/*<END>---------- Subscribe_Interaction_Class ------------<END>*/

/*<BEGIN>---------- Unsubscribe_Interaction_Class ------------<BEGIN>*/
NM_Unsubscribe_Interaction_Class::NM_Unsubscribe_Interaction_Class() {
	this->name = "UNSUBSCRIBE_INTERACTION_CLASS";
	this->type = NetworkMessage::UNSUBSCRIBE_INTERACTION_CLASS;
	/* specific field init */
}
NM_Unsubscribe_Interaction_Class::~NM_Unsubscribe_Interaction_Class() {
}
/*<END>---------- Unsubscribe_Interaction_Class ------------<END>*/

/*<BEGIN>---------- Turn_Interactions_On ------------<BEGIN>*/
NM_Turn_Interactions_On::NM_Turn_Interactions_On() {
	this->name = "TURN_INTERACTIONS_ON";
	this->type = NetworkMessage::TURN_INTERACTIONS_ON;
	/* specific field init */
}
NM_Turn_Interactions_On::~NM_Turn_Interactions_On() {
}
/*<END>---------- Turn_Interactions_On ------------<END>*/

/*<BEGIN>---------- Turn_Interactions_Off ------------<BEGIN>*/
NM_Turn_Interactions_Off::NM_Turn_Interactions_Off() {
	this->name = "TURN_INTERACTIONS_OFF";
	this->type = NetworkMessage::TURN_INTERACTIONS_OFF;
	/* specific field init */
}
NM_Turn_Interactions_Off::~NM_Turn_Interactions_Off() {
}
/*<END>---------- Turn_Interactions_Off ------------<END>*/

/*<BEGIN>---------- Register_Object ------------<BEGIN>*/
NM_Register_Object::NM_Register_Object() {
	this->name = "REGISTER_OBJECT";
	this->type = NetworkMessage::REGISTER_OBJECT;
	/* specific field init */
}
NM_Register_Object::~NM_Register_Object() {
}
void NM_Register_Object::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_int32(objectClass);
	msgBuffer.write_int32(object);
} /* end of serialize */ 
void NM_Register_Object::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	objectClass = msgBuffer.read_int32();
	object      = msgBuffer.read_int32();
} /* end of deserialize */
/*<END>---------- Register_Object ------------<END>*/

/*<BEGIN>---------- Discover_Object ------------<BEGIN>*/
NM_Discover_Object::NM_Discover_Object() {
	this->name = "DISCOVER_OBJECT";
	this->type = NetworkMessage::DISCOVER_OBJECT;
	/* specific field init */
}
NM_Discover_Object::~NM_Discover_Object() {
}
/*<END>---------- Discover_Object ------------<END>*/

/*<BEGIN>---------- Update_Attribute_Values ------------<BEGIN>*/
NM_Update_Attribute_Values::NM_Update_Attribute_Values() {
	this->name = "UPDATE_ATTRIBUTE_VALUES";
	this->type = NetworkMessage::UPDATE_ATTRIBUTE_VALUES;
	/* specific field init */
}
NM_Update_Attribute_Values::~NM_Update_Attribute_Values() {
}
void NM_Update_Attribute_Values::serialize(MessageBuffer& msgBuffer) {
	int i;
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* handleArraySize was done by superclass */
	/* specific code (if any) goes here */
	msgBuffer.write_int32(object);	
	msgBuffer.write_int32(objectClass);
	/* the value pre-encoded by the user (HLA 1.3) */
	for (i = 0 ; i < handleArraySize ; i++)
		msgBuffer.write_string(valueArray[i]) ;
} /* end of serialize */ 
void NM_Update_Attribute_Values::deserialize(MessageBuffer& msgBuffer) {
	int i;
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* handleArraySize was done by superclass */
	/* specific code (if any) goes here */		
	object          = msgBuffer.read_int32();	
	objectClass     = msgBuffer.read_int32();
    valueArray.resize(handleArraySize) ;	
    for (i = 0; i < handleArraySize; i++)
        msgBuffer.read_string(valueArray[i]);
} /* end of deserialize */
/*<END>---------- Update_Attribute_Values ------------<END>*/

/*<BEGIN>---------- Reflect_Attribute_Values ------------<BEGIN>*/
NM_Reflect_Attribute_Values::NM_Reflect_Attribute_Values() {
	this->name = "REFLECT_ATTRIBUTE_VALUES";
	this->type = NetworkMessage::REFLECT_ATTRIBUTE_VALUES;
	/* specific field init */
}
NM_Reflect_Attribute_Values::~NM_Reflect_Attribute_Values() {
}
/*<END>---------- Reflect_Attribute_Values ------------<END>*/

/*<BEGIN>---------- Send_Interaction ------------<BEGIN>*/
NM_Send_Interaction::NM_Send_Interaction() {
	this->name = "SEND_INTERACTION";
	this->type = NetworkMessage::SEND_INTERACTION;
	/* specific field init */
}
NM_Send_Interaction::~NM_Send_Interaction() {
}
void NM_Send_Interaction::serialize(MessageBuffer& msgBuffer) {
	int i;
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_int32(region);
	msgBuffer.write_int32(interactionClass);	
	/* the value pre-encoded by the user (HLA 1.3) */
	for (i = 0 ; i < handleArraySize ; i++)
		msgBuffer.write_string(valueArray[i]);
} /* end of serialize */ 
void NM_Send_Interaction::deserialize(MessageBuffer& msgBuffer) {
	int i;
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	region           = msgBuffer.read_int32();
	interactionClass = msgBuffer.read_int32();
    valueArray.resize(handleArraySize);
    for (i = 0; i < handleArraySize; i++)
        msgBuffer.read_string(valueArray[i]);
} /* end of deserialize */
/*<END>---------- Send_Interaction ------------<END>*/

/*<BEGIN>---------- Receive_Interaction ------------<BEGIN>*/
NM_Receive_Interaction::NM_Receive_Interaction() {
	this->name = "RECEIVE_INTERACTION";
	this->type = NetworkMessage::RECEIVE_INTERACTION;
	/* specific field init */
}
NM_Receive_Interaction::~NM_Receive_Interaction() {
}
/*<END>---------- Receive_Interaction ------------<END>*/

/*<BEGIN>---------- Delete_Object ------------<BEGIN>*/
NM_Delete_Object::NM_Delete_Object() {
	this->name = "DELETE_OBJECT";
	this->type = NetworkMessage::DELETE_OBJECT;
	/* specific field init */
}
NM_Delete_Object::~NM_Delete_Object() {
}
/*<END>---------- Delete_Object ------------<END>*/

/*<BEGIN>---------- Remove_Object ------------<BEGIN>*/
NM_Remove_Object::NM_Remove_Object() {
	this->name = "REMOVE_OBJECT";
	this->type = NetworkMessage::REMOVE_OBJECT;
	/* specific field init */
}
NM_Remove_Object::~NM_Remove_Object() {
}

/*<END>---------- Remove_Object ------------<END>*/

/*<BEGIN>---------- Change_Attribute_Transport_Type ------------<BEGIN>*/
NM_Change_Attribute_Transport_Type::NM_Change_Attribute_Transport_Type() {
	this->name = "CHANGE_ATTRIBUTE_TRANSPORT_TYPE";
	this->type = NetworkMessage::CHANGE_ATTRIBUTE_TRANSPORT_TYPE;
	/* specific field init */
}
NM_Change_Attribute_Transport_Type::~NM_Change_Attribute_Transport_Type() {
}
void NM_Change_Attribute_Transport_Type::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
} /* end of serialize */ 
void NM_Change_Attribute_Transport_Type::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
} /* end of deserialize */
/*<END>---------- Change_Attribute_Transport_Type ------------<END>*/

/*<BEGIN>---------- Change_Attribute_Order_Type ------------<BEGIN>*/
NM_Change_Attribute_Order_Type::NM_Change_Attribute_Order_Type() {
	this->name = "CHANGE_ATTRIBUTE_ORDER_TYPE";
	this->type = NetworkMessage::CHANGE_ATTRIBUTE_ORDER_TYPE;
	/* specific field init */
}
NM_Change_Attribute_Order_Type::~NM_Change_Attribute_Order_Type() {
}
void NM_Change_Attribute_Order_Type::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
} /* end of serialize */ 
void NM_Change_Attribute_Order_Type::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
} /* end of deserialize */
/*<END>---------- Change_Attribute_Order_Type ------------<END>*/

/*<BEGIN>---------- Change_Interaction_Transport_Type ------------<BEGIN>*/
NM_Change_Interaction_Transport_Type::NM_Change_Interaction_Transport_Type() {
	this->name = "CHANGE_INTERACTION_TRANSPORT_TYPE";
	this->type = NetworkMessage::CHANGE_INTERACTION_TRANSPORT_TYPE;
	/* specific field init */
}
NM_Change_Interaction_Transport_Type::~NM_Change_Interaction_Transport_Type() {
}
void NM_Change_Interaction_Transport_Type::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
} /* end of serialize */ 
void NM_Change_Interaction_Transport_Type::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
} /* end of deserialize */
/*<END>---------- Change_Interaction_Transport_Type ------------<END>*/

/*<BEGIN>---------- Change_Interaction_Order_Type ------------<BEGIN>*/
NM_Change_Interaction_Order_Type::NM_Change_Interaction_Order_Type() {
	this->name = "CHANGE_INTERACTION_ORDER_TYPE";
	this->type = NetworkMessage::CHANGE_INTERACTION_ORDER_TYPE;
	/* specific field init */
}
NM_Change_Interaction_Order_Type::~NM_Change_Interaction_Order_Type() {
}
void NM_Change_Interaction_Order_Type::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
} /* end of serialize */ 
void NM_Change_Interaction_Order_Type::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
} /* end of deserialize */
/*<END>---------- Change_Interaction_Order_Type ------------<END>*/

/*<BEGIN>---------- Request_Class_Attribute_Value_Update ------------<BEGIN>*/
NM_Request_Class_Attribute_Value_Update::NM_Request_Class_Attribute_Value_Update() {
	this->name = "REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE";
	this->type = NetworkMessage::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE;
	/* specific field init */
}
NM_Request_Class_Attribute_Value_Update::~NM_Request_Class_Attribute_Value_Update() {
}
void NM_Request_Class_Attribute_Value_Update::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
} /* end of serialize */ 
void NM_Request_Class_Attribute_Value_Update::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
} /* end of deserialize */
/*<END>---------- Request_Class_Attribute_Value_Update ------------<END>*/

/*<BEGIN>---------- Request_Object_Attribute_Value_Update ------------<BEGIN>*/
NM_Request_Object_Attribute_Value_Update::NM_Request_Object_Attribute_Value_Update() {
	this->name = "REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE";
	this->type = NetworkMessage::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE;
	/* specific field init */
}
NM_Request_Object_Attribute_Value_Update::~NM_Request_Object_Attribute_Value_Update() {
}
void NM_Request_Object_Attribute_Value_Update::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_int32(object);
} /* end of serialize */ 
void NM_Request_Object_Attribute_Value_Update::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	object  = msgBuffer.read_int32();
} /* end of deserialize */
/*<END>---------- Request_Object_Attribute_Value_Update ------------<END>*/

/*<BEGIN>---------- Is_Attribute_Owned_By_Federate ------------<BEGIN>*/
NM_Is_Attribute_Owned_By_Federate::NM_Is_Attribute_Owned_By_Federate() {
	this->name = "IS_ATTRIBUTE_OWNED_BY_FEDERATE";
	this->type = NetworkMessage::IS_ATTRIBUTE_OWNED_BY_FEDERATE;
	/* specific field init */
}
NM_Is_Attribute_Owned_By_Federate::~NM_Is_Attribute_Owned_By_Federate() {
}
void NM_Is_Attribute_Owned_By_Federate::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_int32(object);
	msgBuffer.write_int32(attribute);
} /* end of serialize */ 
void NM_Is_Attribute_Owned_By_Federate::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	Super::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	object    = msgBuffer.read_int32();
	attribute = msgBuffer.read_int32();
} /* end of deserialize */
/*<END>---------- Is_Attribute_Owned_By_Federate ------------<END>*/

/*<BEGIN>---------- Query_Attribute_Ownership ------------<BEGIN>*/
NM_Query_Attribute_Ownership::NM_Query_Attribute_Ownership() {
	this->name = "QUERY_ATTRIBUTE_OWNERSHIP";
	this->type = NetworkMessage::QUERY_ATTRIBUTE_OWNERSHIP;
	/* specific field init */
}
NM_Query_Attribute_Ownership::~NM_Query_Attribute_Ownership() {
}
/*<END>---------- Query_Attribute_Ownership ------------<END>*/

/*<BEGIN>---------- Attribute_Is_Not_Owned ------------<BEGIN>*/
NM_Attribute_Is_Not_Owned::NM_Attribute_Is_Not_Owned() {
	this->name = "ATTRIBUTE_IS_NOT_OWNED";
	this->type = NetworkMessage::ATTRIBUTE_IS_NOT_OWNED;
	/* specific field init */
}
NM_Attribute_Is_Not_Owned::~NM_Attribute_Is_Not_Owned() {
}
/*<END>---------- Attribute_Is_Not_Owned ------------<END>*/

/*<BEGIN>---------- Inform_Attribute_Ownership ------------<BEGIN>*/
NM_Inform_Attribute_Ownership::NM_Inform_Attribute_Ownership() {
	this->name = "INFORM_ATTRIBUTE_OWNERSHIP";
	this->type = NetworkMessage::INFORM_ATTRIBUTE_OWNERSHIP;
	/* specific field init */
}
NM_Inform_Attribute_Ownership::~NM_Inform_Attribute_Ownership() {
}
/*<END>---------- Inform_Attribute_Ownership ------------<END>*/

/*<BEGIN>---------- Negotiated_Attribute_Ownership_Divestiture ------------<BEGIN>*/
NM_Negotiated_Attribute_Ownership_Divestiture::NM_Negotiated_Attribute_Ownership_Divestiture() {
	this->name = "NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE";
	this->type = NetworkMessage::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
	/* specific field init */
}
NM_Negotiated_Attribute_Ownership_Divestiture::~NM_Negotiated_Attribute_Ownership_Divestiture() {
}
/*<END>---------- Negotiated_Attribute_Ownership_Divestiture ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Acquisition_Notification ------------<BEGIN>*/
NM_Attribute_Ownership_Acquisition_Notification::NM_Attribute_Ownership_Acquisition_Notification() {
	this->name = "ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION";
	this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION;
	/* specific field init */
}
NM_Attribute_Ownership_Acquisition_Notification::~NM_Attribute_Ownership_Acquisition_Notification() {
}
/*<END>---------- Attribute_Ownership_Acquisition_Notification ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Divestiture_Notification ------------<BEGIN>*/
NM_Attribute_Ownership_Divestiture_Notification::NM_Attribute_Ownership_Divestiture_Notification() {
	this->name = "ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION";
	this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION;
	/* specific field init */
}
NM_Attribute_Ownership_Divestiture_Notification::~NM_Attribute_Ownership_Divestiture_Notification() {
}
/*<END>---------- Attribute_Ownership_Divestiture_Notification ------------<END>*/

/*<BEGIN>---------- Request_Attribute_Ownership_Assumption ------------<BEGIN>*/
NM_Request_Attribute_Ownership_Assumption::NM_Request_Attribute_Ownership_Assumption() {
	this->name = "REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION";
	this->type = NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION;
	/* specific field init */
}
NM_Request_Attribute_Ownership_Assumption::~NM_Request_Attribute_Ownership_Assumption() {
}
/*<END>---------- Request_Attribute_Ownership_Assumption ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Unavailable ------------<BEGIN>*/
NM_Attribute_Ownership_Unavailable::NM_Attribute_Ownership_Unavailable() {
	this->name = "ATTRIBUTE_OWNERSHIP_UNAVAILABLE";
	this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_UNAVAILABLE;
	/* specific field init */
}
NM_Attribute_Ownership_Unavailable::~NM_Attribute_Ownership_Unavailable() {
}
/*<END>---------- Attribute_Ownership_Unavailable ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Acquisition_If_Available ------------<BEGIN>*/
NM_Attribute_Ownership_Acquisition_If_Available::NM_Attribute_Ownership_Acquisition_If_Available() {
	this->name = "ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE";
	this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE;
	/* specific field init */
}
NM_Attribute_Ownership_Acquisition_If_Available::~NM_Attribute_Ownership_Acquisition_If_Available() {
}
/*<END>---------- Attribute_Ownership_Acquisition_If_Available ------------<END>*/

/*<BEGIN>---------- Unconditional_Attribute_Ownership_Divestiture ------------<BEGIN>*/
NM_Unconditional_Attribute_Ownership_Divestiture::NM_Unconditional_Attribute_Ownership_Divestiture() {
	this->name = "UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE";
	this->type = NetworkMessage::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
	/* specific field init */
}
NM_Unconditional_Attribute_Ownership_Divestiture::~NM_Unconditional_Attribute_Ownership_Divestiture() {
}
/*<END>---------- Unconditional_Attribute_Ownership_Divestiture ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Acquisition ------------<BEGIN>*/
NM_Attribute_Ownership_Acquisition::NM_Attribute_Ownership_Acquisition() {
	this->name = "ATTRIBUTE_OWNERSHIP_ACQUISITION";
	this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION;
	/* specific field init */
}
NM_Attribute_Ownership_Acquisition::~NM_Attribute_Ownership_Acquisition() {
}
/*<END>---------- Attribute_Ownership_Acquisition ------------<END>*/

/*<BEGIN>---------- Request_Attribute_Ownership_Release ------------<BEGIN>*/
NM_Request_Attribute_Ownership_Release::NM_Request_Attribute_Ownership_Release() {
	this->name = "REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE";
	this->type = NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE;
	/* specific field init */
}
NM_Request_Attribute_Ownership_Release::~NM_Request_Attribute_Ownership_Release() {
}
/*<END>---------- Request_Attribute_Ownership_Release ------------<END>*/

/*<BEGIN>---------- Cancel_Negotiated_Attribute_Ownership_Divestiture ------------<BEGIN>*/
NM_Cancel_Negotiated_Attribute_Ownership_Divestiture::NM_Cancel_Negotiated_Attribute_Ownership_Divestiture() {
	this->name = "CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE";
	this->type = NetworkMessage::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
	/* specific field init */
}
NM_Cancel_Negotiated_Attribute_Ownership_Divestiture::~NM_Cancel_Negotiated_Attribute_Ownership_Divestiture() {
}
/*<END>---------- Cancel_Negotiated_Attribute_Ownership_Divestiture ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Release_Response ------------<BEGIN>*/
NM_Attribute_Ownership_Release_Response::NM_Attribute_Ownership_Release_Response() {
	this->name = "ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE";
	this->type = NetworkMessage::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE;
	/* specific field init */
}
NM_Attribute_Ownership_Release_Response::~NM_Attribute_Ownership_Release_Response() {
}
/*<END>---------- Attribute_Ownership_Release_Response ------------<END>*/

/*<BEGIN>---------- Cancel_Attribute_Ownership_Acquisition ------------<BEGIN>*/
NM_Cancel_Attribute_Ownership_Acquisition::NM_Cancel_Attribute_Ownership_Acquisition() {
	this->name = "CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION";
	this->type = NetworkMessage::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION;
	/* specific field init */
}
NM_Cancel_Attribute_Ownership_Acquisition::~NM_Cancel_Attribute_Ownership_Acquisition() {
}
/*<END>---------- Cancel_Attribute_Ownership_Acquisition ------------<END>*/

/*<BEGIN>---------- Confirm_Attribute_Ownership_Acquisition_Cancellation ------------<BEGIN>*/
NM_Confirm_Attribute_Ownership_Acquisition_Cancellation::NM_Confirm_Attribute_Ownership_Acquisition_Cancellation() {
	this->name = "CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION";
	this->type = NetworkMessage::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION;
	/* specific field init */
}
NM_Confirm_Attribute_Ownership_Acquisition_Cancellation::~NM_Confirm_Attribute_Ownership_Acquisition_Cancellation() {
}
/*<END>---------- Confirm_Attribute_Ownership_Acquisition_Cancellation ------------<END>*/

/*<BEGIN>---------- DDM_Create_Region ------------<BEGIN>*/
NM_DDM_Create_Region::NM_DDM_Create_Region() {
	this->name = "DDM_CREATE_REGION";
	this->type = NetworkMessage::DDM_CREATE_REGION;
	/* specific field init */
}
NM_DDM_Create_Region::~NM_DDM_Create_Region() {
}
void NM_DDM_Create_Region::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	NetworkMessage::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_int32(space);
	msgBuffer.write_int32(nbExtents);
	msgBuffer.write_int32(region);
} /* end of serialize */ 
void NM_DDM_Create_Region::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	NetworkMessage::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	space     = msgBuffer.read_int32();
	nbExtents = msgBuffer.read_int32();
	region    = msgBuffer.read_int32();
} /* end of deserialize */
/*<END>---------- DDM_Create_Region ------------<END>*/

/*<BEGIN>---------- DDM_Modify_Region ------------<BEGIN>*/
NM_DDM_Modify_Region::NM_DDM_Modify_Region() {
	this->name = "DDM_MODIFY_REGION";
	this->type = NetworkMessage::DDM_MODIFY_REGION;
	/* specific field init */
}
NM_DDM_Modify_Region::~NM_DDM_Modify_Region() {
}
void NM_DDM_Modify_Region::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	NetworkMessage::serialize(msgBuffer);	
	/* specific code (if any) goes here */
	msgBuffer.write_int32(federation);
	msgBuffer.write_int32(federate);
	msgBuffer.write_int32(region);
	writeExtents(msgBuffer);
} /* end of serialize */ 
void NM_DDM_Modify_Region::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	NetworkMessage::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	federation = msgBuffer.read_int32();
	federate   = msgBuffer.read_int32();
	region     = msgBuffer.read_int32();
	readExtents(msgBuffer);
} /* end of deserialize */
/*<END>---------- DDM_Modify_Region ------------<END>*/

/*<BEGIN>---------- DDM_Delete_Region ------------<BEGIN>*/
NM_DDM_Delete_Region::NM_DDM_Delete_Region() {
	this->name = "DDM_DELETE_REGION";
	this->type = NetworkMessage::DDM_DELETE_REGION;
	/* specific field init */
}
NM_DDM_Delete_Region::~NM_DDM_Delete_Region() {
}
void NM_DDM_Delete_Region::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	NetworkMessage::serialize(msgBuffer); 
	/* specific code (if any) goes here */
	msgBuffer.write_int32(federation);
	msgBuffer.write_int32(federate);
	msgBuffer.write_int32(region);
} /* end of serialize */ 
void NM_DDM_Delete_Region::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	NetworkMessage::deserialize(msgBuffer); 
	/* specific code (if any) goes here */
	federation = msgBuffer.read_int32();
	federate   = msgBuffer.read_int32();
	region     = msgBuffer.read_int32();
} /* end of deserialize */
/*<END>---------- DDM_Delete_Region ------------<END>*/

/*<BEGIN>---------- DDM_Associate_Region ------------<BEGIN>*/
NM_DDM_Associate_Region::NM_DDM_Associate_Region() {
	this->name = "DDM_ASSOCIATE_REGION";
	this->type = NetworkMessage::DDM_ASSOCIATE_REGION;
	/* specific field init */
}
NM_DDM_Associate_Region::~NM_DDM_Associate_Region() {
}
void NM_DDM_Associate_Region::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	NM_WithHandleArray::serialize(msgBuffer);
	/* specific code (if any) goes here */
	msgBuffer.write_int32(object);
	msgBuffer.write_int32(region);
	msgBuffer.write_bool(DDM_bool);
} /* end of serialize */ 
void NM_DDM_Associate_Region::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	NM_WithHandleArray::deserialize(msgBuffer);
	/* specific code (if any) goes here */
	object  = msgBuffer.read_int32();
	region  = msgBuffer.read_int32();
	DDM_bool = msgBuffer.read_bool();
} /* end of deserialize */
/*<END>---------- DDM_Associate_Region ------------<END>*/

/*<BEGIN>---------- DDM_Register_Object ------------<BEGIN>*/
NM_DDM_Register_Object::NM_DDM_Register_Object() {
	this->name = "DDM_REGISTER_OBJECT";
	this->type = NetworkMessage::DDM_REGISTER_OBJECT;
	/* specific field init */
}
NM_DDM_Register_Object::~NM_DDM_Register_Object() {
}
void NM_DDM_Register_Object::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	NM_WithHandleArray::serialize(msgBuffer);
	/* specific code (if any) goes here */
	msgBuffer.write_int32(object);
	msgBuffer.write_int32(objectClass);
	msgBuffer.write_int32(region);
	msgBuffer.write_bool(DDM_bool);
} /* end of serialize */ 
void NM_DDM_Register_Object::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	NM_WithHandleArray::deserialize(msgBuffer);
	/* specific code (if any) goes here */
	object      = msgBuffer.read_int32();
	objectClass = msgBuffer.read_int32();
	region      = msgBuffer.read_int32();
	DDM_bool     = msgBuffer.read_bool();
} /* end of deserialize */
/*<END>---------- DDM_Register_Object ------------<END>*/

/*<BEGIN>---------- DDM_Unassociate_Region ------------<BEGIN>*/
NM_DDM_Unassociate_Region::NM_DDM_Unassociate_Region() {
	this->name = "DDM_UNASSOCIATE_REGION";
	this->type = NetworkMessage::DDM_UNASSOCIATE_REGION;
	/* specific field init */
}
NM_DDM_Unassociate_Region::~NM_DDM_Unassociate_Region() {
}
void NM_DDM_Unassociate_Region::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	NetworkMessage::serialize(msgBuffer);
	/* specific code (if any) goes here */
	msgBuffer.write_int32(object);
	msgBuffer.write_int32(region);
} /* end of serialize */ 
void NM_DDM_Unassociate_Region::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      	
	NetworkMessage::deserialize(msgBuffer);
	/* specific code (if any) goes here */
	object = msgBuffer.read_int32();
	region = msgBuffer.read_int32();
} /* end of deserialize */
/*<END>---------- DDM_Unassociate_Region ------------<END>*/

/*<BEGIN>---------- DDM_Subscribe_Attributes ------------<BEGIN>*/
NM_DDM_Subscribe_Attributes::NM_DDM_Subscribe_Attributes() {
	this->name = "DDM_SUBSCRIBE_ATTRIBUTES";
	this->type = NetworkMessage::DDM_SUBSCRIBE_ATTRIBUTES;
	/* specific field init */
}
NM_DDM_Subscribe_Attributes::~NM_DDM_Subscribe_Attributes() {
}
void NM_DDM_Subscribe_Attributes::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	NM_WithHandleArray::serialize(msgBuffer);
	/* specific code (if any) goes here */
	msgBuffer.write_int32(objectClass);
	msgBuffer.write_int32(region);
	msgBuffer.write_bool(DDM_bool);
} /* end of serialize */ 
void NM_DDM_Subscribe_Attributes::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	NM_WithHandleArray::deserialize(msgBuffer);
	/* specific code (if any) goes here */
	objectClass = msgBuffer.read_int32();
	region      = msgBuffer.read_int32();
	DDM_bool     = msgBuffer.read_bool();
} /* end of deserialize */
/*<END>---------- DDM_Subscribe_Attributes ------------<END>*/

/*<BEGIN>---------- DDM_Unsubscribe_Attributes ------------<BEGIN>*/
NM_DDM_Unsubscribe_Attributes::NM_DDM_Unsubscribe_Attributes() {
	this->name = "DDM_UNSUBSCRIBE_ATTRIBUTES";
	this->type = NetworkMessage::DDM_UNSUBSCRIBE_ATTRIBUTES;
	/* specific field init */
}
NM_DDM_Unsubscribe_Attributes::~NM_DDM_Unsubscribe_Attributes() {
}
void NM_DDM_Unsubscribe_Attributes::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	NetworkMessage::serialize(msgBuffer);
	/* specific code (if any) goes here */
	msgBuffer.write_int32(objectClass);
	msgBuffer.write_int32(region);
} /* end of serialize */ 
void NM_DDM_Unsubscribe_Attributes::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	NetworkMessage::deserialize(msgBuffer);
	/* specific code (if any) goes here */
	objectClass = msgBuffer.read_int32();
	region = msgBuffer.read_int32();
} /* end of deserialize */
/*<END>---------- DDM_Unsubscribe_Attributes ------------<END>*/

/*<BEGIN>---------- DDM_Subscribe_Interaction ------------<BEGIN>*/
NM_DDM_Subscribe_Interaction::NM_DDM_Subscribe_Interaction() {
	this->name = "DDM_SUBSCRIBE_INTERACTION";
	this->type = NetworkMessage::DDM_SUBSCRIBE_INTERACTION;
	/* specific field init */
}
NM_DDM_Subscribe_Interaction::~NM_DDM_Subscribe_Interaction() {
}
void NM_DDM_Subscribe_Interaction::serialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	NetworkMessage::serialize(msgBuffer);
	/* specific code (if any) goes here */
	msgBuffer.write_int32(interactionClass);
	msgBuffer.write_int32(region);
	msgBuffer.write_bool(DDM_bool);
} /* end of serialize */ 
void NM_DDM_Subscribe_Interaction::deserialize(MessageBuffer& msgBuffer) {
	/* call mother class */      
	NetworkMessage::deserialize(msgBuffer);
	/* specific code (if any) goes here */
	interactionClass = msgBuffer.read_int32();
	region           = msgBuffer.read_int32();
	DDM_bool           = msgBuffer.read_bool();
} /* end of deserialize */
/*<END>---------- DDM_Subscribe_Interaction ------------<END>*/

/*<BEGIN>---------- DDM_Unsubscribe_Interaction ------------<BEGIN>*/
NM_DDM_Unsubscribe_Interaction::NM_DDM_Unsubscribe_Interaction() {
	this->name = "DDM_UNSUBSCRIBE_INTERACTION";
	this->type = NetworkMessage::DDM_UNSUBSCRIBE_INTERACTION;
	/* specific field init */
}
NM_DDM_Unsubscribe_Interaction::~NM_DDM_Unsubscribe_Interaction() {
}
/*<END>---------- DDM_Unsubscribe_Interaction ------------<END>*/

/*<BEGIN>---------- Provide_Attribute_Value_Update ------------<BEGIN>*/
NM_Provide_Attribute_Value_Update::NM_Provide_Attribute_Value_Update() {
	this->name = "PROVIDE_ATTRIBUTE_VALUE_UPDATE";
	this->type = NetworkMessage::PROVIDE_ATTRIBUTE_VALUE_UPDATE;
	/* specific field init */
}
NM_Provide_Attribute_Value_Update::~NM_Provide_Attribute_Value_Update() {
}
/*<END>---------- Provide_Attribute_Value_Update ------------<END>*/

/*<BEGIN>---------- Last ------------<BEGIN>*/
NM_Last::NM_Last() {
	this->name = "LAST";
	this->type = NetworkMessage::LAST;
	/* specific field init */
}
NM_Last::~NM_Last() {
}
/*<END>---------- Last ------------<END>*/

} /* end of certi namespace */
