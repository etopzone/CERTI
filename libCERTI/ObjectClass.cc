// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: ObjectClass.cc,v 3.7 2003/01/20 21:49:15 breholee Exp $
// ---------------------------------------------------------------------------

#include "ObjectClass.hh"

namespace certi {

static pdCDebug D("OBJECTCLASS", "(ObjClass) - ");

// ---------------------------------------------------------------------------
//! To be used only by CRead, it returns the new Attribute's Handle.
AttributeHandle
ObjectClass::addAttribute(ObjectClassAttribute *theAttribute,
                          Boolean is_inherited)
{
    if (theAttribute == NULL)
        throw RTIinternalError("Tried to add NULL attribute.");

    theAttribute->Handle = attributeSet.size() + 1 ;
    theAttribute->server = server ;

    // If the attribute is inherited, it keeps its security level.
    // If not, it takes the default security level of the class.
    if (is_inherited != RTI_TRUE)
        theAttribute->LevelID = LevelID ;

    attributeSet.push_front(theAttribute);

    D.Out(pdProtocol, "ObjectClass %u has a new attribute %u.",
          Handle, theAttribute->Handle);

    return theAttribute->Handle ;
}

// ---------------------------------------------------------------------------
//! Add the class' attributes to the 'Child' Class.
void
ObjectClass::addAttributesToChild(ObjectClass *the_child)
{
    // The Attribute List is read backwards to respect the same attribute order
    // for the child(Attributes are inserted at the beginning of the list).
    ObjectClassAttribute *childAttribute = NULL ;
    list<ObjectClassAttribute *>::reverse_iterator a ;
    for (a = attributeSet.rbegin(); a != attributeSet.rend(); a++) {
        assert((*a) != NULL);

        childAttribute = new ObjectClassAttribute(*a);
        assert(childAttribute != NULL);

        D.Out(pdProtocol,
              "ObjectClass %u adding new attribute %d to child class %u.",
              Handle, (*a)->Handle, the_child->Handle);

        the_child->addAttribute(childAttribute);

        if (childAttribute->Handle != (*a)->Handle)
            throw RTIinternalError("Error while copying child's attributes.");
    }
}

// ---------------------------------------------------------------------------
/*! Take a Broadcast List and continue to send messages. You should
  take a look at ObjectClassSet::RegisterObject to understand
  what is going on...
*/
void
ObjectClass::broadcastClassMessage(ObjectClassBroadcastList *ocbList)
{
    // 1. Set ObjectHandle to local class Handle.
    ocbList->message->objectClassHandle = Handle ;

    // 2. Update message attribute list by removing child's attributes.
    if ((ocbList->message->Type == m_REFLECT_ATTRIBUTE_VALUES) ||
        (ocbList->message->Type == m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION)) {
        int attr = 0 ;
        while (attr < ocbList->message->HandleArraySize) {
            // If the attribute is not in that class, remove it from
            // the message.
            try {
                getAttributeWithHandle(ocbList->message->HandleArray[attr]);
                attr++ ;
            }
            catch (AttributeNotDefined &e) {
                ocbList->message->removeAttribute(attr);
            }
        }
    }

    // 3. Add class/attributes subscribers to the list.
    switch(ocbList->message->Type) {

    case m_DISCOVER_OBJECT:
    case m_REMOVE_OBJECT: {
        // For each federate, add it to list if at least one attribute has
        // been subscribed.
        FederateHandle federate = 0 ;
        for (federate = 1 ; federate <= MaxSubscriberHandle ; federate++) {
            if (isFederateSubscriber(federate) == RTI_TRUE) {
                ocbList->addFederate(federate);
            }
        }
    }
        break ;

    case m_REFLECT_ATTRIBUTE_VALUES:
    case m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION: {
        // For each class attribute, update the list be adding federates who
        // subscribed to the attribute.
        list<ObjectClassAttribute *>::const_iterator a ;
        for (a = attributeSet.begin(); a != attributeSet.end(); a++) {
            (*a)->updateBroadcastList(ocbList);
        }
    }
        break ;

    default:
        throw RTIinternalError("BroadcastClassMsg: Unknown type.");
    }

    // 4. Send pending messages.
    ocbList->sendPendingMessage(server);
}

// ---------------------------------------------------------------------------
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
        msg->write(socket);
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

// ---------------------------------------------------------------------------
//! sendToOwners.
void
ObjectClass::sendToOwners(CDiffusion *diffusionList,
                          ObjectHandle theObjectHandle,
                          FederateHandle theFederate,
                          const char *theTag,
                          TypeNetworkMessage type)
{
    int nbAttributes = diffusionList->size ;

    FederateHandle toFederate ;
    for (int i = 0  ; i < nbAttributes  ; i++) {
        toFederate = diffusionList->DiffArray[i].NumeroFedere ;
        if (toFederate != 0) {
            NetworkMessage *answer = new NetworkMessage ;
            answer->Type = type ;
            answer->NumeroFederation = server->federation();
            answer->NumeroFedere = theFederate ;
            answer->Exception = e_NO_EXCEPTION ;
            answer->Objectid = theObjectHandle ;
            strcpy(answer->Label, theTag);

            int index = 0 ;
            for (int j = i  ; j < nbAttributes  ; j++) {
                if (diffusionList->DiffArray[j].NumeroFedere == toFederate) {
                    D.Out(pdDebug, "handle : %u",
                          diffusionList->DiffArray[j].AttribHandle);
                    diffusionList->DiffArray[j].NumeroFedere = 0 ;
                    answer->HandleArray[index] = diffusionList
                        ->DiffArray[j].AttribHandle ;
                    index++ ;
                }
            }
            answer->HandleArraySize = index ;
            D.Out(pdDebug,"Envoi message type %u ", type);
            sendToFederate(answer, toFederate);
        }
    }
}

// ---------------------------------------------------------------------------
/*! Throw SecurityError if the Federate is not allowed to access the
  Object Class, and print an Audit message containing reason.
*/
void
ObjectClass::checkFederateAccess(FederateHandle the_federate,
                                 const char *reason)
    throw (SecurityError)
{
    D.Out(pdInit, "Beginning of CheckFederateAccess for the federate %d",
          the_federate);

    // BUG: Should at least but a line in Audit
    if (server == NULL)
        return ;

    Boolean result = server->canFederateAccessData(the_federate, LevelID);

    // BUG: Should use Audit.
    if (result != RTI_TRUE) {
        cout << "Object Class " << Handle << " : SecurityError for federate "
             << the_federate << '(' << reason << ")." << endl ;
        throw SecurityError("Federate should not access Object Class.");
    }
}

// ---------------------------------------------------------------------------
//! ObjectClass constructor (only one).
ObjectClass::ObjectClass(void)
    : Handle(0), Father(0), server(NULL), Depth(0),
      Name(NULL), LevelID(PublicLevelID), MaxSubscriberHandle(0)
{
}

// ---------------------------------------------------------------------------
//! ObjectClass destructor (frees allocated memory).
ObjectClass::~ObjectClass(void)
{
    if (Name != NULL) {
        free(Name);
        Name = NULL ;
    }

    // Deleting instances
    if (!objectSet.empty())
        D.Out(pdError,
              "ObjectClass %d : Instances remaining while exiting...", Handle);

    while (!objectSet.empty()) {
        delete objectSet.front();
        objectSet.pop_front();
    }

    // Deleting Class Attributes
    while (!attributeSet.empty()) {
        delete attributeSet.front();
        attributeSet.pop_front();
    }

    // Deleting Sons
    while (!sonSet.empty()) {
        delete sonSet.front();
        sonSet.pop_front();
    }
}

// ---------------------------------------------------------------------------
/*! Delete the object instance 'theObjectHandle', and starts to
  broadcast the RemoveObject message. Return a BroadcastList of
  Federates, in order to allow our ObjectClassSet to go on with
  the message broadcasting, by giving the list to our parent
  class.
*/
ObjectClassBroadcastList *
ObjectClass::deleteInstance(FederateHandle theFederateHandle,
                            ObjectHandle theObjectHandle,
                            const char* theUserTag)
    throw (DeletePrivilegeNotHeld,
           ObjectNotKnown,
           RTIinternalError)
{
    // 1. Pre-conditions checking(may throw ObjectNotKnown)
    Object *object = getInstanceWithID(theObjectHandle);

    // Is the Federate really the Object Owner?(Checked only on RTIG)
    if ((server != NULL) && (object->Owner != theFederateHandle)) {
        D.Out(pdExcept, "Delete Object %d: Federate %d not owner.",
              theObjectHandle, theFederateHandle);
        throw DeletePrivilegeNotHeld();
    }

    // 2. Remove Instance
    list<Object *>::iterator o ;
    list<Object *>::iterator tmp ;
    for (o = objectSet.begin(); o != objectSet.end(); o++) {
        if ((*o)->ID == theObjectHandle) {
            tmp = o ;
            tmp-- ;
            delete (*o);
            objectSet.erase(o); // i is dereferenced.
            o = tmp ; // loop will increment o
            // A break may be enough ?!
        }
    }

    // 3. Prepare and broadcast message.
    ObjectClassBroadcastList *ocbList = NULL ;
    if (server != NULL) {
        D.Out(pdRegister,
              "Object %u deleted in class %u, now broadcasting...",
              theObjectHandle, Handle);

        NetworkMessage *answer = new NetworkMessage ;
        answer->Type = m_REMOVE_OBJECT ;
        answer->NumeroFederation = server->federation();
        answer->NumeroFedere = theFederateHandle ;
        answer->Exception = e_NO_EXCEPTION ;
        answer->objectClassHandle = Handle ; // Class Handle
        answer->Objectid = theObjectHandle ;
        strcpy(answer->Label, theUserTag);

        ocbList = new ObjectClassBroadcastList(answer, 0);
        broadcastClassMessage(ocbList);
    }
    else {
        D.Out(pdRegister,
              "Object %u deleted in class %u, no broadcast to do.",
              theObjectHandle, Handle);
    }

    // Return the BroadcastList in case it had to be passed to the parent
    // class.
    return ocbList ;
}

// ---------------------------------------------------------------------------
//! Print the ObjectClasses tree to the standard output.
void ObjectClass::display(void) const
{
    cout << " ObjectClass #" << Handle << " \"" << Name << "\":" << endl ;

    // Display inheritance
    cout << " Parent Class Handle: " << Father << endl ;
    cout << " Security Level: " << LevelID << endl ;
    cout << " " << sonSet.size() << " Child(s):" << endl ;

    list<ObjectClassChild *>::const_iterator s = sonSet.begin();
    for (int i = 1  ; s != sonSet.end(); s++,i++) {
        cout << " Son " << i << " Handle: "<< (*s)->Handle << endl ;
    }

    // Display Attributes
    cout << " " << attributeSet.size() << " Attribute(s):" << endl ;
    list<ObjectClassAttribute *>::const_iterator a ;
    for (a = attributeSet.begin(); a != attributeSet.end(); a++) {
        (*a)->display();
    }

    // Display Instances
    cout << " " << objectSet.size() << " Instances(s):" << endl ;
    list<Object *>::const_iterator o ;
    for (o = objectSet.begin(); o != objectSet.end(); o++) {
        (*o)->display();
    }
}

// ---------------------------------------------------------------------------
//! getAttributeHandle.
AttributeHandle
ObjectClass::getAttributeHandle(const char* the_name) const
    throw (AttributeNotDefined,
           RTIinternalError)
{
    list<ObjectClassAttribute *>::const_iterator a ;
    for (a = attributeSet.begin(); a != attributeSet.end(); a++) {
        if (strcmp((*a)->getName(), the_name) == 0)
            return (*a)->Handle ;
    }

    D.Out(pdExcept, "ObjectClass %u: Attribute \"%s\" not defined.",
          Handle, the_name);

    throw AttributeNotDefined();
}

// ---------------------------------------------------------------------------
//! getAttributeName.
const char*
ObjectClass::getAttributeName(AttributeHandle the_handle) const
    throw (AttributeNotDefined,
           RTIinternalError)
{
    return getAttributeWithHandle(the_handle)->getName();
}


// ---------------------------------------------------------------------------
//! getAttributeWithHandle (private module).
ObjectClassAttribute *
ObjectClass::getAttributeWithHandle(AttributeHandle the_handle) const
    throw (AttributeNotDefined)
{
    list<ObjectClassAttribute *>::const_iterator a ;
    for (a = attributeSet.begin(); a != attributeSet.end(); a++) {
        if ((*a)->Handle == the_handle)
            return (*a);
    }

    D.Out(pdDebug, "ObjectClass %d: Attribute %d not defined.",
          Handle, the_handle);

    throw AttributeNotDefined();
}

// ---------------------------------------------------------------------------
//! getInstanceWithId (private module).
Object *
ObjectClass::getInstanceWithID(ObjectHandle the_id) const
    throw (ObjectNotKnown)
{
    list<Object *>::const_iterator o ;
    for (o = objectSet.begin(); o != objectSet.end(); o++) {
        if ((*o)->ID == the_id)
            return (*o);
    }

    throw ObjectNotKnown();
}

// ---------------------------------------------------------------------------
//! Return RTI_TRUE if the Federate is publishing any attribute of this class.
Boolean
ObjectClass::isFederatePublisher(FederateHandle the_federate) const
{
    D.Out(pdRegister, "attributeSet.size() = %d.", attributeSet.size());

    list<ObjectClassAttribute *>::const_iterator a ;
    for (a = attributeSet.begin(); a != attributeSet.end(); a++) {
        if ((*a)->IsPublishing(the_federate) == RTI_TRUE)
            return RTI_TRUE ;
    }

    return RTI_FALSE ;
}

// ---------------------------------------------------------------------------
//! Return RTI_TRUE if the Federate has subscribe to any attribute.
Boolean
ObjectClass::isFederateSubscriber(FederateHandle the_federate) const
{
    list<ObjectClassAttribute *>::const_iterator a ;
    for (a = attributeSet.begin(); a != attributeSet.end(); a++) {
        if ((*a)->hasSubscribed(the_federate) == RTI_TRUE)
            return RTI_TRUE ;
    }

    return RTI_FALSE ;
}

// ---------------------------------------------------------------------------
/*! Return RTI_TRUE if the object instance designated by 'theID' is
  present in that class, else return RTI_FALSE.
*/
Boolean
ObjectClass::isInstanceInClass(ObjectHandle theID)
{
    try {
        getInstanceWithID(theID);
    }
    catch (ObjectNotKnown &e) {
        return RTI_FALSE ;
    }

    return RTI_TRUE ;
}

// ---------------------------------------------------------------------------
//! killFederate.
ObjectClassBroadcastList *
ObjectClass::killFederate(FederateHandle the_federate)
    throw ()
{
    D.Out(pdRegister, "Object Class %d: Killing Federate %d.",
          Handle, the_federate);

    try {
        // Does federate is publishing something ? (not important)
        if (isFederatePublisher(the_federate) == RTI_TRUE)
            publish(the_federate, NULL, 0, RTI_FALSE);

        // Does federate subscribed something ?
        // Problem, no removeObject or discover must not be sent to it.
        if (isFederateSubscriber(the_federate) == RTI_TRUE)
            subscribe(the_federate, NULL, 0, RTI_FALSE);
    }
    catch (SecurityError &e) {}

    // Does federate owns instances ?
    list<Object *>::iterator o ;
    for (o = objectSet.begin(); o != objectSet.end(); o++) {
        if ((*o)->Owner == the_federate)
            // Return non-NULL to indicate that :
            // 1- A RemoveObject message should be broadcasted through parent
            // class
            // 2- The federate may own another instance, and this function
            // must be called again.
            // BUG: String \/
            return deleteInstance(the_federate, (*o)->ID, "Killed");
    }

    D.Out(pdRegister, "Object Class %d:Federate %d killed.",
          Handle, the_federate);

    // Return NULL if the Federate did not own any instance.
    return NULL ;
}

// ---------------------------------------------------------------------------
//! publish.
void
ObjectClass::publish(FederateHandle theFederateHandle,
                     AttributeHandle *theAttributeList,
                     UShort theListSize,
                     bool PubOrUnpub)
    throw (AttributeNotDefined,
           RTIinternalError,
           SecurityError)
{
    D.Out(pdInit, "Beginning of Publish for the federate %d",
          theFederateHandle);

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (UShort index = 0 ; index < theListSize ; index++)
        getAttributeWithHandle(theAttributeList[index]);

    // Check Security Levels
    checkFederateAccess(theFederateHandle, "Publish");

    // Reset all previous publish information.
    D.Out(pdInit, "ObjectClass %d: Reset publish info of Federate %d.",
          Handle, theFederateHandle);

    list<ObjectClassAttribute *>::const_iterator a ;
    for (a = attributeSet.begin(); a != attributeSet.end(); a++) {
        if ((*a)->IsPublishing(theFederateHandle) == RTI_TRUE)
            (*a)->publish(theFederateHandle, RTI_FALSE);
    }

    // Publish attributes one by one.
    ObjectClassAttribute * attribute ;
    for (UShort i = 0 ; i < theListSize ; i++) {
        D.Out(pdInit, "ObjectClass %d: Federate %d publishes attribute %d.",
              Handle, theFederateHandle, theAttributeList[i]);
        attribute = getAttributeWithHandle(theAttributeList[i]);
        attribute->publish(theFederateHandle, PubOrUnpub);
    }
}

// ---------------------------------------------------------------------------
/*! Register a new object instance, and start to broadcast the DiscoverObject
  Message to class subscribers. Return a Broadcast List of Federates, in
  order to allow our ObjectClassSet to go on with the message broadcasting,
  by giving the list to our parent class.
*/
ObjectClassBroadcastList *
ObjectClass::registerInstance(FederateHandle the_federate_handle,
                              ObjectHandle the_object_handle,
                              const char* the_object_name)
    throw (ObjectClassNotPublished,
           ObjectAlreadyRegistered,
           RTIinternalError)
{
    D.Out(pdInit, "ObjectClass.");

    // Pre-conditions checking
    if (isInstanceInClass(the_object_handle) == RTI_TRUE) {
        D.Out(pdExcept, "exception : ObjectAlreadyRegistered.");
        throw ObjectAlreadyRegistered();
    }

    // This condition is only to be checked on the RTIG
    if ((server != NULL) &&
        (isFederatePublisher(the_federate_handle) != RTI_TRUE)) {
        D.Out(pdExcept, "exception : ObjectClassNotPublished.");
        throw ObjectClassNotPublished();
    }

    D.Out(pdInit, "ObjectClass1.");

    // Register new Object.
    Object *object = new Object(the_federate_handle);

    object->ID = the_object_handle ;
    if (the_object_name != NULL)
        object->setName(the_object_name);

    // Ownership management :
    // Copy instance attributes
    // Federate only owns attributes it publishes.
    list<ObjectClassAttribute *>::reverse_iterator a ;
    for (a = attributeSet.rbegin(); a != attributeSet.rend(); a++) {
        if ((*a)->IsPublishing(the_federate_handle))
            object->attributeState.
                push_front(new ObjectAttribute((*a)->Handle,
                                               the_federate_handle));
        else
            object->attributeState.
                push_front(new ObjectAttribute((*a)->Handle,0));

        // privilegeToDelete is owned by federate even not published.
        if (strcmp((*a)->getName(),"privilegeToDelete") == 0)
            object->attributeState.front()->setOwner(the_federate_handle);
    }

    objectSet.push_front(object);

    // Prepare and Broadcast message for this class
    ObjectClassBroadcastList *ocbList = NULL ;
    if (server != NULL) {
        D.Out(pdRegister,
              "Object %u registered in class %u, now broadcasting...",
              the_object_handle, Handle);

        NetworkMessage *answer = new NetworkMessage ;
        answer->Type = m_DISCOVER_OBJECT ;
        answer->NumeroFederation = server->federation();
        answer->NumeroFedere = the_federate_handle ;
        answer->Exception = e_NO_EXCEPTION ;
        answer->objectClassHandle = Handle ; // Class Handle
        answer->Objectid = the_object_handle ;
        strcpy(answer->Label, the_object_name);

        ocbList = new ObjectClassBroadcastList(answer, 0);
        broadcastClassMessage(ocbList);
    }
    else {
        D.Out(pdRegister,
              "Object %u registered in class %u, no broadcast to do.",
              the_object_handle, Handle);
    }

    // Return the BroadcastList in case it had to be passed to the parent
    // class.
    return ocbList ;
}

// ---------------------------------------------------------------------------
//! sendDiscoverMessages.
Boolean
ObjectClass::sendDiscoverMessages(FederateHandle theFederate,
                                  ObjectClassHandle theOriginalClass)
{
    // 1- If this class is not the original class, and the Federate is a
    // subscriber of the class, the Recursive process must be stopped,
    // because the Federate must have received all previous DiscoverObject
    // Message for this class and its sub-classes.
    if ((Handle != theOriginalClass) &&
        (isFederateSubscriber(theFederate) == RTI_TRUE))
        return RTI_FALSE ;

    // 2- If there is no instance of the class, return.(the recursive process
    // must continue).
    if (objectSet.empty())
        return RTI_TRUE ;

    // 3- Else prepare the common part of the Message.
    // Messages are sent on behalf of the original class.
    NetworkMessage *message = new NetworkMessage ;
    message->Type = m_DISCOVER_OBJECT ;
    message->NumeroFederation = server->federation();
    message->NumeroFedere = theFederate ;
    message->Exception = e_NO_EXCEPTION ;
    message->objectClassHandle = theOriginalClass ;

    // 4- For each Object instance in the class, send a Discover message.
    Socket *socket = NULL ;
    list<Object *>::const_iterator o ;
    for (o = objectSet.begin(); o != objectSet.end(); o++) {
        D.Out(pdInit,
              "Sending DiscoverObj to Federate %d for Object %u in class %u ",
              theFederate, (*o)->ID, Handle, message->Label);

        message->Objectid = (*o)->ID ;
        (*o)->getName(message->Label);

        // Send Message to Federate
        try {
            socket = server->getSocketLink(theFederate);
            message->write(socket);
        }
        catch (RTIinternalError &e) {
            D.Out(pdExcept,
                  "Reference to a killed Federate while sending DO msg.");
        }
        catch (NetworkError &e) {
            D.Out(pdExcept, "Network error while sending DO msg, ignoring.");
        }
    } // for each object instance

    delete message ;

    // 5- The same method must be called on my sub-classes.
    return RTI_TRUE ;
}

// ---------------------------------------------------------------------------
//! setName.
void
ObjectClass::setName(const char *new_name)
    throw (ValueLengthExceeded, RTIinternalError)
{
    // Check Length
    if ((new_name == NULL) || (strlen(new_name) > MAX_USER_TAG_LENGTH)) {
        D.Out(pdExcept, "Object class Name %s too long.", new_name);
        throw ValueLengthExceeded("Object class name too long.");
    }

    // Free previous name
    if (Name != NULL)
        free(Name);

    // Store new name
    Name = strdup(new_name);
    if (Name == NULL)
        throw RTIinternalError("Memory Exhausted.");
}

// ---------------------------------------------------------------------------
//! A class' LevelID can only be increased.
void
ObjectClass::setLevelId(SecurityLevelID new_levelID)
{
    if (server->dominates(new_levelID, LevelID) == RTI_FALSE)
        throw SecurityError("Attempt to lower object class level.");

    LevelID = new_levelID ;
}

// ---------------------------------------------------------------------------
/*! Return RTI_TRUE if theFederate had never subscribed to this class
  before. In that case, ObjectClassSet will call SendDiscoverMessages
  on this class and on all child classes to allow them to send
  Discover Messages for already registered instances.
*/
Boolean
ObjectClass::subscribe(FederateHandle theFederate,
                       AttributeHandle *theAttributeList,
                       UShort theListSize,
                       bool SubOrUnsub)
    throw (AttributeNotDefined, RTIinternalError, SecurityError)
{
    // Check Security Levels
    checkFederateAccess(theFederate, "Subscribe");

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (UShort index = 0 ; index < theListSize ; index++)
        getAttributeWithHandle(theAttributeList[index]);

    // Save the Federate number.
    if (theFederate > MaxSubscriberHandle)
        MaxSubscriberHandle = theFederate ;

    // A new subscribtion invalidates all previous subscription to the same
    // object class, so we first remove all previous subscription information.
    D.Out(pdInit,
          "ObjectClass %d: Resetting previous Sub info of Federate %d.",
          Handle, theFederate);

    Boolean wasPreviousSubscriber = RTI_FALSE ;
    list<ObjectClassAttribute *>::iterator a ;
    for (a = attributeSet.begin(); a != attributeSet.end(); a++) {
        if ((*a)->hasSubscribed(theFederate) == RTI_TRUE) {
            (*a)->subscribe(theFederate, RTI_FALSE);
            wasPreviousSubscriber = RTI_TRUE ;
        }
    }

    // Subscribe to attributes one by one.
    ObjectClassAttribute *attribute ;
    for (UShort index = 0 ; index < theListSize ; index++) {
        D.Out(pdInit,
              "ObjectClass %d: Federate %d subscribes to attribute %d.",
              Handle, theFederate, theAttributeList[index]);
        attribute = getAttributeWithHandle(theAttributeList[index]);
        attribute->subscribe(theFederate, SubOrUnsub);
    }

    // If the Federate was not a subscriber before, and has now subscribed
    // to at least one attribute, it must discover class' current instances.

    // BUG: If the Federate unsubscribe, he should receive RemoveObject msgs?
    if ((wasPreviousSubscriber == RTI_FALSE) && // Not sub. before
        (SubOrUnsub == RTI_TRUE) && // subscribe(and not Unsub.)
        (theListSize > 0)) // at least to 1 attribute.
        return RTI_TRUE ;
    else
        return RTI_FALSE ;
}

// ---------------------------------------------------------------------------
//! update Attribute Values (with time).
ObjectClassBroadcastList *
ObjectClass::updateAttributeValues(FederateHandle theFederateHandle,
                                   ObjectHandle theObjectHandle,
                                   AttributeHandle *theAttributeArray,
                                   AttributeValue *theValueArray,
                                   UShort theArraySize,
                                   FederationTime theTime,
                                   const char* theUserTag)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError,
           InvalidObjectHandle)
{
    // Pre-conditions checking
    Object *object = getInstanceWithID(theObjectHandle);

    // Ownership management: Test ownership on each attribute before updating.
    for (int i = 0 ; i < theArraySize ; i++) {
        deque<ObjectAttribute *>::const_iterator j ;
        j = object->attributeState.begin();
        for (; j != object->attributeState.end(); j++) {
            if (theAttributeArray[i] == (*j)->Handle) {
                if ((*j)->getOwner() != theFederateHandle)
                    throw AttributeNotOwned();
            }
        }
    }

    // Federate must be Owner of all attributes(not Owner of the instance).
    // if (object->Owner != theFederateHandle)
    // throw AttributeNotOwned();

    // Prepare and Broadcast message for this class
    ObjectClassBroadcastList *ocbList = NULL ;
    if (server != NULL) {
        NetworkMessage *answer = new NetworkMessage ;
        answer->Type = m_REFLECT_ATTRIBUTE_VALUES ;
        answer->NumeroFederation = server->federation();
        answer->NumeroFedere = theFederateHandle ;
        answer->Exception = e_NO_EXCEPTION ;
        answer->Objectid = theObjectHandle ;
        answer->Date = theTime ;

        strcpy(answer->Label, theUserTag);

        answer->HandleArraySize = theArraySize ;

        for (int i = 0 ; i < theArraySize ; i++) {
            answer->HandleArray[i] = theAttributeArray[i] ;
            answer->setValue(i, theValueArray[i]);
        }

        ocbList = new ObjectClassBroadcastList(answer, attributeSet.size());

        D.Out(pdProtocol,
              "Object %u updated in class %u, now broadcasting...",
              theObjectHandle, Handle);

        broadcastClassMessage(ocbList);
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

// ---------------------------------------------------------------------------
//! isAttributeOwnedByFederate.
Boolean
ObjectClass::isAttributeOwnedByFederate(ObjectHandle theObject,
                                        AttributeHandle theAttribute,
                                        FederateHandle theFederateHandle)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           RTIinternalError)
{
    // Pre-conditions checking

    // may throw ObjectNotKnown
    Object* object = getInstanceWithID(theObject);

    // It may throw AttributeNotDefined
    getAttributeWithHandle(theAttribute);

    D.Out(pdDebug,
          "isAttributeOwnedByFederate sur l'attribut %u de l'objet %u",
          theAttribute, theObject);

    if (server != NULL) {
        deque<ObjectAttribute *>::const_iterator i ;
        i = object->attributeState.begin();
        for (; i != object->attributeState.end(); i++) {
            if (theAttribute == (*i)->Handle) {
                if ((*i)->getOwner() == theFederateHandle)
                    return(RTI_TRUE);
                else
                    return(RTI_FALSE);
            }
        }
    }
    else {
        D.Out(pdDebug,"Only called on RTIG");
    }

    return RTI_FALSE ;
}

// ---------------------------------------------------------------------------
//! queryAttributeOwnership.
void
ObjectClass::queryAttributeOwnership(ObjectHandle theObject,
                                     AttributeHandle theAttribute,
                                     FederateHandle theFederateHandle)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           RTIinternalError)
{
    // Pre-conditions checking

    // may throw ObjectNotKnown
    Object* object = getInstanceWithID(theObject);

    // It may throw AttributeNotDefined
    getAttributeWithHandle(theAttribute);

    D.Out(pdDebug, "queryAttributeOwnership sur l'attribut %u de l'objet %u",
          theAttribute, theObject);

    if (server != NULL) {
        NetworkMessage *answer = new NetworkMessage ;
        answer->NumeroFederation = server->federation();
        answer->Exception = e_NO_EXCEPTION ;
        answer->Objectid = theObject ;
        answer->HandleArray[0] = theAttribute ;

        deque<ObjectAttribute *>::const_iterator i ;
        i = object->attributeState.begin();
        for (; i != object->attributeState.end(); i++) {
            if (theAttribute == (*i)->Handle) {
                answer->NumeroFedere = (*i)->getOwner();
            }
        }

        if (answer->NumeroFedere != 0)
            answer->Type = m_INFORM_ATTRIBUTE_OWNERSHIP ;
        else
            answer->Type = m_ATTRIBUTE_IS_NOT_OWNED ;

        sendToFederate(answer, theFederateHandle);
    }
    else {
        D.Out(pdDebug,"Only called on RTIG");
    }
}

// ---------------------------------------------------------------------------
//! negotiatedAttributeOwnershipDivestiture.
ObjectClassBroadcastList * ObjectClass::
negotiatedAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                        ObjectHandle theObjectHandle,
                                        AttributeHandle *theAttributeList,
                                        UShort theListSize,
                                        const char *theTag)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           AttributeAlreadyBeingDivested,
           RTIinternalError)
{
    // Pre-conditions checking

    // may throw ObjectNotKnown
    Object *object = getInstanceWithID(theObjectHandle);

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (int index = 0 ; index < theListSize ; index++)
        getAttributeWithHandle(theAttributeList[index]);

    // Does federate owns every attributes.
    // Does federate has called NegotiatedAttributeOwnershipDivestiture.
    D.Out(pdDebug, "NegotiatedDivestiture Demandeur : %u",theFederateHandle);

    for (int i = 0 ; i < theListSize ; i++) {
        list<ObjectClassAttribute *>::const_iterator a = attributeSet.begin();
        deque<ObjectAttribute *>::const_iterator j ;
        j = object->attributeState.begin();
        for (; j != object->attributeState.end(); j++,a++) {
            if (theAttributeList[i] == (*j)->Handle) {
                D.Out(pdDebug, "Attribute Name : %s", (*a)->getName());
                D.Out(pdDebug, "Attribute Handle : %u",(*j)->Handle);
                D.Out(pdDebug, "Attribute Owner : %u", (*j)->getOwner());
                if ((*j)->getOwner() != theFederateHandle)
                    throw AttributeNotOwned();
                if ((*j)->beingDivested())
                    throw AttributeAlreadyBeingDivested();
            }
        }
    }

    int compteur_acquisition = 0 ;
    int compteur_assumption = 0 ;
    int compteur_divestiture = 0 ;
    ObjectClassBroadcastList *List = NULL ;
    FederateHandle NewOwner ;

    if (server != NULL) {
        NetworkMessage *AnswerAssumption = NULL ;
        NetworkMessage *AnswerDivestiture = NULL ;
        AnswerDivestiture = new NetworkMessage ;
        AnswerAssumption = new NetworkMessage ;

        AnswerAssumption->HandleArraySize = theListSize ;

        CDiffusion *diffusionAcquisition = new CDiffusion();

        for (int i = 0 ; i < theListSize ; i++) {
            list<ObjectClassAttribute *>::const_iterator a ;
            a = attributeSet.begin();
            deque<ObjectAttribute *>::const_iterator j ;
            j = object->attributeState.begin();
            for (; j != object->attributeState.end(); j++) {
                if (theAttributeList[i] == (*j)->Handle) {
                    if ((*j)->hasCandidates()) {
                        // An attributeOwnershipAcquisition is on the way
                        // with this attribute.

                        // Le demandeur le plus récent devient propriétaire
                        NewOwner = (*j)->getCandidate(1);

                        (*j)->setOwner(NewOwner);

                        // On le supprime de la liste des demandeurs
                        (*j)->removeCandidate(NewOwner);

                        // On réinitialise divesting
                        (*j)->setDivesting(RTI_FALSE);

                        diffusionAcquisition->DiffArray[compteur_acquisition]
                            .NumeroFedere = NewOwner ;
                        diffusionAcquisition->DiffArray[compteur_acquisition]
                            .AttribHandle = (*j)->Handle ;
                        compteur_acquisition++ ;

                        AnswerDivestiture->HandleArray[compteur_divestiture]
                            = theAttributeList[i] ;
                        compteur_divestiture++ ;

                        if (strcmp((*a)->getName(),
                                   "privilegeToDelete") == 0)
                            object->Owner = NewOwner ;
                    }
                    else {
                        AnswerAssumption->HandleArray[compteur_assumption]
                            = theAttributeList[i] ;
                        (*j)->setDivesting(RTI_TRUE);
                        compteur_assumption++ ;
                    }
                }
            }
        }

        if (compteur_acquisition != 0) {
            diffusionAcquisition->size = compteur_acquisition ;
            sendToOwners(diffusionAcquisition, theObjectHandle,
                         theFederateHandle, theTag,
                         m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION);
        }
        delete diffusionAcquisition ;

        if (compteur_divestiture !=0) {
            AnswerDivestiture->Type =
                m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION ;
            AnswerDivestiture->NumeroFederation = server->federation();
            AnswerDivestiture->NumeroFedere = theFederateHandle ;
            AnswerDivestiture->Exception = e_NO_EXCEPTION ;
            AnswerDivestiture->Objectid = theObjectHandle ;
            strcpy(AnswerDivestiture->Label, "\0");
            AnswerDivestiture->HandleArraySize = compteur_divestiture ;

            sendToFederate(AnswerDivestiture,theFederateHandle);
        }
        else
            delete AnswerDivestiture ;

        if (compteur_assumption !=0) {
            AnswerAssumption->Type = m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION ;
            AnswerAssumption->NumeroFederation = server->federation();
            AnswerAssumption->NumeroFedere = theFederateHandle ;
            AnswerAssumption->Exception = e_NO_EXCEPTION ;
            AnswerAssumption->Objectid = theObjectHandle ;
            strcpy(AnswerAssumption->Label, theTag);
            AnswerAssumption->HandleArraySize = compteur_assumption ;

            List = new ObjectClassBroadcastList(AnswerAssumption,
                                                attributeSet.size());

            D.Out(pdProtocol,
                  "Object %u divestiture in class %u, now broadcasting...",
                  theObjectHandle, Handle);
            broadcastClassMessage(List);
        }
        else
            delete AnswerAssumption ;
    }
    else {
        D.Out(pdExcept,
              "NegotiatedAttributeOwnershipDivestiture should not "
              "be called on the RTIA.");
        throw RTIinternalError("NegotiatedAttributeOwnershipDivestiture "
                               "called on the RTIA.");
    }

    // Return the BroadcastList in case it had to be passed to the parent
    // class.
    return List ;
}

// ---------------------------------------------------------------------------
//! attributeOwnershipAcquisitionIfAvailable.
void ObjectClass::
attributeOwnershipAcquisitionIfAvailable(FederateHandle theFederateHandle,
                                         ObjectHandle theObjectHandle,
                                         AttributeHandle *theAttributeList,
                                         UShort theListSize)
    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           AttributeAlreadyBeingAcquired,
           RTIinternalError)
{
    // Pre-conditions checking

    //It may throw ObjectNotKnown.
    Object *object = getInstanceWithID(theObjectHandle);

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (int index = 0 ; index < theListSize ; index++)
        getAttributeWithHandle(theAttributeList[index]);

    if (server != NULL) {
        // Federate must publish the class.
        if (!isFederatePublisher(theFederateHandle)) {
            D.Out(pdExcept, "exception : ObjectClassNotPublished.");
            throw ObjectClassNotPublished();
        }

        //rem attributeSet.size()=attributeState.size()
        for (int i = 0  ; i < theListSize  ; i++) {
            list<ObjectClassAttribute *>::const_iterator a ;
            a = attributeSet.begin();
            deque<ObjectAttribute *>::const_iterator j ;
            j = object->attributeState.begin();
            for (; a != attributeSet.end(); a++,j++) {
                if (theAttributeList[i] == (*a)->Handle) {
                    // The federate has to publish attributes he desire to
                    // acquire.
                    if (!(*a)->IsPublishing(theFederateHandle) &&
                        (strcmp((*a)->getName(), "privilegeToDelete") != 0))
                        throw AttributeNotPublished();
                    // Does federate already owns some attributes.
                    if ((*j)->getOwner() == theFederateHandle)
                        throw FederateOwnsAttributes();
                    // Does federate is on the way to acquire something.
                    if ((*j)->isCandidate(theFederateHandle) != 0)
                        throw AttributeAlreadyBeingAcquired();
                }
            }
        }

        NetworkMessage *Answer_notification = new NetworkMessage ;
        Answer_notification->Type =
            m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION ;
        Answer_notification->NumeroFederation = server->federation();
        Answer_notification->NumeroFedere = theFederateHandle ;
        Answer_notification->Exception = e_NO_EXCEPTION ;
        Answer_notification->Objectid = theObjectHandle ;

        NetworkMessage *Answer_unavailable = new NetworkMessage ;
        Answer_unavailable->Type = m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE ;
        Answer_unavailable->NumeroFederation = server->federation();
        Answer_unavailable->NumeroFedere = theFederateHandle ;
        Answer_unavailable->Exception = e_NO_EXCEPTION ;
        Answer_unavailable->Objectid = theObjectHandle ;

        CDiffusion *diffusionDivestiture = new CDiffusion();

        //
        //Ce service ne doit pas ajouter le fédéré demandeur à la liste
        //des candidats!!!
        //
        int compteur_unavailable = 0 ;
        int compteur_notification = 0 ;
        int compteur_divestiture = 0 ;
        FederateHandle oldOwner ;
        for (int i = 0 ; i < theListSize ; i++) {
            list<ObjectClassAttribute *>::const_iterator a ;
            a = attributeSet.begin();
            deque<ObjectAttribute *>::const_iterator j ;
            j = object->attributeState.begin();
            for (; j != object->attributeState.end(); j++,a++) {
                if (theAttributeList[i] == (*j)->Handle) {
                    oldOwner = (*j)->getOwner();
                    if ((oldOwner == 0) || ((*j)->beingDivested())) {
                        //Cet attribut est libre ou offert par son propriétaire
                        //S'il est offert
                        if ((*j)->beingDivested()) {
                            diffusionDivestiture->DiffArray
                                [compteur_divestiture].NumeroFedere
                                = oldOwner ;
                            diffusionDivestiture->DiffArray
                                [compteur_divestiture].AttribHandle
                                = (*j)->Handle ;
                            compteur_divestiture++ ;
                        }
                        //Qu'il soit offert ou libre
                        Answer_notification->HandleArray[compteur_notification]
                            = theAttributeList[i] ;
                        (*j)->setOwner(theFederateHandle);
                        (*j)->setDivesting(RTI_FALSE);
                        compteur_notification++ ;
                        //object->Owner reste le champ de référence
                        //pour le privilegeToDelete
                        if (strcmp((*a)->getName(),"privilegeToDelete") == 0)
                            object->Owner = theFederateHandle ;
                    }
                    else {
                        //Cet attribut n'est pas disponible!!!
                        Answer_unavailable->HandleArray[compteur_unavailable]
                            = theAttributeList[i] ;
                        (*j)->addCandidate(theFederateHandle);
                        compteur_unavailable++ ;
                    }
                }
            }
        }

        if (compteur_notification != 0) {
            Answer_notification->HandleArraySize = compteur_notification ;
            sendToFederate(Answer_notification,theFederateHandle);
        }
        else
            delete Answer_notification ;

        D.Out(pdDebug,
              "Debut traitement : send divestiture notification message");

        if (compteur_divestiture != 0) {
            diffusionDivestiture->size =compteur_divestiture ;
            sendToOwners(diffusionDivestiture,theObjectHandle,
                         theFederateHandle,"\0",
                         m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION);
        }
        delete diffusionDivestiture ;

        if (compteur_unavailable != 0) {
            Answer_unavailable->HandleArraySize = compteur_unavailable ;
            sendToFederate(Answer_unavailable,theFederateHandle);
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

// ---------------------------------------------------------------------------
//! unconditionalAttributeOwnershipDivestiture.
ObjectClassBroadcastList * ObjectClass::
unconditionalAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                           ObjectHandle theObjectHandle,
                                           AttributeHandle *theAttributeList,
                                           UShort theListSize)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError)
{
    // Pre-conditions checking

    // may throw ObjectNotKnown
    Object *object = getInstanceWithID(theObjectHandle);

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (int index = 0 ; index < theListSize ; index++)
        getAttributeWithHandle(theAttributeList[index]);

    //Le fédéré est-il propriétaire de tous les attributs
    for (int i = 0 ; i < theListSize ; i++) {
        deque<ObjectAttribute *>::const_iterator j ;
        j = object->attributeState.begin();
        for (; j != object->attributeState.end(); j++) {
            if (theAttributeList[i] == (*j)->Handle) {
                if ((*j)->getOwner() != theFederateHandle)
                    throw AttributeNotOwned();
            }
        }
    }

    int compteur_assumption = 0 ;
    int compteur_acquisition = 0 ;
    NetworkMessage *AnswerAssumption = NULL ;
    ObjectClassBroadcastList *List = NULL ;
    FederateHandle NewOwner ;
    if (server != NULL) {
        AnswerAssumption = new NetworkMessage ;
        AnswerAssumption->HandleArraySize = theListSize ;
        CDiffusion *diffusionAcquisition = new CDiffusion();

        for (int i = 0 ; i < theListSize ; i++) {
            list<ObjectClassAttribute *>::const_iterator a ;
            a = attributeSet.begin();
            deque<ObjectAttribute *>::const_iterator j ;
            j = object->attributeState.begin();
            for (; j != object->attributeState.end(); j++,a++) {
                if (theAttributeList[i] == (*j)->Handle) {
                    if ((*j)->hasCandidates()) {
                        // An attributeOwnershipAcquisition is on the way
                        // on this attribute.

                        // Le demandeur le plus récent devient propriétaire
                        NewOwner = (*j)->getCandidate(1);

                        (*j)->setOwner(NewOwner);

                        // We remove it from candidate list.
                        (*j)->removeCandidate(NewOwner);

                        // We reinitialize divesting.
                        (*j)->setDivesting(RTI_FALSE);

                        diffusionAcquisition->DiffArray[compteur_acquisition]
                            .NumeroFedere = NewOwner ;
                        diffusionAcquisition->DiffArray[compteur_acquisition]
                            .AttribHandle = (*j)->Handle ;
                        compteur_acquisition++ ;

                        if (strcmp((*a)->getName(), "privilegeToDelete") == 0)
                            object->Owner = NewOwner ;
                    }
                    else {
                        AnswerAssumption->HandleArray[compteur_assumption] =
                            theAttributeList[i] ;
                        (*j)->setOwner(0);
                        (*j)->setDivesting(RTI_FALSE);
                        compteur_assumption++ ;
                    }
                }
            }
        }

        if (compteur_assumption != 0) {
            AnswerAssumption->Type = m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION ;
            AnswerAssumption->NumeroFederation = server->federation();
            AnswerAssumption->NumeroFedere = theFederateHandle ;
            AnswerAssumption->Exception = e_NO_EXCEPTION ;
            AnswerAssumption->Objectid = theObjectHandle ;
            strcpy(AnswerAssumption->Label, "\0");
            AnswerAssumption->HandleArraySize = compteur_assumption ;

            List = new ObjectClassBroadcastList(AnswerAssumption,
                                                attributeSet.size());

            D.Out(pdProtocol,
                  "Object %u updated in class %u, now broadcasting...",
                  theObjectHandle, Handle);

            broadcastClassMessage(List);
        }
        else
            delete AnswerAssumption ;

        if (compteur_acquisition != 0) {
            diffusionAcquisition->size =compteur_acquisition ;
            sendToOwners(diffusionAcquisition,theObjectHandle,
                         theFederateHandle,"\0",
                         m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION);
        }
        delete diffusionAcquisition ;
    }
    else {
        D.Out(pdExcept, "UnconditionalAttributeOwnershipDivestiture should "
              "not be called on the RTIA.");
        throw RTIinternalError("UnconditionalAttributeOwnershipDivestiture "
                               "called on the RTIA.");
    }

    // Return the BroadcastList in case it had to be passed to the parent
    // class.
    return List ;
}

// ---------------------------------------------------------------------------
//! attributeOwnershipAcquisition.
void
ObjectClass::attributeOwnershipAcquisition(FederateHandle theFederateHandle,
                                           ObjectHandle theObjectHandle,
                                           AttributeHandle *theAttributeList,
                                           UShort theListSize,
                                           const char *theTag)
    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           RTIinternalError)
{
    //Les vérifications des conditions doivent être faites avant
    //toute modification!!!

    // Pre-conditions checking

    // may throw ObjectNotKnown
    Object *object = getInstanceWithID(theObjectHandle);

    for (int i = 0  ; i < theListSize  ; i++) {
        // Do all attribute handles exist ? It may throw AttributeNotDefined.
        getAttributeWithHandle(theAttributeList[i]);

        list<ObjectClassAttribute *>::const_iterator a = attributeSet.begin();
        deque<ObjectAttribute *>::const_iterator j ;
        j = object->attributeState.begin();
        for (; j != object->attributeState.begin(); j++,a++) {
            if (theAttributeList[i] == (*j)->Handle) {
                //Le fédéré est-il déjà propriétaire de certains attributs
                if ((*j)->getOwner() == theFederateHandle)
                    throw FederateOwnsAttributes();
                //Le fédéré publie-t-il les attributs
                if (!(*a)->IsPublishing(theFederateHandle) &&
                    (strcmp((*a)->getName(), "privilegeToDelete") != 0))
                    throw AttributeNotPublished();
            }
        }
    }

    int compteur_notification = 0 ;
    int compteur_divestiture = 0 ;
    int compteur_release = 0 ;
    FederateHandle oldOwner ;
    if (server != NULL) {
        //Le fédéré doit publier la classe
        if (!isFederatePublisher(theFederateHandle)) {
            D.Out(pdExcept, "exception : ObjectClassNotPublished.");
            throw ObjectClassNotPublished();
        }

        NetworkMessage *AnswerNotification = new NetworkMessage ;
        AnswerNotification->Type =
            m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION ;
        AnswerNotification->NumeroFederation = server->federation();
        AnswerNotification->NumeroFedere = theFederateHandle ;
        AnswerNotification->Exception = e_NO_EXCEPTION ;
        AnswerNotification->Objectid = theObjectHandle ;

        CDiffusion *diffusionDivestiture = new CDiffusion();

        CDiffusion *diffusionRelease = new CDiffusion();

        for (int i = 0 ; i < theListSize ; i++) {
            list<ObjectClassAttribute *>::const_iterator a ;
            a = attributeSet.begin();
            deque<ObjectAttribute *>::const_iterator j ;
            j = object->attributeState.begin();
            for (; j != object->attributeState.end(); j++,a++) {
                if (theAttributeList[i] == (*j)->Handle) {
                    oldOwner = (*j)->getOwner();
                    if ((oldOwner == 0) || ((*j)->beingDivested())) {
                        //Cet attribut est libre ou offert par son propriétaire
                        //S'il est offert
                        if ((*j)->beingDivested()) {
                            diffusionDivestiture->
                                DiffArray[compteur_divestiture].NumeroFedere =
                                oldOwner ;
                            diffusionDivestiture->DiffArray
                                [compteur_divestiture].AttribHandle =
                                (*j)->Handle ;
                            compteur_divestiture++ ;
                        }
                        //Qu'il soit offert ou libre
                        if ((*j)->isCandidate(theFederateHandle) != 0)
                            (*j)->removeCandidate(theFederateHandle);
                        AnswerNotification->HandleArray[compteur_notification]
                            = theAttributeList[i] ;
                        (*j)->setOwner(theFederateHandle);
                        (*j)->setDivesting(RTI_FALSE);
                        compteur_notification++ ;

                        // object->Owner reste le champ de référence pour
                        // le privilegeToDelete
                        if (strcmp((*a)->getName(), "privilegeToDelete") == 0)
                            object->Owner = theFederateHandle ;
                    }
                    else {
                        diffusionRelease->DiffArray[compteur_release]
                            .NumeroFedere = oldOwner ;
                        diffusionRelease->DiffArray[compteur_release]
                            .AttribHandle = (*j)->Handle ;
                        compteur_release++ ;

                        //On l'enlève de la liste des demandeurs s'il y était
                        (*j)->removeCandidate(theFederateHandle);

                        //pour le rajouter en 1ere position
                        (*j)->addCandidate(theFederateHandle);
                    }
                }
            }
        }

        if (compteur_notification != 0) {
            AnswerNotification->HandleArraySize = compteur_notification ;
            sendToFederate(AnswerNotification,theFederateHandle);
        }
        else
            delete AnswerNotification ;

        if (compteur_divestiture != 0) {
            diffusionDivestiture->size =compteur_divestiture ;
            sendToOwners(diffusionDivestiture,theObjectHandle,
                         theFederateHandle,"\0",
                         m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION);
        }
        delete diffusionDivestiture ;

        if (compteur_release != 0) {
            diffusionRelease->size =compteur_release ;
            sendToOwners(diffusionRelease,theObjectHandle,theFederateHandle,
                         theTag, m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE);
        }
        delete diffusionRelease ;
    }
    else {
        D.Out(pdExcept,
              "AttributeOwnershipAcquisition should not be called "
              "on the RTIA.");
        throw RTIinternalError("AttributeOwnershipAcquisition called "
                               "on the RTIA");
    }
}

// ---------------------------------------------------------------------------
//! cancelNegotiatedAttributeOwnershipDivestiture.
void
ObjectClass::
cancelNegotiatedAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                              ObjectHandle theObjectHandle,
                                              AttributeHandle *attributeList,
                                              UShort theListSize)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           AttributeDivestitureWasNotRequested,
           RTIinternalError)
{
    // Pre-conditions checking

    // may throw ObjectNotKnown
    Object *object = getInstanceWithID(theObjectHandle);

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (int index = 0 ; index < theListSize ; index++)
        getAttributeWithHandle(attributeList[index]);

    for (int i = 0 ; i < theListSize ; i++) {
        deque<ObjectAttribute *>::const_iterator j ;
        j = object->attributeState.begin();
        for (; j != object->attributeState.end(); j++) {
            if (attributeList[i] == (*j)->Handle) {
                // Does federate owns every attributes.
                if ((*j)->getOwner() != theFederateHandle)
                    throw AttributeNotOwned();
                // Does federate called NegotiatedAttributeOwnershipDivestiture
                if (!(*j)->beingDivested())
                    throw AttributeDivestitureWasNotRequested();
            }
        }
    }

    if (server != NULL) {
        for (int i = 0 ; i < theListSize ; i++) {
            deque<ObjectAttribute *>::const_iterator j ;
            j = object->attributeState.begin();
            for (; j != object->attributeState.end(); j++) {
                if (attributeList[i] == (*j)->Handle)
                    (*j)->setDivesting(RTI_FALSE);
            }
        }
    }
    else {
        D.Out(pdExcept, "CancelNegotiatedAttributeOwnershipDivestiture should "
              "not be called on the RTIA.");
        throw RTIinternalError("CancelNegotiatedAttributeOwnershipDivestiture "
                               "called on the RTIA.");
    }
}

// ---------------------------------------------------------------------------
//! attributeOwnershipRealeaseResponse.
AttributeHandleSet *
ObjectClass::
attributeOwnershipRealeaseResponse(FederateHandle theFederateHandle,
                                   ObjectHandle theObjectHandle,
                                   AttributeHandle *theAttributeList,
                                   UShort theListSize)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           FederateWasNotAskedToReleaseAttribute,
           RTIinternalError)
{
    // Pre-conditions checking

    // may throw ObjectNotKnown
    Object *object = getInstanceWithID(theObjectHandle);

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (int index = 0 ; index < theListSize ; index++)
        getAttributeWithHandle(theAttributeList[index]);

    //Le fédéré est-il propriétaire de tous les attributs
    //Y a-t-il des acquéreurs pour les attributs
    ObjectAttribute *attribute = NULL ;
    for (int i = 0 ; i < theListSize ; i++) {
        deque<ObjectAttribute *>::const_iterator j ;
        j = object->attributeState.begin();
        for (; j != object->attributeState.end(); j++) {
            if (theAttributeList[i] == (*j)->Handle) {
                if ((*j)->getOwner() != theFederateHandle)
                    throw AttributeNotOwned();
                if (!(*j)->hasCandidates())
                    throw FederateWasNotAskedToReleaseAttribute();
            }
        }
    }

    int compteur_acquisition = 0 ;
    FederateHandle newOwner ;
    AttributeHandleSet *theAttribute ;
    if (server != NULL) {
        CDiffusion *diffusionAcquisition = new CDiffusion();

        theAttribute = AttributeHandleSetFactory::create(theListSize);

        for (int i = 0 ; i < theListSize ; i++) {
            list<ObjectClassAttribute *>::const_iterator a ;
            a = attributeSet.begin();
            deque<ObjectAttribute *>::const_iterator j ;
            j = object->attributeState.begin();
            for (; j != object->attributeState.end(); j++,a++) {
                if (theAttributeList[i] == (*j)->Handle) {
                    //Le demandeur le plus récent devient propriétaire
                    newOwner = (*j)->getCandidate(1);

                    (*j)->setOwner(newOwner);

                    //On le supprime de la liste des demandeurs
                    (*j)->removeCandidate(newOwner);

                    //On réinitialise divesting
                    (*j)->setDivesting(RTI_FALSE);

                    diffusionAcquisition->
                        DiffArray[compteur_acquisition].NumeroFedere =
                        newOwner ;
                    diffusionAcquisition->
                        DiffArray[compteur_acquisition].AttribHandle =
                        (*j)->Handle ;
                    compteur_acquisition++ ;
                    theAttribute->add((*j)->Handle);

                    D.Out(pdDebug, "Acquisition handle %u compteur %u",
                          theAttributeList[i], compteur_acquisition);

                    if (strcmp((*a)->getName(), "privilegeToDelete") == 0)
                        object->Owner = newOwner ;
                }
            }
        }

        if (compteur_acquisition != 0) {
            diffusionAcquisition->size =compteur_acquisition ;
            sendToOwners(diffusionAcquisition, theObjectHandle,
                         theFederateHandle, "\0",
                         m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION);
        }
        delete diffusionAcquisition ;
    }
    else {
        D.Out(pdExcept, "NegotiatedAttributeOwnershipDivestiture should not "
              "be called on the RTIA.");
        throw RTIinternalError("NegotiatedAttributeOwnershipDivestiture called"
                               " on the RTIA.");
    }

    return(theAttribute);
}

// ---------------------------------------------------------------------------
//! cancelAttributeOwnershipAcquisition.
void
ObjectClass::
cancelAttributeOwnershipAcquisition(FederateHandle federate_handle,
                                    ObjectHandle object_handle,
                                    AttributeHandle *attribute_list,
                                    UShort list_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeAlreadyOwned,
           AttributeAcquisitionWasNotRequested,
           RTIinternalError)
{
    // Pre-conditions checking

    //It may throw ObjectNotKnown.
    Object *object = getInstanceWithID(object_handle);

    // Do all attribute handles exist ? It may throw AttributeNotDefined.
    for (int index = 0 ; index < list_size ; index++)
        getAttributeWithHandle(attribute_list[index]);

    for (int i = 0 ; i < list_size ; i++)
        D.Out(pdDebug, "CancelAcquisition Object %u Attribute %u ",
              object_handle, attribute_list[i]);

    if (server != NULL) {
        //rem attributeSet.size()=attributeState.size()
        for (int i = 0  ; i < list_size  ; i++) {
            list<ObjectClassAttribute *>::const_iterator a ;
            a = attributeSet.begin();
            deque<ObjectAttribute *>::const_iterator j ;
            j = object->attributeState.begin();
            for (; a != attributeSet.end(); a++,j++) {
                if (attribute_list[i] == (*a)->Handle) {
                    D.Out(pdDebug, "Attribut %u Owner %u", attribute_list[i],
                          (*j)->getOwner());
                    // Does federate is already owning some attributes ?
                    if ((*j)->getOwner() == federate_handle)
                        throw AttributeAlreadyOwned();
                    // Does federate is already doing an acquisition ?
                    if ((*j)->isCandidate(federate_handle) == 0)
                        throw AttributeAcquisitionWasNotRequested();
                }
            }
        }

        NetworkMessage *answer_confirmation = new NetworkMessage ;
        answer_confirmation->Type =
            m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION ;
        answer_confirmation->NumeroFederation = server->federation();
        answer_confirmation->NumeroFedere = federate_handle ;
        answer_confirmation->Exception = e_NO_EXCEPTION ;
        answer_confirmation->Objectid = object_handle ;

        int compteur_confirmation = 0 ;
        for (int i = 0 ; i < list_size ; i++) {
            deque<ObjectAttribute *>::const_iterator j ;
            j = object->attributeState.begin();
            for (; j != object->attributeState.end(); j++) {
                if (attribute_list[i] == (*j)->Handle) {
                    answer_confirmation->HandleArray[compteur_confirmation]
                        = attribute_list[i] ;

                    // We remove federate from candidates.
                    (*j)->removeCandidate(federate_handle);
                    D.Out(pdDebug,
                          "Adding federate %u to attribute %u object %u",
                          federate_handle, attribute_list[i], object_handle);
                    compteur_confirmation++ ;
                }
            }
        }

        if (compteur_confirmation != 0) {
            answer_confirmation->HandleArraySize = compteur_confirmation ;
            sendToFederate(answer_confirmation,federate_handle);
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

} // namespace certi

// $Id: ObjectClass.cc,v 3.7 2003/01/20 21:49:15 breholee Exp $
