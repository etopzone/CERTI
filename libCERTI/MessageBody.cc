// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libcerti
//
// CERTI-libcerti is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libcerti is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: MessageBody.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "MessageBody.hh"

// Comment this out if you don't want to use Integer encoding in stream.
// #define USE_INT_ENCODING

namespace certi {

// ------------------
// -- MessageBody --
// ------------------

MessageBody::MessageBody()
{
  Length     = 0;

  GetPtr     = Buffer;

  if((sizeof(unsigned short) != 2) ||(sizeof(unsigned long) != 4))
    throw RTIinternalError("MessageBody unabled to process ints.");
}


// -------------------
// -- ~MessageBody --
// -------------------

MessageBody::~MessageBody()
{
}


// ---------------
// -- GetLength --
// ---------------

long MessageBody::getLength()
{
  return Length;
}


// ---------------
// -- GetBuffer --
// ---------------

char *MessageBody::getBuffer()
{
  return Buffer;
}


// ----------------
// -- ReadString --
// ----------------

void MessageBody::readString(char *Store, unsigned short StoreLen)
{
  // Read String's Length
  unsigned short StrLength = readShortInt();

  // Is the Store String long enough?
  if(StrLength > StoreLen)
    throw RTIinternalError("String in Message too long for storage.");

  // Get string from stream
  if(StrLength > 0)
    sgetn(Store, StrLength);

  Store [StrLength] = '\0';
}


// -----------------
// -- WriteString --
// -----------------

void MessageBody::writeString(const char *String)
{
  unsigned short StrLength;

  // NULL String is handled like an empty string
  if(String == NULL) {
    writeShortInt(0);
    return;
  }

  // Write string length
  StrLength = std::strlen(String);

  // BUG: Should test string's length.
  //if(StrLength > MAX_BYTES_PER_VALUE)
  //  throw RTIinternalError("String too long.");

  writeShortInt(StrLength);

  // WriteString
  if(StrLength > 0) {
    sputn((char *)String, StrLength);
  }
}

}

// $Id: MessageBody.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
