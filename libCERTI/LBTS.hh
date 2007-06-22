// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003, 2004  ONERA
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
// $Id: LBTS.hh,v 3.7 2007/06/22 08:51:37 erk Exp $
// ----------------------------------------------------------------------------

#ifndef LIBCERTI_LBTS_HH
#define LIBCERTI_LBTS_HH

#include "certi.hh"

#ifndef _WIN32				//dotNet
#include <unistd.h>
#include <stdlib.h>
#endif

#include <map>
#include <vector>

namespace certi {

class CERTI_EXPORT LBTS
{
public:
    typedef std::pair<FederateHandle, FederationTime> FederateClock ;

    LBTS();
    ~LBTS();

    void compute();
    bool exists(FederateHandle) const ;
    void get(std::vector<FederateClock> &) const ;
    void insert(FederateHandle num_fed, FederationTime the_time);
    void remove(FederateHandle num_fed);
    void setFederate(FederateHandle handle) { MyFederateNumber = handle ; };
    size_t size() const { return clocks.size(); };
    void update(FederateHandle num_fed, FederationTime the_time);

protected:
    FederationTime _LBTS ;
    FederateHandle MyFederateNumber ;

private:
    typedef std::map<FederateHandle, FederationTime> ClockSet ;

    ClockSet clocks ;
};

}

#endif // LIBCERTI_LBTS_HH

// $Id: LBTS.hh,v 3.7 2007/06/22 08:51:37 erk Exp $
