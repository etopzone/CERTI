// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2004-2005  ONERA
//
// This file is part of Billard
//
// Billard is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// Billard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
// ----------------------------------------------------------------------------

#ifndef CERTI_BILLARD_DDM_HH
#define CERTI_BILLARD_DDM_HH

#include "RTI.hh"
#include "Billard.hh"

struct Area {
    RTI::Region *region ;
    long x ;
    long y ;
    long size ;
};

class BillardDDM : public Billard
{
public:
    BillardDDM(std::string);
    virtual ~BillardDDM() throw (RTI::FederateInternalError) { };

    virtual void checkRegions();
    virtual void declare();
    virtual void publishAndSubscribe();
    virtual void resign();

protected:
    const int numberOfRegions ;
    std::vector<Area> areas ;
    int subRegion ;
    int pubRegion ;
    RTI::SpaceHandle GeoID ;
    RTI::DimensionHandle dimX ;
    RTI::DimensionHandle dimY ;
};

#endif // CERTI_BILLARD_DDM_HH
