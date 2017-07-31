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
// ----------------------------------------------------------------------------

#include "NM_Classes.hh"
#include "RTIG.hh"
// #include <config.h>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
// #include <memory>

#include "AuditFile.hh"
#include "Exception.hh"
#include "FedTimeD.hh"
#include "Federate.hh"
#include "Federation.hh"
#include "FederationsList.hh"
#include "certi.hh"
// #include "GAV.hh"
#include "GAV.hh"
#include "HandleManager.hh"
#include "NetworkMessage.hh"
#include "PrettyDebug.hh"
#include "Socket.hh"
#include "SocketServer.hh"
#include "SocketTCP.hh"

#define OLD false

using std::endl;
using std::cout;

namespace certi {
namespace rtig {

static PrettyDebug D("RTIG", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);
static PrettyDebug DNULL("RTIG_NULLMSG", "[RTIG NULL MSG]");

// ----------------------------------------------------------------------------
//! Creates a new federation.
void RTIG::processCreateFederation(Socket* link, NM_Create_Federation_Execution* req)
{
    std::string federation = req->getFederationName();
    std::string FEDid = req->getFEDid();
    NM_Create_Federation_Execution rep; // rep for answer to RTIA

    Debug(G, pdGendoc) << "enter RTIG::processCreateFederation" << std::endl;
    Debug(G, pdGendoc) << "BEGIN ** CREATE FEDERATION SERVICE **" << std::endl;

    if (federation.length() == 0) {
        Debug(G, pdGendoc) << "exit  RTIG::processCreateFederation on exception RTIinternalError" << std::endl;
        throw RTIinternalError("Invalid Federation Name");
    }

    my_auditServer << "Federation Name : " << federation;
    Handle h = my_federationHandles.provide();

#ifdef FEDERATION_USES_MULTICAST
    // multicast base address
    unsigned long base_adr_mc = inet_addr(ADRESSE_MULTICAST);
    SocketMC* com_mc = NULL;

    // creer la communication multicast
    com_mc = new SocketMC();
    if (com_mc == NULL) {
        Debug(D, pdExcept) << "Unable to allocate Multicast socket" << std::endl;
        Debug(G, pdGendoc) << "exit  RTIG::processCreateFederation on exception RTIinternalError" << std::endl;
        throw RTIinternalError("Unable to allocate Multicast socket");
    }

    com_mc->CreerSocketMC(base_adr_mc + h, MC_PORT);

    // inserer la nouvelle federation dans la liste des federations
    federations->createFederation(federation, h, com_mc);

    // inserer descripteur fichier pour le prochain appel a un select
    ClientSockets.push_front(com_mc);

#else
    // We catch createFederation because it is useful to send
    // exception reason to RTIA
    try {
        my_federations.createFederation(federation, h, FEDid);
    }
    catch (CouldNotOpenFED& e) {
        rep.setException(e.type(), e.reason());
    }
    catch (ErrorReadingFED& e) {
        rep.setException(e.type(), e.reason());
    }
    catch (FederationExecutionAlreadyExists& e) {
        rep.setException(e.type(), e.reason());
    }
#endif
    // Prepare answer for RTIA : store NetworkMessage rep
    if (rep.getException() == Exception::Type::NO_EXCEPTION) {
        rep.setFederation(h);
        rep.setFEDid(FEDid);
        rep.setFederationName(federation);
        my_auditServer << " created";
    }

    Debug(G, pdGendoc) << "processCreateFederation===>write answer to RTIA" << std::endl;

    rep.send(link, my_NM_msgBufSend); // Send answer to RTIA

    Debug(D, pdInit) << "Federation \"" << federation << "\" created with Handle " << rep.getFederation() << std::endl;

    Debug(G, pdGendoc) << "END ** CREATE FEDERATION SERVICE **" << std::endl;
    Debug(G, pdGendoc) << "exit RTIG::processCreateFederation" << std::endl;
}

// ----------------------------------------------------------------------------
//! Add a new federate to federation.
void RTIG::processJoinFederation(Socket* link, NM_Join_Federation_Execution* req)
{
    std::string federation = req->getFederationName();
    std::string federate = req->getFederateName();

    unsigned int peer = req->getBestEffortPeer();
    unsigned long address = req->getBestEffortAddress();

    FederateHandle num_federe;

    int nb_regulateurs;
    int nb_federes;
    bool pause;

    Debug(G, pdGendoc) << "BEGIN ** JOIN FEDERATION SERVICE **" << std::endl;
    Debug(G, pdGendoc) << "enter RTIG::processJoinFederation" << std::endl;

    if (federation.empty() || federate.empty())
        throw RTIinternalError("Invalid Federation/Federate Name");

    my_auditServer << "Federate \"" << federate << "\" joins Federation \"" << federation << "\"";

    Handle num_federation = my_federations.getFederationHandle(federation);

    // Need to dump the FOM into that
    NM_Join_Federation_Execution rep;
    try {
        num_federe = my_federations.addFederate(num_federation, federate, static_cast<SocketTCP*>(link), rep);
    }
    catch (FederateAlreadyExecutionMember& e) {
        Debug(G, pdGendoc) << "exit RTIG::processJoinFederation on Error" << std::endl;
        Debug(G, pdGendoc) << "END ** JOIN FEDERATION (BAD) SERVICE **" << std::endl;
        // Prepare answer about JoinFederationExecution
        NM_Join_Federation_Execution rep;
        rep.setException(e.type(),
                         certi::stringize() << "Federate with same name <" << federate
                                            << "> has already joined the federation");

        Debug(G, pdGendoc) << "processJoinFederation==>Answer to RTIA JFE ERROR " << rep.getExceptionReason()
                           << std::endl;

        rep.send(link, my_NM_msgBufSend);
        return;
    }

#ifdef FEDERATION_USES_MULTICAST
    SocketMC* com_mc = NULL;

    federations.info(num_federation, nb_federes, nb_regulateurs, pause, com_mc);
    assert(com_mc != NULL);
#else
    my_federations.info(num_federation, nb_federes, nb_regulateurs, pause);
#endif

    // Store Federate <->Socket reference.
    my_socketServer.setReferences(link->returnSocket(), num_federation, num_federe, address, peer);

    my_auditServer << "(" << num_federation << ")with handle " << num_federe << ". Socket "
                   << int(link->returnSocket());

    // Prepare answer about JoinFederationExecution
    rep.setFederationName(federation);
    rep.setFederate(num_federe);
    rep.setFederation(num_federation);
    rep.setNumberOfRegulators(nb_regulateurs);
    rep.setBestEffortPeer(peer);
    rep.setBestEffortAddress(address);

// Now we have to answer about JoinFederationExecution
#ifdef FEDERATION_USES_MULTICAST
    rep.AdresseMulticast = com_mc->returnAdress();
#endif

    Debug(D, pdInit) << "Federate \"" << federate << "\" has joined Federation " << num_federation << " under handle "
                     << num_federe << std::endl;

    // Send answer
    rep.send(link, my_NM_msgBufSend);

    Debug(G, pdGendoc) << "exit RTIG::processJoinFederation" << std::endl;
    Debug(G, pdGendoc) << "END ** JOIN FEDERATION SERVICE **" << std::endl;
}

// ----------------------------------------------------------------------------
//! Removes a federate from federation.
void RTIG::processResignFederation(Socket* link, Handle federation, FederateHandle federe)
{
    NM_Resign_Federation_Execution reponse;

    Debug(G, pdGendoc) << "BEGIN ** RESIGN FEDERATION SERVICE **" << std::endl;
    Debug(G, pdGendoc) << "enter RTIG::processResignFederation" << std::endl;

#if OLD
    my_federations.remove(federation, federe);
#else
    my_federations.searchFederation(federation).remove(federe);
#endif
    Debug(D, pdInit) << "Federate " << federe << " is resigning from federation " << federation << std::endl;

    my_auditServer << "Federate " << federe << " resign federation(" << federation << ")";

    // Send answer to RTIA
    reponse.setFederate(federe);
    reponse.setFederation(federation);
    reponse.send(link, my_NM_msgBufSend);

    Debug(G, pdGendoc) << "exit RTIG::processResignFederation" << std::endl;
    Debug(G, pdGendoc) << "END ** RESIGN FEDERATION SERVICE **" << std::endl;
}

// ----------------------------------------------------------------------------
//! Removes a federation.
void RTIG::processDestroyFederation(Socket* link, NM_Destroy_Federation_Execution* req)
{
    NM_Destroy_Federation_Execution rep;

    std::string federation = req->getFederationName();

    Debug(G, pdGendoc) << "enter RTIG::processDestroyFederation" << std::endl;
    Debug(G, pdGendoc) << "BEGIN ** DESTROY FEDERATION SERVICE **" << std::endl;

    if (federation.length() == 0)
        throw RTIinternalError("Invalid Federation Name");

    Handle num_federation = my_federations.getFederationHandle(federation);
    // May throw RTIinternalError
    //           FederatesCurrentlyJoined
    //           FederationExecutionDoesNotExist
    try {
        my_federations.destroyFederation(num_federation);
        // Here delete federation (num_federation) has been done
        my_federationHandles.free(num_federation);
        Debug(D, pdInit) << "Federation \"" << federation << "\" has been destroyed" << std::endl;
    }
    catch (RTIinternalError& e) {
        std::cerr << "ERROR : " << e.name() << "  reason :" << e.reason() << std::endl;
        rep.setException(e.type(), e.reason());
    }
    catch (FederationExecutionDoesNotExist& e) {
        std::cerr << "ERROR : " << e.name() << "  reason :" << e.reason() << std::endl;
        rep.setException(e.type(), e.reason());
    }
    catch (FederatesCurrentlyJoined& e) {
        std::cerr << "ERROR : " << e.name() << "  reason :" << e.reason() << std::endl;
        rep.setException(e.type(), e.reason());
    }
    catch (Exception& e) {
        std::cerr << "ERROR : " << e.name() << "  reason :" << e.reason() << std::endl;
    }

    rep.setFederate(req->getFederate());
    rep.setFederationName(req->getFederationName());
    if (rep.getException() == Exception::Type::NO_EXCEPTION) {
        my_auditServer << "Federation Name \"" << federation << "\"(" << num_federation << ") destroyed";
    }
    Debug(G, pdGendoc) << "processDestroyFederation===>write DFE to RTIA" << std::endl;

    rep.send(link, my_NM_msgBufSend);

    Debug(G, pdGendoc) << "END ** DESTROY FEDERATION SERVICE **" << std::endl;
    Debug(G, pdGendoc) << "exit RTIG::processDestroyFederation" << std::endl;
}

// ----------------------------------------------------------------------------
//! Set federate's class relevance advisroy switch
void RTIG::processSetClassRelevanceAdvisorySwitch(Socket* link, NM_Set_Class_Relevance_Advisory_Switch* msg)
{
    NM_Set_Class_Relevance_Advisory_Switch rep;

    Debug(G, pdGendoc) << "enter RTIG::processSetClassRelevanceAdvisorySwitch" << std::endl;
    Debug(G, pdGendoc) << "BEGIN ** SET CLASS RELEVANCE ADVISORY SWITCH **" << std::endl;

    if (msg->isClassRelevanceAdvisorySwitchOn()) {
        my_auditServer << "ON";
#if OLD
        my_federations.setClassRelevanceAdvisorySwitch(msg->getFederation(), msg->getFederate());
#else
        my_federations.searchFederation(msg->getFederation())
            .getFederate(msg->getFederate())
            .setClassRelevanceAdvisorySwitch(true);
#endif
        Debug(D, pdTerm) << "Federate " << msg->getFederate() << " of Federation " << msg->getFederation()
                         << " sets AttributeRelevanceAdvisorySwitch" << std::endl;
    }
    else {
        my_auditServer << "OFF";
#if OLD
        my_federations.unsetClassRelevanceAdvisorySwitch(msg->getFederation(), msg->getFederate());
#else
        my_federations.searchFederation(msg->getFederation())
            .getFederate(msg->getFederate())
            .setClassRelevanceAdvisorySwitch(false);
#endif
        Debug(D, pdTerm) << "Federate " << msg->getFederate() << " of Federation " << msg->getFederation()
                         << " clears AttributeRelevanceAdvisorySwitch" << std::endl;
    }

    rep.send(link, my_NM_msgBufSend);

    Debug(G, pdGendoc) << "END ** SET CLASS RELEVANCE ADVISORY SWITCH **" << std::endl;
    Debug(G, pdGendoc) << "exit RTIG::processSetClassRelevanceAdvisorySwitch" << std::endl;
}

// ----------------------------------------------------------------------------
//! Set federate's interaction relevance advisroy switch
void RTIG::processSetInteractionRelevanceAdvisorySwitch(Socket* link, NM_Set_Interaction_Relevance_Advisory_Switch* msg)
{
    NM_Set_Interaction_Relevance_Advisory_Switch rep;

    Debug(G, pdGendoc) << "enter RTIG::processSetInteractionRelevanceAdvisorySwitch" << std::endl;
    Debug(G, pdGendoc) << "BEGIN ** SET INTERACTION RELEVANCE ADVISORY SWITCH **" << std::endl;

    if (msg->isInteractionRelevanceAdvisorySwitchOn()) {
        my_auditServer << "ON";
#if OLD
        my_federations.setInteractionRelevanceAdvisorySwitch(msg->getFederation(), msg->getFederate());
#else
        my_federations.searchFederation(msg->getFederation())
            .getFederate(msg->getFederate())
            .setInteractionRelevanceAdvisorySwitch(true);
#endif
        Debug(D, pdTerm) << "Federate " << msg->getFederate() << " of Federation " << msg->getFederation()
                         << " sets InteractionRelevanceAdvisorySwitch" << std::endl;
    }
    else {
        my_auditServer << "OFF";
#if OLD
        my_federations.unsetInteractionRelevanceAdvisorySwitch(msg->getFederation(), msg->getFederate());
#else
        my_federations.searchFederation(msg->getFederation())
            .getFederate(msg->getFederate())
            .setInteractionRelevanceAdvisorySwitch(false);
#endif
        Debug(D, pdTerm) << "Federate " << msg->getFederate() << " of Federation " << msg->getFederation()
                         << " clears InteractionRelevanceAdvisorySwitch" << std::endl;
    }

    rep.send(link, my_NM_msgBufSend);

    Debug(G, pdGendoc) << "END ** SET INTERACTION RELEVANCE ADVISORY SWITCH **" << std::endl;
    Debug(G, pdGendoc) << "exit RTIG::processSetInteractionRelevanceAdvisorySwitch" << std::endl;
}

// ----------------------------------------------------------------------------
//! Set federate's attribute relevance advisroy switch
void RTIG::processSetAttributeRelevanceAdvisorySwitch(Socket* link, NM_Set_Attribute_Relevance_Advisory_Switch* msg)
{
    NM_Set_Attribute_Relevance_Advisory_Switch rep;

    Debug(G, pdGendoc) << "enter RTIG::processSetAttributeRelevanceAdvisorySwitch" << std::endl;
    Debug(G, pdGendoc) << "BEGIN ** SET ATTRIBUTE RELEVANCE ADVISORY SWITCH **" << std::endl;

    if (msg->isAttributeRelevanceAdvisorySwitchOn()) {
        my_auditServer << "ON";
#if OLD
        my_federations.setAttributeRelevanceAdvisorySwitch(msg->getFederation(), msg->getFederate());
#else
        my_federations.searchFederation(msg->getFederation())
            .getFederate(msg->getFederate())
            .setAttributeRelevanceAdvisorySwitch(true);
#endif
        Debug(D, pdTerm) << "Federate " << msg->getFederate() << " of Federation " << msg->getFederation()
                         << " sets AttributeRelevanceAdvisorySwitch" << std::endl;
    }
    else {
        my_auditServer << "OFF";
#if OLD
        my_federations.unsetAttributeRelevanceAdvisorySwitch(msg->getFederation(), msg->getFederate());
#else
        my_federations.searchFederation(msg->getFederation())
            .getFederate(msg->getFederate())
            .setAttributeRelevanceAdvisorySwitch(false);
#endif
        Debug(D, pdTerm) << "Federate " << msg->getFederate() << " of Federation " << msg->getFederation()
                         << " clears AttributeRelevanceAdvisorySwitch" << std::endl;
    }

    rep.send(link, my_NM_msgBufSend);

    Debug(G, pdGendoc) << "END ** SET ATTRIBUTE RELEVANCE ADVISORY SWITCH **" << std::endl;
    Debug(G, pdGendoc) << "exit RTIG::processSetAttributeRelevanceAdvisorySwitch" << std::endl;
}

// ----------------------------------------------------------------------------
//! Set federate's attribute scope advisroy switch
void RTIG::processSetAttributeScopeAdvisorySwitch(Socket* link, NM_Set_Attribute_Scope_Advisory_Switch* msg)
{
    NM_Set_Attribute_Scope_Advisory_Switch rep;

    Debug(G, pdGendoc) << "enter RTIG::processSetAttributeScopeAdvisorySwitch" << std::endl;
    Debug(G, pdGendoc) << "BEGIN ** SET ATTRIBUTE SCOPE ADVISORY SWITCH **" << std::endl;

    if (msg->isAttributeScopeAdvisorySwitchOn()) {
        my_auditServer << "ON";
#if OLD
        my_federations.setAttributeScopeAdvisorySwitch(msg->getFederation(), msg->getFederate());
#else
        my_federations.searchFederation(msg->getFederation())
            .getFederate(msg->getFederate())
            .setAttributeScopeAdvisorySwitch(true);
#endif
        Debug(D, pdTerm) << "Federate " << msg->getFederate() << " of Federation " << msg->getFederation()
                         << " sets AttributeScopeAdvisorySwitch" << std::endl;
    }
    else {
        my_auditServer << "OFF";
#if OLD
        my_federations.unsetAttributeScopeAdvisorySwitch(msg->getFederation(), msg->getFederate());
#else
        my_federations.searchFederation(msg->getFederation())
            .getFederate(msg->getFederate())
            .setAttributeScopeAdvisorySwitch(false);
#endif
        Debug(D, pdTerm) << "Federate " << msg->getFederate() << " of Federation " << msg->getFederation()
                         << " clears AttributeScopeAdvisorySwitch" << std::endl;
    }

    rep.send(link, my_NM_msgBufSend);

    Debug(G, pdGendoc) << "END ** SET ATTRIBUTE SCOPE ADVISORY SWITCH **" << std::endl;
    Debug(G, pdGendoc) << "exit RTIG::processSetAttributeScopeAdvisorySwitch" << std::endl;
}

// ----------------------------------------------------------------------------
//! Put federate as time regulating.
void RTIG::processSetTimeRegulating(Socket* link, NM_Set_Time_Regulating* msg)
{
    Debug(G, pdGendoc) << "enter RTIG::processSetTimeRegulating" << std::endl;
    Debug(G, pdGendoc) << "BEGIN ** SET TIME REGULATING **" << std::endl;

    if (msg->isRegulatorOn()) {
        my_auditServer << "ON at time " << msg->getDate().getTime();

#if OLD
        my_federations.createRegulator(msg->getFederation(), msg->getFederate(), msg->getDate());
#else
        my_federations.searchFederation(msg->getFederation()).addRegulator(msg->getFederate(), msg->getDate());
#endif

        // send timeRegulationEnabled() to federate.
        NM_Time_Regulation_Enabled rep;
        rep.setFederate(msg->getFederate());
        rep.setFederation(msg->getFederation());
        rep.setDate(msg->getDate());

        Debug(G, pdGendoc) << "      processSetTimeRegulating====> write TRE to RTIA" << std::endl;

        rep.send(link, my_NM_msgBufSend);

        Debug(D, pdTerm) << "Federate " << msg->getFederate() << " of Federation " << msg->getFederation()
                         << " sets TimeRegulation ON" << std::endl;
    }
    else {
        my_auditServer << "OFF";

#if OLD
        my_federations.removeRegulator(msg->getFederation(), msg->getFederate());
#else
        my_federations.searchFederation(msg->getFederation()).removeRegulator(msg->getFederate());
#endif
        Debug(D, pdTerm) << "Federate " << msg->getFederate() << " of Federation " << msg->getFederation()
                         << " sets TimeRegulation OFF" << std::endl;
    }

    Debug(G, pdGendoc) << "END ** SET TIME REGULATING **" << std::endl;
    Debug(G, pdGendoc) << "exit RTIG::processSetTimeRegulating" << std::endl;
}

// ----------------------------------------------------------------------------
//! Put federate as time constrained
void RTIG::processSetTimeConstrained(Socket* link, NM_Set_Time_Constrained* msg)
{
    Debug(G, pdGendoc) << "enter RTIG::processSetTimeConstrained" << std::endl;
    Debug(G, pdGendoc) << "BEGIN ** SET TIME CONSTRAINED **" << std::endl;

    if (msg->isConstrainedOn()) {
        my_auditServer << "ON at time " << msg->getDate().getTime();

#if OLD
        my_federations.addConstrained(msg->getFederation(), msg->getFederate());
#else
        my_federations.searchFederation(msg->getFederation()).getFederate(msg->getFederate()).setConstrained(true);

#endif

        // send timeConstrainedEnabled() to federate.
        NM_Time_Constrained_Enabled rep;
        rep.setFederate(msg->getFederate());
        rep.setFederation(msg->getFederation());
        rep.setDate(msg->getDate());

        Debug(G, pdGendoc) << "      processSetTimeConstrained====> write TCE to RTIA" << std::endl;

        rep.send(link, my_NM_msgBufSend);

        Debug(D, pdTerm) << "Federate " << msg->getFederate() << " of Federation " << msg->getFederation()
                         << " is now constrained" << std::endl;
    }
    else {
        my_auditServer << "OFF";

#if OLD
        my_federations.removeConstrained(msg->getFederation(), msg->getFederate());
#else
        my_federations.searchFederation(msg->getFederation()).getFederate(msg->getFederate()).setConstrained(false);
#endif
        Debug(D, pdTerm) << "Federate " << msg->getFederate() << " of Federation " << msg->getFederation()
                         << " is no more constrained" << std::endl;
    }

    Debug(G, pdGendoc) << "END ** SET TIME CONSTRAINED **" << std::endl;
    Debug(G, pdGendoc) << "exit RTIG::processSetTimeConstrained" << std::endl;
}

void RTIG::processMessageNull(NetworkMessage* msg, bool anonymous)
{
    my_auditServer << "Date " << msg->getDate().getTime();

    Debug(DNULL, pdDebug) << "Rcv NULL MSG (Federate=" << msg->getFederate() << ", Time = " << msg->getDate().getTime()
                          << ")" << std::endl;
    // Catch all exceptions because RTIA does not expect an answer anyway.
    try {
#if OLD
        my_federations.updateRegulator(msg->getFederation(), msg->getFederate(), msg->getDate(), anonymous);
#else
        my_federations.searchFederation(msg->getFederation())
            .updateRegulator(msg->getFederate(), msg->getDate(), anonymous);
#endif
    }
    catch (Exception& e) {
    }
} /* end of processMessageNull */

void RTIG::processMessageNullPrime(NM_Message_Null_Prime* msg)
{
    Debug(DNULL, pdDebug) << "Rcv NULL PRIME MSG (Federate=" << msg->getFederate()
                          << ", Time = " << msg->getDate().getTime() << ")" << std::endl;
/*
	 * Update the NullPrimeDate of the concerned federate.
	 * and check the result in order to decide whether
	 * if the RTIG should send an anonymous NULL message or not
     */
#if OLD
    if (my_federations.handleMessageNullPrime(msg->getFederation(), msg->getFederate(), msg->getDate())) {
#else
    if (my_federations.searchFederation(msg->getFederation())
            .updateLastNERxForFederate(msg->getFederate(), msg->getDate())) {
#endif
        NM_Message_Null nmsg;
#if OLD
        nmsg.setDate(my_federations.getNullPrimeValue(msg->getFederation()));
#else
        nmsg.setDate(my_federations.searchFederation(msg->getFederation()).getMinNERx());
#endif
        nmsg.setFederation(msg->getFederation());
        nmsg.setFederate(0);
        //nmsg.show(std::cout);
        processMessageNull(&nmsg, true);
    }

} /* end of processMessageNullPrime */

// ----------------------------------------------------------------------------
//! processRegisterSynchronization.
void RTIG::processRegisterSynchronization(Socket* link, NM_Register_Federation_Synchronization_Point* req)
{
    Debug(G, pdGendoc) << "BEGIN ** REGISTER FEDERATION SYNCHRONIZATION POINT Service **" << std::endl;
    Debug(G, pdGendoc) << "enter RTIG::processRegisterSynchronization" << std::endl;

    my_auditServer << "Label \"" << req->getLabel() << "\" registered. Tag is \"" << req->getTag() << "\"";

    /* prepare answer to be sent to RTIA */
    NM_Confirm_Synchronization_Point_Registration rep;
    rep.setFederate(req->getFederate());
    rep.setFederation(req->getFederation());
    rep.setLabel(req->getLabel());

    try {
        // boolean true means a federates set exists
        if (req->getExists()) {
#if OLD
            my_federations.manageSynchronization(req->getFederation(),
                                                 req->getFederate(),
                                                 true,
                                                 req->getLabel(),
                                                 req->getTag(),
                                                 req->getFederatesSize(),
                                                 req->getFederates());
#else
            my_federations.searchFederation(req->getFederation())
                .registerSynchronization(req->getFederate(), req->getLabel(), req->getTag(), req->getFederates());
#endif
        }
        else {
#if OLD
            my_federations.manageSynchronization(
                req->getFederation(), req->getFederate(), true, req->getLabel(), req->getTag());
#else
            my_federations.searchFederation(req->getFederation())
                .registerSynchronization(req->getFederate(), req->getLabel(), req->getTag());
#endif
        }
        // send synchronizationPointRegistrationSucceeded() to federate.
        rep.setSuccessIndicator(true);
    }
    catch (Exception& e) {
        /* the registration did fail */
        rep.setSuccessIndicator(false);
        rep.setFailureReason(std::string(e.name()) + ":" + std::string(e.reason()));
    }
    Debug(D, pdTerm) << "Federation " << req->getFederation() << " is now synchronizing" << std::endl;

    Debug(G, pdGendoc) << "      processRegisterSynchronization====> write SPRS to RTIA" << std::endl;
    rep.send(link, my_NM_msgBufSend);

    // proceed with the broadcast iff registration was successful
    if (rep.getSuccessIndicator()) {
        // boolean true means a federates set exists
        if (req->getExists()) {
#if OLD
            my_federations.broadcastSynchronization(req->getFederation(),
                                                    req->getFederate(),
                                                    req->getLabel(),
                                                    req->getTag(),
                                                    req->getFederatesSize(),
                                                    req->getFederates());
#else
            my_federations.searchFederation(req->getFederation())
                .broadcastSynchronization(req->getFederate(), req->getLabel(), req->getTag(), req->getFederates());
#endif
        }
        else {
#if OLD
            my_federations.broadcastSynchronization(
                req->getFederation(), req->getFederate(), req->getLabel(), req->getTag());
#else
            my_federations.searchFederation(req->getFederation())
                .broadcastSynchronization(req->getFederate(), req->getLabel(), req->getTag());
#endif
        }
    }
    Debug(G, pdGendoc) << "exit  RTIG::processRegisterSynchronization" << std::endl;
    Debug(G, pdGendoc) << "END   ** REGISTER FEDERATION SYNCHRONIZATION POINT Service **" << std::endl;
}

// ----------------------------------------------------------------------------
//! processSynchronizationAchieved.
void RTIG::processSynchronizationAchieved(Socket*, NetworkMessage* req)
{
    my_auditServer << "Label \"" << req->getLabel() << "\" ended";

#if OLD
    my_federations.manageSynchronization(req->getFederation(), req->getFederate(), false, req->getLabel(), "");
#else
    my_federations.searchFederation(req->getFederation())
        .unregisterSynchronization(req->getFederate(), req->getLabel());
#endif
    Debug(D, pdTerm) << "Federate " << req->getFederate() << " has synchronized" << std::endl;
}

// ----------------------------------------------------------------------------
void RTIG::processRequestFederationSave(Socket*, NetworkMessage* req)
{
    Debug(G, pdGendoc) << "BEGIN ** REQUEST FEDERATION SAVE SERVICE **" << std::endl;
    Debug(G, pdGendoc) << "enter RTIG::processRequestFederationSave" << std::endl;

    my_auditServer << "Federation save request";

    if (req->isDated()) {
// With time
#if OLD
        my_federations.requestFederationSave(req->getFederation(), req->getFederate(), req->getLabel(), req->getDate());
#else
        my_federations.searchFederation(req->getFederation())
            .requestFederationSave(req->getFederate(), req->getLabel(), req->getDate());
#endif
    }
    else {
// Without time
#if OLD
        my_federations.requestFederationSave(req->getFederation(), req->getFederate(), req->getLabel());
#else
        my_federations.searchFederation(req->getFederation())
            .requestFederationSave(req->getFederate(), req->getLabel());
#endif
    }

    Debug(G, pdGendoc) << "exit  RTIG::processRequestFederationSave" << std::endl;
    Debug(G, pdGendoc) << "END   ** REQUEST FEDERATION SAVE SERVICE **" << std::endl;
}
// ----------------------------------------------------------------------------
void RTIG::processFederateSaveBegun(Socket*, NetworkMessage* req)
{
    Debug(G, pdGendoc) << "enter RTIG::processFederateSaveBegun" << std::endl;
    Debug(G, pdGendoc) << "BEGIN ** FEDERATE SAVE BEGUN SERVICE **" << std::endl;
    Debug(G, pdGendoc) << "processFederateSaveBegun federation = " << req->getFederation() << std::endl;

    my_auditServer << "Federate " << req->getFederate() << " save begun";

#if OLD
    my_federations.federateSaveBegun(req->getFederation(), req->getFederate());
#else
    my_federations.searchFederation(req->getFederation()).federateSaveBegun(req->getFederate());
#endif

    Debug(G, pdGendoc) << "END   ** FEDERATE SAVE BEGUN SERVICE **" << std::endl;
    Debug(G, pdGendoc) << "exit  RTIG::processFederateSaveBegun" << std::endl;
}

// ----------------------------------------------------------------------------
void RTIG::processFederateSaveStatus(Socket*, NetworkMessage* req)
{
    Debug(G, pdGendoc) << "enter RTIG::processFederateSaveStatus" << std::endl;
    if (req->getMessageType() == NetworkMessage::FEDERATE_SAVE_COMPLETE) {
        Debug(G, pdGendoc) << "BEGIN ** FEDERATE SAVE COMPLETE SERVICE **" << std::endl;
    }
    else {
        Debug(G, pdGendoc) << "BEGIN ** FEDERATE SAVE NOT COMPLETE SERVICE **" << std::endl;
    }

    my_auditServer << "Federate " << req->getFederate() << " save ended";

    bool status = req->getMessageType() == NetworkMessage::FEDERATE_SAVE_COMPLETE;
#if OLD
    my_federations.federateSaveStatus(req->getFederation(), req->getFederate(), status);
#else
    my_federations.searchFederation(req->getFederation()).federateSaveStatus(req->getFederate(), status);
#endif

    Debug(G, pdGendoc) << "exit  END   ** FEDERATE SAVE (NOT) COMPLETE SERVICE **" << std::endl;
    Debug(G, pdGendoc) << "exit  RTIG::processFederateSaveStatus" << std::endl;
}

// ----------------------------------------------------------------------------
void RTIG::processRequestFederationRestore(Socket*, NetworkMessage* req)
{
    Debug(G, pdGendoc) << "BEGIN ** REQUEST FEDERATION RESTORE SERVICE **" << std::endl;
    Debug(G, pdGendoc) << "enter RTIG::processRequestFederationRestore" << std::endl;
    my_auditServer << "Federate " << req->getFederate() << " request restore";

#if OLD
    my_federations.requestFederationRestore(req->getFederation(), req->getFederate(), req->getLabel());
#else
    my_federations.searchFederation(req->getFederation()).requestFederationRestore(req->getFederate(), req->getLabel());
#endif
    Debug(G, pdGendoc) << "exit  RTIG::processRequestFederationRestore" << std::endl;
    Debug(G, pdGendoc) << "END   ** REQUEST FEDERATION RESTORE SERVICE **" << std::endl;
}

// ----------------------------------------------------------------------------
void RTIG::processFederateRestoreStatus(Socket*, NetworkMessage* req)
{
    Debug(G, pdGendoc) << "BEGIN ** FEDERATE RESTORE (NOT)COMPLETE **" << std::endl;
    Debug(G, pdGendoc) << "enter RTIG::processRequestFederateRestoreStatus" << std::endl;
    my_auditServer << "Federate " << req->getFederate() << " restore ended";

    bool status = req->getMessageType() == NetworkMessage::FEDERATE_RESTORE_COMPLETE;

#if OLD
    my_federations.federateRestoreStatus(req->getFederation(), req->getFederate(), status);
#else
    my_federations.searchFederation(req->getFederation()).federateRestoreStatus(req->getFederate(), status);
#endif

    Debug(G, pdGendoc) << "exit  RTIG::processRequestFederateRestoreStatus" << std::endl;
    Debug(G, pdGendoc) << "END   ** FEDERATE RESTORE (NOT)COMPLETE **" << std::endl;
}

// ----------------------------------------------------------------------------
// processPublishObjectClass
void RTIG::processPublishObjectClass(Socket* link, NM_Publish_Object_Class* req)
{
    bool pub = (req->getMessageType() == NetworkMessage::PUBLISH_OBJECT_CLASS);

    my_auditServer << "Publish Object Class = " << req->getObjectClass()
                   << ", # of att. = " << req->getAttributesSize();

#if OLD
    my_federations.publishObject(
        req->getFederation(), req->getFederate(), req->getObjectClass(), req->getAttributes(), pub);
#else
    my_federations.searchFederation(req->getFederation())
        .publishObject(req->getFederate(), req->getObjectClass(), req->getAttributes(), pub);
#endif

    Debug(D, pdRegister) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                         << " published object class " << req->getObjectClass() << std::endl;

    if (pub) {
        NM_Publish_Object_Class POC;
        POC.setFederate(req->getFederate());
        POC.setObjectClass(req->getObjectClass());
        POC.send(link, my_NM_msgBufSend); // send answer to RTIA
    }
    else {
        NM_Unpublish_Object_Class UOC;
        UOC.setFederate(req->getFederate());
        UOC.setObjectClass(req->getObjectClass());
        UOC.send(link, my_NM_msgBufSend); // send answer to RTIA
    }
}

// ----------------------------------------------------------------------------
// processSubscribeObjectClass
void RTIG::processSubscribeObjectClass(Socket* link, NM_Subscribe_Object_Class* req)
{
    Debug(G, pdGendoc) << "enter RTIG::processSubscribeObjectClass" << std::endl;
    Debug(G, pdGendoc) << "BEGIN **  SUBSCRIBE OBJECT CLASS SERVICE **" << std::endl;

    std::vector<AttributeHandle> emptyAttributeList;
    bool sub = (req->getMessageType() == NetworkMessage::SUBSCRIBE_OBJECT_CLASS);

    my_auditServer << "Subscribe Object Class = " << req->getObjectClass()
                   << ", # of att. = " << req->getAttributesSize();
#if OLD
    my_federations.subscribeObject(req->getFederation(),
                                   req->getFederate(),
                                   req->getObjectClass(),
                                   sub ? req->getAttributes() : emptyAttributeList);
#else
    my_federations.searchFederation(req->getFederation())
        .subscribeObject(req->getFederate(), req->getObjectClass(), sub ? req->getAttributes() : emptyAttributeList);
#endif

    Debug(D, pdRegister) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                         << " subscribed to object class " << req->getObjectClass() << std::endl;

    if (sub) {
        NM_Subscribe_Object_Class rep;
        rep.setFederate(req->getFederate());
        rep.setObjectClass(req->getObjectClass());
        rep.send(link, my_NM_msgBufSend); // send answer to RTIA
    }
    else {
        NM_Unsubscribe_Object_Class rep;
        rep.setFederate(req->getFederate());
        rep.setObjectClass(req->getObjectClass());
        rep.send(link, my_NM_msgBufSend); // send answer to RTIA
    }

    Debug(G, pdGendoc) << "END   **  SUBSCRIBE OBJECT CLASS SERVICE **" << std::endl;
    Debug(G, pdGendoc) << "exit  RTIG::processSubscribeObjectClass" << std::endl;
}

// ----------------------------------------------------------------------------
// processPublishInteractionClass
void RTIG::processPublishInteractionClass(Socket* link, NM_Publish_Interaction_Class* req)
{
    assert(link != NULL && req != NULL);

    bool pub = (req->getMessageType() == NetworkMessage::PUBLISH_INTERACTION_CLASS);

    my_auditServer << "Publish Interaction Class = " << req->getInteractionClass();
#if OLD
    my_federations.publishInteraction(req->getFederation(), req->getFederate(), req->getInteractionClass(), pub);
#else
    my_federations.searchFederation(req->getFederation())
        .publishInteraction(req->getFederate(), req->getInteractionClass(), pub);
#endif
    Debug(D, pdRequest) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                        << " publishes Interaction " << req->getInteractionClass() << std::endl;

    if (pub) {
        NM_Publish_Interaction_Class rep;
        rep.setFederate(req->getFederate());
        rep.setInteractionClass(req->getInteractionClass());

        rep.send(link, my_NM_msgBufSend); // send answer to RTIA
    }
    else {
        NM_Unpublish_Interaction_Class rep;
        rep.setFederate(req->getFederate());
        rep.setInteractionClass(req->getInteractionClass());

        rep.send(link, my_NM_msgBufSend); // send answer to RTIA
    }
}

// ----------------------------------------------------------------------------
// processSubscribeInteractionClass
void RTIG::processSubscribeInteractionClass(Socket* link, NM_Subscribe_Interaction_Class* req)
{
    bool sub = (req->getMessageType() == NetworkMessage::SUBSCRIBE_INTERACTION_CLASS);

    my_auditServer << "Subscribe Interaction Class = " << req->getInteractionClass();
#if OLD
    my_federations.subscribeInteraction(req->getFederation(), req->getFederate(), req->getInteractionClass(), sub);
#else
    my_federations.searchFederation(req->getFederation())
        .subscribeInteraction(req->getFederate(), req->getInteractionClass(), sub);
#endif
    Debug(D, pdRequest) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                        << " subscribed to Interaction " << req->getInteractionClass() << std::endl;

    if (sub) {
        NM_Subscribe_Interaction_Class rep;
        rep.setFederate(req->getFederate());
        rep.setInteractionClass(req->getInteractionClass());

        rep.send(link, my_NM_msgBufSend); // send answer to RTIA
    }
    else {
        NM_Unsubscribe_Interaction_Class rep;
        rep.setFederate(req->getFederate());
        rep.setInteractionClass(req->getInteractionClass());

        rep.send(link, my_NM_msgBufSend); // send answer to RTIA
    }
}

// ----------------------------------------------------------------------------
// processReserveObjectInstanceName
void RTIG::processReserveObjectInstanceName(Socket* link, NM_Reserve_Object_Instance_Name* req)
{
    Debug(G, pdGendoc) << "enter RTIG::processReserveObjectInstanceName" << std::endl;
    my_auditServer << "Reserve Object Name = " << req->getObjectName();

#if OLD
    my_federations.reserveObjectInstanceName(req->getFederation(), req->getFederate(), req->getObjectName());
#else
    my_federations.searchFederation(req->getFederation())
        .reserveObjectInstanceName(req->getFederate(), req->getObjectName());
#endif

    Debug(G, pdGendoc) << "exit RTIG::processReserveObjectInstanceName" << std::endl;
}

// ----------------------------------------------------------------------------
// processRegisterObject
void RTIG::processRegisterObject(Socket* link, NM_Register_Object* req)
{
    NM_Register_Object rep;

    my_auditServer << "Register Object Class = " << req->getObjectClass();
#if OLD
    rep.setObject(my_federations.registerObject(
        req->getFederation(), req->getFederate(), req->getObjectClass(), req->getLabel()));
#else
    rep.setObject(my_federations.searchFederation(req->getFederation())
                      .registerObject(req->getFederate(), req->getObjectClass(), req->getLabel()));
#endif

    Debug(D, pdRegister) << "Object \"" << req->getLabel() << "\" of Federate " << req->getFederate()
                         << " has been registered under ID " << rep.getObject() << std::endl;

    rep.setFederate(req->getFederate());
    rep.setFederation(req->getFederation());
    rep.setObjectClass(req->getObjectClass());
    // rep.object is set by the call of registerObject
    if (req->hasObjectName()) {
        rep.setObjectName(req->getObjectName());
    }
    if (req->isLabelled()) {
        rep.setLabel(req->getLabel());
    }

    rep.send(link, my_NM_msgBufSend); // Send answer to RTIA
}

// ----------------------------------------------------------------------------
// processUpdateAttributeValues
void RTIG::processUpdateAttributeValues(Socket* link, NM_Update_Attribute_Values* req)
{
    Debug(G, pdGendoc) << "enter RTIG::processUpdateAttributeValues" << std::endl;

    my_auditServer << "ObjID = " << req->getObject() << ", Date = " << req->getDate().getTime();

    // Forward the call
    if (req->isDated()) {
// UAV with time
#if OLD
        my_federations.updateAttribute(req->getFederation(),
                                       req->getFederate(),
                                       req->getObject(),
                                       req->getAttributes(),
                                       req->getValues(),
                                       req->getAttributesSize(),
                                       req->getDate(),
                                       req->getLabel());
#else
        my_federations.searchFederation(req->getFederation())
            .updateAttributeValues(req->getFederate(),
                                   req->getObject(),
                                   req->getAttributes(),
                                   req->getValues(),
                                   req->getDate(),
                                   req->getLabel());
#endif
    }
    else {
// UAV without time
#if OLD
        my_federations.updateAttribute(req->getFederation(),
                                       req->getFederate(),
                                       req->getObject(),
                                       req->getAttributes(),
                                       req->getValues(),
                                       req->getAttributesSize(),
                                       req->getLabel());
#else
        my_federations.searchFederation(req->getFederation())
            .updateAttributeValues(req->getFederate(),
                                   req->getObject(),
                                   req->getAttributes(),
                                   req->getValues(),
                                   req->getAttributesSize(),
                                   req->getLabel());
#endif
    }

    // Building answer (Network Message)
    NM_Update_Attribute_Values rep;
    rep.setFederate(req->getFederate());
    rep.setObject(req->getObject());
    // Don't forget date, label and tag if provided in the request
    if (req->isDated()) {
        rep.setDate(req->getDate());
    }
    if (req->isLabelled()) {
        rep.setLabel(req->getLabel());
    }
    if (req->isTagged()) {
        rep.setTag(req->getTag());
    }

    rep.send(link, my_NM_msgBufSend); // send answer to RTIA
    Debug(G, pdGendoc) << "exit  RTIG::processUpdateAttributeValues" << std::endl;
}

// ----------------------------------------------------------------------------
// processSendInteraction
void RTIG::processSendInteraction(Socket* link, NM_Send_Interaction* req)
{
    Debug(G, pdGendoc) << "BEGIN ** SEND INTERACTION SERVICE **" << std::endl;
    Debug(G, pdGendoc) << "enter RTIG::processSendInteraction" << std::endl;

    // Building Value Array
    my_auditServer << "IntID = " << req->getInteractionClass() << ", date = " << req->getDate().getTime();
    if (req->isDated()) {
#if OLD
        my_federations.updateParameter(req->getFederation(),
                                       req->getFederate(),
                                       req->getInteractionClass(),
                                       req->getParameters(),
                                       req->getValues(),
                                       req->getParametersSize(),
                                       req->getDate(),
                                       req->getRegion(),
                                       req->getLabel());
#else
        my_federations.searchFederation(req->getFederation())
            .broadcastInteraction(req->getFederate(),
                                  req->getInteractionClass(),
                                  req->getParameters(),
                                  req->getValues(),
                                  req->getDate(),
                                  req->getRegion(),
                                  req->getLabel());
#endif
    }
    else {
#if OLD
        my_federations.updateParameter(req->getFederation(),
                                       req->getFederate(),
                                       req->getInteractionClass(),
                                       req->getParameters(),
                                       req->getValues(),
                                       req->getParametersSize(),
                                       req->getRegion(),
                                       req->getLabel());
#else
        my_federations.searchFederation(req->getFederation())
            .broadcastInteraction(req->getFederate(),
                                  req->getInteractionClass(),
                                  req->getParameters(),
                                  req->getValues(),
                                  req->getParametersSize(),
                                  req->getRegion(),
                                  req->getLabel());
#endif
    }

    Debug(D, pdDebug) << "Interaction " << req->getInteractionClass() << " parameters update completed" << std::endl;

    NM_Send_Interaction rep;
    rep.setFederate(req->getFederate());
    rep.setInteractionClass(req->getInteractionClass());
    // Don't forget label and tag
    rep.setLabel(req->getLabel());
    rep.setTag(req->getTag());
    Debug(G, pdGendoc) << "processSendInteraction===>write" << std::endl;
    rep.send(link, my_NM_msgBufSend); // send answer to RTIA

    Debug(G, pdGendoc) << "exit RTIG::processSendInteraction" << std::endl;
    Debug(G, pdGendoc) << "END ** SEND INTERACTION SERVICE **" << std::endl;
}

// ----------------------------------------------------------------------------
// processDeleteObject
void RTIG::processDeleteObject(Socket* link, NM_Delete_Object* req)
{
    Debug(G, pdGendoc) << "BEGIN ** DELETE OBJECT INSTANCE service **" << std::endl;
    Debug(G, pdGendoc) << "enter RTIG::processDeleteObject" << std::endl;
    my_auditServer << "Delete ObjID = " << req->getObject();

    if (req->isDated()) {
#if OLD
        my_federations.destroyObject(
            req->getFederation(), req->getFederate(), req->getObject(), req->getDate(), req->getLabel());
#else
        my_federations.searchFederation(req->getFederation())
            .deleteObject(req->getFederate(), req->getObject(), req->getDate(), req->getLabel());
#endif
    }
    else {
#if OLD
        my_federations.destroyObject(req->getFederation(), req->getFederate(), req->getObject(), req->getLabel());
#else
        my_federations.searchFederation(req->getFederation())
            .deleteObject(req->getFederate(), req->getObject(), req->getLabel());
#endif
    }

    Debug(D, pdRegister) << "Object # " << req->getObject() << " of Federation " << req->getFederation()
                         << " has been deleted" << std::endl;

    NM_Delete_Object rep;
    rep.setFederate(req->getFederate());
    rep.setObject(req->getObject());

    rep.send(link, my_NM_msgBufSend); // send answer to RTIA

    Debug(G, pdGendoc) << "exit RTIG::processDeleteObject" << std::endl;
    Debug(G, pdGendoc) << "END ** DELETE OBJECT INSTANCE **" << std::endl;
}

// ----------------------------------------------------------------------------
// processqueryAttributeOwnership
void RTIG::processQueryAttributeOwnership(Socket* link, NM_Query_Attribute_Ownership* req)
{
    Debug(D, pdDebug) << "Owner of Attribute " << req->getAttribute() << " of Object " << req->getObject() << std::endl;

    my_auditServer << "AttributeHandle = " << req->getAttribute();

#if OLD
    my_federations.searchOwner(req->getFederation(), req->getFederate(), req->getObject(), req->getAttribute());
#else
    my_federations.searchFederation(req->getFederation())
        .queryAttributeOwnership(req->getFederate(), req->getObject(), req->getAttribute());
#endif

    Debug(D, pdDebug) << "Owner of Attribute " << req->getAttribute() << " of Object " << req->getObject() << std::endl;

    NM_Query_Attribute_Ownership rep;
    rep.setFederate(req->getFederate());
    rep.setObject(req->getObject());

    rep.send(link, my_NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processAttributeOwnedByFederate
void RTIG::processAttributeOwnedByFederate(Socket* link, NM_Is_Attribute_Owned_By_Federate* req)
{
    NM_Is_Attribute_Owned_By_Federate rep;

    Debug(D, pdDebug) << "Owner of Attribute " << req->getAttribute() << " of Object " << req->getObject() << std::endl;

    my_auditServer << "AttributeHandle = " << req->getAttribute();

#if OLD
    if (my_federations.isOwner(req->getFederation(), req->getFederate(), req->getObject(), req->getAttribute())) {
#else
    if (my_federations.searchFederation(req->getFederation())
            .isOwner(req->getFederate(), req->getObject(), req->getAttribute())) {
#endif
        rep.setLabel("RTI_TRUE");
    }
    else {
        rep.setLabel("RTI_FALSE");
    }

    Debug(D, pdDebug) << "Owner of Attribute " << req->getAttribute() << " of Object " << req->getObject() << std::endl;

    rep.setFederate(req->getFederate());
    rep.setObject(req->getObject());
    rep.setAttribute(req->getAttribute());

    rep.send(link, my_NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processNegotiatedOwnershipDivestiture
void RTIG::processNegotiatedOwnershipDivestiture(Socket* link, NM_Negotiated_Attribute_Ownership_Divestiture* req)
{
    my_auditServer << "Object = " << req->getObject() << ", # of att. = " << req->getAttributesSize();
#if OLD
    my_federations.negotiateDivestiture(req->getFederation(),
                                        req->getFederate(),
                                        req->getObject(),
                                        req->getAttributes(),
                                        req->getAttributesSize(),
                                        req->getLabel());
#else
    my_federations.searchFederation(req->getFederation())
        .negotiateDivestiture(req->getFederate(), req->getObject(), req->getAttributes(), req->getLabel());
#endif

    Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                      << " negotiate "
                         "divestiture of object "
                      << req->getObject() << std::endl;

    NM_Negotiated_Attribute_Ownership_Divestiture rep;
    rep.setFederate(req->getFederate());
    rep.setObject(req->getObject());

    rep.send(link, my_NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processAcquisitionIfAvailable
void RTIG::processAcquisitionIfAvailable(Socket* link, NM_Attribute_Ownership_Acquisition_If_Available* req)
{
    my_auditServer << "Object = " << req->getObject() << ", # of att. = " << req->getAttributesSize();

#if OLD
    my_federations.acquireIfAvailable(
        req->getFederation(), req->getFederate(), req->getObject(), req->getAttributes(), req->getAttributesSize());
#else
    my_federations.searchFederation(req->getFederation())
        .acquireIfAvailable(req->getFederate(), req->getObject(), req->getAttributes());
#endif

    Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                      << " acquisitionIfAvailable "
                         "of object "
                      << req->getObject() << std::endl;

    NM_Attribute_Ownership_Acquisition_If_Available rep;
    rep.setFederate(req->getFederate());
    rep.setObject(req->getObject());

    rep.send(link, my_NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processUnconditionalDivestiture
void RTIG::processUnconditionalDivestiture(Socket* link, NM_Unconditional_Attribute_Ownership_Divestiture* req)
{
    my_auditServer << "Object = " << req->getObject() << ", # of att. = " << req->getAttributesSize();

#if OLD
    my_federations.divest(
        req->getFederation(), req->getFederate(), req->getObject(), req->getAttributes(), req->getAttributesSize());
#else
    my_federations.searchFederation(req->getFederation())
        .divest(req->getFederate(), req->getObject(), req->getAttributes());
#endif

    Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                      << " UnconditionalDivestiture "
                         "of object "
                      << req->getObject() << std::endl;

    NM_Unconditional_Attribute_Ownership_Divestiture rep;
    rep.setFederate(req->getFederate());
    rep.setObject(req->getObject());

    rep.send(link, my_NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processOwnershipAcquisition
void RTIG::processOwnershipAcquisition(Socket* link, NM_Attribute_Ownership_Acquisition* req)
{
    my_auditServer << "Object = " << req->getObject() << ", # of att. = " << req->getAttributesSize();

#if OLD
    my_federations.acquire(req->getFederation(),
                           req->getFederate(),
                           req->getObject(),
                           req->getAttributes(),
                           req->getAttributesSize(),
                           req->getLabel());
#else
    my_federations.searchFederation(req->getFederation())
        .acquire(req->getFederate(), req->getObject(), req->getAttributes(), req->getLabel());
#endif

    Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                      << " ownership acquisition of object " << req->getObject() << std::endl;

    NM_Attribute_Ownership_Acquisition rep;
    rep.setFederate(req->getFederate());
    rep.setObject(req->getObject());
    rep.setAttributesSize(0);

    rep.send(link, my_NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processCancelNegotiatedDivestiture
void RTIG::processCancelNegotiatedDivestiture(Socket* link, NM_Cancel_Negotiated_Attribute_Ownership_Divestiture* req)
{
    my_auditServer << "Object = " << req->getObject() << ", # of att. = " << req->getAttributesSize();

#if OLD
    my_federations.cancelDivestiture(
        req->getFederation(), req->getFederate(), req->getObject(), req->getAttributes(), req->getAttributesSize());
#else
    my_federations.searchFederation(req->getFederation())
        .cancelDivestiture(req->getFederate(), req->getObject(), req->getAttributes());
#endif

    Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                      << " cancel negotiate "
                         "divestiture of object "
                      << req->getObject() << std::endl;

    NM_Cancel_Negotiated_Attribute_Ownership_Divestiture rep;
    rep.setFederate(req->getFederate());
    rep.setObject(req->getObject());

    rep.send(link, my_NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processReleaseResponse
void RTIG::processReleaseResponse(Socket* link, NM_Attribute_Ownership_Release_Response* req)
{
    my_auditServer << "Object = " << req->getObject() << ", # of att. = " << req->getAttributesSize();

#if OLD
    AttributeHandleSet* attributes = my_federations.respondRelease(
        req->getFederation(), req->getFederate(), req->getObject(), req->getAttributes(), req->getAttributesSize());
#else
    AttributeHandleSet* attributes = my_federations.searchFederation(req->getFederation())
                                         .respondRelease(req->getFederate(), req->getObject(), req->getAttributes());
#endif

    Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                      << " release response "
                         "of object "
                      << req->getObject() << std::endl;

    NM_Attribute_Ownership_Release_Response rep;
    rep.setAttributesSize(attributes->size());

    for (uint32_t i = 0; i < attributes->size(); ++i) {
        rep.setAttributes(attributes->getHandle(i), i);
    }

    rep.setFederate(req->getFederate());
    rep.setObject(req->getObject());

    rep.send(link, my_NM_msgBufSend); // Send answer to RTIA
}

// ----------------------------------------------------------------------------
// processCancelAcquisition
void RTIG::processCancelAcquisition(Socket* link, NM_Cancel_Attribute_Ownership_Acquisition* req)
{
    my_auditServer << "Object = " << req->getObject() << ", # of att. = " << req->getAttributesSize();

#if OLD
    my_federations.cancelAcquisition(
        req->getFederation(), req->getFederate(), req->getObject(), req->getAttributes(), req->getAttributesSize());
#else
    my_federations.searchFederation(req->getFederation())
        .cancelAcquisition(req->getFederate(), req->getObject(), req->getAttributes());
#endif

    Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                      << " release response of object " << req->getObject() << std::endl;

    NM_Cancel_Attribute_Ownership_Acquisition rep;
    rep.setFederate(req->getFederate());
    rep.setObject(req->getObject());

    rep.send(link, my_NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processCreateRegion
void RTIG::processCreateRegion(Socket* link, NM_DDM_Create_Region* req)
{
    // TODO: audit...

    NM_DDM_Create_Region rep;
#if OLD
    rep.setRegion(
        my_federations.createRegion(req->getFederation(), req->getFederate(), req->getSpace(), req->getNbExtents()));

#else
    rep.setRegion(my_federations.searchFederation(req->getFederation())
                      .createRegion(req->getFederate(), req->getSpace(), req->getNbExtents()));
#endif

    Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                      << " creates region " << rep.getRegion() << std::endl;

    rep.setFederate(req->getFederate());
    rep.send(link, my_NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processModifyRegion
void RTIG::processModifyRegion(Socket* link, NM_DDM_Modify_Region* req)
{
    Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                      << " modifies region " << req->getRegion() << std::endl;

#if OLD
    my_federations.modifyRegion(req->getFederation(), req->getFederate(), req->getRegion(), req->getExtents());

#else
    my_federations.searchFederation(req->getFederation())
        .modifyRegion(req->getFederate(), req->getRegion(), req->getExtents());
#endif

    NM_DDM_Modify_Region rep;
    rep.setFederate(req->getFederate());
    rep.send(link, my_NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processDeleteRegion
void RTIG::processDeleteRegion(Socket* link, NM_DDM_Delete_Region* req)
{
    // TODO: audit...

    Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                      << " deletes region " << req->getRegion() << std::endl;

#if OLD
    my_federations.deleteRegion(req->getFederation(), req->getFederate(), req->getRegion());

#else
    my_federations.searchFederation(req->getFederation()).deleteRegion(req->getFederate(), req->getRegion());
#endif

    NM_DDM_Delete_Region rep;

    rep.setFederate(req->getFederate());
    rep.setRegion(req->getRegion());
    rep.send(link, my_NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processAssociateRegion
void RTIG::processAssociateRegion(Socket* link, NM_DDM_Associate_Region* req)
{
    // TODO: audit...

    Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                      << " associates region " << req->getRegion() << " to some attributes of object "
                      << req->getObject() << std::endl;

#if OLD
    my_federations.associateRegion(req->getFederation(),
                                   req->getFederate(),
                                   req->getObject(),
                                   req->getRegion(),
                                   req->getAttributesSize(),
                                   req->getAttributes());

#else
    my_federations.searchFederation(req->getFederation())
        .associateRegion(req->getFederate(), req->getObject(), req->getRegion(), req->getAttributes());
#endif

    NM_DDM_Associate_Region rep;
    rep.setFederate(req->getFederate());
    rep.send(link, my_NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processUnassociateRegion
void RTIG::processUnassociateRegion(Socket* link, NM_DDM_Unassociate_Region* req)
{
// TODO: audit...

#if OLD
    my_federations.unassociateRegion(req->getFederation(), req->getFederate(), req->getObject(), req->getRegion());

#else
    my_federations.searchFederation(req->getFederation())
        .unassociateRegion(req->getFederate(), req->getObject(), req->getRegion());
#endif

    Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                      << " associates region " << req->getRegion() << " from object " << req->getObject() << std::endl;

    NM_DDM_Unassociate_Region rep;
    rep.setFederate(req->getFederate());
    rep.send(link, my_NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processSubscribeAttributes
void RTIG::processSubscribeAttributesWR(Socket* link, NM_DDM_Subscribe_Attributes* req)
{
    // TODO: audit...
    Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                      << " subscribes with region " << req->getRegion() << " to some attributes of class "
                      << req->getObjectClass() << std::endl;

#if OLD
    my_federations.subscribeAttributesWR(req->getFederation(),
                                         req->getFederate(),
                                         req->getObjectClass(),
                                         req->getRegion(),
                                         req->getAttributesSize(),
                                         req->getAttributes());

#else
    my_federations.searchFederation(req->getFederation())
        .subscribeAttributesWR(req->getFederate(), req->getObjectClass(), req->getRegion(), req->getAttributes());
#endif

    NM_DDM_Subscribe_Attributes rep;
    rep.setFederate(req->getFederate());
    rep.setObjectClass(req->getObjectClass());
    rep.send(link, my_NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processUnsubscribeAttributes
void RTIG::processUnsubscribeAttributesWR(Socket* link, NM_DDM_Unsubscribe_Attributes* req)
{
    // TODO: audit...
    Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                      << " unsubscribes with region " << req->getRegion() << " from object class "
                      << req->getObjectClass() << std::endl;

#if OLD
    my_federations.unsubscribeAttributesWR(
        req->getFederation(), req->getFederate(), req->getObjectClass(), req->getRegion());

#else
    my_federations.searchFederation(req->getFederation())
        .unsubscribeAttributesWR(req->getFederate(), req->getObjectClass(), req->getRegion());
#endif

    NM_DDM_Unsubscribe_Attributes rep;
    rep.setFederate(req->getFederate());
    rep.send(link, my_NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processSubscribeInteractions
void RTIG::processSubscribeInteractionWR(Socket* link, NM_DDM_Subscribe_Interaction* req)
{
// TODO: audit...

#if OLD
    my_federations.subscribeInteractionWR(
        req->getFederation(), req->getFederate(), req->getInteractionClass(), req->getRegion());

#else
    my_federations.searchFederation(req->getFederation())
        .subscribeInteractionWR(req->getFederate(), req->getInteractionClass(), req->getRegion());
#endif

    Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                      << " subscribes with region " << req->getRegion() << " to interaction class "
                      << req->getInteractionClass() << std::endl;

    NM_DDM_Subscribe_Interaction rep;
    rep.setFederate(req->getFederate());
    rep.send(link, my_NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processUnsubscribeInteractions
void RTIG::processUnsubscribeInteractionWR(Socket* link, NM_DDM_Unsubscribe_Interaction* req)
{
// TODO: audit...

#if OLD
    my_federations.unsubscribeInteractionWR(
        req->getFederation(), req->getFederate(), req->getInteractionClass(), req->getRegion());

#else
    my_federations.searchFederation(req->getFederation())
        .unsubscribeInteractionWR(req->getFederate(), req->getInteractionClass(), req->getRegion());
#endif

    Debug(D, pdDebug) << "Federate " << req->getFederate() << " of Federation " << req->getFederation()
                      << " unsubscribes with region " << req->getRegion() << " from interaction class "
                      << req->getInteractionClass() << std::endl;

    NM_DDM_Unsubscribe_Interaction rep;
    rep.setFederate(req->getFederate());
    rep.send(link, my_NM_msgBufSend);
}

// ----------------------------------------------------------------------------
// processRegisterObjectWithRegion
void RTIG::processRegisterObjectWithRegion(Socket* link, NM_DDM_Register_Object* req)
{
    NM_DDM_Register_Object rep;
// FIXME bug #9869
// When we were passed a set of region
// we should register object for each region
// the trouble comes from the fact that
// there is both
//     - req->region  (coming from NetworkMessage::region)
//     - req->regions (coming from BasicMessage::regions)
// would be nice to sort those thing out.
#if OLD
    rep.setObject(my_federations.registerObjectWithRegion(req->getFederation(),
                                                          req->getFederate(),
                                                          req->getObjectClass(),
                                                          req->getLabel(),
                                                          req->getRegion(),
                                                          req->getAttributesSize(),
                                                          req->getAttributes()));

#else
    my_federations.searchFederation(req->getFederation())
        .registerObjectWithRegion(
            req->getFederate(), req->getObjectClass(), req->getLabel(), req->getRegion(), req->getAttributes());
#endif

    Debug(D, pdRegister) << "Object \"" << req->getLabel() << "\" of Federate " << req->getFederate()
                         << " has been registered under ID " << rep.getObject() << std::endl;

    rep.setFederate(req->getFederate());
    rep.send(link, my_NM_msgBufSend); // Send answer to RTIA
}

// ----------------------------------------------------------------------------
// processRequestObjectAttributeValueUpdate
void RTIG::processRequestObjectAttributeValueUpdate(Socket* link, NM_Request_Object_Attribute_Value_Update* request)
{
    NM_Request_Object_Attribute_Value_Update answer;
    Debug(G, pdGendoc) << "enter RTIG::processRequestObjectAttributeValueUpdate" << std::endl;
    Debug(G, pdGendoc) << "BEGIN ** REQUEST OBJECT ATTRIBUTE VALUE UPDATE **" << std::endl;

    my_auditServer << "ObjID = " << request->getObject();

    // We have to do verifications about this object and we need owner
    answer.setException(Exception::Type::NO_EXCEPTION);
    try {
// While searching for the federate owner we will send
// a NM_Provide_Attribute_Value_Update
// (see Federation::requestObjectOwner)
#if OLD
        (void) my_federations.requestObjectOwner(request->getFederation(),
                                                 request->getFederate(),
                                                 request->getObject(),
                                                 request->getAttributes(),
                                                 request->getAttributesSize());

#else
        DISCARD_RETURN my_federations.searchFederation(request->getFederation())
            .requestObjectOwner(request->getFederate(), request->getObject(), request->getAttributes());
#endif
    }
    catch (ObjectNotKnown& e) {
        answer.setException(e.type(), e.reason());
    }
    catch (FederationExecutionDoesNotExist& e) {
        answer.setException(e.type(), e.reason());
    }
    catch (RTIinternalError& e) {
        answer.setException(e.type(), e.reason());
    }

    answer.setFederate(request->getFederate());
    answer.setObject(request->getObject());

    answer.send(link, my_NM_msgBufSend); // Send answer to RTIA
    Debug(G, pdGendoc) << "exit  RTIG::processRequestObjectAttributeValueUpdate" << std::endl;
    Debug(G, pdGendoc) << "END   ** REQUEST OBJECT ATTRIBUTE VALUE UPDATE **" << std::endl;
}

// ----------------------------------------------------------------------------
// processRequestClassAttributeValueUpdate
void RTIG::processRequestClassAttributeValueUpdate(Socket* link, NM_Request_Class_Attribute_Value_Update* request)
{
    NM_Request_Class_Attribute_Value_Update answer;

    Debug(G, pdGendoc) << "enter RTIG::processRequestClassAttributeValueUpdate" << std::endl;
    Debug(G, pdGendoc) << "BEGIN ** REQUEST CLASS ATTRIBUTE VALUE UPDATE **" << std::endl;

    answer.setException(Exception::Type::NO_EXCEPTION);
    try {
#if OLD
        my_federations.requestClassAttributeValueUpdate(request->getFederation(),
                                                        request->getFederate(),
                                                        request->getObjectClass(),
                                                        request->getAttributes(),
                                                        request->getAttributesSize());

#else
        my_federations.searchFederation(request->getFederation())
            .requestClassAttributeValueUpdate(
                request->getFederate(), request->getObjectClass(), request->getAttributes());
#endif
    }
    catch (ObjectClassNotDefined& e) {
        answer.setException(e.type(), e.reason());
    }
    catch (FederationExecutionDoesNotExist& e) {
        answer.setException(e.type(), e.reason());
    }
    catch (RTIinternalError& e) {
        answer.setException(e.type(), e.reason());
    }

    answer.setFederate(request->getFederate());
    answer.setObjectClass(request->getObjectClass());

    answer.send(link, my_NM_msgBufSend); // Send answer to RTIA

    Debug(G, pdGendoc) << "exit  RTIG::processRequestClassAttributeValueUpdate" << std::endl;
    Debug(G, pdGendoc) << "END   ** REQUEST CLASS  ATTRIBUTE VALUE UPDATE **" << std::endl;
}
}
} // namespace certi/rtig
