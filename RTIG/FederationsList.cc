// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: FederationsList.cc,v 3.20 2003/10/20 12:54:24 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "FederationsList.hh"

using std::list ;

namespace certi {
namespace rtig {

static pdCDebug D("FEDERATIONSLIST", "(ListFede) - ");

// ----------------------------------------------------------------------------
// Constructor
FederationsList::FederationsList(SocketServer *server, AuditFile *audit)
    : list<Federation *>()
{
    if (server == NULL)
        throw RTIinternalError("FederationsList: No SocketServer was given.");

    if (audit == NULL)
        throw RTIinternalError("FederationsList: No AuditServer was given.");

    socketServer = server ;
    auditFile = audit ;
    verbose = false ;
}

// ----------------------------------------------------------------------------
// Destructor
FederationsList::~FederationsList()
{
    if (!empty())
        D.Out(pdError, "ListeFederation not empty at destruction time.");

    for (list<Federation *>::iterator i = begin(); i != end(); i++) {
        delete(*i);
    }
    clear();
}

// ----------------------------------------------------------------------------
//! Adds a constrained federate to federation.
void
FederationsList::addConstrained(Handle handle,
                                FederateHandle federate)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    // It may throw a bunch of exceptions.
    federation->addConstrained(federate);
}

// ----------------------------------------------------------------------------
/*! Creates a new Federate if its name is unique, and returns its new
  Federate Handle. Also sends Null messages from all other federates
  to initialize its LBTS, and finally a RequestPause message if the
  Federation is already paused.
*/
FederateHandle
FederationsList::addFederate(Handle handle,
                             const char *name,
                             SocketTCP *tcp_link)
    throw (FederationExecutionDoesNotExist,
           FederateAlreadyExecutionMember,
           MemoryExhausted,
           RTIinternalError)
{
    // It may throw RTIinternalError
    checkHandle(handle);

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = NULL ;
    searchFederation(handle, federation);

    // It may raise a bunch of exceptions
    FederateHandle federate = federation->add(name, tcp_link);

    return federate ;
}

// ----------------------------------------------------------------------------
// searchFederation
int
FederationsList::searchFederation(Handle handle,
                                  Federation* &federation)
    throw (FederationExecutionDoesNotExist, RTIinternalError)
{
    federation = NULL ;

    // It may raise RTIinternalError
    checkHandle(handle);

    list<Federation *>::const_iterator i = begin();
    for (int j = 1 ; i != end(); i++, j++) {
        if ((*i)->getHandle() == handle) {
            federation = (*i);
            return j ;
        }
    }

    D.Out(pdExcept, "Unknown Federation Handle %d.", handle);
    throw FederationExecutionDoesNotExist("Bad Federation Handle.");
}

// ----------------------------------------------------------------------------
// createFederation
#ifdef FEDERATION_USES_MULTICAST
void FederationsList::createFederation(const char *name,
                                       Handle handle,
                                       SocketMC *mc_link)
#else
    void FederationsList::createFederation(const char *name,
                                           Handle handle)
#endif
    throw (FederationExecutionAlreadyExists,
           CouldNotOpenRID,
           ErrorReadingRID,
           MemoryExhausted,
           SecurityError,
           RTIinternalError)
{
    Federation *federation = NULL ;
    Handle unused ;

    // It may throw RTIinternalError
    checkHandle(handle);
    auditFile->addToLinef(", Handle : %hu", handle);
    if (name == NULL) throw RTIinternalError("Invalid Federation Name.");

    // It should throw FederationExecutionDoesNotExist.
    try {
        exists(name, unused);
        D.Out(pdExcept, "Federation %s already present.", name);
        throw FederationExecutionAlreadyExists();
    }
    catch (FederationExecutionDoesNotExist &e) {
        D.Out(pdDebug,
              "CreerFederation catches FederationExecutionDoesNotExist.");
    }

    if (size() >= MAX_FEDERATION)
        throw RTIinternalError("Too many federation executions.");


#ifdef FEDERATION_USES_MULTICAST
    federation = new Federation(name, handle, socketServer, auditFile, mc_link);
#else
    federation = new Federation(name, handle, socketServer, auditFile);
#endif

    if (federation == NULL)
        throw MemoryExhausted("No memory left for new Federation.");

    push_front(federation);
    D.Out(pdInit, "New Federation created with Handle %d.", handle);
}

// ----------------------------------------------------------------------------
// createRegulator
void
FederationsList::createRegulator(Handle handle,
                                 FederateHandle federate,
                                 FederationTime time)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    // It may throw a bunch of exceptions.
    federation->addRegulator(federate, time);
}

// ----------------------------------------------------------------------------
// requestId
void
FederationsList::requestId(Handle handle,
                           ObjectHandlecount id_count,
                           ObjectHandle &first_id,
                           ObjectHandle &last_id)
    throw (TooManyIDsRequested)
{
    Federation *federation = NULL ;

    // It may throw RTIinternalError.
    checkHandle(handle);

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    federation->requestId(id_count, first_id, last_id);
}

// ----------------------------------------------------------------------------
// destroyObject
void
FederationsList::destroyObject(Handle handle,
                               FederateHandle federate,
                               ObjectHandle id,
                               const char *tag)
    throw (FederateNotExecutionMember,
           FederationExecutionDoesNotExist,
           DeletePrivilegeNotHeld,
           ObjectNotKnown,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    federation->deleteObject(federate, id, tag);
}

// ----------------------------------------------------------------------------
/*! Return the Handle of the Federation named "name" if it is found in the
  FederationList, else throw FederationExecutionDoesNotExist.
*/
void
FederationsList::exists(const char *name,
                        Handle &handle)
    throw (FederationExecutionDoesNotExist, RTIinternalError)
{
    if (name == NULL) throw RTIinternalError("Null Federation Name.");

    for (list<Federation *>::iterator i = begin(); i != end(); i++) {
        if (!strcmp((*i)->getName(), name)) {
            handle = (*i)->getHandle();
            return ;
        }
    }

    D.Out(pdDebug, "EstPresent throws FederationExecutionDoesNotExist.");
    throw FederationExecutionDoesNotExist();
}

// ----------------------------------------------------------------------------
// info
#ifdef FEDERATION_USES_MULTICAST
void FederationsList::info(Handle handle,
                           int &nb_federates,
                           int &nb_regulators,
                           bool &is_syncing,
                           SocketMC* &comm_mc)
#else
    void FederationsList::info(Handle handle,
                               int &nb_federates,
                               int &nb_regulators,
                               bool &is_syncing)
#endif
    throw (FederationExecutionDoesNotExist, RTIinternalError)
{
    Federation *federation ;

    // It may raise RTIinternalError
    checkHandle(handle);

    // It may throw FederationExecutionNotFound
    searchFederation(handle, federation);

    nb_federates = federation->getNbFederates();
    nb_regulators = federation->getNbRegulators();
    is_syncing = federation->isSynchronizing();

#ifdef FEDERATION_USES_MULTICAST
    comm_mc = federation->MCLink ;
#endif
}

// ----------------------------------------------------------------------------
// registerObject
ObjectHandle
FederationsList::registerObject(Handle handle,
                                FederateHandle federate,
                                ObjectClassHandle object_class,
                                ObjectName name)
    throw (FederateNotExecutionMember,
           FederateNotPublishing,
           ObjectAlreadyRegistered,
           ObjectClassNotDefined,
           ObjectClassNotPublished,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);
    D.Out(pdTrace, "handle = %d, federate = %d.", handle, federate);

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    D.Out(pdTrace, "theObjectClass = %d, name = %s.", object_class, name);
    return(federation->registerObject(federate, object_class, name));
}

// ----------------------------------------------------------------------------
// checkHandle
/*! handle can be any type of handle, not only a Federation handle.
  Throw an exception if Handle is not correct(too low or too big)
*/
void
FederationsList::checkHandle(Handle handle)
    throw (RTIinternalError)
{
    if (handle <= 0) {
        D.Out(pdExcept, "Illegal Handle %d.", handle);
        throw RTIinternalError("Negative or null Handle.");
    }
}

// ----------------------------------------------------------------------------
// updateRegulator
void
FederationsList::updateRegulator(Handle handle,
                                 FederateHandle federate,
                                 FederationTime time)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    federation->updateRegulator(federate, time);
}

// ----------------------------------------------------------------------------
// updateAttribute
void
FederationsList::updateAttribute(Handle handle,
                                 FederateHandle federate,
                                 ObjectHandle id,
                                 AttributeHandle *attributes,
                                 AttributeValue *values,
                                 UShort list_size,
                                 FederationTime time,
                                 const char *tag)
    throw (FederateNotExecutionMember,
           FederationExecutionDoesNotExist,
           ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    federation->updateAttributeValues(federate, id, attributes, values,
                                      list_size, time, tag);
}

// ----------------------------------------------------------------------------
// updateParameter
void
FederationsList::updateParameter(Handle handle,
                                 FederateHandle federate,
                                 InteractionClassHandle interaction,
                                 ParameterHandle *parameters,
                                 ParameterValue *values,
                                 UShort list_size,
                                 FederationTime time,
                                 const char *tag)
    throw (FederateNotExecutionMember,
           FederateNotPublishing,
           FederationExecutionDoesNotExist,
           InteractionClassNotDefined,
           InteractionParameterNotDefined,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    federation->broadcastInteraction(federate, interaction, parameters, values,
                                     list_size, time, tag);
}

// ----------------------------------------------------------------------------
/*! Called by processRegisterSynchronization and
  processSynchronizationAchieved.
*/
void
FederationsList::manageSynchronization(Handle handle,
                                       FederateHandle federate,
                                       bool state,
                                       const char *label,
                                       const char *tag)
    throw (FederationAlreadyPaused,
           FederationNotPaused,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    checkHandle(handle); // It may throw RTIinternalError

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = NULL ;
    searchFederation(handle, federation);

    // It may throw a bunch of exceptions.
    if (state)
        federation->registerSynchronization(federate, label, tag);
    else
        federation->unregisterSynchronization(federate, label);
}

// ----------------------------------------------------------------------------
//! Called by processRegisterSynchronization.
void
FederationsList::broadcastSynchronization(Handle handle,
                                          FederateHandle federate,
                                          const char *label,
                                          const char *tag)
    throw (FederationExecutionDoesNotExist,
           RTIinternalError)
{
    checkHandle(handle); // It may throw RTIinternalError

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = NULL ;
    searchFederation(handle, federation);

    federation->broadcastSynchronization(federate, label, tag);
}

// ----------------------------------------------------------------------------
// publishInteraction
void
FederationsList::publishInteraction(Handle handle,
                                    FederateHandle federate,
                                    InteractionClassHandle interaction,
                                    bool pub)
    throw (InteractionClassNotDefined,
           FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           SecurityError,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    federation->publishInteraction(federate, interaction, pub);
}

// ----------------------------------------------------------------------------
// publishObject
void
FederationsList::publishObject(Handle handle,
                               FederateHandle federate,
                               ObjectClassHandle object_class,
                               AttributeHandle *attributes,
                               UShort list_size,
                               bool pub)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           SecurityError,
           RTIinternalError)

{
    Federation *federation = NULL ;

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    federation->publishObject(federate, object_class, attributes, list_size,
                              pub);
}

// ----------------------------------------------------------------------------
// subscribeInteraction
void
FederationsList::subscribeInteraction(Handle handle,
                                      FederateHandle federate,
                                      InteractionClassHandle interaction,
                                      bool sub)
    throw (InteractionClassNotDefined,
           FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           SecurityError,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    federation->subscribeInteraction(federate, interaction, sub);
}

// ----------------------------------------------------------------------------
// subscribeObject
void
FederationsList::subscribeObject(Handle handle,
                                 FederateHandle federate,
                                 ObjectClassHandle object_class,
                                 AttributeHandle *attributes,
                                 UShort list_size,
                                 bool sub)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           SecurityError,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    federation->subscribeObject(federate, object_class, attributes, list_size,
                                sub);
}

// ----------------------------------------------------------------------------
// removeConstrained
void
FederationsList::removeConstrained(Handle handle,
                                   FederateHandle federate)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    // It may throw a bunch of exceptions.
    federation->removeConstrained(federate);
}

// ----------------------------------------------------------------------------
// destroyFederation
void
FederationsList::destroyFederation(Handle handle)
    throw (FederatesCurrentlyJoined,
           FederationExecutionDoesNotExist,
           RTIinternalError)
{
    Federation *federation ;

    // It may throw RTIinternalError
    checkHandle(handle);

    // It may throw FederationExecutionDoesNotExist
    int rank = searchFederation(handle, federation);

    // It may throw FederatesCurrentlyJoined
    if (federation->empty()) {
        list<Federation *>::iterator i = begin();
        for (int j = 1 ; i != end() && j <= rank ; j++) {
            if (j == rank) {
                erase(i);
            }
        }
        delete federation ;
    }
}

// ----------------------------------------------------------------------------
// remove
void
FederationsList::remove(Handle handle, FederateHandle federate)
    throw (FederationExecutionDoesNotExist,
           FederateOwnsAttributes,
           FederateNotExecutionMember,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw RTIinternalError.
    checkHandle(handle);

    // It may throw FederationExecutionDoesNotExist
    searchFederation(handle, federation);

    // It may throw FederateOwnsAttributes or FederateNotExecutionMember
    federation->remove(federate);
}

// ----------------------------------------------------------------------------
// removeRegulator
void
FederationsList::removeRegulator(Handle handle,
                                 FederateHandle federate)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    federation->removeRegulator(federate);
}

// ----------------------------------------------------------------------------
/*! This Method tries to remove all references to this Federate in the
  Federation. To be used when a Federate is supposed to have crashed.
*/
void
FederationsList::killFederate(Handle handle, FederateHandle federate)
    throw ()
{
    Federation *federation = NULL ;

    try {
        // It may throw RTIinternalError.
        checkHandle(handle);
        checkHandle(federate);

        // It may throw FederationExecutionDoesNotExist.
        searchFederation(handle, federation);
        federation->kill(federate);
    }
    catch (Exception &e) {
        return ;
    }
}

// ----------------------------------------------------------------------------
// isOwner
bool
FederationsList::isOwner(Handle handle,
                         FederateHandle federate,
                         ObjectHandle id,
                         AttributeHandle attribute)
    throw (FederateNotExecutionMember,
           ObjectNotKnown,
           AttributeNotDefined,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *f = NULL ;

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, f);

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    D.Out(pdDebug, "Owner of Attribute %u of Object %u .", attribute, id);

    return(f->isOwner(federate, id, attribute));
}

// ----------------------------------------------------------------------------
// searchOwner
void
FederationsList::searchOwner(Handle handle,
                             FederateHandle federate,
                             ObjectHandle id,
                             AttributeHandle attribute)
    throw (FederateNotExecutionMember,
           ObjectNotKnown,
           AttributeNotDefined,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);


    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    D.Out(pdDebug, "Owner of Attribute %u of Object %u .", attribute, id);

    federation->queryAttributeOwnership(federate, id, attribute);
}

// ----------------------------------------------------------------------------
// negotiateDivestiture
void
FederationsList::negotiateDivestiture(Handle handle,
                                      FederateHandle federate,
                                      ObjectHandle id,
                                      AttributeHandle *attributes,
                                      UShort list_size,
                                      const char *tag)
    throw (FederateNotExecutionMember,
           ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           AttributeAlreadyBeingDivested,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    D.Out(pdDebug, "NegotiatedAttributeOwnershipDivestiture of Object %u.", id);

    federation->negotiateDivestiture(federate, id, attributes, list_size, tag);
}

// ----------------------------------------------------------------------------
// acquireIfAvailable
void
FederationsList::acquireIfAvailable(Handle handle,
                                    FederateHandle federate,
                                    ObjectHandle id,
                                    AttributeHandle *attributes,
                                    UShort list_size)
    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           AttributeAlreadyBeingAcquired,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    federation->acquireIfAvailable(federate, id, attributes, list_size);
}

// ----------------------------------------------------------------------------
// divest
void
FederationsList::divest(Handle handle,
                        FederateHandle federate,
                        ObjectHandle id,
                        AttributeHandle *attributes,
                        UShort list_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    federation->divest(federate, id, attributes, list_size);
}

// ----------------------------------------------------------------------------
// acquire
void
FederationsList::acquire(Handle handle,
                         FederateHandle federate,
                         ObjectHandle id,
                         AttributeHandle *attributes,
                         UShort list_size,
                         const char *tag)
    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    D.Out(pdDebug, "attributeOwnershipAcquisition of Object %u .", id);

    federation->acquire(federate, id, attributes, list_size, tag);
}

// ----------------------------------------------------------------------------
// cancelNegotiatedAttributeOwnershipDivestiture
void
FederationsList::cancelDivestiture(Handle handle,
                                   FederateHandle federate,
                                   ObjectHandle id,
                                   AttributeHandle *attributes,
                                   UShort list_size)
    throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
           AttributeDivestitureWasNotRequested, FederateNotExecutionMember,
           SaveInProgress, RestoreInProgress, RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    D.Out(pdDebug,
          "cancelNegotiatedAttributeOwnershipDivestiture of Object %u .", id);

    federation->cancelDivestiture(federate, id, attributes, list_size);
}

// ----------------------------------------------------------------------------
// respondRelease
AttributeHandleSet*
FederationsList::respondRelease(Handle handle,
                                FederateHandle federate,
                                ObjectHandle id,
                                AttributeHandle *attributes,
                                UShort list_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           FederateWasNotAskedToReleaseAttribute,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    D.Out(pdDebug, "AttributeOwnershipRealeaseResponse of Object %u .", id);

    return federation->respondRelease(federate, id, attributes, list_size);
}

// ----------------------------------------------------------------------------
// cancelAcquisition
void
FederationsList::cancelAcquisition(Handle handle,
                                   FederateHandle federate,
                                   ObjectHandle id,
                                   AttributeHandle *attributes,
                                   UShort list_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeAlreadyOwned,
           AttributeAcquisitionWasNotRequested,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Federation *federation = NULL ;

    // It may throw FederationExecutionDoesNotExist.
    searchFederation(handle, federation);

    // It may throw RTIinternalError.
    checkHandle(handle);
    checkHandle(federate);

    D.Out(pdDebug, "cancelAttributeOwnershipAcquisition of Object %u .", id);

    federation->cancelAcquisition(federate, id, attributes, list_size);
}

// ----------------------------------------------------------------------------
long
FederationsList::createRegion(Handle federation,
                              FederateHandle federate,
                              SpaceHandle space,
                              long nb_extents)
    throw (SpaceNotDefined, InvalidExtents, FederateNotExecutionMember,
           SaveInProgress, RestoreInProgress, RTIinternalError)
{
    Federation *f = 0 ;

    searchFederation(federation, f);
    checkHandle(federation);
    checkHandle(federate);

    D[pdDebug] << "Create " << nb_extents << "-extent Region "
               << "in space " << space << endl ;

    return f->createRegion(federate, space, nb_extents);
}

// ----------------------------------------------------------------------------
void
FederationsList::modifyRegion(Handle federation,
			      FederateHandle federate,
			      long region,
			      std::vector<Extent *> *extents)
    throw (InvalidExtents, SaveInProgress, RestoreInProgress, RTIinternalError)
{
    Federation *f = 0 ;

    searchFederation(federation, f);
    checkHandle(federation);
    checkHandle(federate);

    D[pdDebug] << "Modify region #" << region << endl ;
    f->modifyRegion(federate, region, extents);
}

// ----------------------------------------------------------------------------
void
FederationsList::deleteRegion(Handle federation,
                              FederateHandle federate,
                              long region)
    throw (RegionNotKnown, RegionInUse, FederateNotExecutionMember,
           SaveInProgress, RestoreInProgress, RTIinternalError)
{
    Federation *f = 0 ;

    searchFederation(federation, f);
    checkHandle(federation);
    checkHandle(federate);

    D[pdDebug] << "Delete region " << region << endl ;

    f->deleteRegion(federate, region);
}

// ----------------------------------------------------------------------------
void
FederationsList::associateRegion(Handle federation,
				 FederateHandle federate,
				 ObjectHandle object,
				 RegionHandle region,
				 unsigned short nb_attributes,
				 AttributeHandle *attributes)
	throw (RegionInUse, FederateNotExecutionMember, SaveInProgress,
	       RestoreInProgress, RTIinternalError)
{
    Federation *f = 0 ;

    searchFederation(federation, f);
    checkHandle(federation);
    checkHandle(federate);

    D[pdDebug] << "Associate region for updates " << region << endl ;

    f->associateRegion(federate, object, region, nb_attributes, attributes);
}

// ----------------------------------------------------------------------------
void
FederationsList::unassociateRegion(Handle federation, 
				   FederateHandle federate,
				   ObjectHandle object,
				   RegionHandle region)
	throw (RegionInUse, FederateNotExecutionMember, SaveInProgress,
	       RestoreInProgress, RTIinternalError)
{
    Federation *f = 0 ;

    searchFederation(federation, f);
    checkHandle(federation);
    checkHandle(federate);

    D[pdDebug] << "Unassociate region for updates " << region << endl ;

    f->unassociateRegion(federate, object, region);
}

// ----------------------------------------------------------------------------
void
FederationsList::subscribeAttributesWR(Handle federation,
				       FederateHandle federate,
				       ObjectClassHandle object_class,
				       RegionHandle region,
				       unsigned short nb,
				       AttributeHandle *attributes)
    throw (FederateNotExecutionMember, SaveInProgress, RestoreInProgress,
	   RTIinternalError)
{
    Federation *f = 0 ;

    searchFederation(federation, f);
    checkHandle(federation);
    checkHandle(federate);

    D[pdDebug] << " Subscribe attributes with region " << region << endl ;

    f->subscribeAttributesWR(federate, object_class, region, nb, attributes);
}

// ----------------------------------------------------------------------------
void
FederationsList::unsubscribeAttributesWR(Handle federation,
					 FederateHandle federate,
					 ObjectClassHandle object_class,
					 RegionHandle region)
    throw (FederateNotExecutionMember, SaveInProgress, RestoreInProgress,
	   RTIinternalError)
{
    Federation *f = 0 ;

    searchFederation(federation, f);
    checkHandle(federation);
    checkHandle(federate);

    D[pdDebug] << "Unsubscribe attributes with region " << region << endl ;

    f->unsubscribeAttributesWR(federate, object_class, region);
}

// ----------------------------------------------------------------------------
void
FederationsList::subscribeInteractionWR(Handle federation,
					FederateHandle federate,
					InteractionClassHandle ic,
					RegionHandle region)
    throw (FederateNotExecutionMember, SaveInProgress, RestoreInProgress,
	   RTIinternalError)
{
    Federation *f = 0 ;

    searchFederation(federation, f);
    checkHandle(federation);
    checkHandle(federate);

    D[pdDebug] << "Subscribe interaction with region " << region << endl ;

    f->subscribeInteractionWR(federate, ic, region);
}
    
// ----------------------------------------------------------------------------
void
FederationsList::unsubscribeInteractionWR(Handle federation,
					  FederateHandle federate,
					  InteractionClassHandle ic,
					  RegionHandle region)
    throw (FederateNotExecutionMember, SaveInProgress, RestoreInProgress,
	   RTIinternalError)
{
    Federation *f = 0 ;

    searchFederation(federation, f);
    checkHandle(federation);
    checkHandle(federate);

    D[pdDebug] << "Unsubscribe interaction with region " << region << endl ;

    f->unsubscribeInteractionWR(federate, ic, region);
}

// ----------------------------------------------------------------------------
void
FederationsList::requestFederationSave(Handle the_federation,
                                       FederateHandle the_federate,
                                       const char *the_label,
                                       FederationTime the_time)
{
    checkHandle(the_federation);

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = 0 ;
    searchFederation(the_federation, federation);

    federation->requestFederationSave(the_federate, the_label, the_time);
}

// ----------------------------------------------------------------------------
void
FederationsList::federateSaveBegun(Handle the_federation,
                                   FederateHandle the_federate)
{
    checkHandle(the_federation);

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = 0 ;
    searchFederation(the_federation, federation);

    federation->federateSaveBegun(the_federate);
}

// ----------------------------------------------------------------------------
void
FederationsList::federateSaveStatus(Handle the_federation,
                                    FederateHandle the_federate,
                                    bool the_status)
{
    checkHandle(the_federation);

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = 0 ;
    searchFederation(the_federation, federation);

    federation->federateSaveStatus(the_federate, the_status);
}

// ----------------------------------------------------------------------------
void
FederationsList::requestFederationRestore(Handle the_federation,
                                          FederateHandle the_federate,
                                          const char *the_label)
{
    checkHandle(the_federation);

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = NULL ;
    searchFederation(the_federation, federation);

    federation->requestFederationRestore(the_federate, the_label);
}

// ----------------------------------------------------------------------------
void
FederationsList::federateRestoreStatus(Handle the_federation,
                                       FederateHandle the_federate,
                                       bool the_status)
{
    checkHandle(the_federation);

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = NULL ;
    searchFederation(the_federation, federation);

    federation->federateRestoreStatus(the_federate, the_status);
}

}} // certi::rtig

// EOF $Id: FederationsList.cc,v 3.20 2003/10/20 12:54:24 breholee Exp $

