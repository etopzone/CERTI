// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: DataDistribution.hh,v 3.12 2003/10/20 13:15:14 breholee Exp $
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

    SpaceHandle getRoutingSpaceHandle(std::string);

    std::string getRoutingSpaceName(SpaceHandle);

    DimensionHandle getDimensionHandle(std::string, SpaceHandle)
        throw (SpaceNotDefined, NameNotFound);

    std::string getDimensionName(DimensionHandle, SpaceHandle)
        throw (SpaceNotDefined, DimensionNotDefined);

    SpaceHandle getAttributeSpace(AttributeHandle, ObjectClassHandle)
        throw (ObjectClassNotDefined, AttributeNotDefined);

    SpaceHandle getInteractionSpace(InteractionClassHandle)
        throw (InteractionClassNotDefined);

    long createRegion(SpaceHandle, long, TypeException &)
        throw (SpaceNotDefined);

    void modifyRegion(RegionHandle, std::vector<Extent *> *, TypeException &);
    
    void deleteRegion(long, TypeException &)
        throw (RegionNotKnown, RegionInUse);

    void associateRegion(ObjectHandle, RegionHandle, AttributeHandle *,
			 int, TypeException &)
	throw (RegionNotKnown);

    void unassociateRegion(ObjectHandle, RegionHandle, TypeException &e)
	throw (ObjectNotKnown, InvalidRegionContext, RegionNotKnown);

    void subscribe(ObjectClassHandle, RegionHandle, AttributeHandle *, int,
		   TypeException &)
	throw (RegionNotKnown);

    void unsubscribeAttributes(ObjectClassHandle, RegionHandle, TypeException &)
	throw (RegionNotKnown);

    void subscribe(InteractionClassHandle, RegionHandle, TypeException &)
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

// $Id: DataDistribution.hh,v 3.12 2003/10/20 13:15:14 breholee Exp $
