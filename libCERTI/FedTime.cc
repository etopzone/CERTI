// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libcerti
//
// CERTI-libcerti is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libcerti is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: FedTime.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "fedtime.hh"

namespace certi {

//-----------------------------------------------------------------------
// FedTimeFactory
//-----------------------------------------------------------------------

FedTime* FedTimeFactory::makeZero()
throw(
  MemoryExhausted)
{
  RTIfedTime *fedtime;
  fedtime->_fedTime = 0;
  return fedtime;
}

FedTime* FedTimeFactory::decode(const char *buf)
throw(
  MemoryExhausted)
{
  //not implemented
  throw MemoryExhausted();
}

//-----------------------------------------------------------------------
// FedTime
//-----------------------------------------------------------------------

FedTime::~FedTime() {

}


//-----------------------------------------------------------------------
// RTIfedTime
//-----------------------------------------------------------------------

//-----------------------------------------------------------------
// Constructors and Destructors
//-----------------------------------------------------------------
RTIfedTime::RTIfedTime()
{
  _fedTime = 0;
  _zero = 0;
}

RTIfedTime::RTIfedTime(const Double& dble)
{
  _fedTime = dble;
  initZero() ;
}

RTIfedTime::RTIfedTime(const FedTime& time)
{
  RTIfedTime *tmp ; 
  if(tmp = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    _fedTime = tmp->_fedTime ;
    _zero =  tmp->_zero ;
  }
}
    

RTIfedTime::RTIfedTime(const RTIfedTime& time)
{
  _fedTime = time._fedTime;
  _zero = time._zero;
}

RTIfedTime::~RTIfedTime()  { }

//-----------------------------------------------------------------
// Overloaded functions from FedTime
//-----------------------------------------------------------------
void                RTIfedTime::setZero()
{
  _zero = 0;
}

Boolean             RTIfedTime::isZero()
{
  if(_zero == 0)
    return RTI_TRUE;
  else
    return RTI_FALSE;
}

void                RTIfedTime::setEpsilon()
{
  //not implemented
}

void                RTIfedTime::setPositiveInfinity()
{
  //not implemented
}

Boolean        	    RTIfedTime::isPositiveInfinity()
{
  //not implemented
  return RTI_FALSE;      //  compatibilite avec le type retourne 
}


int                 RTIfedTime::encodedLength() const
{
  //not implemented
  return 0;      //  compatibilite avec le type retourne 
}

void                RTIfedTime::encode(char *buff) const
{
  //not implemented
}

int                 RTIfedTime::getPrintableLength() const
{
  //not implemented
  return 0;      //  compatibilite avec le type retourne 
}

void                RTIfedTime::getPrintableString(char*)
{
  //not implemented
}


//-----------------------------------------------------------------
// Overloaded operators from FedTime
//-----------------------------------------------------------------
FedTime& RTIfedTime::operator+=(const FedTime& time)
throw(InvalidFederationTime)
{
  RTIfedTime *fedtime  ;
  if( fedtime = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    _fedTime += fedtime->_fedTime;
    initZero() ;
    return *this;
  }
  else
    throw( new InvalidFederationTime() );
}

FedTime& RTIfedTime::operator-=(const FedTime& time)
throw(InvalidFederationTime)
{
  RTIfedTime *fedtime ; 
    if(fedtime = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    _fedTime -= fedtime->_fedTime;
    initZero() ;
    return *this;
  }
  else
    throw( new InvalidFederationTime() );
}

Boolean RTIfedTime::operator<=(const FedTime& time) const
throw(InvalidFederationTime)
{
  RTIfedTime *fedtime ;
  if(fedtime = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    if(_fedTime <= fedtime->_fedTime)
      return RTI_TRUE;
    else
      return RTI_FALSE;
  }
  else
    throw( new InvalidFederationTime() );
}

Boolean RTIfedTime::operator<(const FedTime& time) const
throw(InvalidFederationTime)
{
  RTIfedTime *fedtime ;
  if(fedtime = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    if(_fedTime < fedtime->_fedTime)
      return RTI_TRUE;
    else
      return RTI_FALSE;
  }
  else
    throw( new InvalidFederationTime() );
}

Boolean RTIfedTime::operator>=(const FedTime& time) const
throw(InvalidFederationTime)
{
  RTIfedTime *fedtime ;
  if(fedtime = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    if(_fedTime >= fedtime->_fedTime)
      return RTI_TRUE;
    else
      return RTI_FALSE;
  }
  else
    throw( new InvalidFederationTime() );
}
 
Boolean RTIfedTime::operator>(const FedTime& time) const
throw(InvalidFederationTime)
{
  RTIfedTime *fedtime ;
  if(fedtime = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    if(_fedTime > fedtime->_fedTime)
      return RTI_TRUE;
    else
      return RTI_FALSE;
  }
  else
    throw( new InvalidFederationTime() );
}

Boolean RTIfedTime::operator==(const FedTime& time) const
throw(InvalidFederationTime)
{
  RTIfedTime *fedtime ;
  if(fedtime = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    if(_fedTime == fedtime->_fedTime)
      return RTI_TRUE;
    else
      return RTI_FALSE;
  }
  else
    throw( new InvalidFederationTime() );
}
 

FedTime& RTIfedTime::operator=(const FedTime& time)
throw(InvalidFederationTime)
{
  RTIfedTime *fedtime ;
  if(fedtime = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    _fedTime = fedtime->_fedTime;
    initZero() ;
    return *this;
  }
  else
    throw( new InvalidFederationTime() );
}
 
//-----------------------------------------------------------------
// Implementation functions
//-----------------------------------------------------------------
Double         RTIfedTime::getTime() const
{
  return _fedTime;
}
 
//-----------------------------------------------------------------
// Implementation operators
//-----------------------------------------------------------------
/*Boolean RTIfedTime::operator==(const Double& time) const
  throw(InvalidFederationTime)
  {
  if(_fedTime == time)
  return RTI_TRUE;
  else
  return RTI_FALSE;
  }*/

Boolean RTIfedTime::operator!=(const FedTime& time) const
throw(InvalidFederationTime)
{
  RTIfedTime *fedtime ;
  if( fedtime = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    if(_fedTime != fedtime->_fedTime)
      return RTI_TRUE;
    else
      return RTI_FALSE;
  }
  else
    throw InvalidFederationTime() ;
}

/*Boolean RTIfedTime::operator!=(const Double& time) const
  throw(InvalidFederationTime)
  {
  if(_fedTime != time)
  return RTI_TRUE;
  else
  return RTI_FALSE;
  }
  */

FedTime& RTIfedTime::operator=(const RTIfedTime& time)
throw(InvalidFederationTime)
{
  //  RTIfedTime *fedtime = new RTIfedTime();
  // fedtime->_fedTime =((RTIfedTime)time)._fedTime;
  _fedTime = time._fedTime;
  _zero = time._zero ;
  return *this ;
}
 
/* FedTime& RTIfedTime::operator=(const Double& time)
   throw(InvalidFederationTime)
   {
   RTIfedTime *fedtime = new RTIfedTime();
   fedtime->_fedTime = time;
   return *fedtime;
   }
   */
 
/* FedTime& RTIfedTime::operator*=(const FedTime& time)
   throw(InvalidFederationTime)
   {
   RTIfedTime *fedtime = new RTIfedTime();
   fedtime = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time)));
   fedtime->_fedTime *= _fedTime;
   _fedTime = fedtime->_fedTime;
   return *fedtime;
   }
   
 
   FedTime& RTIfedTime::operator/=(const FedTime& time)
   throw(InvalidFederationTime)
   {
   RTIfedTime *fedtime = new RTIfedTime();
   fedtime = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time)));
   fedtime->_fedTime /= _fedTime;
   _fedTime = fedtime->_fedTime;
   return *fedtime;
   }
   */
 
/* FedTime& RTIfedTime::operator+=(const Double& time)
   throw(InvalidFederationTime)
   {
   RTIfedTime *fedtime;
   fedtime->_fedTime = _fedTime + time;
   _fedTime = _fedTime + time;
   return *fedtime;
   }
  
   FedTime& RTIfedTime::operator-=(const Double& time)
   throw(InvalidFederationTime)
   {
   RTIfedTime *fedtime = new RTIfedTime();
   fedtime->_fedTime = _fedTime - time;
   _fedTime = _fedTime - time;
   return *fedtime;
   }
   
   FedTime& RTIfedTime::operator*=(const Double& time)
   throw(InvalidFederationTime)
   {
   RTIfedTime *fedtime = new RTIfedTime();
   fedtime->_fedTime = _fedTime * time;
   _fedTime = _fedTime * time;
   return *fedtime;
   }
 
   FedTime& RTIfedTime::operator/=(const Double& time)
   throw(InvalidFederationTime)
   {
   RTIfedTime *fedtime = new RTIfedTime();
   fedtime->_fedTime = _fedTime / time;
   _fedTime = _fedTime / time;
   return *fedtime;
   }
   */

RTIfedTime  RTIfedTime::operator+(const FedTime& time)
throw(InvalidFederationTime)
{
  RTIfedTime *param ;
  if(param = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    RTIfedTime fedtime = _fedTime + param->_fedTime;
    fedtime.initZero() ;
    return fedtime;
  }
else
  throw InvalidFederationTime() ;
}
 
/* RTIfedTime RTIfedTime::operator+(const Double& time)
   throw(InvalidFederationTime)
   {
   RTIfedTime *fedtime = new RTIfedTime();
   fedtime->_fedTime = _fedTime + time;
   return *fedtime;
   }
   */
 
RTIfedTime RTIfedTime::operator-(const FedTime& time)
throw(InvalidFederationTime)
{
  RTIfedTime *param ;
  if( param = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    RTIfedTime fedtime = _fedTime - param->_fedTime;
    fedtime.initZero() ;
    return fedtime;
  }
  else
    throw InvalidFederationTime() ;
}
 
/* RTIfedTime RTIfedTime::operator-(const Double& time)
   throw(InvalidFederationTime)
   {
   RTIfedTime *fedtime = new RTIfedTime();
   fedtime->_fedTime = _fedTime - time;
   return *fedtime;
   }
   */

RTIfedTime RTIfedTime::operator*(const FedTime& time)
throw(InvalidFederationTime)
{
  RTIfedTime *param ;
  if( param = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    RTIfedTime fedtime = _fedTime * param->_fedTime;
    fedtime.initZero() ;
    return fedtime;
  }
  else
    throw InvalidFederationTime() ;   
}
 
/* RTIfedTime RTIfedTime::operator*(const Double& time)
   throw(InvalidFederationTime)
   {
   RTIfedTime *fedtime = new RTIfedTime();
   fedtime->_fedTime = _fedTime * time;
   return *fedtime;
   }
   */
 
RTIfedTime RTIfedTime::operator/(const FedTime& time)
throw(InvalidFederationTime)
{
  RTIfedTime *param ;
  if( param = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    RTIfedTime fedtime = _fedTime / param->_fedTime ;
    fedtime.initZero() ;
    return fedtime;
  }
  else
    throw InvalidFederationTime() ;  
}
 
/*  RTIfedTime RTIfedTime::operator/(const Double& time)
    throw(InvalidFederationTime)
    {
    RTIfedTime *fedtime = new RTIfedTime();
    fedtime->_fedTime = _fedTime / time;
    return *fedtime;
    }
    */
 
//-----------------------------------------------------------------
// Implementation friends
//-----------------------------------------------------------------
//ostream& operator<<(ostream&, const FedTime&)

//-----------------------------------------------------------------
// Global operators
//-----------------------------------------------------------------

RTIfedTime operator+(const Double& d, const FedTime& time)
throw(InvalidFederationTime)
{
  RTIfedTime *param;
  if(param = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    RTIfedTime fedtime = d + param->_fedTime ;
    fedtime.initZero() ;
    return fedtime;
  }
  else
    throw InvalidFederationTime() ;  
}

RTIfedTime operator-(const Double& d, const FedTime& time)
throw(InvalidFederationTime)
{
  RTIfedTime *param;
  if(param = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    RTIfedTime fedtime = d - param->_fedTime ;
    fedtime.initZero() ;
    return fedtime;
  }
  else
    throw InvalidFederationTime() ;  
}
 
RTIfedTime operator*(const Double& d, const FedTime& time)
throw(InvalidFederationTime)
{
  RTIfedTime *param;
  if(param = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    RTIfedTime fedtime = d * param->_fedTime ;
    fedtime.initZero() ;
    return fedtime;
  }
  else
    throw InvalidFederationTime() ;  
}
 
RTIfedTime operator/(const Double& d, const FedTime& time)
throw(InvalidFederationTime)
{
  RTIfedTime *param;
  if(param = dynamic_cast<RTIfedTime *>(&(const_cast<FedTime&>(time))) ) {
    RTIfedTime fedtime = d /  param->_fedTime ;
    fedtime.initZero() ;
    return fedtime;
  }
  else
    throw InvalidFederationTime() ;  
}

} 

// $Id: FedTime.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
