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
// $Id: ObjectClassAttribute.cc,v 3.5 2003/01/15 14:31:43 breholee Exp $
// ---------------------------------------------------------------------------

#include "ObjectClassAttribute.hh"

namespace certi {

static pdCDebug D("OBJECTCLASSATTRIBUTE", "(Obj_Attr) - ");

// ---------------------------------------------------------------------------
//! Add a publisher to the list of publishing federates (private module).
void
ObjectClassAttribute::addPublisher(FederateHandle theFederate)
  throw(RTIinternalError)
{
  Publisher *publisher = new Publisher(theFederate);
  
  if(publisher == NULL) {
    D.Out(pdExcept, "Memory exhausted while publishing attribute.");
    throw RTIinternalError("Memory Exhausted while publishing attribute.");
  }
  
  publishers.push_front(publisher);
}

// ---------------------------------------------------------------------------
//! Add a subscriber to the list of subscribed federates (private module).
void
ObjectClassAttribute::addSubscriber(FederateHandle theFederate)
  throw(RTIinternalError)
{
  Subscriber *subscriber = new Subscriber(theFederate);
  
  if(subscriber == NULL) {
    D.Out(pdExcept, "Memory exhausted while subscribing attribute.");
    throw RTIinternalError("Memory Exhausted while subscribing attribute.");
  }

  subscribers.push_front(subscriber);
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
      cout << "Attribute " << Handle << " : SecurityError for federate "
           << theFederate << '(' << Reason << ")." << endl;
    throw SecurityError("Federate should not access Object Class Attribute.");
  }
}

// ---------------------------------------------------------------------------
//! No parameters constructor.
/*! This constructor initialize the attribute with default parameters.
 */
ObjectClassAttribute::ObjectClassAttribute(void)
    : Handle(0), LevelID(PublicLevelID), Order(RECEIVE), Transport(BEST_EFFORT)
{
    Name = 0 ;
    server = 0 ;
}


// ---------------------------
// -- ObjectClassAttribute --
// ---------------------------

ObjectClassAttribute::ObjectClassAttribute(ObjectClassAttribute *Source)
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

ObjectClassAttribute::~ObjectClassAttribute(void)
{
  if(Name != NULL) {
    free(Name);
    Name = NULL;
  }

  // Deleting Publishers
  if(!publishers.empty())
    D.Out(pdError, 
	   "Attribute %d: Publishers list not empty at termination.", Handle);

  list<Publisher *>::iterator p ;
  for (p = publishers.begin(); p != publishers.end() ; p++) {
      delete (*p);
  }
  publishers.clear();

  // Deleting Subscribers
  if (!subscribers.empty())
    D.Out(pdError, 
	   "Attribute %d: Subscribers list not empty at termination.", Handle);
  
  list<Subscriber *>::iterator s ; 
  for (s = subscribers.begin(); s != subscribers.end() ; s++) {
      delete (*s);
  }
  subscribers.clear();
}

// ---------------------------------------------------------------------------
//! Removes a publishing federate (private module).
void
ObjectClassAttribute::deletePublisher(int rank)
{
  list<Publisher *>::iterator i = publishers.begin();
  for (int j = 1; i != publishers.end() ; i++,j++) {
      if (j == rank) {
          delete(*i);
          publishers.erase(i);
          return;
      }
  }

  assert(false);
}

// ---------------------------------------------------------------------------
//! Removes a subscribed federate (private module).
void
ObjectClassAttribute::deleteSubscriber(int rank)
{
  list<Subscriber *>::iterator i = subscribers.begin();
  for (int j = 1; i != subscribers.end() ; i++,j++) {
      if (j == rank) {
          delete(*i);
          subscribers.erase(i);
          return;
      }
  }

  assert(false);
}

// ---------------------------------------------------------------------------
//! Displays the attribute information ( handle, name and level id)
void
ObjectClassAttribute::display(void) const
{
    cout << "            Attribute " << Handle << ':';

    if(Name != 0)
        cout << '\"' << Name << '\"';
    else
        cout << "(no name)";

    cout << " [Level "          << LevelID << ']' << endl;
}

// ---------------------------------------------------------------------------
//! returns true if federate is publishing this attribute.
Boolean
ObjectClassAttribute::IsPublishing(FederateHandle theHandle) const
{
    return ( (getPublisherRank(theHandle) != 0) ? RTI_TRUE : RTI_FALSE);
}


// ---------------------------------------------------------------------------
//! returns the list rank where publishing federate was found.
int
ObjectClassAttribute::getPublisherRank(FederateHandle theFederate) const
{
    list<Publisher *>::const_iterator i = publishers.begin();
    for (int j = 1; i != publishers.end() ; i++, j++) {
        if ( (*i)->getHandle() == theFederate )
            return j;
    }

  return 0;
}

// ---------------------------------------------------------------------------
//! returns the list rank where subscribed federate was found.
int
ObjectClassAttribute::getSubscriberRank(FederateHandle theFederate) const
{
    list<Subscriber *>::const_iterator i = subscribers.begin();
    for (int j = 1; i != subscribers.end() ; i++, j++) {
        if ( (*i)->getHandle() == theFederate )
            return j;
    }

  return 0;
}

// ---------------------------------------------------------------------------
//! Returns true if federate has subscribed to this attribute
Boolean
ObjectClassAttribute::hasSubscribed(FederateHandle theHandle) const
{
    return ( (getSubscriberRank(theHandle) != 0) ? RTI_TRUE : RTI_FALSE);
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

//! Sets the name of this attribute
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

void
ObjectClassAttribute::updateBroadcastList(ObjectClassBroadcastList *ocblist)
{
	switch(ocblist->message->Type) {
	
	case m_REFLECT_ATTRIBUTE_VALUES: {
        list<Subscriber *>::iterator i ; 
        for (i = subscribers.begin(); i != subscribers.end() ; i++) {
            ocblist->addFederate((*i)->getHandle(), Handle); // Attribute handle
        }
    }
        break;

	case m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION: {
        list<Publisher *>::iterator i ; 
        for (i = publishers.begin(); i != publishers.end() ; i++) {
            ocblist->addFederate((*i)->getHandle(), Handle); // Attribute handle
        }
    }
	break;

	default: ; // on ne fait rien
	}
}

}

// $Id: ObjectClassAttribute.cc,v 3.5 2003/01/15 14:31:43 breholee Exp $
