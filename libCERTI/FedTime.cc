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
// $Id: FedTime.cc,v 3.6 2005/03/14 19:45:22 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "fedtime.hh"

namespace
{
using certi::RTIfedTime;
using certi::FedTime;

/** FedTime / RTIfedTime reference cast
 */
const RTIfedTime &
rft(const FedTime &time)
    throw (certi::InvalidFederationTime)
{
    try {
	return dynamic_cast<const RTIfedTime &>(time);
    }
    catch (std::bad_cast) {
	throw certi::InvalidFederationTime();
    }
}

}

namespace certi {

// ----------------------------------------------------------------------------
// FedTimeFactory
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
FedTime *
FedTimeFactory::makeZero()
    throw (MemoryExhausted)
{
    return new RTIfedTime();
}

// ----------------------------------------------------------------------------
FedTime *
FedTimeFactory::decode(const char *)
    throw (MemoryExhausted)
{
    //not implemented
    throw MemoryExhausted();
}

// ----------------------------------------------------------------------------
// FedTime
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
FedTime::~FedTime()
{
}

// ----------------------------------------------------------------------------
// RTIfedTime
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
RTIfedTime::RTIfedTime()
{
    _fedTime = 0 ;
    _zero = 0 ;
}

// ----------------------------------------------------------------------------
RTIfedTime::RTIfedTime(const Double &dble)
{
    _fedTime = dble ;
    initZero();
}

// ----------------------------------------------------------------------------
RTIfedTime::RTIfedTime(const FedTime &time)
{
    const RTIfedTime &tmp(rft(time));
    _fedTime = tmp._fedTime ;
    _zero = tmp._zero ;
}

// ----------------------------------------------------------------------------
RTIfedTime::RTIfedTime(const RTIfedTime &time)
    : FedTime(), _fedTime(time._fedTime), _zero(time._zero)
{
}

// ----------------------------------------------------------------------------
RTIfedTime::~RTIfedTime()
{
}

// ----------------------------------------------------------------------------
// Overloaded functions from FedTime
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void
RTIfedTime::setZero()
{
    _zero = 0 ;
}

// ----------------------------------------------------------------------------
Boolean
RTIfedTime::isZero()
{
    return ((_zero == 0) ? RTI_TRUE : RTI_FALSE);
}

// ----------------------------------------------------------------------------
void
RTIfedTime::setEpsilon()
{
    //not implemented
}

// ----------------------------------------------------------------------------
void
RTIfedTime::setPositiveInfinity()
{
    //not implemented
}

// ----------------------------------------------------------------------------
Boolean
RTIfedTime::isPositiveInfinity()
{
    //not implemented
    return RTI_FALSE ; // compatibilite avec le type retourne
}

// ----------------------------------------------------------------------------
int
RTIfedTime::encodedLength() const
{
    //not implemented
    return 0 ; // compatibilite avec le type retourne
}

// ----------------------------------------------------------------------------
void
RTIfedTime::encode(char *) const
{
    //not implemented
}

// ----------------------------------------------------------------------------
int
RTIfedTime::getPrintableLength() const
{
    //not implemented
    return 0 ; // compatibilite avec le type retourne
}

// ----------------------------------------------------------------------------
void
RTIfedTime::getPrintableString(char *)
{
    //not implemented
}

// ----------------------------------------------------------------------------
// Overloaded operators from FedTime
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
FedTime &
RTIfedTime::operator+= (const FedTime &time)
    throw (InvalidFederationTime)
{
    _fedTime += rft(time)._fedTime ;
    initZero();
    return *this;
}

// ----------------------------------------------------------------------------
FedTime &
RTIfedTime::operator-= (const FedTime &time)
    throw (InvalidFederationTime)
{
    _fedTime -= rft(time)._fedTime ;
    initZero();
    return *this;
}

// ----------------------------------------------------------------------------
Boolean
RTIfedTime::operator<= (const FedTime &time) const
    throw (InvalidFederationTime)
{
    if (_fedTime <= rft(time)._fedTime) return RTI_TRUE ;
    else return RTI_FALSE ;
}

// ----------------------------------------------------------------------------
Boolean
RTIfedTime::operator<(const FedTime &time) const
    throw (InvalidFederationTime)
{
    return Boolean(_fedTime < rft(time)._fedTime);
}

// ----------------------------------------------------------------------------
Boolean
RTIfedTime::operator>= (const FedTime &time) const
    throw (InvalidFederationTime)
{
    return Boolean(_fedTime >= rft(time)._fedTime);
}

// ----------------------------------------------------------------------------
Boolean
RTIfedTime::operator>(const FedTime &time) const
    throw (InvalidFederationTime)
{
    return Boolean(_fedTime > rft(time)._fedTime);
}

// ----------------------------------------------------------------------------
Boolean
RTIfedTime::operator== (const FedTime &time) const
    throw (InvalidFederationTime)
{
    return Boolean(_fedTime == rft(time)._fedTime);
}

// ----------------------------------------------------------------------------
FedTime &
RTIfedTime::operator= (const FedTime &time)
    throw (InvalidFederationTime)
{
    _fedTime = rft(time)._fedTime ;
    initZero();
    return *this ;
}

// ----------------------------------------------------------------------------
// Implementation functions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Double
RTIfedTime::getTime() const
{
    return _fedTime ;
}

// ----------------------------------------------------------------------------
// Implementation operators
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Boolean
RTIfedTime::operator!=(const FedTime &time) const
    throw (InvalidFederationTime)
{
    return Boolean(_fedTime != rft(time)._fedTime);
}

// ----------------------------------------------------------------------------
FedTime &
RTIfedTime::operator=(const RTIfedTime &time)
    throw (InvalidFederationTime)
{
    _fedTime = time._fedTime ;
    _zero = time._zero ;
    return *this ;
}

// ----------------------------------------------------------------------------
RTIfedTime
RTIfedTime::operator+(const FedTime &time)
    throw (InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime + rft(time)._fedTime ;
    fedtime.initZero();
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
RTIfedTime::operator-(const FedTime &time)
    throw (InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime - rft(time)._fedTime ;
    fedtime.initZero();
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
RTIfedTime::operator*(const FedTime &time)
    throw (InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime * rft(time)._fedTime ;
    fedtime.initZero();
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
RTIfedTime::operator/(const FedTime &time)
    throw (InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime / rft(time)._fedTime ;
    fedtime.initZero();
    return fedtime ;
}

// ----------------------------------------------------------------------------
// Implementation friends
// ----------------------------------------------------------------------------
//ostream& operator<<(ostream&, const FedTime&)

// ----------------------------------------------------------------------------
// Global operators
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
RTIfedTime
operator+(const Double &d, const FedTime &time)
    throw (InvalidFederationTime)
{
    RTIfedTime fedtime = d + rft(time)._fedTime ;
    fedtime.initZero();
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
operator-(const Double &d, const FedTime &time)
    throw (InvalidFederationTime)
{
    RTIfedTime fedtime = d - rft(time)._fedTime ;
    fedtime.initZero();
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
operator*(const Double &d, const FedTime &time)
    throw (InvalidFederationTime)
{
    RTIfedTime fedtime = d * rft(time)._fedTime ;
    fedtime.initZero();
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
operator/(const Double &d, const FedTime &time)
    throw (InvalidFederationTime)
{
    RTIfedTime fedtime = d / rft(time)._fedTime ;
    fedtime.initZero();
    return fedtime ;
}

} // namespace certi

// $Id: FedTime.cc,v 3.6 2005/03/14 19:45:22 breholee Exp $
