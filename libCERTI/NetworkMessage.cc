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
// $Id: NetworkMessage.cc,v 3.12 2005/05/16 08:31:05 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "NetworkMessage.hh"

#include "PrettyDebug.hh"

#include <cstdio>
#include <cstring>

using std::vector ;

namespace certi {

// ----------------------------------------------------------------------------
void
NetworkMessage::display(const char *s)
{
    printf(" --- MESSAGE RESEAU --- %s ---\n", s);
    printf(" type = %d\n", type);
    printf(" number = %ld-%d\n", federate, number);
    printf(" date = %f\n", date);
    printf(" exception = %d\n", exception);
    printf(" ObjectHandle = %ld\n", objectClass);
    printf(" interactionClass= %ld\n", interactionClass);
    printf(" object = %ld\n", object);
    printf(" handleArraySize = %d\n", handleArraySize);
}

// ----------------------------------------------------------------------------
NetworkMessage::NetworkMessage()
    : type(NOT_USED), exception(e_NO_EXCEPTION)
{
    number = 0 ;

    federation = 0 ;
    federate = 0 ;

    federationName[0] = '\0' ;
    federateName[0] = '\0' ;
    label[0] = '\0' ;

    bestEffortPeer = -1 ;
    bestEffortAddress = 0 ;

    // Clear Header
    memset((void *) &Header, '\0', sizeof(Header));

    date = 0.0 ;

    object = 0 ;
    objectClass = 0 ;
    interactionClass = 0 ;

    handleArraySize = 0 ;
}

// ----------------------------------------------------------------------------
AttributeValue *
NetworkMessage::getAttribValueArray()
{
    AttributeValue *NewValueArray = NULL ;

    NewValueArray = (AttributeValue *) calloc(handleArraySize, sizeof(AttributeValue));

    if (NewValueArray == NULL)
        throw RTIinternalError("No memory.");

    for (int i = 0 ; i <handleArraySize ; i++)
        getValue(i, NewValueArray[i]);

    return NewValueArray ;
}

// ----------------------------------------------------------------------------
ParameterValue *
NetworkMessage::getParamValueArray()
{
    ParameterValue *NewValueArray = NULL ;

    NewValueArray = (ParameterValue *) calloc(handleArraySize, sizeof(ParameterValue));

    if (NewValueArray == NULL)
        throw RTIinternalError("No memory.");

    for (int i = 0 ; i < handleArraySize ; i++)
        getValue(i, NewValueArray[i]);

    return NewValueArray ;
}

// ----------------------------------------------------------------------------
char *
NetworkMessage::getValue(int Rank, char *Value)
    throw (RTIinternalError)
{
    // Pre-Checking
    if ((Rank < 0) || (Rank >= handleArraySize))
        throw RTIinternalError("Bad Rank in Message.");

    // Getting Value
    if (Value != NULL) {
        strcpy(Value, ValueArray[Rank]);
        return NULL ;
    } else
        return strdup(ValueArray[Rank]);
}

// ----------------------------------------------------------------------------
void
NetworkMessage::removeAttribute(UShort Rank)
{
    UShort AttribIndex = 0 ;

    if (Rank >= handleArraySize)
        throw RTIinternalError("Bad Rank while removing message attribute.");

    // Shift Attribute Handles and Values
    for (AttribIndex = Rank ; AttribIndex < handleArraySize - 1 ; AttribIndex ++) {
        handleArray[AttribIndex] = handleArray[AttribIndex + 1] ;
        strcpy(ValueArray[AttribIndex], ValueArray[AttribIndex + 1]);
    }

    handleArraySize -- ;
}

// ----------------------------------------------------------------------------
void
NetworkMessage::removeParameter(UShort Rank)
{
    UShort ParamIndex = 0 ;

    if (Rank >= handleArraySize)
        throw RTIinternalError("Bad Rank while removing message parameter.");

    // Shift Parameter Handles and Values
    for (ParamIndex = Rank ; ParamIndex < handleArraySize - 1 ; ParamIndex ++) {
        handleArray[ParamIndex] = handleArray[ParamIndex + 1] ;
        strcpy(ValueArray[ParamIndex], ValueArray[ParamIndex + 1]);
    }

    handleArraySize -- ;
}

// ----------------------------------------------------------------------------
void
NetworkMessage::setValue(int Rank, const char *Value)
    throw (RTIinternalError)
{
    // Pre-Checking

    if ((Value == NULL) || (strlen(Value) > MAX_BYTES_PER_VALUE))
        throw RTIinternalError("Bad Value for message.");

    if ((Rank < 0) || (Rank >= handleArraySize))
        throw RTIinternalError("Bad Rank for message.");

    // Setting Value
    strcpy(ValueArray[Rank], Value);
}

// ----------------------------------------------------------------------------
// read
void
NetworkMessage::read(Socket *socket)
    throw (NetworkError, NetworkSignal)
{
    bool has_body = readHeader(socket);

    if (has_body)
        readBody(socket);
}

// ----------------------------------------------------------------------------
void
NetworkMessage::write(Socket *socket)
    throw (NetworkError, NetworkSignal)
{
    bool needs_body = writeHeader(socket);

    if (needs_body)
        writeBody(socket);
}

// ----------------------------------------------------------------------------
void
NetworkMessage::setAHS(const AttributeHandle *attr, int size)
{
    handleArraySize = size ;

    for (int i = 0 ; i < size ; ++i) {
        handleArray[i] = attr[i] ;
    }
}

// ----------------------------------------------------------------------------
void
NetworkMessage::readLabel(MessageBody &body)
{
    body.readString(label, MAX_USER_TAG_LENGTH);
}

// ----------------------------------------------------------------------------
//! Read the tag contained into the message.
void NetworkMessage::readTag(MessageBody &body)
{
    body.readString(tag, MAX_USER_TAG_LENGTH);
}

// ----------------------------------------------------------------------------
//! Read the federation name.
void
NetworkMessage::readFederationName(MessageBody &body)
{
    body.readString(federationName, MAX_FEDERATION_NAME_LENGTH);
}

// ----------------------------------------------------------------------------
void
NetworkMessage::readFederateName(MessageBody &body)
{
    body.readString(federateName, MAX_FEDERATE_NAME_LENGTH);
}

} // namespace certi

// $Id: NetworkMessage.cc,v 3.12 2005/05/16 08:31:05 breholee Exp $
