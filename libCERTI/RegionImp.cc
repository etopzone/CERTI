// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
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
// $Id: RegionImp.cc,v 3.7 2004/03/04 20:19:05 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RegionImp.hh"

#include <cassert>

using std::vector ;

namespace certi {

// ----------------------------------------------------------------------------
/** Region Constructor
    \param region_handle Handle of this Region
    \param space_handle Handle of the RoutingSpace of this Region
    \param nb_extents Number of extents
    \param nb_dimensions Number of dimensions
 */
RegionImp::RegionImp(RegionHandle region_handle,
		     SpaceHandle space_handle,
		     size_t nb_extents,
		     size_t nb_dimensions)
    : handle(region_handle),
      space(space_handle),
      extents(nb_extents, Extent(nb_dimensions)),
      coExtents(nb_extents, Extent(nb_dimensions)) { }

// ----------------------------------------------------------------------------
// ~RegionImp
//
RegionImp::~RegionImp() { }

// ----------------------------------------------------------------------------
// getSpaceHandle
//
SpaceHandle
RegionImp::getSpaceHandle() const
    throw ()
{
    return space ;
}

// ----------------------------------------------------------------------------
// getNumberOfExtents
//
ULong
RegionImp::getNumberOfExtents() const
    throw ()
{
    return extents.size();
}

// ----------------------------------------------------------------------------
// getRangeLowerBound
//
ULong
RegionImp::getRangeLowerBound(ExtentIndex index,
                              DimensionHandle dimension) const
    throw (ArrayIndexOutOfBounds)
{
    if (index >= extents.size()) {
        throw ArrayIndexOutOfBounds();
    }
    else {
        return extents[index].getRangeLowerBound(dimension);
    }
}

// ----------------------------------------------------------------------------
// getRangeUpperBound
//
ULong
RegionImp::getRangeUpperBound(ExtentIndex index,
                              DimensionHandle dimension) const
    throw (ArrayIndexOutOfBounds)
{
    if (index >= extents.size()) {
        throw ArrayIndexOutOfBounds();
    }
    else {
        return extents[index].getRangeUpperBound(dimension);
    }
}

// ----------------------------------------------------------------------------
// getRangeLowerBoundNotificationLimit
//
ULong
RegionImp::getRangeLowerBoundNotificationLimit(ExtentIndex index,
                                               DimensionHandle dimension) const
    throw (ArrayIndexOutOfBounds)
{
    if (index >= coExtents.size()) {
        throw ArrayIndexOutOfBounds();
    }
    else {
        return coExtents[index].getRangeLowerBound(dimension);
    }
}

// ----------------------------------------------------------------------------
// getRangeUpperBoundNotificationLimit
//
ULong
RegionImp::getRangeUpperBoundNotificationLimit(ExtentIndex index,
                                               DimensionHandle dimension) const
    throw (ArrayIndexOutOfBounds)
{
    if (index >= coExtents.size()) {
        throw ArrayIndexOutOfBounds();
    }
    else {
        return coExtents[index].getRangeUpperBound(dimension);
    }
}

// ----------------------------------------------------------------------------
// setRangeLowerBound
//
void
RegionImp::setRangeLowerBound(ExtentIndex index,
                              DimensionHandle dimension,
                              ULong val)
    throw (ArrayIndexOutOfBounds)
{
    if (index >= extents.size()) {
        throw ArrayIndexOutOfBounds();
    }
    else {
        extents[index].setRangeLowerBound(dimension, val);
    }
}

// ----------------------------------------------------------------------------
// setRangeUpperBound
//
void
RegionImp::setRangeUpperBound(ExtentIndex index,
                              DimensionHandle dimension,
                              ULong val)
    throw (ArrayIndexOutOfBounds)
{
    if (index >= extents.size()) {
        throw ArrayIndexOutOfBounds();
    }
    else {
        extents[index].setRangeUpperBound(dimension, val);
    }
}

// ----------------------------------------------------------------------------
// getHandle
//
RegionHandle
RegionImp::getHandle() const
{
    return handle ;
}

// ----------------------------------------------------------------------------
// setHandle
//
void
RegionImp::setHandle(RegionHandle h)
{
    handle = h ;
}

// ----------------------------------------------------------------------------
/** Update the Region after an update to the RTI. 
 */
void
RegionImp::notify()
{
    vector<Extent>::iterator e, c ;
    vector<Extent>::iterator end = extents.end();

    for (e = extents.begin(), c = coExtents.begin(); e != end ; ++e) {
	int n = e->size() ;
	for (int h = 1 ; h <= n ; ++h) {
	    c->setRangeUpperBound(h, e->getRangeUpperBound(h));
	    c->setRangeLowerBound(h, e->getRangeLowerBound(h));
	}
    }
}

// ----------------------------------------------------------------------------
/** Get the number of extents in this region
 */
long
RegionImp::getNumberOfExtents()
{
    return extents.size();
}

// ----------------------------------------------------------------------------
/** Get the region's extents
    \return The extents, as a vector
 */
const vector<Extent> &
RegionImp::getExtents() const
{
    return extents ;
}

// ----------------------------------------------------------------------------
// setExtents
//
void
RegionImp::setExtents(const vector<Extent> &e)
    throw (InvalidExtents)
{
    assert(e.size() == extents.size()); // TO REMOVE

    if (e.size() != extents.size())
	throw InvalidExtents();

    extents = e ;
}

} // namespace certi

// $Id: RegionImp.cc,v 3.7 2004/03/04 20:19:05 breholee Exp $
