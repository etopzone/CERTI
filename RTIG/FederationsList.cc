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
#include "Federation.hh"
#include "PrettyDebug.hh"
#include <ostream>
#include <utility>

using std::endl;

namespace certi {

class AttributeHandleSet;
class Extent;
class NM_Join_Federation_Execution;
class SocketServer;
class SocketTCP;

namespace rtig {

static PrettyDebug D("FEDERATIONSLIST", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);

FederationsList::FederationsList(SocketServer& server, AuditFile& audit)
    : socketServer(server), auditFile(audit), verboseLevel(0)
{
}

FederationsList::~FederationsList()
{
    if (!_handleFederationMap.empty())
        D.Out(pdError, "ListeFederation not empty at destruction time.");

    for (HandleFederationMap::const_iterator i = _handleFederationMap.begin(); i != _handleFederationMap.end(); ++i) {
        delete i->second;
    }
}

FederateHandle
FederationsList::addFederate(Handle federationHandle,
                             const std::string& name,
                             SocketTCP* tcp_link,
                             NM_Join_Federation_Execution& objectModelData) throw(FederationExecutionDoesNotExist,
                                                                                  FederateAlreadyExecutionMember,
                                                                                  MemoryExhausted,
                                                                                  RTIinternalError)
{
    G.Out(pdGendoc, "enter FederationsList::addFederate");

    // It may throw FederationExecutionDoesNotExist
    // Return  federation address giving its handle
    Federation* federation = searchFederation(federationHandle);

    // It may raise a bunch of exceptions
    // adding the federate and return its handle
    FederateHandle federate = federation->add(name, tcp_link);

    federation->getFOM(objectModelData);

    G.Out(pdGendoc, "exit FederationsList::addFederate");

    return federate;
}

Federation* FederationsList::searchFederation(Handle federationHandle) throw(FederationExecutionDoesNotExist)
{
    HandleFederationMap::const_iterator i = _handleFederationMap.find(federationHandle);
    if (i != _handleFederationMap.end()) {
        return i->second;
    }

    D.Out(pdExcept, "Unknown Federation Handle %d.", federationHandle);
    throw FederationExecutionDoesNotExist("Bad Federation Handle.");
}

#ifdef FEDERATION_USES_MULTICAST
/** createFederation (with FEDERATION_USES_MULTICAST defined)
/   @param name Federation name
    @param handle Federation handle
    @param mc_link
*/
void FederationsList::createFederation(const std::string& name, Handle federationHandle, SocketMC* mc_link)
#else
/** createFederation (with FEDERATION_USES_MULTICAST not defined)
    @param name Federation name
    @param handle Federation handle
    @param FEDid execution id. of the federation (i.e. file name)
*/
void FederationsList::createFederation(const std::string& name, Handle federationHandle, const std::string& FEDid)
#endif
    throw(FederationExecutionAlreadyExists,
          CouldNotOpenFED,
          ErrorReadingFED,
          MemoryExhausted,
          SecurityError,
          RTIinternalError)
{
    G.Out(pdGendoc, "enter FederationsList::createFederation");
    auditFile << ", Handle : " << federationHandle;
    if (name.empty())
        throw RTIinternalError("Invalid Federation Name.");

    // It should throw FederationExecutionDoesNotExist.
    try {
        getFederationHandle(name);
        D.Out(pdExcept, "Federation %s already present.", name.c_str());
        G.Out(pdGendoc, "exit  FederationsList::createFederation on exception");
        throw FederationExecutionAlreadyExists(name);
    }
    catch (FederationExecutionDoesNotExist& e) {
        D.Out(pdDebug, "CreerFederation catches FederationExecutionDoesNotExist.");
    }

    Federation* federation = nullptr;
#ifdef FEDERATION_USES_MULTICAST
    federation = new Federation(name, federationHandle, socketServer, auditFile, mc_link, verboseLevel);
#else
    try {
        federation = new Federation(name, federationHandle, socketServer, auditFile, FEDid, verboseLevel);
        D.Out(pdDebug, "new Federation created.");
    }
    catch (CouldNotOpenFED& e) {
        D.Out(pdInit, "Federation constructor : Could not open FED file.");
        G.Out(pdGendoc, "exit FederationsList::createFederation on exception CouldNotOpenFED");
        throw CouldNotOpenFED(e.reason());
    }
    catch (ErrorReadingFED& e) {
        D.Out(pdInit, "Federation constructor : Could not read FED file (maybe incorrect).");
        G.Out(pdGendoc, "exit FederationsList::createFederation on exception ErrorReadingFED");
        throw ErrorReadingFED(e.reason());
    }

#endif
    if (federation == nullptr)
        throw MemoryExhausted("No memory left for new Federation.");

    _handleFederationMap[federationHandle] = federation;
    D.Out(pdInit, "New Federation created with Handle %d.", federationHandle);

    G.Out(pdGendoc, "exit FederationsList::createFederation");
}

Handle FederationsList::getFederationHandle(const std::string& name) throw(FederationExecutionDoesNotExist)
{
    G.Out(pdGendoc, "enter FederationsList::getFederationHandle");

    for (HandleFederationMap::const_iterator i = _handleFederationMap.begin(); i != _handleFederationMap.end(); ++i) {
        if (i->second->getName() == name) {
            G.Out(pdGendoc, "exit  FederationsList::getFederationHandle");
            return i->second->getHandle();
        }
    }
    G.Out(pdGendoc, "exit  FederationsList::getFederationHandle on exception");

    D.Out(pdDebug, "getFederationHandle throws FederationExecutionDoesNotExist.");
    throw FederationExecutionDoesNotExist(name);
}

#ifdef FEDERATION_USES_MULTICAST
void FederationsList::info(
    Handle federationHandle, int& nb_federates, int& nb_regulators, bool& is_syncing, SocketMC*& comm_mc)
#else
void FederationsList::info(Handle federationHandle, int& nb_federates, int& nb_regulators, bool& is_syncing)
#endif
    throw(FederationExecutionDoesNotExist, RTIinternalError)
{
    G.Out(pdGendoc, "enter FederationsList::info");

    // It may throw FederationExecutionNotFound
    Federation* federation = searchFederation(federationHandle);

    nb_federates = federation->getNbFederates();
    nb_regulators = federation->getNbRegulators();
    is_syncing = federation->isSynchronizing();
#ifdef FEDERATION_USES_MULTICAST
    comm_mc = federation->MCLink;
#endif
    G.Out(pdGendoc, "exit  FederationsList::info");
}

void FederationsList::subscribeObject(
    Handle federationHandle,
    FederateHandle federate,
    ObjectClassHandle object_class,
    const std::vector<AttributeHandle>& attributes) throw(ObjectClassNotDefined,
                                                          AttributeNotDefined,
                                                          FederationExecutionDoesNotExist,
                                                          FederateNotExecutionMember,
                                                          SaveInProgress,
                                                          RestoreInProgress,
                                                          SecurityError,
                                                          RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation* federation = searchFederation(federationHandle);

    federation->subscribeObject(federate, object_class, attributes);
}

void FederationsList::destroyFederation(Handle federationHandle) throw(FederatesCurrentlyJoined,
                                                                       FederationExecutionDoesNotExist,
                                                                       RTIinternalError)
{
    G.Out(pdGendoc, "enter FederationsList::destroyFederation");

    // It may throw :
    // FederationExecutionDoesNotExist during search federation
    Federation* federation = searchFederation(federationHandle);

    // It may throw FederatesCurrentlyJoined if federation not empty (in empty)
    if (federation->empty()) {
        _handleFederationMap.erase(_handleFederationMap.find(federationHandle));
        delete federation;
    }
    G.Out(pdGendoc, "exit FederationsList::destroyFederation");
}

void FederationsList::killFederate(Handle federationHandle, FederateHandle federate) noexcept
{
    try {
        searchFederation(federationHandle)->kill(federate);
    }
    catch (Exception& e) {
        // It may have thrown FederationExecutionDoesNotExist
        return;
    }
}

void FederationsList::setVerboseLevel(int theVerboseLevel)
{
    this->verboseLevel = theVerboseLevel;
}
}
} // certi::rtig
