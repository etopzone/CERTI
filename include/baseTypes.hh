// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libRTI
//
// CERTI-libRTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libRTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: baseTypes.hh,v 3.4 2003/10/27 10:41:03 breholee Exp $
// ----------------------------------------------------------------------------

// Declaration de la classe Exception et d'autres types de base Ulong
// etc...

#ifndef CERTI_BASE_TYPES_HH
#define CERTI_BASE_TYPES_HH

#include <cstdlib>

namespace certi {

typedef unsigned short UShort ;
typedef short Short ;
typedef unsigned long ULong ;
typedef long Long ;
typedef double Double ;
typedef float Float ;

enum Boolean { RTI_FALSE = 0, RTI_TRUE };

class Exception {
public:
    ULong _serial ;
    char *_reason ;
    const char *_name ;
    Exception();
    Exception(const char *reason);
    Exception(ULong serial, const char *reason=NULL);
    Exception(const Exception &toCopy);
    virtual ~Exception();
    Exception & operator = (const Exception &);
    // friend ostream& operator <<(ostream &, Exception *);
    virtual Exception *cloneSelf() const throw() = 0;
    virtual void throwSelf() const = 0;
};

#define RTI_EXCEPT(A) \
class A : public Exception { \
public: \
 A() { _name = ""#A ; } \
 A(const char *reason) : Exception(reason) { _name = "RTI::"#A ; } \
 A(ULong serial, const char *reason=NULL) \
 : Exception(serial, reason) { _name = "RTI::"#A ; } \
 Exception *cloneSelf() const throw() { return (new A(_reason)); } \
 void throwSelf() const { throw *this ; } \
};

}

#endif // CERTI_BASE_TYPES_HH

// $Id: baseTypes.hh,v 3.4 2003/10/27 10:41:03 breholee Exp $
