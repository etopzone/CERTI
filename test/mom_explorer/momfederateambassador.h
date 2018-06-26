#ifndef MOMFEDERATEAMBASSADOR_H
#define MOMFEDERATEAMBASSADOR_H

#include <RTI/FederateAmbassador.h>
#include <RTI/LogicalTime.h>
#include <RTI/RTI1516.h>

#include <iostream>
#include <map>

using namespace rti1516e;

class MOMFederateAmbassador : public FederateAmbassador {
public:
    MOMFederateAmbassador(RTIambassador& ambassador,
                          const std::wstring& federation_name,
                          const std::wstring& federate_name,
                          const bool is_auto = false,
                          const int report_period = 1,
                          const int report_style = 0
                         );

    virtual ~MOMFederateAmbassador() = default;

    void connect();

    void createFederationExecution();

    void joinFederationExecution();

    void subscribeObjectClasses();

    void publishAndsubscribeInteractions();

    // 4.4
    virtual void connectionLost(std::wstring const& faultDescription) throw(FederateInternalError) override;

    // 4.8
    virtual void
    reportFederationExecutions(FederationExecutionInformationVector const& theFederationExecutionInformationList) throw(
        FederateInternalError) override;

    // 4.12
    virtual void
    synchronizationPointRegistrationSucceeded(std::wstring const& label) throw(FederateInternalError) override;

    virtual void synchronizationPointRegistrationFailed(
        std::wstring const& label, SynchronizationPointFailureReason reason) throw(FederateInternalError) override;

    // 4.13
    virtual void
    announceSynchronizationPoint(std::wstring const& label,
                                 VariableLengthData const& theUserSuppliedTag) throw(FederateInternalError) override;

    // 4.15
    virtual void federationSynchronized(std::wstring const& label,
                                        FederateHandleSet const& failedToSyncSet) throw(FederateInternalError) override;

    // 4.17
    virtual void initiateFederateSave(std::wstring const& label) throw(FederateInternalError) override;

    virtual void initiateFederateSave(std::wstring const& label,
                                      LogicalTime const& theTime) throw(FederateInternalError) override;

    // 4.20
    virtual void federationSaved() throw(FederateInternalError) override;

    virtual void federationNotSaved(SaveFailureReason theSaveFailureReason) throw(FederateInternalError) override;

    // 4.23
    virtual void federationSaveStatusResponse(FederateHandleSaveStatusPairVector const& theFederateStatusVector) throw(
        FederateInternalError) override;

    // 4.25
    virtual void requestFederationRestoreSucceeded(std::wstring const& label) throw(FederateInternalError) override;

    virtual void requestFederationRestoreFailed(std::wstring const& label) throw(FederateInternalError) override;

    // 4.26
    virtual void federationRestoreBegun() throw(FederateInternalError) override;

    // 4.27
    virtual void initiateFederateRestore(std::wstring const& label,
                                         std::wstring const& federateName,
                                         FederateHandle handle) throw(FederateInternalError) override;

    // 4.29
    virtual void federationRestored() throw(FederateInternalError) override;

    virtual void
    federationNotRestored(RestoreFailureReason theRestoreFailureReason) throw(FederateInternalError) override;

    // 4.32
    virtual void federationRestoreStatusResponse(
        FederateRestoreStatusVector const& theFederateRestoreStatusVector) throw(FederateInternalError) override;

    /////////////////////////////////////
    // Declaration Management Services //
    /////////////////////////////////////

    // 5.10
    virtual void startRegistrationForObjectClass(ObjectClassHandle theClass) throw(FederateInternalError) override;

    // 5.11
    virtual void stopRegistrationForObjectClass(ObjectClassHandle theClass) throw(FederateInternalError) override;

    // 5.12
    virtual void turnInteractionsOn(InteractionClassHandle theHandle) throw(FederateInternalError) override;

    // 5.13
    virtual void turnInteractionsOff(InteractionClassHandle theHandle) throw(FederateInternalError) override;

    ////////////////////////////////
    // Object Management Services //
    ////////////////////////////////

    // 6.3
    virtual void objectInstanceNameReservationSucceeded(std::wstring const& theObjectInstanceName) throw(
        FederateInternalError) override;

    virtual void objectInstanceNameReservationFailed(std::wstring const& theObjectInstanceName) throw(
        FederateInternalError) override;

    // 6.6
    virtual void multipleObjectInstanceNameReservationSucceeded(
        std::set<std::wstring> const& theObjectInstanceNames) throw(FederateInternalError) override;

    virtual void multipleObjectInstanceNameReservationFailed(
        std::set<std::wstring> const& theObjectInstanceNames) throw(FederateInternalError) override;

    // 6.9
    virtual void
    discoverObjectInstance(ObjectInstanceHandle theObject,
                           ObjectClassHandle theObjectClass,
                           std::wstring const& theObjectInstanceName) throw(FederateInternalError) override;

    virtual void discoverObjectInstance(ObjectInstanceHandle theObject,
                                        ObjectClassHandle theObjectClass,
                                        std::wstring const& theObjectInstanceName,
                                        FederateHandle producingFederate) throw(FederateInternalError) override;

    // 6.11
    virtual void reflectAttributeValues(ObjectInstanceHandle theObject,
                                        AttributeHandleValueMap const& theAttributeValues,
                                        VariableLengthData const& theUserSuppliedTag,
                                        OrderType sentOrder,
                                        TransportationType theType,
                                        SupplementalReflectInfo theReflectInfo) throw(FederateInternalError) override;

    virtual void reflectAttributeValues(ObjectInstanceHandle theObject,
                                        AttributeHandleValueMap const& theAttributeValues,
                                        VariableLengthData const& theUserSuppliedTag,
                                        OrderType sentOrder,
                                        TransportationType theType,
                                        LogicalTime const& theTime,
                                        OrderType receivedOrder,
                                        SupplementalReflectInfo theReflectInfo) throw(FederateInternalError) override;

    virtual void reflectAttributeValues(ObjectInstanceHandle theObject,
                                        AttributeHandleValueMap const& theAttributeValues,
                                        VariableLengthData const& theUserSuppliedTag,
                                        OrderType sentOrder,
                                        TransportationType theType,
                                        LogicalTime const& theTime,
                                        OrderType receivedOrder,
                                        MessageRetractionHandle theHandle,
                                        SupplementalReflectInfo theReflectInfo) throw(FederateInternalError) override;

    // 6.13
    virtual void receiveInteraction(InteractionClassHandle theInteraction,
                                    ParameterHandleValueMap const& theParameterValues,
                                    VariableLengthData const& theUserSuppliedTag,
                                    OrderType sentOrder,
                                    TransportationType theType,
                                    SupplementalReceiveInfo theReceiveInfo) throw(FederateInternalError) override;

    virtual void receiveInteraction(InteractionClassHandle theInteraction,
                                    ParameterHandleValueMap const& theParameterValues,
                                    VariableLengthData const& theUserSuppliedTag,
                                    OrderType sentOrder,
                                    TransportationType theType,
                                    LogicalTime const& theTime,
                                    OrderType receivedOrder,
                                    SupplementalReceiveInfo theReceiveInfo) throw(FederateInternalError) override;

    virtual void receiveInteraction(InteractionClassHandle theInteraction,
                                    ParameterHandleValueMap const& theParameterValues,
                                    VariableLengthData const& theUserSuppliedTag,
                                    OrderType sentOrder,
                                    TransportationType theType,
                                    LogicalTime const& theTime,
                                    OrderType receivedOrder,
                                    MessageRetractionHandle theHandle,
                                    SupplementalReceiveInfo theReceiveInfo) throw(FederateInternalError) override;

    // 6.15
    virtual void removeObjectInstance(ObjectInstanceHandle theObject,
                                      VariableLengthData const& theUserSuppliedTag,
                                      OrderType sentOrder,
                                      SupplementalRemoveInfo theRemoveInfo) throw(FederateInternalError) override;

    virtual void removeObjectInstance(ObjectInstanceHandle theObject,
                                      VariableLengthData const& theUserSuppliedTag,
                                      OrderType sentOrder,
                                      LogicalTime const& theTime,
                                      OrderType receivedOrder,
                                      SupplementalRemoveInfo theRemoveInfo) throw(FederateInternalError) override;

    virtual void removeObjectInstance(ObjectInstanceHandle theObject,
                                      VariableLengthData const& theUserSuppliedTag,
                                      OrderType sentOrder,
                                      LogicalTime const& theTime,
                                      OrderType receivedOrder,
                                      MessageRetractionHandle theHandle,
                                      SupplementalRemoveInfo theRemoveInfo) throw(FederateInternalError) override;

    // 6.17
    virtual void attributesInScope(ObjectInstanceHandle theObject,
                                   AttributeHandleSet const& theAttributes) throw(FederateInternalError) override;

    // 6.18
    virtual void attributesOutOfScope(ObjectInstanceHandle theObject,
                                      AttributeHandleSet const& theAttributes) throw(FederateInternalError) override;

    // 6.20
    virtual void
    provideAttributeValueUpdate(ObjectInstanceHandle theObject,
                                AttributeHandleSet const& theAttributes,
                                VariableLengthData const& theUserSuppliedTag) throw(FederateInternalError) override;

    // 6.21
    virtual void
    turnUpdatesOnForObjectInstance(ObjectInstanceHandle theObject,
                                   AttributeHandleSet const& theAttributes) throw(FederateInternalError) override;

    virtual void
    turnUpdatesOnForObjectInstance(ObjectInstanceHandle theObject,
                                   AttributeHandleSet const& theAttributes,
                                   std::wstring const& updateRateDesignator) throw(FederateInternalError) override;

    // 6.22
    virtual void
    turnUpdatesOffForObjectInstance(ObjectInstanceHandle theObject,
                                    AttributeHandleSet const& theAttributes) throw(FederateInternalError) override;

    // 6.24
    virtual void confirmAttributeTransportationTypeChange(
        ObjectInstanceHandle theObject,
        AttributeHandleSet theAttributes,
        TransportationType theTransportation) throw(FederateInternalError) override;

    // 6.26
    virtual void
    reportAttributeTransportationType(ObjectInstanceHandle theObject,
                                      AttributeHandle theAttribute,
                                      TransportationType theTransportation) throw(FederateInternalError) override;

    // 6.28
    virtual void confirmInteractionTransportationTypeChange(
        InteractionClassHandle theInteraction,
        TransportationType theTransportation) throw(FederateInternalError) override;

    // 6.30
    virtual void
    reportInteractionTransportationType(FederateHandle federateHandle,
                                        InteractionClassHandle theInteraction,
                                        TransportationType theTransportation) throw(FederateInternalError) override;

    ///////////////////////////////////
    // Ownership Management Services //
    ///////////////////////////////////

    // 7.4
    virtual void requestAttributeOwnershipAssumption(
        ObjectInstanceHandle theObject,
        AttributeHandleSet const& offeredAttributes,
        VariableLengthData const& theUserSuppliedTag) throw(FederateInternalError) override;

    // 7.5
    virtual void
    requestDivestitureConfirmation(ObjectInstanceHandle theObject,
                                   AttributeHandleSet const& releasedAttributes) throw(FederateInternalError) override;

    // 7.7
    virtual void attributeOwnershipAcquisitionNotification(
        ObjectInstanceHandle theObject,
        AttributeHandleSet const& securedAttributes,
        VariableLengthData const& theUserSuppliedTag) throw(FederateInternalError) override;

    // 7.10
    virtual void
    attributeOwnershipUnavailable(ObjectInstanceHandle theObject,
                                  AttributeHandleSet const& theAttributes) throw(FederateInternalError) override;

    // 7.11
    virtual void requestAttributeOwnershipRelease(
        ObjectInstanceHandle theObject,
        AttributeHandleSet const& candidateAttributes,
        VariableLengthData const& theUserSuppliedTag) throw(FederateInternalError) override;

    // 7.16
    virtual void confirmAttributeOwnershipAcquisitionCancellation(
        ObjectInstanceHandle theObject, AttributeHandleSet const& theAttributes) throw(FederateInternalError) override;

    // 7.18
    virtual void informAttributeOwnership(ObjectInstanceHandle theObject,
                                          AttributeHandle theAttribute,
                                          FederateHandle theOwner) throw(FederateInternalError) override;

    virtual void attributeIsNotOwned(ObjectInstanceHandle theObject,
                                     AttributeHandle theAttribute) throw(FederateInternalError) override;

    virtual void attributeIsOwnedByRTI(ObjectInstanceHandle theObject,
                                       AttributeHandle theAttribute) throw(FederateInternalError) override;

    //////////////////////////////
    // Time Management Services //
    //////////////////////////////

    // 8.3
    virtual void timeRegulationEnabled(LogicalTime const& theFederateTime) throw(FederateInternalError) override;

    // 8.6
    virtual void timeConstrainedEnabled(LogicalTime const& theFederateTime) throw(FederateInternalError) override;

    // 8.13
    virtual void timeAdvanceGrant(LogicalTime const& theTime) throw(FederateInternalError) override;

    // 8.22
    virtual void requestRetraction(MessageRetractionHandle theHandle) throw(FederateInternalError) override;

private:
    RTIambassador& my_ambassador;

    const std::wstring& my_federation_name;
    const std::wstring& my_federate_name;
    
    const bool my_auto_mode;
    const int my_report_period;
    const int my_report_style;

    std::map<std::wstring, VariableLengthData> show(const AttributeHandleValueMap& map,
                                                    const ObjectInstanceHandle object_instance);
    std::map<std::wstring, VariableLengthData> show(const ParameterHandleValueMap& map,
                                                    const InteractionClassHandle interaction_class);

    void displayData();

    std::map<ObjectClassHandle, AttributeHandleSet> my_attributes_of_interest;

    std::map<ObjectInstanceHandle, AttributeHandleValueMap> my_data;

    ObjectInstanceHandle my_federation;
    std::vector<ObjectInstanceHandle> my_federates;
};

#endif // MOMFEDERATEAMBASSADOR_H
