// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: ObjectClassAttribute.cc,v 3.18 2005/03/11 13:47:18 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "ObjectClassAttribute.hh"

#include "PrettyDebug.hh"

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

    list<Subscriber *>::iterator s ;
    for (s = subscribers.begin(); s != subscribers.end(); s++) {
        delete (*s);
    }
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
ObjectClassAttribute::deleteSubscriber(FederateHandle fed, RegionImp *region)
{
    list<Subscriber *>::iterator i ;
    for (i = subscribers.begin(); i != subscribers.end(); ++i) {
	if ((*i)->match(fed, region)) {
            delete *i ;
            subscribers.erase(i);
            return ;
        }
    }
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
//! Returns true if federate has subscribed to this attribute
bool
ObjectClassAttribute::hasSubscribed(FederateHandle fed, RegionImp *region) const
{
    list<Subscriber *>::const_iterator i ;
    for (i = subscribers.begin(); i != subscribers.end(); ++i) {
        if ((*i)->match(fed, region))
            return true ;
    }
    return false ;
}

// ----------------------------------------------------------------------------
//! Returns true if federate has subscribed to this attribute
bool
ObjectClassAttribute::hasSubscribed(FederateHandle fed) const
{
    return hasSubscribed(fed, 0);
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
//! unsubscribe
void
ObjectClassAttribute::unsubscribe(FederateHandle fed, RegionImp *region)
    throw (RTIinternalError)
{
    if (hasSubscribed(fed, region)) {
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
//! unsubscribe
void
ObjectClassAttribute::unsubscribe(FederateHandle fed)
    throw (RTIinternalError)
{
    unsubscribe(fed, 0);
}

// ----------------------------------------------------------------------------
//! subscribe
void
ObjectClassAttribute::subscribe(FederateHandle fed, RegionImp *region)
    throw (RTIinternalError, SecurityError)
{
    if (!hasSubscribed(fed, region)) {
        checkFederateAccess(fed, "Subscribe");

	Subscriber *sub = new Subscriber(fed, region);
	if (region) {
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
//! subscribe
void
ObjectClassAttribute::subscribe(FederateHandle fed)
    throw (RTIinternalError, SecurityError)
{
    subscribe(fed, 0);
}

// ----------------------------------------------------------------------------
//! Add all attribute's subscribers to the broadcast list
void
ObjectClassAttribute::updateBroadcastList(ObjectClassBroadcastList *ocblist,
					  const RegionImp *region)
{
    switch(ocblist->message->type) {

      case NetworkMessage::REFLECT_ATTRIBUTE_VALUES: {
          list<Subscriber *>::iterator i ;
          for (i = subscribers.begin(); i != subscribers.end(); i++) {
	      if ((*i)->match(region))
		  ocblist->addFederate((*i)->getHandle(), handle);
	      
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

// $Id: ObjectClassAttribute.cc,v 3.18 2005/03/11 13:47:18 breholee Exp $
