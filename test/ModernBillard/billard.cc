#include "billard.hh"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>

#include "MessageBuffer.hh"

#ifndef X_DISPLAY_MISSING
#include "../libgraphc/graph_c.hh"
#endif

// #define TRACE

#ifdef TRACE
auto* out = &std::wcout;
#else
decltype(std::wcout)* out = nullptr;
#endif

#define debug()                                                                                                        \
    if (out)                                                                                                           \
    *out

using std::wcout;
using std::endl;

std::wostream& operator<<(std::wostream& os, const VariableLengthData& v)
{
    os << "{" << v.size() << "}";

    if (v.size() != 0) {
        auto prev = os.fill('0');
        os << "0" << std::hex;

        for (auto i(0u); i < v.size(); ++i) {
            if (i == 0) {
                os << "x";
            }
            else if (i % 4 == 0) {
                os << ":";
            }
            os << std::setw(2) << static_cast<const uint8_t*>(v.data())[i];
        }

        os << std::dec;
        os.fill(prev);
    }
    return os;
}

std::wostream& operator<<(std::wostream& os, const AttributeHandleValueMap& v)
{
    os << "m{ ";
    for (const auto& element : v) {
        os << element.first << " = " << element.second << ", ";
    }
    os << "} ";
    return os;
}

Billard::Billard(const std::wstring& federation_name, const std::wstring& federate_name, bool& loop_state)
    : my_federation_name(federation_name), my_federate_name(federate_name)
{
    debug() << __func__ << endl;
    my_ambassador->connect(*this, HLA_EVOKED);

    createOrJoin();

    if (isCreator()) {
        register_sync_point(L"Init");

        wcout << "Press ENTER when all federates have joined" << endl;
        getchar();
    }
    else {
        enableCollisions();

        register_sync_point(L"NotAlone");
        synchronize(L"NotAlone");
    }

    synchronize(L"Init");
    waitForSynchronization(L"Init");

    if (isCreator() && hasSynchronizationPending(L"NotAlone")) {
        enableCollisions();

        synchronize(L"NotAlone");
        waitForSynchronization(L"NotAlone");
    }

    publishAndSubscribe();

    enableTimeRegulation();

    tick();

    init();

    declare();

    if (isCreator()) {
        register_sync_point(L"Start");
    }

    synchronize(L"Start");
    waitForSynchronization(L"Start");

    int step_count{0};
    auto time_point = std::chrono::system_clock::now();
    for (loop_state = true; loop_state; ++step_count) {
        step();

        if (step_count % 500 == 0) {
            auto now = std::chrono::system_clock::now();
            wcout << "steps per second: "
                  << 5.0e11 / std::chrono::duration_cast<std::chrono::nanoseconds>(now - time_point).count() << endl;
            time_point = now;
        }
    }
}

Billard::~Billard()
{
    resignAndDelete();
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
    debug() << __func__ << endl;
    try {
        my_ambassador->createFederationExecution(my_federation_name, L"Base.xml");
        has_created = true;
        debug() << "Created federation\n";
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

    my_handle = my_ambassador->joinFederationExecution(my_federate_name, L"ModernBillard", my_federation_name, modules);
    debug() << "Joined federation\n";
}

void Billard::register_sync_point(const std::wstring& label)
{
    debug() << __func__ << ", label:" << label << endl;

    std::wstring tag{L""};
    my_ambassador->registerFederationSynchronizationPoint(label, {tag.c_str(), tag.size()});

    show_sync_points();
}

void Billard::synchronize(const std::wstring& label)
{
    debug() << __func__ << ", label:" << label << endl;

    waitForAnnounce(label);

    my_ambassador->synchronizationPointAchieved(label, true);

    show_sync_points();
}

void Billard::publishAndSubscribe()
{
    debug() << __func__ << endl;

    auto ball_handle = my_ambassador->getObjectClassHandle(L"Ball");
    debug() << "Ball: " << ball_handle << endl;

    AttributeHandleSet attributes;
    attributes.insert(my_ambassador->getAttributeHandle(ball_handle, L"PositionX"));
    attributes.insert(my_ambassador->getAttributeHandle(ball_handle, L"PositionY"));

    my_ambassador->subscribeObjectClassAttributes(ball_handle, attributes);
    my_ambassador->publishObjectClassAttributes(ball_handle, attributes);

    if (has_collision_enabled) {
        auto collision_handle = my_ambassador->getInteractionClassHandle(L"Collision");
        my_ambassador->subscribeInteractionClass(collision_handle);
        my_ambassador->publishInteractionClass(collision_handle);
        debug() << "Collision: " << collision_handle << endl;
    }
}

void Billard::enableTimeRegulation()
{
    debug() << __func__ << endl;
    my_ambassador->enableTimeConstrained();

    while (!my_is_time_constrained) {
        tick();
    }

    my_ambassador->enableTimeRegulation(my_time_interval);

    while (!my_is_time_regulated) {
        tick();
    }
}

void Billard::timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) throw(FederateInternalError)
{
    debug() << __func__ << ", time=" << theFederateTime.toString() << endl;
    my_is_time_constrained = true;
}

void Billard::timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) throw(FederateInternalError)
{
    debug() << __func__ << ", time=" << theFederateTime.toString() << endl;
    my_is_time_regulated = true;
}

void Billard::tick()
{
#ifdef TRACE
    static unsigned int tick_count{0};
    ++tick_count;
    debug() << __func__ << tick_count << '\n';
#endif

    my_ambassador->evokeCallback(1.0);
}

void Billard::init()
{
    debug() << __func__ << endl;

    auto seed = std::stoi(my_handle.toString().substr(std::string("FederateHandle_").size()));

    debug() << "seed: " << seed << endl;

#ifndef X_DISPLAY_MISSING
    InitGraphe(400, 25 + (seed - 1) * 120, 500, 100);
#endif

    my_ball.init(seed);
}

void Billard::declare()
{
    debug() << __func__ << endl;
    auto ball_handle = my_ambassador->registerObjectInstance(my_ambassador->getObjectClassHandle(L"Ball"),
                                                             my_federate_name + L"_Ball");
    my_ball.setHandle(ball_handle);
    debug() << "Registered my ball with handle " << ball_handle << endl;
}

void Billard::step()
{
#ifdef TRACE
    static unsigned int step_count{0};
    ++step_count;
    debug() << __func__ << step_count << '\n';
#endif

    my_time_granted = false;

    debug() << "time at start is " << my_local_time.toString() << endl;

    my_ambassador->queryLogicalTime(my_local_time);

    debug() << "after query, time is " << my_local_time.toString() << endl;

    auto time_aux = RTI1516fedTime{my_local_time};
    time_aux += my_time_interval;

    debug() << "request advance to " << time_aux.toString() << endl;

    my_time_granted = false;
    my_ambassador->timeAdvanceRequest(time_aux);

    for (auto& ball : my_other_balls) {
        ball.setInactive();
    }

    if (has_collision_enabled) {
        for (auto& ball : my_other_balls) {
            ball.setInactive();
        }
    }

    waitForTimeAdvanceGrant();

    debug() << "after wait for grant, time is " << my_local_time.toString() << endl;

    my_ball.erase();

    auto next_step = RTI1516fedTime{my_local_time};
    next_step += my_time_interval;

    // erase screen

    if (has_collision_enabled) {
        for (auto& ball : my_other_balls) {
            if (ball.getHandle().isValid() && ball.isActive()) {
                if (my_ball.checkCollisionWith(ball)) {
                    debug() << "Collision between my ball and ball " << ball.getHandle() << endl;

                    sendCollision(ball, next_step);

                    my_ball.bounceAgainst(ball);
                }
            }

            ball.display();
        }
    }
    my_ball.checkCollisionAndBounceWith(my_board);

    my_ball.moveWithCurrentInertia();

    my_ball.display();

    sendNewPosition(next_step);

    debug() << "[" << my_local_time.toString() << "] New ball position: x=" << my_ball.getX()
            << ", y=" << my_ball.getY() << std::endl;
}

void Billard::resignAndDelete()
{
    my_ambassador->resignFederationExecution(CANCEL_THEN_DELETE_THEN_DIVEST);

    try {
        my_ambassador->destroyFederationExecution(my_federation_name);
    }
    catch (FederatesCurrentlyJoined& e) {
        // We are not the last one, die and let the last one destroy the federation
    }
}

void Billard::sendCollision(const Ball& other, const LogicalTime& time)
{
    static const auto collision_handle = my_ambassador->getInteractionClassHandle(L"Collision");
    static const auto param_ball_handle = my_ambassador->getParameterHandle(collision_handle, L"Ball");
    static const auto param_dx_handle = my_ambassador->getParameterHandle(collision_handle, L"DX");
    static const auto param_dy_handle = my_ambassador->getParameterHandle(collision_handle, L"DY");

    libhla::MessageBuffer buffer;

    ParameterHandleValueMap parameters;

    parameters[param_ball_handle] = other.getHandle().encode();

    buffer.reset();
    buffer.write_double(my_ball.getDX());
    buffer.updateReservedBytes();
    parameters[param_dx_handle] = VariableLengthData(static_cast<char*>(buffer(0)), buffer.size());

    buffer.reset();
    buffer.write_double(my_ball.getDY());
    buffer.updateReservedBytes();
    parameters[param_dy_handle] = VariableLengthData(static_cast<char*>(buffer(0)), buffer.size());

    std::wstring tag{L""};
    my_ambassador->sendInteraction(collision_handle, parameters, {tag.c_str(), tag.size()}, time);
}

void Billard::sendNewPosition(const LogicalTime& time)
{
    static const auto ball_handle = my_ambassador->getObjectClassHandle(L"Ball");
    static const auto attribute_pos_x_handle = my_ambassador->getAttributeHandle(ball_handle, L"PositionX");
    static const auto attribute_pos_y_handle = my_ambassador->getAttributeHandle(ball_handle, L"PositionY");

    libhla::MessageBuffer buffer;

    AttributeHandleValueMap attributes;

    buffer.reset();
    buffer.write_double(my_ball.getX());
    buffer.updateReservedBytes();
    attributes[attribute_pos_x_handle] = VariableLengthData(static_cast<char*>(buffer(0)), buffer.size());

    buffer.reset();
    buffer.write_double(my_ball.getY());
    buffer.updateReservedBytes();
    attributes[attribute_pos_y_handle] = VariableLengthData(static_cast<char*>(buffer(0)), buffer.size());

    debug() << "SEND::" << attributes << endl;

    std::wstring tag{L""};
    my_ambassador->updateAttributeValues(my_ball.getHandle(), attributes, {tag.c_str(), tag.size()}, time);
}

void Billard::announceSynchronizationPoint(
    const std::wstring& label, const VariableLengthData& /*theUserSuppliedTag*/) throw(FederateInternalError)
{
    debug() << __func__ << ", label:" << label << endl;
    my_synchronization_points.push_back(label);
}

void Billard::waitForAnnounce(const std::wstring& label)
{
    debug() << __func__ << ", label:" << label << endl;

    show_sync_points();

    while (!hasSynchronizationPending(label)) {
        tick();
    }

    show_sync_points();
}

void Billard::federationSynchronized(const std::wstring& label,
                                     const FederateHandleSet& /*failedToSyncSet*/) throw(FederateInternalError)
{
    debug() << __func__ << ", label:" << label << endl;
    debug() << L"Federation synchronized on label " << label << endl;
    auto find_it = std::find(begin(my_synchronization_points), end(my_synchronization_points), label);
    if (find_it == end(my_synchronization_points)) {
        throw FederateInternalError(L"Synchronization point <" + label + L"> achieved but never announced");
    }
    my_synchronization_points.erase(find_it);
}

void Billard::waitForSynchronization(const std::wstring& label)
{
    debug() << __func__ << ", label:" << label << endl;

    show_sync_points();

    while (hasSynchronizationPending(label)) {
        tick();
    }

    show_sync_points();
}

void Billard::timeAdvanceGrant(const LogicalTime& theTime) throw(FederateInternalError)
{
    my_local_time = theTime;
    my_time_granted = true;
}

void Billard::waitForTimeAdvanceGrant()
{
    while (!my_time_granted) {
        tick();
    }
}

void Billard::discoverObjectInstance(rti1516e::ObjectInstanceHandle theObject,
                                     rti1516e::ObjectClassHandle theObjectClass,
                                     const std::wstring& /*theObjectInstanceName*/) throw(FederateInternalError)
{
    debug() << __func__ << ", theObject=" << theObject << endl;

    if (theObjectClass == my_ambassador->getObjectClassHandle(L"Ball")) {
        debug() << "Add other ball, handle=" << theObject << endl;
        my_other_balls.emplace_back();
        my_other_balls.back().setHandle(theObject);
    }
}

void Billard::discoverObjectInstance(rti1516e::ObjectInstanceHandle theObject,
                                     rti1516e::ObjectClassHandle /*theObjectClass*/,
                                     const std::wstring& /*theObjectInstanceName*/,
                                     rti1516e::FederateHandle /*producingFederate*/) throw(FederateInternalError)
{
    debug() << "########        " << __func__ << " 2, theObject=" << theObject << endl;
    getchar();
}

void Billard::receiveInteraction(rti1516e::InteractionClassHandle theInteraction,
                                 const ParameterHandleValueMap& theParameterValues,
                                 const rti1516e::VariableLengthData& /*theUserSuppliedTag*/,
                                 rti1516e::OrderType /*sentOrder*/,
                                 rti1516e::TransportationType /*theType*/,
                                 const rti1516e::LogicalTime& /*theTime*/,
                                 rti1516e::OrderType /*receivedOrder*/,
                                 rti1516e::MessageRetractionHandle /*theHandle*/,
                                 rti1516e::SupplementalReceiveInfo /*theReceiveInfo*/) throw(FederateInternalError)
{
    debug() << __func__ << ", theInteraction=" << theInteraction << endl;

    static const auto collision_handle = my_ambassador->getInteractionClassHandle(L"Collision");
    static const auto param_ball_handle = my_ambassador->getParameterHandle(collision_handle, L"Ball");
    static const auto param_dx_handle = my_ambassador->getParameterHandle(collision_handle, L"DX");
    static const auto param_dy_handle = my_ambassador->getParameterHandle(collision_handle, L"DY");

    if (theInteraction == collision_handle) {
        debug() << "Collision" << endl;

        try {
            auto handle = theParameterValues.at(param_ball_handle);
            if (std::memcmp(handle.data(), my_ball.getHandle().encode().data(), handle.size()) == 0) {
                libhla::MessageBuffer mb;

                auto param_dx = theParameterValues.at(param_dx_handle);

                mb.resize(param_dx.size());
                mb.reset();
                std::memcpy(static_cast<char*>(mb(0)), param_dx.data(), param_dx.size());
                mb.assumeSizeFromReservedBytes();

                my_ball.setDX(mb.read_double());

                auto param_dy = theParameterValues.at(param_dx_handle);

                mb.resize(param_dy.size());
                mb.reset();
                std::memcpy(static_cast<char*>(mb(0)), param_dy.data(), param_dy.size());
                mb.assumeSizeFromReservedBytes();

                my_ball.setDY(mb.read_double());

                debug() << "[" << my_local_time.toString() << "] New ball position: x=" << my_ball.getX()
                        << ", y=" << my_ball.getY() << std::endl;
            }
        }
        catch (std::out_of_range& e) {
            wcout << "The interaction should have all three parameters, skip.\n";
        }
    }
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
    debug() << "########        " << __func__ << " 2, theInteraction=" << theInteraction << endl;
    getchar();
}

void Billard::receiveInteraction(rti1516e::InteractionClassHandle theInteraction,
                                 const ParameterHandleValueMap& /*theParameterValues*/,
                                 const rti1516e::VariableLengthData& /*theUserSuppliedTag*/,
                                 rti1516e::OrderType /*sentOrder*/,
                                 rti1516e::TransportationType /*theType*/,
                                 rti1516e::SupplementalReceiveInfo /*theReceiveInfo*/) throw(FederateInternalError)
{
    debug() << "########        " << __func__ << " 3, theInteraction=" << theInteraction << endl;
    getchar();
}

void Billard::reflectAttributeValues(rti1516e::ObjectInstanceHandle theObject,
                                     const AttributeHandleValueMap& theAttributeValues,
                                     const rti1516e::VariableLengthData& /*theUserSuppliedTag*/,
                                     rti1516e::OrderType /*sentOrder*/,
                                     rti1516e::TransportationType /*theType*/,
                                     const rti1516e::LogicalTime& /*theTime*/,
                                     rti1516e::OrderType /*receivedOrder*/,
                                     rti1516e::MessageRetractionHandle /*theHandle*/,
                                     rti1516e::SupplementalReflectInfo /*theReflectInfo*/) throw(FederateInternalError)
{
    debug() << __func__ << ", theObject=" << theObject << endl;

    debug() << "RECEIVE::" << theAttributeValues << endl;

    static const auto ball_handle = my_ambassador->getObjectClassHandle(L"Ball");
    static const auto attribute_pos_x_handle = my_ambassador->getAttributeHandle(ball_handle, L"PositionX");
    static const auto attribute_pos_y_handle = my_ambassador->getAttributeHandle(ball_handle, L"PositionY");

    auto find_it = std::find_if(
        begin(my_other_balls), end(my_other_balls), [&](const Ball& other) { return other.getHandle() == theObject; });

    if (find_it != end(my_other_balls)) {
        auto& otherBall = *find_it;

        otherBall.erase();

        for (const auto& kv : theAttributeValues) {
            decltype(&Ball::updateX) fun = nullptr;

            if (kv.first == attribute_pos_x_handle) {
                fun = &Ball::updateX;
            }
            else if (kv.first == attribute_pos_y_handle) {
                fun = &Ball::updateY;
            }

            if (fun) {
                libhla::MessageBuffer mb;
                mb.resize(kv.second.size());
                mb.reset();
                std::memcpy(static_cast<char*>(mb(0)), kv.second.data(), kv.second.size());
                mb.assumeSizeFromReservedBytes();
                (otherBall.*fun)(mb.read_double());
                otherBall.setActive();
            }
        }

        otherBall.display();

        debug() << "[" << my_local_time.toString() << "] Other ball position: handle=" << otherBall.getHandle()
                << ", x=" << otherBall.getX() << ", y=" << otherBall.getY() << std::endl;
    }
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
    debug() << "########        " << __func__ << " 2, theObject=" << theObject << endl;
    getchar();
}

void Billard::reflectAttributeValues(rti1516e::ObjectInstanceHandle theObject,
                                     const AttributeHandleValueMap& /*theAttributeValues*/,
                                     const rti1516e::VariableLengthData& /*theUserSuppliedTag*/,
                                     rti1516e::OrderType /*sentOrder*/,
                                     rti1516e::TransportationType /*theType*/,
                                     rti1516e::SupplementalReflectInfo /*theReflectInfo*/) throw(FederateInternalError)
{
    debug() << "########        " << __func__ << " 3, theObject=" << theObject << endl;
    getchar();
}

void Billard::show_sync_points() const
{
#ifdef TRACE
    debug() << "  Current synchronization points: {\n";
    for (const auto& point : my_synchronization_points) {
        debug() << "    - " << point << '\n';
    }
    debug() << "  }" << endl;
#endif
}
