// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2004  Benoît Bréholée
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
// $Id: Named.cc,v 3.0 2005/03/25 16:07:31 breholee Exp $
// ----------------------------------------------------------------------------

#include "Named.hh"

namespace certi {

/** Default constructor. Name initialized with the empty string.
 */
Named::Named() { }

/** Constructor with inital value.
    @param s Initial name
*/
Named::Named(std::string s)
    : name(s) { }

/** Set name.
    @param s New name
*/
void
Named::setName(std::string s)
{
    name = s ;
}

/** Get name.
    @return name value
*/
std::string
Named::getName() const
{
    return name ;
}

/** Get C-style name.
    @return pointer on name, as const char *
*/
const char *
Named::getCName() const
{
    return name.c_str();
}

/** Check if the name matches the provided parameter.
    @param s String to compare
    @return true if strings match
 */
bool
Named::isNamed(const std::string &s) const
{
    return name == s ;
}

} // certi

// $Id: Named.cc,v 3.0 2005/03/25 16:07:31 breholee Exp $
