#include "WinPerfClock.hh"
#include <windows.h>

namespace libhla {
namespace clock {

WinPerfClock::WinPerfClock() : Clock("WinPerfClock")
{	
LARGE_INTEGER freq;
if (QueryPerformanceFrequency(&freq))
	{
	resolution = 1.0e9 / static_cast<double>(freq.QuadPart);
	} 
else 	resolution = 0.0 ;
}

double WinPerfClock::getResolution() 
{
return resolution;
}

uint64_t WinPerfClock::getCurrentTicksValue() 
{  
   LARGE_INTEGER tick;
	QueryPerformanceCounter(&tick);
	return static_cast<uint64_t>(tick.QuadPart);
}
	
double WinPerfClock::tick2NanoSecond(const uint64_t ticks) 
{
return resolution*ticks;
}

WinPerfClock::~WinPerfClock()
{
}
} /* end namespace clock  */
} /* end namespace libhla */
