// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
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
// $Id: BillardDDM.cc,v 3.6 2003/12/01 16:41:54 breholee Exp $
// ----------------------------------------------------------------------------

#include "BillardDDM.hh"

#include <cassert>
#include <iostream>

using std::string ;
using std::cout ;
using std::endl ;

static pdCDebug D("BILLARD_DDM", __FILE__);

// ----------------------------------------------------------------------------

/** Constructor
 */
BillardDDM::BillardDDM(string federate)
    : Billard(federate), regionSize(100),
      geo_name("geo"), dimx_name("X"), dimy_name("Y")
{
    if (verbose)
	cout << "Billard 'DDM' Test" << endl ;
}

/** Destructor
 */
BillardDDM::~BillardDDM() { }

/** Get DDM-related handles. Get the routing space handle, and
    dimension handles.
 */
void
BillardDDM::getHandles()
{
    D[pdDebug] << "DDM getHandles" << endl ;

    static bool done = false ;
    if (!done) {
	try {
	    geo_id = rtiamb.getRoutingSpaceHandle(geo_name);
	    dimx_id = rtiamb.getDimensionHandle(dimx_name, geo_id);
	    dimy_id = rtiamb.getDimensionHandle(dimy_name, geo_id);
	  	}
	catch (NameNotFound &e) {
	    cout << "Error: Routing Space or Dimension name not found" << endl ;
	    throw e ;
	}
	if (verbose) {
	    cout << "Routing space handle: " << geo_id << endl ;
	    cout << "Dimension X handle: " << dimx_id << endl ;
	    cout << "Dimension Y handle: " << dimy_id << endl ;
	}
    }
    done = true ;
}

/** Create objects and regions.
 */
void
BillardDDM::declare()
{
    Billard::declare();
    getHandles();

    for (int x = 0 ; x < XMAX ; x += regionSize) {
	regions.resize(x + 1);
	for (int y = 0 ; y < YMAX ; y += regionSize) {
	    try {
		Region *region = rtiamb.createRegion(geo_id, 1);
	        D[pdDebug] << "created region with "
			   << region->getNumberOfExtents() << " extent(s)"
			   << endl ;

		assert(region);
		assert(region->getNumberOfExtents() == 1);
		
		region->setRangeLowerBound(0, dimx_id, x);
		D[pdDebug] << "x lower bound: "
			   << region->getRangeLowerBound(0, dimx_id)
			   << endl ;
		
		region->setRangeUpperBound(0, dimx_id, x + regionSize);
		D[pdDebug] << "x upper bound: " 
			   << region->getRangeUpperBound(0, dimx_id)
			   << endl ;
		
		region->setRangeLowerBound(0, dimy_id, y);
		D[pdDebug] << "y lower bound: "
			   << region->getRangeLowerBound(0, dimy_id)
			   << endl ;
		
		region->setRangeUpperBound(0, dimy_id, y + regionSize);
		D[pdDebug] << "y upper bound: "
			   << region->getRangeUpperBound(0, dimy_id)
			   << endl ;

		rtiamb.notifyAboutRegionModification(*region);
 		regions[x].push_back(region);
	    }
	    catch (Exception &e) {
		D[pdDebug] << __FILE__ << ":" << __LINE__ << ":exception:"
			   << e._name << " [" << e._reason << "]" << endl ;
	    }
	}
    }   

    D[pdDebug] << "created " << regions.size() << " regions" << endl ;        
}

/** Carry out publications and subscriptions
 */
void
Billard::publishAndSubscribe()
{
    getHandles();

    // Add PositionX et PositionY to the attribute set
    AttributeHandleSet *AttributeSet = AttributeHandleSetFactory::create(3);
    AttributeSet->add(AttrXID);
    AttributeSet->add(AttrYID);

    // Subscribe to Bille objects.
    D[pdDebug] << "subscribe: class " << BilleClassID << ", attributes "
	       << AttrXID << " and " << AttrYID << "... " ;
    rtiamb.subscribeObjectClassAttributes(BilleClassID, *AttributeSet,
					  RTI_TRUE);
    D[pdDebug] << "done." << endl ;

    // Publish Boule Objects.
    AttributeSet->add(AttrColorID);
    rtiamb.publishObjectClass(BouleClassID, *AttributeSet);

    // Publish and subscribe to Bing interactions
    rtiamb.subscribeInteractionClass(BingClassID, RTI_TRUE);
    rtiamb.publishInteractionClass(BingClassID);

    AttributeSet->empty();

    D.Out(pdInit, "Local Objects and Interactions published and subscribed.");
}

// $Id: BillardDDM.cc,v 3.6 2003/12/01 16:41:54 breholee Exp $
