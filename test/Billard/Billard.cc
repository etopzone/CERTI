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
// $Id: Billard.cc,v 3.1 2003/08/06 14:37:47 breholee Exp $
// ----------------------------------------------------------------------------

#include "Billard.hh"

#include <config.h>
#include "PrettyDebug.hh"

#include <unistd.h>

using std::string ;
using std::endl ;

static pdCDebug D("BILLARD", __FILE__ "> ");

// ----------------------------------------------------------------------------
Billard::Billard(string federate_name)
    : rtiamb(), fedamb(&rtiamb), objects(rtiamb, fedamb, 500, 100),
      federateName(federate_name),
      handle(0), creator(false), nbTicks(0),
      regulating(false), constrained(false), localTime(0.0), TIME_STEP(1.0),
      XMAX(500), YMAX(100)
{
    fedamb.init(&objects);
}

// ----------------------------------------------------------------------------
FederateHandle
Billard::getHandle() const
{
    return handle ;
}

// ----------------------------------------------------------------------------
void
Billard::join(string federation_name, string fdd_name)
{
    federationName = federation_name ;

    // create federation
    try {
        rtiamb.createFederationExecution(federation_name.c_str(),
                                         fdd_name.c_str());
        D.Out(pdInit, "Federation execution created.");
        creator = true ;
    }
    catch (FederationExecutionAlreadyExists& e) {
        D.Out(pdInit, "Federation execution already created.");
    }

    // join federation
    bool joined = false ;
    int nb = 5 ;

    while (!joined && nb > 0) {
        nb-- ;
        try {
            handle = rtiamb.joinFederationExecution(federateName.c_str(),
                                                    federation_name.c_str(),
                                                    &fedamb);
            joined = true ;
            break ;
        }
        catch (FederateAlreadyExecutionMember& e) {
            D[pdExcept] << "Federate " << federateName.c_str()
                        << "already exists." << endl ;

            throw ;
        }
        catch (FederationExecutionDoesNotExist& e) {
            D.Out(pdExcept, "Federate %s : FederationExecutionDoesNotExist.",
                  federateName.c_str());
            // sleep(1);
        }
        catch (Exception& e) {
            D.Out(pdExcept,
                  "Federate %s :Join Federation Execution failed : %d .",
                  federateName.c_str(), &e);
            throw ;
        }
    }
}

// ----------------------------------------------------------------------------
// Creator put federation in pause.
void
Billard::pause()
{
    if (creator) {
        D.Out(pdInit, "Pause requested");
        try {
            rtiamb.registerFederationSynchronizationPoint(
                "Init", "Waiting all players.");
        }
        catch (Exception& e) {
            D[pdExcept] << "Federate " << federateName.c_str()
                        << " : Register Synchronization Point failed : %d"
                        << endl ;
        }
    }
}

// ----------------------------------------------------------------------------
// Publish and subscribe
void
Billard::publishAndSubscribe()
{
    fedamb.publishAndSubscribe();
}

// ----------------------------------------------------------------------------
// tick
void
Billard::tick()
{
    rtiamb.tick();
    nbTicks++ ;
}

// ----------------------------------------------------------------------------
//! setTimeRegulation
void
Billard::setTimeRegulation(bool start_constrained, bool start_regulating)
{
    D.Out(pdInit, "Time Regulation setup");

    if (start_constrained) {
        if (!constrained) {
            rtiamb.enableTimeConstrained();
            constrained = true ;
            D.Out(pdInit, "Time Constrained enabled.");
        }
        rtiamb.modifyLookahead(TIME_STEP);
    }
    else {
        if (constrained) {
            rtiamb.disableTimeConstrained();
            constrained = false ;
            D.Out(pdInit, "Time Constrained disabled.");
        }
    }

    if (start_regulating) {
        if (!regulating) {
            for (;;) {
                rtiamb.queryFederateTime(localTime);

                try {
                    rtiamb.enableTimeRegulation(localTime, TIME_STEP);
                    regulating = true ;
                    break ;
                }
                catch (FederationTimeAlreadyPassed) {
                    // Si Je ne suis pas le premier, je vais les rattraper.
                    rtiamb.queryFederateTime(localTime);

                    RTIfedTime requestTime(((RTIfedTime&)localTime).getTime());
                    // D.Out(pdInit, "localtime %.2f, requestTime %.2f",
                    // ((RTIfedTime&)localTime).getTime(),
                    // ((RTIfedTime&)requestTime).getTime());
                    requestTime += TIME_STEP ;
                    // D.Out(pdInit, "localtime %.2f, requestTime %.2f",
                    // ((RTIfedTime&)localTime).getTime(),
                    // ((RTIfedTime&)requestTime).getTime());
                    rtiamb.timeAdvanceRequest(requestTime);

                    while (!fedamb.timeAdvanceGranted()) {
                        try {
                            tick();
                        }
                        catch (RTIinternalError) {
                            printf("RTIinternalError Raised in tick.\n");
                            exit(-1);
                        }
                    }
                }
                catch (RTIinternalError) {
                    printf("RTIinternalError Raised in setTimeRegulating.\n");
                    exit(-1);
                }
            }
        }
    }
    else {
        if (regulating) {
            rtiamb.disableTimeRegulation();
            regulating = false ;
        }
    }
}

// ----------------------------------------------------------------------------
//! Synchronize with other federates
void
Billard::synchronize(int autostart)
{
    D.Out(pdInit, "Synchronize");

    if (creator) {
        // Wait a signal from user and stop the pause synchronization.

        if (autostart == 0) {
            cout << "Press ENTER to start execution..." << endl ;
            getchar();
        }
        else {
            while (autostart >= 0) {
                sleep(1);
                printf("\rAutostart : %5d", autostart);
                fflush(stdout);
                autostart-- ;
            }
            printf("\n");
        }

        D.Out(pdInit, "Creator can resume execution...");
        while (!fedamb.paused)
            try {
                D.Out(pdInit, "not paused");
                tick();
            }
            catch (Exception& e) {
                D.Out(pdExcept, "******** Exception ticking the RTI : %d ", &e);
            }
        D.Out(pdDebug, "paused");

        try {
            rtiamb.synchronizationPointAchieved("Init");
        }
        catch (Exception& e) {
            D.Out(pdExcept, "**** Exception achieving a synchronization "
                  "point by creator : %d", &e);
        }

        while (fedamb.paused)
            try {
                tick();
            }
            catch (Exception& e) {
                D.Out(pdExcept, "**** Exception ticking the RTI : %d.", &e);
            }
    }
    else {
        if (autostart != 0) {
            printf("Ignoring autostart (federate is not creator)\n");
        }
        printf("Synchronization...\n");

        if (!fedamb.paused) {
            D.Out(pdInit,
                  "Federate not paused: too early");
            while (!fedamb.paused) {
                try {
                    tick();
                }
                catch (Exception& e) {
                    D.Out(pdExcept,
                          "******** Exception ticking the RTI : %d.", &e);
                }
            }
        }
        D.Out(pdInit, "Federate paused");

        try {
            // Federate ends its synchronization.
            rtiamb.synchronizationPointAchieved("Init");
            D.Out(pdInit, "Pause achieved.");
        }
        catch (Exception& e) {
            D.Out(pdExcept,
                  "**** Exception achieving a synchronization point : %d",
                  &e);
        }

        D.Out(pdInit,
              "Federate waiting end of pause...");
        while (fedamb.paused) {
            try {
                tick();
            }
            catch (Exception& e) {
                D.Out(pdExcept, "******** Exception ticking the RTI : %d.", &e);
            }
        }
        D.Out(pdInit, "End of pause");
    }

    D.Out(pdInit, "Federation is synchronized.");

}

// ----------------------------------------------------------------------------
// init ball with seed
void
Billard::init(int seed)
{
    objects.init(seed);
}

// ----------------------------------------------------------------------------
// init ball with these values
void
Billard::init(int x, int y)
{
    objects.init(x, y);
}

// ----------------------------------------------------------------------------
// create objects
void
Billard::create()
{
    objects.create(federateName);
}

// ----------------------------------------------------------------------------
// step (one simulation step advance)
void
Billard::step()
{
    // fedamb->granted = false ;
    rtiamb.queryFederateTime(localTime);

    try {
        RTIfedTime time_aux(localTime.getTime()+TIME_STEP.getTime());

        D.Out(pdDebug, "time_aux : %.2f - localtime : %.2f - "
              "timestep : %.2f", time_aux.getTime(),
              ((RTIfedTime&)localTime).getTime(),
              ((RTIfedTime&)TIME_STEP).getTime());
        rtiamb.timeAdvanceRequest(time_aux);
    }
    catch (Exception& e) {
        D.Out(pdExcept, "******* Exception sur timeAdvanceRequest.");
    }

    while (!fedamb.timeAdvanceGranted()) {
        try {
            tick();
        }
        catch (Exception& e) {
            D.Out(pdExcept, "******** Exception ticking the RTI : %d.", &e);
        }
    }
    try {
        rtiamb.queryFederateTime(localTime);
    }
    catch (Exception& e) {
        D.Out(pdExcept,
              "**** Exception asking for federate local time : ", &e);
    }

    D.Out(pdTrace, "Time advanced, local time is now %.2f.",
          localTime.getTime());

    RTIfedTime next_step(localTime + TIME_STEP);

    objects.erase();
    objects.step(next_step);
    objects.display();
    objects.update(next_step);
}

// ----------------------------------------------------------------------------
// resign
void
Billard::resign()
{
    objects.destroy(localTime);
    D.Out(pdTerm, "Local objects deleted.");

    setTimeRegulation(false, false);

    try {
        rtiamb.resignFederationExecution(
            DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES);
        D.Out(pdTerm, "Just resigned from federation");
    }
    catch (Exception &e) {
        D.Out(pdExcept,
              "** Exception during resignFederationExecution by federate");
    }
    // Detruire la federation

    if (creator) {
        for (;;) {
            tick();
            try {
                D.Out(pdTerm, "Asking from federation destruction...");
                rtiamb.destroyFederationExecution(federationName.c_str());

                D.Out(pdTerm, "Federation destruction granted.");
                break ;
            }
            catch (FederatesCurrentlyJoined) {
                sleep(5);
            }
        }
    }
    D.Out(pdTerm, "Destroying RTIAmbassador and FedAmbassador.");
    D.Out(pdTerm, "Federation terminated.");
}

// $Id: Billard.cc,v 3.1 2003/08/06 14:37:47 breholee Exp $
