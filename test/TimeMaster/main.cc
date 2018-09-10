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
#include <RTI/NullFederateAmbassador.h>
#include <RTI/RTI1516.h>
#include <RTI/RTI1516fedTime.h>

#include "make_unique.hh"

using namespace std;
using namespace rti1516e;

unique_ptr<rti1516e::RTIambassador> ambassador;

class MyFederateAmbassador : public NullFederateAmbassador {
public:
    bool is_constrained{false};
    bool is_regulator{false};
    bool is_granted{false};

    RTI1516fedTime last_time{0.0};

    void timeConstrainedEnabled(const LogicalTime& theFederateTime) throw(FederateInternalError) override
    {
        is_constrained = true;
        last_time = theFederateTime;
    }

    void timeRegulationEnabled(const LogicalTime& theFederateTime) throw(FederateInternalError) override
    {
        is_regulator = true;
        last_time = theFederateTime;
    }

    void
    announceSynchronizationPoint(const std::wstring& label,
                                 const VariableLengthData& /*theUserSuppliedTag*/) throw(FederateInternalError) override
    {
        wcout << "Received synchronization point <" << label << ">, auto achieve" << endl;
        ambassador->synchronizationPointAchieved(label, true);
    }

    void timeAdvanceGrant(const LogicalTime& theTime) throw(FederateInternalError) override
    {
        is_granted = true;
        last_time = theTime;
    }
};

void signalHandler(int /*signal*/)
{
    exit(0);
}

int main(int argc, char** argv)
{
    cout << "== CERTI TIME MASTER ==" << endl;

    std::signal(SIGINT, signalHandler);

    if (argc != 3) {
        cout << "usage: ./time_master federate_name federation_name" << endl;
        return EXIT_FAILURE;
    }

    wstring federate_name(argv[1], argv[1] + strlen(argv[1]));
    wstring federation_name(argv[2], argv[2] + strlen(argv[2]));

    try {
        cout << "  create RTI Ambassador" << endl;

        ambassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        cout << "* Ambassador created" << endl << endl;

        MyFederateAmbassador federate_ambassador;

        ambassador->connect(federate_ambassador, HLA_EVOKED);

        ambassador->joinFederationExecution(federate_name, L"TimeMaster", federation_name, {});

        ambassador->enableTimeConstrained();

        while (!federate_ambassador.is_constrained) {
            ambassador->evokeCallback(0);
        }

        RTI1516fedTimeInterval time_interval{1.0};

        ambassador->enableTimeRegulation(time_interval);

        while (!federate_ambassador.is_regulator) {
            ambassador->evokeCallback(0);
        }

        for (;;) {
            federate_ambassador.is_granted = false;
            ambassador->evokeCallback(0);

            cout << "Press any key to request a time advance" << endl;
            getchar();

            RTI1516fedTime next_time = federate_ambassador.last_time;
            next_time += time_interval;

            ambassador->timeAdvanceRequest(next_time);

            while (!federate_ambassador.is_granted) {
                ambassador->evokeCallback(0);
            }
        }
    }
    catch (rti1516e::Exception& e) {
        wcout << "* Error: " << e.what() << endl;

        return EXIT_FAILURE;
    }
}
