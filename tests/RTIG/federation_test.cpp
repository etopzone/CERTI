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

// TODO There is a lot of Federate manipulation. Shouldn't we get those details (like cannot set flag if flag already set) down to federate to improve size and readability of federation ?

/* TODO
 * fw_root
 * 
 *    getFOM
 *    void broadcastInteraction()
 *    void broadcastInteraction()
 *    void deleteObject()
 *    void deleteObject()
 *    void publishInteraction()
 *    void publishObject()
 *    void reserveObjectInstanceName()
 *    ObjectHandle registerObject()
 *    void subscribeInteraction()
 *    void subscribeObject()
 *    void updateAttributeValues()
 *    void updateAttributeValues()
 *    bool isOwner()
 *    void queryAttributeOwnership()
 *    void negotiateDivestiture()
 *    void acquireIfAvailable()
 *    void divest()
 *    void acquire()
 *    void cancelDivestiture()
 *    AttributeHandleSet* respondRelease()
 *    void cancelAcquisition()
 *    long createRegion()
 *    void modifyRegion()
 *    void deleteRegion()
 *    void associateRegion()
 *    void unassociateRegion()
 *    void subscribeAttributesWR()
 *    void unsubscribeAttributesWR()
 *    void subscribeInteractionWR()
 *    void unsubscribeInteractionWR()
 * 
 *    ObjectHandle registerObjectWithRegion()
 *    void requestClassAttributeValueUpdate()
 * 
 * network
 * 
 *    void broadcastAnyMessage();
 *    void broadcastSomeMessage();
 *    void subscribeObject()
 * 
 * todo
 *    uptadelastnerxforfederate
 * 
 *    FederationTime computeMinNERx();
 *    void federateSaveStatus()
 *    void requestFederationRestore()
 *    void federateRestoreStatus()
 *    void updateRegulator()
 * 
 *    void deleteRegion()
 *    ObjectHandle registerObjectWithRegion()
 * 
 *    FederateHandle requestObjectOwner()
 */

namespace {
    static constexpr ::certi::Handle invalid_handle {0};
    
    static constexpr ::certi::FederationHandle federation_handle {1};
    
    static constexpr ::certi::FederateHandle ukn_federate {42};
    static constexpr ::certi::Handle ukn_handle {1337};
    
    static constexpr int quiet{0};
    static constexpr int verbose{1};
    
}

class FederationTest : public ::testing::Test {

protected:
    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};

    TemporaryFedFile tmp{"Sample.fed"};

    Federation f{"name", federation_handle, s, a, "Sample.fed", quiet};
};

#ifdef FEDERATION_USES_MULTICAST
TEST_F(FederationTest, CtorMulticastThrowsOnNullMC)
{
    ASSERT_THROW(Federation("multicast", federation_handle, s, a, nullptr, 0), ::certi::RTIinternalError);
}
#endif

TEST_F(FederationTest, CtorThrowsOnNullHandle)
{
    ASSERT_THROW(Federation("name", invalid_handle, s, a, "Test.fed", quiet), ::certi::RTIinternalError);
}

TEST_F(FederationTest, CtorThrowsOnEmptyName)
{
    ASSERT_THROW(Federation("", federation_handle, s, a, "Test.fed", quiet), ::certi::RTIinternalError);
}

TEST_F(FederationTest, CtorFailIfUnableToFindFed)
{
    ASSERT_THROW(Federation("", federation_handle, s, a, "", quiet), ::certi::RTIinternalError);
}

TEST_F(FederationTest, CtorFindsFedInSameFolder)
{
    TemporaryFedFile tmp{"SampleLocal.fed"};

    ASSERT_NO_THROW(Federation("local", federation_handle, s, a, "SampleLocal.fed", quiet));
}

TEST_F(FederationTest, CtorFindsFedInCertiFomPath)
{
    TemporaryEnvironmentLocation env{"CERTI_FOM_PATH"};

    TemporaryFedFile tmp{env.path() + "SampleFomPath.fed"};

    ASSERT_NO_THROW(Federation("fom_path", federation_handle, s, a, "SampleFomPath.fed", quiet));
}

TEST_F(FederationTest, CtorFindsFedInCertiHome)
{
    TemporaryEnvironmentLocation env{"CERTI_HOME"};

    TemporaryFedFile tmp{"SampleCertiHome.fed"};

    ASSERT_NO_THROW(Federation("certi_home", federation_handle, s, a, "SampleCertiHome.fed", quiet));
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

                         ASSERT_NO_THROW(Federation("install_prefix", federation_handle, s, a, "SampleInstallPrefix.fed", quiet));
}

TEST_F(FederationTest, CtorFailsIfFileIsUnopenable)
{
    ASSERT_THROW(Federation("unopenable", federation_handle, s, a, "/root/", quiet), ::certi::CouldNotOpenFED);
}

TEST_F(FederationTest, CtorFailsIfNoExtension)
{
    TemporaryFedFile tmp{"SampleWithoutDotfed"};

    ASSERT_THROW(Federation("no_dot_fed", federation_handle, s, a, "SampleWithoutDotfed", quiet), ::certi::CouldNotOpenFED);
}

TEST_F(FederationTest, CtorFailsIfWrongExtension)
{
    TemporaryFedFile tmp{"SampleWithout.fde"};

    ASSERT_THROW(Federation("bad_extension", federation_handle, s, a, "SampleWithout.fde", quiet), ::certi::CouldNotOpenFED);
}

#ifndef HAVE_XML
TEST_F(FederationTest, CtorFailsIfXmlFedWithoutXmlSupport)
{
    TemporaryFedFile tmp{"SampleWithout.xml"};

    ASSERT_THROW(Federation("bad_extension", federation_handle, s, a, "SampleWithout.xml", quiet), ::certi::CouldNotOpenFED);
}
#endif

TEST_F(FederationTest, VerboseLevelChangesOutput)
{
    std::ostringstream oss;
    std::streambuf* original_cout_rdbuf = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());
    
    Federation f{"name", federation_handle, s, a, "Sample.fed", quiet};
    
    std::ostringstream oss2;
    std::cout.rdbuf(oss2.rdbuf());
    
    Federation f2{"name", federation_handle, s, a, "Sample.fed", verbose};
    
    std::cout.rdbuf(original_cout_rdbuf); // restore
    
    // test your oss content...
    ASSERT_TRUE(oss);
    ASSERT_TRUE(oss2);
    ASSERT_TRUE(oss.str().size() < oss2.str().size());
}

TEST_F(FederationTest, GetHandle)
{
    ASSERT_EQ(federation_handle, f.getHandle());
}

TEST_F(FederationTest, GetName)
{
    ASSERT_EQ("name", f.getName());
}

TEST_F(FederationTest, GetFedId)
{
    ASSERT_EQ("Sample.fed", f.getFEDid());
}

TEST_F(FederationTest, GetMinNERx)
{
    ASSERT_EQ(::certi::FederationTime{}, f.getMinNERx());
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
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason() << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
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
    ASSERT_THROW(f.remove(ukn_federate), ::certi::FederateNotExecutionMember);
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
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason() << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
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
    ASSERT_THROW(f.check(ukn_federate), ::certi::FederateNotExecutionMember);
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
    ASSERT_THROW(f.setClassRelevanceAdvisorySwitch(ukn_federate), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, SetCRASDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);

    // Starts already on

    ASSERT_THROW(f.setClassRelevanceAdvisorySwitch(fed), ::certi::RTIinternalError);
}

TEST_F(FederationTest, UnsetCRASThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.unsetClassRelevanceAdvisorySwitch(ukn_federate), ::certi::FederateNotExecutionMember);
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
    ASSERT_THROW(f.setInteractionRelevanceAdvisorySwitch(ukn_federate), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, SetIRASDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);

    // Starts already on

    ASSERT_THROW(f.setInteractionRelevanceAdvisorySwitch(fed), ::certi::RTIinternalError);
}

TEST_F(FederationTest, UnsetIRASThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.unsetInteractionRelevanceAdvisorySwitch(ukn_federate), ::certi::FederateNotExecutionMember);
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
    ASSERT_THROW(f.setAttributeRelevanceAdvisorySwitch(ukn_federate), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, SetARASDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);

    f.setAttributeRelevanceAdvisorySwitch(fed);

    ASSERT_THROW(f.setAttributeRelevanceAdvisorySwitch(fed), ::certi::RTIinternalError);
}

TEST_F(FederationTest, UnsetARASThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.unsetAttributeRelevanceAdvisorySwitch(ukn_federate), ::certi::FederateNotExecutionMember);
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
    ASSERT_THROW(f.setAttributeScopeAdvisorySwitch(ukn_federate), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, SetASASDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);

    f.setAttributeScopeAdvisorySwitch(fed);

    ASSERT_THROW(f.setAttributeScopeAdvisorySwitch(fed), ::certi::RTIinternalError);
}

TEST_F(FederationTest, UnsetASASThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.unsetAttributeScopeAdvisorySwitch(ukn_federate), ::certi::FederateNotExecutionMember);
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
    ASSERT_THROW(f.addConstrained(ukn_federate), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, AddConstrainedDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);
    
    f.addConstrained(fed);
    
    ASSERT_THROW(f.addConstrained(fed), ::certi::RTIinternalError);
}

TEST_F(FederationTest, RemoveConstrainedThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.removeConstrained(ukn_federate), ::certi::FederateNotExecutionMember);
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
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason() << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
    
    ASSERT_EQ(1, f.getNbRegulators());
    
    ASSERT_TRUE(fed.isRegulator());
    
    try {
        f.removeRegulator(handle);
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason() << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
    
    ASSERT_FALSE(fed.isRegulator());
}

TEST_F(FederationTest, AddRegulatorThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.addRegulator(ukn_federate, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, AddRegulatorDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);
    
    try {
        f.addRegulator(fed, {});
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason() << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
    
    ASSERT_THROW(f.addRegulator(fed, {}), ::certi::RTIinternalError);
}

TEST_F(FederationTest, RemoveRegulatorThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.removeRegulator(ukn_federate), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, RemoveRegulatorDoesNotWorkTwice)
{
    auto fed = f.add("new_federate", nullptr);
    
    // Already not regulator
    
    ASSERT_THROW(f.removeRegulator(fed), ::certi::RTIinternalError);
}

TEST_F(FederationTest, UpdateRegulatorNeedsValidFederateIfNotAnonymous)
{
    ASSERT_THROW(f.updateRegulator(ukn_federate, {}, false), ::certi::FederateNotExecutionMember);
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
    ASSERT_THROW(f.registerSynchronization(ukn_federate, "label", "tag"), ::certi::FederateNotExecutionMember);
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
    ASSERT_THROW(f.registerSynchronization(ukn_federate, "label", "tag", 0, {}), ::certi::FederateNotExecutionMember);
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

TEST_F(FederationTest, UnregisterSynchronizationPerSetThrowsIfUknFederate)
{
    ASSERT_THROW(f.unregisterSynchronization(ukn_federate, "label"), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, UnregisterSynchronizationPerSetThrowsIfLabelEmpty)
{
    auto handle = f.add("fed", nullptr);
    
    ASSERT_THROW(f.unregisterSynchronization(handle, ""), ::certi::RTIinternalError);
}

TEST_F(FederationTest, UnregisterSynchronizationRemovesSyncLabelFromFederate)
{
    auto handle = f.add("fed", nullptr);
    
    // add another fed to return early
    f.add("other_fed", nullptr);
    
    f.registerSynchronization(handle, "label", "tag");
    
    f.unregisterSynchronization(handle, "label");
    
    ASSERT_FALSE(f.getFederate(handle).isSynchronizationLabel("label"));
}

TEST_F(FederationTest, UnregisterSynchronizationWithOtherFederatesPausedStillSynchronizing)
{
    auto handle = f.add("fed", nullptr);
    
    // add another fed to return early
    f.add("other_fed", nullptr);
    
    f.registerSynchronization(handle, "label", "tag");
    
    f.unregisterSynchronization(handle, "label");
    
    ASSERT_TRUE(f.isSynchronizing());
}

TEST_F(FederationTest, UnregisterSynchronizationStopsSynchronizing)
{
    auto handle = f.add("fed", nullptr);
    
    f.registerSynchronization(handle, "label", "tag");
    
    try {
        f.unregisterSynchronization(handle, "label");
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason() << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
    
    ASSERT_FALSE(f.isSynchronizing());
}

TEST_F(FederationTest, BroadcastSynchronizationThrowOnUknFederate)
{
    ASSERT_THROW(f.broadcastSynchronization(ukn_federate, "label", "tag"), ::certi::FederateNotExecutionMember);
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
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason() << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
}

TEST_F(FederationTest, BroadcastSynchronizationPerSetThrowOnUknFederate)
{
    ASSERT_THROW(f.broadcastSynchronization(ukn_federate, "label", "tag", 0, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, BroadcastSynchronizationPerSetThrowOnEmptyLabel)
{
    auto handle = f.add("fed", nullptr);
    
    ASSERT_THROW(f.broadcastSynchronization(handle, "", "tag", 0, {}), ::certi::RTIinternalError);
}

TEST_F(FederationTest, broadcastSynchronizationPerSetSendsNM)
{
    auto handle = f.add("fed", nullptr);
    
    try {
        f.broadcastSynchronization(handle, "label", "tag", 0, {});
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason() << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
}

TEST_F(FederationTest, RequestSaveThrowOnUknFederate)
{
    ASSERT_THROW(f.requestFederationSave(ukn_federate, "label"), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, RequestSaveSetsFederateSavingFlag)
{
    auto handle = f.add("fed", nullptr);
    
    try {
        f.requestFederationSave(handle, "save");
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason() << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
    
    ASSERT_TRUE(f.getFederate(handle).isSaving());
}

TEST_F(FederationTest, RequestSaveThrowsIfAlreadySaving)
{
    auto handle = f.add("fed", nullptr);
    
    try {
        f.requestFederationSave(handle, "save");
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason() << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
    
    ASSERT_THROW(f.requestFederationSave(handle, "save_again"), ::certi::SaveInProgress);
}

TEST_F(FederationTest, RequestTimedSaveThrowOnUknFederate)
{
    ASSERT_THROW(f.requestFederationSave(ukn_federate, "label", {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, RequestTimedSaveSetsFederateSavingFlag)
{
    auto handle = f.add("fed", nullptr);
    
    try {
        f.requestFederationSave(handle, "save", {});
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason() << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
    
    ASSERT_TRUE(f.getFederate(handle).isSaving());
}

TEST_F(FederationTest, RequestTimedSaveThrowsIfAlreadySaving)
{
    auto handle = f.add("fed", nullptr);
    
    try {
        f.requestFederationSave(handle, "save", {});
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason() << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
    
    ASSERT_THROW(f.requestFederationSave(handle, "save_again", {}), ::certi::SaveInProgress);
}

TEST_F(FederationTest, CreateRegionThrowsOnUknFederate)
{
    ASSERT_THROW(f.createRegion(ukn_federate, 1, 1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, ModifyRegionThrowsOnUknFederate)
{
    ASSERT_THROW(f.modifyRegion(ukn_federate, 1, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, DeleteRegionThrowsOnUknFederate)
{
    ASSERT_THROW(f.deleteRegion(ukn_federate, 1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, DeleteRegionThrowsIfSaveInProgress)
{
    auto handle = f.add("fed", nullptr);
    
    try {
        f.requestFederationSave(handle, "save");
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason() << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
    
    ASSERT_THROW(f.deleteRegion(handle, 1), ::certi::SaveInProgress);
}

// TEST_F(FederationTest, DeleteRegionThrowsIfRestoreInProgress)
// {
//     // TODO do not hijack
//     f.restoreInProgress = true;
//     ASSERT_THROW(f.deleteRegion(ukn_federate, 1), ::certi::RestoreInProgress);
// }

TEST_F(FederationTest, AssociateRegionThrowsOnUknFederate)
{
    ASSERT_THROW(f.associateRegion(ukn_federate, 1, 1, 1, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, UnassociateRegionThrowsOnUknFederate)
{
    ASSERT_THROW(f.unassociateRegion(ukn_federate, 1, 1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, BroadcastInteractionTimedThrowsOnUknFederate)
{
    ASSERT_THROW(f.broadcastInteraction(ukn_federate, 1, {}, {}, 0, {}, 1, ""), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, BroadcastInteractionThrowsOnUknFederate)
{
    ASSERT_THROW(f.broadcastInteraction(ukn_federate, 1, {}, {}, 0, 1, ""), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, DeleteObjectTimedThrowsOnUknFederate)
{
    ASSERT_THROW(f.deleteObject(ukn_federate, 1, {}, "tag"), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, DeleteObjectThrowsOnUknFederate)
{
    ASSERT_THROW(f.deleteObject(ukn_federate, 1, "tag"), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, FederateSaveBegunThrowsOnUknFederate)
{
    ASSERT_THROW(f.federateSaveBegun(1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, RequestFederationRestoreThrowsOnUknFederate)
{
    ASSERT_THROW(f.requestFederationRestore(ukn_federate, "label"), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, PublishInteractionThrowsOnUknFederate)
{
    ASSERT_THROW(f.publishInteraction(ukn_federate, 1, false), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, PublishObjectThrowsOnUknFederate)
{
    ASSERT_THROW(f.publishObject(ukn_federate, 1, {}, false), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, SubscribeInteractionThrowsOnUknFederate)
{
    ASSERT_THROW(f.subscribeInteraction(ukn_federate, 1, false), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, SubscribeObjectThrowsOnUknFederate)
{
    ASSERT_THROW(f.subscribeObject(ukn_federate, 1, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, UpdateAttrValuesTimedThrowsOnUknFederate)
{
    ASSERT_THROW(f.updateAttributeValues(ukn_federate, 1, {}, {}, 0, {}, ""), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, UpdateAttrValuesThrowsOnUknFederate)
{
    ASSERT_THROW(f.updateAttributeValues(ukn_federate, 1, {}, {}, 0, ""), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, IsOwnerThrowsOnUknFederate)
{
    ASSERT_THROW(f.isOwner(ukn_federate, 1, 1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, QueryAttrOwnershipThrowsOnUknFederate)
{
    ASSERT_THROW(f.queryAttributeOwnership(ukn_federate, 1, 1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, NegotiateDivestitureThrowsOnUknFederate)
{
    ASSERT_THROW(f.negotiateDivestiture(ukn_federate, 1, {}, 0, ""), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, AcquireIfAvailableThrowsOnUknFederate)
{
    ASSERT_THROW(f.acquireIfAvailable(ukn_federate, 1, {}, 0), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, DivestThrowsOnUknFederate)
{
    ASSERT_THROW(f.divest(ukn_federate, 1, {}, 0), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, AcquireThrowsOnUknFederate)
{
    ASSERT_THROW(f.acquire(ukn_federate, 1, {}, 0, ""), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, CancelDivestitureThrowsOnUknFederate)
{
    ASSERT_THROW(f.cancelDivestiture(ukn_federate, 1, {}, 0), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, RespondReleaseThrowsOnUknFederate)
{
    ASSERT_THROW(f.respondRelease(ukn_federate, 1, {}, 0), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, CancelAcquisitionThrowsOnUknFederate)
{
    ASSERT_THROW(f.cancelAcquisition(ukn_federate, 1, {}, 0), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, SubscribeAttrWRThrowsOnUknFederate)
{
    ASSERT_THROW(f.subscribeAttributesWR(ukn_federate, 1, 1, 0, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, UnsubscribeAttributesWRThrowsOnUknFederate)
{
    ASSERT_THROW(f.unsubscribeAttributesWR(ukn_federate, 1, 1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, SubscribeInteractionWRThrowsOnUknFederate)
{
    ASSERT_THROW(f.subscribeInteractionWR(ukn_federate, 1, 1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, UnsubscribeInteractionWRThrowsOnUknFederate)
{
    ASSERT_THROW(f.unsubscribeInteractionWR(ukn_federate, 1, 1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, RegisterObjectWithRegionThrowsOnUknFederate)
{
    ASSERT_THROW(f.registerObjectWithRegion(ukn_federate, 1, "name", 1, 0, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, FederateSaveStatusThrowsOnUknFederate)
{
    ASSERT_THROW(f.federateSaveStatus(ukn_federate, true), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, FederateSaveStatusUpdatesFlagAndUnderlyingFederate)
{
    auto handle = f.add("fed", nullptr);
    
    f.add("other", nullptr); // to return early
    
    try {
            f.requestFederationSave(handle, "save");
        }
        catch (certi::FederateNotExecutionMember& e) {
            // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
        }
        catch (::certi::RTIinternalError& e) {
            FAIL() << e.name() << " - " << e.reason() << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
        }
        
    f.federateSaveStatus(handle, true);
    
    ASSERT_FALSE(f.getFederate(handle).isSaving());
}

TEST_F(FederationTest, FederateRestoreStatusThrowsOnUknFederate)
{
    ASSERT_THROW(f.federateRestoreStatus(ukn_federate, true), ::certi::FederateNotExecutionMember);
}

// BUG Not checking if federate is known
// TEST_F(FederationTest, RegisterObjectThrowsOnUknFederate)
// {
//     ASSERT_THROW(f.registerObject(ukn_federate, 1, {}), ::certi::FederateNotExecutionMember);
// }

TEST_F(FederationTest, updateLastNERxForFederateThrowsOnUknFederate)
{
    ASSERT_THROW(f.updateLastNERxForFederate(ukn_federate, {}), ::certi::FederateNotExecutionMember);
}
