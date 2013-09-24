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
// ----------------------------------------------------------------------------



#include "ObjectAttribute.hh"
#include "RTIRegion.hh"
#include "PrettyDebug.hh"

#include <iostream>

using std::cout ;
using std::endl ;

namespace certi {

static PrettyDebug D("OBJECTATTRIBUTE", "(Obj_Attr) - ");

// ----------------------------------------------------------------------------
//! Constructor that sets handle and attribute owner.
ObjectAttribute::ObjectAttribute(AttributeHandle new_handle,
                                 FederateHandle new_owner,
                                 ObjectClassAttribute *associated_attribute)
    : handle(new_handle), owner(new_owner), divesting(false), space(0),
      source(associated_attribute), region(0)
{
}

// ----------------------------------------------------------------------------
//! Destructor (nothing to do).
ObjectAttribute::~ObjectAttribute()
{
}

// ----------------------------------------------------------------------------
//! Displays the object attribute information.
void
ObjectAttribute::display() const
{
    if (handle != 0)
        cout << "Attribute " << handle << " ; Owner " << owner << endl ;
}

// ----------------------------------------------------------------------------
//! Returns the owner
FederateHandle
ObjectAttribute::getOwner() const
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
bool
ObjectAttribute::beingDivested() const
{
    return divesting ;
}

// ----------------------------------------------------------------------------
//! Put object attribute in divesting state.
void
ObjectAttribute::setDivesting(bool divestingState)
{
    divesting = divestingState ;
}

// ----------------------------------------------------------------------------
//! Return the candidate position in list, null otherwise.
bool
ObjectAttribute::isCandidate(FederateHandle candidate) const
{
    return ownerCandidates.find(candidate) != ownerCandidates.end();
}

// ----------------------------------------------------------------------------
//! Add a new candidate to list.
void
ObjectAttribute::addCandidate(FederateHandle candidate)
{
    ownerCandidates.insert(candidate);
}

// ----------------------------------------------------------------------------
// Removes a candidate from list.
void
ObjectAttribute::removeCandidate(FederateHandle candidate)
{
    ownerCandidates.erase(candidate);
}

// ----------------------------------------------------------------------------
// Returns the federate candidate at position in list.
FederateHandle
ObjectAttribute::getFirstCandidate() const
    throw (RTIinternalError)
{
    if (ownerCandidates.empty())
        throw RTIinternalError("");

    return *ownerCandidates.begin();
}

// ----------------------------------------------------------------------------
bool
ObjectAttribute::hasCandidates() const
{
    return !ownerCandidates.empty();
}

// ----------------------------------------------------------------------------
AttributeHandle
ObjectAttribute::getHandle() const
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
ObjectAttribute::getSpace() const
{
    return space ;
}

// ----------------------------------------------------------------------------
void
ObjectAttribute::setSpace(SpaceHandle h)
{
    space = h ;
}

// ----------------------------------------------------------------------------
/* Associate this attribute with a DDM region
 */
void
ObjectAttribute::associate(RTIRegion *r)
{
    region = r ;
}

// ----------------------------------------------------------------------------
/* Unassociate attribute from the provided region. If the attribute is
   associated with a different region, nothing is changed.
 */
void
ObjectAttribute::unassociate(RTIRegion *r)
{
    if (region == r)
	region = 0 ;
}

} //namespace certi
