// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003, 2005  ONERA
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
// $Id: InteractionSet.hh,v 3.20 2008/11/01 19:19:34 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_INTERACTION_SET_HH
#define _CERTI_INTERACTION_SET_HH

// forward declaration
namespace certi {
	class Interaction;
}  // namespace certi

// CERTI headers
#include "certi.hh"
#include "SecurityServer.hh"

#include <string>
#include <map>

namespace certi {

class CERTI_EXPORT InteractionSet
{

public:
	InteractionSet(SecurityServer *the_server, bool isRootClassSet=false);
	~InteractionSet();

	/**
	 * Add an interaction class to the set.
	 * @pre theClass should be non NULL
	 * @param[in] theClass the object class to be added
	 */
	void addClass(Interaction *theClass);

	/**
	 * Build inheritance relation between two objects class.
	 * Build a Parent-Child relation between two classe, by setting the
     * Child's Parent handle, and registering the Child in the Parent's SonSet.
     * Also copy all Parent's Attributes/Parameter in the Child Class.
	 * @param[in,out] child the future child object class
	 * @param[in,out] parent the parent object class
	 * @post the child and parent object classes are linked
	 *       with inheritance relationship.
	 */
	void buildParentRelation(Interaction *child, Interaction *parent);

	/**
	 *  Print the Interactions tree to the standard output.
	 */
	void display() const ;

	// --------------------------
	// -- RTI Support Services --
	// --------------------------

	/**
	 * Return the interaction handle associated to name.
	 * @param[in] class_name the name of the interaction for whose handle is requested
	 */
	InteractionClassHandle
	getInteractionClassHandle(const std::string& class_name) const
	throw (NameNotFound);

	std::string
	getInteractionClassName(InteractionClassHandle the_handle) const
	throw (InteractionClassNotDefined);

	ParameterHandle
	getParameterHandle(const char *the_name,
			InteractionClassHandle the_class)
	throw (NameNotFound,
			InteractionClassNotDefined,
			RTIinternalError);

	const std::string&
	getParameterName(ParameterHandle the_handle,
			InteractionClassHandle the_class)
	throw (InteractionParameterNotDefined,
			InteractionClassNotDefined,
			RTIinternalError);

	void killFederate(FederateHandle the_federate)
	throw ();

	/**
	 *  Return interaction associated to handle.
	 *  @param[in] the_handle
	 *  @return the interaction associated to handle
	 *  @throw InteractionClassNotDefined if the handle does not correspond to
	 *         any interactrion class
	 *  @throw RTIinternalError
	 */
	Interaction* getByHandle(InteractionClassHandle the_handle) const
	throw (InteractionClassNotDefined, RTIinternalError);

	// ----------------------------------
	// -- Interaction Class Management --
	// ----------------------------------
	void publish(FederateHandle the_federate_handle,
			InteractionClassHandle the_interaction_handle,
			bool publish)
	throw (FederateNotPublishing,
			InteractionClassNotDefined,
			RTIinternalError,
			SecurityError);

	void subscribe(FederateHandle the_federate_handle,
			InteractionClassHandle the_interaction_handle,
			const RTIRegion *,
			bool subscribe)
	throw (FederateNotSubscribing,
			InteractionClassNotDefined,
			RTIinternalError,
			SecurityError);

	// -------------------------------------
	// -- Interaction Instance Management --
	// -------------------------------------
	void isReady(FederateHandle theFederateHandle,
			InteractionClassHandle theInteraction,
			std::vector <ParameterHandle> &paramArray,
			UShort paramArraySize)
	throw (FederateNotPublishing,
			InteractionClassNotDefined,
			InteractionParameterNotDefined,
			RTIinternalError);

	void broadcastInteraction(FederateHandle theFederateHandle,
			InteractionClassHandle theInteractionHandle,
			std::vector <ParameterHandle> &theParameterList,
			std::vector <ParameterValue_t> &theValueList,
			UShort theListSize,
			FederationTime theTime,
			const RTIRegion *,
			const char *theTag)
	throw (FederateNotPublishing,
			InteractionClassNotDefined,
			InteractionParameterNotDefined,
			RTIinternalError);

	void broadcastInteraction(FederateHandle theFederateHandle,
			InteractionClassHandle theInteractionHandle,
			std::vector <ParameterHandle> &theParameterList,
			std::vector <ParameterValue_t> &theValueList,
			UShort theListSize,
			const RTIRegion *,
			const char *theTag)
	throw (FederateNotPublishing,
			InteractionClassNotDefined,
			InteractionParameterNotDefined,
			RTIinternalError);

	typedef std::map<InteractionClassHandle,Interaction*,std::less<InteractionClassHandle> > Handle2InteractionClassMap_t;
	typedef std::map<std::string,Interaction*,std::less<std::string> > Name2InteractionClassMap_t;
	typedef Handle2InteractionClassMap_t::const_iterator handledIC_const_iterator;
	typedef Name2InteractionClassMap_t::const_iterator namedIC_const_iterator;

	namedIC_const_iterator NamedBegin() const {
		return ICFromName.begin();
	}

	namedIC_const_iterator NamedEnd() const {
		return ICFromName.end();
	}

	const size_t size() {return ICFromName.size();}

private:

	Handle2InteractionClassMap_t ICFromHandle;
	Name2InteractionClassMap_t   ICFromName;
	bool                         isRootClassSet;
	SecurityServer *server ;
};

} // namespace certi

#endif // _CERTI_INTERACTION_SET_HH

// $Id: InteractionSet.hh,v 3.20 2008/11/01 19:19:34 erk Exp $
