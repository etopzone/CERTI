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
// $Id: Range.cc,v 1.1 2003/02/17 12:48:52 breholee Exp $
// ---------------------------------------------------------------------------

#include "Range.hh"

namespace certi {

Range::Range(DimensionHandle handle)
{
    lowerBound = LONG_MAX ;
    upperBound = LONG_MIN ;
    dimension = handle ;
}

Range::Range(DimensionHandle handle, ULong lower, ULong upper)
{
    lowerBound = lower ;
    upperBound = upper ;
    dimension = handle ;
}

void
Range::setLowerBound(ULong lower)
{
    lowerBound = lower ;
}

void
Range::setUpperBound(ULong upper)
{
    upperBound = upper ;
}

ULong
Range::getLowerBound(void)
{
    return lowerBound ;
}

ULong
Range::getUpperBound(void)
{
    return upperBound ;
}

DimensionHandle
Range::getDimensionHandle(void)
{
    return dimension ;
}

} // namespace certi

// $Id: Range.cc,v 1.1 2003/02/17 12:48:52 breholee Exp $
