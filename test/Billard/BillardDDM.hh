// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: BillardDDM.hh,v 3.4 2003/12/01 16:41:54 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_BILLARD_DDM_HH
#define CERTI_BILLARD_DDM_HH

#include "Billard.hh"

#include <string>

class BillardDDM : public Billard
{
public:
    BillardDDM(std::string);
    virtual ~BillardDDM();

    virtual void declare();

protected:
    virtual void getHandles();

    int regionSize ;
    std::vector<std::vector<Region *> > regions ;

    SpaceHandle geo_id ;
    DimensionHandle dimx_id ;
    DimensionHandle dimy_id ;

    const char *geo_name ;
    const char *dimx_name ;
    const char *dimy_name ;
};

#endif // CERTI_BILLARD_DDM_HH

// $Id: BillardDDM.hh,v 3.4 2003/12/01 16:41:54 breholee Exp $
