// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: ObjectClass.hh,v 3.35 2008/06/10 13:41:47 rousse Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_CLASS_HH
#define _CERTI_OBJECT_CLASS_HH

// forward declarations
namespace certi {
    class    Object;	
	class    ObjectAttribute;
	class    ObjectClass;
	class    ObjectClassSet;
	class    ObjectClassAttribute;
	class    ObjectClassBroadcastList;
}

// CERTI headers
#include "certi.hh"
#include "SecurityServer.hh"
#include "Named.hh"

// Standard
#include <list>
#include <string>

namespace certi {

class CDiffusion
{
public:
    typedef struct {
        FederateHandle federate ;
        AttributeHandle attribute ;

    } DiffStruct ;

    //DiffStruct DiffArray[MAX_ATTRIBUTES_PER_CLASS] ;
    std::vector <DiffStruct> DiffArray ;

    int size ;

    CDiffusion() { size = 0 ; };
};

/*! OMT object class. It also contains the instance list from this
  class.
*/
class CERTI_EXPORT ObjectClass : public Named {

public:
    ObjectClass();
    ~ObjectClass();

    void display() const ;

    void setHandle(ObjectClassHandle new_handle);
    ObjectClassHandle getHandle() const ;

    void setSuperclass(ObjectClassHandle h) { superClass = h ; };
    ObjectClassHandle getSuperclass() const { return superClass ; };
    void addSubclass(ObjectClass *);
    ObjectClass* getSubClassByName(const std::string subClassName);
    ObjectClassSet* getSubClasses() {return subClasses;};

    // Security Methods
    void checkFederateAccess(FederateHandle, const char *)
        throw (SecurityError);

    SecurityLevelID getLevelId() const { return levelId ; };

    void setLevelId(SecurityLevelID NewLevelID);

    AttributeHandle addAttribute(ObjectClassAttribute *the_attribute,
                                 bool is_inherited = false);

    void addAttributesToChild(ObjectClass *child);

    // Publication and Subscription
    void publish(FederateHandle theFederateHandle,
                 std::vector <AttributeHandle> &theAttributeList,
                 UShort theListSize,
                 bool PubOrUnpub)
        throw (AttributeNotDefined, RTIinternalError, SecurityError);

    bool subscribe(FederateHandle, std::vector <AttributeHandle> &, int, const RTIRegion *)
        throw (AttributeNotDefined, RTIinternalError, SecurityError);

    void unsubscribe(FederateHandle, const RTIRegion *);
    void unsubscribe(FederateHandle);

    // Ownership Management
    ObjectClassBroadcastList *
    negotiatedAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                            ObjectHandle theObjectHandle,
                                            std::vector <AttributeHandle> &theAttributeList,
                                            UShort theListSize,
                                            const char *theTag)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               AttributeAlreadyBeingDivested, RTIinternalError);


    void
    attributeOwnershipAcquisitionIfAvailable(FederateHandle theFederateHandle,
                                             ObjectHandle theObjectHandle,
                                             std::vector <AttributeHandle> &theAttributeList,
                                             UShort theListSize)
        throw (ObjectNotKnown, ObjectClassNotPublished, AttributeNotDefined,
               AttributeNotPublished, FederateOwnsAttributes,
               AttributeAlreadyBeingAcquired, RTIinternalError);

    ObjectClassBroadcastList *
    unconditionalAttributeOwnershipDivestiture(FederateHandle,
                                               ObjectHandle theObjectHandle,
                                               std::vector <AttributeHandle> &,
                                               UShort theListSize)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               RTIinternalError);

    void
    attributeOwnershipAcquisition(FederateHandle theFederateHandle,
                                  ObjectHandle theObjectHandle,
                                  std::vector <AttributeHandle> &theAttributeList,
                                  UShort theListSize,
                                  const char *theTag)
        throw (ObjectNotKnown, ObjectClassNotPublished, AttributeNotDefined,
               AttributeNotPublished, FederateOwnsAttributes, RTIinternalError);

    AttributeHandleSet *
    attributeOwnershipReleaseResponse(FederateHandle theFederateHandle,
                                      ObjectHandle theObjectHandle,
                                      std::vector <AttributeHandle> &theAttributeList,
                                      UShort theListSize)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               FederateWasNotAskedToReleaseAttribute, RTIinternalError);

    void cancelAttributeOwnershipAcquisition(FederateHandle theFederateHandle,
                                             ObjectHandle theObjectHandle,
                                             std::vector <AttributeHandle> &theAttributeList,
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

    ObjectClassAttribute *getAttribute(AttributeHandle the_handle) const
        throw (AttributeNotDefined);

    // Instance Management
    ObjectClassBroadcastList *deleteInstance(FederateHandle theFederateHandle,
                                             ObjectHandle theObjectHandle,
					     FederationTime theTime,
                                             std::string theUserTag)
        throw (DeletePrivilegeNotHeld,
               ObjectNotKnown,
               RTIinternalError);

    ObjectClassBroadcastList *deleteInstance(FederateHandle theFederateHandle,
                                             ObjectHandle theObjectHandle,
                                             std::string theUserTag)
        throw (DeletePrivilegeNotHeld,
               ObjectNotKnown,
               RTIinternalError);

    bool isInstanceInClass(ObjectHandle theID);

    ObjectClassBroadcastList *
    registerObjectInstance(FederateHandle, Object *, ObjectClassHandle)
        throw (ObjectClassNotPublished, ObjectAlreadyRegistered,
               RTIinternalError);

    void broadcastClassMessage(ObjectClassBroadcastList *ocb_list,
			       const Object * = NULL);

    ObjectClassBroadcastList *
    updateAttributeValues(FederateHandle, Object *, std::vector <AttributeHandle> &,
                          ValueLengthPair *, int, FederationTime, const char *)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               RTIinternalError, InvalidObjectHandle);

    ObjectClassBroadcastList *
    updateAttributeValues(FederateHandle, Object *, std::vector <AttributeHandle> &,
                          ValueLengthPair *, int, const char *)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               RTIinternalError, InvalidObjectHandle);

    void recursiveDiscovering(FederateHandle, ObjectClassHandle)
	throw (ObjectClassNotDefined);

    Object *getInstanceWithID(ObjectHandle the_id) const
        throw (ObjectNotKnown);

    //! This Object help to find a TCPLink from a Federate Handle.
    SecurityServer *server ;

private:
    void sendToFederate(NetworkMessage *msg, FederateHandle theFederate);

    void sendToOwners(CDiffusion *diffusionList,
                      ObjectHandle theObjectHandle,
                      FederateHandle theFederate,
                      const char *theTag,
                      NetworkMessage::Type type);

    void sendMessage(NetworkMessage *msg, FederateHandle theDest);

    bool isFederatePublisher(FederateHandle the_federate) const ;
    bool isSubscribed(FederateHandle) const ;

    // The second parameter is the Class of whose behalf the message
    // are sent. If SDM is called on the original class, the Federate
    // may be a subscriber of the class without stopping the
    // process(because he has just subscribed)
    //
    // Return RTI_TRUE if the same SendDiscoverMessages method must be called
    // on the child classes of this class.
    // Return RTI_FALSE if no messages were sent because the Federate had
    // already receive DO messages for this class(and all child classes).
    bool sendDiscoverMessages(FederateHandle, ObjectClassHandle);

    // Attributes   
    ObjectClassHandle handle ;
    FederateHandle maxSubscriberHandle ; //! greatest subscriber handle
    SecurityLevelID levelId ; //! default level for non inherited attributes
    std::list<ObjectClassAttribute *> attributeSet ;
    std::list<Object *> objectSet ;
    ObjectClassHandle superClass;
    // The set of sub classes of this object class 
    ObjectClassSet*   subClasses;
    /* The message buffer used to send Network messages */
    MessageBuffer NM_msgBufSend;
};

} // namespace certi

#endif // _CERTI_OBJECT_CLASS_HH

// $Id: ObjectClass.hh,v 3.35 2008/06/10 13:41:47 rousse Exp $
