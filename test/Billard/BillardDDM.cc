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
// $Id: BillardDDM.cc,v 3.2 2003/10/20 09:38:23 breholee Exp $
// ----------------------------------------------------------------------------

#include "BillardDDM.hh"

#include <cassert>
#include <iostream>

using std::string ;
using std::cout ;
using std::endl ;

static const char *geo_name = "geo" ;
static const char *x_name = "x" ;
static const char *y_name = "y" ;

static pdCDebug D("BILLARD_DDM", __FILE__);

// ----------------------------------------------------------------------------
// DDM Billard constructor
BillardDDM::BillardDDM(string federate)
    : Billard(federate), regionSize(250)
{
    cout << "Billard 'DDM' Test" << endl ;
}

// ----------------------------------------------------------------------------
/** get DDM-related handles
 */
void
BillardDDM::getHandles()
{
    static bool done = false ;
    if (!done) {
	geo_id = rtiamb.getRoutingSpaceHandle(geo_name);
	x_id = rtiamb.getDimensionHandle(x_name, geo_id);
	y_id = rtiamb.getDimensionHandle(y_name, geo_id);
	if (verbose) {
	    cout << "Routing space handle: " << geo_id << endl ;
	    cout << "Dimension X handle: " << x_id << endl ;
	    cout << "Dimension Y handle: " << y_id << endl ;
	}
    }
    done = true ;
}

// ----------------------------------------------------------------------------
// create objects, regions, etc.
void
BillardDDM::declare()
{
    Billard::declare();
    getHandles();

    int r = (XMAX / regionSize) * (YMAX / regionSize);
    regions.reserve(r);
    D[pdDebug] << "reserve " << r << " regions ..." << endl ;

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
		    D[pdDebug] << "set x lower bound " << x << endl ;
		    region->setRangeLowerBound(0, x_id, x);
		    D[pdDebug] << "set x upper bound " << x + regionSize
			       << endl ;
		    region->setRangeUpperBound(0, x_id, x + regionSize);
		    D[pdDebug] << "set y lower bound " << y << endl ;
		    region->setRangeLowerBound(0, y_id, y);
		    D[pdDebug] << "set y upper bound " << y + regionSize
			       << endl ;
		    region->setRangeUpperBound(0, y_id, y + regionSize);
		}
		catch (ArrayIndexOutOfBounds &e) {
		    D[pdDebug] << __FILE__ << ":" << __LINE__ << ":"
			 << e._name << ": " << e._reason << endl ;
		}
		// rtiamb.notifyAboutRegionModification(*region);
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

// $Id: BillardDDM.cc,v 3.2 2003/10/20 09:38:23 breholee Exp $
