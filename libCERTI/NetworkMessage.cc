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
// $Id: NetworkMessage.cc,v 3.6 2003/06/27 17:26:29 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "NetworkMessage.hh"

#include "PrettyDebug.hh"

#include <cstdio>
#include <cstring>

namespace certi {

// -------------
// -- Affiche --
// -------------

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

// --------------------
// -- NetworkMessage --
// --------------------

NetworkMessage::NetworkMessage()
{
    type = NOT_TYPED ;
    exception = e_NO_EXCEPTION ;

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


// ---------------------
// -- ~NetworkMessage --
// ---------------------

NetworkMessage::~NetworkMessage()
{
}


// -------------------------
// -- GetAttribValueArray --
// -------------------------

AttributeValue *
NetworkMessage::getAttribValueArray()
{
    int i ;
    AttributeValue *NewValueArray = NULL ;

    NewValueArray = (AttributeValue *) calloc(handleArraySize,
                                              sizeof(AttributeValue));

    if (NewValueArray == NULL)
        throw RTIinternalError("No memory.");

    for (i = 0 ; i <handleArraySize ; i++)
        getValue(i, NewValueArray[i]);

    return NewValueArray ;
}


// ------------------------
// -- GetParamValueArray --
// ------------------------

ParameterValue *
NetworkMessage::getParamValueArray()
{
    int i ;
    ParameterValue *NewValueArray = NULL ;

    NewValueArray = (ParameterValue *) calloc(handleArraySize,
                                              sizeof(ParameterValue));

    if (NewValueArray == NULL)
        throw RTIinternalError("No memory.");

    for (i = 0 ; i < handleArraySize ; i++)
        getValue(i, NewValueArray[i]);

    return NewValueArray ;
}


// --------------
// -- GetValue --
// --------------

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


// ---------------------
// -- RemoveAttribute --
// ---------------------

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


// ---------------------
// -- RemoveParameter --
// ---------------------

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


// --------------
// -- SetValue --
// --------------

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

}

// $Id: NetworkMessage.cc,v 3.6 2003/06/27 17:26:29 breholee Exp $
