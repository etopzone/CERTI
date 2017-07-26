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

#include <memory>
#include <string>
#include <unordered_map>

#include "certi.hh"

namespace certi {

class AuditFile;
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
    FederationsList(SocketServer& server, AuditFile& audit, const int verboseLevel = 0) noexcept;
    ~FederationsList();

    int getVerboseLevel() const noexcept;
    void setVerboseLevel(const int verboseLevel) noexcept;

// ---------------------------
// -- Federation Management --
// ---------------------------

#ifdef FEDERATION_USES_MULTICAST
    /** createFederation (with FEDERATION_USES_MULTICAST defined)
     /   @p*aram name Federation name
     @param handle Federation handle
     @param mc_link
     */
    void createFederation(const std::string& name, const FederationHandle handle, SocketMC* multicastSocket);
#else
    /** createFederation (with FEDERATION_USES_MULTICAST not defined)
     @ p*aram name Federation name
     @param handle Federation handle
     @param FEDid execution id. of the federation (i.e. file name)
     */
    void createFederation(const std::string& name, const FederationHandle handle, const std::string& FEDid);
#endif

    /** Return the Handle of the Federation named "name" if it is found in the
     * FederationList, else throw FederationExecutionDoesNotExist.
     */
    Handle getFederationHandle(const std::string& name);

    void destroyFederation(const FederationHandle handle);

#ifdef FEDERATION_USES_MULTICAST
    void info(const FederationHandle handle,
              int& out_federatesCount,
              int& out_regulatorsCount,
              bool& out_isSyncing,
              SocketMC*& out_multicastSocket);
#else
    void info(const FederationHandle handle, int& out_federatesCount, int& out_regulatorsCount, bool& out_isSyncing);
#endif

    // -------------------------
    // -- Federate Management --
    // -------------------------

    /** Creates a new Federate if its name is unique, and returns its new
     * Federate Handle. Also sends Null messages from all other federates
     * to initialize its LBTS, and finally a RequestPause message if the
     * Federation is already paused.
     */
    FederateHandle addFederate(const FederationHandle handle,
                               const std::string& federateName,
                               SocketTCP* federateTcpLink,
                               NM_Join_Federation_Execution& objectModelData);

    /** This Method tries to remove all references to this Federate in the
     * Federation. To be used when a Federate is supposed to have crashed.
     */
    void killFederate(const FederationHandle federation, const FederateHandle federate) noexcept;

    /** Search federation from handle.
     * 
     * @param[in] handle the handle of the search federation
     * @throw FederationExecutionDoesNotExist if the provided federation handle
     *        does not match any created federation.
     */
    Federation& searchFederation(const FederationHandle handle);

private:
    SocketServer& my_socket_server;
    AuditFile& my_audit_file;

    int my_verbose_level;

    std::unordered_map<FederationHandle, std::unique_ptr<Federation>> my_federations;
};
}
} // namespace certi/rtig

#endif // _CERTI_RTIG_FEDERATIONS_LIST_HH
