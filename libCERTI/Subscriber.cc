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
// $Id: Subscriber.cc,v 3.10 2005/03/25 17:13:11 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Subscriber.hh"
#include "RTIRegion.hh"

#include "PrettyDebug.hh"

namespace {

PrettyDebug D("SUBSCRIBER", __FILE__);

}

namespace certi {

// ----------------------------------------------------------------------------
// Constructor
Subscriber::Subscriber(FederateHandle h)
    : handle(h), region(0)
{
}

// ----------------------------------------------------------------------------
// Constructor
Subscriber::Subscriber(FederateHandle h, const RTIRegion *r)
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
const RTIRegion *
Subscriber::getRegion() const
{
    return region ;
}

// ----------------------------------------------------------------------------
bool
Subscriber::operator==(const Subscriber &sub) const
{
    return (handle == sub.getHandle()) && (region == sub.getRegion());
}

// ----------------------------------------------------------------------------
bool
Subscriber::equals(FederateHandle fed, const RTIRegion *r) const
{
    return handle == fed && region == r ;
}

// ----------------------------------------------------------------------------
/** Check if subscriber's region matches (overlaps) with the one in
    parameter. If one of them is the default region (null) the result
    is 'true'
 */
bool
Subscriber::match(const RTIRegion *r) const
{
    D[pdTrace] << "Match test: " << (region ? region->getHandle() : 0) << " vs "
	       << (r ? r->getHandle() : 0) << std::endl ;
    return (region == 0) || (r == 0) || region->overlaps(*r);
}

} // namespace certi

// $Id: Subscriber.cc,v 3.10 2005/03/25 17:13:11 breholee Exp $
