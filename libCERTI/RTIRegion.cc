// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2005  ONERA
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
// $Id: RTIRegion.cc,v 3.1 2005/03/28 19:22:19 breholee Exp $
// ----------------------------------------------------------------------------

#include "RTIRegion.hh"
#include "RoutingSpace.hh"
#include <config.h>

namespace certi {

// ----------------------------------------------------------------------------
/** Region Constructor
    @param region_handle Handle of this Region
    @param space_handle Handle of the RoutingSpace of this Region
    @param nb_extents Number of extents
    @param nb_dimensions Number of dimensions
 */
RTIRegion::RTIRegion(RegionHandle region_handle,
		     const RoutingSpace &routing_space,
		     size_t nb_extents)
    : BaseRegion(region_handle), space(routing_space)
{
    setExtents(std::vector<Extent>(nb_extents, space.createExtent()));
}

// ----------------------------------------------------------------------------
RTIRegion::~RTIRegion()
{
}

// ----------------------------------------------------------------------------
/** Get the handle of the routing space having this region.
 */
SpaceHandle
RTIRegion::getSpaceHandle() const
    throw ()
{
    return space.getHandle();
}

} // namespace certi

// $Id: RTIRegion.cc,v 3.1 2005/03/28 19:22:19 breholee Exp $
