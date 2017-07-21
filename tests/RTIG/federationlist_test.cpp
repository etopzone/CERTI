#include <gtest/gtest.h>

#include <RTIG/FederationsList.hh>

#include <libCERTI/AuditFile.hh>
#include <libCERTI/SocketServer.hh>

#include <config.h>

#include <libCERTI/NM_Classes.hh>

#include "temporaryfedfile.h"

using ::certi::rtig::FederationsList;

/** TODO
 * 
 * killFederate
 * 
 * setVerboseLevel
 * 
 * handleMessageNullPrime
 * 
 * getNullPrimeValue(FederationHandle federation);
 * 
 * private:
 *    searchFederation(Handle federationHandle)
 */

namespace {
static constexpr ::certi::Handle invalid_handle{0};

static constexpr ::certi::FederationHandle ukn_federation{42};
static constexpr ::certi::Handle ukn_handle{1337};

static constexpr ::certi::FederationHandle federation_handle{1};
}

class FederationsListTest : public ::testing::Test {
protected:
    ::certi::SocketServer s{new certi::SocketTCP{}, nullptr};
    ::certi::AuditFile a{"tmp"};

    FederationsList f{s, a};
};

TEST_F(FederationsListTest, createFederationThrowsOnEmptyName)
{
    ASSERT_THROW(f.createFederation("", federation_handle, ""),
                 ::certi::RTIinternalError);
}

TEST_F(FederationsListTest, createFederationRethrows)
{
    ASSERT_THROW(f.createFederation("fed", federation_handle, ""),
                 ::certi::CouldNotOpenFED);
}

TEST_F(FederationsListTest, createFederationDoesNotWorkTwice)
{
    TemporaryFedFile tmp{"FedList.fed"};
    f.createFederation("fed", federation_handle, "FedList.fed");
    
    ASSERT_THROW(f.createFederation("fed", federation_handle, ""),
                 ::certi::FederationExecutionAlreadyExists);
}

TEST_F(FederationsListTest, getFederationHandleThrowsOnUknFederation)
{
    ASSERT_THROW(f.getFederationHandle("this does not exists"),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, getFederationHandleReturnsHandleFromCreate)
{
    TemporaryFedFile tmp{"FedList.fed"};
    f.createFederation("fed", federation_handle, "FedList.fed");
    
    ASSERT_EQ(federation_handle, f.getFederationHandle("fed"));
}

TEST_F(FederationsListTest, infoThrowsOnUknFederation)
{
    int nbFeds{10}, nbRegs{10};
    bool isSyncing{false};
    
    TemporaryFedFile tmp{"FedList.fed"};
    f.createFederation("fed", federation_handle, "FedList.fed");
    
    f.info(federation_handle, nbFeds, nbRegs, isSyncing);
    
    ASSERT_EQ(0, nbFeds);
    ASSERT_EQ(0, nbRegs);
    ASSERT_EQ(false, isSyncing);
}

TEST_F(FederationsListTest, destroyFederationThrowsOnUknFederation)
{
    ASSERT_THROW(f.destroyFederation(ukn_federation),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, killFederateDoesNotThrowsOnUknFederation)
{
    ASSERT_NO_THROW(f.killFederate(ukn_federation, ukn_handle));
}

TEST_F(FederationsListTest, addFederateThrowsOnUknFederation)
{
    certi::NM_Join_Federation_Execution message{};
    ASSERT_THROW(f.addFederate(ukn_federation, "", nullptr, message),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, subscribeObjectThrowsOnUknFederation)
{
    ASSERT_THROW(f.subscribeObject(ukn_federation, invalid_handle, invalid_handle, {}),
                 ::certi::FederationExecutionDoesNotExist);
}
