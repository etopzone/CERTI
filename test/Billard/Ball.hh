// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: Ball.hh,v 3.5 2005/04/30 17:55:43 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_BALL_HH
#define CERTI_BALL_HH

#include "RTI.hh"

class Ball
{
public:
    Ball(RTI::ObjectHandle);

    float x, y ;
    float dx, dy ;
    float radius ;

    float olddx, olddy ;
    float color ;

    bool active ;

    unsigned long ID ; // object handle

    void display();
    void erase();
    void move();
    void setPosition(float, float);
    void setDirection(float, float);
    void checkBorderCollision(float, float);
    bool checkBallCollision(Ball *);

    void init(int);
    void init(int, int);
};

#endif // CERTI_BALL_HH

// $Id: Ball.hh,v 3.5 2005/04/30 17:55:43 breholee Exp $
