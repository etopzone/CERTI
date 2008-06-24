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
// $Id: NetworkMessage.cc,v 3.36 2008/06/24 08:56:49 rousse Exp $
// ----------------------------------------------------------------------------



#include "NetworkMessage.hh"
#include "PrettyDebug.hh"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>

using std::vector ;

namespace certi {
static PrettyDebug G("GENDOC",__FILE__);

// ----------------------------------------------------------------------------
NetworkMessage::NetworkMessage()
    : exception(e_NO_EXCEPTION),
      type(NOT_USED),
      _isDated(false), 
      _isLabelled(false), 
      _isTagged(false)
{
    name               = std::string("NetworkMessage (generic)");
    exceptionReason    = std::string("") ;
    number             = 0 ;

    federation         = 0 ;
    federate           = 0 ;

    federationName = std::string("") ;
    federateName   = std::string("") ;
        
    label          = std::string("") ;
    FEDid          = std::string("") ;

    bestEffortPeer = -1 ;
    bestEffortAddress = 0 ;
    
    date             = 0.0;

    object           = 0 ;
    objectClass      = 0 ;
    interactionClass = 0 ;

    handleArraySize  = 0 ;
    ValueArray.empty();

} /* end of NetworkMessage() */

NetworkMessage::~NetworkMessage() {
	
}

// ----------------------------------------------------------------------------
std::vector <ValueLengthPair> 
NetworkMessage::getAttribValueArray()
{

    std::vector <ValueLengthPair> NewValueArray ;
    

    NewValueArray.resize(handleArraySize) ;

    for (int i = 0 ; i <handleArraySize ; i++)
        {
        NewValueArray[i].length = ValueArray[i].length ;
        NewValueArray[i].value = new char[NewValueArray[i].length] ;
        memcpy(NewValueArray[i].value,ValueArray[i].value,NewValueArray[i].length);
        }

    return NewValueArray ;
}

// ----------------------------------------------------------------------------
std::vector <ParameterLengthPair>
NetworkMessage::getParamValueArray()
{
    std::vector <ParameterLengthPair> NewValueArray ;

    NewValueArray.resize(handleArraySize) ;


    for (int i = 0 ; i < handleArraySize ; i++)
        {
        NewValueArray[i].length = ValueArray[i].length ;
        NewValueArray[i].value = new char[NewValueArray[i].length] ;
        memcpy(NewValueArray[i].value,ValueArray[i].value,NewValueArray[i].length);
        }

    return NewValueArray ;
}

// ----------------------------------------------------------------------------
char *
NetworkMessage::getValue(int Rank, unsigned long *length, char *Value)
    throw (RTIinternalError)
{
    // Pre-Checking
    if ((Rank < 0) || (Rank >= (int)ValueArray.size()))
        throw RTIinternalError("(getValue) Bad Rank in NMessage.");

    // Getting Value
    // First, the length of the Value
    *length = ValueArray[Rank].length ;

    if (Value != NULL) 
        // Value exists, we copy it with memcpy instead of strcpy
        {
        memcpy(Value, (char *)*(ValueArray[Rank].value), *length);
        return NULL ;
        }
    else
        // Value doesn't exist, so create it then copy
        {
        char *TempValue = new char [*length] ;
        memcpy(TempValue,(char *)*(ValueArray[Rank].value), *length) ;
        Value = TempValue ;
        return Value ;
        }
}

// ----------------------------------------------------------------------------
void
NetworkMessage::removeAttribute(UShort Rank)
{
    UShort AttribIndex = 0 ;

    if (Rank >= ValueArray.size())
        throw RTIinternalError("Bad Rank while removing Nmessage attribute.");

    // Shift Attribute Handles and Values
    for (AttribIndex = Rank ; AttribIndex < handleArraySize - 1 ; AttribIndex ++) {
        handleArray[AttribIndex] = handleArray[AttribIndex + 1] ;
        ValueArray[AttribIndex].length = ValueArray[AttribIndex + 1].length ;
        ValueArray[AttribIndex].value = ValueArray[AttribIndex + 1].value ;
    }

    handleArraySize -- ;
}

// ----------------------------------------------------------------------------
void
NetworkMessage::removeParameter(UShort Rank)
{
    UShort ParamIndex = 0 ;

    if (Rank >= ValueArray.size())
        throw RTIinternalError("Bad Rank while removing Nmessage parameter.");

    // Shift Parameter Handles and Values
    for (ParamIndex = Rank ; ParamIndex < handleArraySize - 1 ; ParamIndex ++) {
        handleArray[ParamIndex] = handleArray[ParamIndex + 1] ;
        ValueArray[ParamIndex].length = ValueArray[ParamIndex + 1].length ;
        ValueArray[ParamIndex].value = ValueArray[ParamIndex + 1].value ;
    }

    handleArraySize -- ;
}

// ----------------------------------------------------------------------------
// Store Value into ValueArray[Rank)
void
NetworkMessage::setValue(int Rank, const char *Value, unsigned long length)
    throw (RTIinternalError)
{
    // Pre-Checking
    if (Value == NULL)
        throw RTIinternalError("Bad Value (NULL) for Network message.");

    if ((Rank < 0) || (Rank >= (int)ValueArray.size() ))
        throw RTIinternalError("(setValue) Bad Rank for Nmessage.");

    // Setting Value
    // First we store the length
    ValueArray[Rank].length = length ;

    // then copy Value address into ValueArray
    char *tempValue = new char[length] ;
    memcpy(tempValue, Value, length);
    ValueArray[Rank].value = (char *)tempValue ;

}

// ----------------------------------------------------------------------------
void
NetworkMessage::setAHS(const std::vector <AttributeHandle> &attr, int size)
{
    handleArraySize = size ;
    handleArray.resize(size) ;
    for (int i = 0 ; i < size ; ++i) {
        handleArray[i] = attr[i] ;
    }
}
// ----------------------------------------------------------------------------
void
NetworkMessage::sizeValueArray(int size)
{
ValueArray.resize(size) ;
}
// ----------------------------------------------------------------------------
void
NetworkMessage::displayValueArray(char *titre)
{
printf("(%s) ValueArray size=%d\n",titre,(int)ValueArray.size());
for (int i=0; i<(int)ValueArray.size();i++)
   {
   printf("%d : length=%d : value=",i,(int)(ValueArray[i].length));
   for (int k=0; k<(int)ValueArray[i].length ;k++)
      if (isprint(ValueArray[i].value[k]) == 0 )
         {
         printf(" %x",ValueArray[i].value[k]);
         }
      else
         {
         printf("%c",ValueArray[i].value[k]);
         }
printf("\n");
   }
}

} // namespace certi

// $Id: NetworkMessage.cc,v 3.36 2008/06/24 08:56:49 rousse Exp $
