// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: ObjectClassAttribute.cc,v 3.21 2005/03/25 17:22:36 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "ObjectClassAttribute.hh"

#include "PrettyDebug.hh"
#include "helper.hh"

#include <iostream>
#include <list>
#include <assert.h>

using std::list ;
using std::cout ;
using std::endl ;

namespace certi {

static pdCDebug D("OBJECTCLASSATTRIBUTE", "(Obj.Cl.Attr) ");

// ----------------------------------------------------------------------------
/*! Throw SecurityError if the Federate is not allowed to access the Object
  Class, and print an Audit message containing Reason.
*/
void
ObjectClassAttribute::checkFederateAccess(FederateHandle fed,
					  const char *reason)
    throw (SecurityError)
{
    if (server && !server->canFederateAccessData(fed, level)) {
        cout << "Attribute " << handle << " : SecurityError for federate "
             << fed << '(' << reason << ")." << endl ;
        throw SecurityError("Federate should not access ObjectClassAttribute.");
    }
}

// ----------------------------------------------------------------------------
//! No parameters constructor.
/*! This constructor initialize the attribute with default parameters.
 */
ObjectClassAttribute::ObjectClassAttribute()
    : level(PublicLevelID), order(RECEIVE),
      transport(BEST_EFFORT), server(0), handle(0), space(0)
{
}

// ----------------------------------------------------------------------------
//! Constructor : Copy Handle, Name, Space, Order and Transport
ObjectClassAttribute::ObjectClassAttribute(ObjectClassAttribute *source)
{
    if (source == 0)
        throw RTIinternalError("NULL Attribute when copying it.");

    handle = source->getHandle();
    level = source->level ;
    space = source->getSpace();
    name = source->name ;
    order = source->order ;
    transport = source->transport ;
    server = source->server ;
}

// ----------------------------------------------------------------------------
//! Destructor (Empty private Lists, and free Name memory)
ObjectClassAttribute::~ObjectClassAttribute()
{
    // Deleting Publishers
    if (!publishers.empty())
        D[pdError] << "Attribute " << handle
		   << ": Publishers list not empty at termination." << endl ;

    list<Publisher *>::iterator p ;
    for (p = publishers.begin(); p != publishers.end(); p++) {
        delete (*p);
    }
    publishers.clear();

    // Deleting Subscribers
    if (!subscribers.empty())
        D[pdError] << "Attribute " << handle
		   << ": Subscribers list not empty at termination." << endl ;
    subscribers.clear();
}

// ----------------------------------------------------------------------------
//! Removes a publishing federate
void
ObjectClassAttribute::deletePublisher(FederateHandle fed)
{
    list<Publisher *>::iterator i ;
    for (i = publishers.begin(); i != publishers.end(); ++i) {
	if ((*i)->getHandle() == fed) {
            delete *i ;
            publishers.erase(i);
	    return ;
        }
    }
}

// ----------------------------------------------------------------------------
//! Removes a subscribed federate
void
ObjectClassAttribute::deleteSubscriber(FederateHandle fed)
{
    subscribers.remove_if(HandleComparator<Subscriber>(fed));
}

// ----------------------------------------------------------------------------
//! Removes a subscribed federate
void
ObjectClassAttribute::deleteSubscriber(FederateHandle fed, const RTIRegion *region)
{
    subscribers.remove(Subscriber(fed, region));
}

// ----------------------------------------------------------------------------
//! Displays the attribute information (handle, name and level id)
void
ObjectClassAttribute::display() const
{
    cout << " Attribute " << handle << ':' ;

    if (name.length() > 0)
        cout << '\"' << name << '\"' ;
    else
        cout << "(no name)" ;

    cout << "[Level " << level << ']' << endl ;
}

// ----------------------------------------------------------------------------
//! returns true if federate is publishing this attribute
bool
ObjectClassAttribute::isPublishing(FederateHandle fed) const
{
    list<Publisher *>::const_iterator i = publishers.begin();
    for (i = publishers.begin(); i != publishers.end(); i++) {
        if ((*i)->getHandle() == fed)
            return true ;
    }
    return false ;
}

// ----------------------------------------------------------------------------
/** Indicates whether a federate is subscribed with a particular region
    @param fed federate 
    @param region region
    @return true if the federate/region pair is subscriber
 */
bool
ObjectClassAttribute::isSubscribed(FederateHandle fed,
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
ObjectClassAttribute::isSubscribed(FederateHandle fed) const
{
    return std::find_if(subscribers.begin(),
			subscribers.end(),
			HandleComparator<Subscriber>(fed)) != subscribers.end();
}

// ----------------------------------------------------------------------------
//! publish
void
ObjectClassAttribute::publish(FederateHandle fed)
    throw (RTIinternalError, SecurityError)
{
    if (!isPublishing(fed)) {
        checkFederateAccess(fed, "Publish");
        D[pdInit] << "Attribute " << handle << ": Added Federate " << fed
		  << " to publishers list." << endl ;
	publishers.push_front(new Publisher(fed));
    }
    else
        D[pdError] << "Attribute " << handle
		   << ": Inconsistent publish request from Federate "
		   << fed << endl ;
}

// ----------------------------------------------------------------------------
//! unpublish
void
ObjectClassAttribute::unpublish(FederateHandle fed)
    throw (RTIinternalError, SecurityError)
{
    if (isPublishing(fed)) {
        D[pdTerm] << "Attribute " << handle << ": Removed Federate " << fed
		  << " from publishers list." << endl ;
        deletePublisher(fed);
    }

    else
        D[pdError] << "Attribute " << handle
		   << ": Inconsistent publish request from Federate "
		   << fed << endl ;
}

// ----------------------------------------------------------------------------
//! Sets the name of this attribute
void
ObjectClassAttribute::setName(const char *new_name)
{
    name = new_name == 0 ? "" : new_name ;
}

// ----------------------------------------------------------------------------
void
ObjectClassAttribute::setHandle(AttributeHandle h)
{
    handle = h ;
}

// ----------------------------------------------------------------------------
AttributeHandle
ObjectClassAttribute::getHandle() const
{
    return handle ;
}

// ----------------------------------------------------------------------------
void
ObjectClassAttribute::setSpace(SpaceHandle h)
{
    space = h ;
}

// ----------------------------------------------------------------------------
SpaceHandle
ObjectClassAttribute::getSpace() const
{
    return space ;
}

// ----------------------------------------------------------------------------
/** Unsubscribe a federate if it is associated with a particular region
 */
void
ObjectClassAttribute::unsubscribe(FederateHandle fed, const RTIRegion *region)
    throw (RTIinternalError)
{
    if (isSubscribed(fed, region)) {
        deleteSubscriber(fed, region);
        D[pdTerm] << "Attribute " << handle << ": Removed Federate "
		  << fed << " from subscribers list." << endl ;
    }
    else {
        D[pdError] << "Attribute " << handle << ": Unconsistent unsubscribe "
		   << "request from federate " << fed << endl ;
    }
}

// ----------------------------------------------------------------------------
/** Remove this federate's subscriptions
 */
void
ObjectClassAttribute::unsubscribe(FederateHandle fed)
    throw (RTIinternalError)
{
    if (isSubscribed(fed)) {
        deleteSubscriber(fed);
        D[pdTerm] << "Attribute " << handle << ": Removed Federate "
		  << fed << " from subscribers list." << endl ;
    }
    else {
        D[pdError] << "Attribute " << handle << ": Unconsistent unsubscribe "
		   << "request from federate " << fed << endl ;
    }
}

// ----------------------------------------------------------------------------
//! subscribe
void
ObjectClassAttribute::subscribe(FederateHandle fed, const RTIRegion *region)
    throw (RTIinternalError, SecurityError)
{
    if (!isSubscribed(fed, region)) {
        checkFederateAccess(fed, "Subscribe");

	Subscriber sub(fed, region);
	if (region != 0) {
	    subscribers.push_back(sub);
	}
	else {
	    subscribers.push_front(sub);
	}
	
        D[pdInit] << "Attribute " << handle << ": Added Federate " << fed
		  << " to subscribers list." << endl ;
    }
    else
        D[pdError] << "Attribute " << handle
		   << ": Unconsistent subscribe request from federate "
		   << fed << endl ;
}

// ----------------------------------------------------------------------------
//! Add all attribute's subscribers to the broadcast list
void
ObjectClassAttribute::updateBroadcastList(ObjectClassBroadcastList *ocblist,
					  const RTIRegion *region)
{
    switch(ocblist->message->type) {

      case NetworkMessage::REFLECT_ATTRIBUTE_VALUES: {
          list<Subscriber>::iterator i ;
          for (i = subscribers.begin(); i != subscribers.end(); i++) {
	      if (i->match(region))
		  ocblist->addFederate(i->getHandle(), handle);
          }
      } break ;
      case NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION: {
          list<Publisher *>::iterator i ;
          for (i = publishers.begin(); i != publishers.end(); i++) {
              ocblist->addFederate((*i)->getHandle(), handle);
          }
      } break ;
      
      default: ; // nothing done
    }
}

} // namespace

// $Id: ObjectClassAttribute.cc,v 3.21 2005/03/25 17:22:36 breholee Exp $
