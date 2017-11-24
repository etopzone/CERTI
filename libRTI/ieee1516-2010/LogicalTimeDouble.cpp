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
#if defined(_WIN32) && defined(_MSC_VER)
#pragma warning(disable : 4786)
#pragma warning(disable:4290)
#endif

#include "RTI/certiRTI1516.h"
//#include "RTI/EncodedLogicalTime.h"
//#include "RTI/EncodedLogicalTimeInterval.h"

#include "LogicalTimeDouble.h"

#include <cmath>
#include <climits>
#include <typeinfo>

/*
namespace
{
   class MyEncodedLogicalTime : public rti1516e::EncodedLogicalTime
   {
   private:
     rti1516e::VariableLengthValueClass _value;
     void* _data;
     size_t _size;
   public:
      MyEncodedLogicalTime(const void* data, size_t size) :
        _value(data, size)
      {
      }
      virtual ~MyEncodedLogicalTime() throw () {}

      virtual void const * data() const
      {
         return _value.data();
      }
      virtual size_t size() const
      {
         return _value.size();
      }
   };

   class MyEncodedLogicalTimeInterval : public rti1516e::EncodedLogicalTimeInterval
   {
   private:
      rti1516e::VariableLengthValueClass _value;
      void* _data;
      size_t _size;
   public:
      MyEncodedLogicalTimeInterval(const void* data, size_t size) :
         _value(data, size)
      {
      }
      virtual ~MyEncodedLogicalTimeInterval() throw () {}

      virtual void const * data() const
      {
         return _value.data();
      }
      virtual size_t size() const
      {
         return _value.size();
      }
   };
}
*/

#ifdef __GNUC__
const int64_t MAX_VALUE = LONG_LONG_MAX;
#else
const int64_t MAX_VALUE = 9223372036854775807;
#endif
const int64_t MULTIPLIER = 1000000;

LogicalTimeDouble::LogicalTimeDouble(double value)
{
   int64_t seconds = (long)floor(value);
   int64_t micros = (long)fmod(value * MULTIPLIER, (double)MULTIPLIER);

   _value = seconds * MULTIPLIER + micros;
}

LogicalTimeDouble::LogicalTimeDouble(int64_t value) :
   _value(value)
{
}

LogicalTimeDouble::~LogicalTimeDouble()
   throw ()
{
}

void LogicalTimeDouble::setInitial()
{
   _value = 0;
}

bool LogicalTimeDouble::isInitial() const
{
   return _value == 0;
}

void LogicalTimeDouble::setFinal()
{
   _value = MAX_VALUE;
}

bool LogicalTimeDouble::isFinal() const
{
   return _value == MAX_VALUE;
}

void LogicalTimeDouble::setTo(rti1516e::LogicalTime const & value)
throw (rti1516e::InvalidLogicalTime)
{
	try {
		const LogicalTimeDouble& p = dynamic_cast<const LogicalTimeDouble&>(value);
		_value = p._value;
	} catch (std::bad_cast)
	{
	    throw std::wstring(L"Invalid LogicalTimeDouble");
		//throw rti1516e::InvalidLogicalTime(L"Invalid LogicalTimeDouble");
	}
}

rti1516e::LogicalTime &
LogicalTimeDouble::operator=(rti1516e::LogicalTime const & value)
throw (rti1516e::InvalidLogicalTime)
{
	setTo(value);
	return *this;
}

LogicalTimeDouble &
LogicalTimeDouble::operator=(LogicalTimeDouble const & value)
throw (rti1516e::InvalidLogicalTime)
{
	setTo(value);
	return *this;
}





void LogicalTimeDouble::increaseBy(rti1516e::LogicalTimeInterval const & addend)
throw (rti1516e::IllegalTimeArithmetic, rti1516e::InvalidLogicalTimeInterval)
{
	try {
		const LogicalTimeIntervalDouble& p = dynamic_cast<const LogicalTimeIntervalDouble&>(addend);
		_value += p._value;
	} catch (std::bad_cast)
	{
	    throw std::wstring(L"Invalid LogicalTimeDouble");
		//throw rti1516e::InvalidLogicalTimeInterval(L"Invalid LogicalTimeDouble");
	}
}

void LogicalTimeDouble::decreaseBy(rti1516e::LogicalTimeInterval const & subtrahend)
throw (rti1516e::IllegalTimeArithmetic, rti1516e::InvalidLogicalTimeInterval)
{
	try {
		const LogicalTimeIntervalDouble& p = dynamic_cast<const LogicalTimeIntervalDouble&>(subtrahend);
		_value -= p._value;
	} catch (std::bad_cast)
	{
	    throw std::wstring(L"Invalid LogicalTimeDouble");
		//throw rti1516e::InvalidLogicalTimeInterval(L"Invalid LogicalTimeDouble");
	}

}

std::unique_ptr< rti1516e::LogicalTimeInterval > LogicalTimeDouble::subtract(rti1516e::LogicalTime const & subtrahend) const
throw (rti1516e::InvalidLogicalTime)
{
	try {
		const LogicalTimeDouble& p = dynamic_cast<const LogicalTimeDouble&>(subtrahend);
		return std::unique_ptr< rti1516e::LogicalTimeInterval >(new LogicalTimeIntervalDouble(_value - p._value));
	} catch (std::bad_cast)
	{
	    throw std::wstring(L"Invalid LogicalTimeDouble");
		//throw rti1516e::InvalidLogicalTime(L"Invalid LogicalTimeDouble");
	}
}

bool LogicalTimeDouble::isGreaterThan(rti1516e::LogicalTime const & value) const
throw (rti1516e::InvalidLogicalTime)
{
	try {
		const LogicalTimeDouble& p = dynamic_cast<const LogicalTimeDouble&>(value);
		return _value > p._value;
	} catch (std::bad_cast)
	{
	    throw std::wstring(L"Invalid LogicalTimeDouble");
		//throw rti1516e::InvalidLogicalTime(L"Invalid LogicalTimeDouble");
	}
}

bool LogicalTimeDouble::isLessThan(rti1516e::LogicalTime const & value) const
throw (rti1516e::InvalidLogicalTime)
{
	try {
		const LogicalTimeDouble& p = dynamic_cast<const LogicalTimeDouble&>(value);
		return _value < p._value;
	} catch (std::bad_cast)
	{
	    throw std::wstring(L"Invalid LogicalTimeDouble");
		//throw rti1516e::InvalidLogicalTime(L"Invalid LogicalTimeDouble");
	}
}

bool LogicalTimeDouble::isEqualTo(rti1516e::LogicalTime const & value) const
throw (rti1516e::InvalidLogicalTime)
{
	try {
		const LogicalTimeDouble& p = dynamic_cast<const LogicalTimeDouble&>(value);
		return _value == p._value;
	} catch (std::bad_cast)
	{
	    throw std::wstring(L"Invalid LogicalTimeDouble");
		//throw rti1516e::InvalidLogicalTime(L"Invalid LogicalTimeDouble");
	}
}

bool LogicalTimeDouble::isGreaterThanOrEqualTo(rti1516e::LogicalTime const & value) const
throw (rti1516e::InvalidLogicalTime)
{
	try {
		const LogicalTimeDouble& p = dynamic_cast<const LogicalTimeDouble&>(value);
		return _value >= p._value;
	} catch (std::bad_cast)
	{
	    throw std::wstring(L"Invalid LogicalTimeDouble");
		//throw rti1516e::InvalidLogicalTime(L"Invalid LogicalTimeDouble");
	}
}

bool LogicalTimeDouble::isLessThanOrEqualTo(rti1516e::LogicalTime const & value) const
throw (rti1516e::InvalidLogicalTime)
{
	try {
		const LogicalTimeDouble& p = dynamic_cast<const LogicalTimeDouble&>(value);
		return _value <= p._value;
	} catch (std::bad_cast)
	{
	    throw std::wstring(L"Invalid LogicalTimeDouble");
		//throw rti1516e::InvalidLogicalTime(L"Invalid LogicalTimeDouble");
	}
}

long LogicalTimeDouble::getSeconds() const
{
   return (long)(_value / MULTIPLIER);
}

int LogicalTimeDouble::getMicros() const
{
   return (int)(_value % MULTIPLIER);
}

std::wstring LogicalTimeDouble::toString() const
{
   wchar_t buf[128];
   if (_value == MAX_VALUE) {
      swprintf(buf, 128, L"LogicalTimeDouble<INF>");
   } else {
      swprintf(buf, 128, L"LogicalTimeDouble<%d.%06d>", getSeconds(), getMicros());
   }
   return buf;
}

bool LogicalTimeDouble::operator==(LogicalTime const & lt_left) const
	throw (rti1516e::InvalidLogicalTime)
{
	try {
		const LogicalTimeDouble& left = dynamic_cast<const LogicalTimeDouble&>(lt_left);
		return (((left._value - 1000) < _value) && ((left._value + 1000) > _value));
	} catch (std::bad_cast)
	{
	    throw std::wstring(L"Invalid LogicalTimeDouble");
		//throw rti1516e::InvalidLogicalTime(L"Invalid LogicalTimeDouble");
	}
}

int64_t LogicalTimeDouble::toMilliseconds() const
{
    return _value/1000;
}

rti1516e::VariableLengthData LogicalTimeDouble::encode() const
{
   unsigned char buf[sizeof(_value)];
   int pos = 0;
   buf[pos++] = (unsigned char)((_value >> 56) & 0xFF);
   buf[pos++] = (unsigned char)((_value >> 48) & 0xFF);
   buf[pos++] = (unsigned char)((_value >> 40) & 0xFF);
   buf[pos++] = (unsigned char)((_value >> 32) & 0xFF);
   buf[pos++] = (unsigned char)((_value >> 24) & 0xFF);
   buf[pos++] = (unsigned char)((_value >> 16) & 0xFF);
   buf[pos++] = (unsigned char)((_value >>  8) & 0xFF);
   buf[pos++] = (unsigned char)((_value >>  0) & 0xFF);

   rti1516e::VariableLengthData varData(buf, pos);

   return varData;
}

unsigned long LogicalTimeDouble::encodedLength() const
{
	return 8L;
}

unsigned long LogicalTimeDouble::encode(void* buffer, unsigned long bufferSize) const
       throw (rti1516e::CouldNotEncode)
{
   unsigned char *buf = (unsigned char*)buffer;
   int pos = 0;
   buf[pos++] = (unsigned char)((_value >> 56) & 0xFF);
   buf[pos++] = (unsigned char)((_value >> 48) & 0xFF);
   buf[pos++] = (unsigned char)((_value >> 40) & 0xFF);
   buf[pos++] = (unsigned char)((_value >> 32) & 0xFF);
   buf[pos++] = (unsigned char)((_value >> 24) & 0xFF);
   buf[pos++] = (unsigned char)((_value >> 16) & 0xFF);
   buf[pos++] = (unsigned char)((_value >>  8) & 0xFF);
   buf[pos++] = (unsigned char)((_value >>  0) & 0xFF);

   return 8L;
}

void LogicalTimeDouble::decode(rti1516e::VariableLengthData const & encodedLogicalTime)
   throw (rti1516e::InternalError, rti1516e::CouldNotDecode)
{
   int64_t value = 0;
   unsigned char* buf = (unsigned char*)encodedLogicalTime.data();
   int pos = 0;
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];

   _value = value;
}

void LogicalTimeDouble::decode(void* buffer, unsigned long bufferSize)
      throw (rti1516e::InternalError,
             rti1516e::CouldNotDecode)
{
   int64_t value = 0;
   unsigned char* buf = (unsigned char*)buffer;
   int pos = 0;
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];

   _value = value;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////







LogicalTimeDoubleFactory::~LogicalTimeDoubleFactory()
throw ()
{
}

std::unique_ptr< rti1516e::LogicalTime > LogicalTimeDoubleFactory::makeInitial()
   throw (rti1516e::InternalError)
{
   return std::unique_ptr< rti1516e::LogicalTime >(new LogicalTimeDouble((int64_t)0));
}

std::unique_ptr< rti1516e::LogicalTimeInterval > LogicalTimeDoubleFactory::makeZero()
   throw (rti1516e::InternalError)
{
   return std::unique_ptr< rti1516e::LogicalTimeInterval >(new LogicalTimeIntervalDouble((int64_t)0));
}

std::unique_ptr< rti1516e::LogicalTimeInterval > LogicalTimeDoubleFactory::epsilon()
   throw (rti1516e::InternalError)
{
   return std::unique_ptr< rti1516e::LogicalTimeInterval >(new LogicalTimeIntervalDouble((int64_t)1));
}


LogicalTimeFactoryDouble::~LogicalTimeFactoryDouble()
throw ()
{
}



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////






LogicalTimeIntervalDouble::LogicalTimeIntervalDouble(double value)
{
   int64_t seconds = (long)floor(value);
   int64_t micros = (long)fmod(value * MULTIPLIER, (double)MULTIPLIER);

   _value = seconds * MULTIPLIER + micros;
}

LogicalTimeIntervalDouble::LogicalTimeIntervalDouble(int64_t value) :
   _value(value)
{
}

LogicalTimeIntervalDouble::~LogicalTimeIntervalDouble()
  throw ()
{
}

void LogicalTimeIntervalDouble::setZero()
{
   _value = 0;
}

bool LogicalTimeIntervalDouble::isZero() const
{
   return _value == 0;
}

void LogicalTimeIntervalDouble::setEpsilon()
{
   _value = 1;
}

bool LogicalTimeIntervalDouble::isEpsilon() const
{
   return _value == 1;
}

long LogicalTimeIntervalDouble::getSeconds() const
{
   return (long)(_value / MULTIPLIER);
}

int LogicalTimeIntervalDouble::getMicros() const
{
   return (int)(_value % MULTIPLIER);
}

void LogicalTimeIntervalDouble::setTo(rti1516e::LogicalTimeInterval const & value)
   throw (rti1516e::InvalidLogicalTimeInterval)
{
	try {
		const LogicalTimeIntervalDouble& p = dynamic_cast<const LogicalTimeIntervalDouble&>(value);
		_value = p._value;
	} catch (std::bad_cast)
	{
		throw rti1516e::InvalidLogicalTime(L"Invalid LogicalTimeDouble");
	}
}

std::unique_ptr< rti1516e::LogicalTimeInterval >
LogicalTimeIntervalDouble::subtract(rti1516e::LogicalTimeInterval const & subtrahend) const
   throw (rti1516e::InvalidLogicalTimeInterval)
{
   const LogicalTimeIntervalDouble& p = dynamic_cast<const LogicalTimeIntervalDouble&>(subtrahend);

   int64_t d = _value - p._value;
   if (d < 0) {
      d = -d;
   }
   return std::unique_ptr< rti1516e::LogicalTimeInterval >(new LogicalTimeIntervalDouble(d));
}

bool LogicalTimeIntervalDouble::isGreaterThan(rti1516e::LogicalTimeInterval const & value) const
   throw (rti1516e::InvalidLogicalTimeInterval)
{
	try {
		const LogicalTimeIntervalDouble& p = dynamic_cast<const LogicalTimeIntervalDouble&>(value);
		return _value > p._value;
	} catch (std::bad_cast)
	{
		throw rti1516e::InvalidLogicalTime(L"Invalid LogicalTimeDouble");
	}
}

bool LogicalTimeIntervalDouble::isLessThan(rti1516e::LogicalTimeInterval const & value) const
throw (rti1516e::InvalidLogicalTimeInterval)
{
	try {
		const LogicalTimeIntervalDouble& p = dynamic_cast<const LogicalTimeIntervalDouble&>(value);
		return _value < p._value;
	} catch (std::bad_cast)
	{
		throw rti1516e::InvalidLogicalTime(L"Invalid LogicalTimeDouble");
	}
}

bool LogicalTimeIntervalDouble::isEqualTo(rti1516e::LogicalTimeInterval const & value) const
throw (rti1516e::InvalidLogicalTimeInterval)
{
	try {
		const LogicalTimeIntervalDouble& p = dynamic_cast<const LogicalTimeIntervalDouble&>(value);
		return _value == p._value;
	} catch (std::bad_cast)
	{
		throw rti1516e::InvalidLogicalTime(L"Invalid LogicalTimeDouble");
	}
}

bool LogicalTimeIntervalDouble::isGreaterThanOrEqualTo(rti1516e::LogicalTimeInterval const & value) const
throw (rti1516e::InvalidLogicalTimeInterval)
{
	try {
		const LogicalTimeIntervalDouble& p = dynamic_cast<const LogicalTimeIntervalDouble&>(value);
		return _value >= p._value;
	} catch (std::bad_cast)
	{
		throw rti1516e::InvalidLogicalTime(L"Invalid LogicalTimeDouble");
	}
}

bool LogicalTimeIntervalDouble::isLessThanOrEqualTo(rti1516e::LogicalTimeInterval const & value) const
throw (rti1516e::InvalidLogicalTimeInterval)
{
	try {
		const LogicalTimeIntervalDouble& p = dynamic_cast<const LogicalTimeIntervalDouble&>(value);
		return _value <= p._value;
	} catch (std::bad_cast)
	{
		throw rti1516e::InvalidLogicalTime(L"Invalid LogicalTimeDouble");
	}
}

rti1516e::VariableLengthData LogicalTimeIntervalDouble::encode() const
{
   unsigned char buf[sizeof(_value)];
   int pos = 0;
   buf[pos++] = (unsigned char)((_value >> 56) & 0xFF);
   buf[pos++] = (unsigned char)((_value >> 48) & 0xFF);
   buf[pos++] = (unsigned char)((_value >> 40) & 0xFF);
   buf[pos++] = (unsigned char)((_value >> 32) & 0xFF);
   buf[pos++] = (unsigned char)((_value >> 24) & 0xFF);
   buf[pos++] = (unsigned char)((_value >> 16) & 0xFF);
   buf[pos++] = (unsigned char)((_value >>  8) & 0xFF);
   buf[pos++] = (unsigned char)((_value >>  0) & 0xFF);

   rti1516e::VariableLengthData varData(buf, pos);

   return varData;
}

unsigned long
LogicalTimeIntervalDouble::encodedLength() const
{
	return 8L;
}
unsigned long
LogicalTimeIntervalDouble::encode(void* buffer, unsigned long bufferSize) const
throw (rti1516e::CouldNotEncode)
{
	if (bufferSize < 8L)
	{
		throw rti1516e::CouldNotEncode(L"Not enough space to encode LogicalTimeIntervalDouble");
	}

	unsigned char *buf = (unsigned char *) buffer;
	int pos = 0;
	buf[pos++] = (unsigned char)((_value >> 56) & 0xFF);
	buf[pos++] = (unsigned char)((_value >> 48) & 0xFF);
	buf[pos++] = (unsigned char)((_value >> 40) & 0xFF);
	buf[pos++] = (unsigned char)((_value >> 32) & 0xFF);
	buf[pos++] = (unsigned char)((_value >> 24) & 0xFF);
	buf[pos++] = (unsigned char)((_value >> 16) & 0xFF);
	buf[pos++] = (unsigned char)((_value >>  8) & 0xFF);
	buf[pos++] = (unsigned char)((_value >>  0) & 0xFF);

	return 8L;
}

void LogicalTimeIntervalDouble::decode(rti1516e::VariableLengthData const & encodedLogicalTimeInterval)
throw (rti1516e::InternalError, rti1516e::CouldNotDecode)
{
	if (encodedLogicalTimeInterval.size() < 8L)
	{
		throw rti1516e::CouldNotDecode(L"Not enough data in VariableLengthData to decode LogicalTimeIntervalDouble. (Needs 8 bytes)");
	}

	int64_t value = 0;
	unsigned char* buf = (unsigned char*)encodedLogicalTimeInterval.data();
	int pos = 0;
	value = (value << 8) | buf[pos++];
	value = (value << 8) | buf[pos++];
	value = (value << 8) | buf[pos++];
	value = (value << 8) | buf[pos++];
	value = (value << 8) | buf[pos++];
	value = (value << 8) | buf[pos++];
	value = (value << 8) | buf[pos++];
	value = (value << 8) | buf[pos++];

	_value = value;
}


void
LogicalTimeIntervalDouble::decode(void* buffer, unsigned long bufferSize)
throw (rti1516e::InternalError,
	   rti1516e::CouldNotDecode)
{
	if (bufferSize < 8L)
	{
		throw rti1516e::CouldNotDecode(L"Not enough data in VariableLengthData to decode LogicalTimeIntervalDouble. (Needs 8 bytes)");
	}
	int64_t value = 0;
	unsigned char* buf = (unsigned char*)buffer;
	int pos = 0;
	value = (value << 8) | buf[pos++];
	value = (value << 8) | buf[pos++];
	value = (value << 8) | buf[pos++];
	value = (value << 8) | buf[pos++];
	value = (value << 8) | buf[pos++];
	value = (value << 8) | buf[pos++];
	value = (value << 8) | buf[pos++];
	value = (value << 8) | buf[pos++];

	_value = value;
}


std::wstring LogicalTimeIntervalDouble::toString() const
{
   wchar_t buf[128];
   swprintf(buf, 128, L"LogicalTimeIntervalDouble<%d.%06d>", getSeconds(), getMicros());
   return buf;
}


void
LogicalTimeIntervalDouble::setToDifference(rti1516e::LogicalTime const & minuend,
										   rti1516e::LogicalTime const& subtrahend)
										   throw (rti1516e::InvalidLogicalTime)
{
	try {
		const LogicalTimeDouble& d_minuend = dynamic_cast<const LogicalTimeDouble&>(minuend);
		const LogicalTimeDouble& d_subtrahend = dynamic_cast<const LogicalTimeDouble&>(subtrahend);

		int64_t value = d_minuend.toMilliseconds() - d_subtrahend.toMilliseconds();
		if (value < 0) {
			value = -value;
		}
		_value = value;
	} catch (std::bad_cast)
	{
		throw rti1516e::InvalidLogicalTime(L"Invalid LogicalTimeDouble");
	}

}

rti1516e::LogicalTimeInterval &
LogicalTimeIntervalDouble::operator+=(rti1516e::LogicalTimeInterval const & addend)
throw (rti1516e::InvalidLogicalTimeInterval)
{
	return *this;
}

rti1516e::LogicalTimeInterval &
LogicalTimeIntervalDouble::operator-=(rti1516e::LogicalTimeInterval const & subtrahend)
throw (rti1516e::InvalidLogicalTimeInterval)
{
	return *this;
}







LogicalTimeIntervalDoubleFactory::~LogicalTimeIntervalDoubleFactory()
   throw ()
{
}



/*
LogicalTimeIntervalFactoryDouble::~LogicalTimeIntervalFactoryDouble()
   throw ()
{
}

std::unique_ptr< rti1516e::LogicalTimeInterval > LogicalTimeIntervalFactoryDouble::makeZero()
   throw (rti1516e::InternalError)
{
   return std::unique_ptr< rti1516e::LogicalTimeInterval >(new LogicalTimeIntervalDouble((int64_t)0));
}

std::unique_ptr< rti1516e::LogicalTimeInterval > LogicalTimeIntervalFactoryDouble::epsilon()
   throw (rti1516e::InternalError)
{
   return std::unique_ptr< rti1516e::LogicalTimeInterval >(new LogicalTimeIntervalDouble((int64_t)1));
}

std::unique_ptr< rti1516e::LogicalTimeInterval > LogicalTimeIntervalFactoryDouble::decode(rti1516e::EncodedLogicalTimeInterval const & encodedLogicalTimeInterval)
  // throw (InternalError, CouldNotDecode)
  throw ()
{
   int64_t value = 0;
   unsigned char* buf = (unsigned char*)encodedLogicalTimeInterval.data();
   int pos = 0;
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   value = (value << 8) | buf[pos++];
   return std::unique_ptr< rti1516e::LogicalTimeInterval >(new LogicalTimeIntervalDouble(value));
}
*/
