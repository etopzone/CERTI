// HLA 1.3 Header "fedtime.hh"
// $Id: fedtime.hh,v 3.13 2009/04/02 19:58:11 erk Exp $

#ifndef HLA_FEDTIME_HH
#define HLA_FEDTIME_HH

#include "RTI.hh"
#include <sys/types.h>
#include <string.h>

class FEDTIME_EXPORT RTIfedTime : public RTI::FedTime
{
public:
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

// $Id: fedtime.hh,v 3.13 2009/04/02 19:58:11 erk Exp $
