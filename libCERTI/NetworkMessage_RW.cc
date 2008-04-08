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
// $Id: NetworkMessage_RW.cc,v 3.45.2.2 2008/04/08 14:05:26 erk Exp $
// ----------------------------------------------------------------------------


#include "NetworkMessage.hh"
#include "PrettyDebug.hh"

using std::vector;
using std::endl;
using std::cout;

namespace certi {

static PrettyDebug D("RTIG_MSG", "(NetWorkMS) - ");
static PrettyDebug G("GENDOC",__FILE__ );

// ----------------------------------------------------------------------------
// readBody
void
NetworkMessage::readBody(Socket *socket)
{	
	unsigned short i;
	G.Out(pdGendoc,"enter NetworkMessage::readBody");
	if (Header.bodySize == 0) {
		throw RTIinternalError("ReadBody should not have been called.");
	}
	
	// 1. Read Body from socket.
    // NOTHING TO DO all buffer read in readHeader
	// FIXME need to get rid of Header/Body distinction.

	// 3. Read informations from Message Body according to message type.
	//D.Mes(pdMessage, 'N',Header.type);	

	if (Header.exception != e_NO_EXCEPTION) {
		exceptionReason = msgBuf.read_string();
	}
	else {
		D.Mes(pdDebug,'N',Header.type,"readBody::");
		switch(Header.type) {
		// line number, FEDid, Value Array size and Value array (line contents)
		case GET_FED_FILE:
			number = msgBuf.read_uint16();
			readFEDid();
			if ( number >= 1 ) // open (0) and close (0) no more information
			{
				ValueArray[0].length = msgBuf.read_int32();
				msgBuf.read_bytes(ValueArray[0].value, ValueArray[0].length) ;
			}
			break ;

		case UPDATE_ATTRIBUTE_VALUES:
			object = msgBuf.read_int32();
			readLabel();
			boolean = msgBuf.read_uint32();   // true means with time
			msgBuf.read_bytes((char *) handleArray, handleArraySize * sizeof(AttributeHandle));
			for (i = 0 ; i < handleArraySize ; i ++) {
				ValueArray[i].length = msgBuf.read_int32();
				msgBuf.read_bytes(ValueArray[i].value, ValueArray[i].length) ;
			}
			break ;

		case REFLECT_ATTRIBUTE_VALUES:
			object = msgBuf.read_int32();
			readLabel();
			boolean = msgBuf.read_int32();    // true means with time
			msgBuf.read_bytes((char *) handleArray, handleArraySize * sizeof(AttributeHandle));
			for (i = 0 ; i < handleArraySize ; i ++) {
				ValueArray[i].length = msgBuf.read_int32();
				msgBuf.read_bytes(ValueArray[i].value, ValueArray[i].length) ;
			}
			break ;
			
			      case PROVIDE_ATTRIBUTE_VALUE_UPDATE:
	object = msgBuf.read_int32();
	for (i = 0 ; i < handleArraySize ; i ++) {
	        handleArray[i] = msgBuf.read_int16();
	}
	break ;

			// -- O_I Variable Part With Date(Body Not Empty) --
		case SEND_INTERACTION:
		case RECEIVE_INTERACTION:
			readLabel();
			boolean = msgBuf.read_int32();   // true means with time
			msgBuf.read_bytes((char *) handleArray,
					handleArraySize * sizeof(AttributeHandle));
			for (i = 0 ; i < handleArraySize ; i ++) {
				ValueArray[i].length = msgBuf.read_int32() ;
				msgBuf.read_bytes(ValueArray[i].value, ValueArray[i].length) ;
			}
			region = msgBuf.read_int32();
			break ;


		case REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
			object = msgBuf.read_int32();			
			for (i = 0 ; i < handleArraySize ; i ++) {
				handleArray[i] = msgBuf.read_int16();
			}
			break ;

		case CREATE_FEDERATION_EXECUTION:
			readFederationName();
			readFEDid();
			break ;

		case DESTROY_FEDERATION_EXECUTION:
			readFederationName();
			break ;			

		case REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
			readLabel();
			readTag();
			boolean = msgBuf.read_int32();
			// boolean true means there is an handleArray
			if ( boolean)
			{
				handleArraySize = msgBuf.read_int16();
				for (i = 0 ; i < handleArraySize ; i ++)
					handleArray[i] = msgBuf.read_int16();
			}
			break ;

		case ANNOUNCE_SYNCHRONIZATION_POINT:
			readLabel();
			readTag();
			break ;

		case SYNCHRONIZATION_POINT_ACHIEVED:
		case SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
		case FEDERATION_SYNCHRONIZED:
		case REQUEST_FEDERATION_RESTORE:
			readLabel();
			break ;

		case REQUEST_FEDERATION_RESTORE_SUCCEEDED:
			readLabel();
			G.Out(pdGendoc,"NetworkMessage::readBody type R_F_R_S label= %s",label.c_str());
			break ;

		case INITIATE_FEDERATE_RESTORE:
			readLabel();
			break ;

		case INITIATE_FEDERATE_SAVE:
			readLabel();
			// boolean true means with time (in the header)
			boolean = msgBuf.read_int32();
			break ;

		case REQUEST_FEDERATION_SAVE:
			readLabel();
			// boolean true means with time (in the header)
			boolean = msgBuf.read_int32();
			break ;

		case REQUEST_FEDERATION_RESTORE_FAILED:
			readLabel();
			readTag();
			break ;

		case DELETE_OBJECT:
		case REMOVE_OBJECT:
			object = msgBuf.read_int32();
			boolean = msgBuf.read_int32();   // true means with time
			readLabel();
			break ;

			// -- No Variable Part --

		case IS_ATTRIBUTE_OWNED_BY_FEDERATE:
		case INFORM_ATTRIBUTE_OWNERSHIP:
		case ATTRIBUTE_IS_NOT_OWNED:
		case QUERY_ATTRIBUTE_OWNERSHIP:
			object = msgBuf.read_int32();
			handleArray[0] = msgBuf.read_int16();
			readLabel();
			break ;

		case NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
		case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
		case ATTRIBUTE_OWNERSHIP_ACQUISITION:
		case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
			object = msgBuf.read_int32();
			handleArraySize = msgBuf.read_int16();
			for (i = 0 ; i < handleArraySize ; i ++)
				handleArray[i] = msgBuf.read_int16();
			readLabel();
			break ;

		case ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
		case ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
		case ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
		case UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
		case ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
		case CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
		case ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
		case CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
		case CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
			object = msgBuf.read_int32();
			handleArraySize = msgBuf.read_int16();
			for (i = 0 ; i < handleArraySize ; i ++)
				handleArray[i] = msgBuf.read_int16();
			break ;

		case DDM_CREATE_REGION:
			space = msgBuf.read_int32();
			nbExtents = msgBuf.read_int32();
			region = msgBuf.read_int32();
			break ;

			// -- Join Variable Part --

		case JOIN_FEDERATION_EXECUTION:
			readFederationName();
			readFederateName();
			break ;

			// -- O_I Variable Part(Body not empty) --

		case PUBLISH_OBJECT_CLASS:
		case SUBSCRIBE_OBJECT_CLASS:
			for (i = 0 ; i < handleArraySize ; i ++)
				handleArray[i] = msgBuf.read_int16();
			break ;

		case REGISTER_OBJECT:
		case DISCOVER_OBJECT:
			object = msgBuf.read_int32();
			readLabel();
			break ;

		case DDM_MODIFY_REGION:
			readExtents();
			break ;

		case DDM_ASSOCIATE_REGION:
			object = msgBuf.read_int32();
			region = msgBuf.read_int32();
			boolean = msgBuf.read_int32();
			handleArraySize = msgBuf.read_int16();
			for (i = 0 ; i < handleArraySize ; i ++)
				handleArray[i] = msgBuf.read_int16();
			break ;

		case DDM_SUBSCRIBE_ATTRIBUTES:
			objectClass = msgBuf.read_int32();
			region = msgBuf.read_int32();
			boolean = msgBuf.read_int32();
			handleArraySize = msgBuf.read_int16();
			for (i = 0 ; i < handleArraySize ; i ++)
				handleArray[i] = msgBuf.read_int16();
			break ;

		case DDM_UNASSOCIATE_REGION:
			object = msgBuf.read_int32();
			region = msgBuf.read_int32();
			break ;

		case DDM_UNSUBSCRIBE_ATTRIBUTES:	    
			objectClass = msgBuf.read_int32();
			region = msgBuf.read_int32();
			break ;

		case DDM_SUBSCRIBE_INTERACTION:
		case DDM_UNSUBSCRIBE_INTERACTION:
			interactionClass = msgBuf.read_int32();
			region = msgBuf.read_int32();
			boolean = msgBuf.read_int32();
			break ;

		case DDM_REGISTER_OBJECT:
			objectClass = msgBuf.read_int32();
			object = msgBuf.read_int32();
			region = msgBuf.read_int32();
			readTag();
			handleArraySize = msgBuf.read_int16();
			for (i = 0 ; i < handleArraySize ; i ++)
				handleArray[i] = msgBuf.read_int16();
			break ;

		default:
			D.Out(pdExcept, "Unknown type %d in ReadBody.", Header.type);
			throw RTIinternalError("Unknown/Unimplemented type for body.");
		}
	}
	G.Out(pdGendoc,"exit NetworkMessage::readBody");
}

// ----------------------------------------------------------------------------
bool
NetworkMessage::readHeader(Socket *socket)
{
	G.Out(pdGendoc,"enter NetworkMessage::readHeader");
	// 1- Read reserved bytes from socket
	D.Out(pdDebug,"reading %d bytes for header",msgBuf.reservedBytes);
	socket->receive(msgBuf(0), msgBuf.reservedBytes);	
	msgBuf.show(msgBuf(0),5);
	fflush(stdout);
	msgBuf.assumeSizeFromReservedBytes();
	D.Out(pdDebug,"Got a MsgBuf of size %d bytes (including %d reserved)",msgBuf.size(),msgBuf.reservedBytes);
	socket->receive(msgBuf(msgBuf.reservedBytes),msgBuf.size()-msgBuf.reservedBytes);
	
	// 3- Unmarshall Header
	Header.type        = static_cast<certi::NetworkMessage::Type>(msgBuf.read_int32());
	Header.exception   = static_cast<certi::TypeException>(msgBuf.read_int32());
	Header.federate    = msgBuf.read_int32();
	Header.federation  = msgBuf.read_int32();
	Header.bodySize    = msgBuf.read_int32();
	
	D.Mes(pdDebug,'N',Header.type,"readHeader::");
	// 2- (Pseudo) Parse Header(Static Part)
	type       = Header.type       ;
	exception  = Header.exception  ;
	federate   = Header.federate   ;
	federation = Header.federation ;
	
	// If the message carry an exception, the Body will only contain the
	// exception reason.
	if (exception != e_NO_EXCEPTION)
	{
		G.Out(pdGendoc,"exit  Message::readHeader carrying an exception");
		return true ;
	}

	// 2- Parse Header according to its type(Variable Part)
	switch (Header.type) {
	case MESSAGE_NULL:
		Header.VP.time.date = msgBuf.read_double();
		date = Header.VP.time.date ;
		break ;

	case REQUEST_FEDERATION_SAVE:
		Header.VP.O_I.date = msgBuf.read_double();
		date = Header.VP.O_I.date ;
		break ;

	case UPDATE_ATTRIBUTE_VALUES:
	case REFLECT_ATTRIBUTE_VALUES:
		Header.VP.O_I.handle = msgBuf.read_int32();
		objectClass = Header.VP.O_I.handle ;
		Header.VP.O_I.size   = msgBuf.read_int32();
		handleArraySize = Header.VP.O_I.size ;
		Header.VP.O_I.date = msgBuf.read_double();
		date = Header.VP.O_I.date ;
		break ;

	case SEND_INTERACTION:
	case RECEIVE_INTERACTION:
		Header.VP.O_I.handle = msgBuf.read_int32();
		interactionClass = Header.VP.O_I.handle ;
		Header.VP.O_I.size   = msgBuf.read_int32();
		handleArraySize = Header.VP.O_I.size ;
		Header.VP.O_I.date = msgBuf.read_double();
		date = Header.VP.O_I.date ;
		break ;

	case INITIATE_FEDERATE_SAVE:
		Header.VP.O_I.date = msgBuf.read_double();
		date = Header.VP.O_I.date ;
		break ;

	case REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
		Header.VP.O_I.size   = msgBuf.read_int32();
		handleArraySize = Header.VP.O_I.size ;
		break;

	case REQUEST_FEDERATION_RESTORE:
	case INITIATE_FEDERATE_RESTORE:
	case REQUEST_FEDERATION_RESTORE_SUCCEEDED:
	case REQUEST_FEDERATION_RESTORE_FAILED:
	case CREATE_FEDERATION_EXECUTION:
	case DESTROY_FEDERATION_EXECUTION:
	case REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
	case SYNCHRONIZATION_POINT_ACHIEVED:
	case SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
	case FEDERATION_SYNCHRONIZED:
	case ANNOUNCE_SYNCHRONIZATION_POINT:
	case DELETE_OBJECT:
	case REMOVE_OBJECT:
		Header.VP.O_I.date = msgBuf.read_double();
		date = Header.VP.O_I.date;
		break;
	case CLOSE_CONNEXION:
	case RESIGN_FEDERATION_EXECUTION:
	case IS_ATTRIBUTE_OWNED_BY_FEDERATE:
	case INFORM_ATTRIBUTE_OWNERSHIP:
	case ATTRIBUTE_IS_NOT_OWNED:
	case QUERY_ATTRIBUTE_OWNERSHIP:
	case ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
	case ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
	case ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
	case NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
	case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
	case UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
	case ATTRIBUTE_OWNERSHIP_ACQUISITION:
	case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
	case ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
	case CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
	case ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
	case CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
	case CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
	case DDM_CREATE_REGION:
	case FEDERATE_SAVE_BEGUN:
	case FEDERATE_SAVE_COMPLETE:
	case FEDERATE_SAVE_NOT_COMPLETE:
	case FEDERATION_SAVED:
	case FEDERATION_NOT_SAVED:
	case FEDERATE_RESTORE_COMPLETE:
	case FEDERATE_RESTORE_NOT_COMPLETE:
	case FEDERATION_RESTORE_BEGUN:
	case FEDERATION_RESTORED:
	case FEDERATION_NOT_RESTORED:
	case DDM_ASSOCIATE_REGION:
	case DDM_UNASSOCIATE_REGION:
	case DDM_SUBSCRIBE_ATTRIBUTES:
	case DDM_UNSUBSCRIBE_ATTRIBUTES:
	case DDM_SUBSCRIBE_INTERACTION:
	case DDM_UNSUBSCRIBE_INTERACTION:
	case DDM_REGISTER_OBJECT:
	case GET_FED_FILE:
		break ;

	case SET_TIME_REGULATING:
		Header.VP.time.date = msgBuf.read_double();
		date = Header.VP.time.date ;
		Header.VP.time.R_or_C = msgBuf.read_double();
		regulator = Header.VP.time.R_or_C ;		
		break ;

	case SET_TIME_CONSTRAINED:
		Header.VP.time.date = msgBuf.read_double();
		date = Header.VP.time.date ;
		Header.VP.time.R_or_C = msgBuf.read_double();
		constrained = Header.VP.time.R_or_C ;
		break ;

	case CHANGE_ATTRIBUTE_TRANSPORT_TYPE:
	case CHANGE_ATTRIBUTE_ORDER_TYPE:
	case CHANGE_INTERACTION_TRANSPORT_TYPE:
	case CHANGE_INTERACTION_ORDER_TYPE:
		throw RTIinternalError("Read Message not implemented for T/O.");
		break ;

		// -- Join Variable Part(No body) --

	case JOIN_FEDERATION_EXECUTION:
		Header.VP.Join.NbReg = msgBuf.read_int32();
		numberOfRegulators = Header.VP.Join.NbReg ;
		Header.VP.Join.AdrMC = msgBuf.read_uint32();
		multicastAddress = Header.VP.Join.AdrMC ;
		Header.VP.Join.Addr = msgBuf.read_uint32();
		bestEffortAddress = Header.VP.Join.Addr ;
		Header.VP.Join.peer = msgBuf.read_uint32();
		bestEffortPeer = Header.VP.Join.peer ;
		break ;

		// -- O_I Variable Part(No body) --

	case UNPUBLISH_OBJECT_CLASS:
	case UNSUBSCRIBE_OBJECT_CLASS:
		Header.VP.O_I.handle = msgBuf.read_int32();
		objectClass = Header.VP.O_I.handle ;
		break ;

	case PUBLISH_INTERACTION_CLASS:
	case UNPUBLISH_INTERACTION_CLASS:
	case SUBSCRIBE_INTERACTION_CLASS:
	case UNSUBSCRIBE_INTERACTION_CLASS:
	case TURN_INTERACTIONS_ON:
	case TURN_INTERACTIONS_OFF:
		Header.VP.O_I.handle = msgBuf.read_int32();
		interactionClass = Header.VP.O_I.handle ;
		break ;

		// DDM variable part
	case DDM_DELETE_REGION:
		Header.VP.ddm.region = msgBuf.read_int32();
		region = Header.VP.ddm.region ;
		break ;

		// -- O_I Variable Part(body not empty) --

	case PUBLISH_OBJECT_CLASS:
	case SUBSCRIBE_OBJECT_CLASS:
		Header.VP.O_I.handle = msgBuf.read_int32();
		objectClass = Header.VP.O_I.handle ;
		Header.VP.O_I.size = msgBuf.read_int32();
		handleArraySize = Header.VP.O_I.size ;
		break ;

	case REGISTER_OBJECT:
	case DISCOVER_OBJECT:
		Header.VP.O_I.handle = msgBuf.read_int32();
		objectClass = Header.VP.O_I.handle ;
		break ;

	case DDM_MODIFY_REGION:
		Header.VP.ddm.region = msgBuf.read_int32();
		region = Header.VP.ddm.region ;
		break ;

		// -- Default Handler --

	default:
		D.Out(pdExcept, "Unknown type %d in ReadHeader.", Header.type);
		G.Out(pdGendoc,"exit  NetworkMessage::readHeader with unknown type=%d",Header.type);
		throw RTIinternalError("Received unknown Header type.");
	}

	// 4- If Header.bodySize is not 0, return RTI_TRUE, else RTI_FALSE
	G.Out(pdGendoc,"exit  NetworkMessage::readHeader");
	return Header.bodySize ;
}

// ----------------------------------------------------------------------------
void
NetworkMessage::writeBody(Socket *socket)
{	
	unsigned short i ;

	G.Out(pdGendoc,"enter NetworkMessage::writeBody");
	
	// If the message carry an exception, the Body will only contain the
	// exception reason.
	if (Header.exception != e_NO_EXCEPTION) {
		msgBuf.write_string(exceptionReason);
	}
	else
	{
		D.Mes(pdDebug,'N',Header.type,"writeBody::");
		// 1- Prepare body Structure according to Message type
		switch(Header.type) {
		case GET_FED_FILE:
			msgBuf.write_int16(number);			
			writeFEDid();
			if ( number >= 1 )  // open (0) and close (0) no more information
			{
				msgBuf.write_int32(ValueArray[0].length);
				msgBuf.write_bytes(ValueArray[0].value, ValueArray[0].length);
			}
			break;

		case UPDATE_ATTRIBUTE_VALUES:
			msgBuf.write_int32(object);
			msgBuf.write_string(label);
			msgBuf.write_int32(boolean);    // true means with time (stored in header)
			msgBuf.write_bytes((char *) handleArray, handleArraySize * sizeof(AttributeHandle));

			for (i = 0 ; i < handleArraySize ; i ++) {
				msgBuf.write_int32(ValueArray[i].length) ;
				msgBuf.write_bytes(ValueArray[i].value, ValueArray[i].length);
			}
			break ;

		case REFLECT_ATTRIBUTE_VALUES:
			msgBuf.write_int32(object);
			msgBuf.write_string(label);
			msgBuf.write_int32(boolean);
			msgBuf.write_bytes((char *) handleArray, handleArraySize * sizeof(AttributeHandle));	
			for (i = 0 ; i < handleArraySize ; i ++) {
				msgBuf.write_int32(ValueArray[i].length) ;
				msgBuf.write_bytes(ValueArray[i].value, ValueArray[i].length);
			}
			break ;
			
			  case PROVIDE_ATTRIBUTE_VALUE_UPDATE:
	msgBuf.write_int32(object);
	for (i = 0 ; i < handleArraySize ; i ++) {
	    msgBuf.write_int16(handleArray[i]);
	}
	break ;

			// -- O_I Variable Part With date(body Not Empty) --

		case SEND_INTERACTION:
		case RECEIVE_INTERACTION:
			msgBuf.write_string(label);
			msgBuf.write_int32(boolean);    // true means with time (stored in header)
			msgBuf.write_bytes((char *) handleArray,
					handleArraySize * sizeof(AttributeHandle));
			for (i = 0 ; i < handleArraySize ; i ++) {
				msgBuf.write_int32(ValueArray[i].length);
				msgBuf.write_bytes(ValueArray[i].value, ValueArray[i].length);
			}
			msgBuf.write_int32(region);
			break ;

		case REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
			msgBuf.write_int32(object);
			for (i = 0 ; i < handleArraySize ; i ++) {
				msgBuf.write_int16(handleArray[i]) ;
			} 
			break;

			// -- No Variable Part --

		case CREATE_FEDERATION_EXECUTION:
			writeFederationName();
			writeFEDid();
			break ;

		case DESTROY_FEDERATION_EXECUTION:
			writeFederationName();
			break ;

		case REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
			msgBuf.write_string(label);
			msgBuf.write_string(tag);
			msgBuf.write_int32(boolean);
			// boolean true means we have an handleArray
			if ( boolean )
			{
				msgBuf.write_uint16(handleArraySize);
				for (i = 0 ; i < handleArraySize ; i ++) {
			        msgBuf.write_uint16(handleArray[i]);
				}
			}        
			break ;

		case ANNOUNCE_SYNCHRONIZATION_POINT:
			msgBuf.write_string(label);
			msgBuf.write_string(tag);       
			break ;

		case SYNCHRONIZATION_POINT_ACHIEVED:
		case SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
		case FEDERATION_SYNCHRONIZED:
		case REQUEST_FEDERATION_RESTORE:
			msgBuf.write_string(label);
			break ;

		case REQUEST_FEDERATION_RESTORE_SUCCEEDED:
			msgBuf.write_string(label);
			G.Out(pdGendoc,"NetworkMessage::writeBody type R_F_R_S label=%s",label.c_str());
			break ;


		case INITIATE_FEDERATE_RESTORE:
			msgBuf.write_string(label);
			break ;

		case INITIATE_FEDERATE_SAVE:
			msgBuf.write_string(label);
			// boolean true means with time (in the header)
			msgBuf.write_int32(boolean);
			break ;

		case REQUEST_FEDERATION_SAVE:
			msgBuf.write_string(label);
			// boolean true means with time (in the header)
			msgBuf.write_int32(boolean);
			break ;

		case REQUEST_FEDERATION_RESTORE_FAILED:
			msgBuf.write_string(label);
			msgBuf.write_string(tag);
			break ;

		case DELETE_OBJECT:
		case REMOVE_OBJECT:
			msgBuf.write_int32(object);
			msgBuf.write_int32(boolean);    // true means with time (stored in header)
			msgBuf.write_string(label);
			break ;

		case IS_ATTRIBUTE_OWNED_BY_FEDERATE:
		case INFORM_ATTRIBUTE_OWNERSHIP:
		case ATTRIBUTE_IS_NOT_OWNED:
		case QUERY_ATTRIBUTE_OWNERSHIP:
			msgBuf.write_int32(object);
			msgBuf.write_int32(handleArray[0]);
			msgBuf.write_string(label);
			break ;

		case NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
		case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
		case ATTRIBUTE_OWNERSHIP_ACQUISITION:
		case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
			msgBuf.write_int32(object);
			msgBuf.write_int32(handleArraySize);
			for (i = 0 ; i < handleArraySize ; i ++)
				msgBuf.write_int32(handleArray[i]);
			msgBuf.write_string(label);
			break ;


		case ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
		case ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
		case ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
		case UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
		case ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
		case CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
		case ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
		case CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
		case CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
			msgBuf.write_int32(object);
			msgBuf.write_int32(handleArraySize);
			for (i = 0 ; i < handleArraySize ; i ++)
				msgBuf.write_int32(handleArray[i]);
			break ;

		case DDM_CREATE_REGION:
			msgBuf.write_int32(space);
			msgBuf.write_int32(nbExtents);
			msgBuf.write_int32(region);
			break ;

			// -- Join Variable Part --

		case JOIN_FEDERATION_EXECUTION:
			writeFederationName();
			msgBuf.write_string(federateName);
			break ;

			// -- O_I Variable Part(body not empty) --

		case PUBLISH_OBJECT_CLASS:
		case SUBSCRIBE_OBJECT_CLASS:
			for (i = 0 ; i < handleArraySize ; i ++)
				msgBuf.write_int32(handleArray[i]);
			break ;

		case REGISTER_OBJECT:
		case DISCOVER_OBJECT:
			msgBuf.write_int32(object);
			msgBuf.write_string(label);
			break ;

		case DDM_MODIFY_REGION:
			writeExtents();
			break ;

		case DDM_ASSOCIATE_REGION:
			msgBuf.write_int32(object);
			msgBuf.write_int32(region);
			msgBuf.write_int32(boolean);
			msgBuf.write_int32(handleArraySize);
			for (i = 0 ; i < handleArraySize ; i ++)
				msgBuf.write_int32(handleArray[i]);
			break ;

		case DDM_SUBSCRIBE_ATTRIBUTES:
			msgBuf.write_int32(objectClass);
			msgBuf.write_int32(region);
			msgBuf.write_int32(boolean);
			msgBuf.write_int32(handleArraySize);
			for (i = 0 ; i < handleArraySize ; i ++)
				msgBuf.write_int32(handleArray[i]);
			break ;

		case DDM_UNASSOCIATE_REGION:
			msgBuf.write_int32(object);
			msgBuf.write_int32(region);
			break ;

		case DDM_UNSUBSCRIBE_ATTRIBUTES:
			msgBuf.write_int32(objectClass);
			msgBuf.write_int32(region);
			break ;

		case DDM_SUBSCRIBE_INTERACTION:
		case DDM_UNSUBSCRIBE_INTERACTION:
			msgBuf.write_int32(interactionClass);
			msgBuf.write_int32(region);
			msgBuf.write_int32(boolean);
			break ;

		case DDM_REGISTER_OBJECT:
			msgBuf.write_int32(objectClass);
			msgBuf.write_int32(object);
			msgBuf.write_int32(region);
			msgBuf.write_string(tag);
			msgBuf.write_int32(handleArraySize);
			for (i = 0 ; i < handleArraySize ; i ++)
				msgBuf.write_int32(handleArray[i]);
			break ;

			// -- Default Handler --
		default:
			D.Out(pdExcept, "Unknown type %d in Writebody.", Header.type);
			throw RTIinternalError("Unknown/Unimplemented type for Header.");
		}
	}

	msgBuf.updateReservedBytes();
	D.Out(pdDebug,"[Body] Sending a buffer of <%u> bytes",msgBuf.size());
	msgBuf.show(msgBuf(0),5);
	fflush(stdout);
	socket->send(static_cast<unsigned char*>(msgBuf(0)), msgBuf.size());

	G.Out(pdGendoc,"exit  NetworkMessage::writeBody");
}

void NetworkMessage::serialize() {
	G.Out(pdGendoc,"enter NetworkMessage::serialize");
	/* We serialize the common Network messages part 
	 * ALL Network Message will contain the following
	 */	
	D.Out(pdDebug,("serialize"+getName()).c_str());
	/* type of message */
	msgBuf.write_int32(type);
	msgBuf.write_int32(exception);
	msgBuf.write_int32(federate);
	msgBuf.write_int32(federation);
	if (isDated) {
		msgBuf.write_double(date);
	}
	G.Out(pdGendoc,"exit NetworkMessage::serialize");
} /* end of serialize */

void NetworkMessage::deserialize() {
	G.Out(pdGendoc,"enter NetworkMessage::deserialize");
	/* We serialize the common Network message part 
	 * ALL Network Message will contain the following
	 */	
	D.Out(pdDebug,("deserialize"+getName()).c_str());	
	/* deserialize common part */
	type        = static_cast<certi::NetworkMessage::Type>(msgBuf.read_int32());
	exception   = static_cast<certi::TypeException>(msgBuf.read_int32());
	federate    = msgBuf.read_int32();
	federation  = msgBuf.read_int32();
	if (isDated) {
		date = msgBuf.read_double();
	}
	G.Out(pdGendoc,"exit NetworkMessage::deserialize");
} /* end of deserialize */

void
NetworkMessage::send(Socket *socket) {
	G.Out(pdGendoc,"enter NetworkMessage::send");
	/* 1- serialize the message
	 * This is a polymorphic call 
	 * which may specialized in a daughter class  
	 */
	serialize();
	/* 2- update message buffer 'reserved bytes' header */
	msgBuf.updateReservedBytes();
	D.Out(pdDebug,"[Header] Sending a buffer of <%u> bytes",msgBuf.size());
	msgBuf.show(msgBuf(0),5);
	/* 3- effectively send the raw message to socket */
	socket->send(static_cast<unsigned char*>(msgBuf(0)), msgBuf.size());
    G.Out(pdGendoc,"exit  NetworkMessage::send");
} /* end of send */

void
NetworkMessage::receive(Socket* socket) {
	G.Out(pdGendoc,"enter NetworkMessage::receive");
	/* 1- Read 'reserved bytes' header from socket */
	D.Out(pdDebug,"reading %d bytes for header",msgBuf.reservedBytes);
	socket->receive(msgBuf(0), msgBuf.reservedBytes);	
	msgBuf.show(msgBuf(0),5);
	fflush(stdout);
	/* 2- update (assume) complete message size from reserved bytes */
	msgBuf.assumeSizeFromReservedBytes();
	D.Out(pdDebug,"Got a MsgBuf of size %d bytes (including %d reserved)",msgBuf.size(),msgBuf.reservedBytes);
	/* 3- receive the rest of the message */
	socket->receive(msgBuf(msgBuf.reservedBytes),msgBuf.size()-msgBuf.reservedBytes);
	/* 4- deserialize the message 
	 * This is a polymorphic call 
	 * which may specialized in a daughter class  
	 */ 
	deserialize();
	G.Out(pdGendoc,"exit  NetworkMessage::receive");	
} /* end of receive */
// ----------------------------------------------------------------------------
bool
NetworkMessage::writeHeader(Socket *socket)
{
	G.Out(pdGendoc,"enter NetworkMessage::writeHeader");
	// 2- Fill Header(Static Part)
	Header.type = type ;
	msgBuf.write_int32(Header.type);
	D.Mes(pdDebug,'N',Header.type,"writeHeader::");

	Header.exception = exception ;
	msgBuf.write_int32(Header.exception);

	Header.federate = federate ;
	msgBuf.write_int32(Header.federate);

	Header.federation = federation ;
	msgBuf.write_int32(Header.federation);

	// If the message carry an exception, the Body will only contain the
	// exception reason.
	if (exception != e_NO_EXCEPTION) {
		Header.bodySize = 1;
		msgBuf.write_int32(Header.bodySize);
		G.Out(pdGendoc,"exit  NetworkMessage::writeHeader carrying an exception");
		return true ;
	}

	// 3- Fill Header(Variable Part)[Sorted by Variable part type]
	// Note: Header.bodySize is not set to the actual body size, but
	// to zero to indicate there is no body, or 1 if a body is needed.
	switch(type) {
	case MESSAGE_NULL:
		Header.bodySize = 0 ;
		msgBuf.write_int32(Header.bodySize);
		
		Header.VP.time.date = date ;
		msgBuf.write_double(date);
		break ;

	case UPDATE_ATTRIBUTE_VALUES:
	case REFLECT_ATTRIBUTE_VALUES:
		Header.bodySize = 1 ;
		msgBuf.write_int32(Header.bodySize);
		
		Header.VP.O_I.handle = objectClass ;
		msgBuf.write_int32(objectClass);
		Header.VP.O_I.size = handleArraySize ;
		msgBuf.write_int32(handleArraySize);
		Header.VP.O_I.date = date ;
		msgBuf.write_double(date);
		break ;
		
	case SEND_INTERACTION:
	case RECEIVE_INTERACTION:
		// body contains handleArray, ValueArray, label.
		Header.bodySize = 1 ;
		msgBuf.write_int32(Header.bodySize);
		
		Header.VP.O_I.handle = interactionClass ;
		msgBuf.write_int32(interactionClass);
		Header.VP.O_I.size = handleArraySize ;
		msgBuf.write_int32(handleArraySize);
		Header.VP.O_I.date = date ;
		msgBuf.write_double(date);
		break ;

	case REQUEST_FEDERATION_SAVE:
	case INITIATE_FEDERATE_SAVE:
		Header.bodySize = 1 ;
		msgBuf.write_int32(Header.bodySize);
		// boolean true means with time
		if (boolean) {
			Header.VP.O_I.date = date ;
			msgBuf.write_double(date);
		}
		break ;

	case REQUEST_FEDERATION_RESTORE:
	case REQUEST_FEDERATION_RESTORE_SUCCEEDED:
	case REQUEST_FEDERATION_RESTORE_FAILED:
		Header.bodySize = 1 ;
		msgBuf.write_int32(Header.bodySize);
		break ;

		// Body contains Object handle,handleArray
	case REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
		Header.bodySize = 1 ;
		msgBuf.write_int32(Header.bodySize);
		
		Header.VP.O_I.size = handleArraySize ;
		msgBuf.write_int32(handleArraySize);
		break;

		// -- No Variable Part, No body --

	case CLOSE_CONNEXION:
	case RESIGN_FEDERATION_EXECUTION:
	case FEDERATE_SAVE_BEGUN:
	case FEDERATE_SAVE_COMPLETE:
	case FEDERATE_SAVE_NOT_COMPLETE:
	case FEDERATION_SAVED:
	case FEDERATION_NOT_SAVED:
	case FEDERATE_RESTORE_COMPLETE:
	case FEDERATE_RESTORE_NOT_COMPLETE:
	case FEDERATION_RESTORE_BEGUN:
	case FEDERATION_RESTORED:
	case FEDERATION_NOT_RESTORED:
		Header.bodySize = 0 ;
		msgBuf.write_int32(Header.bodySize);
		break ;

		// -- No Variable Part, body not empty --

	case CREATE_FEDERATION_EXECUTION:
	case DESTROY_FEDERATION_EXECUTION:
		
		// body Contains federationName.
		Header.bodySize = 1 ;
	    msgBuf.write_int32(Header.bodySize);
	    Header.VP.O_I.date = date ;
	    msgBuf.write_double(date);	    
		break;
		
	case INFORM_ATTRIBUTE_OWNERSHIP:
	case ATTRIBUTE_IS_NOT_OWNED:
	case IS_ATTRIBUTE_OWNED_BY_FEDERATE:
	case QUERY_ATTRIBUTE_OWNERSHIP:
		// body Contains ObjectHandle and label
	case ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
	case ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
	case ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
		// body Contains ObjectHandle and handleArray
	case NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
	case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
	case UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
	case ATTRIBUTE_OWNERSHIP_ACQUISITION:
	case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
	case ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
	case CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
	case ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
	case CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
	case CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
	case DDM_CREATE_REGION:
	case INITIATE_FEDERATE_RESTORE:
	case DDM_ASSOCIATE_REGION:
	case DDM_UNASSOCIATE_REGION:
	case DDM_SUBSCRIBE_ATTRIBUTES:
	case DDM_UNSUBSCRIBE_ATTRIBUTES:
	case DDM_SUBSCRIBE_INTERACTION:
	case DDM_UNSUBSCRIBE_INTERACTION:	
	case DDM_REGISTER_OBJECT:
	case GET_FED_FILE:
		Header.bodySize = 1 ;
		msgBuf.write_int32(Header.bodySize);
		
		break ;

	case REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
	case SYNCHRONIZATION_POINT_ACHIEVED:
	case SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
	case FEDERATION_SYNCHRONIZED:
	case ANNOUNCE_SYNCHRONIZATION_POINT:
		// body Contains Label(should be non-empty)
		// BUG: S'il fait moins de 16 octet, il passe dans le header.
		Header.bodySize = 1 ;
		msgBuf.write_int32(Header.bodySize);
		break ;

	case DELETE_OBJECT:
		Header.bodySize = 1;
		msgBuf.write_int32(Header.bodySize);
		
		Header.VP.O_I.date = date;
		msgBuf.write_double(date);
		break;
	case REMOVE_OBJECT:
		// body Contains ObjectHandle, and label
		Header.bodySize = 1 ;
		msgBuf.write_int32(Header.bodySize);
		if (boolean) {
			Header.VP.O_I.date = date ;
			msgBuf.write_double(date);
		}
		break;

		// -- time Variable Part(No body)[Continued] --
	case SET_TIME_REGULATING:
		Header.bodySize = 0 ;
		msgBuf.write_int32(Header.bodySize);
		Header.VP.time.date = date ;
		msgBuf.write_double(date);
		Header.VP.time.R_or_C = regulator ;
		msgBuf.write_double(Header.VP.time.R_or_C);
		break ;

	case SET_TIME_CONSTRAINED:
		Header.bodySize = 0 ;
		msgBuf.write_int32(Header.bodySize);
		Header.VP.time.date = date ;
		msgBuf.write_double(date);
		Header.VP.time.R_or_C = constrained ;
		msgBuf.write_double(Header.VP.time.R_or_C);
		break ;

		// -- T_O Variable Part --

	case CHANGE_ATTRIBUTE_TRANSPORT_TYPE:
	case CHANGE_ATTRIBUTE_ORDER_TYPE:
	case CHANGE_INTERACTION_TRANSPORT_TYPE:
	case CHANGE_INTERACTION_ORDER_TYPE:
		throw RTIinternalError("Write Message not implemented for T/O.");
		break ;

		// -- Join Variable Part --

	case JOIN_FEDERATION_EXECUTION:
		// body contains federationName and federateName
		Header.bodySize = 1 ;
		msgBuf.write_int32(Header.bodySize);
		Header.VP.Join.NbReg = numberOfRegulators ;
		msgBuf.write_int32(numberOfRegulators);
		Header.VP.Join.AdrMC = multicastAddress ;
		msgBuf.write_uint32(multicastAddress);
		Header.VP.Join.Addr = bestEffortAddress ;
		msgBuf.write_uint32(bestEffortAddress);
		Header.VP.Join.peer = bestEffortPeer ;
		msgBuf.write_uint32(bestEffortPeer);
		break ;

		// -- O_I Variable Part(No body) --

	case UNPUBLISH_OBJECT_CLASS:
	case UNSUBSCRIBE_OBJECT_CLASS:
		Header.bodySize = 0 ;
		msgBuf.write_int32(Header.bodySize);
		Header.VP.O_I.handle = objectClass ;
		msgBuf.write_int32(objectClass);
		break ;

	case PUBLISH_INTERACTION_CLASS:
	case UNPUBLISH_INTERACTION_CLASS:
	case SUBSCRIBE_INTERACTION_CLASS:
	case UNSUBSCRIBE_INTERACTION_CLASS:
	case TURN_INTERACTIONS_ON:
	case TURN_INTERACTIONS_OFF:
		Header.bodySize = 0 ;
		msgBuf.write_int32(Header.bodySize);
		Header.VP.O_I.handle = interactionClass ;
		msgBuf.write_int32(interactionClass);
		break ;

		// DDM variable part, no body
	case DDM_DELETE_REGION:
		Header.bodySize = 0 ;
		msgBuf.write_int32(Header.bodySize);
		Header.VP.ddm.region = region ;
		msgBuf.write_int32(region);
		break ;

		// -- O_I Variable Part(body not empty) --

	case PUBLISH_OBJECT_CLASS:
	case SUBSCRIBE_OBJECT_CLASS:
		// body contains handleArray[handleArraySize](if not empty)

		if (handleArraySize > 0) {
			Header.bodySize = 1 ;
		}
		else {
			Header.bodySize = 0 ;
		}
		msgBuf.write_int32(Header.bodySize);
		
		Header.VP.O_I.handle = objectClass ;
		msgBuf.write_int32(objectClass);
		Header.VP.O_I.size = handleArraySize ;
		msgBuf.write_int32(handleArraySize);
		break ;

	case REGISTER_OBJECT:
	case DISCOVER_OBJECT:
		// body Contains ObjectHandle and label
		Header.bodySize = 1 ;
		msgBuf.write_int32(Header.bodySize);
		Header.VP.O_I.handle = objectClass ;
		msgBuf.write_int32(objectClass);
		break ;

	case DDM_MODIFY_REGION:
		Header.bodySize = 1 ;
		msgBuf.write_int32(Header.bodySize);
		Header.VP.ddm.region = region ;
		msgBuf.write_int32(region);
		break ;

	default:
		D.Out(pdExcept, "Unknown type %d in WriteHeader.", Header.type);
		throw RTIinternalError("Unknown/Unimplemented type for Header.");
	}

	/* 
	 * If the message has no body we send the prepared header buffer 
	 * Note that we may pass msgBuf object to send because it will be casted out
	 * by the MessageBuffer Cast operator.     
	 */
	if (Header.bodySize == 0) {
		msgBuf.updateReservedBytes();
		D.Out(pdDebug,"[Header] Sending a buffer of <%u> bytes",msgBuf.size());
		msgBuf.show(msgBuf(0),5);
		socket->send(static_cast<unsigned char*>(msgBuf(0)), msgBuf.size());
	}

	G.Out(pdGendoc,"exit  NetworkMessage::writeHeader");
	return (Header.bodySize != 0);
}

} // namespace certi

// $Id: NetworkMessage_RW.cc,v 3.45.2.2 2008/04/08 14:05:26 erk Exp $
