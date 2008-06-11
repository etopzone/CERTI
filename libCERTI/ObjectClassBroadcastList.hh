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
// $Id: ObjectClassBroadcastList.hh,v 3.10 2008/06/11 15:19:22 rousse Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_OBJECT_CLASS_BROADCAST_LIST_HH
#define CERTI_OBJECT_CLASS_BROADCAST_LIST_HH

#include "certi.hh"
#include "NetworkMessage.hh"
#include "SecurityServer.hh"

#include <list>

#define MAX_STATE_SIZE 1024

namespace certi {

class ObjectBroadcastLine {
public:
    enum State { sent, waiting, notSub };

    ObjectBroadcastLine(FederateHandle fed, State init = notSub);

    FederateHandle Federate ;

    State state[MAX_STATE_SIZE+1] ;

};

/*! This class is used to carry necessary information for broadcasting a
  RegisterObject() or an UpdateAttributeValues().
*/
class ObjectClassBroadcastList {

public:
    ObjectClassBroadcastList(NetworkMessage *, AttributeHandle = 0);
    ~ObjectClassBroadcastList();

    void clear();
    void addFederate(FederateHandle, AttributeHandle = 0);
    void sendPendingMessage(SecurityServer *server);

    NetworkMessage *message ;

private:
    NetworkMessage *adaptMessage(ObjectBroadcastLine *line);

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

// $Id: ObjectClassBroadcastList.hh,v 3.10 2008/06/11 15:19:22 rousse Exp $
