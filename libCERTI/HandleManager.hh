// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  Benoît Bréholée
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
// $Id: HandleManager.hh,v 1.2.2.2 2004/07/02 12:21:43 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef LIBCERTI_HANDLE_MANAGER
#define LIBCERTI_HANDLE_MANAGER

#include "RTItypes.hh"
#include <cassert>
#include <limits>

namespace certi {

/** Manage a set of handles ('provide' and 'free' services). Freed
    handles may be provided again in a future request.
*/
template<class T>
class HandleManager
{
public:
    HandleManager();
    T provide() throw (RTIinternalError);
    void free(T);

private:
    T hmax ; ///< highest handle value
    T current ; ///< next never provided handle
    std::list<T> available ; ///< available handles less than 'current'
};

// ----------------------------------------------------------------------------
/** Constructor. Min value is 1 and Max value is same as max type value.
 */
template<class T>
HandleManager<T>::HandleManager()
    : hmax(std::numeric_limits<T>::max()), current(1) { }

// ----------------------------------------------------------------------------
/** Get a new handle
    @return handle
    @throw RTIinternalError if all handles between first and maximal are used
 */
template<class T> T
HandleManager<T>::provide()
    throw (RTIinternalError)
{
    T handle = 0 ;

    if (available.size() > 0) {
	handle = available.front();
	available.pop_front();
    }
    else {
	if (current >= hmax)
	    throw RTIinternalError("Maximum handle reached");
	else
	    handle = current++ ;
    }

    assert(handle != 0);
    return handle ;
}

// ----------------------------------------------------------------------------
/** Free a handle
    @pre handle is a previously-provided handle
    @param handle Handle to free
 */
template<class T> void
HandleManager<T>::free(T handle)
{
    available.push_back(handle);
}

} // certi

#endif // LIBCERTI_HANDLE_MANAGER
