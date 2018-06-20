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
#include "PrettyDebug.hh"

#include <iostream>

using std::cout;
using std::endl;
using std::list;

namespace certi {

static PrettyDebug D("INTERACTIONSET", "(InterSet) - ");
static PrettyDebug G("GENDOC", __FILE__);

InteractionSet::InteractionSet(SecurityServer* security_server, bool isRootClassSet)
    : TreeNamedAndHandledSet<Interaction>("Interaction Classes", isRootClassSet)
{
    // It can be NULL on the RTIA.
    server = security_server;
}

InteractionSet::~InteractionSet()
{
} /* end of ~InteractionSet */

void InteractionSet::addClass(Interaction* newClass, Interaction* parentClass)
{
    Debug(D, pdInit) << "Adding new interaction class " << newClass->getHandle() << std::endl;
    /* link to server */
    newClass->server = server;
    add(newClass, parentClass);
} /* end of addClass */

// ----------------------------------------------------------------------------
//! broadcastInteraction with time.
Responses InteractionSet::broadcastInteraction(FederateHandle federate_handle,
                                               InteractionClassHandle interaction_handle,
                                               const std::vector<ParameterHandle>& parameter_list,
                                               const std::vector<ParameterValue_t>& value_list,
                                               uint16_t list_size,
                                               FederationTime the_time,
                                               const RTIRegion* region,
                                               const std::string& the_tag)
{
    Debug(G, pdGendoc) << "enter InteractionSet::broadcastInteraction with time" << std::endl;

    Responses responses;

    // It may throw InteractionClassNotDefined.
    Interaction* theInteraction = getObjectFromHandle(interaction_handle);

    InteractionBroadcastList* ibList;
    std::tie(ibList, responses) = theInteraction->sendInteraction(
        federate_handle, parameter_list, value_list, list_size, the_time, region, the_tag);

    // Pass the Message(and its BroadcastList) to the Parent Classes.
    if (ibList != NULL) {
        while (theInteraction->getSuperclass() != 0) {
            theInteraction = getObjectFromHandle(theInteraction->getSuperclass());
            auto resp = theInteraction->broadcastInteractionMessage(ibList, region);
            responses.insert(
                std::end(responses), make_move_iterator(std::begin(resp)), make_move_iterator(std::end(resp)));
        }
        delete ibList;
    }
    else {
        // BroadcastInteraction should not be called on the RTIA(see IsReady)
        throw RTIinternalError("BroadcastInteraction called by RTIA.");
    }
    Debug(G, pdGendoc) << "exit InteractionSet::broadcastInteraction with time" << std::endl;

    return responses;

} /* end of broadcastInteraction (with time) */

// ----------------------------------------------------------------------------
//! broadcastInteraction without time
Responses InteractionSet::broadcastInteraction(FederateHandle federate_handle,
                                               InteractionClassHandle interaction_handle,
                                               const std::vector<ParameterHandle>& parameter_list,
                                               const std::vector<ParameterValue_t>& value_list,
                                               uint16_t list_size,
                                               const RTIRegion* region,
                                               const std::string& the_tag)
{
    Debug(G, pdGendoc) << "enter InteractionSet::broadcastInteraction without time" << std::endl;

    Responses responses;

    // It may throw InteractionClassNotDefined.
    //InteractionClassHandle currentClass = interaction_handle ;
    Interaction* theInteraction = getObjectFromHandle(interaction_handle);

    InteractionBroadcastList* ibList;
    std::tie(ibList, responses)
        = theInteraction->sendInteraction(federate_handle, parameter_list, value_list, list_size, region, the_tag);

    // Pass the Message(and its BroadcastList) to the Parent Classes.
    if (ibList != NULL) {
        while (theInteraction->getSuperclass() != 0) {
            theInteraction = getObjectFromHandle(theInteraction->getSuperclass());
            auto resp = theInteraction->broadcastInteractionMessage(ibList, region);
            responses.insert(
                std::end(responses), make_move_iterator(std::begin(resp)), make_move_iterator(std::end(resp)));
        }
        delete ibList;
    }
    else {
        // BroadcastInteraction should not be called on the RTIA(see IsReady)
        throw RTIinternalError("BroadcastInteraction called by RTIA.");
    }
    Debug(G, pdGendoc) << "exit InteractionSet::broadcastInteraction without time" << std::endl;

    return responses;
} /* end of broadcastInteraction (WITHOUT time) */

// ----------------------------------------------------------------------------
//! Return the interaction handle associated to name.
InteractionClassHandle InteractionSet::getInteractionClassHandle(const std::string& class_name) const
{
    return getHandleFromName(class_name);
} /* end of getInteractionClassHandle */

// ----------------------------------------------------------------------------
//! Return the interaction name associated to handle.
const std::string& InteractionSet::getInteractionClassName(InteractionClassHandle the_handle) const

{
    return getNameFromHandle(the_handle);
} /* end of getInteractionClassName */

// ----------------------------------------------------------------------------
//! Return the parameter handle associated to name and class handle.
ParameterHandle InteractionSet::getParameterHandle(const std::string& the_name, InteractionClassHandle the_class)

{
    // It may throw InteractionClassNotDefined
    Interaction* interaction = getObjectFromHandle(the_class);
    return interaction->getParameterHandle(the_name);
}

// ----------------------------------------------------------------------------
//! Return the parameter name associated to handle and class handle.
const std::string& InteractionSet::getParameterName(ParameterHandle the_handle, InteractionClassHandle the_class)

{
    // It may throw InteractionClassNotDefined
    Interaction* interaction = getObjectFromHandle(the_class);
    return interaction->getParameterName(the_handle);
}

// ----------------------------------------------------------------------------
/*! Return no exception if the Interaction is valid for a SendInteraction, but
  do not broadcast it.(to be used on the RTIA for pre-checking).
 */
void InteractionSet::isReady(FederateHandle federate_handle,
                             InteractionClassHandle the_interaction,
                             const std::vector<ParameterHandle>& param_array,
                             uint16_t param_array_size)

{
    // It may throw InteractionClassNotDefined
    Interaction* interaction = getObjectFromHandle(the_interaction);
    interaction->isReady(federate_handle, param_array, param_array_size);
}

// ----------------------------------------------------------------------------
//! killFederate.
void InteractionSet::killFederate(FederateHandle the_federate) noexcept
{
    handled_const_iterator i;

    for (i = handled_begin(); i != handled_end(); ++i) {
        // Call KillFederate on that class until it returns NULL.
        i->second->killFederate(the_federate);
    }

} /* end of killFederate */

// ----------------------------------------------------------------------------
//! publish.
void InteractionSet::publish(FederateHandle federate_handle, InteractionClassHandle interaction_handle, bool publish)

{
    // It may throw InteractionClassNotDefined
    Interaction* interaction = getObjectFromHandle(interaction_handle);
    if (publish)
        interaction->publish(federate_handle);
    else
        interaction->unpublish(federate_handle);
}

// ----------------------------------------------------------------------------
//! subscribe.
void InteractionSet::subscribe(FederateHandle federate_handle,
                               InteractionClassHandle interaction_handle,
                               const RTIRegion* region,
                               bool subscribe)

{
    // It may throw InteractionClassNotDefined
    Interaction* interaction = getObjectFromHandle(interaction_handle);
    if (subscribe)
        interaction->subscribe(federate_handle, region);
    else
        interaction->unsubscribe(federate_handle, region);
}

} // namespace certi
