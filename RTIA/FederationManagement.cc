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

#include "FederationManagement.hh"
#include <config.h>

#include <cassert>
#include <cstdio>
#include <cstring>
#include <memory>

#include <libCERTI/M_Classes.hh>
#include <libCERTI/NM_Classes.hh>
#include <libCERTI/PrettyDebug.hh>

#include "TimeManagement.hh"

namespace certi {
namespace rtia {

static PrettyDebug D("RTIA_FM", "(RTIA FM) ");
static PrettyDebug G("GENDOC", __FILE__);

FederationManagement::FederationManagement(Communications* cm) : comm{cm}
{
}

FederationManagement::~FederationManagement()
{
    Debug(G, pdGendoc) << "enter ~FederationManagement" << std::endl;

    // BUG: On devrait pouvoir quitter une federation que l'on a cree
    // sans la detruire en partant.
    // a-t-on avertit les autres federes de notre depart et donc
    // de la destruction de notre objet par RemoveObject(HARZI)
    // car le Remove Object ne diffuse pas le message
    //if (_est_createur_federation) {
    //    cout << "RTIA: Staying active to destroy federation..." << endl ;

    //  destroyFederationExecution(_nom_federation, e);
    // while (e != e_NO_EXCEPTION) {
    //			sleep(1);
    //  destroyFederationExecution(_nom_federation, e);
    // }
    Debug(D, pdDebug) << "RTIA: Federate destroyed" << std::endl;
    //}

    Debug(G, pdGendoc) << "exit  ~FederationManagement" << std::endl;
}
    
void FederationManagement::setTm(TimeManagement* time_management)
{
    my_tm = time_management;
}

FederationHandle FederationManagement::getFederationHandle() const
{
    return my_federation_handle;
}

FederateHandle FederationManagement::getFederateHandle() const
{
    return my_federate_handle;
}

FederationManagement::ConnectionState FederationManagement::getConnectionState() const
{
    return my_connection_state;
}

void FederationManagement::setConnectionState(const ConnectionState state)
{
    my_connection_state = state;
}

void FederationManagement::createFederationExecution(const std::string& federation_execution_name,
                                                     const std::vector<std::string> fom_module_designators,
                                                     const std::string& mim_designator,
                                                     const RtiVersion rti_version,
                                                     Exception::Type& e)
{
    NM_Create_Federation_Execution request;

    Debug(G, pdGendoc) << "enter FederationManagement::createFederationExecution" << std::endl;
    Debug(D, pdInit) << "Creating Federation " << federation_execution_name << std::endl;

    e = Exception::Type::NO_EXCEPTION;

    if (my_is_member_of_a_federation) {
        std::cout << "Federate " << my_federate_name << " is yet a federation member" << std::endl;
        e = Exception::Type::RTIinternalError;
    }

    request.setFederationExecutionName(federation_execution_name);

    request.setFomModuleDesignatorsSize(fom_module_designators.size());
    auto i = 0;
    for (const auto& module : fom_module_designators) {
        request.setFomModuleDesignators(module, i++);
    }

    if (!mim_designator.empty()) {
        request.setMimDesignator(mim_designator);
    }
    
    request.setRtiVersion(rti_version);

    Debug(G, pdGendoc) << "createFederationExecution====>   send Message to RTIG" << std::endl;

    comm->sendMessage(&request);

    std::unique_ptr<NetworkMessage> response(
        comm->waitMessage(NetworkMessage::Type::CREATE_FEDERATION_EXECUTION, my_federate_handle));

    Debug(G, pdGendoc) << "createFederationExecution<== receive Message from RTIG" << std::endl;

    // We have to see if C_F_E is OK.

    if (response->getException() == Exception::Type::NO_EXCEPTION) {
        my_federation_name = federation_execution_name;
        my_federation_handle = FederationHandle(response->getFederation());
        Debug(D, pdInit) << "federation created" << std::endl;

        Debug(G, pdGendoc) << "exit FederationManagement::createFederationExecution" << std::endl;
    }
    else {
        e = response->getException();
        Debug(G, pdGendoc) << "exit FederationManagement::createFederationExecution on exception" << std::endl;

        switch (response->getException()) {
        case Exception::Type::CouldNotOpenFED:
            throw CouldNotOpenFED(response->getExceptionReason());
        case Exception::Type::FederationExecutionAlreadyExists:
            throw FederationExecutionAlreadyExists(response->getExceptionReason());
        case Exception::Type::ErrorReadingFED:
            throw ErrorReadingFED(response->getExceptionReason());
        default:
            throw RTIinternalError(response->getExceptionReason());
        }
    }
}

void FederationManagement::destroyFederationExecution(const std::string& theName, Exception::Type& e)
{
    NM_Destroy_Federation_Execution requete;

    Debug(D, pdInit) << "Destroy Federation " << theName << std::endl;
    Debug(G, pdGendoc) << "enter FederationManagement::destroyFederationExecution" << std::endl;
    e = Exception::Type::NO_EXCEPTION;

    // BUG: On devrait pouvoir detruire une federation meme si on n'est
    // pas le createur.
    //if (strcmp(theName, _nom_federation))
    //    e = e_FederationExecutionDoesNotExist ;

    if (e == Exception::Type::NO_EXCEPTION) {
        requete.setFederation(my_federation_handle.get());
        requete.setFederate(my_federate_handle);
        requete.setFederationName(theName);

        Debug(G, pdGendoc) << "destroyFederationExecution====>send Message to RTIG" << std::endl;

        comm->sendMessage(&requete);

        std::unique_ptr<NetworkMessage> reponse(
            comm->waitMessage(NetworkMessage::Type::DESTROY_FEDERATION_EXECUTION, my_federate_handle));

        if (reponse->getException() == Exception::Type::NO_EXCEPTION) {
            my_federation_name = "";
            my_federation_handle = FederationHandle(0);
        }
        else {
            // There is an exception so destroy may be not done on RTIG
            e = reponse->getException();
        }
    }

    Debug(G, pdGendoc) << "exit  FederationManagement::destroyFederationExecution" << std::endl;
}

FederateHandle FederationManagement::joinFederationExecution(const std::string& federate_name,
                                                             const std::string& federate_type,
                                                             const std::string& federation_execution_name,
                                                             const std::vector<std::string> additional_fom_modules,
                                                             const RtiVersion rti_version,
                                                             RootObject* rootObject,
                                                             Exception::Type& e)
{
    Debug(G, pdGendoc) << "enter FederationManagement::joinFederationExecution" << std::endl;
    Debug(D, pdInit) << "Join Federation " << federation_execution_name << " as " << federate_name << std::endl;

    NM_Join_Federation_Execution request;

    e = Exception::Type::NO_EXCEPTION;

    // this federate, may be, has yet joined federation so don't disturb RTIG
    if (my_is_member_of_a_federation) {
        e = Exception::Type::FederateAlreadyExecutionMember;
        return 0;
    }

    request.setBestEffortAddress(comm->getAddress());
    request.setBestEffortPeer(comm->getPort());

    request.setFederationExecutionName(federation_execution_name);
    request.setFederateName(federate_name);
    request.setFederateType(federate_type);
    
    request.setRtiVersion(rti_version);

    request.setAdditionalFomModulesSize(additional_fom_modules.size());
    auto i = 0;
    for (const auto& module : additional_fom_modules) {
        request.setAdditionalFomModules(module, i++);
    }

    Debug(G, pdGendoc) << "joinFederationExecution====>send Message to RTIG" << std::endl;

    comm->sendMessage(&request);

    // Waiting RTIG answer
    std::unique_ptr<NetworkMessage> response(comm->waitMessage(NetworkMessage::Type::JOIN_FEDERATION_EXECUTION, 0));

    // If OK, regulators number is inside the answer.
    // Then we except a NULL message from each.
    if (response->getException() == Exception::Type::NO_EXCEPTION) {
        NM_Join_Federation_Execution& joinResponse = static_cast<NM_Join_Federation_Execution&>(*response);
        rootObject->rebuildFromSerializedFOM(joinResponse);

        my_federation_name = federation_execution_name;
        my_federate_name = federate_name;
        my_federation_handle = FederationHandle(joinResponse.getFederation());
        my_federate_handle = joinResponse.getFederate();
        my_tm->setFederate(my_federate_handle);
#ifdef FEDERATION_USES_MULTICAST
        // creation du socket pour la communication best-effort
        comm->CreerSocketMC(reponse->getMulticastAddress(), MC_PORT);
#endif

        for (int i = 0; i < joinResponse.getNumberOfRegulators(); i++) {
            response.reset(comm->waitMessage(NetworkMessage::Type::MESSAGE_NULL, 0));
            assert(my_tm != NULL);
            my_tm->insert(response->getFederate(), response->getDate());
        }

        my_is_member_of_a_federation = true;
        Debug(G, pdGendoc) << "exit(" << federate_name << ") FederationManagement::joinFederationExecution"
                           << std::endl;
        return my_federate_handle;
    }
    else {
        e = response->getException();
    }

    Debug(G, pdGendoc) << "exit(0) FederationManagement::joinFederationExecution" << std::endl;
    return 0;
}

void FederationManagement::resignFederationExecution(ResignAction, Exception::Type& e)
{
    NM_Resign_Federation_Execution msg;
    Exception::Type exception = Exception::Type::NO_EXCEPTION;

    Debug(G, pdGendoc) << "enter FederationManagement::resignFederationExecution" << std::endl;

    e = Exception::Type::NO_EXCEPTION;

    Debug(D, pdInit) << "Resign Federation." << std::endl;

    if (!my_is_member_of_a_federation)
        e = Exception::Type::FederateNotExecutionMember;

    if (e == Exception::Type::NO_EXCEPTION) {
        my_tm->StopperAvanceTemps();

        if (my_tm->requestRegulateurState())
            my_tm->setTimeRegulating(false, 0, 0, exception);

        msg.setFederation(my_federation_handle.get());
        msg.setFederate(my_federate_handle);

        Debug(G, pdGendoc) << "      resignFederationExecution ===> send NMessage RFE to RTIG" << std::endl;
        comm->sendMessage(&msg);

        // WAIT FOR RTIG answer
        std::unique_ptr<NetworkMessage> reponse(
            comm->waitMessage(NetworkMessage::Type::RESIGN_FEDERATION_EXECUTION, my_federate_handle));

        my_is_member_of_a_federation = false;
        my_federation_handle = FederationHandle(0);
        my_federate_handle = 0;
        Debug(G, pdGendoc) << "exit  FederationManagement::resignFederationExecution" << std::endl;
    }
    else {
        Debug(G, pdGendoc) << "exit  FederationManagement::resignFederationExecution on exception" << std::endl;
    }
}

void FederationManagement::resignFederationExecutionForTermination()
{
    Debug(G, pdGendoc) << "enter FederationManagement::resignFederationExecutionForTermination" << std::endl;

    if (my_is_member_of_a_federation) {
        Exception::Type e;
        resignFederationExecution(DELETE_OBJECTS, e);
    }

    Debug(G, pdGendoc) << "exit  FederationManagement::resignFederationExecutionForTermination" << std::endl;
}

void FederationManagement::registerSynchronization(const std::string& label, const std::string& tag, Exception::Type& e)
{
    Debug(D, pdProtocol) << "RegisterSynchronization." << std::endl;
    Debug(G, pdGendoc) << "enter FederationManagement::registerSynchronization" << std::endl;

    e = Exception::Type::NO_EXCEPTION;

    if (my_synchronization_labels.find(label) != my_synchronization_labels.end())
        e = Exception::Type::FederationAlreadyPaused; // Label already pending.

    my_synchronization_labels.insert(label);

    if (!my_is_member_of_a_federation)
        e = Exception::Type::FederateNotExecutionMember;

    if (e == Exception::Type::NO_EXCEPTION) {
        NM_Register_Federation_Synchronization_Point req;
        req.setFederation(my_federation_handle.get());
        req.setFederate(my_federate_handle);
        req.setLabel(label);
        req.setTag(tag);
        // no federates set so boolean must be false
        req.setExists(false);

        Debug(G, pdGendoc) << "      registerSynchronization====> send Message to RTIG" << std::endl;

        comm->sendMessage(&req);
    }

    Debug(G, pdGendoc) << "exit  FederationManagement::registerSynchronization" << std::endl;
}

void FederationManagement::registerSynchronization(const std::string& label,
                                                   const std::string& tag,
                                                   unsigned short array_size,
                                                   const std::vector<FederateHandle>& fed_array,
                                                   Exception::Type& e)
{
    Debug(D, pdProtocol) << "RegisterSynchronization." << std::endl;
    Debug(G, pdGendoc) << "enter FederationManagement::registerSynchronization with federate set" << std::endl;

    e = Exception::Type::NO_EXCEPTION;

    if (my_synchronization_labels.find(label) != my_synchronization_labels.end())
        e = Exception::Type::FederationAlreadyPaused; // Label already pending.

    my_synchronization_labels.insert(label);

    if (!my_is_member_of_a_federation)
        e = Exception::Type::FederateNotExecutionMember;

    if (e == Exception::Type::NO_EXCEPTION) {
        NM_Register_Federation_Synchronization_Point req;
        req.setFederation(my_federation_handle.get());
        req.setFederate(my_federate_handle);
        req.setLabel(label);
        req.setTag(tag);
        /* the synchronization point concerns a set of federate */
        req.setExists(true);
        req.setFederatesSize(array_size);
        for (int j = 0; j < array_size; j++)
            req.setFederates(fed_array[j], j);

        Debug(G, pdGendoc) << "      registerSynchronization====> send Message to RTIG" << std::endl;

        comm->sendMessage(&req);
    }

    Debug(G, pdGendoc) << "exit  FederationManagement::registerSynchronization with federate set" << std::endl;
}

void FederationManagement::unregisterSynchronization(const std::string& label, Exception::Type& e)
{
    Debug(D, pdProtocol) << "unregisterSynchronization." << std::endl;

    e = Exception::Type::NO_EXCEPTION;

    // Find if this label has been requested by federate or RTIG.
    std::set<std::string>::iterator i = my_synchronization_labels.find(label);
    if (i == my_synchronization_labels.end())
        e = Exception::Type::UnknownLabel;
    else
        my_synchronization_labels.erase(i);

    if (!my_is_member_of_a_federation)
        e = Exception::Type::FederateNotExecutionMember;

    if (e == Exception::Type::NO_EXCEPTION) {
        NM_Synchronization_Point_Achieved req;

        req.setFederation(my_federation_handle.get());
        req.setFederate(my_federate_handle);
        req.setLabel(label);

        comm->sendMessage(&req);
    }
}

void FederationManagement::announceSynchronizationPoint(const std::string& label, const std::string& tag)
{
    Debug(D, pdInit) << "Announce Synchronization Point <" << label << "> (" << tag << ")" << std::endl;

    M_Announce_Synchronization_Point req;

    req.setLabel(label);
    req.setTag(tag);

    // adding label to list of synchronizations to be done.
    my_synchronization_labels.insert(label);

    comm->requestFederateService(&req);
}

void FederationManagement::synchronizationPointRegistrationFailed(const std::string& label)
{
    Debug(D, pdInit) << "Synchronization Point Registration Failed <" << label << ">" << std::endl;

    M_Synchronization_Point_Registration_Failed req;

    Debug(G, pdGendoc) << "enter FederationManagement::synchronizationPointRegistrationFailed" << std::endl;
    req.setLabel(label);

    comm->requestFederateService(&req);

    Debug(G, pdGendoc) << "exit  FederationManagement::synchronizationPointRegistrationFailed" << std::endl;
}

void FederationManagement::synchronizationPointRegistrationSucceeded(const std::string& label)
{
    Debug(D, pdInit) << "Synchronization Point Registration Succeeded <" << label << ">" << std::endl;

    M_Synchronization_Point_Registration_Succeeded req;

    Debug(G, pdGendoc) << "enter FederationManagement::synchronizationPointRegistrationSucceeded" << std::endl;
    req.setLabel(label);

    comm->requestFederateService(&req);

    Debug(G, pdGendoc) << "exit  FederationManagement::synchronizationPointRegistrationSucceeded" << std::endl;
}

void FederationManagement::federationSynchronized(const std::string& label)
{
    Debug(D, pdInit) << "Federation Synchronized <" << label << ">" << std::endl;

    M_Federation_Synchronized req;

    req.setLabel(label);
    comm->requestFederateService(&req);
}

void FederationManagement::requestFederationSave(const std::string& label,
                                                 FederationTime the_time,
                                                 Exception::Type& /*e*/)
{
    Debug(D, pdInit) << "Request for federation save <" << label << ">" << std::endl;
    Debug(G, pdGendoc) << "enter FederationManagement::requestFederationSave with time" << std::endl;

    NM_Request_Federation_Save req;

    req.setDate(the_time);
    req.setLabel(label);
    req.setFederation(my_federation_handle.get());
    req.setFederate(my_federate_handle);

    Debug(G, pdGendoc) << "      requestFederationSave====>send Message R_F_S to RTIG" << std::endl;

    comm->sendMessage(&req);

    // Should make sure that RTIG don't have any save or restore recently set.
    // NetworkMessage rep ;
    // comm->waitMessage(&rep, NetworkMessage::REQUEST_FEDERATION_SAVE, federate);
    // e = rep.exception ;
}

void FederationManagement::requestFederationSave(const std::string& label, Exception::Type& /*e*/)
{
    Debug(D, pdInit) << "Request for federation save <" << label << ">" << std::endl;
    Debug(G, pdGendoc) << "enter FederationManagement::requestFederationSave without time" << std::endl;

    NM_Request_Federation_Save req;

    req.setLabel(label);
    req.setFederation(my_federation_handle.get());
    req.setFederate(my_federate_handle);
    Debug(G, pdGendoc) << "      requestFederationSave====>send Message R_F_S to RTIG" << std::endl;

    comm->sendMessage(&req);

    Debug(G, pdGendoc) << "exit  FederationManagement::requestFederationSave without time" << std::endl;
}

void FederationManagement::federateSaveBegun(Exception::Type&)
{
    Debug(G, pdGendoc) << "enter FederationManagement::federateSaveBegun" << std::endl;
    Debug(D, pdInit) << "Beginning federate save." << std::endl;

    if (!my_is_saving)
        throw SaveNotInitiated("Federation did not initiate saving.");

    NM_Federate_Save_Begun req;

    req.setFederate(my_federate_handle);
    req.setFederation(my_federation_handle.get());

    Debug(G, pdGendoc) << "      federateSaveBegun ====>send Message F_S_B to RTIG" << std::endl;

    comm->sendMessage(&req);

    Debug(G, pdGendoc) << "exit  FederationManagement::federateSaveBegun" << std::endl;
}

void FederationManagement::federateSaveStatus(bool status, Exception::Type&)
{
    Debug(G, pdGendoc) << "enter FederationManagement::federateSaveStatus" << std::endl;
    Debug(D, pdInit) << "Federate " << (status ? "" : "not ") << "saved" << std::endl;

    if (!my_is_saving)
        throw SaveNotInitiated("Federation did not initiate saving.");

    std::unique_ptr<NetworkMessage> req(NM_Factory::create(status ? NetworkMessage::Type::FEDERATE_SAVE_COMPLETE
                                                                  : NetworkMessage::Type::FEDERATE_SAVE_NOT_COMPLETE));

    req->setFederate(my_federate_handle);
    req->setFederation(my_federation_handle.get());

    if (status) {
        Debug(G, pdGendoc) << "      federateSaveStatus ====>send Message F_S_C to RTIG" << std::endl;
    }
    else {
        Debug(G, pdGendoc) << "      federateSaveStatus ====>send Message F_S_N_C to RTIG" << std::endl;
    }

    comm->sendMessage(req.get());

    Debug(G, pdGendoc) << "exit  FederationManagement::federateSaveStatus" << std::endl;
}

void FederationManagement::initiateFederateSave(const std::string& label)
{
    Debug(G, pdGendoc) << "enter FederationManagement::initiateFederateSave" << std::endl;
    Debug(D, pdInit) << "Initiate a federate save <" << label << ">" << std::endl;

    my_is_saving = true;

    M_Initiate_Federate_Save req;

    req.setLabel(label);

    comm->requestFederateService(&req);

    Debug(G, pdGendoc) << "exit  FederationManagement::initiateFederateSave" << std::endl;
}

void FederationManagement::federationSavedStatus(bool status)
{
    Debug(G, pdGendoc) << "enter FederationManagement::federationSavedStatus" << std::endl;
    Debug(D, pdInit) << "Federation " << (status ? "" : "not ") << "saved" << std::endl;

    my_is_saving = false;
    if (status) {
        M_Federation_Saved req;
        comm->requestFederateService(&req);
    }
    else {
        M_Federation_Not_Saved req;
        comm->requestFederateService(&req);
    }
    Debug(G, pdGendoc) << "exit  FederationManagement::federationSavedStatus" << std::endl;
}

void FederationManagement::requestFederationRestore(const std::string& label, Exception::Type&)
{
    Debug(G, pdGendoc) << "enter FederationManagement::requestFederationRestore" << std::endl;
    Debug(D, pdInit) << "Request for federation restore <" << label << ">" << std::endl;

    NM_Request_Federation_Restore req;

    req.setLabel(label);
    req.setFederate(my_federate_handle);
    req.setFederation(my_federation_handle.get());

    Debug(G, pdGendoc) << "     requestFederationRestore  ====>send Message R_F_R to RTIG" << std::endl;

    comm->sendMessage(&req);

    // Should make sure that RTIG don't have any save or restore recently set.
    // ...
    Debug(G, pdGendoc) << "exit  FederationManagement::requestFederationRestore" << std::endl;
}

void FederationManagement::federateRestoreStatus(bool status, Exception::Type&)
{
    Debug(D, pdInit) << "Federate " << (status ? "" : "not ") << "restored" << std::endl;

    if (!my_is_restoring)
        throw RestoreNotRequested("Federation did not initiate restoring.");

    std::unique_ptr<NetworkMessage> req(NM_Factory::create(status
                                                               ? NetworkMessage::Type::FEDERATE_RESTORE_COMPLETE
                                                               : NetworkMessage::Type::FEDERATE_RESTORE_NOT_COMPLETE));
    req->setFederate(my_federate_handle);
    req->setFederation(my_federation_handle.get());
    comm->sendMessage(req.get());
}

void FederationManagement::requestFederationRestoreStatus(bool status,
                                                          const std::string& label,
                                                          const std::string& reason)
{
    Debug(G, pdGendoc) << "enter FederationManagement::requestFederationRestoreStatus" << std::endl;
    Debug(D, pdInit) << "Federation restore request " << (status ? "" : "not ") << "accepted" << std::endl;

    Message* req;

    if (status) {
        req = new M_Request_Federation_Restore_Succeeded();
    }
    else {
        M_Request_Federation_Restore_Failed* RFRF;
        RFRF = new M_Request_Federation_Restore_Failed();
        RFRF->setReason(reason);
        req = RFRF;
    }
    req->setLabel(label);
    comm->requestFederateService(req);
    delete req;
    Debug(G, pdGendoc) << "exit  FederationManagement::requestFederationRestoreStatus" << std::endl;
}

void FederationManagement::federationRestoreBegun()
{
    Debug(G, pdGendoc) << "enter FederationManagement::federationRestoreBegun" << std::endl;
    Debug(D, pdInit) << "Federation restore begun" << std::endl;

    M_Federation_Restore_Begun req;
    comm->requestFederateService(&req);
    Debug(G, pdGendoc) << "exit  FederationManagement::federationRestoreBegun" << std::endl;
}

void FederationManagement::initiateFederateRestore(const std::string& label, FederateHandle handle)
{
    Debug(G, pdGendoc) << "enter FederationManagement::initiateFederateRestore" << std::endl;
    Debug(D, pdInit) << "Initiate federate restore <" << label << "> with federate handle " << handle << std::endl;

    my_is_restoring = true;

    M_Initiate_Federate_Restore req;

    req.setFederate(handle);
    req.setLabel(label);

    comm->requestFederateService(&req);
    Debug(G, pdGendoc) << "exit  FederationManagement::initiateFederateRestore" << std::endl;
}

void FederationManagement::federationRestoredStatus(bool status)
{
    Debug(D, pdInit) << "Federation " << (status ? "" : "not ") << "restored" << std::endl;

    my_is_restoring = false;

    Message* req;

    if (status) {
        req = new M_Federation_Restored();
    }
    else {
        req = new M_Federation_Not_Restored();
    }

    comm->requestFederateService(req);
    delete req;
}

void FederationManagement::checkFederationSaving() throw(SaveInProgress)
{
    if (my_is_saving) {
        throw SaveInProgress("Federation is in saving state");
    }
}

void FederationManagement::checkFederationRestoring() throw(RestoreInProgress)
{
    if (my_is_restoring) {
        throw RestoreInProgress("Federation is in restoring state");
    }
}
}
} // namespace certi/rtia
