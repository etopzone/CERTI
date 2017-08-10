#include <gtest/gtest.h>

#include <libCERTI/ObjectClassBroadcastList.hh>

#include "../mocks/securityserver_mock.h"
#include "../mocks/sockettcp_mock.h"

using ::certi::ObjectBroadcastLine;
using ::certi::ObjectClassBroadcastList;
using ::certi::NetworkMessage;

using ::testing::_;
using ::testing::Return;
using ::testing::Throw;

namespace {
static constexpr ::certi::Handle ukn_handle{42};

static constexpr ::certi::Handle max_handle{10};

static constexpr ::certi::Handle attr_handle{3};

static constexpr ::certi::FederateHandle sender_handle{1};
static constexpr ::certi::FederateHandle federate_handle{2};
static constexpr ::certi::FederateHandle federate2_handle{4};
static constexpr ::certi::FederateHandle federate3_handle{6};

ObjectBroadcastLine* getLineForFederate(ObjectClassBroadcastList& list, ::certi::FederateHandle fed)
{
    auto it = std::find_if(begin(list.___TESTS_ONLY___lines()),
                           end(list.___TESTS_ONLY___lines()),
                           [&fed](ObjectBroadcastLine* line) { return line->getFederate() == fed; });

    if (it == end(list.___TESTS_ONLY___lines())) {
        return nullptr;
    }

    return *it;
}
}

TEST(ObjectBroadcastLineTest, CtorCopiesFedHandle)
{
    ObjectBroadcastLine line{42};

    ASSERT_EQ(42u, line.getFederate());
}

TEST(ObjectClassBroadcastListTest, CtorThrowsOnNullMessage)
{
    ASSERT_THROW(ObjectClassBroadcastList(nullptr, 0), ::certi::RTIinternalError);
}

TEST(ObjectClassBroadcastListTest, CtorThrowsOnUnexpectedMessageType)
{
    auto message = new NetworkMessage;
    ASSERT_THROW(ObjectClassBroadcastList(message, 0), ::certi::RTIinternalError);
}

TEST(ObjectClassBroadcastListTest, CtorAcceptsNM_Remove_Object)
{
    auto message = new ::certi::NM_Remove_Object;
    ObjectClassBroadcastList l(message, 0);

    ASSERT_EQ(message, l.getMsg());
    ASSERT_EQ(message, l.getMsgRO());
    ASSERT_EQ(nullptr, l.getMsgDO());
    ASSERT_EQ(nullptr, l.getMsgRAV());
    ASSERT_EQ(nullptr, l.getMsgRAOA());
    ASSERT_EQ(nullptr, l.getMsgAODN());
}

TEST(ObjectClassBroadcastListTest, CtorAcceptsNM_Discover_Object)
{
    auto message = new ::certi::NM_Discover_Object;
    ObjectClassBroadcastList l(message, 0);

    ASSERT_EQ(message, l.getMsg());
    ASSERT_EQ(nullptr, l.getMsgRO());
    ASSERT_EQ(message, l.getMsgDO());
    ASSERT_EQ(nullptr, l.getMsgRAV());
    ASSERT_EQ(nullptr, l.getMsgRAOA());
    ASSERT_EQ(nullptr, l.getMsgAODN());
}

TEST(ObjectClassBroadcastListTest, CtorAcceptsNM_Reflect_Attribute_Values)
{
    auto message = new ::certi::NM_Reflect_Attribute_Values;
    ObjectClassBroadcastList l(message, 0);

    ASSERT_EQ(message, l.getMsg());
    ASSERT_EQ(nullptr, l.getMsgRO());
    ASSERT_EQ(nullptr, l.getMsgDO());
    ASSERT_EQ(message, l.getMsgRAV());
    ASSERT_EQ(nullptr, l.getMsgRAOA());
    ASSERT_EQ(nullptr, l.getMsgAODN());
}

TEST(ObjectClassBroadcastListTest, CtorAcceptsNM_Request_Attribute_Ownership_Assumption)
{
    auto message = new ::certi::NM_Request_Attribute_Ownership_Assumption;
    ObjectClassBroadcastList l(message, 0);

    ASSERT_EQ(message, l.getMsg());
    ASSERT_EQ(nullptr, l.getMsgRO());
    ASSERT_EQ(nullptr, l.getMsgDO());
    ASSERT_EQ(nullptr, l.getMsgRAV());
    ASSERT_EQ(message, l.getMsgRAOA());
    ASSERT_EQ(nullptr, l.getMsgAODN());
}

TEST(ObjectClassBroadcastListTest, CtorAcceptsNM_Attribute_Ownership_Divestiture_Notification)
{
    auto message = new ::certi::NM_Attribute_Ownership_Divestiture_Notification;
    ObjectClassBroadcastList l(message, 0);

    ASSERT_EQ(message, l.getMsg());
    ASSERT_EQ(nullptr, l.getMsgRO());
    ASSERT_EQ(nullptr, l.getMsgDO());
    ASSERT_EQ(nullptr, l.getMsgRAV());
    ASSERT_EQ(nullptr, l.getMsgRAOA());
    ASSERT_EQ(message, l.getMsgAODN());
}

TEST(ObjectClassBroadcastListTest, CtorKeepsLinesEmptyIfNoMessageFederate)
{
    auto message = new ::certi::NM_Attribute_Ownership_Divestiture_Notification;
    ObjectClassBroadcastList l(message, 0);

    ASSERT_EQ(0u, l.___TESTS_ONLY___lines().size());
}

TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
{
    auto message = new ::certi::NM_Attribute_Ownership_Divestiture_Notification;
    message->setFederate(sender_handle);
    ObjectClassBroadcastList l(message, 0);

    ASSERT_EQ(1u, l.___TESTS_ONLY___lines().size());
    ASSERT_EQ(sender_handle, l.___TESTS_ONLY___lines().front()->getFederate());
    ASSERT_EQ(ObjectBroadcastLine::sent, l.___TESTS_ONLY___lines().front()->stateFor(0));
}

TEST(ObjectClassBroadcastListTest, AddFederateThrowsOnTooHighAttribute)
{
    auto message = new ::certi::NM_Attribute_Ownership_Divestiture_Notification;
    message->setFederate(sender_handle);
    ObjectClassBroadcastList l(message, 0);

    ASSERT_THROW(l.addFederate(federate_handle, 1), ::certi::RTIinternalError);
}

TEST(ObjectClassBroadcastListTest, AddFederateWithOtherFederateCreatesWaitingLine)
{
    auto message = new ::certi::NM_Attribute_Ownership_Divestiture_Notification;
    message->setFederate(sender_handle);
    ObjectClassBroadcastList l(message, max_handle);

    l.addFederate(federate_handle, attr_handle);

    ASSERT_EQ(2u, l.___TESTS_ONLY___lines().size());

    auto line = getLineForFederate(l, federate_handle);

    ASSERT_NE(nullptr, line);

    for (auto i(0u); i < max_handle; ++i) {
        if (i == attr_handle) {
            EXPECT_EQ(ObjectBroadcastLine::waiting, line->stateFor(i));
        }
        else {
            EXPECT_EQ(ObjectBroadcastLine::notSub, line->stateFor(i));
        }
    }
}

TEST(ObjectClassBroadcastListTest, AddFederateWithSameFederateReuseLine)
{
    auto message = new ::certi::NM_Attribute_Ownership_Divestiture_Notification;
    message->setFederate(sender_handle);
    ObjectClassBroadcastList l(message, max_handle);

    l.addFederate(federate_handle, attr_handle);

    l.addFederate(federate_handle, attr_handle + 1);

    ASSERT_EQ(2u, l.___TESTS_ONLY___lines().size());

    auto line = getLineForFederate(l, federate_handle);

    ASSERT_NE(nullptr, line);

    EXPECT_EQ(ObjectBroadcastLine::waiting, line->stateFor(attr_handle));
    EXPECT_EQ(ObjectBroadcastLine::waiting, line->stateFor(attr_handle + 1));
}

// FIXME possible BUG ?
TEST(ObjectClassBroadcastListTest, SendWithAttributeNMThrows)
{
    auto message = new ::certi::NM_Attribute_Ownership_Divestiture_Notification;
    message->setFederate(sender_handle);
    ObjectClassBroadcastList l(message, max_handle);

    ASSERT_THROW(l.sendPendingMessage(nullptr), ::certi::RTIinternalError);
}

TEST(ObjectClassBroadcastListTest, UpcastToAlterNM_Remove_Object)
{
    auto message = new ::certi::NM_Remove_Object;
    message->setFederate(sender_handle);
    message->setObjectClass(ukn_handle);
    ObjectClassBroadcastList l(message, max_handle);

    l.upcastTo(attr_handle);

    ASSERT_EQ(attr_handle, message->getObjectClass());
}

TEST(ObjectClassBroadcastListTest, UpcastToAlterNM_Discover_Object)
{
    auto message = new ::certi::NM_Discover_Object;
    message->setFederate(sender_handle);
    message->setObjectClass(ukn_handle);
    ObjectClassBroadcastList l(message, max_handle);

    l.upcastTo(attr_handle);

    ASSERT_EQ(attr_handle, message->getObjectClass());
}

/* SPDOM
 * TODO check message content
 */
TEST(ObjectClassBroadcastListTest, SendPendingDOMessageNoWaitingNothingSent)
{
    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};
    MockSecurityServer ss(s, a, ::certi::FederationHandle(3));
    EXPECT_CALL(ss, getSocketLink(_, _)).Times(0);

    auto message = new ::certi::NM_Discover_Object;
    message->setFederate(sender_handle);
    ObjectClassBroadcastList l(message, max_handle);

    l.sendPendingMessage(&ss);
}

TEST(ObjectClassBroadcastListTest, SendPendingDOMessageOneSentPerFederateWaitingOn0)
{
    // Federate 1 and 3 will wait
    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};
    MockSecurityServer ss(s, a, ::certi::FederationHandle(3));
    EXPECT_CALL(ss, getSocketLink(federate_handle, _)).Times(1).WillOnce(::testing::ReturnNull());
    EXPECT_CALL(ss, getSocketLink(federate2_handle, _)).Times(0);
    EXPECT_CALL(ss, getSocketLink(federate3_handle, _)).Times(1).WillOnce(::testing::ReturnNull());

    auto message = new ::certi::NM_Discover_Object;
    message->setFederate(sender_handle);
    ObjectClassBroadcastList l(message, max_handle);

    l.addFederate(federate_handle, 0);
    l.addFederate(federate3_handle, 0);

    l.sendPendingMessage(&ss);
}

TEST(ObjectClassBroadcastListTest, SendPendingDOMessageUpdatesState)
{
    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};
    MockSecurityServer ss(s, a, ::certi::FederationHandle(3));
    EXPECT_CALL(ss, getSocketLink(federate_handle, _)).WillOnce(::testing::ReturnNull());

    auto message = new ::certi::NM_Discover_Object;
    message->setFederate(sender_handle);
    ObjectClassBroadcastList l(message, max_handle);

    l.addFederate(federate_handle, 0);

    l.sendPendingMessage(&ss);

    auto line = getLineForFederate(l, federate_handle);

    ASSERT_NE(nullptr, line);
    ASSERT_EQ(ObjectBroadcastLine::sent, line->stateFor(0));
}

TEST(ObjectClassBroadcastListTest, SendPendingDOMessageCatchesRTIinternalErrors)
{
    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};
    MockSecurityServer ss(s, a, ::certi::FederationHandle(3));
    EXPECT_CALL(ss, getSocketLink(federate_handle, _)).WillOnce(Throw(::certi::RTIinternalError("")));

    auto message = new ::certi::NM_Discover_Object;
    message->setFederate(sender_handle);
    ObjectClassBroadcastList l(message, max_handle);

    l.addFederate(federate_handle, 0);

    ASSERT_NO_THROW(l.sendPendingMessage(&ss));
}

TEST(ObjectClassBroadcastListTest, SendPendingDOMessageCatchesNetworkErrors)
{
    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};
    MockSecurityServer ss(s, a, ::certi::FederationHandle(3));
    EXPECT_CALL(ss, getSocketLink(federate_handle, _)).WillOnce(Throw(::certi::NetworkError("")));

    auto message = new ::certi::NM_Discover_Object;
    message->setFederate(sender_handle);
    ObjectClassBroadcastList l(message, max_handle);

    l.addFederate(federate_handle, 0);

    ASSERT_NO_THROW(l.sendPendingMessage(&ss));
}

TEST(ObjectClassBroadcastListTest, SendPendingDOMessageSendsBaseMessage)
{
    auto message = new ::certi::NM_Discover_Object;
    message->setFederate(sender_handle);

    MessageBuffer mb;
    message->serialize(mb);

    MockSocketTcp socket{};
    EXPECT_CALL(socket, send(_, mb.size())).Times(1);

    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};
    MockSecurityServer ss(s, a, ::certi::FederationHandle(3));
    EXPECT_CALL(ss, getSocketLink(federate_handle, _)).WillOnce(Return(&socket));

    ObjectClassBroadcastList l(message, max_handle);

    l.addFederate(federate_handle, 0);

    l.sendPendingMessage(&ss);
}

/* SPRAV
 * TODO check message content
 */
TEST(ObjectClassBroadcastListTest, SendPendingRAVMessageNoWaitingNothingSent)
{
    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};
    MockSecurityServer ss(s, a, ::certi::FederationHandle(3));
    EXPECT_CALL(ss, getSocketLink(_, _)).Times(0);

    auto message = new ::certi::NM_Reflect_Attribute_Values;
    message->setFederate(sender_handle);
    message->setAttributesSize(max_handle);
    ObjectClassBroadcastList l(message, max_handle);

    l.sendPendingMessage(&ss);
}

TEST(ObjectClassBroadcastListTest, SendPendingRAVMessageOneSentPerFederateWaiting)
{
    // Federate 1 and 3 will wait
    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};
    MockSecurityServer ss(s, a, ::certi::FederationHandle(3));
    EXPECT_CALL(ss, getSocketLink(federate_handle, _)).Times(1).WillOnce(::testing::ReturnNull());
    EXPECT_CALL(ss, getSocketLink(federate2_handle, _)).Times(0);
    EXPECT_CALL(ss, getSocketLink(federate3_handle, _)).Times(1).WillOnce(::testing::ReturnNull());

    auto message = new ::certi::NM_Reflect_Attribute_Values;
    message->setFederate(sender_handle);
    message->setAttributesSize(max_handle);
    ObjectClassBroadcastList l(message, max_handle);

    l.addFederate(federate_handle, attr_handle);
    l.addFederate(federate3_handle, attr_handle);

    l.sendPendingMessage(&ss);
}

TEST(ObjectClassBroadcastListTest, SendPendingRAVMessageUpdatesState)
{
    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};
    MockSecurityServer ss(s, a, ::certi::FederationHandle(3));
    EXPECT_CALL(ss, getSocketLink(federate_handle, _)).WillOnce(::testing::ReturnNull());

    auto message = new ::certi::NM_Reflect_Attribute_Values;
    message->setFederate(sender_handle);
    message->setAttributesSize(max_handle);
    ObjectClassBroadcastList l(message, max_handle);

    l.addFederate(federate_handle, attr_handle);

    l.sendPendingMessage(&ss);

    auto line = getLineForFederate(l, federate_handle);

    ASSERT_NE(nullptr, line);
    ASSERT_EQ(ObjectBroadcastLine::sent, line->stateFor(attr_handle));
}

TEST(ObjectClassBroadcastListTest, SendPendingRAVMessageAllWaitingSendsBaseMessage)
{
    auto message = new ::certi::NM_Reflect_Attribute_Values;
    message->setFederate(sender_handle);
    message->setAttributesSize(max_handle);

    MessageBuffer mb;
    message->serialize(mb);

    MockSocketTcp socket{};
    EXPECT_CALL(socket, send(_, mb.size())).Times(1);

    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};
    MockSecurityServer ss(s, a, ::certi::FederationHandle(3));
    EXPECT_CALL(ss, getSocketLink(federate_handle, _)).WillOnce(Return(&socket));

    ObjectClassBroadcastList l(message, max_handle);

    for (auto i(0u); i <= max_handle; ++i) {
        l.addFederate(federate_handle, i);
    }

    l.sendPendingMessage(&ss);
}

TEST(ObjectClassBroadcastListTest, SendPendingRAVMessageNotAllWaitingSendsSmallerMessage)
{
    auto message = new ::certi::NM_Reflect_Attribute_Values;
    message->setFederate(sender_handle);
    message->setAttributesSize(max_handle);

    MessageBuffer mb;
    message->serialize(mb);

    MockSocketTcp socket{};
    EXPECT_CALL(socket, send(_, ::testing::Lt(mb.size()))).Times(1);

    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};
    MockSecurityServer ss(s, a, ::certi::FederationHandle(3));
    EXPECT_CALL(ss, getSocketLink(federate_handle, _)).WillOnce(Return(&socket));

    ObjectClassBroadcastList l(message, max_handle);

    l.addFederate(federate_handle, attr_handle);

    l.sendPendingMessage(&ss);
}

TEST(ObjectClassBroadcastListTest, SendPendingRAOAMessageNotAllWaitingSendsSmallerMessage)
{
    auto message = new ::certi::NM_Request_Attribute_Ownership_Assumption;
    message->setFederate(sender_handle);
    message->setAttributesSize(max_handle);

    MessageBuffer mb;
    message->serialize(mb);

    MockSocketTcp socket{};
    EXPECT_CALL(socket, send(_, ::testing::Lt(mb.size()))).Times(1);

    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};
    MockSecurityServer ss(s, a, ::certi::FederationHandle(3));
    EXPECT_CALL(ss, getSocketLink(federate_handle, _)).WillOnce(Return(&socket));

    ObjectClassBroadcastList l(message, max_handle);

    l.addFederate(federate_handle, attr_handle);

    l.sendPendingMessage(&ss);
}

TEST(ObjectClassBroadcastListTest, SendPendingRAOAMessageCatchesRTIinternalErrors)
{
    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};
    MockSecurityServer ss(s, a, ::certi::FederationHandle(3));
    EXPECT_CALL(ss, getSocketLink(federate_handle, _)).WillOnce(Throw(::certi::RTIinternalError("")));

    auto message = new ::certi::NM_Request_Attribute_Ownership_Assumption;
    message->setFederate(sender_handle);
    message->setAttributesSize(max_handle);
    ObjectClassBroadcastList l(message, max_handle);

    l.addFederate(federate_handle, attr_handle);

    ASSERT_NO_THROW(l.sendPendingMessage(&ss));
}

TEST(ObjectClassBroadcastListTest, SendPendingRAOAMessageCatchesNetworkErrors)
{
    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};
    MockSecurityServer ss(s, a, ::certi::FederationHandle(3));
    EXPECT_CALL(ss, getSocketLink(federate_handle, _)).WillOnce(Throw(::certi::NetworkError("")));

    auto message = new ::certi::NM_Request_Attribute_Ownership_Assumption;
    message->setFederate(sender_handle);
    message->setAttributesSize(max_handle);
    ObjectClassBroadcastList l(message, max_handle);

    l.addFederate(federate_handle, attr_handle);

    ASSERT_NO_THROW(l.sendPendingMessage(&ss));
}
