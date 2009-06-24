// ----------------------------------------------------------------------------
// HLAfixedRecord.hh - IEEE 1516.2 compliant datatypes
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
// $Id: HLAfixedRecord.hh,v 1.3 2009/06/24 12:33:31 gotthardp Exp $
// ----------------------------------------------------------------------------

#ifndef _HLATYPES_FIXEDRECORD_HH
#define _HLATYPES_FIXEDRECORD_HH

#include <HLAbuffer.hh>

namespace libhla {

/**
 * @page certi_HLAfixedRecord Fixed Record
 *
 * The template
\verbatim
 HLAfixedRecord<
   HLAfixedField<INDEX1, DATATYPE1,
   HLAfixedField<INDEX2, DATATYPE2,
   ...
   > ... > TYPENAME;
\endverbatim
 * defines an ordered sequence of DATATYPE entries.
 *
 * The data can be accessed using the field<INDEX>() function. The INDEX is a logical
 * identifier only. The data are stored in the declaration order.
 *
 * For example:
\verbatim
 +-------------+------------------------------------+----------------+-----------+
 |             | Field                              |                |           |
 | Record name +---------+--------------+-----------+ Encoding       | Semantics |
 |             | Name    | Type         | Semantics |                |           |
 +-------------+---------+--------------+-----------+----------------+-----------+
 |             | FIELD_X | HLAfloat32LE |           |                |           |
 |             +---------+--------------+-----------+                |           |
 | Coordinates | FIELD_Y | HLAfloat32LE |           | HLAfixedRecord |           |
 |             +---------+--------------+-----------+                |           |
 |             | FIELD_Z | HLAfloat32LE |           |                |           |
 +-------------+---------+--------------+-----------+----------------+-----------+
 
 enum {
   FIELD_X = 0,
   FIELD_Y,
   FIELD_Z
 };
 typedef HLAfixedRecord<
   HLAfixedField<FIELD_X, HLAfloat32LE,
   HLAfixedField<FIELD_Y, HLAfloat32LE,
   HLAfixedField<FIELD_Z, HLAfloat32LE
   > > > > Coordinates;
 HLAdata<Coordinates> value;

 value->field<FIELD_X>() = 3.14;
 value->field<FIELD_Y>() = 6.28;
 value->field<FIELD_Z>() = 9.42;
\endverbatim
 */

//! Fixed record of fields <R>
template<class R, bool hasVariable = R::m_isVariable>
struct HLAfixedRecord;

template<class R, bool V>
std::ostream& PrintBuffer(std::ostream& stream, HLAfixedRecord<R,V>& buffer)
{ return __print_buffer(stream, (void*)&buffer, buffer.__sizeof()); }

//! Returns <i>th field of the HLAfixedField <L>
template<class R, int i> struct __FieldAt;

// Fixed record optimized for fixed-size fields
template<class R>
struct HLAfixedRecord<R, false>
{
    template <int i>
    typename __FieldAt<R,i>::Type& field() const
    { return *(typename __FieldAt<R,i>::Type*)((char*)this + R::field_offsetof(i)); }

    static const size_t emptysizeof()
    { return R::emptysizeof(); }

    static const size_t __sizeof()
    { return R::__sizeof(); }

    void copy(void* source)
    { ((R*)this)->copy(source); }

    static const size_t m_octetBoundary = R::m_octetBoundary;
    static const bool m_isVariable = false; // fixed-sized fields
};

// Generic fixed record, supports variable-size fields
template<class R>
struct HLAfixedRecord<R, true>
{
    template <int i>
    typename __FieldAt<R,i>::Type& field() const
    { return *(typename __FieldAt<R,i>::Type*)((char*)this + ((R*)this)->field_offsetof(i)); }

    static const size_t emptysizeof()
    { return R::emptysizeof(); }

    const size_t __sizeof()
    { return ((R*)this)->__sizeof(); }

    void copy(void* source)
    { ((R*)this)->copy(source); }

    static const size_t m_octetBoundary = R::m_octetBoundary;
    static const bool m_isVariable = true; // variable-sized fields
};

struct HLAfixedEnd;

//! Record at index <E>, field of type <M>, followed by HLAfixedField <N>
// note: <E> must be "int" to enable implicit conversion from/to enum types
template<int E, class M, class N = HLAfixedEnd, bool hasVariable = M::m_isVariable || N::m_isVariable>
struct HLAfixedField;

// List of fixed-size fields
template<int E, class M, class N>
struct HLAfixedField<E, M, N, false>
{
    static const size_t field_offsetof(int d, size_t offs = 0)
    {
        if (d != E) {
            size_t size = M::__sizeof();
            size += __padding(offs+size, N::memberBoundary);
            return N::field_offsetof(d, offs+size);
        }
        else
            return offs;
    }

    static const size_t memberBoundary = M::m_octetBoundary;

    static const size_t emptysizeof(size_t offs = 0)
    { return __sizeof(offs); }

    // Padding shall not be added after the last element of the array.
    static const size_t __sizeof(size_t offs = 0)
    {
        size_t size = M::__sizeof();
        // if not reached HLAvariantEnd
        if (N::memberBoundary) {
            size += __padding(offs+size, N::memberBoundary);
            return N::__sizeof(offs+size);
        }
        else
            return offs+size;
    }

    void copy(void* source, size_t offsD = 0, size_t offsS = 0)
    {
        ((M*)this)->copy(source);
        if (N::memberBoundary) {
            offsD += M::__sizeof();
            offsD += __padding(offsD, N::memberBoundary);
            offsS += M::__sizeof();
            offsS += __padding(offsS, N::memberBoundary);
            ((N*)((char*)this + M::__sizeof()))->copy((char*)source + M::__sizeof(), offsD, offsS);
        }
    }

    static const size_t m_octetBoundary = MAX(M::m_octetBoundary, N::m_octetBoundary);
    static const bool m_isVariable = false; // fixed-sized fields
};

// List containg variable-size fields
template<int E, class M, class N>
struct HLAfixedField<E, M, N, true>
{
    const size_t field_offsetof(int d, size_t offs = 0) const
    {
        if (d != E) {
            size_t size = ((M*)this)->__sizeof();
            size += __padding(offs+size, N::memberBoundary);
            return ((N*)((char*)this + size))->field_offsetof(d, offs+size);
        }
        else
            return offs;
    }

    static const size_t memberBoundary = M::m_octetBoundary;

    static const size_t emptysizeof(size_t offs = 0)
    {
        size_t size = M::emptysizeof();
        // if not reached HLAvariantEnd
        if (N::memberBoundary) {
            size += __padding(offs+size, N::memberBoundary);
            return N::emptysizeof(offs+size);
        }
        else
            return offs+size;
    }

    // Padding shall not be added after the last element of the array.
    const size_t __sizeof(size_t offs = 0) const
    {
      size_t size = ((M*)this)->__sizeof();
      // if not reached HLAvariantEnd
      if (N::memberBoundary) {
          size += __padding(offs+size, N::memberBoundary);
          return ((N*)((char*)this + size))->__sizeof(offs+size);
      }
      else
          return offs+size;
    }

    void copy(void* source, size_t offsD = 0, size_t offsS = 0)
    {
        ((M*)this)->copy(source);
        if (N::memberBoundary) {
            size_t sizeD = ((M*)this)->__sizeof(); // may differ from source size
            sizeD += __padding(offsD+sizeD, N::memberBoundary);
            size_t sizeS = ((M*)source)->__sizeof();
            sizeS += __padding(offsS+sizeS, N::memberBoundary);
            ((N*)((char*)this + sizeD))->copy((char*)source + sizeS, offsD+sizeD, offsS+sizeS);
        }
    }

    static const size_t m_octetBoundary = MAX(M::m_octetBoundary, N::m_octetBoundary);
    static const bool m_isVariable = true; // variable-sized fields
};

//! Defines a last field in the fixed record
struct HLAfixedEnd
{
    static const size_t field_offsetof(int d, size_t offs = 0)
    { return offs; }

    static const size_t memberBoundary = 0;
    static const size_t emptysizeof(size_t offs = 0)
    { return offs; }
    static const size_t __sizeof(size_t offs = 0)
    { return offs; }
    void copy(void* source, size_t offsD = 0, size_t offsS = 0)
    { /* nop */ }

    static const size_t m_octetBoundary = 0;
    static const bool m_isVariable = false;
};

template<bool C, class Then, class Else>
struct __fixedRecord_if
{ typedef Then X; };

template<class Then, class Else>
struct __fixedRecord_if<false, Then, Else>
{ typedef Else X; };

template<int E, class M, class N, bool V, int d>
struct __FieldAt<HLAfixedField<E, M, N, V>, d>
{ typedef typename __fixedRecord_if<d==E, M, typename __FieldAt<N,d>::Type>::X Type; };

// returned when no HLAfixedField has a given index(i)
template<int d>
struct __FieldAt<HLAfixedEnd, d>
{ typedef HLAfixedEnd Type; };

} // namespace libhla

#endif // _HLATYPES_FIXEDRECORD_HH

// $Id: HLAfixedRecord.hh,v 1.3 2009/06/24 12:33:31 gotthardp Exp $

