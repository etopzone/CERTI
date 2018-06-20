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

#include "DeclarationManagement.hh"
#include <config.h>

#include <memory>

#include <libCERTI/InteractionSet.hh>
#include <libCERTI/M_Classes.hh>
#include <libCERTI/NM_Classes.hh>
#include <libCERTI/ObjectClassSet.hh>

namespace certi {
namespace rtia {

static PrettyDebug D("RTIA_DM", "(RTIA DM) ");
static PrettyDebug G("GENDOC", __FILE__);

DeclarationManagement::DeclarationManagement(Communications* GC, FederationManagement* GF, RootObject* theRootObj)
{
    comm = GC;
    fm = GF;
    rootObject = theRootObj;
}

DeclarationManagement::~DeclarationManagement()
{
}

void DeclarationManagement::publishObjectClass(ObjectClassHandle theClassHandle,
                                               const std::vector<AttributeHandle>& attribArray,
                                               Exception::Type& e)
{
    Debug(G, pdGendoc) << "enter DeclarationManagement::publishObjectClass" << std::endl;
    e = Exception::Type::NO_EXCEPTION;

    // Local update
    try {
        rootObject->ObjectClasses->publish(fm->getFederateHandle(), theClassHandle, attribArray, true);
    }
    catch (Exception* e) {
        Debug(G, pdGendoc) << "exit  DeclarationManagement::publishObjectClass on exception" << std::endl;
        Debug(D, pdExcept) << "Exception catched in PublishObjectClass." << std::endl;
        throw;
    }

    // RTIG update
    NM_Publish_Object_Class req;
    req.setObjectClass(theClassHandle);
    req.setAttributesSize(attribArray.size());
    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());

    for (uint32_t i = 0; i < attribArray.size(); i++)
        req.setAttributes(attribArray[i], i);

    // Send to RTIG
    comm->sendMessage(&req);

    // Receive RTIG answer
    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::PUBLISH_OBJECT_CLASS, req.getFederate()));

    e = rep->getException();
    Debug(G, pdGendoc) << "exit  DeclarationManagement::publishObjectClass" << std::endl;
}

void DeclarationManagement::unpublishObjectClass(ObjectClassHandle theClassHandle, Exception::Type& e)
{
    // Dummy var containing no Attribute at all
    std::vector<AttributeHandle> attribArrayVector;

    e = Exception::Type::NO_EXCEPTION;

    // Local update
    try {
        rootObject->ObjectClasses->publish(fm->getFederateHandle(), theClassHandle, attribArrayVector, false);
    }
    catch (Exception* e) {
        Debug(D, pdExcept) << "Exception catched in UnpublishObjectClass." << std::endl;
        throw;
    }

    // RTIG update
    NM_Unpublish_Object_Class req;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setObjectClass(theClassHandle);

    // Send request to RTIG
    comm->sendMessage(&req);

    // Receive RTIG answer
    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::UNPUBLISH_OBJECT_CLASS, req.getFederate()));

    e = rep->getException();
}

void DeclarationManagement::publishInteractionClass(InteractionClassHandle theInteractionHandle, Exception::Type& e)
{
    e = Exception::Type::NO_EXCEPTION;

    // Local publish
    try {
        rootObject->Interactions->publish(fm->getFederateHandle(), theInteractionHandle, true);
    }
    catch (Exception* e) {
        Debug(D, pdExcept) << "Exception catched in publishInteractionClass." << std::endl;
        throw;
    }

    // RTIG (may be non-local) request
    NM_Publish_Interaction_Class req;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setInteractionClass(theInteractionHandle);

    comm->sendMessage(&req);
    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::PUBLISH_INTERACTION_CLASS, req.getFederate()));

    e = rep->getException();
}

void DeclarationManagement::unpublishInteractionClass(InteractionClassHandle theInteractionHandle, Exception::Type& e)
{
    e = Exception::Type::NO_EXCEPTION;

    // Partie Locale

    try {
        rootObject->Interactions->publish(fm->getFederateHandle(), theInteractionHandle, false);
    }
    catch (Exception* e) {
        Debug(D, pdExcept) << "Exception catched in UnpublishInteractionClass." << std::endl;
        throw;
    }

    // Partie RTIG
    NM_Unpublish_Interaction_Class req;
    req.setInteractionClass(theInteractionHandle);
    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());

    comm->sendMessage(&req);

    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::UNPUBLISH_INTERACTION_CLASS, req.getFederate()));

    e = rep->getException();
}

void DeclarationManagement::subscribeObjectClassAttribute(ObjectClassHandle theClassHandle,
                                                          const std::vector<AttributeHandle>& attribArray,
                                                          uint32_t attribArraySize,
                                                          Exception::Type& e)
{
    NM_Subscribe_Object_Class req;

    Debug(G, pdGendoc) << "enter DeclarationManagement::subscribeObjectClassAttribute" << std::endl;
    // Pas de partie locale pour les abonnements

    // Partie RTIG

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setObjectClass(theClassHandle);
    req.setAttributesSize(attribArraySize);

    for (uint32_t i = 0; i < attribArraySize; i++)
        req.setAttributes(attribArray[i], i);

    // Send the message to RTIG
    Debug(G, pdGendoc) << "                              =====> send S_O_C to RTIG" << std::endl;
    comm->sendMessage(&req);

    // Wait for the RTIG answer
    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::SUBSCRIBE_OBJECT_CLASS, req.getFederate()));
    Debug(G, pdGendoc) << "                              =====> received S_O_C from RTIG" << std::endl;

    e = rep->getException();
    Debug(G, pdGendoc) << "exit  DeclarationManagement::subscribeObjectClassAttribute" << std::endl;
}

void DeclarationManagement::unsubscribeObjectClassAttribute(ObjectClassHandle theClassHandle, Exception::Type& e)
{
    NM_Unsubscribe_Object_Class req;

    e = Exception::Type::NO_EXCEPTION;

    // Pas de Partie Locale pour les abonnements

    // Partie RTIG
    req.setObjectClass(theClassHandle);
    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());

    comm->sendMessage(&req);

    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::UNSUBSCRIBE_OBJECT_CLASS, req.getFederate()));

    e = rep->getException();
}

void DeclarationManagement::subscribeInteractionClass(InteractionClassHandle theClassHandle, Exception::Type& e)
{
    NM_Subscribe_Interaction_Class req;

    e = Exception::Type::NO_EXCEPTION;

    // Partie Locale

    // BUG: Pourquoi il y a-t-il une partie locale pour un abonnement ?
    // Ca ne va pas marcher avec les niveaux de securite !!!!

    try {
        rootObject->Interactions->subscribe(fm->getFederateHandle(), theClassHandle, 0, true);
    }
    catch (Exception* e) {
        Debug(D, pdExcept) << "Exception catched in subscribeInteractionClass." << std::endl;
        throw;
    }

    // Partie RTIG
    req.setInteractionClass(theClassHandle);
    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());

    comm->sendMessage(&req);

    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::SUBSCRIBE_INTERACTION_CLASS, req.getFederate()));

    e = rep->getException();
}

void DeclarationManagement::unsubscribeInteractionClass(InteractionClassHandle theClassHandle, Exception::Type& e)
{
    NM_Unsubscribe_Interaction_Class req;

    e = Exception::Type::NO_EXCEPTION;

    // Partie Locale

    // BUG: Pourquoi il y a-t-il une partie locale pour un abonnement ?
    // Ca ne va pas marcher avec les niveaux de securite !!!!

    try {
        rootObject->Interactions->subscribe(fm->getFederateHandle(), theClassHandle, 0, false);
    }
    catch (Exception* e) {
        Debug(D, pdExcept) << "Exception catched in subscribeInteractionClass." << std::endl;
        throw;
    }

    // Partie RTIG
    req.setInteractionClass(theClassHandle);
    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());

    comm->sendMessage(&req);

    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::UNSUBSCRIBE_INTERACTION_CLASS, req.getFederate()));

    e = rep->getException();
}

void DeclarationManagement::setClassRelevanceAdvisorySwitch(bool state, Exception::Type& e)
{
    Debug(G, pdGendoc) << "enter DeclarationManagement::setClassRelevanceAdvisorySwitch" << std::endl;

    NM_Set_Class_Relevance_Advisory_Switch msg;

    e = Exception::Type::NO_EXCEPTION;

    msg.setFederation(fm->getFederationHandle().get());
    msg.setFederate(fm->getFederateHandle());

    if (state) {
        msg.classRelevanceAdvisorySwitchOn();
    }
    else {
        msg.classRelevanceAdvisorySwitchOff();
    }

    comm->sendMessage(&msg);

    Debug(G, pdGendoc) << "exit DeclarationManagement::setClassRelevanceAdvisorySwitch" << std::endl;
}

void DeclarationManagement::startRegistrationForObjectClass(ObjectClassHandle the_class, Exception::Type& /*e*/)
{
    Debug(G, pdGendoc) << "enter DeclarationManagement::startRegistrationForObjectClass" << std::endl;

    M_Start_Registration_For_Object_Class req;

    req.setObjectClass(the_class);

    comm->requestFederateService(&req);

    Debug(G, pdGendoc) << "exit  DeclarationManagement::startRegistrationForObjectClass" << std::endl;
}

void DeclarationManagement::stopRegistrationForObjectClass(ObjectClassHandle the_class, Exception::Type& e)
{
    M_Stop_Registration_For_Object_Class req;
    // Pas de partie Locale

    // Partie Federe

    req.setObjectClass(the_class);
    comm->sendUN(&req);

    std::unique_ptr<Message> rep(comm->receiveUN());

    if (rep->getMessageType() != req.getMessageType()) {
        Debug(D, pdExcept) << "Unknown response type when waiting for START_REGISTRATION_FOR_OBJECT_CLASS."
                           << std::endl;
        throw RTIinternalError("");
    }

    e = rep->getExceptionType();
}

void DeclarationManagement::setInteractionRelevanceAdvisorySwitch(bool state, Exception::Type& e)
{
    Debug(G, pdGendoc) << "enter DeclarationManagement::setInteractionRelevanceAdvisorySwitch" << std::endl;

    NM_Set_Interaction_Relevance_Advisory_Switch msg;

    e = Exception::Type::NO_EXCEPTION;

    msg.setFederation(fm->getFederationHandle().get());
    msg.setFederate(fm->getFederateHandle());

    if (state) {
        msg.interactionRelevanceAdvisorySwitchOn();
    }
    else {
        msg.interactionRelevanceAdvisorySwitchOff();
    }

    comm->sendMessage(&msg);

    Debug(G, pdGendoc) << "exit DeclarationManagement::setInteractionRelevanceAdvisorySwitch" << std::endl;
}

void DeclarationManagement::turnInteractionsOn(InteractionClassHandle interaction, Exception::Type& e)
{
    M_Turn_Interactions_On req;
    req.setInteractionClass(interaction);
    comm->sendUN(&req);
    std::unique_ptr<Message> rep(comm->receiveUN());

    if (rep->getMessageType() != req.getMessageType()) {
        Debug(D, pdExcept) << "Unknown response type, expecting TURN_INTERACTIONS_ON." << std::endl;
        throw RTIinternalError("");
    }

    e = rep->getExceptionType();
}

void DeclarationManagement::turnInteractionsOff(InteractionClassHandle interaction, Exception::Type& e)
{
    M_Turn_Interactions_Off req;
    req.setInteractionClass(interaction);
    comm->sendUN(&req);
    std::unique_ptr<Message> rep(comm->receiveUN());

    if (rep->getMessageType() != req.getMessageType()) {
        Debug(D, pdExcept) << "Unknown response type, expecting TURN_INTERACTIONS_OFF." << std::endl;
        throw RTIinternalError("");
    }

    e = rep->getExceptionType();
}
}
} // namespace certi/rtia
