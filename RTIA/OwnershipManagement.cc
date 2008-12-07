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
// $Id: OwnershipManagement.cc,v 3.17 2008/12/07 20:16:09 gotthardp Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "OwnershipManagement.hh"

#include "PrettyDebug.hh"
#include "NM_Classes.hh"

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#endif
#include <memory>

namespace certi {
namespace rtia {

static pdCDebug D("RTIA_OWM", "(RTIA OwM) ");

// ----------------------------------------------------------------------------
//! Constructor.
OwnershipManagement::OwnershipManagement(Communications *GC,
                                         FederationManagement *GF)
{
    comm = GC ;
    fm = GF ;
}

// ----------------------------------------------------------------------------
//! Destructor.
OwnershipManagement::~OwnershipManagement()
{
}

// ----------------------------------------------------------------------------
//! isAttributeOwnedByFederate.
const char *
OwnershipManagement::attributeOwnedByFederate(ObjectHandle theObject,
                                              AttributeHandle theAttribute,
                                              TypeException &e)
{
    NM_Is_Attribute_Owned_By_Federate req ;

    D.Out(pdDebug,
          "IS_ATTRIBUTE_OWNED_BY_FEDERATE, attribute %u of object %u",
          theAttribute, theObject);

    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArray.resize(1) ;
    req.handleArray[0] = theAttribute ;
    req.handleArraySize = 1 ;

    D.Out(pdDebug, "Federate %u ", fm->federate);

    comm->sendMessage(&req);

    // waitMessage is only used to verify transmission was OK.
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(
                      NetworkMessage::IS_ATTRIBUTE_OWNED_BY_FEDERATE,
                      req.federate));

    e = rep->exception ;

    if (e == e_NO_EXCEPTION)
        return(rep->getLabel().c_str());
    else
        return("\0");
}

// ----------------------------------------------------------------------------
//! queryAttributeOwnership.
void
OwnershipManagement::queryAttributeOwnership(ObjectHandle theObject,
                                             AttributeHandle theAttribute,
                                             TypeException &e)
{
    NM_Query_Attribute_Ownership req;

    D.Out(pdDebug,
          "QUERY_ATTRIBUTE_OWNERSHIP, attribute %u from object %u",
          theAttribute, theObject);

    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArray.resize(1) ;
    req.handleArray[0] = theAttribute ;
    req.handleArraySize = 1 ;

    D.Out(pdDebug, "Federate %u ", fm->federate);

    comm->sendMessage(&req);

    //AttendreMsg ne sert que pour verifier que la transmission a ete OK
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::QUERY_ATTRIBUTE_OWNERSHIP, req.federate));

    e = rep->exception ;
}

// ----------------------------------------------------------------------------
//! negotiatedAttributeOwnershipDivestiture.
void
OwnershipManagement::
negotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                        std::vector <AttributeHandle> &attribArray,
                                        UShort attribArraySize,
                                        std::string theTag,
                                        TypeException &e)

{
    NM_Negotiated_Attribute_Ownership_Divestiture req;

    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArray.resize(attribArraySize) ;
    req.handleArraySize = attribArraySize ;

    for (int i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    req.setLabel(theTag);

    D.Out(pdDebug, "NEGOTIATED_DIVESTITURE Federate %u ", fm->federate);

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
                      req.federate));

    e = rep->exception ;
}

// ----------------------------------------------------------------------------
//! cancelNnegotiatedAttributeOwnershipDivestiture.
void
OwnershipManagement::
cancelnegotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                              std::vector <AttributeHandle> &attribArray,
                                              UShort attribArraySize,
                                              TypeException &e)
{
    NM_Cancel_Negotiated_Attribute_Ownership_Divestiture req;

    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArray.resize(attribArraySize) ;
    req.handleArraySize = attribArraySize ;

    for (int i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    D.Out(pdDebug, "CANCEL_NEGOTIATED_DIVESTITURE Federate %u ",
          fm->federate);

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(
                      NetworkMessage::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
                      req.federate));

    e = rep->exception ;
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisitionIfAvailable.
void
OwnershipManagement::
attributeOwnershipAcquisitionIfAvailable(ObjectHandle theObject,
                                         std::vector <AttributeHandle> &attribArray,
                                         UShort attribArraySize,
                                         TypeException &e)
{
    NM_Attribute_Ownership_Acquisition_If_Available req;
    
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArray.resize(attribArraySize) ;
    req.handleArraySize = attribArraySize ;

    for (int i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    D.Out(pdDebug, "AcquisitionIfAvailable Federate %u ", fm->federate);

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE,
                      req.federate));

    e = rep->exception ;
}

// ----------------------------------------------------------------------------
//! unconditionalAttributeOwnershipDivestiture.
void
OwnershipManagement::
unconditionalAttributeOwnershipDivestiture(ObjectHandle theObject,
                                           std::vector <AttributeHandle> &attribArray,
                                           UShort attribArraySize,
                                           TypeException &e)
{
    NM_Unconditional_Attribute_Ownership_Divestiture req;

    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArray.resize(attribArraySize) ;
    req.handleArraySize = attribArraySize ;

    for (int i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    D.Out(pdDebug, "UNCONDITIONAL_DIVESTITURE Federate %u ",
          fm->federate);

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
                      req.federate));

    e = rep->exception ;
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisition.
void
OwnershipManagement::
attributeOwnershipAcquisition(ObjectHandle theObject,
                              std::vector <AttributeHandle> &attribArray,
                              UShort attribArraySize,
                              std::string theTag,
                              TypeException &e)
{
    NM_Attribute_Ownership_Acquisition req;

    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArray.resize(attribArraySize) ;
    req.handleArraySize = attribArraySize ;

    for (int i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    req.setLabel(theTag);

    D.Out(pdDebug, "OWNERSHIP_ACQUISITION Federate %u ", fm->federate);

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION,
                      req.federate));

    e = rep->exception ;
}

// ----------------------------------------------------------------------------
//! attributeOwnershipRealeaseResponse.
AttributeHandleSet*
OwnershipManagement::
attributeOwnershipRealeaseResponse(ObjectHandle theObject,
                                   std::vector <AttributeHandle> &attribArray,
                                   UShort attribArraySize,
                                   TypeException &e)
{
    NM_Attribute_Ownership_Release_Response req;

    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArray.resize(attribArraySize) ;
    req.handleArraySize = attribArraySize ;

    D.Out(pdDebug, "RELEASE_RESPONSE Object %u handleArraySize %u",
          theObject, req.handleArraySize);

    for (int i = 0 ; i < attribArraySize ; i++) {
        req.handleArray[i] = attribArray[i] ;
    }


    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE,
                      req.federate));

    e = rep->exception ;

    if (e == e_NO_EXCEPTION) {
        AttributeHandleSet *AttributeSet ;
        AttributeSet = new AttributeHandleSet(rep->handleArraySize);

        for (int i = 0 ; i < rep->handleArraySize ; i++) {
            AttributeSet->add(rep->handleArray[i]);
        }

        return(AttributeSet);
    }
    else
        return NULL ;
}

// ----------------------------------------------------------------------------
//! cancelAttributeOwnershipAcquisition.
void
OwnershipManagement::
cancelattributeOwnershipAcquisition(ObjectHandle theObject,
                                    std::vector <AttributeHandle> &attribArray,
                                    UShort attribArraySize,
                                    TypeException &e)
{
    NM_Cancel_Attribute_Ownership_Acquisition req;

    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArray.resize(attribArraySize) ;
    req.handleArraySize = attribArraySize ;

    for (int i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    D.Out(pdDebug, "CANCEL_ACQUISITION Federate %u ", fm->federate);

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION,
                      req.federate));

    e = rep->exception ;
}

// ----------------------------------------------------------------------------
//! informAttributeOwnership.
void
OwnershipManagement::informAttributeOwnership(ObjectHandle the_object,
                                              AttributeHandle the_attribute,
                                              FederateHandle the_owner,
                                              TypeException &)
{
    Message req;

    req.type = Message::INFORM_ATTRIBUTE_OWNERSHIP ;
    req.setObject(the_object);
    req.setAttribute(the_attribute);
    req.setFederate(the_owner);

    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
//! attributeIsNotOwned.
void
OwnershipManagement::attributeIsNotOwned(ObjectHandle the_object,
                                         AttributeHandle the_attribute,
                                         FederateHandle,
                                         TypeException &)
{
    Message req;

    req.type = Message::ATTRIBUTE_IS_NOT_OWNED ;
    req.setObject(the_object);
    req.setAttribute(the_attribute);
    // req.setFederate(the_owner);

    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
//! attributeOwnershipUnavailable.
void
OwnershipManagement::
attributeOwnershipUnavailable(ObjectHandle the_object,
                              std::vector <AttributeHandle> &the_attributes,
                              UShort the_size,
                              FederateHandle,
                              TypeException &)
{
    Message req;

    req.type = Message::ATTRIBUTE_OWNERSHIP_UNAVAILABLE ;
    req.setObject(the_object);
    req.setAttributes(the_attributes, the_size);

    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisitionNotification.
void
OwnershipManagement::
attributeOwnershipAcquisitionNotification(ObjectHandle the_object,
                                          std::vector <AttributeHandle> &the_attributes,
                                          UShort the_size,
                                          FederateHandle,
                                          TypeException &)
{
    Message req;

    req.type = Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION ;
    req.setObject(the_object);
    req.setAttributes(the_attributes, the_size);

    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
//! requestAttributeOwnershipAssumption.
void
OwnershipManagement::
requestAttributeOwnershipAssumption(ObjectHandle the_object,
                                    std::vector <AttributeHandle> &the_attributes,
                                    UShort the_size,
                                    FederateHandle,
                                    std::string the_tag,
                                    TypeException &)
{
    Message req;

    req.type = Message::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION ;
    req.setObject(the_object);
    req.setAttributes(the_attributes, the_size);
    req.setTag(the_tag);

    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
//! requestAttributeOwnershipRelease.
void
OwnershipManagement::
requestAttributeOwnershipRelease(ObjectHandle the_object,
                                 std::vector <AttributeHandle> &the_attributes,
                                 UShort the_size,
                                 std::string the_tag,
                                 TypeException &)
{
    Message req;

    req.type = Message::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE ;
    req.setObject(the_object);
    req.setAttributes(the_attributes, the_size);
    req.setTag(the_tag);

    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
//! attributeOwnershipDivestitureNotification.
void
OwnershipManagement::
attributeOwnershipDivestitureNotification(ObjectHandle the_object,
                                          std::vector <AttributeHandle> &the_attributes,
                                          UShort the_size,
                                          TypeException &)
{
    Message req;

    req.type = Message::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION ;
    req.setObject(the_object);
    req.setAttributes(the_attributes, the_size);

    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
//! confirmattributeOwnershipAcquisitionCancellation.
void
OwnershipManagement::
confirmAttributeOwnershipAcquisitionCancellation(ObjectHandle the_object,
                                                 std::vector <AttributeHandle> &the_attributes,
                                                 UShort the_size,
                                                 TypeException &)
{
    Message req;

    req.type = Message::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION ;
    req.setObject(the_object);
    req.setAttributes(the_attributes, the_size);

    comm->requestFederateService(&req);
}

}} // namespace certi/rtia

// $Id: OwnershipManagement.cc,v 3.17 2008/12/07 20:16:09 gotthardp Exp $
