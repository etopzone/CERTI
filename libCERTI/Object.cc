// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
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
// $Id: Object.cc,v 3.3 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "Object.hh"

namespace certi {

// -------------
// -- Object --
// -------------

Object::Object(FederateHandle theOwner, ObjectName theName)
  : ID(0), Owner(theOwner), UR(0), SF()
{
  setName(theName);
}


// --------------
// -- ~Object --
// --------------

Object::~Object()
{
  FederateHandle *Federate = NULL;

  if(Name != NULL) {
    free(Name);
    Name = NULL;
  }

  while(SF.getLength() > 0) {
    Federate = SF.Ieme(1);
    delete Federate;
    SF.Supprimer(1);
  }
}


// -------------
// -- Display --
// -------------

void Object::display(void)
{
  if(Name != NULL)
    printf("            Instance: ID=%ld, Name=\"%s\"\n", ID, Name);
  else
    printf("            Instance: ID=%ld,(No Name).\n",  ID);
                    
}
// ------------------
// -- GetAttribute --
// ------------------

 ObjectAttribute *Object::getAttribute(AttributeHandle   theAttribute)
throw(AttributeNotDefined)
{

 	for(int k=1;k<=AttributeState.getLength();k++)
		{
			if( AttributeState.Ieme(k)->Handle == theAttribute )
			  return(AttributeState.Ieme(k));
		}

	throw AttributeNotDefined();		
	
		
}

// -------------
// -- GetName --
// -------------

void Object::getName(char *Buffer)
{
  if(Name != NULL)
    strcpy(Buffer, Name);
  else
    Buffer [0] = '\0';
}


// -------------
// -- SetName --
// -------------

void Object::setName(ObjectName theObjectName)
{
  if(Name != NULL)
    free(Name);

  if(theObjectName != NULL) {

    int Length = strlen(theObjectName);

    if(Length > MAX_USER_TAG_LENGTH) {
      printf("Bad Object Name.\n");
      throw RTIinternalError("Object Name too long.");
    }
    else if(Length == 0)
      Name = NULL;
    else
      Name = strdup(theObjectName);
  }
  else
    Name = NULL;
}
    
}

// $Id: Object.cc,v 3.3 2002/12/11 00:47:33 breholee Exp $
