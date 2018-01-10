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

#include <iostream>
#include <memory>

#include <cstring>

#include <unistd.h>

#include <RTI/Enums.h>
#include <RTI/RTI1516.h>
#include <RTI/RTI1516fedTime.h>

#include "billard.hh"

#include "make_unique.hh"

using namespace std;

int main(int argc, char** argv)
{
    cout << "== CERTI MODERN BILLARD ==" << endl;

    if (argc != 3) {
        cout << "usage: ./mom_explorer federate_name federation_name" << endl;
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

        //         int step_count{0};
        //         for (;st; ++step_count) {
        billard.step();
        //         }
    }
    catch (rti1516e::Exception& e) {
        wcout << "* Error: " << e.what() << endl;

        return EXIT_FAILURE;
    }
}
