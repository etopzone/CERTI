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
// $Id: ObjectClass.hh,v 3.20 2005/03/15 14:37:29 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_CLASS_HH
#define _CERTI_OBJECT_CLASS_HH

// Project
#include "Object.hh"
#include "ObjectAttribute.hh"
#include "ObjectClassBroadcastList.hh"
#include "certi.hh"
#include "SecurityServer.hh"

// Standard
#include <list>

namespace certi {

class CDiffusion
{
public:
    typedef struct {
        FederateHandle federate ;
        AttributeHandle attribute ;

    } DiffStruct ;

    DiffStruct DiffArray[MAX_ATTRIBUTES_PER_CLASS] ;

    int size ;

    CDiffusion() { size = 0 ; };
};

/*! This class is used to describe an object class. It also contains the
  instance list from this class.
*/
class ObjectClass {

public:
    // Constructor & Destructor
    ObjectClass();
    ~ObjectClass();

    void display() const ;

    /*! Name attribute access(GetName reference must be considered
      READ-ONLY).  NewName length must be lower or equal to
      MAX_USER_TAG_LENGTH.
    */
    const char *getName() const { return Name ; };

    void setName(const char *new_name)
        throw (ValueLengthExceeded, RTIinternalError);

    void setHandle(ObjectClassHandle new_handle);
    ObjectClassHandle getHandle() const ;

    // Security Methods
    void checkFederateAccess(FederateHandle, const char *)
        throw (SecurityError);

    SecurityLevelID getLevelId() const { return LevelID ; };

    void setLevelId(SecurityLevelID NewLevelID);

    AttributeHandle addAttribute(ObjectClassAttribute *the_attribute,
                                 Boolean is_inherited = RTI_FALSE);

    void addAttributesToChild(ObjectClass *child);

    // Publication and Subscription
    void publish(FederateHandle theFederateHandle,
                 AttributeHandle *theAttributeList,
                 UShort theListSize,
                 bool PubOrUnpub)
        throw (AttributeNotDefined, RTIinternalError, SecurityError);

    Boolean subscribe(FederateHandle theFederate,
                      AttributeHandle *theAttributeList,
                      UShort theListSize,
                      bool SubOrUnsub,
		      const RegionImp * = 0)
        throw (AttributeNotDefined, RTIinternalError, SecurityError);
    
    void unsubscribe(FederateHandle, RegionImp *);

    // The second parameter is the Class of whose behalf the message
    // are sent. If SDM is called on the original class, the Federate
    // may be a subscriber of the class without stopping the
    // process(because he has just subscribed)
    //
    // Return RTI_TRUE if the same SendDiscoverMessages method must be called
    // on the child classes of this class.
    // Return RTI_FALSE if no messages were sent because the Federate had
    // already receive DO messages for this class(and all child classes).
    Boolean sendDiscoverMessages(FederateHandle theFederate,
                                 ObjectClassHandle theOriginalClass);

    // Ownership Management
    ObjectClassBroadcastList *
    negotiatedAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                            ObjectHandle theObjectHandle,
                                            AttributeHandle *theAttributeList,
                                            UShort theListSize,
                                            const char *theTag)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               AttributeAlreadyBeingDivested, RTIinternalError);


    void
    attributeOwnershipAcquisitionIfAvailable(FederateHandle theFederateHandle,
                                             ObjectHandle theObjectHandle,
                                             AttributeHandle *theAttributeList,
                                             UShort theListSize)
        throw (ObjectNotKnown, ObjectClassNotPublished, AttributeNotDefined,
               AttributeNotPublished, FederateOwnsAttributes,
               AttributeAlreadyBeingAcquired, RTIinternalError);

    ObjectClassBroadcastList *
    unconditionalAttributeOwnershipDivestiture(FederateHandle,
                                               ObjectHandle theObjectHandle,
                                               AttributeHandle*,
                                               UShort theListSize)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               RTIinternalError);

    void
    attributeOwnershipAcquisition(FederateHandle theFederateHandle,
                                  ObjectHandle theObjectHandle,
                                  AttributeHandle *theAttributeList,
                                  UShort theListSize,
                                  const char *theTag)
        throw (ObjectNotKnown, ObjectClassNotPublished, AttributeNotDefined,
               AttributeNotPublished, FederateOwnsAttributes, RTIinternalError);

    AttributeHandleSet *
    attributeOwnershipReleaseResponse(FederateHandle theFederateHandle,
                                      ObjectHandle theObjectHandle,
                                      AttributeHandle *theAttributeList,
                                      UShort theListSize)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               FederateWasNotAskedToReleaseAttribute, RTIinternalError);

    void cancelAttributeOwnershipAcquisition(FederateHandle theFederateHandle,
                                             ObjectHandle theObjectHandle,
                                             AttributeHandle *theAttributeList,
                                             UShort theListSize)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeAlreadyOwned,
               AttributeAcquisitionWasNotRequested, RTIinternalError);

    // RTI Support Services
    AttributeHandle getAttributeHandle(const char *theName) const
        throw (NameNotFound, RTIinternalError);

    const char *getAttributeName(AttributeHandle theHandle) const
        throw (AttributeNotDefined, RTIinternalError);

    ObjectClassBroadcastList *killFederate(FederateHandle theFederate)
        throw ();

    ObjectClassAttribute *
    getAttributeWithHandle(AttributeHandle the_handle) const
        throw (AttributeNotDefined);

    // Instance Management
    ObjectClassBroadcastList *deleteInstance(FederateHandle theFederateHandle,
                                             ObjectHandle theObjectHandle,
                                             const char *theUserTag)
        throw (DeletePrivilegeNotHeld,
               ObjectNotKnown,
               RTIinternalError);

    bool isInstanceInClass(ObjectHandle theID);

    ObjectClassBroadcastList *
    registerObjectInstance(FederateHandle, Object *, ObjectClassHandle)
        throw (ObjectClassNotPublished, ObjectAlreadyRegistered,
               RTIinternalError);

    void broadcastClassMessage(ObjectClassBroadcastList *ocb_list,
			       Object *source = 0);

    ObjectClassBroadcastList *
    updateAttributeValues(FederateHandle theFederateHandle,
                          ObjectHandle theObjectHandle,
                          AttributeHandle *theAttributeArray,
                          AttributeValue *theValueArray,
                          UShort theArraySize,
                          FederationTime theTime,
                          const char *theUserTag)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               RTIinternalError, InvalidObjectHandle);

    // PUBLIC ATTRIBUTES
    // Note: Most of the folling Attributes should be private. In fact,
    // they can not be reached from the RTIG and RTIA components, because
    // they are hidden from them by ObjectClassSet.
    // They were *temporarily* kept public for convenience in the
    // CRead class that build

    // the Object Classes tree and set all Parent/Child dependences and
    // attributes.
    ObjectClassHandle Father ; //!< Object parent number.
    std::list<ObjectClassHandle> sonSet ;

    //! This Object help to find a TCPLink from a Federate Handle.
    SecurityServer *server ;

    //! Depth in the class tree structure. Used only by CRead.
    UShort Depth ;

private:
    void sendToFederate(NetworkMessage *msg, FederateHandle theFederate);

    void sendToOwners(CDiffusion *diffusionList,
                      ObjectHandle theObjectHandle,
                      FederateHandle theFederate,
                      const char *theTag,
                      NetworkMessage::Type type);

    void sendMessage(NetworkMessage *msg, FederateHandle theDest);

    Object *getInstanceWithID(ObjectHandle the_id) const
        throw (ObjectNotKnown);

    bool isFederatePublisher(FederateHandle the_federate) const ;
    bool isFederateSubscriber(FederateHandle the_federate) const ;

    // ATTRIBUTES

    // Should be allocated and deleted locally.
    char *Name ; //!< Object class name.
    ObjectClassHandle handle ; //!< Object class number.

    //! All non-inherited attributes have this default level.
    SecurityLevelID LevelID ;

    //! This Handle is the greatest handle of the class' subscribers.
    FederateHandle MaxSubscriberHandle ;

    std::list<ObjectClassAttribute *> attributeSet ;
    std::list<Object *> objectSet ;
};

} // namespace certi

#endif // _CERTI_OBJECT_CLASS_HH

// $Id: ObjectClass.hh,v 3.20 2005/03/15 14:37:29 breholee Exp $
