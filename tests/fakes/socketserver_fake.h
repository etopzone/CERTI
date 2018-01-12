#pragma once

#include <libCERTI/SocketServer.hh>

class FakeSocketServer : public ::certi::SocketServer {
    using SocketServer::SocketServer;

    virtual ::certi::Socket* getSocketLink(::certi::FederationHandle /*the_federation*/,
                                           ::certi::FederateHandle /*the_federate*/,
                                           ::certi::TransportType /*the_type*/ = ::certi::RELIABLE) const override
    {
        return nullptr;
    }

    virtual void setReferences(long /*the_socket*/,
                               ::certi::FederationHandle /*federation_reference*/,
                               ::certi::FederateHandle /*federate_reference*/,
                               unsigned long /*the_address*/,
                               unsigned int /*the_port*/) override
    {
    }
};
