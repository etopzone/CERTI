#include <gtest/gtest.h>

#include <RTIG/Federation.hh>
#include <libCERTI/RootObject.hh>

// using ::testing::_;

using ::certi::rtig::Federation;

namespace {
static const ::certi::FederationHandle invalid_handle{0};

static const ::certi::FederationHandle federation_handle{1};

static const ::certi::FederateHandle ukn_federate{42};

}

class MomTest : public ::testing::Test {
protected:
    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};

    Federation f{"MOM", federation_handle, s, a, "Test.xml", 0};
};

// Overview, TODO check it is checked by other test cases
// TEST(MomTest, 11_1_1_The_RTI_shall_publish_object_classes)
// {
//     // Test after setup that the mom published
// }
// 
// TEST(MomTest, 11_1_2_The_RTI_shall_register_object_instances_and_update_values_of_attributes)
// {
//     // Test after setup that the mom registered one instance of federation
// }
// 
// TEST(MomTest, 11_1_3_The_RTI_shall_subscribe_to_and_receive_some_interaction_classes)
// {
//     // Test after setup that the mom subscribed
// }
// 
// TEST(MomTest, 11_1_4_The_RTI_shall_publish_and_send_other_interaction_classes)
// {
//     // Test after setup that the mom published
// }
// 
// TEST(MomTest, 11_1_5_All_MOM_items_shall_be_predefined_in_the_FDD)
// {
//     // Test with valid and invalids xml and assert that mom_enabled is good
// }
// 
// TEST(MomTest, 11_1_6_Those_definitions_shall_not_be_revised)
// {
//     // Test that incomplete feds are detected
// }
// 
// TEST(MomTest, 11_1_7_MOM_definitions_may_be_extended_and_augmented)
// {
//     // items can be added without jeoparding mom enabling
// }
// 
// TEST(MomTest, 11_1_8_These_new_elements_shall_not_be_acted_upon_directly_by_the_RTI)
// {
//     // by default the RTI does not care about those new item. how to test ?
// }

TEST(MomTest, 11_2_a_1_The_RTI_shall_publish_and_register_one_instance_of_manager_federate_per_federate)
{
    // ok
}

TEST(MomTest, 11_2_a_2_The_RTI_shall_update_the_information_based_on_federate_adjust)
{
    // ok
}

TEST(MomTest, 11_2_a_3_Information_shall_be_contained_in_an_object_instance)
{
    // ok
}

TEST(MomTest, 11_2_b_The_RTI_shall_publish_and_register_one_instance_of_manager_federation_per_federation)
{
    // ok
}

TEST(MomTest, 11_3_a_The_RTI_shall_act_upon_receiving_adjust_interaction)
{
    
}

TEST(MomTest, 11_3_b_The_RTI_shall_act_upon_receiving_request_interaction)
{
    
}

TEST(MomTest, 11_3_d_The_RTI_shall_impersonate_federate_upon_receiving_service_interaction)
{
    
}

TEST(MomTest, 11_4_1_a_The_RTI_shall_publish_all_object_classes)
{
    // ok
}

TEST(MomTest, 11_4_1_b_The_RTI_shall_publish_all_attributes_of_object_classes)
{
    // ok
}

TEST(MomTest, 11_4_1_c_The_RTI_shall_publish_all_interactions)
{
    // ok
}

TEST(MomTest, 11_4_1_d_The_RTI_shall_send_null_reports_upon_receiving_a_request_interaction_where_the_information_does_not_exist)
{
    
}

TEST(MomTest, 11_4_1_d_1_The_RTI_shall_encode_arrays_as_empty_array_upon_sending_a_null_report)
{
    // ok
}

TEST(MomTest, 11_4_1_e_The_RTI_shall_supply_all_parameters_of_respond_interactions_when_not_null)
{
    // ok
}

TEST(MomTest, 11_4_1_f_The_RTI_shall_be_actively_subscribed_to_all_interactions)
{
    // ok
}

TEST(MomTest, 11_4_1_g_1_The_RTI_shall_be_neither_time_regulating_nor_time_constrained)
{
    // ok
}

TEST(MomTest, 11_4_1_g_2_The_RTI_shall_never_provide_timestamps_nor_user_supplied_tags)
{
    // ok
}

TEST(MomTest, 11_4_1_h_The_RTI_shall_update_values_of_all_attributes_of_federate_using_an_update_region_that_contains_only_the_federate_dimension)
{
    
}

TEST(MomTest, 11_4_1_i_The_RTI_shall_send_all_report_service_invocation_using_an_update_region_that_contains_only_the_federate_and_service_group_dimensions)
{
    
}

TEST(MomTest, 11_4_1_j_The_RTI_shall_send_all_other_report_interactions_using_an_update_region_that_contains_only_the_federate_dimension)
{
    
}

TEST(MomTest, 11_4_1_k_The_RTI_shall_not_engage_in_other_activities_other_than_registering_and_deleting_object_instances_updating_instance_attributes_and_sending_and_receiving_interactions)
{
    // ok
}

TEST(MomTest, 11_4_1_l_The_RTI_shall_always_supply_all_pre_defined_parameters_unless_specified)
{
    // ok
}
