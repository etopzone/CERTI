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
// $Id: RootObject.cc,v 3.19 2003/11/10 14:54:11 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RootObject.hh"
#include "PrettyDebug.hh"
#include "NameComparator.hh"
#include "HandleComparator.hh"

#include <cassert>
#include <string>
#include <cstring>
#include <stdio.h>

using std::vector ;
using std::cout ;
using std::endl ;
using std::string ;
using std::list ;

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
	vector<RoutingSpace>::const_iterator it ;
        for (it = routingSpaces.begin(); it != routingSpaces.end(); ++it) {
            it->display();
        }
    }
}

// ----------------------------------------------------------------------------
/*! Return the LevelID of the level whose name is 'theLevelName' if the
  security server is defined, else return PublicLevelID(on the RTIA).
*/
SecurityLevelID
RootObject::GetSecurityLevelID(const char *theLevelName)
{
    return server ? server->getLevelIDWithName(theLevelName) : PublicLevelID ;
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
//! add a routing space [HLA 1.3]
void
RootObject::addRoutingSpace(const RoutingSpace &rs)
{
    routingSpaces.push_back(rs);
}

// ----------------------------------------------------------------------------
//! get a routing space handle
SpaceHandle
RootObject::getRoutingSpaceHandle(string rs)
    throw (NameNotFound)
{
    vector<RoutingSpace>::const_iterator i = std::find_if(
	routingSpaces.begin(),
	routingSpaces.end(),
	NameComparator<RoutingSpace>(rs));
    
    if (i == routingSpaces.end()) throw NameNotFound();
    else return i->getHandle();
}

// ----------------------------------------------------------------------------
//! get a routing space name
string
RootObject::getRoutingSpaceName(SpaceHandle handle)
    throw (SpaceNotDefined)
{
    vector<RoutingSpace>::const_iterator i = std::find_if(
	routingSpaces.begin(),
	routingSpaces.end(),
	HandleComparator<RoutingSpace>(handle));
    
    if (i == routingSpaces.end()) throw SpaceNotDefined();
    else return i->getName();
}

// ----------------------------------------------------------------------------
//! get a routing space
RoutingSpace &
RootObject::getRoutingSpace(SpaceHandle handle)
    throw (SpaceNotDefined)
{
    vector<RoutingSpace>::iterator i = std::find_if(
	routingSpaces.begin(),
	routingSpaces.end(),
	HandleComparator<RoutingSpace>(handle));
    
    if (i == routingSpaces.end()) throw SpaceNotDefined();
    else return *i ;
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
RegionHandle
RootObject::createRegion(SpaceHandle handle, long nb_extents)
    throw (SpaceNotDefined)
{
    RegionImp *region = new RegionImp(freeRegionHandle++, handle, nb_extents,
				      getRoutingSpace(handle).size());
    addRegion(region);

    assert(region->getNumberOfExtents() == nb_extents);
    return region->getHandle();
}

// ----------------------------------------------------------------------------
// modify a region
void
RootObject::modifyRegion(RegionHandle handle, const vector<Extent> &extents)
    throw (RegionNotKnown, InvalidExtents)
{
    RegionImp *region = getRegion(handle);

    // TODO: check extents are in the routing space, and number ok
    assert(region->getNumberOfExtents() == extents.size());

    region->setExtents(extents);
}

// ----------------------------------------------------------------------------
//! delete a region
void
RootObject::deleteRegion(RegionHandle handle)
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
RootObject::getRegion(RegionHandle handle)
    throw (RegionNotKnown)
{
//     list<RegionImp *>::iterator i ;

//     for (i = regions.begin(); i != regions.end(); i++) {
//         if ((*i)->getHandle() == handle) {
//             return *i ;
//         }
//     }
//     throw RegionNotKnown();

    list<RegionImp *>::iterator it = std::find_if(
	regions.begin(), 
	regions.end(),
	HandleComparator<RegionImp>(handle));
    if (it == regions.end()) throw RegionNotKnown();
    else return *it ;
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
    object = objects->registerObjectInstance(the_federate, the_class, 
					     the_object, the_object_name);

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

// ----------------------------------------------------------------------------
// getObject
Object *
RootObject::getObject(ObjectHandle object)
{
    return objects->getObject(object);
}

// ----------------------------------------------------------------------------
// getObjectClass
ObjectClass *
RootObject::getObjectClass(ObjectClassHandle class_handle)
{
    return ObjectClasses->getWithHandle(class_handle);
}

// ----------------------------------------------------------------------------
// getInteractionClass
Interaction *
RootObject::getInteractionClass(InteractionClassHandle the_class)
{
    return Interactions->getByHandle(the_class);
}

} // namespace certi

// $Id: RootObject.cc,v 3.19 2003/11/10 14:54:11 breholee Exp $
