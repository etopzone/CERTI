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
// $Id: Interaction.cc,v 3.8 2003/01/29 18:22:47 breholee Exp $
// ---------------------------------------------------------------------------

#include "Interaction.hh"

namespace certi {

static pdCDebug D("INTERACTION", "(Interact) - ");

// ---------------------------------------------------------------------------
//! Used only by CRead, return the new parameter's handle.
ParameterHandle
Interaction::addParameter(Parameter *the_parameter, bool is_inherited)
{
    the_parameter->Handle = parameterSet.size() + 1 ;

    // An inherited parameter keeps its security level, any other get the
    // default security level of the class.
    if (!is_inherited)
        the_parameter->LevelID = id ;

    parameterSet.push_front(the_parameter);

    D.Out(pdRegister, "Interaction %u(\"%s\") has a new parameter %u.",
          handle, name, the_parameter->Handle);

    return the_parameter->Handle ;
}

// ---------------------------------------------------------------------------
//! Add the class' attributes to the 'Child' Class.
void
Interaction::addParametersToChild(Interaction *the_child)
{
    // The Parameter List is read backward to respect the same attribute order
    // for the child (Parameters are inserted at the beginning of the list)
    Parameter *child = NULL ;
    list<Parameter *>::reverse_iterator p ;
    for (p = parameterSet.rbegin(); p != parameterSet.rend(); p++) {
        assert((*p) != NULL);

        child = new Parameter(*p);
        assert(child != NULL);

        D.Out(pdProtocol,
              "ObjectClass %u adding new attribute %d to child class %u.",
              handle, (*p)->Handle, the_child->handle);

        the_child->addParameter(child, RTI_TRUE);

        if (child->Handle != (*p)->Handle)
            throw RTIinternalError("Error while copying child's attributes.");
    }
}

// ---------------------------------------------------------------------------
//! addPublisher (private).
void
Interaction::addPublisher(FederateHandle the_federate)
    throw (RTIinternalError)
{
    Publisher *publisher = new Publisher(the_federate);

    if (publisher == NULL) {
        D.Out(pdExcept, "Memory exhausted while publishing interaction.");
        throw
            RTIinternalError("Memory Exhausted while publishing interaction.");
    }

    publishers.push_front(publisher);
}

// ---------------------------------------------------------------------------
// addSubscriber (private).
void
Interaction::addSubscriber(FederateHandle the_federate)
    throw (RTIinternalError)
{
    Subscriber *subscriber = new Subscriber(the_federate);

    if (subscriber == NULL) {
        D.Out(pdExcept, "Memory exhausted while subscribing interaction.");
        throw
            RTIinternalError("Memory Exhausted while "
                             "subscribing interaction.");
    }

    subscribers.push_front(subscriber);
}

// ---------------------------------------------------------------------------
/*! Called by the InteractionSet on Parent Classes whose Childrens
  initiated a SendInteraction, to allow them to broadcast the
  Interaction Message of their child to their own subscribers.
  See InteractionSet::SendInteraction.
*/
void
Interaction::broadcastInteractionMessage(InteractionBroadcastList *ibList)
{
    // 1. Set InteractionHandle to local class Handle.
    ibList->message->InteractionHandle = handle ;

    // 2. Update message Parameters list by removing child's Parameters.
    for (int i = 0  ; i < ibList->message->HandleArraySize  ; ) {
        // If the Parameter is not in that class, remove it from the message.
        try {
            getParameterByHandle(ibList->message->HandleArray[i]);
            i++ ;
        }
        catch(InteractionParameterNotDefined) {
            ibList->message->removeParameter(i);
        }
    }

    // 3. Add Interaction subscribers to the list.
    list<Subscriber *>::iterator s ;
    for (s = subscribers.begin(); s != subscribers.end(); s++) {
        D.Out(pdDebug, "Adding federate %d to BroadcastList.",
              (*s)->getHandle());
        ibList->addFederate((*s)->getHandle());
    }

    // 4. Send pending messages.
    D.Out(pdDebug,"Calling SendPendingMessage...");
    ibList->sendPendingMessage(server);
}

// ---------------------------------------------------------------------------
//! changeTransportationType.
void
Interaction::changeTransportationType(TransportType new_type,
                                      FederateHandle the_handle)
    throw (FederateNotPublishing,
           InvalidTransportType,
           RTIinternalError)
{
    if (!isPublishing(the_handle))
        throw FederateNotPublishing("Change Interaction Transport Type.");

    if ((new_type != RELIABLE) && (new_type != BEST_EFFORT))
        throw InvalidTransportType();

    transport = new_type ;

    D.Out(pdInit,
          "Interaction %d: New Transport type is %d.", handle, transport);
}

// ---------------------------------------------------------------------------
//! changeOrderType.
void
Interaction::changeOrderType(OrderType new_order, FederateHandle the_handle)
    throw (FederateNotPublishing,
           InvalidOrderType,
           RTIinternalError)
{
    if (!isPublishing(the_handle))
        throw FederateNotPublishing("Change Interaction Order Type.");

    if ((new_order != RECEIVE) && (new_order != TIMESTAMP))
        throw InvalidOrderType();

    D.Out(pdInit, "Interaction %d: New Order type is %d.", handle, order);
}

// ---------------------------------------------------------------------------
/*! Throw SecurityError is the Federate is not allowed to access the
  Interaction Class, and print an Audit message containing Reason.
*/
void
Interaction::checkFederateAccess(FederateHandle the_federate,
                                 const char *reason) const
    throw (SecurityError)
{
    // BUG: Should at least but a line in Audit
    if (server == NULL)
        return ;

    bool result = server->canFederateAccessData(the_federate, id);

    // BUG: Should use Audit.
    if (!result) {
        cout << "Interaction " << handle << " : SecurityError for federate "
             << the_federate << '(' << reason << ")." << endl ;
        throw SecurityError("Federate should not access Interaction.");
    }
}

// ---------------------------------------------------------------------------
//! Interaction.
Interaction::Interaction(void)
    : handle(0), parent(0), depth(0), transport(BEST_EFFORT), order(RECEIVE),
      name(NULL), id(PublicLevelID)
{
}

// ---------------------------------------------------------------------------
//! Destructor.
Interaction::~Interaction(void)
{
    if (name != NULL) {
        free(name);
        name = NULL ;
    }

    while (!parameterSet.empty()) {
        delete parameterSet.front();
        parameterSet.pop_front();
    }

    // Deleting Publishers
    if (!publishers.empty())
        D.Out(pdError,
              "Interaction %d: publishers list not empty at termination.",
              handle);

    while (!publishers.empty()) {
        delete publishers.front();
        publishers.pop_front();
    }

    // Deleting Subscribers
    if (!subscribers.empty())
        D.Out(pdError,
              "Interaction %d: Subscribers list not empty at termination.",
              handle);

    while (!subscribers.empty()) {
        delete subscribers.front();
        subscribers.pop_front();
    }

    // Deleting Sons
    while (!children.empty()) {
        children.pop_front();
    }
}

// ---------------------------------------------------------------------------
//! Delete a publisher with rank (private module).
void
Interaction::deletePublisher(int the_rank)
{
    list<Publisher *>::iterator p = publishers.begin();
    for (int i = 1  ; p != publishers.end(); i++,p++) {
        if (i == the_rank) {
            delete (*p);
            publishers.erase(p);
            return ;
        }
    }
}

// ---------------------------------------------------------------------------
//! Delete a subscriber with rank (private module).
void
Interaction::deleteSubscriber(int the_rank)
{
    list<Subscriber *>::iterator s = subscribers.begin();
    for (int i = 1  ; s != subscribers.end(); i++,s++) {
        if (i == the_rank) {
            delete (*s);
            subscribers.erase(s);
            return ;
        }
    }
}

// ---------------------------------------------------------------------------
//! Print the Interaction to the standard output.
void
Interaction::display(void) const
{
    cout << " Interaction " << handle << " \"" << name << "\" :" << endl ;

    // Display inheritance

    cout << " Parent Class Handle: " << parent << endl ;
    cout << " Security Level: " << id << endl ;
    cout << " " << children.size() << " Child(s):" << endl ;

    list<InteractionClassHandle>::const_iterator c = children.begin();
    for (int i = 1  ; c != children.end(); i++,c++) {
        cout << " child " << i << " Handle: " << (*c) << endl ;
    }

    // Display parameters

    cout << " " << parameterSet.size() << " Parameters:" << endl ;

    list<Parameter *>::const_iterator p = parameterSet.begin();
    for ( ; p != parameterSet.end(); p++) {
        (*p)->display();
    }
}

// ---------------------------------------------------------------------------
//! Returns the parameter by its handle (private module).
Parameter*
Interaction::getParameterByHandle(ParameterHandle the_handle) const
    throw (InteractionParameterNotDefined, RTIinternalError)
{
    list<Parameter *>::const_iterator p ;
    for (p = parameterSet.begin(); p != parameterSet.end(); p++) {
        if ((*p)->Handle == the_handle)
            return (*p);
    }

    throw InteractionParameterNotDefined();
}

// ---------------------------------------------------------------------------
//! Returns the parameter handle obtained by its name.
ParameterHandle
Interaction::getParameterHandle(const char* the_name) const
    throw (InteractionParameterNotDefined,
           RTIinternalError)
{
    list<Parameter *>::const_iterator p ;
    for (p = parameterSet.begin(); p != parameterSet.end(); p++) {
        if (strcmp((*p)->getName(), the_name) == 0)
            return (*p)->Handle ;
    }

    throw InteractionParameterNotDefined();
}

// ---------------------------------------------------------------------------
//! Returns the parameter name obtained by its handle.
const char*
Interaction::getParameterName(ParameterHandle the_handle) const
    throw (InteractionParameterNotDefined,
           RTIinternalError)
{
    return getParameterByHandle(the_handle)->getName();
}

// ---------------------------------------------------------------------------
//! Returns the publisher rank from its handle (private module).
/*! Return the Rank of the Federate in the list, or ZERO if not found.
 */
int
Interaction::getPublisherRank(FederateHandle the_federate) const
{
    list<Publisher *>::const_iterator p = publishers.begin();
    for (int i = 1  ; p != publishers.end(); i++,p++) {
        if ( (*p)->getHandle() == the_federate)
            return i ;
    }

    return 0 ;
}

// ---------------------------------------------------------------------------
//! Returns the subscriber rank from its handle (private module).
/*! Return the Rank of the Federate in the list, or ZERO if not found.
 */
int
Interaction::getSubscriberRank(FederateHandle the_federate) const
{
    list<Subscriber *>::const_iterator s = subscribers.begin();
    for (int i = 1  ; s != subscribers.end(); i++,s++) {
        if ( (*s)->getHandle() == the_federate)
            return i ;
    }

    return 0 ;
}

// ---------------------------------------------------------------------------
//! Return true if federate has subscribed to this attribute.
bool
Interaction::isSubscribed(FederateHandle the_handle) const
{
    return(getSubscriberRank(the_handle) != 0);
}

// ---------------------------------------------------------------------------
//! Return true if federate is publishing the attribute.
bool
Interaction::isPublishing(FederateHandle the_handle) const
{
    return(getPublisherRank(the_handle) != 0);
}

// ---------------------------------------------------------------------------
/*! Check a SendInteractionOrder to see if it's OK for sending, but
  without sending it(to be called on the RTIA only).
*/
void
Interaction::isReady(FederateHandle federate_handle,
                     ParameterHandle *parameter_list,
                     UShort list_size) const
    throw (FederateNotPublishing,
           InteractionParameterNotDefined,
           RTIinternalError)
{
    // Is Federate Publishing Interaction?
    if (!isPublishing(federate_handle))
        throw FederateNotPublishing();

    // Are Parameters Defined?
    for (UShort i = 0 ; i < list_size ; i++)
        getParameterByHandle(parameter_list[i]);
}

// ---------------------------------------------------------------------------
//! killFederate.
void
Interaction::killFederate(FederateHandle the_federate)
    throw ()
{
    try {
        // Is federate publishing something ? (not important)
        if (isPublishing(the_federate))
            publish(RTI_FALSE, the_federate);

        // Does federate subscribed to something ?
        if (isSubscribed(the_federate))
            subscribe(RTI_FALSE, the_federate);
    }
    catch (SecurityError &e) {}
}

// ---------------------------------------------------------------------------
/*! Pour publier : PubOrUnpub = RTI_TRUE, sinon PubOrUnPub = RTI_FALSE.
  theHandle : le numero du federe
*/
void
Interaction::publish(bool publish, FederateHandle the_handle)
    throw (FederateNotPublishing, RTIinternalError, SecurityError)
{
    bool alreadyPublishing = isPublishing(the_handle);

    checkFederateAccess(the_handle, (char*) "Publish");

    if (publish) {
        // Federate wants to Publish
        if (alreadyPublishing == RTI_FALSE) {
            D.Out(pdInit,
                  "Interaction %d: Added Federate %d to publishers list.",
                  handle, the_handle);
            addPublisher(the_handle);
        }
        else
            D.Out(pdError,
                  "Interaction %d: Inconsistent publish request from"
                  " Federate %d.", handle, the_handle);
    }
    else {
        // Federate wants to Unpublish
        if (alreadyPublishing == RTI_TRUE) {
            D.Out(pdTerm,
                  "Interaction %d: Removed Federate %d from publishers list.",
                  handle, the_handle);
            deletePublisher(getPublisherRank(the_handle));
        }
        else
            throw FederateNotPublishing();
    }
}

// ---------------------------------------------------------------------------
/*! Called by RTIG in order to start the broadcasting of an Interaction
  Message(to all federates who subscribed to this Interaction Class).
*/
InteractionBroadcastList *
Interaction::sendInteraction(FederateHandle federate_handle,
                             ParameterHandle *parameter_list,
                             ParameterValue *value_list,
                             UShort list_size,
                             FederationTime ,
                             const char* the_tag)
    throw (FederateNotPublishing,
           InteractionClassNotDefined,
           InteractionParameterNotDefined,
           RTIinternalError)
{
    // Pre-conditions checking
    if (!isPublishing(federate_handle))
        throw FederateNotPublishing();

    // Prepare and Broadcast message for this class
    InteractionBroadcastList *ibList = NULL ;
    if (server != NULL) {
        NetworkMessage *answer = new NetworkMessage ;
        answer->Type = m_RECEIVE_INTERACTION ;
        answer->Exception = e_NO_EXCEPTION ;
        answer->NumeroFederation = server->federation();
        answer->NumeroFedere = federate_handle ;
        answer->InteractionHandle = handle ; // Interaction Class Handle

        strcpy(answer->Label, the_tag);

        answer->HandleArraySize = list_size ;
        for (int i = 0  ; i < list_size  ; i++) {
            answer->HandleArray[i] = parameter_list[i] ;
            answer->setValue(i, value_list[i]);
        }

        D.Out(pdProtocol,"Preparing broadcast list.");
        ibList = new InteractionBroadcastList(answer);

        broadcastInteractionMessage(ibList);
    }
    else
        // SendInteraction should not be called on the RTIA.
        throw RTIinternalError("SendInteraction called by RTIA.");

    // Return the BroadcastList in case it had to be passed to the
    // parent class.
    return ibList ;
}

// ---------------------------------------------------------------------------
/*! Name attribute access(GetName reference must be considered READ-ONLY).
  NewName length must be lower or equal to MAX_USER_TAG_LENGTH.
*/
const char *
Interaction::getName(void) const
{
    return name ;
}

// ---------------------------------------------------------------------------
//! Change the interaction name.
void
Interaction::setName(const char *new_name)
    throw (ValueLengthExceeded, RTIinternalError)
{
    // Check Length
    if ((new_name == NULL) ||(strlen(new_name) > MAX_USER_TAG_LENGTH)) {
        D.Out(pdExcept, "Interaction Name %s too long.", new_name);
        throw ValueLengthExceeded("Interaction name too long.");
    }

    // Free previous name
    if (name != NULL)
        free(name);

    // Store new name
    name = strdup(new_name);
    if (name == NULL)
        throw RTIinternalError("Memory Exhausted.");
}

// ---------------------------------------------------------------------------
//! Change the level ID.
/*! A class' LevelID can only be increased.
 */
void
Interaction::setLevelId(SecurityLevelID new_levelID)
{
    if (server->dominates(new_levelID, id) == RTI_FALSE)
        throw SecurityError("Attempt to lower interaction class level.");

    id = new_levelID ;
}

// ---------------------------------------------------------------------------
/*! To subscribe, 'subscribe' = RTI_TRUE, otherwise 'subscribe' = RTI_FALSE.
  theHandle : the federate number.
*/
void
Interaction::subscribe(bool subscribe, FederateHandle the_handle)
    throw (FederateNotSubscribing,
           RTIinternalError,
           SecurityError)
{
    bool alreadySubscribed = isSubscribed(the_handle);

    checkFederateAccess(the_handle, "Subscribe");

    if (subscribe) {
        // Federate wants to Subscribe
        if (alreadySubscribed == RTI_FALSE) {
            addSubscriber(the_handle);
            D.Out(pdInit,
                  "Parameter %d: Added Federate %d to subscribers list.",
                  handle, the_handle);
        }
        else
            D.Out(pdError,
                  "Parameter %d: Unconsistent subscribe request from "
                  "federate %d.", handle, the_handle);
    }
    else {
        // Federate wants to unsubscribe
        if (alreadySubscribed == RTI_TRUE) {
            deleteSubscriber(getSubscriberRank(the_handle));
            D.Out(pdTerm,
                  "Parameter %d: Removed Federate %d from subscribers list.",
                  handle, the_handle);
        }
        else
            throw FederateNotSubscribing();
    }
}

// ---------------------------------------------------------------------------
/*! To subscribe, 'subscribe' = RTI_TRUE, otherwise 'subscribe' = RTI_FALSE.
  Is used to subscribe or not to an interaction in a region.
*/
void
Interaction::subscribe(bool, Subscriber*)
    throw (RegionNotKnown,
           InvalidRoutingSpace,
           RTIinternalError)
{
    // BUG: Subscribe With Region Not Implemented
    D.Out(pdExcept, "subscribe(with Region) not implemented in Interaction.");
    throw RTIinternalError("subscribe(with Region) not implemented.");
}

} // namespace certi

// $Id: Interaction.cc,v 3.8 2003/01/29 18:22:47 breholee Exp $
