// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// ----------------------------------------------------------------------------

#include <config.h>
#include "Statistics.hh"

#include <cstdlib>
#include <iostream>
#include <string>

using std::cout ;
using std::endl ;
using std::string ;

namespace certi {
namespace rtia {

std::map<Message::Type, std::string> Statistics::fedMessageName ;
std::map<NetworkMessage::Type, std::string> Statistics::rtiMessageName ;

// ----------------------------------------------------------------------------
//! Initialize the two set used for collecting number of messages exchanged.
Statistics::Statistics()
{
    //char *statEnv = getenv("CERTI_NO_STATISTICS");
    myDisplay = true ;
    myDisplayZero = false ;

#define FedMsgName(msg_) fedMessageName[msg_] = #msg_
    FedMsgName(Message::CREATE_FEDERATION_EXECUTION);
    FedMsgName(Message::DESTROY_FEDERATION_EXECUTION);
    FedMsgName(Message::JOIN_FEDERATION_EXECUTION);
    FedMsgName(Message::RESIGN_FEDERATION_EXECUTION);
    FedMsgName(Message::REGISTER_FEDERATION_SYNCHRONIZATION_POINT);
    FedMsgName(Message::SYNCHRONIZATION_POINT_REGISTRATION_FAILED);
    FedMsgName(Message::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED);
    FedMsgName(Message::ANNOUNCE_SYNCHRONIZATION_POINT);
    FedMsgName(Message::SYNCHRONIZATION_POINT_ACHIEVED);
    FedMsgName(Message::FEDERATION_SYNCHRONIZED);
    FedMsgName(Message::REQUEST_FEDERATION_SAVE);
    FedMsgName(Message::INITIATE_FEDERATE_SAVE);
    FedMsgName(Message::FEDERATE_SAVE_BEGUN);
    FedMsgName(Message::FEDERATE_SAVE_COMPLETE);
    FedMsgName(Message::FEDERATE_SAVE_NOT_COMPLETE);
    FedMsgName(Message::FEDERATION_SAVED);
    FedMsgName(Message::FEDERATION_NOT_SAVED);
    FedMsgName(Message::REQUEST_FEDERATION_RESTORE);
    FedMsgName(Message::REQUEST_FEDERATION_RESTORE_FAILED);
    FedMsgName(Message::REQUEST_FEDERATION_RESTORE_SUCCEEDED);
    FedMsgName(Message::INITIATE_FEDERATE_RESTORE);
    FedMsgName(Message::FEDERATE_RESTORE_COMPLETE);
    FedMsgName(Message::FEDERATE_RESTORE_NOT_COMPLETE);
    FedMsgName(Message::FEDERATION_RESTORED);
    FedMsgName(Message::FEDERATION_NOT_RESTORED);
    FedMsgName(Message::FEDERATION_RESTORE_BEGUN);

    // Declaration
    FedMsgName(Message::PUBLISH_OBJECT_CLASS);
    FedMsgName(Message::UNPUBLISH_OBJECT_CLASS);
    FedMsgName(Message::PUBLISH_INTERACTION_CLASS);
    FedMsgName(Message::UNPUBLISH_INTERACTION_CLASS);
    FedMsgName(Message::SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES);
    FedMsgName(Message::UNSUBSCRIBE_OBJECT_CLASS);
    FedMsgName(Message::SUBSCRIBE_INTERACTION_CLASS);
    FedMsgName(Message::UNSUBSCRIBE_INTERACTION_CLASS);
    FedMsgName(Message::START_REGISTRATION_FOR_OBJECT_CLASS);
    FedMsgName(Message::STOP_REGISTRATION_FOR_OBJECT_CLASS);
    FedMsgName(Message::TURN_INTERACTIONS_ON);
    FedMsgName(Message::TURN_INTERACTIONS_OFF);

    // Object
    FedMsgName(Message::REQUEST_ID);
    FedMsgName(Message::REGISTER_OBJECT_INSTANCE);
    FedMsgName(Message::UPDATE_ATTRIBUTE_VALUES);
    FedMsgName(Message::DISCOVER_OBJECT_INSTANCE);
    FedMsgName(Message::REFLECT_ATTRIBUTE_VALUES);
    FedMsgName(Message::SEND_INTERACTION);
    FedMsgName(Message::RECEIVE_INTERACTION);
    FedMsgName(Message::DELETE_OBJECT_INSTANCE);
    FedMsgName(Message::LOCAL_DELETE_OBJECT_INSTANCE);
    FedMsgName(Message::REMOVE_OBJECT_INSTANCE);
    FedMsgName(Message::CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE);
    FedMsgName(Message::CHANGE_INTERACTION_TRANSPORTATION_TYPE);
    FedMsgName(Message::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE);
    FedMsgName(Message::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE);
    FedMsgName(Message::PROVIDE_ATTRIBUTE_VALUE_UPDATE);
    FedMsgName(Message::ATTRIBUTES_IN_SCOPE);
    FedMsgName(Message::ATTRIBUTES_OUT_OF_SCOPE);
    FedMsgName(Message::TURN_UPDATES_ON_FOR_OBJECT_INSTANCE);
    FedMsgName(Message::TURN_UPDATES_OFF_FOR_OBJECT_INSTANCE);

    // Ownership
    FedMsgName(Message::REQUEST_ATTRIBUTE_OWNERSHIP_DIVESTITURE);
    FedMsgName(Message::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION);
    FedMsgName(Message::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE);
    FedMsgName(Message::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION);
    FedMsgName(Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION);
    FedMsgName(Message::REQUEST_ATTRIBUTE_OWNERSHIP_ACQUISITION);
    FedMsgName(Message::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE);
    FedMsgName(Message::QUERY_ATTRIBUTE_OWNERSHIP);
    FedMsgName(Message::INFORM_ATTRIBUTE_OWNERSHIP);
    FedMsgName(Message::IS_ATTRIBUTE_OWNED_BY_FEDERATE);
    FedMsgName(Message::ATTRIBUTE_IS_NOT_OWNED);
    FedMsgName(Message::ATTRIBUTE_OWNED_BY_RTI);
    FedMsgName(Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE);
    FedMsgName(Message::ATTRIBUTE_OWNERSHIP_UNAVAILABLE);
    FedMsgName(Message::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE);
    FedMsgName(Message::ATTRIBUTE_OWNERSHIP_ACQUISITION);
    FedMsgName(Message::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE);
    FedMsgName(Message::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE);
    FedMsgName(Message::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION);
    FedMsgName(Message::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION);

    // Time
    FedMsgName(Message::CHANGE_ATTRIBUTE_ORDER_TYPE);
    FedMsgName(Message::CHANGE_INTERACTION_ORDER_TYPE);
    FedMsgName(Message::ENABLE_TIME_REGULATION);
    FedMsgName(Message::DISABLE_TIME_REGULATION);
    FedMsgName(Message::ENABLE_TIME_CONSTRAINED);
    FedMsgName(Message::DISABLE_TIME_CONSTRAINED);
    FedMsgName(Message::QUERY_LBTS);
    FedMsgName(Message::QUERY_FEDERATE_TIME);
    FedMsgName(Message::QUERY_MIN_NEXT_EVENT_TIME);
    FedMsgName(Message::MODIFY_LOOKAHEAD);
    FedMsgName(Message::QUERY_LOOKAHEAD);
    FedMsgName(Message::RETRACT);
    FedMsgName(Message::REQUEST_RETRACTION);
    FedMsgName(Message::TIME_ADVANCE_REQUEST);
    FedMsgName(Message::TIME_ADVANCE_REQUEST_AVAILABLE);
    FedMsgName(Message::NEXT_EVENT_REQUEST);
    FedMsgName(Message::NEXT_EVENT_REQUEST_AVAILABLE);
    FedMsgName(Message::FLUSH_QUEUE_REQUEST);
    FedMsgName(Message::TIME_ADVANCE_GRANT);
    FedMsgName(Message::ENABLE_ASYNCHRONOUS_DELIVERY);
    FedMsgName(Message::DISABLE_ASYNCHRONOUS_DELIVERY);
    FedMsgName(Message::TIME_REGULATION_ENABLED);
    FedMsgName(Message::TIME_CONSTRAINED_ENABLED);

    // Data Distribution
    FedMsgName(Message::DDM_CREATE_REGION);
    FedMsgName(Message::DDM_MODIFY_REGION);
    FedMsgName(Message::DDM_DELETE_REGION);
    FedMsgName(Message::DDM_REGISTER_OBJECT);
    FedMsgName(Message::DDM_ASSOCIATE_REGION);
    FedMsgName(Message::DDM_UNASSOCIATE_REGION);
    FedMsgName(Message::DDM_SUBSCRIBE_ATTRIBUTES);
    FedMsgName(Message::DDM_UNSUBSCRIBE_ATTRIBUTES);
    FedMsgName(Message::DDM_SUBSCRIBE_INTERACTION);
    FedMsgName(Message::DDM_UNSUBSCRIBE_INTERACTION);
    FedMsgName(Message::DDM_SEND_INTERACTION);
    FedMsgName(Message::DDM_REQUEST_UPDATE);

    // Support Services
    FedMsgName(Message::GET_OBJECT_CLASS_HANDLE);
    FedMsgName(Message::GET_OBJECT_CLASS_NAME);
    FedMsgName(Message::GET_ATTRIBUTE_HANDLE);
    FedMsgName(Message::GET_ATTRIBUTE_NAME);
    FedMsgName(Message::GET_INTERACTION_CLASS_HANDLE);
    FedMsgName(Message::GET_INTERACTION_CLASS_NAME);
    FedMsgName(Message::GET_PARAMETER_HANDLE);
    FedMsgName(Message::GET_PARAMETER_NAME);
    FedMsgName(Message::GET_OBJECT_INSTANCE_HANDLE);
    FedMsgName(Message::GET_OBJECT_INSTANCE_NAME);
    FedMsgName(Message::GET_SPACE_HANDLE);
    FedMsgName(Message::GET_SPACE_NAME);
    FedMsgName(Message::GET_DIMENSION_HANDLE);
    FedMsgName(Message::GET_DIMENSION_NAME);
    FedMsgName(Message::GET_ATTRIBUTE_SPACE_HANDLE);
    FedMsgName(Message::GET_OBJECT_CLASS);
    FedMsgName(Message::GET_INTERACTION_SPACE_HANDLE);
    FedMsgName(Message::GET_TRANSPORTATION_HANDLE);
    FedMsgName(Message::GET_TRANSPORTATION_NAME);
    FedMsgName(Message::GET_ORDERING_HANDLE);
    FedMsgName(Message::GET_ORDERING_NAME);
    FedMsgName(Message::ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH);
    FedMsgName(Message::DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH);
    FedMsgName(Message::ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH);
    FedMsgName(Message::DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH);
    FedMsgName(Message::ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH);
    FedMsgName(Message::DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH);
    FedMsgName(Message::ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH);
    FedMsgName(Message::DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH);
    FedMsgName(Message::TICK_REQUEST);
#undef FedMsgName

#define RtiMsgName(msg_) rtiMessageName[msg_] = #msg_
    RtiMsgName(NetworkMessage::CLOSE_CONNEXION);
    RtiMsgName(NetworkMessage::MESSAGE_NULL);
    RtiMsgName(NetworkMessage::CREATE_FEDERATION_EXECUTION);
    RtiMsgName(NetworkMessage::DESTROY_FEDERATION_EXECUTION);
    RtiMsgName(NetworkMessage::JOIN_FEDERATION_EXECUTION);
    RtiMsgName(NetworkMessage::RESIGN_FEDERATION_EXECUTION);
    RtiMsgName(NetworkMessage::SET_TIME_REGULATING);
    RtiMsgName(NetworkMessage::SET_TIME_CONSTRAINED);
    RtiMsgName(NetworkMessage::REGISTER_FEDERATION_SYNCHRONIZATION_POINT);
    RtiMsgName(NetworkMessage::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED);
    RtiMsgName(NetworkMessage::ANNOUNCE_SYNCHRONIZATION_POINT);
    RtiMsgName(NetworkMessage::SYNCHRONIZATION_POINT_ACHIEVED);
    RtiMsgName(NetworkMessage::FEDERATION_SYNCHRONIZED);
    RtiMsgName(NetworkMessage::REQUEST_FEDERATION_SAVE);
    RtiMsgName(NetworkMessage::FEDERATE_SAVE_BEGUN);
    RtiMsgName(NetworkMessage::FEDERATE_SAVE_COMPLETE);
    RtiMsgName(NetworkMessage::FEDERATE_SAVE_NOT_COMPLETE);
    RtiMsgName(NetworkMessage::INITIATE_FEDERATE_SAVE);
    RtiMsgName(NetworkMessage::FEDERATION_SAVED);
    RtiMsgName(NetworkMessage::FEDERATION_NOT_SAVED);
    RtiMsgName(NetworkMessage::REQUEST_FEDERATION_RESTORE);
    RtiMsgName(NetworkMessage::FEDERATE_RESTORE_COMPLETE);
    RtiMsgName(NetworkMessage::FEDERATE_RESTORE_NOT_COMPLETE);
    RtiMsgName(NetworkMessage::REQUEST_FEDERATION_RESTORE_SUCCEEDED);
    RtiMsgName(NetworkMessage::REQUEST_FEDERATION_RESTORE_FAILED);
    RtiMsgName(NetworkMessage::FEDERATION_RESTORE_BEGUN);
    RtiMsgName(NetworkMessage::INITIATE_FEDERATE_RESTORE);
    RtiMsgName(NetworkMessage::FEDERATION_RESTORED);
    RtiMsgName(NetworkMessage::FEDERATION_NOT_RESTORED);
    RtiMsgName(NetworkMessage::PUBLISH_OBJECT_CLASS);
    RtiMsgName(NetworkMessage::UNPUBLISH_OBJECT_CLASS);
    RtiMsgName(NetworkMessage::PUBLISH_INTERACTION_CLASS);
    RtiMsgName(NetworkMessage::UNPUBLISH_INTERACTION_CLASS);
    RtiMsgName(NetworkMessage::SUBSCRIBE_OBJECT_CLASS);
    RtiMsgName(NetworkMessage::UNSUBSCRIBE_OBJECT_CLASS);
    RtiMsgName(NetworkMessage::SUBSCRIBE_INTERACTION_CLASS);
    RtiMsgName(NetworkMessage::UNSUBSCRIBE_INTERACTION_CLASS);
    RtiMsgName(NetworkMessage::TURN_INTERACTIONS_ON);
    RtiMsgName(NetworkMessage::TURN_INTERACTIONS_OFF);
    RtiMsgName(NetworkMessage::REQUEST_ID);
    RtiMsgName(NetworkMessage::REGISTER_OBJECT);
    RtiMsgName(NetworkMessage::DISCOVER_OBJECT);
    RtiMsgName(NetworkMessage::UPDATE_ATTRIBUTE_VALUES);
    RtiMsgName(NetworkMessage::REFLECT_ATTRIBUTE_VALUES);
    RtiMsgName(NetworkMessage::SEND_INTERACTION);
    RtiMsgName(NetworkMessage::RECEIVE_INTERACTION);
    RtiMsgName(NetworkMessage::DELETE_OBJECT);
    RtiMsgName(NetworkMessage::REMOVE_OBJECT);
    RtiMsgName(NetworkMessage::CHANGE_ATTRIBUTE_TRANSPORT_TYPE);
    RtiMsgName(NetworkMessage::CHANGE_ATTRIBUTE_ORDER_TYPE);
    RtiMsgName(NetworkMessage::CHANGE_INTERACTION_TRANSPORT_TYPE);
    RtiMsgName(NetworkMessage::CHANGE_INTERACTION_ORDER_TYPE);
    RtiMsgName(NetworkMessage::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE);
    RtiMsgName(NetworkMessage::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE);
    RtiMsgName(NetworkMessage::IS_ATTRIBUTE_OWNED_BY_FEDERATE);
    RtiMsgName(NetworkMessage::QUERY_ATTRIBUTE_OWNERSHIP);
    RtiMsgName(NetworkMessage::ATTRIBUTE_IS_NOT_OWNED);
    RtiMsgName(NetworkMessage::INFORM_ATTRIBUTE_OWNERSHIP);
    RtiMsgName(NetworkMessage::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE);
    RtiMsgName(NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION);
    RtiMsgName(NetworkMessage::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION);
    RtiMsgName(NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION);
    RtiMsgName(NetworkMessage::ATTRIBUTE_OWNERSHIP_UNAVAILABLE);
    RtiMsgName(NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE);
    RtiMsgName(NetworkMessage::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE);
    RtiMsgName(NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION);
    RtiMsgName(NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE);
    RtiMsgName(NetworkMessage::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE);
    RtiMsgName(NetworkMessage::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE);
    RtiMsgName(NetworkMessage::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION);
    RtiMsgName(NetworkMessage::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION);
    RtiMsgName(NetworkMessage::DDM_CREATE_REGION);
    RtiMsgName(NetworkMessage::DDM_MODIFY_REGION);
    RtiMsgName(NetworkMessage::DDM_DELETE_REGION);
    RtiMsgName(NetworkMessage::DDM_ASSOCIATE_REGION);
#undef addRtiName
}

// ----------------------------------------------------------------------------
//! Destructor (Does nothing).
Statistics::~Statistics()
{
}

// ----------------------------------------------------------------------------
//! Increment counter for RTIG message type received.
void
Statistics::rtiService(NetworkMessage::Type service)
{
    rtiServiceSet[service]++ ;
}

// ----------------------------------------------------------------------------
//! Increment counter for Federate message type received.
void
Statistics::federateService(Message::Type service)
{
    federateServiceSet[service]++ ;
}

// ----------------------------------------------------------------------------
//! Display collected data.
ostream &
operator<<(ostream &s, Statistics &stat)
{
    cout << endl << "RTIA: Statistics (processed messages)" << endl ;

    s << "List of federate services " << endl
      << "--------------------------------------------------" << endl ;

    int sentFederateMessages = 0 ;
    std::map<Message::Type, int>::iterator fi ;
    std::map<Message::Type, int>::iterator fe = stat.federateServiceSet.end();
    for (fi = stat.federateServiceSet.begin(); fi != fe ; ++fi) {
        int nb = fi->second ;
        if (nb || stat.displayZero()) {
            s.width(8);
            s << nb << ' ' << Statistics::fedMessageName[fi->first]
              << " (" << fi->first << ')' << endl ;
        }
        sentFederateMessages += nb ;
    }

    s << endl << "List of RTI services " << endl
      << "--------------------------------------------------" << endl ;

    int sentRtiMessages = 0 ;
    std::map<NetworkMessage::Type, int>::iterator ri ;
    std::map<NetworkMessage::Type, int>::iterator re = stat.rtiServiceSet.end();
    for (ri = stat.rtiServiceSet.begin(); ri != re ; ++ri) {
        int nb = ri->second ;
        if (nb || stat.displayZero()) {
            s.width(8);
            s << nb << ' ' << Statistics::rtiMessageName[ri->first]
              << " (" << ri->first << ')' << endl ;
        }
        sentRtiMessages += nb ;
    }

    s << endl
      << " Number of Federate messages : " << sentFederateMessages << endl
      << " Number of RTIG messages : " << sentRtiMessages << endl ;

    return s ;
}

}} // namespace certi::rtia

// $Id: Statistics.cc,v 3.4 2003/07/03 16:21:58 breholee Exp $
