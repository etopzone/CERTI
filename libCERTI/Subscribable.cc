// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2005  ONERA
//
// This program is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: Subscribable.cc,v 3.11 2011/09/02 21:42:23 erk Exp $
// ----------------------------------------------------------------------------

#include "InteractionBroadcastList.hh"
#include "ObjectClassBroadcastList.hh"
#include "PrettyDebug.hh"
#include "RTIRegion.hh"
#include "Subscribable.hh"
#include "helper.hh"

#include <algorithm>

namespace {

PrettyDebug D("SUBSCRIBABLE", __FILE__);
}

namespace certi {

// ----------------------------------------------------------------------------
// Constructor
Subscriber::Subscriber(FederateHandle h) : handle(h), region(0)
{
}

// ----------------------------------------------------------------------------
// Constructor
Subscriber::Subscriber(FederateHandle h, const RTIRegion* r) : handle(h), region(r)
{
}

// ----------------------------------------------------------------------------
FederateHandle Subscriber::getHandle() const
{
    return handle;
}

// ----------------------------------------------------------------------------
const RTIRegion* Subscriber::getRegion() const
{
    return region;
}

// ----------------------------------------------------------------------------
bool Subscriber::operator==(const Subscriber& sub) const
{
    return (handle == sub.getHandle()) && (region == sub.getRegion());
}

// ----------------------------------------------------------------------------
bool Subscriber::equals(FederateHandle fed, const RTIRegion* r) const
{
    return handle == fed && region == r;
}

// ----------------------------------------------------------------------------
/** Check if subscriber's region matches (overlaps) with the one in
    parameter. If one of them is the default region (null) the result
    is 'true'
 */
bool Subscriber::match(const RTIRegion* r) const
{
    Debug(D, pdTrace) << "Match test: " << (region ? region->getHandle() : 0) << " vs " << (r ? r->getHandle() : 0)
                      << std::endl;
    return (region == 0) || (r == 0) || region->overlaps(*r);
}

// ----------------------------------------------------------------------------
// Constructor
Subscribable::Subscribable(const std::string& name) : Named(name)
{
}

// ----------------------------------------------------------------------------
Subscribable::~Subscribable()
{
    if (!subscribers.empty())
        Debug(D, pdError) << "Subscribers list not empty at termination." << std::endl;
}

// ----------------------------------------------------------------------------
/** Removes a subscribed federate. All subscription regions are concerned.
    @param fed Federate to unsubscribe
 */
void Subscribable::unsubscribe(FederateHandle fed)
{
    std::remove_if(subscribers.begin(), subscribers.end(), HandleComparator<Subscriber>(fed));
}

// ----------------------------------------------------------------------------
/** Unsubscribe a federate with a region
    @param fed Federate to unsubscribe
    @param region Region for unsubscription (other subscriptions may remain)
 */
void Subscribable::unsubscribe(FederateHandle fed, const RTIRegion* region)
{
    subscribers.remove(Subscriber(fed, region));
}

// ----------------------------------------------------------------------------
/** Indicates whether a federate is subscribed with a particular region
    @param fed federate 
    @param region region
    @return true if the federate/region pair is subscriber
 */
bool Subscribable::isSubscribed(FederateHandle fed, const RTIRegion* region) const
{
    return std::find(subscribers.begin(), subscribers.end(), Subscriber(fed, region)) != subscribers.end();
}

// ----------------------------------------------------------------------------
/** Indicates whether a federate is subscribed with any region
    @param fed federate 
    @return true if the federate is subscribed
 */
bool Subscribable::isSubscribed(FederateHandle fed) const
{
    return std::find_if(subscribers.begin(), subscribers.end(), HandleComparator<Subscriber>(fed)) != subscribers.end();
}

// ----------------------------------------------------------------------------
/** Subscribe a federate with a region
    @param fed Federate to subscribe
    @param region Region to use for subscription (0 for default region)
 */
void Subscribable::subscribe(FederateHandle fed, const RTIRegion* region)
{
    if (!isSubscribed(fed, region)) {
        checkFederateAccess(fed, "Subscribe");
        subscribers.push_back(Subscriber(fed, region));
    }
    else {
        Debug(D, pdError) << "Inconsistency in subscribe request from federate " << fed << std::endl;
    }
}

// ----------------------------------------------------------------------------
/** Add federates and associated attribute to a broadcast list.
    @param lst Broadcast list where federates/handles should be added
    @param region Region to check for overlap
    @param handle Handle of this object (Subscribable subclass)
 */
void Subscribable::addFederatesIfOverlap(ObjectClassBroadcastList& lst, const RTIRegion* region, Handle handle) const
{
    std::list<Subscriber>::const_iterator it = subscribers.begin();
    for (; it != subscribers.end(); ++it) {
        if (it->match(region))
            lst.addFederate(it->getHandle(), handle);
    }
}

// ----------------------------------------------------------------------------
/** Add federates to a broadcast list.
    @param lst Broadcast list where federates/handles should be added
    @param region Region to check for overlap
 */
void Subscribable::addFederatesIfOverlap(InteractionBroadcastList& lst, const RTIRegion* region) const
{
    std::list<Subscriber>::const_iterator it = subscribers.begin();
    for (; it != subscribers.end(); ++it) {
        if (it->match(region))
            lst.addFederate(it->getHandle());
    }
}

} // namespace certi

// $Id: Subscribable.cc,v 3.11 2011/09/02 21:42:23 erk Exp $
