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
// $Id: MessageBody.cc,v 3.4 2003/06/27 17:26:29 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "MessageBody.hh"

// Comment this out if you don't want to use Integer encoding in stream.
// #define USE_INT_ENCODING

namespace certi {

// ----------------------------------------------------------------------------
/*! To create a WRITE body, with its own buffer(it will be deleted by the
  destructor). No Read operations should be made.
*/
MessageBody::MessageBody()
{
    Length = 0 ;
    GetPtr = Buffer ;

    if ((sizeof(unsigned short) != 2) || (sizeof(unsigned long) != 4))
        throw RTIinternalError("MessageBody unabled to process ints.");
}

// ----------------------------------------------------------------------------
//! Destructor.
MessageBody::~MessageBody()
{
}

// ----------------------------------------------------------------------------
//! getLength.
/*! Body must have been frozen.
 */
long MessageBody::getLength()
{
    return Length ;
}

// ----------------------------------------------------------------------------
//! getBuffer.
/*! Body must have been frozen.
 */
char *MessageBody::getBuffer()
{
    return Buffer ;
}

// ----------------------------------------------------------------------------
/*! Retrieve a string from the Body, and put it in Store. Store is at least
  (StoreLen + 1) bytes long. Store must be NOT NULL.
*/
void MessageBody::readString(char *Store, unsigned short StoreLen)
{
    // Read String's Length
    unsigned short StrLength = readShortInt();

    // Is the Store String long enough?
    if (StrLength > StoreLen)
        throw RTIinternalError("String in Message too long for storage.");

    // Get string from stream
    if (StrLength > 0)
        sgetn(Store, StrLength);

    Store[StrLength] = '\0' ;
}

// ----------------------------------------------------------------------------
/*! If the string is empty(or NULL), an empty string is written onto the
  stream.
*/
void MessageBody::writeString(const char *String)
{
    unsigned short StrLength ;

    // NULL String is handled like an empty string
    if (String == NULL) {
        writeShortInt(0);
        return ;
    }

    // Write string length
    StrLength = std::strlen(String);

    // BUG: Should test string's length.
    //if (StrLength > MAX_BYTES_PER_VALUE)
    // throw RTIinternalError("String too long.");

    writeShortInt(StrLength);

    // WriteString
    if (StrLength > 0) {
        sputn((char *)String, StrLength);
    }
}

}

// $Id: MessageBody.cc,v 3.4 2003/06/27 17:26:29 breholee Exp $
