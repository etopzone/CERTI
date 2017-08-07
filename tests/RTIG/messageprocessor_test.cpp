#include <gtest/gtest.h>

#include <RTIG/MessageProcessor.hh>

#include "../mocks/sockettcp_mock.h"

#include "temporaryfedfile.h"

using ::certi::rtig::MessageProcessor;
using ::certi::rtig::MessageEvent;

using ::certi::NetworkMessage;

using namespace ::testing;

class MessageProcessorTest : public Test {
protected:
    ::certi::AuditFile audit_server{"tmp"};
    ::certi::SocketServer socket_server{new certi::SocketTCP{}, nullptr};
    ::certi::HandleManager<::certi::Handle> handle_generator{1};
    ::certi::rtig::FederationsList federations{socket_server, audit_server};

    MessageProcessor mp{audit_server, socket_server, handle_generator, federations};
};

TEST_F(MessageProcessorTest, ProcessThrowsOnCloseConnection)
{
    auto message = std::unique_ptr<NetworkMessage>(::certi::NM_Factory::create(NetworkMessage::CLOSE_CONNEXION));

    ASSERT_THROW(mp.processEvent({nullptr, std::move(message)}), ::certi::RTIinternalError);
}

TEST_F(MessageProcessorTest, ProcessThrowsOnDefault)
{
    auto message = std::make_unique<NetworkMessage>();

    ASSERT_THROW(mp.processEvent({nullptr, std::move(message)}), ::certi::RTIinternalError);
}

TEST_F(MessageProcessorTest, Process_NM_Create_Federation_Execution_ThrowsOnEmptyFederationName)
{
    MockSocketTcp socket;
    EXPECT_CALL(socket, send(_, _)).Times(0);

    auto message = std::make_unique<::certi::NM_Create_Federation_Execution>();

    message->setFederationName("");

    ASSERT_THROW(mp.processEvent({&socket, std::move(message)}), ::certi::RTIinternalError);
}

TEST_F(MessageProcessorTest, Process_NM_Create_Federation_Execution)
{
    auto federation_name{"valid"};
    TemporaryFedFile federation_file{"Sample.fed"};

    MockSocketTcp socket;
    EXPECT_CALL(socket, send(_, _)).Times(1);

    auto message = std::make_unique<::certi::NM_Create_Federation_Execution>();
    message->setFederationName(federation_name);
    message->setFEDid(federation_file.path());

    auto first_generator_value = handle_generator.provide();

    try {
        auto responses = mp.processEvent({&socket, std::move(message)});

        // test responses

        // takes an handle from the handle_generator
        ASSERT_EQ(first_generator_value + 2, handle_generator.provide());
        // created a federation
        ASSERT_EQ(first_generator_value + 1, federations.getFederationHandle(federation_name));
    }
    catch (certi::Exception& e) {
        std::cerr << e.name() << " 3- " << e.reason() << std::endl;
    }
}

// TEST_F(MessageProcessorTest, Process_NM_Join_Federation_Execution)
// {
//     auto message = std::make_unique<::certi::NM_Join_Federation_Execution>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Resign_Federation_Execution)
// {
//     auto message = std::make_unique<::certi::NM_Resign_Federation_Execution>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Destroy_Federation_Execution)
// {
//     auto message = std::make_unique<::certi::NM_Destroy_Federation_Execution>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Set_Class_Relevance_Advisory_Switch)
// {
//     auto message = std::make_unique<::certi::NM_Set_Class_Relevance_Advisory_Switch>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Set_Interaction_Relevance_Advisory_Switch)
// {
//     auto message = std::make_unique<::certi::NM_Set_Interaction_Relevance_Advisory_Switch>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Set_Attribute_Relevance_Advisory_Switch)
// {
//     auto message = std::make_unique<::certi::NM_Set_Attribute_Relevance_Advisory_Switch>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Set_Attribute_Scope_Advisory_Switch)
// {
//     auto message = std::make_unique<::certi::NM_Set_Attribute_Scope_Advisory_Switch>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Set_Time_Regulating)
// {
//     auto message = std::make_unique<::certi::NM_Set_Time_Regulating>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Set_Time_Constrained)
// {
//     auto message = std::make_unique<::certi::NM_Set_Time_Constrained>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Message_Null)
// {
//     auto message = std::make_unique<::certi::NM_Message_Null>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Message_Null_Prime)
// {
//     auto message = std::make_unique<::certi::NM_Message_Null_Prime>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Register_Federation_Synchronization_Point)
// {
//     auto message = std::make_unique<::certi::NM_Register_Federation_Synchronization_Point>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Synchronization_Point_Achieved)
// {
//     auto message = std::make_unique<::certi::NM_Synchronization_Point_Achieved>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Request_Federation_Save)
// {
//     auto message = std::make_unique<::certi::NM_Request_Federation_Save>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Federate_Save_Begun)
// {
//     auto message = std::make_unique<::certi::NM_Federate_Save_Begun>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Federate_Save_Complete)
// {
//     auto message = std::make_unique<::certi::NM_Federate_Save_Complete>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Federate_Save_Not_Complete)
// {
//     auto message = std::make_unique<::certi::NM_Federate_Save_Not_Complete>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Request_Federation_Restore)
// {
//     auto message = std::make_unique<::certi::NM_Request_Federation_Restore>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Federate_Restore_Complete)
// {
//     auto message = std::make_unique<::certi::NM_Federate_Restore_Complete>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Federate_Restore_Not_Complete)
// {
//     auto message = std::make_unique<::certi::NM_Federate_Restore_Not_Complete>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Publish_Object_Class)
// {
//     auto message = std::make_unique<::certi::NM_Publish_Object_Class>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Unpublish_Object_Class)
// {
//     auto message = std::make_unique<::certi::NM_Unpublish_Object_Class>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Subscribe_Object_Class)
// {
//     auto message = std::make_unique<::certi::NM_Subscribe_Object_Class>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Unsubscribe_Object_Class)
// {
//     auto message = std::make_unique<::certi::NM_Unsubscribe_Object_Class>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Publish_Interaction_Class)
// {
//     auto message = std::make_unique<::certi::NM_Publish_Interaction_Class>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Unpublish_Interaction_Class)
// {
//     auto message = std::make_unique<::certi::NM_Unpublish_Interaction_Class>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Subscribe_Interaction_Class)
// {
//     auto message = std::make_unique<::certi::NM_Subscribe_Interaction_Class>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Unsubscribe_Interaction_Class)
// {
//     auto message = std::make_unique<::certi::NM_Unsubscribe_Interaction_Class>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Reserve_Object_Instance_Name)
// {
//     auto message = std::make_unique<::certi::NM_Reserve_Object_Instance_Name>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Register_Object)
// {
//     auto message = std::make_unique<::certi::NM_Register_Object>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Update_Attribute_Values)
// {
//     auto message = std::make_unique<::certi::NM_Update_Attribute_Values>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Send_Interaction)
// {
//     auto message = std::make_unique<::certi::NM_Send_Interaction>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Delete_Object)
// {
//     auto message = std::make_unique<::certi::NM_Delete_Object>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Query_Attribute_Ownership)
// {
//     auto message = std::make_unique<::certi::NM_Query_Attribute_Ownership>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Negotiated_Attribute_Ownership_Divestiture)
// {
//     auto message = std::make_unique<::certi::NM_Negotiated_Attribute_Ownership_Divestiture>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Attribute_Ownership_Acquisition_If_Available)
// {
//     auto message = std::make_unique<::certi::NM_Attribute_Ownership_Acquisition_If_Available>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Unconditional_Attribute_Ownership_Divestiture)
// {
//     auto message = std::make_unique<::certi::NM_Unconditional_Attribute_Ownership_Divestiture>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Attribute_Ownership_Acquisition)
// {
//     auto message = std::make_unique<::certi::NM_Attribute_Ownership_Acquisition>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Cancel_Negotiated_Attribute_Ownership_Divestiture)
// {
//     auto message = std::make_unique<::certi::NM_Cancel_Negotiated_Attribute_Ownership_Divestiture>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }

// TEST_F(MessageProcessorTest, Process_NM_Is_Attribute_Owned_By_Federate)
// {
//     auto message = std::make_unique<::certi::NM_Is_Attribute_Owned_By_Federate>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Attribute_Ownership_Release_Response)
// {
//     auto message = std::make_unique<::certi::NM_Attribute_Ownership_Release_Response>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Cancel_Attribute_Ownership_Acquisition)
// {
//     auto message = std::make_unique<::certi::NM_Cancel_Attribute_Ownership_Acquisition>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_DDM_Create_Region)
// {
//     auto message = std::make_unique<::certi::NM_DDM_Create_Region>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_DDM_Modify_Region)
// {
//     auto message = std::make_unique<::certi::NM_DDM_Modify_Region>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_DDM_Delete_Region)
// {
//     auto message = std::make_unique<::certi::NM_DDM_Delete_Region>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_DDM_Associate_Region)
// {
//     auto message = std::make_unique<::certi::NM_DDM_Associate_Region>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_DDM_Unassociate_Region)
// {
//     auto message = std::make_unique<::certi::NM_DDM_Unassociate_Region>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_DDM_Subscribe_Attributes)
// {
//     auto message = std::make_unique<::certi::NM_DDM_Subscribe_Attributes>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_DDM_Unsubscribe_Attributes)
// {
//     auto message = std::make_unique<::certi::NM_DDM_Unsubscribe_Attributes>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_DDM_Subscribe_Interaction)
// {
//     auto message = std::make_unique<::certi::NM_DDM_Subscribe_Interaction>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_DDM_Unsubscribe_Interaction)
// {
//     auto message = std::make_unique<::certi::NM_DDM_Unsubscribe_Interaction>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_DDM_Register_Object)
// {
//     auto message = std::make_unique<::certi::NM_DDM_Register_Object>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Request_Object_Attribute_Value_Update)
// {
//     auto message = std::make_unique<::certi::NM_Request_Object_Attribute_Value_Update>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
//
// TEST_F(MessageProcessorTest, Process_NM_Request_Class_Attribute_Value_Update)
// {
//     auto message = std::make_unique<::certi::NM_Request_Class_Attribute_Value_Update>();
//
//     auto responses = mp.processEvent({nullptr, std::move(message)});
//
//     // test responses
// }
