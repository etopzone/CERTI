// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003-2005  ONERA
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
// $Id: Dimension.cc,v 3.4 2005/03/28 19:14:51 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Dimension.hh"

using std::string ;

namespace certi {

ULong Dimension::axisLowerBound = 0 ;
ULong Dimension::axisUpperBound = LONG_MAX ;

Dimension::Dimension(DimensionHandle h)
{
    handle = h ;
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

} // namespace certi

// $Id: Dimension.cc,v 3.4 2005/03/28 19:14:51 breholee Exp $


