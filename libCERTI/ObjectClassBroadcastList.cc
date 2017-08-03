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

#include <assert.h>

#include <algorithm>

#include "NM_Classes.hh"
#include "ObjectClassBroadcastList.hh"
#include "PrettyDebug.hh"

using std::list;

namespace certi {

static PrettyDebug D("BROADCAST", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);

ObjectBroadcastLine::ObjectBroadcastLine(FederateHandle federate, ObjectBroadcastLine::State initial_state)
    : my_federate{federate}, my_initial_state{initial_state}
{
}

FederateHandle ObjectBroadcastLine::getFederate() const
{
    return my_federate;
}

ObjectBroadcastLine::State ObjectBroadcastLine::stateFor(const AttributeHandle attribute) const
{
    auto it = my_states.find(attribute);
    if (it == end(my_states)) {
        return my_initial_state;
    }
    else {
        return it->second;
    }
}

void ObjectBroadcastLine::setState(const AttributeHandle attribute, const State value)
{
    my_states[attribute] = value;
}

bool ObjectBroadcastLine::isWaitingAny(const AttributeHandle max_handle) const
{
    for (unsigned int attrIndex = 1; attrIndex <= max_handle; ++attrIndex) {
        if (stateFor(attrIndex) == ObjectBroadcastLine::waiting) {
            return true;
        }
    }

    return false;
}

bool ObjectBroadcastLine::isWaitingAll(const std::vector<AttributeHandle>& attributes) const
{
    for (auto& handle : attributes) {
        if (stateFor(handle) != waiting) {
            return false;
        }
    }

    return true;
}

ObjectClassBroadcastList::ObjectClassBroadcastList(NetworkMessage* message,
                                                   AttributeHandle maxAttHandle) throw(RTIinternalError)
    : msg{message}
    , msgRO{nullptr}
    , msgDO{nullptr}
    , msgRAV{nullptr}
    , msgRAOA{nullptr}
    , msgAODN{nullptr}
    , maxHandle{maxAttHandle}
{
    ObjectBroadcastLine* firstLine = NULL;

    if (msg == nullptr) {
        throw RTIinternalError("Null Broadcast Message.");
    }

    /* Initialize specific pointer
                                                            * FIXMEthis is ugly and will be reworked
                                                            */
    switch (msg->getMessageType()) {
    case NetworkMessage::REMOVE_OBJECT:
        msgRO = static_cast<NM_Remove_Object*>(msg);
        break;
    case NetworkMessage::DISCOVER_OBJECT:
        msgDO = static_cast<NM_Discover_Object*>(msg);
        break;
    case NetworkMessage::REFLECT_ATTRIBUTE_VALUES:
        msgRAV = static_cast<NM_Reflect_Attribute_Values*>(msg);
        break;
    case NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
        msgRAOA = static_cast<NM_Request_Attribute_Ownership_Assumption*>(msg);
        break;
    case NetworkMessage::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
        msgAODN = static_cast<NM_Attribute_Ownership_Divestiture_Notification*>(msg);
        break;
    default:
        throw RTIinternalError("Unexpected type of message");
        break;
    }

    // Add reference of the sender.
    if (msg->getFederate() != 0) {
        firstLine = new ObjectBroadcastLine(msg->getFederate(), ObjectBroadcastLine::sent);
        lines.push_front(firstLine);
    }
}

ObjectClassBroadcastList::~ObjectClassBroadcastList()
{
    clear();
}

NetworkMessage* ObjectClassBroadcastList::getMsg()
{
    return msg;
}

NM_Remove_Object* ObjectClassBroadcastList::getMsgRO()
{
    return msgRO;
}

NM_Discover_Object* ObjectClassBroadcastList::getMsgDO()
{
    return msgDO;
}

NM_Reflect_Attribute_Values* ObjectClassBroadcastList::getMsgRAV()
{
    return msgRAV;
}

NM_Request_Attribute_Ownership_Assumption* ObjectClassBroadcastList::getMsgRAOA()
{
    return msgRAOA;
}

NM_Attribute_Ownership_Divestiture_Notification* ObjectClassBroadcastList::getMsgAODN()
{
    return msgAODN;
}

void ObjectClassBroadcastList::clear()
{
    delete msg;
    msg = nullptr;

    maxHandle = 0;

    while (!lines.empty()) {
        delete lines.front();
        lines.pop_front();
    }

    Debug(D, pdTerm) << "List is now empty" << std::endl;
}

void ObjectClassBroadcastList::addFederate(FederateHandle theFederate, AttributeHandle theAttribute)
{
    if (theAttribute > maxHandle) {
        Debug(D, pdExcept) << "Bad attribute handle: " << theAttribute << " > " << maxHandle << std::endl;
        throw RTIinternalError("Invalid Attribute Handle");
    }

    ObjectBroadcastLine* line = getLineWithFederate(theFederate);

    if (line == 0) {
        line = new ObjectBroadcastLine(theFederate, ObjectBroadcastLine::notSub);
        lines.push_front(line);
        Debug(D, pdRegister) << "Adding new line in list for Federate " << theFederate << std::endl;
    }

    if (line->stateFor(theAttribute) != ObjectBroadcastLine::sent) {
        line->setState(theAttribute, ObjectBroadcastLine::waiting);
        Debug(D, pdRegister) << "List attribute " << theAttribute << " for Federate " << theFederate
                             << " is now ObjectBroadcastLine::waiting." << std::endl;
    }
    else {
        Debug(D, pdTrace) << "Message already sent to federate " << theFederate << " about attribute " << theAttribute
                          << std::endl;
    }
}

void ObjectClassBroadcastList::sendPendingMessage(SecurityServer* server)
{
    Debug(G, pdGendoc) << "enter ObjectClassBroadcastList::sendPendingMessage" << std::endl;
    switch (msg->getMessageType()) {
    case NetworkMessage::REFLECT_ATTRIBUTE_VALUES:
    case NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
        sendPendingRAVMessage(server);
        break;

    case NetworkMessage::DISCOVER_OBJECT:
    case NetworkMessage::REMOVE_OBJECT:
        sendPendingDOMessage(server);
        break;

    default:
        throw RTIinternalError("Unknown message type to broadcast.");
        Debug(G, pdGendoc) << "exit  ObjectClassBroadcastList::sendPendingMessage" << std::endl;
    }
}

void ObjectClassBroadcastList::upcastTo(ObjectClassHandle objectClass)
{
    /* Initialize specific pointer
                                                            * FIXMEthis is ugly and will be reworked
                                                            * */
    switch (msg->getMessageType()) {
    case NetworkMessage::REMOVE_OBJECT:
        msgRO->setObjectClass(objectClass);
        break;
    case NetworkMessage::DISCOVER_OBJECT:
        msgDO->setObjectClass(objectClass);
        break;
    case NetworkMessage::REFLECT_ATTRIBUTE_VALUES:
        // FIXME nothing TODO RAV does not embed object class?
        //msgRAV->setObjectClass(objectClass);
        break;
    case NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
        // FIXME nothing TODO RAOA does not embed object class?
        //msgRAOA->setObjectClass(objectClass);
        break;
    case NetworkMessage::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
        // FIXME nothing TODO AODN does not embed object class?
        //msgAODN->setObjectClass(objectClass);
        break;
    default:
        throw RTIinternalError("Unexpected type of message");
        break;
    }
}

void ObjectClassBroadcastList::sendPendingDOMessage(SecurityServer* server)
{
    for (auto& line : lines) {
        // If the federate waits for a message (attribute 0 == waiting)
        if (line->stateFor(0) == ObjectBroadcastLine::waiting) {
            // 1. Send the message to the federate
            Debug(D, pdProtocol) << "Broadcasting message to Federate " << line->getFederate() << std::endl;
            try {
                Socket* socket = server->getSocketLink(line->getFederate());
                if (socket) {
                    msg->send(socket, NM_msgBufSend);
                }
            }
            catch (RTIinternalError& e) {
                Debug(D, pdExcept) << "Reference to a killed Federate while broadcasting." << std::endl;
            }
            catch (NetworkError& e) {
                Debug(D, pdExcept) << "Network error while broadcasting, ignoring." << std::endl;
            }

            // 2. Mark the message as sent
            line->setState(0, ObjectBroadcastLine::sent);
        }
        else {
            Debug(D, pdProtocol) << "No message sent to Federate " << line->getFederate() << std::endl;
        }
    }
}

void ObjectClassBroadcastList::sendPendingRAVMessage(SecurityServer* server)
{
    std::vector<AttributeHandle> relevantAttributes;

    if (NULL != msgRAV) {
        relevantAttributes = msgRAV->getAttributes();
    }

    if (NULL != msgRAOA) {
        relevantAttributes = msgRAOA->getAttributes();
    }

    Debug(G, pdGendoc) << "enter ObjectClassBroadcastList::sendPendingRAVMessage" << std::endl;
    // For each line :
    for (auto& line : lines) {
        // Si AU MOINS UN des attributs est en ObjectBroadcastLine::waiting
        if (line->isWaitingAny(maxHandle)) {
            NetworkMessage* currentMessage{nullptr};

            if (line->isWaitingAll(relevantAttributes)) {
                // YES: Nothing to do.
                currentMessage = msg;
                Debug(D, pdProtocol) << "Broadcasting complete message to Federate " << line->getFederate()
                                     << std::endl;
            }
            else {
                // NO: Create a new message containing only relevant attributes.
                if (msgRAV) {
                    currentMessage = createReducedMessageWithValue(msgRAV, line);
                }
                if (msgRAOA) {
                    currentMessage = createReducedMessage(msgRAOA, line);
                }
                Debug(D, pdProtocol) << "Broadcasting reduced message to Federate " << line->getFederate() << std::endl;
            }

            // 2. Send appropriate message
            try {
#ifdef HLA_USES_UDP
                Socket* socket = server->getSocketLink(line->getFederate(), BEST_EFFORT);
#else
                Socket* socket = server->getSocketLink(line->getFederate());
#endif
                if (socket) {
                    Debug(G, pdGendoc) << "                                 sendPendingRAVMessage=====> write"
                                       << std::endl;
                    currentMessage->send(socket, NM_msgBufSend);
                }
            }
            catch (RTIinternalError& e) {
                Debug(D, pdExcept) << "Reference to a killed Federate while broadcasting." << std::endl;
            }
            catch (NetworkError& e) {
                Debug(D, pdExcept) << "Network error while broadcasting, ignoring." << std::endl;
            }

            // 3. mark attributes as sent.
            for (unsigned int attrIndex = 1; attrIndex <= maxHandle; attrIndex++) {
                if (line->stateFor(attrIndex) == ObjectBroadcastLine::waiting) {
                    line->setState(attrIndex, ObjectBroadcastLine::sent);
                }
            }

            // 4. Delete any created message
            if (currentMessage != msg) {
                delete currentMessage;
            }
        }
        else {
            Debug(D, pdProtocol) << "No message sent to Federate " << line->getFederate() << std::endl;
        }

        Debug(G, pdGendoc) << "exit  ObjectClassBroadcastList::sendPendingRAVMessage" << std::endl;
    }
}

template <typename T>
T* ObjectClassBroadcastList::createReducedMessage(T* msg, ObjectBroadcastLine* line)
{
    // Create the reduced message
    // FIXME whould be better to msg->clone() if a clone method was generated
    // FIXME or maybe copy constructor
    // FIXME T *reducedMessage = new T(msg)
    T* reducedMessage = new T();

    // Copy static informations.
    reducedMessage->setException(msg->getException());
    reducedMessage->setFederation(msg->getFederation());
    reducedMessage->setFederate(msg->getFederate());
    reducedMessage->setObject(msg->getObject());
    if (msg->isDated()) {
        reducedMessage->setDate(msg->getDate());
    }
    if (msg->isTagged()) {
        reducedMessage->setTag(msg->getTag());
    }

    if (msg->isLabelled()) {
        reducedMessage->setLabel(msg->getLabel());
    }

    // Copy attributes that are in the bsWaiting state in Line.
    uint32_t currentSize;
    AttributeHandle currentAttrib;

    currentSize = 0;
    reducedMessage->setAttributesSize(currentSize);

    for (uint32_t i = 0; i < msg->getAttributesSize(); ++i) {
        currentAttrib = msg->getAttributes(i);
        if (line->stateFor(currentAttrib) == ObjectBroadcastLine::waiting) {
            // Update number of attributes in ReducedMessage.
            ++currentSize;
            ;
            reducedMessage->setAttributesSize(currentSize);
            // Copy Attribute Handle.
            reducedMessage->setAttributes(currentAttrib, currentSize - 1);
        }
    }
    return reducedMessage;
}

template <typename T>
T* ObjectClassBroadcastList::createReducedMessageWithValue(T* msg, ObjectBroadcastLine* line)
{
    // Create the reduced message
    // FIXME whould be better to msg->clone() if a clone method was generated
    // FIXME or maybe copy constructor
    // FIXME T *reducedMessage = new T(msg)
    T* reducedMessage = new T();

    // Copy static informations.
    reducedMessage->setException(msg->getException());
    reducedMessage->setFederation(msg->getFederation());
    reducedMessage->setFederate(msg->getFederate());
    reducedMessage->setObject(msg->getObject());
    if (msg->isDated()) {
        reducedMessage->setDate(msg->getDate());
    }
    if (msg->isTagged()) {
        reducedMessage->setTag(msg->getTag());
    }

    if (msg->isLabelled()) {
        reducedMessage->setLabel(msg->getLabel());
    }

    // Copy attributes that are in the bsWaiting state in Line.
    uint32_t currentSize;
    AttributeHandle currentAttrib;

    currentSize = 0;
    reducedMessage->setAttributesSize(currentSize);

    for (uint32_t i = 0; i < msg->getAttributesSize(); ++i) {
        currentAttrib = msg->getAttributes(i);
        if (line->stateFor(currentAttrib) == ObjectBroadcastLine::waiting) {
            // Update number of attributes in ReducedMessage.
            ++currentSize;
            reducedMessage->setAttributesSize(currentSize);
            // Copy Attribute Handle.
            reducedMessage->setAttributes(currentAttrib, currentSize - 1);

            reducedMessage->setValuesSize(currentSize);
            // Copy Attribute Value.
            reducedMessage->setValues(msg->getValues(i), currentSize - 1);
        }
    }
    return reducedMessage;
}

ObjectBroadcastLine* ObjectClassBroadcastList::getLineWithFederate(FederateHandle theFederate)
{
    list<ObjectBroadcastLine*>::iterator i;
    for (i = lines.begin(); i != lines.end(); i++) {
        if ((*i)->getFederate() == theFederate)
            return (*i);
    }

    return 0;
}
} // namespace certi
