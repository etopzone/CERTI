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
// $Id: Subscribable.hh,v 3.1 2005/04/02 15:41:04 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_SUBSCRIBABLE_HH
#define CERTI_SUBSCRIBABLE_HH

#include "Subscriber.hh"

#include <list>

namespace certi {

class Subscribable
{
public:
    virtual ~Subscribable();

    virtual void checkFederateAccess(FederateHandle, const char *) const = 0 ;
    virtual Handle getHandle() const = 0 ;

    bool isSubscribed(FederateHandle, const RTIRegion *) const ;
    bool isSubscribed(FederateHandle) const ;

    void subscribe(FederateHandle, const RTIRegion *);

    void unsubscribe(FederateHandle);
    void unsubscribe(FederateHandle, const RTIRegion *);

    template <class T> void addFederatesIfOverlap(T &, const RTIRegion *, Handle) const ;

private:
    std::list<Subscriber> subscribers ;
};

// ----------------------------------------------------------------------------
/** Add federates and associated subscribable object handle to a broadcast list.
    @param lst Broadcast list where federates/handles should be added
    @param region Region to check for overlap
    @param handle Handle of this object (Subscribable subclass)
 */
template <class T> void
Subscribable::addFederatesIfOverlap(T &lst, const RTIRegion *region, Handle handle) const
{
    std::list<Subscriber>::const_iterator it = subscribers.begin();
    for (; it != subscribers.end(); ++it) {
	if (it->match(region))
	    lst.addFederate(it->getHandle(), handle);
    }
}

#endif // CERTI_SUBSCRIBABLE_HH

} // namespace certi

// $Id: Subscribable.hh,v 3.1 2005/04/02 15:41:04 breholee Exp $
