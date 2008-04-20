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
// $Id: main.cc,v 3.18.2.1 2008/04/20 12:52:21 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>

#include "RTIA.hh"

#include <sys/types.h>
#include <signal.h>

using namespace certi;
using namespace rtia;
using namespace std;

extern "C"void SignalHandler(int Signal);
void NewHandler();

// ----------------------------------------------------------------------------
int main() {
	signal(SIGINT, SignalHandler);
#ifndef _WIN32
	signal(SIGPIPE, SignalHandler);
#endif

	set_new_handler(NewHandler);

	if (NULL!=getenv("RTIA_DEBUG")) {
		cerr << "RTIA:: RTIA_DEBUG is set: Waiting <"<< getenv("RTIA_DEBUG")
				<< " seconds> before starting RTIA"<<endl;
		sleep(atoi(getenv("RTIA_DEBUG")));
	}

	try {
		RTIA rtia;

		try {
			rtia.execute();
		}
		catch (Exception &e) {
			cerr << "RTIA:: RTIA has thrown " << e._name << " exception." << endl;
			cerr.flush();
			if (e._reason) {
				cerr << "RTIA:: Reason: " << e._reason << endl;
				cerr.flush();
				rtia.displayStatistics();
			}

			return (EXIT_FAILURE);
		}

		rtia.displayStatistics();
	} catch (Exception &e) {
		cerr << "RTIA:: RTIA has thrown " << e._name << " exception." << endl;
		if (e._reason) {
			cerr << "RTIA:: Reason: " << e._reason << endl;
		}
	}
	cout << "RTIA:: End execution."<< endl ;

	return (EXIT_SUCCESS);
}

// ----------------------------------------------------------------------------
void SignalHandler(int Signal) {

	printf("\nRTIA: Received signal %d. Exiting peacefully.\n", Signal);
        //exit(0);
}

// ----------------------------------------------------------------------------
void NewHandler() {
	throw MemoryExhausted("RTIA has exhausted memory error");
}

// EOF $Id: main.cc,v 3.18.2.1 2008/04/20 12:52:21 erk Exp $
