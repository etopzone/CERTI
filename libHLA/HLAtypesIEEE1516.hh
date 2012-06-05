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
// $Id: HLAtypesIEEE1516.hh,v 1.6 2012/06/05 15:33:18 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _HLATYPES_IEEE1516_HH
#define _HLATYPES_IEEE1516_HH

/**
 * @page certi_HLAtypesIEEE1516 IEEE 1516.2 Data Encoding Functions
 *
 * These templates implement efficient access functions that provide direct
 * access to IEEE 1516.2 compliant data buffers. The functions are RTI
 * independent and thus compatible with any RTI.
 * The data are manipulated "in situ", no temporary variables are created.
 *
 * Use
\verbatim
 hlaomtdif2cpp -i <file>
\endverbatim
 * to generate FOM/SOM specific datatypes.
 *
 * The following templates are provided:
 * <ul>
 *  <li>\ref certi_HLAbasicType</li>
 *  <li>\ref certi_HLAenumeratedType</li>
 *  <li>\ref certi_HLAfixedArray</li>
 *  <li>\ref certi_HLAfixedRecord</li>
 *  <li>\ref certi_HLAvariableArray</li>
 *  <li>\ref certi_HLAvariantRecord</li>
 * </ul>
 *
 * The extensive use of template metaprogramming allows many operations to
 * be pre-calculated during compile-time.
 * See http://aszt.inf.elte.hu/~gsd/halado_cpp/ch06s09.html
 */

#include <HLAbuffer.hh>
#include <cstring>

namespace libhla {

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

} // namespace libhla

#if defined(_WIN32)
#pragma warning(disable:4503) // suppress warning C4503: decorated name length exceeded
#endif

#include <HLAbasicType.hh>
#include <HLAenumeratedType.hh>
#include <HLAfixedArray.hh>
#include <HLAvariableArray.hh>
#include <HLAfixedRecord.hh>
#include <HLAvariantRecord.hh>

#endif // _HLATYPES_IEEE1516_HH

// $Id: HLAtypesIEEE1516.hh,v 1.6 2012/06/05 15:33:18 erk Exp $

