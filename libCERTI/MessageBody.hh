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
// $Id: MessageBody.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_MESSAGE_BODY_HH
#define _CERTI_MESSAGE_BODY_HH

#include <cstring>
#include "network.hh"
#include "RTItypes.hh"

namespace certi {

// -----------
// -- Corps --
// -----------

// La classe Body est en fait un buffer de caracteres, avec des
// methodes permettant d'y ecrire et d'y lire des entiers(courts et
// longs) et des chaines de caracteres.
//
// Elle est basee sur la classe systeme strstreambuf, qui est un
// flux de caracteres base sur des chaines en memoire. Pour plus
// d'info man ssbuf(3C++).

// This class works like all streams, on a FIFO basis. For example,
// you call writeLongInt(myFirstInt), then writeString(myString),
// and then retrieve your data by calling(IN THE SAME ORDER)
// ReadLongInt and ReadString. If you don't respect the same order,
// results are inpredictables.

// Here is how to use this class:
//
// 1- Create an empty Body: MessageBody *myBody = new MessageBody();
// 
// 2- Put your thing in the Body:
// myBody->writeString("Hello");
// myBody->writeShortInt(1);
//
// 3- Freeze the Body in order to send it: myBody->Freeze();
//
// 4- Send it to the other side, and then destroy your Body:
// Send(myBody->getBuffer(), myBody->getLength());
// delete myBody;
//
// 5- When received on the other side, create a new Body :
// MessageBody *myNewBody = new MessageBody(Buffer, Length);
//
// 6- Retrieve your Data in the same Order :
// myString = myNewBody->readString();
// myInt = myNewBody->readShortInt();
// 
// 7- Delete your Body(don't forget to deallocate the retrieved string also):
// delete myNewBody; 

// There is no overflow checking, so this value would better be big enough.

#define BODY_BUFFER_MAX_SIZE 200

class MessageBody // : private strstreambuf
{
public:

  // -----------------------------------
  // -- Read/Write Methods for Blocks --
  // -----------------------------------
 
  inline void readBlock(char *Block, unsigned short Size)
  { sgetn(Block, Size); };

  // You can use this method to include the header at the beginning
  // of the body, in order to make a single Socket->Send call.
  inline void writeBlock(char *Block, unsigned short Size)
  { sputn(Block, Size); };

  // ----------------------------------------
  // -- Read and Write Methods for Integer --
  // ----------------------------------------
 
  inline unsigned short readShortInt() { 
    unsigned short Result;
    sgetn((char *) &Result, 2);
    return Result; 
  };

  inline void writeShortInt(unsigned short C) { 
    sputn((char *) &C, 2); 
  };

  inline unsigned long readLongInt() { 
    unsigned long Result;
    sgetn((char *) &Result, 4);
    return Result; 
  };

  inline void writeLongInt(unsigned long C) { 
    sputn((char *) &C, 4); 
  };

  // ----------------------------------------
  // -- Read and Write Methods for Strings --
  // ----------------------------------------

  // Retrieve a string from the Body, and put it in Store. Store
  // is at least(StoreLen + 1) bytes long. Store must be NOT NULL
  void readString(char *Store, unsigned short StoreLen);

  // If the string is empty(or NULL), an empty string 
  // is written onto the stream.
  void writeString(const char *String);

  // -----------------------------
  // -- General Purpose Methods --
  // -----------------------------

  // To create a WRITE body, with its own buffer(it will be deleted by the
  // destructor). No Read operations should be made.
  MessageBody();

  // To create a READ body, with a buffer of yours(it will NOT be deleted
  // by the destructor). No Write operations should be made. 
  MessageBody(char *InitBuffer, int InitLength);
  ~MessageBody();

  long getLength(); // Body must have been frozen.
  char *getBuffer(); // Body must have been frozen.

private:

  // ------------------------
  // -- Private Attributes --
  // ------------------------
  char Buffer[BODY_BUFFER_MAX_SIZE];
  unsigned long Length;
  char *GetPtr;
  // There is no PutPtr, because it computed with Buffer + Length.

  // ---------------------
  // -- Private Methods --
  // ---------------------

  inline void sputn(char *Ptr, unsigned short Size) { 
    std::memcpy(Buffer + Length, Ptr, Size);
    Length += Size; 
  };

  inline void sgetn(char *Ptr, unsigned short Size) { 
    std::memcpy(Ptr, GetPtr, Size);
    GetPtr += Size; 
  };
}; 
}

#endif // _CERTI_MESSAGE_BODY_HH

// $Id: MessageBody.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
