// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: Federation.hh,v 3.43 2007/11/16 15:04:22 rousse Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIG_FEDERATION_HH
#define _CERTI_RTIG_FEDERATION_HH

#include "Federate.hh"
#include "RootObject.hh"
#include "LBTS.hh"
#include "SecurityServer.hh"
#include "HandleManager.hh"
#include "certi.hh"

#ifndef WIN32
#include <unistd.h>
#include <stdlib.h>
#endif

#ifdef FEDERATION_USES_MULTICAST
#include "SocketMC.hh"
#endif

// Libraries
#ifdef HAVE_XML
#include <libxml/xmlmemory.h> // FIXME: should be in the .cc
#include <libxml/parser.h>
#include <libxml/tree.h>
#endif // HAVE_XML

namespace certi {
namespace rtig {

class Federation
{
    // ATTRIBUTES --------------------------------------------------------------
private:
    Handle handle ;
    char *name ;
    char *FEDid ;

    //! Labels and Tags not on synchronization.
    std::map<const char *, const char *> synchronizationLabels ;

    HandleManager<FederateHandle> federateHandles ;
    HandleManager<ObjectHandle> objectHandles ;

    // This object is initialized when the Federation is created, with
    // the reference of the RTIG managed Socket Server. The reference of
    // this object is passed down the Classes Tree with the help of RootObj.
    SecurityServer *server ;
    RootObject *root ;

    LBTS regulators ;

#ifdef FEDERATION_USES_MULTICAST
    SocketMC *MCLink ;
#endif

    bool saveXmlData();
    bool restoreXmlData();

    // METHODS -----------------------------------------------------------------
public:
#ifdef FEDERATION_USES_MULTICAST
    Federation(const char *,
               FederationHandle,
               SocketServer &,
               AuditFile &,
               SocketMC*)
#else
        Federation(const char *, Handle, SocketServer &, AuditFile &, const char *)
#endif
        throw (CouldNotOpenFED, ErrorReadingFED, MemoryExhausted, SecurityError,
               RTIinternalError);

    ~Federation();

    int getNbFederates() const ;
    int getNbRegulators() const ;
    bool isSynchronizing() const ;
    Handle getHandle() const ;
    const char *getName() const ;
    const char *getFEDid() const ;

    // -------------------------
    // -- Federate Management --
    // -------------------------
    FederateHandle add(const char *theName, SocketTCP *theTCPLink)
        throw (FederateAlreadyExecutionMember,
               MemoryExhausted,
               RTIinternalError);

    bool empty() const
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

    void registerSynchronization(FederateHandle the_federate,
                                 const char *the_label,
                                 const char *the_tag,
                                 unsigned short federate_setSize,
                                 FederateHandle *federate_set)
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

    void broadcastSynchronization(FederateHandle federate,
                                  const char *label,
                                  const char *tag,
                                  unsigned short federate_setSize,
                                  FederateHandle *federate_set)
        throw (RTIinternalError);


    // Save Management.
    void requestFederationSave(FederateHandle, const char *, FederationTime)
        throw (FederateNotExecutionMember, SaveInProgress);
    void requestFederationSave(FederateHandle, const char *)
        throw (FederateNotExecutionMember, SaveInProgress);


    void federateSaveBegun(FederateHandle)
        throw (FederateNotExecutionMember);

    void federateSaveStatus(FederateHandle, bool)
        throw (FederateNotExecutionMember);

    void requestFederationRestore(FederateHandle the_federate,
                                  const char *the_label)
        throw (FederateNotExecutionMember);

    void federateRestoreStatus(FederateHandle the_federate,
                               bool the_status)
        throw (FederateNotExecutionMember);

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
                                const char *theName)
        throw (FederateNotExecutionMember,
               FederateNotPublishing,
               ObjectAlreadyRegistered,
               ObjectClassNotDefined,
               ObjectClassNotPublished,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

    void subscribeObject(FederateHandle theFederateHandle,
                         ObjectClassHandle theObjectHandle,
                         AttributeHandle *theAttributeList,
                         UShort theListSize)
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
                               ValueLengthPair *theValueList,
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

    void updateAttributeValues(FederateHandle theFederateHandle,
                               ObjectHandle theObjectHandle,
                               AttributeHandle *theAttributeList,
                               ValueLengthPair *theValueList,
                               UShort theListSize,
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
                              ParameterLengthPair *theValueList,
                              UShort theListSize,
                              FederationTime theTime,
			      RegionHandle,
                              const char *theTag)
        throw (FederateNotExecutionMember,
               FederateNotPublishing,
               InteractionClassNotDefined,
               InteractionParameterNotDefined,
               SaveInProgress,
               RestoreInProgress,
               RTIinternalError);

   void broadcastInteraction(FederateHandle theFederateHandle,
                              InteractionClassHandle theInteractionHandle,
                              ParameterHandle *theParameterList,
                              ParameterLengthPair *theValueList,
                              UShort theListSize,
			      RegionHandle,
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

    // Data Distribution Management

    long createRegion(FederateHandle, SpaceHandle, long)
        throw (SpaceNotDefined,
	       InvalidExtents,
	       SaveInProgress,
               RestoreInProgress,
	       RTIinternalError);

    void modifyRegion(FederateHandle, RegionHandle, const std::vector<Extent> &)
	throw (RegionNotKnown,
	       InvalidExtents,
	       SaveInProgress,
	       RestoreInProgress,
	       RTIinternalError);
    
    void deleteRegion(FederateHandle, long)
        throw (RegionNotKnown,
	       RegionInUse,
	       SaveInProgress,
	       RestoreInProgress,
               RTIinternalError);

    void associateRegion(FederateHandle, ObjectHandle, RegionHandle,
			 unsigned short, AttributeHandle *)
	throw (RegionNotKnown,
	       SaveInProgress,
	       RestoreInProgress,
	       RTIinternalError);
	
    void unassociateRegion(FederateHandle, ObjectHandle, RegionHandle)
	throw (RegionNotKnown,
	       SaveInProgress,
	       RestoreInProgress,
	       RTIinternalError);

    void subscribeAttributesWR(FederateHandle, ObjectClassHandle, RegionHandle,
				unsigned short, AttributeHandle *)
	throw (RegionNotKnown,
	       SaveInProgress,
	       RestoreInProgress,
	       RTIinternalError);

    void unsubscribeAttributesWR(FederateHandle, ObjectClassHandle,
				 RegionHandle)
	throw (RegionNotKnown,
	       SaveInProgress,
	       RestoreInProgress,
	       RTIinternalError);

    void subscribeInteractionWR(FederateHandle, InteractionClassHandle,
				RegionHandle)
	throw (RegionNotKnown,
	       SaveInProgress,
	       RestoreInProgress,
	       RTIinternalError);

    void unsubscribeInteractionWR(FederateHandle, InteractionClassHandle,
				  RegionHandle)
	throw (RegionNotKnown,
	       SaveInProgress,
	       RestoreInProgress,
	       RTIinternalError);

    ObjectHandle registerObjectWithRegion(FederateHandle,ObjectClassHandle,
					  const char *, RegionHandle, int,
					  AttributeHandle *)
	throw (ObjectClassNotDefined, ObjectClassNotPublished,
	       AttributeNotDefined, AttributeNotPublished, RegionNotKnown,
	       InvalidRegionContext, ObjectAlreadyRegistered,
	       SaveInProgress, RestoreInProgress,
	       RTIinternalError);               

private:
    // Private methods
    void broadcastAnyMessage(NetworkMessage *msg, FederateHandle Except);

    void broadcastSomeMessage(NetworkMessage *msg, FederateHandle Except,FederateHandle *fede_array, int nbfed);

    Federate &getFederate(const char *theName)
        throw (FederateNotExecutionMember);

    Federate &getFederate(FederateHandle theHandle)
        throw (FederateNotExecutionMember);

    // Private attributes
    typedef std::vector<Federate> FederateList ;

    FederateList federates ;
    bool saveInProgress ;
    bool restoreInProgress ;
    bool saveStatus ; //!< True if saving was correctly done, false otherwise.
    bool restoreStatus ; //!< True if restoring was correctly done.
    bool verbose ;
    std::string saveLabel ; //!< The label associated with the save request.

#ifdef HAVE_XML
    xmlDocPtr doc ; // FIXME: should not be an attribute
#endif // HAVE_XML
};

}} // namespace certi/rtig

#endif // _CERTI_RTIG_FEDERATION_HH

// $Id: Federation.hh,v 3.43 2007/11/16 15:04:22 rousse Exp $
