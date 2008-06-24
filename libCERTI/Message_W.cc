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
// $Id: Message_W.cc,v 3.34 2008/06/24 08:56:49 rousse Exp $
// ----------------------------------------------------------------------------


#include "Message.hh"

#include <cassert>

using std::vector ;

namespace certi {

static PrettyDebug D("RTIA_MSG","Message::");
static PrettyDebug G("GENDOC",__FILE__);

//-----------------------------------------------------------------------------
void
Message::send(SocketUN *socket, MessageBuffer &msgBuffer) throw (NetworkError, NetworkSignal) {
	G.Out(pdGendoc,"enter Message::send");
	/* 0- reset send buffer */
	msgBuffer.reset();
	/* 1- serialize the message
	 * This is a polymorphic call 
	 * which may specialized in a daughter class  
	 */
	serialize(msgBuffer);
	/* 2- update message buffer 'reserved bytes' header */
	msgBuffer.updateReservedBytes();
	D.Out(pdDebug,"Sending <%s> whose buffer has <%u> bytes",getName().c_str(),msgBuffer.size());
	//msgBuf.show(msgBuffer(0),5);
	/* 3- effectively send the raw message to socket */
	socket->send(static_cast<unsigned char*>(msgBuffer(0)), msgBuffer.size());
	G.Out(pdGendoc,"exit  Message::send");
} /* end of send */

// ----------------------------------------------------------------------------
void Message::serialize(MessageBuffer& msgBuffer) {
	G.Out(pdGendoc,"enter Message::serialize");
	/* We serialize the common messages part 
	 * ALL Message will contain the following
	 */	
	if ((type==NOT_USED) || (type==LAST)) {
		throw RTIinternalError("Invalid message type (not a valid type);");
	}
	D.Out(pdDebug, "Serialize <%s>", getName().c_str());
        writeHeader(msgBuffer);
        writeBody(msgBuffer);
	/*
	 * "builtin" Optional part
	 * The subclass may chose in the constructor the variable part.
	 * isDated may be chosen on Message instance basis
	 * (same message may Dated or Not Dated) 
	 */
	G.Out(pdGendoc,"exit Message::serialize");
} /* end of serialize */


// ----------------------------------------------------------------------------
/*! Prepare and write a Body to a socket, should be called after WriteHeader.
  The message is written onto the socket by WriteHeader if no body is
  required, or by WriteBody is a body has been required by WriteHeader.
*/
void
Message::writeBody(MessageBuffer &msgBuffer)
{
    G.Out(pdGendoc,"enter Message::writeBody type %d",type);    

    // If the message carry an exception, the Body will only contain the
    // exception reason.

    if (exception != e_NO_EXCEPTION) {
        msgBuffer.write_string(exceptionReason);
    }
    else {

        // 1- Prepare Body Structure according to Message type
        switch(type) {

          // Body contains federationName,FEDid
          case CREATE_FEDERATION_EXECUTION:
            writeFederationName(msgBuffer);
            writeFEDid(msgBuffer); 
            break ;

          // Body contains federationName
          case DESTROY_FEDERATION_EXECUTION:
            writeFederationName(msgBuffer);
            break ;

          // Body contains label,tag,boolean and maybe
          // handleArraySize,handleArray
          case REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
            msgBuffer.write_string(label);
            msgBuffer.write_string(tag);
            msgBuffer.write_bool(boolean);
            // boolean true means federates set exists
            if ( boolean )
                {
                msgBuffer.write_int16(handleArraySize);
                writeHandleArray(msgBuffer);
                }
            break ;

          // RFS needs label and boolean in the body and time in the header
          case REQUEST_FEDERATION_SAVE:
            msgBuffer.write_string(label);
            msgBuffer.write_bool(boolean);
            break;

          // Body contains label
          case INITIATE_FEDERATE_SAVE:           
            msgBuffer.write_string(label);
            break;

          // Body contains federate,label
          case INITIATE_FEDERATE_RESTORE:
            msgBuffer.write_int16(federate);            
            msgBuffer.write_string(label);
            break;

          // Body contains label,tag
          case ANNOUNCE_SYNCHRONIZATION_POINT:
          case REQUEST_FEDERATION_RESTORE_FAILED:
            msgBuffer.write_string(label);
            msgBuffer.write_string(tag);
            break ;

          // Body contains label (i.e. String)
          case SYNCHRONIZATION_POINT_REGISTRATION_FAILED:
          case SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
          case SYNCHRONIZATION_POINT_ACHIEVED:
          case FEDERATION_SYNCHRONIZED:
          case REQUEST_FEDERATION_RESTORE:
          case REQUEST_FEDERATION_RESTORE_SUCCEEDED:
            msgBuffer.write_string(label);
            break ;

          // Body contains objectClass
          case UNPUBLISH_OBJECT_CLASS:
          case UNSUBSCRIBE_OBJECT_CLASS:
          case START_REGISTRATION_FOR_OBJECT_CLASS:
          case STOP_REGISTRATION_FOR_OBJECT_CLASS:
            msgBuffer.write_int64(objectClass);
            break;

          // Body contains object,attribute,tag
          case IS_ATTRIBUTE_OWNED_BY_FEDERATE:
          case QUERY_ATTRIBUTE_OWNERSHIP:
            msgBuffer.write_int64(object);
            msgBuffer.write_int16(attribute);
            msgBuffer.write_string(tag);
            break ;

          // Body contains object,attribute,federate
          case ATTRIBUTE_IS_NOT_OWNED:
          case INFORM_ATTRIBUTE_OWNERSHIP:
            msgBuffer.write_int64(object);
            msgBuffer.write_int16(attribute);
            msgBuffer.write_int16(federate);
            break ;

          // Body contains object,handleArraySize,HandleArray,tag
          case NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
          case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
          case ATTRIBUTE_OWNERSHIP_ACQUISITION:
          case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
            msgBuffer.write_int64(object);
            msgBuffer.write_int16(handleArraySize);
            writeHandleArray(msgBuffer);
            msgBuffer.write_string(tag);
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
            msgBuffer.write_int64(object);
            msgBuffer.write_int16(handleArraySize);
            writeHandleArray(msgBuffer);
            break ;

          // Body contains object,handleArraySize,
          // handleArray
          case REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
	    msgBuffer.write_int64(object);
            msgBuffer.write_int16(handleArraySize);
            writeHandleArray(msgBuffer);
	    break ;

          // Body contains object,handleArraySize,
          // handleArray
          case PROVIDE_ATTRIBUTE_VALUE_UPDATE:
	    msgBuffer.write_int64(object);
            msgBuffer.write_int16(handleArraySize);
            writeHandleArray(msgBuffer);
	    break ;


          // Body contains object,region,boolean,handleArraySize,handleArray           
	  case DDM_ASSOCIATE_REGION:
	    msgBuffer.write_int64(object);
	    msgBuffer.write_int64(region);
	    msgBuffer.write_bool(boolean);
            msgBuffer.write_int16(handleArraySize);
            writeHandleArray(msgBuffer);
	    break ;

          // Body contains objectClass,object,tag,handleArraySize,handleArray,
          // regions
	  case DDM_REGISTER_OBJECT:
	    msgBuffer.write_int64(objectClass);
	    msgBuffer.write_int64(object);
	    msgBuffer.write_string(tag);
            msgBuffer.write_int16(handleArraySize);
            writeHandleArray(msgBuffer);
	    writeRegions(msgBuffer);
	    break ;

          // Body contains objectClass,region,boolean,handleArraySize,
          // handleArray
	  case DDM_SUBSCRIBE_ATTRIBUTES:
	    msgBuffer.write_int64(objectClass);
	    msgBuffer.write_int64(region);
	    msgBuffer.write_bool(boolean);
            msgBuffer.write_int16(handleArraySize);
            writeHandleArray(msgBuffer);
	    break ;

          // Body contains object,region
	  case DDM_UNASSOCIATE_REGION:
	    msgBuffer.write_int64(object);
	    msgBuffer.write_int64(region);
	    break ;

          // Body contains objectClass,region
	  case DDM_UNSUBSCRIBE_ATTRIBUTES:
	    msgBuffer.write_int64(objectClass);
	    msgBuffer.write_int64(region);
	    break ;

          // Body contains interactionClass,region,boolean	    
	  case DDM_SUBSCRIBE_INTERACTION:
	  case DDM_UNSUBSCRIBE_INTERACTION:
	    msgBuffer.write_int64(interactionClass);
	    msgBuffer.write_int64(region);
	    msgBuffer.write_bool(boolean);
	    break ;

          // Body contains objectClass,attribute,space  
          case GET_ATTRIBUTE_SPACE_HANDLE:
            msgBuffer.write_int64(objectClass);
            msgBuffer.write_int64(attribute);
            msgBuffer.write_int64(space);
            break ;

          // Body contains space,number,region
          case DDM_CREATE_REGION:
            msgBuffer.write_int64(space) ;
            msgBuffer.write_int64(number);
            msgBuffer.write_int64(region);
            break ;

          // Body contains interactionClass,space
          case GET_INTERACTION_SPACE_HANDLE:
            msgBuffer.write_int64(interactionClass);
            msgBuffer.write_int64(space);
            break ;

          // Body contains federate,federationName,federateName
          // Note : federate relevant on RTIA answer only
          case JOIN_FEDERATION_EXECUTION:
            msgBuffer.write_int16(federate);
            writeFederationName(msgBuffer);
            msgBuffer.write_string(federateName);
            break ;

          // FederationTime yet put in header
          // Body contains objectClass,handleArraySize,HandleArray
          case PUBLISH_OBJECT_CLASS:
          case SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES:
            msgBuffer.write_int64(objectClass);
            msgBuffer.write_int16(handleArraySize);
            writeHandleArray(msgBuffer);
            break ;

          // FederationTime yet put in header
          // Body contains objectClass,object,name
          case REGISTER_OBJECT_INSTANCE:
            msgBuffer.write_int64(objectClass);
            msgBuffer.write_int64(object);
            msgBuffer.write_string(name);
            break ;

          // FederationTime (or zero) yet put in header
          // Body contains objectClass,handleArraySize,object,tag,HandleArray,
          // ValueArray,ResignAction and 
          // boolean (true with time, false without time)
          case UPDATE_ATTRIBUTE_VALUES:
          case REFLECT_ATTRIBUTE_VALUES:
            msgBuffer.write_int64(objectClass);
            msgBuffer.write_int16(handleArraySize);
            msgBuffer.write_int64(object);
            msgBuffer.write_string(tag);
            writeHandleArray(msgBuffer);
            writeValueArray(msgBuffer);
            writeResignAction(msgBuffer);
            msgBuffer.write_bool(boolean);
            break ;

          // FederationTime yet put in header
          // Body contains objectClass,object,tag,name,label,resignAction
          case DISCOVER_OBJECT_INSTANCE:
            msgBuffer.write_int64(objectClass);
            msgBuffer.write_int64(object);
            msgBuffer.write_string(tag);
            msgBuffer.write_string(name);
            msgBuffer.write_string(label);
            writeResignAction(msgBuffer);
            break ;

          // FederationTime yet put in header 
          // Body contains objectClass,object,tag,name,label,resignAction  
          case DELETE_OBJECT_INSTANCE:
          case REMOVE_OBJECT_INSTANCE:
            msgBuffer.write_int64(objectClass);
            msgBuffer.write_int64(object);
            msgBuffer.write_string(tag);
            msgBuffer.write_string(name);
            msgBuffer.write_string(label);
            writeResignAction(msgBuffer);
	    msgBuffer.write_bool(boolean);
            break ;

          // Body contains object  
          case LOCAL_DELETE_OBJECT_INSTANCE:
            msgBuffer.write_int64(object);
            break;

          // FederationTime yet put in header
          // Body contains objectClass,name,attribute
          case GET_OBJECT_CLASS_HANDLE:
          case GET_OBJECT_CLASS_NAME:
          case GET_ATTRIBUTE_HANDLE:
          case GET_ATTRIBUTE_NAME:
            msgBuffer.write_int64(objectClass);
            msgBuffer.write_string(name);
            msgBuffer.write_int16(attribute);
            break ;

         // FederationTime yet put in header
          // Body contains object,objectClass
          case GET_OBJECT_CLASS:
            msgBuffer.write_int64(object);
            msgBuffer.write_int64(objectClass);
            break;

          // Body contains name,space
          case GET_SPACE_HANDLE:
          case GET_SPACE_NAME:
            msgBuffer.write_string(name);
            msgBuffer.write_int64(space);
            break ;

          // Body contains name,dimension,space
          case GET_DIMENSION_HANDLE:
          case GET_DIMENSION_NAME:
            msgBuffer.write_string(name);
            msgBuffer.write_int64(dimension);
            msgBuffer.write_int64(space);
            break ;

          // Body contains interactionClass,handleArraySize,tag,handleArray,
          // valueArray,region,resignAction
          // and boolean (true with time, false without time)
          case SEND_INTERACTION:
          case RECEIVE_INTERACTION:
            msgBuffer.write_int64(interactionClass);
            msgBuffer.write_int16(handleArraySize);
            msgBuffer.write_string(tag);
            writeHandleArray(msgBuffer);
            writeValueArray(msgBuffer);
	    msgBuffer.write_int64(region);
            writeResignAction(msgBuffer);
            msgBuffer.write_bool(boolean);
            break ;

          // Body contains interactionClass,name,parameter
          case GET_INTERACTION_CLASS_HANDLE:
          case GET_INTERACTION_CLASS_NAME:
          case GET_PARAMETER_HANDLE:
          case GET_PARAMETER_NAME:
            msgBuffer.write_int64(interactionClass);
            msgBuffer.write_string(name);
            msgBuffer.write_int16(parameter);
            break ;

          // Body contains handleArraySize,transport,order,object,HandleArray
          case CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE:
          case CHANGE_ATTRIBUTE_ORDER_TYPE:
            msgBuffer.write_int16(handleArraySize);
            msgBuffer.write_int64(transport);
            msgBuffer.write_int64(order);           
            msgBuffer.write_int64(object);
            writeHandleArray(msgBuffer);
            break ;

          // Body contains interactionClass,transport,order
          case CHANGE_INTERACTION_TRANSPORTATION_TYPE:
          case CHANGE_INTERACTION_ORDER_TYPE:
            msgBuffer.write_int64(interactionClass);
            msgBuffer.write_int64(transport);
            msgBuffer.write_int64(order);  
            break ;

          // Body contains region,extents            
	  case DDM_MODIFY_REGION:
            msgBuffer.write_int64(region);
	    writeExtents(msgBuffer);
	    break ;

          // Body contains region            
	  case DDM_DELETE_REGION:
            msgBuffer.write_int64(region);
	    break ;

          // Body contains object,name	    
	  case GET_OBJECT_INSTANCE_HANDLE:
	  case GET_OBJECT_INSTANCE_NAME :
	    msgBuffer.write_int64(object);
	    msgBuffer.write_string(name);
	    break;

          // Body contains ResignAction
          case RESIGN_FEDERATION_EXECUTION:
            writeResignAction(msgBuffer);
            break;

          // Body contains interactionClass
          case PUBLISH_INTERACTION_CLASS:
          case UNPUBLISH_INTERACTION_CLASS:
          case SUBSCRIBE_INTERACTION_CLASS:
          case UNSUBSCRIBE_INTERACTION_CLASS:
          case TURN_INTERACTIONS_ON:
          case TURN_INTERACTIONS_OFF:
            msgBuffer.write_int64(interactionClass);
            break ;

          // Body contains boolean
          case ENABLE_TIME_REGULATION:
          case DISABLE_TIME_REGULATION:
          case ENABLE_TIME_CONSTRAINED:
          case DISABLE_TIME_CONSTRAINED:
            msgBuffer.write_bool(boolean);
            msgBuffer.write_double(lookahead);
            break ;

          // Body contains boolean, TickTime, TickTime
          case TICK_REQUEST:
            msgBuffer.write_bool(boolean);
            msgBuffer.write_bytes((char *)&minTickTime, sizeof(minTickTime));
            msgBuffer.write_bytes((char *)&maxTickTime, sizeof(maxTickTime));
            break ;

          // Body contains objectClass, handleArray
          case REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE:
            msgBuffer.write_int64(objectClass);
            msgBuffer.write_int16(handleArraySize);
            writeHandleArray(msgBuffer);
            break;             

          // lookahead
          // Warning : FederationTime has been modified (needs validation)
          case MODIFY_LOOKAHEAD:
          case QUERY_LOOKAHEAD:
            // we put another time but is the lookahead
	    msgBuffer.write_double(lookahead) ;
            break ;
            // -- Default Handler --

          default:
          // Nothing to do
          G.Out(pdGendoc,"exit  Message::writeBody with nothing to do");

        }
    }
    G.Out(pdGendoc,"exit  Message::writeBody");
}

// ----------------------------------------------------------------------------
void
Message::writeHandleArray(MessageBuffer &msgBuffer)
{
    //msgBuffer.write_bytes( (char *)handleArray,(unsigned int)(handleArraySize * sizeof(AttributeHandle))) ; 
    //msgBuffer.write_uint16(handleArraySize) ;
    for ( short i=0 ; i<handleArraySize ; i++)
      {
      msgBuffer.write_uint64(handleArray[i]) ;
      }
}

// ----------------------------------------------------------------------------
/*! Prepare the Message Header 
*/
void
Message::writeHeader(MessageBuffer &msgBuffer)
{
    G.Out(pdGendoc,"enter Message::writeHeader");

    // header contains type,exception and federationTime
    // Note sometimes federationTime is not useful.
    msgBuffer.write_int32(this->type) ;
    msgBuffer.write_int32(this->exception);
    msgBuffer.write_double(getFederationTime());

    // If the message carry an exception, the Body will only contain the
    // exception reason.
    if (exception != e_NO_EXCEPTION) {
        G.Out(pdGendoc,"exit  Message::writeHeader carrying exception");
        return ;
    }

    G.Out(pdGendoc,"exit  Message::writeHeader");
}

// ---------------------------------------------------------------
void
Message::writeResignAction(MessageBuffer &msgBuffer)
{
    //assert(false);
}

// ---------------------------------------------------------------
void
Message::writeFEDid(MessageBuffer &msgBuffer)
{
    msgBuffer.write_string(FEDid) ;
}

// ---------------------------------------------------------------
void
Message::writeFederationName(MessageBuffer &msgBuffer)
{
    msgBuffer.write_string(federationName) ;
}

// ---------------------------------------------------------------------------
void
Message::writeValueArray(MessageBuffer &msgBuffer)
{
    // length and value are stored into valueArray 
    // so we have to write length and then value with a write_bytes
    for (int i = 0 ; i < handleArraySize ; i ++)
        {
        msgBuffer.write_int64(valueArray[i].length) ;
        assert(valueArray[i].value != 0);
        msgBuffer.write_bytes((char *)(valueArray[i].value), valueArray[i].length) ;
    }
}

} // namespace certi

// $Id: Message_W.cc,v 3.34 2008/06/24 08:56:49 rousse Exp $
