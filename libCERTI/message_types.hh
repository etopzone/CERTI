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
// $Id: message_types.hh,v 3.1 2002/12/11 00:47:34 breholee Exp $
// ---------------------------------------------------------------------------

// $Id: message_types.hh,v 3.1 2002/12/11 00:47:34 breholee Exp $
// ---------------------------------------------------------------------------
// Structures utilisee pour les transfert des objets de la classe
// Message a travers des Sockets UNIX.

#ifndef _CERTI_MESSAGE_TYPES_HH
#define _CERTI_MESSAGE_TYPES_HH

#include <cstring>

#include "RTItypes.hh" // Basic Types
#include "exception_type.hh" // Exceptions
#include "service_type.hh"
#include "MessageBody.hh" // Body

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
// - J_R_Struct : Numero de Federe(Join) et ResignAction(Resign)
// - O_I_Struct : ClassHandle + UShort(HandleArraySize) + Date.
//
// L'entete, avec sa partie variable, doit suffire a transporter la plupart
// des messages reseaux sans corps.

// -- Structures de l'union de l'en-tete --

namespace certi {

typedef struct {
  FederationTime Date; // Date, Logical Time, Lookahead, etc.
  Boolean mode ; // IsRegulator or IsConstrained
} MessageTimeStruct;

typedef struct {
  ObjectHandlecount Count;
  ObjectHandle First;
  ObjectHandle Last;
} MessageReqIDStruct;

typedef struct {
  ObjectClassHandle Handle;
  UShort Size;
  TransportType Transport;
  OrderType Order;
} MessageT_O_Struct;

typedef struct {
  FederateHandle NumeroFedere; // Join
  ResignAction Action; // Resign
} MessageJ_R_Struct;

typedef struct {
  ObjectClassHandle Handle;
  UShort Size;
  FederationTime Date;
} MessageO_I_Struct;


typedef union {
  MessageTimeStruct Time; 
  MessageReqIDStruct ReqID; 
  MessageT_O_Struct T_O;
  MessageJ_R_Struct J_R;
  MessageO_I_Struct O_I;
} MessageHeaderUnion;

// -- Structure de l'entete --

typedef struct {
  TypeService Type;
  TypeException Exception;
  UShort BodySize;
  MessageHeaderUnion VP; // Variable Part
} MessageHeader;

}

#endif // _CERTI_MESSAGE_TYPES_HH

// $Id: message_types.hh,v 3.1 2002/12/11 00:47:34 breholee Exp $

