// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: Parameter.cc,v 3.11 2009/11/19 18:15:30 erk Exp $
// ----------------------------------------------------------------------------


#include "Parameter.hh"

#include <stdio.h>
#include <iostream>

namespace certi {

Parameter::Parameter()
    : LevelID(PublicLevelID), handle(0)
{
}

Parameter::Parameter(const std::string& s)
    : Named(s), LevelID(PublicLevelID), handle(0)
{
}

void
Parameter::display(void)
{
    std::cout << " Parameter " << handle << ": "
	      << (name.length() == 0 ? "(no name)" : name.c_str())
	      << " [Level " << LevelID << "]" << std::endl ;
}

} // namespace certi

// $Id: Parameter.cc,v 3.11 2009/11/19 18:15:30 erk Exp $
