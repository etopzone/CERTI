// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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
// ----------------------------------------------------------------------------

#include "Interaction.hh"
#include "InteractionBroadcastList.hh"
#include "InteractionSet.hh"
#include "NM_Classes.hh"
#include "PrettyDebug.hh"

#include <assert.h>
#include <iostream>
#include <sstream>

using std::cout;
using std::endl;
using std::list;

namespace certi {

static PrettyDebug D("INTERACTION", "(Interact) - ");
static PrettyDebug G("GENDOC", __FILE__);

Interaction::Interaction(const std::string& name,
                         InteractionClassHandle handle,
                         TransportType transport,
                         OrderType order)
    : Subscribable(name), transport(transport), order(order), handle(handle), superClass(0), id(PublicLevelID), space(0)
{
    /*
     * The set of interaction subclass has no security server
     */
    subClasses = new InteractionSet(NULL);
} /* end of Interaction constructor */

Interaction::~Interaction()
{
    for (HandleParameterMap::iterator i = _handleParameterMap.begin(); i != _handleParameterMap.end(); ++i) {
        delete i->second;
    }

    if (!publishers.empty()) {
        Debug(D, pdError) << "Interaction " << handle << ": publishers list not empty at termination." << std::endl;
    }

    // Deleting subclasses
    if (NULL != subClasses) {
        delete subClasses;
    }
} /* end of ~Interaction */

void Interaction::addSubClass(Interaction* child)
{
    /* build parent-child relationship */
    /* add child as subclass of parent */
    subClasses->addClass(child, NULL);
    /* link child to parent */
    child->superClass = handle;
    /* forward inherited properties to child */
    /* Add Interaction Class Parameter */
    addInheritedClassParameter(child);
    /* security server is the same for child */
    child->server = server;
    /* inherit security Level */
    child->setSecurityLevelId(id);
} /* end of addSubClass */

ParameterHandle Interaction::addParameter(Parameter* parameter, bool is_inherited)
{
    if (parameter == NULL)
        throw RTIinternalError("Tried to add NULL parameter.");

    ParameterHandle parameterHandle = parameter->getHandle();
    if (_handleParameterMap.find(parameterHandle) != _handleParameterMap.end()) {
        throw RTIinternalError("Interaction::addParameter: Tried to add parameter with duplicate handle.");
    }

    // An inherited parameter keeps its security level, any other get the
    // default security level of the class.
    if (!is_inherited) {
        parameter->LevelID = id;
    }

    _handleParameterMap[parameterHandle] = parameter;

    Debug(D, pdRegister) << "Interaction " << handle << "[" << name << "] has a new parameter " << parameterHandle
                         << "[" << parameter->getName() << "]" << std::flush;

    return parameterHandle;
} /* end of addParameter */

void Interaction::addInheritedClassParameter(Interaction* the_child)
{
    for (HandleParameterMap::iterator i = _handleParameterMap.begin(); i != _handleParameterMap.end(); ++i) {
        assert(i->second != NULL);

        Parameter* child = new Parameter(*i->second);
        assert(child != NULL);

        Debug(D, pdProtocol) << "ObjectClass " << handle << " adding new parameter " << i->second->getHandle()
                             << " to child class " << the_child->handle << std::endl;

        the_child->addParameter(child, true);
    }
} /* end of addInheritedClassParameter */

// ----------------------------------------------------------------------------
/*! Called by the InteractionSet on Parent Classes whose Childrens
  initiated a SendInteraction, to allow them to broadcast the
  Interaction Message of their child to their own subscribers.
  See InteractionSet::SendInteraction.
 */
Responses Interaction::broadcastInteractionMessage(InteractionBroadcastList* ibList, const RTIRegion* region)
{
    Debug(G, pdGendoc) << "enter Interaction::broadcastInteractionMessage" << std::endl;

    // 1. Set InteractionHandle to local class Handle.
    ibList->getMessage().setInteractionClass(handle);

    // 2. Update message Parameters list by removing child's Parameters.
    for (uint32_t i = 0; i < ibList->getMessage().getParametersSize();) {
        // If the Parameter is not in that class, remove it from the message.
        if (hasParameter(ibList->getMessage().getParameters(i))) {
            ++i;
        }
        else {
            ibList->getMessage().removeParameters(i);
        }
    }

    // 3. Add Interaction subscribers to the list.
    addFederatesIfOverlap(*ibList, region);

    // 4. Send pending messages.
    Debug(D, pdDebug) << "Calling SendPendingMessage..." << std::endl;
    auto ret = ibList->preparePendingMessage(*server);

    Debug(G, pdGendoc) << "exit Interaction::broadcastInteractionMessage" << std::endl;

    return ret;
} /* end of broadcastInteractionMessage */

// ----------------------------------------------------------------------------
//! changeTransportationType.
void Interaction::changeTransportationType(TransportType new_type, FederateHandle the_handle)
{
    if (!isPublishing(the_handle))
        throw FederateNotPublishing("Change Interaction Transport Type.");

    if ((new_type != RELIABLE) && (new_type != BEST_EFFORT))
        throw InvalidTransportationHandle("");

    transport = new_type;

    Debug(D, pdInit) << "Interaction " << handle << ": New Transport type is " << transport << std::endl;
} /* end of changeTransportationType */

// ----------------------------------------------------------------------------
//! changeOrderType.
void Interaction::changeOrderType(OrderType new_order, FederateHandle the_handle)
{
    if (!isPublishing(the_handle))
        throw FederateNotPublishing("Change Interaction Order Type.");

    if ((new_order != RECEIVE) && (new_order != TIMESTAMP))
        throw InvalidOrderingHandle("");

    Debug(D, pdInit) << "Interaction " << handle << ": New Order type is " << order << std::endl;
} /* end of changeOrderType */

// ----------------------------------------------------------------------------
/*! Throw SecurityError is the Federate is not allowed to access the
  Interaction Class, and print an Audit message containing Reason.
 */
void Interaction::checkFederateAccess(FederateHandle the_federate, const std::string& reason) const
{
    // BUG: Should at least but a line in Audit
    if (server == NULL)
        return;

    bool result = server->canFederateAccessData(the_federate, id);

    // BUG: Should use Audit.
    if (!result) {
        cout << "Interaction " << handle << " : SecurityError for federate " << the_federate << '(' << reason << ")."
             << endl;
        throw SecurityError("Federate should not access Interaction.");
    }
}

// ----------------------------------------------------------------------------
//! Delete a publisher with rank
void Interaction::deletePublisher(FederateHandle fed)
{
    PublishersList::iterator it = publishers.find(fed);
    if (it != publishers.end())
        publishers.erase(it);
}

// ----------------------------------------------------------------------------
//! Print the Interaction to the standard output.
void Interaction::display() const
{
    cout << " Interaction " << handle << " \"" << name << "\" :" << endl;

    // Display inheritance
    cout << " Parent Class Handle: " << getSuperclass() << endl;
    cout << " Security Level: " << id << endl;
    cout << " " << subClasses->size() << " Child(s):" << endl;

    //    list<InteractionClassHandle>::const_iterator c = children.begin();
    //    for (int i = 1 ; c != children.end(); i++, c++) {
    //        cout << " child " << i << " Handle: " << (*c) << endl ;
    //    }

    // Display parameters

    cout << " " << _handleParameterMap.size() << " Parameters:" << endl;

    for (HandleParameterMap::const_iterator i = _handleParameterMap.begin(); i != _handleParameterMap.end(); ++i) {
        i->second->display();
    }
}

// ----------------------------------------------------------------------------
//! Returns the parameter by its handle
Parameter* Interaction::getParameterByHandle(ParameterHandle the_handle) const
{
    HandleParameterMap::const_iterator i = _handleParameterMap.find(the_handle);
    if (i != _handleParameterMap.end()) {
        return i->second;
    }

    throw InteractionParameterNotDefined("for handle " + std::to_string(the_handle));
} /* end of getParameterByHandle */

// ----------------------------------------------------------------------------
//! Returns the parameter handle obtained by its name.
ParameterHandle Interaction::getParameterHandle(const std::string& the_name) const
{
    for (HandleParameterMap::const_iterator i = _handleParameterMap.begin(); i != _handleParameterMap.end(); ++i) {
        if (i->second->getName() == the_name) {
            return i->second->getHandle();
        }
    }

    throw NameNotFound(the_name);
}

// ----------------------------------------------------------------------------

const std::string& Interaction::getParameterName(ParameterHandle the_handle) const

{
    return getParameterByHandle(the_handle)->getName();
}

// ----------------------------------------------------------------------------
//! Return true if the interaction contains the given parameter
bool Interaction::hasParameter(ParameterHandle parameterHandle) const
{
    return _handleParameterMap.find(parameterHandle) != _handleParameterMap.end();
}

// ----------------------------------------------------------------------------
//! Return true if federate is publishing the attribute.
bool Interaction::isPublishing(FederateHandle fed)
{
    return publishers.find(fed) != publishers.end();
}

// ----------------------------------------------------------------------------
/*! Check a SendInteractionOrder to see if it's OK for sending, but
  without sending it(to be called on the RTIA only).
 */
void Interaction::isReady(FederateHandle federate_handle,
                          const std::vector<ParameterHandle>& parameter_list,
                          uint16_t list_size)
{
    // Is Federate Publishing Interaction?
    if (!isPublishing(federate_handle))
        throw FederateNotPublishing("");

    // Are Parameters Defined?
    for (uint16_t i = 0; i < list_size; i++)
        getParameterByHandle(parameter_list[i]);
}

// ----------------------------------------------------------------------------
//! killFederate.
void Interaction::killFederate(FederateHandle the_federate) noexcept
{
    try {
        // Is federate publishing something ? (not important)
        if (isPublishing(the_federate))
            unpublish(the_federate);

        // Does federate subscribed to something ?
        if (isSubscribed(the_federate))
            unsubscribe(the_federate);
    }
    catch (SecurityError& e) {
    }
}

// ----------------------------------------------------------------------------
//! publish
void Interaction::publish(FederateHandle the_handle)
{
    checkFederateAccess(the_handle, (char*) "Publish");

    if (!isPublishing(the_handle)) {
        Debug(D, pdInit) << "Interaction " << handle << ": Added Federate " << the_handle << " to publishers list."
                         << std::endl;
        publishers.insert(the_handle);
    }
    else {
        Debug(D, pdError) << "Interaction " << handle << ": Inconsistent publish request from Federate " << the_handle
                          << std::endl;
    }
}

// ----------------------------------------------------------------------------
//! publish
void Interaction::unpublish(FederateHandle the_handle)
{
    if (isPublishing(the_handle)) {
        Debug(D, pdTerm) << "Interaction " << handle << ": Removed Federate " << the_handle << " from publishers list."
                         << std::endl;
        deletePublisher(the_handle);
    }
    else {
        throw FederateNotPublishing("");
    }
}

// ----------------------------------------------------------------------------
/*! Called by RTIG in order to start the broadcasting of an Interaction
  Message(to all federates who subscribed to this Interaction Class).
  with time
 */
std::pair<InteractionBroadcastList*, Responses>
Interaction::sendInteraction(FederateHandle federate_handle,
                             const std::vector<ParameterHandle>& parameter_list,
                             const std::vector<ParameterValue_t>& value_list,
                             uint16_t list_size,
                             FederationTime time,
                             const RTIRegion* region,
                             const std::string& the_tag)
{
    Debug(G, pdGendoc) << "enter Interaction::sendInteraction with time" << std::endl;

    Responses responses;
    InteractionBroadcastList* ibList;

    // Pre-conditions checking
    if (!isPublishing(federate_handle))
        throw FederateNotPublishing("");

    // Prepare and Broadcast message for this class
    if (server != NULL) {
        NM_Receive_Interaction answer;
        answer.setException(Exception::Type::NO_EXCEPTION);
        answer.setFederation(server->federation().get());
        answer.setFederate(federate_handle);
        answer.setInteractionClass(handle); // Interaction Class Handle
        answer.setDate(time);

        answer.setLabel(the_tag);

        answer.setParametersSize(list_size);
        answer.setValuesSize(list_size);
        for (int i = 0; i < list_size; i++) {
            answer.setParameters(parameter_list[i], i);
            answer.setValues(value_list[i], i);
        }

        Debug(D, pdProtocol) << "Preparing broadcast list." << std::endl;
        ibList = new InteractionBroadcastList(answer);

        responses = broadcastInteractionMessage(ibList, region);
    }
    else
        // SendInteraction should not be called on the RTIA.
        throw RTIinternalError("SendInteraction called by RTIA.");

    Debug(G, pdGendoc) << "exit Interaction::sendInteraction with time" << std::endl;

    // Return the BroadcastList in case it had to be passed to the
    // parent class.
    return {ibList, std::move(responses)};
}

// ----------------------------------------------------------------------------
/*! Called by RTIG in order to start the broadcasting of an Interaction
  Message(to all federates who subscribed to this Interaction Class).
  without time
 */
std::pair<InteractionBroadcastList*, Responses>
Interaction::sendInteraction(FederateHandle federate_handle,
                             const std::vector<ParameterHandle>& parameter_list,
                             const std::vector<ParameterValue_t>& value_list,
                             uint16_t list_size,
                             const RTIRegion* region,
                             const std::string& the_tag)
{
    Debug(G, pdGendoc) << "enter Interaction::sendInteraction without time" << std::endl;

    Responses responses;
    InteractionBroadcastList* ibList;

    // Pre-conditions checking
    if (!isPublishing(federate_handle))
        throw FederateNotPublishing("");

    // Prepare and Broadcast message for this class
    if (server != NULL) {
        NM_Receive_Interaction answer;
        answer.setException(Exception::Type::NO_EXCEPTION);
        answer.setFederation(server->federation().get());
        answer.setFederate(federate_handle);
        answer.setInteractionClass(handle); // Interaction Class Handle
        answer.setLabel(the_tag);

        answer.setParametersSize(list_size);
        answer.setValuesSize(list_size);

        for (int i = 0; i < list_size; i++) {
            answer.setParameters(parameter_list[i], i);
            answer.setValues(value_list[i], i);
        }

        Debug(D, pdProtocol) << "Preparing broadcast list." << std::endl;
        ibList = new InteractionBroadcastList(answer);

        responses = broadcastInteractionMessage(ibList, region);
    }
    else {
        // SendInteraction should not be called on the RTIA.
        throw RTIinternalError("SendInteraction called by RTIA.");
    }

    Debug(G, pdGendoc) << "exit Interaction::sendInteraction without time" << std::endl;

    // Return the BroadcastList in case it had to be passed to the
    // parent class.
    return {ibList, std::move(responses)};
}

// ----------------------------------------------------------------------------
//! Change the level ID.
/*! A class' LevelID can only be increased.
 */
void Interaction::setSecurityLevelId(SecurityLevelID new_levelID)
{
    if (!server->dominates(new_levelID, id))
        throw SecurityError("Attempt to lower interaction class level.");

    id = new_levelID;
}

// ----------------------------------------------------------------------------
// setSpace
void Interaction::setSpace(SpaceHandle h)
{
    space = h;
}

// ----------------------------------------------------------------------------
// getSpace
SpaceHandle Interaction::getSpace()
{
    return space;
}

} // namespace certi
