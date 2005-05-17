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

#include <list>

namespace certi {

class AttributeHandleSetImp
    : public std::list<AttributeHandle>, public AttributeHandleSet
{
public:
    virtual ~AttributeHandleSetImp();

    virtual ULong size() const ;

    virtual AttributeHandle getHandle(ULong i) const
        throw (ArrayIndexOutOfBounds);

    virtual void add(AttributeHandle h)
        throw (ArrayIndexOutOfBounds, AttributeNotDefined);

    virtual void remove(AttributeHandle h)
        throw (AttributeNotDefined);

    virtual void empty();

    virtual RTI::Boolean isEmpty() const ;
    virtual RTI::Boolean isMember(AttributeHandle h) const ;
};

class FederateHandleSetImp : public std::list<FederateHandle>, RTI::FederateHandleSet
{
public:
    virtual ~FederateHandleSetImp();

    virtual ULong size() const ;

    virtual FederateHandle getHandle(ULong i) const
        throw (ArrayIndexOutOfBounds);

    virtual void add(FederateHandle h)
        throw (ValueCountExceeded);

    virtual void remove(FederateHandle h)
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void empty();

    virtual RTI::Boolean isMember(FederateHandle h) const ;
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

class AttributeHandleValuePairSetImp
    : public std::list<AttributeHandleValuePair *>, RTI::AttributeHandleValuePairSet
{
    // ATTRIBUTES
public:
    OrderType _order ;
    TransportType _transport ;

    // METHODS
public:
    virtual ~AttributeHandleValuePairSetImp();

    virtual ULong size() const ;
    virtual Handle getHandle(ULong i) const throw (ArrayIndexOutOfBounds);
    virtual ULong getValueLength(ULong i) const throw (ArrayIndexOutOfBounds);

    virtual void getValue(ULong i, char *buff, ULong& valueLength) const
        throw (ArrayIndexOutOfBounds);

    virtual char *getValuePointer(ULong i, ULong& valueLength) const
        throw (ArrayIndexOutOfBounds);

    virtual TransportType getTransportType(ULong i) const
        throw (RTI::InvalidHandleValuePairSetContext);

    virtual OrderType getOrderType(ULong i) const
        throw (ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext);

    virtual RTI::Region *getRegion(ULong i) const
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

class CAttributeHandleValuePair
{
public:
    OrderType _order ;
    TransportType _transport ;
    RTI::Boolean _isPublished ;
    RTI::Boolean _isSubscribed ;
    AttributeHandle _attrib ;
    Value _value ;
    CAttributeHandleValuePair *_next ;

public:
    CAttributeHandleValuePair();
    ~CAttributeHandleValuePair();

    void AfficheAttribute();
};

class CAttributeHandleValuePairSet
{
public:
    UShort _size ;

public:
    CAttributeHandleValuePairSet();
    CAttributeHandleValuePairSet(const RTI::AttributeHandleValuePairSet & ahvps);
    CAttributeHandleValuePairSet(const RTI::AttributeHandleSet & ahs);
    ~CAttributeHandleValuePairSet();

    void add(CAttributeHandleValuePair *att);
    CAttributeHandleValuePair *getWithHandle(AttributeHandle) const ;
    CAttributeHandleValuePair *getWithName(const char *) const ;
    CAttributeHandleValuePair *getIeme(UShort) const ;
    void del(AttributeHandle);
    void empty();

    RTI::AttributeHandleValuePairSet* toAHVPS() const ;

protected:
    CAttributeHandleValuePair *_head ;
};

class CParameterHandleValuePair
{
public:
    ParameterHandle _param ;
    Value _value ;
    CParameterHandleValuePair *_next ;

public:
    CParameterHandleValuePair();
    ~CParameterHandleValuePair();

    void AfficheParameter();
};

class RTI::ParameterHandleValuePairSet ;

class CParameterHandleValuePairSet
{
public:
    UShort _size ;

public:
    CParameterHandleValuePairSet();
    CParameterHandleValuePairSet(const RTI::ParameterHandleValuePairSet & phvps);
    ~CParameterHandleValuePairSet();

    void add(CParameterHandleValuePair *par);
    CParameterHandleValuePair *getWithHandle(ParameterHandle) const ;
    CParameterHandleValuePair *getWithName(ParameterName) const ;
    CParameterHandleValuePair *getIeme(UShort) const ;
    void del(ParameterHandle);
    void empty();

    RTI::ParameterHandleValuePairSet* toPHVPS() const ;

protected:
    CParameterHandleValuePair *_head ;

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

class ParameterHandleValuePairSetImp
    : public std::list<ParameterHandleValuePair *>, public RTI::ParameterHandleValuePairSet
{
public:
    OrderType _order ;
    TransportType _transport ;

public:
    ParameterHandleValuePairSetImp(ULong size);
    virtual ~ParameterHandleValuePairSetImp();

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

    virtual RTI::Region *getRegion() const
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
