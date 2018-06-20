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

#ifndef CERTI_OBJECT_ATTRIBUTE_HH
#define CERTI_OBJECT_ATTRIBUTE_HH

// #include "certi.hh"
#include "Exception.hh"
#include "Handle.hh"

#include <set>

namespace certi {

class RTIRegion;

class ObjectClassAttribute;

//! Object attribute information.
/*! This class maintains information about an attribute:
  - handle,
  - ownerCandidates,
  - current owner,
  - divesting state.
*/
class CERTI_EXPORT ObjectAttribute {
public:
    // Public Methods

    // Constructors & Destructors
    ObjectAttribute(AttributeHandle, FederateHandle, ObjectClassAttribute*);
    ~ObjectAttribute();

    void display() const;

    FederateHandle getOwner() const;
    void setOwner(FederateHandle NewOwner);

    void setDivesting(bool divesting_state);
    bool beingDivested() const;

    bool isCandidate(FederateHandle candidate) const;
    void addCandidate(FederateHandle candidate);
    void removeCandidate(FederateHandle candidate);
    FederateHandle getFirstCandidate() const;
    bool hasCandidates() const;

    AttributeHandle getHandle() const;
    void setHandle(AttributeHandle h);

    SpaceHandle getSpace() const;
    void setSpace(SpaceHandle);

    ObjectClassAttribute* getObjectClassAttribute() const
    {
        return source;
    };

    void associate(RTIRegion*);
    void unassociate(RTIRegion*);

    const RTIRegion* getRegion() const
    {
        return region;
    };

private:
    ObjectAttribute(); //!< Declared by not defined (Don't call it).

    // Private Attributes
    AttributeHandle handle; //!< The object attribute handle.
    FederateHandle owner; //!< Federate who owns the attribute.
    bool divesting; //!< Divesting state.
    std::set<FederateHandle> ownerCandidates; //!< Federates candidate.
    SpaceHandle space; //!< Associated routing space
    ObjectClassAttribute* source; //!< The associated class attribute.
    RTIRegion* region;
};
}

#endif // CERTI_OBJECT_ATTRIBUTE_HH
