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

#include "Object.hh"
#include "ObjectAttribute.hh"
#include "RTIRegion.hh"

#include <iostream>
#include <sstream>

using std::cout;
using std::endl;

namespace certi {

// ----------------------------------------------------------------------------
//! Constructor.
Object::Object(FederateHandle the_owner) : Owner(the_owner)
{
}

// ----------------------------------------------------------------------------
//! Destructor.
Object::~Object()
{
    // We should delete the pointee because it belongs to the object.
    AttributeMap::const_iterator i;
    for (i = _attributeMap.begin(); i != _attributeMap.end(); ++i) {
        delete i->second;
    }
}

// ----------------------------------------------------------------------------
//! Display informations about this object (see RootObj::display).
void Object::display() const
{
    cout << " Instance: handle =" << handle;

    if (!name.empty()) {
        cout << ", name=\"" << name << "\"" << endl;
    }
    else {
        cout << ", (No name)." << endl;
    }

    cout << " Attributes: " << _attributeMap.size() << endl;
    for (const auto& pair : _attributeMap) {
        cout << "Attribute #" << pair.first << endl;
        pair.second->display();
    }
}

// ----------------------------------------------------------------------------
void Object::addAttribute(ObjectAttribute* new_attribute)
{
    AttributeHandle attributeHandle = new_attribute->getHandle();
    if (_attributeMap.find(attributeHandle) != _attributeMap.end())
        throw RTIinternalError("Attribute already defined");
    _attributeMap[attributeHandle] = new_attribute;
}

// ----------------------------------------------------------------------------
//! getAttribute.
ObjectAttribute* Object::getAttribute(AttributeHandle attributeHandle) const
{
    AttributeMap::const_iterator i = _attributeMap.find(attributeHandle);
    if (i == _attributeMap.end()) {
        throw AttributeNotDefined(
            "Object::getAttribute(AttributeHandle) Unknown attribute handle <" + std::to_string(attributeHandle) + ">");
    }
    return i->second;
}

// ----------------------------------------------------------------------------
void Object::setClass(ObjectClassHandle h)
{
    classHandle = h;
}

// ----------------------------------------------------------------------------
void Object::setOwner(FederateHandle the_federate)
{
    Owner = the_federate;
}

// ----------------------------------------------------------------------------
//! Verify that the attribute owner is federate.
bool Object::isAttributeOwnedByFederate(FederateHandle the_federate, AttributeHandle the_attribute) const
{
    return getAttribute(the_attribute)->getOwner() == the_federate;
}

// ----------------------------------------------------------------------------
//! Unassociate attributes from this region
void Object::unassociate(RTIRegion* region)
{
    AttributeMap::const_iterator i;
    for (i = _attributeMap.begin(); i != _attributeMap.end(); ++i) {
        i->second->unassociate(region);
    }
}

// ----------------------------------------------------------------------------
//! Remove references for killed federate from all attributes in object
void Object::killFederate(FederateHandle the_federate)
{
    AttributeMap::const_iterator i;
    for (i = _attributeMap.begin(); i != _attributeMap.end(); ++i) {
        if (i->second->getOwner() == the_federate)
            i->second->setOwner(0);
    }
}

} // namespace certi
