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
// ----------------------------------------------------------------------------

#include "ObjectSet.hh"

namespace certi {

static pdCDebug D("OBJECTSET", "(ObjectSet) - ");

// ----------------------------------------------------------------------------
ObjectSet::ObjectSet(SecurityServer *the_server)
    : server(the_server)
{
}

// ----------------------------------------------------------------------------
ObjectSet::~ObjectSet(void)
{
    std::map<ObjectHandle, Object *>::iterator i ;

    for (i = begin(); i != end(); i++) {
        delete (*i).second ;
    }
    erase(begin(), end());
}

} // namespace certi

// $Id: ObjectSet.cc,v 1.1 2003/03/04 17:34:26 breholee Exp $