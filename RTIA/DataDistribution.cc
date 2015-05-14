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
// $Id: DataDistribution.cc,v 3.38 2011/03/25 20:53:54 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "DataDistribution.hh"

#include "Interaction.hh"
#include "InteractionSet.hh"
#include "Object.hh"
#include "ObjectAttribute.hh"
#include "ObjectClass.hh"
#include "ObjectClassSet.hh"
#include "ObjectClassAttribute.hh"
#include "RoutingSpace.hh"
#include "FedRegion.hh"
#include "NM_Classes.hh"

#include <cassert>
#include <memory>

using std::endl ;

namespace certi {
namespace rtia {

static PrettyDebug D("RTIA_DDM", __FILE__);

// ----------------------------------------------------------------------------
DataDistribution::DataDistribution(RootObject *root_object,
		FederationManagement *fed_management,
		Communications *communications)
: rootObject(root_object),
  fm(fed_management),
  comm(communications) { }

// ----------------------------------------------------------------------------
SpaceHandle
DataDistribution::getRoutingSpaceHandle(const std::string& name) const
{
	return rootObject->getRoutingSpaceHandle(name);
}

// ----------------------------------------------------------------------------
// getRoutingSpaceName
//
const std::string&
DataDistribution::getRoutingSpaceName(SpaceHandle handle) const
{
	return rootObject->getRoutingSpaceName(handle);
}

// ----------------------------------------------------------------------------
// getDimensionHandle
//
DimensionHandle
DataDistribution::getDimensionHandle(const std::string& dimension, SpaceHandle space) const
throw (SpaceNotDefined, NameNotFound)
{
	return rootObject->getRoutingSpace(space).getDimensionHandle(dimension);
}

// ----------------------------------------------------------------------------
// getDimensionName
//
const std::string&
DataDistribution::getDimensionName(DimensionHandle dimension,
		SpaceHandle space) const
throw (SpaceNotDefined, DimensionNotDefined)
{
	return rootObject->getRoutingSpace(space).getDimensionName(dimension);
}

// ----------------------------------------------------------------------------
// getAttributeSpaceHandle
//
SpaceHandle
DataDistribution::getAttributeSpace(AttributeHandle attribute,
		ObjectClassHandle object_class) const
throw (ObjectClassNotDefined, AttributeNotDefined)
{
	std::cout<<"DataDistribution::getAttributeSpace"<<std::endl;
	return rootObject->ObjectClasses->getObjectFromHandle(object_class)->
			getAttribute(attribute)->getSpace();
}

// ----------------------------------------------------------------------------
// getInteractionSpaceHandle
//
SpaceHandle
DataDistribution::getInteractionSpace(InteractionClassHandle interaction) const
throw (InteractionClassNotDefined)
{
	return rootObject->Interactions->getObjectFromHandle(interaction)->getSpace();
}

// ----------------------------------------------------------------------------
// createRegion
//
long
DataDistribution::createRegion(SpaceHandle space,
		unsigned long nb_extents,
		TypeException &e)
throw (SpaceNotDefined)
{
	Debug(D, pdDebug) << "Start creating region in space " << space << "..." << endl ;
	NM_DDM_Create_Region req;

	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);
	req.setSpace(space);
	req.setNbExtents(nb_extents);

	comm->sendMessage(&req);
	std::auto_ptr<NM_DDM_Create_Region> rep(static_cast<NM_DDM_Create_Region*>(comm->waitMessage(NetworkMessage::DDM_CREATE_REGION, req.getFederate())));
	e = rep->getException();

	if (e == e_NO_EXCEPTION) {
		Debug(D, pdDebug) << "Create region " << rep->getRegion() << endl ;
		RTIRegion *region = new RTIRegion(rep->getRegion(),
				rootObject->getRoutingSpace(space),
				nb_extents);

		assert(region->getNumberOfExtents() == nb_extents);
		rootObject->addRegion(region);
		return rep->getRegion();
	}
	else {
		return 0 ;
	}
} /* end of createRegion */

// ----------------------------------------------------------------------------
// modifyRegion
//
void
DataDistribution::modifyRegion(RegionHandle handle,
		const std::vector<Extent> &extents,
		TypeException &e)
{
	Debug(D, pdDebug) << "Modify region " << handle << "..." << endl ;

	// check region
	RTIRegion *region = rootObject->getRegion(handle);

	// Request to RTIG
	NM_DDM_Modify_Region req;

	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);
	req.setRegion(handle) ;
	req.setExtents(extents);

	comm->sendMessage(&req);
	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DDM_MODIFY_REGION, req.getFederate()));
	e = rep->getException() ;

	if (e == e_NO_EXCEPTION) {
		region->replaceExtents(extents);
		Debug(D, pdDebug) << "Modified region " << handle << endl ;
	}
} /* end of modifyRegion */

// ----------------------------------------------------------------------------
// deleteRegion
//
void
DataDistribution::deleteRegion(long handle, TypeException &e)
throw (RegionNotKnown, RegionInUse)
{
	Debug(D, pdDebug) << "Delete region " << handle << "..." << endl ;

	// check region
	rootObject->getRegion(handle);

	// Request to RTIG
	NM_DDM_Delete_Region req;

	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);
	req.setRegion(handle);

	comm->sendMessage(&req);
	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DDM_DELETE_REGION, req.getFederate()));
	e = rep->getException() ;

	if (e == e_NO_EXCEPTION) {
		rootObject->deleteRegion(handle);
		Debug(D, pdDebug) << "Deleted region " << handle << endl ;
	}
} /* end of deleteRegion */

// ----------------------------------------------------------------------------
void
DataDistribution::associateRegion(ObjectHandle object,
		RegionHandle region,
		const std::vector <AttributeHandle> &attr,
		uint32_t nb,
		TypeException &e)
throw (RegionNotKnown)
{
	Debug(D, pdDebug) << "Associate Region " << region << std::endl ;

	RTIRegion *r = rootObject->getRegion(region);

	Debug(D, pdDebug) << "- unassociate object " << object << std::endl ;
	rootObject->getObject(object)->unassociate(r);
	for (uint32_t i = 0 ; i < nb ; ++i) {
		Debug(D, pdDebug) << "- associate attribute " << attr[i] << std::endl ;
		rootObject->getObjectAttribute(object, attr[i])->associate(r);
	}

	NM_DDM_Associate_Region req;

	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);
	req.setObject(object);
	req.setRegion(region) ;

	req.setAttributesSize(nb);
	for (uint32_t i=0;i<nb;++i) {
		req.setAttributes(attr[i],i);
	}

	comm->sendMessage(&req);
	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DDM_ASSOCIATE_REGION,req.getFederate()));

	e = rep->getException() ;

} /* end of associateRegion */


ObjectHandle
DataDistribution::registerObject(ObjectClassHandle class_handle,
		const std::string& name,
		const std::vector <AttributeHandle> &attrs,
		uint32_t nb,
		const std::vector<RegionHandle> regions,
		TypeException &e)
{
	Debug(D, pdDebug) << "Register object of class " << class_handle << " with "
			<< regions.size() << " region(s)." << std::endl ;

	NM_DDM_Register_Object req;

	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);
	req.setObjectClass(class_handle);
	req.setTag(name);
	req.setAttributesSize(nb);
	for (uint32_t i=0;i<nb;++i) {
		req.setAttributes(attrs[i],i);
	}
	req.setRegions(regions);

	comm->sendMessage(&req);
	std::auto_ptr<NM_DDM_Register_Object> rep(static_cast<NM_DDM_Register_Object*>(comm->waitMessage(NetworkMessage::DDM_REGISTER_OBJECT,req.getFederate())));
	e = rep->getException() ;


	if (e == e_NO_EXCEPTION) {
		rootObject->registerObjectInstance(fm->federate, class_handle, rep->getObject(),
				rep->getLabel());
		for (uint32_t i = 0 ; i < nb ; ++i) {
			Debug(D, pdDebug) << "Register attribute [" << i << "] Attr: " << attrs[i]
			                                                                        << " Region: " << regions[i] << std::endl ;

			ObjectAttribute *attribute = rootObject->getObjectAttribute(rep->getObject(), attrs[i]);
			RTIRegion *region = rootObject->getRegion(regions[i]);
			attribute->associate(region);
		}
		return rep->getObject();
	}
	else return 0 ;
} /* end of registerObject */

// ----------------------------------------------------------------------------
void
DataDistribution::unassociateRegion(ObjectHandle object,
		RegionHandle region,
		TypeException &e)
throw (ObjectNotKnown, InvalidRegionContext, RegionNotKnown)
{
	Debug(D, pdDebug) << "Unassociate Region " << region << std::endl ;

	RTIRegion *r = rootObject->getRegion(region);

	rootObject->getObject(object)->unassociate(r);

	NM_DDM_Unassociate_Region req;

	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);
	req.setObject(object);
	req.setRegion(region) ;

	comm->sendMessage(&req);
	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DDM_UNASSOCIATE_REGION,
			req.getFederate()));

	e = rep->getException() ;
} /* end of unassociateRegion */

// ----------------------------------------------------------------------------
void
DataDistribution::subscribe(ObjectClassHandle obj_class,
		RegionHandle region,
		const std::vector <AttributeHandle> &attr,
		uint32_t nb,
		TypeException &e)
throw (RegionNotKnown)
{
	Debug(D, pdDebug) << "Subscribe attributes with region " << region << endl ;
	rootObject->getRegion(region);

	NM_DDM_Subscribe_Attributes req;

	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);
	req.setObjectClass(obj_class);
	req.setRegion(region);
	req.setAttributesSize(nb);
	for (uint32_t i=0;i<nb;++i) {
		req.setAttributes(attr[i],i);
	}

	comm->sendMessage(&req);
	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DDM_SUBSCRIBE_ATTRIBUTES,
			req.getFederate()));

	e = rep->getException() ;
} /* end of subscribe */

// ----------------------------------------------------------------------------
void
DataDistribution::unsubscribeAttributes(ObjectClassHandle obj_class,
		RegionHandle region,
		TypeException &e)
throw (RegionNotKnown)
{
	Debug(D, pdDebug) << "Unsubscribe class " << obj_class
			<< " with region " << region << endl ;
	rootObject->getRegion(region);

	NM_DDM_Unsubscribe_Attributes req;

	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);
	req.setObjectClass(obj_class);
	req.setRegion(region);

	comm->sendMessage(&req);
	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DDM_UNSUBSCRIBE_ATTRIBUTES,
			req.getFederate()));

	e = rep->getException() ;
} /* end of unsubscribeAttributes */

// ----------------------------------------------------------------------------
void
DataDistribution::subscribeInteraction(InteractionClassHandle int_class,
		RegionHandle region,
		TypeException &e)
throw (RegionNotKnown)
{
	Debug(D, pdDebug) << "Subscribe interaction with region " << region << endl ;
	rootObject->getRegion(region);

	NM_DDM_Subscribe_Interaction req;

	req.setInteractionClass(int_class);
	req.setRegion(region);
	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);

	comm->sendMessage(&req);
	std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DDM_SUBSCRIBE_INTERACTION,
			req.getFederate()));

	e = rep->getException() ;
} /* end of subscribeInteraction */

// ----------------------------------------------------------------------------
void
DataDistribution::unsubscribeInteraction(InteractionClassHandle int_class,
		RegionHandle region,
		TypeException &e)
throw (RegionNotKnown)
{
	Debug(D, pdDebug) << "Unsubscribe interaction with region " << region << endl ;
	rootObject->getRegion(region);

	NM_DDM_Unsubscribe_Interaction req;

	req.setInteractionClass(int_class);
	req.setRegion(region);
	req.setFederation(fm->_numero_federation);
	req.setFederate(fm->federate);

	comm->sendMessage(&req);
	std::auto_ptr<NetworkMessage>  rep(comm->waitMessage(NetworkMessage::DDM_UNSUBSCRIBE_INTERACTION,
			req.getFederate()));

	e = rep->getException() ;
} /* end of unsubscribeInteraction */

}} // namespace certi::rtia

// $Id: DataDistribution.cc,v 3.38 2011/03/25 20:53:54 erk Exp $
