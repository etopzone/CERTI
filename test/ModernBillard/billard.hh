#ifndef BILLARD_H
#define BILLARD_H

#include <RTI/NullFederateAmbassador.h>
#include <RTI/RTI1516.h>

#include <RTI/RTI1516fedTime.h>

#include <memory>
#include <vector>

#include "ball.h"

using namespace rti1516e;

class Billard : public NullFederateAmbassador {
public:
    Billard(const std::wstring& federation_name, const std::wstring& federate_name, bool& loop_state);

    virtual ~Billard();

    bool isCreator() const;
    bool hasSynchronizationPending(const std::wstring& label);
    void enableCollisions();

    void createOrJoin();

    void register_sync_point(const std::wstring& label);
    void synchronize(const std::wstring& label);

    void publishAndSubscribe();

    void enableTimeRegulation();

    void timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) throw(FederateInternalError) override;

    void timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) throw(FederateInternalError) override;

    void tick();

    void init();

    void declare();

    void step();

    void resignAndDelete();

    void sendCollision(const Ball& other, const rti1516e::LogicalTime& time);

    void sendNewPosition(const rti1516e::LogicalTime& time);

    void announceSynchronizationPoint(
        const std::wstring& label,
        const rti1516e::VariableLengthData& theUserSuppliedTag) throw(FederateInternalError) override;

    void waitForAnnounce(const std::wstring& label);

    virtual void federationSynchronized(const std::wstring& label,
                                        const FederateHandleSet& failedToSyncSet) throw(FederateInternalError) override;

    void waitForSynchronization(const std::wstring& label);

    virtual void timeAdvanceGrant(const rti1516e::LogicalTime& theTime) throw(FederateInternalError) override;

    void waitForTimeAdvanceGrant();

    void discoverObjectInstance(rti1516e::ObjectInstanceHandle theObject,
                                rti1516e::ObjectClassHandle theObjectClass,
                                const std::wstring& theObjectInstanceName) throw(FederateInternalError) override;

    void discoverObjectInstance(rti1516e::ObjectInstanceHandle theObject,
                                rti1516e::ObjectClassHandle theObjectClass,
                                const std::wstring& theObjectInstanceName,
                                rti1516e::FederateHandle producingFederate) throw(FederateInternalError) override;

    void receiveInteraction(rti1516e::InteractionClassHandle theInteraction,
                            const ParameterHandleValueMap& theParameterValues,
                            const rti1516e::VariableLengthData& theUserSuppliedTag,
                            rti1516e::OrderType sentOrder,
                            rti1516e::TransportationType theType,
                            const rti1516e::LogicalTime& theTime,
                            rti1516e::OrderType receivedOrder,
                            rti1516e::MessageRetractionHandle theHandle,
                            rti1516e::SupplementalReceiveInfo theReceiveInfo) throw(FederateInternalError) override;

    void receiveInteraction(rti1516e::InteractionClassHandle theInteraction,
                            const ParameterHandleValueMap& theParameterValues,
                            const rti1516e::VariableLengthData& theUserSuppliedTag,
                            rti1516e::OrderType sentOrder,
                            rti1516e::TransportationType theType,
                            const rti1516e::LogicalTime& theTime,
                            rti1516e::OrderType receivedOrder,
                            rti1516e::SupplementalReceiveInfo theReceiveInfo) throw(FederateInternalError) override;

    void receiveInteraction(rti1516e::InteractionClassHandle theInteraction,
                            const ParameterHandleValueMap& theParameterValues,
                            const rti1516e::VariableLengthData& theUserSuppliedTag,
                            rti1516e::OrderType sentOrder,
                            rti1516e::TransportationType theType,
                            rti1516e::SupplementalReceiveInfo theReceiveInfo) throw(FederateInternalError) override;

    void reflectAttributeValues(rti1516e::ObjectInstanceHandle theObject,
                                const AttributeHandleValueMap& theAttributeValues,
                                const rti1516e::VariableLengthData& theUserSuppliedTag,
                                rti1516e::OrderType sentOrder,
                                rti1516e::TransportationType theType,
                                const rti1516e::LogicalTime& theTime,
                                rti1516e::OrderType receivedOrder,
                                rti1516e::MessageRetractionHandle theHandle,
                                rti1516e::SupplementalReflectInfo theReflectInfo) throw(FederateInternalError) override;

    void reflectAttributeValues(rti1516e::ObjectInstanceHandle theObject,
                                const AttributeHandleValueMap& theAttributeValues,
                                const rti1516e::VariableLengthData& theUserSuppliedTag,
                                rti1516e::OrderType sentOrder,
                                rti1516e::TransportationType theType,
                                const rti1516e::LogicalTime& theTime,
                                rti1516e::OrderType receivedOrder,
                                rti1516e::SupplementalReflectInfo theReflectInfo) throw(FederateInternalError) override;

    void reflectAttributeValues(rti1516e::ObjectInstanceHandle theObject,
                                const AttributeHandleValueMap& theAttributeValues,
                                const rti1516e::VariableLengthData& theUserSuppliedTag,
                                rti1516e::OrderType sentOrder,
                                rti1516e::TransportationType theType,
                                rti1516e::SupplementalReflectInfo theReflectInfo) throw(FederateInternalError) override;

private:
    void show_sync_points() const;

    std::unique_ptr<rti1516e::RTIambassador> my_ambassador{rti1516e::RTIambassadorFactory().createRTIambassador()};

    const std::wstring& my_federation_name;
    const std::wstring& my_federate_name;

    bool has_created{false};
    bool has_collision_enabled{false};

    rti1516e::FederateHandle my_handle;

    std::vector<std::wstring> my_synchronization_points;

    Board my_board{};

    Ball my_ball{};
    std::vector<Ball> my_other_balls{};

    bool my_is_time_constrained{false};
    bool my_is_time_regulated{false};

    bool my_time_granted{false};

    RTI1516fedTimeInterval my_time_interval{1.0};

    RTI1516fedTime my_local_time{0.0};
    //     RTI1516fedTime my_last_granted_time{0.0};
};

#endif // BILLARD_H
