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
// $Id: Parameter.cc,v 3.4 2003/06/27 17:26:29 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Parameter.hh"

#include <stdio.h>
#include <cstring>

namespace certi {

// ----------------
// -- Parameter --
// ----------------

Parameter::Parameter(ParameterName theName)
    throw (RTIinternalError)
{
    if (theName != NULL) {
        Name = strdup(theName);
        if (Name == NULL)
            throw RTIinternalError();
    }

    LevelID = PublicLevelID ;
}


// -------------
// -- Display --
// -------------

void Parameter::display(void)
{
    if (Name != NULL)
        printf(" Parameter %ld: \"%s\"[Level %d]\n",
               Handle, Name, LevelID);
    else
        printf(" Parameter %ld:(no name)[Level %d]\n",
               Handle, LevelID);
}


// -------------
// -- SetName --
// -------------

void Parameter::setName(char *NewName)
    throw (ValueLengthExceeded, RTIinternalError)
{
    // Check Length
    if ((NewName == NULL) || (std::strlen(NewName) > MAX_USER_TAG_LENGTH)) {
        throw ValueLengthExceeded("Parameter name too long.");
    }

    // Free previous name
    if (Name != NULL)
        free(Name);

    // Store new name
    Name = strdup(NewName);
    if (Name == NULL)
        throw RTIinternalError("Memory Exhausted.");
}

}

// $Id: Parameter.cc,v 3.4 2003/06/27 17:26:29 breholee Exp $
