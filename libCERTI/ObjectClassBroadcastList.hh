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
// $Id: ObjectClassBroadcastList.hh,v 3.6 2004/05/18 13:18:55 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_CLASS_BROADCAST_LIST_HH
#define _CERTI_OBJECT_CLASS_BROADCAST_LIST_HH

#include "certi.hh"
#include "NetworkMessage.hh"
#include "SecurityServer.hh"
#include "ObjectBroadcastLine.hh"

#include <list>

namespace certi {

/*! This class is used to carry necessary information for broadcasting a
  RegisterObject() or an UpdateAttributeValues().
*/
class ObjectClassBroadcastList {

public:

    // -----------------------
    // -- Public Attributes --
    // -----------------------

    /*! First, the Message to be broadcast. This message must be allocated
      before calling the constructor of the class, be is deleted by the
      destructor.
    */
    NetworkMessage *message ;

    // --------------------
    // -- Public Methods --
    // --------------------
    ObjectClassBroadcastList(NetworkMessage *theMsg,
                             AttributeHandle MaxAttHandle = 0);
    ~ObjectClassBroadcastList();

    void clear();

    void addFederate(FederateHandle theFederate,
                     AttributeHandle theAttribute = 0);

    void sendPendingMessage(SecurityServer *server);

private:

    // ------------------------
    // -- Private Attributes --
    // ------------------------

    AttributeHandle maxHandle ;

    std::list<ObjectBroadcastLine *> lines ;

    // ---------------------
    // -- Private Methods --
    // ---------------------
    NetworkMessage *adaptMessage(ObjectBroadcastLine *line);

    //! Return the line of the list describing federate 'theFederate', or NULL.
    ObjectBroadcastLine *getLineWithFederate(FederateHandle theFederate);

    /*! Return RTI_TRUE is there is any Attribute in the bsWaiting state in
      the line 'line', else RTI_FALSE.
    */
    Boolean isWaiting(ObjectBroadcastLine *line);

    /*! The two next methods are called by the public SendPendingMessage
      methods. They respectively handle DiscoverObject and
      ReflectAttributeValues messages.
    */
    void sendPendingDOMessage(SecurityServer *server);
    void sendPendingRAVMessage(SecurityServer *server);
};
}

#endif // _CERTI_OBJECT_CLASS_BROADCAST_LIST_HH

// $Id: ObjectClassBroadcastList.hh,v 3.6 2004/05/18 13:18:55 breholee Exp $
