#include "billard.hh"

#include "MessageBuffer.hh"

#include <algorithm>
#include <iostream>

//#define TRACE

#ifdef TRACE
auto* out = &std::wcout;
#else
decltype(std::wcout)* out = nullptr;
#endif

#define debug() if(out) *out

using std::wcout;
using std::endl;

Billard::Billard(RTIambassador& ambassador, const std::wstring& federation_name, const std::wstring& federate_name)
    : my_ambassador{ambassador}, my_federation_name{federation_name}, my_federate_name{federate_name}
{
    wcout << __func__ << endl;
    my_ambassador.connect(*this, HLA_EVOKED);
}

bool Billard::isCreator() const
{
    return has_created;
}

bool Billard::hasSynchronizationPending(const std::wstring& label)
{
    auto find_it = std::find(begin(my_synchronization_points), end(my_synchronization_points), label);

    return find_it != end(my_synchronization_points);
}

void Billard::enableCollisions()
{
    has_collision_enabled = true;
}

void Billard::createOrJoin()
{
    wcout << __func__ << endl;
    try {
        my_ambassador.createFederationExecution(my_federation_name, L"Base.xml");
        has_created = true;
        wcout << "Created federation\n";
    }
    catch (FederationExecutionAlreadyExists& e) {
    }

    std::vector<std::wstring> modules;
    if (has_created) {
        modules.emplace_back(L"Create.xml");
    }
    else {
        modules.emplace_back(L"Join.xml");
    }

    my_handle = my_ambassador.joinFederationExecution(my_federate_name, L"ModernBillard", my_federation_name, modules);
    wcout << "Joined federation\n";
}

void Billard::pause(const std::wstring& label)
{
    wcout << __func__ << ", label:" << label << endl;

    std::wstring tag{L""};
    my_ambassador.registerFederationSynchronizationPoint(label, {tag.c_str(), tag.size()});

    show_sync_points();
}

void Billard::synchronize(const std::wstring& label)
{
    wcout << __func__ << ", label:" << label << endl;

    waitForAnnounce(label);

    my_ambassador.synchronizationPointAchieved(label, true);

    show_sync_points();
}

void Billard::publishAndSubscribe()
{
    wcout << __func__ << endl;

    auto ball_handle = my_ambassador.getObjectClassHandle(L"Ball");
    wcout << "Ball: " << ball_handle << endl;

    AttributeHandleSet attributes;
    attributes.insert(my_ambassador.getAttributeHandle(ball_handle, L"PositionX"));
    attributes.insert(my_ambassador.getAttributeHandle(ball_handle, L"PositionY"));

    my_ambassador.subscribeObjectClassAttributes(ball_handle, attributes);
    my_ambassador.publishObjectClassAttributes(ball_handle, attributes);

    if (has_collision_enabled) {
        auto collision_handle = my_ambassador.getInteractionClassHandle(L"Collision");
        my_ambassador.subscribeInteractionClass(collision_handle);
        my_ambassador.publishInteractionClass(collision_handle);
        wcout << "Collision: " << collision_handle << endl;
    }
}

void Billard::enableTimeRegulation()
{
    wcout << __func__ << endl;
    my_ambassador.enableTimeConstrained();
}

void Billard::tick()
{
#ifdef TRACE
    static tick_count{0};
    ++tick_count;
    wcout << __func__ << tick_count << '\n';
#endif

    my_ambassador.evokeCallback(1.0);
}

void Billard::init()
{
    wcout << __func__ << endl;
    my_ball.init(*reinterpret_cast<const int*>(my_handle.encode().data()));
}

void Billard::declare()
{
    wcout << __func__ << endl;
    auto ball_handle = my_ambassador.registerObjectInstance(my_ambassador.getObjectClassHandle(L"Ball"), my_federate_name + L"_Ball");
    my_ball.setHandle(ball_handle);
    wcout << "\tRegistered ball with handle " << ball_handle << endl;
}

void Billard::step()
{
    debug() << __func__ << endl;

    debug() << "time at start is " << my_local_time.toString() << endl;

    my_ambassador.queryLogicalTime(my_local_time);

    debug() << "after query, time is " << my_local_time.toString() << endl;

    auto time_aux = RTI1516fedTime{my_local_time};
    time_aux += my_time_interval;

    debug() << "request advance to " << time_aux.toString() << endl;

    my_ambassador.timeAdvanceRequest(time_aux);

    if (has_collision_enabled) {
        for (auto& ball : my_other_balls) {
            ball.setInactive();
        }
    }

    waitForTimeAdvanceGrant();

    debug() << "after wait for grant, time is " << my_local_time.toString() << endl;

    auto next_step = RTI1516fedTime{my_local_time};
    next_step += my_time_interval;

    // erase screen

    if (has_collision_enabled) {
        for (auto& ball : my_other_balls) {
            if (ball.getHandle().isValid() && ball.isActive()) {
                if (my_ball.checkCollisionWith(ball)) {
                    debug() << "Collision between my ball and ball " << ball.getHandle() << endl;

                    sendCollision(ball, next_step);
                }
            }
        }
    }
    my_ball.checkCollisionAndBounceWith(my_board);

    my_ball.moveWithCurrentInertia();

    sendNewPosition(next_step);
    
    debug() << "New ball position: x=" << my_ball.getX() << ", y=" << my_ball.getY() << std::endl;
}

void Billard::sendCollision(const Ball& other, const LogicalTime& time)
{
    static const auto collision_handle = my_ambassador.getInteractionClassHandle(L"Collision");
    static const auto param_ball_handle = my_ambassador.getParameterHandle(collision_handle, L"Ball");
    static const auto param_dx_handle = my_ambassador.getParameterHandle(collision_handle, L"DX");
    static const auto param_dy_handle = my_ambassador.getParameterHandle(collision_handle, L"DY");

    libhla::MessageBuffer buffer;

    ParameterHandleValueMap parameters;

    int handle = *reinterpret_cast<const int*>(other.getHandle().encode().data());

    buffer.reset();
    buffer.write_int32(handle);
    buffer.updateReservedBytes();
    parameters[param_ball_handle] = VariableLengthData(static_cast<char*>(buffer(0)), buffer.size());

    buffer.reset();
    buffer.write_double(my_ball.getDX());
    buffer.updateReservedBytes();
    parameters[param_dx_handle] = VariableLengthData(static_cast<char*>(buffer(0)), buffer.size());

    buffer.reset();
    buffer.write_double(my_ball.getDY());
    buffer.updateReservedBytes();
    parameters[param_dy_handle] = VariableLengthData(static_cast<char*>(buffer(0)), buffer.size());

    std::wstring tag{L""};
    my_ambassador.sendInteraction(collision_handle, parameters, {tag.c_str(), tag.size()}, time);
}

void Billard::sendNewPosition(const LogicalTime& time)
{
    static const auto ball_handle = my_ambassador.getObjectClassHandle(L"Ball");
    static const auto attribute_pos_x_handle = my_ambassador.getAttributeHandle(ball_handle, L"PositionX");
    static const auto attribute_pos_y_handle = my_ambassador.getAttributeHandle(ball_handle, L"PositionY");

    libhla::MessageBuffer buffer;

    AttributeHandleValueMap attributes;

    buffer.reset();
    buffer.write_double(my_ball.getX());
    buffer.updateReservedBytes();
    attributes[attribute_pos_x_handle] = VariableLengthData(static_cast<char*>(buffer(0)), buffer.size());

    buffer.reset();
    buffer.write_double(my_ball.getY());
    buffer.updateReservedBytes();
    attributes[attribute_pos_x_handle] = VariableLengthData(static_cast<char*>(buffer(0)), buffer.size());

    std::wstring tag{L""};
    my_ambassador.updateAttributeValues(my_ball.getHandle(), attributes, {tag.c_str(), tag.size()}, time);
}

void Billard::announceSynchronizationPoint(
    const std::wstring& label, const VariableLengthData& /*theUserSuppliedTag*/) throw(FederateInternalError)
{
    wcout << __func__ << ", label:" << label << endl;
    my_synchronization_points.push_back(label);
}

void Billard::waitForAnnounce(const std::wstring& label)
{
    wcout << __func__ << ", label:" << label << endl;

    show_sync_points();

    while (not hasSynchronizationPending(label)) {
        tick();
    }

    show_sync_points();
}

void Billard::federationSynchronized(const std::wstring& label,
                                     const FederateHandleSet& /*failedToSyncSet*/) throw(FederateInternalError)
{
    wcout << __func__ << ", label:" << label << endl;
    wcout << L"Federation synchronized on label " << label << endl;
    auto find_it = std::find(begin(my_synchronization_points), end(my_synchronization_points), label);
    if (find_it == end(my_synchronization_points)) {
        throw FederateInternalError(L"Synchronization point <" + label + L"> achieved but never announced");
    }
    my_synchronization_points.erase(find_it);
}

void Billard::waitForSynchronization(const std::wstring& label)
{
    wcout << __func__ << ", label:" << label << endl;

    show_sync_points();

    while (hasSynchronizationPending(label)) {
        tick();
    }

    show_sync_points();
}

void Billard::timeAdvanceGrant(const LogicalTime& theTime) throw(FederateInternalError)
{
    my_local_time = theTime;
}

void Billard::waitForTimeAdvanceGrant()
{
    auto last_time = my_local_time;
    while (last_time == my_local_time) {
        tick();
    }
}

void Billard::discoverObjectInstance(rti1516e::ObjectInstanceHandle theObject,
                                     rti1516e::ObjectClassHandle /*theObjectClass*/,
                                     const std::wstring& /*theObjectInstanceName*/) throw(FederateInternalError)
{
    wcout << "########        " << __func__ << " 1, theObject=" << theObject << endl;
}

void Billard::discoverObjectInstance(rti1516e::ObjectInstanceHandle theObject,
                                     rti1516e::ObjectClassHandle /*theObjectClass*/,
                                     const std::wstring& /*theObjectInstanceName*/,
                                     rti1516e::FederateHandle /*producingFederate*/) throw(FederateInternalError)
{
    wcout << "########        " << __func__ << " 2, theObject=" << theObject << endl;
}

void Billard::receiveInteraction(rti1516e::InteractionClassHandle theInteraction,
                                 const ParameterHandleValueMap& /*theParameterValues*/,
                                 const rti1516e::VariableLengthData& /*theUserSuppliedTag*/,
                                 rti1516e::OrderType /*sentOrder*/,
                                 rti1516e::TransportationType /*theType*/,
                                 const rti1516e::LogicalTime& /*theTime*/,
                                 rti1516e::OrderType /*receivedOrder*/,
                                 rti1516e::MessageRetractionHandle /*theHandle*/,
                                 rti1516e::SupplementalReceiveInfo /*theReceiveInfo*/) throw(FederateInternalError)
{
    wcout << "########        " << __func__ << " 1, theInteraction=" << theInteraction << endl;
}

void Billard::receiveInteraction(rti1516e::InteractionClassHandle theInteraction,
                                 const ParameterHandleValueMap& /*theParameterValues*/,
                                 const rti1516e::VariableLengthData& /*theUserSuppliedTag*/,
                                 rti1516e::OrderType /*sentOrder*/,
                                 rti1516e::TransportationType /*theType*/,
                                 const rti1516e::LogicalTime& /*theTime*/,
                                 rti1516e::OrderType /*receivedOrder*/,
                                 rti1516e::SupplementalReceiveInfo /*theReceiveInfo*/) throw(FederateInternalError)
{
    wcout << "########        " << __func__ << " 2, theInteraction=" << theInteraction << endl;
}

void Billard::receiveInteraction(rti1516e::InteractionClassHandle theInteraction,
                                 const ParameterHandleValueMap& /*theParameterValues*/,
                                 const rti1516e::VariableLengthData& /*theUserSuppliedTag*/,
                                 rti1516e::OrderType /*sentOrder*/,
                                 rti1516e::TransportationType /*theType*/,
                                 rti1516e::SupplementalReceiveInfo /*theReceiveInfo*/) throw(FederateInternalError)
{
    wcout << "########        " << __func__ << " 3, theInteraction=" << theInteraction << endl;
}

void Billard::reflectAttributeValues(rti1516e::ObjectInstanceHandle theObject,
                                     const AttributeHandleValueMap& /*theAttributeValues*/,
                                     const rti1516e::VariableLengthData& /*theUserSuppliedTag*/,
                                     rti1516e::OrderType /*sentOrder*/,
                                     rti1516e::TransportationType /*theType*/,
                                     const rti1516e::LogicalTime& /*theTime*/,
                                     rti1516e::OrderType /*receivedOrder*/,
                                     rti1516e::MessageRetractionHandle /*theHandle*/,
                                     rti1516e::SupplementalReflectInfo /*theReflectInfo*/) throw(FederateInternalError)
{
    wcout << "########        " << __func__ << " 1, theObject=" << theObject << endl;
}

void Billard::reflectAttributeValues(rti1516e::ObjectInstanceHandle theObject,
                                     const AttributeHandleValueMap& /*theAttributeValues*/,
                                     const rti1516e::VariableLengthData& /*theUserSuppliedTag*/,
                                     rti1516e::OrderType /*sentOrder*/,
                                     rti1516e::TransportationType /*theType*/,
                                     const rti1516e::LogicalTime& /*theTime*/,
                                     rti1516e::OrderType /*receivedOrder*/,
                                     rti1516e::SupplementalReflectInfo /*theReflectInfo*/) throw(FederateInternalError)
{
    wcout << "########        " << __func__ << " 2, theObject=" << theObject << endl;
}

void Billard::reflectAttributeValues(rti1516e::ObjectInstanceHandle theObject,
                                     const AttributeHandleValueMap& /*theAttributeValues*/,
                                     const rti1516e::VariableLengthData& /*theUserSuppliedTag*/,
                                     rti1516e::OrderType /*sentOrder*/,
                                     rti1516e::TransportationType /*theType*/,
                                     rti1516e::SupplementalReflectInfo /*theReflectInfo*/) throw(FederateInternalError)
{
    wcout << "########        " << __func__ << " 3, theObject=" << theObject << endl;
}

void Billard::show_sync_points() const
{
#ifdef TRACE
    wcout << "  Current synchronization points: {\n";
    for (const auto& point : my_synchronization_points) {
        wcout << "    - " << point << '\n';
    }
    wcout << "  }" << endl;
#endif
}
