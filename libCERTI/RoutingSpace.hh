// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: RoutingSpace.hh,v 1.3 2003/03/12 10:07:01 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_ROUTING_SPACE
#define _CERTI_ROUTING_SPACE

#include "Dimension.hh"

#include <vector>
#include <string>
#include <iostream>
using namespace std ;

namespace certi {

class RoutingSpace
{
public:
    RoutingSpace(void);
    ~RoutingSpace(void);

    void setName(string);
    string getName(void);
    void setHandle(SpaceHandle);
    SpaceHandle getHandle(void);
    DimensionHandle getDimensionHandle(string) throw (NameNotFound);
    string getDimensionName(DimensionHandle) throw (DimensionNotDefined);

    void display(void);
    void addDimension(Dimension*);

private:
    SpaceHandle handle ;
    string name ;
    vector<Dimension*> dimensions ;
};

} // namespace certi

#endif // _CERTI_ROUTING_SPACE

// $Id: RoutingSpace.hh,v 1.3 2003/03/12 10:07:01 breholee Exp $

