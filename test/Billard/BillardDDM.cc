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
// $Id: BillardDDM.cc,v 3.4 2003/11/10 15:06:41 breholee Exp $
// ----------------------------------------------------------------------------

#include "BillardDDM.hh"

#include <cassert>
#include <iostream>

using std::string ;
using std::cout ;
using std::endl ;

static pdCDebug D("BILLARD_DDM", __FILE__);

// ----------------------------------------------------------------------------
// DDM Billard constructor
BillardDDM::BillardDDM(string federate)
    : Billard(federate), regionSize(100),
      geo_name("geo"), dimx_name("X"), dimy_name("Y")

{
    if (verbose)
	cout << "Billard 'DDM' Test" << endl ;
}

// ----------------------------------------------------------------------------
/** Get DDM-related handles
 */
void
BillardDDM::getHandles()
{
    D[pdDebug] << "DDM getHandles" << endl ;

    static bool done = false ;
    if (!done) {
	geo_id = rtiamb.getRoutingSpaceHandle(geo_name);
	dimx_id = rtiamb.getDimensionHandle(dimx_name, geo_id);
	dimy_id = rtiamb.getDimensionHandle(dimy_name, geo_id);
	if (verbose) {
	    cout << "Routing space handle: " << geo_id << endl ;
	    cout << "Dimension X handle: " << dimx_id << endl ;
	    cout << "Dimension Y handle: " << dimy_id << endl ;
	}
    }
    done = true ;
}

// ----------------------------------------------------------------------------
/** Create objects, regions, etc.
 */
void
BillardDDM::declare()
{
    Billard::declare();
    getHandles();

    int r = (XMAX / regionSize) * (YMAX / regionSize);

    for (int x = 0 ; x < XMAX ; x += regionSize) {
	for (int y = 0 ; y < YMAX ; y += regionSize) {
	    try {
		Region *region = rtiamb.createRegion(geo_id, 1);
	        D[pdDebug] << "created region with "
			   << region->getNumberOfExtents() << " extent(s)"
			   << endl ;
		try {
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
		}
		catch (ArrayIndexOutOfBounds &e) {
		    D[pdDebug] << __FILE__ << ":" << __LINE__ << ":"
			 << e._name << ": " << e._reason << endl ;
		}
		rtiamb.notifyAboutRegionModification(*region);
		regions.push_back(region);
	    }
	    catch (Exception &e) {
		D[pdDebug] << __FILE__ << ":" << __LINE__ << ":exception:"
			   << e._name << " [" << e._reason << "]" << endl ;
	    }
	}
    }   

    D[pdDebug] << "created " << regions.size() << " regions" << endl ;        
}

// $Id: BillardDDM.cc,v 3.4 2003/11/10 15:06:41 breholee Exp $
