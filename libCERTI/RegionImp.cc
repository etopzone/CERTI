// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libcerti is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libcerti is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: RegionImp.cc,v 1.1 2003/02/17 12:48:52 breholee Exp $
// ---------------------------------------------------------------------------

#include "RegionImp.hh"

namespace certi {

RegionImp::~RegionImp(void)
{
    vector<Extent*>::iterator i ;
    for (i = extents.begin(); i != extents.end(); i++) {
        delete *i ;
    }
    extents.clear();
}

SpaceHandle
RegionImp::getSpaceHandle(void) const
    throw ()
{
    return spaceHandle ;
}

ULong
RegionImp::getNumberOfExtents(void) const
    throw ()
{
    return extents.size();
}

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

} // namespace certi

// $Id: RegionImp.cc,v 1.1 2003/02/17 12:48:52 breholee Exp $
