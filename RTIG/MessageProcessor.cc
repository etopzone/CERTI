#include "MessageProcessor.hh"

#include <iostream>

#include "AuditFile.hh"
#include "Exception.hh"
#include "FedTimeD.hh"
#include "Federate.hh"
#include "Federation.hh"
#include "FederationsList.hh"
#include "GAV.hh"
#include "HandleManager.hh"
#include "NetworkMessage.hh"
#include "PrettyDebug.hh"
#include "Socket.hh"
#include "SocketServer.hh"
#include "SocketTCP.hh"
#include "certi.hh"

using std::endl;

namespace certi {
namespace rtig {

static PrettyDebug D("RTIG", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);
static PrettyDebug DNULL("RTIG_NULLMSG", "[RTIG NULL MSG]");

MessageProcessor::MessageProcessor(AuditFile& audit_server,
                                   SocketServer& socket_server,
                                   HandleManager<Handle>& handle_generator,
                                   FederationsList& federations)
    : my_auditServer{audit_server}
    , my_socketServer{socket_server}
    , my_federationHandleGenerator{handle_generator}
    , my_federations{federations}
{
}

MessageProcessor::Responses MessageProcessor::processEvent(MessageEvent<NetworkMessage> request)
{
//std::cout << __PRETTY_FUNCTION__ << " type (" << request.message()->getMessageName() << ")" << std::endl;

#define BASIC_CASE(MessageType, MessageClass)                                                                          \
                                                                                                                       \
    case NetworkMessage::MessageType:                                                                                  \
        return process(MessageEvent<MessageClass>{std::move(request)})

#define FALLTHROUGH(MessageType) case NetworkMessage::MessageType: // fall through

    switch (request.message()->getMessageType()) {
        BASIC_CASE(MESSAGE_NULL, NM_Message_Null);
        BASIC_CASE(RESIGN_FEDERATION_EXECUTION, NM_Resign_Federation_Execution);
        BASIC_CASE(MESSAGE_NULL_PRIME, NM_Message_Null_Prime);
        BASIC_CASE(UPDATE_ATTRIBUTE_VALUES, NM_Update_Attribute_Values);
        BASIC_CASE(SEND_INTERACTION, NM_Send_Interaction);
        BASIC_CASE(CREATE_FEDERATION_EXECUTION, NM_Create_Federation_Execution);
        BASIC_CASE(DESTROY_FEDERATION_EXECUTION, NM_Destroy_Federation_Execution);
        BASIC_CASE(JOIN_FEDERATION_EXECUTION, NM_Join_Federation_Execution);
        BASIC_CASE(REGISTER_FEDERATION_SYNCHRONIZATION_POINT, NM_Register_Federation_Synchronization_Point);
        BASIC_CASE(SYNCHRONIZATION_POINT_ACHIEVED, NM_Synchronization_Point_Achieved);
        BASIC_CASE(REQUEST_FEDERATION_SAVE, NM_Request_Federation_Save);
        BASIC_CASE(FEDERATE_SAVE_BEGUN, NM_Federate_Save_Begun);
        BASIC_CASE(FEDERATE_SAVE_COMPLETE, NM_Federate_Save_Complete);
        BASIC_CASE(FEDERATE_SAVE_NOT_COMPLETE, NM_Federate_Save_Not_Complete);
        BASIC_CASE(REQUEST_FEDERATION_RESTORE, NM_Request_Federation_Restore);
        BASIC_CASE(FEDERATE_RESTORE_COMPLETE, NM_Federate_Restore_Complete);
        BASIC_CASE(FEDERATE_RESTORE_NOT_COMPLETE, NM_Federate_Restore_Not_Complete);
        BASIC_CASE(REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE, NM_Request_Object_Attribute_Value_Update);
        BASIC_CASE(REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE, NM_Request_Class_Attribute_Value_Update);
        BASIC_CASE(SET_TIME_REGULATING, NM_Set_Time_Regulating);
        BASIC_CASE(SET_TIME_CONSTRAINED, NM_Set_Time_Constrained);
        BASIC_CASE(PUBLISH_OBJECT_CLASS, NM_Publish_Object_Class);
        BASIC_CASE(UNPUBLISH_OBJECT_CLASS, NM_Unpublish_Object_Class);
        BASIC_CASE(PUBLISH_INTERACTION_CLASS, NM_Publish_Interaction_Class);
        BASIC_CASE(UNPUBLISH_INTERACTION_CLASS, NM_Unpublish_Interaction_Class);
        BASIC_CASE(SUBSCRIBE_OBJECT_CLASS, NM_Subscribe_Object_Class);
        BASIC_CASE(UNSUBSCRIBE_OBJECT_CLASS, NM_Unsubscribe_Object_Class);
        BASIC_CASE(SUBSCRIBE_INTERACTION_CLASS, NM_Subscribe_Interaction_Class);
        BASIC_CASE(UNSUBSCRIBE_INTERACTION_CLASS, NM_Unsubscribe_Interaction_Class);
        BASIC_CASE(SET_CLASS_RELEVANCE_ADVISORY_SWITCH, NM_Set_Class_Relevance_Advisory_Switch);
        BASIC_CASE(SET_INTERACTION_RELEVANCE_ADVISORY_SWITCH, NM_Set_Interaction_Relevance_Advisory_Switch);
        BASIC_CASE(SET_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH, NM_Set_Attribute_Relevance_Advisory_Switch);
        BASIC_CASE(SET_ATTRIBUTE_SCOPE_ADVISORY_SWITCH, NM_Set_Attribute_Scope_Advisory_Switch);
        BASIC_CASE(RESERVE_OBJECT_INSTANCE_NAME, NM_Reserve_Object_Instance_Name);
        BASIC_CASE(REGISTER_OBJECT, NM_Register_Object);
        BASIC_CASE(DELETE_OBJECT, NM_Delete_Object);
        BASIC_CASE(IS_ATTRIBUTE_OWNED_BY_FEDERATE, NM_Is_Attribute_Owned_By_Federate);
        BASIC_CASE(QUERY_ATTRIBUTE_OWNERSHIP, NM_Query_Attribute_Ownership);
        BASIC_CASE(NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE, NM_Negotiated_Attribute_Ownership_Divestiture);
        BASIC_CASE(ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE, NM_Attribute_Ownership_Acquisition_If_Available);
        BASIC_CASE(UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE, NM_Unconditional_Attribute_Ownership_Divestiture);
        BASIC_CASE(ATTRIBUTE_OWNERSHIP_ACQUISITION, NM_Attribute_Ownership_Acquisition);
        BASIC_CASE(CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
                   NM_Cancel_Negotiated_Attribute_Ownership_Divestiture);
        BASIC_CASE(ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE, NM_Attribute_Ownership_Release_Response);
        BASIC_CASE(CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION, NM_Cancel_Attribute_Ownership_Acquisition);
        BASIC_CASE(DDM_CREATE_REGION, NM_DDM_Create_Region);
        BASIC_CASE(DDM_MODIFY_REGION, NM_DDM_Modify_Region);
        BASIC_CASE(DDM_DELETE_REGION, NM_DDM_Delete_Region);
        BASIC_CASE(DDM_REGISTER_OBJECT, NM_DDM_Register_Object);
        BASIC_CASE(DDM_ASSOCIATE_REGION, NM_DDM_Associate_Region);
        BASIC_CASE(DDM_UNASSOCIATE_REGION, NM_DDM_Unassociate_Region);
        BASIC_CASE(DDM_SUBSCRIBE_ATTRIBUTES, NM_DDM_Subscribe_Attributes);
        BASIC_CASE(DDM_UNSUBSCRIBE_ATTRIBUTES, NM_DDM_Unsubscribe_Attributes);
        BASIC_CASE(DDM_SUBSCRIBE_INTERACTION, NM_DDM_Subscribe_Interaction);
        BASIC_CASE(DDM_UNSUBSCRIBE_INTERACTION, NM_DDM_Unsubscribe_Interaction);

    case NetworkMessage::CLOSE_CONNEXION:
        throw RTIinternalError("Close connection: Should have been handled by RTIG");

    default:
        // FIXME: Should treat other cases CHANGE_*_ORDER/TRANSPORT_TYPE
        Debug(D, pdError) << "MessageProcessor::processEvent(): unknown type " << request.message()->getMessageType()
                          << std::endl;
        throw RTIinternalError("Unknown Message Type");
    }
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Create_Federation_Execution> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    my_auditServer.setLevel(9);
    {
        std::string federation = request.message()->getFederationName();
        std::string FEDid = request.message()->getFEDid();
        NM_Create_Federation_Execution rep; // rep for answer to RTIA

        Debug(G, pdGendoc) << "enter RTIG::processCreateFederation" << endl;
        Debug(G, pdGendoc) << "BEGIN ** CREATE FEDERATION SERVICE **" << endl;

        if (federation.length() == 0) {
            Debug(G, pdGendoc) << "exit  RTIG::processCreateFederation on exception RTIinternalError" << endl;
            throw RTIinternalError("Invalid Federation Name");
        }

        my_auditServer << "Federation Name : " << federation;
        Handle h = my_federationHandleGenerator.provide();

#ifdef FEDERATION_USES_MULTICAST
        // multicast base address
        unsigned long base_adr_mc = inet_addr(ADRESSE_MULTICAST);
        SocketMC* com_mc = NULL;

        // creer la communication multicast
        com_mc = new SocketMC();
        if (com_mc == NULL) {
            Debug(D, pdExcept) << "Unable to allocate Multicast socket" << endl;
            Debug(G, pdGendoc) << "exit  RTIG::processCreateFederation on exception RTIinternalError" << endl;
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

        Debug(G, pdGendoc) << "processCreateFederation===>write answer to RTIA" << endl;

        rep.send(request.socket(), my_messageBuffer); // Send answer to RTIA

        Debug(D, pdInit) << "Federation \"" << federation << "\" created with Handle " << rep.getFederation() << endl;

        Debug(G, pdGendoc) << "END ** CREATE FEDERATION SERVICE **" << endl;
        Debug(G, pdGendoc) << "exit RTIG::processCreateFederation" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Join_Federation_Execution> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    my_auditServer.setLevel(9);
    {
        std::string federation = request.message()->getFederationName();
        std::string federate = request.message()->getFederateName();

        unsigned int peer = request.message()->getBestEffortPeer();
        unsigned long address = request.message()->getBestEffortAddress();

        FederateHandle num_federe;

        int nb_regulateurs;
        int nb_federes;
        bool pause;

        Debug(G, pdGendoc) << "BEGIN ** JOIN FEDERATION SERVICE **" << endl;
        Debug(G, pdGendoc) << "enter RTIG::processJoinFederation" << endl;

        if (federation.empty() || federate.empty())
            throw RTIinternalError("Invalid Federation/Federate Name");

        my_auditServer << "Federate \"" << federate << "\" joins Federation \"" << federation << "\"";

        Handle num_federation = my_federations.getFederationHandle(federation);

        // Need to dump the FOM into that
        NM_Join_Federation_Execution rep;
        try {
            num_federe
                = my_federations.addFederate(num_federation, federate, static_cast<SocketTCP*>(request.socket()), rep);
        }
        catch (FederateAlreadyExecutionMember& e) {
            Debug(G, pdGendoc) << "exit RTIG::processJoinFederation on Error" << endl;
            Debug(G, pdGendoc) << "END ** JOIN FEDERATION (BAD) SERVICE **" << endl;
            // Prepare answer about JoinFederationExecution
            NM_Join_Federation_Execution rep;
            rep.setException(e.type(),
                             certi::stringize() << "Federate with same name <" << federate
                                                << "> has already joined the federation");

            Debug(G, pdGendoc) << "processJoinFederation==>Answer to RTIA JFE ERROR " << rep.getExceptionReason()
                               << endl;

            rep.send(request.socket(), my_messageBuffer);
            return {};
        }

#ifdef FEDERATION_USES_MULTICAST
        SocketMC* com_mc = NULL;

        federations.info(num_federation, nb_federes, nb_regulateurs, pause, com_mc);
        assert(com_mc != NULL);
#else
        my_federations.info(num_federation, nb_federes, nb_regulateurs, pause);
#endif

        // Store Federate <->Socket reference.
        my_socketServer.setReferences(request.socket()->returnSocket(), num_federation, num_federe, address, peer);

        my_auditServer << "(" << num_federation << ")with handle " << num_federe << ". Socket "
                       << int(request.socket()->returnSocket());

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

        Debug(D, pdInit) << "Federate \"" << federate << "\" has joined Federation " << num_federation
                         << " under handle " << num_federe << endl;

        // Send answer
        rep.send(request.socket(), my_messageBuffer);

        Debug(G, pdGendoc) << "exit RTIG::processJoinFederation" << endl;
        Debug(G, pdGendoc) << "END ** JOIN FEDERATION SERVICE **" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Resign_Federation_Execution> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    auto federation = request.message()->getFederation();
    auto federate = request.message()->getFederate();
    Debug(D, pdTrace) << "Federate (" << request.message()->getFederate() << ") leaves federation ("
                      << request.message()->getFederation() << ")" << std::endl;
    my_auditServer.setLevel(9);
    {
        NM_Resign_Federation_Execution reponse;

        Debug(G, pdGendoc) << "BEGIN ** RESIGN FEDERATION SERVICE **" << endl;
        Debug(G, pdGendoc) << "enter RTIG::processResignFederation" << endl;

        my_federations.searchFederation(federation).remove(federate);

        Debug(D, pdInit) << "Federate " << federate << " is resigning from federation " << federation << endl;

        my_auditServer << "Federate " << federate << " resign federation(" << federation << ")";

        // Send answer to RTIA
        reponse.setFederate(federate);
        reponse.setFederation(federation);
        reponse.send(request.socket(), my_messageBuffer);

        Debug(G, pdGendoc) << "exit RTIG::processResignFederation" << endl;
        Debug(G, pdGendoc) << "END ** RESIGN FEDERATION SERVICE **" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Destroy_Federation_Execution> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    my_auditServer.setLevel(9);
    {
        NM_Destroy_Federation_Execution rep;

        std::string federation = request.message()->getFederationName();

        Debug(G, pdGendoc) << "enter RTIG::processDestroyFederation" << endl;
        Debug(G, pdGendoc) << "BEGIN ** DESTROY FEDERATION SERVICE **" << endl;

        if (federation.length() == 0)
            throw RTIinternalError("Invalid Federation Name");

        Handle num_federation = my_federations.getFederationHandle(federation);
        // May throw RTIinternalError
        //           FederatesCurrentlyJoined
        //           FederationExecutionDoesNotExist
        try {
            my_federations.destroyFederation(num_federation);
            // Here delete federation (num_federation) has been done
            my_federationHandleGenerator.free(num_federation);
            Debug(D, pdInit) << "Federation \"" << federation << "\" has been destroyed" << endl;
        }
        catch (RTIinternalError& e) {
            std::cerr << "ERROR : " << e.name() << "  reason :" << e.reason() << endl;
            rep.setException(e.type(), e.reason());
        }
        catch (FederationExecutionDoesNotExist& e) {
            std::cerr << "ERROR : " << e.name() << "  reason :" << e.reason() << endl;
            rep.setException(e.type(), e.reason());
        }
        catch (FederatesCurrentlyJoined& e) {
            std::cerr << "ERROR : " << e.name() << "  reason :" << e.reason() << endl;
            rep.setException(e.type(), e.reason());
        }
        catch (Exception& e) {
            std::cerr << "ERROR : " << e.name() << "  reason :" << e.reason() << endl;
        }

        rep.setFederate(request.message()->getFederate());
        rep.setFederationName(request.message()->getFederationName());
        if (rep.getException() == Exception::Type::NO_EXCEPTION) {
            my_auditServer << "Federation Name \"" << federation << "\"(" << num_federation << ") destroyed";
        }
        Debug(G, pdGendoc) << "processDestroyFederation===>write DFE to RTIA" << endl;

        rep.send(request.socket(), my_messageBuffer);

        Debug(G, pdGendoc) << "END ** DESTROY FEDERATION SERVICE **" << endl;
        Debug(G, pdGendoc) << "exit RTIG::processDestroyFederation" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Set_Class_Relevance_Advisory_Switch> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "setClassRelevanceAdvisorySwitch" << std::endl;
    my_auditServer.setLevel(6);
    {
        NM_Set_Class_Relevance_Advisory_Switch rep;

        Debug(G, pdGendoc) << "enter RTIG::processSetClassRelevanceAdvisorySwitch" << endl;
        Debug(G, pdGendoc) << "BEGIN ** SET CLASS RELEVANCE ADVISORY SWITCH **" << endl;

        if (request.message()->isClassRelevanceAdvisorySwitchOn()) {
            my_auditServer << "ON";
            my_federations.searchFederation(request.message()->getFederation())
                .getFederate(request.message()->getFederate())
                .setClassRelevanceAdvisorySwitch(true);
            Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                             << request.message()->getFederation() << " sets AttributeRelevanceAdvisorySwitch" << endl;
        }
        else {
            my_auditServer << "OFF";
            my_federations.searchFederation(request.message()->getFederation())
                .getFederate(request.message()->getFederate())
                .setClassRelevanceAdvisorySwitch(false);
            Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                             << request.message()->getFederation() << " clears AttributeRelevanceAdvisorySwitch"
                             << endl;
        }

        rep.send(request.socket(), my_messageBuffer);

        Debug(G, pdGendoc) << "END ** SET CLASS RELEVANCE ADVISORY SWITCH **" << endl;
        Debug(G, pdGendoc) << "exit RTIG::processSetClassRelevanceAdvisorySwitch" << endl;
    }
    return {};
}

MessageProcessor::Responses
MessageProcessor::process(MessageEvent<NM_Set_Interaction_Relevance_Advisory_Switch> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "setInteractionRelevanceAdvisorySwitch" << std::endl;
    my_auditServer.setLevel(6);
    {
        NM_Set_Interaction_Relevance_Advisory_Switch rep;

        Debug(G, pdGendoc) << "enter RTIG::processSetInteractionRelevanceAdvisorySwitch" << endl;
        Debug(G, pdGendoc) << "BEGIN ** SET INTERACTION RELEVANCE ADVISORY SWITCH **" << endl;

        if (request.message()->isInteractionRelevanceAdvisorySwitchOn()) {
            my_auditServer << "ON";
            my_federations.searchFederation(request.message()->getFederation())
                .getFederate(request.message()->getFederate())
                .setInteractionRelevanceAdvisorySwitch(true);
            Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                             << request.message()->getFederation() << " sets InteractionRelevanceAdvisorySwitch"
                             << endl;
        }
        else {
            my_auditServer << "OFF";
            my_federations.searchFederation(request.message()->getFederation())
                .getFederate(request.message()->getFederate())
                .setInteractionRelevanceAdvisorySwitch(false);
            Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                             << request.message()->getFederation() << " clears InteractionRelevanceAdvisorySwitch"
                             << endl;
        }

        rep.send(request.socket(), my_messageBuffer);

        Debug(G, pdGendoc) << "END ** SET INTERACTION RELEVANCE ADVISORY SWITCH **" << endl;
        Debug(G, pdGendoc) << "exit RTIG::processSetInteractionRelevanceAdvisorySwitch" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Set_Attribute_Relevance_Advisory_Switch> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "setAttributeRelevanceAdvisorySwitch" << std::endl;
    my_auditServer.setLevel(6);
    {
        NM_Set_Attribute_Relevance_Advisory_Switch rep;

        Debug(G, pdGendoc) << "enter RTIG::processSetAttributeRelevanceAdvisorySwitch" << endl;
        Debug(G, pdGendoc) << "BEGIN ** SET ATTRIBUTE RELEVANCE ADVISORY SWITCH **" << endl;

        if (request.message()->isAttributeRelevanceAdvisorySwitchOn()) {
            my_auditServer << "ON";
            my_federations.searchFederation(request.message()->getFederation())
                .getFederate(request.message()->getFederate())
                .setAttributeRelevanceAdvisorySwitch(true);
            Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                             << request.message()->getFederation() << " sets AttributeRelevanceAdvisorySwitch" << endl;
        }
        else {
            my_auditServer << "OFF";
            my_federations.searchFederation(request.message()->getFederation())
                .getFederate(request.message()->getFederate())
                .setAttributeRelevanceAdvisorySwitch(false);
            Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                             << request.message()->getFederation() << " clears AttributeRelevanceAdvisorySwitch"
                             << endl;
        }

        rep.send(request.socket(), my_messageBuffer);

        Debug(G, pdGendoc) << "END ** SET ATTRIBUTE RELEVANCE ADVISORY SWITCH **" << endl;
        Debug(G, pdGendoc) << "exit RTIG::processSetAttributeRelevanceAdvisorySwitch" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Set_Attribute_Scope_Advisory_Switch> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "setAttributeScopeAdvisorySwitch" << std::endl;
    my_auditServer.setLevel(6);
    {
        NM_Set_Attribute_Scope_Advisory_Switch rep;

        Debug(G, pdGendoc) << "enter RTIG::processSetAttributeScopeAdvisorySwitch" << endl;
        Debug(G, pdGendoc) << "BEGIN ** SET ATTRIBUTE SCOPE ADVISORY SWITCH **" << endl;

        if (request.message()->isAttributeScopeAdvisorySwitchOn()) {
            my_auditServer << "ON";
            my_federations.searchFederation(request.message()->getFederation())
                .getFederate(request.message()->getFederate())
                .setAttributeScopeAdvisorySwitch(true);
            Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                             << request.message()->getFederation() << " sets AttributeScopeAdvisorySwitch" << endl;
        }
        else {
            my_auditServer << "OFF";
            my_federations.searchFederation(request.message()->getFederation())
                .getFederate(request.message()->getFederate())
                .setAttributeScopeAdvisorySwitch(false);
            Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                             << request.message()->getFederation() << " clears AttributeScopeAdvisorySwitch" << endl;
        }

        rep.send(request.socket(), my_messageBuffer);

        Debug(G, pdGendoc) << "END ** SET ATTRIBUTE SCOPE ADVISORY SWITCH **" << endl;
        Debug(G, pdGendoc) << "exit RTIG::processSetAttributeScopeAdvisorySwitch" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Set_Time_Regulating> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "SetTimeRegulating for federate " << request.message()->getFederate()
                      << ", date:" << request.message()->getDate().getTime() << std::endl;
    my_auditServer.setLevel(8);
    {
        Debug(G, pdGendoc) << "enter RTIG::processSetTimeRegulating" << endl;
        Debug(G, pdGendoc) << "BEGIN ** SET TIME REGULATING **" << endl;

        if (request.message()->isRegulatorOn()) {
            my_auditServer << "ON at time " << request.message()->getDate().getTime();

            my_federations.searchFederation(request.message()->getFederation())
                .addRegulator(request.message()->getFederate(), request.message()->getDate());

            // send timeRegulationEnabled() to federate.
            NM_Time_Regulation_Enabled rep;
            rep.setFederate(request.message()->getFederate());
            rep.setFederation(request.message()->getFederation());
            rep.setDate(request.message()->getDate());

            Debug(G, pdGendoc) << "      processSetTimeRegulating====> write TRE to RTIA" << endl;

            rep.send(request.socket(), my_messageBuffer);

            Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                             << request.message()->getFederation() << " sets TimeRegulation ON" << endl;
        }
        else {
            my_auditServer << "OFF";

            my_federations.searchFederation(request.message()->getFederation())
                .removeRegulator(request.message()->getFederate());
            Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                             << request.message()->getFederation() << " sets TimeRegulation OFF" << endl;
        }

        Debug(G, pdGendoc) << "END ** SET TIME REGULATING **" << endl;
        Debug(G, pdGendoc) << "exit RTIG::processSetTimeRegulating" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Set_Time_Constrained> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "SetTimeConstrained for federate " << request.message()->getFederate() << std::endl;
    my_auditServer.setLevel(8);

    {
        Debug(G, pdGendoc) << "enter RTIG::processSetTimeConstrained" << endl;
        Debug(G, pdGendoc) << "BEGIN ** SET TIME CONSTRAINED **" << endl;

        if (request.message()->isConstrainedOn()) {
            my_auditServer << "ON at time " << request.message()->getDate().getTime();

            my_federations.searchFederation(request.message()->getFederation())
                .getFederate(request.message()->getFederate())
                .setConstrained(true);

            // send timeConstrainedEnabled() to federate.
            NM_Time_Constrained_Enabled rep;
            rep.setFederate(request.message()->getFederate());
            rep.setFederation(request.message()->getFederation());
            rep.setDate(request.message()->getDate());

            Debug(G, pdGendoc) << "      processSetTimeConstrained====> write TCE to RTIA" << endl;

            rep.send(request.socket(), my_messageBuffer);

            Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                             << request.message()->getFederation() << " is now constrained" << endl;
        }
        else {
            my_auditServer << "OFF";

            my_federations.searchFederation(request.message()->getFederation())
                .getFederate(request.message()->getFederate())
                .setConstrained(false);
            Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                             << request.message()->getFederation() << " is no more constrained" << endl;
        }

        Debug(G, pdGendoc) << "END ** SET TIME CONSTRAINED **" << endl;
        Debug(G, pdGendoc) << "exit RTIG::processSetTimeConstrained" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Message_Null> request, bool anonymous)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdDebug) << "Message Null" << std::endl;
    my_auditServer.setLevel(0);
    {
        my_auditServer << "Date " << request.message()->getDate().getTime();

        Debug(DNULL, pdDebug) << "Rcv NULL MSG (Federate=" << request.message()->getFederate()
                              << ", Time = " << request.message()->getDate().getTime() << ")" << endl;
        // Catch all exceptions because RTIA does not expect an answer anyway.
        try {
            my_federations.searchFederation(request.message()->getFederation())
                .updateRegulator(request.message()->getFederate(), request.message()->getDate(), anonymous);
        }
        catch (Exception& e) {
        }
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Message_Null_Prime> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdDebug) << "Message Null Prime" << std::endl;
    my_auditServer.setLevel(0);
    {
        Debug(DNULL, pdDebug) << "Rcv NULL PRIME MSG (Federate=" << request.message()->getFederate()
                              << ", Time = " << request.message()->getDate().getTime() << ")" << endl;
        /*
     * Update the NullPrimeDate of the concerned federate.
     * and check the result in order to decide whether
     * if the RTIG should send an anonymous NULL message or not
     */
        if (my_federations.searchFederation(request.message()->getFederation())
                .updateLastNERxForFederate(request.message()->getFederate(), request.message()->getDate())) {
            std::unique_ptr<NM_Message_Null> nmsg = std::make_unique<NM_Message_Null>();
            nmsg->setDate(my_federations.searchFederation(request.message()->getFederation()).getMinNERx());
            nmsg->setFederation(request.message()->getFederation());
            nmsg->setFederate(0);
            //nmsg.show(std::cout);
            process({request.socket(), std::move(nmsg)}, true);
        }
    }
    return {};
}

MessageProcessor::Responses
MessageProcessor::process(MessageEvent<NM_Register_Federation_Synchronization_Point> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "Federation " << request.message()->getFederation() << ": registerFedSyncPoint from federate "
                      << request.message()->getFederate() << std::endl;
    my_auditServer.setLevel(8);
    {
        Debug(G, pdGendoc) << "BEGIN ** REGISTER FEDERATION SYNCHRONIZATION POINT Service **" << endl;
        Debug(G, pdGendoc) << "enter RTIG::processRegisterSynchronization" << endl;

        my_auditServer << "Label \"" << request.message()->getLabel() << "\" registered. Tag is \""
                       << request.message()->getTag() << "\"";

        /* prepare answer to be sent to RTIA */
        NM_Confirm_Synchronization_Point_Registration rep;
        rep.setFederate(request.message()->getFederate());
        rep.setFederation(request.message()->getFederation());
        rep.setLabel(request.message()->getLabel());

        try {
            // boolean true means a federates set exists
            if (request.message()->getExists()) {
                my_federations.searchFederation(request.message()->getFederation())
                    .registerSynchronization(request.message()->getFederate(),
                                             request.message()->getLabel(),
                                             request.message()->getTag(),
                                             request.message()->getFederates());
            }
            else {
                my_federations.searchFederation(request.message()->getFederation())
                    .registerSynchronization(
                        request.message()->getFederate(), request.message()->getLabel(), request.message()->getTag());
            }
            // send synchronizationPointRegistrationSucceeded() to federate.
            rep.setSuccessIndicator(true);
        }
        catch (Exception& e) {
            /* the registration did fail */
            rep.setSuccessIndicator(false);
            rep.setFailureReason(std::string(e.name()) + ":" + std::string(e.reason()));
        }
        Debug(D, pdTerm) << "Federation " << request.message()->getFederation() << " is now synchronizing" << endl;

        Debug(G, pdGendoc) << "      processRegisterSynchronization====> write SPRS to RTIA" << endl;
        rep.send(request.socket(), my_messageBuffer);

        // proceed with the broadcast iff registration was successful
        if (rep.getSuccessIndicator()) {
            // boolean true means a federates set exists
            if (request.message()->getExists()) {
                my_federations.searchFederation(request.message()->getFederation())
                    .broadcastSynchronization(request.message()->getFederate(),
                                              request.message()->getLabel(),
                                              request.message()->getTag(),
                                              request.message()->getFederates());
            }
            else {
                my_federations.searchFederation(request.message()->getFederation())
                    .broadcastSynchronization(
                        request.message()->getFederate(), request.message()->getLabel(), request.message()->getTag());
            }
        }
        Debug(G, pdGendoc) << "exit  RTIG::processRegisterSynchronization" << endl;
        Debug(G, pdGendoc) << "END   ** REGISTER FEDERATION SYNCHRONIZATION POINT Service **" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Synchronization_Point_Achieved> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "Federation " << request.message()->getFederation()
                      << ": synchronizationPointAchieved from federate " << request.message()->getFederate()
                      << std::endl;
    my_auditServer.setLevel(8);
    {
        my_auditServer << "Label \"" << request.message()->getLabel() << "\" ended";

        my_federations.searchFederation(request.message()->getFederation())
            .unregisterSynchronization(request.message()->getFederate(), request.message()->getLabel());
        Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " has synchronized" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Request_Federation_Save> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "Request federation save from federate " << request.message()->getFederate() << std::endl;
    my_auditServer.setLevel(8);
    {
        Debug(G, pdGendoc) << "BEGIN ** REQUEST FEDERATION SAVE SERVICE **" << endl;
        Debug(G, pdGendoc) << "enter RTIG::processRequestFederationSave" << endl;

        my_auditServer << "Federation save request";

        if (request.message()->isDated()) {
            // With time
            my_federations.searchFederation(request.message()->getFederation())
                .requestFederationSave(
                    request.message()->getFederate(), request.message()->getLabel(), request.message()->getDate());
        }
        else {
            // Without time
            my_federations.searchFederation(request.message()->getFederation())
                .requestFederationSave(request.message()->getFederate(), request.message()->getLabel());
        }

        Debug(G, pdGendoc) << "exit  RTIG::processRequestFederationSave" << endl;
        Debug(G, pdGendoc) << "END   ** REQUEST FEDERATION SAVE SERVICE **" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Federate_Save_Begun> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "Federate " << request.message()->getFederate() << " begun save" << std::endl;
    my_auditServer.setLevel(8);
    {
        Debug(G, pdGendoc) << "enter RTIG::processFederateSaveBegun" << endl;
        Debug(G, pdGendoc) << "BEGIN ** FEDERATE SAVE BEGUN SERVICE **" << endl;
        Debug(G, pdGendoc) << "processFederateSaveBegun federation = " << request.message()->getFederation() << endl;

        my_auditServer << "Federate " << request.message()->getFederate() << " save begun";

        my_federations.searchFederation(request.message()->getFederation())
            .federateSaveBegun(request.message()->getFederate());

        Debug(G, pdGendoc) << "END   ** FEDERATE SAVE BEGUN SERVICE **" << endl;
        Debug(G, pdGendoc) << "exit  RTIG::processFederateSaveBegun" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Federate_Save_Complete> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "Federate " << request.message()->getFederate() << " save complete/not complete" << std::endl;
    my_auditServer.setLevel(8);
    {
        Debug(G, pdGendoc) << "enter RTIG::processFederateSaveStatus" << endl;
        if (request.message()->getMessageType() == NetworkMessage::FEDERATE_SAVE_COMPLETE) {
            Debug(G, pdGendoc) << "BEGIN ** FEDERATE SAVE COMPLETE SERVICE **" << endl;
        }
        else {
            Debug(G, pdGendoc) << "BEGIN ** FEDERATE SAVE NOT COMPLETE SERVICE **" << endl;
        }

        my_auditServer << "Federate " << request.message()->getFederate() << " save ended";

        bool status = request.message()->getMessageType() == NetworkMessage::FEDERATE_SAVE_COMPLETE;
        my_federations.searchFederation(request.message()->getFederation())
            .federateSaveStatus(request.message()->getFederate(), status);

        Debug(G, pdGendoc) << "exit  END   ** FEDERATE SAVE (NOT) COMPLETE SERVICE **" << endl;
        Debug(G, pdGendoc) << "exit  RTIG::processFederateSaveStatus" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Federate_Save_Not_Complete> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "Federate " << request.message()->getFederate() << " save complete/not complete" << std::endl;
    my_auditServer.setLevel(8);
    {
        Debug(G, pdGendoc) << "enter RTIG::processFederateSaveStatus" << endl;
        if (request.message()->getMessageType() == NetworkMessage::FEDERATE_SAVE_COMPLETE) {
            Debug(G, pdGendoc) << "BEGIN ** FEDERATE SAVE COMPLETE SERVICE **" << endl;
        }
        else {
            Debug(G, pdGendoc) << "BEGIN ** FEDERATE SAVE NOT COMPLETE SERVICE **" << endl;
        }

        my_auditServer << "Federate " << request.message()->getFederate() << " save ended";

        bool status = request.message()->getMessageType() == NetworkMessage::FEDERATE_SAVE_COMPLETE;
        my_federations.searchFederation(request.message()->getFederation())
            .federateSaveStatus(request.message()->getFederate(), status);

        Debug(G, pdGendoc) << "exit  END   ** FEDERATE SAVE (NOT) COMPLETE SERVICE **" << endl;
        Debug(G, pdGendoc) << "exit  RTIG::processFederateSaveStatus" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Request_Federation_Restore> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "Federate " << request.message()->getFederate() << " request a restoration" << std::endl;
    my_auditServer.setLevel(8);
    {
        Debug(G, pdGendoc) << "BEGIN ** REQUEST FEDERATION RESTORE SERVICE **" << endl;
        Debug(G, pdGendoc) << "enter RTIG::processRequestFederationRestore" << endl;
        my_auditServer << "Federate " << request.message()->getFederate() << " request restore";

        my_federations.searchFederation(request.message()->getFederation())
            .requestFederationRestore(request.message()->getFederate(), request.message()->getLabel());

        Debug(G, pdGendoc) << "exit  RTIG::processRequestFederationRestore" << endl;
        Debug(G, pdGendoc) << "END   ** REQUEST FEDERATION RESTORE SERVICE **" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Federate_Restore_Complete> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "Federate " << request.message()->getFederate() << " restore complete/not complete"
                      << std::endl;
    my_auditServer.setLevel(8);
    {
        Debug(G, pdGendoc) << "BEGIN ** FEDERATE RESTORE (NOT)COMPLETE **" << endl;
        Debug(G, pdGendoc) << "enter RTIG::processRequestFederateRestoreStatus" << endl;
        my_auditServer << "Federate " << request.message()->getFederate() << " restore ended";

        bool status = request.message()->getMessageType() == NetworkMessage::FEDERATE_RESTORE_COMPLETE;

        my_federations.searchFederation(request.message()->getFederation())
            .federateRestoreStatus(request.message()->getFederate(), status);

        Debug(G, pdGendoc) << "exit  RTIG::processRequestFederateRestoreStatus" << endl;
        Debug(G, pdGendoc) << "END   ** FEDERATE RESTORE (NOT)COMPLETE **" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Federate_Restore_Not_Complete> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "Federate " << request.message()->getFederate() << " restore complete/not complete"
                      << std::endl;
    my_auditServer.setLevel(8);
    {
        Debug(G, pdGendoc) << "BEGIN ** FEDERATE RESTORE (NOT)COMPLETE **" << endl;
        Debug(G, pdGendoc) << "enter RTIG::processRequestFederateRestoreStatus" << endl;
        my_auditServer << "Federate " << request.message()->getFederate() << " restore ended";

        bool status = request.message()->getMessageType() == NetworkMessage::FEDERATE_RESTORE_COMPLETE;

        my_federations.searchFederation(request.message()->getFederation())
            .federateRestoreStatus(request.message()->getFederate(), status);

        Debug(G, pdGendoc) << "exit  RTIG::processRequestFederateRestoreStatus" << endl;
        Debug(G, pdGendoc) << "END   ** FEDERATE RESTORE (NOT)COMPLETE **" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Publish_Object_Class> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "un/publishObjectClass" << std::endl;
    my_auditServer.setLevel(7);
    /* we cast to Publish because Unpublish inherits from Publish */
    {
        bool pub = (request.message()->getMessageType() == NetworkMessage::PUBLISH_OBJECT_CLASS);

        my_auditServer << "Publish Object Class = " << request.message()->getObjectClass()
                       << ", # of att. = " << request.message()->getAttributesSize();

        my_federations.searchFederation(request.message()->getFederation())
            .publishObject(request.message()->getFederate(),
                           request.message()->getObjectClass(),
                           request.message()->getAttributes(),
                           pub);

        Debug(D, pdRegister) << "Federate " << request.message()->getFederate() << " of Federation "
                             << request.message()->getFederation() << " published object class "
                             << request.message()->getObjectClass() << endl;

        if (pub) {
            NM_Publish_Object_Class POC;
            POC.setFederate(request.message()->getFederate());
            POC.setObjectClass(request.message()->getObjectClass());
            POC.send(request.socket(), my_messageBuffer); // send answer to RTIA
        }
        else {
            NM_Unpublish_Object_Class UOC;
            UOC.setFederate(request.message()->getFederate());
            UOC.setObjectClass(request.message()->getObjectClass());
            UOC.send(request.socket(), my_messageBuffer); // send answer to RTIA
        }
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Unpublish_Object_Class> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "un/publishObjectClass" << std::endl;
    my_auditServer.setLevel(7);
    /* we cast to Publish because Unpublish inherits from Publish */
    {
        bool pub = (request.message()->getMessageType() == NetworkMessage::PUBLISH_OBJECT_CLASS);

        my_auditServer << "Publish Object Class = " << request.message()->getObjectClass()
                       << ", # of att. = " << request.message()->getAttributesSize();

        my_federations.searchFederation(request.message()->getFederation())
            .publishObject(request.message()->getFederate(),
                           request.message()->getObjectClass(),
                           request.message()->getAttributes(),
                           pub);

        Debug(D, pdRegister) << "Federate " << request.message()->getFederate() << " of Federation "
                             << request.message()->getFederation() << " published object class "
                             << request.message()->getObjectClass() << endl;

        if (pub) {
            NM_Publish_Object_Class POC;
            POC.setFederate(request.message()->getFederate());
            POC.setObjectClass(request.message()->getObjectClass());
            POC.send(request.socket(), my_messageBuffer); // send answer to RTIA
        }
        else {
            NM_Unpublish_Object_Class UOC;
            UOC.setFederate(request.message()->getFederate());
            UOC.setObjectClass(request.message()->getObjectClass());
            UOC.send(request.socket(), my_messageBuffer); // send answer to RTIA
        }
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Subscribe_Object_Class> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "un/subscribeObjectClass" << std::endl;
    my_auditServer.setLevel(7);
    {
        Debug(G, pdGendoc) << "enter RTIG::processSubscribeObjectClass" << endl;
        Debug(G, pdGendoc) << "BEGIN **  SUBSCRIBE OBJECT CLASS SERVICE **" << endl;

        std::vector<AttributeHandle> emptyAttributeList;
        bool sub = (request.message()->getMessageType() == NetworkMessage::SUBSCRIBE_OBJECT_CLASS);

        my_auditServer << "Subscribe Object Class = " << request.message()->getObjectClass()
                       << ", # of att. = " << request.message()->getAttributesSize();
        my_federations.searchFederation(request.message()->getFederation())
            .subscribeObject(request.message()->getFederate(),
                             request.message()->getObjectClass(),
                             sub ? request.message()->getAttributes() : emptyAttributeList);

        Debug(D, pdRegister) << "Federate " << request.message()->getFederate() << " of Federation "
                             << request.message()->getFederation() << " subscribed to object class "
                             << request.message()->getObjectClass() << endl;

        if (sub) {
            NM_Subscribe_Object_Class rep;
            rep.setFederate(request.message()->getFederate());
            rep.setObjectClass(request.message()->getObjectClass());
            rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
        }
        else {
            NM_Unsubscribe_Object_Class rep;
            rep.setFederate(request.message()->getFederate());
            rep.setObjectClass(request.message()->getObjectClass());
            rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
        }

        Debug(G, pdGendoc) << "END   **  SUBSCRIBE OBJECT CLASS SERVICE **" << endl;
        Debug(G, pdGendoc) << "exit  RTIG::processSubscribeObjectClass" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Unsubscribe_Object_Class> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "un/subscribeObjectClass" << std::endl;
    my_auditServer.setLevel(7);
    {
        Debug(G, pdGendoc) << "enter RTIG::processSubscribeObjectClass" << endl;
        Debug(G, pdGendoc) << "BEGIN **  SUBSCRIBE OBJECT CLASS SERVICE **" << endl;

        std::vector<AttributeHandle> emptyAttributeList;
        bool sub = (request.message()->getMessageType() == NetworkMessage::SUBSCRIBE_OBJECT_CLASS);

        my_auditServer << "Subscribe Object Class = " << request.message()->getObjectClass()
                       << ", # of att. = " << request.message()->getAttributesSize();
        my_federations.searchFederation(request.message()->getFederation())
            .subscribeObject(request.message()->getFederate(),
                             request.message()->getObjectClass(),
                             sub ? request.message()->getAttributes() : emptyAttributeList);

        Debug(D, pdRegister) << "Federate " << request.message()->getFederate() << " of Federation "
                             << request.message()->getFederation() << " subscribed to object class "
                             << request.message()->getObjectClass() << endl;

        if (sub) {
            NM_Subscribe_Object_Class rep;
            rep.setFederate(request.message()->getFederate());
            rep.setObjectClass(request.message()->getObjectClass());
            rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
        }
        else {
            NM_Unsubscribe_Object_Class rep;
            rep.setFederate(request.message()->getFederate());
            rep.setObjectClass(request.message()->getObjectClass());
            rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
        }

        Debug(G, pdGendoc) << "END   **  SUBSCRIBE OBJECT CLASS SERVICE **" << endl;
        Debug(G, pdGendoc) << "exit  RTIG::processSubscribeObjectClass" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Publish_Interaction_Class> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "un/publishInteractionClass" << std::endl;
    my_auditServer.setLevel(7);
    {
        bool pub = (request.message()->getMessageType() == NetworkMessage::PUBLISH_INTERACTION_CLASS);

        my_auditServer << "Publish Interaction Class = " << request.message()->getInteractionClass();
        my_federations.searchFederation(request.message()->getFederation())
            .publishInteraction(request.message()->getFederate(), request.message()->getInteractionClass(), pub);
        Debug(D, pdRequest) << "Federate " << request.message()->getFederate() << " of Federation "
                            << request.message()->getFederation() << " publishes Interaction "
                            << request.message()->getInteractionClass() << endl;

        if (pub) {
            NM_Publish_Interaction_Class rep;
            rep.setFederate(request.message()->getFederate());
            rep.setInteractionClass(request.message()->getInteractionClass());

            rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
        }
        else {
            NM_Unpublish_Interaction_Class rep;
            rep.setFederate(request.message()->getFederate());
            rep.setInteractionClass(request.message()->getInteractionClass());

            rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
        }
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Unpublish_Interaction_Class> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "un/publishInteractionClass" << std::endl;
    my_auditServer.setLevel(7);
    {
        bool pub = (request.message()->getMessageType() == NetworkMessage::PUBLISH_INTERACTION_CLASS);

        my_auditServer << "Publish Interaction Class = " << request.message()->getInteractionClass();
        my_federations.searchFederation(request.message()->getFederation())
            .publishInteraction(request.message()->getFederate(), request.message()->getInteractionClass(), pub);
        Debug(D, pdRequest) << "Federate " << request.message()->getFederate() << " of Federation "
                            << request.message()->getFederation() << " publishes Interaction "
                            << request.message()->getInteractionClass() << endl;

        if (pub) {
            NM_Publish_Interaction_Class rep;
            rep.setFederate(request.message()->getFederate());
            rep.setInteractionClass(request.message()->getInteractionClass());

            rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
        }
        else {
            NM_Unpublish_Interaction_Class rep;
            rep.setFederate(request.message()->getFederate());
            rep.setInteractionClass(request.message()->getInteractionClass());

            rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
        }
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Subscribe_Interaction_Class> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "un/subscribeInteractionClass" << std::endl;
    my_auditServer.setLevel(7);
    {
        bool sub = (request.message()->getMessageType() == NetworkMessage::SUBSCRIBE_INTERACTION_CLASS);

        my_auditServer << "Subscribe Interaction Class = " << request.message()->getInteractionClass();
        my_federations.searchFederation(request.message()->getFederation())
            .subscribeInteraction(request.message()->getFederate(), request.message()->getInteractionClass(), sub);
        Debug(D, pdRequest) << "Federate " << request.message()->getFederate() << " of Federation "
                            << request.message()->getFederation() << " subscribed to Interaction "
                            << request.message()->getInteractionClass() << endl;

        if (sub) {
            NM_Subscribe_Interaction_Class rep;
            rep.setFederate(request.message()->getFederate());
            rep.setInteractionClass(request.message()->getInteractionClass());

            rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
        }
        else {
            NM_Unsubscribe_Interaction_Class rep;
            rep.setFederate(request.message()->getFederate());
            rep.setInteractionClass(request.message()->getInteractionClass());

            rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
        }
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Unsubscribe_Interaction_Class> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "un/subscribeInteractionClass" << std::endl;
    my_auditServer.setLevel(7);
    {
        bool sub = (request.message()->getMessageType() == NetworkMessage::SUBSCRIBE_INTERACTION_CLASS);

        my_auditServer << "Subscribe Interaction Class = " << request.message()->getInteractionClass();
        my_federations.searchFederation(request.message()->getFederation())
            .subscribeInteraction(request.message()->getFederate(), request.message()->getInteractionClass(), sub);
        Debug(D, pdRequest) << "Federate " << request.message()->getFederate() << " of Federation "
                            << request.message()->getFederation() << " subscribed to Interaction "
                            << request.message()->getInteractionClass() << endl;

        if (sub) {
            NM_Subscribe_Interaction_Class rep;
            rep.setFederate(request.message()->getFederate());
            rep.setInteractionClass(request.message()->getInteractionClass());

            rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
        }
        else {
            NM_Unsubscribe_Interaction_Class rep;
            rep.setFederate(request.message()->getFederate());
            rep.setInteractionClass(request.message()->getInteractionClass());

            rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
        }
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Reserve_Object_Instance_Name> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "reserveObjectInstanceName" << std::endl;
    my_auditServer.setLevel(6);
    {
        Debug(G, pdGendoc) << "enter RTIG::processReserveObjectInstanceName" << endl;
        my_auditServer << "Reserve Object Name = " << request.message()->getObjectName();

        my_federations.searchFederation(request.message()->getFederation())
            .reserveObjectInstanceName(request.message()->getFederate(), request.message()->getObjectName());

        Debug(G, pdGendoc) << "exit RTIG::processReserveObjectInstanceName" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Register_Object> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "registerObject" << std::endl;
    my_auditServer.setLevel(6);
    {
        NM_Register_Object rep;

        my_auditServer << "Register Object Class = " << request.message()->getObjectClass();
        rep.setObject(my_federations.searchFederation(request.message()->getFederation())
                          .registerObject(request.message()->getFederate(),
                                          request.message()->getObjectClass(),
                                          request.message()->getLabel()));

        Debug(D, pdRegister) << "Object \"" << request.message()->getLabel() << "\" of Federate "
                             << request.message()->getFederate() << " has been registered under ID " << rep.getObject()
                             << endl;

        rep.setFederate(request.message()->getFederate());
        rep.setFederation(request.message()->getFederation());
        rep.setObjectClass(request.message()->getObjectClass());
        // rep.object is set by the call of registerObject
        if (request.message()->hasObjectName()) {
            rep.setObjectName(request.message()->getObjectName());
        }
        if (request.message()->isLabelled()) {
            rep.setLabel(request.message()->getLabel());
        }

        rep.send(request.socket(), my_messageBuffer); // Send answer to RTIA
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Update_Attribute_Values> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdDebug) << "UpdateAttributeValue" << std::endl;
    my_auditServer.setLevel(1);

    Debug(G, pdGendoc) << "enter RTIG::processUpdateAttributeValues" << endl;

    my_auditServer << "ObjID = " << request.message()->getObject()
                   << ", Date = " << request.message()->getDate().getTime();

    // Forward the call
    if (request.message()->isDated()) {
        // UAV with time
        my_federations.searchFederation(request.message()->getFederation())
            .updateAttributeValues(request.message()->getFederate(),
                                   request.message()->getObject(),
                                   request.message()->getAttributes(),
                                   request.message()->getValues(),
                                   request.message()->getDate(),
                                   request.message()->getLabel());
    }
    else {
        // UAV without time
        my_federations.searchFederation(request.message()->getFederation())
            .updateAttributeValues(request.message()->getFederate(),
                                   request.message()->getObject(),
                                   request.message()->getAttributes(),
                                   request.message()->getValues(),
                                   request.message()->getAttributesSize(),
                                   request.message()->getLabel());
    }

    // Building answer (Network Message)
    auto rep = std::make_unique<NM_Update_Attribute_Values>();
    rep->setFederate(request.message()->getFederate());
    rep->setObject(request.message()->getObject());

    // Don't forget date, label and tag if provided in the request
    if (request.message()->isDated()) {
        rep->setDate(request.message()->getDate());
    }
    if (request.message()->isLabelled()) {
        rep->setLabel(request.message()->getLabel());
    }
    if (request.message()->isTagged()) {
        rep->setTag(request.message()->getTag());
    }

    Debug(G, pdGendoc) << "exit  RTIG::processUpdateAttributeValues" << endl;

    //             rep->send(request.socket(), my_messageBuffer); // send answer to RTIA
    //     return {};

    MessageEvent<NM_Update_Attribute_Values> response(request.socket(), std::move(rep));
    Responses responses;
    responses.emplace_back(MessageEvent<NetworkMessage>{std::move(response)});
    return responses;
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Send_Interaction> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "send interaction" << std::endl;
    my_auditServer.setLevel(2);
    {
        Debug(G, pdGendoc) << "BEGIN ** SEND INTERACTION SERVICE **" << endl;
        Debug(G, pdGendoc) << "enter RTIG::processSendInteraction" << endl;

        // Building Value Array
        my_auditServer << "IntID = " << request.message()->getInteractionClass()
                       << ", date = " << request.message()->getDate().getTime();
        if (request.message()->isDated()) {
            my_federations.searchFederation(request.message()->getFederation())
                .broadcastInteraction(request.message()->getFederate(),
                                      request.message()->getInteractionClass(),
                                      request.message()->getParameters(),
                                      request.message()->getValues(),
                                      request.message()->getDate(),
                                      request.message()->getRegion(),
                                      request.message()->getLabel());
        }
        else {
            my_federations.searchFederation(request.message()->getFederation())
                .broadcastInteraction(request.message()->getFederate(),
                                      request.message()->getInteractionClass(),
                                      request.message()->getParameters(),
                                      request.message()->getValues(),
                                      request.message()->getParametersSize(),
                                      request.message()->getRegion(),
                                      request.message()->getLabel());
        }

        Debug(D, pdDebug) << "Interaction " << request.message()->getInteractionClass()
                          << " parameters update completed" << endl;

        NM_Send_Interaction rep;
        rep.setFederate(request.message()->getFederate());
        rep.setInteractionClass(request.message()->getInteractionClass());
        // Don't forget label and tag
        rep.setLabel(request.message()->getLabel());
        rep.setTag(request.message()->getTag());
        Debug(G, pdGendoc) << "processSendInteraction===>write" << endl;
        rep.send(request.socket(), my_messageBuffer); // send answer to RTIA

        Debug(G, pdGendoc) << "exit RTIG::processSendInteraction" << endl;
        Debug(G, pdGendoc) << "END ** SEND INTERACTION SERVICE **" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Delete_Object> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "DeleteObject" << std::endl;
    my_auditServer.setLevel(6);
    {
        Debug(G, pdGendoc) << "BEGIN ** DELETE OBJECT INSTANCE service **" << endl;
        Debug(G, pdGendoc) << "enter RTIG::processDeleteObject" << endl;
        my_auditServer << "Delete ObjID = " << request.message()->getObject();

        if (request.message()->isDated()) {
            my_federations.searchFederation(request.message()->getFederation())
                .deleteObject(request.message()->getFederate(),
                              request.message()->getObject(),
                              request.message()->getDate(),
                              request.message()->getLabel());
        }
        else {
            my_federations.searchFederation(request.message()->getFederation())
                .deleteObject(
                    request.message()->getFederate(), request.message()->getObject(), request.message()->getLabel());
        }

        Debug(D, pdRegister) << "Object # " << request.message()->getObject() << " of Federation "
                             << request.message()->getFederation() << " has been deleted" << endl;

        NM_Delete_Object rep;
        rep.setFederate(request.message()->getFederate());
        rep.setObject(request.message()->getObject());

        rep.send(request.socket(), my_messageBuffer); // send answer to RTIA

        Debug(G, pdGendoc) << "exit RTIG::processDeleteObject" << endl;
        Debug(G, pdGendoc) << "END ** DELETE OBJECT INSTANCE **" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Query_Attribute_Ownership> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "queryAttributeOwnership" << std::endl;
    my_auditServer.setLevel(2);
    {
        Debug(D, pdDebug) << "Owner of Attribute " << request.message()->getAttribute() << " of Object "
                          << request.message()->getObject() << endl;

        my_auditServer << "AttributeHandle = " << request.message()->getAttribute();

        my_federations.searchFederation(request.message()->getFederation())
            .queryAttributeOwnership(
                request.message()->getFederate(), request.message()->getObject(), request.message()->getAttribute());

        Debug(D, pdDebug) << "Owner of Attribute " << request.message()->getAttribute() << " of Object "
                          << request.message()->getObject() << endl;

        NM_Query_Attribute_Ownership rep;
        rep.setFederate(request.message()->getFederate());
        rep.setObject(request.message()->getObject());

        rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
    }
    return {};
}

MessageProcessor::Responses
MessageProcessor::process(MessageEvent<NM_Negotiated_Attribute_Ownership_Divestiture> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "negotiatedAttributeOwnershipDivestiture" << std::endl;
    my_auditServer.setLevel(6);
    {
        my_auditServer << "Object = " << request.message()->getObject()
                       << ", # of att. = " << request.message()->getAttributesSize();
        my_federations.searchFederation(request.message()->getFederation())
            .negotiateDivestiture(request.message()->getFederate(),
                                  request.message()->getObject(),
                                  request.message()->getAttributes(),
                                  request.message()->getLabel());

        Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                          << request.message()->getFederation() << " negotiate "
                                                                   "divestiture of object "
                          << request.message()->getObject() << endl;

        NM_Negotiated_Attribute_Ownership_Divestiture rep;
        rep.setFederate(request.message()->getFederate());
        rep.setObject(request.message()->getObject());

        rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
    }
    return {};
}

MessageProcessor::Responses
MessageProcessor::process(MessageEvent<NM_Attribute_Ownership_Acquisition_If_Available> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "attributeOwnershipAcquisitionIfAvailable" << std::endl;
    my_auditServer.setLevel(6);
    {
        my_auditServer << "Object = " << request.message()->getObject()
                       << ", # of att. = " << request.message()->getAttributesSize();

        my_federations.searchFederation(request.message()->getFederation())
            .acquireIfAvailable(
                request.message()->getFederate(), request.message()->getObject(), request.message()->getAttributes());

        Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                          << request.message()->getFederation() << " acquisitionIfAvailable "
                                                                   "of object "
                          << request.message()->getObject() << endl;

        NM_Attribute_Ownership_Acquisition_If_Available rep;
        rep.setFederate(request.message()->getFederate());
        rep.setObject(request.message()->getObject());

        rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
    }
    return {};
}

MessageProcessor::Responses
MessageProcessor::process(MessageEvent<NM_Unconditional_Attribute_Ownership_Divestiture> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "unconditionalAttributeOwnershipDivestiture" << std::endl;
    my_auditServer.setLevel(6);
    {
        my_auditServer << "Object = " << request.message()->getObject()
                       << ", # of att. = " << request.message()->getAttributesSize();

        my_federations.searchFederation(request.message()->getFederation())
            .divest(
                request.message()->getFederate(), request.message()->getObject(), request.message()->getAttributes());

        Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                          << request.message()->getFederation() << " UnconditionalDivestiture "
                                                                   "of object "
                          << request.message()->getObject() << endl;

        NM_Unconditional_Attribute_Ownership_Divestiture rep;
        rep.setFederate(request.message()->getFederate());
        rep.setObject(request.message()->getObject());

        rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Attribute_Ownership_Acquisition> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "attributeOwnershipAcquisition" << std::endl;
    my_auditServer.setLevel(6);
    {
        my_auditServer << "Object = " << request.message()->getObject()
                       << ", # of att. = " << request.message()->getAttributesSize();

        my_federations.searchFederation(request.message()->getFederation())
            .acquire(request.message()->getFederate(),
                     request.message()->getObject(),
                     request.message()->getAttributes(),
                     request.message()->getLabel());

        Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                          << request.message()->getFederation() << " ownership acquisition of object "
                          << request.message()->getObject() << endl;

        NM_Attribute_Ownership_Acquisition rep;
        rep.setFederate(request.message()->getFederate());
        rep.setObject(request.message()->getObject());
        rep.setAttributesSize(0);

        rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
    }
    return {};
}

MessageProcessor::Responses
MessageProcessor::process(MessageEvent<NM_Cancel_Negotiated_Attribute_Ownership_Divestiture> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "cancelNegociatedAttributeOwnershipDivestiture" << std::endl;
    my_auditServer.setLevel(6);
    {
        my_auditServer << "Object = " << request.message()->getObject()
                       << ", # of att. = " << request.message()->getAttributesSize();

        my_federations.searchFederation(request.message()->getFederation())
            .cancelDivestiture(
                request.message()->getFederate(), request.message()->getObject(), request.message()->getAttributes());

        Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                          << request.message()->getFederation() << " cancel negotiate "
                                                                   "divestiture of object "
                          << request.message()->getObject() << endl;

        NM_Cancel_Negotiated_Attribute_Ownership_Divestiture rep;
        rep.setFederate(request.message()->getFederate());
        rep.setObject(request.message()->getObject());

        rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Is_Attribute_Owned_By_Federate> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "isAttributeOwnedByFederate" << std::endl;
    my_auditServer.setLevel(2);
    {
        NM_Is_Attribute_Owned_By_Federate rep;

        Debug(D, pdDebug) << "Owner of Attribute " << request.message()->getAttribute() << " of Object "
                          << request.message()->getObject() << endl;

        my_auditServer << "AttributeHandle = " << request.message()->getAttribute();

        if (my_federations.searchFederation(request.message()->getFederation())
                .isOwner(request.message()->getFederate(),
                         request.message()->getObject(),
                         request.message()->getAttribute())) {
            rep.setLabel("RTI_TRUE");
        }
        else {
            rep.setLabel("RTI_FALSE");
        }

        Debug(D, pdDebug) << "Owner of Attribute " << request.message()->getAttribute() << " of Object "
                          << request.message()->getObject() << endl;

        rep.setFederate(request.message()->getFederate());
        rep.setObject(request.message()->getObject());
        rep.setAttribute(request.message()->getAttribute());

        rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Attribute_Ownership_Release_Response> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "attributeOwnershipReleaseResponse" << std::endl;
    my_auditServer.setLevel(6);
    {
        my_auditServer << "Object = " << request.message()->getObject()
                       << ", # of att. = " << request.message()->getAttributesSize();

        AttributeHandleSet* attributes = my_federations.searchFederation(request.message()->getFederation())
                                             .respondRelease(request.message()->getFederate(),
                                                             request.message()->getObject(),
                                                             request.message()->getAttributes());

        Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                          << request.message()->getFederation() << " release response "
                                                                   "of object "
                          << request.message()->getObject() << endl;

        NM_Attribute_Ownership_Release_Response rep;
        rep.setAttributesSize(attributes->size());

        for (uint32_t i = 0; i < attributes->size(); ++i) {
            rep.setAttributes(attributes->getHandle(i), i);
        }

        rep.setFederate(request.message()->getFederate());
        rep.setObject(request.message()->getObject());

        rep.send(request.socket(), my_messageBuffer); // Send answer to RTIA
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Cancel_Attribute_Ownership_Acquisition> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "cancelAttributeOwnershipAcquisition" << std::endl;
    my_auditServer.setLevel(6);
    {
        my_auditServer << "Object = " << request.message()->getObject()
                       << ", # of att. = " << request.message()->getAttributesSize();

        my_federations.searchFederation(request.message()->getFederation())
            .cancelAcquisition(
                request.message()->getFederate(), request.message()->getObject(), request.message()->getAttributes());

        Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                          << request.message()->getFederation() << " release response of object "
                          << request.message()->getObject() << endl;

        NM_Cancel_Attribute_Ownership_Acquisition rep;
        rep.setFederate(request.message()->getFederate());
        rep.setObject(request.message()->getObject());

        rep.send(request.socket(), my_messageBuffer); // send answer to RTIA
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_DDM_Create_Region> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "createRegion" << std::endl;
    my_auditServer.setLevel(6);
    {
        // TODO: audit...

        NM_DDM_Create_Region rep;
        rep.setRegion(my_federations.searchFederation(request.message()->getFederation())
                          .createRegion(request.message()->getFederate(),
                                        request.message()->getSpace(),
                                        request.message()->getNbExtents()));

        Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                          << request.message()->getFederation() << " creates region " << rep.getRegion() << endl;

        rep.setFederate(request.message()->getFederate());
        rep.send(request.socket(), my_messageBuffer);
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_DDM_Modify_Region> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "modifyRegion" << std::endl;
    my_auditServer.setLevel(6);
    {
        Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                          << request.message()->getFederation() << " modifies region " << request.message()->getRegion()
                          << endl;

        my_federations.searchFederation(request.message()->getFederation())
            .modifyRegion(
                request.message()->getFederate(), request.message()->getRegion(), request.message()->getExtents());

        NM_DDM_Modify_Region rep;
        rep.setFederate(request.message()->getFederate());
        rep.send(request.socket(), my_messageBuffer);
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_DDM_Delete_Region> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "deleteRegion" << std::endl;
    my_auditServer.setLevel(6);
    {
        // TODO: audit...

        Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                          << request.message()->getFederation() << " deletes region " << request.message()->getRegion()
                          << endl;

        my_federations.searchFederation(request.message()->getFederation())
            .deleteRegion(request.message()->getFederate(), request.message()->getRegion());

        NM_DDM_Delete_Region rep;

        rep.setFederate(request.message()->getFederate());
        rep.setRegion(request.message()->getRegion());
        rep.send(request.socket(), my_messageBuffer);
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_DDM_Associate_Region> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "associateRegionForUpdates" << std::endl;
    my_auditServer.setLevel(6);
    {
        // TODO: audit...

        Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                          << request.message()->getFederation() << " associates region "
                          << request.message()->getRegion() << " to some attributes of object "
                          << request.message()->getObject() << endl;

        my_federations.searchFederation(request.message()->getFederation())
            .associateRegion(request.message()->getFederate(),
                             request.message()->getObject(),
                             request.message()->getRegion(),
                             request.message()->getAttributes());

        NM_DDM_Associate_Region rep;
        rep.setFederate(request.message()->getFederate());
        rep.send(request.socket(), my_messageBuffer);
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_DDM_Unassociate_Region> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "unassociateRegionForUpdates" << std::endl;
    my_auditServer.setLevel(6);
    {
        // TODO: audit...

        my_federations.searchFederation(request.message()->getFederation())
            .unassociateRegion(
                request.message()->getFederate(), request.message()->getObject(), request.message()->getRegion());

        Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                          << request.message()->getFederation() << " associates region "
                          << request.message()->getRegion() << " from object " << request.message()->getObject()
                          << endl;

        NM_DDM_Unassociate_Region rep;
        rep.setFederate(request.message()->getFederate());
        rep.send(request.socket(), my_messageBuffer);
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_DDM_Subscribe_Attributes> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "subscribeObjectClassAttributes (DDM)" << std::endl;
    my_auditServer.setLevel(6);
    {
        // TODO: audit...
        Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                          << request.message()->getFederation() << " subscribes with region "
                          << request.message()->getRegion() << " to some attributes of class "
                          << request.message()->getObjectClass() << endl;

        my_federations.searchFederation(request.message()->getFederation())
            .subscribeAttributesWR(request.message()->getFederate(),
                                   request.message()->getObjectClass(),
                                   request.message()->getRegion(),
                                   request.message()->getAttributes());

        NM_DDM_Subscribe_Attributes rep;
        rep.setFederate(request.message()->getFederate());
        rep.setObjectClass(request.message()->getObjectClass());
        rep.send(request.socket(), my_messageBuffer);
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_DDM_Unsubscribe_Attributes> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "unsubscribeObjectClassAttributes (DDM)" << std::endl;
    my_auditServer.setLevel(6);
    {
        // TODO: audit...
        Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                          << request.message()->getFederation() << " unsubscribes with region "
                          << request.message()->getRegion() << " from object class "
                          << request.message()->getObjectClass() << endl;

        my_federations.searchFederation(request.message()->getFederation())
            .unsubscribeAttributesWR(
                request.message()->getFederate(), request.message()->getObjectClass(), request.message()->getRegion());

        NM_DDM_Unsubscribe_Attributes rep;
        rep.setFederate(request.message()->getFederate());
        rep.send(request.socket(), my_messageBuffer);
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_DDM_Subscribe_Interaction> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "subscribeInteraction (DDM)" << std::endl;
    my_auditServer.setLevel(6);
    {
        // TODO: audit...

        my_federations.searchFederation(request.message()->getFederation())
            .subscribeInteractionWR(request.message()->getFederate(),
                                    request.message()->getInteractionClass(),
                                    request.message()->getRegion());

        Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                          << request.message()->getFederation() << " subscribes with region "
                          << request.message()->getRegion() << " to interaction class "
                          << request.message()->getInteractionClass() << endl;

        NM_DDM_Subscribe_Interaction rep;
        rep.setFederate(request.message()->getFederate());
        rep.send(request.socket(), my_messageBuffer);
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_DDM_Unsubscribe_Interaction> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "unsubscribeInteraction (DDM)" << std::endl;
    my_auditServer.setLevel(6);
    {
        // TODO: audit...

        my_federations.searchFederation(request.message()->getFederation())
            .unsubscribeInteractionWR(request.message()->getFederate(),
                                      request.message()->getInteractionClass(),
                                      request.message()->getRegion());

        Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                          << request.message()->getFederation() << " unsubscribes with region "
                          << request.message()->getRegion() << " from interaction class "
                          << request.message()->getInteractionClass() << endl;

        NM_DDM_Unsubscribe_Interaction rep;
        rep.setFederate(request.message()->getFederate());
        rep.send(request.socket(), my_messageBuffer);
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_DDM_Register_Object> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "registerObjectWithRegion" << std::endl;
    my_auditServer.setLevel(6);
    {
        NM_DDM_Register_Object rep;
        // FIXME bug #9869
        // When we were passed a set of region
        // we should register object for each region
        // the trouble comes from the fact that
        // there is both
        //     - request.message()->region  (coming from NetworkMessage::region)
        //     - request.message()->regions (coming from BasicMessage::regions)
        // would be nice to sort those thing out.
        my_federations.searchFederation(request.message()->getFederation())
            .registerObjectWithRegion(request.message()->getFederate(),
                                      request.message()->getObjectClass(),
                                      request.message()->getLabel(),
                                      request.message()->getRegion(),
                                      request.message()->getAttributes());

        Debug(D, pdRegister) << "Object \"" << request.message()->getLabel() << "\" of Federate "
                             << request.message()->getFederate() << " has been registered under ID " << rep.getObject()
                             << endl;

        rep.setFederate(request.message()->getFederate());
        rep.send(request.socket(), my_messageBuffer); // Send answer to RTIA
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Request_Object_Attribute_Value_Update> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "RequestAttributeValueUpdate" << std::endl;
    my_auditServer.setLevel(6);
    {
        NM_Request_Object_Attribute_Value_Update answer;
        Debug(G, pdGendoc) << "enter RTIG::processRequestObjectAttributeValueUpdate" << endl;
        Debug(G, pdGendoc) << "BEGIN ** REQUEST OBJECT ATTRIBUTE VALUE UPDATE **" << endl;

        my_auditServer << "ObjID = " << request.message()->getObject();

        // We have to do verifications about this object and we need owner
        answer.setException(Exception::Type::NO_EXCEPTION);
        try {
            // While searching for the federate owner we will send
            // a NM_Provide_Attribute_Value_Update
            // (see Federation::requestObjectOwner)
            (void) my_federations.searchFederation(request.message()->getFederation())
                .requestObjectOwner(request.message()->getFederate(),
                                    request.message()->getObject(),
                                    request.message()->getAttributes());
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

        answer.setFederate(request.message()->getFederate());
        answer.setObject(request.message()->getObject());

        answer.send(request.socket(), my_messageBuffer); // Send answer to RTIA
        Debug(G, pdGendoc) << "exit  RTIG::processRequestObjectAttributeValueUpdate" << endl;
        Debug(G, pdGendoc) << "END   ** REQUEST OBJECT ATTRIBUTE VALUE UPDATE **" << endl;
    }
    return {};
}

MessageProcessor::Responses MessageProcessor::process(MessageEvent<NM_Request_Class_Attribute_Value_Update> request)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    Debug(D, pdTrace) << "RequestClassAttributeValueUpdate" << std::endl;
    my_auditServer.setLevel(6);
    {
        NM_Request_Class_Attribute_Value_Update answer;

        Debug(G, pdGendoc) << "enter RTIG::processRequestClassAttributeValueUpdate" << endl;
        Debug(G, pdGendoc) << "BEGIN ** REQUEST CLASS ATTRIBUTE VALUE UPDATE **" << endl;

        answer.setException(Exception::Type::NO_EXCEPTION);
        try {
            my_federations.searchFederation(request.message()->getFederation())
                .requestClassAttributeValueUpdate(request.message()->getFederate(),
                                                  request.message()->getObjectClass(),
                                                  request.message()->getAttributes());
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

        answer.setFederate(request.message()->getFederate());
        answer.setObjectClass(request.message()->getObjectClass());

        answer.send(request.socket(), my_messageBuffer); // Send answer to RTIA

        Debug(G, pdGendoc) << "exit  RTIG::processRequestClassAttributeValueUpdate" << endl;
        Debug(G, pdGendoc) << "END   ** REQUEST CLASS  ATTRIBUTE VALUE UPDATE **" << endl;
    }
    return {};
}
}
}
