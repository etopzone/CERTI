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
// $Id: DeclarationManagement.cc,v 3.8 2003/06/07 22:24:12 breholee Exp $
// ----------------------------------------------------------------------------

#include "DeclarationManagement.hh"

namespace certi {
namespace rtia {

static pdCDebug D("RTIA_DM", "(RTIA DM) ");

// ----------------------------------------------------------------------------
//! DeclarationManagement
DeclarationManagement::DeclarationManagement(Communications *GC,
                                             FederationManagement *GF,
                                             RootObject *theRootObj)
{
    comm = GC ;
    fm = GF ;
    rootObject = theRootObj ;
}

// ----------------------------------------------------------------------------
// ~DeclarationManagement
DeclarationManagement::~DeclarationManagement()
{
}

// ----------------------------------------------------------------------------
// publishObjectClass
void
DeclarationManagement::publishObjectClass(ObjectClassHandle theClassHandle,
                                          AttributeHandle *attribArray,
                                          UShort attribArraySize,
                                          TypeException &e)
{
    e = e_NO_EXCEPTION ;

    // Partie Locale

    try {
        rootObject->ObjectClasses->publish(fm->federate,
                                           theClassHandle,
                                           attribArray,
                                           attribArraySize,
                                           RTI_TRUE);
    }
    catch (Exception *e) {
        D.Out(pdExcept, "Exception catched in PublishObjectClass.");
        throw e ;
    }

    // Partie RTIG
    NetworkMessage req ;
    req.type = NetworkMessage::PUBLISH_OBJECT_CLASS ;
    req.objectClass = theClassHandle ;
    req.handleArraySize = attribArraySize ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;

    for (int i=0 ; i<attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    // Emission
    comm->sendMessage(&req);

    // Reception
    NetworkMessage rep ;
    comm->waitMessage(&rep, NetworkMessage::PUBLISH_OBJECT_CLASS, req.federate);

    e = rep.exception ;
}

// ----------------------------------------------------------------------------
// unpublishObjectClass
void
DeclarationManagement::unpublishObjectClass(ObjectClassHandle theClassHandle,
                                            TypeException &e)
{
    // Variables leurres
    AttributeHandle *attribArray = NULL ;
    UShort attribArraySize = 0 ;

    e = e_NO_EXCEPTION ;

    // Partie Locale

    try {
        rootObject->ObjectClasses->publish(fm->federate,
                                           theClassHandle,
                                           attribArray,
                                           attribArraySize,
                                           RTI_FALSE);
    } catch (Exception *e) {
        D.Out(pdExcept, "Exception catched in UnpublishObjectClass.");
        throw e ;
    }

    // Partie RTIG
    NetworkMessage req ;
    req.type = NetworkMessage::UNPUBLISH_OBJECT_CLASS ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.objectClass = theClassHandle ;

    // Emission de la requete vers le RTIG
    comm->sendMessage(&req);

    // On attend une reponse
    NetworkMessage rep ;
    comm->waitMessage(&rep, NetworkMessage::UNPUBLISH_OBJECT_CLASS, req.federate);

    e = rep.exception ;
}

// ----------------------------------------------------------------------------
// publishInteractionClass
void
DeclarationManagement::
publishInteractionClass(InteractionClassHandle theInteractionHandle,
                        TypeException &e)
{
    e = e_NO_EXCEPTION ;

    // Partie Locale

    try {
        rootObject->Interactions->publish(fm->federate,
                                          theInteractionHandle,
                                          RTI_TRUE);
    } catch (Exception *e) {
        D.Out(pdExcept, "Exception catched in publishInteractionClass.");
        throw e ;
    }

    // Partie RTIG
    NetworkMessage req ;
    req.type = NetworkMessage::PUBLISH_INTERACTION_CLASS ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.interactionClass = theInteractionHandle ;

    comm->sendMessage(&req);

    NetworkMessage rep ;
    comm->waitMessage(&rep, NetworkMessage::PUBLISH_INTERACTION_CLASS, req.federate);

    e = rep.exception ;
}

// ----------------------------------------------------------------------------
// unpublishInteractionClass
void
DeclarationManagement::
unpublishInteractionClass(InteractionClassHandle theInteractionHandle,
                          TypeException &e)
{
    e = e_NO_EXCEPTION ;

    // Partie Locale

    try {
        rootObject->Interactions->publish(fm->federate,
                                          theInteractionHandle,
                                          RTI_FALSE);
    } catch (Exception *e) {
        D.Out(pdExcept, "Exception catched in UnpublishInteractionClass.");
        throw e ;
    }

    // Partie RTIG
    NetworkMessage req ;
    req.type = NetworkMessage::UNPUBLISH_INTERACTION_CLASS ;
    req.interactionClass = theInteractionHandle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;

    comm->sendMessage(&req);

    NetworkMessage rep ;
    comm->waitMessage(&rep, NetworkMessage::UNPUBLISH_INTERACTION_CLASS, req.federate);

    e = rep.exception ;
}

// ----------------------------------------------------------------------------
// subscribeObjectClassAttribute
void
DeclarationManagement::
subscribeObjectClassAttribute(ObjectClassHandle theClassHandle,
                              AttributeHandle *attribArray,
                              UShort attribArraySize,
                              TypeException &e)
{
    NetworkMessage req, rep ;

    // Pas de partie locale pour les abonnements

    // Partie RTIG

    req.type = NetworkMessage::SUBSCRIBE_OBJECT_CLASS ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.objectClass = theClassHandle ;
    req.handleArraySize = attribArraySize ;

    for (int i=0 ; i<attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    // Emission
    comm->sendMessage(&req);

    // Reception
    comm->waitMessage(&rep,
                      NetworkMessage::SUBSCRIBE_OBJECT_CLASS,
                      req.federate);

    e = rep.exception ;
}

// ----------------------------------------------------------------------------
// subscribeObjectClassAttribute (with Region)
void
DeclarationManagement::subscribeObjectClassAttribute(ObjectClassHandle,
                                                     AttributeHandle,
                                                     HLA_Region,
                                                     TypeException &e)
{
    // BUG: Not implemented in F.0
    e = e_UnimplementedService ;
}

// ----------------------------------------------------------------------------
// unsubscribeObjectClassAttribute
void
DeclarationManagement::
unsubscribeObjectClassAttribute(ObjectClassHandle theClassHandle,
                                TypeException &e)
{
    NetworkMessage req, rep ;

    e = e_NO_EXCEPTION ;

    // Pas de Partie Locale pour les abonnements

    // Partie RTIG
    req.type = NetworkMessage::UNSUBSCRIBE_OBJECT_CLASS ;
    req.objectClass = theClassHandle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;

    comm->sendMessage(&req);

    comm->waitMessage(&rep,
                      NetworkMessage::UNSUBSCRIBE_OBJECT_CLASS,
                      req.federate);

    e = rep.exception ;
}

// ----------------------------------------------------------------------------
// unsubscribeObjectClassAttribute (with Region)
void
DeclarationManagement::unsubscribeObjectClassAttribute(ObjectClassHandle,
                                                       HLA_Region,
                                                       TypeException &e)
{
    e = e_UnimplementedService ;
}

// ----------------------------------------------------------------------------
// subscribeInteractionClass
void
DeclarationManagement::
subscribeInteractionClass(InteractionClassHandle theClassHandle,
                          TypeException &e)
{
    NetworkMessage req, rep ;

    e = e_NO_EXCEPTION ;

    // Partie Locale

    // BUG: Pourquoi il y a-t-il une partie locale pour un abonnement ?
    // Ca ne va pas marcher avec les niveaux de securite !!!!

    try {
        rootObject->Interactions->subscribe(fm->federate,
                                            theClassHandle,
                                            RTI_TRUE);
    } catch (Exception *e) {
        D.Out(pdExcept, "Exception catched in subscribeInteractionClass.");
        throw e ;
    }

    // Partie RTIG

    req.type = NetworkMessage::SUBSCRIBE_INTERACTION_CLASS ;
    req.interactionClass = theClassHandle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;

    comm->sendMessage(&req);

    comm->waitMessage(&rep, NetworkMessage::SUBSCRIBE_INTERACTION_CLASS, req.federate);

    e = rep.exception ;
}

// ----------------------------------------------------------------------------
// subscribeInteractionClass (with Region)
void
DeclarationManagement::subscribeInteractionClass(InteractionClassHandle,
                                                 HLA_Region,
                                                 TypeException &e)
{
    e = e_UnimplementedService ;
}

// ----------------------------------------------------------------------------
// unsubscribeInteractionClass
void
DeclarationManagement::
unsubscribeInteractionClass(InteractionClassHandle theClassHandle,
                            TypeException &e)
{
    NetworkMessage req, rep ;

    e = e_NO_EXCEPTION ;

    // Partie Locale

    // BUG: Pourquoi il y a-t-il une partie locale pour un abonnement ?
    // Ca ne va pas marcher avec les niveaux de securite !!!!

    try {
        rootObject->Interactions->subscribe(fm->federate,
                                            theClassHandle,
                                            RTI_FALSE);
    } catch (Exception *e) {
        D.Out(pdExcept, "Exception catched in subscribeInteractionClass.");
        throw e ;
    }

    // Partie RTIG

    req.type = NetworkMessage::UNSUBSCRIBE_INTERACTION_CLASS ;
    req.interactionClass = theClassHandle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;

    comm->sendMessage(&req);

    comm->waitMessage(&rep,
                      NetworkMessage::UNSUBSCRIBE_INTERACTION_CLASS,
                      req.federate);

    e = rep.exception ;
}

// ----------------------------------------------------------------------------
// unsubscribeInteractionClass (with Region)
void
DeclarationManagement::unsubscribeInteractionClass(InteractionClassHandle,
                                                   HLA_Region,
                                                   TypeException &e)
{
    e = e_UnimplementedService ;
}

// ----------------------------------------------------------------------------
// startRegistrationForObjectClass
void
DeclarationManagement::
startRegistrationForObjectClass(ObjectClassHandle the_class,
                                // CAttributeHandleValuePairSet &theAttributes,
                                TypeException &e)
{
    Message req, rep ;

    // Pas de partie locale

    // Partie Federe

    req.type = Message::START_REGISTRATION_FOR_OBJECT_CLASS ;
    req.setObjectClass(the_class);

    comm->sendUN(&req);

    comm->receiveUN(&rep);

    if (rep.type != req.type) {
        D.Out(pdExcept, "Unknown response type when waiting for "
              "START_REGISTRATION_FOR_OBJECT_CLASS.");
        throw RTIinternalError();
    }

    e = rep.getExceptionType();
}

// ----------------------------------------------------------------------------
// stopRegistrationForObjectClass
void
DeclarationManagement::
stopRegistrationForObjectClass(ObjectClassHandle the_class,
                               TypeException &e)
{
    Message req, rep ;

    // Pas de partie Locale

    // Partie Federe

    req.type = Message::STOP_REGISTRATION_FOR_OBJECT_CLASS ;
    req.setObjectClass(the_class);

    comm->sendUN(&req);

    comm->receiveUN(&rep);

    if (rep.type != req.type) {
        D.Out(pdExcept, "Unknown response type when waiting for "
              "START_REGISTRATION_FOR_OBJECT_CLASS.");
        throw RTIinternalError();
    }

    e = rep.getExceptionType();
}

// ----------------------------------------------------------------------------
// turnInteractionsOn
void
DeclarationManagement::turnInteractionsOn(InteractionClassHandle interaction,
                                          TypeException &e)
{
    Message req, rep ;
    req.type = Message::TURN_INTERACTIONS_ON ;
    req.setInteractionClass(interaction);
    comm->sendUN(&req);
    comm->receiveUN(&rep);

    if (rep.type != req.type) {
        D.Out(pdExcept,
              "Unknown response type, expecting TURN_INTERACTIONS_ON.");
        throw RTIinternalError();
    }

    e = rep.getExceptionType();
}

// ----------------------------------------------------------------------------
// turnInteractionsOff
void
DeclarationManagement::turnInteractionsOff(InteractionClassHandle interaction,
                                           TypeException &e)
{
    Message req ;
    req.type = Message::TURN_INTERACTIONS_OFF ;
    req.setInteractionClass(interaction);
    comm->sendUN(&req);
    Message rep ;
    comm->receiveUN(&rep);

    if (rep.type != req.type) {
        D.Out(pdExcept,
              "Unknown response type, expecting TURN_INTERACTIONS_OFF.");
        throw RTIinternalError();
    }

    e = rep.getExceptionType();
}

}} // namespace certi/rtia

// $Id: DeclarationManagement.cc,v 3.8 2003/06/07 22:24:12 breholee Exp $
