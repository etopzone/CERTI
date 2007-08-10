#include "PosixClock.hh"

#include <stdlib.h>

namespace certi {

PosixClock::PosixClock(clockid_t clockId) : Clock("PosixClock")
{
		this->clockId = clockId;
		clock_getres(clockId,&this->resolution);
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

}
