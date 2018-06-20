// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// ----------------------------------------------------------------------------

#ifndef _CERTI_DATA_DISTRIBUTION
#define _CERTI_DATA_DISTRIBUTION

#include <libCERTI/RootObject.hh>

#include "Communications.hh"
#include "FederationManagement.hh"

namespace certi {
namespace rtia {

class DataDistribution {
public:
    DataDistribution(RootObject*, FederationManagement*, Communications*);

    SpaceHandle getRoutingSpaceHandle(const std::string&) const;

    const std::string& getRoutingSpaceName(SpaceHandle) const;

    DimensionHandle getDimensionHandle(const std::string&, SpaceHandle) const throw(SpaceNotDefined, NameNotFound);

    const std::string& getDimensionName(DimensionHandle, SpaceHandle) const throw(SpaceNotDefined, DimensionNotDefined);

    SpaceHandle getAttributeSpace(AttributeHandle, ObjectClassHandle) const
        throw(ObjectClassNotDefined, AttributeNotDefined);

    SpaceHandle getInteractionSpace(InteractionClassHandle) const throw(InteractionClassNotDefined);

    long createRegion(SpaceHandle, unsigned long, Exception::Type&) throw(SpaceNotDefined);

    void modifyRegion(RegionHandle, const std::vector<Extent>&, Exception::Type&);

    void deleteRegion(long, Exception::Type&) throw(RegionNotKnown, RegionInUse);

    void associateRegion(ObjectHandle,
                         RegionHandle,
                         const std::vector<AttributeHandle>&,
                         uint32_t,
                         Exception::Type&) throw(RegionNotKnown);

    ObjectHandle registerObject(ObjectClassHandle,
                                const std::string&,
                                const std::vector<AttributeHandle>&,
                                uint32_t,
                                const std::vector<RegionHandle>,
                                Exception::Type&);

    void unassociateRegion(ObjectHandle, RegionHandle, Exception::Type& e) throw(ObjectNotKnown,
                                                                                 InvalidRegionContext,
                                                                                 RegionNotKnown);

    void subscribe(ObjectClassHandle,
                   RegionHandle,
                   const std::vector<AttributeHandle>&,
                   uint32_t,
                   Exception::Type&) throw(RegionNotKnown);

    void unsubscribeAttributes(ObjectClassHandle, RegionHandle, Exception::Type&) throw(RegionNotKnown);

    void subscribeInteraction(InteractionClassHandle, RegionHandle, Exception::Type&) throw(RegionNotKnown);

    void unsubscribeInteraction(InteractionClassHandle, RegionHandle, Exception::Type&) throw(RegionNotKnown);

private:
    RootObject* rootObject;
    FederationManagement* fm;
    Communications* comm;
};
}
} // namespace certi/rtia

#endif // _CERTI_DATA_DISTRIBUTION
