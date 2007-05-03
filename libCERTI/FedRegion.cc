// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: FedRegion.cc,v 3.3 2007/05/03 15:46:31 rousse Exp $
// ----------------------------------------------------------------------------

#include "FedRegion.hh"
#include <config.h>

using std::vector ;

namespace certi {

// ----------------------------------------------------------------------------
FedRegion::FedRegion(RegionHandle h, SpaceHandle s, const std::vector<Extent> &ext)
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
SpaceHandle
FedRegion::getSpaceHandle() const
    throw ()
{
    return space ;
}

// ----------------------------------------------------------------------------
ULong
FedRegion::getRangeLowerBound(ExtentIndex index,
			       DimensionHandle dimension) const
    throw (ArrayIndexOutOfBounds)
{
    return BaseRegion::getRangeLowerBound(index, dimension);
}

// ----------------------------------------------------------------------------
ULong
FedRegion::getRangeUpperBound(ExtentIndex index,
			       DimensionHandle dimension) const
    throw (ArrayIndexOutOfBounds)
{
    return BaseRegion::getRangeUpperBound(index, dimension);
}

// ----------------------------------------------------------------------------
void
FedRegion::setRangeLowerBound(ExtentIndex index,
			       DimensionHandle dimension,
			       ULong val)
    throw (ArrayIndexOutOfBounds)
{
    BaseRegion::setRangeLowerBound(index, dimension, val);
}

// ----------------------------------------------------------------------------
void
FedRegion::setRangeUpperBound(ExtentIndex index,
			       DimensionHandle dimension,
			       ULong val)
    throw (ArrayIndexOutOfBounds)
{
    BaseRegion::setRangeUpperBound(index, dimension, val);
}

// ----------------------------------------------------------------------------
// getRangeLowerBoundNotificationLimit
//
ULong
FedRegion::getRangeLowerBoundNotificationLimit(ExtentIndex index,
                                               DimensionHandle dimension) const
    throw (ArrayIndexOutOfBounds)
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
ULong
FedRegion::getRangeUpperBoundNotificationLimit(ExtentIndex index,
                                               DimensionHandle dimension) const
    throw (ArrayIndexOutOfBounds)
{
    if (index >= coExtents.size()) {
        throw ArrayIndexOutOfBounds("");
    }
    else {
        return coExtents[index].getRangeUpperBound(dimension);
    }
}

// ----------------------------------------------------------------------------
ULong
FedRegion::getNumberOfExtents() const
    throw ()
{
    return BaseRegion::getNumberOfExtents();
}

// ----------------------------------------------------------------------------
/** Update the Region after an update to the RTI. 
 */
void
FedRegion::commit()
{
    replaceExtents(coExtents);
}

} // namespace certi

// $Id: FedRegion.cc,v 3.3 2007/05/03 15:46:31 rousse Exp $
