// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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

#ifndef CERTI_OBJECT_CLASS_BROADCAST_LIST_HH
#define CERTI_OBJECT_CLASS_BROADCAST_LIST_HH

#include "MessageEvent.hh"
#include "NM_Classes.hh"
#include "NetworkMessage.hh"
#include "SecurityServer.hh"
#include <include/certi.hh>

#include <vector>

#include <unordered_map>

namespace certi {

/** An object broadcast line represents a federate
 * interested in part (or all) of the attributes of
 * the message referenced in the ObjectClassBroadcastList.
 */
class ObjectBroadcastLine {
public:
    /// The state of the attribute for the federate in this broadcast line
    enum class State {
        Sent = 0, /// the attribute has been sent
        Waiting, /// the attribute is waiting to be sent
        NotSub /// the federate did not subscribed to this attribute
    };

    ObjectBroadcastLine(const FederateHandle federate, const State initial_state = State::NotSub);

    FederateHandle getFederate() const;

    State stateFor(const AttributeHandle attribute) const;

    void setState(const AttributeHandle attribute, const State value);

    bool isWaitingAny(const AttributeHandle max_handle) const;

    bool isWaitingAll(const std::vector<AttributeHandle>& attributes) const;

private:
    /// The Federate Handle
    FederateHandle my_federate;

    State my_initial_state;

    std::unordered_map<AttributeHandle, State> my_states;
};

/**
 * An ObjectClassBroadcastList consist in a message associated
 * with a list of federate which could be interested by part
 * of this message because it has subscribed to part or
 * all of the object instance/class attribute.
 * Thus an instance of this class carries the necessary information for
 * broadcasting the following type of messages:
 * <ul>
 *   <li> NM_Remove_Object </li>
 *       <li> NM_Discover_Object </li>
 *   <li> NM_Reflect_Attribute_Values </li>
 *   <li> NM_Request_Attribute_Ownership_Assumption </li>
 *   <li> NM_Attribute_Ownership_Divestiture_Notification </li>
 * </ul>
 * A federate is represented by an ObjectBroadcastLine.
 */
class ObjectClassBroadcastList {
public:
    /** Message->federate is added to the list, and its state is set as "Sent"
     * for all attributes. For RAVs messages, MaxAttHandle is the greatest
     * attribute handle of the class. For Discover_Object message, it can be 0 to
     * mean "any attribute".
     */
    ObjectClassBroadcastList(std::unique_ptr<NetworkMessage>(message), AttributeHandle maxAttributeHandles = 0);

    ~ObjectClassBroadcastList() = default;

    NetworkMessage& getMsg();
    NM_Remove_Object* getMsgRO();
    NM_Discover_Object* getMsgDO();
    NM_Reflect_Attribute_Values* getMsgRAV();
    NM_Request_Attribute_Ownership_Assumption* getMsgRAOA();
    NM_Attribute_Ownership_Divestiture_Notification* getMsgAODN();

    /** Add a federate interested in the broadcast.
     * 
     * If it was not present in the list,
     * a new line is added and all attributes are marked as bsNotSubscriber.
     * Then if the Federate has not been sent a message for this attribute,
     * the attribute (for the federate) is marked has
     * ObjectBroadcastLine::waiting. theAttribute can be not specified in
     * the case of a DiscoverObject message.
     * 
     * @param[in] federate the handle of the interested Federate
     * @param[in] attribute the addribute inderested in
     */
    void addFederate(FederateHandle federate, AttributeHandle attribute = 0);

    /** Prepare all the pending message to all concerned Federate stored in the broadcast lines.
     * 
     * IMPORTANT: Before calling this method, be sure to set the
     * Message->federation handle.
     * 
     * Broadcast the message to all the Federate in the
     * ObjectBroadcastLine::waiting state. If it is a DiscoverObject
     * message, the message is sent as is, and the Federate is marked as
     * ObjectBroadcastLine::sent for the ANY attribute. If it is a RAV
     * message, the message is first copied, without the Attribute list,
     * and then all pending attributes(in the bsWainting state) are added
     * to the copy. The copy is sent, and attributes are marked as
     * ObjectBroadcastLine::sent.
     */
    Responses preparePendingMessage(SecurityServer& server);

    /**
     * Upcast class to appropriate message.
     * The inheritance feature of HLA imply that a federate subscribing
     * to a superclass attribute which effectively belong to an instance
     * of object of a derived (HLA meaning) class  should receive
     * Federate callback (e.g. discover object) of the appropriate level
     * i.e. the one he subscribed to.
     */
    void upcastTo(ObjectClassHandle objectClass);

    const std::vector<ObjectBroadcastLine>& ___TESTS_ONLY___lines() const
    {
        return my_lines;
    }

protected:
    std::unique_ptr<NetworkMessage> my_message;
    NM_Remove_Object* msgRO{nullptr};
    NM_Discover_Object* msgDO{nullptr};
    NM_Reflect_Attribute_Values* msgRAV{nullptr};
    NM_Request_Attribute_Ownership_Assumption* msgRAOA{nullptr};
    NM_Attribute_Ownership_Divestiture_Notification* msgAODN{nullptr};

private:
    Responses preparePendingDOMessage(SecurityServer& server);
    Responses preparePendingRAVMessage(SecurityServer& server);

    template <typename T>
    std::unique_ptr<NetworkMessage> createResponseMessage(T* message, const ObjectBroadcastLine& line);

    template <typename T>
    std::unique_ptr<NetworkMessage> createResponseMessageWithValues(T* message, const ObjectBroadcastLine& line);

    template <typename T>
    std::unique_ptr<NetworkMessage> createResponseMessage(T* message);

    /// Check if some attributes in the provided line have the "waiting" status.
    AttributeHandle maxHandle;
    std::vector<ObjectBroadcastLine> my_lines;
};

} // namespace certi

#endif // CERTI_OBJECT_CLASS_BROADCAST_LIST_HH
