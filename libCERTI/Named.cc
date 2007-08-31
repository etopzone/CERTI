// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2004  ONERA
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
// $Id: Named.cc,v 3.2 2007/08/31 12:47:40 erk Exp $
// ----------------------------------------------------------------------------

#include "Named.hh"

namespace certi {

Named::Named() { }

Named::Named(std::string name)
    : name(name) { }

void
Named::setName(std::string name)
{
    this->name = name ;
}

void 
Named::setName(const char* name) {
    this->name = name;
}

const std::string
Named::getName() const
{
    return name ;
}


const char *
Named::getCName() const
{
    return name.c_str();
}

bool
Named::isNamed(const std::string &name) const
{
    return (this->name == name) ;
}

} // certi

// $Id: Named.cc,v 3.2 2007/08/31 12:47:40 erk Exp $
