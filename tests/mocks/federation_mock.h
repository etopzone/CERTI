#ifndef MOCKFEDERATION_H
#define MOCKFEDERATION_H

#include <gmock/gmock.h>

#include <RTIG/Federation.hh>
#include <libCERTI/SocketTCP.hh>

class MockFederation {
public:
    MOCK_METHOD2(add, certi::FederateHandle(const std::string& theName, certi::SocketTCP* theTCPLink));

    MOCK_CONST_METHOD0(empty, bool());

    MOCK_CONST_METHOD1(check, bool(certi::FederateHandle federate));

    MOCK_METHOD1(kill, void(certi::FederateHandle federate));

    MOCK_METHOD1(remove, void(certi::FederateHandle federate));

    MOCK_METHOD2(addRegulator, void(certi::FederateHandle theHandle, certi::FederationTime theTime));

    MOCK_METHOD3(updateRegulator, void(certi::FederateHandle theHandle, certi::FederationTime theTime, bool anonymous));

    MOCK_METHOD1(removeRegulator, void(certi::FederateHandle theHandle));

    MOCK_METHOD1(addConstrained, void(certi::FederateHandle theHandle));

    MOCK_METHOD1(removeConstrained, void(certi::FederateHandle theHandle));

    MOCK_METHOD3(registerSynchronization,
                 void(certi::FederateHandle federate, const std::string& the_label, const std::string& the_tag));

    MOCK_METHOD5(registerSynchronization,
                 void(certi::FederateHandle federate,
                      const std::string& the_label,
                      const std::string& the_tag,
                      unsigned short federate_setSize,
                      const std::vector<certi::FederateHandle>& federate_set));

    MOCK_METHOD2(unregisterSynchronization, void(certi::FederateHandle federate, const std::string& theLabel));

    MOCK_METHOD3(broadcastSynchronization,
                 void(certi::FederateHandle federate, const std::string& label, const std::string& tag));

    MOCK_METHOD5(broadcastSynchronization,
                 void(certi::FederateHandle federate,
                      const std::string& label,
                      const std::string& tag,
                      unsigned short federate_setSize,
                      const std::vector<certi::FederateHandle>& federate_set));

    MOCK_METHOD3(requestFederationSave,
                 void(certi::FederateHandle federate, const std::string&, certi::FederationTime));

    MOCK_METHOD2(requestFederationSave, void(certi::FederateHandle federate, const std::string&));

    MOCK_METHOD1(federateSaveBegun, void(certi::FederateHandle));

    MOCK_METHOD2(federateSaveStatus, void(certi::FederateHandle federate, bool));

    MOCK_METHOD2(requestFederationRestore, void(certi::FederateHandle federate, const std::string& the_label));

    MOCK_METHOD2(federateRestoreStatus, void(certi::FederateHandle federate, bool the_status));

    MOCK_METHOD4(deleteObject,
                 void(certi::FederateHandle federate,
                      certi::ObjectHandle object_handle,
                      certi::FederationTime theTime,
                      const std::string& theUserTag));

    MOCK_METHOD3(deleteObject,
                 void(certi::FederateHandle federate,
                      certi::ObjectHandle object_handle,
                      const std::string& theUserTag));

    MOCK_METHOD4(publishObject,
                 void(certi::FederateHandle federate,
                      certi::ObjectClassHandle class_handle,
                      const std::vector<certi::AttributeHandle>& theAttributeList,
                      bool PubOrUnpub));

    MOCK_METHOD2(reserveObjectInstanceName, void(certi::FederateHandle federate, std::string newObjName));

    MOCK_METHOD3(registerObject,
                 certi::ObjectHandle(certi::FederateHandle federate,
                                     certi::ObjectClassHandle class_handle,
                                     const std::string& theName));

    MOCK_METHOD4(requestObjectOwner,
                 certi::FederateHandle(certi::FederateHandle federate,
                                       certi::ObjectHandle theObject,
                                       const std::vector<certi::AttributeHandle>& theAttributeList,
                                       uint32_t theListSize));

    MOCK_METHOD4(requestClassAttributeValueUpdate,
                 void(certi::FederateHandle federate,
                      certi::ObjectClassHandle theClassHandle,
                      const std::vector<certi::AttributeHandle>& theAttributeList,
                      uint32_t theListSize));

    MOCK_METHOD3(subscribeObject,
                 void(certi::FederateHandle federate,
                      certi::ObjectClassHandle object_handle,
                      const std::vector<certi::AttributeHandle>& theAttributeList));

    MOCK_METHOD7(updateAttributeValues,
                 void(certi::FederateHandle federate,
                      certi::ObjectHandle object_handle,
                      const std::vector<certi::AttributeHandle>& theAttributeList,
                      const std::vector<certi::AttributeValue_t>& theValueList,
                      uint16_t theListSize,
                      certi::FederationTime theTime,
                      const std::string& theTag));

    MOCK_METHOD6(updateAttributeValues,
                 void(certi::FederateHandle federate,
                      certi::ObjectHandle object_handle,
                      const std::vector<certi::AttributeHandle>& theAttributeList,
                      const std::vector<certi::AttributeValue_t>& theValueList,
                      uint16_t theListSize,
                      const std::string& theTag));

    MOCK_METHOD8(broadcastInteraction,
                 void(certi::FederateHandle federate,
                      certi::InteractionClassHandle theInteractionHandle,
                      const std::vector<certi::ParameterHandle>& theParameterList,
                      const std::vector<certi::ParameterValue_t>& theValueList,
                      uint16_t theListSize,
                      certi::FederationTime theTime,
                      certi::RegionHandle region,
                      const std::string& theTag));

    MOCK_METHOD7(broadcastInteraction,
                 void(certi::FederateHandle federate,
                      certi::InteractionClassHandle theInteractionHandle,
                      const std::vector<certi::ParameterHandle>& theParameterList,
                      const std::vector<certi::ParameterValue_t>& theValueList,
                      uint16_t theListSize,
                      certi::RegionHandle region,
                      const std::string& theTag));

    MOCK_METHOD3(publishInteraction,
                 void(certi::FederateHandle federate,
                      certi::InteractionClassHandle theInteractionHandle,
                      bool PubOrUnpub));

    MOCK_METHOD3(subscribeInteraction,
                 void(certi::FederateHandle federate,
                      certi::InteractionClassHandle theInteractionHandle,
                      bool SubOrUnsub));

    MOCK_METHOD3(isOwner,
                 bool(certi::FederateHandle federate,
                      certi::ObjectHandle object_handle,
                      certi::AttributeHandle theAttribute));

    MOCK_METHOD3(queryAttributeOwnership,
                 void(certi::FederateHandle federate,
                      certi::ObjectHandle object_handle,
                      certi::AttributeHandle theAttribute));

    MOCK_METHOD5(negotiateDivestiture,
                 void(certi::FederateHandle federate,
                      certi::ObjectHandle object_handle,
                      const std::vector<certi::AttributeHandle>& theAttributeList,
                      uint16_t theListSize,
                      const std::string& theTag));

    MOCK_METHOD4(acquireIfAvailable,
                 void(certi::FederateHandle federate,
                      certi::ObjectHandle object_handle,
                      const std::vector<certi::AttributeHandle>& theAttributeList,
                      uint16_t theListSize));

    MOCK_METHOD4(divest,
                 void(certi::FederateHandle federate,
                      certi::ObjectHandle object_handle,
                      const std::vector<certi::AttributeHandle>& theAttributeList,
                      uint16_t theListSize));

    MOCK_METHOD5(acquire,
                 void(certi::FederateHandle federate,
                      certi::ObjectHandle object_handle,
                      const std::vector<certi::AttributeHandle>& theAttributeList,
                      uint16_t theListSize,
                      const std::string& theTag));

    MOCK_METHOD4(cancelDivestiture,
                 void(certi::FederateHandle federate,
                      certi::ObjectHandle object_handle,
                      const std::vector<certi::AttributeHandle>& theAttributeList,
                      uint16_t theListSize));

    MOCK_METHOD4(respondRelease,
                 certi::AttributeHandleSet*(certi::FederateHandle federate,
                                            certi::ObjectHandle object_handle,
                                            const std::vector<certi::AttributeHandle>& theAttributeList,
                                            uint16_t theListSize));

    MOCK_METHOD4(cancelAcquisition,
                 void(certi::FederateHandle federate,
                      certi::ObjectHandle object_handle,
                      const std::vector<certi::AttributeHandle>& theAttributeList,
                      uint16_t theListSize));

    MOCK_METHOD3(createRegion, long(certi::FederateHandle federate, certi::SpaceHandle, long));

    MOCK_METHOD3(modifyRegion,
                 void(certi::FederateHandle federate, certi::RegionHandle, const std::vector<certi::Extent>&));

    MOCK_METHOD2(deleteRegion, void(certi::FederateHandle federate, long));

    MOCK_METHOD5(associateRegion,
                 void(certi::FederateHandle federate,
                      certi::ObjectHandle,
                      certi::RegionHandle,
                      unsigned short,
                      const std::vector<certi::AttributeHandle>&));

    MOCK_METHOD3(unassociateRegion, void(certi::FederateHandle federate, certi::ObjectHandle, certi::RegionHandle));

    MOCK_METHOD5(subscribeAttributesWR,
                 void(certi::FederateHandle,
                      certi::ObjectClassHandle,
                      certi::RegionHandle,
                      unsigned short,
                      const std::vector<certi::AttributeHandle>&));

    MOCK_METHOD3(unsubscribeAttributesWR,
                 void(certi::FederateHandle federate, certi::ObjectClassHandle, certi::RegionHandle));

    MOCK_METHOD3(subscribeInteractionWR,
                 void(certi::FederateHandle federate, certi::InteractionClassHandle, certi::RegionHandle));

    MOCK_METHOD3(unsubscribeInteractionWR,
                 void(certi::FederateHandle federate, certi::InteractionClassHandle, certi::RegionHandle));

    MOCK_METHOD6(registerObjectWithRegion,
                 certi::ObjectHandle(certi::FederateHandle,
                                     certi::ObjectClassHandle,
                                     const std::string&,
                                     certi::RegionHandle,
                                     int,
                                     const std::vector<certi::AttributeHandle>&));

    MOCK_METHOD1(setClassRelevanceAdvisorySwitch, void(certi::FederateHandle theHandle));

    MOCK_METHOD1(unsetClassRelevanceAdvisorySwitch, void(certi::FederateHandle theHandle));

    MOCK_METHOD1(setInteractionRelevanceAdvisorySwitch, void(certi::FederateHandle theHandle));

    MOCK_METHOD1(unsetInteractionRelevanceAdvisorySwitch, void(certi::FederateHandle theHandle));

    MOCK_METHOD1(setAttributeRelevanceAdvisorySwitch, void(certi::FederateHandle theHandle));

    MOCK_METHOD1(unsetAttributeRelevanceAdvisorySwitch, void(certi::FederateHandle theHandle));

    MOCK_METHOD1(setAttributeScopeAdvisorySwitch, void(certi::FederateHandle theHandle));

    MOCK_METHOD1(unsetAttributeScopeAdvisorySwitch, void(certi::FederateHandle theHandle));

    MOCK_METHOD1(getFOM, void(certi::NM_Join_Federation_Execution& objectModelData));

    MOCK_METHOD2(updateLastNERxForFederate, bool(certi::FederateHandle federate, certi::FederationTime date));

    MOCK_METHOD0(computeMinNERx, certi::FederationTime());

    MOCK_CONST_METHOD0(getMinNERx, certi::FederationTime ());
};

#endif // MOCKFEDERATION_H
