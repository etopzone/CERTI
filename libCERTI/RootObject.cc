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
// $Id: RootObject.cc,v 3.6 2003/03/11 13:10:35 breholee Exp $
// ----------------------------------------------------------------------------

#include "RootObject.hh"

namespace certi {

static pdCDebug D("ROOTOBJECT", "(RootObject) ");

// ----------------------------------------------------------------------------
//! The SocketServer can be NULL on the RTIA.
RootObject::RootObject(SecurityServer *security_server)
    : server(security_server)
{
    ObjectClasses = new ObjectClassSet(server);
    Interactions = new InteractionSet(server);
    objects = new ObjectSet(server);
}

// ----------------------------------------------------------------------------
//! Delete object classes, interactions, objects and routing spaces.
RootObject::~RootObject(void)
{
    delete ObjectClasses ;
    delete Interactions ;
    delete objects ;

    for (int i = 1 ; i < routingSpaces.size(); i++)
        delete routingSpaces[i] ;
    routingSpaces.clear();
}

// ----------------------------------------------------------------------------
//! Print the Root Object tree to the standard output.
void
RootObject::display(void) const
{
    cout << endl << "Root Object Tree :" << endl ;
    ObjectClasses->display();
    Interactions->display();
    if (routingSpaces.size() > 0) {
        cout << "+ Routing Spaces :" << endl ;
        for (vector<RoutingSpace *>::const_iterator i = routingSpaces.begin();
             i != routingSpaces.end();
             i++) {
            (*i)->display();
        }
    }
}

// ----------------------------------------------------------------------------
/*! Return the LevelID of the level whose name is 'theLevelName' if the
  security server is defined, else return PublicLevelID(on the RTIA).
*/
SecurityLevelID
RootObject::GetSecurityLevelID(SecurityLevelName theLevelName)
{
    if (server != NULL)
        return server->getLevelIDWithName(theLevelName);
    else
        return PublicLevelID ;
}

// ----------------------------------------------------------------------------
//! registerFederate.
void
RootObject::registerFederate(const char *the_federate,
                             SecurityLevelID the_level_id)
{
    if (server != NULL)
        server->registerFederate(the_federate, the_level_id);
}

// ----------------------------------------------------------------------------
//! add a routing space[HLA 1.3]
void
RootObject::addRoutingSpace(RoutingSpace *rs)
{
    if (rs != 0) {
        routingSpaces.push_back(rs);
    }
}

// ----------------------------------------------------------------------------
//! get a routing space handle
SpaceHandle
RootObject::getRoutingSpaceHandle(const char *rs)
    throw (NameNotFound)
{
    vector<RoutingSpace *>::const_iterator i ;

    for (i = routingSpaces.begin(); i != routingSpaces.end(); i++) {
        cout << "routing space " << (*i)->getName() << endl ;
        if (!strcmp((*i)->getName().c_str(), rs)) {
            return (*i)->getHandle();
        }
    }
    throw new NameNotFound();
}

// ----------------------------------------------------------------------------
//! get a routing space name
const char *
RootObject::getRoutingSpaceName(SpaceHandle handle)
    throw (SpaceNotDefined)
{
    vector<RoutingSpace *>::const_iterator i ;

    for (i = routingSpaces.begin(); i != routingSpaces.end(); i++) {
        if ((*i)->getHandle() == handle) {
            return (*i)->getName().c_str();
        }
    }
    throw new SpaceNotDefined();
}

} // namespace certi

// $Id: RootObject.cc,v 3.6 2003/03/11 13:10:35 breholee Exp $
