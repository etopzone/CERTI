// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: LBTS.hh,v 3.4 2003/06/27 17:26:28 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_LBTS_HH
#define _CERTI_LBTS_HH

#include "RTItypes.hh"
#include "FederateClock.hh"

#include <deque>

namespace certi {

class LBTS : public std::deque<FederateClock *>
{
public:
    FederationTime _LBTS ;
    FederateHandle MyFederateNumber ;

    // ------------------------------
    // -- Constructor & Destructor --
    // ------------------------------
    LBTS();
    ~LBTS();

    // ---------------------
    // -- Insert & Delete --
    // ---------------------
    void insert(FederateHandle num_fed, FederationTime the_time);
    void remove(FederateHandle num_fed);

    // ---------------------
    // -- Update & Access --
    // ---------------------
    void update(FederateHandle num_fed, FederationTime the_time);
    void get(int i, FederateHandle &num_fed, FederationTime &the_time) const ;

    void exists(FederateHandle num_fed, Boolean &found, int &rank) const ;
    void compute();
};
}

#endif // _CERTI_LBTS_HH

// $Id: LBTS.hh,v 3.4 2003/06/27 17:26:28 breholee Exp $
