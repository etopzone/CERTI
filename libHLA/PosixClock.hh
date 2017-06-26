#ifndef POSIXCLOCK_HH_
#define POSIXCLOCK_HH_

#include "Clock.hh"

#include <ctime>
#include <sys/time.h>

namespace libhla {
namespace clock {

/**
 * A Clock implemented using POSIX clock_xxxxx API.
 */
class HLA_EXPORT PosixClock : public Clock {
public:
    /**
	 * Build a POSIX clock using clock id.
	 * @param[in] clockId the POSIX clock identifier which may be
	 *      - CLOCK_REALTIME (default value)
	 *      - CLOCK_MONOTONIC
	 *      - CLOCK_PROCESS_CPUTIME_ID
	 *      - CLOCK_THREAD_CPUTIME_ID
	 *      - CLOCK_REALTIME_HR
	 *      - CLOCK_MONOTONIC_HR
	 */
    PosixClock(clockid_t newClockId = CLOCK_REALTIME);
    /**
	 * Get the clock resolution in nano-seconds.
	 * @return the clock resolution in nano-seconds
	 */
    virtual double getResolution();
    /**
	 * Get the current ticks value.
	 * @return the current ticks value
	 */
    virtual uint64_t getCurrentTicksValue();
    /**
	 * Convert a number of ticks into a double value
	 * representing nanoseconds.
	 * @param[in] ticks the number of tick to convert
	 * @return the nano-seconds value
	 */
    virtual double tick2NanoSecond(const uint64_t ticks);
    virtual ~PosixClock();

private:
    struct timespec resolution;
    clockid_t clockId;
};

} /* end namespace clock  */
} /* end namespace libhla */
#endif /*POSIXCLOCK_HH_*/
