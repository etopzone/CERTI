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
// $Id: Extent.cc,v 3.4 2003/11/10 14:36:43 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Extent.hh"

using std::vector ;

namespace certi {

// ----------------------------------------------------------------------------
/** Extent constructor
    \param n Number of dimensions in the routing space
 */
Extent::Extent(size_t n)
{
    ranges.resize(n);
}

// ----------------------------------------------------------------------------
/** Get range lower bound
 */
ULong
Extent::getRangeLowerBound(DimensionHandle handle) const
    throw (ArrayIndexOutOfBounds)
{
    if ((handle <= 0) || (handle > ranges.size())) 
	throw ArrayIndexOutOfBounds();
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
	throw ArrayIndexOutOfBounds();
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
	throw ArrayIndexOutOfBounds();
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
	throw ArrayIndexOutOfBounds();
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
/** Get a range dimension handle.
 */
// DimensionHandle
// Extent::getDimensionHandle(size_t n) const
//     throw (ArrayIndexOutOfBounds)
// {
//     RangeSet::const_iterator it = ranges.begin();
//     for (int j = 0 ; j < size(); ++j, ++it) {
// 	if (j == n) return it->first ;
//     }
//     throw ArrayIndexOutOfBounds();
// }

} // namespace certi

// $Id: Extent.cc,v 3.4 2003/11/10 14:36:43 breholee Exp $
