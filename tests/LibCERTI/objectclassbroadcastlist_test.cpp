#include <gtest/gtest.h>

#include "libCERTI/ObjectClassBroadcastList.hh"

using ::certi::ObjectBroadcastLine;
using ::certi::ObjectClassBroadcastList;
using ::certi::NetworkMessage;

namespace {
static constexpr ::certi::Handle invalid_handle{0};
static constexpr ::certi::Handle ukn_handle{42};

static constexpr ::certi::Handle max_handle{10};

static constexpr ::certi::Handle attr_handle{3};

static constexpr ::certi::FederateHandle sender_handle{1};
static constexpr ::certi::FederateHandle federate_handle{2};

ObjectBroadcastLine* getLineForFederate(ObjectClassBroadcastList& list, ::certi::FederateHandle fed)
{
    auto it = std::find_if(begin(list.___TESTS_ONLY___lines()),
                           end(list.___TESTS_ONLY___lines()),
                           [&fed](ObjectBroadcastLine* line) { return line->Federate == fed; });

    if (it == end(list.___TESTS_ONLY___lines())) {
        return nullptr;
    }

    return *it;
}
}

TEST(ObjectBroadcastLineTest, CtorCopiesFedHandle)
{
    ObjectBroadcastLine line{42};

    ASSERT_EQ(42u, line.Federate);
}

TEST(ObjectBroadcastLineTest, CtorInitializeStateSent)
{
    ObjectBroadcastLine line{42, ObjectBroadcastLine::sent};

    for (int i{0}; i < MAX_STATE_SIZE + 1; ++i) {
        ASSERT_EQ(ObjectBroadcastLine::sent, line.state[i]) << ", no" << i;
    }
}

TEST(ObjectBroadcastLineTest, CtorInitializeStateWaiting)
{
    ObjectBroadcastLine line{42, ObjectBroadcastLine::waiting};

    for (int i{0}; i < MAX_STATE_SIZE + 1; ++i) {
        ASSERT_EQ(ObjectBroadcastLine::waiting, line.state[i]) << ", no" << i;
    }
}

TEST(ObjectBroadcastLineTest, CtorInitializeStateNotSub)
{
    ObjectBroadcastLine line{42, ObjectBroadcastLine::notSub};

    for (int i{0}; i < MAX_STATE_SIZE + 1; ++i) {
        ASSERT_EQ(ObjectBroadcastLine::notSub, line.state[i]) << ", no" << i;
    }
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

    ASSERT_EQ(0, l.___TESTS_ONLY___lines().size());
}

TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
{
    auto message = new ::certi::NM_Attribute_Ownership_Divestiture_Notification;
    message->setFederate(sender_handle);
    ObjectClassBroadcastList l(message, 0);

    ASSERT_EQ(1, l.___TESTS_ONLY___lines().size());
    ASSERT_EQ(sender_handle, l.___TESTS_ONLY___lines().front()->Federate);
    ASSERT_EQ(ObjectBroadcastLine::sent, l.___TESTS_ONLY___lines().front()->state[0]);
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

    ASSERT_EQ(2, l.___TESTS_ONLY___lines().size());

    auto line = getLineForFederate(l, federate_handle);

    ASSERT_NE(nullptr, line);

    for (int i{0}; i < max_handle; ++i) {
        if (i == attr_handle) {
            EXPECT_EQ(ObjectBroadcastLine::waiting, line->state[i]);
        }
        else {
            EXPECT_EQ(ObjectBroadcastLine::notSub, line->state[i]);
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
    
    ASSERT_EQ(2, l.___TESTS_ONLY___lines().size());
    
    auto line = getLineForFederate(l, federate_handle);
    
    ASSERT_NE(nullptr, line);
    
    EXPECT_EQ(ObjectBroadcastLine::waiting, line->state[attr_handle]);
    EXPECT_EQ(ObjectBroadcastLine::waiting, line->state[attr_handle+1]);
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

// SPDOM
// mock socketserver
// no waiting == no socket claimed
// claim one socket per sub, no matter the attr

// SPRAVM
// mock socketserver
// no waiting == no socket claimed
// 

/*
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
TEST(ObjectClassBroadcastListTest, CtorKeepsCreateLineForMessageFederate)
*/
