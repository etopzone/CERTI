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
// $Id: HLAtypesIEEE1516.hh,v 1.1 2008/07/15 10:54:11 gotthardp Exp $
// ----------------------------------------------------------------------------

#ifndef _HLATYPES_IEEE1516_HH
#define _HLATYPES_IEEE1516_HH

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
    // static buffer for all instantiations of the HLAbuffer template
    // indexed by pointers after the last element, the end() pointers
    typedef std::map<char*,__HLAbuffer*> BufferList;
    static BufferList gBuffers;

public:
    char *mBegin;
    size_t mCapacity;
    // no automatic free() and realloc() for user allocated memory
    bool mUserAllocated;

    __HLAbuffer(size_t capacity)
     : mUserAllocated(false)
    {
        // exponential growth: capacity *= 1.5
        mCapacity = (size_t)(capacity*1.5);
        mBegin = (char*)malloc(mCapacity);
        // store "this" to a global table
        gBuffers[mBegin + mCapacity] = this;
    }

    __HLAbuffer(void *begin, size_t capacity)
      : mBegin((char*)begin), mCapacity(capacity), mUserAllocated(true)
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

    static BufferList::iterator __buffer_iterator(const void* __this)
    {
        // find the first pointer not less than "this", what is the end() pointer
        BufferList::iterator result = gBuffers.lower_bound((char*)__this);
        if (result == gBuffers.end())
            throw std::runtime_error("HLAbuffer: bad pointer");
        return result;
    }

    static __HLAbuffer& __buffer(const void* __this)
    { return *(__buffer_iterator(__this)->second); }

    //! Resize the data buffer, shifts the data at [offset, __sizeof()] by "shift"
    void resize_buffer(size_t offset, long shift)
    {
        size_t buffer_size = size();

        if (shift > 0) {
            // check buffer overflow
            if (buffer_size+shift > mCapacity) {
                if (!mUserAllocated) {
                    // remove old "this" from the global table
                    gBuffers.erase(__buffer_iterator(mBegin));
                    // exponential growth: capacity *= 1.5
                    mCapacity = (size_t)((buffer_size+shift)*1.5);
                    mBegin = (char*)realloc(mBegin, mCapacity);
                    // store new "this" to a global table
                    gBuffers[mBegin + mCapacity] = this;
                }
                else
                    throw std::length_error("HLAbuffer: data buffer overflow");
            }

            char* ptr = (char*)mBegin + offset;
            memmove(ptr+shift, ptr, mBegin+buffer_size-ptr);
            // clean the buffer for new members
            memset(ptr, '\0', shift);
        }
        else if (shift < 0) {
            char* ptr = (char*)mBegin + offset;
            memmove(ptr+shift, ptr, mBegin+buffer_size-ptr);
        }
    }

    static void __check_memory(const void* __this, size_t size)
    {
        const __HLAbuffer& buffer = __buffer(__this);
        if ((char*)__this + size > (char*)buffer.mBegin + buffer.mCapacity)
            throw std::length_error("HLAbuffer: data buffer overflow");
    }

    virtual const size_t size() const = 0;

    const char* data() const
    { return mBegin; }

    std::ostream& print(std::ostream& stream)
    { return __print_buffer(stream, mBegin, size()); }
};

template<class T>
class HLAdata : public __HLAbuffer
{
public:
  //! Create new buffer
  HLAdata()
   : __HLAbuffer(T::empty_sizeof())
  {
      // clean the member data structures
      // unused bytes are not cleaned, mCapacity > T::empty_sizeof()
      memset(mBegin, '\0', T::empty_sizeof());
  }

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
        uint16_t result =
            (*(uint16_t*)&x<<8 | *(uint16_t*)&x>>8);
        return *(T*)&result;
    }
};

template<class T>
struct __swap<T,4>
{
    inline const T operator()(const T& x) const {
        uint32_t result =
            (*(uint32_t*)&x<<24 | *(uint32_t*)&x>>24 |
            (*(uint32_t*)&x & 0x0000ff00UL)<<8 |
            (*(uint32_t*)&x & 0x00ff0000UL)>>8);
        return *(T*)&result;
    }
};

template<class T>
struct __swap<T,8>
{
    inline const T operator()(const T& x) const {
        uint64_t result =
            (*(uint64_t*)&x<<56 | *(uint64_t*)&x>>56 |
            (*(uint64_t*)&x & 0x000000000000ff00ULL)<<40 |
            (*(uint64_t*)&x & 0x0000000000ff0000ULL)<<24 |
            (*(uint64_t*)&x & 0x00000000ff000000ULL)<< 8 |
            (*(uint64_t*)&x & 0x000000ff00000000ULL)>> 8 |
            (*(uint64_t*)&x & 0x0000ff0000000000ULL)>>24 |
            (*(uint64_t*)&x & 0x00ff000000000000ULL)>>40);
        return *(T*)&result;
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
 * HLAbuffer<HLAinteger32BE> value;
 *
 * value = 42;
 */
template<class T, class S, template<class T>class E>
struct HLAbasicType
{
    HLAbasicType& operator = (const T& data)
    {
        __HLAbuffer::__check_memory(this, __sizeof());
        *(S*)this = E<S>()(data);
        return *this;
    }

    operator T() const
    {
        __HLAbuffer::__check_memory(this, __sizeof());
        return E<S>()(*(S*)this);
    }

    static const size_t empty_sizeof()
    { return __sizeof(); }

    static const size_t __sizeof()
    { return sizeof(S); }

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

/* IEEE 1516.2, Table 25:
 * Simple datatype table
 */
typedef HLAoctet HLAASCIIchar;
typedef HLAoctetPairBE HLAunicodeChar;
typedef HLAoctet HLAbyte;

/* IEEE 1516.2, Table 27:
 * Enumeration datatype table
 */
enum __HLAboolean
{
    HLAfalse = 0,
    HLAtrue = 1
};
// As 64bit architectures may have 64bit enum, we need static-cast to uint32_t.
typedef HLAbasicType<__HLAboolean, uint32_t, BigEndian> HLAboolean;

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
 * typedef HLAfixedArray<HLAinteger32BE,3> Coordinates;
 * HLAbuffer<Coordinates> value;
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

    static const size_t empty_sizeof()
    { return __sizeof(); }

    // Padding shall not be added after the last element of the array.
    static const size_t __sizeof()
    { return offset(N-1) + M::__sizeof(); }

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

    static const size_t empty_sizeof()
    { return N*M::empty_sizeof(); }

    // Padding shall not be added after the last element of the array.
    const size_t __sizeof() const
    {
        size_t offs = offset(N-1);
        return offs + ((M*)((char*)this + offs))->__sizeof();
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
 * typedef HLAvariableArray<HLAinteger32BE> List;
 * HLAbuffer<List> value;
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

    //! Set array size, with memory management (identical for both HLAvariableArray)
    void set_size(long i)
    {
        __HLAbuffer& buffer = __HLAbuffer::__buffer(this);
        // the data following this structure that need to be shifted
        size_t offset = (char*)this - buffer.mBegin;
        size_t my_sizeof = __sizeof();

        const void* result = this;
        if (i > size()) { // enlarge
            buffer.resize_buffer(offset+my_sizeof, (i - (long)size())*(long)M::empty_sizeof());
            result = buffer.mBegin+offset;
            // resize_buffer may invoke realloc(), so "this" pointer changes
            *(HLAinteger32BE*)result = i;
        }
        else if (i < size()) { // shrink
            size() = i;
            buffer.resize_buffer(offset+my_sizeof, (long)__sizeof()-(long)my_sizeof);
            result = buffer.mBegin+offset;
        }
    }

    static const size_t offset(long i)
    { return empty_sizeof() + i*(M::__sizeof() + __padding(M::__sizeof(), M::m_octetBoundary)); }

    M& operator[](long i) const
    {
        if (i >= size())
            throw std::out_of_range("HLAvariableArray: index out of range");
        return *(M*)((char*)this + offset(i));
    }

    static const size_t empty_sizeof()
    { return HLAinteger32BE::__sizeof() + __padding(HLAinteger32BE::__sizeof(), M::m_octetBoundary); }

    // Padding shall not be added after the last element of the array.
    const size_t __sizeof() const
    {
        if (size() > 0)
            return offset(size()-1) + M::__sizeof();
        else
            return empty_sizeof();
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

    //! Set array size, with memory management (identical for both HLAvariableArray)
    void set_size(long i)
    {
        __HLAbuffer& buffer = __HLAbuffer::__buffer(this);
        // the data following this structure that need to be shifted
        size_t offset = (char*)this - buffer.mBegin;
        size_t my_sizeof = __sizeof();

        const void* result = this;
        if (i > size()) { // enlarge
            buffer.resize_buffer(offset+my_sizeof, (i - (long)size())*(long)M::empty_sizeof());
            result = buffer.mBegin+offset;
            // resize_buffer may invoke realloc(), so "this" pointer changes
            *(HLAinteger32BE*)result = i;
        }
        else if (i < size()) { // shrink
            size() = i;
            buffer.resize_buffer(offset+my_sizeof, (long)__sizeof()-(long)my_sizeof);
            result = buffer.mBegin+offset;
        }
    }

    const size_t offset(long i) const
    {
        size_t offs = empty_sizeof();
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

    static const size_t empty_sizeof()
    { return HLAinteger32BE::__sizeof() + __padding(HLAinteger32BE::__sizeof(), M::m_octetBoundary); }

    // Padding shall not be added after the last element of the array.
    const size_t __sizeof() const
    {
        if (size() > 0) {
            size_t offs = offset(size()-1);
            return offs + ((M*)((char*)this + offs))->__sizeof();
        }
        else
            return empty_sizeof();
    }

    static const size_t m_octetBoundary =
        MAX(HLAinteger32BE::m_octetBoundary, M::m_octetBoundary);
    static const bool m_isVariable = true; // variable-size array of variable-size elements
};

//! Fixed record field of type <M>, followed by HLAfixedRecord <N>
/*!
 * HLAfixedRecord<DATATYPE,
 *   HLAfixedRecord<DATATYPE,
 *   ...,
 *   HLAfixedRecordEnd> ... > TYPENAME;
 * defines an ordered sequence of DATATYPE entries.
 *
 * The data can be accessed using the field<INDEX>() function, where the INDEX
 * is a zero-based order of the field being accessed.
 *
 * For example:
 * typedef
 * #define FIELD_X 0
 *   HLAfixedRecord<HLAfloat23LE,
 * #define FIELD_Y 1
 *   HLAfixedRecord<HLAfloat32LE,
 * #define FIELD_Z 2
 *   HLAfixedRecord<HLAfloat32LE,
 *   HLAfixedRecordEnd> > > Coordinates;
 * HLAbuffer<Coordinates> value;
 *
 * (*value).field<FIELD_X>() = 3.14;
 * (*value).field<FIELD_Y>() = 6.28;
 * (*value).field<FIELD_Z>() = 9.42;
 */
template<class M, class N, bool hasVariable = M::m_isVariable || N::m_isVariable>
struct HLAfixedRecord;

template<class M, class N, bool V>
std::ostream& PrintBuffer(std::ostream& stream, HLAfixedRecord<M,N,V>& buffer)
{ return __print_buffer(stream, (void*)&buffer, buffer.__sizeof()); }

//! Returns <i>th field of the HLAfixedRecord <L>
template<class R, unsigned i> struct __FieldAt;

// Fixed record optimized for fixed-size fields
template<class M, class N>
struct HLAfixedRecord<M, N, false>
{
    static const size_t offset(size_t i, size_t offs = 0)
    {
        if (i > 0) {
            size_t size = M::__sizeof();
            size += __padding(offs+size, N::memberBoundary);
            return N::offset(i-1, offs+size);
        }
        else
            return offs;
    }

    static const size_t memberBoundary = M::m_octetBoundary;

    template <unsigned i>
    typename __FieldAt<HLAfixedRecord<M,N>,i>::Type& field() const
    { return *(typename __FieldAt<HLAfixedRecord<M,N>,i>::Type*)((char*)this + offset(i)); }

    static const size_t empty_sizeof()
    { return __sizeof(); }

    // Padding shall not be added after the last element of the array.
    static const size_t __sizeof(size_t offs = 0)
    {
        size_t size = M::__sizeof();
        // if not reached HLAfixedRecordEnd
        if (N::memberBoundary) {
            size += __padding(offs+size, N::memberBoundary);
            return N::__sizeof(offs+size);
        }
        else
            return offs+size;
    }

    static const size_t m_octetBoundary = MAX(M::m_octetBoundary, N::m_octetBoundary);
    static const bool m_isVariable = false; // fixed-sized fields
};

// Generic fixed record, supports variable-size fields
template<class M, class N>
struct HLAfixedRecord<M, N, true>
{
    const size_t offset(size_t i, size_t offs = 0) const
    {
        if (i > 0) {
            size_t size = ((M*)this)->__sizeof();
            size += __padding(offs+size, N::memberBoundary);
            return ((N*)((char*)this + size))->offset(i-1, offs+size);
        }
        else
            return offs;
    }

    static const size_t memberBoundary = M::m_octetBoundary;

    template <unsigned i>
    typename __FieldAt<HLAfixedRecord<M,N,true>,i>::Type& field() const
    { return *(typename __FieldAt<HLAfixedRecord<M,N,true>,i>::Type*)((char*)this + offset(i)); }

    static const size_t empty_sizeof(size_t offs = 0)
    {
        size_t size = M::empty_sizeof();
        // if not reached HLAfixedRecordEnd
        if (N::memberBoundary) {
            size += __padding(offs+size, N::memberBoundary);
            return N::empty_sizeof(offs+size);
        }
        else
            return offs+size;
    }

    // Padding shall not be added after the last element of the array.
    const size_t __sizeof(size_t offs = 0) const
    {
      size_t size = ((M*)this)->__sizeof();
      // if not reached HLAfixedRecordEnd
      if (N::memberBoundary) {
          size += __padding(offs+size, N::memberBoundary);
          return ((N*)((char*)this + size))->__sizeof(offs+size);
      }
      else
          return offs+size;
    }

    static const size_t m_octetBoundary = MAX(M::m_octetBoundary, N::m_octetBoundary);
    static const bool m_isVariable = true; // variable-sized fields
};

//! Defines a last field in the fixed record
struct HLAfixedRecordEnd
{
    static const size_t offset(size_t i, size_t offs = 0)
    { return offs; }

    static const size_t memberBoundary = 0;
    static const size_t empty_sizeof(size_t offs = 0)
    { return offs; }
    static const size_t __sizeof(size_t offs = 0)
    { return offs; }

    static const size_t m_octetBoundary = 0;
    static const bool m_isVariable = false;
};

template<class M, class N, bool V, unsigned i>
struct __FieldAt<HLAfixedRecord<M, N, V>,i>
{ typedef typename __FieldAt<N,i-1>::Type Type; };

template <class M, class N, bool V>
struct __FieldAt<HLAfixedRecord<M, N, V>,0>
{ typedef M Type; };

} // namespace libhla

#endif // _HLATYPES_IEEE1516_HH

// $Id: HLAtypesIEEE1516.hh,v 1.1 2008/07/15 10:54:11 gotthardp Exp $

