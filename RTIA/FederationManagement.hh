// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: FederationManagement.hh,v 3.5 2003/03/21 15:06:46 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIA_FEDERATION_MANAGEMENT
#define _CERTI_RTIA_FEDERATION_MANAGEMENT

#include <config.h>

#include <list>
using std::list ;

#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#include "TimeManagement.hh"
#include "PrettyDebug.hh"
#include "baseTypes.hh"
#include "RTItypes.hh"
#include "NetworkMessage.hh"
#include "Communications.hh"

namespace certi {
namespace rtia {

class TimeManagement ;

class FederationManagement
{
public:

    // -----------------------
    // -- Public Attributes --
    // -----------------------

    TimeManagement *tm ;

    FederationHandle _numero_federation ;
    FederateHandle federate ;

    Boolean _fin_execution ;

    // ----------------------------
    // -- Constructor/Destructor --
    // ----------------------------

    FederationManagement(Communications *GC);

    ~FederationManagement();

    // -----------------
    // -- RTI Methods --(Federation Management)
    // -----------------

    // -- Create/Destroy --

    void createFederationExecution(const char *theName,
                                   TypeException &e);

    void destroyFederationExecution(const char *theName,
                                    TypeException &e);

    // -- Join/Resign --

    FederateHandle
    joinFederationExecution(const char *Federate,
                            const char *Federation,
                            TypeException &e);

    void resignFederationExecution(ResignAction action,
                                   TypeException &e);

    // Synchronization.
    void registerSynchronization(const char* label,
                                 const char* tag,
                                 TypeException &e);
    void unregisterSynchronization(const char* label,
                                   TypeException &e);
    void synchronizationPointRegistrationSucceeded(const char* label);
    void federationSynchronized(const char* label);
    void announceSynchronizationPoint(const char* label,
                                      const char* tag);

private:

    // ------------------------
    // -- Private Attributes --
    // ------------------------

    Communications *comm ;

    Boolean _est_createur_federation ;
    Boolean _est_membre_federation ;

    list<char *> synchronizationLabels; //!< Labels being synchronized.

    char _nom_federation[MAX_FEDERATION_NAME_LENGTH] ;
    char _nom_federe[MAX_FEDERATE_NAME_LENGTH] ;
};

}} // namespace certi/rtia

#endif // _CERTI_RTIA_FEDERATION_MANAGEMENT

// $Id: FederationManagement.hh,v 3.5 2003/03/21 15:06:46 breholee Exp $
