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
#include "GAV.hh"

#include "PrettyDebug.hh"
#include "converter.hh"

#include <algorithm>
#include <assert.h>

using std::list ;

using namespace certi ;

// ----------------------------------------------------------------------------
// AttributeHandleValuePair
// ----------------------------------------------------------------------------

namespace certi {

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
// AttributeHandleValuePairSetImp
// ----------------------------------------------------------------------------
AttributeHandleValuePairSetImp::~AttributeHandleValuePairSetImp()
{
    empty();
}

// ----------------------------------------------------------------------------
ULong
AttributeHandleValuePairSetImp::size() const
{
    return list<AttributeHandleValuePair *>::size();
}

// ----------------------------------------------------------------------------
Handle
AttributeHandleValuePairSetImp::getHandle(ULong i) const
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
AttributeHandleValuePairSetImp::getValueLength(ULong i) const
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
AttributeHandleValuePairSetImp::getValue(ULong i,
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
AttributeHandleValuePairSetImp::getValuePointer(ULong i,
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
AttributeHandleValuePairSetImp::getTransportType(ULong) const
    throw (RTI::InvalidHandleValuePairSetContext)
{
    return _transport ;
}

// ----------------------------------------------------------------------------
OrderType
AttributeHandleValuePairSetImp::getOrderType(ULong) const
    throw (ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext)
{
    return _order ;
}

// ----------------------------------------------------------------------------
RTI::Region *
AttributeHandleValuePairSetImp::getRegion(ULong) const
    throw (ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext)
{
    throw RTIinternalError("Unimplemented service");
}

// ----------------------------------------------------------------------------
void
AttributeHandleValuePairSetImp::add(Handle h,
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
AttributeHandleValuePairSetImp::remove(Handle h)
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
AttributeHandleValuePairSetImp::moveFrom(const AttributeHandleValuePairSet&,
                                         ULong&)
    throw (ValueCountExceeded, ArrayIndexOutOfBounds)
{
    throw RTIinternalError("Unimplemented service");
}

// ----------------------------------------------------------------------------
void
AttributeHandleValuePairSetImp::empty()
{
    while (!list<AttributeHandleValuePair *>::empty()) {
        delete front();
        pop_front();
    }
}

// ----------------------------------------------------------------------------
ULong
AttributeHandleValuePairSetImp::start() const
{
    //not implemented
    return 0 ;
}

// ----------------------------------------------------------------------------
ULong
AttributeHandleValuePairSetImp::valid(ULong) const
{
    //not implemented
    return 0 ;
}

// ----------------------------------------------------------------------------
ULong
AttributeHandleValuePairSetImp::next(ULong) const
{
    //not implemented
    return 0 ;
}

// ----------------------------------------------------------------------------
// AttributeSetFactory
// ----------------------------------------------------------------------------

RTI::AttributeHandleValuePairSet *
RTI::AttributeSetFactory::create(ULong)
    throw (MemoryExhausted, ValueCountExceeded, HandleValuePairMaximumExceeded)
{
    AttributeHandleValuePairSetImp *ahvps ;
    ahvps = new AttributeHandleValuePairSetImp ;
    ahvps->_order = RECEIVE ;
    ahvps->_transport = RELIABLE ;
    return (AttributeHandleValuePairSet *) ahvps ;
}

// ----------------------------------------------------------------------------
// AttributeHandleSetImp
// ----------------------------------------------------------------------------
AttributeHandleSetImp::~AttributeHandleSetImp()
{
    empty();
}

// ----------------------------------------------------------------------------
inline ULong
AttributeHandleSetImp::size() const
{
    return list<AttributeHandle>::size();
}

// ----------------------------------------------------------------------------
AttributeHandle
AttributeHandleSetImp::getHandle(ULong i) const
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
AttributeHandleSetImp::add(AttributeHandle h)
    throw (ArrayIndexOutOfBounds, AttributeNotDefined)
{
    push_front(h);
}

// ----------------------------------------------------------------------------
void
AttributeHandleSetImp::remove(AttributeHandle h)
    throw (AttributeNotDefined)// not guaranteed safe while iterating
{
    if (isMember(h) == RTI::RTI_TRUE)
        list<AttributeHandle>::remove(h);
    else
        throw AttributeNotDefined("");
}

// ----------------------------------------------------------------------------
void
AttributeHandleSetImp::empty()
{
    list<AttributeHandle>::clear();
}

// ----------------------------------------------------------------------------
RTI::Boolean
AttributeHandleSetImp::isEmpty() const
{
    return RTI::Boolean(list<AttributeHandle>::empty());
}

// ----------------------------------------------------------------------------
RTI::Boolean
AttributeHandleSetImp::isMember(AttributeHandle h) const
{
    return RTI::Boolean(find(begin(), end(), h) != end());
}

// ----------------------------------------------------------------------------
// AttributeHandleSetImpFactory
// ----------------------------------------------------------------------------

RTI::AttributeHandleSet *
RTI::AttributeHandleSetFactory::create(ULong)
    throw (MemoryExhausted, ValueCountExceeded)
{
    return new AttributeHandleSetImp();
}

// ----------------------------------------------------------------------------
// FederateHandleSetImp
// ----------------------------------------------------------------------------
FederateHandleSetImp::~FederateHandleSetImp()
{
    empty();
}

// ----------------------------------------------------------------------------
inline ULong
FederateHandleSetImp::size() const
{
    return list<FederateHandle>::size();
}

// ----------------------------------------------------------------------------
FederateHandle
FederateHandleSetImp::getHandle(ULong i) const
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
FederateHandleSetImp::add(FederateHandle h)
    throw (ValueCountExceeded)
{
    push_front(h);
}

// ----------------------------------------------------------------------------
void
FederateHandleSetImp::remove(FederateHandle h)
    throw (ArrayIndexOutOfBounds)
{
    if (isMember(h) == RTI::RTI_TRUE)
        list<FederateHandle>::remove(h);
    else
        throw AttributeNotDefined("");
}

// ----------------------------------------------------------------------------
void
FederateHandleSetImp::empty()
{
    list<FederateHandle>::clear();
}

// ----------------------------------------------------------------------------
RTI::Boolean
FederateHandleSetImp::isMember(FederateHandle h) const
{
    return RTI::Boolean(find(begin(), end(), h) != end());
}

// ----------------------------------------------------------------------------
// FederateHandleSetFactory
// ----------------------------------------------------------------------------

RTI::FederateHandleSet *
RTI::FederateHandleSetFactory::create(ULong)
    throw (MemoryExhausted, ValueCountExceeded)
{
    return ((FederateHandleSet *) new FederateHandleSetImp());
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
// ParameterHandleValuePairSetImp
// ----------------------------------------------------------------------------
ParameterHandleValuePairSetImp::ParameterHandleValuePairSetImp(ULong)
{
    _order = RECEIVE ;
    _transport = RELIABLE ;
}

ParameterHandleValuePairSetImp::~ParameterHandleValuePairSetImp()
{
    empty();
}

// ----------------------------------------------------------------------------
ULong
ParameterHandleValuePairSetImp::size() const
{
    return list<ParameterHandleValuePair *>::size();
}

// ----------------------------------------------------------------------------
Handle
ParameterHandleValuePairSetImp::getHandle(ULong i) const
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
ParameterHandleValuePairSetImp::getValueLength(ULong i) const
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
ParameterHandleValuePairSetImp::getValue(ULong i,
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
ParameterHandleValuePairSetImp::getValuePointer(ULong i,
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
ParameterHandleValuePairSetImp::getTransportType() const
    throw (InvalidHandleValuePairSetContext)
{
    return _transport ;
}

// ----------------------------------------------------------------------------
inline OrderType
ParameterHandleValuePairSetImp::getOrderType() const
    throw (InvalidHandleValuePairSetContext)
{
    return _order ;
}

// ----------------------------------------------------------------------------
RTI::Region *
ParameterHandleValuePairSetImp::getRegion() const
    throw (InvalidHandleValuePairSetContext)
{
    throw RTIinternalError("Unimplemented service");
}

// ----------------------------------------------------------------------------
void
ParameterHandleValuePairSetImp::add(Handle h,
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
ParameterHandleValuePairSetImp::remove(Handle h)
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
ParameterHandleValuePairSetImp::moveFrom(const ParameterHandleValuePairSet&,
                                         ULong&)
    throw (ValueCountExceeded, ArrayIndexOutOfBounds)
{
    throw RTIinternalError("Unimplemented service");
}

// ----------------------------------------------------------------------------
void
ParameterHandleValuePairSetImp::empty()
{
    while (!list<ParameterHandleValuePair *>::empty()) {
        delete front();
        pop_front();
    }
}

// ----------------------------------------------------------------------------
ULong
ParameterHandleValuePairSetImp::start() const
{
    //not implemented
    return 0 ;
}

// ----------------------------------------------------------------------------
ULong
ParameterHandleValuePairSetImp::valid(ULong) const
{
    //not implemented
    return 0 ;
}

// ----------------------------------------------------------------------------
ULong
ParameterHandleValuePairSetImp::next(ULong) const
{
    //not implemented
    return 0 ;
}

// ----------------------------------------------------------------------------
// ParameterSetFactory
// ----------------------------------------------------------------------------

RTI::ParameterHandleValuePairSet *
RTI::ParameterSetFactory::create(ULong size)
    throw (MemoryExhausted, ValueCountExceeded, HandleValuePairMaximumExceeded)
{
    ParameterHandleValuePairSetImp *phvps ;
    phvps = new ParameterHandleValuePairSetImp(size);

    phvps->_order = RECEIVE ;
    phvps->_transport = RELIABLE ;
    return (ParameterHandleValuePairSet *) phvps ;
}

} // namespace certi
