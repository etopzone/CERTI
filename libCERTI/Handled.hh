// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2004  Benoît Bréholée
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
// $Id: Handled.hh,v 1.1 2004/01/09 15:54:55 breholee Exp $
// ----------------------------------------------------------------------------

namespace certi {

template<typename T>
class Handled
{
public:
    Handled();
    Handled(T);

    setHandle(T);
    T getHandle() const ;

protected:
    T handle ;
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
template<typename T>
T
Handled<T>::getHandle() const
{
    return handle ;
}

} // certi

// $Id: Handled.hh,v 1.1 2004/01/09 15:54:55 breholee Exp $
