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
#include "GAV.hh"
#include "SecurityServer.hh"
#include <include/certi.hh>
#include <libHLA/MessageBuffer.hh>

// Standard
#include <map>
#include <string>

namespace certi {

class CERTI_EXPORT ObjectSet {
public:
    // Public Methods.
    ObjectSet(SecurityServer* the_server);
    ~ObjectSet();

    //! Print the Root Object tree to the standard output.
    void display() const;

    ObjectHandle getObjectInstanceHandle(const std::string&) const;

    const std::string& getObjectInstanceName(ObjectHandle the_object) const;

    ObjectClassHandle getObjectClass(ObjectHandle the_object) const;

    void changeAttributeTransportationType(ObjectHandle the_object,
                                           AttributeHandle* the_attributes,
                                           uint16_t the_size,
                                           TransportType the_type);

    void changeAttributeOrderType(ObjectHandle the_object,
                                  AttributeHandle* the_attributes,
                                  uint16_t the_size,
                                  TransportType the_type);

    Object* registerObjectInstance(FederateHandle, ObjectClassHandle, ObjectHandle, const std::string&);

    void deleteObjectInstance(FederateHandle, ObjectHandle, const std::string& tag);

    FederateHandle requestObjectOwner(FederateHandle the_federate, ObjectHandle the_object);

    void killFederate(FederateHandle);

    // Ownership Management.

    bool isAttributeOwnedByFederate(FederateHandle the_federate,
                                    ObjectHandle the_object,
                                    AttributeHandle the_attribute) const;

    void
    queryAttributeOwnership(FederateHandle the_federate, ObjectHandle the_object, AttributeHandle the_attribute) const;

    void negotiatedAttributeOwnershipDivestiture(FederateHandle the_federate,
                                                 ObjectHandle the_object,
                                                 AttributeHandle* the_attributes,
                                                 uint16_t the_size,
                                                 const std::string& the_tag);

    void attributeOwnershipAcquisitionIfAvailable(FederateHandle the_federate,
                                                  ObjectHandle the_object,
                                                  AttributeHandle* the_attributes,
                                                  uint16_t the_size);

    void unconditionalAttributeOwnershipDivestiture(FederateHandle the_federate,
                                                    ObjectHandle the_object,
                                                    AttributeHandle* the_attributes,
                                                    uint16_t the_size);

    void attributeOwnershipAcquisition(FederateHandle the_federate,
                                       ObjectHandle the_object,
                                       AttributeHandle* the_attributes,
                                       uint16_t the_size,
                                       const std::string& the_tag);

    void cancelNegotiatedAttributeOwnershipDivestiture(FederateHandle the_federate,
                                                       ObjectHandle the_object,
                                                       const std::vector<AttributeHandle>&,
                                                       uint16_t the_size);

    AttributeHandleSet* attributeOwnershipReleaseResponse(FederateHandle the_federate,
                                                          ObjectHandle the_object,
                                                          std::vector<AttributeHandle>& the_attributes,
                                                          uint16_t the_size);

    void cancelAttributeOwnershipAcquisition(FederateHandle the_federate,
                                             ObjectHandle the_object,
                                             std::vector<AttributeHandle>& the_attributes,
                                             uint16_t the_size);

    Object* getObject(ObjectHandle the_object) const;

    Object* getObjectByName(const std::string& the_object_name) const;

    void getAllObjectInstancesFromFederate(FederateHandle the_federate, std::vector<ObjectHandle>& handles);

protected:
    void sendToFederate(NetworkMessage* msg, FederateHandle the_federate) const;

    SecurityServer* server;

    typedef std::map<ObjectHandle, Object*, std::less<ObjectHandle>> Handle2ObjectMap_t;
    typedef std::map<std::string, Object*, std::less<std::string>> Name2ObjectMap_t;

    typedef Handle2ObjectMap_t::const_iterator Handle2ObjectMap_const_iterator; 
    typedef Name2ObjectMap_t::const_iterator Name2ObjectMap_const_iterator;

    Handle2ObjectMap_t OFromHandle;
    Name2ObjectMap_t OFromName;
    /* The message buffer used to send Network messages */
    MessageBuffer NM_msgBufSend;
};

} // namespace certi

#endif // _CERTI_OBJECT_SET_HH
