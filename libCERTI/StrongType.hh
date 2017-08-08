// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003, 2004  ONERA
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

#ifndef STRONGTYPE_H
#define STRONGTYPE_H

#include <type_traits>
#include <utility>

namespace certi {
template <typename T, typename Parameter>
class NamedType {
public:
    explicit constexpr NamedType(T const& value) : value_(value)
    {
    }

    template <typename T_ = T>
    explicit constexpr NamedType(T&& value, typename std::enable_if<!std::is_reference<T_>{}, std::nullptr_t>::type = nullptr)
        : value_(std::move(value))
    {
    }

    T& get()
    {
        return value_;
    }

    T const& get() const
    {
        return value_;
    }

private:
    T value_;
};
}

#endif // STRONGTYPE_H
