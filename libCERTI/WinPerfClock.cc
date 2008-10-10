#include "WinPerfClock.hh"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>

namespace certi {

WinPerfClock::WinPerfClock() : Clock("WinPerfClock")
{	
LARGE_INTEGER freq;
if (QueryPerformanceFrequency(&freq))
	{
	std::cout << "Perffreq = " << freq.QuadPart <<std::endl;	
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

}
