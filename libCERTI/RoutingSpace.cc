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
// $Id: RoutingSpace.cc,v 1.2 2003/02/19 18:07:30 breholee Exp $
// ----------------------------------------------------------------------------

#include "RoutingSpace.hh"

namespace certi {

RoutingSpace::RoutingSpace(void)
{

}

RoutingSpace::~RoutingSpace(void)
{
    vector<Dimension*>::iterator i ;
    for (i = dimensions.begin(); i != dimensions.end(); i++) {
        delete *i ;
    }
    dimensions.clear();
}

void
RoutingSpace::addDimension(Dimension* d)
{
    dimensions.push_back(d);
}

void
RoutingSpace::setHandle(SpaceHandle h)
{
    handle = h ;
}

void
RoutingSpace::display(void)
{
    cout << "RoutingSpace « " << name << " »" << endl ;
}

void
RoutingSpace::setName(string s)
{
    name = s ;
}

string
RoutingSpace::getName(void)
{
    return name ;
}

SpaceHandle
RoutingSpace::getHandle(void)
{
    return handle ;
}

} // namespace certi

// $Id: RoutingSpace.cc,v 1.2 2003/02/19 18:07:30 breholee Exp $
