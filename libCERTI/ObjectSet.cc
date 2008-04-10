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
// ----------------------------------------------------------------------------



// Project
#include "Object.hh"
#include "ObjectAttribute.hh"
#include "ObjectSet.hh"
#include "PrettyDebug.hh"
#include "NM_Classes.hh"

// Standard
#include <iostream>
#include <sstream>

using std::pair ;
using std::cout ;
using std::endl ;
using std::ostringstream ;
using std::string ;

namespace certi {

static pdCDebug D("OBJECTSET", "(ObjectSet) - ");
static PrettyDebug G("GENDOC",__FILE__);

// ----------------------------------------------------------------------------
ObjectSet::ObjectSet(SecurityServer *the_server)
    : server(the_server)
{
}

// ----------------------------------------------------------------------------
ObjectSet::~ObjectSet()
{
    std::map<ObjectHandle, Object *>::iterator i ;

    for (i = begin(); i != end(); i++) {
        delete i->second ;
    }
    erase(begin(), end());
}

// ----------------------------------------------------------------------------
void
ObjectSet::changeAttributeTransportationType(ObjectHandle,
                                             AttributeHandle *,
                                             UShort,
                                             TransportType)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError,
           InvalidObjectHandle)
{
    // Object *object = getObject(the_object);
}

// ----------------------------------------------------------------------------
void
ObjectSet::changeAttributeOrderType(ObjectHandle,
                                    AttributeHandle *,
                                    UShort,
                                    TransportType)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError,
           InvalidObjectHandle)
{
    // Object *object = getObject(the_object);
}

// ----------------------------------------------------------------------------
ObjectHandle
ObjectSet::getObjectInstanceHandle(std::string the_name) const
    throw (ObjectNotKnown, RTIinternalError)
{
	std::stringstream msg;
	
    std::map<ObjectHandle, Object *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        if (i->second->getName() == the_name)
            return i->second->getHandle();
    }

	msg << "No object instance with name <" << the_name;
    throw ObjectNotKnown(msg.str().c_str());
}

// ----------------------------------------------------------------------------
const char *
ObjectSet::getObjectInstanceName(ObjectHandle the_object) const
    throw (ObjectNotKnown, RTIinternalError)
{
    Object *object = getObject(the_object);

    return object->getName().c_str();
}

// ----------------------------------------------------------------------------
ObjectClassHandle
ObjectSet::getObjectClass(ObjectHandle the_object) const
    throw (ObjectNotKnown, FederateNotExecutionMember,
           ConcurrentAccessAttempted, RTIinternalError)
{
    return getObject(the_object)->getClass();
}

// ----------------------------------------------------------------------------
Object *
ObjectSet::registerObjectInstance(FederateHandle the_federate,
				  ObjectClassHandle the_class,
                                  ObjectHandle the_object,
                                  std::string the_name)
    throw (ObjectAlreadyRegistered, ConcurrentAccessAttempted,
           SaveInProgress, RestoreInProgress, RTIinternalError)
{
    const_iterator i ;

    i = find(the_object);

    if (i != end()) {
        throw ObjectAlreadyRegistered("Object already in ObjectSet map.");
    }

    if (the_name.size() > 0) {
        for (i = begin(); i != end(); i++) {
            if (i->second->getName() == the_name)
                throw ObjectAlreadyRegistered("Object name already defined.");
        }
    }

    Object *object = new Object(the_federate);
    object->setHandle(the_object);
    object->setClass(the_class);

    if (the_name.size() > 0) {
        object->setName(the_name);
    }
    else {
        ostringstream tmp ;
        tmp << "HLAobject_" << the_object ;
        object->setName(tmp.str());
    }

    pair<ObjectHandle, Object *> tmp(the_object, object);
    insert(tmp);

    return object ;
}

// ----------------------------------------------------------------------------
void
ObjectSet::deleteObjectInstance(FederateHandle,
                                ObjectHandle the_object,
                                const char *)
    throw (ObjectNotKnown,
           DeletePrivilegeNotHeld,
           FederateNotExecutionMember,
           ConcurrentAccessAttempted,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    Object *object = getObject(the_object);

    delete object ; // Remove the Object instance.

    std::map<ObjectHandle, Object *>::erase(the_object);
}

// ----------------------------------------------------------------------------
void
ObjectSet::killFederate(FederateHandle the_federate)
    throw (RTIinternalError)
{
    std::map<ObjectHandle, Object *>::iterator i ;
    for (i = begin(); i != end(); i++) {
        if ((i->second)->getOwner() == the_federate) {
            std::map<ObjectHandle, Object *>::erase(i);
            i = begin();
        }
    }
}

// ----------------------------------------------------------------------------
bool
ObjectSet::isAttributeOwnedByFederate(FederateHandle the_federate,
                                      ObjectHandle the_object,
                                      AttributeHandle the_attribute) const
    throw (ObjectNotKnown, AttributeNotDefined, RTIinternalError)
{
    D.Out(pdDebug, "isAttributeOwnedByFederate called for attribute %u, "
          "objet %u", the_attribute, the_object);

    Object *object = getObject(the_object);

    if (server == 0) {
        throw RTIinternalError("isAttributeOwnedByFederate not called by RTIG");
    }

    return object->isAttributeOwnedByFederate(the_federate, the_attribute);
}

// ----------------------------------------------------------------------------
void
ObjectSet::queryAttributeOwnership(FederateHandle the_federate,
                                   ObjectHandle the_object,
                                   AttributeHandle the_attribute) const
    throw (ObjectNotKnown, AttributeNotDefined, RTIinternalError)
{
    Object *object = getObject(the_object);

    D.Out(pdDebug, "query attribute ownership for attribute %u and object %u",
          the_attribute, the_object);

    if (server) {
        ObjectAttribute * oa ;
        oa = object->getAttribute(the_attribute);

        NetworkMessage *answer;
        if (oa->getOwner()) {
        	answer = NM_Factory::create(NetworkMessage::INFORM_ATTRIBUTE_OWNERSHIP);
        } else {
        	answer = NM_Factory::create(NetworkMessage::ATTRIBUTE_IS_NOT_OWNED);
        }
        
        answer->federation = server->federation();
        answer->exception = e_NO_EXCEPTION ;
        answer->object = the_object ;
        answer->handleArray[0] = the_attribute ;
        answer->federate = oa->getOwner();
        
        sendToFederate(answer, the_federate);
    }
    else {
        D.Out(pdDebug, "Should only be called by RTIG");
    }
}

// ----------------------------------------------------------------------------
void ObjectSet::
negotiatedAttributeOwnershipDivestiture(FederateHandle,
                                        ObjectHandle,
                                        AttributeHandle *,
                                        UShort,
                                        const char *)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           AttributeAlreadyBeingDivested,
           RTIinternalError)
{
    // Object *object = getObject(the_object);
}

// ----------------------------------------------------------------------------
void ObjectSet::
attributeOwnershipAcquisitionIfAvailable(FederateHandle,
                                         ObjectHandle,
                                         AttributeHandle *,
                                         UShort)
    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           AttributeAlreadyBeingAcquired,
           RTIinternalError)
{
    // Object *object = getObject(the_object);
}

// ----------------------------------------------------------------------------
void ObjectSet::
unconditionalAttributeOwnershipDivestiture(FederateHandle,
                                           ObjectHandle,
                                           AttributeHandle *,
                                           UShort)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError)
{
    // Object *object = getObject(the_object);
}

// ----------------------------------------------------------------------------
void
ObjectSet::attributeOwnershipAcquisition(FederateHandle,
                                         ObjectHandle,
                                         AttributeHandle *,
                                         UShort,
                                         const char *)

    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           RTIinternalError)
{
    // Object *object = getObject(the_object);
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

    ObjectAttribute * oa ;
    for (int i = 0 ; i < the_size ; i++) {
        oa = object->getAttribute(the_attributes[i]);

        // Does federate owns every attributes.
        if (oa->getOwner() != the_federate)
            throw AttributeNotOwned("");
        // Does federate called NegotiatedAttributeOwnershipDivestiture
        if (!oa->beingDivested())
            throw AttributeDivestitureWasNotRequested("");
    }

    if (server != NULL) {
        for (int i = 0 ; i < the_size ; i++) {
            oa = object->getAttribute(the_attributes[i]);
            oa->setDivesting(false);
        }
    }
    else {
        D.Out(pdExcept, "CancelNegotiatedAttributeOwnershipDivestiture should "
              "not be called on the RTIA.");
        throw RTIinternalError("CancelNegotiatedAttributeOwnershipDivestiture "
                               "called on the RTIA.");
    }
}

// ----------------------------------------------------------------------------
AttributeHandleSet *
ObjectSet::attributeOwnershipReleaseResponse(FederateHandle,
                                             ObjectHandle,
                                             AttributeHandle *,
                                             UShort)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           FederateWasNotAskedToReleaseAttribute,
           RTIinternalError)
{
    // Object *object = getObject(the_object);

    return 0 ;
}

// ----------------------------------------------------------------------------
void
ObjectSet::cancelAttributeOwnershipAcquisition(FederateHandle,
                                               ObjectHandle,
                                               AttributeHandle *,
                                               UShort)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeAlreadyOwned,
           AttributeAcquisitionWasNotRequested,
           RTIinternalError)
{
    // Object *object = getObject(the_object);
}

// ----------------------------------------------------------------------------
Object *
ObjectSet::getObject(ObjectHandle the_object) const
    throw (ObjectNotKnown)
{
    std::map<ObjectHandle, Object *>::const_iterator i ;
    i = find(the_object);

    if (i != end())
        return i->second ;

    throw ObjectNotKnown("Object not found in map set.");
}

// ----------------------------------------------------------------------------
//! sendToFederate.
void
ObjectSet::sendToFederate(NetworkMessage *msg,
                          FederateHandle the_federate) const
{
    // Send the message 'msg' to the Federate which Handle is theFederate.
    Socket *socket = NULL ;
    try {
#ifdef HLA_USES_UDP
        socket = server->getSocketLink(the_federate, BEST_EFFORT);
#else
        socket = server->getSocketLink(the_federate);
#endif
        msg->send(socket);
    }
    catch (RTIinternalError &e) {
        D.Out(pdExcept,
              "Reference to a killed Federate while broadcasting.");
    }
    catch (NetworkError &e) {
        D.Out(pdExcept, "Network error while broadcasting, ignoring.");
    }
    // BUG: If except = 0, could use Multicast.
}
// ----------------------------------------------------------------------------
FederateHandle
ObjectSet::requestObjectOwner(FederateHandle the_federate,
                                  ObjectHandle the_object)
    throw (ObjectNotKnown)
{
    G.Out(pdGendoc,"enter ObjectSet::requestObjectOwner");
    const_iterator i ;
    i = find(the_object);

    if (i == end())
       {
       // Object not found !
       throw ObjectNotKnown("Object not found in ObjectSet map.");
       }

    // Object found, return the owner
    G.Out(pdGendoc,"exit  ObjectSet::requestObjectOwner");
    return ( i->second->getOwner()) ;
}
} // namespace certi

// $Id: ObjectSet.cc,v 3.18.2.2 2008/04/10 11:35:57 erk Exp $
