// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003-2005  ONERA
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
// $Id: Extent.cc,v 3.8 2005/04/30 16:49:43 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Extent.hh"
#include "Dimension.hh"
#include "PrettyDebug.hh"

using std::vector ;

namespace {

PrettyDebug D("EXTENT", __FILE__);

}

namespace certi {

// ----------------------------------------------------------------------------
/** Extent constructor
    \param n Number of dimensions in the routing space
 */
Extent::Extent(size_t n)
{
    Range range(Dimension::getLowerBound(), Dimension::getUpperBound());
    ranges.resize(n, range);
}

// ----------------------------------------------------------------------------
/** Get range lower bound
 */
ULong
Extent::getRangeLowerBound(DimensionHandle handle) const
    throw (ArrayIndexOutOfBounds)
{
    if ((handle <= 0) || (handle > ranges.size())) 
	throw ArrayIndexOutOfBounds("Invalid dimension handle");
    else
	return ranges[handle - 1].first ;	
}

// ----------------------------------------------------------------------------
/** Get range upper bound
 */
ULong
Extent::getRangeUpperBound(DimensionHandle handle) const
    throw (ArrayIndexOutOfBounds)
{
    if ((handle <= 0) || (handle > ranges.size())) 
	throw ArrayIndexOutOfBounds("Invalid dimension handle");
    else
	return ranges[handle - 1].second ;
}

// ----------------------------------------------------------------------------
/** Set range lower bound
 */
void
Extent::setRangeLowerBound(DimensionHandle handle, ULong val)
    throw (ArrayIndexOutOfBounds)
{
    if ((handle <= 0) || (handle > ranges.size())) 
	throw ArrayIndexOutOfBounds("Invalid dimension handle");
    else
	ranges[handle - 1].first = val ;
}

// ----------------------------------------------------------------------------
/** Set range upper bound
 */
void
Extent::setRangeUpperBound(DimensionHandle handle, ULong val)
    throw (ArrayIndexOutOfBounds)
{
    if ((handle <= 0) || (handle > ranges.size())) 
	throw ArrayIndexOutOfBounds("Invalid dimension handle");
    else
	ranges[handle - 1].second = val ;
}

// ----------------------------------------------------------------------------
/** Get the number of ranges in this Extent.
 */
size_t
Extent::size() const
{
    return ranges.size();
}

// ----------------------------------------------------------------------------
/** Check whether both extents overlap
 */
bool
Extent::overlaps(const Extent &e) const
{
    for (unsigned int i = 1 ; i <= size(); ++i) {
	D[pdTrace] << e.getRangeLowerBound(i) << " < " << getRangeUpperBound(i)
		   << " or " << e.getRangeUpperBound(i) << " < "
		   << getRangeLowerBound(i) << " ?" << std::endl ;
	if (e.getRangeLowerBound(i) > getRangeUpperBound(i) ||
	    e.getRangeUpperBound(i) < getRangeLowerBound(i))
	    return false ;
    }
    return true ;
}

} // namespace certi

// $Id: Extent.cc,v 3.8 2005/04/30 16:49:43 breholee Exp $
