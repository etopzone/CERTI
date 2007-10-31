// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2005  ONERA
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
// $Id: RTIRegion.hh,v 3.2 2007/10/31 10:30:20 erk Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_RTI_REGION_HH
#define CERTI_RTI_REGION_HH

#include "BaseRegion.hh"

namespace certi {

class RoutingSpace;

class CERTI_EXPORT RTIRegion : public BaseRegion
{
public:
    RTIRegion(RegionHandle, const RoutingSpace &, size_t);
    virtual ~RTIRegion();

    virtual SpaceHandle getSpaceHandle() const
        throw ();

protected:
    const RoutingSpace &space ;
};

} // namespace certi

#endif // CERTI_RTI_REGION_HH

// $Id: RTIRegion.hh,v 3.2 2007/10/31 10:30:20 erk Exp $
