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
// $Id: ObjectClassAttribute.cc,v 3.3 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "ObjectClassAttribute.hh"

namespace certi {

static pdCDebug D("OBJECTCLASSATTRIBUTE", "(Obj_Attr) - ");
 
// ------------------
// -- AddPublisher --(private)
// ------------------

void ObjectClassAttribute::addPublisher(FederateHandle theFederate)
  throw(RTIinternalError)
{
  Publisher *publisher = new Publisher(theFederate);
  
  if(publisher == NULL) {
    D.Out(pdExcept, "Memory exhausted while publishing attribute.");
    throw RTIinternalError("Memory Exhausted while publishing attribute.");
  }
  
  Publishers.Inserer(1, publisher);
}


// ------------------
// -- AddSubscriber --(private)
// ------------------

void ObjectClassAttribute::addSubscriber(FederateHandle theFederate)
  throw(RTIinternalError)
{
  Subscriber *subscriber = new Subscriber(theFederate);
  
  if(subscriber == NULL) {
    D.Out(pdExcept, "Memory exhausted while subscribing attribute.");
    throw RTIinternalError("Memory Exhausted while subscribing attribute.");
  }

  Subscribers.Inserer(1, subscriber);
}



// -------------------------
// -- CheckFederateAccess --
// -------------------------

void 
ObjectClassAttribute::checkFederateAccess(FederateHandle theFederate,
					  const char *Reason)
  throw(SecurityError)
{
  Boolean Result;

  // BUG: Should at least but a line in Audit
  if(server == NULL)
    return;

  Result = server->canFederateAccessData(theFederate, LevelID);

  // BUG: Should use Audit.
  if(Result != RTI_TRUE) {
    printf("Attribute %ld : SecurityError for federate %ld(%s).\n",
	    Handle, theFederate, Reason);
    throw SecurityError("Federate should not access Object Class Attribute.");
  }
}


// ---------------------------
// -- ObjectClassAttribute --
// ---------------------------

ObjectClassAttribute::ObjectClassAttribute()
  : Subscribers(), Publishers()
{
  Handle    = 0;
  Name      = NULL;
  LevelID   = PublicLevelID;

  Order     = RECEIVE;
  Transport = BEST_EFFORT;

  server = NULL;
}


// ---------------------------
// -- ObjectClassAttribute --
// ---------------------------

ObjectClassAttribute::ObjectClassAttribute(ObjectClassAttribute *Source)
  : Subscribers(), Publishers()
{
  if(Source == NULL)
    throw RTIinternalError("NULL Attribute when copying it.");

  Handle    = Source->Handle;
  LevelID   = Source->LevelID;

  if(Source->Name != NULL)
    Name    = strdup(Source->Name);
  
  Order     = Source->Order;
  Transport = Source->Transport;

  server = Source->server;
}


// ---------------------------
// -- ObjectClassAttribute --
// ---------------------------

ObjectClassAttribute::~ObjectClassAttribute()
{
  Publisher  *Publisher  = NULL;
  Subscriber *Subscriber = NULL;
  
  if(Name != NULL) {
    free(Name);
    Name = NULL;
  }

  // Deleting Publishers
  if(Publishers.getLength() > 0)
    D.Out(pdError, 
	   "Attribute %d: Publishers list not empty at termination.", Handle);
  
  while(Publishers.getLength() > 0) {
    Publisher = Publishers.Ieme(1);
    Publishers.Supprimer(1);
    delete Publisher;
  }
  
  // Deleting Subscribers
  if(Subscribers.getLength() > 0)
    D.Out(pdError, 
	   "Attribute %d: Subscribers list not empty at termination.", Handle);
  
  while(Subscribers.getLength() > 0) {
    Subscriber = Subscribers.Ieme(1);
    Subscribers.Supprimer(1);
    delete Subscriber;
  }
  
}


// ---------------------
// -- DeletePublisher --(private)
// ---------------------

void ObjectClassAttribute::deletePublisher(int PublisherRank)
{
  Publisher *Publisher = Publishers.Ieme(PublisherRank);

  Publishers.Supprimer(PublisherRank);

  delete Publisher;
}


// ----------------------
// -- DeleteSubscriber --(private)
// ----------------------

void ObjectClassAttribute::deleteSubscriber(int SubscriberRank)
{
  Subscriber *Subscriber = Subscribers.Ieme(SubscriberRank);

  Subscribers.Supprimer(SubscriberRank);

  delete Subscriber;
}


// -------------
// -- Display --
// -------------

void ObjectClassAttribute::display(void)
{
  if(Name != NULL)
    printf("            Attribute %ld: \"%s\" [Level %d]\n",    
	    Handle, Name, LevelID);
  else
    printf("            Attribute %ld:(no name) [Level %d]\n",
	    Handle, LevelID);
}

// ------------------
// -- IsPublishing --
// ------------------

Boolean ObjectClassAttribute::IsPublishing(FederateHandle theHandle)
{
  if(getPublisherRank(theHandle) != 0)
    return RTI_TRUE;
  else
    return RTI_FALSE;

}


// ----------------------
// -- GetPublisherRank --(private)
// ----------------------

int ObjectClassAttribute::getPublisherRank(FederateHandle theFederate)
{
  int i;
  Publisher *Publisher;

  for(i = 1; i <= Publishers.getLength(); i++) {
    Publisher = Publishers.Ieme(i);
    if(Publisher->Handle == theFederate)
      return i;
  }

  return 0;
}


// -----------------------
// -- GetSubscriberRank --(private)
// -----------------------

int ObjectClassAttribute::getSubscriberRank(FederateHandle theFederate)
{
  int i;
  Subscriber *Subscriber;

  for(i = 1; i <= Subscribers.getLength(); i++) {
    Subscriber = Subscribers.Ieme(i);
    if(Subscriber->getHandle() == theFederate)
      return i;
  }

  return 0;
}


// -------------------
// -- HasSubscribed --
// -------------------

Boolean ObjectClassAttribute::hasSubscribed(FederateHandle theHandle)
{
  if(getSubscriberRank(theHandle) != 0)
    return RTI_TRUE;
  else
    return RTI_FALSE;
}


// -------------
// -- Publish --
// -------------

void ObjectClassAttribute::publish(FederateHandle theFederate,
				     bool       PubOrUnpub)
  throw(RTIinternalError,
	 SecurityError)
{
  Boolean AlreadyPublishing = IsPublishing(theFederate);

  if((PubOrUnpub == RTI_TRUE) &&(AlreadyPublishing == RTI_FALSE)) {
    // Federate wants to Publish

    // Check Security Levels
    checkFederateAccess(theFederate, "Publish");

    D.Out(pdInit, "Attribute %d: Added Federate %d to publishers list.",
	   Handle, theFederate);
    addPublisher(theFederate);
  }

  else if((PubOrUnpub == RTI_FALSE) &&(AlreadyPublishing == RTI_TRUE)) {
    // Federate wants to unpublish
    D.Out(pdTerm, "Attribute %d: Removed Federate %d from publishers list.",
	   Handle, theFederate);
    deletePublisher(getPublisherRank(theFederate));
  }

  else 
    D.Out(pdError,
	   "Attribute %d: Inconsistent publish request from Federate %d.",
	   Handle, theFederate);

}


// -------------
// -- SetName --
// -------------

void ObjectClassAttribute::setName(char *NewName)
  throw(ValueLengthExceeded, RTIinternalError)
{
  // Check Length
  if((NewName == NULL) ||(strlen(NewName) > MAX_USER_TAG_LENGTH)) {
    D.Out(pdExcept, "Attribute Name %s too long.", NewName);
    throw ValueLengthExceeded("Attribute name too long.");
  }

  // Free previous name
  if(Name != NULL)
    free(Name);

  // Store new name
  Name = strdup(NewName);
  if(Name == NULL)
    throw RTIinternalError("Memory Exhausted.");
}

  
// ---------------
// -- Subscribe --
// ---------------

void ObjectClassAttribute::subscribe(FederateHandle theFederate,
				       bool        SubOrUnsub)
  throw(RTIinternalError,
	 SecurityError)
{
  Boolean AlreadySubscribed = hasSubscribed(theFederate);

  if((SubOrUnsub == RTI_TRUE) &&(AlreadySubscribed == RTI_FALSE)) {
    // Federate wants to Subscribe

    // Check Security Levels
    checkFederateAccess(theFederate, "Subscribe");

    addSubscriber(theFederate);
    D.Out(pdInit, "Attribute %d: Added Federate %d to subscribers list.",
	   Handle, theFederate);
  }

  else if((SubOrUnsub == RTI_FALSE) &&(AlreadySubscribed == RTI_TRUE)) {
    // Federate wants to unsubscribe
    deleteSubscriber(getSubscriberRank(theFederate));
    D.Out(pdTerm, "Attribute %d: Removed Federate %d from subscribers list.",
	   Handle, theFederate);
  }

  else 
    D.Out(pdError, 
	   "Attribute %d: Unconsistent subscribe request from federate %d.",
	   Handle, theFederate);

}


// -------------------------
// -- UpdateBroadcastList --
// -------------------------

void ObjectClassAttribute::updateBroadcastList(ObjectClassBroadcastList *List)
{
  int          SubIndex   = 0;
  Subscriber *Subscriber = NULL;
  int          PubIndex   = 0;
  Publisher *Publisher = NULL;	

	switch(List->Message->Type) {
	
	case m_REFLECT_ATTRIBUTE_VALUES:

  for(SubIndex = 1; SubIndex <= Subscribers.getLength(); SubIndex++) {
    Subscriber = Subscribers.Ieme(SubIndex);
    List->addFederate(Subscriber->getHandle(),
			 Handle); // Attribute handle
  }
	break;
	
	case m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:

  for(PubIndex = 1; PubIndex <= Publishers.getLength(); PubIndex++) {
    Publisher = Publishers.Ieme(PubIndex);
    List->addFederate(Publisher->Handle,
			 Handle); // Attribute handle
  }
	break;

	default: ;
	  // on ne fait rien
	}
}

}

// $Id: ObjectClassAttribute.cc,v 3.3 2002/12/11 00:47:33 breholee Exp $
