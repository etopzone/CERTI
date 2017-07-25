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
// ----------------------------------------------------------------------------

#include "FederationsList.hh"

#include "AuditFile.hh"
#include "Exception.hh"
#include "Federation.hh"
#include "PrettyDebug.hh"
#include <ostream>
#include <utility>

using std::endl;

namespace certi {

class NM_Join_Federation_Execution;
class SocketServer;
class SocketTCP;

namespace rtig {

static PrettyDebug D("FEDERATIONSLIST", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);

FederationsList::FederationsList(SocketServer& server, AuditFile& audit, const int verboseLevel)
    : my_socket_server(server), my_audit_file(audit), my_verbose_level(verboseLevel)
{
}

FederationsList::~FederationsList()
{
    if (!my_federations.empty()) {
        Debug(D, pdError) << "ListeFederation not empty at destruction time" << std::endl;
    }
}

void FederationsList::setVerboseLevel(const int verboseLevel)
{
    my_verbose_level = verboseLevel;
}

#ifdef FEDERATION_USES_MULTICAST
void FederationsList::createFederation(const std::string& name,
                                       const FederationHandle handle,
                                       SocketMC* multicastSocket)
#else
void FederationsList::createFederation(const std::string& name, const FederationHandle handle, const std::string& FEDid)
#endif
{
    Debug(G, pdGendoc) << "enter FederationsList::createFederation" << std::endl;
    my_audit_file << ", Handle : " << handle;
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
        Debug(D, pdDebug) << "CreerFederation catches FederationExecutionDoesNotExist" << std::endl;
    }

    try {
#ifdef FEDERATION_USES_MULTICAST
        auto federation = std::make_unique<Federation>(
            name, handle, my_socket_server, my_audit_file, multicastSocket, my_verbose_level);
#else
        auto federation
            = std::make_unique<Federation>(name, handle, my_socket_server, my_audit_file, FEDid, my_verbose_level);
#endif
        Debug(D, pdDebug) << "new Federation created" << std::endl;

        auto result = my_federations.insert(std::move(federation)).second;
        if (!result) {
            throw FederationExecutionAlreadyExists(name);
        }

        Debug(D, pdInit) << "New Federation created with Handle" << handle << std::endl;

        Debug(G, pdGendoc) << "exit FederationsList::createFederation" << std::endl;
    }
    catch (CouldNotOpenFED& e) {
        Debug(D, pdInit) << "Federation constructor : Could not open FED file" << std::endl;
        Debug(G, pdGendoc) << "exit FederationsList::createFederation on exception CouldNotOpenFED" << std::endl;
        throw CouldNotOpenFED(e.reason());
    }
    catch (ErrorReadingFED& e) {
        Debug(D, pdInit) << "Federation constructor : Could not read FED file (maybe incorrect)" << std::endl;
        Debug(G, pdGendoc) << "exit FederationsList::createFederation on exception ErrorReadingFED" << std::endl;
        throw ErrorReadingFED(e.reason());
    }
}

Handle FederationsList::getFederationHandle(const std::string& name)
{
    Debug(G, pdGendoc) << "enter FederationsList::getFederationHandle" << std::endl;

    auto it = my_federations.find(name);

    if (it == end(my_federations)) {
        Debug(G, pdGendoc) << "exit  FederationsList::getFederationHandle on exception" << std::endl;

        Debug(D, pdDebug) << "getFederationHandle throws FederationExecutionDoesNotExist" << std::endl;
        throw FederationExecutionDoesNotExist(name);
    }

    Debug(G, pdGendoc) << "exit  FederationsList::getFederationHandle" << std::endl;
    return (*it)->getHandle();
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

#ifdef FEDERATION_USES_MULTICAST
void FederationsList::info(const FederationHandle handle,
                           int& out_federatesCount,
                           int& out_regulatorsCount,
                           bool& out_isSyncing,
                           SocketMC*& out_multicastSocket)
#else
void FederationsList::info(const FederationHandle handle,
                           int& out_federatesCount,
                           int& out_regulatorsCount,
                           bool& out_isSyncing)
#endif
{
    Debug(G, pdGendoc) << "enter FederationsList::info" << std::endl;

    // It may throw FederationExecutionNotFound
    auto& federation = searchFederation(handle);

    out_federatesCount = federation.getNbFederates();
    out_regulatorsCount = federation.getNbRegulators();
    out_isSyncing = federation.isSynchronizing();
#ifdef FEDERATION_USES_MULTICAST
    out_multicastSocket = federation.MCLink;
#endif
    Debug(G, pdGendoc) << "exit  FederationsList::info" << std::endl;
}

FederateHandle FederationsList::addFederate(const FederationHandle handle,
                                            const std::string& federateName,
                                            SocketTCP* federateTcpLink,
                                            NM_Join_Federation_Execution& objectModelData)
{
    Debug(G, pdGendoc) << "enter FederationsList::addFederate" << std::endl;

    // It may throw FederationExecutionDoesNotExist
    // Return  federation address giving its handle
    auto& federation = searchFederation(handle);

    // It may raise a bunch of exceptions
    // adding the federate and return its handle
    FederateHandle federate = federation.add(federateName, federateTcpLink);

    federation.getFOM(objectModelData);

    Debug(G, pdGendoc) << "exit FederationsList::addFederate" << std::endl;

    return federate;
}

void FederationsList::killFederate(const FederationHandle federation, const FederateHandle federate) noexcept
{
    try {
        searchFederation(federation).kill(federate);
    }
    catch (Exception& e) {
        // It may have thrown FederationExecutionDoesNotExist
        return;
    }
}

Federation& FederationsList::searchFederation(const FederationHandle handle)
{
    auto it = my_federations.find(handle);

    if (it == end(my_federations)) {
        Debug(D, pdExcept) << "Unknown Federation Handle " << handle << std::endl;
        throw FederationExecutionDoesNotExist("Bad Federation Handle.");
    }

    return **it;
}

bool FederationsList::FederationComparator::operator()(const std::unique_ptr<Federation>& lhs,
                                                       const std::unique_ptr<Federation>& rhs) const
{
    return lhs->getHandle() < rhs->getHandle();
}

bool FederationsList::FederationComparator::operator()(const std::unique_ptr<Federation>& lhs,
                                                       const FederationHandle rhsHandle) const
{
    return lhs->getHandle() < rhsHandle;
}

bool FederationsList::FederationComparator::operator()(const FederationHandle lhsHandle,
                                                       const std::unique_ptr<Federation>& rhs) const
{
    return lhsHandle < rhs->getHandle();
}

bool FederationsList::FederationComparator::operator()(const std::unique_ptr<Federation>& lhs,
                                                       const std::string& rhsName) const
{
    return lhs->getName() < rhsName;
}

bool FederationsList::FederationComparator::operator()(const std::string& lhsName,
                                                       const std::unique_ptr<Federation>& rhs) const
{
    return lhsName < rhs->getName();
}
}
} // certi::rtig
