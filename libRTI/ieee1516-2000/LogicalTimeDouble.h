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

#ifdef _WIN32
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

class RTI_EXPORT_FEDTIME LogicalTimeDouble : public rti1516::LogicalTime
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
   
   virtual void setTo(rti1516::LogicalTime const & value)
      throw (rti1516::InvalidLogicalTime);

   virtual void increaseBy(rti1516::LogicalTimeInterval const & addend)
      throw (rti1516::IllegalTimeArithmetic, rti1516::InvalidLogicalTimeInterval);

   virtual void decreaseBy(rti1516::LogicalTimeInterval const & subtrahend)
      throw (rti1516::IllegalTimeArithmetic, rti1516::InvalidLogicalTimeInterval);

   virtual std::auto_ptr< rti1516::LogicalTimeInterval > subtract(rti1516::LogicalTime const & subtrahend) const
      throw (rti1516::InvalidLogicalTime);

   virtual bool isGreaterThan(rti1516::LogicalTime const & value) const
      throw (rti1516::InvalidLogicalTime);

   virtual bool isLessThan(rti1516::LogicalTime const & value) const
      throw (rti1516::InvalidLogicalTime);

   virtual bool isEqualTo(rti1516::LogicalTime const & value) const
   throw (rti1516::InvalidLogicalTime);

   virtual bool isGreaterThanOrEqualTo(rti1516::LogicalTime const & value) const
   throw (rti1516::InvalidLogicalTime);

   virtual bool isLessThanOrEqualTo(rti1516::LogicalTime const & value) const
   throw (rti1516::InvalidLogicalTime);

   // JvY Begin
    virtual
    rti1516::LogicalTime &
    operator=(rti1516::LogicalTime const & value)
      throw (rti1516::InvalidLogicalTime);

    virtual
    LogicalTimeDouble &
    operator=(LogicalTimeDouble const & value)
      throw (rti1516::InvalidLogicalTime);


    virtual
    rti1516::LogicalTime &
    operator+=(rti1516::LogicalTimeInterval const & addend)
      throw (rti1516::IllegalTimeArithmetic, rti1516::InvalidLogicalTimeInterval)
	{
		increaseBy(addend);
		return *this;
	}

    virtual
    rti1516::LogicalTime &
    operator-=(rti1516::LogicalTimeInterval const & subtrahend)
      throw (rti1516::IllegalTimeArithmetic, rti1516::InvalidLogicalTimeInterval)
	{
		decreaseBy(subtrahend);
		return *this;
	}

    virtual
    bool
    operator>(rti1516::LogicalTime const & value) const
      throw (rti1516::InvalidLogicalTime)
	{
		return isGreaterThan(value);
	}

    virtual
    bool
    operator<(rti1516::LogicalTime const & value) const
      throw (rti1516::InvalidLogicalTime)
	{
		return isLessThan(value);
	}

    virtual
    bool
    operator>=(rti1516::LogicalTime const & value) const
      throw (rti1516::InvalidLogicalTime)
	{
		return isGreaterThanOrEqualTo(value);
	}

    virtual
    bool
    operator<=(rti1516::LogicalTime const & value) const
      throw (rti1516::InvalidLogicalTime)
	{
		return isLessThanOrEqualTo(value);
	}

    // Generates an encoded value that can be used to send
    // LogicalTimes to other federates in updates or interactions
    virtual rti1516::VariableLengthData encode() const;

    // Alternate encode for directly filling a buffer
    virtual unsigned long encodedLength() const;
    virtual unsigned long encode(void* buffer, unsigned long bufferSize) const 
       throw (rti1516::CouldNotEncode);
   
    // Decode encodedLogicalTime into self
    virtual void decode(rti1516::VariableLengthData const & encodedLogicalTime)
      throw (rti1516::InternalError,
             rti1516::CouldNotDecode);

    // Alternate decode that reads directly from a buffer
    virtual void decode(void* buffer, unsigned long bufferSize)
      throw (rti1516::InternalError,
             rti1516::CouldNotDecode);

    // Returns the name of the implementation, as needed by
    // createFederationExecution.
    virtual std::wstring implementationName() const
	{
		return L"BITS_LogicalTimeDouble";
	}

	// JvY End

   //virtual std::auto_ptr< rti1516::EncodedLogicalTime > encode() const;

   virtual std::wstring toString() const;

   virtual bool operator==(LogicalTime const & left) const
	   throw (rti1516::InvalidLogicalTime);

   virtual int64_t toMilliseconds() const;

private:
   explicit LogicalTimeDouble(int64_t value);

   int64_t _value;

   friend class LogicalTimeDoubleFactory;
   friend class LogicalTimeFactoryDouble;
};

class RTI_EXPORT_FEDTIME LogicalTimeIntervalDouble : public rti1516::LogicalTimeInterval
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

   virtual void setTo(rti1516::LogicalTimeInterval const & value)
      throw (rti1516::InvalidLogicalTimeInterval);
   
   virtual std::auto_ptr< rti1516::LogicalTimeInterval > subtract(rti1516::LogicalTimeInterval const & subtrahend) const
      throw (rti1516::InvalidLogicalTimeInterval);
   
   virtual bool isGreaterThan(rti1516::LogicalTimeInterval const & value) const
      throw (rti1516::InvalidLogicalTimeInterval);
   
   virtual bool isLessThan(rti1516::LogicalTimeInterval const & value) const
      throw (rti1516::InvalidLogicalTimeInterval);
   
   virtual bool isEqualTo(rti1516::LogicalTimeInterval const & value) const
      throw (rti1516::InvalidLogicalTimeInterval);
   
   virtual bool isGreaterThanOrEqualTo(rti1516::LogicalTimeInterval const & value) const
      throw (rti1516::InvalidLogicalTimeInterval);
   
   virtual bool isLessThanOrEqualTo(rti1516::LogicalTimeInterval const & value) const
      throw (rti1516::InvalidLogicalTimeInterval);
   
   //virtual std::auto_ptr< rti1516::EncodedLogicalTimeInterval > encode() const;
   
   virtual std::wstring toString() const;

   // JvY Begin
    virtual
    LogicalTimeInterval &
    operator=(rti1516::LogicalTimeInterval const & value)
      throw (rti1516::InvalidLogicalTimeInterval)
	{
		setTo(value);
		return *this;
	}


    // Set self to the difference between two LogicalTimes
    virtual
    void
    setToDifference(rti1516::LogicalTime const & minuend,
      rti1516::LogicalTime const& subtrahend)
      throw (rti1516::InvalidLogicalTime);

    virtual
    rti1516::LogicalTimeInterval &
    operator+=(rti1516::LogicalTimeInterval const & addend)
      throw (rti1516::InvalidLogicalTimeInterval);

    virtual
    rti1516::LogicalTimeInterval &
    operator-=(rti1516::LogicalTimeInterval const & subtrahend)
      throw (rti1516::InvalidLogicalTimeInterval);
    
    virtual
    bool
    operator>(rti1516::LogicalTimeInterval const & value) const
      throw (rti1516::InvalidLogicalTimeInterval)
	{
		return isGreaterThan(value);
	}

    virtual
    bool
    operator<(rti1516::LogicalTimeInterval const & value) const
      throw (rti1516::InvalidLogicalTimeInterval)
	{
		return isLessThan(value);
	}

    virtual
    bool
    operator==(rti1516::LogicalTimeInterval const & value) const
      throw (rti1516::InvalidLogicalTimeInterval)
	{
		return isEqualTo(value);
	}

    virtual
    bool
    operator>=(rti1516::LogicalTimeInterval const & value) const
      throw (rti1516::InvalidLogicalTimeInterval)
	{
		return isGreaterThanOrEqualTo(value);
	}

    virtual
    bool
    operator<=(rti1516::LogicalTimeInterval const & value) const
      throw (rti1516::InvalidLogicalTimeInterval)
	{
		return isLessThanOrEqualTo(value);
	}
    
    // Generates an encoded value that can be used to send
    // LogicalTimeIntervals to other federates in updates or interactions
    virtual rti1516::VariableLengthData encode() const;

    // Alternate encode for directly filling a buffer
    virtual unsigned long encodedLength() const;
    virtual unsigned long encode(void* buffer, unsigned long bufferSize) const 
       throw (rti1516::CouldNotEncode);

    // Decode encodedValue into self
    virtual void decode(rti1516::VariableLengthData const & encodedValue)
      throw (rti1516::InternalError,
             rti1516::CouldNotDecode);

    // Alternate decode that reads directly from a buffer
    virtual void decode(void* buffer, unsigned long bufferSize)
      throw (rti1516::InternalError,
             rti1516::CouldNotDecode);

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


class RTI_EXPORT_FEDTIME LogicalTimeDoubleFactory : public rti1516::LogicalTimeFactory
{
public:
   virtual ~LogicalTimeDoubleFactory()
      throw ();
 
   virtual std::auto_ptr< rti1516::LogicalTime > makeInitial()
      throw (rti1516::InternalError);

   // Returns a LogicalTime with a value of "initial"
    virtual
    std::auto_ptr< rti1516::LogicalTime >
    makeLogicalTime()
	throw (rti1516::InternalError)
	{
		return makeInitial();
	}
    
    // Returns a LogicalTimeInterval with a value of "zero"
    virtual 
    std::auto_ptr< rti1516::LogicalTimeInterval >
    makeLogicalTimeInterval() 
      throw (rti1516::InternalError)
	{
		return makeZero();
	}

   virtual std::auto_ptr< rti1516::LogicalTimeInterval > makeZero()
      throw (rti1516::InternalError);
 
   virtual std::auto_ptr< rti1516::LogicalTimeInterval > epsilon()
      throw (rti1516::InternalError);

   //virtual std::auto_ptr< rti1516::LogicalTime > decode(rti1516::EncodedLogicalTime const & encodedLogicalTime)
   //   throw (rti1516::InternalError, rti1516::CouldNotDecode);
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

class RTI_EXPORT_FEDTIME LogicalTimeDoubleFactoryFactory : public rti1516::LogicalTimeFactoryFactory
{
public:
    static std::auto_ptr< rti1516::LogicalTimeFactory > 
       makeLogicalTimeFactory(std::wstring const & implementationName);

};

#endif // LogicalTimeDouble_h
