// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2004-2005  ONERA
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
// $Id: BillardDDM.cc,v 3.14 2005/03/28 19:03:29 breholee Exp $
// ----------------------------------------------------------------------------

#include "BillardDDM.hh"

#ifndef X_DISPLAY_MISSING
#include "graph_c.hh"
#endif

using std::string ;
using std::auto_ptr ;
using std::vector ;

// ----------------------------------------------------------------------------
namespace {
PrettyDebug D("BILLARD_DDM", __FILE__);

// ----------------------------------------------------------------------------
/** Draw a square region
 */
void
drawRegion(bool display, int position, int width)
{
#ifndef X_DISPLAY_MISSING
    point origin ;
    origin.X = width * position;
    origin.Y = 0 ;
    rectangler region = Definerr(origin, width, 100, COUL_UNIE,
				 display ? GRAY : WHITE);
    Drawrr(region);
#endif
}
}

// ----------------------------------------------------------------------------
/** Constructor
 */
BillardDDM::BillardDDM(string federate_name)
    : Billard(federate_name), numberOfRegions(4), subRegion(-1), pubRegion(-1)
{
    std::cout << "BillardDDM" << std::endl ;
}

// ----------------------------------------------------------------------------
/** Destructor
 */
BillardDDM::~BillardDDM()
{
}

// ----------------------------------------------------------------------------
/** Declare regions and ball
 */
void
BillardDDM::declare()
{
    int width = XMAX / numberOfRegions ;

    GeoID = rtiamb.getRoutingSpaceHandle("Geo");
    dimX = rtiamb.getDimensionHandle("X", GeoID);
    dimY = rtiamb.getDimensionHandle("Y", GeoID);

    areas.clear();
    for (int i = 0 ; i < numberOfRegions ; ++i) {
	areas.push_back(Area());
	Area &area = areas.back();
	area.x = width * i ;
	area.y = 0 ;
	area.size = width ;
	std::cout << "Region " << i << "... " ;
	area.region = rtiamb.createRegion(GeoID, 1);
	area.region->setRangeLowerBound(0, dimX, area.x);
	area.region->setRangeUpperBound(0, dimX, area.x + width - 1);
	area.region->setRangeLowerBound(0, dimY, 0);
	area.region->setRangeUpperBound(0, dimY, YMAX);
	rtiamb.notifyAboutRegionModification(*area.region);
	std::cout << "ok" << std::endl ;
    }

    local.ID = registerBallInstance(federateName.c_str());
    D[pdDebug] << "Object created (handle " << local.ID << ")" << std::endl ;
}

// ----------------------------------------------------------------------------
/** Check regions grid
 */
void
BillardDDM::checkRegions()
{
    int width = XMAX / numberOfRegions ;
    int region = (int) local.x / width ;

    if (region != subRegion || region != pubRegion) {
	std::cout << "Updating regions ..." << std::endl ;

	auto_ptr<AttributeHandleSet> a(AttributeHandleSetFactory::create(3));
	a->add(AttrXID);
	a->add(AttrYID);

 	// Subscription
	rtiamb.subscribeObjectClassAttributesWithRegion(
	    BilleClassID, *(areas[region].region), *a, RTI_TRUE);
	if (subRegion != -1) {
	    rtiamb.unsubscribeObjectClassWithRegion(
		BilleClassID,
		*(areas[subRegion].region));
	}
  	subRegion = region ;

	// Update region
	rtiamb.associateRegionForUpdates(*areas[region].region, local.ID, *a);
	if (pubRegion != -1) {
	    rtiamb.unassociateRegionForUpdates(*areas[pubRegion].region, local.ID);
	}
	pubRegion = region ;	
    }
}

// ----------------------------------------------------------------------------
/** Carry out publications and subscriptions. In this DDM demo, only object
    publications are done. Other (un)publications and (un)subscriptions 
    occur regularly in the checkRegions function.
 */
void
BillardDDM::publishAndSubscribe()
{
    auto_ptr<AttributeHandleSet> attributes(AttributeHandleSetFactory::create(3));

    getHandles();
    attributes->add(AttrXID);
    attributes->add(AttrYID);
    attributes->add(AttrColorID);

    rtiamb.publishObjectClass(BouleClassID, *attributes);
    rtiamb.publishInteractionClass(BingClassID);

    D.Out(pdInit, "Local Objects and Interactions published.");
}

// $Id: BillardDDM.cc,v 3.14 2005/03/28 19:03:29 breholee Exp $
