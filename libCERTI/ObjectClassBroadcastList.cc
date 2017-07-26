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

#include "NM_Classes.hh"
#include "ObjectClassBroadcastList.hh"
#include "PrettyDebug.hh"

using std::list;

namespace certi {

static PrettyDebug D("BROADCAST", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);

ObjectBroadcastLine::ObjectBroadcastLine(FederateHandle theFederate, ObjectBroadcastLine::State init_state)
    : Federate{theFederate}
{
    std::fill_n(state, MAX_STATE_SIZE + 1, init_state);
}

ObjectClassBroadcastList::ObjectClassBroadcastList(NetworkMessage* msg,
                                                   AttributeHandle maxAttHandle) throw(RTIinternalError)
    : maxHandle(maxAttHandle)
{
    ObjectBroadcastLine* firstLine = NULL;

    if (NULL == msg) {
        throw RTIinternalError("Null Broadcast Message.");
    }

    this->msg = msg;
    /* RAZ specific pointers */
    msgRO = NULL;
    msgDO = NULL;
    msgRAV = NULL;
    msgRAOA = NULL;
    msgAODN = NULL;

    /* Initialize specific pointer
     * FIXME : this is ugly and will be reworked
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
    msg = NULL;

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

    if (line->state[theAttribute] != ObjectBroadcastLine::sent) {
        line->state[theAttribute] = ObjectBroadcastLine::waiting;
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
     * FIXME : this is ugly and will be reworked
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
    Socket* socket = NULL;

    // Pour chaque ligne de la liste
    list<ObjectBroadcastLine*>::iterator i;
    for (i = lines.begin(); i != lines.end(); ++i) {
        // Si le federe attend un message(attribute 0 en attente)
        if ((*i)->state[0] == ObjectBroadcastLine::waiting) {
            // 1. Envoyer le message au federe
            Debug(D, pdProtocol) << "Broadcasting message to Federate " << (*i)->Federate << std::endl;
            try {
                socket = server->getSocketLink((*i)->Federate);
                // socket NULL means federate dead (killed ?)
                if (socket != NULL) {
                    msg->send(socket, NM_msgBufSend);
                }
            }
            catch (RTIinternalError& e) {
                Debug(D, pdExcept) << "Reference to a killed Federate while broadcasting." << std::endl;
            }
            catch (NetworkError& e) {
                Debug(D, pdExcept) << "Network error while broadcasting, ignoring." << std::endl;
            }

            // 2. Marquer le federe comme ayant recu le message.
            (*i)->state[0] = ObjectBroadcastLine::sent;
        }
        else {
            Debug(D, pdProtocol) << "No message sent to Federate " << (*i)->Federate << std::endl;
        }
    }
}

void ObjectClassBroadcastList::sendPendingRAVMessage(SecurityServer* server)
{
    Socket* socket = NULL;
    NetworkMessage* currentMessage = NULL;
    uint32_t attributeSize = 0;
    std::vector<AttributeHandle> vATH;

    if (NULL != msgRAV) {
        attributeSize = msgRAV->getAttributesSize();
        vATH = msgRAV->getAttributes();
    }

    if (NULL != msgRAOA) {
        attributeSize = msgRAOA->getAttributesSize();
        vATH = msgRAOA->getAttributes();
    }

    Debug(G, pdGendoc) << "enter ObjectClassBroadcastList::sendPendingRAVMessage" << std::endl;
    // For each line :
    list<ObjectBroadcastLine*>::iterator i;
    for (i = lines.begin(); i != lines.end(); ++i) {
        // Si AU MOINS UN des attributs est en ObjectBroadcastLine::waiting
        if (isWaiting(*i)) {
            // 1. Est-ce que tous les attributs du message sont en
            // ObjectBroadcastLine::waiting ?
            bool all_waiting = true;
            for (uint32_t attrIndex = 0; attrIndex < attributeSize; ++attrIndex) {
                if ((*i)->state[vATH[attrIndex]] != ObjectBroadcastLine::waiting)
                    all_waiting = false;
            }

            if (!all_waiting) {
                // NO: Create a new message containing only ObjectBroadcastLine::waiting
                // attributes.
                if (NULL != msgRAV) {
                    currentMessage = createReducedMessageWithValue(msgRAV, *i);
                }
                if (NULL != msgRAOA) {
                    currentMessage = createReducedMessage(msgRAOA, *i);
                }
                Debug(D, pdProtocol) << "Broadcasting reduced message to Federate " << (*i)->Federate << std::endl;
            }
            else {
                // YES: Nothing to do.
                currentMessage = msg;
                Debug(D, pdProtocol) << "Broadcasting complete message to Federate " << (*i)->Federate << std::endl;
            }

            // 2. Send message (or reduced one).
            try {
#ifdef HLA_USES_UDP
                socket = server->getSocketLink((*i)->Federate, BEST_EFFORT);
#else
                socket = server->getSocketLink((*i)->Federate);
#endif
                // socket NULL means federate is dead (killed ?)
                if (socket != NULL) {
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

            // 3. marquer les attributs en ObjectBroadcastLine::sent.
            for (unsigned int attrIndex = 1; attrIndex <= maxHandle; attrIndex++) {
                if ((*i)->state[attrIndex] == ObjectBroadcastLine::waiting) {
                    (*i)->state[attrIndex] = ObjectBroadcastLine::sent;
                }
            }

            // 4. Eventuellement effacer la copie du message.
            if (currentMessage != msg) {
                delete currentMessage;
                currentMessage = NULL;
            }

        } // Si AU MOINS UN des attributs est en ObjectBroadcastLine::waiting
        else {
            Debug(D, pdProtocol) << "No message sent to Federate " << (*i)->Federate << std::endl;
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
        if (line->state[currentAttrib] == ObjectBroadcastLine::waiting) {
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
        if (line->state[currentAttrib] == ObjectBroadcastLine::waiting) {
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
        if ((*i)->Federate == theFederate)
            return (*i);
    }

    return 0;
}

bool ObjectClassBroadcastList::isWaiting(ObjectBroadcastLine* line)
{
    for (unsigned int attrIndex = 1; attrIndex <= maxHandle; attrIndex++) {
        if (line->state[attrIndex] == ObjectBroadcastLine::waiting) {
            return true;
        }
    }

    return false;
}
} // namespace certi
