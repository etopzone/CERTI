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
// $Id: RoutingSpace.hh,v 3.7 2007/08/31 12:47:40 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_ROUTING_SPACE
#define _CERTI_ROUTING_SPACE

#include "Dimension.hh"
#include "Handled.hh"
#include "Extent.hh"
#include "Named.hh"

#include <vector>
#include <string>

namespace certi {

class CERTI_EXPORT RoutingSpace : public Handled<SpaceHandle>, public Named
{
public:
    RoutingSpace();
    ~RoutingSpace();
    
    DimensionHandle getDimensionHandle(std::string) const throw (NameNotFound);
    std::string getDimensionName(DimensionHandle) const
	throw (DimensionNotDefined);
    size_t size() const ;

    void display() const ;
    void addDimension(const Dimension &);

    Extent createExtent() const ;

private:
    std::vector<Dimension> dimensions ;
};

} // namespace certi

#endif // _CERTI_ROUTING_SPACE

// $Id: RoutingSpace.hh,v 3.7 2007/08/31 12:47:40 erk Exp $

