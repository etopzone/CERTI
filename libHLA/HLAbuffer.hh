// ----------------------------------------------------------------------------
// HLAbuffer.hh - IEEE 1516.2 compliant datatypes
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
// $Id: HLAbuffer.hh,v 1.11 2010/03/21 17:10:10 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _HLATYPES_BUFFER_HH
#define _HLATYPES_BUFFER_HH

#include <cstdlib>
#include <map>
#include <stdexcept>
#include <utility>

#include "libhla.hh"

namespace libhla {

HLA_EXPORT std::ostream& __print_buffer(std::ostream& stream, const void* buffer, size_t length);

/* Caution:
 * This implementation assumes that there are no compiler-generated data in
 * the heap memory allocated for the __HLAbuffer struct.
 * All structures must have no virtual functions and no non-static members.
 */

class HLA_EXPORT __HLAbuffer {
private:
    // static buffer for all instantiations of the HLAdata template
    // indexed by "last pointers", i.e. pointers to the last byte in the buffer
    typedef std::map<char*, __HLAbuffer*> BufferList;
    static BufferList gBuffers;

    // used to verify that user set correct endianess
    static bool __is_big_endian();
    static bool __is_little_endian();

    void __assert_endianess()
    {
#ifdef HOST_IS_BIG_ENDIAN
        if (!__is_big_endian())
            throw std::runtime_error("compile without -DHOST_IS_BIG_ENDIAN");
#else
        if (!__is_little_endian())
            throw std::runtime_error("compile with -DHOST_IS_BIG_ENDIAN");
#endif
    }

public:
    char* mBegin;
    size_t mCapacity;
    // no automatic free() and realloc() for user allocated memory
    bool mUserAllocated;
    // parameters used during dynamic resizing
    const void* mShakeThat;
    int mShakeValue;

    __HLAbuffer(size_t capacity) : mUserAllocated(false), mShakeThat(NULL)
    {
        __assert_endianess();
        // exponential growth: capacity *= 1.5
        mCapacity = (size_t)(capacity * 1.5);
        mBegin = (char*) calloc(1, mCapacity);
        // store "this" to a global table
        gBuffers[mBegin + mCapacity - 1] = this;
    }

    __HLAbuffer(void* begin, size_t capacity)
        : mBegin((char*) begin), mCapacity(capacity), mUserAllocated(true), mShakeThat(NULL)
    {
        __assert_endianess();
        // store "this" to a global table
        gBuffers[mBegin + mCapacity - 1] = this;
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
        gBuffers[mBegin + mCapacity - 1] = this; // update

        newBuffer.mBegin = oldBegin;
        newBuffer.mCapacity = oldCapacity;
        gBuffers[oldBegin + oldCapacity - 1] = &newBuffer; // update
    }

    static BufferList::iterator __buffer_iterator(const void* __this)
    {
        // find the first pointer not less than "this", the last pointer
        BufferList::iterator result = gBuffers.lower_bound((char*) __this);
        if (result == gBuffers.end())
            throw std::runtime_error("HLAdata: bad pointer");
        return result;
    }

    static __HLAbuffer& __buffer(const void* __this)
    {
        return *(__buffer_iterator(__this)->second);
    }

#ifndef NDEBUG
    static void __check_memory(const void* __this, size_t size)
    {
        const __HLAbuffer& buffer = __buffer(__this);
        if ((char*) __this + size > (char*) buffer.mBegin + buffer.mCapacity)
            throw std::length_error("HLAdata: data buffer overflow");
    }
#endif

    virtual size_t size() const = 0;
    virtual void __shake(const void* __that, int value, long resize) = 0;

    static void shake(const void* __that, int value, long resize)
    {
        __buffer(__that).__shake(__that, value, resize);
    }

    const char* data() const
    {
        return mBegin;
    }

    std::ostream& print(std::ostream& stream)
    {
#ifndef NDEBUG
        __check_memory(mBegin, size());
#endif
        return __print_buffer(stream, mBegin, size());
    }
};

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

/* Calculate the smallest nonnegative value of P that satisfies the following
 * formula: (offset+size+P) mod boundary = 0
 */
inline size_t __padding(size_t size, size_t boundary)
{
    return boundary - ((size - 1) % boundary + 1);
}

} // namespace libhla

#endif // _HLATYPES_BUFFER_HH

// $Id: HLAbuffer.hh,v 1.11 2010/03/21 17:10:10 erk Exp $
