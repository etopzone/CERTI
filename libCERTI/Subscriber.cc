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
// $Id: Subscriber.cc,v 3.6 2003/07/10 16:27:42 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Subscriber.hh"

namespace certi {

// ----------------------------------------------------------------------------
// Constructor
Subscriber::Subscriber(FederateHandle h)
    : handle(h)
{
}

// ----------------------------------------------------------------------------
// Constructor
Subscriber::Subscriber(FederateHandle h, RegionImp *r)
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
RegionImp *
Subscriber::getRegion() const
{
    return region ;
}

// ----------------------------------------------------------------------------
bool
Subscriber::match(FederateHandle fed, RegionImp *r) const
{
    return fed == handle && r == region ;
}

}

// $Id: Subscriber.cc,v 3.6 2003/07/10 16:27:42 breholee Exp $

