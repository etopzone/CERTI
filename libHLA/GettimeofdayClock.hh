// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
// Copyright (C) 2009 Mathias Froehlich
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: GettimeofdayClock.hh,v 1.1 2010/03/21 17:10:10 erk Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_GETTIMEOFDAYCLOCK_HH_
#define CERTI_GETTIMEOFDAYCLOCK_HH_

#include "Clock.hh"

namespace libhla {
namespace clock {

/**
 * A Clock implemented using gettimeofday.
 */
class HLA_EXPORT GettimeofdayClock : public Clock {
public:
    /**
     * Build a UNIX clock.
     */
    GettimeofdayClock();
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
    virtual ~GettimeofdayClock();
};

} /* end of clock namespace */
} /* and of libhla namespace */
#endif /*CERTI_GETTIMEOFDAYCLOCK_HH_*/
