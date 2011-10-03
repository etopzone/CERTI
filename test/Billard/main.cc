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
// $Id: main.cc,v 3.27 2011/10/03 09:18:07 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>

#include "Billard.hh"
#include "BillardDDM.hh"
#include "Display.hh"
#include "Ball.hh"
#include "ColoredBall.hh"
#include "PrettyDebug.hh"

#include "cmdline.h"

#include "graph_c.hh"

#include "RTI.hh"

#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <memory>
#include <math.h>
#include <iostream>
#include <string>
#include <signal.h>
#include <exception>

#ifndef _WIN32
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>
#define    sleep(a)              Sleep(a * 1000)
#define    usleep(a)             Sleep(a / 1000)
#endif

using std::string ;
using std::cout ;
using std::endl ;
using std::cerr ;

static PrettyDebug D("BILLARD", __FILE__);

#ifdef _WIN32
extern "C" void WINAPI WaitForEnd_Win(int *delai);
#endif
extern "C" void WaitForEnd_Unx(int SignalNumber);

void ExceptionHandler();
void TerminateHandler();

static bool exit_billard = false ;

Billard *createBillard(bool, const char *, string);

// ----------------------------------------------------------------------------
/**
 * @defgroup Billard_Application The Billard application 
 * The CERTI "Billard" test program
 * @ingroup CERTI_Applications
 */
int
main(int argc, char **argv)
{
    cout << "CERTI Billard " VERSION << endl ;

    try {
	// Handlers
	signal(SIGINT, WaitForEnd_Unx);
	#ifndef _WIN32
		signal(SIGALRM, WaitForEnd_Unx);
	#endif
	std::set_terminate(TerminateHandler);
	std::set_unexpected(ExceptionHandler);

	// Command line
	gengetopt_args_info args ;
	if (cmdline_parser(argc, argv, &args))
	    exit(EXIT_FAILURE);

	bool verbose = args.verbose_flag ;

	// Federation and .fed names
	string federation = args.federation_arg ;
	string federate = args.name_arg ;
        // -F option gives filename (FED)
        string fedfile = args.filename_arg ;

	// Create billard
	std::auto_ptr<Billard> billard(createBillard(args.demo_given, args.demo_arg, federate));
	billard->setVerbose(verbose);

        // With or without timestamp ?
        if ( args.notimestamp_flag == 0 )
            {
            // without timestamp
            billard->setNotimestamp(true) ;
            cout<<"without TIMESTAMP. If you want TIMESTAMP remove -e option."<<endl;
            }
        else
            {
            // with timestamp
            billard->setNotimestamp(false) ;
            cout<<"with TIMESTAMP. If you want without TIMESTAMP add -e option."<<endl;
            }

	int timer = args.timer_given ? args.timer_arg : 0 ;
	int delay = args.delay_given ? args.delay_arg : 0 ;
	int autostart = args.auto_given ? args.auto_arg : 0 ;

	// Joins federation
	Debug(D, pdDebug) << "Create or join federation" << endl ;
	billard->join(federation, fedfile);
	RTI::FederateHandle handle = billard->getHandle();

	// Display...
	std::auto_ptr<Display> display(Display::instance());
	int y_default = 25 + (handle - 1) * (display->getHeight() + 20);
	display->setWindow(
	    args.xoffset_given ? args.xoffset_arg : 400,
	    args.yoffset_given ? args.yoffset_arg : y_default);

	// Continue initialisation...
	Debug(D, pdDebug) << "Synchronization" << endl ;
	billard->pause();
	Debug(D, pdDebug) << "Publish and subscribe" << endl ;
	billard->publishAndSubscribe();
	display->show();

	if (args.coordinated_flag) {
	    billard->setTimeRegulation(true, true);
	    billard->tick();
	}
	billard->synchronize(autostart);

	// Countdown
	#ifdef _WIN32
		CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)WaitForEnd_Win,&timer,0,NULL);	
	#else
		struct sigaction a ;
		a.sa_handler = WaitForEnd_Unx ;
		sigemptyset(&a.sa_mask);
		a.sa_flags   = SA_RESTART;
		sigaction(SIGALRM, &a, NULL);

		// set timer
		if (timer != 0) {
			 printf("Timer ... : %5d\n", timer);
			 alarm(timer);
		}
	#endif

	// Create object
	if (args.initx_given && args.inity_given) {
	    billard->init(args.initx_arg, args.inity_arg);
	}
	else {
	    billard->init(handle);
	}

	// registers objects, regions, etc.
	billard->declare();
	cout << "Declaration done." << endl ;

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

#ifdef TEST_RFSP
	// Simulation loop
        int numstep = 0 ;
        if ( billard->getCreator() )
            // Creator will stop and ask for the friendly federate
            {
	    while (!exit_billard)
                {
	        if ( numstep != 10 )
                    {
                    billard->step();
                    }
                else
                    {
                    std::cout<<"Creator stops after 10 steps for synchro with" 
                               " friend"<<std::endl;
                    billard->pause_friend() ;
                    }
                numstep++;
                }
            }
        else
            {
 	    while (!exit_billard)
                {
	        billard->step();
                }
            }
#else
	// Simulation loop
 	while (!exit_billard)
            {
	    billard->step();
            }
#endif

	// End of simulation
	D.Out(pdTrace, "End of simulation loop.");
	billard->resign();
    }
    catch (RTI::Exception &e) {
    	cerr << "Billard::main: RTI exception: " << e._name << " [" 
    		 << (e._reason ? e._reason : "undefined") << "]." << endl ;
    }
    catch (...) {
	    cerr << "Billard::main: Error: unknown non-RTI exception." << std::endl ;
    }

    std::cout << "Exiting." << std::endl ;
    return 0;
}

// ----------------------------------------------------------------------------
/** Signal handler
 */
void
WaitForEnd_Unx(int number)
{
switch (number) 
	{
	#ifndef _WIN32
	case SIGALRM: {
		D.Out(pdTerm, "Alarm signal received, exiting...");
		exit_billard = true ;
		} break ;
	#endif
		
	case SIGINT: {
		cout << "Exit request received" << endl ;
		exit_billard = true ;
		} break ;
		
	default: {
		D.Out(pdTerm, "Emergency stop, destroying Ambassadors.");
		D.Out(pdTerm, "Federate terminated.");
		exit(EXIT_FAILURE);
		} 
	}
}

#ifdef _WIN32
void WINAPI WaitForEnd_Win(int *delai)
{

if (*delai) sleep(*delai); else sleep(INFINITE);
D.Out(pdTerm, "Alarm signal received, exiting...");
exit_billard = true ;
}
#endif

// ----------------------------------------------------------------------------
/** Exception handler
 */
void
ExceptionHandler()
{
    cerr << "Billard: unexpected exception." << endl ;
    exit(EXIT_FAILURE);
}

// ----------------------------------------------------------------------------
/** Terminate handler
 */
void
TerminateHandler()
{
    cerr << "Billard: unknown exception." << endl ;
    exit(EXIT_FAILURE);
}

// ----------------------------------------------------------------------------
/** Select a "Billard" subclass, from the command line
    \param demo Use the second parameter if true ; use default if false
    \param s_demo Keyword to select the test
    \param name Federate name
 */
Billard *
createBillard(bool demo, const char *s_demo, string name)
{
    if (demo) {
	Debug(D, pdDebug) << "Create billard " << s_demo << endl ;

	string s = s_demo ;

	if (s == "static-ddm")
	    return new BillardStaticDDM(name);
	else if (s == "dynamic-ddm")
	    return new BillardDynamicDDM(name);
	else
	    cout << "unknown keyword: " << s_demo << endl ;
    }
    
    return new Billard(name);
}
