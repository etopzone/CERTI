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
// $Id: NetworkMessage_RW.cc,v 3.1 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "NetworkMessage.hh"

namespace certi {

static pdCDebug D("MESSAGER", "(messageR) - ");

// You can comment this out if you don't want to optimize network messages.
#define USE_HEADER_AND_BODY


// ----------
// -- Read --
// ----------

void NetworkMessage::read(Socket *socket)
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

  Socket->receive(this, sizeof(NetworkMessage));

#endif
}


// --------------
// -- ReadBody --
// --------------

void NetworkMessage::readBody(Socket *socket)
{  
  MessageBody    Body;
  unsigned short  i;

  if(Header.BodySize == 0)
    throw RTIinternalError("ReadBody should not have been called.");

  // 1. Read Body from socket.

  socket->receive((void *) Body.getBuffer(), Header.BodySize);

  // 3. Read informations from Message Body according to message type.

  if((Header.Type == m_UPDATE_ATTRIBUTE_VALUES)
      ||(Header.Type == m_REFLECT_ATTRIBUTE_VALUES)) {
    Objectid = Body.readLongInt();
    readLabel(&Body);
    Body.readBlock((char *) HandleArray,
      HandleArraySize * sizeof(AttributeHandle));
    
    for(i = 0; i < HandleArraySize; i ++) {
      Body.readString(ValueArray[i], MAX_BYTES_PER_VALUE);
    }
  }

  else

    switch(Header.Type) {

      // -- O_I Variable Part With Date(Body Not Empty) --
      // Those types are put in front line because they are the most used.
    case m_SEND_INTERACTION:
    case m_RECEIVE_INTERACTION:
      readLabel(&Body);
      Body.readBlock((char *) HandleArray,
        HandleArraySize * sizeof(AttributeHandle));
      for(i = 0; i < HandleArraySize; i ++) {
 Body.readString(ValueArray[i], MAX_BYTES_PER_VALUE);
      }
      break;

    case m_CREATE_FEDERATION_EXECUTION:
    case m_DESTROY_FEDERATION_EXECUTION:
      readFederationName(&Body);
      break;

    case m_REQUEST_PAUSE:
    case m_REQUEST_RESUME:
      readLabel(&Body);
      break;

    case m_DELETE_OBJECT:
    case m_REMOVE_OBJECT:
      Objectid = Body.readLongInt();
      readLabel(&Body);
      break;

      // -- No Variable Part --
   
  case m_IS_ATTRIBUTE_OWNED_BY_FEDERATE:
    case m_INFORM_ATTRIBUTE_OWNERSHIP:
    case m_ATTRIBUTE_IS_NOT_OWNED:
  case m_QUERY_ATTRIBUTE_OWNERSHIP:
      Objectid = Body.readLongInt();
   HandleArray [0] = Body.readShortInt();
      readLabel(&Body);   
      break; 


    case m_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
    case m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
  case m_ATTRIBUTE_OWNERSHIP_ACQUISITION:
  case m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:  
      Objectid = Body.readLongInt();
   HandleArraySize = Body.readShortInt();
      for(i = 0; i < HandleArraySize; i ++)
     HandleArray [i] = Body.readShortInt();
   readLabel(&Body);        
      break; 


    case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
    case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
  case m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
  case m_UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
  case m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
  case m_CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
  case m_ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
  case m_CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
  case m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:      
      Objectid = Body.readLongInt();
   HandleArraySize = Body.readShortInt();
      for(i = 0; i < HandleArraySize; i ++)
     HandleArray [i] = Body.readShortInt();   
      break; 



      // -- Join Variable Part --

    case m_JOIN_FEDERATION_EXECUTION:
      readFederationName(&Body);
      readFederateName(&Body);
      break;

      // -- O_I Variable Part(Body not empty) --

    case m_PUBLISH_OBJECT_CLASS:
    case m_SUBSCRIBE_OBJECT_CLASS:
      for(i = 0; i < HandleArraySize; i ++)
 HandleArray [i] = Body.readShortInt();
      break;

    case m_REGISTER_OBJECT:
    case m_DISCOVER_OBJECT:
      Objectid = Body.readLongInt();
   readLabel(&Body);
      break;


      // -- Default Handler --

    default:
      D.Out(pdExcept, "Unknown Type %d in ReadBody.", Header.Type);
      throw RTIinternalError("Unknown/Unimplemented Type for Body.");
    }

}


// ----------------
// -- ReadHeader --
// ----------------

Boolean NetworkMessage::readHeader(Socket *socket)
{
  // 1- Read Header from Socket
  socket->receive((void *) &Header, sizeof(HeaderStruct));

  // 2- Parse Header(Static Part)
  Type             = Header.Type;
  Exception        = Header.Exception;
  NumeroFedere     = Header.NumeroFedere;
  NumeroFederation = Header.NumeroFederation;

  // 2- Parse Header according to its type(Variable Part)
  // NULL, UAV and SendInteraction are the most common ones.

  if(Type == m_MESSAGE_NULL)
    Date = Header.VP.Time.Date;

  else if((Type == m_UPDATE_ATTRIBUTE_VALUES) 
    ||(Type == m_REFLECT_ATTRIBUTE_VALUES)) {
    objectClassHandle = Header.VP.O_I.Handle;
    HandleArraySize   = Header.VP.O_I.Size;
    Date              = Header.VP.O_I.Date;
  }

  else
    switch(Header.Type) {

    case m_SEND_INTERACTION:
    case m_RECEIVE_INTERACTION:
      InteractionHandle = Header.VP.O_I.Handle;
      HandleArraySize   = Header.VP.O_I.Size;
      Date              = Header.VP.O_I.Date;
      break;

      // -- No Variable Part --

    case m_CREATE_FEDERATION_EXECUTION:
    case m_DESTROY_FEDERATION_EXECUTION:
    case m_REQUEST_PAUSE:
    case m_DELETE_OBJECT:
    case m_REMOVE_OBJECT:
    case m_CLOSE_CONNEXION:
    case m_RESIGN_FEDERATION_EXECUTION:
    case m_REQUEST_RESUME:
  case m_IS_ATTRIBUTE_OWNED_BY_FEDERATE:  
    case m_INFORM_ATTRIBUTE_OWNERSHIP:
    case m_ATTRIBUTE_IS_NOT_OWNED:
  case m_QUERY_ATTRIBUTE_OWNERSHIP:
    case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
    case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
  case m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
    case m_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
    case m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
  case m_UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
  case m_ATTRIBUTE_OWNERSHIP_ACQUISITION:
  case m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
  case m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
  case m_CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
  case m_ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
  case m_CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
  case m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:  
      break;

      // -- Time Variable Part(No Body) [Continued]--

    case m_SET_TIME_REGULATING:
      Date          = Header.VP.Time.Date;
      EstRegulateur = Header.VP.Time.R_or_C;
      break;

    case m_SET_TIME_CONSTRAINED:
      Date          = Header.VP.Time.Date;
      EstContraint  = Header.VP.Time.R_or_C;
      break;

      // -- ReqID Variable Part(No Body) --

    case m_REQUEST_ID:
      IDcount       = Header.VP.ReqID.Count;
      FirstID       = Header.VP.ReqID.First;
      LastID        = Header.VP.ReqID.Last;
      break;
      // -- T_O Variable Part --

    case m_CHANGE_ATTRIBUTE_TRANSPORT_TYPE:
    case m_CHANGE_ATTRIBUTE_ORDER_TYPE:
    case m_CHANGE_INTERACTION_TRANSPORT_TYPE:
    case m_CHANGE_INTERACTION_ORDER_TYPE:
      throw RTIinternalError("Read Message not implemented for T/O.");
      break;

      // -- Join Variable Part(No Body) --

    case m_JOIN_FEDERATION_EXECUTION:
      NombreRegulateurs = Header.VP.Join.NbReg;
      AdresseMulticast  = Header.VP.Join.AdrMC;
      BestEffortAddress = Header.VP.Join.Addr;
      BestEffortPeer    = Header.VP.Join.Peer;
      break;

      // -- O_I Variable Part(No Body) --

    case m_UNPUBLISH_OBJECT_CLASS:
    case m_UNSUBSCRIBE_OBJECT_CLASS:
      objectClassHandle = Header.VP.O_I.Handle ;
      break;

    case m_PUBLISH_INTERACTION_CLASS:
    case m_UNPUBLISH_INTERACTION_CLASS:
    case m_SUBSCRIBE_INTERACTION_CLASS:
    case m_UNSUBSCRIBE_INTERACTION_CLASS:
    case m_TURN_INTERACTIONS_ON:
    case m_TURN_INTERACTIONS_OFF:
      InteractionHandle = Header.VP.O_I.Handle;
      break;

      // -- O_I Variable Part(Body not empty) --

    case m_PUBLISH_OBJECT_CLASS:
    case m_SUBSCRIBE_OBJECT_CLASS:
      objectClassHandle = Header.VP.O_I.Handle;
      HandleArraySize   = Header.VP.O_I.Size;
      break;

    case m_REGISTER_OBJECT:
    case m_DISCOVER_OBJECT:
      objectClassHandle = Header.VP.O_I.Handle;
      break;

      // -- Default Handler --

    default:
      D.Out(pdExcept, "Unknown Type %d in ReadHeader.", Header.Type);
      throw RTIinternalError("Received unknown Header Type.");
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

void NetworkMessage::readLabel(MessageBody *Body)
{
  Body->readString(Label, MAX_USER_TAG_LENGTH);
}


// -----------------------
// -- ReadNomFederation --
// -----------------------

void NetworkMessage::readFederationName(MessageBody *Body)
{
  Body->readString(NomFederation, MAX_FEDERATION_NAME_LENGTH);
}


// -------------------
// -- ReadNomFedere --
// -------------------

void NetworkMessage::readFederateName(MessageBody *Body)
{
  Body->readString(NomFedere, MAX_FEDERATE_NAME_LENGTH);
}


// -----------
// -- Write --
// -----------

void NetworkMessage::write(Socket *socket)
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
  socket->send(this, sizeof(NetworkMessage));
  //socket->send((NetworkMessage)this, sizeof(NetworkMessage));
#endif

}


// ---------------
// -- WriteBody --
// ---------------

void NetworkMessage::writeBody(Socket *socket)
{
  MessageBody   Body;
  unsigned short i;

  // 0- Copy the Header at the beginning of the Body, in order to
  //    make a single Socket->Emettre call while sending both.
  // WARNING: As the Body size is not known yet, we will have to
  //          change it in the copy also!
  Body.writeBlock((char *) &Header, sizeof(HeaderStruct));

  // 1- Prepare Body Structure according to Message Type
  if((Header.Type == m_UPDATE_ATTRIBUTE_VALUES)
      ||(Header.Type == m_REFLECT_ATTRIBUTE_VALUES)) {
    Body.writeLongInt(Objectid);
    Body.writeString(Label);
    Body.writeBlock((char *) HandleArray,
       HandleArraySize * sizeof(AttributeHandle));
    
    for(i = 0; i < HandleArraySize; i ++) {
      Body.writeString(ValueArray  [i]);
    }
  }
  else
    switch(Header.Type) {

      // -- O_I Variable Part With Date(Body Not Empty) --

    case m_SEND_INTERACTION:
    case m_RECEIVE_INTERACTION:
      Body.writeString(Label);
      Body.writeBlock((char *) HandleArray,
         HandleArraySize * sizeof(AttributeHandle));
      for(i = 0; i < HandleArraySize; i ++) {
 Body.writeString(ValueArray  [i]);
      }
      break;

      // -- No Variable Part --

    case m_CREATE_FEDERATION_EXECUTION:
    case m_DESTROY_FEDERATION_EXECUTION:
      Body.writeString(NomFederation);
      break;

    case m_REQUEST_PAUSE:
    case m_REQUEST_RESUME:
      Body.writeString(Label);
      break;

    case m_DELETE_OBJECT:
    case m_REMOVE_OBJECT:
      Body.writeLongInt(Objectid);
      Body.writeString(Label);
      break;

  case m_IS_ATTRIBUTE_OWNED_BY_FEDERATE:
    case m_INFORM_ATTRIBUTE_OWNERSHIP:
    case m_ATTRIBUTE_IS_NOT_OWNED:
  case m_QUERY_ATTRIBUTE_OWNERSHIP:
      Body.writeLongInt(Objectid);
   Body.writeShortInt(HandleArray [0]);
      Body.writeString(Label);   
      break;

    case m_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
    case m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
  case m_ATTRIBUTE_OWNERSHIP_ACQUISITION:
  case m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:  
      Body.writeLongInt(Objectid);
   Body.writeShortInt(HandleArraySize);
      for(i = 0; i < HandleArraySize; i ++)
     Body.writeShortInt(HandleArray [i]);
      Body.writeString(Label);        
      break; 


    case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
    case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
  case m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
  case m_UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
  case m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
  case m_CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
  case m_ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
  case m_CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
  case m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:          
      Body.writeLongInt(Objectid);
   Body.writeShortInt(HandleArraySize);
      for(i = 0; i < HandleArraySize; i ++)
     Body.writeShortInt(HandleArray [i]);   
      break; 

      // -- Join Variable Part --

    case m_JOIN_FEDERATION_EXECUTION:
      Body.writeString(NomFederation);
      Body.writeString(NomFedere);
      break;

      // -- O_I Variable Part(Body not empty) --

    case m_PUBLISH_OBJECT_CLASS:
    case m_SUBSCRIBE_OBJECT_CLASS:
      for(i = 0; i < HandleArraySize; i ++)
 Body.writeShortInt(HandleArray [i]);
      break;

    case m_REGISTER_OBJECT:
    case m_DISCOVER_OBJECT:
      Body.writeLongInt(Objectid);
      Body.writeString(Label);
      break;

      // -- Default Handler --

    default:
      D.Out(pdExcept, "Unknown Type %d in WriteBody.", Header.Type);
      throw RTIinternalError("Unknown/Unimplemented Type for Header.");

    }

  // 2- Set Header.BodySize

  // Body Size does not include the copy of the Header!
  Header.BodySize = Body.getLength() - sizeof(HeaderStruct);


  // Put the real Body Size in the copy of the Header.
((HeaderStruct *) Body.getBuffer())->BodySize = Header.BodySize;

  // 3- Write Header to socket, then write Body to socket.
  // socket->send((void *) &Header,           sizeof(HeaderStruct));
  socket->send((void *) Body.getBuffer(), Body.getLength());
}


// -----------------
// -- WriteHeader --
// -----------------

Boolean NetworkMessage::writeHeader(Socket *socket)
{
  // 1- Clear Header
  // memset((void *) &Header, '\0', sizeof(HeaderStruct));

  // 2- Fill Header(Static Part)
  Header.Type             = Type;
  Header.Exception        = Exception;
  Header.NumeroFedere     = NumeroFedere;
  Header.NumeroFederation = NumeroFederation;

  // 3- Fill Header(Variable Part) [Sorted by Variable part type]

  // Note: Header.BodySize is not set to the actual Body size, but
  // to zero to indicate there is no Body, or 1 if a Body is needed.

  if(Type == m_MESSAGE_NULL) {
    Header.BodySize       = 0;
    Header.VP.Time.Date   = Date;
  } 
  else if((Type == m_UPDATE_ATTRIBUTE_VALUES)
    ||(Type == m_REFLECT_ATTRIBUTE_VALUES)) {
    // Body contains ObjectHandle, HandleArray, ValueArray, Label
    Header.BodySize      = 1;
    Header.VP.O_I.Handle = objectClassHandle;
    Header.VP.O_I.Size   = HandleArraySize;
    Header.VP.O_I.Date   = Date;
  }
  else
    switch(Type) {


    case m_SEND_INTERACTION:
    case m_RECEIVE_INTERACTION:
      // Body contains HandleArray, ValueArray, Label.
      Header.BodySize      = 1;
      Header.VP.O_I.Handle = InteractionHandle;
      Header.VP.O_I.Size   = HandleArraySize;
      Header.VP.O_I.Date   = Date;
      break;
      
      // -- No Variable Part, No Body --
      
    case m_CLOSE_CONNEXION:
    case m_RESIGN_FEDERATION_EXECUTION:
//    case m_REQUEST_RESUME:
      Header.BodySize = 0;
      break;
      
      // -- No Variable Part, Body not empty --
      
    case m_CREATE_FEDERATION_EXECUTION:
    case m_DESTROY_FEDERATION_EXECUTION:
      // Body Contains NomFederation.
    case m_INFORM_ATTRIBUTE_OWNERSHIP:
    case m_ATTRIBUTE_IS_NOT_OWNED:
  case m_IS_ATTRIBUTE_OWNED_BY_FEDERATE:  
  case m_QUERY_ATTRIBUTE_OWNERSHIP:
    // Body Contains ObjectHandle and Label
    case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
    case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
  case m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
    // Body Contains ObjectHandle and HandleArray
    case m_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
    case m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
  case m_UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
  case m_ATTRIBUTE_OWNERSHIP_ACQUISITION:
  case m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
  case m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
  case m_CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
  case m_ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
  case m_CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
  case m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION: 
      Header.BodySize      = 1;
      break;

      
    case m_REQUEST_PAUSE:
    case m_REQUEST_RESUME:
      // Body Contains Label(should be non-empty)
      // BUG: S'il fait moins de 16 octet, il passe dans le header.
      Header.BodySize = 1;
      break;
      
    case m_DELETE_OBJECT:
    case m_REMOVE_OBJECT:
      // Body Contains ObjectHandle, and Label
      Header.BodySize = 1;
      break;
      
      // -- Time Variable Part(No Body) [Continued] --
      
    case m_SET_TIME_REGULATING:
      Header.BodySize       = 0;
      Header.VP.Time.Date   = Date;
      Header.VP.Time.R_or_C = EstRegulateur;
      break;
      
    case m_SET_TIME_CONSTRAINED:
      Header.BodySize       = 0;
      Header.VP.Time.Date   = Date;
      Header.VP.Time.R_or_C = EstContraint;
      break;
      
      // -- ReqID Variable Part(No Body) --
      
    case m_REQUEST_ID:
      Header.BodySize       = 0;
      Header.VP.ReqID.Count = IDcount;
      Header.VP.ReqID.First = FirstID;
      Header.VP.ReqID.Last  = LastID;
      break;
      
      // -- T_O Variable Part --
      
    case m_CHANGE_ATTRIBUTE_TRANSPORT_TYPE:
    case m_CHANGE_ATTRIBUTE_ORDER_TYPE:
    case m_CHANGE_INTERACTION_TRANSPORT_TYPE:
    case m_CHANGE_INTERACTION_ORDER_TYPE:
      throw RTIinternalError("Write Message not implemented for T/O.");
      break;
      
      // -- Join Variable Part --
      
    case m_JOIN_FEDERATION_EXECUTION:
      // Body contains NomFederation and NomFedere
      Header.BodySize      = 1;
      Header.VP.Join.NbReg = NombreRegulateurs;
      Header.VP.Join.AdrMC = AdresseMulticast;
      Header.VP.Join.Addr  = BestEffortAddress;
      Header.VP.Join.Peer  = BestEffortPeer;
      break;
      
    // -- O_I Variable Part(No Body) --
      
    case m_UNPUBLISH_OBJECT_CLASS:
    case m_UNSUBSCRIBE_OBJECT_CLASS:
      Header.BodySize      = 0;
      Header.VP.O_I.Handle = objectClassHandle;
      break;
      
    case m_PUBLISH_INTERACTION_CLASS:
    case m_UNPUBLISH_INTERACTION_CLASS:
    case m_SUBSCRIBE_INTERACTION_CLASS:
    case m_UNSUBSCRIBE_INTERACTION_CLASS:
    case m_TURN_INTERACTIONS_ON:
    case m_TURN_INTERACTIONS_OFF:
      Header.BodySize      = 0;
      Header.VP.O_I.Handle = InteractionHandle;
      break;
      
      // -- O_I Variable Part(Body not empty) --
      
    case m_PUBLISH_OBJECT_CLASS:
    case m_SUBSCRIBE_OBJECT_CLASS:
      // Body contains HandleArray [HandleArraySize](if not empty)
      
      if(HandleArraySize > 0)
 Header.BodySize    = 1;
      else
      Header.BodySize    = 0;
      
      Header.VP.O_I.Handle = objectClassHandle;
      Header.VP.O_I.Size   = HandleArraySize;
      break;
      
    case m_REGISTER_OBJECT:
    case m_DISCOVER_OBJECT:
      // Body Contains ObjectHandle and Label
      Header.BodySize      = 1;
      Header.VP.O_I.Handle = objectClassHandle;
      break;
   
      // -- Default Handler --
      
    default:
      D.Out(pdExcept, "Unknown Type %d in WriteHeader.", Header.Type);
      throw RTIinternalError("Unknown/Unimplemented Type for Header.");
      
  }

  // 4- If Header.BodySize = 0, send message and return RTI_FALSE,
  //    Else send nothing(will be done by WriteBody), and return RTI_TRUE.

  if(Header.BodySize == 0) {
    socket->send((void *) &Header, sizeof(HeaderStruct));
    return RTI_FALSE;
  }
  else
    return RTI_TRUE;

}

}

// $Id: NetworkMessage_RW.cc,v 3.1 2002/12/11 00:47:33 breholee Exp $
