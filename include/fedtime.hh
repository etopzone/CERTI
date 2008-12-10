// HLA 1.3 Header "fedtime.hh"
// $Id: fedtime.hh,v 3.12 2008/12/10 17:42:43 gotthardp Exp $

#ifndef HLA_FEDTIME_HH
#define HLA_FEDTIME_HH

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <string.h>
#endif

#if defined(_WIN32)
    #if defined(FedTime_EXPORTS)
        #define FEDTIME_EXPORT __declspec(dllexport)
    #else
        #define FEDTIME_EXPORT __declspec(dllimport)
    #endif
#else
	#define FEDTIME_EXPORT
#endif

#include "RTI.hh"

class FEDTIME_EXPORT RTIfedTime : public RTI::FedTime
{
public:

	static const double epsilon;
	static int fcmp(const double x1, const double x2);

    RTIfedTime();
    RTIfedTime(const RTI::Double &);
    RTIfedTime(const RTI::FedTime &);
    RTIfedTime(const RTIfedTime &);
    virtual ~RTIfedTime();

public:
    virtual void setZero();
    virtual RTI::Boolean isZero();
    virtual void setEpsilon();
    virtual void setPositiveInfinity();
    virtual RTI::Boolean isPositiveInfinity();
    virtual int encodedLength() const;
    virtual void encode(char *) const;
    virtual int getPrintableLength() const;
    virtual void getPrintableString(char *);

    virtual RTI::FedTime& operator+=(const RTI::FedTime &)
	throw (RTI::InvalidFederationTime);

    virtual RTI::FedTime& operator-=(const RTI::FedTime &)
	throw (RTI::InvalidFederationTime);

    virtual RTI::Boolean operator<=(const RTI::FedTime &) const
	throw (RTI::InvalidFederationTime);

    virtual RTI::Boolean operator<(const RTI::FedTime &) const
	throw (RTI::InvalidFederationTime);

    virtual RTI::Boolean operator>=(const RTI::FedTime &) const
	throw (RTI::InvalidFederationTime);

    virtual RTI::Boolean operator>(const RTI::FedTime &) const
	throw (RTI::InvalidFederationTime);

    virtual RTI::Boolean operator==(const RTI::FedTime &) const
	throw (RTI::InvalidFederationTime);

    virtual RTI::FedTime & operator=(const RTI::FedTime &)
	throw (RTI::InvalidFederationTime);

    virtual RTI::Double getTime() const;

    virtual RTI::Boolean operator==(const RTI::Double &) const
	throw (RTI::InvalidFederationTime);

    virtual RTI::Boolean operator!=(const RTI::FedTime &) const
	throw (RTI::InvalidFederationTime);

    virtual RTI::Boolean operator!=(const RTI::Double &) const
	throw (RTI::InvalidFederationTime);

    virtual RTI::FedTime &operator=(const RTIfedTime &)
	throw (RTI::InvalidFederationTime);

    virtual RTI::FedTime &operator=(const RTI::Double &)
	throw (RTI::InvalidFederationTime);

    virtual RTI::FedTime &operator*=(const RTI::FedTime &)
	throw (RTI::InvalidFederationTime);

    virtual RTI::FedTime &operator/=(const RTI::FedTime &)
	throw (RTI::InvalidFederationTime);

    virtual RTI::FedTime &operator+=(const RTI::Double &)
	throw (RTI::InvalidFederationTime);

    virtual RTI::FedTime &operator-=(const RTI::Double &)
	throw (RTI::InvalidFederationTime);

    virtual RTI::FedTime &operator*=(const RTI::Double &)
	throw (RTI::InvalidFederationTime);

    virtual RTI::FedTime &operator/=(const RTI::Double &)
	throw (RTI::InvalidFederationTime);

    virtual RTIfedTime operator+(const RTI::FedTime &)
	throw (RTI::InvalidFederationTime);

    virtual RTIfedTime operator+(const RTI::Double &)
	throw (RTI::InvalidFederationTime);

    virtual RTIfedTime operator-(const RTI::FedTime &)
	throw (RTI::InvalidFederationTime);

    virtual RTIfedTime operator-(const RTI::Double &)
	throw (RTI::InvalidFederationTime);

    virtual RTIfedTime operator*(const RTI::FedTime &)
	throw (RTI::InvalidFederationTime);

    virtual RTIfedTime operator*(const RTI::Double &)
	throw (RTI::InvalidFederationTime);

    virtual RTIfedTime operator/(const RTI::FedTime &)
	throw (RTI::InvalidFederationTime);

    virtual RTIfedTime operator/(const RTI::Double &)
	throw (RTI::InvalidFederationTime);

    friend RTI_STD::ostream FEDTIME_EXPORT &operator<<(RTI_STD::ostream&, const RTI::FedTime &);

private:
    RTI::Double _fedTime ;
    RTI::Double _zero ;
    RTI::Double _epsilon ;
    RTI::Double _positiveInfinity ;
};

RTIfedTime operator+(const RTI::Double &, const RTI::FedTime &);
RTIfedTime operator-(const RTI::Double &, const RTI::FedTime &);
RTIfedTime operator*(const RTI::Double &, const RTI::FedTime &);
RTIfedTime operator/(const RTI::Double &, const RTI::FedTime &);

#endif // HLA_FEDTIME_HH

// $Id: fedtime.hh,v 3.12 2008/12/10 17:42:43 gotthardp Exp $
