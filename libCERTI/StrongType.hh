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

#ifndef STRONGTYPE_HH
#define STRONGTYPE_HH

/**
 * This code is from Jonathan Boccora's "NamedType"
 * 
 * More information https://github.com/joboccara/NamedType
 *                  http://www.fluentcpp.com/
 * 
 */

#include <functional>
#include <iostream>
#include <limits>
#include <ratio>
#include <type_traits>

namespace certi {

/// CRTP
template <typename T, template <typename> class crtpType>
struct crtp {
    T& underlying()
    {
        return static_cast<T&>(*this);
    }
    T const& underlying() const
    {
        return static_cast<T const&>(*this);
    }
};

template <typename T, typename Ratio>
struct ConvertWithRatio {
    static T convertFrom(T t)
    {
        return t * Ratio::den / Ratio::num;
    }
    static T convertTo(T t)
    {
        return t * Ratio::num / Ratio::den;
    }
};

template <typename T, typename Parameter, typename Converter, template <typename> class... Skills>
class NamedTypeImpl;

template <typename T, typename Parameter, template <typename> class... Skills>
using NamedType = NamedTypeImpl<T, Parameter, ConvertWithRatio<T, std::ratio<1>>, Skills...>;

/////////////////////////////////
// UNDERLYING FUNCTIONNALITIES //
/////////////////////////////////

template <typename T>
struct Invalidable : crtp<T, Invalidable> {
    /** We rely on c++'s implicit conversion to bool to ensure validity
         * 
         * e.g for ints, invalid value is 0 and all other are valid
         */
    bool isValid() const
    {
        return this->underlying().get();
    }
};

template <typename T>
struct Incrementable : crtp<T, Incrementable> {
    T& operator+=(T const& other)
    {
        this->underlying().get() += other.get();
        return this->underlying();
    }
};

template <typename T>
struct Addable : crtp<T, Addable> {
    T operator+(T const& other) const
    {
        return T(this->underlying().get() + other.get());
    }
};

template <typename T>
struct Comparable : crtp<T, Comparable> {
    bool operator==(T const& other) const
    {
        return this->underlying().get() == other.get();
    }
    bool operator!=(T const& other) const
    {
        return !(*this == other);
    }
};

template <typename T>
struct Printable : crtp<T, Printable> {
    void print(std::ostream& os) const
    {
        os << this->underlying().get();
    }
};

template <typename Destination>
struct ImplicitlyConvertibleTo {
    template <typename T>
    struct templ : crtp<T, templ> {
        operator Destination() const
        {
            return this->underlying().get();
        }
    };
};

template <typename T, typename Parameter, template <typename> class... Skills>
std::ostream& operator<<(std::ostream& os, NamedType<T, Parameter, Skills...> const& object)
{
    object.print(os);
    return os;
}

template <typename T>
struct Hashable {
    static constexpr bool is_hashable = true;
};

} // namespace certi

namespace std {
template <typename T, typename Parameter, typename Converter, template <typename> class... Skills>
struct hash<certi::NamedTypeImpl<T, Parameter, Converter, Skills...>> {
    using NamedType = certi::NamedTypeImpl<T, Parameter, Converter, Skills...>;
    using checkIfHashable = typename std::enable_if<NamedType::is_hashable, void>::type;

    size_t operator()(certi::NamedTypeImpl<T, Parameter, Converter, Skills...> const& x) const
    {
        return std::hash<T>()(x.get());
    }
};

template <typename T, typename Parameter, typename Converter, template <typename> class... Skills>
struct numeric_limits<certi::NamedTypeImpl<T, Parameter, Converter, Skills...>> : public numeric_limits<T> {
};
}

namespace certi {

template <typename T, T (*from)(T), T (*to)(T)>
struct Convert {
    static T convertFrom(T t)
    {
        return from(t);
    }
    static T convertTo(T t)
    {
        return to(t);
    }
};

template <typename T, typename Parameter, typename Converter, template <typename> class... Skills>
class NamedTypeImpl : public Skills<NamedTypeImpl<T, Parameter, Converter, Skills...>>... {
public:
    // constructor
    explicit NamedTypeImpl(T const& value) : value_(value)
    {
    }
    template <typename T_ = T>
    explicit NamedTypeImpl(T&& value, typename std::enable_if<!std::is_reference<T_>{}, std::nullptr_t>::type = nullptr)
        : value_(std::move(value))
    {
    }

    // get
    T& get()
    {
        return value_;
    }
    T const& get() const
    {
        return value_;
    }

    // conversions
    using UnderlyingType = T;
    template <typename Converter2>
    operator NamedTypeImpl<T, Parameter, Converter2, Skills...>() const
    {
        return NamedTypeImpl<T, Parameter, Converter2, Skills...>(Converter2::convertFrom(Converter::convertTo(get())));
    }

    struct conversions {
        template <T (*f)(T), T (*g)(T)>
        struct compose {
            static T func(T t)
            {
                return f(g(t));
            }
        };
        template <typename Converter1, typename Converter2>
        using ComposeConverter = Convert<T,
                                         compose<Converter1::convertFrom, Converter2::convertFrom>::func,
                                         compose<Converter1::convertTo, Converter2::convertTo>::func>;
        template <typename Converter2>
        using GetConvertible = NamedTypeImpl<T, Parameter, ComposeConverter<Converter, Converter2>, Skills...>;
    };

private:
    T value_;
};

template <typename T, typename Parameter, template <typename> class... Skills>
using NamedType = NamedTypeImpl<T, Parameter, ConvertWithRatio<T, std::ratio<1>>, Skills...>;

template <typename StrongType, typename Ratio>
using MultipleOf = typename StrongType::conversions::
    template GetConvertible<ConvertWithRatio<typename StrongType::UnderlyingType, Ratio>>;

template <typename StrongType, typename Converter>
using ConvertibleTo = typename StrongType::conversions::template GetConvertible<Converter>;

template <template <typename T> class StrongType, typename T>
StrongType<T> make_named(T const& value)
{
    return StrongType<T>(value);
}

} // namespace certi

#endif // STRONGTYPE_HH
