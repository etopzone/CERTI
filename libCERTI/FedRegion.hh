// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: FedRegion.hh,v 3.2 2007/06/22 08:51:36 erk Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_FED_REGION_HH
#define CERTI_FED_REGION_HH

#include "certi.hh"
#include "BaseRegion.hh"

class RoutingSpace ;

namespace certi {

class CERTI_EXPORT FedRegion : public BaseRegion, public RTI::Region
{
public:
    FedRegion(RegionHandle, SpaceHandle, const std::vector<Extent> &);
    virtual ~FedRegion();

    virtual ULong getRangeLowerBound(ExtentIndex, DimensionHandle) const
        throw (ArrayIndexOutOfBounds);

    virtual ULong getRangeUpperBound(ExtentIndex, DimensionHandle) const
        throw (ArrayIndexOutOfBounds);

    virtual void setRangeLowerBound(ExtentIndex, DimensionHandle, ULong)
        throw (ArrayIndexOutOfBounds);

    virtual void setRangeUpperBound(ExtentIndex, DimensionHandle, ULong)
        throw (ArrayIndexOutOfBounds);

    virtual SpaceHandle getSpaceHandle() const
        throw ();

    virtual ULong getNumberOfExtents() const
        throw ();

    virtual ULong getRangeLowerBoundNotificationLimit(ExtentIndex,
                                                      DimensionHandle) const
        throw (ArrayIndexOutOfBounds);

    virtual ULong getRangeUpperBoundNotificationLimit(ExtentIndex,
                                                      DimensionHandle) const
        throw (ArrayIndexOutOfBounds);

    void commit();

private:
    SpaceHandle space ;
    std::vector<Extent> coExtents ;
};

}

#endif // CERTI_FED_REGION_HH

// $Id: FedRegion.hh,v 3.2 2007/06/22 08:51:36 erk Exp $
