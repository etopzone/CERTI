// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
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
// ----------------------------------------------------------------------------

#include "ObjectSet.hh"

namespace certi {

static pdCDebug D("OBJECTSET", "(ObjectSet) - ");

// ----------------------------------------------------------------------------
ObjectSet::ObjectSet(SecurityServer *the_server)
    : server(the_server)
{
}

// ----------------------------------------------------------------------------
ObjectSet::~ObjectSet(void)
{
    std::map<ObjectHandle, Object *>::iterator i ;

    for (i = begin(); i != end(); i++) {
        delete (*i).second ;
    }
    erase(begin(), end());
}

// ----------------------------------------------------------------------------
void
ObjectSet::changeAttributeTransportationType(ObjectHandle the_object,
                                             AttributeHandle *the_attributes,
                                             UShort the_size,
                                             TransportType the_type)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError,
           InvalidObjectHandle)
{
    Object *object = getObject(the_object);
}

// ----------------------------------------------------------------------------
void
ObjectSet::changeAttributeOrderType(ObjectHandle the_object,
                                    AttributeHandle *the_attributes,
                                    UShort the_size,
                                    TransportType the_type)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError,
           InvalidObjectHandle)
{
    Object *object = getObject(the_object);
}

// ----------------------------------------------------------------------------
ObjectHandle
ObjectSet::getObjectInstanceHandle(const char *the_name) const
    throw (ObjectNotKnown,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    char tmp[MAX_USER_TAG_LENGTH] ;

    std::map<ObjectHandle, Object *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        ((*i).second)->getName(tmp);
        if (strcmp(tmp, the_name) == 0)
            return ((*i).second)->getHandle();
    }

    throw ObjectNotKnown("No object instance with that name.");
}

// ----------------------------------------------------------------------------
char *
ObjectSet::getObjectInstanceName(ObjectHandle the_object) const
    throw (ObjectNotKnown,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Object *object = getObject(the_object);

    char *name = (char *)malloc(MAX_USER_TAG_LENGTH*sizeof(char));
    object->getName(name);

    return name ;
}

// ----------------------------------------------------------------------------
ObjectClassHandle
ObjectSet::getObjectClass(ObjectHandle the_object) const
    throw (ObjectNotKnown,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           RTIinternalError)
{
    Object *object = getObject(the_object);

    return 0 ;
}

// ----------------------------------------------------------------------------
ObjectHandle
ObjectSet::registerObjectInstance(ObjectClassHandle the_class,
                                  const char *the_name)
    throw (ObjectClassNotDefined,
           ObjectClassNotPublished,
           ObjectAlreadyRegistered,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{

    return 0 ;
}

// ----------------------------------------------------------------------------
void
ObjectSet::deleteObjectInstance(ObjectHandle the_object,
                                const char *the_tag)
    throw (ObjectNotKnown,
           DeletePrivilegeNotHeld,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Object *object = getObject(the_object);
}

// ----------------------------------------------------------------------------
Boolean
ObjectSet::isAttributeOwnedByFederate(FederateHandle the_federate,
                                      ObjectHandle the_object,
                                      AttributeHandle the_attribute) const
    throw (ObjectNotKnown,
           AttributeNotDefined,
           RTIinternalError)
{
    Object *object = getObject(the_object);

    return RTI_FALSE ;
}

// ----------------------------------------------------------------------------
void
ObjectSet::queryAttributeOwnership(FederateHandle the_federate,
                                   ObjectHandle the_object,
                                   AttributeHandle the_attribute) const
    throw (ObjectNotKnown,
           AttributeNotDefined,
           RTIinternalError)
{
    Object *object = getObject(the_object);
}

// ----------------------------------------------------------------------------
void ObjectSet::
negotiatedAttributeOwnershipDivestiture(FederateHandle the_federate,
                                        ObjectHandle the_object,
                                        AttributeHandle *the_attributes,
                                        UShort the_size,
                                        const char *the_tag)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           AttributeAlreadyBeingDivested,
           RTIinternalError)
{
    Object *object = getObject(the_object);
}

// ----------------------------------------------------------------------------
void ObjectSet::
attributeOwnershipAcquisitionIfAvailable(FederateHandle the_federate,
                                         ObjectHandle the_object,
                                         AttributeHandle *the_attributes,
                                         UShort the_size)
    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           AttributeAlreadyBeingAcquired,
           RTIinternalError)
{
    Object *object = getObject(the_object);
}

// ----------------------------------------------------------------------------
void ObjectSet::
unconditionalAttributeOwnershipDivestiture(FederateHandle the_federate,
                                           ObjectHandle the_object,
                                           AttributeHandle *the_attributes,
                                           UShort the_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError)
{
    Object *object = getObject(the_object);
}

// ----------------------------------------------------------------------------
void
ObjectSet::attributeOwnershipAcquisition(FederateHandle the_federate,
                                         ObjectHandle the_object,
                                         AttributeHandle *the_attributes,
                                         UShort the_size,
                                         const char *the_tag)

    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           RTIinternalError)
{
    Object *object = getObject(the_object);
}

// ----------------------------------------------------------------------------
void ObjectSet::
cancelNegotiatedAttributeOwnershipDivestiture(FederateHandle the_federate,
                                              ObjectHandle the_object,
                                              AttributeHandle *the_attributes,
                                              UShort the_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           AttributeDivestitureWasNotRequested,
           RTIinternalError)
{
    Object *object = getObject(the_object);
}

// ----------------------------------------------------------------------------
AttributeHandleSet *
ObjectSet::attributeOwnershipReleaseResponse(FederateHandle the_federate,
                                             ObjectHandle the_object,
                                             AttributeHandle *the_attributes,
                                             UShort the_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           FederateWasNotAskedToReleaseAttribute,
           RTIinternalError)
{
    Object *object = getObject(the_object);

    return NULL ;
}

// ----------------------------------------------------------------------------
void
ObjectSet::cancelAttributeOwnershipAcquisition(FederateHandle the_federate,
                                               ObjectHandle the_object,
                                               AttributeHandle *the_attributes,
                                               UShort the_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeAlreadyOwned,
           AttributeAcquisitionWasNotRequested,
           RTIinternalError)
{
    Object *object = getObject(the_object);
}

// ----------------------------------------------------------------------------
Object *
ObjectSet::getObject(ObjectHandle the_object) const
    throw (ObjectNotKnown)
{
    std::map<ObjectHandle, Object *>::const_iterator i ;
    i = find(the_object);

    if (i != end())
        return (*i).second ;

    throw ObjectNotKnown("Object not found in map set.");
}

} // namespace certi

// $Id: ObjectSet.cc,v 3.1 2003/04/22 15:55:57 breholee Exp $
