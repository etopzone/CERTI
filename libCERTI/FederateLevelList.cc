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
// $Id: FederateLevelList.cc,v 3.6 2003/02/19 18:07:29 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>

#include "FederateLevelList.hh"

namespace certi {

// ----------------------------------------------------------------------------
//! add a new federate to Tuple.
/*! Register a new federate principal name, and associate the given level
  with it.
*/
void
FederateLevelList::addFederate(const char *the_name,
                               SecurityLevelID the_level_id)
{
    if ((the_name == NULL) || (strlen(the_name) > MAX_FEDERATE_NAME_LENGTH))
        throw RTIinternalError("Federate Name too long(principal name).");

    char *nameCopy = strdup(the_name);
    if (nameCopy == NULL)
        throw RTIinternalError("Could not allocate memory.");

    tuple[nameCopy] = the_level_id ;
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
FederateLevelList::getLevel(const char *theName) const
{
    map<char *, SecurityLevelID>::const_iterator i ;
    i = tuple.find((char *)theName);

    if (i != tuple.end())
        return (*i).second ;

    return PublicLevelID ;
}

}

// $Id: FederateLevelList.cc,v 3.6 2003/02/19 18:07:29 breholee Exp $
