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
// $Id: RootObject.hh,v 3.8 2003/04/09 16:41:10 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_ROOT_OBJECT_HH
#define _CERTI_ROOT_OBJECT_HH

// Project
#include <config.h>
#include "InteractionSet.hh"
#include "ObjectClassSet.hh"
#include "ObjectSet.hh"
#include "RegionImp.hh"
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
    ObjectSet *objects ;

    // -- Methods
    RootObject(SecurityServer *the_server);
    ~RootObject(void);

    void display(void) const ;

    SecurityLevelID GetSecurityLevelID(SecurityLevelName the_name);

    void registerFederate(const char *the_federate,
                          SecurityLevelID the_level_id);

    void addRoutingSpace(RoutingSpace *);
    RoutingSpace *getRoutingSpace(SpaceHandle) throw (SpaceNotDefined);
    SpaceHandle getRoutingSpaceHandle(string) throw (NameNotFound);
    string getRoutingSpaceName(SpaceHandle) throw (SpaceNotDefined);

    void addRegion(RegionImp *);
    long createRegion(SpaceHandle, long) throw (SpaceNotDefined);
    void deleteRegion(long) throw (RegionNotKnown, RegionInUse);
    RegionImp *getRegion(long) throw (RegionNotKnown);

private:
    vector<RoutingSpace *> routingSpaces ;
    list<RegionImp *> regions ;
    SecurityServer *server ;
    long freeRegionHandle ;
};

} // namespace certi

#endif // _CERTI_ROOT_OBJECT_HH

// $Id: RootObject.hh,v 3.8 2003/04/09 16:41:10 breholee Exp $
