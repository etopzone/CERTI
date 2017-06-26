#include <gtest/gtest.h>

#include <iostream>

using ::std::cout;
using ::std::cerr;
using ::std::endl;

// #include <LibHLA/Clock.hh>
#include <libHLA/Clock.hh>

#include <config.h>

void clockTests(libhla::clock::Clock& aClock)
{
    cout << "Testing clock <" << aClock.getName() << "> BEGIN..." << endl;
    cout << "    Clock resolution is: " << aClock.getResolution() << " nano-seconds" << endl;

    uint64_t tick1 = aClock.getCurrentTicksValue();
    uint64_t tick2 = aClock.getCurrentTicksValue();

    cout << "    Tick1 = " << tick1 << endl;
    cout << "    Tick2 = " << tick2 << endl;

    cout << "    Delta = " << aClock.getDeltaNanoSecond(tick1, tick2) << " nano-seconds" << endl;
    cout << "Testing clock <" << aClock.getName() << "> END." << endl;
}

#ifdef HAVE_TSC_CLOCK
#include <libHLA/TSCClock.hh>

TEST(ClockTest, TscClock)
{
    libhla::clock::TSCClock tscClk;

    clockTests(tscClk);
}
#endif

#ifdef HAVE_POSIX_CLOCK
#include <libHLA/PosixClock.hh>

TEST(ClockTest, PosixClock)
{
    libhla::clock::PosixClock posixClk;

    clockTests(posixClk);
}
#endif

#ifdef HAVE_GETTIMEOFDAY
#include <libHLA/GettimeofdayClock.hh>

TEST(ClockTest, GetTimeOfDayClock)
{
    libhla::clock::GettimeofdayClock gettimeofdayClock;

    clockTests(gettimeofdayClock);
}
#endif

#ifdef HAVE_WIN_CLOCK
#include <libHLA/WinClock.hh>
#include <libHLA/WinPerfClock.hh>

TEST(ClockTest, WinClock)
{
    libhla::clock::WinClock WinClk;
    libhla::clock::WinPerfClock WinPerfClk;

    clockTests(WinClk);
    clockTests(WinPerfClk);
}
#endif

TEST(ClockTest, BestClock)
{
    libhla::clock::Clock* myClock = libhla::clock::Clock::getBestClock();
    clockTests(*myClock);
    delete myClock;
}
