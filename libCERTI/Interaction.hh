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
// ----------------------------------------------------------------------------

#ifndef _CERTI_INTERACTION_HH
#define _CERTI_INTERACTION_HH

// forward declaration
namespace certi {
class InteractionBroadcastList;
class InteractionSet;
}  // namespace certi

// CERTI headers
#include "certi.hh"
#include "SecurityServer.hh"
#include "Parameter.hh"
#include "Subscribable.hh"

#include <map>
#include <set>
#include <string>

namespace certi {

/**
 * OMT Interaction class.
 * It also contains:
 *  <ul>
 *    <li> the set of subclasses. </li>
 *    <li> the instance list from this class. </li>
 *  </ul>
 */
class CERTI_EXPORT Interaction : public Subscribable
{
public:

    /**
     * The type of the handle of this class.
     */
    typedef InteractionClassHandle handle_t;

    /**
     * The type of the exception to when such object is not found
     */
    typedef InteractionClassNotDefined ObjectNotDefinedException;

    /**
     * The type for the parameter list.
     */
    typedef std::map<ParameterHandle, Parameter*> HandleParameterMap;

    /**
     * Build an interaction class object.
     * @param[in] theName the name of the interaction class
     * @param[in] handle the handle of this class
     * @param[in] transport the transport type
     * @param[in] order the receive order type
     */
    Interaction(const std::string& name, InteractionClassHandle handle, TransportType transport, OrderType order);
    /**
     * Destructor.
     */
    virtual ~Interaction();

    void setSpace(SpaceHandle);
    SpaceHandle getSpace();

    void setHandle(InteractionClassHandle h) { handle = h ; }
    InteractionClassHandle getHandle() const { return handle ;}

    /**
     * Get the super class handle.
     * @return the super class handle
     */
    InteractionClassHandle getSuperclass() const { return superClass ;};

    /**
     * Add a subclass to this interaction class.
     * @param[in,out] child the interaction to add as a sub class
     */
    void addSubClass(Interaction *child);

    /**
     * Retrieve a sub class by its name.
     * @param[in] subClassName the name of the subclass
     * @return the sub class object class.
     */
    Interaction* getSubClassByName(const std::string& subClassName);

    /**
     * Get the whole set of subclasses.
     */
    InteractionSet* getSubClasses() {return subClasses;};

    /**
     * Add a parameter to an interaction class.
     * @param[in] parameter,
     * @param[in] is_inherited, true if it is an inherited parameter
     * @return the new parameter handle
     */
    ParameterHandle addParameter(Parameter *parameter,
                                 bool is_inherited = false);

    void display() const ;

    // -- Security Methods --
    void checkFederateAccess(FederateHandle the_federate,
            const std::string& reason) const
    throw (SecurityError);

    SecurityLevelID getSecurityLevelId() const { return id ; };
    void setSecurityLevelId(SecurityLevelID NewLevelID);

    // -- Publication and Subscription --
    void publish(FederateHandle)
    throw (FederateNotPublishing, RTIinternalError, SecurityError);

    void unpublish(FederateHandle)
    throw (FederateNotPublishing, RTIinternalError, SecurityError);

    // -- RTI Support Services --
    ParameterHandle getParameterHandle(const std::string&) const
    throw (NameNotFound, RTIinternalError);

    /**
     * Get interaction parameter name from its handle
     * @param[in] the_handle the parameter handle
     * @return the name of the parameter
     */
    const std::string& getParameterName(ParameterHandle the_handle) const
    throw (InteractionParameterNotDefined, RTIinternalError);

    /**
     * Returns true if the Interaction has the parameter with the given handle.
     * @param[in] parameterHandle the parameter handle
     * @return if the Interaction has the parameter
     */
    bool hasParameter(ParameterHandle parameterHandle) const;

    void killFederate(FederateHandle theFederate)
    throw ();

    // -- Transport and Ordering --
    void changeTransportationType(TransportType new_type,
            FederateHandle the_handle)
    throw (FederateNotPublishing, InvalidTransportationHandle, RTIinternalError);

    void changeOrderType(OrderType new_order, FederateHandle the_handle)
    throw (FederateNotPublishing, InvalidOrderingHandle, RTIinternalError);

    // -- Instance Broadcasting --
    void isReady(FederateHandle federate_handle,
            const std::vector <ParameterHandle> &parameter_list,
            uint16_t list_size)
    throw (FederateNotPublishing,
            InteractionParameterNotDefined,
            RTIinternalError);

    InteractionBroadcastList *
    sendInteraction(FederateHandle federate_handle,
            const std::vector <ParameterHandle> &parameter_list,
            const std::vector <ParameterValue_t> &value_list,
            uint16_t list_size,
            FederationTime the_time,
            const RTIRegion *,
            const std::string& the_tag)
    throw (FederateNotPublishing,
            InteractionClassNotDefined,
            InteractionParameterNotDefined,
            RTIinternalError);

    InteractionBroadcastList *
    sendInteraction(FederateHandle federate_handle,
            const std::vector <ParameterHandle> &parameter_list,
            const std::vector <ParameterValue_t> &value_list,
            uint16_t list_size,
            const RTIRegion *,
            const std::string& the_tag)
    throw (FederateNotPublishing,
            InteractionClassNotDefined,
            InteractionParameterNotDefined,
            RTIinternalError);

    void broadcastInteractionMessage(InteractionBroadcastList *, const RTIRegion *);

    /**
     * Getter for the parameter list of the interaction class.
     * param[out] ParameterList_t @see Interaction::HandleParameterMap
     */
    const HandleParameterMap& getHandleParameterMap(void) const { return _handleParameterMap; }

    //! This Object helps to find a TCPLink given a Federate Handle.
    SecurityServer *server ;

    /*! Interaction messages' Transport Type(Reliable, Best Effort),
      Currently not used.
     */
    TransportType transport ;

    //! Interaction message Ordering Type(TSO, FIFO), currently not used.
    OrderType order ;

private:
    /*
     * private default constructor with no code
     * one should not call it.
     */
    Interaction();

    void addInheritedClassParameter(Interaction *new_child);

    InteractionClassHandle handle ; //!< Interaction class handle.

    /**
     * The super class handle.
     * 0 if they aren't any.
     */
    InteractionClassHandle superClass;
    /**
     * The set of interaction classes sub classes.
     */
    InteractionSet* subClasses;

    Parameter *getParameterByHandle(ParameterHandle the_handle) const
    throw (InteractionParameterNotDefined, RTIinternalError);

    void deletePublisher(FederateHandle);
    bool isPublishing(FederateHandle);

    // Attributes
    SecurityLevelID id ; //!< The default Security Level for new parameters
    SpaceHandle space ;

    //! List of this Interaction Class' Parameters.
    HandleParameterMap _handleParameterMap;

    typedef std::set<FederateHandle> PublishersList ;
    PublishersList publishers ;
};

} // namespace

#endif // _CERTI_INTERACTION.HH
