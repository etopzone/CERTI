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
// $Id: RTItypesImp.cc,v 1.1 2014/03/03 15:18:24 erk Exp $
// ----------------------------------------------------------------------------

#include "RTItypesImp.hh"
#include "PrettyDebug.hh"

#include <algorithm>
#include <string.h>

using namespace certi ;

static PrettyDebug D("LIBRTI", __FILE__);
static PrettyDebug G("GENDOC",__FILE__);


// ----------------------------------------------------------------------------
AttributeHandleValuePairSetImp::AttributeHandleValuePairSetImp(RTI::ULong size)
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

RTI::ULong AttributeHandleValuePairSetImp::size() const
{
    return _set.size();
}

RTI::Handle AttributeHandleValuePairSetImp::getHandle(RTI::ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return _set[i].first;
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

RTI::ULong AttributeHandleValuePairSetImp::getValueLength(RTI::ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return _set[i].second.size();
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

void AttributeHandleValuePairSetImp::getValue(RTI::ULong i, char *buff, RTI::ULong &len) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size()) {
        const AttributeHandleValuePair_t& item = _set[i];
        len = item.second.size();
        memcpy(buff, &(item.second[0]), len);
    }
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

char *AttributeHandleValuePairSetImp::getValuePointer(RTI::ULong i, RTI::ULong &len) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size()) {
        const AttributeHandleValuePair_t& item = _set[i];
        len = item.second.size();
        return (char *)&(item.second[0]);
    }
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

RTI::TransportType AttributeHandleValuePairSetImp::getTransportType(RTI::ULong) const
    throw (RTI::InvalidHandleValuePairSetContext)
{
    return _transport;
}

RTI::OrderType AttributeHandleValuePairSetImp::getOrderType(RTI::ULong) const
    throw (RTI::ArrayIndexOutOfBounds, RTI::InvalidHandleValuePairSetContext)
{
    return _order;
}

RTI::Region *AttributeHandleValuePairSetImp::getRegion(RTI::ULong) const
    throw (RTI::ArrayIndexOutOfBounds, RTI::InvalidHandleValuePairSetContext)
{
    throw RTI::RTIinternalError("unimplemented function getRegion()");
}

void AttributeHandleValuePairSetImp::add(RTI::Handle h, const char *str, RTI::ULong len)
    throw (RTI::ValueLengthExceeded, RTI::ValueCountExceeded)
{
    std::vector<char> v;
    v.assign(str,str+len);
    _set.push_back(AttributeHandleValuePair_t(h, v));
}

void AttributeHandleValuePairSetImp::remove(RTI::Handle h)
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

void AttributeHandleValuePairSetImp::moveFrom(const AttributeHandleValuePairSet &, RTI::ULong &)
    throw (RTI::ValueCountExceeded, RTI::ArrayIndexOutOfBounds)
{
    throw RTI::RTIinternalError("unimplemented function moveFrom()");
}

void AttributeHandleValuePairSetImp::empty()
{
    _set.clear();
}

RTI::ULong AttributeHandleValuePairSetImp::start() const
{
    // not implemented
    return 0 ;
}

RTI::ULong AttributeHandleValuePairSetImp::valid(RTI::ULong i) const
{
    // not implemented
    return 0 ;
}

RTI::ULong AttributeHandleValuePairSetImp::next(RTI::ULong i) const
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
AttributeHandleSetImp::AttributeHandleSetImp(RTI::ULong size)
{
    _set.reserve(size);
}

AttributeHandleSetImp::AttributeHandleSetImp(const std::vector<RTI::AttributeHandle> &val)
    : _set(val)
{
}

AttributeHandleSetImp::AttributeHandleSetImp(const std::vector<certi::AttributeHandle> &val)
{
   _set.resize(val.size());
   std::copy(val.begin(), val.end(), _set.begin());
}

AttributeHandleSetImp::~AttributeHandleSetImp()
{
}

RTI::ULong AttributeHandleSetImp::size() const
{
    return _set.size();
}

RTI::AttributeHandle AttributeHandleSetImp::getHandle(RTI::ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return _set[i];
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

void AttributeHandleSetImp::add(RTI::AttributeHandle h)
    throw (RTI::ArrayIndexOutOfBounds, RTI::AttributeNotDefined)
{
    _set.push_back(h);
}

void AttributeHandleSetImp::remove(RTI::AttributeHandle h)
    throw (RTI::AttributeNotDefined)
{
    std::vector<RTI::AttributeHandle>::iterator pos = std::find(_set.begin(), _set.end(), h);
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

RTI::Boolean AttributeHandleSetImp::isMember(RTI::AttributeHandle h) const
{
    return RTI::Boolean(std::find(_set.begin(), _set.end(), h) != _set.end());
}

const std::vector<RTI::AttributeHandle>&
AttributeHandleSetImp::getAttributeHandles() const
{
    return _set;
}

// ----------------------------------------------------------------------------
FederateHandleSetImp::FederateHandleSetImp(RTI::ULong size)
{
    _set.reserve(size);
}

FederateHandleSetImp::~FederateHandleSetImp()
{
}

RTI::ULong FederateHandleSetImp::size() const
{
    return _set.size();
}

RTI::FederateHandle FederateHandleSetImp::getHandle(RTI::ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return _set[i];
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

void FederateHandleSetImp::add(RTI::FederateHandle h)
    throw (RTI::ValueCountExceeded)
{
    _set.push_back(h);
}

void FederateHandleSetImp::remove(RTI::FederateHandle h)
    throw (RTI::ArrayIndexOutOfBounds)
{
    std::vector<RTI::FederateHandle>::iterator pos = std::find(_set.begin(), _set.end(), h);
    if (pos != _set.end())
        _set.erase(pos);
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

void FederateHandleSetImp::empty()
{
    _set.clear();
}

RTI::Boolean FederateHandleSetImp::isMember(RTI::FederateHandle h) const
{
    return RTI::Boolean(std::find(_set.begin(), _set.end(), h) != _set.end());
}

// ----------------------------------------------------------------------------
ParameterHandleValuePairSetImp::ParameterHandleValuePairSetImp(RTI::ULong size)
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

RTI::ULong ParameterHandleValuePairSetImp::size() const
{
    return _set.size();
}

RTI::Handle ParameterHandleValuePairSetImp::getHandle(RTI::ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return _set[i].first;
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

RTI::ULong ParameterHandleValuePairSetImp::getValueLength(RTI::ULong i) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size())
        return _set[i].second.size();
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

void ParameterHandleValuePairSetImp::getValue(RTI::ULong i, char *buff, RTI::ULong &len) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size()) {
        const ParameterHandleValuePair_t& item = _set[i];
        len = item.second.size();
        memcpy(buff, &(item.second[0]), len);
    }
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

char *ParameterHandleValuePairSetImp::getValuePointer(RTI::ULong i, RTI::ULong &len) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (i < size()) {
        const ParameterHandleValuePair_t& item = _set[i];
        len = item.second.size();
        return (char *)&(item.second[0]);
    }
    else
        throw RTI::ArrayIndexOutOfBounds("");
}

RTI::TransportType ParameterHandleValuePairSetImp::getTransportType() const
    throw (RTI::InvalidHandleValuePairSetContext)
{
    return _transport;
}

RTI::OrderType ParameterHandleValuePairSetImp::getOrderType() const
    throw (RTI::InvalidHandleValuePairSetContext)
{
    return _order;
}

RTI::Region *ParameterHandleValuePairSetImp::getRegion() const
    throw (RTI::InvalidHandleValuePairSetContext)
{
    throw RTI::RTIinternalError("unimplemented function getRegion()");
}

void ParameterHandleValuePairSetImp::add(RTI::Handle h, const char *str, RTI::ULong len)
    throw (RTI::ValueLengthExceeded, RTI::ValueCountExceeded)
{
    std::vector<char> v;
    v.assign(str,str+len);
    _set.push_back(ParameterHandleValuePair_t(h, v));
}

void ParameterHandleValuePairSetImp::remove(RTI::Handle h)
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

void ParameterHandleValuePairSetImp::moveFrom(const ParameterHandleValuePairSet &, RTI::ULong &)
    throw (RTI::ValueCountExceeded, RTI::ArrayIndexOutOfBounds)
{
    throw RTI::RTIinternalError("unimplemented function moveFrom()");
}

void ParameterHandleValuePairSetImp::empty()
{
    _set.clear();
}

RTI::ULong ParameterHandleValuePairSetImp::start() const
{
    // not implemented
    return 0;
}

RTI::ULong ParameterHandleValuePairSetImp::valid(RTI::ULong i) const
{
    // not implemented
    return 0;
}

RTI::ULong ParameterHandleValuePairSetImp::next(RTI::ULong i) const
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
RTI::AttributeSetFactory::create(RTI::ULong size)
    throw (MemoryExhausted, ValueCountExceeded, HandleValuePairMaximumExceeded)
{
    return new AttributeHandleValuePairSetImp(size);
}

// ----------------------------------------------------------------------------
RTI::AttributeHandleSet *
RTI::AttributeHandleSetFactory::create(RTI::ULong size)
    throw (MemoryExhausted, ValueCountExceeded)
{
    return new AttributeHandleSetImp(size);
}

// ----------------------------------------------------------------------------
RTI::FederateHandleSet *
RTI::FederateHandleSetFactory::create(RTI::ULong size)
    throw (MemoryExhausted, ValueCountExceeded)
{
    return new FederateHandleSetImp(size);
}

// ----------------------------------------------------------------------------
RTI::ParameterHandleValuePairSet *
RTI::ParameterSetFactory::create(RTI::ULong size)
    throw (MemoryExhausted, ValueCountExceeded, HandleValuePairMaximumExceeded)
{
    return new ParameterHandleValuePairSetImp(size);
}

// ----------------------------------------------------------------------------
RegionImp::RegionImp(RegionHandle h, RTI::SpaceHandle s, const std::vector<Extent> &ext)
    : handle(h), space(s), extents(ext), effectiveExtents(ext)
{
}

RegionImp::~RegionImp()
{
}

RTI::ULong RegionImp::getRangeLowerBound(RTI::ExtentIndex index, RTI::DimensionHandle dimension) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (index < extents.size())
        return extents[index].getRangeLowerBound(dimension);
    else
        throw RTI::ArrayIndexOutOfBounds("Extent index above limit");
}

RTI::ULong RegionImp::getRangeUpperBound(RTI::ExtentIndex index, RTI::DimensionHandle dimension) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (index < extents.size())
        return extents[index].getRangeUpperBound(dimension);
    else
        throw RTI::ArrayIndexOutOfBounds("Extent index above limit");
}

void RegionImp::setRangeLowerBound(RTI::ExtentIndex index, RTI::DimensionHandle dimension, RTI::ULong val)
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (index < extents.size())
        extents[index].setRangeLowerBound(dimension, val);
    else
        throw RTI::ArrayIndexOutOfBounds("Extent index above limit");
}

void RegionImp::setRangeUpperBound(RTI::ExtentIndex index, RTI::DimensionHandle dimension, RTI::ULong val)
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (index < extents.size())
        extents[index].setRangeUpperBound(dimension, val);
    else
        throw RTI::ArrayIndexOutOfBounds("Extent index above limit");
}

RTI::SpaceHandle RegionImp::getSpaceHandle() const
    throw ()
{
    return space;
}

RTI::ULong RegionImp::getNumberOfExtents() const
    throw ()
{
    return extents.size();
}

RTI::ULong RegionImp::getRangeLowerBoundNotificationLimit(RTI::ExtentIndex index, RTI::DimensionHandle dimension) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (index < effectiveExtents.size())
        return effectiveExtents[index].getRangeLowerBound(dimension);
    else
        throw RTI::ArrayIndexOutOfBounds("Extent index above limit");
}

RTI::ULong RegionImp::getRangeUpperBoundNotificationLimit(RTI::ExtentIndex index, RTI::DimensionHandle dimension) const
    throw (RTI::ArrayIndexOutOfBounds)
{
    if (index < effectiveExtents.size())
        return effectiveExtents[index].getRangeUpperBound(dimension);
    else
        throw RTI::ArrayIndexOutOfBounds("Extent index above limit");
}

RegionHandle RegionImp::getHandle() const
{
    return handle;
}

/** Get the region's extents 
    \return The extents, as a vector 
 */
const std::vector<Extent> &
RegionImp::getExtents() const
{
    return extents;
}

/** Register a successful notifyAboutRegionModification()
 */
void RegionImp::commit()
{
    if (extents.size() != effectiveExtents.size())
        throw RTI::InvalidExtents("Different number of extents");

    effectiveExtents = extents;
}

// $Id: RTItypesImp.cc,v 1.1 2014/03/03 15:18:24 erk Exp $
