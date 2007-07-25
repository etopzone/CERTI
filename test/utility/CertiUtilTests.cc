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
#include "PrettyDebug.hh"

#include "Clock.hh"
void clockTests(Clock& aClock) {
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


int
main(int argc, char **argv)
{	
#ifdef HAVE_POSIX_CLOCK	
	PosixClock posixClk;
#endif
#ifdef HAVE_TSC_CLOCK	
	TSCClock   tscClk;
#endif
	
	cout << "CERTI Utility Tests->BEGIN..."<< endl ;
#ifdef HAVE_TSC_CLOCK
    clockTests(tscClk);
#endif
#ifdef HAVE_POSIX_CLOCK
    clockTests(posixClk);
#endif
    cout << "CERTI Utility Test->END." <<endl;
		
	return 0;
}
