// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
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
// $Id: OwnershipManagement.cc,v 3.6 2003/05/09 00:27:17 breholee Exp $
// ----------------------------------------------------------------------------

#include "OwnershipManagement.hh"

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
    NetworkMessage req, rep ;

    D.Out(pdDebug,
          "IS_ATTRIBUTE_OWNED_BY_FEDERATE, attribute %u of object %u",
          theAttribute, theObject);

    req.type = m_IS_ATTRIBUTE_OWNED_BY_FEDERATE ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArray[0] = theAttribute ;
    req.handleArraySize = 1 ;

    D.Out(pdDebug, "Federate %u ", fm->federate);

    comm->sendMessage(&req);

    // waitMessage is only used to verify transmission was OK.
    comm->waitMessage(&rep,
                      m_IS_ATTRIBUTE_OWNED_BY_FEDERATE,
                      req.federate);

    e = rep.exception ;

    if (e == e_NO_EXCEPTION)
        return(rep.label);
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
    NetworkMessage req, rep ;

    D.Out(pdDebug,
          "QUERY_ATTRIBUTE_OWNERSHIP, attribute %u from object %u",
          theAttribute, theObject);

    req.type = m_QUERY_ATTRIBUTE_OWNERSHIP ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArray[0] = theAttribute ;
    req.handleArraySize = 1 ;

    D.Out(pdDebug, "Federate %u ", fm->federate);

    comm->sendMessage(&req);

    //AttendreMsg ne sert que pour verifier que la transmission a ete OK
    comm->waitMessage(&rep, m_QUERY_ATTRIBUTE_OWNERSHIP, req.federate);

    e = rep.exception ;
}

// ----------------------------------------------------------------------------
//! negotiatedAttributeOwnershipDivestiture.
void
OwnershipManagement::
negotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                        AttributeHandle *attribArray,
                                        UShort attribArraySize,
                                        const char *theTag,
                                        TypeException &e)

{
    NetworkMessage req, rep ;

    req.type = m_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArraySize = attribArraySize ;

    for (int i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    strcpy(req.label, theTag);

    D.Out(pdDebug, "NEGOTIATED_DIVESTITURE Federate %u ", fm->federate);

    comm->sendMessage(&req);

    comm->waitMessage(&rep, m_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
                      req.federate);

    e = rep.exception ;
}

// ----------------------------------------------------------------------------
//! cancelNnegotiatedAttributeOwnershipDivestiture.
void
OwnershipManagement::
cancelnegotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                              AttributeHandle *attribArray,
                                              UShort attribArraySize,
                                              TypeException &e)
{
    NetworkMessage req, rep ;

    req.type = m_CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArraySize = attribArraySize ;

    for (int i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    D.Out(pdDebug, "CANCEL_NEGOTIATED_DIVESTITURE Federate %u ",
          fm->federate);

    comm->sendMessage(&req);

    comm->waitMessage(&rep,
                      m_CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
                      req.federate);

    e = rep.exception ;
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisitionIfAvailable.
void
OwnershipManagement::
attributeOwnershipAcquisitionIfAvailable(ObjectHandle theObject,
                                         AttributeHandle *attribArray,
                                         UShort attribArraySize,
                                         TypeException &e)
{
    NetworkMessage req, rep ;

    req.type = m_ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArraySize = attribArraySize ;

    for (int i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    D.Out(pdDebug, "AcquisitionIfAvailable Federate %u ", fm->federate);

    comm->sendMessage(&req);

    comm->waitMessage(&rep, m_ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE,
                      req.federate);

    e = rep.exception ;
}

// ----------------------------------------------------------------------------
//! unconditionalAttributeOwnershipDivestiture.
void
OwnershipManagement::
unconditionalAttributeOwnershipDivestiture(ObjectHandle theObject,
                                           AttributeHandle *attribArray,
                                           UShort attribArraySize,
                                           TypeException &e)
{
    NetworkMessage req, rep ;

    req.type = m_UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArraySize = attribArraySize ;

    for (int i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    D.Out(pdDebug, "UNCONDITIONAL_DIVESTITURE Federate %u ",
          fm->federate);

    comm->sendMessage(&req);

    comm->waitMessage(&rep, m_UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
                      req.federate);

    e = rep.exception ;
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisition.
void
OwnershipManagement::
attributeOwnershipAcquisition(ObjectHandle theObject,
                              AttributeHandle *attribArray,
                              UShort attribArraySize,
                              const char *theTag,
                              TypeException &e)
{
    NetworkMessage req, rep ;

    req.type = m_ATTRIBUTE_OWNERSHIP_ACQUISITION ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArraySize = attribArraySize ;

    for (int i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    strcpy(req.label, theTag);

    D.Out(pdDebug, "OWNERSHIP_ACQUISITION Federate %u ", fm->federate);

    comm->sendMessage(&req);

    comm->waitMessage(&rep, m_ATTRIBUTE_OWNERSHIP_ACQUISITION,
                      req.federate);

    e = rep.exception ;
}

// ----------------------------------------------------------------------------
//! attributeOwnershipRealeaseResponse.
AttributeHandleSet*
OwnershipManagement::
attributeOwnershipRealeaseResponse(ObjectHandle theObject,
                                   AttributeHandle *attribArray,
                                   UShort attribArraySize,
                                   TypeException &e)
{
    NetworkMessage req, rep ;

    req.type = m_ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArraySize = attribArraySize ;

    D.Out(pdDebug, "RELEASE_RESPONSE Object %u handleArraySize %u",
          theObject, req.handleArraySize);

    for (int i = 0 ; i < attribArraySize ; i++) {
        req.handleArray[i] = attribArray[i] ;
    }


    comm->sendMessage(&req);

    comm->waitMessage(&rep, m_ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE,
                      req.federate);

    e = rep.exception ;

    if (e == e_NO_EXCEPTION) {
        AttributeHandleSet *AttributeSet ;
        AttributeSet=AttributeHandleSetFactory::create(rep.handleArraySize);

        for (int i = 0 ; i < rep.handleArraySize ; i++) {
            AttributeSet->add(rep.handleArray[i]);
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
                                    AttributeHandle *attribArray,
                                    UShort attribArraySize,
                                    TypeException &e)
{
    NetworkMessage req, rep ;

    req.type = m_CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = theObject ;
    req.handleArraySize = attribArraySize ;

    for (int i = 0 ; i < attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    D.Out(pdDebug, "CANCEL_ACQUISITION Federate %u ", fm->federate);

    comm->sendMessage(&req);

    comm->waitMessage(&rep, m_CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION,
                      req.federate);

    e = rep.exception ;
}

// ----------------------------------------------------------------------------
//! informAttributeOwnership.
void
OwnershipManagement::informAttributeOwnership(ObjectHandle the_object,
                                              AttributeHandle the_attribute,
                                              FederateHandle the_owner,
                                              TypeException &)
{
    Message req, rep ;

    req.type = INFORM_ATTRIBUTE_OWNERSHIP ;
    req.setObject(the_object);
    req.setAttribute(the_attribute);
    req.setFederate(the_owner);

    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! attributeIsNotOwned.
void
OwnershipManagement::attributeIsNotOwned(ObjectHandle the_object,
                                         AttributeHandle the_attribute,
                                         FederateHandle,
                                         TypeException &)
{
    Message req, rep ;

    req.type = ATTRIBUTE_IS_NOT_OWNED ;
    req.setObject(the_object);
    req.setAttribute(the_attribute);
    // req.setFederate(the_owner);

    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! attributeOwnershipUnavailable.
void
OwnershipManagement::
attributeOwnershipUnavailable(ObjectHandle the_object,
                              AttributeHandle *the_attributes,
                              UShort the_size,
                              FederateHandle,
                              TypeException &)
{
    Message req, rep ;

    req.type = ATTRIBUTE_OWNERSHIP_UNAVAILABLE ;
    req.setObject(the_object);
    req.setAttributes(the_attributes, the_size);

    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisitionNotification.
void
OwnershipManagement::
attributeOwnershipAcquisitionNotification(ObjectHandle the_object,
                                          AttributeHandle *the_attributes,
                                          UShort the_size,
                                          FederateHandle,
                                          TypeException &)
{
    Message req, rep ;

    req.type = ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION ;
    req.setObject(the_object);
    req.setAttributes(the_attributes, the_size);

    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! requestAttributeOwnershipAssumption.
void
OwnershipManagement::
requestAttributeOwnershipAssumption(ObjectHandle the_object,
                                    AttributeHandle *the_attributes,
                                    UShort the_size,
                                    FederateHandle,
                                    char *the_tag,
                                    TypeException &)
{
    Message req, rep ;

    req.type = REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION ;
    req.setObject(the_object);
    req.setAttributes(the_attributes, the_size);
    req.setTag(the_tag);

    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! requestAttributeOwnershipRelease.
void
OwnershipManagement::
requestAttributeOwnershipRelease(ObjectHandle the_object,
                                 AttributeHandle *the_attributes,
                                 UShort the_size,
                                 char *the_tag,
                                 TypeException &)
{
    Message req, rep ;

    req.type = REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE ;
    req.setObject(the_object);
    req.setAttributes(the_attributes, the_size);
    req.setTag(the_tag);

    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! attributeOwnershipDivestitureNotification.
void
OwnershipManagement::
attributeOwnershipDivestitureNotification(ObjectHandle the_object,
                                          AttributeHandle *the_attributes,
                                          UShort the_size,
                                          TypeException &)
{
    Message req, rep ;

    req.type = ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION ;
    req.setObject(the_object);
    req.setAttributes(the_attributes, the_size);

    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
//! confirmattributeOwnershipAcquisitionCancellation.
void
OwnershipManagement::
confirmAttributeOwnershipAcquisitionCancellation(ObjectHandle the_object,
                                                 AttributeHandle *the_attributes,
                                                 UShort the_size,
                                                 TypeException &)
{
    Message req ;

    req.type = CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION ;
    req.setObject(the_object);
    req.setAttributes(the_attributes, the_size);

    Message rep ;
    comm->requestFederateService(&req, &rep);
}

}} // namespace certi/rtia

// $Id: OwnershipManagement.cc,v 3.6 2003/05/09 00:27:17 breholee Exp $
