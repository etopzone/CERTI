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
// $Id: GAV.hh,v 3.4 2003/04/22 16:42:27 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_GAV_HH
#define _CERTI_GAV_HH

#include <config.h>
#include "RTItypes.hh"
#include "converter.hh"
#include "PrettyDebug.hh"

#include <list>
#include <algorithm>
#include <assert.h>

using std::list ;
using std::find ;

namespace certi {

class AttributeHandleSetImp : public list<AttributeHandle>, AttributeHandleSet
{
public:
    virtual ~AttributeHandleSetImp(void);

    virtual ULong size(void) const ;

    virtual AttributeHandle getHandle(ULong i) const
        throw (ArrayIndexOutOfBounds);

    virtual void add(AttributeHandle h)
        throw (ArrayIndexOutOfBounds, AttributeNotDefined);

    virtual void remove(AttributeHandle h)
        throw (AttributeNotDefined);

    virtual void empty(void);

    virtual Boolean isEmpty(void) const ;
    virtual Boolean isMember(AttributeHandle h) const ;
};

class FederateHandleSetImp : public list<FederateHandle>, FederateHandleSet
{
public:
    virtual ~FederateHandleSetImp(void);

    virtual ULong size(void) const ;

    virtual FederateHandle getHandle(ULong i) const
        throw (ArrayIndexOutOfBounds);

    virtual void add(FederateHandle h)
        throw (ValueCountExceeded);

    virtual void remove(FederateHandle h)
        throw (AttributeNotDefined, ArrayIndexOutOfBounds);

    virtual void empty(void);

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
    ~AttributeHandleValuePair(void);
};

class AttributeHandleValuePairSetImp
    : public list<AttributeHandleValuePair *>, AttributeHandleValuePairSet
{
    // ATTRIBUTES
public:
    OrderType _order ;
    TransportType _transport ;

    // METHODS
public:
    virtual ~AttributeHandleValuePairSetImp(void);

    virtual ULong size(void) const ;
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

    virtual void empty(void);
    virtual ULong start(void) const ;
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
    CAttributeHandleValuePair(void);
    ~CAttributeHandleValuePair(void);

    void AfficheAttribute(void);
};

class CAttributeHandleValuePairSet
{
protected:
    CAttributeHandleValuePair *_head ;
public:
    UShort _size ;

public:
    CAttributeHandleValuePairSet(void);
    CAttributeHandleValuePairSet(const AttributeHandleValuePairSet & ahvps);
    CAttributeHandleValuePairSet(const AttributeHandleSet & ahs);
    ~CAttributeHandleValuePairSet(void);

    void add(CAttributeHandleValuePair *att);
    CAttributeHandleValuePair *getWithHandle(AttributeHandle) const ;
    CAttributeHandleValuePair *getWithName(AttributeName) const ;
    CAttributeHandleValuePair *getIeme(UShort) const ;
    void del(AttributeHandle);
    void empty(void);

    AttributeHandleValuePairSet* toAHVPS(void) const ;
};

class CParameterHandleValuePair
{
public:
    ParameterHandle _param ;
    Value _value ;
    CParameterHandleValuePair *_next ;

public:
    CParameterHandleValuePair(void);
    ~CParameterHandleValuePair(void);

    void AfficheParameter(void);
};

class ParameterHandleValuePairSet ;

class CParameterHandleValuePairSet
{
protected:
    CParameterHandleValuePair *_head ;
public:
    UShort _size ;

public:
    CParameterHandleValuePairSet(void);
    CParameterHandleValuePairSet(const ParameterHandleValuePairSet & phvps);
    ~CParameterHandleValuePairSet(void);

    void add(CParameterHandleValuePair *par);
    CParameterHandleValuePair *getWithHandle(ParameterHandle) const ;
    CParameterHandleValuePair *getWithName(ParameterName) const ;
    CParameterHandleValuePair *getIeme(UShort) const ;
    void del(ParameterHandle);
    void empty(void);

    ParameterHandleValuePairSet* toPHVPS(void) const ;
};

class ParameterHandleValuePair
{
public:
    Handle _handle ;
    ULong _valueLength ;
    char *_value ;

    ParameterHandleValuePair(Handle handle, const char *value, ULong length);
    ~ParameterHandleValuePair(void);
};

class ParameterHandleValuePairSetImp
    : public list<ParameterHandleValuePair *>, ParameterHandleValuePairSet
{
public:
    OrderType _order ;
    TransportType _transport ;

public:
    ParameterHandleValuePairSetImp(ULong size);
    virtual ~ParameterHandleValuePairSetImp(void);

    virtual ULong size(void) const ;

    virtual Handle getHandle(ULong i) const throw (ArrayIndexOutOfBounds);

    virtual ULong getValueLength(ULong i) const throw (ArrayIndexOutOfBounds);

    virtual void getValue(ULong i, char *buff, ULong& valueLength) const
        throw (ArrayIndexOutOfBounds);

    virtual char *getValuePointer(ULong i, ULong& valueLength) const
        throw (ArrayIndexOutOfBounds);

    virtual TransportType getTransportType(void) const
        throw (InvalidHandleValuePairSetContext);

    virtual OrderType getOrderType(void) const
        throw (InvalidHandleValuePairSetContext);

    virtual Region *getRegion(void) const
        throw (InvalidHandleValuePairSetContext, UnimplementedService); //CERTI

    virtual void add(Handle h, const char *buff, ULong valueLength)
        throw (ValueLengthExceeded, ValueCountExceeded);

    virtual void remove(Handle h) throw (ArrayIndexOutOfBounds);

    virtual void moveFrom(const ParameterHandleValuePairSet& phvps, ULong& i)
        throw (ValueCountExceeded,
               ArrayIndexOutOfBounds,
               UnimplementedService);

    virtual void empty(void);
    virtual ULong start(void) const ;
    virtual ULong valid(ULong i) const ;
    virtual ULong next(ULong i) const ;
};

} // namespace certi

#endif // _CERTI_GAV_HH
