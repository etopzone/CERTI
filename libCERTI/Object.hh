// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libcerti
//
// CERTI-libcerti is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libcerti is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: Object.hh,v 3.1 2002/11/26 15:48:01 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_HH
#define _CERTI_OBJECT_HH

#include <cstring>
#include <stdio.h>

#include "RTItypes.hh"
#include "List.hh"
#include "ObjectAttribute.hh"

namespace certi {

class Object {

public:

  // -----------------------
  // -- Public Attributes --
  // ----------------------

  // Object Instance ID
  ObjectHandle ID;

  // Owner Handle
  // BUG: Should be handled at the attribute level, not instance level.
  FederateHandle Owner;

  List <ObjectAttribute *> AttributeState;/*FAYET 06.08.01*/
  // liste des attributs de l'instance de la classe d'objet

  HLA_Region UR;

  // --------------------
  // -- Public Methods --
  // --------------------

  // Constructor(on est oblige de mettre un corps sinon ca compile pas)
  Object(FederateHandle theOwner)
    : Owner(theOwner), UR(0), Name(NULL), SF() { ID = 0; } ;

  Object(FederateHandle theOwner, ObjectName theName);

  // Destructor
  ~Object();

  // Display informations about this object(see RootObj::Display)
  void display(void);
 
  // ------------------
  // -- GetAttribute --
  // ------------------

  ObjectAttribute *getAttribute(AttributeHandle theAttribute)
    throw(AttributeNotDefined);

  // -------------------------------
  // -- Private Attributes Access --
  // -------------------------------

  // As 'theObjectName' is duplicated, it can be deleted afterwards.
  void setName(ObjectName theObjectName);

  // Copy the Object Name(or '\0' if there is no Name) in Buffer.
  // Buffer must be at least 
  void getName(char *Buffer);

  // BUG: Prevoir un jour une methode pour changer SF...

private:

  // ------------------------
  // -- Private Attributes --
  // ------------------------

  ObjectName Name;
  // nom de l'instance
 
  List <FederateHandle *> SF;
  // ensemble des federes abonnes a cette classe et dont la region de
  // souscription a une intersection non nulle avec UR.
};

}

#endif // _CERTI_OBJECT_HH

// $Id: Object.hh,v 3.1 2002/11/26 15:48:01 breholee Exp $
