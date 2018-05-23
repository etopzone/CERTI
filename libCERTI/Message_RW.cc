// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2006  ONERA
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// ----------------------------------------------------------------------------

#include "Message.hh"
#include <cassert>
#include <iostream>

namespace certi {

static PrettyDebug D("RTIA_MSG", "Message::");
static PrettyDebug G("GENDOC", __FILE__);

std::ostream& Message::show(std::ostream& out)
{
    out << "[Message -Begin]" << std::endl;
    out << "     type = " << type << std::endl;
    out << "     exception = " << static_cast<unsigned short>(exception) << std::endl;
    if (exception != Exception::Type::NO_EXCEPTION) {
        out << "     exceptionReason = " << exceptionReason << std::endl;
    }
    else {
        BasicMessage::show(out);
    }
    out << "[Message -End]" << std::endl;
    return out;
} /* end of show */

#define STRINGIZE(m) #m
#define CASE(msg_)                                                                                                     \
    case msg_:                                                                                                         \
        return STRINGIZE(msg_);

std::string Message::to_string(const Message::Type& msg_type)
{
    switch (msg_type) {
        //         CASE(Message::NOT_USED)
        CASE(Message::OPEN_CONNEXION)
        CASE(Message::CLOSE_CONNEXION)
        CASE(Message::CREATE_FEDERATION_EXECUTION)
        CASE(Message::DESTROY_FEDERATION_EXECUTION)
        CASE(Message::JOIN_FEDERATION_EXECUTION)
        CASE(Message::RESIGN_FEDERATION_EXECUTION)
        CASE(Message::REGISTER_FEDERATION_SYNCHRONIZATION_POINT)
        CASE(Message::SYNCHRONIZATION_POINT_REGISTRATION_FAILED)
        CASE(Message::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED)
        CASE(Message::ANNOUNCE_SYNCHRONIZATION_POINT)
        CASE(Message::SYNCHRONIZATION_POINT_ACHIEVED)
        CASE(Message::FEDERATION_SYNCHRONIZED)
        CASE(Message::REQUEST_FEDERATION_SAVE)
        CASE(Message::INITIATE_FEDERATE_SAVE)
        CASE(Message::FEDERATE_SAVE_BEGUN)
        CASE(Message::FEDERATE_SAVE_COMPLETE)
        CASE(Message::FEDERATE_SAVE_NOT_COMPLETE)
        CASE(Message::FEDERATION_SAVED)
        CASE(Message::FEDERATION_NOT_SAVED)
        CASE(Message::REQUEST_FEDERATION_RESTORE)
        CASE(Message::REQUEST_FEDERATION_RESTORE_FAILED)
        CASE(Message::REQUEST_FEDERATION_RESTORE_SUCCEEDED)
        CASE(Message::INITIATE_FEDERATE_RESTORE)
        CASE(Message::FEDERATE_RESTORE_COMPLETE)
        CASE(Message::FEDERATE_RESTORE_NOT_COMPLETE)
        CASE(Message::FEDERATION_RESTORED)
        CASE(Message::FEDERATION_NOT_RESTORED)
        CASE(Message::FEDERATION_RESTORE_BEGUN)
        CASE(Message::PUBLISH_OBJECT_CLASS)
        CASE(Message::UNPUBLISH_OBJECT_CLASS)
        CASE(Message::PUBLISH_INTERACTION_CLASS)
        CASE(Message::UNPUBLISH_INTERACTION_CLASS)
        CASE(Message::SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES)
        CASE(Message::UNSUBSCRIBE_OBJECT_CLASS)
        CASE(Message::SUBSCRIBE_INTERACTION_CLASS)
        CASE(Message::UNSUBSCRIBE_INTERACTION_CLASS)
        CASE(Message::START_REGISTRATION_FOR_OBJECT_CLASS)
        CASE(Message::STOP_REGISTRATION_FOR_OBJECT_CLASS)
        CASE(Message::TURN_INTERACTIONS_ON)
        CASE(Message::TURN_INTERACTIONS_OFF)
        CASE(Message::REGISTER_OBJECT_INSTANCE)
        CASE(Message::UPDATE_ATTRIBUTE_VALUES)
        CASE(Message::DISCOVER_OBJECT_INSTANCE)
        CASE(Message::REFLECT_ATTRIBUTE_VALUES)
        CASE(Message::SEND_INTERACTION)
        CASE(Message::RECEIVE_INTERACTION)
        CASE(Message::DELETE_OBJECT_INSTANCE)
        CASE(Message::LOCAL_DELETE_OBJECT_INSTANCE)
        CASE(Message::REMOVE_OBJECT_INSTANCE)
        CASE(Message::CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE)
        CASE(Message::CHANGE_INTERACTION_TRANSPORTATION_TYPE)
        CASE(Message::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE)
        CASE(Message::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE)
        CASE(Message::PROVIDE_ATTRIBUTE_VALUE_UPDATE)
        CASE(Message::ATTRIBUTES_IN_SCOPE)
        CASE(Message::ATTRIBUTES_OUT_OF_SCOPE)
        CASE(Message::TURN_UPDATES_ON_FOR_OBJECT_INSTANCE)
        CASE(Message::TURN_UPDATES_OFF_FOR_OBJECT_INSTANCE)
        CASE(Message::REQUEST_ATTRIBUTE_OWNERSHIP_DIVESTITURE)
        CASE(Message::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION)
        CASE(Message::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE)
        CASE(Message::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION)
        CASE(Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION)
        CASE(Message::REQUEST_ATTRIBUTE_OWNERSHIP_ACQUISITION)
        CASE(Message::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE)
        CASE(Message::QUERY_ATTRIBUTE_OWNERSHIP)
        CASE(Message::INFORM_ATTRIBUTE_OWNERSHIP)
        CASE(Message::IS_ATTRIBUTE_OWNED_BY_FEDERATE)
        CASE(Message::ATTRIBUTE_IS_NOT_OWNED)
        CASE(Message::ATTRIBUTE_OWNED_BY_RTI)
        CASE(Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE)
        CASE(Message::ATTRIBUTE_OWNERSHIP_UNAVAILABLE)
        CASE(Message::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE)
        CASE(Message::ATTRIBUTE_OWNERSHIP_ACQUISITION)
        CASE(Message::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE)
        CASE(Message::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE)
        CASE(Message::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION)
        CASE(Message::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION)
        CASE(Message::CHANGE_ATTRIBUTE_ORDER_TYPE)
        CASE(Message::CHANGE_INTERACTION_ORDER_TYPE)
        CASE(Message::ENABLE_TIME_REGULATION)
        CASE(Message::DISABLE_TIME_REGULATION)
        CASE(Message::ENABLE_TIME_CONSTRAINED)
        CASE(Message::DISABLE_TIME_CONSTRAINED)
        CASE(Message::QUERY_LBTS)
        CASE(Message::QUERY_FEDERATE_TIME)
        CASE(Message::QUERY_MIN_NEXT_EVENT_TIME)
        CASE(Message::MODIFY_LOOKAHEAD)
        CASE(Message::QUERY_LOOKAHEAD)
        CASE(Message::RETRACT)
        CASE(Message::REQUEST_RETRACTION)
        CASE(Message::TIME_ADVANCE_REQUEST)
        CASE(Message::TIME_ADVANCE_REQUEST_AVAILABLE)
        CASE(Message::NEXT_EVENT_REQUEST)
        CASE(Message::NEXT_EVENT_REQUEST_AVAILABLE)
        CASE(Message::FLUSH_QUEUE_REQUEST)
        CASE(Message::TIME_ADVANCE_GRANT)
        CASE(Message::ENABLE_ASYNCHRONOUS_DELIVERY)
        CASE(Message::DISABLE_ASYNCHRONOUS_DELIVERY)
        CASE(Message::TIME_REGULATION_ENABLED)
        CASE(Message::TIME_CONSTRAINED_ENABLED)
        CASE(Message::DDM_CREATE_REGION)
        CASE(Message::DDM_MODIFY_REGION)
        CASE(Message::DDM_DELETE_REGION)
        CASE(Message::DDM_REGISTER_OBJECT)
        CASE(Message::DDM_ASSOCIATE_REGION)
        CASE(Message::DDM_UNASSOCIATE_REGION)
        CASE(Message::DDM_SUBSCRIBE_ATTRIBUTES)
        CASE(Message::DDM_UNSUBSCRIBE_ATTRIBUTES)
        CASE(Message::DDM_SUBSCRIBE_INTERACTION)
        CASE(Message::DDM_UNSUBSCRIBE_INTERACTION)
        CASE(Message::DDM_REQUEST_UPDATE)
        CASE(Message::GET_OBJECT_CLASS_HANDLE)
        CASE(Message::GET_OBJECT_CLASS_NAME)
        CASE(Message::GET_ATTRIBUTE_HANDLE)
        CASE(Message::GET_ATTRIBUTE_NAME)
        CASE(Message::GET_INTERACTION_CLASS_HANDLE)
        CASE(Message::GET_INTERACTION_CLASS_NAME)
        CASE(Message::GET_PARAMETER_HANDLE)
        CASE(Message::GET_PARAMETER_NAME)
        CASE(Message::GET_OBJECT_INSTANCE_HANDLE)
        CASE(Message::GET_OBJECT_INSTANCE_NAME)
        CASE(Message::GET_SPACE_HANDLE)
        CASE(Message::GET_SPACE_NAME)
        CASE(Message::GET_DIMENSION_HANDLE)
        CASE(Message::GET_DIMENSION_NAME)
        CASE(Message::GET_ATTRIBUTE_SPACE_HANDLE)
        CASE(Message::GET_OBJECT_CLASS)
        CASE(Message::GET_INTERACTION_SPACE_HANDLE)
        CASE(Message::GET_TRANSPORTATION_HANDLE)
        CASE(Message::GET_TRANSPORTATION_NAME)
        CASE(Message::GET_ORDERING_HANDLE)
        CASE(Message::GET_ORDERING_NAME)
        CASE(Message::ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH)
        CASE(Message::DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH)
        CASE(Message::ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH)
        CASE(Message::DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH)
        CASE(Message::ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH)
        CASE(Message::DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH)
        CASE(Message::ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH)
        CASE(Message::DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH)
        CASE(Message::TICK_REQUEST)
        CASE(Message::TICK_REQUEST_NEXT)
        CASE(Message::TICK_REQUEST_STOP)
        CASE(Message::RESERVE_OBJECT_INSTANCE_NAME)
        CASE(Message::RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED)
        CASE(Message::RESERVE_OBJECT_INSTANCE_NAME_FAILED)
        CASE(Message::CREATE_FEDERATION_EXECUTION_V4)
        CASE(Message::JOIN_FEDERATION_EXECUTION_V4)
    //         CASE(Message::LAST)
    default:
        return "Unknown NetworkMessage::Type";
    }
}

std::ostream& operator<<(std::ostream& os, const Message& msg)
{
    return const_cast<Message&>(msg).show(os);
}

void Message::serialize(MessageBuffer& msgBuffer)
{
    Debug(G, pdGendoc) << "enter Message::serialize" << std::endl;
    if ((type == NOT_USED) || (type == LAST)) {
        throw RTIinternalError("Invalid network type (not a valid type);");
    }
    Debug(D, pdDebug) << "Serialize <" << getMessageName() << ">" << std::endl;
    //show(std::cerr);
    /* type of message */
    msgBuffer.write_int32(type);
    msgBuffer.write_int32(static_cast<int32_t>(exception));
    if (exception != Exception::Type::NO_EXCEPTION) {
        msgBuffer.write_string(exceptionReason);
    }
    else {
        BasicMessage::serialize(msgBuffer);
    }
    Debug(G, pdGendoc) << "exit Message::serialize" << std::endl;
} /* end of serialize */

void Message::deserialize(MessageBuffer& msgBuffer)
{
    Debug(G, pdGendoc) << "enter Message::deserialize" << std::endl;
    /* We serialize the common Message part
	 * ALL Messages will contain the following
	 */
    Debug(D, pdDebug) << "Deserialize <" << getMessageName() << ">" << std::endl;
    /* deserialize common part */
    type = static_cast<Message::Type>(msgBuffer.read_int32());
    exception = static_cast<Exception::Type>(msgBuffer.read_int32());
    if (exception != Exception::Type::NO_EXCEPTION) {
        msgBuffer.read_string(exceptionReason);
    }
    else {
        BasicMessage::deserialize(msgBuffer);
    }
    Debug(G, pdGendoc) << "exit Message::deserialize" << std::endl;
} /* end of deserialize */

void Message::send(SocketUN* socket, MessageBuffer& msgBuffer)
{
    Debug(G, pdGendoc) << "enter Message::send" << std::endl;
    /* 0- reset send buffer */
    msgBuffer.reset();
    /* 1- serialize the message
	 * This is a polymorphic call
	 * which may specialized in a daughter class
	 */
    serialize(msgBuffer);
    /* 2- update message buffer 'reserved bytes' header */
    msgBuffer.updateReservedBytes();
    Debug(D, pdDebug) << "Sending <" << getMessageName() << "> whose buffer has <" << msgBuffer.size() << "> bytes"
                      << std::endl;
    //msgBuf.show(msgBuffer(0),5);
    /* 3- effectively send the raw message to socket */
    socket->send(static_cast<unsigned char*>(msgBuffer(0)), msgBuffer.size());
    Debug(G, pdGendoc) << "exit  Message::send" << std::endl;
} /* end of send */

void Message::receive(SocketUN* socket, MessageBuffer& msgBuffer)
{
    Debug(G, pdGendoc) << "enter Message::receive" << std::endl;
    /* 0- Reset receive buffer */
    /* FIXME this reset may not be necessary since we do
	 * raw-receive + assume-size
	 */
    msgBuffer.reset();
    /* 1- Read 'reserved bytes' header from socket */
    Debug(D, pdDebug) << "(recv) Reading " << msgBuffer.reservedBytes << " 'reserved' bytes" << std::endl;
    socket->receive(static_cast<const unsigned char*>(msgBuffer(0)), msgBuffer.reservedBytes);
    //msgBuffer.show(msgBuffer(0),5);fflush(stdout);
    /* 2- update (assume) complete message size from reserved bytes */
    msgBuffer.assumeSizeFromReservedBytes();
    Debug(D, pdDebug) << "Got a MsgBuffer of size " << msgBuffer.size() << " bytes (including "
                      << msgBuffer.reservedBytes << " reserved)" << std::endl;
    /* 3- receive the rest of the message */
    socket->receive(static_cast<const unsigned char*>(msgBuffer(msgBuffer.reservedBytes)),
                    msgBuffer.size() - msgBuffer.reservedBytes);
    /* 4- deserialize the message
	 * This is a polymorphic call
	 * which may specialized in a daughter class
	 */
    deserialize(msgBuffer);
    Debug(G, pdGendoc) << "exit  Message::receive" << std::endl;
} /* end of receive */

/*
void
Message::readHandleArray(MessageBuffer &msgBuffer)
{
    handleArraySize = msgBuffer.read_uint16() ;
    handleArray.resize(handleArraySize) ;
    for ( short i=0 ; i<handleArraySize ; i++)
      {
      handleArray[i] = msgBuffer.read_uint64() ;
      }
}

void
Message::writeHandleArray(MessageBuffer &msgBuffer)
{
    msgBuffer.write_uint16(handleArraySize) ;
    for ( short i=0 ; i<handleArraySize ; i++)
      {
      msgBuffer.write_uint64(handleArray[i]) ;
      }
}
 */

} // namespace certi
