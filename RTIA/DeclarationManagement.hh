// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI
//
// CERTI is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// $Id: DeclarationManagement.hh,v 3.2 2003/01/16 17:55:33 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef GD_HH
#define GD_HH

#include <config.h>

#include "PrettyDebug.hh"

#include "FederationManagement.hh"
#include "Communications.hh"
#include "Files.hh"
#include "baseTypes.hh"
#include "RTItypes.hh"
#include "RootObject.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

namespace certi {
namespace rtia {

// Prototypes de classes existantes
class Communications;
class Queues;
class FederationManagement;

// Classe de gestion des declarations(publications et subscriptions).
class DeclarationManagement
{

public:

  // -----------------
  // -- Constructor --
  // -----------------

  DeclarationManagement(Communications *GC,
			Queues *GQueues,
			FederationManagement *GF,
			RootObject *theRootObj);

  ~DeclarationManagement(void);

  // -------------
  // -- Methods --(RTI Declaration Management)
  // -------------

  // -- Publish / Unpublish --

  // 3.1(1)
  void publishObjectClass(ObjectClassHandle theClassHandle, 
			  AttributeHandle *attribArray,
			  UShort attribArraySize,
			  TypeException &e);
 
  // 3.1(2)
  void unpublishObjectClass(ObjectClassHandle theClassHandle, 
			    TypeException &e);

  // 3.2(1)
  void publishInteractionClass(InteractionClassHandle theInteractionHandle,
			       TypeException &e);

  // 3.2(2)
  void unpublishInteractionClass(InteractionClassHandle theInteractionHandle,
				 TypeException &e);

  // -- Subscribe / Unsubscribe --

  // 3.3(1)
  void subscribeObjectClassAttribute(ObjectClassHandle theClassHandle, 
				     AttributeHandle *attribArray,
				     UShort attribArraySize,
				     TypeException &e);
 
  // 3.3(2)
  void subscribeObjectClassAttribute(ObjectClassHandle theClassHandle, 
				     AttributeHandle theAttribute, 
				     HLA_Region theRegion,
				     TypeException &e);

  // 3.3(3) 
  void unsubscribeObjectClassAttribute(ObjectClassHandle theClassHandle, 
				       TypeException &e);
 
  // 3.3(4)
  void unsubscribeObjectClassAttribute(ObjectClassHandle theClass, 
				       HLA_Region theRegion,
				       TypeException &e);

  // 3.4(1)
  void subscribeInteractionClass(InteractionClassHandle theClassHandle, 
				 TypeException &e);

  // 3.4(2)
  void subscribeInteractionClass(InteractionClassHandle theClass, 
				 HLA_Region theRegion,
				 TypeException &e);

  // 3.4(3) 
  void unsubscribeInteractionClass(InteractionClassHandle theClassHandle, 
				   TypeException &e);

  // 3.4(4)
  void unsubscribeInteractionClass(InteractionClassHandle theClass, 
				   HLA_Region theRegion,
				   TypeException &e);
 
  // -- Stop / Start Update & Generation --

  // 3.5(1)
  void startRegistrationForObjectClass(ObjectClassHandle theClass, 
				       TypeException &e);
 
  // 3.5(2)
  void stopRegistrationForObjectClass(ObjectClassHandle theClass, 
				      TypeException &e);

  // 3.6(1)
  void turnInteractionsOn(InteractionClassHandle theHandle, 
			  TypeException &e);


  // 3.6(2)
  void turnInteractionsOff(InteractionClassHandle theHandle, 
			   TypeException &e);

protected:

  // ------------------------
  // -- Private Attributes --
  // ------------------------

  Communications *_GC;
  Queues *_GQueues;
  FederationManagement *_GF;
  RootObject *_theRootObj;
};

}}

#endif

// $Id: DeclarationManagement.hh,v 3.2 2003/01/16 17:55:33 breholee Exp $


