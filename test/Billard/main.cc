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
// $Id: main.cc,v 3.1 2003/08/06 14:37:47 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>

#include "Billard.hh"
#include "Display.hh"
#include "Ball.hh"
#include "ColoredBall.hh"
#include "Fed.hh"
#include "PrettyDebug.hh"

#include "cmdline.h"

#include "graph_c.hh"
#include "RTI.hh"

#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <iostream>
#include <signal.h>
#include <exception>

using namespace std ;

static pdCDebug D("BILLARD", __FILE__ "> ");

extern "C" void sortir(int SignalNumber);
void ExceptionHandler();

#ifdef HAVE_XML
static const bool WITH_XML = true ;
#else
static const bool WITH_XML = false ;
#endif

static bool exit_billard = false ;

// ----------------------------------------------------------------------------
//! Test program entry point.
int
main(int argc, char **argv)
{
    cout << PACKAGE << '-' << VERSION << endl ;

    // Handlers
    signal(SIGINT, sortir);
    signal(SIGALRM, sortir);
    set_terminate(ExceptionHandler);
    set_unexpected(ExceptionHandler);

    // Command line
    gengetopt_args_info args ;
    if (cmdline_parser(argc, argv, &args) != 0)
        exit(EXIT_FAILURE);

    bool verbose = args.verbose_flag ;

    // Names
    string federation = args.federation_arg ;
    string federate = args.name_arg ;
    string fedfile = args.federation_arg + WITH_XML ? ".xml" : ".fed" ;

    // Create billard
    Billard billard(federate);

    int timer = args.timer_given ? args.timer_arg : 0 ;
    int delay = args.delay_given ? args.delay_arg : 0 ;
    int autostart = args.auto_given ? args.auto_arg : 0 ;

    // Joins federation
    billard.join(federation, fedfile);
    FederateHandle handle = billard.getHandle();

    // Display...
    Display *display = Display::instance();
    display->setWindow(
        args.xoffset_given ? args.xoffset_arg : 10,
        args.yoffset_given ? args.yoffset_arg : 10 + (handle - 1) * (display->getHeight() + 20));

    // Continue initialisation...
    billard.pause();
    billard.publishAndSubscribe();

    display->show();

    if (args.coordinated_flag) {
        billard.setTimeRegulation(true, true);
        billard.tick();
    }
    billard.synchronize(autostart);

    // Countdown
    struct sigaction a ;

    a.sa_handler = sortir ;
    sigemptyset(&a.sa_mask);
    sigaction(SIGALRM, &a, NULL);

    // set timer
    if (timer != 0) {
        printf("Timer : %5d\n", timer);
        alarm(timer);
    }

    // Create object
    if (args.initx_given && args.inity_given) {
        billard.init(args.initx_arg, args.inity_arg);
    }
    else {
        billard.init(handle);
    }

    // declare objects
    billard.create();

    // set delay
    if (delay != 0) {
        while (delay >= 0) {
            sleep(1);
            printf("\rDelay : %5d", delay);
            fflush(stdout);
            delay-- ;
        }
        printf("\n");
    }

    // Simlation loop
    while (!exit_billard) {
        billard.step();
    }

    // End of simulation
    D.Out(pdTrace, "End of simulation loop.");
    billard.resign();

    delete display ;
}

// ----------------------------------------------------------------------------
//! sortir.
void
sortir(int SignalNumber)
{
    if (SignalNumber == SIGALRM) {
        D.Out(pdTerm, "Alarm signal received, exiting...");
        exit_billard = true ;
    }
    else {
        D.Out(pdTerm, "Emergency stop, destroying Ambassadors.");
        D.Out(pdTerm, "Federate terminated.");
        exit(EXIT_FAILURE);
    }
}

// ----------------------------------------------------------------------------
//! ExceptionHandler.
void
ExceptionHandler()
{
    D.Out(pdExcept, "****Exception thrown on the 'test_heritage' Federate.");
    exit(-1);
}

// EOF $Id: main.cc,v 3.1 2003/08/06 14:37:47 breholee Exp $
