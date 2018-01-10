// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// ----------------------------------------------------------------------------

#include <cstring>
#include <csignal>

#include <iostream>
#include <memory>
#include <chrono>

#include <RTI/Enums.h>
#include <RTI/RTI1516.h>
#include <RTI/RTI1516fedTime.h>

#include "make_unique.hh"

#include "billard.hh"


using namespace std;

static bool loop_state{false};

void signalHandler(int /*signal*/)
{
    if(loop_state) {
        loop_state = false;
    }
    else {
        std::exit(0);
    }
}

int main(int argc, char** argv)
{
    cout << "== CERTI MODERN BILLARD ==" << endl;
    
    std::signal(SIGINT, signalHandler);

    if (argc != 3) {
        cout << "usage: ./modern_billard federate_name federation_name" << endl;
        return EXIT_FAILURE;
    }

    wstring federate_name(argv[1], argv[1] + strlen(argv[1]));
    wstring federation_name(argv[2], argv[2] + strlen(argv[2]));

    try {
        cout << "  create RTI Ambassador" << endl;

        unique_ptr<rti1516e::RTIambassador> ambassador;
        ambassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        cout << "* Ambassador created" << endl << endl;

        Billard billard(*ambassador, federation_name, federate_name);

        billard.createOrJoin();

        if (billard.isCreator()) {
            billard.pause(L"Init");

            cout << "Press ENTER when all federates have joined" << endl;
            getchar();
        }
        else {
            billard.enableCollisions();

            billard.pause(L"NotAlone");
            billard.synchronize(L"NotAlone");
        }

        billard.synchronize(L"Init");
        billard.waitForSynchronization(L"Init");

        if (billard.isCreator() && billard.hasSynchronizationPending(L"NotAlone")) {
            billard.enableCollisions();

            billard.synchronize(L"NotAlone");
            billard.waitForSynchronization(L"NotAlone");
        }

        billard.publishAndSubscribe();

        billard.enableTimeRegulation();

        billard.tick();

        billard.init();

        billard.declare();

        if (billard.isCreator()) {
            billard.pause(L"Start");
        }

        billard.synchronize(L"Start");
        billard.waitForSynchronization(L"Start");

        int step_count{0};
        auto time_point = std::chrono::system_clock::now();
        for (loop_state = true; loop_state; ++step_count) {
            billard.step();
            
            if(step_count % 500 == 0) {
                auto now = std::chrono::system_clock::now();
                wcout << "steps per second: " << 5.0e11 /  std::chrono::duration_cast<std::chrono::nanoseconds>(now-time_point).count() << endl;
                time_point = now;
            }
            
//             if(step_count == 20) {
//                 loop_state = false;
//             }
        }
    }
    catch (rti1516e::Exception& e) {
        wcout << "* Error: " << e.what() << endl;

        return EXIT_FAILURE;
    }
}
