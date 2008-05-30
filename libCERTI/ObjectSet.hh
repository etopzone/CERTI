// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
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
// ----------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_SET_HH
#define _CERTI_OBJECT_SET_HH

// Project
class Object;
#include "SecurityServer.hh"
#include "MessageBuffer.hh"
#include "certi.hh"

// Standard
#include <map>
#include <string>

namespace certi {

class CERTI_EXPORT ObjectSet : private std::map<ObjectHandle, Object *>
{
public:
    // Public Methods.
    ObjectSet(SecurityServer *the_server);
    ~ObjectSet();

    ObjectHandle
    getObjectInstanceHandle(std::string) const
        throw (ObjectNotKnown, RTIinternalError);

    const char *
    getObjectInstanceName(ObjectHandle the_object) const
        throw (ObjectNotKnown, RTIinternalError);

    ObjectClassHandle
    getObjectClass(ObjectHandle the_object) const
        throw (ObjectNotKnown, FederateNotExecutionMember,
               ConcurrentAccessAttempted, RTIinternalError);

    void changeAttributeTransportationType(ObjectHandle the_object,
                                           AttributeHandle *the_attributes,
                                           UShort the_size,
                                           TransportType the_type)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               RTIinternalError, InvalidObjectHandle);

    void changeAttributeOrderType(ObjectHandle the_object,
                                  AttributeHandle *the_attributes,
                                  UShort the_size,
                                  TransportType the_type)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               RTIinternalError, InvalidObjectHandle);

    Object *registerObjectInstance(FederateHandle, ObjectClassHandle,
				   ObjectHandle, std::string)
        throw (ObjectAlreadyRegistered, ConcurrentAccessAttempted,
               SaveInProgress, RestoreInProgress, RTIinternalError);

    void deleteObjectInstance(FederateHandle, ObjectHandle, std::string tag)
        throw (ObjectNotKnown, DeletePrivilegeNotHeld,
               FederateNotExecutionMember, ConcurrentAccessAttempted,
               SaveInProgress, RestoreInProgress, RTIinternalError);

     FederateHandle requestObjectOwner(FederateHandle the_federate,
                                  ObjectHandle the_object)
    throw (ObjectNotKnown);

    void killFederate(FederateHandle) throw (RTIinternalError);

    // Ownership Management.

    bool isAttributeOwnedByFederate(FederateHandle the_federate,
                                    ObjectHandle the_object,
                                    AttributeHandle the_attribute) const
        throw (ObjectNotKnown, AttributeNotDefined, RTIinternalError);

    void
    queryAttributeOwnership(FederateHandle the_federate,
                            ObjectHandle the_object,
                            AttributeHandle the_attribute) const
        throw (ObjectNotKnown, AttributeNotDefined, RTIinternalError);

    void
    negotiatedAttributeOwnershipDivestiture(FederateHandle the_federate,
                                            ObjectHandle the_object,
                                            AttributeHandle *the_attributes,
                                            UShort the_size,
                                            const char *the_tag)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               AttributeAlreadyBeingDivested, RTIinternalError);

    void
    attributeOwnershipAcquisitionIfAvailable(FederateHandle the_federate,
                                             ObjectHandle the_object,
                                             AttributeHandle *the_attributes,
                                             UShort the_size)
        throw (ObjectNotKnown, ObjectClassNotPublished, AttributeNotDefined,
               AttributeNotPublished, FederateOwnsAttributes,
               AttributeAlreadyBeingAcquired, RTIinternalError);

    void
    unconditionalAttributeOwnershipDivestiture(FederateHandle the_federate,
                                               ObjectHandle the_object,
                                               AttributeHandle *the_attributes,
                                               UShort the_size)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               RTIinternalError);

    void
    attributeOwnershipAcquisition(FederateHandle the_federate,
                                  ObjectHandle the_object,
                                  AttributeHandle *the_attributes,
                                  UShort the_size,
                                  const char *the_tag)
        throw (ObjectNotKnown, ObjectClassNotPublished, AttributeNotDefined,
               AttributeNotPublished, FederateOwnsAttributes, RTIinternalError);

    void
    cancelNegotiatedAttributeOwnershipDivestiture(FederateHandle the_federate,
                                                  ObjectHandle the_object,
                                                  AttributeHandle *,
                                                  UShort the_size)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               AttributeDivestitureWasNotRequested, RTIinternalError);

    AttributeHandleSet *
    attributeOwnershipReleaseResponse(FederateHandle the_federate,
                                      ObjectHandle the_object,
                                      AttributeHandle *the_attributes,
                                      UShort the_size)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               FederateWasNotAskedToReleaseAttribute, RTIinternalError);

    void
    cancelAttributeOwnershipAcquisition(FederateHandle the_federate,
                                        ObjectHandle the_object,
                                        AttributeHandle *the_attributes,
                                        UShort the_size)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeAlreadyOwned,
               AttributeAcquisitionWasNotRequested, RTIinternalError);

    Object *getObject(ObjectHandle the_object) const
        throw (ObjectNotKnown);

protected:    
    void sendToFederate(NetworkMessage *msg,
                        FederateHandle the_federate) const ;

    SecurityServer *server ;
    
	typedef std::map<ObjectHandle,Object*,std::less<ObjectHandle> > Handle2ObjectMap_t;
	typedef std::map<std::string,Object*,std::less<std::string> > Name2ObjectMap_t; 
	typedef Handle2ObjectMap_t::const_iterator handledO_const_iterator; 
	typedef Name2ObjectMap_t::const_iterator namedO_const_iterator;
	Handle2ObjectMap_t OFromHandle;	
	Name2ObjectMap_t   OFromName;
	/* The message buffer used to send Network messages */
	MessageBuffer NM_msgBufSend;
};

} // namespace certi

#endif // _CERTI_OBJECT_SET_HH
