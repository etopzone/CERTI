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
// $Id: DataDistributionServices.cc,v 3.10 2005/02/09 16:31:42 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIambassador.hh"
#include "Message.hh"

#include <cassert>

using std::endl ;

namespace certi {

static pdCDebug D("LIBRTI_DDM", __FILE__);

// ===========================================================================
// DATA DISTRIBUTION MANAGEMENT
// ===========================================================================

// ----------------------------------------------------------------------------
/** Create a routing region for data distribution management.
    \param space The space handle of the region
    \param nb_extents The number of extents
    \return A Region object, associated with the created region
 */
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
    Region *region = new RegionImp(rep.getRegion(), space, nb_extents,
				   rep.getNumber());

    assert(region->getNumberOfExtents() == nb_extents);
    return region ;
}

// ----------------------------------------------------------------------------
/** Notify about region modification. Applies the changes done through
    the region services to the RTI.
    \param r The region to commit to the RTI
 */
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
    D[pdDebug] << "Notify About Region " << region.getHandle()
	      << " Modification" << endl ;
    Message req, rep ;

    req.setType(Message::DDM_MODIFY_REGION);
    req.setRegion(region.getHandle());
    req.setExtents(region.getExtents());

    executeService(&req, &rep);
    region.notify();
}

// ----------------------------------------------------------------------------
/** Delete region. Correctly destroys the region (through the RTI).
    \attention Always use this function to destroy a region. Do NOT
    use the C++ delete operator.
 */
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
RTIambassador::registerObjectInstanceWithRegion(ObjectClassHandle object_class,
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
    D[pdDebug] << "registerObjectInstanceWithRegion 1" << std::endl ;
    Message req, rep ;

    req.setType(Message::DDM_REGISTER_OBJECT);
    req.setObjectClass(object_class);
    req.setTag(tag);
    req.setAHS(attrs, nb);
    req.setRegions(regions, nb);

    D[pdDebug] << "registerObjectInstanceWithRegion 2" << std::endl ;
    executeService(&req, &rep);
    D[pdDebug] << "registerObjectInstanceWithRegion 3" << std::endl ;

    return rep.getObject();
}

// ----------------------------------------------------------------------------
ObjectHandle
RTIambassador::registerObjectInstanceWithRegion(ObjectClassHandle object_class,
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
    req.setObjectClass(object_class);
    req.setAHS(attrs, nb);
    req.setRegions(regions, nb);

    executeService(&req, &rep);

    return rep.getObject();
}

/** Associate region for updates. Make attributes of an object
    be updated through a routing region.
    @param region Region to use for updates
    @param object Object to associate to the region
    @param attributes Handles of the involved attributes
    @sa unassociateRegionForUpdates
*/
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
    D[pdDebug] << "+ Associate Region for Updates" << endl ;

    Message req, rep ;
    
    req.type = Message::DDM_ASSOCIATE_REGION ;
    req.setObject(object);
    req.setRegion(dynamic_cast<RegionImp &>(region).getHandle());
    req.setAHS(attributes);

    executeService(&req, &rep);
    D[pdDebug] << "- Associate Region for Updates" << endl ;
}

/** Unassociate region for updates. Make attributes of an object be updated
    through the default region (ie. Declaration Management services)
    @param region Region to unassociate
    @param object Object to unassociate
    @see associateRegionForUpdates
 */
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
    D[pdDebug] << "+ Unassociate Region for Updates" << endl ;
    Message req, rep ;

    req.type = Message::DDM_UNASSOCIATE_REGION ;
    req.setObject(object);
    req.setRegion(dynamic_cast<RegionImp &>(region).getHandle());

    executeService(&req, &rep);
    D[pdDebug] << "- Unassociate Region for Updates" << endl ;
}

/** Subscribe object class attributes with region.
    @param handle Object class handle
    @param region Region to subscribe with
    @param attributes Attributes involved in the subscription
    @sa unsubscribeObjectClassWithRegion
 */
void
RTIambassador::subscribeObjectClassAttributesWithRegion(
    ObjectClassHandle object_class,
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
    D[pdDebug] << "+ Subscribe Object Class Attributes with Region" << endl ;
    Message req, rep ;

    req.type = Message::DDM_SUBSCRIBE_ATTRIBUTES ;
    req.setObjectClass(object_class);
    req.setRegion(dynamic_cast<RegionImp &>(region).getHandle());
    req.setAHS(attributes);
    req.setBoolean(passive);

    executeService(&req, &rep);
    D[pdDebug] << "- Subscribe Object Class Attributes with Region" << endl ;
}

/** Unsubscribe object class attributes with region.
    @param handle Object Class handle
    @param region Region to unsubscribe with
    @sa subscribeObjectClassAttributesWithRegion
 */
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
    D[pdDebug] << "+ Unsubscribe Object Class " << object_class
	       << " with Region" << endl ;
    Message req, rep ;

    req.type = Message::DDM_UNSUBSCRIBE_ATTRIBUTES ;
    req.setObjectClass(object_class);
    req.setRegion(dynamic_cast<RegionImp &>(region).getHandle());

    executeService(&req, &rep);
    D[pdDebug] << "- Unsubscribe Object Class with Region" << endl ;
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
requestClassAttributeValueUpdateWithRegion(ObjectClassHandle /*object*/,
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

// $Id: DataDistributionServices.cc,v 3.10 2005/02/09 16:31:42 breholee Exp $
