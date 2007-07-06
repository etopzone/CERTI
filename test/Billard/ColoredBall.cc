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
// $Id: ColoredBall.cc,v 3.6 2007/07/06 09:25:19 erk Exp $
// ----------------------------------------------------------------------------

#include "ColoredBall.hh"

#include <config.h>


#include "graph_c.hh"

using namespace std ;

// ----------------------------------------------------------------------------
//! CBoule constructor.
ColoredBall::ColoredBall(RTI::ObjectHandle h) : Ball(h)
{
#ifndef X_DISPLAY_MISSING
    Color = RED ;
#endif
}

// ----------------------------------------------------------------------------
//! Displays the 'boule' on the right place in window.
void
ColoredBall::display()
{
#ifndef X_DISPLAY_MISSING
    cercler disque ;
    point centre ;

    centre.X = (int) x ;
    centre.Y = (int) y ;
    disque = Definecr(centre, (int) radius, COUL_UNIE, (couleur) Color);
    Drawcr(disque);
#endif
}

// $Id: ColoredBall.cc,v 3.6 2007/07/06 09:25:19 erk Exp $
