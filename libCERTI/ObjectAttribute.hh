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
// $Id: ObjectAttribute.hh,v 3.5 2003/04/09 16:41:10 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_ATTRIBUTE_HH
#define _CERTI_OBJECT_ATTRIBUTE_HH

#include <config.h>
#include "RTItypes.hh"
#include "PrettyDebug.hh"

#include <iostream>
#include <list>

using std::cout ;
using std::endl ;
using std::list ;

namespace certi {

//! Object attribute information.
/*! This class maintains information about an attribute:
  - handle,
  - ownerCandidates,
  - current owner,
  - divesting state.
*/
class ObjectAttribute {

public:
    // Public Methods

    // Constructors & Destructors
    ObjectAttribute(void);
    ObjectAttribute(AttributeHandle new_handle, FederateHandle new_owner);
    ~ObjectAttribute(void);

    void display(void) const ;

    FederateHandle getOwner(void) const ;
    void setOwner(FederateHandle NewOwner);

    void setDivesting(Boolean divesting_state);
    Boolean beingDivested(void) const ;

    int isCandidate(FederateHandle candidate) const ;
    void addCandidate(FederateHandle candidate);
    void removeCandidate(FederateHandle candidate);
    FederateHandle getCandidate(int rank) const
        throw (RTIinternalError);
    bool hasCandidates(void) const ;

    AttributeHandle getHandle(void) const ;
    void setHandle(AttributeHandle h);

    SpaceHandle getSpace(void) const ;
    void setSpace(SpaceHandle);

private:
    // Private Attributes
    AttributeHandle handle ; //!< The object attribute handle.
    FederateHandle owner ; //!< Federate who owns the attribute.
    Boolean divesting ; //!< Divesting state.
    list<FederateHandle> ownerCandidates ; //!< Federates candidate.
    SpaceHandle space ; //!< Associated routing space
};

}

#endif // _CERTI_OBJECT_ATTRIBUTE_HH

// $Id: ObjectAttribute.hh,v 3.5 2003/04/09 16:41:10 breholee Exp $
