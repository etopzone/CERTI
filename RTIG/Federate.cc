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
// $Id: Federate.cc,v 3.1 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#include "Federate.hh"

namespace certi {
namespace rtig {

// ---------------------------------------------------------------------------
// Constructor

Federate::Federate(FederateName name_, FederateHandle handle_)
  throw(MemoryExhausted, RTIinternalError)
{
  if((name_ == NULL) || (handle_ == 0))
    throw RTIinternalError("Bad initialization param for Federate.");

  name = strdup(name_);
  if(name == NULL) throw MemoryExhausted("Unable to allocate Federate name.");

  handle = handle_;

  regulator = false ;
  constrained = false ;
}

// ---------------------------------------------------------------------------
// Destructor

Federate::~Federate()
{
  free(name);
}

// ---------------------------------------------------------------------------
// Get attributes

FederateHandle 
Federate::getHandle(void)
{
  return handle ;
}

FederateName
Federate::getName(void)
{
  return name ;
}

bool
Federate::isConstrained(void)
{
  return constrained ;
}

bool
Federate::isRegulator(void)
{
  return regulator ;
}

// ---------------------------------------------------------------------------
// Set attributes

void
Federate::setConstrained(bool c)
{
  constrained = c ;
}

void
Federate::setRegulator(bool r)
{
  regulator = r ;
}

}}

// $Id: Federate.cc,v 3.1 2002/12/11 00:47:33 breholee Exp $
