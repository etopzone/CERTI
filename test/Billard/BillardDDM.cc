// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2004  ONERA
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
// $Id: BillardDDM.cc,v 3.9 2004/08/24 18:25:05 breholee Exp $
// ----------------------------------------------------------------------------

#include "BillardDDM.hh"
#include "RegionImp.hh"

using std::string ;

static PrettyDebug D("BILLARD_DDM", __FILE__);

/** Constructor
 */
BillardDDM::BillardDDM(string federate_name)
    : Billard(federate_name), subRegion(0), pubRegion(0)
{
    std::cout << "BillardDDM" << std::endl ;
}

/** Destructor
 */
BillardDDM::~BillardDDM()
{
}

// ----------------------------------------------------------------------------
/** Declare stuff
 */
void
BillardDDM::declare()
{
    GeoID = rtiamb.getRoutingSpaceHandle("Geo");
    std::cout << "Geo space handle : " << GeoID << std::endl ;

    // Ugly hardcoded choice of regions
    regions.clear();
    for (int i = 0 ; i < 5 ; ++i) {
	regions.push_back(Area());
	Area &area = regions.back();
	area.x = 100 * i ;
	area.y = 0 ;
	area.size = 100 ;
	std::cout << "Region " << i + 1 << "... " ;
	area.region = rtiamb.createRegion(GeoID, 1);
	std::cout << "ok" << std::endl ;
    }

    int region = (int) local.x / 100 + 1 ;
    AttributeHandle attrs[] = { AttrXID, AttrYID, AttrColorID } ;
    std::cout << "Register ball with region..." << std::endl ;
    local.ID = rtiamb.registerObjectInstanceWithRegion(BouleClassID,
						       federateName.c_str(),
						       attrs,
						       &(regions[region - 1].region),
						       1);
    pubRegion = region ;
}

// ----------------------------------------------------------------------------
/** Check regions grid
 */
void
BillardDDM::checkRegions()
{
    int region = (int) local.x / 100 + 1 ;
    if (region != subRegion || region != pubRegion) {
	AttributeHandleSet *attr_set = AttributeHandleSetFactory::create(3);
	attr_set->add(AttrXID);
	attr_set->add(AttrYID);

	// Subscription
	if (subRegion != 0) {
	    rtiamb.unsubscribeObjectClassWithRegion(
		BilleClassID,
		*(regions[subRegion - 1].region));
	}
	rtiamb.subscribeObjectClassAttributesWithRegion(
	    BilleClassID, *(regions[region - 1].region), *attr_set, RTI_TRUE);
	subRegion = region ;

	// Update region
	rtiamb.unassociateRegionForUpdates(*(regions[pubRegion - 1].region),
					   local.ID);
	rtiamb.associateRegionForUpdates(*(regions[region - 1].region),
					 local.ID, *attr_set);
	pubRegion = region ;

	attr_set->empty();
	delete attr_set ;
    }
}

// ----------------------------------------------------------------------------
/** Carry out publications and subscriptions
 */
void
BillardDDM::publishAndSubscribe()
{
//    Billard::publishAndSubscribe();
    // Get all class and attributes handles
    getHandles();

    // Publish and subscribe to Bing interactions
    rtiamb.subscribeInteractionClass(BingClassID, RTI_TRUE);
    rtiamb.publishInteractionClass(BingClassID);
}

// $Id: BillardDDM.cc,v 3.9 2004/08/24 18:25:05 breholee Exp $
