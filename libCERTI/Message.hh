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
// $Id: Message.hh,v 3.0.2.1 2002/11/22 18:44:01 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_MESSAGE_HH
#define _CERTI_MESSAGE_HH

#include <stdio.h>
#include <cstring>
#include "baseTypes.hh"
#include "RTItypes.hh"
#include "SocketUN.hh"

#include "message_types.hh"
#include "PrettyDebug.hh"

namespace certi {

// Classe pour formaliser les messages qui vont etre
// echanges entre le RTI et le federe
class Message
{
  // Construction, Destruction
public:
  // --------------------
  // -- Public Methods --
  // --------------------

  // Constructor & Destructor
  Message();
  ~Message();

  // ----------------------------
  // -- Network Public Methods --(cf. Message_RW.cc)
  // ----------------------------

  // Read and Write NetworkMessage Objects to and from Socket objects.
  void write(SocketUN *Socket) throw(NetworkError, NetworkSignal);
  void read(SocketUN *Socket) throw(NetworkError, NetworkSignal);

  // ------------------------------
  // -- Attribute Access Methods --
  // ------------------------------

  // Value Array Management
  void setValue(int Rank, const char *Value)
    throw(RTIinternalError); // Bad Rank, Bad Value

  // If Value == NULL return a newly allocated copy of Value, else copy it
  // in Value.
  char *getValue(int Rank, char *Value = NULL)
    throw(RTIinternalError); // Bad Rank

  // Return a newly allocated ValueArray, exactly of size HandleArraySize.
  // containing the actual Attrib/Param values. You must FREE this structure.
  AttributeValue *getValueArray();

  // Strings Access
 
  // All the Set* methods check the length of the string, and throw
  // ValueLengthExceeded if the new string is too long.
 
  // All the Get* methods return a pointer on the actual stored value.
  // This pointer is READ-ONLY, and should not be de-allocated.

  char *getLabel() { return Label; };
  void setLabel(char *NewLabel);

  char *getName() { return Name; };
  void setName(const char *NewName);

  char *getFederationName() { return NomFederation; };
  void setFederationName(char *NewNomFederation);

  char *getFederateName() { return NomFedere; };
  void setFederateName(char *NewNomFedere);

  char *getTag() { return Tag; };
  void setTag(const char *NewTag);

  // -----------------------
  // -- Public Attributes --
  // -----------------------

  TypeService Type;
  FederationTime Date;
  Boolean Bool;
  FederationTimeDelta Lookahead;
  TypeException Exception;
  char RaisonException[MAX_EXCEPTION_REASON_LENGTH + 1];
  FederateHandle NumeroFedere;
  ResignAction RAction;
  UShort IDcount;
  ObjectHandle FirstID;
  ObjectHandle LastID;
  ObjectClassHandle objectClassHandle;
  InteractionClassHandle InteractionHandle;
  AttributeHandle AttribHandle;
  ParameterHandle ParamHandle;
  ObjectHandle Objectid;
  TransportType Transport;
  OrderType Order;
  EventRetractionHandle Retract;

  // used for both Attributes and Parameters arrays.
  UShort HandleArraySize;
  AttributeHandle HandleArray[MAX_ATTRIBUTES_PER_CLASS];

  Message& operator =(const Message&);

  void display(char *);

private:

  // ---------------------
  // -- Private Methods --(cf. Message_RW.cc)
  // ---------------------

  // Read a Message Body from a Socket. Should be called after
  // ReadHeader.
  void readBody(SocketUN *Socket);

  // Read a Header from a socket, and process it to read its
  // content.  Return RTI_TRUE if the ReadBody Method has to be
  // called.
  Boolean readHeader(SocketUN *Socket);

  // The message is written onto the socket by WriteHeader if no
  // body is required, or by WriteBody is a body has been required
  // by WriteHeader.

  // Prepare and write a Body to a socket. Should be called after
  // WriteHeader.
  void writeBody(SocketUN *Socket);

  // Prepare and Write a Header to a Socket, and return RTI_TRUE if
  // the WriteBody method has to be called.
  Boolean writeHeader(SocketUN *Socket);

  // -- Others Private Read Methods --

  void readHandleArray(MessageBody *Body);
  void readLabel(MessageBody *Body);
  void readName(MessageBody *Body);
  void readFederationName(MessageBody *Body);
  void readFederateName(MessageBody *Body);
  void readRAction(MessageBody *Body);
  void readTag(MessageBody *Body);
  void readValueArray(MessageBody *Body);

  // -- Others Private Write Methods --

  void writeHandleArray(MessageBody *Body);
  void writeRAction(MessageBody *Body);
  void writeValueArray(MessageBody *Body);

  // ------------------------
  // -- Private Attributes --
  // ------------------------

  MessageHeader Header;
  char Label[MAX_USER_TAG_LENGTH + 1];
  char Name[MAX_USER_TAG_LENGTH + 1];
  char NomFedere[MAX_FEDERATE_NAME_LENGTH + 1];
  char NomFederation[MAX_FEDERATION_NAME_LENGTH + 1];
  char Tag[MAX_USER_TAG_LENGTH + 1];
  AttributeValue ValueArray[MAX_ATTRIBUTES_PER_CLASS];
};
}

#endif // _CERTI_MESSAGE_HH

// $Id: Message.hh,v 3.0.2.1 2002/11/22 18:44:01 breholee Exp $

