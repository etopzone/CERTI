// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: PrettyDebEx.cc,v 4.7 2009/10/21 19:51:13 erk Exp $
// ----------------------------------------------------------------------------


#include "PrettyDebug.hh"
#include "Message.hh"
#include "NetworkMessage.hh"

#include <stdlib.h>
#include <stdarg.h>
#include <iostream>
#include <cstring>
#ifdef _WIN32
	#include <time.h>
#else
	#include <sys/types.h>
	#include <sys/time.h>
	#include <unistd.h>
	#include <string>
#endif

using std::cout ;
using std::cerr ;
using std::endl ;

#ifndef NDEBUG

//***************************************************************************************
//type: 'M'	Message
//type: 'N'	NetWork Message
void PrettyDebug::Mes(pdDebugLevel Level, const char type, const short testMess, const char *context)
{
    std::ostream* stream = getStreamPrintHeader(Level);
    if (!stream)
        return;

char theMessage[100];

strcpy(theMessage,context);

if (type == 'M')
	{///Message.hh

	switch (testMess)
		{
		case certi::Message::CLOSE_CONNEXION : strcat(theMessage,"CLOSE_CONNEXION"); break;
		case certi::Message::CREATE_FEDERATION_EXECUTION :							strcat(theMessage,"CREATE_FEDERATION_EXECUTION"); break;
		case certi::Message::DESTROY_FEDERATION_EXECUTION :						strcat(theMessage,"DESTROY_FEDERATION_EXECUTION"); break;
		case certi::Message::JOIN_FEDERATION_EXECUTION :							strcat(theMessage,"JOIN_FEDERATION_EXECUTION"); break;
		case certi::Message::RESIGN_FEDERATION_EXECUTION :							strcat(theMessage,"RESIGN_FEDERATION_EXECUTION"); break;
		case certi::Message::REGISTER_FEDERATION_SYNCHRONIZATION_POINT :		strcat(theMessage,"REGISTER_FEDERATION_SYNCHRONIZATION_POINT"); break;
		case certi::Message::SYNCHRONIZATION_POINT_REGISTRATION_FAILED :		strcat(theMessage,"SYNCHRONIZATION_POINT_REGISTRATION_FAILED"); break;
		case certi::Message::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED :	strcat(theMessage,"SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED"); break;
		case certi::Message::ANNOUNCE_SYNCHRONIZATION_POINT :						strcat(theMessage,"ANNOUNCE_SYNCHRONIZATION_POINT"); break;
		case certi::Message::SYNCHRONIZATION_POINT_ACHIEVED :						strcat(theMessage,"SYNCHRONIZATION_POINT_ACHIEVED"); break;
		case certi::Message::FEDERATION_SYNCHRONIZED :								strcat(theMessage,"FEDERATION_SYNCHRONIZED"); break;
		case certi::Message::REQUEST_FEDERATION_SAVE :								strcat(theMessage,"REQUEST_FEDERATION_SAVE"); break;
		case certi::Message::INITIATE_FEDERATE_SAVE :								strcat(theMessage,"INITIATE_FEDERATE_SAVE"); break;
		case certi::Message::FEDERATE_SAVE_BEGUN :									strcat(theMessage,"FEDERATE_SAVE_BEGUN"); break;
		case certi::Message::FEDERATE_SAVE_COMPLETE :								strcat(theMessage,"FEDERATE_SAVE_COMPLETE"); break;
		case certi::Message::FEDERATE_SAVE_NOT_COMPLETE :							strcat(theMessage,"FEDERATE_SAVE_COMPLETE"); break;
		case certi::Message::FEDERATION_SAVED :										strcat(theMessage,"FEDERATION_SAVED"); break;
		case certi::Message::FEDERATION_NOT_SAVED :									strcat(theMessage,"FEDERATION_NOT_SAVED"); break;
		case certi::Message::REQUEST_FEDERATION_RESTORE :							strcat(theMessage,"REQUEST_FEDERATION_RESTORE"); break;
		case certi::Message::REQUEST_FEDERATION_RESTORE_FAILED :					strcat(theMessage,"REQUEST_FEDERATION_RESTORE_FAILED"); break;
		case certi::Message::REQUEST_FEDERATION_RESTORE_SUCCEEDED :				strcat(theMessage,"REQUEST_FEDERATION_RESTORE_SUCCEEDED"); break;
		case certi::Message::INITIATE_FEDERATE_RESTORE :							strcat(theMessage,"INITIATE_FEDERATE_RESTORE"); break;
		case certi::Message::FEDERATE_RESTORE_COMPLETE :							strcat(theMessage,"FEDERATE_RESTORE_COMPLETE"); break;
		case certi::Message::FEDERATE_RESTORE_NOT_COMPLETE :						strcat(theMessage,"FEDERATE_RESTORE_NOT_COMPLETE"); break;
		case certi::Message::FEDERATION_RESTORED :									strcat(theMessage,"FEDERATION_RESTORED"); break;
		case certi::Message::FEDERATION_NOT_RESTORED :								strcat(theMessage,"FEDERATION_NOT_RESTORED"); break;
		case certi::Message::FEDERATION_RESTORE_BEGUN :								strcat(theMessage,"FEDERATION_RESTORE_BEGUN"); break;

		case certi::Message::PUBLISH_OBJECT_CLASS :									strcat(theMessage,"PUBLISH_OBJECT_CLASS"); break;
		case certi::Message::UNPUBLISH_OBJECT_CLASS :								strcat(theMessage,"UNPUBLISH_OBJECT_CLASS"); break;
		case certi::Message::PUBLISH_INTERACTION_CLASS :							strcat(theMessage,"PUBLISH_INTERACTION_CLASS"); break;
		case certi::Message::UNPUBLISH_INTERACTION_CLASS :							strcat(theMessage,"UNPUBLISH_INTERACTION_CLASS"); break;
		case certi::Message::SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES :					strcat(theMessage,"SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES"); break;
		case certi::Message::UNSUBSCRIBE_OBJECT_CLASS :								strcat(theMessage,"UNSUBSCRIBE_OBJECT_CLASS"); break;
		case certi::Message::SUBSCRIBE_INTERACTION_CLASS :							strcat(theMessage,"SUBSCRIBE_INTERACTION_CLASS"); break;
		case certi::Message::UNSUBSCRIBE_INTERACTION_CLASS :						strcat(theMessage,"UNSUBSCRIBE_INTERACTION_CLASS"); break;
		case certi::Message::START_REGISTRATION_FOR_OBJECT_CLASS :				strcat(theMessage,"START_REGISTRATION_FOR_OBJECT_CLASS"); break;
		case certi::Message::STOP_REGISTRATION_FOR_OBJECT_CLASS :				strcat(theMessage,"STOP_REGISTRATION_FOR_OBJECT_CLASS"); break;
		case certi::Message::TURN_INTERACTIONS_ON :									strcat(theMessage,"TURN_INTERACTIONS_ON"); break;
		case certi::Message::TURN_INTERACTIONS_OFF :									strcat(theMessage,"TURN_INTERACTIONS_OFF"); break;

		case certi::Message::REGISTER_OBJECT_INSTANCE :								strcat(theMessage,"REGISTER_OBJECT_INSTANCE"); break;
		case certi::Message::UPDATE_ATTRIBUTE_VALUES :								strcat(theMessage,"UPDATE_ATTRIBUTE_VALUES"); break;
		case certi::Message::DISCOVER_OBJECT_INSTANCE :								strcat(theMessage,"DISCOVER_OBJECT_INSTANCE"); break;
		case certi::Message::REFLECT_ATTRIBUTE_VALUES :								strcat(theMessage,"REFLECT_ATTRIBUTE_VALUES"); break;
		case certi::Message::SEND_INTERACTION :										strcat(theMessage,"SEND_INTERACTION"); break;
		case certi::Message::RECEIVE_INTERACTION :									strcat(theMessage,"RECEIVE_INTERACTION"); break;
		case certi::Message::DELETE_OBJECT_INSTANCE :								strcat(theMessage,"DELETE_OBJECT_INSTANCE"); break;
		case certi::Message::LOCAL_DELETE_OBJECT_INSTANCE :						strcat(theMessage,"LOCAL_DELETE_OBJECT_INSTANCE"); break;
		case certi::Message::REMOVE_OBJECT_INSTANCE :								strcat(theMessage,"REMOVE_OBJECT_INSTANCE"); break;
		case certi::Message::CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE :				strcat(theMessage,"CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE"); break;
		case certi::Message::CHANGE_INTERACTION_TRANSPORTATION_TYPE :			strcat(theMessage,"CHANGE_INTERACTION_TRANSPORTATION_TYPE"); break;
		case certi::Message::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE :			strcat(theMessage,"REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE"); break;
		case certi::Message::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE :				strcat(theMessage,"REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE"); break;
		case certi::Message::PROVIDE_ATTRIBUTE_VALUE_UPDATE :						strcat(theMessage,"PROVIDE_ATTRIBUTE_VALUE_UPDATE"); break;
		case certi::Message::ATTRIBUTES_IN_SCOPE :									strcat(theMessage,"ATTRIBUTES_IN_SCOPE"); break;
		case certi::Message::ATTRIBUTES_OUT_OF_SCOPE :								strcat(theMessage,"ATTRIBUTES_OUT_OF_SCOPE"); break;
		case certi::Message::TURN_UPDATES_ON_FOR_OBJECT_INSTANCE :				strcat(theMessage,"TURN_UPDATES_ON_FOR_OBJECT_INSTANCE"); break;
		case certi::Message::TURN_UPDATES_OFF_FOR_OBJECT_INSTANCE :				strcat(theMessage,"TURN_UPDATES_OFF_FOR_OBJECT_INSTANCE"); break;

		case certi::Message::REQUEST_ATTRIBUTE_OWNERSHIP_DIVESTITURE :			strcat(theMessage,"REQUEST_ATTRIBUTE_OWNERSHIP_DIVESTITURE"); break;
		case certi::Message::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION :			strcat(theMessage,"REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION"); break;
		case certi::Message::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE :		strcat(theMessage,"NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE"); break;
		case certi::Message::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION :	strcat(theMessage,"ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION"); break;
		case certi::Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION :	strcat(theMessage,"ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION"); break;
		case certi::Message::REQUEST_ATTRIBUTE_OWNERSHIP_ACQUISITION :			strcat(theMessage,"REQUEST_ATTRIBUTE_OWNERSHIP_ACQUISITION"); break;
		case certi::Message::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE :				strcat(theMessage,"REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE"); break;
		case certi::Message::QUERY_ATTRIBUTE_OWNERSHIP :							strcat(theMessage,"QUERY_ATTRIBUTE_OWNERSHIP"); break;
		case certi::Message::INFORM_ATTRIBUTE_OWNERSHIP :							strcat(theMessage,"INFORM_ATTRIBUTE_OWNERSHIP"); break;
		case certi::Message::IS_ATTRIBUTE_OWNED_BY_FEDERATE : 					strcat(theMessage,"IS_ATTRIBUTE_OWNED_BY_FEDERATE"); break;
		case certi::Message::ATTRIBUTE_IS_NOT_OWNED : 								strcat(theMessage,"ATTRIBUTE_IS_NOT_OWNED"); break;
		case certi::Message::ATTRIBUTE_OWNED_BY_RTI : 								strcat(theMessage,"ATTRIBUTE_OWNED_BY_RTI"); break;
		case certi::Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE :	strcat(theMessage,"ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE"); break;
		case certi::Message::ATTRIBUTE_OWNERSHIP_UNAVAILABLE : 					strcat(theMessage,"ATTRIBUTE_OWNERSHIP_UNAVAILABLE"); break;
		case certi::Message::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE :	strcat(theMessage,"UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE"); break;
		case certi::Message::ATTRIBUTE_OWNERSHIP_ACQUISITION : 					strcat(theMessage,"ATTRIBUTE_OWNERSHIP_ACQUISITION"); break;
		case certi::Message::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE : strcat(theMessage,"CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE"); break;
		case certi::Message::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE :				strcat(theMessage,"ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE"); break;
		case certi::Message::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION :			strcat(theMessage,"CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION"); break;
		case certi::Message::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION : strcat(theMessage,"CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION"); break;

		case certi::Message::CHANGE_ATTRIBUTE_ORDER_TYPE :							strcat(theMessage,"CHANGE_ATTRIBUTE_ORDER_TYPE"); break;
		case certi::Message::CHANGE_INTERACTION_ORDER_TYPE :						strcat(theMessage,"CHANGE_INTERACTION_ORDER_TYPE"); break;
		case certi::Message::ENABLE_TIME_REGULATION :								strcat(theMessage,"ENABLE_TIME_REGULATION"); break;
		case certi::Message::DISABLE_TIME_REGULATION :								strcat(theMessage,"DISABLE_TIME_REGULATION"); break;
		case certi::Message::ENABLE_TIME_CONSTRAINED :								strcat(theMessage,"ENABLE_TIME_CONSTRAINED"); break;
		case certi::Message::DISABLE_TIME_CONSTRAINED :								strcat(theMessage,"DISABLE_TIME_CONSTRAINED"); break;
		case certi::Message::QUERY_LBTS :												strcat(theMessage,"QUERY_LBTS"); break;
		case certi::Message::QUERY_FEDERATE_TIME :									strcat(theMessage,"QUERY_FEDERATE_TIME"); break;
		case certi::Message::QUERY_MIN_NEXT_EVENT_TIME :							strcat(theMessage,"QUERY_MIN_NEXT_EVENT_TIME"); break;
		case certi::Message::MODIFY_LOOKAHEAD :										strcat(theMessage,"MODIFY_LOOKAHEAD"); break;
		case certi::Message::QUERY_LOOKAHEAD :											strcat(theMessage,"QUERY_LOOKAHEAD"); break;
		case certi::Message::RETRACT :													strcat(theMessage,"RETRACT"); break;
		case certi::Message::REQUEST_RETRACTION :										strcat(theMessage,"REQUEST_RETRACTION"); break;
		case certi::Message::TIME_ADVANCE_REQUEST :									strcat(theMessage,"TIME_ADVANCE_REQUEST"); break;
		case certi::Message::TIME_ADVANCE_REQUEST_AVAILABLE :						strcat(theMessage,"TIME_ADVANCE_REQUEST_AVAILABLE"); break;
		case certi::Message::NEXT_EVENT_REQUEST :										strcat(theMessage,"NEXT_EVENT_REQUEST"); break;
		case certi::Message::NEXT_EVENT_REQUEST_AVAILABLE :						strcat(theMessage,"NEXT_EVENT_REQUEST_AVAILABLE"); break;
		case certi::Message::FLUSH_QUEUE_REQUEST :									strcat(theMessage,"FLUSH_QUEUE_REQUEST"); break;
		case certi::Message::TIME_ADVANCE_GRANT :										strcat(theMessage,"TIME_ADVANCE_GRANT"); break;
		case certi::Message::ENABLE_ASYNCHRONOUS_DELIVERY :						strcat(theMessage,"ENABLE_ASYNCHRONOUS_DELIVERY"); break;
		case certi::Message::DISABLE_ASYNCHRONOUS_DELIVERY :						strcat(theMessage,"DISABLE_ASYNCHRONOUS_DELIVERY"); break;
		case certi::Message::TIME_REGULATION_ENABLED :								strcat(theMessage,"TIME_REGULATION_ENABLED"); break;
		case certi::Message::TIME_CONSTRAINED_ENABLED :								strcat(theMessage,"TIME_CONSTRAINED_ENABLED"); break;

		case certi::Message::DDM_CREATE_REGION :										strcat(theMessage,"DDM_CREATE_REGION"); break;
		case certi::Message::DDM_MODIFY_REGION :										strcat(theMessage,"DDM_MODIFY_REGION"); break;
		case certi::Message::DDM_DELETE_REGION :										strcat(theMessage,"DDM_DELETE_REGION"); break;
		case certi::Message::DDM_REGISTER_OBJECT :									strcat(theMessage,"DDM_REGISTER_OBJECT"); break;
		case certi::Message::DDM_ASSOCIATE_REGION :									strcat(theMessage,"DDM_ASSOCIATE_REGION"); break;
		case certi::Message::DDM_UNASSOCIATE_REGION :								strcat(theMessage,"DDM_UNASSOCIATE_REGION"); break;
		case certi::Message::DDM_SUBSCRIBE_ATTRIBUTES :								strcat(theMessage,"DDM_SUBSCRIBE_ATTRIBUTES"); break;
		case certi::Message::DDM_UNSUBSCRIBE_ATTRIBUTES :							strcat(theMessage,"DDM_UNSUBSCRIBE_ATTRIBUTES"); break;
		case certi::Message::DDM_SUBSCRIBE_INTERACTION :							strcat(theMessage,"DDM_SUBSCRIBE_INTERACTION"); break;
		case certi::Message::DDM_UNSUBSCRIBE_INTERACTION :							strcat(theMessage,"DDM_UNSUBSCRIBE_INTERACTION"); break;
		case certi::Message::DDM_REQUEST_UPDATE :										strcat(theMessage,"DDM_REQUEST_UPDATE"); break;

		case certi::Message::GET_OBJECT_CLASS_HANDLE :								strcat(theMessage,"GET_OBJECT_CLASS_HANDLE"); break;
		case certi::Message::GET_OBJECT_CLASS_NAME :									strcat(theMessage,"GET_OBJECT_CLASS_NAME"); break;
		case certi::Message::GET_ATTRIBUTE_HANDLE :									strcat(theMessage,"GET_ATTRIBUTE_HANDLE"); break;
		case certi::Message::GET_ATTRIBUTE_NAME :										strcat(theMessage,"GET_ATTRIBUTE_NAME"); break;
		case certi::Message::GET_INTERACTION_CLASS_HANDLE :						strcat(theMessage,"GET_INTERACTION_CLASS_HANDLE"); break;
		case certi::Message::GET_INTERACTION_CLASS_NAME :							strcat(theMessage,"GET_INTERACTION_CLASS_NAME"); break;
		case certi::Message::GET_PARAMETER_HANDLE :									strcat(theMessage,"GET_PARAMETER_HANDLE"); break;
		case certi::Message::GET_PARAMETER_NAME :										strcat(theMessage,"GET_PARAMETER_NAME"); break;
		case certi::Message::GET_OBJECT_INSTANCE_HANDLE :							strcat(theMessage,"GET_OBJECT_INSTANCE_HANDLE"); break;
		case certi::Message::GET_OBJECT_INSTANCE_NAME :								strcat(theMessage,"GET_OBJECT_INSTANCE_NAME"); break;
		case certi::Message::GET_SPACE_HANDLE :										strcat(theMessage,"GET_SPACE_HANDLE"); break;
		case certi::Message::GET_SPACE_NAME :											strcat(theMessage,"GET_SPACE_NAME"); break;
		case certi::Message::GET_DIMENSION_HANDLE :									strcat(theMessage,"GET_DIMENSION_HANDLE"); break;
		case certi::Message::GET_DIMENSION_NAME :										strcat(theMessage,"GET_DIMENSION_NAME"); break;
		case certi::Message::GET_ATTRIBUTE_SPACE_HANDLE :							strcat(theMessage,"GET_ATTRIBUTE_SPACE_HANDLE"); break;
		case certi::Message::GET_OBJECT_CLASS :										strcat(theMessage,"GET_OBJECT_CLASS"); break;
		case certi::Message::GET_INTERACTION_SPACE_HANDLE :						strcat(theMessage,"GET_INTERACTION_SPACE_HANDLE"); break;
		case certi::Message::GET_TRANSPORTATION_HANDLE :							strcat(theMessage,"GET_INTERACTION_SPACE_HANDLE"); break;
		case certi::Message::GET_TRANSPORTATION_NAME :								strcat(theMessage,"GET_TRANSPORTATION_NAME"); break;
		case certi::Message::GET_ORDERING_HANDLE :									strcat(theMessage,"GET_ORDERING_HANDLE"); break;
		case certi::Message::GET_ORDERING_NAME :										strcat(theMessage,"GET_ORDERING_NAME"); break;
		case certi::Message::ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH :
			strcat(theMessage,"ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH"); break;
		case certi::Message::DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH :
			strcat(theMessage,"DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH"); break;
		case certi::Message::ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH :
			strcat(theMessage,"ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH"); break;
		case certi::Message::DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH :
			strcat(theMessage,"DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH"); break;
		case certi::Message::ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH :
			strcat(theMessage,"ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH"); break;
		case certi::Message::DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH :
			strcat(theMessage,"DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH"); break;
		case certi::Message::ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH :
			strcat(theMessage,"ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH"); break;
		case certi::Message::DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
			strcat(theMessage,"DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH"); break;
		case certi::Message::TICK_REQUEST :												strcat(theMessage,"TICK_REQUEST"); break;
		default:
			sprintf(theMessage,"LOCAL UNKNOWED MESSAGE %hd",testMess);
		}
	}
else if (type == 'N')
	{
	switch (testMess)
		{//NetworkMessage.h
		case certi::NetworkMessage::CLOSE_CONNEXION :										strcat(theMessage,"CLOSE_CONNEXION"); break;
		case certi::NetworkMessage::MESSAGE_NULL :											strcat(theMessage,"MESSAGE_NULL"); break;
		case certi::NetworkMessage::CREATE_FEDERATION_EXECUTION :						strcat(theMessage,"CREATE_FEDERATION_EXECUTION"); break;
		case certi::NetworkMessage::DESTROY_FEDERATION_EXECUTION : 						strcat(theMessage,"DESTROY_FEDERATION_EXECUTION"); break;
		case certi::NetworkMessage::JOIN_FEDERATION_EXECUTION : 							strcat(theMessage,"JOIN_FEDERATION_EXECUTION"); break;
		case certi::NetworkMessage::RESIGN_FEDERATION_EXECUTION : 						strcat(theMessage,"RESIGN_FEDERATION_EXECUTION"); break;
		case certi::NetworkMessage::SET_TIME_REGULATING : 									strcat(theMessage,"SET_TIME_REGULATING"); break;
		case certi::NetworkMessage::TIME_REGULATION_ENABLED : 									strcat(theMessage,"TIME_REGULATION_ENABLED"); break;
		case certi::NetworkMessage::SET_TIME_CONSTRAINED : 			 					strcat(theMessage,"SET_TIME_CONSTRAINED"); break;
		case certi::NetworkMessage::TIME_CONSTRAINED_ENABLED : 									strcat(theMessage,"TIME_CONSTRAINED_ENABLED"); break;
		case certi::NetworkMessage::REGISTER_FEDERATION_SYNCHRONIZATION_POINT : 	strcat(theMessage,"REGISTER_FEDERATION_SYNCHRONIZATION_POINT"); break;
		case certi::NetworkMessage::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED : strcat(theMessage,"SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED"); break;
		case certi::NetworkMessage::ANNOUNCE_SYNCHRONIZATION_POINT : 			 		strcat(theMessage,"ANNOUNCE_SYNCHRONIZATION_POINT"); break;
		case certi::NetworkMessage::SYNCHRONIZATION_POINT_ACHIEVED : 			 		strcat(theMessage,"SYNCHRONIZATION_POINT_ACHIEVED"); break;
		case certi::NetworkMessage::FEDERATION_SYNCHRONIZED : 			 				strcat(theMessage,"FEDERATION_SYNCHRONIZED"); break;
		case certi::NetworkMessage::REQUEST_FEDERATION_SAVE : 			 				strcat(theMessage,"REQUEST_FEDERATION_SAVE"); break;
		case certi::NetworkMessage::FEDERATE_SAVE_BEGUN : 			 						strcat(theMessage,"FEDERATE_SAVE_BEGUN"); break;
		case certi::NetworkMessage::FEDERATE_SAVE_COMPLETE : 			 					strcat(theMessage,"FEDERATE_SAVE_COMPLETE"); break;
		case certi::NetworkMessage::FEDERATE_SAVE_NOT_COMPLETE : 						strcat(theMessage,"FEDERATE_SAVE_NOT_COMPLETE"); break;
		case certi::NetworkMessage::INITIATE_FEDERATE_SAVE : 								strcat(theMessage,"INITIATE_FEDERATE_SAVE"); break;
		case certi::NetworkMessage::FEDERATION_SAVED : 			 							strcat(theMessage,"FEDERATION_SAVED"); break;
		case certi::NetworkMessage::FEDERATION_NOT_SAVED : 			 					strcat(theMessage,"FEDERATION_NOT_SAVED"); break;
		case certi::NetworkMessage::REQUEST_FEDERATION_RESTORE : 						strcat(theMessage,"REQUEST_FEDERATION_RESTORE"); break;
		case certi::NetworkMessage::FEDERATE_RESTORE_COMPLETE : 			 				strcat(theMessage,"FEDERATE_RESTORE_COMPLETE"); break;
		case certi::NetworkMessage::FEDERATE_RESTORE_NOT_COMPLETE : 			 		strcat(theMessage,"FEDERATE_RESTORE_NOT_COMPLETE"); break;
		case certi::NetworkMessage::REQUEST_FEDERATION_RESTORE_SUCCEEDED : 			strcat(theMessage,"REQUEST_FEDERATION_RESTORE_SUCCEEDED"); break;
		case certi::NetworkMessage::REQUEST_FEDERATION_RESTORE_FAILED : 			 	strcat(theMessage,"REQUEST_FEDERATION_RESTORE_FAILED"); break;
		case certi::NetworkMessage::FEDERATION_RESTORE_BEGUN : 			 				strcat(theMessage,"FEDERATION_RESTORE_BEGUN"); break;
		case certi::NetworkMessage::INITIATE_FEDERATE_RESTORE : 							strcat(theMessage,"INITIATE_FEDERATE_RESTORE"); break;
		case certi::NetworkMessage::FEDERATION_RESTORED : 									strcat(theMessage,"FEDERATION_RESTORED"); break;
		case certi::NetworkMessage::FEDERATION_NOT_RESTORED : 							strcat(theMessage,"FEDERATION_NOT_RESTORED"); break;
		case certi::NetworkMessage::PUBLISH_OBJECT_CLASS : 			 					strcat(theMessage,"PUBLISH_OBJECT_CLASS"); break;
		case certi::NetworkMessage::UNPUBLISH_OBJECT_CLASS : 			 					strcat(theMessage,"UNPUBLISH_OBJECT_CLASS"); break;
		case certi::NetworkMessage::PUBLISH_INTERACTION_CLASS : 			 				strcat(theMessage,"PUBLISH_INTERACTION_CLASS"); break;
		case certi::NetworkMessage::UNPUBLISH_INTERACTION_CLASS : 						strcat(theMessage,"UNPUBLISH_INTERACTION_CLASS"); break;
		case certi::NetworkMessage::SUBSCRIBE_OBJECT_CLASS : 								strcat(theMessage,"SUBSCRIBE_OBJECT_CLASS"); break;
		case certi::NetworkMessage::UNSUBSCRIBE_OBJECT_CLASS : 							strcat(theMessage,"UNSUBSCRIBE_OBJECT_CLASS"); break;
		case certi::NetworkMessage::SUBSCRIBE_INTERACTION_CLASS : 						strcat(theMessage,"SUBSCRIBE_INTERACTION_CLASS"); break;
		case certi::NetworkMessage::UNSUBSCRIBE_INTERACTION_CLASS : 					strcat(theMessage,"UNSUBSCRIBE_INTERACTION_CLASS"); break;
		case certi::NetworkMessage::TURN_INTERACTIONS_ON : 								strcat(theMessage,"TURN_INTERACTIONS_ON"); break;
		case certi::NetworkMessage::TURN_INTERACTIONS_OFF : 								strcat(theMessage,"TURN_INTERACTIONS_OFF"); break;
		case certi::NetworkMessage::REGISTER_OBJECT : 										strcat(theMessage,"REGISTER_OBJECT"); break;
		case certi::NetworkMessage::DISCOVER_OBJECT : 										strcat(theMessage,"DISCOVER_OBJECT"); break;
		case certi::NetworkMessage::UPDATE_ATTRIBUTE_VALUES : 							strcat(theMessage,"UPDATE_ATTRIBUTE_VALUES"); break;
		case certi::NetworkMessage::REFLECT_ATTRIBUTE_VALUES : 							strcat(theMessage,"REFLECT_ATTRIBUTE_VALUES"); break;
		case certi::NetworkMessage::SEND_INTERACTION : 										strcat(theMessage,"SEND_INTERACTION"); break;
		case certi::NetworkMessage::RECEIVE_INTERACTION : 									strcat(theMessage,"RECEIVE_INTERACTION"); break;
		case certi::NetworkMessage::DELETE_OBJECT : 											strcat(theMessage,"DELETE_OBJECT"); break;
		case certi::NetworkMessage::REMOVE_OBJECT : 											strcat(theMessage,"REMOVE_OBJECT"); break;
		case certi::NetworkMessage::CHANGE_ATTRIBUTE_TRANSPORT_TYPE : 					strcat(theMessage,"CHANGE_ATTRIBUTE_TRANSPORT_TYPE"); break;
		case certi::NetworkMessage::CHANGE_ATTRIBUTE_ORDER_TYPE : 						strcat(theMessage,"CHANGE_ATTRIBUTE_ORDER_TYPE"); break;
		case certi::NetworkMessage::CHANGE_INTERACTION_TRANSPORT_TYPE : 				strcat(theMessage,"CHANGE_INTERACTION_TRANSPORT_TYPE"); break;
		case certi::NetworkMessage::CHANGE_INTERACTION_ORDER_TYPE : 					strcat(theMessage,"CHANGE_INTERACTION_ORDER_TYPE"); break;
		case certi::NetworkMessage::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE : 			strcat(theMessage,"REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE"); break;
		case certi::NetworkMessage::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE : 			strcat(theMessage,"REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE"); break;
		case certi::NetworkMessage::IS_ATTRIBUTE_OWNED_BY_FEDERATE : 					strcat(theMessage,"IS_ATTRIBUTE_OWNED_BY_FEDERATE"); break;
		case certi::NetworkMessage::QUERY_ATTRIBUTE_OWNERSHIP : 							strcat(theMessage,"QUERY_ATTRIBUTE_OWNERSHIP"); break;
		case certi::NetworkMessage::ATTRIBUTE_IS_NOT_OWNED : 								strcat(theMessage,"ATTRIBUTE_IS_NOT_OWNED"); break;
		case certi::NetworkMessage::INFORM_ATTRIBUTE_OWNERSHIP : 						strcat(theMessage,"INFORM_ATTRIBUTE_OWNERSHIP"); break;
		case certi::NetworkMessage::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE : 	strcat(theMessage,"NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE"); break;
		case certi::NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION : strcat(theMessage,"ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION"); break;
		case certi::NetworkMessage::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION : strcat(theMessage,"ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION"); break;
		case certi::NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION : 		strcat(theMessage,"REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION"); break;
		case certi::NetworkMessage::ATTRIBUTE_OWNERSHIP_UNAVAILABLE : 					strcat(theMessage,"ATTRIBUTE_OWNERSHIP_UNAVAILABLE"); break;
		case certi::NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE : strcat(theMessage,"ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE"); break;
		case certi::NetworkMessage::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE :strcat(theMessage,"UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE"); break;
		case certi::NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION : 					strcat(theMessage,"ATTRIBUTE_OWNERSHIP_ACQUISITION"); break;
		case certi::NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE : 			strcat(theMessage,"REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE"); break;
		case certi::NetworkMessage::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE :strcat(theMessage,"CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE"); break;
		case certi::NetworkMessage::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE : 			strcat(theMessage,"ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE"); break;
		case certi::NetworkMessage::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION : 		strcat(theMessage,"CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION"); break;
		case certi::NetworkMessage::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION :	strcat(theMessage,"CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION"); break;
		case certi::NetworkMessage::DDM_CREATE_REGION : 									strcat(theMessage,"DDM_CREATE_REGION"); break;
		case certi::NetworkMessage::DDM_MODIFY_REGION :										strcat(theMessage,"DDM_MODIFY_REGION"); break;
		case certi::NetworkMessage::DDM_DELETE_REGION :										strcat(theMessage,"DDM_DELETE_REGION"); break;
		case certi::NetworkMessage::DDM_ASSOCIATE_REGION : 								strcat(theMessage,"DDM_ASSOCIATE_REGION"); break;
		case certi::NetworkMessage::DDM_REGISTER_OBJECT : 									strcat(theMessage,"DDM_REGISTER_OBJECT"); break;
		case certi::NetworkMessage::DDM_UNASSOCIATE_REGION : 								strcat(theMessage,"DDM_UNASSOCIATE_REGION"); break;
		case certi::NetworkMessage::DDM_SUBSCRIBE_ATTRIBUTES : 							strcat(theMessage,"DDM_SUBSCRIBE_ATTRIBUTES"); break;
		case certi::NetworkMessage::DDM_UNSUBSCRIBE_ATTRIBUTES : 						strcat(theMessage,"DDM_UNSUBSCRIBE_ATTRIBUTES"); break;
		case certi::NetworkMessage::DDM_SUBSCRIBE_INTERACTION : 							strcat(theMessage,"DDM_SUBSCRIBE_INTERACTION"); break;
		case certi::NetworkMessage::DDM_UNSUBSCRIBE_INTERACTION : 						strcat(theMessage,"DDM_UNSUBSCRIBE_INTERACTION"); break;
		case certi::NetworkMessage::GET_FED_FILE : 											strcat(theMessage,"GET_FED_FILE"); break;
		default:																							sprintf(theMessage,"NETWORK UNKNOWED MESSAGE %hd",testMess);
		}
	}
strcat(theMessage, "\n"); // Add trailing \n
 *stream << theMessage;
}
#endif // NDEBUG

// $Id: PrettyDebEx.cc,v 4.7 2009/10/21 19:51:13 erk Exp $
