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
// ----------------------------------------------------------------------------
// $Id



#include "Message.hh"
#include "certi.hh"
#include "PrettyDebug.hh"

#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <assert.h>

using std::vector ;

namespace certi {

static PrettyDebug G("GENDOC",__FILE__) ;

#define NONE -1

Message::Message()
{
    fed_time.setZero();
    exception = e_NO_EXCEPTION ;
    exceptionReason[0] = '\0' ;
    federateName[0] = '\0' ;
    //federationName = NULL ;is now a string then...
    tag = "" ;
    name[0] = '\0' ;
    label[0] = '\0' ;
    handleArraySize = 0 ;
    objectClass = 0 ; 
    object = 0 ;
    boolean = false ;
    type = NOT_USED ;
    lookahead = 0 ;
    federate = 0 ;
    resignAction = RTI::NO_ACTION ;
    idCount = 0 ;
    firstId = 0 ;
    lastId = 0 ;
    interactionClass = 0 ;
    attribute = 0 ;
    parameter = 0 ;
    transport = 0 ;
    order = 0 ;
    space = 0 ;
    dimension = 0 ;
    number = 0 ;
    region = 0 ;
    //FEDid = NULL ;now string
    valueArray.empty() ;
}

// ----------------------------------------------------------------------------
/** Return a newly allocated ValueArray, exactly of size HandleArraySize.
*/
std::vector <ParameterValue_t>
Message::getValueArray()
{
    G.Out(pdGendoc,"enter Message::getValueArray");
    int i ;

    std::vector <ParameterValue_t> NewValueArray;
    NewValueArray.resize(handleArraySize) ;

    for (i = 0 ; i < handleArraySize ; i++)
        {
        NewValueArray[i] = valueArray[i];
        }

    G.Out(pdGendoc,"return Message::getValueArray");
    return NewValueArray ;
}

// ----------------------------------------------------------------------------
/** set label into a Message
    @param NewLabel copied (strcpy) into Message label (NULL label is stored as an empty string)
*/
void
Message::setLabel(std::string NewLabel)
{
    label = NewLabel ;
}

// ----------------------------------------------------------------------------
void
Message::setName(const char *NewName)
{
   name = NewName ;
}

// ----------------------------------------------------------------------------
void
Message::setType(Type t)
{
    type = t ;
}
// ----------------------------------------------------------------------------
void
Message::setNumber(unsigned long nb)
{
    number = nb ;
}

// ----------------------------------------------------------------------------
void
Message::setSpace(SpaceHandle handle)
{
    space = handle ;
}

// ----------------------------------------------------------------------------
void
Message::setRegion(long handle)
{
    region = handle ;
}

// ----------------------------------------------------------------------------
void
Message::setMinTickTime(RTI::TickTime time)
{
    minTickTime = time;
}

// ----------------------------------------------------------------------------
void
Message::setMaxTickTime(RTI::TickTime time)
{
    maxTickTime = time;
}

// ----------------------------------------------------------------------------
void
Message::setDimension(DimensionHandle handle)
{
    dimension = handle ;
}

// ----------------------------------------------------------------------------
void
Message::setInteractionClass(InteractionClassHandle handle)
{
    interactionClass = handle ;
}

// ----------------------------------------------------------------------------
void
Message::setObjectClass(ObjectClassHandle handle)
{
    objectClass = handle ;
}

// ----------------------------------------------------------------------------
void
Message::setAttribute(AttributeHandle handle)
{
    attribute = handle ;
}

// ----------------------------------------------------------------------------
void
Message::setResignAction(RTI::ResignAction the_action)
{
    resignAction = the_action ;
}

// ----------------------------------------------------------------------------
void
Message::setFederationTimeDelta(FederationTimeDelta the_lookahead)
{
    lookahead = the_lookahead ;
}

// ----------------------------------------------------------------------------
/** Store FedTime into a Message
    @param the_time federation time
*/
void
Message::setFedTime(const double the_time)
{
    fed_time = the_time;
}

// ----------------------------------------------------------------------------
void
Message::setLookahead(const double the_lookahead)
{
    lookahead = the_lookahead;
}

// ----------------------------------------------------------------------------
void
Message::setFederationTime(FederationTime the_time)
{
    fed_time = the_time;
}

// ----------------------------------------------------------------------------
/** Store a boolean into a Message
    @param[in] the_bool boolean
*/
void
Message::setBoolean(bool the_bool)
{
    boolean = the_bool ;
}

// ----------------------------------------------------------------------------
/** Store ObjectHandle into a Message
    @param the_object object handle
*/
void
Message::setObject(ObjectHandle the_object)
{
    object = the_object ;
}

// ----------------------------------------------------------------------------
void
Message::setTransportation(TransportType the_transport)
{
    transport = the_transport;
}

// ----------------------------------------------------------------------------
void
Message::setOrdering(OrderType the_ordering)
{
    order = the_ordering;
}

// ----------------------------------------------------------------------------
void
Message::setEventRetraction(EventRetractionHandle the_event)
{
    eventRetraction = the_event ;
}

// ----------------------------------------------------------------------------
void
Message::setParameter(ParameterHandle the_parameter)
{
    parameter = the_parameter ;
}

// ----------------------------------------------------------------------------
/** Store exception into message
    @param the_exception : exception type (enum)
    @param the_reason : exception reason
*/
void
Message::setException(TypeException the_exception,
                      const std::string& the_reason)
{
    exception = the_exception ;
    exceptionReason = the_reason;
}

// ----------------------------------------------------------------------------
void
Message::setFederate(FederateHandle the_federate)
{
    federate = the_federate ;
}

// ----------------------------------------------------------------------------
const std::vector<AttributeHandle>&
Message::getAHS() const
{
    return handleArray;
}

// ----------------------------------------------------------------------------
void
Message::setAHS(const std::vector<AttributeHandle> &the_attributes)
{
    G.Out(pdGendoc,"enter Message::setAHS");
    handleArraySize = the_attributes.size();
    handleArray.resize(handleArraySize);

    for (unsigned int i = 0 ; i < the_attributes.size(); ++i) {
        handleArray[i] = the_attributes[i];
    }
    G.Out(pdGendoc,"exit  Message::setAHS");
}

// ----------------------------------------------------------------------------
void
Message::setAHS(const AttributeHandle *attr, int size)
{
    G.Out(pdGendoc,"enter Message::setAHS with size");
    handleArraySize = size ;
    handleArray.resize(handleArraySize);

    for (int i = 0 ; i < size ; ++i) {
        handleArray[i] = attr[i] ;
    }
    G.Out(pdGendoc,"exit  Message::setAHS with size");
}

// ----------------------------------------------------------------------------
std::vector<std::pair<AttributeHandle, AttributeValue_t> >
Message::getAHVPS() const
{
    std::vector<std::pair<AttributeHandle, AttributeValue_t> > result;
    result.resize(handleArraySize);

    for (int i = 0 ; i < handleArraySize ; i++) {
        result[i].first = handleArray[i];
        result[i].second.assign(valueArray[i].data(), valueArray[i].length());
    }

    return result;
}

// ----------------------------------------------------------------------------
/** store attribute set into handleArray and valueArray of a Message
    @param the_attributes attribute set
*/
void
Message::setAHVPS(const std::vector<std::pair<AttributeHandle, AttributeValue_t> > &the_attributes)
{
    G.Out(pdGendoc,"enter Message::setAHVPS");

    unsigned long size ;

    size = the_attributes.size() ;
    handleArraySize = size ;
    handleArray.resize(handleArraySize);
    valueArray.resize(size) ;

    for (unsigned long i = 0 ; i < size ; i++) {
        handleArray[i] = the_attributes[size-1-i].first;
        valueArray[i] = the_attributes[size-1-i].second;
    }
    G.Out(pdGendoc,"exit  Message::setAHVPS");
}

// ----------------------------------------------------------------------------
std::vector<std::pair<ParameterHandle, ParameterValue_t> >
Message::getPHVPS() const
{
    std::vector<std::pair<ParameterHandle, ParameterValue_t> > result;
    result.resize(handleArraySize);

    for (int i = 0 ; i < handleArraySize ; i++) {
        result[i].first = handleArray[i];
        result[i].second.assign(valueArray[i].data(), valueArray[i].length());
    }

    return result;
}

// ----------------------------------------------------------------------------
void
Message::setPHVPS(const std::vector<std::pair<ParameterHandle, ParameterValue_t> > &the_parameters)
{
    G.Out(pdGendoc,"enter Message::setPHVPS");

    unsigned long size ;

    size = the_parameters.size() ;
    handleArraySize = size ;
    handleArray.resize(handleArraySize);
    valueArray.resize(size) ;

    for (unsigned long i = 0 ; i < size ; i++) {
        handleArray[i] = the_parameters[size-1-i].first;
        valueArray[i] = the_parameters[size-1-i].second;
    }
    G.Out(pdGendoc,"exit  Message::setPHVPS");
}

// ----------------------------------------------------------------------------
void
Message::setAttributes(std::vector <AttributeHandle> &the_attributes, ushort the_size)
{
    handleArraySize = the_size ;
    handleArray.resize(handleArraySize);

    for (int i = 0 ; i < the_size ; i++) {
        handleArray[i] = the_attributes[i] ;
    }
}

// ----------------------------------------------------------------------------
// setAttributes
// store attributes into handleArray
// store values     into valueArray
void
Message::setAttributes(std::vector <AttributeHandle> &the_attributes,
                       std::vector <AttributeValue_t> &the_values,
                       ushort the_size)
{
    G.Out(pdGendoc,"enter Message::setAttributes");
    handleArraySize = the_size ;
    handleArray.resize(handleArraySize);
    valueArray.resize(the_size) ;

    for (int i = 0 ; i < the_size ; i++) {
        handleArray[i] = the_attributes[i] ;
        valueArray[i] = the_values[i] ;
    }
    G.Out(pdGendoc,"exit  Message::setAttributes");
}

// ----------------------------------------------------------------------------
// setParameters
// store parameters into handleArray
// store values     into valueArray
void
Message::setParameters(std::vector <ParameterHandle> & the_parameters,
                       std::vector <ParameterValue_t> & the_values,
                       ushort the_size)

{
    handleArraySize = the_size ;
    handleArray.resize(handleArraySize);
    valueArray.resize(the_size) ;

    for (int i = 0 ; i < the_size ; i++) {
        handleArray[i] = the_parameters[i] ;
        valueArray[i] = the_values[i] ;
    }
}

// ----------------------------------------------------------------------------
//! Sets the federation name.
void Message::setFederationName(std::string NewNomFederation)
{
    federationName = NewNomFederation ;
}


// ----------------------------------------------------------------------------
// setFederateName
//
void
Message::setFederateName(std::string NewNomFedere)
{
    federateName = NewNomFedere ;
}

// ----------------------------------------------------------------------------
// setTag
/** Store Tag into a Message
    @param new_tag tag
*/
void
Message::setTag(std::string new_tag)
{
    tag = new_tag ;
}

// ----------------------------------------------------------------------------
// setFEDid
//
void
Message::setFEDid(std::string NewFEDid)
{
    FEDid = NewFEDid;
}

// ----------------------------------------------------------------------------
// operator=
//
Message &
Message::operator=(const Message& msg)
{
    type = msg.type ;
    fed_time = msg.fed_time ;
    boolean = msg.boolean ;
    lookahead = msg.lookahead ;
    exception = msg.exception ;

    exceptionReason = msg.exceptionReason;
    federateName = msg.federateName ;

    federationName = msg.federationName ;

    federate = msg.federate ;
    resignAction = msg.resignAction ;
    idCount = msg.idCount ;
    firstId = msg.firstId ;
    lastId = msg.lastId ;

    objectClass = msg.objectClass ;
    interactionClass = msg.interactionClass ;
    attribute = msg.attribute ;
    parameter = msg.parameter ;
    object = msg.object ;
    space = msg.space ;
    dimension = msg.dimension ;
    // transportation = msg.transportation ;
    // ordering = msg.ordering ;
    minTickTime = msg.minTickTime ;
    maxTickTime = msg.maxTickTime ;

    tag = msg.tag ;
    name = msg.name ;

    transport = msg.transport ;
    order = msg.order ;

    eventRetraction.theSerialNumber = msg.eventRetraction.theSerialNumber ;
    eventRetraction.sendingFederate = msg.eventRetraction.sendingFederate ;

    label = msg.label ;

    handleArraySize = msg.handleArraySize ;
    handleArray.resize(handleArraySize);
    valueArray.resize(handleArraySize) ;

    int i ;
    for (i=0 ; i < handleArraySize ; i++)
        handleArray[i] = msg.handleArray[i] ;

    for (i=0 ; i < handleArraySize ; i++)
        valueArray[i] = msg.valueArray[i] ;

    FEDid = msg.FEDid ;

    return *this ;
}

// ----------------------------------------------------------------------------
// display
//
void
Message::display(char *s)
{
    printf(" -- MESSAGE - %s -", s);
    if ( type == CREATE_FEDERATION_EXECUTION )
      printf("CREATE_FEDERATION_EXECUTION : federationName %s : filename %s federateHandle %ld \n",
             ((federationName.c_str()==NULL)?"empty":federationName.c_str()),((FEDid.c_str()==NULL)?"empty":FEDid.c_str()),federate) ;
    if ( type == DESTROY_FEDERATION_EXECUTION )
      printf("DESTROY_FEDERATION_EXECUTION : federation %s : federate %ld\n",
             ((federationName.c_str()==NULL)?"empty":federationName.c_str()),federate) ;
    else if ( type == JOIN_FEDERATION_EXECUTION )
      printf("JOIN_FEDERATION_EXECUTION : federate number %ld federation name %s federate name %s\n",
             federate,((federationName.c_str()==NULL)?"empty":federationName.c_str()),federateName.c_str()) ;
    else if (type == FEDERATE_SAVE_COMPLETE )
        printf("FEDERATE_SAVE_COMPLETE : federationName %s : federatehandle %ld \n",federationName.c_str(),federate);
    else if (type == FEDERATE_RESTORE_COMPLETE )
        printf("FEDERATE_RESTORE_COMPLETE : federationName %s : federatehandle %ld \n",federationName.c_str(),federate);
    else if (type == REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE )
        printf("REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE : \n");
    else
        printf(" type=%d :", type);
    printf(" date=%f: ", fed_time.getTime());
    if (exception == 0 )
        printf("NO_EXCEPTION ");
    else
        printf(" exception=%d: ",exception);
    printf(" reason=%s: ",(exceptionReason.empty()?"empty":exceptionReason.c_str()));
    printf(" objectClass=%ld: ", objectClass);
    printf(" interactionClass=%ld:\n", interactionClass);
    printf(" attribute=%ld:\n", attribute);
    printf(" parameter=%ld:\n", parameter);
    printf(" object=%ld:", object);
    printf(" handleArraySize=%d:", handleArraySize);
    printf(" space %ld:", space);
    printf(" dimension %ld:\n", dimension);
    // printf(" transportation %d:\n", transportation);
    // printf(" ordering %d:\n", ordering);
}

// ----------------------------------------------------------------------------
void
Message::displayvalueArray(char *titre)
{
printf("(%s) valueArray size=%d\n",titre,(int)valueArray.size());
for (int i=0; i<(int)valueArray.size();i++)
   {
   printf("%d : length=%d : value=",i,(int)(valueArray[i].length()));
   for (int k=0; k<(int)valueArray[i].length() ;k++)
      {
      if (isprint(valueArray[i][k]) == 0 )
         {
         printf(" %x",valueArray[i][k]);
         }
      else
         {
         printf("%c",valueArray[i][k]);
         }
      }
printf("\n");
   }
}
} // namespace certi
