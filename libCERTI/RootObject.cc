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
// $Id: RootObject.cc,v 3.14 2003/07/07 23:05:26 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RootObject.hh"

#include "PrettyDebug.hh"

#include <string>
#include <cstring>
#include <stdio.h>

using std::vector ;

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
    freeRegionHandle = 1 ;
}

// ----------------------------------------------------------------------------
//! Delete object classes, interactions, objects and routing spaces.
RootObject::~RootObject()
{
    delete ObjectClasses ;
    delete Interactions ;
    delete objects ;

    vector<RoutingSpace *>::iterator i ;
    vector<RoutingSpace *>::iterator end = routingSpaces.end();
    for (i = routingSpaces.begin(); i < end ; ++i) {
        delete *i ;
    }
    routingSpaces.clear();
}

// ----------------------------------------------------------------------------
//! Print the Root Object tree to the standard output.
void
RootObject::display() const
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
RootObject::getRoutingSpaceHandle(string rs)
    throw (NameNotFound)
{
    vector<RoutingSpace *>::const_iterator i ;

    for (i = routingSpaces.begin(); i != routingSpaces.end(); i++) {
        if ((*i)->getName() == rs) {
            return (*i)->getHandle();
        }
    }
    throw new NameNotFound();
}

// ----------------------------------------------------------------------------
//! get a routing space name
string
RootObject::getRoutingSpaceName(SpaceHandle handle)
    throw (SpaceNotDefined)
{
    vector<RoutingSpace *>::const_iterator i ;

    for (i = routingSpaces.begin(); i != routingSpaces.end(); i++) {
        if ((*i)->getHandle() == handle) {
            return (*i)->getName().c_str();
        }
    }
    throw SpaceNotDefined();
}

// ----------------------------------------------------------------------------
//! get a routing space
RoutingSpace *
RootObject::getRoutingSpace(SpaceHandle handle)
    throw (SpaceNotDefined)
{
    vector<RoutingSpace *>::const_iterator i ;

    for (i = routingSpaces.begin(); i != routingSpaces.end(); i++) {
        if ((*i)->getHandle() == handle) {
            return *i ;
        }
    }
    throw new SpaceNotDefined();
}

// ----------------------------------------------------------------------------
//! add a region
void
RootObject::addRegion(RegionImp *region)
{
    regions.push_back(region);
}

// ----------------------------------------------------------------------------
//! create (and add) a region
long
RootObject::createRegion(SpaceHandle handle, long nb_extents)
    throw (SpaceNotDefined)
{
    RoutingSpace *space = this->getRoutingSpace(handle);

    RegionImp *region = new RegionImp(freeRegionHandle++, handle,
                                      space->getNbDimensions(), nb_extents);

    this->addRegion(region);
    //this->getRoutingSpace(space)->addRegion(region);

    return region->getHandle();
}

// ----------------------------------------------------------------------------
// modify a region
void
RootObject::modifyRegion(long handle, vector<Extent *> *extents)
    throw (RegionNotKnown, InvalidExtents)
{
    RegionImp *region = getRegion(handle);

    // TODO: check extents are in the routing space
    region->setExtents(*extents);
}

// ----------------------------------------------------------------------------
//! delete a region
void
RootObject::deleteRegion(long handle)
    throw (RegionNotKnown, RegionInUse)
{
    list<RegionImp *>::iterator i ;

    for (i = regions.begin(); i != regions.end(); i++) {
        if ((*i)->getHandle() == handle) {
            // TODO: check "in use"
            regions.remove(*i);
            delete *i ;
            return ;
        }
    }
    throw RegionNotKnown();
}

// ----------------------------------------------------------------------------
//! get a region
RegionImp *
RootObject::getRegion(long handle)
    throw (RegionNotKnown)
{
    list<RegionImp *>::iterator i ;

    for (i = regions.begin(); i != regions.end(); i++) {
        if ((*i)->getHandle() == handle) {
            return *i ;
        }
    }
    throw RegionNotKnown();
}

// ----------------------------------------------------------------------------
void
RootObject::registerObjectInstance(FederateHandle the_federate,
                                   ObjectClassHandle the_class,
                                   ObjectHandle the_object,
                                   const char *the_object_name)
    throw (InvalidObjectHandle,
           ObjectClassNotDefined,
           ObjectClassNotPublished,
           ObjectAlreadyRegistered,
           RTIinternalError)
{
    D.Out(pdRegister,
          "Federate %d attempts to register instance %d in class %d.",
          the_federate, the_object, the_class);

    Object *object ;
    object = objects->registerObjectInstance(the_federate, the_object,
                                             the_object_name);

    ObjectClasses->registerObjectInstance(the_federate, object, the_class);
}

// ----------------------------------------------------------------------------
void
RootObject::deleteObjectInstance(FederateHandle the_federate,
                                 ObjectHandle the_object,
                                 const char *the_tag)
    throw (DeletePrivilegeNotHeld, ObjectNotKnown, RTIinternalError)
{
    ObjectClasses->deleteObject(the_federate, the_object, the_tag);
    objects->deleteObjectInstance(the_federate, the_object, the_tag);
}

// ----------------------------------------------------------------------------
void
RootObject::killFederate(FederateHandle the_federate)
    throw (RTIinternalError)
{
    ObjectClasses->killFederate(the_federate);
    Interactions->killFederate(the_federate);
    objects->killFederate(the_federate);
}

// ----------------------------------------------------------------------------
// getObjectClassAttribute
ObjectClassAttribute *
RootObject::getObjectClassAttribute(ObjectHandle object,
				    AttributeHandle attribute)
{
    return objects->getObject(object)->getAttribute(attribute)
	->getObjectClassAttribute();
}

// ----------------------------------------------------------------------------
// getObjectClassAttribute
ObjectAttribute *
RootObject::getObjectAttribute(ObjectHandle object,
			       AttributeHandle attribute)
{
    return objects->getObject(object)->getAttribute(attribute);
}

} // namespace certi

// $Id: RootObject.cc,v 3.14 2003/07/07 23:05:26 breholee Exp $
