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
// $Id: SupportServices.cc,v 3.3 2003/10/27 10:42:47 breholee Exp $
// ----------------------------------------------------------------------------

#include "RTIambassador.hh"

#include <config.h>
#include "Message.hh"

#include <iostream>

using std::endl ;

namespace certi {

static pdCDebug D("LIBRTI_SUPPORT", __FILE__);

// ===========================================================================
// RTI SUPPORT SERVICES
// ===========================================================================

// ----------------------------------------------------------------------------
/** Get object class handle
    \param theName Name of the object class
 */
ObjectClassHandle
RTIambassador::getObjectClassHandle(const char *theName)
    throw (NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;
    req.type = Message::GET_OBJECT_CLASS_HANDLE ;
    req.setName(theName);
    executeService(&req, &rep);
    return rep.getObjectClass();
}

// ----------------------------------------------------------------------------
/** Get object class name.
    \param handle Handle of the object class
    \return The class name associated with the handle, memory has to
    be freed by the caller.
*/
char *
RTIambassador::getObjectClassName(ObjectClassHandle handle)
    throw (ObjectClassNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;
    req.type = Message::GET_OBJECT_CLASS_NAME ;
    req.setObjectClass(handle);
    executeService(&req, &rep);
    return strdup(rep.getName());
}

// ----------------------------------------------------------------------------
/** Get attribute handle.
    \param theName Name of the attribute
    \param whichClass Handle of the attribute's class
 */
AttributeHandle
RTIambassador::getAttributeHandle(const char *theName,
                                  ObjectClassHandle whichClass)
    throw (ObjectClassNotDefined,
           NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)

{
    Message req, rep ;
    req.type = Message::GET_ATTRIBUTE_HANDLE ;
    req.setName(theName);
    req.setObjectClass(whichClass);
    executeService(&req, &rep);
    return rep.getAttribute();
}

// ----------------------------------------------------------------------------
/** Get attribute name.
    \param theHandle Handle of the attribute
    \param whichClass Handle of the attribute's class
 */

char *
RTIambassador::getAttributeName(AttributeHandle theHandle,
                                ObjectClassHandle whichClass)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;
    req.type = Message::GET_ATTRIBUTE_NAME ;
    req.setAttribute(theHandle);
    req.setObjectClass(whichClass);
    executeService(&req, &rep);
    return strdup(rep.getName());
}

// ----------------------------------------------------------------------------
// Get Interaction Class Handle
InteractionClassHandle
RTIambassador::getInteractionClassHandle(const char *theName)
    throw (NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_INTERACTION_CLASS_HANDLE ;
    req.setName(theName);

    executeService(&req, &rep);

    return rep.getInteractionClass();
}

// ----------------------------------------------------------------------------
// Get Interaction Class Name
char *
RTIambassador::getInteractionClassName(InteractionClassHandle theHandle)
    throw (InteractionClassNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_INTERACTION_CLASS_NAME ;
    req.setInteractionClass(theHandle);

    executeService(&req, &rep);

    return strdup(rep.getName());
}

// ----------------------------------------------------------------------------
// Get Parameter Handle
ParameterHandle
RTIambassador::getParameterHandle(const char *theName,
                                  InteractionClassHandle whichClass)
    throw (InteractionClassNotDefined,
           NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_PARAMETER_HANDLE ;
    req.setName(theName);
    req.setInteractionClass(whichClass);

    executeService(&req, &rep);

    return rep.getParameter();
}

// ----------------------------------------------------------------------------
// Get Parameter Name
char *
RTIambassador::getParameterName(ParameterHandle theHandle,
                                InteractionClassHandle whichClass)
    throw (InteractionClassNotDefined,
           InteractionParameterNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_PARAMETER_NAME ;
    req.setParameter(theHandle);
    req.setInteractionClass(whichClass);

    executeService(&req, &rep);

    return strdup(rep.getName());
}

// ----------------------------------------------------------------------------
// Get Object Instance Handle
ObjectHandle
RTIambassador::getObjectInstanceHandle(const char *theName)
    throw (ObjectNotKnown,
           NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError,
           UnimplementedService)
{
    Message req, rep ;

    req.type = Message::GET_OBJECT_INSTANCE_HANDLE ;
    req.setName(theName);

    executeService(&req, &rep);

    return rep.getObject();
}

// ----------------------------------------------------------------------------
// Get Object Instance Name
char *
RTIambassador::getObjectInstanceName(ObjectHandle theHandle)
    throw (ObjectNotKnown,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError,
           UnimplementedService)
{
    Message req, rep ;

    req.type = Message::GET_OBJECT_INSTANCE_NAME ;
    req.setObject(theHandle);

    executeService(&req, &rep);

    return strdup(rep.getName());
}

// ----------------------------------------------------------------------------
/** Get routing space handle
    \param rs_name Name of the routing space
 */
SpaceHandle
RTIambassador::getRoutingSpaceHandle(const char *rs_name)
    throw (NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    D[pdDebug] << "Get routing space handle: " << rs_name << endl ;
    Message req, rep ;
    req.type = Message::GET_SPACE_HANDLE ;
    req.setName(rs_name);
    executeService(&req, &rep);
    return rep.getSpace();
}

// ----------------------------------------------------------------------------
/** Get routing space name
    \param handle Handle of the routing space
 */
char *
RTIambassador::getRoutingSpaceName(SpaceHandle handle)
    throw (SpaceNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;
    req.type = Message::GET_SPACE_NAME ;
    req.setSpace(handle);
    executeService(&req, &rep);
    return strdup(rep.getName());
}

// ----------------------------------------------------------------------------
/** Get dimension handle
    \parameter dimension Name of the dimension
    \parameter space The dimension's routing space handle
 */
DimensionHandle
RTIambassador::getDimensionHandle(const char *dimension,
                                  SpaceHandle space)
    throw (SpaceNotDefined,
           NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;
    req.type = Message::GET_DIMENSION_HANDLE ;
    req.setName(dimension);
    req.setSpace(space);
    executeService(&req, &rep);
    return rep.getDimension();
}

// ----------------------------------------------------------------------------
/** Get dimension name
    \param dimension Handle of the dimension
    \param space The dimension's routing space handle
 */
char *
RTIambassador::getDimensionName(DimensionHandle dimension,
                                SpaceHandle space)
    throw (SpaceNotDefined,
           DimensionNotDefined,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;
    req.type = Message::GET_DIMENSION_NAME ;
    req.setDimension(dimension);
    req.setSpace(space);
    executeService(&req, &rep);
    return strdup(rep.getName());
}

// ----------------------------------------------------------------------------
/** Get attribute routing space handle
    \param attribute The attribute handle
    \param object_class The attribute's class handle
    \return The associated routing space handle
 */
SpaceHandle
RTIambassador::getAttributeRoutingSpaceHandle(AttributeHandle attribute,
                                              ObjectClassHandle object_class)
    throw (ObjectClassNotDefined,
	   AttributeNotDefined,
           FederateNotExecutionMember,
	   ConcurrentAccessAttempted,
           RTIinternalError)
{
    Message req, rep ;
    req.type = Message::GET_ATTRIBUTE_SPACE_HANDLE ;
    req.setAttribute(attribute);
    req.setObjectClass(object_class);
    executeService(&req, &rep);
    return rep.getSpace();
}

// ----------------------------------------------------------------------------
// Get Object Class
ObjectClassHandle
RTIambassador::getObjectClass(ObjectHandle theObject)
    throw (ObjectNotKnown,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError,
           UnimplementedService)
{
    Message req, rep ;
    req.type = Message::GET_OBJECT_CLASS ;
    req.setObject(theObject);
    executeService(&req, &rep);
    return rep.getObjectClass();
}

// ----------------------------------------------------------------------------
/** Get interaction routing space handle
    \param inter The interaction handle
    \return The associated routing space
 */
SpaceHandle
RTIambassador::getInteractionRoutingSpaceHandle(InteractionClassHandle inter)
    throw (InteractionClassNotDefined,
	   FederateNotExecutionMember,
           ConcurrentAccessAttempted,
	   RTIinternalError)
{
    Message req, rep ;
    req.type = Message::GET_INTERACTION_SPACE_HANDLE ;
    req.setInteractionClass(inter);
    this->executeService(&req, &rep);
    return rep.getSpace();
}

// ----------------------------------------------------------------------------
// Get Transportation Handle
TransportationHandle
RTIambassador::getTransportationHandle(const char *theName)
    throw (NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;
    req.type = Message::GET_TRANSPORTATION_HANDLE ;
    req.setName(theName);
    executeService(&req, &rep);
    return rep.getTransportation();
}

// ----------------------------------------------------------------------------
// Get Transportation Name
char *
RTIambassador::getTransportationName(TransportationHandle theHandle)
    throw (InvalidTransportationHandle,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;
    req.type = Message::GET_TRANSPORTATION_NAME ;
    req.setTransportation(theHandle);
    executeService(&req, &rep);
    return(strdup(rep.getName()));
}

// ----------------------------------------------------------------------------
// Get Ordering Handle
OrderingHandle
RTIambassador::getOrderingHandle(const char *theName)
    throw (NameNotFound,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;
    req.type = Message::GET_ORDERING_HANDLE ;
    req.setName(theName);
    executeService(&req, &rep);
    return rep.getOrdering();
}


// ----------------------------------------------------------------------------
// Get Ordering Name
char *
RTIambassador::getOrderingName(OrderingHandle theHandle)
    throw (InvalidOrderingHandle,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;
    req.type = Message::GET_ORDERING_NAME ;
    req.setOrdering(theHandle);
    executeService(&req, &rep);
    return(strdup(rep.getName()));
}

// ----------------------------------------------------------------------------
// Enable Class Relevance Advisory Switch
void
RTIambassador::enableClassRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;
    req.type = Message::ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH ;
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Class Relevance Advisory Switch
void
RTIambassador::disableClassRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;
    req.type = Message::DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH ;
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Enable Attribute Relevance Advisory Switch
void
RTIambassador::enableAttributeRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;
    req.type = Message::ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH ;
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Attribute Relevance Advisory Switch
void
RTIambassador::disableAttributeRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;
    req.type = Message::DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH ;
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Enable Attribute Scope Advisory Switch
void RTIambassador::enableAttributeScopeAdvisorySwitch()
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;
    req.type = Message::ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH ;
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Attribute Scope Advisory Switch
void
RTIambassador::disableAttributeScopeAdvisorySwitch()
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;
    req.type = Message::DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH ;
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Enable Interaction Relevance Advisory Switch
void
RTIambassador::enableInteractionRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;
    req.type = Message::ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH ;
    executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Interaction Relevance Advisory Switch
void
RTIambassador::disableInteractionRelevanceAdvisorySwitch()
    throw (FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError,
           UnimplementedService)
{
    throw UnimplementedService();
    Message req, rep ;
    req.type = Message::DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH ;
    executeService(&req, &rep);
}

} // namespace certi

// $Id: SupportServices.cc,v 3.3 2003/10/27 10:42:47 breholee Exp $
