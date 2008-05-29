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
// $Id: FederationManagement.hh,v 3.23 2008/05/29 12:20:33 rousse Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIA_FEDERATION_MANAGEMENT
#define _CERTI_RTIA_FEDERATION_MANAGEMENT

#include "certi.hh"
#include "NetworkMessage.hh"
#include "Communications.hh"
#include "Statistics.hh"

namespace certi {
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
    void createFederationExecution(std::string theName, TypeException &e)
         throw ( FederationExecutionAlreadyExists,
                 CouldNotOpenFED,ErrorReadingFED,
                 RTIinternalError);
    void destroyFederationExecution(std::string theName, TypeException &e);

    // -- Join/Resign --
    FederateHandle joinFederationExecution(std::string Federate,
                                           std::string Federation,
                                           TypeException &e);

    void resignFederationExecution(RTI::ResignAction action,
                                   TypeException &e);

    // Synchronization.
    void registerSynchronization(std::string label,
                                 std::string tag,
                                 TypeException &e);
    void registerSynchronization(std::string label,
                                 std::string tag,
                                 unsigned short array_size,
                                 FederateHandle *fed_array,
                                 TypeException &e);
    void unregisterSynchronization(std::string label,
                                   TypeException &e);
    void synchronizationPointRegistrationFailed(const char *label);
    void synchronizationPointRegistrationSucceeded(const char *label);
    void federationSynchronized(const char *label);
    void announceSynchronizationPoint(const char *label,
                                      const char *tag);

    // Saving.
    void requestFederationSave(std::string label, FederationTime the_time, TypeException &e);
    void requestFederationSave(std::string label, TypeException &e);
    void federateSaveBegun(TypeException &);
    void federateSaveStatus(bool, TypeException &);
    void initiateFederateSave(const char *label);
    void federationSavedStatus(bool);

    // Restoring.
    void requestFederationRestore(std::string label,
                                  TypeException &e);
    void federateRestoreStatus(bool status, TypeException &e);
    void requestFederationRestoreStatus(bool status,
                                        const char *label,
                                        const char *reason);
    void federationRestoreBegun(void);
    void initiateFederateRestore(const char *label,
                                 FederateHandle handle);
    void federationRestoredStatus(bool status);

    // Attributes
    TimeManagement *tm ;
    Handle _numero_federation ;
    FederateHandle federate ;
    bool _fin_execution ;
    std::string  _FEDid ;

private:
    Communications *comm ;
    Statistics     *stat;

    bool _est_createur_federation ;
    bool _est_membre_federation ;

    bool savingState ;
    bool restoringState ;

    std::list<char *> synchronizationLabels ; //!< Labels being synchronized.

    std::string _nom_federation ;
    std::string _nom_federe;
    
};

}} // namespace certi/rtia

#endif // _CERTI_RTIA_FEDERATION_MANAGEMENT

// $Id: FederationManagement.hh,v 3.23 2008/05/29 12:20:33 rousse Exp $
