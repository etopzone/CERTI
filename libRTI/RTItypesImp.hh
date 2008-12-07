// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI-libRTI
//
// CERTI-libRTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libRTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: RTItypesImp.hh,v 3.1 2008/12/07 20:16:17 gotthardp Exp $
// ----------------------------------------------------------------------------

#ifndef _RTI_TYPESIMP_HH
#define _RTI_TYPESIMP_HH

#include "certi.hh"
#include "Extent.hh"

namespace certi {

template<class I>
struct cast
{
    template<class R>
    const I& operator()(const R& imp)
    {
        try {
            return dynamic_cast<const I&>(imp);
        }
        catch (...)
        {
            throw RTI::RTIinternalError("Incompatible object on input.");
        }
    }
};

typedef std::pair<AttributeHandle, AttributeValue_t> AttributeHandleValuePair_t;

class RTI_EXPORT AttributeHandleValuePairSetImp
    : public RTI::AttributeHandleValuePairSet, protected std::vector<AttributeHandleValuePair_t>
{
public:
    AttributeHandleValuePairSetImp(ULong);
    AttributeHandleValuePairSetImp(const std::vector<AttributeHandleValuePair_t> &);

    virtual ~AttributeHandleValuePairSetImp();

    virtual ULong size() const;

    virtual Handle getHandle(ULong) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual ULong getValueLength(ULong) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void getValue(ULong, char *, ULong &) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual char *getValuePointer(ULong, ULong &) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual TransportType getTransportType(ULong) const
        throw (RTI::InvalidHandleValuePairSetContext);

    virtual OrderType getOrderType(ULong) const
        throw (RTI::ArrayIndexOutOfBounds, RTI::InvalidHandleValuePairSetContext);

    virtual RTI::Region *getRegion(ULong) const
        throw (RTI::ArrayIndexOutOfBounds, RTI::InvalidHandleValuePairSetContext);

    virtual void add(Handle, const char *, ULong)
        throw (RTI::ValueLengthExceeded, RTI::ValueCountExceeded);

    virtual void remove(Handle h)
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void moveFrom(const AttributeHandleValuePairSet &, ULong &)
        throw (RTI::ValueCountExceeded, RTI::ArrayIndexOutOfBounds);

    virtual void empty();

    virtual ULong start() const;
    virtual ULong valid(ULong i) const;
    virtual ULong next(ULong i) const;

    const std::vector<AttributeHandleValuePair_t>& getAttributeHandleValuePairs() const;

protected:
    OrderType _order ;
    TransportType _transport ;
};

class RTI_EXPORT AttributeHandleSetImp
    : public RTI::AttributeHandleSet, protected std::vector<AttributeHandle>
{
public:
    AttributeHandleSetImp(ULong);
    AttributeHandleSetImp(const std::vector<AttributeHandle> &);

    virtual ~AttributeHandleSetImp();

    virtual ULong size() const;

    virtual AttributeHandle getHandle(ULong) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void add(AttributeHandle)
        throw (RTI::ArrayIndexOutOfBounds, RTI::AttributeNotDefined);

    virtual void remove(AttributeHandle)
        throw (RTI::AttributeNotDefined);

    virtual void empty();

    virtual RTI::Boolean isEmpty() const;
    virtual RTI::Boolean isMember(AttributeHandle h) const;

    const std::vector<AttributeHandle>& getAttributeHandles() const;
};

class RTI_EXPORT FederateHandleSetImp
    : public RTI::FederateHandleSet, protected std::vector<FederateHandle>
{
public:
    FederateHandleSetImp(ULong);
    virtual ~FederateHandleSetImp();

    virtual ULong size() const;

    virtual FederateHandle getHandle(ULong) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void add(FederateHandle)
        throw (RTI::ValueCountExceeded);

    virtual void remove(FederateHandle)
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void empty();

    virtual RTI::Boolean isMember(FederateHandle) const;
};

typedef std::pair<ParameterHandle, ParameterValue_t> ParameterHandleValuePair_t;

class RTI_EXPORT ParameterHandleValuePairSetImp
    : public RTI::ParameterHandleValuePairSet, protected std::vector<ParameterHandleValuePair_t>
{
public:
    ParameterHandleValuePairSetImp(ULong);
    ParameterHandleValuePairSetImp(const std::vector<ParameterHandleValuePair_t> &);
    virtual ~ParameterHandleValuePairSetImp();

    virtual ULong size() const;

    virtual Handle getHandle(ULong) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual ULong getValueLength(ULong) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void getValue(ULong, char *, ULong &) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual char *getValuePointer(ULong, ULong &) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual TransportType getTransportType() const
        throw (RTI::InvalidHandleValuePairSetContext);

    virtual OrderType getOrderType() const
        throw (RTI::InvalidHandleValuePairSetContext);

    virtual RTI::Region *getRegion() const
        throw (RTI::InvalidHandleValuePairSetContext);

    virtual void add(Handle, const char *, ULong)
        throw (RTI::ValueLengthExceeded, RTI::ValueCountExceeded);

    virtual void remove(Handle)
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void moveFrom(const ParameterHandleValuePairSet &, ULong &)
        throw (RTI::ValueCountExceeded, RTI::ArrayIndexOutOfBounds);

    virtual void empty();

    virtual ULong start() const;
    virtual ULong valid(ULong i) const;
    virtual ULong next(ULong i) const;

    const std::vector<ParameterHandleValuePair_t>& getParameterHandleValuePairs() const;

protected:
    OrderType _order ;
    TransportType _transport ;
};


class RTI_EXPORT RegionImp : public RTI::Region
{
public:
    RegionImp(RegionHandle, SpaceHandle, const std::vector<Extent> &);
    virtual ~RegionImp();

    virtual ULong getRangeLowerBound(ExtentIndex, DimensionHandle) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual ULong getRangeUpperBound(ExtentIndex, DimensionHandle) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void setRangeLowerBound(ExtentIndex, DimensionHandle, ULong)
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void setRangeUpperBound(ExtentIndex, DimensionHandle, ULong)
        throw (RTI::ArrayIndexOutOfBounds);

    virtual SpaceHandle getSpaceHandle() const
        throw ();

    virtual ULong getNumberOfExtents() const
        throw ();

    virtual ULong getRangeLowerBoundNotificationLimit(ExtentIndex, DimensionHandle) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual ULong getRangeUpperBoundNotificationLimit(ExtentIndex, DimensionHandle) const
        throw (RTI::ArrayIndexOutOfBounds);

private:
    RegionHandle handle;
    SpaceHandle space;
    std::vector<Extent> extents;

    std::vector<Extent> coExtents;
};

}

#endif // _RTI_TYPESIMP_HH

// $Id: RTItypesImp.hh,v 3.1 2008/12/07 20:16:17 gotthardp Exp $
