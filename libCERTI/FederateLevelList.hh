// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: FederateLevelList.hh,v 3.5 2003/01/29 18:18:19 breholee Exp $
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
    std::map<char *, SecurityLevelID> tuple;

public:
  FederateLevelList(void);
  ~FederateLevelList(void);

  void addFederate(const char* the_name, SecurityLevelID the_level_id);

  // Try to find the given principal name in the database, and return
  // its associated level. If not found, return PublicLevelID.
  SecurityLevelID getLevel(const char* theName) const;
};
}

#endif // _CERTI_FEDERATE_LEVEL_LIST_HH

// $Id: FederateLevelList.hh,v 3.5 2003/01/29 18:18:19 breholee Exp $
