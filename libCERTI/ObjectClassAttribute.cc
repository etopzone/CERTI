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
// $Id: ObjectClassAttribute.cc,v 3.27.2.1 2008/04/10 14:57:49 erk Exp $
// ----------------------------------------------------------------------------

#include "ObjectClassAttribute.hh"
#include "ObjectClassBroadcastList.hh"
#include "SecurityServer.hh"
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
    setName(source->getName());
    order = source->order ;
    transport = source->transport ;
    server = source->server ;
}

// ----------------------------------------------------------------------------
//! Destructor
ObjectClassAttribute::~ObjectClassAttribute()
{
}

// ----------------------------------------------------------------------------
/*! Throw SecurityError if the Federate is not allowed to access the Object
  Class, and print an Audit message containing Reason.
*/
void
ObjectClassAttribute::checkFederateAccess(FederateHandle fed,
					  const char *reason) const
{
    if (server && !server->canFederateAccessData(fed, level)) {
        cout << "Attribute " << handle << " : SecurityError for federate "
             << fed << '(' << reason << ")." << endl ;
        throw SecurityError("Federate should not access ObjectClassAttribute.");
    }
}

// ----------------------------------------------------------------------------
//! Removes a publishing federate
void
ObjectClassAttribute::deletePublisher(FederateHandle fed)
{
    PublishersList::iterator it = publishers.find(fed);
    if (it != publishers.end())
	publishers.erase(it);
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
    return publishers.find(fed) != publishers.end();
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
	publishers.insert(fed);
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
//! Add all attribute's subscribers to the broadcast list
void
ObjectClassAttribute::updateBroadcastList(ObjectClassBroadcastList *ocblist,
					  const RTIRegion *region)
{
    switch(ocblist->message->getType()) {

      case NetworkMessage::REFLECT_ATTRIBUTE_VALUES: {
	  addFederatesIfOverlap(*ocblist, region, handle);
      } break ;
      case NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION: {
          PublishersList::iterator i ;
          for (i = publishers.begin(); i != publishers.end(); i++) {
              ocblist->addFederate(*i, handle);
          }
      } break ;
      
      default: ; // nothing done
    }
}

} // namespace

// $Id: ObjectClassAttribute.cc,v 3.27.2.1 2008/04/10 14:57:49 erk Exp $
