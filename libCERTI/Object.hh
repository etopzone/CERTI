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
// $Id: Object.hh,v 3.14 2004/05/18 13:18:53 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_HH
#define _CERTI_OBJECT_HH

#include "certi.hh"
#include "ObjectAttribute.hh"
#include "RegionImp.hh"
#include "Named.hh"
#include "Handled.hh"

#include <deque>
#include <list>

namespace certi {

class Object : public Named, public Handled<ObjectHandle>
{
public:
    Object(FederateHandle the_owner)
        : Owner(the_owner), name(NULL) { handle = 0 ; };

    Object(FederateHandle the_owner, const char *the_name);
    ~Object();

    void display() const ;

    void addAttribute(ObjectAttribute * new_attribute);
    ObjectAttribute *getAttribute(AttributeHandle the_attribute) const
        throw (AttributeNotDefined);

    bool isAttributeOwnedByFederate(FederateHandle, AttributeHandle) const
        throw (AttributeNotDefined, RTIinternalError);

    ObjectClassHandle getClass() const ;
    void setClass(ObjectClassHandle h);

    FederateHandle getOwner() const ;
    void setOwner(FederateHandle);

    void unassociate(RegionImp *);       

    // -----------------------
    // -- Public Attributes --
    // ----------------------
    /*! Owner Handle
      BUG: Should be handled at the attribute level, not instance level.
    */
    FederateHandle Owner ;

private:
    //! Attribute list from object class instance (private).
    std::deque<ObjectAttribute *> attributeState ;

    ObjectHandle handle ; //!< Object Instance ID
    ObjectClassHandle classHandle ; //! Object Class
    ObjectName name ; //!< Instance name.

    /*! federate list subscribed to this class and with subscription region
      intersect with UR.
    */
    std::list<FederateHandle *> sf ;
};

}

#endif // _CERTI_OBJECT_HH

// $Id: Object.hh,v 3.14 2004/05/18 13:18:53 breholee Exp $
