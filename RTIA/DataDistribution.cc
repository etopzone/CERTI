// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
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
// $Id: DataDistribution.cc,v 3.7 2003/06/07 22:24:12 breholee Exp $
// ----------------------------------------------------------------------------

#include "DataDistribution.hh"

// Project
#include <config.h>
#include "ObjectClassAttribute.hh"
#include "RegionImp.hh"

namespace certi {
namespace rtia {

static pdCDebug D("RTIA_DDM", "(RTIA DDM) ");

// ----------------------------------------------------------------------------
DataDistribution::DataDistribution(RootObject *ro,
                                   FederationManagement *fed_management,
                                   Communications *communications)
{
    rootObject = ro ;
    fm = fed_management ;
    comm = communications ;
}

// ----------------------------------------------------------------------------
SpaceHandle
DataDistribution::getRoutingSpaceHandle(string name)
{
    return rootObject->getRoutingSpaceHandle(name);
}

// ----------------------------------------------------------------------------
// getRoutingSpaceName
//
string
DataDistribution::getRoutingSpaceName(SpaceHandle handle)
{
    return rootObject->getRoutingSpaceName(handle);
}

// ----------------------------------------------------------------------------
// getDimensionHandle
//
DimensionHandle
DataDistribution::getDimensionHandle(string dimension, SpaceHandle space)
    throw (SpaceNotDefined, NameNotFound)
{
    return rootObject->getRoutingSpace(space)->getDimensionHandle(dimension);
}

// ----------------------------------------------------------------------------
// getDimensionName
//
string
DataDistribution::getDimensionName(DimensionHandle dimension, SpaceHandle space)
    throw (SpaceNotDefined, DimensionNotDefined)
{
    return rootObject->getRoutingSpace(space)->getDimensionName(dimension);
}

// ----------------------------------------------------------------------------
// getAttributeSpaceHandle
//
SpaceHandle
DataDistribution::getAttributeSpace(AttributeHandle attribute,
                                    ObjectClassHandle object_class)
    throw (ObjectClassNotDefined, AttributeNotDefined)
{
    return rootObject->ObjectClasses->getWithHandle(object_class)->
        getAttributeWithHandle(attribute)->getSpace();
}

// ----------------------------------------------------------------------------
// getInteractionSpaceHandle
//
SpaceHandle
DataDistribution::getInteractionSpace(InteractionClassHandle interaction)
    throw (InteractionClassNotDefined)
{
    return rootObject->Interactions->getByHandle(interaction)->getSpace();
}

// ----------------------------------------------------------------------------
// createRegion
//
long
DataDistribution::createRegion(SpaceHandle space,
                               long nb_extents,
                               TypeException &e)
    throw (SpaceNotDefined)
{
    D[pdDebug] << "Create region in space " << space << "..." << endl ;

    NetworkMessage req, rep ;

    req.type = NetworkMessage::CREATE_REGION ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.space = space ;
    req.nbExtents = nb_extents ;
    
    comm->sendMessage(&req);
    comm->waitMessage(&rep, NetworkMessage::CREATE_REGION, req.federate);
    e = rep.exception ;

    if (e == e_NO_EXCEPTION) {
        long handle = rep.region ;
        int nb = rootObject->getRoutingSpace(space)->getNbDimensions();
        RegionImp *region = new RegionImp(handle, space, nb, nb_extents);
        rootObject->addRegion(region);

        D[pdDebug] << "Created region " << handle << endl ;
        return handle ;
    }
    else
        return 0 ;
}

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
    NetworkMessage req, rep ;

    req.type = NetworkMessage::DELETE_REGION ;
    req.federation = fm->_numero_federation ;
    req.federate = fm->federate ;
    req.region = handle ;

    comm->sendMessage(&req);
    comm->waitMessage(&rep, NetworkMessage::DELETE_REGION, req.federate);
    e = rep.exception ;

    if (e == e_NO_EXCEPTION) {
        rootObject->deleteRegion(handle);
        D[pdDebug] << "Deleted region " << handle << endl ;
    }
}

}} // namespace certi::rtia

// $Id: DataDistribution.cc,v 3.7 2003/06/07 22:24:12 breholee Exp $
