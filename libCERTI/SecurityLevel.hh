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
// $Id: SecurityLevel.hh,v 3.5 2003/06/27 17:26:29 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_SECURITY_LEVEL_HH
#define _CERTI_SECURITY_LEVEL_HH

#include "RTItypes.hh"

#include <stdlib.h>
#include <string.h>

typedef char *SecurityLevelName ; // Max length MAX_USER_TAG_LENGTH
typedef unsigned short SecurityLevelID ;

#define PublicLevelID 0
#define PublicLevelName "Public"

namespace certi {

class SecurityLevel
{
public:
    SecurityLevelName Name ;
    SecurityLevelID LevelID ;

    SecurityLevel(const char *InitName,
                  SecurityLevelID InitLevelID)
    {
        if ((InitName == NULL) || (strlen(InitName) > MAX_USER_TAG_LENGTH))
            throw RTIinternalError("Bad Security Level Name.");
        Name = strdup(InitName);
        LevelID = InitLevelID ;
    }

    ~SecurityLevel() { if (Name != NULL) free(Name); };
};

}

#endif // _CERTI_SECURITY_LEVEL_HH

// $Id: SecurityLevel.hh,v 3.5 2003/06/27 17:26:29 breholee Exp $
