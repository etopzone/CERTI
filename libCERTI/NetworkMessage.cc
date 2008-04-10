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
// $Id: NetworkMessage.cc,v 3.28.2.5 2008/04/10 11:35:57 erk Exp $
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
NetworkMessage::NetworkMessage()
    : type(NOT_USED), 
      exception(e_NO_EXCEPTION), 
      isDated(false), 
      isLabelled(false), 
      isTagged(false)
{
	name               = "NetworkMessage (generic)";
    exceptionReason[0] = '\0' ;
    number             = 0 ;

    federation         = 0 ;
    federate           = 0 ;

    federationName = std::string("") ;
    federateName[0] = '\0' ;
    label[0] = '\0' ;
    FEDid = std::string("") ;

    bestEffortPeer = -1 ;
    bestEffortAddress = 0 ;

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

NetworkMessage::~NetworkMessage() {
	
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


} // namespace certi

// $Id: NetworkMessage.cc,v 3.28.2.5 2008/04/10 11:35:57 erk Exp $
