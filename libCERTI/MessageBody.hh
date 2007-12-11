// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: MessageBody.hh,v 3.15 2007/12/11 16:44:20 rousse Exp $
// ----------------------------------------------------------------------------

#ifndef LIBCERTI_MESSAGE_BODY_HH
#define LIBCERTI_MESSAGE_BODY_HH

#include "certi.hh"

#include <cstring>
#include <vector>

namespace certi {

// ============================================================================
/** 
 * MessageBody is a class managing a char buffer for Message exchange.
 * It provides member functions to read and write integers and strings.
 * To read the contents of a MessageBody, juste read the elements in
 * the order they were written.
 */
/* FIXME EN. shouldn't we use just that need strstream? */
class MessageBody
{
public:
	MessageBody();
	MessageBody(size_t);
	~MessageBody();

	size_t size() const ;					//Buffer size (char)
	
	const unsigned char *getBuffer() const ;					
	unsigned char *getBufferModeRW();

	//--------------------------------------------------------------------------------------OUT
	MessageBody &operator<<(unsigned long);			//Write Buffer
	MessageBody &operator<<(unsigned short);

	void writeString(char *data)                            { sPutS(data);};			
	void writeBlock(char *block, unsigned short size)	{ sPutB(block, size);};
	void writeLongInt(unsigned long data)			{ *this << data;};
	void writeShortInt(unsigned short data)			{ *this << data;};
		
	//--------------------------------------------------------------------------------------IN
	const MessageBody &operator>>(unsigned long &);		//Read Buffer
	const MessageBody &operator>>(unsigned short &);

	void readString(char *data, unsigned short size)  { sGetS(data, size);};
	void readBlock(char *block, unsigned short size) { sGetB(block, size);};
	unsigned long readLongInt() const;	
	unsigned short readShortInt() const; 
	void	addToWritePointer(unsigned int nbWritten) {wPtr+=nbWritten;};

private:
   // FIXME is this reasonable?
   MessageBody* me() const { return const_cast<MessageBody*>(this);}
	void sGetS(char *pData, unsigned int storeSize);
	void sGetB(char *pData, unsigned int reqSize);

	void sPutS(char *pData);
	void sPutB(char *pData, unsigned int storeSize);

	unsigned char		*buffer;		//Buffer
	unsigned int		wPtr;			//Write Pointer
	unsigned int		rPtr;			//Read Pointer
	unsigned int		bSize;		//Buffer Size
};

} // certi

#endif // LIBCERTI_MESSAGE_BODY_HH

// $Id: MessageBody.hh,v 3.15 2007/12/11 16:44:20 rousse Exp $
