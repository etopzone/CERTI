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
// $Id: Dimension.cc,v 1.3 2003/03/12 10:06:06 breholee Exp $
// ----------------------------------------------------------------------------

#include "Dimension.hh"

namespace certi {

ULong Dimension::axisLowerBound = LONG_MIN ;
ULong Dimension::axisUpperBound = LONG_MAX ;

Dimension::Dimension(DimensionHandle h)
{
    handle = h ;
}

// ----------------------------------------------------------------------------
void
Dimension::setName(string s)
{
    name = s ;
}

// ----------------------------------------------------------------------------
string
Dimension::getName(void)
{
    return name ;
}   

// ----------------------------------------------------------------------------
void
Dimension::setLowerBound(ULong v)
{
    Dimension::axisLowerBound = v ;
}

// ----------------------------------------------------------------------------
void
Dimension::setUpperBound(ULong v)
{
    Dimension::axisUpperBound = v ;
}

// ----------------------------------------------------------------------------
DimensionHandle
Dimension::getHandle(void)
{
    return handle ;
}

} // namespace certi

// $Id: Dimension.cc,v 1.3 2003/03/12 10:06:06 breholee Exp $


