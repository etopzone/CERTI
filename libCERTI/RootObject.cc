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
// $Id: RootObject.cc,v 3.2 2003/01/17 18:15:09 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "RootObject.hh"

namespace certi {

// --------------
// -- RootObject --
// --------------

RootObject::RootObject(SecurityServer *theSecurityServer)
  : server(theSecurityServer)
{
  ObjectClasses = new ObjectClassSet(server);

  if(ObjectClasses == NULL)
    throw RTIinternalError();

  Interactions  = new InteractionSet(server);
  
  if(Interactions == NULL)
    throw RTIinternalError();
}

  
// ---------------
// -- ~RootObject --
// ---------------

RootObject::~RootObject()
{
  delete ObjectClasses;
  delete Interactions;
}


// -------------
// -- Display --
// -------------

void RootObject::display(void)
{
  printf("\nRoot Object Tree :\n");

  ObjectClasses->display();

  Interactions->display();
}


// ------------------------
// -- GetSecurityLevelID --
// ------------------------

SecurityLevelID RootObject::GetSecurityLevelID(SecurityLevelName theLevelName)
{
  if(server != NULL)
    return server->getLevelIDWithName(theLevelName);
  else
    return PublicLevelID;
}

// ---------------------------------------------------------------------------
//! registerFederate.
void
RootObject::registerFederate(const char*     the_federate,
                             SecurityLevelID the_level_id)
{
    if(server != NULL)
        server->registerFederate(the_federate, the_level_id);
}

}

// $Id: RootObject.cc,v 3.2 2003/01/17 18:15:09 breholee Exp $
