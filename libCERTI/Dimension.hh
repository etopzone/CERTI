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
// $Id: Dimension.hh,v 3.1 2003/06/27 17:26:28 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_DIMENSION_HH
#define _CERTI_DIMENSION_HH

#include "RTItypes.hh"

#include <string>

namespace certi {

class Dimension
{
public:
    Dimension(DimensionHandle);
    void setName(std::string);
    std::string getName();
    DimensionHandle getHandle();
    static void setLowerBound(ULong);
    static void setUpperBound(ULong);

private:
    std::string name ;
    DimensionHandle handle ;
    static ULong axisLowerBound ;
    static ULong axisUpperBound ;
};

} // namespace certi

#endif

// $Id: Dimension.hh,v 3.1 2003/06/27 17:26:28 breholee Exp $
