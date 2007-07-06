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
// $Id: Display.cc,v 3.5 2007/07/06 09:25:20 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>


#include "Display.hh"
#include "graph_c.hh"

#include <iostream>

Display *Display::myInstance = 0 ;

// ----------------------------------------------------------------------------
// setWindow
void
Display::setWindow(int xoffset, int yoffset)
{
    x = xoffset ;
    y = yoffset ;
}

// ----------------------------------------------------------------------------
// show
void
Display::show()
{
    std::cout << "Display(" << x << ", " << y << ", " << XMAX << ", " << YMAX ;
#ifndef X_DISPLAY_MISSING
    InitGraphe(x, y, XMAX, YMAX);
#endif
    std::cout << ")" << std::endl ;

}

// ----------------------------------------------------------------------------
// Display
Display::Display()
    : XMAX(500), YMAX(100), x(0), y(0) { }

// ----------------------------------------------------------------------------
// getHeight
int
Display::getHeight() const
{
    return YMAX ;
}

// ----------------------------------------------------------------------------
// instance
Display *
Display::instance()
{
    if (!myInstance) {
        myInstance = new Display();
    }
    return myInstance ;
}

// $Id: Display.cc,v 3.5 2007/07/06 09:25:20 erk Exp $
