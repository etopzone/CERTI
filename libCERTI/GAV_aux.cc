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
// $Id: GAV_aux.cc,v 3.0.2.1 2002/11/22 00:52:56 breholee Exp $
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



ULong AttributeHandleValuePairSet::size() const
{
  return _size;
}
  
Handle AttributeHandleValuePairSet::getHandle(
  ULong i) const
throw(
  ArrayIndexOutOfBounds)
{
  AttributeHandleValuePair *tmp = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(j==i)
      return tmp->_handle;
    else
      tmp = tmp->_next;
  }

  throw ArrayIndexOutOfBounds();
}
  
ULong AttributeHandleValuePairSet::getValueLength(
  ULong i) const
throw(
  ArrayIndexOutOfBounds)
{
  AttributeHandleValuePair *tmp = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(j==i) 
      return tmp->_valuelength;
    else
      tmp = tmp->_next;
  }

  throw ArrayIndexOutOfBounds();
}
  
void AttributeHandleValuePairSet::getValue(
  ULong i,
  char*      buff,
  ULong&     valueLength) const
throw(
    ArrayIndexOutOfBounds)
{
  AttributeHandleValuePair *tmp = _head;

  if(i>=_size)
    throw ArrayIndexOutOfBounds();
  else {
    for(unsigned int j=0; j<i; j++) {
      tmp = tmp->_next;
    }
    for(unsigned int i=0; i<tmp->_valuelength; i++) buff[i] = tmp->_value[i];
    // strcpy(buff,tmp->_value);
    valueLength = tmp->_valuelength;
  }

//  throw ArrayIndexOutOfBounds();
}
   
char *AttributeHandleValuePairSet::getValuePointer(
  ULong i,
  ULong&     valueLength) const
throw(
  ArrayIndexOutOfBounds)
{
  AttributeHandleValuePair *tmp = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(j==i) {
      valueLength = tmp->_valuelength;
      return tmp->_value;
    }
    else
      tmp = tmp->_next;
  }

  throw ArrayIndexOutOfBounds();
}
  
TransportType 
AttributeHandleValuePairSet::getTransportType(ULong) const
  throw(ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext)
{
  return _transport;
}
  
OrderType 
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
  
void AttributeHandleValuePairSet::add(
  Handle      h,
  const char* buff,
  ULong       valueLength)
throw(
    ValueLengthExceeded,
    ValueCountExceeded)
{
  AttributeHandleValuePair *newNode = new AttributeHandleValuePair();
  newNode->_next = _head;
  newNode->_handle = h;
  newNode->_valuelength = valueLength;
  for(unsigned int i=0; i < valueLength; i++) newNode->_value[i] = buff[i]; 
  // strcpy(newNode->_value,buff);
  _head = newNode;
  _size++;
}
  
void AttributeHandleValuePairSet::remove(Handle h)
  // not guaranteed safe while iterating
throw(
  ArrayIndexOutOfBounds)
{
  AttributeHandleValuePair *tmp = _head;
  AttributeHandleValuePair *prec = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(tmp->_handle == h) {
      if(j==0) {
        _head = _head->_next;
        _size--;
      }
      else {
        prec->_next = tmp->_next; 
        _size--;
      }
    }
    else {
      prec = tmp;
      tmp = tmp->_next;
    }
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
  
void AttributeHandleValuePairSet::empty() 
  // Empty the Set without deallocating space.
{
  _head =(AttributeHandleValuePair *)NULL;
  _size = 0;
}

ULong AttributeHandleValuePairSet::start() const
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

ULong AttributeHandleSet::size() const
{
  return _size;
}

AttributeHandle AttributeHandleSet::getHandle(ULong i) const
throw(
  ArrayIndexOutOfBounds)
{
  HandlePair *tmp = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(j==i)
      return tmp->_handle;
    else
      tmp = tmp->_next;
  }

  throw ArrayIndexOutOfBounds();
}
  
void AttributeHandleSet::add(AttributeHandle h)
throw(
  ArrayIndexOutOfBounds,
  AttributeNotDefined)
{
  HandlePair *newNode = new HandlePair();
  newNode->_next = _head;
  newNode->_handle = h;
  _head = newNode;
  _size++;
}
  
void AttributeHandleSet::remove(AttributeHandle h)
throw(			// not guaranteed safe while iterating
  AttributeNotDefined)
{
  HandlePair *tmp = _head;
  HandlePair *prec = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(tmp->_handle == h) {
      if(j==0) {
        _head = _head->_next;
        _size--;
      }
      else {
        prec->_next = tmp->_next; 
        _size--;
      }
    }
    else {
      prec = tmp;
      tmp = tmp->_next;
    }
  }

  throw AttributeNotDefined();
}
  
void AttributeHandleSet::empty() // Empty the Set
{
  _head =(HandlePair *)NULL;
  _size = 0;
}

Boolean AttributeHandleSet::isEmpty() const //is set empty?
{
if(_size == 0)
  return RTI_TRUE;
else
  return RTI_FALSE;
}

Boolean AttributeHandleSet::isMember(AttributeHandle h) const
{
 HandlePair *tmp = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(tmp->_handle==h)
      return RTI_TRUE;
    else
      tmp = tmp->_next;
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

ULong ParameterHandleValuePairSet::size() const
{
  return _size;
}
 
Handle ParameterHandleValuePairSet::getHandle(
  ULong i) const
throw(
  ArrayIndexOutOfBounds)
{
  ParameterHandleValuePair *tmp = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(j==i)
      return tmp->_handle;
    else
      tmp = tmp->_next;
  }

  throw ArrayIndexOutOfBounds();
}
  
ULong ParameterHandleValuePairSet::getValueLength(
  ULong i) const
throw(
  ArrayIndexOutOfBounds)
{
  ParameterHandleValuePair *tmp = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(j==i)
      return tmp->_valuelength;
    else
      tmp = tmp->_next;
  }

  throw ArrayIndexOutOfBounds();
}
  
void ParameterHandleValuePairSet::getValue(
  ULong i,
  char*      buff,
  ULong&     valueLength) const
throw(
  ArrayIndexOutOfBounds)
{
  ParameterHandleValuePair *tmp = _head;

  if(i>=_size)
    throw ArrayIndexOutOfBounds();
  else {
    for(unsigned int j=0; j<i; j++) {
      tmp = tmp->_next;
    }
    for(unsigned int i=0; i<tmp->_valuelength; i++) buff[i] = tmp->_value[i];
    // strcpy(buff,tmp->_value);
    valueLength = tmp->_valuelength;
  }
}
  
char *ParameterHandleValuePairSet::getValuePointer(
  ULong i,
  ULong&     valueLength) const
throw(
  ArrayIndexOutOfBounds)
{
  ParameterHandleValuePair *tmp = _head;

  for(unsigned int j=0; j<_size; j++) {
    if(j==i) {
      valueLength = tmp->_valuelength;
      return tmp->_value;
    }
    else
      tmp = tmp->_next;
  }

  throw ArrayIndexOutOfBounds();
}
  
TransportType ParameterHandleValuePairSet::getTransportType(void) const
throw( InvalidHandleValuePairSetContext) 
{
  return _transport;
}
  
OrderType ParameterHandleValuePairSet::getOrderType(void) const
throw( InvalidHandleValuePairSetContext)
{
  return _order;
}
  
Region *ParameterHandleValuePairSet::getRegion(void) const
throw( 
  InvalidHandleValuePairSetContext,
  UnimplementedService)//CERTI
{
  throw UnimplementedService();
}
  
void ParameterHandleValuePairSet::add(
  Handle      h,
  const char* buff,
  ULong       valueLength)
throw(
  ValueLengthExceeded,
  ValueCountExceeded)
{
  ParameterHandleValuePair *newNode = new ParameterHandleValuePair();
  newNode->_next = _head;
  newNode->_handle = h;
  newNode->_valuelength = valueLength;
  for(unsigned int i=0; i < valueLength; i++) newNode->_value[i] = buff[i];

  //  strcpy(newNode->_value,buff);
  _head = newNode;
  _size++;
}
  
// not guaranteed safe while iterating
void 
ParameterHandleValuePairSet::remove(Handle)
  throw(ArrayIndexOutOfBounds)
{
  _size--;
}
  
void 
ParameterHandleValuePairSet::moveFrom(const ParameterHandleValuePairSet&,
				      ULong&)
  throw (ValueCountExceeded, ArrayIndexOutOfBounds,
  UnimplementedService)//CERTI
{
  throw UnimplementedService();
}
  
// Empty the Set without deallocating space.
void 
ParameterHandleValuePairSet::empty(void) 
{
  _head =(ParameterHandleValuePair *)NULL;
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

// $Id: GAV_aux.cc,v 3.0.2.1 2002/11/22 00:52:56 breholee Exp $
