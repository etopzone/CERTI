// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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

#include "NM_Classes.hh"
#include "Object.hh"
#include "ObjectAttribute.hh"
#include "ObjectClass.hh"
#include "ObjectClassAttribute.hh"
#include "ObjectClassBroadcastList.hh"
#include "ObjectClassSet.hh"

#include "PrettyDebug.hh"
#include "SocketTCP.hh"
#include "helper.hh"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>

#include <include/make_unique.hh>

using std::cout;
using std::endl;

namespace certi {

static PrettyDebug D("OBJECTCLASS", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);

// ----------------------------------------------------------------------------
//! To be used only by CRead, it returns the new Attribute's Handle.
AttributeHandle ObjectClass::addAttribute(ObjectClassAttribute* theAttribute, bool is_inherited)
{
    if (theAttribute == NULL)
        throw RTIinternalError("Tried to add NULL attribute.");

    AttributeHandle attributeHandle = theAttribute->getHandle();
    if (_handleClassAttributeMap.find(attributeHandle) != _handleClassAttributeMap.end()) {
        throw RTIinternalError("ObjectClass::addAttribute: Tried to add attribute with duplicate handle.");
    }

    theAttribute->server = server;

    // If the attribute is inherited, it keeps its security level.
    // If not, it takes the default security level of the class.
    if (!is_inherited)
        theAttribute->level = securityLevelId;

    _handleClassAttributeMap[attributeHandle] = theAttribute;

    Debug(D, pdProtocol) << "ObjectClass " << handle << " has a new attribute " << attributeHandle << std::endl;

    return attributeHandle;
}

// ----------------------------------------------------------------------------
//! Add the class' attributes to the 'Child' Class.
void ObjectClass::addInheritedClassAttributes(ObjectClass* the_child)
{
    for (HandleClassAttributeMap::iterator a = _handleClassAttributeMap.begin(); a != _handleClassAttributeMap.end();
         ++a) {
        assert(a->second != NULL);

        ObjectClassAttribute* childAttribute = new ObjectClassAttribute(*a->second);
        assert(childAttribute != NULL);

        Debug(D, pdProtocol) << "ObjectClass " << handle << " adding new attribute " << a->second->getHandle()
                             << " to child class " << the_child->getHandle() << std::endl;

        the_child->addAttribute(childAttribute, true);
    }
} /* end of addInheritedClassAttributes */

// ----------------------------------------------------------------------------
/*! Take a Broadcast List and continue to send messages. You should
  take a look at ObjectClassSet::RegisterObject to understand
  what is going on...
*/
Responses ObjectClass::broadcastClassMessage(ObjectClassBroadcastList* ocbList, const Object* source)
{
    Debug(G, pdGendoc) << "enter ObjectClass::broadcastClassMessage" << std::endl;
    // 1. Set ObjectHandle to local class Handle.
    ocbList->upcastTo(handle);

    Debug(G, pdGendoc) << "      ObjectClass::broadcastClassMessage handle " << handle << std::endl;
    // 2. Update message attribute list by removing child's attributes.
    if ((ocbList->getMsg().getMessageType() == NetworkMessage::Type::REFLECT_ATTRIBUTE_VALUES)) {
        for (uint32_t attr = 0; attr < (ocbList->getMsgRAV()->getAttributesSize());) {
            // If the attribute is not in that class, remove it from the message.
            if (hasAttribute(ocbList->getMsgRAV()->getAttributes(attr))) {
                ++attr;
            }
            else {
                ocbList->getMsgRAV()->removeAttributes(attr);
            }
        }
    }
    if ((ocbList->getMsg().getMessageType() == NetworkMessage::Type::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION)) {
        for (uint32_t attr = 0; attr < (ocbList->getMsgRAV()->getAttributesSize());) {
            // If the attribute is not in that class, remove it from the message.
            if (hasAttribute(ocbList->getMsgRAOA()->getAttributes(attr))) {
                ++attr;
            }
            else {
                ocbList->getMsgRAOA()->removeAttributes(attr);
            }
        }
    }
    // 3. Add class/attributes subscribers to the list.
    switch (ocbList->getMsg().getMessageType()) {
    case NetworkMessage::Type::DISCOVER_OBJECT:
    case NetworkMessage::Type::REMOVE_OBJECT: {
        // For each federate, add it to list if at least one attribute has
        // been subscribed.
        FederateHandle federate = 0;
        for (federate = 1; federate <= maxSubscriberHandle; federate++) {
            if (isSubscribed(federate) && (federate != (ocbList->getMsg().getFederate()))) {
                ocbList->addFederate(federate);
            }
        }
    } break;

    case NetworkMessage::Type::REFLECT_ATTRIBUTE_VALUES: {
        // For each class attribute, update the list by adding federates who
        // subscribed to the attribute.
        for (uint32_t i = 0; i < ocbList->getMsgRAV()->getAttributesSize(); ++i) {
            AttributeHandle attributeHandle = ocbList->getMsgRAV()->getAttributes(i);

            HandleClassAttributeMap::iterator a = _handleClassAttributeMap.find(attributeHandle);
            // May be this is a hard error?
            if (a == _handleClassAttributeMap.end()) {
                continue;
            }

            ObjectAttribute* attr = source->getAttribute(attributeHandle);
            const RTIRegion* update_region = attr->getRegion();
            Debug(D, pdTrace) << "RAV: attr " << attributeHandle << " / region "
                              << (update_region ? update_region->getHandle() : 0) << std::endl;
            a->second->updateBroadcastList(ocbList, update_region);
        }
    } break;

    case NetworkMessage::Type::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION: {
        // For each class attribute, update the list be adding federates who
        // subscribed to the attribute.
        for (HandleClassAttributeMap::iterator i = _handleClassAttributeMap.begin();
             i != _handleClassAttributeMap.end();
             ++i) {
            i->second->updateBroadcastList(ocbList);
        }
    } break;

    default:
        throw RTIinternalError("BroadcastClassMsg: Unexpected message type.");
    }
    // 4. Send pending messages.
    auto ret = ocbList->preparePendingMessage(*server);
    Debug(G, pdGendoc) << "exit  ObjectClass::broadcastClassMessage" << std::endl;
    return ret;
}

// ----------------------------------------------------------------------------
//! sendToFederate.
void ObjectClass::sendToFederate(NetworkMessage* msg, FederateHandle theFederate)
{
    // Send the message 'msg' to the Federate which Handle is theFederate.
    Socket* socket = NULL;
    try {
#ifdef HLA_USES_UDP
        socket = server->getSocketLink(theFederate, BEST_EFFORT);
#else
        socket = server->getSocketLink(theFederate);
#endif
        msg->send(socket, NM_msgBufSend);
    }
    catch (RTIinternalError& e) {
        Debug(D, pdExcept) << "Reference to a killed Federate while broadcasting" << std::endl;
    }
    catch (NetworkError& e) {
        Debug(D, pdExcept) << "Network error while broadcasting, ignoring" << std::endl;
    }
    // BUG: If except = 0, could use Multicast.
}

// ----------------------------------------------------------------------------
void ObjectClass::sendToOwners(CDiffusion& diffusionList,
                               Object* object,
                               FederateHandle theFederate,
                               const std::string& theTag,
                               NM_Attribute_Ownership_Base& answer)
{
    int nbAttributes = diffusionList.size();

    FederateHandle toFederate;
    for (int i = 0; i < nbAttributes; i++) {
        toFederate = diffusionList[i].federate;
        if (toFederate != 0) {
            answer.setFederation(server->federation().get());
            answer.setFederate(theFederate);
            answer.setObject(object->getHandle());
            answer.setLabel(theTag);
            answer.setAttributesSize(nbAttributes);

            int index = 0;
            for (int j = i; j < nbAttributes; j++) {
                if (diffusionList[j].federate == toFederate) {
                    Debug(D, pdDebug) << "handle : " << diffusionList[j].attribute << std::endl;
                    diffusionList[j].federate = 0;
                    answer.setAttributes(diffusionList[j].attribute, index);
                    index++;
                }
            }
            Debug(D, pdDebug) << "Envoi message type " << answer.getMessageName() << std::endl;
            answer.setAttributesSize(index);
            sendToFederate(&answer, toFederate);
        }
    }
} /* end if sendToOwners */

// ----------------------------------------------------------------------------
/*! Throw SecurityError if the Federate is not allowed to access the
  Object Class, and print an Audit message containing reason.
*/
void ObjectClass::checkFederateAccess(FederateHandle the_federate, const std::string& reason)
{
    Debug(D, pdInit) << "Beginning of CheckFederateAccess for the federate " << the_federate << std::endl;

    // BUG: Should at least but a line in Audit
    if (server == NULL) {
        return;
    }

    bool result = server->canFederateAccessData(the_federate, securityLevelId);

    // BUG: Should use Audit.
    if (!result) {
        cout << "Object Class " << handle << " : SecurityError for federate " << the_federate << '(' << reason << ")."
             << endl;
        throw SecurityError("Federate should not access Object Class.");
    }
}

// ----------------------------------------------------------------------------
ObjectClass::ObjectClass(const std::string& name, ObjectClassHandle handle)
    : Named(name)
    , server(NULL)
    , handle(handle)
    , maxSubscriberHandle(0)
    , securityLevelId(PublicLevelID)
    , superClass(0)
    , subClasses(NULL)
{
    subClasses = new ObjectClassSet(NULL);
}

// ----------------------------------------------------------------------------
//! ObjectClass destructor (frees allocated memory).
ObjectClass::~ObjectClass()
{
    // Deleting instances
    if (!_handleObjectMap.empty()) {
        Debug(D, pdError) << "ObjectClass " << handle << " : Instances remaining while exiting..." << std::endl;
    }

    // Deleting Class Attributes
    for (HandleClassAttributeMap::iterator i = _handleClassAttributeMap.begin(); i != _handleClassAttributeMap.end();
         ++i) {
        delete i->second;
    }
    _handleClassAttributeMap.clear();

    // Deleting subclasses
    delete subClasses;
} /* end of ObjectClass destructor */

// ----------------------------------------------------------------------------
/* Delete Instance with time.
   Delete the object instance 'theObjectHandle', and starts to
   broadcast the RemoveObject message. Return a BroadcastList of
   Federates, in order to allow our ObjectClassSet to go on with
   the message broadcasting, by giving the list to our parent
   class.
*/
std::pair<ObjectClassBroadcastList*, Responses> ObjectClass::deleteInstance(FederateHandle the_federate,
                                                                            Object* object,
                                                                            const FederationTime& theTime,
                                                                            const std::string& the_tag)
{
    Responses ret;

    // Is the Federate really the Object Owner?(Checked only on RTIG)
    if ((server != 0) && (object->getOwner() != the_federate)) {
        Debug(D, pdExcept) << "Delete Object " << object->getHandle() << ": Federate " << the_federate << " not owner."
                           << std::endl;
        throw DeletePrivilegeNotHeld("");
    }

    // 2. Remove Instance from list.
    HandleObjectMap::iterator o = _handleObjectMap.find(object->getHandle());
    if (o != _handleObjectMap.end()) {
        _handleObjectMap.erase(o);
    }

    // 3. Prepare and broadcast message.
    ObjectClassBroadcastList* ocbList = NULL;
    if (server != NULL) {
        Debug(D, pdRegister) << "Object " << object->getHandle() << " deleted in class " << handle
                             << ", now broadcasting..." << std::endl;

        auto answer = make_unique<NM_Remove_Object>();
        answer->setFederation(server->federation().get());
        answer->setFederate(the_federate);
        answer->setException(Exception::Type::NO_EXCEPTION);
        answer->setObjectClass(handle); // Class Handle FIXME why this
        answer->setObject(object->getHandle());

        // with time
        answer->setDate(theTime);
        answer->setLabel(the_tag);

        ocbList = new ObjectClassBroadcastList(std::move(answer));
        ret = broadcastClassMessage(ocbList);
    }
    else {
        Debug(D, pdRegister) << "Object " << object->getHandle() << " deleted in class " << handle
                             << ", no broadcast to do." << std::endl;
    }

    // Return the BroadcastList in case it had to be passed to the parent
    // class.
    return {ocbList, std::move(ret)};
}

// ----------------------------------------------------------------------------
/* Delete Instance without time.
   Delete the object instance 'theObjectHandle', and starts to
   broadcast the RemoveObject message. Return a BroadcastList of
   Federates, in order to allow our ObjectClassSet to go on with
   the message broadcasting, by giving the list to our parent
   class.
*/
std::pair<ObjectClassBroadcastList*, Responses>
ObjectClass::deleteInstance(FederateHandle the_federate, Object* object, const std::string& the_tag)
{
    Responses ret;
    // Is the Federate really the Object Owner?(Checked only on RTIG)
    if ((server != 0) && (object->getOwner() != the_federate)) {
        Debug(D, pdExcept) << "Delete Object " << object->getHandle() << ": Federate " << the_federate << " not owner."
                           << std::endl;
        throw DeletePrivilegeNotHeld("");
    }

    // 2. Remove Instance from list.
    HandleObjectMap::iterator o = _handleObjectMap.find(object->getHandle());
    if (o != _handleObjectMap.end()) {
        _handleObjectMap.erase(o);
    }

    // 3. Prepare and broadcast message.
    ObjectClassBroadcastList* ocbList = NULL;
    if (server != NULL) {
        Debug(D, pdRegister) << "Object " << object->getHandle() << " deleted in class " << handle
                             << ", now broadcasting..." << std::endl;

        auto answer = make_unique<NM_Remove_Object>();
        answer->setFederation(server->federation().get());
        answer->setFederate(the_federate);
        answer->setObjectClass(handle); // Class Handle FIXME why do we have a class handle in REMOVE OBJECT?
        answer->setObject(object->getHandle());

        // without time
        answer->setLabel(the_tag);

        ocbList = new ObjectClassBroadcastList(std::move(answer));
        ret = broadcastClassMessage(ocbList);
    }
    else {
        Debug(D, pdRegister) << "Object " << object->getHandle() << " deleted in class " << handle
                             << ", no broadcast to do." << std::endl;
    }

    // Return the BroadcastList in case it had to be passed to the parent
    // class.
    return {ocbList, std::move(ret)};
}

// ----------------------------------------------------------------------------
//! Print the ObjectClasses tree to the standard output.
void ObjectClass::display() const
{
    cout << " ObjectClass #" << handle << " \"" << name << "\":" << endl;

    // Display inheritance
    cout << " Parent Class Handle: " << superClass << endl;
    cout << " Security Level: " << securityLevelId << endl;
    cout << " " << subClasses->size() << " Child(s):" << endl;
    cout << " Subclasses handles:";
    ObjectClassSet::const_iterator i;
    for (i = subClasses->begin(); i != subClasses->end(); ++i) {
        cout << " " << i->second->getHandle() << endl;
    }

    // Display Attributes
    cout << " " << _handleClassAttributeMap.size() << " Attribute(s):" << endl;
    for (HandleClassAttributeMap::const_iterator i = _handleClassAttributeMap.begin();
         i != _handleClassAttributeMap.end();
         ++i) {
        i->second->display();
    }

    // Display Instances
    cout << " " << _handleObjectMap.size() << " Instances(s):" << endl;
    for (HandleObjectMap::const_iterator i = _handleObjectMap.begin(); i != _handleObjectMap.end(); ++i) {
        i->second->display();
    }
}

// ----------------------------------------------------------------------------
//! getAttributeHandle.
AttributeHandle ObjectClass::getAttributeHandle(const std::string& the_name) const
{
    Debug(G, pdGendoc) << "enter ObjectClass::getAttributeHandle" << std::endl;

    for (HandleClassAttributeMap::const_iterator i = _handleClassAttributeMap.begin();
         i != _handleClassAttributeMap.end();
         ++i) {
        if (the_name == i->second->getName()) {
            Debug(G, pdGendoc) << "exit  ObjectClass::getAttributeHandle" << std::endl;
            return i->second->getHandle();
        }
    }

    Debug(D, pdExcept) << "ObjectClass " << handle << ": Attribute \"" << the_name << "\" not defined." << std::endl;
    Debug(G, pdGendoc) << "exit  ObjectClass::getAttributeHandle on NameNotFound" << std::endl;
    throw NameNotFound(the_name);
}

// ----------------------------------------------------------------------------
//! getAttributeName.
const std::string& ObjectClass::getAttributeName(AttributeHandle the_handle) const
{
    return getAttribute(the_handle)->getName();
}

// ----------------------------------------------------------------------------
/** Get attribute
    @param the_handle Attribute's handle
    @return attribute
 */
ObjectClassAttribute* ObjectClass::getAttribute(AttributeHandle the_handle) const
{
    HandleClassAttributeMap::const_iterator i = _handleClassAttributeMap.find(the_handle);
    if (i != _handleClassAttributeMap.end()) {
        return i->second;
    }

    Debug(D, pdExcept) << "ObjectClass " << handle << ": Attribute " << the_handle << " not defined." << std::endl;

    throw AttributeNotDefined("ObjectClass::getAttribute(AttributeHandle) Attribute <" + std::to_string(the_handle)
                              + "> unknown for ObjectClass <"
                              + getName()
                              + ">.");
}

// ----------------------------------------------------------------------------
//! Return true if the attribute with the given handle is an attribute of this object class
bool ObjectClass::hasAttribute(AttributeHandle attributeHandle) const
{
    return _handleClassAttributeMap.find(attributeHandle) != _handleClassAttributeMap.end();
}

// ----------------------------------------------------------------------------
//! Return true if the Federate is publishing any attribute of this class.
bool ObjectClass::isFederatePublisher(FederateHandle the_federate) const
{
    Debug(D, pdRegister) << "_handleClassAttributeMap.size() = " << _handleClassAttributeMap.size() << std::endl;

    for (HandleClassAttributeMap::const_iterator i = _handleClassAttributeMap.begin();
         i != _handleClassAttributeMap.end();
         ++i) {
        if (i->second->isPublishing(the_federate)) {
            return true;
        }
    }
    return false;
}

// ----------------------------------------------------------------------------
/** Indicates whether a federate subscribed to this class, with any
    attribute and any region
    @param fed federate to check for subscription
    @return true if the federate is subscribed
 */
bool ObjectClass::isSubscribed(FederateHandle fed) const
{
    for (HandleClassAttributeMap::const_iterator i = _handleClassAttributeMap.begin();
         i != _handleClassAttributeMap.end();
         ++i) {
        if (i->second->isSubscribed(fed)) {
            return true;
        }
    }
    return false;
}

// ----------------------------------------------------------------------------
/*! Return 'true' if the object instance designated by 'theID' is
  present in that class.
*/
bool ObjectClass::isInstanceInClass(ObjectHandle objectHandle)
{
    return _handleObjectMap.find(objectHandle) != _handleObjectMap.end();
}

// ----------------------------------------------------------------------------
//! killFederate.
std::pair<ObjectClassBroadcastList*, Responses> ObjectClass::killFederate(FederateHandle the_federate) noexcept
{
    Debug(D, pdRegister) << "Object Class " << handle << ": Killing Federate " << the_federate << std::endl;

    std::vector<AttributeHandle> liste_vide;
    try {
        // Does federate is publishing something ? (not important)
        if (isFederatePublisher(the_federate)) {
            publish(the_federate, liste_vide, false);
        }

        // Does federate subscribed something ?
        // Problem, no removeObject or discover must not be sent to it.
        if (isSubscribed(the_federate)) {
            unsubscribe(the_federate);
        }
    }
    catch (SecurityError& e) {
    }

    // Does federate owns instances ?
    for (HandleObjectMap::const_iterator i = _handleObjectMap.begin(); i != _handleObjectMap.end(); ++i) {
        if (i->second->getOwner() == the_federate) {
            // Return non-NULL to indicate that :
            // 1- A RemoveObject message should be broadcasted through parent
            // class
            // 2- The federate may own another instance, and this function
            // must be called again.
            // BUG: String \/
            return deleteInstance(the_federate, i->second, "Killed");
        }
    }

    Debug(D, pdRegister) << "Object Class " << handle << ":Federate " << the_federate << " killed" << std::endl;

    // Return NULL if the Federate did not own any instance.
    return {nullptr, Responses()};
}

// ----------------------------------------------------------------------------
//! publish.
void ObjectClass::publish(FederateHandle theFederateHandle,
                          const std::vector<AttributeHandle>& theAttributeList,
                          bool PubOrUnpub)
{
    Debug(D, pdInit) << "Beginning of Publish for the federate " << theFederateHandle << std::endl;

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (std::vector<AttributeHandle>::const_iterator it = theAttributeList.begin(); it != theAttributeList.end();
         ++it) {
        getAttribute(*it);
    }

    // Check Security Levels
    checkFederateAccess(theFederateHandle, "Publish");

    // Reset all previous publish information.
    Debug(D, pdInit) << "ObjectClass " << handle << ": Reset publish info of Federate " << theFederateHandle
                     << std::endl;

    for (HandleClassAttributeMap::iterator i = _handleClassAttributeMap.begin(); i != _handleClassAttributeMap.end();
         ++i) {
        if (i->second->isPublishing(theFederateHandle)) {
            i->second->unpublish(theFederateHandle);
        }
    }

    // Publish attributes one by one.
    ObjectClassAttribute* attribute;
    for (std::vector<AttributeHandle>::const_iterator it = theAttributeList.begin(); it != theAttributeList.end();
         ++it) {
        Debug(D, pdInit) << "ObjectClass " << handle << ": Federate " << theFederateHandle << " publishes attribute "
                         << *it << std::endl;
        attribute = getAttribute(*it);
        if (PubOrUnpub) {
            attribute->publish(theFederateHandle);
        }
        else {
            attribute->unpublish(theFederateHandle);
        }
    }
}

// ----------------------------------------------------------------------------
/*! Register a new object instance, and start to broadcast the DiscoverObject
  Message to class subscribers. Return a Broadcast List of Federates, in
  order to allow our ObjectClassSet to go on with the message broadcasting,
  by giving the list to our parent class.
*/
std::pair<ObjectClassBroadcastList*, Responses>
ObjectClass::registerObjectInstance(FederateHandle the_federate, Object* the_object, ObjectClassHandle /*classHandle*/)
{
    Responses ret;
    // Pre-conditions checking
    if (isInstanceInClass(the_object->getHandle())) {
        Debug(D, pdExcept) << "exception : ObjectAlreadyRegistered." << std::endl;
        throw ObjectAlreadyRegistered(the_object->getName());
    }

    // This condition is only to be checked on the RTIG
    if ((server != NULL) && (!isFederatePublisher(the_federate))) {
        Debug(D, pdExcept) << "exception : ObjectClassNotPublished." << std::endl;
        throw ObjectClassNotPublished("");
    }

    // Ownership management :
    // Copy instance attributes
    // Federate only owns attributes it publishes.
    for (HandleClassAttributeMap::iterator i = _handleClassAttributeMap.begin(); i != _handleClassAttributeMap.end();
         ++i) {
        ObjectAttribute* oa;
        oa = new ObjectAttribute(
            i->second->getHandle(), i->second->isPublishing(the_federate) ? the_federate : 0, i->second);

        // privilegeToDelete is owned by federate even not published.
        if (i->second->isNamed("privilegeToDelete")) {
            oa->setOwner(the_federate);
        }

        the_object->addAttribute(oa);
    }

    _handleObjectMap[the_object->getHandle()] = the_object;
    Debug(D, pdTrace) << "Added object " << the_object->getHandle() << "/" << _handleObjectMap.size() << " to class "
                      << handle << std::endl;

    // Prepare and Broadcast message for this class
    ObjectClassBroadcastList* ocbList = NULL;
    if (server != NULL) {
        Debug(D, pdRegister) << "Object " << the_object->getHandle() << " registered in class " << handle
                             << ", now broadcasting..." << std::endl;

        auto answer = make_unique<NM_Discover_Object>();
        answer->setFederation(server->federation().get());
        answer->setFederate(the_federate);
        answer->setException(Exception::Type::NO_EXCEPTION);
        answer->setObjectClass(handle); // Class Handle
        answer->setObject(the_object->getHandle());
        answer->setLabel(the_object->getName());
        // BUG FIXME strange!!
        //answer->setDate(0.0);

        ocbList = new ObjectClassBroadcastList(std::move(answer));
        ret = broadcastClassMessage(ocbList);
    }
    else {
        Debug(D, pdRegister) << "Object " << the_object->getHandle() << " registered in class " << handle
                             << ", no broadcast to do." << std::endl;
    }

    // Return the BroadcastList in case it had to be passed to the parent
    // class.
    return {ocbList, std::move(ret)};
}

// ----------------------------------------------------------------------------
/** Send a "Discover Object" message to a federate for each object of
    this class, if the federate was not already subscribed. Subclass
    objects are not considered. Objects may actually be of a
    superclass.
    @param federate Federate to send discovery messages to
    @param super_handle Handle of the class of objects to be
    discovered
    @return true if the process should be applied to subclasses too
 */
bool ObjectClass::sendDiscoverMessages(FederateHandle federate, ObjectClassHandle super_handle)
{
    // If we are in a subclass to which the federate is already subscribed,
    if ((handle != super_handle) && isSubscribed(federate))
        return false;

    // Else, send message for each object
    for (HandleObjectMap::const_iterator i = _handleObjectMap.begin(); i != _handleObjectMap.end(); ++i) {
        if (i->second->getOwner() != federate) {
            NM_Discover_Object message;
            Debug(D, pdInit) << "Sending DiscoverObj to Federate " << federate << " for Object "
                             << i->second->getHandle() << " in class " << handle << std::endl;

            message.setFederation(server->federation().get());
            message.setFederate(federate);
            message.setException(Exception::Type::NO_EXCEPTION);
            message.setObjectClass(super_handle);
            message.setObject(i->second->getHandle());
            message.setLabel(i->second->getName());
            //BUG FIXME strange!!
            //message.setDate(0.0);

            Socket* socket = NULL;
            try {
                socket = server->getSocketLink(federate);
                message.send(socket, NM_msgBufSend);
            }
            catch (RTIinternalError& e) {
                Debug(D, pdExcept) << "Reference to a killed Federate while sending DO msg." << std::endl;
            }
            catch (NetworkError& e) {
                Debug(D, pdExcept) << "Network error while sending DO msg, ignoring." << std::endl;
            }
        }
    }

    return true;
}

// A class' LevelID can only be increased.
void ObjectClass::setSecurityLevelId(SecurityLevelID levelID)
{
    if (!server->dominates(levelID, securityLevelId)) {
        throw SecurityError("Attempt to lower object class level.");
    }

    securityLevelId = levelID;
}

// ----------------------------------------------------------------------------
bool ObjectClass::subscribe(FederateHandle fed, const std::vector<AttributeHandle>& attributes, const RTIRegion* region)
{
    uint32_t nb_attributes = attributes.size();
    checkFederateAccess(fed, "Subscribe");

    // Verify all attributes
    for (std::vector<AttributeHandle>::const_iterator it = attributes.begin(); it != attributes.end(); ++it) {
        // may throw AttributeNotDefined
        getAttribute(*it);
    }

    if (nb_attributes > 0) {
        maxSubscriberHandle = std::max(fed, maxSubscriberHandle);
    }

    bool was_subscriber = isSubscribed(fed);

    /* FIXME what does this means?
     * We first unsubscribe the federate from any attribute he may have
     * subscribed to in order to resubscribe to the new specified set of attribute.
     */
    unsubscribe(fed, region);

    Debug(D, pdTrace) << "ObjectClass::subscribe"
                      << " : fed " << fed << ", class " << handle << ", " << nb_attributes << " attributes, region "
                      << (region ? region->getHandle() : 0) << std::endl;

    /* This loop will be void if the federate did "unsubscribe" */
    for (std::vector<AttributeHandle>::const_iterator it = attributes.begin(); it != attributes.end(); ++it) {
        getAttribute(*it)->subscribe(fed, region);
    }

    return (attributes.size() > 0) && !was_subscriber;
} /* end of subscribe */

// ----------------------------------------------------------------------------
//! update Attribute Values with time.
std::pair<ObjectClassBroadcastList*, Responses>
ObjectClass::updateAttributeValues(FederateHandle the_federate,
                                   Object* object,
                                   const std::vector<AttributeHandle>& the_attributes,
                                   const std::vector<AttributeValue_t>& the_values,
                                   int the_size,
                                   FederationTime the_time,
                                   const std::string& the_tag)
{
    Responses ret;

    // Ownership management: Test ownership on each attribute before updating.
    ObjectAttribute* oa;
    for (int i = 0; i < the_size; i++) {
        oa = object->getAttribute(the_attributes[i]);
        if (oa->getOwner() != the_federate)
            throw AttributeNotOwned("Federate <" + std::to_string(the_federate) + "> is not owner of attribute <"
                                    + std::to_string(oa->getHandle())
                                    + ">");
    }

    // Prepare and Broadcast message for this class
    ObjectClassBroadcastList* ocbList = NULL;
    if (server != NULL) {
        auto answer = make_unique<NM_Reflect_Attribute_Values>();
        answer->setFederation(server->federation().get());
        answer->setFederate(the_federate);
        answer->setException(Exception::Type::NO_EXCEPTION);
        answer->setObject(object->getHandle());
        // with time
        answer->setDate(the_time);
        answer->setLabel(the_tag);
        answer->setAttributesSize(the_size);
        answer->setValuesSize(the_size);

        for (int32_t i = 0; i < the_size; i++) {
            answer->setAttributes(the_attributes[i], i);
            answer->setValues(the_values[i], i);
        }

        ocbList = new ObjectClassBroadcastList(std::move(answer), _handleClassAttributeMap.size());

        Debug(D, pdProtocol) << "Object " << object->getHandle() << " updated in class " << handle
                             << ", now broadcasting..." << std::endl;

        ret = broadcastClassMessage(ocbList, object);
    }
    else {
        Debug(D, pdExcept) << "UpdateAttributeValues should not be called on the RTIA." << std::endl;
        throw RTIinternalError("UpdateAttributeValues called on the RTIA.");
    }

    // Return the BroadcastList in case it had to be passed to the parent
    // class.
    return {ocbList, std::move(ret)};
}

// ----------------------------------------------------------------------------
//! update Attribute Values without time.
std::pair<ObjectClassBroadcastList*, Responses>
ObjectClass::updateAttributeValues(FederateHandle the_federate,
                                   Object* object,
                                   const std::vector<AttributeHandle>& the_attributes,
                                   const std::vector<AttributeValue_t>& the_values,
                                   int the_size,
                                   const std::string& the_tag)
{
    Responses ret;
    // Ownership management: Test ownership on each attribute before updating.
    ObjectAttribute* oa;
    for (int i = 0; i < the_size; i++) {
        oa = object->getAttribute(the_attributes[i]);

        if (oa->getOwner() != the_federate) {
            throw AttributeNotOwned("Attribute #" + std::to_string(the_attributes[i]) + " is not owned by federate #"
                                    + std::to_string(the_federate));
        }
    }

    // Prepare and Broadcast message for this class
    ObjectClassBroadcastList* ocbList = NULL;
    if (server != NULL) {
        auto answer = make_unique<NM_Reflect_Attribute_Values>();
        answer->setFederation(server->federation().get());
        answer->setFederate(the_federate);
        answer->setException(Exception::Type::NO_EXCEPTION);
        answer->setObject(object->getHandle());
        // without time

        answer->setLabel(the_tag);

        answer->setAttributesSize(the_size);
        answer->setValuesSize(the_size);

        for (int32_t i = 0; i < the_size; i++) {
            answer->setAttributes(the_attributes[i], i);
            answer->setValues(the_values[i], i);
        }

        ocbList = new ObjectClassBroadcastList(std::move(answer), _handleClassAttributeMap.size());

        Debug(D, pdProtocol) << "Object " << object->getHandle() << " updated in class " << handle
                             << ", now broadcasting..." << std::endl;

        ret = broadcastClassMessage(ocbList, object);
    }
    else {
        Debug(D, pdExcept) << "UpdateAttributeValues should not be called on the RTIA." << std::endl;
        throw RTIinternalError("UpdateAttributeValues called on the RTIA.");
    }

    // Return the BroadcastList in case it had to be passed to the parent
    // class.
    return {ocbList, std::move(ret)};
}

// ----------------------------------------------------------------------------
//! negotiatedAttributeOwnershipDivestiture.
std::pair<ObjectClassBroadcastList*, Responses>
ObjectClass::negotiatedAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                                     Object* object,
                                                     const std::vector<AttributeHandle>& theAttributeList,
                                                     const std::string& theTag)
{
    Responses ret;
    // Pre-conditions checking

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (unsigned index = 0; index < theAttributeList.size(); index++)
        getAttribute(theAttributeList[index]);

    // Does federate owns every attributes.
    // Does federate has called NegotiatedAttributeOwnershipDivestiture.
    Debug(D, pdDebug) << "NegotiatedDivestiture requested by " << theFederateHandle << std::endl;

    ObjectAttribute* oa;
    ObjectClassAttribute* oca;
    for (unsigned i = 0; i < theAttributeList.size(); i++) {
        oca = getAttribute(theAttributeList[i]);
        oa = object->getAttribute(theAttributeList[i]);

        Debug(D, pdDebug) << "Attribute Name : " << oca->getName() << std::endl;
        Debug(D, pdDebug) << "Attribute Handle : " << oa->getHandle() << std::endl;
        Debug(D, pdDebug) << "Attribute Owner : " << oa->getOwner() << std::endl;
        if (oa->getOwner() != theFederateHandle) {
            throw AttributeNotOwned("");
        }
        if (oa->beingDivested()) {
            throw AttributeAlreadyBeingDivested("");
        }
    }

    int acquisition_count = 0;
    int assumption_count = 0;
    int divestiture_count = 0;
    ObjectClassBroadcastList* List = NULL;
    FederateHandle NewOwner;

    if (server != NULL) {
        auto answerAssumption = make_unique<NM_Request_Attribute_Ownership_Assumption>();
        NM_Attribute_Ownership_Divestiture_Notification AnswerDivestiture;

        answerAssumption->setAttributesSize(theAttributeList.size());

        CDiffusion diffusionAcquisition;

        ObjectAttribute* oa;
        ObjectClassAttribute* oca;
        for (uint32_t i = 0; i < theAttributeList.size(); i++) {
            oa = object->getAttribute(theAttributeList[i]);
            if (oa->hasCandidates()) {
                oca = getAttribute(theAttributeList[i]);
                // An attributeOwnershipAcquisition is on the way
                // with this attribute.

                // The more recent ownership candidate (requester) becomes owner
                NewOwner = oa->getFirstCandidate();

                oa->setOwner(NewOwner);

                // Suppress him from candidate list
                oa->removeCandidate(NewOwner);

                // Reinitialise divesting
                oa->setDivesting(false);

                ++acquisition_count;
                diffusionAcquisition.push_back(DiffusionPair(NewOwner, oa->getHandle()));

                AnswerDivestiture.setAttributes(theAttributeList[i], divestiture_count);
                divestiture_count++;
                /* FIXME not sure that this should be done */
                if (oca->isNamed("privilegeToDelete")) {
                    object->setOwner(NewOwner);
                }
            }
            else {
                answerAssumption->setAttributes(theAttributeList[i], assumption_count);
                oa->setDivesting(true);
                assumption_count++;
            }
        }

        if (acquisition_count != 0) {
            NM_Attribute_Ownership_Acquisition_Notification AOAN;
            sendToOwners(diffusionAcquisition, object, theFederateHandle, theTag, AOAN);
        }

        if (divestiture_count != 0) {
            AnswerDivestiture.setFederation(server->federation().get());
            AnswerDivestiture.setFederate(theFederateHandle);
            AnswerDivestiture.setObject(object->getHandle());
            AnswerDivestiture.setLabel(std::string());
            AnswerDivestiture.setAttributesSize(divestiture_count);
            sendToFederate(&AnswerDivestiture, theFederateHandle);
        }

        if (assumption_count != 0) {
            answerAssumption->setFederation(server->federation().get());
            answerAssumption->setFederate(theFederateHandle);
            answerAssumption->setException(Exception::Type::NO_EXCEPTION);
            answerAssumption->setObject(object->getHandle());
            answerAssumption->setLabel(theTag);
            answerAssumption->setAttributesSize(assumption_count);

            List = new ObjectClassBroadcastList(std::move(answerAssumption), _handleClassAttributeMap.size());

            Debug(D, pdProtocol) << "Object " << object->getHandle() << " divestiture in class " << handle
                                 << ", now broadcasting..." << std::endl;
            ret = broadcastClassMessage(List);
        }
    }
    else {
        Debug(D, pdExcept) << "NegotiatedAttributeOwnershipDivestiture should not be called on the RTIA." << std::endl;
        throw RTIinternalError("NegotiatedAttributeOwnershipDivestiture called on the RTIA.");
    }

    // Return the BroadcastList in case it had to be passed to the parent class.
    return {List, std::move(ret)};
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisitionIfAvailable.
void ObjectClass::attributeOwnershipAcquisitionIfAvailable(FederateHandle the_federate,
                                                           Object* object,
                                                           const std::vector<AttributeHandle>& the_attributes)
{
    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (unsigned index = 0; index < the_attributes.size(); index++) {
        getAttribute(the_attributes[index]);
    }

    if (server) {
        // Federate must publish the class.
        if (!isFederatePublisher(the_federate)) {
            Debug(D, pdExcept) << "exception : ObjectClassNotPublished." << std::endl;
            throw ObjectClassNotPublished("");
        }

        //rem _handleClassAttributeMap.size()=attributeState.size()
        for (unsigned i = 0; i < the_attributes.size(); i++) {
            ObjectClassAttribute* oca = getAttribute(the_attributes[i]);
            ObjectAttribute* oa = object->getAttribute(the_attributes[i]);

            // The federate has to publish attributes he desire to
            // acquire.
            if (!oca->isPublishing(the_federate) && (!oca->isNamed("privilegeToDelete")))
                throw AttributeNotPublished("");
            // Does federate already owns some attributes.
            if (oa->getOwner() == the_federate)
                throw FederateOwnsAttributes("");
            // Does federate is on the way to acquire something.
            if (oa->isCandidate(the_federate))
                throw AttributeAlreadyBeingAcquired("");
        }

        NM_Attribute_Ownership_Acquisition_Notification* Answer_notification
            = new NM_Attribute_Ownership_Acquisition_Notification();
        Answer_notification->setFederation(server->federation().get());
        Answer_notification->setFederate(the_federate);
        Answer_notification->setException(Exception::Type::NO_EXCEPTION);
        Answer_notification->setObject(object->getHandle());
        Answer_notification->setAttributesSize(the_attributes.size());

        NM_Attribute_Ownership_Unavailable* Answer_unavailable = new NM_Attribute_Ownership_Unavailable();
        Answer_unavailable->setFederation(server->federation().get());
        Answer_unavailable->setFederate(the_federate);
        Answer_unavailable->setException(Exception::Type::NO_EXCEPTION);
        Answer_unavailable->setObject(object->getHandle());
        Answer_unavailable->setAttributesSize(the_attributes.size());

        CDiffusion diffusionDivestiture;

        //
        //Ce service ne doit pas ajouter le federe demandeur a la liste
        //des candidats!!!
        //
        int compteur_unavailable = 0;
        int compteur_notification = 0;
        int compteur_divestiture = 0;
        FederateHandle oldOwner;

        for (uint32_t i = 0; i < the_attributes.size(); i++) {
            ObjectClassAttribute* oca = getAttribute(the_attributes[i]);
            ObjectAttribute* oa = object->getAttribute(the_attributes[i]);

            oldOwner = oa->getOwner();
            if ((oldOwner == 0) || (oa->beingDivested())) {
                //Cet attribut est libre ou offert par son proprietaire
                //S'il est offert
                if (oa->beingDivested()) {
                    ++compteur_divestiture;
                    diffusionDivestiture.push_back(DiffusionPair(oldOwner, oa->getHandle()));
                }
                //Qu'il soit offert ou libre
                Answer_notification->setAttributes(the_attributes[i], compteur_notification);
                oa->setOwner(the_federate);
                oa->setDivesting(false);
                compteur_notification++;
                //object->Owner reste le champ de reference
                //pour le privilegeToDelete
                if (oca->isNamed("privilegeToDelete"))
                    object->setOwner(the_federate);
            }
            else {
                //Cet attribut n'est pas disponible!!!
                Answer_unavailable->setAttributes(the_attributes[i], compteur_unavailable);
                oa->addCandidate(the_federate);
                compteur_unavailable++;
            }
        }

        if (compteur_notification != 0) {
            Answer_notification->setAttributesSize(compteur_notification);
            sendToFederate(Answer_notification, the_federate);
        }
        else
            delete Answer_notification;

        Debug(D, pdDebug) << "Start: send divestiture notification message" << std::endl;

        if (compteur_divestiture != 0) {
            NM_Attribute_Ownership_Divestiture_Notification AODN;
            sendToOwners(diffusionDivestiture, object, the_federate, "\0", AODN);
        }

        if (compteur_unavailable != 0) {
            Answer_unavailable->setAttributesSize(compteur_unavailable);
            sendToFederate(Answer_unavailable, the_federate);
        }
        else
            delete Answer_unavailable;
    }
    else {
        Debug(D, pdExcept) << "AttributeOwnershipAcquisitionIfAvailable should not be called on the RTIA." << std::endl;
        throw RTIinternalError("AttributeOwnershipAcquisitionIfAvailable called on the RTIA.");
    }
}

// ----------------------------------------------------------------------------
//! unconditionalAttributeOwnershipDivestiture.
std::pair<ObjectClassBroadcastList*, Responses> ObjectClass::unconditionalAttributeOwnershipDivestiture(
    FederateHandle theFederateHandle, Object* object, const std::vector<AttributeHandle>& theAttributeList)
{
    Responses ret;

    // Pre-conditions checking
    // Do all attribute handles exist?
    // If not then AttributeNotDefined will be thrown.
    for (unsigned index = 0; index < theAttributeList.size(); index++) {
        getAttribute(theAttributeList[index]);
    }

    // Is the federate the owner of all the attributes?
    ObjectAttribute* oa;
    for (unsigned i = 0; i < theAttributeList.size(); i++) {
        oa = object->getAttribute(theAttributeList[i]);
        if (oa->getOwner() != theFederateHandle)
            throw AttributeNotOwned("federate <" + std::to_string(theFederateHandle) + "> not owner of attribute <"
                                    + std::to_string(oa->getHandle())
                                    + ">, it is=<"
                                    + std::to_string(oa->getOwner())
                                    + ">.");
    }

    int assumption_count = 0;
    ObjectClassBroadcastList* List = NULL;
    FederateHandle NewOwner;

    if (server != NULL) {
        auto answerAssumption = make_unique<NM_Unconditional_Attribute_Ownership_Divestiture>();
        answerAssumption->setAttributesSize(theAttributeList.size());
        CDiffusion diffusionAcquisition;

        for (unsigned i = 0; i < theAttributeList.size(); i++) {
            ObjectClassAttribute* oca = getAttribute(theAttributeList[i]);
            ObjectAttribute* oa = object->getAttribute(theAttributeList[i]);

            if (oa->hasCandidates()) {
                // An attributeOwnershipAcquisition is on the way
                // on this attribute.

                // Le demandeur le plus recent devient proprietaire
                NewOwner = oa->getFirstCandidate();

                oa->setOwner(NewOwner);

                // We remove it from candidate list.
                oa->removeCandidate(NewOwner);

                // We reinitialize divesting.
                oa->setDivesting(false);

                diffusionAcquisition.push_back(DiffusionPair(NewOwner, oa->getHandle()));

                if (oca->isNamed("privilegeToDelete")) {
                    object->setOwner(NewOwner);
                }
            }
            else {
                answerAssumption->setAttributes(theAttributeList[i], assumption_count);
                oa->setOwner(0);
                oa->setDivesting(false);
                assumption_count++;
            }
        }

        if (assumption_count != 0) {
            answerAssumption->setFederation(server->federation().get());
            answerAssumption->setFederate(theFederateHandle);
            answerAssumption->setException(Exception::Type::NO_EXCEPTION);
            answerAssumption->setObject(object->getHandle());
            answerAssumption->setLabel(std::string());
            answerAssumption->setAttributesSize(assumption_count);

            List = new ObjectClassBroadcastList(std::move(answerAssumption), _handleClassAttributeMap.size());

            Debug(D, pdProtocol) << "Object " << object->getHandle() << " updated in class " << handle
                                 << ", now broadcasting..." << std::endl;

            ret = broadcastClassMessage(List);
        }

        if (!diffusionAcquisition.empty()) {
            NM_Attribute_Ownership_Acquisition_Notification AOAN;
            sendToOwners(diffusionAcquisition, object, theFederateHandle, "\0", AOAN);
        }
    }
    else {
        Debug(D, pdExcept) << "UnconditionalAttributeOwnershipDivestiture should not be called on the RTIA."
                           << std::endl;
        throw RTIinternalError("UnconditionalAttributeOwnershipDivestiture called on the RTIA.");
    }

    // Return the BroadcastList in case it had to be passed to the parent
    // class.
    //! List could be equal to NULL
    return {List, std::move(ret)};
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisition.
void ObjectClass::attributeOwnershipAcquisition(FederateHandle theFederateHandle,
                                                Object* object,
                                                const std::vector<AttributeHandle>& theAttributeList,
                                                const std::string& theTag)
{
    // Verification of the conditions should be done before any changes!
    // Pre-conditions checking
    for (unsigned i = 0; i < theAttributeList.size(); i++) {
        // Do all attribute handles exist ? It may throw AttributeNotDefined.
        ObjectClassAttribute* oca = getAttribute(theAttributeList[i]);
        ObjectAttribute* oa = object->getAttribute(theAttributeList[i]);
        // Does federate own this attribute?
        if (oa->getOwner() == theFederateHandle)
            throw FederateOwnsAttributes("");
        // Does federate publish the attribute?
        if (!(oca->isPublishing(theFederateHandle) || oca->isNamed("privilegeToDelete")))
            throw AttributeNotPublished("");
    }

    int notification_counter = 0;
    FederateHandle oldOwner;
    if (server != NULL) {
        // The federate have to publish the class
        if (!isFederatePublisher(theFederateHandle)) {
            Debug(D, pdExcept) << "exception : ObjectClassNotPublished." << std::endl;
            throw ObjectClassNotPublished("");
        }
        NM_Attribute_Ownership_Acquisition_Notification* AnswerNotification
            = new NM_Attribute_Ownership_Acquisition_Notification();

        AnswerNotification->setFederation(server->federation().get());
        AnswerNotification->setFederate(theFederateHandle);
        AnswerNotification->setException(Exception::Type::NO_EXCEPTION);
        AnswerNotification->setObject(object->getHandle());
        AnswerNotification->setAttributesSize(theAttributeList.size());

        /*! TODO: remove size field from the CDiffusion -
	 * it copies std::vector functionality.
	 * It could be reasonably to replace the whole structure with
	 * a single typedef std::vector<>
	 */
        CDiffusion diffusionDivestiture;
        CDiffusion diffusionRelease;

        for (unsigned i = 0; i < theAttributeList.size(); i++) {
            ObjectClassAttribute* oca = getAttribute(theAttributeList[i]);
            ObjectAttribute* oa = object->getAttribute(theAttributeList[i]);
            oldOwner = oa->getOwner();
            if ((oldOwner == 0) || (oa->beingDivested())) {
                // This attribute is devested or is offered by its owner
                if (oa->beingDivested()) {
                    diffusionDivestiture.push_back(DiffusionPair(oldOwner, oa->getHandle()));
                }
                //Qu'il soit offert ou libre
                if (oa->isCandidate(theFederateHandle))
                    oa->removeCandidate(theFederateHandle);
                AnswerNotification->setAttributes(theAttributeList[i], notification_counter);
                oa->setOwner(theFederateHandle);
                oa->setDivesting(false);
                notification_counter++;

                // object->Owner reste le champ de reference pour
                // le privilegeToDelete
                if (oca->isNamed("privilegeToDelete"))
                    object->setOwner(theFederateHandle);
            }
            else {
                diffusionRelease.push_back(DiffusionPair(oldOwner, oa->getHandle()));
                // Moving federate to the front position of candidate list
                oa->removeCandidate(theFederateHandle);
                oa->addCandidate(theFederateHandle);
            }
        }
        if (notification_counter != 0) {
            AnswerNotification->setAttributesSize(notification_counter);
            sendToFederate(AnswerNotification, theFederateHandle);
        }
        else
            delete AnswerNotification;

        if (!diffusionDivestiture.empty()) {
            NM_Attribute_Ownership_Divestiture_Notification AODN;
            sendToOwners(diffusionDivestiture, object, theFederateHandle, "\0", AODN);
        }

        if (!diffusionRelease.empty()) {
            NM_Request_Attribute_Ownership_Release RAOR;
            sendToOwners(diffusionRelease, object, theFederateHandle, theTag, RAOR);
        }
    }
    else {
        Debug(D, pdExcept) << "AttributeOwnershipAcquisition should not be called on the RTIA." << std::endl;
        throw RTIinternalError("AttributeOwnershipAcquisition called on the RTIA");
    }
}

// ----------------------------------------------------------------------------
//! attributeOwnershipReleaseResponse.
AttributeHandleSet* ObjectClass::attributeOwnershipReleaseResponse(FederateHandle the_federate,
                                                                   Object* object,
                                                                   const std::vector<AttributeHandle>& the_attributes)
{
    // Pre-conditions checking

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (unsigned index = 0; index < the_attributes.size(); index++) {
        getAttribute(the_attributes[index]);
    }

    //Le f�d�r� est-il propri�taire de tous les attributs
    //Y a-t-il des acqu�reurs pour les attributs
    for (unsigned i = 0; i < the_attributes.size(); i++) {
        ObjectAttribute* oa = object->getAttribute(the_attributes[i]);

        if (oa->getOwner() != the_federate) {
            throw AttributeNotOwned("");
        }
        if (!oa->hasCandidates()) {
            throw FederateWasNotAskedToReleaseAttribute("");
        }
    }

    int compteur_acquisition = 0;
    FederateHandle newOwner;
    AttributeHandleSet* theAttribute;
    if (server != NULL) {
        CDiffusion diffusionAcquisition;

        theAttribute = new AttributeHandleSet(the_attributes.size());

        for (unsigned i = 0; i < the_attributes.size(); i++) {
            ObjectClassAttribute* oca = getAttribute(the_attributes[i]);
            ObjectAttribute* oa = object->getAttribute(the_attributes[i]);

            //Le demandeur le plus r�cent devient propri�taire
            newOwner = oa->getFirstCandidate();

            oa->setOwner(newOwner);

            //On le supprime de la liste des demandeurs
            oa->removeCandidate(newOwner);

            //On r�initialise divesting
            oa->setDivesting(false);

            diffusionAcquisition.push_back(DiffusionPair(newOwner, oa->getHandle()));
            theAttribute->add(oa->getHandle());

            Debug(D, pdDebug) << "Acquisition handle " << the_attributes[i] << " counter " << compteur_acquisition
                              << std::endl;

            if (oca->isNamed("privilegeToDelete")) {
                object->setOwner(newOwner);
            }
        }

        if (!diffusionAcquisition.empty()) {
            NM_Attribute_Ownership_Acquisition_Notification AOAN;
            sendToOwners(diffusionAcquisition, object, the_federate, "\0", AOAN);
        }
    }
    else {
        Debug(D, pdExcept) << "NegotiatedAttributeOwnershipDivestiture should not be called on the RTIA." << std::endl;
        throw RTIinternalError("NegotiatedAttributeOwnershipDivestiture called on the RTIA.");
    }
    return (theAttribute);
}

// ----------------------------------------------------------------------------
//! cancelAttributeOwnershipAcquisition.
void ObjectClass::cancelAttributeOwnershipAcquisition(FederateHandle federate_handle,
                                                      Object* object,
                                                      const std::vector<AttributeHandle>& attribute_list)
{
    // Pre-conditions checking

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (unsigned index = 0; index < attribute_list.size(); index++)
        getAttribute(attribute_list[index]);

    for (unsigned i = 0; i < attribute_list.size(); i++)
        Debug(D, pdDebug) << "CancelAcquisition Object " << object->getHandle() << " Attribute " << attribute_list[i]
                          << std::endl;

    if (server != NULL) {
        //rem _handleClassAttributeMap.size()=attributeState.size()

        for (unsigned i = 0; i < attribute_list.size(); i++) {
            ObjectAttribute* oa = object->getAttribute(attribute_list[i]);

            Debug(D, pdDebug) << "Attribute " << attribute_list[i] << " Owner " << oa->getOwner() << std::endl;
            // Does federate is already owning some attributes ?
            if (oa->getOwner() == federate_handle) {
                throw AttributeAlreadyOwned("");
            }
            // Does federate is already doing an acquisition ?
            if (!oa->isCandidate(federate_handle)) {
                throw AttributeAcquisitionWasNotRequested("");
            }
        }

        NM_Confirm_Attribute_Ownership_Acquisition_Cancellation* answer_confirmation
            = new NM_Confirm_Attribute_Ownership_Acquisition_Cancellation();
        answer_confirmation->setFederation(server->federation().get());
        answer_confirmation->setFederate(federate_handle);
        answer_confirmation->setException(Exception::Type::NO_EXCEPTION);
        answer_confirmation->setObject(object->getHandle());
        answer_confirmation->setAttributesSize(attribute_list.size());

        int compteur_confirmation = 0;
        for (unsigned i = 0; i < attribute_list.size(); i++) {
            ObjectAttribute* oa = object->getAttribute(attribute_list[i]);

            answer_confirmation->setAttributes(attribute_list[i], compteur_confirmation);

            // We remove federate from candidates.
            oa->removeCandidate(federate_handle);
            Debug(D, pdDebug) << "Adding federate " << federate_handle << " to attribute " << attribute_list[i]
                              << " object " << object->getHandle() << std::endl;
            ++compteur_confirmation;
        }

        if (compteur_confirmation != 0) {
            answer_confirmation->setAttributesSize(compteur_confirmation);
            sendToFederate(answer_confirmation, federate_handle);
        }
        else
            delete answer_confirmation;
    }
    else {
        Debug(D, pdExcept) << "CancelAttributeOwnershipAcquisition should not be called on the RTIA." << std::endl;
        throw RTIinternalError("CancelAttributeOwnershipAcquisition called on the RTIA.");
    }
}

// ----------------------------------------------------------------------------
ObjectClassHandle ObjectClass::getHandle() const
{
    return handle;
}

// ----------------------------------------------------------------------------
/** Unsubscribe this federate/region pair
 */
void ObjectClass::unsubscribe(FederateHandle fed, const RTIRegion* region)
{
    Debug(D, pdTrace) << "ObjectClass::unsubscribe: fed " << fed << ", region " << (region ? region->getHandle() : 0)
                      << std::endl;

    for (HandleClassAttributeMap::iterator i = _handleClassAttributeMap.begin(); i != _handleClassAttributeMap.end();
         ++i) {
        if (i->second->isSubscribed(fed, region)) {
            i->second->unsubscribe(fed, region);
        }
    }
}

// ----------------------------------------------------------------------------
/** Unsubscribe this federate, for all regions currently subscribed
 */
void ObjectClass::unsubscribe(FederateHandle fed)
{
    for (HandleClassAttributeMap::iterator i = _handleClassAttributeMap.begin(); i != _handleClassAttributeMap.end();
         ++i) {
        if (i->second->isSubscribed(fed)) {
            i->second->unsubscribe(fed);
        }
    }
} /* end of unsubscribe */

void ObjectClass::addSubClass(ObjectClass* child)
{
    /* build parent-child relationship */
    /* add child as subclass of parent */
    subClasses->addClass(child, NULL);
    /* link child to parent */
    child->superClass = handle;
    /* forward inherited properties to child */
    /* Add Object Class Attribute */
    addInheritedClassAttributes(child);
    /* security server is the same for child */
    child->server = server;
    /* inherit security Level */
    child->setSecurityLevelId(securityLevelId);

} /* end of addChild */

// ----------------------------------------------------------------------------
/** Recursively start discovery of existing objects.
    @param federate FederateHandle to send the discovery message to
    @param subscription ObjectClassHandle of the class actually subscribed by the federate
 */
void ObjectClass::recursiveDiscovering(FederateHandle federate, ObjectClassHandle subscription)
{
    Debug(D, pdInit) << "Recursive Discovering on class " << handle << " for Federate " << federate << "." << std::endl;

    bool go_deeper = sendDiscoverMessages(federate, subscription);

    if (go_deeper) {
        ObjectClassSet::const_iterator i;
        for (i = subClasses->begin(); i != subClasses->end(); ++i) {
            i->second->recursiveDiscovering(federate, subscription);
        }
    }
}

// ----------------------------------------------------------------------------
/** Apply a visitor object to the class hierarchy
    @param v Visitor, its execute() function is called with current object, and
    is expected to return 'true' if the process should continue on subclasses
 */
// void
// ObjectClass::apply(Visitor &v)
// {
//     bool go_deeper = v.execute(*this);

//     if (go_deeper) {
//         list<ObjectClass *>::const_iterator i ;
//         for (i = subClasses.begin(); i != subClasses.end(); ++i) {
//             (*i)->apply(v);
//         }
//     }
// }

} // namespace certi
