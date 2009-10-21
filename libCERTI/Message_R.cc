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
// $Id: Message_R.cc,v 3.35 2009/10/21 20:04:46 erk Exp $
// ----------------------------------------------------------------------------


#include "Message.hh"

#include <cassert>

using std::vector ;

namespace certi {

static PrettyDebug D("RTIA_MSG","Message::");
static PrettyDebug G("GENDOC",__FILE__);

// ----------------------------------------------------------------------------
void
Message::receive(SocketUN* socket, MessageBuffer &msgBuffer) throw (NetworkError, NetworkSignal) {
	G.Out(pdGendoc,"enter Message::receive");
	/* 0- Reset receive buffer */
	/* FIXME this reset may not be necessary since we do 
	 * raw-receive + assume-size
	 */
	msgBuffer.reset();
	/* 1- Read 'reserved bytes' header from socket */
	D.Out(pdDebug,"Reading %d 'reserved' bytes",msgBuffer.reservedBytes);
	socket->receive(static_cast<const unsigned char *>(msgBuffer(0)), msgBuffer.reservedBytes);	
	//msgBuffer.show(msgBuffer(0),5);fflush(stdout);
	/* 2- update (assume) complete message size from reserved bytes */
	msgBuffer.assumeSizeFromReservedBytes();
	D.Out(pdDebug,"Got a MsgBuffer of size %d bytes (including %d reserved)",msgBuffer.size(),msgBuffer.reservedBytes);
	/* 3- receive the rest of the message */
	socket->receive(static_cast<const unsigned char *>(msgBuffer(msgBuffer.reservedBytes)),msgBuffer.size()-msgBuffer.reservedBytes);
	/* 4- deserialize the message 
	 * This is a polymorphic call 
	 * which may specialized in a daughter class  
	 */ 
	deserialize(msgBuffer);
	G.Out(pdGendoc,"exit  Message::receive");	
} /* end of receive */

void Message::deserialize(MessageBuffer& msgBuffer) {
	G.Out(pdGendoc,"enter Message::deserialize");
	/* We serialize the common Message part 
	 * ALL Messages will contain the following
	 */	
	Debug(D, pdDebug) << "Deserialize <" << getName().c_str()<<">"<<std::endl;	
        readHeader(msgBuffer);
        readBody(msgBuffer) ;
	G.Out(pdGendoc,"exit Message::deserialize");
} /* end of deserialize */

// ----------------------------------------------------------------------------
//! Read a Message Body from a Socket, should be called after ReadHeader.
void
Message::readBody(MessageBuffer &msgBuffer)
{
    G.Out(pdGendoc,"enter Message::readBody");
 
    // 1. Read Body from socket.
    //socket->receive(body.getBuffer(), header.bodySize);
	 // FIXME EN: we must update the write pointer of the 
	 //           MessageBody because we have just written 
	 //           on it using direct pointer access !! (nasty usage)
	 //body.addToWritePointer(header.bodySize);
	
    // 3. Read informations from Message Body according to message type.
    if (exception != e_NO_EXCEPTION) {
        exceptionReason = msgBuffer.read_string();
    }
    else {
 
        // 1- Prepare Body Structure according to Message Type
			//D.Mes(pdMessage, 'M', header.type);
			this->trace("RTIG::chooseProcessingMethod ");

        switch(type) {

          // Empty body
          case CLOSE_CONNEXION:
            break;

          // Body contains federationName, FEDid
          // Note : relevant only on federate request
          case CREATE_FEDERATION_EXECUTION:
            readFederationName(msgBuffer);
            readFEDid(msgBuffer) ;
            break ;

          // Body contains federationName
          case DESTROY_FEDERATION_EXECUTION:
            readFederationName(msgBuffer);
            break ;

          // Body contains label,tag,boolean and maybe
          // handleArraySize,handleArray
          case REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
            readLabel(msgBuffer);
            readTag(msgBuffer);
            boolean = msgBuffer.read_bool();
            // boolean true means federates set exists
            if ( boolean )
                {
                handleArraySize = msgBuffer.read_int16();
                readHandleArray(msgBuffer);
                }
            break ;

          // Body contains label,tag
          case ANNOUNCE_SYNCHRONIZATION_POINT:
          case REQUEST_FEDERATION_RESTORE_FAILED:
            readLabel(msgBuffer);
            readTag(msgBuffer);
            break ;

          // Body contains label
          case SYNCHRONIZATION_POINT_REGISTRATION_FAILED:
          case SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
          case SYNCHRONIZATION_POINT_ACHIEVED:
          case FEDERATION_SYNCHRONIZED:
          case REQUEST_FEDERATION_RESTORE:
          case REQUEST_FEDERATION_RESTORE_SUCCEEDED:
            readLabel(msgBuffer);
            break ;

          // Body contains federate,label
          case INITIATE_FEDERATE_RESTORE:
            federate = msgBuffer.read_int16();
            readLabel(msgBuffer);
            break ;

          // Body contains label
          case INITIATE_FEDERATE_SAVE:
            readLabel(msgBuffer);
            break ;

          // Body contains label, boolean
          case REQUEST_FEDERATION_SAVE:
            readLabel(msgBuffer);
            // boolean true means with time (in the header)
            boolean = msgBuffer.read_bool();
            break ;

          // Body contains objectClass
          case UNPUBLISH_OBJECT_CLASS:
          case UNSUBSCRIBE_OBJECT_CLASS:
          case START_REGISTRATION_FOR_OBJECT_CLASS:
          case STOP_REGISTRATION_FOR_OBJECT_CLASS:
            objectClass = msgBuffer.read_int64();
            break;

          // Body contains object,attribute,tag
          case IS_ATTRIBUTE_OWNED_BY_FEDERATE:
          case QUERY_ATTRIBUTE_OWNERSHIP:
            object = msgBuffer.read_int64();
            attribute = msgBuffer.read_int16();
            readTag(msgBuffer);
            break ;

          // Body contains object,attribute,federate
          case ATTRIBUTE_IS_NOT_OWNED:
          case INFORM_ATTRIBUTE_OWNERSHIP:
            object = msgBuffer.read_int64();
            attribute = msgBuffer.read_int16();
            federate = msgBuffer.read_int16();
            break ;

          // Body contains object,handleArraySize,HandleArray,tag
          case NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
          case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
          case ATTRIBUTE_OWNERSHIP_ACQUISITION:
          case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
            object = msgBuffer.read_int64();
            handleArraySize = msgBuffer.read_int16();
            readHandleArray(msgBuffer);
            readTag(msgBuffer);
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
            object = msgBuffer.read_int64();
            handleArraySize = msgBuffer.read_int16();
            readHandleArray(msgBuffer);
            break ;

          // Body contains object,region,boolean,handleArraySize,handleArray
	  case DDM_ASSOCIATE_REGION:
	    object = msgBuffer.read_int64();
	    region = msgBuffer.read_int64();
	    boolean = msgBuffer.read_bool();
	    handleArraySize = msgBuffer.read_int16();
            readHandleArray(msgBuffer);
	    break ;

          // Body contains objectClass,object,tag,handleArraySize,handleArray,
          // regions
	  case DDM_REGISTER_OBJECT:
	    objectClass = msgBuffer.read_int64();
	    object = msgBuffer.read_int64();
	    readTag(msgBuffer);
	    handleArraySize = msgBuffer.read_int16();
            readHandleArray(msgBuffer);
	    readRegions(msgBuffer);
	    break ;

          // Body contains objectClass,region,boolean,handleArraySize,
          // handleArray
	  case DDM_SUBSCRIBE_ATTRIBUTES:
	    objectClass = msgBuffer.read_int64();
	    region = msgBuffer.read_int64();
	    boolean = msgBuffer.read_bool();
	    handleArraySize = msgBuffer.read_int16();
            readHandleArray(msgBuffer);
	    break ;

          // Body contains object,region
	  case DDM_UNASSOCIATE_REGION:
	    object = msgBuffer.read_int64();
	    region = msgBuffer.read_int64();
	    break ;

          // Body contains objectClass,region
	  case DDM_UNSUBSCRIBE_ATTRIBUTES:	    
	    objectClass = msgBuffer.read_int64();
	    region = msgBuffer.read_int64();
	    break ;

          // Body contains interactionClass,region,boolean
	  case DDM_SUBSCRIBE_INTERACTION:
	  case DDM_UNSUBSCRIBE_INTERACTION:
	    interactionClass = msgBuffer.read_int64();
	    region = msgBuffer.read_int64();
	    boolean = msgBuffer.read_bool();
	    break ;

          // Body contains objectClass,attribute,space	    
          case GET_ATTRIBUTE_SPACE_HANDLE:
            objectClass = msgBuffer.read_int64();
            attribute = msgBuffer.read_int64();
            space = msgBuffer.read_int64();
            break ;

          // Body contains space,number,region
          case DDM_CREATE_REGION:
            space  = msgBuffer.read_int64();
            number = msgBuffer.read_int64();
            region = msgBuffer.read_int64();
            break ;

          // Body contains interactionClass,space
          case GET_INTERACTION_SPACE_HANDLE:
            interactionClass = msgBuffer.read_int64();
            space = msgBuffer.read_int64();
            break ;

          // Body contains federate,Federationname,FederateName
          // Note : federate relevant on RTIA answer only
          case JOIN_FEDERATION_EXECUTION:
            federate = msgBuffer.read_int16();
            readFederationName(msgBuffer);
            readFederateName(msgBuffer);
            break ;

          // federationTime got from header
          // Body contains objectClass,handleArraySize,HandleArray
          case PUBLISH_OBJECT_CLASS:
          case SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES:
            objectClass = msgBuffer.read_int64();
            handleArraySize = msgBuffer.read_int16();
            readHandleArray(msgBuffer);
            break ;

          // federationTime got from header
          // Body contains objectClass,object,name
          case REGISTER_OBJECT_INSTANCE:
            objectClass = msgBuffer.read_int64();
            object = msgBuffer.read_int64();
            readName(msgBuffer);
            break ;

          // FederationTime (or zero) yet got from header
          // Body contains objectClass,handleArraySize,object,tag,HandleArray,
          // ValueArray,ResignAction and 
          // boolean (true with time, false without time)
          case UPDATE_ATTRIBUTE_VALUES:
          case REFLECT_ATTRIBUTE_VALUES:
            objectClass = msgBuffer.read_int64();
            handleArraySize = msgBuffer.read_int16();
            object = msgBuffer.read_int64();
            readTag(msgBuffer);
            readHandleArray(msgBuffer);
            readValueArray(msgBuffer);
            readResignAction(msgBuffer);
            boolean = msgBuffer.read_bool();
            break ;

          // FederationTime yet got from header
          // Body contains objectClass,object,tag,name,label,resignAction
          case DISCOVER_OBJECT_INSTANCE:
            objectClass = msgBuffer.read_int64();
            object = msgBuffer.read_int64();
            readTag(msgBuffer);
            readName(msgBuffer);
            readLabel(msgBuffer);
            readResignAction(msgBuffer);
            break ;

          // FederationTime yet put in header 
          // Body contains objectClass,object,tag,name,label,resignAction  
          case DELETE_OBJECT_INSTANCE:
          case REMOVE_OBJECT_INSTANCE:
            objectClass = msgBuffer.read_int64();
            object = msgBuffer.read_int64();
            readTag(msgBuffer);
            readName(msgBuffer);
            readLabel(msgBuffer);
            readResignAction(msgBuffer);
            boolean = msgBuffer.read_bool();
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
            objectClass = msgBuffer.read_int64();
            readName(msgBuffer);
            attribute = msgBuffer.read_int16();
            break ;

         // FederationTime yet got from header
          // Body contains object,objectClass
          case GET_OBJECT_CLASS:
            object = msgBuffer.read_int64();
            objectClass = msgBuffer.read_int64();
            break ;

          // Body contains name,space
          case GET_SPACE_HANDLE:
          case GET_SPACE_NAME:
            this->readName(msgBuffer);
            this->space = msgBuffer.read_int64();
            break ;

          // Body contains name,dimension,space
          case GET_DIMENSION_HANDLE:
          case GET_DIMENSION_NAME:
            this->readName(msgBuffer);
            this->dimension = msgBuffer.read_int64();
            this->space = msgBuffer.read_int64();
            break ;

          case SEND_INTERACTION:
          case RECEIVE_INTERACTION:
            // Body contains interactionClass,handleArraySize,tag,handleArray,
            // valueArray,region,resignAction,boolean
            // boolean true means with time, false without time
            interactionClass = msgBuffer.read_int64();
            handleArraySize = msgBuffer.read_int16();
            readTag(msgBuffer);
            readHandleArray(msgBuffer);
            readValueArray(msgBuffer);
	    region = msgBuffer.read_int64();
            readResignAction(msgBuffer);
            boolean = msgBuffer.read_bool();
            break ;

          // Body contains interactionClass,name,parameter
          case GET_INTERACTION_CLASS_HANDLE:
          case GET_INTERACTION_CLASS_NAME:
          case GET_PARAMETER_HANDLE:
          case GET_PARAMETER_NAME:
            interactionClass = msgBuffer.read_int64();
            readName(msgBuffer);
            parameter = msgBuffer.read_int16();
            break ;

          // Body contains handleArraySize,transport,order,object,HandleArray
          case CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE:
          case CHANGE_ATTRIBUTE_ORDER_TYPE:
            handleArraySize = msgBuffer.read_int64();
            transport = msgBuffer.read_int64();
            order = msgBuffer.read_int64();
            object = msgBuffer.read_int64();
            readHandleArray(msgBuffer);
            break ;

          // Body contains interactionClass,transport,order
          case CHANGE_INTERACTION_TRANSPORTATION_TYPE:
          case CHANGE_INTERACTION_ORDER_TYPE:
            interactionClass = msgBuffer.read_int64();
            transport = msgBuffer.read_int64();
            order = msgBuffer.read_int64();
            break;

          // Body contains name,transport
          case GET_TRANSPORTATION_HANDLE:
          case GET_TRANSPORTATION_NAME:
            this->readName(msgBuffer);
            transport = msgBuffer.read_int64();
            break;

          // Body contains name,order
          case GET_ORDERING_HANDLE:
          case GET_ORDERING_NAME:
            this->readName(msgBuffer);
            order = msgBuffer.read_int64();
            break;

          // Body contains region,extents
	  case DDM_MODIFY_REGION:
            region = msgBuffer.read_int64();
	    readExtents(msgBuffer);
	    break ;

          // Body contains region
	  case DDM_DELETE_REGION:
            region = msgBuffer.read_int64();
	    break ;

          // Body contains object,name
	  case GET_OBJECT_INSTANCE_HANDLE:
	  case GET_OBJECT_INSTANCE_NAME:
            object = msgBuffer.read_int64();
	    readName(msgBuffer);
	    break;

          // Body contains resignAction
          case RESIGN_FEDERATION_EXECUTION:
            readResignAction(msgBuffer);
            break;

          // Body contains interactionClass
          case PUBLISH_INTERACTION_CLASS:
          case UNPUBLISH_INTERACTION_CLASS:
          case SUBSCRIBE_INTERACTION_CLASS:
          case UNSUBSCRIBE_INTERACTION_CLASS:
          case TURN_INTERACTIONS_ON:
          case TURN_INTERACTIONS_OFF:
            interactionClass = msgBuffer.read_int64();
            break ;

          // Body contains boolean
          case ENABLE_TIME_REGULATION:
          case DISABLE_TIME_REGULATION:
          case ENABLE_TIME_CONSTRAINED:
          case DISABLE_TIME_CONSTRAINED:
            boolean = msgBuffer.read_bool();
            lookahead = msgBuffer.read_double();
            break ;

	  // Body contains boolean
	  case ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH:
	  case DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH:
	  case ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
	  case DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
	  case ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH:
	  case DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH:
	  case ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH:
	  case DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH:
	    break;

          // Body contains boolean, TickTime, TickTime
          case TICK_REQUEST:
            boolean = msgBuffer.read_bool();
            msgBuffer.read_bytes((char *)&minTickTime, sizeof(minTickTime));
            msgBuffer.read_bytes((char *)&maxTickTime, sizeof(maxTickTime));
            break ;

          // Body contains objectClass, handleArraySize,
          // handleArray
          case REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE:
            objectClass = msgBuffer.read_int64();
            handleArraySize = msgBuffer.read_int16();
            readHandleArray(msgBuffer);
            break;    

          // Body contains object,handleArraySize,
          // handleArray
          case REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
            object = msgBuffer.read_int64();
            handleArraySize = msgBuffer.read_int16();
            readHandleArray(msgBuffer);
            break;                        

          case PROVIDE_ATTRIBUTE_VALUE_UPDATE:
	    object = msgBuffer.read_int64();
            handleArraySize = msgBuffer.read_int16();
            readHandleArray(msgBuffer);
	    break ;

          // lookahead got from header
          // Warning : FederationTime has been modified (needs validation)
          case MODIFY_LOOKAHEAD:
          case QUERY_LOOKAHEAD:
            // we get another time but is the lookahead
            lookahead = msgBuffer.read_double();
            fed_time.setZero();
            break ;
	    
            // -- Default Handler --
            default:
              G.Out(pdGendoc,"exit  Message::readBody with nothing to do");

        }
    }
    G.Out(pdGendoc,"exit  Message::readBody");
}

// ----------------------------------------------------------------------------
/*! Read a Header Message
*/
void
Message::readHeader(MessageBuffer &msgBuffer)
{
    G.Out(pdGendoc,"enter Message::readHeader");

    // 1- get Header (not a really header but buffer beginning)

    type = (Type)msgBuffer.read_int32() ;
    exception = (TypeException)msgBuffer.read_int32();
    setFederationTime(msgBuffer.read_double());    
    // If the message carry an exception, the Body will only contain the
    // exception reason.
    
    D.Out(pdDebug,"Received message type <%d> ",type);
    		
    if (exception != e_NO_EXCEPTION)
        {
        G.Out(pdGendoc,"exit  Message::readHeader carrying an exception");
        return ;
        }

    G.Out(pdGendoc,"exit  Message::readHeader");
}

// ----------------------------------------------------------------------------
void
Message::readHandleArray(MessageBuffer &msgBuffer)
{
    //handleArraySize = msgBuffer.read_uint16() ;
    handleArray.resize(handleArraySize) ;
    for ( short i=0 ; i<handleArraySize ; i++)
      {
      handleArray[i] = msgBuffer.read_uint64() ;
      }
}

// ----------------------------------------------------------------------------
void
Message::readLabel(MessageBuffer &msgBuffer)
{
    label = msgBuffer.read_string() ;
}

// ----------------------------------------------------------------------------
void
Message::readName(MessageBuffer &msgBuffer)
{
    name = msgBuffer.read_string();
}

// ----------------------------------------------------------------------------
void
Message::readFederationName(MessageBuffer &msgBuffer)
{
    federationName = msgBuffer.read_string() ;  
}

// ----------------------------------------------------------------------------
void
Message::readFederateName(MessageBuffer &msgBuffer)
{
    federateName = msgBuffer.read_string() ;
}

// ----------------------------------------------------------------------------
void
Message::readResignAction(MessageBuffer &msgBuffer)
{
    // BUG: Should do something.
}

// ----------------------------------------------------------------------------
void
Message::readTag(MessageBuffer &msgBuffer)
{
    tag = msgBuffer.read_string();
}

// ----------------------------------------------------------------------------
void
Message::readFEDid(MessageBuffer &msgBuffer)
{
    FEDid = msgBuffer.read_string();
}

// ----------------------------------------------------------------------------
//! readValueArray.
void
Message::readValueArray(MessageBuffer &msgBuffer)
{
   G.Out(pdGendoc,"enter Message::readValueArray");

    valueArray.resize(handleArraySize) ;
    for (int i = 0 ; i < handleArraySize ; i ++)
        valueArray[i] = msgBuffer.read_string();
}

// ----------------------------------------------------------------------------
void Message::trace(const char* context)
{
#ifndef NDEBUG
D.Mes(pdMessage,'M',this->type,context);
#endif
}

} // namespace certi

// $Id: Message_R.cc,v 3.35 2009/10/21 20:04:46 erk Exp $
