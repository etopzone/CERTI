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
// $Id: Message.cc,v 3.1 2002/11/26 15:48:01 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "Message.hh"

namespace certi {

#define NONE -1

Message::Message()
{
  Date                = 0.0;
  Exception           = e_NO_EXCEPTION;
  RaisonException [0] = '\0';
  NomFedere [0]       = '\0';
  NomFederation [0]   = '\0';
  Tag [0]             = '\0';
  Name [0]            = '\0';
  Label [0]           = '\0';
  HandleArraySize     = 0;
}


// ---------------
// -- ~Message --
// ---------------

Message::~Message()
{
}


// --------------
// -- GetValue --
// --------------

char *Message::getValue(int Rank, char *Value)
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


// ------------------------
// -- GetValueArray --
// ------------------------

ParameterValue *Message::getValueArray()
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
// -- SetLabel --
// --------------

void Message::setLabel(char *NewLabel)
{
  if(strlen(NewLabel) > MAX_USER_TAG_LENGTH)
    throw ValueLengthExceeded("Label too long to fit in Message.");

  strcpy(Label, NewLabel);
}


// -------------
// -- SetName --
// -------------

void Message::setName(const char *NewName)
{
  if(strlen(NewName) > MAX_USER_TAG_LENGTH)
    throw ValueLengthExceeded("Name too long to fit in Message.");

  strcpy(Name, NewName);
}


// ----------------------
// -- SetNomFederation --
// ----------------------

void Message::setFederationName(char *NewNomFederation)
{
  if(strlen(NewNomFederation) > MAX_FEDERATION_NAME_LENGTH)
    throw ValueLengthExceeded("NomFederation too long to fit in Message.");

  strcpy(NomFederation, NewNomFederation);
}


// ----------------------
// -- SetNomFedere --
// ----------------------

void Message::setFederateName(char *NewNomFedere)
{
  if(strlen(NewNomFedere) > MAX_FEDERATE_NAME_LENGTH)
    throw ValueLengthExceeded("NomFedere too long to fit in Message.");

  strcpy(NomFedere, NewNomFedere);
}


// ------------
// -- SetTag --
// ------------

void Message::setTag(const char *NewTag)
{
  if(strlen(NewTag) > MAX_USER_TAG_LENGTH)
    throw ValueLengthExceeded("Tag too long to fit in Message.");

  strcpy(Tag, NewTag);
}


// --------------
// -- SetValue --
// --------------

void Message::setValue(int Rank, const char *Value)
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


Message& Message::operator =(const Message& msg)
{
  Type          = msg.Type;
  Date          = msg.Date;
  Bool          = msg.Bool;
  Lookahead     = msg.Lookahead;
  Exception     = msg.Exception;

  strcpy(RaisonException, msg.RaisonException);
  strcpy(NomFedere,       msg.NomFedere);
  strcpy(NomFederation,   msg.NomFederation);

  NumeroFedere  = msg.NumeroFedere;
  RAction       = msg.RAction;
  IDcount       = msg.IDcount;
  FirstID       = msg.FirstID;
  LastID        = msg.LastID;

  objectClassHandle  = msg.objectClassHandle ;
  InteractionHandle = msg.InteractionHandle;
  AttribHandle  = msg.AttribHandle;
  ParamHandle   = msg.ParamHandle;
  Objectid      = msg.Objectid;

  strcpy(Tag,  msg.Tag);
  strcpy(Name, msg.Name);

  Transport     = msg.Transport;
  Order         = msg.Order;

  Retract.theEventTime    = msg.Retract.theEventTime;
  Retract.theSerialNumber = msg.Retract.theSerialNumber;
  Retract.sendingFederate = msg.Retract.sendingFederate;

  strcpy(Label, msg.Label);

  HandleArraySize = msg.HandleArraySize;

  int i;
  for(i=0; i<HandleArraySize; i++)
    HandleArray[i] = msg.HandleArray[i];

  for(i=0; i<<HandleArraySize; i++)
    strcpy(ValueArray[i], msg.ValueArray[i]);

  return *this;
}

void Message::display(char *s)
{
  printf("   --- MESSAGE --- %s ---\n", s);
  printf("    Type=%d:\n", Type);
  printf("    Date=%f:\n", Date);
  printf("    Exception=%d:\n", Exception);
  printf("    ObjectHandle=%ld:\n", objectClassHandle);
  printf("    InteractionHandle=%ld:\n", InteractionHandle);
  printf("    AttribHandle=%ld:\n", AttribHandle);
  printf("    ParamHandle=%ld:\n", ParamHandle);
  printf("    Objectid=%ld:\n", Objectid);
  printf("    HandleArraySize=%d:\n", HandleArraySize);
}

}

// $Id: Message.cc,v 3.1 2002/11/26 15:48:01 breholee Exp $
