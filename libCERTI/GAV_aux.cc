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
// $Id: GAV_aux.cc,v 3.3 2002/11/27 17:45:33 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "RTItypes.hh"
#include "PrettyDebug.hh"

//-----------------------------------------------------------------------
// AttributeHandleValuePair
//-----------------------------------------------------------------------

namespace certi {

AttributeHandleValuePair::AttributeHandleValuePair()
{
  _handle = 0;
  _valuelength = 0;
  _value = new char [1024];
  _next =(AttributeHandleValuePair *)NULL;
}

AttributeHandleValuePair::~AttributeHandleValuePair()
{
}

//-----------------------------------------------------------------------
// AttributeHandleValuePairSet
//-----------------------------------------------------------------------
AttributeHandleValuePairSet::~AttributeHandleValuePairSet()
{
  empty();
}

inline ULong 
AttributeHandleValuePairSet::size() const
{
  return _size;
}
  
Handle 
AttributeHandleValuePairSet::getHandle(ULong i) const
  throw(ArrayIndexOutOfBounds)
{
  AttributeHandleValuePair *tmp = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(j==i) return tmp->_handle;
    else tmp = tmp->_next;
  }
  throw ArrayIndexOutOfBounds();
}
  
ULong 
AttributeHandleValuePairSet::getValueLength(ULong i) const
  throw(ArrayIndexOutOfBounds)
{
  AttributeHandleValuePair *tmp = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(j==i) return tmp->_valuelength;
    else tmp = tmp->_next;
  }
  throw ArrayIndexOutOfBounds();
}
  
void 
AttributeHandleValuePairSet::getValue(ULong i, 
				      char* buff, 
				      ULong& valueLength) const
  throw(ArrayIndexOutOfBounds)
{
  AttributeHandleValuePair *tmp = _head;

  if(i>=_size) throw ArrayIndexOutOfBounds();
  else {
    for(unsigned int j=0; j<i; j++) tmp = tmp->_next ;    
    memcpy(buff, tmp->_value, tmp->_valuelength);
    valueLength = tmp->_valuelength;
  }
}
   
char *
AttributeHandleValuePairSet::getValuePointer(ULong i, 
					     ULong& valueLength) const
  throw(ArrayIndexOutOfBounds)
{
  AttributeHandleValuePair *tmp = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(j==i) {
      valueLength = tmp->_valuelength;
      return tmp->_value;
    }
    else tmp = tmp->_next;
  }
  throw ArrayIndexOutOfBounds();
}
  
inline TransportType 
AttributeHandleValuePairSet::getTransportType(ULong) const
  throw(ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext)
{
  return _transport;
}
  
inline OrderType 
AttributeHandleValuePairSet::getOrderType(ULong) const
  throw(ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext)
{
  return _order;
}
  
Region*
AttributeHandleValuePairSet::getRegion(ULong) const
  throw(ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext,
  UnimplementedService)//CERTI
{
  throw UnimplementedService();
}
  
void 
AttributeHandleValuePairSet::add(Handle h,
				 const char* buff,
				 ULong valueLength) 
  throw(ValueLengthExceeded, ValueCountExceeded)
{
  AttributeHandleValuePair *newNode = new AttributeHandleValuePair();
  newNode->_next = _head;
  newNode->_handle = h;
  newNode->_valuelength = valueLength;
  memcpy(newNode->_value, buff, valueLength);
  _head = newNode;
  _size++;
}
  
void 
AttributeHandleValuePairSet::remove(Handle h)
  throw(ArrayIndexOutOfBounds)
{
  AttributeHandleValuePair *tmp = _head;
  AttributeHandleValuePair *prec = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(tmp->_handle == h) {
      if(j==0) _head = _head->_next;	  
      else prec->_next = tmp->_next; 
      
      delete tmp;
      _size--;
      return;
    }
    prec = tmp;
    tmp = tmp->_next;
  }

  throw ArrayIndexOutOfBounds();
}
  
void 
AttributeHandleValuePairSet::moveFrom(const AttributeHandleValuePairSet&,
				      ULong&)
  throw(ValueCountExceeded, ArrayIndexOutOfBounds,
	UnimplementedService) //CERTI
{
  throw UnimplementedService();
}
  
void 
AttributeHandleValuePairSet::empty() 
{
  AttributeHandleValuePair * tmp  = _head;
  AttributeHandleValuePair * next = _head->_next;
  for (unsigned int i = 0 ; i < _size-1 ; i++) {
    delete tmp;
    tmp = next;
    next = next->_next;
  }
  delete tmp;

  _head =(AttributeHandleValuePair *)NULL;
  _size = 0;
}

ULong 
AttributeHandleValuePairSet::start() const
{
//not implemented
return 0;
}

ULong 
AttributeHandleValuePairSet::valid(ULong) const
{
  //not implemented
  return 0;
}

ULong 
AttributeHandleValuePairSet::next(ULong) const
{
  //not implemented
  return 0;
}

//-----------------------------------------------------------------------
// AttributeSetFactory
//-----------------------------------------------------------------------

AttributeHandleValuePairSet* 
AttributeSetFactory::create(ULong)
  throw(MemoryExhausted, ValueCountExceeded, HandleValuePairMaximumExceeded)
{
  AttributeHandleValuePairSet *ahvps = new AttributeHandleValuePairSet;
  ahvps->_order = RECEIVE;
  ahvps->_transport = RELIABLE;
  ahvps->_head =(AttributeHandleValuePair *)NULL;
  ahvps->_size = 0;
  return ahvps;
}

//-----------------------------------------------------------------------
// HandlePair
//-----------------------------------------------------------------------

HandlePair::HandlePair()
{
  _handle = 0;
  _next =(HandlePair *)NULL;
}

HandlePair::~HandlePair()
{
}

//-----------------------------------------------------------------------
// AttributeHandleSet
//-----------------------------------------------------------------------
AttributeHandleSet::~AttributeHandleSet()
{
  empty();
}

inline ULong 
AttributeHandleSet::size() const
{
  return _size;
}

AttributeHandle 
AttributeHandleSet::getHandle(ULong i) const
  throw(ArrayIndexOutOfBounds)
{
  HandlePair *tmp = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(j==i) return tmp->_handle;
    else tmp = tmp->_next;
  }
  throw ArrayIndexOutOfBounds();
}
  
void 
AttributeHandleSet::add(AttributeHandle h)
  throw(ArrayIndexOutOfBounds,
	AttributeNotDefined)
{
  HandlePair *newNode = new HandlePair();
  newNode->_next = _head;
  newNode->_handle = h;
  _head = newNode;
  _size++;
}
  
void 
AttributeHandleSet::remove(AttributeHandle h)
  throw(AttributeNotDefined)// not guaranteed safe while iterating
{
  HandlePair *tmp = _head;
  HandlePair *prec = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(tmp->_handle == h) {
      if(j==0) _head = _head->_next;	  
      else prec->_next = tmp->_next; 	  
      
      delete tmp;
      _size--;
      return;
    }
    prec = tmp;
    tmp = tmp->_next;
  }
  
  throw AttributeNotDefined();
}
  
void 
AttributeHandleSet::empty()
{
  HandlePair * tmp  = _head;
  HandlePair * next = _head->_next;
  for(unsigned int i = 0 ; i < _size-1 ; i++) {
    delete tmp;
    tmp = next;
    next = next->_next;
  }
  delete tmp;

  _head =(HandlePair *)NULL;
  _size = 0;
}

inline Boolean 
AttributeHandleSet::isEmpty() const 
{
  return ((_size == 0) ? RTI_TRUE : RTI_FALSE);
}

Boolean 
AttributeHandleSet::isMember(AttributeHandle h) const
{
 HandlePair *tmp = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(tmp->_handle==h) return RTI_TRUE;
    else tmp = tmp->_next;
  }

  return RTI_FALSE;
}


//-----------------------------------------------------------------------
// AttributeHandleSetFactory
//-----------------------------------------------------------------------

AttributeHandleSet* 
AttributeHandleSetFactory::create(ULong)
throw(MemoryExhausted, ValueCountExceeded)
{
  AttributeHandleSet *ahs = new AttributeHandleSet();
  ahs->_head =(HandlePair *)NULL;
  ahs->_size = 0;
  return ahs;
}

//-----------------------------------------------------------------------
// ParameterHandleValuePair
//-----------------------------------------------------------------------

ParameterHandleValuePair::ParameterHandleValuePair()
{
  _handle = 0;
  _valuelength = 0;
  _value = new char [1024];
  _next =(ParameterHandleValuePair *)NULL;
}

ParameterHandleValuePair::~ParameterHandleValuePair()
{
}

//-----------------------------------------------------------------------
// ParameterHandleValuePairSet
//-----------------------------------------------------------------------
ParameterHandleValuePairSet::~ParameterHandleValuePairSet()
{
  empty();
}

inline ULong 
ParameterHandleValuePairSet::size() const
{
  return _size;
}
 
Handle 
ParameterHandleValuePairSet::getHandle(ULong i) const
  throw(ArrayIndexOutOfBounds)
{
  ParameterHandleValuePair *tmp = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(j==i) return tmp->_handle;
    else tmp = tmp->_next;
  }
  throw ArrayIndexOutOfBounds();
}
  
ULong 
ParameterHandleValuePairSet::getValueLength(ULong i) const
  throw(ArrayIndexOutOfBounds)
{
  ParameterHandleValuePair *tmp = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(j==i) return tmp->_valuelength;
    else tmp = tmp->_next;
  }
  throw ArrayIndexOutOfBounds();
}
  
void 
ParameterHandleValuePairSet::getValue(ULong i,
				      char* buff,
				      ULong& valueLength) const
  throw(ArrayIndexOutOfBounds)
{
  ParameterHandleValuePair *tmp = _head;

  if(i>=_size) throw ArrayIndexOutOfBounds();
  else {
    for(unsigned int j=0; j<i; j++) tmp = tmp->_next;    
    memcpy(buff, tmp->_value, tmp->_valuelength);
    valueLength = tmp->_valuelength;
  }
}
  
char *
ParameterHandleValuePairSet::getValuePointer(ULong i,
					     ULong& valueLength) const
  throw(ArrayIndexOutOfBounds)
{
  ParameterHandleValuePair *tmp = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(j==i) {
      valueLength = tmp->_valuelength;
      return tmp->_value;
    }
    else tmp = tmp->_next;
  }

  throw ArrayIndexOutOfBounds();
}
  
inline TransportType 
ParameterHandleValuePairSet::getTransportType(void) const
  throw(InvalidHandleValuePairSetContext) 
{
  return _transport;
}
  
inline OrderType 
ParameterHandleValuePairSet::getOrderType(void) const
  throw(InvalidHandleValuePairSetContext)
{
  return _order;
}
  
Region *
ParameterHandleValuePairSet::getRegion(void) const
  throw(InvalidHandleValuePairSetContext,  
	UnimplementedService)//CERTI
{
  throw UnimplementedService();
}
  
void 
ParameterHandleValuePairSet::add(Handle h, 
				 const char* buff, 
				 ULong valueLength)
  throw(ValueLengthExceeded, ValueCountExceeded)
{
  ParameterHandleValuePair *newNode = new ParameterHandleValuePair();
  newNode->_next = _head;
  newNode->_handle = h;
  newNode->_valuelength = valueLength;
  memcpy(newNode->_value, buff, valueLength);
  _head = newNode;
  _size++;
}
  
// not guaranteed safe while iterating
void 
ParameterHandleValuePairSet::remove(Handle h)
  throw(ArrayIndexOutOfBounds)
{
  ParameterHandleValuePair *tmp = _head;
  ParameterHandleValuePair *prec = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(tmp->_handle == h) {
      if(j==0) _head = _head->_next;
      else prec->_next = tmp->_next; 
      delete tmp;
      _size--;
      return;
    }
    prec = tmp;
    tmp = tmp->_next;
  }

  throw ArrayIndexOutOfBounds();
}
  
void 
ParameterHandleValuePairSet::moveFrom(const ParameterHandleValuePairSet&,
				      ULong&)
  throw (ValueCountExceeded, ArrayIndexOutOfBounds,
  UnimplementedService)//CERTI
{
  throw UnimplementedService();
}
  

void 
ParameterHandleValuePairSet::empty(void) 
{
  ParameterHandleValuePair * tmp  = _head;
  ParameterHandleValuePair * next = _head->_next;
  for (unsigned int i = 0 ; i < _size-1 ; i++) {
      delete tmp;
      tmp = next;
      next = next->_next;
  }
  delete tmp;

  _head = (ParameterHandleValuePair *) NULL;
  _size = 0;
}

ULong 
ParameterHandleValuePairSet::start(void) const
{
  //not implemented
  return 0;
}

ULong 
ParameterHandleValuePairSet::valid(ULong) const
{
  //not implemented
  return 0;
}

ULong 
ParameterHandleValuePairSet::next(ULong) const
{
  //not implemented
  return 0;
}


//-----------------------------------------------------------------------
// ParameterSetFactory
//-----------------------------------------------------------------------

ParameterHandleValuePairSet* 
ParameterSetFactory::create(ULong)
  throw (MemoryExhausted, ValueCountExceeded, 
	 HandleValuePairMaximumExceeded)
{
  ParameterHandleValuePairSet *phvps = new ParameterHandleValuePairSet;
  phvps->_order = RECEIVE;
  phvps->_transport = RELIABLE;
  phvps->_head =(ParameterHandleValuePair *)NULL;
  phvps->_size = 0;
  return phvps;
}

}

// $Id: GAV_aux.cc,v 3.3 2002/11/27 17:45:33 breholee Exp $
