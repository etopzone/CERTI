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
// $Id: Objects.hh,v 3.1 2003/08/06 14:37:47 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef BILLARD_OBJECTS_HH
#define BILLARD_OBJECTS_HH

#include "Ball.hh"
#include "ColoredBall.hh"
#include "Fed.hh"

#include <RTI.hh>

#include <vector>
#include <string>

class Objects
{
public:
    Objects(RTIambassador &, Fed &, int, int);

    void create(std::string);
    void destroy(const FedTime &);
    void discover(ObjectHandle);
    void display();
    void erase();
    void init(int);
    void init(int, int);
    void receive(ObjectClassHandle, int, int);
    void remove(ObjectHandle);
    void reflect(ObjectHandle, int, int);
    void step(RTIfedTime &);
    void update(RTIfedTime &);

private:
    RTI::RTIambassador &rtiamb ;
    Fed &fedamb ;

    ColoredBall local ;
    std::vector<Ball> remote ;

    const int XMAX ;
    const int YMAX ;
};

#endif // BILLARD_OBJECTS_HH

// $Id: Objects.hh,v 3.1 2003/08/06 14:37:47 breholee Exp $
