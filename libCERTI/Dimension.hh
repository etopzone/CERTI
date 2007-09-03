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
// $Id: Dimension.hh,v 3.8 2007/09/03 13:59:48 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_DIMENSION_HH
#define _CERTI_DIMENSION_HH

#include "certi.hh"
#include "Named.hh"
#include "Handled.hh"

namespace certi {

/** 
 * Describes a dimension in a routing space.
 * @sa RoutingSpace
 */
class Dimension : public Named, public Handled<DimensionHandle>
{
public:
	/**
	 * Create a dimension object from handle.
	 * Dimension are specified as subelement of the routing
	 * space section of a FED file. A dimension
	 * is a single axis segment with fixed lower and upper bound.
	 * All dimensions of a Federation should share the same 
	 * lower and upper bound. 
	 * @param[in] dimensionHandle the dimension object handle
	 */
    Dimension(DimensionHandle dimensionHandle);
    
    /**
     * Set dimension lower bound.
     * @param[in] lowerBound the lower bound 
     */
    static void setLowerBound(ULong lowerBound);
    
    /**
     * Set dimension upper bound.
     * @param[in] upperBound the upper bound 
     */
    static void setUpperBound(ULong upperBound);
    
    /**
     * Get dimension lower bound.
     * @return the lower bound 
     */
    static ULong getLowerBound() { return axisLowerBound ; }
    
    /**
     * Get dimension upper bound.
     * @return the upper bound 
     */
    static ULong getUpperBound() { return axisUpperBound ; }

private:
    static ULong axisLowerBound ;
    static ULong axisUpperBound ;
};

} // namespace certi

#endif

// $Id: Dimension.hh,v 3.8 2007/09/03 13:59:48 erk Exp $
