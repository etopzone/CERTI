#include <gtest/gtest.h>

#include <RTIG/Federation.hh>

#include <libCERTI/AuditFile.hh>
#include <libCERTI/SocketServer.hh>

#include <config.h>

#include "temporaryenvironmentlocation.h"
#include "temporaryfedfile.h"

#include "sockettcp_mock.h"

using ::testing::_;

using ::certi::rtig::Federation;

class FederationTest : public ::testing::Test {
protected:
    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};

    TemporaryFedFile tmp{"Sample.fed"};

    Federation f{"name", 1, s, a, "Sample.fed", 0};
};

#ifdef FEDERATION_USES_MULTICAST
TEST_F(FederationTest, CtorMulticastThrowsOnNullMC)
{
    ASSERT_THROW(Federation("multicast", 1, s, a, nullptr, 0), ::certi::RTIinternalError);
}
#endif

TEST_F(FederationTest, CtorThrowsOnNullHandle)
{
    ASSERT_THROW(Federation("name", 0, s, a, "Test.fed", 0), ::certi::RTIinternalError);
}

TEST_F(FederationTest, CtorThrowsOnEmptyName)
{
    ASSERT_THROW(Federation("", 1, s, a, "Test.fed", 0), ::certi::RTIinternalError);
}

TEST_F(FederationTest, CtorFailIfUnableToFindFed)
{
    ASSERT_THROW(Federation("", 1, s, a, "", 0), ::certi::RTIinternalError);
}

TEST_F(FederationTest, CtorFindsFedInSameFolder)
{
    TemporaryFedFile tmp{"SampleLocal.fed"};

    ASSERT_NO_THROW(Federation("local", 1, s, a, "SampleLocal.fed", 0));
}

TEST_F(FederationTest, CtorFindsFedInCertiFomPath)
{
    TemporaryEnvironmentLocation env{"CERTI_FOM_PATH"};

    TemporaryFedFile tmp{env.path() + "SampleFomPath.fed"};

    ASSERT_NO_THROW(Federation("fom_path", 1, s, a, "SampleFomPath.fed", 0));
}

TEST_F(FederationTest, CtorFindsFedInCertiHome)
{
    TemporaryEnvironmentLocation env{"CERTI_HOME"};

    TemporaryFedFile tmp{"SampleCertiHome.fed"};

    ASSERT_NO_THROW(Federation("certi_home", 1, s, a, "SampleCertiHome.fed", 0));
}

TEST_F(FederationTest, CtorFindsFedInPackageInstallPrefix)
{
    TemporaryFedFile tmp{PACKAGE_INSTALL_PREFIX "/share/federations/"
                                                "SampleInstallPrefix.fed"};

    ASSERT_NO_THROW(Federation("install_prefix", 1, s, a, "SampleInstallPrefix.fed", 0));
}

TEST_F(FederationTest, CtorFailsIfFileIsUnopenable)
{
    ASSERT_THROW(Federation("unopenable", 1, s, a, "/root/", 0), ::certi::CouldNotOpenFED);
}

TEST_F(FederationTest, CtorFailsIfNoExtension)
{
    TemporaryFedFile tmp{"SampleWithoutDotfed"};

    ASSERT_THROW(Federation("no_dot_fed", 1, s, a, "SampleWithoutDotfed", 0), ::certi::CouldNotOpenFED);
}

TEST_F(FederationTest, CtorFailsIfWrongExtension)
{
    TemporaryFedFile tmp{"SampleWithout.fde"};

    ASSERT_THROW(Federation("bad_extension", 1, s, a, "SampleWithout.fde", 0), ::certi::CouldNotOpenFED);
}

#ifndef HAVE_XML
TEST_F(FederationTest, CtorFailsIfXmlFedWithoutXmlSupport)
{
    TemporaryFedFile tmp{"SampleWithout.xml"};

    ASSERT_THROW(Federation("bad_extension", 1, s, a, "SampleWithout.xml", 0), ::certi::CouldNotOpenFED);
}
#endif

TEST_F(FederationTest, FederationsStartsWithNoFederate)
{
    ASSERT_TRUE(f.empty());
}

TEST_F(FederationTest, AddFederateMakesEmptyThrow)
{
    f.add("new_federate", nullptr);

    ASSERT_EQ(1, f.getNbFederates());

    ASSERT_THROW(f.empty(), ::certi::FederatesCurrentlyJoined);
}

TEST_F(FederationTest, EmptyThrowListOfFederates)
{
    f.add("fed1", nullptr);

    f.add("fed2", nullptr);

    f.add("fed3", nullptr);

    try {
        f.empty();
        FAIL() << "Empty should have thrown list of federates";
    }
    catch (::certi::FederatesCurrentlyJoined& e) {
        EXPECT_EQ("< fed1 fed2 fed3 >", e.reason());
    }
    catch (...) {
        FAIL() << "Empty should have thrown list of federates";
    }
}

TEST_F(FederationTest, CannotAddSameFederateTwice)
{
    f.add("new_federate", nullptr);

    ASSERT_THROW(f.add("new_federate", nullptr), ::certi::FederateAlreadyExecutionMember);

    ASSERT_EQ(1, f.getNbFederates());
}

/// FIXME This should not send NM. Move to Processing!
TEST_F(FederationTest, AddFederateWithoutRegulatorsReceiveNoMessage)
{
    MockSocketTcp socket;
    EXPECT_CALL(socket, send(_, _)).Times(0);

    f.add("new_federate", &socket);
}

/// FIXME This should not send NM. Move to Processing!
TEST_F(FederationTest, AddFederateReceiveNullMessageFromRegulator)
{
    auto fed = f.add("regul1", nullptr);

    try {
        f.addRegulator(fed, {});
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from broadcastAnyMessage, but the regulator should be registered
    }
    catch (...) {
        FAIL() << "Add regulator may throw from SocketServer::getWithReferences, but not from anywhere else";
    }

    ASSERT_EQ(1, f.getNbRegulators());

    MockSocketTcp socket;
    EXPECT_CALL(socket, send(_, _)).Times(1);

    f.add("new_federate", &socket);

    ASSERT_EQ(2, f.getNbFederates());
}

/// FIXME This should not send NM. Move to Processing!
TEST_F(FederationTest, AddFederateSynchronizingReceiveASPMessage)
{
    f.registerSynchronization(f.add("sync_emitter", nullptr), "label", "tag");

    MockSocketTcp socket;
    EXPECT_CALL(socket, send(_, _)).Times(1);

    f.add("new_federate", &socket);

    ASSERT_EQ(2, f.getNbFederates());
}

TEST_F(FederationTest, AddConstrainedWorks)
{
    auto fed = f.add("new_federate", nullptr);

    ASSERT_NO_THROW(f.addConstrained(fed));

    // FIXME find another way to check if really constrained...
    ASSERT_NO_THROW(f.removeConstrained(fed));
}

TEST_F(FederationTest, AddConstrainedThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.addConstrained(1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, AddConstrainedDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);

    f.addConstrained(fed);

    ASSERT_THROW(f.addConstrained(fed), ::certi::RTIinternalError);
}

// TODO There is a lot of Federate manipulation. Shouldn't we get those details (like cannot set flag if flag already set) down to federate to improve size and readability of federation ?

// addRegulator
