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
#include "GAV.hh"

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
/** getValue : Value and its length are tooken from valueArray[Rank]
    @param Rank valueArray rank
    @param length byte number of Value tooken from valueArray[Rank] (modified)
    @param Value Value tooken from valueArray[Rank]
           Value is managed as an address to a set of bytes
    Note : if Value is not present, Value is created and its address is returned
           by getValue
    getValue allows Value as a set of bytes because length (parameter 2) is known
*/
// ----------------------------------------------------------------------------
char *Message::getValue(int Rank, unsigned long *length, char *Value) const
    throw (RTIinternalError)
{
    // Pre-Checking

    if ((Rank < 0) )
        throw RTIinternalError("(getValue) Bad Rank in Message.");

    // Getting Value
    // First, the length of the Value
    *length = valueArray[Rank].length ;  

    if (Value != NULL)
        // Value exists, we copy it with memcpy instead of strcpy
        {
        memcpy(Value,(char *)*(valueArray[Rank].value), *length) ;     
        return NULL ;
        }
    else
        // Value doesn't exists, so create it then copy
        {
        char *TempValue = new char [*length] ;
        memcpy(TempValue,(char *)*(valueArray[Rank].value), *length) ;
        Value = TempValue ;
        return Value ;
        }
}

// ----------------------------------------------------------------------------
/** Return a newly allocated ValueArray, exactly of size HandleArraySize.
*/
std::vector <ParameterLengthPair>
Message::getValueArray()
{
    G.Out(pdGendoc,"enter Message::getValueArray");
    int i ;

    std::vector <ParameterLengthPair> NewValueArray ;
    NewValueArray.resize(handleArraySize) ;

    for (i = 0 ; i < handleArraySize ; i++)
        {
        NewValueArray[i].length = valueArray[i].length ;
        NewValueArray[i].value = new char[NewValueArray[i].length] ;
        memcpy(&(NewValueArray[i].value[0]),valueArray[i].value,NewValueArray[i].length);
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
Message::setFedTime(const RTI::FedTime &the_time)
{
    fed_time = dynamic_cast<const RTIfedTime &>(the_time);
}

// ----------------------------------------------------------------------------
void
Message::setLookahead(const RTI::FedTime& the_lookahead)
{
    lookahead = (FederationTimeDelta) ((RTIfedTime&) the_lookahead).getTime();
}

// ----------------------------------------------------------------------------
void
Message::setFederationTime(FederationTime the_time)
{
    fed_time = RTIfedTime(the_time);
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
Message::setTransportation(RTI::TransportationHandle the_transport)
{
    transport = ((the_transport == 1) ? RELIABLE : BEST_EFFORT);
}

// ----------------------------------------------------------------------------
void
Message::setOrdering(RTI::OrderingHandle the_ordering)
{
    order = ((the_ordering == 1) ? RECEIVE : TIMESTAMP);
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
                      const char *the_reason)
{
    exception = the_exception ;

    if (strlen(the_reason) > MAX_EXCEPTION_REASON_LENGTH)
        throw ValueLengthExceeded("Exception reason too long to fit in"
                                  " Message.");
    exceptionReason = the_reason;
}

// ----------------------------------------------------------------------------
void
Message::setFederate(FederateHandle the_federate)
{
    federate = the_federate ;
}

// ----------------------------------------------------------------------------
AttributeHandleSet*
Message::getAHS() const
{
    AttributeHandleSet *attributeSet ;
    attributeSet = RTI::AttributeHandleSetFactory::create(handleArraySize);

    for (int i = 0 ; i < handleArraySize ; i++) {
        attributeSet->add(handleArray[i]);
    }

    return attributeSet ;
}

// ----------------------------------------------------------------------------
void
Message::setAHS(const AttributeHandleSet &the_attributes)
{
    G.Out(pdGendoc,"enter Message::setAHS");
    handleArraySize = the_attributes.size();
    handleArray.resize(handleArraySize);

    for (unsigned int i = 0 ; i < the_attributes.size(); ++i) {
        handleArray[i] = the_attributes.getHandle(i);
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
RTI::AttributeHandleValuePairSet *
Message::getAHVPS() const
{
    RTI::AttributeHandleValuePairSet *ahvps ;
    ahvps = RTI::AttributeSetFactory::create(handleArraySize);

    unsigned long length ;
    char *value ;

    for (int i = 0 ; i < handleArraySize ; i++)
        {
        length = valueArray[i].length ;
        value = new char[length] ;
        memcpy(value,valueArray[i].value,length);
        ahvps->add(handleArray[i], value, length);
        //delete[] value;
        }

    return ahvps ;
}

// ----------------------------------------------------------------------------
/** store attribute set into handleArray and valueArray of a Message
    @param the_attributes attribute set
*/
void
Message::setAHVPS(const RTI::AttributeHandleValuePairSet &the_attributes)
{
    G.Out(pdGendoc,"enter Message::setAHVPS");
    ULong length ;

    unsigned long size ;
    char *valuebuf=NULL;

    size = the_attributes.size() ;
    handleArraySize = size ;
    handleArray.resize(handleArraySize);
    valueArray.resize(size) ;

    for (unsigned long i = 0 ; i < size ; i++) {

        // handle stored into handleArray[i]
        handleArray[i] = the_attributes.getHandle(size-1-i);
        // value length extracted from the_attributes
        length = the_attributes.getValueLength(size-1-i) ;

        // copying into valuebuf created by getValue
        valuebuf = new char[length] ;
        the_attributes.getValue(size-1-i,valuebuf,length) ;

        // valuebuf address and its length are stored into valueArray[i]
        valueArray[i].length = length ;
        valueArray[i].value = valuebuf ;

    }
    G.Out(pdGendoc,"exit  Message::setAHVPS");
}

// ----------------------------------------------------------------------------
RTI::ParameterHandleValuePairSet *
Message::getPHVPS() const
{
    ParameterHandleValuePairSetImp *phvps ;
    phvps = new ParameterHandleValuePairSetImp(handleArraySize);

    unsigned long length ;
    char *value ;

    for (int i = 0 ; i < handleArraySize ; i++)
        {
        length = valueArray[i].length ;
        value = new char[length] ;
        memcpy(value,valueArray[i].value,length);
        phvps->add(handleArray[i], value, length);
        }

    return phvps ;
}

// ----------------------------------------------------------------------------
void
Message::setPHVPS(const RTI::ParameterHandleValuePairSet &the_parameters)
{
    G.Out(pdGendoc,"enter Message::setPHVPS");
    ULong length ;

    unsigned long size ;
    char *valuebuf=NULL;

    size = the_parameters.size() ;
    handleArraySize = size ;
    handleArray.resize(handleArraySize);
    valueArray.resize(size) ;

    for (unsigned long i = 0 ; i < size ; i++)
        {
        // handle stored into handleArray[i]
        handleArray[i] = the_parameters.getHandle(size-1-i);
        // value length extracted from the_parameters
        length = the_parameters.getValueLength(size-1-i) ;
        // copying into valuebuf created by getValue
        valuebuf = new char[length] ;
        the_parameters.getValue(size-1-i, valuebuf, length) ;
        // valuebuf address and its length are stored into valueArray(i]
        valueArray[i].length = length ;
        valueArray[i].value = valuebuf ;

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
                       std::vector <ValueLengthPair> &the_values,
                       ushort the_size)
{
    G.Out(pdGendoc,"enter Message::setAttributes");
    char *tempValue ;
    handleArraySize = the_size ;
    handleArray.resize(handleArraySize);
    valueArray.resize(the_size) ;

    for (int i = 0 ; i < the_size ; i++) {
        // attributes into handleArray
        handleArray[i] = the_attributes[i] ;
        // values     into valueArray
        tempValue = new char[the_values[i].length] ;
        memcpy(tempValue,the_values[i].value,the_values[i].length ) ;
        setValue(i, tempValue,the_values[i].length ) ;
    }
    G.Out(pdGendoc,"exit  Message::setAttributes");
}

// ----------------------------------------------------------------------------
// setParameters
// store parameters into handleArray
// store values     into valueArray
void
Message::setParameters(std::vector <ParameterHandle> & the_parameters,
                       std::vector <ParameterLengthPair> & the_values,
                       ushort the_size)

{
    char *tempValue ;
    handleArraySize = the_size ;
    handleArray.resize(handleArraySize);
    valueArray.resize(the_size) ;

    for (int i = 0 ; i < the_size ; i++) {
        // parameters into handleArray
        handleArray[i] = the_parameters[i] ;
        // values     into valueArray
        tempValue = new char[the_values[i].length] ;
        memcpy(tempValue,the_values[i].value,the_values[i].length ) ;
        setValue(i, the_values[i].value, the_values[i].length);
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
/** Store Tag into a Message (i.e. in tag, size is limited by MAX_USER_TAG_LENGTH)
    @param new_tag tag (NULL tag is stored as an empty string)
*/
void
Message::setTag(std::string new_tag)
{
    tag = new_tag ;
}

// ----------------------------------------------------------------------------
// setValue
// Store Value into valueArray[Rank)
void
Message::setValue(int Rank, const char *Value, unsigned long length)
    throw (RTIinternalError)
{

    // Pre-Checking

    if ((Value == NULL))
        throw RTIinternalError("Bad Value (NULL) for message.");

    if ((Rank < 0) )
        throw RTIinternalError("(setValue) Bad Rank for message.");

    // Setting Value
    // First we store the value length
    valueArray[Rank].length = length ;
    
    // then copy Value address into valueArray
    valueArray[Rank].value = (char *)Value ;
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
        {
        valueArray[i].length = msg.valueArray[i].length ;
        char * TempValue = new char[valueArray[i].length] ;
        memcpy(TempValue, (char *)*(msg.valueArray[i].value), msg.valueArray[i].length );
        valueArray[i].value=TempValue ;
        }

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
    printf(" reason=%s: ",((exceptionReason.c_str()==NULL)?"empty":exceptionReason.c_str()));
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
   printf("%d : length=%d : value=",i,(int)(valueArray[i].length));
   for (int k=0; k<(int)valueArray[i].length ;k++)
      {
      if (isprint(valueArray[i].value[k]) == 0 )
         {
         printf(" %x",valueArray[i].value[k]);
         }
      else
         {
         printf("%c",valueArray[i].value[k]);
         }
      }
printf("\n");
   }
}
} // namespace certi
