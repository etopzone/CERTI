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
// $Id: FederationServices.cc,v 3.2 2004/03/04 20:19:05 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIambassador.hh"

#include "Message.hh"

namespace certi {

static pdCDebug D("LIBRTI", "(libRTI FM) - ");

// ===========================================================================
// FEDERATION  MANAGEMENT
// ===========================================================================

// ----------------------------------------------------------------------------
//! Create Federation Execution.
/*! Send a CREATE_FEDERATION_EXECUTION request type to inform rtia process a
  new federation is being created.
*/
void
RTIambassador::createFederationExecution(const char *executionName,
                                         const char */* FED */)
    throw (FederationExecutionAlreadyExists,
           ConcurrentAccessAttempted,
           CouldNotOpenFED, //not implemented
           ErrorReadingFED, //not implemented
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::CREATE_FEDERATION_EXECUTION ;
    req.setFederationName(executionName);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! Destroy Federation Execution.
void
RTIambassador::destroyFederationExecution(const char *executionName)
    throw (FederatesCurrentlyJoined,
           FederationExecutionDoesNotExist,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::DESTROY_FEDERATION_EXECUTION ;
    req.setFederationName(executionName);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! Join Federation Execution.
FederateHandle
RTIambassador::joinFederationExecution(const char *yourName,
				       const char *executionName,
				       FederateAmbassadorPtr fedamb)
    throw (FederateAlreadyExecutionMember,
           FederationExecutionDoesNotExist,
           CouldNotOpenRID, //CERTI
           ErrorReadingRID, //CERTI
           CouldNotOpenFED, //not implemented
           ErrorReadingFED, //not implemented
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    fed_amb = (FederateAmbassador *) fedamb ;

    req.type = Message::JOIN_FEDERATION_EXECUTION ;
    req.setFederateName(yourName);
    req.setFederationName(executionName);

    executeService(&req, &rep);
    return rep.getFederate();
}

// ----------------------------------------------------------------------------
//! Resign Federation Execution.
void
RTIambassador::resignFederationExecution(ResignAction theAction)
    throw (FederateOwnsAttributes,
           FederateNotExecutionMember,
           InvalidResignAction,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::RESIGN_FEDERATION_EXECUTION ;
    req.setResignAction(theAction);
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! Register Federation Synchronization Point
void
RTIambassador::registerFederationSynchronizationPoint(const char *label,
                                                      const char *the_tag)
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::REGISTER_FEDERATION_SYNCHRONIZATION_POINT ;
    req.setLabel(label);
    req.setTag(the_tag);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
void
RTIambassador::registerFederationSynchronizationPoint(const char *label,
						      const char *theTag,
						      const FederateHandleSet &)
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService) //CERTI
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::REGISTER_FEDERATION_SYNCHRONIZATION_POINT ;
    req.setLabel(label);
    req.setTag(theTag);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! Synchronization Point Achieved
void
RTIambassador::synchronizationPointAchieved(const char *label)
    throw (SynchronizationPointLabelWasNotAnnounced,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::SYNCHRONIZATION_POINT_ACHIEVED ;
    req.setLabel(label);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! Request Federation Save.
void
RTIambassador::requestFederationSave(const char *label,
                                     const FedTime& theTime)
    throw (FederationTimeAlreadyPassed,
           InvalidFederationTime,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::REQUEST_FEDERATION_SAVE ;
    req.setFedTime(theTime);
    req.setLabel(label);
    req.setBoolean(RTI_TRUE);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! Request Federation Save.
void
RTIambassador::requestFederationSave(const char *label)
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::REQUEST_FEDERATION_SAVE ;
    req.setLabel(label);
    req.setBoolean(RTI_FALSE);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! Federate Save Begun.
void
RTIambassador::federateSaveBegun()
    throw (SaveNotInitiated,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RestoreInProgress,
           RTIinternalError)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::FEDERATE_SAVE_BEGUN ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! Federate Save Complete.
void
RTIambassador::federateSaveComplete()
    throw (SaveNotInitiated,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RestoreInProgress,
           RTIinternalError)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::FEDERATE_SAVE_COMPLETE ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Federate Save Not Complete.
void
RTIambassador::federateSaveNotComplete()
    throw (SaveNotInitiated,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RestoreInProgress,
           RTIinternalError)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::FEDERATE_SAVE_NOT_COMPLETE ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! Request Restore.
void
RTIambassador::requestFederationRestore(const char *label)
    throw (SpecifiedSaveLabelDoesNotExist, //CERTI
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService) //CERTI
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::REQUEST_FEDERATION_RESTORE ;
    req.setLabel(label);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! Restore Complete.
void
RTIambassador::federateRestoreComplete()
    throw (UnknownLabel,
           RestoreNotRequested,
           RTICannotRestore,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RTIinternalError,
           UnimplementedService) //CERTI
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::FEDERATE_RESTORE_COMPLETE ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! Federate Restore Not Complete.
void
RTIambassador::federateRestoreNotComplete()
    throw (UnknownLabel,
           RestoreNotRequested,
           FederateNotExecutionMember,
           RTICannotRestore,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RTIinternalError,
           UnimplementedService) //CERTI
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::FEDERATE_RESTORE_NOT_COMPLETE ;

    executeService(&req, &rep);
}

}

// $Id: FederationServices.cc,v 3.2 2004/03/04 20:19:05 breholee Exp $
