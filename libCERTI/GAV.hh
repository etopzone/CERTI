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

#ifndef CERTI_GAV_HH
#define CERTI_GAV_HH

#include "certi.hh"
#include "Exception.hh"

#include <list>

namespace certi {

class AttributeHandleSet
    : public std::list<AttributeHandle>
{
public:
    AttributeHandleSet(ULong size);
    virtual ~AttributeHandleSet();

    virtual ULong size() const ;

    virtual AttributeHandle getHandle(ULong i) const
        throw (ArrayIndexOutOfBounds);

    virtual void add(AttributeHandle h)
        throw (ArrayIndexOutOfBounds, AttributeNotDefined);

    virtual void remove(AttributeHandle h)
        throw (AttributeNotDefined);

    virtual void empty();

    virtual bool isEmpty() const ;
    virtual bool isMember(AttributeHandle h) const ;
};

class FederateHandleSet : public std::list<FederateHandle>
{
public:
    FederateHandleSet(ULong size);
    virtual ~FederateHandleSet();

    virtual ULong size() const ;

    virtual FederateHandle getHandle(ULong i) const
        throw (ArrayIndexOutOfBounds);

    virtual void add(FederateHandle h)
        throw (ValueCountExceeded);

    virtual void remove(FederateHandle h)
        throw (ArrayIndexOutOfBounds);

    virtual void empty();

    virtual bool isMember(FederateHandle h) const ;
};

class AttributeHandleValuePair
{
    // ATTRIBUTES
public:
    Handle _handle ;
    ULong _valueLength ;
    char *_value ;

    // METHODS
public :
    AttributeHandleValuePair(Handle handle,
                             const char *value,
                             ULong value_length);
    ~AttributeHandleValuePair();
};

class AttributeHandleValuePairSet
    : public std::list<AttributeHandleValuePair *>
{
    // ATTRIBUTES
public:
    OrderType _order ;
    TransportType _transport ;

    // METHODS
public:
    AttributeHandleValuePairSet(ULong size);
    virtual ~AttributeHandleValuePairSet();

    virtual ULong size() const ;
    virtual Handle getHandle(ULong i) const throw (ArrayIndexOutOfBounds);
    virtual ULong getValueLength(ULong i) const throw (ArrayIndexOutOfBounds);

    virtual void getValue(ULong i, char *buff, ULong& valueLength) const
        throw (ArrayIndexOutOfBounds);

    virtual char *getValuePointer(ULong i, ULong& valueLength) const
        throw (ArrayIndexOutOfBounds);

    virtual TransportType getTransportType(ULong i) const
        throw (InvalidHandleValuePairSetContext);

    virtual OrderType getOrderType(ULong i) const
        throw (ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext);

    virtual void add(Handle h, const char *buff, ULong valueLength)
        throw (ValueLengthExceeded, ValueCountExceeded);
    virtual void remove(Handle h) throw (ArrayIndexOutOfBounds);

    virtual void moveFrom(const AttributeHandleValuePairSet& ahvps, ULong& i)
        throw (ValueCountExceeded, ArrayIndexOutOfBounds);

    virtual void empty();
    virtual ULong start() const ;
    virtual ULong valid(ULong i) const ;
    virtual ULong next(ULong i) const ;
};

class ParameterHandleValuePair
{
public:
    Handle _handle ;
    ULong _valueLength ;
    char *_value ;

    ParameterHandleValuePair(Handle handle, const char *value, ULong length);
    ~ParameterHandleValuePair();
};

class ParameterHandleValuePairSet
    : public std::list<ParameterHandleValuePair *>
{
public:
    OrderType _order ;
    TransportType _transport ;

public:
    ParameterHandleValuePairSet(ULong size);
    virtual ~ParameterHandleValuePairSet();

    virtual ULong size() const ;

    virtual Handle getHandle(ULong i) const throw (ArrayIndexOutOfBounds);

    virtual ULong getValueLength(ULong i) const throw (ArrayIndexOutOfBounds);

    virtual void getValue(ULong i, char *buff, ULong& valueLength) const
        throw (ArrayIndexOutOfBounds);

    virtual char *getValuePointer(ULong i, ULong& valueLength) const
        throw (ArrayIndexOutOfBounds);

    virtual TransportType getTransportType() const
        throw (InvalidHandleValuePairSetContext);

    virtual OrderType getOrderType() const
        throw (InvalidHandleValuePairSetContext);

    virtual void add(Handle h, const char *buff, ULong valueLength)
        throw (ValueLengthExceeded, ValueCountExceeded);

    virtual void remove(Handle h) throw (ArrayIndexOutOfBounds);

    virtual void moveFrom(const ParameterHandleValuePairSet& phvps, ULong& i)
        throw (ValueCountExceeded, ArrayIndexOutOfBounds);

    virtual void empty();
    virtual ULong start() const ;
    virtual ULong valid(ULong i) const ;
    virtual ULong next(ULong i) const ;
};

} // namespace certi

#endif // CERTI_GAV_HH
