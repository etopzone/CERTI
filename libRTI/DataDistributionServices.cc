// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libRTI
//
// CERTI-libRTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libRTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: DataDistributionServices.cc,v 3.3 2003/07/10 21:32:53 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIambassador.hh"

#include "Message.hh"

namespace certi {

static pdCDebug D("LIBRTI", "(libRTI DDM) - ");

// ===========================================================================
// DATA DISTRIBUTION MANAGEMENT
// ===========================================================================

// ----------------------------------------------------------------------------
// Create Region
Region *
RTIambassador::createRegion(SpaceHandle space, ULong nb_extents)
    throw (SpaceNotDefined,
           InvalidExtents,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.setType(Message::DDM_CREATE_REGION);
    req.setSpace(space);
    req.setNumber(nb_extents);
    executeService(&req, &rep);

    Region *region = new RegionImp(rep.getRegion(), rep.getNumber(),
                                   space, nb_extents);

    return region ;
}

// ----------------------------------------------------------------------------
// Notify About Region Modification
void
RTIambassador::notifyAboutRegionModification(Region &r)
    throw (RegionNotKnown,
           InvalidExtents,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    RegionImp &region = dynamic_cast<RegionImp &>(r);
    Message req, rep ;

    req.setType(Message::DDM_MODIFY_REGION);
    req.setRegion(region.getHandle());
    req.setExtents(region.getExtents());

    executeService(&req, &rep);
    region.notify();
}

// ----------------------------------------------------------------------------
// Delete Region
void
RTIambassador::deleteRegion(Region *region)
    throw (RegionNotKnown,
           RegionInUse,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    if (region == 0) {
        throw RegionNotKnown();
    }

    Message req, rep ;

    req.setType(Message::DDM_DELETE_REGION);
    req.setRegion(((RegionImp *) region)->getHandle());
    executeService(&req, &rep);

    delete region ;
}

// ----------------------------------------------------------------------------
// Register Object Instance With Region
ObjectHandle
RTIambassador::registerObjectInstanceWithRegion(ObjectClassHandle object,
                                                const char *tag,
                                                AttributeHandle attrs[],
                                                Region *regions[],
                                                ULong nb)
    throw (ObjectClassNotDefined,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           RegionNotKnown,
           InvalidRegionContext,
           ObjectAlreadyRegistered,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.setType(Message::DDM_REGISTER_OBJECT);
    req.setObject(object);
    req.setTag(tag);
    req.setAHS(attrs, nb);
    RegionImp *r = dynamic_cast<RegionImp *>(regions[0]);
    req.setRegions(const_cast<const RegionImp **>(&r), nb);

    executeService(&req, &rep);
    return rep.getObject();
}

// ----------------------------------------------------------------------------
ObjectHandle
RTIambassador::registerObjectInstanceWithRegion(ObjectClassHandle object,
                                                AttributeHandle attrs[],
                                                Region *regions[],
                                                ULong nb)
    throw (ObjectClassNotDefined,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           RegionNotKnown,
           InvalidRegionContext,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.setType(Message::DDM_REGISTER_OBJECT);
    req.setObject(object);
    req.setAHS(attrs, nb);
    RegionImp *r = dynamic_cast<RegionImp *>(regions[0]);
    req.setRegions(const_cast<const RegionImp **>(&r), nb);

    executeService(&req, &rep);

    return rep.getObject();
}

// ----------------------------------------------------------------------------
// Associate Region For Updates
void
RTIambassador::associateRegionForUpdates(Region &region,
                                         ObjectHandle object,
                                         const AttributeHandleSet &attributes)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           InvalidRegionContext,
           RegionNotKnown,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::DDM_ASSOCIATE_REGION ;
    req.setObject(object);
    req.setRegion(dynamic_cast<RegionImp &>(region).getHandle());
    req.setAHS(attributes);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// UnAssociate Region For Updates
void
RTIambassador::unassociateRegionForUpdates(Region &region,
                                           ObjectHandle object)
    throw (ObjectNotKnown,
           InvalidRegionContext,
           RegionNotKnown,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::DDM_UNASSOCIATE_REGION ;
    req.setObject(object);
    req.setRegion(dynamic_cast<RegionImp &>(region).getHandle());

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Subscribe Object Class Attributes With Region
void
RTIambassador::
subscribeObjectClassAttributesWithRegion(ObjectClassHandle object_class,
                                         Region &region,
					 const AttributeHandleSet &attributes,
                                         Boolean passive)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           RegionNotKnown,
           InvalidRegionContext,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::DDM_SUBSCRIBE_ATTRIBUTES ;
    req.setObjectClass(object_class);
    req.setRegion(dynamic_cast<RegionImp &>(region).getHandle());
    req.setAHS(attributes);
    req.setBoolean(passive);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// UnSubscribe Object Class Attributes With Region
void
RTIambassador::unsubscribeObjectClassWithRegion(ObjectClassHandle object_class,
                                                Region &region)
    throw (ObjectClassNotDefined,
           RegionNotKnown,
           ObjectClassNotSubscribed,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::DDM_UNSUBSCRIBE_ATTRIBUTES ;
    req.setObjectClass(object_class);
    req.setRegion(dynamic_cast<RegionImp &>(region).getHandle());

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Subscribe Interaction Class With Region
void
RTIambassador::subscribeInteractionClassWithRegion(InteractionClassHandle ic,
                                                   Region &region,
                                                   Boolean passive)
    throw (InteractionClassNotDefined,
           RegionNotKnown,
           InvalidRegionContext,
           FederateLoggingServiceCalls,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::DDM_SUBSCRIBE_INTERACTION ;
    req.setInteractionClass(ic);
    req.setRegion(dynamic_cast<RegionImp &>(region).getHandle());
    req.setBoolean(passive);

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// UnSubscribe Interaction Class With Region
void
RTIambassador::unsubscribeInteractionClassWithRegion(InteractionClassHandle ic,
                                                     Region &region)
    throw (InteractionClassNotDefined,
           InteractionClassNotSubscribed,
           RegionNotKnown,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::DDM_UNSUBSCRIBE_INTERACTION ;
    req.setInteractionClass(ic);
    req.setRegion(dynamic_cast<RegionImp &>(region).getHandle());

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Send Interaction With Region
EventRetractionHandle
RTIambassador::sendInteractionWithRegion(InteractionClassHandle interaction,
                                         const ParameterHandleValuePairSet &par,
                                         const FedTime &time,
                                         const char *tag,
                                         const Region &region)
    throw (InteractionClassNotDefined,
           InteractionClassNotPublished,
           InteractionParameterNotDefined,
           InvalidFederationTime,
           RegionNotKnown,
           InvalidRegionContext,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.setType(Message::DDM_SEND_INTERACTION);
    req.setInteractionClass(interaction);
    req.setPHVPS(par);
    req.setFedTime(time);
    req.setTag(tag);
    req.setRegion(dynamic_cast<const RegionImp &>(region).getHandle());

    executeService(&req, &rep);

    return rep.getEventRetraction();
}

void
RTIambassador::sendInteractionWithRegion(InteractionClassHandle interaction,
                                         const ParameterHandleValuePairSet &par,
                                         const char *tag,
                                         const Region &region)
    throw (InteractionClassNotDefined,
           InteractionClassNotPublished,
           InteractionParameterNotDefined,
           RegionNotKnown,
           InvalidRegionContext,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Message req, rep ;

    req.setType(Message::DDM_SEND_INTERACTION);
    req.setInteractionClass(interaction);
    req.setPHVPS(par);
    req.setTag(tag);
    req.setRegion(dynamic_cast<const RegionImp &>(region).getHandle());

    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Request Class Attribute Value Update With Region
void RTIambassador::
requestClassAttributeValueUpdateWithRegion(ObjectClassHandle object,
                                           const AttributeHandleSet &attrs,
                                           const Region &region)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           RegionNotKnown,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();

    Message req, rep ;
    req.setType(Message::DDM_REQUEST_UPDATE);
    req.setAHS(attrs);
    req.setRegion(dynamic_cast<const RegionImp &>(region).getHandle());
    executeService(&req, &rep);    
}

} // namespace

// $Id: DataDistributionServices.cc,v 3.3 2003/07/10 21:32:53 breholee Exp $
