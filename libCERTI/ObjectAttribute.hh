// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
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
// $Id: ObjectAttribute.hh,v 3.1 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_ATTRIBUTE_HH
#define _CERTI_OBJECT_ATTRIBUTE_HH

#include <stdio.h>
#include "RTItypes.hh"
#include "List.hh"
#include "PrettyDebug.hh"

namespace certi {

class ObjectAttribute {

public:


  // -----------------------
  // -- Public Attributes --
  // -----------------------

  AttributeHandle Handle;

  List <FederateHandle *> OwnerCandidate;/*FAYET 06.08.01*/
  // liste des demandes d'acquisition de propriété en cours
	
  // --------------------
  // -- Public Methods --
  // --------------------

  // Constructors
  ObjectAttribute();	
	
  ObjectAttribute(AttributeHandle Handle,FederateHandle NewOwner);


  // Destructor 
  ~ObjectAttribute();

  // Display the content of this class(see RootObj::Display)
  void display(void);


  FederateHandle getOwner() {return AttributeOwner; };

  void  setOwner(FederateHandle NewOwner)
    throw(RTIinternalError);
		
  void  setDivesting(Boolean DivestingState)
    throw(RTIinternalError);		
		
  int isCandidate(FederateHandle candidate)
    throw(RTIinternalError);
	
  void addCandidate(FederateHandle candidate)
    throw(RTIinternalError);
	
  void removeCandidate(FederateHandle candidate)
    throw(RTIinternalError);
		
  FederateHandle getCandidate(int indice)
    throw(RTIinternalError);
		
  Boolean beingDivested()
    throw(RTIinternalError)
  {
    return(Divesting);
  };

private:

  // ------------------
  // -- Private Part --
  // ------------------
	
  FederateHandle AttributeOwner;
	
  Boolean Divesting;
	
};

}

#endif // _CERTI_OBJECT_ATTRIBUTE_HH

// $Id: ObjectAttribute.hh,v 3.1 2002/12/11 00:47:33 breholee Exp $
