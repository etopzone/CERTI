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
// $Id: Extent.cc,v 3.3 2003/07/01 13:32:58 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Extent.hh"

using std::vector ;

namespace certi {

// ----------------------------------------------------------------------------
// Extent
//
Extent::Extent(int n)
{
    if (n > 0) {
        ranges.resize(n);
    }
}

// ----------------------------------------------------------------------------
// ~Extent
//
Extent::~Extent()
{
    vector<Range*>::iterator i ;
    for (i = ranges.begin(); i != ranges.end(); i++) {
        delete *i ;
    }
    ranges.clear();
}

// ----------------------------------------------------------------------------
// getRangeLowerBound
//
ULong
Extent::getRangeLowerBound(DimensionHandle handle)
    throw (ArrayIndexOutOfBounds)
{
    vector<Range*>::iterator i ;
    for (i = ranges.begin(); i != ranges.end(); i++) {
        if ((*i)->getDimensionHandle() == handle) {
            return (*i)->getLowerBound();
        }
    }
    throw ArrayIndexOutOfBounds();
}

// ----------------------------------------------------------------------------
// getRangeUpperBound
//
ULong
Extent::getRangeUpperBound(DimensionHandle handle)
    throw (ArrayIndexOutOfBounds)
{
    vector<Range*>::iterator i ;
    for (i = ranges.begin(); i != ranges.end(); i++) {
        if ((*i)->getDimensionHandle() == handle) {
            return (*i)->getUpperBound();
        }
    }
    throw ArrayIndexOutOfBounds();
}

// ----------------------------------------------------------------------------
// setRangeLowerBound
//
void
Extent::setRangeLowerBound(DimensionHandle handle, ULong val)
    throw (ArrayIndexOutOfBounds)
{
    vector<Range*>::iterator i ;
    for (i = ranges.begin(); i != ranges.end(); i++) {
        if ((*i)->getDimensionHandle() == handle) {
            (*i)->setLowerBound(val);
            return ;
        }
    }
    throw ArrayIndexOutOfBounds();
}

// ----------------------------------------------------------------------------
// setRangeUpperBound
//
void
Extent::setRangeUpperBound(DimensionHandle handle, ULong val)
    throw (ArrayIndexOutOfBounds)
{
    vector<Range*>::iterator i ;
    for (i = ranges.begin(); i != ranges.end(); i++) {
        if ((*i)->getDimensionHandle() == handle) {
            (*i)->setUpperBound(val);
            return ;
        }
    }
    throw ArrayIndexOutOfBounds();
}

// ----------------------------------------------------------------------------
// getNumberOfRanges
//
long
Extent::getNumberOfRanges()
{
    return ranges.size();
}

} // namespace certi

// $Id: Extent.cc,v 3.3 2003/07/01 13:32:58 breholee Exp $
