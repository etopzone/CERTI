// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
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
// $Id: RegionImp.hh,v 3.1 2003/04/09 16:41:10 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_REGION_IMP_HH
#define _CERTI_REGION_IMP_HH

#include "Extent.hh"
#include "RTItypes.hh"

#include <vector>
using std::vector ;

namespace certi {

typedef long RegionHandle ;

class RegionImp : public Region
{
public:
    RegionImp(long, SpaceHandle, long, long);
    virtual ~RegionImp(void);

    virtual ULong getRangeLowerBound(ExtentIndex, DimensionHandle) const
        throw (ArrayIndexOutOfBounds);

    virtual ULong getRangeUpperBound(ExtentIndex, DimensionHandle) const
        throw (ArrayIndexOutOfBounds);

    virtual void setRangeLowerBound(ExtentIndex, DimensionHandle, ULong)
        throw (ArrayIndexOutOfBounds);

    virtual void setRangeUpperBound(ExtentIndex, DimensionHandle, ULong)
        throw (ArrayIndexOutOfBounds);

    virtual SpaceHandle getSpaceHandle(void) const
        throw ();

    virtual ULong getNumberOfExtents(void) const
        throw ();

    virtual ULong getRangeLowerBoundNotificationLimit(ExtentIndex,
                                                      DimensionHandle) const
        throw (ArrayIndexOutOfBounds);

    virtual ULong getRangeUpperBoundNotificationLimit(ExtentIndex,
                                                      DimensionHandle) const
        throw (ArrayIndexOutOfBounds);

    long getHandle(void);
    void setHandle(long);

private:
    SpaceHandle space ;
    long handle ;
    vector<Extent*> extents ;
    vector<Extent*> coExtents ;

};

} // namespace certi

#endif // _CERTI_REGION_IMP_HH

// $Id: RegionImp.hh,v 3.1 2003/04/09 16:41:10 breholee Exp $
