// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
//
// This file is part of CERTI-libRTI
//
// CERTI-libRTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libRTI is distributed in the hope that it will be useful, but
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

#ifndef CERTI_BASE_REGION_HH
#define CERTI_BASE_REGION_HH

#include "Extent.hh"
#include "Handled.hh"

#include <vector>

namespace certi {

typedef Handle RegionHandle;

class CERTI_EXPORT BaseRegion : public Handled<RegionHandle> {
public:
    BaseRegion(RegionHandle);
    virtual ~BaseRegion();

    virtual uint32_t getRangeLowerBound(ExtentIndex, DimensionHandle) const;

    virtual uint32_t getRangeUpperBound(ExtentIndex, DimensionHandle) const;

    virtual void setRangeLowerBound(ExtentIndex, DimensionHandle, uint32_t);

    virtual void setRangeUpperBound(ExtentIndex, DimensionHandle, uint32_t);

    virtual uint32_t getNumberOfExtents() const noexcept;

    virtual SpaceHandle getSpaceHandle() const noexcept = 0;

    const std::vector<Extent>& getExtents() const;
    void replaceExtents(const std::vector<Extent>&);
    bool overlaps(const BaseRegion& region) const;

protected:
    void setExtents(const std::vector<Extent>&);

    std::vector<Extent> extents;
};

} // namespace certi

#endif // CERTI_BASE_REGION_HH
