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
// $Id: ObjectClassAttribute.hh,v 3.9 2003/07/09 16:05:22 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_CLASS_ATTRIBUTE_HH
#define _CERTI_OBJECT_CLASS_ATTRIBUTE_HH

#include "RTItypes.hh"
#include "SecurityLevel.hh"

#include "Subscriber.hh"
#include "Publisher.hh"
#include "ObjectClassBroadcastList.hh"


namespace certi {

/*! This class descrives an object class attribute (handle, level id, ordering,
  transportation mode and name). This class also keeps track of published and
  subscribed federates.
*/
class ObjectClassAttribute {

public:
    // -----------------------
    // -- Public Attributes --
    // -----------------------
    SecurityLevelID LevelID ;

    OrderType Order ;
    TransportType Transport ;

    // This Object help to find a TCPLink from a Federate Handle.
    SecurityServer *server ;

    // --------------------
    // -- Public Methods --
    // --------------------
    ObjectClassAttribute();
    ObjectClassAttribute(ObjectClassAttribute *source);
    ~ObjectClassAttribute();

    void display() const ;

    /*! Name attribute access(GetName reference must be considered READ-ONLY).
      NewName lenght must be lower or equal to MAX_USER_TAG_LENGTH.
    */
    char *getName() const {return Name ; };

    void setName(char *NewName)
        throw (ValueLengthExceeded, RTIinternalError);

    void setHandle(AttributeHandle h);
    AttributeHandle getHandle() const ;

    void setSpace(SpaceHandle);
    SpaceHandle getSpace() const ;

    // Security methods
    void checkFederateAccess(FederateHandle the_federate, const char *reason)
        throw (SecurityError);

    // Publish & subscribe methods
    Boolean isPublishing(FederateHandle the_handle) const ;
    Boolean hasSubscribed(FederateHandle the_handle) const ;

    void publish(FederateHandle theFederate, bool PubOrUnpub)
        throw (RTIinternalError, SecurityError);

    void subscribe(FederateHandle theFederate, bool SubOrUnsub)
        throw (RTIinternalError, SecurityError);

    void subscribe(FederateHandle, RegionImp *);
    void unsubscribe(FederateHandle, RegionImp *);
    
    // Update attribute values
    void updateBroadcastList(ObjectClassBroadcastList *ocb_list);

private:
    // The four next methods do the memory management stuff to
    // add and delete publishers and subscribers.
    void addPublisher(FederateHandle theFederate)
        throw (RTIinternalError);

    void addSubscriber(FederateHandle theFederate)
        throw (RTIinternalError);

    void deletePublisher(int PublisherRank);
    void deleteSubscriber(int SubscriberRank);

    // Both following methods return the Rank of the Federate in the list,
    // or ZERO if not found.
    int getPublisherRank(FederateHandle theFederate) const ;
    int getSubscriberRank(FederateHandle theFederate) const ;

    AttributeHandle handle ; //!< The attribute handle.
    AttributeName Name ; //!< The attribute name, must be locally allocated.
    SpaceHandle space ; //!< Routing space

    list<Subscriber *> subscribers ; //!< The subscriber's list.
    list<Publisher *> publishers ; //!< The publisher's list.
};
}

#endif // _CERTI_OBJECT_CLASS_ATTRIBUTE_HH

// $Id: ObjectClassAttribute.hh,v 3.9 2003/07/09 16:05:22 breholee Exp $
