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
// $Id: Interaction.hh,v 3.34 2008/11/02 00:26:41 erk Exp $
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

#include <list>
#include <set>
#include <string>

namespace certi {

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
	 * Build an interaction class object.
	 * @param[in] name the name of the interaction class
	 * @param[in] handle the handle of this class
	 * @param[in] transport the transport type
	 * @param[in] order the receive order type
	 */
    Interaction(const std::string& name, InteractionClassHandle handle, TransportType transport, OrderType order);
    /**
     * Destructor.
     */
    ~Interaction();

    void setSpace(SpaceHandle);
    SpaceHandle getSpace();

    void setHandle(InteractionClassHandle h) { handle = h ; }
    InteractionClassHandle getHandle() const { return handle ;}

	/**
	 * Set the super class (parent class) of this object class;
	 * @param[in] h the handle of the super class.
	 */
	void setSuperclass(InteractionClassHandle h) { superClass = h ; };
	/**
	 * Get the super class handle.
	 * @return the super class handle
	 */
	InteractionClassHandle getSuperclass() const { return superClass ; };
	/**
	 * Add a subclass to this object class.
	 * @param[in] sc the interaction to add as a sub class
	 */
	void addSubclass(Interaction *sc);
	/**
	 * Retrieve a sub class by its name.
	 * @param[in] subClassName the name of the subclass
	 * @return the sub class object class.
	 */
	Interaction* getSubClassByName(const std::string subClassName);
	/**
	 * Get the whole set of subclasses.
	 */
	InteractionSet* getSubClasses() {return subClasses;};

    ParameterHandle addParameter(Parameter *the_parameter,
                                 bool is_inherited = false);

    void addToChild(Interaction *new_child);
    void display() const ;

    // -- Security Methods --
    void checkFederateAccess(FederateHandle the_federate,
                             const char *reason) const
        throw (SecurityError);

    SecurityLevelID getSecurityLevelId() const { return id ; };
    void setSecurityLevelId(SecurityLevelID NewLevelID);

    // -- Publication and Subscription --
    void publish(FederateHandle)
        throw (FederateNotPublishing, RTIinternalError, SecurityError);

    void unpublish(FederateHandle)
        throw (FederateNotPublishing, RTIinternalError, SecurityError);

    // -- RTI Support Services --
    ParameterHandle getParameterHandle(const char *) const
        throw (NameNotFound, RTIinternalError);

    /**
     * Get interaction parameter name from its handle
     * @param[in] the_handle the parameter handle
     * @return the name of the parameter
     */
    const std::string& getParameterName(ParameterHandle the_handle) const
        throw (InteractionParameterNotDefined, RTIinternalError);

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
                 std::vector <ParameterHandle> &parameter_list,
                 UShort list_size)
        throw (FederateNotPublishing,
               InteractionParameterNotDefined,
               RTIinternalError);

    InteractionBroadcastList *
    sendInteraction(FederateHandle federate_handle,
                    std::vector <ParameterHandle> &parameter_list,
                    std::vector <ParameterValue_t> &value_list,
                    UShort list_size,
                    FederationTime the_time,
		    const RTIRegion *,
                    const char *the_tag)
        throw (FederateNotPublishing,
               InteractionClassNotDefined,
               InteractionParameterNotDefined,
               RTIinternalError);

    InteractionBroadcastList *
    sendInteraction(FederateHandle federate_handle,
                    std::vector <ParameterHandle> &parameter_list,
                    std::vector <ParameterValue_t> &value_list,
                    UShort list_size,
		    const RTIRegion *,
                    const char *the_tag)
        throw (FederateNotPublishing,
               InteractionClassNotDefined,
               InteractionParameterNotDefined,
               RTIinternalError);

    void broadcastInteractionMessage(InteractionBroadcastList *, const RTIRegion *);

    //! This Object helps to find a TCPLink given a Federate Handle.
    SecurityServer *server ;
    /**
     * The super class handle.
     * 0 if they aren't any.
     */
    InteractionClassHandle parent ;

    //std::list<InteractionClassHandle> children ;

    UShort depth ;

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
    std::list<Parameter *> parameterSet ;

    typedef std::set<FederateHandle> PublishersList ;
    PublishersList publishers ;
};

} // namespace

#endif // _CERTI_INTERACTION.HH

// $Id: Interaction.hh,v 3.34 2008/11/02 00:26:41 erk Exp $
