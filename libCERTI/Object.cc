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
// $Id: Object.cc,v 3.11 2003/06/25 15:06:42 breholee Exp $
// ----------------------------------------------------------------------------

#include "Object.hh"

namespace certi {

// ----------------------------------------------------------------------------
//! Constructor.
Object::Object(FederateHandle the_owner, const char *the_name)
    : Owner(the_owner), UR(0), handle(0)
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

    throw AttributeNotDefined();
}

// ----------------------------------------------------------------------------
//! Copy the Object name(or '\0' if there is no name) in the_name.
void
Object::getName(ObjectName the_name) const
{
    if (name != NULL)
        strcpy(the_name, name);
    else
        the_name[0] = '\0' ;
}

// ----------------------------------------------------------------------------
const char *
Object::getName() const
{
    return name ;
}

// ----------------------------------------------------------------------------
//! As 'theObjectName' is duplicated, it can be deleted afterwards.
void
Object::setName(const char *the_object_name)
{
    if (name != NULL)
        free(name);

    if (the_object_name != NULL) {
        int length = strlen(the_object_name);

        if (length > MAX_USER_TAG_LENGTH) {
            cout << "Bad Object name." << endl ;
            throw RTIinternalError("Object name too long.");
        }
        else if (length == 0)
            name = NULL ;
        else
            name = strdup(the_object_name);
    }
    else
        name = NULL ;
}

// ----------------------------------------------------------------------------
ObjectHandle
Object::getHandle() const
{
    return handle ;
}

// ----------------------------------------------------------------------------
void
Object::setHandle(ObjectHandle h)
{
    handle = h ;
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

} // namespace certi

// $Id: Object.cc,v 3.11 2003/06/25 15:06:42 breholee Exp $
