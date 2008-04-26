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
// $Id: ObjectClassBroadcastList.cc,v 3.19 2008/04/26 14:59:41 erk Exp $
// ----------------------------------------------------------------------------



#include "ObjectClassBroadcastList.hh"
#include "PrettyDebug.hh"
#include "NM_Classes.hh"

using std::list ;

namespace certi {

static pdCDebug D("BROADCAST", __FILE__);
static PrettyDebug G("GENDOC",__FILE__);

// ----------------------------------------------------------------------------
/** ObjectBroadcastLine  
 */
ObjectBroadcastLine::ObjectBroadcastLine(FederateHandle theFederate,
                                         ObjectBroadcastLine::State init_state)
{
    Federate = theFederate ;
    for (AttributeHandle i = 0 ; i <= MAX_ATTRIBUTES_PER_CLASS ; i++)
        state[i] = init_state ;
}

// ============================================================================

// ----------------------------------------------------------------------------
/*! Return a copy of the REFLECT_ATTRIBUTE_VALUES message 'Message' containing
  references omly to the attributes marked as bsWaiting in the line 'line'.
  The returned message should be deleted later.
*/
NetworkMessage *
ObjectClassBroadcastList::adaptMessage(ObjectBroadcastLine *line)
{
    G.Out(pdGendoc,"enter ObjectClassBroadcastList::adaptMessage");
    G.Out(pdGendoc,"      message->objectClass=%d",message->objectClass);

    if ((message->getType() != NetworkMessage::REFLECT_ATTRIBUTE_VALUES) &&
        (message->getType() != NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION))
        throw RTIinternalError("Bad Message type in Broadcast's AdaptMsg.");

    // Copy static informations.
    NetworkMessage *reducedMessage = NM_Factory::create(message->getType());    
    reducedMessage->exception = message->exception ;
    reducedMessage->federation = message->federation ;
    reducedMessage->federate = message->federate ;
    reducedMessage->object = message->object ;
    reducedMessage->setDate(message->getDate());
    reducedMessage->boolean = message->boolean ; // FIXME Useful ?
    reducedMessage->objectClass = message->objectClass ;

    reducedMessage->setLabel(message->getLabel());

    // Copy attributes that are in the bsWaiting state in Line.
    UShort currentSize ;
    AttributeHandle currentAttrib ;
    char buffer[MAX_BYTES_PER_VALUE + 1] ;
    unsigned long length ;

    reducedMessage->handleArraySize = 0 ;

    for (UShort i = 0 ; i < message->handleArraySize ; i++) {

        currentAttrib = message->handleArray[i] ;

        if (line->state[currentAttrib] == ObjectBroadcastLine::waiting) {

            // Update number of attributes in ReducedMessage.
            currentSize = reducedMessage->handleArraySize ;
            reducedMessage->handleArraySize ++ ;

            // Copy Attribute Handle.
            reducedMessage->handleArray[currentSize] = currentAttrib ;

            if (message->getType() == NetworkMessage::REFLECT_ATTRIBUTE_VALUES) {
                // Copy Attribute Value.
                message->getValue(i, &length, buffer);
                reducedMessage->setValue(currentSize, buffer, length);

            }
        }
    }
    G.Out(pdGendoc,"      reducedMessage->objectClass=%d",reducedMessage->objectClass);
    G.Out(pdGendoc,"exit  ObjectClassBroadcastList::adaptMessage");
    return reducedMessage ;
}

// ----------------------------------------------------------------------------
/*! Add a federate to the list. If it was not present in the list, a
  new line is added and all attributes are marked as bsNotSubscriber.
  Then if the Federate has not been sent a message for this attribute,
  the attribute (for the federate) is marked has
  ObjectBroadcastLine::waiting. theAttribute can be not specified in
  the case of a DiscoverObject message.
*/
void
ObjectClassBroadcastList::addFederate(FederateHandle theFederate,
                                      AttributeHandle theAttribute)
{
    if (theAttribute > maxHandle) {
        D.Out(pdExcept, "Bad attribute handle: %u > %u.", theAttribute,
              maxHandle);
        throw RTIinternalError("");
    }

    ObjectBroadcastLine *line = getLineWithFederate(theFederate);

    if (line == 0) {
        line =
            new ObjectBroadcastLine(theFederate, ObjectBroadcastLine::notSub);
        lines.push_front(line);
        D.Out(pdRegister, "Adding new line in list for Federate %d.",
              theFederate);
    }

    if (line->state[theAttribute] != ObjectBroadcastLine::sent) {
        line->state[theAttribute] = ObjectBroadcastLine::waiting ;
        D.Out(pdRegister, "List attribute %d for Federate %d is now "
              "ObjectBroadcastLine::waiting.", theAttribute, theFederate);
    }
    else
        D.Out(pdTrace,
              "Message already sent to federate %d about attribute %d.",
              theFederate, theAttribute);
}

// ----------------------------------------------------------------------------
/*! theMsg must have been allocated, and will be destroyed by the destructor.
  theMsg->federate is added to the list, and its state is set as "Sent"
  for all attributes. For RAVs messages, MaxAttHandle is the greatest
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

    message = theMsg ;

    // Add reference of the sender.
    if (message->federate != 0) {
        firstLine = new ObjectBroadcastLine(message->federate,
                                            ObjectBroadcastLine::sent);
        lines.push_front(firstLine);
    }
}

// ----------------------------------------------------------------------------
//! Free all structures, including Message.
ObjectClassBroadcastList::~ObjectClassBroadcastList()
{
    this->clear();
}

// ----------------------------------------------------------------------------
//! Empty the list so it can reused (like the destructor).
void
ObjectClassBroadcastList::clear()
{
    delete message ;
    message = 0 ;

    maxHandle = 0 ;

    while (!lines.empty()) {
        delete lines.front();
        lines.pop_front();
    }

    D.Out(pdTerm, "List is now empty.");
}



ObjectBroadcastLine*
ObjectClassBroadcastList::getLineWithFederate(FederateHandle theFederate)
{
    list<ObjectBroadcastLine *>::iterator i ;
    for (i = lines.begin(); i != lines.end(); i++) {
        if ((*i)->Federate == theFederate)
            return (*i);
    }

    return 0 ;
}

// ----------------------------------------------------------------------------
/** Check if some attributes in the provided line have the "waiting"
    status.
 */
bool
ObjectClassBroadcastList::isWaiting(ObjectBroadcastLine *line)
{
    for (unsigned int attrIndex = 1 ; attrIndex <= maxHandle ; attrIndex++) {
        if (line->state[attrIndex] == ObjectBroadcastLine::waiting) {
            return true ;
        }
    }

    return false ;
}


// --------------------------
// -- SendPendingDOMessage --
// --------------------------

void
ObjectClassBroadcastList::sendPendingDOMessage(SecurityServer *server)
{
    Socket *socket = NULL ;

    // Pour chaque ligne de la liste
    list<ObjectBroadcastLine *>::iterator i ;
    for (i = lines.begin(); i != lines.end(); ++i) {
        // Si le federe attend un message(attribute 0 en attente)
        if ((*i)->state[0] == ObjectBroadcastLine::waiting) {

            // 1. Envoyer le message au federe
            D.Out(pdProtocol,
                  "Broadcasting message to Federate %d.", (*i)->Federate);
            try {
                socket = server->getSocketLink((*i)->Federate);
                message->send(socket);
            }
            catch (RTIinternalError &e) {
                D.Out(pdExcept,
                      "Reference to a killed Federate while broadcasting.");
            }
            catch (NetworkError &e) {
                D.Out(pdExcept, "Network error while broadcasting, ignoring.");
            }

            // 2. Marquer le federe comme ayant recu le message.
            (*i)->state[0] = ObjectBroadcastLine::sent ;
        }
        else
            D.Out(pdProtocol, "No message sent to Federate %d.",
                  (*i)->Federate);
    }
}

// ----------------------------------------------------------------------------
/*! IMPORTANT: Before calling this method, be sure to set the
  Message->federation handle.

  Broadcast the message to all the Federate in the
  ObjectBroadcastLine::waiting state. If it is a DiscoverObject
  message, the message is sent as is, and the Federate is marked as
  ObjectBroadcastLine::sent for the ANY attribute. If it is a RAV
  message, the message is first copied, without the Attribute list,
  and then all pending attributes(in the bsWainting state) are added
  to the copy. The copy is sent, and attributes are marked as
  ObjectBroadcastLine::sent.
*/
void ObjectClassBroadcastList::sendPendingMessage(SecurityServer *server)
{
    G.Out(pdGendoc,"enter ObjectClassBroadcastList::sendPendingMessage");
    switch (message->getType()) {

      case NetworkMessage::REFLECT_ATTRIBUTE_VALUES:
      case NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
        sendPendingRAVMessage(server);
        break ;

      case NetworkMessage::DISCOVER_OBJECT:
      case NetworkMessage::REMOVE_OBJECT:
        sendPendingDOMessage(server);
        break ;

      default:
        throw RTIinternalError("Unknown message type to broadcast.");
    G.Out(pdGendoc,"exit  ObjectClassBroadcastList::sendPendingMessage");
    }
}

// ---------------------------
// -- SendPendingRAVMessage --
// ---------------------------

void
ObjectClassBroadcastList::sendPendingRAVMessage(SecurityServer *server)
{
    Socket *socket = 0 ;
    NetworkMessage *currentMessage = 0 ;

    G.Out(pdGendoc,"enter ObjectClassBroadcastList::sendPendingRAVMessage");
    // For each line :
    list<ObjectBroadcastLine *>::iterator i ;
    for (i = lines.begin(); i != lines.end(); i++) {

        // Si AU MOINS UN des attributs est en ObjectBroadcastLine::waiting
        if (isWaiting(*i)) {

            // 1. Est-ce que tous les attributs du message sont en
            // ObjectBroadcastLine::waiting ?
            bool all_waiting = true ;
            for (unsigned int attrIndex = 0 ;
                 attrIndex < message->handleArraySize ;
                 attrIndex ++) {
                AttributeHandle attrib = message->handleArray[attrIndex] ;
                if ((*i)->state[attrib] != ObjectBroadcastLine::waiting)
                    all_waiting = false ;
            }

            if (!all_waiting) {
                // NO: Create a new message containing only ObjectBroadcastLine::waiting
                // attributes.
                currentMessage = adaptMessage(*i);
                D.Out(pdProtocol,
                      "Broadcasting reduced message to Federate %d.",
                      (*i)->Federate);
            }
            else {
                // YES: Nothing to do.
                currentMessage = message ;
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
                G.Out(pdGendoc,"                                 sendPendingRAVMessage=====> write");
                currentMessage->send(socket);
            }
            catch (RTIinternalError &e) {
                D.Out(pdExcept,
                      "Reference to a killed Federate while broadcasting.");
            }
            catch (NetworkError &e) {
                D.Out(pdExcept, "Network error while broadcasting, ignoring.");
            }

            // 3. marquer les attributs en ObjectBroadcastLine::sent.
            for (unsigned int attrIndex = 1 ;
                 attrIndex <= maxHandle ;
                 attrIndex ++) {
                if ((*i)->state[attrIndex] == ObjectBroadcastLine::waiting) {
                    (*i)->state[attrIndex] = ObjectBroadcastLine::sent ;
                }
            }

            // 4. Eventuellement effacer la copie du message.
            if (currentMessage != message) {
                delete currentMessage ;
                currentMessage = NULL ;
            }

        } // Si AU MOINS UN des attributs est en ObjectBroadcastLine::waiting
        else
            D.Out(pdProtocol, "No message sent to Federate %d.",
                  (*i)->Federate);
    G.Out(pdGendoc,"exit  ObjectClassBroadcastList::sendPendingRAVMessage");
    }
}

} // namespace certi

// $Id: ObjectClassBroadcastList.cc,v 3.19 2008/04/26 14:59:41 erk Exp $
