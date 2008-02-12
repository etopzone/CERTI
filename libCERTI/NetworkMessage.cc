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
// $Id: NetworkMessage.cc,v 3.26 2008/02/12 14:26:43 rousse Exp $
// ----------------------------------------------------------------------------



#include "NetworkMessage.hh"
#include "PrettyDebug.hh"

#include <cstdio>
#include <cstring>
#include <cassert>

using std::vector ;

namespace certi {
static PrettyDebug G("GENDOC",__FILE__);
// ----------------------------------------------------------------------------
void
NetworkMessage::display(const char *s)
{
    printf(" -- NETWORK MESSAGE - %s -", s);
    if ( type == CREATE_FEDERATION_EXECUTION )
        printf("CREATE_FEDERATION_EXECUTION : federationName %s : filename %s :"
               " federateHandle %d : federationHandle %d\n",federationName,FEDid,federate,federation);
    else if (type == JOIN_FEDERATION_EXECUTION )
        printf("JOIN_FEDERATION_EXECUTION : federation %s : federate %s \n",federationName,federateName);
    else if (type == GET_FED_FILE ) 
        printf("GET_FED_FILE : number %ld\n",number) ;
    else
        printf(" type = %d\n", type);
        
    printf(" number = %ld-%d", federate, number);
    printf(" date = %f ", date);
    printf(" exception = %d ", exception);
    printf(" ObjectHandle = %ld\n", objectClass);
    printf(" interactionClass= %ld\n", interactionClass);
    printf(" object = %ld\n", object);
    printf(" handleArraySize = %d\n", handleArraySize);
}

// ----------------------------------------------------------------------------
NetworkMessage::NetworkMessage()
    : type(NOT_USED), exception(e_NO_EXCEPTION)
{
    exceptionReason[0] = '\0' ;

    number = 0 ;

    federation = 0 ;
    federate = 0 ;

    federationName = NULL ;
    federateName[0] = '\0' ;
    label[0] = '\0' ;
    FEDid = NULL ;

    bestEffortPeer = -1 ;
    bestEffortAddress = 0 ;

    // Clear Header
    memset((void *) &Header, '\0', sizeof(Header));

    date = 0.0 ;

    object = 0 ;
    objectClass = 0 ;
    interactionClass = 0 ;

    handleArraySize = 0 ;

    for ( int i=0 ; i<MAX_ATTRIBUTES_PER_CLASS ; i++ )
        {
        ValueArray[i].length = 0 ;
        ValueArray[i].value[0]  = '\0' ;
        }

}

// ----------------------------------------------------------------------------
ValueLengthPair *
NetworkMessage::getAttribValueArray()
{

    ValueLengthPair *NewValueArray = NULL ;

    NewValueArray = (ValueLengthPair *) calloc(handleArraySize, sizeof(ValueLengthPair));
    
    unsigned long length ;

    if (NewValueArray == NULL)
        throw RTIinternalError("No memory.");

    for (int i = 0 ; i <handleArraySize ; i++)
        {
        getValue(i, &length, NewValueArray[i].value);
        NewValueArray[i].length = length ;
        }

    return NewValueArray ;
}

// ----------------------------------------------------------------------------
ParameterLengthPair *
NetworkMessage::getParamValueArray()
{
    ParameterLengthPair *NewValueArray = NULL ;

    NewValueArray = (ParameterLengthPair *) calloc(handleArraySize, sizeof(ParameterLengthPair));

    if (NewValueArray == NULL)
        throw RTIinternalError("No memory.");

    unsigned long length ;

    for (int i = 0 ; i < handleArraySize ; i++)
        {
        getValue(i, &length, NewValueArray[i].value);
        NewValueArray[i].length = length ;
        }

    return NewValueArray ;
}

// ----------------------------------------------------------------------------
char *
NetworkMessage::getValue(int Rank, unsigned long *length, char *Value)
    throw (RTIinternalError)
{
    // Pre-Checking
    if ((Rank < 0) || (Rank >= handleArraySize))
        throw RTIinternalError("Bad Rank in Message.");

    // Getting Value
    // First, the length of the Value
    *length = ValueArray[Rank].length ;
    if (Value != NULL) 
        // Value exists, we copy it with memcpy instead of strcpy
        {
        memcpy(Value, ValueArray[Rank].value, *length);
        return NULL ;
        }
    else
        // Value doesn't exist, so create it then copy
        {
        char *Value = new char [*length] ;
        memcpy(Value,ValueArray[Rank].value, *length) ;
        return Value ;
        }
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
        ValueArray[AttribIndex].length = ValueArray[AttribIndex + 1].length ;
        memcpy(ValueArray[AttribIndex].value,ValueArray[AttribIndex + 1].value,
               ValueArray[AttribIndex + 1].length) ;
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
        ValueArray[ParamIndex].length = ValueArray[ParamIndex + 1].length ;
        memcpy(ValueArray[ParamIndex].value,ValueArray[ParamIndex + 1].value,
               ValueArray[ParamIndex + 1].length) ;
    }

    handleArraySize -- ;
}

// ----------------------------------------------------------------------------
void
NetworkMessage::setValue(int Rank, const char *Value, unsigned long length)
    throw (RTIinternalError)
{
    // Pre-Checking
    if ((Value == NULL) || (length > MAX_BYTES_PER_VALUE))
        throw RTIinternalError("Bad Value for message.");

    if ((Rank < 0) || (Rank >= handleArraySize))
        throw RTIinternalError("Bad Rank for message.");

    // Setting Value
    // First we store the length, then copy Value with memcpy instead of strcpy
    ValueArray[Rank].length = length ;
    memcpy(ValueArray[Rank].value, Value, length);

}

// ----------------------------------------------------------------------------
/** read a NetworkMessage
    First read Header and upon readHeader result read or not the Body with readBody
    @param socket 
*/
void
NetworkMessage::read(Socket *socket)
    throw (NetworkError, NetworkSignal)
{
    // G.Out(pdGendoc,"enter NetworkMessage::read");
    bool has_body = readHeader(socket);

    if (has_body)
        readBody(socket);
    // G.Out(pdGendoc,"exit  NetworkMessage::read");
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
/** Store a boolean into a NetworkMessage
    @param[in] the_bool boolean
*/
void
NetworkMessage::setBoolean(bool the_bool)
{
    boolean = the_bool ;
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
    short federationNameSize ;

    federationNameSize = body.readShortInt() ;
    federationName = new char[federationNameSize+1] ;
    if ( federationNameSize == 0 )
        {
        federationName[0] = '\0' ;
        }
    else
        body.readString(federationName,federationNameSize);    
}

// ----------------------------------------------------------------------------
void
NetworkMessage::readFederateName(MessageBody &body)
{
    body.readString(federateName, MAX_FEDERATE_NAME_LENGTH);
}

// ----------------------------------------------------------------------------
// setFEDid
//
void
NetworkMessage::setFEDid(const char *NewFEDid)
{
    assert ( NewFEDid != NULL) ;
    FEDid = new char[strlen(NewFEDid)+1] ;
    strcpy(FEDid, NewFEDid);
}

// ----------------------------------------------------------------------------
void
NetworkMessage::readFEDid(MessageBody &body)
{
    short FEDidSize ;
    FEDidSize = body.readShortInt() ;
    FEDid = new char[FEDidSize+1] ;
    if ( FEDidSize == 0 )
        FEDid[0] = '\0' ;
    else
        body.readString(FEDid,FEDidSize);
}

// ---------------------------------------------------------------
void
NetworkMessage::writeFEDid(MessageBody &body)
{
    body.writeShortInt(strlen(FEDid));
    body.writeString(FEDid);
}

// ---------------------------------------------------------------
void
NetworkMessage::writeFederationName(MessageBody &body)
{ 
    assert(federationName != NULL) ;  
    body.writeShortInt(strlen(federationName));
    body.writeString(federationName);
}

} // namespace certi

// $Id: NetworkMessage.cc,v 3.26 2008/02/12 14:26:43 rousse Exp $
