// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: SecurityServer.hh,v 3.4 2003/02/19 18:07:30 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_SECURITY_SERVER_HH
#define _CERTI_SECURITY_SERVER_HH

#include <config.h>

#include <list>
using std::list ;

#include "SocketServer.hh"
#include "AuditFile.hh"
#include "FederateLevelList.hh"
#include "SecureTCPSocket.hh"

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
class SecurityServer : private list<SecurityLevel *>
{
public:

    // ------------------------------
    // -- Constructor & Destructor --
    // ------------------------------

    // Initialize the Federation's SocketServer.
    SecurityServer(SocketServer *theRTIGServer,
                   AuditFile *theAuditFile,
                   FederationHandle theFederation);

    // Free the List.
    ~SecurityServer();

    // -----------------------
    // -- Public Components --
    // -----------------------

    //! This part of the security server is linked to the RTIG Audit Server.
    AuditFile *Audit ;

    // ----------------------------
    // -- Socket Related Methods --
    // ----------------------------
    FederationHandle federation() const { return MyFederation ; };

    Socket *getSocketLink(FederateHandle theFederate,
                          TransportType theType = RELIABLE) const ;

    // ------------------------------
    // -- Security Related Methods --
    // ------------------------------
    Boolean dominates(SecurityLevelID A, SecurityLevelID B) const ;

    Boolean canFederateAccessData(FederateHandle theFederate,
                                  SecurityLevelID theDataLevelID);

    SecurityLevelID getLevelIDWithName(SecurityLevelName theName);

    void registerFederate(const char *the_federate,
                          SecurityLevelID the_level_id);

private:

    // ------------------------
    // -- Private Attributes --
    // ------------------------

    SocketServer *RTIG_SocketServer ; // Never free this object.
    FederationHandle MyFederation ;

    SecurityLevelID LastLevelID ; //!< Last Level ID attributed.

    FederateLevelList FedLevelList ;

    // ---------------------
    // -- Private Methods --
    // ---------------------

    SecurityLevelID getLevel(const char *theFederate) const ;

    void insertPublicLevel(void);
};

}

#endif // _CERTI_SECURITY_SERVER_HH

// $Id: SecurityServer.hh,v 3.4 2003/02/19 18:07:30 breholee Exp $
