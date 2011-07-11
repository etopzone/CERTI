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
// $Id: RoutingSpace.hh,v 3.13 2011/07/11 11:17:26 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_ROUTING_SPACE
#define _CERTI_ROUTING_SPACE


// CERTI headers
#include "Dimension.hh"
#include "Handled.hh"
#include "Extent.hh"
#include "Named.hh"

// Standard headers
#include <vector>
#include <string>

namespace certi {

class CERTI_EXPORT RoutingSpace : public Handled<SpaceHandle>, public Named
{
public:
	/** 
	 * Constructor
	 */
    RoutingSpace();
    
    /** 
     * Destructor
     */
    virtual ~RoutingSpace();
    
    /** 
     * Get the handle of the specified dimension
     * @param dimension_name Dimension name
     * @return Dimension handle
     */
    DimensionHandle getDimensionHandle(const std::string& dimension_name) const throw (NameNotFound);
    
    /** 
     * Get the name of the specified dimension
     * @param dimension_handle Dimension handle
     * @return the Dimension name
     */
    const std::string& getDimensionName(DimensionHandle dimension_handle) const
	throw (DimensionNotDefined);
    
    /** 
     * Get the number of dimensions of this space
     * @return Number of dimensions
     */
    size_t size() const ;

    /** 
     * Display (stdout) the routing space details
     */
    void display() const ;
    
    /** 
     * Add a dimension. A copy of the dimension parameter is added in the
     * routing space.
     * @param[in] d Dimension to add
     */
    void addDimension(const Dimension &d);
    
    Extent createExtent() const ;

    const std::vector<Dimension>& getDimensions() const { return dimensions; }

private:
    std::vector<Dimension> dimensions ;
};

} // namespace certi

#endif // _CERTI_ROUTING_SPACE

// $Id: RoutingSpace.hh,v 3.13 2011/07/11 11:17:26 erk Exp $

