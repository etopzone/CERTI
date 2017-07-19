#include <gtest/gtest.h>

#define TEST_FOR_FEDERATION
#include <RTIG/Federation.hh>

#include <libCERTI/AuditFile.hh>
#include <libCERTI/SocketServer.hh>

#include <config.h>

#include "temporaryenvironmentlocation.h"
#include "temporaryfedfile.h"

#include "../mocks/sockettcp_mock.h"

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
    TemporaryFedFile tmp{PACKAGE_INSTALL_PREFIX
#ifdef WIN32
                         "\\share\\federations\\"
#else
                         "/share/federations/"
#endif
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

TEST_F(FederationTest, VerboseLevelChangesOutput)
{
    std::ostringstream oss;
    std::streambuf* original_cout_rdbuf = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());
    
    Federation f{"name", 1, s, a, "Sample.fed", 0};
    
    std::ostringstream oss2;
    std::cout.rdbuf(oss2.rdbuf());
    
    Federation f2{"name", 1, s, a, "Sample.fed", 1};
    
    std::cout.rdbuf(original_cout_rdbuf); // restore
    
    // test your oss content...
    ASSERT_TRUE(oss);
    ASSERT_TRUE(oss2);
    ASSERT_TRUE(oss.str().size() < oss2.str().size());
}

TEST_F(FederationTest, GetHandle)
{
    ASSERT_EQ(1, f.getHandle());
}

TEST_F(FederationTest, GetName)
{
    ASSERT_EQ("name", f.getName());
}

TEST_F(FederationTest, GetFedId)
{
    ASSERT_EQ("Sample.fed", f.getFEDid());
}

TEST_F(FederationTest, FederationsStartsEmpty)
{
    ASSERT_TRUE(f.empty());
}

TEST_F(FederationTest, FederationsStartsWithNoFederate)
{
    ASSERT_EQ(0, f.getNbFederates());
}

TEST_F(FederationTest, FederationsStartsWithNoRegulators)
{
    ASSERT_EQ(0, f.getNbRegulators());
}

TEST_F(FederationTest, FederationsDoesNotStartSynchronizing)
{
    ASSERT_FALSE(f.isSynchronizing());
}

TEST_F(FederationTest, AddFederateCreateUnderlying)
{
    auto fed = f.add("fed", nullptr);
    
    ASSERT_EQ("fed", f.getFederate(fed).getName());
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

TEST_F(FederationTest, RemoveFederateUpdatesUnderlying)
{
    auto fed = f.add("fed", nullptr);
    
    f.remove(fed);
    
    ASSERT_TRUE(f.empty());
}

TEST_F(FederationTest, RemoveFederateThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.remove(1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, CannotRemoveSameFederateTwice)
{
    auto fed = f.add("new_federate", nullptr);
    
    f.remove(fed);
    
    ASSERT_THROW(f.remove(fed), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, KillRemoveFederate)
{
    auto fed = f.add("new", nullptr);
    
    f.kill(fed);
    
    ASSERT_TRUE(f.empty());
}

TEST_F(FederationTest, KillRemoveFederateFromRegulators)
{
    auto fed = f.add("new", nullptr);
    
    try {
        f.addRegulator(fed, {});
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from broadcastAnyMessage, but the regulator should be registered
    }
    catch (...) {
        FAIL() << "Add regulator may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
    
    f.kill(fed);
    
    ASSERT_EQ(0, f.getNbRegulators());
}

TEST_F(FederationTest, EmptyThrowsIfFederatesExists)
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

TEST_F(FederationTest, CheckReturnsTrueIfFederateExist)
{
    ASSERT_TRUE(f.check(f.add("fed", nullptr)));
}

TEST_F(FederationTest, CheckThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.check(1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, SetUnsetCRASAlterUnderlyingFederate)
{
    auto handle = f.add("fed", nullptr);
    
    auto& fed = f.getFederate(handle);
    
    ASSERT_TRUE(fed.isClassRelevanceAdvisorySwitch()) << "Federate starts with CRAS == true";
    
    f.unsetClassRelevanceAdvisorySwitch(handle);
    
    ASSERT_FALSE(fed.isClassRelevanceAdvisorySwitch());
    
    f.setClassRelevanceAdvisorySwitch(handle);
    
    ASSERT_TRUE(fed.isClassRelevanceAdvisorySwitch());
}

TEST_F(FederationTest, SetCRASThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.setClassRelevanceAdvisorySwitch(1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, SetCRASDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);

    // Starts already on

    ASSERT_THROW(f.setClassRelevanceAdvisorySwitch(fed), ::certi::RTIinternalError);
}

TEST_F(FederationTest, UnsetCRASThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.unsetClassRelevanceAdvisorySwitch(1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, UnsetCRASDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);

    f.unsetClassRelevanceAdvisorySwitch(fed);

    ASSERT_THROW(f.unsetClassRelevanceAdvisorySwitch(fed), ::certi::RTIinternalError);
}

TEST_F(FederationTest, SetUnsetIRASAlterUnderlyingFederate)
{
    auto handle = f.add("fed", nullptr);
    
    auto& fed = f.getFederate(handle);
    
    ASSERT_TRUE(fed.isInteractionRelevanceAdvisorySwitch()) << "Federate starts with IRAS == true";
    
    f.unsetInteractionRelevanceAdvisorySwitch(handle);
    
    ASSERT_FALSE(fed.isInteractionRelevanceAdvisorySwitch());
    
    f.setInteractionRelevanceAdvisorySwitch(handle);
    
    ASSERT_TRUE(fed.isInteractionRelevanceAdvisorySwitch());
}

TEST_F(FederationTest, SetIRASThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.setInteractionRelevanceAdvisorySwitch(1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, SetIRASDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);

    // Starts already on

    ASSERT_THROW(f.setInteractionRelevanceAdvisorySwitch(fed), ::certi::RTIinternalError);
}

TEST_F(FederationTest, UnsetIRASThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.unsetInteractionRelevanceAdvisorySwitch(1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, UnsetIRASDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);

    f.unsetInteractionRelevanceAdvisorySwitch(fed);

    ASSERT_THROW(f.unsetInteractionRelevanceAdvisorySwitch(fed), ::certi::RTIinternalError);
}

TEST_F(FederationTest, SetUnsetARASAlterUnderlyingFederate)
{
    auto handle = f.add("fed", nullptr);
    
    auto& fed = f.getFederate(handle);
    
    ASSERT_FALSE(fed.isAttributeRelevanceAdvisorySwitch()) << "Federate starts with ARAS == false";
    
    f.setAttributeRelevanceAdvisorySwitch(handle);
    
    ASSERT_TRUE(fed.isAttributeRelevanceAdvisorySwitch());
    
    f.unsetAttributeRelevanceAdvisorySwitch(handle);
    
    ASSERT_FALSE(fed.isAttributeRelevanceAdvisorySwitch());
}

TEST_F(FederationTest, SetARASThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.setAttributeRelevanceAdvisorySwitch(1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, SetARASDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);

    f.setAttributeRelevanceAdvisorySwitch(fed);

    ASSERT_THROW(f.setAttributeRelevanceAdvisorySwitch(fed), ::certi::RTIinternalError);
}

TEST_F(FederationTest, UnsetARASThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.unsetAttributeRelevanceAdvisorySwitch(1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, UnsetARASDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);

    // Starts already off

    ASSERT_THROW(f.unsetAttributeRelevanceAdvisorySwitch(fed), ::certi::RTIinternalError);
}

TEST_F(FederationTest, SetUnsetASASAlterUnderlyingFederate)
{
    auto handle = f.add("fed", nullptr);
    
    auto& fed = f.getFederate(handle);
    
    ASSERT_FALSE(fed.isAttributeScopeAdvisorySwitch()) << "Federate starts with ASAS == false";
    
    f.setAttributeScopeAdvisorySwitch(handle);
    
    ASSERT_TRUE(fed.isAttributeScopeAdvisorySwitch());
    
    f.unsetAttributeScopeAdvisorySwitch(handle);
    
    ASSERT_FALSE(fed.isAttributeScopeAdvisorySwitch());
}

TEST_F(FederationTest, SetASASThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.setAttributeScopeAdvisorySwitch(1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, SetASASDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);

    f.setAttributeScopeAdvisorySwitch(fed);

    ASSERT_THROW(f.setAttributeScopeAdvisorySwitch(fed), ::certi::RTIinternalError);
}

TEST_F(FederationTest, UnsetASASThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.unsetAttributeScopeAdvisorySwitch(1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, UnsetASASDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);

    // Starts already off

    ASSERT_THROW(f.unsetAttributeScopeAdvisorySwitch(fed), ::certi::RTIinternalError);
}

TEST_F(FederationTest, AddRemoveConstrainedAlterUnderlyingFederate)
{
    auto handle = f.add("new_federate", nullptr);
    
    auto& fed = f.getFederate(handle);
    
    ASSERT_FALSE(fed.isConstrained()) << "Federate starts with constrained == false";
    
    f.addConstrained(handle);
    
    ASSERT_TRUE(fed.isConstrained());
    
    f.removeConstrained(handle);
    
    ASSERT_FALSE(fed.isConstrained());
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

TEST_F(FederationTest, RemoveConstrainedThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.removeConstrained(1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, RemoveConstrainedDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);
    
    // Already not constrained
    
    ASSERT_THROW(f.removeConstrained(fed), ::certi::RTIinternalError);
}

TEST_F(FederationTest, AddRemoveRegulatorAlterUnderlyingFederate)
{
    auto handle = f.add("new_federate", nullptr);
    
    auto& fed = f.getFederate(handle);
    
    ASSERT_FALSE(fed.isRegulator()) << "Federate starts with constrained == false";
    
    try {
        f.addRegulator(handle, {});
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from broadcastAnyMessage, but the regulator should be registered
    }
    catch (...) {
        FAIL() << "Add regulator may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
    
    ASSERT_EQ(1, f.getNbRegulators());
    
    ASSERT_TRUE(fed.isRegulator());
    
    try {
        f.removeRegulator(handle);
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from broadcastAnyMessage, but the regulator should be registered
    }
    catch (...) {
        FAIL() << "Add regulator may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
    
    ASSERT_FALSE(fed.isRegulator());
}

TEST_F(FederationTest, AddRegulatorThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.addRegulator(1, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, AddRegulatorDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);
    
    try {
        f.addRegulator(fed, {});
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from broadcastAnyMessage, but the regulator should be registered
    }
    catch (...) {
        FAIL() << "Add regulator may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
    
    ASSERT_THROW(f.addRegulator(fed, {}), ::certi::RTIinternalError);
}

TEST_F(FederationTest, RemoveRegulatorThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.removeRegulator(1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, RemoveRegulatorDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);
    
    // Already not regulator
    
    ASSERT_THROW(f.removeRegulator(fed), ::certi::RTIinternalError);
}

TEST_F(FederationTest, UpdateRegulatorNeedsValidFederateIfNotAnonymous)
{
    ASSERT_THROW(f.updateRegulator(1, {}, false), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, UpdateRegulatorNeedsValidRegulatorIfNotAnonymous)
{
    auto handle = f.add("regulator", nullptr);
    
    ASSERT_THROW(f.updateRegulator(handle, {}, false), ::certi::RTIinternalError);
}

// TODO UpdateRegulator not tested

TEST_F(FederationTest, RegisterSynchronizationAddsLabelToAllFederates)
{
    auto handle = f.add("fed", nullptr);
    
    auto& fed = f.getFederate(handle);
    
    auto& fed1 = f.getFederate(f.add("fed 1", nullptr));
    auto& fed2 = f.getFederate(f.add("fed 2", nullptr));
    auto& fed3 = f.getFederate(f.add("fed 3", nullptr));
    
    f.registerSynchronization(handle, "label", "tag");
    
    ASSERT_TRUE(fed.isSynchronizationLabel("label"));
    
    ASSERT_TRUE(fed1.isSynchronizationLabel("label"));
    ASSERT_TRUE(fed2.isSynchronizationLabel("label"));
    ASSERT_TRUE(fed3.isSynchronizationLabel("label"));
}

TEST_F(FederationTest, RegisterSynchronizationThrowsIfUknFederate)
{
    ASSERT_THROW(f.registerSynchronization(1, "label", "tag"), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, RegisterSynchronizationThrowsIfLabelEmpty)
{
    auto handle = f.add("fed", nullptr);
    
    ASSERT_THROW(f.registerSynchronization(handle, "", "tag"), ::certi::RTIinternalError);
}

TEST_F(FederationTest, RegisterSynchronizationThrowsIfLabelAlreadyExists)
{
    auto handle = f.add("fed", nullptr);
    
    f.registerSynchronization(handle, "label", "tag");
    
    ASSERT_THROW(f.registerSynchronization(handle, "label", "tag"), ::certi::FederationAlreadyPaused);
}

// FIXME Possible bug here !!
TEST_F(FederationTest, RegisterSynchronizationPerSetIfNoSetEmitterIsNotSynchronized)
{
    auto handle = f.add("emitter", nullptr);
    
    auto& fed = f.getFederate(handle);
    
    ::std::vector<::certi::FederateHandle> federatesToUpdate;
    
    f.registerSynchronization(handle, "label", "tag", federatesToUpdate.size(), federatesToUpdate);
    
    ASSERT_FALSE(fed.isSynchronizationLabel("label")) << "Emitter is not synchronizing if set was empty";
}

TEST_F(FederationTest, RegisterSynchronizationPerSetAddsLabelToSpecifiedFederates)
{
    auto handle = f.add("emitter", nullptr);
    
    auto& fed = f.getFederate(handle);
    
    auto h1 = f.add("fed 1", nullptr);
    auto h2 = f.add("fed 2", nullptr);
    auto h3 = f.add("fed 3", nullptr);
    
    ::std::vector<::certi::FederateHandle> federatesToUpdate;
    federatesToUpdate.push_back(h2);
    //     federatesToUpdate.push_back(h3);
    
    f.registerSynchronization(handle, "label", "tag", federatesToUpdate.size(), federatesToUpdate);
    
    ASSERT_TRUE(fed.isSynchronizationLabel("label")) << "The sync label must also be set to the emitter";
    
    ASSERT_FALSE(f.getFederate(h1).isSynchronizationLabel("label"));
    ASSERT_TRUE(f.getFederate(h2).isSynchronizationLabel("label"));
    ASSERT_FALSE(f.getFederate(h3).isSynchronizationLabel("label"));
}

TEST_F(FederationTest, RegisterSynchronizationPerSetThrowsIfUknFederate)
{
    ASSERT_THROW(f.registerSynchronization(1, "label", "tag", 0, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, RegisterSynchronizationPerSetThrowsIfLabelEmpty)
{
    auto handle = f.add("fed", nullptr);
    
    ASSERT_THROW(f.registerSynchronization(handle, "", "tag", 0, {}), ::certi::RTIinternalError);
}

TEST_F(FederationTest, RegisterSynchronizationPerSetThrowsIfLabelAlreadyExists)
{
    auto handle = f.add("fed", nullptr);
    
    f.registerSynchronization(handle, "label", "tag");
    
    ASSERT_THROW(f.registerSynchronization(handle, "label", "tag", 0, {}), ::certi::FederationAlreadyPaused);
}

// BUG: std::terminate is called, thanks to bad throw list...
TEST_F(FederationTest, BroadcastSynchronizationThrowOnUknFederate)
{
    ASSERT_THROW(f.broadcastSynchronization(1, "label", "tag"), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, BroadcastSynchronizationThrowOnEmptyLabel)
{
    auto handle = f.add("fed", nullptr);
    
    ASSERT_THROW(f.broadcastSynchronization(handle, "", "tag"), ::certi::RTIinternalError);
}

TEST_F(FederationTest, broadcastSynchronizationSendsNM)
{
    auto handle = f.add("fed", nullptr);
    
    try {
        f.broadcastSynchronization(handle, "label", "tag");
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from broadcastSynchronization, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason() << " : broadcastSynchronization may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
}

TEST_F(FederationTest, BroadcastSynchronizationPerSetThrowOnUknFederate)
{
    ASSERT_THROW(f.broadcastSynchronization(1, "label", "tag", 0, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, BroadcastSynchronizationPerSetThrowOnEmptyLabel)
{
    auto handle = f.add("fed", nullptr);
    
    ASSERT_THROW(f.broadcastSynchronization(handle, "", "tag", 0, {}), ::certi::RTIinternalError);
}

// TODO There is a lot of Federate manipulation. Shouldn't we get those details (like cannot set flag if flag already set) down to federate to improve size and readability of federation ?
