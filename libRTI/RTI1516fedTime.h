#ifndef RTI1516_FEDTIME_H
#define RTI1516_FEDTIME_H

#include <RTI/certiLogicalTime.h>
#include <RTI/certiLogicalTimeInterval.h>
#include <RTI/certiLogicalTimeFactory.h>

namespace rti1516 {
	class VariableLengthData;
}

class RTI_EXPORT_FEDTIME RTI1516fedTime : public rti1516::LogicalTime
{
public:
	RTI1516fedTime(double timeVal);
	RTI1516fedTime(const RTI1516fedTime &other);

	virtual ~RTI1516fedTime() 
		throw ();

	virtual
		void
		setInitial();

	virtual
		bool
		isInitial() const;

	virtual
		void
		setFinal();

	virtual
		bool
		isFinal() const;

	virtual
		rti1516::LogicalTime &
		operator=(rti1516::LogicalTime const & value)
		throw (rti1516::InvalidLogicalTime);

	virtual
		RTI1516fedTime &
		operator=(RTI1516fedTime const & value)
		throw ();

	virtual
		rti1516::LogicalTime &
		operator+=(rti1516::LogicalTimeInterval const & addend)
		throw (rti1516::IllegalTimeArithmetic, rti1516::InvalidLogicalTimeInterval);

	virtual
		rti1516::LogicalTime &
		operator-=(rti1516::LogicalTimeInterval const & subtrahend)
		throw (rti1516::IllegalTimeArithmetic, rti1516::InvalidLogicalTimeInterval);

	virtual
		bool
		operator>(rti1516::LogicalTime const & value) const
		throw (rti1516::InvalidLogicalTime);

	virtual
		bool
		operator<(rti1516::LogicalTime const & value) const
		throw (rti1516::InvalidLogicalTime);

	virtual
		bool
		operator==(rti1516::LogicalTime const & value) const
		throw (rti1516::InvalidLogicalTime);

	virtual
		bool
		operator>=(rti1516::LogicalTime const & value) const
		throw (rti1516::InvalidLogicalTime);

	virtual
		bool
		operator<=(rti1516::LogicalTime const & value) const
		throw (rti1516::InvalidLogicalTime);

	// Generates an encoded value that can be used to send
	// LogicalTimes to other federates in updates or interactions
	// Not implemented.
	virtual
		rti1516::VariableLengthData 
		encode() const;

	// Alternate encode for directly filling a buffer
	virtual 
		unsigned long 
		encodedLength() const;

	virtual 
		unsigned long 
		encode(void* buffer, unsigned long bufferSize) const 
		throw (rti1516::CouldNotEncode);

	// Decode encodedLogicalTime into self
	// Not implemented.
	virtual 
		void 
		decode(rti1516::VariableLengthData const & encodedLogicalTime)
		throw (rti1516::InternalError,
		rti1516::CouldNotDecode);

	// Alternate decode that reads directly from a buffer
	virtual 
		void 
		decode(void* buffer, unsigned long bufferSize)
		throw (rti1516::InternalError,
		rti1516::CouldNotDecode);

	virtual 
		std::wstring 
		toString() const;

	// Returns the name of the implementation, as needed by
	// createFederationExecution.
	virtual 
		std::wstring 
		implementationName() const;

	double getFedTime() const
	{ return _fedTime; }

	bool isInfinity() const
	{ return _fedTime == _positiveInfinity; }


private:
	double _fedTime;
	double _zero;
	double _epsilon;
	double _positiveInfinity;

}; // class RTI_EXPORT_FEDTIME RTI1516fedTime

class RTI_EXPORT_FEDTIME RTI1516fedTimeInterval : public rti1516::LogicalTimeInterval
{
public:
	RTI1516fedTimeInterval(double timeVal);
	RTI1516fedTimeInterval(const RTI1516fedTimeInterval &other);

	virtual
		~RTI1516fedTimeInterval()
		throw ();

	virtual
		void
		setZero();

	virtual
		bool
		isZero() const;

	virtual
		void
		setEpsilon();

	virtual
		bool
		isEpsilon() const;

	virtual
		rti1516::LogicalTimeInterval &
		operator=(rti1516::LogicalTimeInterval const & value)
		throw (rti1516::InvalidLogicalTimeInterval);

	virtual
		RTI1516fedTimeInterval &
		operator=(RTI1516fedTimeInterval const & other)
		throw ();

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
		throw (rti1516::InvalidLogicalTimeInterval);

	virtual
		bool
		operator<(rti1516::LogicalTimeInterval const & value) const
		throw (rti1516::InvalidLogicalTimeInterval);

	virtual
		bool
		operator==(rti1516::LogicalTimeInterval const & value) const
		throw (rti1516::InvalidLogicalTimeInterval);

	virtual
		bool
		operator>=(rti1516::LogicalTimeInterval const & value) const
		throw (rti1516::InvalidLogicalTimeInterval);

	virtual
		bool
		operator<=(rti1516::LogicalTimeInterval const & value) const
		throw (rti1516::InvalidLogicalTimeInterval);

	// Generates an encoded value that can be used to send
	// LogicalTimeIntervals to other federates in updates or interactions
	// Not implemented.
	virtual 
		rti1516::VariableLengthData 
		encode() const;

	// Alternate encode for directly filling a buffer
	virtual 
		unsigned long 
		encodedLength() const;

	virtual 
		unsigned long 
		encode(void* buffer, unsigned long bufferSize) const 
		throw (rti1516::CouldNotEncode);

	// Decode encodedValue into self
	// Not implemented.
	virtual 
		void 
		decode(rti1516::VariableLengthData const & encodedValue)
		throw (rti1516::InternalError, rti1516::CouldNotDecode);

	// Alternate decode that reads directly from a buffer
	virtual 
		void 
		decode(void* buffer, unsigned long bufferSize)
		throw (rti1516::InternalError, rti1516::CouldNotDecode);

	virtual 
		std::wstring 
		toString() const;

	// Returns the name of the implementation, as needed by
	// createFederationExecution.
	virtual 
		std::wstring 
		implementationName() const;

	double getInterval() const
	{ return _fedInterval; }

	double getEpsilon() const
	{ return _epsilon; }

private:
	double _fedInterval;
	double _zero;
	double _epsilon;
	double _positiveInfinity;

}; // class RTI_EXPORT_FEDTIME RTI1516fedTimeInterval

class RTI_EXPORT_FEDTIME RTI1516fedTimeFactory : public rti1516::LogicalTimeFactory
{
public:
	virtual
		~RTI1516fedTimeFactory()
		throw ();

	// Returns a LogicalTime with a value of "initial"
	virtual
		std::auto_ptr< rti1516::LogicalTime >
		makeLogicalTime()
		throw (rti1516::InternalError);

	virtual
		std::auto_ptr< rti1516::LogicalTime >
		makeLogicalTime(double timeVal)
		throw (rti1516::InternalError);

	// Returns a LogicalTimeInterval with a value of "zero"
	virtual 
		std::auto_ptr< rti1516::LogicalTimeInterval >
		makeLogicalTimeInterval() 
		throw (rti1516::InternalError);

	virtual 
		std::auto_ptr< rti1516::LogicalTimeInterval >
		makeLogicalTimeInterval(double timeInterval) 
		throw (rti1516::InternalError);

private:
	friend class rti1516::LogicalTimeFactoryFactory;

	// Private constructor - Only for LogicalTimeFactoryFactory to access.
	RTI1516fedTimeFactory()
		throw();

}; // class RTI_EXPORT_FEDTIME RTI1516fedTimeFactory



// The LogicalTimeFactoryFactory must also be implemented by the 
// federate. The definition is copied here (though commented out) 
// for reference.

//namespace rti1516
//{  
//  class RTI_EXPORT_FEDTIME LogicalTimeFactoryFactory
//  {
//  public:
//
//    // The name is used to choose among several LogicalTimeFactories that might
//    // be present in the fedtime library.  Each federation chooses its
//    // implementation by passing the appropriate name to createFederationExecution.
//    // If the supplied name is the empty string, a default LogicalTimeFactory is
//    // returned.  If the supplied implementation name does not match any name 
//    // supported by the library, then a NULL pointer is returned. 
//    static std::auto_ptr< LogicalTimeFactory > 
//       makeLogicalTimeFactory(std::wstring const & implementationName);
//  };
//}




# endif // RTI1516_FED_TIME_H
