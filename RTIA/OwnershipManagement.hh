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
// $Id: OwnershipManagement.hh,v 3.5 2003/06/27 17:26:28 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIA_OWM
#define _CERTI_RTIA_OWM

#include "FederationManagement.hh"
#include "Communications.hh"
#include "Files.hh"
#include "baseTypes.hh"
#include "RTItypes.hh"
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
                                            AttributeHandle *attribArray,
                                            UShort attribArraySize,
                                            const char *theTag,
                                            TypeException &e);
    void
    cancelnegotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                                  AttributeHandle *,
                                                  UShort attribArraySize,
                                                  TypeException &e);

    void
    attributeOwnershipAcquisitionIfAvailable(ObjectHandle theObject,
                                             AttributeHandle *attribArray,
                                             UShort attribArraySize,
                                             TypeException &e);

    void
    cancelattributeOwnershipAcquisition(ObjectHandle theObject,
                                        AttributeHandle *attribArray,
                                        UShort attribArraySize,
                                        TypeException &e);

    void
    attributeOwnershipAcquisitionNotification(ObjectHandle theObject,
                                              AttributeHandle *attribArray,
                                              UShort attribArraySize,
                                              FederateHandle theOwner,
                                              TypeException &e);
    void
    attributeOwnershipUnavailable(ObjectHandle theObject,
                                  AttributeHandle *attribArray,
                                  UShort attribArraySize,
                                  FederateHandle theOwner,
                                  TypeException &e);

    void
    requestAttributeOwnershipAssumption(ObjectHandle theObject,
                                        AttributeHandle *attribArray,
                                        UShort attribArraySize,
                                        FederateHandle theOwner,
                                        char *theTag,
                                        TypeException &e);

    void
    unconditionalAttributeOwnershipDivestiture(ObjectHandle theObject,
                                               AttributeHandle *attribArray,
                                               UShort attribArraySize,
                                               TypeException &e);

    void
    attributeOwnershipAcquisition(ObjectHandle theObject,
                                  AttributeHandle *attribArray,
                                  UShort attribArraySize,
                                  const char *theTag,
                                  TypeException &e);

    void
    requestAttributeOwnershipRelease(ObjectHandle theObject,
                                     AttributeHandle *attribArray,
                                     UShort attribArraySize,
                                     char *theTag,
                                     TypeException &e);

    void
    attributeOwnershipDivestitureNotification(ObjectHandle theObject,
                                              AttributeHandle *attribArray,
                                              UShort attribArraySize,
                                              TypeException &e);
    AttributeHandleSet*
    attributeOwnershipRealeaseResponse(ObjectHandle theObject,
                                       AttributeHandle *attribArray,
                                       UShort attribArraySize,
                                       TypeException &e);

    void
    confirmAttributeOwnershipAcquisitionCancellation(ObjectHandle theObject,
                                                     AttributeHandle *,
                                                     UShort attribArraySize,
                                                     TypeException &e);

protected:
    Communications *comm ;
    Queues *queues ;
    FederationManagement *fm ;
};

}} // namespace certi/rtia

#endif // _CERTI_RTIA_OWM

// $Id: OwnershipManagement.hh,v 3.5 2003/06/27 17:26:28 breholee Exp $
