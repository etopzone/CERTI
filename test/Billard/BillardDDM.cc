// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2004-2005  ONERA
//
// This file is part of Billard
//
// Billard is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// Billard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: BillardDDM.cc,v 3.22 2009/10/21 20:04:46 erk Exp $
// ----------------------------------------------------------------------------

#include "BillardDDM.hh"
#include "PrettyDebug.hh"

#ifndef X_DISPLAY_MISSING
#include "graph_c.hh"
#endif

#include <algorithm>
#include <cmath>
#include <iostream>

#ifdef _WIN32
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#endif

#include <memory>

using std::string;
using std::unique_ptr;
using std::vector;

// ============================================================================
namespace {
PrettyDebug D("BILLARD_DDM", __FILE__);

// ----------------------------------------------------------------------------
/** Draw a square region
 */
void drawRegion(bool display, int position, int width)
{
#ifndef X_DISPLAY_MISSING
    point origin;
    origin.X = width * position;
    origin.Y = 0;
    rectangler region = Definerr(origin, width, 100, COUL_UNIE, display ? GRAY : WHITE);
    Drawrr(region);
#else
    (void) display;
    (void) position;
    (void) width;
#endif
}

} // anonymous namespace

// ============================================================================
/** Constructor
 */
BillardStaticDDM::BillardStaticDDM(std::string federate_name)
    : Billard(federate_name), numberOfRegions(4), subRegion(-1), pubRegion(-1)
{
    std::cout << "BillardStaticDDM" << std::endl;
}

// ----------------------------------------------------------------------------
/** Declare regions and ball
 */
void BillardStaticDDM::declare()
{
    int width = XMAX / numberOfRegions;

    GeoID = rtiamb.getRoutingSpaceHandle("Geo");
    dimX = rtiamb.getDimensionHandle("X", GeoID);
    dimY = rtiamb.getDimensionHandle("Y", GeoID);

    std::cout << "Create regions";

    for (int i = 0; i < numberOfRegions; ++i) {
        // Subscription region
        RTI::Region* region = rtiamb.createRegion(GeoID, 1);
        sub.push_back(region);
        const int margin = 20;
        region->setRangeLowerBound(0, dimX, std::max(0, width * i - margin));
        region->setRangeUpperBound(0, dimX, std::min(XMAX - 1, width * (i + 1) - 1 + margin));
        region->setRangeLowerBound(0, dimY, 0);
        region->setRangeUpperBound(0, dimY, YMAX);
        rtiamb.notifyAboutRegionModification(*region);
        std::cout << ".";

        // Publication region
        region = rtiamb.createRegion(GeoID, 1);
        pub.push_back(region);
        region->setRangeLowerBound(0, dimX, width * i);
        region->setRangeUpperBound(0, dimX, width * (i + 1) - 1);
        region->setRangeLowerBound(0, dimY, 0);
        region->setRangeUpperBound(0, dimY, YMAX);
        rtiamb.notifyAboutRegionModification(*region);
        std::cout << ".";
    }
    std::cout << " done." << std::endl;

    local.ID = registerBallInstance(federateName.c_str());
    Debug(D, pdDebug) << "Object created (handle " << local.ID << ")" << std::endl;
}

// ----------------------------------------------------------------------------
/** Check regions grid
 */
void BillardStaticDDM::checkRegions()
{
    int width = XMAX / numberOfRegions;
    int region = static_cast<int>(local.x) / width;

    if (region != subRegion || region != pubRegion) {
        unique_ptr<RTI::AttributeHandleSet> attributes(RTI::AttributeHandleSetFactory::create(3));
        attributes->add(AttrXID);
        attributes->add(AttrYID);

        // Subscription
        rtiamb.subscribeObjectClassAttributesWithRegion(BilleClassID, *sub[region], *attributes, RTI::RTI_TRUE);
        if (subRegion != -1) {
            rtiamb.unsubscribeObjectClassWithRegion(BilleClassID, *sub[subRegion]);
        }
        subRegion = region;

        // Update region
        rtiamb.associateRegionForUpdates(*pub[region], local.ID, *attributes);
        if (pubRegion != -1) {
            rtiamb.unassociateRegionForUpdates(*pub[pubRegion], local.ID);
        }
        pubRegion = region;
    }
}

// ----------------------------------------------------------------------------
/** Carry out publications and subscriptions. In this DDM demo, only object
    publications are done. Other (un)publications and (un)subscriptions 
    occur regularly in the checkRegions function.
 */
void BillardStaticDDM::publishAndSubscribe()
{
    unique_ptr<RTI::AttributeHandleSet> attributes(RTI::AttributeHandleSetFactory::create(3));

    getHandles();
    attributes->add(AttrXID);
    attributes->add(AttrYID);
    attributes->add(AttrColorID);

    rtiamb.publishObjectClass(BouleClassID, *attributes);
    rtiamb.publishInteractionClass(BingClassID);

    Debug(D, pdInit) << "Local Objects and Interactions published." << std::endl;
}

// ----------------------------------------------------------------------------
/** Resign federation
 */
void BillardStaticDDM::resign()
{
    if (subRegion != -1)
        rtiamb.unsubscribeObjectClassWithRegion(BilleClassID, *(sub[subRegion]));

    if (pubRegion != -1)
        rtiamb.unassociateRegionForUpdates(*pub[pubRegion], local.ID);

    for (int i = 0; i < numberOfRegions; ++i) {
        rtiamb.deleteRegion(sub[i]);
        rtiamb.deleteRegion(pub[i]);
    }
    Billard::resign();
}

// ============================================================================
/** Constructor
 */
BillardDynamicDDM::BillardDynamicDDM(std::string federate_name) : Billard(federate_name), region(0)
{
    std::cout << "BillardDynamicDDM" << std::endl;
}

// ----------------------------------------------------------------------------
/** Declare regions and ball
 */
void BillardDynamicDDM::declare()
{
    GeoID = rtiamb.getRoutingSpaceHandle("Geo");
    dimX = rtiamb.getDimensionHandle("X", GeoID);
    dimY = rtiamb.getDimensionHandle("Y", GeoID);

    region = rtiamb.createRegion(GeoID, 1);

    local.ID = registerBallInstance(federateName.c_str());
    Debug(D, pdDebug) << "Object created (handle " << local.ID << ")" << std::endl;

    unique_ptr<RTI::AttributeHandleSet> attributes(RTI::AttributeHandleSetFactory::create(3));
    attributes->add(AttrXID);
    attributes->add(AttrYID);

    rtiamb.subscribeObjectClassAttributesWithRegion(BilleClassID, *region, *attributes, RTI::RTI_TRUE);

    rtiamb.associateRegionForUpdates(*region, local.ID, *attributes);
}

// ----------------------------------------------------------------------------
/** Check regions grid
 */
void BillardDynamicDDM::checkRegions()
{
    const int margin = 40;

    region->setRangeLowerBound(0, dimX, std::max(0, static_cast<int>(local.x) - margin));
    region->setRangeUpperBound(0, dimX, static_cast<int>(local.x) + margin);
    region->setRangeLowerBound(0, dimY, std::max(0, static_cast<int>(local.y) - margin));
    region->setRangeUpperBound(0, dimY, static_cast<int>(local.y) + margin);

    rtiamb.notifyAboutRegionModification(*region);
}

// ----------------------------------------------------------------------------
/** Carry out publications and subscriptions. In this DDM demo, only object
    publications are done. Other (un)publications and (un)subscriptions 
    occur regularly in the checkRegions function.
 */
void BillardDynamicDDM::publishAndSubscribe()
{
    unique_ptr<RTI::AttributeHandleSet> attributes(RTI::AttributeHandleSetFactory::create(3));

    getHandles();
    attributes->add(AttrXID);
    attributes->add(AttrYID);
    attributes->add(AttrColorID);

    rtiamb.publishObjectClass(BouleClassID, *attributes);
    rtiamb.publishInteractionClass(BingClassID);

    Debug(D, pdInit) << "Local Objects and Interactions published." << std::endl;
}

// ----------------------------------------------------------------------------
/** Resign federation
 */
void BillardDynamicDDM::resign()
{
    rtiamb.unsubscribeObjectClassWithRegion(BilleClassID, *region);
    rtiamb.unassociateRegionForUpdates(*region, local.ID);

    rtiamb.deleteRegion(region);

    Billard::resign();
}
