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
// $Id: FederateLevelList.hh,v 3.9 2004/03/04 19:50:56 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_FEDERATE_LEVEL_LIST_HH
#define _CERTI_FEDERATE_LEVEL_LIST_HH

#include "SecurityLevel.hh"

#include <map>
#include <string>

namespace certi {

class FederateLevelList
{
protected:
    std::map<std::string, SecurityLevelID> tuple ;

public:
    FederateLevelList();
    ~FederateLevelList();

    void addFederate(const std::string &the_name,
		     SecurityLevelID the_level_id);

    // Try to find the given principal name in the database, and return
    // its associated level. If not found, return PublicLevelID.
    SecurityLevelID getLevel(const std::string &theName) const ;
};

}

#endif // _CERTI_FEDERATE_LEVEL_LIST_HH

// $Id: FederateLevelList.hh,v 3.9 2004/03/04 19:50:56 breholee Exp $
