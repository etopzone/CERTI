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
// $Id: DeclarationManagement.cc,v 3.21 2008/07/09 13:48:28 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>

#include "InteractionSet.hh"
#include "ObjectClassSet.hh"
#include "DeclarationManagement.hh"
#include "NM_Classes.hh"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#endif

#include <memory>

namespace certi {
namespace rtia {

static pdCDebug D("RTIA_DM", "(RTIA DM) ");
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
                                          std::vector <AttributeHandle> &attribArray,
                                          UShort attribArraySize,
                                          TypeException &e)
{
    G.Out(pdGendoc,"enter DeclarationManagement::publishObjectClass") ;
    e = e_NO_EXCEPTION ;

    // Partie Locale

    try {
        rootObject->ObjectClasses->publish(fm->federate,
                                           theClassHandle,
                                           attribArray,
                                           attribArraySize,
                                           true);
    }
    catch (Exception *e) {
        G.Out(pdGendoc,"exit  DeclarationManagement::publishObjectClass on exception") ;
        D.Out(pdExcept, "Exception catched in PublishObjectClass.");
        throw e ;
    }

    // Partie RTIG
    NM_Publish_Object_Class req ;    
    req.objectClass     = theClassHandle ;
    req.handleArraySize = attribArraySize ;
    req.handleArray.resize(attribArraySize) ;
    req.federation      = fm->_numero_federation ;
    req.federate        = fm->federate ;

    for (int i=0 ; i<attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    // Emission
    comm->sendMessage(&req);

    // Reception    
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::PUBLISH_OBJECT_CLASS, req.federate));

    e = rep->exception ;
    G.Out(pdGendoc,"exit  DeclarationManagement::publishObjectClass") ;
} /* end of publishObjectClass */

// ----------------------------------------------------------------------------
// unpublishObjectClass
void
DeclarationManagement::unpublishObjectClass(ObjectClassHandle theClassHandle,
                                            TypeException &e)
{
    // Variables leurres
    //AttributeHandle *attribArray = NULL ;
    UShort attribArraySize = 0 ;
    std::vector <AttributeHandle> attribArrayVector ;
    attribArrayVector.empty();

    e = e_NO_EXCEPTION ;

    // Partie Locale

    try {
        rootObject->ObjectClasses->publish(fm->federate,
                                           theClassHandle,
                                           attribArrayVector,
                                           attribArraySize,
                                           false);
    } catch (Exception *e) {
        D.Out(pdExcept, "Exception catched in UnpublishObjectClass.");
        throw e ;
    }

    // Partie RTIG
    NM_Unpublish_Object_Class req ;
   
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.objectClass = theClassHandle ;

    // Emission de la requete vers le RTIG
    comm->sendMessage(&req);

    // On attend une reponse
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::UNPUBLISH_OBJECT_CLASS, req.federate));

    e = rep->exception ;
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
    
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.interactionClass = theInteractionHandle ;

    comm->sendMessage(&req);    
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::PUBLISH_INTERACTION_CLASS, req.federate));

    e = rep->exception ;
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
    req.interactionClass = theInteractionHandle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;

    comm->sendMessage(&req);
    
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::UNPUBLISH_INTERACTION_CLASS, req.federate));

    e = rep->exception ;
} /* end of unpublishInteractionClass */

// ----------------------------------------------------------------------------
// subscribeObjectClassAttribute
void
DeclarationManagement::
subscribeObjectClassAttribute(ObjectClassHandle theClassHandle,
                              std::vector <AttributeHandle> &attribArray,
                              UShort attribArraySize,
                              TypeException &e)
{
    NM_Subscribe_Object_Class req;

    G.Out(pdGendoc,"enter DeclarationManagement::subscribeObjectClassAttribute");
    // Pas de partie locale pour les abonnements

    // Partie RTIG

    req.federation      = fm->_numero_federation ;
    req.federate        = fm->federate ;
    req.objectClass     = theClassHandle ;
    req.handleArray.resize(attribArraySize) ;
    req.handleArraySize = attribArraySize ;

    for (int i=0 ; i<attribArraySize ; i++)
        req.handleArray[i] = attribArray[i] ;

    // Emission
    G.Out(pdGendoc,"                              =====> send S_O_C to RTIG");
    comm->sendMessage(&req);

    // Reception
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(
                      NetworkMessage::SUBSCRIBE_OBJECT_CLASS,
                      req.federate));
    G.Out(pdGendoc,"                              =====> received S_O_C from RTIG");

    e = rep->exception ;
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
    req.objectClass = theClassHandle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(
                      NetworkMessage::UNSUBSCRIBE_OBJECT_CLASS,
                      req.federate));

    e = rep->exception ;
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
    req.interactionClass = theClassHandle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::SUBSCRIBE_INTERACTION_CLASS,
		      req.federate));

    e = rep->exception ;
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
    req.interactionClass = theClassHandle ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;

    comm->sendMessage(&req);

    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(
                      NetworkMessage::UNSUBSCRIBE_INTERACTION_CLASS,
                      req.federate));

    e = rep->exception ;
} /* end of unsubscribeInteractionClass */

// ----------------------------------------------------------------------------
// setClassRelevanceAdvisorySwitch
void
DeclarationManagement::
setClassRelevanceAdvisorySwitch(bool state, TypeException &e) {
    G.Out(pdGendoc,"enter DeclarationManagement::setClassRelevanceAdvisorySwitch");
    G.Out(pdGendoc,"exit DeclarationManagement::setClassRelevanceAdvisorySwitch");
}

// ----------------------------------------------------------------------------
// startRegistrationForObjectClass
void
DeclarationManagement::
startRegistrationForObjectClass(ObjectClassHandle the_class,
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
        throw RTIinternalError("");
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
        throw RTIinternalError("");
    }

    e = rep.getExceptionType();
}

// ----------------------------------------------------------------------------
// setInteractionRelevanceAdvisorySwitch
void
DeclarationManagement::
setInteractionRelevanceAdvisorySwitch(bool state, TypeException &e) {
    G.Out(pdGendoc,"enter DeclarationManagement::setInteractionRelevanceAdvisorySwitch");

    G.Out(pdGendoc,"exit DeclarationManagement::setInteractionRelevanceAdvisorySwitch");
}

/* further refinement required, see OM.cc e.g. attributesInScope */

// ----------------------------------------------------------------------------
// turnInteractionsOn
void
DeclarationManagement::
turnInteractionsOn(InteractionClassHandle interaction,
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
        throw RTIinternalError("");
    }

    e = rep.getExceptionType();
}

// ----------------------------------------------------------------------------
// turnInteractionsOff
void
DeclarationManagement::
turnInteractionsOff(InteractionClassHandle interaction,
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
        throw RTIinternalError("");
    }

    e = rep.getExceptionType();
}

}} // namespace certi/rtia

// $Id: DeclarationManagement.cc,v 3.21 2008/07/09 13:48:28 erk Exp $
