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
// $Id: InteractionSet.cc,v 3.3 2003/01/17 17:43:11 breholee Exp $
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

// ---------------------------------------------------------------------------
//! getInteractionClassHandle.
InteractionClassHandle
InteractionSet::getInteractionClassHandle(const char* the_name)
    throw(InteractionClassNotDefined,
          RTIinternalError)
{
    if(the_name == NULL)
        throw RTIinternalError();

    Interaction *interaction = NULL;
    for(int i = 1; i <= lg; i++) {
        interaction = Ieme(i);
        if(strcmp(interaction->getName(), the_name) == 0)
            return interaction->handle;
    }

    throw InteractionClassNotDefined();
}

// ---------------------------------------------------------------------------
//! getInteractionClassName.
const char*
InteractionSet::getInteractionClassName(InteractionClassHandle the_handle)
    throw(InteractionClassNotDefined,
          RTIinternalError)
{
    Interaction *interaction = NULL;

    for(int i = 1; i <= lg; i++) {
        interaction = Ieme(i);
        if(interaction->handle == the_handle)
            return interaction->getName();
    }

    throw InteractionClassNotDefined();
}

// ---------------------------------------------------------------------------
//! getParameterHandle.
ParameterHandle
InteractionSet::getParameterHandle(const char*    the_name,
                                   InteractionClassHandle the_class)
    throw(InteractionParameterNotDefined,
          InteractionClassNotDefined,
          RTIinternalError)
{
    if(the_name == NULL)
        throw RTIinternalError();

    // It may throw InteractionClassNotDefined
    Interaction *interaction = getByHandle(the_class);

    return interaction->getParameterHandle(the_name);
}

// ---------------------------------------------------------------------------
//! getParameterName.
const char*
InteractionSet::getParameterName(ParameterHandle        the_handle,
                                 InteractionClassHandle the_class)
    throw(InteractionParameterNotDefined,
          InteractionClassNotDefined,
          RTIinternalError)
{
  // It may throw InteractionClassNotDefined
    Interaction *interaction = getByHandle(the_class);

    return interaction->getParameterName(the_handle);
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

// $Id: InteractionSet.cc,v 3.3 2003/01/17 17:43:11 breholee Exp $
