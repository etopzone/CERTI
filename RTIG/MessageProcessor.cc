#include "MessageProcessor.hh"

#include <iostream>

#include <include/certi.hh>

#include <libCERTI/AuditFile.hh>
#include <libCERTI/Exception.hh>
#include <libCERTI/FedTimeD.hh>
#include <libCERTI/GAV.hh>
#include <libCERTI/HandleManager.hh>
#include <libCERTI/NetworkMessage.hh>
#include <libCERTI/PrettyDebug.hh>
#include <libCERTI/Socket.hh>
#include <libCERTI/SocketServer.hh>
#include <libCERTI/SocketTCP.hh>

#include "Federate.hh"
#include "Federation.hh"
#include "FederationsList.hh"
#include "make_unique.hh"

using std::endl;

namespace certi {
namespace rtig {

static PrettyDebug D("RTIG", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);
static PrettyDebug DNULL("RTIG_NULLMSG", "[RTIG NULL MSG]");
static PrettyDebug DTUS("RTIG_TIME_UP", "[RTIG TIME UP]");
static PrettyDebug MP("MESSAGEPROCESSOR", __FILE__);

MessageProcessor::MessageProcessor(AuditFile& audit_server,
                                   SocketServer& socket_server,
                                   HandleManager<Handle>& handle_generator,
                                   FederationsList& federations)
    : my_auditServer(audit_server)
    , my_socketServer(socket_server)
    , my_federationHandleGenerator(handle_generator)
    , my_federations(federations)
{
}

Responses MessageProcessor::processEvent(MessageEvent<NetworkMessage> request)
{
#define xstr(a) str(a)
#define str(a) #a

#define BASIC_CASE(MessageType, MessageClass)                                                                          \
    case NetworkMessage::Type::MessageType:                                                                            \
        Debug(MP, pdTrace) << xstr(MessageClass) << std::endl;                                                         \
        return process(MessageEvent<MessageClass>{std::move(request)})

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
        BASIC_CASE(ENABLE_ASYNCHRONOUS_DELIVERY, NM_Enable_Asynchronous_Delivery);
        BASIC_CASE(DISABLE_ASYNCHRONOUS_DELIVERY, NM_Disable_Asynchronous_Delivery);
        BASIC_CASE(TIME_STATE_UPDATE, NM_Time_State_Update);

    case NetworkMessage::Type::CLOSE_CONNEXION:
        throw RTIinternalError("Close connection: Should have been handled by RTIG");

    default:
        // FIXME: Should treat other cases CHANGE_*_ORDER/TRANSPORT_TYPE
        Debug(D, pdError) << "MessageProcessor::processEvent(): unknown type "
                          << static_cast<std::underlying_type<NetworkMessage::Type>::type>(
                                 request.message()->getMessageType())
                          << std::endl;
        throw RTIinternalError("Unknown Message Type");
    }
}

Responses MessageProcessor::process(MessageEvent<NM_Create_Federation_Execution>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(9));

    const auto& federation = request.message()->getFederationExecutionName();
    const auto& fom_modules = request.message()->getFomModuleDesignators();
    const auto& mim_designator = request.message()->getMimDesignator();
    const auto& rti_version = request.message()->getRtiVersion();

    my_auditServer << "Federation Name : " << federation;
    auto handle = FederationHandle(my_federationHandleGenerator.provide());

#ifdef FEDERATION_USES_MULTICAST
    // multicast base address
    unsigned long base_adr_mc = inet_addr(ADRESSE_MULTICAST);

    // creer la communication multicast
    SocketMC* com_mc = new SocketMC();
    if (!com_mc) {
        Debug(D, pdExcept) << "Unable to allocate Multicast socket" << endl;
        Debug(G, pdGendoc) << "exit  RTIG::processCreateFederation on exception RTIinternalError" << endl;
        throw RTIinternalError("Unable to allocate Multicast socket");
    }

    com_mc->CreerSocketMC(base_adr_mc + h, MC_PORT);

    // inserer la nouvelle federation dans la liste des federations
    auto rep = my_federations->createFederation(
        federation, handle, my_socketServer, my_auditServer, fom_modules, mim_designator, rti_version, com_mc);

    // inserer descripteur fichier pour le prochain appel a un select
    ClientSockets.push_front(com_mc);
#else
    auto rep = my_federations.createFederation(
        federation, handle, my_socketServer, my_auditServer, fom_modules, mim_designator, rti_version);
#endif
    my_auditServer << " created";

    responses = my_federations.searchFederation(FederationHandle(rep->getFederation())).enableMomIfAvailable();

    responses.emplace_back(request.sockets().front(), std::move(rep));

    Debug(D, pdInit) << "Federation \"" << federation << "\" created with Handle " << handle.get() << endl;

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Join_Federation_Execution>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(9));

    const auto& federation_name = request.message()->getFederationExecutionName();
    const auto& federate_name = request.message()->getFederateName();
    const auto& federate_type = request.message()->getFederateType();
    const auto& additional_modules = request.message()->getAdditionalFomModules();
    const auto& rti_version = request.message()->getRtiVersion();

    unsigned int peer = request.message()->getBestEffortPeer();
    unsigned long address = request.message()->getBestEffortAddress();

    if (federation_name.empty() || federate_name.empty()) {
        throw RTIinternalError("Invalid Federation/Federate Name");
    }

    my_auditServer << "Federate \"" << federate_name << "\" joins Federation \"" << federation_name << "\"";

    auto federation_handle = my_federations.getFederationHandle(federation_name);
    auto& federation = my_federations.searchFederation(federation_handle);

    FederateHandle federate_handle;
    std::tie(federate_handle, responses) = federation.add(federate_name,
                                                          federate_type,
                                                          additional_modules,
                                                          rti_version,
                                                          static_cast<SocketTCP*>(request.sockets().front()),
                                                          peer,
                                                          address);
    
    my_auditServer << "(" << federation_handle << ") with handle " << federate_handle << ". Socket "
                   << int(request.sockets().front()->returnSocket());

    Debug(D, pdInit) << "Federate \"" << federate_name << "\" has joined Federation " << federation_handle
                     << " under handle " << federate_handle << endl;

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Resign_Federation_Execution>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(9));

    auto federation = FederationHandle(request.message()->getFederation());
    const auto& federate = request.message()->getFederate();
    Debug(D, pdTrace) << "Federate (" << request.message()->getFederate() << ") leaves federation ("
                      << request.message()->getFederation() << ")" << std::endl;

    responses = my_federations.searchFederation(federation).remove(federate);

    Debug(D, pdInit) << "Federate " << federate << " is resigning from federation " << federation << endl;

    my_auditServer << "Federate " << federate << " resign federation(" << federation << ")";

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Destroy_Federation_Execution>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(9));

    auto rep = make_unique<NM_Destroy_Federation_Execution>();

    std::string federation = request.message()->getFederationName();

    FederationHandle federation_handle = my_federations.getFederationHandle(federation);

    my_federations.destroyFederation(federation_handle);

    // Here delete federation (num_federation) has been done
    my_federationHandleGenerator.free(federation_handle.get());

    Debug(D, pdInit) << "Federation \"" << federation << "\" has been destroyed" << endl;

    rep->setFederate(request.message()->getFederate());
    rep->setFederationName(federation);

    my_auditServer << "Federation Name \"" << federation << "\" (" << federation_handle << ") destroyed";

    Debug(G, pdGendoc) << "processDestroyFederation===>write DFE to RTIA" << endl;

    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Set_Class_Relevance_Advisory_Switch>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    auto rep = make_unique<NM_Set_Class_Relevance_Advisory_Switch>();

    if (request.message()->isClassRelevanceAdvisorySwitchOn()) {
        my_auditServer << "ON";
        my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
            .getFederate(request.message()->getFederate())
            .setClassRelevanceAdvisorySwitch(true);
        Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                         << request.message()->getFederation() << " sets AttributeRelevanceAdvisorySwitch" << endl;
    }
    else {
        my_auditServer << "OFF";
        my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
            .getFederate(request.message()->getFederate())
            .setClassRelevanceAdvisorySwitch(false);
        Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                         << request.message()->getFederation() << " clears AttributeRelevanceAdvisorySwitch" << endl;
    }

    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Set_Interaction_Relevance_Advisory_Switch>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    auto rep = make_unique<NM_Set_Interaction_Relevance_Advisory_Switch>();

    if (request.message()->isInteractionRelevanceAdvisorySwitchOn()) {
        my_auditServer << "ON";
        my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
            .getFederate(request.message()->getFederate())
            .setInteractionRelevanceAdvisorySwitch(true);
        Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                         << request.message()->getFederation() << " sets InteractionRelevanceAdvisorySwitch" << endl;
    }
    else {
        my_auditServer << "OFF";
        my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
            .getFederate(request.message()->getFederate())
            .setInteractionRelevanceAdvisorySwitch(false);
        Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                         << request.message()->getFederation() << " clears InteractionRelevanceAdvisorySwitch" << endl;
    }

    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Set_Attribute_Relevance_Advisory_Switch>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    auto rep = make_unique<NM_Set_Attribute_Relevance_Advisory_Switch>();

    if (request.message()->isAttributeRelevanceAdvisorySwitchOn()) {
        my_auditServer << "ON";
        my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
            .getFederate(request.message()->getFederate())
            .setAttributeRelevanceAdvisorySwitch(true);
        Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                         << request.message()->getFederation() << " sets AttributeRelevanceAdvisorySwitch" << endl;
    }
    else {
        my_auditServer << "OFF";
        my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
            .getFederate(request.message()->getFederate())
            .setAttributeRelevanceAdvisorySwitch(false);
        Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                         << request.message()->getFederation() << " clears AttributeRelevanceAdvisorySwitch" << endl;
    }

    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Set_Attribute_Scope_Advisory_Switch>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    auto rep = make_unique<NM_Set_Attribute_Scope_Advisory_Switch>();

    if (request.message()->isAttributeScopeAdvisorySwitchOn()) {
        my_auditServer << "ON";
        my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
            .getFederate(request.message()->getFederate())
            .setAttributeScopeAdvisorySwitch(true);
        Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                         << request.message()->getFederation() << " sets AttributeScopeAdvisorySwitch" << endl;
    }
    else {
        my_auditServer << "OFF";
        my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
            .getFederate(request.message()->getFederate())
            .setAttributeScopeAdvisorySwitch(false);
        Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                         << request.message()->getFederation() << " clears AttributeScopeAdvisorySwitch" << endl;
    }

    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Set_Time_Regulating>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(8));

    Debug(D, pdTrace) << "SetTimeRegulating for federate " << request.message()->getFederate()
                      << ", date:" << request.message()->getDate().getTime() << std::endl;

    if (request.message()->isRegulatorOn()) {
        my_auditServer << "ON at time " << request.message()->getDate().getTime();

        responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                        .addRegulator(request.message()->getFederate(), request.message()->getDate());

        Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                         << request.message()->getFederation() << " sets TimeRegulation ON" << endl;
    }
    else {
        my_auditServer << "OFF";

        responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                        .removeRegulator(request.message()->getFederate());

        Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                         << request.message()->getFederation() << " sets TimeRegulation OFF" << endl;
    }

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Set_Time_Constrained>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(8));

    responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                    .setConstrained(request.message()->getFederate(),
                                    request.message()->isConstrainedOn(),
                                    request.message()->getDate());

    if (request.message()->isConstrainedOn()) {
        my_auditServer << "ON at time " << request.message()->getDate().getTime();

        Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                         << request.message()->getFederation() << " is now constrained" << endl;
    }
    else {
        my_auditServer << "OFF";

        Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " of Federation "
                         << request.message()->getFederation() << " is no more constrained" << endl;
    }

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Message_Null>&& request, bool anonymous)
{
    my_auditServer.setLevel(AuditLine::Level(0));

    my_auditServer << "Date " << request.message()->getDate().getTime();

    Debug(DNULL, pdDebug) << "Rcv NULL MSG (Federate=" << request.message()->getFederate()
                          << ", Time = " << request.message()->getDate().getTime() << ")" << endl;
        
    // Catch all exceptions because RTIA does not expect an answer anyway.
    try {
        return my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
            .updateRegulator(request.message()->getFederate(),
                             request.message()->getDate(),
                             /*request.message()->getLogicalTime(),
                             request.message()->getLookahead(),
                             request.message()->getState(),
                             request.message()->getGalt(),
                             request.message()->getLits(),*/
                             anonymous);
    }
    catch (Exception& e) {
    }

    return {};
}

Responses MessageProcessor::process(MessageEvent<NM_Message_Null_Prime>&& request)
{
    my_auditServer.setLevel(AuditLine::Level(0));

    Debug(DNULL, pdDebug) << "Rcv NULL PRIME MSG (Federate=" << request.message()->getFederate()
                          << ", Time = " << request.message()->getDate().getTime() << ")" << endl;
    /* Update the NullPrimeDate of the concerned federate.
         * and check the result in order to decide whether
         * if the RTIG should send an anonymous NULL message or not
         */
    if (my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
            .updateLastNERxForFederate(request.message()->getFederate(), request.message()->getDate())) {
        std::unique_ptr<NM_Message_Null> nmsg = make_unique<NM_Message_Null>();
        nmsg->setDate(
            my_federations.searchFederation(FederationHandle(request.message()->getFederation())).getMinNERx());
        nmsg->setFederation(request.message()->getFederation());
        nmsg->setFederate(0);
        //nmsg.show(std::cout);
        return process({request.sockets().front(), std::move(nmsg)}, true);
    }

    return {};
}

Responses MessageProcessor::process(MessageEvent<NM_Register_Federation_Synchronization_Point>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(8));

    auto federation = FederationHandle(request.message()->getFederation());
    const auto& federate = request.message()->getFederate();
    const auto& label = request.message()->getLabel();
    const auto& tag = request.message()->getTag();

    Debug(D, pdTrace) << "Federation " << federation << ": registerFedSyncPoint from federate " << federate
                      << std::endl;

    my_auditServer << "Label \"" << label << "\" registered. Tag is \"" << tag << "\"";

    /* prepare answer to be sent to RTIA */
    auto rep = make_unique<NM_Confirm_Synchronization_Point_Registration>();
    rep->setFederate(federate);
    rep->setFederation(federation.get());
    rep->setLabel(label);

    try {
        // boolean true means a federates set exists
        if (request.message()->getExists()) {
            my_federations.searchFederation(federation)
                .registerSynchronization(federate, label, tag, request.message()->getFederates());
        }
        else {
            my_federations.searchFederation(federation).registerSynchronization(federate, label, tag);
        }

        rep->setSuccessIndicator(true);
    }
    catch (Exception& e) {
        /* the registration did fail */
        rep->setSuccessIndicator(false);
        rep->setFailureReason(e.name() + ":" + e.reason());
    }
    Debug(D, pdTerm) << "Federation " << federation << " is now synchronizing" << endl;

    Debug(G, pdGendoc) << "      processRegisterSynchronization====> write SPRS to RTIA" << endl;

    // proceed with the broadcast iff registration was successful
    if (rep->getSuccessIndicator()) {
        // boolean true means a federates set exists
        if (request.message()->getExists()) {
            responses = my_federations.searchFederation(federation)
                            .broadcastSynchronization(federate, label, tag, request.message()->getFederates());
        }
        else {
            responses = my_federations.searchFederation(federation).broadcastSynchronization(federate, label, tag);
        }
    }

    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Synchronization_Point_Achieved>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(8));

    Debug(D, pdTrace) << "Federation " << request.message()->getFederation()
                      << ": synchronizationPointAchieved from federate " << request.message()->getFederate()
                      << std::endl;

    my_auditServer << "Label \"" << request.message()->getLabel() << "\" ended";

    responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                    .unregisterSynchronization(request.message()->getFederate(), request.message()->getLabel());

    Debug(D, pdTerm) << "Federate " << request.message()->getFederate() << " has synchronized" << endl;

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Request_Federation_Save>&& request)
{
    Debug(D, pdTrace) << "Request federation save from federate " << request.message()->getFederate() << std::endl;
    my_auditServer.setLevel(AuditLine::Level(8));

    my_auditServer << "Federation save request";

    if (request.message()->isDated()) {
        // With time
        return my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
            .requestFederationSave(
                request.message()->getFederate(), request.message()->getLabel(), request.message()->getDate());
    }
    else {
        // Without time
        return my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
            .requestFederationSave(request.message()->getFederate(), request.message()->getLabel());
    }
}

Responses MessageProcessor::process(MessageEvent<NM_Federate_Save_Begun>&& request)
{
    Debug(D, pdTrace) << "Federate " << request.message()->getFederate() << " begun save" << std::endl;
    my_auditServer.setLevel(AuditLine::Level(8));

    Debug(G, pdGendoc) << "processFederateSaveBegun federation = " << request.message()->getFederation() << endl;

    my_auditServer << "Federate " << request.message()->getFederate() << " save begun";

    my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .federateSaveBegun(request.message()->getFederate());

    return {};
}

Responses MessageProcessor::process(MessageEvent<NM_Federate_Save_Complete>&& request)
{
    Debug(D, pdTrace) << "Federate " << request.message()->getFederate() << " save complete" << std::endl;
    my_auditServer.setLevel(AuditLine::Level(8));

    my_auditServer << "Federate " << request.message()->getFederate() << " save ended";

    return my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .federateSaveStatus(request.message()->getFederate(), true);
}

Responses MessageProcessor::process(MessageEvent<NM_Federate_Save_Not_Complete>&& request)
{
    Debug(D, pdTrace) << "Federate " << request.message()->getFederate() << " save not complete" << std::endl;
    my_auditServer.setLevel(AuditLine::Level(8));

    my_auditServer << "Federate " << request.message()->getFederate() << " save ended";

    return my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .federateSaveStatus(request.message()->getFederate(), false);
}

Responses MessageProcessor::process(MessageEvent<NM_Request_Federation_Restore>&& request)
{
    Debug(D, pdTrace) << "Federate " << request.message()->getFederate() << " request a restoration" << std::endl;
    my_auditServer.setLevel(AuditLine::Level(8));

    my_auditServer << "Federate " << request.message()->getFederate() << " request restore";

    return my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .requestFederationRestore(request.message()->getFederate(), request.message()->getLabel());
}

Responses MessageProcessor::process(MessageEvent<NM_Federate_Restore_Complete>&& request)
{
    Debug(D, pdTrace) << "Federate " << request.message()->getFederate() << " restore complete" << std::endl;
    my_auditServer.setLevel(AuditLine::Level(8));

    my_auditServer << "Federate " << request.message()->getFederate() << " restore ended";

    return my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .federateRestoreStatus(request.message()->getFederate(), true);
}

Responses MessageProcessor::process(MessageEvent<NM_Federate_Restore_Not_Complete>&& request)
{
    Debug(D, pdTrace) << "Federate " << request.message()->getFederate() << " restore not complete" << std::endl;
    my_auditServer.setLevel(AuditLine::Level(8));

    my_auditServer << "Federate " << request.message()->getFederate() << " restore ended";

    return my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .federateRestoreStatus(request.message()->getFederate(), false);
}

Responses MessageProcessor::process(MessageEvent<NM_Publish_Object_Class>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(7));

    my_auditServer << "Publish Object Class = " << request.message()->getObjectClass()
                   << ", # of att. = " << request.message()->getAttributesSize();

    responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                    .publishObject(request.message()->getFederate(),
                                   request.message()->getObjectClass(),
                                   request.message()->getAttributes(),
                                   true);

    Debug(D, pdRegister) << "Federate " << request.message()->getFederate() << " of Federation "
                         << request.message()->getFederation() << " published object class "
                         << request.message()->getObjectClass() << endl;

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Unpublish_Object_Class>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(7));

    my_auditServer << "Unpublish Object Class = " << request.message()->getObjectClass()
                   << ", # of att. = " << request.message()->getAttributesSize();

    responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                    .publishObject(request.message()->getFederate(),
                                   request.message()->getObjectClass(),
                                   request.message()->getAttributes(),
                                   false);

    Debug(D, pdRegister) << "Federate " << request.message()->getFederate() << " of Federation "
                         << request.message()->getFederation() << " unpublished object class "
                         << request.message()->getObjectClass() << endl;

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Subscribe_Object_Class>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(7));

    my_auditServer << "Subscribe Object Class = " << request.message()->getObjectClass()
                   << ", # of att. = " << request.message()->getAttributesSize();

    responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                    .subscribeObject(request.message()->getFederate(),
                                     request.message()->getObjectClass(),
                                     request.message()->getAttributes(),
                                     true);

    Debug(D, pdRegister) << "Federate " << request.message()->getFederate() << " of Federation "
                         << request.message()->getFederation() << " subscribed to object class "
                         << request.message()->getObjectClass() << endl;

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Unsubscribe_Object_Class>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(7));

    my_auditServer << "Unubscribe Object Class = " << request.message()->getObjectClass()
                   << ", # of att. = " << request.message()->getAttributesSize();

    responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                    .subscribeObject(request.message()->getFederate(), request.message()->getObjectClass(), {}, false);

    Debug(D, pdRegister) << "Federate " << request.message()->getFederate() << " of Federation "
                         << request.message()->getFederation() << " subscribed to object class "
                         << request.message()->getObjectClass() << endl;

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Publish_Interaction_Class>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(7));

    my_auditServer << "Publish Interaction Class = " << request.message()->getInteractionClass();

    responses
        = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
              .publishInteraction(request.message()->getFederate(), request.message()->getInteractionClass(), true);

    Debug(D, pdRequest) << "Federate " << request.message()->getFederate() << " of Federation "
                        << request.message()->getFederation() << " publishes Interaction "
                        << request.message()->getInteractionClass() << endl;

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Unpublish_Interaction_Class>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(7));

    my_auditServer << "Unpublish Interaction Class = " << request.message()->getInteractionClass();
    responses
        = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
              .publishInteraction(request.message()->getFederate(), request.message()->getInteractionClass(), false);
    Debug(D, pdRequest) << "Federate " << request.message()->getFederate() << " of Federation "
                        << request.message()->getFederation() << " unpublishes Interaction "
                        << request.message()->getInteractionClass() << endl;

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Subscribe_Interaction_Class>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(7));

    my_auditServer << "Subscribe Interaction Class = " << request.message()->getInteractionClass();

    responses
        = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
              .subscribeInteraction(request.message()->getFederate(), request.message()->getInteractionClass(), true);

    Debug(D, pdRequest) << "Federate " << request.message()->getFederate() << " of Federation "
                        << request.message()->getFederation() << " subscribed to Interaction "
                        << request.message()->getInteractionClass() << endl;

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Unsubscribe_Interaction_Class>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(7));

    my_auditServer << "Unsubscribe Interaction Class = " << request.message()->getInteractionClass();

    responses
        = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
              .subscribeInteraction(request.message()->getFederate(), request.message()->getInteractionClass(), false);

    Debug(D, pdRequest) << "Federate " << request.message()->getFederate() << " of Federation "
                        << request.message()->getFederation() << " unsubscribed to Interaction "
                        << request.message()->getInteractionClass() << endl;

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Reserve_Object_Instance_Name>&& request)
{
    my_auditServer.setLevel(AuditLine::Level(6));

    my_auditServer << "Reserve Object Name = " << request.message()->getObjectName();

    my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .reserveObjectInstanceName(request.message()->getFederate(), request.message()->getObjectName());

    return {};
}

Responses MessageProcessor::process(MessageEvent<NM_Register_Object>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    my_auditServer << "Register Object Class = " << request.message()->getObjectClass();

    ObjectHandle object_handle;
    std::tie(object_handle, responses)
        = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
              .registerObject(
                  request.message()->getFederate(), request.message()->getObjectClass(), request.message()->getLabel());

    Debug(D, pdRegister) << "Object \"" << request.message()->getLabel() << "\" of Federate "
                         << request.message()->getFederate() << " has been registered under ID " << object_handle
                         << endl;

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Update_Attribute_Values>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(1));

    my_auditServer << "ObjID = " << request.message()->getObject()
                   << ", Date = " << request.message()->getDate().getTime();

    // Forward the call
    if (request.message()->isDated()) {
        // UAV with time
        responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                        .updateAttributeValues(request.message()->getFederate(),
                                               request.message()->getObject(),
                                               request.message()->getAttributes(),
                                               request.message()->getValues(),
                                               request.message()->getDate(),
                                               request.message()->getLabel());
    }
    else {
        // UAV without time
        responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                        .updateAttributeValues(request.message()->getFederate(),
                                               request.message()->getObject(),
                                               request.message()->getAttributes(),
                                               request.message()->getValues(),
                                               request.message()->getLabel());
    }

    // Building answer (Network Message)
    auto rep = make_unique<NM_Update_Attribute_Values>();
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

    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Send_Interaction>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(2));

    // Building Value Array
    my_auditServer << "IntID = " << request.message()->getInteractionClass()
                   << ", date = " << request.message()->getDate().getTime();
    if (request.message()->isDated()) {
        responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                        .broadcastInteraction(request.message()->getFederate(),
                                              request.message()->getInteractionClass(),
                                              request.message()->getParameters(),
                                              request.message()->getValues(),
                                              request.message()->getDate(),
                                              request.message()->getRegion(),
                                              request.message()->getLabel());
    }
    else {
        responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                        .broadcastInteraction(request.message()->getFederate(),
                                              request.message()->getInteractionClass(),
                                              request.message()->getParameters(),
                                              request.message()->getValues(),
                                              request.message()->getParametersSize(),
                                              request.message()->getRegion(),
                                              request.message()->getLabel());
    }

    Debug(D, pdDebug) << "Interaction " << request.message()->getInteractionClass() << " parameters update completed"
                      << endl;

    auto rep = make_unique<NM_Send_Interaction>();
    rep->setFederate(request.message()->getFederate());
    rep->setInteractionClass(request.message()->getInteractionClass());
    // Don't forget label and tag
    if (request.message()->isDated()) {
        rep->setDate(request.message()->getDate());
    }
    if (request.message()->isLabelled()) {
        rep->setLabel(request.message()->getLabel());
    }
    if (request.message()->isTagged()) {
        rep->setTag(request.message()->getTag());
    }

    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Delete_Object>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    my_auditServer << "Delete ObjID = " << request.message()->getObject();

    if (request.message()->isDated()) {
        responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                        .deleteObject(request.message()->getFederate(),
                                      request.message()->getObject(),
                                      request.message()->getDate(),
                                      request.message()->getLabel());
    }
    else {
        responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                        .deleteObject(request.message()->getFederate(),
                                      request.message()->getObject(),
                                      request.message()->getLabel());
    }

    Debug(D, pdRegister) << "Object # " << request.message()->getObject() << " of Federation "
                         << request.message()->getFederation() << " has been deleted" << endl;

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Query_Attribute_Ownership>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(2));

    Debug(D, pdDebug) << "Owner of Attribute " << request.message()->getAttribute() << " of Object "
                      << request.message()->getObject() << endl;

    my_auditServer << "AttributeHandle = " << request.message()->getAttribute();

    my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .queryAttributeOwnership(
            request.message()->getFederate(), request.message()->getObject(), request.message()->getAttribute());

    Debug(D, pdDebug) << "Owner of Attribute " << request.message()->getAttribute() << " of Object "
                      << request.message()->getObject() << endl;

    auto rep = make_unique<NM_Query_Attribute_Ownership>();
    rep->setFederate(request.message()->getFederate());
    rep->setObject(request.message()->getObject());

    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Negotiated_Attribute_Ownership_Divestiture>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    my_auditServer << "Object = " << request.message()->getObject()
                   << ", # of att. = " << request.message()->getAttributesSize();
    responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                    .negotiateDivestiture(request.message()->getFederate(),
                                          request.message()->getObject(),
                                          request.message()->getAttributes(),
                                          request.message()->getLabel());

    Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                      << request.message()->getFederation() << " negotiate "
                                                               "divestiture of object "
                      << request.message()->getObject() << endl;

    auto rep = make_unique<NM_Negotiated_Attribute_Ownership_Divestiture>();
    rep->setFederate(request.message()->getFederate());
    rep->setObject(request.message()->getObject());

    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Attribute_Ownership_Acquisition_If_Available>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    my_auditServer << "Object = " << request.message()->getObject()
                   << ", # of att. = " << request.message()->getAttributesSize();

    my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .acquireIfAvailable(
            request.message()->getFederate(), request.message()->getObject(), request.message()->getAttributes());

    Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                      << request.message()->getFederation() << " acquisitionIfAvailable "
                                                               "of object "
                      << request.message()->getObject() << endl;

    auto rep = make_unique<NM_Attribute_Ownership_Acquisition_If_Available>();
    rep->setFederate(request.message()->getFederate());
    rep->setObject(request.message()->getObject());

    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Unconditional_Attribute_Ownership_Divestiture>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    my_auditServer << "Object = " << request.message()->getObject()
                   << ", # of att. = " << request.message()->getAttributesSize();

    responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                    .divest(request.message()->getFederate(),
                            request.message()->getObject(),
                            request.message()->getAttributes());

    Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                      << request.message()->getFederation() << " UnconditionalDivestiture "
                                                               "of object "
                      << request.message()->getObject() << endl;

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Attribute_Ownership_Acquisition>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    my_auditServer << "Object = " << request.message()->getObject()
                   << ", # of att. = " << request.message()->getAttributesSize();

    my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .acquire(request.message()->getFederate(),
                 request.message()->getObject(),
                 request.message()->getAttributes(),
                 request.message()->getLabel());

    Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                      << request.message()->getFederation() << " ownership acquisition of object "
                      << request.message()->getObject() << endl;

    auto rep = make_unique<NM_Attribute_Ownership_Acquisition>();
    rep->setFederate(request.message()->getFederate());
    rep->setObject(request.message()->getObject());
    rep->setAttributesSize(0);

    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Cancel_Negotiated_Attribute_Ownership_Divestiture>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    my_auditServer << "Object = " << request.message()->getObject()
                   << ", # of att. = " << request.message()->getAttributesSize();

    my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .cancelDivestiture(
            request.message()->getFederate(), request.message()->getObject(), request.message()->getAttributes());

    Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                      << request.message()->getFederation() << " cancel negotiate "
                                                               "divestiture of object "
                      << request.message()->getObject() << endl;

    auto rep = make_unique<NM_Cancel_Negotiated_Attribute_Ownership_Divestiture>();
    rep->setFederate(request.message()->getFederate());
    rep->setObject(request.message()->getObject());

    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Is_Attribute_Owned_By_Federate>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(2));

    auto rep = make_unique<NM_Is_Attribute_Owned_By_Federate>();

    Debug(D, pdDebug) << "Owner of Attribute " << request.message()->getAttribute() << " of Object "
                      << request.message()->getObject() << endl;

    my_auditServer << "AttributeHandle = " << request.message()->getAttribute();

    if (my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
            .isOwner(
                request.message()->getFederate(), request.message()->getObject(), request.message()->getAttribute())) {
        rep->setLabel("RTI_TRUE");
    }
    else {
        rep->setLabel("RTI_FALSE");
    }

    Debug(D, pdDebug) << "Owner of Attribute " << request.message()->getAttribute() << " of Object "
                      << request.message()->getObject() << endl;

    rep->setFederate(request.message()->getFederate());
    rep->setObject(request.message()->getObject());
    rep->setAttribute(request.message()->getAttribute());

    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Attribute_Ownership_Release_Response>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    my_auditServer << "Object = " << request.message()->getObject()
                   << ", # of att. = " << request.message()->getAttributesSize();

    AttributeHandleSet* attributes
        = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
              .respondRelease(
                  request.message()->getFederate(), request.message()->getObject(), request.message()->getAttributes());

    Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                      << request.message()->getFederation() << " release response "
                                                               "of object "
                      << request.message()->getObject() << endl;

    auto rep = make_unique<NM_Attribute_Ownership_Release_Response>();
    rep->setAttributesSize(attributes->size());

    for (auto i(0u); i < attributes->size(); ++i) {
        rep->setAttributes(attributes->getHandle(i), i);
    }

    rep->setFederate(request.message()->getFederate());
    rep->setObject(request.message()->getObject());

    responses.emplace_back(request.sockets().front(), std::move(rep)); // Send answer to RTIA

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Cancel_Attribute_Ownership_Acquisition>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    my_auditServer << "Object = " << request.message()->getObject()
                   << ", # of att. = " << request.message()->getAttributesSize();

    my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .cancelAcquisition(
            request.message()->getFederate(), request.message()->getObject(), request.message()->getAttributes());

    Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                      << request.message()->getFederation() << " release response of object "
                      << request.message()->getObject() << endl;

    auto rep = make_unique<NM_Cancel_Attribute_Ownership_Acquisition>();
    rep->setFederate(request.message()->getFederate());
    rep->setObject(request.message()->getObject());

    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_DDM_Create_Region>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    // TODO: audit...

    auto rep = make_unique<NM_DDM_Create_Region>();
    rep->setRegion(my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                       .createRegion(request.message()->getFederate(),
                                     request.message()->getSpace(),
                                     request.message()->getNbExtents()));

    Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                      << request.message()->getFederation() << " creates region " << rep->getRegion() << endl;

    rep->setFederate(request.message()->getFederate());
    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_DDM_Modify_Region>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                      << request.message()->getFederation() << " modifies region " << request.message()->getRegion()
                      << endl;

    my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .modifyRegion(
            request.message()->getFederate(), request.message()->getRegion(), request.message()->getExtents());

    auto rep = make_unique<NM_DDM_Modify_Region>();
    rep->setFederate(request.message()->getFederate());
    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_DDM_Delete_Region>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    // TODO: audit...

    Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                      << request.message()->getFederation() << " deletes region " << request.message()->getRegion()
                      << endl;

    my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .deleteRegion(request.message()->getFederate(), request.message()->getRegion());

    auto rep = make_unique<NM_DDM_Delete_Region>();

    rep->setFederate(request.message()->getFederate());
    rep->setRegion(request.message()->getRegion());
    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_DDM_Associate_Region>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    // TODO: audit...

    Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                      << request.message()->getFederation() << " associates region " << request.message()->getRegion()
                      << " to some attributes of object " << request.message()->getObject() << endl;

    my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .associateRegion(request.message()->getFederate(),
                         request.message()->getObject(),
                         request.message()->getRegion(),
                         request.message()->getAttributes());

    auto rep = make_unique<NM_DDM_Associate_Region>();
    rep->setFederate(request.message()->getFederate());
    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_DDM_Unassociate_Region>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));
    {
        // TODO: audit...

        my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
            .unassociateRegion(
                request.message()->getFederate(), request.message()->getObject(), request.message()->getRegion());

        Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                          << request.message()->getFederation() << " associates region "
                          << request.message()->getRegion() << " from object " << request.message()->getObject()
                          << endl;

        auto rep = make_unique<NM_DDM_Unassociate_Region>();
        rep->setFederate(request.message()->getFederate());
        responses.emplace_back(request.sockets().front(), std::move(rep));
    }
    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_DDM_Subscribe_Attributes>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    // TODO: audit...
    Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                      << request.message()->getFederation() << " subscribes with region "
                      << request.message()->getRegion() << " to some attributes of class "
                      << request.message()->getObjectClass() << endl;

    my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .subscribeAttributesWR(request.message()->getFederate(),
                               request.message()->getObjectClass(),
                               request.message()->getRegion(),
                               request.message()->getAttributes());

    auto rep = make_unique<NM_DDM_Subscribe_Attributes>();
    rep->setFederate(request.message()->getFederate());
    rep->setObjectClass(request.message()->getObjectClass());
    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_DDM_Unsubscribe_Attributes>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    // TODO: audit...
    Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                      << request.message()->getFederation() << " unsubscribes with region "
                      << request.message()->getRegion() << " from object class " << request.message()->getObjectClass()
                      << endl;

    my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .unsubscribeAttributesWR(
            request.message()->getFederate(), request.message()->getObjectClass(), request.message()->getRegion());

    auto rep = make_unique<NM_DDM_Unsubscribe_Attributes>();
    rep->setFederate(request.message()->getFederate());
    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_DDM_Subscribe_Interaction>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    // TODO: audit...

    my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .subscribeInteractionWR(
            request.message()->getFederate(), request.message()->getInteractionClass(), request.message()->getRegion());

    Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                      << request.message()->getFederation() << " subscribes with region "
                      << request.message()->getRegion() << " to interaction class "
                      << request.message()->getInteractionClass() << endl;

    auto rep = make_unique<NM_DDM_Subscribe_Interaction>();
    rep->setFederate(request.message()->getFederate());
    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_DDM_Unsubscribe_Interaction>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    // TODO: audit...

    my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .unsubscribeInteractionWR(
            request.message()->getFederate(), request.message()->getInteractionClass(), request.message()->getRegion());

    Debug(D, pdDebug) << "Federate " << request.message()->getFederate() << " of Federation "
                      << request.message()->getFederation() << " unsubscribes with region "
                      << request.message()->getRegion() << " from interaction class "
                      << request.message()->getInteractionClass() << endl;

    auto rep = make_unique<NM_DDM_Unsubscribe_Interaction>();
    rep->setFederate(request.message()->getFederate());
    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_DDM_Register_Object>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    auto rep = make_unique<NM_DDM_Register_Object>();
    // FIXME bug #9869
    // When we were passed a set of region
    // we should register object for each region
    // the trouble comes from the fact that
    // there is both
    //     - request.message()->region  (coming from NetworkMessage::region)
    //     - request.message()->regions (coming from BasicMessage::regions)
    // would be nice to sort those thing out.
    ObjectHandle object_handle;
    std::tie(object_handle, responses)
        = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
              .registerObjectWithRegion(request.message()->getFederate(),
                                        request.message()->getObjectClass(),
                                        request.message()->getLabel(),
                                        request.message()->getRegion(),
                                        request.message()->getAttributes());

    Debug(D, pdRegister) << "Object \"" << request.message()->getLabel() << "\" of Federate "
                         << request.message()->getFederate() << " has been registered under ID " << rep->getObject()
                         << endl;

    rep->setFederate(request.message()->getFederate());
    responses.emplace_back(request.sockets().front(), std::move(rep)); // Send answer to RTIA

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Request_Object_Attribute_Value_Update>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    my_auditServer << "ObjID = " << request.message()->getObject();

    responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                    .requestObjectOwner(request.message()->getFederate(),
                                        request.message()->getObject(),
                                        request.message()->getAttributes())
                    .second;

    auto rep = make_unique<NM_Request_Object_Attribute_Value_Update>();
    rep->setException(Exception::Type::NO_EXCEPTION);
    rep->setFederate(request.message()->getFederate());
    rep->setObject(request.message()->getObject());

    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Request_Class_Attribute_Value_Update>&& request)
{
    Responses responses;

    my_auditServer.setLevel(AuditLine::Level(6));

    responses = my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
                    .requestClassAttributeValueUpdate(request.message()->getFederate(),
                                                      request.message()->getObjectClass(),
                                                      request.message()->getAttributes());

    auto rep = make_unique<NM_Request_Class_Attribute_Value_Update>();
    rep->setException(Exception::Type::NO_EXCEPTION);
    rep->setFederate(request.message()->getFederate());
    rep->setObjectClass(request.message()->getObjectClass());

    responses.emplace_back(request.sockets().front(), std::move(rep));

    return responses;
}

Responses MessageProcessor::process(MessageEvent<NM_Enable_Asynchronous_Delivery>&& request)
{
    return my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .updateAsynchronousDelivery(request.message()->getFederate(), true);
}

Responses MessageProcessor::process(MessageEvent<NM_Disable_Asynchronous_Delivery>&& request)
{
    return my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
        .updateAsynchronousDelivery(request.message()->getFederate(), false);
}

Responses MessageProcessor::process(MessageEvent<NM_Time_State_Update>&& request)
{
    Debug(DTUS, pdDebug) << "Rcv Time State Update (Federate=" << request.message()->getFederate()
                          << ", Time = " << request.message()->getDate().getTime() << ")" << endl;
        
    // Catch all exceptions because RTIA does not expect an answer anyway.
    try {
        return my_federations.searchFederation(FederationHandle(request.message()->getFederation()))
            .updateTimeState(request.message()->getFederate(),
                             request.message()->getDate(),
                             request.message()->getLookahead(),
                             request.message()->getState(),
                             request.message()->getGalt(),
                             request.message()->getLits());
    }
    catch (Exception& e) {
    }

    return {};
}
}
}
