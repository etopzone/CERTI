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
// $Id: RootObject.hh,v 3.4 2003/02/19 18:07:30 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_ROOT_OBJECT_HH
#define _CERTI_ROOT_OBJECT_HH

// Project
#include <config.h>
#include "InteractionSet.hh"
#include "ObjectClassSet.hh"
#include "RoutingSpace.hh"
#include "SecurityServer.hh"
#include "PrettyDebug.hh"

// Standard libraries
#include <string>
#include <cstring>
#include <stdio.h>
#include <vector>
using std::string ;
using std::vector ;

namespace certi {

class RootObject
{
public:
    // -- Attributes
    ObjectClassSet *ObjectClasses ;
    InteractionSet *Interactions ;

    // -- Methods
    // The SocketServer can be NULL on the RTIA.
    RootObject(SecurityServer *theSecurityServer);

    // Delete ObjectClasses and Interactions.
    ~RootObject();

    // Print the Root Object tree to the standard output.
    void display(void);

    // Return the LevelID of the level whose name is 'theLevelName' if
    // the security server is defined, else return PublicLevelID(on the RTIA).
    SecurityLevelID GetSecurityLevelID(SecurityLevelName theLevelName);

    void registerFederate(const char *the_federate,
                          SecurityLevelID the_level_id);

    void addRoutingSpace(RoutingSpace *);
    SpaceHandle getRoutingSpaceHandle(const char *)
        throw (NameNotFound);

private:
    vector<RoutingSpace *> routingSpaces ;
    SecurityServer *server ;
};

} // namespace certi

#endif // _CERTI_ROOT_OBJECT_HH

// $Id: RootObject.hh,v 3.4 2003/02/19 18:07:30 breholee Exp $
