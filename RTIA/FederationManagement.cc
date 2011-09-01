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
// $Id: FederationManagement.cc,v 3.89 2011/09/01 13:50:55 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "FederationManagement.hh"

#include "TimeManagement.hh"
#include "PrettyDebug.hh"
#include "NM_Classes.hh"
#include "M_Classes.hh"

#include <cstring>
#include <cstdio>
#include <cassert>
#include <memory>

namespace certi {
namespace rtia {

static PrettyDebug D("RTIA_FM", "(RTIA FM) ");
static PrettyDebug G("GENDOC",__FILE__);

// ----------------------------------------------------------------------------
//! FederationManagement.
  FederationManagement::FederationManagement(Communications *GC, Statistics* newStat)
    : savingState(false), restoringState(false)
{
    G.Out(pdGendoc,"enter FederationManagement::FederationManagement");
    comm = GC ;
    stat = newStat ;
    tm = NULL ;

    _numero_federation = 0 ;
    federate = 0 ;

    _connection_state = CONNECTION_PRELUDE;

    _est_membre_federation = false ;

    G.Out(pdGendoc,"exit  FederationManagement::FederationManagement");
}

// ----------------------------------------------------------------------------
//! Destructor.
FederationManagement::~FederationManagement()
{
    G.Out(pdGendoc,"enter ~FederationManagement");

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
    Debug(D,pdDebug) << "RTIA: Federate destroyed" << std::endl ;
    //}
    
    G.Out(pdGendoc,"exit  ~FederationManagement");
}

// ----------------------------------------------------------------------------
//! createFederationExecution.
void
FederationManagement::
createFederationExecution(const std::string& theName,
                          const std::string& fedId,
                          TypeException &e)
         throw ( FederationExecutionAlreadyExists,
                 CouldNotOpenFED,ErrorReadingFED,
                 RTIinternalError)
{
    NM_Create_Federation_Execution requete;

    G.Out(pdGendoc,"enter FederationManagement::createFederationExecution");
    D.Out(pdInit, "Creating Federation %s.", theName.c_str());

    e = e_NO_EXCEPTION ;

    if (_est_membre_federation)
       {
        std::cout<<"Federate "<<_nom_federe<<" is yet a federation member"<<std::endl;
        e = e_RTIinternalError ;
        }

    if (e == e_NO_EXCEPTION)
        {               
        requete.setFederationName(theName);
        requete.setFEDid(fedId);

        G.Out(pdGendoc,"createFederationExecution====>   send Message to RTIG");

        comm->sendMessage(&requete);

        std::auto_ptr<NetworkMessage> reponse(comm->waitMessage(NetworkMessage::CREATE_FEDERATION_EXECUTION,
                          federate));

        G.Out(pdGendoc,"createFederationExecution<== receive Message from RTIG");

        // We have to see if C_F_E is OK.

        if (reponse->getException() == e_NO_EXCEPTION)
            {
            _nom_federation = theName;            
            _numero_federation = reponse->getFederation() ;
            D.Out(pdInit, "est createur");
            }
        else if (reponse->getException() == e_CouldNotOpenFED)
            // RTIG encounters a problem creating federation execution
            {
            e = reponse->getException();
            G.Out(pdGendoc,"exit FederationManagement::"
                           "createFederationExecution on exception");
            throw CouldNotOpenFED (reponse->getExceptionReason()) ;
            }           
        else if (reponse->getException() == e_FederationExecutionAlreadyExists)
            {
            e = reponse->getException();
            G.Out(pdGendoc,"exit FederationManagement::"
                           "createFederationExecution on exception");
            throw FederationExecutionAlreadyExists (reponse->getExceptionReason()) ;
            }
        else if (reponse->getException() == e_ErrorReadingFED)
            {
            e = reponse->getException();
            G.Out(pdGendoc,"exit FederationManagement::"
                           "createFederationExecution on exception ErrorReadingFED");
            throw ErrorReadingFED (reponse->getExceptionReason()) ;
            }
        else
            {
            e = reponse->getException() ;
            G.Out(pdGendoc,"exit FederationManagement::"
                           "createFederationExecution on exception RTIinternalError");
            throw RTIinternalError (reponse->getExceptionReason()) ;
            D.Out(pdInit, "deja cree");
            }
        }

    G.Out(pdGendoc,"exit FederationManagement::createFederationExecution");

}


// ----------------------------------------------------------------------------
//! destroyFederationExecution.
void
FederationManagement::
destroyFederationExecution(const std::string& theName,
                           TypeException &e)
{
    NM_Destroy_Federation_Execution requete ;

    D.Out(pdInit, "Destroy Federation %s.", theName.c_str());
    G.Out(pdGendoc,"enter FederationManagement::destroyFederationExecution");
    e = e_NO_EXCEPTION ;

    // BUG: On devrait pouvoir detruire une federation meme si on n'est
    // pas le createur.
    //if (strcmp(theName, _nom_federation))
    //    e = e_FederationExecutionDoesNotExist ;

    if (e == e_NO_EXCEPTION)
        {
        requete.setFederation(_numero_federation);
        requete.setFederate(federate);
        requete.setFederationName(theName);

        G.Out(pdGendoc,"destroyFederationExecution====>send Message to RTIG");

        comm->sendMessage(&requete);

        std::auto_ptr<NetworkMessage> reponse(comm->waitMessage(
                          NetworkMessage::DESTROY_FEDERATION_EXECUTION,
                          federate));

        if (reponse->getException() == e_NO_EXCEPTION) {
            _nom_federation    = "" ;
            _numero_federation = 0 ;
            }
        else
            {
            // There is an exception so destroy may be not done on RTIG
            e = reponse->getException() ;
            }
    }

G.Out(pdGendoc,"exit  FederationManagement::destroyFederationExecution");
}

// ----------------------------------------------------------------------------
//! joinFederationExecution.
FederateHandle
FederationManagement::
joinFederationExecution(const std::string& Federate,
                        const std::string& Federation,
                        RootObject* rootObject,
                        TypeException &e)
{
    NM_Join_Federation_Execution requete;
    int i, nb ;

    G.Out(pdGendoc,"enter FederationManagement::joinFederationExecution");
    D.Out(pdInit, "Join Federation %s as %s.", Federation.c_str(), Federate.c_str());

    e = e_NO_EXCEPTION ;

    // this federate, may be, has yet joined federation so don't disturb RTIG
    if (_est_membre_federation){
          e = e_FederateAlreadyExecutionMember ;
         return(0);
    }

    if (e == e_NO_EXCEPTION)
        {
        requete.setFederationName(Federation);
        requete.setFederateName(Federate);

        requete.setBestEffortAddress(comm->getAddress());
        requete.setBestEffortPeer(comm->getPort());

        G.Out(pdGendoc,"joinFederationExecution====>send Message to RTIG");

        comm->sendMessage(&requete);

        // Waiting RTIG answer
        std::auto_ptr<NetworkMessage> reponse(comm->waitMessage(NetworkMessage::JOIN_FEDERATION_EXECUTION, 0));

        // If OK, regulators number is inside the answer.
        // Then we except a NULL message from each.
        if (reponse->getException() == e_NO_EXCEPTION) {
            NM_Join_Federation_Execution* joinResponse = static_cast<NM_Join_Federation_Execution*>(reponse.get());
            rootObject->rebuildFromSerializedFOM(*joinResponse);

            _nom_federation = Federation;
            _nom_federe = Federate;
            _numero_federation = reponse->getFederation() ;
            federate = reponse->getFederate();
            //std::cout << "federate ID=" << federate << std::endl;
            tm->setFederate(federate);
#ifdef FEDERATION_USES_MULTICAST
            // creation du socket pour la communication best-effort
            comm->CreerSocketMC(reponse->AdresseMulticast, MC_PORT);
#endif

            nb = joinResponse->getNumberOfRegulators() ;
            for (i=0 ; i<nb ; i++) {
                reponse.reset(comm->waitMessage(NetworkMessage::MESSAGE_NULL, 0));
                assert(tm != NULL);
                tm->insert(reponse->getFederate(), reponse->getDate());
            }

            _est_membre_federation = true ;
            G.Out(pdGendoc,"exit(%d) FederationManagement::joinFederationExecution",federate);
            return(federate);
        }
        else {
            e = reponse->getException();
        }
    }
    G.Out(pdGendoc,"exit(0) FederationManagement::joinFederationExecution");
    return(0);
}

// -------------------------------
// -- resignFederationExecution --
// -------------------------------

void
FederationManagement::resignFederationExecution(ResignAction,
                                                TypeException &e)
{
    NM_Resign_Federation_Execution msg;
    TypeException exception = e_NO_EXCEPTION ;

    G.Out(pdGendoc,"enter FederationManagement::resignFederationExecution");

    e = e_NO_EXCEPTION ;

    D.Out(pdInit, "Resign Federation.");

    if (!_est_membre_federation)
        e = e_FederateNotExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        tm->StopperAvanceTemps();

        if (tm->requestRegulateurState())
            tm->setTimeRegulating(false,0,0, exception);

        msg.setFederation(_numero_federation);
        msg.setFederate(federate) ;

        G.Out(pdGendoc,"      resignFederationExecution ===> send NMessage RFE to RTIG");
        comm->sendMessage(&msg);

        // WAIT FOR RTIG answer
        std::auto_ptr<NetworkMessage> reponse(comm->waitMessage(NetworkMessage::RESIGN_FEDERATION_EXECUTION, federate));

        _est_membre_federation = false ;
        _numero_federation = 0 ;
        federate = 0 ;
        G.Out(pdGendoc,"exit  FederationManagement::resignFederationExecution");
	}
    else
	{    
        G.Out(pdGendoc,"exit  FederationManagement::resignFederationExecution on exception");
	}
}

// Resigns if we are still member, call this before we throw away all the rtia members
void
FederationManagement::resignFederationExecutionForTermination()
{
    G.Out(pdGendoc,"enter FederationManagement::resignFederationExecutionForTermination");

    if (_est_membre_federation) {
        TypeException e ;
        resignFederationExecution(DELETE_OBJECTS, e);
    }

    G.Out(pdGendoc,"exit  FederationManagement::resignFederationExecutionForTermination");
}

// ----------------------------------------------------------------------------
//! Register synchronization.
void
FederationManagement::registerSynchronization(const std::string& label,
                                              const std::string& tag,
                                              TypeException &e)
{
    D.Out(pdProtocol, "RegisterSynchronization.");
    G.Out(pdGendoc,"enter FederationManagement::registerSynchronization");

    e = e_NO_EXCEPTION ;

    if (_synchronizationLabels.find(label) != _synchronizationLabels.end())
        e = e_FederationAlreadyPaused ; // Label already pending.

    _synchronizationLabels.insert(label);

    if (!_est_membre_federation)
        e = e_FederateNotExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        NM_Register_Federation_Synchronization_Point req ;        
        req.setFederation( _numero_federation);
        req.setFederate (federate);
        req.setLabel(label);
        req.setTag(tag);
        // no federates set so boolean must be false
        req.setExists(false);

        G.Out(pdGendoc,"      registerSynchronization====> send Message to RTIG");

        comm->sendMessage(&req);
    }

    G.Out(pdGendoc,"exit  FederationManagement::registerSynchronization");

}

// ----------------------------------------------------------------------------
//! Register synchronization with set of federates
void
FederationManagement::registerSynchronization(const std::string& label,
                                              const std::string& tag,
                                              unsigned short array_size,
                                              const std::vector <FederateHandle> &fed_array,
                                              TypeException &e)
{
    D.Out(pdProtocol, "RegisterSynchronization.");
    G.Out(pdGendoc,"enter FederationManagement::registerSynchronization with federate set");

    e = e_NO_EXCEPTION ;

    if (_synchronizationLabels.find(label) != _synchronizationLabels.end())
        e = e_FederationAlreadyPaused ; // Label already pending.

    _synchronizationLabels.insert(label);

    if (!_est_membre_federation)
        e = e_FederateNotExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        NM_Register_Federation_Synchronization_Point req;        
        req.setFederation( _numero_federation);
        req.setFederate (federate);
        req.setLabel(label);
        req.setTag(tag);
        /* the synchronization point concerns a set of federate */
        req.setExists(true);
        req.setFederatesSize(array_size);
        for ( int j=0 ; j < array_size ; j++)
            req.setFederates(fed_array[j],j) ;

        G.Out(pdGendoc,"      registerSynchronization====> send Message to RTIG");

        comm->sendMessage(&req);
    }

    G.Out(pdGendoc,"exit  FederationManagement::registerSynchronization with federate set");

}
// ----------------------------------------------------------------------------
//! Unregister synchronization.
void
FederationManagement::unregisterSynchronization(const std::string& label,
                                                TypeException &e)
{
    D.Out(pdProtocol, "unregisterSynchronization.");

    e = e_NO_EXCEPTION ;

    // Find if this label has been requested by federate or RTIG.
    std::set<std::string>::iterator i = _synchronizationLabels.find(label);
    if (i == _synchronizationLabels.end())
        e = e_UnknownLabel ;
    else
      _synchronizationLabels.erase(i);
    
    if (!_est_membre_federation)
        e = e_FederateNotExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        NM_Synchronization_Point_Achieved req ;
        
        req.setFederation( _numero_federation);
        req.setFederate (federate);
        req.setLabel(label);

        comm->sendMessage(&req);
    }
}

// ----------------------------------------------------------------------------
void
FederationManagement::announceSynchronizationPoint(const std::string& label,
                                                   const std::string& tag)
{
    D.Out(pdInit, "Announce Synchronization Point \"%s\"(%s).", label.c_str(), tag.c_str());

    M_Announce_Synchronization_Point req;

    req.setLabel(label);
    req.setTag(tag);

    // adding label to list of synchronizations to be done.
    _synchronizationLabels.insert(label);

    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
void FederationManagement::
synchronizationPointRegistrationFailed(const std::string& label)
{
    D.Out(pdInit, "Synchronization Point Registration Failed \"%s\".",
          label.c_str());

    M_Synchronization_Point_Registration_Failed req;

    G.Out(pdGendoc,"enter FederationManagement::synchronizationPointRegistrationFailed");
    req.setLabel(label);

    comm->requestFederateService(&req);

    G.Out(pdGendoc,"exit  FederationManagement::synchronizationPointRegistrationFailed");

}

// ----------------------------------------------------------------------------
void FederationManagement::
synchronizationPointRegistrationSucceeded(const std::string& label)
{
    D.Out(pdInit, "Synchronization Point Registration Succeeded \"%s\".",
          label.c_str());

    M_Synchronization_Point_Registration_Succeeded req;

    G.Out(pdGendoc,"enter FederationManagement::synchronizationPointRegistrationSucceeded");
    req.setLabel(label);

    comm->requestFederateService(&req);

    G.Out(pdGendoc,"exit  FederationManagement::synchronizationPointRegistrationSucceeded");

}

// ----------------------------------------------------------------------------
void
FederationManagement::federationSynchronized(const std::string& label)
{
    D.Out(pdInit, "Federation Synchronized \"%s\".", label.c_str());

    M_Federation_Synchronized req;

    req.setLabel(label);
    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
// requestFederationSave with time
void
FederationManagement::requestFederationSave(const std::string& label,
                                            FederationTime the_time,
                                            TypeException &e)
{
    D.Out(pdInit, "Request for federation save \"%s\".", label.c_str());
    G.Out(pdGendoc,"enter FederationManagement::requestFederationSave "
                   "with time");

    NM_Request_Federation_Save req ;
    
    req.setDate(the_time);
    req.setLabel(label);
    req.setFederation( _numero_federation);
    req.setFederate (federate);

    G.Out(pdGendoc,"      requestFederationSave====>send Message R_F_S to RTIG");

    comm->sendMessage(&req);

    // Should make sure that RTIG don't have any save or restore recently set.
    // NetworkMessage rep ;
    // comm->waitMessage(&rep, NetworkMessage::REQUEST_FEDERATION_SAVE, federate);
    // e = rep.exception ;
}

// ----------------------------------------------------------------------------
// requestFederationSave without time
void
FederationManagement::requestFederationSave(const std::string& label,
                                            TypeException &e)
{
    D.Out(pdInit, "Request for federation save \"%s\".", label.c_str());
    G.Out(pdGendoc,"enter FederationManagement::requestFederationSave "
                   "without time");

    NM_Request_Federation_Save req ;
    
    req.setLabel(label);
    req.setFederation( _numero_federation);
    req.setFederate (federate);
    G.Out(pdGendoc,"      requestFederationSave====>send Message R_F_S to RTIG");

    comm->sendMessage(&req);

    G.Out(pdGendoc,"exit  FederationManagement::requestFederationSave "
                   "without time");
}

// ----------------------------------------------------------------------------
void
FederationManagement::federateSaveBegun(TypeException &)
{
    G.Out(pdGendoc,"enter FederationManagement::federateSaveBegun");
    D.Out(pdInit, "Beginning federate save.");

    if (!savingState)
        throw SaveNotInitiated("Federation did not initiate saving.");

    NM_Federate_Save_Begun req ;
    
    req.setFederate (federate);
    req.setFederation( _numero_federation);

    G.Out(pdGendoc,"      federateSaveBegun ====>send Message F_S_B to RTIG");

    comm->sendMessage(&req);

    G.Out(pdGendoc,"exit  FederationManagement::federateSaveBegun");
}

// ----------------------------------------------------------------------------
void
FederationManagement::federateSaveStatus(bool status, TypeException &)
{
    G.Out(pdGendoc,"enter FederationManagement::federateSaveStatus");
    D.Out(pdInit, "Federate %ssaved.", status ? "" : "not ");

    if (!savingState)
        throw SaveNotInitiated("Federation did not initiate saving.");

    std::auto_ptr<NetworkMessage> req(NM_Factory::create(status ? NetworkMessage::FEDERATE_SAVE_COMPLETE : NetworkMessage::FEDERATE_SAVE_NOT_COMPLETE));
   
    req->setFederate(federate);
    req->setFederation(_numero_federation);

    if (status)
       {    
       G.Out(pdGendoc,"      federateSaveStatus ====>send Message F_S_C to RTIG");
       }
    else
       {    
       G.Out(pdGendoc,"      federateSaveStatus ====>send Message F_S_N_C to RTIG");
       }

    comm->sendMessage(req.get());

    G.Out(pdGendoc,"exit  FederationManagement::federateSaveStatus");
}

// ----------------------------------------------------------------------------
void
FederationManagement::initiateFederateSave(const std::string& label)
{
    G.Out(pdGendoc,"enter FederationManagement::initiateFederateSave");
    D.Out(pdInit, "Initiate a federate save \"%s\".", label.c_str());

    savingState = true ;

    M_Initiate_Federate_Save req;

    req.setLabel(label);

    comm->requestFederateService(&req);

    G.Out(pdGendoc,"exit  FederationManagement::initiateFederateSave");
}

// ----------------------------------------------------------------------------
void
FederationManagement::federationSavedStatus(bool status)
{
    G.Out(pdGendoc,"enter FederationManagement::federationSavedStatus");
    D.Out(pdInit, "Federation %ssaved.", status ? "" : "not ");

    savingState = false ;
    if (status) {
        M_Federation_Saved     req;
    	comm->requestFederateService(&req);
    } else {
        M_Federation_Not_Saved req;
    	comm->requestFederateService(&req);
    }
    G.Out(pdGendoc,"exit  FederationManagement::federationSavedStatus");
}

// ----------------------------------------------------------------------------
void
FederationManagement::requestFederationRestore(const std::string& label,
                                               TypeException &)
{
    G.Out(pdGendoc,"enter FederationManagement::requestFederationRestore");
    D.Out(pdInit, "Request for federation restore \"%s\".", label.c_str());

    NM_Request_Federation_Restore req ;
    
    req.setLabel(label);
    req.setFederate (federate);
    req.setFederation( _numero_federation);

    G.Out(pdGendoc,"     requestFederationRestore  ====>send Message R_F_R to RTIG");

    comm->sendMessage(&req);

    // Should make sure that RTIG don't have any save or restore recently set.
    // ...
    G.Out(pdGendoc,"exit  FederationManagement::requestFederationRestore");
}

// ----------------------------------------------------------------------------
void
FederationManagement::federateRestoreStatus(bool status, TypeException &)
{
    D.Out(pdInit, "Federate %srestored.", status ? "" : "not ");

    if (!restoringState)
        throw RestoreNotRequested("Federation did not initiate restoring.");

    std::auto_ptr<NetworkMessage> req(NM_Factory::create(status ? NetworkMessage::FEDERATE_RESTORE_COMPLETE : NetworkMessage::FEDERATE_RESTORE_NOT_COMPLETE));
    req->setFederate(federate);
    req->setFederation(_numero_federation);
    comm->sendMessage(req.get());
}

// ----------------------------------------------------------------------------
void
FederationManagement::requestFederationRestoreStatus(bool status,
                                                     const std::string& label,
                                                     const std::string& reason)
{
    G.Out(pdGendoc,"enter FederationManagement::requestFederationRestoreStatus");
    D.Out(pdInit, "Federation restore request %saccepted",
          status ? "" : "not ");

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
    G.Out(pdGendoc,"exit  FederationManagement::requestFederationRestoreStatus");
}

// ----------------------------------------------------------------------------
void
FederationManagement::federationRestoreBegun()
{
    G.Out(pdGendoc,"enter FederationManagement::federationRestoreBegun");
    D.Out(pdInit, "Federation restore begun");

    M_Federation_Restore_Begun req;
    comm->requestFederateService(&req);
    G.Out(pdGendoc,"exit  FederationManagement::federationRestoreBegun");
}

// ----------------------------------------------------------------------------
void
FederationManagement::initiateFederateRestore(const std::string& label,
                                              FederateHandle handle)
{
    G.Out(pdGendoc,"enter FederationManagement::initiateFederateRestore");
    D.Out(pdInit, "Initiate federate restore \"%s\" with federate handle %d.",
          label.c_str(), handle);

    restoringState = true ;

    M_Initiate_Federate_Restore req;

    req.setFederate(handle);
    req.setLabel(label);

    comm->requestFederateService(&req);
    G.Out(pdGendoc,"exit  FederationManagement::initiateFederateRestore");
}

// ----------------------------------------------------------------------------
void
FederationManagement::federationRestoredStatus(bool status) {
    D.Out(pdInit, "Federation %srestored.", status ? "" : "not ");

    restoringState = false ;

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

// ----------------------------------------------------------------------------
void
FederationManagement::checkFederationSaving()
    throw (SaveInProgress)
{
    if (savingState) {
        throw SaveInProgress("Federation is in saving state");
    }
}

// ----------------------------------------------------------------------------
void
FederationManagement::checkFederationRestoring()
    throw (RestoreInProgress)
{
    if (restoringState) {
        throw RestoreInProgress("Federation is in restoring state");
    }
}

}} // namespace certi/rtia

