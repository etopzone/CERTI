// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: Interaction.cc,v 3.29 2007/07/06 09:25:17 erk Exp $
// ----------------------------------------------------------------------------



#include "Interaction.hh"
#include "PrettyDebug.hh"

#include <iostream>
#include <assert.h>

using std::cout ;
using std::endl ;
using std::list ;

namespace certi {

static pdCDebug D("INTERACTION", "(Interact) - ");

// ----------------------------------------------------------------------------
//! Used only by CRead, return the new parameter's handle.
ParameterHandle
Interaction::addParameter(Parameter *the_parameter, bool is_inherited)
{
    the_parameter->setHandle(parameterSet.size() + 1);

    // An inherited parameter keeps its security level, any other get the
    // default security level of the class.
    if (!is_inherited)
        the_parameter->LevelID = id ;

    parameterSet.push_front(the_parameter);

    D[pdRegister] << "Interaction " << handle << "(\"" << name.c_str()
		  << "\") has a new parameter " << the_parameter->getHandle();

    return the_parameter->getHandle();
}

// ----------------------------------------------------------------------------
//! Add the class' attributes to the 'Child' Class.
void
Interaction::addParametersToChild(Interaction *the_child)
{
    // The Parameter List is read backward to respect the same attribute order
    // for the child (Parameters are inserted at the beginning of the list)
    Parameter *child = NULL ;
    list<Parameter *>::reverse_iterator it ;
    for (it = parameterSet.rbegin(); it != parameterSet.rend(); it++) {
        assert((*it) != NULL);

        child = new Parameter(**it);
        assert(child != NULL);

        D.Out(pdProtocol,
              "ObjectClass %u adding new attribute %d to child class %u.",
              handle, (*it)->getHandle(), the_child->handle);

        the_child->addParameter(child, true);

        if (child->getHandle() != (*it)->getHandle())
            throw RTIinternalError("Error while copying child's attributes.");
    }
}

// ----------------------------------------------------------------------------
/*! Called by the InteractionSet on Parent Classes whose Childrens
  initiated a SendInteraction, to allow them to broadcast the
  Interaction Message of their child to their own subscribers.
  See InteractionSet::SendInteraction.
*/
void
Interaction::broadcastInteractionMessage(InteractionBroadcastList *ibList,
					 const RTIRegion *region)
{
    // 1. Set InteractionHandle to local class Handle.
    ibList->message->interactionClass = handle ;

    // 2. Update message Parameters list by removing child's Parameters.
    for (int i = 0 ; i < ibList->message->handleArraySize ;) {
        // If the Parameter is not in that class, remove it from the message.
        try {
            getParameterByHandle(ibList->message->handleArray[i]);
            i++ ;
        }
        catch (InteractionParameterNotDefined) {
            ibList->message->removeParameter(i);
        }
    }

    // 3. Add Interaction subscribers to the list.
    addFederatesIfOverlap(*ibList, region);

    // 4. Send pending messages.
    D.Out(pdDebug, "Calling SendPendingMessage...");
    ibList->sendPendingMessage(server);
}

// ----------------------------------------------------------------------------
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
        throw InvalidTransportType("");

    transport = new_type ;

    D.Out(pdInit,
          "Interaction %d: New Transport type is %d.", handle, transport);
}

// ----------------------------------------------------------------------------
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
        throw InvalidOrderType("");

    D.Out(pdInit, "Interaction %d: New Order type is %d.", handle, order);
}

// ----------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------
//! Interaction.
Interaction::Interaction()
    : parent(0), depth(0), transport(BEST_EFFORT), order(RECEIVE), handle(0),
      id(PublicLevelID)
{
}

// ----------------------------------------------------------------------------
//! Destructor.
Interaction::~Interaction()
{
    while (!parameterSet.empty()) {
        delete parameterSet.front();
        parameterSet.pop_front();
    }

    if (!publishers.empty())
        D.Out(pdError, "Interaction %d: publishers list not empty at termination.", handle);

    // Deleting Sons
    while (!children.empty()) {
        children.pop_front();
    }
}

// ----------------------------------------------------------------------------
//! Delete a publisher with rank
void
Interaction::deletePublisher(FederateHandle fed)
{
    PublishersList::iterator it = publishers.find(fed);
    if (it != publishers.end())
	publishers.erase(it);
}

// ----------------------------------------------------------------------------
//! Print the Interaction to the standard output.
void
Interaction::display() const
{
    cout << " Interaction " << handle << " \"" << name << "\" :" << endl ;

    // Display inheritance
    cout << " Parent Class Handle: " << parent << endl ;
    cout << " Security Level: " << id << endl ;
    cout << " " << children.size() << " Child(s):" << endl ;

    list<InteractionClassHandle>::const_iterator c = children.begin();
    for (int i = 1 ; c != children.end(); i++, c++) {
        cout << " child " << i << " Handle: " << (*c) << endl ;
    }

    // Display parameters

    cout << " " << parameterSet.size() << " Parameters:" << endl ;

    list<Parameter *>::const_iterator p = parameterSet.begin();
    for (; p != parameterSet.end(); p++) {
        (*p)->display();
    }
}

// ----------------------------------------------------------------------------
//! Returns the parameter by its handle
Parameter*
Interaction::getParameterByHandle(ParameterHandle the_handle) const
    throw (InteractionParameterNotDefined, RTIinternalError)
{
    list<Parameter *>::const_iterator p ;
    for (p = parameterSet.begin(); p != parameterSet.end(); p++) {
        if ((*p)->getHandle() == the_handle)
            return (*p);
    }

    throw InteractionParameterNotDefined("");
}

// ----------------------------------------------------------------------------
//! Returns the parameter handle obtained by its name.
ParameterHandle
Interaction::getParameterHandle(const char *the_name) const
    throw (NameNotFound, RTIinternalError)
{
    list<Parameter *>::const_iterator p ;
    for (p = parameterSet.begin(); p != parameterSet.end(); p++) {
        if ((*p)->getName() == the_name)
            return (*p)->getHandle();
    }

    throw NameNotFound("");
}

// ----------------------------------------------------------------------------
//! Returns the parameter name obtained by its handle.
const char *
Interaction::getParameterName(ParameterHandle the_handle) const
    throw (InteractionParameterNotDefined,
           RTIinternalError)
{
    return getParameterByHandle(the_handle)->getName().c_str();
}

// ----------------------------------------------------------------------------
//! Return true if federate is publishing the attribute.
bool
Interaction::isPublishing(FederateHandle fed)
{
    return publishers.find(fed) != publishers.end();
}

// ----------------------------------------------------------------------------
/*! Check a SendInteractionOrder to see if it's OK for sending, but
  without sending it(to be called on the RTIA only).
*/
void
Interaction::isReady(FederateHandle federate_handle,
                     ParameterHandle *parameter_list,
                     UShort list_size)
    throw (FederateNotPublishing,
           InteractionParameterNotDefined,
           RTIinternalError)
{
    // Is Federate Publishing Interaction?
    if (!isPublishing(federate_handle))
        throw FederateNotPublishing("");

    // Are Parameters Defined?
    for (UShort i = 0 ; i < list_size ; i++)
        getParameterByHandle(parameter_list[i]);
}

// ----------------------------------------------------------------------------
//! killFederate.
void
Interaction::killFederate(FederateHandle the_federate)
    throw ()
{
    try {
        // Is federate publishing something ? (not important)
        if (isPublishing(the_federate))
            unpublish(the_federate);

        // Does federate subscribed to something ?
        if (isSubscribed(the_federate))
            unsubscribe(the_federate);
    }
    catch (SecurityError &e) {}
}

// ----------------------------------------------------------------------------
//! publish
void
Interaction::publish(FederateHandle the_handle)
    throw (FederateNotPublishing, RTIinternalError, SecurityError)
{
    checkFederateAccess(the_handle, (char *) "Publish");
    
    if (!isPublishing(the_handle)) {
	D.Out(pdInit, "Interaction %d: Added Federate %d to publishers list.", handle, the_handle);
	publishers.insert(the_handle);
    }
    else
	D.Out(pdError, "Interaction %d: Inconsistent publish request from Federate %d.", handle, the_handle);
}

// ----------------------------------------------------------------------------
//! publish
void
Interaction::unpublish(FederateHandle the_handle)
    throw (FederateNotPublishing, RTIinternalError, SecurityError)
{
    if (isPublishing(the_handle)) {
	D.Out(pdTerm,
	      "Interaction %d: Removed Federate %d from publishers list.",
	      handle, the_handle);
	deletePublisher(the_handle);
    }
    else {
	throw FederateNotPublishing("");
    }
}

// ----------------------------------------------------------------------------
/*! Called by RTIG in order to start the broadcasting of an Interaction
  Message(to all federates who subscribed to this Interaction Class).
*/
InteractionBroadcastList *
Interaction::sendInteraction(FederateHandle federate_handle,
                             ParameterHandle *parameter_list,
                             ParameterLengthPair *value_list,
                             UShort list_size,
                             FederationTime time,
			     const RTIRegion *region,
                             const char *the_tag)
    throw (FederateNotPublishing,
           InteractionClassNotDefined,
           InteractionParameterNotDefined,
           RTIinternalError)
{
    // Pre-conditions checking
    if (!isPublishing(federate_handle))
        throw FederateNotPublishing("");

    // Prepare and Broadcast message for this class
    InteractionBroadcastList *ibList = NULL ;
    if (server != NULL) {
        NetworkMessage *answer = new NetworkMessage ;
        answer->type = NetworkMessage::RECEIVE_INTERACTION ;
        answer->exception = e_NO_EXCEPTION ;
        answer->federation = server->federation();
        answer->federate = federate_handle ;
        answer->interactionClass = handle ; // Interaction Class Handle
	answer->date = time ;

        strcpy(answer->label, the_tag);

        answer->handleArraySize = list_size ;
        for (int i = 0 ; i < list_size ; i++) {
            answer->handleArray[i] = parameter_list[i] ;
            answer->setValue(i, value_list[i].value, value_list[i].length);
        }

        D.Out(pdProtocol, "Preparing broadcast list.");
        ibList = new InteractionBroadcastList(answer);

        broadcastInteractionMessage(ibList, region);
    }
    else
        // SendInteraction should not be called on the RTIA.
        throw RTIinternalError("SendInteraction called by RTIA.");

    // Return the BroadcastList in case it had to be passed to the
    // parent class.
    return ibList ;
}

// ----------------------------------------------------------------------------
/*! Name attribute access(GetName reference must be considered READ-ONLY).
  NewName length must be lower or equal to MAX_USER_TAG_LENGTH.
*/
const char *
Interaction::getName() const
{
    return name.c_str();
}

// ----------------------------------------------------------------------------
//! Change the interaction name.
void
Interaction::setName(const char *new_name)
    throw (ValueLengthExceeded, RTIinternalError)
{
    name = new_name == 0 ? "" : new_name ;
}

// ----------------------------------------------------------------------------
//! Change the level ID.
/*! A class' LevelID can only be increased.
 */
void
Interaction::setLevelId(SecurityLevelID new_levelID)
{
    if (!server->dominates(new_levelID, id))
        throw SecurityError("Attempt to lower interaction class level.");

    id = new_levelID ;
}

// ----------------------------------------------------------------------------
// setSpace
void
Interaction::setSpace(SpaceHandle h)
{
    space = h ;
}

// ----------------------------------------------------------------------------
// getSpace
SpaceHandle
Interaction::getSpace()
{
    return space ;
}

} // namespace certi

// $Id: Interaction.cc,v 3.29 2007/07/06 09:25:17 erk Exp $
