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
// $Id: Subscribable.cc,v 3.1 2005/04/02 15:41:04 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Subscribable.hh"
#include "helper.hh"
#include "PrettyDebug.hh"

namespace {

PrettyDebug D("SUBSCRIBABLE", __FILE__);

}

namespace certi {

// ----------------------------------------------------------------------------
Subscribable::~Subscribable()
{
    if (!subscribers.empty())
	D[pdError] << "Subscribers list not empty at termination." << std::endl ;
}

// ----------------------------------------------------------------------------
/** Removes a subscribed federate. All subscription regions are concerned.
    @param fed Federate to unsubscribe
 */
void
Subscribable::unsubscribe(FederateHandle fed)
{
    subscribers.remove_if(HandleComparator<Subscriber>(fed));
}

// ----------------------------------------------------------------------------
/** Unsubscribe a federate with a region
    @param fed Federate to unsubscribe
    @param region Region for unsubscription (other subscriptions may remain)
 */
void
Subscribable::unsubscribe(FederateHandle fed, const RTIRegion *region)
{
    subscribers.remove(Subscriber(fed, region));
}

// ----------------------------------------------------------------------------
/** Indicates whether a federate is subscribed with a particular region
    @param fed federate 
    @param region region
    @return true if the federate/region pair is subscriber
 */
bool
Subscribable::isSubscribed(FederateHandle fed,
			   const RTIRegion *region) const
{
    return std::find(subscribers.begin(),
		     subscribers.end(),
		     Subscriber(fed, region)) != subscribers.end();
}

// ----------------------------------------------------------------------------
/** Indicates whether a federate is subscribed with any region
    @param fed federate 
    @return true if the federate is subscribed
 */
bool
Subscribable::isSubscribed(FederateHandle fed) const
{
    return std::find_if(subscribers.begin(),
			subscribers.end(),
			HandleComparator<Subscriber>(fed)) != subscribers.end();
}

// ----------------------------------------------------------------------------
/** Subscribe a federate with a region
    @param fed Federate to subscribe
    @param region Region to use for subscription (0 for default region)
 */
void
Subscribable::subscribe(FederateHandle fed, const RTIRegion *region)
{
    if (!isSubscribed(fed, region)) {
        checkFederateAccess(fed, "Subscribe");
	subscribers.push_back(Subscriber(fed, region));
    }
    else
        D[pdError] << "Inconsistency in subscribe request from federate "
		   << fed << std::endl ;
}

} // namespace certi

// $Id: Subscribable.cc,v 3.1 2005/04/02 15:41:04 breholee Exp $
