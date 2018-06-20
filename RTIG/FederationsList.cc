// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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
// ----------------------------------------------------------------------------

#include "FederationsList.hh"

#include <include/certi.hh>

#include <algorithm>
#include <ostream>
#include <utility>

#include <libCERTI/AuditFile.hh>
#include <libCERTI/Exception.hh>
#include <libCERTI/NM_Classes.hh>
#include <libCERTI/PrettyDebug.hh>

#include "Federation.hh"
#include "make_unique.hh"

using std::endl;

namespace certi {

class SocketServer;
class SocketTCP;

namespace rtig {

static PrettyDebug D("FEDERATIONSLIST", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);

FederationsList::FederationsList(const int verboseLevel) noexcept : my_verbose_level(verboseLevel)
{
}

FederationsList::~FederationsList()
{
    if (!my_federations.empty()) {
        Debug(D, pdError) << "ListeFederation not empty at destruction time" << std::endl;
    }
}

int FederationsList::getVerboseLevel() const noexcept
{
    return my_verbose_level;
}

void FederationsList::setVerboseLevel(const int verboseLevel) noexcept
{
    my_verbose_level = verboseLevel;
}

#ifdef FEDERATION_USES_MULTICAST
std::unique_ptr<NM_Create_Federation_Execution> FederationsList::createFederation(const std::string& name,
                                                                                  const FederationHandle handle,
                                                                                  SocketServer& socket_server,
                                                                                  AuditFile& audit,
                                                                                  const std::vector<std::string> fom_modules,
                                                                                  const std::string& mim_module,
                                                                                  const RtiVersion rti_version,
                                                                                  SocketMC* multicastSocket)
#else
std::unique_ptr<NM_Create_Federation_Execution> FederationsList::createFederation(const std::string& name,
                                                                                  const FederationHandle handle,
                                                                                  SocketServer& socket_server,
                                                                                  AuditFile& audit,
                                                                                  const std::vector<std::string> fom_modules,
                                                                                  const std::string& mim_module,
                                                                                  const RtiVersion rti_version)
#endif
{
    Debug(G, pdGendoc) << "enter FederationsList::createFederation" << std::endl;
    audit << ", Handle : " << handle;
    if (name.empty()) {
        throw RTIinternalError("Invalid Federation Name.");
    }

    // It should throw FederationExecutionDoesNotExist.
    try {
        getFederationHandle(name);
        Debug(D, pdExcept) << "Federation " << name << " already present" << std::endl;
        Debug(G, pdGendoc) << "exit  FederationsList::createFederation on exception" << std::endl;
        throw FederationExecutionAlreadyExists(name);
    }
    catch (FederationExecutionDoesNotExist& e) {
        Debug(D, pdDebug) << "CreateFederation catches FederationExecutionDoesNotExist" << std::endl;
    }

    try {
        auto federation
#ifdef FEDERATION_USES_MULTICAST
            = make_unique<Federation>(name, handle, socket_server, audit, fom_modules, mim_module, rti_version, multicastSocket, my_verbose_level);
#else
            = make_unique<Federation>(name, handle, socket_server, audit, fom_modules, mim_module, rti_version, my_verbose_level);
#endif
        Debug(D, pdDebug) << "new Federation created" << std::endl;

        // Federation's existence has already been checked above, no need to check insert's result
        (void) my_federations.insert(std::make_pair(handle, std::move(federation))).second;

        Debug(D, pdInit) << "New Federation created with Handle" << handle << std::endl;

        Debug(G, pdGendoc) << "exit FederationsList::createFederation" << std::endl;
    }
    catch (Exception& e) {
        Debug(D, pdInit) << "FederationsList could not create Federation : " << e.name() << " - " << e.reason()
                         << std::endl;
        Debug(G, pdGendoc) << "exit FederationsList::createFederation on exception" << std::endl;
        throw;
    }

    auto rep = make_unique<NM_Create_Federation_Execution>();

    rep->setFederation(handle.get());
    rep->setFomModuleDesignatorsSize(fom_modules.size());
    auto i = 0;
    for(const auto& module: fom_modules) {
        rep->setFomModuleDesignators(module, i++);
    }
    rep->setMimDesignator(mim_module);
    rep->setFederationExecutionName(name);

    return rep;
}

FederationHandle FederationsList::getFederationHandle(const std::string& name)
{
    Debug(G, pdGendoc) << "enter FederationsList::getFederationHandle" << std::endl;

    auto it = std::find_if(begin(my_federations),
                           end(my_federations),
                           [&name](decltype(my_federations)::value_type& kv) { return kv.second->getName() == name; });

    if (it == end(my_federations)) {
        Debug(G, pdGendoc) << "exit  FederationsList::getFederationHandle on exception" << std::endl;

        Debug(D, pdDebug) << "getFederationHandle throws FederationExecutionDoesNotExist" << std::endl;
        throw FederationExecutionDoesNotExist(name);
    }

    Debug(G, pdGendoc) << "exit  FederationsList::getFederationHandle" << std::endl;
    return it->first;
}

void FederationsList::destroyFederation(const FederationHandle handle)
{
    Debug(G, pdGendoc) << "enter FederationsList::destroyFederation" << std::endl;

    // It may throw :
    // FederationExecutionDoesNotExist during search federation
    auto& federation = searchFederation(handle);

    // It may throw FederatesCurrentlyJoined if federation not empty (in empty)
    if (federation.empty()) {
        my_federations.erase(my_federations.find(handle));
    }
    Debug(G, pdGendoc) << "exit FederationsList::destroyFederation" << std::endl;
}

Responses FederationsList::killFederate(const FederationHandle federation, const FederateHandle federate) noexcept
{
    try {
        return searchFederation(federation).kill(federate);
    }
    catch (Exception& e) {
        // It may have thrown FederationExecutionDoesNotExist
        return Responses();
    }
}

Federation& FederationsList::searchFederation(const FederationHandle handle)
{
    auto it = my_federations.find(handle);

    if (it == end(my_federations)) {
        Debug(D, pdExcept) << "Unknown Federation Handle " << handle << std::endl;
        throw FederationExecutionDoesNotExist("Bad Federation Handle.");
    }

    return *it->second;
}
}
} // certi::rtig
