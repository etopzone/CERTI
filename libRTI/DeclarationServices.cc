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
// $Id: DeclarationServices.cc,v 3.1 2003/07/04 12:00:58 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIambassador.hh"

#include "Message.hh"

namespace certi {

static pdCDebug D("LIBRTI", "(libRTI DM) - ");

// ===========================================================================
// DECLARATION MANAGEMENT
// ===========================================================================

// ----------------------------------------------------------------------------
// Publish Object Class
void
RTIambassador::publishObjectClass(ObjectClassHandle theClass,
                                  const AttributeHandleSet& attributeList)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           OwnershipAcquisitionPending,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           SecurityError, //CERTI
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::PUBLISH_OBJECT_CLASS ;
    req.setObjectClass(theClass);
    req.setAHS(attributeList);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// UnPublish Object Class
void
RTIambassador::unpublishObjectClass(ObjectClassHandle theClass)
    throw (ObjectClassNotDefined,
           ObjectClassNotPublished, //not implemented
           OwnershipAcquisitionPending, //not implemented
           FederateOwnsAttributes, //CERTI
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           SecurityError, //CERTI
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::UNPUBLISH_OBJECT_CLASS ;
    req.setObjectClass(theClass);
    executeService(&req, &rep);
}


// ----------------------------------------------------------------------------
// Publish Interaction Class
void
RTIambassador::publishInteractionClass(InteractionClassHandle theInteraction)
    throw (InteractionClassNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress, //not implemented
           RestoreInProgress, //not implemented
           SecurityError, //RTI
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::PUBLISH_INTERACTION_CLASS ;
    req.setInteractionClass(theInteraction);
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Publish Interaction Class
void
RTIambassador::unpublishInteractionClass(InteractionClassHandle theInteraction)
    throw (InteractionClassNotDefined,
           InteractionClassNotPublished, //not implemented
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           SecurityError, //RTI
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::UNPUBLISH_INTERACTION_CLASS ;
    req.setInteractionClass(theInteraction);
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Subscribe Object Class Attributes
void
RTIambassador::
subscribeObjectClassAttributes(ObjectClassHandle theClass,
                               const AttributeHandleSet& attributeList,
                               Boolean active)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           SecurityError, //RTI
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES ;
    req.setObjectClass(theClass);
    req.setAHS(attributeList);
    req.setBoolean(active);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// UnSubscribe Object Class Attribute
void
RTIambassador::unsubscribeObjectClass(ObjectClassHandle theClass)
    throw (ObjectClassNotDefined,
           ObjectClassNotSubscribed, //not implemented
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           SecurityError, //RTI
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::UNSUBSCRIBE_OBJECT_CLASS ;
    req.setObjectClass(theClass);
    executeService(&req, &rep);
}


// ----------------------------------------------------------------------------
// Subscribe Interaction Class
void
RTIambassador::subscribeInteractionClass(InteractionClassHandle theClass,
                                         Boolean /*active*/)
    throw (InteractionClassNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           FederateLoggingServiceCalls, //not implemented
           SaveInProgress, //not implemented
           RestoreInProgress, //not implemented
           SecurityError, //RTI
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::SUBSCRIBE_INTERACTION_CLASS ;
    req.setInteractionClass(theClass);
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// UnSubscribe Interaction Class
void
RTIambassador::unsubscribeInteractionClass(InteractionClassHandle theClass)
    throw (InteractionClassNotDefined,
           InteractionClassNotSubscribed, //not implemented
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           SecurityError, //RTI
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::UNSUBSCRIBE_INTERACTION_CLASS ;
    req.setInteractionClass(theClass);
    executeService(&req, &rep);
}

}

// $Id: DeclarationServices.cc,v 3.1 2003/07/04 12:00:58 breholee Exp $
