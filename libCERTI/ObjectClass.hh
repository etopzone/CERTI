// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
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
// $Id: ObjectClass.hh,v 3.10 2003/04/09 16:41:10 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_CLASS_HH
#define _CERTI_OBJECT_CLASS_HH

#include <config.h>

#include <list>
using std::list ;

#include <iostream>
using std::cout ;
using std::endl ;

#include "RTItypes.hh"
#include "ObjectClassAttribute.hh"
#include "Object.hh"
#include "ObjectAttribute.hh"
#include "SocketTCP.hh"
#include "SecurityServer.hh"
#include "ObjectClassBroadcastList.hh"
#include "PrettyDebug.hh"

namespace certi {

class CDiffusion {

public:

    typedef struct {
        FederateHandle federate ;
        AttributeHandle attribute ;

    } DiffStruct ;

    DiffStruct DiffArray[MAX_ATTRIBUTES_PER_CLASS] ;

    int size ;

    CDiffusion(void) { size = 0 ; };
};

/*! This class is used to describe an object class. It also contains the
  instance list from this class.
*/
class ObjectClass {

public:

    // -----------------------
    // -- Public Attributes --(cf. Note)
    // -----------------------

    // Note: Most of the folling Attributes should be private. In fact,
    // they can not be reached from the RTIG and RTIA components, because
    // they are hidden from them by ObjectClassSet.

    // They are kept public for convenience in the CRead class that build
    // the Object Classes tree and set all Father/Son dependences and
    // attributes.
    ObjectClassHandle Father ; //!< Object father number.
    list<ObjectClassHandle> sonSet ; //<! Son classes set from this object class.

    //! This Object help to find a TCPLink from a Federate Handle.
    SecurityServer *server ;

    //! Depth in the class tree structure. Used only by CRead.
    UShort Depth ;

    // ----------------------------
    // -- Initialization Methods --
    // ----------------------------

    // Constructor & Destructor
    ObjectClass(void);
    ~ObjectClass(void);

    void display(void) const ;

    /*! Name attribute access(GetName reference must be considered READ-ONLY).
      NewName length must be lower or equal to MAX_USER_TAG_LENGTH.
    */
    const char *getName(void) const {return Name ; };

    void setName(const char *new_name)
        throw (ValueLengthExceeded, RTIinternalError);

    void setHandle(ObjectClassHandle new_handle);
    ObjectClassHandle getHandle(void) const ;

    // ----------------------
    // -- Security Methods --
    // ----------------------

    void checkFederateAccess(FederateHandle the_federate, const char *the_reason)
        throw (SecurityError);

    SecurityLevelID getLevelId(void) const
    { return LevelID ; };

    void setLevelId(SecurityLevelID NewLevelID);

    // ----------------------------------------------------------------------------
    // -- CRead Methods(used when building the Root Object Tree) --
    // ----------------------------------------------------------------------------
    // To be used only by CRead. It returns the new Attribute's Handle.
    AttributeHandle addAttribute(ObjectClassAttribute *the_attribute,
                                 Boolean is_inherited = RTI_FALSE);

    void addAttributesToChild(ObjectClass *child);

    // ----------------------------------
    // -- Publication and Subscription --
    // ----------------------------------
    void publish(FederateHandle theFederateHandle,
                 AttributeHandle *theAttributeList,
                 UShort theListSize,
                 bool PubOrUnpub)
        throw (AttributeNotDefined,
               RTIinternalError,
               SecurityError);


    Boolean subscribe(FederateHandle theFederate,
                      AttributeHandle *theAttributeList,
                      UShort theListSize,
                      bool SubOrUnsub)
        throw (AttributeNotDefined,
               RTIinternalError,
               SecurityError);

    // The second parameter is the Class of whose behalf the message are sent.
    // If SDM is called on the original class, the Federate may be a subscriber
    // of the class without stopping the process(because he has just subscribed)
    //
    // Return RTI_TRUE if the same SendDiscoverMessages method must be called
    // on the child classes of this class.
    // Return RTI_FALSE if no messages were sent because the Federate had
    // already receive DO messages for this class(and all child classes).
    Boolean sendDiscoverMessages(FederateHandle theFederate,
                                 ObjectClassHandle theOriginalClass);

    // --------------------------
    // -- Ownership Management --
    // --------------------------
    Boolean
    isAttributeOwnedByFederate(ObjectHandle theObject,
                               AttributeHandle theAttribute,
                               FederateHandle theFederateHandle)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               RTIinternalError);


    void
    queryAttributeOwnership(ObjectHandle theObject,
                            AttributeHandle theAttribute,
                            FederateHandle theFederateHandle)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               RTIinternalError);

    ObjectClassBroadcastList *
    negotiatedAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                            ObjectHandle theObjectHandle,
                                            AttributeHandle *theAttributeList,
                                            UShort theListSize,
                                            const char *theTag)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               AttributeAlreadyBeingDivested,
               RTIinternalError);


    void
    attributeOwnershipAcquisitionIfAvailable(FederateHandle theFederateHandle,
                                             ObjectHandle theObjectHandle,
                                             AttributeHandle *theAttributeList,
                                             UShort theListSize)
        throw (ObjectNotKnown,
               ObjectClassNotPublished,
               AttributeNotDefined,
               AttributeNotPublished,
               FederateOwnsAttributes,
               AttributeAlreadyBeingAcquired,
               RTIinternalError);

    ObjectClassBroadcastList *
    unconditionalAttributeOwnershipDivestiture(FederateHandle,
                                               ObjectHandle theObjectHandle,
                                               AttributeHandle*,
                                               UShort theListSize)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               RTIinternalError);

    void
    attributeOwnershipAcquisition(FederateHandle theFederateHandle,
                                  ObjectHandle theObjectHandle,
                                  AttributeHandle *theAttributeList,
                                  UShort theListSize,
                                  const char *theTag)
        throw (ObjectNotKnown,
               ObjectClassNotPublished,
               AttributeNotDefined,
               AttributeNotPublished,
               FederateOwnsAttributes,
               RTIinternalError);


    void
    cancelNegotiatedAttributeOwnershipDivestiture(FederateHandle,
                                                  ObjectHandle theObjectHandle,
                                                  AttributeHandle*,
                                                  UShort theListSize)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               AttributeDivestitureWasNotRequested,
               RTIinternalError);

    AttributeHandleSet *
    attributeOwnershipReleaseResponse(FederateHandle theFederateHandle,
                                      ObjectHandle theObjectHandle,
                                      AttributeHandle *theAttributeList,
                                      UShort theListSize)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               FederateWasNotAskedToReleaseAttribute,
               RTIinternalError);

    void cancelAttributeOwnershipAcquisition(FederateHandle theFederateHandle,
                                             ObjectHandle theObjectHandle,
                                             AttributeHandle *theAttributeList,
                                             UShort theListSize)

        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeAlreadyOwned,
               AttributeAcquisitionWasNotRequested,
               RTIinternalError);


    // --------------------------
    // -- RTI Support Services --
    // --------------------------

    AttributeHandle getAttributeHandle(const char *theName) const
        throw (AttributeNotDefined,
               RTIinternalError);

    const char *getAttributeName(AttributeHandle theHandle) const
        throw (AttributeNotDefined,
               RTIinternalError);

    ObjectClassBroadcastList *killFederate(FederateHandle theFederate)
        throw ();

    ObjectClassAttribute *
    getAttributeWithHandle(AttributeHandle the_handle) const
        throw (AttributeNotDefined);

    // -------------------------
    // -- Instance Management --
    // -------------------------
    ObjectClassBroadcastList *deleteInstance(FederateHandle theFederateHandle,
                                             ObjectHandle theObjectHandle,
                                             const char *theUserTag)
        throw (DeletePrivilegeNotHeld,
               ObjectNotKnown,
               RTIinternalError);

    Boolean isInstanceInClass(ObjectHandle theID);

    ObjectClassBroadcastList *registerInstance(FederateHandle,
                                               ObjectHandle theObjectHandle,
                                               const char *theObjectName)
        throw (ObjectClassNotPublished,
               ObjectAlreadyRegistered,
               RTIinternalError);

    void broadcastClassMessage(ObjectClassBroadcastList *ocb_list);

    ObjectClassBroadcastList *
    updateAttributeValues(FederateHandle theFederateHandle,
                          ObjectHandle theObjectHandle,
                          AttributeHandle *theAttributeArray,
                          AttributeValue *theValueArray,
                          UShort theArraySize,
                          FederationTime theTime,
                          const char *theUserTag)
        throw (ObjectNotKnown,
               AttributeNotDefined,
               AttributeNotOwned,
               RTIinternalError,
               InvalidObjectHandle);

private:

    // ------------------------
    // -- Private Attributes --
    // ------------------------

    // Should be allocated and deleted locally.
    char *Name ; //!< Object class name.
    ObjectClassHandle handle ; //!< Object class number.

    //! All non-inherited attributes have this default level.
    SecurityLevelID LevelID ;

    //! This Handle is the greatest handle of the class' subscribers.
    FederateHandle MaxSubscriberHandle ;

    list<ObjectClassAttribute *> attributeSet ; //!< Object class attributes list.
    list<Object *> objectSet ; //!< Object class instance list.

    // ---------------------
    // -- Private Methods --
    // ---------------------

    void sendToFederate(NetworkMessage *msg, FederateHandle theFederate);

    void sendToOwners(CDiffusion *diffusionList,
                      ObjectHandle theObjectHandle,
                      FederateHandle theFederate,
                      const char *theTag,
                      TypeNetworkMessage type);

    void sendMessage(NetworkMessage *msg, FederateHandle theDest);

    Object *getInstanceWithID(ObjectHandle the_id) const
        throw (ObjectNotKnown);

    // --------------------------------
    // -- Publication / Subscription --
    // --------------------------------
    Boolean isFederatePublisher(FederateHandle the_federate) const ;
    Boolean isFederateSubscriber(FederateHandle the_federate) const ;
};

} // namespace certi

#endif // _CERTI_OBJECT_CLASS_HH

// $Id: ObjectClass.hh,v 3.10 2003/04/09 16:41:10 breholee Exp $
