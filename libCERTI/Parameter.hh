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
// $Id: Parameter.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

// $Id: Parameter.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------
// Classe Parameter, qui decrit un parametre associe a une classe
// d'interaction.

#ifndef _CERTI_PARAMETER_HH
#define _CERTI_PARAMETER_HH

#include <stdio.h>
#include <cstring>

#include "RTItypes.hh"
#include "SecurityLevel.hh"

namespace certi {

class Parameter 
{
  
  // ATTRIBUTES ------------------------------------------------------------
public:
  ParameterHandle Handle;
  SecurityLevelID LevelID;

private:
  ParameterName Name; // The Name is always locally allocated and deleted.

  // METHODS ---------------------------------------------------------------
public:
  Parameter() { Name = NULL; LevelID = PublicLevelID; };
  Parameter(Parameter *Init) { 
    Name = strdup(Init->Name); 
    LevelID = Init->LevelID; 
  };
 
  Parameter(ParameterName theName) // The 'theName' parameter is
    // copied, it can deleted after
    // the call.
    throw(RTIinternalError);
  ~Parameter() {if(Name != NULL) free(Name); }

  void display();   // Print the parameter's attributes to
  // stdout(see RootObj::Display)

  // Name attribute access(GetName reference must be considered READ-ONLY).
  // NewName lenght must be lower or equal to MAX_USER_TAG_LENGTH.
  char *getName() {return Name; };

  void  setName(char *NewName)
    throw(ValueLengthExceeded, RTIinternalError);

};

}

#endif // _CERTI_PARAMETER_HH

// $Id: Parameter.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $


