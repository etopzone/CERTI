// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2006  ONERA
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
// ----------------------------------------------------------------------------

#include <config.h>

#include "RTIG.hh"
#include "certi.hh"

#ifdef WIN32
#include <signal.h>
#endif

#include <new>
#include <csignal>

using namespace certi ;
using namespace certi::rtig ;
using std::cerr ;
using std::cout ;
using std::endl ;

static RTIG *rti ;

// ----------------------------------------------------------------------------
//! SignalHandler.
extern "C" void SignalHandler(int sig)
{
    rti->signalHandler(sig);

    // Catch signal again.
    std::signal(sig, SignalHandler);
}

// ----------------------------------------------------------------------------
//! NewHandler
void
NewHandler()
    throw (MemoryExhausted)
{
    throw MemoryExhausted("");
}

// ----------------------------------------------------------------------------
//! RTIG server entry point.
int main(int argc, char *argv[])
{
    gengetopt_args_info args ;
    if (cmdline_parser(argc, argv, &args)) exit(EXIT_FAILURE);
    bool verbose = args.verbose_flag ;

    if (verbose) {
	cout << "CERTI RTIG " VERSION " - Copyright 2002-2006  ONERA" 
	     << endl ;
	cout << "This is free software ; see the source for copying "
	     << "conditions. There is NO\nwarranty ; not even for "
	     << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."
	     << endl << endl ;
    }

    std::signal(SIGINT, SignalHandler);
	#ifndef WIN32
		std::signal(SIGPIPE, SignalHandler);
	#endif

    std::set_new_handler(NewHandler);

    try {
        rti = new RTIG();
	rti->setVerbose(args.verbose_flag);
        rti->execute();
        delete rti ;
    }
    catch (Exception &e) {
        cerr << "RTIG: exception: " << e._name << " -- Exiting." << endl ;
        exit(EXIT_FAILURE);
    }

    if (verbose) {
	cout << "CERTI RTIG exiting." << endl ;
    }
    exit(EXIT_SUCCESS);
}
