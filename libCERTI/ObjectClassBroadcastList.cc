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

#include <assert.h>

#include <algorithm>

#include "NM_Classes.hh"
#include "ObjectClassBroadcastList.hh"
#include "PrettyDebug.hh"

#include <include/make_unique.hh>

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
        if (stateFor(attrIndex) == State::Waiting) {
            return true;
        }
    }

    return false;
}

bool ObjectBroadcastLine::isWaitingAll(const std::vector<AttributeHandle>& attributes) const
{
    for (auto& handle : attributes) {
        if (stateFor(handle) != State::Waiting) {
            return false;
        }
    }

    return true;
}

ObjectClassBroadcastList::ObjectClassBroadcastList(std::unique_ptr<NetworkMessage> message,
                                                   AttributeHandle maxAttHandle)
    : my_message{std::move(message)}, maxHandle{maxAttHandle}
{
    if (!my_message) {
        throw RTIinternalError("Null Broadcast Message.");
    }

    /* Initialize specific pointer
     * FIXMEthis is ugly and will be reworked
     */
    switch (my_message->getMessageType()) {
    case NetworkMessage::Type::REMOVE_OBJECT:
        msgRO = static_cast<NM_Remove_Object*>(my_message.get());
        break;
    case NetworkMessage::Type::DISCOVER_OBJECT:
        msgDO = static_cast<NM_Discover_Object*>(my_message.get());
        break;
    case NetworkMessage::Type::REFLECT_ATTRIBUTE_VALUES:
        msgRAV = static_cast<NM_Reflect_Attribute_Values*>(my_message.get());
        break;
    case NetworkMessage::Type::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
        msgRAOA = static_cast<NM_Request_Attribute_Ownership_Assumption*>(my_message.get());
        break;
    case NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
        msgAODN = static_cast<NM_Attribute_Ownership_Divestiture_Notification*>(my_message.get());
        break;
    default:
        throw RTIinternalError("Unexpected type of message");
        break;
    }

    // Add reference of the sender.
    if (my_message->getFederate() != 0) {
        my_lines.emplace_back(my_message->getFederate(), ObjectBroadcastLine::State::Sent);
    }
}

NetworkMessage& ObjectClassBroadcastList::getMsg()
{
    return *my_message;
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

void ObjectClassBroadcastList::addFederate(FederateHandle theFederate, AttributeHandle theAttribute)
{
    if (theAttribute > maxHandle) {
        Debug(D, pdExcept) << "Bad attribute handle: " << theAttribute << " > " << maxHandle << std::endl;
        throw RTIinternalError("Invalid Attribute Handle");
    }

    auto it = std::find_if(begin(my_lines), end(my_lines), [&](const ObjectBroadcastLine& line) {
        return line.getFederate() == theFederate;
    });

    if (it == end(my_lines)) {
        Debug(D, pdRegister) << "Adding new line in list for Federate " << theFederate << std::endl;
        my_lines.emplace_back(theFederate, ObjectBroadcastLine::State::NotSub);
        my_lines.back().setState(theAttribute, ObjectBroadcastLine::State::Waiting);
    }
    else if (it->stateFor(theAttribute) != ObjectBroadcastLine::State::Sent) {
        it->setState(theAttribute, ObjectBroadcastLine::State::Waiting);
        Debug(D, pdRegister) << "List attribute " << theAttribute << " for Federate " << theFederate
                             << " is now :waiting." << std::endl;
    }
    else {
        Debug(D, pdTrace) << "Message already sent to federate " << theFederate << " about attribute " << theAttribute
                          << std::endl;
    }
}

Responses ObjectClassBroadcastList::preparePendingMessage(SecurityServer& server)
{
    Debug(G, pdGendoc) << "enter ObjectClassBroadcastList::sendPendingMessage" << std::endl;
    switch (my_message->getMessageType()) {
    case NetworkMessage::Type::REFLECT_ATTRIBUTE_VALUES:
    case NetworkMessage::Type::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
        return preparePendingRAVMessage(server);
    case NetworkMessage::Type::DISCOVER_OBJECT:
    case NetworkMessage::Type::REMOVE_OBJECT:
        return preparePendingDOMessage(server);
    default:
        throw RTIinternalError("Unknown message type to broadcast.");
        Debug(G, pdGendoc) << "exit  ObjectClassBroadcastList::sendPendingMessage" << std::endl;
    }
}

void ObjectClassBroadcastList::upcastTo(ObjectClassHandle objectClass)
{
    /* Initialize specific pointer
     * FIXME this is ugly and will be reworked
     */
    switch (my_message->getMessageType()) {
    case NetworkMessage::Type::REMOVE_OBJECT:
        msgRO->setObjectClass(objectClass);
        break;
    case NetworkMessage::Type::DISCOVER_OBJECT:
        msgDO->setObjectClass(objectClass);
        break;
    case NetworkMessage::Type::REFLECT_ATTRIBUTE_VALUES:
        // FIXME nothing TODO RAV does not embed object class?
        //msgRAV->setObjectClass(objectClass);
        break;
    case NetworkMessage::Type::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
        // FIXME nothing TODO RAOA does not embed object class?
        //msgRAOA->setObjectClass(objectClass);
        break;
    case NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
        // FIXME nothing TODO AODN does not embed object class?
        //msgAODN->setObjectClass(objectClass);
        break;
    default:
        throw RTIinternalError("Unexpected type of message");
        break;
    }
}

Responses ObjectClassBroadcastList::preparePendingDOMessage(SecurityServer& server)
{
    Responses responses;

    std::vector<Socket*> sockets;
    for (auto& line : my_lines) {
        // If the federate waits for a message (attribute 0 == waiting)
        if (line.stateFor(0) == ObjectBroadcastLine::State::Waiting) {
            // 1. Prepare message for federate
            Debug(D, pdProtocol) << "Broadcasting message to Federate " << line.getFederate() << std::endl;
            try {
#ifdef HLA_USES_UDP
                sockets.push_back(server.getSocketLink(line.getFederate(), BEST_EFFORT));
#else
                sockets.push_back(server.getSocketLink(line.getFederate()));
#endif
            }
            catch (Exception& e) {
                Debug(D, pdExcept) << "Reference to a killed Federate while broadcasting." << std::endl;
            }

            // 2. Mark the message as sent
            line.setState(0, ObjectBroadcastLine::State::Sent);
        }
        else {
            Debug(D, pdProtocol) << "No message sent to Federate " << line.getFederate() << std::endl;
        }
    }

    if (sockets.size() != 0) {
        if (msgDO) {
            responses.emplace_back(sockets, createResponseMessage(msgDO));
        }
        else if (msgRO) {
            responses.emplace_back(sockets, createResponseMessage(msgRO));
        }
    }

    return responses;
}

Responses ObjectClassBroadcastList::preparePendingRAVMessage(SecurityServer& server)
{
    Debug(G, pdGendoc) << "enter ObjectClassBroadcastList::sendPendingRAVMessage" << std::endl;

    Responses responses;

    std::vector<AttributeHandle> relevantAttributes;

    if (msgRAV) {
        relevantAttributes = msgRAV->getAttributes();
    }

    if (msgRAOA) {
        relevantAttributes = msgRAOA->getAttributes();
    }

    for (auto& line : my_lines) {
        // If *at least* one of the attributes is waiting
        if (line.isWaitingAny(maxHandle)) {
            std::unique_ptr<NetworkMessage> currentMessage;

            if (line.isWaitingAll(relevantAttributes)) {
                // YES: Nothing to do.
                if (msgRAV) {
                    currentMessage = createResponseMessage(msgRAV);
                }
                if (msgRAOA) {
                    currentMessage = createResponseMessage(msgRAOA);
                }
                Debug(D, pdProtocol) << "Broadcasting complete message to Federate " << line.getFederate() << std::endl;
            }
            else {
                // NO: Create a new message containing only relevant attributes.
                if (msgRAV) {
                    currentMessage = createResponseMessageWithValues(msgRAV, line);
                }
                if (msgRAOA) {
                    currentMessage = createResponseMessage(msgRAOA, line);
                }
                Debug(D, pdProtocol) << "Broadcasting reduced message to Federate " << line.getFederate() << std::endl;
            }

            // 2. Send appropriate message
            try {
                std::vector<Socket*> sockets;
#ifdef HLA_USES_UDP
                sockets.push_back(server.getSocketLink(line.getFederate(), BEST_EFFORT));
#else
                sockets.push_back(server.getSocketLink(line.getFederate()));
#endif
                responses.emplace_back(sockets, std::move(currentMessage));
            }
            catch (Exception& e) {
                Debug(D, pdExcept) << "Reference to a killed Federate while broadcasting." << std::endl;
            }

            // 3. mark attributes as sent.
            for (unsigned int attrIndex = 1; attrIndex <= maxHandle; attrIndex++) {
                if (line.stateFor(attrIndex) == ObjectBroadcastLine::State::Waiting) {
                    line.setState(attrIndex, ObjectBroadcastLine::State::Sent);
                }
            }
            //*/
        }
        else {
            Debug(D, pdProtocol) << "No message sent to Federate " << line.getFederate() << std::endl;
        }

        Debug(G, pdGendoc) << "exit  ObjectClassBroadcastList::sendPendingRAVMessage" << std::endl;
    }

    return responses;
}

template <typename T>
std::unique_ptr<NetworkMessage> ObjectClassBroadcastList::createResponseMessage(T* message,
                                                                                const ObjectBroadcastLine& line)
{
    auto reducedMessage = make_unique<T>(*message);

    // Copy attributes that are in the Waiting state in Line.
    uint32_t currentSize = 0;
    reducedMessage->setAttributesSize(currentSize);

    for (uint32_t i = 0; i < message->getAttributesSize(); ++i) {
        auto currentAttrib = message->getAttributes(i);
        if (line.stateFor(currentAttrib) == ObjectBroadcastLine::State::Waiting) {
            // Update number of attributes in ReducedMessage.
            ++currentSize;

            reducedMessage->setAttributesSize(currentSize);
            reducedMessage->setAttributes(currentAttrib, currentSize - 1);
        }
    }
    return std::unique_ptr<NetworkMessage>(static_cast<NetworkMessage*>(reducedMessage.release()));
}

template <typename T>
std::unique_ptr<NetworkMessage>
ObjectClassBroadcastList::createResponseMessageWithValues(T* message, const ObjectBroadcastLine& line)
{
    auto reducedMessage = make_unique<T>(*message);

    // Copy attributes that are in the Waiting state in Line.
    uint32_t currentSize = 0;
    reducedMessage->setAttributesSize(currentSize);

    for (uint32_t i = 0; i < message->getAttributesSize(); ++i) {
        auto currentAttrib = message->getAttributes(i);
        if (line.stateFor(currentAttrib) == ObjectBroadcastLine::State::Waiting) {
            // Update number of attributes in ReducedMessage.
            ++currentSize;

            reducedMessage->setAttributesSize(currentSize);
            reducedMessage->setAttributes(currentAttrib, currentSize - 1);

            reducedMessage->setValuesSize(currentSize);
            reducedMessage->setValues(message->getValues(i), currentSize - 1);
        }
    }
    return std::unique_ptr<NetworkMessage>(static_cast<NetworkMessage*>(reducedMessage.release()));
}

template <typename T>
std::unique_ptr<NetworkMessage> ObjectClassBroadcastList::createResponseMessage(T* message)
{
    auto reducedMessage = make_unique<T>(*message);

    return std::unique_ptr<NetworkMessage>(static_cast<NetworkMessage*>(reducedMessage.release()));
}
} // namespace certi
