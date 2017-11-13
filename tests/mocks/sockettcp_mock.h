#ifndef MOCKSOCKETTCP_H
#define MOCKSOCKETTCP_H

#include <gmock/gmock.h>

#include <libCERTI/SocketTCP.hh>

class MockSocketTcp : public certi::SocketTCP {
public:
    MOCK_METHOD2(send, void(const unsigned char* data, size_t size));

    MOCK_METHOD2(receive, void(void* buffer, unsigned long size));
};

#endif // MOCKSOCKETTCP_H
