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
// ----------------------------------------------------------------------------

#ifndef CERTI_GAV_HH
#define CERTI_GAV_HH

// #include "certi.hh"
#include "Exception.hh"
#include "Handle.hh"

#include <list>

namespace certi {

class CERTI_EXPORT AttributeHandleSet {
public:
    AttributeHandleSet(uint32_t size);
    virtual ~AttributeHandleSet();

    virtual uint32_t size() const;

    virtual AttributeHandle getHandle(uint32_t i) const;

    virtual void add(AttributeHandle h);

    virtual void remove(AttributeHandle h);

    virtual void empty();

    virtual bool isEmpty() const;
    virtual bool isMember(AttributeHandle h) const;

protected:
    std::list<AttributeHandle> _set;
};

class FederateHandleSet {
public:
    FederateHandleSet(uint32_t size);
    virtual ~FederateHandleSet();

    virtual uint32_t size() const;

    virtual FederateHandle getHandle(uint32_t i) const;

    virtual void add(FederateHandle h);

    virtual void remove(FederateHandle h);

    virtual void empty();

    virtual bool isMember(FederateHandle h) const;

protected:
    std::list<FederateHandle> _set;
};

class AttributeHandleValuePair {
    // ATTRIBUTES
public:
    Handle _handle;
    uint32_t _valueLength;
    char* _value;

    // METHODS
public:
    AttributeHandleValuePair(Handle handle, const char* value, uint32_t value_length);
    ~AttributeHandleValuePair();
};

class AttributeHandleValuePairSet {
    // ATTRIBUTES
public:
    OrderType _order;
    TransportType _transport;

    // METHODS
public:
    AttributeHandleValuePairSet(uint32_t size);
    virtual ~AttributeHandleValuePairSet();

    virtual uint32_t size() const;
    virtual Handle getHandle(uint32_t i) const;
    virtual uint32_t getValueLength(uint32_t i) const;

    virtual void getValue(uint32_t i, char* buff, uint32_t& valueLength) const;

    virtual char* getValuePointer(uint32_t i, uint32_t& valueLength) const;

    virtual TransportType getTransportType(uint32_t i) const;

    virtual OrderType getOrderType(uint32_t i) const;

    virtual void add(Handle h, const char* buff, uint32_t valueLength);
    virtual void remove(Handle h);

    virtual void moveFrom(const AttributeHandleValuePairSet& ahvps, uint32_t& i);

    virtual void empty();
    virtual uint32_t start() const;
    virtual uint32_t valid(uint32_t i) const;
    virtual uint32_t next(uint32_t i) const;

protected:
    std::list<AttributeHandleValuePair*> _set;
};

class ParameterHandleValuePair {
public:
    Handle _handle;
    uint32_t _valueLength;
    char* _value;

    ParameterHandleValuePair(Handle handle, const char* value, uint32_t length);
    ~ParameterHandleValuePair();
};

class ParameterHandleValuePairSet {
public:
    OrderType _order;
    TransportType _transport;

public:
    ParameterHandleValuePairSet(uint32_t size);
    virtual ~ParameterHandleValuePairSet();

    virtual uint32_t size() const;

    virtual Handle getHandle(uint32_t i) const;

    virtual uint32_t getValueLength(uint32_t i) const;

    virtual void getValue(uint32_t i, char* buff, uint32_t& valueLength) const;

    virtual char* getValuePointer(uint32_t i, uint32_t& valueLength) const;

    virtual TransportType getTransportType() const;

    virtual OrderType getOrderType() const;

    virtual void add(Handle h, const char* buff, uint32_t valueLength);

    virtual void remove(Handle h);

    virtual void moveFrom(const ParameterHandleValuePairSet& phvps, uint32_t& i);

    virtual void empty();
    virtual uint32_t start() const;
    virtual uint32_t valid(uint32_t i) const;
    virtual uint32_t next(uint32_t i) const;

protected:
    std::list<ParameterHandleValuePair*> _set;
};

} // namespace certi

#endif // CERTI_GAV_HH
