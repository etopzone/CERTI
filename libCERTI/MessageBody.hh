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
// $Id: MessageBody.hh,v 3.8 2004/05/18 13:18:53 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef LIBCERTI_MESSAGE_BODY_HH
#define LIBCERTI_MESSAGE_BODY_HH

#include "certi.hh"

#include <cstring>
#include <vector>

namespace certi {

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
    ~MessageBody();

    void readBlock(char *block, unsigned short size) const { 
	sgetn(block, size);
    };

    void writeBlock(char *block, unsigned short size) { sputn(block, size); };

    unsigned short readShortInt() const {
        unsigned short result ;
        sgetn((char *) &result, 2);
        return result ;
    };

    void writeShortInt(unsigned short c) { sputn((char *) &c, 2); };

    unsigned long readLongInt() const {
        unsigned long result ;
        sgetn((char *) &result, 4);
        return result ;
    };

    void writeLongInt(unsigned long c) { sputn((char *) &c, 4); };

    void readString(char *, unsigned short);
    void writeString(const char *);

    size_t getLength() const ;
    const char *getBuffer() const ;

private:
    void sputn(char *ptr, unsigned short size) {
	std::copy(ptr, ptr + size, std::back_inserter(buffer));
    };
    void sgetn(char *ptr, unsigned short size) const {
	for (int i = 0 ; i < size ; ++i, ++getPtr)
	    ptr[i] = *getPtr ;	
    };

    std::vector<char> buffer ;
    mutable std::vector<char>::iterator getPtr ;
};

} // certi

#endif // LIBCERTI_MESSAGE_BODY_HH

// $Id: MessageBody.hh,v 3.8 2004/05/18 13:18:53 breholee Exp $
