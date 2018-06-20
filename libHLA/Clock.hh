// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
//
// This program is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
// ----------------------------------------------------------------------------

#ifndef CLOCK_HH_
#define CLOCK_HH_

#include "libhla.hh" // IWYU pragma: export

namespace libhla {
namespace clock {

/**
 * An abstract Clock class.
 * Clock abstraction which tries to use high-resolution
 * system clock in order to be able to help user to
 * profile it's application using simple clock API.
 * Derived class will use effective clock API like
 * POSIX clock_xxxx functions:
 * http://www.opengroup.org/onlinepubs/000095399/functions/clock_getres.html
 * or other hardware assisted clock like Pentium TSC.
 */
class HLA_EXPORT Clock {
public:
    /**
     * The Clock default constructor.
     * @param[in] newName the clock name
     */
    Clock(const std::string& newName);
    /**
     * Get the clock name.
     * @return the clock name
     */
    const std::string& getName() const
    {
        return name;
    };
    /**
     * Get the clock resolution in nano-seconds.
     * @return the clock resolution in nano-seconds
     */
    virtual double getResolution() = 0;
    /**
     * Get the current ticks value.
     * @return the current ticks value
     */
    virtual uint64_t getCurrentTicksValue() = 0;
    /**
     * Get the delta between current ticks value and
     * the one provided as argument.
     * @param[in] previousTicks a previous ticks value
     * @return the delta tick value
     */
    virtual uint64_t getDeltaTicks(const uint64_t previousTicks);
    /**
     * Convert a number of ticks into a double value
     * representing nanoseconds.
     * @param[in] ticks the number of tick to convert
     * @return the nano-seconds value
     */
    virtual double tick2NanoSecond(const uint64_t ticks) = 0;
    /**
     * Get delta between the provided ticks values in nano-seconds.
     * @param[in] firstTicks the first tick value
     * @param[in] secondTicks the second (later) tick value
     * @return the delta time between the two ticks value in nano-seconds.
     */
    virtual double getDeltaNanoSecond(const uint64_t firstTicks, const uint64_t secondTicks);

    /**
     * Get delta between current ticks value and
     * the one provided as argument as nano-second value
     * @param[in] previousTicks
     */
    virtual double getDeltaNanoSecond(const uint64_t previousTicks);

    virtual ~Clock();

    /** 
    * Retrieve the best possible clock. 
    * @return  the obtained clock      
    */
    static Clock* getBestClock();

protected:
    std::string name;
};

} /* end namespace clock  */
} /* end namespace libhla */

#endif /*CLOCK_HH_*/
