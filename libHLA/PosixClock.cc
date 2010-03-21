#include "PosixClock.hh"

#include <stdlib.h>

namespace libhla {
namespace clock {

PosixClock::PosixClock(clockid_t newClockId) : Clock("PosixClock")
{
    clockId = newClockId;
    clock_getres(newClockId, &resolution);
}   

double 
PosixClock::getResolution() {
	return resolution.tv_sec*1e9 + resolution.tv_nsec;
}

uint64_t PosixClock::getCurrentTicksValue() {
	struct timespec  current;
	/* convert struct timespec to number of nano-second */ 
	clock_gettime(clockId, &current);
	return current.tv_sec*1000000000 + current.tv_nsec;
}

double   
PosixClock::tick2NanoSecond(const uint64_t ticks) {
	 return ticks;
}

PosixClock::~PosixClock()
{
}

} /* end namespace clock  */
} /* end namespace libhla */
