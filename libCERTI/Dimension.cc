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
// ----------------------------------------------------------------------------

#include "Dimension.hh"
#include <limits.h>

namespace certi {

uint32_t Dimension::axisLowerBound = 0;
uint32_t Dimension::axisUpperBound = UINT_MAX;

Dimension::Dimension(DimensionHandle dimensionHandle)
{
    /* initialize handle from superclass */
    this->handle = dimensionHandle;
}

// ----------------------------------------------------------------------------
void Dimension::setLowerBound(uint32_t lowerBound)
{
    Dimension::axisLowerBound = lowerBound;
}

// ----------------------------------------------------------------------------
void Dimension::setUpperBound(uint32_t upperBound)
{
    Dimension::axisUpperBound = upperBound;
}

} // namespace certi
