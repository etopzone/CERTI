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
// $Id: GAV.hh,v 3.7 2003/07/10 15:06:49 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_GAV_HH
#define _CERTI_GAV_HH

#include "RTItypes.hh"

#include <list>

namespace certi {

class AttributeHandleSetImp
    : public std::list<AttributeHandle>, AttributeHandleSet
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

    virtual Boolean isEmpty() const ;
    virtual Boolean isMember(AttributeHandle h) const ;
};

class FederateHandleSetImp : public std::list<FederateHandle>, FederateHandleSet
{
public:
    virtual ~FederateHandleSetImp();

    virtual ULong size() const ;

    virtual FederateHandle getHandle(ULong i) const
        throw (ArrayIndexOutOfBounds);

    virtual void add(FederateHandle h)
        throw (ValueCountExceeded);

    virtual void remove(FederateHandle h)
        throw (AttributeNotDefined, ArrayIndexOutOfBounds);

    virtual void empty();

    virtual Boolean isMember(FederateHandle h) const ;
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
    : public std::list<AttributeHandleValuePair *>, AttributeHandleValuePairSet
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
        throw (ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext);

    virtual OrderType getOrderType(ULong i) const
        throw (ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext);

    virtual Region *getRegion(ULong i) const
        throw (ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext,
               UnimplementedService); //CERTI

    virtual void add(Handle h, const char *buff, ULong valueLength)
        throw (ValueLengthExceeded, ValueCountExceeded);
    virtual void remove(Handle h) throw (ArrayIndexOutOfBounds);

    virtual void moveFrom(const AttributeHandleValuePairSet& ahvps, ULong& i)
        throw (ValueCountExceeded,
               ArrayIndexOutOfBounds,
               UnimplementedService);

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
    Boolean _isPublished ;
    Boolean _isSubscribed ;
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
    CAttributeHandleValuePairSet(const AttributeHandleValuePairSet & ahvps);
    CAttributeHandleValuePairSet(const AttributeHandleSet & ahs);
    ~CAttributeHandleValuePairSet();

    void add(CAttributeHandleValuePair *att);
    CAttributeHandleValuePair *getWithHandle(AttributeHandle) const ;
    CAttributeHandleValuePair *getWithName(const char *) const ;
    CAttributeHandleValuePair *getIeme(UShort) const ;
    void del(AttributeHandle);
    void empty();

    AttributeHandleValuePairSet* toAHVPS() const ;

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

class ParameterHandleValuePairSet ;

class CParameterHandleValuePairSet
{
public:
    UShort _size ;

public:
    CParameterHandleValuePairSet();
    CParameterHandleValuePairSet(const ParameterHandleValuePairSet & phvps);
    ~CParameterHandleValuePairSet();

    void add(CParameterHandleValuePair *par);
    CParameterHandleValuePair *getWithHandle(ParameterHandle) const ;
    CParameterHandleValuePair *getWithName(ParameterName) const ;
    CParameterHandleValuePair *getIeme(UShort) const ;
    void del(ParameterHandle);
    void empty();

    ParameterHandleValuePairSet* toPHVPS() const ;

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
    : public std::list<ParameterHandleValuePair *>, ParameterHandleValuePairSet
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

    virtual Region *getRegion() const
        throw (InvalidHandleValuePairSetContext, UnimplementedService); //CERTI

    virtual void add(Handle h, const char *buff, ULong valueLength)
        throw (ValueLengthExceeded, ValueCountExceeded);

    virtual void remove(Handle h) throw (ArrayIndexOutOfBounds);

    virtual void moveFrom(const ParameterHandleValuePairSet& phvps, ULong& i)
        throw (ValueCountExceeded,
               ArrayIndexOutOfBounds,
               UnimplementedService);

    virtual void empty();
    virtual ULong start() const ;
    virtual ULong valid(ULong i) const ;
    virtual ULong next(ULong i) const ;
};

} // namespace certi

#endif // _CERTI_GAV_HH
