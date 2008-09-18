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
// $Id: NetworkMessage.cc,v 3.37 2008/09/18 14:41:29 gotthardp Exp $
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
    valueArray.empty();

} /* end of NetworkMessage() */

NetworkMessage::~NetworkMessage() {
	
}

// ----------------------------------------------------------------------------
std::vector <AttributeValue_t> 
NetworkMessage::getAttribValueArray()
{
    std::vector <std::string> NewValueArray ;

    NewValueArray.resize(handleArraySize) ;

    for (int i = 0 ; i <handleArraySize ; i++)
        NewValueArray[i] = valueArray[i] ;

    return NewValueArray ;
}

// ----------------------------------------------------------------------------
std::vector <ParameterValue_t>
NetworkMessage::getParamValueArray()
{
    std::vector <std::string> NewValueArray ;

    NewValueArray.resize(handleArraySize) ;

    for (int i = 0 ; i < handleArraySize ; i++)
        NewValueArray[i] = valueArray[i] ;

    return NewValueArray ;
}

// ----------------------------------------------------------------------------
void
NetworkMessage::removeAttribute(UShort Rank)
{
    if (Rank >= valueArray.size())
        throw RTIinternalError("Bad Rank while removing Nmessage attribute.");

    handleArray.erase(handleArray.begin() + Rank);
    valueArray.erase(valueArray.begin() + Rank);

    handleArraySize -- ;
}

// ----------------------------------------------------------------------------
void
NetworkMessage::removeParameter(UShort Rank)
{
    if (Rank >= valueArray.size())
        throw RTIinternalError("Bad Rank while removing Nmessage parameter.");

    handleArray.erase(handleArray.begin() + Rank);
    valueArray.erase(valueArray.begin() + Rank);

    handleArraySize -- ;
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
valueArray.resize(size) ;
}
// ----------------------------------------------------------------------------
void
NetworkMessage::displayValueArray(char *titre)
{
    printf("(%s) valueArray size=%d\n",titre,(int)valueArray.size());
    for (int i=0; i<(int)valueArray.size();i++)
    {
        printf("%d : length=%d : value=",i,(int)(valueArray[i].length()));

        for(int k=0; k<(int)valueArray[i].length(); k++)
            if (isprint(valueArray[i][k]) == 0 )
                printf(" %x",valueArray[i][k]);
            else
                printf("%c",valueArray[i][k]);

        printf("\n");
    }
}

} // namespace certi

// $Id: NetworkMessage.cc,v 3.37 2008/09/18 14:41:29 gotthardp Exp $
