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
// $Id: Message.cc,v 3.24 2004/05/18 13:18:53 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Message.hh"

#include "certi.hh"
#include "PrettyDebug.hh"
#include "GAV.hh"
#include "converter.hh"

#include <cstdio>
#include <cstring>

using std::vector ;

namespace certi {

#define NONE -1

Message::Message()
{
    date = 0.0 ;
    exception = e_NO_EXCEPTION ;
    exceptionReason[0] = '\0' ;
    federateName[0] = '\0' ;
    federationName[0] = '\0' ;
    tag[0] = '\0' ;
    name[0] = '\0' ;
    label[0] = '\0' ;
    handleArraySize = 0 ;
}

// ----------------------------------------------------------------------------
//! Destructor
//
Message::~Message()
{
}

// ----------------------------------------------------------------------------
//! getValue
//
char *Message::getValue(int Rank, char *Value) const
    throw (RTIinternalError)
{
    // Pre-Checking

    if ((Rank < 0) || (Rank >= handleArraySize))
        throw RTIinternalError("Bad Rank in Message.");

    // Getting Value

    if (Value != NULL) {
        strcpy(Value, valueArray[Rank]);
        return NULL ;
    }
    else
        return strdup(valueArray[Rank]);
}

// ----------------------------------------------------------------------------
// getValueArray
//
ParameterValue *
Message::getValueArray()
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

// ----------------------------------------------------------------------------
// setLabel
//
void Message::setLabel(const char *NewLabel)
{
    if (strlen(NewLabel) > MAX_USER_TAG_LENGTH)
        throw ValueLengthExceeded("Label too long to fit in Message.");

    strcpy(label, NewLabel);
}

// ----------------------------------------------------------------------------
// setName
//
void
Message::setName(const char *NewName)
{
    if (strlen(NewName) > MAX_USER_TAG_LENGTH)
        throw ValueLengthExceeded("Name too long to fit in Message.");

    strcpy(name, NewName);
}

// ----------------------------------------------------------------------------
// setType
//
void
Message::setType(Type t)
{
    type = t ;
}
// ----------------------------------------------------------------------------
// setNbExtents
void
Message::setNumber(unsigned long nb)
{
    number = nb ;
}

// ----------------------------------------------------------------------------
// setSpace
void
Message::setSpace(SpaceHandle handle)
{
    space = handle ;
}

// ----------------------------------------------------------------------------
// setRegion
void
Message::setRegion(long handle)
{
    region = handle ;
}

// ----------------------------------------------------------------------------
// setDimension
void
Message::setDimension(DimensionHandle handle)
{
    dimension = handle ;
}

// ----------------------------------------------------------------------------
// setInteractionClass
void
Message::setInteractionClass(InteractionClassHandle handle)
{
    interactionClass = handle ;
}

// ----------------------------------------------------------------------------
// setObjectClass
void
Message::setObjectClass(ObjectClassHandle handle)
{
    objectClass = handle ;
}

// ----------------------------------------------------------------------------
// setAttribute
void
Message::setAttribute(AttributeHandle handle)
{
    attribute = handle ;
}

// ----------------------------------------------------------------------------
void
Message::setResignAction(ResignAction the_action)
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
Message::setFedTime(const FedTime &the_time)
{
    date = (FederationTime) (RTIfedTime(the_time)._fedTime);
}

// ----------------------------------------------------------------------------
void
Message::setLookahead(const FedTime& the_lookahead)
{
    lookahead = (FederationTimeDelta) ((RTIfedTime&) the_lookahead).getTime();
}

// ----------------------------------------------------------------------------
void
Message::setFederationTime(FederationTime the_time)
{
    date = the_time ;
}

// ----------------------------------------------------------------------------
void
Message::setBoolean(Boolean the_bool)
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
Message::setTransportation(TransportationHandle the_transport)
{
    transport = ((the_transport == 1) ? RELIABLE : BEST_EFFORT);
}

// ----------------------------------------------------------------------------
void
Message::setOrdering(OrderingHandle the_ordering)
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
    attributeSet = AttributeHandleSetFactory::create(handleArraySize);

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
void
Message::setRegions(const RegionImp **reg, int size)
{
    regions.resize(size);
    
    for (int i = 0 ; i < size ; ++i) {
        regions[i] = reg[i]->getHandle();
    }
}

// ----------------------------------------------------------------------------
AttributeHandleValuePairSet*
Message::getAHVPS() const
{
    CAttributeHandleValuePairSet * theAttributes ;
    theAttributes = new CAttributeHandleValuePairSet ;

    for (int i = 0 ; i < handleArraySize ; i++) {
        CAttributeHandleValuePair *att = new CAttributeHandleValuePair ;

        att->_attrib = handleArray[i] ;

        // BUG: Federate may be expecting to find value name
        // (a call to GetWithName for example).
        strcpy(att->_value.name, "");
        getValue(i, att->_value.value);

        // BUG: Federate is expecting to find value type.
        strcpy(att->_value.type, "");

        theAttributes->add(att);
    }

    return theAttributes->toAHVPS();
}

// ----------------------------------------------------------------------------
void
Message::setAHVPS(const AttributeHandleValuePairSet &the_attributes)
{
    ULong length ;
    CAttributeHandleValuePairSet *theAttributes_aux ;
    theAttributes_aux = new CAttributeHandleValuePairSet(the_attributes);
    CAttributeHandleValuePair *tmp ;

    handleArraySize = theAttributes_aux->_size ;

    for (int i = 0 ; i < theAttributes_aux->_size ; i++) {
        tmp = theAttributes_aux->getIeme(i);
        handleArray[i] = tmp->_attrib ;

        // codage
        getObjectToStringLength(tmp->_value.value,
                                tmp->_value.length,
                                length);
        char *value = new char[length] ;
        objectToString(tmp->_value.value, tmp->_value.length, value);
        setValue(i, value);
    }
    delete theAttributes_aux ;
}

// ----------------------------------------------------------------------------
ParameterHandleValuePairSet*
Message::getPHVPS() const
{
    CParameterHandleValuePairSet* theParameters ;
    theParameters = new CParameterHandleValuePairSet ;

    for (int i = 0 ; i < handleArraySize ; i++) {
        CParameterHandleValuePair *par = new CParameterHandleValuePair ;

        par->_param = handleArray[i] ;

        // BUG: Federate may be expecting to find value name
        // (a call to GetWithName for example).
        strcpy(par->_value.name, "");

        getValue(i, par->_value.value);

        // BUG: Federate is expecting to find value type.
        strcpy(par->_value.type, "");

        theParameters->add(par);
    }

    return theParameters->toPHVPS();
}

// ----------------------------------------------------------------------------
void
Message::setPHVPS(const ParameterHandleValuePairSet &the_parameters)
{
    ULong length ;
    CParameterHandleValuePairSet *theParameters_aux ;
    theParameters_aux = new CParameterHandleValuePairSet(the_parameters);
    CParameterHandleValuePair *tmp ;

    handleArraySize = theParameters_aux->_size ;

    for (int i = 0 ; i < theParameters_aux->_size ; i++) {
        tmp = theParameters_aux->getIeme(i);
        handleArray[i] = tmp->_param ;

        // codage
        getObjectToStringLength(tmp->_value.value,
                                tmp->_value.length,
                                length);
        char *value = new char[length] ;
        objectToString(tmp->_value.value, tmp->_value.length, value);
        setValue(i, value);
    }

    delete theParameters_aux ;
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
                       AttributeValue *the_values,
                       ushort the_size)
{
    handleArraySize = the_size ;

    for (int i = 0 ; i < the_size ; i++) {
        handleArray[i] = the_attributes[i] ;
        setValue(i, the_values[i]);
    }
}

// ----------------------------------------------------------------------------
void
Message::setParameters(ParameterHandle * the_parameters,
                       ParameterValue * the_values,
                       ushort the_size)
{
    handleArraySize = the_size ;

    for (int i = 0 ; i < the_size ; i++) {
        handleArray[i] = the_parameters[i] ;
        setValue(i, the_values[i]);
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
Message::setValue(int Rank, const char *Value)
    throw (RTIinternalError)
{
    // Pre-Checking

    if ((Value == NULL) || (strlen(Value) > MAX_BYTES_PER_VALUE))
        throw RTIinternalError("Bad Value for message.");

    if ((Rank < 0) || (Rank >= handleArraySize))
        throw RTIinternalError("Bad Rank for message.");

    // Setting Value

    strcpy(valueArray[Rank], Value);
}

// ----------------------------------------------------------------------------
// operator=
//
Message &
Message::operator=(const Message& msg)
{
    type = msg.type ;
    date = msg.date ;
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

    eventRetraction.theEventTime = msg.eventRetraction.theEventTime ;
    eventRetraction.theSerialNumber = msg.eventRetraction.theSerialNumber ;
    eventRetraction.sendingFederate = msg.eventRetraction.sendingFederate ;

    strcpy(label, msg.label);

    handleArraySize = msg.handleArraySize ;

    int i ;
    for (i=0 ; i < handleArraySize ; i++)
        handleArray[i] = msg.handleArray[i] ;

    for (i=0 ; i<<handleArraySize ; i++)
        strcpy(valueArray[i], msg.valueArray[i]);

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
    printf(" date=%f:\n", date);
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

// $Id: Message.cc,v 3.24 2004/05/18 13:18:53 breholee Exp $
