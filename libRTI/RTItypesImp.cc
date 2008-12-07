// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2006  ONERA
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
// $Id: RTItypesImp.cc,v 3.1 2008/12/07 20:16:17 gotthardp Exp $
// ----------------------------------------------------------------------------

#include "certi.hh"

#include "RTItypesImp.hh"
#include "GAV.hh"
#include "PrettyDebug.hh"

#include <algorithm>
#include <string.h>

using namespace certi ;

namespace {

static PrettyDebug D("LIBRTI", __FILE__);
static PrettyDebug G("GENDOC",__FILE__);

}

// ----------------------------------------------------------------------------
AttributeHandleValuePairSetImp::AttributeHandleValuePairSetImp(ULong size)
{
    _order = RECEIVE;
    _transport = RELIABLE;

    std::vector<AttributeHandleValuePair_t>::reserve(size);
}

AttributeHandleValuePairSetImp::AttributeHandleValuePairSetImp(const std::vector<AttributeHandleValuePair_t> &val)
    : std::vector<AttributeHandleValuePair_t>(val)
{
    _order = RECEIVE;
    _transport = RELIABLE;
}

AttributeHandleValuePairSetImp::~AttributeHandleValuePairSetImp()
{
}

ULong AttributeHandleValuePairSetImp::size() const
{
    return std::vector<AttributeHandleValuePair_t>::size();
}

Handle AttributeHandleValuePairSetImp::getHandle(ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return operator[](i).first;
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

ULong AttributeHandleValuePairSetImp::getValueLength(ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return operator[](i).second.size();
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

void AttributeHandleValuePairSetImp::getValue(ULong i, char *buff, ULong &len) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size()) {
        const AttributeHandleValuePair_t& item = operator[](i);
        len = item.second.size();
        memcpy(buff, item.second.data(), len);
    }
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

char *AttributeHandleValuePairSetImp::getValuePointer(ULong i, ULong &len) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size()) {
        const AttributeHandleValuePair_t& item = operator[](i);
        len = item.second.size();
        return (char *)item.second.data();
    }
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

TransportType AttributeHandleValuePairSetImp::getTransportType(ULong) const
    throw (RTI::InvalidHandleValuePairSetContext)
{
    return _transport;
}

OrderType AttributeHandleValuePairSetImp::getOrderType(ULong) const
    throw (RTI::ArrayIndexOutOfBounds, RTI::InvalidHandleValuePairSetContext)
{
    return _order;
}

RTI::Region *AttributeHandleValuePairSetImp::getRegion(ULong) const
    throw (RTI::ArrayIndexOutOfBounds, RTI::InvalidHandleValuePairSetContext)
{
    throw RTI::RTIinternalError("unimplemented function getRegion()");
}

void AttributeHandleValuePairSetImp::add(Handle h, const char *str, ULong len)
    throw (RTI::ValueLengthExceeded, RTI::ValueCountExceeded)
{
    push_back(AttributeHandleValuePair_t(h, std::string(str, len)));
}

void AttributeHandleValuePairSetImp::remove(Handle h)
    throw (RTI::ArrayIndexOutOfBounds)
{
    for (iterator pos = begin(); pos != end(); pos++) {
        if (pos->first == h) {
            std::vector<AttributeHandleValuePair_t>::erase(pos);
            return;
        }
    }

    throw RTI::ArrayIndexOutOfBounds("");
}

void AttributeHandleValuePairSetImp::moveFrom(const AttributeHandleValuePairSet &, ULong &)
    throw (RTI::ValueCountExceeded, RTI::ArrayIndexOutOfBounds)
{
    throw RTI::RTIinternalError("unimplemented function moveFrom()");
}

void AttributeHandleValuePairSetImp::empty()
{
    std::vector<AttributeHandleValuePair_t>::clear();
}

ULong AttributeHandleValuePairSetImp::start() const
{
    // not implemented
    return 0 ;
}

ULong AttributeHandleValuePairSetImp::valid(ULong i) const
{
    // not implemented
    return 0 ;
}

ULong AttributeHandleValuePairSetImp::next(ULong i) const
{
    // not implemented
    return 0 ;
}

const std::vector<AttributeHandleValuePair_t>&
AttributeHandleValuePairSetImp::getAttributeHandleValuePairs() const
{
    return *this;
}

// ----------------------------------------------------------------------------
AttributeHandleSetImp::AttributeHandleSetImp(ULong size)
{
    std::vector<AttributeHandle>::reserve(size);
}

AttributeHandleSetImp::AttributeHandleSetImp(const std::vector<AttributeHandle> &val)
    : std::vector<AttributeHandle>(val)
{
}

AttributeHandleSetImp::~AttributeHandleSetImp()
{
}

ULong AttributeHandleSetImp::size() const
{
    return std::vector<AttributeHandle>::size();
}

AttributeHandle AttributeHandleSetImp::getHandle(ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return operator[](i);
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

void AttributeHandleSetImp::add(AttributeHandle h)
    throw (RTI::ArrayIndexOutOfBounds, RTI::AttributeNotDefined)
{
    std::vector<AttributeHandle>::push_back(h);
}

void AttributeHandleSetImp::remove(AttributeHandle h)
    throw (RTI::AttributeNotDefined)
{
    iterator pos = std::find(begin(), end(), h);
    if (pos != end())
        std::vector<AttributeHandle>::erase(pos);
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

void AttributeHandleSetImp::empty()
{
    std::vector<AttributeHandle>::clear();
}

RTI::Boolean AttributeHandleSetImp::isEmpty() const
{
    return RTI::Boolean(std::vector<AttributeHandle>::empty());
}

RTI::Boolean AttributeHandleSetImp::isMember(AttributeHandle h) const
{
    return RTI::Boolean(std::find(begin(), end(), h) != end());
}

const std::vector<AttributeHandle>&
AttributeHandleSetImp::getAttributeHandles() const
{
    return *this;
}

// ----------------------------------------------------------------------------
FederateHandleSetImp::FederateHandleSetImp(ULong size)
{
    std::vector<FederateHandle>::reserve(size);
}

FederateHandleSetImp::~FederateHandleSetImp()
{
}

ULong FederateHandleSetImp::size() const
{
    return std::vector<FederateHandle>::size();
}

FederateHandle FederateHandleSetImp::getHandle(ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return operator[](i);
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

void FederateHandleSetImp::add(FederateHandle h)
    throw (RTI::ValueCountExceeded)
{
    std::vector<FederateHandle>::push_back(h);
}

void FederateHandleSetImp::remove(FederateHandle h)
    throw (RTI::ArrayIndexOutOfBounds)
{
    iterator pos = std::find(begin(), end(), h);
    if (pos != end())
        std::vector<FederateHandle>::erase(pos);
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

void FederateHandleSetImp::empty()
{
    std::vector<AttributeHandle>::clear();
}

RTI::Boolean FederateHandleSetImp::isMember(FederateHandle h) const
{
    return RTI::Boolean(std::find(begin(), end(), h) != end());
}

// ----------------------------------------------------------------------------
ParameterHandleValuePairSetImp::ParameterHandleValuePairSetImp(ULong size)
{
    _order = RECEIVE;
    _transport = RELIABLE;

    std::vector<ParameterHandleValuePair_t>::reserve(size);
}

ParameterHandleValuePairSetImp::ParameterHandleValuePairSetImp(const std::vector<ParameterHandleValuePair_t> &val)
    : std::vector<ParameterHandleValuePair_t>(val)
{
    _order = RECEIVE;
    _transport = RELIABLE;
}

ParameterHandleValuePairSetImp::~ParameterHandleValuePairSetImp()
{
}

ULong ParameterHandleValuePairSetImp::size() const
{
    return std::vector<ParameterHandleValuePair_t>::size();
}

Handle ParameterHandleValuePairSetImp::getHandle(ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return operator[](i).first;
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

ULong ParameterHandleValuePairSetImp::getValueLength(ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return operator[](i).second.size();
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

void ParameterHandleValuePairSetImp::getValue(ULong i, char *buff, ULong &len) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size()) {
        const ParameterHandleValuePair_t& item = operator[](i);
        len = item.second.size();
        memcpy(buff, item.second.data(), len);
    }
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

char *ParameterHandleValuePairSetImp::getValuePointer(ULong i, ULong &len) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size()) {
        const ParameterHandleValuePair_t& item = operator[](i);
        len = item.second.size();
        return (char *)item.second.data();
    }
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

TransportType ParameterHandleValuePairSetImp::getTransportType() const
    throw (RTI::InvalidHandleValuePairSetContext)
{
    return _transport;
}

OrderType ParameterHandleValuePairSetImp::getOrderType() const
    throw (RTI::InvalidHandleValuePairSetContext)
{
    return _order;
}

RTI::Region *ParameterHandleValuePairSetImp::getRegion() const
    throw (RTI::InvalidHandleValuePairSetContext)
{
    throw RTI::RTIinternalError("unimplemented function getRegion()");
}

void ParameterHandleValuePairSetImp::add(Handle h, const char *str, ULong len)
    throw (RTI::ValueLengthExceeded, RTI::ValueCountExceeded)
{
    push_back(ParameterHandleValuePair_t(h, std::string(str, len)));
}

void ParameterHandleValuePairSetImp::remove(Handle h)
    throw (RTI::ArrayIndexOutOfBounds)
{
    for (iterator pos = begin(); pos != end(); pos++) {
        if (pos->first == h) {
            std::vector<ParameterHandleValuePair_t>::erase(pos);
            return;
        }
    }

    throw RTI::ArrayIndexOutOfBounds("");
}

void ParameterHandleValuePairSetImp::moveFrom(const ParameterHandleValuePairSet &, ULong &)
    throw (RTI::ValueCountExceeded, RTI::ArrayIndexOutOfBounds)
{
    throw RTI::RTIinternalError("unimplemented function moveFrom()");
}

void ParameterHandleValuePairSetImp::empty()
{
    return std::vector<ParameterHandleValuePair_t>::clear();
}

ULong ParameterHandleValuePairSetImp::start() const
{
    // not implemented
    return 0;
}

ULong ParameterHandleValuePairSetImp::valid(ULong i) const
{
    // not implemented
    return 0;
}

ULong ParameterHandleValuePairSetImp::next(ULong i) const
{
    // not implemented
    return 0;
}

const std::vector<ParameterHandleValuePair_t> &
ParameterHandleValuePairSetImp::getParameterHandleValuePairs() const
{
    return *this;
}

// ----------------------------------------------------------------------------
RTI::AttributeHandleValuePairSet *
RTI::AttributeSetFactory::create(ULong size)
    throw (MemoryExhausted, ValueCountExceeded, HandleValuePairMaximumExceeded)
{
    return new AttributeHandleValuePairSetImp(size);
}

// ----------------------------------------------------------------------------
RTI::AttributeHandleSet *
RTI::AttributeHandleSetFactory::create(ULong size)
    throw (MemoryExhausted, ValueCountExceeded)
{
    return new AttributeHandleSetImp(size);
}

// ----------------------------------------------------------------------------
RTI::FederateHandleSet *
RTI::FederateHandleSetFactory::create(ULong size)
    throw (MemoryExhausted, ValueCountExceeded)
{
    return new FederateHandleSetImp(size);
}

// ----------------------------------------------------------------------------
RTI::ParameterHandleValuePairSet *
RTI::ParameterSetFactory::create(ULong size)
    throw (MemoryExhausted, ValueCountExceeded, HandleValuePairMaximumExceeded)
{
    return new ParameterHandleValuePairSetImp(size);
}

// ----------------------------------------------------------------------------
RegionImp::RegionImp(RegionHandle h, SpaceHandle s, const std::vector<Extent> &ext)
    : handle(h), space(s), coExtents(ext)
{
}

RegionImp::~RegionImp()
{
}

ULong RegionImp::getRangeLowerBound(ExtentIndex index, DimensionHandle dimension) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (index < extents.size())
        return extents[index].getRangeLowerBound(dimension);
    else
        throw RTI::ArrayIndexOutOfBounds("Extent index above limit");
}

ULong RegionImp::getRangeUpperBound(ExtentIndex index, DimensionHandle dimension) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (index < extents.size())
        return extents[index].getRangeUpperBound(dimension);
    else
        throw RTI::ArrayIndexOutOfBounds("Extent index above limit");
}

void RegionImp::setRangeLowerBound(ExtentIndex index, DimensionHandle dimension, ULong val)
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (index < extents.size())
        extents[index].setRangeLowerBound(dimension, val);
    else
        throw RTI::ArrayIndexOutOfBounds("Extent index above limit");
}

void RegionImp::setRangeUpperBound(ExtentIndex index, DimensionHandle dimension, ULong val)
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (index < extents.size())
        extents[index].setRangeUpperBound(dimension, val);
    else
        throw RTI::ArrayIndexOutOfBounds("Extent index above limit");
}

SpaceHandle RegionImp::getSpaceHandle() const
    throw ()
{
    return space;
}

ULong RegionImp::getNumberOfExtents() const
    throw ()
{
    return coExtents.size();
}

ULong RegionImp::getRangeLowerBoundNotificationLimit(ExtentIndex index, DimensionHandle dimension) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (index < coExtents.size())
        return coExtents[index].getRangeLowerBound(dimension);
    else
        throw RTI::ArrayIndexOutOfBounds("Extent index above limit");
}

ULong RegionImp::getRangeUpperBoundNotificationLimit(ExtentIndex index, DimensionHandle dimension) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (index < coExtents.size())
        return coExtents[index].getRangeUpperBound(dimension);
    else
        throw RTI::ArrayIndexOutOfBounds("Extent index above limit");
}

// $Id: RTItypesImp.cc,v 3.1 2008/12/07 20:16:17 gotthardp Exp $
