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
// $Id: RegionImp.hh,v 3.2 2003/06/27 17:26:29 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_REGION_IMP_HH
#define _CERTI_REGION_IMP_HH

#include "Extent.hh"
#include "RTItypes.hh"

#include <vector>

namespace certi {

typedef long RegionHandle ;

class RegionImp : public Region
{
public:
    RegionImp(long, SpaceHandle, long, long);
    virtual ~RegionImp();

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

    long getHandle();
    void setHandle(long);

private:
    SpaceHandle space ;
    long handle ;
    std::vector<Extent*> extents ;
    std::vector<Extent*> coExtents ;
};

} // namespace certi

#endif // _CERTI_REGION_IMP_HH

// $Id: RegionImp.hh,v 3.2 2003/06/27 17:26:29 breholee Exp $
