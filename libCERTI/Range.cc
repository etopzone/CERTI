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
// $Id: Range.cc,v 3.2 2003/06/27 17:26:29 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Range.hh"

namespace certi {

// ----------------------------------------------------------------------------
// Range
//
Range::Range(DimensionHandle handle)
{
    lowerBound = LONG_MAX ;
    upperBound = LONG_MIN ;
    dimension = handle ;
}

// ----------------------------------------------------------------------------
// Range
//
Range::Range(DimensionHandle handle, ULong lower, ULong upper)
{
    lowerBound = lower ;
    upperBound = upper ;
    dimension = handle ;
}

// ----------------------------------------------------------------------------
// setLowerBound
//
void
Range::setLowerBound(ULong lower)
{
    lowerBound = lower ;
}

// ----------------------------------------------------------------------------
// setUpperBound
//
void
Range::setUpperBound(ULong upper)
{
    upperBound = upper ;
}

// ----------------------------------------------------------------------------
// getLowerBound
//
ULong
Range::getLowerBound()
{
    return lowerBound ;
}

// ----------------------------------------------------------------------------
// getUpperBound
//
ULong
Range::getUpperBound()
{
    return upperBound ;
}

// ----------------------------------------------------------------------------
// getDimensionHandle
//
DimensionHandle
Range::getDimensionHandle()
{
    return dimension ;
}

} // namespace certi

// $Id: Range.cc,v 3.2 2003/06/27 17:26:29 breholee Exp $
