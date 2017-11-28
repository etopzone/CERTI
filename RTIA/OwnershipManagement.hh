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
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIA_OWM
#define _CERTI_RTIA_OWM

#include <string>

#include <include/certi.hh>

#include <libCERTI/RootObject.hh>

#include "Communications.hh"
#include "FederationManagement.hh"
#include "Files.hh"

namespace certi {
namespace rtia {

// Prototypes de classes existantes
class Communications;
class Queues;
class FederationManagement;

//! Ownership Management main class
class OwnershipManagement {
public:
    OwnershipManagement(Communications* GC, FederationManagement* GF);

    ~OwnershipManagement();

    // Ownership Management services
    std::string attributeOwnedByFederate(ObjectHandle theObject, AttributeHandle theAttribute, Exception::Type& e);

    void queryAttributeOwnership(ObjectHandle theObject, AttributeHandle theAttribute, Exception::Type& e);

    void informAttributeOwnership(ObjectHandle the_object,
                                  AttributeHandle the_attribute,
                                  FederateHandle the_owner,
                                  Exception::Type& e);

    void
    attributeIsNotOwned(ObjectHandle the_object, AttributeHandle the_attribute, FederateHandle, Exception::Type& e);

    void negotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                                 const std::vector<AttributeHandle>& attribArray,
                                                 uint32_t attribArraySize,
                                                 const std::string& theTag,
                                                 Exception::Type& e);

    void cancelnegotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                                       const std::vector<AttributeHandle>& attribArray,
                                                       uint32_t attribArraySize,
                                                       Exception::Type& e);

    void attributeOwnershipAcquisitionIfAvailable(ObjectHandle theObject,
                                                  const std::vector<AttributeHandle>& attribArray,
                                                  uint32_t attribArraySize,
                                                  Exception::Type& e);

    void cancelattributeOwnershipAcquisition(ObjectHandle theObject,
                                             const std::vector<AttributeHandle>& attribArray,
                                             uint32_t attribArraySize,
                                             Exception::Type& e);

    void attributeOwnershipAcquisitionNotification(ObjectHandle the_object,
                                                   const std::vector<AttributeHandle>& the_attributes,
                                                   uint32_t the_size,
                                                   FederateHandle,
                                                   Exception::Type& e);

    void attributeOwnershipUnavailable(ObjectHandle the_object,
                                       const std::vector<AttributeHandle>& the_attributes,
                                       uint32_t the_size,
                                       FederateHandle,
                                       Exception::Type& e);

    void requestAttributeOwnershipAssumption(ObjectHandle the_object,
                                             const std::vector<AttributeHandle>& the_attributes,
                                             uint32_t the_size,
                                             FederateHandle,
                                             const std::string& the_tag,
                                             Exception::Type& e);

    void unconditionalAttributeOwnershipDivestiture(ObjectHandle theObject,
                                                    const std::vector<AttributeHandle>& attribArray,
                                                    uint32_t attribArraySize,
                                                    Exception::Type& e);

    void attributeOwnershipAcquisition(ObjectHandle theObject,
                                       const std::vector<AttributeHandle>& attribArray,
                                       uint32_t attribArraySize,
                                       const std::string& theTag,
                                       Exception::Type& e);

    void requestAttributeOwnershipRelease(ObjectHandle the_object,
                                          const std::vector<AttributeHandle>& the_attributes,
                                          uint32_t the_size,
                                          const std::string& the_tag,
                                          Exception::Type& e);

    void attributeOwnershipDivestitureNotification(ObjectHandle the_object,
                                                   const std::vector<AttributeHandle>& the_attributes,
                                                   uint32_t the_size,
                                                   Exception::Type& e);

    AttributeHandleSet* attributeOwnershipRealeaseResponse(ObjectHandle theObject,
                                                           const std::vector<AttributeHandle>& attribArray,
                                                           uint32_t attribArraySize,
                                                           Exception::Type& e);

    void confirmAttributeOwnershipAcquisitionCancellation(ObjectHandle the_object,
                                                          const std::vector<AttributeHandle>& the_attributes,
                                                          uint32_t the_size,
                                                          Exception::Type& e);

protected:
    Communications* comm;
    Queues* queues;
    FederationManagement* fm;
};
}
} // namespace certi/rtia

#endif // _CERTI_RTIA_OWM
