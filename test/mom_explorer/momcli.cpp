#include "momcli.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <iterator>
#include <RTI/Enums.h>
#include <RTI/RTI1516.h>
#include <RTI/RTI1516fedTime.h>

MomCli::MomCli(rti1516e::RTIambassador& amb, const std::wstring& federation_name) : cli(), my_ambassador(amb)
{
    registerCommand("tick", std::bind(&MomCli::command_tick, this));
    registerCommand("resign federation execution", std::bind(&MomCli::command_resign_federation_execution, this));
    registerCommand("destroy federation execution", std::bind(&MomCli::command_destroy_federation_execution, this, federation_name));
    registerCommand("enable time regulation", std::bind(&MomCli::command_enable_time_regulation, this));
    registerCommand("enable time constrained", std::bind(&MomCli::command_enable_time_constrained, this));
    registerCommand("disable time regulation", std::bind(&MomCli::command_disable_time_regulation, this));
    registerCommand("disable time constrained", std::bind(&MomCli::command_disable_time_constrained, this));

    registerCommand("federate adjust set timing", std::bind(&MomCli::command_federate_adjust_set_timing, this));
    registerCommand("federate adjust modify attribute state",
                    std::bind(&MomCli::command_federate_adjust_modify_attribute_state, this));
    registerCommand("federate adjust set switches", std::bind(&MomCli::command_federate_adjust_set_switches, this));
    registerCommand("federate request publications", std::bind(&MomCli::command_federate_request_publications, this));
    registerCommand("federate request subscriptions", std::bind(&MomCli::command_federate_request_subscriptions, this));
    registerCommand("federate request object instances that can be deleted",
                    std::bind(&MomCli::command_federate_request_object_instances_that_can_be_deleted, this));
    registerCommand("federate request object instances updated",
                    std::bind(&MomCli::command_federate_request_object_instances_updated, this));
    registerCommand("federate request object instances reflected",
                    std::bind(&MomCli::command_federate_request_object_instances_reflected, this));
    registerCommand("federate request updates sent", std::bind(&MomCli::command_federate_request_updates_sent, this));
    registerCommand("federate request interactions sent",
                    std::bind(&MomCli::command_federate_request_interactions_sent, this));
    registerCommand("federate request reflections received",
                    std::bind(&MomCli::command_federate_request_reflections_received, this));
    registerCommand("federate request interactions received",
                    std::bind(&MomCli::command_federate_request_interactions_received, this));
    registerCommand("federate request object instance information",
                    std::bind(&MomCli::command_federate_request_object_instance_information, this));
    registerCommand("federate request FOM module data",
                    std::bind(&MomCli::command_federate_request_FOM_module_data, this));
    registerCommand("federate service resign federation execution",
                    std::bind(&MomCli::command_federate_service_resign_federation_execution, this));
    registerCommand("federate service synchronization point achieved",
                    std::bind(&MomCli::command_federate_service_synchronization_point_achieved, this));
    registerCommand("federate service federate save begun",
                    std::bind(&MomCli::command_federate_service_federate_save_begun, this));
    registerCommand("federate service federate save complete",
                    std::bind(&MomCli::command_federate_service_federate_save_complete, this));
    registerCommand("federate service federate restore complete",
                    std::bind(&MomCli::command_federate_service_federate_restore_complete, this));
    registerCommand("federate service publish object class attributes",
                    std::bind(&MomCli::command_federate_service_publish_object_class_attributes, this));
    registerCommand("federate service un publish object class attributes",
                    std::bind(&MomCli::command_federate_service_un_publish_object_class_attributes, this));
    registerCommand("federate service publish interaction class",
                    std::bind(&MomCli::command_federate_service_publish_interaction_class, this));
    registerCommand("federate service un publish interaction class",
                    std::bind(&MomCli::command_federate_service_un_publish_interaction_class, this));
    registerCommand("federate service subscribe object class attributes",
                    std::bind(&MomCli::command_federate_service_subscribe_object_class_attributes, this));
    registerCommand("federate service un subscribe object class attributes",
                    std::bind(&MomCli::command_federate_service_un_subscribe_object_class_attributes, this));
    registerCommand("federate service subscribe interaction class",
                    std::bind(&MomCli::command_federate_service_subscribe_interaction_class, this));
    registerCommand("federate service un subscribe interaction class",
                    std::bind(&MomCli::command_federate_service_un_subscribe_interaction_class, this));
    registerCommand("federate service delete object instance",
                    std::bind(&MomCli::command_federate_service_delete_object_instance, this));
    registerCommand("federate service local delete object instance",
                    std::bind(&MomCli::command_federate_service_local_delete_object_instance, this));
    registerCommand("federate service request attribute transportation type change",
                    std::bind(&MomCli::command_federate_service_request_attribute_transportation_type_change, this));
    registerCommand("federate service request interaction transportation type change",
                    std::bind(&MomCli::command_federate_service_request_interaction_transportation_type_change, this));
    registerCommand("federate service unconditional attribute ownership divestiture",
                    std::bind(&MomCli::command_federate_service_unconditional_attribute_ownership_divestiture, this));
    registerCommand("federate service enable time regulation",
                    std::bind(&MomCli::command_federate_service_enable_time_regulation, this));
    registerCommand("federate service disable time regulation",
                    std::bind(&MomCli::command_federate_service_disable_time_regulation, this));
    registerCommand("federate service enable time constrained",
                    std::bind(&MomCli::command_federate_service_enable_time_constrained, this));
    registerCommand("federate service disable time constrained",
                    std::bind(&MomCli::command_federate_service_disable_time_constrained, this));
    registerCommand("federate service time advance request",
                    std::bind(&MomCli::command_federate_service_time_advance_request, this));
    registerCommand("federate service time advance request available",
                    std::bind(&MomCli::command_federate_service_time_advance_request_available, this));
    registerCommand("federate service next message request",
                    std::bind(&MomCli::command_federate_service_next_message_request, this));
    registerCommand("federate service next message request available",
                    std::bind(&MomCli::command_federate_service_next_message_request_available, this));
    registerCommand("federate service flush queue request",
                    std::bind(&MomCli::command_federate_service_flush_queue_request, this));
    registerCommand("federate service enable asynchronous delivery",
                    std::bind(&MomCli::command_federate_service_enable_asynchronous_delivery, this));
    registerCommand("federate service disable asynchronous delivery",
                    std::bind(&MomCli::command_federate_service_disable_asynchronous_delivery, this));
    registerCommand("federate service modify lookahead",
                    std::bind(&MomCli::command_federate_service_modify_lookahead, this));
    registerCommand("federate service change attribute order type",
                    std::bind(&MomCli::command_federate_service_change_attribute_order_type, this));
    registerCommand("federate service change interaction order type",
                    std::bind(&MomCli::command_federate_service_change_interaction_order_type, this));
    registerCommand("Federation adjust set switches", std::bind(&MomCli::command_federation_adjust_set_switches, this));
    registerCommand("Federation request synchronization points",
                    std::bind(&MomCli::command_federation_request_synchronization_points, this));
    registerCommand("Federation request synchronization point status",
                    std::bind(&MomCli::command_federation_request_synchronization_point_status, this));
    registerCommand("Federation request FOM module data",
                    std::bind(&MomCli::command_federation_request_FOM_module_data, this));
    registerCommand("Federation request MIM data", std::bind(&MomCli::command_federation_request_MIM_data, this));
}

void MomCli::command_tick()
{
    std::wcout << "=>evokeCallback(0.1)\n";
    my_ambassador.evokeCallback(0.1);
}

void MomCli::command_resign_federation_execution()
{
    std::wcout << "=>resignFederationExecution(NO_ACTION)\n";
    my_ambassador.resignFederationExecution(rti1516e::NO_ACTION);
}

void MomCli::command_destroy_federation_execution(const std::wstring& federation_name)
{
    std::wcout << "=>destroyFederationExecution <" << federation_name << ">\n";
    my_ambassador.destroyFederationExecution(federation_name);
}

void MomCli::command_enable_time_regulation()
{
    std::wcout << "=>enableTimeRegulation\n";
    my_ambassador.enableTimeRegulation(RTI1516fedTimeInterval(1.0));
}

void MomCli::command_enable_time_constrained()
{
    std::wcout << "=>enableTimeConstrained\n";
    my_ambassador.enableTimeConstrained();
}

void MomCli::command_disable_time_regulation()
{
    std::wcout << "=>disableTimeRegulation\n";
    my_ambassador.disableTimeRegulation();
}

void MomCli::command_disable_time_constrained()
{
    std::wcout << "=>disableTimeConstrained\n";
    my_ambassador.disableTimeConstrained();
}

void MomCli::command_federate_adjust_set_timing()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAadjust.HLAsetTiming\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int report_period = std::stoi(requestArgument("HLAreportPeriod"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAadjust.HLAsetTiming",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAreportPeriod", {&report_period, 4}}});
}

void MomCli::command_federate_adjust_modify_attribute_state()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAadjust.HLAmodifyAttributeState\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int object_instance = std::stoi(requestArgument("HLAobjectInstance"));
    int attribute = std::stoi(requestArgument("HLAattribute"));
    bool attr_state = std::stoi(requestArgument("HLAattributeState"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAadjust.HLAmodifyAttributeState",
                    {{L"HLAfederate", {&federate_handle, 4}},
                     {L"HLAobjectInstance", {&object_instance, 4}},
                     {L"HLAattribute", {&attribute, 4}},
                     {L"HLAattributeState", {&attr_state, 4}}});
}

void MomCli::command_federate_adjust_set_switches()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAadjust.HLAsetSwitches\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    bool crds = std::stoi(requestArgument("HLAconveyRegionDesignatorSets"));
    bool cpf = std::stoi(requestArgument("HLAconveyProducingFederate"));
    bool sr = std::stoi(requestArgument("HLAserviceReporting"));
    bool er = std::stoi(requestArgument("HLAexceptionReporting"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAadjust.HLAsetSwitches",
                    {{L"HLAfederate", {&federate_handle, 4}},
                     {L"HLAconveyRegionDesignatorSets", {&crds, 4}},
                     {L"HLAconveyProducingFederate", {&cpf, 4}},
                     {L"HLAserviceReporting", {&sr, 4}},
                     {L"HLAexceptionReporting", {&er, 4}}});
}

void MomCli::command_federate_request_publications()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLArequest.HLArequestPublications\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));

    sendInteraction(L"HLAmanager.HLAfederate.HLArequest.HLArequestPublications",
                    {{L"HLAfederate", {&federate_handle, 4}}});
#if 0
        try {
            std::string tag = "";
            auto fedIch = my_ambassador.getInteractionClassHandle(L"HLAmanager.HLAfederate");
            auto ich
                = my_ambassador.getInteractionClassHandle(L"HLAmanager.HLAfederate.HLArequest.HLArequestPublications");

            my_ambassador.sendInteraction(
                ich,
                {{my_ambassador.getParameterHandle(fedIch, L"HLAfederate"), {&federate_handle, 4}}},
                {tag.c_str(), tag.size()});
        }
        catch (rti1516e::Exception& e) {
            std::wcerr << "**Caught exception: " << e << std::endl;
        }
#endif
}

void MomCli::command_federate_request_subscriptions()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLArequest.HLArequestSubscriptions\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));

    sendInteraction(L"HLAmanager.HLAfederate.HLArequest.HLArequestSubscriptions",
                    {{L"HLAfederate", {&federate_handle, 4}}});
}

void MomCli::command_federate_request_object_instances_that_can_be_deleted()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesThatCanBeDeleted\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));

    sendInteraction(L"HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesThatCanBeDeleted",
                    {{L"HLAfederate", {&federate_handle, 4}}});
}

void MomCli::command_federate_request_object_instances_updated()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesUpdated\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));

    sendInteraction(L"HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesUpdated",
                    {{L"HLAfederate", {&federate_handle, 4}}});
}

void MomCli::command_federate_request_object_instances_reflected()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesReflected\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));

    sendInteraction(L"HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstancesReflected",
                    {{L"HLAfederate", {&federate_handle, 4}}});
}

void MomCli::command_federate_request_updates_sent()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLArequest.HLArequestUpdatesSent\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));

    sendInteraction(L"HLAmanager.HLAfederate.HLArequest.HLArequestUpdatesSent",
                    {{L"HLAfederate", {&federate_handle, 4}}});
}

void MomCli::command_federate_request_interactions_sent()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLArequest.HLArequestInteractionsSent\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));

    sendInteraction(L"HLAmanager.HLAfederate.HLArequest.HLArequestInteractionsSent",
                    {{L"HLAfederate", {&federate_handle, 4}}});
}

void MomCli::command_federate_request_reflections_received()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLArequest.HLArequestReflectionsReceived\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));

    sendInteraction(L"HLAmanager.HLAfederate.HLArequest.HLArequestReflectionsReceived",
                    {{L"HLAfederate", {&federate_handle, 4}}});
}

void MomCli::command_federate_request_interactions_received()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLArequest.HLArequestInteractionsReceived\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));

    sendInteraction(L"HLAmanager.HLAfederate.HLArequest.HLArequestInteractionsReceived",
                    {{L"HLAfederate", {&federate_handle, 4}}});
}

void MomCli::command_federate_request_object_instance_information()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstanceInformation\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int object = std::stoi(requestArgument("HLAobjectInstance"));

    sendInteraction(L"HLAmanager.HLAfederate.HLArequest.HLArequestObjectInstanceInformation",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAobjectInstance", {&object, 4}}});
}

void MomCli::command_federate_request_FOM_module_data()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLArequest.HLArequestFOMmoduleData\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int fom_module = std::stoi(requestArgument("HLAFOMmoduleIndicator"));

    sendInteraction(L"HLAmanager.HLAfederate.HLArequest.HLArequestFOMmoduleData",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAFOMmoduleIndicator", {&fom_module, 4}}});
}

void MomCli::command_federate_service_resign_federation_execution()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAresignFederationExecution\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int action = std::stoi(requestArgument("HLAresignAction"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAresignFederationExecution",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAresignAction", {&action, 4}}});
}

void MomCli::command_federate_service_synchronization_point_achieved()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAsynchronizationPointAchieved\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    std::string label = requestArgument("HLAlabel");

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAsynchronizationPointAchieved",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAlabel", {label.c_str(), label.size()}}});
}

void MomCli::command_federate_service_federate_save_begun()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAfederateSaveBegun\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAfederateSaveBegun",
                    {{L"HLAfederate", {&federate_handle, 4}}});
}

void MomCli::command_federate_service_federate_save_complete()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAfederateSaveComplete\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    bool success_indicator = std::stoi(requestArgument("HLAsuccessIndicator"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAfederateSaveComplete",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAsuccessIndicator", {&success_indicator, 4}}});
}

void MomCli::command_federate_service_federate_restore_complete()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAfederateRestoreComplete\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    bool success_indicator = std::stoi(requestArgument("HLAsuccessIndicator"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAfederateRestoreComplete",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAsuccessIndicator", {&success_indicator, 4}}});
}

void MomCli::command_federate_service_publish_object_class_attributes()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLApublishObjectClassAttributes\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int object_class = std::stoi(requestArgument("HLAobjectClass"));
    auto attr_list = requestVectorArguments("HLAattributeList");

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLApublishObjectClassAttributes",
                    {{L"HLAfederate", {&federate_handle, 4}},
                     {L"HLAobjectClass", {&object_class, 4}},
                     {L"HLAattributeList", {&attr_list[0], attr_list.size() * 4}}});
}

void MomCli::command_federate_service_un_publish_object_class_attributes()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAunpublishObjectClassAttributes\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int object_class = std::stoi(requestArgument("HLAobjectClass"));
    auto attr_list = requestVectorArguments("HLAattributeList");

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAunpublishObjectClassAttributes",
                    {{L"HLAfederate", {&federate_handle, 4}},
                     {L"HLAobjectClass", {&object_class, 4}},
                     {L"HLAattributeList", {&attr_list[0], attr_list.size() * 4}}});
}

void MomCli::command_federate_service_publish_interaction_class()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLApublishInteractionClass\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int interaction_class = std::stoi(requestArgument("HLAinteractionClass"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLApublishInteractionClass",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAinteractionClass", {&interaction_class, 4}}});
}

void MomCli::command_federate_service_un_publish_interaction_class()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAunpublishInteractionClass\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int interaction_class = std::stoi(requestArgument("HLAinteractionClass"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAunpublishInteractionClass",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAinteractionClass", {&interaction_class, 4}}});
}

void MomCli::command_federate_service_subscribe_object_class_attributes()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAsubscribeObjectClassAttributes\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int object_class = std::stoi(requestArgument("HLAobjectClass"));
    auto attr_list = requestVectorArguments("HLAattributeList");
    bool active = std::stoi(requestArgument("HLAactive"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAsubscribeObjectClassAttributes",
                    {{L"HLAfederate", {&federate_handle, 4}},
                     {L"HLAobjectClass", {&object_class, 4}},
                     {L"HLAattributeList", {&attr_list[0], attr_list.size() * 4}},
                     {L"HLAactive", {&active, 4}}});
}

void MomCli::command_federate_service_un_subscribe_object_class_attributes()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAunsubscribeObjectClassAttributes\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int object_class = std::stoi(requestArgument("HLAobjectClass"));
    auto attr_list = requestVectorArguments("HLAattributeList");

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAunsubscribeObjectClassAttributes",
                    {{L"HLAfederate", {&federate_handle, 4}},
                     {L"HLAobjectClass", {&object_class, 4}},
                     {L"HLAattributeList", {&attr_list[0], attr_list.size() * 4}}});
}

void MomCli::command_federate_service_subscribe_interaction_class()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLArequest.HLAsubscribeInteractionClass\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int interaction_class = std::stoi(requestArgument("HLAinteractionClass"));
    bool active = std::stoi(requestArgument("HLAactive"));

    sendInteraction(L"HLAmanager.HLAfederate.HLArequest.HLAsubscribeInteractionClass",
                    {{L"HLAfederate", {&federate_handle, 4}},
                     {L"HLAinteractionClass", {&interaction_class, 4}},
                     {L"HLAactive", {&active, 4}}});
}

void MomCli::command_federate_service_un_subscribe_interaction_class()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAunsubscribeInteractionClass\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int interaction_class = std::stoi(requestArgument("HLAinteractionClass"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAunsubscribeInteractionClass",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAinteractionClass", {&interaction_class, 4}}});
}

void MomCli::command_federate_service_delete_object_instance()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAdeleteObjectInstance\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int object = std::stoi(requestArgument("HLAobjectInstance"));
    std::string tag = requestArgument("HLAtag");
    double timestamp = std::stod(requestArgument("HLAtimeStamp"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAdeleteObjectInstance",
                    {{L"HLAfederate", {&federate_handle, 4}},
                     {L"HLAobjectInstance", {&object, 4}},
                     {L"HLAtag", {tag.c_str(), tag.size()}},
                     {L"HLAtimeStamp", {&timestamp, 8}}});
}

void MomCli::command_federate_service_local_delete_object_instance()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAlocalDeleteObjectInstance\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int object = std::stoi(requestArgument("HLAobjectInstance"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAlocalDeleteObjectInstance",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAobjectInstance", {&object, 4}}});
}

void MomCli::command_federate_service_request_attribute_transportation_type_change()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLArequestAttributeTransportationTypeChange\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int object = std::stoi(requestArgument("HLAobjectInstance"));
    auto attr_list = requestVectorArguments("HLAattributeList");
    std::string transportation = requestArgument("HLAtransportation");

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLArequestAttributeTransportationTypeChange",
                    {{L"HLAfederate", {&federate_handle, 4}},
                     {L"HLAobjectInstance", {&object, 4}},
                     {L"HLAattributeList", {&attr_list[0], attr_list.size() * 4}},
                     {L"HLAtransportation", {transportation.c_str(), transportation.size()}}});
}

void MomCli::command_federate_service_request_interaction_transportation_type_change()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLArequestInteractionTransportationTypeChange\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int interaction_class = std::stoi(requestArgument("HLAinteractionClass"));
    std::string transportation = requestArgument("HLAtransportation");

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLArequestInteractionTransportationTypeChange",
                    {{L"HLAfederate", {&federate_handle, 4}},
                     {L"HLAinteractionClass", {&interaction_class, 4}},
                     {L"HLAtransportation", {transportation.c_str(), transportation.size()}}});
}

void MomCli::command_federate_service_unconditional_attribute_ownership_divestiture()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAunconditionalAttributeOwnershipDivestiture\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int object = std::stoi(requestArgument("HLAobjectInstance"));
    auto attr_list = requestVectorArguments("HLAattributeList");

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAunconditionalAttributeOwnershipDivestiture",
                    {{L"HLAfederate", {&federate_handle, 4}},
                     {L"HLAobjectInstance", {&object, 4}},
                     {L"HLAattributeList", {&attr_list[0], attr_list.size() * 4}}});
}

void MomCli::command_federate_service_enable_time_regulation()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAenableTimeRegulation\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    double lookahead = std::stod(requestArgument("HLAlookahead"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAenableTimeRegulation",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAlookahead", {&lookahead, 8}}});
}

void MomCli::command_federate_service_disable_time_regulation()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAdisableTimeRegulation\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAdisableTimeRegulation",
                    {{L"HLAfederate", {&federate_handle, 4}}});
}

void MomCli::command_federate_service_enable_time_constrained()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAenableTimeConstrained\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAenableTimeConstrained",
                    {{L"HLAfederate", {&federate_handle, 4}}});
}

void MomCli::command_federate_service_disable_time_constrained()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAdisableTimeConstrained\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAdisableTimeConstrained",
                    {{L"HLAfederate", {&federate_handle, 4}}});
}

void MomCli::command_federate_service_time_advance_request()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAtimeAdvanceRequest\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    double timestamp = std::stod(requestArgument("HLAtimeStamp"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAtimeAdvanceRequest",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAtimeStamp", {&timestamp, 8}}});
}

void MomCli::command_federate_service_time_advance_request_available()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAtimeAdvanceRequestAvailable\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    double timestamp = std::stod(requestArgument("HLAtimeStamp"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAtimeAdvanceRequestAvailable",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAtimeStamp", {&timestamp, 8}}});
}

void MomCli::command_federate_service_next_message_request()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAnextMessageRequest\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    double timestamp = std::stod(requestArgument("HLAtimeStamp"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAnextMessageRequest",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAtimeStamp", {&timestamp, 8}}});
}

void MomCli::command_federate_service_next_message_request_available()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAnextMessageRequestAvailable\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    double timestamp = std::stod(requestArgument("HLAtimeStamp"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAnextMessageRequestAvailable",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAtimeStamp", {&timestamp, 8}}});
}

void MomCli::command_federate_service_flush_queue_request()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAflushQueueRequest\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    double timestamp = std::stod(requestArgument("HLAtimeStamp"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAflushQueueRequest",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAtimeStamp", {&timestamp, 8}}});
}

void MomCli::command_federate_service_enable_asynchronous_delivery()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAenableAsynchronousDelivery\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAenableAsynchronousDelivery",
                    {{L"HLAfederate", {&federate_handle, 4}}});
}

void MomCli::command_federate_service_disable_asynchronous_delivery()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAdisableAsynchronousDelivery\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAdisableAsynchronousDelivery",
                    {{L"HLAfederate", {&federate_handle, 4}}});
}

void MomCli::command_federate_service_modify_lookahead()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAmodifyLookahead\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    double lookahead = std::stod(requestArgument("HLAlookahead"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAmodifyLookahead",
                    {{L"HLAfederate", {&federate_handle, 4}}, {L"HLAlookahead", {&lookahead, 8}}});
}

void MomCli::command_federate_service_change_attribute_order_type()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAchangeAttributeOrderType\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int object = std::stoi(requestArgument("HLAobjectInstance"));
    auto attr_list = requestVectorArguments("HLAattributeList");
    int order = std::stoi(requestArgument("HLAsendOrder"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAchangeAttributeOrderType",
                    {{L"HLAfederate", {&federate_handle, 4}},
                     {L"HLAobjectInstance", {&object, 4}},
                     {L"HLAattributeList", {&attr_list[0], attr_list.size() * 4}},
                     {L"HLAsendOrder", {&order, 4}}});
}

void MomCli::command_federate_service_change_interaction_order_type()
{
    std::wcout << "=>HLAmanager.HLAfederate.HLAservice.HLAchangeInteractionOrderType\n";

    int federate_handle = std::stoi(requestArgument("HLAfederate"));
    int interaction_class = std::stoi(requestArgument("HLAinteractionClass"));
    int order = std::stoi(requestArgument("HLAsendOrder"));

    sendInteraction(L"HLAmanager.HLAfederate.HLAservice.HLAchangeInteractionOrderType",
                    {{L"HLAfederate", {&federate_handle, 4}},
                     {L"HLAinteractionClass", {&interaction_class, 4}},
                     {L"HLAsendOrder", {&order, 4}}});
}

void MomCli::command_federation_adjust_set_switches()
{
    std::wcout << "=>HLAmanager.HLAfederation.HLAadjust.HLAsetSwitches\n";

    bool auto_provide = std::stoi(requestArgument("HLAautoProvide"));

    sendInteraction(L"HLAmanager.HLAfederation.HLAadjust.HLAsetSwitches", {{L"HLAautoProvide", {&auto_provide, 4}}});
}

void MomCli::command_federation_request_synchronization_points()
{
    std::wcout << "=>HLAmanager.HLAfederation.HLArequest.HLArequestSynchronizationPoints\n";

    sendInteraction(L"HLAmanager.HLAfederation.HLArequest.HLArequestSynchronizationPoints", std::map<std::wstring, rti1516e::VariableLengthData>{});
}

void MomCli::command_federation_request_synchronization_point_status()
{
    std::wcout << "=>HLAmanager.HLAfederation.HLArequest.HLArequestSynchronizationPointStatus\n";

    std::string sync_point = requestArgument("HLAsyncPointName");

    sendInteraction(L"HLAmanager.HLAfederation.HLArequest.HLArequestSynchronizationPointStatus",
                    {{L"HLAsyncPointName", {&sync_point, 4}}});
}

void MomCli::command_federation_request_FOM_module_data()
{
    std::wcout << "=>HLAmanager.HLAfederation.HLArequest.HLArequestFOMmoduleData\n";

    int fom_module = std::stoi(requestArgument("HLAFOMmoduleIndicator"));

    sendInteraction(L"HLAmanager.HLAfederation.HLArequest.HLArequestFOMmoduleData",
                    {{L"HLAFOMmoduleIndicator", {&fom_module, 4}}});
}

void MomCli::command_federation_request_MIM_data()
{
    std::wcout << "=>HLAmanager.HLAfederation.HLArequest.HLArequestMIMData\n";

    sendInteraction(L"HLAmanager.HLAfederation.HLArequest.HLArequestMIMData", std::map<std::wstring, rti1516e::VariableLengthData>{});
}

std::vector<int> MomCli::requestVectorArguments(const std::string& name)
{
    std::vector<int> result;

    std::cout << "  Please provide list <" << name << "> (empty means end of list)" << std::flush;

    for (auto element = requestArgument("first element"); !element.empty(); element = requestArgument("next element")) {
        result.push_back(std::stoi(element));
    }

    return result;
}

void MomCli::sendInteraction(const std::wstring& interaction,
                             std::map<std::wstring, rti1516e::VariableLengthData> parameters)
{
    try {
        std::string tag = "";
        auto ich = my_ambassador.getInteractionClassHandle(interaction);

        std::map<rti1516e::ParameterHandle, rti1516e::VariableLengthData> params;

        std::transform(begin(parameters),
                       end(parameters),
                       std::inserter(params, begin(params)),
                       [&](decltype(parameters)::value_type p) {
                           return std::make_pair(my_ambassador.getParameterHandle(ich, p.first), p.second);
                       });

        my_ambassador.sendInteraction(ich, params, {tag.c_str(), tag.size()});
    }
    catch (rti1516e::Exception& e) {
        std::wcerr << "**Caught exception: " << e << std::endl;
    }
}
