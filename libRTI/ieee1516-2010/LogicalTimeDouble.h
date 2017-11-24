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
#ifndef LogicalTimeDouble_h
#define LogicalTimeDouble_h

#if defined(_WIN32) && defined(_MSC_VER)
#pragma warning(disable:4290)
#endif

// Standard integer types.
#ifdef _MSC_VER
#ifndef FAKED_INTTYPES_DEFINED
#define FAKED_INTTYPES_DEFINED
typedef unsigned __int64  uint64_t;
typedef signed __int64    int64_t;
typedef unsigned __int32  uint32_t;
typedef signed __int32    int32_t;
typedef unsigned __int16  uint16_t;
typedef signed __int16    int16_t;
typedef unsigned __int8   uint8_t;
typedef signed __int8     int8_t;
typedef short int         int_least16_t;
#endif
#else
#include <inttypes.h>
#endif

#include "RTI/certiLogicalTime.h"
#include "RTI/certiLogicalTimeInterval.h"
#include "RTI/certiLogicalTimeFactory.h"

// Microsoft has not implemented the swprintf function according
// to the ISO C standard. However, they have a function _snwprintf
// that matches the standardized prototype for swprintf.
#ifdef _WIN32 
#define swprintf _snwprintf
#endif

class RTI_EXPORT_FEDTIME LogicalTimeDouble : public rti1516e::LogicalTime
{
public:
   LogicalTimeDouble(double value);

   virtual ~LogicalTimeDouble()
      throw ();

   virtual void setInitial();

   virtual bool isInitial() const;

   virtual void setFinal();

   virtual bool isFinal() const;

   virtual long getSeconds() const;
   virtual int getMicros() const;
   
   virtual void setTo(rti1516e::LogicalTime const & value)
      throw (rti1516e::InvalidLogicalTime);

   virtual void increaseBy(rti1516e::LogicalTimeInterval const & addend)
      throw (rti1516e::IllegalTimeArithmetic, rti1516e::InvalidLogicalTimeInterval);

   virtual void decreaseBy(rti1516e::LogicalTimeInterval const & subtrahend)
      throw (rti1516e::IllegalTimeArithmetic, rti1516e::InvalidLogicalTimeInterval);

   virtual std::unique_ptr< rti1516e::LogicalTimeInterval > subtract(rti1516e::LogicalTime const & subtrahend) const
      throw (rti1516e::InvalidLogicalTime);

   virtual bool isGreaterThan(rti1516e::LogicalTime const & value) const
      throw (rti1516e::InvalidLogicalTime);

   virtual bool isLessThan(rti1516e::LogicalTime const & value) const
      throw (rti1516e::InvalidLogicalTime);

   virtual bool isEqualTo(rti1516e::LogicalTime const & value) const
   throw (rti1516e::InvalidLogicalTime);

   virtual bool isGreaterThanOrEqualTo(rti1516e::LogicalTime const & value) const
   throw (rti1516e::InvalidLogicalTime);

   virtual bool isLessThanOrEqualTo(rti1516e::LogicalTime const & value) const
   throw (rti1516e::InvalidLogicalTime);

   // JvY Begin
    virtual
    rti1516e::LogicalTime &
    operator=(rti1516e::LogicalTime const & value)
      throw (rti1516e::InvalidLogicalTime);

    virtual
    LogicalTimeDouble &
    operator=(LogicalTimeDouble const & value)
      throw (rti1516e::InvalidLogicalTime);


    virtual
    rti1516e::LogicalTime &
    operator+=(rti1516e::LogicalTimeInterval const & addend)
      throw (rti1516e::IllegalTimeArithmetic, rti1516e::InvalidLogicalTimeInterval)
	{
		increaseBy(addend);
		return *this;
	}

    virtual
    rti1516e::LogicalTime &
    operator-=(rti1516e::LogicalTimeInterval const & subtrahend)
      throw (rti1516e::IllegalTimeArithmetic, rti1516e::InvalidLogicalTimeInterval)
	{
		decreaseBy(subtrahend);
		return *this;
	}

    virtual
    bool
    operator>(rti1516e::LogicalTime const & value) const
      throw (rti1516e::InvalidLogicalTime)
	{
		return isGreaterThan(value);
	}

    virtual
    bool
    operator<(rti1516e::LogicalTime const & value) const
      throw (rti1516e::InvalidLogicalTime)
	{
		return isLessThan(value);
	}

    virtual
    bool
    operator>=(rti1516e::LogicalTime const & value) const
      throw (rti1516e::InvalidLogicalTime)
	{
		return isGreaterThanOrEqualTo(value);
	}

    virtual
    bool
    operator<=(rti1516e::LogicalTime const & value) const
      throw (rti1516e::InvalidLogicalTime)
	{
		return isLessThanOrEqualTo(value);
	}

    // Generates an encoded value that can be used to send
    // LogicalTimes to other federates in updates or interactions
    virtual rti1516e::VariableLengthData encode() const;

    // Alternate encode for directly filling a buffer
    virtual unsigned long encodedLength() const;
    virtual unsigned long encode(void* buffer, unsigned long bufferSize) const 
       throw (rti1516e::CouldNotEncode);
   
    // Decode encodedLogicalTime into self
    virtual void decode(rti1516e::VariableLengthData const & encodedLogicalTime)
      throw (rti1516e::InternalError,
             rti1516e::CouldNotDecode);

    // Alternate decode that reads directly from a buffer
    virtual void decode(void* buffer, unsigned long bufferSize)
      throw (rti1516e::InternalError,
             rti1516e::CouldNotDecode);

    // Returns the name of the implementation, as needed by
    // createFederationExecution.
    virtual std::wstring implementationName() const
	{
		return L"BITS_LogicalTimeDouble";
	}

	// JvY End

   //virtual std::unique_ptr< rti1516e::EncodedLogicalTime > encode() const;

   virtual std::wstring toString() const;

   virtual bool operator==(LogicalTime const & left) const
	   throw (rti1516e::InvalidLogicalTime);

   virtual int64_t toMilliseconds() const;

private:
   explicit LogicalTimeDouble(int64_t value);

   int64_t _value;

   friend class LogicalTimeDoubleFactory;
   friend class LogicalTimeFactoryDouble;
};

class RTI_EXPORT_FEDTIME LogicalTimeIntervalDouble : public rti1516e::LogicalTimeInterval
{
public:
   LogicalTimeIntervalDouble(double value);

   virtual ~LogicalTimeIntervalDouble()
      throw ();
   
   virtual void setZero();
   
   virtual bool isZero() const;

    virtual void setEpsilon();

   virtual bool isEpsilon() const;

   virtual long getSeconds() const;
   virtual int getMicros() const;

   virtual void setTo(rti1516e::LogicalTimeInterval const & value)
      throw (rti1516e::InvalidLogicalTimeInterval);
   
   virtual std::unique_ptr< rti1516e::LogicalTimeInterval > subtract(rti1516e::LogicalTimeInterval const & subtrahend) const
      throw (rti1516e::InvalidLogicalTimeInterval);
   
   virtual bool isGreaterThan(rti1516e::LogicalTimeInterval const & value) const
      throw (rti1516e::InvalidLogicalTimeInterval);
   
   virtual bool isLessThan(rti1516e::LogicalTimeInterval const & value) const
      throw (rti1516e::InvalidLogicalTimeInterval);
   
   virtual bool isEqualTo(rti1516e::LogicalTimeInterval const & value) const
      throw (rti1516e::InvalidLogicalTimeInterval);
   
   virtual bool isGreaterThanOrEqualTo(rti1516e::LogicalTimeInterval const & value) const
      throw (rti1516e::InvalidLogicalTimeInterval);
   
   virtual bool isLessThanOrEqualTo(rti1516e::LogicalTimeInterval const & value) const
      throw (rti1516e::InvalidLogicalTimeInterval);
   
   //virtual std::unique_ptr< rti1516e::EncodedLogicalTimeInterval > encode() const;
   
   virtual std::wstring toString() const;

   // JvY Begin
    virtual
    LogicalTimeInterval &
    operator=(rti1516e::LogicalTimeInterval const & value)
      throw (rti1516e::InvalidLogicalTimeInterval)
	{
		setTo(value);
		return *this;
	}


    // Set self to the difference between two LogicalTimes
    virtual
    void
    setToDifference(rti1516e::LogicalTime const & minuend,
      rti1516e::LogicalTime const& subtrahend)
      throw (rti1516e::InvalidLogicalTime);

    virtual
    rti1516e::LogicalTimeInterval &
    operator+=(rti1516e::LogicalTimeInterval const & addend)
      throw (rti1516e::InvalidLogicalTimeInterval);

    virtual
    rti1516e::LogicalTimeInterval &
    operator-=(rti1516e::LogicalTimeInterval const & subtrahend)
      throw (rti1516e::InvalidLogicalTimeInterval);
    
    virtual
    bool
    operator>(rti1516e::LogicalTimeInterval const & value) const
      throw (rti1516e::InvalidLogicalTimeInterval)
	{
		return isGreaterThan(value);
	}

    virtual
    bool
    operator<(rti1516e::LogicalTimeInterval const & value) const
      throw (rti1516e::InvalidLogicalTimeInterval)
	{
		return isLessThan(value);
	}

    virtual
    bool
    operator==(rti1516e::LogicalTimeInterval const & value) const
      throw (rti1516e::InvalidLogicalTimeInterval)
	{
		return isEqualTo(value);
	}

    virtual
    bool
    operator>=(rti1516e::LogicalTimeInterval const & value) const
      throw (rti1516e::InvalidLogicalTimeInterval)
	{
		return isGreaterThanOrEqualTo(value);
	}

    virtual
    bool
    operator<=(rti1516e::LogicalTimeInterval const & value) const
      throw (rti1516e::InvalidLogicalTimeInterval)
	{
		return isLessThanOrEqualTo(value);
	}
    
    // Generates an encoded value that can be used to send
    // LogicalTimeIntervals to other federates in updates or interactions
    virtual rti1516e::VariableLengthData encode() const;

    // Alternate encode for directly filling a buffer
    virtual unsigned long encodedLength() const;
    virtual unsigned long encode(void* buffer, unsigned long bufferSize) const 
       throw (rti1516e::CouldNotEncode);

    // Decode encodedValue into self
    virtual void decode(rti1516e::VariableLengthData const & encodedValue)
      throw (rti1516e::InternalError,
             rti1516e::CouldNotDecode);

    // Alternate decode that reads directly from a buffer
    virtual void decode(void* buffer, unsigned long bufferSize)
      throw (rti1516e::InternalError,
             rti1516e::CouldNotDecode);

    // Returns the name of the implementation, as needed by
    // createFederationExecution.
    virtual std::wstring implementationName() const
	{
		return L"BITS_LogicalTimeIntervalDouble";
	}

   // JvY End

private:
   explicit LogicalTimeIntervalDouble(int64_t value);
      
   int64_t _value;

   friend class LogicalTimeDouble;
   friend class LogicalTimeDoubleFactory;
   friend class LogicalTimeFactoryDouble;
   friend class LogicalTimeIntervalDoubleFactory;
   friend class LogicalTimeIntervalFactoryDouble;
};


class RTI_EXPORT_FEDTIME LogicalTimeDoubleFactory : public rti1516e::LogicalTimeFactory
{
public:
   virtual ~LogicalTimeDoubleFactory()
      throw ();
 
   virtual std::unique_ptr< rti1516e::LogicalTime > makeInitial()
      throw (rti1516e::InternalError);

   // Returns a LogicalTime with a value of "initial"
    virtual
    std::unique_ptr< rti1516e::LogicalTime >
    makeLogicalTime()
	throw (rti1516e::InternalError)
	{
		return makeInitial();
	}
    
    // Returns a LogicalTimeInterval with a value of "zero"
    virtual 
    std::unique_ptr< rti1516e::LogicalTimeInterval >
    makeLogicalTimeInterval() 
      throw (rti1516e::InternalError)
	{
		return makeZero();
	}

   virtual std::unique_ptr< rti1516e::LogicalTimeInterval > makeZero()
      throw (rti1516e::InternalError);
 
   virtual std::unique_ptr< rti1516e::LogicalTimeInterval > epsilon()
      throw (rti1516e::InternalError);

   //virtual std::unique_ptr< rti1516e::LogicalTime > decode(rti1516e::EncodedLogicalTime const & encodedLogicalTime)
   //   throw (rti1516e::InternalError, rti1516e::CouldNotDecode);
};

class RTI_EXPORT_FEDTIME LogicalTimeFactoryDouble : public LogicalTimeDoubleFactory
{
public:
   virtual ~LogicalTimeFactoryDouble()
      throw ();
 
};

class RTI_EXPORT_FEDTIME LogicalTimeIntervalDoubleFactory : public LogicalTimeDoubleFactory
{
public:
   virtual ~LogicalTimeIntervalDoubleFactory()
      throw ();
 
};

class RTI_EXPORT_FEDTIME LogicalTimeIntervalFactoryDouble : public LogicalTimeDoubleFactory
{
public:
   virtual ~LogicalTimeIntervalFactoryDouble()
      throw ()
   {
   }
 
};

class RTI_EXPORT_FEDTIME LogicalTimeDoubleFactoryFactory : public rti1516e::LogicalTimeFactoryFactory
{
public:
    static std::unique_ptr< rti1516e::LogicalTimeFactory > 
       makeLogicalTimeFactory(std::wstring const & implementationName);

};

#endif // LogicalTimeDouble_h
