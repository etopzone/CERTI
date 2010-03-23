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
// $Id: OwnershipManagement.cc,v 3.25 2010/03/23 13:13:27 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "OwnershipManagement.hh"

#include "PrettyDebug.hh"
#include "NM_Classes.hh"
#include "M_Classes.hh"

#include <memory>

namespace certi {
namespace rtia {

static PrettyDebug D("RTIA_OWM", "(RTIA OwM) ");

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

    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setObject(theObject);
    req.setAttribute(theAttribute);

    D.Out(pdDebug, "Federate %u ", fm->federate);

    comm->sendMessage(&req);

    // waitMessage is only used to verify transmission was OK.
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(
                      NetworkMessage::IS_ATTRIBUTE_OWNED_BY_FEDERATE,
                      req.getFederate()));

    e = rep->getException() ;

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

    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setObject(theObject);
    req.setAttribute(theAttribute);

    D.Out(pdDebug, "Federate %u ", fm->federate);

    comm->sendMessage(&req);

    //AttendreMsg ne sert que pour verifier que la transmission a ete OK
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::QUERY_ATTRIBUTE_OWNERSHIP, req.getFederate()));

    e = rep->getException() ;
}

// ----------------------------------------------------------------------------
//! negotiatedAttributeOwnershipDivestiture.
void
OwnershipManagement::
negotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                        const std::vector <AttributeHandle> &attribArray,
                                        uint32_t attribArraySize,
                                        const std::string& theTag,
                                        TypeException &e)

{
    NM_Negotiated_Attribute_Ownership_Divestiture req;

    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setObject(theObject);
    req.setAttributesSize(attribArraySize) ;

    for (uint32_t i = 0 ; i < attribArraySize ; i++)
        req.setAttributes(attribArray[i],i) ;

    req.setLabel(theTag);

    D.Out(pdDebug, "NEGOTIATED_DIVESTITURE Federate %u ", fm->federate);

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
                      req.getFederate()));

    e = rep->getException() ;
}

// ----------------------------------------------------------------------------
//! cancelNnegotiatedAttributeOwnershipDivestiture.
void
OwnershipManagement::
cancelnegotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                              const std::vector <AttributeHandle> &attribArray,
                                              uint32_t attribArraySize,
                                              TypeException &e)
{
    NM_Cancel_Negotiated_Attribute_Ownership_Divestiture req;

    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setObject(theObject);
    req.setAttributesSize(attribArraySize) ;

    for (uint32_t i = 0 ; i < attribArraySize ; i++)
        req.setAttributes(attribArray[i],i) ;

    D.Out(pdDebug, "CANCEL_NEGOTIATED_DIVESTITURE Federate %u ",
          fm->federate);

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(
                      NetworkMessage::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
                      req.getFederate()));

    e = rep->getException() ;
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisitionIfAvailable.
void
OwnershipManagement::
attributeOwnershipAcquisitionIfAvailable(ObjectHandle theObject,
                                         const std::vector <AttributeHandle> &attribArray,
                                         uint32_t attribArraySize,
                                         TypeException &e)
{
    NM_Attribute_Ownership_Acquisition_If_Available req;
    
    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setObject(theObject);
    req.setAttributesSize(attribArraySize) ;

    for (uint32_t i = 0 ; i < attribArraySize ; i++)
        req.setAttributes(attribArray[i],i) ;

    D.Out(pdDebug, "AcquisitionIfAvailable Federate %u ", fm->federate);

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE,
                      req.getFederate()));

    e = rep->getException() ;
}

// ----------------------------------------------------------------------------
//! unconditionalAttributeOwnershipDivestiture.
void
OwnershipManagement::
unconditionalAttributeOwnershipDivestiture(ObjectHandle theObject,
                                           const std::vector <AttributeHandle> &attribArray,
                                           uint32_t attribArraySize,
                                           TypeException &e)
{
    NM_Unconditional_Attribute_Ownership_Divestiture req;

    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setObject(theObject);
    req.setAttributesSize(attribArraySize) ;

    for (uint32_t i = 0 ; i < attribArraySize ; i++)
        req.setAttributes(attribArray[i],i) ;

    D.Out(pdDebug, "UNCONDITIONAL_DIVESTITURE Federate %u ",
          fm->federate);

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
                      req.getFederate()));

    e = rep->getException() ;
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisition.
void
OwnershipManagement::
attributeOwnershipAcquisition(ObjectHandle theObject,
                              const std::vector <AttributeHandle> &attribArray,
                              uint32_t attribArraySize,
                              const std::string& theTag,
                              TypeException &e)
{
    NM_Attribute_Ownership_Acquisition req;

    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setObject(theObject);
    req.setAttributesSize(attribArraySize) ;

    for (uint32_t i = 0 ; i < attribArraySize ; i++)
        req.setAttributes(attribArray[i],i) ;

    req.setLabel(theTag);

    D.Out(pdDebug, "OWNERSHIP_ACQUISITION Federate %u ", fm->federate);

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION,
                      req.getFederate()));

    e = rep->getException() ;
}

// ----------------------------------------------------------------------------
//! attributeOwnershipRealeaseResponse.
AttributeHandleSet*
OwnershipManagement::
attributeOwnershipRealeaseResponse(ObjectHandle theObject,
                                   const std::vector <AttributeHandle> &attribArray,
                                   uint32_t attribArraySize,
                                   TypeException &e)
{
    NM_Attribute_Ownership_Release_Response req;

    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setObject(theObject);
    req.setAttributesSize(attribArraySize) ;

    D.Out(pdDebug, "RELEASE_RESPONSE Object %u handleArraySize %u",
          theObject, req.getAttributesSize());

    for (uint32_t i = 0 ; i < attribArraySize ; i++) {
        req.setAttributes(attribArray[i],i) ;
    }


    comm->sendMessage(&req);

    std::auto_ptr<NM_Attribute_Ownership_Release_Response> rep(static_cast<NM_Attribute_Ownership_Release_Response*>(comm->waitMessage(NetworkMessage::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE,
                      req.getFederate())));

    e = rep->getException() ;

    if (e == e_NO_EXCEPTION) {
        AttributeHandleSet *AttributeSet ;
        AttributeSet = new AttributeHandleSet(rep->getAttributesSize());

        for (uint32_t i = 0 ; i < rep->getAttributesSize() ; i++) {
            AttributeSet->add(rep->getAttributes(i));
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
                                    const std::vector <AttributeHandle> &attribArray,
                                    uint32_t attribArraySize,
                                    TypeException &e)
{
    NM_Cancel_Attribute_Ownership_Acquisition req;

    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setObject(theObject);
    req.setAttributesSize(attribArraySize);

    for (uint32_t i = 0 ; i < attribArraySize ; i++)
        req.setAttributes(attribArray[i],i) ;

    D.Out(pdDebug, "CANCEL_ACQUISITION Federate %u ", fm->federate);

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION,
                      req.getFederate()));

    e = rep->getException() ;
}

// ----------------------------------------------------------------------------
//! informAttributeOwnership.
void
OwnershipManagement::informAttributeOwnership(ObjectHandle the_object,
                                              AttributeHandle the_attribute,
                                              FederateHandle the_owner,
                                              TypeException &)
{
    M_Inform_Attribute_Ownership req;

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
    M_Attribute_Is_Not_Owned req;

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
                              const std::vector <AttributeHandle> &the_attributes,
                              uint32_t the_size,
                              FederateHandle,
                              TypeException &)
{
    M_Attribute_Ownership_Unavailable req;

    req.setObject(the_object);
    req.setAttributesSize(the_size);
    for (uint32_t i=0;i<the_size;++i) {
        req.setAttributes(the_attributes[i], i);
    }

    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisitionNotification.
void
OwnershipManagement::
attributeOwnershipAcquisitionNotification(ObjectHandle the_object,
                                          const std::vector <AttributeHandle> &the_attributes,
                                          uint32_t the_size,
                                          FederateHandle,
                                          TypeException &)
{
    M_Attribute_Ownership_Acquisition_Notification req;

    req.setObject(the_object);
    req.setAttributesSize(the_size);
    for (uint32_t i=0;i<the_size;++i) {
    	req.setAttributes(the_attributes[i], i);
    }

    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
//! requestAttributeOwnershipAssumption.
void
OwnershipManagement::
requestAttributeOwnershipAssumption(ObjectHandle the_object,
                                    const std::vector <AttributeHandle> &the_attributes,
                                    uint32_t the_size,
                                    FederateHandle,
                                    const std::string& the_tag,
                                    TypeException &)
{
    M_Request_Attribute_Ownership_Assumption req;

    req.setObject(the_object);
    req.setTag(the_tag);
    req.setAttributesSize(the_size);
    for (uint32_t i=0;i<the_size;++i) {
    	req.setAttributes(the_attributes[i], i);
    }
    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
//! requestAttributeOwnershipRelease.
void
OwnershipManagement::
requestAttributeOwnershipRelease(ObjectHandle the_object,
                                 const std::vector <AttributeHandle> &the_attributes,
                                 uint32_t the_size,
                                 const std::string& the_tag,
                                 TypeException &)
{
    M_Request_Attribute_Ownership_Release req;

    req.setObject(the_object);
    req.setTag(the_tag);
    req.setAttributesSize(the_size);
    for (uint32_t i=0;i<the_size;++i) {
    	req.setAttributes(the_attributes[i], i);
    }
    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
//! attributeOwnershipDivestitureNotification.
void
OwnershipManagement::
attributeOwnershipDivestitureNotification(ObjectHandle the_object,
                                          const std::vector <AttributeHandle> &the_attributes,
                                          uint32_t the_size,
                                          TypeException &)
{
    M_Attribute_Ownership_Divestiture_Notification req;

    req.setObject(the_object);
    req.setAttributesSize(the_size);
    for (uint32_t i=0;i<the_size;++i) {
    	req.setAttributes(the_attributes[i], i);
    }
    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
//! confirmattributeOwnershipAcquisitionCancellation.
void
OwnershipManagement::
confirmAttributeOwnershipAcquisitionCancellation(ObjectHandle the_object,
                                                 const std::vector <AttributeHandle> &the_attributes,
                                                 uint32_t the_size,
                                                 TypeException &)
{
    M_Confirm_Attribute_Ownership_Acquisition_Cancellation req;

    req.setObject(the_object);
    req.setAttributesSize(the_size);
    for (uint32_t i=0;i<the_size;++i) {
    	req.setAttributes(the_attributes[i], i);
    }
    comm->requestFederateService(&req);
}

}} // namespace certi/rtia

// $Id: OwnershipManagement.cc,v 3.25 2010/03/23 13:13:27 erk Exp $
