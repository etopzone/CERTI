// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: Object.cc,v 3.5 2003/01/20 21:49:15 breholee Exp $
// ---------------------------------------------------------------------------

#include "Object.hh"

namespace certi {

// ---------------------------------------------------------------------------
//! Constructor.
Object::Object(FederateHandle the_owner, const char* the_name)
    : ID(0), Owner(the_owner), UR(0)
{
    setName(the_name);
}

// ---------------------------------------------------------------------------
//! Destructor.
Object::~Object(void)
{
    if (Name != NULL) {
        free(Name);
        Name = NULL;
    }

    while (!sf.empty()) {
        delete sf.front();
        sf.pop_front();
    }
}

// ---------------------------------------------------------------------------
//! Display informations about this object (see RootObj::display).
void
Object::display(void) const
{
    cout << " Instance: ID=" << ID;

    if (Name != NULL)
        cout << ", Name=\"" << Name << "\"" << endl;
    else
        cout << ",(No Name)." << endl;
}

// ---------------------------------------------------------------------------
//! getAttribute.
ObjectAttribute *
Object::getAttribute(AttributeHandle the_attribute) const
    throw(AttributeNotDefined)
{
    deque<ObjectAttribute *>::const_iterator i ;
    for (i = attributeState.begin(); i != attributeState.end() ; i++) {
        if ((*i)->Handle == the_attribute)
            return (*i);
    }

    throw AttributeNotDefined();
}

// ---------------------------------------------------------------------------
//! Copy the Object Name(or '\0' if there is no Name) in the_name.
void
Object::getName(ObjectName the_name) const
{
    if (Name != NULL)
        strcpy(the_name, Name);
    else
        the_name[0] = '\0';
}

// ---------------------------------------------------------------------------
//! As 'theObjectName' is duplicated, it can be deleted afterwards.
void
Object::setName(const char* the_object_name)
{
    if (Name != NULL)
        free(Name);

    if (the_object_name != NULL) {
        int length = strlen(the_object_name);

        if (length > MAX_USER_TAG_LENGTH) {
            cout << "Bad Object Name." << endl;
            throw RTIinternalError("Object Name too long.");
        }
        else if (length == 0)
            Name = NULL;
        else
            Name = strdup(the_object_name);
    }
    else
        Name = NULL;
}

} // namespace certi

// $Id: Object.cc,v 3.5 2003/01/20 21:49:15 breholee Exp $
