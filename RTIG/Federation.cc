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
// $Id: Federation.cc,v 3.100 2008/11/09 12:41:39 gotthardp Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Federation.hh"
#include "NM_Classes.hh"
#include <sstream>
#include <cassert>
#include <memory>

#ifdef _WIN32

#endif

#include "fed.hh"
#include "XmlParser.hh"
#include "Object.hh"
#include "ObjectAttribute.hh"
#include "ObjectSet.hh"
#include "ObjectClassSet.hh"
#include "InteractionSet.hh"

#include "ObjectClassAttribute.hh"
#include "PrettyDebug.hh"
#include "LBTS.hh"
#include "NM_Classes.hh"

#ifdef _WIN32
#include <windows.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#else
#include <map>
#include <fstream>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#endif

using std::pair ;
using std::ifstream ;
using std::ios ;
using std::cout ;
using std::endl ;
using std::string ;
using std::list ;
using std::cerr ;
using std::vector ;

// Definitions
#ifdef HAVE_XML
#define ROOT_NODE (const xmlChar*) "rtigSaveData"
#define NODE_FEDERATION (const xmlChar*) "federation"
#define NODE_FEDERATE (const xmlChar*) "federate"
#endif // HAVE_XML

namespace certi {

class XmlParser ;

namespace rtig {

static PrettyDebug D("FEDERATION", __FILE__);
static PrettyDebug G("GENDOC",__FILE__);

// ----------------------------------------------------------------------------
//! Constructor
/*! Allocates memory the Name's storage, and read its FED file to store the
  result in RootObj.
*/
#ifdef FEDERATION_USES_MULTICAST
/** with FEDERATION_USES_MULTICAST defined
    @param federation_name
    @param federation_handle
    @param socket_server
    @param audit_server
    @param mc_link
*/
Federation::Federation(const char *federation_name,
                       FederationHandle federation_handle,
                       SocketServer &socket_server,
                       AuditFile &audit_server,
                       SocketMC *mc_link,
                       int theVerboseLevel)
#else
/** with FEDERATION_USES_MULTICAST not defined
    @param federation_name
    @param federation_handle
    @param socket_server
    @param audit_server
    @param FEDid_name i.e. FED file name (may be a .fed or a .xml file)
*/
    Federation::Federation(const char *federation_name,
                           Handle federation_handle,
                           SocketServer &socket_server,
                           AuditFile &audit_server,
                           const char *FEDid_name,
                           int theVerboseLevel)
#endif
    throw (CouldNotOpenFED, ErrorReadingFED, MemoryExhausted, SecurityError,
           RTIinternalError)
    : federateHandles(1), objectHandles(1), saveInProgress(false),
      restoreInProgress(false), saveStatus(true), restoreStatus(true),
      verboseLevel(theVerboseLevel)

{
    //    fedparser::FedParser *fed_reader ;
  STAT_STRUCT file_stat;

#ifdef FEDERATION_USES_MULTICAST // -----------------
    // Initialize Multicast
    if (mc_link == NULL) {
        D.Out(pdExcept, "Null Multicast socket for new Federation.");
        throw RTIinternalError("NULL Multicast socket for new Federation.");
    }

    D.Out(pdInit, "New Federation %d will use Multicast.", federation_handle);
    MCLink = mc_link ;
#endif // FEDERATION_USES_MULTICAST // --------------

    G.Out(pdGendoc,"enter Federation::Federation");
    // Allocates Name
    if ((federation_name == NULL) || (federation_handle == 0))
        throw RTIinternalError("Null init parameter in Federation creation.");

    name = std::string(federation_name);

    // Default Attribute values
    handle = federation_handle;
    FEDid  = std::string(FEDid_name);

    D.Out(pdInit, "New Federation created with Handle %d, now reading FOM.",
          handle);

    // Initialize the Security Server.
    server = new SecurityServer(socket_server, audit_server, handle);

    // Read FOM File to initialize Root Object.
    root = new RootObject(server);

	if (verboseLevel>0) {
		cout << "New federation: " << name << endl ;
	}

    // We should try to open FOM file from different
    // predefined places:
    //
    // 1 - bare filename considered as a path provided through FEDid_name
    //
    // 2 - getenv(CERTI_HOME)+"/share/federations"+ FEDid_name
    //
    // 3 - default (unix) installation place plus FEDid_name
    //     "/usr/local/share/federation/" + FEDid_name
    //
    string filename   = FEDid;
    bool   filefound  = false;
    if (verboseLevel>0) {
    	cout << "Looking for FOM file... " << endl ;

    	cout << "   Trying... " << filename;
    }
    filefound = (0==STAT_FUNCTION(filename.c_str(),&file_stat));

#ifdef _WIN32
    if (!filefound) {
      char temp[260];
      if (verboseLevel>0) {
    	  cout << " --> cannot access." <<endl;
      }
      GetCurrentDirectory(260,temp);
      filename = string(temp);
      filename = filename + "\\share\\federations\\"+string(FEDid_name);
      if (verboseLevel>0) {
    	  cout << "   Now trying..." << filename;
      }
      filefound = (0==STAT_FUNCTION(filename.c_str(),&file_stat));
    }

    if (!filefound && (NULL!=getenv("CERTI_HOME"))) {
      if (verboseLevel>0) {
    		cout << " --> cannot access." <<endl;
      }
      filename = string(getenv("CERTI_HOME"))+"\\share\\federations\\"+FEDid_name;
      if (verboseLevel>0) {
        cout << "   Now trying..." << filename;
      }
      filefound = (0==STAT_FUNCTION(filename.c_str(),&file_stat));
    }
#else
    if (!filefound) {
      if (verboseLevel>0) {
         cout << " --> cannot access." <<endl;
      }
      filename = "/usr/local/share/federations/"+string(FEDid_name);
      if (verboseLevel>0) {
         cout << "   Now trying..." << filename;
      }
      filefound = (0==STAT_FUNCTION(filename.c_str(),&file_stat));
    }

    if (!filefound && (NULL!=getenv("CERTI_HOME"))) {
      if (verboseLevel>0) {
         cout << " --> cannot access." <<endl;
      }
      filename = string(getenv("CERTI_HOME"))+"/share/federations/"+FEDid_name;
      if (verboseLevel>0) {
         cout << "   Now trying..." << filename;
      }
      filefound = (0==STAT_FUNCTION(filename.c_str(),&file_stat));
    }
#endif

    if (!filefound) {
      if (verboseLevel>0) {
        cout << " --> cannot access." <<endl;
      }
      cerr << "Next step will fail"<<endl;
      G.Out(pdGendoc,"exit Federation::Federation on exception CouldNotOpenFED");
      throw CouldNotOpenFED("RTIG cannot find FED file.");
    }

    // now really assign FEDid
    FEDid = filename;

    // Try to open to verify if file exists
    std::ifstream fedTry(FEDid.c_str());
    if (!fedTry.is_open())
        {
    	if (verboseLevel>0) {
           cout << "... failed : ";
    	}
        G.Out(pdGendoc,"exit Federation::Federation on exception CouldNotOpenFED");
        throw CouldNotOpenFED("RTIG have found but cannot open FED file");
        }
    else {
    	if (verboseLevel>0) {
    		cout << "... opened." << endl ;
    	}
        fedTry.close();
    }

    int  nbcar_filename = filename.length() ;
    bool is_a_fed       = false ;
    bool is_an_xml      = false ;

    // hope there is a . before fed or xml
    if ( filename[nbcar_filename-4] != '.' )
        {
        G.Out(pdGendoc,"exit Federation::Federation on exception CouldNotOpenFED");
        throw CouldNotOpenFED("Incorrect FED file name, cannot find "
         "extension (character '.' is missing [or not in reverse 4th place])");
        }

    string extension = filename.substr(nbcar_filename-3,3) ;
    D.Out(pdTrace,"filename is: %s (extension is <%s>",filename.c_str(),extension.c_str());
    if ( !strcasecmp(extension.c_str(),"fed") )
        {
        is_a_fed = true ;
        D.Out(pdTrace, "Trying to use .fed file");
        }
    else if  ( !strcasecmp(extension.c_str(),"xml") )
        {
        is_an_xml = true ;
        D.Out(pdTrace, "Trying to use .xml file");
        }
    else {
        G.Out(pdGendoc,"exit Federation::Federation on exception CouldNotOpenFED");
        throw CouldNotOpenFED("Incorrect FED file name : nor .fed nor .xml file");
    }

    std::ifstream fedFile(filename.c_str());

    if (fedFile.is_open())
        {
    	fedFile.close();
        if ( is_a_fed )
            {
        	// parse FED file and show the parse on stdout if verboseLevel>=2
	    int err = fedparser::build(filename.c_str(), root, (verboseLevel>=2));
	    if (err != 0 )
                {
                G.Out(pdGendoc,"exit Federation::Federation on exception ErrorReadingFED");
                throw ErrorReadingFED("fed parser found error in FED file");
	        }

            // Retrieve the FED file last modification time(for Audit)
            STAT_STRUCT StatBuffer ;
            #if defined(_WIN32) && _MSC_VER >= 1400
            char MTimeBuffer[26];
            #else
            char *MTimeBuffer ;
            #endif

            if (STAT_FUNCTION(filename.c_str(), &StatBuffer) == 0) {
            #if defined(_WIN32) && _MSC_VER >= 1400
                ctime_s(&MTimeBuffer[0],26,&StatBuffer.st_mtime);
            #else
                MTimeBuffer = ctime(&StatBuffer.st_mtime);
            #endif
                MTimeBuffer[strlen(MTimeBuffer) - 1] = 0 ; // Remove trailing \n
                server->audit << "(Last modified " << MTimeBuffer << ")" ;
            }
            else
                server->audit << "(could not retrieve last modif time, errno "
		    	  << errno << ")." ;
            }
        else if ( is_an_xml )
            {
            if (XmlParser::exists()) {
                XmlParser *parser = new XmlParser(root);
                server->audit << ", XML File : " << filename.c_str() ;

                try {
                    parser->parse(filename);
                }
                catch (Exception *e) {
                    delete parser ;
                    delete server ;
                    server = NULL ;
                    delete root ;
                    root = NULL ;
                    throw e ;
                }
                delete parser ;
            }
            else {
                cout << "nor fed nor xml" << endl ;
                G.Out(pdGendoc,"exit Federation::Federation on exception CouldNotOpenFED");
		throw CouldNotOpenFED("Incorrect FED file name : nor fed nor xml");
	    }
        }
    }

    G.Out(pdGendoc,"exit Federation::Federation");

}

// ----------------------------------------------------------------------------
// Destructor
Federation::~Federation()
{
    D.Out(pdInit, "Destroying Federation %d...", handle);

    // If there are Federates, delete them all!
//     for (list<Federate *>::const_iterator i = begin(); i != end(); i++) {
//         delete(*i);
//     }
//     clear();

    // Free local allocations
    delete root ;
    delete server ;

#ifdef FEDERATION_USES_MULTICAST
    FermerConnexion(mc_link);
    delete mc_link ;
#endif

}

// ----------------------------------------------------------------------------
//! Returns the number of federates in federation.
int
Federation::getNbFederates() const
{
    return federates.size();
}

// ----------------------------------------------------------------------------
//! Return true if federation is being synchronized.
bool
Federation::isSynchronizing() const
{
    return !synchronizationLabels.empty();
}

// ----------------------------------------------------------------------------
//! Returns the federation handle.
Handle
Federation::getHandle() const
{
    return handle ;
}

// ----------------------------------------------------------------------------
//! Returns the federation name given in 'Create Federation Execution'.
const char *
Federation::getName() const
{
    return name.c_str() ;
}

// ----------------------------------------------------------------------------
//! Returns the number of federates regulator.
int
Federation::getNbRegulators() const
{
    return regulators.size();
}

// ----------------------------------------------------------------------------
//! Returns the FEDid name given in 'Create Federation Execution'.
const char *
Federation::getFEDid() const
{
    return FEDid.c_str() ;
}

// ----------------------------------------------------------------------------
/*! Add the Federate to the Federation, and return its new federate handle.
  Also send
  Null messages from all others federates to initialize its LBTS, and
  finally a RequestPause message if the Federation is already paused.
*/
FederateHandle
Federation::add(const char *federate_name, SocketTCP *tcp_link)
    throw (FederateAlreadyExecutionMember, MemoryExhausted, RTIinternalError)
{
    if (federate_name == 0) {
        D.Out(pdExcept, "Tried to add a NULL named federate.");
        throw RTIinternalError("Tried to add NULL federate to federation.");
    }

    try {
        getFederate(federate_name);
        throw FederateAlreadyExecutionMember("");
    }
    catch (FederateNotExecutionMember &e) {
        // Nothing to do.
    }

    FederateHandle federate_handle = federateHandles.provide();
    federates.push_back(Federate(federate_name, federate_handle));
    D.Out(pdInit, "Federate %d joined Federation %d.", federate_handle, handle);

    // Send, to the newly added federate, a Null message from each regulating
    // federate (i) with their logical time h(i). This permits to calculate
    // its LBTS.
    NM_Message_Null nullMessage;
    NM_Announce_Synchronization_Point ASPMessage;
    try {
	std::vector<LBTS::FederateClock> v ;
	regulators.get(v);

        for (unsigned int i = 0 ; i < v.size(); ++i) {
            nullMessage.federation = handle ;
	        nullMessage.federate = v[i].first ;
	        nullMessage.setDate(v[i].second);
            D.Out(pdTerm,
		  "Sending NULL message(type %d) from %d to new federate.",
                  nullMessage.getType(), nullMessage.federate);

            nullMessage.send(tcp_link,NM_msgBufSend);
        }

        // If federation is synchronizing, put federate in same state.
        if (isSynchronizing()) {
            ASPMessage.federate = federate_handle ;
            ASPMessage.federation = handle ;

            std::map<const char *, const char *>::const_iterator i ;
            i = synchronizationLabels.begin();
            for (; i != synchronizationLabels.end(); i++) {
                ASPMessage.setLabel((*i).first);
                ASPMessage.setTag((*i).second);
                D.Out(pdTerm, "Sending synchronization message %s (type %d)"
                      " to the new Federate.", (*i).first, ASPMessage.getType());

                ASPMessage.send(tcp_link,NM_msgBufSend);
                federates.back().addSynchronizationLabel((*i).first);
            }
        }
    }
    catch (NetworkError) {
        throw RTIinternalError("Network Error while initializing federate.");
    }

    return federate_handle ;
}

// ----------------------------------------------------------------------------
// Set Federate's attribute IsConstrained to true.
/*! FIXME: name/ merge with removeConstrained.
 */
void
Federation::addConstrained(FederateHandle federate_handle)
    throw (FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember
    Federate &federate = getFederate(federate_handle);

    if (federate.isConstrained()) {
        D.Out(pdExcept, "Federate %d already constrained.", federate_handle);
        throw RTIinternalError("Time Regulating already enabled.");
    }

    federate.setConstrained(true);
    D.Out(pdTerm, "Federation %d: Federate %d is now constrained.",
          handle, federate_handle);
}

// ----------------------------------------------------------------------------
//! Add the Federate to the Regulators List.
/*! Check if it's already present, but not if the Time 'theTime' is allowed
  or not.
*/
void
Federation::addRegulator(FederateHandle federate_handle, FederationTime time)
    throw (FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember
    Federate &federate = getFederate(federate_handle);

    // It may throw RTIinternalError if Federate was not regulators.
    regulators.insert(federate_handle, time);
    federate.setRegulator(true);

    D.Out(pdTerm, "Federation %d: Federate %d is now a regulator(Time=%f).",
          handle, federate_handle, time);

    NM_Set_Time_Regulating msg ;
    msg.exception = e_NO_EXCEPTION ;
    msg.federation = handle ;
    msg.federate = federate_handle ;
    msg.regulatorOn();
    msg.setDate(time);

    this->broadcastAnyMessage(&msg, 0);
}

// ----------------------------------------------------------------------------
//! Broadcast 'msg' to all Federate except the specified one
void
Federation::broadcastAnyMessage(NetworkMessage *msg,
                                FederateHandle except_federate)
{
    Socket *socket = NULL ;

    // Broadcast the message 'msg' to all Federates in the Federation
    // except to Federate whose Handle is 'Except_Federate'.
    for (FederateList::const_iterator i = federates.begin(); i != federates.end(); ++i) {
        if (i->getHandle() != except_federate) {
            try {
#ifdef HLA_USES_UDP
                socket = server->getSocketLink(i->getHandle(), BEST_EFFORT);
#else
                socket = server->getSocketLink(i->getHandle());
#endif
                msg->send(socket,NM_msgBufSend);
            }
            catch (RTIinternalError &e) {
                D[pdExcept] << "Reference to a killed Federate while "
                            << "broadcasting." << endl ;
            }
            catch (NetworkError &e) {
                D.Out(pdExcept, "Network error while broadcasting, ignoring.");
            }
        }
    }

    // BUG: If except = 0, could use Multicast.
}

// ----------------------------------------------------------------------------
//! Broadcast 'msg' to some Federates except the specified one
void
Federation::broadcastSomeMessage(NetworkMessage *msg,
                                FederateHandle except_federate,
                                std::vector <FederateHandle> &fede_array,
                                int nbfed)
{
    int ifed ;
    Socket *socket = NULL ;

    if ( fede_array.size() != 0 || nbfed == 0)
        {
        // Broadcast the message 'msg' to some Federates (done in fede_array)
        // in the Federation
        // except to Federate whose Handle is 'Except_Federate'.
        for (FederateList::const_iterator i = federates.begin(); i != federates.end(); ++i)
            {
            if (i->getHandle() != except_federate)
                {
                ifed = 0 ;
                while ( ifed < nbfed )
                    {
                    if ( i->getHandle() == fede_array[ifed] )
                        // Federate i has to be informed because into fede_array
                        {
                        try
                            {
#ifdef HLA_USES_UDP
                            socket = server->getSocketLink(i->getHandle(), BEST_EFFORT);
#else
                            socket = server->getSocketLink(i->getHandle());
#endif
                            msg->send(socket,NM_msgBufSend);
                            }
                        catch (RTIinternalError &e)
                            {
                            D[pdExcept] << "Reference to a killed Federate while "
                                        << "broadcasting." << endl ;
                            }
                        catch (NetworkError &e)
                            {
                            D.Out(pdExcept, "Network error while broadcasting, ignoring.");
                            }
                        }
                    ifed++;
                    }
                }
            }
        }

    // BUG: If except = 0, could use Multicast.
}

// ----------------------------------------------------------------------------
//! broadcastInteraction with time
void
Federation::broadcastInteraction(FederateHandle federate_handle,
                                 InteractionClassHandle interaction,
                                 std::vector <ParameterHandle> &parameter_handles,
                                 std::vector <ParameterValue_t> &parameter_values,
                                 UShort list_size,
                                 FederationTime time,
				 RegionHandle region_handle,
                                 const char *tag)
    throw (FederateNotExecutionMember,
           FederateNotPublishing,
           InteractionClassNotDefined,
           InteractionParameterNotDefined,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{

    G.Out(pdGendoc,"enter Federation::broadcastInteraction with time");

    // It may throw FederateNotExecutionMember.
    this->check(federate_handle);

    const RTIRegion *region = 0 ;
    if (region_handle != 0)
	region = root->getRegion(region_handle);

    root->Interactions->broadcastInteraction(federate_handle,
                                             interaction,
                                             parameter_handles,
                                             parameter_values,
                                             list_size,
                                             time,
					     region,
                                             tag);
    D.Out(pdRequest, "Federation %d: Broadcasted Interaction %d from Federate "
          "%d nb params %d.", handle, interaction, federate_handle, list_size);
    for (int i=0 ; i < list_size ; i++)
        D.Out(pdRequest,
              " Param %d Value %s",
              parameter_handles[i],
              parameter_values[i].c_str());

    G.Out(pdGendoc,"exit Federation::broadcastInteraction with time");

}

// ----------------------------------------------------------------------------
//! broadcastInteraction without time
void
Federation::broadcastInteraction(FederateHandle federate_handle,
                                 InteractionClassHandle interaction,
                                 std::vector <ParameterHandle> &parameter_handles,
                                 std::vector <ParameterValue_t> &parameter_values,
                                 UShort list_size,
				 RegionHandle region_handle,
                                 const char *tag)
    throw (FederateNotExecutionMember,
           FederateNotPublishing,
           InteractionClassNotDefined,
           InteractionParameterNotDefined,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{

    G.Out(pdGendoc,"enter Federation::broadcastInteraction without time");

    // It may throw FederateNotExecutionMember.
    this->check(federate_handle);

    const RTIRegion *region = 0 ;
    if (region_handle != 0)
	region = root->getRegion(region_handle);

    root->Interactions->broadcastInteraction(federate_handle,
                                             interaction,
                                             parameter_handles,
                                             parameter_values,
                                             list_size,
					     region,
                                             tag);
    D.Out(pdRequest, "Federation %d: Broadcasted Interaction %d from Federate "
          "%d nb params %d.", handle, interaction, federate_handle, list_size);
    for (int i=0 ; i < list_size ; i++)
        D.Out(pdRequest,
              " Param %d Value %s",
              parameter_handles[i],
              parameter_values[i].c_str());

    G.Out(pdGendoc,"exit Federation::broadcastInteraction without time");

}

// ----------------------------------------------------------------------------
/** Removes an object instance from federation.
    @param federate Federate requesting removal
    @param id Object handle
    @param theTime Federation Time
    @param tag Label for this operation
 */
void
Federation::deleteObject(FederateHandle federate,
                         ObjectHandle id,
			 FederationTime theTime,
                         const char *tag)
    throw (FederateNotExecutionMember,
           DeletePrivilegeNotHeld,
           ObjectNotKnown,
           SaveInProgress,
           RestoreInProgress,
	   InvalidFederationTime,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    D.Out(pdRegister, "Federation %d: Federate %d destroys object %d.",
          this->handle, federate, id);

    root->deleteObjectInstance(federate, id, theTime, tag);
    objectHandles.free(id);
}
// ----------------------------------------------------------------------------
/** Removes an object instance from federation.
    @param federate Federate requesting removal
    @param id Object handle
    @param tag Label for this operation
 */
void
Federation::deleteObject(FederateHandle federate,
                         ObjectHandle id,
                         const char *tag)
    throw (FederateNotExecutionMember,
           DeletePrivilegeNotHeld,
           ObjectNotKnown,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    D.Out(pdRegister, "Federation %d: Federate %d destroys object %d.",
          this->handle, federate, id);

    root->deleteObjectInstance(federate, id, tag);
    objectHandles.free(id);
}

// ----------------------------------------------------------------------------
//! Add a new synchronization point to federation.
void
Federation::registerSynchronization(FederateHandle federate,
                                    const char *label,
                                    const char *tag)
    throw (FederateNotExecutionMember,
           FederationAlreadyPaused,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{

    G.Out(pdGendoc,"enter Federation::registerSynchronization for all federates");

    this->check(federate); // It may throw FederateNotExecutionMember.

    if (label == NULL )
        throw RTIinternalError("Bad pause label(null).");

    // Verify label does not already exists
    std::map<const char *, const char *>::const_iterator i ;
    i = synchronizationLabels.begin();
    for (; i != synchronizationLabels.end(); i++) {
        if (!strcmp((*i).first, label)) {
            throw FederationAlreadyPaused(""); // Label already pending.
        }
    }

    // If not already in pending labels, insert to list.
    synchronizationLabels.insert(pair<const char *, const char *>(strdup(label),
                                                                  strdup(tag)));

    // Add label to each federate (may throw RTIinternalError).
    FederateList::iterator j ;
    for (j = federates.begin(); j != federates.end(); ++j) {
        j->addSynchronizationLabel(label);
    }

    D[pdTerm] << "Federation " << handle << " is now synchronizing for label "
              << label << endl ;

    G.Out(pdGendoc,"exit  Federation::registerSynchronization for all federates");


}

// ----------------------------------------------------------------------------
//! Add a new synchronization point (with federates set) to federation.
void
Federation::registerSynchronization(FederateHandle federate,
                                    const char *label,
                                    const char *tag,
                                    unsigned short federate_setSize,
                                    std::vector <FederateHandle> &federate_set)
    throw (FederateNotExecutionMember,
           FederationAlreadyPaused,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{

    G.Out(pdGendoc,"enter Federation::registerSynchronization for federate set");

    this->check(federate); // It may throw FederateNotExecutionMember.

    if (label == NULL)
        throw RTIinternalError("Bad pause label(null).");

    // Verify label does not already exists
    std::map<const char *, const char *>::const_iterator i ;
    i = synchronizationLabels.begin();
    for (; i != synchronizationLabels.end(); i++) {
        if (!strcmp((*i).first, label)) {
            throw FederationAlreadyPaused(""); // Label already pending.
        }
    }

    // If not already in pending labels, insert to list.
    synchronizationLabels.insert(pair<const char *, const char *>(strdup(label),
                                                                  strdup(tag)));

    // Add label to each federate into the set only (may throw RTIinternalError).
    FederateList::iterator j ;
    for (int i=0 ; i < federate_setSize  ;i++ )
        {
        for (j = federates.begin(); j != federates.end(); ++j)
            {
            if ( (federate_set[i] == j->getHandle()) || (federate == j->getHandle()) )
                             j->addSynchronizationLabel(label);
            }
        }

    D[pdTerm] << "Federation " << handle << " is now synchronizing for label "
              << label << endl ;

    G.Out(pdGendoc,"exit  Federation::registerSynchronization for federate set");


}

// ----------------------------------------------------------------------------
/*! Broadcast an 'Announce Synchronization Point' when registering a new
  synchronization point.
*/
void
Federation::broadcastSynchronization(FederateHandle federate,
                                     const char *label,
                                     const char *tag)
    throw (RTIinternalError)
{

    G.Out(pdGendoc,"enter Federation::broadcastSynchronization");

    this->check(federate); // It may throw FederateNotExecutionMember.

    if (label == NULL)
        throw RTIinternalError("Bad pause label(null).");

    // broadcast announceSynchronizationPoint() to all federates in federation.
    NM_Announce_Synchronization_Point msg ;
    msg.federate = federate ;
    msg.federation = handle ;
    msg.setLabel(label);
    msg.setTag(tag);

    G.Out(pdGendoc,"      broadcastSynchronization is calling broadcastAnyMessage for all federates");

    broadcastAnyMessage(&msg, 0);

    G.Out(pdGendoc,"exit  Federation::broadcastSynchronization");

}

// ----------------------------------------------------------------------------
/*! Broadcast an 'Announce Synchronization Point' when registering a new
  synchronization point onto a set of federates
*/
void
Federation::broadcastSynchronization(FederateHandle federate,
                                     const char *label,
                                     const char *tag,
                                     unsigned short federate_setSize,
                                     std::vector <FederateHandle> &federate_set)
    throw (RTIinternalError)
{

    G.Out(pdGendoc,"enter Federation::broadcastSynchronization to some federates");

    this->check(federate); // It may throw FederateNotExecutionMember.

    if (label == NULL)
        throw RTIinternalError("Bad pause label(null or too long).");

    // broadcast announceSynchronizationPoint() to all federates in federation.
    NM_Announce_Synchronization_Point msg ;
    msg.federate = federate ;
    msg.federation = handle ;
    msg.setLabel(label);
    msg.setTag(tag);

    G.Out(pdGendoc,"      broadcastSynchronization is calling broadcastSomeMessage");

    broadcastSomeMessage(&msg, 0, federate_set, (unsigned short)federate_setSize);

    G.Out(pdGendoc,"exit  Federation::broadcastSynchronization to some federates");

}


// ----------------------------------------------------------------------------
//! Request a federation save with time.
/*! This service puts each federate from federation in saving state.
  !! the_time is not managed yet.
*/
void
Federation::requestFederationSave(FederateHandle the_federate,
                                  const char *the_label,
                                  FederationTime time )
    throw (FederateNotExecutionMember, SaveInProgress)
{
    G.Out(pdGendoc,"enter Federation::requestFederationSave with time");

    check(the_federate);

    if (saveInProgress)
        throw SaveInProgress("Already in saving state.");

    for (FederateList::iterator j = federates.begin(); j != federates.end(); ++j) {
        j->setSaving(true);
    }

    saveStatus = true ;
    saveInProgress = true ;
    saveLabel = the_label ;

    NM_Initiate_Federate_Save msg ;
    msg.federate = the_federate ;
    msg.federation = handle ;
    msg.setLabel(the_label);
    // timed message
    msg.setDate(time);

    G.Out(pdGendoc,"      requestFederationSave====>broadcast I_F_S to all");

    broadcastAnyMessage(&msg, 0);

    G.Out(pdGendoc,"exit  Federation::requestFederationSave with time");
}

// ----------------------------------------------------------------------------
//! Request a federation save without time.
/*! This service puts each federate from federation in saving state.
  !! the_time is not managed yet.
*/
void
Federation::requestFederationSave(FederateHandle the_federate,
                                  const char *the_label)
    throw (FederateNotExecutionMember, SaveInProgress)
{
    G.Out(pdGendoc,"enter Federation::requestFederationSave without time");

    check(the_federate);

    if (saveInProgress)
        throw SaveInProgress("Already in saving state.");

    for (FederateList::iterator j = federates.begin(); j != federates.end(); ++j) {
        j->setSaving(true);
    }

    saveStatus = true ;
    saveInProgress = true ;
    saveLabel = the_label ;

    NM_Initiate_Federate_Save msg ;
    msg.federate = the_federate ;
    msg.federation = handle ;
    msg.setLabel(the_label);

    G.Out(pdGendoc,"                  requestFederationSave====>broadcast I_F_S"
                   " to all");

    broadcastAnyMessage(&msg, 0);

    G.Out(pdGendoc,"exit  Federation::requestFederationSave without time");
}
// ----------------------------------------------------------------------------
/*! Received from a federate to inform a save has been received and is being
  processed.
  Maybe, a timeout should be set to determine if federate still alive.
*/
void
Federation::federateSaveBegun(FederateHandle the_federate)
    throw (FederateNotExecutionMember)
{
    G.Out(pdGendoc,"enter Federation::federateSaveBegun");
    check(the_federate);
    G.Out(pdGendoc,"exit  Federation::federateSaveBegun");
}

// ----------------------------------------------------------------------------
//! Informs that a federate returns a save end message (with success or not!).
void
Federation::federateSaveStatus(FederateHandle the_federate, bool the_status)
    throw (FederateNotExecutionMember)
{
    G.Out(pdGendoc,"enter Federation::federateSaveStatus");

    Federate &federate = getFederate(the_federate);
    federate.setSaving(false);

    if (!the_status)
        saveStatus = false ;

    // Verify that all federates save ended (complete or not).
    for (FederateList::iterator j = federates.begin(); j != federates.end(); ++j) {
        if (j->isSaving())
            {
            G.Out(pdGendoc,"exit  Federation::federateSaveStatus one federate has not save ended");
            return ;
            }
    }

    // Save RTIG Data for future restoration.
    if (saveStatus) {
        saveStatus = saveXmlData();
    }

    // Send end save message.
    std::auto_ptr<NetworkMessage> msg(NM_Factory::create(saveStatus ? NetworkMessage::FEDERATION_SAVED : NetworkMessage::FEDERATION_NOT_SAVED )) ;

    msg->federate = the_federate ;
    msg->federation = handle ;

    broadcastAnyMessage(msg.get(), 0);

    G.Out(pdGendoc,"            =======> broadcast F_S or F_N_S");

    // Reinitialize state.
    saveStatus = true ;
    saveInProgress = false ;

    G.Out(pdGendoc,"exit  Federation::federateSaveStatus");
}

// ----------------------------------------------------------------------------
//! Informs that a federate is requesting a save.
void
Federation::requestFederationRestore(FederateHandle the_federate,
                                     const char *the_label)
    throw (FederateNotExecutionMember)
{
    G.Out(pdGendoc,"enter Federation::requestFederationRestore");

    check(the_federate);

    if (restoreInProgress)
        throw RestoreInProgress("Already in restoring state.");

    Socket * socket ;
    NetworkMessage *msg;


    // Informs sending federate of success/failure in restoring.
    // At this point, only verify that file is present.
    bool success = true ;
#ifdef HAVE_XML
    string filename = string(name) + "_" + string(the_label) + ".xcs" ;
    doc = xmlParseFile(filename.c_str());

    // Did libXML manage to parse the file ?
    if (doc == 0) {
        cerr << "XML restore file not parsed successfully" << endl ;
        xmlFreeDoc(doc);
        success = false ;
    }
    if (success) {
        success = restoreXmlData();
    }
#else
    success = false ;
#endif // HAVE_XML

// JYR Note : forcing success to true to skip xmlParseFile (not compliant ?)
    success = true ;

    if (success) {
    	msg = NM_Factory::create(NetworkMessage::REQUEST_FEDERATION_RESTORE_SUCCEEDED);
    } else {
    	msg = NM_Factory::create(NetworkMessage::REQUEST_FEDERATION_RESTORE_FAILED);
    }

    msg->federate = the_federate ;
    msg->federation = handle ;
    msg->setLabel(the_label);

    socket = server->getSocketLink(msg->federate);

    if (success)
       G.Out(pdGendoc,"             =====> send message R_F_R_S to RTIA");
    else
       G.Out(pdGendoc,"             =====> send message R_F_R_F to RTIA");

    msg->send(socket,NM_msgBufSend);
    delete msg ;

    // Reading file failed: not restoring !
    if (!success)
        {
        G.Out(pdGendoc,"exit  Federation::requestFederationRestore on success false");
        return ;
        }

    // Otherwise...
    for (FederateList::iterator i = federates.begin(); i != federates.end(); ++i) {
        i->setRestoring(true);
    }
    restoreStatus = true ;
    restoreInProgress = true ;

    // Informs federates a new restore is being done.
    msg = NM_Factory::create(NetworkMessage::FEDERATION_RESTORE_BEGUN);
    msg->federate = the_federate ;
    msg->federation = handle ;

    G.Out(pdGendoc,"             =====> broadcast message F_R_B");

    broadcastAnyMessage(msg, 0);
    delete msg ;

    // For each federate, send an initiateFederateRestore with correct handle.
    msg = NM_Factory::create(NetworkMessage::INITIATE_FEDERATE_RESTORE);
    msg->federation = handle ;
    msg->setLabel(the_label);

    for (FederateList::iterator i = federates.begin(); i != federates.end(); ++i) {
        msg->federate = i->getHandle();

        // send message.
        socket = server->getSocketLink(msg->federate);
        G.Out(pdGendoc,"             =====> send message I_F_R to federate %d",msg->federate);
        msg->send(socket,NM_msgBufSend);
    }
    delete msg;
    G.Out(pdGendoc,"exit  Federation::requestFederationRestore");
}

// ----------------------------------------------------------------------------
//! Informs that a federate has ended a restore.
/*! If each federate in federation has ended restoring, this service send a
  federation restore status.
*/
void
Federation::federateRestoreStatus(FederateHandle the_federate,
                                  bool the_status)
    throw (FederateNotExecutionMember)
{
    G.Out(pdGendoc,"enter Federation::federateRestoreStatus");
    Federate &federate = getFederate(the_federate);
    federate.setRestoring(false);

    if (!the_status)
        restoreStatus = false ;

    // Verify that all federates save ended (complete or not).
    for (FederateList::iterator j = federates.begin(); j != federates.end(); ++j) {
        if (j->isRestoring())
            return ;
    }

    // Send end restore message.
    std::auto_ptr<NetworkMessage> msg(NM_Factory::create(restoreStatus ? NetworkMessage::FEDERATION_RESTORED : NetworkMessage::FEDERATION_NOT_RESTORED)) ;

    msg->federate = the_federate ;
    msg->federation = handle ;

    broadcastAnyMessage(msg.get(), 0);

    // Reinitialize state.
    restoreStatus = true ;
    restoreInProgress = false ;
    G.Out(pdGendoc,"exit  Federation::federateRestoreStatus");
}

// ----------------------------------------------------------------------------
//! Return the Federate whose Handle is theHandle, if found.
Federate &
Federation::getFederate(FederateHandle federate_handle)
    throw (FederateNotExecutionMember)
{
    for (FederateList::iterator i = federates.begin(); i != federates.end(); ++i) {
        if (i->getHandle() == federate_handle)
            return *i ;
    }

    throw FederateNotExecutionMember("Federate Handle not found.");
}

// ----------------------------------------------------------------------------
//! Return the Federate whose Name is theName, if found.
Federate &
Federation::getFederate(const char *federate_name)
    throw (FederateNotExecutionMember)
{
	std::stringstream msg;

    for (FederateList::iterator i = federates.begin(); i != federates.end(); ++i) {
        if (strcmp(i->getName(), federate_name) == 0)
            return *i ;
    }

    msg << "Federate <"<<federate_name<<"> not [yet] member of Federation <" << getName() <<">.";
    throw FederateNotExecutionMember(msg.str().c_str());
}

// ----------------------------------------------------------------------------
//! Return true if no federates are in federation.
/*! Return true if there are no Federates left in the Federation, else throw
  FederatesCurrentlyJoined.
*/
bool
Federation::empty() const
    throw (FederatesCurrentlyJoined)
{
    if (federates.empty())
        return true ;
    else
        throw FederatesCurrentlyJoined("");
}

// ----------------------------------------------------------------------------
//! Return true if federate is in federation.
/*! Return true if the Federate is part of the Federation, else throw an
  exception.
*/
bool
Federation::check(FederateHandle federate_handle) const
    throw (FederateNotExecutionMember)
{
    for (FederateList::const_iterator i = federates.begin(); i != federates.end(); ++i) {
        if (i->getHandle() == federate_handle)
            return true ;
    }

    throw FederateNotExecutionMember("");
}

// ----------------------------------------------------------------------------
/** Make a federate resign the federation. This function tries to
    remove all references to this federate in the federation. To be used
    when a Federate is supposed to have crashed.
    @param federate Handle of the federate to kill
*/
void
Federation::kill(FederateHandle federate)
    throw ()
{
    // NOTE: Connection to the federate is already closed.
    D.Out(pdInit, "Killing Federate %d.", federate);

    // is regulator ?
    try {
        removeRegulator(federate);
        D.Out(pdInit, "Regulator Federate %d removed...", federate);
    } catch (Exception &e) {}

    // is constrained ?
    try {
        removeConstrained(federate);
        D.Out(pdInit, "Constrained Federate %d removed...", federate);
    } catch (Exception &e) {}

    // Remove references to this federate in root object
    root->killFederate(federate);
    D[pdTrace] << "Federate " << federate << " removed from the Root Object "
               << endl ;

    // delete from federations list
    try {
        remove(federate);
        D.Out(pdInit, "Federate %d removed...", federate);
    } catch (Exception &e) {}
}

// ----------------------------------------------------------------------------
// publishInteraction

void
Federation::publishInteraction(FederateHandle federate,
                               InteractionClassHandle interaction,
                               bool pub)
    throw (InteractionClassNotDefined,
           FederateNotExecutionMember,
           SaveInProgress,
           SecurityError,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw InteractionClassNotDefined
    root->Interactions->publish(federate, interaction, pub);
    D.Out(pdRequest,
          "Federation %d: Federate %d has(un)published Interaction %d.",
          handle, federate, interaction);
}

// ----------------------------------------------------------------------------
// publishObject

void
Federation::publishObject(FederateHandle federate,
                          ObjectClassHandle object,
                          std::vector <AttributeHandle> &attributes,
                          UShort list_size,
                          bool pub)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           FederateNotExecutionMember,
           SaveInProgress,
           SecurityError,
           RestoreInProgress,
           RTIinternalError)
{
    G.Out(pdGendoc,"enter Federation::publishObject");
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->ObjectClasses->publish(federate, object, attributes, list_size, pub);

    D.Out(pdRegister,
          "Federation %d: Federate %d(un)publishes %d attrib. of ObjClass %d.",
          handle, federate, list_size, object);
    G.Out(pdGendoc,"exit  Federation::publishObject");
}

// ----------------------------------------------------------------------------
//! Adds a new object instance to federation.
ObjectHandle
Federation::registerObject(FederateHandle federate,
                           ObjectClassHandle class_handle,
                           const char *object_name)
    throw (FederateNotExecutionMember,
           FederateNotPublishing,
           ObjectAlreadyRegistered,
           ObjectClassNotDefined,
           ObjectClassNotPublished,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{

    ObjectHandle new_id = objectHandles.provide();

    G.Out(pdGendoc,"enter Federation::registerObject");
    D.Out(pdRegister,
          "Federation %d: Federate %d registering Object %d of Class %d.",
          handle, federate, new_id, class_handle);

    // create a name if necessary
    string strname = "" ;
    strname += object_name ? string(object_name) : "HLA" + new_id ;

    // Register Object.
    root->registerObjectInstance(federate, class_handle, new_id,
				 strname.c_str());
    G.Out(pdGendoc,"exit Federation::registerObject");
    return new_id ;
}

// ----------------------------------------------------------------------------
/** Remove a federate.
    @param federate_handle Handle of the federate to remove.
    @bug Currently does not check if Federate owns attributes. The
    Federate Object is deleted.
*/
void
Federation::remove(FederateHandle federate_handle)
    throw (FederateOwnsAttributes, FederateNotExecutionMember)
{
    for (FederateList::iterator i = federates.begin(); i != federates.end(); ++i) {
        if (i->getHandle() == federate_handle) {
            // BUG: RemoveFederate: Should see if Federate owns attributes
	    federateHandles.free(federate_handle);
            federates.erase(i);

            D.Out(pdInit, "Federation %d: Removed Federate %d.", handle,
                  federate_handle);
            return ;
        }
    }

    D.Out(pdExcept, "Federation %d could not remove unknown federate %d.",
          handle, federate_handle);
    throw FederateNotExecutionMember("");
}

// ----------------------------------------------------------------------------
//! Set Federate's IsConstrained to false.
void
Federation::removeConstrained(FederateHandle federate_handle)
    throw (FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember
    Federate &federate = getFederate(federate_handle);

    if (!federate.isConstrained()) {
        D.Out(pdExcept, "Federate %d was not constrained.", federate_handle);
        throw RTIinternalError("Time constrained not enabled.");
    }

    federate.setConstrained(false);
    D.Out(pdTerm, "Federation %d: Federate %d is not constrained anymore.",
          handle, federate_handle);
}

// ----------------------------------------------------------------------------
//! Remove the Federate from the Regulators' list.
void
Federation::removeRegulator(FederateHandle federate_handle)
    throw (FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember
    Federate &federate = getFederate(federate_handle);

    // It may throw RTIinternalError if Federate was not regulators.
    regulators.remove(federate_handle);

    federate.setRegulator(false);

    D.Out(pdTerm, "Federation %d: Federate %d is not a regulator anymore.",
          handle, federate_handle);

    NM_Set_Time_Regulating msg ;
    msg.federation = handle ;
    msg.federate = federate_handle ;
    msg.regulatorOff();

    broadcastAnyMessage(&msg, 0);
}

// ----------------------------------------------------------------------------
//! unregisterSynchronization.
void
Federation::unregisterSynchronization(FederateHandle federate_handle,
                                      const char *label)
    throw (FederateNotExecutionMember,
           FederationNotPaused,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{

    G.Out(pdGendoc,"enter Federation::unregisterSynchronization");

    this->check(federate_handle); // It may throw FederateNotExecutionMember.

    if (label == NULL)
        throw RTIinternalError("Bad pause label(null).");

    // Set federate synchronized on this label.
    Federate &federate = getFederate(federate_handle);
    federate.removeSynchronizationLabel(label);

    // Test in every federate is synchronized. Otherwise, quit method.
    for (FederateList::iterator j = federates.begin(); j != federates.end(); ++j) {
        if (j->isSynchronizationLabel(label))
            return ;
    }

    // All federates from federation has called synchronizationPointAchieved.

    D.Out(pdTerm, "Federation %d is not Paused anymore.", handle);
    // Remove label from federation list.
    std::map<const char *, const char *>::iterator i ;
    i = synchronizationLabels.begin();
    for (; i != synchronizationLabels.end(); i++) {
        if (!strcmp((*i).first, label)) {
            // Allocated by strdup().
            free(const_cast<char *>((*i).first));
            free(const_cast<char *>((*i).second));
            synchronizationLabels.erase(i);
            break ;
        }
    }

    // send a federationSynchronized().
    NM_Federation_Synchronized msg ;
    msg.federation = handle ;
    msg.federate = federate_handle ;
    msg.setLabel(label);

    broadcastAnyMessage(&msg, 0);

    D.Out(pdTerm, "Federation %d is synchronized on %s.", handle, label);

    G.Out(pdGendoc,"exit  Federation::unregisterSynchronization");

}

// ----------------------------------------------------------------------------
// subscribeInteraction

void
Federation::subscribeInteraction(FederateHandle federate,
                                 InteractionClassHandle interaction,
                                 bool sub)
    throw (InteractionClassNotDefined,
           FederateNotExecutionMember,
           SaveInProgress,
           SecurityError,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->Interactions->subscribe(federate, interaction, 0, sub);
    D.Out(pdRegister,
          "Federation %d: Federate %d(un)subscribes to Interaction %d.",
          handle, federate, interaction);
}

// ----------------------------------------------------------------------------
// subscribeObject

void
Federation::subscribeObject(FederateHandle federate,
                            ObjectClassHandle object,
                            std::vector <AttributeHandle> &attributes,
                            UShort list_size)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           FederateNotExecutionMember,
           SaveInProgress,
           SecurityError,
           RestoreInProgress,
           RTIinternalError)
{
    G.Out(pdGendoc,"enter Federation::subscribeObject");
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->ObjectClasses->subscribe(federate, object, attributes, list_size);
    D.Out(pdRegister,
          "Federation %d: Federate %d(un)sub. to %d attrib. of ObjClass %d.",
          handle, federate, list_size, object);
    G.Out(pdGendoc,"exit  Federation::subscribeObject");
}

// ----------------------------------------------------------------------------
// updateAttributeValues with time

void
Federation::updateAttributeValues(FederateHandle federate,
                                  ObjectHandle id,
                                  std::vector <AttributeHandle> &attributes,
                                  std::vector <AttributeValue_t> &values,
                                  UShort list_size,
                                  FederationTime time,
                                  const char *tag)
    throw (FederateNotExecutionMember,
           ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    G.Out(pdGendoc,"enter Federation::updateAttributeValues with time");
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->ObjectClasses->updateAttributeValues(federate, id, attributes, values,
                                               list_size, time, tag);

    D.Out(pdRegister,
          "Federation %d: Federate %d updated attributes of Object %d.",
          handle, federate, id);
    G.Out(pdGendoc,"exit  Federation::updateAttributeValues with time");
}

// ----------------------------------------------------------------------------
// updateAttributeValues without time

void
Federation::updateAttributeValues(FederateHandle federate,
                                  ObjectHandle id,
                                  std::vector <AttributeHandle> &attributes,
                                  std::vector <AttributeValue_t> &values,
                                  UShort list_size,
                                  const char *tag)
    throw (FederateNotExecutionMember,
           ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    G.Out(pdGendoc,"enter Federation::updateAttributeValues without time");
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->ObjectClasses->updateAttributeValues(federate, id, attributes, values,
                                               list_size, tag);

    D.Out(pdRegister,
          "Federation %d: Federate %d updated attributes of Object %d.",
          handle, federate, id);
    G.Out(pdGendoc,"exit  Federation::updateAttributeValues without time");
}
// ----------------------------------------------------------------------------
//! Update the current time of a regulator federate.
void
Federation::updateRegulator(FederateHandle federate_handle,
                            FederationTime time)
    throw (FederateNotExecutionMember,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember
    Federate &federate = getFederate(federate_handle);

    if (!federate.isRegulator()) {
        D.Out(pdExcept, "Federate %d is not a regulator.", federate_handle);
        throw RTIinternalError("Time regulation not enabled.");
    }

    D.Out(pdTerm, "Federation %d: Federate %d's new time is %f.",
          handle, federate_handle, time);

    regulators.update(federate_handle, time);

    NM_Message_Null msg ;
    msg.federation = handle ;
    msg.federate = federate_handle ;
    msg.setDate(time);

    broadcastAnyMessage(&msg, federate_handle);
}

// ----------------------------------------------------------------------------
// isOwner (isAttributeOwnedByFederate)
bool
Federation::isOwner(FederateHandle federate,
                    ObjectHandle id,
                    AttributeHandle attribute)
    throw (FederateNotExecutionMember,
           ObjectNotKnown,
           AttributeNotDefined,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    D.Out(pdDebug, "Owner of Object %u Atrribute %u", id, attribute);

    // It may throw *NotDefined
    return root->objects->isAttributeOwnedByFederate(id, attribute, federate);
}

// ----------------------------------------------------------------------------
// queryAttributeOwnership

void
Federation::queryAttributeOwnership(FederateHandle federate,
                                    ObjectHandle id,
                                    AttributeHandle attribute)
    throw (FederateNotExecutionMember,
           ObjectNotKnown,
           AttributeNotDefined,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    D.Out(pdDebug, "Owner of Object %u Atrribute %u", id, attribute);

    // It may throw *NotDefined
    root->objects->queryAttributeOwnership(id, attribute, federate);
}

// ----------------------------------------------------------------------------
// negotiateDivestiture (negotiatedAttributeOwnershipDivestiture)

void
Federation::negotiateDivestiture(FederateHandle federate,
                                 ObjectHandle id,
                                 std::vector <AttributeHandle> &attribs,
                                 UShort list_size,
                                 const char *tag)
    throw (FederateNotExecutionMember,
           ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           AttributeAlreadyBeingDivested,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->ObjectClasses->negotiatedAttributeOwnershipDivestiture(federate,
                                                                 id,
                                                                 attribs,
                                                                 list_size,
                                                                 tag);
}

// ----------------------------------------------------------------------------
// acquireIfAvailable (attributeOwnershipAcquisitionIfAvailable)

void
Federation::acquireIfAvailable(FederateHandle federate,
                               ObjectHandle id,
                               std::vector <AttributeHandle> &attribs,
                               UShort list_size)
    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           AttributeAlreadyBeingAcquired,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->ObjectClasses->attributeOwnershipAcquisitionIfAvailable(federate,
                                                                  id,
                                                                  attribs,
                                                                  list_size);
}

// ----------------------------------------------------------------------------
// divest (unconditionalAttributeOwnershipDivestiture)

void
Federation::divest(FederateHandle federate,
                   ObjectHandle id,
                   std::vector <AttributeHandle> &attrs,
                   UShort list_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->ObjectClasses->unconditionalAttributeOwnershipDivestiture(federate,
                                                                    id,
                                                                    attrs,
                                                                    list_size);
}

// ----------------------------------------------------------------------------
// attributeOwnershipAcquisition

void
Federation::acquire(FederateHandle federate,
                    ObjectHandle id,
                    std::vector <AttributeHandle> &attributes,
                    UShort list_size,
                    const char *tag)
    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->ObjectClasses->attributeOwnershipAcquisition(federate,
                                                       id,
                                                       attributes,
                                                       list_size,
                                                       tag);

    D.Out(pdDebug, "Acquisition on Object %u ", id);
}

// ----------------------------------------------------------------------------
// cancelDivestiture (cancelNegotiatedAttributeOwnershipDivestiture)

void
Federation::cancelDivestiture(FederateHandle federate,
                              ObjectHandle id,
                              std::vector <AttributeHandle> &attributes,
                              UShort list_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           AttributeDivestitureWasNotRequested,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->objects->cancelNegotiatedAttributeOwnershipDivestiture(federate,
                                                                 id,
                                                                 attributes,
                                                                 list_size);

    D.Out(pdDebug, "CancelDivestiture sur Objet %u ", id);
}

// ----------------------------------------------------------------------------
// respondRelease (attributeOwnershipRealeaseResponse)

AttributeHandleSet*
Federation::respondRelease(FederateHandle federate,
                           ObjectHandle id,
                           std::vector <AttributeHandle> &attributes,
                           UShort list_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           FederateWasNotAskedToReleaseAttribute,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    D.Out(pdDebug, "RespondRelease on Object %u.", id);

    // It may throw *NotDefined
    return(root->ObjectClasses->attributeOwnershipReleaseResponse(federate,
                                                                  id,
                                                                  attributes,
                                                                  list_size));
}

// ----------------------------------------------------------------------------
// cancelAttributeOwnershipAcquisition

void
Federation::cancelAcquisition(FederateHandle federate,
                              ObjectHandle id,
                              std::vector <AttributeHandle> &attributes,
                              UShort list_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeAlreadyOwned,
           AttributeAcquisitionWasNotRequested,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    D.Out(pdDebug, "CancelAcquisition sur Objet %u ", id);

    // It may throw *NotDefined
    root->ObjectClasses->cancelAttributeOwnershipAcquisition(federate,
                                                             id,
                                                             attributes,
                                                             list_size);
}

// ----------------------------------------------------------------------------
long
Federation::createRegion(FederateHandle federate,
                         SpaceHandle space,
                         long nb_extents)
    throw (SpaceNotDefined, InvalidExtents, SaveInProgress, RestoreInProgress,
           RTIinternalError)
{
    this->check(federate);

    return root->createRegion(space, nb_extents);
}

// ----------------------------------------------------------------------------
// modifyRegion
void
Federation::modifyRegion(FederateHandle federate, RegionHandle region,
			 const std::vector<Extent> &extents)
    throw (RegionNotKnown, InvalidExtents, SaveInProgress, RestoreInProgress,
	   RTIinternalError)
{
    check(federate);
    root->modifyRegion(region, extents);
}

// ----------------------------------------------------------------------------
//! deleteRegion
void
Federation::deleteRegion(FederateHandle federate,
                         long region)
    throw (RegionNotKnown, RegionInUse, SaveInProgress, RestoreInProgress,
           RTIinternalError)
{
    this->check(federate);

    if (saveInProgress) {
        throw SaveInProgress("");
    }
    if (restoreInProgress) {
        throw RestoreInProgress("");
    }

    // TODO: check RegionInUse
    root->deleteRegion(region);
}

// ----------------------------------------------------------------------------
//! associateRegion
void
Federation::associateRegion(FederateHandle federate,
			    ObjectHandle object,
			    RegionHandle the_handle,
			    unsigned short nb,
			    std::vector <AttributeHandle> &attributes)
    throw (RegionNotKnown, SaveInProgress, RestoreInProgress, RTIinternalError)
{
    check(federate);

    RTIRegion *region = root->getRegion(the_handle);

    root->getObject(object)->unassociate(region);

    for (int i = 0 ; i < nb ; ++i) {
	root->getObjectAttribute(object, attributes[i])->associate(region);
    }
}

// ----------------------------------------------------------------------------
//! unassociateRegion
void
Federation::unassociateRegion(FederateHandle federate, ObjectHandle object,
			      RegionHandle the_handle)
    throw (RegionNotKnown, SaveInProgress, RestoreInProgress,
	   RTIinternalError)
{
    check(federate);

    RTIRegion *region = root->getRegion(the_handle);
    root->getObject(object)->unassociate(region);
}

// ----------------------------------------------------------------------------
//! subscribeAttributesWR
void
Federation::subscribeAttributesWR(FederateHandle federate,
				  ObjectClassHandle c,
				  RegionHandle region_handle,
				  unsigned short nb,
				  std::vector <AttributeHandle> &attributes)
    throw (RegionNotKnown,
	   SaveInProgress,
	   RestoreInProgress,
	   RTIinternalError)
{
    check(federate);
    root->ObjectClasses->subscribe(federate, c, attributes, nb,
				   root->getRegion(region_handle));
}

// ----------------------------------------------------------------------------
//! unsubscribeAttributesWR
void
Federation::unsubscribeAttributesWR(FederateHandle federate,
				    ObjectClassHandle object_class,
				    RegionHandle region_handle)
	throw (RegionNotKnown,
	       SaveInProgress,
	       RestoreInProgress,
	       RTIinternalError)
{
    check(federate);

    RTIRegion *region = root->getRegion(region_handle);

    root->getObjectClass(object_class)->unsubscribe(federate, region);
}

// ----------------------------------------------------------------------------
void
Federation::subscribeInteractionWR(FederateHandle federate,
				   InteractionClassHandle interaction,
				   RegionHandle region_handle)
	throw (RegionNotKnown,
	       SaveInProgress,
	       RestoreInProgress,
	       RTIinternalError)
{
    check(federate);

    RTIRegion *region = root->getRegion(region_handle);

    root->getInteractionClass(interaction)->subscribe(federate, region);
}

// ----------------------------------------------------------------------------
void
Federation::unsubscribeInteractionWR(FederateHandle federate,
				     InteractionClassHandle interaction,
				     RegionHandle region_handle)
	throw (RegionNotKnown,
	       SaveInProgress,
	       RestoreInProgress,
	       RTIinternalError)
{
    check(federate);

    RTIRegion *region = root->getRegion(region_handle);

    root->getInteractionClass(interaction)->unsubscribe(federate, region);
}

// ----------------------------------------------------------------------------
ObjectHandle
Federation::registerObjectWithRegion(FederateHandle federate,
				     ObjectClassHandle class_handle,
				     const char *object_name,
				     RegionHandle region_handle,
				     int nb,
				     std::vector <AttributeHandle> &attributes)
    throw (ObjectClassNotDefined, ObjectClassNotPublished,
	   AttributeNotDefined, AttributeNotPublished, RegionNotKnown,
	   InvalidRegionContext, ObjectAlreadyRegistered,
	   SaveInProgress, RestoreInProgress,
	   RTIinternalError)
{
    G.Out(pdGendoc,"enter Federation::registerObjectWithRegion");
    check(federate);

    // Register object
    ObjectHandle object = objectHandles.provide();
    D[pdDebug] << "Register object with region : Object " << object
	       << ", class " << class_handle << ", region " << region_handle
	       << std::endl ;
    string strname = "" ;    // create a name if necessary
    strname += object_name ? string(object_name) : "HLA" + object ;

    root->registerObjectInstance(federate, class_handle, object,
				 strname.c_str());

    D[pdDebug] << "- object \"" << strname.c_str()
	       << "\" registered" << std::endl ;

    // Associate region
    RTIRegion *region = root->getRegion(region_handle);
    root->getObject(object)->unassociate(region);

    for (int i = 0 ; i < nb ; ++i) {
	root->getObjectAttribute(object, attributes[i])->associate(region);
    }

    D[pdDebug] << "- " << nb << " attribute(s) associated with region "
	       << region_handle << std::endl ;
    G.Out(pdGendoc,"exit  Federation::registerObjectWithRegion");
    return object ;
}

// ----------------------------------------------------------------------------
bool
Federation::restoreXmlData()
{
#ifndef HAVE_XML
    return false ;
#else
    xmlNodePtr cur ;

    cur = xmlDocGetRootElement(doc);
    if (cur == 0) {
        cerr << "XML file is empty" << endl ;
        xmlFreeDoc(doc);
        return false ;
    }

    // Is this root element an ROOT_NODE ?
    if (xmlStrcmp(cur->name, ROOT_NODE)) {
        cerr << "Wrong XML file: not the expected root node" << endl ;
        return false ;
    }

    cur = cur->xmlChildrenNode ;
    if (xmlStrcmp(cur->name, NODE_FEDERATION)) {
        cerr << "Wrong XML file structure" << endl ;
        return false ;
    }

    if (strcmp(name.c_str(), XmlParser::CleanXmlGetProp(cur,(const xmlChar*)"name")) != 0) {
        cerr << "Wrong federation name" << endl ;
    }


    cur = cur->xmlChildrenNode ;

    bool status ;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, NODE_FEDERATE))) {
            for (FederateList::iterator i = federates.begin(); i != federates.end(); ++i) {
                if (!strcmp(i->getName(),XmlParser::CleanXmlGetProp(cur, (const xmlChar*) "name"))) {
                    // Set federate constrained status
                    if (!strcmp("true", XmlParser::CleanXmlGetProp(cur, (const xmlChar*) "constrained"))) {
                        status = true ;
                    }
                    else {
                        status = false ;
                    }

                    i->setConstrained(status);

                    // Set federate regulating status
                    status = !strcmp("true", XmlParser::CleanXmlGetProp(cur, (const xmlChar *) "regulator"));

                    i->setRegulator(status);

                    i->setHandle(strtol(XmlParser::CleanXmlGetProp(cur, (const xmlChar *) "handle"), 0, 10));
                    break ;
                }
            }
        }
        cur = cur->next ;
    }

    return status ;
#endif // HAVE_XML
}

// ----------------------------------------------------------------------------
bool
Federation::saveXmlData()
{
#ifndef HAVE_XML
    return false ;
#else
    doc = xmlNewDoc((const xmlChar *) "1.0");
    doc->children = xmlNewDocNode(doc, NULL, ROOT_NODE, NULL);

    xmlNodePtr federation ;
    federation = xmlNewChild(doc->children, NULL, NODE_FEDERATION, NULL);

    xmlSetProp(federation, (const xmlChar *) "name", (const xmlChar *) name.c_str());

    char t[10] ;
    sprintf(t, "%ld", handle);
    xmlSetProp(federation, (const xmlChar *) "handle", (const xmlChar *) t);

    xmlNodePtr federate ;

    for (FederateList::iterator i = federates.begin(); i != federates.end(); ++i) {
        federate = xmlNewChild(federation, NULL, NODE_FEDERATE, NULL);

        xmlSetProp(federate,
                   (const xmlChar *) "name",
                   (const xmlChar *) i->getName());

        sprintf(t, "%ld", i->getHandle());
        xmlSetProp(federate, (const xmlChar *) "handle", (const xmlChar *) t);

        xmlSetProp(federate,
                   (const xmlChar *)"constrained", (const xmlChar *)
                   ((i->isConstrained()) ? "true" : "false"));
        xmlSetProp(federate, (const xmlChar *) "regulator",
                   (const xmlChar *)((i->isRegulator()) ? "true" : "false"));
    }

    xmlSetDocCompressMode(doc, 9);

    string filename = string(name) + "_" + string(saveLabel) + ".xcs" ;
    xmlSaveFile(filename.c_str(), doc);

    // TODO: tests

    return true ;
#endif // HAVE_XML
}
// ----------------------------------------------------------------------------
FederateHandle
Federation::requestObjectOwner(FederateHandle theFederateHandle,
                             ObjectHandle theObject,
                             std::vector <AttributeHandle> &theAttributeList,
                             UShort theListSize)
        throw (ObjectNotKnown)
{
FederateHandle theOwnerHandle ;
NM_Provide_Attribute_Value_Update mess ;

    G.Out(pdGendoc,"enter Federation::requestObjectOwner");

    // Request Object.
    theOwnerHandle = root->requestObjectOwner(theFederateHandle,theObject) ;

    // Send a PROVIDE_ATTRIBUTE_VALUE_UPDATE to the owner
    mess.federate = theFederateHandle ;
    mess.object = theObject ;
    mess.handleArraySize = theListSize ;
    mess.handleArray.resize(theListSize) ;
    for (int i = 0 ; i < theListSize ; i++)
        {
        mess.handleArray[i] = theAttributeList[i] ;
        }

     // JYR : BUG if getSocketLink return NULL means
     // owner federate has been killed and so rtig don't crash
     // better development needed
     if ( server->getSocketLink(theOwnerHandle) == NULL )
        {
        throw ObjectNotKnown ( "Owner federate killed") ;
        }

     mess.send(server->getSocketLink(theOwnerHandle),NM_msgBufSend);

   G.Out(pdGendoc,"            requestObjectOwner ===> write PAVU to RTIA %d"
                   ,theOwnerHandle);
    G.Out(pdGendoc,"exit  Federation::requestObjectOwner");
    return(theOwnerHandle);

}

}} // namespace certi/rtig

// $Id: Federation.cc,v 3.100 2008/11/09 12:41:39 gotthardp Exp $

