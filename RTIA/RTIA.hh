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
// $Id: RTIA.hh,v 3.2 2003/01/16 17:55:33 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef RTIA_HH
#define RTIA_HH

#include <config.h>

#include <iostream>
using std::cout;
using std::endl;

#include "PrettyDebug.hh"

#include "FedParser.hh"

#include <sys/types.h>
#include <unistd.h>

#include <csignal>
#include <string>

#include "Communications.hh"
#include "TimeManagement.hh"
#include "FederationManagement.hh"
#include "ObjectManagement.hh"
#include "OwnershipManagement.hh"
#include "DeclarationManagement.hh"

namespace certi {
namespace rtia {

class RTIA
{
public:

  RTIA(void);
  ~RTIA(void);
  void execute(void);
 
private:
 
  FederationManagement *_GF;
  TimeManagement *_GT;
  Communications *_GC;
  Queues *_GQueues;
  ObjectManagement *_GO;
  OwnershipManagement *_GP; 
  DeclarationManagement *_GD;
  RootObject *_theRootObj;

  int nb_messages[18];
  int nb_requetes[52];
  int TOTAL;
  int nb_evenements;

  void processNetworkMessage(NetworkMessage*);
  void processFederateRequest(Message*);
  void chooseFederateProcessing(Message *req, Message &rep,	TypeException &e);
 
  void count(void);
};
}
}

#endif

// $Id: RTIA.hh,v 3.2 2003/01/16 17:55:33 breholee Exp $

