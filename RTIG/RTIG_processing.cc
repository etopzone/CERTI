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
// $Id: RTIG_processing.cc,v 3.81 2009/04/02 19:58:10 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIG.hh"
#include "NM_Classes.hh"

#include <iostream>
#include <cassert>
#include <memory>

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

    auditServer << "Federation Name : " << federation.c_str() ;
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
        auditServer <<" created.";

        }

    G.Out(pdGendoc,"processCreateFederation===>write answer to RTIA");

    rep.send(link,NM_msgBufSend); // Send answer to RTIA

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
    std::string federate   = req->federateName ;
    std::string filename ;

    unsigned int peer     = req->bestEffortPeer ;
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

    try
       {
        num_federe = federations.addFederate(num_federation,
                                          federate.c_str(),
                                          static_cast<SocketTCP*>(link));
        }
    catch (FederateAlreadyExecutionMember &e)
        {
        // Federate yet has joined this federation(same or another with same name)
        // RTIG has to return something to RTIA
        // RTIA waits a GET_FED_FILE message
        // RTIG says not OK to RTIA in a GET_FED_FILE message
        NM_Get_FED_File repFED ;
       repFED.exception = e_FederateAlreadyExecutionMember ;
       repFED.exceptionReason="Federate with same name has yet joined the federation";
       G.Out(pdGendoc,"processJoinFederation==>Answer to RTIA GFF ERROR %s",repFED.exceptionReason.c_str());
       repFED.send(link,NM_msgBufSend);

       G.Out(pdGendoc,"exit RTIG::processJoinFederation on Error");
       G.Out(pdGendoc,"END ** JOIN FEDERATION (BAD) SERVICE **");
       // FIXME strange to send 2 messages?
       // Prepare answer about JoinFederationExecution
       NM_Join_Federation_Execution rep ;
       rep.exception = e_FederateAlreadyExecutionMember ;
       rep.exceptionReason = "Federate with same name has yet joined the federation";

       G.Out(pdGendoc,"processJoinFederation==>Answer to RTIA JFE ERROR %s",rep.exceptionReason.c_str());

       rep.send(link,NM_msgBufSend);
       return ;
       }

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
    std::ifstream fedFile(filename.c_str());
    if ( !fedFile.is_open()) {
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

    repFED.send(link,NM_msgBufSend);

    if ( e ==  e_NO_EXCEPTION )
        {
        // Wait for OK from RTIA
    	NM_Get_FED_File msg ;
        D.Out(pdTrace,"wait NetworkMessage of Type %d",msg.getType());
        msg.receive(link,NM_msgBufReceive);
        assert ( msg.number == 0 );
        // RTIA has opened working file then RTIG has to transfer file contents
        // line by line
        std::string fileLine;
        int num_line = 0 ;
        while (!fedFile.eof())
            {
            num_line++;
            // Read a line
            std::getline(fedFile,fileLine);
            fileLine = fileLine+"\n";
            // RTIG sends line to RTIA and number gives line number
            repFED.exception = e_NO_EXCEPTION ;
            repFED.federate = num_federe ;
            repFED.federation = num_federation ;
            repFED.number = num_line ;
            repFED.FEDid = filename;
            // line transfered
            repFED.setFEDLine(fileLine);
            // Send answer
            repFED.send(link,NM_msgBufSend);

            // Wait for OK from RTIA
            msg.receive(link,NM_msgBufReceive);
            assert ( msg.number == num_line );
            }

	    // close
	    fedFile.close();
        repFED.exception = e_NO_EXCEPTION ;
        repFED.federate = num_federe ;
        repFED.federation = num_federation ;
        repFED.number = 0 ;
        repFED.FEDid = filename;
        // Send answer

        G.Out(pdGendoc,"processJoinFederation====>End  FED file transfer");

        repFED.send(link,NM_msgBufSend);
        }
    // END of FED file processing

    // Now we have to answer about JoinFederationExecution

#ifdef FEDERATION_USES_MULTICAST
    rep.AdresseMulticast = com_mc->returnAdress();
#endif

    D.Out(pdInit, "Federate \"%s\" has joined Federation %u under handle %u.",
          federate.c_str(), num_federation, num_federe);

    // Send answer

    rep.send(link,NM_msgBufSend);

    G.Out(pdGendoc,"exit RTIG::processJoinFederation");
    G.Out(pdGendoc,"END ** JOIN FEDERATION SERVICE **");

}

// ----------------------------------------------------------------------------
//! Removes a federate from federation.
void
RTIG::processResignFederation(Socket *link,Handle federation,
                              FederateHandle federe)
{
    NM_Resign_Federation_Execution reponse ;

    G.Out(pdGendoc,"BEGIN ** RESIGN FEDERATION SERVICE **");
    G.Out(pdGendoc,"enter RTIG::processResignFederation");

    federations.remove(federation, federe);
    D.Out(pdInit, "Federate %u is resigning from federation %u.",
          federe, federation);

    auditServer << "Federate " << federe << " resign federation("<<federation<<")" ;

    // Send answer to RTIA
    reponse.federate = federe ;
    reponse.federation = federation ;
    reponse.send(link,NM_msgBufSend);

    G.Out(pdGendoc,"exit RTIG::processResignFederation");
    G.Out(pdGendoc,"END ** RESIGN FEDERATION SERVICE **");

}

// ----------------------------------------------------------------------------
//! Removes a federation.
void
RTIG::processDestroyFederation(Socket *link, NetworkMessage *req)
{
    NM_Destroy_Federation_Execution rep ;
    Handle num_federation ;

    std::string federation = req->federationName ;

    G.Out(pdGendoc,"enter RTIG::processDestroyFederation");
    G.Out(pdGendoc,"BEGIN ** DESTROY FEDERATION SERVICE **");

    if (federation.length() == 0) throw RTIinternalError("Invalid Federation Name.");

    federations.exists(federation.c_str(), num_federation);
    // May throw RTIinternalError
    //           FederatesCurrentlyJoined
    //           FederationExecutionDoesNotExist
    try {
      federations.destroyFederation(num_federation);
      // Here delete federation (num_federation) has been done
      federationHandles.free(num_federation);
      D.Out(pdInit, "Federation \"%s\" has been destroyed.", federation.c_str());
    }
    catch (Exception &e)
      { printf("ERROR : %s  reason : %s\n",e._name,e._reason.c_str());
        if (strcmp(e._name,"RTIinternalError")==0 )
          {
          rep.exception = e_RTIinternalError;
          rep.exceptionReason = e._reason;
          }
        else if (strcmp(e._name,"FederationExecutionDoesNotExist")==0 )
          {
          rep.exception = e_FederationExecutionDoesNotExist;
          rep.exceptionReason =e._reason;
          }
        else if (strcmp(e._name,"FederatesCurrentlyJoined")==0 )
          {
          rep.exception = e_FederatesCurrentlyJoined;
          rep.exceptionReason = "at least one federate joined";
          }
      }

    rep.federate = req->federate ;
    rep.federationName = req->federationName;
    if ( rep.exception == e_NO_EXCEPTION )
       {
       auditServer << "Federation Name \"" << federation.c_str() << "\"("<<num_federation<<") destroyed." ;
       }
    G.Out(pdGendoc,"processDestroyFederation===>write DFE to RTIA");

    rep.send(link,NM_msgBufSend);

    G.Out(pdGendoc,"END ** DESTROY FEDERATION SERVICE **");
    G.Out(pdGendoc,"exit RTIG::processDestroyFederation");
}

// ----------------------------------------------------------------------------
//! Set federate's class relevance advisroy switch
void
RTIG::processSetClassRelevanceAdvisorySwitch(Socket *link,
			     NM_Set_Class_Relevance_Advisory_Switch *msg)
{
  NM_Set_Class_Relevance_Advisory_Switch rep;

  G.Out(pdGendoc,"enter RTIG::processSetClassRelevanceAdvisorySwitch");
  G.Out(pdGendoc,"BEGIN ** SET CLASS RELEVANCE ADVISORY SWITCH **");

  if (msg->getClassRelevanceAdvisorySwitch()) {
        auditServer << "ON";
	federations.setClassRelevanceAdvisorySwitch(msg->federation, msg->federate);
        D.Out(pdTerm, "Federate %u of Federation %u sets AttributeRelevanceAdvisorySwitch.",
              msg->federate, msg->federation);
  }
  else {
        auditServer << "OFF";
	federations.unsetClassRelevanceAdvisorySwitch(msg->federation, msg->federate);
        D.Out(pdTerm, "Federate %u of Federation %u clears AttributeRelevanceAdvisorySwitch.",
              msg->federate, msg->federation);
  }

  rep.send(link,NM_msgBufSend);

  G.Out(pdGendoc,"END ** SET CLASS RELEVANCE ADVISORY SWITCH **");
  G.Out(pdGendoc,"exit RTIG::processSetClassRelevanceAdvisorySwitch");
}

// ----------------------------------------------------------------------------
//! Set federate's interaction relevance advisroy switch
void
RTIG::processSetInteractionRelevanceAdvisorySwitch(Socket *link,
			     NM_Set_Interaction_Relevance_Advisory_Switch *msg)
{
  NM_Set_Interaction_Relevance_Advisory_Switch rep;

  G.Out(pdGendoc,"enter RTIG::processSetInteractionRelevanceAdvisorySwitch");
  G.Out(pdGendoc,"BEGIN ** SET INTERACTION RELEVANCE ADVISORY SWITCH **");

  if (msg->getInteractionRelevanceAdvisorySwitch()) {
        auditServer << "ON";
	federations.setInteractionRelevanceAdvisorySwitch(msg->federation, msg->federate);
        D.Out(pdTerm, "Federate %u of Federation %u sets InteractionRelevanceAdvisorySwitch.",
              msg->federate, msg->federation);
  }
  else {
        auditServer << "OFF";
	federations.unsetInteractionRelevanceAdvisorySwitch(msg->federation, msg->federate);
        D.Out(pdTerm, "Federate %u of Federation %u clears InteractionRelevanceAdvisorySwitch.",
              msg->federate, msg->federation);
  }

  rep.send(link,NM_msgBufSend);

  G.Out(pdGendoc,"END ** SET INTERACTION RELEVANCE ADVISORY SWITCH **");
  G.Out(pdGendoc,"exit RTIG::processSetInteractionRelevanceAdvisorySwitch");
}

// ----------------------------------------------------------------------------
//! Set federate's attribute relevance advisroy switch
void
RTIG::processSetAttributeRelevanceAdvisorySwitch(Socket *link,
			     NM_Set_Attribute_Relevance_Advisory_Switch *msg)
{
  NM_Set_Attribute_Relevance_Advisory_Switch rep;

  G.Out(pdGendoc,"enter RTIG::processSetAttributeRelevanceAdvisorySwitch");
  G.Out(pdGendoc,"BEGIN ** SET ATTRIBUTE RELEVANCE ADVISORY SWITCH **");

  if (msg->getAttributeRelevanceAdvisorySwitch()) {
        auditServer << "ON";
	federations.setAttributeRelevanceAdvisorySwitch(msg->federation, msg->federate);
        D.Out(pdTerm, "Federate %u of Federation %u sets AttributeRelevanceAdvisorySwitch.",
              msg->federate, msg->federation);
  }
  else {
        auditServer << "OFF";
	federations.unsetAttributeRelevanceAdvisorySwitch(msg->federation, msg->federate);
        D.Out(pdTerm, "Federate %u of Federation %u clears AttributeRelevanceAdvisorySwitch.",
              msg->federate, msg->federation);
  }

  rep.send(link,NM_msgBufSend);

  G.Out(pdGendoc,"END ** SET ATTRIBUTE RELEVANCE ADVISORY SWITCH **");
  G.Out(pdGendoc,"exit RTIG::processSetAttributeRelevanceAdvisorySwitch");
}

// ----------------------------------------------------------------------------
//! Set federate's attribute scope advisroy switch
void
RTIG::processSetAttributeScopeAdvisorySwitch(Socket *link,
			     NM_Set_Attribute_Scope_Advisory_Switch *msg)
{
  NM_Set_Attribute_Scope_Advisory_Switch rep;

  G.Out(pdGendoc,"enter RTIG::processSetAttributeScopeAdvisorySwitch");
  G.Out(pdGendoc,"BEGIN ** SET ATTRIBUTE SCOPE ADVISORY SWITCH **");

  if (msg->getAttributeScopeAdvisorySwitch()) {
        auditServer << "ON";
	federations.setAttributeScopeAdvisorySwitch(msg->federation, msg->federate);
        D.Out(pdTerm, "Federate %u of Federation %u sets AttributeScopeAdvisorySwitch.",
              msg->federate, msg->federation);
  }
  else {
        auditServer << "OFF";
	federations.unsetAttributeScopeAdvisorySwitch(msg->federation, msg->federate);
        D.Out(pdTerm, "Federate %u of Federation %u clears AttributeScopeAdvisorySwitch.",
              msg->federate, msg->federation);
  }

  rep.send(link,NM_msgBufSend);

  G.Out(pdGendoc,"END ** SET ATTRIBUTE SCOPE ADVISORY SWITCH **");
  G.Out(pdGendoc,"exit RTIG::processSetAttributeScopeAdvisorySwitch");
}

// ----------------------------------------------------------------------------
//! Put federate as time regulating.
void
RTIG::processSetTimeRegulating(Socket *link, NM_Set_Time_Regulating *msg)
{
  G.Out(pdGendoc,"enter RTIG::processSetTimeRegulating");
  G.Out(pdGendoc,"BEGIN ** SET TIME REGULATING **");

  if (msg->isRegulator()) {
        auditServer << "ON at time " << msg->getDate().getTime();

        federations.createRegulator(msg->federation,
                                     msg->federate,
                                     msg->getDate());

        // send timeRegulationEnabled() to federate.
        NM_Time_Regulation_Enabled rep ;
        rep.federate = msg->federate ;
        rep.federation = msg->federation ;
	rep.setDate(msg->getDate());

        G.Out(pdGendoc,"      processSetTimeRegulating====> write TRE to RTIA");

        rep.send(link,NM_msgBufSend);

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

    G.Out(pdGendoc,"END ** SET TIME REGULATING **");
    G.Out(pdGendoc,"exit RTIG::processSetTimeRegulating");
}

// ----------------------------------------------------------------------------
//! Put federate as time constrained
void
RTIG::processSetTimeConstrained(Socket *link, NM_Set_Time_Constrained *msg)
{
  G.Out(pdGendoc,"enter RTIG::processSetTimeConstrained");
  G.Out(pdGendoc,"BEGIN ** SET TIME CONSTRAINED **");

  if (msg->isConstrained()) {
        auditServer << "ON at time " << msg->getDate().getTime();

        federations.addConstrained(msg->federation,
                                    msg->federate);

        // send timeConstrainedEnabled() to federate.
        NM_Time_Constrained_Enabled rep ;
        rep.federate = msg->federate ;
        rep.federation = msg->federation ;
	rep.setDate(msg->getDate());

        G.Out(pdGendoc,"      processSetTimeConstrained====> write TCE to RTIA");

        rep.send(link,NM_msgBufSend);

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

    G.Out(pdGendoc,"END ** SET TIME CONSTRAINED **");
    G.Out(pdGendoc,"exit RTIG::processSetTimeConstrained");
}

// ----------------------------------------------------------------------------
//! processMessageNull.
void
RTIG::processMessageNull(NetworkMessage *msg)
{
    auditServer << "Date " << msg->getDate().getTime();

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
RTIG::processRegisterSynchronization(Socket *link, NM_Register_Federation_Synchronization_Point *req)
{

    G.Out(pdGendoc,"BEGIN ** REGISTER FEDERATION SYNCHRONIZATION POINT Service **");
    G.Out(pdGendoc,"enter RTIG::processRegisterSynchronization");

    auditServer << "Label \"" << req->getLabel().c_str() << "\" registered. Tag is \""
		<< req->getTag().c_str() << "\"" ;

    // boolean true means a federates set exists
    if ( req->doesSetExist() )
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

    rep.send(link,NM_msgBufSend);

    // boolean true means a federates set exists
    if ( req->doesSetExist() )
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

    if ( req->isDated() )
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

    auditServer << "Publish Object Class = " << req->objectClass << ", # of att. = "
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
    rep->federate    = req->federate ;
    rep->objectClass = req->objectClass ;

    rep->send(link,NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processSubscribeObjectClass
void
RTIG::processSubscribeObjectClass(Socket *link, NetworkMessage *req)
{
    G.Out(pdGendoc,"enter RTIG::processSubscribeObjectClass");
    G.Out(pdGendoc,"BEGIN **  SUBSCRIBE OBJECT CLASS SERVICE **");

    std::vector <AttributeHandle> arrayVide ;
    arrayVide.empty() ;
    bool sub = (req->getType() == NetworkMessage::SUBSCRIBE_OBJECT_CLASS);

    auditServer << "Subscribe Object Class = " << req->objectClass
		<< ", # of att. = " << req->handleArraySize ;

    federations.subscribeObject(req->federation,
				req->federate,
				req->objectClass,
				sub ? req->handleArray : arrayVide,
				sub ? req->handleArraySize : 0);

    /* here we want a callback -> inform publisher(s) of objectclass
     * with startRegistrationForObjectClass */

    D.Out(pdRegister,
          "Federate %u of Federation %u subscribed to object class %d.",
          req->federate, req->federation, req->objectClass);

    std::auto_ptr<NetworkMessage> rep(NM_Factory::create(req->getType()));
    rep->federate = req->federate ;
    rep->objectClass = req->objectClass ;
    rep->handleArraySize = 0 ;

    rep->send(link,NM_msgBufSend); // send answer to RTIA

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

    auditServer << "Publish Interaction Class = " << req->interactionClass ;
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

    rep->send(link,NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processSubscribeInteractionClass
void
RTIG::processSubscribeInteractionClass(Socket *link, NetworkMessage *req)
{
    bool sub = (req->getType() == NetworkMessage::SUBSCRIBE_INTERACTION_CLASS);

    auditServer << "Subscribe Interaction Class = " << req->interactionClass ;
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

    rep->send(link,NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processRegisterObject
void
RTIG::processRegisterObject(Socket *link, NetworkMessage *req)
{
  std::auto_ptr<NetworkMessage> rep(NM_Factory::create(req->getType()));;

    auditServer << "Register Object Class = " << req->objectClass ;
    rep->object = federations.registerObject(req->federation,
                                             req->federate,
                                             req->objectClass,
                                             const_cast<char*>(req->getLabel().c_str()));
    auditServer << ", Handle = " << rep->object ;

    D.Out(pdRegister,
          "Object \"%s\" of Federate %u has been registered under ID %u.",
          req->getLabel().c_str(), req->federate, rep->object);

    rep->federate    = req->federate ;
    rep->federation  = req->federation;
    rep->objectClass = req->objectClass;
    // rep.object is set by the call of registerObject
    rep->setLabel(req->getLabel());

    rep->send(link,NM_msgBufSend); // Send answer to RTIA
}

// ----------------------------------------------------------------------------
// processUpdateAttributeValues
void
RTIG::processUpdateAttributeValues(Socket *link, NetworkMessage *req)
{
    G.Out(pdGendoc,"enter RTIG::processUpdateAttributeValues");
    std::vector <AttributeValue_t> ValueArray ;

    auditServer << "ObjID = " << req->object
		<< ", Date = " << req->getDate().getTime();

    // Get Value Array
    ValueArray = req->getAttribValueArray();

    // Forward the call
    if ( req->isDated() )
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
    ValueArray.empty();

    // Building answer (Network Message re)
    NM_Update_Attribute_Values rep ;
    rep.federate = req->federate ;
    rep.object   = req->object ;
    rep.setDate(req->getDate());
    rep.handleArraySize = 0 ;
    // Don't forget label and tag
    rep.setLabel(req->getLabel());
    rep.setTag(req->getTag());

    rep.send(link,NM_msgBufSend); // send answer to RTIA
    G.Out(pdGendoc,"exit  RTIG::processUpdateAttributeValues");
}

// ----------------------------------------------------------------------------
// processSendInteraction
void
RTIG::processSendInteraction(Socket *link, NetworkMessage *req)
{
    std::vector <ParameterValue_t> values ;

    G.Out(pdGendoc,"BEGIN ** SEND INTERACTION SERVICE **");
    G.Out(pdGendoc,"enter RTIG::processSendInteraction");

    // Building Value Array
    auditServer << "IntID = " << req->interactionClass
		<< ", date = " << req->getDate().getTime();
    values = req->getParamValueArray();

    if ( req->isDated() )
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
    values.empty();

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
    rep.send(link,NM_msgBufSend); // send answer to RTIA

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
    auditServer << "Delete ObjID = " << req->object ;

    if ( req->isDated() ) {
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
    rep.object   = req->object ;

    rep.send(link,NM_msgBufSend); // send answer to RTIA

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

    rep.send(link,NM_msgBufSend); // send answer to RTIA
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

    rep.send(link,NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processNegotiatedOwnershipDivestiture
void
RTIG::processNegotiatedOwnershipDivestiture(Socket *link, NetworkMessage *req)
{
    auditServer << "Object = " <<  req->object
		<< ", # of att. = " << req->handleArraySize ;
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

    rep.send(link,NM_msgBufSend); // send answer to RTIA
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

    rep.send(link,NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processUnconditionalDivestiture
void
RTIG::processUnconditionalDivestiture(Socket *link, NetworkMessage *req)
{
    auditServer << "Object = " << req->object
		<< ", # of att. = " << req->handleArraySize ;

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

    rep.send(link,NM_msgBufSend); // send answer to RTIA
}

// ----------------------------------------------------------------------------
// processOwnershipAcquisition
void
RTIG::processOwnershipAcquisition(Socket *link, NetworkMessage *req)
{
    auditServer << "Object = " << req->object
		<< ", # of att. = " << req->handleArraySize ;

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

    rep.send(link,NM_msgBufSend); // send answer to RTIA
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

    rep.send(link,NM_msgBufSend); // send answer to RTIA
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
    rep.handleArray.resize(rep.handleArraySize) ;

    for (unsigned int i = 0 ; i < attributes->size(); i++) {
        rep.handleArray[i] = attributes->getHandle(i);
    }

    rep.federate = req->federate ;
    rep.object = req->object ;

    rep.send(link,NM_msgBufSend); // Send answer to RTIA
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

    rep.send(link,NM_msgBufSend); // send answer to RTIA
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
    rep.send(link,NM_msgBufSend);
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
    rep.send(link,NM_msgBufSend);
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
    rep.send(link,NM_msgBufSend);
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
    rep.send(link,NM_msgBufSend);
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
    rep.send(link,NM_msgBufSend);
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
    rep.send(link,NM_msgBufSend);
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
    rep.send(link,NM_msgBufSend);
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
    rep.send(link,NM_msgBufSend);
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
    rep.send(link,NM_msgBufSend);
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
    rep->send(link,NM_msgBufSend); // Send answer to RTIA
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

    answer.send(link,NM_msgBufSend); // Send answer to RTIA
    G.Out(pdGendoc,"exit  RTIG::processRequestObjectAttributeValueUpdate");
    G.Out(pdGendoc,"END   ** REQUEST OBJECT ATTRIBUTE VALUE UPDATE **");
}

}} // namespace certi/rtig

// $Id: RTIG_processing.cc,v 3.81 2009/04/02 19:58:10 erk Exp $
