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
// $Id: FederateLevelList.hh,v 3.2 2003/01/15 18:02:22 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_FEDERATE_LEVEL_LIST_HH
#define _CERTI_FEDERATE_LEVEL_LIST_HH

#include <map>
using std::map;

#include "RTItypes.hh"
#include "SecurityLevel.hh"

namespace certi {

class FederateLevelList
{
protected:
    std::map<FederateName, SecurityLevelID> tuple;

public:
  FederateLevelList(void);
  ~FederateLevelList(void);

  // Register a new federate principal name, and associate the given level
  // with it.
  void addFederate(FederateName theName, SecurityLevelID theLevelID);

  // Try to find the given principal name in the database, and return
  // its associated level. If not found, return PublicLevelID.
  SecurityLevelID getLevel(const char* theName) const;
};
}

#endif // _CERTI_FEDERATE_LEVEL_LIST_HH

// $Id: FederateLevelList.hh,v 3.2 2003/01/15 18:02:22 breholee Exp $
