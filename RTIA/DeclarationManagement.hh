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
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIA_DM
#define _CERTI_RTIA_DM

#include <include/certi.hh>

#include <libCERTI/PrettyDebug.hh>
#include <libCERTI/RootObject.hh>

#include "Communications.hh"
#include "FederationManagement.hh"
#include "Files.hh"

namespace certi {
namespace rtia {

// Prototypes de classes existantes
class Communications;
class Queues;
class FederationManagement;

// Classe de gestion des declarations(publications et subscriptions).
class DeclarationManagement {
public:
    DeclarationManagement(Communications*, FederationManagement*, RootObject*);
    ~DeclarationManagement();

    void publishObjectClass(ObjectClassHandle theClassHandle,
                            const std::vector<AttributeHandle>& attribArray,
                            Exception::Type& e);

    void unpublishObjectClass(ObjectClassHandle theClassHandle, Exception::Type& e);

    void publishInteractionClass(InteractionClassHandle theInteractionHandle, Exception::Type& e);

    void unpublishInteractionClass(InteractionClassHandle theInteractionHandle, Exception::Type& e);

    void subscribeObjectClassAttribute(ObjectClassHandle theClassHandle,
                                       const std::vector<AttributeHandle>& attribArray,
                                       uint32_t attribArraySize,
                                       Exception::Type& e);

    void unsubscribeObjectClassAttribute(ObjectClassHandle theClassHandle, Exception::Type& e);

    void subscribeInteractionClass(InteractionClassHandle theClassHandle, Exception::Type& e);

    void unsubscribeInteractionClass(InteractionClassHandle theClassHandle, Exception::Type& e);

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
    void setClassRelevanceAdvisorySwitch(bool state, Exception::Type& e);

    // 5.10
    void startRegistrationForObjectClass(ObjectClassHandle the_class, Exception::Type& e);

    // 5.11
    void stopRegistrationForObjectClass(ObjectClassHandle the_class, Exception::Type& e);

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
    void setInteractionRelevanceAdvisorySwitch(bool state, Exception::Type& e);

    // 5.12
    void turnInteractionsOn(InteractionClassHandle interaction, Exception::Type& e);

    // 5.13
    void turnInteractionsOff(InteractionClassHandle interaction, Exception::Type& e);

protected:
    Communications* comm;
    Queues* queues;
    FederationManagement* fm;
    RootObject* rootObject;
};
}
} // namespace certi/rtia

#endif // _CERTI_RTIA_DM
