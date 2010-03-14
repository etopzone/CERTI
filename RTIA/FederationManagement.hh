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
// $Id: FederationManagement.hh,v 3.31 2010/03/14 15:35:53 gotthardp Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIA_FEDERATION_MANAGEMENT
#define _CERTI_RTIA_FEDERATION_MANAGEMENT

#include <set>
#include <string>

#include "certi.hh"
#include "NetworkMessage.hh"
#include "Communications.hh"
#include "Statistics.hh"

namespace certi {

class RootObject;

namespace rtia {

class TimeManagement ;

class FederationManagement
{
public:
  FederationManagement(Communications *, Statistics*);
    ~FederationManagement();

    void checkFederationSaving(void) throw (SaveInProgress);
    void checkFederationRestoring(void) throw (RestoreInProgress);

    // -- Create/Destroy --
    void createFederationExecution(const std::string& theName,
                                   const std::string& fedId, TypeException &e)
         throw ( FederationExecutionAlreadyExists,
                 CouldNotOpenFED,ErrorReadingFED,
                 RTIinternalError);
    void destroyFederationExecution(const std::string& theName, TypeException &e);

    // -- Join/Resign --
    FederateHandle joinFederationExecution(const std::string& Federate,
                                           const std::string& Federation,
                                           RootObject* rootObject,
                                           TypeException &e);

    void resignFederationExecution(ResignAction action,
                                   TypeException &e);
    void resignFederationExecutionForTermination();

    // Synchronization.
    void registerSynchronization(const std::string& label,
                                 const std::string& tag,
                                 TypeException &e);
    void registerSynchronization(const std::string& label,
                                 const std::string& tag,
                                 unsigned short array_size,
                                 const std::vector <FederateHandle> &fed_array,
                                 TypeException &e);
    void unregisterSynchronization(const std::string& label,
                                   TypeException &e);
    void synchronizationPointRegistrationFailed(const std::string& label);
    void synchronizationPointRegistrationSucceeded(const std::string& label);
    void federationSynchronized(const std::string& label);
    void announceSynchronizationPoint(const std::string& label,
                                      const std::string& tag);

    // Saving.
    void requestFederationSave(const std::string& label, FederationTime the_time, TypeException &e);
    void requestFederationSave(const std::string& label, TypeException &e);
    void federateSaveBegun(TypeException &);
    void federateSaveStatus(bool, TypeException &);
    void initiateFederateSave(const std::string& label);
    void federationSavedStatus(bool);

    // Restoring.
    void requestFederationRestore(const std::string& label,
                                  TypeException &e);
    void federateRestoreStatus(bool status, TypeException &e);
    void requestFederationRestoreStatus(bool status,
                                        const std::string& label,
                                        const std::string& reason);
    void federationRestoreBegun(void);
    void initiateFederateRestore(const std::string& label,
                                 FederateHandle handle);
    void federationRestoredStatus(bool status);

    // Attributes
    TimeManagement *tm ;
    Handle _numero_federation ;
    FederateHandle federate ;

    /**
     * State of the connection towards the federate. The state is changed
     * through the OPEN_CONNEXION and CLOSE_CONNEXION messages.
     */
    typedef enum ConnectionState {
        //! Initial state before OPEN_CONNEXION; no other message allowed.
        CONNECTION_PRELUDE,
        //! Communication is active.
        CONNECTION_READY,
        //! State after CLOSE_CONNEXION; no further messages allowed.
        CONNECTION_FIN
    } ConnectionState_t;

    ConnectionState_t _connection_state;

private:
    Communications *comm ;
    Statistics     *stat;

    bool _est_membre_federation ;

    bool savingState ;
    bool restoringState ;

    std::set<std::string> _synchronizationLabels ; //!< Labels being synchronized.

    std::string _nom_federation ;
    std::string _nom_federe;
    
};

}} // namespace certi/rtia

#endif // _CERTI_RTIA_FEDERATION_MANAGEMENT

// $Id: FederationManagement.hh,v 3.31 2010/03/14 15:35:53 gotthardp Exp $
