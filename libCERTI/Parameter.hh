// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003, 2005  ONERA
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
// $Id: Parameter.hh,v 3.7 2007/08/31 12:47:40 erk Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_PARAMETER_HH
#define CERTI_PARAMETER_HH

#include "certi.hh"
#include "SecurityLevel.hh"
#include "Named.hh"

#include <string>

namespace certi {

class Parameter : public Named
{
public:
    Parameter();
    Parameter(std::string);

    void display();
    
    void setHandle(Handle p) { handle = p ; };
    Handle getHandle() const { return handle ; };

    SecurityLevelID LevelID ;

private:
    Handle handle ;
};

} // namespace certi

#endif // CERTI_PARAMETER_HH

// $Id: Parameter.hh,v 3.7 2007/08/31 12:47:40 erk Exp $
