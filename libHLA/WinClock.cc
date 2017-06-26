#include "WinClock.hh"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>

namespace libhla {
namespace clock {

WinClock::WinClock() : Clock("WinClock")
{
    resolution = 1.0e9 / CLOCKS_PER_SEC;
}

double WinClock::getResolution()
{
    return resolution;
}

uint64_t WinClock::getCurrentTicksValue()
{
    return static_cast<uint64_t>(::clock());
}

double WinClock::tick2NanoSecond(const uint64_t ticks)
{
    return resolution * ticks;
}

WinClock::~WinClock()
{
}

} /* end namespace clock  */
} /* end namespace libhla */
