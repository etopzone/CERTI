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
// $Id: Interaction.hh,v 3.20 2005/04/09 15:17:31 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_INTERACTION_HH
#define _CERTI_INTERACTION_HH

#include "certi.hh"
#include "SecurityServer.hh"
#include "Parameter.hh"
#include "Subscribable.hh"
#include "InteractionBroadcastList.hh"

#include <list>
#include <set>
#include <string>

namespace certi {

class Interaction : public Subscribable
{
public:
    Interaction();
    ~Interaction();

    const char *getName() const ;
    void setName(const char *new_name)
        throw (ValueLengthExceeded, RTIinternalError);

    void setSpace(SpaceHandle);
    SpaceHandle getSpace();

    void setHandle(InteractionClassHandle h) { handle = h ; }
    InteractionClassHandle getHandle() const { return handle ; }

    ParameterHandle addParameter(Parameter *the_parameter,
                                 bool is_inherited = false);

    void addParametersToChild(Interaction *new_child);
    void display() const ;

    // -- Security Methods --
    void checkFederateAccess(FederateHandle the_federate,
                             const char *reason) const
        throw (SecurityError);

    SecurityLevelID getLevelId() const { return id ; };
    void setLevelId(SecurityLevelID NewLevelID);

    // -- Publication and Subscription --
    void publish(FederateHandle)
        throw (FederateNotPublishing, RTIinternalError, SecurityError);

    void unpublish(FederateHandle)
        throw (FederateNotPublishing, RTIinternalError, SecurityError);
    
    // -- RTI Support Services --
    ParameterHandle getParameterHandle(const char *) const
        throw (NameNotFound, RTIinternalError);

    const char *getParameterName(ParameterHandle) const
        throw (InteractionParameterNotDefined, RTIinternalError);

    void killFederate(FederateHandle theFederate)
        throw ();

    // -- Transport and Ordering --
    void changeTransportationType(TransportType new_type,
                                  FederateHandle the_handle)
        throw (FederateNotPublishing, InvalidTransportType, RTIinternalError);

    void changeOrderType(OrderType new_order, FederateHandle the_handle)
        throw (FederateNotPublishing, InvalidOrderType, RTIinternalError);

    // -- Instance Broadcasting --
    void isReady(FederateHandle federate_handle,
                 ParameterHandle *parameter_list,
                 UShort list_size)
        throw (FederateNotPublishing,
               InteractionParameterNotDefined,
               RTIinternalError);

    InteractionBroadcastList *
    sendInteraction(FederateHandle federate_handle,
                    ParameterHandle *parameter_list,
                    ParameterValue *value_list,
                    UShort list_size,
                    FederationTime the_time,
		    const RTIRegion *,
                    const char *the_tag)
        throw (FederateNotPublishing,
               InteractionClassNotDefined,
               InteractionParameterNotDefined,
               RTIinternalError);

    void broadcastInteractionMessage(InteractionBroadcastList *, const RTIRegion *);

    //! This Object helps to find a TCPLink given a Federate Handle.
    SecurityServer *server ;
    InteractionClassHandle parent ;
    std::list<InteractionClassHandle> children ;
    UShort depth ;

    /*! Interaction messages' Transport Type(Reliable, Best Effort),
      Currently not used.
    */
    TransportType transport ;

    //! Interaction message Ordering Type(TSO, FIFO), currently not used.
    OrderType order ;

private:
    InteractionClassHandle handle ; //!< Interaction class handle.

    Parameter *getParameterByHandle(ParameterHandle the_handle) const
        throw (InteractionParameterNotDefined, RTIinternalError);

    void deletePublisher(FederateHandle);
    bool isPublishing(FederateHandle);

    // Attributes    
    std::string name ;
    SecurityLevelID id ; //!< The default Security Level for new parameters
    SpaceHandle space ;

    //! List of this Interaction Class' Parameters.
    std::list<Parameter *> parameterSet ;

    typedef std::set<FederateHandle> PublishersList ;
    PublishersList publishers ;
};

} // namespace

#endif // _CERTI_INTERACTION.HH

// $Id: Interaction.hh,v 3.20 2005/04/09 15:17:31 breholee Exp $
