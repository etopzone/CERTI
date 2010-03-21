#ifndef WinClock_HH_
#define WinClock_HH_

#include "Clock.hh"

namespace libhla {
namespace clock {
/**
 */
class HLA_EXPORT WinClock : public Clock
{
public:
	WinClock();
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
	virtual ~WinClock();

private:
	/**
	 * The WinClock resolution in nano-seconds
	 */
	double resolution;

};
} /* end namespace clock  */
} /* end namespace libhla */
#endif /*WinClock_HH_*/
