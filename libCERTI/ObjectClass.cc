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

#include  "Object.hh"
#include  "ObjectAttribute.hh"
#include  "ObjectClass.hh"
#include  "ObjectClassAttribute.hh"
#include  "ObjectClassSet.hh"
#include  "ObjectClassBroadcastList.hh"
#include  "NM_Classes.hh"

#include "SocketTCP.hh"
#include "PrettyDebug.hh"
#include "helper.hh"
#include <sstream>
#include <memory>
#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

namespace certi {

static PrettyDebug D("OBJECTCLASS", __FILE__);
static PrettyDebug G("GENDOC",__FILE__);

// ----------------------------------------------------------------------------
//! To be used only by CRead, it returns the new Attribute's Handle.
AttributeHandle
ObjectClass::addAttribute(ObjectClassAttribute *theAttribute,
                          bool is_inherited)
{
    if (theAttribute == NULL)
        throw RTIinternalError("Tried to add NULL attribute.");
    
    AttributeHandle attributeHandle = theAttribute->getHandle();
    if (_handleClassAttributeMap.find(attributeHandle) != _handleClassAttributeMap.end()) {
        throw RTIinternalError("ObjectClass::addAttribute: Tried to add attribute with duplicate handle.");
    }

    theAttribute->server = server ;

    // If the attribute is inherited, it keeps its security level.
    // If not, it takes the default security level of the class.
    if (!is_inherited)
        theAttribute->level = securityLevelId ;

    _handleClassAttributeMap[attributeHandle] = theAttribute;

    D.Out(pdProtocol, "ObjectClass %u has a new attribute %u.",
          handle, attributeHandle);

    return attributeHandle;
}

// ----------------------------------------------------------------------------
//! Add the class' attributes to the 'Child' Class.
void
ObjectClass::addInheritedClassAttributes(ObjectClass *the_child)
{
    for (HandleClassAttributeMap::iterator a = _handleClassAttributeMap.begin(); a != _handleClassAttributeMap.end(); ++a) {
        assert(a->second != NULL);

        ObjectClassAttribute *childAttribute = new ObjectClassAttribute(*a->second);
        assert(childAttribute != NULL);

        D.Out(pdProtocol,
              "ObjectClass %u adding new attribute %d to child class %u.",
              handle, a->second->getHandle(), the_child->getHandle());

        the_child->addAttribute(childAttribute, true);
    }
} /* end of addInheritedClassAttributes */

// ----------------------------------------------------------------------------
/*! Take a Broadcast List and continue to send messages. You should
  take a look at ObjectClassSet::RegisterObject to understand
  what is going on...
*/
void
ObjectClass::broadcastClassMessage(ObjectClassBroadcastList *ocbList,
				   const Object *source)
{
    G.Out(pdGendoc,"enter ObjectClass::broadcastClassMessage");
    // 1. Set ObjectHandle to local class Handle.
    ocbList->upcastTo(handle) ;

    G.Out(pdGendoc,"      ObjectClass::broadcastClassMessage handle=%d",handle);
    // 2. Update message attribute list by removing child's attributes.
    if ((ocbList->getMsg()->getMessageType() == NetworkMessage::REFLECT_ATTRIBUTE_VALUES) ||
        (ocbList->getMsg()->getMessageType() == NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION)) {
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
    // 3. Add class/attributes subscribers to the list.
    switch(ocbList->getMsg()->getMessageType()) {
      case NetworkMessage::DISCOVER_OBJECT:
      case NetworkMessage::REMOVE_OBJECT: {
          // For each federate, add it to list if at least one attribute has
          // been subscribed.
          FederateHandle federate = 0 ;
          for (federate = 1 ; federate <= maxSubscriberHandle ; federate++) {
              if (isSubscribed(federate) && (federate != (ocbList->getMsg()->getFederate()))) {
                  ocbList->addFederate(federate);
              }
          }
      }
      break ;

      case NetworkMessage::REFLECT_ATTRIBUTE_VALUES: {
          // For each class attribute, update the list by adding federates who
          // subscribed to the attribute.
          for (uint32_t i = 0 ; i < ocbList->getMsgRAV()->getAttributesSize() ; ++i) {
              AttributeHandle attributeHandle = ocbList->getMsgRAV()->getAttributes(i);

              HandleClassAttributeMap::iterator a = _handleClassAttributeMap.find(attributeHandle);
              // May be this is a hard error?
              if (a == _handleClassAttributeMap.end()) {
                  continue;
              }

              ObjectAttribute *attr = source->getAttribute(attributeHandle);
              const RTIRegion *update_region = attr->getRegion();
              Debug(D, pdTrace) << "RAV: attr " << attributeHandle
                                << " / region " << (update_region ? update_region->getHandle() : 0)
                                << std::endl ;
              a->second->updateBroadcastList(ocbList, update_region);
          }
      } break ;

      case NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION: {
          // For each class attribute, update the list be adding federates who
          // subscribed to the attribute.
          for (HandleClassAttributeMap::iterator i = _handleClassAttributeMap.begin(); i != _handleClassAttributeMap.end(); ++i) {
              i->second->updateBroadcastList(ocbList);
          }
      } break ;

      default:
        throw RTIinternalError("BroadcastClassMsg: Unexpected message type.");
    }
    // 4. Send pending messages.
    ocbList->sendPendingMessage(server);
    G.Out(pdGendoc,"exit  ObjectClass::broadcastClassMessage");
}

// ----------------------------------------------------------------------------
//! sendToFederate.
void
ObjectClass::sendToFederate(NetworkMessage *msg, FederateHandle theFederate)
{
    // Send the message 'msg' to the Federate which Handle is theFederate.
    Socket *socket = NULL ;
    try {
#ifdef HLA_USES_UDP
        socket = server->getSocketLink(theFederate, BEST_EFFORT);
#else
        socket = server->getSocketLink(theFederate);
#endif
        msg->send(socket,NM_msgBufSend);
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
void
ObjectClass::sendToOwners(CDiffusion& diffusionList,
                          Object* object,
                          FederateHandle theFederate,
                          const std::string& theTag,
                          NM_Attribute_Ownership_Base& answer)
{
    int nbAttributes = diffusionList.size();

    FederateHandle toFederate ;
    for (int i = 0 ; i < nbAttributes ; i++) {
        toFederate = diffusionList[i].federate ;
        if (toFederate != 0) {
            answer.setFederation(server->federation());
            answer.setFederate(theFederate);
            answer.setObject(object->getHandle());
            answer.setLabel(theTag);
            answer.setAttributesSize(nbAttributes) ;

            int index = 0 ;
            for (int j = i ; j < nbAttributes ; j++) {
                if (diffusionList[j].federate == toFederate) {
                    D.Out(pdDebug, "handle : %u", diffusionList[j].attribute);
                    diffusionList[j].federate = 0 ;
                    answer.setAttributes(diffusionList[j].attribute,index) ;
                    index++ ;
                }
            }
            D.Out(pdDebug, "Envoi message type %s ", answer.getMessageName());
            answer.setAttributesSize(index); 
            sendToFederate(&answer, toFederate);
        }
    }
}  /* end if sendToOwners */

// ----------------------------------------------------------------------------
/*! Throw SecurityError if the Federate is not allowed to access the
  Object Class, and print an Audit message containing reason.
*/
void
ObjectClass::checkFederateAccess(FederateHandle the_federate,
                                 const std::string& reason)
    throw (SecurityError)
{
    D.Out(pdInit, "Beginning of CheckFederateAccess for the federate %d",
          the_federate);

    // BUG: Should at least but a line in Audit
    if (server == NULL)
        return ;

    bool result = server->canFederateAccessData(the_federate, securityLevelId);

    // BUG: Should use Audit.
    if (!result) {
        cout << "Object Class " << handle << " : SecurityError for federate "
             << the_federate << '(' << reason << ")." << endl ;
        throw SecurityError("Federate should not access Object Class.");
    }
}

// ----------------------------------------------------------------------------
ObjectClass::ObjectClass(const std::string& name, ObjectClassHandle handle)
    : Named(name), server(NULL), handle(handle), maxSubscriberHandle(0), securityLevelId(PublicLevelID),
      superClass(0), subClasses(NULL)
{
	subClasses = new ObjectClassSet(NULL);
}

// ----------------------------------------------------------------------------
//! ObjectClass destructor (frees allocated memory).
ObjectClass::~ObjectClass()
{
    // Deleting instances
    if (!_handleObjectMap.empty()) {
        D.Out(pdError,
              "ObjectClass %d : Instances remaining while exiting...", handle);
    }

    // Deleting Class Attributes
    for (HandleClassAttributeMap::iterator i = _handleClassAttributeMap.begin(); i != _handleClassAttributeMap.end(); ++i) {
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
ObjectClassBroadcastList *
ObjectClass::deleteInstance(FederateHandle the_federate,
                            Object* object,
                            const FederationTime& theTime,
                            const std::string& the_tag)
    throw (DeletePrivilegeNotHeld,
           ObjectNotKnown,
           RTIinternalError)
{
    // Is the Federate really the Object Owner?(Checked only on RTIG)
    if ((server != 0) && (object->getOwner() != the_federate)) {
        D.Out(pdExcept, "Delete Object %d: Federate %d not owner.",
              object->getHandle(), the_federate);
        throw DeletePrivilegeNotHeld("");
    }

    // 2. Remove Instance from list.
    HandleObjectMap::iterator o = _handleObjectMap.find(object->getHandle());
    if (o != _handleObjectMap.end()) {
        _handleObjectMap.erase(o);
    }

    // 3. Prepare and broadcast message.
    ObjectClassBroadcastList *ocbList = NULL ;
    if (server != NULL) {
        D.Out(pdRegister,
              "Object %u deleted in class %u, now broadcasting...",
              object->getHandle(), handle);

        NM_Remove_Object *answer = new NM_Remove_Object();
        answer->setFederation(server->federation());
        answer->setFederate(the_federate);
        answer->setException(e_NO_EXCEPTION);
        answer->setObjectClass(handle); // Class Handle FIXME why this
        answer->setObject(object->getHandle());

	// with time
        answer->setDate(theTime);
	    answer->setLabel(the_tag);

        ocbList = new ObjectClassBroadcastList(answer, 0);
        broadcastClassMessage(ocbList);
    }
    else {
        D.Out(pdRegister,
              "Object %u deleted in class %u, no broadcast to do.",
              object->getHandle(), handle);
    }

    // Return the BroadcastList in case it had to be passed to the parent
    // class.
    return ocbList ;
}

// ----------------------------------------------------------------------------
/* Delete Instance without time.
   Delete the object instance 'theObjectHandle', and starts to
   broadcast the RemoveObject message. Return a BroadcastList of
   Federates, in order to allow our ObjectClassSet to go on with
   the message broadcasting, by giving the list to our parent
   class.
*/
ObjectClassBroadcastList *
ObjectClass::deleteInstance(FederateHandle the_federate,
                            Object* object,
                            const std::string& the_tag)
    throw (DeletePrivilegeNotHeld,
           ObjectNotKnown,
           RTIinternalError)
{
    // Is the Federate really the Object Owner?(Checked only on RTIG)
    if ((server != 0) && (object->getOwner() != the_federate)) {
        D.Out(pdExcept, "Delete Object %d: Federate %d not owner.",
              object->getHandle(), the_federate);
        throw DeletePrivilegeNotHeld("");
    }

    // 2. Remove Instance from list.
    HandleObjectMap::iterator o = _handleObjectMap.find(object->getHandle());
    if (o != _handleObjectMap.end()) {
        _handleObjectMap.erase(o);
    }

    // 3. Prepare and broadcast message.
    ObjectClassBroadcastList *ocbList = NULL ;
    if (server != NULL) {
        D.Out(pdRegister,
              "Object %u deleted in class %u, now broadcasting...",
              object->getHandle(), handle);

        NM_Remove_Object *answer = new NM_Remove_Object();
        answer->setFederation(server->federation());
        answer->setFederate(the_federate);
        answer->setObjectClass(handle); // Class Handle FIXME why do we have a class handle in REMOVE OBJECT?
        answer->setObject(object->getHandle());

	    // without time
	    answer->setLabel(the_tag);

        ocbList = new ObjectClassBroadcastList(answer, 0);
        broadcastClassMessage(ocbList);
    }
    else {
        D.Out(pdRegister,
              "Object %u deleted in class %u, no broadcast to do.",
              object->getHandle(), handle);
    }

    // Return the BroadcastList in case it had to be passed to the parent
    // class.
    return ocbList ;
}

// ----------------------------------------------------------------------------
//! Print the ObjectClasses tree to the standard output.
void ObjectClass::display() const
{
    cout << " ObjectClass #" << handle << " \"" << name << "\":" << endl ;

    // Display inheritance
    cout << " Parent Class Handle: " << superClass << endl ;
    cout << " Security Level: " << securityLevelId << endl ;
    cout << " " << subClasses->size() << " Child(s):" << endl ;
    cout << " Subclasses handles:" ;
    ObjectClassSet::const_iterator i ;
    for (i = subClasses->begin(); i != subClasses->end(); ++i) {
    	cout << " " << i->second->getHandle() << endl;
    }


    // Display Attributes
    cout << " " << _handleClassAttributeMap.size() << " Attribute(s):" << endl ;
    for (HandleClassAttributeMap::const_iterator i = _handleClassAttributeMap.begin(); i != _handleClassAttributeMap.end(); ++i) {
        i->second->display();
    }

    // Display Instances
    cout << " " << _handleObjectMap.size() << " Instances(s):" << endl ;
    for (HandleObjectMap::const_iterator i = _handleObjectMap.begin(); i != _handleObjectMap.end(); ++i) {
        i->second->display();
    }
}

// ----------------------------------------------------------------------------
//! getAttributeHandle.
AttributeHandle
ObjectClass::getAttributeHandle(const std::string& the_name) const
    throw (NameNotFound, RTIinternalError)
{
    G.Out(pdGendoc,"enter ObjectClass::getAttributeHandle");

    for (HandleClassAttributeMap::const_iterator i = _handleClassAttributeMap.begin(); i != _handleClassAttributeMap.end(); ++i) {
        if (the_name == i->second->getName()) {
            G.Out(pdGendoc,"exit  ObjectClass::getAttributeHandle");
            return i->second->getHandle();
        }
    }

    D.Out(pdExcept, "ObjectClass %u: Attribute \"%s\" not defined.",
          handle, the_name.c_str());
    G.Out(pdGendoc,"exit  ObjectClass::getAttributeHandle on NameNotFound");
    throw NameNotFound(the_name);
}

// ----------------------------------------------------------------------------
//! getAttributeName.
const std::string&
ObjectClass::getAttributeName(AttributeHandle the_handle) const
    throw (AttributeNotDefined,
           RTIinternalError)
{
    return getAttribute(the_handle)->getName();
}

// ----------------------------------------------------------------------------
/** Get attribute
    @param the_handle Attribute's handle
    @return attribute
 */
ObjectClassAttribute *
ObjectClass::getAttribute(AttributeHandle the_handle) const
    throw (AttributeNotDefined)
{
    HandleClassAttributeMap::const_iterator i = _handleClassAttributeMap.find(the_handle);
    if (i != _handleClassAttributeMap.end()) {
        return i->second;
    }

    D.Out(pdExcept, "ObjectClass %d: Attribute %d not defined.",
          handle, the_handle);

    throw AttributeNotDefined(stringize() << "ObjectClass::getAttribute(AttributeHandle) Attribute <" <<the_handle<<"> unknown for ObjectClass <"<<getName()<<">.");
}

// ----------------------------------------------------------------------------
//! Return true if the attribute with the given handle is an attribute of this object class
bool
ObjectClass::hasAttribute(AttributeHandle attributeHandle) const
{
    return _handleClassAttributeMap.find(attributeHandle) != _handleClassAttributeMap.end();
}

// ----------------------------------------------------------------------------
//! Return true if the Federate is publishing any attribute of this class.
bool
ObjectClass::isFederatePublisher(FederateHandle the_federate) const
{
    D.Out(pdRegister, "_handleClassAttributeMap.size() = %d.", _handleClassAttributeMap.size());

    for (HandleClassAttributeMap::const_iterator i = _handleClassAttributeMap.begin(); i != _handleClassAttributeMap.end(); ++i) {
        if (i->second->isPublishing(the_federate)) {
            return true ;
        }
    }
    return false ;
}

// ----------------------------------------------------------------------------
/** Indicates whether a federate subscribed to this class, with any
    attribute and any region
    @param fed federate to check for subscription
    @return true if the federate is subscribed
 */
bool
ObjectClass::isSubscribed(FederateHandle fed) const
{
    for (HandleClassAttributeMap::const_iterator i = _handleClassAttributeMap.begin(); i != _handleClassAttributeMap.end(); ++i) {
        if (i->second->isSubscribed(fed)) {
            return true ;
        }
    }
    return false ;
}

// ----------------------------------------------------------------------------
/*! Return 'true' if the object instance designated by 'theID' is
  present in that class.
*/
bool
ObjectClass::isInstanceInClass(ObjectHandle objectHandle)
{
    return _handleObjectMap.find(objectHandle) != _handleObjectMap.end();
}

// ----------------------------------------------------------------------------
//! killFederate.
ObjectClassBroadcastList *
ObjectClass::killFederate(FederateHandle the_federate)
    throw ()
{
    D.Out(pdRegister, "Object Class %d: Killing Federate %d.",
          handle, the_federate);
    std::vector <AttributeHandle> liste_vide ;
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
    catch (SecurityError &e) {}

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

    D.Out(pdRegister, "Object Class %d:Federate %d killed.",
          handle, the_federate);

    // Return NULL if the Federate did not own any instance.
    return NULL ;
}

// ----------------------------------------------------------------------------
//! publish.
void
ObjectClass::publish(FederateHandle theFederateHandle,
                     const std::vector <AttributeHandle> &theAttributeList,
                     bool PubOrUnpub)
    throw (AttributeNotDefined,
           RTIinternalError,
           SecurityError)
{
    D.Out(pdInit, "Beginning of Publish for the federate %d",
          theFederateHandle);

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (std::vector <AttributeHandle>::const_iterator it = theAttributeList.begin();
         it != theAttributeList.end(); ++it) {
        getAttribute(*it);
    }

    // Check Security Levels
    checkFederateAccess(theFederateHandle, "Publish");

    // Reset all previous publish information.
    D.Out(pdInit, "ObjectClass %d: Reset publish info of Federate %d.",
          handle, theFederateHandle);

    for (HandleClassAttributeMap::iterator i = _handleClassAttributeMap.begin();
         i != _handleClassAttributeMap.end(); ++i) {
         if (i->second->isPublishing(theFederateHandle)) {
            i->second->unpublish(theFederateHandle);
         }
    }

    // Publish attributes one by one.
    ObjectClassAttribute * attribute ;
    for (std::vector <AttributeHandle>::const_iterator it = theAttributeList.begin();
                 it != theAttributeList.end(); ++it) {
        D.Out(pdInit, "ObjectClass %d: Federate %d publishes attribute %d.",
              handle, theFederateHandle, *it);
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
ObjectClassBroadcastList *
ObjectClass::registerObjectInstance(FederateHandle the_federate,
                                    Object *the_object,
                                    ObjectClassHandle classHandle)
    throw (ObjectClassNotPublished,
           ObjectAlreadyRegistered,
           RTIinternalError)
{
    // Pre-conditions checking
    if (isInstanceInClass(the_object->getHandle())) {
        D.Out(pdExcept, "exception : ObjectAlreadyRegistered.");
        throw ObjectAlreadyRegistered(the_object->getName());
    }

    // This condition is only to be checked on the RTIG
    if ((server != NULL) && (!isFederatePublisher(the_federate))) {
        D.Out(pdExcept, "exception : ObjectClassNotPublished.");
        throw ObjectClassNotPublished("");
    }

    // Ownership management :
    // Copy instance attributes
    // Federate only owns attributes it publishes.
    for (HandleClassAttributeMap::iterator i = _handleClassAttributeMap.begin(); i != _handleClassAttributeMap.end(); ++i) {
        ObjectAttribute * oa ;
	oa = new ObjectAttribute(i->second->getHandle(),
				 i->second->isPublishing(the_federate) ? the_federate : 0,
				 i->second);

        // privilegeToDelete is owned by federate even not published.
        if (i->second->isNamed("privilegeToDelete")) {
            oa->setOwner(the_federate);
        }

        the_object->addAttribute(oa);
    }

    _handleObjectMap[the_object->getHandle()] = the_object;
    Debug(D, pdTrace) << "Added object " << the_object->getHandle() << "/"
	       << _handleObjectMap.size() << " to class " << handle << std::endl ;

    // Prepare and Broadcast message for this class
    ObjectClassBroadcastList *ocbList = NULL ;
    if (server != NULL) {
        D.Out(pdRegister,
              "Object %u registered in class %u, now broadcasting...",
              the_object->getHandle(), handle);

        NM_Discover_Object *answer = new NM_Discover_Object();
        answer->setFederation(server->federation());
        answer->setFederate(the_federate);
        answer->setException(e_NO_EXCEPTION) ;
        answer->setObjectClass(handle); // Class Handle
        answer->setObject(the_object->getHandle());
        answer->setLabel(the_object->getName());
	// BUG FIXME strange!!
	//answer->setDate(0.0);

        ocbList = new ObjectClassBroadcastList(answer, 0);
        broadcastClassMessage(ocbList);
    }
    else {
        D.Out(pdRegister,
              "Object %u registered in class %u, no broadcast to do.",
              the_object->getHandle(), handle);
    }

    // Return the BroadcastList in case it had to be passed to the parent
    // class.
    return ocbList ;
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
bool
ObjectClass::sendDiscoverMessages(FederateHandle federate,
                                  ObjectClassHandle super_handle)
{
    // If we are in a subclass to which the federate is already subscribed,
    if ((handle != super_handle) && isSubscribed(federate))
        return false ;

    // Else, send message for each object
    for (HandleObjectMap::const_iterator i = _handleObjectMap.begin(); i != _handleObjectMap.end(); ++i) {
	if (i->second->getOwner() != federate) {
	    NM_Discover_Object message;
	    D.Out(pdInit,
		  "Sending DiscoverObj to Federate %d for Object %u in class %u ",
		  federate, i->second->getHandle(), handle, message.getLabel().c_str());

	    message.setFederation(server->federation());
	    message.setFederate(federate);
	    message.setException(e_NO_EXCEPTION) ;
	    message.setObjectClass(super_handle);
	    message.setObject(i->second->getHandle());
	    message.setLabel(i->second->getName());
	    //BUG FIXME strange!!
	    //message.setDate(0.0);

	    Socket *socket = NULL ;
	    try {
		socket = server->getSocketLink(federate);
		message.send(socket,NM_msgBufSend);
	    }
	    catch (RTIinternalError &e) {
		D.Out(pdExcept,
		      "Reference to a killed Federate while sending DO msg.");
	    }
	    catch (NetworkError &e) {
		D.Out(pdExcept, "Network error while sending DO msg, ignoring.");
	    }
        }
    }

    return true ;
}

// A class' LevelID can only be increased.
void
ObjectClass::setSecurityLevelId(SecurityLevelID new_levelID) throw (SecurityError)
{
    if (!server->dominates(new_levelID, securityLevelId))
        throw SecurityError("Attempt to lower object class level.");

    securityLevelId = new_levelID ;
}

// ----------------------------------------------------------------------------
bool
ObjectClass::subscribe(FederateHandle fed,
                       const std::vector <AttributeHandle> &attributes,
                       const RTIRegion *region)
throw (AttributeNotDefined, RTIinternalError, SecurityError) {

    uint32_t nb_attributes = attributes.size();
    checkFederateAccess(fed, "Subscribe");

    // Verify all attributes
    for (std::vector<AttributeHandle>::const_iterator it = attributes.begin();
         it != attributes.end(); ++it) {
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

    Debug(D, pdTrace) << "ObjectClass::subscribe" << " : fed " << fed << ", class " << handle
            << ", " << nb_attributes << " attributes, region "
            << (region ? region->getHandle() : 0) << std::endl ;

    /* This loop will be void if the federate did "unsubscribe" */
    for (std::vector<AttributeHandle>::const_iterator it = attributes.begin();
         it != attributes.end(); ++it) {
        getAttribute(*it)->subscribe(fed, region);
    }

    return (attributes.size() > 0) && !was_subscriber ;
} /* end of subscribe */

// ----------------------------------------------------------------------------
//! update Attribute Values with time.
ObjectClassBroadcastList *
ObjectClass::updateAttributeValues(FederateHandle the_federate,
                                   Object *object,
                                   const std::vector <AttributeHandle> &the_attributes,
                                   const std::vector <AttributeValue_t> &the_values,
                                   int the_size,
                                   FederationTime the_time,
                                   const std::string& the_tag)
    throw (AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError,
           InvalidObjectHandle)
{
    // Ownership management: Test ownership on each attribute before updating.
    ObjectAttribute * oa ;
    for (int i = 0 ; i < the_size ; i++) {
        oa = object->getAttribute(the_attributes[i]);
        if (oa->getOwner() != the_federate)
            throw AttributeNotOwned(stringize() << "Federate <"<<the_federate<<"> is not owner of attribute <"<<oa->getHandle()<<">");
    }

    // Prepare and Broadcast message for this class
    ObjectClassBroadcastList *ocbList = NULL ;
    if (server != NULL) {
    	NM_Reflect_Attribute_Values *answer = new NM_Reflect_Attribute_Values();
        answer->setFederation(server->federation());
        answer->setFederate(the_federate);
        answer->setException(e_NO_EXCEPTION) ;
        answer->setObject(object->getHandle());
        // with time
        answer->setDate(the_time);
        answer->setLabel(the_tag);
        answer->setAttributesSize(the_size) ;
        answer->setValuesSize(the_size);

        for (int32_t i = 0 ; i < the_size ; i++) {
            answer->setAttributes(the_attributes[i],i);
            answer->setValues(the_values[i],i) ;
        }

        ocbList = new ObjectClassBroadcastList(answer, _handleClassAttributeMap.size());

        D.Out(pdProtocol,
              "Object %u updated in class %u, now broadcasting...",
              object->getHandle(), handle);

        broadcastClassMessage(ocbList, object);
    }
    else {
        D.Out(pdExcept,
              "UpdateAttributeValues should not be called on the RTIA.");
        throw RTIinternalError("UpdateAttributeValues called on the RTIA.");
    }

    // Return the BroadcastList in case it had to be passed to the parent
    // class.
    return ocbList ;
}

// ----------------------------------------------------------------------------
//! update Attribute Values without time.
ObjectClassBroadcastList *
ObjectClass::updateAttributeValues(FederateHandle the_federate,
                                   Object *object,
                                   const std::vector <AttributeHandle> &the_attributes,
                                   const std::vector <AttributeValue_t> &the_values,
                                   int the_size,
                                   const std::string& the_tag)
    throw (AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError,
           InvalidObjectHandle)
{
    // Ownership management: Test ownership on each attribute before updating.
    ObjectAttribute * oa ;
    for (int i = 0 ; i < the_size ; i++) {
        oa = object->getAttribute(the_attributes[i]);

        if (oa->getOwner() != the_federate)
            throw AttributeNotOwned("");
    }

    // Prepare and Broadcast message for this class
    ObjectClassBroadcastList *ocbList = NULL ;
    if (server != NULL) {
    	NM_Reflect_Attribute_Values *answer = new NM_Reflect_Attribute_Values() ;
        answer->setFederation(server->federation());
        answer->setFederate(the_federate);
        answer->setException(e_NO_EXCEPTION) ;
        answer->setObject(object->getHandle());
        // without time

        answer->setLabel(the_tag);

        answer->setAttributesSize(the_size);
        answer->setValuesSize(the_size);

        for (int32_t i = 0 ; i < the_size ; i++) {
            answer->setAttributes(the_attributes[i],i);
            answer->setValues(the_values[i],i);
        }

        ocbList = new ObjectClassBroadcastList(answer, _handleClassAttributeMap.size());

        D.Out(pdProtocol,
              "Object %u updated in class %u, now broadcasting...",
              object->getHandle(), handle);

        broadcastClassMessage(ocbList, object);
    }
    else {
        D.Out(pdExcept,
              "UpdateAttributeValues should not be called on the RTIA.");
        throw RTIinternalError("UpdateAttributeValues called on the RTIA.");
    }

    // Return the BroadcastList in case it had to be passed to the parent
    // class.
    return ocbList ;
}

// ----------------------------------------------------------------------------
//! negotiatedAttributeOwnershipDivestiture.
ObjectClassBroadcastList * ObjectClass::
negotiatedAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                        Object* object,
                                        const std::vector <AttributeHandle> &theAttributeList,
                                        const std::string& theTag)
    throw (AttributeNotDefined,
           AttributeNotOwned,
           AttributeAlreadyBeingDivested,
           RTIinternalError)
{
    // Pre-conditions checking

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (unsigned index = 0 ; index < theAttributeList.size() ; index++)
        getAttribute(theAttributeList[index]);

    // Does federate owns every attributes.
    // Does federate has called NegotiatedAttributeOwnershipDivestiture.
    D.Out(pdDebug, "NegotiatedDivestiture requested by : %u", theFederateHandle);

    ObjectAttribute * oa ;
    ObjectClassAttribute * oca ;
    for (unsigned i = 0 ; i < theAttributeList.size() ; i++) {
        oca = getAttribute(theAttributeList[i]);
        oa = object->getAttribute(theAttributeList[i]);

        D.Out(pdDebug, "Attribute Name : %s", oca->getName().c_str());
        D.Out(pdDebug, "Attribute Handle : %u", oa->getHandle());
        D.Out(pdDebug, "Attribute Owner : %u", oa->getOwner());
        if (oa->getOwner() != theFederateHandle)
            throw AttributeNotOwned("");
        if (oa->beingDivested())
            throw AttributeAlreadyBeingDivested("");
    }

    int compteur_acquisition = 0 ;
    int compteur_assumption = 0 ;
    int compteur_divestiture = 0 ;
    ObjectClassBroadcastList *List = NULL ;
    FederateHandle NewOwner ;

    if (server != NULL) {
    	NM_Request_Attribute_Ownership_Assumption* AnswerAssumption = new NM_Request_Attribute_Ownership_Assumption();
        NM_Attribute_Ownership_Divestiture_Notification AnswerDivestiture;

        AnswerAssumption->setAttributesSize(theAttributeList.size());

        CDiffusion diffusionAcquisition;

        ObjectAttribute * oa ;
        ObjectClassAttribute * oca ;
        for (uint32_t i = 0 ; i < theAttributeList.size() ; i++) {
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

                ++compteur_acquisition;
                diffusionAcquisition.push_back(DiffusionPair(NewOwner,oa->getHandle()));

                AnswerDivestiture.setAttributes(theAttributeList[i],compteur_divestiture);
                compteur_divestiture++ ;
                /* FIXME not sure that this should be done */
                if (oca->isNamed("privilegeToDelete")) {
                    object->setOwner(NewOwner);
                }
            }
            else {
                AnswerAssumption->setAttributes(theAttributeList[i],compteur_assumption) ;
                oa->setDivesting(true);
                compteur_assumption++ ;
            }
        }

        if (compteur_acquisition != 0) {
        	NM_Attribute_Ownership_Acquisition_Notification AOAN;
            sendToOwners(diffusionAcquisition, object,
                         theFederateHandle, theTag,
                         AOAN);
        }

        if (compteur_divestiture !=0) {
            AnswerDivestiture.setFederation(server->federation());
            AnswerDivestiture.setFederate(theFederateHandle);
            AnswerDivestiture.setObject(object->getHandle());
            AnswerDivestiture.setLabel(std::string());
            AnswerDivestiture.setAttributesSize(compteur_divestiture);
            sendToFederate(&AnswerDivestiture, theFederateHandle);
        }

        if (compteur_assumption !=0) {
            AnswerAssumption->setFederation(server->federation());
            AnswerAssumption->setFederate(theFederateHandle);
            AnswerAssumption->setException(e_NO_EXCEPTION) ;
            AnswerAssumption->setObject(object->getHandle());
            AnswerAssumption->setLabel(theTag);
            AnswerAssumption->setAttributesSize(compteur_assumption);

            List = new ObjectClassBroadcastList(AnswerAssumption,
                                                _handleClassAttributeMap.size());

            D.Out(pdProtocol,
                  "Object %u divestiture in class %u, now broadcasting...",
                  object->getHandle(), handle);
            broadcastClassMessage(List);
        } else {
            delete AnswerAssumption;
        }

    }
    else {
        D.Out(pdExcept,
              "NegotiatedAttributeOwnershipDivestiture should not "
              "be called on the RTIA.");
        throw RTIinternalError("NegotiatedAttributeOwnershipDivestiture "
                               "called on the RTIA.");
    }

    // Return the BroadcastList in case it had to be passed to the parent class.
    return List ;
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisitionIfAvailable.
void ObjectClass::
attributeOwnershipAcquisitionIfAvailable(FederateHandle the_federate,
                                         Object* object,
                                         const std::vector <AttributeHandle> &the_attributes)
    throw (ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           AttributeAlreadyBeingAcquired,
           RTIinternalError)
{
    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (unsigned index = 0 ; index < the_attributes.size() ; index++) {
        getAttribute(the_attributes[index]);
    }

    if (server) {
        // Federate must publish the class.
        if (!isFederatePublisher(the_federate)) {
            D.Out(pdExcept, "exception : ObjectClassNotPublished.");
            throw ObjectClassNotPublished("");
        }

        //rem _handleClassAttributeMap.size()=attributeState.size()
        for (unsigned i = 0 ; i < the_attributes.size() ; i++) {
            ObjectClassAttribute * oca = getAttribute(the_attributes[i]);
            ObjectAttribute * oa = object->getAttribute(the_attributes[i]);

            // The federate has to publish attributes he desire to
            // acquire.
            if (!oca->isPublishing(the_federate) &&
                (!oca->isNamed("privilegeToDelete")))
                throw AttributeNotPublished("");
            // Does federate already owns some attributes.
            if (oa->getOwner() == the_federate)
                throw FederateOwnsAttributes("");
            // Does federate is on the way to acquire something.
            if (oa->isCandidate(the_federate))
                throw AttributeAlreadyBeingAcquired("");
        }

        NM_Attribute_Ownership_Acquisition_Notification *Answer_notification = new NM_Attribute_Ownership_Acquisition_Notification();
        Answer_notification->setFederation(server->federation());
        Answer_notification->setFederate(the_federate);
        Answer_notification->setException(e_NO_EXCEPTION) ;
        Answer_notification->setObject(object->getHandle());
        Answer_notification->setAttributesSize(the_attributes.size());

        NM_Attribute_Ownership_Unavailable *Answer_unavailable = new NM_Attribute_Ownership_Unavailable();
        Answer_unavailable->setFederation(server->federation());
        Answer_unavailable->setFederate(the_federate);
        Answer_unavailable->setException(e_NO_EXCEPTION) ;
        Answer_unavailable->setObject(object->getHandle());
        Answer_unavailable->setAttributesSize(the_attributes.size()) ;

        CDiffusion diffusionDivestiture;

        //
        //Ce service ne doit pas ajouter le federe demandeur a la liste
        //des candidats!!!
        //
        int compteur_unavailable = 0 ;
        int compteur_notification = 0 ;
        int compteur_divestiture = 0 ;
        FederateHandle oldOwner ;

        for (uint32_t i = 0 ; i < the_attributes.size() ; i++) {
            ObjectClassAttribute * oca = getAttribute(the_attributes[i]);
            ObjectAttribute * oa = object->getAttribute(the_attributes[i]);

            oldOwner = oa->getOwner();
            if ((oldOwner == 0) || (oa->beingDivested())) {
                //Cet attribut est libre ou offert par son proprietaire
                //S'il est offert
                if (oa->beingDivested()) {
                    ++compteur_divestiture;
                    diffusionDivestiture.push_back(DiffusionPair(oldOwner,oa->getHandle()));
                }
                //Qu'il soit offert ou libre
                Answer_notification->setAttributes(the_attributes[i],compteur_notification);
                oa->setOwner(the_federate);
                oa->setDivesting(false);
                compteur_notification++ ;
                //object->Owner reste le champ de reference
                //pour le privilegeToDelete
                if (oca->isNamed("privilegeToDelete"))
                    object->setOwner(the_federate);
            }
            else {
                //Cet attribut n'est pas disponible!!!
                Answer_unavailable->setAttributes(the_attributes[i],compteur_unavailable);
                oa->addCandidate(the_federate);
                compteur_unavailable++ ;
            }
        }

        if (compteur_notification != 0) {
            Answer_notification->setAttributesSize(compteur_notification);
            sendToFederate(Answer_notification, the_federate);
        }
        else
            delete Answer_notification ;

        D.Out(pdDebug,
              "Debut traitement : send divestiture notification message");

        if (compteur_divestiture != 0) {
        	NM_Attribute_Ownership_Divestiture_Notification AODN;
            sendToOwners(diffusionDivestiture, object,
                         the_federate, "\0",
                         AODN);
        }

        if (compteur_unavailable != 0) {
            Answer_unavailable->setAttributesSize(compteur_unavailable);
            sendToFederate(Answer_unavailable, the_federate);
        }
        else
            delete Answer_unavailable ;
    }
    else {
        D.Out(pdExcept, "AttributeOwnershipAcquisitionIfAvailable should not "
              "be called on the RTIA.");
        throw RTIinternalError("AttributeOwnershipAcquisitionIfAvailable "
                               "called on the RTIA.");
    }
}

// ----------------------------------------------------------------------------
//! unconditionalAttributeOwnershipDivestiture.
ObjectClassBroadcastList * ObjectClass::
unconditionalAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                           Object* object,
                                           const std::vector <AttributeHandle> &theAttributeList)
    throw (AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError)
{
    // Pre-conditions checking
    // Do all attribute handles exist?
    // If not then AttributeNotDefined will be thrown.
    for (unsigned index = 0 ; index < theAttributeList.size() ; index++) {
        getAttribute(theAttributeList[index]);
    }

    // Is the federate the owner of all the attributes?
    ObjectAttribute * oa ;
    for (unsigned i = 0 ; i < theAttributeList.size() ; i++) {
        oa = object->getAttribute(theAttributeList[i]);
        if (oa->getOwner() != theFederateHandle)
            throw AttributeNotOwned(stringize() << "federate <"
                                                << theFederateHandle
                                                << "> not owner of attribute <"
                                                << oa->getHandle()<<">, it is=<"<<oa->getOwner()<<">.");
    }

    int compteur_assumption = 0 ;
    NM_Unconditional_Attribute_Ownership_Divestiture *AnswerAssumption = NULL ;
    ObjectClassBroadcastList *List = NULL ;
    FederateHandle NewOwner ;
    
    if (server != NULL) {
      
        AnswerAssumption = new NM_Unconditional_Attribute_Ownership_Divestiture();
        AnswerAssumption->setAttributesSize(theAttributeList.size());
        CDiffusion diffusionAcquisition;

        for (unsigned i = 0 ; i < theAttributeList.size() ; i++) {
            ObjectClassAttribute * oca = getAttribute(theAttributeList[i]);
            ObjectAttribute * oa = object->getAttribute(theAttributeList[i]);

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

                diffusionAcquisition.push_back(DiffusionPair(NewOwner,oa->getHandle())) ;

                if (oca->isNamed("privilegeToDelete")) {
                    object->setOwner(NewOwner);
                }
            }
            else {
                AnswerAssumption->setAttributes(theAttributeList[i],compteur_assumption);
                oa->setOwner(0);
                oa->setDivesting(false);
                compteur_assumption++ ;
            }
        }
	
        if (compteur_assumption != 0) {
            AnswerAssumption->setFederation(server->federation());
            AnswerAssumption->setFederate(theFederateHandle);
            AnswerAssumption->setException(e_NO_EXCEPTION) ;
            AnswerAssumption->setObject(object->getHandle());
            AnswerAssumption->setLabel(std::string());
            AnswerAssumption->setAttributesSize(compteur_assumption);

            List = new ObjectClassBroadcastList(AnswerAssumption,
                                                _handleClassAttributeMap.size());

            D.Out(pdProtocol,
                  "Object %u updated in class %u, now broadcasting...",
                  object->getHandle(), handle);

            broadcastClassMessage(List);
        }
        else {
            delete AnswerAssumption;
        }
	
        if (!diffusionAcquisition.empty()) {
        	NM_Attribute_Ownership_Acquisition_Notification AOAN;
            sendToOwners(diffusionAcquisition, object,
                         theFederateHandle, "\0",
                         AOAN);
        }
    }
    else {
        D.Out(pdExcept, "UnconditionalAttributeOwnershipDivestiture should "
              "not be called on the RTIA.");
        throw RTIinternalError("UnconditionalAttributeOwnershipDivestiture "
                               "called on the RTIA.");
    }

    // Return the BroadcastList in case it had to be passed to the parent
    // class.
    //! List could be equal to NULL
    return List ;
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisition.
void
ObjectClass::attributeOwnershipAcquisition(FederateHandle theFederateHandle,
                                           Object* object,
                                           const std::vector <AttributeHandle> &theAttributeList,
                                           const std::string& theTag)
    throw (ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           RTIinternalError)
{
    // Verification of the conditions should be done before any changes!
    // Pre-conditions checking
    for (unsigned i = 0 ; i < theAttributeList.size() ; i++) {
        // Do all attribute handles exist ? It may throw AttributeNotDefined.
        ObjectClassAttribute * oca = getAttribute(theAttributeList[i]);
        ObjectAttribute * oa = object->getAttribute(theAttributeList[i]);
	// Does federate own this attribute?
        if ( oa->getOwner() == theFederateHandle )
            throw FederateOwnsAttributes("");
        // Does federate publish the attribute?
        if ( !(oca->isPublishing(theFederateHandle) || oca->isNamed("privilegeToDelete")) )
            throw AttributeNotPublished("");
    }
    
    //! TODO: replace 'compteur' with 'counter'
    int compteur_notification = 0 ;
    FederateHandle oldOwner ;
    if (server != NULL) {
        // The federate have to publish the class
        if (!isFederatePublisher(theFederateHandle)) {
            D.Out(pdExcept, "exception : ObjectClassNotPublished.");
            throw ObjectClassNotPublished("");
        }
        NM_Attribute_Ownership_Acquisition_Notification *AnswerNotification = new NM_Attribute_Ownership_Acquisition_Notification();

        AnswerNotification->setFederation(server->federation());
        AnswerNotification->setFederate(theFederateHandle);
        AnswerNotification->setException(e_NO_EXCEPTION) ;
        AnswerNotification->setObject(object->getHandle());
        AnswerNotification->setAttributesSize(theAttributeList.size()) ;

	/*! TODO: remove size field from the CDiffusion -
	 * it copies std::vector functionality.
	 * It could be reasonably to replace the whole structure with
	 * a single typedef std::vector<>
	 */
        CDiffusion diffusionDivestiture;
        CDiffusion diffusionRelease;
	
        for (unsigned i = 0 ; i < theAttributeList.size() ; i++) {
            ObjectClassAttribute * oca = getAttribute(theAttributeList[i]);
            ObjectAttribute * oa = object->getAttribute(theAttributeList[i]);
            oldOwner = oa->getOwner();
            if ((oldOwner == 0) || (oa->beingDivested())) {
                // This attribute is devested or is offered by its owner
                if (oa->beingDivested()) {
                    diffusionDivestiture.push_back(DiffusionPair(oldOwner,oa->getHandle()));
                }
                //Qu'il soit offert ou libre
                if (oa->isCandidate(theFederateHandle))
                    oa->removeCandidate(theFederateHandle);
                AnswerNotification->setAttributes(theAttributeList[i],compteur_notification) ;
                oa->setOwner(theFederateHandle);
                oa->setDivesting(false);
                compteur_notification++ ;

                // object->Owner reste le champ de reference pour
                // le privilegeToDelete
                if (oca->isNamed("privilegeToDelete"))
                    object->setOwner(theFederateHandle);
            }
            else {
                diffusionRelease.push_back(DiffusionPair(oldOwner,oa->getHandle()));
                // Moving federate to the front position of candidate list
                oa->removeCandidate(theFederateHandle);
                oa->addCandidate(theFederateHandle);
            }
        }
        if (compteur_notification != 0) {
            AnswerNotification->setAttributesSize(compteur_notification);
            sendToFederate(AnswerNotification, theFederateHandle);
        }
        else
            delete AnswerNotification ;

        if (!diffusionDivestiture.empty()) {
        	NM_Attribute_Ownership_Divestiture_Notification AODN;
            sendToOwners(diffusionDivestiture, object,
                         theFederateHandle, "\0",
                         AODN);
        }

        if (!diffusionRelease.empty()) {
        	NM_Request_Attribute_Ownership_Release RAOR;
            sendToOwners(diffusionRelease, object, theFederateHandle,
                         theTag, RAOR);
        }
    }
    else {
        D.Out(pdExcept,
              "AttributeOwnershipAcquisition should not be called "
              "on the RTIA.");
        throw RTIinternalError("AttributeOwnershipAcquisition called "
                               "on the RTIA");
    }
}

// ----------------------------------------------------------------------------
//! attributeOwnershipReleaseResponse.
AttributeHandleSet *
ObjectClass::
attributeOwnershipReleaseResponse(FederateHandle the_federate,
                                  Object* object,
                                  const std::vector <AttributeHandle> &the_attributes)
    throw (AttributeNotDefined,
           AttributeNotOwned,
           FederateWasNotAskedToReleaseAttribute,
           RTIinternalError)
{
    // Pre-conditions checking

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (unsigned index = 0 ; index < the_attributes.size() ; index++) {
        getAttribute(the_attributes[index]);
    }

    //Le f�d�r� est-il propri�taire de tous les attributs
    //Y a-t-il des acqu�reurs pour les attributs
    for (unsigned i = 0 ; i < the_attributes.size() ; i++) {
        ObjectAttribute * oa = object->getAttribute(the_attributes[i]);

        if (oa->getOwner() != the_federate) {
            throw AttributeNotOwned("");
        }
        if (!oa->hasCandidates()) {
            throw FederateWasNotAskedToReleaseAttribute("");
        }
    }

    int compteur_acquisition = 0 ;
    FederateHandle newOwner ;
    AttributeHandleSet *theAttribute ;
    if (server != NULL) {
        CDiffusion diffusionAcquisition;

        theAttribute = new AttributeHandleSet(the_attributes.size());

        for (unsigned i = 0 ; i < the_attributes.size() ; i++) {
            ObjectClassAttribute * oca  = getAttribute(the_attributes[i]);
            ObjectAttribute* oa = object->getAttribute(the_attributes[i]);

            //Le demandeur le plus r�cent devient propri�taire
            newOwner = oa->getFirstCandidate();

            oa->setOwner(newOwner);

            //On le supprime de la liste des demandeurs
            oa->removeCandidate(newOwner);

            //On r�initialise divesting
            oa->setDivesting(false);

            diffusionAcquisition.push_back(DiffusionPair(newOwner,oa->getHandle()));
            theAttribute->add(oa->getHandle());

            D.Out(pdDebug, "Acquisition handle %u compteur %u",
                  the_attributes[i], compteur_acquisition);

            if (oca->isNamed("privilegeToDelete"))
                object->setOwner(newOwner);
        }

        if (!diffusionAcquisition.empty()) {
        	NM_Attribute_Ownership_Acquisition_Notification AOAN;
            sendToOwners(diffusionAcquisition, object, the_federate, "\0",AOAN);
        }
    }
    else {
        D.Out(pdExcept, "NegotiatedAttributeOwnershipDivestiture should not "
              "be called on the RTIA.");
        throw RTIinternalError("NegotiatedAttributeOwnershipDivestiture called"
                               " on the RTIA.");
    }
    return(theAttribute);
}

// ----------------------------------------------------------------------------
//! cancelAttributeOwnershipAcquisition.
void
ObjectClass::
cancelAttributeOwnershipAcquisition(FederateHandle federate_handle,
                                    Object* object,
                                    const std::vector <AttributeHandle> &attribute_list)
    throw (AttributeNotDefined,
           AttributeAlreadyOwned,
           AttributeAcquisitionWasNotRequested,
           RTIinternalError)
{
    // Pre-conditions checking

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (unsigned index = 0 ; index < attribute_list.size() ; index++)
        getAttribute(attribute_list[index]);

    for (unsigned i = 0 ; i < attribute_list.size(); i++)
        D.Out(pdDebug, "CancelAcquisition Object %u Attribute %u ",
              object->getHandle(), attribute_list[i]);

    if (server != NULL) {
        //rem _handleClassAttributeMap.size()=attributeState.size()

        for (unsigned i = 0 ; i < attribute_list.size() ; i++) {
            ObjectAttribute * oa = object->getAttribute(attribute_list[i]);

            D.Out(pdDebug, "Attribut %u Owner %u", attribute_list[i], oa->getOwner());
            // Does federate is already owning some attributes ?
            if (oa->getOwner() == federate_handle)
                throw AttributeAlreadyOwned("");
            // Does federate is already doing an acquisition ?
            if (!oa->isCandidate(federate_handle))
                throw AttributeAcquisitionWasNotRequested("");
        }

        NM_Confirm_Attribute_Ownership_Acquisition_Cancellation *answer_confirmation = new NM_Confirm_Attribute_Ownership_Acquisition_Cancellation();
        answer_confirmation->setFederation(server->federation());
        answer_confirmation->setFederate(federate_handle);
        answer_confirmation->setException(e_NO_EXCEPTION) ;
        answer_confirmation->setObject(object->getHandle());
        answer_confirmation->setAttributesSize(attribute_list.size()) ;

        int compteur_confirmation = 0 ;
        for (unsigned i = 0 ; i < attribute_list.size(); i++) {
            ObjectAttribute * oa = object->getAttribute(attribute_list[i]);

            answer_confirmation->setAttributes(attribute_list[i],compteur_confirmation) ;

            // We remove federate from candidates.
            oa->removeCandidate(federate_handle);
            D.Out(pdDebug, "Adding federate %u to attribute %u object %u",
                  federate_handle, attribute_list[i], object->getHandle());
            compteur_confirmation++ ;
        }

        if (compteur_confirmation != 0) {
            answer_confirmation->setAttributesSize(compteur_confirmation);
            sendToFederate(answer_confirmation, federate_handle);
        }
        else
            delete answer_confirmation ;
    }
    else {
        D.Out(pdExcept, "CancelAttributeOwnershipAcquisition should not "
              "be called on the RTIA.");
        throw RTIinternalError("CancelAttributeOwnershipAcquisition called "
                               "on the RTIA.");
    }
}

// ----------------------------------------------------------------------------
ObjectClassHandle
ObjectClass::getHandle() const
{
    return handle ;
}

// ----------------------------------------------------------------------------
/** Unsubscribe this federate/region pair
 */
void
ObjectClass::unsubscribe(FederateHandle fed, const RTIRegion *region)
{
    Debug(D, pdTrace) << "ObjectClass::unsubscribe: fed " << fed
                      << ", region "
                      << (region ? region->getHandle() : 0) << std::endl;

    for (HandleClassAttributeMap::iterator i = _handleClassAttributeMap.begin();
         i != _handleClassAttributeMap.end(); ++i) {
        if (i->second->isSubscribed(fed, region)) {
            i->second->unsubscribe(fed, region);
        }
    }
}

// ----------------------------------------------------------------------------
/** Unsubscribe this federate, for all regions currently subscribed
 */
void
ObjectClass::unsubscribe(FederateHandle fed)
{
    for (HandleClassAttributeMap::iterator i = _handleClassAttributeMap.begin(); i != _handleClassAttributeMap.end(); ++i) {
	if (i->second->isSubscribed(fed)) {
	    i->second->unsubscribe(fed);
	}
    }
} /* end of unsubscribe */

void
ObjectClass::addSubClass(ObjectClass *child) {

	/* build parent-child relationship */
	/* add child as subclass of parent */
    subClasses->addClass(child,NULL);
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
void
ObjectClass::recursiveDiscovering(FederateHandle federate,
				  ObjectClassHandle subscription)
	throw (ObjectClassNotDefined)
{
    Debug(D, pdInit) << "Recursive Discovering on class " << handle
	      << " for Federate " << federate << "." << std::endl ;

    bool go_deeper = sendDiscoverMessages(federate, subscription);

    if (go_deeper) {
        ObjectClassSet::const_iterator i ;
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
