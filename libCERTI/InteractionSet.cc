// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: InteractionSet.cc,v 3.7 2003/05/23 09:45:15 breholee Exp $
// ----------------------------------------------------------------------------

#include "InteractionSet.hh"

namespace certi {

static pdCDebug D("INTERACTIONSET", "(InterSet) - ");

// ----------------------------------------------------------------------------
//! addClass.
/*! No memory is allocated, please don't free the pointed object.
 */
void
InteractionSet::addClass(Interaction *the_class)
{
    D.Out(pdInit, "Adding new interaction class %d, ", the_class->handle);

    the_class->server = server ;

    // BUG: We must verify that no other class with the same name does already
    // exists. Make a call to getClassHandle.
    push_front(the_class);
}

// ----------------------------------------------------------------------------
//! broadcastInteraction.
void
InteractionSet::broadcastInteraction(FederateHandle federate_handle,
                                     InteractionClassHandle interaction_handle,
                                     ParameterHandle *parameter_list,
                                     ParameterValue *value_list,
                                     UShort list_size,
                                     FederationTime the_time,
                                     const char *the_tag)
    throw (FederateNotPublishing,
           InteractionClassNotDefined,
           InteractionParameterNotDefined,
           RTIinternalError)
{
    // It may throw InteractionClassNotDefined.
    //InteractionClassHandle currentClass = interaction_handle ;
    Interaction *theInteraction = getByHandle(interaction_handle);

    InteractionBroadcastList *ibList ;
    ibList = theInteraction->sendInteraction(federate_handle,
                                             parameter_list,
                                             value_list,
                                             list_size,
                                             the_time,
                                             the_tag);

    // Pass the Message(and its BroadcastList) to the Parent Classes.
    if (ibList != NULL) {
        //currentClass = theInteraction->parent ;
        //while (CurrentClass != 0) {
        // theInteraction = getByHandle(CurrentClass);
        // theInteraction->broadcastInteractionMessage(List);
        // CurrentClass = theInteraction->Father ;
        //}
        delete ibList ;
    }
    else
        // BroadcastInteraction should not be called on the RTIA(see IsReady)
        throw RTIinternalError("BroadcastInteraction called by RTIA.");
}

// ----------------------------------------------------------------------------
/*! Build a Parent-Child relation between two object class, by setting the
  Child's Parent handle, and registering the Child in the Parent's SonSet.
  Also copy all Parent's Attributes in the Child Class.
*/
void
InteractionSet::buildParentRelation(Interaction *child, Interaction *parent)
{
    // Register parent to son.
    child->parent = parent->handle ;

    // Transfert security level.
    child->setLevelId(parent->getLevelId());

    // Register son to parent.
    parent->children.push_front(child->handle);

    // Copy parent Attribute into child class.
    parent->addParametersToChild(child);
}

// ----------------------------------------------------------------------------
//! interactionSet.
/*! 'security_server' can be NULL on the RTIA.
 */
InteractionSet::InteractionSet(SecurityServer *security_server)
    : list<Interaction *>(), server(security_server)
{
}

// ----------------------------------------------------------------------------
//! Destructor (frees list).
InteractionSet::~InteractionSet()
{
    while (!empty()) {
        delete front();
        pop_front();
    }
}

// ----------------------------------------------------------------------------
//! Print the Interactions tree to the standard output.
void
InteractionSet::display() const
{
    cout << " Interactions :" << endl ;

    list<Interaction *>::const_iterator i = begin();
    for (; i != end(); i++) {
        (*i)->display();
    }
}

// ----------------------------------------------------------------------------
//! Return interaction associated to handle.
Interaction *
InteractionSet::getByHandle(InteractionClassHandle the_handle)
    throw (InteractionClassNotDefined,
           RTIinternalError)
{
    list<Interaction *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        if ((*i)->handle == the_handle)
            return (*i);
    }

    throw InteractionClassNotDefined();
}

// ----------------------------------------------------------------------------
//! Return the interaction handle associated to name.
InteractionClassHandle
InteractionSet::getInteractionClassHandle(const char *the_name)
    throw (NameNotFound, RTIinternalError)
{
    if (the_name == NULL)
        throw RTIinternalError();

    list<Interaction *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        if (strcmp((*i)->getName(), the_name) == 0)
            return (*i)->handle ;
    }

    throw NameNotFound();
}

// ----------------------------------------------------------------------------
//! Return the interaction name associated to handle.
const char *
InteractionSet::getInteractionClassName(InteractionClassHandle the_handle)
    throw (InteractionClassNotDefined, RTIinternalError)
{
    list<Interaction *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        if ((*i)->handle == the_handle)
            return (*i)->getName();
    }

    throw InteractionClassNotDefined();
}

// ----------------------------------------------------------------------------
//! Return the parameter handle associated to name and class handle.
ParameterHandle
InteractionSet::getParameterHandle(const char *the_name,
                                   InteractionClassHandle the_class)
    throw (NameNotFound,
           InteractionClassNotDefined,
           RTIinternalError)
{
    if (the_name == NULL)
        throw RTIinternalError();

    // It may throw InteractionClassNotDefined
    Interaction *interaction = getByHandle(the_class);
    return interaction->getParameterHandle(the_name);
}

// ----------------------------------------------------------------------------
//! Return the parameter name associated to handle and class handle.
const char *
InteractionSet::getParameterName(ParameterHandle the_handle,
                                 InteractionClassHandle the_class)
    throw (InteractionParameterNotDefined,
           InteractionClassNotDefined,
           RTIinternalError)
{
    // It may throw InteractionClassNotDefined
    Interaction *interaction = getByHandle(the_class);
    return interaction->getParameterName(the_handle);
}

// ----------------------------------------------------------------------------
/*! Return no exception if the Interaction is valid for a SendInteraction, but
  do not broadcast it.(to be used on the RTIA for pre-checking).
*/
void
InteractionSet::isReady(FederateHandle federate_handle,
                        InteractionClassHandle the_interaction,
                        ParameterHandle *param_array,
                        UShort param_array_size)
    throw (FederateNotPublishing,
           InteractionClassNotDefined,
           InteractionParameterNotDefined,
           RTIinternalError)
{
    // It may throw InteractionClassNotDefined
    Interaction *interaction = getByHandle(the_interaction);
    interaction->isReady(federate_handle, param_array, param_array_size);
}

// ----------------------------------------------------------------------------
//! killFederate.
void
InteractionSet::killFederate(FederateHandle the_federate)
    throw ()
{
    list<Interaction *>::iterator i ;
    for (i = begin(); i != end(); i++) {
        (*i)->killFederate(the_federate);
    }
}

// ----------------------------------------------------------------------------
//! publish.
void
InteractionSet::publish(FederateHandle federate_handle,
                        InteractionClassHandle interaction_handle,
                        bool publish)
    throw (FederateNotPublishing,
           InteractionClassNotDefined,
           RTIinternalError,
           SecurityError)
{
    // It may throw InteractionClassNotDefined
    Interaction *interaction = getByHandle(interaction_handle);
    interaction->publish(publish, federate_handle);
}

// ----------------------------------------------------------------------------
//! subscribe.
void
InteractionSet::subscribe(FederateHandle federate_handle,
                          InteractionClassHandle interaction_handle,
                          bool subscribe)
    throw (FederateNotSubscribing,
           InteractionClassNotDefined,
           RTIinternalError,
           SecurityError)
{
    // It may throw InteractionClassNotDefined
    Interaction *interaction = getByHandle(interaction_handle);
    interaction->subscribe(subscribe, federate_handle);
}

} // namespace certi

// $Id: InteractionSet.cc,v 3.7 2003/05/23 09:45:15 breholee Exp $
