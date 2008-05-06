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

/*
 fcmp
 Copyright (c) 1998-2000 Theodore C. Belding
 University of Michigan Center for the Study of Complex Systems
 <mailto:Ted.Belding@umich.edu>
 <http://fcmp.sourceforge.net>    
  
 This file is part of the fcmp distribution. fcmp is free software;
 you can redistribute and modify it under the terms of the GNU Library
 General Public License (LGPL), version 2 or later.  This software
 comes with absolutely no warranty. See the file COPYING for details
 and terms of copying.

 Description:
 
 Knuth's floating point comparison operators, from:
 Knuth, D. E. (1998). The Art of Computer Programming.
 Volume 2: Seminumerical Algorithms. 3rd ed. Addison-Wesley.
 Section 4.2.2, p. 233. ISBN 0-201-89684-2.

 Input parameters:
 x1, x2: numbers to be compared
 epsilon: determines tolerance

 epsilon should be carefully chosen based on the machine's precision,
 the observed magnitude of error, the desired precision, and the
 magnitude of the numbers to be compared. See the fcmp README file for
 more information.

 This routine may be used for both single-precision (float) and
 double-precision (double) floating-point numbers.
 
 Returns:
 -1 if x1 < x2
  0 if x1 == x2
  1 if x1 > x2
*/

inline int
fcmp(double x1,double x2, double epsilon)
{
    int exponent;
    double delta;
    double difference;
    
    /* Get exponent(max(fabs(x1), fabs(x2))) and store it in exponent. */

    /* If neither x1 nor x2 is 0, */
    /* this is equivalent to max(exponent(x1), exponent(x2)). */

    /* If either x1 or x2 is 0, its exponent returned by frexp would be 0, */
    /* which is much larger than the exponents of numbers close to 0 in */
    /* magnitude. But the exponent of 0 should be less than any number */
    /* whose magnitude is greater than 0. */
    
    /* So we only want to set exponent to 0 if both x1 and */
    /* x2 are 0. Hence, the following works for all x1 and x2. */

    frexp(fabs(x1) > fabs(x2) ? x1 : x2, &exponent);

    /* Do the comparison. */

    /* delta = epsilon * pow(2, exponent) */

    /* Form a neighborhood around x2 of size delta in either direction. */
    /* If x1 is within this delta neighborhood of x2, x1 == x2. */
    /* Otherwise x1 > x2 or x1 < x2, depending on which side of */
    /* the neighborhood x1 is on. */
    
    delta = ldexp(epsilon, exponent); 
    
    difference = x1 - x2;

    if (difference > delta)
        return 1; /* x1 > x2 */
    else if (difference < -delta) 
        return -1;  /* x1 < x2 */
    else /* -delta <= difference <= delta */
        return 0;  /* x1 == x2 */
}

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

const double RTIfedTime::epsilon = std::numeric_limits<double>::epsilon();

int
RTIfedTime::fcmp(const double x1, const double x2) {
	return ::fcmp(x1,x2,RTIfedTime::epsilon); 
}

// ----------------------------------------------------------------------------
// RTIfedTime
RTIfedTime::RTIfedTime()
    : _fedTime(0),
      _zero(0),
      _epsilon(RTIfedTime::epsilon),
      _positiveInfinity(std::numeric_limits<double>::infinity()) { }

RTIfedTime::RTIfedTime(const RTI::Double &time)
    : _fedTime(time),
      _zero(0),
      _epsilon(RTIfedTime::epsilon),
      _positiveInfinity(std::numeric_limits<double>::infinity()) { }

RTIfedTime::RTIfedTime(const FedTime &time)
    : _fedTime(rft(time).getTime()),
      _zero(0),
      _epsilon(RTIfedTime::epsilon),
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
inline RTI::Boolean
RTIfedTime::isPositiveInfinity()
{
    return RTI::Boolean(_fedTime == _positiveInfinity);
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
	return RTI::Boolean(::fcmp(_fedTime, rft(time)._fedTime, _epsilon) <= 0);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator<(const FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    if (is_infinity(*this))
	return RTI::RTI_FALSE ;
    else 
	return RTI::Boolean(is_infinity(time) || ::fcmp(_fedTime, rft(time)._fedTime, _epsilon) < 0);
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
	return RTI::Boolean(::fcmp(_fedTime, rft(time)._fedTime, _epsilon) >= 0);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator>(const FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    if (is_infinity(time))
	return RTI::RTI_FALSE ;
    else
	return RTI::Boolean(is_infinity(*this) || ::fcmp(_fedTime, rft(time)._fedTime, _epsilon) > 0);
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
	return RTI::Boolean(::fcmp(_fedTime, rft(time)._fedTime, _epsilon) == 0);
}

// ----------------------------------------------------------------------------
FedTime &
RTIfedTime::operator=(const FedTime &time)
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
RTIfedTime::operator==(const Double &time) const
    throw (RTI::InvalidFederationTime)
{
    return RTI::Boolean(!is_infinity(*this) && ::fcmp(_fedTime, time, _epsilon) == 0);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator!=(const FedTime &time) const
    throw (RTI::InvalidFederationTime)
{
    return RTI::Boolean(::fcmp(_fedTime, rft(time)._fedTime, _epsilon) != 0);
}

// ----------------------------------------------------------------------------
RTI::Boolean
RTIfedTime::operator!=(const Double &time) const
    throw (RTI::InvalidFederationTime)
{
    return RTI::Boolean(is_infinity(*this) || ::fcmp(_fedTime, time, _epsilon) != 0);
}

// ----------------------------------------------------------------------------
FedTime &
RTIfedTime::operator=(const RTIfedTime &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime = time._fedTime ;
    return *this ;
}

// ----------------------------------------------------------------------------
FedTime &
RTIfedTime::operator=(const Double &time)
    throw (RTI::InvalidFederationTime)
{
    _fedTime = time ;
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
