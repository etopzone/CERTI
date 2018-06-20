// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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

#include <csignal>
#include <cstring>

#include <chrono>
#include <iostream>
#include <memory>

#include <RTI/Enums.h>
#include <RTI/RTI1516.h>
#include <RTI/RTI1516fedTime.h>

#include "billard.hh"

using namespace std;

static bool loop_state{false};

void signalHandler(int /*signal*/)
{
    if (loop_state) {
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
        Billard billard(federation_name, federate_name, loop_state);
    }
    catch (rti1516e::Exception& e) {
        wcout << "* Error: " << e.what() << endl;

        return EXIT_FAILURE;
    }
}
