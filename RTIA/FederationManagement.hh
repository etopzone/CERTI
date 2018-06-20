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

#ifndef _CERTI_RTIA_FEDERATION_MANAGEMENT
#define _CERTI_RTIA_FEDERATION_MANAGEMENT

#include <set>
#include <string>

#include <include/certi.hh>

#include <libCERTI/NetworkMessage.hh>

#include "Communications.hh"
#include "Statistics.hh"

namespace certi {

class RootObject;

namespace rtia {

class TimeManagement;

class FederationManagement {
public:

    /** State of the connection towards the federate. The state is changed
     * through the OPEN_CONNEXION and CLOSE_CONNEXION messages.
     */
    enum class ConnectionState {
        Prelude, /// Initial state before OPEN_CONNEXION; no other message allowed.
        Ready, /// Communication is active.
        Ended /// State after CLOSE_CONNEXION; no further messages allowed.
    };
    
    FederationManagement(Communications* cm);
    ~FederationManagement();
    
    void setTm(TimeManagement* time_management);
    
    FederationHandle getFederationHandle() const;
    FederateHandle getFederateHandle() const;

    ConnectionState getConnectionState() const;
    void setConnectionState(const ConnectionState state);

    void checkFederationSaving(void) throw(SaveInProgress);
    void checkFederationRestoring(void) throw(RestoreInProgress);
    
    std::string getRtiVersion() const;
    void setRtiVersion(const std::string& version);

    // -- Create/Destroy --
    void createFederationExecution(const std::string& federation_execution_name,
                                   const std::vector<std::string> fom_module_designators,
                                   const std::string& mim_designator,
                                   const RtiVersion rti_version,
                                   Exception::Type& e);
    
    void destroyFederationExecution(const std::string& theName, Exception::Type& e);

    // -- Join/Resign --
    FederateHandle joinFederationExecution(const std::string& federate_name,
                                           const std::string& federate_type,
                                           const std::string& federation_execution_name,
                                           const std::vector<std::string> additional_fom_modules,
                                           const RtiVersion rti_version,
                                           RootObject* rootObject,
                                           Exception::Type& e);

    void resignFederationExecution(ResignAction action, Exception::Type& e);

    /// Resigns if we are still member, call this before we throw away all the rtia members
    void resignFederationExecutionForTermination();

    // Synchronization.
    void registerSynchronization(const std::string& label, const std::string& tag, Exception::Type& e);
    void registerSynchronization(const std::string& label,
                                 const std::string& tag,
                                 unsigned short array_size,
                                 const std::vector<FederateHandle>& fed_array,
                                 Exception::Type& e);
    void unregisterSynchronization(const std::string& label, Exception::Type& e);
    void synchronizationPointRegistrationFailed(const std::string& label);
    void synchronizationPointRegistrationSucceeded(const std::string& label);
    void federationSynchronized(const std::string& label);
    void announceSynchronizationPoint(const std::string& label, const std::string& tag);

    // Saving.
    void requestFederationSave(const std::string& label, FederationTime the_time, Exception::Type& e);
    void requestFederationSave(const std::string& label, Exception::Type& e);
    void federateSaveBegun(Exception::Type&);
    void federateSaveStatus(bool, Exception::Type&);
    void initiateFederateSave(const std::string& label);
    void federationSavedStatus(bool);

    // Restoring.
    void requestFederationRestore(const std::string& label, Exception::Type& e);
    void federateRestoreStatus(bool status, Exception::Type& e);
    void requestFederationRestoreStatus(bool status, const std::string& label, const std::string& reason);
    void federationRestoreBegun(void);
    void initiateFederateRestore(const std::string& label, FederateHandle handle);
    void federationRestoredStatus(bool status);

private:
    Communications* comm;
    TimeManagement* my_tm {nullptr};

    FederationHandle my_federation_handle {0};
    FederateHandle my_federate_handle {0};

    bool my_is_member_of_a_federation {false};

    bool my_is_saving {false};
    bool my_is_restoring {false};

    std::set<std::string> my_synchronization_labels {}; /// Labels being synchronized.

    std::string my_federation_name {""};
    std::string my_federate_name {""};
    
    ConnectionState my_connection_state {ConnectionState::Prelude};
};
}
} // namespace certi/rtia

#endif // _CERTI_RTIA_FEDERATION_MANAGEMENT
