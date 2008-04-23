// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: NetworkMessage.hh,v 3.30.2.11 2008/04/23 06:23:21 erk Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_NETWORK_MESSAGE_HH
#define CERTI_NETWORK_MESSAGE_HH

#include "fedtime.hh"
#include "Exception.hh"
#include "Socket.hh"
#include "RTIRegion.hh"
#include "BasicMessage.hh"

#include <vector>
#include <string>

#ifdef FEDERATION_USES_MULTICAST
#define MC_PORT 60123
#define ADRESSE_MULTICAST "224.0.0.251"
#endif // FEDERATION_USES_MULTICAST

#define PORT_TCP_RTIG "60400"
#define PORT_UDP_RTIG "60500"

namespace certi {

/**
 * NetworkMessage is the base class used
 * for modelling message exchanged between RTIG and RTIA.
 * NetworkMessage is the base class of a class hierarchy.
 * Each specific message is a (direct of indirect)
 * daughter class of NetworkMessage.    
 */ 
class CERTI_EXPORT NetworkMessage : public BasicMessage
{
public:

	typedef enum Type {
		NOT_USED = 0, // Not used.
		CLOSE_CONNEXION,
		MESSAGE_NULL,
		CREATE_FEDERATION_EXECUTION,
		DESTROY_FEDERATION_EXECUTION,
		JOIN_FEDERATION_EXECUTION,
		RESIGN_FEDERATION_EXECUTION,
		SET_TIME_REGULATING,
		SET_TIME_CONSTRAINED,
		REGISTER_FEDERATION_SYNCHRONIZATION_POINT,
		SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED, // RTIG to RTIA
		ANNOUNCE_SYNCHRONIZATION_POINT, // RTIG to RTIA
		SYNCHRONIZATION_POINT_ACHIEVED,
		FEDERATION_SYNCHRONIZED, // RTIG to RTIA
		REQUEST_FEDERATION_SAVE,
		FEDERATE_SAVE_BEGUN,
		FEDERATE_SAVE_COMPLETE,
		FEDERATE_SAVE_NOT_COMPLETE,
		INITIATE_FEDERATE_SAVE, // RTIG to RTIA
		FEDERATION_SAVED, // RTIG to RTIA
		FEDERATION_NOT_SAVED, // RTIG to RTIA
		REQUEST_FEDERATION_RESTORE,
		FEDERATE_RESTORE_COMPLETE,
		FEDERATE_RESTORE_NOT_COMPLETE,
		REQUEST_FEDERATION_RESTORE_SUCCEEDED, // RTIG to RTIA
		REQUEST_FEDERATION_RESTORE_FAILED, // RTIG to RTIA
		FEDERATION_RESTORE_BEGUN, // RTIG to RTIA
		INITIATE_FEDERATE_RESTORE, // RTIG to RTIA
		FEDERATION_RESTORED, // RTIG to RTIA
		FEDERATION_NOT_RESTORED, // RTIG to RTIA
		PUBLISH_OBJECT_CLASS,
		UNPUBLISH_OBJECT_CLASS,
		PUBLISH_INTERACTION_CLASS,
		UNPUBLISH_INTERACTION_CLASS,
		SUBSCRIBE_OBJECT_CLASS,
		UNSUBSCRIBE_OBJECT_CLASS,
		SUBSCRIBE_INTERACTION_CLASS,
		UNSUBSCRIBE_INTERACTION_CLASS,
		TURN_INTERACTIONS_ON, // only RTIG->RTIA
		TURN_INTERACTIONS_OFF, // only RTIG->RTIA
		REGISTER_OBJECT,
		DISCOVER_OBJECT, // only RTIG->RTIA
		UPDATE_ATTRIBUTE_VALUES,
		REFLECT_ATTRIBUTE_VALUES, // only RTIG->RTIA
		SEND_INTERACTION,
		RECEIVE_INTERACTION, // only RTIG->RTIA
		DELETE_OBJECT,
		REMOVE_OBJECT, // only RTIG->RTIA
		CHANGE_ATTRIBUTE_TRANSPORT_TYPE,
		CHANGE_ATTRIBUTE_ORDER_TYPE,
		CHANGE_INTERACTION_TRANSPORT_TYPE,
		CHANGE_INTERACTION_ORDER_TYPE,
		REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE,
		REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE,
		IS_ATTRIBUTE_OWNED_BY_FEDERATE,
		QUERY_ATTRIBUTE_OWNERSHIP,
		ATTRIBUTE_IS_NOT_OWNED,
		INFORM_ATTRIBUTE_OWNERSHIP,
		NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
		ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION,
		ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION,
		REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION,
		ATTRIBUTE_OWNERSHIP_UNAVAILABLE,
		ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE,
		UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
		ATTRIBUTE_OWNERSHIP_ACQUISITION,
		REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE,
		CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
		ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE,
		CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION,
		CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION,
		DDM_CREATE_REGION,
		DDM_MODIFY_REGION,
		DDM_DELETE_REGION,
		DDM_ASSOCIATE_REGION,
		DDM_REGISTER_OBJECT,
		DDM_UNASSOCIATE_REGION,
		DDM_SUBSCRIBE_ATTRIBUTES,
		DDM_UNSUBSCRIBE_ATTRIBUTES,
		DDM_SUBSCRIBE_INTERACTION,
		DDM_UNSUBSCRIBE_INTERACTION,
		PROVIDE_ATTRIBUTE_VALUE_UPDATE,
		GET_FED_FILE,
		LAST
	} Message_T;	

	NetworkMessage();
	virtual ~NetworkMessage();

	const NetworkMessage::Message_T getType() const {return type;};
	const TypeException getException() const {return exception;};

	/**
	 * Serialize the message into a buffer
	 */
	virtual void serialize(MessageBuffer& msgBuffer);
	/**
	 * DeSerialize the message from a buffer
	 */
	virtual void deserialize(MessageBuffer& msgBuffer);

	/**
	 * Deserialize a message using the message buffer
	 * from another message.
	 * This is used to avoid copy in a virtual constructor
	 * for network Message.
	 */ 
	void deserialize(NetworkMessage& anotherMsg) 
		{anotherMsg.msgBuf.assumeSizeFromReservedBytes();
		 deserialize(anotherMsg.msgBuf);};
	
	void send(Socket* socket) throw (NetworkError, NetworkSignal);
	void receive(Socket* socket) throw (NetworkError, NetworkSignal);

	// Parameter and Attribute Management
	// Remove the Parameter of rank 'Rank' in the ParamArray and its value in
	// ValueArray. If there are other parameter in the list, they are shifted
	// one rank back.
	// Ex: ParamArraySize = 3,
	// ParamArray =[1, 2, 3], ValueArray =["Val1", "Val2", "Val3"]
	//->removeParameter(1)
	// ParamArraySize = 2,
	// ParamArray =[1, 3], ValueArray =["Val1", "Val3"]
	void removeParameter(UShort Rank);

	// See RemoveParameter for explanations.
	void removeAttribute(UShort Rank);

	// Value Array Management

	// setValue : Value and its length are stored into ValueArray[Rank]
	void setValue(int Rank, const char *Value, unsigned long length)
	throw (RTIinternalError); // Bad Rank, Bad Value

	// getValue : Value and its length are tooken from ValueArray[Rank]
	// If Value == NULL return a newly allocated copy of Value, else copy it
	// in Value.
	char *getValue(int Rank, unsigned long *length, char *Value = NULL)
	throw (RTIinternalError); // Bad Rank

	// Return a newly allocated ValueArray, exactly of size AttribArraySize.
	// containing the actual Attribute values. You must FREE this structure.
	ValueLengthPair *getAttribValueArray();

	// Return a newly allocated ValueArray, exactly of size ParamArraySize,
	// containing the actual Parameter values. You must FREE this structure.
	ParameterLengthPair *getParamValueArray();

	void setAHS(const AttributeHandle *, int);


	void setBoolean(bool);
	bool getBoolean() const { return boolean ; };


	UShort number ;

	std::string federationName ;
	std::string federateName;
	std::string FEDid ;

	int bestEffortPeer ;
	unsigned long bestEffortAddress ;

	int numberOfRegulators ;

	/* NM_DDM_Base class fields */
	SpaceHandle            space;
	int32_t                nbExtents;
	int32_t                region;
	ObjectHandle           object;
	ObjectClassHandle      objectClass;
	InteractionClassHandle interactionClass;
	bool boolean ;

	/**
	 * Indicate if the message is dated or not
	 */
	bool isDated;
	/** 
	 * If ones set Date then this is a Dated message
	 * Message builder which setDate will generate a Dated message 
	 */
	void setDate(FederationTime date) {isDated=true; this->date = date;};
	const FederationTime getDate() const {return this->date;};

	unsigned long multicastAddress ;

	/**
	 * Indicate if the message is Labelled or not
	 */
	bool isLabelled;	

	void setLabel(const std::string label) {isLabelled = true; this->label = label;};
	void setLabel(const char *new_label) {isLabelled = true; label = std::string(new_label); }
	const std::string getLabel() const {return this->label;};

	/**
	 * Indicate if the message is Tagged or not
	 */
	bool isTagged;
	void setTag(const std::string tag) {isTagged = true; this->tag = tag;};
	void setTag(const char *new_tag) {isTagged = true; tag = std::string(new_tag); }
	const std::string getTag() const {return this->tag;};

	ObjectHandlecount idCount ;
	ObjectHandle firstId ;
	ObjectHandle lastId ;

	EventRetractionHandle eventRetraction ;

	/* NM_WithHandleArray class specific fields */
	UShort handleArraySize ;
	/* FIXME will make this a vector<AttributeHandle> */
	AttributeHandle handleArray[MAX_ATTRIBUTES_PER_CLASS] ;

	TransportType transport ;
	OrderType order ;

	/** The name corresponding to message type */
	const std::string getName() const {return name;}

	/** 
	 * The exception type 
	 * if the message is carrying an exception
	 */
	TypeException exception ;
	/**
	 * The federation handle 
	 * the message is part of this federation activity
	 */
	Handle federation ;
	/** 
	 * The federate handle
	 * the message is for this particular federate
	 */
	FederateHandle federate ;

	/**
	 * The exception reason (if the message carry one)
	 */
	std::string exceptionReason;	

	/* used by some sub-classes */
	int32_t attribute;

protected:
	/** 
	 * The message name.
	 * should be initialized by the specialized
	 * network message constructor
	 */
	std::string name;	

	/** 
	 * The network message type
	 * type field cannot be accessed directly 
	 *   - only NM constructor may set it.
	 *   - getter should be used to get it. 
	 */
	Message_T type;

	/** 
	 * The date of message if it is dated.
	 * date field cannot be accessed directly but only using
	 * getter/setter.
	 */
	FederationTime date ;

	/** 
	 * The label of message if it is labelled.
	 * date field cannot be accessed directly but only using
	 * getter/setter.
	 */
	std::string label;
	/** 
	 * The tag of message if it is tagged.
	 * date field cannot be accessed directly but only using
	 * getter/setter.
	 */
	std::string tag;

	// ValueArray is now a ValueLengthPair
	ValueLengthPair ValueArray[MAX_ATTRIBUTES_PER_CLASS] ;

private:
};

// BUG: FIXME this is used by SocketMC and should
//      be thrown away as soon as possible.
#define TAILLE_MSG_RESEAU sizeof(NetworkMessage)

} // namespace certi

#endif // CERTI_NETWORK_MESSAGE_HH

// $Id: NetworkMessage.hh,v 3.30.2.11 2008/04/23 06:23:21 erk Exp $
