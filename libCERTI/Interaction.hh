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
// $Id: Interaction.hh,v 3.0.2.1 2002/11/22 18:44:01 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_INTERACTION_HH
#define _CERTI_INTERACTION_HH
 
#include <stdio.h>

#include "PrettyDebug.hh"
#include "RTItypes.hh"
#include "Parameter.hh"
#include "Subscriber.hh"
#include "Publisher.hh"

#include "List.hh"
#include "SecurityServer.hh"
#include "InteractionBroadcastList.hh"

namespace certi {

// We need to define a class because List only allows pointers, and
// not integer for example.
class InteractionChild {
public:
  InteractionClassHandle handle;
  InteractionChild(InteractionClassHandle theHandle) { handle = theHandle; };
};

class Interaction
{
  // ATTRIBUTES ------------------------------------------------------------ 
public:
  InteractionClassHandle handle; // Identifiant de la classe d'interaction
  SecurityServer *server;  // This Object help to find a TCPLink
  // given a Federate Handle.
  InteractionClassHandle parent; // Parent Class' Handle
  List <InteractionChild *> children;  // Children Classes' Handles List
  UShort depth;

  // Interaction messages' Transport Type(Reliable, Best Effort).
  // Currently not used.
  TransportType transport;
 
  // Interaction message Ordering Type(TSO, FIFO).
  // Currently not used.
  OrderType order;

private: 
  InteractionClassName name; // Must be locally allocated and deleted.  
  SecurityLevelID id;  // The default Security Level for new parameters

 
  List<Parameter*> parameterSet ;  // List of this Interaction
  // Class' Parameters.
  
  List<Subscriber*> subscribers;   // List of the Federates(Handles)
  // who subscribed to this Class.

  List <Publisher *> publishers;   // List of the Federates(Handles)
  // publishing this Class.
 

  // METHODS ---------------------------------------------------------------
public:
  Interaction();
  ~Interaction();
 
  // Name attribute access(GetName reference must be considered READ-ONLY).
  // NewName lenght must be lower or equal to MAX_USER_TAG_LENGTH.
  char *getName() {return name; };

  void setName(char *NewName)
    throw(ValueLengthExceeded, RTIinternalError);

  // Used only by CRead. Return the new parameter's handle.
  ParameterHandle addParameter(Parameter *theParameter,
			       bool Inherited = false);
 
  // Add the class' attributes to the 'Child' Class.
  void addParametersToChild(Interaction *Child);

  // Print the Interaction to the standard output.
  void display(void);

  // -- Security Methods --

  // Throw SecurityError is the Federate is not allowed to access the 
  // Interaction Class, and print an Audit message containing Reason.
  void checkFederateAccess(FederateHandle theFederate, char *Reason)
    throw(SecurityError);

  SecurityLevelID getLevelId(void) { return id ; } ;

  // A class' LevelID can only be increased.
  void setLevelId(SecurityLevelID NewLevelID);

  // -- Publication and Subscription --

  // Pour publier : PubOrUnpub = RTI_TRUE, sinon PubOrUnPub = RTI_FALSE.
  // theHandle : le numero du federe
  void publish(bool PubOrUnpub, FederateHandle theHandle)
    throw(FederateNotPublishing, RTIinternalError, SecurityError);
 
  // Pour s'abonner : SubOrUnsub = RTI_TRUE, sinon SubOrUnsub = RTI_FALSE.
  // theHandle : le numero dufedere
  void subscribe(bool SubOrUnsub, FederateHandle theHandle)
    throw(FederateNotSubscribing, RTIinternalError, SecurityError);
 
  // Pour s'abonner : SubOrUnsub = RTI_TRUE, sinon SubOrUnsub = RTI_FALSE.
  // permet de souscrire ou non a une interaction dans une region.
  void subscribe(bool SubOrUnsub, Subscriber *theSubscriber)
    throw(RegionNotKnown, InvalidRoutingSpace, RTIinternalError);

  // -- RTI Support Services --

  ParameterHandle getParameterHandle(const ParameterName theName)
    throw(InteractionParameterNotDefined, RTIinternalError);

  const ParameterName getParameterName(ParameterHandle theHandle)
    throw(InteractionParameterNotDefined, RTIinternalError);

  void killFederate(FederateHandle theFederate)
    throw();

  // -- Transport and Ordering --

  void changeTransportationType(TransportType NewType, 
				FederateHandle theHandle)
    throw(FederateNotPublishing, InvalidTransportType, RTIinternalError);
 
  void changeOrderType(OrderType NewOrder, FederateHandle theHandle)
    throw(FederateNotPublishing, InvalidOrderType, RTIinternalError);

  // -- Instance Broadcasting --

  // Check a SendInteractionOrder to see if it's OK for sending, but 
  // without sending it(to be called on the RTIA only).
  void isReady(FederateHandle theFederateHandle,
	       ParameterHandle *theParameterList,
	       UShort theListSize)
    throw(FederateNotPublishing,
	  InteractionParameterNotDefined,
	  RTIinternalError);

  // Called by RTIG in order to start the broadcasting of an Interaction
  // Message(to all federates who subscribed to this Interaction Class).
  InteractionBroadcastList * 
  sendInteraction(FederateHandle theFederateHandle,
		  ParameterHandle *theParameterList,
		  ParameterValue *theValueList,
		  UShort theListSize,
		  FederationTime theTime,
		  const char*  theTag)
    throw(FederateNotPublishing,
	  InteractionClassNotDefined,
	  InteractionParameterNotDefined,
	  RTIinternalError);

  // Called by the InteractionSet on Parent Classes whose Childrens
  // initiated a SendInteraction, to allow them to broadcast the
  // Interaction Message of their child to their own subscribers.
  // See InteractionSet::SendInteraction.
  void broadcastInteractionMessage(InteractionBroadcastList *List);

private:
 
  Parameter *getParameterByHandle(ParameterHandle theHandle)
    throw(InteractionParameterNotDefined,
	  RTIinternalError);

  // -- Private Publishers' Management --

  void addPublisher(FederateHandle theFederate)
    throw(RTIinternalError);

  void deletePublisher(int PublisherRank);

  // Return the Rank of the Federate in the list, or ZERO if not found.
  int getPublisherRank(FederateHandle theFederate);

  // Return RTI_TRUE if the Federate is publishing the attribute,
  // else return RTI_FALSE.
  bool isPublishing(FederateHandle theHandle);

  // -- Private Subcribers' Management --

  void addSubscriber(FederateHandle theFederate)
    throw(RTIinternalError);

  void deleteSubscriber(int SubscriberRank);

  // Return true if the Federate has subscribed to this attribue.
  bool isSubscribed(FederateHandle theHandle);

  // Return the Rank of the Federate in the list, or ZERO if not found.
  int getSubscriberRank(FederateHandle theFederate);

};
}

#endif // _CERTI_INTERACTION.HH

// $Id: Interaction.hh,v 3.0.2.1 2002/11/22 18:44:01 breholee Exp $
