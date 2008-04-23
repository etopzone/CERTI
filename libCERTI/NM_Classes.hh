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
#include <vector>

namespace certi {
/**
 * Network Message factory
 */
class CERTI_EXPORT NM_Factory {
public:
	static NetworkMessage* create(NetworkMessage::Message_T type) throw (RTIinternalError);
	static NetworkMessage* receive(Socket* socket) throw (NetworkError,RTIinternalError);
};

class CERTI_EXPORT NM_WithHandleArray : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_WithHandleArray();
	virtual ~NM_WithHandleArray();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);	
	/* specific Getter/Setter */	
	AttributeHandle getHandle(int i) {return handleArray[i];};
	void setHandle(AttributeHandle h, int i) {handleArray[i]=h;};
protected:
	/* specific field */
private:
};

class CERTI_EXPORT NM_DDM_Base : public NM_WithHandleArray {
public:
	typedef NM_WithHandleArray Super;
	NM_DDM_Base();
	virtual ~NM_DDM_Base();
	virtual void serialize(MessageBuffer& msgBuffer)   = 0;
	virtual void deserialize(MessageBuffer& msgBuffer) = 0;	
	/* specific Getter/Setter */	
	const SpaceHandle getSpace() const {return space;};
	void setSpace(SpaceHandle space) {this->space=space;};

	const int32_t geNbExtents() const {return nbExtents;};
	void setNbExtents(int32_t nbExtents) {this->nbExtents=nbExtents;};

	const int32_t getRegion() const {return region;};
	void setRegion(int32_t region) {this->region=region;};	

	const ObjectHandle getObject() const {return object;};
	void setObject(ObjectHandle object) {this->object=object;};

	const ObjectClassHandle getObjectClass() const {return objectClass;};
	void setObjectClass(ObjectClassHandle objectClass) {this->objectClass=objectClass;};

	const InteractionClassHandle getInteractionClass() const {return interactionClass;};
	void setInteractionClass(InteractionClassHandle interactionClass) {this->interactionClass=interactionClass;};
protected:
	/* specific field */
private:
};


/*<BEGIN>---------- Not_Used ------------<BEGIN>*/
class CERTI_EXPORT NM_Not_Used : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Not_Used();
	virtual ~NM_Not_Used();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Not_Used ------------<END>*/

/*<BEGIN>---------- Close_Connexion ------------<BEGIN>*/
class CERTI_EXPORT NM_Close_Connexion : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Close_Connexion();
	virtual ~NM_Close_Connexion();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Close_Connexion ------------<END>*/

/*<BEGIN>---------- Message_Null ------------<BEGIN>*/
class CERTI_EXPORT NM_Message_Null : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Message_Null();
	virtual ~NM_Message_Null();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Message_Null ------------<END>*/

/*<BEGIN>---------- Create_Federation_Execution ------------<BEGIN>*/
class CERTI_EXPORT NM_Create_Federation_Execution : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Create_Federation_Execution();
	virtual ~NM_Create_Federation_Execution();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */	  
	//	std::string federationName ;
	//	std::string FEDid ;
private:
};

/*<END>---------- Create_Federation_Execution ------------<END>*/

/*<BEGIN>---------- Destroy_Federation_Execution ------------<BEGIN>*/
class CERTI_EXPORT NM_Destroy_Federation_Execution : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Destroy_Federation_Execution();
	virtual ~NM_Destroy_Federation_Execution();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */	 
	//	std::string federationName;
private:
};

/*<END>---------- Destroy_Federation_Execution ------------<END>*/

/*<BEGIN>---------- Join_Federation_Execution ------------<BEGIN>*/
class CERTI_EXPORT NM_Join_Federation_Execution : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Join_Federation_Execution();
	virtual ~NM_Join_Federation_Execution();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
	//	int32_t  numberOfRegulators;
	//	uint32_t multicastAddress;
	//	uint32_t bestEffortAddress;
	//	uint32_t bestEffortPeer;
	//	std::string federationName;
	//	std::string federateName;
private:
};

/*<END>---------- Join_Federation_Execution ------------<END>*/

/*<BEGIN>---------- Resign_Federation_Execution ------------<BEGIN>*/
class CERTI_EXPORT NM_Resign_Federation_Execution : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Resign_Federation_Execution();
	virtual ~NM_Resign_Federation_Execution();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Resign_Federation_Execution ------------<END>*/

/*<BEGIN>---------- Set_Time_Regulating ------------<BEGIN>*/
class CERTI_EXPORT NM_Set_Time_Regulating : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Set_Time_Regulating();
	virtual ~NM_Set_Time_Regulating();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
	void regulatorOn() {regulator=true;};
	void regulatorOff() {regulator=false;};	
	bool isRegulator() {return regulator;};
protected:
	/* specific field */
	/**
	 * Toggle which indicates if federate
	 * wants to become regulator or leave
	 * its regulator state
	 */
	bool regulator ;
private:
};

/*<END>---------- Set_Time_Regulating ------------<END>*/

/*<BEGIN>---------- Set_Time_Constrained ------------<BEGIN>*/
class CERTI_EXPORT NM_Set_Time_Constrained : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Set_Time_Constrained();
	virtual ~NM_Set_Time_Constrained();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
	void constrainedOn() {constrained=true;};
	void constrainedOff() {constrained=false;};		
	bool isConstrained() {return constrained;};
protected:
	/* specific field */	
	/**
	 * Toggle which indicates if federate
	 * wants to become constrained or leave
	 * its constrained state
	 */
	bool constrained;
private:
};

/*<END>---------- Set_Time_Constrained ------------<END>*/

/*<BEGIN>---------- Register_Federation_Synchronization_Point ------------<BEGIN>*/
class CERTI_EXPORT NM_Register_Federation_Synchronization_Point : public NM_WithHandleArray {
public:
	typedef NM_WithHandleArray Super;
	NM_Register_Federation_Synchronization_Point();
	virtual ~NM_Register_Federation_Synchronization_Point();	
	/* specific Getter/Setter */
protected:
	/* specific field */	  
private:
};

/*<END>---------- Register_Federation_Synchronization_Point ------------<END>*/

/*<BEGIN>---------- Synchronization_Point_Registration_Succeeded ------------<BEGIN>*/
class CERTI_EXPORT NM_Synchronization_Point_Registration_Succeeded : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Synchronization_Point_Registration_Succeeded();
	virtual ~NM_Synchronization_Point_Registration_Succeeded();
	/* specific Getter/Setter */
protected:
	/* specific field */	 

private:
};

/*<END>---------- Synchronization_Point_Registration_Succeeded ------------<END>*/

/*<BEGIN>---------- Announce_Synchronization_Point ------------<BEGIN>*/
class CERTI_EXPORT NM_Announce_Synchronization_Point : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Announce_Synchronization_Point();
	virtual ~NM_Announce_Synchronization_Point();
	/* specific Getter/Setter */
protected:
	/* specific field */	  
private:
};

/*<END>---------- Announce_Synchronization_Point ------------<END>*/

/*<BEGIN>---------- Synchronization_Point_Achieved ------------<BEGIN>*/
class CERTI_EXPORT NM_Synchronization_Point_Achieved : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Synchronization_Point_Achieved();
	virtual ~NM_Synchronization_Point_Achieved();
	/* specific Getter/Setter */
protected:
	/* specific field */	  
private:
};

/*<END>---------- Synchronization_Point_Achieved ------------<END>*/

/*<BEGIN>---------- Federation_Synchronized ------------<BEGIN>*/
class CERTI_EXPORT NM_Federation_Synchronized : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Federation_Synchronized();
	virtual ~NM_Federation_Synchronized();
	/* specific Getter/Setter */
protected:
	/* specific field */	 
private:
};

/*<END>---------- Federation_Synchronized ------------<END>*/

/*<BEGIN>---------- Request_Federation_Save ------------<BEGIN>*/
class CERTI_EXPORT NM_Request_Federation_Save : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Request_Federation_Save();
	virtual ~NM_Request_Federation_Save();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Request_Federation_Save ------------<END>*/

/*<BEGIN>---------- Federate_Save_Begun ------------<BEGIN>*/
class CERTI_EXPORT NM_Federate_Save_Begun : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Federate_Save_Begun();
	virtual ~NM_Federate_Save_Begun();
	/* specific Getter/Setter */
protected:
	/* specific field */

private:
};

/*<END>---------- Federate_Save_Begun ------------<END>*/

/*<BEGIN>---------- Federate_Save_Complete ------------<BEGIN>*/
class CERTI_EXPORT NM_Federate_Save_Complete : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Federate_Save_Complete();
	virtual ~NM_Federate_Save_Complete();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Federate_Save_Complete ------------<END>*/

/*<BEGIN>---------- Federate_Save_Not_Complete ------------<BEGIN>*/
class CERTI_EXPORT NM_Federate_Save_Not_Complete : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Federate_Save_Not_Complete();
	virtual ~NM_Federate_Save_Not_Complete();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Federate_Save_Not_Complete ------------<END>*/

/*<BEGIN>---------- Initiate_Federate_Save ------------<BEGIN>*/
class CERTI_EXPORT NM_Initiate_Federate_Save : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Initiate_Federate_Save();
	virtual ~NM_Initiate_Federate_Save();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Initiate_Federate_Save ------------<END>*/

/*<BEGIN>---------- Federation_Saved ------------<BEGIN>*/
class CERTI_EXPORT NM_Federation_Saved : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Federation_Saved();
	virtual ~NM_Federation_Saved();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Federation_Saved ------------<END>*/

/*<BEGIN>---------- Federation_Not_Saved ------------<BEGIN>*/
class CERTI_EXPORT NM_Federation_Not_Saved : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Federation_Not_Saved();
	virtual ~NM_Federation_Not_Saved();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Federation_Not_Saved ------------<END>*/

/*<BEGIN>---------- Request_Federation_Restore ------------<BEGIN>*/
class CERTI_EXPORT NM_Request_Federation_Restore : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Request_Federation_Restore();
	virtual ~NM_Request_Federation_Restore();
	/* specific Getter/Setter */
protected:
	/* specific field */

private:
};

/*<END>---------- Request_Federation_Restore ------------<END>*/

/*<BEGIN>---------- Federate_Restore_Complete ------------<BEGIN>*/
class CERTI_EXPORT NM_Federate_Restore_Complete : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Federate_Restore_Complete();
	virtual ~NM_Federate_Restore_Complete();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Federate_Restore_Complete ------------<END>*/

/*<BEGIN>---------- Federate_Restore_Not_Complete ------------<BEGIN>*/
class CERTI_EXPORT NM_Federate_Restore_Not_Complete : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Federate_Restore_Not_Complete();
	virtual ~NM_Federate_Restore_Not_Complete();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Federate_Restore_Not_Complete ------------<END>*/

/*<BEGIN>---------- Request_Federation_Restore_Succeeded ------------<BEGIN>*/
class CERTI_EXPORT NM_Request_Federation_Restore_Succeeded : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Request_Federation_Restore_Succeeded();
	virtual ~NM_Request_Federation_Restore_Succeeded();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Request_Federation_Restore_Succeeded ------------<END>*/

/*<BEGIN>---------- Request_Federation_Restore_Failed ------------<BEGIN>*/
class CERTI_EXPORT NM_Request_Federation_Restore_Failed : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Request_Federation_Restore_Failed();
	virtual ~NM_Request_Federation_Restore_Failed();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Request_Federation_Restore_Failed ------------<END>*/

/*<BEGIN>---------- Federation_Restore_Begun ------------<BEGIN>*/
class CERTI_EXPORT NM_Federation_Restore_Begun : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Federation_Restore_Begun();
	virtual ~NM_Federation_Restore_Begun();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Federation_Restore_Begun ------------<END>*/

/*<BEGIN>---------- Initiate_Federate_Restore ------------<BEGIN>*/
class CERTI_EXPORT NM_Initiate_Federate_Restore : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Initiate_Federate_Restore();
	virtual ~NM_Initiate_Federate_Restore();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Initiate_Federate_Restore ------------<END>*/

/*<BEGIN>---------- Federation_Restored ------------<BEGIN>*/
class CERTI_EXPORT NM_Federation_Restored : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Federation_Restored();
	virtual ~NM_Federation_Restored();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Federation_Restored ------------<END>*/

/*<BEGIN>---------- Federation_Not_Restored ------------<BEGIN>*/
class CERTI_EXPORT NM_Federation_Not_Restored : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Federation_Not_Restored();
	virtual ~NM_Federation_Not_Restored();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Federation_Not_Restored ------------<END>*/

/*<BEGIN>---------- Publish_Object_Class ------------<BEGIN>*/
class CERTI_EXPORT NM_Publish_Object_Class : public NM_WithHandleArray {
public:
	typedef NM_WithHandleArray Super;
	NM_Publish_Object_Class();
	virtual ~NM_Publish_Object_Class();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
	//ObjectClassHandle objectClass;
private:
};

/*<END>---------- Publish_Object_Class ------------<END>*/

/*<BEGIN>---------- Unpublish_Object_Class ------------<BEGIN>*/
class CERTI_EXPORT NM_Unpublish_Object_Class : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Unpublish_Object_Class();
	virtual ~NM_Unpublish_Object_Class();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
	//	ObjectClassHandle objectClass;
private:
};

/*<END>---------- Unpublish_Object_Class ------------<END>*/

/*<BEGIN>---------- Publish_Interaction_Class ------------<BEGIN>*/
class CERTI_EXPORT NM_Publish_Interaction_Class : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Publish_Interaction_Class();
	virtual ~NM_Publish_Interaction_Class();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
	const InteractionClassHandle getInteractionClassHandle() const {return interactionClass;};
	void setInteractionClassHandle(InteractionClassHandle interactionClass) {this->interactionClass=interactionClass;};
protected:
	/* specific field */
	//	InteractionClassHandle interactionClass;
private:
};

/*<END>---------- Publish_Interaction_Class ------------<END>*/

/*<BEGIN>---------- Unpublish_Interaction_Class ------------<BEGIN>*/
class CERTI_EXPORT NM_Unpublish_Interaction_Class : public NM_Publish_Interaction_Class {
public:
	typedef NM_Publish_Interaction_Class Super;
	NM_Unpublish_Interaction_Class();
	virtual ~NM_Unpublish_Interaction_Class();	
	/* specific Getter/Setter */
protected:
	/* specific field */	
private:
};

/*<END>---------- Unpublish_Interaction_Class ------------<END>*/

/*<BEGIN>---------- Subscribe_Object_Class ------------<BEGIN>*/
class CERTI_EXPORT NM_Subscribe_Object_Class : public NM_WithHandleArray {
public:
	typedef NM_WithHandleArray Super;
	NM_Subscribe_Object_Class();
	virtual ~NM_Subscribe_Object_Class();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
	//ObjectClassHandle objectClass;
private:
};

/*<END>---------- Subscribe_Object_Class ------------<END>*/

/*<BEGIN>---------- Unsubscribe_Object_Class ------------<BEGIN>*/
class CERTI_EXPORT NM_Unsubscribe_Object_Class : public NM_Unpublish_Object_Class {
public:
	typedef NM_Unpublish_Object_Class Super;
	NM_Unsubscribe_Object_Class();
	virtual ~NM_Unsubscribe_Object_Class();
	/* specific Getter/Setter */
protected:
	/* specific field */	
private:
};

/*<END>---------- Unsubscribe_Object_Class ------------<END>*/

/*<BEGIN>---------- Subscribe_Interaction_Class ------------<BEGIN>*/
class CERTI_EXPORT NM_Subscribe_Interaction_Class : public NM_Publish_Interaction_Class {
public:
	typedef NM_Publish_Interaction_Class Super;
	NM_Subscribe_Interaction_Class();
	virtual ~NM_Subscribe_Interaction_Class();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Subscribe_Interaction_Class ------------<END>*/

/*<BEGIN>---------- Unsubscribe_Interaction_Class ------------<BEGIN>*/
class CERTI_EXPORT NM_Unsubscribe_Interaction_Class : public NM_Publish_Interaction_Class {
public:
	typedef NM_Publish_Interaction_Class Super;
	NM_Unsubscribe_Interaction_Class();
	virtual ~NM_Unsubscribe_Interaction_Class();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Unsubscribe_Interaction_Class ------------<END>*/

/*<BEGIN>---------- Turn_Interactions_On ------------<BEGIN>*/
class CERTI_EXPORT NM_Turn_Interactions_On : public NM_Publish_Interaction_Class {
public:
	typedef NM_Publish_Interaction_Class Super;
	NM_Turn_Interactions_On();
	virtual ~NM_Turn_Interactions_On();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Turn_Interactions_On ------------<END>*/

/*<BEGIN>---------- Turn_Interactions_Off ------------<BEGIN>*/
class CERTI_EXPORT NM_Turn_Interactions_Off : public NM_Publish_Interaction_Class {
public:
	typedef NM_Publish_Interaction_Class Super;
	NM_Turn_Interactions_Off();
	virtual ~NM_Turn_Interactions_Off();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Turn_Interactions_Off ------------<END>*/

/*<BEGIN>---------- Register_Object ------------<BEGIN>*/
class CERTI_EXPORT NM_Register_Object : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Register_Object();
	virtual ~NM_Register_Object();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
	//	ObjectHandle object;
private:
};

/*<END>---------- Register_Object ------------<END>*/

/*<BEGIN>---------- Discover_Object ------------<BEGIN>*/
class CERTI_EXPORT NM_Discover_Object : public NM_Register_Object {
public:
	typedef NM_Register_Object Super;
	NM_Discover_Object();
	virtual ~NM_Discover_Object();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Discover_Object ------------<END>*/

/*<BEGIN>---------- Update_Attribute_Values ------------<BEGIN>*/
class CERTI_EXPORT NM_Update_Attribute_Values : public NM_WithHandleArray {
public:
	typedef NM_WithHandleArray Super;
	NM_Update_Attribute_Values();
	virtual ~NM_Update_Attribute_Values();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
	//	ObjectClassHandle objectClass;	
	//	ValueLengthPair ValueArray[MAX_ATTRIBUTES_PER_CLASS];
private:
};

/*<END>---------- Update_Attribute_Values ------------<END>*/

/*<BEGIN>---------- Reflect_Attribute_Values ------------<BEGIN>*/
class CERTI_EXPORT NM_Reflect_Attribute_Values : public NM_Update_Attribute_Values {
public:
	typedef NM_Update_Attribute_Values Super;
	NM_Reflect_Attribute_Values();
	virtual ~NM_Reflect_Attribute_Values();       
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Reflect_Attribute_Values ------------<END>*/

/*<BEGIN>---------- Send_Interaction ------------<BEGIN>*/
class CERTI_EXPORT NM_Send_Interaction : public NM_WithHandleArray {
public:
	typedef NM_WithHandleArray Super;
	NM_Send_Interaction();
	virtual ~NM_Send_Interaction();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
	//	InteractionClassHandle interactionClass;	
	//	ValueLengthPair ValueArray[MAX_ATTRIBUTES_PER_CLASS];
private:
};

/*<END>---------- Send_Interaction ------------<END>*/

/*<BEGIN>---------- Receive_Interaction ------------<BEGIN>*/
class CERTI_EXPORT NM_Receive_Interaction : public NM_Send_Interaction {
public:
	typedef NM_Send_Interaction Super;
	NM_Receive_Interaction();
	virtual ~NM_Receive_Interaction();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Receive_Interaction ------------<END>*/

/*<BEGIN>---------- Delete_Object ------------<BEGIN>*/
class CERTI_EXPORT NM_Delete_Object : public NM_Register_Object {
public:
	typedef NM_Register_Object Super;
	NM_Delete_Object();
	virtual ~NM_Delete_Object();
	/* specific Getter/Setter */
protected:
	/* specific field */

private:
};

/*<END>---------- Delete_Object ------------<END>*/

/*<BEGIN>---------- Remove_Object ------------<BEGIN>*/
class CERTI_EXPORT NM_Remove_Object : public NM_Delete_Object {
public:
	typedef NM_Delete_Object Super;
	NM_Remove_Object();
	virtual ~NM_Remove_Object();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Remove_Object ------------<END>*/

/*<BEGIN>---------- Change_Attribute_Transport_Type ------------<BEGIN>*/
class CERTI_EXPORT NM_Change_Attribute_Transport_Type : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Change_Attribute_Transport_Type();
	virtual ~NM_Change_Attribute_Transport_Type();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Change_Attribute_Transport_Type ------------<END>*/

/*<BEGIN>---------- Change_Attribute_Order_Type ------------<BEGIN>*/
class CERTI_EXPORT NM_Change_Attribute_Order_Type : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Change_Attribute_Order_Type();
	virtual ~NM_Change_Attribute_Order_Type();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Change_Attribute_Order_Type ------------<END>*/

/*<BEGIN>---------- Change_Interaction_Transport_Type ------------<BEGIN>*/
class CERTI_EXPORT NM_Change_Interaction_Transport_Type : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Change_Interaction_Transport_Type();
	virtual ~NM_Change_Interaction_Transport_Type();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Change_Interaction_Transport_Type ------------<END>*/

/*<BEGIN>---------- Change_Interaction_Order_Type ------------<BEGIN>*/
class CERTI_EXPORT NM_Change_Interaction_Order_Type : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Change_Interaction_Order_Type();
	virtual ~NM_Change_Interaction_Order_Type();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Change_Interaction_Order_Type ------------<END>*/

/*<BEGIN>---------- Request_Class_Attribute_Value_Update ------------<BEGIN>*/
class CERTI_EXPORT NM_Request_Class_Attribute_Value_Update : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Request_Class_Attribute_Value_Update();
	virtual ~NM_Request_Class_Attribute_Value_Update();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Request_Class_Attribute_Value_Update ------------<END>*/

/*<BEGIN>---------- Request_Object_Attribute_Value_Update ------------<BEGIN>*/
class CERTI_EXPORT NM_Request_Object_Attribute_Value_Update : public NM_WithHandleArray {
public:
	typedef NM_WithHandleArray Super;
	NM_Request_Object_Attribute_Value_Update();
	virtual ~NM_Request_Object_Attribute_Value_Update();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
	const ObjectHandle getObject() const {return object;};
	void setObject(ObjectHandle object) {this->object=object;};
protected:
	/* specific field */
private:
};

/*<END>---------- Request_Object_Attribute_Value_Update ------------<END>*/

/*<BEGIN>---------- Is_Attribute_Owned_By_Federate ------------<BEGIN>*/
class CERTI_EXPORT NM_Is_Attribute_Owned_By_Federate : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Is_Attribute_Owned_By_Federate();
	virtual ~NM_Is_Attribute_Owned_By_Federate();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
	//	ObjectHandle    object;
	//	AttributeHandle attribute;
private:
};

/*<END>---------- Is_Attribute_Owned_By_Federate ------------<END>*/

/*<BEGIN>---------- Query_Attribute_Ownership ------------<BEGIN>*/
class CERTI_EXPORT NM_Query_Attribute_Ownership : public NM_Is_Attribute_Owned_By_Federate {
public:
	typedef NM_Is_Attribute_Owned_By_Federate Super;
	NM_Query_Attribute_Ownership();
	virtual ~NM_Query_Attribute_Ownership();      
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Query_Attribute_Ownership ------------<END>*/

/*<BEGIN>---------- Attribute_Is_Not_Owned ------------<BEGIN>*/
class CERTI_EXPORT NM_Attribute_Is_Not_Owned : public NM_Is_Attribute_Owned_By_Federate {
public:
	typedef NM_Is_Attribute_Owned_By_Federate Super;
	NM_Attribute_Is_Not_Owned();
	virtual ~NM_Attribute_Is_Not_Owned();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Attribute_Is_Not_Owned ------------<END>*/

/*<BEGIN>---------- Inform_Attribute_Ownership ------------<BEGIN>*/
class CERTI_EXPORT NM_Inform_Attribute_Ownership : public NM_Is_Attribute_Owned_By_Federate {
public:
	typedef NM_Is_Attribute_Owned_By_Federate Super;
	NM_Inform_Attribute_Ownership();
	virtual ~NM_Inform_Attribute_Ownership();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Inform_Attribute_Ownership ------------<END>*/

/*<BEGIN>---------- Negotiated_Attribute_Ownership_Divestiture ------------<BEGIN>*/
class CERTI_EXPORT NM_Negotiated_Attribute_Ownership_Divestiture : public NM_Request_Object_Attribute_Value_Update {
public:
	typedef NM_Request_Object_Attribute_Value_Update Super;
	NM_Negotiated_Attribute_Ownership_Divestiture();
	virtual ~NM_Negotiated_Attribute_Ownership_Divestiture();
	/* specific Getter/Setter */	
protected:
	/* specific field */
private:
};

/*<END>---------- Negotiated_Attribute_Ownership_Divestiture ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Acquisition_Notification ------------<BEGIN>*/
class CERTI_EXPORT NM_Attribute_Ownership_Acquisition_Notification : public NM_Request_Object_Attribute_Value_Update {
public:
	typedef NM_Request_Object_Attribute_Value_Update Super;
	NM_Attribute_Ownership_Acquisition_Notification();
	virtual ~NM_Attribute_Ownership_Acquisition_Notification();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Attribute_Ownership_Acquisition_Notification ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Divestiture_Notification ------------<BEGIN>*/
class CERTI_EXPORT NM_Attribute_Ownership_Divestiture_Notification : public NM_Request_Object_Attribute_Value_Update {
public:
	typedef NM_Request_Object_Attribute_Value_Update Super;
	NM_Attribute_Ownership_Divestiture_Notification();
	virtual ~NM_Attribute_Ownership_Divestiture_Notification();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Attribute_Ownership_Divestiture_Notification ------------<END>*/

/*<BEGIN>---------- Request_Attribute_Ownership_Assumption ------------<BEGIN>*/
class CERTI_EXPORT NM_Request_Attribute_Ownership_Assumption : public NM_Negotiated_Attribute_Ownership_Divestiture {
public:
	typedef NM_Negotiated_Attribute_Ownership_Divestiture Super;
	NM_Request_Attribute_Ownership_Assumption();
	virtual ~NM_Request_Attribute_Ownership_Assumption();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Request_Attribute_Ownership_Assumption ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Unavailable ------------<BEGIN>*/
class CERTI_EXPORT NM_Attribute_Ownership_Unavailable : public NM_Request_Object_Attribute_Value_Update {
public:
	typedef NM_Request_Object_Attribute_Value_Update Super;
	NM_Attribute_Ownership_Unavailable();
	virtual ~NM_Attribute_Ownership_Unavailable();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Attribute_Ownership_Unavailable ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Acquisition_If_Available ------------<BEGIN>*/
class CERTI_EXPORT NM_Attribute_Ownership_Acquisition_If_Available : public NM_Request_Object_Attribute_Value_Update {
public:
	typedef NM_Request_Object_Attribute_Value_Update Super;
	NM_Attribute_Ownership_Acquisition_If_Available();
	virtual ~NM_Attribute_Ownership_Acquisition_If_Available();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Attribute_Ownership_Acquisition_If_Available ------------<END>*/

/*<BEGIN>---------- Unconditional_Attribute_Ownership_Divestiture ------------<BEGIN>*/
class CERTI_EXPORT NM_Unconditional_Attribute_Ownership_Divestiture : public NM_Request_Object_Attribute_Value_Update {
public:
	typedef NM_Request_Object_Attribute_Value_Update Super;
	NM_Unconditional_Attribute_Ownership_Divestiture();
	virtual ~NM_Unconditional_Attribute_Ownership_Divestiture();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Unconditional_Attribute_Ownership_Divestiture ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Acquisition ------------<BEGIN>*/
class CERTI_EXPORT NM_Attribute_Ownership_Acquisition : public NM_Negotiated_Attribute_Ownership_Divestiture {
public:
	typedef NM_Negotiated_Attribute_Ownership_Divestiture Super;
	NM_Attribute_Ownership_Acquisition();
	virtual ~NM_Attribute_Ownership_Acquisition();	
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Attribute_Ownership_Acquisition ------------<END>*/

/*<BEGIN>---------- Request_Attribute_Ownership_Release ------------<BEGIN>*/
class CERTI_EXPORT NM_Request_Attribute_Ownership_Release : public NM_Negotiated_Attribute_Ownership_Divestiture {
public:
	typedef NM_Negotiated_Attribute_Ownership_Divestiture Super;
	NM_Request_Attribute_Ownership_Release();
	virtual ~NM_Request_Attribute_Ownership_Release();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Request_Attribute_Ownership_Release ------------<END>*/

/*<BEGIN>---------- Cancel_Negotiated_Attribute_Ownership_Divestiture ------------<BEGIN>*/
class CERTI_EXPORT NM_Cancel_Negotiated_Attribute_Ownership_Divestiture : public NM_Request_Object_Attribute_Value_Update {
public:
	typedef NM_Request_Object_Attribute_Value_Update Super;
	NM_Cancel_Negotiated_Attribute_Ownership_Divestiture();
	virtual ~NM_Cancel_Negotiated_Attribute_Ownership_Divestiture();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Cancel_Negotiated_Attribute_Ownership_Divestiture ------------<END>*/

/*<BEGIN>---------- Attribute_Ownership_Release_Response ------------<BEGIN>*/
class CERTI_EXPORT NM_Attribute_Ownership_Release_Response : public NM_Request_Object_Attribute_Value_Update {
public:
	typedef NM_Request_Object_Attribute_Value_Update Super;
	NM_Attribute_Ownership_Release_Response();
	virtual ~NM_Attribute_Ownership_Release_Response();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Attribute_Ownership_Release_Response ------------<END>*/

/*<BEGIN>---------- Cancel_Attribute_Ownership_Acquisition ------------<BEGIN>*/
class CERTI_EXPORT NM_Cancel_Attribute_Ownership_Acquisition : public NM_Request_Object_Attribute_Value_Update {
public:
	typedef NM_Request_Object_Attribute_Value_Update Super;
	NM_Cancel_Attribute_Ownership_Acquisition();
	virtual ~NM_Cancel_Attribute_Ownership_Acquisition();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Cancel_Attribute_Ownership_Acquisition ------------<END>*/

/*<BEGIN>---------- Confirm_Attribute_Ownership_Acquisition_Cancellation ------------<BEGIN>*/
class CERTI_EXPORT NM_Confirm_Attribute_Ownership_Acquisition_Cancellation : public NM_Request_Object_Attribute_Value_Update {
public:
	typedef NM_Request_Object_Attribute_Value_Update Super;
	NM_Confirm_Attribute_Ownership_Acquisition_Cancellation();
	virtual ~NM_Confirm_Attribute_Ownership_Acquisition_Cancellation();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Confirm_Attribute_Ownership_Acquisition_Cancellation ------------<END>*/

/*<BEGIN>---------- DDM_Create_Region ------------<BEGIN>*/
class CERTI_EXPORT NM_DDM_Create_Region : public NM_DDM_Base {
public:
	typedef NM_DDM_Base Super;
	NM_DDM_Create_Region();
	virtual ~NM_DDM_Create_Region();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
	const SpaceHandle getSpace() const {return space;};
	void setSpace(SpaceHandle space) {this->space=space;};	
	const int32_t getRegion() const {return region;};
	void setRegion(SpaceHandle region) {this->region=region;};
protected:
	/* specific field */
	//	SpaceHandle space;
	//	int32_t nbExtents;
	//	int32_t region;
private:
};

/*<END>---------- DDM_Create_Region ------------<END>*/

/*<BEGIN>---------- DDM_Modify_Region ------------<BEGIN>*/
class CERTI_EXPORT NM_DDM_Modify_Region : public NM_DDM_Base {
public:
	typedef NM_DDM_Base Super;
	NM_DDM_Modify_Region();
	virtual ~NM_DDM_Modify_Region();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
	const int32_t getRegion() const {return region;};
	void setRegion(SpaceHandle region) {this->region=region;};
protected:
	/* specific field */
	//	int32_t region;
private:
};

/*<END>---------- DDM_Modify_Region ------------<END>*/

/*<BEGIN>---------- DDM_Delete_Region ------------<BEGIN>*/
class CERTI_EXPORT NM_DDM_Delete_Region : public NM_DDM_Base {
public:
	typedef NM_DDM_Base Super;
	NM_DDM_Delete_Region();
	virtual ~NM_DDM_Delete_Region();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
	const int32_t getRegion() const {return region;};
	void setRegion(SpaceHandle region) {this->region=region;};
protected:
	/* specific field */
	//	int32_t region;	
private:
};

/*<END>---------- DDM_Delete_Region ------------<END>*/

/*<BEGIN>---------- DDM_Associate_Region ------------<BEGIN>*/
class CERTI_EXPORT NM_DDM_Associate_Region : public NM_DDM_Base {
public:
	typedef NM_DDM_Base Super;
	NM_DDM_Associate_Region();
	virtual ~NM_DDM_Associate_Region();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
	const int32_t getRegion() const {return region;};
	void setRegion(SpaceHandle region) {this->region=region;};
	const ObjectHandle getObject() const {return object;};
	void setObject(ObjectHandle object) {this->object=object;};		
protected:
	/* specific field */
	//	int32_t      region;
	//	ObjectHandle object;
private:
};

/*<END>---------- DDM_Associate_Region ------------<END>*/

/*<BEGIN>---------- DDM_Register_Object ------------<BEGIN>*/
class CERTI_EXPORT NM_DDM_Register_Object : public NM_DDM_Base {
public:
	typedef NM_DDM_Base Super;
	NM_DDM_Register_Object();
	virtual ~NM_DDM_Register_Object();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- DDM_Register_Object ------------<END>*/

/*<BEGIN>---------- DDM_Unassociate_Region ------------<BEGIN>*/
class CERTI_EXPORT NM_DDM_Unassociate_Region : public NM_DDM_Base {
public:
	typedef NM_DDM_Base Super;
	NM_DDM_Unassociate_Region();
	virtual ~NM_DDM_Unassociate_Region();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- DDM_Unassociate_Region ------------<END>*/

/*<BEGIN>---------- DDM_Subscribe_Attributes ------------<BEGIN>*/
class CERTI_EXPORT NM_DDM_Subscribe_Attributes : public NM_DDM_Base {
public:
	typedef NM_DDM_Base Super;
	NM_DDM_Subscribe_Attributes();
	virtual ~NM_DDM_Subscribe_Attributes();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- DDM_Subscribe_Attributes ------------<END>*/

/*<BEGIN>---------- DDM_Unsubscribe_Attributes ------------<BEGIN>*/
class CERTI_EXPORT NM_DDM_Unsubscribe_Attributes : public NM_DDM_Base {
public:
	typedef NM_DDM_Base Super;
	NM_DDM_Unsubscribe_Attributes();
	virtual ~NM_DDM_Unsubscribe_Attributes();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- DDM_Unsubscribe_Attributes ------------<END>*/

/*<BEGIN>---------- DDM_Subscribe_Interaction ------------<BEGIN>*/
class CERTI_EXPORT NM_DDM_Subscribe_Interaction : public NM_DDM_Base {
public:
	typedef NM_DDM_Base Super;
	NM_DDM_Subscribe_Interaction();
	virtual ~NM_DDM_Subscribe_Interaction();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- DDM_Subscribe_Interaction ------------<END>*/

/*<BEGIN>---------- DDM_Unsubscribe_Interaction ------------<BEGIN>*/
class CERTI_EXPORT NM_DDM_Unsubscribe_Interaction : public NM_DDM_Subscribe_Interaction {
public:
	typedef NM_DDM_Subscribe_Interaction Super;
	NM_DDM_Unsubscribe_Interaction();
	virtual ~NM_DDM_Unsubscribe_Interaction();	
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- DDM_Unsubscribe_Interaction ------------<END>*/

/*<BEGIN>---------- Provide_Attribute_Value_Update ------------<BEGIN>*/
class CERTI_EXPORT NM_Provide_Attribute_Value_Update : public NM_Request_Object_Attribute_Value_Update {
public:
	typedef NM_Request_Object_Attribute_Value_Update Super;
	NM_Provide_Attribute_Value_Update();
	virtual ~NM_Provide_Attribute_Value_Update();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Provide_Attribute_Value_Update ------------<END>*/

/*<BEGIN>---------- Get_Fed_File ------------<BEGIN>*/
class CERTI_EXPORT NM_Get_FED_File : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Get_FED_File();
	virtual ~NM_Get_FED_File();
	virtual void serialize(MessageBuffer& msgBuffer);
	virtual void deserialize(MessageBuffer& msgBuffer);
	/* specific Getter/Setter */
	void setFEDid(std::string FEDid) {this->FEDid=FEDid;};
	const std::string getFEDid() const {return FEDid;};
	//	void setLineFollows() {lineFollows=true;};
	//	void unsetLineFollows() {lineFollows=true;};
	//const bool getLineFollows() {return lineFollows;};
	void setFEDLine(std::string line) {this->line=line;};
	std::string getFEDLine() {return line;};
protected:
	/* specific field */
	//	std::string FEDid;
	//	bool lineFollows;
	std::string line;
private:
};

/*<END>---------- Get_Fed_File ------------<END>*/

/*<BEGIN>---------- Last ------------<BEGIN>*/
class CERTI_EXPORT NM_Last : public NetworkMessage {
public:
	typedef NetworkMessage Super;
	NM_Last();
	virtual ~NM_Last();
	/* specific Getter/Setter */
protected:
	/* specific field */
private:
};

/*<END>---------- Last ------------<END>*/

} /* end of namespace certi */

#endif
