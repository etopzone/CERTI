// ----------------------------------------------------------------------------
// HLAtypesIEEE1516.hh - IEEE 1516.2 compliant datatypes
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
// $Id: HLAtypesIEEE1516.hh,v 1.3 2008/08/02 11:44:12 gotthardp Exp $
// ----------------------------------------------------------------------------

#ifndef _HLATYPES_IEEE1516_HH
#define _HLATYPES_IEEE1516_HH

#if defined(_WIN32)
#pragma warning(disable:4503) // suppress warning C4503: decorated name length exceeded
#endif

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <map>
#include <stdexcept>

/* These templates implement efficient access functions that provide direct
 * access to IEEE 1516.2 compliant data buffers.
 * The data are manipulated "in situ", no temporary variables are created.
 *
 * Use hlaomtdif2cpp -i <file> to generate FOM/SOM specific datatypes.
 *
 * The extensive use of template metaprogramming allows many operations to
 * be pre-calculated during compile-time.
 * See http://aszt.inf.elte.hu/~gsd/halado_cpp/ch06s09.html
 */

#ifdef _MSC_VER
typedef unsigned __int64  uint64_t;
typedef __int64           int64_t;
typedef unsigned __int32  uint32_t;
typedef __int32           int32_t;
typedef unsigned __int16  uint16_t;
typedef __int16           int16_t;
typedef unsigned __int8   uint8_t;
typedef __int8            int8_t;
#else
#include <stdint.h>
#endif

namespace libhla {

std::ostream& __print_buffer(std::ostream& stream, const void *buffer, size_t length);

/* Caution:
 * This implementation assumes that there are no compiler-generated data in
 * the heap memory allocated for the __HLAbuffer struct.
 * All structures must have no virtual functions and no non-static members.
 */

class __HLAbuffer
{
private:
    // static buffer for all instantiations of the HLAdata template
    // indexed by pointers after the last element, the end() pointers
    typedef std::map<char*,__HLAbuffer*> BufferList;
    static BufferList gBuffers;

public:
    char *mBegin;
    size_t mCapacity;
    // no automatic free() and realloc() for user allocated memory
    bool mUserAllocated;
    // parameters used during dynamic resizing
    const void* mShakeThat;
    int mShakeValue;

    __HLAbuffer(size_t capacity)
     : mUserAllocated(false), mShakeThat(NULL)
    {
        // exponential growth: capacity *= 1.5
        mCapacity = (size_t)(capacity*1.5);
        mBegin = (char*)calloc(1, mCapacity);
        // store "this" to a global table
        gBuffers[mBegin + mCapacity] = this;
    }

    __HLAbuffer(void *begin, size_t capacity)
      : mBegin((char*)begin), mCapacity(capacity), mUserAllocated(true), mShakeThat(NULL)
    {
        // store "this" to a global table
        gBuffers[mBegin + mCapacity] = this;
    }

    virtual ~__HLAbuffer()
    {
        if (!mUserAllocated)
            free(mBegin);
        // remove "this" from the global table
        gBuffers.erase(__buffer_iterator(mBegin));
    }

    void __exchange_buffers(__HLAbuffer& newBuffer)
    {
        char* oldBegin = mBegin;
        size_t oldCapacity = mCapacity;

        mBegin = newBuffer.mBegin;
        mCapacity = newBuffer.mCapacity;
        gBuffers[mBegin + mCapacity] = this; // update

        newBuffer.mBegin = oldBegin;
        newBuffer.mCapacity = oldCapacity;
        gBuffers[oldBegin + oldCapacity] = &newBuffer; // update
    }

    static BufferList::iterator __buffer_iterator(const void* __this)
    {
        // find the first pointer not less than "this", what is the end() pointer
        BufferList::iterator result = gBuffers.lower_bound((char*)__this);
        if (result == gBuffers.end())
            throw std::runtime_error("HLAdata: bad pointer");
        return result;
    }

    static __HLAbuffer& __buffer(const void* __this)
    { return *(__buffer_iterator(__this)->second); }

#ifndef NDEBUG
    static void __check_memory(const void* __this, size_t size)
    {
        const __HLAbuffer& buffer = __buffer(__this);
        if ((char*)__this + size > (char*)buffer.mBegin + buffer.mCapacity)
            throw std::length_error("HLAdata: data buffer overflow");
    }
#endif

    virtual const size_t size() const = 0;
    virtual void __shake(const void* __that, int value, long resize) = 0;

    static void shake(const void* __that, int value, long resize)
    { __buffer(__that).__shake(__that, value, resize); }

    const char* data() const
    { return mBegin; }

    std::ostream& print(std::ostream& stream)
    {
#ifndef NDEBUG
        __check_memory(mBegin, size());
#endif
        return __print_buffer(stream, mBegin, size());
    }
};

template<class T>
class HLAdata : public __HLAbuffer
{
public:
    //! Create new buffer
    HLAdata(size_t capacity = T::emptysizeof())
      : __HLAbuffer(capacity)
    { }

    //! Create buffer from existing data
    HLAdata(void *begin, size_t capacity)
      : __HLAbuffer(begin, capacity)
    {
        if (capacity < size())
            throw std::length_error("HLAdata: data buffer overflow");
        // do *not* clean the data buffer, it may contain valid data
    }

    T& operator *() const
    { return *(T*)mBegin; }

    T* operator ->() const
    { return (T*)mBegin; }

    virtual const size_t size() const
    { return ((T*)mBegin)->__sizeof(); }

    //! Shake the buffer
    /* Resizing the variable-size elements may have significant impact on padding
     * of other elements. This function changes a <value> of <__that> element
     * and updates the padding through the whole buffer.
     */
    virtual void __shake(const void* __that, int value, long resize)
    {
        HLAdata<T> newData(size()+resize);

        // copy the data to the temporary buffer, while changing a <value> of <__that>
        newData.mShakeThat = __that;
        newData.mShakeValue = value;
        // create copy of the data
        ((T*)newData.mBegin)->copy(mBegin);

        if (mUserAllocated)
        {
            if (newData.size() < mCapacity)
                // copy data back to the original buffer
                memcpy(mBegin, newData.mBegin, newData.size());
            else
                throw std::length_error("HLAdata: data buffer overflow");
        }
        else
            __exchange_buffers(newData);
    }
};

//! Swap <i> bytes of the <T> type
/*! Template specializations are defined for each possible <i>.
 */
template<class T, int i = sizeof(T)>
struct __swap;

//! Conversion to the Little Endian encoding
template<class T>
struct LittleEndian
{
    inline const T operator()(const T& x) const {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        return x;
#elif __BYTE_ORDER == __BIG_ENDIAN
        return __swap<T>()( x );
#else
#error Undefined __BYTE_ORDER
#endif
    }
};

//! Conversion to the Big Endian encoding
template<class T>
struct BigEndian
{
    inline const T operator()(const T& x) const {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        return __swap<T>()( x );
#elif __BYTE_ORDER == __BIG_ENDIAN
        return x;
#endif
    }
};

template<class T>
struct __swap<T,1>
{
    inline const T operator()(const T& x) const {
        return x;
    }
};

template<class T>
struct __swap<T,2>
{
    inline const T operator()(const T& x) const {
        union {
            uint16_t u16;
            T x;
        } result;
        result.u16 =
            (*(uint16_t*)&x<<8 | *(uint16_t*)&x>>8);
        return result.x;
    }
};

template<class T>
struct __swap<T,4>
{
    inline const T operator()(const T& x) const {
        union {
            uint32_t u32;
            T x;
        } result;
        result.u32 =
            (*(uint32_t*)&x<<24 | *(uint32_t*)&x>>24 |
            (*(uint32_t*)&x & 0x0000ff00UL)<<8 |
            (*(uint32_t*)&x & 0x00ff0000UL)>>8);
        return result.x;
    }
};

template<class T>
struct __swap<T,8>
{
    inline const T operator()(const T& x) const {
        union {
            uint64_t u64;
            T x;
        } result;
        result.u64 =
            (*(uint64_t*)&x<<56 | *(uint64_t*)&x>>56 |
            (*(uint64_t*)&x & 0x000000000000ff00ULL)<<40 |
            (*(uint64_t*)&x & 0x0000000000ff0000ULL)<<24 |
            (*(uint64_t*)&x & 0x00000000ff000000ULL)<< 8 |
            (*(uint64_t*)&x & 0x000000ff00000000ULL)>> 8 |
            (*(uint64_t*)&x & 0x0000ff0000000000ULL)>>24 |
            (*(uint64_t*)&x & 0x00ff000000000000ULL)>>40);
        return result.x;
    }
};

//! HLA basic type, represented by <T> type stored in <S> using <E> encoding.
/*!
 * HLAbasicType<DATATYPE, STORAGE, ENCODING>
 * defines a user-convenient DATATYPE, stored in STORAGE using given ENCODING.
 *
 * The data are stored in a buffer of sizeof(STORAGE).
 *
 * The buffer is casted to a DATATYPE that provide data access operators. The
 * data can be accessed in an usual way.
 * The DATATYPE may have any sizeof(), but must have static-cast to STORAGE.
 *
 * For example:
 * typedef HLAbasicType<long, uint32_t, LittleEndian> HLAinteger32BE;
 * HLAdata<HLAinteger32BE> value;
 *
 * value = 42;
 */
template<class T, class S, template<class T>class E>
struct HLAbasicType
{
    HLAbasicType& operator = (const T& data)
    {
#ifndef NDEBUG
        __HLAbuffer::__check_memory(this, __sizeof());
#endif
        *(S*)this = E<S>()(data);
        return *this;
    }

    operator T() const
    {
#ifndef NDEBUG
        __HLAbuffer::__check_memory(this, __sizeof());
#endif
        return E<S>()(*(S*)this);
    }

    static const size_t emptysizeof()
    { return __sizeof(); }

    static const size_t __sizeof()
    { return sizeof(S); }

    void copy(void* source)
    {
#ifndef NDEBUG
        __HLAbuffer::__check_memory(this, __sizeof());
#endif
        memcpy((char*)this, source, __sizeof());
    }

    static const size_t m_octetBoundary = sizeof(S);
    static const bool m_isVariable = false;
};

template<class T, class S, template<class T>class E>
std::ostream& PrintBuffer(std::ostream& stream, HLAbasicType<T,S,E>& buffer)
{ return __print_buffer(stream, (void*)&buffer, buffer.__sizeof()); }

/* IEEE 1516.2, Table 23:
 * Basic data representation table
 */
typedef HLAbasicType<short, int16_t, BigEndian> HLAinteger16BE;
typedef HLAbasicType<long, int32_t, BigEndian> HLAinteger32BE;
typedef HLAbasicType<long long, int64_t, BigEndian> HLAinteger64BE;
typedef HLAbasicType<float, float, BigEndian> HLAfloat32BE;
typedef HLAbasicType<double, double, BigEndian> HLAfloat64BE;
typedef HLAbasicType<wchar_t, wchar_t, BigEndian> HLAoctetPairBE;

typedef HLAbasicType<short, int16_t, LittleEndian> HLAinteger16LE;
typedef HLAbasicType<long, int32_t, LittleEndian> HLAinteger32LE;
typedef HLAbasicType<long long, int64_t, LittleEndian> HLAinteger64LE;
typedef HLAbasicType<float, float, LittleEndian> HLAfloat32LE;
typedef HLAbasicType<double, double, LittleEndian> HLAfloat64LE;
typedef HLAbasicType<wchar_t, wchar_t, LittleEndian> HLAoctetPairLE;

typedef HLAbasicType<char, char, BigEndian> HLAoctet;

/* Additional datatypes used by RPR-FOM
 */
typedef HLAbasicType<unsigned short, uint16_t, BigEndian> Unsignedinteger16BE;
typedef HLAbasicType<unsigned long, uint32_t, BigEndian> Unsignedinteger32BE;
typedef HLAbasicType<unsigned long, uint64_t, BigEndian> Unsignedinteger64BE;

//! HLA enumerated type, enumeration <E> with representation <R>.
/*!
 * HLAenumeratedType<ENUMERATION, REPRESENTATION>
 * defines an user-convenient ENUMERATION stored using given REPRESENTATION.
 *
 * The data can be accessed in an usual way.
 *
 * Some models may use one enumerated value in multiple enumerations. To avoid
 * name collisions it's recommended to put the ENUMERATION in an individual namespace.
 *
 * For example:
 * +------------+----------------+------------+--------+-----------+
 * | Name       | Representation | Enumerator | Values | Semantics |
 * +------------+----------------+------------+--------+-----------+
 * |            |                | HLAfalse   | 0      |           |
 * | HLAboolean | HLAinteger32BE +------------+--------+-----------+
 * |            |                | HLAfalse   | 1      |           |
 * +------------+----------------+------------+--------+-----------+
 * 
 * namespace __HLAboolean {
 * enum __enum {
 *   HLAfalse = 0,
 *   HLAtrue = 1
 * };
 * }
 * typedef HLAenumeratedType<__HLAboolean::__enum, HLAinteger32BE> HLAboolean;
 * HLAdata<HLAboolean> value;
 *
 * value = HLAtrue;
 */
template<class E, class R>
struct HLAenumeratedType
{
    HLAenumeratedType& operator = (const E& data)
    {
        *(R*)this = data;
        return *this;
    }

    HLAenumeratedType& operator = (const int& data)
    {
        *(R*)this = data;
        return *this;
    }

    operator E() const
    {
        int result = *(R*)this;
        return (E)result;
    }

    operator int() const
    { return *(R*)this; }

    static const size_t emptysizeof()
    { return R::emptysizeof(); }

    static const size_t __sizeof()
    { return R::__sizeof(); }

    void copy(void* source)
    { ((R*)this)->copy(source); }

    static const size_t m_octetBoundary = R::m_octetBoundary;
    static const bool m_isVariable = false;
};

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

/* Calculate the smallest nonnegative value of P that satisfies the following
 * formula: (offset+size+P) mod boundary = 0
 */
inline size_t __padding(size_t size, size_t boundary)
{ return boundary - ((size-1)%boundary + 1); }

//! Fixed array of <N> elements of type <M>
/*!
 * HLAfixedArray<DATATYPE, NUMBER>
 * defines a fixed array of NUMBER elements of type DATATYPE.
 *
 * The data can be accessed in an usual way.
 *
 * For example:
 * +-------------+----------------+-------------+-----------------+-----------+
 * | Name        | Element type   | Cardinality | Encoding        | Semantics |
 * +-------------+----------------+-------------+-----------------+-----------+
 * | Coordinates | HLAinteger32BE | 3           | HLAfixedArray   |           |
 * +-------------+----------------+-------------+-----------------+-----------+
 *
 * typedef HLAfixedArray<HLAinteger32BE,3> Coordinates;
 * HLAdata<Coordinates> value;
 *
 * (*value)[0] = 100;
 * (*value)[1] = 200;
 */
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

//! Variable array of type <M>
/*!
 * HLAvariableArray<DATATYPE>
 * defines an array of a variable number of DATATYPE elements.
 *
 * The size() member must be set before accessing the data. No data are moved
 * when the size() is changed.
 *
 * For example:
 * +-------------+----------------+-------------+-----------------+-----------+
 * | Name        | Element type   | Cardinality | Encoding        | Semantics |
 * +-------------+----------------+-------------+-----------------+-----------+
 * | List        | HLAinteger32BE | Dynamic     | HLAvaribleArray |           |
 * +-------------+----------------+-------------+-----------------+-----------+
 *
 * typedef HLAvariableArray<HLAinteger32BE> List;
 * HLAdata<List> value;
 *
 * (*value).set_size(2);
 * (*value)[0] = 100;
 * (*value)[1] = 200;
 */
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

//! Fixed record of fields <R>
/*!
 * HLAfixedRecord<
 *   HLAfixedField<INDEX1, DATATYPE1,
 *   HLAfixedField<INDEX2, DATATYPE2,
 *   ...
 *   > ... > TYPENAME;
 * defines an ordered sequence of DATATYPE entries.
 *
 * The data can be accessed using the field<INDEX>() function. The INDEX is a logical
 * identifier only. The data are stored in the declaration order.
 *
 * For example:
 * +-------------+------------------------------------+----------------+-----------+
 * |             | Field                              |                |           |
 * | Record name +---------+--------------+-----------+ Encoding       | Semantics |
 * |             | Name    | Type         | Semantics |                |           |
 * +-------------+---------+--------------+-----------+----------------+-----------+
 * |             | FIELD_X | HLAfloat32LE |           |                |           |
 * |             +---------+--------------+-----------+                |           |
 * | Coordinates | FIELD_X | HLAfloat32LE |           | HLAfixedRecord |           |
 * |             +---------+--------------+-----------+                |           |
 * |             | FIELD_X | HLAfloat32LE |           |                |           |
 * +-------------+---------+--------------+-----------+----------------+-----------+
 * 
 * enum {
 *   FIELD_X = 0,
 *   FIELD_Y,
 *   FIELD_Z
 * };
 * typedef HLAfixedRecord<
 *   HLAfixedField<FIELD_X, HLAfloat32LE,
 *   HLAfixedField<FIELD_Y, HLAfloat32LE,
 *   HLAfixedField<FIELD_Z, HLAfloat32LE
 *   > > > > Coordinates;
 * HLAdata<Coordinates> value;
 *
 * value->field<FIELD_X>() = 3.14;
 * value->field<FIELD_Y>() = 6.28;
 * value->field<FIELD_Z>() = 9.42;
 */
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

template<bool C, class T, class E>
struct __if
{ typedef T X; };

template<class T, class E>
struct __if<false, T, E>
{ typedef E X; };

template<int E, class M, class N, bool V, int d>
struct __FieldAt<HLAfixedField<E, M, N, V>, d>
{ typedef typename __if<d==E, M, typename __FieldAt<N,d>::Type>::X Type; }; 

// returned when no HLAfixedField has a given index(i)
template<int d>
struct __FieldAt<HLAfixedEnd, d>
{ typedef HLAfixedEnd Type; };

//! Variant record of discriminant <D> at index <E> and alternatives <R>
/*!
 * HLAvariantRecord<
 *   INDEX, DATATYPE,
 *   HLAvariantField<ENUMERATORS1, INDEX1, DATATYPE1,
 *   HLAvariantField<ENUMERATORS2, INDEX2, DATATYPE2,
 *   ...
 *   > ... > TYPENAME;
 * defines an ordered sequence of DATATYPE entries.
 *
 * The data can be accessed using the field<INDEX>() function. The INDEX is a logical
 * identifier only. 
 * The first field is a discriminant. It is followed by an alternative whose
 * ENUMERATORS match the discriminant value.
 *
 * For example:
 * +-------------+-------------------------------+------------------------------------+------------------+-----------+
 * |             | Discriminant                  | Alternative                        |                  |           |
 * | Record name +------+-----------+------------+---------+--------------+-----------+ Encoding         | Semantics |
 * |             | Name | Type      | Enumerator | Name    | Type         | Semantics |                  |           |
 * +-------------+------+-----------+------------+---------+--------------+-----------+------------------+-----------+
 * |             |      |           | AXIS_X     | FIELD_X | HLAfloat32LE |           |                  |           |
 * | Coordinates | TYPE | TypesEnum +------------+---------+--------------+-----------+ HLAvariantRecord |           |
 * |             |      |           | AXIS_Y     | FIELD_Y | HLAfloat32LE |           |                  |           |
 * +-------------+------+-----------+------------+---------+--------------+-----------+------------------+-----------+
 * 
 * namespace __Fields {
 * enum __enum {
 *   TYPE = 0,
 *   FIELD_X = 101,
 *   FIELD_Y = 102
 * };
 * }
 * typedef HLAenumeratedType<__Fields::__enum, HLAinteger32BE> Fields;
 * typedef HLAvariantRecord<
 *   __Fields::TYPE, TypesEnum,
 *   HLAvariantField<HLAsetValue<AXIS_X>, __Fields::FIELD_X, HLAfloat32LE,
 *   HLAvariantField<HLAsetValue<AXIS_Y>, __Fields::FIELD_Y, HLAfloat32LE
 *   > > > Coordinates;
 * HLAdata<Coordinates> value;
 *
 * value->set_discriminant(AXIS_X);
 * value->field<__Fields::FIELD_X>() = 3.14;
 */
template<int DE, class DM, class R, bool hasVariable = R::m_isVariable>
struct HLAvariantRecord;

template<int DE, class DM, class R, bool V>
std::ostream& PrintBuffer(std::ostream& stream, HLAvariantRecord<DE,DM,R,V>& buffer)
{ return __print_buffer(stream, (void*)&buffer, buffer.__sizeof()); }

//! Returns the discriminant, or a <i>th field of the HLAvariantField <R>
template<int DE, class DM, class R, int e>
struct __DiscriminantOrFieldAt;

// Variant record optimized for fixed-size fields
template<int DE, class DM, class R>
struct HLAvariantRecord<DE, DM, R, false>
{
    //! Get/Set the discriminant, without memory management
    /* This function assumes no data are stored in the buffer after this array.
     * You may want to use set_discriminant() instead.
     */
    DM& discriminant() const
    { return *(DM*)this; }

    //! Set the discriminant, with memory management
    void set_discriminant(int d)
    {
        if (d == (int)discriminant())
            return; // no change

        // as changing the discriminant may impact padding, the whole buffer needs to be reorganized
        __HLAbuffer::shake(this, d, (long)R::field_emptysizeof(d));
    }

    template <int e>
    typename __DiscriminantOrFieldAt<DE,DM,R,e>::Type& field() const
    {
        // return the discriminant
        if (e == DE)
            return *(typename __DiscriminantOrFieldAt<DE,DM,R,e>::Type*)this;

        if (e != R::get_field(discriminant()))
            throw std::out_of_range("HLAvariantRecord: wrong discriminant");
        // return one of the alternatives
        return *(typename __DiscriminantOrFieldAt<DE,DM,R,e>::Type*)((char*)this + emptysizeof());
    }

    static const size_t emptysizeof()
    { return DM::__sizeof() + __padding(DM::__sizeof(), R::m_octetBoundary); }

    const size_t __sizeof() const
    {
        if (R::has_field(discriminant()))
            return emptysizeof() + R::field_sizeof(discriminant());
        else
            return emptysizeof();
    }

    void copy(void* source)
    {
        __HLAbuffer& buffer = __HLAbuffer::__buffer(this);
        if(source == buffer.mShakeThat) {
            *(DM*)this = buffer.mShakeValue;
            // switching variant: content is removed
        }
        else {
            int d = *(DM*)source;
            // not switching variant: copy the content
            *(DM*)this = d;
            ((R*)this)->copy(d, (char*)source + emptysizeof());
        }
    }

    static const size_t m_octetBoundary = MAX(DM::m_octetBoundary, R::m_octetBoundary);
    static const bool m_isVariable = true; // variant record of fixed-size fields
};

// Generic variant record, supports variable-size fields
template<int DE, class DM, class R>
struct HLAvariantRecord<DE, DM, R, true>
{
    //! Get/Set the discriminant, without memory management
    /* This function assumes no data are stored in the buffer after this array.
     * You may want to use set_discriminant() instead.
     */
    DM& discriminant() const
    { return *(DM*)this; }

    //! Set the discriminant, with memory management
    void set_discriminant(int d)
    {
        if (d == (int)discriminant())
            return; // no change

        // as changing the discriminant may impact padding, the whole buffer needs to be reorganized
        __HLAbuffer::shake(this, d, (long)R::field_emptysizeof(d));
    }

    template <int e>
    typename __DiscriminantOrFieldAt<DE,DM,R,e>::Type& field() const
    {
        // return the discriminant
        if (e == DE)
            return *(typename __DiscriminantOrFieldAt<DE,DM,R,e>::Type*)this;

        if (e != R::get_field(discriminant()))
            throw std::out_of_range("HLAvariantRecord: wrong discriminant");
        // return one of the alternatives
        return *(typename __DiscriminantOrFieldAt<DE,DM,R,e>::Type*)((char*)this + emptysizeof());
    }

    static const size_t emptysizeof()
    { return DM::__sizeof() + __padding(DM::__sizeof(), R::m_octetBoundary); }

    const size_t __sizeof() const
    {
        if (R::has_field(discriminant()))
            return emptysizeof() + ((R*)((char*)this + emptysizeof()))->field_sizeof(discriminant());
        else
            return emptysizeof();
    }

    void copy(void* source)
    {
        __HLAbuffer& buffer = __HLAbuffer::__buffer(this);
        if(source == buffer.mShakeThat) {
            *(DM*)this = buffer.mShakeValue;
            // switching variant: content is removed
        }
        else {
            int d = *(DM*)source;
            // not switching variant: copy the content
            *(DM*)this = d;
            ((R*)this)->copy(d, (char*)source + emptysizeof());
        }
    }

    static const size_t m_octetBoundary = MAX(DM::m_octetBoundary, R::m_octetBoundary);
    static const bool m_isVariable = true; // variant record of variable-sized fields
};

struct HLAsetEnd;

//! Defines a value in an enumerator list; to be used with HLAvariantField
template<int e, class N = HLAsetEnd>
struct HLAsetValue
{
    static int includes(int i)
    {
        if (i == e)
            return 1;
        else
            return N::includes(i);
    }
};

//! Defines a range in an enumerator list
template<int e1, int e2, class N = HLAsetEnd>
struct HLAsetRange
{
    static int includes(int i)
    {
        if (e1 <= i && i <= e2)
            return 1;
        else
            return N::includes(i);
    }
};

//! Defines the "HLAother" symbol in an enumerator list
template<class N = HLAsetEnd>
struct HLAsetOther
{
    static int includes(int i)
    { return 1; }
};

//! Defines the end of an enumerator list
struct HLAsetEnd
{
    static int includes(int i)
    { return 0; }
};

struct HLAvariantEnd;

//! Record at index <E>, field of type <M>, followed by HLAvariantField <N>
// note: <E> must be "int" to enable implicit conversion from/to enum types
template<class D, int E, class M, class N = HLAvariantEnd, bool hasVariable = M::m_isVariable || N::m_isVariable>
struct HLAvariantField;

// List of fixed-size fields
template<class D, int E, class M, class N>
struct HLAvariantField<D, E, M, N, false>
{
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

    static const size_t field_emptysizeof(int e)
    {
        if (e == E)
            return M::emptysizeof();
        else
            return N::field_emptysizeof(e);
    }

    static const size_t field_sizeof(int e)
    {
        if (e == E)
            return M::__sizeof();
        else
            return N::field_sizeof(e);
    }

    void copy(int e, void* source)
    {
        if (e == E)
            return ((M*)this)->copy(source);
        else
            return ((N*)this)->copy(e, source);
    }

    static const size_t m_octetBoundary = MAX(M::m_octetBoundary, N::m_octetBoundary);
    static const bool m_isVariable = false; // fixed-sized fields
};

// List containg variable-size fields
template<class D, int E, class M, class N>
struct HLAvariantField<D, E, M, N, true>
{
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

    static const size_t field_emptysizeof(int e)
    {
        if (e == E)
            return M::emptysizeof();
        else
            return N::field_emptysizeof(e);
    }

    const size_t field_sizeof(int e) const
    {
        if (e == E)
            return ((M*)this)->__sizeof();
        else
            return ((N*)this)->field_sizeof(e);
    }

    void copy(int e, void* source)
    {
        if (e == E)
            return ((M*)this)->copy(source);
        else
            return ((N*)this)->copy(e, source);
    }

    static const size_t m_octetBoundary = MAX(M::m_octetBoundary, N::m_octetBoundary);
    static const bool m_isVariable = true; // variable-sized fields
};

//! Defines a last field in the fixed record
struct HLAvariantEnd
{
    static bool has_field(int d)
    { return false; }
    static int get_field(int d)
    { throw std::out_of_range("HLAvariantRecord: unknown discriminant"); }
    static const size_t field_emptysizeof(int e)
    { return 0; }
    static const size_t field_sizeof(int e)
    { return 0; }
    void copy(int e, void* source)
    { /* nop */ }

    static const size_t m_octetBoundary = 0;
    static const bool m_isVariable = false;
};

template<int DE, class DM, class R, int e>
struct __DiscriminantOrFieldAt
{ typedef typename __if<e==DE, DM, typename __FieldAt<R,e>::Type>::X Type; }; 

template<class D, int E, class M, class N, bool V, int e>
struct __FieldAt<HLAvariantField<D,E,M,N,V>, e>
{ typedef typename __if<e==E, M, typename __FieldAt<N,e>::Type>::X Type; }; 

// returned when no HLAvariantField has a given index(i)
template<int e>
struct __FieldAt<HLAvariantEnd, e>
{ typedef HLAvariantEnd Type; };

} // namespace libhla

#endif // _HLATYPES_IEEE1516_HH

// $Id: HLAtypesIEEE1516.hh,v 1.3 2008/08/02 11:44:12 gotthardp Exp $

