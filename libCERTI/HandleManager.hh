// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// ----------------------------------------------------------------------------

#ifndef LIBCERTI_HANDLE_MANAGER
#define LIBCERTI_HANDLE_MANAGER

#include "certi.hh"

#include <limits>

namespace certi {

/**
 * Manage a set of handles ('provide' and 'free' services). Freed
 * handles may be provided again in a future request.
 */
template<typename T>
class HandleManager
{
public:
	/**
	 * Constructor with init value.
	 * The default maximal value will be based on the type max given
	 * by numeric_limits traits.
	 * @param[in] init Initial handle value (use for first requested handle)
	 */
	HandleManager(T);
	/**
	 * Constructor with init value and max.
	 * @param[in] init Initial handle value (use for first requested handle)
	 * @param[in] hmax Maximal handle value
	 */
	HandleManager(T init, size_t hmax);
	/**
	 * Provide a new unique federation execution-wide handle.
	 * IEEE-1516.1-2000 - 10.1.1 Names says:
	 *   "Each name in an FDD (object classes, interactions, attributes,
	 *    parameters, dimensions, transportation types, order types)
	 *    shall have a unique, and unpredictable, federation execution-wide handle."
	 *
	 * @return the new handle
	 * @throw RTIinternalError if all handles between first and maximal are used
	 */
	T provide() throw (RTIinternalError);

	/**
	 * Free a handle.
	 * @pre handle is a previously-provided handle
	 * @param handle Handle to free
	 * @todo CHECK how this could be handled, currently free does nothing!
	 */
	void free(T handle);

private:
	size_t maximum ;
	T highest ;
	std::list<T> available ;
};


template<typename T>
HandleManager<T>::HandleManager(T init)
: maximum(std::numeric_limits<T>::max()), highest(init) { }

template<typename T>
HandleManager<T>::HandleManager(T init, size_t hmax)
: highest(init), maximum(hmax) { }

template<typename T> T
HandleManager<T>::provide() throw (RTIinternalError)
{
	T handle = 0 ;

	if (available.size() > 0) {
		handle = available.front();
		available.pop_front();
	}
	else {
		if (highest < maximum)
			handle = highest++ ;
		else
			throw RTIinternalError("Maximum handle reached");
	}

	return handle ;
} /* end of provide */

template<typename T> void
HandleManager<T>::free(T handle)
{
	//if (handle + 1 == highest)
	//--highest ;
	//else
	//available.push_back(handle);
}

} // certi

#endif // LIBCERTI_HANDLE_MANAGER
