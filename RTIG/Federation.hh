// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
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
// $Id: Federation.hh,v 3.10 2003/03/21 15:06:46 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIG_FEDERATION_HH
#define _CERTI_RTIG_FEDERATION_HH

#include <config.h>
#include "Federate.hh"
#include "RootObject.hh"
#include "LBTS.hh"
#include "SecurityServer.hh"
#include "RTItypes.hh"
#include "FedParser.hh" // CRead : FED file reader.
#include "XmlParser.hh"
#ifdef FEDERATION_USES_MULTICAST
#include "SocketMC.hh"
#endif
#include "PrettyDebug.hh"

#include <map>
#include <fstream>
#include <iostream>
#include <string.h> // strlen

using std::map ;
using std::pair ;
using std::ifstream ;
using std::ios ;
using std::cout ;
using std::endl ;

using namespace certi ;

namespace certi {
namespace rtig {

class Federation : private list<Federate *>
{
    // ATTRIBUTES --------------------------------------------------------------
private:
    FederationHandle handle ;
    char *name ;
    FederateHandle nextFederateHandle ;

    //! Labels and Tags not on synchronization.
    std::map<const char *, const char *> synchronizationLabels ;

    ObjectHandle nextObjectId ;

    // This object is initialized when the Federation is created, with
    // the reference of the RTIG managed Socket Server. The reference of
    // this object is passed down the Classes Tree with the help of RootObj.
    SecurityServer *server ;
    RootObject *root ;

    LBTS regulators ;

#ifdef FEDERATION_USES_MULTICAST
    SocketMC *MCLink ;
#endif

    // METHODS -----------------------------------------------------------------
public:
#ifdef FEDERATION_USES_MULTICAST
    Federation(const char *,
               FederationHandle,
               SocketServer*,
               AuditFile*,
               SocketMC*)
#else
        Federation(const char *, FederationHandle, SocketServer*, AuditFile*)
#endif
        throw (CouldNotOpenRID, ErrorReadingRID, MemoryExhausted, SecurityError,
               RTIinternalError);

    ~Federation(void);

    void requestId(ObjectHandlecount IDCount,
                   ObjectHandle &FirstID,
                   ObjectHandle &LastID)
        throw (TooManyIDsRequested);

    int getNbFederates(void) const ;
    int getNbRegulators(void) const ;
    bool isSynchronizing(void) const ;
    FederationHandle getHandle(void) const ;
    const char *getName(void) const ;

    // -------------------------
    // -- Federate Management --
    // -------------------------
    FederateHandle add(const char *theName, SocketTCP *theTCPLink)
        throw (FederateAlreadyExecutionMember,
               MemoryExhausted,
               RTIinternalError);

    bool empty(void) const
        throw (FederatesCurrentlyJoined);

    bool check(FederateHandle theHandle) const
        throw (FederateNotExecutionMember);

    void kill(FederateHandle theFederate) throw ();

    void remove(FederateHandle theHandle)
        throw (FederateOwnsAttributes,
               FederateNotExecutionMember);

    // ---------------------
    // -- Time Management --
    // ---------------------
    void addRegulator(FederateHandle theHandle, FederationTime theTime)
        throw (FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError); // includes Time Regulation already enabled.

    void updateRegulator(FederateHandle theHandle, FederationTime theTime)
        throw (FederateNotExecutionMember,
               RTIinternalError);

    void removeRegulator(FederateHandle theHandle)
        throw (FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError); // includes Time Regulation already disabled.

    void addConstrained(FederateHandle theHandle)
        throw (FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError); // includes Time constrained already enabled.

    void removeConstrained(FederateHandle theHandle)
        throw (FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError); // includes Time constrained already disabled.

    // Synchronization Management.
    void registerSynchronization(FederateHandle the_federate,
                                 const char *the_label,
                                 const char *the_tag)
        throw (FederateNotExecutionMember,
               FederationAlreadyPaused,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void unregisterSynchronization(FederateHandle theFederate,
                                   const char *theLabel)
        throw (FederateNotExecutionMember,
               FederationNotPaused,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void broadcastSynchronization(FederateHandle federate,
                                  const char *label,
                                  const char *tag)
        throw (RTIinternalError);

    // -----------------------
    // -- Object Management --
    // -----------------------

    void deleteObject(FederateHandle theFederateHandle,
                      ObjectHandle theObjectHandle,
                      const char *theUserTag)
        throw (FederateNotExecutionMember,
               DeletePrivilegeNotHeld,
               ObjectNotKnown,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void publishObject(FederateHandle theFederateHandle,
                       ObjectClassHandle theObjectHandle,
                       AttributeHandle *theAttributeList,
                       UShort theListSize,
                       bool PubOrUnpub)
        throw (ObjectClassNotDefined,
               AttributeNotDefined,
               FederateNotExecutionMember,
               SaveInProgress,
               SecurityError,
               RestoreInProgress,
               RTIinternalError);

    ObjectHandle registerObject(FederateHandle theFederateHandle,
                                ObjectClassHandle theClass,
                                ObjectName theName)
        throw (FederateNotExecutionMember,
               FederateNotPublishing,
               ObjectAlreadyRegistered,
               ObjectClassNotDefined,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void subscribeObject(FederateHandle theFederateHandle,
                         ObjectClassHandle theObjectHandle,
                         AttributeHandle *theAttributeList,
                         UShort theListSize,
                         bool SubOrUnSub)
        throw (ObjectClassNotDefined,
               AttributeNotDefined,
               FederateNotExecutionMember,
               SaveInProgress,
               SecurityError,
               RestoreInProgress,
               RTIinternalError);

    void updateAttributeValues(FederateHandle theFederateHandle,
                               ObjectHandle theObjectHandle,
                               AttributeHandle *theAttributeList,
                               AttributeValue *theValueList,
                               UShort theListSize,
                               FederationTime theTime,
                               const char *theTag)
        throw (FederateNotExecutionMember,
               ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    // ----------------------------
    // -- Interaction Management --
    // ----------------------------

    void broadcastInteraction(FederateHandle theFederateHandle,
                              InteractionClassHandle theInteractionHandle,
                              ParameterHandle *theParameterList,
                              ParameterValue *theValueList,
                              UShort theListSize,
                              FederationTime theTime,
                              const char *theTag)
        throw (FederateNotExecutionMember,
               FederateNotPublishing,
               InteractionClassNotDefined,
               InteractionParameterNotDefined,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void publishInteraction(FederateHandle theFederateHandle,
                            InteractionClassHandle theInteractionHandle,
                            bool PubOrUnpub)
        throw (InteractionClassNotDefined,
               FederateNotExecutionMember,
               SaveInProgress,
               SecurityError,
               RestoreInProgress,
               RTIinternalError);

    void subscribeInteraction(FederateHandle theFederateHandle,
                              InteractionClassHandle theInteractionHandle,
                              bool SubOrUnsub)
        throw (InteractionClassNotDefined,
               FederateNotExecutionMember,
               SaveInProgress,
               SecurityError,
               RestoreInProgress,
               RTIinternalError);


    // --------------------------
    // -- Ownership Management --
    // --------------------------

    bool isOwner(FederateHandle theFederateHandle,
                 ObjectHandle theObjectHandle,
                 AttributeHandle theAttribute)
        throw (FederateNotExecutionMember,
               ObjectNotKnown,
               AttributeNotDefined,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void queryAttributeOwnership(FederateHandle theFederateHandle,
                                 ObjectHandle theObjectHandle,
                                 AttributeHandle theAttribute)
        throw (FederateNotExecutionMember,
               ObjectNotKnown,
               AttributeNotDefined,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void negotiateDivestiture(FederateHandle theFederateHandle,
                              ObjectHandle theObjectHandle,
                              AttributeHandle *theAttributeList,
                              UShort theListSize,
                              const char *theTag)
        throw (FederateNotExecutionMember,
               ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               AttributeAlreadyBeingDivested,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void acquireIfAvailable(FederateHandle theFederateHandle,
                            ObjectHandle theObjectHandle,
                            AttributeHandle *theAttributeList,
                            UShort theListSize)
        throw (ObjectNotKnown,
               ObjectClassNotPublished,
               AttributeNotDefined,
               AttributeNotPublished,
               FederateOwnsAttributes,
               AttributeAlreadyBeingAcquired,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void divest(FederateHandle theFederateHandle,
                ObjectHandle theObjectHandle,
                AttributeHandle *theAttributeList,
                UShort theListSize)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void acquire(FederateHandle theFederateHandle,
                 ObjectHandle theObjectHandle,
                 AttributeHandle *theAttributeList,
                 UShort theListSize,
                 const char *theTag)
        throw (ObjectNotKnown,
               ObjectClassNotPublished,
               AttributeNotDefined,
               AttributeNotPublished,
               FederateOwnsAttributes,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void cancelDivestiture(FederateHandle theFederateHandle,
                           ObjectHandle theObjectHandle,
                           AttributeHandle *theAttributeList,
                           UShort theListSize)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               AttributeDivestitureWasNotRequested,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    AttributeHandleSet* respondRelease(FederateHandle theFederateHandle,
                                       ObjectHandle theObjectHandle,
                                       AttributeHandle *theAttributeList,
                                       UShort theListSize)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               FederateWasNotAskedToReleaseAttribute,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void cancelAcquisition(FederateHandle theFederateHandle,
                           ObjectHandle theObjectHandle,
                           AttributeHandle *theAttributeList,
                           UShort theListSize)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeAlreadyOwned,
               AttributeAcquisitionWasNotRequested,
               FederateNotExecutionMember,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

private:
    // ---------------------
    // -- Private Methods --
    // ---------------------
    void broadcastAnyMessage(NetworkMessage *msg, FederateHandle Except);

    Federate *getByName(const char *theName) const
        throw (FederateNotExecutionMember);

    Federate *getByHandle(FederateHandle theHandle) const
        throw (FederateNotExecutionMember);

    FederateHandle getNewHandle(void)
        throw (RTIinternalError);
};

}} // namespace certi/rtig

#endif // _CERTI_RTIG_FEDERATION_HH

// $Id: Federation.hh,v 3.10 2003/03/21 15:06:46 breholee Exp $
