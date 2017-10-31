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
// ----------------------------------------------------------------------------

#include "Interaction.hh"
#include "InteractionSet.hh"
#include "NM_Classes.hh"
#include "NameReservation.hh"
#include "Object.hh"
#include "ObjectAttribute.hh"
#include "ObjectClass.hh"
#include "ObjectClassAttribute.hh"
#include "ObjectClassSet.hh"
#include "ObjectSet.hh"
#include "PrettyDebug.hh"
#include "RTIRegion.hh"
#include "RootObject.hh"
#include "RoutingSpace.hh"
#include "helper.hh"

#include <algorithm>
#include <cassert>
#include <stdio.h>
#include <string>

using std::vector;
using std::list;

namespace certi {

static PrettyDebug D("ROOTOBJECT", "(RootObject) ");
static PrettyDebug G("GENDOC", __FILE__);

RootObject::RootObject(SecurityServer* security_server) : server(security_server), regionHandles(1)
{
    /* this object class set is the root one */
    ObjectClasses = new ObjectClassSet(server, true);
    /* this interaction class set is the root one */
    Interactions = new InteractionSet(server, true);
    objects = new ObjectSet(server);
    reservedNames = new NameReservationSet();
}

RootObject::~RootObject()
{
    delete ObjectClasses;
    delete Interactions;
    delete objects;
    delete reservedNames;
}

void RootObject::display() const
{
    std::cout << std::endl << "Root Object Tree BEGINS:" << std::endl;
    std::cout << "------------------------" << std::endl;

    if (ObjectClasses) {
        std::cout << *ObjectClasses;
    }
    else {
        std::cout << "ObjectClasses is NULL!!" << std::endl;
    }

    if (Interactions) {
        std::cout << *Interactions;
    }
    else {
        std::cout << "Is is NULL!!" << std::endl;
    }

    if (!spaces.empty()) {
        std::cout << "+ Routing Spaces :" << std::endl;
        for (const auto& space : spaces) {
            space.display();
        }
    }
    
    objects->display();
    
    std::cout << std::endl << "Root Object Tree ENDS." << std::endl;
    std::cout << "------------------------" << std::endl;
}

SecurityLevelID RootObject::getSecurityLevelID(const std::string& levelName)
{
    return server ? server->getLevelIDWithName(levelName) : PublicLevelID;
}

void RootObject::registerFederate(const std::string& the_federate, SecurityLevelID the_level_id)
{
    if (server) {
        server->registerFederate(the_federate, the_level_id);
    }
}

void RootObject::addRoutingSpace(const RoutingSpace& rs)
{
    spaces.push_back(rs);
    spaces.back().setHandle(spaces.size());
}

SpaceHandle RootObject::getRoutingSpaceHandle(const std::string& rs)
{
    auto it = std::find_if(begin(spaces), end(spaces), NameComparator<RoutingSpace>(rs));

    if (it == end(spaces)) {
        throw NameNotFound("");
    }
    return it->getHandle();
}

const std::string& RootObject::getRoutingSpaceName(SpaceHandle handle)
{
    if (handle <= 0 || (size_t) handle > spaces.size()) {
        throw SpaceNotDefined("");
    }
    return spaces[handle - 1].getName();
}

RoutingSpace& RootObject::getRoutingSpace(SpaceHandle handle)
{
    if (handle <= 0 || (size_t) handle > spaces.size()) {
        throw SpaceNotDefined("");
    }
    return spaces[handle - 1];
}

void RootObject::addRegion(RTIRegion* region)
{
    regions.push_back(region);
}

RegionHandle RootObject::createRegion(SpaceHandle handle, unsigned long nb_extents)
{
    RTIRegion* region = new RTIRegion(regionHandles.provide(), getRoutingSpace(handle), nb_extents);
    addRegion(region);

    assert(region->getNumberOfExtents() == nb_extents);
    return region->getHandle();
}

void RootObject::modifyRegion(RegionHandle handle, const std::vector<Extent>& extents)
{
    RTIRegion* region = getRegion(handle);
    region->replaceExtents(extents);
}

void RootObject::deleteRegion(RegionHandle region_handle)
{
    auto it = std::find_if(begin(regions), end(regions), HandleComparator<RTIRegion>(region_handle));

    if (it == end(regions)) {
        throw RegionNotKnown("");
    }

    // TODO: check RegionInUse
    regions.remove(*it);
    regionHandles.free((*it)->getHandle());
    delete *it;
}

RTIRegion* RootObject::getRegion(RegionHandle handle)
{
    auto it = std::find_if(regions.begin(), regions.end(), HandleComparator<RTIRegion>(handle));

    if (it == regions.end()) {
        throw RegionNotKnown("");
    }
    return *it;
}

bool RootObject::reserveObjectInstanceName(FederateHandle the_federate, const std::string& the_object_name)
{
    // Empty strings not allowed
    if (the_object_name.size() <= 0) {
        return false;
    }

    // According to spec, the HLA prefix is reserved for RTI-internal objects.
    if (the_object_name.compare(0, 3, "HLA") == 0) {
        return false;
    }

    // Name reservation clashes with registered object?
    Object* object = objects->getObjectByName(the_object_name);
    if (object != 0) {
        return false;
    }

    // Name reservation clashes with other reserved name?
    auto it = reservedNames->find(the_object_name);
    if (it != reservedNames->end()) {
        return false;
    }

    // Make name reservation
    NameReservation* nr = new NameReservation(the_federate, the_object_name);
    std::pair<std::string, NameReservation*> nr_pair(the_object_name, nr);
    reservedNames->insert(nr_pair);

    return true;
}

void RootObject::registerObjectInstance(FederateHandle the_federate,
                                        ObjectClassHandle the_class,
                                        ObjectHandle the_object,
                                        const std::string& the_object_name)
{
    Debug(D, pdRegister) << "Federate " << the_federate << " attempts to register instance " << the_object
                         << " in class " << the_class << std::endl;

    auto it = reservedNames->find(the_object_name);
    if (it != reservedNames->end()) {
        const NameReservation* nr = it->second;
        if (nr->getHandle() != the_federate) {
            throw ObjectAlreadyRegistered("The name was reserved by another federate.");
        }
    }

    Object* object;
    object = objects->registerObjectInstance(the_federate, the_class, the_object, the_object_name);
    try {
        ObjectClasses->registerObjectInstance(the_federate, object, the_class);
    }
    catch (...) {
        // the object is added to the ObjectSet before we check to see if the
        // object class has been defined or published.  Therefore, if an
        // exception is thrown and the instance was not added, we remove
        // it from the ObjectSet here and rethrow the exception.
        objects->deleteObjectInstance(the_federate, the_object, "");
        throw;
    }

    if (it != reservedNames->end()) {
        NameReservation* nr = it->second;
        reservedNames->erase(it);
        delete nr;
    }
}

void RootObject::deleteObjectInstance(FederateHandle the_federate,
                                      ObjectHandle objectHandle,
                                      FederationTime theTime,
                                      const std::string& the_tag)
{
    Object* object = objects->getObject(objectHandle);
    ObjectClasses->deleteObject(the_federate, object, theTime, the_tag);
    objects->deleteObjectInstance(the_federate, objectHandle, the_tag);
}

void RootObject::deleteObjectInstance(FederateHandle the_federate,
                                      ObjectHandle objectHandle,
                                      const std::string& the_tag)
{
    Object* object = objects->getObject(objectHandle);
    ObjectClasses->deleteObject(the_federate, object, the_tag);
    objects->deleteObjectInstance(the_federate, objectHandle, the_tag);
}

void RootObject::killFederate(FederateHandle the_federate)
{
    ObjectClasses->killFederate(the_federate);
    Interactions->killFederate(the_federate);
    objects->killFederate(the_federate);
}

ObjectClassAttribute* RootObject::getObjectClassAttribute(ObjectHandle object, AttributeHandle attribute)
{
    return objects->getObject(object)->getAttribute(attribute)->getObjectClassAttribute();
}

ObjectAttribute* RootObject::getObjectAttribute(ObjectHandle object, AttributeHandle attribute)
{
    return objects->getObject(object)->getAttribute(attribute);
}

Object* RootObject::getObject(ObjectHandle object)
{
    return objects->getObject(object);
}

ObjectClass* RootObject::getObjectClass(ObjectClassHandle class_handle)
{
    return ObjectClasses->getObjectFromHandle(class_handle);
}

Interaction* RootObject::getInteractionClass(InteractionClassHandle the_class)
{
    return Interactions->getObjectFromHandle(the_class);
}

FederateHandle RootObject::requestObjectOwner(FederateHandle theFederateHandle, ObjectHandle theObject)
{
    Debug(G, pdGendoc) << "into RootObject::requestObjectOwner" << std::endl;

    return (objects->requestObjectOwner(theFederateHandle, theObject));
}

void RootObject::getAllObjectInstancesFromFederate(FederateHandle the_federate, std::vector<ObjectHandle>& handles)
{
    objects->getAllObjectInstancesFromFederate(the_federate, handles);
}

void RootObject::addObjectClass(ObjectClass* currentOC, ObjectClass* parentOC)
{
    ObjectClasses->addClass(currentOC, parentOC);
}

void RootObject::addInteractionClass(Interaction* currentIC, Interaction* parentIC)
{
    Interactions->addClass(currentIC, parentIC);
}

void RootObject::convertToSerializedFOM(NM_Join_Federation_Execution& message)
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
    for (ObjectClassSet::handled_const_iterator i = ObjectClasses->handled_begin(); i != ObjectClasses->handled_end();
         ++i, ++idx) {
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
            if (name.find(parent->getName() + ".") == 0) {
                name = name.substr(parent->getName().size() + 1);
            }
        }

        // Transfer the short name
        moc.setName(name);

        // Transfer the attributes that are not inheritted
        uint32_t jdx = 0;
        const ObjectClass::HandleClassAttributeMap& attributeMap = i->second->getHandleClassAttributeMap();
        ObjectClass::HandleClassAttributeMap::const_iterator j = attributeMap.begin();
        for (; j != attributeMap.end(); ++j) {
            // Dump only those attributes from the list that are not alreay in the parent
            if (parent && parent->hasAttribute(j->second->getHandle())) {
                continue;
            }

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
    for (InteractionSet::handled_const_iterator i = Interactions->handled_begin(); i != Interactions->handled_end();
         ++i, ++idx) {
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
            if (name.find(parent->getName() + ".") == 0) {
                name = name.substr(parent->getName().size() + 1);
            }
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
            if (parent && parent->hasParameter(parameter->getHandle())) {
                continue;
            }

            mic.setParametersSize(++jdx);
            NM_FOM_Parameter& mp = mic.getParameters(jdx - 1);

            mp.setHandle(parameter->getHandle());
            mp.setName(parameter->getName());
        }
    }
}

void RootObject::rebuildFromSerializedFOM(const NM_Join_Federation_Execution& message)
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
            ObjectClassAttribute* attribute = new ObjectClassAttribute(ma.getName(), ma.getHandle());
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

        Interaction* current
            = new Interaction(mic.getName(), mic.getInteractionClass(), mic.getTransport(), mic.getOrder());
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

            Parameter* parameter = new Parameter(mp.getName(), mp.getHandle());
            current->addParameter(parameter);
        }
    }
}

} // namespace certi
