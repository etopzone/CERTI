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
// $Id: Message_W.cc,v 3.8 2007/11/05 14:30:04 rousse Exp $
// ----------------------------------------------------------------------------


#include "Message.hh"

#include <cassert>

using std::vector ;

namespace certi {

static pdCDebug D("RTIA_MSG","(LocalMESS) - ");
static PrettyDebug G("GENDOC",__FILE__);


// ----------------------------------------------------------------------------
//! Write NetworkMessage Objects to Socket objects.
void
Message::write(SocketUN *socket)
    throw (NetworkError, NetworkSignal)
{

    bool result ;
    result = writeHeader(socket);
    if (result)
        writeBody(socket);

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

          // Body contains federationName,FEDid
          case CREATE_FEDERATION_EXECUTION:
            body.writeString(federationName);
            body.writeString(FEDid);
            break ;

          // Body contains federationName
          case DESTROY_FEDERATION_EXECUTION:
            body.writeString(federationName);
            break ;

          // Body contains label,tag,boolean and maybe
          // handleArraySize,handleArray
          case REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
            body.writeString(label);
            body.writeString(tag);
            body.writeLongInt(boolean);
            // boolean true means federates set exists
            if ( boolean )
                {
                body.writeShortInt(handleArraySize);
                writeHandleArray(body);
                }
            break ;

          // RFS needs label and boolean in the body and time in the header
          case REQUEST_FEDERATION_SAVE:
            body.writeString(label);
            body.writeLongInt(boolean);
            break;

          // Body contains label
          case INITIATE_FEDERATE_SAVE:           
            body.writeString(label);
            break;

          // Body contains federate,label
          case INITIATE_FEDERATE_RESTORE:
            body.writeShortInt(federate);            
            body.writeString(label);
            break;

          // Body contains label,tag
          case ANNOUNCE_SYNCHRONIZATION_POINT:
          case REQUEST_FEDERATION_RESTORE_FAILED:
            body.writeString(label);
            body.writeString(tag);
            break ;

          // Body contains label (i.e. String)
          case SYNCHRONIZATION_POINT_REGISTRATION_FAILED:
          case SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
          case SYNCHRONIZATION_POINT_ACHIEVED:
          case FEDERATION_SYNCHRONIZED:
          case REQUEST_FEDERATION_RESTORE:
          case REQUEST_FEDERATION_RESTORE_SUCCEEDED:
            body.writeString(label);
            break ;

          // Body contains objectClass
          case UNPUBLISH_OBJECT_CLASS:
          case UNSUBSCRIBE_OBJECT_CLASS:
          case START_REGISTRATION_FOR_OBJECT_CLASS:
          case STOP_REGISTRATION_FOR_OBJECT_CLASS:
            body.writeLongInt(objectClass);
            break;

          // Body contains object,attribute,tag
          case IS_ATTRIBUTE_OWNED_BY_FEDERATE:
          case QUERY_ATTRIBUTE_OWNERSHIP:
            body.writeLongInt(object);
            body.writeShortInt(attribute);
            body.writeString(tag);
            break ;

          // Body contains object,attribute,federate
          case ATTRIBUTE_IS_NOT_OWNED:
          case INFORM_ATTRIBUTE_OWNERSHIP:
            body.writeLongInt(object);
            body.writeShortInt(attribute);
            body.writeShortInt(federate);
            break ;

          // Body contains object,handleArraySize,HandleArray,tag
          case NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
          case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
          case ATTRIBUTE_OWNERSHIP_ACQUISITION:
          case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
            body.writeLongInt(object);
            body.writeShortInt(handleArraySize);
            writeHandleArray(body);
            body.writeString(tag);
            break ;

          // Body contains object,handleArraySize,HandleArray
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

          // Body contains object,federationName,federate,handleArraySize,
          // handleArray
          case REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
	    body.writeLongInt(object);
            body.writeString(federationName);
            body.writeShortInt(federate);
            body.writeShortInt(handleArraySize);
            writeHandleArray(body);
	    break ;

          // Body contains object,region,boolean,handleArraySize,handleArray           
	  case DDM_ASSOCIATE_REGION:
	    body.writeLongInt(object);
	    body.writeLongInt(region);
	    body.writeLongInt(boolean);
            body.writeShortInt(handleArraySize);
            writeHandleArray(body);
	    break ;

          // Body contains objectClass,object,tag,handleArraySize,handleArray,
          // regions
	  case DDM_REGISTER_OBJECT:
	    body.writeLongInt(objectClass);
	    body.writeLongInt(object);
	    body.writeString(tag);
            body.writeShortInt(handleArraySize);
            writeHandleArray(body);
	    writeRegions(body);
	    break ;

          // Body contains objectClass,region,boolean,handleArraySize,
          // handleArray
	  case DDM_SUBSCRIBE_ATTRIBUTES:
	    body.writeLongInt(objectClass);
	    body.writeLongInt(region);
	    body.writeLongInt(boolean);
            body.writeShortInt(handleArraySize);
            writeHandleArray(body);
	    break ;

          // Body contains object,region
	  case DDM_UNASSOCIATE_REGION:
	    body.writeLongInt(object);
	    body.writeLongInt(region);
	    break ;

          // Body contains objectClass,region
	  case DDM_UNSUBSCRIBE_ATTRIBUTES:
	    body.writeLongInt(objectClass);
	    body.writeLongInt(region);
	    break ;

          // Body contains interactionClass,region,boolean	    
	  case DDM_SUBSCRIBE_INTERACTION:
	  case DDM_UNSUBSCRIBE_INTERACTION:
	    body.writeLongInt(interactionClass);
	    body.writeLongInt(region);
	    body.writeLongInt(boolean);
	    break ;

          // Body contains objectClass,attribute,space  
          case GET_ATTRIBUTE_SPACE_HANDLE:
            body.writeLongInt(objectClass);
            body.writeLongInt(attribute);
            body.writeLongInt(space);
            break ;

          // Body contains space,number,region
          case DDM_CREATE_REGION:
            body.writeLongInt(space) ;
            body.writeLongInt(number);
            body.writeLongInt(region);
            break ;

          // Body contains interactionClass,space
          case GET_INTERACTION_SPACE_HANDLE:
            body.writeLongInt(interactionClass);
            body.writeLongInt(space);
            break ;

          // Body contains federate,federationName,federateName
          // Note : federate relevant on RTIA answer only
          case JOIN_FEDERATION_EXECUTION:
            body.writeShortInt(federate);
            body.writeString(federationName);
            body.writeString(federateName);
            break ;

          // FederationTime yet put in header
          // Body contains objectClass,handleArraySize,HandleArray
          case PUBLISH_OBJECT_CLASS:
          case SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES:
            body.writeLongInt(objectClass);
            body.writeShortInt(handleArraySize);
            writeHandleArray(body);
            break ;

          // FederationTime yet put in header
          // Body contains objectClass,object,name
          case REGISTER_OBJECT_INSTANCE:
            body.writeLongInt(objectClass);
            body.writeLongInt(object);
            body.writeString(name);
            break ;

          // FederationTime (or zero) yet put in header
          // Body contains objectClass,handleArraySize,object,tag,HandleArray,
          // ValueArray,ResignAction and 
          // boolean (true with time, false without time)
          case UPDATE_ATTRIBUTE_VALUES:
          case REFLECT_ATTRIBUTE_VALUES:
            body.writeLongInt(objectClass);
            body.writeShortInt(handleArraySize);
            body.writeLongInt(object);
            body.writeString(tag);
            writeHandleArray(body);
            writeValueArray(body);
            writeResignAction(body);
            body.writeLongInt(boolean);
            break ;

          // FederationTime yet put in header
          // Body contains objectClass,object,tag,name,label,resignAction
          case DISCOVER_OBJECT_INSTANCE:
            body.writeLongInt(objectClass);
            body.writeLongInt(object);
            body.writeString(tag);
            body.writeString(name);
            body.writeString(label);
            writeResignAction(body);
            break ;

          // FederationTime yet put in header 
          // Body contains objectClass,object,tag,name,label,resignAction  
          case DELETE_OBJECT_INSTANCE:
          case REMOVE_OBJECT_INSTANCE:
            body.writeLongInt(objectClass);
            body.writeLongInt(object);
            body.writeString(tag);
            body.writeString(name);
            body.writeString(label);
            writeResignAction(body);
            break ;

          // Body contains object  
          case LOCAL_DELETE_OBJECT_INSTANCE:
            body.writeLongInt(object);
            break;

          // FederationTime yet put in header
          // Body contains objectClass,name,attribute
          case GET_OBJECT_CLASS_HANDLE:
          case GET_OBJECT_CLASS_NAME:
          case GET_ATTRIBUTE_HANDLE:
          case GET_ATTRIBUTE_NAME:
            body.writeLongInt(objectClass);
            body.writeString(name);
            body.writeShortInt(attribute);
            break ;

          // Body contains name,space
          case GET_SPACE_HANDLE:
          case GET_SPACE_NAME:
            body.writeString(name);
            body.writeLongInt(space);
            break ;

          // Body contains name,dimension,space
          case GET_DIMENSION_HANDLE:
          case GET_DIMENSION_NAME:
            body.writeString(name);
            body.writeLongInt(dimension);
            body.writeLongInt(space);
            break ;

          // Body contains interactionClass,handleArraySize,tag,handleArray,
          // valueArray,region,resignAction
          // and boolean (true with time, false without time)
          case SEND_INTERACTION:
          case RECEIVE_INTERACTION:
            body.writeLongInt(interactionClass);
            body.writeShortInt(handleArraySize);
            body.writeString(tag);
            writeHandleArray(body);
            writeValueArray(body);
	    body.writeLongInt(region);
            writeResignAction(body);
            body.writeLongInt(boolean);
            break ;

          // Body contains interactionClass,name,parameter
          case GET_INTERACTION_CLASS_HANDLE:
          case GET_INTERACTION_CLASS_NAME:
          case GET_PARAMETER_HANDLE:
          case GET_PARAMETER_NAME:
            body.writeLongInt(interactionClass);
            body.writeString(name);
            body.writeShortInt(parameter);
            break ;

          // Body contains handleArraySize,transport,order,object,HandleArray
          case CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE:
          case CHANGE_ATTRIBUTE_ORDER_TYPE:
            body.writeShortInt(handleArraySize);
            body.writeLongInt(transport);
            body.writeLongInt(order);           
            body.writeLongInt(object);
            writeHandleArray(body);
            break ;

          // Body contains interactionClass,transport,order
          case CHANGE_INTERACTION_TRANSPORTATION_TYPE:
          case CHANGE_INTERACTION_ORDER_TYPE:
            body.writeLongInt(interactionClass);
            body.writeLongInt(transport);
            body.writeLongInt(order);  
            break ;

          // Body contains region,extents            
	  case DDM_MODIFY_REGION:
            body.writeLongInt(region);
	    writeExtents(body);
	    break ;

          // Body contains region            
	  case DDM_DELETE_REGION:
            body.writeLongInt(region);
	    break ;

          // Body contains object,name	    
	  case GET_OBJECT_INSTANCE_HANDLE:
	  case GET_OBJECT_INSTANCE_NAME :
	    body.writeLongInt(object);
	    body.writeString(name);
	    break;

          // Body contains ResignAction
          case RESIGN_FEDERATION_EXECUTION:
            writeResignAction(body);
            break;

          // Body contains interactionClass
          case PUBLISH_INTERACTION_CLASS:
          case UNPUBLISH_INTERACTION_CLASS:
          case SUBSCRIBE_INTERACTION_CLASS:
          case UNSUBSCRIBE_INTERACTION_CLASS:
          case TURN_INTERACTIONS_ON:
          case TURN_INTERACTIONS_OFF:
            body.writeLongInt(interactionClass);
            break ;

          // Body contains boolean
          case ENABLE_TIME_REGULATION:
          case DISABLE_TIME_REGULATION:
          case ENABLE_TIME_CONSTRAINED:
          case DISABLE_TIME_CONSTRAINED:
          case TICK_REQUEST:
            body.writeLongInt(boolean);
            break ;
	    
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

    // 2- Fill Header
    // header contains type,exception and federationTime
    // Note sometimes federationTime is not useful.
    header.type = this->type ;
    header.exception = this->exception ;
    header.date = getFederationTime();

    // If the message carry an exception, the Body will only contain the
    // exception reason.

    if (exception != e_NO_EXCEPTION) {
        header.bodySize = 1 ;
        return true ;
    }

    // 3- Determine if body exists or not
    // Note: Header.bodySize is not set to the actual Body size, but
    // to zero to indicate there is no Body, or 1 if a Body is needed.

    switch(this->type) {

      // ------------------ BODY NOT EMPTY -----------------------------------

      // ------- federationTime not useful
      case CREATE_FEDERATION_EXECUTION:
      case DESTROY_FEDERATION_EXECUTION:
      case REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
      case SYNCHRONIZATION_POINT_REGISTRATION_FAILED:
      case SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
      case ANNOUNCE_SYNCHRONIZATION_POINT:
      case SYNCHRONIZATION_POINT_ACHIEVED:
      case FEDERATION_SYNCHRONIZED:
      case IS_ATTRIBUTE_OWNED_BY_FEDERATE:
      case QUERY_ATTRIBUTE_OWNERSHIP:
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
      case INITIATE_FEDERATE_SAVE:
      case DDM_CREATE_REGION:
      case DDM_ASSOCIATE_REGION:
      case DDM_UNASSOCIATE_REGION:
      case DDM_SUBSCRIBE_ATTRIBUTES:
      case DDM_UNSUBSCRIBE_ATTRIBUTES:
      case DDM_SUBSCRIBE_INTERACTION:
      case DDM_UNSUBSCRIBE_INTERACTION:
      case DDM_REGISTER_OBJECT:
      case REQUEST_FEDERATION_RESTORE:
      case REQUEST_FEDERATION_RESTORE_SUCCEEDED:
      case REQUEST_FEDERATION_RESTORE_FAILED:
      case REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
      case RESIGN_FEDERATION_EXECUTION:
      case INITIATE_FEDERATE_RESTORE:
      case JOIN_FEDERATION_EXECUTION:
      case UNPUBLISH_OBJECT_CLASS:
      case UNSUBSCRIBE_OBJECT_CLASS:
      case START_REGISTRATION_FOR_OBJECT_CLASS:
      case STOP_REGISTRATION_FOR_OBJECT_CLASS:
      case PUBLISH_INTERACTION_CLASS:
      case UNPUBLISH_INTERACTION_CLASS:
      case SUBSCRIBE_INTERACTION_CLASS:
      case UNSUBSCRIBE_INTERACTION_CLASS:
      case TURN_INTERACTIONS_ON:
      case TURN_INTERACTIONS_OFF:
      case LOCAL_DELETE_OBJECT_INSTANCE:
      case CHANGE_INTERACTION_TRANSPORTATION_TYPE:
      case CHANGE_INTERACTION_ORDER_TYPE:
      case DDM_DELETE_REGION:
      case DDM_MODIFY_REGION:
      case CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE:
      case CHANGE_ATTRIBUTE_ORDER_TYPE:
      case ENABLE_TIME_REGULATION:
      case DISABLE_TIME_REGULATION:
      case ENABLE_TIME_CONSTRAINED:
      case DISABLE_TIME_CONSTRAINED:
      case TICK_REQUEST:
        header.bodySize = 1 ;
        break ;

      // ------- federationTime (relevant or zero) depending on boolean value
      //         i.e. boolean false don't need date.
      case REQUEST_FEDERATION_SAVE:
      case UPDATE_ATTRIBUTE_VALUES:
      case REFLECT_ATTRIBUTE_VALUES:
      case SEND_INTERACTION:
      case RECEIVE_INTERACTION:
        if ( ! boolean )
          {
          header.date = 0 ;
          }
        header.bodySize = 1 ;
        break ;

      // ------- federationTime useful
      case PUBLISH_OBJECT_CLASS:
      case SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES:
      case REGISTER_OBJECT_INSTANCE:
      case DELETE_OBJECT_INSTANCE:
      case REMOVE_OBJECT_INSTANCE:
      case DISCOVER_OBJECT_INSTANCE:
      case GET_OBJECT_CLASS_HANDLE:
      case GET_OBJECT_CLASS_NAME:
      case GET_ATTRIBUTE_HANDLE:
      case GET_ATTRIBUTE_NAME:
      case GET_INTERACTION_CLASS_HANDLE:
      case GET_INTERACTION_CLASS_NAME:
      case GET_PARAMETER_HANDLE:
      case GET_PARAMETER_NAME:
      case GET_SPACE_HANDLE:
      case GET_SPACE_NAME:
      case GET_DIMENSION_HANDLE:
      case GET_DIMENSION_NAME:
      case GET_OBJECT_INSTANCE_HANDLE:
      case GET_OBJECT_INSTANCE_NAME:
        header.bodySize = 1 ;
        break ;
 
      // --------------------- BODY EMPTY ----------------------------
      // Header yet contains type,exception,size and federationTime.

      // ------- federationTime not useful       
      case FEDERATE_SAVE_BEGUN:
      case FEDERATE_SAVE_COMPLETE:
      case FEDERATE_SAVE_NOT_COMPLETE:
      case FEDERATION_SAVED:
      case FEDERATION_NOT_SAVED:
      case FEDERATE_RESTORE_COMPLETE:
      case FEDERATE_RESTORE_NOT_COMPLETE:
      case FEDERATION_RESTORED:
      case FEDERATION_NOT_RESTORED:
      case FEDERATION_RESTORE_BEGUN:
      case ENABLE_ASYNCHRONOUS_DELIVERY:
      case DISABLE_ASYNCHRONOUS_DELIVERY:
        header.bodySize = 0 ;
        break ;

      // ------- federationTime useful
      case QUERY_LBTS:
      case QUERY_FEDERATE_TIME:
      case TIME_ADVANCE_REQUEST:
      case NEXT_EVENT_REQUEST:
      case TIME_ADVANCE_GRANT:
        header.bodySize = 0 ;
        break ;

      // federationTime yet put in header
      // We store lookahead in place
      case MODIFY_LOOKAHEAD:
      case QUERY_LOOKAHEAD:
        header.date = lookahead ;
        header.bodySize = 0 ;
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

// $Id: Message_W.cc,v 3.8 2007/11/05 14:30:04 rousse Exp $
