// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
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
// $Id: DebugOStream.hh,v 3.2 2003/06/26 15:20:25 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_DEBUGOSTREAM_HH
#define _CERTI_DEBUGOSTREAM_HH

#include <config.h>
#include <iostream>

// Attention, defining methods in the class declaration make them
// inline. All methods in DebugOStream are inline.

class DebugOStream
{
private:
    std::ostream& ostr;
    
public:
    static DebugOStream nullOutputStream;
    
    DebugOStream(std::ostream& theostr) : ostr(theostr) {}
    
    int isNullOstream(void) { 
        return(this != &(nullOutputStream));
    }
    
    // Global insertors on strings and characters are defined as
    // members of DebugOStream.
    DebugOStream& operator<<(const char* thestr)
    {
        if (this != &(DebugOStream::nullOutputStream))
            ostr << thestr;
        return *this ;
    }
    
    DebugOStream& operator<<(const signed char* thestr) 
    {
        if (this != &(DebugOStream::nullOutputStream)) ostr << thestr;
        return *this ;
    }
    
    DebugOStream& operator<<(const unsigned char* thestr) 
    {
        if (this != &(DebugOStream::nullOutputStream)) ostr << thestr;
        return *this ;
    }
    
    DebugOStream& operator<<(char ch) 
    {
        if (this != &(DebugOStream::nullOutputStream)) ostr << ch;
        return *this ;
    }
    
    DebugOStream& operator<<(signed char ch) 
    {
        if (this != &(DebugOStream::nullOutputStream)) ostr << ch;
        return *this ;
    }
    
    DebugOStream& operator<<(unsigned char ch) 
    {
        if (this != &(DebugOStream::nullOutputStream)) ostr << ch;
        return *this ;
    }
    
    //Copy And Redefine the ostream public interface
    //Arithmetic Inserters
    DebugOStream& operator<<(bool n)
    {
        if (this != &(DebugOStream::nullOutputStream)) ostr << n;
        return(*this);
    }

    DebugOStream& operator<<(short n)
    {
        if (this != &(DebugOStream::nullOutputStream)) ostr << n;
        return(*this);
    }
 
    DebugOStream& operator<<(unsigned short n)
    {
        if (this != &(DebugOStream::nullOutputStream)) ostr << n;
        return(*this);
    }

    DebugOStream& operator<<(int n)
    {
        if (this != &(DebugOStream::nullOutputStream)) ostr << n;
        return(*this);
    }

    DebugOStream& operator<<(unsigned int n)
    {
        if (this != &(DebugOStream::nullOutputStream)) ostr << n;
        return(*this);
    }

    DebugOStream& operator<<(long n)
    {
        if (this != &(DebugOStream::nullOutputStream)) ostr << n;
        return(*this);
    }

    DebugOStream& operator<<(unsigned long n)
    {
        if (this != &(DebugOStream::nullOutputStream)) ostr << n;
        return(*this);
    }

    DebugOStream& operator<<(float f)
    {
        if (this != &(DebugOStream::nullOutputStream)) ostr << f;
        return(*this);
    }  

    DebugOStream& operator<<(double f)
    {
        if (this != &(DebugOStream::nullOutputStream)) ostr << f;
        return(*this);
    }

    DebugOStream& operator<<(long double f)
    {
        if (this != &(DebugOStream::nullOutputStream)) ostr << f;
        return(*this);
    }

    //Specifique gcc, dans la norme le pointeur n'est pas constant.
    DebugOStream& operator<<(const void* p)
    {
        if (this != &(DebugOStream::nullOutputStream)) ostr << p;
        return(*this);
    }

// #ifdef _GLIBCPP_USE_LONG_LONG
//     //Specifique gcc
//     DebugOStream& operator<<(long long n)
//     {
//         if (this != &(DebugOStream::nullOutputStream)) ostr << n;
//         return(*this);
//     }

//     //Specifique gcc
//     DebugOStream& operator<<(unsigned long long n)
//     {
//         if (this != &(DebugOStream::nullOutputStream)) ostr << n;
//         return(*this);
//     }
// #endif

    //Specifique gcc
    DebugOStream& operator<<(std::streambuf* sb)
    {
        if (this != &(DebugOStream::nullOutputStream)) ostr << sb;
        return(*this);
    }

    //Dans la norme C++, il faut passer une référence et pas un
    //pointeur mais ca marche pas avec gcc

    //DebugOStream& operator<<(streambuf& sb)
    //{
    //  if (this != &(DebugOStream::nullOutputStream)) ostr << sb;
    //  return(*this);
    //}

    DebugOStream& operator<<(std::ostream& (*pf)(std::ostream&))
    {
        if (this != &(DebugOStream::nullOutputStream)) 
            ostr << pf;
        return(*this);
    }

    DebugOStream& operator<<(std::ios& (*pf)(std::ios&))
    {
        if (this != &(DebugOStream::nullOutputStream)) 
            ostr << pf;
        return(*this);
    }

    DebugOStream& operator<<(std::ios_base& (*pf)(std::ios_base&))
    {
        if (this != &(DebugOStream::nullOutputStream)) 
            ostr << pf;
        return(*this);
    }

    // Unformatted output:
    DebugOStream& put(char c)
    {
        if (this != &(DebugOStream::nullOutputStream)) 
            ostr.put(c);
        return(*this);
    }
   
    DebugOStream& write(const char* str, std::streamsize n)
    {
        if (this != &(DebugOStream::nullOutputStream)) 
            ostr.write(str, n);
        return(*this);
    }

    // Other methods
    DebugOStream& flush(void)
    {
        if (this != &(DebugOStream::nullOutputStream)) 
            ostr.flush();
        return(*this);
    }

    DebugOStream& seekp(std::streampos pos)
    {
        if (this != &(DebugOStream::nullOutputStream)) 
            ostr.seekp(pos);
        return(*this);
    }

    DebugOStream& seekp(std::streamoff off, std::ios_base::seekdir dir)
    {
        if (this != &(DebugOStream::nullOutputStream)) 
            ostr.seekp(off, dir);
        return(*this);
    }

    std::streampos tellp(void)
    {
        if (this != &(DebugOStream::nullOutputStream)) 
            return(ostr.tellp());
        else return(0);
    }

    //End Copy And Redifine the ostream public interface

    //The methods herited from ios_base and ios are not redefined in
    //DebugOStream to avoid basic access to these ostreams aimed at
    //printing simple debug messages
};

#endif // _CERTI_DEBUGOSTREAM_HH

// $Id: DebugOStream.hh,v 3.2 2003/06/26 15:20:25 breholee Exp $
