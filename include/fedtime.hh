// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libRTI
//
// CERTI-libRTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libRTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: fedtime.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_FED_TIME_HH
#define _CERTI_FED_TIME_HH

#include <iostream>

#include <sys/types.h>
#include <cstring>
#include <stdio.h>

#include "baseTypes.hh"
#include "RTItypes.hh"

using namespace std ;

namespace certi {

class FedTime {
public:
  virtual ~FedTime();
  virtual void setZero() = 0;
  virtual Boolean isZero() = 0;
  virtual void setEpsilon() = 0;
  virtual void setPositiveInfinity() = 0;
  virtual Boolean isPositiveInfinity() = 0;
 
  virtual FedTime& operator+=(const FedTime&)
    throw(InvalidFederationTime) = 0;

  virtual FedTime& operator-=(const FedTime&)
    throw(InvalidFederationTime) = 0;
 
  virtual Boolean operator<=(const FedTime&) const
    throw(InvalidFederationTime) = 0;

  virtual Boolean operator<(const FedTime&) const
    throw(InvalidFederationTime) = 0;

  virtual Boolean operator>=(const FedTime&) const
    throw(InvalidFederationTime) = 0;
 
  virtual Boolean operator>(const FedTime&) const
    throw(InvalidFederationTime) = 0;

  virtual Boolean operator==(const FedTime&) const
    throw(InvalidFederationTime) = 0;
 
  virtual FedTime& operator=(const FedTime&)
    throw(InvalidFederationTime) = 0;

  virtual int encodedLength() const = 0;
  virtual void encode(char *buff) const = 0;
  virtual int getPrintableLength() const = 0;
  virtual void getPrintableString(char*) = 0; 
};

class FedTimeFactory {
public:
  static FedTime* makeZero() throw(MemoryExhausted);
  static FedTime* decode(const char *buf) throw(MemoryExhausted);
};

class RTIfedTime : public FedTime {
  //-----------------------------------------------------------------
  // Constructors and Destructors
  //-----------------------------------------------------------------
public:
  RTIfedTime();
  RTIfedTime(const Double&);
  RTIfedTime(const FedTime&);
  RTIfedTime(const RTIfedTime&);
  virtual ~RTIfedTime();

  //-----------------------------------------------------------------
  // Overloaded functions from FedTime
  //-----------------------------------------------------------------
public:
  void setZero();
  Boolean isZero();
  void setEpsilon();
  void setPositiveInfinity();
  Boolean isPositiveInfinity();
  int encodedLength() const;
  void encode(char *buff) const;
  int getPrintableLength() const;
  void getPrintableString(char*);

  //-----------------------------------------------------------------
  // Overloaded operators from FedTime
  //-----------------------------------------------------------------
public:
  FedTime& operator+=(const FedTime&)
    throw(InvalidFederationTime);

  FedTime& operator-=(const FedTime&)
    throw(InvalidFederationTime);
 
  Boolean operator<=(const FedTime&) const
    throw(InvalidFederationTime);

  Boolean operator<(const FedTime&) const
    throw(InvalidFederationTime);

  Boolean operator>=(const FedTime&) const
    throw(InvalidFederationTime);
 
  Boolean operator>(const FedTime&) const
    throw(InvalidFederationTime);

  Boolean operator==(const FedTime&) const
    throw(InvalidFederationTime);
 
  FedTime& operator=(const FedTime&)
    throw(InvalidFederationTime);

  //-----------------------------------------------------------------
  // Implementation functions
  //-----------------------------------------------------------------
public:
  Double getTime() const;

  //-----------------------------------------------------------------
  // Implementation operators
  //-----------------------------------------------------------------
  // Boolean operator==(const Double&) const
  // throw(InvalidFederationTime); 

  Boolean operator!=(const FedTime&) const
    throw(InvalidFederationTime);

  FedTime& operator=(const RTIfedTime&)
    throw(InvalidFederationTime);

  FedTime& operator*=(const FedTime&)
    throw(InvalidFederationTime);

  FedTime& operator/=(const FedTime&)
    throw(InvalidFederationTime);

  RTIfedTime operator+(const FedTime&)
    throw(InvalidFederationTime);

  RTIfedTime operator-(const FedTime&)
    throw(InvalidFederationTime);

  RTIfedTime operator*(const FedTime&)
    throw(InvalidFederationTime);

  RTIfedTime operator/(const FedTime&)
    throw(InvalidFederationTime);

  //-----------------------------------------------------------------
  // Implementation friends
  //-----------------------------------------------------------------
public:
  friend ostream& operator<<(ostream&, const FedTime&);

  //-----------------------------------------------------------------
  // Implementation member variables
  //-----------------------------------------------------------------
public :
  Double _fedTime;
  Double _zero;
  Double _epsilon;
  Double _positiveInfinity;

  //------------------------
  // private inline function 
  //------------------------
public: 
  inline void initZero(){
    _zero = _fedTime == 0?0:1 ; 
  }
};

//-----------------------------------------------------------------
// Global operators
//-----------------------------------------------------------------

RTIfedTime operator+(const Double&, const FedTime&)
  throw(InvalidFederationTime);
RTIfedTime operator-(const Double&, const FedTime&)
  throw(InvalidFederationTime);
RTIfedTime operator*(const Double&, const FedTime&)
  throw(InvalidFederationTime);
RTIfedTime operator/(const Double&, const FedTime&)
  throw(InvalidFederationTime);

}

#endif // _CERTI_FED_TIME_HH

// $Id: fedtime.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $

