// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: Publisher.hh,v 3.3 2003/01/15 12:07:46 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_PUBLISHER_HH
#define _CERTI_PUBLISHER_HH

#include <config.h>

#include "RTItypes.hh"

namespace certi {

//! Contains the federate handle of a publisher.
class Publisher
{
public:
    Publisher(FederateHandle);

    FederateHandle getHandle(void) const;

protected:    
    FederateHandle handle;
};
  
} // namespace certi

#endif // _CERTI_PUBLISHER_HH

// $Id: Publisher.hh,v 3.3 2003/01/15 12:07:46 breholee Exp $
