// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
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
// $Id: ObjectAttribute.cc,v 3.7 2003/04/09 16:41:10 breholee Exp $
// ----------------------------------------------------------------------------

#include "ObjectAttribute.hh"

namespace certi {

static pdCDebug D("OBJECTATTRIBUTE", "(Obj_Attr) - ");

// ----------------------------------------------------------------------------
//! Default constructor (all atttributes to default state).
ObjectAttribute::ObjectAttribute(void)
    : handle(0), owner(0), divesting(RTI_FALSE), space(0)
{
}

// ----------------------------------------------------------------------------
//! Constructor that sets handle and attribute owner.
ObjectAttribute::ObjectAttribute(AttributeHandle newHandle,
                                 FederateHandle newOwner)
    : handle(newHandle), owner(newOwner), divesting(RTI_FALSE), space(0)
{
}

// ----------------------------------------------------------------------------
//! Destructor (nothing to do).
ObjectAttribute::~ObjectAttribute(void)
{
}

// ----------------------------------------------------------------------------
//! Displays the object attribute information.
void
ObjectAttribute::display(void) const
{
    if (handle != 0)
        cout << "Attribute " << handle << " ; Owner " << owner << endl ;
}

// ----------------------------------------------------------------------------
//! Returns the owner
FederateHandle
ObjectAttribute::getOwner(void) const
{
    return owner ;
}

// ----------------------------------------------------------------------------
//! Change the federate owner.
void
ObjectAttribute::setOwner(FederateHandle newOwner)
{
    owner = newOwner ;
}

// ----------------------------------------------------------------------------
//! Returns attribute divesting state.
Boolean
ObjectAttribute::beingDivested(void) const
{
    return(divesting);
}

// ----------------------------------------------------------------------------
//! Put object attribute in divesting state.
void
ObjectAttribute::setDivesting(Boolean divestingState)
{
    divesting = divestingState ;
}

// ----------------------------------------------------------------------------
//! Return the candidate position in list, null otherwise.
int
ObjectAttribute::isCandidate(FederateHandle candidate) const
{
    list<FederateHandle>::const_iterator i = ownerCandidates.begin();
    for (int j = 1 ; i != ownerCandidates.end(); i++, j++) {
        if ((*i) == candidate)
            return j ;
    }

    return 0 ;
}

// ----------------------------------------------------------------------------
//! Add a new candidate to list.
void
ObjectAttribute::addCandidate(FederateHandle candidate)
{
    ownerCandidates.push_front(candidate);
}

// ----------------------------------------------------------------------------
// Removes a candidate from list.
void
ObjectAttribute::removeCandidate(FederateHandle candidate)
{
    ownerCandidates.remove(candidate);
}

// ----------------------------------------------------------------------------
// Returns the federate candidate at position in list.
FederateHandle
ObjectAttribute::getCandidate(int indice) const
    throw (RTIinternalError)
{
    if ((indice <= 0) || (indice > ownerCandidates.size()))
        throw RTIinternalError();

    list<FederateHandle>::const_iterator i = ownerCandidates.begin();
    for (int j = 1 ; i != ownerCandidates.end(); i++, j++) {
        if (j == indice)
            return (*i);
    }
}

// ----------------------------------------------------------------------------
bool
ObjectAttribute::hasCandidates(void) const
{
    return (!ownerCandidates.empty());
}

// ----------------------------------------------------------------------------
AttributeHandle
ObjectAttribute::getHandle(void) const
{
    return handle ;
}

// ----------------------------------------------------------------------------
void
ObjectAttribute::setHandle(AttributeHandle h)
{
    handle = h ;
}

// ----------------------------------------------------------------------------
SpaceHandle
ObjectAttribute::getSpace(void) const
{
    return space ;
}

// ----------------------------------------------------------------------------
void
ObjectAttribute::setSpace(SpaceHandle h)
{
    space = h ;
}


} //namespace certi

// $Id: ObjectAttribute.cc,v 3.7 2003/04/09 16:41:10 breholee Exp $
