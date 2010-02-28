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

#include <math.h>

#include "certi.hh"
#include "fedtime.hh"
#include "FedTimeD.hh"

#include <algorithm>
#include <limits>
#include <sstream>
#include <typeinfo>

using std::stringstream ;

namespace
{

// ----------------------------------------------------------------------------
/** FedTime to RTIfedTime reference cast
 */
inline const RTIfedTime &
rft(const RTI::FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    try {
	return dynamic_cast<const RTIfedTime &>(time);
    }
    catch (std::bad_cast) {
	//throw RTI::InvalidFederationTime("Could not cast to RTIfedTime");
    }
}

inline bool
is_infinity(const RTI::FedTime &time)
{
    return const_cast<RTI::FedTime &>(time).isPositiveInfinity() == RTI::RTI_TRUE ;
}

const char *infinity_str = "+inf" ;

} // anonymous namespace

// ----------------------------------------------------------------------------
// FedTimeFactory
RTI::FedTime *
RTI::FedTimeFactory::makeZero()
    throw (RTI::MemoryExhausted)
{
    try {
        return new RTIfedTime();
    }
    catch (std::bad_alloc) {		
        //throw RTI::MemoryExhausted("Cannot allocate RTI::FedTime.");
    }
}

RTI::FedTime *
RTI::FedTimeFactory::decode(const char *buf)
    throw (RTI::MemoryExhausted)
{
	union ud {
		double   dv;
		uint64_t uv;
	} value;
    try {
    	value.uv = CERTI_DECODE_DOUBLE_FROM_UINT64BE(buf);
        return new RTIfedTime(value.dv);
    }
    catch (std::bad_alloc) {
        //throw RTI::MemoryExhausted("Cannot allocate RTI::FedTime.");
    }
}

// ----------------------------------------------------------------------------
// RTIfedTime
RTIfedTime::RTIfedTime()
    : _fedTime(0),
      _zero(0),
      _epsilon(std::numeric_limits<double>::epsilon()),
      _positiveInfinity(std::numeric_limits<double>::infinity()) { }

RTIfedTime::RTIfedTime(const RTI::Double &time)
    : _fedTime(time),
      _zero(0),
      _epsilon(std::numeric_limits<double>::epsilon()),
      _positiveInfinity(std::numeric_limits<double>::infinity()) { }

RTIfedTime::RTIfedTime(const RTI::FedTime &time)
    : _fedTime(rft(time).getTime()),
      _zero(0),
      _epsilon(std::numeric_limits<double>::epsilon()),
      _positiveInfinity(std::numeric_limits<double>::infinity()) { }

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
    _fedTime = _zero;
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::isZero()
{
    return RTI::Boolean(_fedTime == _zero);
}

// ----------------------------------------------------------------------------
void
RTIfedTime::setEpsilon()
{
    _fedTime = _epsilon;
}

// ----------------------------------------------------------------------------
void
RTIfedTime::setPositiveInfinity()
{
    _fedTime = _positiveInfinity;
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::isPositiveInfinity()
{
    return RTI::Boolean(_fedTime == _positiveInfinity);
}

// ----------------------------------------------------------------------------
int
RTIfedTime::encodedLength() const
{
	// current implementation of RTIfedtime takes
	// four IEEE-754 double values:
	// _fedTime
	// _zero
	// _epsilon
	// _positiveInfinity
	// but we only transmit _fedTime because other value will
	// be reconstructed autonomously
	return (sizeof(double));
}

// ----------------------------------------------------------------------------
void
RTIfedTime::encode(char *buffer) const
{
#ifdef HOST_IS_BIG_ENDIAN
	memcpy(buffer, &_fedTime, sizeof(double));
#else
	uint64_t value;
	value = CERTI_ENCODE_DOUBLE_TO_UINT64BE(&_fedTime);
	memcpy(buffer,&value,sizeof(double));
#endif
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
RTI::FedTime &
RTIfedTime::operator+=(const RTI::FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime += rft(time)._fedTime ;
    return *this;
}

// ----------------------------------------------------------------------------
RTI::FedTime &
RTIfedTime::operator-=(const RTI::FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime -= rft(time)._fedTime ;
    return *this;
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator<=(const RTI::FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    if (const_cast<RTI::FedTime &>(time).isPositiveInfinity())
	return RTI::RTI_TRUE ;
    else if (is_infinity(*this))
	return RTI::RTI_FALSE ;
    else
	return RTI::Boolean(certi::fcmp(_fedTime, rft(time)._fedTime, _epsilon) <= 0);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator<(const RTI::FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    if (is_infinity(*this))
	return RTI::RTI_FALSE ;
    else
	return RTI::Boolean(is_infinity(time) || certi::fcmp(_fedTime, rft(time)._fedTime, _epsilon) < 0);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator>=(const RTI::FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    if (is_infinity(*this))
	return RTI::RTI_TRUE ;
    else if (is_infinity(time))
	return RTI::RTI_FALSE ;
    else
	return RTI::Boolean(certi::fcmp(_fedTime, rft(time)._fedTime, _epsilon) >= 0);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator>(const RTI::FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    if (is_infinity(time))
	return RTI::RTI_FALSE ;
    else
	return RTI::Boolean(is_infinity(*this) || certi::fcmp(_fedTime, rft(time)._fedTime, _epsilon) > 0);
}

// ----------------------------------------------------------------------------
inline RTI::Boolean
RTIfedTime::operator==(const RTI::FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    if (is_infinity(*this) && is_infinity(time))
	return RTI::RTI_TRUE ;
    else if (is_infinity(*this) || is_infinity(time))
	return RTI::RTI_FALSE ;
    else
	return RTI::Boolean(certi::fcmp(_fedTime, rft(time)._fedTime, _epsilon) == 0);
}

// ----------------------------------------------------------------------------
RTI::FedTime &
RTIfedTime::operator=(const RTI::FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime = rft(time)._fedTime ;
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
RTIfedTime::operator==(const RTI::Double &time) const
    throw (RTI::InvalidFederationTime)
{
    return RTI::Boolean(!is_infinity(*this) && certi::fcmp(_fedTime, time, _epsilon) == 0);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator!=(const RTI::FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    return RTI::Boolean(certi::fcmp(_fedTime, rft(time)._fedTime, _epsilon) != 0);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator!=(const RTI::Double &time) const
    throw (RTI::InvalidFederationTime)
{
    return RTI::Boolean(is_infinity(*this) || certi::fcmp(_fedTime, time, _epsilon) != 0);
}

// ----------------------------------------------------------------------------
RTI::FedTime &
RTIfedTime::operator=(const RTIfedTime &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime = time._fedTime ;
    return *this ;
}

// ----------------------------------------------------------------------------
RTI::FedTime &
RTIfedTime::operator=(const RTI::Double &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime = time ;
    return *this ;
}

RTI::FedTime &
RTIfedTime::operator*=(const RTI::FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime *= rft(time)._fedTime ;
    return *this ;
}

RTI::FedTime &
RTIfedTime::operator/=(const RTI::FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime /= rft(time)._fedTime ;
    return *this ;
}

RTI::FedTime &
RTIfedTime::operator+=(const RTI::Double &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime += time ;
    return *this ;
}

RTI::FedTime &
RTIfedTime::operator-=(const RTI::Double &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime -= time ;
    return *this ;
}

RTI::FedTime &
RTIfedTime::operator*=(const RTI::Double &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime *= time ;
    return *this ;
}

RTI::FedTime &
RTIfedTime::operator/=(const RTI::Double &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime /= time ;
    return *this ;
}

// ----------------------------------------------------------------------------
RTIfedTime
RTIfedTime::operator+(const RTI::FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime + rft(time)._fedTime ;
    return fedtime ;
}

RTIfedTime
RTIfedTime::operator+(const RTI::Double &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime + time ;
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
RTIfedTime::operator-(const RTI::FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime - rft(time)._fedTime ;
    return fedtime ;
}

RTIfedTime
RTIfedTime::operator-(const RTI::Double &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime - time ;
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
RTIfedTime::operator*(const RTI::FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime * rft(time)._fedTime ;
    return fedtime ;
}

RTIfedTime
RTIfedTime::operator*(const RTI::Double &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime * time ;
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
RTIfedTime::operator/(const RTI::FedTime &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime / rft(time)._fedTime ;
    return fedtime ;
}

RTIfedTime
RTIfedTime::operator/(const RTI::Double &time)
    throw (RTI::InvalidFederationTime)
{
    RTIfedTime fedtime = _fedTime / time ;
    return fedtime ;
}

// ----------------------------------------------------------------------------
std::ostream &
operator<<(std::ostream &s, const RTI::FedTime &time)
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
operator+(const RTI::Double &d, const RTI::FedTime &time)
{
    RTIfedTime fedtime = d + rft(time).getTime();
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
operator-(const RTI::Double &d, const RTI::FedTime &time)
{
    RTIfedTime fedtime = d - rft(time).getTime();
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
operator*(const RTI::Double &d, const RTI::FedTime &time)
{
    RTIfedTime fedtime = d * rft(time).getTime();
    return fedtime ;
}

// ----------------------------------------------------------------------------
RTIfedTime
operator/(const RTI::Double &d, const RTI::FedTime &time)
{
    RTIfedTime fedtime = d / rft(time).getTime();
    return fedtime ;
}
