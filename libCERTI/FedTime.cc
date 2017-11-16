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
//
// ----------------------------------------------------------------------------

#include <math.h>

#include "FedTimeD.hh"

#include <limits>
#include <sstream>

using namespace certi;

namespace {

const char* infinity_str = "+inf";

} // anonymous namespace

// ----------------------------------------------------------------------------
// FedTime

const double FedTime::epsilon = std::numeric_limits<double>::epsilon();

int FedTime::fcmp(const double x1, const double x2)
{
    return certi::fcmp(x1, x2, FedTime::epsilon);
}

// ----------------------------------------------------------------------------
// FedTime
FedTime::FedTime()
    : _fedTime(0), _zero(0), _epsilon(FedTime::epsilon), _positiveInfinity(std::numeric_limits<double>::infinity())
{
}

FedTime::FedTime(const double& time)
    : _fedTime(time), _zero(0), _epsilon(FedTime::epsilon), _positiveInfinity(std::numeric_limits<double>::infinity())
{
}

FedTime::FedTime(const FedTime& time)
    : _fedTime(time._fedTime), _zero(time._zero), _epsilon(time._epsilon), _positiveInfinity(time._positiveInfinity)
{
}

// ----------------------------------------------------------------------------
FedTime::~FedTime()
{
}

// ----------------------------------------------------------------------------
void FedTime::setZero()
{
    _fedTime = _zero;
}

// ----------------------------------------------------------------------------
bool FedTime::isZero() const
{
    return bool(_fedTime == _zero);
}

// ----------------------------------------------------------------------------
void FedTime::setEpsilon()
{
    _fedTime = _epsilon;
}

// ----------------------------------------------------------------------------
void FedTime::setPositiveInfinity()
{
    _fedTime = _positiveInfinity;
}

// ----------------------------------------------------------------------------
bool FedTime::isPositiveInfinity() const
{
    return bool(_fedTime == _positiveInfinity);
}

// ----------------------------------------------------------------------------
int FedTime::encodedLength() const
{
    // current implementation of FedTime takes
    // four IEEE-754 double values.
    return (sizeof(_fedTime) + sizeof(_zero) + sizeof(_epsilon) + sizeof(_positiveInfinity));
}

// ----------------------------------------------------------------------------
void FedTime::encode(char* /*buffer*/) const
{
    //	libhla::HLAfloat64LE encoded_fedTime;
    //	encoded_fedTime = _fedTime;
    //	strncpy(buffer,(char*)&(encoded_fedTime),encoded_fedTime.__sizeof());
}

// ----------------------------------------------------------------------------
int FedTime::getPrintableLength() const
{
    std::stringstream s;

    if (isPositiveInfinity())
        s << infinity_str;
    else
        s << _fedTime;

    return s.str().length() + 1;
}

// ----------------------------------------------------------------------------
void FedTime::getPrintableString(char* str)
{
    std::stringstream s;

    if (isPositiveInfinity())
        s << infinity_str;
    else
        s << _fedTime;

    s >> str;
    str[s.str().length()] = 0;
}

// ----------------------------------------------------------------------------
FedTime& FedTime::operator+=(const FedTime& time)
{
    _fedTime += time._fedTime;
    return *this;
}

// ----------------------------------------------------------------------------
FedTime& FedTime::operator-=(const FedTime& time)
{
    _fedTime -= time._fedTime;
    return *this;
}

// ----------------------------------------------------------------------------
bool FedTime::operator<=(const FedTime& time) const
{
    if (const_cast<FedTime&>(time).isPositiveInfinity())
        return true;
    else if (isPositiveInfinity())
        return false;
    else
        return bool(::fcmp(_fedTime, time._fedTime, _epsilon) <= 0);
}

// ----------------------------------------------------------------------------
bool FedTime::operator<(const FedTime& time) const
{
    if (isPositiveInfinity())
        return false;
    else
        return bool(time.isPositiveInfinity() || ::fcmp(_fedTime, time._fedTime, _epsilon) < 0);
}

// ----------------------------------------------------------------------------
bool FedTime::operator>=(const FedTime& time) const
{
    if (isPositiveInfinity())
        return true;
    else if (time.isPositiveInfinity())
        return false;
    else
        return bool(::fcmp(_fedTime, time._fedTime, _epsilon) >= 0);
}

// ----------------------------------------------------------------------------
bool FedTime::operator>(const FedTime& time) const
{
    if (time.isPositiveInfinity())
        return false;
    else
        return bool(isPositiveInfinity() || ::fcmp(_fedTime, time._fedTime, _epsilon) > 0);
}

// ----------------------------------------------------------------------------
bool FedTime::operator==(const FedTime& time) const
{
    if (isPositiveInfinity() && time.isPositiveInfinity())
        return true;
    else if (isPositiveInfinity() || time.isPositiveInfinity())
        return false;
    else
        return bool(::fcmp(_fedTime, time._fedTime, _epsilon) == 0);
}

// ----------------------------------------------------------------------------
double FedTime::getTime() const
{
    return _fedTime;
}

// ----------------------------------------------------------------------------
bool FedTime::operator==(const double& time) const
{
    return bool(!isPositiveInfinity() && ::fcmp(_fedTime, time, _epsilon) == 0);
}

// ----------------------------------------------------------------------------
bool FedTime::operator!=(const FedTime& time) const
{
    return bool(::fcmp(_fedTime, time._fedTime, _epsilon) != 0);
}

// ----------------------------------------------------------------------------
bool FedTime::operator!=(const double& time) const
{
    return bool(isPositiveInfinity() || ::fcmp(_fedTime, time, _epsilon) != 0);
}

// ----------------------------------------------------------------------------
FedTime& FedTime::operator=(const FedTime& time)
{
    _fedTime = time._fedTime;
    return *this;
}

// ----------------------------------------------------------------------------
FedTime& FedTime::operator=(const double& time)
{
    _fedTime = time;
    return *this;
}

FedTime& FedTime::operator*=(const FedTime& time)
{
    _fedTime *= time._fedTime;
    return *this;
}

FedTime& FedTime::operator/=(const FedTime& time)
{
    _fedTime /= time._fedTime;
    return *this;
}

FedTime& FedTime::operator+=(const double& time)
{
    _fedTime += time;
    return *this;
}

FedTime& FedTime::operator-=(const double& time)
{
    _fedTime -= time;
    return *this;
}

FedTime& FedTime::operator*=(const double& time)
{
    _fedTime *= time;
    return *this;
}

FedTime& FedTime::operator/=(const double& time)
{
    _fedTime /= time;
    return *this;
}

// ----------------------------------------------------------------------------
FedTime FedTime::operator+(const FedTime& time)
{
    FedTime fedtime = _fedTime + time._fedTime;
    return fedtime;
}

FedTime FedTime::operator+(const double& time)
{
    FedTime fedtime = _fedTime + time;
    return fedtime;
}

// ----------------------------------------------------------------------------
FedTime FedTime::operator-(const FedTime& time)
{
    FedTime fedtime = _fedTime - time._fedTime;
    return fedtime;
}

FedTime FedTime::operator-(const double& time)
{
    FedTime fedtime = _fedTime - time;
    return fedtime;
}

// ----------------------------------------------------------------------------
FedTime FedTime::operator*(const FedTime& time)
{
    FedTime fedtime = _fedTime * time._fedTime;
    return fedtime;
}

FedTime FedTime::operator*(const double& time)
{
    FedTime fedtime = _fedTime * time;
    return fedtime;
}

// ----------------------------------------------------------------------------
FedTime FedTime::operator/(const FedTime& time)
{
    FedTime fedtime = _fedTime / time._fedTime;
    return fedtime;
}

FedTime FedTime::operator/(const double& time)
{
    FedTime fedtime = _fedTime / time;
    return fedtime;
}

// ----------------------------------------------------------------------------
namespace certi {
std::ostream& operator<<(std::ostream& s, const FedTime& time)
{
    if (time.isPositiveInfinity())
        s << infinity_str;
    else
        s << time.getTime();
    return s;
}
}
// ============================================================================

// ----------------------------------------------------------------------------
FedTime operator+(const double& d, const FedTime& time)
{
    FedTime fedtime = d + time.getTime();
    return fedtime;
}

// ----------------------------------------------------------------------------
FedTime operator-(const double& d, const FedTime& time)
{
    FedTime fedtime = d - time.getTime();
    return fedtime;
}

// ----------------------------------------------------------------------------
FedTime operator*(const double& d, const FedTime& time)
{
    FedTime fedtime = d * time.getTime();
    return fedtime;
}

// ----------------------------------------------------------------------------
FedTime operator/(const double& d, const FedTime& time)
{
    FedTime fedtime = d / time.getTime();
    return fedtime;
}
