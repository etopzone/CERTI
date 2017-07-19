#include <gtest/gtest.h>

#include <SocketServer.hh>

#include "../mocks/sockettcp_mock.h"

using ::certi::SocketServer;

TEST(SocketServer, CtorThrowsIfNullTcpSocket)
{
    ASSERT_THROW(SocketServer(nullptr, nullptr), ::certi::RTIinternalError);
}

TEST(SocketServer, CtorAcceptsNullUdpSocket)
{
    MockSocketTcp socket;
    
    SocketServer(&socket, nullptr);
}

TEST(SocketServer, )
{
}
