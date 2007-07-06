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
// $Id: Object.cc,v 3.20 2007/07/06 09:25:18 erk Exp $
// ----------------------------------------------------------------------------



#include "Object.hh"

#include <iostream>
#include <cstring>
#include <algorithm>

using std::cout ;
using std::endl ;
using std::deque ;
using std::list ;

namespace certi {

// ----------------------------------------------------------------------------
//! Constructor.
Object::Object(FederateHandle the_owner, const char *the_name)
    : Owner(the_owner), handle(0)
{
    setName(the_name);
}

// ----------------------------------------------------------------------------
//! Destructor.
Object::~Object()
{
    if (name != NULL) {
        free(name);
        name = NULL ;
    }

    while (!sf.empty()) {
        delete sf.front();
        sf.pop_front();
    }
}

// ----------------------------------------------------------------------------
//! Display informations about this object (see RootObj::display).
void
Object::display() const
{
    cout << " Instance: handle =" << handle ;

    if (name != NULL)
        cout << ", name=\"" << name << "\"" << endl ;
    else
        cout << ", (No name)." << endl ;
}

// ----------------------------------------------------------------------------
void
Object::addAttribute(ObjectAttribute * new_attribute)
{
    attributeState.push_front(new_attribute);
}

// ----------------------------------------------------------------------------
//! getAttribute.
ObjectAttribute *
Object::getAttribute(AttributeHandle the_attribute) const
    throw (AttributeNotDefined)
{
    deque<ObjectAttribute *>::const_iterator i ;
    for (i = attributeState.begin(); i != attributeState.end(); i++) {
        if ((*i)->getHandle() == the_attribute)
            return (*i);
    }

    throw AttributeNotDefined("");
}

// ----------------------------------------------------------------------------
ObjectClassHandle
Object::getClass() const
{
    return classHandle ;
}

// ----------------------------------------------------------------------------
void
Object::setClass(ObjectClassHandle h)
{
    classHandle = h ;
}

// ----------------------------------------------------------------------------
FederateHandle
Object::getOwner() const
{
    return Owner ;
}

// ----------------------------------------------------------------------------
void
Object::setOwner(FederateHandle the_federate)
{
    Owner = the_federate ;
}

// ----------------------------------------------------------------------------
//! Verify that the attribute owner is federate.
bool
Object::isAttributeOwnedByFederate(FederateHandle the_federate,
                                   AttributeHandle the_attribute) const
    throw (AttributeNotDefined, RTIinternalError)
{
    deque<ObjectAttribute *>::const_iterator i ;
    for (i = attributeState.begin(); i != attributeState.end(); i++) {
        if ((*i)->getHandle() == the_attribute) {
            return (*i)->getOwner() == the_federate ;
        }
    }

    throw AttributeNotDefined("Instance doesn't have this attribute handle");
}

// ----------------------------------------------------------------------------
//! Unassociate attributes from this region
void
Object::unassociate(RTIRegion *region)
{
    deque<ObjectAttribute *>::const_iterator i ;
    for (i = attributeState.begin(); i != attributeState.end(); i++) {
	(*i)->unassociate(region);
    }
}

} // namespace certi

// $Id: Object.cc,v 3.20 2007/07/06 09:25:18 erk Exp $
