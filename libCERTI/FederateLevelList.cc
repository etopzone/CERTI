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
// $Id: FederateLevelList.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "FederateLevelList.hh"

namespace certi {

  // -----------------
  // -- addFederate --
  // -----------------

  void 
  FederateLevelList::addFederate(FederateName theName,
				 SecurityLevelID theLevelID)
  {
    FederateLevelTuple *Tuple = new FederateLevelTuple(theName, theLevelID);
    Inserer(1, Tuple);
  }


  // ------------------------
  // -- FederateLevelList --
  // ------------------------

  FederateLevelList::FederateLevelList()
    : List <FederateLevelTuple *>()
  {
  }


  // -------------------------
  // -- ~FederateLevelList --
  // -------------------------

  FederateLevelList::~FederateLevelList()
  {
    FederateLevelTuple *Tuple;

    while(lg > 0) {
      Tuple = Ieme(1);
      Supprimer(1);
      delete Tuple;
    }
  }


  // --------------------
  // -- FederateLevelTuple --
  // --------------------

  FederateLevelTuple::FederateLevelTuple(FederateName theName,
					 SecurityLevelID theLevelID)
  {
    if((theName == NULL) ||(strlen(theName) > MAX_FEDERATE_NAME_LENGTH))
      throw RTIinternalError("Federate Name too long(principal name).");

    name = strdup(theName);

    if(name == NULL)
      throw RTIinternalError("Could not allocate memory.");

    id = theLevelID;
  }


  // ---------------------
  // -- ~FederateLevelTuple --
  // ---------------------

  FederateLevelTuple::~FederateLevelTuple()
  {
    if(name != NULL)
      free(name);

    name = NULL;
    id = PublicLevelID;
  }


  // ----------------------
  // -- getLevel --
  // ----------------------

  SecurityLevelID 
  FederateLevelList::getLevel(const char *theName)
  {
    int i;
    FederateLevelTuple *tuple;

    for(i = 1; i <= lg; i++) {
      tuple = Ieme(i);
      if(strcmp(theName, tuple->name) == 0) {
	return tuple->id;
      }
    }
    return PublicLevelID;
  }
}

// $Id: FederateLevelList.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
