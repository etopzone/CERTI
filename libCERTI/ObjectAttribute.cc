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
// $Id: ObjectAttribute.cc,v 3.0.2.1 2002/11/22 00:52:56 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "ObjectAttribute.hh"

namespace certi {

static pdCDebug D("OBJECTATTRIBUTE", "(Obj_Attr) - ");

// ---------------------------
// -- ObjectAttribute --
// ---------------------------

ObjectAttribute::ObjectAttribute()
 
{
  Handle    					= 0;
  AttributeOwner	    = 0;
	Divesting 					= RTI_FALSE;	
}

// ---------------------------
// -- ObjectAttribute --
// ---------------------------

ObjectAttribute::ObjectAttribute(AttributeHandle NewHandle,
				    FederateHandle NewOwner)

{
  Handle    					= NewHandle;
  AttributeOwner	    = NewOwner;
	Divesting 					= RTI_FALSE;
}



// ---------------------------
// -- ObjectAttribute --
// ---------------------------

ObjectAttribute::~ObjectAttribute()
{

}
  
// -------------
// -- Display --
// -------------

void ObjectAttribute::display(void)
{
  if(Handle != 0)
    printf("Attribute %ld ; Owner %ld \n",Handle, AttributeOwner);
}


// --------------
// -- SetOwner --
// --------------

void ObjectAttribute::setOwner(FederateHandle NewOwner)
  throw(RTIinternalError)
{

	AttributeOwner = NewOwner;
}

// -------------------
// -- BeingDivested --
// -------------------

// Boolean ObjectAttribute::beingDivested()
//   throw(RTIinternalError)
// {
// 	return(Divesting);
// }

// -------------------
// -- SetDivesting --
// -------------------

void ObjectAttribute::setDivesting(Boolean DivestingState)
  throw(RTIinternalError)
{
	Divesting = DivestingState;
}

// -----------------
// -- isCandidate --
// -----------------

int ObjectAttribute::isCandidate(FederateHandle candidate)
  throw(RTIinternalError)
{
	for(int k=1;k <= OwnerCandidate.lg;k++)
		{
		if(getCandidate(k) == candidate)
				return(k);
		}
	return(0);
}

// ------------------
// -- AddCandidate --
// ------------------

void ObjectAttribute::addCandidate(FederateHandle candidate)
  throw(RTIinternalError)
{

	OwnerCandidate.Inserer(1,new FederateHandle(candidate));

}

// ---------------------
// -- DeleteCandidate --
// ---------------------

void ObjectAttribute::removeCandidate(FederateHandle candidate)
  throw(RTIinternalError)
{
	for(int k=1;k <= OwnerCandidate.lg;k++)
		{
		if(getCandidate(k) == candidate)
			{
				OwnerCandidate.Supprimer(k);
				break;
			}
		}
}


// -------------------
// -- GetCandidate --
// -------------------

FederateHandle ObjectAttribute::getCandidate(int indice)
  throw(RTIinternalError)
{
	if((indice <= 0)	||(indice > OwnerCandidate.lg))
			throw RTIinternalError();
	else
			return(*(OwnerCandidate.Ieme(indice)));
}

}

// $Id: ObjectAttribute.cc,v 3.0.2.1 2002/11/22 00:52:56 breholee Exp $
