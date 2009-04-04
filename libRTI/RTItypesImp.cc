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
// $Id: RTItypesImp.cc,v 3.3 2009/04/04 13:18:12 gotthardp Exp $
// ----------------------------------------------------------------------------

#include "certi.hh"

#include "RTItypesImp.hh"
#include "GAV.hh"
#include "PrettyDebug.hh"

#include <algorithm>
#include <string.h>

using namespace certi ;

static PrettyDebug D("LIBRTI", __FILE__);
static PrettyDebug G("GENDOC",__FILE__);


// ----------------------------------------------------------------------------
AttributeHandleValuePairSetImp::AttributeHandleValuePairSetImp(ULong size)
{
    _order = RECEIVE;
    _transport = RELIABLE;

    _set.reserve(size);
}

AttributeHandleValuePairSetImp::AttributeHandleValuePairSetImp(const std::vector<AttributeHandleValuePair_t> &val)
    : _set(val)
{
    _order = RECEIVE;
    _transport = RELIABLE;
}

AttributeHandleValuePairSetImp::~AttributeHandleValuePairSetImp()
{
}

ULong AttributeHandleValuePairSetImp::size() const
{
    return _set.size();
}

Handle AttributeHandleValuePairSetImp::getHandle(ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return _set[i].first;
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

ULong AttributeHandleValuePairSetImp::getValueLength(ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return _set[i].second.size();
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

void AttributeHandleValuePairSetImp::getValue(ULong i, char *buff, ULong &len) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size()) {
        const AttributeHandleValuePair_t& item = _set[i];
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
        const AttributeHandleValuePair_t& item = _set[i];
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
    _set.push_back(AttributeHandleValuePair_t(h, std::string(str, len)));
}

void AttributeHandleValuePairSetImp::remove(Handle h)
    throw (RTI::ArrayIndexOutOfBounds)
{
    for (std::vector<AttributeHandleValuePair_t>::iterator pos = _set.begin();
            pos != _set.end(); pos++) {
        if (pos->first == h) {
            _set.erase(pos);
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
    _set.clear();
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
    return _set;
}

// ----------------------------------------------------------------------------
AttributeHandleSetImp::AttributeHandleSetImp(ULong size)
{
    _set.reserve(size);
}

AttributeHandleSetImp::AttributeHandleSetImp(const std::vector<AttributeHandle> &val)
    : _set(val)
{
}

AttributeHandleSetImp::~AttributeHandleSetImp()
{
}

ULong AttributeHandleSetImp::size() const
{
    return _set.size();
}

AttributeHandle AttributeHandleSetImp::getHandle(ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return _set[i];
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

void AttributeHandleSetImp::add(AttributeHandle h)
    throw (RTI::ArrayIndexOutOfBounds, RTI::AttributeNotDefined)
{
    _set.push_back(h);
}

void AttributeHandleSetImp::remove(AttributeHandle h)
    throw (RTI::AttributeNotDefined)
{
    std::vector<AttributeHandle>::iterator pos = std::find(_set.begin(), _set.end(), h);
    if (pos != _set.end())
        _set.erase(pos);
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

void AttributeHandleSetImp::empty()
{
    _set.clear();
}

RTI::Boolean AttributeHandleSetImp::isEmpty() const
{
    return RTI::Boolean(_set.empty());
}

RTI::Boolean AttributeHandleSetImp::isMember(AttributeHandle h) const
{
    return RTI::Boolean(std::find(_set.begin(), _set.end(), h) != _set.end());
}

const std::vector<AttributeHandle>&
AttributeHandleSetImp::getAttributeHandles() const
{
    return _set;
}

// ----------------------------------------------------------------------------
FederateHandleSetImp::FederateHandleSetImp(ULong size)
{
    _set.reserve(size);
}

FederateHandleSetImp::~FederateHandleSetImp()
{
}

ULong FederateHandleSetImp::size() const
{
    return _set.size();
}

FederateHandle FederateHandleSetImp::getHandle(ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return _set[i];
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

void FederateHandleSetImp::add(FederateHandle h)
    throw (RTI::ValueCountExceeded)
{
    _set.push_back(h);
}

void FederateHandleSetImp::remove(FederateHandle h)
    throw (RTI::ArrayIndexOutOfBounds)
{
    std::vector<FederateHandle>::iterator pos = std::find(_set.begin(), _set.end(), h);
    if (pos != _set.end())
        _set.erase(pos);
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

void FederateHandleSetImp::empty()
{
    _set.clear();
}

RTI::Boolean FederateHandleSetImp::isMember(FederateHandle h) const
{
    return RTI::Boolean(std::find(_set.begin(), _set.end(), h) != _set.end());
}

// ----------------------------------------------------------------------------
ParameterHandleValuePairSetImp::ParameterHandleValuePairSetImp(ULong size)
{
    _order = RECEIVE;
    _transport = RELIABLE;

    _set.reserve(size);
}

ParameterHandleValuePairSetImp::ParameterHandleValuePairSetImp(const std::vector<ParameterHandleValuePair_t> &val)
    : _set(val)
{
    _order = RECEIVE;
    _transport = RELIABLE;
}

ParameterHandleValuePairSetImp::~ParameterHandleValuePairSetImp()
{
}

ULong ParameterHandleValuePairSetImp::size() const
{
    return _set.size();
}

Handle ParameterHandleValuePairSetImp::getHandle(ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return _set[i].first;
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

ULong ParameterHandleValuePairSetImp::getValueLength(ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return _set[i].second.size();
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

void ParameterHandleValuePairSetImp::getValue(ULong i, char *buff, ULong &len) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size()) {
        const ParameterHandleValuePair_t& item = _set[i];
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
        const ParameterHandleValuePair_t& item = _set[i];
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
    _set.push_back(ParameterHandleValuePair_t(h, std::string(str, len)));
}

void ParameterHandleValuePairSetImp::remove(Handle h)
    throw (RTI::ArrayIndexOutOfBounds)
{
    for (std::vector<ParameterHandleValuePair_t>::iterator pos = _set.begin();
            pos != _set.end(); pos++) {
        if (pos->first == h) {
            _set.erase(pos);
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
    return _set.clear();
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
    return _set;
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

// $Id: RTItypesImp.cc,v 3.3 2009/04/04 13:18:12 gotthardp Exp $
