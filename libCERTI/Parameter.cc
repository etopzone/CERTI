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
// $Id: Parameter.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "Parameter.hh"

namespace certi {

// ----------------
// -- Parameter --
// ----------------

Parameter::Parameter(ParameterName theName) 
  throw(RTIinternalError)
{
  if(theName != NULL) {
    Name = strdup(theName);
    if(Name == NULL)
      throw RTIinternalError();
  }

  LevelID = PublicLevelID;
}


// -------------
// -- Display --
// -------------

void Parameter::display(void)
{
  if(Name != NULL)
    printf("            Parameter %d: \"%s\" [Level %d]\n",
	    Handle, Name, LevelID);
  else
    printf("            Parameter %d:(no name) [Level %d]\n",
	    Handle, LevelID);
}


// -------------
// -- SetName --
// -------------

void Parameter::setName(char *NewName)
  throw(ValueLengthExceeded, RTIinternalError)
{
  // Check Length
  if((NewName == NULL) ||(std::strlen(NewName) > MAX_USER_TAG_LENGTH)) {
    throw ValueLengthExceeded("Parameter name too long.");
  }

  // Free previous name
  if(Name != NULL)
    free(Name);

  // Store new name
  Name = strdup(NewName);
  if(Name == NULL)
    throw RTIinternalError("Memory Exhausted.");
}

}

// $Id: Parameter.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
