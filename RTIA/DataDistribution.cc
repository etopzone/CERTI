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
// $Id: DataDistribution.cc,v 3.27 2008/11/08 11:36:05 erk Exp $
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

using std::string ;
using std::endl ;

namespace certi {
namespace rtia {

static pdCDebug D("RTIA_DDM", __FILE__);

// ----------------------------------------------------------------------------
DataDistribution::DataDistribution(RootObject *root_object,
                                   FederationManagement *fed_management,
                                   Communications *communications)
    : rootObject(root_object),
      fm(fed_management),
      comm(communications) { }

// ----------------------------------------------------------------------------
SpaceHandle
DataDistribution::getRoutingSpaceHandle(std::string name) const
{
    return rootObject->getRoutingSpaceHandle(name);
}

// ----------------------------------------------------------------------------
// getRoutingSpaceName
//
string
DataDistribution::getRoutingSpaceName(SpaceHandle handle) const
{
    return rootObject->getRoutingSpaceName(handle);
}

// ----------------------------------------------------------------------------
// getDimensionHandle
//
DimensionHandle
DataDistribution::getDimensionHandle(std::string dimension, SpaceHandle space) const
    throw (SpaceNotDefined, NameNotFound)
{
    return rootObject->getRoutingSpace(space).getDimensionHandle(dimension);
}

// ----------------------------------------------------------------------------
// getDimensionName
//
string
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
    D[pdDebug] << "Start creating region in space " << space << "..." << endl ;
    NM_DDM_Create_Region req;

    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.space = space ;
    req.nbExtents = nb_extents ;

    comm->sendMessage(&req);
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DDM_CREATE_REGION, req.federate));
    e = rep->exception ;

    if (e == e_NO_EXCEPTION) {
	D[pdDebug] << "Create region " << rep->region << endl ;
        RTIRegion *region = new RTIRegion(rep->region,
					  rootObject->getRoutingSpace(space),
					  nb_extents);

	assert(region->getNumberOfExtents() == nb_extents);
        rootObject->addRegion(region);
        return rep->region;
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
    D[pdDebug] << "Modify region " << handle << "..." << endl ;

    // check region
    RTIRegion *region = rootObject->getRegion(handle);

    // Request to RTIG
    NM_DDM_Modify_Region req;

    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.region = handle ;
    req.setExtents(extents);

    comm->sendMessage(&req);
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DDM_MODIFY_REGION, req.federate));
    e = rep->exception ;

    if (e == e_NO_EXCEPTION) {
	region->replaceExtents(extents);
	D[pdDebug] << "Modified region " << handle << endl ;
    }
} /* end of modifyRegion */

// ----------------------------------------------------------------------------
// deleteRegion
//
void
DataDistribution::deleteRegion(long handle, TypeException &e)
    throw (RegionNotKnown, RegionInUse)
{
    D[pdDebug] << "Delete region " << handle << "..." << endl ;

    // check region
    rootObject->getRegion(handle);

    // Request to RTIG
    NM_DDM_Delete_Region req;

    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.region = handle ;

    comm->sendMessage(&req);
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DDM_DELETE_REGION, req.federate));
    e = rep->exception ;

    if (e == e_NO_EXCEPTION) {
        rootObject->deleteRegion(handle);
        D[pdDebug] << "Deleted region " << handle << endl ;
    }
} /* end of deleteRegion */

// ----------------------------------------------------------------------------
void
DataDistribution::associateRegion(ObjectHandle object,
				  RegionHandle region,
				  std::vector <AttributeHandle> &attr,
				  int nb,
				  TypeException &e)
    throw (RegionNotKnown)
{
    D[pdDebug] << "Associate Region " << region << std::endl ;

    RTIRegion *r = rootObject->getRegion(region);

    D[pdDebug] << "- unassociate object " << object << std::endl ;
    rootObject->getObject(object)->unassociate(r);
    for (int i = 0 ; i < nb ; ++i) {
	D[pdDebug] << "- associate attribute " << attr[i] << std::endl ;
	rootObject->getObjectAttribute(object, attr[i])->associate(r);
    }

    NM_DDM_Associate_Region req;

    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = object ;
    req.region = region ;

    req.setAHS(attr, nb);

    comm->sendMessage(&req);
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DDM_ASSOCIATE_REGION,req.federate));

    e = rep->exception ;

} /* end of associateRegion */

// ----------------------------------------------------------------------------
ObjectHandle
DataDistribution::registerObject(ObjectClassHandle class_handle,
				 const std::string name,
				 const std::vector <AttributeHandle> &attrs,
				 int nb,
				 const std::vector<RegionHandle> regions,
				 TypeException &e)
{
    D[pdDebug] << "Register object of class " << class_handle << " with "
	       << regions.size() << " region(s)." << std::endl ;

    NM_DDM_Register_Object req;

    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.objectClass = class_handle ;
    req.setTag(name.c_str());
    req.setAHS(attrs, nb);
    req.setRegions(regions);

    comm->sendMessage(&req);
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DDM_REGISTER_OBJECT,req.federate));

    e = rep->exception ;

    if (e == e_NO_EXCEPTION) {
        rootObject->registerObjectInstance(fm->federate, class_handle, rep->object,
                                           rep->getLabel().c_str());
	for (int i = 0 ; i < nb ; ++i) {
	    D[pdDebug] << "Register attribute [" << i << "] Attr: " << attrs[i]
		       << " Region: " << regions[i] << std::endl ;

	    ObjectAttribute *attribute = rootObject->getObjectAttribute(rep->object, attrs[i]);
	    RTIRegion *region = rootObject->getRegion(regions[i]);
	    attribute->associate(region);
	}
	    return rep->object;
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
    D[pdDebug] << "Unassociate Region " << region << std::endl ;

    RTIRegion *r = rootObject->getRegion(region);

    rootObject->getObject(object)->unassociate(r);

    NM_DDM_Unassociate_Region req;

    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.object = object ;
    req.region = region ;

    comm->sendMessage(&req);
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DDM_UNASSOCIATE_REGION,
		      req.federate));

   e = rep->exception ;
} /* end of unassociateRegion */

// ----------------------------------------------------------------------------
void
DataDistribution::subscribe(ObjectClassHandle obj_class,
			    RegionHandle region,
			    std::vector <AttributeHandle> &attr,
			    int nb,
			    TypeException &e)
    throw (RegionNotKnown)
{
    D[pdDebug] << "Subscribe attributes with region " << region << endl ;
    rootObject->getRegion(region);

    NM_DDM_Subscribe_Attributes req;

    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.objectClass = obj_class ;
    req.region = region ;
    req.setAHS(attr, nb);

    comm->sendMessage(&req);
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DDM_SUBSCRIBE_ATTRIBUTES,
		      req.federate));

    e = rep->exception ;
} /* end of subscribe */

// ----------------------------------------------------------------------------
void
DataDistribution::unsubscribeAttributes(ObjectClassHandle obj_class,
					RegionHandle region,
					TypeException &e)
    throw (RegionNotKnown)
{
    D[pdDebug] << "Unsubscribe class " << obj_class
	       << " with region " << region << endl ;
    rootObject->getRegion(region);

    NM_DDM_Unsubscribe_Attributes req;

    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.objectClass = obj_class ;
    req.region = region ;

    comm->sendMessage(&req);
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DDM_UNSUBSCRIBE_ATTRIBUTES,
		      req.federate));

    e = rep->exception ;
} /* end of unsubscribeAttributes */

// ----------------------------------------------------------------------------
void
DataDistribution::subscribeInteraction(InteractionClassHandle int_class,
				       RegionHandle region,
				       TypeException &e)
    throw (RegionNotKnown)
{
    D[pdDebug] << "Subscribe interaction with region " << region << endl ;
    rootObject->getRegion(region);

    NM_DDM_Subscribe_Interaction req;

    req.interactionClass = int_class ;
    req.region = region ;

    comm->sendMessage(&req);
    std::auto_ptr<NetworkMessage> rep(comm->waitMessage(NetworkMessage::DDM_SUBSCRIBE_INTERACTION,
		      req.federate));

    e = rep->exception ;
} /* end of subscribeInteraction */

// ----------------------------------------------------------------------------
void
DataDistribution::unsubscribeInteraction(InteractionClassHandle int_class,
					 RegionHandle region,
					 TypeException &e)
    throw (RegionNotKnown)
{
    D[pdDebug] << "Unsubscribe interaction with region " << region << endl ;
    rootObject->getRegion(region);

    NM_DDM_Unsubscribe_Interaction req;

    req.interactionClass = int_class ;
    req.region = region ;

    comm->sendMessage(&req);
    std::auto_ptr<NetworkMessage>  rep(comm->waitMessage(NetworkMessage::DDM_UNSUBSCRIBE_INTERACTION,
		      req.federate));

    e = rep->exception ;
} /* end of unsubscribeInteraction */

}} // namespace certi::rtia

// $Id: DataDistribution.cc,v 3.27 2008/11/08 11:36:05 erk Exp $
