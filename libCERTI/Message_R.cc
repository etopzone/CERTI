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
// $Id: Message_R.cc,v 3.3 2007/10/30 09:25:55 rousse Exp $
// ----------------------------------------------------------------------------


#include "Message.hh"

#include <cassert>

using std::vector ;

namespace certi {

static pdCDebug D("RTIA_MSG","(LocalMESS) - ");
static PrettyDebug G("GENDOC",__FILE__);

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

          // Body contains FederationName, FEDid
          case CREATE_FEDERATION_EXECUTION:
            readFederationName(body);
            readFEDid(body) ;
            break ;

          case DESTROY_FEDERATION_EXECUTION:
            readFederationName(body);
            break ;

          // Body contains label,tag,boolean and maybe
          // handleArraySize,handleArray
          case REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
            readLabel(body);
            readTag(body);
            boolean = body.readLongInt();
            // boolean true means federates set exists
            if ( boolean )
                {
                handleArraySize = body.readShortInt();
                readHandleArray(body);
                }
            break ;

          // Body contains label,tag
          case ANNOUNCE_SYNCHRONIZATION_POINT:
          case REQUEST_FEDERATION_RESTORE_FAILED:
            readLabel(body);
            readTag(body);
            break ;

          // Body contains label
          case SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
          case SYNCHRONIZATION_POINT_ACHIEVED:
          case FEDERATION_SYNCHRONIZED:
          case REQUEST_FEDERATION_RESTORE:
          case REQUEST_FEDERATION_RESTORE_SUCCEEDED:
            readLabel(body);
            break ;

          // Body contains federate,label
          case INITIATE_FEDERATE_RESTORE:
            federate = body.readShortInt();
            readLabel(body);
            break ;

          // Body contains label
          case INITIATE_FEDERATE_SAVE:
            readLabel(body);
            break ;

          // Body contains label, boolean
          case REQUEST_FEDERATION_SAVE:
            readLabel(body);
            // boolean true means with time (in the header)
            boolean = body.readLongInt();
            break ;

          // Body contains objectClass
          case UNPUBLISH_OBJECT_CLASS:
          case UNSUBSCRIBE_OBJECT_CLASS:
          case START_REGISTRATION_FOR_OBJECT_CLASS:
          case STOP_REGISTRATION_FOR_OBJECT_CLASS:
            objectClass = body.readLongInt();
            break;

          // Body contains object,attribute,tag
          case IS_ATTRIBUTE_OWNED_BY_FEDERATE:
          case QUERY_ATTRIBUTE_OWNERSHIP:
            object = body.readLongInt();
            attribute = body.readShortInt();
            readTag(body);
            break ;

          // Body contains object,attribute,federate
          case ATTRIBUTE_IS_NOT_OWNED:
          case INFORM_ATTRIBUTE_OWNERSHIP:
            object = body.readLongInt();
            attribute = body.readShortInt();
            federate = body.readShortInt();
            break ;

          // Body contains object,handleArraySize,HandleArray,tag
          case NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
          case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
          case ATTRIBUTE_OWNERSHIP_ACQUISITION:
          case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
            object = body.readLongInt();
            handleArraySize = body.readShortInt();
            readHandleArray(body);
            readTag(body);
            break ;

          // Body contains object,handleArraySize,HandleArray
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

          // Body contains object,region,boolean,handleArraySize,handleArray
	  case DDM_ASSOCIATE_REGION:
	    object = body.readLongInt();
	    region = body.readLongInt();
	    boolean = body.readLongInt();
	    handleArraySize = body.readShortInt();
            readHandleArray(body);
	    break ;

          // Body contains objectClass,object,tag,handleArraySize,handleArray,
          // regions
	  case DDM_REGISTER_OBJECT:
	    objectClass = body.readLongInt();
	    object = body.readLongInt();
	    readTag(body);
	    handleArraySize = body.readShortInt();
            readHandleArray(body);
	    readRegions(body);
	    break ;

          // Body contains objectClass,region,boolean,handleArraySize,
          // handleArray
	  case DDM_SUBSCRIBE_ATTRIBUTES:
	    objectClass = body.readLongInt();
	    region = body.readLongInt();
	    boolean = body.readLongInt();
	    handleArraySize = body.readShortInt();
            readHandleArray(body);
	    break ;

          // Body contains object,region
	  case DDM_UNASSOCIATE_REGION:
	    object = body.readLongInt();
	    region = body.readLongInt();
	    break ;

          // Body contains objectClass,region
	  case DDM_UNSUBSCRIBE_ATTRIBUTES:	    
	    objectClass = body.readLongInt();
	    region = body.readLongInt();
	    break ;

          // Body contains interactionClass,region,boolean
	  case DDM_SUBSCRIBE_INTERACTION:
	  case DDM_UNSUBSCRIBE_INTERACTION:
	    interactionClass = body.readLongInt();
	    region = body.readLongInt();
	    boolean = body.readLongInt();
	    break ;

          // Body contains objectClass,attribute,space	    
          case GET_ATTRIBUTE_SPACE_HANDLE:
            objectClass = body.readLongInt();
            attribute = body.readLongInt();
            space = body.readLongInt();
            break ;

          // Body contains space,number,region
          case DDM_CREATE_REGION:
            space  = body.readLongInt();
            number = body.readLongInt();
            region = body.readLongInt();
            break ;

          // Body contains interactionClass,space
          case GET_INTERACTION_SPACE_HANDLE:
            interactionClass = body.readLongInt();
            space = body.readLongInt();
            break ;

            // --- MessageJ_R_Struct --

          // Body contains federate,Federationname,FederateName
          case JOIN_FEDERATION_EXECUTION:
            federate = body.readShortInt();
            readFederationName(body);
            readFederateName(body);
            break ;

            // --- MessageO_I_Struct ---

          // federationTime got from header
          // Body contains objectClass,handleArraySize,HandleArray
          case PUBLISH_OBJECT_CLASS:
          case SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES:
            objectClass = body.readLongInt();
            handleArraySize = body.readShortInt();
            readHandleArray(body);
            break ;

          // federationTime got from header
          // Body contains objectClass,object,name
          case REGISTER_OBJECT_INSTANCE:
            objectClass = body.readLongInt();
            object = body.readLongInt();
            readName(body);
            break ;

          // FederationTime (or zero) yet got from header
          // Body contains objectClass,handleArraySize,object,tag,HandleArray,
          // ValueArray,ResignAction and 
          // boolean (true with time, false without time)
          case UPDATE_ATTRIBUTE_VALUES:
          case REFLECT_ATTRIBUTE_VALUES:
            objectClass = body.readLongInt();
            handleArraySize = body.readShortInt();
            object = body.readLongInt();
            readTag(body);
            readHandleArray(body);
            readValueArray(body);
            readResignAction(body);
            boolean = body.readLongInt();
            break ;

          // FederationTime yet got from header
          // Body contains objectClass,object,tag,name,label,resignAction
          case DISCOVER_OBJECT_INSTANCE:
            objectClass = body.readLongInt();
            object = body.readLongInt();
            readTag(body);
            readName(body);
            readLabel(body);
            readResignAction(body);
            break ;

          // FederationTime yet put in header 
          // Body contains objectClass,object,tag,name,label,resignAction  
          case DELETE_OBJECT_INSTANCE:
          case REMOVE_OBJECT_INSTANCE:
            objectClass = body.readLongInt();
            object = body.readLongInt();
            readTag(body);
            readName(body);
            readLabel(body);
            readResignAction(body);
            break ;

          // Body contains object 
          case LOCAL_DELETE_OBJECT_INSTANCE:
            break ;

          // FederationTime yet got from header
          // Body contains objectClass,name,attribute
          case GET_OBJECT_CLASS_HANDLE:
          case GET_OBJECT_CLASS_NAME:
          case GET_ATTRIBUTE_HANDLE:
          case GET_ATTRIBUTE_NAME:
            objectClass = body.readLongInt();
            readName(body);
            attribute = body.readShortInt();
            break ;

          // Body contains name,space
          case GET_SPACE_HANDLE:
          case GET_SPACE_NAME:
            this->readName(body);
            this->space = body.readLongInt();
            break ;

          // Body contains name,dimension,space
          case GET_DIMENSION_HANDLE:
          case GET_DIMENSION_NAME:
            this->readName(body);
            this->dimension = body.readLongInt();
            this->space = body.readLongInt();
            break ;

          case SEND_INTERACTION:
          case RECEIVE_INTERACTION:
            // B.c. Tag, HandleArray[], ValueArray[], RAction
            // and boolean (true means SI with time, false without time)
            interactionClass = body.readLongInt();
            handleArraySize = body.readShortInt();
            readTag(body);
            readHandleArray(body);
            readValueArray(body);
	    region = body.readLongInt();
            readResignAction(body);
            boolean = body.readLongInt();
            break ;

          // Body contains interactionClass,name,parameter
          case GET_INTERACTION_CLASS_HANDLE:
          case GET_INTERACTION_CLASS_NAME:
          case GET_PARAMETER_HANDLE:
          case GET_PARAMETER_NAME:
            interactionClass = body.readLongInt();
            readName(body);
            parameter = body.readShortInt();
            break ;

            // --- MessageT_O_Struct, body not empty ---
          // Body contains handleArraySize,transport,order,object,HandleArray
          case CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE:
          case CHANGE_ATTRIBUTE_ORDER_TYPE:
            handleArraySize = body.readShortInt();
            transport = body.readLongInt();
            order = body.readLongInt();
            object = body.readLongInt();
            readHandleArray(body);
            break ;

          // Body contains interactionClass,transport,order
          case CHANGE_INTERACTION_TRANSPORTATION_TYPE:
          case CHANGE_INTERACTION_ORDER_TYPE:
            interactionClass = body.readLongInt();
            transport = body.readLongInt();
            order = body.readLongInt();
            break;

          // Body contains region,extents
	  case DDM_MODIFY_REGION:
            region = body.readLongInt();
	    readExtents(body);
	    break ;

          // Body contains region
	  case DDM_DELETE_REGION:
            region = body.readLongInt();
	    break ;

          // Body contains object,name
	  case GET_OBJECT_INSTANCE_HANDLE:
	  case GET_OBJECT_INSTANCE_NAME:
            object = body.readLongInt();
	    readName(body);
	    break;

          // Body contains resignAction
          case RESIGN_FEDERATION_EXECUTION:
            readResignAction(body);
            break;

          // Body contains interactionClass
          case PUBLISH_INTERACTION_CLASS:
          case UNPUBLISH_INTERACTION_CLASS:
          case SUBSCRIBE_INTERACTION_CLASS:
          case UNSUBSCRIBE_INTERACTION_CLASS:
          case TURN_INTERACTIONS_ON:
          case TURN_INTERACTIONS_OFF:
            interactionClass = body.readLongInt();
            break ;

          // Body contains boolean
          case ENABLE_TIME_REGULATION:
          case DISABLE_TIME_REGULATION:
          case ENABLE_TIME_CONSTRAINED:
          case DISABLE_TIME_CONSTRAINED:
          case TICK_REQUEST:
            boolean = body.readLongInt();
            break ;

          // Body contains object,federationName,federate,handleArraySize,
          // handleArray
          case REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
            object = body.readLongInt();
            readFederationName(body);
            federate = body.readShortInt();
            handleArraySize = body.readShortInt();
            readHandleArray(body);
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
    setFederationTime(header.date);

    // If the message carry an exception, the Body will only contain the
    // exception reason.

    if (exception != e_NO_EXCEPTION)
        return true ;

    // 2- Parse Header according to its type(Variable Part)
    // NULL, UAV and SendInteraction are the most common ones.

    switch(type) {

        // --- No Variable Part, Body not empty ---
      // header contains type, exception, size and no more useful
      case CREATE_FEDERATION_EXECUTION:  // Body contains FederationName,FEDid
      case DESTROY_FEDERATION_EXECUTION: // Body contains FederationName
      case REGISTER_FEDERATION_SYNCHRONIZATION_POINT: // Body contains Label
                                                      // tag,boolean and maybe
                                                      // handleArraySize,
                                                      // handleArray
      case SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED: // Body contains label
      case ANNOUNCE_SYNCHRONIZATION_POINT:           // Body contains label,tag
      case SYNCHRONIZATION_POINT_ACHIEVED:               // Body contains label
      case FEDERATION_SYNCHRONIZED:                      // Body contains label
      case INITIATE_FEDERATE_SAVE:                       // Body contains label
      case IS_ATTRIBUTE_OWNED_BY_FEDERATE:// B.c. object, attribute and Tag.
      case QUERY_ATTRIBUTE_OWNERSHIP: // B.c. object and attribute.
      case ATTRIBUTE_IS_NOT_OWNED:    //Body contains object,attribute,federate
      case INFORM_ATTRIBUTE_OWNERSHIP://Body contains object,attribute,federate
      case NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE: // Body contains object,
                                                       // handleArraySize,
                                                       // handleArray
      case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
      case ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:// Body contains object,
                                                        // handleArraySize,
                                                        // handleArray
      case ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:// Body contains object
                                                        // handleArraySize,
                                                        // handleArray
      case ATTRIBUTE_OWNERSHIP_UNAVAILABLE:// Body contains object
                                                        // handleArraySize,
                                                        // handleArray
      case UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
      case ATTRIBUTE_OWNERSHIP_ACQUISITION:
      case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
      case ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:// Body contains object
                                                        // handleArraySize,
                                                        // handleArray
      // Body contains object,handleArraySize,handleArray 
      case CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
      case ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:// Body contains object
                                                        // handleArraySize,
                                                        // handleArray 
      case CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:// Body contains object
                                                        // handleArraySize,
                                                        // handleArray
      // Body contains object,handleArraySize,handleArray 
      case CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
      // Body contains objectClass,attribute,space
      case GET_ATTRIBUTE_SPACE_HANDLE:
      case GET_INTERACTION_SPACE_HANDLE: //Body contains interactionClass,space
      case DDM_CREATE_REGION:              // Body contains space,number,region
      case REQUEST_FEDERATION_RESTORE:           // Body contains label.
      case REQUEST_FEDERATION_RESTORE_SUCCEEDED: // Body contains label.
      case REQUEST_FEDERATION_RESTORE_FAILED:    // Body contains label,tag
      // Body contains object,region,boolean,handleArraySize,handleArray
      case DDM_ASSOCIATE_REGION:
      case DDM_UNASSOCIATE_REGION:               // Body contains object,region
      // Body contains objectClass,region,boolean,handleArraySize,handleArray
      case DDM_SUBSCRIBE_ATTRIBUTES:
      case DDM_UNSUBSCRIBE_ATTRIBUTES:      // Body contains objectClass,region
      // Body contains interactionClass,region,boolean
      case DDM_SUBSCRIBE_INTERACTION:
      // Body contains interactionClass,region,boolean
      case DDM_UNSUBSCRIBE_INTERACTION:
      // Body contains objectClass,object,tag,handleArraySize,handleArray,
      // regions
      case DDM_REGISTER_OBJECT:
        break ;

        // --- MessageJ_R_Struct --

      // header contains type, exception, size and no more useful
      // Body contains resignAction
      case RESIGN_FEDERATION_EXECUTION:
        break ;

      // header contains type, exception, size and no more useful
      // Body contains federate,label
      case INITIATE_FEDERATE_RESTORE:
        break ;

      // header contains type, exception, size and no more useful
      case JOIN_FEDERATION_EXECUTION: // Body contains federate,FederationName
        break ;                       //               federateName

        // --- MessageO_I_Struct, No body ---

      // header contains type, exception, size and no more useful
      // Body contains objectClass
      case UNPUBLISH_OBJECT_CLASS:
      case UNSUBSCRIBE_OBJECT_CLASS:
      case START_REGISTRATION_FOR_OBJECT_CLASS:
      case STOP_REGISTRATION_FOR_OBJECT_CLASS:
        break ;

      // header contains type, exception, size and no more useful
      // Body contains interactionClass
      case PUBLISH_INTERACTION_CLASS:
      case UNPUBLISH_INTERACTION_CLASS:
      case SUBSCRIBE_INTERACTION_CLASS:
      case UNSUBSCRIBE_INTERACTION_CLASS:
      case TURN_INTERACTIONS_ON:
      case TURN_INTERACTIONS_OFF:
        break;

        // --- MessageO_I_Struct, body not Empty ---

      // RFS without time will receive date=0 in the header
      case REQUEST_FEDERATION_SAVE:
        break ;

      // FederationTime yet got from header
      // Body contains objectClass,handleArraySize,HandleArray
      case PUBLISH_OBJECT_CLASS:
      case SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES:
        break ;

      // FederationTime yet got from header
      // Body contains objectClass,object,name
      case REGISTER_OBJECT_INSTANCE:
        break ;

      // FederationTime yet got from header 
      // Body contains objectClass,object,tag,name,label,resignAction  
      case DELETE_OBJECT_INSTANCE:
      case REMOVE_OBJECT_INSTANCE:
        break ;

      // Body contains object  
      case LOCAL_DELETE_OBJECT_INSTANCE:
        break;

      // FederationTime yet got from header
      // Body contains objectClass,object,tag,name,label,resignAction
      case DISCOVER_OBJECT_INSTANCE:
        break ;

      // FederationTime yet got from header
      // Body contains objectClass,name,attribute
      case GET_OBJECT_CLASS_HANDLE:
      case GET_OBJECT_CLASS_NAME:
      case GET_ATTRIBUTE_HANDLE:
      case GET_ATTRIBUTE_NAME:
        break ;

      // FederationTime (maybe zero) yet got from header
      // Body contains objectClass,handleArraySize,object,tag,HandleArray,
      // ValueArray,ResignAction and 
      // boolean (true with time, false without time)
      case UPDATE_ATTRIBUTE_VALUES:
      case REFLECT_ATTRIBUTE_VALUES:
        break ;

      // FederationTime yet got from header
      case SEND_INTERACTION: // B.c. Tag, HandleArray[], ValueArray[]
      case RECEIVE_INTERACTION: // B.c. Tag, HandleArray[], ValueArray[], resignAction
        // interactionClass = header.VP.O_I.handle ;
        // handleArraySize = header.VP.O_I.size ;
        // setFederationTime(header.VP.O_I.date );
        break ;

      // FederationTime yet got from header
      // Body contains interactionClass,name,parameter
      case GET_INTERACTION_CLASS_HANDLE:
      case GET_INTERACTION_CLASS_NAME:
      case GET_PARAMETER_HANDLE:
      case GET_PARAMETER_NAME:
        break ;

      // FederationTime yet got from header
      // Body contains name,space
      case GET_SPACE_HANDLE:
      case GET_SPACE_NAME:
        break ;

      // FederationTime yet got from header
      // Body contains name,dimension,space
      case GET_DIMENSION_NAME:
      case GET_DIMENSION_HANDLE:
        break ;

      // Body contains interactionClass,transport,order
      case CHANGE_INTERACTION_TRANSPORTATION_TYPE:
      case CHANGE_INTERACTION_ORDER_TYPE:
        break ;

      // Body contains region
      case DDM_DELETE_REGION:
        //region = header.VP.ddm.region ;
        break ;

      // Body contains region,extents
      case DDM_MODIFY_REGION:
	break ;	
	
        // --- MessageT_O_Struct, Body not empty ---
      // Body contains handleArraySize,transport,order,object,HandleArray
      case CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE:
      case CHANGE_ATTRIBUTE_ORDER_TYPE:
        break ;

        // --- TimeStruct, No Body ---
        // case REQUEST_FEDERATION_TIME:

      // FederationTime yet got from header
      // Body is empty
      case QUERY_LBTS:
      case QUERY_FEDERATE_TIME:
      case TIME_ADVANCE_REQUEST:
      case NEXT_EVENT_REQUEST:
      case TIME_ADVANCE_GRANT:
        break ;

      // lookahead got from header
      // Body is empty
      // Warning : FederationTime has been modified (needs validation)
      case MODIFY_LOOKAHEAD:
      case QUERY_LOOKAHEAD:
        lookahead = header.date ;
        fed_time.setZero();
        break ;

      // Body contains boolean
      case ENABLE_TIME_REGULATION:
      case DISABLE_TIME_REGULATION:
      case ENABLE_TIME_CONSTRAINED:
      case DISABLE_TIME_CONSTRAINED:
      case TICK_REQUEST:
        break ;


      // FederationTime yet got from header
      // Body contains object,name
      case GET_OBJECT_INSTANCE_HANDLE:
      case GET_OBJECT_INSTANCE_NAME:
        break ;

      // Body contains object,federationName,federate,handleArraySize,
      // handleArray
      case REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
        break;        


      // Nothing more to do. Body is empty.
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
        break;

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

} // namespace certi

// $Id: Message_R.cc,v 3.3 2007/10/30 09:25:55 rousse Exp $
