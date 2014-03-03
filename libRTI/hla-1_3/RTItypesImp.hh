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
// $Id: RTItypesImp.hh,v 1.1 2014/03/03 15:18:23 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _RTI_TYPESIMP_HH
#define _RTI_TYPESIMP_HH

#include "Extent.hh"
#include "RTI.hh"
#include "certi.hh"

namespace certi {

template<class I>
struct certi_cast
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

    template<class R>
    I& operator()(R& imp)
    {
        try {
            return dynamic_cast<I&>(imp);
        }
        catch (...)
        {
            throw RTI::RTIinternalError("Incompatible object on input.");
        }
    }
};

typedef std::pair<AttributeHandle, AttributeValue_t> AttributeHandleValuePair_t;

class RTI_EXPORT AttributeHandleValuePairSetImp : public RTI::AttributeHandleValuePairSet
{
public:
    AttributeHandleValuePairSetImp(RTI::ULong);
    AttributeHandleValuePairSetImp(const std::vector<AttributeHandleValuePair_t> &);

    virtual ~AttributeHandleValuePairSetImp();

    virtual RTI::ULong size() const;

    virtual RTI::Handle getHandle(RTI::ULong) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual RTI::ULong getValueLength(RTI::ULong) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void getValue(RTI::ULong, char *, RTI::ULong &) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual char *getValuePointer(RTI::ULong, RTI::ULong &) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual RTI::TransportType getTransportType(RTI::ULong) const
        throw (RTI::InvalidHandleValuePairSetContext);

    virtual RTI::OrderType getOrderType(RTI::ULong) const
        throw (RTI::ArrayIndexOutOfBounds, RTI::InvalidHandleValuePairSetContext);

    virtual RTI::Region *getRegion(RTI::ULong) const
        throw (RTI::ArrayIndexOutOfBounds, RTI::InvalidHandleValuePairSetContext);

    virtual void add(RTI::Handle, const char *,RTI:: ULong)
        throw (RTI::ValueLengthExceeded, RTI::ValueCountExceeded);

    virtual void remove(RTI::Handle h)
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void moveFrom(const AttributeHandleValuePairSet &, RTI::ULong &)
        throw (RTI::ValueCountExceeded, RTI::ArrayIndexOutOfBounds);

    virtual void empty();

    virtual RTI::ULong start() const;
    virtual RTI::ULong valid(RTI::ULong i) const;
    virtual RTI::ULong next(RTI::ULong i) const;

    const std::vector<AttributeHandleValuePair_t>& getAttributeHandleValuePairs() const;

protected:
    std::vector<AttributeHandleValuePair_t> _set;
    OrderType _order ;
    TransportType _transport ;
};

class RTI_EXPORT AttributeHandleSetImp : public RTI::AttributeHandleSet
{
public:
    AttributeHandleSetImp(RTI::ULong);
    AttributeHandleSetImp(const std::vector<RTI::AttributeHandle> &val);
    AttributeHandleSetImp(const std::vector<certi::AttributeHandle> &val);

    virtual ~AttributeHandleSetImp();

    virtual RTI::ULong size() const;

    virtual RTI::AttributeHandle getHandle(RTI::ULong) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void add(RTI::AttributeHandle)
        throw (RTI::ArrayIndexOutOfBounds, RTI::AttributeNotDefined);

    virtual void remove(RTI::AttributeHandle)
        throw (RTI::AttributeNotDefined);

    virtual void empty();

    virtual RTI::Boolean isEmpty() const;
    virtual RTI::Boolean isMember(RTI::AttributeHandle h) const;

    const std::vector<RTI::AttributeHandle>& getAttributeHandles() const;

protected:
    std::vector<RTI::AttributeHandle> _set;
};

class RTI_EXPORT FederateHandleSetImp : public RTI::FederateHandleSet
{
public:
    FederateHandleSetImp(RTI::ULong);
    virtual ~FederateHandleSetImp();

    virtual RTI::ULong size() const;

    virtual RTI::FederateHandle getHandle(RTI::ULong) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void add(RTI::FederateHandle)
        throw (RTI::ValueCountExceeded);

    virtual void remove(RTI::FederateHandle)
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void empty();

    virtual RTI::Boolean isMember(RTI::FederateHandle) const;

protected:
    std::vector<RTI::FederateHandle> _set;
};

typedef std::pair<RTI::ParameterHandle, ParameterValue_t> ParameterHandleValuePair_t;

class RTI_EXPORT ParameterHandleValuePairSetImp : public RTI::ParameterHandleValuePairSet
{
public:
    ParameterHandleValuePairSetImp(RTI::ULong);
    ParameterHandleValuePairSetImp(const std::vector<ParameterHandleValuePair_t> &);
    virtual ~ParameterHandleValuePairSetImp();

    virtual RTI::ULong size() const;

    virtual RTI::Handle getHandle(RTI::ULong) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual RTI::ULong getValueLength(RTI::ULong) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void getValue(RTI::ULong, char *, RTI::ULong &) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual char *getValuePointer(RTI::ULong, RTI::ULong &) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual RTI::TransportType getTransportType() const
        throw (RTI::InvalidHandleValuePairSetContext);

    virtual RTI::OrderType getOrderType() const
        throw (RTI::InvalidHandleValuePairSetContext);

    virtual RTI::Region *getRegion() const
        throw (RTI::InvalidHandleValuePairSetContext);

    virtual void add(RTI::Handle, const char *, RTI::ULong)
        throw (RTI::ValueLengthExceeded, RTI::ValueCountExceeded);

    virtual void remove(RTI::Handle)
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void moveFrom(const ParameterHandleValuePairSet &, RTI::ULong &)
        throw (RTI::ValueCountExceeded, RTI::ArrayIndexOutOfBounds);

    virtual void empty();

    virtual RTI::ULong start() const;
    virtual RTI::ULong valid(RTI::ULong i) const;
    virtual RTI::ULong next(RTI::ULong i) const;

    const std::vector<ParameterHandleValuePair_t>& getParameterHandleValuePairs() const;

protected:
    std::vector<ParameterHandleValuePair_t> _set;
    RTI::OrderType _order ;
    RTI::TransportType _transport ;
};


class RTI_EXPORT RegionImp : public RTI::Region
{
public:
    RegionImp(RegionHandle, RTI::SpaceHandle, const std::vector<Extent>&);
    virtual ~RegionImp();

    virtual RTI::ULong getRangeLowerBound(RTI::ExtentIndex, RTI::DimensionHandle) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual RTI::ULong getRangeUpperBound(RTI::ExtentIndex, RTI::DimensionHandle) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void setRangeLowerBound(RTI::ExtentIndex, RTI::DimensionHandle, RTI::ULong)
        throw (RTI::ArrayIndexOutOfBounds);

    virtual void setRangeUpperBound(RTI::ExtentIndex, RTI::DimensionHandle, RTI::ULong)
        throw (RTI::ArrayIndexOutOfBounds);

    virtual RTI::SpaceHandle getSpaceHandle() const
        throw ();

    virtual RTI::ULong getNumberOfExtents() const
        throw ();

    virtual RTI::ULong getRangeLowerBoundNotificationLimit(RTI::ExtentIndex, RTI::DimensionHandle) const
        throw (RTI::ArrayIndexOutOfBounds);

    virtual RTI::ULong getRangeUpperBoundNotificationLimit(RTI::ExtentIndex, RTI::DimensionHandle) const
        throw (RTI::ArrayIndexOutOfBounds);

    RegionHandle getHandle() const;

    const std::vector<Extent>& getExtents() const;

    void commit();

private:
    RegionHandle handle;
    RTI::SpaceHandle space;
    std::vector<Extent> extents;
    // extents used in last notifyAboutRegionModification
    std::vector<Extent> effectiveExtents;
};

}

#endif // _RTI_TYPESIMP_HH

// $Id: RTItypesImp.hh,v 1.1 2014/03/03 15:18:23 erk Exp $
