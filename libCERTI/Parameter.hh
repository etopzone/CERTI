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
// $Id: Parameter.hh,v 3.4 2003/10/27 10:16:36 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_PARAMETER_HH
#define _CERTI_PARAMETER_HH

#include "RTItypes.hh"
#include "SecurityLevel.hh"

namespace certi {

class Parameter
{
public:
    SecurityLevelID LevelID ;

private:
    ParameterHandle Handle ;
    ParameterName Name ; // The Name is always locally allocated and deleted.

public:
    Parameter() { Name = NULL ; LevelID = PublicLevelID ; };
    Parameter(Parameter *Init) {
        Name = strdup(Init->Name);
        LevelID = Init->LevelID ;
    };

    Parameter(ParameterName theName) // The 'theName' parameter is
        // copied, it can deleted after
        // the call.
        throw (RTIinternalError);
    ~Parameter() {if (Name != NULL) free(Name); }

    void display(); // Print the parameter's attributes to
    // stdout(see RootObj::Display)

    // Name attribute access(GetName reference must be considered READ-ONLY).
    // NewName lenght must be lower or equal to MAX_USER_TAG_LENGTH.
    char *getName() {return Name ; };

    void setName(const char *NewName)
        throw (ValueLengthExceeded, RTIinternalError);

    void setHandle(ParameterHandle p) { Handle = p ; }
    ParameterHandle getHandle() const { return Handle ; }
};

}

#endif // _CERTI_PARAMETER_HH

// $Id: Parameter.hh,v 3.4 2003/10/27 10:16:36 breholee Exp $
