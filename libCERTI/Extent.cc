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
// ----------------------------------------------------------------------------


#include "Extent.hh"
#include "Dimension.hh"
#include "PrettyDebug.hh"
#include <iostream>

using std::vector ;

namespace {

PrettyDebug D("EXTENT", __FILE__);

}

namespace certi {

Extent::Extent(size_t n)
{
    Range range(Dimension::getLowerBound(), Dimension::getUpperBound());
    ranges.resize(n, range);
}

// ----------------------------------------------------------------------------

uint32_t
Extent::getRangeLowerBound(DimensionHandle handle) const
    throw (ArrayIndexOutOfBounds)
{
    if ((handle <= 0) || (handle > ranges.size())) 
	throw ArrayIndexOutOfBounds("Invalid dimension handle");
    else
	return ranges[handle - 1].first ;	
}

uint32_t
Extent::getRangeUpperBound(DimensionHandle handle) const
    throw (ArrayIndexOutOfBounds)
{
    if ((handle <= 0) || (handle > ranges.size())) 
	throw ArrayIndexOutOfBounds("Invalid dimension handle");
    else
	return ranges[handle - 1].second ;
}

void
Extent::setRangeLowerBound(DimensionHandle handle, uint32_t val)
    throw (ArrayIndexOutOfBounds)
{
    if ((handle <= 0) || (handle > ranges.size())) 
	throw ArrayIndexOutOfBounds("Invalid dimension handle");
    else
	ranges[handle - 1].first = val ;
}

void
Extent::setRangeUpperBound(DimensionHandle handle, uint32_t val)
    throw (ArrayIndexOutOfBounds)
{
    if ((handle <= 0) || (handle > ranges.size())) 
	throw ArrayIndexOutOfBounds("Invalid dimension handle");
    else
	ranges[handle - 1].second = val ;
}

size_t
Extent::size() const
{
    return ranges.size();
}

bool
Extent::overlaps(const Extent &e) const
{
    for (unsigned int i = 1 ; i <= size(); ++i) {
	Debug(D, pdTrace) << e.getRangeLowerBound(i) << " < " << getRangeUpperBound(i)
		   << " or " << e.getRangeUpperBound(i) << " < "
		   << getRangeLowerBound(i) << " ?" << std::endl ;
	if (e.getRangeLowerBound(i) > getRangeUpperBound(i) ||
	    e.getRangeUpperBound(i) < getRangeLowerBound(i))
	    return false ;
    }
    return true ;
}

} // namespace certi
