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
// $Id: DebugOStream.hh,v 4.3 2009/10/21 19:51:13 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_DEBUGOSTREAM_HH
#define _CERTI_DEBUGOSTREAM_HH

#include <iostream>
#include "certi.hh"

/** Stream used by PrettyDebug
 */
class CERTI_EXPORT DebugOStream
{
private:
    std::ostream* ostr;

public:
    DebugOStream(std::ostream* theostr) : ostr(theostr) {}

    int isNullOstream(void) {
        return ostr == 0;
    }

    // Global insertors on strings and characters are defined as
    // members of DebugOStream.
    DebugOStream& operator<<(const char* thestr)
    {
        if (ostr)
            *ostr << thestr;
        return *this ;
    }

    DebugOStream& operator<<(const signed char* thestr)
    {
        if (ostr) *ostr << thestr;
        return *this ;
    }

    DebugOStream& operator<<(const unsigned char* thestr)
    {
        if (ostr) *ostr << thestr;
        return *this ;
    }

    DebugOStream& operator<<(char ch)
    {
        if (ostr) *ostr << ch;
        return *this ;
    }

    DebugOStream& operator<<(signed char ch)
    {
        if (ostr) *ostr << ch;
        return *this ;
    }

    DebugOStream& operator<<(unsigned char ch)
    {
        if (ostr) *ostr << ch;
        return *this ;
    }

    //Copy And Redefine the ostream public interface
    //Arithmetic Inserters
    DebugOStream& operator<<(bool n)
    {
        if (ostr) *ostr << n;
        return(*this);
    }

    DebugOStream& operator<<(short n)
    {
        if (ostr) *ostr << n;
        return(*this);
    }

    DebugOStream& operator<<(unsigned short n)
    {
        if (ostr) *ostr << n;
        return(*this);
    }

    DebugOStream& operator<<(int n)
    {
        if (ostr) *ostr << n;
        return(*this);
    }

    DebugOStream& operator<<(unsigned int n)
    {
        if (ostr) *ostr << n;
        return(*this);
    }

    DebugOStream& operator<<(long n)
    {
        if (ostr) *ostr << n;
        return(*this);
    }

    DebugOStream& operator<<(unsigned long n)
    {
        if (ostr) *ostr << n;
        return(*this);
    }

    DebugOStream& operator<<(float f)
    {
        if (ostr) *ostr << f;
        return(*this);
    }

    DebugOStream& operator<<(double f)
    {
        if (ostr) *ostr << f;
        return(*this);
    }

    DebugOStream& operator<<(long double f)
    {
        if (ostr) *ostr << f;
        return(*this);
    }

    //Specifique gcc, dans la norme le pointeur n'est pas constant.
    DebugOStream& operator<<(const void* p)
    {
        if (ostr) *ostr << p;
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
        if (ostr) *ostr << sb;
        return(*this);
    }

    //Dans la norme C++, il faut passer une r�f�rence et pas un
    //pointeur mais ca marche pas avec gcc

    //DebugOStream& operator<<(streambuf& sb)
    //{
    //  if (this != &(DebugOStream::nullOutputStream)) ostr << sb;
    //  return(*this);
    //}

    DebugOStream& operator<<(std::ostream& (*pf)(std::ostream&))
    {
        if (ostr)
            *ostr << pf;
        return(*this);
    }

    DebugOStream& operator<<(std::ios& (*pf)(std::ios&))
    {
        if (ostr)
            *ostr << pf;
        return(*this);
    }

    DebugOStream& operator<<(std::ios_base& (*pf)(std::ios_base&))
    {
        if (ostr)
            *ostr << pf;
        return(*this);
    }

    // Unformatted output:
    DebugOStream& put(char c)
    {
        if (ostr)
            ostr->put(c);
        return(*this);
    }

    DebugOStream& write(const char* str, std::streamsize n)
    {
        if (ostr)
            ostr->write(str, n);
        return(*this);
    }

    // Other methods
    DebugOStream& flush(void)
    {
        if (ostr)
            ostr->flush();
        return(*this);
    }

    DebugOStream& seekp(std::streampos pos)
    {
        if (ostr)
            ostr->seekp(pos);
        return(*this);
    }

    DebugOStream& seekp(std::streamoff off, std::ios_base::seekdir dir)
    {
        if (ostr)
            ostr->seekp(off, dir);
        return(*this);
    }

    std::streampos tellp(void)
    {
        if (ostr)
            return(ostr->tellp());
        else return(0);
    }

    //End Copy And Redifine the ostream public interface

    //The methods herited from ios_base and ios are not redefined in
    //DebugOStream to avoid basic access to these ostreams aimed at
    //printing simple debug messages
};

#endif // _CERTI_DEBUGOSTREAM_HH

// $Id: DebugOStream.hh,v 4.3 2009/10/21 19:51:13 erk Exp $
