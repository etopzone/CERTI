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
//
// ----------------------------------------------------------------------------

#include "NetworkMessage.hh"
#include "PrettyDebug.hh"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using std::vector;

namespace certi {
static PrettyDebug G("GENDOC", __FILE__);

// ----------------------------------------------------------------------------
NetworkMessage::NetworkMessage() : type(Type::NOT_USED), exception(Exception::Type::NO_EXCEPTION)
{
    messageName = "NetworkMessage (generic)";
    exceptionReason = "Not Assigned";
    federation = 0;
    federate = 0;

} /* end of NetworkMessage() */

NetworkMessage::~NetworkMessage()
{
}

std::ostream& NetworkMessage::show(std::ostream& out)
{
    out << "[NetworkMessage -Begin]" << std::endl;
    if (isDated()) {
        out << " date       = " << getDate() << std::endl;
    }
    out << " federation = " << federation << std::endl;
    out << " federate   = " << federate << std::endl;
    out << "[NetworkMessage -End]" << std::endl;
    return out;
}

#define STRINGIZE(m) #m
#define CASE(msg_)                                                                                                     \
    case msg_:                                                                                                         \
        return STRINGIZE(msg_);

std::string NetworkMessage::to_string(const NetworkMessage::Type& msg_type)
{
    switch(msg_type) {
//         CASE(NetworkMessage::Type::NOT_USED)
        CASE(NetworkMessage::Type::CLOSE_CONNEXION)
        CASE(NetworkMessage::Type::MESSAGE_NULL)
        CASE(NetworkMessage::Type::CREATE_FEDERATION_EXECUTION)
        CASE(NetworkMessage::Type::DESTROY_FEDERATION_EXECUTION)
        CASE(NetworkMessage::Type::JOIN_FEDERATION_EXECUTION)
        CASE(NetworkMessage::Type::ADDITIONAL_FOM_MODULE)
        CASE(NetworkMessage::Type::RESIGN_FEDERATION_EXECUTION)
        CASE(NetworkMessage::Type::SET_TIME_REGULATING)
        CASE(NetworkMessage::Type::SET_TIME_CONSTRAINED)
        CASE(NetworkMessage::Type::TIME_REGULATION_ENABLED)
        CASE(NetworkMessage::Type::TIME_CONSTRAINED_ENABLED)
        CASE(NetworkMessage::Type::REGISTER_FEDERATION_SYNCHRONIZATION_POINT)
        CASE(NetworkMessage::Type::CONFIRM_SYNCHRONIZATION_POINT_REGISTRATION)
        CASE(NetworkMessage::Type::ANNOUNCE_SYNCHRONIZATION_POINT)
        CASE(NetworkMessage::Type::SYNCHRONIZATION_POINT_ACHIEVED)
        CASE(NetworkMessage::Type::FEDERATION_SYNCHRONIZED)
        CASE(NetworkMessage::Type::REQUEST_FEDERATION_SAVE)
        CASE(NetworkMessage::Type::FEDERATE_SAVE_BEGUN)
        CASE(NetworkMessage::Type::FEDERATE_SAVE_COMPLETE)
        CASE(NetworkMessage::Type::FEDERATE_SAVE_NOT_COMPLETE)
        CASE(NetworkMessage::Type::INITIATE_FEDERATE_SAVE)
        CASE(NetworkMessage::Type::FEDERATION_SAVED)
        CASE(NetworkMessage::Type::FEDERATION_NOT_SAVED)
        CASE(NetworkMessage::Type::REQUEST_FEDERATION_RESTORE)
        CASE(NetworkMessage::Type::FEDERATE_RESTORE_COMPLETE)
        CASE(NetworkMessage::Type::FEDERATE_RESTORE_NOT_COMPLETE)
        CASE(NetworkMessage::Type::REQUEST_FEDERATION_RESTORE_SUCCEEDED)
        CASE(NetworkMessage::Type::REQUEST_FEDERATION_RESTORE_FAILED)
        CASE(NetworkMessage::Type::FEDERATION_RESTORE_BEGUN)
        CASE(NetworkMessage::Type::INITIATE_FEDERATE_RESTORE)
        CASE(NetworkMessage::Type::FEDERATION_RESTORED)
        CASE(NetworkMessage::Type::FEDERATION_NOT_RESTORED)
        CASE(NetworkMessage::Type::PUBLISH_OBJECT_CLASS)
        CASE(NetworkMessage::Type::UNPUBLISH_OBJECT_CLASS)
        CASE(NetworkMessage::Type::PUBLISH_INTERACTION_CLASS)
        CASE(NetworkMessage::Type::UNPUBLISH_INTERACTION_CLASS)
        CASE(NetworkMessage::Type::SUBSCRIBE_OBJECT_CLASS)
        CASE(NetworkMessage::Type::UNSUBSCRIBE_OBJECT_CLASS)
        CASE(NetworkMessage::Type::SUBSCRIBE_INTERACTION_CLASS)
        CASE(NetworkMessage::Type::UNSUBSCRIBE_INTERACTION_CLASS)
        CASE(NetworkMessage::Type::TURN_INTERACTIONS_ON) 
        CASE(NetworkMessage::Type::TURN_INTERACTIONS_OFF) 
        CASE(NetworkMessage::Type::REGISTER_OBJECT)
        CASE(NetworkMessage::Type::DISCOVER_OBJECT) 
        CASE(NetworkMessage::Type::UPDATE_ATTRIBUTE_VALUES)
        CASE(NetworkMessage::Type::REFLECT_ATTRIBUTE_VALUES) 
        CASE(NetworkMessage::Type::SEND_INTERACTION)
        CASE(NetworkMessage::Type::RECEIVE_INTERACTION) 
        CASE(NetworkMessage::Type::DELETE_OBJECT)
        CASE(NetworkMessage::Type::REMOVE_OBJECT)
        CASE(NetworkMessage::Type::CHANGE_ATTRIBUTE_TRANSPORT_TYPE)
        CASE(NetworkMessage::Type::CHANGE_ATTRIBUTE_ORDER_TYPE)
        CASE(NetworkMessage::Type::CHANGE_INTERACTION_TRANSPORT_TYPE)
        CASE(NetworkMessage::Type::CHANGE_INTERACTION_ORDER_TYPE)
        CASE(NetworkMessage::Type::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE)
        CASE(NetworkMessage::Type::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE)
        CASE(NetworkMessage::Type::IS_ATTRIBUTE_OWNED_BY_FEDERATE)
        CASE(NetworkMessage::Type::QUERY_ATTRIBUTE_OWNERSHIP)
        CASE(NetworkMessage::Type::ATTRIBUTE_IS_NOT_OWNED)
        CASE(NetworkMessage::Type::INFORM_ATTRIBUTE_OWNERSHIP)
        CASE(NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_BASE)
        CASE(NetworkMessage::Type::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE)
        CASE(NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION)
        CASE(NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION)
        CASE(NetworkMessage::Type::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION)
        CASE(NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_UNAVAILABLE)
        CASE(NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE)
        CASE(NetworkMessage::Type::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE)
        CASE(NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_ACQUISITION)
        CASE(NetworkMessage::Type::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE)
        CASE(NetworkMessage::Type::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE)
        CASE(NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE)
        CASE(NetworkMessage::Type::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION)
        CASE(NetworkMessage::Type::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION)
        CASE(NetworkMessage::Type::DDM_CREATE_REGION)
        CASE(NetworkMessage::Type::DDM_MODIFY_REGION)
        CASE(NetworkMessage::Type::DDM_DELETE_REGION)
        CASE(NetworkMessage::Type::DDM_ASSOCIATE_REGION)
        CASE(NetworkMessage::Type::DDM_REGISTER_OBJECT)
        CASE(NetworkMessage::Type::DDM_UNASSOCIATE_REGION)
        CASE(NetworkMessage::Type::DDM_SUBSCRIBE_ATTRIBUTES)
        CASE(NetworkMessage::Type::DDM_UNSUBSCRIBE_ATTRIBUTES)
        CASE(NetworkMessage::Type::DDM_SUBSCRIBE_INTERACTION)
        CASE(NetworkMessage::Type::DDM_UNSUBSCRIBE_INTERACTION)
        CASE(NetworkMessage::Type::PROVIDE_ATTRIBUTE_VALUE_UPDATE)
        CASE(NetworkMessage::Type::SET_CLASS_RELEVANCE_ADVISORY_SWITCH)
        CASE(NetworkMessage::Type::SET_INTERACTION_RELEVANCE_ADVISORY_SWITCH)
        CASE(NetworkMessage::Type::SET_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH)
        CASE(NetworkMessage::Type::SET_ATTRIBUTE_SCOPE_ADVISORY_SWITCH)
        CASE(NetworkMessage::Type::START_REGISTRATION_FOR_OBJECT_CLASS)
        CASE(NetworkMessage::Type::STOP_REGISTRATION_FOR_OBJECT_CLASS)
        CASE(NetworkMessage::Type::RESERVE_OBJECT_INSTANCE_NAME)
        CASE(NetworkMessage::Type::RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED)
        CASE(NetworkMessage::Type::RESERVE_OBJECT_INSTANCE_NAME_FAILED)
        CASE(NetworkMessage::Type::MESSAGE_NULL_PRIME)
        CASE(NetworkMessage::Type::ENABLE_ASYNCHRONOUS_DELIVERY)
        CASE(NetworkMessage::Type::DISABLE_ASYNCHRONOUS_DELIVERY)
        CASE(NetworkMessage::Type::TIME_ADVANCE_REQUEST)
        CASE(NetworkMessage::Type::TIME_ADVANCE_REQUEST_AVAILABLE)
        CASE(NetworkMessage::Type::NEXT_MESSAGE_REQUEST)
        CASE(NetworkMessage::Type::NEXT_MESSAGE_REQUEST_AVAILABLE)
        CASE(NetworkMessage::Type::TIME_STATE_UPDATE)
        CASE(NetworkMessage::Type::MOM_STATUS)
//         CASE(NetworkMessage::Type::LAST)
        default:
            return "Unknown NetworkMessage::Type";
    }
}

std::ostream& operator<<(std::ostream& os, const NetworkMessage& msg)
{
    return const_cast<NetworkMessage&>(msg).show(os);
}

} // namespace certi
