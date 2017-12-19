#include <gtest/gtest.h>

#include <include/make_unique.hh>
#include <RTIG/MessageProcessor.hh>

#include "../mocks/sockettcp_mock.h"

#include "temporaryfedfile.h"

using ::certi::rtig::MessageProcessor;
using ::certi::MessageEvent;

using ::certi::NetworkMessage;

using namespace ::testing;

class MessageProcessorTest : public Test {
protected:
    ::certi::AuditFile audit_server{"tmp"};
    ::certi::SocketServer socket_server{new certi::SocketTCP{}, nullptr};
    ::certi::HandleManager<::certi::Handle> handle_generator{1};
    ::certi::rtig::FederationsList federations{};

    MessageProcessor mp{audit_server, socket_server, handle_generator, federations};
};

TEST_F(MessageProcessorTest, ProcessThrowsOnCloseConnection)
{
    auto message = std::unique_ptr<NetworkMessage>(::certi::NM_Factory::create(NetworkMessage::Type::CLOSE_CONNEXION));

    ASSERT_THROW(mp.processEvent({nullptr, std::move(message)}), ::certi::RTIinternalError);
}

TEST_F(MessageProcessorTest, ProcessThrowsOnBasicMessage)
{
    auto message = make_unique<NetworkMessage>();

    ASSERT_THROW(mp.processEvent({nullptr, std::move(message)}), ::certi::RTIinternalError);
}

TEST_F(MessageProcessorTest, Process_NM_Create_Federation_Execution_Empty)
{
    auto message = make_unique<::certi::NM_Create_Federation_Execution>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Create_Federation_Execution)
{
    std::string federation_name{"valid"};
    TemporaryFedFile federation_file{"Sample.fed"};

    MockSocketTcp socket;
    EXPECT_CALL(socket, send(_, _)).Times(0);

    auto message = make_unique<::certi::NM_Create_Federation_Execution>();
    message->setFederationExecutionName(federation_name);
    message->setFomModuleDesignatorsSize(1);
    message->setFomModuleDesignators(federation_file.path(), 0);

    auto first_generator_value = handle_generator.provide();

    auto responses = mp.processEvent({&socket, std::move(message)});

    // test responses
    ASSERT_EQ(1u, responses.size());

    // takes an handle from the handle_generator
    ASSERT_EQ(first_generator_value + 2, handle_generator.provide());
    // created a federation
    ASSERT_EQ(::certi::FederationHandle(first_generator_value + 1), federations.getFederationHandle(federation_name));
}

TEST_F(MessageProcessorTest, Process_NM_Join_Federation_Execution_Empty)
{
    auto message = make_unique<::certi::NM_Join_Federation_Execution>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Resign_Federation_Execution_Empty)
{
    auto message = make_unique<::certi::NM_Resign_Federation_Execution>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Destroy_Federation_Execution_Empty)
{
    auto message = make_unique<::certi::NM_Destroy_Federation_Execution>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Set_Class_Relevance_Advisory_Switch_Empty)
{
    auto message = make_unique<::certi::NM_Set_Class_Relevance_Advisory_Switch>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Set_Interaction_Relevance_Advisory_Switch_Empty)
{
    auto message = make_unique<::certi::NM_Set_Interaction_Relevance_Advisory_Switch>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Set_Attribute_Relevance_Advisory_Switch_Empty)
{
    auto message = make_unique<::certi::NM_Set_Attribute_Relevance_Advisory_Switch>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Set_Attribute_Scope_Advisory_Switch_Empty)
{
    auto message = make_unique<::certi::NM_Set_Attribute_Scope_Advisory_Switch>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Set_Time_Regulating_Empty)
{
    auto message = make_unique<::certi::NM_Set_Time_Regulating>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Set_Time_Constrained_Empty)
{
    auto message = make_unique<::certi::NM_Set_Time_Constrained>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Message_Null_Empty)
{
    auto message = make_unique<::certi::NM_Message_Null>();

    ASSERT_NO_THROW(mp.processEvent({nullptr, std::move(message)});) << "This message has a special exception handling";
}

TEST_F(MessageProcessorTest, Process_NM_Message_Null_Prime_Empty)
{
    auto message = make_unique<::certi::NM_Message_Null_Prime>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Register_Federation_Synchronization_Point_Empty)
{
    auto message = make_unique<::certi::NM_Register_Federation_Synchronization_Point>();

    ASSERT_NO_THROW(mp.processEvent({nullptr, std::move(message)});) << "This message has a special exception handling";
}

TEST_F(MessageProcessorTest, Process_NM_Synchronization_Point_Achieved_Empty)
{
    auto message = make_unique<::certi::NM_Synchronization_Point_Achieved>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Request_Federation_Save_Empty)
{
    auto message = make_unique<::certi::NM_Request_Federation_Save>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Federate_Save_Begun_Empty)
{
    auto message = make_unique<::certi::NM_Federate_Save_Begun>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Federate_Save_Complete_Empty)
{
    auto message = make_unique<::certi::NM_Federate_Save_Complete>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Federate_Save_Not_Complete_Empty)
{
    auto message = make_unique<::certi::NM_Federate_Save_Not_Complete>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Request_Federation_Restore_Empty)
{
    auto message = make_unique<::certi::NM_Request_Federation_Restore>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Federate_Restore_Complete_Empty)
{
    auto message = make_unique<::certi::NM_Federate_Restore_Complete>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Federate_Restore_Not_Complete_Empty)
{
    auto message = make_unique<::certi::NM_Federate_Restore_Not_Complete>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Publish_Object_Class_Empty)
{
    auto message = make_unique<::certi::NM_Publish_Object_Class>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Unpublish_Object_Class_Empty)
{
    auto message = make_unique<::certi::NM_Unpublish_Object_Class>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Subscribe_Object_Class_Empty)
{
    auto message = make_unique<::certi::NM_Subscribe_Object_Class>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Unsubscribe_Object_Class_Empty)
{
    auto message = make_unique<::certi::NM_Unsubscribe_Object_Class>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Publish_Interaction_Class_Empty)
{
    auto message = make_unique<::certi::NM_Publish_Interaction_Class>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Unpublish_Interaction_Class_Empty)
{
    auto message = make_unique<::certi::NM_Unpublish_Interaction_Class>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Subscribe_Interaction_Class_Empty)
{
    auto message = make_unique<::certi::NM_Subscribe_Interaction_Class>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Unsubscribe_Interaction_Class_Empty)
{
    auto message = make_unique<::certi::NM_Unsubscribe_Interaction_Class>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Reserve_Object_Instance_Name_Empty)
{
    auto message = make_unique<::certi::NM_Reserve_Object_Instance_Name>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Register_Object_Empty)
{
    auto message = make_unique<::certi::NM_Register_Object>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Update_Attribute_Values_Empty)
{
    auto message = make_unique<::certi::NM_Update_Attribute_Values>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Send_Interaction_Empty)
{
    auto message = make_unique<::certi::NM_Send_Interaction>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Delete_Object_Empty)
{
    auto message = make_unique<::certi::NM_Delete_Object>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Query_Attribute_Ownership_Empty)
{
    auto message = make_unique<::certi::NM_Query_Attribute_Ownership>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Negotiated_Attribute_Ownership_Divestiture_Empty)
{
    auto message = make_unique<::certi::NM_Negotiated_Attribute_Ownership_Divestiture>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Attribute_Ownership_Acquisition_If_Available_Empty)
{
    auto message = make_unique<::certi::NM_Attribute_Ownership_Acquisition_If_Available>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Unconditional_Attribute_Ownership_Divestiture_Empty)
{
    auto message = make_unique<::certi::NM_Unconditional_Attribute_Ownership_Divestiture>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Attribute_Ownership_Acquisition_Empty)
{
    auto message = make_unique<::certi::NM_Attribute_Ownership_Acquisition>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Cancel_Negotiated_Attribute_Ownership_Divestiture_Empty)
{
    auto message = make_unique<::certi::NM_Cancel_Negotiated_Attribute_Ownership_Divestiture>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Is_Attribute_Owned_By_Federate_Empty)
{
    auto message = make_unique<::certi::NM_Is_Attribute_Owned_By_Federate>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Attribute_Ownership_Release_Response_Empty)
{
    auto message = make_unique<::certi::NM_Attribute_Ownership_Release_Response>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Cancel_Attribute_Ownership_Acquisition_Empty)
{
    auto message = make_unique<::certi::NM_Cancel_Attribute_Ownership_Acquisition>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_DDM_Create_Region_Empty)
{
    auto message = make_unique<::certi::NM_DDM_Create_Region>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_DDM_Modify_Region_Empty)
{
    auto message = make_unique<::certi::NM_DDM_Modify_Region>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_DDM_Delete_Region_Empty)
{
    auto message = make_unique<::certi::NM_DDM_Delete_Region>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_DDM_Associate_Region_Empty)
{
    auto message = make_unique<::certi::NM_DDM_Associate_Region>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_DDM_Unassociate_Region_Empty)
{
    auto message = make_unique<::certi::NM_DDM_Unassociate_Region>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_DDM_Subscribe_Attributes_Empty)
{
    auto message = make_unique<::certi::NM_DDM_Subscribe_Attributes>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_DDM_Unsubscribe_Attributes_Empty)
{
    auto message = make_unique<::certi::NM_DDM_Unsubscribe_Attributes>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_DDM_Subscribe_Interaction_Empty)
{
    auto message = make_unique<::certi::NM_DDM_Subscribe_Interaction>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_DDM_Unsubscribe_Interaction_Empty)
{
    auto message = make_unique<::certi::NM_DDM_Unsubscribe_Interaction>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_DDM_Register_Object_Empty)
{
    auto message = make_unique<::certi::NM_DDM_Register_Object>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Request_Object_Attribute_Value_Update_Empty)
{
    auto message = make_unique<::certi::NM_Request_Object_Attribute_Value_Update>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}

TEST_F(MessageProcessorTest, Process_NM_Request_Class_Attribute_Value_Update_Empty)
{
    auto message = make_unique<::certi::NM_Request_Class_Attribute_Value_Update>();

    ASSERT_ANY_THROW(mp.processEvent({nullptr, std::move(message)}););
}
