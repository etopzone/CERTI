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
// $Id: DeclarationManagement.cc,v 3.36 2011/09/02 21:42:24 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>

#include "InteractionSet.hh"
#include "ObjectClassSet.hh"
#include "DeclarationManagement.hh"
#include "NM_Classes.hh"
#include "M_Classes.hh"

#include <memory>

namespace certi {
namespace rtia {

static PrettyDebug D("RTIA_DM", "(RTIA DM) ");
static PrettyDebug G("GENDOC",__FILE__) ;

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
                                          const std::vector <AttributeHandle> &attribArray,
                                          TypeException &e)
{
    G.Out(pdGendoc,"enter DeclarationManagement::publishObjectClass") ;
    e = e_NO_EXCEPTION ;

    // Local update
    try {
        rootObject->ObjectClasses->publish(fm->federate,
                                           theClassHandle,
                                           attribArray,
                                           true);
    }
    catch (Exception *e) {
        G.Out(pdGendoc,"exit  DeclarationManagement::publishObjectClass on exception") ;
        D.Out(pdExcept, "Exception catched in PublishObjectClass.");
        throw e ;
    }

    // RTIG update
    NM_Publish_Object_Class req ;    
    req.setObjectClass(theClassHandle);
    req.setAttributesSize(attribArray.size());
    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);

    for (uint32_t i=0 ; i<attribArray.size() ; i++)
        req.setAttributes(attribArray[i],i) ;

    // Send to RTIG
    comm->sendMessage(&req);

    // Receive RTIG answer
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::PUBLISH_OBJECT_CLASS, req.getFederate()));

    e = rep->getException() ;
    G.Out(pdGendoc,"exit  DeclarationManagement::publishObjectClass") ;
} /* end of publishObjectClass */

// ----------------------------------------------------------------------------
// unpublishObjectClass
void
DeclarationManagement::unpublishObjectClass(ObjectClassHandle theClassHandle,
                                            TypeException &e)
{
    // Dummy var containing no Attribute at all
    std::vector <AttributeHandle> attribArrayVector;

    e = e_NO_EXCEPTION ;

    // Local update
    try {
        rootObject->ObjectClasses->publish(fm->federate,
                                           theClassHandle,
                                           attribArrayVector,
                                           false);
    } catch (Exception *e) {
        D.Out(pdExcept, "Exception catched in UnpublishObjectClass.");
        throw e ;
    }

    // RTIG update
    NM_Unpublish_Object_Class req ;

    req.setFederation(fm->_numero_federation) ;
    req.setFederate(fm->federate);
    req.setObjectClass(theClassHandle);

    // Send request to RTIG
    comm->sendMessage(&req);

    // Receive RTIG answer
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::UNPUBLISH_OBJECT_CLASS, req.getFederate()));

    e = rep->getException() ;
} /* end of unpublishObjectClass */

// ----------------------------------------------------------------------------
// publishInteractionClass
void
DeclarationManagement::
publishInteractionClass(InteractionClassHandle theInteractionHandle,
                        TypeException &e)
{
    e = e_NO_EXCEPTION ;

    // Local publish
    try {
        rootObject->Interactions->publish(fm->federate,
                                          theInteractionHandle,
                                          true);
    } catch (Exception *e) {
        D.Out(pdExcept, "Exception catched in publishInteractionClass.");
        throw e ;
    }

    // RTIG (may be non-local) request
    NM_Publish_Interaction_Class req ;
    
    req.setFederation(fm->_numero_federation) ;
    req.setFederate(fm->federate);
    req.setInteractionClass(theInteractionHandle);

    comm->sendMessage(&req);    
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::PUBLISH_INTERACTION_CLASS, req.getFederate()));

    e = rep->getException() ;
} /* end of publishInteractionClass */

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
                                          false);
    } catch (Exception *e) {
        D.Out(pdExcept, "Exception catched in UnpublishInteractionClass.");
        throw e ;
    }

    // Partie RTIG
    NM_Unpublish_Interaction_Class req;   
    req.setInteractionClass(theInteractionHandle);
    req.setFederation(fm->_numero_federation) ;
    req.setFederate(fm->federate);

    comm->sendMessage(&req);
    
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::UNPUBLISH_INTERACTION_CLASS, req.getFederate()));

    e = rep->getException() ;
} /* end of unpublishInteractionClass */

// ----------------------------------------------------------------------------
// subscribeObjectClassAttribute
void
DeclarationManagement::
subscribeObjectClassAttribute(ObjectClassHandle theClassHandle,
                              const std::vector <AttributeHandle> &attribArray,
                              uint32_t attribArraySize,
                              TypeException &e)
{
    NM_Subscribe_Object_Class req;

    G.Out(pdGendoc,"enter DeclarationManagement::subscribeObjectClassAttribute");
    // Pas de partie locale pour les abonnements

    // Partie RTIG

    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);
    req.setObjectClass(theClassHandle);
    req.setAttributesSize(attribArraySize);

    for (uint32_t i=0 ; i<attribArraySize ; i++)
        req.setAttributes(attribArray[i],i) ;

    // Send the message to RTIG
    G.Out(pdGendoc,"                              =====> send S_O_C to RTIG");
    comm->sendMessage(&req);

    // Wait for the RTIG answer
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(
                      NetworkMessage::SUBSCRIBE_OBJECT_CLASS,
                      req.getFederate()));
    G.Out(pdGendoc,"                              =====> received S_O_C from RTIG");

    e = rep->getException() ;
    G.Out(pdGendoc,"exit  DeclarationManagement::subscribeObjectClassAttribute");
} /* end of subscribeObjectClassAttribute */

// ----------------------------------------------------------------------------
// unsubscribeObjectClassAttribute
void
DeclarationManagement::
unsubscribeObjectClassAttribute(ObjectClassHandle theClassHandle,
                                TypeException &e)
{
    NM_Unsubscribe_Object_Class req;

    e = e_NO_EXCEPTION ;

    // Pas de Partie Locale pour les abonnements

    // Partie RTIG    
    req.setObjectClass(theClassHandle);
    req.setFederation(fm->_numero_federation) ;
    req.setFederate(fm->federate);

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(
                      NetworkMessage::UNSUBSCRIBE_OBJECT_CLASS,
                      req.getFederate()));

    e = rep->getException() ;
} /* end of unsubscribeObjectClassAttribute */

// ----------------------------------------------------------------------------
// subscribeInteractionClass
void
DeclarationManagement::
subscribeInteractionClass(InteractionClassHandle theClassHandle,
                          TypeException &e)
{
    NM_Subscribe_Interaction_Class req;

    e = e_NO_EXCEPTION ;

    // Partie Locale

    // BUG: Pourquoi il y a-t-il une partie locale pour un abonnement ?
    // Ca ne va pas marcher avec les niveaux de securite !!!!

    try {
        rootObject->Interactions->subscribe(fm->federate, theClassHandle, 0, true);
    } catch (Exception *e) {
        D.Out(pdExcept, "Exception catched in subscribeInteractionClass.");
        throw e ;
    }

    // Partie RTIG    
    req.setInteractionClass(theClassHandle);
    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::SUBSCRIBE_INTERACTION_CLASS,
		      req.getFederate()));

    e = rep->getException() ;
} /* end of subscribeInteractionClass */

// ----------------------------------------------------------------------------
// unsubscribeInteractionClass
void
DeclarationManagement::
unsubscribeInteractionClass(InteractionClassHandle theClassHandle,
                            TypeException &e)
{
    NM_Unsubscribe_Interaction_Class req;

    e = e_NO_EXCEPTION ;

    // Partie Locale

    // BUG: Pourquoi il y a-t-il une partie locale pour un abonnement ?
    // Ca ne va pas marcher avec les niveaux de securite !!!!

    try {
        rootObject->Interactions->subscribe(fm->federate, theClassHandle, 0, false);
    } catch (Exception *e) {
        D.Out(pdExcept, "Exception catched in subscribeInteractionClass.");
        throw e ;
    }

    // Partie RTIG    
    req.setInteractionClass(theClassHandle);
    req.setFederation(fm->_numero_federation);
    req.setFederate(fm->federate);

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(
                      NetworkMessage::UNSUBSCRIBE_INTERACTION_CLASS,
                      req.getFederate()));

    e = rep->getException() ;
} /* end of unsubscribeInteractionClass */

void
DeclarationManagement::
setClassRelevanceAdvisorySwitch(bool state, TypeException &e) {
    G.Out(pdGendoc,"enter DeclarationManagement::setClassRelevanceAdvisorySwitch");

    NM_Set_Class_Relevance_Advisory_Switch msg ;

    e = e_NO_EXCEPTION ;

    msg.setFederation(fm->_numero_federation);
    msg.setFederate(fm->federate);

    if (state) {
        msg.classRelevanceAdvisorySwitchOn();
    }
    else {
        msg.classRelevanceAdvisorySwitchOff();
    }

    comm->sendMessage(&msg);

    G.Out(pdGendoc,"exit DeclarationManagement::setClassRelevanceAdvisorySwitch");
}

// ----------------------------------------------------------------------------
// startRegistrationForObjectClass
void
DeclarationManagement::
startRegistrationForObjectClass(ObjectClassHandle the_class,
                                TypeException &e)
{
    G.Out(pdGendoc,"enter DeclarationManagement::startRegistrationForObjectClass");

    M_Start_Registration_For_Object_Class req;

    req.setObjectClass(the_class);

    comm->requestFederateService(&req);

    G.Out(pdGendoc,"exit  DeclarationManagement::startRegistrationForObjectClass");
}

// ----------------------------------------------------------------------------
// stopRegistrationForObjectClass
void
DeclarationManagement::
stopRegistrationForObjectClass(ObjectClassHandle the_class,
                               TypeException &e)
{
    M_Stop_Registration_For_Object_Class req;
    // Pas de partie Locale

    // Partie Federe

    req.setObjectClass(the_class);
    comm->sendUN(&req);

    std::auto_ptr<Message>  rep(comm->receiveUN());

    if (rep->getMessageType() != req.getMessageType()) {
        D.Out(pdExcept, "Unknown response type when waiting for "
              "START_REGISTRATION_FOR_OBJECT_CLASS.");
        throw RTIinternalError("");
    }

    e = rep->getExceptionType();
}

void
DeclarationManagement::
setInteractionRelevanceAdvisorySwitch(bool state, TypeException &e) {
    G.Out(pdGendoc,"enter DeclarationManagement::setInteractionRelevanceAdvisorySwitch");

    NM_Set_Interaction_Relevance_Advisory_Switch msg ;

    e = e_NO_EXCEPTION ;

    msg.setFederation(fm->_numero_federation);
    msg.setFederate(fm->federate);

    if (state) {
        msg.interactionRelevanceAdvisorySwitchOn();
    }
    else {
        msg.interactionRelevanceAdvisorySwitchOff();
    }

    comm->sendMessage(&msg);

    G.Out(pdGendoc,"exit DeclarationManagement::setInteractionRelevanceAdvisorySwitch");
}

// ----------------------------------------------------------------------------
// turnInteractionsOn
void
DeclarationManagement::
turnInteractionsOn(InteractionClassHandle interaction,
                   TypeException &e)
{
    M_Turn_Interactions_On req;
    req.setInteractionClass(interaction);
    comm->sendUN(&req);
    std::auto_ptr<Message> rep(comm->receiveUN());

    if (rep->getMessageType() != req.getMessageType()) {
        D.Out(pdExcept,
              "Unknown response type, expecting TURN_INTERACTIONS_ON.");
        throw RTIinternalError("");
    }

    e = rep->getExceptionType();
}

// ----------------------------------------------------------------------------
// turnInteractionsOff
void
DeclarationManagement::
turnInteractionsOff(InteractionClassHandle interaction,
                    TypeException &e)
{
    M_Turn_Interactions_Off req ;
    req.setInteractionClass(interaction);
    comm->sendUN(&req);
    std::auto_ptr<Message> rep(comm->receiveUN());

    if (rep->getMessageType() != req.getMessageType()) {
        D.Out(pdExcept,
              "Unknown response type, expecting TURN_INTERACTIONS_OFF.");
        throw RTIinternalError("");
    }

    e = rep->getExceptionType();
}

}} // namespace certi/rtia

// $Id: DeclarationManagement.cc,v 3.36 2011/09/02 21:42:24 erk Exp $
