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
// $Id: Range.hh,v 3.1 2004/05/17 23:06:58 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RANGE_HH
#define _CERTI_RANGE_HH

#include "RTItypes.hh"

namespace certi {

class Range : public Handled<DimensionHandle>
{
public:
    Range(DimensionHandle);
    Range(DimensionHandle, ULong, ULong);

    void setLowerBound(ULong);
    void setUpperBound(ULong);

    ULong getLowerBound();
    ULong getUpperBound();

private:
    ULong lowerBound ;
    ULong upperBound ;
};

} // namespace certi

#endif // _CERTI_RANGE_HH

// $Id: Range.hh,v 3.1 2004/05/17 23:06:58 breholee Exp $
