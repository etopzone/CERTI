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
// $Id: SecurityLevel.hh,v 3.12 2009/11/19 18:15:30 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_SECURITY_LEVEL_HH
#define _CERTI_SECURITY_LEVEL_HH

#include "Exception.hh"
#include <include/certi.hh>

#include <string>

typedef std::string SecurityLevelName;
typedef unsigned short SecurityLevelID;

#define PublicLevelID 0
#define PublicLevelName "Public"

namespace certi {

class SecurityLevel {
public:
    SecurityLevelName Name;
    SecurityLevelID LevelID;

    SecurityLevel(const std::string& InitName, SecurityLevelID InitLevelID)
    {
        if (InitName.empty())
            throw RTIinternalError("Bad Security Level Name.");
        Name = InitName;
        LevelID = InitLevelID;
    }
};
}

#endif // _CERTI_SECURITY_LEVEL_HH

// $Id: SecurityLevel.hh,v 3.12 2009/11/19 18:15:30 erk Exp $
