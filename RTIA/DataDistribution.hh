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
// $Id: DataDistribution.hh,v 3.1 2003/02/17 09:17:03 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_DATA_DISTRIBUTION
#define _CERTI_DATA_DISTRIBUTION

#include <config.h>
#include "RootObject.hh"

namespace certi {
namespace rtia {

class DataDistribution
{
public:
    DataDistribution(RootObject *);
  
    SpaceHandle getRoutingSpaceHandle(const char *);

private:
    RootObject *rootObject ;
};

}} // namespace certi/rtia

#endif // _CERTI_DATA_DISTRIBUTION

// $Id: DataDistribution.hh,v 3.1 2003/02/17 09:17:03 breholee Exp $
