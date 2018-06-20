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

#ifndef _CERTI_RTIG_FEDERATIONS_LIST_HH
#define _CERTI_RTIG_FEDERATIONS_LIST_HH

#include <memory>
#include <string>
#include <unordered_map>

#include <libCERTI/Handle.hh>
#include <libCERTI/MessageEvent.hh>

namespace certi {

class AuditFile;
class NM_Join_Federation_Execution;
class SocketServer;
class SocketTCP;
class NM_Create_Federation_Execution;

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
    FederationsList(const int verboseLevel = 0) noexcept;
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
    std::unique_ptr<NM_Create_Federation_Execution> createFederation(const std::string& name,
                                                                     const FederationHandle handle,
                                                                     SocketServer& socket_server,
                                                                     AuditFile& audit,
                                                                     const std::vector<std::string> fom_modules,
                                                                     const std::string& mim_module,
                                                                     const RtiVersion rti_version,
                                                                     SocketMC* multicastSocket);
#else
    /** createFederation (with FEDERATION_USES_MULTICAST not defined)
     @ p*aram name Federation name
     @param handle Federation handle
     @param FEDid execution id. of the federation (i.e. file name)
     */
    std::unique_ptr<NM_Create_Federation_Execution> createFederation(const std::string& name,
                                                                     const FederationHandle handle,
                                                                     SocketServer& socket_server,
                                                                     AuditFile& audit,
                                                                     const std::vector<std::string> fom_modules,
                                                                     const std::string& mim_module,
                                                                     const RtiVersion rti_version);
#endif

    /** Return the Handle of the Federation named "name" if it is found in the
     * FederationList, else throw FederationExecutionDoesNotExist.
     */
    FederationHandle getFederationHandle(const std::string& name);

    void destroyFederation(const FederationHandle handle);

    // -------------------------
    // -- Federate Management --
    // -------------------------

    /** This Method tries to remove all references to this Federate in the
     * Federation. To be used when a Federate is supposed to have crashed.
     */
    Responses killFederate(const FederationHandle federation, const FederateHandle federate) noexcept;

    /** Search federation from handle.
     * 
     * @param[in] handle the handle of the search federation
     * @throw FederationExecutionDoesNotExist if the provided federation handle
     *        does not match any created federation.
     */
    Federation& searchFederation(const FederationHandle handle);

private:
    int my_verbose_level;

    std::unordered_map<FederationHandle, std::unique_ptr<Federation>> my_federations;
};
}
} // namespace certi/rtig

#endif // _CERTI_RTIG_FEDERATIONS_LIST_HH
