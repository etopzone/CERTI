// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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
// ----------------------------------------------------------------------------

#ifndef CERTI_MESSAGE_HH
#define CERTI_MESSAGE_HH

#include "BasicMessage.hh"
#include "Exception.hh"
#include "FedTimeD.hh"
#include "GAV.hh"
#include "SocketUN.hh"

#include <string>
#include <vector>

namespace certi {

/**
 * The Message class is used to formalize messages that are going to be
 * exchanged between the RTI and the federate. In fact with current
 * CERTI architecture those messages are exchanged between the federate
 * process (though libRTI call) and the RTIA process.
 * In turn RTIA exchange messages with RTIG process.
 * @ingroup libCERTI
 */
class CERTI_EXPORT Message : public BasicMessage {
public:
    enum Type {
        NOT_USED = 0,
        OPEN_CONNEXION,
        CLOSE_CONNEXION,

        // gestion federation
        CREATE_FEDERATION_EXECUTION, // Legacy version for backward compatibility, especially with jCerti
        DESTROY_FEDERATION_EXECUTION,
        JOIN_FEDERATION_EXECUTION, // Legacy version for backward compatibility, especially with jCerti
        RESIGN_FEDERATION_EXECUTION,
        REGISTER_FEDERATION_SYNCHRONIZATION_POINT,
        SYNCHRONIZATION_POINT_REGISTRATION_FAILED,
        SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED,
        ANNOUNCE_SYNCHRONIZATION_POINT,
        SYNCHRONIZATION_POINT_ACHIEVED,
        FEDERATION_SYNCHRONIZED,
        REQUEST_FEDERATION_SAVE,
        INITIATE_FEDERATE_SAVE,
        FEDERATE_SAVE_BEGUN,
        FEDERATE_SAVE_COMPLETE,
        FEDERATE_SAVE_NOT_COMPLETE,
        FEDERATION_SAVED,
        FEDERATION_NOT_SAVED,
        REQUEST_FEDERATION_RESTORE,
        REQUEST_FEDERATION_RESTORE_FAILED,
        REQUEST_FEDERATION_RESTORE_SUCCEEDED,
        INITIATE_FEDERATE_RESTORE,
        FEDERATE_RESTORE_COMPLETE,
        FEDERATE_RESTORE_NOT_COMPLETE,
        FEDERATION_RESTORED,
        FEDERATION_NOT_RESTORED,
        FEDERATION_RESTORE_BEGUN,

        // Declaration
        PUBLISH_OBJECT_CLASS,
        UNPUBLISH_OBJECT_CLASS,
        PUBLISH_INTERACTION_CLASS,
        UNPUBLISH_INTERACTION_CLASS,
        SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES,
        UNSUBSCRIBE_OBJECT_CLASS,
        SUBSCRIBE_INTERACTION_CLASS,
        UNSUBSCRIBE_INTERACTION_CLASS,
        START_REGISTRATION_FOR_OBJECT_CLASS,
        STOP_REGISTRATION_FOR_OBJECT_CLASS,
        TURN_INTERACTIONS_ON,
        TURN_INTERACTIONS_OFF,

        // Object
        REGISTER_OBJECT_INSTANCE,
        UPDATE_ATTRIBUTE_VALUES,
        DISCOVER_OBJECT_INSTANCE,
        REFLECT_ATTRIBUTE_VALUES,
        SEND_INTERACTION,
        RECEIVE_INTERACTION,
        DELETE_OBJECT_INSTANCE,
        LOCAL_DELETE_OBJECT_INSTANCE,
        REMOVE_OBJECT_INSTANCE,
        CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE,
        CHANGE_INTERACTION_TRANSPORTATION_TYPE,
        REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE,
        REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE,
        PROVIDE_ATTRIBUTE_VALUE_UPDATE,
        ATTRIBUTES_IN_SCOPE,
        ATTRIBUTES_OUT_OF_SCOPE,
        TURN_UPDATES_ON_FOR_OBJECT_INSTANCE,
        TURN_UPDATES_OFF_FOR_OBJECT_INSTANCE,

        // Ownership
        REQUEST_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
        REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION,
        NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
        ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION,
        ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION,
        REQUEST_ATTRIBUTE_OWNERSHIP_ACQUISITION,
        REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE,
        QUERY_ATTRIBUTE_OWNERSHIP,
        INFORM_ATTRIBUTE_OWNERSHIP,
        IS_ATTRIBUTE_OWNED_BY_FEDERATE,
        ATTRIBUTE_IS_NOT_OWNED,
        ATTRIBUTE_OWNED_BY_RTI,
        ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE,
        ATTRIBUTE_OWNERSHIP_UNAVAILABLE,
        UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
        ATTRIBUTE_OWNERSHIP_ACQUISITION,
        CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
        ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE,
        CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION,
        CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION,

        // Time
        CHANGE_ATTRIBUTE_ORDER_TYPE,
        CHANGE_INTERACTION_ORDER_TYPE,
        ENABLE_TIME_REGULATION,
        DISABLE_TIME_REGULATION,
        ENABLE_TIME_CONSTRAINED,
        DISABLE_TIME_CONSTRAINED,
        QUERY_LBTS,
        QUERY_FEDERATE_TIME,
        QUERY_MIN_NEXT_EVENT_TIME,
        MODIFY_LOOKAHEAD,
        QUERY_LOOKAHEAD,
        RETRACT,
        REQUEST_RETRACTION,
        TIME_ADVANCE_REQUEST,
        TIME_ADVANCE_REQUEST_AVAILABLE,
        NEXT_EVENT_REQUEST,
        NEXT_EVENT_REQUEST_AVAILABLE,
        FLUSH_QUEUE_REQUEST,
        TIME_ADVANCE_GRANT,
        ENABLE_ASYNCHRONOUS_DELIVERY,
        DISABLE_ASYNCHRONOUS_DELIVERY,
        TIME_REGULATION_ENABLED,
        TIME_CONSTRAINED_ENABLED,

        // Data Distribution Management
        DDM_CREATE_REGION,
        DDM_MODIFY_REGION,
        DDM_DELETE_REGION,
        DDM_REGISTER_OBJECT,
        DDM_ASSOCIATE_REGION,
        DDM_UNASSOCIATE_REGION,
        DDM_SUBSCRIBE_ATTRIBUTES,
        DDM_UNSUBSCRIBE_ATTRIBUTES,
        DDM_SUBSCRIBE_INTERACTION,
        DDM_UNSUBSCRIBE_INTERACTION,
        DDM_REQUEST_UPDATE,

        // Support Services
        GET_OBJECT_CLASS_HANDLE,
        GET_OBJECT_CLASS_NAME,
        GET_ATTRIBUTE_HANDLE,
        GET_ATTRIBUTE_NAME,
        GET_INTERACTION_CLASS_HANDLE,
        GET_INTERACTION_CLASS_NAME,
        GET_PARAMETER_HANDLE,
        GET_PARAMETER_NAME,
        GET_OBJECT_INSTANCE_HANDLE,
        GET_OBJECT_INSTANCE_NAME,
        GET_SPACE_HANDLE,
        GET_SPACE_NAME,
        GET_DIMENSION_HANDLE,
        GET_DIMENSION_NAME,
        GET_ATTRIBUTE_SPACE_HANDLE,
        GET_OBJECT_CLASS,
        GET_INTERACTION_SPACE_HANDLE,
        GET_TRANSPORTATION_HANDLE,
        GET_TRANSPORTATION_NAME,
        GET_ORDERING_HANDLE,
        GET_ORDERING_NAME,
        ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH,
        DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH,
        ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH,
        DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH,
        ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH,
        DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH,
        ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH,
        DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH,
        TICK_REQUEST,
        TICK_REQUEST_NEXT,
        TICK_REQUEST_STOP,
        RESERVE_OBJECT_INSTANCE_NAME, // HLA1516
        RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED, // HLA1516
        RESERVE_OBJECT_INSTANCE_NAME_FAILED, // HLA1516
        
        CREATE_FEDERATION_EXECUTION_V4, // CERTI V4 C++
        JOIN_FEDERATION_EXECUTION_V4, // CERTI V4 C++

        LAST // should be the "last" (not used)
    };
    
    static constexpr size_t the_message_type_count = static_cast<size_t>(LAST);

    static std::string to_string(const Message::Type& msg_type);

public:
    /** Default Message creator */
    Message();

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
    Type getMessageType() const
    {
        return type;
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
	 * Send a serialized message on a socket.
	 * @param[in] socket the socket that should be used to send the message
	 * @param[in] msgBuffer the buffer containing the serialized message
	 */
    void send(SocketUN* socket, MessageBuffer& msgBuffer);

    /**
	 * Receive a serialized message from a socket.
	 * @param[in] socket the socket used to received the message from
	 * @param[out] msgBuffer the buffer were the read message will be written
	 */
    void receive(SocketUN* socket, MessageBuffer& msgBuffer);

    void setException(const Exception::Type, const std::string& the_reason = "");
    Exception::Type getExceptionType() const
    {
        return exception;
    };
    const char* getExceptionReason() const
    {
        return exceptionReason.c_str();
    };
    std::wstring getExceptionReasonW() const
    {
        return std::wstring(exceptionReason.begin(), exceptionReason.end());
    };

protected:
    Type type;
    Exception::Type exception;
    std::string exceptionReason;

    ResignAction resignAction;
    EventRetractionHandle eventRetraction;
    SpaceHandle space;
    DimensionHandle dimension;
    const char* messageName;

private:
    Message& operator=(const Message&);
};

std::ostream& operator<<(std::ostream& os, const Message& msg);

} // namespace certi

#endif // CERTI_MESSAGE_HH
