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
// $Id: NameComparator.hh,v 1.2 2003/11/13 10:37:36 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef LIBCERTI_NAME_COMPARATOR
#define LIBCERTI_NAME_COMPARATOR

#include <string>

namespace certi {

/** NameComparator is a comparison functor for objects having a getName()
    method. 
 */
template<class T>
class NameComparator
{
public:
    NameComparator(std::string);
    bool operator()(const T *) const ;
    bool operator()(const T &) const ;
private:
    std::string name ;
};

template<class T>
NameComparator<T>::NameComparator(std::string h)
    : name(h) { }

template <class T>
bool 
NameComparator<T>::operator()(const T *op) const
{
    return name == op->getName();
}

template <class T>
bool 
NameComparator<T>::operator()(const T &op) const
{
    return name == op.getName() ;
}

}

#endif // LIBCERTI_NAME_COMPARATOR
