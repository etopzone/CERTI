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
// $Id: Interaction.cc,v 3.0.2.1 2002/11/22 00:52:56 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "Interaction.hh"

namespace certi {

  static pdCDebug D("INTERACTION", "(Interact) - ");

  // ------------------
  // -- AddParameter --
  // ------------------

  ParameterHandle Interaction::addParameter(Parameter *theParameter,
					    bool Inherited)
  {
    theParameter->Handle = parameterSet.lg + 1;

    // An inherited parameter keeps its security level, any other get the
    // default security level of the class.
    if(!Inherited)
      theParameter->LevelID = id;

    parameterSet.Inserer(1, theParameter);

    D.Out(pdRegister, "Interaction %u(\"%s\") has a new parameter %u.",
	  handle, name, theParameter->Handle);

    return theParameter->Handle;
  }


  // --------------------------
  // -- AddParametersToChild --
  // --------------------------

  void Interaction::addParametersToChild(Interaction *Child)
  {
    int ParamIndex;
    Parameter *parameter = NULL;
    Parameter *child = NULL;

    // The Parameter List is read backwards to respect the same attribute
    // order for the child(Parameters are inserted at the beginning of the list)

    for(ParamIndex = parameterSet.lg; ParamIndex > 0; ParamIndex--) {
      parameter = parameterSet.Ieme(ParamIndex);
      assert(parameter != NULL);

      child = new Parameter(parameter);
      assert(child != NULL);
 
      D.Out(pdProtocol, 
	    "ObjectClass %u adding new attribute %d to child class %u.",
	    handle, parameter->Handle, Child->handle);

      Child->addParameter(child, RTI_TRUE);

      if(child->Handle != parameter->Handle)
	throw RTIinternalError("Error while copying child's attributes.");

    }
  }


  // ------------------
  // -- AddPublisher --(private)
  // ------------------

  void Interaction::addPublisher(FederateHandle theFederate)
    throw(RTIinternalError)
  {
    Publisher *publisher = new Publisher(theFederate);
 
    if(publisher == NULL) {
      D.Out(pdExcept, "Memory exhausted while publishing interaction.");
      throw RTIinternalError("Memory Exhausted while publishing interaction.");
    }
 
    publishers.Inserer(1, publisher);
  }


  // ------------------
  // -- AddSubscriber --(private)
  // ------------------

  void Interaction::addSubscriber(FederateHandle theFederate)
    throw(RTIinternalError)
  {
    Subscriber *subscriber = new Subscriber(theFederate);
 
    if(subscriber == NULL) {
      D.Out(pdExcept, "Memory exhausted while subscribing interaction.");
      throw RTIinternalError("Memory Exhausted while subscribing inter.");
    }

    subscribers.Inserer(1, subscriber);
  }


  // ---------------------------------
  // -- BroadcastInteractionMessage --
  // ---------------------------------

  void Interaction::
  broadcastInteractionMessage(InteractionBroadcastList *List)
  {
    int SubIndex = 0; // Class subscribers index
    int ParamIndex = 0; // Message's Parameter index.

    Subscriber *theSubscriber = NULL;

    // 1. Set InteractionHandle to local class Handle.
    //printf("(BroadcastInteractionMessage) debut\n");
    List->Message->InteractionHandle = handle;

    // 2. Update message Parameters list by removing child's Parameters.
    ParamIndex = 0;

    while(ParamIndex < List->Message->HandleArraySize) {
      // If the Parameter is not in that class, remove it from the message.
      try {
	getParameterByHandle(List->Message->HandleArray [ParamIndex]);
	ParamIndex ++;
      } catch(InteractionParameterNotDefined) {
	List->Message->removeParameter(ParamIndex);
      }
    }
    //printf("(BroadcastInteractionMessage) handle : %d paramindex %d\n",
    // Handle,ParamIndex); 
    // 3. Add Interaction subscribers to the list.
 
    for(SubIndex = 1; SubIndex <= subscribers.lg; SubIndex++) { 
      theSubscriber = subscribers.Ieme(SubIndex);
      D.Out(pdDebug, "Ajout du Federe %d a la BroadcastList.",
	    theSubscriber->getHandle());
      List->addFederate(theSubscriber->getHandle());
    }

    // 4. Send pending messages.
    D.Out(pdDebug,"Calling SendPendingMessage...");
    List->sendPendingMessage(server);
  }


  // ------------------------------
  // -- ChangeTransportationType --
  // ------------------------------

  void Interaction::changeTransportationType(TransportType NewType,
					     FederateHandle theHandle)
    throw(FederateNotPublishing,
	  InvalidTransportType,
	  RTIinternalError)
  {
    if(!isPublishing(theHandle))
      throw FederateNotPublishing("Change Interaction Transport Type.");

    if((NewType != RELIABLE) &&(NewType != BEST_EFFORT))
      throw InvalidTransportType();

    transport = NewType;

    D.Out(pdInit, 
	  "Interaction %d: New Transport type is %d.", handle, transport);
  }


  // ---------------------
  // -- ChangeOrderType --
  // ---------------------

  void Interaction::changeOrderType(OrderType NewOrder,
				    FederateHandle theHandle)
    throw(FederateNotPublishing,
	  InvalidOrderType,
	  RTIinternalError)
  {
    if(!isPublishing(theHandle))
      throw FederateNotPublishing("Change Interaction Order Type.");

    if((NewOrder != RECEIVE) &&(NewOrder != TIMESTAMP))
      throw InvalidOrderType();

    D.Out(pdInit, 
	  "Interaction %d: New Order type is %d.", handle, order);
  }


  // -------------------------
  // -- CheckFederateAccess --
  // -------------------------

  void Interaction::checkFederateAccess(FederateHandle theFederate,
					char *Reason)
    throw(SecurityError)
  {
    bool result = false ;

    // BUG: Should at least but a line in Audit
    if(server == NULL)
      return;

    result = server->canFederateAccessData(theFederate, id);

    // BUG: Should use Audit.
    if(!result) {
      printf("Interaction %ld : SecurityError for federate %ld(%s).\n",
	     handle, theFederate, Reason);
      throw SecurityError("Federate should not access Interaction.");
    }
  }


  // ------------------
  // -- Interaction --
  // ------------------

  Interaction::Interaction()
    : parameterSet(), subscribers(), publishers()
  {
    handle = 0;
    name = NULL;
    parent = 0;
 
    depth = 0;
 
    id = PublicLevelID;

    order = RECEIVE;
    transport = BEST_EFFORT;
  }


  // -------------------
  // -- ~Interaction --
  // -------------------

  Interaction::~Interaction()
  {
    InteractionChild *Son = NULL;
    Parameter *Parameter = NULL;
    Publisher *publisher = NULL;
    Subscriber *Subscriber = NULL;

    if(name != NULL) {
      free(name);
      name = NULL;
    }

    while(parameterSet.lg > 0) {
      Parameter = parameterSet.Ieme(1);
      parameterSet.Supprimer(1);
      delete Parameter;
    }

    // Deleting Publishers
    if(publishers.lg > 0)
      D.Out(pdError, 
	    "Interaction %d: publishers list not empty at termination.",
	    handle);
 
    while(publishers.lg > 0) {
      publisher = publishers.Ieme(1);
      publishers.Supprimer(1);
      delete publisher;
    }
 
    // Deleting Subscribers
    if(subscribers.lg > 0)
      D.Out(pdError, 
	    "Interaction %d: Subscribers list not empty at termination.",
	    handle);
 
    while(subscribers.lg > 0) {
      Subscriber = subscribers.Ieme(1);
      subscribers.Supprimer(1);
      delete Subscriber;
    }
 
    // Deleting Sons
    while(children.lg > 0) {
      Son = children.Ieme(1);
      children.Supprimer(1);
      delete Son;
    }

  }


  // ---------------------
  // -- DeletePublisher --(private)
  // ---------------------

  void Interaction::deletePublisher(int PublisherRank)
  {
    Publisher *publisher = publishers.Ieme(PublisherRank);

    publishers.Supprimer(PublisherRank);

    delete publisher;
  }


  // ----------------------
  // -- DeleteSubscriber --(private)
  // ----------------------

  void Interaction::deleteSubscriber(int SubscriberRank)
  {
    Subscriber *Subscriber = subscribers.Ieme(SubscriberRank);

    subscribers.Supprimer(SubscriberRank);

    delete Subscriber;
  }


  // -------------
  // -- Display --
  // -------------

  void Interaction::display(void)
  {
    int i;
    InteractionChild *Son = NULL;
    Parameter *Parameter = NULL;

    printf(" Interaction %ld \"%s\" :\n", handle, name);

    // Display inheritance

    printf(" Parent Class Handle: %ld\n", parent);
    printf(" Security Level: %d\n", id);
    printf(" %d Child(s):\n", children.lg);

    for(i = 1; i <= children.lg; i++) {
      Son = children.Ieme(i);
      printf(" Son %d Handle: %ld\n", i, Son->handle);
    }

    // Display Parameters

    printf(" %d Parameters:\n", parameterSet.lg);

    for(i = 1; i <= parameterSet.lg; i++) {
      Parameter = parameterSet.Ieme(i);
      Parameter->display();
    }

  }


  // --------------------------
  // -- GetParameterByHandle --(private)
  // --------------------------

  Parameter *Interaction::getParameterByHandle(ParameterHandle theHandle)
    throw(InteractionParameterNotDefined,
	  RTIinternalError)
  {
    int i;
    Parameter *Parameter = NULL;

    for(i = 1; i <= parameterSet.lg; i++) {
      Parameter = parameterSet.Ieme(i);
      if(Parameter->Handle == theHandle) 
	return Parameter;
    }

    throw InteractionParameterNotDefined();
  }


  // ------------------------
  // -- GetParameterHandle --
  // ------------------------

  ParameterHandle Interaction::
  getParameterHandle(const ParameterName theName)
    throw(InteractionParameterNotDefined,
	  RTIinternalError)
  {
    int i;
    Parameter *Parameter = NULL;

    for(i = 1; i <= parameterSet.lg; i++) {
      Parameter = parameterSet.Ieme(i);
      if(strcmp(Parameter->getName(), theName) == 0)
	return Parameter->Handle;
    }
 
    throw InteractionParameterNotDefined();
  }


  // ----------------------
  // -- GetParameterName --
  // ----------------------

  const ParameterName Interaction::getParameterName(ParameterHandle theHandle)
    throw(InteractionParameterNotDefined,
	  RTIinternalError)
  {
    return getParameterByHandle(theHandle)->getName();
  }


  // ----------------------
  // -- GetPublisherRank --(private)
  // ----------------------

  int Interaction::getPublisherRank(FederateHandle theFederate)
  {
    int i;
    Publisher *publisher;

    for(i = 1; i <= publishers.lg; i++) {
      publisher = publishers.Ieme(i);
      if(publisher->Handle == theFederate)
	return i;
    }

    return 0;
  }


  // -----------------------
  // -- GetSubscriberRank --(private)
  // -----------------------

  int Interaction::getSubscriberRank(FederateHandle theFederate)
  {
    int i;
    Subscriber *Subscriber;

    for(i = 1; i <= subscribers.lg; i++) {
      Subscriber = subscribers.Ieme(i);
      if(Subscriber->getHandle() == theFederate)
	return i;
    }

    return 0;
  }


  // -------------------
  // -- HasSubscribed --
  // -------------------

  bool Interaction::isSubscribed(FederateHandle theHandle)
  {
    return (getSubscriberRank(theHandle) != 0) ;
  }



  // ------------------
  // -- isPublishing --
  // ------------------

  bool Interaction::isPublishing(FederateHandle theHandle)
  {
    return (getPublisherRank(theHandle) != 0) ;
  }


  // -------------
  // -- IsReady --
  // -------------

  void Interaction::isReady(FederateHandle theFederateHandle,
			    ParameterHandle *theParameterList,
			    UShort theListSize)
    throw(FederateNotPublishing,
	  InteractionParameterNotDefined,
	  RTIinternalError)
  {
    UShort ParamIndex;

    // Is Federate Publishing Interaction?
    if(!isPublishing(theFederateHandle))
      throw FederateNotPublishing();

    // Are Parameters Defined?
    for(ParamIndex = 0; ParamIndex < theListSize; ParamIndex ++)
      getParameterByHandle(theParameterList [ParamIndex]);

  }


  // ------------------
  // -- KillFederate --
  // ------------------

  void Interaction::killFederate(FederateHandle theFederate)
    throw()
  {
    try {

      // Est-ce que le federe publie des trucs ?(pas important)
      if(isPublishing(theFederate))
	publish(RTI_FALSE, theFederate);
 
      // Est-ce que le federe a souscrit a quelquechose ?
      if(isSubscribed(theFederate))
	subscribe(RTI_FALSE, theFederate);

    } catch(SecurityError &e) {}
  }

  // -------------
  // -- Publish --
  // -------------

  void Interaction::publish(bool PubOrUnpub, 
			    FederateHandle theHandle)
    throw(FederateNotPublishing,
	  RTIinternalError,
	  SecurityError)
  {
    bool AlreadyPublishing = isPublishing(theHandle);

    checkFederateAccess(theHandle, (char*) "Publish");

    if(PubOrUnpub) {
      // Federate wants to Publish
      if(AlreadyPublishing == RTI_FALSE) {
	D.Out(pdInit, "Interaction %d: Added Federate %d to publishers list.",
	      handle, theHandle);
	addPublisher(theHandle);
      }
      else
	D.Out(pdError,
	      "Interaction %d: Inconsistent publish request from Federate %d.",
	      handle, theHandle);
    }

    else if(!PubOrUnpub) {
      // Federate wants to Unpublish
      if(AlreadyPublishing == RTI_TRUE) {
	D.Out(pdTerm, 
	      "Interaction %d: Removed Federate %d from publishers list.",
	      handle, theHandle);
	deletePublisher(getPublisherRank(theHandle));
      }
      else
	throw FederateNotPublishing();
    }

  }


  // ---------------------
  // -- SendInteraction --
  // ---------------------

  InteractionBroadcastList * 
  Interaction::sendInteraction(FederateHandle theFederateHandle,
			       ParameterHandle *theParameterList,
			       ParameterValue *theValueList,
			       UShort theListSize,
			       FederationTime,
			       UserSuppliedTag theTag)
    throw(FederateNotPublishing,
	  InteractionClassNotDefined,
	  InteractionParameterNotDefined,
	  RTIinternalError)
  {
    int i;
    NetworkMessage *Answer = NULL;
    InteractionBroadcastList *List = NULL;

    // Pre-conditions checking
    if(!isPublishing(theFederateHandle))
      throw FederateNotPublishing();

    // Prepare and Broadcast message for this class
    if(server != NULL) 
      {
	Answer = new NetworkMessage;
	Answer->Type = m_RECEIVE_INTERACTION;
	Answer->Exception = e_NO_EXCEPTION;
	Answer->NumeroFederation = server->federation();
	Answer->NumeroFedere = theFederateHandle;

	Answer->InteractionHandle = handle; // Interaction Class Handle

	strcpy(Answer->Label, theTag);

	Answer->HandleArraySize = theListSize;
	for(i = 0;i < theListSize;i++) {
	  Answer->HandleArray [i] = theParameterList[i];
	  Answer->setValue(i, theValueList [i]);
	}
 
	D.Out(pdProtocol,"Preparation de la liste de diffusion.");
	List = new InteractionBroadcastList(Answer);

	broadcastInteractionMessage(List);
      }

    else
      // SendInteraction should not be called on the RTIA.
      throw RTIinternalError("SendInteraction called by RTIA.");

    // Return the BroadcastList in case it had to be passed to the parent class.
    return List;
  }


  // -------------
  // -- SetName --
  // -------------

  void Interaction::setName(char *NewName)
    throw(ValueLengthExceeded, RTIinternalError)
  {
    // Check Length
    if((NewName == NULL) ||(strlen(NewName) > MAX_USER_TAG_LENGTH)) {
      D.Out(pdExcept, "Interaction Name %s too long.", NewName);
      throw ValueLengthExceeded("Interaction name too long.");
    }

    // Free previous name
    if(name != NULL)
      free(name);

    // Store new name
    name = strdup(NewName);
    if(name == NULL)
      throw RTIinternalError("Memory Exhausted.");
  }


  // ----------------
  // -- SetLevelID --
  // ----------------

  void Interaction::setLevelId(SecurityLevelID NewLevelID)
  {
    if(server->dominates(NewLevelID, id) == RTI_FALSE)
      throw SecurityError("Attempt to lower interaction class level.");

    id = NewLevelID;
  }
 

  // ---------------
  // -- Subscribe --
  // ---------------

  void Interaction::subscribe(bool SubOrUnsub,
			      FederateHandle theHandle)
    throw(FederateNotSubscribing,
	  RTIinternalError,
	  SecurityError)
  {
    bool AlreadySubscribed = isSubscribed(theHandle);

    checkFederateAccess(theHandle, (char*) "Subscribe");

    if(SubOrUnsub) {
      // Federate wants to Subscribe
      if(AlreadySubscribed == RTI_FALSE) {
	addSubscriber(theHandle);
	D.Out(pdInit, "Parameter %d: Added Federate %d to subscribers list.",
	      handle, theHandle);
      }
      else
	D.Out(pdError, 
	      "Parameter %d: Unconsistent subscribe request from federate %d.",
	      handle, theHandle);
    }

    else if(!SubOrUnsub) {
      // Federate wants to unsubscribe
      if(AlreadySubscribed == RTI_TRUE) {
	deleteSubscriber(getSubscriberRank(theHandle));
	D.Out(pdTerm, 
	      "Parameter %d: Removed Federate %d from subscribers list.",
	      handle, theHandle);
      }
      else
	throw FederateNotSubscribing();
    }

  }


  // ---------------
  // -- Subscribe --
  // ---------------

  void Interaction::subscribe(bool, Subscriber*)
    throw(RegionNotKnown,
	  InvalidRoutingSpace,
	  RTIinternalError)
  {
    // BUG: Subscribe With Region Not Implemented
    D.Out(pdExcept, "subscribe(with Region) not implemented in Interaction.");
    throw RTIinternalError("subscribe(with Region) not implemented.");
  }
}

// $Id: Interaction.cc,v 3.0.2.1 2002/11/22 00:52:56 breholee Exp $
