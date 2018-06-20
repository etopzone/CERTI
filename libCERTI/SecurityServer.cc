// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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
// ----------------------------------------------------------------------------

#include "PrettyDebug.hh"
#include "SecurityServer.hh"

#include <cassert>

using std::cout;
using std::list;
using std::endl;

static PrettyDebug D("SECURITY_SERVER", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);

namespace certi {

SecurityServer::SecurityServer(SocketServer& theRTIGServer, AuditFile& theAuditServer, FederationHandle theFederation)
    : list<SecurityLevel*>(), audit(theAuditServer), myFederation(theFederation), RTIG_SocketServer(theRTIGServer)
{
    if (!myFederation.isValid()) {
        throw RTIinternalError("");
    }
}

SecurityServer::~SecurityServer()
{
    while (!empty()) {
        delete front();
        pop_front();
    }
}

FederationHandle SecurityServer::federation() const
{
    return myFederation;
}

SocketServer& SecurityServer::getSocketServer() const
{
    return RTIG_SocketServer;
}
    
Socket* SecurityServer::getSocketLink(FederateHandle theFederate, TransportType theType) const
{
    if (theFederate == my_mom_federate_handle) {
        return nullptr;
    }

    return RTIG_SocketServer.getSocketLink(myFederation, theFederate, theType);
}

FederateHandle SecurityServer::getFederateHandle(Socket* theSocket) const
{
    return RTIG_SocketServer.getFederateFromSocket(myFederation, theSocket);
}

bool SecurityServer::dominates(SecurityLevelID A, SecurityLevelID B) const
{
    return A == B || B == PublicLevelID;
}

bool SecurityServer::canFederateAccessData(FederateHandle theFederate, SecurityLevelID theDataLevelID)
{
    if (theFederate == my_mom_federate_handle) {
        return true;
    }

    // 1- Get the socket of this federate
    //(Killed federate/link will throw an RTIinternalError)
    Socket* federate_socket {nullptr};
    try {
        federate_socket = getSocketLink(theFederate);
    }
    catch (RTIinternalError& e) {
        return dominates(PublicLevelID, theDataLevelID);
    }

    // 2- Check if it is a secure socket
    SecureTCPSocket* secure_socket {nullptr};
    if (federate_socket) {
        secure_socket = dynamic_cast<SecureTCPSocket*>(federate_socket);
    }

    if (secure_socket == nullptr) {
        // If not, all federate are at Public Level.
        return dominates(PublicLevelID, theDataLevelID);
    }

    // 3- If yes, retrieve federate principal name.
    const char* FederateName = secure_socket->getPeerName();

    // 4- Retrieve Federate level
    SecurityLevelID FederateLevel{getLevel(FederateName)};

    return dominates(FederateLevel, theDataLevelID);
}

SecurityLevelID SecurityServer::getLevelIDWithName(const std::string& theName)
{
    if (empty()) {
        Debug(D, pdDebug) << "Empty list: added default (public) level" << endl;
        insertPublicLevel();
    }

    if (theName.empty()) {
        Debug(D, pdDebug) << "Security Level Name empty." << endl;
        throw RTIinternalError("Security Level Name empty.");
    }

    list<SecurityLevel*>::const_iterator i = begin();
    for (; i != end(); ++i) {
        if ((*i)->Name == theName)
            return (*i)->LevelID;
    }

    // Level not Found
    Debug(D, pdDebug) << "Level " << theName << " not found. Adding it to the list." << endl;
    ++LastLevelID;
    SecurityLevel* StoredLevel = new SecurityLevel(theName, LastLevelID);
    push_back(StoredLevel);

    return LastLevelID;
}

void SecurityServer::registerFederate(const std::string& the_federate, SecurityLevelID the_level_id)
{
    FedLevelList.addFederate(the_federate, the_level_id);
}

void SecurityServer::registerMomFederateHandle(const FederateHandle handle)
{
    my_mom_federate_handle = handle;
}

SecurityLevelID SecurityServer::getLevel(const std::string& theFederate) const
{
    return FedLevelList.getLevel(theFederate);
}

void SecurityServer::insertPublicLevel()
{
    SecurityLevel* PublicLevel;

    PublicLevel = new SecurityLevel(PublicLevelName, PublicLevelID);
    assert(PublicLevel != NULL);

    push_front(PublicLevel);

    LastLevelID = PublicLevelID;
}
}
