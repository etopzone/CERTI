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
// $Id: network.hh,v 3.1 2002/12/11 00:47:34 breholee Exp $
// ---------------------------------------------------------------------------

// Structures utilisee pour les transfert des objets de la classe
// NetworkMessage a travers des Sockets.

#ifndef _CERTI_NETWORK_MESSAGE_HH
#define _CERTI_NETWORK_MESSAGE_HH

#include <cstring>

#include "RTItypes.hh" // Basic Types
#include "exception_type.hh" // Exceptions
#include "MessageBody.hh" // Body

namespace certi {

// Types de messages echanges entre RTIA et RTIG.
typedef enum {
  m_NOT_TYPED = 0, // Not used.
  m_CLOSE_CONNEXION,
  m_MESSAGE_NULL,
  m_CREATE_FEDERATION_EXECUTION,
  m_DESTROY_FEDERATION_EXECUTION,
  m_JOIN_FEDERATION_EXECUTION,
  m_RESIGN_FEDERATION_EXECUTION,
  m_SET_TIME_REGULATING,
  m_SET_TIME_CONSTRAINED,
  m_REQUEST_PAUSE,
  m_REQUEST_RESUME,
  m_PUBLISH_OBJECT_CLASS,
  m_UNPUBLISH_OBJECT_CLASS,
  m_PUBLISH_INTERACTION_CLASS,
  m_UNPUBLISH_INTERACTION_CLASS,
  m_SUBSCRIBE_OBJECT_CLASS,
  m_UNSUBSCRIBE_OBJECT_CLASS,
  m_SUBSCRIBE_INTERACTION_CLASS,
  m_UNSUBSCRIBE_INTERACTION_CLASS,
  m_TURN_INTERACTIONS_ON, // Uniquement RTIG->RTIA
  m_TURN_INTERACTIONS_OFF, // Uniquement RTIG->RTIA
  m_REQUEST_ID,
  m_REGISTER_OBJECT,
  m_DISCOVER_OBJECT, // Uniquement RTIG->RTIA
  m_UPDATE_ATTRIBUTE_VALUES,
  m_REFLECT_ATTRIBUTE_VALUES, // Uniquement RTIG->RTIA
  m_SEND_INTERACTION,
  m_RECEIVE_INTERACTION, // Uniquement RTIG->RTIA
  m_DELETE_OBJECT,
  m_REMOVE_OBJECT, // Uniquement RTIG->RTIA
  m_CHANGE_ATTRIBUTE_TRANSPORT_TYPE,
  m_CHANGE_ATTRIBUTE_ORDER_TYPE,
  m_CHANGE_INTERACTION_TRANSPORT_TYPE,
  m_CHANGE_INTERACTION_ORDER_TYPE,
  m_REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE,
  m_REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE,
  m_IS_ATTRIBUTE_OWNED_BY_FEDERATE,
  m_QUERY_ATTRIBUTE_OWNERSHIP,
  m_ATTRIBUTE_IS_NOT_OWNED,
  m_INFORM_ATTRIBUTE_OWNERSHIP,
  m_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
  m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION,
  m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION,
  m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION,
  m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE,
  m_ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE,
  m_UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
  m_ATTRIBUTE_OWNERSHIP_ACQUISITION,
  m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE,
  m_CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
  m_ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE,
  m_CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION,
  m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION
} TypeNetworkMessage;


// ------------
// -- EnTete --
// ------------

// L'entete est composee d'une partie fixe qui comprend :
// - Type de message reseau
// - Exception
// - NumeroFederation
// - NumeroFedere
// et d'une partie semi-variable, dont la taille est fixe, mais le
// contenu peut avoir differentes significations.
//
// Cette partie semi-variable est en fait une union des structures
// suivantes :
// - TimeStruct : Date + Boolean
// - ReqIDStruct : IDCount + ID + ID
// - T_O_Struct : ClassHandle + UShort(HandleArraySize) + TransportType
// + OrderType.
// - JoinStruct : Int(Nbre regulateurs) + Adresse Multicast
// - O_I_Struct : ClassHandle + UShort(HandleArraySize) + Date.
//
// L'entete, avec sa partie variable, doit suffire a transporter la plupart
// des messages reseaux sans corps.

// -- Structures de l'union de l'en-tete --

typedef struct {
  FederationTime Date; // Date, Logical Time
  Boolean R_or_C; // IsRegulator or IsConstrained
} TimeStruct;

typedef struct {
  ObjectHandlecount Count;
  ObjectHandle First;
  ObjectHandle Last;
} ReqIDStruct;

typedef struct {
  ObjectClassHandle Handle;
  UShort HandleArraySize;
  TransportType Transport;
  OrderType Order;
} T_O_Struct;

typedef struct {
  int NbReg;
  unsigned long AdrMC;
  unsigned long Addr;
  unsigned int Peer;
} JoinStruct;

typedef struct {
  ObjectClassHandle Handle;
  UShort Size;
  FederationTime Date;
} O_I_Struct;


typedef union {
  TimeStruct Time; // Les noms des variables n'ont pas d'importance
  ReqIDStruct ReqID; // mais ils doivent etre presents.
  T_O_Struct T_O;
  JoinStruct Join;
  O_I_Struct O_I;
} HeaderUnion;

// -- Structure de l'entete --

typedef struct {
  TypeNetworkMessage Type;
  TypeException Exception;
  FederationHandle NumeroFederation;
  FederateHandle NumeroFedere;
  UShort BodySize;
  HeaderUnion VP; // Variable Part
} HeaderStruct;

}

#endif // _CERTI_NETWORK_MESSAGE_HH

// $Id: network.hh,v 3.1 2002/12/11 00:47:34 breholee Exp $

