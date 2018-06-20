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

#include "Clock.hh"
#include "config.h"

#ifdef HAVE_TSC_CLOCK
#include "TSCClock.hh"
#endif

#ifdef HAVE_POSIX_CLOCK
#include "PosixClock.hh"
#endif

#ifdef HAVE_GETTIMEOFDAY
#include "GettimeofdayClock.hh"
#endif

#ifdef HAVE_WIN_CLOCK
#include "WinClock.hh"
#include "WinPerfClock.hh"
#endif

namespace libhla {
namespace clock {

Clock* Clock::getBestClock()
{
// clang-format off
#ifdef _WIN32
    #ifdef HAVE_WIN_CLOCK
        return new WinPerfClock();
    #else
        return new WinClock();
    #endif
#else
    #ifdef HAVE_TSC_CLOCK
        return new TSCClock();
    #else
        #ifdef HAVE_POSIX_CLOCK
            return new PosixClock();
        #else
            #ifdef HAVE_GETTIMEOFDAY
                return new GettimeofdayClock();
            #else
                return nullptr;
            #endif
        #endif
    #endif
#endif
// clang-format off
} /* end of getBestClock */

} /* end namespace clock  */
} /* end namespace libhla */
