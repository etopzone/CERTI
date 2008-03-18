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
// $Id: FederationManagement.cc,v 3.50.2.1 2008/03/18 15:55:57 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "FederationManagement.hh"

#include "TimeManagement.hh"
#include "PrettyDebug.hh"

#ifdef _WIN32
#include <windows.h>
#else
#include <list>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#endif
#include <assert.h>

using std::list ;
using std::string ;

namespace certi {
namespace rtia {

static pdCDebug D("RTIA_FM", "(RTIA FM) ");
static PrettyDebug G("GENDOC",__FILE__);

// ----------------------------------------------------------------------------
//! FederationManagement.
  FederationManagement::FederationManagement(Communications *GC, Statistics* stat)
    : savingState(false), restoringState(false)
{
    G.Out(pdGendoc,"enter FederationManagement::FederationManagement");
    comm = GC ;
    this->stat = stat ;
    tm = NULL ;

    _numero_federation = 0 ;
    federate = 0 ;

    _fin_execution = false ;

    //_est_createur_federation = false ;
    _est_membre_federation = false ;

    _nom_federation = NULL ;
    _nom_federe[0] = 0 ;
    _FEDid = NULL ;
    G.Out(pdGendoc,"exit  FederationManagement::FederationManagement");
}

// ----------------------------------------------------------------------------
//! Destructor.
FederationManagement::~FederationManagement()
{
    TypeException e ;
    G.Out(pdGendoc,"enter ~FederationManagement");

    if (_est_membre_federation) {
        resignFederationExecution(RTI::DELETE_OBJECTS, e);
    }

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
        cout << "RTIA: Federation destroyed" << endl ;
    //}

    delete _nom_federation ;
    delete _FEDid ;
    G.Out(pdGendoc,"exit  ~FederationManagement");
}

// ----------------------------------------------------------------------------
//! createFederationExecution.
void
FederationManagement::
createFederationExecution(const char *theName,
                          TypeException &e)
         throw ( FederationExecutionAlreadyExists,
                 CouldNotOpenFED,ErrorReadingFED,
                 RTIinternalError)
{
    NetworkMessage requete;

    G.Out(pdGendoc,"enter FederationManagement::createFederationExecution");
    D.Out(pdInit, "Creating Federation %s.", theName);

    e = e_NO_EXCEPTION ;

    // BUG: pour l'instant, le federe ne peut creer q'une federation
    //if (_est_createur_federation)
    //    {
    //    std::cout<<"Federate "<<_nom_federe<<" is yet a federation creator"<<std::endl;
    //    e = e_RTIinternalError ;
    //    }
    //else if (_est_membre_federation)
    if (_est_membre_federation)
       {
        std::cout<<"Federate "<<_nom_federe<<" is yet a federation member"<<std::endl;
        e = e_RTIinternalError ;
        }

    if (e == e_NO_EXCEPTION)
        {
        requete.type = NetworkMessage::CREATE_FEDERATION_EXECUTION ;        
        requete.federationName = theName;
        requete.FEDid = _FEDid;        

        G.Out(pdGendoc,"createFederationExecution====>   send Message to RTIG");

        comm->sendMessage(&requete);

        std::auto_ptr<NetworkMessage> reponse(comm->waitMessage(NetworkMessage::CREATE_FEDERATION_EXECUTION,
                          federate));

        G.Out(pdGendoc,"createFederationExecution<== receive Message from RTIG");

        // We have to see if C_F_E is OK.

        if (reponse->exception == e_NO_EXCEPTION)
            {
            _nom_federation = new char[strlen(theName)+1] ;
            strcpy(_nom_federation, theName);
            _numero_federation = reponse->federation ;
            //_est_createur_federation = true ;
            D.Out(pdInit, "est createur");
            }
        else if (reponse->exception == e_CouldNotOpenFED)
            // RTIG encounters a problem creating federation execution
            {
            e = reponse->exception;
            G.Out(pdGendoc,"exit FederationManagement::"
                           "createFederationExecution on exception");
            throw CouldNotOpenFED (reponse->exceptionReason.c_str()) ;
            }           
        else if (reponse->exception == e_FederationExecutionAlreadyExists)
            {
            e = reponse->exception;
            G.Out(pdGendoc,"exit FederationManagement::"
                           "createFederationExecution on exception");
            throw FederationExecutionAlreadyExists (reponse->exceptionReason.c_str()) ;
            }
        else if (reponse->exception == e_ErrorReadingFED)
            {
            e = reponse->exception;
            G.Out(pdGendoc,"exit FederationManagement::"
                           "createFederationExecution on exception ErrorReadingFED");
            throw ErrorReadingFED (reponse->exceptionReason.c_str()) ;
            }
        else
            {
            e = reponse->exception ;
            G.Out(pdGendoc,"exit FederationManagement::"
                           "createFederationExecution on exception RTIinternalError");
            throw RTIinternalError (reponse->exceptionReason.c_str()) ;
            D.Out(pdInit, "deja cree");
            }
        }

    G.Out(pdGendoc,"exit FederationManagement::createFederationExecution");

}


// ----------------------------------------------------------------------------
//! destroyFederationExecution.
void
FederationManagement::
destroyFederationExecution(const char *theName,
                           TypeException &e)
{
    NetworkMessage requete;

    D.Out(pdInit, "Destroy Federation %s.", theName);
    G.Out(pdGendoc,"enter FederationManagement::destroyFederationExecution");
    e = e_NO_EXCEPTION ;

    // BUG: On devrait pouvoir detruire une federation meme si on n'est
    // pas le createur.
    //if (strcmp(theName, _nom_federation))
    //    e = e_FederationExecutionDoesNotExist ;

    if (e == e_NO_EXCEPTION) {
        requete.type = NetworkMessage::DESTROY_FEDERATION_EXECUTION ;
        requete.federation = _numero_federation ;
        requete.federate = federate ;
        requete.federationName = theName;        

        G.Out(pdGendoc,"destroyFederationExecution====>send Message to RTIG");
        comm->sendMessage(&requete);

        std::auto_ptr<NetworkMessage> reponse(comm->waitMessage(
                          NetworkMessage::DESTROY_FEDERATION_EXECUTION,
                          federate));

        if (reponse->exception == e_NO_EXCEPTION) {
            _nom_federation = NULL ;
            _numero_federation = 0 ;
            _est_createur_federation = false ;
            _fin_execution = true ;
            // Now, remove temporary file (if not yet done)
            if ( _FEDid != NULL )
               {
               if ( _FEDid[0] != '\0' )
                   {
                   std::remove(_FEDid);
                   _FEDid[0] = '\0' ;
                   }
               }
        }
        else
            e = reponse->exception ;
    }
G.Out(pdGendoc,"exit  FederationManagement::destroyFederationExecution");
}

// ----------------------------------------------------------------------------
//! joinFederationExecution.
FederateHandle
FederationManagement::
joinFederationExecution(const char *Federate,
                        const char *Federation,
                        TypeException &e)
{
    NetworkMessage requete, requeteFED ;
    int i, nb ;
    char *filename ; // Needed for working file name

    G.Out(pdGendoc,"enter FederationManagement::joinFederationExecution");
    D.Out(pdInit, "Join Federation %s as %s.", Federation, Federate);

    e = e_NO_EXCEPTION ;

    if (_est_createur_federation && strcmp(Federation, _nom_federation))
        e = e_RTIinternalError ;

    if (_est_membre_federation)
        e = e_FederateAlreadyExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        requete.type = NetworkMessage::JOIN_FEDERATION_EXECUTION ;
        requete.federationName = Federation;        
        requete.federateName = Federate;

        requete.bestEffortAddress = comm->getAddress();
        requete.bestEffortPeer = comm->getPort();

        G.Out(pdGendoc,"joinFederationExecution====>send Message to RTIG");

        comm->sendMessage(&requete);

        // Waiting RTIG answer for FED file opened
        std::auto_ptr<NetworkMessage> reponse(comm->waitMessage(NetworkMessage::GET_FED_FILE, 0));

        if ( reponse->exception != e_NO_EXCEPTION)
            {
            // Bad answer from RTIG
            e = reponse->exception ;
            }
        else
            {
	    stat->rtiService(NetworkMessage::GET_FED_FILE);
            // RTIA have to open a new file for working
            // We have to build a name for working file, name begins by _RTIA_ (6 char)
            // First pid converted in char and added
            // Then federation name
            // File type (4)
            char pid_name[10];
            sprintf(pid_name,"%d_",getpid());

            filename = new char[6+strlen(pid_name)+strlen(Federation)+4+1] ;
            strcpy(filename,"_RTIA_");
            strcat(filename,pid_name); 
            strcat(filename,Federation);
            // Last file type : fed or xml ?
   
            string filename_RTIG = reponse->FEDid ;
            int nbcar_filename_RTIG=filename_RTIG.length();        
            string extension = filename_RTIG.substr(nbcar_filename_RTIG-3,3) ;
              if ( !strcasecmp(extension.c_str(),"fed") )
                  {
                  strcat(filename,".fed");
                  }
              else if  ( !strcasecmp(extension.c_str(),"xml") )
                  {
                  strcat(filename,".xml");
                  } 
              else 
                  throw CouldNotOpenFED("nor .fed nor .xml"); 
            // FED filename for working must be stored
            _FEDid =  new char[strlen(filename)+1] ;
            strcpy(_FEDid,filename) ;              
            // RTIA opens working file
            FILE *fdd;
            if ( (fdd=fopen(filename,"w")) == NULL )
                throw RTIinternalError("FED file has vanished.") ;
              
            // RTIA says RTIG OK for file transfer
            requeteFED.type = NetworkMessage::GET_FED_FILE ;
            requeteFED.federateName = Federate;
            requeteFED.FEDid = filename;            
            if ( e == e_NO_EXCEPTION)
                requeteFED.number = 0 ;  // OK for open
            else
                requeteFED.number = 1 ; 

            G.Out(pdGendoc,"joinFederationExecution====> begin FED file get from RTIG");

            comm->sendMessage(&requeteFED);
    
            // Now read loop from RTIG to get line contents and then write it into file
            char *file_line = NULL ;
            unsigned long length=0 ;
            int num_line = 0 ; // no line read            
            for (;;)
                {            	
                reponse.reset(comm->waitMessage(NetworkMessage::GET_FED_FILE, 0));
                if ( reponse->exception != e_NO_EXCEPTION)
                    {
                    cout << "Bad answer from RTIG" << endl ;
                    e = e_RTIinternalError ;
                    break ;
                    }
		stat->rtiService(NetworkMessage::GET_FED_FILE);
                // Line read
                num_line++ ;
                // Check for EOF
                if ( reponse->number == 0 )
                    break;
                assert ( num_line == reponse->number ) ;
                reponse->handleArraySize = 1 ;
                file_line = reponse->getValue(0,&length) ;
                int nbw = fputs(file_line,fdd);
                file_line = NULL ;
                // RTIA says OK to RTIG
                requeteFED.type = NetworkMessage::GET_FED_FILE ;
                requeteFED.federateName =Federate;
                requeteFED.number = num_line ; 
                requeteFED.FEDid = filename;

                comm->sendMessage(&requeteFED);            
                }
            // close working file
            fclose(fdd); 
            }                          
 
        G.Out(pdGendoc,"joinFederationExecution====> end FED file get from RTIG"); 
          
         // Waiting RTIG answer (from any federate)
        reponse.reset(comm->waitMessage(NetworkMessage::JOIN_FEDERATION_EXECUTION, 0));

        // If OK, regulators number is inside the answer.
        // Then we except a NULL message from each.
        if (reponse->exception == e_NO_EXCEPTION) {
            _nom_federation = new char[strlen(Federation)+1] ;
            strcpy(_nom_federation, Federation);
            strcpy(_nom_federe, Federate);
            _numero_federation = reponse->federation ;
            federate = reponse->federate ;
            tm->setFederate(reponse->federate);

#ifdef FEDERATION_USES_MULTICAST
            // creation du socket pour la communication best-effort
            comm->CreerSocketMC(reponse->AdresseMulticast, MC_PORT);
#endif

            nb = reponse->numberOfRegulators ;
            for (i=0 ; i<nb ; i++) {
                reponse.reset(comm->waitMessage(NetworkMessage::MESSAGE_NULL, 0));
                assert(tm != NULL);
                tm->insert(reponse->federate, reponse->date);
            }

            _est_membre_federation = true ;
            G.Out(pdGendoc,"exit(%d) FederationManagement::joinFederationExecution",federate);
            return(federate);
        }
        else
            e = reponse->exception ;
    }
    G.Out(pdGendoc,"exit(0) FederationManagement::joinFederationExecution");
    return(0);
}

// -------------------------------
// -- resignFederationExecution --
// -------------------------------

void
FederationManagement::resignFederationExecution(RTI::ResignAction,
                                                TypeException &e)
{
    NetworkMessage msg ;
    TypeException exception = e_NO_EXCEPTION ;

    e = e_NO_EXCEPTION ;

    D.Out(pdInit, "Resign Federation.");

    if (!_est_membre_federation)
        e = e_FederateNotExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        tm->StopperAvanceTemps();

        if (tm->requestRegulateurState())
            tm->setTimeRegulating(false, exception);

        msg.type = NetworkMessage::RESIGN_FEDERATION_EXECUTION ;
        msg.federation = _numero_federation ;
        msg.federate = federate ;

        comm->sendMessage(&msg);

        _est_membre_federation = false ;
        _numero_federation = 0 ;
        federate = 0 ;
        // Now, remove temporary file (if not yet done)
        if ( _FEDid != NULL)
            {
            if ( _FEDid[0] != '\0' )
                {
                std::remove(_FEDid);
                _FEDid[0] = '\0' ;
                }
            }

        // BUG: Voir DestroyFederation ou ~GF.
        if (!_est_createur_federation)
            _fin_execution = true ;
    }
}

// ----------------------------------------------------------------------------
//! Register synchronization.
void
FederationManagement::registerSynchronization(const char *label,
                                              const char *tag,
                                              TypeException &e)
{
    D.Out(pdProtocol, "RegisterSynchronization.");
    G.Out(pdGendoc,"enter FederationManagement::registerSynchronization");

    assert(label != NULL);

    e = e_NO_EXCEPTION ;

    list<char *>::const_iterator i = synchronizationLabels.begin();
    bool exists = false ;
    for (; i != synchronizationLabels.end(); i++) {
        if (!strcmp((*i), label)) {
            e = e_FederationAlreadyPaused ; // Label already pending.
            exists = true ;
            break ;
        }
    }

    if (!exists)
        synchronizationLabels.push_back(strdup(label));

    if (!_est_membre_federation)
        e = e_FederateNotExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        NetworkMessage req ;
        req.type = NetworkMessage::REGISTER_FEDERATION_SYNCHRONIZATION_POINT ;
        req.federation = _numero_federation ;
        req.federate = federate ;
        req.setLabel(label);
        req.setTag(tag);
        // no federates set so boolean must be false
        req.setBoolean(false) ;

        G.Out(pdGendoc,"      registerSynchronization====> send Message to RTIG");

        comm->sendMessage(&req);
    }

    G.Out(pdGendoc,"exit  FederationManagement::registerSynchronization");

}

// ----------------------------------------------------------------------------
//! Register synchronization with set of federates
void
FederationManagement::registerSynchronization(const char *label,
                                              const char *tag,
                                              unsigned short array_size,
                                              FederateHandle *fed_array,
                                              TypeException &e)
{
    D.Out(pdProtocol, "RegisterSynchronization.");
    G.Out(pdGendoc,"enter FederationManagement::registerSynchronization with federate set");

    assert(label != NULL);

    e = e_NO_EXCEPTION ;

    list<char *>::const_iterator i = synchronizationLabels.begin();
    bool exists = false ;
    for (; i != synchronizationLabels.end(); i++) {
        if (!strcmp((*i), label)) {
            e = e_FederationAlreadyPaused ; // Label already pending.
            exists = true ;
            break ;
        }
    }

    if (!exists)
        synchronizationLabels.push_back(strdup(label));

    if (!_est_membre_federation)
        e = e_FederateNotExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        NetworkMessage req ;
        req.type = NetworkMessage::REGISTER_FEDERATION_SYNCHRONIZATION_POINT ;
        req.federation = _numero_federation ;
        req.federate = federate ;
        req.setLabel(label);
        req.setTag(tag);
        // federates set exists so boolean must be true
        req.setBoolean(true) ;
        req.handleArraySize = array_size ;
        for ( int j=0 ; j < array_size ; j++)
            req.handleArray[j] = fed_array[j] ;

        G.Out(pdGendoc,"      registerSynchronization====> send Message to RTIG");

        comm->sendMessage(&req);
    }

    G.Out(pdGendoc,"exit  FederationManagement::registerSynchronization with federate set");

}
// ----------------------------------------------------------------------------
//! Unregister synchronization.
void
FederationManagement::unregisterSynchronization(const char *label,
                                                TypeException &e)
{
    D.Out(pdProtocol, "unregisterSynchronization.");

    assert(label != NULL);

    e = e_NO_EXCEPTION ;

    // Find if this label has been requested by federate or RTIG.
    list<char *>::iterator i = synchronizationLabels.begin();
    bool exists = false ;
    for (; i != synchronizationLabels.end(); ++i) {
        if (!strcmp((*i), label)) {
            // Label already pending.
            exists = true ;
            break ;
        }
    }
    if (!exists)
        e = e_UnknownLabel ;
    else {
      /* delete[] *i ;
       * the label has been allocated using strdup (i.e. malloc-like)
       * so that we MUST use free (and not delete[]) to free it :))
       */
      free(*i);
      synchronizationLabels.erase(i);
    }
    
    if (!_est_membre_federation)
        e = e_FederateNotExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        NetworkMessage req ;

        req.type = NetworkMessage::SYNCHRONIZATION_POINT_ACHIEVED ;
        req.federation = _numero_federation ;
        req.federate = federate ;
        req.setLabel(label);

        comm->sendMessage(&req);
    }
}

// ----------------------------------------------------------------------------
void
FederationManagement::announceSynchronizationPoint(const char *label,
                                                   const char *tag)
{
    D.Out(pdInit, "Announce Synchronization Point \"%s\"(%s).", label, tag);

    Message req;

    assert(label != NULL);

    req.type = Message::ANNOUNCE_SYNCHRONIZATION_POINT ;
    req.setLabel(label);
    req.setTag(tag);

    // adding label to list of synchronizations to be done.
    list<char *>::const_iterator i = synchronizationLabels.begin();
    bool exists = false ;
    for (; i != synchronizationLabels.end(); i++) {
        if (!strcmp((*i), label)) {
            // label exists (only if initiator).
            exists = true ;
            break ;
        }
    }
    if (!exists)
        synchronizationLabels.push_back(strdup(label));

    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
void FederationManagement::
synchronizationPointRegistrationFailed(const char *label)
{
    D.Out(pdInit, "Synchronization Point Registration Failed \"%s\".",
          label);

    Message req;

    G.Out(pdGendoc,"enter FederationManagement::synchronizationPointRegistrationFailed");

    assert(label != NULL);

    req.type = Message::SYNCHRONIZATION_POINT_REGISTRATION_FAILED ;
    req.setLabel(label);

    comm->requestFederateService(&req);

    G.Out(pdGendoc,"exit  FederationManagement::synchronizationPointRegistrationFailed");

}

// ----------------------------------------------------------------------------
void FederationManagement::
synchronizationPointRegistrationSucceeded(const char *label)
{
    D.Out(pdInit, "Synchronization Point Registration Succeeded \"%s\".",
          label);

    Message req;

    G.Out(pdGendoc,"enter FederationManagement::synchronizationPointRegistrationSucceeded");

    assert(label != NULL);

    req.type = Message::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED ;
    req.setLabel(label);

    comm->requestFederateService(&req);

    G.Out(pdGendoc,"exit  FederationManagement::synchronizationPointRegistrationSucceeded");

}

// ----------------------------------------------------------------------------
void
FederationManagement::federationSynchronized(const char *label)
{
    D.Out(pdInit, "Federation Synchronized \"%s\".", label);

    Message req;

    assert(label != NULL);

    req.type = Message::FEDERATION_SYNCHRONIZED ;
    req.setLabel(label);

    comm->requestFederateService(&req);
}

// ----------------------------------------------------------------------------
// requestFederationSave with time
void
FederationManagement::requestFederationSave(const char *label,
                                            FederationTime the_time,
                                            TypeException &e)
{
    D.Out(pdInit, "Request for federation save \"%s\".", label);
    G.Out(pdGendoc,"enter FederationManagement::requestFederationSave "
                   "with time");

    assert(label != 0);

    NetworkMessage req ;
    req.type = NetworkMessage::REQUEST_FEDERATION_SAVE ;
    req.date = the_time ;
    req.setLabel(label);
    req.federation = _numero_federation ;
    req.federate = federate ;
    // boolean true because with time
    req.setBoolean(true);

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
FederationManagement::requestFederationSave(const char *label,
                                            TypeException &e)
{
    D.Out(pdInit, "Request for federation save \"%s\".", label);
    G.Out(pdGendoc,"enter FederationManagement::requestFederationSave "
                   "without time");

    assert(label != 0);

    NetworkMessage req ;
    req.type = NetworkMessage::REQUEST_FEDERATION_SAVE ;
    req.setLabel(label);
    req.federation = _numero_federation ;
    req.federate = federate ;
    // boolean false because without time
    req.setBoolean(false);

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

    NetworkMessage req ;

    req.type = NetworkMessage::FEDERATE_SAVE_BEGUN ;
    req.federate = federate ;
    req.federation = _numero_federation ;

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

    NetworkMessage req ;

    req.type= status ? NetworkMessage::FEDERATE_SAVE_COMPLETE : NetworkMessage::FEDERATE_SAVE_NOT_COMPLETE ;
    req.federate = federate ;
    req.federation = _numero_federation ;

    if (status)
       {    
       G.Out(pdGendoc,"      federateSaveStatus ====>send Message F_S_C to RTIG");
       }
    else
       {    
       G.Out(pdGendoc,"      federateSaveStatus ====>send Message F_S_N_C to RTIG");
       }

    comm->sendMessage(&req);

    G.Out(pdGendoc,"exit  FederationManagement::federateSaveStatus");
}

// ----------------------------------------------------------------------------
void
FederationManagement::initiateFederateSave(const char *label)
{
    G.Out(pdGendoc,"enter FederationManagement::initiateFederateSave");
    D.Out(pdInit, "Initiate a federate save \"%s\".", label);

    savingState = true ;

    Message req;

    assert(label != 0);

    req.type = Message::INITIATE_FEDERATE_SAVE ;
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

    Message req;

    req.type = status ? Message::FEDERATION_SAVED : Message::FEDERATION_NOT_SAVED ;

    comm->requestFederateService(&req);

    G.Out(pdGendoc,"exit  FederationManagement::federationSavedStatus");
}

// ----------------------------------------------------------------------------
void
FederationManagement::requestFederationRestore(const char *label,
                                               TypeException &)
{
    G.Out(pdGendoc,"enter FederationManagement::requestFederationRestore");
    D.Out(pdInit, "Request for federation restore \"%s\".", label);

    assert(label != NULL);

    NetworkMessage req ;
    req.type = NetworkMessage::REQUEST_FEDERATION_RESTORE ;
    req.setLabel(label);
    req.federate = federate ;
    req.federation = _numero_federation ;

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

    NetworkMessage req ;
    req.federate = federate ;
    req.federation = _numero_federation ;
    if (status)
        req.type = NetworkMessage::FEDERATE_RESTORE_COMPLETE ;
    else
        req.type = NetworkMessage::FEDERATE_RESTORE_NOT_COMPLETE ;

    comm->sendMessage(&req);
}

// ----------------------------------------------------------------------------
void
FederationManagement::requestFederationRestoreStatus(bool status,
                                                     const char *label,
                                                     const char *reason)
{
    G.Out(pdGendoc,"enter FederationManagement::requestFederationRestoreStatus");
    D.Out(pdInit, "Federation restore request %saccepted",
          status ? "" : "not ");

    Message req;

    req.setLabel(label);

    if (status)
        req.type = Message::REQUEST_FEDERATION_RESTORE_SUCCEEDED ;
    else {
        req.type = Message::REQUEST_FEDERATION_RESTORE_FAILED ;
        req.setTag(reason);
    }

    comm->requestFederateService(&req);
    G.Out(pdGendoc,"exit  FederationManagement::requestFederationRestoreStatus");
}

// ----------------------------------------------------------------------------
void
FederationManagement::federationRestoreBegun()
{
    G.Out(pdGendoc,"enter FederationManagement::federationRestoreBegun");
    D.Out(pdInit, "Federation restore begun");

    Message req;
    req.type = Message::FEDERATION_RESTORE_BEGUN ;

    comm->requestFederateService(&req);

    G.Out(pdGendoc,"exit  FederationManagement::federationRestoreBegun");
}

// ----------------------------------------------------------------------------
void
FederationManagement::initiateFederateRestore(const char *label,
                                              FederateHandle handle)
{
    G.Out(pdGendoc,"enter FederationManagement::initiateFederateRestore");
    D.Out(pdInit, "Initiate federate restore \"%s\" with federate handle %d.",
          label, handle);

    restoringState = true ;

    Message req;
    req.type = Message::INITIATE_FEDERATE_RESTORE ;
    req.setFederate(handle);
    req.setLabel(label);

    comm->requestFederateService(&req);
    G.Out(pdGendoc,"exit  FederationManagement::initiateFederateRestore");
}

// ----------------------------------------------------------------------------
void
FederationManagement::federationRestoredStatus(bool status)
{
    D.Out(pdInit, "Federation %srestored.", status ? "" : "not ");

    restoringState = false ;

    Message req;

    if (status)
        req.type = Message::FEDERATION_RESTORED ;
    else
        req.type = Message::FEDERATION_NOT_RESTORED ;

    comm->requestFederateService(&req);
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

// $Id: FederationManagement.cc,v 3.50.2.1 2008/03/18 15:55:57 erk Exp $
