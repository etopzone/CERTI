// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: MessageBody.cc,v 3.6 2004/01/09 16:07:29 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "MessageBody.hh"

namespace certi {

/** Constructor. Created MessageBody is empty, buffer size 0.
 */
MessageBody::MessageBody()
{
    buffer.reserve(200);
    getPtr = buffer.begin();

    if ((sizeof(unsigned short) != 2) || (sizeof(unsigned long) != 4))
        throw RTIinternalError("MessageBody unabled to process ints.");
}

/** Constructor, with an initial buffer size.
    @param n Buffer size
*/
MessageBody::MessageBody(size_t n)
{
    buffer.resize(n);
    getPtr = buffer.begin();

    if ((sizeof(unsigned short) != 2) || (sizeof(unsigned long) != 4))
        throw RTIinternalError("MessageBody unabled to process ints.");
}

/** Destructor.
 */
MessageBody::~MessageBody() { }

/** Get the size of the buffer.
    \return buffer size
*/
size_t
MessageBody::getLength() const
{
    return buffer.size();
}

/** Get buffer pointer.
    \return buffer pointer. 
*/
const char *
MessageBody::getBuffer() const
{
    return &(buffer[0]);
}

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

} // certi

// $Id: MessageBody.cc,v 3.6 2004/01/09 16:07:29 breholee Exp $
