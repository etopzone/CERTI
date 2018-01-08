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

#include "Billard.hh"
#include <RTI/RTI1516.h>

class BillardStaticDDM : public Billard {
public:
    BillardStaticDDM(std::string);
    virtual ~BillardStaticDDM() throw(rti1516e::FederateInternalError){};

    virtual void checkRegions();
    virtual void declare();
    virtual void publishAndSubscribe();
    virtual void resign();

protected:
    const int numberOfRegions;
    std::vector<rti1516e::Region*> sub;
    std::vector<rti1516e::Region*> pub;
    int subRegion;
    int pubRegion;
    rti1516e::SpaceHandle GeoID;
    rti1516e::DimensionHandle dimX;
    rti1516e::DimensionHandle dimY;
};

class BillardDynamicDDM : public Billard {
public:
    BillardDynamicDDM(std::string);
    virtual ~BillardDynamicDDM() throw(rti1516e::FederateInternalError){};

    virtual void checkRegions();
    virtual void declare();
    virtual void publishAndSubscribe();
    virtual void resign();

protected:
    rti1516e::Region* region;
    rti1516e::SpaceHandle GeoID;
    rti1516e::DimensionHandle dimX;
    rti1516e::DimensionHandle dimY;
};

#endif // CERTI_BILLARD_DDM_HH
