// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003-2005  ONERA
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
// ----------------------------------------------------------------------------

#include "DataDistribution.hh"
#include <config.h>

#include <cassert>
#include <memory>

#include <libCERTI/FedRegion.hh>
#include <libCERTI/Interaction.hh>
#include <libCERTI/InteractionSet.hh>
#include <libCERTI/NM_Classes.hh>
#include <libCERTI/Object.hh>
#include <libCERTI/ObjectAttribute.hh>
#include <libCERTI/ObjectClass.hh>
#include <libCERTI/ObjectClassAttribute.hh>
#include <libCERTI/ObjectClassSet.hh>
#include <libCERTI/RoutingSpace.hh>

using std::endl;

namespace certi {
namespace rtia {

static PrettyDebug D("RTIA_DDM", __FILE__);

DataDistribution::DataDistribution(RootObject* root_object,
                                   FederationManagement* fed_management,
                                   Communications* communications)
    : rootObject(root_object), fm(fed_management), comm(communications)
{
}

SpaceHandle DataDistribution::getRoutingSpaceHandle(const std::string& name) const
{
    return rootObject->getRoutingSpaceHandle(name);
}

const std::string& DataDistribution::getRoutingSpaceName(SpaceHandle handle) const
{
    return rootObject->getRoutingSpaceName(handle);
}

DimensionHandle DataDistribution::getDimensionHandle(const std::string& dimension, SpaceHandle space) const
    throw(SpaceNotDefined, NameNotFound)
{
    return rootObject->getRoutingSpace(space).getDimensionHandle(dimension);
}

const std::string& DataDistribution::getDimensionName(DimensionHandle dimension, SpaceHandle space) const
    throw(SpaceNotDefined, DimensionNotDefined)
{
    return rootObject->getRoutingSpace(space).getDimensionName(dimension);
}

SpaceHandle DataDistribution::getAttributeSpace(AttributeHandle attribute, ObjectClassHandle object_class) const
    throw(ObjectClassNotDefined, AttributeNotDefined)
{
    std::cout << "DataDistribution::getAttributeSpace" << std::endl;
    return rootObject->ObjectClasses->getObjectFromHandle(object_class)->getAttribute(attribute)->getSpace();
}

SpaceHandle DataDistribution::getInteractionSpace(InteractionClassHandle interaction) const
    throw(InteractionClassNotDefined)
{
    return rootObject->Interactions->getObjectFromHandle(interaction)->getSpace();
}

long DataDistribution::createRegion(SpaceHandle space,
                                    unsigned long nb_extents,
                                    Exception::Type& e) throw(SpaceNotDefined)
{
    Debug(D, pdDebug) << "Start creating region in space " << space << "..." << endl;
    NM_DDM_Create_Region req;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setSpace(space);
    req.setNbExtents(nb_extents);

    comm->sendMessage(&req);
    std::unique_ptr<NM_DDM_Create_Region> rep(static_cast<NM_DDM_Create_Region*>(
        comm->waitMessage(NetworkMessage::Type::DDM_CREATE_REGION, req.getFederate())));
    e = rep->getException();

    if (e == Exception::Type::NO_EXCEPTION) {
        Debug(D, pdDebug) << "Create region " << rep->getRegion() << endl;
        RTIRegion* region = new RTIRegion(rep->getRegion(), rootObject->getRoutingSpace(space), nb_extents);

        assert(region->getNumberOfExtents() == nb_extents);
        rootObject->addRegion(region);
        return rep->getRegion();
    }
    else {
        return 0;
    }
}

void DataDistribution::modifyRegion(RegionHandle handle, const std::vector<Extent>& extents, Exception::Type& e)
{
    Debug(D, pdDebug) << "Modify region " << handle << "..." << endl;

    // check region
    RTIRegion* region = rootObject->getRegion(handle);

    // Request to RTIG
    NM_DDM_Modify_Region req;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setRegion(handle);
    req.setExtents(extents);

    comm->sendMessage(&req);
    std::unique_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::Type::DDM_MODIFY_REGION, req.getFederate()));
    e = rep->getException();

    if (e == Exception::Type::NO_EXCEPTION) {
        region->replaceExtents(extents);
        Debug(D, pdDebug) << "Modified region " << handle << endl;
    }
}

void DataDistribution::deleteRegion(long handle, Exception::Type& e) throw(RegionNotKnown, RegionInUse)
{
    Debug(D, pdDebug) << "Delete region " << handle << "..." << endl;

    // check region
    rootObject->getRegion(handle);

    // Request to RTIG
    NM_DDM_Delete_Region req;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setRegion(handle);

    comm->sendMessage(&req);
    std::unique_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::Type::DDM_DELETE_REGION, req.getFederate()));
    e = rep->getException();

    if (e == Exception::Type::NO_EXCEPTION) {
        rootObject->deleteRegion(handle);
        Debug(D, pdDebug) << "Deleted region " << handle << endl;
    }
}

void DataDistribution::associateRegion(ObjectHandle object,
                                       RegionHandle region,
                                       const std::vector<AttributeHandle>& attr,
                                       uint32_t nb,
                                       Exception::Type& e) throw(RegionNotKnown)
{
    Debug(D, pdDebug) << "Associate Region " << region << std::endl;

    RTIRegion* r = rootObject->getRegion(region);

    Debug(D, pdDebug) << "- unassociate object " << object << std::endl;
    rootObject->getObject(object)->unassociate(r);
    for (uint32_t i = 0; i < nb; ++i) {
        Debug(D, pdDebug) << "- associate attribute " << attr[i] << std::endl;
        rootObject->getObjectAttribute(object, attr[i])->associate(r);
    }

    NM_DDM_Associate_Region req;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setObject(object);
    req.setRegion(region);

    req.setAttributesSize(nb);
    for (uint32_t i = 0; i < nb; ++i) {
        req.setAttributes(attr[i], i);
    }

    comm->sendMessage(&req);
    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::DDM_ASSOCIATE_REGION, req.getFederate()));

    e = rep->getException();

}

ObjectHandle DataDistribution::registerObject(ObjectClassHandle class_handle,
                                              const std::string& name,
                                              const std::vector<AttributeHandle>& attrs,
                                              uint32_t nb,
                                              const std::vector<RegionHandle> regions,
                                              Exception::Type& e)
{
    Debug(D, pdDebug) << "Register object of class " << class_handle << " with " << regions.size() << " region(s)."
                      << std::endl;

    NM_DDM_Register_Object req;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setObjectClass(class_handle);
    req.setTag(name);
    req.setAttributesSize(nb);
    for (uint32_t i = 0; i < nb; ++i) {
        req.setAttributes(attrs[i], i);
    }
    req.setRegions(regions);

    comm->sendMessage(&req);
    std::unique_ptr<NM_DDM_Register_Object> rep(static_cast<NM_DDM_Register_Object*>(
        comm->waitMessage(NetworkMessage::Type::DDM_REGISTER_OBJECT, req.getFederate())));
    e = rep->getException();

    if (e == Exception::Type::NO_EXCEPTION) {
        auto responses
            = rootObject->registerObjectInstance(fm->getFederateHandle(), class_handle, rep->getObject(), rep->getLabel());
        std::cout << "==========================" << std::endl;
        std::cout << "RESPONSES FROM ROOT OBJECT" << std::endl;
        for (auto& rep : responses) {
            std::cout << (int) rep.message()->getMessageType() << std::endl;
        }
        std::cout << "==========================" << std::endl;

        for (uint32_t i = 0; i < nb; ++i) {
            Debug(D, pdDebug) << "Register attribute [" << i << "] Attr: " << attrs[i] << " Region: " << regions[i]
                              << std::endl;

            ObjectAttribute* attribute = rootObject->getObjectAttribute(rep->getObject(), attrs[i]);
            RTIRegion* region = rootObject->getRegion(regions[i]);
            attribute->associate(region);
        }
        return rep->getObject();
    }
    else
        return 0;
}

void DataDistribution::unassociateRegion(ObjectHandle object,
                                         RegionHandle region,
                                         Exception::Type& e) throw(ObjectNotKnown, InvalidRegionContext, RegionNotKnown)
{
    Debug(D, pdDebug) << "Unassociate Region " << region << std::endl;

    RTIRegion* r = rootObject->getRegion(region);

    rootObject->getObject(object)->unassociate(r);

    NM_DDM_Unassociate_Region req;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setObject(object);
    req.setRegion(region);

    comm->sendMessage(&req);
    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::DDM_UNASSOCIATE_REGION, req.getFederate()));

    e = rep->getException();
}

void DataDistribution::subscribe(ObjectClassHandle obj_class,
                                 RegionHandle region,
                                 const std::vector<AttributeHandle>& attr,
                                 uint32_t nb,
                                 Exception::Type& e) throw(RegionNotKnown)
{
    Debug(D, pdDebug) << "Subscribe attributes with region " << region << endl;
    rootObject->getRegion(region);

    NM_DDM_Subscribe_Attributes req;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setObjectClass(obj_class);
    req.setRegion(region);
    req.setAttributesSize(nb);
    for (uint32_t i = 0; i < nb; ++i) {
        req.setAttributes(attr[i], i);
    }

    comm->sendMessage(&req);
    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::DDM_SUBSCRIBE_ATTRIBUTES, req.getFederate()));

    e = rep->getException();
}

void DataDistribution::unsubscribeAttributes(ObjectClassHandle obj_class,
                                             RegionHandle region,
                                             Exception::Type& e) throw(RegionNotKnown)
{
    Debug(D, pdDebug) << "Unsubscribe class " << obj_class << " with region " << region << endl;
    rootObject->getRegion(region);

    NM_DDM_Unsubscribe_Attributes req;

    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());
    req.setObjectClass(obj_class);
    req.setRegion(region);

    comm->sendMessage(&req);
    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::DDM_UNSUBSCRIBE_ATTRIBUTES, req.getFederate()));

    e = rep->getException();
}

void DataDistribution::subscribeInteraction(InteractionClassHandle int_class,
                                            RegionHandle region,
                                            Exception::Type& e) throw(RegionNotKnown)
{
    Debug(D, pdDebug) << "Subscribe interaction with region " << region << endl;
    rootObject->getRegion(region);

    NM_DDM_Subscribe_Interaction req;

    req.setInteractionClass(int_class);
    req.setRegion(region);
    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());

    comm->sendMessage(&req);
    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::DDM_SUBSCRIBE_INTERACTION, req.getFederate()));

    e = rep->getException();
}

void DataDistribution::unsubscribeInteraction(InteractionClassHandle int_class,
                                              RegionHandle region,
                                              Exception::Type& e) throw(RegionNotKnown)
{
    Debug(D, pdDebug) << "Unsubscribe interaction with region " << region << endl;
    rootObject->getRegion(region);

    NM_DDM_Unsubscribe_Interaction req;

    req.setInteractionClass(int_class);
    req.setRegion(region);
    req.setFederation(fm->getFederationHandle().get());
    req.setFederate(fm->getFederateHandle());

    comm->sendMessage(&req);
    std::unique_ptr<NetworkMessage> rep(
        comm->waitMessage(NetworkMessage::Type::DDM_UNSUBSCRIBE_INTERACTION, req.getFederate()));

    e = rep->getException();
}
}
} // namespace certi::rtia
