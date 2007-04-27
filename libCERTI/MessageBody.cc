// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005 ONERA
//
// This program is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: MessageBody.cc,v 3.10 2007/04/27 16:24:50 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "MessageBody.hh"
#include <cassert>

namespace certi {

// ----------------------------------------------------------------------------
/** Constructor. Created MessageBody is empty, buffer size 0.
 */
MessageBody::MessageBody()
{
    buffer.reserve(BUFFER_SIZE_DEFAULT);
    getPtr = buffer.begin();
}

/** Constructor, with an initial buffer size.
    @param n Buffer size
*/
MessageBody::MessageBody(size_t n)
{
    buffer.resize(n);
    getPtr = buffer.begin();
}

// ----------------------------------------------------------------------------
/** Get the size of the buffer.
    @return buffer size
*/
size_t
MessageBody::size() const
{
    return buffer.size();
}

// ----------------------------------------------------------------------------
/** Get buffer pointer.
    \return buffer pointer. 
*/
const unsigned char *
MessageBody::getBuffer() const
{

  return &(buffer[0]);

//   if (buffer.size() >0 ) {
//     return &(buffer[0]);
//   } else {
//     return NULL;
//   }
}

unsigned char *
MessageBody::getBufferRW()
{
  if (buffer.size() >0 ) {
    return &(buffer[0]);
  } else {
    return NULL;
  }
}

// ----------------------------------------------------------------------------
/** Read a string from the body, and put it in a buffer.
    @pre store is not null
    @param store Output buffer
    @param store_length Maximal string length (+1 gives minimal store size)
*/
void
MessageBody::readString(char *store, unsigned short store_length)
{
    unsigned short length = readShortInt();

    if (length > store_length)
        throw RTIinternalError("String in Message too long for storage.");

    if (length > 0)
        sgetn(store, length);

    store[length] = 0 ;
}

// ----------------------------------------------------------------------------
/** Write a string. If the string is empty (or NULL), an empty string
    is written onto the stream.
    @param s String to copy
*/
void 
MessageBody::writeString(const char *s)
{
    if (s) {
	unsigned short length = std::strlen(s);
	writeShortInt(length);
	if (length > 0)
	    sputn((char *) s, length);
    }
    else 
	writeShortInt(0);
}

// ----------------------------------------------------------------------------
MessageBody &
MessageBody::operator<<(unsigned long val)
{
    assert(val <= 4294967295U);
    for (int c = 3 ; c >= 0 ; --c) {
	buffer.push_back((val >> c * 8) & 0xFF);
    }
    return *this ;
}

// ----------------------------------------------------------------------------
const MessageBody &
MessageBody::operator>>(unsigned long &val) const
{
    val = 0 ;
    for (int i = 0 ; i < 4 ; ++i, ++getPtr) {
	val |= static_cast<unsigned long>(*getPtr) << (3 - i) * 8 ;
    }
    return *this ;
}

// ----------------------------------------------------------------------------
MessageBody &
MessageBody::operator<<(unsigned short val)
{
    for (int c = 1 ; c >= 0 ; --c) {
	buffer.push_back((val >> c * 8) & 0xFF);
    }
    return *this ;
}

// ----------------------------------------------------------------------------
const MessageBody &
MessageBody::operator>>(unsigned short &val) const
{
    val = 0 ;
    for (int i = 0 ; i < 2 ; ++i, ++getPtr) {
	val |= static_cast<unsigned short>(*getPtr) << (1 - i) * 8 ;
    }
    return *this ;
}

} // certi

// $Id: MessageBody.cc,v 3.10 2007/04/27 16:24:50 erk Exp $
