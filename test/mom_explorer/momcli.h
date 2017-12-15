#ifndef MOMCLI_H
#define MOMCLI_H

#include "cli.h"

#include <RTI/RTIambassador.h>

class MomCli : public cli {
public:
    MomCli(rti1516e::RTIambassador& amb, const std::wstring& federation_name);

private:
    void command_tick();
    void command_resign_federation_execution();
    void command_destroy_federation_execution(const std::wstring& federation_name);
    void command_enable_time_regulation();
    void command_enable_time_constrained();
    void command_disable_time_regulation();
    void command_disable_time_constrained();

    //////////////////////
    // MOM INTERACTIONS //
    //////////////////////

    void command_federate_adjust_set_timing();
    void command_federate_adjust_modify_attribute_state();
    void command_federate_adjust_set_switches();
    void command_federate_request_publications();
    void command_federate_request_subscriptions();
    void command_federate_request_object_instances_that_can_be_deleted();
    void command_federate_request_object_instances_updated();
    void command_federate_request_object_instances_reflected();
    void command_federate_request_updates_sent();
    void command_federate_request_interactions_sent();
    void command_federate_request_reflections_received();
    void command_federate_request_interactions_received();
    void command_federate_request_object_instance_information();
    void command_federate_request_FOM_module_data();
    void command_federate_service_resign_federation_execution();
    void command_federate_service_synchronization_point_achieved();
    void command_federate_service_federate_save_begun();
    void command_federate_service_federate_save_complete();
    void command_federate_service_federate_restore_complete();
    void command_federate_service_publish_object_class_attributes();
    void command_federate_service_un_publish_object_class_attributes();
    void command_federate_service_publish_interaction_class();
    void command_federate_service_un_publish_interaction_class();
    void command_federate_service_subscribe_object_class_attributes();
    void command_federate_service_un_subscribe_object_class_attributes();
    void command_federate_service_subscribe_interaction_class();
    void command_federate_service_un_subscribe_interaction_class();
    void command_federate_service_delete_object_instance();
    void command_federate_service_local_delete_object_instance();
    void command_federate_service_request_attribute_transportation_type_change();
    void command_federate_service_request_interaction_transportation_type_change();
    void command_federate_service_unconditional_attribute_ownership_divestiture();
    void command_federate_service_enable_time_regulation();
    void command_federate_service_disable_time_regulation();
    void command_federate_service_enable_time_constrained();
    void command_federate_service_disable_time_constrained();
    void command_federate_service_time_advance_request();
    void command_federate_service_time_advance_request_available();
    void command_federate_service_next_message_request();
    void command_federate_service_next_message_request_available();
    void command_federate_service_flush_queue_request();
    void command_federate_service_enable_asynchronous_delivery();
    void command_federate_service_disable_asynchronous_delivery();
    void command_federate_service_modify_lookahead();
    void command_federate_service_change_attribute_order_type();
    void command_federate_service_change_interaction_order_type();
    void command_federation_adjust_set_switches();
    void command_federation_request_synchronization_points();
    void command_federation_request_synchronization_point_status();
    void command_federation_request_FOM_module_data();
    void command_federation_request_MIM_data();
    
    std::vector<int> requestVectorArguments(const std::string& name);
    
    void sendInteraction(const std::wstring& interaction,
                         std::map<std::wstring, rti1516e::VariableLengthData> parameters);

    rti1516e::RTIambassador& my_ambassador;
};

#endif // MOMCLI_H
