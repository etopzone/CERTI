// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: main.cc,v 3.8 2003/06/07 22:24:12 breholee Exp $
// ----------------------------------------------------------------------------

#include "RTIA.hh"

using namespace certi ;
using namespace rtia ;

extern "C" void SignalHandler(int Signal);
void NewHandler();

// ----------------------------------------------------------------------------
int
main(int argc, char *argv[])
{
    signal(SIGINT, SignalHandler);
    signal(SIGPIPE, SignalHandler);

    std::set_new_handler(NewHandler);

    RTIA rtia ;

    try {
	rtia.execute();
    }
    catch (Exception &e) {
	cerr << "\nRTIA has thrown " << e._name << " exception." << endl ;
	if (e._reason)
	    cerr << "Reason: " << e._reason << endl ;
	
	return EXIT_FAILURE ;
    }

    rtia.displayStatistics();
    cout << "RTIA: End execution." << endl ;
    
    return EXIT_SUCCESS ;
}

// ----------------------------------------------------------------------------
void
SignalHandler(int Signal)
{
    int pid = getpid();

    printf("\nRTIA: Received signal %d. Killing myself softly.\n", Signal);
    kill(SIGKILL, pid);
}

// ----------------------------------------------------------------------------
void
NewHandler()
{
    throw MemoryExhausted();
}

// EOF $Id: main.cc,v 3.8 2003/06/07 22:24:12 breholee Exp $
