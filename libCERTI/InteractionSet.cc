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
// $Id: InteractionSet.cc,v 3.1 2002/11/26 15:48:01 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "InteractionSet.hh"

namespace certi {

static pdCDebug D("INTERACTIONSET", "(InterSet) - ");

// --------------
// -- AddClass --
// --------------

void InteractionSet::addClass(Interaction *theClass)
{
  D.Out(pdInit, "Adding new interaction class %d,", theClass->handle);

  theClass->server = server ;

  // BUG: Verifier si une classe du meme nom n'existe pas deja, avec 
  // GetClassHandle par exemple.
  Inserer(1, theClass);
}


// --------------------------
// -- BroadcastInteraction --
// --------------------------

void InteractionSet::
broadcastInteraction(FederateHandle          theFederateHandle,
		      InteractionClassHandle  theInteractionHandle,
		      ParameterHandle        *theParameterList,
		      ParameterValue         *theValueList,
		      UShort                  theListSize,
		      FederationTime          theTime,
		      const char*          theTag)

  throw(FederateNotPublishing,
	 InteractionClassNotDefined,
	 InteractionParameterNotDefined,
	 RTIinternalError)
{
  Interaction              *theInteraction = NULL;
  InteractionBroadcastList *List           = NULL;
  InteractionClassHandle     CurrentClass   = theInteractionHandle;

  // It may throw InteractionClassNotDefined.
  theInteraction = getByHandle(theInteractionHandle);

  List = theInteraction->sendInteraction(theFederateHandle,
					    theParameterList,
					    theValueList,
					    theListSize,
					    theTime,
					    theTag);
  
  // Pass the Message(and its BroadcastList) to the Parent Classes.
  if(List != NULL) {
    
    CurrentClass = theInteraction->parent;
    
    //while(CurrentClass != 0) {
    //  theInteraction = getByHandle(CurrentClass);
    //  theInteraction->broadcastInteractionMessage(List);
    //  CurrentClass   = theInteraction->Father;
    //}
    
    delete List;
  }
  else 
    // BroadcastInteraction should not be called on the RTIA(see IsReady)
    throw RTIinternalError("BroadcastInteraction called by RTIA.");
}


// -------------------------
// -- BuildParentRelation --
// -------------------------

void InteractionSet::buildParentRelation(Interaction *Child,
					   Interaction *Parent)
{
  InteractionChild *IntSon = NULL;

  // Register Parent to Son
  Child->parent   = Parent->handle;
  
  // Transfert security level
  Child->setLevelId(Parent->getLevelId());

  // Register Son to Parent
  IntSon = new InteractionChild(Child->handle);
  Parent->children.Inserer(1, IntSon);

  // Copy Parent Attribute into Child class.
  Parent->addParametersToChild(Child);

}


// ---------------------
// -- InteractionSet --
// ---------------------

InteractionSet::InteractionSet(SecurityServer *theSecurityServer)
  :   List <Interaction *>()
{
  // It can be NULL on the RTIA
  server = theSecurityServer;
}


// ----------------------
// -- ~InteractionSet --
// ----------------------

InteractionSet::~InteractionSet() 
{
  Interaction *Interaction = NULL;

  while(lg > 0) {
    Interaction = Ieme(1);
    Supprimer(1);
    delete Interaction;
  }
}


// -------------
// -- Display --
// -------------

void InteractionSet::display(void)
{
  int i;
  Interaction *Class = NULL;

  printf("   Interactions :\n");

  for(i = 1; i <= lg; i++) {
    Class = Ieme(i);
    Class->display();
  }

}

// -----------------
// -- GetByHandle --
// -----------------

Interaction *InteractionSet::getByHandle(InteractionClassHandle theHandle)
  throw(InteractionClassNotDefined,
	 RTIinternalError)
{
  int           i;
  Interaction *Interaction = NULL;

  for(i = 1; i <= lg; i++) {
    Interaction = Ieme(i);
    if(Interaction->handle == theHandle)
      return Interaction;
  }

  throw InteractionClassNotDefined();
}


// -------------------------------
// -- GetInteractionClassHandle --
// -------------------------------

InteractionClassHandle InteractionSet::
getInteractionClassHandle(const InteractionClassName theName)
  throw(InteractionClassNotDefined,
	 RTIinternalError)
{
  int           i;
  Interaction *Interaction = NULL;

  if(theName == NULL)
    throw RTIinternalError();

  for(i = 1; i <= lg; i++) {
    Interaction = Ieme(i);
    if(strcmp(Interaction->getName(), theName) == 0)
      return Interaction->handle;
  }

  throw InteractionClassNotDefined();
}


// -----------------------------
// -- GetInteractionClassName --
// -----------------------------

const InteractionClassName InteractionSet::
getInteractionClassName(InteractionClassHandle theHandle)
  throw(InteractionClassNotDefined,
	 RTIinternalError)
{
  int           i;
  Interaction *Interaction = NULL;

  for(i = 1; i <= lg; i++) {
    Interaction = Ieme(i);
    if(Interaction->handle == theHandle)
      return Interaction->getName();
  }

  throw InteractionClassNotDefined();
}


// ------------------------
// -- GetParameterHandle --
// ------------------------

ParameterHandle InteractionSet::
getParameterHandle(const ParameterName    theName,
		    InteractionClassHandle theClass)
  throw(InteractionParameterNotDefined,
	 InteractionClassNotDefined,
	 RTIinternalError)
{
  Interaction *Interaction = NULL;

  if(theName == NULL)
    throw RTIinternalError();

  // It may throw InteractionClassNotDefined
  Interaction = getByHandle(theClass);

  return Interaction->getParameterHandle(theName);
}


// ----------------------
// -- GetParameterName --
// ----------------------

const ParameterName InteractionSet::
getParameterName(ParameterHandle        theHandle,
		  InteractionClassHandle theClass)
  throw(InteractionParameterNotDefined,
	 InteractionClassNotDefined,
	 RTIinternalError)
{
  Interaction *Interaction = NULL;

  // It may throw InteractionClassNotDefined
  Interaction = getByHandle(theClass);

  return Interaction->getParameterName(theHandle);
}


// -------------
// -- IsReady --
// -------------

void InteractionSet::isReady(FederateHandle          theFederateHandle,
			       InteractionClassHandle  theInteraction,
			       ParameterHandle        *paramArray,
			       UShort                  paramArraySize)
  throw(FederateNotPublishing,
	 InteractionClassNotDefined,
	 InteractionParameterNotDefined,
	 RTIinternalError)
{
  Interaction *Interaction = NULL;

  // It may throw InteractionClassNotDefined
  Interaction = getByHandle(theInteraction);

  Interaction->isReady(theFederateHandle,
			  paramArray,
			  paramArraySize);

}


// ------------------
// -- KillFederate --
// ------------------

void InteractionSet::killFederate(FederateHandle theFederate)
  throw()
{
  int           i;
  Interaction *Interaction = NULL;

  for(i = 1; i <= lg; i++) {
    Interaction = Ieme(i);
    Interaction->killFederate(theFederate);
  }
}


// -------------
// -- Publish --
// -------------

void InteractionSet::
publish(FederateHandle          theFederateHandle,
	 InteractionClassHandle  theInteractionHandle,
	 bool                 PubOrUnpub)
  throw(FederateNotPublishing,
	 InteractionClassNotDefined,
	 RTIinternalError,
	 SecurityError)
{
  Interaction *Interaction = NULL;

  // It may throw InteractionClassNotDefined
  Interaction = getByHandle(theInteractionHandle);

  Interaction->publish(PubOrUnpub, theFederateHandle);
}


// ---------------
// -- Subscribe --
// ---------------

void InteractionSet::
subscribe(FederateHandle          theFederateHandle,
	   InteractionClassHandle  theInteractionHandle,
	   bool                SubOrUnsub)
  throw(FederateNotSubscribing,
	 InteractionClassNotDefined,
	 RTIinternalError,
	 SecurityError)
{
  Interaction *Interaction = NULL;

  // It may throw InteractionClassNotDefined
  Interaction = getByHandle(theInteractionHandle);

  Interaction->subscribe(SubOrUnsub, theFederateHandle);
}

}

// $Id: InteractionSet.cc,v 3.1 2002/11/26 15:48:01 breholee Exp $
