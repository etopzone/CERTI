// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: DataDistribution.cc,v 3.4 2003/03/12 10:09:49 breholee Exp $
// ----------------------------------------------------------------------------

#include "DataDistribution.hh"

namespace certi {
namespace rtia {

static pdCDebug D("RTIA_DDM", "(RTIA DDM) ");

DataDistribution::DataDistribution(RootObject *ro)
{
    rootObject = ro ;
}

SpaceHandle
DataDistribution::getRoutingSpaceHandle(string name)
{
    return rootObject->getRoutingSpaceHandle(name);
}

string
DataDistribution::getRoutingSpaceName(SpaceHandle handle)
{
    return rootObject->getRoutingSpaceName(handle);
}

DimensionHandle
DataDistribution::getDimensionHandle(string dimension, SpaceHandle space)
    throw (SpaceNotDefined, NameNotFound)
{
    return rootObject->getRoutingSpace(space)->getDimensionHandle(dimension);
}

string
DataDistribution::getDimensionName(DimensionHandle dimension, SpaceHandle space)
    throw (SpaceNotDefined, DimensionNotDefined)
{
    return rootObject->getRoutingSpace(space)->getDimensionName(dimension);
}

}} // namespace certi

// $Id: DataDistribution.cc,v 3.4 2003/03/12 10:09:49 breholee Exp $
