// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libRTI
//
// CERTI-libRTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libRTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: TimeServices.cc,v 3.3 2005/03/13 22:35:39 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIambassador.hh"

#include "Message.hh"

namespace certi {

static pdCDebug D("LIBRTI", "(libRTI TM) - ");

// ===========================================================================
// TIME MANAGEMENT
// ===========================================================================

// ----------------------------------------------------------------------------
// Enable Time Regulation
void
RTIambassador::enableTimeRegulation(const FedTime& /*theFederateTime*/,
                                    const FedTime& /*theLookahead*/)
    throw (FederationTimeAlreadyPassed,
           TimeRegulationAlreadyEnabled, //not implemented
           EnableTimeRegulationPending, //not implemented
           TimeAdvanceAlreadyInProgress, //not implemented
           InvalidFederationTime, //not implemented
           InvalidLookahead, //not implemented
           ConcurrentAccessAttempted, //not implemented
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::ENABLE_TIME_REGULATION ;
    req.setBoolean(RTI_TRUE);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Time Regulation
void
RTIambassador::disableTimeRegulation()
    throw (FederationTimeAlreadyPassed,
           TimeRegulationWasNotEnabled, //not implemented
           ConcurrentAccessAttempted,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::DISABLE_TIME_REGULATION ;
    req.setBoolean(RTI_FALSE);
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Enable Time Constrained
void
RTIambassador::enableTimeConstrained()
    throw (TimeConstrainedAlreadyEnabled, //not implemented
           EnableTimeConstrainedPending, //not implemented
           TimeAdvanceAlreadyInProgress, //not implemented
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::ENABLE_TIME_CONSTRAINED ;
    req.setBoolean(RTI_TRUE);
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Time Constrained
void
RTIambassador::disableTimeConstrained()
    throw (TimeConstrainedWasNotEnabled, //not implemented
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::DISABLE_TIME_CONSTRAINED ;
    req.setBoolean(RTI_FALSE);
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Time Advance Request
void
RTIambassador::timeAdvanceRequest(const FedTime& theTime)
    throw (TimeAdvanceAlreadyInProgress,
           FederationTimeAlreadyPassed,
           InvalidFederationTime,
           EnableTimeRegulationPending, //not implemented
           EnableTimeConstrainedPending, //not implemented
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::TIME_ADVANCE_REQUEST ;
    req.setFedTime(theTime);
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Time Advance Request Available
void
RTIambassador::timeAdvanceRequestAvailable(const FedTime& theTime)
    throw (InvalidFederationTime,
           FederationTimeAlreadyPassed,
           TimeAdvanceAlreadyInProgress,
           EnableTimeRegulationPending,
           EnableTimeConstrainedPending,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();

    Message req, rep ;

    req.type = Message::TIME_ADVANCE_REQUEST_AVAILABLE ;
    req.setFedTime(theTime);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Next Event Request
void
RTIambassador::nextEventRequest(const FedTime& theTime)
    throw (TimeAdvanceAlreadyInProgress,
           FederationTimeAlreadyPassed,
           InvalidFederationTime,
           EnableTimeRegulationPending, //not implemented
           EnableTimeConstrainedPending, //not implemented
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::NEXT_EVENT_REQUEST ;
    req.setFedTime(theTime);
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Next Event Request Available
void
RTIambassador::nextEventRequestAvailable(const FedTime& theTime)
    throw (InvalidFederationTime,
           FederationTimeAlreadyPassed,
           TimeAdvanceAlreadyInProgress,
           EnableTimeRegulationPending,
           EnableTimeConstrainedPending,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::NEXT_EVENT_REQUEST ;
    req.setFedTime(theTime);
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Flush Queue Request
void
RTIambassador::flushQueueRequest(const FedTime& theTime)
    throw (TimeAdvanceAlreadyInProgress,
           FederationTimeAlreadyPassed,
           InvalidFederationTime,
           EnableTimeRegulationPending, //not implemented
           EnableTimeConstrainedPending, //not implemented
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::FLUSH_QUEUE_REQUEST ;
    req.setFedTime(theTime);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Enable Asynchronous Delivery
void
RTIambassador::enableAsynchronousDelivery()
    throw (AsynchronousDeliveryAlreadyEnabled,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::ENABLE_ASYNCHRONOUS_DELIVERY ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Asynchronous Delivery
void
RTIambassador::disableAsynchronousDelivery()
    throw (AsynchronousDeliveryAlreadyDisabled,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::DISABLE_ASYNCHRONOUS_DELIVERY ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Query LBTS
void
RTIambassador::queryLBTS(FedTime& theTime)
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::QUERY_LBTS ;
    executeService(&req, &rep);

    theTime = rep.getFedTime();
}

// ----------------------------------------------------------------------------
// Query Federate Time
void
RTIambassador::queryFederateTime(FedTime& theTime)
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::QUERY_FEDERATE_TIME ;
    executeService(&req, &rep);

    theTime = rep.getFedTime();
}

// ----------------------------------------------------------------------------
// Query Minimum Next Event Time
void
RTIambassador::queryMinNextEventTime(FedTime& theTime)
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    theTime = RTIfedTime(0.0);
}

// ----------------------------------------------------------------------------
// Modify Lookahead
void
RTIambassador::modifyLookahead(const FedTime& theLookahead)
    throw (InvalidFederationTimeDelta, //CERTI
           InvalidLookahead, //not implemented
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::MODIFY_LOOKAHEAD ;
    req.setLookahead(theLookahead);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Query Lookahead
void
RTIambassador::queryLookahead(FedTime &theTime)
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::QUERY_LOOKAHEAD ;
    executeService(&req, &rep);

    try { 
        RTIfedTime &ret = dynamic_cast<RTIfedTime&>(theTime); 
        ret = RTIfedTime((Double) rep.getFederationTimeDelta());
    }
    catch (std::bad_cast) {
	throw RTIinternalError("theTime is not a RTIfedTime object");
    }
}

// ----------------------------------------------------------------------------
// Retract
void
RTIambassador::retract(EventRetractionHandle handle)
    throw (InvalidRetractionHandle,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService) //CERTI
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::RETRACT ;
    req.setEventRetraction(handle);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Change Attribute Order Type
void
RTIambassador::changeAttributeOrderType(ObjectHandle theObject,
                                        const AttributeHandleSet& attrs,
                                        OrderingHandle theType)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           InvalidOrderType, //CERTI
           InvalidOrderingHandle, //not implemented
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::CHANGE_ATTRIBUTE_ORDER_TYPE ;
    req.setObject(theObject);
    req.setOrdering(theType);
    req.setAHS(attrs);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Change Interaction Order Type
void
RTIambassador::changeInteractionOrderType(InteractionClassHandle theClass,
                                          OrderingHandle theType)
    throw (InteractionClassNotDefined,
           InteractionClassNotPublished,
           InvalidOrderType, //CERTI
           InvalidOrderingHandle, //not implemented
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::CHANGE_INTERACTION_ORDER_TYPE ;
    req.setInteractionClass(theClass);
    req.setOrdering(theType);

    executeService(&req, &rep);
}

}

// $Id: TimeServices.cc,v 3.3 2005/03/13 22:35:39 breholee Exp $
