// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
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
// $Id: Message.cc,v 3.10.4.1 2003/04/22 21:47:14 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>

#include "Message.hh"

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


// ---------------
// -- ~Message --
// ---------------

Message::~Message()
{
}


// --------------
// -- GetValue --
// --------------

char *Message::getValue(int Rank, char *Value)
    throw (RTIinternalError)
{
    // Pre-Checking

    if ((Rank < 0) || (Rank >= handleArraySize))
        throw RTIinternalError("Bad Rank in Message.");

    // Getting Value

    if (Value != NULL) {
        strcpy(Value, valueArray[Rank]);
        return NULL ;
    } else
        return strdup(valueArray[Rank]);

}


// ------------------------
// -- GetValueArray --
// ------------------------

ParameterValue *Message::getValueArray()
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
// -- SetLabel --
// --------------

void Message::setLabel(const char *NewLabel)
{
    if (strlen(NewLabel) > MAX_USER_TAG_LENGTH)
        throw ValueLengthExceeded("Label too long to fit in Message.");

    strcpy(label, NewLabel);
}


// -------------
// -- SetName --
// -------------

void Message::setName(const char *NewName)
{
    if (strlen(NewName) > MAX_USER_TAG_LENGTH)
        throw ValueLengthExceeded("Name too long to fit in Message.");

    strcpy(name, NewName);
}

// ----------------------------------------------------------------------------
// setType
void
Message::setType(TypeService t)
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
// addExtent
void
Message::addExtent(Extent *e)
{
    extentSet.push_back(e);
}

// ----------------------
// -- SetNomFederation --
// ----------------------

void Message::setFederationName(const char *NewNomFederation)
{
    if (strlen(NewNomFederation) > MAX_FEDERATION_NAME_LENGTH)
        throw ValueLengthExceeded("NomFederation too long to fit in Message.");

    strcpy(federationName, NewNomFederation);
}


// ----------------------
// -- SetNomFedere --
// ----------------------

void Message::setFederateName(const char *NewNomFedere)
{
    if (strlen(NewNomFedere) > MAX_FEDERATE_NAME_LENGTH)
        throw ValueLengthExceeded("NomFedere too long to fit in Message.");

    strcpy(federateName, NewNomFedere);
}


// ------------
// -- SetTag --
// ------------

void Message::setTag(const char *NewTag)
{
    if (strlen(NewTag) > MAX_USER_TAG_LENGTH)
        throw ValueLengthExceeded("Tag too long to fit in Message.");

    if (NewTag != NULL) strcpy(tag, NewTag);
    else strcpy(tag, "");
}


// --------------
// -- SetValue --
// --------------

void Message::setValue(int Rank, const char *Value)
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


Message &
Message::operator = (const Message& msg)
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
    transportation = msg.transportation ;
    ordering = msg.ordering ;

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
    printf(" space %d:\n", space);
    printf(" dimension %d:\n", dimension);
    printf(" transportation %d:\n", transportation);
    printf(" ordering %d:\n", ordering);
}

} // namespace certi

// $Id: Message.cc,v 3.10.4.1 2003/04/22 21:47:14 breholee Exp $
