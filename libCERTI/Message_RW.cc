// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libcerti
//
// CERTI-libcerti is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libcerti is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: Message_RW.cc,v 3.2 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "Message.hh"

namespace certi {

static pdCDebug D("MESSAGE", "(message ) - ");

// You can comment this out if you don't want to optimize network messages.
#define USE_HEADER_AND_BODY

// ----------
// -- Read --
// ----------

void Message::read(SocketUN *socket)
  throw(NetworkError,
	 NetworkSignal)
{
#ifdef USE_HEADER_AND_BODY

  Boolean Result;

  // 1- Read Header
  Result = readHeader(socket);

  // 2- if ReadHeader returned RTI_TRUE, Read Body.
  if(Result == RTI_TRUE)
    readBody(socket);

#else

  socket->receive((void *) this, sizeof(Message));

#endif
}


// --------------
// -- ReadBody --
// --------------

void Message::readBody(SocketUN *socket)
{  
  MessageBody    Body;

  if(Header.BodySize == 0)
    throw RTIinternalError("ReadBody should not have been called.");

  // 1. Read Body from socket.

  socket->receive((void *) Body.getBuffer(), Header.BodySize);

  // 3. Read informations from Message Body according to message type.

  if(Header.Exception != e_NO_EXCEPTION) {
    Body.readString(RaisonException, MAX_EXCEPTION_REASON_LENGTH);
  }
  else {

    // 1- Prepare Body Structure according to Message Type
    switch(Header.Type) {

      // --- No Variable Part, Body not empty ---

    case CREATE_FEDERATION_EXECUTION:
    case DESTROY_FEDERATION_EXECUTION:
      readFederationName(&Body);
      break;

    case REQUEST_PAUSE:
    case REQUEST_RESUME:
    case INITIATE_PAUSE:
    case INITIATE_RESUME:
    case PAUSE_ACHIEVED:
//    case RESUME_ACHIEVED:
      readLabel(&Body);
      break;

			    
    case IS_ATTRIBUTE_OWNED_BY_FEDERATE:
		case QUERY_ATTRIBUTE_OWNERSHIP:
     // B.c. Objectid,AttribHandle and Tag.
      Objectid         = Body.readLongInt();
      AttribHandle     = Body.readShortInt();
      readTag(&Body);
      break;

		case ATTRIBUTE_IS_NOT_OWNED:
		case INFORM_ATTRIBUTE_OWNERSHIP:
      Objectid         = Body.readLongInt();
      AttribHandle     = Body.readShortInt();
			NumeroFedere     = Body.readShortInt();
      break;
									
		case NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
		case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
		case ATTRIBUTE_OWNERSHIP_ACQUISITION:
		case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
		case ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
      Objectid         = Body.readLongInt();
			HandleArraySize =  Body.readShortInt(); 
      readHandleArray(&Body);
      readTag(&Body);
      break;	

		case ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
		case ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
		case ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
		case UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
		case CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
		case ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
		case CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
		case CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION: 
      Objectid         = Body.readLongInt();
			HandleArraySize =  Body.readShortInt();
      readHandleArray(&Body);
      break;	

      // --- MessageJ_R_Struct --

    case JOIN_FEDERATION_EXECUTION:
      readFederationName(&Body);
      readFederateName(&Body);
      break;

      // --- MessageO_I_Struct ---

    case PUBLISH_OBJECT_CLASS:
    case SUBSCRIBE_OBJECT_CLASS_ATTRIBUTE:
      readHandleArray(&Body);
      break;
      
    case REGISTER_OBJECT:
      Objectid         = Body.readLongInt();
      readName(&Body);/*FAYET 25.07.01*/
      break;
      
    case UPDATE_ATTRIBUTE_VALUES:
    case REFLECT_ATTRIBUTE_VALUES:
      // B.c. Objectid, Tag, HandleArray[], ValueArray[] and RAction.
      Objectid         = Body.readLongInt();
      readTag(&Body);
      readHandleArray(&Body);
      readValueArray(&Body);
      readRAction(&Body);
      break;    
      
    case DISCOVER_OBJECT:
    case DELETE_OBJECT:
    case REMOVE_OBJECT:
      // B.c. Objectid, Tag, Label, RAction
      Objectid         = Body.readLongInt();
      readTag(&Body);
			readName(&Body);/*FAYET 25.07.01*/	
      readLabel(&Body);
      readRAction(&Body);
      break;
      
    case GET_OBJECT_CLASS_HANDLE:
    case GET_OBJECT_CLASS_NAME:
    case GET_ATTRIBUTE_HANDLE:
    case GET_ATTRIBUTE_NAME:
      // B.c. Name(and AttribHandle)
      readName(&Body);
      AttribHandle = Body.readShortInt();
      break;
    
    case SEND_INTERACTION:
    case RECEIVE_INTERACTION: 
      // B.c. Tag, HandleArray[], ValueArray[], RAction
      readTag(&Body);
      readHandleArray(&Body);
      readValueArray(&Body);
      readRAction(&Body);
      break;    
      
    case GET_INTERACTION_CLASS_HANDLE:
    case GET_INTERACTION_CLASS_NAME:
    case GET_PARAMETER_HANDLE:
    case GET_PARAMETER_NAME:
      // Body contains Name and ParamHandle
      readName(&Body);
      ParamHandle = Body.readShortInt();
      break;

    
      // --- MessageT_O_Struct, Body not empty ---

    case CHANGE_ATTRIBUTE_TRANSPORT_TYPE:
    case CHANGE_ATTRIBUTE_ORDER_TYPE:
      // B.c. Objectid, HandleArray
      Objectid         = Body.readLongInt();
      readHandleArray(&Body);
      break;

      // -- Default Handler --
   
    default:
      D.Out(pdExcept, "Unknown Type %d in ReadBody.", Header.Type);
      throw RTIinternalError("Message: Unknown Type for Body(Read).");
    }
  }
}


// ----------------
// -- ReadHeader --
// ----------------

Boolean Message::readHeader(SocketUN *socket)
{
  // 1- Read Header from Socket
  socket->receive((void *) &Header, sizeof(MessageHeader));

  // 2- Parse Header(Static Part)
  Type             = Header.Type;
  Exception        = Header.Exception;

  // If the message carry an exception, the Body will only contain the
  // Exception reason.

  if(Exception != e_NO_EXCEPTION)
    return RTI_TRUE;

  // 2- Parse Header according to its type(Variable Part)
  // NULL, UAV and SendInteraction are the most common ones.

  switch(Type) {
    
    // --- No Variable Part, No Body ---

//  case REQUEST_RESUME:
//  case INITIATE_RESUME:
  case RESUME_ACHIEVED:

    // --- No Variable Part, Body not empty ---

  case CREATE_FEDERATION_EXECUTION:   // Body contains NomFederation
  case DESTROY_FEDERATION_EXECUTION:  // Body contains NomFedere
  case REQUEST_PAUSE:                 // Body contains Label
  case REQUEST_RESUME:                // Body contains Label
  case INITIATE_PAUSE:                // Body contains Label
  case INITIATE_RESUME:               // Body contains Label
  case PAUSE_ACHIEVED:                // Body contains Label
  case IS_ATTRIBUTE_OWNED_BY_FEDERATE:// B.c. Objectid, AttribHandle and Tag. 
	case QUERY_ATTRIBUTE_OWNERSHIP: // B.c. Objectid and AttribHandle.
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
    break;


    // --- MessageJ_R_Struct --

  case RESIGN_FEDERATION_EXECUTION:   // No Body
    RAction = Header.VP.J_R.Action;
    break;

  case JOIN_FEDERATION_EXECUTION:     // Body contains NomFederation&NomFedere
    NumeroFedere = Header.VP.J_R.NumeroFedere;
    break;

    // --- MessageO_I_Struct, No Body ---

  case UNPUBLISH_OBJECT_CLASS:
  case UNSUBSCRIBE_OBJECT_CLASS_ATTRIBUTE:
    objectClassHandle = Header.VP.O_I.Handle;
    break;

  
  case PUBLISH_INTERACTION_CLASS:
  case UNPUBLISH_INTERACTION_CLASS:
  case SUBSCRIBE_INTERACTION_CLASS:
  case UNSUBSCRIBE_INTERACTION_CLASS:
  case TURN_INTERACTIONS_ON:
  case TURN_INTERACTIONS_OFF:
    InteractionHandle = Header.VP.O_I.Handle;
    break;

    // --- MessageO_I_Struct, Body not Empty ---

  case PUBLISH_OBJECT_CLASS:               // Body contains HandleArray
  case SUBSCRIBE_OBJECT_CLASS_ATTRIBUTE:   // Body contains HandleArray
  case REGISTER_OBJECT:                    // Body contains Objectid
  case UPDATE_ATTRIBUTE_VALUES:            // B.c. Objectid, Tag, HandleArray[]
                                           // ValueArray[] and RAction.
  case DISCOVER_OBJECT:                    // B.c. Objectid, Tag and RAction
  case REFLECT_ATTRIBUTE_VALUES:           // B.c. Objectid, Tag, HandleArray[]
                                           // and ValueArray[]
  case DELETE_OBJECT:                      // Body contains Objectid, Tag.
  case REMOVE_OBJECT:                      // B.c. Objectid, Tag, Label&RAction
  case GET_OBJECT_CLASS_HANDLE:            // Body contains Name
  case GET_OBJECT_CLASS_NAME:              // Body contains Name
  case GET_ATTRIBUTE_HANDLE:               // B.c. Name and AttribHandle.
  case GET_ATTRIBUTE_NAME:                 // B.c. Name and AttribHandle.
    objectClassHandle    = Header.VP.O_I.Handle;
    HandleArraySize = Header.VP.O_I.Size;
    Date            = Header.VP.O_I.Date;
    break;
    
  case SEND_INTERACTION:    // B.c. Tag, HandleArray[], ValueArray[]
  case RECEIVE_INTERACTION: // B.c. Tag, HandleArray[], ValueArray[], RAction
  case GET_INTERACTION_CLASS_HANDLE: // Body contains Name
  case GET_INTERACTION_CLASS_NAME:   // Body contains Name
  case GET_PARAMETER_HANDLE:         // Body contains Name and ParamHandle
  case GET_PARAMETER_NAME:           // Body contains Name and ParamHandle
    InteractionHandle = Header.VP.O_I.Handle;
    HandleArraySize   = Header.VP.O_I.Size;
    Date              = Header.VP.O_I.Date;
    break;

    // --- ReqIDStruct, No Body ---

  case REQUEST_ID:
    IDcount = Header.VP.ReqID.Count;
    FirstID = Header.VP.ReqID.First;
    LastID  = Header.VP.ReqID.Last;
    break;

    // --- MessageT_O_Struct, No Body ---

  case CHANGE_INTERACTION_TRANSPORT_TYPE:
  case CHANGE_INTERACTION_ORDER_TYPE:
    InteractionHandle = Header.VP.T_O.Handle;
    Transport         = Header.VP.T_O.Transport;
    Order             = Header.VP.T_O.Order;
    break;
    
    // --- MessageT_O_Struct, Body not empty ---

  case CHANGE_ATTRIBUTE_TRANSPORT_TYPE: // B.c. Objectid, HandleArray.
  case CHANGE_ATTRIBUTE_ORDER_TYPE:     // B.c. Objectid, HandleArray.
    HandleArraySize   = Header.VP.T_O.Size;
    Transport         = Header.VP.T_O.Transport;
    Order             = Header.VP.T_O.Order;
    break;

    // --- TimeStruct, No Body ---
  case REQUEST_FEDERATION_TIME:
  case REQUEST_LBTS:
  case REQUEST_FEDERATE_TIME:
  case TIME_ADVANCE_REQUEST:
  case NEXT_EVENT_REQUEST:
  case TIME_ADVANCE_GRANT:
    Date = Header.VP.Time.Date;
    break;


  case SET_LOOKAHEAD:
  case REQUEST_LOOKAHEAD:
    Lookahead = Header.VP.Time.Date;
    break;

  case SET_TIME_REGULATING:
  case SET_TIME_CONSTRAINED:
  case TICK_REQUEST:
    Bool = Header.VP.Time.mode;
    break;
		

   // -- Default Handler --
      
  default:
    D.Out(pdExcept, "Unknown Type %d in ReadHeader.", Header.Type);
    throw RTIinternalError("Message: Received unknown Header Type.");
  }
  
  // 4- If Header.BodySize is not 0, return RTI_TRUE, else RTI_FALSE

  if(Header.BodySize == 0)
    return RTI_FALSE;
  else
    return RTI_TRUE;
}


// ---------------
// -- ReadLabel --
// ---------------

void Message::readHandleArray(MessageBody *Body)
{
  Body->readBlock((char *) HandleArray,
		    HandleArraySize * sizeof(AttributeHandle));
}


// ---------------
// -- ReadLabel --
// ---------------

void Message::readLabel(MessageBody *Body)
{
  Body->readString(Label, MAX_USER_TAG_LENGTH);
}


// --------------
// -- ReadName --
// --------------

void Message::readName(MessageBody *Body)
{
  Body->readString(Name, MAX_USER_TAG_LENGTH);
}


// -----------------------
// -- ReadNomFederation --
// -----------------------

void Message::readFederationName(MessageBody *Body)
{
  Body->readString(NomFederation, MAX_FEDERATION_NAME_LENGTH);
}


// -------------------
// -- ReadNomFedere --
// -------------------

void Message::readFederateName(MessageBody *Body)
{
  Body->readString(NomFedere, MAX_FEDERATE_NAME_LENGTH);
}


// ---------------------
// -- ReadRAction --
// ---------------------

void 
Message::readRAction(MessageBody*)
{
  // BUG: Should do something.
}


// -------------
// -- ReadTag --
// -------------

void Message::readTag(MessageBody *Body)
{
  Body->readString(Tag, MAX_USER_TAG_LENGTH);
}


// --------------------
// -- ReadValueArray --
// --------------------

void Message::readValueArray(MessageBody *Body)
{
  for(int i = 0; i < HandleArraySize; i ++) {
    Body->readString(ValueArray[i], MAX_BYTES_PER_VALUE);
  }
}


// -----------
// -- Write --
// -----------

void Message::write(SocketUN *socket)
    throw(NetworkError,
	   NetworkSignal)
{
#ifdef USE_HEADER_AND_BODY

  Boolean Result;

  // 1- Call WriteHeader
  Result = writeHeader(socket);

  // 2- If WriteHeader returned RTI_TRUE, call WriteBody.
  if(Result == RTI_TRUE)
    writeBody(socket);

#else
  socket->send((void *) this, sizeof(Message));
#endif
}


// ---------------
// -- WriteBody --
// ---------------

void Message::writeBody(SocketUN *socket)
{
  MessageBody   Body;

  // 0- Copy the Header at the beginning of the Body, in order to
  //    make a single Socket->Emettre call while sending both.
  // WARNING: As the Body size is not known yet, we will have to
  //          change it in the copy also!
  Body.writeBlock((char *) &Header, sizeof(MessageHeader));

  // If the message carry an exception, the Body will only contain the
  // Exception reason.

  if(Header.Exception != e_NO_EXCEPTION) {
    Body.writeString(RaisonException);
  }
  else {

    // 1- Prepare Body Structure according to Message Type
    switch(Header.Type) {

      // --- No Variable Part, Body not empty ---

    case CREATE_FEDERATION_EXECUTION:
    case DESTROY_FEDERATION_EXECUTION:
      Body.writeString(NomFederation);
      break;

    case REQUEST_PAUSE:
    case REQUEST_RESUME:
    case INITIATE_PAUSE:
    case INITIATE_RESUME:
    case PAUSE_ACHIEVED:
      Body.writeString(Label);
      break;
			
    case IS_ATTRIBUTE_OWNED_BY_FEDERATE:
		case QUERY_ATTRIBUTE_OWNERSHIP:		  		
      // B.c. Objectid,AttribHandle,Tag 
      Body.writeLongInt(Objectid);
      Body.writeShortInt(AttribHandle);
      Body.writeString(Tag);
      break;			    

		case ATTRIBUTE_IS_NOT_OWNED:
		case INFORM_ATTRIBUTE_OWNERSHIP:
      Body.writeLongInt(Objectid);
      Body.writeShortInt(AttribHandle);
      Body.writeShortInt(NumeroFedere);			
      break;

		case NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
		case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
		case ATTRIBUTE_OWNERSHIP_ACQUISITION:
		case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE: 
      Body.writeLongInt(Objectid);
			Body.writeShortInt(HandleArraySize); 
      writeHandleArray(&Body);
      Body.writeString(Tag);
      break;			

		case ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
		case ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
		case ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
		case UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
		case ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
		case CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
		case ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
		case CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
		case CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION: 
      Body.writeLongInt(Objectid);
			Body.writeShortInt(HandleArraySize);
      writeHandleArray(&Body);
      break; 

      // --- MessageJ_R_Struct --

    case JOIN_FEDERATION_EXECUTION:
      Body.writeString(NomFederation);
      Body.writeString(NomFedere);
      break;

      // --- MessageO_I_Struct ---

    case PUBLISH_OBJECT_CLASS:
    case SUBSCRIBE_OBJECT_CLASS_ATTRIBUTE:
      writeHandleArray(&Body);
      break;
      
    case REGISTER_OBJECT:
      Body.writeLongInt(Objectid);
      Body.writeString(Name);
      break;
      
    case UPDATE_ATTRIBUTE_VALUES:
    case REFLECT_ATTRIBUTE_VALUES:
      // B.c. Objectid, Tag, HandleArray[], ValueArray[] and RAction.
      Body.writeLongInt(Objectid);
      Body.writeString(Tag);
      writeHandleArray(&Body);
      writeValueArray(&Body);
      writeRAction(&Body);
      break;    
      
    case DISCOVER_OBJECT:
    case DELETE_OBJECT:
    case REMOVE_OBJECT:
      // B.c. Objectid, Tag, Label, RAction
      Body.writeLongInt(Objectid);
      Body.writeString(Tag);
      Body.writeString(Name);/*FAYET 25.07.01*/
      Body.writeString(Label);
      writeRAction(&Body);
      break;
      
    case GET_OBJECT_CLASS_HANDLE:
    case GET_OBJECT_CLASS_NAME:
    case GET_ATTRIBUTE_HANDLE:
    case GET_ATTRIBUTE_NAME:
      // B.c. Name(and AttribHandle)
      Body.writeString(Name);
      Body.writeShortInt(AttribHandle);
      break;
    
    case SEND_INTERACTION:
    case RECEIVE_INTERACTION: 
      // B.c. Tag, HandleArray[], ValueArray[], RAction
      Body.writeString(Tag);
      writeHandleArray(&Body);
      writeValueArray(&Body);
      writeRAction(&Body);
      break;    
      
    case GET_INTERACTION_CLASS_HANDLE:
    case GET_INTERACTION_CLASS_NAME:
    case GET_PARAMETER_HANDLE:
    case GET_PARAMETER_NAME:
      // Body contains Name and ParamHandle
      Body.writeString(Name);
      Body.writeShortInt(ParamHandle);
      break;

    
      // --- MessageT_O_Struct, Body not empty ---

    case CHANGE_ATTRIBUTE_TRANSPORT_TYPE:
    case CHANGE_ATTRIBUTE_ORDER_TYPE:
      // B.c. Objectid, HandleArray
      Body.writeLongInt(Objectid);
      writeHandleArray(&Body);
      break;

      // -- Default Handler --

    default:
      D.Out(pdExcept, "Unknown Type %d in WriteBody.", Header.Type);
      throw RTIinternalError("Message: Unknown Type for Body.");

    }
  }

  // 2- Set Header.BodySize

  // Body Size does not include the copy of the Header!
  Header.BodySize = Body.getLength() - sizeof(MessageHeader);

  // Put the real Body Size in the copy of the Header.
((MessageHeader *) Body.getBuffer())->BodySize = Header.BodySize;

  // 3- Write Header to socket, then write Body to socket.
  //  socket->send((void *) &Header,           sizeof(MessageHeader));
  socket->send((void *) Body.getBuffer(), Body.getLength());
}


// ----------------------
// -- WriteHandleArray --
// ----------------------

void Message::writeHandleArray(MessageBody *Body)
{
  Body->writeBlock((char *) HandleArray,
		     HandleArraySize * sizeof(AttributeHandle));
}


// -----------------
// -- WriteHeader --
// -----------------

Boolean Message::writeHeader(SocketUN *socket)
{
  // 1- Clear Header
  memset((void *) &Header, '\0', sizeof(MessageHeader));

  // 2- Fill Header(Static Part)
  Header.Type             = Type;
  Header.Exception        = Exception;

  // If the message carry an exception, the Body will only contain the
  // Exception reason.

  if(Exception != e_NO_EXCEPTION) {
    Header.BodySize = 1;
    return RTI_TRUE;
  }

  // 3- Fill Header(Variable Part) [Sorted by Variable part type]

  // Note: Header.BodySize is not set to the actual Body size, but
  // to zero to indicate there is no Body, or 1 if a Body is needed.

  switch(Type) {
    
    // --- No Variable Part, No Body ---

//  case REQUEST_RESUME:
//  case INITIATE_RESUME:
  case RESUME_ACHIEVED:
    Header.BodySize = 0;
    break;

    // --- No Variable Part, Body not empty ---

  case CREATE_FEDERATION_EXECUTION:   // Body contains NomFederation
  case DESTROY_FEDERATION_EXECUTION:  // Body contains NomFederation
  case REQUEST_PAUSE:                 // Body contains Label
  case REQUEST_RESUME:                // Body contains Label
  case INITIATE_PAUSE:                // Body contains Label
  case INITIATE_RESUME:               // Body contains label
  case PAUSE_ACHIEVED:                // Body contains Label
  case IS_ATTRIBUTE_OWNED_BY_FEDERATE:// B.c. Objectid, AttribHandle and Tag.
	case QUERY_ATTRIBUTE_OWNERSHIP: // B.c. Objectid and AttribHandle.
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
    Header.BodySize = 1;
    break;


    // --- MessageJ_R_Struct --

  case RESIGN_FEDERATION_EXECUTION:   // No Body
    Header.VP.J_R.Action = RAction;
    Header.BodySize      = 0;
    break;

  case JOIN_FEDERATION_EXECUTION:     // Body contains NomFederation&NomFedere
    Header.VP.J_R.NumeroFedere = NumeroFedere;
    Header.BodySize            = 1;
    break;

    // --- MessageO_I_Struct, No Body ---

  case UNPUBLISH_OBJECT_CLASS:
  case UNSUBSCRIBE_OBJECT_CLASS_ATTRIBUTE:
    Header.VP.O_I.Handle = objectClassHandle ;
    Header.BodySize      = 0;
    break;

  
  case PUBLISH_INTERACTION_CLASS:
  case UNPUBLISH_INTERACTION_CLASS:
  case SUBSCRIBE_INTERACTION_CLASS:
  case UNSUBSCRIBE_INTERACTION_CLASS:
  case TURN_INTERACTIONS_ON:
  case TURN_INTERACTIONS_OFF:
    Header.VP.O_I.Handle = InteractionHandle;
    Header.BodySize      = 0;
    break;

    // --- MessageO_I_Struct, Body not Empty ---

  case PUBLISH_OBJECT_CLASS:               // Body contains HandleArray
  case SUBSCRIBE_OBJECT_CLASS_ATTRIBUTE:   // Body contains HandleArray
  case REGISTER_OBJECT:                    // Body contains Objectid
  case UPDATE_ATTRIBUTE_VALUES:            // B.c. Objectid, Tag, HandleArray[]
                                           // ValueArray[] and RAction.
  case DISCOVER_OBJECT:                    // B.c. Objectid, Tag and RAction
  case REFLECT_ATTRIBUTE_VALUES:           // B.c. Objectid, Tag, HandleArray[]
                                           // and ValueArray[]
  case DELETE_OBJECT:                      // Body contains Objectid, Tag.
  case REMOVE_OBJECT:                      // B.c. Objectid, Tag, Label&RAction
  case GET_OBJECT_CLASS_HANDLE:            // Body contains Name
  case GET_OBJECT_CLASS_NAME:              // Body contains Name
  case GET_ATTRIBUTE_HANDLE:               // B.c. Name and AttribHandle.
  case GET_ATTRIBUTE_NAME:                 // B.c. Name and AttribHandle.
    Header.VP.O_I.Handle = objectClassHandle ;
    Header.VP.O_I.Size   = HandleArraySize;
    Header.VP.O_I.Date   = Date;
    Header.BodySize      = 1;
    break;

   
  case SEND_INTERACTION:    // B.c. Tag, HandleArray[], ValueArray[]
  case RECEIVE_INTERACTION: // B.c. Tag, HandleArray[], ValueArray[], RAction
  case GET_INTERACTION_CLASS_HANDLE: // Body contains Name
  case GET_INTERACTION_CLASS_NAME:   // Body contains Name
  case GET_PARAMETER_HANDLE:         // Body contains Name and ParamHandle
  case GET_PARAMETER_NAME:           // Body contains Name and ParamHandle
    Header.VP.O_I.Handle = InteractionHandle;
    Header.VP.O_I.Size   = HandleArraySize;
    Header.VP.O_I.Date   = Date;
    Header.BodySize      = 1;
    break;

    // --- ReqIDStruct, No Body ---

  case REQUEST_ID:
    Header.VP.ReqID.Count = IDcount;
    Header.VP.ReqID.First = FirstID;
    Header.VP.ReqID.Last  = LastID;
    Header.BodySize       = 0;
    break;


    // --- MessageT_O_Struct, No Body ---

  case CHANGE_INTERACTION_TRANSPORT_TYPE:
  case CHANGE_INTERACTION_ORDER_TYPE:
    Header.VP.T_O.Handle    = InteractionHandle;
    Header.VP.T_O.Transport = Transport;
    Header.VP.T_O.Order     = Order;
    Header.BodySize         = 0;
    break;
    
    // --- MessageT_O_Struct, Body not empty ---

  case CHANGE_ATTRIBUTE_TRANSPORT_TYPE: // B.c. Objectid, HandleArray.
  case CHANGE_ATTRIBUTE_ORDER_TYPE:     // B.c. Objectid, HandleArray.
    Header.VP.T_O.Size      = HandleArraySize;
    Header.VP.T_O.Transport = Transport;
    Header.VP.T_O.Order     = Order;
    Header.BodySize         = 1;
    break;

    // --- TimeStruct, No Body ---
  case REQUEST_FEDERATION_TIME:
  case REQUEST_LBTS:
  case REQUEST_FEDERATE_TIME:
  case TIME_ADVANCE_REQUEST:
  case NEXT_EVENT_REQUEST:
  case TIME_ADVANCE_GRANT:
    Header.VP.Time.Date = Date;
    Header.BodySize     = 0;
    break;


  case SET_LOOKAHEAD:
  case REQUEST_LOOKAHEAD:
    Header.VP.Time.Date = Lookahead;
    Header.BodySize     = 0;
    break;

  case SET_TIME_REGULATING:
  case SET_TIME_CONSTRAINED:
  case TICK_REQUEST:
    Header.VP.Time.mode = Bool ;
    Header.BodySize     = 0;
    break;


      // -- Default Handler --
      
  default:
    D.Out(pdExcept, "Unknown Type %d in WriteHeader.", Header.Type);
    throw RTIinternalError("Message: Unknown Type for Header.");
    
  }

  // 4- If Header.BodySize = 0, send message and return RTI_FALSE,
  //    Else send nothing(will be done by WriteBody), and return RTI_TRUE.

  if(Header.BodySize == 0) {
    socket->send((void *) &Header, sizeof(MessageHeader));
    return RTI_FALSE;
  }
  else
    return RTI_TRUE;

}


// ---------------------
// -- WriteRAction --
// ---------------------

void 
Message::writeRAction(MessageBody*)
{
  // BUG: Should do something.
}


// ---------------------
// -- WriteValueArray --
// ---------------------

void Message::writeValueArray(MessageBody *Body)
{
  for(int i = 0; i < HandleArraySize; i ++) {
    Body->writeString(ValueArray[i]);
  }
}

}

// $Id: Message_RW.cc,v 3.2 2002/12/11 00:47:33 breholee Exp $
