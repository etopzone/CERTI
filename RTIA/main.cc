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
// $Id: main.cc,v 3.30 2014/02/28 15:53:34 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>

#include "RTIA.hh"
#include "RTIA_cmdline.h"

#include <sys/types.h>
#include <csignal>

using namespace certi;
using namespace rtia;
using namespace std;

int normal_end;

// ----------------------------------------------------------------------------
void SignalHandler(int signal) {
    printf("\nRTIA: Received signal %d. Exiting peacefully.\n", signal);
    normal_end = 1;
    // Catch signal again.
    std::signal(signal, SignalHandler);
}

// ----------------------------------------------------------------------------
void NewHandler() {
    throw MemoryExhausted("RTIA (main) has exhausted memory error");
}

/**
 * @defgroup certi_executable_RTIA RTIA
 *
 * The CERTI RunTime Infrastructure Ambassador (RTIA) is a process
 * which is automatically launched by the federate as soon as its
 * RTIambassador is created.
 * The command line usage of the RTIA is following:
 * \par rtia [-v] [-p \<port\>]
 * \par
 * <ul>
 *   <li> \b -v  (optional) verbose, display more information </li>
 *   <li> \b -p  (optional) tcp port to be used to communicate with FederateAmbassador</li>
 * </ul>
 * RTIA will try to connect to RTIG process on the machine specified in CERTI_HOME
 * (see \ref certi_user_env) environment variable. If it is void or not set then he will
 * try to connect to localhost. RTIA connect to TCP port specified by CERTI_TCP_PORT
 * and UDP port specified by CERTI_UDP_PORT.
 *
 * @ingroup certi_executable
 */
int
main(int argc, char **argv) {
	signal(SIGINT, SignalHandler);
#ifndef _WIN32
	signal(SIGPIPE, SignalHandler);
#endif

	set_new_handler(NewHandler);
    normal_end = 0;

	// Command line
	gengetopt_args_info args ;
	if (cmdline_parser(argc, argv, &args))
	    exit(EXIT_FAILURE);

	try {
		int rtia_port = 0;
		if (args.port_given) {
			rtia_port = args.port_arg;
		}
		int rtia_fd = -1;
		if (args.fd_given) {
			rtia_fd = args.fd_arg;
		}

		RTIA rtia(rtia_port, rtia_fd);

		PrettyDebug::setFederateName("RTIA::UnknownFederate");

		try {
			rtia.execute();
		}
		catch (Exception &e) {
                        if (! normal_end) {
			        cerr << "RTIA:: RTIA has thrown " << e._name << " exception." << endl;
			        cerr.flush();
                        }
			if (!e._reason.empty()) {
                                if (! normal_end) {
				      cerr << "RTIA:: Reason: " << e._reason << endl;
				      cerr.flush();
                                }
				rtia.displayStatistics();
			}

			return (EXIT_FAILURE);
		}

		rtia.displayStatistics();
	} catch (Exception &e) {
                if (! normal_end) {
		      cerr << "RTIA:: RTIA has thrown " << e._name << " exception." << endl;
		      if (!e._reason.empty()) {
		 	  cerr << "RTIA:: Reason: " << e._reason << endl;
		      }
                }
	}
	cout << "RTIA:: End execution."<< endl ;

	return (EXIT_SUCCESS);
}


// EOF
