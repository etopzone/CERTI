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
// $Id: RTIG_processing.cc,v 3.56.2.5 2008/04/10 19:55:47 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIG.hh"
#include "NM_Classes.hh"

#include <assert.h>

using std::endl ;
using std::cout ;

namespace certi {
namespace rtig {

static PrettyDebug D("RTIG", __FILE__);
static PrettyDebug G("GENDOC",__FILE__);

// ----------------------------------------------------------------------------
//! Creates a new federation.
void
RTIG::processCreateFederation(Socket *link, NetworkMessage *req)
{
    std::string federation = req->federationName;
    std::string FEDid      = req->FEDid;    
    NM_Create_Federation_Execution  rep;   // rep for answer to RTIA

    G.Out(pdGendoc,"enter RTIG::processCreateFederation");
    G.Out(pdGendoc,"BEGIN ** CREATE FEDERATION SERVICE **");

    if (federation.length() == 0) 
        {
        G.Out(pdGendoc,"exit  RTIG::processCreateFederation on exception RTIinternalError");        
        throw RTIinternalError("Invalid Federation Name.");
        }

    //auditServer << "Federation Name : " << federation ;
    Handle h = federationHandles.provide();

#ifdef FEDERATION_USES_MULTICAST
    // multicast base address
    unsigned long base_adr_mc = inet_addr(ADRESSE_MULTICAST);
    SocketMC *com_mc = NULL ;

    // creer la communication multicast
    com_mc = new SocketMC();
    if (com_mc == NULL) {
        D.Out(pdExcept, "Unable to allocate Multicast socket.");
        G.Out(pdGendoc,"exit  RTIG::processCreateFederation on exception RTIinternalError")
        throw RTIinternalError("Unable to allocate Multicast socket.");
    }

    com_mc->CreerSocketMC(base_adr_mc + h, MC_PORT);

    // inserer la nouvelle federation dans la liste des federations
    federations->createFederation(federation, h, com_mc);

    // inserer descripteur fichier pour le prochain appel a un select
    ClientSockets.push_front(com_mc);

#else    
    // We catch createFederation because it is useful to send
    // exception reason to RTIA 
    try {
        federations.createFederation(federation.c_str(), h, FEDid.c_str());
        }
    catch (CouldNotOpenFED e)
        {
        rep.exception = e_CouldNotOpenFED ;
        rep.exceptionReason=e._reason;
        }
    catch (ErrorReadingFED e)
        {
        rep.exception = e_ErrorReadingFED ;
        rep.exceptionReason = e._reason ;
        }
    catch (FederationExecutionAlreadyExists e)
        {
        rep.exception = e_FederationExecutionAlreadyExists ;
        rep.exceptionReason =e._reason ;
        }
#endif
    // Prepare answer for RTIA : store NetworkMessage rep    
    if ( rep.exception == e_NO_EXCEPTION )
        {
        rep.federation = h ;
        rep.FEDid = FEDid;        
        rep.federationName = federation;        
        }

    G.Out(pdGendoc,"processCreateFederation===>write answer to RTIA");

    rep.send(link); // Send answer to RTIA

    D.Out(pdInit, "Federation \"%s\" created with Handle %d.",
          federation.c_str(), rep.federation);

    G.Out(pdGendoc,"END ** CREATE FEDERATION SERVICE **");
    G.Out(pdGendoc,"exit RTIG::processCreateFederation");
}

// ----------------------------------------------------------------------------
//! Add a new federate to federation.
void
RTIG::processJoinFederation(Socket *link, NetworkMessage *req)
{
    std::string federation = req->federationName ;
    std::string federate = req->federateName ;
    std::string filename ;
    
    unsigned int peer = req->bestEffortPeer ;
    unsigned long address = req->bestEffortAddress ;

    Handle num_federation ;
    FederateHandle num_federe ;

    int nb_regulateurs ;
    int nb_federes ;
    bool pause ;

    G.Out(pdGendoc,"BEGIN ** JOIN FEDERATION SERVICE **");
    G.Out(pdGendoc,"enter RTIG::processJoinFederation");

    if ((federation.length()==0) || (federate.length() == 0))
        throw RTIinternalError("Invalid Federation/Federate Name.");

    auditServer << "Federate \"" << federate.c_str() << "\" joins Federation \""
		<< federation.c_str() << "\"" ;

    federations.exists(federation.c_str(), num_federation);

    num_federe = federations.addFederate(num_federation,
                                          federate.c_str(),
                                          (SecureTCPSocket *) link);

#ifdef FEDERATION_USES_MULTICAST
    SocketMC *com_mc = NULL ;

    federations.info(num_federation, nb_federes, nb_regulateurs,
                      pause, com_mc);
    assert(com_mc != NULL);
#else
    filename = federations.info(num_federation, nb_federes, nb_regulateurs, pause);
#endif

    // Store Federate <->Socket reference.
    socketServer.setReferences(link->returnSocket(),
                                num_federation,
                                num_federe,
                                address,
                                peer);

    auditServer << "(" << num_federation << ")with handle " << num_federe
		<< ". Socket " << link->returnSocket();

    // Prepare answer about JoinFederationExecution
    // This answer wille be made AFTER FED file processing
    NM_Join_Federation_Execution rep ;        
    rep.federationName = federation;    
    rep.federate = num_federe ;
    rep.federation = num_federation ;
    rep.numberOfRegulators = nb_regulateurs ;
    rep.bestEffortPeer = peer ;
    rep.bestEffortAddress = address ;

    // Here begin FED file processing i.e. RTIG gives FED file contents to RTIA
    TypeException e = e_NO_EXCEPTION ;
    // Open FED file and says to RTIA if success
    FILE *fdd ;
    if ( (fdd=fopen(filename.c_str(),"r")) == NULL )
        {
        // Problem : file has been opened during create federation and now we can't
        // May be file has been deleted       
        cout << "processJoinFederation : FED file " << filename << " has vanished." << endl;
        e = e_RTIinternalError ;
        }

    // RTIG says OK or not to RTIA
    NM_Get_FED_File repFED ;    
    repFED.federate = num_federe ;
    repFED.federation = num_federation ;
    repFED.number = 0 ;
    repFED.FEDid = filename ;    
    repFED.exception = e ;
    // Send answer
    D.Out(pdTrace,"send NetworkMessage of Type %d after open \"%s\"",
          repFED.getType(),repFED.FEDid.c_str());
    G.Out(pdGendoc,"processJoinFederation====>Begin FED file transfer");

    repFED.send(link);

    if ( e ==  e_NO_EXCEPTION )  
        {
        // Wait for OK from RTIA
    	NM_Get_FED_File msg ;        
        D.Out(pdTrace,"wait NetworkMessage of Type %d",msg.getType());
        msg.receive(link);
        assert ( msg.number == 0 );
        // RTIA has opened working file then RTIG has to transfer file contents
        // line by line
        char file_line[MAX_BYTES_PER_VALUE+1];  
        int num_line = 0 ;
        while ( fgets(file_line,MAX_BYTES_PER_VALUE,fdd) != NULL )
            {
            num_line++;
            // RTIG sends line to RTIA and number gives line number            
            repFED.exception = e_NO_EXCEPTION ;
            repFED.federate = num_federe ;
            repFED.federation = num_federation ;
            repFED.number = num_line ;
            repFED.FEDid = filename;            
            // line transfered
            repFED.setFEDLine(std::string(file_line));            
            // Send answer
            repFED.send(link);

            // Wait for OK from RTIA
            msg.receive(link);
            assert ( msg.number == num_line );
            }
    
	// close
	fclose(fdd) ;        
        repFED.exception = e_NO_EXCEPTION ;
        repFED.federate = num_federe ;
        repFED.federation = num_federation ;
        repFED.number = 0 ;
        repFED.FEDid = filename;        

        // Send answer

        G.Out(pdGendoc,"processJoinFederation====>End  FED file transfer");

        repFED.send(link);
        }
    // END of FED file processing

    // Now we have to answer about JoinFederationExecution

#ifdef FEDERATION_USES_MULTICAST
    rep.AdresseMulticast = com_mc->returnAdress();
#endif

    D.Out(pdInit, "Federate \"%s\" has joined Federation %u under handle %u.",
          federate.c_str(), num_federation, num_federe);

    // Send answer

    rep.send(link);

    G.Out(pdGendoc,"exit RTIG::processJoinFederation");
    G.Out(pdGendoc,"END ** JOIN FEDERATION SERVICE **");

}

// ----------------------------------------------------------------------------
//! Removes a federate from federation.
void
RTIG::processResignFederation(Handle federation,
                              FederateHandle federe)
{

    G.Out(pdGendoc,"BEGIN ** RESIGN FEDERATION SERVICE **");
    G.Out(pdGendoc,"enter RTIG::processResignFederation");

    federations.remove(federation, federe);
    D.Out(pdInit, "Federate %u is resigning from federation %u.",
          federe, federation);

    G.Out(pdGendoc,"exit RTIG::processResignFederation");
    G.Out(pdGendoc,"END ** RESIGN FEDERATION SERVICE **");

}

// ----------------------------------------------------------------------------
//! Removes a federation.
void
RTIG::processDestroyFederation(Socket *link, NetworkMessage *req)
{
    Handle num_federation ;
    std::string federation = req->federationName ;

    G.Out(pdGendoc,"enter RTIG::processDestroyFederation");
    G.Out(pdGendoc,"BEGIN ** DESTROY FEDERATION SERVICE **");

    if (federation.length() == 0) throw RTIinternalError("Invalid Federation Name.");

    auditServer << "Name \"" << federation.c_str() << "\"" ;
    federations.exists(federation.c_str(), num_federation);
    federations.destroyFederation(num_federation);
    federationHandles.free(num_federation);
    D.Out(pdInit, "Federation \"%s\" has been destroyed.", federation.c_str());

    NM_Destroy_Federation_Execution rep ;    
    rep.federate = req->federate ;
    rep.federationName = req->federationName;    

    G.Out(pdGendoc,"processDestroyFederation===>write to RTIA");

    rep.send(link);

    G.Out(pdGendoc,"END ** DESTROY FEDERATION SERVICE **");
    G.Out(pdGendoc,"exit RTIG::processDestroyFederation");
}

// ----------------------------------------------------------------------------
//! Put federate as time regulating.
void
RTIG::processSetTimeRegulating(NetworkMessage *msg)
{
    if (msg->regulator) {
        auditServer << "ON at time " << msg->getDate();

        federations.createRegulator(msg->federation,
                                     msg->federate,
                                     msg->getDate());
        D.Out(pdTerm, "Federate %u of Federation %u sets TimeRegulation ON.",
              msg->federate, msg->federation);
    }
    else {
        auditServer << "OFF" ;

        federations.removeRegulator(msg->federation,
                                     msg->federate);
        D.Out(pdTerm, "Federate %u of Federation %u sets TimeRegulation OFF.",
              msg->federate, msg->federation);
    }
}

// ----------------------------------------------------------------------------
//! Put federate as time constrained
void
RTIG::processSetTimeConstrained(NetworkMessage *msg)
{
    if (msg->constrained) {
        auditServer << "ON at time " << msg->getDate();

        federations.addConstrained(msg->federation,
                                    msg->federate);
        D.Out(pdTerm, "Federate %u of Federation %u is now constrained.",
              msg->federate, msg->federation);
    }
    else {
        auditServer << "OFF" ;

        federations.removeConstrained(msg->federation,
                                       msg->federate);
        D.Out(pdTerm, "Federate %u of Federation %u is no more constrained.",
              msg->federate, msg->federation);
    }
}

// ----------------------------------------------------------------------------
//! processMessageNull.
void
RTIG::processMessageNull(NetworkMessage *msg)
{
    auditServer << "Date " << msg->getDate() ;

    // Catch all exceptions because RTIA does not expect an answer anyway.
    try {
        federations.updateRegulator(msg->federation,
                                     msg->federate,
                                     msg->getDate());
    } catch (Exception &e) {}
}

// ----------------------------------------------------------------------------
//! processRegisterSynchronization.
void
RTIG::processRegisterSynchronization(Socket *link, NetworkMessage *req)
{

    G.Out(pdGendoc,"BEGIN ** REGISTER FEDERATION SYNCHRONIZATION POINT Service **");
    G.Out(pdGendoc,"enter RTIG::processRegisterSynchronization");

    auditServer << "Label \"" << req->getLabel().c_str() << "\" registered. Tag is \""
		<< req->getTag().c_str() << "\"" ;

    // boolean true means a federates set exists
    if ( req->boolean )
        federations.manageSynchronization(req->federation,
                                          req->federate,
                                          true,
                                          req->getLabel().c_str(),
                                          req->getTag().c_str(),
                                          req->handleArraySize,
                                          req->handleArray);
    else
        federations.manageSynchronization(req->federation,
                                          req->federate,
                                          true,
                                          req->getLabel().c_str(),
                                          req->getTag().c_str());
    D.Out(pdTerm, "Federation %u is now synchronizing.", req->federation);

    // send synchronizationPointRegistrationSucceeded() to federate.
    NM_Synchronization_Point_Registration_Succeeded rep ;    
    rep.federate = req->federate ;
    rep.federation = req->federation ;
    rep.setLabel(req->getLabel().c_str());

    G.Out(pdGendoc,"      processRegisterSynchronization====> write SPRS to RTIA");

    rep.send(link);

    // boolean true means a federates set exists
    if ( req->boolean )
        federations.broadcastSynchronization(req->federation,
                                          req->federate,
                                          req->getLabel().c_str(),
                                          req->getTag().c_str(),
                                          req->handleArraySize,
                                          req->handleArray);
    else
        federations.broadcastSynchronization(req->federation,
                                          req->federate,
                                          req->getLabel().c_str(),
                                          req->getTag().c_str());

    G.Out(pdGendoc,"exit  RTIG::processRegisterSynchronization");
    G.Out(pdGendoc,"END   ** REGISTER FEDERATION SYNCHRONIZATION POINT Service **");

}

// ----------------------------------------------------------------------------
//! processSynchronizationAchieved.
void
RTIG::processSynchronizationAchieved(Socket *, NetworkMessage *req)
{
    auditServer << "Label \"" << req->getLabel().c_str() << "\" ended." ;

    federations.manageSynchronization(req->federation,
                                       req->federate,
                                       false,
                                       req->getLabel().c_str(),
                                       "");
    D.Out(pdTerm, "Federate %u has synchronized.", req->federate);
}

// ----------------------------------------------------------------------------
void
RTIG::processRequestFederationSave(Socket *, NetworkMessage *req)
{
    G.Out(pdGendoc,"BEGIN ** REQUEST FEDERATION SAVE SERVICE **");
    G.Out(pdGendoc,"enter RTIG::processRequestFederationSave");

    auditServer << "Federation save request." ;

    if ( req->boolean )
        // With time
        federations.requestFederationSave(req->federation, req->federate,
                                          req->getLabel().c_str(), req->getDate());
    else
        // Without time
        federations.requestFederationSave(req->federation, req->federate,
                                          req->getLabel().c_str());

    G.Out(pdGendoc,"exit  RTIG::processRequestFederationSave");
    G.Out(pdGendoc,"END   ** REQUEST FEDERATION SAVE SERVICE **");
}
// ----------------------------------------------------------------------------
void
RTIG::processFederateSaveBegun(Socket *, NetworkMessage *req)
{
    G.Out(pdGendoc,"enter RTIG::processFederateSaveBegun");
    G.Out(pdGendoc,"BEGIN ** FEDERATE SAVE BEGUN SERVICE **");
    G.Out(pdGendoc,"processFederateSaveBegun federation = %d",req->federation);

    auditServer << "Federate " << req->federate << " save begun." ;

    federations.federateSaveBegun(req->federation, req->federate);

    G.Out(pdGendoc,"END   ** FEDERATE SAVE BEGUN SERVICE **");
    G.Out(pdGendoc,"exit  RTIG::processFederateSaveBegun");
}

// ----------------------------------------------------------------------------
void
RTIG::processFederateSaveStatus(Socket *, NetworkMessage *req)
{
    G.Out(pdGendoc,"enter RTIG::processFederateSaveStatus");
    if (req->getType() == NetworkMessage::FEDERATE_SAVE_COMPLETE)
        G.Out(pdGendoc,"BEGIN ** FEDERATE SAVE COMPLETE SERVICE **");
    else
        G.Out(pdGendoc,"BEGIN ** FEDERATE SAVE NOT COMPLETE SERVICE **");

    auditServer << "Federate " << req->federate << " save ended." ;

    bool status = req->getType() == NetworkMessage::FEDERATE_SAVE_COMPLETE ;
    federations.federateSaveStatus(req->federation, req->federate, status);

    G.Out(pdGendoc,"exit  END   ** FEDERATE SAVE (NOT) COMPLETE SERVICE **");
    G.Out(pdGendoc,"exit  RTIG::processFederateSaveStatus");
}

// ----------------------------------------------------------------------------
void
RTIG::processRequestFederationRestore(Socket *, NetworkMessage *req)
{
    G.Out(pdGendoc,"BEGIN ** REQUEST FEDERATION RESTORE SERVICE **");
    G.Out(pdGendoc,"enter RTIG::processRequestFederationRestore");
    auditServer << "Federate " << req->federate << " request restore." ;

    federations.requestFederationRestore(req->federation, req->federate,
                                          req->getLabel().c_str());
    G.Out(pdGendoc,"exit  RTIG::processRequestFederationRestore");
    G.Out(pdGendoc,"END   ** REQUEST FEDERATION RESTORE SERVICE **");
}

// ----------------------------------------------------------------------------
void
RTIG::processFederateRestoreStatus(Socket *, NetworkMessage *req)
{
    G.Out(pdGendoc,"BEGIN ** FEDERATE RESTORE (NOT)COMPLETE **");
    G.Out(pdGendoc,"enter RTIG::processRequestFederateRestoreStatus");
    auditServer << "Federate " << req->federate << " restore ended." ;

    bool status = req->getType() == NetworkMessage::FEDERATE_RESTORE_COMPLETE ;

    federations.federateRestoreStatus(req->federation, req->federate, status);

    G.Out(pdGendoc,"exit  RTIG::processRequestFederateRestoreStatus");
    G.Out(pdGendoc,"END   ** FEDERATE RESTORE (NOT)COMPLETE **");
}

// ----------------------------------------------------------------------------
// processPublishObjectClass
void
RTIG::processPublishObjectClass(Socket *link, NetworkMessage *req)
{
    bool pub = (req->getType() == NetworkMessage::PUBLISH_OBJECT_CLASS);

    auditServer << "Class = " << req->objectClass << ", # of att. = " 
		<< req->handleArraySize ;

    federations.publishObject(req->federation,
                               req->federate,
                               req->objectClass,
                               req->handleArray,
                               req->handleArraySize,
                               pub);

    D.Out(pdRegister, "Federate %u of Federation %u published object class %d.",
          req->federate, req->federation, req->objectClass);

    std::auto_ptr<NetworkMessage> rep(NM_Factory::create(req->getType()));    
    rep->federate = req->federate ;
    rep->objectClass = req->objectClass ;
    rep->handleArraySize = 0 ;

    rep->send(link); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processSubscribeObjectClass
void
RTIG::processSubscribeObjectClass(Socket *link, NetworkMessage *req)
{
    G.Out(pdGendoc,"enter RTIG::processSubscribeObjectClass");
    G.Out(pdGendoc,"BEGIN **  SUBSCRIBE OBJECT CLASS SERVICE **");

    bool sub = (req->getType() == NetworkMessage::SUBSCRIBE_OBJECT_CLASS);

    auditServer << "Class = " << req->objectClass
		<< ", # of att. = " << req->handleArraySize ;

    federations.subscribeObject(req->federation,
				req->federate,
				req->objectClass,
				sub ? req->handleArray : 0,
				sub ? req->handleArraySize : 0);

    D.Out(pdRegister,
          "Federate %u of Federation %u subscribed to object class %d.",
          req->federate, req->federation, req->objectClass);

    std::auto_ptr<NetworkMessage> rep(NM_Factory::create(req->getType()));
    rep->federate = req->federate ;
    rep->objectClass = req->objectClass ;
    rep->handleArraySize = 0 ;

    rep->send(link); // send answer to RTIA

    G.Out(pdGendoc,"END   **  SUBSCRIBE OBJECT CLASS SERVICE **");
    G.Out(pdGendoc,"exit  RTIG::processSubscribeObjectClass");
}

// ----------------------------------------------------------------------------
// processPublishInteractionClass
void
RTIG::processPublishInteractionClass(Socket *link, NetworkMessage *req)
{
    assert(link != NULL && req != NULL);

    bool pub = (req->getType() == NetworkMessage::PUBLISH_INTERACTION_CLASS);

    auditServer << "Class = " << req->interactionClass ;
    federations.publishInteraction(req->federation,
                                    req->federate,
                                    req->interactionClass,
                                    pub);
    D.Out(pdRequest, "Federate %u of Federation %u publishes Interaction %d.",
          req->federate,
          req->federation,
          req->interactionClass);

    std::auto_ptr<NetworkMessage> rep(NM_Factory::create(req->getType())); ;   
    rep->federate = req->federate ;
    rep->interactionClass = req->interactionClass ;

    rep->send(link); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processSubscribeInteractionClass
void
RTIG::processSubscribeInteractionClass(Socket *link, NetworkMessage *req)
{
    bool sub = (req->getType() == NetworkMessage::SUBSCRIBE_INTERACTION_CLASS);

    auditServer << "Class = %u" << req->interactionClass ;
    federations.subscribeInteraction(req->federation,
                                      req->federate,
                                      req->interactionClass,
                                      sub);
    D.Out(pdRequest,
          "Federate %u of Federation %u subscribed to Interaction %d.",
          req->federate,
          req->federation,
          req->interactionClass);

    std::auto_ptr<NetworkMessage> rep(NM_Factory::create(req->getType()));;    
    rep->federate = req->federate ;
    rep->interactionClass = req->interactionClass ;

    rep->send(link); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processRegisterObject
void
RTIG::processRegisterObject(Socket *link, NetworkMessage *req)
{
	std::auto_ptr<NetworkMessage> rep(NM_Factory::create(req->getType()));;

    auditServer << "Class = %u" << req->objectClass ;
    rep->object = federations.registerObject(req->federation,
                                             req->federate,
                                             req->objectClass,
                                             const_cast<char*>(req->getLabel().c_str()));
    auditServer << ", Handle = " << rep->object ;

    D.Out(pdRegister,
          "Object \"%s\" of Federate %u has been registered under ID %u.",
          req->getLabel().c_str(), req->federate, rep->object);

    rep->federate = req->federate ;
    rep->setLabel(req->getLabel());
    // rep.object is set by the call of registerObject

    rep->send(link); // Send answer to RTIA
}

// ----------------------------------------------------------------------------
// processUpdateAttributeValues
void
RTIG::processUpdateAttributeValues(Socket *link, NetworkMessage *req)
{
    ValueLengthPair *ValueArray = NULL ;

    auditServer << "ObjID = " << req->object
		<< ", Date = " << req->getDate() ;

    // Get Value Array
    ValueArray = req->getAttribValueArray();

    // Forward the call
    if ( req->getBoolean() )
        {
        // UAV with time
        federations.updateAttribute(req->federation,
                                 req->federate,
                                 req->object,
                                 req->handleArray,
                                 ValueArray,
                                 req->handleArraySize,
                                 req->getDate(),
                                 req->getLabel().c_str());
        }
    else
        {
        // UAV without time
        federations.updateAttribute(req->federation,
                                 req->federate,
                                 req->object,
                                 req->handleArray,
                                 ValueArray,
                                 req->handleArraySize,
                                 req->getLabel().c_str());
        }
    free(ValueArray);

    // Building answer (Network Message re)
    NM_Update_Attribute_Values rep ;
    rep.federate = req->federate ;
    rep.object = req->object ;
    rep.setDate(req->getDate());
    rep.handleArraySize = 0 ;
    // Don't forget label and tag
    rep.setLabel(req->getLabel());
    rep.setTag(req->getTag());

    rep.send(link); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processSendInteraction
void
RTIG::processSendInteraction(Socket *link, NetworkMessage *req)
{
    ValueLengthPair *values = NULL ;

    G.Out(pdGendoc,"BEGIN ** SEND INTERACTION SERVICE **");
    G.Out(pdGendoc,"enter RTIG::processSendInteraction");

    // Building Value Array
    auditServer << "IntID = " << req->interactionClass
		<< ", date = " << req->getDate() ;
    values = req->getParamValueArray();

    if ( req->getBoolean() )
        {
        federations.updateParameter(req->federation,
				req->federate,
				req->interactionClass,
				req->handleArray,
				values,
				req->handleArraySize,
				req->getDate(),
				req->region,
				req->getLabel().c_str());
        }
    else
        {
        federations.updateParameter(req->federation,
				req->federate,
				req->interactionClass,
				req->handleArray,
				values,
				req->handleArraySize,
				req->region,
				req->getLabel().c_str());
        }
    free(values);

    D.Out(pdDebug, "Interaction %d parameters update completed",
          req->interactionClass);

    NM_Send_Interaction rep ;    
    rep.federate = req->federate ;
    rep.interactionClass = req->interactionClass ;
    rep.handleArraySize = 0 ;
    // Don't forget label and tag
    rep.setLabel(req->getLabel());
    rep.setTag(req->getTag());
    G.Out(pdGendoc,"processSendInteraction===>write");
    rep.send(link); // send answer to RTIA

    G.Out(pdGendoc,"exit RTIG::processSendInteraction");
    G.Out(pdGendoc,"END ** SEND INTERACTION SERVICE **");

}

// ----------------------------------------------------------------------------
// processDeleteObject
void
RTIG::processDeleteObject(Socket *link, NetworkMessage *req)
{
    G.Out(pdGendoc,"BEGIN ** DELETE OBJECT INSTANCE service **");
    G.Out(pdGendoc,"enter RTIG::processDeleteObject");
    auditServer << "ObjID = %u" << req->object ;

    if ( req->getBoolean() ) {
    	federations.destroyObject(req->federation,
        	                  req->federate,
                                  req->object,
				  req->getDate(),
                                  const_cast<char*>(req->getLabel().c_str()));
    }
    else {
    	federations.destroyObject(req->federation,
        	                  req->federate,
                                  req->object,
                                  const_cast<char*>(req->getLabel().c_str()));
    }

    D.Out(pdRegister, "Object # %u of Federation %u has been deleted.",
          req->object, req->federation);

    NM_Delete_Object rep ;    
    rep.federate = req->federate ;
    rep.object = req->object ;

    rep.send(link); // send answer to RTIA
    
    G.Out(pdGendoc,"exit RTIG::processDeleteObject");
    G.Out(pdGendoc,"END ** DELETE OBJECT INSTANCE **");
}

// ----------------------------------------------------------------------------
// processqueryAttributeOwnership
void
RTIG::processQueryAttributeOwnership(Socket *link, NetworkMessage *req)
{
    D.Out(pdDebug, "Owner of Attribute %u of Object %u .",
          req->handleArray[0], req->object);

    auditServer << "AttributeHandle = " << req->handleArray[0] ;

    federations.searchOwner(req->federation,
                             req->federate,
                             req->object,
                             req->handleArray[0]);

    D.Out(pdDebug, "Owner of Attribute %u of Object %u .",
          req->handleArray[0], req->object);

    NM_Query_Attribute_Ownership rep ;    
    rep.federate = req->federate ;
    rep.object = req->object ;

    rep.send(link); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processAttributeOwnedByFederate
void
RTIG::processAttributeOwnedByFederate(Socket *link, NetworkMessage *req)
{
    NM_Is_Attribute_Owned_By_Federate rep ;

    D.Out(pdDebug, "Owner of Attribute %u of Object %u .",
          req->handleArray[0], req->object);

    auditServer << "AttributeHandle = " << req->handleArray[0] ;

    if (federations.isOwner(req->federation,
                             req->federate,
                             req->object,
                             req->handleArray[0]))
        rep.setLabel("RTI_TRUE");
    else
        rep.setLabel("RTI_FALSE");

    D.Out(pdDebug, "Owner of Attribute %u of Object %u .",
          req->handleArray[0], req->object);
    
    rep.federate = req->federate ;
    rep.object = req->object ;

    rep.send(link); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processNegotiatedOwnershipDivestiture
void
RTIG::processNegotiatedOwnershipDivestiture(Socket *link, NetworkMessage *req)
{
    auditServer << "Object = " <<  req->object
		<< ", # of att. = %u" << req->handleArraySize ;
    federations.negotiateDivestiture(req->federation,
                                      req->federate,
                                      req->object,
                                      req->handleArray,
                                      req->handleArraySize,
                                      req->getLabel().c_str());

    D.Out(pdDebug, "Federate %u of Federation %u negotiate "
          "divestiture of object %u.",
          req->federate, req->federation, req->object);

    NM_Negotiated_Attribute_Ownership_Divestiture rep ;    
    rep.federate = req->federate ;
    rep.object = req->object ;
    rep.handleArraySize = 0 ;

    rep.send(link); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processAcquisitionIfAvailable
void
RTIG::processAcquisitionIfAvailable(Socket *link, NetworkMessage *req)
{
    auditServer << "Object = " << req->object
		<< ", # of att. = " << req->handleArraySize ;

    federations.acquireIfAvailable(req->federation,
                                    req->federate,
                                    req->object,
                                    req->handleArray,
                                    req->handleArraySize);

    D.Out(pdDebug,
          "Federate %u of Federation %u acquisitionIfAvailable "
          "of object %u.",
          req->federate, req->federation, req->object);

    NM_Attribute_Ownership_Acquisition_If_Available rep ;   
    rep.federate = req->federate ;
    rep.object = req->object ;
    rep.handleArraySize = 0 ;

    rep.send(link); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processUnconditionalDivestiture
void
RTIG::processUnconditionalDivestiture(Socket *link, NetworkMessage *req)
{
    auditServer << "Object = " << req->object
		<< ", # of att. = %u" << req->handleArraySize ;

    federations.divest(req->federation,
                        req->federate,
                        req->object,
                        req->handleArray,
                        req->handleArraySize);

    D.Out(pdDebug,
          "Federate %u of Federation %u UnconditionalDivestiture "
          "of object %u.",
          req->federate, req->federation, req->object);

    NM_Unconditional_Attribute_Ownership_Divestiture rep ;    
    rep.federate = req->federate ;
    rep.object = req->object ;
    rep.handleArraySize = 0 ;

    rep.send(link); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processOwnershipAcquisition
void
RTIG::processOwnershipAcquisition(Socket *link, NetworkMessage *req)
{
    auditServer << "Object = " << req->object
		<< ", # of att. = %u" << req->handleArraySize ;

    federations.acquire(req->federation,
                         req->federate,
                         req->object,
                         req->handleArray,
                         req->handleArraySize,
                         req->getLabel().c_str());

    D.Out(pdDebug,
          "Federate %u of Federation %u ownership acquisition of object %u.",
          req->federate, req->federation, req->object);

    NM_Attribute_Ownership_Acquisition rep ;    
    rep.federate = req->federate ;
    rep.object = req->object ;
    rep.handleArraySize = 0 ;

    rep.send(link); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processCancelNegotiatedDivestiture
void
RTIG::processCancelNegotiatedDivestiture(Socket *link, NetworkMessage *req)
{
    auditServer << "Object = " << req->object
		<< ", # of att. = " << req->handleArraySize ;

    federations.cancelDivestiture(req->federation,
                                   req->federate,
                                   req->object,
                                   req->handleArray,
                                   req->handleArraySize);

    D.Out(pdDebug, "Federate %u of Federation %u cancel negotiate "
          "divestiture of object %u.",
          req->federate, req->federation, req->object);

    NM_Cancel_Negotiated_Attribute_Ownership_Divestiture rep ;    
    rep.federate = req->federate ;
    rep.object = req->object ;
    rep.handleArraySize = 0 ;

    rep.send(link); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processReleaseResponse
void
RTIG::processReleaseResponse(Socket *link, NetworkMessage *req)
{
    auditServer << "Object = " << req->object
		<< ", # of att. = " << req->handleArraySize ;

    AttributeHandleSet *attributes =
        federations.respondRelease(req->federation,
                                    req->federate,
                                    req->object,
                                    req->handleArray,
                                    req->handleArraySize);

    D.Out(pdDebug, "Federate %u of Federation %u release response "
          "of object %u.",
          req->federate, req->federation, req->object);

    NM_Attribute_Ownership_Release_Response rep ;
    rep.handleArraySize = attributes->size();

    for (unsigned int i = 0 ; i < attributes->size(); i++) {
        rep.handleArray[i] = attributes->getHandle(i);
    }
    
    rep.federate = req->federate ;
    rep.object = req->object ;

    rep.send(link); // Send answer to RTIA
}

// ----------------------------------------------------------------------------
// processCancelAcquisition
void
RTIG::processCancelAcquisition(Socket *link, NetworkMessage *req)
{
    auditServer << "Object = " << req->object
		<< ", # of att. = " << req->handleArraySize ;

    federations.cancelAcquisition(req->federation,
                                   req->federate,
                                   req->object,
                                   req->handleArray,
                                   req->handleArraySize);

    D.Out(pdDebug,
          "Federate %u of Federation %u release response of object %u.",
          req->federate, req->federation, req->object);

    NM_Cancel_Attribute_Ownership_Acquisition rep ;    
    rep.federate = req->federate ;
    rep.object = req->object ;
    rep.handleArraySize = 0 ;

    rep.send(link); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processCreateRegion
void
RTIG::processCreateRegion(Socket *link, NetworkMessage *req)
{
    // TODO: audit...

    NM_DDM_Create_Region rep ;

    rep.region = federations.createRegion(req->federation,
                                           req->federate,
                                           req->space,
                                           req->nbExtents);

    D[pdDebug] << "Federate " << req->federate << " of Federation "
               << req->federation << " creates region " << rep.region
               << endl ;

    rep.federate = req->federate ;
    rep.send(link);
}

// ----------------------------------------------------------------------------
// processModifyRegion
void
RTIG::processModifyRegion(Socket *link, NetworkMessage *req)
{
    federations.modifyRegion(req->federation, req->federate,
			      req->region, req->getExtents());
    
    D[pdDebug] << "Federate " << req->federate << " of Federation "
               << req->federation << " modifies region " << req->region
               << endl ;

    NM_DDM_Modify_Region rep ;
    rep.federate = req->federate ;
    rep.send(link);
}

// ----------------------------------------------------------------------------
// processDeleteRegion
void
RTIG::processDeleteRegion(Socket *link, NetworkMessage *req)
{
    // TODO: audit...

    federations.deleteRegion(req->federation, req->federate, req->region);

    D[pdDebug] << "Federate " << req->federate << " of Federation "
               << req->federation << " deletes region " << req->region
               << endl ;

    NM_DDM_Delete_Region rep ;    
    rep.federate = req->federate ;
    rep.region = req->region ;
    rep.send(link);
}

// ----------------------------------------------------------------------------
// processAssociateRegion
void
RTIG::processAssociateRegion(Socket *link, NetworkMessage *req)
{
    // TODO: audit...

    D[pdDebug] << "Federate " << req->federate << " of Federation "
               << req->federation << " associates region " << req->region
               << " to some attributes of object " << req->object << endl ;

    federations.associateRegion(req->federation, req->federate, req->object,
				 req->region, req->handleArraySize,
				 req->handleArray);

    NM_DDM_Associate_Region rep ;    
    rep.federate = req->federate ;
    rep.send(link);
}

// ----------------------------------------------------------------------------
// processUnassociateRegion
void
RTIG::processUnassociateRegion(Socket *link, NetworkMessage *req)
{
    // TODO: audit...

    federations.unassociateRegion(req->federation, req->federate,
				   req->object, req->region);

    D[pdDebug] << "Federate " << req->federate << " of Federation "
               << req->federation << " associates region " << req->region
               << " from object " << req->object << endl ;

    NM_DDM_Unassociate_Region rep ;
    rep.federate = req->federate ;
    rep.send(link);
}

// ----------------------------------------------------------------------------
// processSubscribeAttributes
void
RTIG::processSubscribeAttributesWR(Socket *link, NetworkMessage *req)
{
    // TODO: audit...
    D[pdDebug] << "Federate " << req->federate << " of Federation "
               << req->federation << " subscribes with region " << req->region
               << " to some attributes of class " << req->objectClass << endl ;

    federations.subscribeAttributesWR(req->federation, req->federate,
				       req->objectClass, req->region,
				       req->handleArraySize, req->handleArray);

    NM_DDM_Subscribe_Attributes rep ;    
    rep.federate = req->federate ;
    rep.objectClass = req->objectClass ;
    rep.handleArraySize = 0 ;
    rep.send(link);
}

// ----------------------------------------------------------------------------
// processUnsubscribeAttributes
void
RTIG::processUnsubscribeAttributesWR(Socket *link, NetworkMessage *req)
{
    // TODO: audit...
    D[pdDebug] << "Federate " << req->federate << " of Federation "
               << req->federation << " unsubscribes with region " << req->region
               << " from object class " << req->objectClass << endl ;

    federations.unsubscribeAttributesWR(req->federation, req->federate,
					 req->objectClass, req->region);

    NM_DDM_Unsubscribe_Attributes rep ;
    rep.federate = req->federate ;
    rep.send(link);
}

// ----------------------------------------------------------------------------
// processSubscribeInteractions
void
RTIG::processSubscribeInteractionWR(Socket *link, NetworkMessage *req)
{
    // TODO: audit...

    federations.subscribeInteractionWR(req->federation, req->federate,
					req->interactionClass, req->region);

    D[pdDebug] << "Federate " << req->federate << " of Federation "
               << req->federation << " subscribes with region " << req->region
               << " to interaction class " << req->interactionClass << endl ;

    NM_DDM_Subscribe_Interaction rep ;    
    rep.federate = req->federate ;
    rep.send(link);
}

// ----------------------------------------------------------------------------
// processUnsubscribeInteractions
void
RTIG::processUnsubscribeInteractionWR(Socket *link, NetworkMessage *req)
{
    // TODO: audit...

    federations.unsubscribeInteractionWR(req->federation, req->federate,
				   req->interactionClass, req->region);

    D[pdDebug] << "Federate " << req->federate << " of Federation "
               << req->federation << " unsubscribes with region " << req->region
               << " from interaction class " << req->interactionClass << endl ;

    NM_DDM_Unsubscribe_Interaction rep ;
    rep.federate = req->federate ;
    rep.send(link);
}

// ----------------------------------------------------------------------------
// processRegisterObjectWithRegion
void
RTIG::processRegisterObjectWithRegion(Socket *link, NetworkMessage *req)
{
	std::auto_ptr<NetworkMessage> rep(NM_Factory::create(req->getType()));
    // FIXME bug #9869
    // When we were passed a set of region
    // we should register object for each region  
    // the trouble comes from the fact that
    // there is both
    //     - req->region  (coming from NetworkMessage::region)
    //     - req->regions (coming from BasicMessage::regions)
    // would be nice to sort those thing out.
    rep->object = federations.registerObjectWithRegion(req->federation,
						      req->federate,
						      req->objectClass,
						      const_cast<char*>(req->getLabel().c_str()),
						      req->region,
						      req->handleArraySize,
						      req->handleArray);
	
    D.Out(pdRegister,
          "Object \"%s\" of Federate %u has been registered under ID %u.",
          req->getLabel().c_str(), req->federate, rep->object);

    rep->federate = req->federate ;
    rep->send(link); // Send answer to RTIA
}

// ----------------------------------------------------------------------------
// processRequestObjectAttributeValueUpdate
void
RTIG::processRequestObjectAttributeValueUpdate(Socket *link, NetworkMessage *request)
{
    NM_Request_Object_Attribute_Value_Update answer ;
    Handle federateOwner ;  // federate owner of the object
    G.Out(pdGendoc,"enter RTIG::processRequestObjectAttributeValueUpdate");
    G.Out(pdGendoc,"BEGIN ** REQUEST OBJECT ATTRIBUTE VALUE UPDATE **");

    auditServer << "ObjID = " << request->object ;

    // We have to do verifications about this object and we need owner
    answer.exception = e_NO_EXCEPTION ;
    try 
      {
      federateOwner = federations.requestObjectOwner(request->federation,
                                 request->federate,
                                 request->object,
                                 request->handleArray,
                                 request->handleArraySize);
      }
     catch (ObjectNotKnown e)
        {
        answer.exception = e_ObjectNotKnown ;
        answer.exceptionReason = e._reason ;
        }
     catch (FederationExecutionDoesNotExist e)
        {
        answer.exception = e_FederationExecutionDoesNotExist ;
        answer.exceptionReason = e._reason ;
        }
     catch (RTIinternalError e)
        {
        answer.exception = e_RTIinternalError ;
        answer.exceptionReason = e._reason ;
        }

    answer.federate = request->federate ;
    answer.object = request->object ;

    answer.send(link); // Send answer to RTIA
    G.Out(pdGendoc,"exit  RTIG::processRequestObjectAttributeValueUpdate");
    G.Out(pdGendoc,"END   ** REQUEST OBJECT ATTRIBUTE VALUE UPDATE **");
}

}} // namespace certi/rtig

// $Id: RTIG_processing.cc,v 3.56.2.5 2008/04/10 19:55:47 erk Exp $
