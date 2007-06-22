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
// $Id: MessageBody.cc,v 3.12 2007/06/22 08:51:37 erk Exp $
// ----------------------------------------------------------------------------

#include "Certi_Win.h"
#include "MessageBody.hh"
#include <cassert>

namespace certi {

/// Constructor. Created MessageBody is empty, buffer size 0
MessageBody::MessageBody()
{
wPtr= rPtr= 0;			//Init Read/Write Pointer
buffer= (unsigned char*)malloc(bSize= BUFFER_SIZE_DEFAULT);
}

/// Constructor, with an initial buffer size.@param n Buffer size
MessageBody::MessageBody(size_t n)
{
wPtr= rPtr= 0;			//Init Read/Write Pointer
buffer= (unsigned char*)malloc(bSize= n);
}

/// Release 
MessageBody::~MessageBody()
{
if (buffer) free(buffer);
}

/// Get the size of the buffer. @return buffer size
size_t MessageBody::size() const { return wPtr;}

/// Get buffer pointer. @return buffer pointer. 
const unsigned char *MessageBody::getBuffer() const	{ return &(buffer[0]);}
unsigned char *MessageBody::getBufferModeRW()			{ return &(buffer[0]);}

// ----------------------------------------------------------------------------
void MessageBody::sGetB(char *pData, unsigned int reqSize)
{//Read a block : char[0..R..W..]

if (reqSize > (wPtr - rPtr))
	throw RTIinternalError("[sGetB] Bloc too long for storage.");
else
	memcpy(pData,&buffer[rPtr],reqSize); rPtr+= reqSize;
}
		
// ----------------------------------------------------------------------------
void MessageBody::sGetS(char *pData, unsigned int storeSize) 
{//Read a string : char[NN.R..W..]	NN: message length 

unsigned int bufSize = readShortInt();
if (bufSize > storeSize)
	throw RTIinternalError("[sGetS] String in Message too long for storage.");
else if (bufSize > 0)
	sGetB(pData, bufSize);
pData[bufSize]= 0;
}

// ----------------------------------------------------------------------------
void MessageBody::sPutS(char *pData)
{//Write a string into Buffer
unsigned short	dataSize= strlen(pData); 

if ((dataSize + sizeof(unsigned short)) < bSize)
	{//Put a string : write length [NN] + string
	*this << dataSize;
	sPutB(pData,dataSize);
	}
else
	throw RTIinternalError("[sPutS] String in Message too long for storage.");
}

// ----------------------------------------------------------------------------
void MessageBody::sPutB(char *pData, unsigned int storeSize)
{//Write a block

if (storeSize < (bSize - wPtr))
	{ 
	memcpy(&buffer[wPtr],pData,storeSize); 
	wPtr+= storeSize;
	}
else
	throw RTIinternalError("[sPutB] String in Message too long for storage.");
}

// ----------------------------------------------------------------------------
unsigned long MessageBody::readLongInt() const	
{ 
unsigned long  result; 		
*(me()) >> result; 
return result;
};
unsigned short MessageBody::readShortInt() const 
{ 
unsigned short result; 
*(me()) >> result; 
return result;
};

// ----------------------------------------------------------------------------
MessageBody &MessageBody::operator<<(unsigned long val)
{
assert(val <= 4294967295U);
#ifdef BIG_INDIAN
	buffer[wPtr++]= (char)(0x000000FF & (val >> 24));
	buffer[wPtr++]= (char)(0x000000FF & (val >> 16));
	buffer[wPtr++]= (char)(0x000000FF & (val >>  8));
	buffer[wPtr++]= (char)(0x000000FF & val;
#else
	buffer[wPtr++]= (char)(0x000000FF & val);
	buffer[wPtr++]= (char)(0x000000FF & (val >>  8));
	buffer[wPtr++]= (char)(0x000000FF & (val >> 16));
	buffer[wPtr++]= (char)(0x000000FF & (val >> 24));
#endif

return *this ;
}

MessageBody &MessageBody::operator<<(unsigned short val)
{
assert(val <= 4294967295U);
#ifdef BIG_INDIAN
	buffer[wPtr++]= (char)0x000000FF & (val >>  8);
	buffer[wPtr++]= (char)0x000000FF & val;
#else
	buffer[wPtr++]= (char)(0x000000FF & val);
	buffer[wPtr++]= (char)(0x000000FF & (val >>  8));
#endif

return *this ;
}

// ----------------------------------------------------------------------------
const MessageBody &MessageBody::operator>>(unsigned long &val)
{
#ifdef BIG_INDIAN
	val = (unsigned long) (buffer[rPtr++] << 24);
	val|= (unsigned long) (buffer[rPtr++] << 16);
	val|= (unsigned long) (buffer[rPtr++] <<  8);
	val|= (unsigned long) buffer[rPtr++];
#else
	val = (unsigned long) buffer[rPtr++];
	val|= (unsigned long) (buffer[rPtr++] <<  8);
	val|= (unsigned long) (buffer[rPtr++] << 16);
	val|= (unsigned long) (buffer[rPtr++] << 24);
#endif
return *this ;
}

const MessageBody &MessageBody::operator>>(unsigned short &val)
{
#ifdef BIG_INDIAN
	val = (unsigned short) (buffer[rPtr++] <<  8);
	val|= (unsigned short) buffer[rPtr++];
#else
	val = (unsigned short) buffer[rPtr++];
	val|= (unsigned short) (buffer[rPtr++] <<  8);
#endif
return *this ;
}

} // certi

// $Id: MessageBody.cc,v 3.12 2007/06/22 08:51:37 erk Exp $
