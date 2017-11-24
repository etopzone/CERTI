#ifndef RTI1516E_FEDTIME_H
#define RTI1516E_FEDTIME_H

#include <RTI/certiLogicalTime.h>
#include <RTI/certiLogicalTimeInterval.h>
#include <RTI/certiLogicalTimeFactory.h>

namespace rti1516e {
	class VariableLengthData;
}

class RTI_EXPORT_FEDTIME RTI1516fedTime : public rti1516e::LogicalTime
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
		rti1516e::LogicalTime &
		operator=(rti1516e::LogicalTime const & value)
		throw (rti1516e::InvalidLogicalTime);

	virtual
		RTI1516fedTime &
		operator=(RTI1516fedTime const & value)
		throw ();

	virtual
		rti1516e::LogicalTime &
		operator+=(rti1516e::LogicalTimeInterval const & addend)
		throw (rti1516e::IllegalTimeArithmetic, rti1516e::InvalidLogicalTimeInterval);

	virtual
		rti1516e::LogicalTime &
		operator-=(rti1516e::LogicalTimeInterval const & subtrahend)
		throw (rti1516e::IllegalTimeArithmetic, rti1516e::InvalidLogicalTimeInterval);

	virtual
		bool
		operator>(rti1516e::LogicalTime const & value) const
		throw (rti1516e::InvalidLogicalTime);

	virtual
		bool
		operator<(rti1516e::LogicalTime const & value) const
		throw (rti1516e::InvalidLogicalTime);

	virtual
		bool
		operator==(rti1516e::LogicalTime const & value) const
		throw (rti1516e::InvalidLogicalTime);

	virtual
		bool
		operator>=(rti1516e::LogicalTime const & value) const
		throw (rti1516e::InvalidLogicalTime);

	virtual
		bool
		operator<=(rti1516e::LogicalTime const & value) const
		throw (rti1516e::InvalidLogicalTime);

	// Generates an encoded value that can be used to send
	// LogicalTimes to other federates in updates or interactions
	// Not implemented.
	virtual
		rti1516e::VariableLengthData
		encode() const;

	// Alternate encode for directly filling a buffer
	virtual 
		size_t 
		encodedLength() const;

	virtual 
		size_t 
		encode(void* buffer, size_t bufferSize) const 
		throw (rti1516e::CouldNotEncode);

	// Decode encodedLogicalTime into self
	// Not implemented.
	virtual 
		void 
		decode(rti1516e::VariableLengthData const & encodedLogicalTime)
		throw (rti1516e::InternalError,
		rti1516e::CouldNotDecode);

	// Alternate decode that reads directly from a buffer
	virtual 
		void 
		decode(void* buffer, size_t bufferSize)
		throw (rti1516e::InternalError,
		rti1516e::CouldNotDecode);

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

class RTI_EXPORT_FEDTIME RTI1516fedTimeInterval : public rti1516e::LogicalTimeInterval
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
		rti1516e::LogicalTimeInterval &
		operator=(rti1516e::LogicalTimeInterval const & value)
		throw (rti1516e::InvalidLogicalTimeInterval);

	virtual
		RTI1516fedTimeInterval &
		operator=(RTI1516fedTimeInterval const & other)
		throw ();

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
		throw (rti1516e::InvalidLogicalTimeInterval);

	virtual
		bool
		operator<(rti1516e::LogicalTimeInterval const & value) const
		throw (rti1516e::InvalidLogicalTimeInterval);

	virtual
		bool
		operator==(rti1516e::LogicalTimeInterval const & value) const
		throw (rti1516e::InvalidLogicalTimeInterval);

	virtual
		bool
		operator>=(rti1516e::LogicalTimeInterval const & value) const
		throw (rti1516e::InvalidLogicalTimeInterval);

	virtual
		bool
		operator<=(rti1516e::LogicalTimeInterval const & value) const
		throw (rti1516e::InvalidLogicalTimeInterval);

	// Generates an encoded value that can be used to send
	// LogicalTimeIntervals to other federates in updates or interactions
	// Not implemented.
	virtual 
		rti1516e::VariableLengthData
		encode() const;

	// Alternate encode for directly filling a buffer
	virtual 
		size_t 
		encodedLength() const;

	virtual 
		size_t 
		encode(void* buffer, size_t bufferSize) const 
		throw (rti1516e::CouldNotEncode);

	// Decode encodedValue into self
	// Not implemented.
	virtual 
		void 
		decode(rti1516e::VariableLengthData const & encodedValue)
		throw (rti1516e::InternalError, rti1516e::CouldNotDecode);

	// Alternate decode that reads directly from a buffer
	virtual 
		void 
		decode(void* buffer, size_t bufferSize)
		throw (rti1516e::InternalError, rti1516e::CouldNotDecode);

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

class RTI_EXPORT_FEDTIME RTI1516fedTimeFactory : public rti1516e::LogicalTimeFactory
{
public:
	virtual
		~RTI1516fedTimeFactory()
		throw ();

	// Returns a LogicalTime with a value of "initial"
	virtual
		std::unique_ptr< rti1516e::LogicalTime >
		makeLogicalTime()
		throw (rti1516e::InternalError);

	virtual
		std::unique_ptr< rti1516e::LogicalTime >
		makeLogicalTime(double timeVal)
		throw (rti1516e::InternalError);

	// Returns a LogicalTimeInterval with a value of "zero"
	virtual 
		std::unique_ptr< rti1516e::LogicalTimeInterval >
		makeLogicalTimeInterval() 
		throw (rti1516e::InternalError);

	virtual 
		std::unique_ptr< rti1516e::LogicalTimeInterval >
		makeLogicalTimeInterval(double timeInterval) 
		throw (rti1516e::InternalError);

private:
	friend class rti1516e::LogicalTimeFactoryFactory;

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
//    static std::unique_ptr< LogicalTimeFactory > 
//       makeLogicalTimeFactory(std::wstring const & implementationName);
//  };
//}




# endif // RTI1516_FED_TIME_H

