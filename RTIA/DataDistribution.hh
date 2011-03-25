// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: DataDistribution.hh,v 3.22 2011/03/25 20:53:54 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_DATA_DISTRIBUTION
#define _CERTI_DATA_DISTRIBUTION

// Project
#include "Communications.hh"
#include "FederationManagement.hh"
#include "RootObject.hh"

namespace certi {
namespace rtia {

class DataDistribution
{
public:
    DataDistribution(RootObject *, FederationManagement *, Communications *);

    SpaceHandle getRoutingSpaceHandle(const std::string&) const ;

    const std::string& getRoutingSpaceName(SpaceHandle) const ;

    DimensionHandle getDimensionHandle(const std::string&, SpaceHandle) const
        throw (SpaceNotDefined, NameNotFound);

    const std::string& getDimensionName(DimensionHandle, SpaceHandle) const
        throw (SpaceNotDefined, DimensionNotDefined);

    SpaceHandle getAttributeSpace(AttributeHandle, ObjectClassHandle) const
        throw (ObjectClassNotDefined, AttributeNotDefined);

    SpaceHandle getInteractionSpace(InteractionClassHandle) const
        throw (InteractionClassNotDefined);

    long createRegion(SpaceHandle, unsigned long, TypeException &)
        throw (SpaceNotDefined);

    void modifyRegion(RegionHandle, const std::vector<Extent> &,
		      TypeException &);
    
    void deleteRegion(long, TypeException &)
        throw (RegionNotKnown, RegionInUse);

    void associateRegion(ObjectHandle, RegionHandle, const std::vector <AttributeHandle> &,
			 uint32_t, TypeException &)
	throw (RegionNotKnown);

    ObjectHandle registerObject(ObjectClassHandle, const std::string&,
				const std::vector <AttributeHandle> &, uint32_t,
				const std::vector<RegionHandle>,
				TypeException &);

    void unassociateRegion(ObjectHandle, RegionHandle, TypeException &e)
	throw (ObjectNotKnown, InvalidRegionContext, RegionNotKnown);

    void subscribe(ObjectClassHandle, RegionHandle, const std::vector <AttributeHandle> &, uint32_t,
		   TypeException &)
	throw (RegionNotKnown);

    void unsubscribeAttributes(ObjectClassHandle, RegionHandle, TypeException &)
	throw (RegionNotKnown);

    void subscribeInteraction(InteractionClassHandle, RegionHandle,
			      TypeException &)
	throw (RegionNotKnown);

    void unsubscribeInteraction(InteractionClassHandle, RegionHandle,
				TypeException &)
	throw (RegionNotKnown);
    
private:
    RootObject *rootObject ;
    FederationManagement *fm ;
    Communications *comm ;
};

}} // namespace certi/rtia

#endif // _CERTI_DATA_DISTRIBUTION

// $Id: DataDistribution.hh,v 3.22 2011/03/25 20:53:54 erk Exp $
