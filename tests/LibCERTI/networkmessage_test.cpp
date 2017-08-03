#include <gtest/gtest.h>

#include "libCERTI/NetworkMessage.hh"

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
