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
// $Id: RoutingSpace.cc,v 3.4 2003/11/10 14:54:11 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RoutingSpace.hh"
#include "NameComparator.hh"
#include "HandleComparator.hh"

#include <iostream>

using std::vector ;
using std::cout ;
using std::endl ;
using std::string ;

namespace certi {

// ----------------------------------------------------------------------------
/** Constructor
 */
RoutingSpace::RoutingSpace() { }

// ----------------------------------------------------------------------------
/** Destructor
 */
RoutingSpace::~RoutingSpace() { }

// ----------------------------------------------------------------------------
/** Add a dimension. A copy of the dimension parameter is added in the
    routing space.
 */
void
RoutingSpace::addDimension(const Dimension &d)
{
    dimensions.push_back(d);
}

// ----------------------------------------------------------------------------
/** set the routing space's handle
 */
void
RoutingSpace::setHandle(SpaceHandle h)
{
    handle = h ;
}

// ----------------------------------------------------------------------------
/** display (stdout) the routing space details
 */
void
RoutingSpace::display() const
{
    cout << "RoutingSpace « " << name << " »" << endl ;
}

// ----------------------------------------------------------------------------
/** set the routing space's name
 */
void
RoutingSpace::setName(string s)
{
    name = s ;
}

// ----------------------------------------------------------------------------
/** get the routing space's name
 */
string
RoutingSpace::getName() const
{
    return name ;
}

// ----------------------------------------------------------------------------
/** get the routing space's handle
 */
SpaceHandle
RoutingSpace::getHandle() const
{
    return handle ;
}

// ----------------------------------------------------------------------------
/** get the handle of the specified dimension
 */
DimensionHandle
RoutingSpace::getDimensionHandle(string dimension_name) const
    throw (NameNotFound)
{
    vector<Dimension>::const_iterator it = std::find_if(
	dimensions.begin(),
	dimensions.end(),
	NameComparator<Dimension>(dimension_name));

    if (it == dimensions.end())	throw NameNotFound();
    else return it->getHandle();
}

// ----------------------------------------------------------------------------
/** get the name of the specified dimension
 */
string
RoutingSpace::getDimensionName(DimensionHandle dimension_handle) const
    throw (DimensionNotDefined)
{
    vector<Dimension>::const_iterator it = std::find_if(
	dimensions.begin(),
	dimensions.end(),
	HandleComparator<Dimension>(dimension_handle));

    if (it == dimensions.end()) throw DimensionNotDefined();
    else return it->getName();
}

// ----------------------------------------------------------------------------
/** get the number of dimensions of this space
 */
size_t
RoutingSpace::size() const
{
    return dimensions.size();
}

} // namespace certi

// $Id: RoutingSpace.cc,v 3.4 2003/11/10 14:54:11 breholee Exp $
