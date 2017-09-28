// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2004  ONERA
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

#ifndef LIBCERTI_HANDLED_HH
#define LIBCERTI_HANDLED_HH

#include "Handle.hh"

namespace certi {

/**
 * CERTI Handled class.
 * All classes for which there is an RTI::<handleType>
 * should inherit from Handled< RTI::<handleType>>.
 * @sa Dimension
 * @sa Attribute
 * @sa ObjectClass
 */
template <typename T>
class Handled {
public:
    /**
	 * The handle type from generic class template argument.
	 */
    typedef T handle_type;

    /** 
     * Default constructor. 
     * Handle value is initialized with zero
     */
    Handled();

    /**
     * Create an handled.
     * @param[in] hValue initial handle value
     */
    Handled(handle_type hValue);

    /**
     * Create an handled.
     * @param[in] hValue the handle value
     */
    void setHandle(handle_type hValue);

    /** 
     * Get handle
     * @return Handle value
     */
    handle_type getHandle() const;

protected:
    handle_type handle;
};

template <typename T>
Handled<T>::Handled() : handle(0)
{
}

template <typename T>
Handled<T>::Handled(T hValue) : handle(hValue)
{
}

template <typename T>
void Handled<T>::setHandle(T hValue)
{
    handle = hValue;
}

template <class T>
T Handled<T>::getHandle() const
{
    return handle;
}

} // certi

#endif // LIBCERTI_HANDLED_HH
