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
// $Id: NetworkMessage.hh,v 3.1 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_NETWORK_MESSAGE
#define _CERTI_NETWORK_MESSAGE

#include <stdio.h>
#include <cstring>
#include "baseTypes.hh"
#include "RTItypes.hh"

#include "PrettyDebug.hh"
#include "Socket.hh"
#include "network.hh"

#ifdef FEDERATION_USES_MULTICAST
#define MC_PORT 60123
#define ADRESSE_MULTICAST "224.0.0.251"
#endif // FEDERATION_USES_MULTICAST

#define PORT_TCP_RTIG "60400"
#define PORT_UDP_RTIG "60500"

namespace certi {

class NetworkMessage
{
public :

  // --------------------
  // -- Public Methods --
  // --------------------

  // Constructor & Destructor
  NetworkMessage();
  ~NetworkMessage();


  // Display method
  void display(const char *);

  // Parameter and Attribute Management

  // Remove the Parameter of rank 'Rank' in the ParamArray and its value in
  // ValueArray. If there are other parameter in the list, they are shifted
  // one rank back.
  // Ex: ParamArraySize = 3, 
  // ParamArray =[1, 2, 3], ValueArray =["Val1", "Val2", "Val3"]
  //->removeParameter(1)
  // ParamArraySize = 2,
  // ParamArray =[1, 3], ValueArray =["Val1", "Val3"]
  void removeParameter(UShort Rank);

  // See RemoveParameter for explanations.
  void removeAttribute(UShort Rank);

  // ----------------------------
  // -- Network Public Methods --(cf. NetworkMessage_RW.cc)
  // ----------------------------

  // Read and Write NetworkMessage Objects to and from Socket objects.

  void write(Socket *Socket)
    throw(NetworkError,
	  NetworkSignal);

  void read(Socket *Socket)
    throw(NetworkError,
	  NetworkSignal);

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

  // Return a newly allocated ValueArray, exactly of size AttribArraySize.
  // containing the actual Attribute values. You must FREE this structure.
  AttributeValue *getAttribValueArray();

  // Return a newly allocated ValueArray, exactly of size ParamArraySize,
  // containing the actual Parameter values. You must FREE this structure.
  ParameterValue *getParamValueArray();

  // -----------------------
  // -- Public Attributes --
  // -----------------------

  TypeNetworkMessage Type;
  TypeException Exception;

  UShort NumeroMessage;

  FederationHandle NumeroFederation;
  FederateHandle NumeroFedere;
  char NomFederation[MAX_FEDERATION_NAME_LENGTH + 1];
  char NomFedere[MAX_FEDERATE_NAME_LENGTH + 1];
 
  int BestEffortPeer;
  unsigned long BestEffortAddress;

  Boolean EstRegulateur;
  Boolean EstContraint;

  FederationTime Date;

  int NombreRegulateurs;
  unsigned long AdresseMulticast;

  char Label[MAX_USER_TAG_LENGTH + 1];

  ObjectHandlecount IDcount;
  ObjectHandle FirstID;
  ObjectHandle LastID;

  ObjectHandle Objectid;
  ObjectClassHandle objectClassHandle ;
  InteractionClassHandle InteractionHandle;

  EventRetractionHandle Retract;

  UShort HandleArraySize;
  AttributeHandle HandleArray[MAX_ATTRIBUTES_PER_CLASS];

  TransportType Transport;
  OrderType Order;

private:

  // ------------------------
  // -- Private Attributes --
  // ------------------------

  HeaderStruct Header;
  AttributeValue ValueArray[MAX_ATTRIBUTES_PER_CLASS];

  // ---------------------
  // -- Private Methods --(cf. NetworkMessage_RW.cc)
  // ---------------------

  // Read a Message Body from a Socket. Should be called after ReadHeader.
  void readBody(Socket *Socket);

  // Read a Header from a socket, and process it to read its content.
  // Return RTI_TRUE if the ReadBody Method has to be called.
  Boolean readHeader(Socket *Socket);

  // The message is written onto the socket by WriteHeader if no body
  // is required, or by WriteBody is a body has been required by WriteHeader.

  // Prepare and write a Body to a socket. Should be called after
  // WriteHeader.
  void writeBody(Socket *Socket);

  // Prepare and Write a Header to a Socket, and return RTI_TRUE
  // if the WriteBody method has to be called.
  Boolean writeHeader(Socket *Socket);

  // -- Others Private Read Methods --

  void readLabel(MessageBody *Body);
  void readFederationName(MessageBody *Body);
  void readFederateName(MessageBody *Body);

};

#define TAILLE_MSG_RESEAU sizeof(NetworkMessage)

}

#endif // _CERTI_NETWORK_MESSAGE

// $Id: NetworkMessage.hh,v 3.1 2002/12/11 00:47:33 breholee Exp $
