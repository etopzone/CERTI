// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
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
// $Id: NetworkMessage_RW.cc,v 3.5 2003/04/09 16:41:10 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>

#include "NetworkMessage.hh"

namespace certi {

static pdCDebug D("MESSAGER", "(messageR) - ");

// You can comment this out if you don't want to optimize network messages.
#define USE_HEADER_AND_BODY


// ----------
// -- Read --
// ----------

void
NetworkMessage::read(Socket *socket)
    throw (NetworkError,
           NetworkSignal)
{
#ifdef USE_HEADER_AND_BODY

    Boolean Result ;
    // 1- Read Header
    Result = readHeader(socket);

    // 2- if ReadHeader returned RTI_TRUE, Read Body.
    if (Result == RTI_TRUE)
        readBody(socket);

#else

    Socket->receive(this, sizeof(NetworkMessage));

#endif
}


// --------------
// -- ReadBody --
// --------------

void
NetworkMessage::readBody(Socket *socket)
{
    MessageBody Body ;
    unsigned short i ;

    if (Header.bodySize == 0)
        throw RTIinternalError("ReadBody should not have been called.");

    // 1. Read Body from socket.

    socket->receive((void *) Body.getBuffer(), Header.bodySize);

    // 3. Read informations from Message Body according to message type.

    if ((Header.type == m_UPDATE_ATTRIBUTE_VALUES)
        || (Header.type == m_REFLECT_ATTRIBUTE_VALUES)) {
        object = Body.readLongInt();
        readLabel(&Body);
        Body.readBlock((char *) handleArray,
                       handleArraySize * sizeof(AttributeHandle));

        for (i = 0 ; i < handleArraySize ; i ++) {
            Body.readString(ValueArray[i], MAX_BYTES_PER_VALUE);
        }
    }

    else

        switch(Header.type) {

            // -- O_I Variable Part With Date(Body Not Empty) --
        case m_SEND_INTERACTION:
        case m_RECEIVE_INTERACTION:
            readLabel(&Body);
            Body.readBlock((char *) handleArray,
                           handleArraySize * sizeof(AttributeHandle));
            for (i = 0 ; i < handleArraySize ; i ++) {
                Body.readString(ValueArray[i], MAX_BYTES_PER_VALUE);
            }
            break ;

        case m_CREATE_FEDERATION_EXECUTION:
        case m_DESTROY_FEDERATION_EXECUTION:
            readFederationName(&Body);
            break ;

        case m_REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
        case m_ANNOUNCE_SYNCHRONIZATION_POINT:
            readLabel(&Body);
            readTag(&Body);
            break ;

        case m_SYNCHRONIZATION_POINT_ACHIEVED:
        case m_SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
        case m_FEDERATION_SYNCHRONIZED:
            readLabel(&Body);
            break ;

        case m_DELETE_OBJECT:
        case m_REMOVE_OBJECT:
            object = Body.readLongInt();
            readLabel(&Body);
            break ;

            // -- No Variable Part --

        case m_IS_ATTRIBUTE_OWNED_BY_FEDERATE:
        case m_INFORM_ATTRIBUTE_OWNERSHIP:
        case m_ATTRIBUTE_IS_NOT_OWNED:
        case m_QUERY_ATTRIBUTE_OWNERSHIP:
            object = Body.readLongInt();
            handleArray[0] = Body.readShortInt();
            readLabel(&Body);
            break ;

        case m_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        case m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
        case m_ATTRIBUTE_OWNERSHIP_ACQUISITION:
        case m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
            object = Body.readLongInt();
            handleArraySize = Body.readShortInt();
            for (i = 0 ; i < handleArraySize ; i ++)
                handleArray[i] = Body.readShortInt();
            readLabel(&Body);
            break ;

        case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
        case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
        case m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
        case m_UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        case m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
        case m_CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        case m_ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
        case m_CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
        case m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
            object = Body.readLongInt();
            handleArraySize = Body.readShortInt();
            for (i = 0 ; i < handleArraySize ; i ++)
                handleArray[i] = Body.readShortInt();
            break ;

        case m_CREATE_REGION:
            space = Body.readLongInt();
            nbExtents = Body.readLongInt();
            region = Body.readLongInt();
            break ;

            // -- Join Variable Part --

        case m_JOIN_FEDERATION_EXECUTION:
            readFederationName(&Body);
            readFederateName(&Body);
            break ;

            // -- O_I Variable Part(Body not empty) --

        case m_PUBLISH_OBJECT_CLASS:
        case m_SUBSCRIBE_OBJECT_CLASS:
            for (i = 0 ; i < handleArraySize ; i ++)
                handleArray[i] = Body.readShortInt();
            break ;

        case m_REGISTER_OBJECT:
        case m_DISCOVER_OBJECT:
            object = Body.readLongInt();
            readLabel(&Body);
            break ;

            // -- Default Handler --

        default:
            D.Out(pdExcept, "Unknown type %d in ReadBody.", Header.type);
            throw RTIinternalError("Unknown/Unimplemented type for Body.");
        }

}


// ----------------
// -- ReadHeader --
// ----------------

Boolean
NetworkMessage::readHeader(Socket *socket)
{
    // 1- Read Header from Socket
    socket->receive((void *) &Header, sizeof(HeaderStruct));

    // 2- Parse Header(Static Part)
    type = Header.type ;
    exception = Header.exception ;
    federate = Header.federate ;
    federation = Header.federation ;

    // 2- Parse Header according to its type(Variable Part)
    // NULL, UAV and SendInteraction are the most common ones.

    if (type == m_MESSAGE_NULL)
        date = Header.VP.time.date ;

    else if ((type == m_UPDATE_ATTRIBUTE_VALUES)
             || (type == m_REFLECT_ATTRIBUTE_VALUES)) {
        objectClass = Header.VP.O_I.handle ;
        handleArraySize = Header.VP.O_I.size ;
        date = Header.VP.O_I.date ;
    }

    else
        switch(Header.type) {

        case m_SEND_INTERACTION:
        case m_RECEIVE_INTERACTION:
            interactionClass = Header.VP.O_I.handle ;
            handleArraySize = Header.VP.O_I.size ;
            date = Header.VP.O_I.date ;
            break ;

            // -- No Variable Part --

        case m_CREATE_FEDERATION_EXECUTION:
        case m_DESTROY_FEDERATION_EXECUTION:
        case m_REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
        case m_SYNCHRONIZATION_POINT_ACHIEVED:
        case m_SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
        case m_FEDERATION_SYNCHRONIZED:
        case m_ANNOUNCE_SYNCHRONIZATION_POINT:
        case m_DELETE_OBJECT:
        case m_REMOVE_OBJECT:
        case m_CLOSE_CONNEXION:
        case m_RESIGN_FEDERATION_EXECUTION:
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
        case m_CREATE_REGION:
            break ;

            // -- time Variable Part(No Body)[Continued]--

        case m_SET_TIME_REGULATING:
            date = Header.VP.time.date ;
            regulator = Header.VP.time.R_or_C ;
            break ;

        case m_SET_TIME_CONSTRAINED:
            date = Header.VP.time.date ;
            constrained = Header.VP.time.R_or_C ;
            break ;

            // -- ReqID Variable Part(No Body) --

        case m_REQUEST_ID:
            idCount = Header.VP.ReqID.count ;
            firstId = Header.VP.ReqID.first ;
            lastId = Header.VP.ReqID.last ;
            break ;
            // -- T_O Variable Part --

        case m_CHANGE_ATTRIBUTE_TRANSPORT_TYPE:
        case m_CHANGE_ATTRIBUTE_ORDER_TYPE:
        case m_CHANGE_INTERACTION_TRANSPORT_TYPE:
        case m_CHANGE_INTERACTION_ORDER_TYPE:
            throw RTIinternalError("Read Message not implemented for T/O.");
            break ;

            // -- Join Variable Part(No Body) --

        case m_JOIN_FEDERATION_EXECUTION:
            numberOfRegulators = Header.VP.Join.NbReg ;
            multicastAddress = Header.VP.Join.AdrMC ;
            bestEffortAddress = Header.VP.Join.Addr ;
            bestEffortPeer = Header.VP.Join.peer ;
            break ;

            // -- O_I Variable Part(No Body) --

        case m_UNPUBLISH_OBJECT_CLASS:
        case m_UNSUBSCRIBE_OBJECT_CLASS:
            objectClass = Header.VP.O_I.handle ;
            break ;

        case m_PUBLISH_INTERACTION_CLASS:
        case m_UNPUBLISH_INTERACTION_CLASS:
        case m_SUBSCRIBE_INTERACTION_CLASS:
        case m_UNSUBSCRIBE_INTERACTION_CLASS:
        case m_TURN_INTERACTIONS_ON:
        case m_TURN_INTERACTIONS_OFF:
            interactionClass = Header.VP.O_I.handle ;
            break ;

            // DDM variable part
        case m_DELETE_REGION:
            region = Header.VP.ddm.region ;
            break ;

            // -- O_I Variable Part(Body not empty) --

        case m_PUBLISH_OBJECT_CLASS:
        case m_SUBSCRIBE_OBJECT_CLASS:
            objectClass = Header.VP.O_I.handle ;
            handleArraySize = Header.VP.O_I.size ;
            break ;

        case m_REGISTER_OBJECT:
        case m_DISCOVER_OBJECT:
            objectClass = Header.VP.O_I.handle ;
            break ;

            // -- Default Handler --

        default:
            D.Out(pdExcept, "Unknown type %d in ReadHeader.", Header.type);
            throw RTIinternalError("Received unknown Header type.");
        }

    // 4- If Header.bodySize is not 0, return RTI_TRUE, else RTI_FALSE

    if (Header.bodySize == 0)
        return RTI_FALSE ;
    else
        return RTI_TRUE ;
}


// ---------------
// -- ReadLabel --
// ---------------

void
NetworkMessage::readLabel(MessageBody *Body)
{
    Body->readString(label, MAX_USER_TAG_LENGTH);
}

// ---------------------------------------------------------------------------
//! Read the tag contained into the message.
void NetworkMessage::readTag(MessageBody *Body)
{
    Body->readString(tag, MAX_USER_TAG_LENGTH);
}

// ---------------------------------------------------------------------------
//! Read the federation name.
void
NetworkMessage::readFederationName(MessageBody *Body)
{
    Body->readString(federationName, MAX_FEDERATION_NAME_LENGTH);
}


// -------------------
// -- ReadNomFedere --
// -------------------

void
NetworkMessage::readFederateName(MessageBody *Body)
{
    Body->readString(federateName, MAX_FEDERATE_NAME_LENGTH);
}


// -----------
// -- Write --
// -----------

void
NetworkMessage::write(Socket *socket)
    throw (NetworkError,
           NetworkSignal)
{

#ifdef USE_HEADER_AND_BODY
    Boolean Result ;
    // 1- Call WriteHeader
    Result = writeHeader(socket);

    // 2- If WriteHeader returned RTI_TRUE, call WriteBody.
    if (Result == RTI_TRUE)
        writeBody(socket);

#else
    socket->send(this, sizeof(NetworkMessage));
    //socket->send((NetworkMessage)this, sizeof(NetworkMessage));
#endif

}


// ---------------
// -- WriteBody --
// ---------------

void
NetworkMessage::writeBody(Socket *socket)
{
    MessageBody Body ;
    unsigned short i ;

    // 0- Copy the Header at the beginning of the Body, in order to
    // make a single Socket->Emettre call while sending both.
    // WARNING: As the Body size is not known yet, we will have to
    // change it in the copy also!
    Body.writeBlock((char *) &Header, sizeof(HeaderStruct));

    // 1- Prepare Body Structure according to Message type
    if ((Header.type == m_UPDATE_ATTRIBUTE_VALUES)
        || (Header.type == m_REFLECT_ATTRIBUTE_VALUES)) {
        Body.writeLongInt(object);
        Body.writeString(label);
        Body.writeBlock((char *) handleArray,
                        handleArraySize * sizeof(AttributeHandle));

        for (i = 0 ; i < handleArraySize ; i ++) {
            Body.writeString(ValueArray[i]);
        }
    }
    else
        switch(Header.type) {

            // -- O_I Variable Part With date(Body Not Empty) --

        case m_SEND_INTERACTION:
        case m_RECEIVE_INTERACTION:
            Body.writeString(label);
            Body.writeBlock((char *) handleArray,
                            handleArraySize * sizeof(AttributeHandle));
            for (i = 0 ; i < handleArraySize ; i ++) {
                Body.writeString(ValueArray[i]);
            }
            break ;

            // -- No Variable Part --

        case m_CREATE_FEDERATION_EXECUTION:
        case m_DESTROY_FEDERATION_EXECUTION:
            Body.writeString(federationName);
            break ;

        case m_REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
        case m_ANNOUNCE_SYNCHRONIZATION_POINT:
            Body.writeString(label);
            Body.writeString(tag);
            break ;

        case m_SYNCHRONIZATION_POINT_ACHIEVED:
        case m_SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
        case m_FEDERATION_SYNCHRONIZED:
            Body.writeString(label);
            break ;

        case m_DELETE_OBJECT:
        case m_REMOVE_OBJECT:
            Body.writeLongInt(object);
            Body.writeString(label);
            break ;

        case m_IS_ATTRIBUTE_OWNED_BY_FEDERATE:
        case m_INFORM_ATTRIBUTE_OWNERSHIP:
        case m_ATTRIBUTE_IS_NOT_OWNED:
        case m_QUERY_ATTRIBUTE_OWNERSHIP:
            Body.writeLongInt(object);
            Body.writeShortInt(handleArray[0]);
            Body.writeString(label);
            break ;

        case m_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        case m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
        case m_ATTRIBUTE_OWNERSHIP_ACQUISITION:
        case m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
            Body.writeLongInt(object);
            Body.writeShortInt(handleArraySize);
            for (i = 0 ; i < handleArraySize ; i ++)
                Body.writeShortInt(handleArray[i]);
            Body.writeString(label);
            break ;


        case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
        case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
        case m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
        case m_UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        case m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
        case m_CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        case m_ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
        case m_CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
        case m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
            Body.writeLongInt(object);
            Body.writeShortInt(handleArraySize);
            for (i = 0 ; i < handleArraySize ; i ++)
                Body.writeShortInt(handleArray[i]);
            break ;

        case m_CREATE_REGION:
            Body.writeLongInt(space);
            Body.writeLongInt(nbExtents);
            Body.writeLongInt(region);
            break ;

            // -- Join Variable Part --

        case m_JOIN_FEDERATION_EXECUTION:
            Body.writeString(federationName);
            Body.writeString(federateName);
            break ;

            // -- O_I Variable Part(Body not empty) --

        case m_PUBLISH_OBJECT_CLASS:
        case m_SUBSCRIBE_OBJECT_CLASS:
            for (i = 0 ; i < handleArraySize ; i ++)
                Body.writeShortInt(handleArray[i]);
            break ;

        case m_REGISTER_OBJECT:
        case m_DISCOVER_OBJECT:
            Body.writeLongInt(object);
            Body.writeString(label);
            break ;

            // -- Default Handler --

        default:
            D.Out(pdExcept, "Unknown type %d in WriteBody.", Header.type);
            throw RTIinternalError("Unknown/Unimplemented type for Header.");

        }

    // 2- Set Header.bodySize

    // Body Size does not include the copy of the Header!
    Header.bodySize = Body.getLength() - sizeof(HeaderStruct);


    // Put the real Body Size in the copy of the Header.
    ((HeaderStruct *) Body.getBuffer())->bodySize = Header.bodySize ;

    // 3- Write Header to socket, then write Body to socket.
    // socket->send((void *) &Header, sizeof(HeaderStruct));
    socket->send((void *) Body.getBuffer(), Body.getLength());
}


// -----------------
// -- WriteHeader --
// -----------------

Boolean
NetworkMessage::writeHeader(Socket *socket)
{
    // 1- Clear Header
    // memset((void *) &Header, '\0', sizeof(HeaderStruct));

    // 2- Fill Header(Static Part)
    Header.type = type ;
    Header.exception = exception ;
    Header.federate = federate ;
    Header.federation = federation ;

    // 3- Fill Header(Variable Part)[Sorted by Variable part type]

    // Note: Header.bodySize is not set to the actual Body size, but
    // to zero to indicate there is no Body, or 1 if a Body is needed.

    if (type == m_MESSAGE_NULL) {
        Header.bodySize = 0 ;
        Header.VP.time.date = date ;
    }
    else if ((type == m_UPDATE_ATTRIBUTE_VALUES)
             || (type == m_REFLECT_ATTRIBUTE_VALUES)) {
        // Body contains ObjectHandle, handleArray, ValueArray, label
        Header.bodySize = 1 ;
        Header.VP.O_I.handle = objectClass ;
        Header.VP.O_I.size = handleArraySize ;
        Header.VP.O_I.date = date ;
    }
    else
        switch(type) {


        case m_SEND_INTERACTION:
        case m_RECEIVE_INTERACTION:
            // Body contains handleArray, ValueArray, label.
            Header.bodySize = 1 ;
            Header.VP.O_I.handle = interactionClass ;
            Header.VP.O_I.size = handleArraySize ;
            Header.VP.O_I.date = date ;
            break ;

            // -- No Variable Part, No Body --

        case m_CLOSE_CONNEXION:
        case m_RESIGN_FEDERATION_EXECUTION:
            Header.bodySize = 0 ;
            break ;

            // -- No Variable Part, Body not empty --

        case m_CREATE_FEDERATION_EXECUTION:
        case m_DESTROY_FEDERATION_EXECUTION:
            // Body Contains federationName.
        case m_INFORM_ATTRIBUTE_OWNERSHIP:
        case m_ATTRIBUTE_IS_NOT_OWNED:
        case m_IS_ATTRIBUTE_OWNED_BY_FEDERATE:
        case m_QUERY_ATTRIBUTE_OWNERSHIP:
            // Body Contains ObjectHandle and label
        case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
        case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
        case m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
            // Body Contains ObjectHandle and handleArray
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
        case m_CREATE_REGION:
            Header.bodySize = 1 ;
            break ;

        case m_REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
        case m_SYNCHRONIZATION_POINT_ACHIEVED:
        case m_SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
        case m_FEDERATION_SYNCHRONIZED:
        case m_ANNOUNCE_SYNCHRONIZATION_POINT:
            // Body Contains Label(should be non-empty)
            // BUG: S'il fait moins de 16 octet, il passe dans le header.
            Header.bodySize = 1 ;
            break ;

        case m_DELETE_OBJECT:
        case m_REMOVE_OBJECT:
            // Body Contains ObjectHandle, and label
            Header.bodySize = 1 ;
            break ;

            // -- time Variable Part(No Body)[Continued] --

        case m_SET_TIME_REGULATING:
            Header.bodySize = 0 ;
            Header.VP.time.date = date ;
            Header.VP.time.R_or_C = regulator ;
            break ;

        case m_SET_TIME_CONSTRAINED:
            Header.bodySize = 0 ;
            Header.VP.time.date = date ;
            Header.VP.time.R_or_C = constrained ;
            break ;

            // -- ReqID Variable Part(No Body) --

        case m_REQUEST_ID:
            Header.bodySize = 0 ;
            Header.VP.ReqID.count = idCount ;
            Header.VP.ReqID.first = firstId ;
            Header.VP.ReqID.last = lastId ;
            break ;

            // -- T_O Variable Part --

        case m_CHANGE_ATTRIBUTE_TRANSPORT_TYPE:
        case m_CHANGE_ATTRIBUTE_ORDER_TYPE:
        case m_CHANGE_INTERACTION_TRANSPORT_TYPE:
        case m_CHANGE_INTERACTION_ORDER_TYPE:
            throw RTIinternalError("Write Message not implemented for T/O.");
            break ;

            // -- Join Variable Part --

        case m_JOIN_FEDERATION_EXECUTION:
            // Body contains federationName and federateName
            Header.bodySize = 1 ;
            Header.VP.Join.NbReg = numberOfRegulators ;
            Header.VP.Join.AdrMC = multicastAddress ;
            Header.VP.Join.Addr = bestEffortAddress ;
            Header.VP.Join.peer = bestEffortPeer ;
            break ;

            // -- O_I Variable Part(No Body) --

        case m_UNPUBLISH_OBJECT_CLASS:
        case m_UNSUBSCRIBE_OBJECT_CLASS:
            Header.bodySize = 0 ;
            Header.VP.O_I.handle = objectClass ;
            break ;

        case m_PUBLISH_INTERACTION_CLASS:
        case m_UNPUBLISH_INTERACTION_CLASS:
        case m_SUBSCRIBE_INTERACTION_CLASS:
        case m_UNSUBSCRIBE_INTERACTION_CLASS:
        case m_TURN_INTERACTIONS_ON:
        case m_TURN_INTERACTIONS_OFF:
            Header.bodySize = 0 ;
            Header.VP.O_I.handle = interactionClass ;
            break ;

            // DDM variable part, no body
        case m_DELETE_REGION:
            Header.bodySize = 0 ;
            Header.VP.ddm.region = region ;
            break ;

            // -- O_I Variable Part(Body not empty) --

        case m_PUBLISH_OBJECT_CLASS:
        case m_SUBSCRIBE_OBJECT_CLASS:
            // Body contains handleArray[handleArraySize](if not empty)

            if (handleArraySize > 0)
                Header.bodySize = 1 ;
            else
                Header.bodySize = 0 ;

            Header.VP.O_I.handle = objectClass ;
            Header.VP.O_I.size = handleArraySize ;
            break ;

        case m_REGISTER_OBJECT:
        case m_DISCOVER_OBJECT:
            // Body Contains ObjectHandle and label
            Header.bodySize = 1 ;
            Header.VP.O_I.handle = objectClass ;
            break ;

            // -- Default Handler --

        default:
            D.Out(pdExcept, "Unknown type %d in WriteHeader.", Header.type);
            throw RTIinternalError("Unknown/Unimplemented type for Header.");

        }

    // 4- If Header.bodySize = 0, send message and return RTI_FALSE,
    // Else send nothing(will be done by WriteBody), and return RTI_TRUE.

    if (Header.bodySize == 0) {
        socket->send((void *) &Header, sizeof(HeaderStruct));
        return RTI_FALSE ;
    }
    else
        return RTI_TRUE ;

}

} // namespace certi

// $Id: NetworkMessage_RW.cc,v 3.5 2003/04/09 16:41:10 breholee Exp $
