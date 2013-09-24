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
// ----------------------------------------------------------------------------

#ifndef CERTI_FED_REGION_HH
#define CERTI_FED_REGION_HH

#include "certi.hh"
#include "BaseRegion.hh"

namespace certi {

class RoutingSpace ;

class CERTI_EXPORT FedRegion : public BaseRegion
{
public:
    FedRegion(RegionHandle, SpaceHandle, const std::vector<Extent> &);
    virtual ~FedRegion();

    virtual uint32_t getRangeLowerBound(ExtentIndex, DimensionHandle) const
        throw (ArrayIndexOutOfBounds);

    virtual uint32_t getRangeUpperBound(ExtentIndex, DimensionHandle) const
        throw (ArrayIndexOutOfBounds);

    virtual void setRangeLowerBound(ExtentIndex, DimensionHandle, uint32_t)
        throw (ArrayIndexOutOfBounds);

    virtual void setRangeUpperBound(ExtentIndex, DimensionHandle, uint32_t)
        throw (ArrayIndexOutOfBounds);

    virtual SpaceHandle getSpaceHandle() const
        throw ();

    virtual uint32_t getNumberOfExtents() const
        throw ();

    virtual uint32_t getRangeLowerBoundNotificationLimit(ExtentIndex,
                                                      DimensionHandle) const
        throw (ArrayIndexOutOfBounds);

    virtual uint32_t getRangeUpperBoundNotificationLimit(ExtentIndex,
                                                      DimensionHandle) const
        throw (ArrayIndexOutOfBounds);

    void commit();

private:
    SpaceHandle space ;
    std::vector<Extent> coExtents ;
};

}

#endif // CERTI_FED_REGION_HH
