// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2004, 2005  ONERA
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
// $Id: BillardDDM.cc,v 3.11 2005/03/13 17:45:16 breholee Exp $
// ----------------------------------------------------------------------------

#include "BillardDDM.hh"
#include "RegionImp.hh"

#ifndef X_DISPLAY_MISSING
#include "graph_c.hh"
#endif

using std::string ;
using std::auto_ptr ;

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
    : Billard(federate_name), numberOfRegions(3), subRegion(-1), pubRegion(-1)
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
    int width = 500 / numberOfRegions ;

    GeoID = rtiamb.getRoutingSpaceHandle("Geo");
    std::cout << "Geo space handle : " << GeoID << std::endl ;

    areas.clear();
    for (int i = 0 ; i < numberOfRegions ; ++i) {
	areas.push_back(Area());
	Area &area = areas.back();
	area.x = width * i ;
	area.y = 0 ;
	area.size = width ;
	std::cout << "Region " << i << "... " ;
	area.region = rtiamb.createRegion(GeoID, 1);
	std::cout << "ok" << std::endl ;
    }

    int region = (int) local.x / width ;
    AttributeHandle attrs[] = { AttrXID, AttrYID } ;
    Region *regs[] = { areas[region].region, areas[region].region } ;
    std::cout << "Register ball with region..." << std::endl ;
    local.ID = rtiamb.registerObjectInstanceWithRegion(BouleClassID,
						       federateName.c_str(),
						       attrs, regs, 2);
    D[pdDebug] << "Object created (handle " << local.ID << ")" << std::endl ;
    pubRegion = region ;
}

// ----------------------------------------------------------------------------
/** Check regions grid
 */
void
BillardDDM::checkRegions()
{
    int width = 500 / numberOfRegions ;
    int region = (int) local.x / width ;

    if (region != subRegion || region != pubRegion) {
	if (subRegion != -1)
	    drawRegion(true, subRegion, width);
	drawRegion(false, region, width);
	//	std::cout << "Connect to region " << region << std::endl ;
	auto_ptr<AttributeHandleSet> a(AttributeHandleSetFactory::create(3));
	a->add(AttrXID);
	a->add(AttrYID);

 	// Subscription
	if (subRegion != -1) {
	    rtiamb.unsubscribeObjectClassWithRegion(
		BilleClassID,
		*(areas[subRegion].region));
	}
	rtiamb.subscribeObjectClassAttributesWithRegion(
	    BilleClassID, *(areas[region].region), *a, RTI_TRUE);
  	subRegion = region ;

	// Update region
	if (pubRegion != -1) {
	    rtiamb.unassociateRegionForUpdates(*(areas[pubRegion].region),
					       local.ID);
	}
	rtiamb.associateRegionForUpdates(*(areas[region].region),
					 local.ID, *a);
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

// $Id: BillardDDM.cc,v 3.11 2005/03/13 17:45:16 breholee Exp $
