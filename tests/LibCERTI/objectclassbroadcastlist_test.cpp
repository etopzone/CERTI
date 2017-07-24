#include <gtest/gtest.h>

#include "libCERTI/ObjectClassBroadcastList.hh"

using ::certi::ObjectBroadcastLine;

TEST(ObjectBroadcastLineTest, CtorCopiesFedHandle)
{
    ObjectBroadcastLine line{42};

    ASSERT_EQ(42, line.Federate);
}

TEST(ObjectBroadcastLineTest, CtorInitializeStateSent)
{
    ObjectBroadcastLine line{42, ObjectBroadcastLine::sent};

    for(int i{0}; i<MAX_STATE_SIZE+1; ++i) {
        ASSERT_EQ(ObjectBroadcastLine::sent, line.state[i]) << ", no" << i;
    }
}

TEST(ObjectBroadcastLineTest, CtorInitializeStateWaiting)
{
    ObjectBroadcastLine line{42, ObjectBroadcastLine::waiting};
    
    for(int i{0}; i<MAX_STATE_SIZE+1; ++i) {
        ASSERT_EQ(ObjectBroadcastLine::waiting, line.state[i]) << ", no" << i;
    }
}

TEST(ObjectBroadcastLineTest, CtorInitializeStateNotSub)
{
    ObjectBroadcastLine line{42, ObjectBroadcastLine::notSub};
    
    for(int i{0}; i<MAX_STATE_SIZE+1; ++i) {
        ASSERT_EQ(ObjectBroadcastLine::notSub, line.state[i]) << ", no" << i;
    }
}

#ifdef BENCHMARK_ARRAY_FILL

#include <chrono>

enum State {
    sent,
    waiting,
    notSub
};

enum class CState: char {
    sent,
    waiting,
    notSub
};

#define SIIIZE 1024*8

using AttributeHandle = uint32_t;

TEST(ObjectClassBroadcastList, initFor_sent)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    State state[SIIIZE+1] ;
    
    auto init_state = sent;
    
    for (AttributeHandle i = 0 ; i <= SIIIZE ; i++)
        state[i] = init_state ;
    
    auto end = std::chrono::high_resolution_clock::now();
    
    for(int i{0}; i<SIIIZE+1; ++i) {
        ASSERT_EQ(init_state, state[i]) << ", no" << i;
    }
    
    std::cerr << "initFor_sent: " << (end - start).count() << " ns" << std::endl;
}

TEST(ObjectClassBroadcastList, initFor_waiting)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    State state[SIIIZE+1] ;
    
    auto init_state = waiting;
    
    for (AttributeHandle i = 0 ; i <= SIIIZE ; i++)
        state[i] = init_state ;
    
    auto end = std::chrono::high_resolution_clock::now();
    
    for(int i{0}; i<SIIIZE+1; ++i) {
        ASSERT_EQ(init_state, state[i]) << ", no" << i;
    }
    
    std::cerr << "initFor_waiting: " << (end - start).count() << " ns" << std::endl;
}

TEST(ObjectClassBroadcastList, initFor_notSub)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    State state[SIIIZE+1] ;
    
    auto init_state = notSub;
    
    for (AttributeHandle i = 0 ; i <= SIIIZE ; i++)
        state[i] = init_state ;
    
    auto end = std::chrono::high_resolution_clock::now();
    
    for(int i{0}; i<SIIIZE+1; ++i) {
        ASSERT_EQ(init_state, state[i]) << ", no" << i;
    }
    
    std::cerr << "initFor_notSub: " << (end - start).count() << " ns" << std::endl;
}

TEST(ObjectClassBroadcastList, initCtor_sent)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    auto init_state = sent;
    
    State state[SIIIZE+1] = {init_state};
    
    auto end = std::chrono::high_resolution_clock::now();
    
    for(int i{0}; i<SIIIZE+1; ++i) {
        ASSERT_EQ(init_state, state[i]) << ", no" << i;
    }
    
    std::cerr << "initCtor_sent: " << (end - start).count() << " ns" << std::endl;
}

TEST(ObjectClassBroadcastList, initCtor_waiting)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    auto init_state = waiting;
    
    State state[SIIIZE+1] = {init_state};
    
    auto end = std::chrono::high_resolution_clock::now();
    
    for(int i{0}; i<SIIIZE+1; ++i) {
        ASSERT_EQ(init_state, state[i]) << ", no" << i;
    }
    
    std::cerr << "initCtor_waiting: " << (end - start).count() << " ns" << std::endl;
}

TEST(ObjectClassBroadcastList, initCtor_notSub)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    auto init_state = notSub;
    
    State state[SIIIZE+1] = {init_state};
    
    auto end = std::chrono::high_resolution_clock::now();
    
    for(int i{0}; i<SIIIZE+1; ++i) {
        ASSERT_EQ(init_state, state[i]) << ", no" << i;
    }
    
    std::cerr << "initCtor_notSub: " << (end - start).count() << " ns" << std::endl;
}

TEST(ObjectClassBroadcastList, initMemset_sent)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    State state[SIIIZE+1] ;
    
    auto init_state = sent;
    
    memset(state, init_state, SIIIZE+1 * sizeof(init_state));
    
    auto end = std::chrono::high_resolution_clock::now();
    
    for(int i{0}; i<SIIIZE+1; ++i) {
        ASSERT_EQ(init_state, state[i]) << ", no" << i;
    }
    
    std::cerr << "initMemset_sent: " << (end - start).count() << " ns" << std::endl;
}

TEST(ObjectClassBroadcastList, initMemset_waiting)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    State state[SIIIZE+1] ;
    
    auto init_state = waiting;
    
    memset(state, init_state, SIIIZE+1 * sizeof(init_state));
    
    auto end = std::chrono::high_resolution_clock::now();
    
    for(int i{0}; i<SIIIZE+1; ++i) {
        ASSERT_EQ(init_state, state[i]) << ", no" << i;
    }
    
    std::cerr << "initMemset_waiting: " << (end - start).count() << " ns" << std::endl;
}

TEST(ObjectClassBroadcastList, initMemset_notSub)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    State state[SIIIZE+1] ;
    
    auto init_state = notSub;
    
    memset(state, init_state, SIIIZE+1 * sizeof(init_state));
    
    auto end = std::chrono::high_resolution_clock::now();
    
    for(int i{0}; i<SIIIZE+1; ++i) {
        ASSERT_EQ(init_state, state[i]) << ", no" << i;
    }
    
    std::cerr << "initMemset_notSub: " << (end - start).count() << " ns" << std::endl;
}

TEST(ObjectClassBroadcastList, initCFor_sent)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    CState state[SIIIZE+1] ;
    
    auto init_state = CState::sent;
    
    for (AttributeHandle i = 0 ; i <= SIIIZE ; i++)
        state[i] = init_state ;
    
    auto end = std::chrono::high_resolution_clock::now();
    
    for(int i{0}; i<SIIIZE+1; ++i) {
        ASSERT_EQ(init_state, state[i]) << ", no" << i;
    }
    
    std::cerr << "initCFor_sent: " << (end - start).count() << " ns" << std::endl;
}

TEST(ObjectClassBroadcastList, initCFor_waiting)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    CState state[SIIIZE+1] ;
    
    auto init_state = CState::waiting;
    
    for (AttributeHandle i = 0 ; i <= SIIIZE ; i++)
        state[i] = init_state ;
    
    auto end = std::chrono::high_resolution_clock::now();
    
    for(int i{0}; i<SIIIZE+1; ++i) {
        ASSERT_EQ(init_state, state[i]) << ", no" << i;
    }
    
    std::cerr << "initCFor_waiting: " << (end - start).count() << " ns" << std::endl;
}

TEST(ObjectClassBroadcastList, initCFor_notSub)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    CState state[SIIIZE+1] ;
    
    auto init_state = CState::notSub;
    
    for (AttributeHandle i = 0 ; i <= SIIIZE ; i++)
        state[i] = init_state ;
    
    auto end = std::chrono::high_resolution_clock::now();
    
    for(int i{0}; i<SIIIZE+1; ++i) {
        ASSERT_EQ(init_state, state[i]) << ", no" << i;
    }
    
    std::cerr << "initCFor_notSub: " << (end - start).count() << " ns" << std::endl;
}

TEST(ObjectClassBroadcastList, initFill_sent)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    auto init_state = CState::sent;
    
    CState state[SIIIZE+1];
    
    std::fill_n(state, SIIIZE+1, init_state);
    
    auto end = std::chrono::high_resolution_clock::now();
    
    for(int i{0}; i<SIIIZE+1; ++i) {
        ASSERT_EQ(init_state, state[i]) << ", no" << i;
    }
    
    std::cerr << "initFill_sent: " << (end - start).count() << " ns" << std::endl;
}

TEST(ObjectClassBroadcastList, initFill_waiting)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    auto init_state = CState::waiting;
    
    CState state[SIIIZE+1];
    
    std::fill_n(state, SIIIZE+1, init_state);
    
    auto end = std::chrono::high_resolution_clock::now();
    
    for(int i{0}; i<SIIIZE+1; ++i) {
        ASSERT_EQ(init_state, state[i]) << ", no" << i;
    }
    
    std::cerr << "initFill_waiting: " << (end - start).count() << " ns" << std::endl;
}

TEST(ObjectClassBroadcastList, initFill_notSub)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    auto init_state = CState::notSub;
    
    CState state[SIIIZE+1];
    
    std::fill_n(state, SIIIZE+1, init_state);
    
    auto end = std::chrono::high_resolution_clock::now();
    
    for(int i{0}; i<SIIIZE+1; ++i) {
        ASSERT_EQ(init_state, state[i]) << ", no" << i;
    }
    
    std::cerr << "initFill_notSub: " << (end - start).count() << " ns" << std::endl;
}

#endif
