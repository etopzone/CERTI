// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
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
// $Id: RTIambassador.cc,v 3.24 2003/05/05 20:21:39 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>

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

#include "RTIambassador.hh"

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
RTIambassador::RTIambassador(void)
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
RTIambassador::~RTIambassador(void)
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

    req.type = CREATE_FEDERATION_EXECUTION ;
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

    req.type = DESTROY_FEDERATION_EXECUTION ;
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

    req.type = JOIN_FEDERATION_EXECUTION ;
    req.setFederateName(yourName);
    req.setFederationName(executionName);

    executeService(&req, &rep);
    return rep.federate ;
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

    // envoyer la requete au RTI
    req.type = RESIGN_FEDERATION_EXECUTION ;
    req.resignAction = theAction ;
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

    req.type = REGISTER_FEDERATION_SYNCHRONIZATION_POINT ;
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

    req.type = REGISTER_FEDERATION_SYNCHRONIZATION_POINT ;
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

    req.type = SYNCHRONIZATION_POINT_ACHIEVED ;
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

    req.type = REQUEST_FEDERATION_SAVE ;
    req.date = (FederationTime) (RTIfedTime(theTime)._fedTime);
    req.setLabel(label);

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

    req.type = REQUEST_FEDERATION_SAVE ;
    req.date = (FederationTime)0 ;
    req.setLabel(label);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! Federate Save Begun.
void
RTIambassador::federateSaveBegun(void)
    throw (SaveNotInitiated,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RestoreInProgress,
           RTIinternalError)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = FEDERATE_SAVE_BEGUN ;

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

    req.type = FEDERATE_SAVE_COMPLETE ;

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

    req.type = FEDERATE_SAVE_NOT_COMPLETE ;

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

    req.type = REQUEST_FEDERATION_RESTORE ;
    req.setLabel(label);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! Restore Complete.
void
RTIambassador::federateRestoreComplete(void)
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

    req.type = FEDERATE_RESTORE_COMPLETE ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! Federate Restore Not Complete.
void
RTIambassador::federateRestoreNotComplete(void)
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

    req.type = FEDERATE_RESTORE_NOT_COMPLETE ;

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
    CAttributeHandleValuePairSet *attributeList_aux ;
    attributeList_aux = new CAttributeHandleValuePairSet(attributeList);
    CAttributeHandleValuePair *tmp ;

    // Envoyer la requete au RTI
    req.type = PUBLISH_OBJECT_CLASS ;
    req.objectClass = theClass ;
    req.handleArraySize = attributeList_aux->_size ;

    for (int i = 0 ; i < attributeList_aux->_size ; i++) {
        tmp = attributeList_aux->getIeme(i);
        req.handleArray[i] = tmp->_attrib ;
    }

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

    // envoyer la requete au RTI
    req.type = UNPUBLISH_OBJECT_CLASS ;
    req.objectClass = theClass ;
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

    // envoyer la requete au RTI
    req.type = PUBLISH_INTERACTION_CLASS ;
    req.interactionClass = theInteraction ;
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

    // envoyer la requete au RTI
    req.type = UNPUBLISH_INTERACTION_CLASS ;
    req.interactionClass = theInteraction ;
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Subscribe Object Class Attributes
void
RTIambassador::
subscribeObjectClassAttributes(ObjectClassHandle theClass,
                               const AttributeHandleSet& attributeList,
                               Boolean /*active*/)
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
    CAttributeHandleValuePairSet *attributeList_aux ;
    attributeList_aux = new CAttributeHandleValuePairSet(attributeList);
    CAttributeHandleValuePair *tmp ;

    // envoyer la requete au RTI
    req.type = SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES ;
    req.objectClass = theClass ;
    req.handleArraySize = attributeList_aux->_size ;

    for (int i = 0 ; i < attributeList_aux->_size ; i++) {
        tmp = attributeList_aux->getIeme(i);
        req.handleArray[i] = tmp->_attrib ;
    }

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

    // envoyer la requete au RTI
    req.type = UNSUBSCRIBE_OBJECT_CLASS ;
    req.objectClass = theClass ;
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

    // envoyer la requete au RTI
    req.type = SUBSCRIBE_INTERACTION_CLASS ;
    req.interactionClass = theClass ;
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

    // envoyer la requete au RTI
    req.type = UNSUBSCRIBE_INTERACTION_CLASS ;
    req.interactionClass = theClass ;
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

    // envoyer la requete au RTI
    req.type = REGISTER_OBJECT_INSTANCE ;
    req.setName((char *) theObjectName);
    req.objectClass = theClass ;
    executeService(&req, &rep);

    return rep.object ;
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

    // envoyer la requete au RTI
    req.type = REGISTER_OBJECT_INSTANCE ;
    req.setName("\0");
    req.objectClass = theClass ;

    executeService(&req, &rep);

    return rep.object ;
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
    ULong longueur ;
    Message req, rep ;

    CAttributeHandleValuePairSet *theAttributes_aux ;
    theAttributes_aux = new CAttributeHandleValuePairSet(theAttributes);

    CAttributeHandleValuePair *tmp ;

    // Envoyer la requete au RTI
    req.type = UPDATE_ATTRIBUTE_VALUES ;
    req.object = theObject ;
    req.date = (FederationTime) ((RTIfedTime&) theTime).getTime();
    req.setTag(theTag);
    req.handleArraySize = theAttributes_aux->_size ;

    for (int i = 0 ; i < theAttributes_aux->_size ; i++) {
        tmp = theAttributes_aux->getIeme(i);
        req.handleArray[i] = tmp->_attrib ;

        // codage
        getObjectToStringLength(tmp->_value.value,
                                tmp->_value.length,
                                longueur);
        char *valeur = new char[longueur] ;
        objectToString(tmp->_value.value, tmp->_value.length, valeur);
        req.setValue(i, valeur);
    }

    executeService(&req, &rep);
    delete theAttributes_aux ;
    return rep.eventRetraction ;
}

// ----------------------------------------------------------------------------
void
RTIambassador::updateAttributeValues(ObjectHandle,
                                     const AttributeHandleValuePairSet&,
                                     const char *)
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
    ULong longueur ;
    Message req, rep ;
    CParameterHandleValuePairSet *theParameters_aux ;
    theParameters_aux = new CParameterHandleValuePairSet(theParameters);
    CParameterHandleValuePair *tmp ;

    // envoyer la requete au RTI
    req.type = SEND_INTERACTION ;
    req.interactionClass = theInteraction ;
    req.date = (FederationTime) ((RTIfedTime&) theTime).getTime();
    req.setTag(theTag);
    req.handleArraySize = theParameters_aux->_size ;

    for (int i = 0 ; i < theParameters_aux->_size ; i++) {
        tmp = theParameters_aux->getIeme(i);
        req.handleArray[i] = tmp->_param ;

        // codage
        getObjectToStringLength(tmp->_value.value,
                                tmp->_value.length,
                                longueur);
        char *valeur = new char[longueur] ;
        objectToString(tmp->_value.value, tmp->_value.length, valeur);
        req.setValue(i, valeur);
    }

    executeService(&req, &rep);
    delete theParameters_aux ;
    return rep.eventRetraction ;
}

// ----------------------------------------------------------------------------
void
RTIambassador::sendInteraction(InteractionClassHandle,
                               const ParameterHandleValuePairSet &,
                               const char *)
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

    // envoyer la requete au RTI
    req.type = DELETE_OBJECT_INSTANCE ;
    req.object = theObject ;
    req.date = (FederationTime) ((RTIfedTime&) theTime).getTime();

    req.setTag(theTag);
    executeService(&req, &rep);
    return rep.eventRetraction ;
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

    req.type = DELETE_OBJECT_INSTANCE ;
    req.object = theObject ;
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

    req.type = LOCAL_DELETE_OBJECT_INSTANCE ;
    req.object = theObject ;

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
    CAttributeHandleValuePairSet *theAttributes_aux ;
    theAttributes_aux = new CAttributeHandleValuePairSet(theAttributes);
    CAttributeHandleValuePair *tmp ;
    TransportType theType_aux ;

    theType_aux = ((theType == 1) ? RELIABLE : BEST_EFFORT);

    // Envoyer la requete au RTI
    req.type = CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE ;
    req.object = theObject ;
    req.transport = theType_aux ;

    req.handleArraySize = theAttributes_aux->_size ;

    for (int i = 0 ; i < theAttributes_aux->_size ; i++) {
        tmp = theAttributes_aux->getIeme(i);
        req.handleArray[i] = tmp->_attrib ;
    }

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
    TransportType theType_aux ;

    theType_aux = ((theType == 1) ? RELIABLE : BEST_EFFORT);

    // envoyer la requete au RTI
    req.type = CHANGE_INTERACTION_TRANSPORTATION_TYPE ;
    req.interactionClass = theClass ;
    req.transport = theType_aux ;
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

    req.type = REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE ;
    req.object = theObject ;

    CAttributeHandleValuePairSet *theAttributes_aux ;
    theAttributes_aux = new CAttributeHandleValuePairSet(theAttributes);
    CAttributeHandleValuePair *tmp ;

    req.handleArraySize = theAttributes_aux->_size ;

    for (int i = 0 ; i < theAttributes_aux->_size ; i++) {
        tmp = theAttributes_aux->getIeme(i);
        req.handleArray[i] = tmp->_attrib ;
    }

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

    req.type = REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE ;
    req.objectClass = theClass ;

    CAttributeHandleValuePairSet *theAttributes_aux ;
    theAttributes_aux = new CAttributeHandleValuePairSet(attrs);
    CAttributeHandleValuePair *tmp ;

    req.handleArraySize = theAttributes_aux->_size ;

    for (int i = 0 ; i < theAttributes_aux->_size ; i++) {
        tmp = theAttributes_aux->getIeme(i);
        req.handleArray[i] = tmp->_attrib ;
    }

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

    // Envoyer la requete au RTI
    req.type = UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE ;
    req.object = theObject ;

    req.handleArraySize = attrs.size();

    for (int i = 0 ; i < attrs.size(); i++) {
        req.handleArray[i] = attrs.getHandle(i);
    }

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

    // Envoyer la requete au RTI
    req.type = NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE ;
    req.object = theObject ;

    req.setTag(theTag);

    req.handleArraySize = attrs.size();

    for (int i = 0 ; i < attrs.size(); i++) {
        req.handleArray[i] = attrs.getHandle(i);
    }

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

    // Envoyer la requete au RTI
    req.type = ATTRIBUTE_OWNERSHIP_ACQUISITION ;
    req.object = theObject ;

    req.setTag(theTag);

    req.handleArraySize = desiredAttributes.size();

    for (int i = 0 ; i < desiredAttributes.size(); i++) {
        req.handleArray[i] = desiredAttributes.getHandle(i);
    }

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

    // Envoyer la requete au RTI
    req.type = ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE ;
    req.object = theObject ;

    req.handleArraySize = attrs.size();

    for (int i = 0 ; i < attrs.size(); i++) {
        req.handleArray[i] = attrs.getHandle(i);
    }

    executeService(&req, &rep);

    if (rep.exception == e_NO_EXCEPTION) {
        AttributeHandleSet *AttributeSet ;
        AttributeSet = AttributeHandleSetFactory::create(rep.handleArraySize);

        for (int i = 0 ; i < rep.handleArraySize ; i++) {
            AttributeSet->add(rep.handleArray[i]);
        }
        return AttributeSet ;
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

    // Envoyer la requete au RTI
    req.type = CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE ;
    req.object = theObject ;

    req.handleArraySize = attrs.size();

    for (int i = 0 ; i < attrs.size(); i++) {
        req.handleArray[i] = attrs.getHandle(i);
    }

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

    // Envoyer la requete au RTI
    req.type = CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION ;
    req.object = (ObjectHandle) theObject ;

    req.handleArraySize = attrs.size();

    for (int i = 0 ; i < attrs.size(); i++) {
        req.handleArray[i] = attrs.getHandle(i);
    }

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

    // Envoyer la requete au RTI
    req.type = ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE ;
    req.object = theObject ;

    req.handleArraySize = desired.size();

    for (int i = 0 ; i < desired.size(); i++) {
        req.handleArray[i] = desired.getHandle(i);
        D.Out(pdTrace, "Objet %u Attribut %u", theObject, req.handleArray[i]);
    }

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

    // envoyer la requete au RTIA/RTIG
    req.type = QUERY_ATTRIBUTE_OWNERSHIP ;
    req.object = theObject ;
    req.attribute = theAttribute ;

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

    // envoyer la requete au RTIA
    req.type = IS_ATTRIBUTE_OWNED_BY_FEDERATE ;
    req.object = theObject ;
    req.attribute = theAttribute ;

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

    // envoyer la requete au RTI
    req.type = ENABLE_TIME_REGULATION ;
    req.boolean = RTI_TRUE ;
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Time Regulation
void
RTIambassador::disableTimeRegulation(void)
    throw (FederationTimeAlreadyPassed,
           TimeRegulationWasNotEnabled, //not implemented
           ConcurrentAccessAttempted,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    // envoyer la requete au RTI
    req.type = DISABLE_TIME_REGULATION ;
    req.boolean = RTI_FALSE ;
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Enable Time Constrained
void
RTIambassador::enableTimeConstrained(void)
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

    // envoyer la requete au RTI
    req.type = ENABLE_TIME_CONSTRAINED ;
    req.boolean = RTI_TRUE ;
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Time Constrained
void
RTIambassador::disableTimeConstrained(void)
    throw (TimeConstrainedWasNotEnabled, //not implemented
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    // envoyer la requete au RTI
    req.type = DISABLE_TIME_CONSTRAINED ;
    req.boolean = RTI_FALSE ;
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

    // envoyer la requete au RTI
    req.type = TIME_ADVANCE_REQUEST ;
    req.date = (FederationTime) (RTIfedTime(theTime)._fedTime);
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Time Advance Request Available
void
RTIambassador::timeAdvanceRequestAvailable(const FedTime& /*theTime*/)
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

    // envoyer la requete au RTI
    req.type = NEXT_EVENT_REQUEST ;
    req.date = (FederationTime) ((RTIfedTime&) theTime).getTime();
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

    // envoyer la requete au RTI
    req.type = NEXT_EVENT_REQUEST ;
    req.date = (FederationTime) ((RTIfedTime&) theTime).getTime();
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

    req.type = FLUSH_QUEUE_REQUEST ;
    req.date = (FederationTime) ((RTIfedTime&) theTime).getTime();

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Enable Asynchronous Delivery
void
RTIambassador::enableAsynchronousDelivery(void)
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

    req.type = ENABLE_ASYNCHRONOUS_DELIVERY ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Asynchronous Delivery
void
RTIambassador::disableAsynchronousDelivery(void)
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

    req.type = DISABLE_ASYNCHRONOUS_DELIVERY ;

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

    // envoyer la requete au RTI
    req.type = QUERY_LBTS ;
    executeService(&req, &rep);

    theTime = RTIfedTime(rep.date);
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

    // envoyer la requete au RTI
    req.type = QUERY_FEDERATE_TIME ;
    executeService(&req, &rep);

    theTime = RTIfedTime(rep.date);
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

    // envoyer la requete au RTI
    req.type = MODIFY_LOOKAHEAD ;
    req.lookahead =
        (FederationTimeDelta) ((RTIfedTime&) theLookahead).getTime();
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

    // envoyer la requete au RTI
    req.type = QUERY_LOOKAHEAD ;
    executeService(&req, &rep);

    RTIfedTime *tmp = new RTIfedTime((Double) rep.lookahead);
    theTime = *(dynamic_cast<FedTime *>(tmp));
}

// ----------------------------------------------------------------------------
// Retract
void
RTIambassador::retract(EventRetractionHandle theHandle)
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

    req.type = RETRACT ;
    req.eventRetraction = theHandle ;

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
    CAttributeHandleValuePairSet *attrs_aux ;
    attrs_aux = new CAttributeHandleValuePairSet(attrs);
    CAttributeHandleValuePair *tmp ;

    OrderType theType_aux ;
    theType_aux = ((theType == 1) ? RECEIVE : TIMESTAMP);

    // Envoyer la requete au RTI
    req.type = CHANGE_ATTRIBUTE_ORDER_TYPE ;
    req.object = theObject ;
    req.order = theType_aux ;

    req.handleArraySize = attrs_aux->_size ;

    for (int i = 0 ; i < attrs_aux->_size ; i++) {
        tmp = attrs_aux->getIeme(i);
        req.handleArray[i] = tmp->_attrib ;
    }

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

    OrderType theType_aux ;
    theType_aux = ((theType == 1) ? RECEIVE : TIMESTAMP);

    // envoyer la requete au RTI
    req.type = CHANGE_INTERACTION_ORDER_TYPE ;
    req.interactionClass = theClass ;
    req.order = theType_aux ;
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

    req.setType(CREATE_REGION);
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
RTIambassador::notifyAboutRegionModification(Region &)
    throw (RegionNotKnown,
           InvalidExtents,
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

    req.setType(DELETE_REGION);
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
RTIambassador::associateRegionForUpdates(Region &,
                                         ObjectHandle,
                                         const AttributeHandleSet &)
    throw (ObjectNotKnown,
           AttributeNotDefined,
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

    // envoyer la requete au RTI
    req.type = GET_OBJECT_CLASS_HANDLE ;
    req.setName(theName);

    executeService(&req, &rep);

    return rep.objectClass ;
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

    req.type = GET_OBJECT_CLASS_NAME ;
    req.objectClass = handle ;

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

    // envoyer la requete au RTI
    req.type = GET_ATTRIBUTE_HANDLE ;
    req.setName((AttributeName) theName);
    req.objectClass = whichClass ;

    executeService(&req, &rep);

    return rep.attribute ;
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

    req.type = GET_ATTRIBUTE_NAME ;
    req.attribute = theHandle ;
    req.objectClass = whichClass ;

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

    // envoyer la requete au RTI
    req.type = GET_INTERACTION_CLASS_HANDLE ;
    req.setName(theName);

    executeService(&req, &rep);

    return rep.interactionClass ;
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

    req.type = GET_INTERACTION_CLASS_NAME ;
    req.interactionClass = theHandle ;

    executeService(&req, &rep); // Send request to RTI

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

    // envoyer la requete au RTI
    req.type = GET_PARAMETER_HANDLE ;
    req.setName((ParameterName) theName);
    req.interactionClass = whichClass ;

    executeService(&req, &rep);

    return rep.parameter ;
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

    req.type = GET_PARAMETER_NAME ;
    req.parameter = theHandle ;
    req.interactionClass = whichClass ;

    executeService(&req, &rep); // Send request to RTI.

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

    req.type = GET_OBJECT_INSTANCE_HANDLE ;
    req.setName(theName);

    executeService(&req, &rep);

    return(rep.object);
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

    req.type = GET_OBJECT_INSTANCE_NAME ;
    req.object = theHandle ;

    executeService(&req, &rep);

    return(strdup(rep.getName()));
}

// ----------------------------------------------------------------------------
// Get Routing Space Handle
SpaceHandle
RTIambassador::getRoutingSpaceHandle(const char *name)
    throw (NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;

    req.type = GET_SPACE_HANDLE ;
    req.setName(name);
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

    req.type = GET_SPACE_NAME ;
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

    req.type = GET_DIMENSION_HANDLE ;
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

    req.type = GET_DIMENSION_NAME ;
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

    req.type = GET_ATTRIBUTE_SPACE_HANDLE ;
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

    req.type = GET_OBJECT_CLASS ;
    req.object = theObject ;

    executeService(&req, &rep);

    return(rep.objectClass);
}

// ----------------------------------------------------------------------------
// Get Interaction Routing Space Handle
SpaceHandle
RTIambassador::getInteractionRoutingSpaceHandle(InteractionClassHandle inter)
    throw (InteractionClassNotDefined, FederateNotExecutionMember,
           ConcurrentAccessAttempted, RTIinternalError, UnimplementedService)
{
    Message req, rep ;

    req.type = GET_INTERACTION_SPACE_HANDLE ;
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

    req.type = GET_TRANSPORTATION_HANDLE ;
    req.setName(theName);

    executeService(&req, &rep);

    return(rep.transportation);
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

    req.type = GET_TRANSPORTATION_NAME ;
    req.transportation = theHandle ;

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

    req.type = GET_ORDERING_HANDLE ;
    req.setName(theName);

    executeService(&req, &rep);

    return(rep.ordering);
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

    req.type = GET_ORDERING_NAME ;
    req.ordering = theHandle ;

    executeService(&req, &rep);

    return(strdup(rep.getName()));
}

// ----------------------------------------------------------------------------
// Enable Class Relevance Advisory Switch
void
RTIambassador::enableClassRelevanceAdvisorySwitch(void)
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Class Relevance Advisory Switch
void
RTIambassador::disableClassRelevanceAdvisorySwitch(void)
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;

    req.type = DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH ;

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

    req.type = ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH ;

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

    req.type = DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH ;

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

    req.type = ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH ;

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

    req.type = DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH ;

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

    req.type = ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH ;

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

    req.type = DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH ;

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Tick
Boolean
RTIambassador::tick(void)
    throw (SpecifiedSaveLabelDoesNotExist,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    int i = 0 ;
    Message vers_RTI, vers_Fed ;

    CAttributeHandleValuePairSet theAttributes ;
    CParameterHandleValuePairSet theParameters ;

    // Throw exception if reentrant call.
    if (is_reentrant)
        throw ConcurrentAccessAttempted();

    is_reentrant = true ;

    // Prevenir le RTI
    vers_RTI.type = TICK_REQUEST ;

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
        if (vers_Fed.type == TICK_REQUEST) {
            is_reentrant = false ;
            processException(&vers_Fed);
            return vers_Fed.boolean ;
        }

        // Sinon, le RTI nous demande un service, donc on appele une methode
        // du FederateAmbassador.

        vers_RTI.exception = e_NO_EXCEPTION ;
        vers_RTI.exceptionReason[0] = 0 ;
        try {
            switch (vers_Fed.type) {

              case SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
                fed_amb->synchronizationPointRegistrationSucceeded(vers_Fed.getLabel());
                break ;

              case ANNOUNCE_SYNCHRONIZATION_POINT:
                fed_amb->announceSynchronizationPoint(vers_Fed.getLabel(),
                                                      vers_Fed.getTag());
                break ;

              case FEDERATION_SYNCHRONIZED:
                fed_amb->federationSynchronized(vers_Fed.getLabel());
                break ;

              case INITIATE_FEDERATE_SAVE:
                fed_amb->initiateFederateSave(vers_Fed.getLabel());
                break ;

              case FEDERATION_SAVED:
                fed_amb->federationSaved();
                break ;

              case REQUEST_FEDERATION_RESTORE_SUCCEEDED:
                fed_amb->requestFederationRestoreSucceeded(
                    vers_Fed.getLabel());
                break ;

              case REQUEST_FEDERATION_RESTORE_FAILED:
                fed_amb->requestFederationRestoreFailed(vers_Fed.getLabel(),
                                                        vers_Fed.getTag());
                break ;

              case FEDERATION_RESTORE_BEGUN:
                fed_amb->federationRestoreBegun();
                break ;

              case INITIATE_FEDERATE_RESTORE:
                fed_amb->initiateFederateRestore(vers_Fed.getLabel(),
                                                 vers_Fed.federate);
                break ;

              case FEDERATION_RESTORED:
                fed_amb->federationRestored();
                break ;

              case FEDERATION_NOT_RESTORED:
                fed_amb->federationNotRestored();
                break ;

              case START_REGISTRATION_FOR_OBJECT_CLASS: {
                  fed_amb->
                      startRegistrationForObjectClass(vers_Fed.objectClass);
              } break ;

              case STOP_REGISTRATION_FOR_OBJECT_CLASS: {
                  fed_amb->
                      stopRegistrationForObjectClass(vers_Fed.objectClass);
              } break ;

              case TURN_INTERACTIONS_ON: {
                  fed_amb->turnInteractionsOn(vers_Fed.interactionClass);
              } break ;

              case TURN_INTERACTIONS_OFF: {
                  fed_amb->turnInteractionsOff(vers_Fed.interactionClass);
              } break ;

              case DISCOVER_OBJECT_INSTANCE: {
                  fed_amb->
                      discoverObjectInstance(vers_Fed.object,
                                             vers_Fed.objectClass,
                                             (char *) (vers_Fed.getName()));
              } break ;

              case REFLECT_ATTRIBUTE_VALUES: {
                  for (i = 0 ; i < vers_Fed.handleArraySize ; i++) {
                      CAttributeHandleValuePair *att =
                          new CAttributeHandleValuePair ;

                      att->_attrib = vers_Fed.handleArray[i] ;

                      // BUG: Federate may be expecting to find value name
                      // (a call to GetWithName for example).
                      strcpy(att->_value.name, "");
                      vers_Fed.getValue(i, att->_value.value);

                      // BUG: Federate is expecting to find value type.
                      strcpy(att->_value.type, "");

                      theAttributes.add(att);
                  }

                  AttributeHandleValuePairSet* theAttributes_aux =
                      theAttributes.toAHVPS();

                  fed_amb->
                      reflectAttributeValues(vers_Fed.object,
                                             *theAttributes_aux,
                                             RTIfedTime(vers_Fed.date),
                                             (char *) vers_Fed.getTag(),
                                             vers_Fed.eventRetraction);

                  delete theAttributes_aux ;
              } break ;

              case RECEIVE_INTERACTION: {
                  for (i = 0 ; i < vers_Fed.handleArraySize ; i++) {
                      CParameterHandleValuePair *par =
                          new CParameterHandleValuePair ;

                      par->_param = vers_Fed.handleArray[i] ;

                      // BUG: Federate may be expecting to find value name
                      // (a call to GetWithName for example).
                      strcpy(par->_value.name, "");

                      vers_Fed.getValue(i, par->_value.value);

                      // BUG: Federate is expecting to find value type.
                      strcpy(par->_value.type, "");

                      theParameters.add(par);
                  }

                  ParameterHandleValuePairSet *theParameters_aux ;
                  theParameters_aux = theParameters.toPHVPS();

                  fed_amb->receiveInteraction(vers_Fed.interactionClass,
                                              *theParameters_aux,
                                              RTIfedTime(vers_Fed.date),
                                              (char *) vers_Fed.getTag(),
                                              vers_Fed.eventRetraction);

                  delete theParameters_aux ;
              } break ;

              case REMOVE_OBJECT_INSTANCE: {
                  fed_amb->removeObjectInstance(vers_Fed.object,
                                                RTIfedTime(vers_Fed.date),
                                                (char *) vers_Fed.getTag(),
                                                vers_Fed.eventRetraction);
              } break ;

              case PROVIDE_ATTRIBUTE_VALUE_UPDATE: {
                  // fed_amb->provideAttributeValueUpdate();
              } break ;

              case REQUEST_RETRACTION: {
                  // fed_amb->reflectRetraction();
              } break ;

              case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION: {

                  AttributeHandleSet *AttributeSet ;
                  AttributeSet =
                      AttributeHandleSetFactory::create(vers_Fed.handleArraySize);

                  for (int i = 0 ; i < vers_Fed.handleArraySize ; i++) {
                      AttributeSet->add(vers_Fed.handleArray[i]);
                  }

                  fed_amb->
                      requestAttributeOwnershipAssumption(vers_Fed.object,
                                                          *AttributeSet,
                                                          (char *)
                                                          vers_Fed.getTag());
                  AttributeSet->empty();
              } break ;

              case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE: {

                  AttributeHandleSet *AttributeSet ;
                  AttributeSet =
                      AttributeHandleSetFactory::create(vers_Fed.handleArraySize);

                  for (int i = 0 ; i < vers_Fed.handleArraySize ; i++) {
                      AttributeSet->add(vers_Fed.handleArray[i]);
                  }

                  fed_amb->requestAttributeOwnershipRelease(vers_Fed.object,
                                                            *AttributeSet,
                                                            (char *)
                                                            vers_Fed.getTag());

                  AttributeSet->empty();
              } break ;

              case ATTRIBUTE_OWNERSHIP_UNAVAILABLE: {

                  AttributeHandleSet *AttributeSet ;
                  AttributeSet =
                      AttributeHandleSetFactory::create(vers_Fed.handleArraySize);

                  for (int i = 0 ; i < vers_Fed.handleArraySize ; i++) {
                      AttributeSet->add(vers_Fed.handleArray[i]);
                  }
                  fed_amb->attributeOwnershipUnavailable(vers_Fed.object,
                                                         *AttributeSet);
                  AttributeSet->empty();
              } break ;

              case ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION: {

                  AttributeHandleSet *AttributeSet ;
                  AttributeSet =
                      AttributeHandleSetFactory::create(vers_Fed.handleArraySize);

                  for (int i = 0 ; i < vers_Fed.handleArraySize ; i++) {
                      AttributeSet->add(vers_Fed.handleArray[i]);
                  }
                  fed_amb->
                      attributeOwnershipAcquisitionNotification(vers_Fed.object,
                                                                *AttributeSet);
                  AttributeSet->empty();
              } break ;

              case ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION: {

                  AttributeHandleSet *AttributeSet ;
                  AttributeSet =
                      AttributeHandleSetFactory::create(vers_Fed.handleArraySize);

                  for (int i = 0 ; i < vers_Fed.handleArraySize ; i++) {
                      AttributeSet->add(vers_Fed.handleArray[i]);
                  }
                  fed_amb->
                      attributeOwnershipDivestitureNotification(vers_Fed.object,
                                                                *AttributeSet);
                  AttributeSet->empty();
              } break ;

              case CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION: {

                  AttributeHandleSet *AttributeSet ;
                  AttributeSet =
                      AttributeHandleSetFactory::create(vers_Fed.handleArraySize);

                  for (int i = 0 ; i < vers_Fed.handleArraySize ; i++) {
                      AttributeSet->add(vers_Fed.handleArray[i]);
                  }
                  fed_amb->
                      confirmAttributeOwnershipAcquisitionCancellation
                      (vers_Fed.object, *AttributeSet);
                  AttributeSet->empty();
              } break ;

              case INFORM_ATTRIBUTE_OWNERSHIP: {
                  fed_amb->
                      informAttributeOwnership(vers_Fed.object,
                                               vers_Fed.attribute,
                                               vers_Fed.federate);
              } break ;

              case ATTRIBUTE_IS_NOT_OWNED: {
                  fed_amb->attributeIsNotOwned(vers_Fed.object,
                                               vers_Fed.attribute);
              } break ;

              case TIME_ADVANCE_GRANT: {
                  fed_amb->timeAdvanceGrant(RTIfedTime(vers_Fed.date));
              } break ;

              default: {
                  leave("RTI service requested by RTI is unknown.");
              }
            }
        }
        catch (InvalidFederationTime &e) {
            vers_RTI.exception = e_InvalidFederationTime ;
            strcpy(vers_RTI.exceptionReason, e._reason);
        }
        catch (TimeAdvanceWasNotInProgress &e) {
            vers_RTI.exception = e_TimeAdvanceWasNotInProgress ;
            strcpy(vers_RTI.exceptionReason, e._reason);
        }
        catch (FederationTimeAlreadyPassed &e) {
            vers_RTI.exception = e_FederationTimeAlreadyPassed ;
            strcpy(vers_RTI.exceptionReason, e._reason);
        }
        catch (FederateInternalError &e) {
            vers_RTI.exception = e_FederateInternalError ;
            strcpy(vers_RTI.exceptionReason, e._reason);
        }
        catch (Exception &e) {
            vers_RTI.exception = e_RTIinternalError ;
            strcpy(vers_RTI.exceptionReason, e._reason);
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
RTIambassador::tick(TickTime /*minimum*/,
                    TickTime /*maximum*/)
    throw (SpecifiedSaveLabelDoesNotExist,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    int i = 0 ;
    Message vers_RTI, vers_Fed ;

    CAttributeHandleValuePairSet theAttributes ;
    CParameterHandleValuePairSet theParameters ;

    // Lever une exception si appel reentrant
    if (is_reentrant)
        throw ConcurrentAccessAttempted();

    is_reentrant = true ;

    // Prevenir le RTI
    vers_RTI.type = TICK_REQUEST ;

    try {
        vers_RTI.write((SocketUN *) this);
    }
    catch (NetworkError) {
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
            cout << "LibRTI: Catched NetworkError, throw RTIinternalError."
                 << endl ;
            throw RTIinternalError();
        }

        // Si c'est de type TICK_REQUEST, il n'y a qu'a traiter l'exception.
        if (vers_Fed.type == TICK_REQUEST) {
            is_reentrant = false ;
            processException(&vers_Fed);
            return vers_Fed.boolean ;
        }

        // Sinon, le RTI nous demande un service, donc on appele une
        // methode du FederateAmbassador.
        vers_RTI.exception = e_NO_EXCEPTION ;
        vers_RTI.exceptionReason[0] = 0 ;

        try {
            switch(vers_Fed.type) {

              case SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
                fed_amb->synchronizationPointRegistrationSucceeded(vers_Fed.getLabel());
                break ;

              case ANNOUNCE_SYNCHRONIZATION_POINT:
                fed_amb->announceSynchronizationPoint(vers_Fed.getLabel(),
                                                      vers_Fed.getTag());
                break ;

              case FEDERATION_SYNCHRONIZED:
                fed_amb->federationSynchronized(vers_Fed.getLabel());
                break ;

              case INITIATE_FEDERATE_SAVE:
                fed_amb->initiateFederateSave(vers_Fed.getLabel());
                break ;

              case FEDERATION_SAVED:
                fed_amb->federationSaved();
                break ;

              case REQUEST_FEDERATION_RESTORE_SUCCEEDED:
                fed_amb->requestFederationRestoreSucceeded(
                    vers_Fed.getLabel());
                break ;

              case REQUEST_FEDERATION_RESTORE_FAILED:
                fed_amb->requestFederationRestoreFailed(vers_Fed.getLabel(),
                                                        vers_Fed.getTag());
                break ;

              case FEDERATION_RESTORE_BEGUN:
                fed_amb->federationRestoreBegun();
                break ;

              case INITIATE_FEDERATE_RESTORE:
                fed_amb->initiateFederateRestore(vers_Fed.getLabel(),
                                                 vers_Fed.federate);
                break ;

              case FEDERATION_RESTORED:
                fed_amb->federationRestored();
                break ;

              case FEDERATION_NOT_RESTORED:
                fed_amb->federationNotRestored();
                break ;

              case START_REGISTRATION_FOR_OBJECT_CLASS: {
                  fed_amb->
                      startRegistrationForObjectClass(vers_Fed.objectClass);
              } break ;

              case STOP_REGISTRATION_FOR_OBJECT_CLASS: {
                  fed_amb->
                      stopRegistrationForObjectClass(vers_Fed.objectClass);
              } break ;

              case TURN_INTERACTIONS_ON: {
                  fed_amb->turnInteractionsOn(vers_Fed.interactionClass);
              } break ;

              case TURN_INTERACTIONS_OFF: {
                  fed_amb->turnInteractionsOff(vers_Fed.interactionClass);
              } break ;

              case DISCOVER_OBJECT_INSTANCE: {
                  fed_amb->
                      discoverObjectInstance(vers_Fed.object,
                                             vers_Fed.objectClass,
                                             (char *) (vers_Fed.getName()));
              } break ;

              case REFLECT_ATTRIBUTE_VALUES: {

                  for (i = 0 ; i < vers_Fed.handleArraySize ; i++) {
                      CAttributeHandleValuePair *att =
                          new CAttributeHandleValuePair ;

                      att->_attrib = vers_Fed.handleArray[i] ;
                      strcpy(att->_value.name, "");
                      vers_Fed.getValue(i, att->_value.value);

                      // BUG: Le federe s'attend surement a trouver le
                      // type de la valeur.
                      strcpy(att->_value.type, "");
                      theAttributes.add(att);
                  }

                  AttributeHandleValuePairSet *theAttributes_aux =
                      theAttributes.toAHVPS();

                  fed_amb->
                      reflectAttributeValues(vers_Fed.object,
                                             *theAttributes_aux,
                                             RTIfedTime(vers_Fed.date),
                                             (char *) vers_Fed.getTag(),
                                             vers_Fed.eventRetraction);
              } break ;

              case RECEIVE_INTERACTION: {

                  for (i = 0 ; i < vers_Fed.handleArraySize ; i++) {

                      CParameterHandleValuePair *par =
                          new CParameterHandleValuePair ;

                      par->_param = vers_Fed.handleArray[i] ;
                      strcpy(par->_value.name, "");
                      vers_Fed.getValue(i, par->_value.value);

                      // Pareil pour le type de la valeur.
                      strcpy(par->_value.type, "");
                      theParameters.add(par);
                  }

                  ParameterHandleValuePairSet *theParameters_aux =
                      theParameters.toPHVPS();

                  fed_amb->receiveInteraction(vers_Fed.interactionClass,
                                              *theParameters_aux,
                                              RTIfedTime(vers_Fed.date),
                                              (char *) vers_Fed.getTag(),
                                              vers_Fed.eventRetraction);
              } break ;

              case REMOVE_OBJECT_INSTANCE: {
                  fed_amb->removeObjectInstance(vers_Fed.object,
                                                RTIfedTime(vers_Fed.date),
                                                (char *) vers_Fed.getTag(),
                                                vers_Fed.eventRetraction);
              } break ;

              case PROVIDE_ATTRIBUTE_VALUE_UPDATE: {
                  // fed_amb->provideAttributeValueUpdate();
              } break ;

              case REQUEST_RETRACTION: {
                  // fed_amb->reflectRetraction();
              } break ;

              case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION: {

                  AttributeHandleSet *AttributeSet ;
                  AttributeSet =
                      AttributeHandleSetFactory::create(vers_Fed.handleArraySize);

                  for (int i = 0 ; i < vers_Fed.handleArraySize ; i++) {
                      AttributeSet->add(vers_Fed.handleArray[i]);
                  }

                  fed_amb->
                      requestAttributeOwnershipAssumption(vers_Fed.object,
                                                          *AttributeSet,
                                                          (char *)
                                                          vers_Fed.getTag());
                  AttributeSet->empty();
              } break ;

              case ATTRIBUTE_OWNERSHIP_UNAVAILABLE: {

                  AttributeHandleSet *AttributeSet ;
                  AttributeSet =
                      AttributeHandleSetFactory::create(vers_Fed.handleArraySize);

                  for (int i = 0 ; i < vers_Fed.handleArraySize ; i++) {
                      AttributeSet->add(vers_Fed.handleArray[i]);
                  }
                  fed_amb->
                      attributeOwnershipUnavailable(vers_Fed.object,
                                                    *AttributeSet);
                  AttributeSet->empty();
              } break ;

              case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE: {

                  AttributeHandleSet *AttributeSet ;
                  AttributeSet =
                      AttributeHandleSetFactory::create(vers_Fed.handleArraySize);

                  for (int i = 0 ; i < vers_Fed.handleArraySize ; i++) {
                      AttributeSet->add(vers_Fed.handleArray[i]);
                  }

                  fed_amb->
                      requestAttributeOwnershipRelease((ObjectHandle)
                                                       vers_Fed.object,
                                                       *AttributeSet,
                                                       (char *)
                                                       vers_Fed.getTag());
                  AttributeSet->empty();
              } break ;

              case ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION: {

                  AttributeHandleSet *AttributeSet ;
                  AttributeSet =
                      AttributeHandleSetFactory::create(vers_Fed.handleArraySize);

                  for (int i = 0 ; i < vers_Fed.handleArraySize ; i++) {
                      AttributeSet->add(vers_Fed.handleArray[i]);
                  }
                  fed_amb->
                      attributeOwnershipAcquisitionNotification((ObjectHandle)
                                                                vers_Fed.object,
                                                                *AttributeSet);
                  AttributeSet->empty();
              } break ;

              case ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION: {

                  AttributeHandleSet *AttributeSet ;
                  AttributeSet =
                      AttributeHandleSetFactory::create(vers_Fed.handleArraySize);

                  for (int i = 0 ; i < vers_Fed.handleArraySize ; i++) {
                      AttributeSet->add(vers_Fed.handleArray[i]);
                  }
                  fed_amb->
                      attributeOwnershipDivestitureNotification((ObjectHandle)
                                                                vers_Fed.object,
                                                                *AttributeSet);
                  AttributeSet->empty();
              } break ;

              case CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION: {

                  AttributeHandleSet *AttributeSet ;
                  AttributeSet =
                      AttributeHandleSetFactory::create(vers_Fed.handleArraySize);

                  for (int i = 0 ; i < vers_Fed.handleArraySize ; i++) {
                      AttributeSet->add(vers_Fed.handleArray[i]);
                  }
                  fed_amb->
                      confirmAttributeOwnershipAcquisitionCancellation
                      ((ObjectHandle) vers_Fed.object, *AttributeSet);
                  AttributeSet->empty();
              } break ;

              case INFORM_ATTRIBUTE_OWNERSHIP: {
                  fed_amb->
                      informAttributeOwnership((ObjectHandle) vers_Fed.object,
                                               vers_Fed.attribute,
                                               vers_Fed.federate);
              } break ;

              case ATTRIBUTE_IS_NOT_OWNED: {
                  fed_amb->attributeIsNotOwned((ObjectHandle) vers_Fed.object,
                                               vers_Fed.attribute);
              } break ;

              case TIME_ADVANCE_GRANT: {
                  fed_amb->timeAdvanceGrant(RTIfedTime(vers_Fed.date));
              } break ;

              default: {
                  leave("RTI service requested by RTI is unknown.");
              }
            }
        }
        catch (InvalidFederationTime &e) {
            vers_RTI.exception = e_InvalidFederationTime ;
            strcpy(vers_RTI.exceptionReason, e._reason);
        }
        catch (TimeAdvanceWasNotInProgress &e) {
            vers_RTI.exception = e_TimeAdvanceWasNotInProgress ;
            strcpy(vers_RTI.exceptionReason, e._reason);
        }
        catch (FederationTimeAlreadyPassed &e) {
            vers_RTI.exception = e_FederationTimeAlreadyPassed ;
            strcpy(vers_RTI.exceptionReason, e._reason);
        }
        catch (FederateInternalError &e) {
            vers_RTI.exception = e_FederateInternalError ;
            strcpy(vers_RTI.exceptionReason, e._reason);
        }
        catch (Exception &e) {
            vers_RTI.exception = e_RTIinternalError ;
            strcpy(vers_RTI.exceptionReason, e._reason);
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
//! Process exception from received message.
/*! When a message is received from RTIA, it can contains an exception.
  This exception is processed by this module and a new exception is thrown.
*/
void
RTIambassador::processException(Message *msg)
{
    D.Out(pdExcept, "nom de l'exception : %d .", msg->exception);
    switch(msg->exception) {
      case e_NO_EXCEPTION: {
      } break ;

      case e_ArrayIndexOutOfBounds: {
          D.Out(pdExcept, "Throwing e_ArrayIndexOutOfBounds exception.");
          throw ArrayIndexOutOfBounds(msg->exceptionReason);
      } break ;

      case e_AttributeAlreadyOwned: {
          D.Out(pdExcept, "Throwing e_AttributeAlreadyOwned exception.");
          throw AttributeAlreadyOwned(msg->exceptionReason);
      } break ;

      case e_AttributeAlreadyBeingAcquired: {
          D.Out(pdExcept, "Throwing e_AttributeAlreadyBeingAcquired exception.");
          throw AttributeAlreadyBeingAcquired(msg->exceptionReason);
      } break ;

      case e_AttributeAlreadyBeingDivested: {
          D.Out(pdExcept, "Throwing e_AttributeAlreadyBeingDivested exception.");
          throw AttributeAlreadyBeingDivested(msg->exceptionReason);
      } break ;

      case e_AttributeDivestitureWasNotRequested: {
          D.Out(pdExcept,
                "Throwing e_AttributeDivestitureWasNotRequested exception.");
          throw AttributeDivestitureWasNotRequested(msg->exceptionReason);
      } break ;

      case e_AttributeAcquisitionWasNotRequested: {
          D.Out(pdExcept,
                "Throwing e_AttributeAcquisitionWasNotRequested exception.");
          throw AttributeAcquisitionWasNotRequested(msg->exceptionReason);
      } break ;

      case e_AttributeNotDefined: {
          D.Out(pdExcept, "Throwing e_AttributeNotDefined exception.");
          throw AttributeNotDefined(msg->exceptionReason);
      } break ;

      case e_AttributeNotKnown: {
          D.Out(pdExcept, "Throwing e_AttributeNotKnown exception.");
          throw AttributeNotKnown(msg->exceptionReason);
      } break ;

      case e_AttributeNotOwned: {
          D.Out(pdExcept, "Throwing e_AttributeNotOwned exception.");
          throw AttributeNotOwned(msg->exceptionReason);
      } break ;

      case e_AttributeNotPublished: {
          D.Out(pdExcept, "Throwing e_AttributeNotPublished exception.");
          throw AttributeNotPublished(msg->exceptionReason);
      } break ;

      case e_AttributeNotSubscribed: {
          D.Out(pdExcept, "Throwing e_AttributeNotSubscribed exception.");
          throw AttributeNotSubscribed(msg->exceptionReason);
      } break ;

      case e_ConcurrentAccessAttempted: {
          D.Out(pdExcept, "Throwing e_ConcurrentAccessAttempted exception.");
          throw ConcurrentAccessAttempted(msg->exceptionReason);
      } break ;

      case e_CouldNotDiscover: {
          D.Out(pdExcept, "Throwing e_CouldNotDiscover exception.");
          throw CouldNotDiscover(msg->exceptionReason);
      } break ;

      case e_CouldNotOpenRID: {
          D.Out(pdExcept, "Throwing e_CouldNotOpenRID exception.");
          throw CouldNotOpenRID(msg->exceptionReason);
      } break ;

      case e_CouldNotRestore: {
          D.Out(pdExcept, "Throwing e_CouldNotRestore exception.");
          throw CouldNotRestore(msg->exceptionReason);
      } break ;

      case e_DeletePrivilegeNotHeld: {
          D.Out(pdExcept, "Throwing e_DeletePrivilegeNotHeld exception.");
          throw DeletePrivilegeNotHeld(msg->exceptionReason);
      } break ;

      case e_ErrorReadingRID: {
          D.Out(pdExcept, "Throwing e_ErrorReadingRID exception.");
          throw ErrorReadingRID(msg->exceptionReason);
      } break ;

      case e_EventNotKnown: {
          D.Out(pdExcept, "Throwing e_EventNotKnown exception.");
          throw EventNotKnown(msg->exceptionReason);
      } break ;

      case e_FederateAlreadyPaused: {
          D.Out(pdExcept, "Throwing e_FederateAlreadyPaused exception.");
          throw FederateAlreadyPaused(msg->exceptionReason);
      } break ;

      case e_FederateAlreadyExecutionMember: {
          D.Out(pdExcept, "Throwing e_FederateAlreadyExecutionMember exception.");
          throw FederateAlreadyExecutionMember(msg->exceptionReason);
      } break ;

      case e_FederateDoesNotExist: {
          D.Out(pdExcept, "Throwing e_FederateDoesNotExist exception.");
          throw FederateDoesNotExist(msg->exceptionReason);
      } break ;

      case e_FederateInternalError: {
          D.Out(pdExcept, "Throwing e_FederateInternalError exception.");
          throw FederateInternalError(msg->exceptionReason);
      } break ;

      case e_FederateNameAlreadyInUse: {
          D.Out(pdExcept, "Throwing e_FederateNameAlreadyInUse exception.");
          throw FederateNameAlreadyInUse(msg->exceptionReason);
      } break ;

      case e_FederateNotExecutionMember: {
          D.Out(pdExcept, "Throwing e_FederateNotExecutionMember exception.");
          throw FederateNotExecutionMember(msg->exceptionReason);
      } break ;

      case e_FederateNotPaused: {
          D.Out(pdExcept, "Throwing e_FederateNotPaused exception.");
          throw FederateNotPaused(msg->exceptionReason);
      } break ;

      case e_FederateOwnsAttributes: {
          D.Out(pdExcept, "Throwing e_FederateOwnsAttributes exception.");
          throw FederateOwnsAttributes(msg->exceptionReason);
      } break ;

      case e_FederatesCurrentlyJoined: {
          D.Out(pdExcept, "Throwing e_FederatesCurrentlyJoined exception.");
          throw FederatesCurrentlyJoined(msg->exceptionReason);
      } break ;

      case e_FederateWasNotAskedToReleaseAttribute: {
          D.Out(pdExcept,
                "Throwing e_FederateWasNotAskedToReleaseAttribute exception.");
          D.Out(pdDebug,
                "Throwing e_FederateWasNotAskedToReleaseAttribute exception.");
          throw FederateWasNotAskedToReleaseAttribute(msg->exceptionReason);
      } break ;

      case e_FederationAlreadyPaused: {
          D.Out(pdExcept, "Throwing e_FederationAlreadyPaused exception.");
          throw FederationAlreadyPaused(msg->exceptionReason);
      } break ;

      case e_FederationExecutionAlreadyExists: {
          D.Out(pdExcept, "Throwing e_FederationExecutionAlreadyExists excep.");
          throw FederationExecutionAlreadyExists(msg->exceptionReason);
      } break ;

      case e_FederationExecutionDoesNotExist: {
          D.Out(pdExcept, "Throwing e_FederationExecutionDoesNotExist except.");
          throw FederationExecutionDoesNotExist(msg->exceptionReason);
      } break ;

      case e_FederationNotPaused: {
          D.Out(pdExcept, "Throwing e_FederationNotPaused exception.");
          throw FederationNotPaused(msg->exceptionReason);
      } break ;

      case e_FederationTimeAlreadyPassed: {
          D.Out(pdExcept, "Throwing e_FederationTimeAlreadyPassed exception.");
          throw FederationTimeAlreadyPassed(msg->exceptionReason);
      } break ;

      case e_FederateNotPublishing: {
          D.Out(pdExcept, "Throwing e_FederateNotPublishing exception.");
          throw FederateNotPublishing(msg->exceptionReason);
      } break ;

      case e_FederateNotSubscribing: {
          D.Out(pdExcept, "Throwing e_FederateNotSubscribing exception.");
          throw FederateNotSubscribing(msg->exceptionReason);
      } break ;

      case e_RegionNotKnown: {
          D.Out(pdExcept, "Throwing e_RegionNotKnown exception.");
          throw RegionNotKnown(msg->exceptionReason);
      } break ;

      case e_IDsupplyExhausted: {
          D.Out(pdExcept, "Throwing e_IDsupplyExhausted exception.");
          throw IDsupplyExhausted(msg->exceptionReason);
      } break ;

      case e_InteractionClassNotDefined: {
          D.Out(pdExcept, "Throwing e_InteractionClassNotDefined exception.");
          throw InteractionClassNotDefined(msg->exceptionReason);
      } break ;

      case e_InteractionClassNotKnown: {
          D.Out(pdExcept, "Throwing e_InteractionClassNotKnown exception.");
          throw InteractionClassNotKnown(msg->exceptionReason);
      } break ;

      case e_InteractionClassNotPublished: {
          D.Out(pdExcept, "Throwing e_InteractionClassNotPublished exception.");
          throw InteractionClassNotPublished(msg->exceptionReason);
      } break ;

      case e_InteractionParameterNotDefined: {
          D.Out(pdExcept, "Throwing e_InteractionParameterNotDefined exception.");
          throw InteractionParameterNotDefined(msg->exceptionReason);
      } break ;

      case e_InteractionParameterNotKnown: {
          D.Out(pdExcept, "Throwing e_InteractionParameterNotKnown exception.");
          throw InteractionParameterNotKnown(msg->exceptionReason);
      } break ;

      case e_InvalidDivestitureCondition: {
          D.Out(pdExcept, "Throwing e_InvalidDivestitureCondition exception.");
          throw InvalidDivestitureCondition(msg->exceptionReason);
      } break ;

      case e_InvalidExtents: {
          D.Out(pdExcept, "Throwing e_InvalidExtents exception.");
          throw InvalidExtents(msg->exceptionReason);
      } break ;

      case e_InvalidFederationTime: {
          D.Out(pdExcept, "Throwing e_InvalidFederationTime exception.");
          throw InvalidFederationTime(msg->exceptionReason);
      } break ;

      case e_InvalidFederationTimeDelta: {
          D.Out(pdExcept, "Throwing e_InvalidFederationTimeDelta exception.");
          throw InvalidFederationTimeDelta(msg->exceptionReason);
      } break ;

      case e_InvalidObjectHandle: {
          D.Out(pdExcept, "Throwing e_InvalidObjectHandle exception.");
          throw InvalidObjectHandle(msg->exceptionReason);
      } break ;

      case e_InvalidOrderType: {
          D.Out(pdExcept, "Throwing e_InvalidOrderType exception.");
          throw InvalidOrderType(msg->exceptionReason);
      } break ;

      case e_InvalidResignAction: {
          D.Out(pdExcept, "Throwing e_InvalidResignAction exception.");
          throw InvalidResignAction(msg->exceptionReason);
      } break ;

      case e_InvalidRetractionHandle: {
          D.Out(pdExcept, "Throwing e_InvalidRetractionHandle exception.");
          throw InvalidRetractionHandle(msg->exceptionReason);
      } break ;

      case e_InvalidRoutingSpace: {
          D.Out(pdExcept, "Throwing e_InvalidRoutingSpace exception.");
          throw InvalidRoutingSpace(msg->exceptionReason);
      } break ;

      case e_InvalidTransportType: {
          D.Out(pdExcept, "Throwing e_InvalidTransportType exception.");
          throw InvalidTransportType(msg->exceptionReason);
      } break ;

      case e_MemoryExhausted: {
          D.Out(pdExcept, "Throwing e_MemoryExhausted exception.");
          throw MemoryExhausted(msg->exceptionReason);
      } break ;

      case e_NameNotFound: {
          D.Out(pdExcept, "Throwing e_NameNotFound exception.");
          throw NameNotFound(msg->exceptionReason);
      } break ;

      case e_NoPauseRequested: {
          D.Out(pdExcept, "Throwing e_NoPauseRequested exception.");
          throw NoPauseRequested(msg->exceptionReason);
      } break ;

      case e_NoResumeRequested: {
          D.Out(pdExcept, "Throwing e_NoResumeRequested exception.");
          throw NoResumeRequested(msg->exceptionReason);
      } break ;

      case e_ObjectClassNotDefined: {
          D.Out(pdExcept, "Throwing e_ObjectClassNotDefined exception.");
          throw ObjectClassNotDefined(msg->exceptionReason);
      } break ;

      case e_ObjectClassNotKnown: {
          D.Out(pdExcept, "Throwing e_ObjectClassNotKnown exception.");
          throw ObjectClassNotKnown(msg->exceptionReason);
      } break ;

      case e_ObjectClassNotPublished: {
          D.Out(pdExcept, "Throwing e_ObjectClassNotPublished exception.");
          throw ObjectClassNotPublished(msg->exceptionReason);
      } break ;

      case e_ObjectClassNotSubscribed: {
          D.Out(pdExcept, "Throwing e_ObjectClassNotSubscribed exception.");
          throw ObjectClassNotSubscribed(msg->exceptionReason);
      } break ;

      case e_ObjectNotKnown: {
          D.Out(pdExcept, "Throwing e_ObjectNotKnown exception.");
          throw ObjectNotKnown(msg->exceptionReason);
      } break ;

      case e_ObjectAlreadyRegistered: {
          D.Out(pdExcept, "Throwing e_ObjectAlreadyRegistered exception.");
          throw ObjectAlreadyRegistered(msg->exceptionReason);
      } break ;

      case e_RestoreInProgress: {
          D.Out(pdExcept, "Throwing e_RestoreInProgress exception.");
          throw RestoreInProgress(msg->exceptionReason);
      } break ;

      case e_RestoreNotRequested: {
          D.Out(pdExcept, "Throwing e_RestoreNotRequested exception.");
          throw RestoreNotRequested(msg->exceptionReason);
      } break ;

      case e_RTIinternalError: {
          D.Out(pdExcept, "Throwing e_RTIinternalError exception.");
          throw RTIinternalError(msg->exceptionReason);
      } break ;

      case e_SpaceNotDefined: {
          D.Out(pdExcept, "Throwing e_SpaceNotDefined exception.");
          throw SpaceNotDefined(msg->exceptionReason);
      } break ;

      case e_SaveInProgress: {
          D.Out(pdExcept, "Throwing e_SaveInProgress exception.");
          throw SaveInProgress(msg->exceptionReason);
      } break ;

      case e_SaveNotInitiated: {
          D.Out(pdExcept, "Throwing e_SaveNotInitiated exception.");
          throw SaveNotInitiated(msg->exceptionReason);
      } break ;

      case e_SecurityError: {
          D.Out(pdExcept, "Throwing e_SecurityError exception.");
          throw SecurityError(msg->exceptionReason);
      } break ;

      case e_SpecifiedSaveLabelDoesNotExist: {
          D.Out(pdExcept, "Throwing e_SpecifiedSaveLabelDoesNotExist exception.");
          throw SpecifiedSaveLabelDoesNotExist(msg->exceptionReason);
      } break ;

      case e_TimeAdvanceAlreadyInProgress: {
          D.Out(pdExcept, "Throwing e_TimeAdvanceAlreadyInProgress exception.");
          throw TimeAdvanceAlreadyInProgress(msg->exceptionReason);
      } break ;

      case e_TimeAdvanceWasNotInProgress: {
          D.Out(pdExcept, "Throwing e_TimeAdvanceWasNotInProgress exception.");
          throw TimeAdvanceWasNotInProgress(msg->exceptionReason);
      } break ;

      case e_TooManyIDsRequested: {
          D.Out(pdExcept, "Throwing e_TooManyIDsRequested exception.");
          throw TooManyIDsRequested(msg->exceptionReason);
      } break ;

      case e_UnableToPerformSave: {
          D.Out(pdExcept, "Throwing e_UnableToPerformSave exception.");
          throw UnableToPerformSave(msg->exceptionReason);
      } break ;

      case e_UnimplementedService: {
          D.Out(pdExcept, "Throwing e_UnimplementedService exception.");
          throw UnimplementedService(msg->exceptionReason);
      } break ;

      case e_UnknownLabel: {
          D.Out(pdExcept, "Throwing e_UnknownLabel exception.");
          throw UnknownLabel(msg->exceptionReason);
      } break ;

      case e_ValueCountExceeded: {
          D.Out(pdExcept, "Throwing e_ValueCountExceeded exception.");
          throw ValueCountExceeded(msg->exceptionReason);
      } break ;

      case e_ValueLengthExceeded: {
          D.Out(pdExcept, "Throwing e_ValueLengthExceeded exception.");
          throw ValueLengthExceeded(msg->exceptionReason);
      } break ;

      default: {
          D.Out(pdExcept, "Throwing unknown exception !");
          cout << "LibRTI: Receiving unknown exception." << endl ;
          throw RTIinternalError(msg->exceptionReason);
      } break ;
    }
}

} // namespace certi

// $Id: RTIambassador.cc,v 3.24 2003/05/05 20:21:39 breholee Exp $
