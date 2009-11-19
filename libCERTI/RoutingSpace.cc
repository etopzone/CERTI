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
// $Id: RoutingSpace.cc,v 3.16 2009/11/19 18:15:30 erk Exp $
// ----------------------------------------------------------------------------


#include "RoutingSpace.hh"
#include "helper.hh"

#include <iostream>
#include <cassert>
#include <algorithm>

using std::vector ;
using std::cout ;
using std::endl ;

namespace certi {

// ----------------------------------------------------------------------------

RoutingSpace::RoutingSpace() { }

// ----------------------------------------------------------------------------

RoutingSpace::~RoutingSpace() { }

// ----------------------------------------------------------------------------
void
RoutingSpace::addDimension(const Dimension &d)
{
    dimensions.push_back(d);
    assert(dimensions.back().getHandle() == dimensions.size());
}

// ----------------------------------------------------------------------------
void
RoutingSpace::display() const
{
    cout << "RoutingSpace \"" << getName() << "\"" << endl ;
}

// ----------------------------------------------------------------------------
DimensionHandle
RoutingSpace::getDimensionHandle(const std::string& dimension_name) const
    throw (NameNotFound)
{
    vector<Dimension>::const_iterator it = std::find_if(
	dimensions.begin(),
	dimensions.end(),
	NameComparator<Dimension>(dimension_name));

    if (it == dimensions.end())
	throw NameNotFound("");
    else
	return it->getHandle();
}

// ----------------------------------------------------------------------------
const std::string&
RoutingSpace::getDimensionName(DimensionHandle dimension_handle) const
    throw (DimensionNotDefined)
{
    vector<Dimension>::const_iterator it = std::find_if(
	dimensions.begin(),
	dimensions.end(),
	HandleComparator<Dimension>(dimension_handle));

    if (it == dimensions.end())
	throw DimensionNotDefined("");
    else
	return it->getName();
}

// ----------------------------------------------------------------------------
size_t
RoutingSpace::size() const
{
    return dimensions.size();
}

// ----------------------------------------------------------------------------
Extent
RoutingSpace::createExtent() const
{
    Extent extent(size());

    vector<Dimension>::const_iterator it ;
    for (it = dimensions.begin(); it != dimensions.end(); ++it) {
	Handle h = it->getHandle();
	extent.setRangeLowerBound(h, it->getLowerBound());
	extent.setRangeUpperBound(h, it->getUpperBound());
    }

    return extent ;
}

} // namespace certi

// $Id: RoutingSpace.cc,v 3.16 2009/11/19 18:15:30 erk Exp $
