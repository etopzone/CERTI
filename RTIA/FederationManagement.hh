// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
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
// $Id: FederationManagement.hh,v 3.1 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef GF_HH
#define GF_HH

#include <config.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#include "TimeManagement.hh"
#include "PrettyDebug.hh"
#include "baseTypes.hh"
#include "RTItypes.hh"
#include "NetworkMessage.hh"
#include "Communications.hh"

namespace certi {
namespace rtia {

class TimeManagement;

class FederationManagement
{
public:

  // -----------------------
  // -- Public Attributes --
  // -----------------------

  TimeManagement *_GT;

  FederationHandle _numero_federation;
  FederateHandle federate;

  Boolean _fin_execution;

  // ----------------------------
  // -- Constructor/Destructor --
  // ----------------------------

  FederationManagement(Communications *GC);

  ~FederationManagement();

  // -----------------
  // -- RTI Methods --(Federation Management)
  // -----------------

  // -- Create/Destroy --

  void createFederationExecution(const FederationExecutionName theName,
			       TypeException &e);

  void destroyFederationExecution(const FederationExecutionName theName,
				TypeException &e);

  // -- Join/Resign --

  FederateHandle 
  joinFederationExecution(const FederateName Federate,
			const FederationExecutionName Federation,
			TypeException &e);

  void resignFederationExecution(ResignAction action,
			       TypeException &e);

  // -- Pause/Resume --
  void requestPause(const PauseLabel label, TypeException &e);
  void pauseAchieved(const PauseLabel label, TypeException &e);
  void requestResume(const PauseLabel label, TypeException &e);
  void resumeAchieved(TypeException &e);
  void initiatePause(const PauseLabel label);
  void initiateResume(const PauseLabel label);

private:

  // ------------------------
  // -- Private Attributes --
  // ------------------------

  Communications *_GC;

  Boolean _est_createur_federation;
  Boolean _est_membre_federation;
  Boolean _est_pause;

  char _nom_federation [MAX_FEDERATION_NAME_LENGTH];
  char _nom_federe [MAX_FEDERATE_NAME_LENGTH];
  char _label_pause [100];

  Boolean _RTI_demande_pause;
  Boolean _RTI_demande_fin_pause;
};

}}

#endif

// $Id: FederationManagement.hh,v 3.1 2002/12/11 00:47:33 breholee Exp $
