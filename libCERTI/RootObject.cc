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
// $Id: RootObject.cc,v 3.4 2003/02/19 18:07:30 breholee Exp $
// ----------------------------------------------------------------------------

#include "RootObject.hh"

namespace certi {

static pdCDebug D("ROOTOBJECT", "(RootObject) ");

// ----------------------------------------------------------------------------
RootObject::RootObject(SecurityServer *security_server)
    : server(security_server)
{
    ObjectClasses = new ObjectClassSet(server);
    if (ObjectClasses == NULL)
        throw RTIinternalError();

    Interactions = new InteractionSet(server);
    if (Interactions == NULL)
        throw RTIinternalError();
}

// ----------------------------------------------------------------------------
RootObject::~RootObject()
{
    delete ObjectClasses ;
    delete Interactions ;
    for (int i = 1 ; i < routingSpaces.size(); i++)
        delete routingSpaces[i] ;
    routingSpaces.clear();
}

// ----------------------------------------------------------------------------
void
RootObject::display(void)
{
    printf("\nRoot Object Tree :\n");
    ObjectClasses->display();
    Interactions->display();
    if (routingSpaces.size() > 0) {
        printf("+ Routing Spaces :\n");
        for (vector<RoutingSpace *>::iterator i = routingSpaces.begin();
             i != routingSpaces.end();
             i++) {
            (*i)->display();
        }
    }
}

// ----------------------------------------------------------------------------
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
//! get a routing space handle[HLA 1.3]
SpaceHandle
RootObject::getRoutingSpaceHandle(const char *rs)
    throw (NameNotFound)
{
    vector<RoutingSpace *>::iterator i ;

    cout << "GET ROUTING SPACE HANDLE : " << rs << endl ;
    for (i = routingSpaces.begin(); i != routingSpaces.end(); i++) {
        cout << "routing space " << (*i)->getName() << endl ;
        if (!strcmp((*i)->getName().c_str(), rs)) {
            return (*i)->getHandle();
        }
    }
    //throw new NameNotFound();
    return 0 ;
}

} // namespace certi

// $Id: RootObject.cc,v 3.4 2003/02/19 18:07:30 breholee Exp $
