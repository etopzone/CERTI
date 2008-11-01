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
// $Id: InteractionSet.cc,v 3.25 2008/11/01 19:19:35 erk Exp $
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


InteractionSet::InteractionSet(SecurityServer *security_server, bool isRootClassSet)
: isRootClassSet(isRootClassSet), server(security_server) {
}

InteractionSet::~InteractionSet() {
	/* clear name map */
	ICFromName.clear();
	/*
	 * If we are Root ClassSet (the class set owned by RootObject)
	 *    we delete the content
	 * If not we only clear the map in order to avoid double deletion.
	 *
	 * FIXME EN: this is a trick in order because we do not
	 *           really maintain a tree of ObjectClass in order
	 *           to support flat interaction class name
	 *           ("Boule" instead of "Bille.Boule")
	 *           We may get rid of this as soon as we want to support
	 *           same name for interaction class in different branch of the tree.
	 */
	if (isRootClassSet) {
		while (!ICFromHandle.empty()) {
			delete (ICFromHandle.begin()->second);
			ICFromHandle.erase(ICFromHandle.begin());
		}
	}
	else {
		ICFromHandle.clear();
	}
} /* end of ~InteractionSet */

void
InteractionSet::addClass(Interaction *newClass) {

	Name2InteractionClassMap_t::iterator findit;
	std::stringstream                       msg;

	D.Out(pdInit, "Adding new interaction class %d, ", newClass->getHandle());

	/* link to server */
	newClass->server = server ;

	/*
	 * Check whether addition of this interaction class
	 * will generate a name collision or not.
	 * i.e. we may not add an object class of the SAME
	 * name to the interaction class set
	 */
	findit = ICFromName.find(newClass->getName());
	if (findit != ICFromName.end()) {
		msg << "Name collision another interaction class named <"
		<< newClass->getName()
		<< "> with handle <"
		<< findit->second->getHandle()
		<< "> was found when trying to add identically named object class with handle <"
		<< newClass->getHandle();
		throw RTIinternalError(msg.str().c_str());
	}
	/* store ref to new class in ObjectClass from Handle Map */
	ICFromHandle[newClass->getHandle()] = newClass;
	/* store ref to new class in ObjectClass from Name Map */
	ICFromName[newClass->getName()] = newClass;

} /* end of addClass */

// ----------------------------------------------------------------------------
//! broadcastInteraction with time.
void
InteractionSet::broadcastInteraction(FederateHandle federate_handle,
		InteractionClassHandle interaction_handle,
		std::vector <ParameterHandle> &parameter_list,
		std::vector <ParameterValue_t> &value_list,
		UShort list_size,
		FederationTime the_time,
		const RTIRegion *region,
		const char *the_tag)
throw (FederateNotPublishing,
		InteractionClassNotDefined,
		InteractionParameterNotDefined,
		RTIinternalError) {

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

} /* end of broadcastInteraction (with time) */

// ----------------------------------------------------------------------------
//! broadcastInteraction without time
void
InteractionSet::broadcastInteraction(FederateHandle federate_handle,
		InteractionClassHandle interaction_handle,
		std::vector <ParameterHandle> &parameter_list,
		std::vector <ParameterValue_t> &value_list,
		UShort list_size,
		const RTIRegion *region,
		const char *the_tag)
throw (FederateNotPublishing,
		InteractionClassNotDefined,
		InteractionParameterNotDefined,
		RTIinternalError) {

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
} /* end of broadcastInteraction (WITHOUT time) */

void
InteractionSet::buildParentRelation(Interaction *child, Interaction *parent)
{
	// Register parent to son.
	child->setSuperclass(parent->getHandle());

	// Transfert security level.
	child->setLevelId(parent->getLevelId());

	// Register son to parent.
	parent->addSubclass(child);

	// Copy parent Attribute into child class.
	parent->addParametersToChild(child);
} /* end of buildParentRelation */

void
InteractionSet::display() const
{
	cout << " Interactions :" << endl ;
	handledIC_const_iterator i;
	for (i = ICFromHandle.begin(); i != ICFromHandle.end(); ++i) {
		i->second->display();
	}
} /* end of display */

Interaction*
InteractionSet::getByHandle(InteractionClassHandle the_handle) const
throw (InteractionClassNotDefined, RTIinternalError)
{
	std::stringstream msg;

	handledIC_const_iterator iter;

	iter = ICFromHandle.find(the_handle);

	if (iter != ICFromHandle.end()) {
		return iter->second;
	} else {
		msg << "Unknown Object Class Handle <" << the_handle << ">";
		D.Out(pdExcept, "Unknown Object Class Handle %d .", the_handle);
		throw InteractionClassNotDefined(msg.str().c_str());
	}
} /* end of getByHandle */

// ----------------------------------------------------------------------------
//! Return the interaction handle associated to name.
InteractionClassHandle
InteractionSet::getInteractionClassHandle(const std::string& class_name) const
throw (NameNotFound)  {

	std::string                 currentName;
	std::string                 remainingName;
	InteractionClassHandle      currentHandle;
	Interaction*                currentClass;
	InteractionSet const*       currentClassSet;
	namedIC_const_iterator      iter;

	currentClassSet = this;
	remainingName = class_name;
	D.Out(pdDebug,"Looking for interaction class <%s>",class_name.c_str());
	/*
	 * If the name is qualified (a.k.a. hierarchical name)
	 * like "Bille.Boule"
	 * then iterate through subClass in order to reach the leaf
	 * "unqualified name"
	 */
	while (Named::isQualifiedClassName(remainingName)) {
		/*
		 * The first current should be the name of
		 * of a subclass of the current ClassSet
		 */
		currentName = Named::getNextClassName(remainingName);
		D.Out(pdDebug,"Now Looking for interaction class <%s>",currentName.c_str());
		/*
		 * Get the handle of the subclass
		 * NOTE that we won't recurse more than once here
		 * since the provided 'currentName' is not qualified
		 * 'by design'
		 * The recursive deepness is at most 2.
		 */
		currentHandle = currentClassSet->getInteractionClassHandle(currentName);
		/* Get the corresponding class object */
		currentClass = currentClassSet->getByHandle(currentHandle);
		/* now update currentClassSet */
		currentClassSet = currentClass->getSubClasses();
	}

	/*
	 * Now the current classClassSet should be a leaf
	 * so that we can search in the
	 */
	iter = currentClassSet->ICFromName.find(remainingName);

	if (iter != currentClassSet->ICFromName.end()) {
		G.Out(pdGendoc,"exit ObjectClassSet::getObjectClassHandle");
		return iter->second->getHandle();
	} else {
		G.Out(pdGendoc,"exit ObjectClassSet::getObjectClassHandle on NameNotFound");
		throw NameNotFound(class_name.c_str());
	}

} /* end of getInteractionClassHandle */

// ----------------------------------------------------------------------------
//! Return the interaction name associated to handle.
std::string
InteractionSet::getInteractionClassName(InteractionClassHandle the_handle) const
throw (InteractionClassNotDefined)
{
	return getByHandle(the_handle)->getName();
} /* end of getInteractionClassName */

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
const std::string&
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
throw () {
    Handle2InteractionClassMap_t::iterator i;

    for (i = ICFromHandle.begin(); i != ICFromHandle.end(); ++i) {
        // Call KillFederate on that class until it returns NULL.
        i->second->killFederate(the_federate);
    }

} /* end of killFederate */

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

// $Id: InteractionSet.cc,v 3.25 2008/11/01 19:19:35 erk Exp $
