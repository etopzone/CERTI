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
// ----------------------------------------------------------------------------

#ifndef _CERTI_INTERACTION_SET_HH
#define _CERTI_INTERACTION_SET_HH

// CERTI headers
#include "Interaction.hh"
#include "SecurityServer.hh"
#include "TreeNamedAndHandledSet.hh"
#include <include/certi.hh>

// System headers
#include <map>
#include <string>

namespace certi {

class CERTI_EXPORT InteractionSet : public TreeNamedAndHandledSet<Interaction> {
public:
    InteractionSet(SecurityServer* the_server, bool isRootClassSet = false);
    ~InteractionSet();

    /**
	 * Add an interaction class to the set.
	 * @pre theClass should be non NULL
	 * @param[in] theClass the object class to be added
	 * @param[in] parentClass the parent class (may be NULL)
	 */
    void addClass(Interaction* theClass, Interaction* parentClass);

    // --------------------------
    // -- RTI Support Services --
    // --------------------------

    /**
	 * Return the interaction class handle corresponding to the provided name.
	 * @param[in] class_name the name of the interaction whose handle is requested
	 * @return the interaction class handle
	 */
    InteractionClassHandle getInteractionClassHandle(const std::string& class_name) const;

    /**
	 * Return the interaction class name correspondig to the provided handle.
	 * @param[in] the_handle the handle of the interaction whose name is requested
	 * @return the interaction class name
	 */
    const std::string& getInteractionClassName(InteractionClassHandle the_handle) const;

    ParameterHandle getParameterHandle(const std::string& the_name, InteractionClassHandle the_class);

    const std::string& getParameterName(ParameterHandle the_handle, InteractionClassHandle the_class);

    void killFederate(FederateHandle the_federate) noexcept;

    // ----------------------------------
    // -- Interaction Class Management --
    // ----------------------------------
    void publish(FederateHandle the_federate_handle, InteractionClassHandle the_interaction_handle, bool publish);

    void subscribe(FederateHandle the_federate_handle,
                   InteractionClassHandle the_interaction_handle,
                   const RTIRegion*,
                   bool subscribe);

    // -------------------------------------
    // -- Interaction Instance Management --
    // -------------------------------------
    void isReady(FederateHandle theFederateHandle,
                 InteractionClassHandle theInteraction,
                 const std::vector<ParameterHandle>& paramArray,
                 uint16_t paramArraySize);

    Responses broadcastInteraction(FederateHandle theFederateHandle,
                                   InteractionClassHandle theInteractionHandle,
                                   const std::vector<ParameterHandle>& theParameterList,
                                   const std::vector<ParameterValue_t>& theValueList,
                                   uint16_t theListSize,
                                   FederationTime theTime,
                                   const RTIRegion*,
                                   const std::string& theTag);

    Responses broadcastInteraction(FederateHandle theFederateHandle,
                                   InteractionClassHandle theInteractionHandle,
                                   const std::vector<ParameterHandle>& theParameterList,
                                   const std::vector<ParameterValue_t>& theValueList,
                                   uint16_t theListSize,
                                   const RTIRegion*,
                                   const std::string& theTag);

private:
    SecurityServer* server;
};

} // namespace certi

#endif // _CERTI_INTERACTION_SET_HH
