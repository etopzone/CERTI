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
// $Id: Object.hh,v 3.22 2009/10/11 17:04:17 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_HH
#define _CERTI_OBJECT_HH

// forward declaration
namespace certi {
class ObjectAttribute;
class RTIRegion;
}

#include "Exception.hh"
#include "Handled.hh"
#include "Named.hh"
#include <include/certi.hh>

#include <map>

namespace certi {

class CERTI_EXPORT Object : public Named, public Handled<ObjectHandle> {
public:
    Object(FederateHandle the_owner);
    virtual ~Object();

    void display() const;

    void addAttribute(ObjectAttribute* new_attribute);
    ObjectAttribute* getAttribute(AttributeHandle the_attribute) const;

    bool isAttributeOwnedByFederate(FederateHandle, AttributeHandle) const;

    ObjectClassHandle getClass() const
    {
        return classHandle;
    }
    void setClass(ObjectClassHandle h);

    FederateHandle getOwner() const
    {
        return Owner;
    }
    void setOwner(FederateHandle);

    void unassociate(RTIRegion*);

    void killFederate(FederateHandle);

private:
    /*! Owner Handle
      BUG: Should be handled at the attribute level, not instance level.
    */
    FederateHandle Owner;

    typedef std::map<AttributeHandle, ObjectAttribute*> AttributeMap;
    //! Attribute list from object class instance.
    AttributeMap _attributeMap;

    ObjectClassHandle classHandle; //! Object Class
};
}

#endif // _CERTI_OBJECT_HH

// $Id: Object.hh,v 3.22 2009/10/11 17:04:17 erk Exp $
