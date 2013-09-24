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

#include "certi.hh"
#include "NetworkMessage.hh"
#include "NM_Classes.hh"
#include "SecurityServer.hh"

#include <list>

#define MAX_STATE_SIZE 1024

namespace certi {

/**
 * An object broadcast line represents a federate
 * interested in part (or all) of the attributes of
 * the message referenced in the ObjectClassBroadcastList.
 */
class ObjectBroadcastLine {
public:
	/**
	 * The state of the attribute
	 * for the federate in this broadcast line
	 */
	enum State {
		sent,    /**< the attribute has been sent                       */
		waiting, /**< the attribute is waiting to be sent               */
		notSub   /**< the federate did not subscribed to this attribute */
	};

	ObjectBroadcastLine(FederateHandle fed, State init = notSub);

	/* The Federate Handle */
	FederateHandle Federate ;

	/**
	 * The state of the attributes.
	 * The index of the state array is the attribute handle
	 * FIXME we should rather use a map, because the
	 * FIXME attribute handle may be very sparse.
	 */
	State state[MAX_STATE_SIZE+1] ;

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
 * 	 <li> NM_Discover_Object </li>
 *   <li> NM_Reflect_Attribute_Values </li>
 *   <li> NM_Request_Attribute_Ownership_Assumption </li>
 *   <li> NM_Attribute_Ownership_Divestiture_Notification </li>
 * </ul>
 * A federate is represented by an ObjectBroadcastLine.
 */
class ObjectClassBroadcastList {

public:
	/**
	 * The provided msg must have been allocated, and will be destroyed
	 * by the destructor.
	 * msg->federate is added to the list, and its state is set as "Sent"
	 * for all attributes. For RAVs messages, MaxAttHandle is the greatest
	 * attribute handle of the class. For Discover_Object message, it can be 0 to
	 * mean "any attribute".
	 */
	ObjectClassBroadcastList(NetworkMessage *msg, AttributeHandle maxAttributeHandles = 0)
	throw (RTIinternalError);

	~ObjectClassBroadcastList();

	NetworkMessage* getMsg() {return msg;}
	NM_Remove_Object*                                getMsgRO()   {return msgRO;}
	NM_Discover_Object*                              getMsgDO()	  {return msgDO;}
	NM_Reflect_Attribute_Values*     		  	     getMsgRAV()  {return msgRAV;}
	NM_Request_Attribute_Ownership_Assumption*       getMsgRAOA() {return msgRAOA;}
	NM_Attribute_Ownership_Divestiture_Notification* getMsgAODN() {return msgAODN;}

	/**
	 * Clear the broadcast lines.
	 */
	void clear();

	/**
	 * Add a federate interested in the broadcast.
	 * @param[in] federate, the handle of the interested Federate
	 * @param[in] attribute,
	 */
	void addFederate(FederateHandle federate, AttributeHandle attribute = 0);

	/**
	 * Send all the pending message to all concerned
	 * Federate stored in the broadcast lines.
	 */
	void sendPendingMessage(SecurityServer *server);

	/**
	 * Upcast class to appropriate message.
	 * The inheritance feature of HLA imply that a federate subscribing
	 * to a superclass attribute which effectively belong to an instance
	 * of object of a derived (HLA meaning) class  should receive
	 * Federate callback (e.g. discover object) of the appropriate level
	 * i.e. the one he subscribed to.
	 */
	void upcastTo(ObjectClassHandle objectClass);

protected:
	NetworkMessage                                      *msg;
	NM_Remove_Object                                  *msgRO;
	NM_Discover_Object                                *msgDO;
	NM_Reflect_Attribute_Values                      *msgRAV;
	NM_Request_Attribute_Ownership_Assumption       *msgRAOA;
	NM_Attribute_Ownership_Divestiture_Notification *msgAODN;

private:

	template <typename T>
	T* createReducedMessage(T* msg, ObjectBroadcastLine *line);

	template <typename T>
	T* createReducedMessageWithValue(T* msg, ObjectBroadcastLine *line);


	//! Return the line of the list describing federate 'theFederate', or NULL.
	ObjectBroadcastLine *getLineWithFederate(FederateHandle theFederate);

	bool isWaiting(ObjectBroadcastLine *line);

	/*! The two next methods are called by the public SendPendingMessage
      methods. They respectively handle DiscoverObject and
      ReflectAttributeValues messages.
	 */
	void sendPendingDOMessage(SecurityServer *server);
	void sendPendingRAVMessage(SecurityServer *server);

	AttributeHandle maxHandle ;
	std::list<ObjectBroadcastLine *> lines ;
	/* The message buffer used to send Network messages */
	MessageBuffer NM_msgBufSend;
};

} // namespace certi

#endif // CERTI_OBJECT_CLASS_BROADCAST_LIST_HH
