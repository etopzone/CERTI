// ----------------------------------------------------------------------------
// HLAbuffer.cc - IEEE 1516.2 compliant datatypes
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
// $Id: HLAbuffer.cc,v 1.2 2008/11/19 10:25:03 gotthardp Exp $
// ----------------------------------------------------------------------------

#include "HLAbuffer.hh"

#include <algorithm>
#include <iomanip>
#include <iostream>

// #define HLATYPES_IEEE1516_DISPLAYPRINTABLE

namespace libhla {

__HLAbuffer::BufferList __HLAbuffer::gBuffers;

bool __HLAbuffer::__is_big_endian()
{
#ifdef HOST_IS_BIG_ENDIAN
    return true;
#else
    return false;
#endif
}

bool __HLAbuffer::__is_little_endian()
{
#ifdef HOST_IS_BIG_ENDIAN
    return false;
#else
    return true;
#endif
}

//! Print the physical data buffer (for debugging purposes only)
std::ostream& __print_buffer(std::ostream& stream, const void* buffer, size_t length)
{
    static const size_t cBytesPerLine = 16;
    size_t offset = 0;

    while (length != 0) {
        stream << std::hex << std::setfill('0') << std::setw(4) << (unsigned) offset << ": ";

        size_t i = std::min(cBytesPerLine, length);
        const unsigned char* p = (const unsigned char*) buffer;

        for (size_t j = i; j > 0; j--)
            stream << " " << std::hex << std::setfill('0') << std::setw(2) << (unsigned) *p++;

#ifdef HLATYPES_IEEE1516_DISPLAYPRINTABLE
        for (size_t j = cBytesPerLine - i; j > 0; j--)
            stream << "   ";

        stream << "   ";

        p = (const unsigned char*) buffer;
        for (size_t j = i; j > 0; j--) {
            char ch = *p++;
            stream << (isprint(ch) ? ch : ' ');
        }
#endif
        stream << std::endl;
        buffer = (const unsigned char*) buffer + i;
        length -= i;
        offset += i;
    }

    return stream;
}

} // namespace libhla

// $Id: HLAbuffer.cc,v 1.2 2008/11/19 10:25:03 gotthardp Exp $
