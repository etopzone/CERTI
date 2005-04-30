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
// $Id: ColoredBall.hh,v 3.2 2005/04/30 17:55:43 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_COLORED_BALL_HH
#define CERTI_COLORED_BALL_HH

#include "Ball.hh"

class ColoredBall : public Ball
{
public:
    int Color ;

    ColoredBall(RTI::ObjectHandle);

    void display();
};

#endif // CERTI_COLORED_BALL_HH

// $Id: ColoredBall.hh,v 3.2 2005/04/30 17:55:43 breholee Exp $
