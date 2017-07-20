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

TEST_F(FederationsListTest, setClassRelevanceAdvisorySwitchThrowsOnUknFederation)
{
    ASSERT_THROW(f.setClassRelevanceAdvisorySwitch(ukn_federation, invalid_handle),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, unsetClassRelevanceAdvisorySwitchThrowsOnUknFederation)
{
    ASSERT_THROW(f.unsetClassRelevanceAdvisorySwitch(ukn_federation, invalid_handle),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, setInteractionRelevanceAdvisorySwitchThrowsOnUknFederation)
{
    ASSERT_THROW(f.setInteractionRelevanceAdvisorySwitch(ukn_federation, invalid_handle),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, unsetInteractionRelevanceAdvisorySwitchThrowsOnUknFederation)
{
    ASSERT_THROW(f.unsetInteractionRelevanceAdvisorySwitch(ukn_federation, invalid_handle),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, setAttributeRelevanceAdvisorySwitchThrowsOnUknFederation)
{
    ASSERT_THROW(f.setAttributeRelevanceAdvisorySwitch(ukn_federation, invalid_handle),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, unsetAttributeRelevanceAdvisorySwitchThrowsOnUknFederation)
{
    ASSERT_THROW(f.unsetAttributeRelevanceAdvisorySwitch(ukn_federation, invalid_handle),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, setAttributeScopeAdvisorySwitchThrowsOnUknFederation)
{
    ASSERT_THROW(f.setAttributeScopeAdvisorySwitch(ukn_federation, invalid_handle),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, unsetAttributeScopeAdvisorySwitchThrowsOnUknFederation)
{
    ASSERT_THROW(f.unsetAttributeScopeAdvisorySwitch(ukn_federation, invalid_handle),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, addConstrainedThrowsOnUknFederation)
{
    ASSERT_THROW(f.addConstrained(ukn_federation, invalid_handle), ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, createRegulatorThrowsOnUknFederation)
{
    ASSERT_THROW(f.createRegulator(ukn_federation, invalid_handle, {}), ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, destroyObjectThrowsOnUknFederation)
{
    ASSERT_THROW(f.destroyObject(ukn_federation, invalid_handle, invalid_handle, "tag"),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, destroyObjectWithTimeThrowsOnUknFederation)
{
    ASSERT_THROW(f.destroyObject(ukn_federation, invalid_handle, invalid_handle, {}, "tag"),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, registerObjectThrowsOnUknFederation)
{
    ASSERT_THROW(f.registerObject(ukn_federation, invalid_handle, invalid_handle, "name"),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, updateRegulatorThrowsOnUknFederation)
{
    ASSERT_THROW(f.updateRegulator(ukn_federation, invalid_handle, invalid_handle, true),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, updateAttributeThrowsOnUknFederation)
{
    ASSERT_THROW(f.updateAttribute(ukn_federation, invalid_handle, invalid_handle, {}, {}, 0, {}, ""),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, updateAttributeWithTimeThrowsOnUknFederation)
{
    ASSERT_THROW(f.updateAttribute(ukn_federation, invalid_handle, invalid_handle, {}, {}, 0, ""),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, updateParameterThrowsOnUknFederation)
{
    ASSERT_THROW(f.updateParameter(ukn_federation, invalid_handle, invalid_handle, {}, {}, 0, invalid_handle, ""),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, updateParameterWithTimeThrowsOnUknFederation)
{
    ASSERT_THROW(f.updateParameter(ukn_federation, invalid_handle, invalid_handle, {}, {}, 0, {}, invalid_handle, ""),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, manageSynchronizationThrowsOnUknFederation)
{
    ASSERT_THROW(f.manageSynchronization(ukn_federation, invalid_handle, false, "", ""),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, manageSynchronizationWithSetThrowsOnUknFederation)
{
    ASSERT_THROW(f.manageSynchronization(ukn_federation, invalid_handle, false, "", "", 0, {}),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, broadcastSynchronizationThrowsOnUknFederation)
{
    ASSERT_THROW(f.broadcastSynchronization(ukn_federation, invalid_handle, "", ""),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, broadcastSynchronizationWithSetThrowsOnUknFederation)
{
    ASSERT_THROW(f.broadcastSynchronization(ukn_federation, invalid_handle, "", "", 0, {}),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, publishInteractionThrowsOnUknFederation)
{
    ASSERT_THROW(f.publishInteraction(ukn_federation, invalid_handle, invalid_handle, true),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, publishObjectThrowsOnUknFederation)
{
    ASSERT_THROW(f.publishObject(ukn_federation, invalid_handle, invalid_handle, {}, true),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, subscribeInteractionThrowsOnUknFederation)
{
    ASSERT_THROW(f.subscribeInteraction(ukn_federation, invalid_handle, invalid_handle, true),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, subscribeObjectThrowsOnUknFederation)
{
    ASSERT_THROW(f.subscribeObject(ukn_federation, invalid_handle, invalid_handle, {}),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, removeConstrainedThrowsOnUknFederation)
{
    ASSERT_THROW(f.removeConstrained(ukn_federation, invalid_handle), ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, removeThrowsOnUknFederation)
{
    ASSERT_THROW(f.remove(ukn_federation, invalid_handle), ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, removeRegulatorThrowsOnUknFederation)
{
    ASSERT_THROW(f.removeRegulator(ukn_federation, invalid_handle), ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, isOwnerThrowsOnUknFederation)
{
    ASSERT_THROW(f.isOwner(ukn_federation, invalid_handle, invalid_handle, invalid_handle),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, searchOwnerThrowsOnUknFederation)
{
    ASSERT_THROW(f.searchOwner(ukn_federation, invalid_handle, invalid_handle, invalid_handle),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, negotiateDivestitureThrowsOnUknFederation)
{
    ASSERT_THROW(f.negotiateDivestiture(ukn_federation, invalid_handle, invalid_handle, {}, 0, "tag"),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, acquireIfAvailableThrowsOnUknFederation)
{
    ASSERT_THROW(f.acquireIfAvailable(ukn_federation, invalid_handle, invalid_handle, {}, 0),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, divestThrowsOnUknFederation)
{
    ASSERT_THROW(f.divest(ukn_federation, invalid_handle, invalid_handle, {}, 0),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, acquireThrowsOnUknFederation)
{
    ASSERT_THROW(f.acquire(ukn_federation, invalid_handle, invalid_handle, {}, 0, "tag"),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, cancelDivestitureThrowsOnUknFederation)
{
    ASSERT_THROW(f.cancelDivestiture(ukn_federation, invalid_handle, invalid_handle, {}, 0),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, respondReleaseThrowsOnUknFederation)
{
    ASSERT_THROW(f.respondRelease(ukn_federation, invalid_handle, invalid_handle, {}, 0),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, cancelAcquisitionThrowsOnUknFederation)
{
    ASSERT_THROW(f.cancelAcquisition(ukn_federation, invalid_handle, invalid_handle, {}, 0),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, createRegionThrowsOnUknFederation)
{
    ASSERT_THROW(f.createRegion(ukn_federation, invalid_handle, invalid_handle, 0),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, modifyRegionThrowsOnUknFederation)
{
    ASSERT_THROW(f.modifyRegion(ukn_federation, invalid_handle, invalid_handle, {}),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, deleteRegionThrowsOnUknFederation)
{
    ASSERT_THROW(f.deleteRegion(ukn_federation, invalid_handle, invalid_handle),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, associateRegionThrowsOnUknFederation)
{
    ASSERT_THROW(f.associateRegion(ukn_federation, invalid_handle, invalid_handle, invalid_handle, 0, {}),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, unassociateRegionThrowsOnUknFederation)
{
    ASSERT_THROW(f.unassociateRegion(ukn_federation, invalid_handle, invalid_handle, invalid_handle),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, subscribeAttributesWRThrowsOnUknFederation)
{
    ASSERT_THROW(f.subscribeAttributesWR(ukn_federation, invalid_handle, invalid_handle, invalid_handle, 0, {}),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, unsubscribeAttributesWRThrowsOnUknFederation)
{
    ASSERT_THROW(f.unsubscribeAttributesWR(ukn_federation, invalid_handle, invalid_handle, invalid_handle),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, subscribeInteractionWRThrowsOnUknFederation)
{
    ASSERT_THROW(f.subscribeInteractionWR(ukn_federation, invalid_handle, invalid_handle, invalid_handle),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, unsubscribeInteractionWRThrowsOnUknFederation)
{
    ASSERT_THROW(f.unsubscribeInteractionWR(ukn_federation, invalid_handle, invalid_handle, invalid_handle),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, registerObjectWithRegionThrowsOnUknFederation)
{
    ASSERT_THROW(f.registerObjectWithRegion(ukn_federation, invalid_handle, invalid_handle, "", invalid_handle, 0, {}),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, requestFederationSaveThrowsOnUknFederation)
{
    ASSERT_THROW(f.requestFederationSave(ukn_federation, invalid_handle, ""), ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, requestFederationSaveTimedThrowsOnUknFederation)
{
    ASSERT_THROW(f.requestFederationSave(ukn_federation, invalid_handle, "", {}),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, federateSaveBegunThrowsOnUknFederation)
{
    ASSERT_THROW(f.federateSaveBegun(ukn_federation, invalid_handle), ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, federateSaveStatusThrowsOnUknFederation)
{
    ASSERT_THROW(f.federateSaveStatus(ukn_federation, invalid_handle, false), ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, requestFederationRestoreThrowsOnUknFederation)
{
    ASSERT_THROW(f.requestFederationRestore(ukn_federation, invalid_handle, ""),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, federateRestoreStatusThrowsOnUknFederation)
{
    ASSERT_THROW(f.federateRestoreStatus(ukn_federation, invalid_handle, false),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, requestObjectOwnerThrowsOnUknFederation)
{
    ASSERT_THROW(f.requestObjectOwner(ukn_federation, invalid_handle, invalid_handle, {}, 0),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, requestClassAttributeValueUpdateThrowsOnUknFederation)
{
    ASSERT_THROW(f.requestClassAttributeValueUpdate(ukn_federation, invalid_handle, invalid_handle, {}, 0),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, reserveObjectInstanceNameThrowsOnUknFederation)
{
    ASSERT_THROW(f.reserveObjectInstanceName(ukn_federation, invalid_handle, ""),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, handleMessageNullPrimeThrowsOnUknFederation)
{
    ASSERT_THROW(f.handleMessageNullPrime(ukn_federation, invalid_handle, {}),
                 ::certi::FederationExecutionDoesNotExist);
}

TEST_F(FederationsListTest, getNullPrimeValueThrowsOnUknFederation)
{
    ASSERT_THROW(f.getNullPrimeValue(ukn_federation),
                 ::certi::FederationExecutionDoesNotExist);
}
