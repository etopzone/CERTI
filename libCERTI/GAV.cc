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
// $Id: GAV.cc,v 3.18.4.1 2009/01/05 13:34:51 gotthardp Exp $
// ----------------------------------------------------------------------------

#include "GAV.hh"
#include "PrettyDebug.hh"
#include <string.h>

#include <algorithm>
#include <assert.h>

using std::list ;

using namespace certi ;

// ----------------------------------------------------------------------------
// AttributeHandleValuePair
// ----------------------------------------------------------------------------

namespace certi {

static pdCDebug D("GAV", "(gav) - ");

// ----------------------------------------------------------------------------
AttributeHandleValuePair::AttributeHandleValuePair(Handle handle,
                                                   const char *value,
                                                   ULong value_length)
{
    _handle = handle ;
    _valueLength = value_length ;

    _value = new char[value_length];
    memcpy(_value, value, value_length);
}

// ----------------------------------------------------------------------------
AttributeHandleValuePair::~AttributeHandleValuePair()
{
    delete[] _value ;
}

// ----------------------------------------------------------------------------
// AttributeHandleValuePairSet
// ----------------------------------------------------------------------------
AttributeHandleValuePairSet::AttributeHandleValuePairSet(ULong)
{
    _order = RECEIVE ;
    _transport = RELIABLE ;
}

// ----------------------------------------------------------------------------
AttributeHandleValuePairSet::~AttributeHandleValuePairSet()
{
    empty();
}

// ----------------------------------------------------------------------------
ULong
AttributeHandleValuePairSet::size() const
{
    return list<AttributeHandleValuePair *>::size();
}

// ----------------------------------------------------------------------------
Handle
AttributeHandleValuePairSet::getHandle(ULong i) const
    throw (ArrayIndexOutOfBounds)
{
    list<AttributeHandleValuePair *>::const_iterator j = begin();
    for (ULong k = 0 ; j != end(); j++, k++) {
        if (i == k)
            return (*j)->_handle ;
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
ULong
AttributeHandleValuePairSet::getValueLength(ULong i) const
    throw (ArrayIndexOutOfBounds)
{
    list<AttributeHandleValuePair *>::const_iterator j = begin();
    for (ULong k = 0 ; j != end(); j++, k++) {
        if (i == k)
            return (*j)->_valueLength ;
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
void
AttributeHandleValuePairSet::getValue(ULong i,
                                         char *buff,
                                         ULong& value_length) const
    throw (ArrayIndexOutOfBounds)
{
    list<AttributeHandleValuePair *>::const_iterator j = begin();
    for (ULong k = 0 ; j != end(); j++, k++) {
        if (i == k) {
            value_length = (*j)->_valueLength ;
            memcpy(buff, (*j)->_value, (*j)->_valueLength);
            return ;
        }
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
char *
AttributeHandleValuePairSet::getValuePointer(ULong i,
                                                ULong& value_length) const
    throw (ArrayIndexOutOfBounds)
{
    list<AttributeHandleValuePair *>::const_iterator j = begin();
    for (ULong k = 0 ; j != end(); j++, k++) {
        if (i == k) {
            value_length = (*j)->_valueLength ;
            return (*j)->_value ;
        }
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
TransportType
AttributeHandleValuePairSet::getTransportType(ULong) const
    throw (InvalidHandleValuePairSetContext)
{
    return _transport ;
}

// ----------------------------------------------------------------------------
OrderType
AttributeHandleValuePairSet::getOrderType(ULong) const
    throw (ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext)
{
    return _order ;
}

// ----------------------------------------------------------------------------
void
AttributeHandleValuePairSet::add(Handle h,
                                    const char *buff,
                                    ULong value_length)
    throw (ValueLengthExceeded, ValueCountExceeded)
{
    AttributeHandleValuePair *ahvp ;
    ahvp = new AttributeHandleValuePair(h, buff, value_length);

    push_front(ahvp);
}

// ----------------------------------------------------------------------------
void
AttributeHandleValuePairSet::remove(Handle h)
    throw (ArrayIndexOutOfBounds)
{
    list<AttributeHandleValuePair *>::iterator j ;
    for (j = begin(); j != end(); j++) {
        if ((*j)->_handle == h) {
            delete (*j);
            erase(j);
            return ;
        }
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
void
AttributeHandleValuePairSet::moveFrom(const AttributeHandleValuePairSet&,
                                         ULong&)
    throw (ValueCountExceeded, ArrayIndexOutOfBounds)
{
    throw RTIinternalError("Unimplemented service");
}

// ----------------------------------------------------------------------------
void
AttributeHandleValuePairSet::empty()
{
    while (!list<AttributeHandleValuePair *>::empty()) {
        delete front();
        pop_front();
    }
}

// ----------------------------------------------------------------------------
ULong
AttributeHandleValuePairSet::start() const
{
    //not implemented
    return 0 ;
}

// ----------------------------------------------------------------------------
ULong
AttributeHandleValuePairSet::valid(ULong) const
{
    //not implemented
    return 0 ;
}

// ----------------------------------------------------------------------------
ULong
AttributeHandleValuePairSet::next(ULong) const
{
    //not implemented
    return 0 ;
}

// ----------------------------------------------------------------------------
// AttributeHandleSet
// ----------------------------------------------------------------------------
AttributeHandleSet::AttributeHandleSet(ULong size)
{
}

AttributeHandleSet::~AttributeHandleSet()
{
    empty();
}

// ----------------------------------------------------------------------------
inline ULong
AttributeHandleSet::size() const
{
    return list<AttributeHandle>::size();
}

// ----------------------------------------------------------------------------
AttributeHandle
AttributeHandleSet::getHandle(ULong i) const
    throw (ArrayIndexOutOfBounds)
{
    list<AttributeHandle>::const_iterator h ;
    ULong j ;
    for (j = 0, h = begin(); h != end(); h++, j++) {
        if (i == j)
            return (*h);
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
void
AttributeHandleSet::add(AttributeHandle h)
    throw (ArrayIndexOutOfBounds, AttributeNotDefined)
{
    push_front(h);
}

// ----------------------------------------------------------------------------
void
AttributeHandleSet::remove(AttributeHandle h)
    throw (AttributeNotDefined)// not guaranteed safe while iterating
{
    if (isMember(h) == true)
        list<AttributeHandle>::remove(h);
    else
        throw AttributeNotDefined("");
}

// ----------------------------------------------------------------------------
void
AttributeHandleSet::empty()
{
    list<AttributeHandle>::clear();
}

// ----------------------------------------------------------------------------
bool
AttributeHandleSet::isEmpty() const
{
    return list<AttributeHandle>::empty();
}

// ----------------------------------------------------------------------------
bool
AttributeHandleSet::isMember(AttributeHandle h) const
{
    return find(begin(), end(), h) != end();
}

// ----------------------------------------------------------------------------
// FederateHandleSet
// ----------------------------------------------------------------------------
FederateHandleSet::~FederateHandleSet()
{
    empty();
}

// ----------------------------------------------------------------------------
inline ULong
FederateHandleSet::size() const
{
    return list<FederateHandle>::size();
}

// ----------------------------------------------------------------------------
FederateHandle
FederateHandleSet::getHandle(ULong i) const
    throw (ArrayIndexOutOfBounds)
{
    list<FederateHandle>::const_iterator h ;
    ULong j ;
    for (j = 0, h = begin(); h != end(); h++, j++) {
        if (i == j)
            return (*h);
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
void
FederateHandleSet::add(FederateHandle h)
    throw (ValueCountExceeded)
{
    push_front(h);
}

// ----------------------------------------------------------------------------
void
FederateHandleSet::remove(FederateHandle h)
    throw (ArrayIndexOutOfBounds)
{
    if (isMember(h) == true)
        list<FederateHandle>::remove(h);
    else
        throw AttributeNotDefined("");
}

// ----------------------------------------------------------------------------
void
FederateHandleSet::empty()
{
    list<FederateHandle>::clear();
}

// ----------------------------------------------------------------------------
bool
FederateHandleSet::isMember(FederateHandle h) const
{
    return find(begin(), end(), h) != end();
}

// ----------------------------------------------------------------------------
// ParameterHandleValuePair
// ----------------------------------------------------------------------------

ParameterHandleValuePair::ParameterHandleValuePair(Handle handle,
                                                   const char *value,
                                                   ULong value_length)
{
    _handle = handle ;
    _valueLength = value_length ;

    _value = (char *) malloc(value_length);
    memcpy(_value, value, value_length);
}

// ----------------------------------------------------------------------------
ParameterHandleValuePair::~ParameterHandleValuePair()
{
    free(_value);
}

// ----------------------------------------------------------------------------
// ParameterHandleValuePairSet
// ----------------------------------------------------------------------------
ParameterHandleValuePairSet::ParameterHandleValuePairSet(ULong)
{
    _order = RECEIVE ;
    _transport = RELIABLE ;
}

ParameterHandleValuePairSet::~ParameterHandleValuePairSet()
{
    empty();
}

// ----------------------------------------------------------------------------
ULong
ParameterHandleValuePairSet::size() const
{
    return list<ParameterHandleValuePair *>::size();
}

// ----------------------------------------------------------------------------
Handle
ParameterHandleValuePairSet::getHandle(ULong i) const
    throw (ArrayIndexOutOfBounds)
{
    list<ParameterHandleValuePair *>::const_iterator j = begin();
    for (ULong k = 0 ; j != end(); j++, k++) {
        if (i == k)
            return (*j)->_handle ;
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
ULong
ParameterHandleValuePairSet::getValueLength(ULong i) const
    throw (ArrayIndexOutOfBounds)
{
    list<ParameterHandleValuePair *>::const_iterator j = begin();
    for (ULong k = 0 ; j != end(); j++, k++) {
        if (i == k)
            return (*j)->_valueLength ;
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
void
ParameterHandleValuePairSet::getValue(ULong i,
                                         char *buff,
                                         ULong& value_length) const
    throw (ArrayIndexOutOfBounds)
{
    list<ParameterHandleValuePair *>::const_iterator j = begin();
    for (ULong k = 0 ; j != end(); j++, k++) {
        if (i == k) {
            value_length = (*j)->_valueLength ;
            memcpy(buff, (*j)->_value, (*j)->_valueLength);
            return ;
        }
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
char *
ParameterHandleValuePairSet::getValuePointer(ULong i,
                                                ULong& value_length) const
    throw (ArrayIndexOutOfBounds)
{
    list<ParameterHandleValuePair *>::const_iterator j = begin();
    for (ULong k = 0 ; j != end(); j++, k++) {
        if (i == k) {
            value_length = (*j)->_valueLength ;
            return (*j)->_value ;
        }
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
inline TransportType
ParameterHandleValuePairSet::getTransportType() const
    throw (InvalidHandleValuePairSetContext)
{
    return _transport ;
}

// ----------------------------------------------------------------------------
inline OrderType
ParameterHandleValuePairSet::getOrderType() const
    throw (InvalidHandleValuePairSetContext)
{
    return _order ;
}

// ----------------------------------------------------------------------------
void
ParameterHandleValuePairSet::add(Handle h,
                                    const char *buff,
                                    ULong value_length)
    throw (ValueLengthExceeded, ValueCountExceeded)
{
    ParameterHandleValuePair *phvp ;
    phvp = new ParameterHandleValuePair(h, buff, value_length);

    push_front(phvp);
}

// ----------------------------------------------------------------------------
void
ParameterHandleValuePairSet::remove(Handle h)
    throw (ArrayIndexOutOfBounds)
{
    list<ParameterHandleValuePair *>::iterator j ;
    for (j = begin(); j != end(); j++) {
        if ((*j)->_handle == h) {
            delete (*j);
            erase(j);
            return ;
        }
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
void
ParameterHandleValuePairSet::moveFrom(const ParameterHandleValuePairSet&,
                                         ULong&)
    throw (ValueCountExceeded, ArrayIndexOutOfBounds)
{
    throw RTIinternalError("Unimplemented service");
}

// ----------------------------------------------------------------------------
void
ParameterHandleValuePairSet::empty()
{
    while (!list<ParameterHandleValuePair *>::empty()) {
        delete front();
        pop_front();
    }
}

// ----------------------------------------------------------------------------
ULong
ParameterHandleValuePairSet::start() const
{
    //not implemented
    return 0 ;
}

// ----------------------------------------------------------------------------
ULong
ParameterHandleValuePairSet::valid(ULong) const
{
    //not implemented
    return 0 ;
}

// ----------------------------------------------------------------------------
ULong
ParameterHandleValuePairSet::next(ULong) const
{
    //not implemented
    return 0 ;
}

} // namespace certi

// $Id: GAV.cc,v 3.18.4.1 2009/01/05 13:34:51 gotthardp Exp $

