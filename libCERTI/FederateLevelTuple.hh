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
// $Id: FederateLevelTuple.hh,v 3.1 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_FEDERATE_LEVEL_TUPLE_HH
#define _CERTI_FEDERATE_LEVEL_TUPLE_HH

#include "RTItypes.hh"
#include "SecurityLevel.hh"

namespace certi {

class FederateLevelTuple 
{
public:
  FederateLevelTuple(FederateName, SecurityLevelID);
  ~FederateLevelTuple();
    
public:
  FederateName name ;
  SecurityLevelID id ; 
};
}

#endif // _CERTI_FEDERATE_LEVEL_TUPLE_HH

// $Id: FederateLevelTuple.hh,v 3.1 2002/12/11 00:47:33 breholee Exp $
