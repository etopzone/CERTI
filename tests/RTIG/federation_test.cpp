#include <gtest/gtest.h>

#define TEST_FOR_FEDERATION
#include <RTIG/Federation.hh>

#include <libCERTI/AuditFile.hh>
#include <libCERTI/NM_Classes.hh>
#include <libCERTI/SocketServer.hh>
#include <libCERTI/SocketTCP.hh>

#include <config.h>

#include "temporaryenvironmentlocation.h"
#include "temporaryfedfile.h"

#include "../mocks/sockettcp_mock.h"
#include "../fakes/socketserver_fake.h"

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
static const ::certi::FederationHandle invalid_handle{0};

static const ::certi::FederationHandle federation_handle{1};

static const ::certi::FederateHandle ukn_federate{42};

static const std::string fed_type{"fed_type"};

static constexpr int quiet{0};
static constexpr int verbose{1};
}

class FederationTest : public ::testing::Test {
protected:
    FakeSocketServer s{new ::certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};

    TemporaryFedFile tmp{"Sample.fed"};

    Federation f{"name", federation_handle, s, a, {"Sample.fed"}, "", ::certi::HLA_1_3, quiet};

    MockSocketTcp federate_socket;
};

#ifdef FEDERATION_USES_MULTICAST
TEST_F(FederationTest, CtorMulticastThrowsOnNullMC)
{
    ASSERT_THROW(Federation("multicast", federation_handle, s, a, nullptr, 0), ::certi::RTIinternalError);
}
#endif

TEST_F(FederationTest, CtorThrowsOnNullHandle)
{
    ASSERT_THROW(Federation("name", invalid_handle, s, a, {"Test.fed"}, "", ::certi::HLA_1_3, quiet),
                 ::certi::RTIinternalError);
}

TEST_F(FederationTest, CtorThrowsOnEmptyName)
{
    ASSERT_THROW(Federation("", federation_handle, s, a, {"Test.fed"}, "", ::certi::HLA_1_3, quiet),
                 ::certi::RTIinternalError);
}

TEST_F(FederationTest, CtorFailIfUnableToFindFed)
{
    ASSERT_THROW(Federation("", federation_handle, s, a, {""}, "", ::certi::HLA_1_3, quiet), ::certi::RTIinternalError);
}

TEST_F(FederationTest, CtorFindsFedInSameFolder)
{
    TemporaryFedFile tmp{"SampleLocal.fed"};

    ASSERT_NO_THROW(Federation("local", federation_handle, s, a, {"SampleLocal.fed"}, "", ::certi::HLA_1_3, quiet));
}

TEST_F(FederationTest, CtorFindsFedInCertiFomPath)
{
    TemporaryEnvironmentLocation env{"CERTI_FOM_PATH"};

    TemporaryFedFile tmp{env.path() + "SampleFomPath.fed"};

    ASSERT_NO_THROW(
        Federation("fom_path", federation_handle, s, a, {"SampleFomPath.fed"}, "", ::certi::HLA_1_3, quiet));
}

TEST_F(FederationTest, CtorFindsFedInCertiHome)
{
    TemporaryEnvironmentLocation env{"CERTI_HOME"};

    TemporaryFedFile tmp{"SampleCertiHome.fed"};

    ASSERT_NO_THROW(
        Federation("certi_home", federation_handle, s, a, {"SampleCertiHome.fed"}, "", ::certi::HLA_1_3, quiet));
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

    ASSERT_NO_THROW(Federation(
        "install_prefix", federation_handle, s, a, {"SampleInstallPrefix.fed"}, "", ::certi::HLA_1_3, quiet));
}

TEST_F(FederationTest, CtorFailsIfFileIsUnopenable)
{
    ASSERT_THROW(Federation("unopenable", federation_handle, s, a, {"/root/"}, "", ::certi::HLA_1_3, quiet),
                 ::certi::CouldNotOpenFED);
}

TEST_F(FederationTest, CtorFailsIfNoExtension)
{
    TemporaryFedFile tmp{"SampleWithoutDotfed"};

    ASSERT_THROW(
        Federation("no_dot_fed", federation_handle, s, a, {"SampleWithoutDotfed"}, "", ::certi::HLA_1_3, quiet),
        ::certi::CouldNotOpenFED);
}

TEST_F(FederationTest, CtorFailsIfWrongExtension)
{
    TemporaryFedFile tmp{"SampleWithout.fde"};

    ASSERT_THROW(
        Federation("bad_extension", federation_handle, s, a, {"SampleWithout.fde"}, "", ::certi::HLA_1_3, quiet),
        ::certi::CouldNotOpenFED);
}

#ifndef HAVE_XML
TEST_F(FederationTest, CtorFailsIfXmlFedWithoutXmlSupport)
{
    TemporaryFedFile tmp{"SampleWithout.xml"};

    ASSERT_THROW(
        Federation("bad_extension", federation_handle, s, a, {"SampleWithout.xml"}, "", ::certi::HLA_1_3, quiet),
        ::certi::CouldNotOpenFED);
}
#endif

TEST_F(FederationTest, VerboseLevelChangesOutput)
{
    std::ostringstream oss;
    std::streambuf* original_cout_rdbuf = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());

    Federation f{"name", federation_handle, s, a, {"Sample.fed"}, "", ::certi::HLA_1_3, quiet};

    std::ostringstream oss2;
    std::cout.rdbuf(oss2.rdbuf());

    Federation f2{"name", federation_handle, s, a, {"Sample.fed"}, "", ::certi::HLA_1_3, verbose};

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
    ASSERT_EQ("Sample.fed", f.getFomModules().front());
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
    auto fed = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;
    ASSERT_EQ("fed", f.getFederate(fed).getName());
}

TEST_F(FederationTest, CannotAddSameFederateTwice)
{
    f.add("fed", "typeerate", {}, ::certi::HLA_1_3, &federate_socket, 0, 0);

    ASSERT_THROW(f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0),
                 ::certi::FederateAlreadyExecutionMember);

    ASSERT_EQ(1, f.getNbFederates());
}

TEST_F(FederationTest, RemoveFederateUpdatesUnderlying)
{
    auto fed = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    f.remove(fed);

    ASSERT_TRUE(f.empty());
}

TEST_F(FederationTest, RemoveFederateThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.remove(ukn_federate), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, CannotRemoveSameFederateTwice)
{
    std::cout << f.getNbFederates() << std::endl;
    auto fed = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;
    std::cout << f.getNbFederates() << std::endl;

    f.remove(fed);
    std::cout << f.getNbFederates() << std::endl;

    ASSERT_THROW(f.remove(fed), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, KillRemoveFederate)
{
    auto fed = f.add("new", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    f.kill(fed);

    ASSERT_TRUE(f.empty());
}

TEST_F(FederationTest, KillRemoveFederateFromRegulators)
{
    auto fed = f.add("new", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    try {
        f.addRegulator(fed, {});
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason()
               << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }

    f.kill(fed);

    ASSERT_EQ(0, f.getNbRegulators());
}

TEST_F(FederationTest, EmptyThrowsIfFederatesExists)
{
    f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0);

    ASSERT_EQ(1, f.getNbFederates());

    ASSERT_THROW(f.empty(), ::certi::FederatesCurrentlyJoined);
}

TEST_F(FederationTest, EmptyThrowListOfFederates)
{
    f.add("fed1", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0);

    f.add("fed2", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0);

    f.add("fed3", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0);

    try {
        f.empty();
        FAIL() << "Empty should have thrown list of federates";
    }
    catch (::certi::FederatesCurrentlyJoined& e) {
        EXPECT_TRUE(e.reason().find("fed1") != std::string::npos);
        EXPECT_TRUE(e.reason().find("fed2") != std::string::npos);
        EXPECT_TRUE(e.reason().find("fed3") != std::string::npos);
    }
    catch (...) {
        FAIL() << "Empty should have thrown list of federates";
    }
}

TEST_F(FederationTest, CheckReturnsTrueIfFederateExist)
{
    ASSERT_TRUE(f.check(f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first));
}

TEST_F(FederationTest, CheckThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.check(ukn_federate), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, GetFederateThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.getFederate(ukn_federate), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, GetFederateReturnsGoodFederate)
{
    auto handle1 = f.add("fed1", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;
    auto handle2 = f.add("fed2", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    auto& fed1 = f.getFederate(handle1);

    ASSERT_EQ(handle1, fed1.getHandle());
    ASSERT_EQ("fed1", fed1.getName());

    auto& fed2 = f.getFederate(handle2);

    ASSERT_EQ(handle2, fed2.getHandle());
    ASSERT_EQ("fed2", fed2.getName());
}

TEST_F(FederationTest, AddRemoveRegulatorAlterUnderlyingFederate)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    auto& fed = f.getFederate(handle);

    ASSERT_FALSE(fed.isRegulator()) << "Federate starts with constrained == false";

    try {
        f.addRegulator(handle, {});
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason()
               << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
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
        FAIL() << e.name() << " - " << e.reason()
               << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }

    ASSERT_FALSE(fed.isRegulator());
}

TEST_F(FederationTest, AddRegulatorThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.addRegulator(ukn_federate, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, AddRegulatorDoesNotWorkTwice)
{
    auto fed = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    try {
        f.addRegulator(fed, {});
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason()
               << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }

    ASSERT_THROW(f.addRegulator(fed, {}), ::certi::RTIinternalError);
}

TEST_F(FederationTest, RemoveRegulatorThrowsOnUnknownFederate)
{
    ASSERT_THROW(f.removeRegulator(ukn_federate), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, RemoveRegulatorDoesNotWorkTwice)
{
    auto fed = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    // Already not regulator

    ASSERT_THROW(f.removeRegulator(fed), ::certi::RTIinternalError);
}

TEST_F(FederationTest, UpdateRegulatorNeedsValidFederateIfNotAnonymous)
{
    ASSERT_THROW(f.updateRegulator(ukn_federate, {}, false), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, UpdateRegulatorNeedsValidRegulatorIfNotAnonymous)
{
    auto handle = f.add("regulator", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    ASSERT_THROW(f.updateRegulator(handle, {}, false), ::certi::RTIinternalError);
}

// TODO UpdateRegulator not tested

TEST_F(FederationTest, RegisterSynchronizationAddsLabelToAllFederates)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    auto& fed = f.getFederate(handle);

    auto& fed1 = f.getFederate(f.add("fed 1", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first);
    auto& fed2 = f.getFederate(f.add("fed 2", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first);
    auto& fed3 = f.getFederate(f.add("fed 3", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first);

    f.registerSynchronization(handle, "label", "tag");

    ASSERT_TRUE(fed.hasSynchronizationLabel("label"));

    ASSERT_TRUE(fed1.hasSynchronizationLabel("label"));
    ASSERT_TRUE(fed2.hasSynchronizationLabel("label"));
    ASSERT_TRUE(fed3.hasSynchronizationLabel("label"));
}

TEST_F(FederationTest, RegisterSynchronizationThrowsIfUknFederate)
{
    ASSERT_THROW(f.registerSynchronization(ukn_federate, "label", "tag"), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, RegisterSynchronizationThrowsIfLabelEmpty)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    ASSERT_THROW(f.registerSynchronization(handle, "", "tag"), ::certi::RTIinternalError);
}

TEST_F(FederationTest, RegisterSynchronizationThrowsIfLabelAlreadyExists)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    f.registerSynchronization(handle, "label", "tag");

    ASSERT_THROW(f.registerSynchronization(handle, "label", "tag"), ::certi::FederationAlreadyPaused);
}

// FIXME Possible bug here !!
TEST_F(FederationTest, RegisterSynchronizationPerSetIfNoSetEmitterIsNotSynchronized)
{
    auto handle = f.add("emitter", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    auto& fed = f.getFederate(handle);

    ::std::vector<::certi::FederateHandle> federatesToUpdate;

    f.registerSynchronization(handle, "label", "tag", federatesToUpdate);

    ASSERT_FALSE(fed.hasSynchronizationLabel("label")) << "Emitter is not synchronizing if set was empty";
}

TEST_F(FederationTest, RegisterSynchronizationPerSetAddsLabelToSpecifiedFederates)
{
    auto handle = f.add("emitter", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    auto& fed = f.getFederate(handle);

    auto h1 = f.add("fed 1", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;
    auto h2 = f.add("fed 2", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;
    auto h3 = f.add("fed 3", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    ::std::vector<::certi::FederateHandle> federatesToUpdate;
    federatesToUpdate.push_back(h2);
    //     federatesToUpdate.push_back(h3);

    f.registerSynchronization(handle, "label", "tag", federatesToUpdate);

    ASSERT_TRUE(fed.hasSynchronizationLabel("label")) << "The sync label must also be set to the emitter";

    ASSERT_FALSE(f.getFederate(h1).hasSynchronizationLabel("label"));
    ASSERT_TRUE(f.getFederate(h2).hasSynchronizationLabel("label"));
    ASSERT_FALSE(f.getFederate(h3).hasSynchronizationLabel("label"));
}

TEST_F(FederationTest, RegisterSynchronizationPerSetThrowsIfUknFederate)
{
    ASSERT_THROW(f.registerSynchronization(ukn_federate, "label", "tag", {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, RegisterSynchronizationPerSetThrowsIfLabelEmpty)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    ASSERT_THROW(f.registerSynchronization(handle, "", "tag", {}), ::certi::RTIinternalError);
}

TEST_F(FederationTest, RegisterSynchronizationPerSetThrowsIfLabelAlreadyExists)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    f.registerSynchronization(handle, "label", "tag");

    ASSERT_THROW(f.registerSynchronization(handle, "label", "tag", {}), ::certi::FederationAlreadyPaused);
}

TEST_F(FederationTest, UnregisterSynchronizationPerSetThrowsIfUknFederate)
{
    ASSERT_THROW(f.unregisterSynchronization(ukn_federate, "label"), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, UnregisterSynchronizationPerSetThrowsIfLabelEmpty)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    ASSERT_THROW(f.unregisterSynchronization(handle, ""), ::certi::RTIinternalError);
}

TEST_F(FederationTest, UnregisterSynchronizationRemovesSyncLabelFromFederate)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    // add another fed to return early
    f.add("other_fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0);

    f.registerSynchronization(handle, "label", "tag");

    f.unregisterSynchronization(handle, "label");

    ASSERT_FALSE(f.getFederate(handle).hasSynchronizationLabel("label"));
}

TEST_F(FederationTest, UnregisterSynchronizationWithOtherFederatesPausedStillSynchronizing)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    // add another fed to return early
    f.add("other_fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0);

    f.registerSynchronization(handle, "label", "tag");

    f.unregisterSynchronization(handle, "label");

    ASSERT_TRUE(f.isSynchronizing());
}

TEST_F(FederationTest, UnregisterSynchronizationStopsSynchronizing)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    f.registerSynchronization(handle, "label", "tag");

    try {
        f.unregisterSynchronization(handle, "label");
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason()
               << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }

    ASSERT_FALSE(f.isSynchronizing());
}

TEST_F(FederationTest, BroadcastSynchronizationThrowOnUknFederate)
{
    ASSERT_THROW(f.broadcastSynchronization(ukn_federate, "label", "tag"), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, BroadcastSynchronizationThrowOnEmptyLabel)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    ASSERT_THROW(f.broadcastSynchronization(handle, "", "tag"), ::certi::RTIinternalError);
}

TEST_F(FederationTest, broadcastSynchronizationSendsNM)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    try {
        f.broadcastSynchronization(handle, "label", "tag");
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason()
               << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
}

TEST_F(FederationTest, BroadcastSynchronizationPerSetThrowOnUknFederate)
{
    ASSERT_THROW(f.broadcastSynchronization(ukn_federate, "label", "tag", {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, BroadcastSynchronizationPerSetThrowOnEmptyLabel)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    ASSERT_THROW(f.broadcastSynchronization(handle, "", "tag", {}), ::certi::RTIinternalError);
}

TEST_F(FederationTest, broadcastSynchronizationPerSetSendsNM)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    try {
        f.broadcastSynchronization(handle, "label", "tag", {});
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason()
               << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }
}

TEST_F(FederationTest, RequestSaveThrowOnUknFederate)
{
    ASSERT_THROW(f.requestFederationSave(ukn_federate, "label"), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, RequestSaveSetsFederateSavingFlag)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    try {
        f.requestFederationSave(handle, "save");
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason()
               << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }

    ASSERT_TRUE(f.getFederate(handle).isSaving());
}

TEST_F(FederationTest, RequestSaveThrowsIfAlreadySaving)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    try {
        f.requestFederationSave(handle, "save");
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason()
               << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }

    ASSERT_THROW(f.requestFederationSave(handle, "save_again"), ::certi::SaveInProgress);
}

TEST_F(FederationTest, RequestTimedSaveThrowOnUknFederate)
{
    ASSERT_THROW(f.requestFederationSave(ukn_federate, "label"), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, RequestTimedSaveSetsFederateSavingFlag)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    try {
        f.requestFederationSave(handle, "save");
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason()
               << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }

    ASSERT_TRUE(f.getFederate(handle).isSaving());
}

TEST_F(FederationTest, RequestTimedSaveThrowsIfAlreadySaving)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    try {
        f.requestFederationSave(handle, "save");
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason()
               << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
    }

    ASSERT_THROW(f.requestFederationSave(handle, "save_again"), ::certi::SaveInProgress);
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
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    try {
        f.requestFederationSave(handle, "save");
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason()
               << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
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
    ASSERT_THROW(f.associateRegion(ukn_federate, 1, 1, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, UnassociateRegionThrowsOnUknFederate)
{
    ASSERT_THROW(f.unassociateRegion(ukn_federate, 1, 1), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, BroadcastInteractionTimedThrowsOnUknFederate)
{
    ASSERT_THROW(f.broadcastInteraction(ukn_federate, 1, {}, {}, {}, 1, ""), ::certi::FederateNotExecutionMember);
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
    ASSERT_THROW(f.subscribeObject(ukn_federate, 1, {}, true), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, UpdateAttrValuesTimedThrowsOnUknFederate)
{
    ASSERT_THROW(f.updateAttributeValues(ukn_federate, 1, {}, {}, {}, ""), ::certi::FederateNotExecutionMember);
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
    ASSERT_THROW(f.negotiateDivestiture(ukn_federate, 1, {}, ""), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, AcquireIfAvailableThrowsOnUknFederate)
{
    ASSERT_THROW(f.acquireIfAvailable(ukn_federate, 1, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, DivestThrowsOnUknFederate)
{
    ASSERT_THROW(f.divest(ukn_federate, 1, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, AcquireThrowsOnUknFederate)
{
    ASSERT_THROW(f.acquire(ukn_federate, 1, {}, ""), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, CancelDivestitureThrowsOnUknFederate)
{
    ASSERT_THROW(f.cancelDivestiture(ukn_federate, 1, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, RespondReleaseThrowsOnUknFederate)
{
    ASSERT_THROW(f.respondRelease(ukn_federate, 1, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, CancelAcquisitionThrowsOnUknFederate)
{
    ASSERT_THROW(f.cancelAcquisition(ukn_federate, 1, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, SubscribeAttrWRThrowsOnUknFederate)
{
    ASSERT_THROW(f.subscribeAttributesWR(ukn_federate, 1, 1, {}), ::certi::FederateNotExecutionMember);
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
    ASSERT_THROW(f.registerObjectWithRegion(ukn_federate, 1, "name", 1, {}), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, FederateSaveStatusThrowsOnUknFederate)
{
    ASSERT_THROW(f.federateSaveStatus(ukn_federate, true), ::certi::FederateNotExecutionMember);
}

TEST_F(FederationTest, FederateSaveStatusUpdatesFlagAndUnderlyingFederate)
{
    auto handle = f.add("fed", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0).first;

    f.add("other", fed_type, {}, ::certi::HLA_1_3, &federate_socket, 0, 0); // to return early

    try {
        f.requestFederationSave(handle, "save");
    }
    catch (certi::FederateNotExecutionMember& e) {
        // SocketServer is empty, so we will throw from SocketServer::getWithReferences, but the regulator should be registered
    }
    catch (::certi::RTIinternalError& e) {
        FAIL() << e.name() << " - " << e.reason()
               << " : may throw from SocketServer::getWithReferences, but not from anywhere else";
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
