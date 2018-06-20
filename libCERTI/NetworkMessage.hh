// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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

#ifndef CERTI_NETWORK_MESSAGE_HH
#define CERTI_NETWORK_MESSAGE_HH

#include "BasicMessage.hh"
#include "Exception.hh"
#include "FedTimeD.hh"
#include "RTIRegion.hh"
#include "Socket.hh"

#include <string>
#include <vector>

#ifdef FEDERATION_USES_MULTICAST
#define MC_PORT 60123
#define ADRESSE_MULTICAST "224.0.0.251"
#endif // FEDERATION_USES_MULTICAST

#define PORT_TCP_RTIG "60400"
#define PORT_UDP_RTIG "60500"

namespace certi {

/**
 * NetworkMessage is the base class used
 * for modeling message exchanged between RTIG and RTIA.
 * NetworkMessage is the base class of a class hierarchy.
 * Each specific message is a (direct of indirect)
 * daughter class of NetworkMessage.    
 */
class CERTI_EXPORT NetworkMessage : public BasicMessage {
public:
    enum class Type : int32_t {
        NOT_USED = 0, // Not used.
        CLOSE_CONNEXION,
        MESSAGE_NULL,
        CREATE_FEDERATION_EXECUTION,
        DESTROY_FEDERATION_EXECUTION,
        JOIN_FEDERATION_EXECUTION,
        ADDITIONAL_FOM_MODULE,
        RESIGN_FEDERATION_EXECUTION,
        SET_TIME_REGULATING,
        SET_TIME_CONSTRAINED,
        TIME_REGULATION_ENABLED, // RTIG to RTIA
        TIME_CONSTRAINED_ENABLED, // RTIG to RTIA
        REGISTER_FEDERATION_SYNCHRONIZATION_POINT,
        CONFIRM_SYNCHRONIZATION_POINT_REGISTRATION, // RTIG to RTIA
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
        ATTRIBUTE_OWNERSHIP_BASE, /* NOT USED */
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
        /* GET_FED_FILE_SUPPRESSED, */
        SET_CLASS_RELEVANCE_ADVISORY_SWITCH,
        SET_INTERACTION_RELEVANCE_ADVISORY_SWITCH,
        SET_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH,
        SET_ATTRIBUTE_SCOPE_ADVISORY_SWITCH,
        START_REGISTRATION_FOR_OBJECT_CLASS,
        STOP_REGISTRATION_FOR_OBJECT_CLASS,
        RESERVE_OBJECT_INSTANCE_NAME, // HLA1516, only RTIA->RTIG
        RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED, // HLA1516, only RTIG->RTIA
        RESERVE_OBJECT_INSTANCE_NAME_FAILED, // HLA1516, only RTIG->RTIA
        MESSAGE_NULL_PRIME, // CERTI specific for handling NER or NERA and zero-lk
        ENABLE_ASYNCHRONOUS_DELIVERY,
        DISABLE_ASYNCHRONOUS_DELIVERY,
        TIME_ADVANCE_REQUEST,
        TIME_ADVANCE_REQUEST_AVAILABLE,
        NEXT_MESSAGE_REQUEST,
        NEXT_MESSAGE_REQUEST_AVAILABLE,
        TIME_STATE_UPDATE,
        MOM_STATUS,
        LAST
    };
    
    static constexpr size_t the_message_type_count = static_cast<size_t>(NetworkMessage::Type::LAST);

    static std::string to_string(const NetworkMessage::Type& msg_type);
    
    NetworkMessage();
    virtual ~NetworkMessage();

    /**
	 * Get the name of [the type of] the message.
	 * @return the message type name.
	 */
    const char* getMessageName() const
    {
        return messageName;
    }
    /**
	 * Get the message type.
	 * @return the type of the message
	 */
    NetworkMessage::Type getMessageType() const
    {
        return type;
    };

    Exception::Type getException() const
    {
        return exception;
    };

    Exception::Type& getRefException()
    {
        return exception;
    };

    void setException(const Exception::Type e)
    {
        exception = e;
    };

    void setException(const Exception::Type e, const std::string& reason)
    {
        exception = e;
        exceptionReason = reason;
    };

    const std::string& getExceptionReason()
    {
        return exceptionReason;
    };

    /**
	 * Serialize the message into a buffer
	 * @param[in] msgBuffer the serialization buffer
	 */
    virtual void serialize(MessageBuffer& msgBuffer);

    virtual std::ostream& show(std::ostream& out);

    /**
	 * DeSerialize the message from a buffer
	 * @param[in] msgBuffer the deserialization buffer
	 */
    virtual void deserialize(MessageBuffer& msgBuffer);

    /**
     * Send a message buffer to the socket
     */
    void send(Socket* socket, MessageBuffer& msgBuffer);

    /**
     * Send a message buffer to each socket in the list
     */
    void send(std::vector<Socket*> sockets, MessageBuffer& msgBuffer);

    /**
	 * Receive a message buffer from the socket
	 */
    void receive(Socket* socket, MessageBuffer& msgBuffer);

    EventRetractionHandle eventRetraction; /* FIXME to be suppressed */

    Handle getFederation() const
    {
        return federation;
    };

    void setFederation(Handle federation)
    {
        this->federation = federation;
    };

    FederateHandle getFederate() const
    {
        return federate;
    };

    void setFederate(FederateHandle federate)
    {
        this->federate = federate;
    };

protected:
    /** 
	 * The message name.
	 * It should be initialized by the specialized
	 * network message constructor.
	 * From a performance point of view it's better to keep
	 * this as a plain char* because it won't be changed
	 * it's thus cheaper to build than an std::string.
	 */
    const char* messageName;

    /** 
	 * The network message type
	 * type field cannot be accessed directly 
	 *   - only NM constructor may set it.
	 *   - getter should be used to get it. 
	 */
    Type type;

    /**
	 * The exception type
	 * if the message is carrying an exception
	 */
    Exception::Type exception;

    /**
	 * The exception reason (if the message carry one)
	 */
    std::string exceptionReason;

    /**
	 * The federation handle
	 * the message is part of this federation activity
	 */
    Handle federation;

    /**
	 * The federate handle
	 * the message is for this particular federate
	 */
    FederateHandle federate;

private:
};

std::ostream& operator<<(std::ostream& os, const NetworkMessage& msg);

// BUG: FIXME this is used by SocketMC and should
//      be thrown away as soon as possible.
#define TAILLE_MSG_RESEAU sizeof(NetworkMessage)

} // namespace certi

#endif // CERTI_NETWORK_MESSAGE_HH
