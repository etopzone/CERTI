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
// $Id: RegionImp.hh,v 3.6 2003/11/13 10:43:02 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_REGION_IMP_HH
#define _CERTI_REGION_IMP_HH

#include "Extent.hh"
#include "RTItypes.hh"

#include <vector>

namespace certi {

typedef unsigned long RegionHandle ;

/** Implementation for the HLA Region interface. A region is a set of extents
    in a routing space. It contains two sets of such extents : current values
    (not commited to the RTI yet), and the last commited values. This 
    implementation also keeps track of the routing space handle, and 
    introduces a handle for the region.
    \sa Region, RoutingSpace, Dimension, Extent
 */
class RegionImp : public Region
{
public:
    RegionImp(RegionHandle, SpaceHandle, size_t, size_t);
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

    RegionHandle getHandle() const ;
    void setHandle(RegionHandle);
    long getNumberOfExtents();
    void notify();

    //const Extent &getExtent(ExtentIndex) const throw (ArrayIndexOutOfBounds);
    const std::vector<Extent> &getExtents() const ;
    void setExtents(const std::vector<Extent> &) throw (InvalidExtents);
    void add(const Extent &);

private:
    SpaceHandle space ;
    RegionHandle handle ;
    std::vector<Extent> extents ;
    std::vector<Extent> coExtents ;
};

} // namespace certi

#endif // _CERTI_REGION_IMP_HH

// $Id: RegionImp.hh,v 3.6 2003/11/13 10:43:02 breholee Exp $
