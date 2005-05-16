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
// $Id: MessageBody.hh,v 3.10 2005/05/16 08:22:30 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef LIBCERTI_MESSAGE_BODY_HH
#define LIBCERTI_MESSAGE_BODY_HH

#include "certi.hh"

#include <cstring>
#include <vector>

namespace certi {

// ============================================================================
/** MessageBody is a class managing a char buffer for Message exchange.
    It provide member functions to read and write integers and strings.
    To read the contents of a MessageBody, juste read the elements in
    the order they were written.
*/
class MessageBody
{
public:
    MessageBody();
    MessageBody(size_t);

    size_t size() const ;
    const unsigned char *getBuffer() const ;

    MessageBody &operator<<(unsigned long);
    const MessageBody &operator>>(unsigned long &) const ;

    MessageBody &operator<<(unsigned short);
    const MessageBody &operator>>(unsigned short &) const ;

    void writeBlock(char *block, unsigned short size) { sputn(block, size); };
    void readBlock(char *block, unsigned short size) const { sgetn(block, size); };

    void writeLongInt(unsigned long c) { *this << c ; };
    unsigned long readLongInt() const { unsigned long result ; *this >> result ; return result ; };

    void writeShortInt(unsigned short c) { sputn((char *) &c, 2); };
    unsigned short readShortInt() const { unsigned short result ; sgetn((char *) &result, 2); return result ; };

    void writeString(const char *);
    void readString(char *, unsigned short);

private:
    void sputn(char *ptr, unsigned short size) { std::copy(ptr, ptr + size, std::back_inserter(buffer)); };

    void sgetn(char *ptr, unsigned short size) const {
	for (int i = 0 ; i < size ; ++i, ++getPtr)
	    ptr[i] = *getPtr ;
    };

    std::vector<unsigned char> buffer ;
    mutable std::vector<unsigned char>::iterator getPtr ;
};

} // certi

#endif // LIBCERTI_MESSAGE_BODY_HH

// $Id: MessageBody.hh,v 3.10 2005/05/16 08:22:30 breholee Exp $
