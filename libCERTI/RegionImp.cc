// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
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
// $Id: RegionImp.cc,v 3.1 2003/04/09 16:41:10 breholee Exp $
// ----------------------------------------------------------------------------

#include "RegionImp.hh"

namespace certi {

// ----------------------------------------------------------------------------
// RegionImp
//
RegionImp::RegionImp(long h, SpaceHandle s, long dimensions, long nb_extents)
{
    handle = h ;
    space = s ;

    extents.reserve(nb_extents);
    coExtents.reserve(nb_extents);

    for (int i = 0; i < nb_extents; i++) {
        extents.push_back(new Extent(dimensions));
        coExtents.push_back(new Extent(dimensions));
    }
}

// ----------------------------------------------------------------------------
// ~RegionImp
//
RegionImp::~RegionImp(void)
{
    vector<Extent*>::iterator i ;
    for (i = extents.begin(); i != extents.end(); i++) {
        delete *i ;
    }
    for (i = coExtents.begin(); i != coExtents.end(); i++) {
        delete *i ;
    }
    extents.clear();
    coExtents.clear();
}

// ----------------------------------------------------------------------------
// getSpaceHandle
//
SpaceHandle
RegionImp::getSpaceHandle(void) const
    throw ()
{
    return space ;
}

// ----------------------------------------------------------------------------
// getNumberOfExtents
//
ULong
RegionImp::getNumberOfExtents(void) const
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
    if ((index >= extents.size()) || (index < 0)) {
        throw ArrayIndexOutOfBounds();
    }
    else {
        return extents[index]->getRangeLowerBound(dimension);
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
    if ((index >= extents.size()) || (index < 0)) {
        throw ArrayIndexOutOfBounds();
    }
    else {
        return extents[index]->getRangeUpperBound(dimension);
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
    if ((index >= coExtents.size()) || (index < 0)) {
        throw ArrayIndexOutOfBounds();
    }
    else {
        return coExtents[index]->getRangeLowerBound(dimension);
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
    if ((index >= coExtents.size()) || (index < 0)) {
        throw ArrayIndexOutOfBounds();
    }
    else {
        return coExtents[index]->getRangeUpperBound(dimension);
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
    if ((index >= extents.size()) || (index < 0)) {
        throw ArrayIndexOutOfBounds();
    }
    else {
        extents[index]->setRangeLowerBound(dimension, val);
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
    if ((index >= extents.size()) || (index < 0)) {
        throw ArrayIndexOutOfBounds();
    }
    else {
        extents[index]->setRangeUpperBound(dimension, val);
    }
}

// ----------------------------------------------------------------------------
// getHandle
//
long
RegionImp::getHandle(void)
{
    return handle ;
}

// ----------------------------------------------------------------------------
// setHandle
//
void
RegionImp::setHandle(long h)
{
    handle = h ;
}

} // namespace certi

// $Id: RegionImp.cc,v 3.1 2003/04/09 16:41:10 breholee Exp $
