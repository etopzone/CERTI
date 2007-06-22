// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2006  ONERA
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
// ----------------------------------------------------------------------------

#include "Certi_Win.h"

#include "certi.hh"
#include "fedtime.hh"

#ifdef _WIN32
	#ifdef max
		#undef max
	#endif
#endif

#include <algorithm>
#include <limits>
#include <sstream>

using std::stringstream ;

typedef RTI::FedTime FedTime ;
typedef RTI::Double Double ;

namespace
{

// ----------------------------------------------------------------------------
/** FedTime to RTIfedTime reference cast
 */
inline const RTIfedTime &
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

inline bool
is_infinity(const FedTime &time)
{
    return const_cast<FedTime &>(time).isPositiveInfinity() == RTI::RTI_TRUE ;
}

const double epsilon = 1.0e-9 ;
const double infinity = std::numeric_limits<double>::max();
const char *infinity_str = "+inf" ;

} // anonymous namespace

// ----------------------------------------------------------------------------
// FedTimeFactory
FedTime *
RTI::FedTimeFactory::makeZero()
    throw (RTI::MemoryExhausted)
{
    return new RTIfedTime();
}

FedTime *
RTI::FedTimeFactory::decode(const char *)
    throw (RTI::MemoryExhausted)
{
    throw RTIinternalError("Not implemented");
}

// ----------------------------------------------------------------------------
// FedTime
//FedTime::~FedTime()
//{
//}

// ----------------------------------------------------------------------------
// RTIfedTime
RTIfedTime::RTIfedTime()
    : _fedTime(0), _zero(0), _epsilon(0), _positiveInfinity(0) { }

RTIfedTime::RTIfedTime(const RTI::Double &time)
    : _fedTime(time), _zero(0), _epsilon(0), _positiveInfinity(0) { }

RTIfedTime::RTIfedTime(const FedTime &time)
    : _fedTime(rft(time).getTime()),
      _zero(0),
      _epsilon(0),
      _positiveInfinity(const_cast<FedTime &>(time).isPositiveInfinity()) { }

// ----------------------------------------------------------------------------
RTIfedTime::RTIfedTime(const RTIfedTime &time)
    : FedTime(),
      _fedTime(time._fedTime),
      _zero(time._zero),
      _epsilon(time._epsilon),
      _positiveInfinity(time._positiveInfinity) { }

// ----------------------------------------------------------------------------
RTIfedTime::~RTIfedTime()
{
}

// ----------------------------------------------------------------------------
void
RTIfedTime::setZero()
{
    _fedTime = 0 ;
    _zero = 0 ;
    _epsilon = 0 ;
    _positiveInfinity = 0 ;
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::isZero()
{
    return RTI::Boolean(_fedTime == 0.0);
}

// ----------------------------------------------------------------------------
void
RTIfedTime::setEpsilon()
{
    _fedTime = epsilon ;
}

// ----------------------------------------------------------------------------
void
RTIfedTime::setPositiveInfinity()
{
    _positiveInfinity = 1.0 ;
}

// ----------------------------------------------------------------------------
inline RTI::Boolean
RTIfedTime::isPositiveInfinity()
{
    return RTI::Boolean(_positiveInfinity != 0.0);
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
    stringstream s ;

    if (is_infinity(*this))
	s << infinity_str ;
    else
	s << _fedTime ;

    return s.str().length() + 1 ;
}

// ----------------------------------------------------------------------------
void
RTIfedTime::getPrintableString(char *str)
{
    stringstream s ;

    if (is_infinity(*this))
	s << infinity_str ;
    else
	s << _fedTime ;

    s >> str ;
    str[s.str().length()] = 0 ;
}

// ----------------------------------------------------------------------------
FedTime &
RTIfedTime::operator+=(const FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime += rft(time)._fedTime ;
    return *this;
}

// ----------------------------------------------------------------------------
FedTime &
RTIfedTime::operator-=(const FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime -= rft(time)._fedTime ;
    return *this;
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator<=(const FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    if (const_cast<FedTime &>(time).isPositiveInfinity())
	return RTI::RTI_TRUE ;
    else if (is_infinity(*this))
	return RTI::RTI_FALSE ;
    else
	return RTI::Boolean(_fedTime <= rft(time)._fedTime);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator<(const FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    if (is_infinity(*this))
	return RTI::RTI_FALSE ;
    else 
	return RTI::Boolean(is_infinity(time) || _fedTime < rft(time)._fedTime);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator>=(const FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    if (is_infinity(*this))
	return RTI::RTI_TRUE ;
    else if (is_infinity(time))
	return RTI::RTI_FALSE ;
    else
	return RTI::Boolean(_fedTime >= rft(time)._fedTime);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator>(const FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    if (is_infinity(time))
	return RTI::RTI_FALSE ;
    else
	return RTI::Boolean(is_infinity(*this) || _fedTime > rft(time)._fedTime);
}

// ----------------------------------------------------------------------------
inline RTI::Boolean
RTIfedTime::operator==(const FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    if (is_infinity(*this) && is_infinity(time))
	return RTI::RTI_TRUE ;
    else if (is_infinity(*this) || is_infinity(time))
	return RTI::RTI_FALSE ;
    else
	return RTI::Boolean(_fedTime == rft(time)._fedTime);
}

// ----------------------------------------------------------------------------
FedTime &
RTIfedTime::operator=(const FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime = rft(time)._fedTime ;
    _positiveInfinity = rft(time)._positiveInfinity ;
    return *this ;
}

// ----------------------------------------------------------------------------
RTI::Double
RTIfedTime::getTime() const
{
    return _fedTime ;
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator==(const Double &time) const
    throw (RTI::InvalidFederationTime)
{
    return RTI::Boolean(!is_infinity(*this) && _fedTime == time);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator!=(const FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    return RTI::Boolean(!(*this == time));
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator!=(const Double &time) const
    throw (RTI::InvalidFederationTime)
{
    return RTI::Boolean(is_infinity(*this) || _fedTime != time);
}

// ----------------------------------------------------------------------------
FedTime &
RTIfedTime::operator=(const RTIfedTime &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime = time._fedTime ;
    _positiveInfinity = time._positiveInfinity ;
    return *this ;
}

// ----------------------------------------------------------------------------
FedTime &
RTIfedTime::operator=(const Double &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime = time ;
    _positiveInfinity = 0.0 ;
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
std::ostream &
operator<<(std::ostream &s, const FedTime &time)
{
    if (is_infinity(time))
	s << infinity_str ;
    else
	s << rft(time).getTime();
    return s ;
}

// ============================================================================

// ----------------------------------------------------------------------------
RTIfedTime
operator+(const Double &d, const FedTime &time)
{
    RTIfedTime fedtime = d + rft(time).getTime();
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
operator-(const Double &d, const FedTime &time)
{
    RTIfedTime fedtime = d - rft(time).getTime();
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
operator*(const Double &d, const FedTime &time)
{
    RTIfedTime fedtime = d * rft(time).getTime();
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
operator/(const Double &d, const FedTime &time)
{
    RTIfedTime fedtime = d / rft(time).getTime();
    return fedtime ;
}
