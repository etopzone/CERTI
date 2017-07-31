// ----------------------------------------------------------------------------
// HLAvariantRecord.hh - IEEE 1516.2 compliant datatypes
// Copyright (C) 2008  Petr Gotthard <petr.gotthard@centrum.cz>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1, as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// $Id: HLAvariantRecord.hh,v 1.3 2011/07/11 11:00:09 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _HLATYPES_VARIANTRECORD_HH
#define _HLATYPES_VARIANTRECORD_HH

#include "HLAbuffer.hh"

namespace libhla {

/**
 * @page certi_HLAvariantRecord Variant Record
 *
 * The template
\verbatim
 HLAvariantRecord<
   INDEX, DATATYPE,
   HLAvariantField<ENUMERATORS1, INDEX1, DATATYPE1,
   HLAvariantField<ENUMERATORS2, INDEX2, DATATYPE2,
   ...
   > ... > TYPENAME;
\endverbatim
 * defines an ordered sequence of DATATYPE entries.
 *
 * The data can be accessed using the field<INDEX>() function. The INDEX is a logical
 * identifier only. 
 * The first field is a discriminant. It is followed by an alternative whose
 * ENUMERATORS match the discriminant value.
 *
 * For example:
\verbatim
 +-------------+-------------------------------+------------------------------------+------------------+-----------+
 |             | Discriminant                  | Alternative                        |                  |           |
 | Record name +------+-----------+------------+---------+--------------+-----------+ Encoding         | Semantics |
 |             | Name | Type      | Enumerator | Name    | Type         | Semantics |                  |           |
 +-------------+------+-----------+------------+---------+--------------+-----------+------------------+-----------+
 |             |      |           | AXIS_X     | FIELD_X | HLAfloat32LE |           |                  |           |
 | Coordinates | TYPE | TypesEnum +------------+---------+--------------+-----------+ HLAvariantRecord |           |
 |             |      |           | AXIS_Y     | FIELD_Y | HLAfloat32LE |           |                  |           |
 +-------------+------+-----------+------------+---------+--------------+-----------+------------------+-----------+
 
 namespace __Fields {
 enum __enum {
   TYPE = 0,
   FIELD_X = 101,
   FIELD_Y = 102
 };
 }
 typedef HLAenumeratedType<__Fields::__enum, HLAinteger32BE> Fields;
 typedef HLAvariantRecord<
   __Fields::TYPE, TypesEnum,
   HLAvariantField<HLAsetValue<AXIS_X>, __Fields::FIELD_X, HLAfloat32LE,
   HLAvariantField<HLAsetValue<AXIS_Y>, __Fields::FIELD_Y, HLAfloat32LE
   > > > Coordinates;
 HLAdata<Coordinates> value;

 value->set_discriminant(AXIS_X);
 value->field<__Fields::FIELD_X>() = 3.14;
\endverbatim
 */

//! Variant record of discriminant <D> at index <E> and alternatives <R>
template <int DE, class DM, class R, bool hasVariable = R::m_isVariable>
struct HLAvariantRecord;

template <int DE, class DM, class R, bool V>
std::ostream& PrintBuffer(std::ostream& stream, HLAvariantRecord<DE, DM, R, V>& buffer)
{
    return __print_buffer(stream, (void*) &buffer, buffer.__sizeof());
}

//! Returns <i>th field of the HLAfixedField <L>
template <class R, int i>
struct __FieldAt;

//! Returns the discriminant, or a <i>th field of the HLAvariantField <R>
template <int DE, class DM, class R, int e>
struct __DiscriminantOrFieldAt;

// Variant record optimized for fixed-size fields
template <int DE, class DM, class R>
struct HLAvariantRecord<DE, DM, R, false> {
    //! Get/Set the discriminant, without memory management
    /* This function assumes no data are stored in the buffer after this array.
     * You may want to use set_discriminant() instead.
     */
    DM& discriminant() const
    {
        return *(DM*) this;
    }

    //! Set the discriminant, with memory management
    void set_discriminant(int d)
    {
        if (d == (int) discriminant())
            return; // no change

        // as changing the discriminant may impact padding, the whole buffer needs to be reorganized
        __HLAbuffer::shake(this, d, (long) R::field_emptysizeof(d));
    }

    template <int e>
    typename __DiscriminantOrFieldAt<DE, DM, R, e>::Type& field() const
    {
        // return the discriminant
        if (e == DE)
            return *(typename __DiscriminantOrFieldAt<DE, DM, R, e>::Type*) this;

        if (e != R::get_field(discriminant()))
            throw std::out_of_range("HLAvariantRecord: wrong discriminant");
        // return one of the alternatives
        return *(typename __DiscriminantOrFieldAt<DE, DM, R, e>::Type*) ((char*) this + emptysizeof());
    }

    static size_t emptysizeof()
    {
        return DM::__sizeof() + __padding(DM::__sizeof(), R::m_octetBoundary);
    }

    size_t __sizeof() const
    {
        if (R::has_field(discriminant()))
            return emptysizeof() + R::field_sizeof(discriminant());
        else
            return emptysizeof();
    }

    void copy(void* source)
    {
        __HLAbuffer& buffer = __HLAbuffer::__buffer(this);
        if (source == buffer.mShakeThat) {
            *(DM*) this = buffer.mShakeValue;
            // switching variant: content is removed
        }
        else {
            int d = *(DM*) source;
            // not switching variant: copy the content
            *(DM*) this = d;
            ((R*) this)->copy(d, (char*) source + emptysizeof());
        }
    }

    static const size_t m_octetBoundary = MAX(DM::m_octetBoundary, R::m_octetBoundary);
    static const bool m_isVariable = true; // variant record of fixed-size fields
};

// Generic variant record, supports variable-size fields
template <int DE, class DM, class R>
struct HLAvariantRecord<DE, DM, R, true> {
    //! Get/Set the discriminant, without memory management
    /* This function assumes no data are stored in the buffer after this array.
     * You may want to use set_discriminant() instead.
     */
    DM& discriminant() const
    {
        return *(DM*) this;
    }

    //! Set the discriminant, with memory management
    void set_discriminant(int d)
    {
        if (d == (int) discriminant())
            return; // no change

        // as changing the discriminant may impact padding, the whole buffer needs to be reorganized
        __HLAbuffer::shake(this, d, (long) R::field_emptysizeof(d));
    }

    template <int e>
    typename __DiscriminantOrFieldAt<DE, DM, R, e>::Type& field() const
    {
        // return the discriminant
        if (e == DE)
            return *(typename __DiscriminantOrFieldAt<DE, DM, R, e>::Type*) this;

        if (e != R::get_field(discriminant()))
            throw std::out_of_range("HLAvariantRecord: wrong discriminant");
        // return one of the alternatives
        return *(typename __DiscriminantOrFieldAt<DE, DM, R, e>::Type*) ((char*) this + emptysizeof());
    }

    static size_t emptysizeof()
    {
        return DM::__sizeof() + __padding(DM::__sizeof(), R::m_octetBoundary);
    }

    size_t __sizeof() const
    {
        if (R::has_field(discriminant()))
            return emptysizeof() + ((R*) ((char*) this + emptysizeof()))->field_sizeof(discriminant());
        else
            return emptysizeof();
    }

    void copy(void* source)
    {
        __HLAbuffer& buffer = __HLAbuffer::__buffer(this);
        if (source == buffer.mShakeThat) {
            *(DM*) this = buffer.mShakeValue;
            // switching variant: content is removed
        }
        else {
            int d = *(DM*) source;
            // not switching variant: copy the content
            *(DM*) this = d;
            ((R*) this)->copy(d, (char*) source + emptysizeof());
        }
    }

    static const size_t m_octetBoundary = MAX(DM::m_octetBoundary, R::m_octetBoundary);
    static const bool m_isVariable = true; // variant record of variable-sized fields
};

struct HLAsetEnd;

//! Defines a value in an enumerator list; to be used with HLAvariantField
template <int e, class N = HLAsetEnd>
struct HLAsetValue {
    static int includes(int i)
    {
        if (i == e)
            return 1;
        else
            return N::includes(i);
    }
};

//! Defines a range in an enumerator list
template <int e1, int e2, class N = HLAsetEnd>
struct HLAsetRange {
    static int includes(int i)
    {
        if (e1 <= i && i <= e2)
            return 1;
        else
            return N::includes(i);
    }
};

//! Defines the "HLAother" symbol in an enumerator list
template <class N = HLAsetEnd>
struct HLAsetOther {
    static int includes(int /*i*/)
    {
        return 1;
    }
};

//! Defines the end of an enumerator list
struct HLAsetEnd {
    static int includes(int /*i*/)
    {
        return 0;
    }
};

struct HLAvariantEnd;

//! Record at index <E>, field of type <M>, followed by HLAvariantField <N>
// note: <E> must be "int" to enable implicit conversion from/to enum types
template <class D, int E, class M, class N = HLAvariantEnd, bool hasVariable = M::m_isVariable || N::m_isVariable>
struct HLAvariantField;

// List of fixed-size fields
template <class D, int E, class M, class N>
struct HLAvariantField<D, E, M, N, false> {
    static bool has_field(int d)
    {
        if (D::includes(d))
            return true; // found
        else
            return N::has_field(d); // continue searching
    }

    static int get_field(int d)
    {
        if (D::includes(d))
            return E;
        else
            return N::get_field(d);
    }

    static size_t field_emptysizeof(int e)
    {
        if (e == E)
            return M::emptysizeof();
        else
            return N::field_emptysizeof(e);
    }

    static size_t field_sizeof(int e)
    {
        if (e == E)
            return M::__sizeof();
        else
            return N::field_sizeof(e);
    }

    void copy(int e, void* source)
    {
        if (e == E)
            ((M*) this)->copy(source);
        else
            ((N*) this)->copy(e, source);
    }

    static const size_t m_octetBoundary = MAX(M::m_octetBoundary, N::m_octetBoundary);
    static const bool m_isVariable = false; // fixed-sized fields
};

// List containg variable-size fields
template <class D, int E, class M, class N>
struct HLAvariantField<D, E, M, N, true> {
    static bool has_field(int d)
    {
        if (D::includes(d))
            return true; // found
        else
            return N::has_field(d); // continue searching
    }

    static int get_field(int d)
    {
        if (D::includes(d))
            return E;
        else
            return N::get_field(d);
    }

    static size_t field_emptysizeof(int e)
    {
        if (e == E)
            return M::emptysizeof();
        else
            return N::field_emptysizeof(e);
    }

    size_t field_sizeof(int e) const
    {
        if (e == E)
            return ((M*) this)->__sizeof();
        else
            return ((N*) this)->field_sizeof(e);
    }

    void copy(int e, void* source)
    {
        if (e == E)
            ((M*) this)->copy(source);
        else
            ((N*) this)->copy(e, source);
    }

    static const size_t m_octetBoundary = MAX(M::m_octetBoundary, N::m_octetBoundary);
    static const bool m_isVariable = true; // variable-sized fields
};

//! Defines a last field in the fixed record
struct HLAvariantEnd {
    static bool has_field(int /*d*/)
    {
        return false;
    }
    static int get_field(int /*d*/)
    {
        throw std::out_of_range("HLAvariantRecord: unknown discriminant");
    }
    static size_t field_emptysizeof(int /*e*/)
    {
        return 0;
    }
    static size_t field_sizeof(int /*e*/)
    {
        return 0;
    }
    void copy(int /*e*/, void* /*source*/)
    { /* nop */
    }

    static const size_t m_octetBoundary = 0;
    static const bool m_isVariable = false;
};

template <bool C, class Then, class Else>
struct __variantRecord_if {
    typedef Then X;
};

template <class Then, class Else>
struct __variantRecord_if<false, Then, Else> {
    typedef Else X;
};

template <int DE, class DM, class R, int e>
struct __DiscriminantOrFieldAt {
    typedef typename __variantRecord_if<e == DE, DM, typename __FieldAt<R, e>::Type>::X Type;
};

template <class D, int E, class M, class N, bool V, int e>
struct __FieldAt<HLAvariantField<D, E, M, N, V>, e> {
    typedef typename __variantRecord_if<e == E, M, typename __FieldAt<N, e>::Type>::X Type;
};

// returned when no HLAvariantField has a given index(i)
template <int e>
struct __FieldAt<HLAvariantEnd, e> {
    typedef HLAvariantEnd Type;
};

} // namespace libhla

#endif // _HLATYPES_VARIANTRECORD_HH

// $Id: HLAvariantRecord.hh,v 1.3 2011/07/11 11:00:09 erk Exp $
