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
// $Id: RootObject.cc,v 3.53 2011/12/31 13:25:58 erk Exp $
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
#include "NM_Classes.hh"
#include "helper.hh"
#include "NameReservation.hh"

#include <string>
#include <stdio.h>
#include <cassert>
#include <algorithm>

using std::vector ;
using std::cout ;
using std::endl ;
using std::list ;


namespace certi {

static PrettyDebug D("ROOTOBJECT", "(RootObject) ");
static PrettyDebug G("GENDOC",__FILE__);


RootObject::RootObject(SecurityServer *security_server)
    : server(security_server), regionHandles(1)
{
	/* this object class set is the root one */
    ObjectClasses = new ObjectClassSet(server,true);
    /* this interaction class set is the root one */
    Interactions  = new InteractionSet(server,true);
    objects       = new ObjectSet(server);
	reservedNames = new NameReservationSet();
}

RootObject::~RootObject()
{
    delete ObjectClasses ;
    delete Interactions ;
    delete objects ;
	delete reservedNames ;
}

// ----------------------------------------------------------------------------
//! Print the Root Object tree to the standard output.
void
RootObject::display() const
{
    cout << endl << "Root Object Tree BEGINS:" << endl ;
    cout << "------------------------" << endl;
    if (NULL!=ObjectClasses) {
    	cout << *ObjectClasses;
    } else {
    	cout << "ObjectClasses is NULL!!" << endl;
    }
    if (NULL!=Interactions) {
        cout << *Interactions;
    } else {
    	cout << "Is is NULL!!" << endl;
    }
    if (spaces.size() > 0) {
        cout << "+ Routing Spaces :" << endl ;
	vector<RoutingSpace>::const_iterator it ;
        for (it = spaces.begin(); it != spaces.end(); ++it) {
            it->display();
        }
    }
    cout << endl << "Root Object Tree ENDS." << endl ;
    cout << "------------------------" << endl ;
}

SecurityLevelID
RootObject::getSecurityLevelID(const std::string& levelName)
{
    return server ? server->getLevelIDWithName(levelName) : PublicLevelID;
}

// ----------------------------------------------------------------------------
//! registerFederate.
void
RootObject::registerFederate(const std::string& the_federate,
                             SecurityLevelID the_level_id)
{
    if (server != NULL)
        server->registerFederate(the_federate, the_level_id);
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
RootObject::getRoutingSpaceHandle(const std::string& rs)
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
const std::string&
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
bool
RootObject::reserveObjectInstanceName(FederateHandle the_federate,
									  const std::string &the_object_name)
{
	// Empty strings not allowed
	if (the_object_name.size() <= 0)
	{
		return false;
	}

	// According to spec, the HLA prefix is reserved for RTI-internal objects.
	if (the_object_name.compare(0, 3, "HLA") == 0)
	{
		return false;
	}

	// Name reservation clashes with registered object?
	Object *object = objects->getObjectByName(the_object_name);
	if (object != 0)
	{
		return false;
	}

	// Name reservation clashes with other reserved name?
	NameReservationSet::const_iterator it = reservedNames->find(the_object_name);
	if (it != reservedNames->end() )
	{
		return false;
	}

	// Make name reservation
	NameReservation *nr = new NameReservation(the_federate, the_object_name);
	std::pair<std::string, NameReservation *> nr_pair(the_object_name, nr);
	reservedNames->insert(nr_pair);

	return true;
}

// ----------------------------------------------------------------------------
void
RootObject::registerObjectInstance(FederateHandle the_federate,
                                   ObjectClassHandle the_class,
                                   ObjectHandle the_object,
                                   const std::string& the_object_name)
    throw (InvalidObjectHandle,
           ObjectClassNotDefined,
           ObjectClassNotPublished,
           ObjectAlreadyRegistered,
           RTIinternalError)
{
    D.Out(pdRegister,
          "Federate %d attempts to register instance %d in class %d.",
          the_federate, the_object, the_class);

	NameReservationSet::iterator it = reservedNames->find(the_object_name);
	if (it != reservedNames->end() )
	{
		const NameReservation *nr = it->second;
		if (nr->getHandle() != the_federate)
		{
			throw ObjectAlreadyRegistered("The name was reserved by another federate.");
		}
	}


    Object *object ;
    object = objects->registerObjectInstance(the_federate, the_class,
                                             the_object, the_object_name);
    try {
        ObjectClasses->registerObjectInstance(the_federate, object, the_class);
    }
    catch(...)
    {   //the object is added to the ObjectSet before we check to see if the
        //object class has been defined or published.  Therefore, if an
        //exception is thrown and the instance was not added, we remove
        //it from the ObjectSet here and rethrow the exception.
        objects->deleteObjectInstance(the_federate, the_object, "");
        throw;
    }

	if (it != reservedNames->end() )
	{
		NameReservation *nr = it->second;
		reservedNames->erase(it);
		delete nr;
	}

}

// ----------------------------------------------------------------------------
void
RootObject::deleteObjectInstance(FederateHandle the_federate,
                                 ObjectHandle objectHandle,
				 FederationTime theTime,
                                 const std::string& the_tag)
    throw (DeletePrivilegeNotHeld, ObjectNotKnown, RTIinternalError)
{
    Object *object = objects->getObject(objectHandle);
    ObjectClasses->deleteObject(the_federate, object, theTime, the_tag);
    objects->deleteObjectInstance(the_federate, objectHandle, the_tag);
}

// ----------------------------------------------------------------------------
void
RootObject::deleteObjectInstance(FederateHandle the_federate,
                                 ObjectHandle objectHandle,
                                 const std::string& the_tag)
    throw (DeletePrivilegeNotHeld, ObjectNotKnown, RTIinternalError)
{
    Object *object = objects->getObject(objectHandle);
    ObjectClasses->deleteObject(the_federate, object, the_tag);
    objects->deleteObjectInstance(the_federate, objectHandle, the_tag);
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

// ----------------------------------------------------------------------------
// getAllObjectInstancesFromFederate
void
RootObject::getAllObjectInstancesFromFederate(FederateHandle the_federate, std::vector<ObjectHandle>& handles)
{
	objects->getAllObjectInstancesFromFederate(the_federate,handles);
} /* end of getAllObjectInstancesFromFederate */

void
RootObject::addObjectClass(ObjectClass* currentOC,ObjectClass* parentOC) {
	ObjectClasses->addClass(currentOC,parentOC);
} /* end of addObjectClass */

void
RootObject::addInteractionClass(Interaction* currentIC, Interaction* parentIC) {
    Interactions->addClass(currentIC,parentIC);
} /* end of addInteractionClass */

void
RootObject::convertToSerializedFOM(NM_Join_Federation_Execution& message)
{
        // The rounting spaces
        uint32_t routingSpaceCount = spaces.size();
        message.setRoutingSpacesSize(routingSpaceCount);
        for (uint32_t i = 0; i < routingSpaceCount; ++i) {
                const RoutingSpace& rs = spaces[i];
                NM_FOM_Routing_Space& mrs = message.getRoutingSpaces(i);

                mrs.setSpace(rs.getHandle());
                mrs.setName(rs.getName());

                uint32_t dimensionCount = rs.getDimensions().size();
                mrs.setDimensionsSize(dimensionCount);
                for (uint32_t j = 0; j < dimensionCount; ++j) {
                        const Dimension& d = rs.getDimensions()[j];
                        NM_FOM_Dimension& md = mrs.getDimensions(j);

                        md.setHandle(d.getHandle());
                        md.setName(d.getName());
                }
        }

        // The object classes
        message.setObjectClassesSize(ObjectClasses->size());
        uint32_t idx = 0;
        for (ObjectClassSet::handled_const_iterator i = ObjectClasses->handled_begin();
             i != ObjectClasses->handled_end(); ++i, ++idx) {
                const ObjectClass* objectClass = i->second;
                NM_FOM_Object_Class& moc = message.getObjectClasses(idx);

                moc.setHandle(objectClass->getHandle());
                std::string name = objectClass->getName();
                ObjectClassHandle superclassHandle = objectClass->getSuperclass();
                moc.setSuperClass(superclassHandle);

                ObjectClass* parent = 0;
                if (0 < superclassHandle) {
                        parent = getObjectClass(superclassHandle);

                        // strip the common substring from the parents name.
                        if (name.find(parent->getName() + ".") == 0)
                                name = name.substr(parent->getName().size() + 1);
                }

                // Transfer the short name
                moc.setName(name);

                // Transfer the attributes that are not inheritted
                uint32_t jdx = 0;
                const ObjectClass::HandleClassAttributeMap& attributeMap = i->second->getHandleClassAttributeMap();
                ObjectClass::HandleClassAttributeMap::const_iterator j = attributeMap.begin();
                for (; j != attributeMap.end(); ++j) {
                        // Dump only those attributes from the list that are not alreay in the parent
                        if (parent && parent->hasAttribute(j->second->getHandle()))
                                continue;

                        const ObjectClassAttribute* attribute = j->second;

                        moc.setAttributesSize(++jdx);
                        NM_FOM_Attribute& ma = moc.getAttributes(jdx - 1);

                        ma.setHandle(attribute->getHandle());
                        ma.setName(attribute->getName());
                        ma.setSpaceHandle(attribute->getSpace());
                        ma.setOrder(attribute->order);
                        ma.setTransport(attribute->transport);
                }
        }


        // The interaction classes
        message.setInteractionClassesSize(Interactions->size());
        idx = 0;
        for (InteractionSet::handled_const_iterator i = Interactions->handled_begin();
             i != Interactions->handled_end(); ++i, ++idx) {
                Interaction* interactionClass = i->second;
                NM_FOM_Interaction_Class& mic = message.getInteractionClasses(idx);

                mic.setInteractionClass(interactionClass->getHandle());
                std::string name = interactionClass->getName();
                InteractionClassHandle superclassHandle = interactionClass->getSuperclass();
                mic.setSuperClass(superclassHandle);
                mic.setSpace(interactionClass->getSpace());
                mic.setOrder(interactionClass->order);
                mic.setTransport(interactionClass->transport);

                // Dump only those attributes from the list that are not alreay in the parent
                Interaction* parent = 0;
                if (0 < superclassHandle) {
                        parent = getInteractionClass(superclassHandle);

                        // strip the common substring from the parents name.
                        if (name.find(parent->getName() + ".") == 0)
                                name = name.substr(parent->getName().size() + 1);
                }

                // Transfer the simple name
                mic.setName(name);

                // Transfer the new parameters
                uint32_t jdx = 0;
                const Interaction::HandleParameterMap& parameterMap = i->second->getHandleParameterMap();
                Interaction::HandleParameterMap::const_iterator j = parameterMap.begin();
                for (; j != parameterMap.end(); ++j) {
                        // Dump only those attributes from the list that are not alreay in the parent
                        const Parameter* parameter = j->second;
                        if (parent && parent->hasParameter(parameter->getHandle()))
                                continue;

                        mic.setParametersSize(++jdx);
                        NM_FOM_Parameter& mp = mic.getParameters(jdx - 1);

                        mp.setHandle(parameter->getHandle());
                        mp.setName(parameter->getName());
                }
        }
}

void
RootObject::rebuildFromSerializedFOM(const NM_Join_Federation_Execution& message)
{
        // The number of routing space records to read
        uint32_t routingSpaceCount = message.getRoutingSpacesSize();
        for (uint32_t i = 0; i < routingSpaceCount; ++i) {
                const NM_FOM_Routing_Space& mrs = message.getRoutingSpaces(i);

                RoutingSpace current;
                current.setHandle(mrs.getSpace());
                current.setName(mrs.getName());

                uint32_t dimensionCount = mrs.getDimensionsSize();
                for (uint32_t j = 0; j < dimensionCount; ++j) {
                        const NM_FOM_Dimension& md = mrs.getDimensions(j);

                        Dimension dimension(md.getHandle());
                        dimension.setName(md.getName());
                        current.addDimension(dimension);
                }

                addRoutingSpace(current);
        }

        // The number of object class records to read
        uint32_t objectClassCount = message.getObjectClassesSize();
        for (uint32_t i = 0; i < objectClassCount; ++i) {
                const NM_FOM_Object_Class& moc = message.getObjectClasses(i);

                // add the object class to the root object
                ObjectClass* current = new ObjectClass(moc.getName(), moc.getHandle());
                ObjectClass* parent = 0;
                ObjectClassHandle superclassHandle = moc.getSuperClass();
                if (0 < superclassHandle) {
                        parent = getObjectClass(superclassHandle);
                }
                addObjectClass(current, parent);

                uint32_t attributeCount = moc.getAttributesSize();
                for (uint32_t j = 0; j < attributeCount; ++j) {
                        const NM_FOM_Attribute& ma = moc.getAttributes(j);

                        // OrderType order = ma.getOrder();
                        // TransportType transport = ma.getTransport();
                        ObjectClassAttribute *attribute = new ObjectClassAttribute(ma.getName(), ma.getHandle());
                        // attribute->setTransport(ma.getTransport());
                        attribute->transport = ma.getTransport();
                        // attribute->setOrder(ma.getOrder());
                        attribute->order = ma.getOrder();
                        attribute->setSpace(ma.getSpaceHandle());
                        current->addAttribute(attribute);
                }
        }

        // The number of interactions records to read
        uint32_t interactionsCount = message.getInteractionClassesSize();
        for (uint32_t i = 0; i < interactionsCount; ++i) {
                const NM_FOM_Interaction_Class& mic = message.getInteractionClasses(i);

                Interaction* current = new Interaction(mic.getName(), mic.getInteractionClass(), mic.getTransport(), mic.getOrder());
                current->setSpace(mic.getSpace());
                Interaction* parent = 0;
                InteractionClassHandle superclassHandle = mic.getSuperClass();
                if (0 < superclassHandle) {
                        parent = getInteractionClass(superclassHandle);
                }

                addInteractionClass(current, parent);

                uint32_t parameterCount = mic.getParametersSize();
                for (uint32_t j = 0; j < parameterCount; ++j) {
                        const NM_FOM_Parameter& mp = mic.getParameters(j);

                        Parameter *parameter = new Parameter(mp.getName(), mp.getHandle());
                        current->addParameter(parameter);
                }
        }
} /* end of rebuildFromSerializedFOM */

} // namespace certi

// $Id: RootObject.cc,v 3.53 2011/12/31 13:25:58 erk Exp $
