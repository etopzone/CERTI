#include <gtest/gtest.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>

#include <RTIG/Federation.hh>

#include <libCERTI/SocketServer.hh>

#include <libCERTI/AuditFile.hh>

#include <config.h>

#include "temporaryenvironmentlocation.h"
#include "temporaryfedfile.h"

#include "sockettcp_mock.h"

using ::testing::_;

using ::certi::rtig::Federation;
using ::certi::SocketServer;
using ::certi::AuditFile;

using ::certi::RTIinternalError;
using ::certi::CouldNotOpenFED;
using ::certi::FederateAlreadyExecutionMember;

TEST(FederationTest, ConstructorGoodInit)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    TemporaryFedFile tmp{"Sample.fed"};

    Federation("name", 1, s, a, "Sample.fed", 0);
}

#ifdef FEDERATION_USES_MULTICAST
TEST(FederationTest, MulticastConstructorThrowsOnNullMC)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    ASSERT_THROW(Federation("multicast", 1, s, a, nullptr, 0), RTIinternalError);
}
#endif

TEST(FederationTest, ConstructorThrowsOnNullHandle)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    ASSERT_THROW(Federation("name", 0, s, a, "Test.fed", 0), RTIinternalError);
}

TEST(FederationTest, ConstructorThrowsOnEmptyName)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    ASSERT_THROW(Federation("", 1, s, a, "Test.fed", 0), RTIinternalError);
}

TEST(FederationTest, ConstructorFailIfUnableToFindFed)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    ASSERT_THROW(Federation("", 1, s, a, "", 0), RTIinternalError);
}

TEST(FederationTest, ConstructorFindsFedInSameFolder)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    TemporaryFedFile tmp{"SampleLocal.fed"};

    ASSERT_NO_THROW(Federation("local", 1, s, a, "SampleLocal.fed", 0));
}

TEST(FederationTest, ConstructorFindsFedInCertiFomPath)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    TemporaryEnvironmentLocation env{"CERTI_FOM_PATH"};

    TemporaryFedFile tmp{env.path() + "SampleFomPath.fed"};

    ASSERT_NO_THROW(Federation("fom_path", 1, s, a, "SampleFomPath.fed", 0));
}

TEST(FederationTest, ConstructorFindsFedInCertiHome)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    TemporaryEnvironmentLocation env{"CERTI_HOME"};

    TemporaryFedFile tmp{"SampleCertiHome.fed"};

    ASSERT_NO_THROW(Federation("certi_home", 1, s, a, "SampleCertiHome.fed", 0));
}

TEST(FederationTest, ConstructorFindsFedInPackageInstallPrefix)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    TemporaryFedFile tmp{PACKAGE_INSTALL_PREFIX "/share/federations/"
                                                "SampleInstallPrefix.fed"};

    ASSERT_NO_THROW(Federation("install_prefix", 1, s, a, "SampleInstallPrefix.fed", 0));
}

TEST(FederationTest, ConstructorFailsIfFileIsUnopenable)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    ASSERT_THROW(Federation("unopenable", 1, s, a, "/root/", 0), CouldNotOpenFED);
}

TEST(FederationTest, ConstructorFailsIfNoExtension)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    TemporaryFedFile tmp{"SampleWithoutDotfed"};

    ASSERT_THROW(Federation("no_dot_fed", 1, s, a, "SampleWithoutDotfed", 0), CouldNotOpenFED);
}

TEST(FederationTest, ConstructorFailsIfWrongExtension)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    TemporaryFedFile tmp{"SampleWithout.fde"};

    ASSERT_THROW(Federation("bad_extension", 1, s, a, "SampleWithout.fde", 0), CouldNotOpenFED);
}

#ifndef HAVE_XML
TEST(FederationTest, ConstructorFailsIfXmlFedWithoutXmlSupport)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    TemporaryFedFile tmp{"SampleWithout.xml"};

    ASSERT_THROW(Federation("bad_extension", 1, s, a, "SampleWithout.xml", 0), CouldNotOpenFED);
}
#endif

TEST(FederationTest, AddFederateWorks)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    TemporaryFedFile tmp{"Sample.fed"};

    Federation f("name", 1, s, a, "Sample.fed", 0);

    f.add("new_federate", nullptr);

    ASSERT_EQ(1, f.getNbFederates());
}

TEST(FederationTest, CannotAddSameFederateTwice)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    TemporaryFedFile tmp{"Sample.fed"};

    Federation f("name", 1, s, a, "Sample.fed", 0);

    f.add("new_federate", nullptr);

    ASSERT_THROW(f.add("new_federate", nullptr), FederateAlreadyExecutionMember);

    ASSERT_EQ(1, f.getNbFederates());
}

/// FIXME This should not send NM. Move to Processing!
TEST(FederationTest, AddFederateWithoutRegulatorsReceiveNoMessage)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    MockSocketTcp socket;
    EXPECT_CALL(socket, send(_, _)).Times(0);

    TemporaryFedFile tmp{"Sample.fed"};

    Federation f("name", 1, s, a, "Sample.fed", 0);

    f.add("new_federate", &socket);
}

/// FIXME This should not send NM. Move to Processing!
TEST(FederationTest, AddFederateReceiveNullMessageFromRegulator)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    TemporaryFedFile tmp{"Sample.fed"};

    Federation f("name", 1, s, a, "Sample.fed", 0);

    auto fed = f.add("regul1", nullptr);

    try {
        f.addRegulator(fed, {});
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from broadcastAnyMessage, but the regulator should be registered
    }
    catch(...) {
        FAIL() << "Add regulator may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
    
    ASSERT_EQ(1, f.getNbRegulators());

    MockSocketTcp socket;
    EXPECT_CALL(socket, send(_, _)).Times(1);

    f.add("new_federate", &socket);

    ASSERT_EQ(2, f.getNbFederates());
}

/// FIXME This should not send NM. Move to Processing!
TEST(FederationTest, AddFederateSynchronizingReceiveASPMessage)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};
    
    TemporaryFedFile tmp{"Sample.fed"};
    
    Federation f("name", 1, s, a, "Sample.fed", 0);
    
    f.registerSynchronization(f.add("sync_emitter", nullptr), "label", "tag");
    
    MockSocketTcp socket;
    EXPECT_CALL(socket, send(_, _)).Times(1);
    
    f.add("new_federate", &socket);
    
    ASSERT_EQ(2, f.getNbFederates());
}

TEST(FederationTest, AddConstrainedWorks)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};
    
    TemporaryFedFile tmp{"Sample.fed"};
    
    Federation f("name", 1, s, a, "Sample.fed", 0);
    
    auto fed = f.add("new_federate", nullptr);
    
    ASSERT_NO_THROW(f.addConstrained(fed));
    
    // FIXME find another way to check if really constrained...
    ASSERT_NO_THROW(f.removeConstrained(fed));
}

TEST(FederationTest, AddConstrainedThrowsOnUnknownFederate)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};
    
    TemporaryFedFile tmp{"Sample.fed"};
    
    Federation f("name", 1, s, a, "Sample.fed", 0);
    
    ASSERT_THROW(f.addConstrained(1), certi::FederateNotExecutionMember);
}

TEST(FederationTest, AddConstrainedDoesNotWorkTwice)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};
    
    TemporaryFedFile tmp{"Sample.fed"};
    
    Federation f("name", 1, s, a, "Sample.fed", 0);
    
    auto fed = f.add("new_federate", nullptr);
    
    f.addConstrained(fed);
    
    ASSERT_THROW(f.addConstrained(fed), RTIinternalError);
}

// TODO There is a lot of Federate manipulation. Shouldn't we get those details (like cannot set flag if flag already set) down to federate to improve size and readability of federation ?

// addRegulator
