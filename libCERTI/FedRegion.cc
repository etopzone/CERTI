// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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
// ----------------------------------------------------------------------------

#include "FedRegion.hh"

using std::vector;

namespace certi {

// ----------------------------------------------------------------------------
FedRegion::FedRegion(RegionHandle h, SpaceHandle s, const std::vector<Extent>& ext)
    : BaseRegion(h), space(s), coExtents(ext)
{
    setExtents(ext);
}

// ----------------------------------------------------------------------------
FedRegion::~FedRegion()
{
}

// ----------------------------------------------------------------------------
/** Get the handle of the routing space having this region.
 */
SpaceHandle FedRegion::getSpaceHandle() const noexcept
{
    return space;
}

// ----------------------------------------------------------------------------
uint32_t FedRegion::getRangeLowerBound(ExtentIndex index, DimensionHandle dimension) const
{
    return BaseRegion::getRangeLowerBound(index, dimension);
}

// ----------------------------------------------------------------------------
uint32_t FedRegion::getRangeUpperBound(ExtentIndex index, DimensionHandle dimension) const
{
    return BaseRegion::getRangeUpperBound(index, dimension);
}

// ----------------------------------------------------------------------------
void FedRegion::setRangeLowerBound(ExtentIndex index, DimensionHandle dimension, uint32_t val)
{
    BaseRegion::setRangeLowerBound(index, dimension, val);
}

// ----------------------------------------------------------------------------
void FedRegion::setRangeUpperBound(ExtentIndex index, DimensionHandle dimension, uint32_t val)
{
    BaseRegion::setRangeUpperBound(index, dimension, val);
}

// ----------------------------------------------------------------------------
// getRangeLowerBoundNotificationLimit
//
uint32_t FedRegion::getRangeLowerBoundNotificationLimit(ExtentIndex index, DimensionHandle dimension) const
{
    if (index >= coExtents.size()) {
        throw ArrayIndexOutOfBounds("");
    }
    else {
        return coExtents[index].getRangeLowerBound(dimension);
    }
}

// ----------------------------------------------------------------------------
// getRangeUpperBoundNotificationLimit
//
uint32_t FedRegion::getRangeUpperBoundNotificationLimit(ExtentIndex index, DimensionHandle dimension) const
{
    if (index >= coExtents.size()) {
        throw ArrayIndexOutOfBounds("");
    }
    else {
        return coExtents[index].getRangeUpperBound(dimension);
    }
}

// ----------------------------------------------------------------------------
uint32_t FedRegion::getNumberOfExtents() const noexcept
{
    return BaseRegion::getNumberOfExtents();
}

// ----------------------------------------------------------------------------
/** Update the Region after an update to the RTI. 
 */
void FedRegion::commit()
{
    replaceExtents(coExtents);
}

} // namespace certi
