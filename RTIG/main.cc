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
// $Id: main.cc,v 3.6 2003/06/27 17:26:28 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>

#include "RTIG.hh"
#include "RTItypes.hh"

#include <new>
#include <csignal>

using namespace certi ;
using namespace certi::rtig ;

static RTIG *rtip ;

// ----------------------------------------------------------------------------
//! SignalHandler.
extern "C" void SignalHandler(int sig)
{
    rtip->signalHandler(sig);

    // Catch signal again.
    std::signal(sig, SignalHandler);
}

// ----------------------------------------------------------------------------
//! NewHandler
void
NewHandler()
{
    throw MemoryExhausted();
}

// ----------------------------------------------------------------------------
//! RTIG server entry point.
int main(int argc, char *argv[])
{
    gengetopt_args_info args_info ;
    if (cmdline_parser(argc, argv, &args_info) != 0) exit(EXIT_FAILURE);

    cout << "CERTI " VERSION " - Copyright 2002, 2003  ONERA" << endl ;
    cout << "This is free software ; see the source for copying conditions. "
         << "There is NO\nwarranty ; not even for MERCHANTABILITY or FITNESS "
         << "FOR A PARTICULAR PURPOSE." << endl << endl ;

    signal(SIGINT, SignalHandler);
    signal(SIGPIPE, SignalHandler);

    std::set_new_handler(NewHandler);

    try {
        rtip = new RTIG();
        rtip->execute();
        delete rtip ;
    }
    catch (MemoryExhausted &e) {
        cerr << "RTIG: not enough memory. Exiting." << endl ;
        exit(EXIT_FAILURE);
    }
    catch (Exception &e) {
        cerr << "Unhandled exception in RTIG. Exiting." << endl ;
        exit(EXIT_FAILURE);
    }

    cout << "RTIG exiting." << endl ;
    exit(EXIT_SUCCESS);
}

// $Id: main.cc,v 3.6 2003/06/27 17:26:28 breholee Exp $
