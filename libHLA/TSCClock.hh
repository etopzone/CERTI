#ifndef TSCCLOCK_HH_
#define TSCCLOCK_HH_

#include "Clock.hh"

namespace libhla {
namespace clock {
/**
 * The TSCClock is a Clock using the Time Stamp Counter
 * which may be found in Pentium-like processors
 * http://en.wikipedia.org/wiki/Time_Stamp_Counter
 * Note that using TSC on a symmetric (or multi-core)
 * multi-processor system may gives unexpected results
 * moreover TSC behavior may be biaised if processor
 * supports dynamic frequency scaling such as Pentium M
 * used on mobile device.
 * If you want a more "robust" Clock class you may use PosixClock.
 */
class HLA_EXPORT TSCClock : public Clock
{
public:
	TSCClock();
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
	virtual double   tick2NanoSecond(const uint64_t ticks);
	virtual ~TSCClock();
private:
	/**
	 * Get processor frequency
	 * @return the processor frequency in MHz
	 */
	static double getProcessorFrequency();

	/**
	 * Read the Time Stamp Counter using
	 * rdtsc assembly instruction
	 */
	inline uint64_t readTSC() {
	  uint64_t tsc;
	  __asm__ __volatile__("rdtsc" : "=A" (tsc));
	  return tsc;
	};

	/**
	 * The TSC clock resolution in nano-seconds
	 */
	double resolution;

};

} /* end namespace clock  */
} /* end namespace libhla */
#endif /*TSCCLOCK_HH_*/
