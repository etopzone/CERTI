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
// $Id: RoutingSpace.cc,v 3.3 2003/07/07 23:05:26 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RoutingSpace.hh"

#include <iostream>

using std::vector ;
using std::cout ;
using std::endl ;
using std::string ;

namespace certi {

// ----------------------------------------------------------------------------
/** Constructor
 */
RoutingSpace::RoutingSpace()
{
}

// ----------------------------------------------------------------------------
/** Destructor
 */
RoutingSpace::~RoutingSpace()
{
    vector<Dimension*>::iterator i ;
    for (i = dimensions.begin(); i != dimensions.end(); i++) {
        delete *i ;
    }
    dimensions.clear();
}

// ----------------------------------------------------------------------------
/** add a dimension
 */
void
RoutingSpace::addDimension(Dimension* d)
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
RoutingSpace::display()
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
RoutingSpace::getName()
{
    return name ;
}

// ----------------------------------------------------------------------------
/** get the routing space's handle
 */
SpaceHandle
RoutingSpace::getHandle()
{
    return handle ;
}

// ----------------------------------------------------------------------------
/** get the handle of the specified dimension
 */
DimensionHandle
RoutingSpace::getDimensionHandle(string dimension)
    throw (NameNotFound)
{
    vector<Dimension *>::iterator i ;

    for (i = dimensions.begin(); i != dimensions.end(); i++) {
        if ((*i)->getName() == dimension)
            return (*i)->getHandle();
    }
    throw new NameNotFound();
}

// ----------------------------------------------------------------------------
/** get the name of the specified dimension
 */
string
RoutingSpace::getDimensionName(DimensionHandle dimension)
    throw (DimensionNotDefined)
{
    vector<Dimension *>::iterator i ;

    for (i= dimensions.begin(); i != dimensions.end(); i++) {
        if ((*i)->getHandle() == dimension) {
            return (*i)->getName();
        }
    }
    throw new DimensionNotDefined();
}

// ----------------------------------------------------------------------------
/** get the number of dimensions of this space
 */
long
RoutingSpace::getNbDimensions()
{
    return dimensions.size();
}

} // namespace certi

// $Id: RoutingSpace.cc,v 3.3 2003/07/07 23:05:26 breholee Exp $
