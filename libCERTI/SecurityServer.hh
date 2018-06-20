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

#ifndef _CERTI_SECURITY_SERVER_HH
#define _CERTI_SECURITY_SERVER_HH

#include "AuditFile.hh"
#include "FederateLevelList.hh"
#include "SecureTCPSocket.hh"
#include "SocketServer.hh"

#include <list>

namespace certi {

/*! This class is an interface at the Federation Level for the previous class,
  where the Federation Handle is constant for all calls. It also adds
  security features, like a mapping between Security Levels Names and Level
  IDs, and a method to check if a Federate has a valid clearance to access
  Data at a certain level.

  Liste dynamique gerant la securite des donnees, en affectant des numeros
  aux differents niveaux de securite, des niveaux a chacun des federes, et
  en reglementant les acces aux donnees par les federes.
*/
class CERTI_EXPORT SecurityServer : private std::list<SecurityLevel*> {
public:
    SecurityServer(SocketServer& theRTIGServer, AuditFile& theAuditServer, FederationHandle theFederation);
    /// Removes existing SecurityLevel instances before deleting instance.
    virtual ~SecurityServer();

    //! This part of the security server is linked to the RTIG Audit Server.
    AuditFile& audit;

    FederationHandle federation() const;
    
    SocketServer& getSocketServer() const;

    /** Each call to this method is passed to the RTIG's SocketServer, by
     *  including our Federation Handle.
     */
    virtual Socket* getSocketLink(FederateHandle theFederate, TransportType theType = RELIABLE) const;

    FederateHandle getFederateHandle(Socket* theSocket) const;

    // Security related methods

    /// Compares two security level ID.
    bool dominates(SecurityLevelID A, SecurityLevelID B) const;

    /** Determines if federate can access to datas.
     *  1- Get the socket of this federate
     *  2- Check if it is a secure socket
     *  3- If yes, retrieve federate principal name.
     *  4- Retrieve Federate level
     */
    bool canFederateAccessData(FederateHandle theFederate, SecurityLevelID theDataLevelID);

    /// Returns the level ID associated with name otherwise creates a new one.
    SecurityLevelID getLevelIDWithName(const std::string& theName);

    /// Register a new federate with security level id.
    void registerFederate(const std::string& the_federate, SecurityLevelID the_level_id);

    void registerMomFederateHandle(const FederateHandle handle);

private:

    /// Returns the federate level id stored in a FederateLevelList.
    SecurityLevelID getLevel(const std::string& theFederate) const;

    /// Insert the public level name and id into the list.
    void insertPublicLevel();
    
    FederationHandle myFederation;
    SocketServer& RTIG_SocketServer;

    SecurityLevelID LastLevelID; /// Last Level ID attributed.
    FederateLevelList FedLevelList;

    FederateHandle my_mom_federate_handle;
};
}

#endif // _CERTI_SECURITY_SERVER_HH
