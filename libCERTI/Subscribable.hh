// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2005  ONERA
//
// This program is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: Subscribable.hh,v 3.10 2011/09/02 21:42:23 erk Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_SUBSCRIBABLE_HH
#define CERTI_SUBSCRIBABLE_HH

namespace certi {
	class ObjectClassBroadcastList;
	class InteractionBroadcastList;
	class RTIRegion ;
}

#include "certi.hh"
#include "Named.hh"
#include <list>


namespace certi {

class Subscriber
{
public:
    Subscriber(FederateHandle);
    Subscriber(FederateHandle, const RTIRegion *);

    FederateHandle getHandle() const ;
    const RTIRegion *getRegion() const ;
    bool equals(FederateHandle, const RTIRegion *) const ;
    bool match(const RTIRegion *) const ;

    bool operator==(const Subscriber &) const ;

protected:
    FederateHandle handle ;   //!< The ID of the Subscriber.
    const RTIRegion *region ; //!< the subscription region
};

/**
 * This class is a base class for "subscribable" classes.
 * That is ObjectClassAttribute and Interaction.
 */
class CERTI_EXPORT Subscribable : public Named
{
public:
    Subscribable(const std::string& name);
    virtual ~Subscribable();

    virtual void checkFederateAccess(FederateHandle, const std::string&) const = 0 ;
    virtual Handle getHandle() const = 0 ;

    bool isSubscribed(FederateHandle, const RTIRegion *) const ;
    bool isSubscribed(FederateHandle) const ;

    void subscribe(FederateHandle, const RTIRegion *);

    void unsubscribe(FederateHandle);
    void unsubscribe(FederateHandle, const RTIRegion *);

    void addFederatesIfOverlap(ObjectClassBroadcastList &, const RTIRegion *, Handle) const ;
    void addFederatesIfOverlap(InteractionBroadcastList &, const RTIRegion *) const ;

private:
    std::list<Subscriber> subscribers ;
};

} // namespace certi

#endif // CERTI_SUBSCRIBABLE_HH

// $Id: Subscribable.hh,v 3.10 2011/09/02 21:42:23 erk Exp $
