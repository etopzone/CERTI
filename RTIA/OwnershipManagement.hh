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
// $Id: OwnershipManagement.hh,v 3.10 2010/02/27 16:53:36 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIA_OWM
#define _CERTI_RTIA_OWM

#include "certi.hh"
#include "FederationManagement.hh"
#include "Communications.hh"
#include "Files.hh"
#include "RootObject.hh"

namespace certi {
namespace rtia {

// Prototypes de classes existantes
class Communications ;
class Queues ;
class FederationManagement ;

//! Ownership Management main class
class OwnershipManagement
{
public:
    OwnershipManagement(Communications *GC,
                        FederationManagement *GF);

    ~OwnershipManagement();

    // Ownership Management services
    const char *
    attributeOwnedByFederate(ObjectHandle theObject,
                             AttributeHandle theAttribute,
                             TypeException &e);

    void
    queryAttributeOwnership(ObjectHandle theObject,
                            AttributeHandle theAttribute,
                            TypeException &e);

    void
    informAttributeOwnership(ObjectHandle theObject,
                             AttributeHandle theAttribute,
                             FederateHandle theOwner,
                             TypeException &e);

    void
    attributeIsNotOwned(ObjectHandle theObject,
                        AttributeHandle theAttribute,
                        FederateHandle theOwner,
                        TypeException &e);

    void
    negotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                            const std::vector <AttributeHandle> &attribArray,
                                            uint32_t attribArraySize,
                                            const std::string& theTag,
                                            TypeException &e);
    void
    cancelnegotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                                  const std::vector <AttributeHandle> &,
                                                  uint32_t attribArraySize,
                                                  TypeException &e);

    void
    attributeOwnershipAcquisitionIfAvailable(ObjectHandle theObject,
    		                                 const std::vector <AttributeHandle> &attribArray,
                                             uint32_t attribArraySize,
                                             TypeException &e);

    void
    cancelattributeOwnershipAcquisition(ObjectHandle theObject,
    		                            const std::vector <AttributeHandle> &attribArray,
                                        uint32_t attribArraySize,
                                        TypeException &e);

    void
    attributeOwnershipAcquisitionNotification(ObjectHandle theObject,
                                              const std::vector <AttributeHandle> &attribArray,
                                              uint32_t attribArraySize,
                                              FederateHandle theOwner,
                                              TypeException &e);
    void
    attributeOwnershipUnavailable(ObjectHandle theObject,
                                  const std::vector <AttributeHandle> &attribArray,
                                  uint32_t attribArraySize,
                                  FederateHandle theOwner,
                                  TypeException &e);

    void
    requestAttributeOwnershipAssumption(ObjectHandle theObject,
                                        const std::vector <AttributeHandle> &attribArray,
                                        uint32_t attribArraySize,
                                        FederateHandle theOwner,
                                        const std::string& theTag,
                                        TypeException &e);

    void
    unconditionalAttributeOwnershipDivestiture(ObjectHandle theObject,
                                               const std::vector <AttributeHandle> &attribArray,
                                               uint32_t attribArraySize,
                                               TypeException &e);

    void
    attributeOwnershipAcquisition(ObjectHandle theObject,
                                  const std::vector <AttributeHandle> &attribArray,
                                  uint32_t attribArraySize,
                                  const std::string& theTag,
                                  TypeException &e);

    void
    requestAttributeOwnershipRelease(ObjectHandle theObject,
                                     const std::vector <AttributeHandle> &attribArray,
                                     uint32_t attribArraySize,
                                     const std::string& theTag,
                                     TypeException &e);

    void
    attributeOwnershipDivestitureNotification(ObjectHandle theObject,
                                              const std::vector <AttributeHandle> &attribArray,
                                              uint32_t attribArraySize,
                                              TypeException &e);
    AttributeHandleSet*
    attributeOwnershipRealeaseResponse(ObjectHandle theObject,
                                       const std::vector <AttributeHandle> &attribArray,
                                       uint32_t attribArraySize,
                                       TypeException &e);

    void
    confirmAttributeOwnershipAcquisitionCancellation(ObjectHandle theObject,
                                                     const std::vector <AttributeHandle> &,
                                                     uint32_t attribArraySize,
                                                     TypeException &e);

protected:
    Communications *comm ;
    Queues *queues ;
    FederationManagement *fm ;
};

}} // namespace certi/rtia

#endif // _CERTI_RTIA_OWM

// $Id: OwnershipManagement.hh,v 3.10 2010/02/27 16:53:36 erk Exp $
