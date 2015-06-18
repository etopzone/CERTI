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
// ----------------------------------------------------------------------------

#include <config.h>
#include "Federation.hh"
#include "NM_Classes.hh"
#include <cstring>
#include <cerrno>
#include <memory>
#include <sys/stat.h>
#include <time.h>

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
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#define ROOT_NODE (const xmlChar*) "rtigSaveData"
#define NODE_FEDERATION (const xmlChar*) "federation"
#define NODE_FEDERATE (const xmlChar*) "federate"
#endif // HAVE_XML

// Path splitting functions
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

#if defined(_WIN32) && !defined(__MINGW32__)
# define strcasecmp stricmp
#endif

namespace certi {

class XmlParser ;

namespace rtig {

static PrettyDebug D("FEDERATION", __FILE__);
static PrettyDebug G("GENDOC",__FILE__);
static PrettyDebug DNULL("RTIG_NULLMSG","[RTIG NULL MSG]");

/**
 * \defgroup certi_FOM_FileSearch CERTI FOM file search algorithm
 * When a federate calls the CreateFederationExcution API
 * RTIG tries to open FOM file from different predefined places,
 * using various environment variables:
 *
 * -# Bare filename considered as a path provided through <code> FEDid_name </code>
 * -# Use CERTI federation object model serach PATH
 *    <code>getenv(CERTI_FOM_PATH) + FEDid_name</code>.
 *    <br><code>CERTI_FOM_PATH</code> environment variable may contains a list of path
 *    separated with ':'.
 * -# Using the <code> CERTI_HOME </code> environment variable
 *    <code>getenv(CERTI_HOME)+"/share/federations/"+ FEDid_name</code>
 * -# installation place plus <code>FEDid_name</code>
 *    <br><code>PACKAGE_INSTALL_PREFIX + "/share/federation/" + FEDid_name</code>
 * -# on Unix <code>"/usr/local/share/federation/" + FEDid_name</code>
 *    for backward compatibility reason.
 */

#ifdef FEDERATION_USES_MULTICAST
Federation::Federation(const std::string& federation_name,
                       FederationHandle federation_handle,
                       SocketServer &socket_server,
                       AuditFile &audit_server,
                       SocketMC *mc_link,
                       int theVerboseLevel)
#else
Federation::Federation(const std::string& federation_name,
                       Handle federation_handle,
                       SocketServer &socket_server,
                       AuditFile &audit_server,
                       const std::string& FEDid_name,
                       int theVerboseLevel)
#endif
throw (CouldNotOpenFED, ErrorReadingFED, MemoryExhausted, SecurityError,
        RTIinternalError)
        : federateHandles(1), objectHandles(1), saveInProgress(false),
          restoreInProgress(false), saveStatus(true), restoreStatus(true),
          verboseLevel(theVerboseLevel)

{
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
    if (federation_name.empty() || (federation_handle == 0))
        throw RTIinternalError("Null init parameter in Federation creation.");

    name = federation_name;

    // Default Attribute values
    handle = federation_handle;
    FEDid  = FEDid_name;

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
    // --> see doxygen doc at the top of this file.
    string filename   = FEDid;
    bool   filefound  = false;
    if (verboseLevel>0) {
        cout << "Looking for FOM file... " << endl ;
        cout << "   Trying... " << filename;
    }
    filefound = (0==STAT_FUNCTION(filename.c_str(),&file_stat));

    /* This is the main path handling loop */
    if (!filefound) {
        vector<string> fom_paths;
#ifdef WIN32
        char temp[260];
        GetCurrentDirectory(260,temp);
        fom_paths.insert(fom_paths.end(),string(temp)+"\\share\\federations\\");
#endif

        /* add paths from CERTI_FOM_PATH */
        if (NULL!=getenv("CERTI_FOM_PATH")) {
            string path = getenv("CERTI_FOM_PATH");
            vector<string> certi_fom_paths = split(path, ':');
            fom_paths.insert(fom_paths.end(),certi_fom_paths.begin(),certi_fom_paths.end());
        }

        if (NULL!=getenv("CERTI_HOME")) {
#ifdef WIN32
            fom_paths.insert(fom_paths.end(),string(getenv("CERTI_HOME"))+"\\share\\federations\\");
#else
            fom_paths.insert(fom_paths.end(),string(getenv("CERTI_HOME"))+"/share/federations/");
#endif
        }

#ifdef WIN32
        fom_paths.insert(fom_paths.end(),PACKAGE_INSTALL_PREFIX "\\share\\federations\\");
#else
        fom_paths.insert(fom_paths.end(),PACKAGE_INSTALL_PREFIX "/share/federations/");
        fom_paths.insert(fom_paths.end(),"/usr/local/share/federations/");
#endif

        /* try to open FED using fom_paths prefixes */
        for (vector<string>::iterator i = fom_paths.begin(); i != fom_paths.end(); i++) {
            if (verboseLevel>0) {
                cout << " --> cannot access." <<endl;
            }
            filename = (*i)+FEDid_name;
            if (verboseLevel>0) {
                cout << "   Now trying... " << filename;
            }
            filefound = (0==STAT_FUNCTION(filename.c_str(),&file_stat));
            if (filefound) break;
        }
    }

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

#ifdef HAVE_XML
        	XmlParser *parser = NULL;
            if (XmlParser::exists()) {

            	switch (XmlParser::version(filename)) {
            	case XmlParser::XML_IEEE1516_2000:
            	case XmlParser::XML_LEGACY:
            		parser = new XmlParser2000(root);
            		break;
            	case XmlParser::XML_IEEE1516_2010:
            		parser = new XmlParser2010(root);
            		break;
            	}
                server->audit << ", XML File : " << filename ;

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
            else 
#endif
			{
                cerr << "CERTI was Compiled without XML support" << endl ;
                G.Out(pdGendoc,"exit Federation::Federation on exception CouldNotOpenFED");
                throw CouldNotOpenFED("Could not parse XML file. (CERTI Compiled without XML lib.)");
            }
        }
    }

    minNERx.setZero();
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
	return _handleFederateMap.size();
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
const std::string&
Federation::getName() const
{
	return name;
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
const std::string&
Federation::getFEDid() const
{
	return FEDid;
}

// ----------------------------------------------------------------------------
/*! Add the Federate to the Federation, and return its new federate handle.
  Also send
  Null messages from all others federates to initialize its LBTS, and
  finally a RequestPause message if the Federation is already paused.
 */
FederateHandle
Federation::add(const std::string& federate_name, SocketTCP *tcp_link)
throw (FederateAlreadyExecutionMember, MemoryExhausted, RTIinternalError)
{
	try {
		getFederate(federate_name);
		throw FederateAlreadyExecutionMember(federate_name);
	}
	catch (FederateNotExecutionMember &e) {
		// Nothing to do.
	}

	FederateHandle federate_handle = federateHandles.provide();
	_handleFederateMap.insert(HandleFederateMap::value_type(federate_handle, Federate(federate_name, federate_handle)));
	Federate& federate = getFederate(federate_handle);
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
			nullMessage.setFederation(handle);
			nullMessage.setFederate(v[i].first);
			nullMessage.setDate(v[i].second);
			D.Out(pdTerm,
					"Sending NULL message(type %d) from %d to new federate.",
					nullMessage.getMessageType(), nullMessage.getFederate());

			nullMessage.send(tcp_link,NM_msgBufSend);
		}

		// If federation is synchronizing, put federate in same state.
		if (isSynchronizing()) {
			ASPMessage.setFederate(federate_handle);
			ASPMessage.setFederation(handle);

			std::map<std::string, std::string>::const_iterator i ;
			i = synchronizationLabels.begin();
			for (; i != synchronizationLabels.end(); i++) {
				ASPMessage.setLabel((*i).first);
				ASPMessage.setTag((*i).second);
				D.Out(pdTerm, "Sending synchronization message %s (type %d)"
						" to the new Federate.", (*i).first.c_str(), ASPMessage.getMessageType());

				ASPMessage.send(tcp_link,NM_msgBufSend);
				federate.addSynchronizationLabel((*i).first);
			}
		}
	}
	catch (NetworkError&) {
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
		throw RTIinternalError("Time Constrained already enabled.");
	}

	federate.setConstrained(true);
	D.Out(pdTerm, "Federation %d: Federate %d is now constrained.",
			handle, federate_handle);
		}

// ----------------------------------------------------------------------------
// Set Class Relevance Advisory Switch
void
Federation::setClassRelevanceAdvisorySwitch(FederateHandle federate_handle)
throw (FederateNotExecutionMember,
		SaveInProgress,
		RestoreInProgress,
		RTIinternalError)
		{
	// It may throw FederateNotExecutionMember
	Federate &federate = getFederate(federate_handle);

	if (federate.isClassRelevanceAdvisorySwitch()) {
		D.Out(pdExcept, "Federate %d already set CRA switch.", federate_handle);
		throw RTIinternalError("CRA switch already enabled.");
	}

	federate.setClassRelevanceAdvisorySwitch(true);
	D.Out(pdTerm, "Federation %d: Federate %d sets CRA switch.",
			handle, federate_handle);
		}

// ----------------------------------------------------------------------------
// Set Interaction Relevance Advisory Switch
void
Federation::setInteractionRelevanceAdvisorySwitch(FederateHandle federate_handle)
throw (FederateNotExecutionMember,
		SaveInProgress,
		RestoreInProgress,
		RTIinternalError)
		{
	// It may throw FederateNotExecutionMember
	Federate &federate = getFederate(federate_handle);

	if (federate.isInteractionRelevanceAdvisorySwitch()) {
		D.Out(pdExcept, "Federate %d already set IRA switch.", federate_handle);
		throw RTIinternalError("IRA switch already enabled.");
	}

	federate.setInteractionRelevanceAdvisorySwitch(true);
	D.Out(pdTerm, "Federation %d: Federate %d sets IRA switch.",
			handle, federate_handle);
		}

// ----------------------------------------------------------------------------
// Set Attribute Relevance Advisory Switch
void
Federation::setAttributeRelevanceAdvisorySwitch(FederateHandle federate_handle)
throw (FederateNotExecutionMember,
		SaveInProgress,
		RestoreInProgress,
		RTIinternalError)
		{
	// It may throw FederateNotExecutionMember
	Federate &federate = getFederate(federate_handle);

	if (federate.isAttributeRelevanceAdvisorySwitch()) {
		D.Out(pdExcept, "Federate %d already set ARA switch.", federate_handle);
		throw RTIinternalError("ARA switch already enabled.");
	}

	federate.setAttributeRelevanceAdvisorySwitch(true);
	D.Out(pdTerm, "Federation %d: Federate %d sets ARA switch.",
			handle, federate_handle);
		}

// ----------------------------------------------------------------------------
// Set Attribute Scope Advisory Switch
void
Federation::setAttributeScopeAdvisorySwitch(FederateHandle federate_handle)
throw (FederateNotExecutionMember,
		SaveInProgress,
		RestoreInProgress,
		RTIinternalError)
		{
	// It may throw FederateNotExecutionMember
	Federate &federate = getFederate(federate_handle);

	if (federate.isAttributeScopeAdvisorySwitch()) {
		D.Out(pdExcept, "Federate %d already set ASA switch.", federate_handle);
		throw RTIinternalError("ASA switch already enabled.");
	}

	federate.setAttributeScopeAdvisorySwitch(true);
	D.Out(pdTerm, "Federation %d: Federate %d sets ASA switch.",
			handle, federate_handle);
		}

// ----------------------------------------------------------------------------
// Clear Class Relevance Advisory Switch
void
Federation::unsetClassRelevanceAdvisorySwitch(FederateHandle federate_handle)
throw (FederateNotExecutionMember,
		SaveInProgress,
		RestoreInProgress,
		RTIinternalError)
		{
	// It may throw FederateNotExecutionMember
	Federate &federate = getFederate(federate_handle);

	if (!federate.isClassRelevanceAdvisorySwitch()) {
		D.Out(pdExcept, "Federate %d did not set CRA switch.", federate_handle);
		throw RTIinternalError("CRA switch not enabled.");
	}

	federate.setClassRelevanceAdvisorySwitch(false);
	D.Out(pdTerm, "Federation %d: Federate %d clears CRA switch.",
			handle, federate_handle);
		}

// ----------------------------------------------------------------------------
// Clear Interaction Relevance Advisory Switch
void
Federation::unsetInteractionRelevanceAdvisorySwitch(FederateHandle federate_handle)
throw (FederateNotExecutionMember,
		SaveInProgress,
		RestoreInProgress,
		RTIinternalError)
		{
	// It may throw FederateNotExecutionMember
	Federate &federate = getFederate(federate_handle);

	if (!federate.isInteractionRelevanceAdvisorySwitch()) {
		D.Out(pdExcept, "Federate %d did not set IRA switch.", federate_handle);
		throw RTIinternalError("IRA switch not enabled.");
	}

	federate.setInteractionRelevanceAdvisorySwitch(false);
	D.Out(pdTerm, "Federation %d: Federate %d clears IRA switch.",
			handle, federate_handle);
		}

// ----------------------------------------------------------------------------
// Clear Attribute Relevance Advisory Switch
void
Federation::unsetAttributeRelevanceAdvisorySwitch(FederateHandle federate_handle)
throw (FederateNotExecutionMember,
		SaveInProgress,
		RestoreInProgress,
		RTIinternalError)
		{
	// It may throw FederateNotExecutionMember
	Federate &federate = getFederate(federate_handle);

	if (!federate.isAttributeRelevanceAdvisorySwitch()) {
		D.Out(pdExcept, "Federate %d did not set ARA switch.", federate_handle);
		throw RTIinternalError("ARA switch not enabled.");
	}

	federate.setAttributeRelevanceAdvisorySwitch(false);
	D.Out(pdTerm, "Federation %d: Federate %d clears ARA switch.",
			handle, federate_handle);
		}

// ----------------------------------------------------------------------------
//  Clear Attribute Scope Advisory Switch
void
Federation::unsetAttributeScopeAdvisorySwitch(FederateHandle federate_handle)
throw (FederateNotExecutionMember,
		SaveInProgress,
		RestoreInProgress,
		RTIinternalError)
		{
	// It may throw FederateNotExecutionMember
	Federate &federate = getFederate(federate_handle);

	if (!federate.isAttributeScopeAdvisorySwitch()) {
		D.Out(pdExcept, "Federate %d did not set ASA switch.", federate_handle);
		throw RTIinternalError("ASA switch not enabled.");
	}

	federate.setAttributeScopeAdvisorySwitch(false);
	D.Out(pdTerm, "Federation %d: Federate %d clears ASA switch.",
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
			handle, federate_handle, time.getTime());

	NM_Set_Time_Regulating msg ;
	msg.setException(e_NO_EXCEPTION);
	msg.setFederation(handle);
	msg.setFederate(federate_handle);
	msg.regulatorOn();
	msg.setDate(time);

	this->broadcastAnyMessage(&msg, 0,false);
}

void
Federation::getFOM(NM_Join_Federation_Execution& objectModelData)
{
	root->convertToSerializedFOM(objectModelData);
}

bool
Federation::updateLastNERxForFederate(FederateHandle federate, FederationTime date)
throw (FederateNotExecutionMember) {
	bool retval = false;
	FederationTime newMin;
	Federate& f = getFederate(federate);

	f.setLastNERxValue(date);
	Debug(D,pdDebug) << "Federate <"<<f.getName()<<"> has new NERx value="<<date.getTime() << std::endl;
	newMin = computeMinNERx();
	if (newMin > minNERx) {
		Debug(D,pdDebug) << "New minNERx =" << newMin << std::endl;
		retval = true;
		minNERx = newMin;
		/* if a new Min is found then we shall reset the NERx status of ALL NERing federates
		 * Rationale: an Anonymous NULL MESSAGE dated with newMin will be sent to them.
		 *            after that a NERing Federate which remains in its NERing loop should
		 *            send a new NULL PRIME message.
		 */
		for (HandleFederateMap::iterator i = _handleFederateMap.begin(); i != _handleFederateMap.end(); ++i) {
			if (i->second.isUsingNERx()) {
				//i->second.setLastNERxValue(FedTime(0.0)); // not needed
				i->second.setIsUsingNERx(false);
				Debug(D,pdDebug) << "Federate <" << i->second.getName() <<"> not NERing anymore." <<  std::endl;
			}
		}
	}
	return retval;
} /* end of updateLastNERxForFederate */

FederationTime
Federation::computeMinNERx() {
	FederationTime retval;
	uint32_t       nbFed;
	LBTS           NER_regulators;
	std::vector<LBTS::FederateClock> clocks;
	retval.setZero();
	nbFed = 0;
	regulators.get(clocks);

	/* Build a set of clocks */
	for (std::vector<LBTS::FederateClock>::iterator it = clocks.begin(); it!=clocks.end(); ++it) {
		FederateHandle h=it->first;
		HandleFederateMap::iterator f = _handleFederateMap.find(h);
		if (f->second.isUsingNERx()) {
			nbFed++;
			NER_regulators.insert(h,f->second.getLastNERxValue());
		} else {
			NER_regulators.insert(h,it->second);
		}
	}

	/* compute the new NERx LBTS (minimum) */
	NER_regulators.compute();
	retval = NER_regulators.getLBTSValue();
	Debug(D,pdDebug) << "MinNERx =" << retval.getTime() << std::endl;

	/* the minimum is different from 0 iff more than 2 federate use NERx */
	if (nbFed<2) {
		retval.setZero();
	}

	Debug(D,pdDebug) << "computeMinNERx ="<<retval.getTime() << std::endl;
	return retval;
} /* end of computeMinNERx */

void
Federation::broadcastAnyMessage(NetworkMessage *msg,
		FederateHandle except_federate, bool anonymous)
{
	Socket *socket = NULL ;

	// Broadcast the message 'msg' to all Federates in the Federation
	// except to Federate whose Handle is 'Except_Federate'.
	for (HandleFederateMap::iterator i = _handleFederateMap.begin(); i != _handleFederateMap.end(); ++i) {
		if (anonymous || (i->first != except_federate)) {
			try {
#ifdef HLA_USES_UDP
				socket = server->getSocketLink(i->second.getHandle(), BEST_EFFORT);
#else
				socket = server->getSocketLink(i->second.getHandle());
#endif
				msg->send(socket,NM_msgBufSend);
			}
			catch (RTIinternalError &e) {
				Debug(D, pdExcept) << "Reference to a killed Federate while "
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
		const std::vector <FederateHandle> &fede_array,
		uint32_t nbfed)
{
	uint32_t ifed ;
	Socket *socket = NULL ;

	if ( fede_array.size() != 0 || nbfed == 0)
	{
		// Broadcast the message 'msg' to some Federates (done in fede_array)
		// in the Federation
		// except to Federate whose Handle is 'Except_Federate'.
		for (HandleFederateMap::iterator i = _handleFederateMap.begin(); i != _handleFederateMap.end(); ++i)
		{
			if (i->second.getHandle() != except_federate)
			{
				ifed = 0 ;
				while ( ifed < nbfed )
				{
					if ( i->second.getHandle() == fede_array[ifed] )
						// Federate i has to be informed because into fede_array
						{
						try
						{
#ifdef HLA_USES_UDP
							socket = server->getSocketLink(i->second.getHandle(), BEST_EFFORT);
#else
							socket = server->getSocketLink(i->second.getHandle());
#endif
							msg->send(socket,NM_msgBufSend);
						}
						catch (RTIinternalError &e)
						{
							Debug(D, pdExcept) << "Reference to a killed Federate while "
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
        const std::vector <ParameterHandle> &parameter_handles,
        const std::vector <ParameterValue_t> &parameter_values,
        uint16_t list_size,
        FederationTime time,
        RegionHandle region_handle,
        const std::string& tag)
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

    G.Out(pdGendoc,"exit Federation::broadcastInteraction with time");

        }

// ----------------------------------------------------------------------------
//! broadcastInteraction without time
void
Federation::broadcastInteraction(FederateHandle federate_handle,
        InteractionClassHandle interaction,
        const std::vector <ParameterHandle> &parameter_handles,
        const std::vector <ParameterValue_t> &parameter_values,
        uint16_t list_size,
        RegionHandle region_handle,
        const std::string& tag)
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
                string(&(parameter_values[i][0]), parameter_values[i].size()).c_str());

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
		const std::string& tag)
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
		const std::string& tag)
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
		const std::string& label,
		const std::string& tag)
throw (FederateNotExecutionMember,
		FederationAlreadyPaused,
		SaveInProgress,
		RestoreInProgress,
		RTIinternalError)
		{

	G.Out(pdGendoc,"enter Federation::registerSynchronization for all federates");

	this->check(federate); // It may throw FederateNotExecutionMember.

	if (label.empty())
		throw RTIinternalError("Bad pause label(null).");

	// Verify label does not already exists
	std::map<std::string, std::string>::const_iterator i = synchronizationLabels.find(label);
	if (i != synchronizationLabels.end()) {
		throw FederationAlreadyPaused("Label already pending"); // Label already pending.
	}

	// If not already in pending labels, insert to list.
	synchronizationLabels.insert(pair<const std::string, std::string>(label, tag));

	// Add label to each federate (may throw RTIinternalError).
	for (HandleFederateMap::iterator i = _handleFederateMap.begin(); i != _handleFederateMap.end(); ++i) {
		i->second.addSynchronizationLabel(label);
	}

	Debug(D, pdTerm) << "Federation " << handle << " is now synchronizing for label "
			<< label << endl ;

	G.Out(pdGendoc,"exit  Federation::registerSynchronization for all federates");
		} /* end of Federation::registerSynchronization */

// ----------------------------------------------------------------------------
//! Add a new synchronization point (with federates set) to federation.
void
Federation::registerSynchronization(FederateHandle federate,
		const std::string& label,
		const std::string& tag,
		unsigned short federate_setSize,
		const std::vector <FederateHandle> &federate_set)
throw (FederateNotExecutionMember,
		FederationAlreadyPaused,
		SaveInProgress,
		RestoreInProgress,
		RTIinternalError)
		{

	G.Out(pdGendoc,"enter Federation::registerSynchronization for federate set");

	this->check(federate); // It may throw FederateNotExecutionMember.

	if (label.empty())
		throw RTIinternalError("Bad pause label(null).");

	// Verify label does not already exists
	std::map<std::string, std::string>::const_iterator i = synchronizationLabels.find(label);
	if (i != synchronizationLabels.end()) {
		throw FederationAlreadyPaused(""); // Label already pending.
	}

	// If not already in pending labels, insert to list.
	synchronizationLabels.insert(pair<const std::string, std::string>(label, tag));

	// Add label to each federate into the set only (may throw RTIinternalError).
	for (int i=0 ; i < federate_setSize  ;i++ )
	{
		for (HandleFederateMap::iterator j = _handleFederateMap.begin(); j != _handleFederateMap.end(); ++j)
		{
			if ( (federate_set[i] == j->second.getHandle()) || (federate == j->second.getHandle()) )
				j->second.addSynchronizationLabel(label);
		}
	}

	Debug(D, pdTerm) << "Federation " << handle << " is now synchronizing for label "
			<< label << endl ;

	G.Out(pdGendoc,"exit  Federation::registerSynchronization for federate set");

		}

// ----------------------------------------------------------------------------
/*! Broadcast an 'Announce Synchronization Point' when registering a new
  synchronization point.
 */
void
Federation::broadcastSynchronization(FederateHandle federate,
		const std::string& label,
		const std::string& tag)
throw (RTIinternalError)
{

	G.Out(pdGendoc,"enter Federation::broadcastSynchronization");

	this->check(federate); // It may throw FederateNotExecutionMember.

	if (label.empty())
		throw RTIinternalError("Bad pause label(null).");

	// broadcast announceSynchronizationPoint() to all federates in federation.
	NM_Announce_Synchronization_Point msg ;
	msg.setFederate(federate);
	msg.setFederation(handle);
	msg.setLabel(label);
	msg.setTag(tag);

	G.Out(pdGendoc,"      broadcastSynchronization is calling broadcastAnyMessage for all federates");

	broadcastAnyMessage(&msg, 0,false);

	G.Out(pdGendoc,"exit  Federation::broadcastSynchronization");

}

// ----------------------------------------------------------------------------
/*! Broadcast an 'Announce Synchronization Point' when registering a new
  synchronization point onto a set of federates
 */
void
Federation::broadcastSynchronization(FederateHandle federate,
		const std::string& label,
		const std::string& tag,
		unsigned short federate_setSize,
		const std::vector <FederateHandle> &federate_set)
throw (RTIinternalError)
{

	G.Out(pdGendoc,"enter Federation::broadcastSynchronization to some federates");

	this->check(federate); // It may throw FederateNotExecutionMember.

	if (label.empty())
		throw RTIinternalError("Bad pause label(null or too long).");

	// broadcast announceSynchronizationPoint() to all federates in federation.
	NM_Announce_Synchronization_Point msg ;
	msg.setFederate(federate) ;
	msg.setFederation(handle);
	msg.setLabel(label);
	msg.setTag(tag);

	G.Out(pdGendoc,"      broadcastSynchronization is calling broadcastSomeMessage");

	broadcastSomeMessage(&msg, 0, federate_set, federate_setSize);

	G.Out(pdGendoc,"exit  Federation::broadcastSynchronization to some federates");

}


// ----------------------------------------------------------------------------
//! Request a federation save with time.
/*! This service puts each federate from federation in saving state.
  !! the_time is not managed yet.
 */
void
Federation::requestFederationSave(FederateHandle the_federate,
		const std::string& the_label,
		FederationTime time )
throw (FederateNotExecutionMember, SaveInProgress)
{
	G.Out(pdGendoc,"enter Federation::requestFederationSave with time");

	check(the_federate);

	if (saveInProgress)
		throw SaveInProgress("Already in saving state.");

	for (HandleFederateMap::iterator j = _handleFederateMap.begin(); j != _handleFederateMap.end(); ++j) {
		j->second.setSaving(true);
	}

	saveStatus = true ;
	saveInProgress = true ;
	saveLabel = the_label ;

	NM_Initiate_Federate_Save msg ;
	msg.setFederate(the_federate);
	msg.setFederation(handle);
	msg.setLabel(the_label);
	// timed message
	msg.setDate(time);

	G.Out(pdGendoc,"      requestFederationSave====>broadcast I_F_S to all");

	broadcastAnyMessage(&msg, 0, false);

	G.Out(pdGendoc,"exit  Federation::requestFederationSave with time");
}

// ----------------------------------------------------------------------------
//! Request a federation save without time.
/*! This service puts each federate from federation in saving state.
  !! the_time is not managed yet.
 */
void
Federation::requestFederationSave(FederateHandle the_federate,
		const std::string& the_label)
throw (FederateNotExecutionMember, SaveInProgress)
{
	G.Out(pdGendoc,"enter Federation::requestFederationSave without time");

	check(the_federate);

	if (saveInProgress)
		throw SaveInProgress("Already in saving state.");

	for (HandleFederateMap::iterator j = _handleFederateMap.begin(); j != _handleFederateMap.end(); ++j) {
		j->second.setSaving(true);
	}

	saveStatus = true ;
	saveInProgress = true ;
	saveLabel = the_label ;

	NM_Initiate_Federate_Save msg ;
	msg.setFederate(the_federate);
	msg.setFederation(handle);
	msg.setLabel(the_label);

	G.Out(pdGendoc,"                  requestFederationSave====>broadcast I_F_S"
			" to all");

	broadcastAnyMessage(&msg, 0, false);

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
	for (HandleFederateMap::iterator j = _handleFederateMap.begin(); j != _handleFederateMap.end(); ++j) {
		if (j->second.isSaving())
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

	msg->setFederate(the_federate);
	msg->setFederation(handle);

	broadcastAnyMessage(msg.get(), 0,false);

	G.Out(pdGendoc,"            =======> broadcast F_S or F_N_S");

	// Reinitialize state.
	saveStatus = true ;
	saveInProgress = false ;

	G.Out(pdGendoc,"exit  Federation::federateSaveStatus");
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//! Informs that a federate is requesting a save.
void
Federation::requestFederationRestore(FederateHandle the_federate,
		const std::string& the_label)
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
	string filename = name + "_" + the_label + ".xcs" ;
	success = restoreXmlData(filename);
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

	msg->setFederate(the_federate);
	msg->setFederation(handle);
	msg->setLabel(the_label);

	socket = server->getSocketLink(msg->getFederate());

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
	for (HandleFederateMap::iterator i = _handleFederateMap.begin(); i != _handleFederateMap.end(); ++i) {
		i->second.setRestoring(true);
	}
	restoreStatus = true ;
	restoreInProgress = true ;

	// Informs federates a new restore is being done.
	msg = NM_Factory::create(NetworkMessage::FEDERATION_RESTORE_BEGUN);
	msg->setFederate(the_federate);
	msg->setFederation(handle);

	G.Out(pdGendoc,"             =====> broadcast message F_R_B");

	broadcastAnyMessage(msg, 0, false);
	delete msg ;

	// For each federate, send an initiateFederateRestore with correct handle.
	msg = NM_Factory::create(NetworkMessage::INITIATE_FEDERATE_RESTORE);
	msg->setFederation(handle) ;
	msg->setLabel(the_label);

	for (HandleFederateMap::iterator i = _handleFederateMap.begin(); i != _handleFederateMap.end(); ++i) {
		msg->setFederate(i->second.getHandle());

		// send message.
		socket = server->getSocketLink(msg->getFederate());
		G.Out(pdGendoc,"             =====> send message I_F_R to federate %d",msg->getFederate());
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
	for (HandleFederateMap::iterator i = _handleFederateMap.begin(); i != _handleFederateMap.end(); ++i) {
		if (i->second.isRestoring())
			return ;
	}

	// Send end restore message.
	std::auto_ptr<NetworkMessage> msg(NM_Factory::create(restoreStatus ? NetworkMessage::FEDERATION_RESTORED : NetworkMessage::FEDERATION_NOT_RESTORED)) ;

	msg->setFederate(the_federate);
	msg->setFederation(handle);

	broadcastAnyMessage(msg.get(), 0, false);

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
	HandleFederateMap::iterator i = _handleFederateMap.find(federate_handle);
	if (i == _handleFederateMap.end())
		throw FederateNotExecutionMember(certi::stringize() << "Federate Handle <"<< federate_handle << "> not found.");
	return i->second;
}

// ----------------------------------------------------------------------------
//! Return the Federate whose Name is theName, if found.
Federate &
Federation::getFederate(const std::string& federate_name)
throw (FederateNotExecutionMember)
{
	for (HandleFederateMap::iterator i = _handleFederateMap.begin(); i != _handleFederateMap.end(); ++i) {
		if (i->second.getName() == federate_name)
			return i->second;
	}

	throw FederateNotExecutionMember(stringize() <<
			"Federate <"<<federate_name<<"> not [yet] member of Federation <" << getName() <<">.");
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
	if (_handleFederateMap.empty()) {
		return true ;
	}
	else {
	    // build the list of name of the federate currently joined
	    HandleFederateMap::const_iterator it;
	    std::stringstream msg;
	    msg << "<";
	    for (it = _handleFederateMap.begin(); it != _handleFederateMap.end(); it++) {
	        msg << " " << (it->second).getName();
	    }
	    msg << " >";
		throw FederatesCurrentlyJoined(msg.str().c_str());
	}
}

// ----------------------------------------------------------------------------
// Return true if federate is in federation.

bool
Federation::check(FederateHandle federate_handle) const
throw (FederateNotExecutionMember)
{
    HandleFederateMap::const_iterator i = _handleFederateMap.find(federate_handle);
    if (i == _handleFederateMap.end()) {
        throw FederateNotExecutionMember(certi::stringize() << "Federate Handle <"<< federate_handle << "> not found in federation <" << handle);
    }
    return true;
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
	Debug(D, pdTrace) << "Federate " << federate << " removed from the Root Object "
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
                          const std::vector <AttributeHandle> &attributes,
                          bool pub)
throw (ObjectClassNotDefined,
        AttributeNotDefined,
        FederateNotExecutionMember,
        SaveInProgress,
        SecurityError,
        RestoreInProgress,
        RTIinternalError) {
    G.Out(pdGendoc,"enter Federation::publishObject");
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined*
    root->ObjectClasses->publish(federate, object, attributes, pub);

    if (attributes.size()!=0) {  // do only for subscription
        // get object class from object class handle
        ObjectClass *objectClass = root->ObjectClasses->getObjectFromHandle(object);

        // get publishers of attributes
        ObjectClassAttribute::PublishersList_t publishers;
        ObjectClassAttribute::PublishersList_t tmp_publishers;

        // first for: iterate through the attribute list and get publishers of
        //            each attribute
        // second for: iterate through the temporary publishers list and store
        //             non-duplicate entries in publishers

        // get attributes of object class
       const ObjectClass::HandleClassAttributeMap& attributeMap = objectClass->getHandleClassAttributeMap();

       for (ObjectClass::HandleClassAttributeMap::const_iterator i = attributeMap.begin();
                i != attributeMap.end(); ++i) {
            tmp_publishers = i->second->getPublishers();
            for (ObjectClassAttribute::PublishersList_t::const_iterator
                    j=tmp_publishers.begin();
                    j!=tmp_publishers.end();
                    j++) {
                // insert only non-duplicate entries ->
                // pair<iterator, bool> set::insert(const TYPE& val);
                publishers.insert(*j);
            }
            tmp_publishers.clear();
        }

        // notify all publishers
        std::set<FederateHandle> federate_set;
        for (ObjectClassAttribute::PublishersList_t::const_iterator
                k=publishers.begin();
                k!=publishers.end();
                k++) {
            if (getFederate(*k).isClassRelevanceAdvisorySwitch()) {
                federate_set.insert(*k);
            }
        }

        // broadcastSomeMessage needs a vector, no set -> conversion
        vector<FederateHandle> federate_vector(federate_set.size());
        std::copy(federate_set.begin(), federate_set.end(), federate_vector.begin());

        NM_Start_Registration_For_Object_Class msg ;
        msg.setFederate(federate);
        msg.setFederation(handle);
        msg.setObjectClass(object);

        this->broadcastSomeMessage(&msg, 0, federate_vector, (unsigned short)federate_vector.size());

        publishers.clear();
        federate_set.clear();
        federate_vector.clear();
    }
    else { // unsubscribe branch
        // test if objectClass is subscribed by anyone else
        // -> yes : do nothing
        // -> no : test if publisher sets its CRA switch
        //    -> no : do nothing
        //    -> yes : inform publisher with federate service stopRegistrationForObjectClass
        //

    }

    
    D.Out(pdRegister,
            "Federation %d: Federate %d(un)publishes %d attrib. of ObjClass %d.",
            handle, federate, attributes.size(), object);
    G.Out(pdGendoc,"exit  Federation::publishObject");
}


void 
Federation::reserveObjectInstanceName(FederateHandle theFederateHandle,
	                           std::string newObjName)
	throw (IllegalName,
	       SaveInProgress,
	       RestoreInProgress,
	       RTIinternalError)
{
	Socket * socket ;
	NetworkMessage *msg;

	bool reservation_ok = false;

	reservation_ok = root->reserveObjectInstanceName(theFederateHandle, newObjName);

	if (reservation_ok)
	{
		msg = NM_Factory::create(NetworkMessage::RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED);
		NM_Reserve_Object_Instance_Name_Succeeded *okMsg = dynamic_cast<NM_Reserve_Object_Instance_Name_Succeeded *>(msg);

		okMsg->setObjectName(newObjName);
		G.Out(pdGendoc,"             =====> send message R_O_I_N_S to federate %d",msg->getFederate());
	} else {
		msg = NM_Factory::create(NetworkMessage::RESERVE_OBJECT_INSTANCE_NAME_FAILED);
		NM_Reserve_Object_Instance_Name_Failed *nokMsg = dynamic_cast<NM_Reserve_Object_Instance_Name_Failed *>(msg);

		nokMsg->setObjectName(newObjName);
		G.Out(pdGendoc,"             =====> send message R_O_I_N_F to federate %d",msg->getFederate());
	}

	msg->setFederation(handle);
	msg->setFederate(theFederateHandle);
	// send message.
	socket = server->getSocketLink(msg->getFederate());
	msg->send(socket,NM_msgBufSend);

	delete msg;
}


// ----------------------------------------------------------------------------
//! Adds a new object instance to federation.
ObjectHandle
Federation::registerObject(FederateHandle federate,
		ObjectClassHandle class_handle,
		const std::string& object_name)
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

	string strname;
	if (!object_name.empty()) {
		strname = object_name;
	} else {
		// create a name if necessary
		strname = stringize() << "HLAObject_" << new_id;
	}

	// Register Object.
	try
	{
		root->registerObjectInstance(federate, class_handle, new_id, strname);
	}
	catch(...)
	{   //If an exception was thrown, the object instance was not added
		//and we can free the object handle id for future use and rethrow
		//the exception
		objectHandles.free(new_id);
		throw;
	}
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
	HandleFederateMap::iterator i = _handleFederateMap.find(federate_handle);
	if (i != _handleFederateMap.end()) {
		// BUG: RemoveFederate: Should see if Federate owns attributes
		federateHandles.free(federate_handle);
		_handleFederateMap.erase(i);

		D.Out(pdInit, "Federation %d: Removed Federate %d.", handle,
				federate_handle);
		return;
	}

	D.Out(pdExcept, "Federation %d could not remove unknown federate %d.",
			handle, federate_handle);
	throw FederateNotExecutionMember(certi::stringize() << "Federate Handle=<"<<federate_handle <<">");
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
	msg.setFederation(handle);
	msg.setFederate(federate_handle);
	msg.regulatorOff();

	broadcastAnyMessage(&msg, 0, false);
		}

// ----------------------------------------------------------------------------
//! unregisterSynchronization.
void
Federation::unregisterSynchronization(FederateHandle federate_handle,
		const std::string& label)
throw (FederateNotExecutionMember,
		FederationNotPaused,
		SaveInProgress,
		RestoreInProgress,
		RTIinternalError)
		{

	G.Out(pdGendoc,"enter Federation::unregisterSynchronization");

	this->check(federate_handle); // It may throw FederateNotExecutionMember.

	if (label.empty())
		throw RTIinternalError("Bad pause label(null).");

	// Set federate synchronized on this label.
	Federate &federate = getFederate(federate_handle);
	federate.removeSynchronizationLabel(label);

	// Test in every federate is synchronized. Otherwise, quit method.
	for (HandleFederateMap::iterator i = _handleFederateMap.begin(); i != _handleFederateMap.end(); ++i) {
		if (i->second.isSynchronizationLabel(label))
			return;
	}

	// All federates from federation has called synchronizationPointAchieved.

	D.Out(pdTerm, "Federation %d is not Paused anymore.", handle);
	// Remove label from federation list.
	std::map<std::string, std::string>::iterator i = synchronizationLabels.find(label);
	if (i != synchronizationLabels.end()) {
		synchronizationLabels.erase(i);
	}

	// send a federationSynchronized().
	NM_Federation_Synchronized msg ;
	msg.setFederation(handle);
	msg.setFederate(federate_handle);
	msg.setLabel(label);

	broadcastAnyMessage(&msg, 0, false);

	D.Out(pdTerm, "Federation %d is synchronized on %s.", handle, label.c_str());

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
                            const std::vector <AttributeHandle> &attributes)
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

    /*
     * The subscription process in CERTI:
     * In RTIG.cc the network messages SUBSCRIBE_OBJECT_CLASS and UNSUBSCRIBE_
     * OBJECT_CLASS are both mapped to the method processSubscribeObject within
     * RTIG_processing.cc. RTIG_proccessing invokes this method
     * (subscribeObject).
     */

    // It may throw AttributeNotDefined
    root->ObjectClasses->subscribe(federate, object, attributes);

    /*
     * The above code line (root->ObjectClasses->subscribe(...) calls the
     * subscription within the CERTI library in ObjectClassSet.cc. Further on,
     * ObjectClassSet::subscribe invokes ObjectClass::subscribe. That method
     * first unsubscribes all attributes, afterwards the subscription is
     * done in a for loop till list_size.
     * This means: Unsubscription and subscription are realized by the same
     * method. Only the list_size parameter controls the
     * unsubscription/subscription process.
     *
     * Do we need a cleaner solution, e.g. separate methods for subscription/
     * unsubscription?
     */

    if (attributes.size()!=0) {  // do only for subscription
        // get object class from object class handle
        ObjectClass *objectClass = root->ObjectClasses->getObjectFromHandle(object);

        // get publishers of attributes
        ObjectClassAttribute::PublishersList_t publishers;
        ObjectClassAttribute::PublishersList_t tmp_publishers;

        // first for: iterate through the attribute list and get publishers of
        //            each attribute
        // second for: iterate through the temporary publishers list and store
        //             non-duplicate entries in publishers

        // get attributes of object class
       const ObjectClass::HandleClassAttributeMap& attributeMap = objectClass->getHandleClassAttributeMap();

       for (ObjectClass::HandleClassAttributeMap::const_iterator i = attributeMap.begin();
                i != attributeMap.end(); ++i) {
            tmp_publishers = i->second->getPublishers();
            for (ObjectClassAttribute::PublishersList_t::const_iterator
                    j=tmp_publishers.begin();
                    j!=tmp_publishers.end();
                    j++) {
                // insert only non-duplicate entries ->
                // pair<iterator, bool> set::insert(const TYPE& val);
                publishers.insert(*j);
            }
            tmp_publishers.clear();
        }

        // notify all publishers
        std::set<FederateHandle> federate_set;
        for (ObjectClassAttribute::PublishersList_t::const_iterator
                k=publishers.begin();
                k!=publishers.end();
                k++) {
            if (getFederate(*k).isClassRelevanceAdvisorySwitch()) {
                federate_set.insert(*k);
            }
        }

        // broadcastSomeMessage needs a vector, no set -> conversion
        vector<FederateHandle> federate_vector(federate_set.size());
        std::copy(federate_set.begin(), federate_set.end(), federate_vector.begin());

        NM_Start_Registration_For_Object_Class msg ;
        msg.setFederate(federate);
        msg.setFederation(handle);
        msg.setObjectClass(object);

        this->broadcastSomeMessage(&msg, 0, federate_vector, (unsigned short)federate_vector.size());

        publishers.clear();
        federate_set.clear();
        federate_vector.clear();
    }
    else { // unsubscribe branch
        /* test if objectClass is subscribed by anyone else
         * -> yes : do nothing
         * -> no : test if publisher sets its CRA switch
         *  	-> no : do nothing
         *  	-> yes : inform publisher with federate service stopRegistrationForObjectClass
         */

    }

    D.Out(pdRegister,
            "Federation %d: Federate %d(un)sub. to %d attrib. of ObjClass %d.",
            handle, federate, attributes.size(), object);
    G.Out(pdGendoc,"exit  Federation::subscribeObject");
  }

// ----------------------------------------------------------------------------
// updateAttributeValues with time

void
Federation::updateAttributeValues(FederateHandle federate,
		ObjectHandle objectHandle,
		const std::vector <AttributeHandle>  &attributes,
		const std::vector <AttributeValue_t> &values,
		uint16_t list_size,
		FederationTime time,
		const std::string& tag)
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

	// Get the object pointer by id from the root object
	Object *object = root->objects->getObject(objectHandle);

	// It may throw *NotDefined
	root->ObjectClasses->updateAttributeValues(federate, object, attributes, values, time, tag);

	D.Out(pdRegister,
			"Federation %d: Federate %d updated attributes of Object %d.",
			handle, federate, objectHandle);
	G.Out(pdGendoc,"exit  Federation::updateAttributeValues with time");
		}

// ----------------------------------------------------------------------------
// updateAttributeValues without time

void
Federation::updateAttributeValues(FederateHandle federate,
		ObjectHandle objectHandle,
		const std::vector <AttributeHandle> &attributes,
		const std::vector <AttributeValue_t> &values,
		uint16_t list_size,
		const std::string& tag)
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

	// Get the object pointer by id from the root object
	Object *object = root->objects->getObject(objectHandle);

	// It may throw *NotDefined
	root->ObjectClasses->updateAttributeValues(federate, object, attributes, values, tag);

	D.Out(pdRegister,
			"Federation %d: Federate %d updated attributes of Object %d.",
			handle, federate, objectHandle);
	G.Out(pdGendoc,"exit  Federation::updateAttributeValues without time");
		}
// ----------------------------------------------------------------------------
//! Update the current time of a regulator federate.
void
Federation::updateRegulator(FederateHandle federate_handle,
		FederationTime time, bool anonymous)
throw (FederateNotExecutionMember,
		RTIinternalError)
		{

	/* if it is an anonymous update (from NULL PRIME message)
	 * no need to check federate.
	 */
	if (!anonymous) {
		// It may throw FederateNotExecutionMember
		Federate &federate = getFederate(federate_handle);

		if (!federate.isRegulator()) {
			D.Out(pdExcept, "Federate %d is not a regulator.", federate_handle);
			throw RTIinternalError("Time regulation not enabled.");
		}

		D.Out(pdDebug, "Federation %d: Federate %d's new time is %f.",
				handle, federate_handle, time.getTime());
		regulators.update(federate_handle, time);
	}

	NM_Message_Null msg ;
	msg.setFederation(handle);
	if (anonymous) {
		msg.setFederate(0);
	} else {
		msg.setFederate(federate_handle);
	}
	msg.setDate(time);
	DNULL.Out(pdDebug,"Snd NULL MSG (Federate=%d, Time = %f)",
	                    msg.getFederate(), msg.getDate().getTime());
	broadcastAnyMessage(&msg, federate_handle, anonymous);
} /* end of updateRegulator */

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
		ObjectHandle objectHandle,
		const std::vector <AttributeHandle> &attribs,
		uint16_t list_size,
		const std::string& tag)
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

	// Get the object pointer by id from the root object
	Object *object = root->objects->getObject(objectHandle);

	// It may throw *NotDefined
	root->ObjectClasses->negotiatedAttributeOwnershipDivestiture(federate, object, attribs, tag);
		}

// ----------------------------------------------------------------------------
// acquireIfAvailable (attributeOwnershipAcquisitionIfAvailable)

void
Federation::acquireIfAvailable(FederateHandle federate,
		ObjectHandle objectHandle,
		const std::vector <AttributeHandle> &attribs,
		uint16_t list_size)
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

	// Get the object pointer by id from the root object
	Object *object = root->objects->getObject(objectHandle);

	// It may throw *NotDefined
	root->ObjectClasses->attributeOwnershipAcquisitionIfAvailable(federate, object, attribs);
		}

// ----------------------------------------------------------------------------
// divest (unconditionalAttributeOwnershipDivestiture)

void
Federation::divest(FederateHandle federate,
		ObjectHandle objectHandle,
		const std::vector <AttributeHandle> &attrs,
		uint16_t list_size)
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

	// Get the object pointer by id from the root object
	Object *object = root->objects->getObject(objectHandle);

	// It may throw *NotDefined
	root->ObjectClasses->unconditionalAttributeOwnershipDivestiture(federate, object, attrs);
		}

// ----------------------------------------------------------------------------
// attributeOwnershipAcquisition

void
Federation::acquire(FederateHandle federate,
		ObjectHandle objectHandle,
		const std::vector <AttributeHandle> &attributes,
		uint16_t list_size,
		const std::string& tag)
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

	// Get the object pointer by id from the root object
	Object *object = root->objects->getObject(objectHandle);

	// It may throw *NotDefined
	root->ObjectClasses->attributeOwnershipAcquisition(federate, object, attributes, tag);

	D.Out(pdDebug, "Acquisition on Object %u ", objectHandle);
		}

// ----------------------------------------------------------------------------
// cancelDivestiture (cancelNegotiatedAttributeOwnershipDivestiture)

void
Federation::cancelDivestiture(FederateHandle federate,
		ObjectHandle id,
		const std::vector <AttributeHandle> &attributes,
		uint16_t list_size)
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
		ObjectHandle objectHandle,
		const std::vector <AttributeHandle> &attributes,
		uint16_t list_size)
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

	D.Out(pdDebug, "RespondRelease on Object %u.", objectHandle);

	// Get the object pointer by id from the root object
	Object *object = root->objects->getObject(objectHandle);

	// It may throw *NotDefined
	return root->ObjectClasses->attributeOwnershipReleaseResponse(federate, object, attributes);
		}

// ----------------------------------------------------------------------------
// cancelAttributeOwnershipAcquisition

void
Federation::cancelAcquisition(FederateHandle federate,
		ObjectHandle objectHandle,
		const std::vector <AttributeHandle> &attributes,
		uint16_t list_size)
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

	D.Out(pdDebug, "CancelAcquisition sur Objet %u ", objectHandle);

	// Get the object pointer by id from the root object
	Object *object = root->objects->getObject(objectHandle);

	// It may throw *NotDefined
	root->ObjectClasses->cancelAttributeOwnershipAcquisition(federate, object, attributes);
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
		const std::vector <AttributeHandle> &attributes)
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
		const std::vector <AttributeHandle> &attributes)
throw (RegionNotKnown,
		SaveInProgress,
		RestoreInProgress,
		RTIinternalError)
		{
	check(federate);
	root->ObjectClasses->subscribe(federate, c, attributes, root->getRegion(region_handle));
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
		const std::string& object_name,
		RegionHandle region_handle,
		int nb,
		const std::vector <AttributeHandle> &attributes)
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
	Debug(D, pdDebug) << "Register object with region : Object " << object
			<< ", class " << class_handle << ", region " << region_handle
			<< std::endl ;
	string strname;
	if (!object_name.empty()) {
		strname = object_name;
	} else {
		// create a name if necessary
		strname = stringize() << "HLAObject_" << object;
	}

	root->registerObjectInstance(federate, class_handle, object, strname);

	Debug(D, pdDebug) << "- object \"" << strname
			<< "\" registered" << std::endl ;

	// Associate region
	RTIRegion *region = root->getRegion(region_handle);
	root->getObject(object)->unassociate(region);

	for (int i = 0 ; i < nb ; ++i) {
		root->getObjectAttribute(object, attributes[i])->associate(region);
	}

	Debug(D, pdDebug) << "- " << nb << " attribute(s) associated with region "
			<< region_handle << std::endl ;
	G.Out(pdGendoc,"exit  Federation::registerObjectWithRegion");
	return object ;
		}

// ----------------------------------------------------------------------------
bool
Federation::restoreXmlData(std::string docFilename)
{
#ifndef HAVE_XML
	return false ;
#else

    xmlDocPtr doc = xmlParseFile(docFilename.c_str());

    // Did libXML manage to parse the file ?
    if (doc == 0) {
        cerr << "XML restore file not parsed successfully" << endl ;
        xmlFreeDoc(doc);
        return false ;
    }
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

	bool status = false;
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, NODE_FEDERATE))) {
			for (HandleFederateMap::iterator i = _handleFederateMap.begin(); i != _handleFederateMap.end(); ++i) {
				if (!strcmp(i->second.getName().c_str(),XmlParser::CleanXmlGetProp(cur, (const xmlChar*) "name"))) {
					// Set federate constrained status
					if (!strcmp("true", XmlParser::CleanXmlGetProp(cur, (const xmlChar*) "constrained"))) {
						status = true ;
					}
					else {
						status = false ;
					}

					i->second.setConstrained(status);

					// Set federate regulating status
					status = !strcmp("true", XmlParser::CleanXmlGetProp(cur, (const xmlChar *) "regulator"));

					i->second.setRegulator(status);

					i->second.setHandle(strtol(XmlParser::CleanXmlGetProp(cur, (const xmlChar *) "handle"), 0, 10));
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
	xmlDocPtr doc = xmlNewDoc((const xmlChar *) "1.0");
	doc->children = xmlNewDocNode(doc, NULL, ROOT_NODE, NULL);

	xmlNodePtr federation ;
	federation = xmlNewChild(doc->children, NULL, NODE_FEDERATION, NULL);

	xmlSetProp(federation, (const xmlChar *) "name", (const xmlChar *) name.c_str());

	char t[10] ;
	sprintf(t, "%u", handle);
	xmlSetProp(federation, (const xmlChar *) "handle", (const xmlChar *) t);

	xmlNodePtr federate ;

	for (HandleFederateMap::iterator i = _handleFederateMap.begin(); i != _handleFederateMap.end(); ++i) {
		federate = xmlNewChild(federation, NULL, NODE_FEDERATE, NULL);

		xmlSetProp(federate,
				(const xmlChar *) "name",
				(const xmlChar *) i->second.getName().c_str());

		sprintf(t, "%u", i->second.getHandle());
		xmlSetProp(federate, (const xmlChar *) "handle", (const xmlChar *) t);

		xmlSetProp(federate,
				(const xmlChar *)"constrained", (const xmlChar *)
				((i->second.isConstrained()) ? "true" : "false"));
		xmlSetProp(federate, (const xmlChar *) "regulator",
				(const xmlChar *)((i->second.isRegulator()) ? "true" : "false"));
	}

	xmlSetDocCompressMode(doc, 9);

	string filename = name + "_" + saveLabel + ".xcs" ;
	xmlSaveFile(filename.c_str(), doc);

	// TODO: tests

	return true ;
#endif // HAVE_XML
}

// ----------------------------------------------------------------------------
FederateHandle
Federation::requestObjectOwner(FederateHandle theFederateHandle,
		ObjectHandle theObject,
		const std::vector <AttributeHandle> &theAttributeList,
		uint32_t theListSize)
throw (ObjectNotKnown)
{
	G.Out(pdGendoc,"enter Federation::requestObjectOwner");

    Object* actualObject = root->getObject(theObject);
    
    typedef std::map<FederateHandle, std::vector<AttributeHandle> > ATTRIBUTES_FOR_FEDERATES_T;
    ATTRIBUTES_FOR_FEDERATES_T attributesForFederates;
    
    for (uint32_t i = 0 ; i < theListSize ; ++i)
    {
        FederateHandle federateHandle = actualObject->getAttribute(theAttributeList[i])->getOwner();
        
        // Only attributes that are owned by someone should be asked
        if (federateHandle != 0)
        {
            ATTRIBUTES_FOR_FEDERATES_T::iterator P = attributesForFederates.find(federateHandle);
            if (P == attributesForFederates.end())
                P = attributesForFederates.insert(std::pair<FederateHandle, std::vector<AttributeHandle> >(federateHandle, std::vector<AttributeHandle>())).first;
        
            P->second.push_back(theAttributeList[i]);
        }
    }
    
    for (ATTRIBUTES_FOR_FEDERATES_T::const_iterator P = attributesForFederates.begin() ; P != attributesForFederates.end() ; ++P)
    {
        FederateHandle theOwnerHandle = P->first;
        NM_Provide_Attribute_Value_Update mess ;
        
        // Send a PROVIDE_ATTRIBUTE_VALUE_UPDATE to the owner
        mess.setFederate(theFederateHandle);
        mess.setObject(theObject);
        mess.setAttributesSize(P->second.size()) ;
        for (uint32_t i = 0 ; i < P->second.size() ; ++i)
            mess.setAttributes(P->second[i],i) ;
        
        mess.send(server->getSocketLink(theOwnerHandle),NM_msgBufSend);
    }

	FederateHandle theOwnerHandle ;

	// Request Object.
	theOwnerHandle = root->requestObjectOwner(theFederateHandle,theObject) ;

	G.Out(pdGendoc,"            requestObjectOwner ===> write PAVU to RTIA %d"
			,theOwnerHandle);
	G.Out(pdGendoc,"exit  Federation::requestObjectOwner");
	
    return(theOwnerHandle);
}

// ----------------------------------------------------------------------------
void 
Federation::requestClassAttributeValueUpdate(FederateHandle theFederateHandle,
								ObjectClassHandle theClassHandle,
								const std::vector <AttributeHandle> &theAttributeList,
								uint32_t theListSize)
		throw ( ObjectClassNotDefined, 
				RTIinternalError)
{
	G.Out(pdGendoc,"enter Federation::requestClassAttributeValueUpdate");

	// get object class 
	ObjectClass* oClass = root->getObjectClass(theClassHandle);
	if(!oClass) {
		throw ObjectClassNotDefined(certi::stringize() << "ObjectClassHandle <"<<theClassHandle<<"> is unknown.");
	}

	// send PAVU for all objects of this class
	ObjectClass::HandleObjectMap instances = oClass->getClassInstances();
	for( ObjectClass::HandleObjectMap::const_iterator it =
		 instances.begin(); it != instances.end(); ++it )
	{
        requestObjectOwner(theFederateHandle, it->first, theAttributeList, theListSize);
	}
    G.Out(pdGendoc,"exit  Federation::requestClassAttributeValueUpdate");
}

}} // namespace certi/rtig

// $Id: Federation.cc,v 3.152 2013/09/24 14:27:58 erk Exp $

