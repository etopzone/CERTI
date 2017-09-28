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
// ----------------------------------------------------------------------------

#include "Named.hh"

namespace certi {

Named::Named()
{
}

Named::Named(const std::string& newName) : name(newName)
{
}

Named::~Named()
{
}

void Named::setName(const std::string& newName)
{
    name = newName;
}

const std::string& Named::getName() const
{
    return name;
}

bool Named::isNamed(const std::string& newName) const
{
    return (name == newName);
}

bool Named::validateHLAName(const std::string& /*name*/)
{
    // FIXME EN. should implement clean validation scheme
    // using IEEE-1516.2-2000 (§3.3.1 Names).
    return true;
}

bool Named::isQualifiedClassName(const std::string& name)
{
    size_t idx = name.find_first_of('.', 0);
    return (idx != std::string::npos);
}

std::string Named::getNextClassName(std::string& qualifiedClassName)
{
    std::string retval;
    size_t idx = 0;
    /* take the substring from start up to '.' (excluded) */
    idx = qualifiedClassName.find_first_of('.', 0);
    if (idx != std::string::npos) {
        retval = qualifiedClassName.substr(0, idx);
        /* update the string with the remaining string */
        qualifiedClassName = qualifiedClassName.substr(idx + 1);
    }
    else {
        retval = qualifiedClassName;
    }
    return retval;
} /* end of getNextClassName */

std::string Named::getLeafClassName(const std::string& qualifiedClassName)
{
    size_t idx = 0;
    /* take the substring from '.' (excluded) until the end */
    idx = qualifiedClassName.find_last_of('.');
    return qualifiedClassName.substr(idx + 1);
} /* end of getLeafClassName */

} // certi
