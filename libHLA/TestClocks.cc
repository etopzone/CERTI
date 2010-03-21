// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// ----------------------------------------------------------------------------
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include "config.h"
#include <cassert>
#include <string>
#include <cstring>
#include <cstdio>
#include "Clock.hh"
void clockTests(libhla::clock::Clock& aClock) {
	uint64_t tick1;
	uint64_t tick2;
	cout << "Testing clock <" << aClock.getName() << "> BEGIN..." <<endl;
	cout << "    Clock resolution is: " << aClock.getResolution() << " nano-seconds" <<endl;
	tick1 = aClock.getCurrentTicksValue();
	tick2 = aClock.getCurrentTicksValue();
	cout << "    Tick1 = " << tick1 << endl;
	cout << "    Tick2 = " << tick2 << endl;
	cout << "    Delta = " << aClock.getDeltaNanoSecond(tick1,tick2) << " nano-seconds" <<endl;
	cout << "Testing clock <" << aClock.getName() << "> END." <<endl;
}
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


int
main(int argc, char **argv)
{
#ifdef HAVE_POSIX_CLOCK
	libhla::clock::PosixClock posixClk;
#endif
#ifdef HAVE_GETTIMEOFDAY
        libhla::clock::GettimeofdayClock gettimeofdayClock;
#endif
#ifdef HAVE_TSC_CLOCK
	libhla::clock::TSCClock   tscClk;
#endif
#ifdef HAVE_WIN_CLOCK
	libhla::clock::WinClock       WinClk;
	libhla::clock::WinPerfClock   WinPerfClk;
#endif

#ifdef HAVE_TSC_CLOCK
	clockTests(tscClk);
#endif
#ifdef HAVE_POSIX_CLOCK
	clockTests(posixClk);
#endif
#ifdef HAVE_GETTIMEOFDAY
	clockTests(gettimeofdayClock);
#endif
#ifdef HAVE_WIN_CLOCK
	clockTests(WinClk);
	clockTests(WinPerfClk);
#endif

	libhla::clock::Clock *myClock = libhla::clock::Clock::getBestClock () ;
        clockTests(*myClock);
        delete myClock ; 
	cout << "LibHLA Clock Test->END." <<endl;
    /* getchar(); */
	return 0;
}
