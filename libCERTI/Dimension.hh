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
// $Id: Dimension.hh,v 3.6 2005/03/25 17:01:57 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_DIMENSION_HH
#define _CERTI_DIMENSION_HH

#include "certi.hh"
#include "Named.hh"
#include "Handled.hh"

#include <string>

namespace certi {

/** Describes a dimension in a routing space.
    \sa RoutingSpace
 */
class Dimension : public Named, public Handled<DimensionHandle>
{
public:
    Dimension(DimensionHandle);
    static void setLowerBound(ULong);
    static void setUpperBound(ULong);
    static ULong getLowerBound() { return axisLowerBound ; }
    static ULong getUpperBound() { return axisUpperBound ; }

private:
    static ULong axisLowerBound ;
    static ULong axisUpperBound ;
};

} // namespace certi

#endif

// $Id: Dimension.hh,v 3.6 2005/03/25 17:01:57 breholee Exp $
