// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: SecurityServer.cc,v 3.18 2009/11/19 18:15:30 erk Exp $
// ----------------------------------------------------------------------------



#include "SecurityServer.hh"
#include "PrettyDebug.hh"

#include <cassert>

using std::list ;
using std::endl ;

static PrettyDebug D("SECURITY_SERVER", __FILE__);
static PrettyDebug G("GENDOC",__FILE__);

namespace certi {

// ----------------------------------------------------------------------------
/*! Each call to this method is passed to the RTIG's SocketServer, by
  including our Federation Handle.
*/
Socket*
SecurityServer::getSocketLink(FederateHandle theFederate,
                              TransportType theType) const
{
// G.Out(pdGendoc,"into SecurityServer::getSocketLink");
Socket * sock = RTIG_SocketServer.getSocketLink(myFederation, theFederate, theType);
if ( sock == NULL )
   // Federate killed
   return NULL ;
else
   return sock ;
}

// ----------------------------------------------------------------------------
//! Compares two security level ID.
bool
SecurityServer::dominates(SecurityLevelID A, SecurityLevelID B) const
{
    return A == B || B == PublicLevelID ;
}

// ----------------------------------------------------------------------------
/*! Determines if federate can access to datas.
  1- Get the socket of this federate
  2- Check if it is a secure socket
  3- If yes, retrieve federate principal name.
  4- Retrieve Federate level
*/
bool
SecurityServer::canFederateAccessData(FederateHandle theFederate,
                                      SecurityLevelID theDataLevelID)
{
    Socket          *FederateSocket=NULL;
    SecureTCPSocket *SecureSocket=NULL;

    SecurityLevelID FederateLevel ;

    // 1- Get the socket of this federate
    //(Killed federate/link will throw an RTIinternalError)
    try {
        FederateSocket = getSocketLink(theFederate);
    }
    catch (RTIinternalError &e) {
        return dominates(PublicLevelID, theDataLevelID);
    }

    // 2- Check if it is a secure socket
    if (NULL!=FederateSocket) {
    	SecureSocket = dynamic_cast<SecureTCPSocket *>(FederateSocket);
    }

    if (NULL==SecureSocket) {
        // If not, all federate are at Public Level.
    	return dominates(PublicLevelID, theDataLevelID);
    }

    // 3- If yes, retrieve federate principal name.
    const char *FederateName = SecureSocket->getPeerName();

    // 4- Retrieve Federate level
    FederateLevel = getLevel(FederateName);

    return dominates(FederateLevel, theDataLevelID);
}

// ----------------------------------------------------------------------------
//! SecurityServer constructor.
SecurityServer::SecurityServer(SocketServer &theRTIGServer,
                               AuditFile &theAuditServer,
                               Handle theFederation)
    :  list<SecurityLevel *>(), audit(theAuditServer),
       RTIG_SocketServer(theRTIGServer)
{
    myFederation = theFederation ;

    if (myFederation == 0)
        throw RTIinternalError("");
}

// ----------------------------------------------------------------------------
//! Removes existing SecurityLevel instances before deleting instance.
SecurityServer::~SecurityServer()
{
    while (!empty()) {
        delete front();
        pop_front();
    }
}

// ----------------------------------------------------------------------------
//! Returns the federate level id stored in a FederateLevelList.
SecurityLevelID
SecurityServer::getLevel(const std::string& theFederate) const
{
    return FedLevelList.getLevel(theFederate);
}

// ----------------------------------------------------------------------------
//! Returns the level ID associated with name otherwise creates a new one.
SecurityLevelID
SecurityServer::getLevelIDWithName(const std::string& theName)
{
    if (empty()) {
	Debug(D, pdDebug) << "Empty list: added default (public) level" << endl ;
        insertPublicLevel();
    }

    if (theName.empty()) {
	Debug(D, pdDebug) << "Security Level Name empty." << endl ;
        throw RTIinternalError("Security Level Name empty.");
    }

    list<SecurityLevel *>::const_iterator i = begin();
    for (; i != end(); i++) {
        if ((*i)->Name == theName)
            return (*i)->LevelID ;
    }

    // Level not Found
    Debug(D, pdDebug) << "Level " << theName << " not found. Adding it to the list."
	       << endl ;
    LastLevelID ++ ;
    SecurityLevel *StoredLevel = new SecurityLevel(theName, LastLevelID);
    push_back(StoredLevel);

    return LastLevelID ;
}

// ----------------------------------------------------------------------------
//! Insert the public level name and id into the list.
void
SecurityServer::insertPublicLevel()
{
    SecurityLevel *PublicLevel ;

    PublicLevel = new SecurityLevel(PublicLevelName, PublicLevelID);
    assert(PublicLevel != NULL);

    push_front(PublicLevel);

    LastLevelID = PublicLevelID ;
}

// ----------------------------------------------------------------------------
//! Register a new federate with security level id.
void
SecurityServer::registerFederate(const std::string& the_federate,
                                 SecurityLevelID the_level_id)
{
    FedLevelList.addFederate(the_federate, the_level_id);
}

}

// $Id: SecurityServer.cc,v 3.18 2009/11/19 18:15:30 erk Exp $
