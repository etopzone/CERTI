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
// $Id: RootObject.hh,v 3.1 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

// $Id: RootObject.hh,v 3.1 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_ROOT_OBJECT_HH
#define _CERTI_ROOT_OBJECT_HH

#include <stdio.h>

#include "ObjectClassSet.hh"
#include "InteractionSet.hh"
#include "SecurityServer.hh"

namespace certi {

class RootObject 
{

public:
  
  // -----------------------
  // -- Public Attributes --
  // -----------------------

  ObjectClassSet *ObjectClasses;
  InteractionSet *Interactions;

  // -------------------
  // -- Public Method --
  // -------------------

  // The SocketServer can be NULL on the RTIA.
  RootObject(SecurityServer *theSecurityServer);

  // Delete ObjectClasses and Interactions. 
  ~RootObject();

  // Print the Root Object tree to the standard output.
  void display(void);

  // ----------------------
  // -- Security Methods --
  // ----------------------

  // Return the LevelID of the level whose name is 'theLevelName' if
  // the security server is defined, else return PublicLevelID(on the RTIA).
  SecurityLevelID GetSecurityLevelID(SecurityLevelName theLevelName);

  void registerFederate(FederateName theFederate,
			SecurityLevelID theLevelID);

private:

  SecurityServer *server ;
};

}

#endif // _CERTI_ROOT_OBJECT_HH

// $Id: RootObject.hh,v 3.1 2002/12/11 00:47:33 breholee Exp $
