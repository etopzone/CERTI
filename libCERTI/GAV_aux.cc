// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: GAV_aux.cc,v 3.6 2003/01/26 23:14:05 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "RTItypes.hh"
#include "PrettyDebug.hh"

//-----------------------------------------------------------------------
// AttributeHandleValuePair
//-----------------------------------------------------------------------

namespace certi {

// ---------------------------------------------------------------------------
AttributeHandleValuePair::AttributeHandleValuePair(Handle handle,
                                                   const char *value,
                                                   ULong value_length)
{
    _handle = handle ;
    _valueLength = value_length ;

    _value = (char *)malloc(value_length);
    memcpy(_value, value, value_length);
}

// ---------------------------------------------------------------------------
AttributeHandleValuePair::~AttributeHandleValuePair(void)
{
    delete _value ;
}

//-----------------------------------------------------------------------
// AttributeHandleValuePairSet
//-----------------------------------------------------------------------
AttributeHandleValuePairSet::~AttributeHandleValuePairSet(void)
{
    empty();
}

// ---------------------------------------------------------------------------
inline ULong 
AttributeHandleValuePairSet::size(void) const
{
    return list<AttributeHandleValuePair *>::size();
}

// ---------------------------------------------------------------------------
Handle
AttributeHandleValuePairSet::getHandle(ULong i) const
    throw (ArrayIndexOutOfBounds)
{
    list<AttributeHandleValuePair *>::const_iterator j = begin();
    for (ULong k = 0 ; j != end() ; j++, k++) {
        if (i == k)
            return (*j)->_handle ;
    }

    throw ArrayIndexOutOfBounds();
}

// ---------------------------------------------------------------------------
ULong 
AttributeHandleValuePairSet::getValueLength(ULong i) const
    throw (ArrayIndexOutOfBounds)
{
    list<AttributeHandleValuePair *>::const_iterator j = begin();
    for (ULong k = 0 ; j != end() ; j++, k++) {
        if (i == k)
            return (*j)->_valueLength ;
    }

    throw ArrayIndexOutOfBounds();
}

// ---------------------------------------------------------------------------
void 
AttributeHandleValuePairSet::getValue(ULong i,
                                      char* buff,
                                      ULong& value_length) const
    throw (ArrayIndexOutOfBounds)
{
    list<AttributeHandleValuePair *>::const_iterator j = begin();
    for (ULong k = 0 ; j != end() ; j++, k++) {
        if (i == k) {
            value_length = (*j)->_valueLength ;
            memcpy(buff, (*j)->_value, (*j)->_valueLength);
            return ;
        }
    }

    throw ArrayIndexOutOfBounds();
}

// ---------------------------------------------------------------------------
char *
AttributeHandleValuePairSet::getValuePointer(ULong i,
                                             ULong& value_length) const
    throw (ArrayIndexOutOfBounds)
{
    list<AttributeHandleValuePair *>::const_iterator j = begin();
    for (ULong k = 0 ; j != end() ; j++, k++) {
        if (i == k) {
            value_length = (*j)->_valueLength ;
            return (*j)->_value ;
        }
    }

    throw ArrayIndexOutOfBounds();
}

// ---------------------------------------------------------------------------
inline TransportType 
AttributeHandleValuePairSet::getTransportType(ULong) const
    throw (ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext)
{
    return _transport ;
}

// ---------------------------------------------------------------------------
inline OrderType 
AttributeHandleValuePairSet::getOrderType(ULong) const
    throw (ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext)
{
    return _order ;
}

// ---------------------------------------------------------------------------
Region*
AttributeHandleValuePairSet::getRegion(ULong) const
    throw (ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext,
           UnimplementedService)//CERTI
{
    throw UnimplementedService();
}

// ---------------------------------------------------------------------------
void
AttributeHandleValuePairSet::add(Handle h,
                                 const char* buff,
                                 ULong value_length)
    throw (ValueLengthExceeded, ValueCountExceeded)
{
    AttributeHandleValuePair *ahvp ;
    ahvp = new AttributeHandleValuePair(h, buff, value_length);

    push_front(ahvp);
}

// ---------------------------------------------------------------------------
void 
AttributeHandleValuePairSet::remove(Handle h)
    throw (ArrayIndexOutOfBounds)
{
    list<AttributeHandleValuePair *>::iterator j ;
    for (j = begin() ; j != end() ; j++) {
        if ((*j)->_handle == h) {
            delete (*j);
            erase(j);
            return ;
        }
    }

    throw ArrayIndexOutOfBounds();
}

// ---------------------------------------------------------------------------
void 
AttributeHandleValuePairSet::moveFrom(const AttributeHandleValuePairSet&,
                                      ULong&)
    throw (ValueCountExceeded, ArrayIndexOutOfBounds,
           UnimplementedService) //CERTI
{
    throw UnimplementedService();
}

// ---------------------------------------------------------------------------
void 
AttributeHandleValuePairSet::empty(void)
{
    while (!list<AttributeHandleValuePair *>::empty()) {
        delete front();
        pop_front();
    }
}

// ---------------------------------------------------------------------------
ULong 
AttributeHandleValuePairSet::start(void) const
{
    //not implemented
    return 0 ;
}

// ---------------------------------------------------------------------------
ULong 
AttributeHandleValuePairSet::valid(ULong) const
{
    //not implemented
    return 0 ;
}

// ---------------------------------------------------------------------------
ULong 
AttributeHandleValuePairSet::next(ULong) const
{
    //not implemented
    return 0 ;
}

//-----------------------------------------------------------------------
// AttributeSetFactory
//-----------------------------------------------------------------------

AttributeHandleValuePairSet* 
AttributeSetFactory::create(ULong)
    throw (MemoryExhausted, ValueCountExceeded, HandleValuePairMaximumExceeded)
{
    AttributeHandleValuePairSet *ahvps = new AttributeHandleValuePairSet ;
    ahvps->_order = RECEIVE ;
    ahvps->_transport = RELIABLE ;
    return ahvps ;
}

//-----------------------------------------------------------------------
// AttributeHandleSet
//-----------------------------------------------------------------------
AttributeHandleSet::~AttributeHandleSet(void)
{
    empty();
}

// ---------------------------------------------------------------------------
inline ULong 
AttributeHandleSet::size(void) const
{
    return list<AttributeHandle>::size();
}

// ---------------------------------------------------------------------------
AttributeHandle 
AttributeHandleSet::getHandle(ULong i) const
    throw (ArrayIndexOutOfBounds)
{
    list<AttributeHandle>::const_iterator h ;
    ULong j ;
    for (j = 0, h = begin() ; h != end() ; h++, j++) {
        if (i == j)
            return (*h);
    }

    throw ArrayIndexOutOfBounds();
}

// ---------------------------------------------------------------------------
void 
AttributeHandleSet::add(AttributeHandle h)
    throw (ArrayIndexOutOfBounds, AttributeNotDefined)
{
    push_front(h);
}

// ---------------------------------------------------------------------------
void 
AttributeHandleSet::remove(AttributeHandle h)
    throw (AttributeNotDefined)// not guaranteed safe while iterating
{
    if (isMember(h) == RTI_TRUE)
        list<AttributeHandle>::remove(h);
    else
        throw AttributeNotDefined();
}

// ---------------------------------------------------------------------------
void 
AttributeHandleSet::empty(void)
{
    list<AttributeHandle>::clear();
}

// ---------------------------------------------------------------------------
inline Boolean
AttributeHandleSet::isEmpty(void) const
{
    return ((list<AttributeHandle>::empty()) ? RTI_TRUE : RTI_FALSE);
}

// ---------------------------------------------------------------------------
Boolean
AttributeHandleSet::isMember(AttributeHandle h) const
{
    list<AttributeHandle>::const_iterator result = find(begin(), end(), h);

    if (result == end())
        return RTI_FALSE ;
    else
        return RTI_TRUE ;
}

//-----------------------------------------------------------------------
// AttributeHandleSetFactory
//-----------------------------------------------------------------------

AttributeHandleSet* 
AttributeHandleSetFactory::create(ULong)
    throw (MemoryExhausted, ValueCountExceeded)
{
    return(new AttributeHandleSet());
}

//-----------------------------------------------------------------------
// FederateHandleSet
//-----------------------------------------------------------------------
FederateHandleSet::~FederateHandleSet(void)
{
    empty();
}

// ---------------------------------------------------------------------------
inline ULong 
FederateHandleSet::size(void) const
{
    return list<FederateHandle>::size();
}

// ---------------------------------------------------------------------------
FederateHandle 
FederateHandleSet::getHandle(ULong i) const
    throw (ArrayIndexOutOfBounds)
{
    list<FederateHandle>::const_iterator h ;
    ULong j ;
    for (j = 0, h = begin() ; h != end() ; h++,j++) {
        if (i == j)
            return (*h);
    }

    throw ArrayIndexOutOfBounds();
}

// ---------------------------------------------------------------------------
void 
FederateHandleSet::add(FederateHandle h)
    throw (ValueCountExceeded)
{
    push_front(h);
}

// ---------------------------------------------------------------------------
void 
FederateHandleSet::remove(FederateHandle h)
    throw (AttributeNotDefined, ArrayIndexOutOfBounds)
{
    if (isMember(h) == RTI_TRUE)
        list<FederateHandle>::remove(h);
    else
        throw AttributeNotDefined();
}

// ---------------------------------------------------------------------------
void 
FederateHandleSet::empty(void)
{
    list<FederateHandle>::clear();
}

// ---------------------------------------------------------------------------
Boolean
FederateHandleSet::isMember(FederateHandle h) const
{
    list<FederateHandle>::const_iterator result = find(begin(), end(), h);

    if (result == end())
        return RTI_FALSE ;
    else
        return RTI_TRUE ;
}

//-----------------------------------------------------------------------
// ParameterHandleValuePair
//-----------------------------------------------------------------------

ParameterHandleValuePair::ParameterHandleValuePair(Handle handle,
                                                   const char * value,
                                                   ULong value_length)
{
    _handle = handle ;
    _valueLength = value_length ;

    _value = (char *)malloc(value_length);
    memcpy(_value, value, value_length);
}

// ---------------------------------------------------------------------------
ParameterHandleValuePair::~ParameterHandleValuePair(void)
{
    delete _value ;
}

//-----------------------------------------------------------------------
// ParameterHandleValuePairSet
//-----------------------------------------------------------------------
ParameterHandleValuePairSet::~ParameterHandleValuePairSet(void)
{
    empty();
}

// ---------------------------------------------------------------------------
inline ULong 
ParameterHandleValuePairSet::size(void) const
{
    return list<ParameterHandleValuePair *>::size();
}

// ---------------------------------------------------------------------------
Handle 
ParameterHandleValuePairSet::getHandle(ULong i) const
    throw (ArrayIndexOutOfBounds)
{
    list<ParameterHandleValuePair *>::const_iterator j = begin();
    for (ULong k = 0 ; j != end() ; j++, k++) {
        if (i == k)
            return (*j)->_handle ;
    }

    throw ArrayIndexOutOfBounds();
}

// ---------------------------------------------------------------------------
ULong 
ParameterHandleValuePairSet::getValueLength(ULong i) const
    throw (ArrayIndexOutOfBounds)
{
    list<ParameterHandleValuePair *>::const_iterator j = begin();
    for (ULong k = 0 ; j != end() ; j++,k++) {
        if (i == k)
            return (*j)->_valueLength ;
    }

    throw ArrayIndexOutOfBounds();
}

// ---------------------------------------------------------------------------
void 
ParameterHandleValuePairSet::getValue(ULong i,
                                      char* buff,
                                      ULong& value_length) const
    throw (ArrayIndexOutOfBounds)
{
    list<ParameterHandleValuePair *>::const_iterator j = begin();
    for (ULong k = 0 ; j != end() ; j++, k++) {
        if (i == k) {
            value_length = (*j)->_valueLength ;
            memcpy(buff, (*j)->_value, (*j)->_valueLength);
            return ;
        }
    }

    throw ArrayIndexOutOfBounds();
}

// ---------------------------------------------------------------------------
char *
ParameterHandleValuePairSet::getValuePointer(ULong i,
                                             ULong& value_length) const
    throw (ArrayIndexOutOfBounds)
{
    list<ParameterHandleValuePair *>::const_iterator j = begin();
    for (ULong k = 0 ; j != end() ; j++, k++) {
        if (i == k) {
            value_length = (*j)->_valueLength ;
            return (*j)->_value ;
        }
    }

    throw ArrayIndexOutOfBounds();
}

// ---------------------------------------------------------------------------
inline TransportType 
ParameterHandleValuePairSet::getTransportType(void) const
    throw (InvalidHandleValuePairSetContext) 
{
    return _transport ;
}

// ---------------------------------------------------------------------------
inline OrderType 
ParameterHandleValuePairSet::getOrderType(void) const
    throw (InvalidHandleValuePairSetContext)
{
    return _order ;
}

// ---------------------------------------------------------------------------
Region *
ParameterHandleValuePairSet::getRegion(void) const
    throw (InvalidHandleValuePairSetContext, 
           UnimplementedService)//CERTI
{
    throw UnimplementedService();
}

// ---------------------------------------------------------------------------
void 
ParameterHandleValuePairSet::add(Handle h,
                                 const char* buff,
                                 ULong value_length)
    throw (ValueLengthExceeded, ValueCountExceeded)
{
    ParameterHandleValuePair *phvp ;
    phvp = new ParameterHandleValuePair(h, buff, value_length);

    push_front(phvp);
}

// ---------------------------------------------------------------------------
void 
ParameterHandleValuePairSet::remove(Handle h)
    throw (ArrayIndexOutOfBounds)
{
    list<ParameterHandleValuePair *>::iterator j ;
    for (j = begin() ; j != end() ; j++) {
        if ((*j)->_handle == h) {
            delete (*j);
            erase(j);
            return ;
        }
    }

    throw ArrayIndexOutOfBounds();
}

// ---------------------------------------------------------------------------
void 
ParameterHandleValuePairSet::moveFrom(const ParameterHandleValuePairSet&,
                                      ULong&)
    throw (ValueCountExceeded, ArrayIndexOutOfBounds,
           UnimplementedService)//CERTI
{
    throw UnimplementedService();
}

// ---------------------------------------------------------------------------
void 
ParameterHandleValuePairSet::empty(void) 
{
    while (!list<ParameterHandleValuePair *>::empty()) {
        delete front();
        pop_front();
    }
}

// ---------------------------------------------------------------------------
ULong 
ParameterHandleValuePairSet::start(void) const
{
    //not implemented
    return 0 ;
}

// ---------------------------------------------------------------------------
ULong 
ParameterHandleValuePairSet::valid(ULong) const
{
    //not implemented
    return 0 ;
}

// ---------------------------------------------------------------------------
ULong 
ParameterHandleValuePairSet::next(ULong) const
{
    //not implemented
    return 0 ;
}

//-----------------------------------------------------------------------
// ParameterSetFactory
//-----------------------------------------------------------------------

ParameterHandleValuePairSet* 
ParameterSetFactory::create(ULong)
    throw (MemoryExhausted, ValueCountExceeded,
           HandleValuePairMaximumExceeded)
{
    ParameterHandleValuePairSet *phvps = new ParameterHandleValuePairSet ;
    phvps->_order = RECEIVE ;
    phvps->_transport = RELIABLE ;
    return phvps ;
}

}

// $Id: GAV_aux.cc,v 3.6 2003/01/26 23:14:05 breholee Exp $
