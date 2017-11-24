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

#include "momfederateambassador.h"

#include "make_unique.hh"

#ifdef _WIN32
#include <windows.h>
#define    sleep(a)              Sleep(a * 1000)
#define    usleep(a)             Sleep(a / 1000)
#endif

using namespace std;

int main(int argc, char** argv)
{
    cout << "== CERTI MOM EXPLORER ==" << endl;

    if (argc != 5) {
        cout << "usage: ./mom_explorer federate_name {create,join} federation_name {auto,manual}" << endl;
        return EXIT_FAILURE;
    }

    wstring federate_name(argv[1], argv[1] + strlen(argv[1]));
    wstring action(argv[2], argv[2] + strlen(argv[2]));
    wstring federation_name(argv[3], argv[3] + strlen(argv[3]));
    wstring mode(argv[4], argv[4] + strlen(argv[4]));

    try {
        auto amb_factory = make_unique<rti1516e::RTIambassadorFactory>();

        cout << "  create RTI Ambassador" << endl;

        unique_ptr<rti1516e::RTIambassador> ambassador;
        ambassador = amb_factory->createRTIambassador();
        cout << "* Ambassador created" << endl << endl;

        auto fed_ambassador = make_unique<MOMFederateAmbassador>(*ambassador, federation_name, federate_name);

        fed_ambassador->connect();

        if (action == L"create") {
            fed_ambassador->createFederationExecution();
        }

        fed_ambassador->joinFederationExecution();

        fed_ambassador->subscribeObjectClasses();

        fed_ambassador->publishAndsubscribeInteractions();

        cout << "# " << endl;

        if (mode == L"auto") {
            while (true) {
                ambassador->evokeCallback(0.1);
                sleep(1);
            }
        }
        else {
            string request;
            while (getline(cin, request)) {
                std::cerr << request << std::endl;
                try {
                    if (request == "t") {
                        wcout << "=>evokeCallback(0.1)" << endl;
                        ambassador->evokeCallback(0.1);
                    }
                    else if (request == "resign") {
                        cout << "=>resignFederationExecution(NO_ACTION)" << endl;
                        ambassador->resignFederationExecution(NO_ACTION);
                    }
                    else if (request == "destroy") {
                        wcout << "=>destroyFederationExecution <" << federation_name << ">" << endl;
                        ambassador->destroyFederationExecution(federation_name);

                        return EXIT_SUCCESS;
                    }
                    else if (request == "etr") {
                        wcout << "=>enableTimeRegulation" << endl;
                        ambassador->enableTimeRegulation(RTI1516fedTimeInterval(1.0));
                    }
                    else if (request == "etc") {
                        wcout << "=>enableTimeConstrained" << endl;
                        ambassador->enableTimeConstrained();
                    }
                    else if (request == "dtr") {
                        wcout << "=>disableTimeRegulation" << endl;
                        ambassador->disableTimeRegulation();
                    }
                    else if (request == "dtc") {
                        wcout << "=>disableTimeConstrained" << endl;
                        ambassador->disableTimeConstrained();
                    }
                    else if (request == "req") {
                        wcout << "=>request" << endl;
                        std::string tag;
                        
                        wcout << "\tRequest ? " << endl;
                        getline(cin, request);
                        if (request == "publications") {
                            wcout << "\tFederate Handle ? " << endl;
                            getline(cin, request);

                            int handle = std::atoi(request.c_str());

                            ambassador->sendInteraction(
                                ambassador->getInteractionClassHandle(
                                    L"HLAmanager.HLAfederate.HLArequest.HLArequestPublications"),
                                {{ambassador->getParameterHandle(
                                      ambassador->getInteractionClassHandle(L"HLAmanager.HLAfederate"), L"HLAfederate"),
                                  {&handle, 4}}},
                                {tag.c_str(), tag.size()});
                        }
                        else if (request == "sps") {
                            wcout << "\tSync point name ? " << endl;
                            getline(cin, request);

                            ambassador->sendInteraction(
                                ambassador->getInteractionClassHandle(
                                    L"HLAmanager.HLAfederation.HLArequest.HLArequestSynchronizationPointStatus"),
                                {{ambassador->getParameterHandle(
                                      ambassador->getInteractionClassHandle(L"HLAmanager.HLAfederation.HLArequest.HLArequestSynchronizationPointStatus"), L"HLAsyncPointName"),
                                  {request.c_str(), request.size()}}},
                                {tag.c_str(), tag.size()});
                        }
                        else {
                            wcout << "\tUnknown request" << endl;
                        }
                    }
                    else {
                        wcout << "**unknown command**" << endl;
                    }

                    cout << endl << "# " << endl;
                }
                catch (rti1516e::Exception& e) {
                    wcout << "* Error: " << e.what() << endl;

                    return EXIT_FAILURE;
                }
            }
        }
    }
    catch (rti1516e::Exception& e) {
        wcout << "* Error: " << e.what() << endl;

        return EXIT_FAILURE;
    }
}
