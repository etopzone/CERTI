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
// $Id: RTIambassador.cc,v 3.32 2003/07/01 13:26:55 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIambassador.hh"

#include "baseTypes.hh"
#include "RTItypes.hh"
#include "GAV.hh"
#include "FederateAmbassador.hh"
#include "Message.hh"
#include "SocketUN.hh"
#include "RootObject.hh"
#include "RegionImp.hh"
#include "fedtime.hh"
#include "converter.hh"
#include "PrettyDebug.hh"

#include <signal.h>
#include <csignal>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <assert.h>

using std::cout ;
using std::cerr ;
using std::endl ;


namespace certi {

static pdCDebug D("LIBRTI", "(libRTI) - ");

// ----------------------------------------------------------------------------
void
RTIambassador::leave(const char *msg)
{
    cout << "libRTI: " << msg << endl ;
    exit(EXIT_FAILURE);
}

// ----------------------------------------------------------------------------
void
RTIambassador::executeService(Message *req, Message *rep)
{
    // raise exception if reentrant call.
    if (is_reentrant)
        throw ConcurrentAccessAttempted();

    D.Out(pdDebug, "sending request to RTIA.");

    is_reentrant = true ;
    try {
        req->write((SocketUN *) this);
    }
    catch (NetworkError) {
        cout << "LibRTI: Catched NetworkError (write), "
             << "throw RTIinternalError." << endl ;
        throw RTIinternalError();
    }

    D.Out(pdDebug, "waiting RTIA reply.");

    // waiting RTI reply.
    try {
        rep->read((SocketUN *) this);
    }
    catch (NetworkError) {
        cout << "LibRTI: Catched NetworkError (read), "
             << "throw RTIinternalError." << endl ;
        throw RTIinternalError();
    }

    D.Out(pdDebug, "RTIA reply received.");

    if (rep->type != req->type) {
        cout << "LibRTI: Assertion failed: rep->type != req->type" << endl ;
        throw RTIinternalError("RTIambassador::executeService: "
                               "rep->type != req->type");
    }

    is_reentrant = false ;
    D.Out(pdDebug, "processing returned exception (from reply).");
    processException(rep);
    D.Out(pdDebug, "exception processed.");
}

// ----------------------------------------------------------------------------
//! Start RTIambassador processes for communication with RTIG.
/*! When a new RTIambassador is created in the application, a new process rtia
  is launched. This process is used for data exchange with rtig server.
  This process connects to rtia after one second delay (UNIX socket).
*/
RTIambassador::RTIambassador()
    throw (MemoryExhausted, RTIinternalError) : SocketUN(stIgnoreSignal)
{
    is_reentrant = false ;
    const char *rtiaexec = "rtia" ;
    const char *rtiaenv = getenv("CERTI_RTIA");
    const char *rtiacall ;
    if (rtiaenv) rtiacall = rtiaenv ;
    else rtiacall = rtiaexec ;

    // creating RTIA process.
    switch((pid_RTIA = fork())) {
      case -1: // fork failed.
        perror("fork");
        throw RTIinternalError("fork failed in RTIambassador constructor");
        break ;

      case 0: // child process (RTIA).
        execlp(rtiacall, NULL);
        perror("execlp");

        cerr << "Could not launch RTIA process." << endl
             << "Maybe RTIA is not in search PATH environment." << endl ;

        exit(-1);

      default: // father process (Federe).
        sleep(1);
        connectUN(pid_RTIA);
        break ;
    }
}

// ----------------------------------------------------------------------------
//! Closes processes.
/*! When destructor is called, kill rtia process.
 */
RTIambassador::~RTIambassador()
{
    kill(pid_RTIA, SIGINT);
}

// ===========================================================================
// FEDERATION MANAGEMENT
// ===========================================================================

// ----------------------------------------------------------------------------
//! Create Federation Execution.
/*! Send a CREATE_FEDERATION_EXECUTION request type to inform rtia process a
  new federation is being created.
*/
void
RTIambassador::createFederationExecution(const char *executionName,
                                         const char *FED)
    throw (FederationExecutionAlreadyExists,
           ConcurrentAccessAttempted,
           CouldNotOpenFED, //not implemented
           ErrorReadingFED, //not implemented
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;
    // char *exeName = new char[20] ;
    // strcpy(exeName, executionName);
    // strcat(exeName, "\56");
    // strcat(exeName, "fed");

    req.type = Message::CREATE_FEDERATION_EXECUTION ;
    req.setFederationName(executionName);

    // if (!strcasecmp(FED, exeName)) {
    executeService(&req, &rep);
    // }
    // else {
    // throw RTIinternalError();
    // }
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
RTIambassador::
joinFederationExecution(const char *yourName,
                        const char *executionName,
                        FederateAmbassadorPtr federateAmbassadorReference)
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

    fed_amb = (FederateAmbassador *) federateAmbassadorReference ;

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
void RTIambassador::
registerFederationSynchronizationPoint(const char *label,
                                       const char *theTag,
                                       const FederateHandleSet &/*syncSet*/)
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
           OwnershipAcquisitionPending, //not implemented
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
                                     const AttributeHandleValuePairSet& the_attributes,
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
    req.setAHVPS(the_attributes);
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
    req.setBoolean(RTI_TRUE);

    executeService(&req, &rep);

    return rep.getEventRetraction();
}

// ----------------------------------------------------------------------------
void
RTIambassador::sendInteraction(InteractionClassHandle the_interaction,
                               const ParameterHandleValuePairSet & the_parameters,
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
    req.setPHVPS(the_parameters);
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
                                                 const AttributeHandleSet& theAttributes)
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
    req.setAHS(theAttributes);

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

// ===========================================================================
// OWNERSHIP MANAGEMENT
// ===========================================================================

// ----------------------------------------------------------------------------
// UnConditional Attribute Ownership Divestiture
void
RTIambassador::
unconditionalAttributeOwnershipDivestiture(ObjectHandle theObject,
                                           const AttributeHandleSet &attrs)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE ;
    req.setObject(theObject);
    req.setAHS(attrs);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Negotiated Attribute Ownership Divestiture
void
RTIambassador::
negotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                        const AttributeHandleSet& attrs,
                                        const char *theTag)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           AttributeAlreadyBeingDivested,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE ;
    req.setObject(theObject);
    req.setTag(theTag);
    req.setAHS(attrs);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Attribute Ownership Acquisition
void
RTIambassador::
attributeOwnershipAcquisition(ObjectHandle theObject,
                              const AttributeHandleSet& desiredAttributes,
                              const char *theTag)
    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::ATTRIBUTE_OWNERSHIP_ACQUISITION ;
    req.setObject(theObject);
    req.setTag(theTag);
    req.setAHS(desiredAttributes);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Attribute Ownership Release Response
AttributeHandleSet*
RTIambassador::
attributeOwnershipReleaseResponse(ObjectHandle theObject,
                                  const AttributeHandleSet& attrs)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           FederateWasNotAskedToReleaseAttribute,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE ;
    req.setObject(theObject);
    req.setAHS(attrs);

    executeService(&req, &rep);

    if (rep.getExceptionType() == e_NO_EXCEPTION) {
        return rep.getAHS();
    }

    return NULL ;
}

// ----------------------------------------------------------------------------
// Cancel Negotiated Attribute Ownership Divestiture
void
RTIambassador::
cancelNegotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                              const AttributeHandleSet& attrs)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           AttributeDivestitureWasNotRequested,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE ;
    req.setObject(theObject);
    req.setAHS(attrs);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Cancel Attribute Ownership Acquisition
void
RTIambassador::
cancelAttributeOwnershipAcquisition(ObjectHandle theObject,
                                    const AttributeHandleSet& attrs)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeAlreadyOwned,
           AttributeAcquisitionWasNotRequested,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION ;
    req.setObject(theObject);
    req.setAHS(attrs);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Attribute Ownership Acquisition If Available
void
RTIambassador::
attributeOwnershipAcquisitionIfAvailable(ObjectHandle theObject,
                                         const AttributeHandleSet& desired)
    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           AttributeAlreadyBeingAcquired,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE ;
    req.setObject(theObject);
    req.setAHS(desired);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Query Attribute Ownership
void
RTIambassador::
queryAttributeOwnership(ObjectHandle theObject,
                        AttributeHandle theAttribute)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotKnown,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::QUERY_ATTRIBUTE_OWNERSHIP ;
    req.setObject(theObject);
    req.setAttribute(theAttribute);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// 5.16 Is Attribute Owned By Federate
Boolean
RTIambassador::isAttributeOwnedByFederate(ObjectHandle theObject,
                                          AttributeHandle theAttribute)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::IS_ATTRIBUTE_OWNED_BY_FEDERATE ;
    req.setObject(theObject);
    req.setAttribute(theAttribute);

    executeService(&req, &rep);

    return ((strcmp(rep.getTag(), "RTI_TRUE") == 0) ? RTI_TRUE : RTI_FALSE);
}

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
RTIambassador::timeAdvanceRequest(FedTime& theTime)
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
RTIambassador::queryLookahead(FedTime& theTime)
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::QUERY_LOOKAHEAD ;
    executeService(&req, &rep);

    RTIfedTime *tmp = new RTIfedTime((Double) rep.getFederationTimeDelta());
    theTime = *(dynamic_cast<FedTime *>(tmp));
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

// ===========================================================================
// DATA DISTRIBUTION MANAGEMENT
// ===========================================================================

// ----------------------------------------------------------------------------
// Create Region
Region*
RTIambassador::createRegion(SpaceHandle space,
                            ULong nb_extents)
    throw (SpaceNotDefined,
           InvalidExtents,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.setType(Message::CREATE_REGION);
    req.setSpace(space);
    req.setNumber(nb_extents);
    executeService(&req, &rep);

    Region *region = new RegionImp(rep.getRegion(), rep.getNumber(),
                                   space, nb_extents);

    return region ;
}

// ----------------------------------------------------------------------------
// Notify About Region Modification
void
RTIambassador::notifyAboutRegionModification(Region &r)
    throw (RegionNotKnown,
           InvalidExtents,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    RegionImp &region = dynamic_cast<RegionImp &>(r);
    Message req, rep ;

    req.setType(Message::MODIFY_REGION);
    req.setRegion(region.getHandle());
    req.setExtents(region.getExtents());

    executeService(&req, &rep);
    region.notify();
}

// ----------------------------------------------------------------------------
// Delete Region
void
RTIambassador::deleteRegion(Region *region)
    throw (RegionNotKnown,
           RegionInUse,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    if (region == 0) {
        throw RegionNotKnown();
    }

    Message req, rep ;

    req.setType(Message::DELETE_REGION);
    req.setRegion(((RegionImp *) region)->getHandle());
    executeService(&req, &rep);

    delete region ;
}

// ----------------------------------------------------------------------------
// Register Object Instance With Region
ObjectHandle
RTIambassador::registerObjectInstanceWithRegion(ObjectClassHandle,
                                                const char *,
                                                AttributeHandle attrs[],
                                                Region *regions[],
                                                ULong)
    throw (ObjectClassNotDefined,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           RegionNotKnown,
           InvalidRegionContext,
           ObjectAlreadyRegistered,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
}

// ----------------------------------------------------------------------------
ObjectHandle
RTIambassador::registerObjectInstanceWithRegion(ObjectClassHandle,
                                                AttributeHandle attrs[],
                                                Region *regions[],
                                                ULong)
    throw (ObjectClassNotDefined,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           RegionNotKnown,
           InvalidRegionContext,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
}

// ----------------------------------------------------------------------------
// Associate Region For Updates
void
RTIambassador::associateRegionForUpdates(Region &region,
                                         ObjectHandle object,
                                         const AttributeHandleSet &attributes)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           InvalidRegionContext,
           RegionNotKnown,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    
}

// ----------------------------------------------------------------------------
// UnAssociate Region For Updates
void
RTIambassador::unassociateRegionForUpdates(Region &/*theRegion*/,
                                           ObjectHandle /*theObject*/)
    throw (ObjectNotKnown,
           InvalidRegionContext,
           RegionNotKnown,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
}

// ----------------------------------------------------------------------------
// Subscribe Object Class Attributes With Region
void
RTIambassador::
subscribeObjectClassAttributesWithRegion(ObjectClassHandle,
                                         Region &,
                                         const AttributeHandleSet &,
                                         Boolean)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           RegionNotKnown,
           InvalidRegionContext,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
}

// ----------------------------------------------------------------------------
// UnSubscribe Object Class Attributes With Region
void
RTIambassador::unsubscribeObjectClassWithRegion(ObjectClassHandle /*theClass*/,
                                                Region &/*theRegion*/)
    throw (ObjectClassNotDefined,
           RegionNotKnown,
           ObjectClassNotSubscribed,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
}

// ----------------------------------------------------------------------------
// Subscribe Interaction Class With Region
void
RTIambassador::subscribeInteractionClassWithRegion(InteractionClassHandle,
                                                   Region &,
                                                   Boolean)
    throw (InteractionClassNotDefined,
           RegionNotKnown,
           InvalidRegionContext,
           FederateLoggingServiceCalls,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
}


// ----------------------------------------------------------------------------
// UnSubscribe Interaction Class With Region
void
RTIambassador::unsubscribeInteractionClassWithRegion(InteractionClassHandle,
                                                     Region &)
    throw (InteractionClassNotDefined,
           InteractionClassNotSubscribed,
           RegionNotKnown,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
}

// ----------------------------------------------------------------------------
// Send Interaction With Region
EventRetractionHandle
RTIambassador::sendInteractionWithRegion(InteractionClassHandle,
                                         const ParameterHandleValuePairSet &,
                                         const FedTime &,
                                         const char *,
                                         const Region &)
    throw (InteractionClassNotDefined,
           InteractionClassNotPublished,
           InteractionParameterNotDefined,
           InvalidFederationTime,
           RegionNotKnown,
           InvalidRegionContext,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
}

void
RTIambassador::sendInteractionWithRegion(InteractionClassHandle,
                                         const ParameterHandleValuePairSet &,
                                         const char *,
                                         const Region &)
    throw (InteractionClassNotDefined,
           InteractionClassNotPublished,
           InteractionParameterNotDefined,
           RegionNotKnown,
           InvalidRegionContext,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
}


// ----------------------------------------------------------------------------
// Request Class Attribute Value Update With Region
void RTIambassador::
requestClassAttributeValueUpdateWithRegion(ObjectClassHandle /*theClass*/,
                                           const AttributeHandleSet &,
                                           const Region &/*theRegion*/)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           RegionNotKnown,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
}

// ===========================================================================
// RTI SUPPORT SERVICES
// ===========================================================================

// ----------------------------------------------------------------------------
// Get Object Class Handle
ObjectClassHandle
RTIambassador::getObjectClassHandle(const char *theName)
    throw (NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_OBJECT_CLASS_HANDLE ;
    req.setName(theName);

    executeService(&req, &rep);

    return rep.getObjectClass();
}

// ----------------------------------------------------------------------------
//! Get Object Class Name
/*! Returns the class name associated with the handle, memory has to
  be freed by the caller.
*/
char *
RTIambassador::getObjectClassName(ObjectClassHandle handle)
    throw (ObjectClassNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_OBJECT_CLASS_NAME ;
    req.setObjectClass(handle);

    executeService(&req, &rep);

    return strdup(rep.getName());
}

// ----------------------------------------------------------------------------
// Get Attribute Handle
AttributeHandle
RTIambassador::getAttributeHandle(const char *theName,
                                  ObjectClassHandle whichClass)
    throw (ObjectClassNotDefined,
           NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)

{
    Message req, rep ;

    req.type = Message::GET_ATTRIBUTE_HANDLE ;
    req.setName(theName);
    req.setObjectClass(whichClass);

    executeService(&req, &rep);

    return rep.getAttribute();
}

// ----------------------------------------------------------------------------
// Get Attribute Name
char *
RTIambassador::getAttributeName(AttributeHandle theHandle,
                                ObjectClassHandle whichClass)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_ATTRIBUTE_NAME ;
    req.setAttribute(theHandle);
    req.setObjectClass(whichClass);

    executeService(&req, &rep); // Send request to RTI.

    return strdup(rep.getName());
}

// ----------------------------------------------------------------------------
// Get Interaction Class Handle
InteractionClassHandle
RTIambassador::getInteractionClassHandle(const char *theName)
    throw (NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_INTERACTION_CLASS_HANDLE ;
    req.setName(theName);

    executeService(&req, &rep);

    return rep.getInteractionClass();
}


// ----------------------------------------------------------------------------
// Get Interaction Class Name
char *
RTIambassador::getInteractionClassName(InteractionClassHandle theHandle)
    throw (InteractionClassNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_INTERACTION_CLASS_NAME ;
    req.setInteractionClass(theHandle);

    executeService(&req, &rep);

    return strdup(rep.getName());
}

// ----------------------------------------------------------------------------
// Get Parameter Handle
ParameterHandle
RTIambassador::getParameterHandle(const char *theName,
                                  InteractionClassHandle whichClass)
    throw (InteractionClassNotDefined,
           NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_PARAMETER_HANDLE ;
    req.setName(theName);
    req.setInteractionClass(whichClass);

    executeService(&req, &rep);

    return rep.getParameter();
}

// ----------------------------------------------------------------------------
// Get Parameter Name
char *
RTIambassador::getParameterName(ParameterHandle theHandle,
                                InteractionClassHandle whichClass)
    throw (InteractionClassNotDefined,
           InteractionParameterNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_PARAMETER_NAME ;
    req.setParameter(theHandle);
    req.setInteractionClass(whichClass);

    executeService(&req, &rep);

    return strdup(rep.getName());
}

// ----------------------------------------------------------------------------
// Get Object Instance Handle
ObjectHandle
RTIambassador::getObjectInstanceHandle(const char *theName)
    throw (ObjectNotKnown,
           NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::GET_OBJECT_INSTANCE_HANDLE ;
    req.setName(theName);

    executeService(&req, &rep);

    return rep.getObject();
}

// ----------------------------------------------------------------------------
// Get Object Instance Name
char *
RTIambassador::getObjectInstanceName(ObjectHandle theHandle)
    throw (ObjectNotKnown,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::GET_OBJECT_INSTANCE_NAME ;
    req.setObject(theHandle);

    executeService(&req, &rep);

    return(strdup(rep.getName()));
}

// ----------------------------------------------------------------------------
// Get Routing Space Handle
SpaceHandle
RTIambassador::getRoutingSpaceHandle(const char *rs_name)
    throw (NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_SPACE_HANDLE ;
    req.setName(rs_name);
    this->executeService(&req, &rep);

    return rep.getSpace();
}

// ----------------------------------------------------------------------------
// Get Routing Space Name
char *
RTIambassador::getRoutingSpaceName(SpaceHandle handle)
    throw (SpaceNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_SPACE_NAME ;
    req.setSpace(handle);
    this->executeService(&req, &rep);

    return strdup(rep.getName());
}

// ----------------------------------------------------------------------------
// Get Dimension Handle
DimensionHandle
RTIambassador::getDimensionHandle(const char *dimension,
                                  SpaceHandle space)
    throw (SpaceNotDefined,
           NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_DIMENSION_HANDLE ;
    req.setName(dimension);
    req.setSpace(space);
    this->executeService(&req, &rep);

    return rep.getDimension();
}

// ----------------------------------------------------------------------------
// Get Dimension Name
char *
RTIambassador::getDimensionName(DimensionHandle dimension,
                                SpaceHandle space)
    throw (SpaceNotDefined,
           DimensionNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_DIMENSION_NAME ;
    req.setDimension(dimension);
    req.setSpace(space);
    this->executeService(&req, &rep);

    return strdup(rep.getName());
}

// ----------------------------------------------------------------------------
// Get Attribute Routing Space Handle
SpaceHandle
RTIambassador::getAttributeRoutingSpaceHandle(AttributeHandle attribute,
                                              ObjectClassHandle objectClass)
    throw (ObjectClassNotDefined, AttributeNotDefined,
           FederateNotExecutionMember, ConcurrentAccessAttempted,
           RTIinternalError, UnimplementedService)
{
    Message req, rep ;

    req.type = Message::GET_ATTRIBUTE_SPACE_HANDLE ;
    req.setAttribute(attribute);
    req.setObjectClass(objectClass);

    executeService(&req, &rep);

    return rep.getSpace();
}

// ----------------------------------------------------------------------------
// Get Object Class
ObjectClassHandle
RTIambassador::getObjectClass(ObjectHandle theObject)
    throw (ObjectNotKnown,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::GET_OBJECT_CLASS ;
    req.setObject(theObject);

    executeService(&req, &rep);

    return rep.getObjectClass();
}

// ----------------------------------------------------------------------------
// Get Interaction Routing Space Handle
SpaceHandle
RTIambassador::getInteractionRoutingSpaceHandle(InteractionClassHandle inter)
    throw (InteractionClassNotDefined, FederateNotExecutionMember,
           ConcurrentAccessAttempted, RTIinternalError, UnimplementedService)
{
    Message req, rep ;

    req.type = Message::GET_INTERACTION_SPACE_HANDLE ;
    req.setInteractionClass(inter);

    this->executeService(&req, &rep);

    return rep.getSpace();
}

// ----------------------------------------------------------------------------
// Get Transportation Handle
TransportationHandle
RTIambassador::getTransportationHandle(const char *theName)
    throw (NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::GET_TRANSPORTATION_HANDLE ;
    req.setName(theName);

    executeService(&req, &rep);

    return rep.getTransportation();
}

// ----------------------------------------------------------------------------
// Get Transportation Name
char *
RTIambassador::getTransportationName(TransportationHandle theHandle)
    throw (InvalidTransportationHandle,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::GET_TRANSPORTATION_NAME ;
    req.setTransportation(theHandle);

    executeService(&req, &rep);

    return(strdup(rep.getName()));
}

// ----------------------------------------------------------------------------
// Get Ordering Handle
OrderingHandle
RTIambassador::getOrderingHandle(const char *theName)
    throw (NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::GET_ORDERING_HANDLE ;
    req.setName(theName);

    executeService(&req, &rep);

    return rep.getOrdering();
}


// ----------------------------------------------------------------------------
// Get Ordering Name
char *
RTIambassador::getOrderingName(OrderingHandle theHandle)
    throw (InvalidOrderingHandle,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::GET_ORDERING_NAME ;
    req.setOrdering(theHandle);

    executeService(&req, &rep);

    return(strdup(rep.getName()));
}

// ----------------------------------------------------------------------------
// Enable Class Relevance Advisory Switch
void
RTIambassador::enableClassRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Class Relevance Advisory Switch
void
RTIambassador::disableClassRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Enable Attribute Relevance Advisory Switch
void
RTIambassador::enableAttributeRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Attribute Relevance Advisory Switch
void
RTIambassador::disableAttributeRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Enable Attribute Scope Advisory Switch
void RTIambassador::enableAttributeScopeAdvisorySwitch()
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Attribute Scope Advisory Switch
void
RTIambassador::disableAttributeScopeAdvisorySwitch()
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Enable Interaction Relevance Advisory Switch
void
RTIambassador::enableInteractionRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Interaction Relevance Advisory Switch
void
RTIambassador::disableInteractionRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = Message::DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Tick
Boolean
RTIambassador::tick()
    throw (SpecifiedSaveLabelDoesNotExist,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message vers_RTI, vers_Fed ;

    // Throw exception if reentrant call.
    if (is_reentrant)
        throw ConcurrentAccessAttempted();

    is_reentrant = true ;

    // Prevenir le RTI
    vers_RTI.type = Message::TICK_REQUEST ;

    try {
        vers_RTI.write((SocketUN *) this);
    }
    catch (NetworkError) {
        cout << "tick 1." << endl ;
        cout << "LibRTI: Catched NetworkError, throw RTIinternalError."
             << endl ;
        throw RTIinternalError();
    }

    for (;;) {

        // Lire la reponse du RTIA local
        try {
            vers_Fed.read((SocketUN *) this);
        }
        catch (NetworkError) {
            cout << "tick 2." << endl ;
            cout << "LibRTI: Catched NetworkError, throw RTIinternalError."
                 << endl ;
            throw RTIinternalError();
        }

        // Si c'est de type TICK_REQUEST, il n'y a qu'a traiter l'exception.
        if (vers_Fed.type == Message::TICK_REQUEST) {
            is_reentrant = false ;
            processException(&vers_Fed);
            return vers_Fed.getBoolean();
        }

        // Sinon, le RTI nous demande un service, donc on appele une methode
        // du FederateAmbassador.

        vers_RTI.setException(e_NO_EXCEPTION);
        try {
            switch (vers_Fed.type) {

              case Message::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
                fed_amb->synchronizationPointRegistrationSucceeded(vers_Fed.getLabel());
                break ;

              case Message::ANNOUNCE_SYNCHRONIZATION_POINT:
                fed_amb->announceSynchronizationPoint(vers_Fed.getLabel(),
                                                      vers_Fed.getTag());
                break ;

              case Message::FEDERATION_SYNCHRONIZED:
                fed_amb->federationSynchronized(vers_Fed.getLabel());
                break ;

              case Message::INITIATE_FEDERATE_SAVE:
                fed_amb->initiateFederateSave(vers_Fed.getLabel());
                break ;

              case Message::FEDERATION_SAVED:
                fed_amb->federationSaved();
                break ;

              case Message::REQUEST_FEDERATION_RESTORE_SUCCEEDED:
                fed_amb->requestFederationRestoreSucceeded(
                    vers_Fed.getLabel());
                break ;

              case Message::REQUEST_FEDERATION_RESTORE_FAILED:
                fed_amb->requestFederationRestoreFailed(vers_Fed.getLabel(),
                                                        vers_Fed.getTag());
                break ;

              case Message::FEDERATION_RESTORE_BEGUN:
                fed_amb->federationRestoreBegun();
                break ;

              case Message::INITIATE_FEDERATE_RESTORE:
                fed_amb->initiateFederateRestore(vers_Fed.getLabel(),
                                                 vers_Fed.getFederate());
                break ;

              case Message::FEDERATION_RESTORED:
                fed_amb->federationRestored();
                break ;

              case Message::FEDERATION_NOT_RESTORED:
                fed_amb->federationNotRestored();
                break ;

              case Message::START_REGISTRATION_FOR_OBJECT_CLASS: {
                  fed_amb->
                      startRegistrationForObjectClass(vers_Fed.getObjectClass());
              } break ;

              case Message::STOP_REGISTRATION_FOR_OBJECT_CLASS: {
                  fed_amb->
                      stopRegistrationForObjectClass(vers_Fed.getObjectClass());
              } break ;

              case Message::TURN_INTERACTIONS_ON: {
                  fed_amb->turnInteractionsOn(vers_Fed.getInteractionClass());
              } break ;

              case Message::TURN_INTERACTIONS_OFF: {
                  fed_amb->turnInteractionsOff(vers_Fed.getInteractionClass());
              } break ;

              case Message::DISCOVER_OBJECT_INSTANCE: {
                  fed_amb->
                      discoverObjectInstance(vers_Fed.getObject(),
                                             vers_Fed.getObjectClass(),
                                             vers_Fed.getName());
              } break ;

              case Message::REFLECT_ATTRIBUTE_VALUES: {
                  AttributeHandleValuePairSet * theAttributes = vers_Fed.getAHVPS();
                  fed_amb->
                      reflectAttributeValues(vers_Fed.getObject(),
                                             *theAttributes,
                                             vers_Fed.getFedTime(),
                                             vers_Fed.getTag(),
                                             vers_Fed.getEventRetraction());

                  delete theAttributes ;
              } break ;

              case Message::RECEIVE_INTERACTION: {
                  ParameterHandleValuePairSet * theParameters = vers_Fed.getPHVPS();

                  fed_amb->receiveInteraction(vers_Fed.getInteractionClass(),
                                              *theParameters,
                                              vers_Fed.getFedTime(),
                                              vers_Fed.getTag(),
                                              vers_Fed.getEventRetraction());

                  delete theParameters ;
              } break ;

              case Message::REMOVE_OBJECT_INSTANCE: {
                  fed_amb->removeObjectInstance(vers_Fed.getObject(),
                                                vers_Fed.getFedTime(),
                                                vers_Fed.getTag(),
                                                vers_Fed.getEventRetraction());
              } break ;

              case Message::PROVIDE_ATTRIBUTE_VALUE_UPDATE: {



                  // fed_amb->provideAttributeValueUpdate();
              } break ;

              case Message::REQUEST_RETRACTION: {

              } break ;

              case Message::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION: {
                  AttributeHandleSet *attributeSet = vers_Fed.getAHS();

                  fed_amb->
                      requestAttributeOwnershipAssumption(vers_Fed.getObject(),
                                                          *attributeSet,
                                                          vers_Fed.getTag());
                  delete attributeSet ;
              } break ;

              case Message::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE: {
                  AttributeHandleSet *attributeSet = vers_Fed.getAHS();

                  fed_amb->requestAttributeOwnershipRelease(vers_Fed.getObject(),
                                                            *attributeSet,
                                                            vers_Fed.getTag());

                  delete attributeSet ;
              } break ;

              case Message::ATTRIBUTE_OWNERSHIP_UNAVAILABLE: {
                  AttributeHandleSet *attributeSet = vers_Fed.getAHS();

                  fed_amb->attributeOwnershipUnavailable(vers_Fed.getObject(),
                                                         *attributeSet);

                  delete attributeSet ;
              } break ;

              case Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION: {
                  AttributeHandleSet *attributeSet = vers_Fed.getAHS();

                  fed_amb->
                      attributeOwnershipAcquisitionNotification(vers_Fed.getObject(),
                                                                *attributeSet);

                  delete attributeSet ;
              } break ;

              case Message::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION: {
                  AttributeHandleSet *attributeSet = vers_Fed.getAHS();

                  fed_amb->
                      attributeOwnershipDivestitureNotification(vers_Fed.getObject(),
                                                                *attributeSet);

                  delete attributeSet ;
              } break ;

              case Message::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION: {
                  AttributeHandleSet *attributeSet = vers_Fed.getAHS();

                  fed_amb->
                      confirmAttributeOwnershipAcquisitionCancellation
                      (vers_Fed.getObject(), *attributeSet);

                  delete attributeSet ;
              } break ;

              case Message::INFORM_ATTRIBUTE_OWNERSHIP: {
                  fed_amb->
                      informAttributeOwnership(vers_Fed.getObject(),
                                               vers_Fed.getAttribute(),
                                               vers_Fed.getFederate());
              } break ;

              case Message::ATTRIBUTE_IS_NOT_OWNED: {
                  fed_amb->attributeIsNotOwned(vers_Fed.getObject(),
                                               vers_Fed.getAttribute());
              } break ;

              case Message::TIME_ADVANCE_GRANT: {
                  fed_amb->timeAdvanceGrant(vers_Fed.getFedTime());
              } break ;

              default: {
                  leave("RTI service requested by RTI is unknown.");
              }
            }
        }
        catch (InvalidFederationTime &e) {
            vers_RTI.setException(e_InvalidFederationTime, e._reason);
        }
        catch (TimeAdvanceWasNotInProgress &e) {
            vers_RTI.setException(e_TimeAdvanceWasNotInProgress, e._reason);
        }
        catch (FederationTimeAlreadyPassed &e) {
            vers_RTI.setException(e_FederationTimeAlreadyPassed, e._reason);
        }
        catch (FederateInternalError &e) {
            vers_RTI.setException(e_FederateInternalError, e._reason);
        }
        catch (Exception &e) {
            vers_RTI.setException(e_RTIinternalError, e._reason);
        }

        // retourner au RTI la reponse du service demande
        vers_RTI.type = vers_Fed.type ;

        try {
            vers_RTI.write((SocketUN *) this);
        }
        catch (NetworkError) {
            cout << "tick 3." << endl ;
            cout << "LibRTI: Catched NetworkError, throw RTIinternalError."
                 << endl ;
            throw RTIinternalError();
        }
    }
}

// ----------------------------------------------------------------------------
Boolean
RTIambassador::tick(TickTime minimum, TickTime maximum)
    throw (SpecifiedSaveLabelDoesNotExist, ConcurrentAccessAttempted,
           RTIinternalError)
{
    return tick();
}

// ----------------------------------------------------------------------------
//! Process exception from received message.
/*! When a message is received from RTIA, it can contains an exception.
  This exception is processed by this module and a new exception is thrown.
*/
void
RTIambassador::processException(Message *msg)
{
    D.Out(pdExcept, "nom de l'exception : %d .", msg->getExceptionType());
    switch(msg->getExceptionType()) {
      case e_NO_EXCEPTION: {
      } break ;

      case e_ArrayIndexOutOfBounds: {
          D.Out(pdExcept, "Throwing e_ArrayIndexOutOfBounds exception.");
          throw ArrayIndexOutOfBounds(msg->getExceptionReason());
      } break ;

      case e_AttributeAlreadyOwned: {
          D.Out(pdExcept, "Throwing e_AttributeAlreadyOwned exception.");
          throw AttributeAlreadyOwned(msg->getExceptionReason());
      } break ;

      case e_AttributeAlreadyBeingAcquired: {
          D.Out(pdExcept, "Throwing e_AttributeAlreadyBeingAcquired exception.");
          throw AttributeAlreadyBeingAcquired(msg->getExceptionReason());
      } break ;

      case e_AttributeAlreadyBeingDivested: {
          D.Out(pdExcept, "Throwing e_AttributeAlreadyBeingDivested exception.");
          throw AttributeAlreadyBeingDivested(msg->getExceptionReason());
      } break ;

      case e_AttributeDivestitureWasNotRequested: {
          D.Out(pdExcept,
                "Throwing e_AttributeDivestitureWasNotRequested exception.");
          throw AttributeDivestitureWasNotRequested(msg->getExceptionReason());
      } break ;

      case e_AttributeAcquisitionWasNotRequested: {
          D.Out(pdExcept,
                "Throwing e_AttributeAcquisitionWasNotRequested exception.");
          throw AttributeAcquisitionWasNotRequested(msg->getExceptionReason());
      } break ;

      case e_AttributeNotDefined: {
          D.Out(pdExcept, "Throwing e_AttributeNotDefined exception.");
          throw AttributeNotDefined(msg->getExceptionReason());
      } break ;

      case e_AttributeNotKnown: {
          D.Out(pdExcept, "Throwing e_AttributeNotKnown exception.");
          throw AttributeNotKnown(msg->getExceptionReason());
      } break ;

      case e_AttributeNotOwned: {
          D.Out(pdExcept, "Throwing e_AttributeNotOwned exception.");
          throw AttributeNotOwned(msg->getExceptionReason());
      } break ;

      case e_AttributeNotPublished: {
          D.Out(pdExcept, "Throwing e_AttributeNotPublished exception.");
          throw AttributeNotPublished(msg->getExceptionReason());
      } break ;

      case e_AttributeNotSubscribed: {
          D.Out(pdExcept, "Throwing e_AttributeNotSubscribed exception.");
          throw AttributeNotSubscribed(msg->getExceptionReason());
      } break ;

      case e_ConcurrentAccessAttempted: {
          D.Out(pdExcept, "Throwing e_ConcurrentAccessAttempted exception.");
          throw ConcurrentAccessAttempted(msg->getExceptionReason());
      } break ;

      case e_CouldNotDiscover: {
          D.Out(pdExcept, "Throwing e_CouldNotDiscover exception.");
          throw CouldNotDiscover(msg->getExceptionReason());
      } break ;

      case e_CouldNotOpenRID: {
          D.Out(pdExcept, "Throwing e_CouldNotOpenRID exception.");
          throw CouldNotOpenRID(msg->getExceptionReason());
      } break ;

      case e_CouldNotRestore: {
          D.Out(pdExcept, "Throwing e_CouldNotRestore exception.");
          throw CouldNotRestore(msg->getExceptionReason());
      } break ;

      case e_DeletePrivilegeNotHeld: {
          D.Out(pdExcept, "Throwing e_DeletePrivilegeNotHeld exception.");
          throw DeletePrivilegeNotHeld(msg->getExceptionReason());
      } break ;

      case e_ErrorReadingRID: {
          D.Out(pdExcept, "Throwing e_ErrorReadingRID exception.");
          throw ErrorReadingRID(msg->getExceptionReason());
      } break ;

      case e_EventNotKnown: {
          D.Out(pdExcept, "Throwing e_EventNotKnown exception.");
          throw EventNotKnown(msg->getExceptionReason());
      } break ;

      case e_FederateAlreadyPaused: {
          D.Out(pdExcept, "Throwing e_FederateAlreadyPaused exception.");
          throw FederateAlreadyPaused(msg->getExceptionReason());
      } break ;

      case e_FederateAlreadyExecutionMember: {
          D.Out(pdExcept, "Throwing e_FederateAlreadyExecutionMember exception.");
          throw FederateAlreadyExecutionMember(msg->getExceptionReason());
      } break ;

      case e_FederateDoesNotExist: {
          D.Out(pdExcept, "Throwing e_FederateDoesNotExist exception.");
          throw FederateDoesNotExist(msg->getExceptionReason());
      } break ;

      case e_FederateInternalError: {
          D.Out(pdExcept, "Throwing e_FederateInternalError exception.");
          throw FederateInternalError(msg->getExceptionReason());
      } break ;

      case e_FederateNameAlreadyInUse: {
          D.Out(pdExcept, "Throwing e_FederateNameAlreadyInUse exception.");
          throw FederateNameAlreadyInUse(msg->getExceptionReason());
      } break ;

      case e_FederateNotExecutionMember: {
          D.Out(pdExcept, "Throwing e_FederateNotExecutionMember exception.");
          throw FederateNotExecutionMember(msg->getExceptionReason());
      } break ;

      case e_FederateNotPaused: {
          D.Out(pdExcept, "Throwing e_FederateNotPaused exception.");
          throw FederateNotPaused(msg->getExceptionReason());
      } break ;

      case e_FederateOwnsAttributes: {
          D.Out(pdExcept, "Throwing e_FederateOwnsAttributes exception.");
          throw FederateOwnsAttributes(msg->getExceptionReason());
      } break ;

      case e_FederatesCurrentlyJoined: {
          D.Out(pdExcept, "Throwing e_FederatesCurrentlyJoined exception.");
          throw FederatesCurrentlyJoined(msg->getExceptionReason());
      } break ;

      case e_FederateWasNotAskedToReleaseAttribute: {
          D.Out(pdExcept,
                "Throwing e_FederateWasNotAskedToReleaseAttribute exception.");
          D.Out(pdDebug,
                "Throwing e_FederateWasNotAskedToReleaseAttribute exception.");
          throw FederateWasNotAskedToReleaseAttribute(msg->getExceptionReason());
      } break ;

      case e_FederationAlreadyPaused: {
          D.Out(pdExcept, "Throwing e_FederationAlreadyPaused exception.");
          throw FederationAlreadyPaused(msg->getExceptionReason());
      } break ;

      case e_FederationExecutionAlreadyExists: {
          D.Out(pdExcept, "Throwing e_FederationExecutionAlreadyExists excep.");
          throw FederationExecutionAlreadyExists(msg->getExceptionReason());
      } break ;

      case e_FederationExecutionDoesNotExist: {
          D.Out(pdExcept, "Throwing e_FederationExecutionDoesNotExist except.");
          throw FederationExecutionDoesNotExist(msg->getExceptionReason());
      } break ;

      case e_FederationNotPaused: {
          D.Out(pdExcept, "Throwing e_FederationNotPaused exception.");
          throw FederationNotPaused(msg->getExceptionReason());
      } break ;

      case e_FederationTimeAlreadyPassed: {
          D.Out(pdExcept, "Throwing e_FederationTimeAlreadyPassed exception.");
          throw FederationTimeAlreadyPassed(msg->getExceptionReason());
      } break ;

      case e_FederateNotPublishing: {
          D.Out(pdExcept, "Throwing e_FederateNotPublishing exception.");
          throw FederateNotPublishing(msg->getExceptionReason());
      } break ;

      case e_FederateNotSubscribing: {
          D.Out(pdExcept, "Throwing e_FederateNotSubscribing exception.");
          throw FederateNotSubscribing(msg->getExceptionReason());
      } break ;

      case e_RegionNotKnown: {
          D.Out(pdExcept, "Throwing e_RegionNotKnown exception.");
          throw RegionNotKnown(msg->getExceptionReason());
      } break ;

      case e_IDsupplyExhausted: {
          D.Out(pdExcept, "Throwing e_IDsupplyExhausted exception.");
          throw IDsupplyExhausted(msg->getExceptionReason());
      } break ;

      case e_InteractionClassNotDefined: {
          D.Out(pdExcept, "Throwing e_InteractionClassNotDefined exception.");
          throw InteractionClassNotDefined(msg->getExceptionReason());
      } break ;

      case e_InteractionClassNotKnown: {
          D.Out(pdExcept, "Throwing e_InteractionClassNotKnown exception.");
          throw InteractionClassNotKnown(msg->getExceptionReason());
      } break ;

      case e_InteractionClassNotPublished: {
          D.Out(pdExcept, "Throwing e_InteractionClassNotPublished exception.");
          throw InteractionClassNotPublished(msg->getExceptionReason());
      } break ;

      case e_InteractionParameterNotDefined: {
          D.Out(pdExcept, "Throwing e_InteractionParameterNotDefined exception.");
          throw InteractionParameterNotDefined(msg->getExceptionReason());
      } break ;

      case e_InteractionParameterNotKnown: {
          D.Out(pdExcept, "Throwing e_InteractionParameterNotKnown exception.");
          throw InteractionParameterNotKnown(msg->getExceptionReason());
      } break ;

      case e_InvalidDivestitureCondition: {
          D.Out(pdExcept, "Throwing e_InvalidDivestitureCondition exception.");
          throw InvalidDivestitureCondition(msg->getExceptionReason());
      } break ;

      case e_InvalidExtents: {
          D.Out(pdExcept, "Throwing e_InvalidExtents exception.");
          throw InvalidExtents(msg->getExceptionReason());
      } break ;

      case e_InvalidFederationTime: {
          D.Out(pdExcept, "Throwing e_InvalidFederationTime exception.");
          throw InvalidFederationTime(msg->getExceptionReason());
      } break ;

      case e_InvalidFederationTimeDelta: {
          D.Out(pdExcept, "Throwing e_InvalidFederationTimeDelta exception.");
          throw InvalidFederationTimeDelta(msg->getExceptionReason());
      } break ;

      case e_InvalidObjectHandle: {
          D.Out(pdExcept, "Throwing e_InvalidObjectHandle exception.");
          throw InvalidObjectHandle(msg->getExceptionReason());
      } break ;

      case e_InvalidOrderType: {
          D.Out(pdExcept, "Throwing e_InvalidOrderType exception.");
          throw InvalidOrderType(msg->getExceptionReason());
      } break ;

      case e_InvalidResignAction: {
          D.Out(pdExcept, "Throwing e_InvalidResignAction exception.");
          throw InvalidResignAction(msg->getExceptionReason());
      } break ;

      case e_InvalidRetractionHandle: {
          D.Out(pdExcept, "Throwing e_InvalidRetractionHandle exception.");
          throw InvalidRetractionHandle(msg->getExceptionReason());
      } break ;

      case e_InvalidRoutingSpace: {
          D.Out(pdExcept, "Throwing e_InvalidRoutingSpace exception.");
          throw InvalidRoutingSpace(msg->getExceptionReason());
      } break ;

      case e_InvalidTransportType: {
          D.Out(pdExcept, "Throwing e_InvalidTransportType exception.");
          throw InvalidTransportType(msg->getExceptionReason());
      } break ;

      case e_MemoryExhausted: {
          D.Out(pdExcept, "Throwing e_MemoryExhausted exception.");
          throw MemoryExhausted(msg->getExceptionReason());
      } break ;

      case e_NameNotFound: {
          D.Out(pdExcept, "Throwing e_NameNotFound exception.");
          throw NameNotFound(msg->getExceptionReason());
      } break ;

      case e_NoPauseRequested: {
          D.Out(pdExcept, "Throwing e_NoPauseRequested exception.");
          throw NoPauseRequested(msg->getExceptionReason());
      } break ;

      case e_NoResumeRequested: {
          D.Out(pdExcept, "Throwing e_NoResumeRequested exception.");
          throw NoResumeRequested(msg->getExceptionReason());
      } break ;

      case e_ObjectClassNotDefined: {
          D.Out(pdExcept, "Throwing e_ObjectClassNotDefined exception.");
          throw ObjectClassNotDefined(msg->getExceptionReason());
      } break ;

      case e_ObjectClassNotKnown: {
          D.Out(pdExcept, "Throwing e_ObjectClassNotKnown exception.");
          throw ObjectClassNotKnown(msg->getExceptionReason());
      } break ;

      case e_ObjectClassNotPublished: {
          D.Out(pdExcept, "Throwing e_ObjectClassNotPublished exception.");
          throw ObjectClassNotPublished(msg->getExceptionReason());
      } break ;

      case e_ObjectClassNotSubscribed: {
          D.Out(pdExcept, "Throwing e_ObjectClassNotSubscribed exception.");
          throw ObjectClassNotSubscribed(msg->getExceptionReason());
      } break ;

      case e_ObjectNotKnown: {
          D.Out(pdExcept, "Throwing e_ObjectNotKnown exception.");
          throw ObjectNotKnown(msg->getExceptionReason());
      } break ;

      case e_ObjectAlreadyRegistered: {
          D.Out(pdExcept, "Throwing e_ObjectAlreadyRegistered exception.");
          throw ObjectAlreadyRegistered(msg->getExceptionReason());
      } break ;

      case e_RestoreInProgress: {
          D.Out(pdExcept, "Throwing e_RestoreInProgress exception.");
          throw RestoreInProgress(msg->getExceptionReason());
      } break ;

      case e_RestoreNotRequested: {
          D.Out(pdExcept, "Throwing e_RestoreNotRequested exception.");
          throw RestoreNotRequested(msg->getExceptionReason());
      } break ;

      case e_RTIinternalError: {
          D.Out(pdExcept, "Throwing e_RTIinternalError exception.");
          throw RTIinternalError(msg->getExceptionReason());
      } break ;

      case e_SpaceNotDefined: {
          D.Out(pdExcept, "Throwing e_SpaceNotDefined exception.");
          throw SpaceNotDefined(msg->getExceptionReason());
      } break ;

      case e_SaveInProgress: {
          D.Out(pdExcept, "Throwing e_SaveInProgress exception.");
          throw SaveInProgress(msg->getExceptionReason());
      } break ;

      case e_SaveNotInitiated: {
          D.Out(pdExcept, "Throwing e_SaveNotInitiated exception.");
          throw SaveNotInitiated(msg->getExceptionReason());
      } break ;

      case e_SecurityError: {
          D.Out(pdExcept, "Throwing e_SecurityError exception.");
          throw SecurityError(msg->getExceptionReason());
      } break ;

      case e_SpecifiedSaveLabelDoesNotExist: {
          D.Out(pdExcept, "Throwing e_SpecifiedSaveLabelDoesNotExist exception.");
          throw SpecifiedSaveLabelDoesNotExist(msg->getExceptionReason());
      } break ;

      case e_TimeAdvanceAlreadyInProgress: {
          D.Out(pdExcept, "Throwing e_TimeAdvanceAlreadyInProgress exception.");
          throw TimeAdvanceAlreadyInProgress(msg->getExceptionReason());
      } break ;

      case e_TimeAdvanceWasNotInProgress: {
          D.Out(pdExcept, "Throwing e_TimeAdvanceWasNotInProgress exception.");
          throw TimeAdvanceWasNotInProgress(msg->getExceptionReason());
      } break ;

      case e_TooManyIDsRequested: {
          D.Out(pdExcept, "Throwing e_TooManyIDsRequested exception.");
          throw TooManyIDsRequested(msg->getExceptionReason());
      } break ;

      case e_UnableToPerformSave: {
          D.Out(pdExcept, "Throwing e_UnableToPerformSave exception.");
          throw UnableToPerformSave(msg->getExceptionReason());
      } break ;

      case e_UnimplementedService: {
          D.Out(pdExcept, "Throwing e_UnimplementedService exception.");
          throw UnimplementedService(msg->getExceptionReason());
      } break ;

      case e_UnknownLabel: {
          D.Out(pdExcept, "Throwing e_UnknownLabel exception.");
          throw UnknownLabel(msg->getExceptionReason());
      } break ;

      case e_ValueCountExceeded: {
          D.Out(pdExcept, "Throwing e_ValueCountExceeded exception.");
          throw ValueCountExceeded(msg->getExceptionReason());
      } break ;

      case e_ValueLengthExceeded: {
          D.Out(pdExcept, "Throwing e_ValueLengthExceeded exception.");
          throw ValueLengthExceeded(msg->getExceptionReason());
      } break ;

      default: {
          D.Out(pdExcept, "Throwing unknown exception !");
          cout << "LibRTI: Receiving unknown exception." << endl ;
          throw RTIinternalError(msg->getExceptionReason());
      } break ;
    }
}

} // namespace certi

// $Id: RTIambassador.cc,v 3.32 2003/07/01 13:26:55 breholee Exp $
