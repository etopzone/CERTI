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
// $Id: FedTime.cc,v 3.7 2005/04/30 17:06:45 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>

#include "RTI.hh"
#include "fedtime.hh"

typedef RTI::FedTime FedTime ;
typedef RTI::Double Double ;

namespace
{

/** FedTime / RTIfedTime reference cast
 */
const RTIfedTime &
rft(const FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    try {
	return dynamic_cast<const RTIfedTime &>(time);
    }
    catch (std::bad_cast) {
	throw RTI::InvalidFederationTime("Could not cast to RTIfedTime");
    }
}

} // anonymous namespace

namespace certi {

// ----------------------------------------------------------------------------
// FedTimeFactory
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
FedTime *
RTI::FedTimeFactory::makeZero()
    throw (RTI::MemoryExhausted)
{
    return new RTIfedTime();
}

// ----------------------------------------------------------------------------
FedTime *
RTI::FedTimeFactory::decode(const char *)
    throw (RTI::MemoryExhausted)
{
    throw RTI::RTIinternalError("Not implemented");
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
RTIfedTime::RTIfedTime(const RTI::Double &dble)
{
    _fedTime = dble ;
    _zero = _fedTime != 0 ;
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
    _fedTime = 0 ;
    _zero = 0 ;
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::isZero()
{
    return RTI::Boolean(_zero == 0);
}

// ----------------------------------------------------------------------------
void
RTIfedTime::setEpsilon()
{
    throw RTI::RTIinternalError("Not implemented");
}

// ----------------------------------------------------------------------------
void
RTIfedTime::setPositiveInfinity()
{
    throw RTI::RTIinternalError("Not implemented");
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::isPositiveInfinity()
{
    throw RTI::RTIinternalError("Not implemented");
}

// ----------------------------------------------------------------------------
int
RTIfedTime::encodedLength() const
{
    throw RTI::RTIinternalError("Not implemented");
}

// ----------------------------------------------------------------------------
void
RTIfedTime::encode(char *) const
{
    throw RTI::RTIinternalError("Not implemented");
}

// ----------------------------------------------------------------------------
int
RTIfedTime::getPrintableLength() const
{
    throw RTI::RTIinternalError("Not implemented");
}

// ----------------------------------------------------------------------------
void
RTIfedTime::getPrintableString(char *)
{
    throw RTI::RTIinternalError("Not implemented");
}

// ----------------------------------------------------------------------------
// Overloaded operators from FedTime
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
FedTime &
RTIfedTime::operator+=(const FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime += rft(time)._fedTime ;
    _zero = _fedTime != 0 ;
    return *this;
}

// ----------------------------------------------------------------------------
FedTime &
RTIfedTime::operator-=(const FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime -= rft(time)._fedTime ;
    _zero = _fedTime != 0 ;
    return *this;
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator<=(const FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    return RTI::Boolean(_fedTime <= rft(time)._fedTime);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator<(const FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    return RTI::Boolean(_fedTime < rft(time)._fedTime);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator>=(const FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    return RTI::Boolean(_fedTime >= rft(time)._fedTime);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator>(const FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    return RTI::Boolean(_fedTime > rft(time)._fedTime);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator==(const FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    return RTI::Boolean(_fedTime == rft(time)._fedTime);
}

// ----------------------------------------------------------------------------
FedTime &
RTIfedTime::operator=(const FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime = rft(time)._fedTime ;
    _zero = _fedTime != 0 ;
    return *this ;
}

// ----------------------------------------------------------------------------
// Implementation functions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
RTI::Double
RTIfedTime::getTime() const
{
    return _fedTime ;
}

// ----------------------------------------------------------------------------
// Implementation operators
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator==(const Double &time) const
    throw (RTI::InvalidFederationTime)
{
    return RTI::Boolean(_fedTime == time);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator!=(const FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    return RTI::Boolean(_fedTime != rft(time)._fedTime);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator!=(const Double &time) const
    throw (RTI::InvalidFederationTime)
{
    return RTI::Boolean(_fedTime != time);
}

// ----------------------------------------------------------------------------
FedTime &
RTIfedTime::operator=(const RTIfedTime &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime = time._fedTime ;
    _zero = time._zero ;
    return *this ;
}

// ----------------------------------------------------------------------------
FedTime &
RTIfedTime::operator=(const Double &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime = time ;
    _zero = _fedTime != 0 ;
    return *this ;
}

FedTime &
RTIfedTime::operator*=(const RTI::FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime *= rft(time)._fedTime ;
    return *this ;
}

FedTime &
RTIfedTime::operator/=(const RTI::FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime /= rft(time)._fedTime ;
    return *this ;
}

FedTime &
RTIfedTime::operator+=(const RTI::Double &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime += time ;
    return *this ;
}

FedTime &
RTIfedTime::operator-=(const RTI::Double &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime -= time ;
    return *this ;
}

FedTime &
RTIfedTime::operator*=(const RTI::Double &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime *= time ;
    return *this ;
}

FedTime &
RTIfedTime::operator/=(const RTI::Double &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime /= time ;
    return *this ;
}

// ----------------------------------------------------------------------------
RTIfedTime
RTIfedTime::operator+(const FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime + rft(time)._fedTime ;
    return fedtime ;
}

RTIfedTime
RTIfedTime::operator+(const Double &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime + time ;
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
RTIfedTime::operator-(const FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime - rft(time)._fedTime ;
    return fedtime ;
}

RTIfedTime
RTIfedTime::operator-(const Double &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime - time ;
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
RTIfedTime::operator*(const FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime * rft(time)._fedTime ;
    return fedtime ;
}

RTIfedTime
RTIfedTime::operator*(const Double &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime * time ;
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
RTIfedTime::operator/(const FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime / rft(time)._fedTime ;
    return fedtime ;
}

RTIfedTime
RTIfedTime::operator/(const Double &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime / time ;
    return fedtime ;
}

// ----------------------------------------------------------------------------
// Implementation friends
// ----------------------------------------------------------------------------
std::ostream &
operator<<(std::ostream &s, const FedTime &time)
{
    s << rft(time).getTime();
    return s ;
}

// ----------------------------------------------------------------------------
// Global operators
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
RTIfedTime
operator+(const Double &d, const FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = d + rft(time).getTime();
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
operator-(const Double &d, const FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = d - rft(time).getTime();
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
operator*(const Double &d, const FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = d * rft(time).getTime();
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
operator/(const Double &d, const FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = d / rft(time).getTime();
    return fedtime ;
}

} // namespace certi

// $Id: FedTime.cc,v 3.7 2005/04/30 17:06:45 breholee Exp $
