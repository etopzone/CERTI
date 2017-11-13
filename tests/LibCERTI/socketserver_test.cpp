#include <gtest/gtest.h>

#include <libCERTI/SocketServer.hh>

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

// BUG Throws a network error, but not in throw list
// TEST(SocketServer, Open)
// {
//     MockSocketTcp socket;
//     
//     SocketServer s(&socket, nullptr);
// }
