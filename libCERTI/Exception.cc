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
// $Id: Exception.cc,v 3.0 2003/03/21 13:43:01 breholee Exp $
// ----------------------------------------------------------------------------

// Project
#include <config.h>
#include "baseTypes.hh"

namespace certi {

Exception::Exception()
{
    _reason = NULL ;
    _serial = 0 ;
}

Exception::~Exception()
{
}

Exception::Exception(const char *reason)
{
    _reason = (char *)reason ;
    _serial = 0 ;
}

Exception::Exception(ULong serial, const char *reason)
{
    _serial = serial ;
    _reason = (char *)reason ;
}


Exception::Exception(const Exception &toCopy)
{
    _serial = toCopy._serial ;
    _reason = toCopy._reason ;
    _name = toCopy._name ;
}

Exception& Exception::operator = (const Exception &toCopy)
{
    _serial = toCopy._serial ;
    _reason = toCopy._reason ;
    _name = toCopy._name ;

    return *this ;
}

} // namespace certi

// $Id: Exception.cc,v 3.0 2003/03/21 13:43:01 breholee Exp $$
