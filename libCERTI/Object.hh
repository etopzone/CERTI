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
// $Id: Object.hh,v 3.10 2003/06/27 17:26:29 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_HH
#define _CERTI_OBJECT_HH

#include "RTItypes.hh"
#include "ObjectAttribute.hh"

#include <deque>
#include <list>

namespace certi {

class Object {

public:

    // -----------------------
    // -- Public Attributes --
    // ----------------------
    /*! Owner Handle
      BUG: Should be handled at the attribute level, not instance level.
    */
    FederateHandle Owner ;

    HLA_Region UR ;

    // --------------------
    // -- Public Methods --
    // --------------------
    Object(FederateHandle the_owner)
        : Owner(the_owner), UR(0), name(NULL) { handle = 0 ; };

    Object(FederateHandle the_owner, const char *the_name);
    ~Object();

    void display() const ;

    void addAttribute(ObjectAttribute * new_attribute);
    ObjectAttribute *getAttribute(AttributeHandle the_attribute) const
        throw (AttributeNotDefined);

    bool isAttributeOwnedByFederate(FederateHandle, AttributeHandle) const
        throw (AttributeNotDefined, RTIinternalError);

    // -------------------------------
    // -- Private Attributes Access --
    // -------------------------------
    void setName(const char *the_object_name);
    void getName(ObjectName the_name) const ;
    const char *getName() const ;

    ObjectHandle getHandle() const ;
    void setHandle(ObjectHandle h);

    FederateHandle getOwner() const ;
    void setOwner(FederateHandle);

    // BUG: Prevoir un jour une methode pour changer SF...

private:

    // ------------------------
    // -- Private Attributes --
    // ------------------------
    //! Attribute list from object class instance (private).
    std::deque<ObjectAttribute *> attributeState ;

    ObjectHandle handle ; //!< Object Instance ID
    ObjectName name ; //!< Instance name.

    /*! federate list subscribed to this class and with subscription region
      intersect with UR.
    */
    std::list<FederateHandle *> sf ;
};

}

#endif // _CERTI_OBJECT_HH

// $Id: Object.hh,v 3.10 2003/06/27 17:26:29 breholee Exp $
