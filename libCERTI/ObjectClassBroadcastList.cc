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
// $Id: ObjectClassBroadcastList.cc,v 3.4 2003/01/15 14:31:43 breholee Exp $
// ---------------------------------------------------------------------------

#include "ObjectClassBroadcastList.hh"

namespace certi {

static pdCDebug D("OBJBROADCASTLIST", "(broadcas) - ");

// ---------------------------------------------------------------------------
/*! Return a copy of the REFLECT_ATTRIBUTE_VALUES message 'Message' containing
  references omly to the attributes marked as bsWaiting in the line 'line'.
  The returned message should be deleted later.
*/
NetworkMessage *
ObjectClassBroadcastList::adaptMessage(ObjectBroadcastLine *line)
{
    if ((message->Type != m_REFLECT_ATTRIBUTE_VALUES) &&
        (message->Type != m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION))
        throw RTIinternalError("Bad Message type in Broadcast's AdaptMsg.");

    // Copy static informations.
    NetworkMessage *reducedMessage = new NetworkMessage;
    reducedMessage->Type = message->Type;
    reducedMessage->Exception = message->Exception;
    reducedMessage->NumeroFederation = message->NumeroFederation;
    reducedMessage->NumeroFedere = message->NumeroFedere;
    reducedMessage->Objectid = message->Objectid;
    reducedMessage->Date = message->Date;

    strcpy(reducedMessage->Label, message->Label);

    // Copy attributes that are in the bsWaiting state in Line.
    UShort currentSize;
    AttributeHandle currentAttrib;
    char buffer[MAX_BYTES_PER_VALUE + 1] ;

    reducedMessage->HandleArraySize  = 0;

    for (UShort i = 0; i < message->HandleArraySize; i++) {

        currentAttrib = message->HandleArray[i];

        if (line->State[currentAttrib] == bsWaiting) {

            // Update number of attributes in ReducedMessage.
            currentSize =  reducedMessage->HandleArraySize;
            reducedMessage->HandleArraySize ++;

            // Copy Attribute Handle.
            reducedMessage->HandleArray[currentSize] = currentAttrib;

            if (message->Type == m_REFLECT_ATTRIBUTE_VALUES) {
                // Copy Attribute Value.
                message->getValue(i, buffer);
                reducedMessage->setValue(currentSize, buffer);
            }
        }
    }

    return reducedMessage;
}

// ---------------------------------------------------------------------------
/*! Add a federate to the list. If it was not present in the list, a new line
  is added and all attributes are marked as bsNotSubscriber.  Then if the
  Federate has not been sent a message for this attribute, the attribute
  (for the federate) is marked has bsWaiting.  theAttribute can be not
  specified in the case of a DiscoverObject message.
*/
void
ObjectClassBroadcastList::addFederate(FederateHandle theFederate,
                                      AttributeHandle theAttribute)
{
    if (theAttribute > maxHandle) {
        D.Out(pdExcept, "Bad attribute handle: %u > %u.", theAttribute, 
              maxHandle);
        throw RTIinternalError();
    }

    ObjectBroadcastLine *line = getLineWithFederate(theFederate);

    if (line == 0) {
        line = new ObjectBroadcastLine(theFederate, bsNotSub);
        lines.push_front(line);
        D.Out(pdRegister, "Adding new line in list for Federate %d.", 
              theFederate);
    }
  
    if (line->State[theAttribute] != bsSent) {
        line->State[theAttribute] = bsWaiting;
        D.Out(pdRegister, "List attribute %d for Federate %d is now bsWaiting.",
              theAttribute, theFederate);
    }
    else
        D.Out(pdTrace, 
              "Message already sent to federate %d about attribute %d.",
              theFederate, theAttribute);
}


// -----------------------------
// --  ObjectBroadcastLine --
// -----------------------------
ObjectBroadcastLine::ObjectBroadcastLine(FederateHandle theFederate,
                                         BroadcastState theInitialState)
{
    Federate = theFederate;

    for (AttributeHandle i = 0; i <= MAX_ATTRIBUTES_PER_CLASS; i++)
        State[i] = theInitialState;
}

// ---------------------------------------------------------------------------
/*! theMsg must have been allocated, and will be destroyed by the destructor.
  theMsg->NumeroFedere is added to the list, and its state is set as "Sent"
  for all attributes.  For RAVs messages, MaxAttHandle is the greatest
  attribute handle of the class. For Discover_Object message, it can be 0 to
  mean "any attribute".
*/
ObjectClassBroadcastList::ObjectClassBroadcastList(NetworkMessage *theMsg,
                                                   AttributeHandle maxAttHandle)
    : maxHandle(maxAttHandle)
{
    ObjectBroadcastLine *firstLine = 0 ;

    if (theMsg == 0)
        throw RTIinternalError("Null Broadcast Message.");

    message = theMsg;

    // Add reference of the sender.
    if (message->NumeroFedere != 0) {
        firstLine = new ObjectBroadcastLine(message->NumeroFedere, bsSent);
        lines.push_front(firstLine);
    }
}

// ---------------------------------------------------------------------------
//! Free all structures, including Message.
ObjectClassBroadcastList::~ObjectClassBroadcastList(void)
{
    this->clear();
}

// ---------------------------------------------------------------------------
//! Empty the list so it can reused (like the destructor).
void
ObjectClassBroadcastList::clear(void)
{
    delete message;
    message = 0 ;

    maxHandle = 0;

    while (!lines.empty()) {
        delete lines.front();
        lines.pop_front();
    }

    D.Out(pdTerm, "List is now empty.");
}


// -------------------------
// -- GetLineWithFederate --
// -------------------------

ObjectBroadcastLine*
ObjectClassBroadcastList::getLineWithFederate(FederateHandle theFederate)
{
    list<ObjectBroadcastLine *>::iterator i ;
    for (i = lines.begin(); i != lines.end() ; i++) {
        if ((*i)->Federate == theFederate)
            return (*i);
    }

    return 0 ;
}

// ---------------
// -- IsWaiting --
// ---------------

Boolean
ObjectClassBroadcastList::isWaiting(ObjectBroadcastLine *line)
{
    for (unsigned int attrIndex = 1; attrIndex <= maxHandle; attrIndex++) {
        if (line->State [attrIndex] == bsWaiting) {
            return RTI_TRUE;
        }
    }

    return RTI_FALSE;
}


// --------------------------
// -- SendPendingDOMessage --
// --------------------------

void
ObjectClassBroadcastList::sendPendingDOMessage(SecurityServer *server)
{
    Socket *socket = NULL;

    // Pour chaque ligne de la liste
    list<ObjectBroadcastLine *>::iterator i ;
    for (i = lines.begin(); i != lines.end() ; i++) {
        // Si le federe attend un message(attribute 0 en attente)
        if ((*i)->State[0] == bsWaiting) {

            // 1. Envoyer le message au federe
            D.Out(pdProtocol, 
                  "Broadcasting message to Federate %d.", (*i)->Federate);
            try {
                socket = server->getSocketLink((*i)->Federate);
                message->write(socket);
            }
            catch(RTIinternalError &e) {
                D.Out(pdExcept, 
                      "Reference to a killed Federate while broadcasting.");
            }
            catch(NetworkError &e) {
                D.Out(pdExcept, "Network error while broadcasting, ignoring.");
            }

            // 2. Marquer le federe comme ayant recu le message.
            (*i)->State[0] = bsSent;
        }
        else
            D.Out(pdProtocol, "No message sent to Federate %d.", 
                  (*i)->Federate);
    }
}

// ---------------------------------------------------------------------------
/*! IMPORTANT: Before calling this method, be sure to set the
  Message->NumeroFederation handle.

  Broadcast the message to all the Federate in the bsWaiting state. If it is
  a DiscoverObject message, the message is sent as is, and the Federate is
  marked as bsSent for the ANY attribute.  If it is a RAV message, the
  message is first copied, without the Attribute list, and then all pending
  attributes(in the bsWainting state) are added to the copy. The copy is
  sent, and attributes are marked as bsSent.
*/
void ObjectClassBroadcastList::sendPendingMessage(SecurityServer *server)
{
    switch (message->Type) {

    case m_REFLECT_ATTRIBUTE_VALUES:
    case m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
        sendPendingRAVMessage(server);
        break;

    case m_DISCOVER_OBJECT:
    case m_REMOVE_OBJECT:
        sendPendingDOMessage(server);
        break;
    
    default:
        throw RTIinternalError("Unknown message type to broadcast.");
    }
}

// ---------------------------
// -- SendPendingRAVMessage --
// ---------------------------

void
ObjectClassBroadcastList::sendPendingRAVMessage(SecurityServer *server)
{
    Boolean allWaiting;
    AttributeHandle currentAttrib;

    Socket *socket = 0 ;
    NetworkMessage *currentMessage = 0 ;
  
    // For each line :
    list<ObjectBroadcastLine *>::iterator i ;
    for (i = lines.begin(); i != lines.end() ; i++) {

        // Si AU MOINS UN des attributs est en bsWaiting
        if (isWaiting(*i) == RTI_TRUE) {
      
            // 1. Est-ce que tous les attributs du message sont en bsWaiting ?
            allWaiting = RTI_TRUE;
            for (unsigned int attrIndex = 0;
                attrIndex < message->HandleArraySize;
                attrIndex ++) {
                currentAttrib = message->HandleArray[attrIndex];
                if ((*i)->State [currentAttrib] != bsWaiting)
                    allWaiting = RTI_FALSE;
            }
      
            if (allWaiting == RTI_FALSE) {
                // NO: Create a new message containing only bsWaiting
                // attributes.
                currentMessage = adaptMessage(*i);
                D.Out(pdProtocol, 
                      "Broadcasting reduced message to Federate %d.",
                      (*i)->Federate);
            }
            else {
                // YES: Nothing to do.
                currentMessage = message;
                D.Out(pdProtocol, 
                      "Broadcasting complete message to Federate %d.",
                      (*i)->Federate);
            }

            // 2. Send message (or reduced one).
            try {
#ifdef HLA_USES_UDP
                socket = server->getSocketLink((*i)->Federate, BEST_EFFORT);
#else
                socket = server->getSocketLink((*i)->Federate);
#endif
                currentMessage->write(socket);
            }
            catch (RTIinternalError &e) {
                D.Out(pdExcept, 
                      "Reference to a killed Federate while broadcasting.");
            }
            catch(NetworkError &e) {
                D.Out(pdExcept, "Network error while broadcasting, ignoring.");
            }
      
            // 3. marquer les attributs en bsSent.
            for (unsigned int attrIndex = 1; 
                 attrIndex <= maxHandle; 
                 attrIndex ++) {
                if((*i)->State [attrIndex] == bsWaiting) {
                    (*i)->State [attrIndex] = bsSent;
                }
            }
      
            // 4. Eventuellement effacer la copie du message.
            if (currentMessage != message) {
                delete currentMessage;
                currentMessage = NULL;
            }
      
        } // Si AU MOINS UN des attributs est en bsWaiting
        else
            D.Out(pdProtocol, "No message sent to Federate %d.", 
                  (*i)->Federate);
    } // Pour chaque ligne...
}

}

// $Id: ObjectClassBroadcastList.cc,v 3.4 2003/01/15 14:31:43 breholee Exp $
