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
// $Id: Message_RW.cc,v 3.33 2007/07/06 09:25:17 erk Exp $
// ----------------------------------------------------------------------------


#include "Message.hh"

#include <cassert>

using std::vector ;

namespace certi {

static pdCDebug D("RTIA_MSG","(LocalMESS) - ");

// You can comment this out if you don't want to optimize network messages.
#define USE_HEADER_AND_BODY

void Message::trace(const char* context)
{
D.Mes(pdMessage,'M',this->type,context);
}

// ----------------------------------------------------------------------------
//! Read NetworkMessage Objects from Socket objects.
void
Message::read(SocketUN *socket)
    throw (NetworkError, NetworkSignal)
{
#ifdef USE_HEADER_AND_BODY

    bool has_body = readHeader(socket);
    if (has_body) readBody(socket);

#else
#error    
    socket->receive((void *) this, sizeof(Message));
#endif
}

// ----------------------------------------------------------------------------
//! Read a Message Body from a Socket, should be called after ReadHeader.
void
Message::readBody(SocketUN *socket)
{
    assert(header.bodySize > 0);

    MessageBody body(header.bodySize);

    // 1. Read Body from socket.
    socket->receive(body.getBuffer(), header.bodySize);
	 // FIXME EN: we must update the write pointer of the 
	 //           MessageBody because we have just written 
	 //           on it using direct pointer access !! (nasty usage)
	 body.addToWritePointer(header.bodySize);
	
    // 3. Read informations from Message Body according to message type.
    if (header.exception != e_NO_EXCEPTION) {
        body.readString(exceptionReason, MAX_EXCEPTION_REASON_LENGTH);
    }
    else {
 
        // 1- Prepare Body Structure according to Message Type
			//D.Mes(pdMessage, 'M', header.type);
			this->trace("RTIG::chooseProcessingMethod ");

        switch(header.type) {

            // --- No Variable Part, Body not empty ---

          case CREATE_FEDERATION_EXECUTION:
            readFederationName(body);
            readFEDid(body) ;
            break ;

          case DESTROY_FEDERATION_EXECUTION:
            readFederationName(body);
            break ;

          case REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
          case ANNOUNCE_SYNCHRONIZATION_POINT:
          case REQUEST_FEDERATION_RESTORE_FAILED:
            readLabel(body);
            readTag(body);
            break ;

          case SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
          case SYNCHRONIZATION_POINT_ACHIEVED:
          case FEDERATION_SYNCHRONIZED:
          case REQUEST_FEDERATION_SAVE:
          case INITIATE_FEDERATE_SAVE:
          case REQUEST_FEDERATION_RESTORE:
          case REQUEST_FEDERATION_RESTORE_SUCCEEDED:
          case INITIATE_FEDERATE_RESTORE:
            readLabel(body);
            break ;

          case IS_ATTRIBUTE_OWNED_BY_FEDERATE:
          case QUERY_ATTRIBUTE_OWNERSHIP:
            // B.c. Objectid, AttribHandle and Tag.
            object = body.readLongInt();
            attribute = body.readShortInt();
            readTag(body);
            break ;

          case ATTRIBUTE_IS_NOT_OWNED:
          case INFORM_ATTRIBUTE_OWNERSHIP:
            object = body.readLongInt();
            attribute = body.readShortInt();
            federate = body.readShortInt();
            break ;

          case NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
          case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
          case ATTRIBUTE_OWNERSHIP_ACQUISITION:
          case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
            object = body.readLongInt();
            handleArraySize = body.readShortInt();
            readHandleArray(body);
            readTag(body);
            break ;

          case ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
          case ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
          case ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
          case UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
          case CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
          case ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
          case CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
          case CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
          case ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
            object = body.readLongInt();
            handleArraySize = body.readShortInt();
            readHandleArray(body);
            break ;

	  case DDM_ASSOCIATE_REGION:
	    object = body.readLongInt();
	    region = body.readLongInt();
	    boolean = body.readLongInt();
	    handleArraySize = body.readShortInt();
            readHandleArray(body);
	    break ;

	  case DDM_REGISTER_OBJECT:
	    objectClass = body.readLongInt();
	    object = body.readLongInt();
	    readTag(body);
	    handleArraySize = body.readShortInt();
            readHandleArray(body);
	    readRegions(body);
	    break ;

	  case DDM_SUBSCRIBE_ATTRIBUTES:
	    objectClass = body.readLongInt();
	    region = body.readLongInt();
	    boolean = body.readLongInt();
	    handleArraySize = body.readShortInt();
            readHandleArray(body);
	    break ;

	  case DDM_UNASSOCIATE_REGION:
	    object = body.readLongInt();
	    region = body.readLongInt();
	    break ;

	  case DDM_UNSUBSCRIBE_ATTRIBUTES:	    
	    objectClass = body.readLongInt();
	    region = body.readLongInt();
	    break ;

	  case DDM_SUBSCRIBE_INTERACTION:
	  case DDM_UNSUBSCRIBE_INTERACTION:
	    interactionClass = body.readLongInt();
	    region = body.readLongInt();
	    boolean = body.readLongInt();
	    break ;
	    
          case GET_ATTRIBUTE_SPACE_HANDLE:
            objectClass = body.readLongInt();
            attribute = body.readLongInt();
            space = body.readLongInt();
            break ;

          case DDM_CREATE_REGION:
            space = body.readLongInt();
            number = body.readLongInt();
            region = body.readLongInt();
            break ;

          case GET_INTERACTION_SPACE_HANDLE:
            interactionClass = body.readLongInt();
            space = body.readLongInt();
            break ;

            // --- MessageJ_R_Struct --

          case JOIN_FEDERATION_EXECUTION:
            readFederationName(body);
            readFederateName(body);
            break ;

            // --- MessageO_I_Struct ---

          case PUBLISH_OBJECT_CLASS:
          case SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES:
            readHandleArray(body);
            break ;

          case REGISTER_OBJECT_INSTANCE:
            object = body.readLongInt();
            readName(body);
            break ;

          case UPDATE_ATTRIBUTE_VALUES:
            // B.c. object, Tag, HandleArray[], ValueArray[] and RAction.
            // and boolean (true means UAV with time, false without time)
            object = body.readLongInt();
            readTag(body);
            readHandleArray(body);
            readValueArray(body);
            readResignAction(body);
            boolean = body.readLongInt();
            break ;

          case REFLECT_ATTRIBUTE_VALUES:
            // B.c. object, Tag, HandleArray[], ValueArray[] and RAction.
            object = body.readLongInt();
            readTag(body);
            readHandleArray(body);
            readValueArray(body);
            readResignAction(body);
            break ;

          case DISCOVER_OBJECT_INSTANCE:
          case DELETE_OBJECT_INSTANCE:
          case REMOVE_OBJECT_INSTANCE:
            // B.c. object, Tag, Label, RAction
            object = body.readLongInt();
            readTag(body);
            readName(body);
            readLabel(body);
            readResignAction(body);
            break ;

          case GET_OBJECT_CLASS_HANDLE:
          case GET_OBJECT_CLASS_NAME:
          case GET_ATTRIBUTE_HANDLE:
          case GET_ATTRIBUTE_NAME:
            // B.c. Name(and attribute)
            readName(body);
            attribute = body.readShortInt();
            break ;

          case GET_SPACE_HANDLE:
          case GET_SPACE_NAME:
            this->readName(body);
            this->space = body.readLongInt();
            break ;

          case GET_DIMENSION_HANDLE:
          case GET_DIMENSION_NAME:
            this->readName(body);
            this->dimension = body.readLongInt();
            this->space = body.readLongInt();
            break ;

          case SEND_INTERACTION:
          case RECEIVE_INTERACTION:
            // B.c. Tag, HandleArray[], ValueArray[], RAction
            readTag(body);
            readHandleArray(body);
            readValueArray(body);
	    region = body.readLongInt();
            readResignAction(body);
            break ;

          case GET_INTERACTION_CLASS_HANDLE:
          case GET_INTERACTION_CLASS_NAME:
          case GET_PARAMETER_HANDLE:
          case GET_PARAMETER_NAME:
            // body contains Name and ParamHandle
            readName(body);
            parameter = body.readShortInt();
            break ;

            // --- MessageT_O_Struct, body not empty ---

          case CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE:
          case CHANGE_ATTRIBUTE_ORDER_TYPE:
            // B.c. object, HandleArray
            object = body.readLongInt();
            readHandleArray(body);
            break ;

	  case DDM_MODIFY_REGION:
	    readExtents(body);
	    break ;

	  case GET_OBJECT_INSTANCE_HANDLE:
	  case GET_OBJECT_INSTANCE_NAME:
            object = body.readLongInt();
	    readName(body);
	    break;
	    
            // -- Default Handler --

          default:
            D.Out(pdExcept, "Unknown Type %d in ReadBody.", header.type);
            throw RTIinternalError("Message: Unknown Type for Body(Read).");
        }
    }
}

// ----------------------------------------------------------------------------
/*! Read a Header from a socket, and process it to read its content. Return
  RTI_TRUE if the ReadBody Method has to be called.
*/
bool
Message::readHeader(SocketUN *socket)
{
    // 1- Read Header from Socket
    socket->receive((const unsigned char *) &header, sizeof(MessageHeader));

    // 2- Parse Header(Static Part)
    type = header.type ;
    exception = header.exception ;

    // If the message carry an exception, the Body will only contain the
    // exception reason.

    if (exception != e_NO_EXCEPTION)
        return true ;

    // 2- Parse Header according to its type(Variable Part)
    // NULL, UAV and SendInteraction are the most common ones.

    switch(type) {

        // --- No Variable Part, Body not empty ---

      case CREATE_FEDERATION_EXECUTION: // Body contains NomFederation
      case DESTROY_FEDERATION_EXECUTION: // Body contains NomFedere
      case REGISTER_FEDERATION_SYNCHRONIZATION_POINT: // Body contains Label
      case SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
      case ANNOUNCE_SYNCHRONIZATION_POINT:
      case SYNCHRONIZATION_POINT_ACHIEVED: // Body contains Label
      case FEDERATION_SYNCHRONIZED:
      case INITIATE_FEDERATE_SAVE:
      case IS_ATTRIBUTE_OWNED_BY_FEDERATE:// B.c. object, attribute and Tag.
      case QUERY_ATTRIBUTE_OWNERSHIP: // B.c. object and attribute.
      case ATTRIBUTE_IS_NOT_OWNED:
      case INFORM_ATTRIBUTE_OWNERSHIP:
      case NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
      case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
      case ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
      case ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
      case ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
      case UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
      case ATTRIBUTE_OWNERSHIP_ACQUISITION:
      case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
      case ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
      case CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
      case ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
      case CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
      case CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
      case GET_ATTRIBUTE_SPACE_HANDLE:
      case GET_INTERACTION_SPACE_HANDLE:
      case DDM_CREATE_REGION:
      case REQUEST_FEDERATION_RESTORE:
      case REQUEST_FEDERATION_RESTORE_SUCCEEDED:
      case REQUEST_FEDERATION_RESTORE_FAILED:
      case FEDERATE_RESTORE_COMPLETE:
      case FEDERATE_RESTORE_NOT_COMPLETE:
      case FEDERATION_RESTORED:
      case FEDERATION_NOT_RESTORED:
      case FEDERATION_RESTORE_BEGUN:
      case DDM_ASSOCIATE_REGION:
      case DDM_UNASSOCIATE_REGION:
      case DDM_SUBSCRIBE_ATTRIBUTES:
      case DDM_UNSUBSCRIBE_ATTRIBUTES:
      case DDM_SUBSCRIBE_INTERACTION:
      case DDM_UNSUBSCRIBE_INTERACTION:
      case DDM_REGISTER_OBJECT:
        break ;

        // --- MessageJ_R_Struct --

      case RESIGN_FEDERATION_EXECUTION: // No body
        resignAction = header.VP.J_R.action ;
        break ;

      case INITIATE_FEDERATE_RESTORE:
      case JOIN_FEDERATION_EXECUTION: // Body contains NomFederation&NomFedere
        federate = header.VP.J_R.federate ;
        break ;

        // --- MessageO_I_Struct, No body ---

      case UNPUBLISH_OBJECT_CLASS:
      case UNSUBSCRIBE_OBJECT_CLASS:
        objectClass = header.VP.O_I.handle ;
        break ;


      case PUBLISH_INTERACTION_CLASS:
      case UNPUBLISH_INTERACTION_CLASS:
      case SUBSCRIBE_INTERACTION_CLASS:
      case UNSUBSCRIBE_INTERACTION_CLASS:
      case TURN_INTERACTIONS_ON:
      case TURN_INTERACTIONS_OFF:
        interactionClass = header.VP.O_I.handle ;
        break ;

        // --- MessageO_I_Struct, body not Empty ---

      case PUBLISH_OBJECT_CLASS: // Body contains HandleArray
      case SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES: // Body contains HandleArray
      case REGISTER_OBJECT_INSTANCE: // Body contains object
      case DISCOVER_OBJECT_INSTANCE: // B.c. object, Tag and resignAction
      case REFLECT_ATTRIBUTE_VALUES: // B.c. object, Tag, HandleArray[]
        // and ValueArray[]
      case DELETE_OBJECT_INSTANCE: // Body contains object, Tag.
      case REMOVE_OBJECT_INSTANCE: // B.c. object, Tag, Label&resignAction
      case GET_OBJECT_CLASS_HANDLE: // Body contains Name
      case GET_OBJECT_CLASS_NAME: // Body contains Name
      case GET_ATTRIBUTE_HANDLE: // B.c. Name and attribute.
      case GET_ATTRIBUTE_NAME: // B.c. Name and attribute.
        objectClass = header.VP.O_I.handle ;
        handleArraySize = header.VP.O_I.size ;
        setFederationTime(header.VP.O_I.date);
        break ;

     case UPDATE_ATTRIBUTE_VALUES: // B.c. object, Tag, HandleArray[]
        // ValueArray[] and resignAction.
        objectClass = header.VP.O_I.handle ;
        handleArraySize = header.VP.O_I.size ;
        setFederationTime(header.VP.O_I.date);
        break ;

      case SEND_INTERACTION: // B.c. Tag, HandleArray[], ValueArray[]
      case RECEIVE_INTERACTION: // B.c. Tag, HandleArray[], ValueArray[], resignAction
      case GET_INTERACTION_CLASS_HANDLE: // Body contains Name
      case GET_INTERACTION_CLASS_NAME: // Body contains Name
      case GET_PARAMETER_HANDLE: // Body contains Name and parameter
      case GET_PARAMETER_NAME: // Body contains Name and parameter
        interactionClass = header.VP.O_I.handle ;
        handleArraySize = header.VP.O_I.size ;
        setFederationTime(header.VP.O_I.date );
        break ;

      case GET_SPACE_HANDLE:
      case GET_SPACE_NAME:
      case GET_DIMENSION_NAME:
      case GET_DIMENSION_HANDLE:
        this->space = header.VP.O_I.handle ;
        handleArraySize = header.VP.O_I.size ;
        setFederationTime(header.VP.O_I.date );
        break ;

        // --- MessageT_O_Struct, No Body ---

      case CHANGE_INTERACTION_TRANSPORTATION_TYPE:
      case CHANGE_INTERACTION_ORDER_TYPE:
        interactionClass = header.VP.T_O.handle ;
        transport = header.VP.T_O.transport ;
        order = header.VP.T_O.order ;
        break ;

        // Message_DDM, no body
      case DDM_DELETE_REGION:
        region = header.VP.ddm.region ;
        break ;

	// Message_DDM, Body
      case DDM_MODIFY_REGION:
	region = header.VP.ddm.region ;
	break ;	
	
        // --- MessageT_O_Struct, Body not empty ---

      case CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE: // B.c. object, HandleArray.
      case CHANGE_ATTRIBUTE_ORDER_TYPE: // B.c. object, HandleArray.
        handleArraySize = header.VP.T_O.size ;
        transport = header.VP.T_O.transport ;
        order = header.VP.T_O.order ;
        break ;

        // --- TimeStruct, No Body ---
        // case REQUEST_FEDERATION_TIME:
      case QUERY_LBTS:
      case QUERY_FEDERATE_TIME:
      case TIME_ADVANCE_REQUEST:
      case NEXT_EVENT_REQUEST:
      case TIME_ADVANCE_GRANT:
        setFederationTime(header.VP.time.date );
        break ;

      case MODIFY_LOOKAHEAD:
      case QUERY_LOOKAHEAD:
        lookahead = header.VP.time.date ;
        break ;

      case ENABLE_TIME_REGULATION:
      case DISABLE_TIME_REGULATION:
      case ENABLE_TIME_CONSTRAINED:
      case DISABLE_TIME_CONSTRAINED:
      case TICK_REQUEST:
        boolean = header.VP.time.mode ;
        break ;

      case GET_OBJECT_INSTANCE_HANDLE:
      case GET_OBJECT_INSTANCE_NAME:
	object = header.VP.O_I.handle;
        handleArraySize = header.VP.O_I.size ;
        setFederationTime(header.VP.O_I.date);
        break ;

      default:
        D.Out(pdExcept, "Unknown type %d in ReadHeader.", header.type);
        throw RTIinternalError("Message: Received unknown Header type.");
    }

    // 4- Return depends on body
    return header.bodySize != 0 ;
}

// ----------------------------------------------------------------------------
void
Message::readHandleArray(MessageBody &body)
{
    body.readBlock((char *) handleArray, handleArraySize * sizeof(AttributeHandle));
}

// ----------------------------------------------------------------------------
void
Message::readLabel(MessageBody &body)
{
    body.readString(label, MAX_USER_TAG_LENGTH);
}

// ----------------------------------------------------------------------------
void
Message::readName(MessageBody &body)
{
    body.readString(name, MAX_USER_TAG_LENGTH);
}

// ----------------------------------------------------------------------------
void
Message::readFederationName(MessageBody &body)
{
    body.readString(federationName, MAX_FEDERATION_NAME_LENGTH);
}

// ----------------------------------------------------------------------------
void
Message::readFederateName(MessageBody &body)
{
    body.readString(federateName, MAX_FEDERATE_NAME_LENGTH);
}

// ----------------------------------------------------------------------------
void
Message::readResignAction(MessageBody &)
{
    // BUG: Should do something.
}

// ----------------------------------------------------------------------------
void
Message::readTag(MessageBody &body)
{
    body.readString(tag, MAX_USER_TAG_LENGTH);
}

// ----------------------------------------------------------------------------
void
Message::readFEDid(MessageBody &body)
{
    body.readString(FEDid, MAX_FEDFILE_NAME_LENGTH);
}
// ----------------------------------------------------------------------------
//! readValueArray.
void
Message::readValueArray(MessageBody &body)
{
// valueArray contains length and value
// so we have to read length and then value with a readBlock
    for (int i = 0 ; i < handleArraySize ; i ++)
        {
        valueArray[i].length = body.readLongInt() ;
        body.readBlock((char *) valueArray[i].value, valueArray[i].length);
        }
}

// ----------------------------------------------------------------------------
//! Write NetworkMessage Objects to Socket objects.
void
Message::write(SocketUN *socket)
    throw (NetworkError, NetworkSignal)
{
#ifdef USE_HEADER_AND_BODY

    bool result ;

    result = writeHeader(socket);
    if (result)
        writeBody(socket);

#else
    socket->send((void *) this, sizeof(Message));
#endif
}

// ----------------------------------------------------------------------------
/*! Prepare and write a Body to a socket, should be called after WriteHeader.
  The message is written onto the socket by WriteHeader if no body is
  required, or by WriteBody is a body has been required by WriteHeader.
*/
void
Message::writeBody(SocketUN *socket)
{
    MessageBody body ;

    // 0- Copy the Header at the beginning of the Body, in order to
    // make a single Socket->Emettre call while sending both.
    // WARNING: As the Body size is not known yet, we will have to
    // change it in the copy also!
    body.writeBlock((char *) &header, sizeof(MessageHeader));

    // If the message carry an exception, the Body will only contain the
    // exception reason.

    if (header.exception != e_NO_EXCEPTION) {
        body.writeString(exceptionReason);
    }
    else {

        // 1- Prepare Body Structure according to Message type
        switch(header.type) {

            // --- No Variable Part, Body not empty ---

          case CREATE_FEDERATION_EXECUTION:
            body.writeString(federationName);
            body.writeString(FEDid);
            break ;

          case DESTROY_FEDERATION_EXECUTION:
            body.writeString(federationName);
            break ;

          case REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
          case ANNOUNCE_SYNCHRONIZATION_POINT:
          case REQUEST_FEDERATION_RESTORE_FAILED:
            body.writeString(label);
            body.writeString(tag);
            break ;

          case SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
          case SYNCHRONIZATION_POINT_ACHIEVED:
          case FEDERATION_SYNCHRONIZED:
            body.writeString(label);
            break ;

          case IS_ATTRIBUTE_OWNED_BY_FEDERATE:
          case QUERY_ATTRIBUTE_OWNERSHIP:
            // B.c. object, attribute, Tag
            body.writeLongInt(object);
            body.writeShortInt(attribute);
            body.writeString(tag);
            break ;

          case ATTRIBUTE_IS_NOT_OWNED:
          case INFORM_ATTRIBUTE_OWNERSHIP:
            body.writeLongInt(object);
            body.writeShortInt(attribute);
            body.writeShortInt(federate);
            break ;

          case NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
          case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
          case ATTRIBUTE_OWNERSHIP_ACQUISITION:
          case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
            body.writeLongInt(object);
            body.writeShortInt(handleArraySize);
            writeHandleArray(body);
            body.writeString(tag);
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
            body.writeLongInt(object);
            body.writeShortInt(handleArraySize);
            writeHandleArray(body);
            break ;

	  case DDM_ASSOCIATE_REGION:
	    body.writeLongInt(object);
	    body.writeLongInt(region);
	    body.writeLongInt(boolean);
            body.writeShortInt(handleArraySize);
            writeHandleArray(body);
	    break ;

	  case DDM_REGISTER_OBJECT:
	    body.writeLongInt(objectClass);
	    body.writeLongInt(object);
	    body.writeString(tag);
            body.writeShortInt(handleArraySize);
            writeHandleArray(body);
	    writeRegions(body);
	    break ;

	  case DDM_SUBSCRIBE_ATTRIBUTES:
	    body.writeLongInt(objectClass);
	    body.writeLongInt(region);
	    body.writeLongInt(boolean);
            body.writeShortInt(handleArraySize);
            writeHandleArray(body);
	    break ;

	  case DDM_UNASSOCIATE_REGION:
	    body.writeLongInt(object);
	    body.writeLongInt(region);
	    break ;

	  case DDM_UNSUBSCRIBE_ATTRIBUTES:
	    body.writeLongInt(objectClass);
	    body.writeLongInt(region);
	    break ;
	    
	  case DDM_SUBSCRIBE_INTERACTION:
	  case DDM_UNSUBSCRIBE_INTERACTION:
	    body.writeLongInt(interactionClass);
	    body.writeLongInt(region);
	    body.writeLongInt(boolean);
	    break ;
	    
          case GET_ATTRIBUTE_SPACE_HANDLE:
            body.writeLongInt(objectClass);
            body.writeLongInt(attribute);
            body.writeLongInt(space);
            break ;

          case DDM_CREATE_REGION:
            body.writeLongInt(space);
            body.writeLongInt(number);
            body.writeLongInt(region);
            break ;

          case GET_INTERACTION_SPACE_HANDLE:
            body.writeLongInt(interactionClass);
            body.writeLongInt(space);
            break ;

            // --- MessageJ_R_Struct --

          case JOIN_FEDERATION_EXECUTION:
            body.writeString(federationName);
            body.writeString(federateName);
            break ;

            // --- MessageO_I_Struct ---

          case PUBLISH_OBJECT_CLASS:
          case SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES:
            writeHandleArray(body);
            break ;

          case REGISTER_OBJECT_INSTANCE:
            body.writeLongInt(object);
            body.writeString(name);
            break ;

          case UPDATE_ATTRIBUTE_VALUES:
          case REFLECT_ATTRIBUTE_VALUES:
            // B.c. object, Tag, handleArray[], ValueArray[] and resignAction.
            // and also a boolean (true UAV or RAV with time, false without time)
            body.writeLongInt(object);
            body.writeString(tag);
            writeHandleArray(body);
            writeValueArray(body);
            writeResignAction(body);
            body.writeLongInt(boolean);
            break ;

          case DISCOVER_OBJECT_INSTANCE:
          case DELETE_OBJECT_INSTANCE:
          case REMOVE_OBJECT_INSTANCE:
            // B.c. object, Tag, label, resignAction
            body.writeLongInt(object);
            body.writeString(tag);
            body.writeString(name);
            body.writeString(label);
            writeResignAction(body);
            break ;

          case GET_OBJECT_CLASS_HANDLE:
          case GET_OBJECT_CLASS_NAME:
          case GET_ATTRIBUTE_HANDLE:
          case GET_ATTRIBUTE_NAME:
            // B.c. name(and attribute)
            body.writeString(name);
            body.writeShortInt(attribute);
            break ;

          case GET_SPACE_HANDLE:
          case GET_SPACE_NAME:
            body.writeString(name);
            body.writeLongInt(space);
            break ;

          case GET_DIMENSION_HANDLE:
          case GET_DIMENSION_NAME:
            body.writeString(name);
            body.writeLongInt(dimension);
            body.writeLongInt(space);
            break ;

          case SEND_INTERACTION:
          case RECEIVE_INTERACTION:
            // B.c. Tag, HandleArray[], ValueArray[], resignAction
            body.writeString(tag);
            writeHandleArray(body);
            writeValueArray(body);
	    body.writeLongInt(region);
            writeResignAction(body);
            break ;

          case GET_INTERACTION_CLASS_HANDLE:
          case GET_INTERACTION_CLASS_NAME:
          case GET_PARAMETER_HANDLE:
          case GET_PARAMETER_NAME:
            // Body contains name and parameter
            body.writeString(name);
            body.writeShortInt(parameter);
            break ;


            // --- MessageT_O_Struct, Body not empty ---

          case CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE:
          case CHANGE_ATTRIBUTE_ORDER_TYPE:
            // B.c. object, HandleArray
            body.writeLongInt(object);
            writeHandleArray(body);
            break ;

	  case DDM_MODIFY_REGION:
	    writeExtents(body);
	    break ;
	    
	  case GET_OBJECT_INSTANCE_HANDLE:
	  case GET_OBJECT_INSTANCE_NAME :
	    body.writeLongInt(object);
	    body.writeString(name);
	    break;
	    
            // -- Default Handler --

          default:
            D.Out(pdExcept, "Unknown type %d in Writebody.", header.type);
            throw RTIinternalError("Message: Unknown type for body.");

        }
    }

    // 2- Set Header.bodySize

    // Body Size does not include the copy of the Header!
    header.bodySize = body.size() - sizeof(MessageHeader);

    // Put the real Body Size in the copy of the Header.
    (reinterpret_cast<MessageHeader *>(body.getBufferModeRW()))->bodySize = header.bodySize ;

    // 3- Write Header to socket, then write Body to socket.
    // socket->send((void *) &Header, sizeof(MessageHeader));
    socket->send(body.getBuffer(), body.size());
}

// ----------------------------------------------------------------------------
void
Message::writeHandleArray(MessageBody &body)
{
    body.writeBlock((char *) handleArray, handleArraySize * sizeof(AttributeHandle));
}

// ----------------------------------------------------------------------------
/*! Prepare and Write a Header to a Socket, and return RTI_TRUE if the
  WriteBody method has to be called.
  The message is written onto the socket by WriteHeader if no body is
  required, or by WriteBody is a body has been required by WriteHeader.
*/
bool
Message::writeHeader(SocketUN *socket)
{
    // 1- Clear Header
    memset((void *) &header, '\0', sizeof(MessageHeader));

    // 2- Fill Header(Static Part)
    header.type = this->type ;
    header.exception = this->exception ;

    // If the message carry an exception, the Body will only contain the
    // exception reason.

    if (exception != e_NO_EXCEPTION) {
        header.bodySize = 1 ;
        return true ;
    }

    // 3- Fill Header(Variable Part)[Sorted by Variable part type]
    // Note: Header.bodySize is not set to the actual Body size, but
    // to zero to indicate there is no Body, or 1 if a Body is needed.

    switch(this->type) {

        // --- No Variable Part, Body not empty ---

      case CREATE_FEDERATION_EXECUTION: // Body contains federationName
      case DESTROY_FEDERATION_EXECUTION: // Body contains federationName
      case REGISTER_FEDERATION_SYNCHRONIZATION_POINT: // Body contains Label
      case SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
      case ANNOUNCE_SYNCHRONIZATION_POINT:
      case SYNCHRONIZATION_POINT_ACHIEVED: // Body contains Label
      case FEDERATION_SYNCHRONIZED:
      case IS_ATTRIBUTE_OWNED_BY_FEDERATE:// B.c. object, attribute and Tag.
      case QUERY_ATTRIBUTE_OWNERSHIP: // B.c. object and attribute.
      case ATTRIBUTE_IS_NOT_OWNED:
      case INFORM_ATTRIBUTE_OWNERSHIP:
      case NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
      case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
      case ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
      case ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
      case ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
      case UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
      case ATTRIBUTE_OWNERSHIP_ACQUISITION:
      case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
      case ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
      case CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
      case ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
      case CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
      case CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
      case GET_ATTRIBUTE_SPACE_HANDLE:
      case GET_INTERACTION_SPACE_HANDLE:
      case DDM_CREATE_REGION:
      case DDM_ASSOCIATE_REGION:
      case DDM_UNASSOCIATE_REGION:
      case DDM_SUBSCRIBE_ATTRIBUTES:
      case DDM_UNSUBSCRIBE_ATTRIBUTES:
      case DDM_SUBSCRIBE_INTERACTION:
      case DDM_UNSUBSCRIBE_INTERACTION:
      case DDM_REGISTER_OBJECT:
        header.bodySize = 1 ;
        break ;

      case REQUEST_FEDERATION_SAVE: // Body contains Label
        header.VP.O_I.date = getFederationTime() ;
        header.bodySize = 1 ;
        break ;

      case FEDERATE_SAVE_BEGUN:
      case FEDERATE_SAVE_COMPLETE:
      case FEDERATE_SAVE_NOT_COMPLETE:
      case FEDERATION_SAVED:
      case FEDERATION_NOT_SAVED:
      case REQUEST_FEDERATION_RESTORE:
      case REQUEST_FEDERATION_RESTORE_SUCCEEDED:
      case REQUEST_FEDERATION_RESTORE_FAILED:
      case FEDERATE_RESTORE_COMPLETE:
      case FEDERATE_RESTORE_NOT_COMPLETE:
      case FEDERATION_RESTORED:
      case FEDERATION_NOT_RESTORED:
      case FEDERATION_RESTORE_BEGUN:
        header.bodySize = 0 ;
        break ;

        // --- MessageJ_R_Struct --

      case RESIGN_FEDERATION_EXECUTION: // No Body
        header.VP.J_R.action = resignAction ;
        header.bodySize = 0 ;
        break ;

      case INITIATE_FEDERATE_RESTORE:
      case JOIN_FEDERATION_EXECUTION: // Body contains federationName&federateName
        header.VP.J_R.federate = federate ;
        header.bodySize = 1 ;
        break ;

        // --- MessageO_I_Struct, No Body ---

      case UNPUBLISH_OBJECT_CLASS:
      case UNSUBSCRIBE_OBJECT_CLASS:
        header.VP.O_I.handle = this->objectClass ;
        header.bodySize = 0 ;
        break ;

      case PUBLISH_INTERACTION_CLASS:
      case UNPUBLISH_INTERACTION_CLASS:
      case SUBSCRIBE_INTERACTION_CLASS:
      case UNSUBSCRIBE_INTERACTION_CLASS:
      case TURN_INTERACTIONS_ON:
      case TURN_INTERACTIONS_OFF:
        header.VP.O_I.handle = this->interactionClass ;
        header.bodySize = 0 ;
        break ;

        // --- MessageO_I_Struct, Body not Empty ---

      case PUBLISH_OBJECT_CLASS: // Body contains handleArray
      case SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES: // Body contains handleArray
      case REGISTER_OBJECT_INSTANCE: // Body contains object
      case DISCOVER_OBJECT_INSTANCE: // B.c. object, Tag and resignAction
      case REFLECT_ATTRIBUTE_VALUES: // B.c. object, Tag, handleArray[]
        // and ValueArray[]
      case DELETE_OBJECT_INSTANCE: // Body contains object, Tag.
      case REMOVE_OBJECT_INSTANCE: // B.c. object, Tag, label&resignAction
      case GET_OBJECT_CLASS_HANDLE: // Body contains name
      case GET_OBJECT_CLASS_NAME: // Body contains name
      case GET_ATTRIBUTE_HANDLE: // B.c. name and attribute.
      case GET_ATTRIBUTE_NAME: // B.c. name and attribute.
        header.VP.O_I.handle = objectClass ;
        header.VP.O_I.size = handleArraySize ;
        header.VP.O_I.date = getFederationTime() ;
        header.bodySize = 1 ;
        break ;

      case UPDATE_ATTRIBUTE_VALUES: // B.c. object, Tag, handleArray[]
        // ValueArray[] and resignAction.
        header.VP.O_I.handle = objectClass ;
        header.VP.O_I.size = handleArraySize ;
        // UAV with time (boolean=true) needs date
        if ( boolean )
            header.VP.O_I.date = getFederationTime() ;
        else
            header.VP.O_I.date = 0 ;
        header.bodySize = 1 ;
        break ;

      case SEND_INTERACTION: // B.c. Tag, handleArray[], ValueArray[]
      case RECEIVE_INTERACTION: // B.c. Tag, handleArray[],
        // ValueArray[], resignAction
      case GET_INTERACTION_CLASS_HANDLE: // Body contains name
      case GET_INTERACTION_CLASS_NAME: // Body contains name
      case GET_PARAMETER_HANDLE: // Body contains name and parameter
      case GET_PARAMETER_NAME: // Body contains name and parameter
        header.VP.O_I.handle = interactionClass ;
        header.VP.O_I.size = handleArraySize ;
        header.VP.O_I.date = getFederationTime() ;
        header.bodySize = 1 ;
        break ;

      case GET_SPACE_HANDLE:
      case GET_SPACE_NAME:
      case GET_DIMENSION_HANDLE:
      case GET_DIMENSION_NAME:
        header.VP.O_I.handle = space ;
        header.VP.O_I.size = handleArraySize ;
        header.VP.O_I.date = getFederationTime() ;
        header.bodySize = 1 ;
        break ;

        // --- MessageT_O_Struct, No Body ---

      case CHANGE_INTERACTION_TRANSPORTATION_TYPE:
      case CHANGE_INTERACTION_ORDER_TYPE:
        header.VP.T_O.handle = interactionClass ;
        header.VP.T_O.transport = transport ;
        header.VP.T_O.order = order ;
        header.bodySize = 0 ;
        break ;

        // Message_DDM, no body
      case DDM_DELETE_REGION:
        header.VP.ddm.region = region ;
        header.bodySize = 0 ;
        break ;

	// Message_DDM, Body
      case DDM_MODIFY_REGION:
	header.VP.ddm.region = region ;
	header.bodySize = 1 ;
	break ;

        // --- MessageT_O_Struct, Body not empty ---

      case CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE: // B.c. object, handleArray.
      case CHANGE_ATTRIBUTE_ORDER_TYPE: // B.c. object, handleArray.
        header.VP.T_O.size = handleArraySize ;
        header.VP.T_O.transport = transport ;
        header.VP.T_O.order = order ;
        header.bodySize = 1 ;
        break ;

        // --- TimeStruct, No Body ---
        // case REQUEST_FEDERATION_TIME:
      case QUERY_LBTS:
      case QUERY_FEDERATE_TIME:
      case TIME_ADVANCE_REQUEST:
      case NEXT_EVENT_REQUEST:
      case TIME_ADVANCE_GRANT:
        header.VP.time.date = getFederationTime() ;
        header.bodySize = 0 ;
        break ;


      case MODIFY_LOOKAHEAD:
      case QUERY_LOOKAHEAD:
        header.VP.time.date = lookahead ;
        header.bodySize = 0 ;
        break ;

      case ENABLE_TIME_REGULATION:
      case DISABLE_TIME_REGULATION:
      case ENABLE_TIME_CONSTRAINED:
      case DISABLE_TIME_CONSTRAINED:
      case TICK_REQUEST:
        header.VP.time.mode = boolean ;
        header.bodySize = 0 ;
        break ;

      case GET_OBJECT_INSTANCE_HANDLE:
      case GET_OBJECT_INSTANCE_NAME:
        header.VP.O_I.handle = object;
        header.VP.O_I.size = handleArraySize ;
        header.VP.O_I.date = getFederationTime() ;
        header.bodySize = 1 ;
        break ;

        // -- Default Handler --
      default:
        D.Out(pdExcept, "Unknown type %d in WriteHeader.", header.type);
        throw RTIinternalError("Message: Unknown type for Header.");

    }

    // 4- If Header.bodySize = 0, send message and return RTI_FALSE,
    // Else send nothing(will be done by WriteBody), and return RTI_TRUE.
    if (header.bodySize == 0)
        socket->send((const unsigned char *) &header, sizeof(MessageHeader));

    return header.bodySize != 0 ;
}

// ---------------------------------------------------------------
void
Message::writeResignAction(MessageBody &)
{
    //assert(false);
}

// ---------------------------------------------------------------------------
void
Message::writeValueArray(MessageBody &body)
{
    // length and value are stored into valueArray 
    // so we have to write length and then value with a writeBlock
    for (int i = 0 ; i < handleArraySize ; i ++)
        {
        body.writeLongInt(valueArray[i].length) ;
        body.writeBlock(valueArray[i].value, valueArray[i].length) ;
    }
}

} // namespace certi

// $Id: Message_RW.cc,v 3.33 2007/07/06 09:25:17 erk Exp $
