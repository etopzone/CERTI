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
// $Id: Subscriber.cc,v 3.8 2005/03/15 14:34:51 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Subscriber.hh"

namespace certi {

// ----------------------------------------------------------------------------
// Constructor
Subscriber::Subscriber(FederateHandle h)
    : handle(h), region(0)
{
}

// ----------------------------------------------------------------------------
// Constructor
Subscriber::Subscriber(FederateHandle h, const RegionImp *r)
    : handle(h), region(r)
{
}

// ----------------------------------------------------------------------------
FederateHandle
Subscriber::getHandle() const
{
    return handle ;
}

// ----------------------------------------------------------------------------
const RegionImp *
Subscriber::getRegion() const
{
    return region ;
}

// ----------------------------------------------------------------------------
bool
Subscriber::match(FederateHandle fed, const RegionImp *r) const
{
    return fed == handle && r == region ;
}

// ----------------------------------------------------------------------------
/** Check if subscriber's region matches (overlaps) with the one in
    parameter. If one of them is the default region (null) the result
    is 'true'
 */
bool
Subscriber::match(const RegionImp *r) const
{
    return region == 0 || r == 0 || region->overlaps(*r);
}

} // namespace certi

// $Id: Subscriber.cc,v 3.8 2005/03/15 14:34:51 breholee Exp $
