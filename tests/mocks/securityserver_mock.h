#ifndef MOCK_CERTI_SECURITY_SERVER_HH
#define MOCK_CERTI_SECURITY_SERVER_HH

#include <gmock/gmock.h>

#include <libCERTI/SecurityServer.hh>

class MockSecurityServer : public certi::SecurityServer {
public:
    using certi::SecurityServer::SecurityServer;
    
    MOCK_CONST_METHOD2(getSocketLink, certi::Socket*(certi::FederateHandle theFederate, certi::TransportType theType));
};

#endif // MOCK_CERTI_SECURITY_SERVER_HH
