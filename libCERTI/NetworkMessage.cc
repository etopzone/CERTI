// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
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
// $Id: NetworkMessage.cc,v 3.2 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "NetworkMessage.hh"

namespace certi {

// -------------
// -- Affiche --
// -------------

void NetworkMessage::display(const char *s)
{
	printf("   --- MESSAGE RESEAU --- %s ---\n", s);
        printf("    Type             = %d\n",    Type);
        printf("    NumeroMessage    = %ld-%d\n", NumeroFedere, NumeroMessage);
        printf("    Date             = %f\n",    Date);
        printf("    Exception        = %d\n",    Exception);
        printf("    ObjectHandle     = %ld\n",    objectClassHandle);
        printf("    InteractionHandle= %ld\n",    InteractionHandle);
        printf("    Objectid         = %ld\n",    Objectid);
        printf("    HandleArraySize  = %d\n",    HandleArraySize);
}

// --------------------
// -- NetworkMessage --
// --------------------

NetworkMessage::NetworkMessage()
{
  Type      = m_NOT_TYPED;
  Exception = e_NO_EXCEPTION;

  NumeroMessage    = 0;

  NumeroFederation = 0;
  NumeroFedere     = 0;

  NomFederation [0] = '\0';
  NomFedere     [0] = '\0';
  Label         [0] = '\0';

  BestEffortPeer = -1 ;
  BestEffortAddress = 0 ;

  // Clear Header
  memset((void *) &Header, '\0', sizeof(Header));

  Date              = 0.0;

  Objectid          = 0;
  objectClassHandle = 0;
  InteractionHandle = 0;

  HandleArraySize   = 0;
}


// ---------------------
// -- ~NetworkMessage --
// ---------------------

NetworkMessage::~NetworkMessage()
{
}


// -------------------------
// -- GetAttribValueArray --
// -------------------------

AttributeValue *NetworkMessage::getAttribValueArray()
{
  int             i;
  AttributeValue *NewValueArray = NULL;

  NewValueArray =(AttributeValue *) calloc(HandleArraySize, 
					     sizeof(AttributeValue));

  if(NewValueArray == NULL)
    throw RTIinternalError("No memory.");

  for(i = 0; i <HandleArraySize; i++)
    getValue(i, NewValueArray [i]);

  return NewValueArray;
}  


// ------------------------
// -- GetParamValueArray --
// ------------------------

ParameterValue *NetworkMessage::getParamValueArray()
{
  int             i;
  ParameterValue *NewValueArray = NULL;

  NewValueArray =(ParameterValue *) calloc(HandleArraySize, 
					     sizeof(ParameterValue));

  if(NewValueArray == NULL)
    throw RTIinternalError("No memory.");

  for(i = 0; i < HandleArraySize; i++)
    getValue(i, NewValueArray [i]);

  return NewValueArray;
}  


// --------------
// -- GetValue --
// --------------

char *NetworkMessage::getValue(int Rank, char *Value)
    throw(RTIinternalError)
{
  // Pre-Checking

  if((Rank < 0) ||(Rank >= HandleArraySize))
    throw RTIinternalError("Bad Rank in Message.");

  // Getting Value

  if(Value != NULL) {
    strcpy(Value, ValueArray [Rank]);
    return NULL;
  } else
    return strdup(ValueArray [Rank]);

}


// ---------------------
// -- RemoveAttribute --
// ---------------------

void NetworkMessage::removeAttribute(UShort Rank)
{
  UShort AttribIndex = 0;

  if(Rank >= HandleArraySize)
    throw RTIinternalError("Bad Rank while removing message attribute.");

  // Shift Attribute Handles and Values
  for(AttribIndex = Rank; AttribIndex < HandleArraySize - 1; AttribIndex ++) {
    HandleArray [AttribIndex] = HandleArray [AttribIndex + 1];
    strcpy(ValueArray [AttribIndex], ValueArray [AttribIndex + 1]);
  }

  HandleArraySize --;
}


// ---------------------
// -- RemoveParameter --
// ---------------------

void NetworkMessage::removeParameter(UShort Rank)
{
  UShort ParamIndex = 0;

  if(Rank >= HandleArraySize)
    throw RTIinternalError("Bad Rank while removing message parameter.");

  // Shift Parameter Handles and Values
  for(ParamIndex = Rank; ParamIndex < HandleArraySize - 1; ParamIndex ++) {
    HandleArray [ParamIndex] = HandleArray [ParamIndex + 1];
    strcpy(ValueArray [ParamIndex], ValueArray [ParamIndex + 1]);
  }

  HandleArraySize --;
}


// --------------
// -- SetValue --
// --------------

void NetworkMessage::setValue(int Rank, const char *Value)
  throw(RTIinternalError)
{
  // Pre-Checking

  if((Value == NULL) ||(strlen(Value) > MAX_BYTES_PER_VALUE))
    throw RTIinternalError("Bad Value for message.");

  if((Rank < 0) ||(Rank >= HandleArraySize))
    throw RTIinternalError("Bad Rank for message.");

  // Setting Value

  strcpy(ValueArray [Rank], Value);
}

}

// $Id: NetworkMessage.cc,v 3.2 2002/12/11 00:47:33 breholee Exp $
