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
// $Id: FederateLevelList.cc,v 3.3 2003/01/15 18:08:30 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "FederateLevelList.hh"

namespace certi {

//! add a new federate to Tuple.
void 
FederateLevelList::addFederate(FederateName theName,
                               SecurityLevelID theLevelID)
{
    if((theName == NULL) || (strlen(theName) > MAX_FEDERATE_NAME_LENGTH))
        throw RTIinternalError("Federate Name too long(principal name).");

    char *nameCopy = strdup(theName);
    if(nameCopy == NULL)
      throw RTIinternalError("Could not allocate memory.");

    tuple[nameCopy] = theLevelID;
}

//! FederateLevelList constructor.
FederateLevelList::FederateLevelList(void)
{
}

//! Empty map before destroying instance.
FederateLevelList::~FederateLevelList(void)
{
    while (!tuple.empty()) {
        free((*tuple.begin()).first);
        tuple.erase(tuple.begin());
    }
}

// getLevel returns the level id associated the federate name given.
SecurityLevelID 
FederateLevelList::getLevel(const char* theName) const
{
    map<FederateName, SecurityLevelID>::const_iterator i;
    i = tuple.find((char*)theName);

    if (i != tuple.end())
        return (*i).second;

    return PublicLevelID;
}
}

// $Id: FederateLevelList.cc,v 3.3 2003/01/15 18:08:30 breholee Exp $
