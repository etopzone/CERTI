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
// $Id: Interaction.hh,v 3.14 2003/10/20 09:30:44 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_INTERACTION_HH
#define _CERTI_INTERACTION_HH

#include "RTItypes.hh"
#include "SecurityServer.hh"
#include "Parameter.hh"
#include "Subscriber.hh"
#include "Publisher.hh"
#include "InteractionBroadcastList.hh"

#include <list>
#include <string>

namespace certi {

class Interaction
{
public:
    Interaction();
    ~Interaction();

    const char *getName() const ;
    void setName(const char *new_name)
        throw (ValueLengthExceeded, RTIinternalError);

    void setSpace(SpaceHandle);
    SpaceHandle getSpace();

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

    void subscribe(FederateHandle)
        throw (FederateNotSubscribing, RTIinternalError, SecurityError);

    void unsubscribe(FederateHandle)
        throw (FederateNotSubscribing, RTIinternalError, SecurityError);

    void subscribe(FederateHandle, RegionImp *)
        throw (FederateNotSubscribing, RTIinternalError, SecurityError);

    void unsubscribe(FederateHandle, RegionImp *)
        throw (FederateNotSubscribing, RTIinternalError, SecurityError);
    
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
                    const char *the_tag)
        throw (FederateNotPublishing,
               InteractionClassNotDefined,
               InteractionParameterNotDefined,
               RTIinternalError);

    void broadcastInteractionMessage(InteractionBroadcastList *ibList);

    // Attributes
    
    InteractionClassHandle handle ; //!< Interaction class handle.
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
    Parameter *getParameterByHandle(ParameterHandle the_handle) const
        throw (InteractionParameterNotDefined, RTIinternalError);

    void deletePublisher(FederateHandle);
    void deleteSubscriber(FederateHandle, RegionImp *);
    bool isPublishing(FederateHandle);
    bool isSubscribed(FederateHandle);
    bool isSubscribed(FederateHandle, RegionImp *);

    // Attributes    
    std::string name ;
    SecurityLevelID id ; //!< The default Security Level for new parameters
    SpaceHandle space ;

    //! List of this Interaction Class' Parameters.
    std::list<Parameter *> parameterSet ;
    //! List of the Federates(Handles) who subscribed to this Class.
    std::list<Subscriber *> subscribers ;
    //! List of the Federates(Handles) publishing this Class.
    std::list<Publisher *> publishers ;
};

} // namespace

#endif // _CERTI_INTERACTION.HH

// $Id: Interaction.hh,v 3.14 2003/10/20 09:30:44 breholee Exp $
