// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2004, 2006  ONERA
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

#ifndef LIBCERTI_HELPER_HH
#define LIBCERTI_HELPER_HH

#include <string>

namespace certi {

/** HandleComparator is a comparison functor for objects having a getHandle()
    method. 
 */
template <class T>
class HandleComparator {
public:
    HandleComparator(Handle h) : handle(h){};
    bool operator()(const T* op) const
    {
        return op->getHandle() == handle;
    };
    bool operator()(const T& op) const
    {
        return op.getHandle() == handle;
    };

private:
    Handle handle;
};

/** NameComparator is a comparison functor for objects having a getName()
    method. 
 */
template <class T>
class NameComparator {
public:
    NameComparator(const std::string& h) : name(h){};
    bool operator()(const T* op) const
    {
        return name == op->getName();
    };
    bool operator()(const T& op) const
    {
        return name == op.getName();
    };

private:
    std::string name;
};

} // certi

#endif // LIBCERTI_HELPER_HH
