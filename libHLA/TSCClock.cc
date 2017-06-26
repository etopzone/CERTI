#include "TSCClock.hh"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace libhla {
namespace clock {

TSCClock::TSCClock() : Clock("TSCClock")
{
    resolution = 1.0e3 / getProcessorFrequency();
}

double TSCClock::getResolution()
{
    return resolution;
}

uint64_t TSCClock::getCurrentTicksValue()
{
    return readTSC();
}

double TSCClock::tick2NanoSecond(const uint64_t ticks)
{
    return resolution * ticks;
}

TSCClock::~TSCClock()
{
}

double TSCClock::getProcessorFrequency()
{
    FILE* fdcpu;
    char left[256], right[256];
    double freq = -1.0;

    fdcpu = fopen("/proc/cpuinfo", "r");

    while (EOF != fscanf(fdcpu, "%s : %s \n", left, right)) {
        if (strstr(left, "MHz")) {
            freq = atof(right);
            break;
        }
    }
    fclose(fdcpu);
    return freq;
}

} /* end namespace clock  */
} /* end namespace libhla */
