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
// $Id: Handled.hh,v 3.1 2006/03/09 19:54:28 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef LIBCERTI_HANDLED_HH
#define LIBCERTI_HANDLED_HH

namespace certi {

template<typename T>
class Handled
{
public:
    typedef T handle_type ;

    Handled();
    Handled(handle_type);

    void setHandle(handle_type);
    handle_type getHandle() const ;

protected:
    handle_type handle ;
};

/** Default constructor. Handle is initialized with zero
 */
template<typename T>
Handled<T>::Handled()
    : handle(0) { }

/** Constructor with initial value.
    @param h Initial handle value
*/
template<typename T>
Handled<T>::Handled(T h)
    : handle(h) { }

/** Set handle
    @param h New handle value
*/
template<typename T>
void
Handled<T>::setHandle(T h)
{
    handle = h ;
}

/** Get handle
    @return Handle value
*/
template<class T>
T
Handled<T>::getHandle() const
{
    return handle ;
}

} // certi

#endif // LIBCERTI_HANDLED_HH

// $Id: Handled.hh,v 3.1 2006/03/09 19:54:28 breholee Exp $
