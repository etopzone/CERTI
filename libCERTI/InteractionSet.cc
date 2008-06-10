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
// $Id: InteractionSet.cc,v 3.21 2008/06/10 13:41:46 rousse Exp $
// ----------------------------------------------------------------------------

#include "Interaction.hh"
#include "InteractionSet.hh"
#include "InteractionBroadcastList.hh"
#include "PrettyDebug.hh"

#include <iostream>

using std::cout ;
using std::endl ;
using std::list ;

namespace certi {

static pdCDebug D("INTERACTIONSET", "(InterSet) - ");
static PrettyDebug G("GENDOC",__FILE__) ;

// ----------------------------------------------------------------------------
//! addClass.
/*! No memory is allocated, please don't free the pointed object.
 */
void
InteractionSet::addClass(Interaction *the_class)
{
    D.Out(pdInit, "Adding new interaction class %d, ", the_class->getHandle());

    the_class->server = server ;

    // BUG: We must verify that no other class with the same name does already
    // exists. Make a call to getClassHandle.
    push_front(the_class);
}

// ----------------------------------------------------------------------------
//! broadcastInteraction with time.
void
InteractionSet::broadcastInteraction(FederateHandle federate_handle,
                                     InteractionClassHandle interaction_handle,
                                     std::vector <ParameterHandle> &parameter_list,
                                     ParameterLengthPair *value_list,
                                     UShort list_size,
                                     FederationTime the_time,
				     const RTIRegion *region,
                                     const char *the_tag)
    throw (FederateNotPublishing,
           InteractionClassNotDefined,
           InteractionParameterNotDefined,
           RTIinternalError)
{

    G.Out(pdGendoc,"enter InteractionSet::broadcastInteraction with time") ;

    // It may throw InteractionClassNotDefined.
    //InteractionClassHandle currentClass = interaction_handle ;
    Interaction *theInteraction = getByHandle(interaction_handle);

    InteractionBroadcastList *ibList ;
    ibList = theInteraction->sendInteraction(federate_handle,
                                             parameter_list,
                                             value_list,
                                             list_size,
                                             the_time,
					     region,
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

    G.Out(pdGendoc,"exit InteractionSet::broadcastInteraction with time") ;

}

// ----------------------------------------------------------------------------
//! broadcastInteraction without time
void
InteractionSet::broadcastInteraction(FederateHandle federate_handle,
                                     InteractionClassHandle interaction_handle,
                                     std::vector <ParameterHandle> &parameter_list,
                                     ParameterLengthPair *value_list,
                                     UShort list_size,
				     const RTIRegion *region,
                                     const char *the_tag)
    throw (FederateNotPublishing,
           InteractionClassNotDefined,
           InteractionParameterNotDefined,
           RTIinternalError)
{

    G.Out(pdGendoc,"enter InteractionSet::broadcastInteraction without time") ;

    // It may throw InteractionClassNotDefined.
    //InteractionClassHandle currentClass = interaction_handle ;
    Interaction *theInteraction = getByHandle(interaction_handle);

    InteractionBroadcastList *ibList ;
    ibList = theInteraction->sendInteraction(federate_handle,
                                             parameter_list,
                                             value_list,
                                             list_size,
					     region,
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

    G.Out(pdGendoc,"exit InteractionSet::broadcastInteraction without time") ;

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
    child->parent = parent->getHandle();

    // Transfert security level.
    child->setLevelId(parent->getLevelId());

    // Register son to parent.
    parent->children.push_front(child->getHandle());

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
        if ((*i)->getHandle() == the_handle)
            return (*i);
    }

    throw InteractionClassNotDefined("");
}

// ----------------------------------------------------------------------------
//! Return the interaction handle associated to name.
InteractionClassHandle
InteractionSet::getInteractionClassHandle(const char *the_name)
    throw (NameNotFound, RTIinternalError)
{
    if (the_name == NULL)
        throw RTIinternalError("");

    list<Interaction *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        if (strcmp((*i)->getCName(), the_name) == 0)
            return (*i)->getHandle();
    }

    throw NameNotFound("");
}

// ----------------------------------------------------------------------------
//! Return the interaction name associated to handle.
const char *
InteractionSet::getInteractionClassName(InteractionClassHandle the_handle)
    throw (InteractionClassNotDefined, RTIinternalError)
{
    list<Interaction *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        if ((*i)->getHandle() == the_handle)
            return (*i)->getCName();
    }

    throw InteractionClassNotDefined("");
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
        throw RTIinternalError("");

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
                        std::vector <ParameterHandle> &param_array,
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
    if (publish)
	interaction->publish(federate_handle);
    else 
	interaction->unpublish(federate_handle);
}

// ----------------------------------------------------------------------------
//! subscribe.
void
InteractionSet::subscribe(FederateHandle federate_handle,
                          InteractionClassHandle interaction_handle,
			  const RTIRegion *region,
                          bool subscribe)
    throw (FederateNotSubscribing,
           InteractionClassNotDefined,
           RTIinternalError,
           SecurityError)
{
    // It may throw InteractionClassNotDefined
    Interaction *interaction = getByHandle(interaction_handle);
    if (subscribe)
	interaction->subscribe(federate_handle, region);
    else
	interaction->unsubscribe(federate_handle, region);
}

} // namespace certi

// $Id: InteractionSet.cc,v 3.21 2008/06/10 13:41:46 rousse Exp $
