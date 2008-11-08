// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: RootObject.cc,v 3.40 2008/11/08 11:36:05 erk Exp $
// ----------------------------------------------------------------------------

#include "Object.hh"
#include "ObjectSet.hh"
#include "ObjectAttribute.hh"
#include "ObjectClass.hh"
#include "ObjectClassSet.hh"
#include "ObjectClassAttribute.hh"
#include "Interaction.hh"
#include "InteractionSet.hh"
#include "RTIRegion.hh"
#include "RoutingSpace.hh"
#include "RootObject.hh"
#include "PrettyDebug.hh"
#include "helper.hh"

#include <string>
#include <stdio.h>
#include <cstring>
#include <cassert>
#include <algorithm>

using std::vector ;
using std::cout ;
using std::endl ;
using std::string ;
using std::list ;


namespace certi {

static pdCDebug D("ROOTOBJECT", "(RootObject) ");
static PrettyDebug G("GENDOC",__FILE__);


RootObject::RootObject(SecurityServer *security_server)
    : server(security_server), regionHandles(1)
{
	/* this object class set is the root one */
    ObjectClasses = new ObjectClassSet(server,true);
    /* this interaction class set is the root one */
    Interactions  = new InteractionSet(server,true);
    objects       = new ObjectSet(server);
}

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
    cout << ObjectClasses;
    cout << Interactions;
    if (spaces.size() > 0) {
        cout << "+ Routing Spaces :" << endl ;
	vector<RoutingSpace>::const_iterator it ;
        for (it = spaces.begin(); it != spaces.end(); ++it) {
            it->display();
        }
    }
}

SecurityLevelID
RootObject::getSecurityLevelID(const std::string& levelName)
{
    return server ? server->getLevelIDWithName(levelName.c_str()) : PublicLevelID;
}

// ----------------------------------------------------------------------------
//! registerFederate.
void
RootObject::registerFederate(const std::string& the_federate,
                             SecurityLevelID the_level_id)
{
    if (server != NULL)
        server->registerFederate(the_federate.c_str(), the_level_id);
}

// ----------------------------------------------------------------------------
/** Add a routing space. The actual routing space is a copy of the one
    provided as parameter, and the handle may be modified.
    \param rs Routing space to add
 */
void
RootObject::addRoutingSpace(const RoutingSpace &rs)
{
    spaces.push_back(rs);
    spaces.back().setHandle(spaces.size());
}

// ----------------------------------------------------------------------------
//! get a routing space handle
SpaceHandle
RootObject::getRoutingSpaceHandle(std::string rs)
    throw (NameNotFound)
{
    vector<RoutingSpace>::const_iterator i = std::find_if(
	spaces.begin(),
	spaces.end(),
	NameComparator<RoutingSpace>(rs));

    if (i == spaces.end()) throw NameNotFound("");
    else return i->getHandle();
}

// ----------------------------------------------------------------------------
//! get a routing space name
string
RootObject::getRoutingSpaceName(SpaceHandle handle)
    throw (SpaceNotDefined)
{
    if (handle <= 0 || (size_t) handle > spaces.size())
	throw SpaceNotDefined("");
    else
	return spaces[handle - 1].getName();
}

// ----------------------------------------------------------------------------
//! get a routing space
RoutingSpace &
RootObject::getRoutingSpace(SpaceHandle handle)
    throw (SpaceNotDefined)
{
    if (handle <= 0 || (size_t) handle > spaces.size())
	throw SpaceNotDefined("");
    else
	return spaces[handle - 1] ;
}

// ----------------------------------------------------------------------------
//! add a region
void
RootObject::addRegion(RTIRegion *region)
{
    regions.push_back(region);
}

// ----------------------------------------------------------------------------
//! create (and add) a region
RegionHandle
RootObject::createRegion(SpaceHandle handle, unsigned long nb_extents)
    throw (SpaceNotDefined)
{
    RTIRegion *region = new RTIRegion(regionHandles.provide(),
				      getRoutingSpace(handle),
				      nb_extents);
    addRegion(region);

    assert(region->getNumberOfExtents() == nb_extents);
    return region->getHandle();
}

// ----------------------------------------------------------------------------
// modify a region
void
RootObject::modifyRegion(RegionHandle handle, const std::vector<Extent> &extents)
    throw (RegionNotKnown, InvalidExtents)
{
    RTIRegion *region = getRegion(handle);
    region->replaceExtents(extents);
}

// ----------------------------------------------------------------------------
/** Delete a region
    \param region_handle Region to delete
*/
void
RootObject::deleteRegion(RegionHandle region_handle)
    throw (RegionNotKnown, RegionInUse)
{
    list<RTIRegion *>::iterator it = std::find_if(
	regions.begin(),
	regions.end(),
	HandleComparator<RTIRegion>(region_handle));

    if (it == regions.end()) throw RegionNotKnown("");
    else {
	// TODO: check RegionInUse
	regions.remove(*it);
	regionHandles.free((*it)->getHandle());
	delete *it ;
    }
}

// ----------------------------------------------------------------------------
/** Get a region
    \param handle RegionHandle to get
    \return Pointer to the region
*/
RTIRegion *
RootObject::getRegion(RegionHandle handle)
    throw (RegionNotKnown)
{
    list<RTIRegion *>::iterator it = std::find_if(
	regions.begin(),
	regions.end(),
	HandleComparator<RTIRegion>(handle));

    if (it == regions.end()) throw RegionNotKnown("");
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
				 FederationTime theTime,
                                 std::string the_tag)
    throw (DeletePrivilegeNotHeld, ObjectNotKnown, RTIinternalError)
{
    ObjectClasses->deleteObject(the_federate, the_object, theTime, the_tag);
    objects->deleteObjectInstance(the_federate, the_object, the_tag);
}

// ----------------------------------------------------------------------------
void
RootObject::deleteObjectInstance(FederateHandle the_federate,
                                 ObjectHandle the_object,
                                 std::string the_tag)
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
// getObjectAttribute
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
    return ObjectClasses->getObjectFromHandle(class_handle);
}

// ----------------------------------------------------------------------------
// getInteractionClass
Interaction *
RootObject::getInteractionClass(InteractionClassHandle the_class)
{
    return Interactions->getObjectFromHandle(the_class);
}
// ----------------------------------------------------------------------------
// requestObjectInstance
FederateHandle
RootObject::requestObjectOwner(FederateHandle theFederateHandle, ObjectHandle theObject)
        throw (ObjectNotKnown)
{
    G.Out(pdGendoc,"into RootObject::requestObjectOwner");

    return(objects->requestObjectOwner(theFederateHandle, theObject));

}

void
RootObject::addObjectClass(ObjectClass* currentOC,ObjectClass* parentOC) {
	ObjectClasses->addClass(currentOC,parentOC);
} /* end of addObjectClass */

void
RootObject::addInteractionClass(Interaction* currentIC, Interaction* parentIC) {
	Interactions->addClass(currentIC,parentIC);
} /* end of addInteractionClass */

} // namespace certi

// $Id: RootObject.cc,v 3.40 2008/11/08 11:36:05 erk Exp $
