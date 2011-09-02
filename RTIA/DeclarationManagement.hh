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
// $Id: DeclarationManagement.hh,v 3.12 2011/09/02 21:42:25 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIA_DM
#define _CERTI_RTIA_DM

#include "FederationManagement.hh"
#include "Communications.hh"
#include "Files.hh"
#include "certi.hh"
#include "RootObject.hh"
#include "PrettyDebug.hh"

namespace certi {
namespace rtia {

// Prototypes de classes existantes
class Communications ;
class Queues ;
class FederationManagement ;

// Classe de gestion des declarations(publications et subscriptions).
class DeclarationManagement
{
public:
    DeclarationManagement(Communications*, FederationManagement*, RootObject*);
    ~DeclarationManagement();

    void publishObjectClass(ObjectClassHandle theClassHandle,
                            const std::vector <AttributeHandle> &attribArray,
                            TypeException &e);

    void unpublishObjectClass(ObjectClassHandle theClassHandle,
                              TypeException &e);

    void publishInteractionClass(InteractionClassHandle theInteractionHandle,
                                 TypeException &e);

    void unpublishInteractionClass(InteractionClassHandle theInteractionHandle,
                                   TypeException &e);

    void subscribeObjectClassAttribute(ObjectClassHandle theClassHandle,
                                       const std::vector <AttributeHandle> &attribArray,
                                       uint32_t attribArraySize,
                                       TypeException &e);

    void unsubscribeObjectClassAttribute(ObjectClassHandle theClassHandle,
                                         TypeException &e);

    void subscribeInteractionClass(InteractionClassHandle theClassHandle,
                                   TypeException &e);

    void unsubscribeInteractionClass(InteractionClassHandle theClassHandle,
                                     TypeException &e);

    /**
     * Transmits the Networkmessage NM_Set_Class_Relevance_Advisory_Switch to 
     * RTIG. The transmission sets the ClassRelevanceAdvisory switch at RTIG 
     * side to the value of the input parameter state. The invocation is caused
     * by a successfull transmission of the message 
     * ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH from the federate to RTIA.
     * @param[in] state is the new value for the switch, true means enable, 
     * false means disable
     * @param[in,out] e is a reference to a possible exception
     */
    void setClassRelevanceAdvisorySwitch(bool state,
                                         TypeException &e);

    // 5.10
    void startRegistrationForObjectClass(ObjectClassHandle theClass,
                                         TypeException &e);

    // 5.11
    void stopRegistrationForObjectClass(ObjectClassHandle theClass,
                                        TypeException &e);

    /**
     * Transmits the Networkmessage NM_Set_Interaction_Relevance_Advisory_Switch
     * to RTIG. The transmission sets the InteractionRelevanceAdvisory switch at
     * RTIG side to the value of the input parameter state. The invocation is 
     * caused by a successfull transmission of the message 
     * ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH from the federate to RTIA.
     * @param[in] state is the new value for the switch, true means enable, 
     * false means disable
     * @param[in,out] e is a reference to a possible exception
     */
    void setInteractionRelevanceAdvisorySwitch(bool state,
                                               TypeException &e);

    // 5.12
    void turnInteractionsOn(InteractionClassHandle theHandle,
                            TypeException &e);

    // 5.13
    void turnInteractionsOff(InteractionClassHandle theHandle,
                             TypeException &e);

protected:
    Communications *comm ;
    Queues *queues ;
    FederationManagement *fm ;
    RootObject *rootObject ;
};

}} // namespace certi/rtia

#endif // _CERTI_RTIA_DM

// $Id: DeclarationManagement.hh,v 3.12 2011/09/02 21:42:25 erk Exp $
