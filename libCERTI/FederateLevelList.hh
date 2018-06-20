// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
//
// This program is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// ----------------------------------------------------------------------------

#ifndef CERTI_FEDERATE_LEVEL_LIST_HH
#define CERTI_FEDERATE_LEVEL_LIST_HH

#include "SecurityLevel.hh"

#include <map>
#include <string>

namespace certi {

class CERTI_EXPORT FederateLevelList {
public:
    FederateLevelList();
    ~FederateLevelList();

    void addFederate(const std::string& the_name, SecurityLevelID the_level_id);

    // Try to find the given principal name in the database, and return
    // its associated level. If not found, return PublicLevelID.
    SecurityLevelID getLevel(const std::string& theName) const;

protected:
    std::map<std::string, SecurityLevelID> tuple;
};

} // namespace certi

#endif // CERTI_FEDERATE_LEVEL_LIST_HH
