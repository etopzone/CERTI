// ----------------------------------------------------------------------------
// HLAfixedArray.hh - IEEE 1516.2 compliant datatypes
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
// $Id: HLAfixedArray.hh,v 1.2 2009/06/24 12:33:31 gotthardp Exp $
// ----------------------------------------------------------------------------

#ifndef _HLATYPES_FIXEDARRAY_HH
#define _HLATYPES_FIXEDARRAY_HH

#include <stdexcept>

namespace libhla {

/**
 * @page certi_HLAfixedArray Fixed Array
 *
 * The template
\verbatim
 HLAfixedArray<DATATYPE, NUMBER>
\endverbatim
 * defines a fixed array of NUMBER elements of type DATATYPE.
 *
 * The data can be accessed in an usual way.
 *
 * For example:
\verbatim
 +-------------+----------------+-------------+-----------------+-----------+
 | Name        | Element type   | Cardinality | Encoding        | Semantics |
 +-------------+----------------+-------------+-----------------+-----------+
 | Coordinates | HLAinteger32BE | 3           | HLAfixedArray   |           |
 +-------------+----------------+-------------+-----------------+-----------+

 typedef HLAfixedArray<HLAinteger32BE,3> Coordinates;
 HLAdata<Coordinates> value;

 (*value)[0] = 100;
 (*value)[1] = 200;
\endverbatim
 */

//! Fixed array of <N> elements of type <M>
template<class M, int N, bool hasVariable = M::m_isVariable>
struct HLAfixedArray;

template<class M, int N, bool hasVariable>
std::ostream& PrintBuffer(std::ostream& stream, HLAfixedArray<M,N,hasVariable>& buffer)
{ return __print_buffer(stream, (void*)&buffer, buffer.__sizeof()); }

// Fixed array optimized for fixed-size elements
// note: this is the most efficient data type
template<class M, int N>
struct HLAfixedArray<M, N, false>
{
    static const size_t size()
    { return N; }

    static const size_t offset(long i)
    { return i*(M::__sizeof() + __padding(M::__sizeof(), M::m_octetBoundary)); }

    M& operator[](long i) const
    {
        if (i < 0 || i >= (long)size())
            throw std::out_of_range("HLAfixedArray: index out of range");
        return *(M*)((char*)this + offset(i));
    }

    static const size_t emptysizeof()
    { return __sizeof(); }

    // Padding shall not be added after the last element of the array.
    static const size_t __sizeof()
    { return offset(N-1) + M::__sizeof(); }

    void copy(void* source)
    {
        size_t offs = 0;
        // copy all elements of the structure
        for (size_t i = 0; i < N; i++) {
            ((M*)((char*)this + offs))->copy((char*)source + offs);
            offs += M::__sizeof() + __padding(M::__sizeof(), M::m_octetBoundary);
        }
    }

    static const size_t m_octetBoundary = M::m_octetBoundary;
    static const bool m_isVariable = false; // fixed-size array of fixed-size elements
};

// Generic fixed array, supports variable-size elements
template<class M, int N>
struct HLAfixedArray<M, N, true>
{
    static const size_t size()
    { return N; }

    const size_t offset(long i) const
    {
        size_t offs = 0;
        // count every member, the elements may be variable-sized
        for (long j=0; j<i; j++) {
            offs += ((M*)((char*)this + offs))->__sizeof();
            offs += __padding(offs, M::m_octetBoundary);
        }
        return offs;
    }

    M& operator[](long i) const
    {
        if (i >= size())
            throw std::out_of_range("HLAfixedArray: index out of range");
        return *(M*)((char*)this + offset(i));
    }

    static const size_t emptysizeof()
    {
        size_t size = N*M::emptysizeof();
        // padding shall not be added after the last element of the array
        if(N > 1)
            size += (N-1)*__padding(M::emptysizeof(), M::m_octetBoundary);
        return size;
    }

    const size_t __sizeof() const
    {
        size_t offs = offset(N-1);
        return offs + ((M*)((char*)this + offs))->__sizeof();
    }

    void copy(void* source)
    {
        size_t offsD = 0;
        size_t offsS = 0;
        // copy all elements of the structure, the elements may be variable-sized
        for (size_t i = 0; i < N; i++) {
            ((M*)((char*)this + offsD))->copy((char*)source + offsS);

            offsD += ((M*)((char*)this + offsD))->__sizeof(); // may differ from source size
            offsD += __padding(offsD, M::m_octetBoundary);
            offsS += ((M*)((char*)source + offsS))->__sizeof();
            offsS += __padding(offsS, M::m_octetBoundary);
        }
    }

    static const size_t m_octetBoundary = M::m_octetBoundary;
    static const bool m_isVariable = true; // variable-sized elements
};

} // namespace libhla

#endif // _HLATYPES_FIXEDARRAY_HH

// $Id: HLAfixedArray.hh,v 1.2 2009/06/24 12:33:31 gotthardp Exp $

