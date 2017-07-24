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

#ifndef _CERTI_RTIG_FEDERATIONS_LIST_HH
#define _CERTI_RTIG_FEDERATIONS_LIST_HH

#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "BaseRegion.hh"
#include "Exception.hh"
#include "FedTimeD.hh"
#include "certi.hh"

namespace certi {

class AttributeHandleSet;
class AuditFile;
class Extent;
class NM_Join_Federation_Execution;
class SocketServer;
class SocketTCP;

namespace rtig {

class Federation;

/**
 * FederationsList keeps track on federation operations.
 * There is one federation list in each RTIG instance.
 * You usually have only one RTIG instance.
 */
class FederationsList {
public:
    // constructor/destructor
    FederationsList(SocketServer& server, AuditFile& audit);
    ~FederationsList();

// ---------------------------
// -- Federation Management --
// ---------------------------

#ifdef FEDERATION_USES_MULTICAST
    void createFederation(const std::string&, FederationHandle, SocketMC*)
#else
    void createFederation(const std::string&, Handle, const std::string& FEDid)
#endif
        throw(FederationExecutionAlreadyExists,
              CouldNotOpenFED,
              ErrorReadingFED,
              MemoryExhausted,
              SecurityError,
              RTIinternalError);

    /** Return the Handle of the Federation named "name" if it is found in the
     * FederationList, else throw FederationExecutionDoesNotExist.
     */
    Handle getFederationHandle(const std::string& name) throw(FederationExecutionDoesNotExist);

    void destroyFederation(Handle) throw(FederatesCurrentlyJoined, FederationExecutionDoesNotExist, RTIinternalError);

#ifdef FEDERATION_USES_MULTICAST
    void info(FederationHandle theHandle, int& nb_federes, int& nb_regulateurs, bool& is_syncing, SocketMC*& comm_mc)
#else
    void info(Handle theHandle, int& nb_federes, int& nb_regulateurs, bool& is_syncing)
#endif
        throw(FederationExecutionDoesNotExist, RTIinternalError);

    // -------------------------
    // -- Federate Management --
    // -------------------------

    /** Creates a new Federate if its name is unique, and returns its new
     * Federate Handle. Also sends Null messages from all other federates
     * to initialize its LBTS, and finally a RequestPause message if the
     * Federation is already paused.
     */
    FederateHandle addFederate(Handle theHandle,
                               const std::string& theFederateName,
                               SocketTCP* theTCPLink,
                               NM_Join_Federation_Execution& objectModelData) throw(FederationExecutionDoesNotExist,
                                                                                    FederateAlreadyExecutionMember,
                                                                                    MemoryExhausted,
                                                                                    RTIinternalError);

    /** This Method tries to remove all references to this Federate in the
     * Federation. To be used when a Federate is supposed to have crashed.
     */
    void killFederate(Handle, FederateHandle) noexcept;

    void setVerboseLevel(int theVerboseLevel);

    /** Search federation from handle.
     * 
     * @param[in] federationHandle the handle of the search federation
     * @throw FederationExecutionDoesNotExist if the provided federation handle
     *        does not match any created federation.
     */
    Federation& searchFederation(Handle federationHandle) throw(FederationExecutionDoesNotExist);

private:
    SocketServer& socketServer;
    AuditFile& auditFile;

    int verboseLevel;
    
    struct FederationComparator {
        using is_transparent = void;
        
        bool operator()(const std::unique_ptr<Federation>& lhs, const std::unique_ptr<Federation>& rhs) const;
        bool operator()(const std::unique_ptr<Federation>& lhs, const FederationHandle rhsHandle) const;
        bool operator()(const FederationHandle lhsHandle, const std::unique_ptr<Federation>& rhs) const;
        bool operator()(const std::unique_ptr<Federation>& lhs, const std::string& rhsName) const;
        bool operator()(const std::string& lhsName, const std::unique_ptr<Federation>& rhs) const;
    };

//     typedef std::map<Handle, Federation*> HandleFederationMap;
//     HandleFederationMap _handleFederationMap;
    std::set<std::unique_ptr<Federation>, FederationComparator> my_federations;
};
}
} // namespace certi/rtig

#endif // _CERTI_RTIG_FEDERATIONS_LIST_HH
