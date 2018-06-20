// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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

#include "OwnershipManagement.hh"

#include <memory>

#include <config.h>

#include <libCERTI/M_Classes.hh>
#include <libCERTI/NM_Classes.hh>
#include <libCERTI/PrettyDebug.hh>

namespace certi {
namespace rtia {

static PrettyDebug D("RTIA_OWM", "(RTIA OwM) ");

OwnershipManagement::OwnershipManagement(Communications* GC, FederationManagement* GF)
{
    comm = GC;
    fm = GF;
}

OwnershipManagement::~OwnershipManagement()
{
}

std::string
OwnershipManagement::attributeOwnedByFederate(ObjectHandle theObject, AttributeHandle theAttribute, Exception::Type& e)
{
    NM_Is_Attribute_Owned_By_Federate req;

    Debug(D, pdDebug) << "IS_ATTRIBUTE_OWNED_BY_FEDERATE, attribute " << theAttribute << " of object " << theObject
                      << std::endl;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setObject(theObject);
    req.setAttribute(theAttribute);

    Debug(D, pdDebug) << "Federate " << fm->getFederateHandle() << std::endl;

    comm->sendMessage(&req);

    // waitMessage is only used to verify transmission was OK.
    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::IS_ATTRIBUTE_OWNED_BY_FEDERATE, req.getFederate()));

    e = rep->getException();

    if (e == Exception::Type::NO_EXCEPTION) {
        return rep->getLabel();
    }
    else {
        return "";
    }
}

void OwnershipManagement::queryAttributeOwnership(ObjectHandle theObject,
                                                  AttributeHandle theAttribute,
                                                  Exception::Type& e)
{
    NM_Query_Attribute_Ownership req;

    Debug(D, pdDebug) << "QUERY_ATTRIBUTE_OWNERSHIP, attribute " << theAttribute << "from object " << theObject
                      << std::endl;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setObject(theObject);
    req.setAttribute(theAttribute);

    Debug(D, pdDebug) << "Federate " << fm->getFederateHandle() << std::endl;

    comm->sendMessage(&req);

    //AttendreMsg ne sert que pour verifier que la transmission a ete OK
    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::QUERY_ATTRIBUTE_OWNERSHIP, req.getFederate()));

    e = rep->getException();
}

void OwnershipManagement::negotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                                                  const std::vector<AttributeHandle>& attribArray,
                                                                  uint32_t attribArraySize,
                                                                  const std::string& theTag,
                                                                  Exception::Type& e)

{
    NM_Negotiated_Attribute_Ownership_Divestiture req;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setObject(theObject);
    req.setAttributesSize(attribArraySize);

    for (uint32_t i = 0; i < attribArraySize; i++)
        req.setAttributes(attribArray[i], i);

    req.setLabel(theTag);

    Debug(D, pdDebug) << "NEGOTIATED_DIVESTITURE Federate " << fm->getFederateHandle() << std::endl;

    comm->sendMessage(&req);

    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE, req.getFederate()));

    e = rep->getException();
}

void OwnershipManagement::cancelnegotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                                                        const std::vector<AttributeHandle>& attribArray,
                                                                        uint32_t attribArraySize,
                                                                        Exception::Type& e)
{
    NM_Cancel_Negotiated_Attribute_Ownership_Divestiture req;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setObject(theObject);
    req.setAttributesSize(attribArraySize);

    for (uint32_t i = 0; i < attribArraySize; i++)
        req.setAttributes(attribArray[i], i);

    Debug(D, pdDebug) << "CANCEL_NEGOTIATED_DIVESTITURE Federate " << fm->getFederateHandle() << std::endl;

    comm->sendMessage(&req);

    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE, req.getFederate()));

    e = rep->getException();
}

void OwnershipManagement::attributeOwnershipAcquisitionIfAvailable(ObjectHandle theObject,
                                                                   const std::vector<AttributeHandle>& attribArray,
                                                                   uint32_t attribArraySize,
                                                                   Exception::Type& e)
{
    NM_Attribute_Ownership_Acquisition_If_Available req;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setObject(theObject);
    req.setAttributesSize(attribArraySize);

    for (uint32_t i = 0; i < attribArraySize; i++)
        req.setAttributes(attribArray[i], i);

    Debug(D, pdDebug) << "AcquisitionIfAvailable Federate " << fm->getFederateHandle() << std::endl;

    comm->sendMessage(&req);

    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE, req.getFederate()));

    e = rep->getException();
}

void OwnershipManagement::unconditionalAttributeOwnershipDivestiture(ObjectHandle theObject,
                                                                     const std::vector<AttributeHandle>& attribArray,
                                                                     uint32_t attribArraySize,
                                                                     Exception::Type& e)
{
    NM_Unconditional_Attribute_Ownership_Divestiture req;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setObject(theObject);
    req.setAttributesSize(attribArraySize);

    for (uint32_t i = 0; i < attribArraySize; i++)
        req.setAttributes(attribArray[i], i);

    Debug(D, pdDebug) << "UNCONDITIONAL_DIVESTITURE Federate " << fm->getFederateHandle() << std::endl;

    comm->sendMessage(&req);

    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE, req.getFederate()));

    e = rep->getException();
}

void OwnershipManagement::attributeOwnershipAcquisition(ObjectHandle theObject,
                                                        const std::vector<AttributeHandle>& attribArray,
                                                        uint32_t attribArraySize,
                                                        const std::string& theTag,
                                                        Exception::Type& e)
{
    NM_Attribute_Ownership_Acquisition req;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setObject(theObject);
    req.setAttributesSize(attribArraySize);

    for (uint32_t i = 0; i < attribArraySize; i++)
        req.setAttributes(attribArray[i], i);

    req.setLabel(theTag);

    Debug(D, pdDebug) << "OWNERSHIP_ACQUISITION Federate " << fm->getFederateHandle() << std::endl;

    comm->sendMessage(&req);

    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_ACQUISITION, req.getFederate()));

    e = rep->getException();
}

AttributeHandleSet*
OwnershipManagement::attributeOwnershipRealeaseResponse(ObjectHandle theObject,
                                                        const std::vector<AttributeHandle>& attribArray,
                                                        uint32_t attribArraySize,
                                                        Exception::Type& e)
{
    NM_Attribute_Ownership_Release_Response req;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setObject(theObject);
    req.setAttributesSize(attribArraySize);

    Debug(D, pdDebug) << "RELEASE_RESPONSE Object " << theObject << " handleArraySize " << req.getAttributesSize()
                      << std::endl;

    for (uint32_t i = 0; i < attribArraySize; i++) {
        req.setAttributes(attribArray[i], i);
    }

    comm->sendMessage(&req);

    std::unique_ptr<NM_Attribute_Ownership_Release_Response> rep(static_cast<NM_Attribute_Ownership_Release_Response*>(
        comm->waitMessage(NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE, req.getFederate())));

    e = rep->getException();

    if (e == Exception::Type::NO_EXCEPTION) {
        AttributeHandleSet* AttributeSet;
        AttributeSet = new AttributeHandleSet(rep->getAttributesSize());

        for (uint32_t i = 0; i < rep->getAttributesSize(); i++) {
            AttributeSet->add(rep->getAttributes(i));
        }

        return (AttributeSet);
    }
    else
        return NULL;
}

void OwnershipManagement::cancelattributeOwnershipAcquisition(ObjectHandle theObject,
                                                              const std::vector<AttributeHandle>& attribArray,
                                                              uint32_t attribArraySize,
                                                              Exception::Type& e)
{
    NM_Cancel_Attribute_Ownership_Acquisition req;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setObject(theObject);
    req.setAttributesSize(attribArraySize);

    for (uint32_t i = 0; i < attribArraySize; i++)
        req.setAttributes(attribArray[i], i);

    Debug(D, pdDebug) << "CANCEL_ACQUISITION Federate " << fm->getFederateHandle() << std::endl;

    comm->sendMessage(&req);

    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION, req.getFederate()));

    e = rep->getException();
}

void OwnershipManagement::informAttributeOwnership(ObjectHandle the_object,
                                                   AttributeHandle the_attribute,
                                                   FederateHandle the_owner,
                                                   Exception::Type& /*e*/)
{
    M_Inform_Attribute_Ownership req;

    req.setObject(the_object);
    req.setAttribute(the_attribute);
    req.setFederate(the_owner);

    comm->requestFederateService(&req);
}

void OwnershipManagement::attributeIsNotOwned(ObjectHandle the_object,
                                              AttributeHandle the_attribute,
                                              FederateHandle,
                                              Exception::Type& /*e*/)
{
    M_Attribute_Is_Not_Owned req;

    req.setObject(the_object);
    req.setAttribute(the_attribute);
    // req.setFederate(the_owner);

    comm->requestFederateService(&req);
}

void OwnershipManagement::attributeOwnershipUnavailable(ObjectHandle the_object,
                                                        const std::vector<AttributeHandle>& the_attributes,
                                                        uint32_t the_size,
                                                        FederateHandle,
                                                        Exception::Type& /*e*/)
{
    M_Attribute_Ownership_Unavailable req;

    req.setObject(the_object);
    req.setAttributesSize(the_size);
    for (uint32_t i = 0; i < the_size; ++i) {
        req.setAttributes(the_attributes[i], i);
    }

    comm->requestFederateService(&req);
}

void OwnershipManagement::attributeOwnershipAcquisitionNotification(ObjectHandle the_object,
                                                                    const std::vector<AttributeHandle>& the_attributes,
                                                                    uint32_t the_size,
                                                                    FederateHandle,
                                                                    Exception::Type& /*e*/)
{
    M_Attribute_Ownership_Acquisition_Notification req;

    req.setObject(the_object);
    req.setAttributesSize(the_size);
    for (uint32_t i = 0; i < the_size; ++i) {
        req.setAttributes(the_attributes[i], i);
    }

    comm->requestFederateService(&req);
}

void OwnershipManagement::requestAttributeOwnershipAssumption(ObjectHandle the_object,
                                                              const std::vector<AttributeHandle>& the_attributes,
                                                              uint32_t the_size,
                                                              FederateHandle,
                                                              const std::string& the_tag,
                                                              Exception::Type& /*e*/)
{
    M_Request_Attribute_Ownership_Assumption req;

    req.setObject(the_object);
    req.setTag(the_tag);
    req.setAttributesSize(the_size);
    for (uint32_t i = 0; i < the_size; ++i) {
        req.setAttributes(the_attributes[i], i);
    }
    comm->requestFederateService(&req);
}

void OwnershipManagement::requestAttributeOwnershipRelease(ObjectHandle the_object,
                                                           const std::vector<AttributeHandle>& the_attributes,
                                                           uint32_t the_size,
                                                           const std::string& the_tag,
                                                           Exception::Type& /*e*/)
{
    M_Request_Attribute_Ownership_Release req;

    req.setObject(the_object);
    req.setTag(the_tag);
    req.setAttributesSize(the_size);
    for (uint32_t i = 0; i < the_size; ++i) {
        req.setAttributes(the_attributes[i], i);
    }
    comm->requestFederateService(&req);
}

void OwnershipManagement::attributeOwnershipDivestitureNotification(ObjectHandle the_object,
                                                                    const std::vector<AttributeHandle>& the_attributes,
                                                                    uint32_t the_size,
                                                                    Exception::Type& /*e*/)
{
    M_Attribute_Ownership_Divestiture_Notification req;

    req.setObject(the_object);
    req.setAttributesSize(the_size);
    for (uint32_t i = 0; i < the_size; ++i) {
        req.setAttributes(the_attributes[i], i);
    }
    comm->requestFederateService(&req);
}

void OwnershipManagement::confirmAttributeOwnershipAcquisitionCancellation(
    ObjectHandle the_object,
    const std::vector<AttributeHandle>& the_attributes,
    uint32_t the_size,
    Exception::Type& /*e*/)
{
    M_Confirm_Attribute_Ownership_Acquisition_Cancellation req;

    req.setObject(the_object);
    req.setAttributesSize(the_size);
    for (uint32_t i = 0; i < the_size; ++i) {
        req.setAttributes(the_attributes[i], i);
    }
    comm->requestFederateService(&req);
}
}
} // namespace certi/rtia
