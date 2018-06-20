// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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
// ----------------------------------------------------------------------------

#include "GAV.hh"
#include "PrettyDebug.hh"
#include <string.h>

#include <algorithm>
#include <assert.h>

using std::list;

using namespace certi;

// ----------------------------------------------------------------------------
// AttributeHandleValuePair
// ----------------------------------------------------------------------------

namespace certi {

static PrettyDebug D("GAV", "(gav) - ");

// ----------------------------------------------------------------------------
AttributeHandleValuePair::AttributeHandleValuePair(Handle handle, const char* value, uint32_t value_length)
{
    _handle = handle;
    _valueLength = value_length;

    _value = new char[value_length];
    memcpy(_value, value, value_length);
}

// ----------------------------------------------------------------------------
AttributeHandleValuePair::~AttributeHandleValuePair()
{
    delete[] _value;
}

// ----------------------------------------------------------------------------
// AttributeHandleValuePairSet
// ----------------------------------------------------------------------------
AttributeHandleValuePairSet::AttributeHandleValuePairSet(uint32_t)
{
    _order = RECEIVE;
    _transport = RELIABLE;
}

// ----------------------------------------------------------------------------
AttributeHandleValuePairSet::~AttributeHandleValuePairSet()
{
    empty();
}

// ----------------------------------------------------------------------------
uint32_t AttributeHandleValuePairSet::size() const
{
    return _set.size();
}

// ----------------------------------------------------------------------------
Handle AttributeHandleValuePairSet::getHandle(uint32_t i) const
{
    list<AttributeHandleValuePair*>::const_iterator j = _set.begin();
    for (uint32_t k = 0; j != _set.end(); ++j, ++k) {
        if (i == k)
            return (*j)->_handle;
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
uint32_t AttributeHandleValuePairSet::getValueLength(uint32_t i) const
{
    list<AttributeHandleValuePair*>::const_iterator j = _set.begin();
    for (uint32_t k = 0; j != _set.end(); ++j, ++k) {
        if (i == k)
            return (*j)->_valueLength;
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
void AttributeHandleValuePairSet::getValue(uint32_t i, char* buff, uint32_t& value_length) const
{
    list<AttributeHandleValuePair*>::const_iterator j = _set.begin();
    for (uint32_t k = 0; j != _set.end(); ++j, ++k) {
        if (i == k) {
            value_length = (*j)->_valueLength;
            memcpy(buff, (*j)->_value, (*j)->_valueLength);
            return;
        }
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
char* AttributeHandleValuePairSet::getValuePointer(uint32_t i, uint32_t& value_length) const
{
    list<AttributeHandleValuePair*>::const_iterator j = _set.begin();
    for (uint32_t k = 0; j != _set.end(); ++j, ++k) {
        if (i == k) {
            value_length = (*j)->_valueLength;
            return (*j)->_value;
        }
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
TransportType AttributeHandleValuePairSet::getTransportType(uint32_t) const
{
    return _transport;
}

// ----------------------------------------------------------------------------
OrderType AttributeHandleValuePairSet::getOrderType(uint32_t) const
{
    return _order;
}

// ----------------------------------------------------------------------------
void AttributeHandleValuePairSet::add(Handle h, const char* buff, uint32_t value_length)
{
    AttributeHandleValuePair* ahvp;
    ahvp = new AttributeHandleValuePair(h, buff, value_length);

    _set.push_front(ahvp);
}

// ----------------------------------------------------------------------------
void AttributeHandleValuePairSet::remove(Handle h)
{
    list<AttributeHandleValuePair*>::iterator j;
    for (j = _set.begin(); j != _set.end(); j++) {
        if ((*j)->_handle == h) {
            delete (*j);
            _set.erase(j);
            return;
        }
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
void AttributeHandleValuePairSet::moveFrom(const AttributeHandleValuePairSet&, uint32_t&)
{
    throw RTIinternalError("Unimplemented service");
}

// ----------------------------------------------------------------------------
void AttributeHandleValuePairSet::empty()
{
    while (!_set.empty()) {
        delete _set.front();
        _set.pop_front();
    }
}

// ----------------------------------------------------------------------------
uint32_t AttributeHandleValuePairSet::start() const
{
    //not implemented
    return 0;
}

// ----------------------------------------------------------------------------
uint32_t AttributeHandleValuePairSet::valid(uint32_t) const
{
    //not implemented
    return 0;
}

// ----------------------------------------------------------------------------
uint32_t AttributeHandleValuePairSet::next(uint32_t) const
{
    //not implemented
    return 0;
}

// ----------------------------------------------------------------------------
// AttributeHandleSet
// ----------------------------------------------------------------------------
AttributeHandleSet::AttributeHandleSet(uint32_t /*size*/)
{
}

AttributeHandleSet::~AttributeHandleSet()
{
    empty();
}

// ----------------------------------------------------------------------------
inline uint32_t AttributeHandleSet::size() const
{
    return _set.size();
}

// ----------------------------------------------------------------------------
AttributeHandle AttributeHandleSet::getHandle(uint32_t i) const
{
    list<AttributeHandle>::const_iterator h;
    uint32_t j;
    for (j = 0, h = _set.begin(); h != _set.end(); h++, j++) {
        if (i == j)
            return (*h);
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
void AttributeHandleSet::add(AttributeHandle h)
{
    _set.push_front(h);
}

// ----------------------------------------------------------------------------
void AttributeHandleSet::remove(AttributeHandle h) // not guaranteed safe while iterating
{
    if (isMember(h) == true)
        _set.remove(h);
    else
        throw AttributeNotDefined("AttributeHandleSet::remove(AttributeHandle) Attribute <" + std::to_string(h)
                                  + "> not member of the set");
}

// ----------------------------------------------------------------------------
void AttributeHandleSet::empty()
{
    _set.clear();
}

// ----------------------------------------------------------------------------
bool AttributeHandleSet::isEmpty() const
{
    return _set.empty();
}

// ----------------------------------------------------------------------------
bool AttributeHandleSet::isMember(AttributeHandle h) const
{
    return find(_set.begin(), _set.end(), h) != _set.end();
}

// ----------------------------------------------------------------------------
// FederateHandleSet
// ----------------------------------------------------------------------------
FederateHandleSet::~FederateHandleSet()
{
    empty();
}

// ----------------------------------------------------------------------------
inline uint32_t FederateHandleSet::size() const
{
    return _set.size();
}

// ----------------------------------------------------------------------------
FederateHandle FederateHandleSet::getHandle(uint32_t i) const
{
    list<FederateHandle>::const_iterator h;
    uint32_t j;
    for (j = 0, h = _set.begin(); h != _set.end(); h++, j++) {
        if (i == j)
            return (*h);
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
void FederateHandleSet::add(FederateHandle h)
{
    _set.push_front(h);
}

// ----------------------------------------------------------------------------
void FederateHandleSet::remove(FederateHandle h)
{
    if (isMember(h) == true)
        _set.remove(h);
    else
        throw AttributeNotDefined("FederateHandleSet::remove(FederateHandle) Federate <" + std::to_string(h)
                                  + "> not member of the set");
}

// ----------------------------------------------------------------------------
void FederateHandleSet::empty()
{
    _set.clear();
}

// ----------------------------------------------------------------------------
bool FederateHandleSet::isMember(FederateHandle h) const
{
    return find(_set.begin(), _set.end(), h) != _set.end();
}

// ----------------------------------------------------------------------------
// ParameterHandleValuePair
// ----------------------------------------------------------------------------

ParameterHandleValuePair::ParameterHandleValuePair(Handle handle, const char* value, uint32_t value_length)
{
    _handle = handle;
    _valueLength = value_length;

    _value = (char*) malloc(value_length);
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
ParameterHandleValuePairSet::ParameterHandleValuePairSet(uint32_t)
{
    _order = RECEIVE;
    _transport = RELIABLE;
}

ParameterHandleValuePairSet::~ParameterHandleValuePairSet()
{
    empty();
}

// ----------------------------------------------------------------------------
uint32_t ParameterHandleValuePairSet::size() const
{
    return _set.size();
}

// ----------------------------------------------------------------------------
Handle ParameterHandleValuePairSet::getHandle(uint32_t i) const
{
    list<ParameterHandleValuePair*>::const_iterator j = _set.begin();
    for (uint32_t k = 0; j != _set.end(); ++j, ++k) {
        if (i == k)
            return (*j)->_handle;
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
uint32_t ParameterHandleValuePairSet::getValueLength(uint32_t i) const
{
    list<ParameterHandleValuePair*>::const_iterator j = _set.begin();
    for (uint32_t k = 0; j != _set.end(); ++j, ++k) {
        if (i == k)
            return (*j)->_valueLength;
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
void ParameterHandleValuePairSet::getValue(uint32_t i, char* buff, uint32_t& value_length) const
{
    list<ParameterHandleValuePair*>::const_iterator j = _set.begin();
    for (uint32_t k = 0; j != _set.end(); ++j, ++k) {
        if (i == k) {
            value_length = (*j)->_valueLength;
            memcpy(buff, (*j)->_value, (*j)->_valueLength);
            return;
        }
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
char* ParameterHandleValuePairSet::getValuePointer(uint32_t i, uint32_t& value_length) const
{
    list<ParameterHandleValuePair*>::const_iterator j = _set.begin();
    for (uint32_t k = 0; j != _set.end(); ++j, ++k) {
        if (i == k) {
            value_length = (*j)->_valueLength;
            return (*j)->_value;
        }
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
inline TransportType ParameterHandleValuePairSet::getTransportType() const
{
    return _transport;
}

// ----------------------------------------------------------------------------
inline OrderType ParameterHandleValuePairSet::getOrderType() const
{
    return _order;
}

// ----------------------------------------------------------------------------
void ParameterHandleValuePairSet::add(Handle h, const char* buff, uint32_t value_length)
{
    ParameterHandleValuePair* phvp;
    phvp = new ParameterHandleValuePair(h, buff, value_length);

    _set.push_front(phvp);
}

// ----------------------------------------------------------------------------
void ParameterHandleValuePairSet::remove(Handle h)
{
    list<ParameterHandleValuePair*>::iterator j;
    for (j = _set.begin(); j != _set.end(); j++) {
        if ((*j)->_handle == h) {
            delete (*j);
            _set.erase(j);
            return;
        }
    }

    throw ArrayIndexOutOfBounds("");
}

// ----------------------------------------------------------------------------
void ParameterHandleValuePairSet::moveFrom(const ParameterHandleValuePairSet&, uint32_t&)
{
    throw RTIinternalError("Unimplemented service");
}

// ----------------------------------------------------------------------------
void ParameterHandleValuePairSet::empty()
{
    while (!_set.empty()) {
        delete _set.front();
        _set.pop_front();
    }
}

// ----------------------------------------------------------------------------
uint32_t ParameterHandleValuePairSet::start() const
{
    //not implemented
    return 0;
}

// ----------------------------------------------------------------------------
uint32_t ParameterHandleValuePairSet::valid(uint32_t) const
{
    //not implemented
    return 0;
}

// ----------------------------------------------------------------------------
uint32_t ParameterHandleValuePairSet::next(uint32_t) const
{
    //not implemented
    return 0;
}

} // namespace certi
