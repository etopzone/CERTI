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
// $Id: baseTypes.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "baseTypes.hh"

namespace certi {

Exception::Exception()
{
  _reason = NULL;
  _serial = 0;
}

Exception::~Exception()
{
}

Exception::Exception(const char *reason)
{
  _reason =(char *)reason;
  _serial = 0;
}

Exception::Exception(ULong serial, const char *reason)
{
  _serial = serial;
  _reason =(char *)reason;
}


Exception::Exception(const Exception &toCopy)
{
  _serial = toCopy._serial;
  _reason = toCopy._reason;
  _name   = toCopy._name;
}

Exception& Exception::operator =(const Exception &toCopy)
{
  _serial = toCopy._serial;
  _reason = toCopy._reason;
  _name   = toCopy._name;

  return *this;
}

}

// $Id: baseTypes.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $$
