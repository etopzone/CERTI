// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: Billard.hh,v 3.3 2003/10/20 09:37:27 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_BILLARD_HH
#define CERTI_BILLARD_HH

#include "Fed.hh"
#include "Objects.hh"

#include <RTI.hh>

#include <string>

class Billard
{
public:
    Billard(std::string);

    void init(int);
    void init(int, int);
    virtual void declare();
    void join(std::string, std::string);
    void pause();
    void publishAndSubscribe();
    void resign();
    void setTimeRegulation(bool, bool);
    void setVerbose(bool flag) { verbose = flag ; }
    void step();
    void synchronize(int);
    void tick();

    FederateHandle getHandle() const ;

protected:
    RTIambassador rtiamb ;
    Fed fedamb ;
    Objects objects ;

    std::string federateName ;
    std::string federationName ;

    FederateHandle handle ;
    bool creator ;
    long nbTicks ;

    bool regulating ;
    bool constrained ;
    RTIfedTime localTime ;
    const RTIfedTime TIME_STEP ;

    const int XMAX ;
    const int YMAX ;

    bool verbose ;
};

#endif // CERTI_BILLARD_HH

// $Id: Billard.hh,v 3.3 2003/10/20 09:37:27 breholee Exp $
