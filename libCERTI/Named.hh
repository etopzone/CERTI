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
// $Id: Named.hh,v 3.1 2006/03/09 19:54:28 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef LIBCERTI_NAMED_HH
#define LIBCERTI_NAMED_HH

#include <string>

namespace certi {

class Named
{
public:
    Named();
    Named(std::string);

    void setName(std::string);
    std::string getName() const ;
    const char *getCName() const ;
    bool isNamed(const std::string &) const ;

protected:
    std::string name ;
};

} // certi

#endif // LIBCERTI_NAMED_HH

// $Id: Named.hh,v 3.1 2006/03/09 19:54:28 breholee Exp $
