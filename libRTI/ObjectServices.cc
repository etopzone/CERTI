// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: ObjectServices.cc,v 3.2 2005/04/05 12:15:49 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIambassador.hh"

#include "Message.hh"

namespace certi {

static pdCDebug D("LIBRTI", "(libRTI OM) - ");

// ===========================================================================
// OBJECT MANAGEMENT
// ===========================================================================

// ----------------------------------------------------------------------------
// Register Object
ObjectHandle
RTIambassador::registerObjectInstance(ObjectClassHandle theClass,
                                      const char *theObjectName)
    throw (ObjectClassNotDefined,
           ObjectClassNotPublished,
           InvalidObjectHandle, //RTI
           ObjectAlreadyRegistered,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::REGISTER_OBJECT_INSTANCE ;
    req.setName(theObjectName);
    req.setObjectClass(theClass);
    executeService(&req, &rep);

    return rep.getObject();
}

// ----------------------------------------------------------------------------
ObjectHandle
RTIambassador::registerObjectInstance(ObjectClassHandle theClass)
    throw (ObjectClassNotDefined,
           ObjectClassNotPublished,
           InvalidObjectHandle, //RTI
           ObjectAlreadyRegistered,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::REGISTER_OBJECT_INSTANCE ;
    req.setName("\0");
    req.setObjectClass(theClass);

    executeService(&req, &rep);

    return rep.getObject();
}

// ----------------------------------------------------------------------------
// Update Attribute Values
EventRetractionHandle
RTIambassador::
updateAttributeValues(ObjectHandle theObject,
                      const AttributeHandleValuePairSet& theAttributes,
                      const FedTime& theTime,
                      const char *theTag)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           InvalidFederationTime,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::UPDATE_ATTRIBUTE_VALUES ;
    req.setObject(theObject);
    req.setFedTime(theTime);
    req.setTag(theTag);
    req.setAHVPS(theAttributes);
    req.setBoolean(RTI_TRUE);

    executeService(&req, &rep);

    return rep.getEventRetraction();
}

// ----------------------------------------------------------------------------
void
RTIambassador::updateAttributeValues(ObjectHandle the_object,
                                     const AttributeHandleValuePairSet& attrs,
                                     const char *the_tag)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService) //CERTI
{
    throw UnimplementedService();

    Message req, rep ;

    req.type = Message::UPDATE_ATTRIBUTE_VALUES ;
    req.setObject(the_object);
    req.setTag(the_tag);
    req.setAHVPS(attrs);
    req.setBoolean(RTI_FALSE);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Send Interaction
EventRetractionHandle
RTIambassador::sendInteraction(InteractionClassHandle theInteraction,
                               const ParameterHandleValuePairSet& theParameters,
                               const FedTime& theTime,
                               const char *theTag)
    throw (InteractionClassNotDefined,
           InteractionClassNotPublished,
           InteractionParameterNotDefined,
           InvalidFederationTime,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::SEND_INTERACTION ;
    req.setInteractionClass(theInteraction);
    req.setFedTime(theTime);
    req.setTag(theTag);
    req.setPHVPS(theParameters);
    req.setRegion(0);
    req.setBoolean(RTI_TRUE);
    
    executeService(&req, &rep);

    return rep.getEventRetraction();
}

// ----------------------------------------------------------------------------
void
RTIambassador::sendInteraction(InteractionClassHandle the_interaction,
                               const ParameterHandleValuePairSet &parameters,
                               const char *the_tag)
    throw (InteractionClassNotDefined,
           InteractionClassNotPublished,
           InteractionParameterNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService) //CERTI
{
    throw UnimplementedService();

    Message req, rep ;

    req.type = Message::SEND_INTERACTION ;
    req.setInteractionClass(the_interaction);
    req.setTag(the_tag);
    req.setPHVPS(parameters);
    req.setRegion(0);
    req.setBoolean(RTI_FALSE);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Delete Object
EventRetractionHandle
RTIambassador::deleteObjectInstance(ObjectHandle theObject,
                                    const FedTime& theTime,
                                    const char *theTag)
    throw (ObjectNotKnown,
           DeletePrivilegeNotHeld,
           InvalidFederationTime,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::DELETE_OBJECT_INSTANCE ;
    req.setObject(theObject);
    req.setFedTime(theTime);
    req.setTag(theTag);

    executeService(&req, &rep);
    return rep.getEventRetraction();
}

// ----------------------------------------------------------------------------
void
RTIambassador::deleteObjectInstance(ObjectHandle theObject,
                                    const char *theTag)
    throw (ObjectNotKnown,
           DeletePrivilegeNotHeld,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService) //CERTI
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::DELETE_OBJECT_INSTANCE ;
    req.setObject(theObject);
    req.setTag(theTag);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Local Delete Object Instance
void
RTIambassador::localDeleteObjectInstance(ObjectHandle theObject)
    throw (ObjectNotKnown,
           FederateOwnsAttributes, //not implemented
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService) //CERTI
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::LOCAL_DELETE_OBJECT_INSTANCE ;
    req.setObject(theObject);

    executeService(&req, &rep);
}


// ----------------------------------------------------------------------------
// Change Attribute Transportation Type
void
RTIambassador::
changeAttributeTransportationType(ObjectHandle theObject,
                                  const AttributeHandleSet& theAttributes,
                                  TransportationHandle theType)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           InvalidTransportType, //CERTI
           InvalidTransportationHandle, //not implemented
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE ;
    req.setObject(theObject);
    req.setTransportation(theType);
    req.setAHS(theAttributes);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Change Interaction Transportation Type
void
RTIambassador::
changeInteractionTransportationType(InteractionClassHandle theClass,
                                    TransportationHandle theType)
    throw (InteractionClassNotDefined,
           InteractionClassNotPublished,
           InvalidTransportType, //CERTI
           InvalidTransportationHandle, //not implemented
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::CHANGE_INTERACTION_TRANSPORTATION_TYPE ;
    req.setInteractionClass(theClass);
    req.setTransportation(theType);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Request Attribute Value Update
void
RTIambassador::requestObjectAttributeValueUpdate(ObjectHandle theObject,
                                                 const AttributeHandleSet &ahs)
    throw (ObjectNotKnown, // not implemented
           AttributeNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService) //CERTI
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE ;
    req.setObject(theObject);
    req.setAHS(ahs);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
void
RTIambassador::requestClassAttributeValueUpdate(ObjectClassHandle theClass,
                                                const AttributeHandleSet &attrs)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService) //CERTI
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE ;
    req.setObjectClass(theClass);
    req.setAHS(attrs);

    executeService(&req, &rep);
}

}

// $Id: ObjectServices.cc,v 3.2 2005/04/05 12:15:49 breholee Exp $
