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

#include <config.h>
#include "Message.hh"

#include "certi.hh"
#include "PrettyDebug.hh"
#include "GAV.hh"

#include <cstdio>
#include <cstring>

using std::vector ;

namespace certi {

#define NONE -1

Message::Message()
{
    fed_time.setZero();
    exception = e_NO_EXCEPTION ;
    exceptionReason[0] = '\0' ;
    federateName[0] = '\0' ;
    federationName[0] = '\0' ;
    tag[0] = '\0' ;
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

}

//getValue allows Value as a set of bytes because length (parameter 2) is known
// ----------------------------------------------------------------------------
char *Message::getValue(int Rank, unsigned long *length, char *Value) const
    throw (RTIinternalError)
{
    // Pre-Checking

    if ((Rank < 0) || (Rank >= handleArraySize))
        throw RTIinternalError("Bad Rank in Message.");

    // Getting Value
    // First, the length of the Value
    *length = valueArray[Rank].length ;  

    if (Value != NULL)
        // Value exists, we copy it with memcpy instead of strcpy
        {
        memcpy(Value,valueArray[Rank].value, *length) ;     
        return NULL ;
        }
    else
        // Value doesn't exists, so create it then copy
        {
        char *Value = new char [*length] ;
        memcpy(Value,valueArray[Rank].value, *length) ;
        return Value ;
        }
}

// ----------------------------------------------------------------------------
ParameterLengthPair *
Message::getValueArray()
{
    int i ;
    unsigned long length ;

    ParameterLengthPair *NewValueArray = NULL ;

    NewValueArray = (ParameterLengthPair *) calloc(handleArraySize,
                                              sizeof(ParameterLengthPair));

    if (NewValueArray == NULL)
        throw RTIinternalError("No memory.");

    for (i = 0 ; i < handleArraySize ; i++)
        {
        getValue(i, &length, NewValueArray[i].value);
        NewValueArray[i].length = length ;
        }

    return NewValueArray ;
}

// ----------------------------------------------------------------------------
void
Message::setLabel(const char *NewLabel)
{
    if (strlen(NewLabel) > MAX_USER_TAG_LENGTH)
        throw ValueLengthExceeded("Label too long to fit in Message.");

    strcpy(label, NewLabel);
}

// ----------------------------------------------------------------------------
void
Message::setName(const char *NewName)
{
    if (strlen(NewName) > MAX_USER_TAG_LENGTH)
        throw ValueLengthExceeded("Name too long to fit in Message.");

    strcpy(name, NewName);
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
void
Message::setBoolean(bool the_bool)
{
    boolean = the_bool ;
}

// ----------------------------------------------------------------------------
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
void
Message::setException(TypeException the_exception,
                      const char *the_reason)
{
    exception = the_exception ;

    if (strlen(the_reason) > MAX_EXCEPTION_REASON_LENGTH)
        throw ValueLengthExceeded("Exception reason too long to fit in"
                                  " Message.");
    strcpy(exceptionReason, the_reason);
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
    handleArraySize = the_attributes.size();

    for (unsigned int i = 0 ; i < the_attributes.size(); ++i) {
        handleArray[i] = the_attributes.getHandle(i);
    }
}

// ----------------------------------------------------------------------------
void
Message::setAHS(const AttributeHandle *attr, int size)
{
    handleArraySize = size ;

    for (int i = 0 ; i < size ; ++i) {
        handleArray[i] = attr[i] ;
    }
}

// ----------------------------------------------------------------------------
RTI::AttributeHandleValuePairSet *
Message::getAHVPS() const
{
    CAttributeHandleValuePairSet theAttributes ;

    for (int i = 0 ; i < handleArraySize ; i++) {
        CAttributeHandleValuePair *att = new CAttributeHandleValuePair ;

        att->_attrib = handleArray[i] ;

        // BUG: Federate may be expecting to find value name
        // (a call to GetWithName for example).
        strcpy(att->_value.name, "");
        getValue(i, &(att->_value.length), att->_value.value);

        // BUG: Federate is expecting to find value type.
        strcpy(att->_value.type, "");

        theAttributes.add(att);
    }

    return theAttributes.toAHVPS();
}

// ----------------------------------------------------------------------------
void
Message::setAHVPS(const RTI::AttributeHandleValuePairSet &the_attributes)
{
    ULong length ;
    CAttributeHandleValuePairSet theAttributes_aux(the_attributes);

    const int size(theAttributes_aux._size);
    handleArraySize = size ;

    for (int i = 0 ; i < size ; i++) {
        CAttributeHandleValuePair *tmp = theAttributes_aux.getIeme(i);
        handleArray[i] = tmp->_attrib ;

        length = tmp->_value.length ;
        char *value = new char[length] ;
        memcpy(value,tmp->_value.value,tmp->_value.length) ;
        setValue(i, value, length);
        delete[] value;
    }
}

// ----------------------------------------------------------------------------
RTI::ParameterHandleValuePairSet *
Message::getPHVPS() const
{
    CParameterHandleValuePairSet theParameters ;

    for (int i = 0 ; i < handleArraySize ; i++) {
        CParameterHandleValuePair *par = new CParameterHandleValuePair ;

        par->_param = handleArray[i] ;

        // BUG: Federate may be expecting to find value name
        // (a call to GetWithName for example).
        strcpy(par->_value.name, "");

        getValue(i, &(par->_value.length), par->_value.value);

        // BUG: Federate is expecting to find value type.
        strcpy(par->_value.type, "");

        theParameters.add(par);
    }

    return theParameters.toPHVPS();
}

// ----------------------------------------------------------------------------
void
Message::setPHVPS(const RTI::ParameterHandleValuePairSet &the_parameters)
{
    ULong length ;
    CParameterHandleValuePairSet theParameters_aux(the_parameters);

    const int size(theParameters_aux._size);
    handleArraySize = size ;

    for (int i = 0 ; i < size ; i++) {
        CParameterHandleValuePair *tmp = theParameters_aux.getIeme(i);
        handleArray[i] = tmp->_param ;

        length = tmp->_value.length ;
        char *value = new char[length] ;
        setValue(i, value, length);
        delete[] value;
    }
}

// ----------------------------------------------------------------------------
void
Message::setAttributes(AttributeHandle *the_attributes, ushort the_size)
{
    handleArraySize = the_size ;

    for (int i = 0 ; i < the_size ; i++) {
        handleArray[i] = the_attributes[i] ;
    }
}

// ----------------------------------------------------------------------------
void
Message::setAttributes(AttributeHandle *the_attributes,
                       ValueLengthPair *the_values,
                       ushort the_size)
{
    handleArraySize = the_size ;

    for (int i = 0 ; i < the_size ; i++) {
        handleArray[i] = the_attributes[i] ;
        setValue(i, the_values[i].value,the_values[i].length ) ;
    }
}

// ----------------------------------------------------------------------------
void
Message::setParameters(ParameterHandle * the_parameters,
                       ParameterLengthPair * the_values,
                       ushort the_size)

{
    handleArraySize = the_size ;

    for (int i = 0 ; i < the_size ; i++) {
        handleArray[i] = the_parameters[i] ;
        setValue(i, the_values[i].value, the_values[i].length);
    }
}

// ----------------------------------------------------------------------------
//! Sets the federation name.
void Message::setFederationName(const char *NewNomFederation)
{
    if (strlen(NewNomFederation) > MAX_FEDERATION_NAME_LENGTH)
        throw ValueLengthExceeded("NomFederation too long to fit in Message.");

    strcpy(federationName, NewNomFederation);
}


// ----------------------------------------------------------------------------
// setFederateName
//
void
Message::setFederateName(const char *NewNomFedere)
{
    if (strlen(NewNomFedere) > MAX_FEDERATE_NAME_LENGTH)
        throw ValueLengthExceeded("NomFedere too long to fit in Message.");

    strcpy(federateName, NewNomFedere);
}

// ----------------------------------------------------------------------------
// setTag
//
void
Message::setTag(const char *new_tag)
{
    if (new_tag) {
	if (strlen(new_tag) > MAX_USER_TAG_LENGTH) {
	    throw ValueLengthExceeded("Tag too long to fit in Message.");
	}
	strcpy(tag, new_tag);
    }
    else {
	strcpy(tag, "");
    }
}

// ----------------------------------------------------------------------------
// setValue
//
void
Message::setValue(int Rank, const char *Value, unsigned long length)
    throw (RTIinternalError)
{

    // Pre-Checking

    if ((Value == NULL) || (length > MAX_BYTES_PER_VALUE))
        throw RTIinternalError("Bad Value for message.");

    if ((Rank < 0) || (Rank >= handleArraySize))
        throw RTIinternalError("Bad Rank for message.");

    // Setting Value
    // First we store the length, then copy with memcpy instead of strcpy
    valueArray[Rank].length = length ;
    memcpy(valueArray[Rank].value, Value, length);
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

    strcpy(exceptionReason, msg.exceptionReason);
    strcpy(federateName, msg.federateName);
    strcpy(federationName, msg.federationName);

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

    strcpy(tag, msg.tag);
    strcpy(name, msg.name);

    transport = msg.transport ;
    order = msg.order ;

    eventRetraction.theSerialNumber = msg.eventRetraction.theSerialNumber ;
    eventRetraction.sendingFederate = msg.eventRetraction.sendingFederate ;

    strcpy(label, msg.label);

    handleArraySize = msg.handleArraySize ;

    int i ;
    for (i=0 ; i < handleArraySize ; i++)
        handleArray[i] = msg.handleArray[i] ;

    for (i=0 ; i < handleArraySize ; i++)
        {
        valueArray[i].length = msg.valueArray[i].length ;
        memcpy(valueArray[i].value, msg.valueArray[i].value, msg.valueArray[i].length );
        }

    return *this ;
}

// ----------------------------------------------------------------------------
// display
//
void
Message::display(char *s)
{
    printf(" --- MESSAGE --- %s ---\n", s);
    printf(" type=%d:\n", type);
    printf(" date=%f:\n", fed_time.getTime());
    printf(" exception=%d:\n", exception);
    printf(" objectClass=%ld:\n", objectClass);
    printf(" interactionClass=%ld:\n", interactionClass);
    printf(" attribute=%ld:\n", attribute);
    printf(" parameter=%ld:\n", parameter);
    printf(" object=%ld:\n", object);
    printf(" handleArraySize=%d:\n", handleArraySize);
    printf(" space %ld:\n", space);
    printf(" dimension %ld:\n", dimension);
    // printf(" transportation %d:\n", transportation);
    // printf(" ordering %d:\n", ordering);
}

} // namespace certi
