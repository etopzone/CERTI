// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2014  ONERA
//
// This file is part of CERTI-libRTI
//
// CERTI-libRTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libRTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// ----------------------------------------------------------------------------

#include <RTI/certiLogicalTime.h>
#include <RTI/certiLogicalTimeFactory.h>
#include <RTI/certiLogicalTimeInterval.h>

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <memory>

#include "RTI1516fedTime.h"
#include "certi.hh"

rti1516::LogicalTime& rti1516::LogicalTime::
operator=(rti1516::LogicalTime const& /*value*/) throw(rti1516::InvalidLogicalTime)
{
    throw std::wstring(L"Operator should be overloaded");
    //throw rti1516::InvalidLogicalTime(L"Operator should be overloaded");
}

rti1516::LogicalTimeInterval& rti1516::LogicalTimeInterval::
operator=(rti1516::LogicalTimeInterval const& /*value*/) throw(rti1516::InvalidLogicalTimeInterval)
{
    throw std::wstring(L"Operator should be overloaded");
    //throw rti1516::InvalidLogicalTimeInterval(L"Operator should be overloaded");
}

/////////////////////////////////////////////////////////////
//------------------- RTI1516fedTime ----------------------//
/////////////////////////////////////////////////////////////

RTI1516fedTime::RTI1516fedTime(double timeVal)
    : _fedTime(timeVal)
    , _zero(0)
    , _epsilon(std::numeric_limits<double>::epsilon())
    , _positiveInfinity(std::numeric_limits<double>::infinity())
{
}

RTI1516fedTime::RTI1516fedTime(const RTI1516fedTime& other)
    : _fedTime(other._fedTime), _zero(other._zero), _epsilon(other._epsilon), _positiveInfinity(other._positiveInfinity)
{
}

RTI1516fedTime::~RTI1516fedTime() throw()
{
}

void RTI1516fedTime::setInitial()
{
    _fedTime = (0);
    _zero = (0);
    _epsilon = (std::numeric_limits<double>::epsilon());
    _positiveInfinity = (std::numeric_limits<double>::infinity());
}

bool RTI1516fedTime::isInitial() const
{
    return (_fedTime == _zero);
}

void RTI1516fedTime::setFinal()
{
    _fedTime = (0);
    _zero = (0);
    _epsilon = (std::numeric_limits<double>::epsilon());
    _positiveInfinity = (std::numeric_limits<double>::infinity());
}

bool RTI1516fedTime::isFinal() const
{
    return (_fedTime == _zero);
}

rti1516::LogicalTime& RTI1516fedTime::operator=(rti1516::LogicalTime const& value) throw(rti1516::InvalidLogicalTime)
{
    if (this != &value) {
        const RTI1516fedTime* other = dynamic_cast<const RTI1516fedTime*>(&value);
        if (other == NULL) {
#if defined(_WIN32)
            throw std::wstring(L"Different LogicalTime implementation");
//throw rti1516::InvalidLogicalTime(L"Different LogicalTime implementation");
#else
//throw rti1516::InvalidLogicalTime(L"Different LogicalTime implementation");
#endif
        }
        else {
            _fedTime = other->_fedTime;
            _zero = other->_zero;
            _epsilon = other->_epsilon;
            _positiveInfinity = other->_positiveInfinity;
        }
    }

    return *this;
}

RTI1516fedTime& RTI1516fedTime::operator=(RTI1516fedTime const& other) throw()
{
    if (this != &other) {
        _fedTime = other._fedTime;
        _zero = other._zero;
        _epsilon = other._epsilon;
        _positiveInfinity = other._positiveInfinity;
    }
    return *this;
}

rti1516::LogicalTime& RTI1516fedTime::
operator+=(rti1516::LogicalTimeInterval const& value) throw(rti1516::IllegalTimeArithmetic,
                                                            rti1516::InvalidLogicalTimeInterval)
{
    const RTI1516fedTimeInterval* other = dynamic_cast<const RTI1516fedTimeInterval*>(&value);
    if (other == NULL) {
#if defined(_WIN32)
        throw std::wstring(L"Different LogicalTime implementation");
#else
//throw rti1516::InvalidLogicalTime(L"Different LogicalTime implementation");
#endif
    }
    else {
        _fedTime += other->getInterval();
    }

    return *this;
}

rti1516::LogicalTime& RTI1516fedTime::
operator-=(rti1516::LogicalTimeInterval const& value) throw(rti1516::IllegalTimeArithmetic,
                                                            rti1516::InvalidLogicalTimeInterval)
{
    const RTI1516fedTimeInterval* other = dynamic_cast<const RTI1516fedTimeInterval*>(&value);
    if (other == NULL) {
#if defined(_WIN32)
        throw std::wstring(L"Different LogicalTime implementation");
#else
//throw rti1516::InvalidLogicalTime(L"Different LogicalTime implementation");
#endif
    }
    else {
        _fedTime -= other->getInterval();
    }

    return *this;
}

bool RTI1516fedTime::operator>(rti1516::LogicalTime const& value) const throw(rti1516::InvalidLogicalTime)
{
    const RTI1516fedTime* other = dynamic_cast<const RTI1516fedTime*>(&value);
    if (other == NULL) {
#if defined(_WIN32)
        throw std::wstring(L"Different LogicalTime implementation");
#else
//throw rti1516::InvalidLogicalTime(L"Different LogicalTime implementation");
#endif
    }
    else {
        return _fedTime > other->_fedTime;
    }
    return false;
}

bool RTI1516fedTime::operator<(rti1516::LogicalTime const& value) const throw(rti1516::InvalidLogicalTime)
{
    const RTI1516fedTime* other = dynamic_cast<const RTI1516fedTime*>(&value);
    if (other == NULL) {
#if defined(_WIN32)
        throw std::wstring(L"Different LogicalTime implementation");
#else
//throw rti1516::InvalidLogicalTime(L"Different LogicalTime implementation");
#endif
    }
    else {
        return _fedTime < other->_fedTime;
    }
    return false;
}

bool RTI1516fedTime::operator==(rti1516::LogicalTime const& value) const throw(rti1516::InvalidLogicalTime)
{
    const RTI1516fedTime* other = dynamic_cast<const RTI1516fedTime*>(&value);
    if (other == NULL) {
#if defined(_WIN32)
        throw std::wstring(L"Different LogicalTime implementation");
#else
//throw rti1516::InvalidLogicalTime(L"Different LogicalTime implementation");
#endif
    }
    else {
        // TODO Should use epsilon during comparison
        return _fedTime == other->_fedTime;
    }
    return false;
}

bool RTI1516fedTime::operator>=(rti1516::LogicalTime const& value) const throw(rti1516::InvalidLogicalTime)
{
    const RTI1516fedTime* other = dynamic_cast<const RTI1516fedTime*>(&value);
    if (other == NULL) {
#if defined(_WIN32)
        throw std::wstring(L"Different LogicalTime implementation");
#else
//throw rti1516::InvalidLogicalTime(L"Different LogicalTime implementation");
#endif
    }
    else {
        return _fedTime >= other->_fedTime;
    }
    return false;
}

bool RTI1516fedTime::operator<=(rti1516::LogicalTime const& value) const throw(rti1516::InvalidLogicalTime)
{
    const RTI1516fedTime* other = dynamic_cast<const RTI1516fedTime*>(&value);
    if (other == NULL) {
#if defined(_WIN32)
        throw std::wstring(L"Different LogicalTime implementation");
#else
//throw rti1516::InvalidLogicalTime(L"Different LogicalTime implementation");
#endif
    }
    else {
        return _fedTime <= other->_fedTime;
    }
    return false;
}

// Generates an encoded value that can be used to send
// LogicalTimes to other federates in updates or interactions

rti1516::VariableLengthData RTI1516fedTime::encode() const
{
    uint64_t value;
#ifdef HOST_IS_BIG_ENDIAN
    memcpy(&value, &_fedTime, sizeof(double));
#else
    value = CERTI_ENCODE_DOUBLE_TO_UINT64BE(&_fedTime);
#endif
    return rti1516::VariableLengthData(&value, sizeof(uint64_t));
}

// Alternate encode for directly filling a buffer

unsigned long RTI1516fedTime::encodedLength() const
{
    return sizeof(double);
}

unsigned long RTI1516fedTime::encode(void* buffer, unsigned long bufferSize) const throw(rti1516::CouldNotEncode)
{
    if (bufferSize >= sizeof(double)) {
#ifdef HOST_IS_BIG_ENDIAN
        memcpy(buffer, &_fedTime, sizeof(double));
#else
        uint64_t value;
        value = CERTI_ENCODE_DOUBLE_TO_UINT64BE(&_fedTime);
        memcpy(buffer, &value, sizeof(double));
#endif
        return sizeof(double);
    }
    else {
#if defined(_WIN32)
        throw std::wstring(L"Not enough space in buffer to encode RTI1516fedTime");
#else
        return 0;
//throw rti1516::CouldNotEncode(L"Not enough space in buffer to encode RTI1516fedTime");
#endif
    }
}

// Decode encodedLogicalTime into self

void RTI1516fedTime::decode(rti1516::VariableLengthData const& encodedLogicalTime) throw(rti1516::InternalError,
                                                                                         rti1516::CouldNotDecode)
{
    union ud {
        double dv;
        uint64_t uv;
    } value;
#ifdef HOST_IS_BIG_ENDIAN
    memcpy(&(value.uv), encodedLogicalTime.data(), sizeof(double));
#else
    value.uv = CERTI_DECODE_DOUBLE_FROM_UINT64BE(encodedLogicalTime.data());
#endif
    _fedTime = value.dv;
}

// Alternate decode that reads directly from a buffer

void RTI1516fedTime::decode(void* buffer, unsigned long bufferSize) throw(rti1516::InternalError,
                                                                          rti1516::CouldNotDecode)
{
    if (bufferSize >= sizeof(double)) {
        union ud {
            double dv;
            uint64_t uv;
        } value;
        value.uv = CERTI_DECODE_DOUBLE_FROM_UINT64BE(buffer);
        _fedTime = value.dv;
    }
    else {
#if defined(_WIN32) && defined(_MSC_VER)
        throw std::wstring(L"Not enough space in buffer to decode RTI1516fedTime");
#else
//throw rti1516::CouldNotEncode(L"Not enough space in buffer to decode RTI1516fedTime");
#endif
    }
}

std::wstring RTI1516fedTime::toString() const
{
    std::wstringstream stream;

    stream << _fedTime;

    return stream.str();
}

// Returns the name of the implementation, as needed by
// createFederationExecution.

std::wstring RTI1516fedTime::implementationName() const
{
    static std::wstring implName(L"certiFedTime1516");
    return implName;
}

/////////////////////////////////////////////////////////////
//--------------- RTI1516fedTimeInterval ------------------//
/////////////////////////////////////////////////////////////

RTI1516fedTimeInterval::RTI1516fedTimeInterval(double timeVal)
    : _fedInterval(timeVal)
    , _zero(0)
    , _epsilon(std::numeric_limits<double>::epsilon())
    , _positiveInfinity(std::numeric_limits<double>::infinity())
{
}

RTI1516fedTimeInterval::RTI1516fedTimeInterval(const RTI1516fedTimeInterval& other)
    : _fedInterval(other._fedInterval)
    , _zero(other._zero)
    , _epsilon(other._epsilon)
    , _positiveInfinity(other._positiveInfinity)
{
}

RTI1516fedTimeInterval::~RTI1516fedTimeInterval() throw()
{
}

void RTI1516fedTimeInterval::setZero()
{
    _fedInterval = _zero;
}

bool RTI1516fedTimeInterval::isZero() const
{
    return _fedInterval == _zero;
}

void RTI1516fedTimeInterval::setEpsilon()
{
    _epsilon = (std::numeric_limits<double>::epsilon());
}

bool RTI1516fedTimeInterval::isEpsilon() const
{
    return std::abs(_fedInterval) <= std::abs(_epsilon);
}

rti1516::LogicalTimeInterval& RTI1516fedTimeInterval::
operator=(rti1516::LogicalTimeInterval const& value) throw(rti1516::InvalidLogicalTimeInterval)
{
    if (this != &value) {
        const RTI1516fedTimeInterval* other = dynamic_cast<const RTI1516fedTimeInterval*>(&value);
        if (other == NULL) {
#if defined(_WIN32) && defined(_MSC_VER)
            throw std::wstring(L"Different LogicalTimeInterval implementation");
#else
//throw rti1516::InvalidLogicalTimeInterval(L"Different LogicalTimeInterval implementation");
#endif
        }
        else {
            _fedInterval = other->_fedInterval;
            _zero = other->_zero;
            _epsilon = other->_epsilon;
            _positiveInfinity = other->_positiveInfinity;
        }
    }

    return *this;
}

RTI1516fedTimeInterval& RTI1516fedTimeInterval::operator=(RTI1516fedTimeInterval const& other) throw()
{
    if (this != &other) {
        _fedInterval = other._fedInterval;
        _zero = other._zero;
        _epsilon = other._epsilon;
        _positiveInfinity = other._positiveInfinity;
    }
    return *this;
}

// Set self to the difference between two LogicalTimes

void RTI1516fedTimeInterval::setToDifference(rti1516::LogicalTime const& minuend,
                                             rti1516::LogicalTime const& subtrahend) throw(rti1516::InvalidLogicalTime)
{
    const RTI1516fedTime* val1 = dynamic_cast<const RTI1516fedTime*>(&minuend);
    const RTI1516fedTime* val2 = dynamic_cast<const RTI1516fedTime*>(&subtrahend);

    if (val1 == 0 && val2 == 0) {
        // Both wrong
        std::wstring message(L"First and second argument are different LogicalTime implementations.");
#if defined(_WIN32) && defined(_MSC_VER)
        throw message;
#else
//throw rti1516::InvalidLogicalTime(message);
#endif
    }
    else if (val1 == 0) {
        // first wrong
        std::wstring message(L"First argument uses a different LogicalTime implementations.");
#if defined(_WIN32) && defined(_MSC_VER)
        throw message;
#else
//throw rti1516::InvalidLogicalTime(message);
#endif
    }
    else if (val2 == 0) {
        // second wrong
        std::wstring message(L"Second argument uses a different LogicalTime implementations.");
#if defined(_WIN32) && defined(_MSC_VER)
        throw message;
#else
//throw rti1516::InvalidLogicalTime(message);
#endif
    }
    else {
        // Both correct
        // TODO Should handle infinity?
        _fedInterval = val1->getFedTime() - val2->getFedTime();
    }
} /* end of RTI1516fedTimeInterval::setToDifference */

rti1516::LogicalTimeInterval& RTI1516fedTimeInterval::
operator+=(rti1516::LogicalTimeInterval const& value) throw(rti1516::InvalidLogicalTimeInterval)
{
    const RTI1516fedTimeInterval* other = dynamic_cast<const RTI1516fedTimeInterval*>(&value);
    if (other == NULL) {
#if defined(_WIN32) && defined(_MSC_VER)
        throw std::wstring(L"Different LogicalTimeInterval implementation");
#else
//throw rti1516::InvalidLogicalTimeInterval(L"Different LogicalTimeInterval implementation");
#endif
    }
    else {
        _fedInterval += other->getInterval();
    }
    return *this;
}

rti1516::LogicalTimeInterval& RTI1516fedTimeInterval::
operator-=(rti1516::LogicalTimeInterval const& value) throw(rti1516::InvalidLogicalTimeInterval)
{
    const RTI1516fedTimeInterval* other = dynamic_cast<const RTI1516fedTimeInterval*>(&value);
    if (other == NULL) {
#if defined(_WIN32) && defined(_MSC_VER)
        throw std::wstring(L"Different LogicalTimeInterval implementation");
#else
//throw rti1516::InvalidLogicalTimeInterval(L"Different LogicalTimeInterval implementation");
#endif
    }
    else {
        _fedInterval -= other->getInterval();
    }
    return *this;
}

bool RTI1516fedTimeInterval::operator>(rti1516::LogicalTimeInterval const& value) const
    throw(rti1516::InvalidLogicalTimeInterval)
{
    const RTI1516fedTimeInterval* other = dynamic_cast<const RTI1516fedTimeInterval*>(&value);
    if (other == NULL) {
#if defined(_WIN32) && defined(_MSC_VER)
        throw std::wstring(L"Different LogicalTimeInterval implementation");
#else
//throw rti1516::InvalidLogicalTimeInterval(L"Different LogicalTimeInterval implementation");
#endif
    }
    else {
        return getInterval() > other->getInterval();
    }
    return false;
} /* end of RTI1516fedTimeInterval::operator> */

bool RTI1516fedTimeInterval::operator<(rti1516::LogicalTimeInterval const& value) const
    throw(rti1516::InvalidLogicalTimeInterval)
{
    const RTI1516fedTimeInterval* other = dynamic_cast<const RTI1516fedTimeInterval*>(&value);
    if (other == NULL) {
#if defined(_WIN32) && defined(_MSC_VER)
        throw std::wstring(L"Different LogicalTimeInterval implementation");
#else
//throw rti1516::InvalidLogicalTimeInterval(L"Different LogicalTimeInterval implementation");
#endif
    }
    else {
        return getInterval() > other->getInterval();
    }
    return false;
}

bool RTI1516fedTimeInterval::operator==(rti1516::LogicalTimeInterval const& value) const
    throw(rti1516::InvalidLogicalTimeInterval)
{
    const RTI1516fedTimeInterval* other = dynamic_cast<const RTI1516fedTimeInterval*>(&value);
    if (other == NULL) {
#if defined(_WIN32) && defined(_MSC_VER)
        throw std::wstring(L"Different LogicalTimeInterval implementation");
#else
//throw rti1516::InvalidLogicalTimeInterval(L"Different LogicalTimeInterval implementation");
#endif
    }
    else {
        return getInterval() == other->getInterval();
    }
    return false;
}

bool RTI1516fedTimeInterval::operator>=(rti1516::LogicalTimeInterval const& value) const
    throw(rti1516::InvalidLogicalTimeInterval)
{
    const RTI1516fedTimeInterval* other = dynamic_cast<const RTI1516fedTimeInterval*>(&value);
    if (other == NULL) {
#if defined(_WIN32) && defined(_MSC_VER)
        throw std::wstring(L"Different LogicalTimeInterval implementation");
#else
//throw rti1516::InvalidLogicalTimeInterval(L"Different LogicalTimeInterval implementation");
#endif
    }
    else {
        return getInterval() >= other->getInterval();
    }
    return false;
}

bool RTI1516fedTimeInterval::operator<=(rti1516::LogicalTimeInterval const& value) const
    throw(rti1516::InvalidLogicalTimeInterval)
{
    const RTI1516fedTimeInterval* other = dynamic_cast<const RTI1516fedTimeInterval*>(&value);
    if (other == NULL) {
#if defined(_WIN32) && defined(_MSC_VER)
        throw std::wstring(L"Different LogicalTimeInterval implementation");
#else
//throw rti1516::InvalidLogicalTimeInterval(L"Different LogicalTimeInterval implementation");
#endif
    }
    else {
        return getInterval() <= other->getInterval();
    }
    return false;
}

// Generates an encoded value that can be used to send
// LogicalTimeIntervals to other federates in updates or interactions

rti1516::VariableLengthData RTI1516fedTimeInterval::encode() const
{
    uint64_t value;
#ifdef HOST_IS_BIG_ENDIAN
    memcpy(&value, &_fedTime, sizeof(double));
#else
    value = CERTI_ENCODE_DOUBLE_TO_UINT64BE(&_fedInterval);
#endif
    return rti1516::VariableLengthData(&value, sizeof(uint64_t));
}

// Alternate encode for directly filling a buffer

unsigned long RTI1516fedTimeInterval::encodedLength() const
{
    return sizeof(double);
}

unsigned long RTI1516fedTimeInterval::encode(void* buffer, unsigned long bufferSize) const
    throw(rti1516::CouldNotEncode)
{
    if (bufferSize >= sizeof(double)) {
#ifdef HOST_IS_BIG_ENDIAN
        memcpy(buffer, &_fedInterval, sizeof(double));
#else
        uint64_t value;
        value = CERTI_ENCODE_DOUBLE_TO_UINT64BE(&_fedInterval);
        memcpy(buffer, &value, sizeof(double));
#endif
        return sizeof(double);
    }
    else {
#if defined(_WIN32) && defined(_MSC_VER)
        throw std::wstring(L"Not enough space in buffer to encode RTI1516fedTimeInterval");
#else
//throw rti1516::CouldNotEncode(L"Not enough space in buffer to encode RTI1516fedTimeInterval");
#endif
    }
    return false;
}

// Decode encodedValue into self

void RTI1516fedTimeInterval::decode(rti1516::VariableLengthData const& encodedValue) throw(rti1516::InternalError,
                                                                                           rti1516::CouldNotDecode)
{
    union ud {
        double dv;
        uint64_t uv;
    } value;
#ifdef HOST_IS_BIG_ENDIAN
    memcpy(&(value.uv), encodedValue.data(), sizeof(double));
#else
    value.uv = CERTI_DECODE_DOUBLE_FROM_UINT64BE(encodedValue.data());
#endif
    _fedInterval = value.dv;
}

// Alternate decode that reads directly from a buffer

void RTI1516fedTimeInterval::decode(void* buffer, unsigned long bufferSize) throw(rti1516::InternalError,
                                                                                  rti1516::CouldNotDecode)
{
    if (bufferSize >= sizeof(double)) {
        union ud {
            double dv;
            uint64_t uv;
        } value;
        value.uv = CERTI_DECODE_DOUBLE_FROM_UINT64BE(buffer);
        _fedInterval = value.dv;
    }
    else {
#if defined(_WIN32) && defined(_MSC_VER)
        throw std::wstring(L"Not enough space in buffer to decode RTI1516fedTimeInterval");
#else
//throw rti1516::CouldNotEncode(L"Not enough space in buffer to decode RTI1516fedTimeInterval");
#endif
    }
}

std::wstring RTI1516fedTimeInterval::toString() const
{
    std::wstringstream stream;
    stream << _fedInterval;

    return stream.str();
}

// Returns the name of the implementation, as needed by
// createFederationExecution.

std::wstring RTI1516fedTimeInterval::implementationName() const
{
    static std::wstring implName(L"certiFedTime1516");
    return implName;
}

/////////////////////////////////////////////////////////////
//---------------- RTI1516fedTimeFactory ------------------//
/////////////////////////////////////////////////////////////

RTI1516fedTimeFactory::RTI1516fedTimeFactory() throw()
{
}

RTI1516fedTimeFactory::~RTI1516fedTimeFactory() throw()
{
}

// Returns a LogicalTime with a value of "initial"

std::auto_ptr<rti1516::LogicalTime> RTI1516fedTimeFactory::makeLogicalTime() throw(rti1516::InternalError)
{
    RTI1516fedTime* fedTime = new RTI1516fedTime(0);

    return std::auto_ptr<rti1516::LogicalTime>(fedTime);
}

std::auto_ptr<rti1516::LogicalTime>
RTI1516fedTimeFactory::makeLogicalTime(double timeVal) throw(rti1516::InternalError)
{
    RTI1516fedTime* fedTime = new RTI1516fedTime(timeVal);

    return std::auto_ptr<rti1516::LogicalTime>(fedTime);
}

// Returns a LogicalTimeInterval with a value of "zero"

std::auto_ptr<rti1516::LogicalTimeInterval>
RTI1516fedTimeFactory::makeLogicalTimeInterval() throw(rti1516::InternalError)
{
    RTI1516fedTimeInterval* fedTimeInterval = new RTI1516fedTimeInterval(0);

    return std::auto_ptr<rti1516::LogicalTimeInterval>(fedTimeInterval);
}

std::auto_ptr<rti1516::LogicalTimeInterval>
RTI1516fedTimeFactory::makeLogicalTimeInterval(double timeInterval) throw(rti1516::InternalError)
{
    RTI1516fedTimeInterval* fedTimeInterval = new RTI1516fedTimeInterval(timeInterval);

    return std::auto_ptr<rti1516::LogicalTimeInterval>(fedTimeInterval);
}

std::auto_ptr<rti1516::LogicalTimeFactory>
rti1516::LogicalTimeFactoryFactory::makeLogicalTimeFactory(std::wstring const& /*implementationName*/)
{
    RTI1516fedTimeFactory* fedTimeFactory = new RTI1516fedTimeFactory();

    return std::auto_ptr<rti1516::LogicalTimeFactory>(fedTimeFactory);
}
