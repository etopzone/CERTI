// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: Object.cc,v 3.4 2003/01/17 17:54:30 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "Object.hh"

namespace certi {

// ---------------------------------------------------------------------------
//! Constructor.
Object::Object(FederateHandle the_owner, const char* the_name)
    : ID(0), Owner(the_owner), UR(0), SF()
{
    setName(the_name);
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

// ---------------------------------------------------------------------------
//! Copy the Object Name(or '\0' if there is no Name) in the_name.
void Object::getName(ObjectName the_name)
{
    if(Name != NULL)
        strcpy(the_name, Name);
    else
        the_name[0] = '\0';
}

// ---------------------------------------------------------------------------
//! As 'theObjectName' is duplicated, it can be deleted afterwards.
void Object::setName(const char* the_object_name)
{
    if(Name != NULL)
        free(Name);

    if(the_object_name != NULL) {
        int length = strlen(the_object_name);

        if(length > MAX_USER_TAG_LENGTH) {
            cout << "Bad Object Name." << endl;
            throw RTIinternalError("Object Name too long.");
        }
        else if(length == 0)
            Name = NULL;
        else
            Name = strdup(the_object_name);
    }
    else
        Name = NULL;
}
}

// $Id: Object.cc,v 3.4 2003/01/17 17:54:30 breholee Exp $
