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
// $Id: RegionImp.cc,v 3.4 2003/07/03 15:59:21 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RegionImp.hh"

using std::vector ;

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

    for (int i = 0 ; i < nb_extents ; i++) {
        extents.push_back(new Extent(dimensions));
        coExtents.push_back(new Extent(dimensions));
    }
}

// ----------------------------------------------------------------------------
// ~RegionImp
//
RegionImp::~RegionImp()
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
RegionImp::getSpaceHandle() const
    throw ()
{
    return space ;
}

// ----------------------------------------------------------------------------
// getNumberOfExtents
//
ULong
RegionImp::getNumberOfExtents() const
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
RegionHandle
RegionImp::getHandle() const
{
    return handle ;
}

// ----------------------------------------------------------------------------
// setHandle
//
void
RegionImp::setHandle(RegionHandle h)
{
    handle = h ;
}

// ----------------------------------------------------------------------------
// notify
//
void
RegionImp::notify()
{
    vector<Extent *>::iterator e, c ;
    vector<Extent *>::iterator end = extents.end();

    for (e = extents.begin(), c = coExtents.begin(); e != end ; ++e) {
	int n = (*e)->getNumberOfRanges();
	for (int i = 0 ; i < n ; ++i) {
	    (*c)->setRangeUpperBound(i, (*e)->getRangeUpperBound(i));
	    (*c)->setRangeLowerBound(i, (*e)->getRangeLowerBound(i));
	}
    }
}

// ----------------------------------------------------------------------------
// getNumberOfExtents
//
long
RegionImp::getNumberOfExtents()
{
    return extents.size();
}

// ----------------------------------------------------------------------------
// getExtent
//
Extent *
RegionImp::getExtent(ExtentIndex i) const
    throw (ArrayIndexOutOfBounds)
{
    if (i < 0 || i >= extents.size()) {
	throw ArrayIndexOutOfBounds();
    }
    return extents[i] ;
}

// ----------------------------------------------------------------------------
// getExtents
//
vector<Extent *> *
RegionImp::getExtents()
{
    return &extents ;
}

// ----------------------------------------------------------------------------
// setExtents
//
void
RegionImp::setExtents(const vector<Extent *> &e)
    throw (InvalidExtents)
{
    if (e.size() != extents.size())
	throw InvalidExtents();

    int n = extents.size();
    for (int i = 0 ; i < n ; ++i) {
	int m = e[i]->getNumberOfRanges();
	for (int j = 0 ; j < m ; ++j) {
	    extents[i]->setRangeUpperBound(j, e[i]->getRangeUpperBound(j));
	    extents[i]->setRangeLowerBound(j, e[i]->getRangeLowerBound(j));
	}
    }
}

} // namespace certi

// $Id: RegionImp.cc,v 3.4 2003/07/03 15:59:21 breholee Exp $
