// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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
// ----------------------------------------------------------------------------

#include "ObjectClassAttribute.hh"
#include "ObjectClassBroadcastList.hh"
#include "PrettyDebug.hh"
#include "SecurityServer.hh"

#include <iostream>

using std::cout;
using std::endl;

namespace certi {

static PrettyDebug D("OBJECTCLASSATTRIBUTE", "(Obj.Cl.Attr) ");

// ----------------------------------------------------------------------------
//! Constructor : Set const attribute handle and name
ObjectClassAttribute::ObjectClassAttribute(const std::string& name, AttributeHandle attributeHandle)
    : Subscribable(name), level(PublicLevelID), order(0), transport(0), server(0), handle(attributeHandle), space(0)
{
}

// ----------------------------------------------------------------------------
//! Constructor : Copy Handle, Name, Space, Order and Transport
ObjectClassAttribute::ObjectClassAttribute(const ObjectClassAttribute& objectClassAttribute)
    : Subscribable(objectClassAttribute.getName())
    , level(objectClassAttribute.level)
    , order(objectClassAttribute.order)
    , transport(objectClassAttribute.transport)
    , server(objectClassAttribute.server)
    , handle(objectClassAttribute.handle)
    , space(objectClassAttribute.space)
{
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
void ObjectClassAttribute::checkFederateAccess(FederateHandle fed, const std::string& reason) const
{
    if (server && !server->canFederateAccessData(fed, level)) {
        cout << "Attribute " << handle << " : SecurityError for federate " << fed << '(' << reason << ")." << endl;
        throw SecurityError("Federate should not access ObjectClassAttribute.");
    }
}

// ----------------------------------------------------------------------------
//! Removes a publishing federate
void ObjectClassAttribute::deletePublisher(FederateHandle fed)
{
    PublishersList_t::iterator it = publishers.find(fed);
    if (it != publishers.end()) {
        publishers.erase(it);
    }
}

// ----------------------------------------------------------------------------
//! Displays the attribute information (handle, name and level id)
void ObjectClassAttribute::display() const
{
    cout << " Attribute " << handle << ':';

    if (name.length() > 0)
        cout << '\"' << name << '\"';
    else
        cout << "(no name)";

    cout << "[Level " << level << ']' << endl;
}

// ----------------------------------------------------------------------------
//! returns true if federate is publishing this attribute
bool ObjectClassAttribute::isPublishing(FederateHandle fed) const
{
    return (publishers.find(fed)) != publishers.end();
}

// ----------------------------------------------------------------------------
//! publish
void ObjectClassAttribute::publish(FederateHandle fed)
{
    if (!isPublishing(fed)) {
        checkFederateAccess(fed, "Publish");
        Debug(D, pdInit) << "Attribute " << handle << ": Added Federate " << fed << " to publishers list." << endl;
        publishers.insert(fed);
    }
    else {
        Debug(D, pdError) << "Attribute " << handle << ": Inconsistent publish request from Federate " << fed << endl;
    }
} /* end of publish */

// ----------------------------------------------------------------------------
//! unpublish
void ObjectClassAttribute::unpublish(FederateHandle fed)
{
    if (isPublishing(fed)) {
        Debug(D, pdTerm) << "Attribute " << handle << ": Removed Federate " << fed << " from publishers list." << endl;
        deletePublisher(fed);
    }
    else {
        Debug(D, pdError) << "Attribute " << handle << ": Inconsistent publish request from Federate " << fed << endl;
    }
}

// ----------------------------------------------------------------------------
AttributeHandle ObjectClassAttribute::getHandle() const
{
    return handle;
}

// ----------------------------------------------------------------------------
void ObjectClassAttribute::setSpace(SpaceHandle h)
{
    space = h;
}

// ----------------------------------------------------------------------------
SpaceHandle ObjectClassAttribute::getSpace() const
{
    return space;
}

// ----------------------------------------------------------------------------
//! Add all attribute's subscribers to the broadcast list
void ObjectClassAttribute::updateBroadcastList(ObjectClassBroadcastList* ocblist, const RTIRegion* region)
{
    switch (ocblist->getMsg().getMessageType()) {
    case NetworkMessage::Type::REFLECT_ATTRIBUTE_VALUES: {
        addFederatesIfOverlap(*ocblist, region, handle);
    } break;
    case NetworkMessage::Type::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION: {
        PublishersList_t::iterator i;
        for (i = publishers.begin(); i != publishers.end(); ++i) {
            ocblist->addFederate(*i, handle);
        }
    } break;

    default:; // nothing done
    }
}

} // namespace
