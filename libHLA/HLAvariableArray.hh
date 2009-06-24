// ----------------------------------------------------------------------------
// HLAvariableArray.hh - IEEE 1516.2 compliant datatypes
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
// $Id: HLAvariableArray.hh,v 1.4 2009/06/24 12:33:31 gotthardp Exp $
// ----------------------------------------------------------------------------

#ifndef _HLATYPES_VARIABLEARRAY_HH
#define _HLATYPES_VARIABLEARRAY_HH

#include <HLAbuffer.hh>
#include <HLAbasicType.hh>

namespace libhla {

/**
 * @page certi_HLAvariableArray Variable Array
 *
 * The template
\verbatim
 HLAvariableArray<DATATYPE>
\endverbatim
 * defines an array of a variable number of DATATYPE elements.
 *
 * The size() member must be set before accessing the data. No data are moved
 * when the size() is changed.
 *
 * For example:
\verbatim
 +-------------+----------------+-------------+-----------------+-----------+
 | Name        | Element type   | Cardinality | Encoding        | Semantics |
 +-------------+----------------+-------------+-----------------+-----------+
 | List        | HLAinteger32BE | Dynamic     | HLAvaribleArray |           |
 +-------------+----------------+-------------+-----------------+-----------+

 typedef HLAvariableArray<HLAinteger32BE> List;
 HLAdata<List> value;

 (*value).set_size(2);
 (*value)[0] = 100;
 (*value)[1] = 200;
\endverbatim
 */

//! Variable array of type <M>
template<class M, bool hasVariable = M::m_isVariable>
struct HLAvariableArray;

template<class M, bool hasVariable>
std::ostream& PrintBuffer(std::ostream& stream, HLAvariableArray<M,hasVariable>& buffer)
{ return __print_buffer(stream, (void*)&buffer, buffer.__sizeof()); }

// Variable array optimized for fixed-size elements
template<class M>
struct HLAvariableArray<M, false>
{
    //! Get/Set array size, without memory management
    /* This function assumes no data are stored in the buffer after this array.
     * You may want to use set_size() instead.
     */
    HLAinteger32BE& size() const
    { return (HLAinteger32BE&)*this; }

    //! Set array size, with memory management
    void set_size(long i)
    {
        if (i == size())
            return; // no change

        // as changing the array size may impact padding, the whole buffer needs to be reorganized
        __HLAbuffer::shake(this, i,
            (i-size())*(long)(M::__sizeof() + __padding(M::__sizeof(), M::m_octetBoundary)));
    }

    static const size_t offset(long i)
    { return emptysizeof() + i*(M::__sizeof() + __padding(M::__sizeof(), M::m_octetBoundary)); }

    M& operator[](long i) const
    {
        if (i >= size())
            throw std::out_of_range("HLAvariableArray: index out of range");
        return *(M*)((char*)this + offset(i));
    }

    static const size_t emptysizeof()
    { return HLAinteger32BE::__sizeof() + __padding(HLAinteger32BE::__sizeof(), M::m_octetBoundary); }

    // Padding shall not be added after the last element of the array.
    const size_t __sizeof() const
    {
        if (size() > 0)
            return offset(size()-1) + M::__sizeof();
        else
            return emptysizeof();
    }

    void copy(void* source)
    {
        int N = *(HLAinteger32BE*)source;
        int toCopy;

        __HLAbuffer& buffer = __HLAbuffer::__buffer(this);
        if(source == buffer.mShakeThat) {
            *(HLAinteger32BE*)this = buffer.mShakeValue;
            toCopy = std::min(N, buffer.mShakeValue); // number of elements to copy
        }
        else {
            *(HLAinteger32BE*)this = N;
            toCopy = N; // copy all elements
        }

        size_t offs = emptysizeof();
        // copy all elements of the structure, the elements may be variable-sized
        for (int i = 0; i < toCopy; i++) {
            ((M*)((char*)this + offs))->copy((char*)source + offs);
            offs += M::__sizeof() + __padding(M::__sizeof(), M::m_octetBoundary);
        }
    }

    static const size_t m_octetBoundary =
        MAX(HLAinteger32BE::m_octetBoundary, M::m_octetBoundary);
    static const bool m_isVariable = true; // variable-sized array
};

// Generic variable array, supports variable-size elements
// note: this is the less efficient data type
template<class M>
struct HLAvariableArray<M, true>
{
    //! Get/Set array size, without memory management
    /* This function assumes no data are stored in the buffer after this array.
     * You may want to use set_size() instead.
     */
    HLAinteger32BE& size() const
    { return (HLAinteger32BE&)*this; }

    //! Set array size, with memory management
    void set_size(long i)
    {
        if (i == size())
            return; // no change

        // as changing the array size may impact padding, the whole buffer needs to be reorganized
        __HLAbuffer::shake(this, i,
            (i-size())*(long)(M::emptysizeof() + __padding(M::emptysizeof(), M::m_octetBoundary)));
    }

    const size_t offset(long i) const
    {
        size_t offs = emptysizeof();
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
            throw std::out_of_range("HLAvariableArray: index out of range");
        return *(M*)((char*)this + offset(i));
    }

    static const size_t emptysizeof()
    { return HLAinteger32BE::__sizeof() + __padding(HLAinteger32BE::__sizeof(), M::m_octetBoundary); }

    // Padding shall not be added after the last element of the array.
    const size_t __sizeof() const
    {
        if (size() > 0) {
            size_t offs = offset(size()-1);
            return offs + ((M*)((char*)this + offs))->__sizeof();
        }
        else
            return emptysizeof();
    }

    void copy(void* source)
    {
        int N = *(HLAinteger32BE*)source;
        int toCopy;

        __HLAbuffer& buffer = __HLAbuffer::__buffer(this);
        if(source == buffer.mShakeThat) {
            *(HLAinteger32BE*)this = buffer.mShakeValue;
            toCopy = std::min(N, buffer.mShakeValue); // number of elements to copy
        }
        else {
            *(HLAinteger32BE*)this = N;
            toCopy = N; // copy all elements
        }

        size_t offsD = emptysizeof();
        size_t offsS = emptysizeof();
        // copy all elements of the structure, the elements may be variable-sized
        for (int i = 0; i < toCopy; i++) {
            ((M*)((char*)this + offsD))->copy((char*)source + offsS);

            offsD += ((M*)((char*)this + offsD))->__sizeof(); // may differ from source size
            offsD += __padding(offsD, M::m_octetBoundary);
            offsS += ((M*)((char*)source + offsS))->__sizeof();
            offsS += __padding(offsS, M::m_octetBoundary);
        }
    }

    static const size_t m_octetBoundary =
        MAX(HLAinteger32BE::m_octetBoundary, M::m_octetBoundary);
    static const bool m_isVariable = true; // variable-size array of variable-size elements
};

/* IEEE 1516.2, Table 29:
 * Array datatype table
 */
struct HLAASCIIstring : public HLAvariableArray<HLAASCIIchar>
{
    HLAASCIIstring& operator = (const std::string& it)
    {
        __HLAbuffer& buffer = __HLAbuffer::__buffer(this);
        size_t offset = (char*)this - buffer.mBegin;

        set_size(it.size());
        // note: set_size may have caused realloc()
        memcpy(buffer.mBegin + offset + emptysizeof(), it.data(), it.size());

        return *this;
    }

    operator std::string() const
    {
        return std::string((char*)this + emptysizeof(), size());
    }
};

typedef HLAvariableArray<HLAunicodeChar> HLAunicodeString;
typedef HLAvariableArray<HLAbyte> HLAopaqueData;

} // namespace libhla

#endif // _HLATYPES_VARIABLEARRAY_HH

// $Id: HLAvariableArray.hh,v 1.4 2009/06/24 12:33:31 gotthardp Exp $

