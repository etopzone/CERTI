// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
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
// $Id: MessageBody.hh,v 3.3 2003/02/17 16:00:06 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_MESSAGE_BODY_HH
#define _CERTI_MESSAGE_BODY_HH

#include <config.h>

#include <cstring>
//#include "network.hh"
#include "RTItypes.hh"

namespace certi {

//! There is no overflow checking, so this value would better be big enough.
#define BODY_BUFFER_MAX_SIZE 200

/*! La classe Body est en fait un buffer de caracteres, avec des methodes
 permettant d'y ecrire et d'y lire des entiers(courts et longs) et des
 chaines de caracteres.

 Elle est basee sur la classe systeme strstreambuf, qui est un flux de
 caracteres base sur des chaines en memoire. Pour plus d'info man
 ssbuf(3C++).

 This class works like all streams, on a FIFO basis. For example, you call
 writeLongInt(myFirstInt), then writeString(myString), and then retrieve
 your data by calling(IN THE SAME ORDER) ReadLongInt and ReadString. If you
 don't respect the same order, results are inpredictables.

 Here is how to use this class:

 -# Create an empty Body: MessageBody *myBody = new MessageBody();

 -# Put your thing in the Body:
 myBody->writeString("Hello");
 myBody->writeShortInt(1);

 -# Freeze the Body in order to send it: myBody->Freeze();

 -# Send it to the other side, and then destroy your Body:
 Send(myBody->getBuffer(), myBody->getLength());
 delete myBody ;

 -# When received on the other side, create a new Body :
 MessageBody *myNewBody = new MessageBody(Buffer, Length);

 -# Retrieve your Data in the same Order :
 myString = myNewBody->readString();
 myInt = myNewBody->readShortInt();

 -# Delete your Body(don't forget to deallocate the retrieved string also):
 delete myNewBody ;
*/
class MessageBody // : private strstreambuf
{
public:

 // -----------------------------------
 // -- Read/Write Methods for Blocks --
 // -----------------------------------

 inline void readBlock(char *Block, unsigned short Size)
 { sgetn(Block, Size); };

 /*! You can use this method to include the header at the beginning of the
 body, in order to make a single Socket->Send call.
 */
 inline void writeBlock(char *Block, unsigned short Size)
 { sputn(Block, Size); };

 // ----------------------------------------
 // -- Read and Write Methods for Integer --
 // ----------------------------------------

 inline unsigned short readShortInt(void)
 {
 unsigned short Result ;
 sgetn((char *) &Result, 2);
 return Result ;
 };

 inline void writeShortInt(unsigned short C)
 {
 sputn((char *) &C, 2);
 };

 inline unsigned long readLongInt(void)
 {
 unsigned long Result ;
 sgetn((char *) &Result, 4);
 return Result ;
 };

 inline void writeLongInt(unsigned long C)
 {
 sputn((char *) &C, 4);
 };

 // ----------------------------------------
 // -- Read and Write Methods for Strings --
 // ----------------------------------------
 void readString(char *store, unsigned short store_len);
 void writeString(const char *str);

 // -----------------------------
 // -- General Purpose Methods --
 // -----------------------------
 MessageBody(void);

 // To create a READ body, with a buffer of yours(it will NOT be deleted
 // by the destructor). No Write operations should be made.
 //MessageBody(char *InitBuffer, int InitLength);
 ~MessageBody(void);

 long getLength(void);
 char *getBuffer(void);

private:

 // ------------------------
 // -- Private Attributes --
 // ------------------------
 char Buffer[BODY_BUFFER_MAX_SIZE] ;
 unsigned long Length ;
 char *GetPtr ;
 // There is no PutPtr, because it computed with Buffer + Length.

 // ---------------------
 // -- Private Methods --
 // ---------------------

 inline void sputn(char *Ptr, unsigned short Size) {
 std::memcpy(Buffer + Length, Ptr, Size);
 Length += Size ;
 };

 inline void sgetn(char *Ptr, unsigned short Size) {
 std::memcpy(Ptr, GetPtr, Size);
 GetPtr += Size ;
 };
};
}

#endif // _CERTI_MESSAGE_BODY_HH

// $Id: MessageBody.hh,v 3.3 2003/02/17 16:00:06 breholee Exp $
