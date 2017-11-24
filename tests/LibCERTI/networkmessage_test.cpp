#include <gtest/gtest.h>

#include "libCERTI/NetworkMessage.hh"

#include <include/make_unique.hh>

using ::certi::NetworkMessage;

TEST(NetworkMessageTest, CopyCtorExistsAndWorks)
{
    NetworkMessage msg;
    msg.setFederate(1);
    msg.setFederation(3);
    
    NetworkMessage msg2 = msg;
    
    ASSERT_EQ(msg.getFederate(), msg2.getFederate());
    ASSERT_EQ(msg.getFederation(), msg2.getFederation());
}

TEST(NetworkMessageTest, CopyCtorInUniquePtrExistsAndWorks)
{
    NetworkMessage msg;
    msg.setFederate(1);
    msg.setFederation(3);
    
    auto msg2 = make_unique<NetworkMessage>(msg);
    
    ASSERT_EQ(msg.getFederate(), msg2->getFederate());
    ASSERT_EQ(msg.getFederation(), msg2->getFederation());
}
